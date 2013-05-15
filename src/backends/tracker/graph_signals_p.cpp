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

#include "graph_signals_p.h"
#include "tracker_p.h"

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{

			const QString changedSignal = QString::fromLatin1("GraphUpdated");
			const QString changedSignature = QString::fromLatin1("sa(iiii)a(iiii)");

			QDebug operator<<(QDebug d, Quad const &t)
			{
				return d.nospace() << t.graph << t.subject << t.predicate << t.object;
			}

			// D-Bus marshalling
			QDBusArgument &operator<<(QDBusArgument &argument, const Quad &t)
			{
			    argument.beginStructure();
			    argument << t.graph << t.subject << t.predicate << t.object;
			    argument.endStructure();
			    return argument;
			}

			// D-Bus demarshalling
			const QDBusArgument &operator>>(const QDBusArgument &argument, Quad &t)
			{
			    argument.beginStructure();
			    argument >> t.graph >> t.subject >> t.predicate >> t.object;
			    argument.endStructure();
			    return argument;
			}


			GraphClassUpdateDispatcher::GraphClassUpdateDispatcher
					( TrackerService *tracker
					, QString class_uri, QString ontology, QString class_)
				: ClassUpdateDispatcher(class_uri, ontology, class_)
				, tracker_(tracker)
				, connected_(false)
				, compatibility_()
			{
				// << this  << short_name << " created";
			}

			GraphClassUpdateDispatcher::~GraphClassUpdateDispatcher()
			{
				// << this << short_name << " destroyed";
			}

			void GraphClassUpdateDispatcher::setCompatibilitySignalMode(bool enabled)
			{
				if(enabled)
					compatibility_.reset(new QSet<QSharedPointer<ClassUpdateCompatibilitySegment> >());
				else
					compatibility_.reset();
			}

			bool GraphClassUpdateDispatcher::ensureConnect()
			{
				if(connected_)
					return true;

			    qDBusRegisterMetaType<Quad>();
			    qDBusRegisterMetaType<QVector<Quad> >();

				connected_ = true;

			    bool ok = trackerBus().connect
			    		( service_g, resources_path_g, resources_interface_g, changedSignal
			    		, class_uri_in_strlist, changedSignature
			    		, this, SLOT(GraphUpdated(QString, QVector<Quad>, QVector<Quad>)));
			    if (!ok)
			    {
			        warning() << "Cannot connect to GraphUpdated signal from Tracker";
					connected_ = false;
			        return false;
			    }

			    // Query from tracker: all the resources of the given type, and their
			    // tracker:id's. Store the mapping. The query is made asynchronously, and
			    // connection to the real signal is done only when we get the results.

			    /*
			    QSparqlQuery allUris(QString::fromLatin1("select tracker:id(?u) ?u { ?u a ?:class}"));
			    allUris.bindValue(QString::fromLatin1("?:class"), klass);

			    d->result = d->qsparqlConn->exec(allUris);
			    if (!d->result) {
			        qWarning() << QString::fromLatin1("Cannot execute queries");
			        return;
			    }
			    if (d->result->hasError()) {
			        qWarning() << "Cannot execute queries:" << d->result->lastError().text();
			        delete d->result;
			        d->result = 0;
			        return;
			    }
			    connect(d->result, SIGNAL(finished()),
			            d, SLOT(allUrisFinished()));
			    */

			    // this warning cannot be received anymore, unless we query for
			    // tracker:notify property explicitly. We could do that...

			    /*
				warning() << "while connecting to "
						  << Tracker::service_g
						  << class_path
						  << Tracker::resources_class_interface_g
						  << ":"
						  << "\n\t" << class_signals->lastError()
						  << "\n\tmake sure the class has tracker:notify = true enabled";
				*/

				debug(3) << "Listening to signals from " + short_name;

				return true;
			}

			void GraphClassUpdateDispatcher::ensureDisconnect()
			{
				if(!connected_)
					return;
			    trackerBus().disconnect
			    		( service_g, resources_path_g, resources_interface_g, changedSignal
			    		, class_uri_in_strlist, changedSignature
			    		, this, SLOT(GraphUpdated(QString, QVector<Quad>, QVector<Quad>)));
				debug(3) << "Stopped listening to signals from " + short_name;

			    connected_ = false;
			}

		    void GraphClassUpdateDispatcher::GraphUpdated
		    		( QString class_, QVector<Quad> deleted, QVector<Quad> inserted)
		    {
		    	Q_UNUSED(class_);
		    	QStringList inserted_or_deleted_uris;
		    	QVector<int> inserted_or_deleted_ids;
		    	// << "GraphUpdated(" << this << "): Received signals for " << class_;
		    	if(compatibility_)
		    		compatibility_->insert(QSharedPointer<ClassUpdateCompatibilitySegment>
		    				(new ClassUpdateCompatibilitySegment(this, deleted, inserted)));
		    }

			ClassUpdateCompatibilitySegment::ClassUpdateCompatibilitySegment
					( GraphClassUpdateDispatcher *dispatcher
					, QVector<Quad> deletes, QVector<Quad> inserts)
				: dispatcher_(dispatcher), deletes_(deletes), inserts_(inserts)
				, ids_to_uris_()
			{
				TrackerService *ts = dispatcher_->tracker_;
		    	QSet<int> unknown_ids;
	    		QVector<Quad> *quads[] = { &deletes_, &inserts_ };
	    		for(int i = 0; i < 2; ++i)
		    	{
			    	for(QVector<Quad>::const_iterator cqi = quads[i]->constBegin()
			    			, cqiend = quads[i]->constEnd(); cqi != cqiend; ++cqi)
			    		{
			    			if(cqi->subject && !ts->id2iri_.at(cqi->subject).size())
			    				unknown_ids.insert(cqi->subject);
			    			if(cqi->predicate && !ts->id2iri_.at(cqi->predicate).size())
			    				unknown_ids.insert(cqi->predicate);
			    			if(cqi->object && !ts->id2iri_.at(cqi->object).size())
			    				unknown_ids.insert(cqi->object);
			    		}
		    	}
	    		if(unknown_ids.size())
	    		{
	    			RDFVariable var = RDFVariable::fromType<rdfs::Resource>();
	    			var.filter("tracker:id").isMemberOf(unknown_ids);
	    			ids_to_uris_ = dispatcher_->tracker_->RDFGraphImplBase::modelQuery(RDFSelect()
									.addColumn("uri", var)
									.addColumn("id", var.filter("tracker:id"))
	    					, RDFStrategyFlags());
	    			connect(ids_to_uris_.model(), SIGNAL(modelUpdated()), SLOT(segmentComplete()));
	    		} else
	    			dispatchClassUpdateSignals();
			}

			void ClassUpdateCompatibilitySegment::segmentComplete()
			{
				for(int i = 0; i < ids_to_uris_->rowCount(); ++i)
				{
					// << "Registering" << ids_to_uris_->liveNode(i, 0).uri().toEncoded()
					//		<< ids_to_uris_->liveNode(i, 1).literal().toInt();
					dispatcher_->tracker_->id2iri_.register_
							( ids_to_uris_->liveNode(i, 1).literal().toInt()
							, ids_to_uris_->liveNode(i, 0).uri().toEncoded());
				}
    			dispatchClassUpdateSignals();
			}

			void ClassUpdateCompatibilitySegment::dispatchClassUpdateSignals()
			{
				QMap<QString, int> added_subjects, removed_subjects
						, changed_subjects, changed_properties;

	    		QVector<Quad> *quads[] = { &deletes_, &inserts_ };
				QString str;
				TrackerService *ts = dispatcher_->tracker_;
				int rdfs_type_id = ts->id2iri_.reverseAt(Core::rdfs::type::encodedIri());
				int class_id = ts->id2iri_.reverseAt(dispatcher_->class_uri);
	    		for(int i = 0; i < 2; ++i)
		    	{
					for(QVector<Quad>::const_iterator cqi = quads[i]->constBegin()
							, cqiend = quads[i]->constEnd(); cqi != cqiend; ++cqi)
						if(cqi->predicate == rdfs_type_id)
						{
							if(cqi->object == class_id)
								(i ? added_subjects : removed_subjects)
										.insert(ts->id2iri_.at(cqi->subject), 0);
						} else
						{
							if(cqi->subject)
								changed_subjects.insert(ts->id2iri_.at(cqi->subject), 0);
							if(cqi->predicate)
								changed_properties.insert(ts->id2iri_.at(cqi->predicate), 0);
						}
		    	}
	    		if(added_subjects.size())
	    			dispatcher_->handleAddedSubjects(added_subjects.keys());
	    		if(removed_subjects.size())
	    			dispatcher_->handleRemovedSubjects(removed_subjects.keys());
	    		if(changed_subjects.size())
					dispatcher_->handleChangedSubjects
							(changed_subjects.keys(), changed_properties.keys());
			}

		}
	}
}

