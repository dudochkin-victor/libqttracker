/*
 * This file is part of libqttracker project
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
 * compositemodule_p.h.h
 *
 *  Created on: Jan 4, 2010
 *      Author: "Iridian Kiiskinen"
 */

#ifndef SOPRANOLIVE_COMPOSITEMODULE_P_H_
#define SOPRANOLIVE_COMPOSITEMODULE_P_H_

#include <QtCore/QLinkedList>
#include <typeinfo>
#include "utils/visualizer.h"

namespace SopranoLive
{
	/*
	 * Base class of the \a module mechanism, which is an aspect
	 * oriented tool for dynamically attaching and detaching virtual function
	 * implementations from in the middle of a \a module \a chain.
	 *
	 * Module system builds around four concepts:
	 * Compositor, module, interface and methods.
	 * All of these must conform to requirements below:
	 *
	 * Compositor:
	 *         any class which inherits Compositor<Compositor>
	 *         contains an ordered list of modules, called links
	 *
	 * Module:
	 *         any class which inherits Module<Compositor>
	 *         implements any number of module interfaces through
	 *         ModuleInterface<Interface, Compositor> notation
	 *
	 * Module Interface:
	 *         any class containing module methods
	 *
	 * Module Method:
	 *         has InterfaceChain<Interface> chain parameter in the
	 *         interface, has a default implementation which forwards the
	 *         call forwards in the chain through the \a chain parameter
	 * Compositor base interface
	 */
		template<typename Compositor_>
	struct Compositor;

		template<typename Compositor_>
	struct Module;

		template<typename Interface_, typename Compositor_, typename Derived_, typename BaseCompositor_>
	struct ModuleInterface;

		template<typename Compositor_>
	struct InterfaceChain;

	namespace ModuleDetail
	{
			template<typename Compositor_>
		struct Typedefs
		{
			typedef Compositor_ ModuleCompositor;
			typedef QSharedPointer<Module<Compositor_> > ModulePtr;
			typedef QLinkedList<ModulePtr> ModuleList;
			typedef typename QLinkedList<ModulePtr>::iterator ModuleHandle;
		};

			template<typename Compositor_>
		struct CommonModuleInterface;

			template<typename Compositor_>
		struct Interfaces
			: ModuleDetail::Typedefs<Compositor_>
		{
			/* VZR_CLASS(Interfaces);*/

			Interfaces() : module_interfaces_head_(0) {}
			Interfaces(Interfaces const &) : module_interfaces_head_(0) {}
			CommonModuleInterface<Compositor_> * module_interfaces_head_; // VZR_MEMBER

		};

			template<typename Compositor_>
		struct CommonModuleInterface
		{
			/* VZR_CLASS(CommonModuleInterface);*/

			typedef ModuleDetail::Typedefs<Compositor_> Defs;
			friend struct Compositor<Compositor_>;

		protected:
			void registerInterface(Interfaces<Compositor_> *ifs)
			{
				qSwap(	(ifs
							? ifs
							: dynamic_cast<Interfaces<Compositor_> *>(this)
						)->module_interfaces_head_
					, module_interfaces_next_);
			}

			CommonModuleInterface(Interfaces<Compositor_> *ifs = 0)
				: module_interfaces_next_(this)
			{ registerInterface(ifs); }

			CommonModuleInterface(CommonModuleInterface const &, Interfaces<Compositor_> *ifs = 0)
				: module_interfaces_next_(this)
			{ registerInterface(ifs); }

			virtual void interfaceAttach(typename Defs::ModuleHandle ths, typename Defs::ModuleList &modules) = 0;
			virtual void interfaceDetach(typename Defs::ModuleHandle ths, typename Defs::ModuleList &modules) = 0;

		private:
			CommonModuleInterface * module_interfaces_next_; // VZR_MEMBER

		};

			template<typename Interface_>
		struct Interface
			: Interface_
		{
			/* VZR_CLASS(Interface, (Interface_));*/

			Interface() : next_chain_interface_(this) {}
			Interface(Interface const &) : next_chain_interface_(this) {}

			Interface * next_chain_interface_; // VZR_MEMBER
		};

