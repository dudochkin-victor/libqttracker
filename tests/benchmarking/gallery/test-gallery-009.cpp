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
 *
 */

#include <gallery-benchmark.h>

static SopranoLive::RDFSelect allImagesOnDevice ()
{
    SopranoLive::RDFVariable photos_var = isTestGraphResource (
                SopranoLive::RDFVariable::fromType<SopranoLive::nfo::FileDataObject>());

    photos_var.isOfType<SopranoLive::nfo::Visual>();

    SopranoLive::RDFSelect query;

    photos_var.isOfType<SopranoLive::nmm::Photo>(false, SopranoLive::RDFStrategy::DerivedProperty);
    query.addColumn ("camera", 
            photos_var.optional().property<SopranoLive::nfo::device>());
    query.addColumn ("url", photos_var.property<SopranoLive::nie::url>());
    query.addColumn ("exposure",
            photos_var.optional().property<SopranoLive::nmm::exposureTime>());
    query.addColumn ("fnumber",
            photos_var.optional().property<SopranoLive::nmm::fnumber>());
    query.addColumn ("flength",
            photos_var.optional().property<SopranoLive::nmm::focalLength>());
    query.addColumn ("mimetype", photos_var.property<SopranoLive::nie::mimeType>());
    query.addColumn ("photosandvideo", photos_var);
    return query;
}

void GalleryBenchmark::test_gallery_009_benchmark()
{
    QBENCHMARK {
        LiveNodes query_model = ::tracker()->modelQuery (allImagesOnDevice(),
                                                       SopranoLive::RDFStrategy::LiveStrategy);
        QCOMPARE(query_model->rowCount(), 500);
    }
}

void GalleryBenchmark::test_gallery_009_streaming_1()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allImagesOnDevice());

        QObject::connect(streaming_model.model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)) , this, SLOT(rowsAdded()));

        QTime timer;
        timer.start();

        while((timer.elapsed() < 10000) && (modelUpdates < 1))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 1);
        //QCOMPARE(streaming_model->rowCount(), 50);
    }
}

void GalleryBenchmark::test_gallery_009_streaming_2()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allImagesOnDevice());

        QObject::connect(streaming_model.model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)) , this, SLOT(rowsAdded()));

        QTime timer;
        timer.start();

        while((timer.elapsed() < 10000) && (modelUpdates < 1))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 1);
        //QCOMPARE(streaming_model->rowCount(), 50);
        while((timer.elapsed() < 10000) && (modelUpdates < 2))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 2);
        //QCOMPARE(streaming_model->rowCount(), 550);
    }
}

void GalleryBenchmark::test_gallery_009_streaming_end()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allImagesOnDevice());

        QObject::connect(streaming_model.model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)) , this, SLOT(rowsAdded()));

        QTime timer;
        timer.start();

        while((timer.elapsed() < 10000) && (modelUpdates < 1))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 1);
        //QCOMPARE(streaming_model->rowCount(), 50);
        while((timer.elapsed() < 10000) && (modelUpdates < 2))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 2);
        //QCOMPARE(streaming_model->rowCount(), 550);
        while((timer.elapsed() < 10000) && (modelUpdates < 20))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 20);
        //QCOMPARE(streaming_model->rowCount(), 15000);
    }
}
