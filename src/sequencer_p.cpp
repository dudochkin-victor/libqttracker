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
 * sequencer_p.cpp
 *
 *  Created on: Sep 13, 2010
 *      Author: iridian
 */

#include <QDebug>
#include <QCoreApplication>
#include "sequencer_p.h"

namespace SopranoLive
{
	enum StateFlags
	{	ScopeMask = DependencyChain::All | DependencyChain::Any | DependencyChain::Latest
	,	Branch = DependencyChain::Latest << 1
	,	Running = Branch << 1
	};

	Detail::DependencyChainNode::DependencyChainNode
			( int flags, QString quality)
		: flags_(flags), quality_(quality)
	{}

	Detail::DependencyChainNode::DependencyChainNode
			( int flags, QList<DependencyChain> const &chain_branches)
		: flags_(flags | Branch)
		, branches_(ChainBranches(new QList<DependencyChain>(chain_branches)))
	{}

	QDebug Detail::operator<<(QDebug d, DependencyChainNode const &node)
	{
		d.nospace() << "DCN(" << node.flags_ << ", " << node.quality_;
		if(QList<DependencyChain> const *branches = node.branches_.data())
			for(QList<DependencyChain>::const_iterator dci = branches->begin()
					, dciend = branches->end(); dci != dciend; ++dci)
				d.nospace() << ", Branch{" << *dci << "}";
		return d << ")";
	}

	/*!
	 * A data structure describing a chain of dependency steps.
	 * \n It is used with \ref Sequencer::addOperation to determine the starting conditions of
	 * an operation. A simple dependency can be specified with
	 * \code
DependencyChain().allFinished("read_operation");
	 * \endcode
	 * which describes a dependency on all operations marked as "read_operation" to be finished.
	 * Each step is associated with a \a condition like the "AllFinished" above, and a set of
	 * sub-objects, like the operations filtered by "read_operation" above. These sub-objects
	 * can be also be a set of recursive dependency chains:
	 * \code
DependencyChain().anyFinished(DependencyChains()
		<< DependencyChain().allFinished("read_operation")
		<< DependencyChain().allFinished("write_operation"));
	 * \endcode
	 * This describes a dependency on either all read operations or all write operations to be
	 * complete. The dependencies can also be temporally chained:
	 * \code
DependencyChain().allSucceeded("read_operation").allSucceeded("write_operation"));
	 * \endcode
	 * Which will first wait all read operations to succeed, after which it will wait for
	 * any remaining write operations to succeed, ignoring any read operations that failed
	 * before the last read operation succeeded (TODO: dummy example, come up with a better one).
	 * \n
	 * \n Formally, each step has four states, just like the operation has
	 * (see \ref Sequencer::addOperation): Not started, started, succeeded and failed.
	 * \n Each step also has a \a condition and a set of \a sub-objects. The condition is used to
	 * determine the state of the step from the state of the sub-objects as follows:
	 * \n 1. Step is Started when any sub-object has been started.
	 * \n 2. Step is Succeeded when the condition is satisfied by the sub-object states.
	 * \n 3. Step is Failed when the condition can no longer be satisfied by any subsequent
	 * sub-object changes.
	 * \n
	 * \n The state of a step is evaluated only after the previous step has succeeded or if the
	 * step is the first step.
	 * \n
	 * The state of the whole chain goes according to following rules:
	 * \n 1. Chain is Started when any step has started.
	 * \n 2. Chain is Succeeded when last step has succeeded.
	 * \n 3. Chain is Failed when any step has failed.
	 */
	struct DependencyChain;

	using Detail::DependencyChainNode;

	//! require operations with given \a quality to fulfill given \a condition
	DependencyChain &DependencyChain::addStep(int condition, QString quality)
	{
		push_back(DependencyChainNode(condition, quality));
		return *this;
	}

	//! require given \a chain_branches to fulfill given \a condition
	DependencyChain &DependencyChain::addStep
			( int condition, QList<DependencyChain> const &chain_branches)
	{
		push_back(DependencyChainNode(condition, chain_branches));
		return *this;
	}

