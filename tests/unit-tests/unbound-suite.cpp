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
 * unbound-suite.cpp
 *
 *  Created on: Feb 16, 2010
 *      Author: "Iridian Kiiskinen"
 */

#include "unit-tests.h"

void UnitTests::unbound_PhoneNumberContains()
{
	initGlobalData();
	{
		RDFVariable contacts = isGlobalResource(RDFVariable::fromType<nco::Contact>());
		contacts.property<nco::hasPhoneNumber>().property<nco::phoneNumber>().contains("5556");

		LiveNodes results = ::tracker()->modelVariable(contacts);

		//ModelTest *model_test = new ModelTest(results.model());

		QVERIFY(results->rowCount() == 1);

		RDFVariable contact = contact2_.variable();
		contact.addDerivedObject<nco::hasPhoneNumber>().addDerivedObject<nco::phoneNumber>();

		QCOMPARE(results->liveNode(0).uri(), contact2_.uri());
		QVERIFY(results->liveNode(0) != contact1_);
	}

	{
		RDFVariable contacts = isGlobalResource(RDFVariable::fromType<nco::Contact>());
		contacts.property<nco::hasPhoneNumber>().property<nco::phoneNumber>().hasSuffix("5544");

		LiveNodes results = ::tracker()->modelVariable(contacts);

		//ModelTest *model_test = new ModelTest(results.model());

		QVERIFY(results->rowCount() == 1);

		RDFVariable contact = contact1_.variable();
		contact.addDerivedObject<nco::hasPhoneNumber>().addDerivedObject<nco::phoneNumber>();

		QCOMPARE(results->liveNode(0).uri(), contact1_.uri());
		QVERIFY(results->liveNode(0) != contact2_);
	}
	//delete model_test;

	checkQueryCount("unbound_PhoneNumberContains");
}

void UnitTests::unbound_RDFVariableLink()
{
	RDFVariable var = RDFVariable::fromType<nco::Contact>("var");
	var.addDerivedObject<nie::title>();

	QVERIFY(RDFSelect().addColumn(var).getQuery() == RDFSelect().addColumn(RDFVariableLink(var)).getQuery());
	QVERIFY(var.derivedProperties().size() == RDFVariableLink(var).derivedProperties().size());
	QVERIFY(var.derivedProperties().size() == RDFVariable(RDFVariableLink(var)).derivedProperties().size());

	checkQueryCount("unbound_RDFVariableLink");
}

void UnitTests::unbound_RDFProperty()
{
	RDFVariable obj_info;
	obj_info.addDerivedObject<nie::title>();
	RDFProperty pdata = RDFProperty::fromObjectOf<nie::isLogicalPartOf>(obj_info);

	RDFProperty pcopydata = pdata.linkCopy();
	pcopydata.bind(QUrl("http://foo"));

	QVERIFY(pdata.target().derivedProperties().size() == pcopydata.target().derivedProperties().size());

	checkQueryCount("unbound_RDFProperty");
}

void UnitTests::unbound_IMAccountIsBoundNot()
{
	QSKIP("This test doesnt even assert anything, and it pollutes tracker with data.", SkipAll);

	RDFVariable contact = RDFVariable::fromType<nco::PersonContact>();
	contact.property<nco::contactUID>() = LiteralValue("pseudorandomvalue6987235");

	RDFVariable im = contact.optional().property<nco::hasIMAccount>();
	RDFFilter doesntExist = im.isBound().not_();// do not create if it already exist
	RDFUpdate up;
	QUrl newImAccount = ::tracker()->createLiveNode().uri();

	QList<RDFStatement> insertions;
	insertions  << RDFStatement(contact, nco::hasIMAccount::iri(), newImAccount)
				<< RDFStatement(newImAccount, rdf::type::iri(), nco::IMAccount::iri());
	up.addInsertion(insertions); //this means that filter (not alreadyexisting) applies to both insertions
	::tracker()->executeQuery(up);

	checkQueryCount("unbound_IMAccountIsBoundNot");
}

