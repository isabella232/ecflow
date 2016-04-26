//============================================================================
// Copyright 2016 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#include "AttributeEditor.hpp"

#include "AttributeEditorFactory.hpp"
#include "ConnectState.hpp"
#include "VAttribute.hpp"
#include "VAttributeType.hpp"
#include "ServerHandler.hpp"
#include "UserMessage.hpp"
#include "VConfig.hpp"
#include "VRepeat.hpp"

#include <QPushButton>

#define _USE_MODELESS_ATTRIBUTEDITOR
#define _UI_ATTRIBUTEDITOR_DEBUG

#ifdef _USE_MODELESS_ATTRIBUTEDITOR
static QList<AttributeEditor*> editors;
#endif

AttributeEditor::AttributeEditor(VInfo_ptr info,QWidget* parent) : QDialog(parent), info_(info), form_(0)
{    
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

#ifdef _USE_MODELESS_ATTRIBUTEDITOR
    setModal(false);
#endif

    Q_ASSERT(info_ && info_->isAttribute() && info_->attribute());
    messageLabel_->hide();
    
    QString wt=windowTitle();
    wt+="  -  " + QString::fromStdString(VConfig::instance()->appLongName());
    setWindowTitle(wt);

    attachInfo();

    connect(buttonBox_,SIGNAL(clicked(QAbstractButton*)),
            this,SLOT(slotButton(QAbstractButton*)));
}

AttributeEditor::~AttributeEditor()
{
#ifdef _UI_ATTRIBUTEDITOR_DEBUG
    UserMessage::debug("AttributeEditor::~AttributeEditor -->");
#endif
    detachInfo();
#ifdef _USE_MODELESS_ATTRIBUTEDITOR
    editors.removeOne(this);
#endif
}    

void AttributeEditor::edit(VInfo_ptr info,QWidget *parent)
{
    Q_ASSERT(info && info->isAttribute() && info->attribute());
    VAttribute* a=info->attribute();
    Q_ASSERT(a->type());

#ifdef _USE_MODELESS_ATTRIBUTEDITOR
    Q_FOREACH(AttributeEditor* e,editors)
    {
        if((e->info_ && info) &&
           *(e->info_.get()) == *(info.get()))
        {
            e->raise();
            return;
        }
    }
#endif

    //For repeats we create an editor for each type
    std::string typeStr=a->type()->strName();
    if(a->type()->name() == "repeat")
    {
        typeStr+="_" + VRepeat::type(info->node());
    }

    //The edtior will be automatically deleted on close (Qt::WA_DeleteOnClose is set)
    if(AttributeEditor* e=AttributeEditorFactory::create(typeStr,info,0))
    {
#ifdef _USE_MODELESS_ATTRIBUTEDITOR
        editors << e;
        e->show();
#else
        e->exec();
#endif
    }
}

void AttributeEditor::addForm(QWidget* w)
{
    form_=w;
    mainLayout_->insertWidget(3,w,1);
}

void AttributeEditor::accept()
{
    apply();
    QDialog::accept();
}

void AttributeEditor::attachInfo()
{
#ifdef _UI_ATTRIBUTEDITOR_DEBUG
    UserMessage::debug("AttributeEditor::attachInfo -->");
#endif

    if(info_)
    {
        if(info_->server())
        {
            info_->server()->addNodeObserver(this);
            info_->server()->addServerObserver(this);
        }

        info_->addObserver(this);
    } 

#ifdef _UI_ATTRIBUTEDITOR_DEBUG
    UserMessage::debug("<-- AttributeEditor::attachInfo");
#endif
}

void AttributeEditor::detachInfo()
{
#ifdef _UI_ATTRIBUTEDITOR_DEBUG
    UserMessage::debug("AttributeEditor::detachInfo -->");
#endif
    if(info_)
    {
        if(info_->server())
        {
#ifdef _UI_ATTRIBUTEDITOR_DEBUG
            UserMessage::debug("   remove NodeObserver");
#endif
            info_->server()->removeNodeObserver(this);
#ifdef _UI_ATTRIBUTEDITOR_DEBUG
            UserMessage::debug("   remove ServerObserver");
#endif
            info_->server()->removeServerObserver(this);
        }
#ifdef _UI_ATTRIBUTEDITOR_DEBUG
            UserMessage::debug("   remove InfoObserver");
#endif
            info_->removeObserver(this);
    }
#ifdef _UI_ATTRIBUTEDITOR_DEBUG
    UserMessage::debug("<-- AttributeEditor::detachInfo");
#endif
}

void AttributeEditor::slotButton(QAbstractButton* b)
{
    if(b && buttonBox_->buttonRole(b) == QDialogButtonBox::ResetRole)
        resetValue();
}

void AttributeEditor::checkButtonStatus()
{
    setResetStatus(isValueChanged());
}

