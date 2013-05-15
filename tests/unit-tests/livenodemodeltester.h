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
 * livenodemodeltester.h
 *
 *  Created on: Jun 30, 2010
 *      Author: "Iridian Kiiskinen"
 */

#ifndef SOPRANOLIVE_LIVENODEMODELTESTER_H_
#define SOPRANOLIVE_LIVENODEMODELTESTER_H_

#include <QSignalSpy>
#include <QtTracker/Tracker>

class ModelTest;

namespace SopranoLive
{
	struct LiveNodeModelTester
		: public QObject
		, public LiveNodes
	{
		Q_OBJECT

	public:
		QString name_;
		int debug_verbosity_;
		int timeout_;

		QMap<QString, int> signal_counts_;

		//! most recent signal values
		QModelIndex index_, topleft_, bottomright_;
		int start_, end_;

		QSharedPointer<QSignalSpy> spy_;
		QTimer timer_;

		ModelTest *modelTest;

		LiveNodeModelTester
				( LiveNodes model, QString model_name = QString()
				, int timeout_msec = 5000, int debug_verbosity = 0);
		~LiveNodeModelTester();

		//! Set the timeout of signal waits to given \a timeout_msec
		void setTimeout(int timeout_msec);
		//! Prepare waiting for given signal
		void prepareWaitForModelSignal(const char *signal);
		//! Wait for signal prepared by prepareWaitForModelSignal
		bool waitForModelSignal();
		//! Wait for given \a signal or timeout has elapsed
		bool waitForModelSignal(const char *signal);

		//! RAII struct which calls waitForModelSignal on the model on its destructor
		struct Waiter
		{
			LiveNodeModelTester &tester_;
			mutable bool has_waited_, ret_;
			operator bool();
			Waiter(LiveNodeModelTester &tester);
			Waiter(Waiter const &cp);
			~Waiter();
		private:
			Waiter();
		};
		Waiter modelSignalWaiter(const char *signal);

		//! Initialize waiting for rowsAboutToBeInserted and return a \ret Waiter
		Waiter rowsAboutToBeInsertedWaiter();
		//! Initialize waiting for rowsInserted and return a \ret Waiter
		Waiter rowsInsertedWaiter();
		//! Initialize waiting for rowsAboutToBeRemoved and return a \ret Waiter
		Waiter rowsAboutToBeRemovedWaiter();
		//! Initialize waiting for rowsRemoved and return a \ret Waiter
		Waiter rowsRemovedWaiter();
		//! Initialize waiting for dataChanged and return a \ret Waiter
		Waiter dataChangedWaiter();

		//! Initialize waiting for modelUpdated and return a \ret Waiter
		Waiter modelUpdatedWaiter();
		//! Initialize waiting for derivedsUpdated and return a \ret Waiter
		Waiter derivedsUpdatedWaiter();

		//! \return the count of signals arrived for given \a slot_name.
		//! \param slot_name the name of the slot function, not full signature
		int signalCount(QString slot_name);

		//! Clears all signals
		void clearSignals();

		void dump();

		void runTests();

	public slots:

		void modelUpdated();
		void derivedsUpdated();
		void rowsAboutToBeInserted(const QModelIndex &, int start, int end);
		void rowsInserted(const QModelIndex &, int start, int end);
		void rowsAboutToBeRemoved(const QModelIndex &, int start, int end);
		void rowsRemoved(const QModelIndex &, int start, int end);
		void dataChanged(const QModelIndex &topleft_inc, const QModelIndex &botright_inc);
	};
}
#endif /* SOPRANOLIVE_LIVENODEMODELTESTER_H_ */
