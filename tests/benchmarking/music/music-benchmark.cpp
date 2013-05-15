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

#include <music-benchmark.h>

MusicBenchmark::MusicBenchmark ()
{
    ;
}

void MusicBenchmark::test_audio_01 ()
{
        /*
        query = "SELECT ?title ?artist nmm:albumTitle (nmm:musicAlbum (?song))
        WHERE { {
                ?song a nmm:MusicPiece .
                ?song nie:title ?title .
                ?song nmm:performer ?perf .
                ?perf nmm:artistName ?artist .
                }
        }
        ORDER BY ?title
        */
        /*
        SELECT ?title ?artist nmm:albumTitle (nmm:musicAlbum (?song))\
        WHERE { { \
                        ?song a nmm:MusicPiece . \
                        ?song nie:title ?title .\
                        ?song nmm:performer ?perf . \
                        ?perf nmm:artistName ?artist .
        }}  \
                        ORDER BY ?title "

         */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;

                RDFVariable title ("title");
                RDFVariable artist ("artist");
                RDFVariable song ("song");
                RDFVariable perf ("perf");

                song.isOfType <nmm::MusicPiece> ();
                song.property <nie::title> (title);
                song.property <nmm::performer> (perf);
                perf.property <nmm::artistName> (artist);

                query.addColumn (title);
                query.addColumn (artist);
                query.addColumn (song.function <nmm::musicAlbum> ().function <nmm::albumTitle> ());

                query.orderBy (title);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 10000) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(10000).arg(query_model->rowCount())));
                }
        }
}

