/*
 * This file is part of libqttracker project
 *
 * Copyright (C) 2010, Nokia
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
#include "unit-tests.h"

#include <QtTracker/domains/tags.h>

using namespace SopranoLive::Domains::Tags;

void UnitTests::domain_tags_hasAnyTags() {
	clearLocalResources();
	QStringList tags;
	tags << "tag1" << "tag2" << "tag3";
	QStringList tags2;
	tags2 << "tag1" << "tag2" << "tag4";
	QStringList emptyTags;

	LiveNode obj = regLocalResource(::tracker()->createLiveNode());
	addTags(obj, tags);
	RDFVariable rdfVar(obj.uri());

	RDFFilter filter = hasAnyTag(rdfVar, tags2);

	LiveNodes nodes = ::tracker()->modelVariable(filter);
	QCOMPARE(nodes->rowCount(), 2);
	nodes = ::tracker()->modelVariable(rdfVar);
	QCOMPARE(nodes->rowCount(), 2);

	// after adding empty tags list no changes should be observed
	addTags(obj, emptyTags);
	nodes = ::tracker()->modelVariable(rdfVar);
	QCOMPARE(nodes->rowCount(), 2);
	nodes = ::tracker()->modelVariable(filter);
	QCOMPARE(nodes->rowCount(), 2);

	// Don't checkQueryCount() as the tag functions insert resources into
	// tracker therefore the results are not consistent.
}

void UnitTests::domain_tags_hasAllTags() {
	clearLocalResources();
	QStringList tags;
	tags << "tag1" << "tag2" << "tag3";
	QStringList tags2;
	tags2 << "tag1" << "tag2" << "tag4";

	LiveNode obj = regLocalResource(::tracker()->createLiveNode());
	addTags(obj, tags);
	RDFVariable rdfVar(obj.uri());

	RDFFilter filter = hasAllTags(rdfVar, tags2);

	LiveNodes nodes = ::tracker()->modelVariable(filter);
	QCOMPARE(nodes->rowCount(), 1);
	nodes = ::tracker()->modelVariable(rdfVar);
	QCOMPARE(nodes->rowCount(), 1);

	// Don't checkQueryCount() as the tag functions insert resources into
	// tracker therefore the results are not consistent.
}

void UnitTests::domain_tags_addAndRemoveTags()
{
	QSKIP("Testcase fails if there is any data in tracker. After fresh reset it passess. Needs investigating", SkipSingle);
	clearLocalResources();
	QStringList tags;
	// Beware our mighty unicode tags!
    tags << "Normal"
		 << "AaaaaaaaAAAAAAAASSSSS SSSssssssvvvvvvvvVV VVVDDDDDDDDDDAAAAAAASSSSD70"
		 << QString::fromUtf8("\xc3\xa4\x41\xc3\x84\xc3\xa5\xe4\xb8\xad\xe6\x96\x87\xce\x95"
							  "\xce\xbb\xce\xbb\xce\xac\xce\xb4\xce\xb1\xe2\x82\xa4\xe2\x82\xac")
		 << QString::fromUtf8("12 \xe2\x82\xa4\xe2\x82\xac")
		 << "*&!'\"({})\\n%s";

	LiveNode obj = regLocalResource(::tracker()->createLiveNode());
	addTags(obj, tags);

    RDFVariable rdfVar(obj.uri());
    RDFVariable tagVar = rdfVar.object<nao::hasTag>();
	LiveNodes nodes = ::tracker()->modelVariable(tagVar);
	QCOMPARE(nodes->rowCount(), 5);

	QSet<QString> theirTags;
	for (int i = 0; i < 5; ++i) {
		StrictLive<nao::Tag> liveTag = nodes->strictLiveResource<nao::Tag>(i);
		QVERIFY(!!liveTag);
		theirTags.insert(liveTag->getPrefLabel());
	}
	QSet<QString> ourTags(tags.toSet());
	QCOMPARE(theirTags, ourTags);

	// We only want the beautiful ones.
	QStringList toRemove;
	toRemove << tags.takeAt(2) << tags.takeAt(3);
	removeTags(obj, toRemove);
	StrictLive<rdfs::Resource> obj2 = ::tracker()->strictLiveResource<rdfs::Resource>(obj.uri());
	QVERIFY(!!obj2);
	nodes = obj2->getNaoHasTags();
	QCOMPARE(nodes->rowCount(), 3);
	theirTags.clear();
	for (int i = 0; i < 3; ++i) {
		StrictLive<nao::Tag> tagobj(nodes->strictLiveResource<nao::Tag>(i));
		theirTags.insert(tagobj->getPrefLabel());
	}
	ourTags = tags.toSet();
	QCOMPARE(theirTags, ourTags);

	// Don't checkQueryCount() as the tag functions insert resources into
	// tracker therefore the results are not consistent.
}
