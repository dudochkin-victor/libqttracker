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
 * mafw_p.h
 *
 *  Created on: Apr 6, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef QTTRACKER_MAFW_P_H
#define QTTRACKER_MAFW_P_H

#include <QObject>

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include <QtCore/QMetaType>
#include <QtCore/QVector>
#include <QtCore/QStringList>

#include "../../include/QtTracker/tracker_common.h"
#include "../../include/QtMafw/mafw.h"
#include "../../liveresource_p.h"
#include "../../rowstoremodel_p.h"
//#include "../../rdfunbound_p.h"
#include "../../rdfservice_p.h"
#include "../../util_p.h"

using namespace SopranoLive;

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Mafw
		{

			class Mafw;

			class MafwResource
                : public ResourceOfService
			{
			public:
				MafwResource(QSharedPointer<Mafw> const &graph, Node const &node = Node())
					: ResourceOfService(graph, node) {}

				LiveNodeModelBasePtr implGetProperties(RDFDerivedSelect *columns, TripleModulePtr const &triple_module = TripleModulePtr()) const;
			};

			class Mafw
				: public RDFServiceBasic
			{
			public:
				Mafw(MafwRdfResource& resource);

				MafwRdfResource& resource;
				//QString stem;
				//QString session_id;
				int id_counter;

                                static QSharedPointer<Mafw> getRDFGraph(MafwRdfResource& resource);

				QUrl serviceIri() const;

				QVector<QStringList> rawExecuteQueryOnGraph(QString const &query, int type, RDFGraphImplBase const &graph);
				void rawInsertTripleToGraph(QString const &subject, QString const &predicate, QString const &object, RDFGraphImplBase const &graph);
				void rawDeleteTripleFromGraph(QString const &subject, QString const &predicate, QString const &object, RDFGraphImplBase const &graph);
				void rawLoadOnGraph(QUrl const &file_uri, RDFGraphImplBase const &graph);

				QUrl createUniqueIri(QString stem_text);
				LiveNode createLiveNode(CppType type = CppType::of<QUrl>());

				MafwResource *newLiveResource(Node const &node = Node());

				Node rawNodeFromString(QString string, CppType type);
				QString rawStringFromNode(Node const &node);

				LiveNodeModelBasePtr browse(LiveNode const &node, RDFVariable const &predicate, RDFVariable const &object,
                        RDFDerivedSelect const &columns, bool streaming, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>(), TripleModulePtr const &triple_module = TripleModulePtr());

				LiveNodes modelQuery(QString const &select, QStringList const &columns, RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>(), TripleModulePtr const &triple_module = TripleModulePtr());
				LiveNodeModelBasePtr baseImplModelQuery(RDFDerivedSelect *select, RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>(), TripleModulePtr const &triple_module = TripleModulePtr());

				QList<LiveNodes> executeQuery(RDFQuery const &query, RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());

				void insertTripleToGraph(LiveNode const &subject, LiveNode const &predicate, LiveNode const &object, RDFGraphImplBase const &graph);
				void deleteTripleFromGraph(LiveNode const &subject, LiveNode const &predicate, LiveNode const &object, RDFGraphImplBase const &graph);

			};

			class MafwModelStrategy
				: public QObject, public ContentModule_Simple
			{
				Q_OBJECT

			protected:
				QSharedPointer<Mafw> service_;
				RDFDerivedSelect columns_;
				bool streaming_; // Streaming mode i.e. non-blocking updates
				uint ongoingID_; // query/browse ID being executed
				bool waiting_; // Blocking is in action i.e. application waiting for results
				bool updatePassThrough_; // Internal update in action

			public:
				MafwModelStrategy
						( QSharedPointer<Mafw> service, RDFDerivedSelect const &columns
						, bool streaming, ChangePolicy change_policy = ReportChanges);
				virtual ~MafwModelStrategy();

				virtual QString getQueryText() const;
				void attached(RowStoreModel &model);
				void detached(RowStoreModel &model);

				virtual uint initiateQuery();
				virtual void cancelQuery(uint query_id);

				ReadyFlags refreshModel_chain(LiveNodeModel::RefreshMode mode
						, InterfaceChain<ModelCommons> chain);

				bool alterStrategy_chain(RDFStrategyFlags flags, RDFStrategyFlags mask
						, QModelIndex const &index, InterfaceChain<ModelCommons> chain);

				// TODO: handle copying of the strategy. Low priority.
				// \ref LiveNodeModel::assignModel is the only entity that requires the
				// strategy of the source model to be copyable.
				// Even if the copy is not implemented, the assignModel will work, however no updates
				// will be happening on the new model.
				ContentModulePtr copy() const { return ContentModulePtr(); }

			public slots:
			    /**
			     * Receive query results from source
			     */
			    virtual void handleQueryResult(uint queryId, QStringList resultRow, uint index, int remainingCount);

			    /**
			     * Receive query error from source
			     */
			    virtual void handleQueryError(uint queryId, int err, QString errorMessage);
			};

			struct MMS_Simple_TextQuery : public MafwModelStrategy
			{
				Q_OBJECT

				QString query_text_;

			public:
				MMS_Simple_TextQuery
						( QSharedPointer<Mafw> service, QString const &query_text
						, RDFDerivedSelect const &columns, bool streaming);

				static QSharedPointer<MMS_Simple_TextQuery> create
						( QSharedPointer<Mafw> const &service, QString const &query_text
						, QStringList const &columns, bool streaming);

				QString getQueryText() const;
			};

            /**
             * Browse class for browsing MafwRdfResource contents.
             */
			class MMS_Simple_Browse : public MafwModelStrategy
            {
                Q_OBJECT

				/** The node to browse */
				LiveNode const node_;
				/** The predicate to browse for */
				RDFVariable const predicate_;
				/** The result browsed object's type */
				RDFVariable const object_;

				QString predicateStr_;
				QStringList additionalPredicates_;

			public:
				/**
					 * Constructor.
					 * @param service The service to connect the browse action.
					 * @param node The node to browse.
					 * @param predicate The predicate to browse for.
					 * @param object The browsed object's type
					 */
				MMS_Simple_Browse
						( QSharedPointer<Mafw> service, LiveNode const &node
						  , RDFVariable const &predicate, RDFVariable const &object
						  , RDFDerivedSelect const &columns, bool streaming);
				~MMS_Simple_Browse();

				void attached(RowStoreModel &model);

				uint initiateQuery();
				void cancelQuery(uint query_id);

			public slots:
				/**
					 * Slot for handling received browse result
					 * @see MafwRdfResource::browse
					 */
				void handleResult
						( uint browseId, QString subjectId, QMap<QString,QVariant> meta
						  , uint index, int remainingCount);

				/**
					 * Slot for handling browse errors
					 * @see MafwRdfResource::browse
					 */
				void handleError( uint browseId, uint error, QString errorMessage );
			};

		}
	}
}
#endif /* QTTRACKER_MAFW_P_H */
