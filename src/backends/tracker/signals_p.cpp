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
 * signals_p.cpp
 *
 *  Created on: Apr 20, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include "signals_p.h"
#include "tracker_p.h"

#define SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING
#define SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include "../../include/QtTracker/ontologies/rdf.h"
#undef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#undef SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING

#include "dbus/asyncdbusinterface.h"

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			ClassUpdateDispatcher::ClassUpdateDispatcher
					(QString class_uri, QString ontology, QString class_)
				: class_signals()
				, class_path(Tracker::resources_class_path_g + "/" + ontology + "/" + class_)
				, class_uri(class_uri)
				, class_uri_in_strlist(class_uri)
				, short_name(ontology + ":" + class_)
				, connected_count_(0)
			{
				// << this  << short_name << " created";
			}

			ClassUpdateDispatcher::~ClassUpdateDispatcher()
			{
				// << this << short_name << " destroyed";
			}


			bool ClassUpdateDispatcher::ensureConnect()
			{
				if(class_signals && class_signals->isValid())
					return true;

				class_signals.reset(new AsyncDBusInterface( Tracker::service_g
						, class_path
						, Tracker::resources_class_interface_g
						, trackerBus()));

				if(!class_signals->isValid()
					|| !QObject::connect
							( class_signals.data(), SIGNAL(SubjectsAdded(const QStringList &))
							, this, SLOT(handleAddedSubjects(const QStringList &)))
					|| !QObject::connect
							( class_signals.data(), SIGNAL(SubjectsRemoved(const QStringList &))
							, this, SLOT(handleRemovedSubjects(const QStringList &)))
					||
					( !QObject::connect
						( class_signals.data()
						, SIGNAL(SubjectsChanged(const QStringList &, const QStringList &))
						, this
						, SLOT(handleChangedSubjects(const QStringList &, const QStringList &)))
					&& !QObject::connect
						( class_signals.data(), SIGNAL(SubjectsChanged(const QStringList &))
						, this, SLOT(handleChangedSubjects(const QStringList &))))
					)
				{
					warning() << "while connecting to "
							  << Tracker::service_g
							  << class_path
							  << Tracker::resources_class_interface_g
							  << ":"
							  << "\n\t" << class_signals->lastError()
							  << "\n\tmake sure the class has tracker:notify = true enabled";
					return false;
				}

				debug(3) << "Listening to signals from " + short_name;
				return true;
			}

			void ClassUpdateDispatcher::ensureDisconnect()
			{
				if(!class_signals)
					return;
				// signals automatically detached
				class_signals->blockSignals(true);
				class_signals.reset();
				connected_count_ = 0;
			}

			bool ClassUpdateDispatcher::checkForDisconnect()
			{
				if(resource2handler.size() || typehandlers.size() || connected_count_)
					return true;
				ensureDisconnect();
				return false;
			}

			void ClassUpdateDispatcher::connectNotify(char const *signal)
			{
				Q_UNUSED(signal);
				if(!connected_count_++)
					ensureConnect();
			}
			void ClassUpdateDispatcher::disconnectNotify(char const *signal)
			{
				Q_UNUSED(signal);
				if(!--connected_count_)
					checkForDisconnect();
			}


			bool ClassUpdateDispatcher::registerForEvents
					(TripleUpdateContextPtr const &handler, Events events
					, QString const &resource, QString const &property)
			{
				if(!ensureConnect())
					return false;
				debug(3) << "Event handler for" << short_name << "registered";
				if(events & AddsAndRemoves)
					typehandlers.push_back(handler);
				if(events & Updates)
					resource2handler.insertMulti
							( resource
							, QPair<Property, TripleUpdateContextWeakPtr>
									(property, handler));
				handler->mode_ = TripleUpdateContext::FullUpdates;
				return true;
			}

			//! \return false if the context was invalidated and should be deleted
			bool notifyEntry
					( QPair<QString, TripleUpdateContextWeakPtr> const &entry_pair
					, RawTriplesData const &data)
			{
				TripleUpdateContextPtr strong_entry = entry_pair.second;
				if(!strong_entry)
					return false;

				if(entry_pair.first.size()
						&& data.predicates().size()
						&& !data.predicates().contains(entry_pair.first))
					return true;
				return strong_entry->tripleUpdates(data);
			}

			//! \return false if the context was invalidated and should be deleted
			bool notifyEntry(TripleUpdateContextWeakPtr const &entry, RawTriplesData const &data)
			{
				if(TripleUpdateContextPtr strong_entry = entry)
					return strong_entry->tripleUpdates(data);
				return false;
			}

				template<typename Cont>
			bool tripleNotifyRange
				( Cont &container
				, typename Cont::iterator i, typename Cont::iterator const &iend
				, RawTriplesData const &data
				)
			{
				while(i != iend)
					if(notifyEntry(*i, data))
						++i;
					else
					{
						container.erase(i++);
						if(!container.size())
							return false;
					}
				return true;
			}

				template<typename Cont>
			bool tripleNotifyForEach(Cont &container, RawTriplesData const &data)
			{
				return tripleNotifyRange(container, container.begin(), container.end(), data);
			}

			void ClassUpdateDispatcher::handleAddedSubjects(QStringList const &subjects)
			{
				debug(2) << "Signal from" << short_name << ": "
					"adds in following subjects:\n\t" << subjects.join(", ");

				if(!tripleNotifyForEach
						( typehandlers
						, RawTriplesData
							( TripleUpdateContext::Adds | RDFStrategy::SubjectPredicateObject
							, subjects
							, rdf_type_as_list_g
							, class_uri_in_strlist))
					)
					checkForDisconnect();

				sharedFromThis(), Q_EMIT subjectsAdded(subjects);
			}
			void ClassUpdateDispatcher::handleRemovedSubjects(QStringList const &subjects)
			{
				debug(2) << "Signal from" << short_name << ": "
					"removes in following subjects:\n\t" << subjects.join(", ");

				if(!tripleNotifyForEach
						( typehandlers
						, RawTriplesData
							( TripleUpdateContext::Removes | RDFStrategy::SubjectPredicateObject
							, subjects
							, rdf_type_as_list_g
							, class_uri_in_strlist))
					)
					checkForDisconnect();

				sharedFromThis()
						, Q_EMIT subjectsRemoved(subjects)
						, Q_EMIT baseRemoveSubjectsd(subjects);
			}
			void ClassUpdateDispatcher::handleChangedSubjects(QStringList const &subjects)
			{
				handleChangedSubjects(subjects, QStringList());
			}
			void ClassUpdateDispatcher::handleChangedSubjects
					( QStringList const &subjects, QStringList const &props)
			{
				debug(2) << "Signal from" << short_name
						 << ": changes in following subjects:\n\t" << subjects.join(", ")
						 << "\n\t\tand predicates:\n\t" << props.join(", ");

				// TODO: sort and unique subjects and props?

				RawTriplesData changes
					( TripleUpdateContext::Adds | TripleUpdateContext::Removes
							| RDFStrategy::SubjectPredicate
					, subjects, props, QStringList());

				bool no_container_empty = true;

				/* TODO: further evaluate whether this is needed
				no_container_empty &=
					tripleNotifyForEach<&TripleUpdateContext::tripleUpdates>
						(typehandlers, changes);
				*/

				RegisteredResources::iterator ari = resource2handler.begin()
											, ariend = resource2handler.upperBound(QString());

				no_container_empty &=
					tripleNotifyRange(resource2handler, ari, ariend, changes);

				if(ariend != resource2handler.end())
					for(QStringList::const_iterator si = subjects.begin(), siend = subjects.end()
							; si != siend ; ++si)
						no_container_empty &=
							tripleNotifyRange
								( resource2handler
								, resource2handler.lowerBound(*si)
								, resource2handler.upperBound(*si)
								, changes);

				if(!no_container_empty)
					checkForDisconnect();

				sharedFromThis()
						, Q_EMIT subjectsChanged(subjects)
						, Q_EMIT subjectsChanged(subjects, props);
			}

			QThreadStorage<QMap<QString, QSharedPointer<ClassUpdateDispatcher> > *> signals_tls;

			ClassUpdateDispatcher *ClassUpdateDispatcher::create
					(QString class_uri, QString ontology, QString class_)
			{
				if(!signals_tls.hasLocalData())
					signals_tls.setLocalData
						(new QMap<QString, QSharedPointer<ClassUpdateDispatcher> >());
				QSharedPointer<ClassUpdateDispatcher> &dispatcher
					= (*signals_tls.localData())[class_uri];
				if(!dispatcher)
					dispatcher = QSharedPointer<ClassUpdateDispatcher>
							(new ClassUpdateDispatcher(class_uri, ontology, class_));
				return dispatcher.data();
			}

			void ClassUpdateDispatcher::destroyAll()
			{
				signals_tls.setLocalData(0);
			}

			QUrl ClassUpdateDispatcher::classUri()
			{
				return QUrl(class_uri);
			}
		}
	}
}
