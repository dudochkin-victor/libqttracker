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
 * signals_p.h
 *
 *  Created on: Apr 20, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef QTTRACKER_BACKENDS_TRACKER_SIGNALS_P_H_
#define QTTRACKER_BACKENDS_TRACKER_SIGNALS_P_H_

#include <QtDBus/QDBusInterface>
#include <QtCore/QScopedPointer>
#include "common_p.h"
#include "../../include/QtTracker/tracker.h"
#include "../../rdfcache_p.h"

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			class ClassUpdateHandler;

			class ClassUpdateDispatcher
				: public QSharedFromThis<ClassUpdateDispatcher, ClassUpdateSignaler>
			{
				Q_OBJECT

				/* VZR_CLASS(ClassUpdateDispatcher
						, (QSharedFromThis<ClassUpdateDispatcher, ClassUpdateSignaler>));*/

			public:
				typedef QString Property;
				typedef QMap<QString, QPair<Property, TripleUpdateContextWeakPtr> >
					RegisteredResources;
				typedef QList<TripleUpdateContextWeakPtr> TypeHandlers;

				QUrl classUri();

				enum EventsFlags
				{
					Adds = 1 << 0
				,	Removes = 1 << 1
				,	Updates = 1 << 2
				, 	AddsAndRemoves = Adds | Removes
				,	AllEvents = Adds | Removes | Updates
				};
				typedef unsigned Events;
				bool registerForEvents(TripleUpdateContextPtr const &handler
						, Events events = AllEvents
						, QString const &resource = QString()
						, QString const &property = QString());

			public Q_SLOTS:
				void handleAddedSubjects(QStringList const &subjects);
				void handleRemovedSubjects(QStringList const &subjects);
				void handleChangedSubjects(QStringList const &subjects);
				void handleChangedSubjects(QStringList const &subjects, QStringList const &props);

				static ClassUpdateDispatcher *create
						(QString uri, QString ontology, QString class_);

				static void destroyAll();

			protected:
				friend class TrackerService;
				ClassUpdateDispatcher(QString uri, QString ontology, QString class_);
			public:
				~ClassUpdateDispatcher();
			protected:

				bool checkForDisconnect();
				virtual void ensureDisconnect();
				virtual bool ensureConnect();

				// overrides of QObject
				void connectNotify(char const *signal);
				void disconnectNotify(char const *signal);

				RDFServicePtr service_; // VZR_MEMBER

				QScopedPointer<QDBusAbstractInterface> class_signals; // VZR_MEMBER
				QString class_path; // VZR_MEMBER
				QString class_uri; // VZR_MEMBER
				QStringList class_uri_in_strlist; // VZR_MEMBER
				QString short_name; // VZR_MEMBER

				RegisteredResources resource2handler; // VZR_MEMBER
				TypeHandlers typehandlers; // VZR_MEMBER

				int connected_count_; // VZR_MEMBER

				friend class ClassUpdateHandler;
			};
			typedef QSharedPointer<ClassUpdateDispatcher> ClassUpdateDispatcherPtr;
		}
	}
}


#endif /* QTTRACKER_BACKENDS_TRACKER_SIGNALS_P_H_ */
