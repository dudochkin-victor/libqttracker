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
 * conv-suite.cpp
 *
 *  Created on: Feb 16, 2010
 *      Author: "Iridian Kiiskinen"
 */

#include "unit-tests.h"

void UnitTests::conv_ResourceLifetime()
{
	LiveNode node = ::tracker()->createLiveNode();

	QVERIFY(node.toString().size());

	QVERIFY(!!node);

	QVERIFY(!!::tracker()->strictLiveNode(node));

	LiveNode node2 = ::tracker()->strictLiveNode(node.uri());
	QVERIFY(!!node);

	node->remove();

	QVERIFY(!::tracker()->strictLiveNode(node));

	checkQueryCount("conv_ResourceLifetime");
}

void UnitTests::conv_TransactionResources()
{
	RDFTransactionPtr transaction = ::tracker()->createTransaction();
	RDFTransactionPtr transaction2 = ::tracker()->createTransaction();
	QVERIFY(!!transaction2);

	QVERIFY(!::tracker()->createTransaction(RDFTransaction::Exclusive));

	Live<nfo::Image> image1 = regLocalResource(::tracker()->createLiveNode());
	StrictLive<nfo::Image> image2 = regLocalResource
			(::tracker()->createLiveResource<nfo::Image>());

	QVERIFY(::tracker()->modelVariable(image1.variable())->rowCount() == 0);
	QVERIFY(::tracker()->modelVariable(image2.variable())->rowCount() == 0);

	QVERIFY(!transaction->commitAndReinitiate());
	QVERIFY(!transaction->isActive());
	QVERIFY(transaction->isOtherActive());

	QVERIFY(::tracker()->modelVariable(image1.variable())->rowCount() == 0);
	QVERIFY(::tracker()->modelVariable(image2.variable())->rowCount() == 0);

	QVERIFY(transaction2->commitAndReinitiate());

	QVERIFY(::tracker()->modelVariable(image1.variable())->rowCount() == 1);
	QVERIFY(::tracker()->modelVariable(image2.variable())->rowCount() == 1);

	StrictLive<nfo::Image> image3 = regLocalResource(::tracker()->createLiveResource<nfo::Image>());

	QVERIFY(::tracker()->modelVariable(image3.variable())->rowCount() == 0);

	RDFTransactionPtr transaction3 = ::tracker()->createTransaction();

	QVERIFY(::tracker()->modelVariable(image3.variable())->rowCount() == 0);

	QVERIFY(transaction2->isOtherActive());

	QVERIFY(!transaction2->rollback(false));
	QVERIFY(!transaction2->isActive());
	QVERIFY(transaction2->isOtherActive());
	QVERIFY(transaction2->isPendingRollback());

	QVERIFY(!transaction3->commit(false));
	QVERIFY(!transaction3->isActive());
	QVERIFY(!transaction2->isOtherActive());
	QVERIFY(!transaction2->isPendingRollback());

	QVERIFY(::tracker()->modelVariable(image3.variable())->rowCount() == 0);

	RDFTransactionPtr expl_transaction4 = ::tracker()->createTransaction(RDFTransaction::Exclusive);

	QVERIFY(!transaction3->reinitiate());

	StrictLive<nfo::Image> image4 = regLocalResource(::tracker()->createLiveResource<nfo::Image>());

	QVERIFY(::tracker()->modelVariable(image4.variable())->rowCount() == 0);

	QVERIFY(expl_transaction4->commit(false));

	QVERIFY(::tracker()->modelVariable(image4.variable())->rowCount() == 1);

	QVERIFY(transaction3->reinitiate());
	QVERIFY(!expl_transaction4->reinitiate());

	checkQueryCount("conv_TransactionResources");
}

