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
 * sequencer_p.h
 *
 *  Created on: Sep 8, 2010
 *      Author: iridian
 */

#ifndef SOPRANOLIVE_SEQUENCER_P_H_
#define SOPRANOLIVE_SEQUENCER_P_H_

#include <QVector>
#include <QLinkedList>
#include "sequencer.h"

namespace SopranoLive
{

	class SequenceOperation;
	typedef QLinkedList<QSharedPointer<SequenceOperation> > SequenceOperations;

	class SequencerSubObjectParent
	{
	public:
		SequencerSubObjectParent(SequencerSubObjectParent *parent);
		virtual void subObjectStarted();
		virtual void subObjectSucceeded() = 0;
		virtual void subObjectFailed
				( SequencedOperation::State failure = SequencedOperation::FailedSignal) = 0;

		virtual ~SequencerSubObjectParent() {}

	protected:
		SequencerSubObjectParent *parent_;
		SequencedOperation::State state_;
	};

	class SequencerStep
		: public SequencerSubObjectParent
	{
	public:
		SequencerStep(SequenceOperation *operation, SequencerSubObjectParent *parent, int flags);
		~SequencerStep();

		void subObjectStarted();
		void subObjectSucceeded();
		void subObjectFailed(SequencedOperation::State failure = SequencedOperation::FailedSignal);
	protected:
		friend class SequencerDependencyChain;
		SequenceOperation *operation_;
		int remaining_sub_objects_;
		int required_sub_objects_;
		int flags_;

		void primeSubObjects(int count);

		void subObjectEvent(int trigger);

	};

	class SequencerDependencyChain
		: public SequencerSubObjectParent
	{
	public:
		SequencerDependencyChain
				( SequenceOperation *operation
				, DependencyChain const &dependency_steps, SequencerSubObjectParent *parent);
		~SequencerDependencyChain();

		SequenceOperation *operation_;
		DependencyChain dependency_steps_;
		SequencerSubObjectParent *parent_;

		DependencyChain::const_iterator current_step_i_;
		QSharedPointer<SequencerStep> current_step_;

		void evaluateCurrentStep();

		void subObjectSucceeded();
		void subObjectFailed(SequencedOperation::State failure = SequencedOperation::FailedSignal);
	};

	class SequencerBranchStep
		: public SequencerStep
	{
	public:
		SequencerBranchStep
				(SequenceOperation *operation, SequencerSubObjectParent *parent, int flags
				, QList<DependencyChain> const &branches);
	private:
		QVector<QSharedPointer<SequencerDependencyChain> > branches_;
	};

	class SequenceOperation
		: public QObject
		, public SequencerSubObjectParent
		, public SequencedOperation
	{
		Q_OBJECT

	public:
		SequenceOperation(Sequencer::Private *sequencer
				, QSet<QString> const &qualities, DependencyChain const &dependencies
				, SequencerSlotCall starting_slot
				, QList<SequencerSignal> success_signals, QList<SequencerSignal> failure_signals
				, Qt::ConnectionType connection_type);
		~SequenceOperation();

		SequencedOperation::State state() const;
		QString stateString() const;

		QSet<QString> qualities() const;
		DependencyChain dependencies() const;
		SequencerSlotCall startingSlot() const;
		QList<SequencerSignal> successSignals() const;
		QList<SequencerSignal> failureSignals() const;
		Qt::ConnectionType connectionType() const;

		void abort();
		void waitForStarted();
		void waitForFinished();

		void prime(SequenceOperations::iterator self_ref);
		bool hasQuality(QString const &quality) const;
		void addDependent(QWeakPointer<SequencerStep> const &dependent);

	private:
		friend class SequencerDependencyChain;
		friend class Sequencer::Private;

		QSet<QString> qualities_;
		SequencerSlotCall starting_slot_;
		QList<SequencerSignal> success_signals_, failure_signals_;
		Qt::ConnectionType connection_type_;
		Sequencer::Private *sequencer_;
		SequencerDependencyChain dependency_chain_;

		SequenceOperations::iterator self_ref_;

		QList<QWeakPointer<SequencerStep> > dependents_;

		int emitting_success_objects_;

		void start();

		void subObjectStarted();
		void subObjectSucceeded();
		void subObjectFailed(SequencedOperation::State failure = SequencedOperation::FailedSignal);

		void checkRemainingSuccessSignalers();

	private Q_SLOTS:

		void success();
		void failure();
		void destroyed();
	};

	class Sequencer::Private
		: public QSharedData
		, public SequenceOperations
	{
		friend class SequenceOperation;
		Sequencer *public_;
	public:
		Private(Sequencer *public_);

		void signalDependents(SequenceOperation *op);
	};
}
#endif /* SOPRANOLIVE_SEQUENCER_P_H_ */
