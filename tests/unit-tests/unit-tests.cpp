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


// to update watermarks, #define OUTPUT_QUERY_COUNTS, run the test and overwrite this function by copypaste from the console output
#undef OUTPUT_QUERY_COUNTS

namespace UnitTestsVars
{
	static QString access_type = "QSPARQL_DIRECT";
}

const QString UnitTests::global_test_context_str = "http://sopranolive.org/coresuite/global_test_context";
const QString UnitTests::local_test_context_str = "http://sopranolive.org/coresuite/local_test_context";

QList<Live<nmo::CommunicationChannel> > UnitTests::prepareChannels(int count)
{
	QList<Live<nmo::CommunicationChannel> > ret;
	for(int i = 0; i < count; ++i)
		ret << Live<nmo::CommunicationChannel>();
	return ret;
}

UnitTests::UnitTests()
	: channels_(prepareChannels(ChannelCount)),
	  channel1_(channels_[0]), channel2_(channels_[1]), email_count_(0)

{
	qDebug() << "********* Running UnitTests with " << ::UnitTestsVars::access_type << "Tracker Access *********";
	// Initialize the literals used in the tests
	str_[email_aaa] = "coresuite_email_aaa@example.com";
	str_[email_yyy] = "coresuite_email_yyy@example.com";
	str_[phone_5544] = "+35855-5555544";
	str_[phone_5566] = "+35855-5555566";
	str_[street_A] = "A-street 555";
	str_[street_B] = "B-street 555";
	str_[street_C] = "C-street 555";
	str_[plaintext1] = "Plain text 1";
	str_[plaintext2] = "Plain text 2";
	str_[plaintext3] = "Plain text 3";
	str_[plaintext4] = "Plain text 4";
	str_[plaintext5] = "Plain text 5";
	str_[subject1] = "Subject 1";
	str_[subject2] = "Subject 2";
	str_[subject3] = "Subject 3";
	str_[subject4] = "Subject 4";
	str_[subject5] = "Subject 5";

	// number string data
	str_[num1] = "1234567";
	str_[num2] = "2345678";

	// tag strings
	str_[favorite] = "favorite";
	str_[recent] = "recent";
	str_[happy] = "happy";

	for(int i = 0, iend = DateCount; i != iend; ++i)
		dates_[i] = QDateTime::fromString("2001-10-26T21:35:52", Qt::ISODate).addSecs(60 * i);
}

void UnitTests::init ()
{
	markQueryCount();
}

void UnitTests::cleanup ()
{
	disconnect(this, SLOT(signalSlot0()));
	disconnect(this, SLOT(signalSlot1()));
	disconnect(this, SLOT(signalSlot2()));
	disconnect(this, SLOT(signalSlot3()));
	disconnect(this, SLOT(signalSlot4()));
	for (int i=0; i<6; ++i)
		signal_count[i] = 0;

	clearLocalResources();
	testSuiteProcessEvents();
}

void UnitTests::initTestCase ()
{
	// Clear the test resources possibly left over by the previous test run (e.g., because of a
	// crash)
	cleanupTestContexts();

	initQueryCountWatermarks();
	initTestContexts();

	testSuiteProcessEvents();

	if(::UnitTestsVars::access_type == QString("DBUS"))
		::tracker()->setServiceAttribute("tracker_access_method", QString("DBUS"));
	if(::UnitTestsVars::access_type == QString("QSPARQL_DIRECT"))
		::tracker()->setServiceAttribute("tracker_access_method", QString("QSPARQL_DIRECT"));

}

void UnitTests::cleanupTestCase ()
{
	cleanupGlobalData();
	cleanupTestContexts();

	special_trackerRelease();

	testSuiteProcessEvents();

#ifdef OUTPUT_QUERY_COUNTS
	outputQueryCounts();
#endif
}

void UnitTests::testSuiteProcessEvents()
{
	while(QCoreApplication::hasPendingEvents())
	{
		QCoreApplication::sendPostedEvents();
		QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
		QCoreApplication::processEvents();
	}
}

// asserts that the given \a resource is a global test resource
RDFVariable UnitTests::isGlobalResource(RDFVariable const &resource)
{
	resource.property<nie::isLogicalPartOf>(global_test_context.variable());
	return resource;
}

// asserts that the given \a resource is a local test resource
RDFVariable UnitTests::isLocalResource(RDFVariable const &resource)
{
	resource.property<nie::isLogicalPartOf>(local_test_context.variable());
	return resource;
}

// asserts that the given \a resource is either a global or local test resource
RDFVariable UnitTests::isTestResource(RDFVariable const &resource)
{
	resource.unionMerge(RDFVariableList() << isGlobalResource() << isLocalResource());
	return resource;
}

// removes all local resources
void UnitTests::clearLocalResources()
{
	::tracker()->removeResources(isLocalResource());
}

