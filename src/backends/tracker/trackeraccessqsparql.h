/*
 * This file is part of libqttracker project
 *
 * Copyright (C) 2009-2010, Nokia
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

#ifndef TRACKERACCESSQSPARQL_H
#define TRACKERACCESSQSPARQL_H

#include "trackeraccess.h"
class QSparqlResult;

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			class TrackerService;

			class TrackerAccessQSparql : public TrackerAccess
			{
				QSharedPointer<TrackerService> service_;
				AccessType accessType_;
				QSparqlResult* sparqlResult_;
				bool waitingForFinished_;

			public:
				TrackerAccessQSparql(QSharedPointer<TrackerService> service, AccessType accessType);
				~TrackerAccessQSparql();

				QSet<QString> sequencingQualities() const;
				DependencyChain sequencingDependencies() const;
				void startQuery(const QString& query);
				void abort();
				void waitForQueryComplete();
				bool isWaitingForQueryComplete() const;
				TrackerQueryResult takeQueryResult();

			private:
				bool isError() const;
				QString errorMessage() const;

			private:
				TrackerAccessQSparql(TrackerAccessQSparql const &);
				TrackerAccessQSparql& operator=(TrackerAccessQSparql const &);
			};
		}
	}
}

#endif // TRACKERACCESSQSPARQL_H