void UnitTests::conv_ContactManipulationQuerying()
{
	QString uid = QUuid::createUuid().toString();

	Live<nco::PersonContact> contact = regLocalResource(::tracker()->liveNode(QUrl("contact:"+uid)));
	contact->setContactUID(uid);
	QDateTime current(QDateTime::currentDateTime());
	current.setTime(current.time().addMSecs(-current.time().msec()));
	contact->setContentCreated(current);

	QCOMPARE(uid, contact->getContactUID());
	QCOMPARE(current, contact->getContentCreated());

	Live<nco::EmailAddress> liveEmail = contact->firstHasEmailAddress();
	if(0 == liveEmail) {
	   liveEmail = regLocalResource(contact->addHasEmailAddress());
	}
	liveEmail->setEmailAddress("test@text.com");

	Live<nco::Contact> role = regLocalResource(contact->getRepresentative());
	QVERIFY2(role, "contact->getHasAffiliation() is supposed to add node if not existing");

	Live<nco::PhoneNumber> no = regLocalResource(role->addHasPhoneNumber());
	no->setPhoneNumber(str_[num1]);
	Live<nco::PhoneNumber> no1 = regLocalResource(role->addHasPhoneNumber());
	no1->setPhoneNumber(str_[num2]);

	// check emails
	RDFVariable contact_var = RDFVariable::fromType<nco::PersonContact>();
	contact_var.property<nco::contactUID>(LiteralValue(uid));
	RDFSelect query;
	query.addColumn("contact", contact_var);

	query.addColumn("email", contact_var.property<nco::hasEmailAddress>().property<nco::emailAddress>());

	LiveNodes ncoContacts = ::tracker()->modelQuery(query);

	QCOMPARE(ncoContacts.value().size(), 1);
	QCOMPARE(ncoContacts->index(0, 1).data().toString(), QString("test@text.com"));

	// check work phone numbers
	RDFVariable rdfcontact1 = isLocalResource(RDFVariable::fromType<nco::PersonContact>());
	RDFVariable phone = rdfcontact1.property<nco::representative>().property<nco::hasPhoneNumber>();
	rdfcontact1.property<nco::contactUID>(LiteralValue(uid));
	RDFSelect queryidsnumbers;
	queryidsnumbers.addColumn("contactId", rdfcontact1.property<nco::contactUID>());
	queryidsnumbers.addColumn("phoneno", phone.property<nco::phoneNumber>());
	LiveNodes ncoContacts1 = ::tracker()->modelQuery(queryidsnumbers);

	QCOMPARE(ncoContacts1.value().size(), 2);

	QStringList numbers;
	numbers << str_[num1] << str_[num2];
	QVERIFY(numbers.contains(ncoContacts1->index(0, 1).data().toString()));
	QVERIFY(numbers.contains(ncoContacts1->index(1, 1).data().toString()));

	checkQueryCount("conv_ContactManipulationQuerying");
}

void UnitTests::conv_addHasAffil()
{
	Live<nco::PersonContact> ncoContact = regLocalResource(::tracker()->createLiveNode());
	Live<nco::Affiliation> liveAffil = ncoContact->firstHasAffiliation();
	QVERIFY(!liveAffil);
	if (!liveAffil)
	{
		liveAffil = regLocalResource(ncoContact->addHasAffiliation());
	}
	QVERIFY(!!liveAffil);
	QVERIFY(!!ncoContact->firstHasAffiliation());

	checkQueryCount("conv_addHasAffil");
}

void UnitTests::conv_firstHasEmailAddress()
{
	Live<nco::PersonContact> contact = regLocalResource(::tracker()->createLiveNode());
	contact->setContactUID(QString::number(999));
	contact->setContentCreated(QDateTime::currentDateTime());
	Live<nco::EmailAddress> liveEmail = contact->firstHasEmailAddress();
	QVERIFY(liveEmail == 0);
	if(liveEmail == 0)
		liveEmail = regLocalResource(contact->addHasEmailAddress());
	QVERIFY(!!contact->firstHasEmailAddress());
	liveEmail->setEmailAddress("test@text.com");

	checkQueryCount("conv_firstHasEmailAddress");
}

void UnitTests::conv_sequentialAddContainsBookmarkFolder()
{
	//create 2 Nodes of Type BookmarkFolder
	Live<nfo::BookmarkFolder> rootBookmarkFolder = regLocalResource(::tracker()->createLiveNode());
	Live<nfo::BookmarkFolder> childNode1 = regLocalResource(::tracker()->createLiveNode());
	Live<nfo::BookmarkFolder> childNode2 = regLocalResource(::tracker()->createLiveNode());

	//add the previous created nodes as childs to the folder
	rootBookmarkFolder->addContainsBookmarkFolder(childNode1); //success
	rootBookmarkFolder->addContainsBookmarkFolder(childNode2); //fail

	LiveNodes results = rootBookmarkFolder->getContainsBookmarkFolders(isLocalResource());

	QVERIFY(results->rowCount() == 2);

	QVERIFY(results->liveNode(0) == childNode1 || results->liveNode(0) == childNode2);
	QVERIFY(results->liveNode(1) == childNode1 || results->liveNode(1) == childNode2);

	checkQueryCount("conv_sequentialAddContainsBookmarkFolder");
}