void UnitTests::markQueryCount()
{
	current_query_mark_ = ::tracker()->serviceAttribute("query_count").toInt();
	current_update_mark_ = ::tracker()->serviceAttribute("update_count").toInt();
}

void UnitTests::checkQueryCount(QString const &testname)
{
	/*! We check query counts only when QTTRACKER_QUERYCOUNTS variable is greater than 0
	  */
	char *logv = getenv("QTTRACKER_QUERYCOUNTS");
	if(logv && strtoul(logv, NULL, 10)>0)
	{
		int current_mark = ::tracker()->serviceAttribute("query_count").toInt();
		int current_count =  current_mark - current_query_mark_;
		current_query_mark_ = current_mark;
		query_counts_[testname + "_selects"] = current_count;
		QMap<TestName, int>::const_iterator cqci = query_count_watermarks_.find(testname + "_selects");
		if(cqci != query_count_watermarks_.end())
			if (current_count != *cqci)
			{
				QMap<TestName, int>::const_iterator qc_min =  query_count_watermarks_.find(testname + "_selects_min");
				QMap<TestName, int>::const_iterator qc_max =  query_count_watermarks_.find(testname + "_selects_max");
				if((qc_min == query_count_watermarks_.end() || qc_max == query_count_watermarks_.end()) ||
				   (current_count < *qc_min) || (current_count > *qc_max))
				{
					QString msg = " query count: " + testname + "_selects"
						", actual: " + QString::number(current_count) + ", expected: " + QString::number(*cqci);
					QFAIL(msg.toLatin1());
				}
			}

		current_mark = ::tracker()->serviceAttribute("update_count").toInt();
		current_count =  current_mark - current_update_mark_;
		current_update_mark_ = current_mark;
		query_counts_[testname + "_updates"] = current_count;
		cqci = query_count_watermarks_.find(testname + "_updates");
		if(cqci != query_count_watermarks_.end())
			if (current_count != *cqci)
			{
				QMap<TestName, int>::const_iterator qc_min =  query_count_watermarks_.find(testname + "_updates_min");
				QMap<TestName, int>::const_iterator qc_max =  query_count_watermarks_.find(testname + "_updates_max");
				if((qc_min == query_count_watermarks_.end() || qc_max == query_count_watermarks_.end()) ||
				   (current_count < *qc_min) || (current_count > *qc_max))
				{
					QString msg = " query count: " + testname + "_updates"
						", actual: " + QString::number(current_count) + ", expected: " + QString::number(*cqci);
					QFAIL(msg.toLatin1());
				}
			}
	}
}

void UnitTests::outputQueryCounts()
{
	QString result;
	result += "\n\tvoid UnitTests::initQueryCountWatermarks() \n\t{\n";
	for(QMap<TestName, int>::const_iterator cqci = query_counts_.begin(), cqciend = query_counts_.end(); cqci != cqciend; ++cqci)
		((((result += "\t\tquery_count_watermarks_[\"") += cqci.key()) += "\"] = ") += QString::number(cqci.value())) += ";\n";
	result += "\t}\n";
	qCritical() << result;
}

