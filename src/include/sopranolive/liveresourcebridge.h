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
 * liveresourcebridge.h
 *
 *  Created on: Mar 30, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_LIVERESOURCEBRIDGE_H_
#define SOPRANOLIVE_LIVERESOURCEBRIDGE_H_

#include "livenode.h"
#include "liveresource.h"

namespace SopranoLive
{

	/*!
	 * Live entity.
	 * \n By-reference, bridge base class to a \ref LiveResource object using a shared pointer.
	 * \n Used by convenience library to add interfaces to objects.
	 * \sa \ref live_entities
	 */
		template<typename Base_, typename Backend_>
	class Q_DECL_EXPORT LiveResourceBridgeMixin
		: public Base_
	{
	protected:
		LiveNode backend_;

		Backend_ &backend()
		{ return static_cast<Backend_ &>(*(backend_.isLive()
				? backend_
				: nonLiveBackend(backend_)).resource().data()); }

		Backend_ const &backend() const
		{ return static_cast<Backend_ &>(*(backend_.isLive()
				? backend_
				: nonLiveBackend(backend_)).resource().data()); }

		virtual LiveNode nonLiveBackend(LiveNode const &backend) const
		{
			Q_UNUSED(backend);
			// XXXLOG
			// critical()
			// << "non-gracefully handled non-live resource bridge backend for resource:"
			// << backend.toString();
			return LiveNode();
		}
	public:

		QSharedPointer<LiveResource> nodeInterface() { return backend().sharedFromThis(); }
		Node const &node() const { return backend_.node(); }

		typename Base_::GraphType *graph() const { return backend().graph(); }
		RDFVariable sourceVariable() const
		{ return backend_.isLive() ? backend_->sourceVariable() : backend_.variable(); }
		LiveNodes types
				( RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) const
		{ return backend().types(strategy, parent); }

		LiveNodes implGetProperties
				( RDFProperty &property_data
				, RDFVariableList const &additional_columns = RDFVariableList()) const
		{ return backend().implGetProperties(property_data, additional_columns); }
		int implHasProperties(RDFProperty &property_data) const
		{ return backend().implHasProperties(property_data); }

		LiveNode getObject(LiveNode const &predicate, CppType object_type = CppType()) const
		{ return backend().getObject(predicate, object_type); }
		using LiveResource::getObject;
		LiveNode firstObject(LiveNode const &predicate, CppType object_type = CppType()) const
		{ return backend().firstObject(predicate, object_type); }
		using LiveResource::firstObject;
		void setObjects(LiveNode const &predicate, LiveNodes const &objects) const
		{ return backend().setObjects(predicate, objects); }
		LiveNode setObject(LiveNode const &predicate, LiveNode const &object) const
		{ return backend().setObject(predicate, object); }
		LiveNode addObject(LiveNode const &predicate, LiveNode const &object) const
		{ return backend().addObject(predicate, object); }
		LiveNode addObject(LiveNode const &predicate, CppType object_type) const
		{ return backend().addObject(predicate, object_type); }
		using LiveResource::addObject;
		void removeObject(LiveNode const &predicate, LiveNode const &object) const
		{ return backend().removeObject(predicate, object); }
		void removeObjects
				( RDFVariable const &predicate = RDFVariable()
				, RDFVariable const &object = RDFVariable()) const
		{ return backend().removeObjects(predicate, object); }

		LiveNode getSubject(LiveNode const &predicate, CppType subject_type = CppType()) const
		{ return backend().getSubject(predicate, subject_type); }
		using LiveResource::getSubject;
		LiveNode firstSubject(LiveNode const &predicate, CppType subject_type = CppType()) const
		{ return backend().firstSubject(predicate, subject_type); }
		using LiveResource::firstSubject;
		void setSubjects(LiveNode const &predicate, LiveNodes const &subjects) const
		{ return backend().setSubjects(predicate, subjects); }
		LiveNode setSubject(LiveNode const &predicate, LiveNode const &subject) const
		{ return backend().setSubject(predicate, subject); }
		LiveNode addSubject(LiveNode const &predicate, LiveNode const &subject) const
		{ return backend().addSubject(predicate, subject); }
		LiveNode addSubject(LiveNode const &predicate, CppType subject_type) const
		{ return backend().addSubject(predicate, subject_type); }
		using LiveResource::addSubject;
		void removeSubject(LiveNode const &predicate, LiveNode const &subject) const
		{ return backend().removeSubject(predicate, subject); }
		void removeSubjects
				( RDFVariable const &predicate = RDFVariable()
				, RDFVariable const &subject = RDFVariable()) const
		{ return backend().removeSubjects(predicate, subject); }
/*
		QSharedPointer<LiveResource> getInterface(LiveNode const &interface_name) const
		{ return backend().getInterface(interface_name); }
		QSharedPointer<LiveResource> getInterface(QUrl const &interface_name) const
		{ return backend().getInterface(interface_name); }
*/
		void removeInterface(QUrl const &interface_name) const
		{ return backend().removeInterface(interface_name); }
		void remove() const { return backend().remove(); }

	protected:
		LiveResourceBridgeMixin(LiveNode const &backend) : backend_(backend) {}
		LiveResourceBridgeMixin
				( QSharedPointer<Backend_> const &backend = QSharedPointer<Backend_>())
			: backend_(backend) {}
	public:

		void attachInterface(QUrl const &interface_name, LiveResource &interface)
		{ 	if(backend_.isLive())
				static_cast<Backend_ *>(backend_.resource().data())->attachInterface
						(interface_name, interface); }
		void detachInterface(QUrl const &interface_name, LiveResource &interface)
		{ 	if(backend_.isLive())
				static_cast<Backend_ *>(backend_.resource().data())->detachInterface
						(interface_name, interface); }

		void beginInterfaceModifications(bool no_interface_registration = false)
		{ 	if(backend_.isLive())
				static_cast<Backend_ *>(backend_.resource().data())
						->beginInterfaceModifications(no_interface_registration); }
		void endInterfaceModifications()
		{ 	if(backend_.isLive())
				static_cast<Backend_ *>(backend_.resource().data())
						->endInterfaceModifications(); }

		virtual void startObserving(QString const &property)
		{ if(backend_.isLive()) static_cast<Backend_ *>
				(backend_.resource().data())->startObserving(property); }
		virtual void stopObserving(QString const &property)
		{ if(backend_.isLive()) static_cast<Backend_ *>
				(backend_.resource().data())->stopObserving(property); }

			template<typename Interface_>
		void attachInterfaceHelper()
		{ if(backend_.isLive()) static_cast<Backend_ *>(backend_.resource().data())
				->attachInterface(Interface_::iri(), *this);	}

			template<typename Interface_>
		void detachInterfaceHelper()
		{ if(backend_.isLive()) static_cast<Backend_ *>(backend_.resource().data())
				->detachInterface(Interface_::iri(), *this); }

	private:
		friend class LiveResource;
	};

