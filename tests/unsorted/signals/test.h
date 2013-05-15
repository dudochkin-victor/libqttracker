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
 * test.h
 *
 *  Created on: Jun 5, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef TEST_H_
#define TEST_H_

#include <QtTest>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QFile>
#include <QTextStream>

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nfo.h>

using namespace SopranoLive;

class TestModelObjects
	: public QObject
{
	Q_OBJECT

	LiveNodes objects;

	void displayModel(LiveNodes const &objects);

public Q_SLOTS:
	void objectsModelUpdated();
	void objectRowsInserted(const QModelIndex &parent, int start, int end);
	void objectRowsRemoved(const QModelIndex &parent, int start, int end);
	void objectDataChanged(const QModelIndex &topleft, const QModelIndex &bottomright);

//private Q_SLOTS:
	void run();
};

#endif /* TEST_H_ */