void UnitTests::initQueryCountWatermarks()
{
	query_count_watermarks_["base_AffilOptionalUnboundUpdate_selects"] = 5;
	query_count_watermarks_["base_AffilOptionalUnboundUpdate_updates"] = 8;
	query_count_watermarks_["base_LiveNodeModelRemoveRows_selects"] = 1;
	query_count_watermarks_["base_LiveNodeModelRemoveRows_updates"] = 12;
	query_count_watermarks_["base_LiveNodeModel_findColumnsByName_selects"] = 1;
	query_count_watermarks_["base_LiveNodeModel_findColumnsByName_updates"] = 8;
	query_count_watermarks_["base_LiveNodeModel_subModel_selects"] = 1;
	query_count_watermarks_["base_LiveNodeModel_subModel_updates"] = 20;
	query_count_watermarks_["base_LiveNodeModel_wholeIndex_selects"] = 1;
	query_count_watermarks_["base_LiveNodeModel_wholeIndex_updates"] = 8;
	query_count_watermarks_["base_LiveNodesNodesAccess_selects"] = 1;
	query_count_watermarks_["base_LiveNodesNodesAccess_updates"] = 5;
	query_count_watermarks_["base_OrderedModel_selects"] = 1;
	query_count_watermarks_["base_OrderedModel_updates"] = 1;
	query_count_watermarks_["base_ResourceLifetime_selects"] = 3;
	query_count_watermarks_["base_ResourceLifetime_updates"] = 2;
	query_count_watermarks_["base_SimpleAddCachedObject_selects"] = 2;
	query_count_watermarks_["base_SimpleAddCachedObject_updates"] = 4;
	query_count_watermarks_["base_addRowsToModel_selects"] = 1;
	query_count_watermarks_["base_addRowsToModel_updates"] = 3;
	query_count_watermarks_["base_aggregateModelVariables_selects"] = 1;
	query_count_watermarks_["base_aggregateModelVariables_updates"] = 0;
	query_count_watermarks_["base_blankUpdateResults_selects"] = 0;
	query_count_watermarks_["base_blankUpdateResults_selects_min"] = 0;
	query_count_watermarks_["base_blankUpdateResults_selects_max"] = 1;
	query_count_watermarks_["base_blankUpdateResults_updates"] = 7;
	query_count_watermarks_["base_chainedDerivedProperties_selects"] = 2;
	query_count_watermarks_["base_chainedDerivedProperties_updates"] = 3;
	query_count_watermarks_["base_childModels_selects"] = 5;
	query_count_watermarks_["base_childModels_updates"] = 5;
	query_count_watermarks_["base_getIsRead_selects"] = 1;
	query_count_watermarks_["base_getIsRead_updates"] = 4;
	query_count_watermarks_["base_hasTypeMusicAlbum_selects"] = 7;
	query_count_watermarks_["base_hasTypeMusicAlbum_updates"] = 6;
	query_count_watermarks_["base_liveFeeds_selects"] = 66;
	query_count_watermarks_["base_liveFeeds_selects_min"] = 41;
	query_count_watermarks_["base_liveFeeds_selects_max"] = 66;
	query_count_watermarks_["base_livePhotos_selects"] = 10;
	query_count_watermarks_["base_livePhotos_selects_min"] = 7;
	query_count_watermarks_["base_livePhotos_selects_max"] = 14;
	query_count_watermarks_["base_livePhotos_updates"] = 4;
	query_count_watermarks_["base_livePhotos_updates_min"] = 4;
	query_count_watermarks_["base_livePhotos_updates_max"] = 5;
	query_count_watermarks_["base_modelUpdatedSignal_selects"] = 3;
	query_count_watermarks_["base_modelUpdatedSignal_updates"] = 0;
	query_count_watermarks_["base_multiurlModel_selects"] = 0;
	query_count_watermarks_["base_multiurlModel_updates"] = 16;
	query_count_watermarks_["base_multiurlWithDerivedsLifetime_selects"] = 2;
	query_count_watermarks_["base_multiurlWithDerivedsLifetime_updates"] = 23;
	query_count_watermarks_["base_multiurlWithDeriveds_selects"] = 1;
	query_count_watermarks_["base_multiurlWithDeriveds_updates"] = 22;
	query_count_watermarks_["base_rdfGraph_modelResources_selects"] = 1;
	query_count_watermarks_["base_rdfGraph_modelResources_updates"] = 0;
	query_count_watermarks_["base_rdfGraph_modelVariable_selects"] = 1;
	query_count_watermarks_["base_rdfGraph_modelVariable_updates"] = 12;
	query_count_watermarks_["base_rdfGraph_removeResources_selects"] = 1;
	query_count_watermarks_["base_rdfGraph_removeResources_updates"] = 13;
	query_count_watermarks_["base_removeLiveNodeAlsoInAModel_selects"] = 1;
	query_count_watermarks_["base_removeLiveNodeAlsoInAModel_selects_min"] = 1;
	query_count_watermarks_["base_removeLiveNodeAlsoInAModel_selects_max"] = 3;
	query_count_watermarks_["base_removeLiveNodeAlsoInAModel_updates"] = 4;
	query_count_watermarks_["base_selectUrnFilteredPhotos_selects"] = 2;
	query_count_watermarks_["base_selectUrnFilteredPhotos_updates"] = 5;
	query_count_watermarks_["base_setDataOnInsertedRow_selects"] = 1;
	query_count_watermarks_["base_setDataOnInsertedRow_selects_min"] = 1;
	query_count_watermarks_["base_setDataOnInsertedRow_selects_max"] = 3;
	query_count_watermarks_["base_setDataOnInsertedRow_updates"] = 8;
	query_count_watermarks_["base_setDataTest_selects"] = 1;
	query_count_watermarks_["base_setDataTest_updates"] = 2;
	query_count_watermarks_["base_setMultipleTimes_selects"] = 1;
	query_count_watermarks_["base_setMultipleTimes_updates"] = 5;
	query_count_watermarks_["base_signalEmission_selects"] = 2;
	query_count_watermarks_["base_signalEmission_updates"] = 3;
	query_count_watermarks_["base_storingQDateTimeWithLocal_selects"] = 1;
	query_count_watermarks_["base_storingQDateTimeWithLocal_updates"] = 5;
	query_count_watermarks_["base_storingQDateTimeWithUTCOffset_selects"] = 1;
	query_count_watermarks_["base_storingQDateTimeWithUTCOffset_selects_min"] = 1;
	query_count_watermarks_["base_storingQDateTimeWithUTCOffset_selects_max"] = 2;
	query_count_watermarks_["base_storingQDateTimeWithUTCOffset_updates"] = 5;
	query_count_watermarks_["base_storingQDateTimeWithUTC_selects"] = 1;
	query_count_watermarks_["base_storingQDateTimeWithUTC_selects_min"] = 1;
	query_count_watermarks_["base_storingQDateTimeWithUTC_selects_max"] = 2;
	query_count_watermarks_["base_storingQDateTimeWithUTC_updates"] = 5;
	query_count_watermarks_["base_streamingResultsOrdered_selects"] = 5;
	query_count_watermarks_["base_streamingResultsOrdered_updates"] = 0;
	query_count_watermarks_["base_streamingResults_selects"] = 19;
	query_count_watermarks_["base_streamingResults_selects_min"] = 16;
	query_count_watermarks_["base_streamingResults_selects_max"] = 19;
	query_count_watermarks_["base_streamingResults_updates"] = 0;
	query_count_watermarks_["base_strictMultiurlModelWithWrongTypes_selects"] = 1;
	query_count_watermarks_["base_strictMultiurlModelWithWrongTypes_selects_min"] = 1;
	query_count_watermarks_["base_strictMultiurlModelWithWrongTypes_selects_max"] = 3;
	query_count_watermarks_["base_strictMultiurlModelWithWrongTypes_updates"] = 16;
	query_count_watermarks_["base_strictMultiurlModelWithWrongTypes_updates_min"] = 16;
	query_count_watermarks_["base_strictMultiurlModelWithWrongTypes_updates_max"] = 33;
	query_count_watermarks_["base_strictMultiurlModel_selects"] = 0;
	query_count_watermarks_["base_strictMultiurlModel_updates"] = 16;
	query_count_watermarks_["base_strictMultiurlModel_updates_min"] = 0;
	query_count_watermarks_["base_strictMultiurlModel_updates_max"] = 16;
	query_count_watermarks_["base_testDummyModel_selects"] = 0;
	query_count_watermarks_["base_testDummyModel_updates"] = 0;
	query_count_watermarks_["conv_ContactManipulationQuerying_selects"] = 7;
	query_count_watermarks_["conv_ContactManipulationQuerying_updates"] = 20;
	query_count_watermarks_["conv_ResourceLifetime_selects"] = 1;
	query_count_watermarks_["conv_ResourceLifetime_updates"] = 2;
	query_count_watermarks_["conv_TransactionResources_selects"] = 11;
	query_count_watermarks_["conv_TransactionResources_updates"] = 2;
	query_count_watermarks_["conv_addHasAffil_selects"] = 2;
	query_count_watermarks_["conv_addHasAffil_updates"] = 7;
	query_count_watermarks_["conv_contentLastModified_selects"] = 1;
	query_count_watermarks_["conv_contentLastModified_updates"] = 5;
	query_count_watermarks_["conv_firstHasEmailAddress_selects"] = 2;
	query_count_watermarks_["conv_firstHasEmailAddress_updates"] = 10;
	query_count_watermarks_["conv_getDerivedsWhenModelUpdated_selects"] = 3;
	query_count_watermarks_["conv_getDerivedsWhenModelUpdated_updates"] = 1;
	query_count_watermarks_["conv_sequentialAddContainsBookmarkFolder_selects"] = 1;
	query_count_watermarks_["conv_sequentialAddContainsBookmarkFolder_updates"] = 14;
	query_count_watermarks_["conv_setAffilTitle_selects"] = 1;
	query_count_watermarks_["conv_setAffilTitle_updates"] = 10;
	query_count_watermarks_["conv_setObjectWithEmpty_selects"] = 2;
	query_count_watermarks_["conv_setObjectWithEmpty_selects_min"] = 2;
	query_count_watermarks_["conv_setObjectWithEmpty_selects_max"] = 3;
	query_count_watermarks_["conv_setObjectWithEmpty_updates"] = 10;
	query_count_watermarks_["conv_setObjectsWithEmpty_selects"] = 1;
	query_count_watermarks_["conv_setObjectsWithEmpty_selects_min"] = 1;
	query_count_watermarks_["conv_setObjectsWithEmpty_selects_max"] = 2;
	query_count_watermarks_["conv_setObjectsWithEmpty_updates"] = 8;
	query_count_watermarks_["conv_staticResourcesSupport_selects"] = 3;
	query_count_watermarks_["conv_staticResourcesSupport_selects_min"] = 3;
	query_count_watermarks_["conv_staticResourcesSupport_selects_max"] = 4;
	query_count_watermarks_["conv_staticResourcesSupport_updates"] = 11;
	query_count_watermarks_["conv_storingQDateTimeWithLocal_selects"] = 2;
	query_count_watermarks_["conv_storingQDateTimeWithLocal_selects_min"] = 2;
	query_count_watermarks_["conv_storingQDateTimeWithLocal_selects_max"] = 3;
	query_count_watermarks_["conv_storingQDateTimeWithLocal_updates"] = 5;
	query_count_watermarks_["conv_storingQDateTimeWithUTCOffset_selects"] = 2;
	query_count_watermarks_["conv_storingQDateTimeWithUTCOffset_selects_min"] = 2;
	query_count_watermarks_["conv_storingQDateTimeWithUTCOffset_selects_max"] = 3;
	query_count_watermarks_["conv_storingQDateTimeWithUTCOffset_updates"] = 5;
	query_count_watermarks_["conv_storingQDateTimeWithUTC_selects"] = 2;
	query_count_watermarks_["conv_storingQDateTimeWithUTC_selects_min"] = 2;
	query_count_watermarks_["conv_storingQDateTimeWithUTC_selects_max"] = 3;
	query_count_watermarks_["conv_storingQDateTimeWithUTC_updates"] = 5;
	query_count_watermarks_["conv_testFileLastAccessed_selects"] = 1;
	query_count_watermarks_["conv_testFileLastAccessed_updates"] = 4;
	query_count_watermarks_["conv_wildcardPropertyRemove_selects"] = 3;
	query_count_watermarks_["conv_wildcardPropertyRemove_updates"] = 3;
	query_count_watermarks_["raw_ResourceLifetime_selects"] = 0;
	query_count_watermarks_["raw_ResourceLifetime_updates"] = 0;
	query_count_watermarks_["raw_blankUpdateResults_selects"] = 0;
	query_count_watermarks_["raw_blankUpdateResults_updates"] = 0;
	query_count_watermarks_["stress_testTrackerHeavyLoad_selects"] = 1;
	query_count_watermarks_["stress_testTrackerHeavyLoad_updates"] = 7;
	query_count_watermarks_["tracker_signalsTests_selects"] = 7;
	query_count_watermarks_["tracker_signalsTests_selects_min"] = 3;
	query_count_watermarks_["tracker_signalsTests_selects_max"] = 7;
	query_count_watermarks_["tracker_subjectsAdded_selects"] = 1;
	query_count_watermarks_["tracker_subjectsAdded_selects_min"] = 0;
	query_count_watermarks_["tracker_subjectsAdded_selects_max"] = 1;
	query_count_watermarks_["tracker_subjectsAdded_updates"] = 4;
	query_count_watermarks_["tracker_sync_selects"] = 0;
	query_count_watermarks_["tracker_sync_updates"] = 2;
	query_count_watermarks_["unbound_ContainsEscapeCharacter_selects"] = 1;
	query_count_watermarks_["unbound_ContainsEscapeCharacter_updates"] = 4;
	query_count_watermarks_["unbound_PhoneNumberContains_selects"] = 2;
	query_count_watermarks_["unbound_PhoneNumberContains_updates"] = 0;
	query_count_watermarks_["unbound_RDFProperty_selects"] = 0;
	query_count_watermarks_["unbound_RDFProperty_updates"] = 0;
	query_count_watermarks_["unbound_RDFVariableLink_selects"] = 0;
	query_count_watermarks_["unbound_RDFVariableLink_updates"] = 0;
	query_count_watermarks_["unbound_SubQueryCrash_selects"] = 3;
	query_count_watermarks_["unbound_SubQueryCrash_updates"] = 0;
	query_count_watermarks_["unbound_definiteConnectedUpdate_selects"] = 3;
	query_count_watermarks_["unbound_definiteConnectedUpdate_updates"] = 5;
	query_count_watermarks_["unbound_groupConcat_selects"] = 1;
	query_count_watermarks_["unbound_groupConcat_updates"] = 0;
	query_count_watermarks_["unbound_insertingEmptyUrl_selects"] = 2;
	query_count_watermarks_["unbound_insertingEmptyUrl_updates"] = 4;
	query_count_watermarks_["unbound_isZeroOrDoesntExist_selects"] = 1;
	query_count_watermarks_["unbound_isZeroOrDoesntExist_updates"] = 11;
	query_count_watermarks_["unbound_messagecountSubQuery_selects"] = 1;
	query_count_watermarks_["unbound_messagecountSubQuery_updates"] = 0;
	query_count_watermarks_["unbound_messagingSubQuery_selects"] = 1;
	query_count_watermarks_["unbound_messagingSubQuery_updates"] = 0;
	query_count_watermarks_["unbound_nonColumnOrderBy_selects"] = 1;
	query_count_watermarks_["unbound_nonColumnOrderBy_selects_min"] = 1;
	query_count_watermarks_["unbound_nonColumnOrderBy_selects_max"] = 2;
	query_count_watermarks_["unbound_nonColumnOrderBy_updates"] = 15;
	query_count_watermarks_["unbound_nonColumnOrderBy_updates_min"] = 15;
	query_count_watermarks_["unbound_nonColumnOrderBy_updates_max"] = 16;
	query_count_watermarks_["unbound_optionalChildOfLink_selects"] = 0;
	query_count_watermarks_["unbound_optionalChildOfLink_updates"] = 0;
	query_count_watermarks_["unbound_orderByOuterVariable_selects"] = 1;
	query_count_watermarks_["unbound_orderByOuterVariable_selects_min"] = 1;
	query_count_watermarks_["unbound_orderByOuterVariable_selects_max"] = 2;
	query_count_watermarks_["unbound_orderByOuterVariable_updates"] = 21;
	query_count_watermarks_["unbound_queryContactAndMinProperties_selects"] = 1;
	query_count_watermarks_["unbound_queryContactAndMinProperties_updates"] = 0;
	query_count_watermarks_["unbound_unionChildren_selects"] = 1;
	query_count_watermarks_["unbound_unionChildren_selects_min"] = 1;
	query_count_watermarks_["unbound_unionChildren_selects_max"] = 2;
	query_count_watermarks_["unbound_unionChildren_updates"] = 21;
}

