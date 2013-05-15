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
 * rdfcache_p.cpp
 *
 *  Created on: Apr 28, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include "rdfcache_p.h"
#include "updatermodule_p.h"
#include "util_p.h"
//#define RDFGRAPHCACHE_DEBUG

namespace SopranoLive
{
	ResourceOfGraphCache::ResourceOfGraphCache
			( QSharedPointer<RDFGraphCache> const &graph, LiveNode const &backend)
		: ResourceOfGraphBridge(graph, backend)
		, self_reference_(graph->cached_nodes_.insert(backend.toString(), this))
		, invalidated_backend_()
		, type_set_(graph->typeSetStorage())
	{
#ifdef RDFGRAPHCACHE_DEBUG
		debug(4, graph.data()) << "ResourceOfGraphCache ROFC(" << this << ") created:"
								<< this->node().toString();
#endif
	}

	ResourceOfGraphCache::~ResourceOfGraphCache()
	{
#ifdef RDFGRAPHCACHE_DEBUG
		debug(4, graph()) << "ResourceOfGraphCache ROFC(" << this << ") destroyed:"
								<< this->node().toString();
#endif
		invalidate();
		graphCache()->cached_nodes_.erase(self_reference_);
	}


	void ResourceOfGraphCache::invalidate()
	{
		if(backend_.isLive())
		{
			invalidated_backend_ = backend_.resource();
			resetBridge(backend().node());
		}
		// TODO: do we wipe the property cache as well? wut.
	}

	Node const &ResourceOfGraphCache::node() const
	{
		return this->backend_.node();
	}

	inline RDFGraphCache *ResourceOfGraphCache::graphCache() const
	{
		return static_cast<RDFGraphCache *>(graph_.data());
	}

	void ResourceOfGraphCache::insertCachedProperty
			( RawPropertyData const &property, QModelIndex const &sub_index) const
	{
#ifdef RDFGRAPHCACHE_DEBUG
		debug(4, graph()) << "Inserted cached property:" << node().toString()
						  << "->" << property.predicate()
						  << ":" << QString::number(property.strategy(), 16) << "@"
						  << sub_index.model()
						  << "{" << sub_index.row() << "," << sub_index.column() << "}";
#endif

		cached_properties_.insertMulti(property, sub_index);
	}

	TripleIndex ResourceOfGraphCache::findCachedProperty
			(RawPropertyData const &raw_property
			, RDFStrategyFlags strategy, RDFStrategyFlags strategy_mask) const
	{
		// first look for precise match
		CachedSubModels::iterator smi = cached_properties_.find(raw_property)
				, smiend = cached_properties_.end();

#ifdef RDFGRAPHCACHE_DEBUG
		debug(4, graph()) << "Searching cached property:" << node().toString()
						  << "->" << raw_property.predicate() << ":"
						  << QString::number(raw_property.strategy(), 16);
#endif

		while(smi != smiend && smi.key() == raw_property)
			if(!smi->model())
				smi = cached_properties_.erase(smi);
			else if(LiveNodeModel::matchStrategy(smi->strategy(), strategy, strategy_mask))
				return *smi;
			else
				++smi;

		// then look for a wildcard predicate supplier, based only on strategy
		{
			RawPropertyData catchall_property;
			smi = cached_properties_.find(catchall_property);
			while(smi != smiend && smi.key() == catchall_property)
				if(!smi->model())
					smi = cached_properties_.erase(smi);
				else
				{
					TripleIndex ret = smi->index(raw_property);
					TripleModel *model = ret.model();
					if(model && !((model->strategy(ret) ^ strategy) & strategy_mask))
					{
						if(strategy & RDFStrategy::Cached)
							insertCachedProperty(raw_property, ret);
						return ret;
					}
					++smi;
				}
		}
		return TripleIndex();
	}

	TripleModel *ResourceOfGraphCache::adoptTripleModelOrWrap(LiveNodeModel *livenodemodel) const
	{
		// if backend supplies us with valid TripleModel which can be adopted, use that
		TripleModel *ret = dynamic_cast<TripleModel *>(livenodemodel);
		if(!ret || !ret->adopt(graph()->sharedFromThis<RDFGraphImplBase>()))
		{
			// TODO, wrap the resulting container to our own TripleModel
			// and request updates through signalers registered to the caching system
			critical() << "while requesting property model with caching enabled, "
					   << graph()->service()->serviceIri().toString()
					   << " does not return an adoptable TripleModel.";
			return 0;
		}
		return ret;
	}

	LiveNodes ResourceOfGraphCache::types
			( RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent) const
	{
		return ResourceOfGraphBridge::types(strategy, parent);
	}

	int ResourceOfGraphCache::implHasProperties(RDFProperty &property_data) const
	{
		// TODO: extend cache lookup for other properties than type also
		if(property_data.predicate().metaValue().toString() == rdf_type_as_list_g[0]
			&& property_data.strategy() & RDFStrategy::Object)
		{
			LiveNode node = property_data.target().metaValue();
			if(!node.isResource())
			{
				if(property_data.strategy() & RDFStrategy::UseOnlyCache)
					return type_set_.size();
			}
			else if(type_set_.contains(node.toString()))
				return 1;
		}
		return ResourceOfGraphBridge::implHasProperties(property_data);
	}

