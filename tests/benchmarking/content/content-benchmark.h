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
#ifndef _CONTENT_BENCHMARK_H
#define _CONTENT_BENCHMARK_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QtTest>

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/tracker.h>
#include <QtTracker/ontologies/nie.h>
#include <QtTracker/ontologies/nmo.h>
#include <QtTracker/ontologies/nco.h>
#include <QtTracker/ontologies/fts.h>

using namespace SopranoLive;

class ContentBenchmark : public QObject
{
        Q_OBJECT
public:
        ContentBenchmark ();

private slots:
        void test_cm_01 ();
        void test_cm_02 ();
        void test_cm_03 ();
        void test_cm_04 ();
        void test_cm_05 ();
        void test_cm_06 ();
};

#endif /* _CONTENT_BENCHMARK_H */