void AttributeEditor::setResetStatus(bool st)
{
    QPushButton *resetpb=buttonBox_->button(QDialogButtonBox::Reset);
    Q_ASSERT(resetpb);
    resetpb->setEnabled(st);
}

void AttributeEditor::setSuspended(bool st)
{
    Q_ASSERT(form_);
    form_->setEnabled(!st);

    QPushButton *okPb=buttonBox_->button(QDialogButtonBox::Ok);
    Q_ASSERT(okPb);
    okPb->setEnabled(!st);

    QPushButton *resetPb=buttonBox_->button(QDialogButtonBox::Reset);
    Q_ASSERT(resetPb);
    resetPb->setEnabled(!st);
}

void AttributeEditor::notifyDataLost(VInfo* info)
{
    if(info_ && info_.get() == info)
    {
        detachInfo();
        messageLabel_->showWarning("The parent node and the edited attribute <b>does not exist</b> anymore! Please close the dialog!");
        setSuspended(true);
    }
}

void AttributeEditor::notifyBeginNodeChange(const VNode* vn, const std::vector<ecf::Aspect::Type>& aspect,const VNodeChange&)
{
#ifdef _UI_ATTRIBUTEDITOR_DEBUG
    UserMessage::debug("AttributeEditor::notifyBeginNodeChange -->");
#endif
    if(info_ && info_->node() && info_->node() == vn)
    {
        bool attrNumCh=(std::find(aspect.begin(),aspect.end(),ecf::Aspect::ADD_REMOVE_ATTR) != aspect.end());
        if(attrNumCh)
        {
#ifdef _UI_ATTRIBUTEDITOR_DEBUG
    UserMessage::debug("   ADD_REMOVE_ATTR");
#endif
            VAttribute* a=info_->attribute();
            Q_ASSERT(a);
            if(!a->isValid(info_->node()))
            {
#ifdef _UI_ATTRIBUTEDITOR_DEBUG
                UserMessage::debug("   attribute does not exist");
#endif
                detachInfo();
                messageLabel_->showWarning("The edited attribute does not exist anymore! Please close the dialog!");
                setSuspended(true);
            }
        }
        else
        {
            //TODO: figure ot what to do when the edited attribute changed
        }
    }
#ifdef _UI_ATTRIBUTEDITOR_DEBUG
    UserMessage::debug("<-- AttributeEditor::notifyBeginNodeChange");
#endif
}

void AttributeEditor::notifyDefsChanged(ServerHandler* server, const std::vector<ecf::Aspect::Type>& a)
{
    if(info_ && info_->server() && info_->server() == server)
    {
        //TODO: implement

    }
}

void AttributeEditor::notifyServerDelete(ServerHandler* server)
{
    if(info_ && info_->server() == server)
    {
        detachInfo();
        messageLabel_->showWarning("Server <b>" + QString::fromStdString(server->name()) + "</b> was removed from ecFlowUI! The attribute does not exist anymore! Please close the dialog!");
        setSuspended(true);
    }
}
    
//This must be called at the beginning of a reset
void AttributeEditor::notifyBeginServerClear(ServerHandler* server)
{
    if(info_)
    {
        if(info_->server() && info_->server() == server)
        {
            messageLabel_->showWarning("Server <b>" + QString::fromStdString(server->name()) + "</b> is being reloaded. \
                   Until it is finished this attribute <b>cannot be modified</b>!");

            messageLabel_->startLoadLabel();

            setSuspended(true);
            checkButtonStatus();
        }
    }
}

//This must be called at the end of a reset
void AttributeEditor::notifyEndServerScan(ServerHandler* server)
{
    if(info_)
    {
        if(info_->server() && info_->server() == server)
        {
            messageLabel_->hide();
            messageLabel_->clear();

            //We try to ressurect the info. We have to do it explicitly because it is not guaranteed
            //that notifyEndServerScan() will be first called on the VInfo then on the InfoPanel. So it
            //is possible that the node exists but is still set to NULL in VInfo.
            info_->regainData();

            //If the node is not available dataLost() will be called.
            if(!info_->node())
                return;

            setSuspended(false);
        }
    }
}

void AttributeEditor::notifyServerConnectState(ServerHandler* server)
{
    Q_ASSERT(server);
    if(info_->server() && info_->server() == server)
    {
        switch(server->connectState()->state())
        {
        case ConnectState::Lost:
            messageLabel_->showWarning("Connection lost to server <b>" + QString::fromStdString(server->name()) + "</b>. \
                   Until it the connection regained the attribute <b>cannot be modified</b>!");
            setSuspended(true);
            break;
        case ConnectState::Normal:
            messageLabel_->hide();
            messageLabel_->clear();
            setSuspended(false);
            break;
        default:
            break;
        }
    }
}