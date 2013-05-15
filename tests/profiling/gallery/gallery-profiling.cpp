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

#include <gallery-profiling.h>
#include <streaming-mode-wait.h>

// this function is called by both of our queries
LiveNodes setQuery(SopranoLive::RDFSelect const & query)
{
    ::tracker()->setServiceAttribute("streaming_first_block_size", QVariant(50));
    ::tracker()->setServiceAttribute("streaming_block_size", QVariant(500));

    return ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy | SopranoLive::RDFStrategy::StreamingStrategy );
}

GalleryProfiling::GalleryProfiling ()
{
    ;
}

enum SortOrder {
    Date = 0,
    Name = 1
};

LiveNodes GalleryProfiling::queryContent(bool limit)
{   
    SopranoLive::RDFVariable photos_and_videos_var(SopranoLive::RDFVariable::fromType<SopranoLive::nfo::FileDataObject>());
    photos_and_videos_var.isOfType<SopranoLive::nfo::Visual>();

    SopranoLive::RDFSelect query;

    photos_and_videos_var.isOfType<SopranoLive::nmm::Photo>(false, SopranoLive::RDFStrategy::DerivedProperty);
    photos_and_videos_var.isOfType<SopranoLive::nmm::Video>(false, SopranoLive::RDFStrategy::DerivedProperty);

    query.addColumn ("url", photos_and_videos_var.property<SopranoLive::nie::url>());
    SopranoLive::RDFVariable name = photos_and_videos_var.property<SopranoLive::nfo::fileName>(); query.addColumn ("filename", name);
    //query.addColumn ("filename", photos_and_videos_var.property<SopranoLive::nfo::fileName>());
    SopranoLive::RDFVariable modified = photos_and_videos_var.property<SopranoLive::nfo::fileLastModified>();

    query.addColumn ("modified", modified);
    query.addColumn ("width", photos_and_videos_var.optional().property<SopranoLive::nfo::width>());
    query.addColumn ("height", photos_and_videos_var.optional().property<SopranoLive::nfo::height>());
    query.addColumn ("mimetype", photos_and_videos_var.property<SopranoLive::nie::mimeType>());
    query.addColumn ("duration", photos_and_videos_var.function<SopranoLive::nfo::duration>());
    query.addColumn ("photosandvideo", photos_and_videos_var);

    SortOrder sorting = Date;
    SopranoLive::RDFVariable sortOrder = ((sorting == Date) ? modified : name);

    // Order the query by default
    query.orderBy(sortOrder, true);

    if (limit){
        query.limit(50);
    }

    return ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
}

LiveNodes GalleryProfiling::queryContentStreaming()
{   
    SopranoLive::RDFVariable photos_and_videos_var(SopranoLive::RDFVariable::fromType<SopranoLive::nfo::FileDataObject>());
    photos_and_videos_var.isOfType<SopranoLive::nfo::Visual>();

    SopranoLive::RDFSelect query;

    photos_and_videos_var.isOfType<SopranoLive::nmm::Photo>(false, SopranoLive::RDFStrategy::DerivedProperty);
    photos_and_videos_var.isOfType<SopranoLive::nmm::Video>(false, SopranoLive::RDFStrategy::DerivedProperty);

    query.addColumn ("url", photos_and_videos_var.property<SopranoLive::nie::url>());
    SopranoLive::RDFVariable name = photos_and_videos_var.property<SopranoLive::nfo::fileName>(); query.addColumn ("filename", name);
    //query.addColumn ("filename", photos_and_videos_var.property<SopranoLive::nfo::fileName>());
    SopranoLive::RDFVariable modified = photos_and_videos_var.property<SopranoLive::nfo::fileLastModified>();

    query.addColumn ("modified", modified);
    query.addColumn ("width", photos_and_videos_var.optional().property<SopranoLive::nfo::width>());
    query.addColumn ("height", photos_and_videos_var.optional().property<SopranoLive::nfo::height>());
    query.addColumn ("mimetype", photos_and_videos_var.property<SopranoLive::nie::mimeType>());
    query.addColumn ("duration", photos_and_videos_var.function<SopranoLive::nfo::duration>());
    query.addColumn ("photosandvideo", photos_and_videos_var);

    SortOrder sorting = Date;
    SopranoLive::RDFVariable sortOrder = ((sorting == Date) ? modified : name);

    // Order the query by default
    query.orderBy(sortOrder, true);

    // Store LiveNodes object to later usage   
    return setQuery(query);
}

