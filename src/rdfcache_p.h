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
 * rdfcache_p.h
 *
 *  Created on: Apr 28, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_RDFCACHE_P_H_
#define SOPRANOLIVE_RDFCACHE_P_H_

#include <QMap>
#include "rdfservice_p.h"
#include "liveresource_p.h"
#include "derived_columns_p.h"
#include "livenodemodelbridge_p.h"

namespace SopranoLive
{
	class RDFGraphCache;

	class PropertyOfSingleResourceModule;
	class CachedPropertyManager;

	typedef QSharedPointer<PropertyOfSingleResourceModule> PropertyOfSingleResourceModulePtr;
	typedef QSharedPointer<CachedPropertyManager> CachedPropertyManagerPtr;

	class ResourceOfGraphCache
		: public ResourceOfGraphBridge
	{
		/* VZR_CLASS(ResourceOfGraphCache, (ResourceOfGraphBridge));*/

	public:
		~ResourceOfGraphCache();
		typedef QHash< RawPropertyDataWithEqualityMask<RawEqualityMask>, PersistentTripleIndex> CachedSubModels;

		void insertCachedProperty(RawPropertyData const &predicate, QModelIndex const &sub_index) const;
		TripleIndex findCachedProperty
				( RawPropertyData const &predicate, RDFStrategyFlags strategy, RDFStrategyFlags strategy_mask) const;
		TripleIndex findCachedProperty
				( RawPropertyData const &predicate, RDFStrategyFlags strategy = RDFStrategy::Disabled) const
		{
			return findCachedProperty(predicate, strategy, strategy);
		}

		void invalidate();

	public:
		friend class RDFGraphCache;

		ResourceOfGraphCache(QSharedPointer<RDFGraphCache> const &graph, LiveNode const &backend);

		Node const &node() const;
		inline RDFGraphCache *graphCache() const;

		LiveNodes modelCachedProperty
				( RawPropertyData const &predicate, RDFStrategyFlags strategy = RDFStrategy::Disabled
				, RDFStrategyFlags strategy_mask = RDFStrategy::Disabled
				, PropertyOfSingleResourceModulePtr const &column_module = PropertyOfSingleResourceModulePtr()
				) const;

		TripleModel *adoptTripleModelOrWrap(LiveNodeModel *livenodemodel) const;

		LiveNodes types(RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent) const;

		int implHasProperties(RDFProperty &property_data) const;

		LiveNodeModelBasePtr implGetProperties
				( RDFDerivedSelect *columns, TripleModulePtr const &triple_module = TripleModulePtr()) const;
		LiveNodes implGetProperties
				( RDFProperty &property_data, RDFVariableList const &additional_columns = RDFVariableList()) const;

		LiveNode getObject(LiveNode const &predicate, CppType object_type = CppType()) const;
		LiveNode firstObject(LiveNode const &predicate, CppType object_type = CppType()) const;

		LiveNode getSubject(LiveNode const &predicate, CppType subject_type = CppType()) const;
		LiveNode firstSubject(LiveNode const &predicate, CppType subject_type = CppType()) const;

		void remove() const;

		void attachInterface(QUrl const &interface_name, LiveResource &interface);

		enum { backend_wants_plain_model_property_query = true };

		mutable CachedSubModels cached_properties_;

		typedef QMap<QString, ResourceOfGraphCache *> Nodes;
		mutable Nodes::iterator self_reference_; // VZR_MEMBER
		QSharedPointer<LiveResource> invalidated_backend_; // VZR_MEMBER

		SharedTypeSet type_set_; // VZR_MEMBER
	private:
		void operator=(ResourceOfGraphCache const &/*disabled*/);
	};
	typedef QSharedPointer<ResourceOfGraphCache> ResourceOfGraphCachePtr;

	class RDFGraphCache
		: public RDFGraphBridge
	{
		/* VZR_CLASS(RDFGraphCache, (RDFGraphBridge));*/

	public:

		enum { StrategiesImplementedByGraphCache = ( RDFStrategy::Streaming | RDFStrategy::Windowed
				| RDFStrategy::Live | RDFStrategy::Writable | RDFStrategy::Cached
				| RDFStrategy::ExpandDerivedProperties
				) };
		RDFGraphCache(QSharedPointer<RDFGraphImplBase> const &backend);
		~RDFGraphCache();

		LiveNodeModelPtr preGetProperties(ResourceOfGraphCache const &resource, RDFDerivedSelect *columns);
		LiveNodeModelPtr postGetProperties(ResourceOfGraphCache const &resource, RDFDerivedSelect *columns);

		typedef QMap<QString, ResourceOfGraphCache *> Nodes;

		LiveNode createLiveNode(CppType type);

		LiveNode implLiveNode(LiveNode const &node, LiveNode const &type = LiveNode()
				, bool strict = false, QList<QUrl> const &resource_contexts = QList<QUrl>());

		LiveNodes implLiveNodes(LiveNodeList const &urls, RDFVariable const &object_info,
					RDFStrategyFlags model_strategy, bool strict,
					QSharedPointer<QObject> const &parent);

		ResourceOfGraphCache *newLiveResource(Node const &node);
		ResourceOfGraphCache *newLiveResourceStraightFromBackEnd(LiveNode const &node);

		LiveNode wrapLiveNode(LiveNode const &node);
		LiveNodes wrapLiveNodes(LiveNodes const &model);
		LiveNodeModelBasePtr wrapLiveNodeModelBase(LiveNodeModelBasePtr const &model);

		TripleModulePtr preProcessQuery
				( RDFDerivedSelect *columns, RDFStrategyFlags *strategy, TripleModulePtr const &triple_module
				, RDFStrategyFlags *our_strategies, RawPropertyData *rpdata = 0, LiveResource const *res = 0
				);
		LiveNodeModelBasePtr baseImplModelQuery
				( RDFDerivedSelect *select, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()
				, TripleModulePtr const &triple_module = TripleModulePtr());
		LiveNodeModelBasePtr postProcessTripleModel
				( LiveNodeModelBasePtr const &model, RDFStrategyFlags strategy, RDFStrategyFlags our_strategies
				, TripleModulePtr const &triple_strat = TripleModulePtr());

		QList<LiveNodes> executeQuery
				( RDFQuery const &query, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());

		SharedTypeSetStorage &typeSetStorage() const { return *type_set_storage_; }
	protected:
		friend class ResourceOfGraphCache;
		mutable Nodes cached_nodes_; // VZR_MEMBER
		LiveNode dummy; // VZR_MEMBER

		mutable QSharedPointer<SharedTypeSetStorage> type_set_storage_; // VZR_MEMBER
	};