	//! require all actions with given \a quality to have started
	DependencyChain &DependencyChain::allStarted(QString quality)
	{
		return addStep(AllStarted, quality);
	}
	//! require all given \a chain_branches to have started
	DependencyChain &DependencyChain::allStarted(QList<DependencyChain> const &chain_branches)
	{
		return addStep(AllStarted, chain_branches);
	}
	//! require all actions with given \a quality to have finished
	DependencyChain &DependencyChain::allFinished(QString quality)
	{
		return addStep(AllFinished, quality);
	}
	//! require all given \a chain_branches to have finished
	DependencyChain &DependencyChain::allFinished(QList<DependencyChain> const &chain_branches)
	{
		return addStep(AllFinished, chain_branches);
	}
	//! require all actions with given \a quality to have finished with success
	DependencyChain &DependencyChain::allSucceeded(QString quality)
	{
		return addStep(AllSucceeded, quality);
	}
	//! require all given \a chain_branches to have finished with success
	DependencyChain &DependencyChain::allSucceeded(QList<DependencyChain> const &chain_branches)
	{
		return addStep(AllSucceeded, chain_branches);
	}
	//! require all actions with given \a quality to have finished with failure
	DependencyChain &DependencyChain::allFailed(QString quality)
	{
		return addStep(AllFailed, quality);
	}
	//! require all given \a chain_branches to have finished with failure
	DependencyChain &DependencyChain::allFailed(QList<DependencyChain> const &chain_branches)
	{
		return addStep(AllFailed, chain_branches);
	}

	//! require any action with given \a quality to have started
	DependencyChain &DependencyChain::anyStarted(QString quality)
	{
		return addStep(AnyStarted, quality);
	}
	//! require the any given \a chain_branches to have started
	DependencyChain &DependencyChain::anyStarted(QList<DependencyChain> const &chain_branches)
	{
		return addStep(AnyStarted, chain_branches);
	}
	//! require any action with given \a quality to have finished
	DependencyChain &DependencyChain::anyFinished(QString quality)
	{
		return addStep(AnyFinished, quality);
	}
	//! require the any given \a chain_branches to have finished
	DependencyChain &DependencyChain::anyFinished(QList<DependencyChain> const &chain_branches)
	{
		return addStep(AnyFinished, chain_branches);
	}
	//! require any action with given \a quality to have finished with success
	DependencyChain &DependencyChain::anySucceeded(QString quality)
	{
		return addStep(AnySucceeded, quality);
	}
	//! require the any given \a chain_branches to have finished with success
	DependencyChain &DependencyChain::anySucceeded(QList<DependencyChain> const &chain_branches)
	{
		return addStep(AnySucceeded, chain_branches);
	}
	//! require any action with given \a quality to have finished with failure
	DependencyChain &DependencyChain::anyFailed(QString quality)
	{
		return addStep(AnyFailed, quality);
	}
	//! require the any given \a chain_branches to have finished with failure
	DependencyChain &DependencyChain::anyFailed(QList<DependencyChain> const &chain_branches)
	{
		return addStep(AnyFailed, chain_branches);
	}

	//! require latest action with given \a quality to have started
	DependencyChain &DependencyChain::latestStarted(QString quality)
	{
		return addStep(LatestStarted, quality);
	}
	//! require latest action with given \a quality to have finished
	DependencyChain &DependencyChain::latestFinished(QString quality)
	{
		return addStep(LatestFinished, quality);
	}
	//! require latest action with given \a quality to have finished with success
	DependencyChain &DependencyChain::latestSucceeded(QString quality)
	{
		return addStep(LatestSucceeded, quality);
	}
	//! require latest action with given \a quality to have finished with failure
	DependencyChain &DependencyChain::latestFailed(QString quality)
	{
		return addStep(LatestFailed, quality);
	}



	SequencerSignal::SequencerSignal(QWeakPointer<QObject> const &object, const char *signal_name)
		: object_(object), object_ref_(), signal_name_(signal_name)
	{}

	SequencerSignal::SequencerSignal(const char *signal_name, QSharedPointer<QObject> const &object)
		: object_(object), object_ref_(object), signal_name_(signal_name)
	{}