	typedef LiveResourceBridgeMixin<> LiveResourceBridge;

	struct Live_Policy;
		template<typename Base = Live_Policy>
	struct StrictLive_Policy;

	/*!
	 * Live or non-live entity.
	 * \n By-value, smart, live node to \a Resource_ object using a shared pointer.
	 * Is strictly typed, meaning that if the resource it is initialized to does not
	 * have the given \a Resource_ type, it will return to non-live state.
	 * \n It also can be in a non-live state, where it only has a value.
	 * Uses a \ref Soprano::Node to represent the node.
	 * \param Resource_ the type of object referred to as live entity.
	 * \sa \ref live_entities
	 */
		template< typename Resource_>
	class Q_DECL_EXPORT Live
		: public ValueOfLiveResource<Resource_, Live_Policy, Live<Resource_> >
	{
		typedef ValueOfLiveResource<Resource_, Live_Policy, Live<Resource_> > Base;
	public:
		/*!
		 * \ref live_entities "Non-blocking".
		 * Default initialize to non-live empty state.
		 */
		Live() {}

		/*!
		 * \ref live_entities "Non-blocking".
		 * \param has_type true if given \a cp is guaranteed to have the dynamic types
		 * of the \a Resource_
		 * Copy initialize to given \a cp.
		 */
		Live(Live const &cp, bool has_type = true) : Base(cp, has_type) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 * \param has_type true if given \a cp is guaranteed to have the dynamic types
		 * of the \a Resource_
		 * Copy initialize to given \a cp.
		 */
			template<typename OtherResource_, typename OtherPolicy_, typename OtherMD_>
		Live( ValueOfLiveResource<OtherResource_, OtherPolicy_, OtherMD_> const &cp
				, bool has_type = false) : Base(cp, has_type) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to live state to given \a resource.
		 */
		Live(LiveResourcePtr resource) : Base(resource) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to live state to given \a resource.
		 */
			template<typename Other_>
		Live(QSharedVirtualInheritEnabledPointer<Other_, LiveResourcePtr> const &resource)
			: Base(resource) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to live state to given \a resource.
		 */
			template<typename Other_>
		Live(QSharedPointer<Other_> const &resource)
			: Base(resource) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to live state to given \a resource.
		 */
			template<typename Other_>
		Live(QWeakPointer<Other_> const &resource)
			: Base(resource) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to non-live state by given \a node.
		 */
		Live(Node const &node) : Base(node) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to non-live state by given \a iri.
		 */
		Live(QUrl const &iri) : Base(Node(iri)) {}
	};