void UnitTests::prepareGlobalData()
{
	// contact data

	contact1_ = regGlobalResource(::tracker()->createLiveResource<nco::PersonContact>());
	LiveNode ret = contact1_->addObject(nmo::hasEmailAddress::iri(), AutoGen::Property<nmo::hasEmailAddress>::metaType());
	regGlobalResource(ret);
	regGlobalResource(contact1_->addHasEmailAddress())->setEmailAddress(str_[email_aaa]);
	regGlobalResource(contact1_->addHasEmailAddress())->setEmailAddress(str_[email_yyy]);
	regGlobalResource(contact1_->addHasPhoneNumber())->setPhoneNumber(str_[phone_5544]);
	contact1_->setFullname("Jack Daniels");
	contact1_->setContactUID("Foobar");

	contact2_ = regGlobalResource(::tracker()->createLiveResource<nco::PersonContact>());
	regGlobalResource(contact2_->addHasPostalAddress())->setStreetAddress(str_[street_C]);
	regGlobalResource(contact2_->addHasPostalAddress())->setStreetAddress(str_[street_B]);
	regGlobalResource(contact2_->addHasPostalAddress())->setStreetAddress(str_[street_A]);
	regGlobalResource(contact2_->addHasPhoneNumber())->setPhoneNumber(str_[phone_5566]);
	contact2_->setFullname("Jill Trace");
	contact2_->setContactUID("Foobar");

	{
		channel1_ = regGlobalResource(::tracker()->createLiveResource<nmo::CommunicationChannel>());
		channel1_->addHasParticipant(contact1_);
		channel1_->addHasParticipant(nco::default_contact_me::iri());

		channel2_ = regGlobalResource(::tracker()->createLiveResource<nmo::CommunicationChannel>());
		channel2_->addHasParticipant(contact2_);
		channel2_->addHasParticipant(nco::default_contact_me::iri());

		Live<nmo::Message> msg;

		channel_messages_.push_back(regGlobalResource(msg = ::tracker()->createLiveResource<nmo::Message>()));
		msg->setCommunicationChannel(channel1_);
		msg->setSentDate(dates_[ch1msg1_date]);
		msg->setPlainTextContent(str_[plaintext1]);
		msg->setIsRead(true);
		channel1_->setLastMessageDate(dates_[ch1msg1_date]);

		channel_messages_.push_back(regGlobalResource(msg = ::tracker()->createLiveResource<nmo::Message>()));
		msg->setCommunicationChannel(channel2_);
		msg->setSentDate(dates_[ch2msg1_date]);
		msg->setPlainTextContent(str_[plaintext2]);
		msg->setIsRead(true);

		channel_messages_.push_back(regGlobalResource(msg = ::tracker()->createLiveResource<nmo::Message>()));
		msg->setCommunicationChannel(channel2_);
		msg->setSentDate(dates_[ch2msg2_date]);
		msg->setPlainTextContent(str_[plaintext3]);

		channel_messages_.push_back(regGlobalResource(msg = ::tracker()->createLiveResource<nmo::Message>()));
		msg->setCommunicationChannel(channel2_);
		msg->setSentDate(dates_[ch2msg3_date]);
		msg->setPlainTextContent(str_[plaintext4]);
		msg->setIsRead(true);
		channel2_->setLastMessageDate(dates_[ch2msg3_date]);

		channel_messages_.push_back(regGlobalResource(msg = ::tracker()->createLiveResource<nmo::Message>()));
		msg->setSentDate(dates_[nochmsg_date]);
		msg->setPlainTextContent(str_[plaintext5]);

		for(int i = 2, iend = ChannelCount; i < iend; ++i)
		{
			channels_[i] = regGlobalResource(::tracker()->createLiveResource<nmo::CommunicationChannel>());
			channels_[i]->addHasParticipant(regGlobalResource(::tracker()->createLiveResource<nco::PersonContact>()))
					->setFullname("Person/Channel#" + QString::number(i) + " participant");
			channels_[i]->addHasParticipant(nco::default_contact_me::iri());

			for(int m = 0, mend = i; m != mend; ++m)
			{
				channel_messages_.push_back(regGlobalResource(msg = ::tracker()->createLiveResource<nmo::Message>()));
				msg->setCommunicationChannel(channels_[i]);
				msg->setSentDate(dates_[m % DateCount]);
				msg->setPlainTextContent("Channel#" + QString::number(i) + "/m#" + QString::number(m));
				if(m < i - 2)
					msg->setIsRead(true);
			}
			channels_[i]->setLastMessageDate(dates_[i >= DateCount ? DateCount - 1 : i]);
		}

	}

	{
		Live<nmo::Email> email;

		emails_.push_back(regGlobalResource(email = ::tracker()->createLiveResource<nmo::Email>()));
		email->setPlainTextContent(str_[plaintext1]);
		email->setSubject(str_[subject1]);
		email->setFrom(contact1_);

		emails_.push_back(regGlobalResource(email = ::tracker()->createLiveResource<nmo::Email>()));
		email->setPlainTextContent(str_[plaintext2]);
		email->setSubject(str_[subject2]);
		email->setFrom(contact1_);

		emails_.push_back(regGlobalResource(email = ::tracker()->createLiveResource<nmo::Email>()));
		email->setPlainTextContent(str_[plaintext3]);
		email->setSubject(str_[subject3]);
		email->setFrom(contact1_);

		emails_.push_back(regGlobalResource(email = ::tracker()->createLiveResource<nmo::Email>()));
		email->setPlainTextContent(str_[plaintext4]);
		email->setSubject(str_[subject4]);
		email->setFrom(contact1_);

		emails_.push_back(regGlobalResource(email = ::tracker()->createLiveResource<nmo::Email>()));
		email->setPlainTextContent(str_[plaintext5]);
		email->setSubject(str_[subject5]);
		email->setFrom(contact1_);

		email_count_ += 5;
	}
}

