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
 * rdftrackerupdate_p.cpp
 *
 *  Created on: Jul 4, 2010
 *      Author: "Iridian Kiiskinen"
 */

#include "rdftrackerupdate_p.h"

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			SharedNull<RDFTrackerUpdatePrivate> RDFTrackerUpdatePrivate::shared_null;

			RDFTrackerUpdatePrivate::Builder::Builder(RDFTrackerUpdatePrivate const &query)
				: RDFUpdate::Data::Builder(query)
			{}
			RDFTrackerUpdatePrivate const &RDFTrackerUpdatePrivate::Builder::query() const
			{
				return static_cast<RDFTrackerUpdatePrivate const &>
					(RDFUpdate::Data::Builder::query);
			}

			//! Override for custom node strings. Converts QDate into QDateTime for tracker.
			QString RDFTrackerUpdatePrivate::Builder::nodeString(Soprano::Node const &node) const
			{
				// duplicate code with RDFTrackerSelectPrivate in trackerselectprivate_p.cpp
				if(node.isLiteral())
				{
					LiteralValue l = node.literal();
					if(l.isDate())
						return RDFUpdate::Data::Builder::nodeString
								(LiteralValue(QDateTime(l.toDate(), QTime(), Qt::UTC)));
				}
				return RDFUpdate::Data::Builder::nodeString(node);
			}

			QString RDFTrackerUpdatePrivate::Builder::recurseFilter
					(Unboundtor::Filter const &filter, int indentation)
			{
				if(filter.filtertype_ == Filter::isMemberOf)
				{
					Unboundtor::FunctionFilter const &ffilter
							= static_cast<Unboundtor::FunctionFilter const &>(filter);
					QStringList multis;
					for(QLinkedList<Unboundtor::HasVariable>::const_iterator vi = ffilter.arguments.begin()
							, viend = ffilter.arguments.end(); vi != viend; ++vi)
						multis.push_back(*variableName(vi->variable()));
					QString member = *variableName
							(static_cast<Unboundtor::MemberFilter const &>(filter).member());
					return "(" + member + " IN (" + multis.join(",") + "))";
				}
				return RDFUpdate::Data::Builder::recurseFilter(filter, indentation);
			}


		}
	}
}
