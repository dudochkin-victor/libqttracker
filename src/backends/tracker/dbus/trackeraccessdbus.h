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

#ifndef TRACKERACCESSDBUS_H
#define TRACKERACCESSDBUS_H

#include <QSharedPointer>
#include <QDBusPendingReply>
#include "../trackeraccess.h"

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			class TrackerService;


			/*!
			 * Implementation of interface TrackerAccess for Dbus.
			 */
			class TrackerAccessDbus : public TrackerAccess
			{
				/* VZR_CLASS(TrackerAccessDbus, (TrackerAccess));*/

				bool pending_completion_watcher_;  // VZR_MEMBER
				AccessType accessType_; // VZR_MEMBER

			public:
				QSharedPointer<TrackerService> service_; // VZR_MEMBER

				QSharedPointer<QDBusPendingReply<> > content_reply_; // VZR_MEMBER
				// completion_watcher_ signals when the whole process is done
				// content_reply_ will contain the reply of the data. Distinction made
				// to facilitate Sync as completion marker for update calls
				QSharedPointer<QDBusPendingCallWatcher> completion_watcher_; // VZR_MEMBER


				TrackerAccessDbus(QSharedPointer<TrackerService> service, AccessType accessType);

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
			};
		}
	}
}


#endif // TRACKERACCESSDBUS_H
