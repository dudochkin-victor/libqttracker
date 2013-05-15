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
 * rdftrackerupdate_p.h
 *
 *  Created on: Jul 4, 2010
 *      Author: "Iridian Kiiskinen"
 */

#ifndef SOPRANOLIVE_RDFTRACKERUPDATE_P_H_
#define SOPRANOLIVE_RDFTRACKERUPDATE_P_H_

#include "../../rdfunbound_p.h"
namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			struct RDFTrackerUpdatePrivate
				: RDFUpdate::Data
			{
				static SharedNull<RDFTrackerUpdatePrivate> shared_null;

				RDFTrackerUpdatePrivate *clone() const
				{ return new RDFTrackerUpdatePrivate(*this); }

			protected:
				RDFTrackerUpdatePrivate()
					: RDFUpdate::Data() {}
			public:
				RDFTrackerUpdatePrivate(RDFUpdate::Data const &select)
					: RDFUpdate::Data(select) {}

				struct Builder
					: RDFUpdate::Data::Builder
				{
					Builder(RDFTrackerUpdatePrivate const &query);
					RDFTrackerUpdatePrivate const &query() const;

					QString recurseFilter(Unboundtor::Filter const &filter, int indentation);
					QString nodeString(Soprano::Node const &node) const;
				};
				QSharedPointer<RDFUpdate::Data::Builder> createUpdateBuilder() const
				{
					return QSharedPointer<RDFUpdate::Data::Builder>(new Builder(*this));
				}

				static RDFUpdate prototype()
				{ return RDFUpdate::Data::create(&shared_null); }
				static RDFUpdate prototype(RDFUpdate const &update)
				{ return RDFUpdate::Data::create(new RDFTrackerUpdatePrivate(*update.d_func())); }
			};
		}
	}
}
#endif /* SOPRANOLIVE_RDFTRACKERUPDATE_P_H_ */