	// implGetProperties(RDFDerivedSelect *, ..) defined next to baseImplModelQuery, see below
	LiveNodes ResourceOfGraphCache::implGetProperties
			(RDFProperty &property_data, RDFVariableList const &additional_columns) const
	{
		// performance code. Most of the property queries should end up here:
		// when no additional constraints, info or derived properties are
		// requested, we can simply check for a trivial property cache hit.
		if(!additional_columns.size())
			if(LiveNodeModelBasePtr ret = findCachedProperty
					(RawPropertyData(property_data), property_data.strategy(), RawStrategyMask)
						.childModel())
				return LiveNodes(ret);

		// otherwise fall back to the generic handler.
		return LiveResourceImplBase::implGetProperties(property_data, additional_columns);
	}

	LiveNode ResourceOfGraphCache::getObject(LiveNode const &predicate, CppType object_type) const
	{
		if(LiveNode node = firstObject(predicate, object_type))
			return node;
		return graph()->liveNode(ResourceOfGraphBridge::getObject(predicate, object_type));
	}
	LiveNode ResourceOfGraphCache::firstObject
			( LiveNode const &predicate, CppType object_type) const
	{
		TripleIndex index = findCachedProperty(RawPropertyData::fromObject(predicate.toString()));
		if(index.model())
			return index.model()->liveNode(index);
		return graph()->liveNode(ResourceOfGraphBridge::firstObject(predicate, object_type));
	}

	LiveNode ResourceOfGraphCache::getSubject
			( LiveNode const &predicate, CppType subject_type) const
	{
		if(LiveNode node = firstSubject(predicate, subject_type))
			return node;
		return graph()->liveNode(ResourceOfGraphBridge::getSubject(predicate, subject_type));
	}
	LiveNode ResourceOfGraphCache::firstSubject
			( LiveNode const &predicate, CppType subject_type) const
	{
		TripleIndex index = findCachedProperty
				(RawPropertyData::fromSubject(predicate.toString()));
		if(index.model())
			return index.model()->liveNode(index);
		return graph()->liveNode(ResourceOfGraphBridge::firstSubject(predicate, subject_type));
	}

	void ResourceOfGraphCache::remove() const
	{
		ResourceOfGraphBridge::remove();
		const_cast<ResourceOfGraphCache &>(*this).invalidate();
	}

	void ResourceOfGraphCache::attachInterface
			( QUrl const &interface_name, LiveResource &interface)
	{
		QString in = interface_name.toString();
		if(type_set_.contains(in))
			return;
		ResourceOfGraphBridge::attachInterface(interface_name, interface);
		type_set_ += in;
	}

/*
	 // TODO: implement
	void ResourceOfGraphCache::attachInterface
			( QUrl const &interface_name, LiveResource &interface)
	{
		LiveResource::addObject<rdf::type>(interface_name);
	}
*/
	RDFGraphCache::RDFGraphCache(QSharedPointer<RDFGraphImplBase> const &backend)
		: RDFGraphBridge(backend)
		, type_set_storage_(new SharedTypeSetStorage())
	{
#ifdef RDFGRAPHCACHE_DEBUG
		debug(2, backend_.data()) << "RDFGraphCache(" << this << ") created";
#endif
	}

	RDFGraphCache::~RDFGraphCache()
	{
#ifdef RDFGRAPHCACHE_DEBUG
		debug(2, backend_.data()) << "RDFGraphCache(" << this << ") destroyed";
#endif
	}

	LiveNodeModelPtr RDFGraphCache::preGetProperties
			( ResourceOfGraphCache const &resource, RDFDerivedSelect *columns)
	{
		// TODO: resource and columns unused
		Q_UNUSED(resource); Q_UNUSED(columns); return LiveNodeModelPtr();
	}
	LiveNodeModelPtr RDFGraphCache::postGetProperties
			( ResourceOfGraphCache const &resource, RDFDerivedSelect *columns)
	{
		Q_UNUSED(resource); Q_UNUSED(columns);
		// TODO handle spliced stuff
		return LiveNodeModelPtr();
	}


	LiveNode RDFGraphCache::createLiveNode(CppType type)
	{
		return newLiveResourceStraightFromBackEnd(backend().createLiveNode(type))
				->initialSharedFromThis<ResourceOfGraphCache>();
	}