	SequencerSignal::~SequencerSignal()
	{}

	SequencerSignal SequencerSignal::shared
			( QSharedPointer<QObject> const &object, const char *signal_name)
	{ return SequencerSignal(signal_name, object); }


	QWeakPointer<QObject> SequencerSignal::object() const { return object_; }
	const char *SequencerSignal::signalName() const { return signal_name_; }



	SequencerSlotCall::SequencerSlotCall
			( QWeakPointer<QObject> const &object, const char *slot_name)
		: object_(object), slot_name_(slot_name)
	{}

	SequencerSlotCall::SequencerSlotCall
			( const char *slot_name, QSharedPointer<QObject> const &object)
		: object_(object), slot_name_(slot_name)
	{}

	SequencerSlotCall::~SequencerSlotCall()
	{}

	SequencerSlotCall SequencerSlotCall::shared
			( QSharedPointer<QObject> const &object, const char *slot_name)
	{	return SequencerSlotCall(slot_name, object); }

	QWeakPointer<QObject> SequencerSlotCall::object() const { return object_; }
	const char *SequencerSlotCall::slotName() const { return slot_name_; }




	SequencerSubObjectParent::SequencerSubObjectParent(SequencerSubObjectParent *parent)
		: parent_(parent), state_(SequencedOperation::NotStarted)
	{}

	void SequencerSubObjectParent::subObjectStarted()
	{
		if(state_)
			return;
		state_ = SequencedOperation::Started;
		if(parent_)
			parent_->subObjectStarted();
	}

	SequencerStep::SequencerStep
			( SequenceOperation *operation, SequencerSubObjectParent *parent, int flags)
		: SequencerSubObjectParent(parent), operation_(operation), flags_(flags)
	{
		// << "SequencerStep(" << this << ") created in" << operation_;
	}

	SequencerStep::~SequencerStep()
	{
		// << "SequencerStep(" << (void*)this << ") destroyed in SequenceOperation(" << (void*)operation_ << ")";
	}

	void SequencerStep::primeSubObjects(int count)
	{
		remaining_sub_objects_ = count;
		required_sub_objects_
				= flags_ & (DependencyChain::Any | DependencyChain::Latest)
						? 1
						: remaining_sub_objects_;
		// << "SequenceStep(" << this << ") primed with" << flags_ << "and required"
		//		"(" << required_sub_objects_ << "), remaining(" << remaining_sub_objects_ << ")";
	}

	void SequencerStep::subObjectEvent(int trigger)
	{
		// << "SequenceStep(" << this << ") received event" << trigger << "with" << flags_ << "and required"
		//		"(" << required_sub_objects_ << "), remaining(" << remaining_sub_objects_  << ")";
		if((trigger & flags_)
				&& !--required_sub_objects_
				&& parent_)
			parent_->subObjectSucceeded();
		if(trigger & DependencyChain::Finished
				&& (--remaining_sub_objects_ < required_sub_objects_)
				&& parent_)
			parent_->subObjectFailed(SequencedOperation::FailedDependencies);
	}

	void SequencerStep::subObjectStarted()
	{
		// << operation_ << __func__;
		SequencerSubObjectParent::subObjectStarted();
		subObjectEvent(DependencyChain::Started);
	}
	void SequencerStep::subObjectSucceeded()
	{
		// << operation_ << __func__;
		SequencerSubObjectParent::subObjectStarted();
		subObjectEvent(DependencyChain::Succeeded);
	}
	void SequencerStep::subObjectFailed(SequencedOperation::State failure)
	{
		// << operation_ << __func__;
		Q_UNUSED(failure);
		subObjectEvent(DependencyChain::Failed);
	}

	SequencerBranchStep::SequencerBranchStep
			(SequenceOperation *operation, SequencerSubObjectParent *parent, int flags
			, QList<DependencyChain> const &branches)
		: SequencerStep(operation, parent, flags), branches_()
	{
		branches_.reserve(branches.size());
		primeSubObjects(branches.size());
		for(QList<DependencyChain>::const_iterator ccbi = branches.begin()
				, ccbiend = branches.end(); ccbi != ccbiend; ++ccbi)
		{
			branches_.push_back(QSharedPointer<SequencerDependencyChain>
					(new SequencerDependencyChain(operation, *ccbi, this)));
			branches_.back()->evaluateCurrentStep();
		}
	}

