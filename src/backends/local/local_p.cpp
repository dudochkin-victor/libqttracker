/*
 * This file is part of LibQtLocal project
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
 * local_p.cpp
 *
 *  Created on: Jul 24, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */


#include "local_p.h"

#include <uuid.h>

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Local
		{
			QThreadStorage<QWeakPointer<RDFGraphCache> *> local_tls;
			QThreadStorage<QSharedPointer<RDFGraphCache> *> local_persistent_tls;

			QSharedPointer<RDFGraphCache> LocalService::getRDFGraphCache(bool persistent, bool register_weak)
			{
				QSharedPointer<RDFGraphCache> ret;
				if(!local_tls.hasLocalData()
					// check if the weak pointer in local data is valid and store it to ret
					|| !(ret = *local_tls.localData()))
				{
					QSharedPointer<LocalService> local = QSharedCreator<LocalService>::create();
					local->setParent( local.toWeakRef() );
					ret = QSharedCreator<RDFGraphCache>::create(local);
				}
				if(persistent && !local_persistent_tls.hasLocalData())
					local_persistent_tls.setLocalData(new QSharedPointer<RDFGraphCache>(ret));
				if(persistent || register_weak)
					local_tls.setLocalData(new QWeakPointer<RDFGraphCache>(ret));
				return ret;
			}

			QSharedPointer<LocalService> LocalService::get()
			{
				return getRDFGraphCache(false)->backend().sharedFromThis<LocalService>();
			}

			RDFServicePtr local()
			{
				return LocalService::getRDFGraphCache();
			}

			RDFServicePtr service()
			{
				return LocalService::getRDFGraphCache(false);
			}

			RDFServicePtr localRelease()
			{
				if(!local_persistent_tls.hasLocalData())
					return RDFServicePtr();
				RDFServicePtr ret = *local_persistent_tls.localData();
				local_persistent_tls.setLocalData(0);
				return ret;
			}

			LMS_Simple_Select::LMS_Simple_Select(QSharedPointer<LocalService> service, RDFDerivedSelect const &select)
				: LocalModelStrategy<>(service)
				, select_(select)
			{
			}
			LMS_Simple_Select::LMS_Simple_Select(QSharedPointer<LocalService> service, QString const &query_text, QStringList const &columns)
				: LocalModelStrategy<>(service)
				, select_()
			{
				Q_UNUSED(query_text);
				for(QStringList::const_iterator cci = columns.begin(), cciend = columns.end(); cci != cciend; ++cci)
					select_.addColumn(RDFVariable(*cci));
			}
			void LMS_Simple_Select::attached(RowStoreModel &rowstoremodel)
			{
				LocalModelStrategy<>::attached(rowstoremodel);
				model()->setColumnInformation(select_);
				model()->refreshModel();
			}

			QSharedPointer<ContentModule> LMS_Simple_Select::copy() const
			{
				return QSharedCreator<LMS_Simple_Select>::create(*this);
			}

			ModelCommons::ReadyFlags LMS_Simple_Select::refreshModel_chain(LiveNodeModel::RefreshMode mode, InterfaceChain<ModelCommons> chain)
			{
				ModelCommons::ReadyFlags ret = ContentModule_Simple::refreshModel_chain(mode, chain);

				if(mode & LiveNodeModel::Flush)
				{
					setRunning(true);
					query_id_ = service->query_id_counter++;
					debug(2) << "modeling local empty query (id =" << query_id_ << "):\n " << select_.getQuery() << "\n";
				}

				if(!updatePending() && running())
				{
					QVector<QStringList> reply_value;
					QString error; // put something here if you want failing queries

					setUpdatePending(true);

					if(!error.size())
					{
						debug(2) << "done modeling local empty query (id =" << query_id_ << "):";
						debug(4) << "   query was:\n" + select_.getQuery() << "\n";
						debug(3) << "   results:\n" << reply_value;
					} else
					{
						warning() << "error while modeling query (id =" << query_id_ << "):\n" << error
								  << "   query was:\n" << select_.getQuery() << "\n";
					}
					if(!error.size())
						model()->transitionRowsImpl(0, rows_->size(), RowStore(reply_value));

					setRunning(false);
				}
				return ret;
			}

			LocalResource::LocalResource(QSharedPointer<RDFServiceImplBase> const &service, Node const &node)
				: ResourceOfService(service, node)
			{
			}

			LocalResource::~LocalResource()
			{
			}

			LocalService::LocalService()
				: RDFServiceBasic("Local", Contexts::local::iriList())
				, stem(Contexts::local::encodedIri())
				, id_counter(0), query_id_counter(0), update_id_counter(0)
			{
				loadLocalPrefixes();

				// these types are registered to enable queued error signaling in LiveNodeModelImplBase
				qRegisterMetaType<RDFStrategyFlags>("RDFStrategyFlags");
				qRegisterMetaType<QModelIndex>("QModelIndex");
			}

			LocalService::~LocalService()
			{
			}

			QUrl LocalService::serviceIri() const { return Contexts::local::iri(); }

			void LocalService::loadLocalPrefixes()
			{
				loadPrefixes();
				// TODO: replace hard-coded prefixes with something read from a file, or
				//  included from a build-time autogenerated header

				// nepomuk ontologies http://www.semanticdesktop.org/ontologies/
				addPrefix("nrl", QUrl("http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#"));
				addPrefix("nao", QUrl("http://www.semanticdesktop.org/ontologies/2007/08/15/nao#"));
				addPrefix("nie", QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#"));
				addPrefix("nfo", QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#"));
				addPrefix("nco", QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#"));
				addPrefix("nmo", QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#"));
				addPrefix("ncal", QUrl("http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#"));
				addPrefix("nexif", QUrl("http://www.semanticdesktop.org/ontologies/2007/05/10/nexif#"));
				addPrefix("nid3", QUrl("http://www.semanticdesktop.org/ontologies/2007/05/10/nid3#"));
				addPrefix("pimo", QUrl("http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#"));
				addPrefix("tmo", QUrl("http://www.semanticdesktop.org/ontologies/2008/05/20/tmo#"));
				addPrefix("nexif", QUrl("http://www.semanticdesktop.org/ontologies/2007/05/10/nexif#"));

				// local ontologies
				addPrefix("nmm", QUrl("http://www.local-project.org/temp/nmm#"));
				addPrefix("mto", QUrl("http://www.local-project.org/temp/mto#"));
				addPrefix("local", QUrl("http://www.local-project.org/ontologies/local#"));
				addPrefix("fts", QUrl("http://www.local-project.org/ontologies/fts#"));
				addPrefix("poi", QUrl("http://www.local-project.org/ontologies/poi#"));

				// maemo ontologies
				addPrefix("maemo", QUrl("http://maemo.org/ontologies/local#"));

				// misc ontologies
				addPrefix("exif", QUrl("http://www.kanzaki.com/ns/exif#"));
				addPrefix("protege", QUrl("http://protege.stanford.edu/system#"));
				addPrefix("xesam", QUrl("http://freedesktop.org/standards/xesam/1.0/core#"));
			}

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

			// graphs not supported
			QVector<QStringList> LocalService::rawExecuteQueryOnGraph(QString const &query, int type, RDFGraphImplBase const &/*graph*/)
			{
				Q_UNUSED(query);
				Q_UNUSED(type);
				warning() << "rawExecuteQuery not supported";
				return QVector<QStringList>();
			}

			void LocalService::rawInsertTripleToGraph(QString const &subject, QString const &predicate, QString const &object, RDFGraphImplBase const &/*graph*/)
			{
				Q_UNUSED(subject);
				Q_UNUSED(predicate);
				Q_UNUSED(object);
				warning() << "rawInsertTriple not supported";
				return;
			}

			void LocalService::rawDeleteTripleFromGraph(QString const &subject, QString const &predicate, QString const &object, RDFGraphImplBase const &/*graph*/)
			{
				Q_UNUSED(subject);
				Q_UNUSED(predicate);
				Q_UNUSED(object);
				warning() << "rawDeleteTriple not supported";
				return;
			}

			void LocalService::rawLoadOnGraph(QUrl const &file_uri, RDFGraphImplBase const &/*graph*/)
			{
				Q_UNUSED(file_uri);
				warning() << "rawLoad not supported";
				return;
			}

			QUrl LocalService::createUniqueIri(QString stem_text)
			{
				uuid_t uuid;
				uuid_generate(uuid);
				char uuid_string[37];
				uuid_unparse(uuid, uuid_string);
				return stem + stem_text + "/" + QString(uuid_string);
			}

			LocalResource *LocalService::newLiveResource(Node const &node)
			{
				return new LocalResource(sharedFromThis<RDFGraphBasic>(), node);
			}

			Node LocalService::rawNodeFromString(QString string, CppType type)
			{
				return nodeFromString(string, type);
			}

			QString LocalService::rawStringFromNode(Node const &node)
			{
				return stringFromNode(node);
			}

			LiveNodes LocalService::modelQuery(QString const &select, QStringList const &columns, RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent, TripleModulePtr const &triple_module)
			{
				Q_UNUSED(select);
				Q_UNUSED(columns);
				Q_UNUSED(strategy);
				Q_UNUSED(parent);
				Q_UNUSED(triple_module);

				warning() << "returning an empty dummy model";
				return LiveNodes(newTripleModel(strategy, QSharedCreator<LMS_Simple_Select>::create(sharedFromThis<LocalService>(), select, columns), triple_module, parent));
			}

			LiveNodeModelBasePtr LocalService::baseImplModelQuery(RDFDerivedSelect *select, RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent, TripleModulePtr const &triple_module)
			{
				Q_UNUSED(select);
				Q_UNUSED(strategy);
				Q_UNUSED(parent);
				Q_UNUSED(triple_module);

				warning("LocalService::modelQuery") << "returning an empty dummy model";
				return newTripleModel(strategy, QSharedCreator<LMS_Simple_Select>::create(sharedFromThis<LocalService>(), RDFDerivedSelect().swap(*select)), triple_module, parent);
			}

			QList<LiveNodes> LocalService::executeQuery(RDFQuery const &select, RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent)
			{
				Q_UNUSED(select);
				Q_UNUSED(strategy);
				Q_UNUSED(parent);

				warning() << "not supported";
				return QList<LiveNodes>();
			}
		}
	}
}
