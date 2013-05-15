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
 * mafw_p.cpp
 *
 *  Created on: Apr 6, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include <QVector>

#include "../../include/QtTracker/Tracker"
//#include "../../include/QtTracker/ontologies/nie.h"

#include "mafw_p.h"
#include "rdfcache_p.h"

const QString TRACKER_SOURCE_UUID = "MafwTrackerSource";
const QString ERROR_TEXT_START_FAILURE = "failed to start";

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Mafw
		{
			QString stringFromNode(Node const &node)
			{
				if(node.isLiteral())
					switch(node.literal().type())
					{
					case QVariant::Date: return node.literal().toDate().toString(Qt::ISODate);
					case QVariant::DateTime: return node.literal().toDateTime().toString(Qt::ISODate);
					case QVariant::Time: return node.literal().toTime().toString(Qt::ISODate);
					default: break;
					}
				return node.toString();
			}

			Node nodeFromString(QString const &value, CppType type = CppType())
			{
				unsigned id = type.metaTypeId();

				// XXX: maybe fuller support of user types, now assume all user types are resources
				if(id >= QVariant::UserType)
					return Node(QUrl::fromEncoded(value.toAscii(), QUrl::StrictMode));

				switch(type.metaTypeId())
				{
				case CppType::None: break;
				case QVariant::Bool: return Node(LiteralValue(value.toInt())); // TODO: check
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
				case QVariant::ByteArray: return Node(LiteralValue(value.toAscii())); // TODO: really?
				case QVariant::BitArray: break; // return Node(LiteralValue(QBitArray));
				case QVariant::Date: return Node(LiteralValue(QDate::fromString(value, Qt::ISODate)));
				case QVariant::Time: return Node(LiteralValue(QTime::fromString(value, Qt::ISODate)));
				case QVariant::DateTime: return Node(LiteralValue(QDateTime::fromString(value, Qt::ISODate)));
				case QVariant::Url: return Node(QUrl::fromEncoded(value.toAscii(), QUrl::StrictMode));
				default: break;
				}
				return Node(LiteralValue(value));
			}

			MafwModelStrategy::MafwModelStrategy
					( QSharedPointer<Mafw> service, RDFDerivedSelect const &columns
					, bool streaming, ChangePolicy change_policy)
					: ContentModule_Simple(change_policy)
					, service_(service), columns_(columns), streaming_(streaming)
					, ongoingID_(0), waiting_(false), updatePassThrough_(false)
			{
				debug(2) << "streaming" << streaming;
			}

			MafwModelStrategy::~MafwModelStrategy()
			{
				if( ongoingID_ )
				{
					debug(2) << "implicit abortion for query ID" << ongoingID_;

					cancelQuery(ongoingID_);
					ongoingID_ = 0;
				}
				debug(2) << "destroyed";
			}

			QString MafwModelStrategy::getQueryText() const
			{
				return model()->columns_.getQuery();
			}

			void MafwModelStrategy::attached(RowStoreModel &rowstoremodel)
			{
				ContentModule_Simple::attached(rowstoremodel);
				model()->setColumnInformation(columns_);
				columns_ = RDFDerivedSelect();
				model()->refreshModel(LiveNodeModel::Flush);
			}
			void MafwModelStrategy::detached(RowStoreModel &rowstoremodel)
			{
				columns_ = model()->columns_;
				ContentModule_Simple::detached(rowstoremodel);
			}


			ModelCommons::ReadyFlags MafwModelStrategy::refreshModel_chain
					(LiveNodeModel::RefreshMode mode, InterfaceChain<ModelCommons> chain)
			{
				// with refreshModel, these might not be necessary
				if( updatePassThrough_ || waiting_)
					return RDFStrategyFlags(0);

				// base class returns AllReady in ret, we will disable that if needed
				ReadyFlags ret = ContentModule_Simple::refreshModel_chain(mode, chain);

				if(mode & LiveNodeModel::Flush)
				{
					ongoingID_ = initiateQuery();
					// query here
					if( ongoingID_ == 0 )
					{
						model()->emitErrorImpl( ERROR_TEXT_START_FAILURE );
						warning() << "query cannot be initiated! Query ID = 0";
					}
					else
					{
						setRunning(true);
						debug(2) << "Query ID = " << ongoingID_;
					}
				}

				if(ongoingID_)
				{
					waiting_=true;

					// if request is for Blocking content and we're not streaming, block
					if((mode & LiveNodeModel::Block) && !streaming_)
					{
						debug(2) << "MMS_Simple_Browse::update BLOCKING";
						while( ongoingID_ )
						{
							QCoreApplication::instance()->processEvents
									(QEventLoop::WaitForMoreEvents);
						}
						debug(2) << "MMS_Simple_Browse::update STOPPED BLOCKING";

						ret = AllReady;
					} else
						// we're not ready yet. Disable ready values.
						ret &= ~AllReady;

					waiting_=false;
				} else
					ret = AllReady;
				return ret;
			}

			uint MafwModelStrategy::initiateQuery()
			{
				return service_->resource.query
						( getQueryText(), this
						, SLOT( handleQueryResult(uint, QStringList, uint, int) )
						, SLOT( handleQueryError(uint, int, QString) ) );
			}

			bool MafwModelStrategy::alterStrategy_chain
					( RDFStrategyFlags flags, RDFStrategyFlags mask, QModelIndex const &index
					, InterfaceChain<ModelCommons> chain)
			{
				debug(2, model(), "MafwModelStrategy::alterStrategy") << model()->strategy();

				// diagnostics
				RDFStrategyFlags running_resp = mask & RDFStrategy::Running;

				if(running_resp & flags & ~model()->strategy(index))
				{
					warning(model(), "MafwModelStrategy::alterStrategy") << "re-enabling running not supported";
					return false;
				}

				// delegation
				if(!ContentModule_Simple::alterStrategy_chain
						(flags, mask & ~running_resp, index, chain))
					return false;

				// implementation
				if(running_resp & ~flags & model()->strategy(index))
				{
					debug(2, model(), "MafwModelStrategy::alterStrategy") << "turning off Running";

					model()->doDisableStrategyFlags(RDFStrategy::Running);

					if( ongoingID_ )
					{
						cancelQuery(ongoingID_);
						ongoingID_ = 0;
						model()->refreshModel(LiveNodeModel::Poll);
					}
				}
				return true;
			}

			void MafwModelStrategy::cancelQuery(uint query_id)
			{
				debug(2) << " do abortion for query ID" << query_id;
				service_->resource.cancelQuery(query_id);
			}


			void MafwModelStrategy::handleQueryResult
					( uint queryId, QStringList resultRow, uint index, int remainingCount)
			{
				Q_UNUSED( queryId );

				debug(2) << index << remainingCount << resultRow;

				if(model()->strategy() & LiveNodeModel::Running)
				{
					updatePassThrough_ = true;
					model()->replaceRowImpl(index, resultRow);
					updatePassThrough_ = false;
				}
				else
				{
					warning() << "not in Running state";
				}

				if(remainingCount <= 0 )
				{
					ongoingID_ = 0;

					model()->refreshModel(LiveNodeModel::Poll);
					model()->doDisableStrategyFlags(RDFStrategy::Running);
				}
			}

			void MafwModelStrategy::handleQueryError(uint queryId, int err, QString errorMessage )
			{
				debug(2) << queryId << err << errorMessage;

				if(ongoingID_)
				{
					ongoingID_ = 0;

					model()->refreshModel(LiveNodeModel::Poll);
					model()->doDisableStrategyFlags(RDFStrategy::Running);

					if( err ) model()->emitErrorImpl( errorMessage );
				}
			}

			MMS_Simple_TextQuery::MMS_Simple_TextQuery
					( QSharedPointer<Mafw> service, QString const &query_text
					, RDFDerivedSelect const &columns, bool streaming)
				: MafwModelStrategy(service, columns, streaming), query_text_(query_text)
			{}

			QSharedPointer<MMS_Simple_TextQuery> MMS_Simple_TextQuery::create
					( QSharedPointer<Mafw> const &service, QString const &query_text
					, QStringList const &columns, bool streaming)
			{
				RDFDerivedSelect sel;
				for(QStringList::const_iterator fi = columns.begin(); fi != columns.end(); ++fi)
					sel.addColumn(RDFVariable(*fi));
				return QSharedCreator<MMS_Simple_TextQuery>::create(service, query_text, sel, streaming);
			}

			QString MMS_Simple_TextQuery::getQueryText() const
			{
				return query_text_;
			}

			MMS_Simple_Browse::MMS_Simple_Browse
					( QSharedPointer<Mafw> service, LiveNode const &node
					, RDFVariable const &predicate, RDFVariable const &object
					, RDFDerivedSelect const &columns, bool streaming)
			: MafwModelStrategy(service, columns, streaming), node_(node)
			, predicate_(predicate), object_(object)
			, predicateStr_(predicate_.metaValue().toString())
			{
				debug(2) << "browse strategy created for LiveNode(QUrl(" << node.toString() << ")) property " << predicateStr_
					<< "in" << (streaming_ ? "STREAMING" : "BLOCKING") << "mode";

				// we only handle the base property results, thus we should have only 1 base column: the property column itself
				if(columns_.baseColumnCount() != 1)
				{
					warning() << "internal error: incorrect number of base columns";
				}

				// deal with this better. Perhaps let caching layer deal with these.
				if(columns_.derivedVirtualColumnCount() != 0)
				{
					warning() << "multiple valued properties are not supported";
				}

				// we should verify other qualities of columns as well to see what we can support
				// right now, we only support columns which are 1. objects (not subjects) 2. do not recurse
				RDFDerivedColumnList const &cols = columns_.columns();
				for(RDFDerivedColumnList::const_iterator cci = cols.begin() + columns_.baseColumnCount()
						, cciend = cols.begin() + columns_.realColumnCount()
						; cci != cciend; ++cci)
				{
					QString derived_predicate = cci->propertyData().predicate().metaValue().toString();

					enum { wrong_parent = 1 << 0, not_object = 1 << 1, multiple_valued = 1 << 2, debug = 1 << 3 };

					int error_flags
						= (cci->parent() != 0) ? wrong_parent : 0
						| ((cci->strategy() & RDFStrategy::PredicateObject) != RDFStrategy::PredicateObject) ? not_object : 0
						| (!(cci->strategy() & RDFStrategy::NonMultipleValued) ? multiple_valued : 0);

					if(error_flags)
					{
						xwarning() {
							QString derived_chain;
							RDFDerivedColumn const *col = &*cci;
							do derived_chain.prepend(
								(col->strategy() & RDFStrategy::Object ? ".derivedObject<" : ".derivedSubject<")
								+ col->propertyData().predicate().metaValue().toString() + ">()");
							while(col->parent() != -1 && (col = &cols[col->parent()]));
							log << "\terrors for derived property"
								<< "LiveNode(QUrl(" << node.toString() << "))" << derived_chain.toAscii().data() << "with value:" << object.metaValue().toString() << ":";
							if(error_flags & wrong_parent)
								log << "\tnested derived properties not supported";
							if(error_flags & not_object)
								log << "\tincorrect property type: only derived object properties supported";
							if(error_flags & wrong_parent)
								log << "multiple valued derived properties not supported";
						}
					}

					if(!error_flags)
					{
						additionalPredicates_ << derived_predicate;
					}
				}
				debug(2) << "additionalPredicates" << additionalPredicates_;
			}

			MMS_Simple_Browse::~MMS_Simple_Browse()
			{
				if( ongoingID_ )
				{
					debug(2) << "implicit abortion for browse ID" << ongoingID_;

					cancelQuery(ongoingID_);
					ongoingID_ = 0;
				}
				debug(2) << "destroyed";
			}

			void MMS_Simple_Browse::attached(RowStoreModel &rowstoremodel)
			{
				debug(2) << "Browsing: " << node_.uri() << "for" << predicateStr_;
				MafwModelStrategy::attached(rowstoremodel);
			}

			uint MMS_Simple_Browse::initiateQuery()
			{
				// browse here
				return service_->resource.browse(
						node_.iri().toEncoded(),
						predicate_.metaValue().toString(),
						additionalPredicates_,
						0,
						0,
						this,
						SLOT(handleResult(uint,QString,QMap<QString,QVariant>,uint,int)),
						SLOT(handleError(uint,uint,QString)));
			}

			void MMS_Simple_Browse::cancelQuery(uint query_id)
			{
				debug(2) << "do abortion for browse ID" << query_id;
				service_->resource.cancelBrowse(query_id);
			}


			void MMS_Simple_Browse::handleResult(uint browseId, QString subjectId, QMap<QString,QVariant> meta, uint index, int remainingCount)
			{
				Q_UNUSED(browseId);
				Q_ASSERT(subjectId.length() > 0);

				if(subjectId.contains("::"))
				{
					subjectId = subjectId.split("::")[1];
				}

				QStringList row;

				RDFDerivedColumnList const &cols = model()->columns_.refColumns();

				// at this point, assume that incoming data is correctly formed. Put sanity checks to constructor/attach.

				// there are two modes this function call be called:
				// mode 1: asking some property of subject
				// results will come with subject as subjectId and the property value inside meta
				// mode 2: getting children items of subject (e.g. asking hasLogicalParts)
				// the property value will be given in the subject id
				// and derived properties are given in meta.

				if( subjectId==node_.iri().toEncoded() )
				{
					// mode 1
					QString property = cols.first().propertyData().predicate().metaValue().toString();

					QMap<QString, QVariant>::const_iterator cpi = meta.find(property);
					if(cpi != meta.end())
						row << cpi.value().toString();
					else
					{
						warning() << "Property not received: " << property;
						row << QString();
					}
				}
				else
				{
					// mode 2
					row << subjectId;

					for(RDFDerivedColumnList::const_iterator cci = cols.begin() + 1, cciend = cols.begin() + model()->columns_.realColumnCount()
							; cci != cciend; ++cci)
					{
						// if perf intensive, these could be cached.
						QString key = cci->propertyData().predicate().metaValue().toString();

						QMap<QString,QVariant>::const_iterator derived_prop = meta.find(key);

						if(derived_prop != meta.end())
							row << derived_prop->toString();
						else
						{
							if(key.size())
								debug(2) << "Property not received: " << key;
							row << QString();
						}
					}
				}

				debug(2) << "row" << row;

				if( model()->strategy() & LiveNodeModel::Running )
				{
					updatePassThrough_ = true;
					model()->replaceRowImpl(index,row);
					updatePassThrough_ = false;
				}
				else
				{
					warning() << "but not in Running state";
				}

				if(remainingCount <= 0 )
				{
					ongoingID_ = 0;
					model()->refreshModel(LiveNodeModel::Poll);
					model()->doDisableStrategyFlags(RDFStrategy::Running);
				}

			}

			void MMS_Simple_Browse::handleError( uint browseId, uint error,
												 QString errorMessage )
			{
				Q_UNUSED(browseId);

				warning() << "code" << error << " msg: " << errorMessage;

				if(ongoingID_)
				{
					ongoingID_ = 0;
					model()->refreshModel(LiveNodeModel::Poll);
					model()->doDisableStrategyFlags(RDFStrategy::Running);

					if( error ) model()->emitErrorImpl( errorMessage );
				}
			}

			LiveNodeModelBasePtr MafwResource::implGetProperties(RDFDerivedSelect *columns, TripleModulePtr const &triple_module) const
			{
				RDFProperty const &property_data = (*columns)[0].propertyData();

				// we only handle object property strategies, check for that
				if(property_data.strategy() & RDFStrategy::Object)
				{
					RDFVariable const &predicate = property_data.predicate();
					RDFVariable const &object = property_data.target();
					RDFStrategyFlags model_strategy = property_data.strategy();
					QSharedPointer<QObject> const &parent = property_data.parent();

					debug(2) << "subject: "
							 << node_.toString()
							 << " predicate: "
							 << predicate.metaValue().toString()
							 << " predicate type: "
							 << predicate.varCppType().metaTypeId()
							 << " strategy: "
							 << hex << model_strategy << dec;

					if(Mafw *mafw = dynamic_cast<Mafw*>(graph()))
					{
						bool streaming = model_strategy & LiveNodeModel::Streaming;
						return mafw->browse(node_, predicate, object, *columns, streaming, model_strategy, parent, triple_module);
					}
				}
				return LiveNodeModelBasePtr();
			}

			Mafw::Mafw(MafwRdfResource& reso)
					: RDFServiceBasic("Mafw", Contexts::QtMafw::iriList())
					, resource(reso)
			{
				loadPrefixes();
				qRegisterMetaType<RDFStrategyFlags>("RDFStrategyFlags");
				qRegisterMetaType<QModelIndex>("QModelIndex");
			}

			LiveNodeModelBasePtr Mafw::browse(LiveNode const &node,
			                       RDFVariable const &predicate,
			                       RDFVariable const &object,
			                       RDFDerivedSelect const &columns,
			                       bool streaming,
			                       RDFStrategyFlags model_strategy,
			                       QSharedPointer<QObject> const &parent,
			                       TripleModulePtr const &triple_module)
			{
				return newTripleModel(model_strategy, QSharedCreator<MMS_Simple_Browse>::create(
				                                                                          sharedFromThis<Mafw>(),
				                                                                          node,
				                                                                          predicate,
				                                                                          object,
				                                                                          columns,
				                                                                          streaming), triple_module, parent);
			}

			QUrl Mafw::serviceIri() const { return QUrl( resource.uuid() ); }

			// graphs not supported
			QVector<QStringList> Mafw::rawExecuteQueryOnGraph(QString const &query, int type, RDFGraphImplBase const &/*graph*/)
			{
				if(type != RDFQuery::SPARQL && type != RDFQuery::SPARQLUpdate)
				{
					warning() << "QtMafw: unrecognized query type";
					return QVector<QStringList>();
				}

				warning() << "QtMafw: error while executing query: rawExecuteQueryOnGraph not implemented";
				debug(2) << "QtMafw: \twhile executing query: " << query;
				return QVector<QStringList>();
			}

			void Mafw::rawInsertTripleToGraph(QString const &subject, QString const &predicate, QString const &object, RDFGraphImplBase const &/*graph*/)
			{
				warning() << "QtMafw: error while executing insert: rawInsertTripleToGraph not implemented";
				debug(2) << "QtMafw: \twhile inserting triple: " << subject << predicate << object;
			}

			void Mafw::rawDeleteTripleFromGraph(QString const &subject, QString const &predicate, QString const &object, RDFGraphImplBase const &/*graph*/)
			{
				warning() << "QtMafw: error while executing delete: rawDeleteTripleFromGraph not implemented";
				debug(2) << "QtMafw: \twhile deleting triple: " << subject << predicate << object;
			}

			void Mafw::rawLoadOnGraph(QUrl const &file_uri, RDFGraphImplBase const &/*graph*/)
			{
				warning() << "QtMafw: error while executing load: rawLoadOnGraph not implemented";
				debug(2) << "QtMafw: \twhile Loading uri: " << file_uri.toString();
			}

			QUrl Mafw::createUniqueIri(QString stem_text)
			{
				warning() << "NOT IMPLEMENTED";
				return /*stem +*/ stem_text + /*session_id +*/ QString::number(id_counter++);
			}

			LiveNode Mafw::createLiveNode(CppType type)
			{
				debug(2) << "Mafw::createLiveNode";

				if(type.metaTypeId() == qMetaTypeId<QUrl>() || type.metaTypeId() >= QMetaType::User)
					// TODO: implement delayed creation etc.
					return liveNode(createUniqueIri("generated"));
				return liveNode(nodeFromString(QString(), type));
			}

			MafwResource *Mafw::newLiveResource(Node const &node)
			{
				return new MafwResource(sharedFromThis<Mafw>(), node);
			}


			Node Mafw::rawNodeFromString(QString string, CppType type)
			{
				return nodeFromString(string, type);
			}

			QString Mafw::rawStringFromNode(Node const &node)
			{
				return stringFromNode(node);
			}

			void Mafw::insertTripleToGraph(LiveNode const &subject, LiveNode const &predicate, LiveNode const &object, RDFGraphImplBase const &graph)
			{
				rawInsertTripleToGraph(stringFromNode(subject), stringFromNode(predicate), stringFromNode(object), graph);
			}

			void Mafw::deleteTripleFromGraph(LiveNode const &subject, LiveNode const &predicate, LiveNode const &object, RDFGraphImplBase const &graph)
			{
				rawDeleteTripleFromGraph(stringFromNode(subject), stringFromNode(predicate), stringFromNode(object), graph);
			}

			LiveNodes Mafw::modelQuery(QString const &select, QStringList const &columns, RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent, TripleModulePtr const &triple_module)
			{
				debug(2) << strategy;

				if(strategy & (RDFStrategy::Windowed | RDFStrategy::Live))
				{
					warning() << "QtMafw: RDFService model strategy not supported, falling back to DefaultStrategy";
					strategy &= ~(RDFStrategy::Windowed | RDFStrategy::Live);
				}

				bool streaming = strategy & LiveNodeModel::Streaming;

				return LiveNodes(newTripleModel(strategy, MMS_Simple_TextQuery::create
							(sharedFromThis<Mafw>(), select, columns, streaming)
						, triple_module, parent));
			}

			LiveNodeModelBasePtr Mafw::baseImplModelQuery(RDFDerivedSelect *select, RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent, TripleModulePtr const &triple_module)
			{
				debug(2, "Mafw::modelQuery") << strategy;

				if(strategy & (LiveNodeModel::Windowed | RDFStrategy::Live))
				{
					warning() << "QtMafw: RDFService model strategy not supported, falling back to DefaultStrategy";
					strategy &= ~(RDFStrategy::Windowed | RDFStrategy::Live);
				}

				bool streaming = strategy & LiveNodeModel::Streaming;

				debug(2, "Mafw::modelQuery") << "streaming:" << streaming;

				return newTripleModel(strategy, QSharedCreator<MafwModelStrategy>::create
							( sharedFromThis<Mafw>(), RDFDerivedSelect().swap(*select), streaming)
						, triple_module, parent);
			}

			QList<LiveNodes> Mafw::executeQuery(RDFQuery const &select, RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent)
			{
				Q_UNUSED(parent);

				if(strategy & (RDFStrategy::Live | LiveNodeModel::Windowed))
				{
					warning() << "QtMafw: RDFService model strategy not supported, falling back to DefaultStrategy";
				}

				rawExecuteQueryOnGraph(select.getQuery(RDFQuery::SPARQLUpdate), RDFQuery::SPARQLUpdate, *this);
				return QList<LiveNodes>();
			}

			//QThreadStorage<QSharedPointer<Mafw> *> mafw_tls;

			QSharedPointer<Mafw> Mafw::getRDFGraph(MafwRdfResource& resource)
			{
				QSharedPointer<Mafw> mafw = QSharedCreator<Mafw>::create<MafwRdfResource>(resource);
				mafw->setParent( mafw.toWeakRef() );
				return mafw;
			}

			RDFServicePtr getRDFService(MafwRdfResource& resource)
			{
				if( resource.uuid()==TRACKER_SOURCE_UUID ) {
					return SopranoLive::BackEnds::Tracker::tracker();
				}
				else
				{
					return Mafw::getRDFGraph(resource);
				}
			}

			RDFServicePtr getCachedRDFService(MafwRdfResource& resource)
			{
				QSharedPointer<Mafw> mafw = Mafw::getRDFGraph(resource);
				QSharedPointer<RDFGraphCache> cache = QSharedCreator<RDFGraphCache>::create(mafw);
				return cache;
			}

			LiveNode rootNode(RDFServicePtr service)
			{
				return service->liveNode(
						nodeFromString(QString("0"),CppType::of<QUrl>()));
			}

			QString getObjectId(const LiveNode& node)
			{
				if( !node.isResource() ) return "";

				RDFServicePtr rdfService=node->graph()->service();

				if( rdfService==SopranoLive::BackEnds::Tracker::tracker()->service() ) {
					return TRACKER_SOURCE_UUID+"::"+node.uri().toEncoded();
				}

				Mafw* backend=dynamic_cast<Mafw*>(rdfService.data());
				if( backend )
				{
					return backend->resource.uuid()+"::"+node.uri().toEncoded();
				}
				return QString();
			}

			void setMafwVerbosity(int level)
			{
				SopranoLive::setVerbosity(level);
			}
		}
	}
}
