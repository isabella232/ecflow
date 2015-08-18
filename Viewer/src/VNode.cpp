//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#include "VNode.hpp"

#include "Node.hpp"
#include "Variable.hpp"

#include "ConnectState.hpp"
#include "ServerDefsAccess.hpp"
#include "ServerHandler.hpp"
#include "VAttribute.hpp"
#include "VFileInfo.hpp"
#include "VNState.hpp"
#include "VSState.hpp"

#include <boost/algorithm/string.hpp>

//=================================================
// VNode
//=================================================

VNode::VNode(VNode* parent,node_ptr node) :
    node_(node),
    parent_(parent),
    attrNum_(-1),
    cachedAttrNum_(-1)
{
	if(parent_)
		parent_->addChild(this);

	if(node_)
		node_->set_graphic_ptr(this);
}

ServerHandler* VNode::server() const
{
	return (parent_)?(parent_->server()):NULL;
}

bool VNode::isTopLevel() const
{
	return (parent_ && parent_->isServer());
	//return (node_)?(node_->isSuite() != NULL):false;
}

void VNode::clear()
{
	children_.clear();
	attrNum_=-1,
	cachedAttrNum_=-1;
}

bool VNode::hasAccessed() const
{
	return true; //!name_.empty();
}

//At the beginning of the update we get the current number of attributes
void VNode::beginUpdateAttrNum()
{
	//if(attrNum_ != -1)
	//{
	//	attrNum_=cachedAttrNum_;
	//}

	//attrNum_=VAttribute::totalNum(node_);
}

//At the end of the update we set the cached value to the current number of attributes
void VNode::endUpdateAttrNum()
{
	cachedAttrNum_=attrNum_;
	attrNum_=VAttribute::totalNum(this);
}

short VNode::cachedAttrNum() const
{
	return cachedAttrNum_;
}

short VNode::attrNum() const
{
	//If if was not initialised we get its value
	if(attrNum_==-1)
	{
		attrNum_=VAttribute::totalNum(this);

		if(cachedAttrNum_ == -1)
			cachedAttrNum_=attrNum_;
	}

	return attrNum_;
}

short VNode::currentAttrNum() const
{
	return VAttribute::totalNum(this);
}

QStringList VNode::getAttributeData(int row,VAttribute** type)
{
	QStringList lst;
	VAttribute::getData(this,row,type,lst);
	return lst;
}

VAttribute* VNode::getAttributeType(int row)
{
	VAttribute** type;
	VAttribute::getType(this,row,type);
	return *type;
}

void VNode::addChild(VNode* vn)
{
	children_.push_back(vn);
}

void VNode::removeChild(VNode* vn)
{
	std::vector<VNode*>::iterator it=std::find(children_.begin(),children_.end(),vn);
	if(it != children_.end())
	{
		children_.erase(it);
	}
}

VNode* VNode::childAt(int index) const
{
	return (index>=0 && index < children_.size())?children_.at(index):0;
}

int VNode::indexOfChild(const VNode* vn) const
{
	for(unsigned int i=0; i < children_.size(); i++)
	{
		if(children_.at(i) == vn)
			return i;
	}

	return -1;
}

int VNode::indexOfChild(node_ptr n) const
{
	for(unsigned int i=0; i < children_.size(); i++)
	{
		if(children_.at(i)->node() == n)
			return i;
	}

	return -1;
}

VNode *VNode::findChild(const std::string& name) const
{
	for(unsigned int i=0; i < children_.size(); i++)
	{
		if(children_.at(i)->sameName(name))
			return children_.at(i);
	}
	return 0;
}

void VNode::collect(std::vector<VNode*>& vec) const
{
	for(int i=0; i < numOfChildren(); i++)
	{
		vec.push_back(children_.at(i));
		children_.at(i)->collect(vec);
	}
}

VNode* VNode::find(const std::vector<std::string>& pathVec)
{
	if(pathVec.size() == 0)
		return this;

	if(pathVec.size() == 1)
	{
		return findChild(pathVec.at(0));
	}

	std::vector<std::string> rest(pathVec.begin()+1,pathVec.end());
	VNode *n = findChild(pathVec.at(0));

	return n?n->find(rest):NULL;
}

