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
 * liveresource_p.cpp
 *
 *  Created on: Jan 10, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include "liveresource_p.h"
#include "livenodemodelbase_p.h"
#include "util_p.h"

namespace SopranoLive
{
	bool LiveResource::hasType(LiveNode const &type, RDFStrategyFlags flags) const
	{
		RDFProperty prop(RDFProperty::fromObjectOf<rdf::type>(type, flags));
		return implHasProperties(prop);
	}

	LiveNodes LiveResourceImplBase::implGetProperties
			( RDFProperty &property_data, RDFVariableList const &additional_columns) const
	{
		RDFDerivedSelect columns = graph()->service_context_data_->select();
		property_data.bind(sourceVariable());
		columns.addColumn(property_data, RDFDerivedColumn::BaseColumn);
		if(additional_columns.size())
			columns.addColumns(additional_columns);
		return LiveNodes(implGetProperties(&columns));
	}

	const Node liveresource_null_node = Node();

	ResourceOfGraphBridge::ResourceOfGraphBridge
			( QSharedPointer<RDFGraphImplBase> const &graph, LiveNode const &backend)
		: Base(graph, backend) {}

	LiveNodes ResourceOfGraphBridge::types
			( RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent) const
	{ return graph()->wrapLiveNodes(backend().types(strategy, parent)); }

	LiveNodeModelBasePtr ResourceOfGraphBridge::implGetProperties
			( RDFDerivedSelect *columns, TripleModulePtr const &triple_module) const
	{ return graph()->wrapLiveNodeModelBase(backend().implGetProperties(columns, triple_module)); }

	LiveNodes ResourceOfGraphBridge::implGetProperties
			( RDFProperty &property_data, RDFVariableList const &additional_columns) const
	{ return LiveResourceImplBase::implGetProperties(property_data, additional_columns); }

	LiveNode ResourceOfGraphBridge::getObject
			( LiveNode const &predicate, CppType object_type) const
	{ return graph()->wrapLiveNode(backend().getObject(predicate, object_type)); }

	LiveNode ResourceOfGraphBridge::firstObject
			( LiveNode const &predicate, CppType object_type) const
	{ return graph()->wrapLiveNode(backend().firstObject(predicate, object_type)); }

	LiveNode ResourceOfGraphBridge::setObject
			( LiveNode const &predicate, LiveNode const &object) const
	{ return graph()->wrapLiveNode(backend().setObject(predicate, object)); }

	LiveNode ResourceOfGraphBridge::addObject
			( LiveNode const &predicate, LiveNode const &object) const
	{ return graph()->wrapLiveNode(backend().addObject(predicate, object)); }
	LiveNode ResourceOfGraphBridge::addObject
			( LiveNode const &predicate, CppType object_type) const
	{ return graph()->wrapLiveNode(backend().addObject(predicate, object_type)); }

	LiveNode ResourceOfGraphBridge::getSubject
			( LiveNode const &predicate, CppType subject_type) const
	{ return graph()->wrapLiveNode(backend().getSubject(predicate, subject_type)); }

	LiveNode ResourceOfGraphBridge::firstSubject
			( LiveNode const &predicate, CppType subject_type) const
	{ return graph()->wrapLiveNode(backend().firstSubject(predicate, subject_type)); }

	LiveNode ResourceOfGraphBridge::setSubject
			( LiveNode const &predicate, LiveNode const &subject) const
	{ return graph()->wrapLiveNode(backend().setSubject(predicate, subject)); }

	LiveNode ResourceOfGraphBridge::addSubject
			( LiveNode const &predicate, LiveNode const &subject) const
	{ return graph()->wrapLiveNode(backend().addSubject(predicate, subject)); }
	LiveNode ResourceOfGraphBridge::addSubject
			( LiveNode const &predicate, CppType subject_type) const
	{ return graph()->wrapLiveNode(backend().addSubject(predicate, subject_type)); }

