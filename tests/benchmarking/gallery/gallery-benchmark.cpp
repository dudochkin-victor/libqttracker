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

GalleryBenchmark::GalleryBenchmark ()
{
    ;
}

void GalleryBenchmark::test_gallery_01()
{
  /*
  SELECT ?url ?filename ?modified ?_width ?_height
  WHERE {
        ?media a nfo:Visual;
        nie:url ?url;
        nfo:fileName ?filename ;
        nfo:fileLastModified ?modified .
        OPTIONAL    {?media nfo:width ?_width. }
        OPTIONAL   { ?media nfo:height ?_height .}
  }
  ORDER BY ?modified LIMIT 10000
  */

  QBENCHMARK {
    RDFSelect query;
    RDFVariable visual = RDFVariable::fromType<nfo::Visual>("media");


    RDFVariable name = visual.property<nfo::fileName>();
    query.addColumn ("url", visual.property<nie::url>());
    query.addColumn ("filename", name);
    RDFVariable modified = visual.property<nfo::fileLastModified>();

    query.addColumn ("modified", modified);
    query.addColumn ("width", visual.optional().property<nfo::width>());
    query.addColumn ("height", visual.optional().property<nfo::height>());

    query.orderBy (modified, true);
    query.limit (10000);

    LiveNodes query_model = ::tracker()->modelQuery (query, RDFStrategy::LiveStrategy);
        if (query_model->rowCount() != 1000) {
                QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(10000).arg(query_model->rowCount())));
        }
  }
}

void GalleryBenchmark::test_gallery_04()
{
  /*
  SELECT ?url ?filename ?modified
  WHERE {
        ?media a nfo:Visual;
        nie:url ?url;
        nfo:fileName ?filename ;
        nfo:fileLastModified ?modified .
  }
  ORDER BY ?modified LIMIT 10000"
  */
  QBENCHMARK {
    RDFSelect query;
    RDFVariable visual = RDFVariable::fromType<nfo::Visual>("media");

    query.addColumn ("url", visual.property<nie::url>());
    query.addColumn ("filename", visual.property<nfo::fileName>());

    RDFVariable modified = visual.property<nfo::fileLastModified>();
    query.addColumn ("modified", modified);

    query.orderBy (modified, true);
    query.limit (10000);

    LiveNodes query_model = ::tracker()->modelQuery (query, RDFStrategy::LiveStrategy);
        if (query_model->rowCount() != 10000) {
                QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(10000).arg(query_model->rowCount())));
        }
  }
}

void GalleryBenchmark::test_gallery_05()
{
  /*
   SELECT ?url ?filename ?modified ?_width ?_height
   WHERE {
        ?media a nfo:Visual;
        nie:url ?url;
        nfo:fileName ?filename ;
        nfo:fileLastModified ?modified .
        OPTIONAL    {?media nfo:width ?_width. }
        OPTIONAL   { ?media nfo:height ?_height .}
   }
   ORDER BY ?modified LIMIT 500"
  */
  QBENCHMARK {
    RDFSelect query;
    RDFVariable visual = RDFVariable::fromType<nfo::Visual>("media");


    RDFVariable name = visual.property<nfo::fileName>();
    query.addColumn ("url", visual.property<nie::url>());
    query.addColumn ("filename", name);
    RDFVariable modified = visual.property<nfo::fileLastModified>();

    query.addColumn ("modified", modified);
    query.addColumn ("width", visual.optional().property<nfo::width>());
    query.addColumn ("height", visual.optional().property<nfo::height>());

    query.orderBy (modified, true);
    query.limit (500);

    LiveNodes query_model = ::tracker()->modelQuery (query, RDFStrategy::LiveStrategy);
        if (query_model->rowCount() != 500) {
                QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(500).arg(query_model->rowCount())));
        }
  }
}

/* Time taken for querying all images and videos without OPTIONAL*/
void GalleryBenchmark::test_gallery_06()
{
   /*
    SELECT ?url ?filename ?modified
    WHERE {
        ?media a nfo:Visual;
        nie:url ?url;
        nfo:fileName ?filename ;
        nfo:fileLastModified ?modified .
    }
    ORDER BY ?modified LIMIT 500"
    */
    QBENCHMARK {
        RDFSelect query;
        RDFVariable visual = RDFVariable::fromType<nfo::Visual>("media");

        query.addColumn ("url", visual.property<nie::url>());
        query.addColumn ("filename", visual.property<nfo::fileName>());

        RDFVariable modified = visual.property<nfo::fileLastModified>();
        query.addColumn ("modified", modified);

        query.orderBy (modified, true);
        query.limit (500);

        LiveNodes query_model = ::tracker()->modelQuery (query, RDFStrategy::LiveStrategy);
        if (query_model->rowCount() != 500) {
                QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(500).arg(query_model->rowCount())));
        }
    }
}