	LiveNode RDFGraphCache::implLiveNode
			(LiveNode const &node, LiveNode const &type
			, bool strict, QList<QUrl> const &resource_contexts)
	{
		ResourceOfGraphCachePtr ret;

		LiveResource *node_res = node.entity().data();
		bool type_checked = false;

		if(!node_res || (node_res->graph() != this))
		{
			Nodes::iterator ni = cached_nodes_.find(rawStringFromNode(node));
			if(ni != cached_nodes_.end())
				ret = (*ni)->sharedFromThis<ResourceOfGraphCache>();
			else if(!node.isValid())
				return node;
			else
			{
				LiveNode backend_node = backend().implLiveNode
						(node, type, strict, resource_contexts);
				if(!backend_node)
					return backend_node;
				type_checked = true;
				ret = newLiveResourceStraightFromBackEnd(backend_node)
						->initialSharedFromThis<ResourceOfGraphCache>();
			}
		} else
		{
			ret = node_res->sharedFromThis<ResourceOfGraphCache>();

			// check if the node has been invalidated, denoted by backend_ being nonlive
			if(!ret->backend_.isLive())
			{
				// try to revalidate the backend
				if(!ret->resetBridge(backend().implLiveNode
								(ret->invalidated_backend_, type, strict, resource_contexts)))
					// backend rejected node revalidation
					return ret->backend_;
				type_checked = true;
				ret->invalidated_backend_.clear();
			}
		}

		// at this point, we know that ret exists on this graph

		if(type.isValid())
		{
			if(!type_checked)
			{
				// TODO: efficientlierize, add type cache checking here
				if(false)
				{
				} else
				if(!backend().implLiveNode(ret->backend_, type, strict, resource_contexts))
					return LiveNode();
			}
			// add type cache insertion here
		}

		if(node.type() == Node::ResourceNode)
			// TODO: fix hack
			const_cast<Node &>(ret->backend_.node()) = node.node();
		return ret;
	}

	LiveNodes RDFGraphCache::implLiveNodes(LiveNodeList const &urls, RDFVariable const &object_info,
					       RDFStrategyFlags model_strategy, bool strict,
					       QSharedPointer<QObject> const &parent)
	{
		if (model_strategy & (RDFStrategy::Windowed | RDFStrategy::Streaming | RDFStrategy::Writable | RDFStrategy::Live))
			warning() << "Windowed / Streaming / Writable / Live strategies not supported";

		RDFVariable var = object_info;
		var.isMemberOf(urls);

		if (model_strategy & RDFStrategy::BypassCache)
			goto do_the_query;

		{
			RDFDerivedSelect dep_sel = service_context_data_->select();
			dep_sel.addColumn(var);
			// expand the derived columns
			dep_sel.processDerivedColumns();

			// This is where we gather the data for the model
			QVector<QStringList> cached_data(urls.size());

			// The data will then be added to this model
			TripleModelPtr model = QSharedCreator<TripleModel>::create(sharedFromThis<RDFGraphCache>(), model_strategy);

			int base_col_count = dep_sel.baseColumnCount();
			int base_and_property_col_count = dep_sel.realColumnCount();
			RDFDerivedColumnList &cols = dep_sel.refColumns();
			// set the derivedselect as the columns of the model
			model->setColumnInformation(dep_sel);
			int row = 0;
			foreach (LiveNode const &url, urls)
			{
				QStringList cached_row;
				cached_row << service()->rawStringFromNode(url);
				// Handle the single-valued derived properties; the
				// multiple-valued properties are ignored (for now).
				for (int ci = base_col_count; ci < base_and_property_col_count; ++ci)
				{
					RDFDerivedColumn &spcinfo = cols[ci];
					int parent_col = spcinfo.parent();
					QString parent_val = cached_row[parent_col];
					Nodes::iterator ni = cached_nodes_.find(parent_val);
					if (ni == cached_nodes_.end())
						goto do_the_query;
					ResourceOfGraphCache *res = *ni;
					TripleIndex pix = res->findCachedProperty(spcinfo.propertyData());
					if (!pix.isValid())
						goto do_the_query;
					cached_row << pix.model()->data(pix).toString();
				}
				cached_data[row] = cached_row;
				++row;
			}
			// Attach the CacheModule, which now has information to expand
			// the rows.  Now we have only one primary column, and several
			// single-valued derived properties.
			if (model_strategy & RDFStrategy::Cached)
				model->changeTripleModule(
					QSharedCreator<OneMainColumnManyProperties>::create
							(sharedFromThis<RDFGraphCache>()));

			model->appendRowsImpl(RowStore(cached_data));

			// For now, skip the strictness / non-strictness handling, if everything is found in
			// the cache.
			return LiveNodes(model);
		}
do_the_query:
		if (!strict)
		{
			RDFUpdate up = service_context_data_->update();
			QVector<RDFProperty> deriveds = object_info.derivedProperties();
			foreach (RDFProperty const &prop, deriveds)
			{
				if (prop.strategy() & RDFStrategy::Object &&
				    prop.predicate().metaValue() == rdfs::type::iri() &&
				    prop.target().metaIsDefinite())
				{
					foreach (LiveNode const &url, urls)
						up.addInsertion(url, rdfs::type::iri(), prop.target().metaValue());
				}
			}
			executeQuery(up);
		}
		RDFDerivedSelect sel2 = service_context_data_->select();
		sel2.addColumn(var);
		return LiveNodes(baseImplModelQuery
				( &sel2
				, model_strategy | RDFStrategy::Running
				, parent, TripleModulePtr()));
	}