void GalleryProfiling::queryContentBenchmark()
{
    QBENCHMARK {
        LiveNodes query_model = queryContent(false);
        QCOMPARE(query_model->rowCount(), 15000);
    }
}

void GalleryProfiling::queryContentStreamingBasicBenchmark()
{
    QBENCHMARK {
        LiveNodes query_model = queryContentStreaming();

        StreamingModeWait basic_wait (query_model);
        basic_wait.waitOnFinished();

        QCOMPARE(query_model->rowCount(), 15000);
    }
}

void GalleryProfiling::rowsAdded()
{
    modelUpdates++;
}

void GalleryProfiling::queryContentStreaming_1()
{
    //::tracker()->setVerbosity(4);

    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = queryContentStreaming();

        QObject::connect(streaming_model.model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)) , this, SLOT(rowsAdded()));

        QTime timer;
        timer.start();

        while((timer.elapsed() < 10000) && (modelUpdates < 1))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 1);
        //QCOMPARE(streaming_model->rowCount(), 50);
    }
}

void GalleryProfiling::queryContentStreaming_2()
{
    //::tracker()->setVerbosity(2);

    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = queryContentStreaming();

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

void GalleryProfiling::queryContentStreaming_3()
{
    //::tracker()->setVerbosity(2);

    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = queryContentStreaming();

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

void GalleryProfiling::queryContentStreaming_4()
{
    //::tracker()->setVerbosity(2);

    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = queryContentStreaming();

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

void GalleryProfiling::queryContentStreaming_5()
{
    //::tracker()->setVerbosity(2);

    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = queryContentStreaming();

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

void GalleryProfiling::queryContentStreaming_6()
{
    //::tracker()->setVerbosity(2);

    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = queryContentStreaming();

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

void GalleryProfiling::queryContentStreaming_7()
{
    //::tracker()->setVerbosity(2);

    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = queryContentStreaming();

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

void GalleryProfiling::queryContentStreaming_8()
{
    //::tracker()->setVerbosity(2);

    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = queryContentStreaming();

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

#if 0
// Then the Camera Roll query, which is exactly the same otherwise but 
// it uses a filter to only get content
// that have "booti 008" in property nmm:camera

QString deviceMake()
{
    return QString("booti");
}

QString deviceModel()
{
    return QString("008");
}

void GalleryProfiling::queryCameraContent()
{   
    SopranoLive::RDFVariable 
    photos_and_videos_var(SopranoLive::RDFVariable::fromType<SopranoLive::nfo::FileDataObject>());
    photos_and_videos_var.isOfType<SopranoLive::nfo::Visual>();

    SopranoLive::RDFSelect query;

    photos_and_videos_var.isOfType<SopranoLive::nmm::Photo>(false, SopranoLive::RDFStrategy::DerivedProperty);
    photos_and_videos_var.isOfType<SopranoLive::nmm::Video>(false, SopranoLive::RDFStrategy::DerivedProperty);

    query.addColumn ("url", photos_and_videos_var.property<SopranoLive::nie::url>());
    query.addColumn ("filename", photos_and_videos_var.property<SopranoLive::nfo::fileName>());
    SopranoLive::RDFVariable modified = photos_and_videos_var.property<SopranoLive::nfo::fileLastModified>();
    query.addColumn ("modified", modified);
    query.addColumn ("width", photos_and_videos_var.optional().property<SopranoLive::nfo::width>());
    query.addColumn ("height", photos_and_videos_var.optional().property<SopranoLive::nfo::height>());
    query.addColumn ("mimetype", photos_and_videos_var.property<SopranoLive::nie::mimeType>());
    query.addColumn ("duration", photos_and_videos_var.function<SopranoLive::nfo::duration>());
    query.addColumn ("photosandvideo", photos_and_videos_var);
    // TODO: Change nmm::camera to nfo::device when tracker
    // libqttracker has been integrated
    photos_and_videos_var.property<SopranoLive::nmm::camera>(SopranoLive::LiteralValue(deviceMake() + ' ' + deviceModel()));

    // Sort by date, newest first.
    query.orderBy(2, SopranoLive::RDFSelect::Ascending);

    if (true){
        query.limit(1);
    }

    // Store RDFSelect object to later usage
    setQuery(query);
}
#endif 

// Main function and entry point
QTEST_MAIN(GalleryProfiling)
