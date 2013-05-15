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

static SopranoLive::RDFSelect allImagesAndVideos ()
{   
    SopranoLive::RDFVariable photos_and_videos_var = isTestGraphResource (
                SopranoLive::RDFVariable::fromType<SopranoLive::nfo::FileDataObject>());

    photos_and_videos_var.isOfType<SopranoLive::nfo::Visual>();

    SopranoLive::RDFSelect query;

    photos_and_videos_var.isOfType<SopranoLive::nmm::Photo>(false, SopranoLive::RDFStrategy::DerivedProperty);
    photos_and_videos_var.isOfType<SopranoLive::nmm::Video>(false, SopranoLive::RDFStrategy::DerivedProperty);
    query.addColumn ("url", photos_and_videos_var.property<SopranoLive::nie::url>());
    SopranoLive::RDFVariable name = photos_and_videos_var.property<SopranoLive::nfo::fileName>();
    query.addColumn ("filename", name);
    SopranoLive::RDFVariable modified = photos_and_videos_var.property<SopranoLive::nfo::fileLastModified>();

    query.addColumn ("modified", modified);
    query.addColumn ("width", photos_and_videos_var.optional().property<SopranoLive::nfo::width>());
    query.addColumn ("height", photos_and_videos_var.optional().property<SopranoLive::nfo::height>());
    query.addColumn ("mimetype", photos_and_videos_var.property<SopranoLive::nie::mimeType>());
    query.addColumn ("duration", photos_and_videos_var.function<SopranoLive::nfo::duration>());
    query.addColumn ("photosandvideo", photos_and_videos_var);

    SortOrder sorting = Date;
    SopranoLive::RDFVariable sortOrder = ((sorting == Date) ? modified : name);

    query.orderBy (sortOrder, true);

    return query;
}

void GalleryBenchmark::test_gallery_001_benchmark()
{
    QBENCHMARK {
        LiveNodes query_model = ::tracker()->modelQuery (allImagesAndVideos(),
                                                       SopranoLive::RDFStrategy::LiveStrategy);
        QCOMPARE(query_model->rowCount(), 15000);
    }
}

void GalleryBenchmark::test_gallery_001_streaming_1()
{
    ::tracker()->setVerbosity(4);
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allImagesAndVideos());

        QObject::connect(streaming_model.model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)) , this, SLOT(rowsAdded()));

        QTime timer;
        timer.start();

        while((timer.elapsed() < 10000) && (modelUpdates < 1))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 1);
        //QCOMPARE(streaming_model->rowCount(), 50);
    }
}

void GalleryBenchmark::test_gallery_001_streaming_2()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allImagesAndVideos());

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

void GalleryBenchmark::test_gallery_001_streaming_3()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allImagesAndVideos());

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
        while((timer.elapsed() < 10000) && (modelUpdates < 3))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 3);
        //QCOMPARE(streaming_model->rowCount(), 1050);
    }

}

void GalleryBenchmark::test_gallery_001_streaming_4()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allImagesAndVideos());

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
        while((timer.elapsed() < 10000) && (modelUpdates < 3))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 3);
        //QCOMPARE(streaming_model->rowCount(), 1050);
        while((timer.elapsed() < 10000) && (modelUpdates < 4))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 4);
        //QCOMPARE(streaming_model->rowCount(), 1550);
    }

}

void GalleryBenchmark::test_gallery_001_streaming_5()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allImagesAndVideos());

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
        while((timer.elapsed() < 10000) && (modelUpdates < 3))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 3);
        //QCOMPARE(streaming_model->rowCount(), 1050);
        while((timer.elapsed() < 10000) && (modelUpdates < 4))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 4);
        //QCOMPARE(streaming_model->rowCount(), 1550);
        while((timer.elapsed() < 10000) && (modelUpdates < 5))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 5);
        //QCOMPARE(streaming_model->rowCount(), 2050);
    }

}

void GalleryBenchmark::test_gallery_001_streaming_6()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allImagesAndVideos());

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
        while((timer.elapsed() < 10000) && (modelUpdates < 3))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 3);
        //QCOMPARE(streaming_model->rowCount(), 1050);
        while((timer.elapsed() < 10000) && (modelUpdates < 4))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 4);
        //QCOMPARE(streaming_model->rowCount(), 1550);
        while((timer.elapsed() < 10000) && (modelUpdates < 5))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 5);
        //QCOMPARE(streaming_model->rowCount(), 2050);
        while((timer.elapsed() < 10000) && (modelUpdates < 6))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 6);
        //QCOMPARE(streaming_model->rowCount(), 2550);
    }

}

void GalleryBenchmark::test_gallery_001_streaming_7()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allImagesAndVideos());

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
        while((timer.elapsed() < 10000) && (modelUpdates < 3))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 3);
        //QCOMPARE(streaming_model->rowCount(), 1050);
        while((timer.elapsed() < 10000) && (modelUpdates < 4))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 4);
        //QCOMPARE(streaming_model->rowCount(), 1550);
        while((timer.elapsed() < 10000) && (modelUpdates < 5))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 5);
        //QCOMPARE(streaming_model->rowCount(), 2050);
        while((timer.elapsed() < 10000) && (modelUpdates < 6))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 6);
        //QCOMPARE(streaming_model->rowCount(), 2550);
        while((timer.elapsed() < 10000) && (modelUpdates < 7))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 7);
        //QCOMPARE(streaming_model->rowCount(), 3050);
    }

}

void GalleryBenchmark::test_gallery_001_streaming_end()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allImagesAndVideos());

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
        while((timer.elapsed() < 10000) && (modelUpdates < 3))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 3);
        //QCOMPARE(streaming_model->rowCount(), 1050);
        while((timer.elapsed() < 10000) && (modelUpdates < 4))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 4);
        //QCOMPARE(streaming_model->rowCount(), 1550);
        while((timer.elapsed() < 10000) && (modelUpdates < 5))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 5);
        //QCOMPARE(streaming_model->rowCount(), 2050);
        while((timer.elapsed() < 10000) && (modelUpdates < 6))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 6);
        //QCOMPARE(streaming_model->rowCount(), 2550);
        while((timer.elapsed() < 10000) && (modelUpdates < 7))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 7);
        //QCOMPARE(streaming_model->rowCount(), 3050);
        while((timer.elapsed() < 10000) && (modelUpdates < 30))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 30);
        //QCOMPARE(streaming_model->rowCount(), 15000);
    }
}
