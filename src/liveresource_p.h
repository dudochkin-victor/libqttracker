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
 * liveresource_p.h
 *
 *  Created on: Jan 11, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_LIVERESOURCE_P_H
#define SOPRANOLIVE_LIVERESOURCE_P_H

#include <QStringList>
#include <QtDebug>

#include "include/sopranolive/liveresourcebridge.h"
#include "include/sopranolive/unbound/rdfderivedselect.h"
#include "rdfservicebase_p.h"

namespace SopranoLive
{

	class LiveResourceImplBase
		: public LiveResource
	{
		/* VZR_CLASS(LiveResourceImplBase, (LiveResource));*/

	public:
		typedef RDFGraphImplBase GraphType;
		virtual GraphType *graph() const = 0;
		virtual LiveNodeModelBasePtr implGetProperties
				( RDFDerivedSelect *columns, TripleModulePtr const &triple_module) const = 0;
		LiveNodeModelBasePtr implGetProperties(RDFDerivedSelect *columns) const;
		LiveNodes implGetProperties
				( RDFProperty &property_data
				, RDFVariableList const &additional_columns = RDFVariableList()) const;
	};

	class ResourceOfGraphBridge
		: public EntityOfGraph<LiveResourceBridgeMixin<LiveResourceImplBase> >
	{
		/* VZR_CLASS(ResourceOfGraphBridge
				, (EntityOfGraph<LiveResourceBridgeMixin<LiveResourceImplBase> >));*/

	public:
		typedef EntityOfGraph<LiveResourceBridgeMixin<LiveResourceImplBase> > Base;
		ResourceOfGraphBridge
				( QSharedPointer<RDFGraphImplBase> const &graph, LiveNode const &backend);

		LiveNodes types
				( RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) const;

		LiveNodeModelBasePtr implGetProperties
				( RDFDerivedSelect *columns, TripleModulePtr const &triple_module) const;
		LiveNodes implGetProperties
				( RDFProperty &property_data
				, RDFVariableList const &additional_columns = RDFVariableList()) const;

		LiveNode getObject(LiveNode const &predicate, CppType object_type = CppType()) const;
		using LiveResource::getObject;

		LiveNode firstObject(LiveNode const &predicate, CppType object_type = CppType()) const;
		using LiveResource::firstObject;

		LiveNode setObject(LiveNode const &predicate, LiveNode const &object) const;

		LiveNode addObject(LiveNode const &predicate, LiveNode const &object) const;
		LiveNode addObject(LiveNode const &predicate, CppType object_type) const;
		using LiveResource::addObject;

		LiveNode getSubject(LiveNode const &predicate, CppType subject_type = CppType()) const;
		using LiveResource::getSubject;

		LiveNode firstSubject(LiveNode const &predicate, CppType subject_type = CppType()) const;
		using LiveResource::firstSubject;

		LiveNode setSubject(LiveNode const &predicate, LiveNode const &subject) const;

		LiveNode addSubject(LiveNode const &predicate, LiveNode const &subject) const;
		LiveNode addSubject(LiveNode const &predicate, CppType subject_type) const;

		LiveNode nonLiveBackend(LiveNode const &backend) const;
		LiveNode const &resetBridge(LiveNode const &new_backend);
	};

	class ResourceByGraphQueries
		: public EntityOfGraph<LiveResourceImplBase>
	{
		/* VZR_CLASS(ResourceByGraphQueries
				, (EntityOfGraph<LiveResourceImplBase>));*/

	protected:
		ResourceByGraphQueries(QSharedPointer<RDFGraphImplBase> const &graph);
	public:
		typedef EntityOfGraph<LiveResourceImplBase> Base;
		LiveNodes types
				( RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) const;

		using LiveResourceImplBase::implGetProperties;
		LiveNodeModelBasePtr implGetProperties
				( RDFDerivedSelect *columns, TripleModulePtr const &triple_module) const;

		int implHasProperties(RDFProperty &property_data) const;

		LiveNode getObject(LiveNode const &predicate, CppType object_type = CppType()) const;
		LiveNode firstObject(LiveNode const &predicate, CppType object_type = CppType()) const;
		void setObjects(LiveNode const &predicate, LiveNodes const &objects) const;
		LiveNode setObject(LiveNode const &predicate, LiveNode const &object) const;
		LiveNode addObject(LiveNode const &predicate, LiveNode const &object) const;
		LiveNode addObject(LiveNode const &predicate, CppType object_type) const;
		void removeObject(LiveNode const &predicate, LiveNode const &object) const;
		void removeObjects
				( RDFVariable const &predicate = RDFVariable()
				, RDFVariable const &object = RDFVariable()) const;

		LiveNode getSubject(LiveNode const &predicate, CppType subject_type = CppType()) const;
		LiveNode firstSubject(LiveNode const &predicate, CppType subject_type = CppType()) const;
		void setSubjects(LiveNode const &predicate, LiveNodes const &subjects) const;
		LiveNode setSubject(LiveNode const &predicate, LiveNode const &subject) const;
		LiveNode addSubject(LiveNode const &predicate, LiveNode const &subject) const;
		LiveNode addSubject(LiveNode const &predicate, CppType subject_type) const;
		void removeSubject(LiveNode const &predicate, LiveNode const &subject) const;
		void removeSubjects
				( RDFVariable const &predicate = RDFVariable()
				, RDFVariable const &subject = RDFVariable()) const;

		QSharedPointer<LiveResource> getInterface(Node const &interface_name) const;
		QSharedPointer<LiveResource> getInterface(QUrl const &interface_name) const;

		void removeInterface(QUrl const &interface_name) const;
		void remove() const;

		void attachInterface(QUrl const &interface_name, LiveResource &interface);
		void detachInterface(QUrl const &interface_name, LiveResource &interface);

		void beginInterfaceModifications(bool discard_modifications);
		void endInterfaceModifications();

		void startObserving(QString const &property);
		void stopObserving(QString const &property);
	};

	class ResourceFromVariableSource
		: public ResourceByGraphQueries
	{
		/* VZR_CLASS(ResourceFromVariableSource, (ResourceByGraphQueries));*/

	public:
		ResourceFromVariableSource
				( QSharedPointer<RDFGraphImplBase> const &graph
				, RDFVariable const &source
				, QSharedPointer<ResourceOfGraphBridge> const &parent);

		Node const &node() const;
		RDFVariable sourceVariable() const;

		void updateSource(RDFVariable const &new_source);
	protected:
		virtual Node evaluateSource() const = 0;

		RDFVariableLink source_; // VZR_MEMBER
		QSharedPointer<ResourceOfGraphBridge> parent_; // VZR_MEMBER

	};

	class ResourceOfService
		: public ResourceByGraphQueries
	{
		/* VZR_CLASS(ResourceOfService, (ResourceByGraphQueries));*/

	public:
		ResourceOfService(QSharedPointer<RDFServiceImplBase> const &graph, Node const &node);

		Node const &node() const;
		RDFServiceImplBase *graph() const;
		RDFVariable sourceVariable() const;

	protected:
		Node node_; // VZR_MEMBER

	};

}


#endif /* SOPRANO_LIVERESOURCE_P_H */
