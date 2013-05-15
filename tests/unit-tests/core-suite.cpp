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

#include "unit-tests.h"

void UnitTests::simpleSignalSlot()
{
	simple_signal_flag = true;
}

void UnitTests::signalSlot0()
{
	++signal_count[0];
}
void UnitTests::signalSlot1()
{
	++signal_count[1];
}
void UnitTests::signalSlot2()
{
	++signal_count[2];
}
void UnitTests::signalSlot3()
{
	++signal_count[3];
}
void UnitTests::signalSlot4()
{
	++signal_count[4];
}
void UnitTests::strategyAltered(RDFStrategyFlags mask, RDFStrategyFlags flags, QModelIndex const &)
{
	// accumulate the flags. Reset last_mask to reset accumulation.
	last_flags &= last_mask & ~mask;
	last_flags |= flags & mask;
	last_mask |= mask;
	++signal_count[StrategyAlteredSignal];
}

void UnitTests::special_trackerRelease()
{
	// This test can only be run when all references to tracker are released.
	testSuiteProcessEvents();

	{
		RDFService *previous_service_after_setverbosity = SopranoLive::BackEnds::Tracker::trackerRelease().data();
		// make sure tracker backend gets properly destroyed
		QVERIFY(previous_service_after_setverbosity != ::tracker());
	}

	{
		LiveNode referencer = ::tracker()->liveNode(QUrl("http://test-tracker-backend-deletion.com/"));
		RDFService *previous_service_after_referencer = SopranoLive::BackEnds::Tracker::trackerRelease().data();
		referencer->remove();
		referencer = LiveNode();
		QVERIFY(previous_service_after_referencer != ::tracker());

		BackEnds::Tracker::sync(true);

		// The resource won't be deleted if this process exits before tracker has processed the
		// delete query. Thus, do a dummy select.
		/*
		RDFSelect sel;
		sel.addColumn("everything", RDFVariable::fromType<rdfs::Resource>());
		LiveNodes res = ::tracker()->modelQuery(sel);
		res->rowCount();
		*/
	}

	RDFService * previous_service = SopranoLive::BackEnds::Tracker::trackerRelease().data();

	testSuiteProcessEvents();
	// make sure tracker backend gets properly destroyed
	QVERIFY2(previous_service != SopranoLive::BackEnds::Tracker::service()
			, "Memory leak: tracker service could not be released");
}

void UnitTests::raw_ResourceLifetime()
{
	Soprano::Node node = ::tracker()->createUniqueIri();

	QVERIFY(node.toString().size());

	QCOMPARE(::tracker()->rawSparqlQuery("SELECT ?res WHERE { ?res a rdfs:Resource . FILTER(?res = " + node.toN3() + ") }").size(), 0);

	::tracker()->rawSparqlUpdateQuery("INSERT {" + node.toN3() + " a rdfs:Resource }");

	QCOMPARE(::tracker()->rawSparqlQuery("SELECT ?res WHERE { ?res a rdfs:Resource . FILTER(?res = " + node.toN3() + ") }").size(), 1);

	::tracker()->rawSparqlUpdateQuery("DELETE {" + node.toN3() + " a rdfs:Resource }");

	QCOMPARE(::tracker()->rawSparqlQuery("SELECT ?res WHERE { ?res a rdfs:Resource . FILTER(?res = " + node.toN3() + ") }").size(), 0);

	checkQueryCount("raw_ResourceLifetime");
}

void UnitTests::domain_Tags()
{
	// Warning: This test may insert non-test-resource data to tracker (depending on the contents of
	// tracker before the test) and there is nothing we can do about it.  (The tags are inserted by
	// the tag handling functions and we cannot register them as test resources.)

	using namespace Domains::Tags;
	LiveNode resource_a = regLocalResource(::tracker()->createLiveNode());
	LiveNode resource_b = regLocalResource(::tracker()->createLiveNode());

	addTags(resource_a, (QStringList() << str_[recent] << str_[happy]));

	QCOMPARE(getTags(resource_a).size(), 2);

	addTags(RDFVariable::fromContainer(LiveNodeList() << resource_a << resource_b), QStringList(str_[favorite]));

	QCOMPARE(getTags(resource_a).size(), 3);
	QCOMPARE(getTags(resource_b), QStringList(str_[favorite]));

	removeTags(resource_a, QStringList() << str_[favorite] << str_[recent]);

	QCOMPARE(getTags(resource_a), QStringList(str_[happy]));

	// Don't check the query count for this test; depending on whether the tags existed in tracker
	// before running this test, the number of updates will be different. (The tags are inserted by
	// the tag suite, they are not part of the test context.)
}

void UnitTests::tracker_signalsTests_data()
{
	QTest::addColumn<QUrl>("notify_type");
	QList<QUrl> notified_types = ::tracker()->modelVariable(
			RDFVariable(LiteralValue(true)).subject<Ontologies::tracker::notify>());

	if(notified_types.size())
		QTest::newRow(notified_types.first().toString().toAscii().data())
				<< notified_types.first();

	//foreach(QUrl notified_type, notified_types)
	//	QTest::newRow(notified_type.toString().toAscii().data()) << notified_type;
}