void UnitTests::conv_setAffilTitle()
{
	Live<nco::PersonContact> ncoContact = regLocalResource(::tracker()->createLiveNode());
	Live<nco::Affiliation> affil = regLocalResource(ncoContact->addHasAffiliation());
	affil->addTitle("test");
	QCOMPARE(affil->firstTitle(), QString("test"));
	ncoContact->remove();
	affil->remove();

	checkQueryCount("conv_setAffilTitle");
}

//https://projects.maemo.org/bugzilla/show_bug.cgi?id=193607
void UnitTests::conv_getBigInt()
{
	Live<nfo::FileDataObject> fileObject = regLocalResource(::tracker()->createLiveNode());

	const int sampleInt = 789;
	int retrievedInt;
	fileObject->setFileSize(sampleInt);
	retrievedInt=fileObject->getFileSize();
	QCOMPARE(retrievedInt, sampleInt);

	const long sampleLong=2147483647;
	long retrievedLong;
	fileObject->setFileSize(sampleLong);
	retrievedLong=fileObject->getFileSize();
	QCOMPARE(retrievedLong, sampleLong);


	const qint64 sampleInt64=9223372036854775807LL;
	qint64 retrievedInt64;
	fileObject->setFileSize(sampleInt64);
	retrievedInt64=fileObject->getFileSize();
	if(sampleInt64!=retrievedInt64)
		QFAIL("Comparison of int64 failed");

	fileObject->remove();

}


void UnitTests::conv_contentLastModified()
{
	Live<nco::PersonContact> ncoContact = regLocalResource(::tracker()->createLiveNode());
	QDateTime now = QDateTime::currentDateTime();
	now = now.addMSecs(-now.time().msec());
	ncoContact->setContentLastModified(now);

	QDateTime back = ncoContact->getContentLastModified();

	QVERIFY(now == back);

	checkQueryCount("conv_contentLastModified");
}

// https://projects.maemo.org/bugzilla/show_bug.cgi?id=144665
void UnitTests::conv_testFileLastAccessed()
{
	Live<nfo::MediaList> mlist = regLocalResource(::tracker()->createLiveResource<nfo::MediaList>());
	mlist->setTitle("some_title");

	RDFSelect select;
	RDFVariable playlistVariable = isLocalResource(RDFVariable::fromType<nfo::MediaList>());

	RDFVariable playlistName = playlistVariable.property<nie::title>();
	RDFVariable playlistAccessed = playlistVariable.optional().
							 property<nfo::fileLastAccessed>();

	select.addColumn("playlist", playlistName);
	select.addColumn("accessed", playlistAccessed);

	LiveNodes nodes = ::tracker()->modelQuery(select);

	QVERIFY(nodes->rowCount() == 1);
	QVERIFY(nodes->liveNode(0, 0).toString() == "some_title");
	QVERIFY(!nodes->liveNode(0, 1).isValid());

	checkQueryCount("conv_testFileLastAccessed");
}

/*
SELECT ?address
	(SELECT COUNT(?msg) AS ?msgcnt WHERE { ?msg nmo:from ?from })
WHERE {
	?from a nco:Contact ;
		  nco:hasEmailAddress ?address .
}

unsupported atm

void UnitTests::unbound_messagecountSubQuery()
{
	RDFSelect outer;

	RDFVariable address = outer.newColumn<nco::Contact>("address");
	RDFSelect inner = outer.subQuery ();
	RDFVariable msgcnt;
	outer.addCountColumn("total messages", msgcnt);


	from.isOfType<nco::Contact>();
	from.property<nco::hasEmailAddress>(name);
	RDFSelect inner = outer.subQuery();
	RDFVariable in_from = inner.newColumn("from");
	RDFVariable msg;
	msg.property<nmo::from>(in_from);
	msg.isOfType<nmo::Email>();
	outer.addCountColumn("total messages", msg);
	outer.groupBy(from);

	LiveNodes rows = ::tracker()->modelQuery(outer);

	QVERIFY(rows->rowCount() == 2);
}
*/

