/*
 * Copyright (C) 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

/* tests from iridian's stressSuite (stresssuite.cpp)*/

#include "unit-tests.h"

void UnitTests::stress_testTrackerHeavyLoad()
{
	QFETCH(int,limit);

	QString playlistUri="urn:testlist";

	// ensure that there is enough songs

	{
		RDFTransactionPtr transact=::tracker()->createTransaction();
		for(int i=0; i<limit; i++)
		{
			Live<nmm::MusicPiece> trackerPlaylist = regLocalResource(::tracker()->liveNode(QUrl("file:song"+QString::number(i))));
		}
		transact->commit();
	}

	// load songs

	RDFSelect select;
	RDFVariable song = RDFVariable::fromType<nmm::MusicPiece>();
	select.addColumn("Song", song);
	select.limit(limit);

	LiveNodes queryModel = BackEnds::Tracker::tracker()->modelQuery(select);
	int receivedCount = queryModel.nodes().size();
	QCOMPARE(receivedCount, limit);

	// save playlist

	Live<nfo::MediaList> trackerPlaylist = regLocalResource(::tracker()->liveNode(QUrl(playlistUri)));

	trackerPlaylist->removeObjects(nfo::mediaListEntry::iri());

	RDFTransactionPtr transact=::tracker()->createTransaction();
	foreach(LiveNode item, queryModel)
	{
		trackerPlaylist->addMediaListEntry(item);
	}
	transact->commit();

	checkQueryCount("stress_testTrackerHeavyLoad");
}

void UnitTests::stress_testTrackerHeavyLoad_data()
{
	QTest::addColumn<int>("limit");

	// this is not a real stress test so we reduced the number of songs to 20
	QTest::newRow("Iter 1") << 20;
	QTest::newRow("Iter 2") << 20;
	QTest::newRow("Iter 3") << 20;
	QTest::newRow("Iter 4") << 20;
}