std::string VNode::genVariable(const std::string& key) const
{
    std::string val;
    if(node_ )
    	node_->findGenVariableValue(key,val);
    return val;
}

std::string VNode::findVariable(const std::string& key,bool substitute) const
{
	std::string val;
	if(!node_ )
	    return val;

	const Variable& var=node_->findVariable(key);
	if(!var.empty())
	{
		val=var.theValue();
		if(substitute)
		{
		    node_->variableSubsitution(val);
		}
		return val;
	}
	const Variable& gvar=node_->findGenVariable(key);
	if(!gvar.empty())
	{
	   val=gvar.theValue();
	   if(substitute)
	   {
		   node_->variableSubsitution(val);
	   }
	   return val;
	}

	return val;
}

std::string VNode::findInheritedVariable(const std::string& key,bool substitute) const
{
    std::string val;
    if(!node_ )
    	return val;

    const Variable& var=node_->findVariable(key);
    if(!var.empty())
    {
    	val=var.theValue();
    	if(substitute)
    	{
    		node_->variableSubsitution(val);
    	}
    	return val;
    }
    const Variable& gvar=node_->findGenVariable(key);
    if(!gvar.empty())
    {
    	val=gvar.theValue();
       	if(substitute)
       	{
       		node_->variableSubsitution(val);
       	}
       	return val;
    }

    //Try to find it in the parent
    if(parent())
    {
    	return parent()->findInheritedVariable(key,substitute);
    }

    return val;
}

int VNode::variablesNum() const
{
	if(node_.get())
		return static_cast<int>(node_->variables().size());

	return 0;
}

int VNode::genVariablesNum() const
{
	std::vector<Variable> gv;

	if(node_.get())
	{
		node_->gen_variables(gv);
		return static_cast<int>(gv.size());
	}

	return 0;
}

void VNode::variables(std::vector<Variable>& vars)
{
	vars.clear();
	if(node_.get())
		vars=node_->variables();
}

void VNode::genVariables(std::vector<Variable>& genVars)
{
	genVars.clear();
	if(node_.get())
		node_->gen_variables(genVars);
}

std::string VNode::absNodePath() const
{
	return (node_)?node_->absNodePath():"";
}

bool VNode::sameName(const std::string& name) const
{
	return(strName() == name)?true:false;
	//return (node_)?(node_->name() == name):false;
}

std::string VNode::strName() const
{
	if(node_ && node_.get())
		return node_->name();

	return std::string();
	/*
	if(name_.empty())
	{
		if(node_)
			name_=node_->name();
	}
	return name_;*/
}

QString VNode::name() const
{
	return QString::fromStdString(strName());
	//return (node_)?QString::fromStdString(node_->name()):QString();
}

QString VNode::stateName()
{
	return VNState::toName(this);
}

QString VNode::defaultStateName()
{
	return VNState::toDefaultStateName(this);
}

bool VNode::isSuspended() const
{
	return (node_ && node_->isSuspended());
}

QColor  VNode::stateColour() const
{
	return VNState::toColour(this);
}

QColor  VNode::stateFontColour() const
{
	return VNState::toFontColour(this);
}

LogServer_ptr VNode::logServer()
{
	LogServer_ptr lsv;

	if(!node_)
		return lsv;

	std::string logHost=findInheritedVariable("ECF_LOGHOST",true);
	std::string logPort=findInheritedVariable("ECF_LOGPORT");
	//if(logHost.empty())
	//{
	//	logHost=findInheritedVariable("LOGHOST",true);
	//	logPort=findInheritedVariable("LOGPORT");
	//}

	std::string micro=findInheritedVariable("ECF_MICRO");
	if(!logHost.empty() && !logPort.empty() &&
	  (micro.empty() || logHost.find(micro) ==  std::string::npos))
	{
		lsv=LogServer_ptr(new LogServer(logHost,logPort));
		return lsv;
	}

	return lsv;
}