	SequencerDependencyChain::SequencerDependencyChain
			( SequenceOperation *operation
			, DependencyChain const &dependency_steps, SequencerSubObjectParent *parent)
		: SequencerSubObjectParent(parent)
		, operation_(operation)
		, dependency_steps_(dependency_steps), parent_(parent)
		, current_step_i_(dependency_steps_.begin())
	{
		// << "SequencerDependencyChain(" << this << ") created";
	}

	SequencerDependencyChain::~SequencerDependencyChain()
	{
		// << "SequencerDependencyChain(" << (void*)this << ") destroyed";
	}

	void SequencerDependencyChain::evaluateCurrentStep()
	{
		if(current_step_i_ == dependency_steps_.end())
		{
			current_step_.clear();
			parent_->subObjectSucceeded();
			return;
		}
		else if(current_step_i_->flags_ & Branch)
		{
			if(current_step_i_->branches_->size())
			{
				current_step_ = QSharedPointer<SequencerStep>(new SequencerBranchStep
						(operation_, this, current_step_i_->flags_, *current_step_i_->branches_));
				return;
			}
		}
		else
		{
			current_step_ = QSharedPointer<SequencerStep>
					(new SequencerStep(operation_, this, current_step_i_->flags_));
			int depended_operations = 0;
			int started_operations = 0;
			for(SequenceOperations::iterator soi = operation_->self_ref_
					, soibegin = operation_->sequencer_->begin(); soi != soibegin; )
				if((*--soi)->hasQuality(current_step_i_->quality_))
				{
					++depended_operations;
					// << operation_ << "adds" << current_step_i_->flags_
					//		<< "dependency to" << soi->data();
					(*soi)->addDependent(current_step_);
					if((*soi)->state() == SequencedOperation::Started)
						started_operations++;
					if(current_step_i_->flags_ & DependencyChain::Latest)
						break;
				}
			if(depended_operations)
			{
				current_step_->primeSubObjects(depended_operations);
				QWeakPointer<SequencerStep> weak_step = current_step_;
				while(started_operations--)
					if(QSharedPointer<SequencerStep> step = weak_step.toStrongRef())
						step->subObjectStarted();
				return;
			}
		}
		++current_step_i_;
		evaluateCurrentStep();
	}

	void SequencerDependencyChain::subObjectSucceeded()
	{
		++current_step_i_;
		evaluateCurrentStep();
	}
	void SequencerDependencyChain::subObjectFailed(SequencedOperation::State failure)
	{
		current_step_.clear();
		parent_->subObjectFailed(failure);
	}

	SequenceOperation::SequenceOperation(Sequencer::Private *sequencer
			, QSet<QString> const &qualities, DependencyChain const &dependencies
			, SequencerSlotCall starting_slot
			, QList<SequencerSignal> success_signals, QList<SequencerSignal> failure_signals
			, Qt::ConnectionType connection_type)
		: SequencerSubObjectParent(0)
		, qualities_(qualities)
		, starting_slot_(starting_slot)
		, success_signals_(success_signals), failure_signals_(failure_signals)
		, connection_type_(connection_type)
		, sequencer_(sequencer)
		, dependency_chain_(this, dependencies, this)
	{
		// << this << "created";
	}

	SequenceOperation::~SequenceOperation()
	{
		// << "SequenceOperation(" << (void*)this << ") destroyed";
	}

	SequencedOperation::State SequenceOperation::state() const { return state_; }

