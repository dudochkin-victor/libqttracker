/*
 * This file is part of LibQtTracker project
 *
 * Copyright (C) 2009, Nokia
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
/*
 * rdfservicebase_p.cpp
 *
 *  Created on: Aug 13, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include "rdfservicebase_p.h"
#include "liveresource_p.h"
#include "livenodemodelbridge_p.h"
#include "derived_columns_p.h"

namespace SopranoLive
{
	// TODO: on ABI break, remove this
	LiveNode RDFGraph::implLiveNode
			( LiveNode const &node, LiveNode const &type, bool strict
			, QList<QUrl> const &resource_contexts)
	{
		return static_cast<RDFService *>(this)->implLiveNode
				(node, type, strict, resource_contexts);
	}

	// TODO: on ABI break, make inline
	LiveNode RDFGraph::liveNode(LiveNode const &node, bool strict)
	{
		return implLiveNode(node, LiveNode(), strict);
	}

    LiveNodes RDFGraph::implLiveNodes(LiveNodeList const &urls, RDFVariable const &object_info,
                                      RDFStrategyFlags model_strategy, bool strict,
                                      QSharedPointer<QObject> const &parent)
    {
        return static_cast<RDFService *>(this)->implLiveNodes
            (urls, object_info, model_strategy, strict, parent);
    }

    LiveNodes RDFGraph::liveNodes(LiveNodeList const &urls, RDFVariable const &object_info,
                                  RDFStrategyFlags model_strategy,
                                  QSharedPointer<QObject> const &parent)
    {
        return implLiveNodes(urls, object_info, model_strategy, false, parent);
    }

    LiveNodes RDFGraph::strictLiveNodes(LiveNodeList const &urls, RDFVariable const &object_info,
                                        RDFStrategyFlags model_strategy,
                                        QSharedPointer<QObject> const &parent)
    {
        return implLiveNodes(urls, object_info, model_strategy, true, parent);
    }

	LiveNodes RDFGraph::modelResources
			( LiveNode const &type, RDFStrategyFlags strategy
			, QSharedPointer<QObject> const &parent)
	{
		RDFProperty resource_info(type, RDFVariable(), strategy, parent);
		return implModelResources(resource_info);
	}

	LiveNodes RDFGraph::liveResources
			( LiveNode const &type, RDFStrategyFlags strategy
			, QSharedPointer<QObject> const &parent)
	{
		RDFProperty resource_info
				( type, RDFVariable()
				, (strategy & ~RDFStrategy::AllowLive) | RDFStrategy::RequireLive, parent);
		return implModelResources(resource_info);
	}

	LiveNodes RDFGraph::implModelResources(RDFProperty &resource_info)
	{
		return static_cast<RDFService *>(this)->implModelResources(resource_info);
	}

	LiveNodes RDFGraph::modelVariable
			( RDFVariable const &var, RDFStrategyFlags model_strategy
			, QSharedPointer<QObject> const &parent)
	{
		return modelVariables(RDFVariableList() << var, model_strategy, parent);
	}

	LiveNodes RDFGraph::modelVariables
			( RDFVariableList const &variables, RDFStrategyFlags model_strategy
			, QSharedPointer<QObject> const &parent)
	{
		return static_cast<RDFService *>(this)->modelVariables(variables, model_strategy, parent);
	}

	void RDFGraph::removeResources(RDFVariable const &variable)
	{
		return static_cast<RDFService *>(this)->removeResources(variable);
	}

	// TODO: on ABI break, remove this and declaration
	LiveNode RDFService::liveNode(LiveNode const &node, LiveNode const &strict_type)
	{
		return implLiveNode(node, strict_type, strict_type.isValid() ? true : false);
	}

	LiveNodes RDFService::modelVariable
			( RDFVariable const &var, RDFStrategyFlags model_strategy
			, QSharedPointer<QObject> const &parent)
	{
		return modelVariables(RDFVariableList() << var, model_strategy, parent);
	}

	TripleUpdateContext::TripleUpdateContext
			( QWeakPointer<TripleUpdateReceiver> const &receiver, RDFStatement const &triple_mask
			, QModelIndex const &index)
		: receiver_(receiver), mode_(Inactive), triple_mask_(triple_mask), index_(index)
	{}

	TripleUpdateContext::~TripleUpdateContext()
	{
		index_ = QModelIndex();
	}

	ServiceContextData::ServiceContextData()
	{}

	RDFDerivedSelect ServiceContextData::select()
	{
		return select_prototype_;
	}

	RDFDerivedSelect ServiceContextData::select(RDFSelect const &copy)
	{
		return RDFDerivedSelect(select_prototype_).load(copy);
	}

	RDFUpdate ServiceContextData::update()
	{
		return update_prototype_;
	}

	RDFUpdate ServiceContextData::update(RDFUpdate const &copy)
	{
		return RDFUpdate(update_prototype_).setOperations(copy.operations());
	}

	RDFGraphImplBase::RDFGraphImplBase(ServiceContextDataPtr const &service_context_data)
		: discard_interface_modifications(false), interface_adds()
		, service_context_data_(service_context_data)
	{}

	LiveNode RDFGraphImplBase::wrapLiveNode(LiveNode const &node)
	{
		if(!node.isLive() || node->graph() == this)
			return node;
		return QSharedCreator<ResourceOfGraphBridge>::create
				(sharedFromThis<RDFGraphImplBase>(), node);
	}

	LiveNodes RDFGraphImplBase::wrapLiveNodes(LiveNodes const &model)
	{
		if(!model.isLive() || model->graph() == this)
			return model;
		return LiveNodes(QSharedCreator<LiveNodeModelOfGraphBridge>::create
				(sharedFromThis<RDFGraphImplBase>(), model.entity()));
	}

	LiveNodeModelBasePtr RDFGraphImplBase::wrapLiveNodeModelBase
			( LiveNodeModelBasePtr const &model)
	{
		if(model->graph() == this)
			return model;
		return QSharedCreator<LiveNodeModelBaseBridge>::create
				(sharedFromThis<RDFGraphImplBase>(), model);
	}

	LiveNode RDFGraphImplBase::implLiveNode
			( LiveNode const &node, LiveNode const &type, bool strict
			, QList<QUrl> const &resource_contexts)
	{
		LiveNode n(node);
		if(!n.isLive()
				|| (strict && !(n = n->graph()->implLiveNode
								(n, type, strict, resource_contexts)).isLive()))
			return node;
		if(node->graph() == this)
			return node;
		return wrapLiveNode(node);
	}

	LiveNodes RDFGraphImplBase::modelQuery
			( QString const &select, QStringList const &columns, RDFStrategyFlags model_strategy
			, QSharedPointer<QObject> const &parent)
	{	return modelQuery
				( select, columns
				, model_strategy | RDFStrategy::Running
				, parent, TripleModulePtr());
	}

	LiveNodeModelBasePtr RDFGraphImplBase::baseImplModelQuery
			( RDFDerivedSelect *select, RDFStrategyFlags model_strategy
			, QSharedPointer<QObject> const &parent)
	{
		return baseImplModelQuery(select, model_strategy, parent, TripleModulePtr());
	}

	LiveNodeModelBasePtr RDFGraphImplBase::implModelQuery
			( RDFDerivedSelect *select, RDFStrategyFlags model_strategy
			, QSharedPointer<QObject> const &parent)
	{
		return baseImplModelQuery
				( select
				, model_strategy | RDFStrategy::Running
				, parent, TripleModulePtr());
	}

	LiveNodes RDFGraphImplBase::modelQuery
			( RDFSelect const &select, RDFStrategyFlags model_strategy
			, QSharedPointer<QObject> const &parent)
	{
		RDFDerivedSelect dep_sel = service_context_data_->select(select);
		return LiveNodes(baseImplModelQuery
				( &dep_sel
				, model_strategy | RDFStrategy::Running
				, parent, TripleModulePtr()));
	}

	LiveNodes RDFGraphImplBase::modelVariable
			( RDFVariable const &variable, RDFStrategyFlags model_strategy
			, QSharedPointer<QObject> const &parent)
	{
		RDFDerivedSelect dep_sel = service_context_data_->select();
		dep_sel.addColumn(variable);
		return LiveNodes(baseImplModelQuery
				( &dep_sel
				, model_strategy | RDFStrategy::Running
				, parent, TripleModulePtr()));
	}

	LiveNodes RDFGraphImplBase::modelVariables
			( RDFVariableList const &variables, RDFStrategyFlags model_strategy
			, QSharedPointer<QObject> const &parent)
	{
		RDFDerivedSelect dep_sel = service_context_data_->select();
		dep_sel.addColumns(variables);
		return LiveNodes(baseImplModelQuery
				( &dep_sel
				, model_strategy | RDFStrategy::Running
				, parent, TripleModulePtr()));
	}

	RDFServiceImplBase::RDFServiceImplBase(ServiceContextDataPtr const &service_context_data)
		: RDFGraphImplBase(service_context_data)
	{}

	QString RDFServiceImplBase::rawQueryString(RDFQuery const &query) const
	{
		RDFUpdate const &real_update = query.toRDFUpdate();
		if(!real_update.isEmpty())
			return service_context_data_->update(real_update).getQuery();
		RDFSelect const &real_select = query.toRDFSelect();
		if(!real_select.isEmpty())
			return service_context_data_->select(real_select).getQuery();
		return QString();
	}

	QDebugHelper::QDebugHelper(QDebug const &cp) : QDebug(cp) {}
	QDebugHelper::~QDebugHelper() {}

	void RDFServiceImplBase::setVerbosity(int level)
	{
		SopranoLive::setVerbosity(level);
	}

}