bool VNode::isAncestor(const VNode* n)
{
	if(n == this)
		return true;

    VNode* nd=parent();
    while(nd)
    {
    	if(n == nd)
           return true;

    	nd=nd->parent();
    }
    return false;
}

std::vector<VNode*> VNode::ancestors(SortMode sortMode)
{
	std::vector<VNode*> nodes;

	VNode* n=this;

	nodes.push_back(n);
	n=n->parent();

	if(sortMode == ChildToParentSort)
	{
		while(n)
		{
			nodes.push_back(n);
			n=n->parent();
		}
	}

	else if (sortMode == ParentToChildSort)
	{
		while(n)
		{
			nodes.insert(nodes.begin(),n);
			n=n->parent();
		}
	}

	return nodes;
}

VNode* VNode::ancestorAt(int idx,SortMode sortMode)
{
	if(sortMode == ChildToParentSort &&  idx==0)
		return this;

	std::vector<VNode*> nodes=ancestors(sortMode);

	if(nodes.size() >= 0 && nodes.size() > idx)
	{
		return nodes.at(idx);
	}

	return NULL;
}

const std::string&  VNode::nodeType()
{
	static std::string suiteStr("suite");
	static std::string familyStr("family");
	static std::string taskStr("task");
	static std::string defaultStr("node");
	static std::string serverStr("server");

	if(isServer())
		return serverStr;

	node_ptr np=node();

	if(!np || !np.get())
		return defaultStr;

	if(np->isSuite())
		return suiteStr;
	else if(np->isFamily())
		return familyStr;
	else if(np->isTask())
			return taskStr;

	return defaultStr;
}

void VNode::why(std::vector<std::string>& theReasonWhy) const
{
	if(node_ && node_.get())
	{
		node_->bottom_up_why(theReasonWhy);
	}
}

/*
//Get the triggers list for the Triggers view
void VNode::triggers(TriggerList& tlr)
{
	//Check the node itself
	if(tlr.self())
	{
		if(node_ && !node_->isSuite())
		{
			std::set<VNode*> theSet;
			std::set<VNode*>::iterator sit;
			AstCollateXNodesVisitor astVisitor(theSet);

			if(node_->completeAst())
				node_->completeAst()->accept(astVisitor);

			if(node_->triggerAst())
				node_->triggerAst()->accept(astVisitor);

			for (sit = theSet.begin(); sit != theSet.end(); ++sit)
				tlr.next_node( *(*sit), 0, trigger_lister::normal, *sit);
     }

	 //Go through the attributes
     for(std::vector<VNode*>::iterator it=children_.begin(); it!= children_.end(); it++)
     {
        int type = node_->type();
        {
           ecf_concrete_node<InLimit const> *c =
                    dynamic_cast<ecf_concrete_node<InLimit const>*> (n->__node__());
           InLimit const * i = c ? c->get() : 0;
           if (i) {
              node *xn = 0;
              if ((xn = find_limit(i->pathToNode(), i->name())))
                 tlr.next_node(*xn,0,trigger_lister::normal,xn);
           }
        }
        if (type == NODE_DATE || type == NODE_TIME)
           tlr.next_node(*n,0,trigger_lister::normal,n);
     }
    }
  }

*/


//=================================================
//
// VNodeRoot - this represents the server
//
//=================================================

VServer::VServer(ServerHandler* server) :
	VNode(0,node_ptr()),
	server_(server),
	totalNum_(0)
{
}

VServer::~VServer()
{
	clear();
}


int VServer::totalNumOfTopLevel(VNode* n) const
{
	if(!n->isTopLevel())
		return -1;

	int idx=indexOfChild(n);
	if(idx != -1)
		return totalNumOfTopLevel(idx);

	return -1;
}

int VServer::totalNumOfTopLevel(int idx) const
{
	assert(totalNumInChild_.size() == children_.size());

	if(idx >=0 && idx < totalNumInChild_.size())
	{
		return totalNumInChild_.at(idx);
	}

	return -1;
}

//--------------------------------
// Clear
//--------------------------------

