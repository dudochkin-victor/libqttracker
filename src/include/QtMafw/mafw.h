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
 * mafw.h
 *
 *  Created on: Jan 5, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */



#ifndef SOPRANOLIVE_QTMAFW_MAFW_H
#define SOPRANOLIVE_QTMAFW_MAFW_H

#include "RDFService"
#include "mafw_common.h"

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Mafw
		{
			class Q_DECL_EXPORT MafwRdfResource
			{
			public:
				/**
				 * Returns the Resource ID. Sources are identified by an arbitrary long
				 * unique string. The only restriction is that it MUST NOT contain the
				 * substring "::" (that is two colons).
				 */
				virtual QString uuid() const = 0;

				/**
				 * query method performs a query on a source. The results of the query are
				 * delivered to the slot of the given receiver specified by the member argument.
				 * Format of the callback slot is presented below:
				 *
				 * @code
				 * public slot:
				 *    void handleQueryResult(uint queryId, const QStringList& resultRow, uint index, int remainingCount);
				 *
				 * When the query is finished, the error slot is called. Also if an error happens
				 * before the query is succesfully finished, the error slot is called. Error 0 means no error.
				 * Format of the error slot is presented below:
				 *
				 * public slot:
				 *    void handleQueryError(uint queryId, int error, QString errorMessage);
				 *
				 * @param query The well-formatted SPARQL query to be handled by the source.
				 * @param receiver The QObject whose result and/or error slots are called.
				 * @param resultSlot The slot used to receive result rows.
				 * @param errorSlot The slot is invoked at the end of query or when an error occurs.
				 * @return The id of the query.
				 */
				virtual uint query(QString query, QObject* receiver, const char* resultSlot,
								   const char* errorSlot) = 0;

				/**
				 * Browse content, using RDF predicates.
				 * @param objectId The id of the object to browse, for a container to get
				 * content (children).
				 * @param predicate RDF predicate to get the value for. In case of containers, use
				 * "nie::hasLogicalPart" predicate to get children of the container.
				 * @param additionalPredicates Additional "metadata" predicates to be included in the
				 * browse e.g. "nie::Title".
				 * @param skipCount The amount of items to skip from the beginning.
				 * @param itemCount The maximum amount of items to return.
				 * @param receiver The object to receive the result/error signals
				 * @param resultSlot The slot to receive the results. The slot signature:
				 * @code
				 * void resultSlot(uint queryId, QString subjectId,
				 *      QMap<QString,QVariant> meta, uint index, int remainingCount)
				 * @endcode
				 * @param errorSlot The slot to receive possible error codes. The slot signature:
				 * @code
				 * void errorSlot( uint browseId, const int& error );
				 * @endcode
				 * The error is a MafwError::Code.
				 * @return Id for the browse request. Zero if browse cannot executed at all.
				 *
				 * Browse properties of given resource node. If given predicate is detected to point to resource node
				 * then fetch also additionalPredicates from that node.
				 *
				 * Example invocation:
				 * @code
				 * source->browse( node.iri().toString(), rdfs::type::iri().toString(), QStringList(), 0, 5, this,
							SLOT( browseResultSlot(uint, QString, QMap<QString,QVariant>, uint, int) ),
							SLOT( browseErrorSlot(uint, int, QString) ) );
				 * @endcode
				 *
				 * Callback slot signatures:
				 * @code
				 * public slot:
				 *    void handleBrowseResult(uint browseId, QString objectId, QMap<QString,QVariant> metamap, uint index, int remainingCount);
				 * @endcode
				 *
				 * @code
				 * public slot:
				 *    void handleBrowseError(uint queryId, int errorCode, QString errorMessage);
				 * @endcode
				 */
				virtual uint browse( const QString objectId, const QString predicate, const QStringList additionalPredicates,
									 uint skipCount, uint itemCount,
									 QObject* receiver, const char* resultSlot,
									 const char* errorSlot ) = 0;

				/**
				 * Cancels ongoing browse. Browsing is cancelled immediately, but
				 * available result will still come. The error slot given by browse call
				 * is called when cancel is made.
				 * @param browseId the browse request id to be cancelled
				 */
				virtual void cancelBrowse( uint browseId ) = 0;

				/**
				 * Cancels ongoing query. Query is cancelled immediately, but
				 * available result will still come. The error slot given by query call
				 * is called when cancel is made.
				 * @param queryId the query request id to be cancelled
				 */
				virtual void cancelQuery( uint queryId ) = 0;

				/**
				 * Insert statement to resource.
				 */
				virtual void insertTriple(QString const &subject, QString const &predicate, QString const &object) {
					Q_UNUSED( subject );
					Q_UNUSED( predicate );
					Q_UNUSED( object );
				};

				/**
				 * Remove statement from resource.
				 */
				virtual void deleteTriple(QString const &subject, QString const &predicate, QString const &object) {
					Q_UNUSED( subject );
					Q_UNUSED( predicate );
					Q_UNUSED( object );
				};
			};

			/*!
			 * returns a \ref RDFService proxy object providing connection to given MAFW source.
			 */
			Q_DECL_EXPORT QSharedPointer<SopranoLive::RDFService> getRDFService(MafwRdfResource& resource);

			/*!
			 * returns a \ref RDFService proxy object providing connection to given MAFW source via cache layer.
			 */
			Q_DECL_EXPORT QSharedPointer<SopranoLive::RDFService> getCachedRDFService(MafwRdfResource& resource);

			/*!
			 * Returns the root of a MafwResource i.e. the browsable root.
			 * @param service RDFService to browse root for, must be castable to a MafwRdfResource
			 * i.e. RDFService of a MafwSource, which can be fetched via getRDFService.
			 * @return LiveNodes of the root container in MafwRdfResource. If the parameter
			 * service was not MafwRdfResource an empty LiveNodes object is returned.
			 */
			Q_DECL_EXPORT LiveNode rootNode(QSharedPointer<SopranoLive::RDFService> service);

			/*!
			 * Returns the object ID of an LiveNode.
			 * @param node The LiveNode whose object ID to return.
			 * @return The object ID of the LiveNode.
			 */
			Q_DECL_EXPORT QString getObjectId(const LiveNode& node);

			/*!
			 * By default, 0 on release builds, 1 on debug builds.
			 */
			Q_DECL_EXPORT void setMafwVerbosity(int level = 0);
		}
	}
}
#endif // SOPRANOLIVE_QTMAFW_MAFW_H
