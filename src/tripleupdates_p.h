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
 * tripleupdates_p.h
 *
 *  Created on: Dec 14, 2009
 *      Author: "Iridian Kiiskinen"
 */

#ifndef SOPRANOLIVE_TRIPLEUPDATES_P_H_
#define SOPRANOLIVE_TRIPLEUPDATES_P_H_

#include "rawdatatypes_p.h"
#include "compositemodule_p.h"

namespace SopranoLive
{
	class TripleModule;
	typedef QSharedPointer<TripleModule> TripleModulePtr;

	class TripleUpdateReceiver;
	class TripleUpdateNotifier;

	// shared context between a single receiver and possibly multiple notifiers.
	struct TripleUpdateContext
		: QSharedFromThis<TripleUpdateContext>
	{
		/* VZR_CLASS(TripleUpdateContext, (QSharedFromThis<TripleUpdateContext>));*/

		enum Offsets
		{
			offset_Adds = RDFStrategy::offset_ContextSpecificFlags,	offset_Removes, offset_Partial
		};
		enum Flags
		{
			Adds = RDFStrategyFlags(1) << offset_Adds
		,	Removes = RDFStrategyFlags(1) << offset_Removes
		,	AddsRemoves = Adds | Removes
		,	PartialUpdate = RDFStrategyFlags(1) << offset_Partial
		};

		enum Mode
		{
			Inactive // no updates are being reported
		, 	PartialUpdates // some updates covered by triple mask are being reported
		, 	FullUpdates // all updates covered by triple_mask are being reported
		};

		TripleUpdateContext
				( QWeakPointer<TripleUpdateReceiver> const &receiver
				, RDFStatement const &triple_mask
				, QModelIndex const &index = QModelIndex());

		~TripleUpdateContext();

		QWeakPointer<TripleUpdateReceiver> receiver_; // VZR_MEMBER
		Mode mode_; // VZR_MEMBER


		// possibly should be inside a derived class. Each update receiver could
		// want their own context dependent data passed on to them in the update.
		RDFStatement triple_mask_; // VZR_MEMBER


		// TODO: solve the issue of meta-indices representing columns (row == -1),
		// which dont map to cells
		QModelIndex index_; // VZR_MEMBER

		virtual bool tripleUpdates(RawTriplesData const &triples) const;
	};
	typedef QWeakPointer<TripleUpdateContext> TripleUpdateContextWeakPtr;
	typedef QSharedPointer<TripleUpdateContext> TripleUpdateContextPtr;
	typedef QSharedPointer<TripleUpdateContext const> TripleUpdateContextConstPtr;

	class TripleUpdateReceiver
	{
		/* VZR_CLASS(TripleUpdateReceiver);*/

	public:
		// \param triples contains the update information. Its mask denotes
		// whether the modification is \a Adds or \a Removes, or if both are
		// set, the update could be both and has to be separately verified.
		// The mask also denotes which of the \a Subject, \a Predicate and
		// \a Object fields inside the triple data structure have been
		// specified; remaining sets are empty and denote wildcard updates
		// which have to be separately verified.
		// \param handle contains an optional reference to a model location.
		// \return true if the update was handled, false otherwise
		virtual bool tripleUpdates_chain
				( RawTriplesData const &triples, TripleUpdateContextConstPtr const &context
				, InterfaceChain<TripleUpdateReceiver> chain)
		{
			return chain.advance(*this)
				? chain->tripleUpdates_chain(triples, context, chain)
				: false;
		}

		bool tripleUpdates(RawTriplesData const &triples, TripleUpdateContextConstPtr const &context)
		{
			return tripleUpdates_chain(triples, context, InterfaceChain<TripleUpdateReceiver>());
		}

	protected:
		static void registerForTripleUpdates(TripleUpdateContextPtr const &notifier);
	};
	typedef QSharedPointer<TripleUpdateReceiver> TripleUpdateReceiverPtr;

	class TripleUpdateNotifier
	{
		/* VZR_CLASS(TripleUpdateNotifier);*/

	public:
		virtual TripleUpdateContext::Mode registerForTripleUpdates(TripleUpdateContextPtr const &context) = 0;
	};
}
#endif /* SOPRANOLIVE_TRIPLEUPDATES_P_H_ */
