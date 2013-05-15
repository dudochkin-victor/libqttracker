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
 * local_p.h
 *
 *  Created on: Jul 24, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_BACKENDS_LOCAL_LOCAL_P_H_
#define SOPRANOLIVE_BACKENDS_LOCAL_LOCAL_P_H_

#include "../../include/sopranolive/local_common.h"
#include "../../rdfcache_p.h"
#include "../../util_p.h"

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Local
		{
			class LocalResource
				: public ResourceOfService
			{
			public:
				LocalResource(QSharedPointer<RDFServiceImplBase> const &service, Node const &node);
				~LocalResource();
			};

			struct LocalService
				: RDFServiceBasic
			{
				LocalService();
				~LocalService();

				QUrl serviceIri() const;

				static QSharedPointer<LocalService> get();
				static QSharedPointer<RDFGraphCache> getRDFGraphCache(bool persistent = true, bool register_weak = false);

				QString stem;
				int id_counter;
				int query_id_counter;
				int update_id_counter;

				void loadLocalPrefixes();

				QVector<QStringList> rawExecuteQueryOnGraph(QString const &query, int type, RDFGraphImplBase const &graph);
				void rawInsertTripleToGraph(QString const &subject, QString const &predicate, QString const &object, RDFGraphImplBase const &graph);
				void rawDeleteTripleFromGraph(QString const &subject, QString const &predicate, QString const &object, RDFGraphImplBase const &graph);
				void rawLoadOnGraph(QUrl const &file_uri, RDFGraphImplBase const &graph);

				QUrl createUniqueIri(QString stem_text);

				LocalResource *newLiveResource(Node const &node = Node());

				Node rawNodeFromString(QString string, CppType type);
				QString rawStringFromNode(Node const &node);

				LiveNodes modelQuery(QString const &select, QStringList const &columns, RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>(), TripleModulePtr const &triple_module = TripleModulePtr());
				LiveNodeModelBasePtr baseImplModelQuery(RDFDerivedSelect *select, RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>(), TripleModulePtr const &triple_module = TripleModulePtr());
				QList<LiveNodes> executeQuery(RDFQuery const &query, RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());
			};

				template<typename Base = ContentModule_Simple>
			struct LocalModelStrategy
				: Base
			{
				QSharedPointer<LocalService> service;
				LocalModelStrategy(QSharedPointer<LocalService> service, ContentModule::ChangePolicy change_policy = ContentModule::ReportChanges)
					: Base(change_policy), service(service)
				{}
			};

			struct LMS_Simple_Select
				: LocalModelStrategy<>
			{
				RDFDerivedSelect select_;
				int query_id_;

				LMS_Simple_Select(QSharedPointer<LocalService> service, RDFDerivedSelect const &select);
				LMS_Simple_Select(QSharedPointer<LocalService> service, QString const &query_text, QStringList const &columns);
				RDFStrategyFlags refreshModel_chain(LiveNodeModel::RefreshMode mode, InterfaceChain<ModelCommons> chain);
			private:
				void attached(RowStoreModel &model);
				ContentModulePtr copy() const;
			};
		}
	}
}

#endif /* SOPRANOLIVE_BACKENDS_LOCAL_LOCAL_P_H_ */
