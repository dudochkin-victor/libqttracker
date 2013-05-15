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
 * rdfservice_p.h
 *
 *  Created on: Mar 30, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_RDFSERVICE_P_H_
#define SOPRANOLIVE_RDFSERVICE_P_H_

#include <map>
#include "liveresource_p.h"
#include "triplemodel_p.h"
#include "transaction_p.h"

namespace SopranoLive
{

	class RDFGraphBridge
		: public RDFServiceImplBase
	{
		/* VZR_CLASS(RDFGraphBridge, (RDFServiceImplBase));*/

	protected:
		RDFGraphBridge(QSharedPointer<RDFGraphImplBase> const &backend)
			: RDFServiceImplBase(backend->service_context_data_), backend_(backend) {}

		QSharedPointer<RDFGraphImplBase> backend_; // VZR_MEMBER
	public:
		RDFGraphImplBase &backend() { return *backend_.data(); }
		RDFGraphImplBase const &backend() const { return *backend_.data(); }

		RDFServiceImplBase &serviceBackend()
		{ return static_cast<RDFServiceImplBase &>(*backend_.data()); }
		RDFServiceImplBase const &serviceBackend() const
		{ return static_cast<RDFServiceImplBase const &>(*backend_.data()); }

		RDFServicePtr service() const{ return backend().service(); }
		QUrl graphIri() const { return backend().graphIri(); }
		QUrl serviceIri() const { return backend().serviceIri(); }
		QUrl createUniqueIri(QString stem_text) { return backend().createUniqueIri(stem_text); }

		LiveNode createLiveNode(CppType type = CppType::of<QUrl>())
		{ return wrapLiveNode(backend().createLiveNode(type)); }
		LiveNode implLiveNode
				( LiveNode const &node, LiveNode const &type = LiveNode()
				, bool strict = false, QList<QUrl> const &resource_contexts = QList<QUrl>())
		{ return wrapLiveNode(backend().implLiveNode(node, type, strict, resource_contexts)); }

		LiveNodes implLiveNodes
    		( LiveNodeList const &urls, RDFVariable const &object_info,
              RDFStrategyFlags model_strategy, bool strict,
              QSharedPointer<QObject> const &parent)
		{
			return wrapLiveNodes(backend().implLiveNodes(urls, object_info,
														 model_strategy, strict, parent));
		}

		void addPrefix(QString prefix, QUrl value) { return backend().addPrefix(prefix, value); }
		QUrl prefixSuffix(QString prefix, QString suffix)
		{ return backend().prefixSuffix(prefix, suffix); }

		void insertTriple
				( LiveNode const &subject, LiveNode const &predicate, LiveNode const &object)
		{ return backend().insertTriple(subject, predicate, object); }
		void deleteTriple
				( LiveNode const &subject, LiveNode const &predicate, LiveNode const &object)
		{ return backend().deleteTriple(subject, predicate, object); }

		LiveNodes modelQuery
				( QString const &select, QStringList const &columns
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()
				, TripleModulePtr const &triple_module = TripleModulePtr())
		{ return wrapLiveNodes(backend().modelQuery
				(select, columns, model_strategy, parent, triple_module)); }

		LiveNodeModelBasePtr baseImplModelQuery
				( RDFDerivedSelect *select
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()
				, TripleModulePtr const &triple_module = TripleModulePtr())
		{ return wrapLiveNodeModelBase(backend().baseImplModelQuery
				(select, model_strategy, parent, triple_module)); }

		QList<LiveNodes> executeQuery
				( RDFQuery const &query
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())
		{
			QList<LiveNodes> ret;
			foreach(LiveNodes itam, backend().executeQuery(query, model_strategy, parent))
				ret.push_back(wrapLiveNodes(itam));
			return ret;
		}
		LiveNodes implModelResources(RDFProperty &resource_info)
		{ return wrapLiveNodes(backend().implModelResources(resource_info)); }

		void removeResources(RDFVariable const &variable)
		{ return backend().removeResources(variable); }

		TripleUpdateContext::Mode registerForTripleUpdates(TripleUpdateContextPtr const &ctxt)
		{ return backend().registerForTripleUpdates(ctxt); }

		// cludge. They should be in RDFServiceBridge but meh.

		// TODO: exposes backend service to user via RDFTransaction::service
		RDFTransactionPtr createTransaction(RDFTransaction::Mode mode)
		{ return serviceBackend().createTransaction(mode); }
		RDFTransactionPtr pendingTransaction() const
		{ return serviceBackend().pendingTransaction(); }
		RDFServicePtr createChildService() { return serviceBackend().createChildService(); }

		QVector<QStringList> rawExecuteQuery(QString const &query, int type = RDFQuery::SPARQL)
		{ return serviceBackend().rawExecuteQuery(query, type); }

		void rawInsertTriple
				( QString const &subject, QString const &predicate, QString const &object)
		{ return serviceBackend().rawInsertTriple(subject, predicate, object); }
		void rawDeleteTriple
				( QString const &subject, QString const &predicate, QString const &object)
		{ return serviceBackend().rawInsertTriple(subject, predicate, object); }

		QString rawQueryString(RDFQuery const &query) const
		{ return serviceBackend().rawQueryString(query); }

		void rawLoad(QUrl const &uri) { return serviceBackend().rawLoad(uri); }

		Node rawNodeFromString(QString string, CppType type)
		{ return serviceBackend().rawNodeFromString(string, type); }
		QString rawStringFromNode(Node const &node)
		{ return serviceBackend().rawStringFromNode(node); }

		QVector<QStringList> rawExecuteQueryOnGraph
				( QString const &query, int type, RDFGraphImplBase const &graph)
		{ return serviceBackend().rawExecuteQueryOnGraph(query, type, graph); }
		void rawInsertTripleToGraph
				( QString const &subject, QString const &predicate, QString const &object
				, RDFGraphImplBase const &graph)
		{ serviceBackend().rawInsertTripleToGraph(subject, predicate, object, graph); }
		void rawDeleteTripleFromGraph
				( QString const &subject, QString const &predicate, QString const &object
				, RDFGraphImplBase const &graph)
		{ serviceBackend().rawDeleteTripleFromGraph(subject, predicate, object, graph); }
		void rawLoadOnGraph(QUrl const &file_uri, RDFGraphImplBase const &graph)
		{ serviceBackend().rawLoadOnGraph(file_uri, graph); }
		void insertTripleToGraph
				( LiveNode const &subject, LiveNode const &predicate, LiveNode const &object
				, RDFGraphImplBase const &graph)
		{ serviceBackend().insertTripleToGraph(subject, predicate, object, graph); }
		void deleteTripleFromGraph
				( LiveNode const &subject, LiveNode const &predicate, LiveNode const &object
				, RDFGraphImplBase const &graph)
		{ serviceBackend().deleteTripleFromGraph(subject, predicate, object, graph); }

		QVariant serviceAttribute(QString const &attribute_name) const
		{ return serviceBackend().serviceAttribute(attribute_name); }
		QVariantMap serviceAttributes() const { return serviceBackend().serviceAttributes(); }
		bool setServiceAttribute(QString const &service_attribute, QVariant const &value)
		{ return serviceBackend().setServiceAttribute(service_attribute, value); }

		QDebug logMessage(int level, char const *func, char const *loc) const
		{ return backend().logMessage(level, func, loc); }
	};

