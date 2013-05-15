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
#ifndef _QTTRACKER_UNIT_TEST_H
#define _QTTRACKER_UNIT_TEST_H

#include <QtTest>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QFile>
#include <QTextStream>

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nie.h>
#include <QtTracker/ontologies/nco.h>
#include <QtTracker/ontologies/nmo.h>
#include <QtTracker/ontologies/mfo.h>
#include <QtTracker/ontologies/nfo.h>
#include <QtTracker/ontologies/tracker.h>
#include <QtTracker/domains/tags.h>
#include <sopranolive/rdfchain.h>

#include "livenodemodeltester.h"

using namespace SopranoLive;

class UnitTests : public QObject
{
	Q_OBJECT

public:
	UnitTests();

	enum QueryType { ObjectBasedQuery, TextualQuery };

private:
	static const QString global_test_context_str, local_test_context_str;
	Live<nie::InformationElement> global_test_context, local_test_context;

	// Test resources.
	// test resources have two main categories:
	// 1. test global resources shared by all tests in the test executable
	// 2. test resources used by a single test function

	// Test function naming
	// as informative guideline, prefix the test with one of the following
	// raw: only raw layer functions are used (raw sparql, functions that have raw in their name, etc.)
	// unbound: query building test (might still require tracker to be installed)
	// base: base service layer test, livenode/model, service management
	// conv: tests focusing on generated convenience class usage
	// domain: domain tests
	// raw and domain tests should be placed inside core-suite.cpp
	// other tests should be placed in their own base-suite.cpp conv-suite.cpp and unbound-suite.cpp

	QList<Live<nmo::CommunicationChannel> > prepareChannels(int count);

	// registers the given \a resource as a global test resource
		template<typename T_>
	T_ &regGlobalResource(T_ &resource)
	{
		Live<nie::InformationElement>(resource)->addIsLogicalPartOf(global_test_context);
		return resource;
	}

	// \overload
		template<typename T_>
	T_ const &regGlobalResource(T_ const &resource)
	{
		Live<nie::InformationElement>(resource)->addIsLogicalPartOf(global_test_context);
		return resource;
	}

	// registers the given \a resource as a local test resource
		template<typename T_>
	T_ &regLocalResource(T_ &resource)
	{
		Live<nie::InformationElement>(resource)->addIsLogicalPartOf(local_test_context);
		return resource;
	}

	// \overload
		template<typename T_>
	T_ const &regLocalResource(T_ const &resource)
	{
		Live<nie::InformationElement>(resource)->addIsLogicalPartOf(local_test_context);
		return resource;
	}

	RDFVariable isGlobalResource(RDFVariable const &resource = RDFVariable());
	RDFVariable isLocalResource(RDFVariable const &resource = RDFVariable());
	RDFVariable isTestResource(RDFVariable const &resource = RDFVariable());
	void clearLocalResources();
	typedef QString TestName;
	QMap<TestName, int> query_count_watermarks_;
	QMap<TestName, int> query_counts_;
	QString current_test_;
	int current_query_mark_, current_update_mark_;

	void markQueryCount();
	void checkQueryCount(QString const &testname);
	void outputQueryCounts();
	void initQueryCountWatermarks();

	// https://projects.maemo.org/bugzilla/show_bug.cgi?id=153757
	Live<nco::IMContact> imContactForTpContactId(const QString& tpCId );

	// For storing global test data
	QMap<int, QString> str_;
	enum
	{
		email_aaa, email_yyy
		, phone_5544, phone_5566
		, street_A, street_B, street_C
		, num1, num2
		, favorite, recent, happy
		, plaintext1, plaintext2, plaintext3, plaintext4, plaintext5
		, subject1, subject2, subject3, subject4, subject5
	};
	QMap<int, QDateTime> dates_;
	enum
	{
		date1, date2, date3, date4, date5
	,	DateCount = 10
	,	ch2msg1_date = 0, ch1msg1_date, ch2msg2_date, ch2msg3_date, nochmsg_date
	,	ChannelCount = 10
	};
	Live<nco::PersonContact> contact1_, contact2_;
	QList<Live<nmo::CommunicationChannel> > channels_;
	Live<nmo::CommunicationChannel> &channel1_, &channel2_;

	QList<Live<nmo::Message> > channel_messages_;
	Live<nmo::Contact> email_contact_;
	QList<Live<nmo::Email> > emails_;
	int email_count_;
	bool simple_signal_flag;
	int signal_count[6];
	int getDerivedsWhenModelUpdated_helper_count;
	RDFStrategyFlags last_mask, last_flags;
	enum { StrategyAlteredSignal = 5 };

protected slots:
	void simpleSignalSlot();
	void signalSlot0();
	void signalSlot1();
	void signalSlot2();
	void signalSlot3();
	void signalSlot4();
	void strategyAltered(RDFStrategyFlags, RDFStrategyFlags, QModelIndex const &);
	void getDerivedsWhenModelUpdated_helper();

private:

	void coreSuite_cleanup();
	// Setting up the test environment
	void initTestContexts();
	void cleanupTestContexts();

	// If you need the global test data, call this in your test.
	void initGlobalData();
	// This you don't need to call yourself.
	void cleanupGlobalData();
	void prepareGlobalData();

	// A test case which can only be executed after cleanup
	void special_trackerRelease();

