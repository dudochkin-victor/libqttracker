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
 * livenodemodelbridge_p.cpp
 *
 *  Created on: Jun 30, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */
#include "livenodemodelbridge_p.h"

namespace SopranoLive
{
// LCOV_EXCL_START
	void LiveNodeModelBridgeDetail::connectLiveNodeModelBridge(QObject *backend, QObject *bridge, unsigned connect_signals)
	{
		qWarning() << __func__ << "BEWARE: untested code";
		if(connect_signals & sig_headerDataChanged)
			QObject::connect(backend, SIGNAL(headerDataChanged(Qt::Orientation, int, int)), bridge, SIGNAL(headerDataChanged(Qt::Orientation, int, int)));
		if(connect_signals & sig_layoutChanged)
			QObject::connect(backend, SIGNAL(layoutChanged()), bridge, SIGNAL(layoutChanged()));
		if(connect_signals & sig_layoutAboutToBeChanged)
			QObject::connect(backend, SIGNAL(layoutAboutToBeChanged()), bridge, SIGNAL(layoutAboutToBeChanged()));


		if(connect_signals & sig_dataChanged)
			QObject::connect(backend, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), bridge, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)));

		if(connect_signals & sig_rowsAboutToBeInserted)
			QObject::connect(backend, SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)), bridge, SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)));
		if(connect_signals & sig_rowsInserted)
			QObject::connect(backend, SIGNAL(rowsInserted(const QModelIndex &, int, int)), bridge, SIGNAL(rowsInserted(const QModelIndex &, int, int)));
		if(connect_signals & sig_rowsAboutToBeRemoved)
			QObject::connect(backend, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)), bridge, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)));
		if(connect_signals & sig_rowsRemoved)
			QObject::connect(backend, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), bridge, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));

		if(connect_signals & sig_columnsAboutToBeInserted)
			QObject::connect(backend, SIGNAL(columnsAboutToBeInserted(const QModelIndex &, int, int)), bridge, SIGNAL(columnsAboutToBeInserted(const QModelIndex &, int, int)));
		if(connect_signals & sig_columnsInserted)
			QObject::connect(backend, SIGNAL(columnsInserted(const QModelIndex &, int, int)), bridge, SIGNAL(columnsInserted(const QModelIndex &, int, int)));
		if(connect_signals & sig_columnsAboutToBeRemoved)
			QObject::connect(backend, SIGNAL(columnsAboutToBeRemoved(const QModelIndex &, int, int)), bridge, SIGNAL(columnsAboutToBeRemoved(const QModelIndex &, int, int)));
		if(connect_signals & sig_columnsRemoved)
			QObject::connect(backend, SIGNAL(columnsRemoved(const QModelIndex &, int, int)), bridge, SIGNAL(columnsRemoved(const QModelIndex &, int, int)));

		if(connect_signals & sig_modelAboutToBeReset)
			QObject::connect(backend, SIGNAL(modelAboutToBeReset()), bridge, SIGNAL(modelAboutToBeReset()));
		if(connect_signals & sig_modelReset)
			QObject::connect(backend, SIGNAL(modelReset()), bridge, SIGNAL(modelReset()));


		if(connect_signals & sig_modelUpdated)
			QObject::connect(backend, SIGNAL(modelUpdated()), bridge, SIGNAL(modelUpdated()));
		if(connect_signals & sig_derivedsUpdated)
			QObject::connect(backend, SIGNAL(derivedsUpdated()), bridge, SIGNAL(derivedsUpdated()));
		if(connect_signals & sig_rowsUpdated)
			QObject::connect(backend, SIGNAL(rowsUpdated(int, int, const QModelIndex &)), bridge, SIGNAL(rowsUpdated(int, int, const QModelIndex &)));
	}
// LCOV_EXCL_STOP
}
