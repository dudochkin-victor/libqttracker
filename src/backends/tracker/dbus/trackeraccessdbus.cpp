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

#include "trackeraccessdbus.h"
#include "dbus_types.h"
#include "../tracker_p.h"
#include <QDBusPendingCallWatcher>
#include <QString>

Q_DECLARE_METATYPE(QEventLoop::ProcessEventsFlags)

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			TrackerAccessDbus::TrackerAccessDbus(QSharedPointer<TrackerService> service, AccessType accessType) :
					pending_completion_watcher_(false), accessType_(accessType), service_(service)
			{
			}

			QSet<QString> TrackerAccessDbus::sequencingQualities() const
			{
				switch(accessType_)
				{
				case READ:
					return  QSet<QString>() << "read";
				case UPDATE:  // FALLTHROUGH
				case MUTE_UPDATE:
					return QSet<QString>() << "write";
				default:
					Q_ASSERT_X(false, " TrackerAccessDbus::sequencingDependencies", "Unknown access type");
					return QSet<QString>();
				}
			}

			DependencyChain TrackerAccessDbus::sequencingDependencies() const
			{
				switch(accessType_)
				{
				case READ:
					return DependencyChain().latestStarted("read").allFinished("write");
				case UPDATE:  // FALLTHROUGH
				case MUTE_UPDATE:
					return DependencyChain().latestStarted("write").allFinished("read");
				default:
					Q_ASSERT_X(false, " TrackerAccessDbus::sequencingDependencies", "Unknown access type");
					return DependencyChain();
				}
			}

			void TrackerAccessDbus::startQuery(const QString& query)
			{
				content_reply_ = QSharedPointer<QDBusPendingReply<> >(new QDBusPendingReply<>
						(service_->resources.SparqlQuery(query)));
				completion_watcher_ = QSharedPointer<QDBusPendingCallWatcher>
						(new QDBusPendingCallWatcher(*content_reply_), &QObject::deleteLater);

				connect(completion_watcher_.data()
						, SIGNAL(finished(QDBusPendingCallWatcher*)), SIGNAL(queryCompleted()));
			}

			void TrackerAccessDbus::abort()
			{
				if(content_reply_ && !content_reply_->isFinished()) content_reply_.clear();
				completion_watcher_.clear();
				pending_completion_watcher_ = false;
			}

			void TrackerAccessDbus::waitForQueryComplete()
			{
				// The while is here because TrackerUpdateModule might have multi-step
				// completion cycle. Specifically when Sync is requested, the first step is to wait
				// for the update query to finish, which calls TrackerUpdateModule::rynSync.
				// This sets up a new completion_watcher_ which waits for tracker.Sync to finish.

				while(completion_watcher_ && !pending_completion_watcher_
						&& !completion_watcher_->isFinished())
				{
					pending_completion_watcher_ = true;

					PERF_EVENT("before_completion_wait %d", query_id_);

					QSharedPointer<QDBusPendingCallWatcher> watcher = completion_watcher_;
					service_->waitForFinished(*watcher);

					PERF_EVENT("after_completion_wait %d", query_id_);

					pending_completion_watcher_ = false;
				}
			}

			bool TrackerAccessDbus::isWaitingForQueryComplete() const
			{
				return pending_completion_watcher_;
			}

			TrackerQueryResult TrackerAccessDbus::takeQueryResult()
			{
				const bool finished = completion_watcher_ ? completion_watcher_->isFinished() : false;
				const bool error = isError();
				TrackerQueryResult result(finished, error, errorMessage());
				if(finished && !error)
				{
					switch(accessType_)
					{
					case READ:
						{
						result.setSelectResult(QDBusPendingReply<QVector<QStringList> > (*content_reply_).value());
						break;
						}
					case UPDATE:
						{
						result.setUpdateResult(QDBusPendingReply<UpdateResultOperations>(*content_reply_).value());
						break;
						}
					case MUTE_UPDATE:
						// Do not access query result
						break;
					}
				}

				if(finished)
				{
					completion_watcher_.clear();
					content_reply_.clear();
				}

				return result;
			}

			bool TrackerAccessDbus::isError() const
			{
				const bool content_error = content_reply_ ? content_reply_->isError() : true;
				const bool watcher_error = completion_watcher_ ? completion_watcher_->isError() : true;
				return(content_error || watcher_error);
			}

			QString TrackerAccessDbus::errorMessage() const
			{
				if(content_reply_ && content_reply_->isError())
					return content_reply_->error().message();
				else if(completion_watcher_ && completion_watcher_->isError())
					return completion_watcher_->error().message();
				else
					return QString();
			}

		}
	}
}