	/*!
	 * Live or non-live entity.
	 * \n By-value, smart, live node to \a Resource_ object using a shared pointer.
	 * Is loosely typed, meaning that if the resource it is initialized to does not
	 * have the given \a Resource_ type, this type will be added to the resource.
	 * \n It also can be in a non-live state, where it only has a value.
	 * Uses a \ref Soprano::Node to represent the node.
	 * \param Resource_ the type of object referred to as live entity.
	 * \sa \ref live_entities
	 */
		template< typename Resource_>
	class Q_DECL_EXPORT StrictLive
		: public ValueOfLiveResource<Resource_, StrictLive_Policy<>, StrictLive<Resource_> >
	{
		typedef ValueOfLiveResource<Resource_, StrictLive_Policy<>, StrictLive<Resource_> >
				Base;
	public:
		/*!
		 * \ref live_entities "Non-blocking".
		 * Default initialize to non-live empty state.
		 */
		StrictLive() {}

		/*!
		 * \ref live_entities "Non-blocking".
		 * \param has_type true if given \a cp is guaranteed to have the dynamic types of
		 * the \a Resource_
		 * Copy initialize to given \a cp.
		 */
		StrictLive(StrictLive const &cp, bool has_type = true) : Base(cp, has_type) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 * \param has_type true if given \a cp is guaranteed to have the dynamic types of
		 * the \a Resource_
		 * Copy initialize to given \a cp, to live state if cp has type \a resource,
		 * otherwise to non-live state.
		 */
			template<typename OtherResource_, typename OtherPolicy_, typename OtherMD_>
		StrictLive
				( ValueOfLiveResource<OtherResource_, OtherPolicy_, OtherMD_> const &cp
				, bool has_type = false) : Base(cp, has_type) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to live state to given \a resource if it has the type \a Resource_,
		 * otherwise to non-live state.
		 */
		StrictLive(LiveResourcePtr resource) : Base(resource) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to live state to given \a resource if it has the type \a Resource_,
		 * otherwise to non-live state.
		 */
			template<typename Other_>
		StrictLive(QSharedVirtualInheritEnabledPointer<Other_, LiveResourcePtr> const &resource)
			: Base(resource) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to live state to given \a resource if it has the type \a Resource_,
		 * otherwise to non-live state.
		 */
			template<typename Other_>
		StrictLive(QSharedPointer<Other_> const &resource)
			: Base(resource) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to live state to given \a resource if it has the type \a Resource_,
		 * otherwise to non-live state.
		 */
			template<typename Other_>
		StrictLive(QWeakPointer<Other_> const &resource)
			: Base(resource) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to non-live state by given \a node.
		 */
		StrictLive(Node const &node) : Base(node) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to non-live state by given \a iri.
		 */
		StrictLive(QUrl const &iri) : Base(Node(iri)) {}
	};

		template<typename Resource_>
	class Q_DECL_EXPORT LiveResourceBridgeFacade
		: public Resource_
	{
		static LiveResourcePtr const &callBeginInterfaceModifications
				( LiveResourcePtr const &backend, LiveResource &original
				, bool no_interface_registration = false)
		{
			original.beginInterfaceModifications(no_interface_registration);
			return backend;
		}

		static LiveResourcePtr const &callBeginInterfaceModifications
				( LiveResourcePtr const &backend, Resource_ &original
				, bool no_interface_registration = true)
		{
			original.beginInterfaceModifications(no_interface_registration);
			return backend;
		}

	public:
		LiveResourceBridgeFacade(LiveResourcePtr const &backend, bool has_type_checked = false)
			: LiveResourceBridge(callBeginInterfaceModifications
					(backend, *backend, has_type_checked))
		{
			this->backend().endInterfaceModifications();
		}

		LiveResourceBridgeFacade
				( ValueOfLiveResource
						<LiveResource, ValueOfOmniTypeLiveResource_Policy, OmniTypeLiveNode>
								const &backend, bool = false)
			: LiveResourceBridge(callBeginInterfaceModifications
					(backend.entity(), *backend.entity(), true))
		{
			this->backend().endInterfaceModifications();
		}

			template<typename Other_>
		LiveResourceBridgeFacade
				( LiveResourceBridgeFacade<Other_> const &other_
				, bool has_type_checked = false)
			: LiveResourceBridge(callBeginInterfaceModifications
					(other_.backend(), other_, has_type_checked))
		{
			this->backend().endInterfaceModifications();
		}

			template<typename AnyPointer_>
		LiveResourceBridgeFacade(AnyPointer_ const &pointer_, bool has_type_checked = false)
			: LiveResourceBridge(callBeginInterfaceModifications
					( pointer_->template sharedFromThis<LiveResource>()
					, *pointer_, has_type_checked))
		{
			this->backend().endInterfaceModifications();
		}
	};

