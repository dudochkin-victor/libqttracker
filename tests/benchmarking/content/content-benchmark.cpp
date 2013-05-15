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

#include <content-benchmark.h>

ContentBenchmark::ContentBenchmark ()
{
}

void ContentBenchmark::test_cm_01 ()
{
        /*Get all the contacts that match fts and get relevant UI info for them*/

        /*SELECT DISTINCT ?url ?photourl ?imstatus tracker:coalesce(?family, ?given, ?orgname, ?nick, ?email, ?phone, ?blog)
        WHERE {
        { ?url a nco:PersonContact.?url fts:match 'fami*'. }
        UNION
        { ?url a nco:PersonContact. ?url nco:hasEmailAddress ?add.?add fts:match 'fami*'. }
        UNION
        { ?url a nco:PersonContact. ?url nco:hasPostalAddress ?post.?post fts:match 'fami*'. }
        OPTIONAL { ?url maemo:relevance ?relevance. }
        OPTIONAL { ?url nco:photo ?photo. ?photo nie:url ?photourl. }
        OPTIONAL { ?url nco:imContactStatusMessage ?imstatus. }
        OPTIONAL { ?url nco:nameFamily ?family. }
        OPTIONAL { ?url nco:nameFamily ?given. }
        OPTIONAL { ?url nco:org ?org. ?org nco:nameGiven ?orgname. }
        OPTIONAL { ?url nco:hasIMAccount ?acc. ?acc nco:imNickname ?nick. }
        OPTIONAL { ?url nco:hasEmailAddress ?hasemail. ?hasemail nco:emailAddress ?email. }
        OPTIONAL { ?url nco:hasPhoneNumber ?hasphone. ?hasphone nco:phoneNumber ?phone. }
        OPTIONAL { ?url nco:blogUrl ?blog. }
        }
        ORDER BY ?relevance
        LIMIT 100*/
        ::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                query.distinct (true);

                RDFVariable url ("url");
                RDFVariable photourl ("photourl");
                RDFVariable imstatus ("imstatus");
                RDFVariable family ("family");
                RDFVariable given ("given");
                RDFVariable orgname ("orgname");
                RDFVariable nick ("nick");
                RDFVariable email ("email");
                RDFVariable phone ("phone");
                RDFVariable blog ("blog");
                RDFVariable relevance ("relevance");

                RDFVariableList alternatives = url.unionChildren (3);

                /* ---- */

                RDFVariable ua_url = alternatives [0];

                ua_url.isOfType <nco::PersonContact> ();
                ua_url.property (fts::iri ("match")).merge (LiteralValue ("fami*"));

                /* ---- */

                RDFVariable ub_url = alternatives [1];
                RDFVariable ub_email ("email");

                ub_url.isOfType <nco::PersonContact> ();
                ub_url.property <nco::hasEmailAddress> (ub_email);
                ub_email.property (fts::iri ("match")).merge (LiteralValue ("fami*"));

                /* ---- */

                RDFVariable uc_url = alternatives [2];
                RDFVariable uc_post ("post");

                uc_url.isOfType <nco::PersonContact> ();
                uc_url.property <nco::hasPostalAddress> (uc_post);
                uc_post.property (fts::iri ("match")).merge (LiteralValue ("fami*"));

                /* ---- */

                RDFVariable photo ("photo");
                RDFVariable org ("org");
                RDFVariable account ("account");
                RDFVariable hasemail ("hasemail");
                RDFVariable hasphone ("hasphone");

                url.optional().property <nco::photo> (photo); 
                photo.property <nie::url> (photourl);

                url.optional().property <nco::imContactStatusMessage> (imstatus); 
                url.optional().property <nco::nameFamily> (family); 
                url.optional().property <nco::nameGiven> (given); 

                url.optional().property <nco::org> (org); 
                org.property <nco::nameGiven> (orgname);

                url.optional().property <nco::hasIMAccount> (account); 
                account.property <nco::imNickname> (nick);

                url.optional().property <nco::hasEmailAddress> (hasemail); 
                hasemail.property <nco::emailAddress> (email);

                url.optional().property <nco::hasPhoneNumber> (hasphone); 
                hasphone.property <nco::phoneNumber> (phone);

                url.optional().property <nco::blogUrl> (blog); 
                url.optional().property <maemo::relevance> (relevance); 

                /* ---- */

                query.addColumn (url);
                query.addColumn (photourl);
                query.addColumn (imstatus);
                RDFFilter coalesce = family.filter ("tracker:coalesce", RDFVariableList () <<
                                                    given <<
                                                    orgname <<
                                                    nick <<
                                                    email <<
                                                    phone <<
                                                    blog);
                query.addColumn (coalesce);

                query.orderBy (relevance);
                query.limit (100);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 100) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(100).arg(query_model->rowCount())));
                }
        }
}

