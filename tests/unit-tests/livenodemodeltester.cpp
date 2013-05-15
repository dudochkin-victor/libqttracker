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
 * livenodemodeltester.cpp
 *
 *  Created on: Jun 30, 2010
 *      Author: "Iridian Kiiskinen"
 */

#include "livenodemodeltester.h"
#include "modeltest/modeltest.h"
#include <QSignalSpy>
#include <QTimer>


namespace SopranoLive
{

	LiveNodeModelTester::LiveNodeModelTester
	( LiveNodes nodes, QString name, int timeout_msec, int debug_verbosity)
		: LiveNodes(nodes), name_(name), debug_verbosity_(debug_verbosity), timeout_(timeout_msec),
		  modelTest(0)
	{
		connect(model(), SIGNAL(modelUpdated()), this, SLOT(modelUpdated()));
		connect(model(), SIGNAL(derivedsUpdated()), this, SLOT(derivedsUpdated()));

		connect(model(), SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int))
				, this, SLOT(rowsAboutToBeInserted(const QModelIndex &, int, int)));
		connect(model(), SIGNAL(rowsInserted(const QModelIndex &, int, int))
				, this, SLOT(rowsInserted(const QModelIndex &, int, int)));

		connect(model(), SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int))
				, this, SLOT(rowsAboutToBeRemoved(const QModelIndex &, int, int)));
		connect(model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int))
				, this, SLOT(rowsRemoved(const QModelIndex &, int, int)));

		connect(model(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &))
				, this, SLOT(dataChanged(const QModelIndex &, const QModelIndex &)));

	}

	LiveNodeModelTester::~LiveNodeModelTester()
	{
		delete modelTest;
	}

	void LiveNodeModelTester::setTimeout(int timeout_msec)
	{
		timeout_ = timeout_msec;
	}

	void LiveNodeModelTester::prepareWaitForModelSignal(const char *signal)
	{
		spy_ = QSharedPointer<QSignalSpy>(new QSignalSpy(model(), signal));
		timer_.setSingleShot(true);
		timer_.start(timeout_);
	}
	bool LiveNodeModelTester::waitForModelSignal()
	{
		bool ret = false;
		if(!spy_)
			qDebug() << "prepareWaitForModelSignal not called";
		else
		while (QCoreApplication::hasPendingEvents()
			|| (spy_->count() < 1 && timer_.isActive()))
		{
			QCoreApplication::sendPostedEvents();
			QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
			QCoreApplication::processEvents();
			ret = timer_.isActive();
		}
		spy_.clear();
		return ret;
	}

	bool LiveNodeModelTester::waitForModelSignal(const char *signal)
	{
		prepareWaitForModelSignal(signal);
		return waitForModelSignal();
	}

	LiveNodeModelTester::Waiter::operator bool()
	{
		if(!has_waited_)
		{
			ret_ = tester_.waitForModelSignal();
			has_waited_ = true;
		}
		return ret_;
	}
	LiveNodeModelTester::Waiter::Waiter(LiveNodeModelTester &tester)
		: tester_(tester), has_waited_(false), ret_(false)
	{}
	LiveNodeModelTester::Waiter::Waiter(Waiter const &cp)
		: tester_(cp.tester_), has_waited_(cp.has_waited_), ret_(cp.ret_)
	{
		cp.has_waited_ = true;
	}
	LiveNodeModelTester::Waiter::~Waiter()
	{
		if(!has_waited_)
			if (!tester_.waitForModelSignal()) qWarning() << "waitForModelSignal() timed out";
	}

	LiveNodeModelTester::Waiter LiveNodeModelTester::modelSignalWaiter(const char *signal)
	{
		prepareWaitForModelSignal(signal);
		return Waiter(*this);
	}

	LiveNodeModelTester::Waiter LiveNodeModelTester::rowsAboutToBeInsertedWaiter()
	{
		return modelSignalWaiter(SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)));
	}
	//! Initialize waiting for rowsInserted and return a \ret Waiter
	LiveNodeModelTester::Waiter LiveNodeModelTester::rowsInsertedWaiter()
	{
		return modelSignalWaiter(SIGNAL(rowsInserted(const QModelIndex &, int, int)));
	}
	//! Initialize waiting for rowsAboutToBeRemoved and return a \ret Waiter
	LiveNodeModelTester::Waiter LiveNodeModelTester::rowsAboutToBeRemovedWaiter()
	{
		return modelSignalWaiter(SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)));
	}
	//! Initialize waiting for rowsRemoved and return a \ret Waiter
	LiveNodeModelTester::Waiter LiveNodeModelTester::rowsRemovedWaiter()
	{
		return modelSignalWaiter(SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
	}
	//! Initialize waiting for dataChanged and return a \ret Waiter
	LiveNodeModelTester::Waiter LiveNodeModelTester::dataChangedWaiter()
	{
		return modelSignalWaiter(SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)));
	}

	//! Initialize waiting for modelUpdated and return a \ret Waiter
	LiveNodeModelTester::Waiter LiveNodeModelTester::modelUpdatedWaiter()
	{
		return modelSignalWaiter(SIGNAL(modelUpdated()));
	}
	//! Initialize waiting for derivedsUpdated and return a \ret Waiter
	LiveNodeModelTester::Waiter LiveNodeModelTester::derivedsUpdatedWaiter()
	{
		return modelSignalWaiter(SIGNAL(derivedsUpdated()));
	}

	int LiveNodeModelTester::signalCount(QString slot_name)
	{
		return signal_counts_[slot_name];
	}

	void LiveNodeModelTester::clearSignals()
	{
		if(debug_verbosity_)
			qDebug() << "Model LNM(" << model() << ":" << name_ << ") clearing signal counts";
		signal_counts_.clear();
	}

	void LiveNodeModelTester::dump()
	{
		qDebug() << "Model LNM(" << model() << ":" << name_ << ") dump:";
		qDebug() << *model();
	}


	void LiveNodeModelTester::modelUpdated()
	{
		signal_counts_["modelUpdated"]++;
		if(debug_verbosity_)
		{
			qDebug() << "Model LNM(" << model() << ":" << name_ << ") updated";
			if(debug_verbosity_ >= 2) dump();
		}
	}
	void LiveNodeModelTester::derivedsUpdated()
	{
		signal_counts_["derivedsUpdated"]++;
		if(debug_verbosity_)
		{
			qDebug() << "Model LNM(" << model() << ":" << name_ << ") deriveds updated";
			if(debug_verbosity_ >= 2) dump();
		}
	}

	void LiveNodeModelTester::rowsAboutToBeInserted(const QModelIndex &index, int start, int end)
	{
		signal_counts_["rowsAboutToBeInserted"]++;
		index_ = index;
		start_ = start;
		end_ = end;
		if(debug_verbosity_)
		{
			qDebug() << "Model LNM(" << model() << ":" << name_ << ") rows about to be inserted: "
					<< start << end;
			if(debug_verbosity_ >= 2) dump();
		}
	}

	void LiveNodeModelTester::rowsInserted(const QModelIndex &index, int start, int end)
	{
		signal_counts_["rowsInserted"]++;
		index_ = index;
		start_ = start;
		end_ = end;
		if(debug_verbosity_)
		{
			qDebug() << "Model LNM(" << model() << ":" << name_ << ") rows inserted: "
					<< start << end;
			if(debug_verbosity_ >= 2) dump();
		}
	}

	void LiveNodeModelTester::rowsAboutToBeRemoved(const QModelIndex &index, int start, int end)
	{
		signal_counts_["rowsAboutToBeRemoved"]++;
		index_ = index;
		start_ = start;
		end_ = end;
		if(debug_verbosity_)
		{
			qDebug() << "Model LNM(" << model() << ":" << name_ << ") rows about to be removed: "
					<< start << end;
			if(debug_verbosity_ >= 2) dump();
		}
	}

	void LiveNodeModelTester::rowsRemoved(const QModelIndex &index, int start, int end)
	{
		signal_counts_["rowsRemoved"]++;
		index_ = index;
		start_ = start;
		end_ = end;
		if(debug_verbosity_)
		{
			qDebug() << "Model LNM(" << model() << ":" << name_ << ") rows removed: "
					<< start << end;
			if(debug_verbosity_ >= 2) dump();
		}
	}

	void LiveNodeModelTester::dataChanged
			(const QModelIndex &topleft_inc, const QModelIndex &botright_inc)
	{
		signal_counts_["dataChanged"]++;
		topleft_ = topleft_inc;
		bottomright_ = botright_inc;
		if(debug_verbosity_)
		{
			qDebug() << "Model LNM(" << model() << ":" << name_ << ") data changed: "
					<< topleft_inc << botright_inc;
			if(debug_verbosity_ >= 2) dump();
		}
	}

	void LiveNodeModelTester::runTests()
	{
		// XXX disabled
		return;
		// we don't want to run it earlier
		modelTest = new ModelTest(model());
	}

}
