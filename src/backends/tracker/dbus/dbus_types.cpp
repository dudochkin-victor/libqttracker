/*
 * This file is part of LibQtTracker project
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
 * dbus_types.cpp
 *
 *  Created on: Jan 12, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include <QtDebug>
#include <QtDBus/QDBusMetaType>
#include "dbus_types.h"

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			int registerQTrackerDbusTypes()
			{
				qRegisterMetaType<QVector<QStringList> >();
				qDBusRegisterMetaType<QVector<QStringList> >();
				qDBusRegisterMetaType<QMap<QString, QString> >();
				qDBusRegisterMetaType<QVector<QMap<QString, QString> > >();
				qDBusRegisterMetaType<QVector<QVector<QMap<QString, QString> > > >();
				return 0;
			}
		}
	}
}