void ContentBenchmark::test_cm_02 ()
{
        /*Get all the contacts that match fts and get relevant UI info for them*/

        /*SELECT DISTINCT ?url tracker:coalesce(nco:nameFamily(?url), nco:nameGiven(?url), 'unknown')
        WHERE {
        { ?url a nco:PersonContact.?url fts:match 'fami*'. }
        UNION { ?url a nco:PersonContact. ?url nco:hasEmailAddress ?add.?add fts:match 'fami*'. }
        UNION { ?url a nco:PersonContact. ?url nco:hasPostalAddress ?post.?post fts:match 'fami*'. }
        } LIMIT 100*/
        ::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                query.distinct (true);

                RDFVariable url ("url");

                RDFVariableList alternatives = url.unionChildren (3);

                /* ---- */

                RDFVariable ua_url = alternatives [0];

                ua_url.isOfType <nco::PersonContact> ();
                ua_url.property (fts::iri ("match")).merge (LiteralValue ("fami*"));

                /* ---- */

                RDFVariable ub_url = alternatives [1];
                RDFVariable ub_email ("email");

                ub_url.isOfType <nco::PersonContact> ();
                ub_url.property <nco::hasEmailAddress> (ub_email);
                ub_email.property (fts::iri ("match")).merge (LiteralValue ("fami*"));

                /* ---- */

                RDFVariable uc_url = alternatives [2];
                RDFVariable uc_post ("post");

                uc_url.isOfType <nco::PersonContact> ();
                uc_url.property <nco::hasPostalAddress> (uc_post);
                uc_post.property (fts::iri ("match")).merge (LiteralValue ("fami*"));

                /* ---- */

                query.addColumn (url);
                RDFVariable nameFamily = url.function <nco::nameFamily> ();
                RDFVariable nameGiven = url.function <nco::nameGiven> ();
                RDFFilter coalesce = nameFamily.filter ("tracker:coalesce", RDFVariableList () << nameGiven << LiteralValue (QString ("unknown")));
                query.addColumn (coalesce);

                query.limit (100);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 100) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(100).arg(query_model->rowCount())));
                }
        }
}

void ContentBenchmark::test_cm_03 ()
{
        /*Get all the messages */

        /*SELECT DISTINCT ?url nie:title(?url)
        WHERE {
        { ?url a nmo:Message. ?url fts:match 'fami*'. }
        UNION { ?url a nmo:Message. ?url nmo:from ?from . ?from fts:match 'fami*'. }
        UNION { ?url a nmo:Message. ?url nmo:recipient ?to . ?to fts:match 'fami*'. }
        } LIMIT 100*/
        ::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                query.distinct (true);

                RDFVariable url ("url");

                RDFVariableList alternatives = url.unionChildren (3); 

                /* ---- */

                RDFVariable ua_url = alternatives [0];
                ua_url.isOfType <nmo::Message> ();
                ua_url.property (fts::iri ("match")).merge (LiteralValue ("fami*"));

                /* ---- */

                RDFVariable ub_url = alternatives [1];
                RDFVariable ub_from ("from");
                ub_url.isOfType <nmo::Message> ();
                ub_url.property <nmo::from> (ub_from);
                ub_from.property (fts::iri ("match")).merge (LiteralValue ("fami*"));

                /* ---- */

                RDFVariable uc_url = alternatives [2];
                RDFVariable uc_to ("to");
                uc_url.isOfType <nmo::Message> ();
                uc_url.property <nmo::to> (uc_to);
                uc_to.property (fts::iri ("match")).merge (LiteralValue ("fami*"));

                /* ---- */

                query.addColumn (url);
                query.addColumn (url.function <nie::title> ());

                query.limit (100);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 100) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(100).arg(query_model->rowCount())));
                }
        }
}