	class RDFServiceBridge
		: public RDFGraphBridge
	{
		/* VZR_CLASS(RDFServiceBridge, (RDFGraphBridge));*/

	protected:
		RDFServiceBridge(QSharedPointer<RDFServiceImplBase> const &backend)
			: RDFGraphBridge(backend) {}
	public:
	};

	class ResourceOfService;

	struct RDFGraphBasic
		// should actually implement only RDFGraphBasic, but so that RDFServiceBasic can extend
		// this, we inherit RDFService and give its methods dummy stub implementations
		: RDFServiceImplBase
	{
		/* VZR_CLASS(RDFGraphBasic, (RDFServiceImplBase));*/

		typedef QMap<QString, QString> Prefixes;
		Prefixes prefixes; // VZR_MEMBER

		Prefixes reverse_prefixes; // VZR_MEMBER

		typedef QMap<QString, int> ImplementedContexts;
		ImplementedContexts implemented_contexts_; // VZR_MEMBER


		QSharedPointer<RDFServiceBasic> external_parent_ref_; // VZR_MEMBER
		QWeakPointer<RDFServiceBasic> parent_; // VZR_MEMBER


		QUrl graph_iri; // VZR_MEMBER

		RDFGraphBasic
				( QSharedPointer<RDFServiceBasic> parent, ServiceContextDataPtr const &ctxt)
			: RDFServiceImplBase(ctxt)
			, external_parent_ref_(parent), parent_(parent.toWeakRef()), graph_iri()
		{}


		// overrides from RDFGraph

		RDFServicePtr service() const;

		QUrl graphIri() const;

		QVector<QStringList> rawExecuteQuery(QString const &query, int type = RDFQuery::SPARQL);
		void rawInsertTriple
				( QString const &subject, QString const &predicate, QString const &object);
		void rawDeleteTriple
				( QString const &subject, QString const &predicate, QString const &object);
		void rawLoad(QUrl const &file_uri);

		QUrl createUniqueIri(QString stem_text);
		LiveNode createLiveNode(CppType type = CppType::of<QUrl>());

		void addPrefix(QString prefix, QUrl value);

		QUrl prefixSuffix(QString prefix, QString suffix);
		QPair<QString, QString> splitToPrefixSuffix(QString const &iri);

		LiveResourcePtr liveNodeRaw(LiveNode const &node, QString const &raw);
		LiveNode implLiveNode(LiveNode const &node, LiveNode const &type = LiveNode()
				, bool strict = false, QList<QUrl> const &resource_contexts = QList<QUrl>());

		LiveNodes implLiveNodes
    		( LiveNodeList const &urls, RDFVariable const &object_info,
              RDFStrategyFlags model_strategy, bool strict,
              QSharedPointer<QObject> const &parent);

		ResourceOfService *newLiveResource(Node const &node);

		virtual Node rawNodeFromString(QString string, CppType type);
		virtual QString rawStringFromNode(Node const &node);

		void insertTriple
				( LiveNode const &subject, LiveNode const &predicate, LiveNode const &object);
		void deleteTriple
				( LiveNode const &subject, LiveNode const &predicate, LiveNode const &object);

		LiveNodes modelQuery
				( QString const &select, QStringList const &columns
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()
				, TripleModulePtr const &triple_module = TripleModulePtr());
		LiveNodeModelBasePtr baseImplModelQuery
				( RDFDerivedSelect *select
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()
				, TripleModulePtr const &triple_module = TripleModulePtr());
		QList<LiveNodes> executeQuery
				( RDFQuery const &query
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());

		LiveNodes implModelResources(RDFProperty &resource_info);

		virtual void addRemoveResourceClause(RDFStatementList *deleter, RDFVariable const &res);
		RDFStatementList *recurseRemoveResources
				( RDFStatementList *deleter, RDFVariable const &res
				, QLinkedList<RDFVariable> *post_deletes);
		void removeResources(RDFVariable const &variable);

		// dummy stubs for RDFService virtuals.

		RDFTransactionPtr initiateTransaction() { return RDFTransactionPtr (); }
		RDFTransactionPtr pendingTransaction() const { return RDFTransactionPtr(); }
		RDFServicePtr createChildService() { return RDFServicePtr(); }

		TripleUpdateContext::Mode registerForTripleUpdates(TripleUpdateContextPtr const &ctxt);

		// own functions

		bool isAnyContextImplemented(QList<QUrl> const &contexts);
		void addImplementedContext(QUrl const &context);

	protected:
		void setParent( QWeakPointer<RDFServiceBasic> parent ) { parent_=parent; }
	};

