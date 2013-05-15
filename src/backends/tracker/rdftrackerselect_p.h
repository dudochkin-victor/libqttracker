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
 * rdftrackerselect_p.h
 *
 *  Created on: May 10, 2010
 *      Author: "Iridian Kiiskinen"
 */

#ifndef SOPRANOLIVE_BACKENDS_TRACKER_RDFTRACKERSELECT_P_H_
#define SOPRANOLIVE_BACKENDS_TRACKER_RDFTRACKERSELECT_P_H_

#include "../../rdfderivedselect_p.h"

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			struct RDFTrackerSelectPrivate
				: RDFDerivedSelect::Private
			{
				static SharedNull<RDFTrackerSelectPrivate> shared_null;

				RDFTrackerSelectPrivate *clone() const
				{ return new RDFTrackerSelectPrivate(*this); }

			protected:
				RDFTrackerSelectPrivate()
					: RDFDerivedSelect::Private() {}
			public:
				RDFTrackerSelectPrivate(RDFSelect::Data const &select)
					: RDFDerivedSelect::Private(select) {}

				typedef int ResourceColumn;
				typedef int TrackerIDColumn;
				QMap<ResourceColumn, TrackerIDColumn> resource_to_tracker_id;

				unsigned columnSubstituent(unsigned column);

				void insertGroupBy(unsigned column, unsigned order_by_priority);
				void insertOrderBy(unsigned column, bool ascending, unsigned group_by_priority);

				struct Builder
					: RDFDerivedSelect::Private::Builder
				{
					Builder(RDFTrackerSelectPrivate const &query);
					RDFTrackerSelectPrivate const &query() const
					{
						return static_cast<RDFTrackerSelectPrivate const &>
							(RDFDerivedSelect::Private::Builder::query());
					}

					QString nodeString(Soprano::Node const &node) const;

					QString recurseFilter(Unboundtor::Filter const &filter, int indentation = 0);

					RDFSelectColumnList buildSelectColumnRange() const;
					bool processSelectColumn(RDFSelectColumn &column) const;
				};
				QSharedPointer<RDFQuery::Data::Builder> createSelectBuilder() const
				{
					return QSharedPointer<RDFQuery::Data::Builder>(new Builder(*this));
				}

				RDFDerivedColumn &joinColumn
						( RDFDerivedColumn &column, bool flat = RDFDerivedSelect::Linked) const;

				static RDFDerivedSelect prototype()
				{ return RDFDerivedSelect(&shared_null); }
				static RDFDerivedSelect prototype(RDFSelect const &select)
				{ return RDFDerivedSelect(new RDFTrackerSelectPrivate(*select.d_func())); }
			};
		}
	}
}

#endif /* SOPRANOLIVE_BACKENDS_TRACKER_RDFTRACKERSELECT_P_H_ */
