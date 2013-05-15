/*
 * This file is part of libqttracker project
 *
 * Copyright (C) 2009-2010, Nokia
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

#include "trackerutil.h"
#include <QDateTime>

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{

			QString toIsoUtcOffsetString(const QDateTime& dt)
			{
				QString result;
				int utcOffset = dt.utcOffset();
				if(utcOffset == 0)
				{
					result = "Z";
				}
				else
				{
					if(dt.utcOffset() > 0)
					{
						result = "+";
					}
					else
					{
						utcOffset = -utcOffset;
						result = "-";
					}
					result += QTime().addSecs(utcOffset).toString("hh:mm");
				}
				return result;
			}

			QString toIsoDateWithUtcOffsetString(const QDateTime& dt)
			{
				return(dt.toString(Qt::ISODate) + toIsoUtcOffsetString(dt));
			}

		}
	}
}