		struct NullDeleter
		{
			void operator()(void *) {}
		};

	}

	// inherited by all module classes
		template<typename Compositor_>
	struct Module
		: QSharedFromThis<Module<Compositor_>, ModuleDetail::Interfaces<Compositor_> >
	{
		typedef typename ModuleDetail::Typedefs<Compositor_> Defs;

		/* VZR_CLASS(Module
				, (QSharedFromThis<Module<Compositor_>, ModuleDetail::Interfaces<Compositor_> >));*/

	protected:
		// public interface

		// these functions can be overridden by the modules, are never called
		// directly, only through Compositor<Compositor_> module attach
		// functions or by derived class functions calling their base implementation
		friend class Compositor<Compositor_>;

		// \return true if the module can be detached to given \a compositor; false otherwise
		virtual bool aboutToDetach(Compositor_ &compositor) = 0;

		// \return non-null pointer to the module that shall be attached to
		// given \a compositor; null pointer if attach cannot be performed.
		// Returned module must implement same interfaces as this.
		// Single compositor modules typically return a pointer to this.
		// Only called during the attach process. When an overriding attach
		// calls base attach, the returned value of must be respected and
		// returned unaltered. Base attach is thus called if and only if
		// the overriding checks for attach validity pass.
		virtual typename Defs::ModulePtr attach(Compositor_ &compositor) = 0;

		// module has been attached to the given \a compositor
		virtual void attached(Compositor_ &object) = 0;

		// module has been detached to the given \a compositor
		virtual void detached(Compositor_ &parent) = 0;
	};

	// inherited by all compositor classes
		template<typename Compositor_>
	struct Compositor
		: ModuleDetail::Typedefs<Compositor_>
	{
		/* VZR_CLASS(Compositor);*/

		typedef Compositor_ BaseCompositor;
		typedef ModuleDetail::Typedefs<Compositor_> Defs;
		typedef typename Defs::ModulePtr ModulePtr;
		typedef typename Defs::ModuleHandle ModuleHandle;

		QLinkedList<ModulePtr> const &modules() const { return modules_; }

		ModuleHandle attachModule(ModuleHandle const &before, ModulePtr module)
		{
			if(!module)
				return modules_.insert(before, module);

			if(!(module = module->attach(static_cast<Compositor_ &>(*this))))
				return modules_.end();

			ModuleHandle module_handle = modules_.insert(before, module);

			for(ModuleDetail::CommonModuleInterface<Compositor_> *ii = module->module_interfaces_head_
					; ii; ii = ii->module_interfaces_next_)
				ii->interfaceAttach(module_handle, modules_);

			module->attached(static_cast<Compositor_ &>(*this));

			return module_handle;
		}
		ModuleHandle attachFrontModule(ModulePtr const &module = ModulePtr())
		{ return attachModule(modules_.begin(), module); }

		ModuleHandle attachBackModule(ModulePtr const &module = ModulePtr())
		{ return attachModule(modules_.end(), module); }

		bool detachModule(ModuleHandle const &module_handle)
		{
			ModulePtr module = *module_handle;
			if(!module)
				return modules_.erase(module_handle), true;

			if(!module->aboutToDetach(*static_cast<Compositor_ *>(this)))
				return false;

			for(ModuleDetail::CommonModuleInterface<Compositor_> *ii = module->module_interfaces_head_
					; ii; ii = ii->module_interfaces_next_)
				ii->interfaceDetach(module_handle, modules_);

			modules_.erase(module_handle);

			module->detached(static_cast<Compositor_ &>(*this));

			return true;
		}

		ModuleHandle replaceModule(ModuleHandle const &module_handle, ModulePtr other_module)
		{
			Compositor_ *ths = static_cast<Compositor_ *>(this);
			ModulePtr &module = *module_handle;

			if(module == other_module)
				return module_handle;

			if((module && !module->aboutToDetach(*ths))
				|| (other_module && !(other_module = other_module->attach(*ths))))
				return modules_.end();

			if(module)
				for(ModuleDetail::CommonModuleInterface<Compositor_> *ii = module->module_interfaces_head_
						; ii; ii = ii->module_interfaces_next_)
					ii->interfaceDetach(module_handle, modules_);

			qSwap(module, other_module);

			if(module)
			{
				for(ModuleDetail::CommonModuleInterface<Compositor_> *ii = module->module_interfaces_head_
						; ii; ii = ii->module_interfaces_next_)
					ii->interfaceAttach(module_handle, modules_);
				module->attached(*ths);
			}
			if(other_module)
				other_module->detached(*ths);
			return module_handle;
		}

		//! Destructs the compositor and implicitly detaches all modules.
		//! \n If this function is part of a memory corrupting stack trace, you should
		//! call \ref detachAllCompositorModules (check for more details) explicitly
		//! in your most derived compositor class.
		~Compositor()
		{
			detachAllCompositorModules();
		}

		typedef QLinkedList<ModulePtr> CallChainCache;
		CallChainCache callChainCache() const { return modules_; }

		//! Dump attached module information
		void dumpModuleInfo(QDebug debug) const
		{
			debug << "Attached modules for " << typeid(this).name() << "(" << this << "):";
			for(typename QLinkedList<ModulePtr>::const_iterator ci = modules_.begin()
					, ciend = modules_.end(); ci != ciend; ++ci)
				if(ci->data())
					debug << "\n\t" << typeid(*ci->data()).name() << "(" << ci->data() << ")";
				else
					debug << "\n\tempty module";
		}
	protected:

		//! Detaches all modules from the compositor. When destructing the compositor,
		//! this function must be called from the most derived class that the modules
		//! rely on. Otherwise the modules might end up accessing already destroyed
		//! members of the compositor.
		void detachAllCompositorModules()
		{
			while(modules_.size())
				detachModule(modules_.begin());
		}


	private:
		QLinkedList<ModulePtr> modules_; // VZR_MEMBER

	};

