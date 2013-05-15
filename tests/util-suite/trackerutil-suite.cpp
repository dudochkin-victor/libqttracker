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

#include "trackerutil-suite.h"
#include "backends/tracker/trackerutil.h"

using namespace SopranoLive::BackEnds::Tracker;

void TrackerUtilSuite::toIsoUtcOffsetString_UTC()
{
	const QString dtStr = "2010-03-15T19:32:52Z";
	QDateTime dt = QDateTime::fromString(dtStr, Qt::ISODate);
	QCOMPARE(dt.timeSpec(), Qt::UTC);
	QCOMPARE(dt.utcOffset(), 0);
	QCOMPARE(dt.toUTC(), dt);

	QCOMPARE(toIsoDateWithUtcOffsetString(dt), dtStr);
}

void TrackerUtilSuite::toIsoUtcOffsetString_UTC_positive_offset()
{
	const QString dtStr = "2010-03-15T19:32:52+02:00";
	QDateTime dt = QDateTime::fromString(dtStr, Qt::ISODate);
	QCOMPARE(dt.timeSpec(), Qt::OffsetFromUTC);
	QCOMPARE(dt.utcOffset(), 2*60*60);

	QCOMPARE(toIsoDateWithUtcOffsetString(dt), dtStr);
}

void TrackerUtilSuite::toIsoUtcOffsetString_UTC_negative_offset()
{
	const QString dtStr = "2010-03-15T19:32:52-02:00";
	QDateTime dt = QDateTime::fromString(dtStr, Qt::ISODate);
	QCOMPARE(dt.timeSpec(), Qt::OffsetFromUTC);
	QCOMPARE(dt.utcOffset(), -2*60*60);

	QCOMPARE(toIsoDateWithUtcOffsetString(dt), dtStr);
}

void TrackerUtilSuite::toIsoUtcOffsetString_UTC_zero_offset()
{
	const QString dtStr = "2010-03-15T19:32:52";

	QDateTime dt = QDateTime::fromString(dtStr + "-00:00", Qt::ISODate);
	QCOMPARE(toIsoDateWithUtcOffsetString(dt), dtStr + "Z");

	dt = QDateTime::fromString(dtStr + "+00:00", Qt::ISODate);
	QCOMPARE(toIsoDateWithUtcOffsetString(dt), dtStr + "Z");
}

QTEST_MAIN(TrackerUtilSuite)