/* Time taken for querying 500 images and videos */
void GalleryBenchmark::test_gallery_03()
{
    /*
    SELECT ?media
    WHERE {
        ?media a nfo:Visual;
        nao:hasTag ?tag .
        ?tag nao:prefLabel 'TEST'
    }
    */
    QBENCHMARK {
        RDFSelect query;
        RDFVariable visual = RDFVariable::fromType<nfo::Visual>();
        RDFVariable tag ("tag");
        visual.property <nao::hasTag>(tag);
        tag.property <nao::prefLabel>(LiteralValue(QString("TEST")));
        query.addColumn ("media", visual);

        LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
        if (query_model->rowCount() != 5500) {
                QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(5500).arg(query_model->rowCount())));
        }
    }
}

void GalleryBenchmark::test_gallery_07()
{
    /*
    SELECT ?media
    WHERE {
        ?media a nfo:Visual;
        nao:hasTag ?tag .
        ?tag nao:prefLabel 'TEST'
    } LIMIT 500
    */
    QBENCHMARK {
        RDFSelect query;
        RDFVariable visual = RDFVariable::fromType<nfo::Visual>();
        RDFVariable tag ("tag");
        visual.property <nao::hasTag>(tag);
        tag.property <nao::prefLabel>(LiteralValue(QString("TEST")));
        query.addColumn ("media", visual);

        query.limit (500);

        LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
        if (query_model->rowCount() != 500) {
                QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(500).arg(query_model->rowCount())));
        }
    }
}

void GalleryBenchmark::test_gallery_02()
{
  /*
  SELECT ?media WHERE {
        ?media a nfo:Visual;
        nfo:device 'NOKIA'
  }
  */
  QBENCHMARK {
        RDFSelect query;
        RDFVariable visual = RDFVariable::fromType<nfo::Visual>();
        visual.property <nfo::device>(LiteralValue(QString("NOKIA")));
        query.addColumn ("media", visual);

        LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
        if (query_model->rowCount() != 1000) {
                QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(1000).arg(query_model->rowCount())));
        }
  }
}

void GalleryBenchmark::test_gallery_08()
{
  /*
  SELECT ?media WHERE {
        ?media a nfo:Visual;
        nfo:device 'NOKIA'
  } LIMIT 500
  */
  QBENCHMARK {
        RDFSelect query;
        RDFVariable visual = RDFVariable::fromType<nfo::Visual>();
        visual.property <nfo::device>(LiteralValue(QString("NOKIA")));
        query.addColumn ("media", visual);

        query.limit (500);

        LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
        if (query_model->rowCount() != 500) {
                QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(500).arg(query_model->rowCount())));
        }
  }
}

void GalleryBenchmark::test_gallery_09()
{
  /*
  SELECT ?url ?height ?width ?mime ?camera ?exposuretime ?fnumber ?focallength
  WHERE {
        ?image a nmm:Photo;
        nie:url ?url;
        nie:mimeType ?mime.
        OPTIONAL { ?image nfo:height ?height .}
        OPTIONAL { ?image nfo:width  ?width .}
        OPTIONAL { ?image nfo:device ?camera .}
        OPTIONAL { ?image nmm:exposureTime ?exposuretime .}
        OPTIONAL { ?image nmm:fnumber ?fnumber .}
        OPTIONAL { ?image nmm:focalLength ?focallength .}
  } LIMIT 10000"
  */
  QBENCHMARK {
    RDFSelect query;

    RDFVariable image = RDFVariable::fromType<nmm::Photo>();

    RDFVariable url;
    image.property <nie::url> (url);
    query.addColumn ("url", url);


    query.addColumn ("height", image.optional().property<nfo::height>());
    query.addColumn ("width", image.optional().property<nfo::width>());

    query.addColumn ("mimetype", image.property<SopranoLive::nie::mimeType>());

    query.addColumn ("camera", image.optional().property<nfo::device>());
    query.addColumn ("exposuretime", image.optional().property<nmm::exposureTime>());
    query.addColumn ("fnumber", image.optional().property<nmm::fnumber>());
    query.addColumn ("focallength", image.optional().property<nmm::focalLength>());

    query.limit (10000);

    LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
        if (query_model->rowCount() != 10000) {
                QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(10000).arg(query_model->rowCount())));
        }
  }
}