// https://projects.maemo.org/bugzilla/show_bug.cgi?id=149230
void UnitTests::conv_wildcardPropertyRemove()
{
	QUrl info_url;

	{
		Live<nie::InformationElement> info;
		{
			RDFTransactionPtr tx = ::tracker()->createTransaction();

			info = regLocalResource(::tracker()->createLiveResource<nie::InformationElement>());
			info_url = info.uri();
			info->addNcoContributor(regLocalResource(::tracker()->createLiveResource<nco::Contact>()))
					->setFullname("Jack");
			info->addNcoContributor(regLocalResource(::tracker()->createLiveResource<nco::Contact>()))
					->setFullname("John");
			info->addNcoContributor(regLocalResource(::tracker()->createLiveResource<nco::Contact>()))
					->setFullname("");
			info->addNcoContributor(regLocalResource(::tracker()->createLiveResource<nco::Contact>()))
					->setFullname("");

			tx->commit();
		}
		{
			QCOMPARE(info->getNcoContributors()->rowCount(), 4);

			QCOMPARE(info->getNcoContributors(RDFVariable(LiteralValue("")).subject<nco::fullname>())->rowCount(), 2);

			info->removeNcoContributors(RDFVariable(LiteralValue("")).subject<nco::fullname>());
		}
	}

	QCOMPARE(::tracker()->liveResource<nie::InformationElement>(info_url)->getNcoContributors()->rowCount(), 2);

	checkQueryCount("conv_wildcardPropertyRemove");
}

void UnitTests::conv_staticResourcesSupport()
{
	// add checks for no spurious inserts happening

	Live<nco::PersonContact> someone = regLocalResource(::tracker()->createLiveResource<nco::PersonContact>());

	// Instead of using nco::default_contact_me , we could actually use
	// any generated class here, to get a live node that represents the
	// actual reflective class/property info
	Live<nco::PersonContact> me = ::tracker()->liveResource<nco::default_contact_me>();

	// the ::iri() is a generic way to get a uri out of a generated
	// convenience class, we use it here as it works just fine.
	someone->setGender(nco::gender_female::iri());

	Live<nco::Gender> my_old_gender = me->firstGender();

	// we could do this too, and should be ~as performant (no queries
	// to backend for getting the nco::gender_female)
	me->setGender(::tracker()->liveResource<nco::gender_female>());

	Live<nco::ContactGroup> group = regLocalResource(::tracker()->createLiveResource<nco::ContactGroup>());
	me->addBelongsToGroup(group);
	someone->addBelongsToGroup(group);

	{
		// the unbound counterpart for static resource, me is now explicitly
		// constrained to nco::default_contact_me
		RDFVariable me_var = RDFVariable::fromInstance<nco::default_contact_me>();

		RDFVariable all_my_female_contacts = isLocalResource(RDFVariable::fromType<nco::PersonContact>());

		// first is the property, second is the value of the property
		all_my_female_contacts.property<nco::gender, nco::gender_female>();

		// constrain all_my_female_contacts to female contacts belonging to the same group as me
		RDFVariable my_groups = me_var.property<nco::belongsToGroup>();
		all_my_female_contacts.property<nco::belongsToGroup>(my_groups);
		all_my_female_contacts != me_var;

		LiveNodes associations = ::tracker()->modelVariable(all_my_female_contacts);
		QCOMPARE(associations->rowCount(), 1);
		QCOMPARE(associations->liveResource<nco::PersonContact>(0), someone);
	}

	if(my_old_gender.isLive())
		me->setGender(my_old_gender);
	else
		me->removeGenders();

	QVERIFY(me->firstGender() == my_old_gender);

	checkQueryCount("conv_staticResourcesSupport");
}