	struct RDFServiceBasic
		: RDFGraphBasic
	{
		/* VZR_CLASS(RDFServiceBasic, (RDFGraphBasic));*/

		QString name_; // VZR_MEMBER

		typedef QVariantMap AttributeMap;
		mutable AttributeMap attributes_;

		Transaction * transaction_; // VZR_MEMBER

		RDFServiceBasic
				( QString name, QList<QUrl> const &implemented_contexts = QList<QUrl>()
				, QSharedPointer<RDFServiceBasic> const &paren = QSharedPointer<RDFServiceBasic>()
				, ServiceContextDataPtr const &ctxt
						= ServiceContextDataPtr(new ServiceContextData())
				);
		~RDFServiceBasic();

		void loadPrefixes();

		void insertTripleToGraph
				( LiveNode const &subject, LiveNode const &predicate, LiveNode const &object
				, RDFGraphImplBase const &graph);
		void deleteTripleFromGraph
				( LiveNode const &subject, LiveNode const &predicate, LiveNode const &object
				, RDFGraphImplBase const &graph);

		RDFTransactionPtr createTransaction(RDFTransaction::Mode mode = RDFTransaction::Default);
		RDFTransactionPtr pendingTransaction() const;

		RDFServicePtr createChildService();

		RowStoreModelPtr newRowStoreModel
				( RDFStrategyFlags strategy_flags
				, ContentModulePtr const &strategy = ContentModulePtr()
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());
		TripleModelPtr newTripleModel
				( RDFStrategyFlags strategy_flags
				, ContentModulePtr const &strategy = ContentModulePtr()
				, TripleModulePtr const &cache_module = TripleModulePtr()
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());

		QVariant serviceAttribute(QString const &attribute_name) const;
		QVariantMap serviceAttributes() const;
		bool setServiceAttribute(QString const &service_attribute, QVariant const &value);

		QDebug logMessage(int level, char const *func, char const *loc) const;
	};
}

#endif /* SOPRANOLIVE_RDFSERVICE_P_H_ */
