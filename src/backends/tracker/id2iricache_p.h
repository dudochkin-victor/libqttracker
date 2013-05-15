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
 * id2iricache_p.h
 *
 *  Created on: Sep 6, 2010
 *      Author: iridian
 */

#ifndef SOPRANOLIVE_ID2IRICACHE_P_H_
#define SOPRANOLIVE_ID2IRICACHE_P_H_

#include "common_p.h"

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			class IDIriCache
			{
			public:
				typedef int TrackerID;
				typedef int RefCount;
				typedef QMap<TrackerID, QString> ID2Iri;
				typedef QMap<QString, QPair<RefCount, ID2Iri::iterator> > Iri2ID;
				typedef Iri2ID::iterator Handle;

			private:
				ID2Iri id_to_iri_;
				Iri2ID iri_to_id_;

			public:
				IDIriCache();

				QString at(int tracker_id) const;
				int reverseAt(QString iri) const;

				Iri2ID::const_iterator end() const;

				Handle register_(int tracker_id, QString iri);

				void unregister(Handle entry_id);
				void unregister(int tracker_id);
				void unregisterReverse(QString iri);
			};
		}
	}
}
#endif /* SOPRANOLIVE_ID2IRICACHE_P_H_ */
