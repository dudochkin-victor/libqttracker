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
 * sequencer.h
 *
 *  Created on: Sep 13, 2010
 *      Author: iridian
 */

#ifndef SOPRANOLIVE_SEQUENCER_H_
#define SOPRANOLIVE_SEQUENCER_H_

#include <QSet>
#include <QList>
#include <QPair>
#include <QString>
#include <QSharedPointer>

namespace SopranoLive
{

	struct DependencyChain;
	namespace Detail
	{
		struct Q_DECL_EXPORT DependencyChainNode
		{
			typedef QSharedPointer<QList<DependencyChain> > ChainBranches;

			DependencyChainNode(int flags, QString quality);
			DependencyChainNode(int flags, QList<DependencyChain> const &chain_branches);

			int flags_;
			QString quality_;
			ChainBranches branches_;
		};
		QDebug operator<<(QDebug d, DependencyChainNode const &node);

		class Q_DECL_EXPORT SequencerArgument
			: public QGenericArgument
		{
		public:
			inline SequencerArgument(const char *aName = 0, void *aData = 0)
				: QGenericArgument(aName, aData) {}
			virtual ~SequencerArgument() {}
		};

			template<typename T>
		class Q_DECL_EXPORT SequencerArgumentStorage
			: public SequencerArgument

		{
			T value_;

		public:
			SequencerArgumentStorage(QArgument<T> const &arg)
				: SequencerArgument(arg.name(), static_cast<void *>(&value_))
				, value_(*static_cast<T const *>(arg.data()))
			{}
			~SequencerArgumentStorage() {};
		};
	}

	struct Q_DECL_EXPORT DependencyChain
		: QList<Detail::DependencyChainNode>
	{
		enum ConditionFlags
		{ 	Started = 1, Succeeded = 2, Failed = 4, Finished = Succeeded | Failed
		, 	All = 0, Any = 8, Latest = 16
		};

		enum Condition
		{	AllStarted = All | Started, AllFinished = All | Finished
		,	AllSucceeded = All | Succeeded, AllFailed = All | Failed
		,	AnyStarted = Any | Started, AnyFinished = Any | Finished
		,	AnySucceeded = Any | Succeeded, AnyFailed = Any | Failed
		,	LatestStarted = Latest | Started, LatestFinished = Latest | Finished
		,	LatestSucceeded = Latest | Succeeded, LatestFailed = Latest | Failed
		};

		DependencyChain &addStep(int condition, QString quality);
		DependencyChain &addStep(int condition, QList<DependencyChain> const &chain_branches);

		DependencyChain &allStarted(QString quality);
		DependencyChain &allStarted(QList<DependencyChain> const &chain_branches);
		DependencyChain &allFinished(QString quality);
		DependencyChain &allFinished(QList<DependencyChain> const &chain_branches);
		DependencyChain &allSucceeded(QString quality);
		DependencyChain &allSucceeded(QList<DependencyChain> const &chain_branches);
		DependencyChain &allFailed(QString quality);
		DependencyChain &allFailed(QList<DependencyChain> const &chain_branches);

		DependencyChain &anyStarted(QString quality);
		DependencyChain &anyStarted(QList<DependencyChain> const &chain_branches);
		DependencyChain &anyFinished(QString quality);
		DependencyChain &anyFinished(QList<DependencyChain> const &chain_branches);
		DependencyChain &anySucceeded(QString quality);
		DependencyChain &anySucceeded(QList<DependencyChain> const &chain_branches);
		DependencyChain &anyFailed(QString quality);
		DependencyChain &anyFailed(QList<DependencyChain> const &chain_branches);

		DependencyChain &latestStarted(QString quality);
		DependencyChain &latestFinished(QString quality);
		DependencyChain &latestSucceeded(QString quality);
		DependencyChain &latestFailed(QString quality);
	};
	typedef QList<DependencyChain> DependencyChains;

	class Q_DECL_EXPORT SequencerSignal
	{
	public:
		SequencerSignal
				( QWeakPointer<QObject> const &object = QWeakPointer<QObject>()
				, const char *signal_name = 0);
		~SequencerSignal();
		static SequencerSignal shared
				( QSharedPointer<QObject> const &object, const char *signal_name);