void MusicBenchmark::test_audio_02 ()
{
        /*
        SELECT nmm:albumTitle(?album) AS ?Album  nmm:artistName (nmm:performer (?Songs)) COUNT(?Songs)  AS ?Songs  ?album \
        WHERE { 
                { 
                ?Songs a nmm:MusicPiece .
                ?Songs nmm:musicAlbum ?album .
                }
        }GROUP BY ?album ORDER BY ?album LIMIT 5000
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;

                RDFVariable album ("album");
                RDFVariable Songs ("Songs");

                Songs.isOfType <nmm::MusicPiece> ();
                Songs.property <nmm::musicAlbum> (album);

                query.addColumn ("Album", album.function <nmm::albumTitle> ());
                query.addColumn (Songs.function <nmm::performer> ().function <nmm::artistName> ());
                query.addCountColumn ("Songs", Songs);
                query.addColumn (album);

                query.groupBy (album);
                query.orderBy (album);
                query.limit (5000);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 1000) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(1000).arg(query_model->rowCount())));
                }
        }
}

void MusicBenchmark::test_audio_03 ()
{
        /*
        SELECT nmm:artistName(?artist) nmm:albumTitle(?album) COUNT(?album) ?artist WHERE {
                ?song a nmm:MusicPiece .
                ?song nmm:performer ?artist .
                OPTIONAL { ?song nmm:musicAlbum ?album . } 
        }
        GROUP BY ?artist ORDER BY ?artist LIMIT 5000
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;

                RDFVariable artist ("artist");
                RDFVariable album ("album");
                RDFVariable song ("song");

                song.isOfType <nmm::MusicPiece> ();
                song.property <nmm::performer> (artist);
                song.optional().property <nmm::musicAlbum> (album);

                query.addColumn (artist.function <nmm::artistName> ());
                query.addColumn (album.function <nmm::albumTitle> ());
                query.addCountColumn (album);
                query.addColumn (artist);
                query.orderBy (artist);
                query.limit (5000);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 1) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(1).arg(query_model->rowCount())));
                }
        }
}

void MusicBenchmark::test_audio_04 ()
{
        /*
        SELECT ?artist ?name COUNT(?album) COUNT (?song)
        WHERE {
                      ?song a nmm:MusicPiece ;
                      nmm:musicAlbum ?album;
                      nmm:performer ?artist .
                      ?artist nmm:artistName ?name.
        } GROUP BY ?artist
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;

                RDFVariable artist ("artist");
                RDFVariable name ("name");
                RDFVariable album ("album");
                RDFVariable song ("song");

                song.isOfType <nmm::MusicPiece> ();
                song.property <nmm::musicAlbum> (album);
                song.property <nmm::performer> (artist);
                artist.property <nmm::artistName> (name);

                query.addColumn (artist);
                query.addColumn (name);
                query.addCountColumn (album);
                query.addCountColumn (song);

                query.groupBy (artist);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}

void MusicBenchmark::test_audio_05 ()
{
        /*
        SELECT nmm:artistName(?artist) COUNT(?songs) WHERE {
                ?mp a nmm:MusicPiece .
                ?mp nmm:performer ?artist ;
                nie:title ?songs .
        }
        GROUP BY ?artist ORDER BY DESC(nmm:artistName(?artist))
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;

                RDFVariable artist ("artist");
                RDFVariable songs ("songs");
                RDFVariable mp ("mp");

                mp.isOfType <nmm::MusicPiece> ();
                mp.property <nmm::performer> (artist);
                mp.property <nie::title> (songs);

                query.addColumn (artist.function <nmm::artistName> ());
                query.addCountColumn (songs);

                query.groupBy (artist);
                query.orderBy (artist.function <nmm::artistName> (), RDFSelect::Descending);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}

void MusicBenchmark::test_audio_06 ()
{
        /*
        SELECT nie:title(?a) COUNT(?songs) WHERE {
                ?a a nmm:MusicAlbum .
                ?mp nmm:musicAlbum ?a ;
                nie:title ?songs .
        }
        GROUP BY ?a ORDER BY DESC(nie:title(?a))
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;

                RDFVariable a ("a");
                RDFVariable songs ("songs");
                RDFVariable mp ("mp");

                a.isOfType <nmm::MusicAlbum> ();
                mp.property <nmm::musicAlbum> (a);
                mp.property <nie::title> (songs);

                query.addColumn (a.function <nie::title> ());
                query.addCountColumn (songs);

                query.groupBy (a);
                query.orderBy (a.function <nie::title> (), RDFSelect::Descending);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 1000) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(1000).arg(query_model->rowCount())));
                }
        }
}

void MusicBenchmark::test_audio_07 ()
{
        /*
        SELECT  ?album COUNT(?songs) AS ?count  WHERE {
                ?a a nmm:MusicAlbum;
                nie:title ?album.
                ?mp nmm:musicAlbum ?a;
                nie:title ?songs.
        } GROUP BY ?album ORDER BY DESC(?album)
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                RDFVariable album ("album");
                RDFVariable songs ("songs");
                RDFVariable a ("a");
                RDFVariable mp ("mp");

                a.isOfType <nmm::MusicAlbum> ();
                a.property <nie::title> (album);
                mp.property <nmm::musicAlbum> (a);
                mp.property <nie::title> (songs);

                query.addColumn (album);
                query.addCountColumn (songs);

                query.groupBy (album);
                query.orderBy (album, RDFSelect::Descending);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 100) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(100).arg(query_model->rowCount())));
                }
        }
}

void MusicBenchmark::test_audio_08 ()
{
        /*
        SELECT nmm:artistName(?artist) AS ?artistTitle (nmm:musicAlbum (?song)) (nmm:albumTitle (?album))COUNT(?album) AS ?album ?artist
        WHERE { 
                ?song a nmm:MusicPiece  .
                ?song nmm:performer ?artist .
        } GROUP BY ?artist  ORDER BY ?artist LIMIT 5000
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                RDFVariable artist ("artist");
                RDFVariable song ("song");

                song.isOfType <nmm::MusicPiece> ();
                song.property <nmm::performer> (artist);

                query.addColumn ("artistTitle", artist.function <nmm::artistName> ());
                query.addColumn (song.function <nmm::musicAlbum> ());
                query.addColumn (artist);

                query.groupBy (artist);
                query.orderBy (artist);
                query.limit (5000);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}

void MusicBenchmark::test_audio_09 ()
{
        /*
        SELECT nmm:albumTitle(?album) AS ?Album  (nmm:performer(?Songs)) nmm:artistName COUNT(?Songs)  AS ?Songs  ?album
        WHERE {
                {
                ?Songs a nmm:MusicPiece .
                ?Songs nmm:musicAlbum ?album .
                }
        }GROUP BY ?album ORDER BY ?album LIMIT 100
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;

                RDFVariable album ("album");
                RDFVariable Songs ("Songs");

                Songs.isOfType <nmm::MusicPiece> ();
                Songs.property <nmm::musicAlbum> (album);

                query.addColumn ("Album", album.function <nmm::albumTitle> ());
                query.addColumn (Songs.function <nmm::performer> ());
                query.addCountColumn ("Songs", Songs);
                query.addColumn (album);

                query.groupBy (album);
                query.orderBy (album);
                query.limit (100);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 100) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(100).arg(query_model->rowCount())));
                }
        }
}

void MusicBenchmark::test_audio_10 ()
{
        /*
        SELECT nmm:artistName(?artist) AS ?artistTitle (nmm:musicAlbum (?song)) nmm:albumTitle COUNT(?album) AS
                           ?album ?artist
        WHERE { 
                ?song a nmm:MusicPiece  .
                ?song nmm:performer ?artist .
        } GROUP BY ?artist  ORDER BY ?artist  LIMIT 100
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                RDFVariable artist ("artist");
                RDFVariable song ("song");
                RDFVariable album ("album");

                song.isOfType <nmm::MusicPiece> ();
                song.property <nmm::performer> (artist);

                query.addColumn ("artistTitle", artist.function <nmm::artistName> ());
                query.addColumn (song.function <nmm::musicAlbum> ());
                query.addCountColumn (album);
                query.addColumn (artist);

                query.groupBy (artist);
                query.orderBy (artist);
                query.limit (100);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}

void MusicBenchmark::test_audio_11 ()
{
        /*
        SELECT nmm:artistName(?artist) nmm:albumTitle(?album) COUNT(?album) ?artist WHERE {
                ?song a nmm:MusicPiece .
                ?song nmm:performer ?artist .
                OPTIONAL  { ?song nmm:musicAlbum ?album . }
        }
        GROUP BY ?artist ORDER BY ?artist LIMIT 100
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;

                RDFVariable artist ("artist");
                RDFVariable album ("album");
                RDFVariable song ("song");

                song.isOfType <nmm::MusicPiece> ();
                song.property <nmm::performer> (artist);
                song.optional().property <nmm::musicAlbum> (album);

                query.addColumn (artist.function <nmm::artistName> ());
                query.addColumn (album.function <nmm::albumTitle> ());
                query.addCountColumn (album);
                query.addColumn (artist);

                query.groupBy (artist);
                query.orderBy (artist);
                query.limit (100);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}

QTEST_MAIN(MusicBenchmark)