	ResourceOfGraphCache *RDFGraphCache::newLiveResource(Node const &node)
	{
		return newLiveResourceStraightFromBackEnd(backend().liveNode(node));
	}

	ResourceOfGraphCache *RDFGraphCache::newLiveResourceStraightFromBackEnd(LiveNode const &node)
	{
		return new ResourceOfGraphCache(sharedFromThis<RDFGraphCache>(), node);
	}

	LiveNode RDFGraphCache::wrapLiveNode(LiveNode const &node)
	{
		return implLiveNode(node);
	}

	LiveNodes RDFGraphCache::wrapLiveNodes(LiveNodes const &model)
	{
		if(!model.isLive()
			|| model->graph() == this
			|| model.entity().staticCast<LiveNodeModelBase>()
					->adopt(sharedFromThis<RDFGraphImplBase>()))
			return model;
		return LiveNodes(QSharedCreator<LiveNodeModelOfGraphBridge>::create
				(sharedFromThis<RDFGraphImplBase>(), model.entity()));
	}

	LiveNodeModelBasePtr RDFGraphCache::wrapLiveNodeModelBase(LiveNodeModelBasePtr const &model)
	{
		if(model->graph() == this
			|| model->adopt(sharedFromThis<RDFGraphImplBase>()))
			return model;
		return QSharedCreator<LiveNodeModelBaseBridge>::create(sharedFromThis<RDFGraphImplBase>(), model);
	}

		template<typename Strategy>
	QSharedPointer<Strategy> tryOuter(TripleModulePtr const &outer, RDFGraphCache *graphcache)
	{
		if(QSharedPointer<Strategy> outer_as_requested = outer.dynamicCast<Strategy>())
			return outer_as_requested;
		else if(outer)
			// TODO: implement chaining, scaffolding is in place already
			warning(graphcache) << "getProperties: model strategy chaining not implemented "
				"while supplying an external non-compatible triple_module";
		return QSharedPointer<Strategy>();
	}

	TripleModulePtr RDFGraphCache::preProcessQuery
			( RDFDerivedSelect *columns, RDFStrategyFlags *strategy
			, TripleModulePtr const &triple_module, RDFStrategyFlags *implementable_strategies
			, RawPropertyData *rpdata, LiveResource const *res)
	{

		// Check if we need derived column handling.
		// directly implement derived expanding and caching, without giving backend a shot
		TripleModulePtr outer_strat = triple_module;
		if((columns->processDerivedColumns
				(0, *implementable_strategies & RDFStrategy::ExpandDerivedProperties)
				|| columns->derivedColumnCount())
			&& (*implementable_strategies & (RDFStrategy::Live | RDFStrategy::Writable
						| RDFStrategy::Cached | RDFStrategy::ExposeDerivedProperties)))
		{
			if(rpdata)
			{
				if(!(outer_strat = tryOuter<PropertyOfSingleResourceModule>
						(triple_module, this)))
					outer_strat = QSharedCreator<PropertyOfSingleResourceModule>::create
							(sharedFromThis<RDFGraphCache>(), *rpdata, res->sharedFromThis());
			}
			else if(columns->baseColumnCount() == 1)
			{
				if(!(outer_strat = tryOuter<OneMainColumnManyProperties>
						(triple_module, this)))
					outer_strat = QSharedCreator<OneMainColumnManyProperties>::create
							(sharedFromThis<RDFGraphCache>());
			}
			else
			{
				if(!(outer_strat = tryOuter<ManyMainColumnManyProperties>
						(triple_module, this)))
					outer_strat = QSharedCreator<ManyMainColumnManyProperties>::create
							(sharedFromThis<RDFGraphCache>());
			}
			*implementable_strategies |= *strategy & RDFStrategy::Running;
			*strategy &= ~RDFStrategy::Running;
		} // else nothing to cache


		// If the columns have any order by column directives, we can implement
		// streaming, but let backend give it a shot first.
		if(*implementable_strategies & RDFStrategy::Streaming)
		{
			RDFSelect::OrderByInfo const obs = columns->getOrderBys();

			bool can_implement_streaming = true;

			// we can always implement streaming by default, unless some of the order by criteria
			// are not projected onto columns.

			for(RDFSelect::OrderByInfo::const_iterator cobi = obs.begin(), cobiend = obs.end()
					; cobi != cobiend; ++cobi)
				if(cobi->first < 0)
				{
					can_implement_streaming = false;
					break;
				}

			if(can_implement_streaming)
			// Let backend offer a more efficient, specific streaming first.
			// However, specify RequireStreaming so that if backend doesnt
			// offer streaming, it will return an empty model, so that UpdaterModule
			// can implement streaming by breaking the query into small new piece queries.
			{
				*implementable_strategies |= *strategy & RDFStrategy::Running;
				*strategy &= ~(RDFStrategy::StreamingMask | RDFStrategy::Running);
				*strategy |= RDFStrategy::RequireStreaming;
			}
			else
				// remove streaming from our strategies
				*implementable_strategies &= ~RDFStrategy::StreamingMask;
		}

		*implementable_strategies &= ~(RDFStrategy::ExpandDerivedProperties);
		*strategy &= ~(RDFStrategy::ExpandDerivedProperties | RDFStrategy::Cached);

		return outer_strat;
	}