void UnitTests::initTestContexts()
{
	// Initialize the data needed in all tests
	global_test_context = ::tracker()->liveNode(QUrl(global_test_context_str));
	local_test_context = ::tracker()->liveNode(QUrl(local_test_context_str));

	regGlobalResource(local_test_context);

	QVERIFY(::tracker()->modelVariable(global_test_context.variable())->rowCount() == 1);
	QVERIFY(::tracker()->modelVariable(local_test_context.variable())->rowCount() == 1);
}

void UnitTests::cleanupTestContexts()
{
	local_test_context = LiveNode();
	global_test_context = LiveNode();


	// Create all resources which are part of the local context or the global context. Doing this by
	// hand, so that we don't need to create the LiveNode:s for test contexts, if they don't exist yet.
	::tracker()->rawSparqlUpdateQuery("DELETE { ?uri a rdfs:Resource } "
									  "WHERE { ?uri nie:isLogicalPartOf "
									  "<" + local_test_context_str + "> .}");

	// this will also delete the local_test_context
	::tracker()->rawSparqlUpdateQuery("DELETE { ?uri a rdfs:Resource } "
									  "WHERE { ?uri nie:isLogicalPartOf "
									  "<" + global_test_context_str + "> .}");

	::tracker()->executeQuery(RDFUpdate().addDeletion
			(QUrl(global_test_context_str), rdf::type::iri(), rdfs::Resource::iri()))
			[0]->refreshModel(LiveNodeModel::Block);

	BackEnds::Tracker::sync(true);

	QCOMPARE(::tracker()->rawSparqlQuery("SELECT ?res WHERE {?res a rdfs:Resource . "
										 "FILTER(?res = <" + global_test_context_str + ">) }").size(), 0);
	QCOMPARE(::tracker()->rawSparqlQuery("SELECT ?res WHERE {?res a rdfs:Resource . "
										 "FILTER(?res = <" + local_test_context_str + ">) }").size(), 0);
}

