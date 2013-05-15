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
 * tracker.cpp
 *
 *  Created on: Jan 5, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include "tracker_p.h"
#include "rdftrackerselect_p.h"
#include "rdftrackerupdate_p.h"
#include "../local/local_p.h"
#include "../../include/QtTracker/Tracker"
#include "../../util_p.h"
#include "dbus/trackeraccessdbus.h"
#include "trackeraccessqsparql.h"
#include <QtSparql/QSparqlConnection>

#include <uuid.h>


Q_DECLARE_METATYPE(QEventLoop::ProcessEventsFlags)

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			QString service_g;
			QString base_path_g;

			QString search_interface_g;
			QString search_path_g;

			QString resources_interface_g;
			QString resources_path_g;

			QString resources_class_path_g;
			QString resources_class_interface_g;

			bool graph_update_signals_g = false;

			QMutex tracker_global_lock_g;

			QStringList initReservedSparqlWords()
			{
				QStringList ret;
				ret << "id" << "uri";
				return ret;
			}
			QStringList reserved_sparql_words = initReservedSparqlWords();

			using namespace SopranoLive;


			void dispatchBatchSparqlUpdate(RDFUpdate const &update)
			{
				service()->executeQuery(update, BatchMode);
			}

			void dispatchBatchCommit()
			{
				TrackerService::get()->resources.BatchCommit();
			}

			UpdateResultOperations sparqlUpdateBlank(const QString& query)
			{
				QSharedPointer<TrackerService> tr = TrackerService::get();
				QDBusPendingReply<UpdateResultOperations> reply
						= tr->resources.SparqlUpdateBlank(query);
				tr->waitForFinished(reply);
				return reply.value();
			}

			void sync(bool block)
			{
				QSharedPointer<TrackerService> tr = TrackerService::get();
				int sync_id = tr->sync_id_counter++;
				debug(2, tr.data()) << "syncing tracker (sync id =" << sync_id << ")";
				PERF_EVENT("before_sync %d", sync_id);
				QDBusPendingReply<> rep(tr->resources.Sync());
				if(block)
				{
					PERF_EVENT("before_waiting_sync %d", sync_id);
					tr->waitForFinished(rep);
					PERF_EVENT("after_waiting_sync %d", sync_id);
					debug(2, tr.data()) << "done syncing tracker (sync id =" << sync_id << ")";
				}
			}

			bool initializeTrackerEnvironment()
			{
				registerQTrackerDbusTypes();

				service_g = "org.freedesktop.Tracker1";
				base_path_g = "/org/freedesktop/Tracker1";

				if(!trackerBus().interface()->isServiceRegistered(service_g)
					&& (trackerBus().interface()->startService(service_g)
						, !trackerBus().interface()->isServiceRegistered(service_g)))
					critical() << "cannot connect to org.freedesktop.Tracker1 service";

				search_interface_g = service_g + ".Search";
				search_path_g = base_path_g + "/Search";

				resources_interface_g = service_g + ".Resources";
				resources_path_g = base_path_g + "/Resources";

				resources_class_interface_g = service_g + ".Resources.Class";
				resources_class_path_g = base_path_g + "/Resources/Classes";

				QDBusReply<QString> result = trackerBus().call(QDBusMessage::createMethodCall
						( service_g, resources_path_g
						, "org.freedesktop.DBus.Introspectable", "Introspect"));

				if(!result.isValid())
					critical()
							<< "cannot access" << resources_path_g << "introspection:"
							<< result.error();
				else if(result.value().contains("GraphUpdated"))
					graph_update_signals_g = true;

				return true;
			}

			class ClosingDBusConnection : public QDBusConnection
			{
			public:
				ClosingDBusConnection(const QDBusConnection& c)
					: QDBusConnection(c)
					{
					}
				~ClosingDBusConnection()
					{
						disconnectFromBus(name());
					}
			};

			QThreadStorage<ClosingDBusConnection*> dbus_tls;
			QAtomicInt counter = 0;

			QDBusConnection trackerBus(bool is_thread_private)
			{
				if(!is_thread_private)
					return QDBusConnection::sessionBus();

				// Create a separate D-Bus connection for each thread. Use ClosingDBusConnection so
				// that the bus gets disconnected when the thread storage is deleted.
				if (!dbus_tls.hasLocalData()) {
					QString id = QString::number(counter.fetchAndAddRelaxed(1))
							.prepend("libqttracker-dbus-");
					dbus_tls.setLocalData(new ClosingDBusConnection
							(QDBusConnection::connectToBus(QDBusConnection::SessionBus, id)));
				}
				return *dbus_tls.localData();
			}

			QThreadStorage<QWeakPointer<RDFGraphCache> *> tracker_tls;
			QThreadStorage<TLSDestroyer<QSharedPointer<RDFGraphCache> > *> tracker_persistent_tls;

			QSharedPointer<RDFGraphCache> TrackerService::getRDFGraphCache(bool persistent)
			{
				QSharedPointer<RDFGraphCache> ret;
				if(!tracker_tls.hasLocalData()
					// check if the weak pointer in local data is valid and store it to ret
					|| !(ret = *tracker_tls.localData()))
				{
					static bool initialized = false;
					if(!initialized)
					{
						QMutexLocker lock(&tracker_global_lock_g);
						if(!initialized)
							initialized = initializeTrackerEnvironment();
					}

					QSharedPointer<TrackerService> tracker
							= QSharedCreator<TrackerService>::create();
					tracker->setParent( tracker.toWeakRef() );
					ret = QSharedCreator<RDFGraphCache>::create(tracker);
					tracker_tls.setLocalData(new QWeakPointer<RDFGraphCache>(ret));
				}
				if(persistent && !tracker_persistent_tls.hasLocalData())
					tracker_persistent_tls.setLocalData
							(new TLSDestroyer<QSharedPointer<RDFGraphCache> >(ret));
				return ret;
			}

			QSharedPointer<TrackerService> TrackerService::get()
			{
				return getRDFGraphCache(false)->backend().sharedFromThis<TrackerService>();
			}

			RDFServicePtr tracker()
			{
				return TrackerService::getRDFGraphCache();
			}

			RDFServicePtr service()
			{
				return TrackerService::getRDFGraphCache(false);
			}

			RDFServicePtr trackerRelease()
			{
				if(!tracker_persistent_tls.hasLocalData())
					return RDFServicePtr();
				RDFServicePtr ret = *tracker_persistent_tls.localData();
				tracker_persistent_tls.setLocalData(0);
				return ret;
			}

			void enableProcessEventsOnBlock
					( RDFServicePtr const &tracker_service, QEventLoop::ProcessEventsFlags flags)
			{
				tracker_service->setServiceAttribute("process_events_on_block"
						, QVariant::fromValue(flags));
			}

			void disableProcessEventsOnBlock(RDFServicePtr const &tracker_service)
			{
				tracker_service->setServiceAttribute("process_events_on_block", QVariant());
			}

			QString stringFromNode(Node const &node)
			{
				if(node.isLiteral())
					switch(node.literal().type())
					{
					case QVariant::Date:
						return node.literal().toDate().toString(Qt::ISODate);
					case QVariant::DateTime:
						return node.literal().toDateTime().toString(Qt::ISODate);
					case QVariant::Time:
						return node.literal().toTime().toString(Qt::ISODate);
					default: break;
					}
				return node.toString();
			}

			Node nodeFromString(QString const &value, CppType type = CppType())
			{
				unsigned id = type.metaTypeId();

				if(!value.size())
				{
					if(id == QVariant::String)
						return Node(LiteralValue(value));
					return Node();
				}

				// XXX: maybe fuller support of user types, now assume all user types are
				// resources
				if(id >= QVariant::UserType)
					return Node(QUrl::fromEncoded(value.toAscii()));

				switch(id)
				{
				case CppType::None: break;
				case QVariant::Bool: return Node(LiteralValue(value == "true"));
				case QVariant::Int: return Node(LiteralValue(value.toInt()));
				case QVariant::UInt: return Node(LiteralValue(value.toUInt()));
				case QVariant::LongLong: return Node(LiteralValue(value.toLongLong()));
				case QVariant::ULongLong: return Node(LiteralValue(value.toULongLong()));
				case QVariant::Double: return Node(LiteralValue(value.toDouble()));
				case QVariant::Char: break; // return Node(LiteralValue(QChar));
				case QVariant::Map: break; // return Node(LiteralValue(QVariantMap));
				case QVariant::List: break; // return Node(LiteralValue(QVariantList));
				case QVariant::String: break; // return Node(LiteralValue(QString));
				case QVariant::StringList: break; // return Node(LiteralValue(QStringList));

				case QVariant::ByteArray:
					return Node(LiteralValue(value.toAscii())); // TODO: really?

				case QVariant::BitArray: break; // return Node(LiteralValue(QBitArray));

				case QVariant::Date:
					return Node(LiteralValue(QDate::fromString(value, Qt::ISODate)));

				case QVariant::Time:
					return Node(LiteralValue(QTime::fromString(value, Qt::ISODate)));

				case QVariant::DateTime:
					return Node(LiteralValue(QDateTime::fromString(value, Qt::ISODate)));
				// As of 3.9.2009, tracker is permissive regarding IRI's, so are we.
				case QVariant::Url: return Node(QUrl::fromEncoded(value.toAscii()));
				//	case QVariant::Url:
				//		return Node(QUrl::fromEncoded(value.toAscii(), QUrl::StrictMode));
				default: break;
				}
				return Node(LiteralValue(value));
			}

			TrackerResource::TrackerResource
					( QSharedPointer<RDFServiceImplBase> const &service, Node const &node)
				: ResourceOfService(service, node), has_signaling_type()
			{
				debug(4, graph_.data()) << "TrackerResource TR(" << this << ") created:"
										<< node.toString();
			}

			TrackerResource::~TrackerResource()
			{
				debug(4, graph_.data()) << "TrackerResource TR(" << this << ") destroyed:"
										<< node().toString();
			}

			TrackerService *TrackerResource::tracker() const
			{
				return static_cast<TrackerService *>(graph_.data());
			}

			void TrackerResource::removeInterface(QUrl const &interface_name) const
			{
				// tracker properly manages removal of all dependent triples
				graph_->deleteTriple(node(), rdf::type::iri(), interface_name);
			}

			LiveNodeModelBasePtr TrackerResource::implGetProperties(RDFDerivedSelect *columns
					, TripleModulePtr const &triple_module) const
			{
				RDFProperty &property_data = (*columns)[0].refPropertyData();

				bool implementing_live = false;
				if((property_data.strategy() & RDFStrategy::Live) && (columns->size() == 1))
				{
					property_data.disableStrategyFlags(RDFStrategy::LiveMask);
					implementing_live = true;
				}

				LiveNodeModelBasePtr ret = ResourceOfService::implGetProperties
						(columns, triple_module);

				if(implementing_live)
				{
					TripleModelPtr trip_ret = ret.staticCast<TripleModel>();
					TripleUpdateContextPtr ctxt(new TripleUpdateContext
							(trip_ret, property_data.asStatementWith(sourceVariable())));
					if(tracker()->registerForTripleUpdates(ctxt))
					{
						trip_ret->doEnableStrategyFlags(RDFStrategy::Live);
						live_update_context_ = ctxt;
					}
					else
						warning(graph_.data()) << "live property models with non-explicit predicates"
							" not supported, non-live model returned. Specify an explicit "
							"predicate.";
				}
				return ret;
			}