	QString SequenceOperation::stateString() const
	{
		if(state_ == SequencedOperation::NotStarted)
			return "NotStarted";
		else if(state_ == SequencedOperation::Started)
			return "Started";
		else if(state_ == SequencedOperation::Succeeded)
			return "Succeeded";
		else if(state_ == SequencedOperation::FailedSignal)
			return "FailedSignal";
		else if(state_ == SequencedOperation::FailedDependencies)
			return "FailedDependencies";
		else if(state_ == SequencedOperation::FailedNullStartSignalObject)
			return "FailedNullStartSignalObject";
		else if(state_ == SequencedOperation::FailedStartSignalInvocation)
			return "FailedStartSignalInvocation";
		else if(state_ == SequencedOperation::FailedNoSuccessSignalers)
			return "FailedNoSuccessSignalers";
		else if(state_ == SequencedOperation::FailedAborted)
			return "FailedAborted";
		return "";
	}

	QSet<QString> SequenceOperation::qualities() const { return qualities_; }
	DependencyChain SequenceOperation::dependencies() const { return dependency_chain_.dependency_steps_; }

	SequencerSlotCall SequenceOperation::startingSlot() const { return starting_slot_; }
	QList<SequencerSignal> SequenceOperation::successSignals() const { return success_signals_; }
	QList<SequencerSignal> SequenceOperation::failureSignals() const { return failure_signals_; }
	Qt::ConnectionType SequenceOperation::connectionType() const { return connection_type_; }

	void SequenceOperation::abort()
	{
		if(state_ < SequencedOperation::Succeeded)
		{
			state_ = SequencedOperation::FailedAborted;
			sequencer_->signalDependents(this);
		}
	}

	void SequenceOperation::waitForStarted()
	{
		QSharedPointer<SequenceOperation> self_holder = *self_ref_;

		QEventLoop loop;
		while(state() < SequencedOperation::Started)
		{
			QCoreApplication::sendPostedEvents();
			QCoreApplication::processEvents();
		}
	}

	void SequenceOperation::waitForFinished()
	{
		QSharedPointer<SequenceOperation> self_holder = *self_ref_;

		QEventLoop loop;
		while(state() < SequencedOperation::Succeeded)
		{
			QCoreApplication::sendPostedEvents();
			QCoreApplication::processEvents();
		}
	}

	void SequenceOperation::prime(SequenceOperations::iterator self_ref)
	{
		self_ref_ = self_ref;
		dependency_chain_.evaluateCurrentStep();
	}

	void SequenceOperation::start()
	{
		QSet<QObject *> processed_success_emitters;
		// register success signals
		for(QList<SequencerSignal>::const_iterator cssi = success_signals_.begin()
				, cssiend = success_signals_.end(); cssi != cssiend; ++cssi)
			if(QObject *emitter = cssi->object().data())
			{
				connect(emitter, cssi->signalName(), SLOT(success()));
				processed_success_emitters.insert(emitter);
				// << "connected" << emitter << cssi->second << "to success()";
			}

		// register destroyed signals
		for(QSet<QObject *>::const_iterator cpei = processed_success_emitters.begin()
				, cpeiend = processed_success_emitters.end(); cpei != cpeiend; ++cpei)
			connect(*cpei, SIGNAL(destroyed()), SLOT(destroyed()));

		emitting_success_objects_ = processed_success_emitters.size();

		// register failure signals
		for(QList<SequencerSignal>::const_iterator cfsi = failure_signals_.begin()
				, cfsiend = failure_signals_.end(); cfsi != cfsiend; ++cfsi)
			if(QObject *emitter = cfsi->object().data())
				connect(emitter, cfsi->signalName(), SLOT(failure()));
	}

	bool SequenceOperation::hasQuality(QString const &quality) const
	{
		return qualities_.contains(quality);
	}

	void SequenceOperation::addDependent(QWeakPointer<SequencerStep> const &dependent)
	{
		dependents_.push_back(dependent);
	}