	LiveNodeModelBasePtr ResourceOfGraphCache::implGetProperties
			(RDFDerivedSelect *columns, TripleModulePtr const &triple_module) const
	{
		// first column is the property that is requested
		RDFProperty &pdata = (*columns)[0].refPropertyData();

		RawPropertyData rpdata(pdata);

		RDFStrategyFlags strategy = pdata.strategy();
		// these are strategies we could enable
		RDFStrategyFlags implementable_strategies
				= strategy & RDFGraphCache::StrategiesImplementedByGraphCache;
		TripleModulePtr outer_strat = graphCache()->preProcessQuery
				(columns, &strategy, triple_module, &implementable_strategies, &rpdata, this);
		pdata.setStrategy(strategy);

		LiveNodeModelBasePtr ret;
		if( // if cache is not bypassed, try to find the property from there
			// TODO: add proper matching of requested derived properties and the like
			   (!(strategy & RDFStrategy::BypassCache)
				&& !outer_strat
				&& (ret = findCachedProperty(rpdata, strategy, RawStrategyMask).childModel()))
			// backend tries modeling
			|| (ret = backend().implGetProperties(columns, TripleModulePtr()))
			)
			// if we are caching or live, we require a TripleModule for managing the cache or
			// the triple updates. Also model needs to be adopted if it comes from the backend.
			// TODO: handle proper result typing, local custom filters also here
			return graphCache()->postProcessTripleModel
					(ret, strategy, implementable_strategies, outer_strat);

		// no model from cache nor from backend. Fail.
		warning(graph()) << "getProperties: model strategy"
						 << strategy << "not supported, failing";
		return LiveNodeModelBasePtr();
	}

	LiveNodeModelBasePtr RDFGraphCache::baseImplModelQuery
			( RDFDerivedSelect *select, RDFStrategyFlags strategy
			, QSharedPointer<QObject> const &parent, TripleModulePtr const &triple_module)
	{
		// these are strategies we could enable
		RDFStrategyFlags implementable_strategies
				= strategy & RDFGraphCache::StrategiesImplementedByGraphCache;
		TripleModulePtr outer_strat = preProcessQuery
				(select, &strategy, triple_module, &implementable_strategies);

		if(LiveNodeModelBasePtr ret = backend().baseImplModelQuery(select, strategy, parent))
			return postProcessTripleModel(ret, strategy, implementable_strategies, outer_strat);

		// no model from cache nor from backend. Fail.
		warning() << "modelQuery: model strategy" << strategy << "not supported, failing";
		return LiveNodeModelBasePtr();
	}

	LiveNodeModelBasePtr RDFGraphCache::postProcessTripleModel
			( LiveNodeModelBasePtr const &model, RDFStrategyFlags strategy
			, RDFStrategyFlags implementable_strategies, TripleModulePtr const &triple_strat)
	{
		// retrieve a valid triple model for containing the triple strategy.
		// if the model returned from cache or from backend can be directly used
		// and is compatible, use it, otherwise, wrap it within a proxy.
		LiveNodeModelBasePtr ret = wrapLiveNodeModelBase(model);

		TripleModel *triple_model = dynamic_cast<TripleModel *>(ret.data());
		if(triple_strat)
			triple_model->changeTripleModule(triple_strat);

		RDFStrategyFlags backend_strategies = ret->strategy();

		// Strategies the backend didn't implement and which we can implement.
		implementable_strategies ^= (backend_strategies & implementable_strategies);

		// RDFStrategy::Writable not implemented yet
		if(implementable_strategies & RDFStrategy::Writable)
		{
			if(!triple_model)
				warning() << "postProcessTripleModel: "
					"was not able to obtain a triple model for RDFStrategy::Writable";
			else
			{
				// If Writable without WriteBack and model comes from cache, a copy-on-write
				// schema should be used.
				implementable_strategies &= ~RDFStrategy::Writable;
				warning() << "postProcessTripleModel: "
						"RDFStrategy::Writable not implemented (yet).";
			}
		}

		if(RDFStrategyFlags enabled_flags = implementable_strategies
				& (RDFStrategy::Streaming | RDFStrategy::Windowed | RDFStrategy::Live))
		{
			if(!triple_model)
				warning() << "postProcessTripleModel: "
						"was not able to obtain a triple model for RDFStrategy::Streaming"
						" / RDFStrategy::Windowed / RDFStrategy::Live";
			else
			{
				triple_model->attachFrontModule(QSharedCreator<UpdaterModule>::create
						(enabled_flags));
				triple_model->enableStrategyFlags(enabled_flags);
			}
		}

		if(RDFStrategyFlags running = implementable_strategies & RDFStrategy::Running)
			model->enableStrategyFlags(running);

		if(RDFStrategyFlags req_streaming = strategy & RDFStrategy::StreamingMask)
			if(~backend_strategies & ~implementable_strategies & RDFStrategy::Streaming) {
				if(req_streaming == RDFStrategy::RequireStreaming)
					warning() << "modelQuery: cannot implement streaming: make sure the query contains at "
						"least one sorted column (ensure orderBy is specified after variable "
						"is associated with a column)";
				else
					debug(2) << "modelQuery: cannot implement streaming: make sure the query contains at "
						"least one sorted column (ensure orderBy is specified after variable "
						"is associated with a column)";
			}

		if(implementable_strategies & RDFStrategy::Cached && triple_model)
			triple_model->registerInto(*this);

		return ret;
	}