	// inherited by module classes which implement the given \a Interface_
	// inherits given \a Interface_ indirectly
		template<typename Interface_, typename Compositor_, typename Module_
				, typename BaseCompositor_ = typename Compositor_::BaseCompositor>
	struct ModuleInterface
		: ModuleDetail::Interface<Interface_>
		, ModuleDetail::CommonModuleInterface<BaseCompositor_>
	{
		/* VZR_CLASS(ModuleInterface
				, (ModuleDetail::Interface<Interface_>)
				, (ModuleDetail::CommonModuleInterface<BaseCompositor_>));*/

		typedef typename ModuleDetail::Typedefs<BaseCompositor_> Defs;
		typedef typename ModuleDetail::Interface<Interface_> ModuleDetailInterface;

		ModuleInterface()
			: ModuleDetail::CommonModuleInterface<BaseCompositor_>(static_cast<Module_ *>(this)) {}

		ModuleInterface(ModuleInterface const &cp)
			: ModuleDetail::Interface<Interface_>(cp)
			, ModuleDetail::CommonModuleInterface<BaseCompositor_>(cp, static_cast<Module_ *>(this)) {}

		ModuleDetailInterface *findNext
				(typename Defs::ModuleHandle cur_inclusive, typename Defs::ModuleHandle const &end)
		{
			while(cur_inclusive != end)
				if(Module<BaseCompositor_> *module = cur_inclusive++->data())
					if(ModuleDetailInterface *iface = dynamic_cast<ModuleDetailInterface *>(module))
						return iface;
			return 0;
		}

		ModuleDetailInterface *findPrevious
				(typename Defs::ModuleHandle cur_exclusive, typename Defs::ModuleHandle const &first)
		{
			while(cur_exclusive != first)
				if(Module<BaseCompositor_> *module = (--cur_exclusive)->data())
					if(ModuleDetailInterface *iface = dynamic_cast<ModuleDetailInterface *>(module))
						return iface;
			return 0;
		}

		void interfaceAttach(typename Defs::ModuleHandle ths, typename Defs::ModuleList &modules)
		{
			if(ModuleDetailInterface *prev = findPrevious(ths, modules.begin()))
			{
				this->next_chain_interface_ = prev->next_chain_interface_;
				prev->next_chain_interface_ = this;
			} else
			if(ModuleDetailInterface *next = findNext(++ths, modules.end()))
				this->next_chain_interface_ = next;
			else
				this->next_chain_interface_ = this;
		}

		void interfaceDetach(typename Defs::ModuleHandle ths, typename Defs::ModuleList &modules)
		{
			if(ModuleDetailInterface *iface = findPrevious(ths, modules.begin()))
				iface->next_chain_interface_ = this->next_chain_interface_;
			this->next_chain_interface_ = 0;
		}
	};

