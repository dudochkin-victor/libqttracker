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
 * transaction_p.cpp
 *
 *  Created on: Nov 29, 2010
 *      Author: iridian
 */

#include "transaction_p.h"
#include "rdfservice_p.h"

namespace SopranoLive
{
	Transaction::Transaction
			( QSharedPointer<RDFServiceBasic> const &service, RDFTransaction::Mode mode)
		: TransactionBase(mode), service_(service), active_subs_(0)
	{
		debug(4, service_.data()) << "Transaction TX(" << this << ") created";
	}

	Transaction::~Transaction()
	{
		if(isActive())
		{
			warning(service_.data()) << "~RDFTransaction: rolling back";
			if(mode_ & RDFTransaction::CommitOnDestroy)
				commit();
			else
				rollback();
		}
		debug(4, service_.data()) << "Transaction TX(" << this << ") destroyed";
	}

	bool Transaction::commit(bool is_blocking)
	{
		if(!isActive())
		{
			warning(service_.data()) << "RDFTransaction::commit: "
				"transaction is already inactive, cannot commit";
			return false;
		}

		// Nullify the service's transaction so that when we executeQuery, the
		// service doesn't try to append the query to its transaction but
		// really executes it.
		service_->transaction_ = 0;
		inactivate();

		if(update_.isEmpty())
			return doEmitCommitFinished(), true;

		QList<LiveNodes> res = service_->executeQuery(update_, mode_ & BackEndFlagsMask);
		if(res.size())
		{
			pending_commit_ = res[0];
			res.clear();
			connect( pending_commit_.model(), SIGNAL(modelUpdated())
					, this, SLOT(doEmitCommitFinished()));
			connect( pending_commit_.model(), SIGNAL(error
						( QString const &, RDFStrategyFlags
						, RDFStrategyFlags, QModelIndex const &))
					, this, SLOT(doEmitCommitError(QString)));
			if(is_blocking)
				pending_commit_->refreshModel(LiveNodeModel::Block);
		}
		update_ = RDFUpdate();
		return true;
	}

	// TODO: implement signaling
	bool Transaction::rollback(bool is_blocking)
	{
		Q_UNUSED(is_blocking);
		if(!isActive())
		{
			warning(service_.data()) << "RDFTransaction::rollback: "
					"transaction has already been committed/rolled back";
			return false;
		}
		service_->transaction_ = 0;
		inactivate();
		mode_ &= ~RDFTransaction::PendingRollback;

		update_ = RDFUpdate();

		sharedFromThis(), Q_EMIT rollbackFinished();
		return true;
	}

	bool Transaction::reinitiate()
	{
		if(!service_->transaction_)
		{
			service_->transaction_ = this;
			update_ = service_->service_context_data_->update();
			activate();
			return true;
		} else
			warning(service_.data())<< "RDFTransaction::initiate: "
					"a transaction is already pending on target service";
		return false;
	}

	// TODO: implement blocking commits and signaling
	bool Transaction::commitAndReinitiate(bool is_blocking)
	{
		if(service_->transaction_ == this && !commit(is_blocking))
			return false;
		return reinitiate();
	}

	// TODO: implement signaling
	bool Transaction::rollbackAndReinitiate(bool is_blocking)
	{
		if(service_->transaction_ == this && !rollback(is_blocking))
			return false;
		return reinitiate();
	}

	//! check if the given \a child transaction can be made a child of this
	bool Transaction::canAdopt(RDFTransaction::Mode const &child_mode) const
	{
		if((mode_ | child_mode) & RDFTransaction::Exclusive)
			return false;
		return (mode_ & BackEndFlagsMask) == (child_mode & BackEndFlagsMask);
	}

	void Transaction::doEmitCommitFinished()
	{
		sharedFromThis(), Q_EMIT commitFinished();
	}
	void Transaction::doEmitRollbackFinished()
	{
		sharedFromThis(), Q_EMIT rollbackFinished();
	}
	void Transaction::doEmitCommitError(QString error)
	{
		sharedFromThis(), Q_EMIT commitError(error);
	}

	RDFServicePtr Transaction::service() const
	{
		return service_;
	}

	InPlaceSubTransaction::InPlaceSubTransaction
			(QSharedPointer<Transaction> const &parent, RDFTransaction::Mode mode)
		: TransactionBase(mode), parent_()
	{
		setParent(parent);
		debug(4, parent_->service_.data()) << "InPlaceSubTransaction IPSTX(" << this << ") created";
	}

	InPlaceSubTransaction::~InPlaceSubTransaction()
	{
		debug(4, parent_->service_.data()) << "InPlaceSubTransaction IPSTX(" << this << ") destroyed,"
					  << ((mode_ & RDFTransaction::CommitOnDestroy) ? " committing" : " rolling back");
		if(mode_ & RDFTransaction::CommitOnDestroy)
			commit();
		else
			rollback();
	}

	RDFServicePtr InPlaceSubTransaction::service() const
	{ return parent_->service(); }

	RDFTransaction::Mode InPlaceSubTransaction::mode() const
	{
		return mode_ |
			(parent_->active_subs_ - (isActive() ? 1 : 0) ? RDFTransaction::OtherActive : 0);
	}


	void InPlaceSubTransaction::setParent
			(QSharedPointer<Transaction> const &parent)
	{
		if(parent_)
			parent_->disconnect(this);
		parent_ = parent;
		Transaction *p = parent.data();
		connect(p, SIGNAL(commitFinished()), this, SIGNAL(commitFinished()));
		connect(p, SIGNAL(rollbackFinished()), this, SIGNAL(rollbackFinished()));
		connect(p, SIGNAL(commitError(QString)), this, SIGNAL(commitError(QString)));
	}

	bool InPlaceSubTransaction::isInPlacePendingRollback() const
	{ return parent_->isInPlacePendingRollback(); }

	bool InPlaceSubTransaction::commit(bool is_blocking)
	{
		if(!isActive())
			return false;
		inactivate();
		if(--parent_->active_subs_)
			return false;
		if(parent_->isPendingRollback())
		{
			parent_->rollback(is_blocking);
			return false;
		}
		return parent_->commit(is_blocking);
	}
	bool InPlaceSubTransaction::rollback(bool is_blocking)
	{
		if(!isActive())
			return false;
		inactivate();
		if(--parent_->active_subs_)
		{
			parent_->mode_ |= RDFTransaction::PendingRollback;
			return false;
		}
		return parent_->rollback(is_blocking);
	}
	bool InPlaceSubTransaction::reinitiate()
	{
		if(isActive())
			return false;
		if(!parent_->active_subs_)
		{
			Transaction *active = parent_->service_->transaction_;
			if(!active)
				parent_->reinitiate();
			else if(!active->canAdopt(mode_))
				return false;
			else
				setParent(parent_->service_->transaction_->sharedFromThis<Transaction>());
		}
		parent_->active_subs_++;
		activate();
		return true;
	}

	// TODO: think these through. Currently, a simple solution.
	bool InPlaceSubTransaction::commitAndReinitiate(bool is_blocking)
	{ return commit(is_blocking) && reinitiate(); }
	// TODO: think these through. Currently, a simple solution.
	bool InPlaceSubTransaction::rollbackAndReinitiate(bool is_blocking)
	{ return rollback(is_blocking) && reinitiate(); }

}