//Clear the whole contents
void VServer::clear()
{
	//Delete the children nodes. It will recursively delete all the nodes.
	for(std::vector<VNode*>::const_iterator it=children_.begin(); it != children_.end(); ++it)
	{
		deleteNode(*it);
	}

	//Clear the children vector
	children_.clear();

	totalNumInChild_.clear();

	//A sanity check
	assert(totalNum_ == 0);

	//Deallocate the nodes vector
	nodes_=std::vector<VNode*>();
}

//Clear the contents of a particular VNode
/*void VServer::clear(VNode* node)
{
	//Delete the children of node. It will recursively delete all the nodes
	//below this node
	for(unsigned int i=0; i < node->numOfChildren(); i++)
	{
		deleteNode(node->childAt(i));
	}

	//Clear the node contents
	node->clear();
}*/

//Delete a particular node
void VServer::deleteNode(VNode* node)
{
	for(unsigned int i=0; i < node->numOfChildren(); i++)
	{
		deleteNode(node->childAt(i));
	}

	delete node;
	totalNum_--;
}

//------------------------------------------
// Variables
//------------------------------------------

int VServer::variablesNum() const
{
	ServerDefsAccess defsAccess(server_);
	if(defsAccess.defs())
		return static_cast<int>(defsAccess.defs()->server().user_variables().size());

	return 0;
}

int VServer::genVariablesNum() const
{
	ServerDefsAccess defsAccess(server_);
	if(defsAccess.defs())
		return static_cast<int>(defsAccess.defs()->server().server_variables().size());

	return 0;
}

void VServer::variables(std::vector<Variable>& vars)
{
	vars.clear();
	ServerDefsAccess defsAccess(server_);
	if (defsAccess.defs())
		vars=defsAccess.defs()->server().user_variables();
}

void VServer::genVariables(std::vector<Variable>& vars)
{
	vars.clear();
	ServerDefsAccess defsAccess(server_);
	if (defsAccess.defs())
		vars=defsAccess.defs()->server().server_variables();
}

//------------------------------------------
// Find
//------------------------------------------

VNode* VServer::toVNode(const Node* nc) const
{
	return static_cast<VNode*>(nc->graphic_ptr());
}

VNode* VServer::find(const std::string& fullPath)
{
	if(fullPath.empty())
		return NULL;

	if(fullPath == "/")
		return this;

	std::vector<std::string> pathVec;
	boost::split(pathVec,fullPath,boost::is_any_of("/"));

	if(pathVec.size() > 0 && pathVec.at(0).empty())
	{
		pathVec.erase(pathVec.begin());
	}

	return VNode::find(pathVec);
}

std::string VServer::findVariable(const std::string& key,bool substitute) const
{
	std::string val;

	ServerDefsAccess defsAccess(server_);  // will reliquish its resources on destruction
	defs_ptr defs = defsAccess.defs();
	if (!defs)
		return val;

	const Variable& var=defs->server().findVariable(key);
    if(!var.empty())
    {
    	val=var.theValue();
    	if(substitute)
    	{
    		//defs->server().variableSubsitution(val);
    	}
    }
    return val;
}

std::string VServer::findInheritedVariable(const std::string& key,bool substitute) const
{
	return findVariable(key,substitute);
}

//----------------------------------------------
// Scan
//----------------------------------------------

//Clear the contents and get the number of children (suites)
//the server contain. At this point we do not build the tree.
void VServer::beginScan(VServerChange& change)
{
	//Clear the contents
	clear();

	//Get the Defs.
	{
		ServerDefsAccess defsAccess(server_);  // will reliquish its resources on destruction
		defs_ptr defs = defsAccess.defs();
		if (!defs)
			return;

		const std::vector<suite_ptr> &suites = defs->suiteVec();
		change.suiteNum_=suites.size();

		std::vector<node_ptr> nv;
		defs->get_all_nodes(nv);
		change.totalNum_=change.suiteNum_+nv.size();
	}

	//This will use ServerDefsAccess as well. So we have to be sure that t=the mutex is
	//released at this point.
	change.attrNum_=currentAttrNum();
}