		template<typename Interface_, typename Compositor_
			, typename BaseCompositor_ = typename Compositor_::BaseCompositor>
	struct CompositorInterface
		: ModuleInterface<Interface_, Compositor_, Compositor_, BaseCompositor_>
	{
		/* VZR_CLASS(CompositorInterface
				, (ModuleInterface<Interface_, Compositor_, Compositor_, BaseCompositor_>));*/
	};

		template<typename Compositor_>
	struct CompositorAndBaseModule
		: Compositor<Compositor_>
		, Module<Compositor_>
	{
		/* VZR_CLASS(CompositorAndBaseModule
				, (Compositor<Compositor_>)
				, (Module<Compositor_>));*/

		typedef typename Compositor<Compositor_>::ModulePtr ModulePtr;
		typedef typename Compositor<Compositor_>::ModuleHandle ModuleHandle;

		CompositorAndBaseModule()
			: Compositor<Compositor_>(), Module<Compositor_>()
			, compositor_base_module_(Compositor<Compositor_>
					::attachFrontModule(this->template initialSharedFromThis<Module<Compositor_> >
						(ModuleDetail::NullDeleter())))
		{}
		CompositorAndBaseModule(CompositorAndBaseModule const &cp)
			: Compositor<Compositor_>(cp), Module<Compositor_>(cp)
			, compositor_base_module_(Compositor<Compositor_>
					::attachFrontModule(this->template initialSharedFromThis<Module<Compositor_> >
						(ModuleDetail::NullDeleter())))
		{}
		~CompositorAndBaseModule()
		{}
		bool aboutToDetach(Compositor_ &) { return true; }
		typename Module<Compositor_>::ModulePtr attach(Compositor_ &)
		{ return this->sharedFromThis(); }
		void attached(Compositor_ &) {}
		void detached(Compositor_ &) {}

		ModuleHandle attachFrontModule(ModulePtr const &module = ModulePtr())
		{ return attachModule(compositor_base_module_ + 1, module); }
	private:
		ModuleHandle compositor_base_module_; // VZR_MEMBER

	};

		template<typename Interface_, typename ModuleInterface_>
	struct InterfaceChainBase
	{
		/* VZR_CLASS(InterfaceChainBase);*/

		InterfaceChainBase()
			: current_() {}

		InterfaceChainBase(InterfaceChainBase const &cp)
			: current_(cp.current_) {}

		InterfaceChainBase(ModuleInterface_ *current)
			: current_(current) {}

		Interface_ *advance(Interface_ &iface)
		{
			if(!current_ && !(current_ = dynamic_cast<ModuleInterface_ *>(&iface)))
				return 0;
			return current_ = current_->next_chain_interface_;
		}
		Interface_ *operator->() const { return current_; }

		bool isFirstCompositeCall() const
		{
			return current_ == 0;
		}

		/*
		 * Primes the composite call module chain.
		 * \return true if the chain was successfully primed, false if the chain has already
		 * been primed earlier. If priming was successful, the call should be delegated
		 * onwards in the chain. Otherwise, we are at the end of the chain, and the default
		 * implementation for the function should be handled.
		 */
		bool primeCompositeCall(ModuleInterface_ *composite_interface)
		{
			if(!isFirstCompositeCall())
				return false;
			current_ = composite_interface;
			return true;
		}

			template<typename Compositor_>
		typename Compositor_::CallChainCache primeCompositorCall(Compositor_ *comp)
		{
			if(primeCompositeCall(comp))
				return comp->callChainCache();
			return typename Compositor_::CallChainCache();
		}

		ModuleInterface_ * current_; // VZR_MEMBER
	};

