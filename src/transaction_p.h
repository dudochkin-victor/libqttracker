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
 * transaction_p.h
 *
 *  Created on: Nov 29, 2010
 *      Author: iridian
 */

#ifndef SOPRANOLIVE_TRANSACTION_P_H_
#define SOPRANOLIVE_TRANSACTION_P_H_

#include "rdfservicebase_p.h"

namespace SopranoLive
{

	struct TransactionBase
		: RDFTransaction
	{
		/* VZR_CLASS(TransactionBase, (RDFTransaction));*/

		RDFTransaction::Mode mode_; // VZR_MEMBER

		TransactionBase(RDFTransaction::Mode mode = RDFTransaction::Default) : mode_(mode) {}

		Mode mode() const { return mode_; }

		bool isPending() const { return !(mode_ & RDFTransaction::Inactive); }
		void inactivate() { mode_ |= RDFTransaction::Inactive; }
		void activate() { mode_ &=  ~RDFTransaction::Inactive; }
		bool isExclusive() const { return mode_ & RDFTransaction::Exclusive; }
		bool isInPlacePendingRollback() const
		{ return mode_ & RDFTransaction::PendingRollback; }
	};

	// return direct references to this only if it is exclusive
	struct Transaction
		: TransactionBase
	{
		Q_OBJECT

		/* VZR_CLASS(Transaction, (TransactionBase));*/

	public:

		Transaction(QSharedPointer<RDFServiceBasic> const &svc, RDFTransaction::Mode mode);
		~Transaction();

		QSharedPointer<RDFServiceBasic> service_; // VZR_MEMBER
		RDFUpdate update_; // VZR_MEMBER

		int active_subs_; // VZR_MEMBER
		LiveNodes pending_commit_; // VZR_MEMBER


		RDFServicePtr service() const;

		bool commit(bool is_blocking = false);
		bool rollback(bool is_blocking = false);
		bool reinitiate();
		bool commitAndReinitiate(bool is_blocking = false);
		bool rollbackAndReinitiate(bool is_blocking = false);

		bool canAdopt(RDFTransaction::Mode const &child_mode) const;

	public Q_SLOTS:
		void doEmitCommitFinished();
		void doEmitRollbackFinished();
		void doEmitCommitError(QString err);

	public:
	};

	struct InPlaceSubTransaction
		: TransactionBase
	{
		/* VZR_CLASS(InPlaceSubTransaction, (TransactionBase));*/

		InPlaceSubTransaction
				(QSharedPointer<Transaction> const &parent
				, RDFTransaction::Mode mode = RDFTransaction::Mode());
		~InPlaceSubTransaction();

		QSharedPointer<Transaction> parent_; // VZR_MEMBER

		RDFServicePtr service() const;
		Mode mode() const;

		void setParent(QSharedPointer<Transaction> const &parent);

		bool isInPlacePending() const;
		bool isInPlacePendingRollback() const;

		bool commit(bool is_blocking = false);
		bool rollback(bool is_blocking = false);
		bool reinitiate();
		bool commitAndReinitiate(bool is_blocking = false);
		bool rollbackAndReinitiate(bool is_blocking = false);
	};

}
#endif /* SOPRANOLIVE_TRANSACTION_P_H_ */