void UnitTests::tracker_signalsTests()
{
	using BackEnds::Tracker::ClassUpdateSignaler;

	QFETCH(QUrl, notify_type);

	ClassUpdateSignaler *contact_updates = ClassUpdateSignaler::get(notify_type);

	QObject::connect
			( contact_updates, SIGNAL(subjectsAdded(const QStringList &))
			, this, SLOT(signalSlot0()));
	QObject::connect
			(contact_updates, SIGNAL(subjectsRemoved(const QStringList &))
			, this, SLOT(signalSlot1()));
	QObject::connect
			(contact_updates, SIGNAL(subjectsChanged(const QStringList &))
			, this, SLOT(signalSlot2()));
	QObject::connect
			(contact_updates, SIGNAL(subjectsChanged(const QStringList &, const QStringList &))
			, this, SLOT(signalSlot3()));

	QTime timer;

	Live<nie::InformationElement> element;
	{
		signal_count[0] = false;
		timer.start();

		element = regLocalResource(::tracker()->createLiveResource<nie::InformationElement>());
		element->addRdfType(notify_type);
		QCOMPARE(::tracker()->modelVariable(element.variable())->rowCount(), 1);

		while(timer.elapsed() < 5000 && !signal_count[0])
			QCoreApplication::processEvents();

		bool element_added_signal = signal_count[0];

		QVERIFY(element_added_signal);
	}
	{
		signal_count[2] = false;
		signal_count[3] = false;
		timer.start();

		element->setTitle("title");
		QCOMPARE( ::tracker()->modelVariable(element.variable().property<nie::title>())
						->firstNode().toString()
		        , QString("title"));

		while(timer.elapsed() < 5000 && (!signal_count[2] || !signal_count[3]))
			QCoreApplication::processEvents();

		bool subject_changed_signal = signal_count[2];
		bool subject_changed_w_predicate_signal = signal_count[3];

		QVERIFY(subject_changed_signal);
		QVERIFY(subject_changed_w_predicate_signal);
	}
	{
		signal_count[1] = false;
		timer.start();

		element->remove();
		QCOMPARE(::tracker()->modelVariable(element.variable())->rowCount(), 0);

		while(timer.elapsed() < 5000 && !signal_count[1])
			QCoreApplication::processEvents();

		bool subject_removed_signal = signal_count[1];

		QVERIFY(subject_removed_signal);
	}

	checkQueryCount("tracker_signalsTests");
}

// https://projects.maemo.org/bugzilla/show_bug.cgi?id=137391
void UnitTests::tracker_subjectsAdded()
{
	int oldcount = signal_count[4] = 0;

	using BackEnds::Tracker::ClassUpdateSignaler;

	ClassUpdateSignaler *signaler = ClassUpdateSignaler::get<mfo::FeedChannel>();

	QObject::connect
			( signaler, SIGNAL(subjectsAdded(const QStringList &))
			, this, SLOT(signalSlot4()));

	// Create a live node, verify that the subjectsAdded signal has been sent.
	Live<mfo::FeedChannel> node1 = regLocalResource(::tracker()->createLiveNode());

	QTime timer;
	timer.start();

	while(timer.elapsed() < 5000 && oldcount == signal_count[4])
		QCoreApplication::processEvents();

	QCOMPARE(signal_count[4], ++oldcount);

	// Get a strict live node, verify that the subjectsAdded signal has not been sent.
	LiveNode node2 = ::tracker()->strictLiveNode(node1.uri());

	timer.start();

	// this timeout is hardcoded, and might no catch signals that get emitted really late.
	// should perhaps observe outgoing queries instead.

	while(timer.elapsed() < 1000)
		QCoreApplication::processEvents();

	QCOMPARE(signal_count[4], oldcount);

	checkQueryCount("tracker_subjectsAdded");
}

void UnitTests::tracker_sync()
{
	// no real way to make sure sync is valid on the other end, but at least make sure
	// the dbus method gets called properly and the results are waited on
	RDFServicePtr svc = ::tracker();

	int sync_count = svc->serviceAttribute("sync_count").toInt();

	{
		BackEnds::Tracker::sync(true);
	}

	QCOMPARE(svc->serviceAttribute("sync_count").toInt(), sync_count + 1);

	{
		QUrl url = svc->createUniqueIri();
		QList<LiveNodes> ret = svc->executeQuery(RDFUpdate()
					.addInsertion(url, rdfs::type::iri(), nmo::Email::iri())
					.addInsertion(url, nie::isLogicalPartOf::iri(), local_test_context.variable())
				, BackEnds::Tracker::SyncOnCommit
				);

		QCOMPARE(ret.size(), 1);

		QSignalSpy spy(ret[0].model(), SIGNAL(modelUpdated()));
		QTime timer;
		timer.start();
		while(!spy.size() && timer.elapsed() < 500)
			QCoreApplication::processEvents();
	}

	QCOMPARE(svc->serviceAttribute("sync_count").toInt(), sync_count + 2);

	{
		RDFTransactionPtr tx = svc->createTransaction(BackEnds::Tracker::SyncOnCommit);
		regLocalResource(svc->createLiveResource<nmo::Email>());

		QSignalSpy spy(tx.data(), SIGNAL(commitFinished()));
		tx->commit();

		QTime timer;
		timer.start();
		while(!spy.size() && timer.elapsed() < 500)
			QCoreApplication::processEvents();
	}


	QCOMPARE(svc->serviceAttribute("sync_count").toInt(), sync_count + 3);

	checkQueryCount("tracker_sync");
}

void UnitTests::tracker_dateToDateTime()
{
	Live<nie::InformationElement> elem = regLocalResource
			( ::tracker()->createLiveResource<nie::InformationElement>());

	QDate date = QDate::currentDate();
	RDFUpdate up;
	up.addInsertion(elem.variable(), nie::contentLastModified::iri(), LiteralValue(date));

	::tracker()->executeQuery(up);

	QDateTime datetime = elem->getContentLastModified();
	QCOMPARE(datetime.date(), date);

	RDFVariable var = isLocalResource();
	var.property<nie::contentLastModified>(LiteralValue(date));
	QUrl url = ::tracker()->firstNode(var).uri();

	QCOMPARE(url, elem.uri());

	QString query_text = ::tracker()->rawQueryString(up);
	QVERIFY(query_text.indexOf("T00:00:00Z") != -1);

}