void UnitTests::unbound_ContainsEscapeCharacter()
{
	StrictLive<nmo::CommunicationChannel> channel = regLocalResource(::tracker()->createLiveResource<nmo::CommunicationChannel>());
	channel->setSubject("example.user@gmail.com");

	RDFSelect query;
	RDFVariable channel_var = RDFVariable::fromType<nmo::CommunicationChannel>();
	isLocalResource(channel_var);

	channel_var.property<nie::subject>().filter("str").contains("example.user@gmail.com");

	query.addColumn("channel", channel_var);

	LiveNodes result = ::tracker()->modelQuery(query);

	QVERIFY(result->rowCount() == 1);
	QVERIFY(result->liveNode(0) == channel);

	checkQueryCount("unbound_ContainsEscapeCharacter");
}

void UnitTests::unbound_queryContactAndMinProperties()
{
	initGlobalData();
	RDFVariable contact = isGlobalResource(RDFVariable::fromType<nco::Contact>());

	LiveNodes results = ::tracker()->modelQuery(RDFSelect()
			.addColumn("contact", contact)
			.addMinColumn("email", contact.optional().property<nmo::hasEmailAddress>().property<nmo::emailAddress>())
			.addMinColumn("address", contact.optional().property<nmo::hasPostalAddress>().property<nmo::streetAddress>())
			.addMinColumn("phone", contact.optional().property<nmo::hasPhoneNumber>().property<nmo::phoneNumber>())
			.groupBy(contact)
			.orderBy(3, RDFSelect::Descending)
		);

	QCOMPARE(results->rowCount(), 11);

	QCOMPARE(results->liveNode(0, 0).uri(), contact2_.uri());
	QCOMPARE(results->liveNode(0, 1).toString(), QString(""));
	QCOMPARE(results->liveNode(0, 2).toString(), str_[street_A]);
	QCOMPARE(results->liveNode(0, 3).toString(), str_[phone_5566]);

	QCOMPARE(results->liveNode(1, 0).uri(), contact1_.uri());
	QCOMPARE(results->liveNode(1, 1).toString(), str_[email_aaa]);
	QCOMPARE(results->liveNode(1, 2).toString(), QString(""));
	QCOMPARE(results->liveNode(1, 3).toString(), str_[phone_5544]);

	checkQueryCount("unbound_queryContactAndMinProperties");
}

void UnitTests::unbound_isZeroOrDoesntExist()
{
	Live<nmm::MusicPiece> music1 = regLocalResource(::tracker()->createLiveResource<nmm::MusicPiece>());
	Live<nmm::MusicPiece> music2 = regLocalResource(::tracker()->createLiveResource<nmm::MusicPiece>());
	Live<nmm::MusicPiece> music3 = regLocalResource(::tracker()->createLiveResource<nmm::MusicPiece>());

	music2->setUsageCounter(0);
	music3->setUsageCounter(1);

	RDFVariable music = isLocalResource(RDFVariable::fromType<nmm::MusicPiece>());

	RDFVariable usage = music.optional().property<nie::usageCounter>();
	usage.doesntExist() || usage == LiteralValue(0);

	RDFSelect unplayed;

	unplayed.addColumn(music)
			.addColumn(usage);

	unplayed.orderBy(usage);

	LiveNodes musics = ::tracker()->modelQuery(unplayed);

	QVERIFY(musics->rowCount() == 2);

	QVERIFY(musics->liveNode(0) == music1);
	QVERIFY(musics->liveNode(1) == music2);

	checkQueryCount("unbound_isZeroOrDoesntExist");
}

void UnitTests::unbound_optionalChildOfLink()
{
	RDFVariable var = RDFVariable::fromType<nco::Contact>();
	RDFVariableLink varlink(var);
	RDFVariable varopt = varlink.optional();
	varopt.property<nco::fullname>(LiteralValue("Jack"));
	// TODO: test something

	checkQueryCount("unbound_optionalChildOfLink");
}