	void Sequencer::Private::signalDependents(SequenceOperation *op)
	{
		SequencedOperation::State state = op->state();
		if(state == SequencedOperation::Started)
		{
			public_->started(op);
			for(QList<QWeakPointer<SequencerStep> >::const_iterator cdi = op->dependents_.begin()
					, cdiend = op->dependents_.end(); cdi != cdiend; ++cdi)
				if(QSharedPointer<SequencerStep> dependent = *cdi)
					dependent->subObjectStarted();
			return; // no erase, see end of function
		}

		QSharedPointer<SequenceOperation> holder = *op->self_ref_;
		erase(op->self_ref_);

		if(state == SequencedOperation::Succeeded)
		{
			public_->succeeded(op);
			for(QList<QWeakPointer<SequencerStep> >::const_iterator cdi = op->dependents_.begin()
					, cdiend = op->dependents_.end(); cdi != cdiend; ++cdi)
				if(QSharedPointer<SequencerStep> dependent = *cdi)
					dependent->subObjectSucceeded();
		}
		else
		{
			public_->failed(op);
			for(QList<QWeakPointer<SequencerStep> >::const_iterator cdi = op->dependents_.begin()
					, cdiend = op->dependents_.end(); cdi != cdiend; ++cdi)
				if(QSharedPointer<SequencerStep> dependent = *cdi)
					dependent->subObjectFailed(state);
		}
	}

	void SequenceOperation::subObjectStarted() {}

	void SequenceOperation::subObjectSucceeded()
	{
		if(QObject *object = starting_slot_.object().data())
		{
			start();
			int params = starting_slot_.size();
			if(QMetaObject::invokeMethod
					( object, starting_slot_.slotName(), connection_type_
					, (params > 0 ? starting_slot_[0] : QGenericArgument())
					, (params > 1 ? starting_slot_[1] : QGenericArgument())
					, (params > 2 ? starting_slot_[2] : QGenericArgument())
					, (params > 3 ? starting_slot_[3] : QGenericArgument())
					, (params > 4 ? starting_slot_[4] : QGenericArgument())
					, (params > 5 ? starting_slot_[5] : QGenericArgument())
					, (params > 6 ? starting_slot_[6] : QGenericArgument())
					, (params > 7 ? starting_slot_[7] : QGenericArgument())
					, (params > 8 ? starting_slot_[8] : QGenericArgument())
					))
				state_ = SequencedOperation::Started;
			else
				state_ = SequencedOperation::FailedStartSignalInvocation;
		}
		else
			state_ = SequencedOperation::FailedNullStartSignalObject;
		sequencer_->signalDependents(this);
		if(state_ == SequencedOperation::Started)
			checkRemainingSuccessSignalers();
	}
	void SequenceOperation::subObjectFailed(SequencedOperation::State failure)
	{
		Q_UNUSED(failure);
		state_ = SequencedOperation::FailedDependencies;
		sequencer_->signalDependents(this);
	}

	void SequenceOperation::checkRemainingSuccessSignalers()
	{
		if(state_ > SequencedOperation::Started
				|| emitting_success_objects_)
			return;
		state_ = SequencedOperation::FailedNoSuccessSignalers;
		sequencer_->signalDependents(this);
	}

	void SequenceOperation::success()
	{
		if(state_ == SequencedOperation::Started)
		{
			state_ = SequencedOperation::Succeeded;
			sequencer_->signalDependents(this);
		}
	}
	void SequenceOperation::failure()
	{
		if(state_ == SequencedOperation::Started)
		{
			state_ = SequencedOperation::FailedSignal;
			sequencer_->signalDependents(this);
		}
	}
	void SequenceOperation::destroyed()
	{
		--emitting_success_objects_;
		checkRemainingSuccessSignalers();
	}

	Sequencer::Private::Private(Sequencer *public_)
		: public_(public_)
	{}

	/*! Sequence object slot execution based on dependencies against one or more
	 * signals emitted by objects specified as dependency
	 * \code
	Sequencer seq;
	seq.sequenceItem
		( QStringList() << "write"
		, DependencyChain().allFinished("read")
		, SequencerSlotCall(update_tracker_module_1, SIGNAL(beginUpdate()))
		, SequencerSlotCall(update_tracker_module_1, SLOT(operationFinished()))
		);

	seq.sequenceItem
		( QStringList() << "read"
		, DependencyChain().latestStarted("read").allFinished("write")
		, SequencerSlotCall(select_tracker_module_1, SIGNAL(beginSelect()))
		, SequencerSlotCall(update_tracker_module_1, SLOT(operationFinished()))
		);
	 * \endcode
	 */
	class Sequencer;

