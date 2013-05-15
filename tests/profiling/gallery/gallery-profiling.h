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
#ifndef _GALLERY_PROFILING_H
#define _GALLERY_PROFILING_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QtTest>

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/tracker.h>
#include <QtTracker/ontologies/nmm.h>

using namespace SopranoLive;

class GalleryProfiling : public QObject
{
        Q_OBJECT
public:
        GalleryProfiling ();

public slots:
        void rowsAdded ();

private:
        int modelUpdates;

        LiveNodes queryContent(bool);
        LiveNodes queryContentStreaming();
        //void queryCameraContent();

private slots:
        void queryContentBenchmark();
        void queryContentStreamingBasicBenchmark();

        void queryContentStreaming_1();
        void queryContentStreaming_2();
        void queryContentStreaming_3();
        void queryContentStreaming_4();
        void queryContentStreaming_5();
        void queryContentStreaming_6();
        void queryContentStreaming_7();
        void queryContentStreaming_8();
};

#endif /* _GALLERY_PROFILING_H */