/*
SELECT ?channel COUNT(?message) ?date ?text
WHERE {
	?channel a nmo:CommunicationChannel .
	{
		SELECT ?date ?text
		WHERE {
			?message nmo:communicationChannel ?channel ;
				nmo:receivedDate ?date ;
				nie:plainTextContent ?text .
		} ORDER BY DESC(?date) LIMIT 50
	}
} GROUP BY ?channel ORDER BY ?date
*/
void UnitTests::unbound_messagingSubQuery()
{
	initGlobalData();
	RDFSelect outer;
	RDFVariable channel = isGlobalResource(outer.newColumn<nmo::CommunicationChannel>("channel"));

	RDFSubSelect inner;
	RDFVariable message = isGlobalResource(inner.variable());
	message.object<nmo::communicationChannel>(channel);

	inner.orderBy(message.property<nmo::receivedDate>(), RDFSelect::Descending);
	RDFVariable text = message.object<nie::plainTextContent>(RDFVariable("text"));

	inner.addColumn(channel);
	inner.limit(50);

	outer.addCountColumn("total messages", message);
	outer.addColumn(text);
	outer.groupBy(channel);

	LiveNodes rows = ::tracker()->modelQuery(outer);

	QCOMPARE(rows->rowCount(), 2);

	checkQueryCount("unbound_messagingSubQuery");
}

/*
SELECT ?name SUM(?msg_count) AS ?msgs
WHERE {
	?from a nco:Contact  ;
			  nco:hasEmailAddress ?name . {
		SELECT ?from COUNT(?msg) AS ?msg_count
		WHERE {
			?msg a nmo:Email ;
					 nmo:from ?from .
		} GROUP BY ?from
	}
}
*/
void UnitTests::unbound_messagecountSubQuery()
{
	initGlobalData();
	RDFSelect outer;

	RDFVariable from = RDFVariable::fromType<nco::Contact>();

	RDFVariable name = from.property<nco::hasEmailAddress>();
	outer.addColumn("name", name);
	outer.groupBy(name);

	RDFVariable msg_sum = outer.newSumColumn("total messages");

	{
		RDFSubSelect inner;
		RDFVariable msg = inner.newCountColumnAs(msg_sum);
		msg.isOfType<nmo::Email>();
		msg.property<nmo::from>(from);
		inner.addColumn(from);
		inner.groupBy(from);
	}
	LiveNodes rows = ::tracker()->modelQuery(outer);

	QVERIFY(rows->rowCount() >= 1);

	checkQueryCount("unbound_messagecountSubQuery");
}