void GalleryBenchmark::test_gallery_10()
{

    /* 
    SELECT ?url ?height ?width ?mime ?camera ?exposuretime ?fnumber ?focallength
    WHERE {
         ?image a nmm:Photo;
         nie:url ?url;
         nie:mimeType ?mime.
         OPTIONAL { ?image nfo:height ?height .}
         OPTIONAL { ?image nfo:width  ?width .}
         OPTIONAL { ?image nfo:device ?camera .}
         OPTIONAL { ?image nmm:exposureTime ?exposuretime .}
         OPTIONAL { ?image nmm:fnumber ?fnumber .}
         OPTIONAL { ?image nmm:focalLength ?focallength .}
    } LIMIT 500"
    */
  QBENCHMARK {
    RDFSelect query;

    RDFVariable image = RDFVariable::fromType<nmm::Photo>();

    RDFVariable url;
    image.property <nie::url> (url);
    query.addColumn ("url", url);


    query.addColumn ("height", image.optional().property<nfo::height>());
    query.addColumn ("width", image.optional().property<nfo::width>());

    query.addColumn ("mimetype", image.property<SopranoLive::nie::mimeType>());

    query.addColumn ("camera", image.optional().property<nfo::device>());
    query.addColumn ("exposuretime", image.optional().property<nmm::exposureTime>());
    query.addColumn ("fnumber", image.optional().property<nmm::fnumber>());
    query.addColumn ("focallength", image.optional().property<nmm::focalLength>());

    query.limit (500);

    LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
        if (query_model->rowCount() != 500) {
                QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(500).arg(query_model->rowCount())));
        }
  }
}

void GalleryBenchmark::test_gallery_11()
{
   /*
   SELECT ?url ?filename ?modified ?_width ?_height
   WHERE {
        {?media a nmm:Photo.} UNION {?media a nmm:Video.}
        ?media nie:url ?url.
        ?media nfo:fileName ?filename .
        ?media nfo:fileLastModified ?modified .
        OPTIONAL    {?media nfo:width ?_width. }
        OPTIONAL   { ?media nfo:height ?_height .} }
        ORDER BY ?modified LIMIT 500"
   */
  QBENCHMARK {
    RDFSelect query;

    RDFVariable photo = RDFVariable::fromType<nmm::Photo>();
    RDFVariable video = RDFVariable::fromType<nmm::Video>();

    RDFVariable visual ("media");
    visual.unionMerge (RDFVariableList () << photo << video);

    RDFVariable url;
    visual.property <nie::url> (url);
    query.addColumn ("url", url);
    query.addColumn ("filename", visual.property<nfo::fileName>());

    RDFVariable modified = visual.property<nfo::fileLastModified>();
    query.addColumn ("modified", modified);

    query.addColumn ("height", visual.optional().property<nfo::height>());
    query.addColumn ("width", visual.optional().property<nfo::width>());

    query.limit (500);

    LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
        if (query_model->rowCount() != 500) {
                QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(500).arg(query_model->rowCount())));
        }
  }
}

void GalleryBenchmark::test_gallery_12()
{
    /*
    SELECT nie:url(?image)
           nfo:height(?image)
           nfo:width(?image)
           nie:mimeType(?image)
           nfo:device(?image)
           nmm:exposureTime(?image)
           nmm:fnumber(?image)
           nmm:focalLength(?image)
    WHERE {
        ?image a nmm:Photo .
    } limit 10000"
    */
    QBENCHMARK {
        RDFSelect query;
        RDFVariable photo = RDFVariable::fromType<nmm::Photo>("image");

        query.addColumn ("url",          photo.function <nie::url> ());
        query.addColumn ("height",       photo.function <nfo::height> ());
        query.addColumn ("width",        photo.function <nfo::width> ());
        query.addColumn ("mimeType",     photo.function <nie::mimeType> ());
        query.addColumn ("device",       photo.function <nfo::device> ());
        query.addColumn ("exposureTime", photo.function <nmm::exposureTime> ());
        query.addColumn ("fnumber",      photo.function <nmm::fnumber> ());
        query.addColumn ("focalLength",  photo.function <nmm::focalLength> ());

        query.limit (10000);

        LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
        if (query_model->rowCount() != 10000) {
                QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(10000).arg(query_model->rowCount())));
        }
    }
}

void GalleryBenchmark::test_gallery_13()
{
    /*
    SELECT nie:url(?image)
           nfo:height(?image)
           nfo:width(?image)
           nie:mimeType(?image)
           nfo:device(?image)
           nmm:exposureTime(?image)
           nmm:fnumber(?image)
           nmm:focalLength(?image)
    WHERE {
        ?image a nmm:Photo .
    } limit 500"
    */
    QBENCHMARK {
        RDFSelect query;
        RDFVariable photo = RDFVariable::fromType<nmm::Photo>("image");

        query.addColumn ("url",          photo.function <nie::url> ());
        query.addColumn ("height",       photo.function <nfo::height> ());
        query.addColumn ("width",        photo.function <nfo::width> ());
        query.addColumn ("mimeType",     photo.function <nie::mimeType> ());
        query.addColumn ("device",       photo.function <nfo::device> ());
        query.addColumn ("exposureTime", photo.function <nmm::exposureTime> ());
        query.addColumn ("fnumber",      photo.function <nmm::fnumber> ());
        query.addColumn ("focalLength",  photo.function <nmm::focalLength> ());

        query.limit (500);

        LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
        if (query_model->rowCount() != 500) {
                QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(500).arg(query_model->rowCount())));
        }
    }
}

// Main function and entry point
QTEST_MAIN(GalleryBenchmark)
