//============================================================================
// Copyright 2009-2018 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "ServerLoadItemWidget.hpp"

#include <QHBoxLayout>

#include "Node.hpp"

#include "InfoProvider.hpp"

#ifdef ECFLOW_LOGVIEW
#include "LogLoadWidget.hpp"
#endif

#include "ServerHandler.hpp"
#include "UiLog.hpp"
#include "VNode.hpp"
#include "VNState.hpp"

ServerLoadItemWidget::ServerLoadItemWidget(QWidget *parent)
{
    QHBoxLayout* hb=new QHBoxLayout(this);
    hb->setContentsMargins(0,0,0,0);
#ifdef ECFLOW_LOGVIEW
    w_=new LogLoadWidget(this);
#else
    w_=new QWidget(this);
#endif
    hb->addWidget(w_);

    //This tab is always visible whatever node is selected!!!
    //We keep the data unchanged unless a new server is selected
    keepServerDataOnLoad_=true;

    //We will KEEP the contents when the item (aka tab) becomes unselected
    //unselectedFlags_.clear();
}

ServerLoadItemWidget::~ServerLoadItemWidget()
{
    clearContents();
}

QWidget* ServerLoadItemWidget::realWidget()
{
    return this;
}

void ServerLoadItemWidget::reload(VInfo_ptr info)
{
    assert(active_);

    if(suspended_)
        return;

    clearContents();

    bool same=hasSameContents(info);

    //set the info. We do not need to observe the node!!!
    info_=info;

    if(!same)
        load();
}

void ServerLoadItemWidget::load()
{
#ifdef ECFLOW_LOGVIEW
    if(info_ && info_->server())
    {
        ServerHandler *sh=info_->server();
        Q_ASSERT(sh);
        QString logFile;
        if(VServer* vs=sh->vRoot())
        {
            logFile=QString::fromStdString(vs->findVariable("ECF_LOG",false));

            w_->load(QString::fromStdString(sh->name()),
                         QString::fromStdString(sh->host()),
                         QString::fromStdString(sh->port()),
                         logFile,-50000); //last 50000 rows are read
        }
    }
#endif
}

void ServerLoadItemWidget::clearContents()
{
#ifdef ECFLOW_LOGVIEW
    w_->clear();
#endif
    InfoPanelItem::clear();
}

//We are independent of the server's state
void ServerLoadItemWidget::updateState(const FlagSet<ChangeFlag>& flags)
{
}

bool ServerLoadItemWidget::hasSameContents(VInfo_ptr info)
{
    if(info && info_ && info->server())
    {
        return info->server() == info_->server();
    }
    return false;
}

//We are independent of the server's state
void ServerLoadItemWidget::serverSyncFinished()
{
}

static InfoPanelItemMaker<ServerLoadItemWidget> maker1("server_load");