/* https://projects.maemo.org/bugzilla/show_bug.cgi?id=151910
+       SELECT ?channel ?participant nco:fullname(?participant)
+           ?last_date nie:plainTextContent(?last_message)
+           (SELECT COUNT(?message) AS ?message_count WHERE {
+                 ?message nmo:communicationChannel ?channel })
+           (SELECT COUNT(?message) AS ?message_count WHERE {
+                 ?message nmo:communicationChannel ?channel ;
+                 nmo:isRead true })
+       WHERE {
+           SELECT ?channel ?participant ?last_date
+               (SELECT ?message WHERE {
+                       ?message nmo:communicationChannel ?channel ;
+                                nmo:sentDate ?date }
+                      ORDER BY DESC(?date) LIMIT 1) AS ?last_message
+           WHERE {
+               ?channel a nmo:CommunicationChannel ;
+                   nmo:lastMessageDate ?last_date ;
+                   nmo:hasParticipant ?participant .
+               FILTER (?participant != nco:default-contact-me)
+           } ORDER BY DESC(?last_date) LIMIT 50
+       }
*/
void UnitTests::unbound_SubQueryCrash()
{
	initGlobalData();
	RDFSelect channel_selection;

	// Where of the outer
	RDFVariable channel = channel_selection.variable("channel");

	isTestResource(channel);

	// add participant info and last message date outer columns
	RDFVariable participant = channel_selection.newColumn("participant");

	RDFVariable participant_name = participant.function<nco::fullname>();
	channel_selection.addColumn("name", participant_name);

	RDFVariable last_date = channel_selection.newColumn("last date");

	RDFVariable last_message = channel_selection.variable("last message");

	channel_selection.addColumn(last_message.function<nie::plainTextContent>());

	{
		// select count of all messages and add it as an expression column to outer
		RDFSubSelect all_subsel;
		RDFVariable message = all_subsel.newCountColumn("total messages");
		message.property<nmo::communicationChannel>(channel);
		channel_selection.addColumn(all_subsel.asExpression());
	}

	{
		// select count of all already read messages and add it as an expression column to outer
		RDFSubSelect read_subsel;
		RDFVariable read_msg = read_subsel.newCountColumn("total unread messages");
		read_msg.property<nmo::communicationChannel>(channel);
		read_msg.property<nmo::isRead>(LiteralValue("true"));
		channel_selection.addColumn(read_subsel.asExpression());
	}

	// Inner select inside outer's where
	RDFSubSelect inner_subsel;

	RDFVariable inner_channel = inner_subsel.newColumnAs<nmo::CommunicationChannel>(channel);
	RDFVariable inner_participant = inner_subsel.newColumnAs(participant);
	RDFVariable inner_last_date = inner_subsel.newColumnAs(last_date);

	inner_channel.property<nmo::hasParticipant>(inner_participant);
	inner_channel.property<nmo::lastMessageDate>(inner_last_date);
	inner_participant != nco::default_contact_me::iri();

	channel_selection.orderBy(last_date, RDFSelect::Descending);
	channel_selection.orderBy(participant_name, RDFSelect::Ascending);
	//inner_subsel.limit(50);

	{
		RDFSubSelect lastmsg_subsel;
		RDFVariable message = lastmsg_subsel.newColumn("message");
		message.property<nmo::communicationChannel>(inner_channel);
		lastmsg_subsel
			.orderBy(message.property<nmo::sentDate>(), RDFSelect::Descending)
			.limit(1);

		inner_subsel.addColumnAs(lastmsg_subsel.asExpression(), last_message);
	}

	LiveNodes rows = ::tracker()->modelQuery(channel_selection);

	QCOMPARE(rows->rowCount(), 10);
	QCOMPARE(rows->columnCount(), 6);
	QCOMPARE(rows->liveNode(6,0).uri(), contact2_.uri());
	QCOMPARE(rows->liveNode(6,1).toString(), contact2_->getFullname());
	QCOMPARE(rows->liveNode(6,2).literal().toDateTime(), dates_[ch2msg3_date]);
	QCOMPARE(rows->liveNode(6,3).toString(), str_[plaintext4]);
	QCOMPARE(rows->liveNode(6,4).literal().toInt(), 3);
	QCOMPARE(rows->liveNode(6,5).literal().toInt(), 2);
	QCOMPARE(rows->liveNode(9,0).uri(), contact1_.uri());
	QCOMPARE(rows->liveNode(9,1).toString(), contact1_->getFullname());
	QCOMPARE(rows->liveNode(9,2).literal().toDateTime(), dates_[ch1msg1_date]);
	QCOMPARE(rows->liveNode(9,3).toString(), str_[plaintext1]);
	QCOMPARE(rows->liveNode(9,4).literal().toInt(), 1);
	QCOMPARE(rows->liveNode(9,5).literal().toInt(), 1);

	checkQueryCount("unbound_SubQueryCrash");
}