		QWeakPointer<QObject> object() const;
		const char *signalName() const;

	private:
		SequencerSignal(const char *signal_name, QSharedPointer<QObject> const &object);
		QWeakPointer<QObject> object_;
		QSharedPointer<QObject> object_ref_;
		const char *signal_name_;
	};

	class Q_DECL_EXPORT SequencerSlotCall
	{
	public:
		SequencerSlotCall
				( QWeakPointer<QObject> const &object = QWeakPointer<QObject>()
				, const char *slot_name = 0);
		~SequencerSlotCall();
		static SequencerSlotCall shared
				( QSharedPointer<QObject> const &object, const char *slot_name = 0);

		QWeakPointer<QObject> object() const;
		const char *slotName() const;

			template<typename T_>
		SequencerSlotCall &operator<<(QArgument<T_> const &arg)
		{
			arguments_.push_back(QSharedPointer<Detail::SequencerArgument>
					(new Detail::SequencerArgumentStorage<T_>(arg)));
			return *this;
		}

		int size() const { return arguments_.size(); }
		QGenericArgument const &operator[](int i) const { return *arguments_[i].data(); }

	private:
		SequencerSlotCall(const char *slot_name, QSharedPointer<QObject> const &object);

		QWeakPointer<QObject> object_;
		QSharedPointer<QObject> object_ref_;
		const char *slot_name_;
		QList<QSharedPointer<Detail::SequencerArgument> > arguments_;
	};

	class Q_DECL_EXPORT SequencedOperation
	{
	public:
		enum State
		{	NotStarted
		,	Started
		, 	Succeeded
		, 	FailedSignal
		,	FailedDependencies
		, 	FailedNullStartSignalObject
		, 	FailedStartSignalInvocation
		, 	FailedNoSuccessSignalers
		, 	FailedAborted
		};

		virtual SequencedOperation::State state() const = 0;
		virtual QString stateString() const = 0;

		virtual QSet<QString> qualities() const = 0;
		virtual DependencyChain dependencies() const = 0;
		virtual SequencerSlotCall startingSlot() const = 0;
		virtual QList<SequencerSignal> successSignals() const = 0;
		virtual QList<SequencerSignal> failureSignals() const = 0;
		virtual Qt::ConnectionType connectionType() const = 0;

		virtual void abort() = 0;
		virtual void waitForStarted() = 0;
		virtual void waitForFinished() = 0;
		virtual ~SequencedOperation() {};

		friend QDebug operator<<(QDebug debug, SequencedOperation const &sequencer);
	};

	class Q_DECL_EXPORT Sequencer
		: public QObject
	{
		Q_OBJECT
	public:
		Sequencer();
		~Sequencer();

		QSharedPointer<SequencedOperation> addOperation
				( QSet<QString> const &qualities
				, DependencyChain const &dependencies
				, SequencerSlotCall starting_slot_call
				, QList<SequencerSignal> success_signals
				, QList<SequencerSignal> failure_signals = QList<SequencerSignal>()
				, Qt::ConnectionType connection_type = Qt::AutoConnection);

		QSharedPointer<SequencedOperation> addOperation
				( QSet<QString> const &qualities
				, DependencyChain const &dependencies
				, SequencerSlotCall starting_slot_call
				, SequencerSignal success_signal
				, SequencerSignal failure_signal = SequencerSignal()
				, Qt::ConnectionType connection_type = Qt::AutoConnection);

		struct Private;

		friend QDebug operator<<(QDebug debug, Sequencer const &sequencer);

	Q_SIGNALS:
		void started(SequencedOperation *operation);
		void succeeded(SequencedOperation *operation);
		void failed(SequencedOperation *operation);

	private:
		QSharedDataPointer<Private> d;

		// copying and assignment are disabled
		Sequencer(Sequencer const &cp);
		Sequencer &operator=(Sequencer const &cp);

	};
}
#endif /* SOPRANOLIVE_SEQUENCER_H_ */