		template<typename Interface_>
	struct InterfaceChain
		: public InterfaceChainBase<Interface_, ModuleDetail::Interface<Interface_> >
	{
		typedef InterfaceChainBase<Interface_, ModuleDetail::Interface<Interface_> > Base;

		/* VZR_CLASS(InterfaceChain, (Base));*/

		InterfaceChain() : Base() {}
		InterfaceChain(InterfaceChain const &cp) : Base(cp) {}
		InterfaceChain(Interface_ &current)
			: Base(dynamic_cast<ModuleDetail::Interface<Interface_> *>(&current)) {}
		InterfaceChain(ModuleDetail::Interface<Interface_> &current) : Base(&current) {}

		// disabled: you must use the InterfaceChain<Iface const> notation for const functions
		InterfaceChain(ModuleDetail::Interface<Interface_> const &current);
	};

		template<typename Interface_>
	struct InterfaceChain<Interface_ const>
		: public InterfaceChainBase<Interface_ const, ModuleDetail::Interface<Interface_> const>
	{
		typedef InterfaceChainBase<Interface_ const, ModuleDetail::Interface<Interface_> const>
				Base;

		/* VZR_CLASS(InterfaceChain, (Base));*/

		InterfaceChain() : Base() {}
		InterfaceChain(InterfaceChain const &cp) : Base(cp) {}
		InterfaceChain(Interface_ const &current)
			: Base(dynamic_cast<ModuleDetail::Interface<Interface_> const *>(&current)) {}
		InterfaceChain(ModuleDetail::Interface<Interface_> const &current) : Base(&current) {}
		InterfaceChain(InterfaceChain<Interface_> const &cp) : Base(cp.current_) {}
	};

	// module that can belong to only one compositor at a time
		template<typename Compositor_>
	struct SingleModule
		: Module<Compositor_>
	{
		/* VZR_CLASS(SingleModule, (Module<Compositor_>));*/

	protected:
		Compositor_ * compositor_; // VZR_MEMBER


		bool aboutToDetach(Compositor_ &compositor) { return compositor_ == &compositor; }
		typename Module<Compositor_>::ModulePtr attach(Compositor_ &compositor)
		{
			if(compositor_)
				return typename Module<Compositor_>::ModulePtr();
			compositor_ = &compositor;
			return this->sharedFromThis();
		}
		void attached(Compositor_ &compositor) { Q_UNUSED(compositor); }
		void detached(Compositor_ &compositor) { compositor_ = 0; Q_UNUSED(compositor); }
	public:

		SingleModule(Compositor_ *compositor = 0) : compositor_(compositor) {}
		// when copying a module, the copy is not attached
		SingleModule(SingleModule const &) : compositor_(0) {}

		Compositor_ *compositor() const { return compositor_; }
	};

		template<typename DerivedCompositor_, typename Base_>
	struct SingleModuleOfDerivedCompositor
		: Base_
	{
		/* VZR_CLASS(SingleModuleOfDerivedCompositor, (Base_));*/

		SingleModuleOfDerivedCompositor(DerivedCompositor_ *compositor = 0) : Base_(compositor) {}

		typename Base_::ModulePtr attach(typename Base_::ModuleCompositor &compositor)
		{
			return dynamic_cast<DerivedCompositor_ *>(&compositor)
					? Base_::attach(compositor)
					: typename Base_::ModulePtr();
		}

		void attached(typename Base_::ModuleCompositor &compositor)
		{ 	attached(static_cast<DerivedCompositor_ &>(compositor)); }

		void detached(typename Base_::ModuleCompositor &compositor)
		{ 	detached(static_cast<DerivedCompositor_ &>(compositor)); }

		virtual void attached(DerivedCompositor_ &compositor) { Base_::attached(compositor); }
		virtual void detached(DerivedCompositor_ &compositor) { Base_::detached(compositor); }

		DerivedCompositor_ *compositor() const
		{
			return static_cast<DerivedCompositor_ *>(Base_::compositor());
		}
	};

}
#endif /* SOPRANOLIVE_COMPOSITEMODULE_P_H_ */
