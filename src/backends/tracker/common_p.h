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
 * common_p.h
 *
 *  Created on: Apr 20, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef QTTRACKER_BACKENDS_TRACKER_COMMON_P_H_
#define QTTRACKER_BACKENDS_TRACKER_COMMON_P_H_


#include <QString>
#include <QMutex>
#include "dbus/tracker_resources.h"


namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			extern QString service_g;
			extern QString base_path_g;

			extern QString search_interface_g;
			extern QString search_path_g;

			extern QString resources_interface_g;
			extern QString resources_path_g;

			extern QString resources_class_path_g;
			extern QString resources_class_interface_g;

			extern QMutex tracker_global_lock_g;

			extern bool graph_update_signals_g;

			bool initializeTrackerEnvironment();

			QDBusConnection trackerBus(bool is_thread_private = true);

			class TrackerService;
		}
	}
}

#endif /* QTTRACKER_BACKENDS_TRACKER_COMMON_P_H_ */