	QList<LiveNodes> RDFGraphCache::executeQuery
			(RDFQuery const &query, RDFStrategyFlags model_strategy
			, QSharedPointer<QObject> const &parent)
	{
		return RDFGraphBridge::executeQuery(query, model_strategy, parent);
	}

	CacheModule::CacheModule(RDFGraphImplBasePtr const &graph)
		: DerivedColumnModule(graph)
	{}

	CacheModule::ModulePtr CacheModule::attach(LiveNodeModelBase &model)
	{
		if(dynamic_cast<RDFGraphCache *>(model.graph()))
			return DerivedColumnModule::attach(model);
		warning(&model) << "Cannot attach to a model which doesnt belong to a RDFGraphCache";
		return ModulePtr();
	}

	void CacheModule::attached(TripleModel &triplemodel)
	{
		DerivedColumnModule::attached(triplemodel);
		single_item_columns_ = primeSingleItemColumns();

#ifdef RDFGRAPHCACHE_DEBUG
		debug(4, &triplemodel) << "Attaching TripleModel(" << &triplemodel << ") updates to "
			"CacheModule(" << this << ")";
#endif
		connect( &triplemodel, SIGNAL(rowsInserted(const QModelIndex &, int, int))
		       , this, SLOT(rowsInserted(const QModelIndex &, int, int)));
		connect( &triplemodel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &))
		       , this, SLOT(dataChanged(const QModelIndex &, const QModelIndex &)));
		connect( &triplemodel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int))
		       , this, SLOT(rowsAboutToBeRemoved(const QModelIndex &, int, int)));
		rowsInserted(QModelIndex(), 0, model()->row_store.size() - 1);
	}
	void CacheModule::detached(TripleModel &triplemodel)
	{
		rowsAboutToBeRemoved(QModelIndex(), 0, model()->row_store.size() - 1);
#ifdef RDFGRAPHCACHE_DEBUG
		debug(4, &triplemodel) << "Detaching TripleModel(" << &triplemodel << ") updates from "
				"CacheModule(" << this << ")";
#endif
		disconnect( &triplemodel, SIGNAL(rowsInserted(const QModelIndex &, int, int))
		          , this, SLOT(rowsInserted(const QModelIndex &, int, int)));
		disconnect( &triplemodel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &))
		       , this, SLOT(dataChanged(const QModelIndex &, const QModelIndex &)));
		disconnect(&triplemodel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int))
		          , this, SLOT(rowsAboutToBeRemoved(const QModelIndex &, int, int)));
		DerivedColumnModule::detached(triplemodel);
	}

	ResourceOfGraphCachePtr CacheModule::cachedResource(QString const &str, LiveNode const &node)
	{
		if(!graphCache())
			return ResourceOfGraphCachePtr();
		ResourceOfGraphCachePtr &cache_property_ = cached_resources_[str];
		if(!cache_property_)
			cache_property_ = graphCache()->liveNode(node)
					->sharedFromThis<ResourceOfGraphCache>();
		return cache_property_;
	}
	ResourceOfGraphCachePtr CacheModule::cachedResource(LiveNode const &node)
	{
		return cachedResource(node.toString(), node);
	}
	ResourceOfGraphCachePtr CacheModule::cachedResource(QString const &res_str, int col)
	{
		RDFGraphCache *gc = graphCache();
		if(!gc || !res_str.size())
			return ResourceOfGraphCachePtr();
		ResourceOfGraphCachePtr &cache_property_ = cached_resources_[res_str];
		if(!cache_property_)
			cache_property_ = gc->implLiveNode
					( gc->rawNodeFromString(res_str, (*columns_)[col].variable().varCppType())
					, LiveNode(), true
					, Contexts::NoChecks::iriList())
							->sharedFromThis<ResourceOfGraphCache>();
		return cache_property_;
	}

	LiveNodeModelBasePtr CacheModule::modelColumnVariable(RDFDerivedColumn &column_info)
	{
#ifdef RDFGRAPHCACHE_DEBUG
		debug(4, model()) << "modeling strategy" << column_info.strategy();
#endif
		return modelColumnVariableImpl(column_info
				, QSharedCreator<PropertyOfManyResourcesModule>::create
						(graph_, column_info.propertyData()));
	}

	struct InsertRowItam { ResourceOfGraphCachePtr res; QString str; int row; };

	void CacheModule::registerSingleItemWildcardEntries
			( SingleItemColumns const &cols, int first_row, int last_row)
	{
		QVector<int>::const_iterator cciend = cols.end();
		int row = first_row;
		for(RowStore::iterator cur_i = rows_->begin() + first_row
					, end_i = (rows_->begin() + (last_row + 1)).skipGap()
				; cur_i != end_i; row = cur_i.skipGap(1).key())
		{
			int row_length = cur_i->size();
			for(QVector<int>::const_iterator col_i = cols.begin(); col_i != cciend; ++col_i)
				if(*col_i >= row_length)
					break;
				else if(ResourceOfGraphCachePtr res = cachedResource((*cur_i)[*col_i], *col_i))
				{
					res->insertCachedProperty
							( RawPropertyData(), createIndex(row, *col_i, RDFStrategyFlags()));
					res->type_set_ |= model()->column_types_[*col_i];
				}
		}
	}

	CacheModule::SingleItemColumns CacheModule::primeSingleItemColumns()
	{
		// by default, register all real columns as single item columns.
		SingleItemColumns ret;
		for(int i = 0; i < columns_->realColumnCount(); ++i)
			ret << i;
		return ret;
	}

	void CacheModule::rowsInserted(const QModelIndex &parent, int first, int last)
	{
		// TODO: parent unused
		Q_UNUSED(parent);
		if(!columns_->size())
			return;

		registerSingleItemWildcardEntries(single_item_columns_, first, last);
		/*
		int base_col_count = columns_->baseColumnCount();
		int base_and_property_col_count = columns_->realColumnCount();
		RDFDerivedColumnList &cols = (*columns_).refColumns();
		if(base_col_count != base_and_property_col_count)
		{
			QVector<InsertRowItam> res_cache(base_and_property_col_count);
			QString pstr;

			RowStore::iterator cur_i = rows_->begin() + first
					, end_i = (rows_->begin() + (last + 1)).skipGap();

			unsigned row;
			for(; cur_i != end_i; row = cur_i.skipGap(1).key())
				for(int ci = base_col_count, ciend = std::min<int>(base_and_property_col_count
						, cur_i->size()); ci < ciend; )
				{
					RDFDerivedColumn &spcinfo = cols[ci];
					int parent_index = spcinfo.parent();
					RDFDerivedColumn &parent_info = cols[parent_index];
					InsertRowItam &parent = res_cache[parent_index];
					if(parent.str != (pstr = (*cur_i)[parent_index]))
					{
						parent.str = pstr;
						parent.row = row;
						if((parent.res = cachedResource(pstr)))
							for(int spciend = std::min<int>
									(ci + parent_info.derivedColumnCount(), ciend)
								; ci < spciend; ++ci)
							{
								RDFDerivedColumn &cinfo = cols[ci];
								// TODO: improve? currently all properties of all given resources
								// are directly registered
								parent.res->insertCachedProperty(RawPropertyData
										// cinfo.strategy must be replaced eventually with model
										// parent information
										(cinfo.propertyData()), createIndex
												(cur_i.key(), ci, cinfo.strategy()));
							}
					} else
						ci += parent_info.derivedColumnCount();
				}
		}
		*/
	}

	void CacheModule::dataChanged(QModelIndex const &top_left, QModelIndex const &bottom_right)
	{
		CacheModule::rowsInserted(QModelIndex(), top_left.row(), bottom_right.row());
	}

	void CacheModule::rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
	{
		Q_UNUSED(last);
		warning(model()) << model()->index(first, -1, parent)
						 << "not implemented";
	}

	void CacheModule::mainModelUpdated()
	{
		warning(model()) << "not implemented";
	}

	void CacheModule::cachedModelUpdated()
	{
		warning(model()) << "not implemented";
	}

	OneMainColumnManyProperties::OneMainColumnManyProperties(RDFGraphImplBasePtr const &graph)
		: CacheModule(graph)
	{}

	void OneMainColumnManyProperties::registerInto(RDFGraphCache &cache)
	{
		// TODO: unused param
		Q_UNUSED(cache);
	}

	ManyMainColumnManyProperties::ManyMainColumnManyProperties(RDFGraphImplBasePtr const &graph)
		: CacheModule(graph)
	{}

	void ManyMainColumnManyProperties::registerInto(RDFGraphCache &cache)
	{
		// TODO: cache unused
		Q_UNUSED(cache);
	}


	PropertyOfSingleResourceModule::PropertyOfSingleResourceModule
			( RDFGraphImplBasePtr const &graph, RawPropertyData const &property_data
			, LiveResourceConstPtr const &property)
		: OneMainColumnManyProperties(graph), property_data_(property_data), property_(property)
	{}

	void PropertyOfSingleResourceModule::registerInto(RDFGraphCache &cache)
	{
		// TODO: cache unused
		Q_UNUSED(cache);
		if(ResourceOfGraphCachePtr cachedres = cachedResource
				(property_.constCast<LiveResource>()))
			cachedres->insertCachedProperty(property_data_, model()->wholeIndex());
	}

	QModelIndex PropertyOfSingleResourceModule::index
			(RawTripleData const &triple, QModelIndex const &current) const
	{
		if(RawPropertyData prop_cand = triple.toRawPropertyData())
		{
			bool is_valid = current.model();
			if((!is_valid || model()->isWholeIndex(current)) && prop_cand == property_data_)
				return model()->wholeIndex();
			if(is_valid)
				return index(prop_cand, current);
		}
		// TODO: handle other columns and the generic triple access form.
		// Currently relying on index(RawPropertyData) handling property access
		return QModelIndex();
	}

	bool PropertyOfSingleResourceModule::tripleUpdates_chain
			( RawTriplesData const &triples, TripleUpdateContextConstPtr const &context
			, InterfaceChain<TripleUpdateReceiver> chain)
	{
		do
		{
			if(triples.mask() & TripleUpdateContext::PartialUpdate)
				break;
			QStringList property, items;
			if(property_data_.strategy() & RDFStrategy::Subject)
			{
				property = triples.objects();
				items = triples.subjects();
			}
			else
			{
				property = triples.subjects();
				items = triples.objects();
			}
			if(!property.contains(property_->node().toString())
					|| !triples.predicates().contains(property_data_.predicate()))
				break;
			RDFStrategyFlags mode = triples.mask() & TripleUpdateContext::AddsRemoves;
			if(mode == TripleUpdateContext::Adds)
				addSetItems(items, model());
			else if(mode == TripleUpdateContext::Removes)
				removeSetItems(items, model());
			else
				break;
			return true;
		} while(0);

		return TripleUpdateReceiver::tripleUpdates_chain(triples, context, chain);
	}

	PropertyOfManyResourcesModule::PropertyOfManyResourcesModule
			(RDFGraphImplBasePtr const &graph, RawPropertyData const &property_data)
		: ManyMainColumnManyProperties(graph), property_data_(property_data)
		, ascending_primary_(true)
	{}


	int PropertyOfManyResourcesModule::searchResource(QString const &res) const
	{
		int low = 0;
		int high = rows_->size();
		RowStore::const_iterator cur_i = rows_->constBegin();
		int old_mid = low;
		while(low != high)
		{
			int mid = ((low + high) >> 1);
			cur_i += mid - old_mid;
 			if(ascending_primary_
 					? QString::compare(res, (*cur_i)[0]) > 0
					: QString::compare(res, (*cur_i)[0]) < 0)
				low = mid + 1;
			else
				high = mid;
			old_mid = mid;
		};
		return low;
	}

	void PropertyOfManyResourcesModule::attached(TripleModel &model)
	{
		CacheModule::attached(model);
		if(columns_->size())
		{
			ascending_primary_ = (*columns_)[0].variable().metaStrategy()
					& RDFStrategy::Ascending;
		}
	}

	QModelIndex PropertyOfManyResourcesModule::index
			(RawTripleData const &triple, QModelIndex const &current) const
	{
		if(model()->isWholeIndex(current))
		{
			if(RawPropertyDataWithEqualityMask<RawEqualityMask>(triple.toRawPropertyData())
					!= property_data_)
				return QModelIndex();
			int row = searchResource((triple.mask() & RDFStrategy::Object)
					? triple.object()
					: triple.subject());
			//return createIndex(row, 1, (*columns_)[1].strategy());
			return createIndex(row, 1, 0);
		}
		return index(triple.toRawPropertyData(), current);
	}

	CacheModule::SingleItemColumns PropertyOfManyResourcesModule::primeSingleItemColumns()
	{
		// do not register the first column, holding the parent resources of the properties
		SingleItemColumns ret;
		for(int i = 1; i < columns_->realColumnCount(); ++i)
			ret << i;
		return ret;
	}

	void PropertyOfManyResourcesModule::registerInto(RDFGraphCache &cache)
	{
		// TODO: cache unused
		Q_UNUSED(cache);
	}

	void PropertyOfManyResourcesModule::rowsInserted
			(const QModelIndex &parent, int first, int last)
	{
		CacheModule::rowsInserted(parent, first, last);

		RowStore::iterator cur_i = rows_->begin() + first
				, end_i = (rows_->begin() + (last + 1)).skipGap();
		while(cur_i != end_i)
		{
			if(cur_i->size() > 1)
			{
				QString res = (*cur_i)[0];
				int top, bottom;

				top = bottom = cur_i.key();
				while(cur_i.skipGap(1) != end_i && cur_i->size() > 1 && (*cur_i)[0] == res)
					bottom = cur_i.key();
				if(ResourceOfGraphCachePtr cachedres = cachedResource(res, 0))
				{
					cachedres->insertCachedProperty
							//(property_data_, createIndex(top, 1, property_data_.strategy()));
							(property_data_, createIndex(top, 1, 0));
#ifdef RDFGRAPHCACHE_DEBUG
					debug(4, model()) << "Registered resource(" << res << ") property "
									  << property_data_.predicate() << ", " << 0;
#endif

				}
			} else
				cur_i.skipGap(1);
		}
	}
}
