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

/* Time taken for querying all images and videos without OPTIONAL*/
void GalleryBenchmark::test_gallery_004_benchmark()
{
    QBENCHMARK {
        SopranoLive::RDFVariable photos_and_videos_var = isTestGraphResource (
                    SopranoLive::RDFVariable::fromType<SopranoLive::nfo::FileDataObject>());

        photos_and_videos_var.isOfType<SopranoLive::nfo::Visual>();

        SopranoLive::RDFSelect query;

        photos_and_videos_var.isOfType<SopranoLive::nmm::Photo>(false,
                                                                SopranoLive::RDFStrategy::DerivedProperty);
        photos_and_videos_var.isOfType<SopranoLive::nmm::Video>(false, 
                                                                SopranoLive::RDFStrategy::DerivedProperty);
        query.addColumn ("url", photos_and_videos_var.property<SopranoLive::nie::url>());
        SopranoLive::RDFVariable name = photos_and_videos_var.property<SopranoLive::nfo::fileName>();
        query.addColumn ("filename", name);
        SopranoLive::RDFVariable modified =
                 photos_and_videos_var.property<SopranoLive::nfo::fileLastModified>();

        query.addColumn ("modified", modified);
        query.addColumn ("mimetype", photos_and_videos_var.property<SopranoLive::nie::mimeType>());
        query.addColumn ("duration", photos_and_videos_var.function<SopranoLive::nfo::duration>());
        query.addColumn ("photosandvideo", photos_and_videos_var);

        SortOrder sorting = Date;
        SopranoLive::RDFVariable sortOrder = ((sorting == Date) ? modified : name);

        query.orderBy (sortOrder, true);
        query.limit (10000);

        LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
        QCOMPARE(query_model->rowCount(), 10000);
    }
}

/* Time taken for querying 500 images and videos */
void GalleryBenchmark::test_gallery_005_benchmark()
{
    QBENCHMARK {
        SopranoLive::RDFVariable photos_and_videos_var = isTestGraphResource (
                    SopranoLive::RDFVariable::fromType<SopranoLive::nfo::FileDataObject>());

        photos_and_videos_var.isOfType<SopranoLive::nfo::Visual>();

        SopranoLive::RDFSelect query;

        photos_and_videos_var.isOfType<SopranoLive::nmm::Photo>(false,
                                                                SopranoLive::RDFStrategy::DerivedProperty);
        photos_and_videos_var.isOfType<SopranoLive::nmm::Video>(false, 
                                                                SopranoLive::RDFStrategy::DerivedProperty);
        query.addColumn ("url", photos_and_videos_var.property<SopranoLive::nie::url>());
        SopranoLive::RDFVariable name = photos_and_videos_var.property<SopranoLive::nfo::fileName>();
        query.addColumn ("filename", name);
        SopranoLive::RDFVariable modified =
                 photos_and_videos_var.property<SopranoLive::nfo::fileLastModified>();

        query.addColumn ("modified", modified);
        query.addColumn ("width", photos_and_videos_var.optional().property<SopranoLive::nfo::width>());
        query.addColumn ("height", photos_and_videos_var.optional().property<SopranoLive::nfo::height>());
        query.addColumn ("mimetype", photos_and_videos_var.property<SopranoLive::nie::mimeType>());
        query.addColumn ("duration", photos_and_videos_var.function<SopranoLive::nfo::duration>());
        query.addColumn ("photosandvideo", photos_and_videos_var);

        SortOrder sorting = Date;
        SopranoLive::RDFVariable sortOrder = ((sorting == Date) ? modified : name);

        query.orderBy (sortOrder, true);
        query.limit (500);

        LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
        QCOMPARE(query_model->rowCount(), 500);
    }
}

/* Time taken for querying 100 images and videos without OPTIONALS */
void GalleryBenchmark::test_gallery_006_benchmark()
{
    QBENCHMARK {
        SopranoLive::RDFVariable photos_and_videos_var = isTestGraphResource (
                    SopranoLive::RDFVariable::fromType<SopranoLive::nfo::FileDataObject>());

        photos_and_videos_var.isOfType<SopranoLive::nfo::Visual>();

        SopranoLive::RDFSelect query;

        photos_and_videos_var.isOfType<SopranoLive::nmm::Photo>(false,
                                                                SopranoLive::RDFStrategy::DerivedProperty);
        photos_and_videos_var.isOfType<SopranoLive::nmm::Video>(false, 
                                                                SopranoLive::RDFStrategy::DerivedProperty);
        query.addColumn ("url", photos_and_videos_var.property<SopranoLive::nie::url>());
        SopranoLive::RDFVariable name = photos_and_videos_var.property<SopranoLive::nfo::fileName>();
        query.addColumn ("filename", name);
        SopranoLive::RDFVariable modified =
                 photos_and_videos_var.property<SopranoLive::nfo::fileLastModified>();

        query.addColumn ("modified", modified);
        query.addColumn ("mimetype", photos_and_videos_var.property<SopranoLive::nie::mimeType>());
        query.addColumn ("duration", photos_and_videos_var.function<SopranoLive::nfo::duration>());
        query.addColumn ("photosandvideo", photos_and_videos_var);

        SortOrder sorting = Date;
        SopranoLive::RDFVariable sortOrder = ((sorting == Date) ? modified : name);

        query.orderBy (sortOrder, true);
        query.limit (100);

        LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
        QCOMPARE(query_model->rowCount(), 100);
    }
}

/* Time taken for querying 500 images */
void GalleryBenchmark::test_gallery_010_benchmark()
{
    QBENCHMARK {
        SopranoLive::RDFVariable photos_var = isTestGraphResource (
                    SopranoLive::RDFVariable::fromType<SopranoLive::nfo::FileDataObject>());

        photos_var.isOfType<SopranoLive::nfo::Visual>();

        SopranoLive::RDFSelect query;

        photos_var.isOfType<SopranoLive::nmm::Photo>(false, SopranoLive::RDFStrategy::DerivedProperty);
        query.addColumn ("camera", 
                photos_var.optional().property<SopranoLive::nfo::device>());
        query.addColumn ("url", photos_var.property<SopranoLive::nie::url>());
        query.addColumn ("width", photos_var.optional().property<SopranoLive::nfo::width>());
        query.addColumn ("height", photos_var.optional().property<SopranoLive::nfo::height>());
        query.addColumn ("exposure",
                photos_var.optional().property<SopranoLive::nmm::exposureTime>());
        query.addColumn ("fnumber",
                photos_var.optional().property<SopranoLive::nmm::fnumber>());
        query.addColumn ("flength",
                 photos_var.optional().property<SopranoLive::nmm::focalLength>());
        query.addColumn ("mimetype", photos_var.property<SopranoLive::nie::mimeType>());
        query.addColumn ("photosandvideo", photos_var);

        query.limit (500);

        LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
        QCOMPARE(query_model->rowCount(), 500);
    }
}

/* Time taken for querying all images */
/* TODO - Should these be optionals */
void GalleryBenchmark::test_gallery_012_benchmark()
{
    QBENCHMARK {
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

        LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
        QCOMPARE(query_model->rowCount(), 10000);
    }
}

/* Time taken for querying 500 images */
/* TODO - should these be optionals? */
void GalleryBenchmark::test_gallery_013_benchmark()
{
    QBENCHMARK {
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

        query.limit(500);

        LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
        QCOMPARE(query_model->rowCount(), 500);
    }
}
