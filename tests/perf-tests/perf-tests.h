/*
 * Copyright (C) 2010 Nokia Corporation.
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
 */
#ifndef _QTTRACKER_PERF_TEST_H
#define _QTTRACKER_PERF_TEST_H

#include <QtTest>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QtDBus>
#include <QTime>

#include <QtTracker/ontologies/nie.h>
#include <QtTracker/ontologies/nfo.h>
#include <QtTracker/ontologies/nmm.h>
#include <QtTracker/Tracker>
#include <sopranolive/rdfchain.h>

#define PROFILE_TIMESTAMPS
#include "util_p.h"

using namespace SopranoLive;

class PerfTests : public QObject
{
    Q_OBJECT
	void testSuiteProcessEvents();
protected Q_SLOTS:
private:
	void waitForSignal(QObject *obj, char const *signal);

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();
	void init();
	void cleanup();

private Q_SLOTS:
	void basic_plainQuery_qtdbus();
    void basic_plainQuery_raw();
    void basic_plainQuery();

	void complex_albumsForArtist_raw();
	void complex_albumsForArtist();
	void complex_gallery();

private:
	int iterations;
};

Q_DECLARE_METATYPE(QVector<QStringList>);

#endif