		// TODO: document or drop QTTRACKER_PERF_RTTI
#if defined(QT_NO_DYNAMIC_CAST) || !defined(QTTRACKER_PERF_RTTI)
		template<typename T>
	static T noRTTIBaseOrNullCast(T base_class) { return base_class; }

		template<typename T>
	static T noRTTIBaseOrNullCast(void *) { return 0; }

#define SOPRANOLIVE_BRIDGE_ENTITY_CAST noRTTIBaseOrNullCast
#else
#define SOPRANOLIVE_BRIDGE_ENTITY_CAST dynamic_cast
#endif

	struct Q_DECL_EXPORT Live_Policy
		: ValueOfLiveResource_Policy
	{
			template<typename Resource_>
		struct Defines
		{
			typedef Node Value;
			typedef Node const *ValuePointer;
			typedef Resource_ Entity;
			typedef QSharedVirtualInheritEnabledPointer<Entity, LiveResourcePtr> EntityPointer;
		};

			template<typename This_, typename OtherEntityPointer_>
		typename This_::EntityPointer bridgeEntity
				( OtherEntityPointer_ const &ptr, bool has_type = false) const
		{
			if(!ptr)
				return typename This_::EntityPointer();
			typename This_::Entity *ret = SOPRANOLIVE_BRIDGE_ENTITY_CAST
					<typename This_::Entity *>(&*ptr);
			if(!ret)
				ret = new LiveResourceBridgeFacade<typename This_::Entity>(ptr, has_type);
			return typename This_::EntityPointer
					( ret, ret->template initialSharedFromThis<LiveResource>());
		}

			template<typename This_, typename Other_>
		void copyConstruct(Other_ const &other, bool has_type = false)
		{
			if(typename This_::EntityPointer bridged
					= md<This_>().template bridgeEntity<This_>(other, has_type))
				md<This_>().entity_ = bridged;
			else
			{
				md<This_>().entity_ = typename This_::EntityPointer();
				md<This_>().value_ = md<This_>().template copyValue<This_>(other.value());
			}
		}
	};

		template<typename Base>
	struct Q_DECL_EXPORT StrictLive_Policy
		: Base
	{
			template<typename This_, typename OtherEntityPointer_>
		typename This_::EntityPointer bridgeEntity
				( OtherEntityPointer_ const &ptr, bool has_type = false) const
		{
			if(ptr)
			{
				typename This_::Entity *ret = SOPRANOLIVE_BRIDGE_ENTITY_CAST
						<typename This_::Entity *>(&*ptr);
				if(ret || ((has_type || ptr.template hasType<typename This_::Entity>())
						&& (ret = new LiveResourceBridgeFacade<typename This_::Entity>
								(ptr, true))))
					return typename This_::EntityPointer
						( ret, ret->template initialSharedFromThis<LiveResource>());
			}
			return typename This_::EntityPointer();
		}
	};

#undef SOPRANOLIVE_BRIDGE_ENTITY_CAST

}

#define SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY
#include "ontologies/core/rdf.h"
#include "ontologies/core/rdfs.h"
#undef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY

namespace SopranoLive
{
		template<typename Type_>
	Q_DECL_EXPORT bool LiveResource::hasType(RDFStrategyFlags flags) const
	{
		RDFProperty type = RDFProperty::fromObjectOf<Ontologies::Core::rdf::type, Type_>(flags);
		return implHasProperties(type);
	}
}

#endif /* LIVERESOURCEBRIDGE_H_ */