void UnitTests::initGlobalData()
{
	// Create the data only once
	static bool done = false;

	if (done)
		return;
	done = true;

	{
		RDFTransactionPtr tx = ::tracker()->createTransaction();
		prepareGlobalData();
		tx->commit(true);
	}

	::tracker()->rawSparqlUpdateQuery("INSERT { <urn:channel:test01> a nmo:CommunicationChannel ; "
									  "nie:isLogicalPartOf <" + global_test_context_str + "> . }");
	::tracker()->rawSparqlUpdateQuery("INSERT { <urn:channel:test02> a nmo:CommunicationChannel ; "
									  "nie:isLogicalPartOf <" + global_test_context_str + "> . }");

	::tracker()->rawSparqlUpdateQuery("INSERT { <urn:msg:test01> a nmo:Message; "
									  "nie:isLogicalPartOf <" + global_test_context_str + "> ; "
									"nmo:communicationChannel <urn:channel:test01> ; "
									"nmo:receivedDate '2001-10-26T21:35:52' ; "
									"nie:plainTextContent 'test1' }");

	::tracker()->rawSparqlUpdateQuery("INSERT { <urn:msg:test02> a nmo:Message; "
									  "nie:isLogicalPartOf <" + global_test_context_str + "> ; "
									"nmo:communicationChannel <urn:channel:test02> ; "
									"nmo:receivedDate '2001-10-26T21:36:52' ; "
									"nie:plainTextContent 'test2' }");

	::tracker()->rawSparqlUpdateQuery("INSERT { <urn:msg:test03> a nmo:Message; "
									  "nie:isLogicalPartOf <" + global_test_context_str + "> ; "
									"nmo:communicationChannel <urn:channel:test02> ; "
									"nmo:receivedDate '2001-10-26T21:37:52' ; "
									"nie:plainTextContent 'test3' }");

	::tracker()->rawSparqlUpdateQuery("INSERT { <urn:msg:test04> a nmo:Message; "
									  "nie:isLogicalPartOf <" + global_test_context_str + "> ; "
									"nmo:communicationChannel <urn:channel:test02> ; "
									"nmo:receivedDate '2001-10-26T21:38:52' ; "
									"nie:plainTextContent 'test4' }");

	::tracker()->rawSparqlUpdateQuery("INSERT { <urn:msg:test05> a nmo:Message; "
									  "nie:isLogicalPartOf <" + global_test_context_str + "> ; "
									"nmo:receivedDate '2001-10-26T21:39:52' ; "
									"nie:plainTextContent 'test5' }");

	::tracker()->rawSparqlUpdateQuery("INSERT { <urn:contact:test01> a nco:Contact ; "
									  "nie:isLogicalPartOf <" + global_test_context_str + "> ; "
										"nco:hasEmailAddress 'test@test.com' }");

	::tracker()->rawSparqlUpdateQuery("INSERT { <urn:msg:test06> a nmo:Email; "
									  "nie:isLogicalPartOf <" + global_test_context_str + "> ; "
									"nmo:from <urn:contact:test01> }");
	::tracker()->rawSparqlUpdateQuery("INSERT { <urn:msg:test07> a nmo:Email; "
									  "nie:isLogicalPartOf <" + global_test_context_str + "> ; "
									"nmo:from <urn:contact:test01> }");
	::tracker()->rawSparqlUpdateQuery("INSERT { <urn:msg:test08> a nmo:Email; "
									  "nie:isLogicalPartOf <" + global_test_context_str + "> ; "
									"nmo:from <urn:contact:test01> }");
	::tracker()->rawSparqlUpdateQuery("INSERT { <urn:msg:test09> a nmo:Email; "
									  "nie:isLogicalPartOf <" + global_test_context_str + "> ; "
									"nmo:from <urn:contact:test01> }");
	::tracker()->rawSparqlUpdateQuery("INSERT { <urn:msg:test10> a nmo:Email; "
									  "nie:isLogicalPartOf <" + global_test_context_str + "> ; "
									"nmo:from <urn:contact:test01> }");
	BackEnds::Tracker::sync(true);

	// The queries for the global data creation shouldn't affect the query count of the tests.
	markQueryCount();
}

void UnitTests::cleanupGlobalData()
{
	emails_.clear();
	contact1_ = contact2_ = email_contact_ = LiveNode();
	channel_messages_.clear();
	channels_.clear();
}

void UnitTests::UnitSelf ()
{
	// do nothing - just init/cleanup
}

int main(int argc, char *argv[])
{
	int result = 0;

	/*! Let's parse app arguments to see which backend should we use in the suite.
	  * We have to erase our extra backend-choosing argument after all, because QTest
	  * will complain that he doesn't know it and exit with error.
	  * To make it simple, we assume that the extra parameter is the last one.
	  */
	for(int i=1; i<argc; i++)
	{
		if(QString(argv[i]).contains(QString("--qsparql-access")))
		{
			::UnitTestsVars::access_type = QString("QSPARQL_DIRECT");
			argv[i]=(char *)"";
			argc--;
		}
		if(QString(argv[i]).contains(QString("--dbus-access")))
		{
			::UnitTestsVars::access_type = QString("DBUS");
			argv[i]=(char *)"";
			argc--;
		}

	}
	// Let's run the test suite now.
	QCoreApplication app(argc, argv);
	UnitTests t1;
	result += QTest::qExec(&t1, argc, argv);

	return result;
}