// https://projects.maemo.org/bugzilla/show_bug.cgi?id=149230
void UnitTests::unbound_unionChildren()
{
	RDFSelect select;
	Live<nmm::MusicPiece> song1 = regLocalResource(::tracker()->createLiveResource<nmm::MusicPiece>());
		song1->setTitle("Song 1");
		song1->setTrackerAdded(dates_[date1]);
	Live<nmm::MusicPiece> song2 = regLocalResource(::tracker()->createLiveResource<nmm::MusicPiece>());
		song2->setTitle("Song 2");
		song2->removeTrackerAddeds();
		song2->setContentAccessed(dates_[date2]);
	Live<nmm::MusicPiece> song3 = regLocalResource(::tracker()->createLiveResource<nmm::MusicPiece>());
		// no title
		song3->setContentAccessed(dates_[date3]);
		song3->removeTrackerAddeds();
	Live<nmm::MusicPiece> song4 = regLocalResource(::tracker()->createLiveResource<nmm::MusicPiece>());
		song4->setTitle("Song 4");
		song4->removeTrackerAddeds();

	RDFVariable song = select.newColumn<nmm::MusicPiece>("nmm::MusicPiece");

	isLocalResource(song);

	RDFVariable title = song.optional().property<nie::title>();

	RDFVariable activity;
	RDFVariableList alternatives = activity.unionChildren(2);

	alternatives[0].subject<nie::contentAccessed>(song);
	alternatives[1].subject<SopranoLive::Ontologies::tracker::added>(song);

	select.addColumn("nie::title", title);
	select.addColumn("activity", activity);

	select.orderBy(activity);
	select.groupBy(song);
	select.distinct(true);

	LiveNodes query = ::tracker()->modelQuery(select);

	QCOMPARE(query->rowCount(), 3);
	QCOMPARE(query->liveNode(0).uri(), song1.uri());
	QCOMPARE(query->liveNode(0, 2).literal().toDateTime(), dates_[date1]);
	QCOMPARE(query->liveNode(1).uri(), song2.uri());
	QCOMPARE(query->liveNode(1, 2).literal().toDateTime(), dates_[date2]);
	QCOMPARE(query->liveNode(2).uri(), song3.uri());
	QCOMPARE(query->liveNode(2, 2).literal().toDateTime(), dates_[date3]);

	checkQueryCount("unbound_unionChildren");
}

void UnitTests::unbound_nonColumnOrderBy()
{
	Live<nmm::MusicAlbum> album1 = regLocalResource(::tracker()->createLiveResource<nmm::MusicAlbum>());
	album1->setTitle("Aaardvark");
	album1->setContentAccessed(dates_[date1]);

	Live<nmm::MusicAlbum> album2 = regLocalResource(::tracker()->createLiveResource<nmm::MusicAlbum>());
	album2->setTitle("Hoyaa");
	album2->setContentAccessed(dates_[date3]);

	Live<nmm::MusicAlbum> album3 = regLocalResource(::tracker()->createLiveResource<nmm::MusicAlbum>());
	album3->setTitle("Xyzzy");
	album3->setContentAccessed(dates_[date2]);

	RDFSelect select;
	RDFVariable album_var = isLocalResource(select.newColumn<nmm::MusicAlbum>("Album"));
	select.addColumn("Title", album_var.property<nie::title>());
	select.orderBy(album_var.property<nie::contentAccessed>());

	LiveNodes ret = ::tracker()->modelQuery(select);
	QCOMPARE(ret->rowCount(), 3);
	QCOMPARE(ret->liveNode(0).uri(), album1.uri());
	QCOMPARE(ret->liveNode(1).uri(), album3.uri());
	QCOMPARE(ret->liveNode(2).uri(), album2.uri());

	checkQueryCount("unbound_nonColumnOrderBy");
}

void UnitTests::unbound_groupConcat()
{
	initGlobalData();
	RDFSelect sel;
	RDFVariable channel = isGlobalResource(sel.newColumn<nmo::CommunicationChannel>("channel"));
	RDFVariable messages = channel.subject<nmo::communicationChannel>(RDFVariable("message"));
	RDFVariable participant = channel.property<nmo::hasParticipant>(RDFVariable("participant"));
	participant != nco::default_contact_me::iri();
	sel.orderBy(participant.property<nco::fullname>());
	sel.orderBy(messages.property<nmo::sentDate>());
	sel.groupBy(channel);
	sel.distinct();

	sel.addColumn(messages.property<nmo::plainTextContent>().filter("GROUP_CONCAT", LiteralValue(",")));

	LiveNodes res = ::tracker()->modelQuery(sel);

	QCOMPARE(res->rowCount(), 10);
	QCOMPARE(res->liveNode(0, 0).uri(), channel1_.uri());
	QCOMPARE(res->liveNode(0, 1).toString(), str_[plaintext1]);
	QCOMPARE(res->liveNode(1, 0).uri(), channel2_.uri());
	QCOMPARE(res->liveNode(1, 1).toString(), str_[plaintext2] + "," + str_[plaintext3] + "," + str_[plaintext4]);
	for(int ch = 2, chend = ChannelCount; ch != chend; ++ch)
		QCOMPARE(res->liveNode(ch, 0).uri(), channels_[ch].uri());

	checkQueryCount("unbound_groupConcat");
}