	LiveNode invalidated_dummy_g;

	LiveNode ResourceOfGraphBridge::nonLiveBackend(LiveNode const &backend) const
	{
		warning(graph()) << "trying to access an invalidated resource" << backend.toString();
		QUrl uri = graph()->graphIri();
		uri.addQueryItem("removed_dummy", backend.toString());
		return invalidated_dummy_g = BackEnds::Local::service()->liveNode(uri);
	}

	LiveNode const &ResourceOfGraphBridge::resetBridge(LiveNode const &new_backend)
	{
		return backend_ = new_backend;
	}


	ResourceByGraphQueries::ResourceByGraphQueries(QSharedPointer<RDFGraphImplBase> const &graph)
		: Base(graph) {}

	LiveNodes ResourceByGraphQueries::types
			( RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent) const
	{
		return graph_->modelVariable(sourceVariable().type(), strategy, parent);
	}

	LiveNodeModelBasePtr ResourceByGraphQueries::implGetProperties
			( RDFDerivedSelect *columns, TripleModulePtr const &triple_module) const
	{
		RDFProperty &property_data = columns->refColumns().front().refPropertyData();
		return graph_->baseImplModelQuery
				( columns
				, (property_data.strategy() & RDFStrategy::ModelFlagsMask) | RDFStrategy::Running
				, property_data.parent(), triple_module);
	}

	int ResourceByGraphQueries::implHasProperties(RDFProperty &property_data) const
	{
		RDFSelect sel;
		sel.addColumn
			( property_data.strategy() & RDFStrategy::Object ? "object" : "subject"
			, property_data.bind(sourceVariable()));
		return graph_->modelQuery
				( sel, property_data.strategy(), property_data.parent())->rowCount();
	}

	LiveNode ResourceByGraphQueries::getObject
			( LiveNode const &predicate, CppType object_type) const
	{
		if(LiveNode ret = firstObject(predicate, object_type))
			return ret;
		return addObject(predicate, object_type);
	}

	LiveNode ResourceByGraphQueries::firstObject
			( LiveNode const &predicate, CppType object_type) const
	{
		LiveNodes objects = getObjects(predicate, RDFVariable().isOfType(object_type));
		for(int row = 0, rowend = objects->rowCount(); row != rowend; ++row)
			if(LiveNode node = objects->liveNode(row))
				return node;
		return LiveNode();
	}

	void ResourceByGraphQueries::setObjects
			( LiveNode const &predicate, LiveNodes const &objects) const
	{
		RDFUpdate update = graph_->service_context_data_->update();
		update.addDeletion(sourceVariable(), predicate, RDFVariable());

		LiveNodeList new_objects = objects;
		for(LiveNodeList::const_iterator oci = new_objects.begin()
				; oci != new_objects.end(); ++oci)
			if (!oci->isEmpty())
				update.addInsertion(sourceVariable(), predicate, *oci);

		graph_->executeQuery(update);

	}

	LiveNode ResourceByGraphQueries::setObject
			( LiveNode const &predicate, LiveNode const &object) const
	{
		RDFUpdate update = graph_->service_context_data_->update();
		update.addDeletion(sourceVariable(), predicate, RDFVariable());
		if (!object.isEmpty())
			update.addInsertion(sourceVariable(), predicate, object);
		graph_->executeQuery(update);
		return graph_->liveNode(object);
	}

	LiveNode ResourceByGraphQueries::addObject
			( LiveNode const &predicate, LiveNode const &object) const
	{
		if(object.isEmpty())
			return addObject(predicate, CppType::of<QUrl>());
		graph_->insertTriple(node(), predicate, object);
		return graph_->liveNode(object);
	}

	LiveNode ResourceByGraphQueries::addObject
			( LiveNode const &predicate, CppType object_type) const
	{
		LiveNode ret = graph_->createLiveNode(object_type);
		graph_->insertTriple(node(), predicate, ret);
		return ret;
	}

