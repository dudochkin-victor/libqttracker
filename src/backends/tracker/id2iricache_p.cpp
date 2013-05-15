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
 * id2iridcache_p.cpp
 *
 *  Created on: Sep 6, 2010
 *      Author: iridian
 */

#include "id2iricache_p.h"

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			IDIriCache::IDIriCache()
			{}

			QString IDIriCache::at(int tracker_id) const
			{
				ID2Iri::const_iterator ret = id_to_iri_.constFind(tracker_id);
				if(ret == id_to_iri_.end())
					return QString();
				return ret.value();
			}

			int IDIriCache::reverseAt(QString iri) const
			{
				Iri2ID::const_iterator ret = iri_to_id_.constFind(iri);
				if(ret == iri_to_id_.end())
					return 0;
				return ret->second.key();
			}

			IDIriCache::Iri2ID::const_iterator IDIriCache::end() const
			{
				return iri_to_id_.end();
			}

			IDIriCache::Handle IDIriCache::register_(int tracker_id, QString iri)
			{
				ID2Iri::iterator iid = id_to_iri_.find(tracker_id);
				Iri2ID::iterator iiri = iri_to_id_.find(iri);
				if(iid == id_to_iri_.end() && iiri == iri_to_id_.end())
					return iri_to_id_.insert(iri, QPair<RefCount, ID2Iri::iterator>
							(1, id_to_iri_.insert(tracker_id, iri)));
				else if(iiri->second == iid)
				{
					++iiri->first;
					return iiri;
				}
				return iri_to_id_.end();
			}

			void IDIriCache::unregister(Handle entry_id)
			{
				if(!--entry_id->first)
				{
					id_to_iri_.erase(entry_id->second);
					iri_to_id_.erase(entry_id);
				}
			}

			void IDIriCache::unregister(int tracker_id)
			{
				ID2Iri::const_iterator release_i = id_to_iri_.find(tracker_id);
				if(release_i != id_to_iri_.end())
					unregister(iri_to_id_.find(*release_i));
			}

			void IDIriCache::unregisterReverse(QString iri)
			{
				Handle release_i = iri_to_id_.find(iri);
				if(release_i != iri_to_id_.end())
					unregister(release_i);
			}
		}
	}
}
