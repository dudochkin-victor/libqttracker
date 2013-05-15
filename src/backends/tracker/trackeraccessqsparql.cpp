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

#include "trackeraccessqsparql.h"
#include "tracker_p.h"
#include "trackerutil.h"
#include "util_p.h"
#include <QString>
#include <QtSparql/QSparqlConnection>
#include <QtSparql/QSparqlQuery>
#include <QtSparql/QSparqlResult>
#include <QtSparql/QSparqlError>

Q_DECLARE_METATYPE(QEventLoop::ProcessEventsFlags)

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{

			TrackerAccessQSparql::TrackerAccessQSparql(QSharedPointer<TrackerService> service, AccessType accessType)
					: service_(service)
					, accessType_(accessType)
					, sparqlResult_(0)
					, waitingForFinished_(false)
			{
				if(service_->directConnection_.isNull())
				{
					service_->directConnection_ = QSharedPointer<QSparqlConnection>(new QSparqlConnection("QTRACKER_DIRECT"));
					debug(6) << "QSparqlConnection::isValid" << service_->directConnection_ ->isValid();
				}
			}

			TrackerAccessQSparql::~TrackerAccessQSparql()
			{
				delete sparqlResult_;
			}

			QSet<QString> TrackerAccessQSparql::sequencingQualities() const
			{
				switch(accessType_)
				{
				case READ:
					return  QSet<QString>() << "read";
				default:
					Q_ASSERT_X(accessType_ != READ, " TrackerAccessQSparql::sequencingDependencies", "Unsupported access type");
					return QSet<QString>() ;
				}
			}

			DependencyChain TrackerAccessQSparql::sequencingDependencies() const
			{
				switch(accessType_)
				{
				case READ:
					return DependencyChain()
							.allFinished("read")  // Do not run queries in parallel with QTRACKER_DIRECT connection
							.allFinished("write");
				default:
					Q_ASSERT_X(accessType_ != READ, " TrackerAccessQSparql::sequencingDependencies", "Unsupported access type");
					return DependencyChain();
				}
			}


			void TrackerAccessQSparql::startQuery(const QString& query)
			{
				debug(6) << " TrackerAccessQSparql::startQuery" << quint32(this);
				abort();
				debug(6) << "QUERY: " << query;

				sparqlResult_ = service_->directConnection_ ->exec(QSparqlQuery(query));
				if(sparqlResult_->isFinished() && sparqlResult_->hasError())
					sharedFromThis(), Q_EMIT queryCompleted();
				else
					connect(sparqlResult_, SIGNAL(finished()), this, SIGNAL(queryCompleted()));
			}

			void TrackerAccessQSparql::abort()
			{
				delete sparqlResult_;
				sparqlResult_ = 0;
			}

			void TrackerAccessQSparql::waitForQueryComplete()
			{
				debug(6) << "ENTER: TrackerAccessQSparql::waitForQueryComplete" <<	quint32(this);
				if(!waitingForFinished_ && sparqlResult_)
				{
					debug(6) << "WAITING QUERY" << sparqlResult_->isFinished() << sparqlResult_->hasError();
					waitingForFinished_ = true;
					sparqlResult_->waitForFinished();
					debug(6) << "WAITING STOPPED" << sparqlResult_->isFinished() << sparqlResult_->hasError();
					waitingForFinished_ = false;
				}
			debug(6) << "EXIT: TrackerAccessQSparql::waitForQueryComplete" <<	quint32(this);
			}

			bool TrackerAccessQSparql::isWaitingForQueryComplete() const
			{
				return waitingForFinished_;
			}

			TrackerQueryResult TrackerAccessQSparql::takeQueryResult()
			{
				debug(6) << " TrackerAccessQSparql::takeQueryResult()" << quint32(this);
				const bool finished = sparqlResult_ ? sparqlResult_->isFinished() : false;
				const bool error =isError();
				TrackerQueryResult result(finished, error, errorMessage());
				debug(6) << "TrackerQueryResult: " << finished << error << result.errorMessage() << !!sparqlResult_;
				if(finished && !error)
				{
					QVector<QStringList> data;
					while(sparqlResult_->next())
					{
						QStringList row;
						for(int i = 0; i < sparqlResult_->current().count(); ++i)
						{
							QVariant const &val = sparqlResult_->value(i);
							QString strVal;
							if(val.type() == QVariant::DateTime)
								strVal = toIsoDateWithUtcOffsetString(val.toDateTime());
							else
								strVal = val.toString();
							row << strVal;
						}
						debug(6) << "ROW: " << row;
						data << row;
					}
					result.setSelectResult(data);
					debug(6) << "TOTAL ROWS: " << data.count();
					sparqlResult_->first();
					sparqlResult_->previous();
				}

				if(finished)
				{
					sparqlResult_->deleteLater();
					sparqlResult_ = 0;
				}

				return result;
			}

			bool TrackerAccessQSparql::isError() const
			{
				return (sparqlResult_ && sparqlResult_->isFinished()) ? sparqlResult_->hasError() : false;
			}

			QString TrackerAccessQSparql::errorMessage() const
			{
				return (sparqlResult_ && sparqlResult_->isFinished() && sparqlResult_->hasError()) ? sparqlResult_->lastError().message() : QString("");
			}

		}
	}
}