// https://projects.maemo.org/bugzilla/show_bug.cgi?id=158723
void UnitTests::conv_getDerivedsWhenModelUpdated()
{
	RDFTransactionPtr tx = ::tracker()->createTransaction();

	// Create test data: 3 contacs; with 0, 1 and 2 im addresses
	Live<nco::PersonContact> contactWithoutIm = regLocalResource(::tracker()->createLiveNode());
	contactWithoutIm->setNameFamily("A");
	Live<nco::PersonContact> contactWithIm = regLocalResource(::tracker()->createLiveNode());
	contactWithIm->setNameFamily("B");

	Live<nco::IMAddress> imAddress1 = regLocalResource(::tracker()->createLiveNode());
	contactWithIm->setHasIMAddresss(LiveNodeList() << imAddress1);

	Live<nco::PersonContact> contactWithTwoIms = regLocalResource(::tracker()->createLiveNode());
	contactWithTwoIms->setNameFamily("C");

	Live<nco::IMAddress> imAddress2a = regLocalResource(::tracker()->createLiveNode());
	Live<nco::IMAddress> imAddress2b = regLocalResource(::tracker()->createLiveNode());
	contactWithTwoIms->setHasIMAddresss(LiveNodeList() << imAddress2a << imAddress2b);

	tx->commit();

	// Build the query
	RDFVariable contactVar = isLocalResource(RDFVariable::fromType<nco::PersonContact>());
	RDFVariable imAddressVar = contactVar.addDerivedObject<nco::hasIMAddress>();
	RDFVariable nameFamilyVar = contactVar.object<nco::nameFamily>();
	RDFSelect select;
	select.addColumn("contact", contactVar);
	select.addColumn("family name", nameFamilyVar);
	// Ensure that we get the contacts in the creation order
	select.orderBy(nameFamilyVar);

	LiveNodes query = ::tracker()->modelQuery(select);

	// Setup the model to be accessed when the modelUpdated signal comes.
	// Here we cannot use QSignalSpy; accessing the data needs to be done
	// while the functions emitting modelUpdated are still in progress.
	QObject::connect(query.model(), SIGNAL(modelUpdated()), this, SLOT(getDerivedsWhenModelUpdated_helper()));

	// Give the query time to execute
	getDerivedsWhenModelUpdated_helper_count = 0;
	QTime timer;
	timer.start();
	while(timer.elapsed() < 5000 && getDerivedsWhenModelUpdated_helper_count == 0)
		QCoreApplication::processEvents();

	QCOMPARE(getDerivedsWhenModelUpdated_helper_count, 1);
	checkQueryCount("conv_getDerivedsWhenModelUpdated");
}

void UnitTests::getDerivedsWhenModelUpdated_helper()
{
	getDerivedsWhenModelUpdated_helper_count = 1;
	LiveNodeModel* model = (LiveNodeModel*)QObject::sender();
	QCOMPARE(model->rowCount(), 3);

	// Check contact without im address
	Live<nco::Contact> contact0 = model->liveResource<nco::Contact>(0, 0);
	LiveNodes imaddresses = contact0->getHasIMAddresss();
	QCOMPARE(imaddresses->rowCount(), 0);

	// Check contact with one im address
	Live<nco::Contact> contact1 = model->liveResource<nco::Contact>(1, 0);
	imaddresses = contact1->getHasIMAddresss();
	QCOMPARE(imaddresses->rowCount(), 1);

	// Check contact with two im addresses
	Live<nco::Contact> contact2 = model->liveResource<nco::Contact>(2, 0);

	imaddresses = contact2->getHasIMAddresss();
	QCOMPARE(imaddresses->rowCount(), 2);
}

void UnitTests::conv_storingQDateTimeWithLocal()
{
	Live<nco::PersonContact> contact = regLocalResource(::tracker()->createLiveNode());

	// Store a QDateTime with the time spec "local time"
	QDateTime stamp = QDateTime::fromString("2010-03-15T21:32:52", Qt::ISODate);

	// Compute the local offset from UTC
	QDateTime sameInUtc = QDateTime(stamp.date(), stamp.time(), Qt::UTC);
	int utcOffset = stamp.secsTo(sameInUtc);

	contact->setContentLastModified(stamp);

	// The time we get back is always in UTC
	QDateTime back = contact->getContentLastModified();

	// Time stamps correspond to the same moment in time
	QCOMPARE(stamp, back);

	// But tracker internally stores the time zone info.
	RDFVariable contactVar = RDFVariable::fromType<nco::PersonContact>();
	RDFVariable lastModifiedVar = contactVar.object<nie::contentLastModified>();
	RDFSelect select;
	select.addColumn("contact", contactVar);
	select.addColumn(lastModifiedVar.filter("fn:timezone-from-dateTime"));

	LiveNodes model = ::tracker()->modelQuery(select);
	QCOMPARE(model->rowCount(), 1);
	QCOMPARE(model->data(model->index(0, 1)), QVariant(utcOffset));

	checkQueryCount("conv_storingQDateTimeWithLocal");
}

