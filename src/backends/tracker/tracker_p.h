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
 * tracker.h
 *
 *  Created on: Jan 5, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef QTTRACKER_BACKENDS_TRACKER_TRACKER_P_H_
#define QTTRACKER_BACKENDS_TRACKER_TRACKER_P_H_

#include <QDBusPendingCallWatcher>
#include "../../liveresource_p.h"
#include "../../util_p.h"
#include "../../sequencer.h"
//#include "../../rdfunbound_p.h"
#include "common_p.h"
#include "graph_signals_p.h"
#include "id2iricache_p.h"
#include "trackeraccess.h"

class QSparqlConnection;

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			class TrackerAccess;
			class TrackerQueryResult;
			class TrackerAccessDbus;

			enum { BatchSPARQLUpdate = RDFQuery::SPARQLUpdate + 1 };

			class TrackerResource
				: public ResourceOfService
			{
				/* VZR_CLASS(TrackerResource, (ResourceOfService));*/

			public:
				TrackerResource
						(QSharedPointer<RDFServiceImplBase> const &service, Node const &node);
				~TrackerResource();

				mutable QString has_signaling_type; // VZR_MEMBER
				mutable TripleUpdateContextPtr live_update_context_; // VZR_MEMBER

				TrackerService *tracker() const;

				void removeInterface(QUrl const &/*interface_name*/) const;

				LiveNodeModelBasePtr implGetProperties(RDFDerivedSelect *columns
						, TripleModulePtr const &triple_module = TripleModulePtr()) const;
			};

			/*!
			 * Responsible for providing access to the local user tracker database.
			 * minor, specific: offers services for processing events while waiting pending call
			 * major, specific: tracker signal to live strategy handling
			 * major, generic: raw layer functionality implementation
			 * minor, generic: creation of unique identifiers
			 * major, generic: conversion between raw tracker data and generic qttracker data
			 * major, generic: query processing
			 * minor, generic: service attributes
			 */

			struct TrackerService
				: RDFServiceBasic
			{
				Q_OBJECT

				/* VZR_CLASS(TrackerService, (RDFServiceBasic));*/

			public:
				TrackerService();
				~TrackerService();

				QUrl serviceIri() const;

				static QSharedPointer<RDFGraphCache> getRDFGraphCache(bool persistent = true);
				static QSharedPointer<TrackerService> get();

				mutable org::freedesktop::Tracker1::Resources resources; // VZR_MEMBER
				QSharedPointer<QSparqlConnection> directConnection_; // VZR_MEMBER
				QString stem; // VZR_MEMBER

				int id_counter; // VZR_MEMBER
				int query_id_counter; // VZR_MEMBER

				int update_id_counter; // VZR_MEMBER
				int sync_id_counter; // VZR_MEMBER

				RDFServicePtr local_service_; // VZR_MEMBER

				IDIriCache id2iri_; // VZR_MEMBER

				Sequencer query_sequencer_; // VZR_MEMBER

				QSharedPointer<TrackerAccess> createTrackerAccess(AccessType queryType);

				bool waitForFinishedPendingCall
						( QDBusPendingCall &pending_call
						, QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents) const;

					template<typename Reply_>
				bool waitForFinished(Reply_ &reply)
				{
					if(!reply.isFinished())
					{
						QVariantMap::const_iterator process_flags
								= attributes_.constFind("process_events_on_block");
						if(process_flags != attributes_.constEnd())
							return waitForFinishedPendingCall
									( reply, process_flags->value<QEventLoop::ProcessEventsFlags>());
						reply.waitForFinished();
					}
					return reply.isFinished();
				}

				typedef QMap<QString, ClassUpdateDispatcher *> QString2Dispatcher;
				typedef QScopedPointer<QMap<QString, GraphClassUpdateDispatcherPtr> > Dispatchers;

				Dispatchers graph_dispatchers_; // VZR_MEMBER
				QString2Dispatcher class2dispatcher; // VZR_MEMBER

				QString2Dispatcher property2dispatcher; // VZR_MEMBER

				ClassUpdateDispatcher *dispatcherFromClass(QString const &class_name);
				ClassUpdateDispatcher *dispatcherFromProperty(QString const &property_name);

				void loadTrackerPrefixes();

				QVector<QStringList> rawExecuteQueryOnGraph
						( QString const &query, int type, RDFGraphImplBase const &graph);
				void rawInsertTripleToGraph
						( QString const &subject, QString const &predicate, QString const &object
						, RDFGraphImplBase const &graph);
				void rawDeleteTripleFromGraph
						( QString const &subject, QString const &predicate, QString const &object
						, RDFGraphImplBase const &graph);
				void rawLoadOnGraph
						(QUrl const &file_uri, RDFGraphImplBase const &graph);

				QUrl createUniqueIri(QString stem_text);

				TrackerResource *newLiveResource(Node const &node = Node());

				Node rawNodeFromString(QString string, CppType type);
				QString rawStringFromNode(Node const &node);

				RDFStrategyFlags preprocessQuery(RDFStrategyFlags strategy);

				LiveNodes modelQuery
						( QString const &select, QStringList const &columns
						, RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy
						, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()
						, TripleModulePtr const &triple_module = TripleModulePtr());
				LiveNodeModelBasePtr baseImplModelQuery
						( RDFDerivedSelect *select
						, RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy
						, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()
						, TripleModulePtr const &triple_module = TripleModulePtr());
				QList<LiveNodes> executeQuery
						( RDFQuery const &query
						, RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy
						, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());

				void addRemoveResourceClause(RDFStatementList *deleter, RDFVariable const &res);

				QVariant serviceAttribute(QString const &attribute_name) const;
				QVariantMap serviceAttributes() const;

				int defaultTimeout() const;
				void setDefaultTimeout(int timeout = -1);

				QStringList inferNotifyTypes(RDFVariable const &resource) const;

				TripleUpdateContext::Mode registerForTripleUpdates
						(TripleUpdateContextPtr const &context);

			public Q_SLOTS:
				void sequencedStarted(SequencedOperation *operation);
				void sequencedSucceeded(SequencedOperation *operation);
				void sequencedFailed(SequencedOperation *operation);
			};

			/*
			 * one instance per query. Has only one active operation at a time.
			 * major: contains the query structure and text to apply to the model
			 * major: acts as the link between the backend data source and public API model
			 * major: implements refreshModel to initiate query, wait and process results
			 */

			struct TrackerContentModule
				: QObject, ContentModule_Simple
			{
				Q_OBJECT

				/* VZR_CLASS(TrackerContentModule, (QObject), (ContentModule_Simple));*/

			public:

				QSharedPointer<TrackerService> service_; // VZR_MEMBER
				RDFDerivedSelect columns_; // VZR_MEMBER
				int query_id_; // VZR_MEMBER

				bool flush_on_attach_; // VZR_MEMBER

				// a handle to the active sequenced operation for this query
				QSharedPointer<SequencedOperation> sequenced_; // VZR_MEMBER

				bool pending_sequenced_; // VZR_MEMBER

				QSharedPointer<TrackerAccess> tracker_access_; // VZR_MEMBER

				TrackerContentModule
						( QSharedPointer<TrackerService> service
						, QSharedPointer<TrackerAccess> tracker_access
						, RDFDerivedSelect const &columns
						, bool flush_on_attach = true
						, ContentModule::ChangePolicy change_policy = ContentModule::ReportChanges);
				TrackerContentModule(TrackerContentModule const &cp);
				~TrackerContentModule();

				virtual QString getQueryText() const;
				void attached(RowStoreModel &model);
				void detached(RowStoreModel &model);

				virtual void abort();

				ReadyFlags refreshModel_chain
						( LiveNodeModel::RefreshMode mode, InterfaceChain<ModelCommons> chain);

			public Q_SLOTS:
				virtual void sequencedStart();
				void queryCompleted();

			public:
				virtual ModelCommons::ReadyFlags processResults(TrackerQueryResult const &queryResult);

				bool alterStrategy_chain
						( RDFStrategyFlags flags, RDFStrategyFlags mask, QModelIndex const &index
						, InterfaceChain<ModelCommons> chain);

				ContentModulePtr copy() const;

			Q_SIGNALS:
				void sequencedSuccess();
				void sequencedFailure();
			};

			// Responsibility: Executing a RDFUpdate with the Tracker backend;
			// getting informed when the update is done.

			// FIXME: a more natural relationship between these 2 than inheritance?
			struct TrackerUpdateModule
				: TrackerContentModule
			{
				Q_OBJECT

				/* VZR_CLASS(TrackerUpdateModule, (TrackerContentModule));*/

			public:
				RDFUpdate update_; // VZR_MEMBER

				bool batch_mode_; // VZR_MEMBER
				bool sync_on_commit_; // VZR_MEMBER


				// Many TrackerUpdateModules may share the same D-Bus call.  This happens when the
				// tracker update query will contain multiple insert statements.  Each insert
				// statement gets its own model (and thus, a TrackerUpdateModule.) The
				// TrackerUpdateModule knows which is the index of the related insert statement in
				// the update, and also the other TrackerUpdateModules sharing the same D-Bus call.
				QList<TrackerUpdateModule *> sister_modules_; // VZR_MEMBER

				int my_index_; // VZR_MEMBER

				TrackerAccessDbus *tracker_access_dbus_; // VZR_MEMBER

				TrackerUpdateModule(QSharedPointer<TrackerService> service,
									RDFDerivedSelect const& columns,
									RDFUpdate const &update,
									bool flush_on_attach = true,
									bool batch_mode = false,
									bool sync_on_commit = true,
									ContentModule::ChangePolicy change_policy = ContentModule::ReportChanges);
				TrackerUpdateModule(TrackerUpdateModule const& cp);

				~TrackerUpdateModule();

				QString getQueryText() const;

				void abort();

				void setSisterModules(QList<TrackerUpdateModule*> const &sister_modules, int ix);

			public Q_SLOTS:
				void sequencedStart();
				void runSync();

			public:
				ModelCommons::ReadyFlags processResults(TrackerQueryResult const &queryResult);

			};

			struct TCM_TextQuery
				: TrackerContentModule
			{
				/* VZR_CLASS(TCM_TextQuery, (TrackerContentModule));*/

				QString query_text_; // VZR_MEMBER

				static QSharedPointer<TCM_TextQuery> create
						( QSharedPointer<TrackerService> service
						, QSharedPointer<TrackerAccess> tracker_access
						, QString const &query_text
						, QStringList const &columns, bool flush_on_attach = true);
			private:
				friend class QSharedCreator<TCM_TextQuery>;
				TCM_TextQuery
						( QSharedPointer<TrackerService> service
						, QSharedPointer<TrackerAccess> tracker_access
						, QString const &query_text
						, RDFDerivedSelect const &columns, bool flush_on_attach);
				QString getQueryText() const;
				ContentModulePtr copy() const;
			};
		}
	}
}
#endif /* QTTRACKER_BACKENDS_TRACKER_TRACKER_P_H_ */