//Build the whole tree.
void VServer::endScan()
{
	totalNum_=0;

	//Get the Defs
	{
		ServerDefsAccess defsAccess(server_);  // will reliquish its resources on destruction
		defs_ptr defs = defsAccess.defs();
		if (!defs)
			return;

		//Scan the suits.This will recursively scan all nodes in the tree.
		const std::vector<suite_ptr> &suites = defs->suiteVec();

		for(unsigned int i=0; i < suites.size();i++)
		{
			VNode* vn=new VNode(this,suites.at(i));
			totalNum_++;
			scan(vn);
		}
	}

	//This will use ServerDefsAccess as well. So we have to be sure that the mutex is
	//released at this point.
	endUpdateAttrNum();

	if(totalNum_ > 0)
	{
		nodes_.reserve(totalNum_);
		collect(nodes_);
	}
}

void VServer::scan(VNode *node)
{
	int prevTotalNum=totalNum_;

	std::vector<node_ptr> nodes;
	node->node()->immediateChildren(nodes);

	//totalNum_+=nodes.size();

	for(std::vector<node_ptr>::const_iterator it=nodes.begin(); it != nodes.end(); ++it)
	{
		VNode* vn=new VNode(node,*it);
		totalNum_++;
		scan(vn);
	}

	if(node->parent() == this)
	{
		 totalNumInChild_.push_back(totalNum_-prevTotalNum);
	}
}

VNode* VServer::nodeAt(int idx) const
{
	assert(idx>=0 && idx < nodes_.size());
	return nodes_.at(idx);
}

int VServer::indexOfNode(const VNode* vn) const
{
	for(int i=0; i < nodes_.size(); i++)
	{
		if(nodes_.at(i) == vn)
			return i;
	}
	return -1;
}

//----------------------------------------------
// Update
//----------------------------------------------

void VServer::beginUpdate(VNode* node,const std::vector<ecf::Aspect::Type>& aspect,VNodeChange& change)
{
	//NOTE: when this function is called the real node (Node) has already been updated. However the
	//views do not know about this change. So at this point (this is the begin step of the update)
	//all VNode functions have to return the values valid before the update happened!!!!!!!
	//The main goal of this function is to cleverly provide the views with some information about the nature of the update.

	//Update the generated variables. There is no notification about their change so we have to to do it!!!
	if(node->node())
	{
		Suite *s=NULL;
		s=node->node()->isSuite();
		if(!s)
		{
			s=node->node()->suite();
		}

		if(s && s->begun())
		{
			node->node()->update_generated_variables();
		}
	}

	bool attrNumCh=(std::find(aspect.begin(),aspect.end(),ecf::Aspect::ADD_REMOVE_ATTR) != aspect.end());
	bool nodeNumCh=(std::find(aspect.begin(),aspect.end(),ecf::Aspect::ADD_REMOVE_NODE) != aspect.end());

	//----------------------------------------------------------------------
	// The number of attributes changed but the number of nodes did not
	//----------------------------------------------------------------------

	if(attrNumCh && !nodeNumCh)
	{
		//The attributes were never used. None of the views have ever
		//wanted to display/access these attributes so far, so we can
		//just ignore this update!!
		if(!node->isAttrNumInitialised())
		{
			change.ignore_=true;
		}
		//Otherwise we just register the number of attributes before and after the update
		else
		{
			node->beginUpdateAttrNum();

			//This it the current number of attributes stored in the real Node. This call will not change the
			//the number of attributes (attrNum_ stored in the VNode!!!!)
			change.attrNum_=node->currentAttrNum();

			//this is the number of attributes before the update.
			change.cachedAttrNum_=node->cachedAttrNum();
		}

		return;
	}

	//---------------------------------------------------------------------------------
	// The number of nodes changed.
	//---------------------------------------------------------------------------------
	else if(nodeNumCh)
	{
		change.rescan_=true;
	}

	//In any other cases it is just a simple update (value or status changed)
}

//-------------------------------------------------------------------------------------------
// Finishes the update. It has to be consistent with the changes registered in VNodeChange.
// If anything does not match we return false that will call reset!!!
//-------------------------------------------------------------------------------------------

