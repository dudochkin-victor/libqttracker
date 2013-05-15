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

static SopranoLive::RDFSelect allVisualsOnDevice ()
{
    SopranoLive::RDFVariable visuals_var = isTestGraphResource (
                SopranoLive::RDFVariable::fromType<SopranoLive::nfo::FileDataObject>());

    visuals_var.isOfType<SopranoLive::nfo::Visual>();

    SopranoLive::RDFSelect query;

    visuals_var.property<SopranoLive::nfo::device>(SopranoLive::LiteralValue(QString("NOKIA")));

    query.addColumn ("visuals", visuals_var);


    return query;
}

void GalleryBenchmark::test_gallery_002_benchmark()
{
    QBENCHMARK {
        LiveNodes query_model = ::tracker()->modelQuery (allVisualsOnDevice(),
                                                       SopranoLive::RDFStrategy::LiveStrategy);
        QCOMPARE(query_model->rowCount(), 4964);
    }
}

void GalleryBenchmark::test_gallery_002_streaming_1()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allVisualsOnDevice());

        QObject::connect(streaming_model.model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)) , this, SLOT(rowsAdded()));

        QTime timer;
        timer.start();

        while((timer.elapsed() < 10000) && (modelUpdates < 1))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 1);
        //QCOMPARE(streaming_model->rowCount(), 50);
    }
}

void GalleryBenchmark::test_gallery_002_streaming_2()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allVisualsOnDevice());

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

void GalleryBenchmark::test_gallery_002_streaming_3()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allVisualsOnDevice());

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

void GalleryBenchmark::test_gallery_002_streaming_4()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allVisualsOnDevice());

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

void GalleryBenchmark::test_gallery_002_streaming_5()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allVisualsOnDevice());

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

void GalleryBenchmark::test_gallery_002_streaming_6()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allVisualsOnDevice());

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

void GalleryBenchmark::test_gallery_002_streaming_7()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allVisualsOnDevice());

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

void GalleryBenchmark::test_gallery_002_streaming_end()
{
    QBENCHMARK {
        modelUpdates = 0;
        LiveNodes streaming_model = setStreamingQuery(allVisualsOnDevice());

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
        while((timer.elapsed() < 10000) && (modelUpdates < 10))
            QCoreApplication::processEvents();
        QCOMPARE(modelUpdates, 10);
        //QCOMPARE(streaming_model->rowCount(), 15000);
    }
}