/*
			QSharedPointer<ContentModule> TCM_Model_Property::copy() const
			{
				return QSharedCreator<TCM_Model_Property>::create(*this);
			}
*/

			// if query_id is supplied, the query is flushed and executed
			// when attached. Otherwise, query has to be explicitly flushed.
			TrackerContentModule::TrackerContentModule
					( QSharedPointer<TrackerService> service
					, QSharedPointer<TrackerAccess> tracker_access
					, RDFDerivedSelect const &columns
					, bool flush_on_attach, ContentModule::ChangePolicy change_policy)
				: ContentModule_Simple(change_policy)
				, service_(service)
				, columns_(columns)
				, query_id_(0)
				, flush_on_attach_(flush_on_attach)
				, sequenced_(), pending_sequenced_(false)
				, tracker_access_(tracker_access)
			{
				debug(4, service_.data()) << "TrackerContentModule TCM(" << this << ") created";
			}
			TrackerContentModule::TrackerContentModule(TrackerContentModule const &cp)
				: QObject(), ContentModule_Simple(cp)
				, service_(cp.service_)
				, columns_(cp.columns_)
				, query_id_()
				, flush_on_attach_(cp.flush_on_attach_)
				, sequenced_(), pending_sequenced_(false)
				, tracker_access_(cp.tracker_access_)
			{
				debug(4, service_.data()) << "TrackerContentModule TCM(" << this << ") copied"
						" from TCM(" << &cp << ")";
			}
			TrackerContentModule::~TrackerContentModule()
			{
				debug(4, service_.data()) << "TrackerContentModule TCM(" << this << ") destroyed";
			}


			QString TrackerContentModule::getQueryText() const
			{
				return model()->columns_.getQuery(reserved_sparql_words);
			}

			void TrackerContentModule::attached(RowStoreModel &rowstoremodel)
			{
				ContentModule_Simple::attached(rowstoremodel);
				model()->setColumnInformation(columns_);
				columns_ = RDFDerivedSelect();
				if(flush_on_attach_)
					model()->refreshModel(LiveNodeModel::Flush);
			}
			void TrackerContentModule::detached(RowStoreModel &rowstoremodel)
			{
				columns_ = model()->columns_;
				abort();
				ContentModule_Simple::detached(rowstoremodel);
			}

			void TrackerContentModule::abort()
			{
				if(sequenced_)
				{
					sequenced_->abort();
					sequenced_.clear();
					pending_sequenced_ = false;
				}
				tracker_access_->abort();
			}

			ModelCommons::ReadyFlags TrackerContentModule::refreshModel_chain
					(LiveNodeModel::RefreshMode mode, InterfaceChain<ModelCommons> chain)
			{
				// << "TrackerContentModule(" << this << ")::" << __func__
				// << "mode(" << mode << "), running(" << running() << ")"
				// << "completion_watcher(" << completion_watcher_ << ")";

				if(!service_->resources.isValid()) {
					warning(service_.data()) << service_->resources.lastError();
					return ReadyFlags();
				}

				ReadyFlags ret = ContentModule_Simple::refreshModel_chain(mode, chain);

				// First step of refresh: flush the query if requested.
				// This is also how a new query is started, the entity creating the
				// TrackerContentModule passes the query in the constructor, and when the
				// module is attached(), a flushing refresh is initiated.
				if(mode & LiveNodeModel::Flush)
				{
					abort();

					setRunning(true);

					PERF_EVENT("sequencing %d", query_id_);

					// will sequence the call to "sequencedStart"
					// should add sequencing by query_id as a quality, and possibly depend on a pending
					// query, if one cant be aborted for some reason
					QSharedPointer<TrackerContentModule> self
							= sharedFromThis<TrackerContentModule>();
					sequenced_ = service_->query_sequencer_.addOperation
							( tracker_access_->sequencingQualities()
							, tracker_access_->sequencingDependencies()
							, SequencerSlotCall::shared(self, "sequencedStart")
							, SequencerSignal::shared(self, SIGNAL(sequencedSuccess()))
							, SequencerSignal::shared(self, SIGNAL(sequencedFailure())));

					ret = NothingReady;
				}

				// second step of refresh: if blocking, wait for all necessary steps for
				// query to complete
				if((mode & LiveNodeModel::Block) && running())
				{
					if(sequenced_ && !pending_sequenced_)
					{
						pending_sequenced_ = true;
						QSharedPointer<SequencedOperation> seq = sequenced_;

						PERF_EVENT("before_sequence_wait %d", query_id_);
						// wait for sequencedStart to be called if not already so.
						// During the wait we might get aborted. Make as little assumptions
						// as possible
						seq->waitForStarted();
						PERF_EVENT("after_sequence_wait %d", query_id_);

						if(seq->state() > SequencedOperation::Succeeded)
							// failed (typically aborted)
							model()->emitErrorImpl(seq->stateString(), 0, QModelIndex(), false);

						pending_sequenced_ = false;
					}

					// The wait is here because TrackerUpdateModule might have multi-step
					// completion cycle.
					tracker_access_->waitForQueryComplete();
				}

				// third step of refresh: if a query has completed, process the results
				TrackerQueryResult queryResult(tracker_access_->takeQueryResult());
				if(queryResult.isFinished())
				{
					setRunning(false);
					sequenced_.clear();

					if(queryResult.isError())
					{
						QString error_message = queryResult.errorMessage();

						if(RowStoreModel *m = model())
							m->emitErrorImpl(error_message, 0, QModelIndex(), !(mode & LiveNodeModel::Block));

						warning(service_.data()) << "error while modeling query (id =" << query_id_
												  << "):\n" << error_message;
						debug(4, service_.data()) << "   query was:\n" << getQueryText() << "\n";

						sharedFromThis(), Q_EMIT sequencedFailure();

						ret = NothingReady;
					} else
					{
						ret = processResults(queryResult);
					}
				}

				return ret;
			}

			void TrackerContentModule::sequencedStart()
			{
				QString query = getQueryText();

				PERF_EVENT("before_query %d", service_->query_id_counter);


				tracker_access_->startQuery(query);

				connect(tracker_access_.data(), SIGNAL(queryCompleted()), SLOT(queryCompleted()));

				query_id_ = service_->query_id_counter++;
				debug(2, service_.data()) << "modeling query (id =" << query_id_ << "):\n"
								   << query << "\n";
			}

			void TrackerContentModule::queryCompleted()
			{
				if(tracker_access_->isWaitingForQueryComplete())
					return;
				if(RowStoreModel *m = model())
					m->refreshModelSignal();
				else
					// we have been detached, most likely aborted. But as we're still alive, it
					// means we're still being sequenced and must emit sequencedSuccess or failure.
					refreshModel_chain(LiveNodeModel::Poll, InterfaceChain<ModelCommons>());
			}

			ModelCommons::ReadyFlags TrackerContentModule::processResults(TrackerQueryResult const &queryResult)
			{
				QVector<QStringList> reply_data(queryResult.selectResult());

				if(RowStoreModel *m = model())
				{
					PERF_EVENT("before_transitionrows %d", query_id_);

					m->transitionRowsImpl
						(0, model()->row_store.size(), RowStore(reply_data));

					PERF_EVENT("after_transitionrows %d", query_id_);

					debug(2, service_.data()) << "done modeling query (id =" << query_id_ << "):";
					debug(4, service_.data()) << "   query was:\n" << getQueryText() << "\n";
					debug(5, service_.data()) << "   raw data:\n" << reply_data;
					debug(3, service_.data()) << "   results:\n" << *m;
				} else
				{
					debug(2, service_.data()) << "finalized detached model query (id =" << query_id_ << "):";
					debug(4, service_.data()) << "   query was:\n" << getQueryText() << "\n";
					debug(3, service_.data()) << "   raw data:\n" << reply_data;
				}

				sharedFromThis(), Q_EMIT sequencedSuccess();

				return AllReady;
			}


			bool TrackerContentModule::alterStrategy_chain
					( RDFStrategyFlags flags, RDFStrategyFlags mask, QModelIndex const &index
					, InterfaceChain<ModelCommons> chain)
			{
				debug(5, model(), "TrackerContentModule::alterStrategy") << flags << mask
																		 << model()->strategy();

				// diagnostics. Handle Running.
				RDFStrategyFlags running_resp = mask & RDFStrategy::Running;

				// delegation
				if(!ContentModule_Simple::alterStrategy_chain
						(flags, mask & ~running_resp, index, chain))
					return false;

				// implementation
				if(running_resp)
				{
					if(!(flags & RDFStrategy::Running))
					{
						debug(5, model(), "TrackerContentModule::alterStrategy")
							<< "TrackerContentModule::alterStrategy: disabling Running, stopping query";

						abort();

						setRunning(false);
					} else
					if(!(model()->strategy_ & RDFStrategy::ModelReady))
					{
						debug(5, model(), "TrackerContentModule::alterStrategy")
							<< flags << mask
							<< "enabling Running, executing query";
						model()->refreshModel();
					}
				}
				return true;
			}


			QSharedPointer<ContentModule> TrackerContentModule::copy() const
			{
				return QSharedCreator<TrackerContentModule>::create(*this);
			}

			TrackerUpdateModule::TrackerUpdateModule
					( QSharedPointer<TrackerService> service
					, RDFDerivedSelect const& columns
					, RDFUpdate const &update
					, bool flush_on_attach, bool batch_mode, bool sync_on_commit
					, ContentModule::ChangePolicy change_policy)
				: TrackerContentModule
						( service, QSharedCreator<TrackerAccessDbus>::create
								(service, batch_mode ? MUTE_UPDATE : UPDATE)
						, columns, flush_on_attach, change_policy)
				, update_(update)
				, batch_mode_(batch_mode)
				, sync_on_commit_(sync_on_commit)
				, my_index_(0)
				, tracker_access_dbus_(static_cast<TrackerAccessDbus*>(tracker_access_.data()))
			{
				debug(4, service_.data()) << "TrackerUpdateModule TUM(" << this << ") created";
			}

			TrackerUpdateModule::TrackerUpdateModule(TrackerUpdateModule const &cp)
				: TrackerContentModule(cp)
				, update_(cp.update_)
				, batch_mode_(cp.batch_mode_), sync_on_commit_(cp.sync_on_commit_)
				, my_index_(0)
			{
				debug(4, service_.data()) << "TrackerUpdateModule TUM(" << this << ") copied"
						" from TUM(" << &cp << ")";
			}

			TrackerUpdateModule::~TrackerUpdateModule()
			{
				// Remove this module from the sister lists of the sister modules
				foreach(TrackerUpdateModule *sister, sister_modules_)
				{
					if (sister != 0)
						sister->sister_modules_[my_index_] = 0;
				}
				debug(4, service_.data()) << "TrackerUpdateModule TUM(" << this << ") destroyed";
			}

			QString TrackerUpdateModule::getQueryText() const
			{
				return update_.getQuery(reserved_sparql_words);
			}

			void TrackerUpdateModule::abort()
			{
				// we are a write operation, and due to synchronization purposes
				// we should not be aborted before we reach completion.
				// We shouldn't be refreshed either.
			}

			void TrackerUpdateModule::setSisterModules
					( QList<TrackerUpdateModule*> const &sister_modules, int ix)
			{
				sister_modules_ = sister_modules;
				my_index_ = ix;
			}

			// If none of the sister models has initiated an asynchronous call over D-Bus, do it
			// now. If somebody has, just start watching it.
			void TrackerUpdateModule::sequencedStart()
			{
				if(!tracker_access_dbus_->content_reply_)
				{
					// This is the first sister module to initiate the creation of the
					// pending call.
					QString query = getQueryText();

					if (batch_mode_)
					{
						// First call the SparqlBatchUpdate, then BatchCommit. The update is
						// finished when BatchCommit returns.
						tracker_access_dbus_->content_reply_ = QSharedPointer<QDBusPendingReply<> >(new QDBusPendingReply<>
								(service_->resources.BatchSparqlUpdate(query)));

						// TODO: verify following:
						// Assuming that when content reply arrives, all data is written to storage.
						// Calling BatchCommit as it was the previous behaviour before recent changes
						// in tracker behavior.
						service_->resources.BatchCommit();
						tracker_access_dbus_->completion_watcher_ = QSharedPointer<QDBusPendingCallWatcher>
								(new QDBusPendingCallWatcher(*tracker_access_dbus_->content_reply_));

						// TODO: why does BatchCommit give timeout?
						// completion_watcher_ = QSharedPointer<QDBusPendingCallWatcher>
						//		(new QDBusPendingCallWatcher(service_->resources.BatchCommit()));

					} else
					{
						// Normal mode; just call SparqlUpdate. The update is finished when it
						// returns.
						tracker_access_dbus_->content_reply_ = QSharedPointer<QDBusPendingReply<> >(new QDBusPendingReply<>
								(service_->resources.SparqlUpdateBlank(query)));
						tracker_access_dbus_->completion_watcher_ = QSharedPointer<QDBusPendingCallWatcher>
								(new QDBusPendingCallWatcher(*tracker_access_dbus_->content_reply_));
					}

					connect(tracker_access_dbus_->completion_watcher_.data()
							, SIGNAL(finished(QDBusPendingCallWatcher*))
							, sync_on_commit_
									? SLOT(runSync())
									: SLOT(queryCompleted()));

					// Give the same watcher to the sister models as a shared pointer
					for(QList<TrackerUpdateModule*>::const_iterator csmi = sister_modules_.begin()
							, csmiend = sister_modules_.end(); csmi != csmiend; ++csmi)
					{
						if(*csmi != 0 && (*csmi != this))
						{
							(*csmi)->tracker_access_dbus_->content_reply_ = tracker_access_dbus_->content_reply_;
							(*csmi)->tracker_access_dbus_->completion_watcher_ = tracker_access_dbus_->completion_watcher_;
							if(!sync_on_commit_)
								(*csmi)->connect(tracker_access_dbus_->completion_watcher_.data()
										, SIGNAL(finished(QDBusPendingCallWatcher*))
										, SLOT(queryCompleted()));
						}
					}
					query_id_ = service_->update_id_counter++;
					debug(2, service_.data()) << (sync_on_commit_
												  ? "modeling and syncing"
												  : "modeling")
											  << "update query (update id =" << query_id_ << "):\n"
											  << query << "\n";
				}
			}

			void TrackerUpdateModule::runSync()
			{
				service_->sync_id_counter++;
				tracker_access_dbus_->completion_watcher_ = QSharedPointer<QDBusPendingCallWatcher>
								(new QDBusPendingCallWatcher(service_->resources.Sync()));
				foreach(TrackerUpdateModule *sister, sister_modules_)
				{
					if(sister != 0)
					{
						sister->tracker_access_dbus_->completion_watcher_ = tracker_access_dbus_->completion_watcher_;
						sister->connect
								( tracker_access_dbus_->completion_watcher_.data()
								, SIGNAL(finished(QDBusPendingCallWatcher*))
								, SLOT(queryCompleted()));
					}
				}
			}

			ModelCommons::ReadyFlags TrackerUpdateModule::processResults(TrackerQueryResult const &queryResult)
			{
				RowStoreModel *m = model();
				if(m && !batch_mode_)
				{
					// If the update contained blank nodes, the result contains the
					// generated ID's for those.

					// D-Bus signature of the result: aaa{ss}
					// a{ss} is a map: blank node id -> generated uri
					// middle "a" is because an insert block may have multiple solutions
					// highest-level "a" is because the query may have multiple insert
					// statements

					UpdateResultOperations const result = queryResult.updateResult();

					if(result.size() > my_index_)
					{
						setUpdatePending(true);
						// This TrackerUpdatemodule needs to handle the insert statement
						// with the index my_index_.
						UpdateResultSolutions const generated_blanks = result[my_index_];
						if(generated_blanks.size() > 0)
						{
							QStringList keys;
							// We assume that all the maps have the same keys
							// = blank node names. They will be the columns of the model.
							foreach(RDFDerivedColumn const &dc, m->columns_.columns())
								keys << dc.name();

							QVector<QStringList> data_for_model(generated_blanks.size());

							for (int i=0; i<generated_blanks.size(); ++i)
							{
								UpdateResultBlanksMap const solution = generated_blanks[i];
								foreach(const QString& key, keys)
								{
									data_for_model[i] << solution[key];
								}
							}

							m->transitionRowsImpl
								(0, m->row_store.size(), RowStore(data_for_model));
						}
					}
				}

				debug(2, service_.data()) << "done modeling query (id =" << query_id_ << "):";
				debug(4, service_.data()) << "   query was:\n" << getQueryText() << "\n";
				if(m)
					debug(3, service_.data()) << "   results:\n" << *m;

				sharedFromThis(), Q_EMIT sequencedSuccess();

				return AllReady;
			}

			TCM_TextQuery::TCM_TextQuery
					( QSharedPointer<TrackerService> service
					, QSharedPointer<TrackerAccess> tracker_access
					, QString const &query_text
					, RDFDerivedSelect const &columns, bool flush_on_attach)
				: TrackerContentModule(service, tracker_access, columns, flush_on_attach)
				, query_text_(query_text)
			{}

			QSharedPointer<TCM_TextQuery> TCM_TextQuery::create
					( QSharedPointer<TrackerService> service
					, QSharedPointer<TrackerAccess> tracker_access
					, QString const &query_text
					, QStringList const &columns, bool flush_on_attach)
			{
				RDFDerivedSelect sel;
				for(QStringList::const_iterator fi = columns.begin(); fi != columns.end(); ++fi)
					sel.addColumn(RDFVariable(*fi));
				return QSharedCreator<TCM_TextQuery>::create
						(service, tracker_access, query_text, sel, flush_on_attach);
			}

			QSharedPointer<ContentModule> TCM_TextQuery::copy() const
			{
				return QSharedCreator<TCM_TextQuery>::create(*this);
			}

			QString TCM_TextQuery::getQueryText() const
			{
				return query_text_;
			}


			TrackerService::TrackerService()
				: RDFServiceBasic("Tracker", Contexts::QtTracker::iriList())
				, resources(service_g, resources_path_g, trackerBus(), this)
				, id_counter(0), query_id_counter(0), update_id_counter(0), sync_id_counter(0)
				, local_service_(Local::LocalService::getRDFGraphCache(false, true))
				, id2iri_()

			{
				stem = Contexts::QtTracker::encodedIri();

				service_context_data_->select_prototype_ = RDFTrackerSelectPrivate::prototype();
				service_context_data_->update_prototype_ = RDFTrackerUpdatePrivate::prototype();

				if(graph_update_signals_g)
					graph_dispatchers_.reset(new QMap<QString, GraphClassUpdateDispatcherPtr>());

				setDefaultTimeout(-1);
				setServiceAttribute("tracker_access_method", QString("DBUS"));

				if(global_log_verbosity >= 5)
				{
					connect(&query_sequencer_
							, SIGNAL(started(SequencedOperation *))
							, SLOT(sequencedStarted(SequencedOperation *)));

					connect(&query_sequencer_
							, SIGNAL(succeeded(SequencedOperation *))
							, SLOT(sequencedSucceeded(SequencedOperation *)));

					connect(&query_sequencer_
							, SIGNAL(failed(SequencedOperation *))
							, SLOT(sequencedFailed(SequencedOperation *)));
				}

				loadTrackerPrefixes();
			}

			TrackerService::~TrackerService()
			{
			}

			QUrl TrackerService::serviceIri() const
			{
				return QUrl("http://www.tracker-project.org/");
			}

			QSharedPointer<TrackerAccess> TrackerService::createTrackerAccess(AccessType queryType)
			{
				QVariant attr = serviceAttribute("tracker_access_method");
				QString access_method;
				if(attr.isValid())
					access_method=attr.toString();

				if(access_method=="QSPARQL_DIRECT")
					return QSharedCreator<TrackerAccessQSparql>::create
							(sharedFromThis<TrackerService>(), queryType);
				else
					return QSharedCreator<TrackerAccessDbus>::create
							(sharedFromThis<TrackerService>(), queryType);
			}

			bool TrackerService::waitForFinishedPendingCall
					( QDBusPendingCall &pending_call
					, QEventLoop::ProcessEventsFlags flags) const
			{
				QDBusPendingCallWatcher watcher(pending_call);
				QEventLoop loop;
				QObject::connect(&watcher, SIGNAL(finished(QDBusPendingCallWatcher *))
						, &loop, SLOT(quit()));
				loop.exec(flags);
				return true;
			}

			ClassUpdateDispatcher *TrackerService::dispatcherFromClass(QString const &class_uri)
			{
				QString2Dispatcher::iterator i = class2dispatcher.find(class_uri);
				if(i == class2dispatcher.end())
				{
					QPair<QString, QString> onto_class = splitToPrefixSuffix(class_uri);
					ClassUpdateDispatcher *disp;
					if(onto_class.first.size())
					{
						disp = graph_dispatchers_
								? ((*graph_dispatchers_)[class_uri] = (new GraphClassUpdateDispatcher
										(this, class_uri, onto_class.first, onto_class.second))
										->initialSharedFromThis<GraphClassUpdateDispatcher>()).data()
								: ClassUpdateDispatcher::create
									(class_uri, onto_class.first, onto_class.second);
					}
					else
						warning() << "while registering class signals: "
							"no ontology prefix registered for" << class_uri;
					i = class2dispatcher.insert(class_uri, disp);
				}
				if(graph_dispatchers_)
					static_cast<GraphClassUpdateDispatcher *>(*i)->
							setCompatibilitySignalMode(true);
				return *i;
			}
			ClassUpdateDispatcher *TrackerService::dispatcherFromProperty(QString const &property)
			{
				QString2Dispatcher::iterator i = property2dispatcher.find(property);
				if(i == property2dispatcher.end())
				{
					QList<LiveNode> domainresult
							= tracker()->modelVariable
								(RDFVariable(QUrl(property)).property<rdfs::domain>());
					QString domain = domainresult.size() ? domainresult[0].toString() : QString();
					ClassUpdateDispatcher *disp;
					if(domain.size())
						disp = dispatcherFromClass(domain);
					else
						warning() << "QtTracker: while registering class signals: "
							"predicate" << property << "has no domain";
					i = property2dispatcher.insert(property, disp);
				}
				return *i;
			}

			QVariant TrackerService::serviceAttribute(QString const &attribute_name) const
			{
				if(attribute_name == "query_count")
					return QVariant(query_id_counter);
				else if(attribute_name == "update_count")
					return QVariant(update_id_counter);
				else if(attribute_name == "sync_count")
					return QVariant(sync_id_counter);
				return RDFServiceBasic::serviceAttribute(attribute_name);
			}

			QVariantMap TrackerService::serviceAttributes() const
			{
				QVariantMap ret = RDFServiceBasic::serviceAttributes();
				ret["query_count"] = QVariant(query_id_counter);
				ret["update_count"] = QVariant(update_id_counter);
				ret["sync_count"] = QVariant(sync_id_counter);
				return ret;
			}

			int trackerDefaultTimeout()
			{
				return TrackerService::get()->defaultTimeout();
			}

			int TrackerService::defaultTimeout() const
			{
				return attributes_["tracker_timeout"].toInt();
			}

			void setTrackerDefaultTimeout(int timeout)
			{
				// persist tracker if default timeout is retrieved before client
				// calls ::tracker() explicitly or implicitly
				::tracker();
				TrackerService::get()->setDefaultTimeout(timeout);
			}

			void TrackerService::setDefaultTimeout(int timeout)
			{
				attributes_["tracker_timeout"] = QVariant(timeout);
			}

			QStringList TrackerService::inferNotifyTypes(RDFVariable const &resource) const
			{
				QStringList ret;

				QVector<RDFProperty> deriveds = resource.derivedProperties();
				for(QVector<RDFProperty>::const_iterator di = deriveds.constBegin()
											, diend = deriveds.constEnd(); di != diend; ++di)
					if( (di->strategy() & RDFStrategy::Object)
							&& di->target().metaIsDefinite()
							&& di->target().metaStrategy() & Notify)
						// TODO: assumes all types are partial information types, add support
						// for distinguishing a fully defining type from partial types
						ret << di->target().metaValue().toString();
				return ret;
			}

			TripleUpdateContext::Mode TrackerService::registerForTripleUpdates
					(TripleUpdateContextPtr const &context)
			{
				// variables describing the desired triples. We're mostly
				// interested in the types of subj, which are stored in
				// subject's derived properties to get the proper class handler
				RDFVariable subj = context->triple_mask_.subject();
				RDFVariable pred = context->triple_mask_.predicate();
				RDFVariable obj = context->triple_mask_.object();

				QString pred_str, subj_str;
				QStringList notify_types;

				ClassUpdateDispatcher::Events events;

				if(pred.metaValue() == rdf::type::iri())
				{
					if(obj.metaStrategy() & Notify)
						notify_types << obj.metaValue().toString();
					events = ClassUpdateDispatcher::AddsAndRemoves;
				} else
				{
					if(subj.metaIsDefinite())
						subj_str = subj.metaValue().toString();
					if(pred.metaIsDefinite())
					{
						pred_str = pred.metaValue().toString();
						events = ClassUpdateDispatcher::Updates;
					} else
						events = ClassUpdateDispatcher::AllEvents;
				}

				// << "Registering signals for "
				//		"triples(" << subj_str << ", " << pred_str << ", -)";

				if(!notify_types.size()
						&& !(notify_types = inferNotifyTypes(subj)).size()
					// && ! make dynamic query for type
					// TODO: handle synchronization issues! The registration for updates must
					// happen before the query actual gets executed, otherwise we might miss
					// some changes
					)
				{
					// TODO: output error?
					// << "No signaler found";
					return TripleUpdateContext::Inactive;
				}

				// << "Signaler found from types(" << notify_type << ")";

				foreach(QString const &notify_type, notify_types)
				{
					dispatcherFromClass(notify_type)->registerForEvents
							(context, events, subj_str, pred_str);
				}

				return context->mode_;
			}

			void TrackerService::sequencedStarted(SequencedOperation *operation)
			{
				debug(5) << "sequence started" << *operation;
			}

			void TrackerService::sequencedSucceeded(SequencedOperation *operation)
			{
				debug(5) << "sequence succeeded" << *operation;
			}

			void TrackerService::sequencedFailed(SequencedOperation *operation)
			{
				debug(5) << "sequence failed" << *operation;
			}

			void TrackerService::loadTrackerPrefixes()
			{
				loadPrefixes();
				// TODO: replace hard-coded prefixes with something read from a file, or
				//  included from a build-time autogenerated header

				const char *ontologies[][2] =
				{
					{ "nrl", "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#" }
					// nepomuk ontologies http://www.semanticdesktop.org/ontologies/
				,	{ "nrl", "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#" }
				,	{ "nao", "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#" }
				,	{ "nie", "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#" }
				,	{ "nfo", "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#" }
				,	{ "nco", "http://www.semanticdesktop.org/ontologies/2007/03/22/nco#" }
				,	{ "nmo", "http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#" }
				,	{ "ncal", "http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#" }
				,	{ "nexif", "http://www.semanticdesktop.org/ontologies/2007/05/10/nexif#" }
				,	{ "nid3", "http://www.semanticdesktop.org/ontologies/2007/05/10/nid3#" }
				,	{ "pimo", "http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#" }
				,	{ "tmo", "http://www.semanticdesktop.org/ontologies/2008/05/20/tmo#" }
				,	{ "nexif", "http://www.semanticdesktop.org/ontologies/2007/05/10/nexif#" }

					// tracker ontologies
				,	{ "nmm", "http://www.tracker-project.org/temp/nmm#" }
				,	{ "mfo", "http://www.tracker-project.org/temp/mfo#" }
				,	{ "mto", "http://www.tracker-project.org/temp/mto#" }
				,	{ "tracker", "http://www.tracker-project.org/ontologies/tracker#" }
				,	{ "fts", "http://www.tracker-project.org/ontologies/fts#" }
				,	{ "poi", "http://www.tracker-project.org/ontologies/poi#" }

					// maemo ontologies
				,	{ "maemo", "http://maemo.org/ontologies/tracker#" }

					// misc ontologies
				,	{ "exif", "http://www.kanzaki.com/ns/exif#" }
				,	{ "protege", "http://protege.stanford.edu/system#" }
				,	{ "xesam", "http://freedesktop.org/standards/xesam/1.0/core#" }
				};
				for(unsigned i = 0; i < sizeof(ontologies) / sizeof(const char*[2]); ++i)
					addPrefix(ontologies[i][0], QUrl(ontologies[i][1]));
			}

			// graphs not supported
			QVector<QStringList> TrackerService::rawExecuteQueryOnGraph
					( QString const &query, int type, RDFGraphImplBase const &/*graph*/)
			{
				if(!resources.isValid())
				{
					warning() << resources.lastError().message();
					return QVector<QStringList>();
				}

				if(type != RDFQuery::SPARQL
						&& type != RDFQuery::SPARQLUpdate && type != BatchSPARQLUpdate)
				{
					warning() << "unrecognized query type";
					return QVector<QStringList>();
				}

				debug(2) << "executing"  << (type == RDFQuery::SPARQL
											 ? "select"
											 : (type == RDFQuery::SPARQLUpdate ? "update" : "batch update"))
						 << "query:" << query;

				QVector<QStringList> ret;

				QString reply_error = QString();
				if(type == RDFQuery::SPARQL)
				{
					QDBusPendingReply<QVector<QStringList> > reply = resources.SparqlQuery(query);

					waitForFinished(reply);

					if(reply.isError())
						reply_error = reply.error().message();
					ret = reply.value();
				} else
				if(type == RDFQuery::SPARQLUpdate || type == BatchSPARQLUpdate)
				{
					QDBusPendingReply<> reply =
						(type == RDFQuery::SPARQLUpdate
								? resources.SparqlUpdate(query)
								: resources.BatchSparqlUpdate(query));
					waitForFinished(reply);
					if(reply.isError())
						reply_error = reply.error().message();
				}

				if(reply_error.size())
				{
					warning() << "error while executing"
							  << (type == RDFQuery::SPARQL ? "select" : "update")
							  << "query: " << reply_error;
					debug(4) << "   query was:\n" + query;
				}
				else
					debug(2) << "   query results:" << ret;
				return ret;
			}

			void TrackerService::rawInsertTripleToGraph
					( QString const &subject, QString const &predicate, QString const &object
					, RDFGraphImplBase const &/*graph*/)
			{
				Q_UNUSED(subject); Q_UNUSED(predicate); Q_UNUSED(object);
				critical() << "deprecated API: not implemented";
				return;
			}

			void TrackerService::rawDeleteTripleFromGraph
					( QString const &subject, QString const &predicate, QString const &object
					, RDFGraphImplBase const &/*graph*/)
			{
				Q_UNUSED(subject); Q_UNUSED(predicate); Q_UNUSED(object);
				critical() << "deprecated API: not implemented";
				return;
			}

			void TrackerService::rawLoadOnGraph
					( QUrl const &file_uri, RDFGraphImplBase const &/*graph*/)
			{
				if(!resources.isValid())
				{
					warning() << resources.lastError().message();
					return;
				}

				QDBusPendingReply<> reply = resources.Load(file_uri.toString());
				if(global_log_verbosity > 2)
				{
					debug(2) << "Loading uri: " << file_uri;
					waitForFinished(reply);
					if(reply.isError())
						warning() << "while executing load(" << file_uri << "), tracker error: "
								  << reply.error().message();
				}
			}

			QUrl TrackerService::createUniqueIri(QString stem_text)
			{
				uuid_t uuid;
				uuid_generate(uuid);
				char uuid_string[37];
				uuid_unparse(uuid, uuid_string);
				return stem + stem_text + "/" + QString(uuid_string);
			}

			TrackerResource *TrackerService::newLiveResource(Node const &node)
			{
				return new TrackerResource(sharedFromThis<RDFGraphBasic>(), node);
			}

			Node TrackerService::rawNodeFromString(QString string, CppType type)
			{
				return nodeFromString(string, type);
			}

			QString TrackerService::rawStringFromNode(Node const &node)
			{
				return stringFromNode(node);
			}

			RDFStrategyFlags TrackerService::preprocessQuery(RDFStrategyFlags strategy)
			{
				// nullify all requested features with Allow specification
				return strategy & ~RDFStrategy::AllowsMask
						& ~(strategy >> (RDFStrategy::offset_AllowStreaming
								- RDFStrategy::offset_Streaming));
			}

			LiveNodes TrackerService::modelQuery
					( QString const &select, QStringList const &columns
					, RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent
					, TripleModulePtr const &triple_module)
			{
				strategy = preprocessQuery(strategy);

				TripleModelPtr ret = newTripleModel
					// tracker backend doesn't implement any strategies
					( 0
					, TCM_TextQuery::create
						( sharedFromThis<TrackerService>()
						, createTrackerAccess(READ)
						, select, columns
						, false)
					, triple_module
					, parent
					);

				ret->alterStrategy(strategy, RDFStrategy::Running);

				return LiveNodes(ret);
			}

			LiveNodeModelBasePtr TrackerService::baseImplModelQuery
					( RDFDerivedSelect *select, RDFStrategyFlags strategy
					, QSharedPointer<QObject> const &parent
					, TripleModulePtr const &triple_module)
			{
				strategy = preprocessQuery(strategy);

				TripleModelPtr ret = newTripleModel
					( 0
					, QSharedCreator<TrackerContentModule>::create
						( sharedFromThis<TrackerService>()
						, createTrackerAccess(READ)
						, RDFDerivedSelect().swap(*select)
						, false
						)
					, triple_module
					, parent
					);

				ret->alterStrategy(strategy, RDFStrategy::Running);

				return ret;
			}

			QList<LiveNodes> TrackerService::executeQuery
					( RDFQuery const &update, RDFStrategyFlags strategy
					, QSharedPointer<QObject> const &parent)
			{
				Q_UNUSED(parent);

				// FIXME: MAJOR, this function and functionality underneath should go
				// considerable refactoring


				if(transaction_)
				{
					// We have an active transaction; don't execute the update
					// now but add it to the transaction.
					RDFUpdate const &real_update = update.toRDFUpdate();
					if(!real_update.isEmpty())
					{
						transaction_->update_.appendUpdate(real_update);
					} else
						warning() << "TrackerService::executeQuery: "
								"executing select queries not supported, returning null";
					// TODO: we should return something that can emit completion signals
					// even if we're in a tx as that's perfectly viable
					return QList<LiveNodes>();
				}

				if(strategy &
						(LiveNodeModel::Streaming | LiveNodeModel::Windowed | RDFStrategy::Live))
					warning() << "executeQuery: requested strategy (streaming, windowed or live)"
						" not supported, falling back to DefaultStrategy";

				RDFUpdate const &real_update = update.toRDFUpdate();
				// TODO: only support sparql update, fix.
				// TODO: also, handle the transaction more cleanly.
				if(real_update.isEmpty())
					warning() << "TrackerService::executeQuery: "
							"executing select queries not supported, returning null";

				int operation_count = real_update.operations().size();
				// if update has no operations, dont perform any queries
				if(!operation_count)
				{
					warning() << "TrackerService::executeQuery: null update, returning empty list";
					return QList<LiveNodes>();
				}

				RDFUpdate tracker_update = service_context_data_->update(real_update);

				if(strategy & RDFStrategy::DiscardUpdateResults)
				{
					rawExecuteQueryOnGraph
						( tracker_update.getQuery(reserved_sparql_words, RDFQuery::SPARQLUpdate)
						, (strategy & BatchMode)
							? int(BatchSPARQLUpdate)
							: int(RDFQuery::SPARQLUpdate)
						, *this);
					if(strategy & SyncOnCommit)
						// TODO: add support for direct blocking updates?
						sync(false);
					return QList<LiveNodes>();
				}

				// If we're doing batch update, no data will be returned as models.
				// Just create one model and a TrackerUpdateModule for it.
				if (strategy & BatchMode)
				{
					LiveNodes ret = LiveNodes(newTripleModel
							( strategy & 0
							, QSharedCreator<TrackerUpdateModule>::create
									( sharedFromThis<TrackerService>()
									, RDFDerivedSelect()
									, tracker_update
									, true
									, strategy & BatchMode
									, strategy & SyncOnCommit)
							, TripleModulePtr()
							, parent));
					return QList<LiveNodes>() << ret;
				}

				QList<RDFUpdate::Operation> ops = tracker_update.operations();

				// The modules cannot contain QSharedPointers to each other, otherwise they would
				// keep each other alive. Hence this complication.
				// TODO: replace with weak pointers?
				QList<QSharedPointer<TrackerUpdateModule> > update_modules;
				QList<TrackerUpdateModule*> update_modules_raw;
				for (int i = 0; i < operation_count; ++i)
				{
					// Check which are the blank variables in the operation; they will be the
					// columns of the returned model
					RDFDerivedSelect columns;
					QSet<QString> blank_variables;
					QList<RDFVariableStatement> inserts = ops[i].inserts();
					foreach(const RDFVariableStatement& stmt, inserts)
					{
						RDFVariable subject = stmt.subject();
						RDFVariable object = stmt.object();
						if (subject.metaIsExplicit() && !subject.metaIsDefinite())
						{
							blank_variables << subject.metaValue().toString();
						}
						if (object.metaIsExplicit() && !object.metaIsDefinite())
						{
							blank_variables << object.metaValue().toString();
						}
					}
					// create a new model only if we're the first model or
					// if the operation has insertion blanks in it
					if(blank_variables.size() || i == 0)
					{
						foreach (const QString& blank, blank_variables)
						{
							columns.addColumn(blank, RDFVariable());
						}
						update_modules << QSharedCreator<TrackerUpdateModule>::create(
							sharedFromThis<TrackerService>(),
							columns,
							tracker_update,
							true,
							strategy & BatchMode,
							strategy & SyncOnCommit);
						update_modules_raw << update_modules[i].data();
					} else
					{
						update_modules << QSharedPointer<TrackerUpdateModule>();
						update_modules_raw << 0;
					}
				}

				QList<LiveNodes> models;
				for (int i = 0; i < operation_count; ++i)
					if(TrackerUpdateModule *active = update_modules_raw[i])
					{
						// Each TrackerUpdateModule has its own model (where it will eventually insert
						// the data), but the data comes from the same source.  Hence, the modules need
						// to know about each other.
						active->setSisterModules(update_modules_raw, i);
						models << LiveNodes(newTripleModel(strategy & 0, update_modules[i],
														   TripleModulePtr(),
														   parent));
					} else
						models << LiveNodes();
				return models;
			}

			void TrackerService::addRemoveResourceClause
					(RDFStatementList *removed_statements, RDFVariable const &res)
			{
				removed_statements->push_back(RDFStatement(res, rdf::type::iri(), rdfs::Resource::iri()));
				QVector<RDFProperty> const dp = res.derivedProperties();
				for(QVector<RDFProperty>::const_iterator cdpi = dp.begin(), cdpiend = dp.end()
						; cdpi != cdpiend; ++cdpi)
					if(cdpi->strategy() & RemoveSubjectProperty)
					{
						RDFProperty prop = cdpi->deepCopy();
						RDFVariable var = res.optional().bindSubject(prop);
						removed_statements->push_back(RDFStatement(var, prop.predicate(), res));
					}
			}

			ClassUpdateSignaler *ClassUpdateSignaler::get(QUrl class_uri)
			{
				// persist tracker if a ClassUpdateSignaler is retrieved before client calls
				// ::tracker() explicitly or implicitly
				::tracker();
				return TrackerService::get()->dispatcherFromClass(class_uri.toString());
			}

			ClassUpdateSignaler *ClassUpdateSignaler::get
					(QString ontology_prefix, QString class_name)
			{
				// persist tracker if a ClassUpdateSignaler is retrieved before client calls
				// ::tracker() explicitly or implicitly
				::tracker();
				return TrackerService::get()->dispatcherFromClass(TrackerService::get()
						->prefixSuffix(ontology_prefix, class_name).toString());
			}
		}
	}
}