void UnitTests::conv_storingQDateTimeWithUTC()
{
	Live<nco::PersonContact> contact = regLocalResource(::tracker()->createLiveNode());

	// Use a time stamp on the other side of the daylight savings boundary than in the previous test.
	QDateTime stamp = QDateTime::fromString("2010-05-15T12:09:10", Qt::ISODate).toUTC();
	contact->setContentLastModified(stamp);
	QDateTime back = contact->getContentLastModified();

	QCOMPARE(stamp, back);

	// But tracker internally stores the time zone info.
	RDFVariable contactVar = RDFVariable::fromType<nco::PersonContact>();
	RDFVariable lastModifiedVar = contactVar.object<nie::contentLastModified>();
	RDFSelect select;
	select.addColumn("contact", contactVar);
	select.addColumn(lastModifiedVar.filter("fn:timezone-from-dateTime"));

	LiveNodes model = ::tracker()->modelQuery(select);
	QCOMPARE(model->rowCount(), 1);
	QCOMPARE(model->data(model->index(0, 1)), QVariant(0));

	checkQueryCount("conv_storingQDateTimeWithUTC");
}

void UnitTests::conv_storingQDateTimeWithUTCOffset()
{
	Live<nco::PersonContact> contact = regLocalResource(::tracker()->createLiveNode());

	QDateTime stamp = QDateTime::fromString("2010-11-30T12:09:10", Qt::ISODate).toUTC();
	int utcOffset = 5*60*60;
	stamp.setUtcOffset(utcOffset);

	contact->setContentLastModified(stamp);
	QDateTime back = contact->getContentLastModified();

	QCOMPARE(stamp, back);

	// But tracker internally stores the time zone info.
	RDFVariable contactVar = RDFVariable::fromType<nco::PersonContact>();
	RDFVariable lastModifiedVar = contactVar.object<nie::contentLastModified>();
	RDFSelect select;
	select.addColumn("contact", contactVar);
	select.addColumn(lastModifiedVar.filter("fn:timezone-from-dateTime"));

	LiveNodes model = ::tracker()->modelQuery(select);
	QCOMPARE(model->rowCount(), 1);
	QCOMPARE(model->data(model->index(0, 1)), QVariant(utcOffset));

	checkQueryCount("conv_storingQDateTimeWithUTCOffset");
}

void UnitTests::conv_picturesAroundNoon()
{
	QSKIP("depends on NB#163715", SkipAll);

	QDateTime t1 = QDateTime::fromString("2010-01-01T00:33:00Z", Qt::ISODate);
	QDateTime t2 = QDateTime::fromString("2010-02-05T15:33:00", Qt::ISODate);
	t2.setUtcOffset(3*3600);
	QDateTime t3 = QDateTime::fromString("2010-03-01T12:23:00", Qt::ISODate);
	t3.setUtcOffset(4*3600);
	QDateTime t4 = QDateTime::fromString("2010-04-01T12:53:00", Qt::ISODate);
	t4.setUtcOffset(-3600);
	QDateTime t5 = QDateTime::fromString("2010-05-01T12:13:00Z", Qt::ISODate);

	Live<nfo::Image> image1 = regLocalResource(::tracker()->createLiveNode());
	image1->setContentCreated(t1);
	Live<nfo::Image> image2 = regLocalResource(::tracker()->createLiveNode());
	image2->setContentCreated(t2);
	Live<nfo::Image> image3 = regLocalResource(::tracker()->createLiveNode());
	image3->setContentCreated(t3);
	Live<nfo::Image> image4 = regLocalResource(::tracker()->createLiveNode());
	image4->setContentCreated(t4);
	Live<nfo::Image> image5 = regLocalResource(::tracker()->createLiveNode());
	image5->setContentCreated(t5);

	RDFVariable imageVar = isLocalResource(RDFVariable::fromType<nfo::Image>());
	RDFVariable shotTimeVar = imageVar.object<nie::contentCreated>();
	RDFVariable shotHourVar = shotTimeVar.filter("fn:hours-from-dateTime");
	shotHourVar >= LiteralValue(12);
	shotHourVar <= LiteralValue(13);
	RDFSelect sel;
	sel.addColumn("image", imageVar);
	sel.addColumn("shothour", shotHourVar);
	sel.orderBy(shotTimeVar);
	LiveNodes pics = ::tracker()->modelQuery(sel);
	QCOMPARE(pics->rowCount(), 3);
	QCOMPARE(pics->data(pics->index(0, 0)).value<QUrl>(), image3.node().uri());
	QCOMPARE(pics->data(pics->index(0, 0)).value<QUrl>(), image4.node().uri());
	QCOMPARE(pics->data(pics->index(0, 0)).value<QUrl>(), image5.node().uri());
}