void UnitTests::unbound_graphSupport()
{
	initGlobalData();
	Live<nie::InformationElement> graph1 = regLocalResource
			(::tracker()->createLiveResource<nie::InformationElement>());
	Live<nie::InformationElement> graph2 = regLocalResource
			(::tracker()->createLiveResource<nie::InformationElement>());

	RDFUpdate up;
	up.addInsertion(RDFStatementList()
					<< RDFStatement(contact1_.variable(), nco::note::iri(), LiteralValue("note"))
			, graph1);
	::tracker()->executeQuery(up);

	{
		RDFVariable var = contact1_.variable();
		LiveNodes res = ::tracker()->modelVariables(RDFVariableList()
				<< var.object<nco::note>()
				<< var.pattern().graph());
		QCOMPARE(res->rowCount(), 1);
		QCOMPARE(res->liveNode(0, 0).toString(), QString("note"));
		QCOMPARE(res->liveNode(0, 1).uri(), graph1.uri());
	}

	{
		RDFVariable var = contact1_.variable();
		LiveNodes res = ::tracker()->modelVariable
				(var.inGraph(graph1.variable()).object<nco::note>());
		QCOMPARE(res->rowCount(), 1);
		QCOMPARE(res->liveNode(0, 0).toString(), QString("note"));
	}

	{
		RDFVariable var = contact1_.variable();
		LiveNodes res = ::tracker()->modelVariable
				(var.inGraph(graph2.variable()).object<nco::note>());
		QCOMPARE(res->rowCount(), 0);
	}
}

void UnitTests::unbound_orderByOuterVariable()
{
	Live<nmm::MusicPiece> song1 = regLocalResource(::tracker()->createLiveResource<nmm::MusicPiece>());
		song1->setTitle("Song 1");
		song1->setTrackerAdded(dates_[date1]);
	Live<nmm::MusicPiece> song2 = regLocalResource(::tracker()->createLiveResource<nmm::MusicPiece>());
		song2->setTitle("Song 2");
		song2->removeTrackerAddeds();
		song2->setContentAccessed(dates_[date2]);
	Live<nmm::MusicPiece> song3 = regLocalResource(::tracker()->createLiveResource<nmm::MusicPiece>());
		// no title
		song3->setContentAccessed(dates_[date3]);
		song3->removeTrackerAddeds();
	Live<nmm::MusicPiece> song4 = regLocalResource(::tracker()->createLiveResource<nmm::MusicPiece>());
		song4->setTitle("Song 4");
		song4->removeTrackerAddeds();

	RDFSelect select;

	RDFVariable song = isLocalResource(RDFVariable::fromType<nmm::MusicPiece>());
	RDFVariable title = song.optional().property<nie::title>();

	RDFFilter activity
	= song.function<nie::contentAccessed>()
			.function(SopranoLive::Ontologies::tracker::iri("coalesce"),
					song.function<SopranoLive::Ontologies::tracker::added>());

	select.addColumn("uri", song);
	select.addColumn("title", title);
	select.addColumn("activity", activity);

	select.orderBy(activity, RDFSelect::Descending);

	LiveNodes res = ::tracker()->modelQuery(select);

	QCOMPARE(res->rowCount(), 4);
	QCOMPARE(res->liveNode(0, 1).toString(), QString(""));
	QCOMPARE(res->liveNode(1, 1).toString(), QString("Song 2"));
	QCOMPARE(res->liveNode(2, 1).toString(), QString("Song 1"));
	QCOMPARE(res->liveNode(3, 1).toString(), QString("Song 4"));

	checkQueryCount("unbound_orderByOuterVariable");
}