	class CacheModule
		: public DerivedColumnModule
	{
		Q_OBJECT

		/* VZR_CLASS(CacheModule, (DerivedColumnModule));*/

	protected:
		CacheModule(RDFGraphImplBasePtr const &graph);

		ModulePtr attach(LiveNodeModelBase &model);
		void attached(TripleModel &model);
		void detached(TripleModel &model);

		RDFGraphCache *graphCache() const { return static_cast<RDFGraphCache *>(model()->graph()); }
		// TODO: replace with a structure that supports removal too.

		typedef QHash<QString, ResourceOfGraphCachePtr> QString2Resource;
		QString2Resource cached_resources_; // VZR_MEMBER
		typedef QVector<int>  SingleItemColumns;
		SingleItemColumns single_item_columns_; // VZR_MEMBER

		ResourceOfGraphCachePtr cachedResource(QString const &str, LiveNode const &node);
		ResourceOfGraphCachePtr cachedResource(LiveNode const &node);
		ResourceOfGraphCachePtr cachedResource(QString const &res_str, int column);

		LiveNodeModelBasePtr modelColumnVariable(RDFDerivedColumn &column_info);

		void registerSingleItemWildcardEntries
				( SingleItemColumns const &cols, int first_row, int last_row);

		virtual SingleItemColumns primeSingleItemColumns();


	public Q_SLOTS:
		virtual void rowsInserted(const QModelIndex &parent, int first, int last);
		virtual void dataChanged(QModelIndex const &top_left, QModelIndex const &bottom_right);
		virtual void rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);

		virtual void mainModelUpdated();
		virtual void cachedModelUpdated();
	};

	class OneMainColumnManyProperties
		: public CacheModule
		, public ItemsAsColumnModule
	{
		/* VZR_CLASS(OneMainColumnManyProperties, (CacheModule), (ItemsAsColumnModule));*/

	public:
		OneMainColumnManyProperties(RDFGraphImplBasePtr const &graph);
		void registerInto(RDFGraphCache &cache);
	};

	class ManyMainColumnManyProperties
		: public CacheModule
	{
		/* VZR_CLASS(ManyMainColumnManyProperties, (CacheModule));*/

	public:
		ManyMainColumnManyProperties(RDFGraphImplBasePtr const &graph);
		void registerInto(RDFGraphCache &cache);
	};

	class PropertyOfSingleResourceModule
		: public OneMainColumnManyProperties
		, public ModuleInterface
				<TripleUpdateReceiver, TripleModel, PropertyOfSingleResourceModule>
	{
		/* VZR_CLASS(PropertyOfSingleResourceModule
				, (OneMainColumnManyProperties)
				, (ModuleInterface
						<TripleUpdateReceiver, TripleModel, PropertyOfSingleResourceModule>));*/

		RawPropertyData property_data_; // VZR_MEMBER
		LiveResourceConstPtr property_; // VZR_MEMBER

	public:
		PropertyOfSingleResourceModule
				( RDFGraphImplBasePtr const &graph, RawPropertyData const &property_data
				, LiveResourceConstPtr const &property);

		using OneMainColumnManyProperties::index;
		QModelIndex index(RawTripleData const &triple, QModelIndex const &current) const;

		void registerInto(RDFGraphCache &cache);

		bool tripleUpdates_chain
				( RawTriplesData const &triples, TripleUpdateContextConstPtr const &context
				, InterfaceChain<TripleUpdateReceiver> chain);
	};

	class PropertyOfManyResourcesModule
		: public ManyMainColumnManyProperties
	{
		/* VZR_CLASS(PropertyOfManyResourcesModule, (ManyMainColumnManyProperties));*/

		RawPropertyData property_data_; // VZR_MEMBER
		bool ascending_primary_; // VZR_MEMBER


		int searchResource(QString const &res) const;
	public:
		PropertyOfManyResourcesModule(RDFGraphImplBasePtr const &graph, RawPropertyData const &property_data);

		void attached(TripleModel &model);

		using ManyMainColumnManyProperties::index;
		QModelIndex index(RawTripleData const &triple, QModelIndex const &current) const;

		SingleItemColumns primeSingleItemColumns();

		void rowsInserted(const QModelIndex &parent, int first, int last);
		void registerInto(RDFGraphCache &cache);
	};
}

#endif /* SOPRANOLIVE_RDFCACHE_P_H_ */