	Sequencer::Sequencer()
		: d(new Private(this))
	{}

	Sequencer::~Sequencer()
	{}

	/*!
	 * Adds a new operation to the end of the sequence. An operation is something which
	 * can be started by calling the given \a starting_slot, and which then gets completed
	 * by emitting one of the given \a success_signals or given \a failure_signals.
	 *
	 * An operation also has given \a dependencies which specify the conditions after which
	 * the operation should be started. To that end, an operation has a set of given \a
	 * qualities that can be depended upon. Thus different operations in the sequence can
	 * be started out of order, but never before their dependencies.
	 *
	 * \n \n Each operation has four states: Not started, started, succeeded and failed.
	 * First the operation is \a not \a started. Once its dependencies are met the sequencer
	 * calls the given \a starting_slot and moves the operation into \a started state and
	 * starts listening on the given \a success_signals and \a failure_signals. If any of
	 * the success_signals or failed_signals are received, the operation is moved into
	 * \a succeeded or \a failed states respectively. If the dependencies of an operation
	 * are failed, the operation is put directly into \a failed state without going through
	 * \a started state. After all state changes, all depending operations are checked to see
	 * if they should started or failed directly.
	 *
	 * \param qualities describe the qualities of the operation, so that other operations
	 * can depend on it. The same quality can be shared with many operations but dont need to.
	 *
	 * \param dependencies describe the conditions on when the operation can be started. See
	 * \ref DependencyChain
	 *
	 * \param starting_slot specify the QObject and slot name (dont use SLOT macro) and possible
	 * arguments to the slot which are to be invoked to start the operation.
	 *
	 * \param success_signals specify the list of QObject/signal (use SIGNAL macro) pairs any
	 * of which signals the successful completion of the operation. If all QObject's in this
	 * list are destroyed, or if the list is empty, the operation is considered failed.
	 *
	 * \param failure_signals specify the list of QObject/signal pairs any of which signals
	 * the failed completion of the operation.
	 */
	QSharedPointer<SequencedOperation> Sequencer::addOperation
			( QSet<QString> const &qualities
			, DependencyChain const &dependencies
			, SequencerSlotCall starting_slot
			, QList<SequencerSignal> success_signals
			, QList<SequencerSignal> failure_signals
			, Qt::ConnectionType connection_type)
	{
		Private *dd = d.data();

		QSharedPointer<SequenceOperation> ret(new SequenceOperation
				( dd, qualities, dependencies, starting_slot
				, success_signals, failure_signals
				, connection_type));

		dd->push_back(ret);
		ret->prime(--dd->end());
		return ret;
	}

	//! \overload for \ref addOperation with only singular success and failure signals.
	QSharedPointer<SequencedOperation> Sequencer::addOperation
			( QSet<QString> const &qualities
			, DependencyChain const &dependencies
			, SequencerSlotCall starting_slot
			, SequencerSignal success_signal
			, SequencerSignal failure_signal
			, Qt::ConnectionType connection_type)
	{
		return addOperation
				( qualities, dependencies, starting_slot
				, QList<SequencerSignal>() << success_signal
				, QList<SequencerSignal>() << failure_signal
				, connection_type);
	}

	QDebug operator<<(QDebug debug, SequencedOperation const &sequenced_operation)
	{
		debug << "SequencedOperation(" << &sequenced_operation << ")"
				<< sequenced_operation.qualities()
				<< sequenced_operation.stateString();
		return debug;
	}

	QDebug operator<<(QDebug debug, Sequencer const &sequencer)
	{
		QLinkedList<QSharedPointer<SequenceOperation> > const &ops = *sequencer.d.data();
		debug << "Sequencer(" << &sequencer << "), " << ops.size() << "operations:";
		int i = 1;
		for(QLinkedList<QSharedPointer<SequenceOperation> >::const_iterator csoi = ops.begin()
				, csoiend = ops.end(); csoi != csoiend; ++csoi, ++i)
			debug << i << ":" << **csoi;
		return debug;
	}
}
