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
 * rdfservicebase_p.h
 *
 *  Created on: May 31, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_RDFSERVICEBASE_P_H_
#define SOPRANOLIVE_RDFSERVICEBASE_P_H_

#include <QtCore/QExplicitlySharedDataPointer>
#include "tripleupdates_p.h"
#include "include/sopranolive/unbound/rdfderivedselect.h"
#include "include/sopranolive/rdfservice.h"
#include "qsharedset_p.h"
#include "util_p.h"

namespace SopranoLive
{
	struct ResourceData;
	struct RDFServiceBasic;

	class LiveNodeModelBase;
	typedef QSharedPointer<LiveNodeModelBase> LiveNodeModelBasePtr;
	typedef QSharedPointer<const LiveNodeModelBase> ConstLiveNodeModelBasePtr;

	typedef QSharedSetStorage<QString> SharedTypeSetStorage;
	typedef QSharedSet<QString> SharedTypeSet;

	//! Stores shared service context data.
	struct ServiceContextData
	{
		/* VZR_CLASS(ServiceContextData);*/

		ServiceContextData();

		//! The shared type storage of the context

		SharedTypeSetStorage shared_type_storage_; // VZR_MEMBER
		RDFDerivedSelect select_prototype_; // VZR_MEMBER

		RDFUpdate update_prototype_; // VZR_MEMBER

		virtual RDFDerivedSelect select();
		virtual RDFDerivedSelect select(RDFSelect const &copy);
		virtual RDFUpdate update();
		virtual RDFUpdate update(RDFUpdate const &copy);
	};
	typedef QSharedPointer<ServiceContextData> ServiceContextDataPtr;

	class RDFGraphImplBase
		: public RDFService, public TripleUpdateNotifier, public LogContext
	{
		/* VZR_CLASS(RDFGraphImplBase, (RDFService), (TripleUpdateNotifier), (LogContext));*/

	public:
		RDFGraphImplBase(ServiceContextDataPtr const &service_context_data);

		// a bit hacky. See the implementation of ResourceByGraphQueries::attachInterface
		bool discard_interface_modifications; // VZR_MEMBER

		RDFStatementList interface_adds; // VZR_MEMBER
		ServiceContextDataPtr service_context_data_; // VZR_MEMBER


		virtual LiveResource *newLiveResource(Node const &node) = 0;

		virtual LiveNode wrapLiveNode(LiveNode const &node);
		virtual LiveNodes wrapLiveNodes(LiveNodes const &model);
		virtual LiveNodeModelBasePtr wrapLiveNodeModelBase(LiveNodeModelBasePtr const &model);

		LiveNode implLiveNode(LiveNode const &node, LiveNode const &type = LiveNode()
				, bool strict = false, QList<QUrl> const &resource_contexts = QList<QUrl>());

		virtual LiveNodes modelQuery
				( QString const &select, QStringList const &columns
				, RDFStrategyFlags model_strategy, QSharedPointer<QObject> const &parent
				, TripleModulePtr const &triple_module) = 0;
		LiveNodes modelQuery
				( QString const &select, QStringList const &columns
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());

		virtual LiveNodeModelBasePtr baseImplModelQuery
				( RDFDerivedSelect *select, RDFStrategyFlags model_strategy
				, QSharedPointer<QObject> const &parent
				, TripleModulePtr const &triple_module) = 0;
		LiveNodeModelBasePtr baseImplModelQuery
				( RDFDerivedSelect *select, RDFStrategyFlags model_strategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());
		LiveNodeModelBasePtr implModelQuery
				( RDFDerivedSelect *select
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());

		LiveNodes modelQuery
				( RDFSelect const &select
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());
		LiveNodes modelVariable
				( RDFVariable const &variable
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());
		LiveNodes modelVariables
				( RDFVariableList const &variables
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());
	};
	typedef QSharedPointer<RDFGraphImplBase> RDFGraphImplBasePtr;

		template<typename Base_>
	class EntityOfGraph
		: public Base_
	{
		/* VZR_CLASS(EntityOfGraph, (Base_));*/

	protected:

		EntityOfGraph(QSharedPointer<RDFGraphImplBase> const &graph
				= QSharedPointer<RDFGraphImplBase>())
			: Base_(), graph_(graph) {}
			template<typename P_>
		EntityOfGraph(QSharedPointer<RDFGraphImplBase> const &graph, P_ const &p)
			: Base_(p), graph_(graph) {}

		RDFGraphImplBase *graph() const { return graph_.data(); }

		QSharedPointer<RDFGraphImplBase> graph_; // VZR_MEMBER
	};

	class RDFServiceImplBase
		: public RDFGraphImplBase
	{
		/* VZR_CLASS(RDFServiceImplBase, (RDFGraphImplBase));*/

	public:
		RDFServiceImplBase(ServiceContextDataPtr const &service_context_data);

		virtual QVector<QStringList> rawExecuteQueryOnGraph
				( QString const &query, int type, RDFGraphImplBase const &graph) = 0;
		virtual void rawInsertTripleToGraph
				( QString const &subject, QString const &predicate, QString const &object
				, RDFGraphImplBase const &graph) = 0;
		virtual void rawDeleteTripleFromGraph
				( QString const &subject, QString const &predicate, QString const &object
				, RDFGraphImplBase const &graph) = 0;
		virtual void rawLoadOnGraph(QUrl const &file_uri, RDFGraphImplBase const &graph) = 0;

		virtual void insertTripleToGraph
				( LiveNode const &subject, LiveNode const &predicate, LiveNode const &object
				, RDFGraphImplBase const &graph) = 0;
		virtual void deleteTripleFromGraph
				( LiveNode const &subject, LiveNode const &predicate, LiveNode const &object
				, RDFGraphImplBase const &graph) = 0;


		QString rawQueryString(RDFQuery const &query) const;

		void setVerbosity(int level);
	};
}

#endif /* SOPRANOLIVE_RDFSERVICEBASE_P_H_ */