	void ResourceByGraphQueries::removeObject
			( LiveNode const &predicate, LiveNode const &object) const
	{
		graph_->deleteTriple(node(), predicate, object);
	}

	void ResourceByGraphQueries::removeObjects
			( RDFVariable const &predicate, RDFVariable const &objects) const
	{
		RDFUpdate removes = graph_->service_context_data_->update();
		removes.addDeletion(sourceVariable(), predicate, objects);
		graph_->executeQuery(removes);
	}


	LiveNode ResourceByGraphQueries::getSubject
			( LiveNode const &predicate, CppType subject_type) const
	{
		if(LiveNode ret = firstSubject(predicate, subject_type))
			return ret;
		return addSubject(predicate, subject_type);
	}

	LiveNode ResourceByGraphQueries::firstSubject
			( LiveNode const &predicate, CppType subject_type) const
	{
		LiveNodes subjects = getSubjects(predicate, RDFVariable().isOfType(subject_type));
		for(int row = 0, rowend = subjects->rowCount(); row != rowend; ++row)
			if(LiveNode node = subjects->liveNode(row))
				return node;
		return LiveNode();
	}

	void ResourceByGraphQueries::setSubjects
			( LiveNode const &predicate, LiveNodes const &subjects) const
	{
		RDFUpdate update = graph_->service_context_data_->update();
		update.addDeletion(RDFVariable(), predicate, node());

		LiveNodeList new_subjects = subjects;
		for(LiveNodeList::const_iterator sci = new_subjects.begin()
				; sci != new_subjects.end(); ++sci)
			if (!sci->isEmpty())
				update.addInsertion(*sci, predicate, node());

		graph_->executeQuery(update);
	}

	LiveNode ResourceByGraphQueries::setSubject
			( LiveNode const &predicate, LiveNode const &subject) const
	{
		// TODO: incorrect generic semantics, fix
		return addSubject(predicate, subject);
	}

	LiveNode ResourceByGraphQueries::addSubject
			( LiveNode const &predicate, LiveNode const &subject) const
	{
		if(subject.isEmpty())
			return addSubject(predicate, CppType::of<QUrl>());
		graph_->insertTriple(subject, predicate, node());
		return graph_->liveNode(subject);
	}

	LiveNode ResourceByGraphQueries::addSubject
			( LiveNode const &predicate, CppType subject_type) const
	{
		LiveNode ret = graph_->createLiveNode(subject_type);
		graph_->insertTriple(ret, predicate, node());
		return ret;
	}

	void ResourceByGraphQueries::removeSubject
			( LiveNode const &predicate, LiveNode const &subject) const
	{
		graph_->deleteTriple(node(), predicate, subject);
	}

	void ResourceByGraphQueries::removeSubjects
			( RDFVariable const &predicate, RDFVariable const &subjects) const
	{
		RDFUpdate removes = graph_->service_context_data_->update();
		removes.addDeletion(sourceVariable(), predicate, subjects);
		graph_->executeQuery(removes);
	}

	QSharedPointer<LiveResource> ResourceByGraphQueries::getInterface
			( Node const &/*interface_name*/) const
	{
		QObject ret;
		// TODO: implement interfaces
		// TODO: as an answer to above, already implemented on client side
		return QSharedPointer<LiveResource>();
	}

	QSharedPointer<LiveResource> ResourceByGraphQueries::getInterface
			( QUrl const &/*interface_name*/) const
	{
		// TODO: implement interfaces
		// TODO: as an answer to above, already implemented on client side
		return QSharedPointer<LiveResource>();
	}