// https://projects.maemo.org/bugzilla/show_bug.cgi?id=162082
void UnitTests::conv_setObjectWithEmpty()
{
	Live<nco::Contact> contact = regLocalResource(::tracker()->createLiveNode());

	// First set a valid photo
	contact->setPhoto(regLocalResource(::tracker()->createLiveNode()));
	LiveNodes photos = contact->getPhotos();
	QCOMPARE(photos->rowCount(), 1);

	// Then an empty one
	contact->setPhoto(LiveNode(QUrl()));

	// Expected result: setting an empty photo removes the photo
	photos = contact->getPhotos();
	QCOMPARE(photos->rowCount(), 0);

	checkQueryCount("conv_setObjectWithEmpty");
}

void UnitTests::conv_setObjectsWithEmpty()
{
	Live<nco::Contact> contact = regLocalResource(::tracker()->createLiveNode());

	LiveNodeList many;
	many << LiveNode(QUrl());
	many << regLocalResource(::tracker()->createLiveNode());

	contact->setPhotos(LiveNodes(many));

	// Expected result: only the valid photo was set, the empty one was ignored.
	LiveNodes photos = contact->getPhotos();
	QCOMPARE(photos->rowCount(), 1);

	checkQueryCount("conv_setObjectsWithEmpty");
}

// https://projects.maemo.org/bugzilla/show_bug.cgi?id=153757
Live<nco::IMContact> UnitTests::imContactForTpContactId(const QString& tpCId )
{
	Live<nco::IMContact> ncoContact;
	ncoContact = regLocalResource(::tracker()->createLiveNode());
	ncoContact->setContactUID(tpCId);

	LiveNodes liveIMAccounts = ncoContact->getHasIMAccounts();
	if (0 < liveIMAccounts->rowCount())
		return ncoContact;
	// end temp fix part

	RDFVariable contact = RDFVariable::fromType<nco::PersonContact>();
	contact.property<nco::contactUID>() = LiteralValue(tpCId);

	RDFVariable im = contact.optional().property<nco::hasIMAccount>();

	RDFUpdate up;
	QUrl newImAccount = regLocalResource(::tracker()->createLiveNode()).uri();

	QList<RDFVariableStatement> insertions;
	insertions  << RDFVariableStatement(contact, nco::hasIMAccount::iri(), newImAccount)
	<< RDFVariableStatement(newImAccount, rdf::type::iri(), nco::IMAccount::iri());

	up.addInsertion(insertions); //this means that filter (not already existing) applies to both insertions

	::tracker()->executeQuery(up);
	return ncoContact;
}

void UnitTests::conv_IMCapabilityAudioCalls()
{
	// Insert
	Live<nco::IMContact> ncoContact = imContactForTpContactId("0458645");
	ncoContact->setImContactId("someone@gmail.com");
	QList<LiveNode> caps;
	Live<nco::IMCapability> cap = regLocalResource(::tracker()->createLiveNode());
	caps.append(cap);
	ncoContact->setImContactCapabilitys(caps);

	// Read
	RDFVariable imaccount;
	imaccount = imaccount.fromType<nco::IMContact> ();
	RDFSelect queryidsimacccounts;
	queryidsimacccounts.addColumn("contactId", imaccount.property<nco::contactUID> ());
	queryidsimacccounts.addColumn("IMId", imaccount.property<nco::imContactId> ());
	queryidsimacccounts.addColumn("audio_cap", imaccount.property<nco::imContactCapability>(cap));

	LiveNodes account = ::tracker()->modelQuery(queryidsimacccounts);

	QCOMPARE (account.model()->rowCount (), 1);
	QCOMPARE (account.model()->columnCount (), 3);
	QModelIndex ind = account.model()->index (0, 0);
	QCOMPARE (account.model()->data(ind), QVariant ("0458645"));

	QTime timer;
	timer.start();
	while(timer.elapsed() < 1000)
		QCoreApplication::processEvents();
}