void ContentBenchmark::test_cm_04 ()
{
        /*Get all the messages */

        /*SELECT ?url ?fileLastModified ?relevance ?fileName ?mimeType ?url2
                WHERE {
                ?url a nfo:Image .
                ?url nfo:fileLastModified ?fileLastModified.
                ?url nfo:fileName ?fileName .
                ?url nie:mimeType ?mimeType .
                ?url nie:url ?url2 .
                OPTIONAL { ?url maemo:relevance ?relevance. }
                } ORDER BY ?_fileName*/
        ::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;

                RDFVariable url ("url");
                RDFVariable fileLastModified ("fileLastModified");
                RDFVariable relevance ("relevance");
                RDFVariable fileName ("fileName");
                RDFVariable mimeType ("mimeType");
                RDFVariable url2 ("url2");

                url.isOfType <nfo::Image> ();
                url.property <nfo::fileLastModified> (fileLastModified);
                url.property <nfo::fileName> (fileName);
                url.property <nie::mimeType> (mimeType);
                url.property <nie::url> (url2);
                url.optional().property <maemo::relevance> (relevance);

                query.addColumn (url);
                query.addColumn (fileLastModified);
                query.addColumn (relevance);
                query.addColumn (fileName);
                query.addColumn (mimeType);
                query.addColumn (url2);

                query.orderBy (fileName);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 10000) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(10000).arg(query_model->rowCount())));
                }
        }
}

void ContentBenchmark::test_cm_05 ()
{
        /*Get all the matching data */
        ::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                query.distinct (true);

                RDFVariable glob_url ("glob_url");

                RDFVariableList alternatives = glob_url.unionChildren (2);

                /* ---- */

                RDFSubSelect ua_query;
                RDFVariable ua_url = ua_query.variable (alternatives[0]);

                ua_url.isOfType <nmo::Message> ();
                ua_url.property (fts::iri ("match")).merge (LiteralValue ("fami*"));

                ua_query.addColumn (ua_url);

                /* ---- */

                RDFSubSelect ub_query;
                RDFVariable ub_url = ub_query.variable (alternatives[1]);
                RDFVariable ub_from ("from");

                ub_url.isOfType <nmo::Message> ();
                ub_url.property <nmo::from> (ub_from);
                ub_from.property (fts::iri ("match")).merge (LiteralValue ("fami*"));

                ub_query.addColumn (ub_url);

                /* ---- */

                query.addColumn (glob_url);
                query.limit (100);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
        /*SELECT DISTINCT ?glob_url
                WHERE
                {
                  { SELECT ?url as ?glob_url
                    WHERE { ?url a nmo:Message .
                     ?url fts:match 'fami*' .
                     ?url nmo:from ?from . } }
                  UNION
                  { SELECT ?url as ?glob_url
                    WHERE { ?url a nmo:Message .
                                     ?url nmo:from ?from .
                                     ?from fts:match 'fami*'. } }
                  UNION
                  { SELECT ?url as ?glob_url
                    WHERE { ?url a nmo:Message .
                     ?url nmo:to ?to .
                     ?to fts:match 'fami*' . } }
                  UNION
                  { SELECT ?url as ?glob_url
                    WHERE { ?url a nmo:Message.
                     ?url nmo:communicationChannel ?cha .
                     ?cha fts:match 'fami*'. } }
                  UNION
                  { SELECT ?url as ?glob_url
                    WHERE { ?url a nco:PersonContact .
                     ?url fts:match 'fami*'. } }
                  UNION
                  { SELECT ?url as ?glob_url
                    WHERE { ?url a nco:PersonContact .
                     ?url nco:hasEmailAddress ?email .
                     ?email fts:match 'fami*'. } }
                  UNION
                  { SELECT ?url as ?glob_url
                    WHERE { ?url a nco:PersonContact .
                     ?url nco:hasPostalAddress ?post .
                     ?post fts:match 'fami*'. } }
                  UNION
                  { SELECT ?url as ?glob_url
                    WHERE { ?url a nmm:MusicPiece .
                     ?url nmm:performer ?artist .
                     ?artist fts:match 'fami*' . } }
                  UNION
                  { SELECT ?url as ?glob_url
                    WHERE { ?url a nmm:MusicPiece .
                     ?url nmm:musicAlbum ?album .
                     ?album fts:match 'fami*' . } }
                  }
                LIMIT 100*/
}