	void ResourceByGraphQueries::removeInterface(QUrl const &interface_name) const
	{
		RDFSelect select;
		RDFVariable subject = select.newColumn("subject");
		RDFVariable predicate = select.newColumn("predicate");
		RDFVariable object = select.newColumn("object");
		subject[predicate] = object;

		RDFVariable source = sourceVariable();

		RDFPattern all_objects = subject.pattern().child();
		subject.aliasIn(all_objects) = source;
		predicate.aliasIn(all_objects).property<rdfs::domain>() = interface_name;

		RDFPattern all_subjects = all_objects.union_();
		object.aliasIn(all_subjects) = source;
		predicate.aliasIn(all_subjects).property<rdfs::range>() = interface_name;

		RDFPattern all_types = all_subjects.union_();
		subject.aliasIn(all_types) = source;
		subject.aliasIn(all_types).type() = interface_name;

		LiveNodes model = graph_->modelQuery(select);

		RDFUpdate removes = graph_->service_context_data_->update();

		for(int i = 0, iend = model->rowCount(); i != iend; ++i)
			removes.addDeletion(model->liveNode(model->index(i, 0))
					, model->liveNode(model->index(i, 1))
					, model->liveNode(model->index(i, 2)));

		graph_->executeQuery(removes);
	}

	void ResourceByGraphQueries::remove() const
	{
		graph_->removeResources(sourceVariable());
	}


	void ResourceByGraphQueries::attachInterface
			( QUrl const &interface_name, LiveResource &/*interface*/)
	{
		if(!graph()->discard_interface_modifications)
			graph()->interface_adds.push_back(RDFStatement
					(sourceVariable(), rdf::type::iri(), interface_name));
	}

	void ResourceByGraphQueries::detachInterface
			( QUrl const &/*interface_name*/, LiveResource &/*interface*/)
	{}

	void ResourceByGraphQueries::beginInterfaceModifications(bool discard_interface_modifications)
	{
		if(graph()->interface_adds.size())
			critical() << "ERROR: concurrent access to same graph; "
				"SopranoLive is reentrant but not thread-safe on graph level.";
		else
			graph()->discard_interface_modifications = discard_interface_modifications;
	}

	void ResourceByGraphQueries::endInterfaceModifications()
	{
		if(!graph()->discard_interface_modifications)
		{
			RDFUpdate adds = graph_->service_context_data_->update();
			if(graph()->interface_adds.size())
			{
				adds.addInsertion(graph()->interface_adds);
				graph()->executeQuery(adds);
			}
			graph()->interface_adds.clear();
		}
	}

	void ResourceByGraphQueries::startObserving(QString const &/*property*/)
	{
		warning() << "not implemented";
		// TODO: implement observers and signals
	}
	void ResourceByGraphQueries::stopObserving(QString const &/*property*/)
	{
		warning() << "not implemented";
		// TODO: implement observers and signals
	}


	ResourceFromVariableSource::ResourceFromVariableSource
			( QSharedPointer<RDFGraphImplBase> const &graph
			, RDFVariable const &source, QSharedPointer<ResourceOfGraphBridge> const &parent)
		: ResourceByGraphQueries(graph), source_(source), parent_(parent)
	{
		parent_->resetBridge(initialSharedFromThis<ResourceFromVariableSource>());
	}

	Node const &ResourceFromVariableSource::node() const
	{
		LiveNode node = graph_->liveNode(evaluateSource());
		parent_->resetBridge(node);
			// note that we most likely get destroyed here when refcount goes to 0.
		return node->node();
	}

	RDFVariable ResourceFromVariableSource::sourceVariable() const
	{
		return source_;
	}

	void ResourceFromVariableSource::updateSource(RDFVariable const &new_source)
	{
		source_.switchTarget(new_source);
	}

	ResourceOfService::ResourceOfService
			( QSharedPointer<RDFServiceImplBase> const &graph, Node const &node)
		: ResourceByGraphQueries(graph), node_(node)
	{}

	Node const &ResourceOfService::node() const
	{
		return node_;
	}

	RDFServiceImplBase *ResourceOfService::graph() const
	{
		return static_cast<RDFServiceImplBase *>(graph_.data());
	}

	RDFVariable ResourceOfService::sourceVariable() const
	{
		return node_;
	}
}