void VServer::endUpdate(VNode* node,const std::vector<ecf::Aspect::Type>& aspect,const VNodeChange& change)
{
	bool attrNumCh=(std::find(aspect.begin(),aspect.end(),ecf::Aspect::ADD_REMOVE_ATTR) != aspect.end());
	bool nodeNumCh=(std::find(aspect.begin(),aspect.end(),ecf::Aspect::ADD_REMOVE_NODE) != aspect.end());

	//--------------------------------------------------------------
	// The number of attributes changed but the number of nodes did not
	//-------------------------------------------------------------

	if(attrNumCh && ! nodeNumCh)
	{
		//This call updates the number of attributes stored in the VNode
		node->endUpdateAttrNum();
	}
}

void VServer::suites(std::vector<std::string>& sv)
{
	for(int i=0; i < numOfChildren(); i++)
	{
		sv.push_back(children_.at(i)->strName());
	}
}

std::string VServer::strName() const
{
	if(server_)
		return server_->name();

	return std::string();


	/*if(name_.empty())
	{
		if(server_)
			name_=server_->name();
	}
	return name_;*/
}

QString VServer::stateName()
{
	if(VSState::isRunningState(server_))
	{
		return VNState::toName(server_);
	}

	return VNState::toName(server_);
}

QString VServer::defaultStateName()
{
	return stateName();
}

bool VServer::isSuspended() const
{
	return false;
}

QColor  VServer::stateColour() const
{
	if(VSState::isRunningState(server_))
	{
		return VNState::toColour(server_);
	}

	return VSState::toColour(server_);
}

QColor  VServer::stateFontColour() const
{
	if(VSState::isRunningState(server_))
	{
		return VNState::toFontColour(server_);
	}

	return VSState::toFontColour(server_);
}

void VServer::why(std::vector<std::string>& theReasonWhy) const
{
	ServerDefsAccess defsAccess(server_);  // will reliquish its resources on destruction
	defs_ptr defs = defsAccess.defs();
	if (!defs)
		return;

	defs->why(theReasonWhy);
}

QString VServer::toolTip()
{
	QString txt="<b>Server</b>: " + QString::fromStdString(server_->name()) + "<br>";
	txt+="<b>Host</b>: " + QString::fromStdString(server_->host());
	txt+=" <b>Port</b>: " + QString::fromStdString(server_->port()) + "<br>";

	ConnectState* st=server_->connectState();

	if(server_->activity() == ServerHandler::LoadActivity)
	{
		txt+="<b>Server is being loaded!</b><br>";
		//txt+="<b>Started</b>: " + VFileInfo::formatDateAgo(st->lastConnectTime()) + "<br>";
	}
	else
	{
		if(st->state() == ConnectState::Normal)
		{
			txt+="<b>Server status</b>: " + VSState::toName(server_) + "<br>";
			txt+="<b>Status</b>: " + VNState::toName(server_) + "<br>";
			txt+="<b>Total number of nodes</b>: " +  QString::number(totalNum_);
		}
		else if(st->state() == ConnectState::Lost)
		{
			QColor colErr(255,0,0);
			txt+="<b><font color=" + colErr.name() +">Failed to connect to server!</b><br>";
			txt+="<b>Last connection</b>: " + VFileInfo::formatDateAgo(st->lastConnectTime()) + "<br>";
			txt+="<b>Last failed attempt</b>: " + VFileInfo::formatDateAgo(st->lastLostTime()) + "<br>";
			if(!st->errorMessage().empty())
				txt+="<b>Error message</b>:<br>" + QString::fromStdString(st->shortErrorMessage());
		}
		else if(st->state() == ConnectState::Disconnected)
		{
			QColor colErr(255,0,0);
			txt+="<b><font color=" + colErr.name() +">Server is disconnected!</b><br>";
			txt+="<b>Disconnected</b>: " + VFileInfo::formatDateAgo(st->lastDisconnectTime()) + "<br>";
		}
	}
	return txt;
}