void ContentBenchmark::test_cm_06 ()
{
        /*Get all the matching data */

        /*SELECT DISTINCT ?glob_url ?first ?second
                WHERE
                {
                  { SELECT ?url as ?glob_url
                    nmo:messageSubject(?url) as ?first
                    tracker:coalesce(nco:fullname(?from), nco:nameGiven(?from), nco:nameFamily(?from), nco:org(?from),'unknown') as ?second
                    WHERE { ?url a nmo:Message .
                     ?url fts:match 'fami*' .
                     ?url nmo:from ?from . } }
                  UNION
                  { SELECT ?url as ?glob_url
                    nmo:messageSubject(?url) as ?first
                    tracker:coalesce(nco:fullname(?from), nco:nameGiven(?from), nco:nameFamily(?from), nco:org(?from),'unknown') as ?second
                    WHERE { ?url a nmo:Message .
                                     ?url nmo:from ?from .
                                     ?from fts:match 'fami*'. } }
                  UNION
                  { SELECT ?url as ?glob_url
                    nmo:messageSubject(?url) as ?first
                    tracker:coalesce(nco:fullname(?from), nco:nameGiven(?from), nco:nameFamily(?from), nco:org(?from),'unknown') as ?second
                    WHERE { ?url a nmo:Message .
                     ?url nmo:to ?to .
                     ?to fts:match 'fami*' .
                     ?url nmo:from ?from . } }
                  UNION
                  { SELECT ?url as ?glob_url
                    nmo:messageSubject(?url) as ?first
                    tracker:coalesce(nco:fullname(?from), nco:nameGiven(?from), nco:nameFamily(?from), nco:org(?from),'unknown') as ?second
                    WHERE { ?url a nmo:Message.
                     ?url nmo:communicationChannel ?cha .
                     ?cha fts:match 'fami*'.
                     ?url nmo:from ?from . } }
                  UNION
                  { SELECT ?url as ?glob_url
                    tracker:coalesce(nco:fullname(?url), nco:nameGiven(?url), nco:nameFamily(?url), nco:org(?url),'unknown') as ?first
                    tracker:coalesce(nco:emailAddress(?email), nco:imNickname(?im), 'unknown') as ?second
                    WHERE { ?url a nco:PersonContact .
                     ?url fts:match 'fami*'.
                     { SELECT ?em as ?email WHERE { ?url nco:hasEmailAddress ?em }  LIMIT 1 }
                     { SELECT ?imadd as ?im WHERE { ?url nco:hasIMAddress ?imadd }  LIMIT 1 } } }
                  UNION
                  { SELECT ?url as ?glob_url
                    tracker:coalesce(nco:fullname(?url), nco:nameGiven(?url), nco:nameFamily(?url), nco:org(?url),'unknown') as ?first
                    tracker:coalesce(nco:emailAddress(?email), nco:imNickname(?im), 'unknown') as ?second
                    WHERE { ?url a nco:PersonContact .
                     ?url nco:hasEmailAddress ?email .
                     ?email fts:match 'fami*'.
                     { SELECT ?imadd as ?im WHERE { ?url nco:hasIMAddress ?imadd }  LIMIT 1 } } }
                  UNION
                  { SELECT ?url as ?glob_url
                    tracker:coalesce(nco:fullname(?url), nco:nameGiven(?url), nco:nameFamily(?url), nco:org(?url),'unknown') as ?first
                    tracker:coalesce(nco:emailAddress(?email), nco:imNickname(?im), 'unknown') as ?second
                    WHERE { ?url a nco:PersonContact .
                     ?url nco:hasPostalAddress ?post .
                     ?post fts:match 'fami*'.
                     { SELECT ?em as ?email WHERE { ?url nco:hasEmailAddress ?em }  LIMIT 1 }
                     { SELECT ?imadd as ?im WHERE { ?url nco:hasIMAddress ?imadd }  LIMIT 1 } } }
                  UNION
                  { SELECT ?url as ?glob_url
                    nie:title(?url) as ?first
                    fn:concat(nmm:artistName(?artist),'-',nmm:albumTitle(?album))
                    WHERE { ?url a nmm:MusicPiece .
                     ?url nmm:performer ?artist .
                     ?artist fts:match 'fami*' .
                     OPTIONAL { ?url nmm:musicAlbum ?album . } } }
                  UNION
                  { SELECT ?url as ?glob_url
                    nie:title(?url) as ?first
                    fn:concat(nmm:artistName(?artist),'-',nmm:albumTitle(?album))
                    WHERE { ?url a nmm:MusicPiece .
                     ?url nmm:musicAlbum ?album .
                     ?album fts:match 'fami*' .
                     OPTIONAL { ?url nmm:performer ?artist }} }
                  }
                LIMIT 100*/
        QBENCHMARK {
                RDFSelect query;

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}

QTEST_MAIN(ContentBenchmark)
