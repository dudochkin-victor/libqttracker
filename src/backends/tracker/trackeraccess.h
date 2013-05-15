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

#ifndef TRACKERACCESS_H
#define TRACKERACCESS_H

#include <QObject>
#include <QStringList>
#include <QVector>
#include <QSet>
#include <QString>

#include "common_p.h"
#include "../../include/QtTracker/Tracker"

namespace SopranoLive
{
	struct DependencyChain;

	namespace BackEnds
	{
		namespace Tracker
		{
			/*!
			  *
			  */
			enum AccessType
			{
				READ,
				UPDATE,
				//! Do not store query result. Used by batch mode update.
				MUTE_UPDATE
			};

			/*!
			 * State of query execution.
			 *
			 * \sa TrackerAccess::takeQueryState
			 */
			class TrackerQueryResult
			{
				/* VZR_CLASS(TrackerQueryResult);*/

				bool isFinished_; // VZR_MEMBER
				bool isError_; // VZR_MEMBER

				QString errorMessage_; // VZR_MEMBER
				QVector<QStringList> select_result_; // VZR_MEMBER

				UpdateResultOperations update_result_; // VZR_MEMBER

			public:
				TrackerQueryResult(bool isFinished, bool isError, QString const &errorMessage);
				bool isFinished() const;
				bool isError() const;
				QString errorMessage() const;
				void setSelectResult(QVector<QStringList> const &select_result);
				void setUpdateResult(UpdateResultOperations const &update_result);
				QVector<QStringList> selectResult() const;
				UpdateResultOperations updateResult() const;
			};

			/*!
			 * Abstract interface for accessing data from Tracker.
			 */
			class TrackerAccess : public QSharedObject<>
			{
				Q_OBJECT

				/* VZR_CLASS(TrackerAccess, (QObject));*/

			public:
				/*!
				 * \return sequncing qualities of this access implementation.
				 */
				virtual QSet<QString> sequencingQualities() const =0;

				/*!
				 * \return sequncing dependencies of this access implementation.
				 */
				virtual DependencyChain sequencingDependencies() const =0;

				/*!
				 * Starts query execution in tracker.
				 * Emits queryComplete() signal when query is executed.
				 *
				 * \param query the query string
				 * \sa queryCompleted
				 */
				virtual void startQuery(const QString& query) =0;

				/*!
				 * Aborts query execution. No signals will be emitted.
				 */
				virtual void abort() =0;

				/*!
				  * Waits synchronously for pending query to complete.
				  *
				  * \sa isWaitingForQueryComplete
				  */
				virtual void waitForQueryComplete() =0;

				/*!
				  * Returns true if waitForQueryComplete() is in progress.
				  *
				  * \sa waitForQueryComplete
				  */
				virtual bool isWaitingForQueryComplete() const =0;

				/*!
				 * Returns the result of the query. If the query is finished the internal state of
				 * this object is reset.
				 */				
				virtual TrackerQueryResult takeQueryResult() =0;

			Q_SIGNALS:
				/*!
				 * Emitted when query has been executed.
				 *
				 * \sa startQuery
				 */
				void queryCompleted();
			};
		}
	}
}


#endif // TRACKERACCESS_H