	void testSuiteProcessEvents();

private slots: // test cases
	void init();
	void cleanup();
	void initTestCase();
	void cleanupTestCase();

	void UnitSelf();

	void stress_testTrackerHeavyLoad();
	void stress_testTrackerHeavyLoad_data();
	void raw_ResourceLifetime();
	void base_ResourceLifetime();
	void conv_ResourceLifetime();
	void unbound_PhoneNumberContains();
	void conv_TransactionResources();
	void unbound_RDFVariableLink();
	void unbound_RDFProperty();
	void base_hasTypeMusicAlbum();
	void base_AffilOptionalUnboundUpdate();
	void conv_ContactManipulationQuerying();
	void domain_Tags();
	void conv_addHasAffil();
	void conv_firstHasEmailAddress();
	void base_OrderedModel();
	void unbound_IMAccountIsBoundNot();
	void unbound_ContainsEscapeCharacter();
	void base_LiveNodeModelRemoveRows();
        void base_LiveNodeModel_wholeIndex();
        void base_LiveNodeModel_findColumnsByName();
        void base_LiveNodeModel_subModel();
	void base_SimpleAddCachedObject();
	void base_signalEmission();
	void base_setMultipleTimes();
	void conv_sequentialAddContainsBookmarkFolder();
	void base_LiveNodesNodesAccess();
	void base_selectUrnFilteredPhotos();
	void base_addRowsToModel();
	void unbound_queryContactAndMinProperties();
	void conv_setAffilTitle();
	void conv_getBigInt();
	void base_aggregateModelVariables();
	void base_childModels();
	void base_setDataTest_data();
	void base_setDataTest();
	void base_testDummyModel();
	void conv_contentLastModified();
	void conv_testFileLastAccessed();
	void unbound_isZeroOrDoesntExist();
	void unbound_optionalChildOfLink();
	void unbound_messagingSubQuery();
	void unbound_messagecountSubQuery();
	void base_liveQuery();
	void unbound_SubQueryCrash();
	void base_modelUpdatedSignal();
	void conv_wildcardPropertyRemove();
	void unbound_unionChildren();
	void base_modelTests();
	void unbound_nonColumnOrderBy();
	void unbound_groupConcat();
	void conv_staticResourcesSupport();
	void base_setDataOnInsertedRow();
	void base_liveTranscendental();
	void base_chainedNonexistentDerivedProperties();
	void tracker_signalsTests_data();
	void tracker_signalsTests();
	void base_TransactionCommitFinished();
	void base_TransactionBatchCommitFinished();
	void base_TransactionRollbackFinished();
	void conv_getDerivedsWhenModelUpdated();
	void conv_storingQDateTimeWithLocal();
	void conv_storingQDateTimeWithUTC();
	void conv_storingQDateTimeWithUTCOffset();
	void base_storingQDateTimeWithLocal();
	void base_storingQDateTimeWithUTC();
	void base_storingQDateTimeWithUTCOffset();
	void conv_picturesAroundNoon();
	void base_multiurlModel();
	void base_strictMultiurlModel();
	void base_strictMultiurlModelWithWrongTypes();
	void base_multiurlModelOrdered();
	void base_nonStrictMultiurlModel();
	void base_multiurlWithDeriveds();
	void base_multiurlWithDerivedsLifetime();
	void conv_setObjectWithEmpty();
	void conv_setObjectsWithEmpty();
	void raw_blankUpdateResults();
	void base_blankUpdateResults();
	void unbound_graphSupport();
	void unbound_orderByOuterVariable();
	void base_livePhotos();
	void domain_tags_addAndRemoveTags();
	void base_removeLiveNodeAlsoInAModel();
	void conv_IMCapabilityAudioCalls();
	void base_getIsRead();
	void tracker_subjectsAdded();
	void base_streamingResults();
	void base_streamingResultsOrdered();
	void domain_tags_hasAnyTags();
	void domain_tags_hasAllTags();
	void soprano_vocabulary_xsd();
	void soprano_vocabulary_rdf();
	void soprano_languagetag_constructors();
	void soprano_languagetag_operators();
	void soprano_languagetag_lookup();
	void soprano_languagetag_matches();
	void soprano_literalvalue_defaults();
	void soprano_literalvalue_operators();
	void soprano_literalvalue_debug();
	void unbound_insertingEmptyUrl();
	void tracker_sync();
	void base_simpleAddDerivedObject();
	void base_rdfGraph_modelResources();
	void base_rdfGraph_removeResources();
	void base_rdfGraph_modelVariable();
	void sparsevector_size();
	void sparsevector_append();
	void sparsevector_index();
	void sparsevector_insert();
	void sparsevector_replace();
	void sparsevector_reset();
	void sparsevector_erase();
	void sparsevector_clear();
	void sparsevector_resize();
	void sparsevector_constructor();
	void sparsevector_copy_constructor();
	void sparsevector_assignment_op();
	void sparsevector_index_op();
	void sparsevector_find();
	void base_streamingOnOrderedFunctionColumn();
	void unbound_definiteConnectedUpdate();
	void tracker_dateToDateTime();
	void base_liveFeeds();
	void unbound_literalColumns();
	void base_removeResources();
	void base_modelExercise();
	void unbound_filterFalse();
	void unbound_silentOperations();
	void base_liveVisual();
	void base_identityColumn();
};
#endif