void UnitTests::unbound_insertingEmptyUrl()
{
	RDFServicePtr svc = ::tracker();
	Live<nie::InformationElement> node = regLocalResource
			(svc->createLiveResource<nie::InformationElement>());
	RDFUpdate up;
	up.addInsertion(node.variable(), nie::disclaimer::iri(), LiteralValue("I have it now"));
	up.addInsertion(node.variable(), nie::title::iri(), QUrl());

	svc->executeQuery(up);

	QCOMPARE(svc->modelVariable(node.variable().property<nie::disclaimer>())->rowCount(), 1);
	QCOMPARE(svc->modelVariable(node.variable().property<nie::title>())->rowCount(), 0);

	checkQueryCount("unbound_insertingEmptyUrl");
}

void UnitTests::unbound_definiteConnectedUpdate()
{
	RDFServicePtr svc = ::tracker();
	Live<nco::PersonContact> pc = regLocalResource(svc->createLiveResource<nco::PersonContact>());
	pc->setContactUID("11223344");

	RDFUpdate up;
	up.addInsertion(pc.uri(), nco::nameGiven::iri(), LiteralValue("Jennifer"));

	RDFVariable pc2 = pc.variable();
	pc2.optional().property<nco::contactUID>().doesntExist();
	up.addInsertion(pc2, nco::contactUID::iri(), LiteralValue("44332211"));

	up.addInsertion(pc.uri(), nco::nameFamily::iri(), LiteralValue("Alonso"));

	svc->executeQuery(up);

	QCOMPARE(pc->getNameGiven(), QString("Jennifer"));
	QCOMPARE(pc->getNameFamily(), QString("Alonso"));
	QCOMPARE(pc->getContactUID(), QString("11223344"));

	checkQueryCount("unbound_definiteConnectedUpdate");
}

void UnitTests::unbound_literalColumns()
{
    RDFSelect select;
    select.addColumn("Context", LiteralValue("Home"));

    QCOMPARE(select.columns().count(), 1);
    QCOMPARE(select.columns().first().name(), QString::fromLatin1("Context"));

    LiveNodes nodes = ::tracker()->modelQuery(select);
    LiveNodeModel *model = nodes.model();

    QCOMPARE(model->headerData(0, Qt::Horizontal).toString(), QString::fromLatin1("Context"));

    QCOMPARE(model->rowCount(), 1);
    QCOMPARE(model->data(model->index(0, 0)).toString(), QString::fromLatin1("Home"));
}

void UnitTests::unbound_filterFalse()
{
	RDFSelect sel;
	sel.limit(1);
	sel.pattern().useFilter(LiteralValue(false));
	sel.addColumn(RDFVariable::fromType<rdfs::Resource>());

	QCOMPARE(::tracker()->modelQuery(sel)->rowCount(), 0);
}

void UnitTests::unbound_silentOperations()
{
	RDFServicePtr svc = ::tracker();

	Live<nmo::Email> res_node = regLocalResource(svc->createLiveResource<nmo::Email>());
	QUrl res = res_node.uri();
	res_node = Live<nmo::Email>();

	RDFUpdate up;
	up.addInsertion(RDFStatementList()
					<< RDFStatement(res, nie::title::iri(), LiteralValue("title")));
	up.addOperation(RDFUpdate::Operation::insertion
			(RDFStatementList() << RDFStatement(res, nie::title::iri(), LiteralValue("silent failure")))
			.setSilent());
	up.addInsertion(res, nmo::messageId::iri(), LiteralValue("id"));

	svc->executeQuery(up);
	QCOMPARE(svc->modelVariable(RDFVariable(res).property<nmo::messageId>())
			->liveNode(0, 0).toString(), QString("id"));

	RDFUpdate up2;
	// this should get rolled back
	up2.addOperation(RDFUpdate::Operation::deletion
			(RDFStatementList() << RDFStatement(res, rdfs::type::iri(), rdfs::Resource::iri())));

	// because this insertion should fail after above removal
	up2.addOperation(RDFUpdate::Operation::insertion
			(RDFStatementList() << RDFStatement(res, nie::title::iri(), LiteralValue("title2"))));

	svc->executeQuery(up2);
	QCOMPARE(svc->strictLiveResource<nmo::Email>(res).isValid(), true);
}
