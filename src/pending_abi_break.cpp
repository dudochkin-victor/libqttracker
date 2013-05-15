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
 * pending_abi_break.cpp
 *
 *  Created on: Mar 22, 2010
 *      Author: "Iridian Kiiskinen"
 */

#include "rdfservice_p.h"
#include "rdfunbound_p.h"

namespace SopranoLive
{
	// deprecated: remove
	bool RDFVariable::metaIsUnknown() const
	{
		return d->variable()->isUnconnected();
	}

	// deprecated: remove
	RDFPattern RDFVariable::varPattern() const
	{
		return pattern();
	}

	// deprecated: remove
	void RDFVariable::varSetPattern(RDFPattern const &pattern)
	{
		setPattern(pattern);
	}

	bool RDFStatement::hasConstraints() const { return hasConstrainedBlanks(); }

	// Add LiveResource::hasType strategyflag default argument and remove this
	bool LiveResource::hasType(LiveNode const &type) const
	{
		return hasType(type, RDFStrategyFlags());
	}


	// replace isPending with this
	bool RDFTransaction::isActive() const { return isPending(); }

	// remove
	bool RDFTransaction::isInPlacePending() const { return isOtherActive(); }

	// replace isInPlacePending with this
	bool RDFTransaction::isOtherActive() const { return mode() & OtherActive; }

	// replace isInPlacePendingRollback with this
	bool RDFTransaction::isPendingRollback() const { return isInPlacePendingRollback(); }

	// remove when the deprecated inPlaceTransaction is removed
	RDFTransactionPtr RDFService::inPlaceTransaction(bool exclusive)
	{
		return createTransaction(exclusive ? RDFTransaction::Exclusive : RDFTransaction::Default);
	}

	// remove
	bool InPlaceSubTransaction::isInPlacePending() const
	{ return isOtherActive(); }

	QDebugHelper localDebug(unsigned)
	{ return qDebug() << "SopranoLive::localDebug deprecated. "
			"libqttracker no longer exposes the logging API externally, remove the debug code"; }

	QDebugHelper localWarning(unsigned)
	{ return qDebug() << "SopranoLive::localWarning deprecated. "
			"libqttracker no longer exposes the logging API externally, remove the debug code"; }

	QDebugHelper localCritical(unsigned)
	{ return qDebug() << "SopranoLive::localCritical deprecated. "
			"libqttracker no longer exposes the logging API externally, remove the debug code."; }
}
