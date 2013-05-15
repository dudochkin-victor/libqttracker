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

#ifndef QTTRACKER_BACKENDS_TRACKER_GRAPH_SIGNALS_P_H_
#define QTTRACKER_BACKENDS_TRACKER_GRAPH_SIGNALS_P_H_

#include <QtDBus/QDBusInterface>
#include <QtCore/QScopedPointer>
#include "common_p.h"
#include "signals_p.h"

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			struct Quad
			{
			    int graph;
			    int subject;
			    int predicate;
			    int object;
			};
			QDebug operator<<(QDebug d, Quad const &t);
			QDBusArgument& operator<<(QDBusArgument& argument, const Quad &t);
			const QDBusArgument& operator>>(const QDBusArgument& argument, Quad &t);

			class ClassUpdateCompatibilitySegment;

			class GraphClassUpdateDispatcher
				: public ClassUpdateDispatcher
			{
				Q_OBJECT

				/* VZR_CLASS(GraphClassUpdateDispatcher, (ClassUpdateDispatcher));*/

			private:
				friend class TrackerService;
				GraphClassUpdateDispatcher
						( TrackerService *service, QString uri, QString ontology, QString class_);

			public:
				~GraphClassUpdateDispatcher();

				void setCompatibilitySignalMode(bool enabled);
			private:
				void ensureDisconnect();
				bool ensureConnect();
				TrackerService *tracker_; // VZR_MEMBER

				bool connected_;// VZR_MEMBER
				typedef QScopedPointer<QSet<QSharedPointer<ClassUpdateCompatibilitySegment> > >
						CompatibilitySegments;
				CompatibilitySegments compatibility_; // VZR_MEMBER

				friend class ClassUpdateCompatibilitySegment;

			Q_SIGNALS:
			    void SignalGraphUpdated(QString class_, QVector<Quad> deleted, QVector<Quad> inserted);

			public Q_SLOTS:
			    void GraphUpdated(QString class_, QVector<Quad> deleted, QVector<Quad> inserted);

			};
			typedef QSharedPointer<GraphClassUpdateDispatcher> GraphClassUpdateDispatcherPtr;

			class ClassUpdateCompatibilitySegment
				: public QObject
			{
				Q_OBJECT

				/* VZR_CLASS(ClassUpdateCompatibilitySegment, (QObject));*/
				GraphClassUpdateDispatcher *dispatcher_; // VZR_MEMBER

				QVector<Quad> deletes_; // VZR_MEMBER
				QVector<Quad> inserts_; // VZR_MEMBER

				LiveNodes ids_to_uris_; // VZR_MEMBER
			public:
				ClassUpdateCompatibilitySegment
						( GraphClassUpdateDispatcher *dispatcher
						, QVector<Quad> deletes, QVector<Quad> inserts);

			public Q_SLOTS:
				void segmentComplete();

			private:
    			void dispatchClassUpdateSignals();
			};
		}
	}
}
using SopranoLive::BackEnds::Tracker::Quad;
Q_DECLARE_METATYPE(Quad)
Q_DECLARE_METATYPE(QVector<Quad>)


#endif /* QTTRACKER_BACKENDS_TRACKER_GRAPH_SIGNALS_P_H_ */

