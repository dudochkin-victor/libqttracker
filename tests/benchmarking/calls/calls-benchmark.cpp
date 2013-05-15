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

#include <calls-benchmark.h>

CallsBenchmark::CallsBenchmark ()
{
    ;
}

void CallsBenchmark::test_calls_01 ()
{
        /*
        SELECT ?duration ?phonenumber WHERE {
                ?call  a  nmo:Call ;
                nmo:duration ?duration ;
                nmo:from [a nco:Contact ; nco:hasPhoneNumber ?phonenumber]
        }LIMIT 10000
        */

        /* Querying the duration of calls of contacts */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                RDFVariable call ("call");
                RDFVariable duration ("duration");
                RDFVariable phonenumber ("phonenumber");
                RDFVariable _a;

                call.isOfType <nmo::Call> ();
                call.property <nmo::duration> (duration);
                _a.isOfType <nco::Contact> ();
                _a.property <nco::hasPhoneNumber> (phonenumber);
                call.property <nmo::from> (_a);

                query.addColumn (duration);
                query.addColumn (phonenumber);

                query.limit (10000);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 1000) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(1000).arg(query_model->rowCount())));
                }
        }
}

void CallsBenchmark::test_calls_02 ()
{
        /*
        SELECT ?name ?date ?number ?duration
        WHERE {
                ?m a nmo:Call;
                nmo:sentDate ?date ;
                nmo:duration ?duration;
                nmo:to ?contact .
                ?contact a nco:PersonContact;
                nco:hasPhoneNumber ?number .
                OPTIONAL {
                        ?contact a nco:PersonContact ;
                        nco:nameFamily ?name
                }
                FILTER (?duration > 0) .
        } ORDER BY desc(?date) LIMIT 1000
        */

        /* Querying the dialed calls */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                RDFVariable name ("name");
                RDFVariable date ("date");
                RDFVariable number ("number");
                RDFVariable duration ("duration");
                RDFVariable contact ("contact");
                RDFVariable _m;

                _m.isOfType <nmo::Call> ();
                _m.property <nmo::sentDate> (date);
                _m.property <nmo::duration> (duration);
                _m.property <nmo::to> (contact);
                contact.isOfType <nco::PersonContact> ();
                contact.property <nco::hasPhoneNumber> (number);

                RDFVariable opt_contact = contact.optional();
                opt_contact.isOfType <nco::PersonContact> ();
                opt_contact.property <nco::nameFamily> (name);

                duration.greater (LiteralValue (0));

                query.addColumn (name);
                query.addColumn (date);
                query.addColumn (number);
                query.addColumn (duration);
                query.orderBy (date, RDFSelect::Descending);
                query.limit (1000);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 0) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(0).arg(query_model->rowCount())));
                }
        }
}

void CallsBenchmark::test_calls_03 ()
{
        /*
        SELECT ?name ?date ?number ?duration
        WHERE {
                ?m a nmo:Call;
                nmo:receivedDate ?date ;
                nmo:duration ?duration;
                nmo:from ?contact .
                ?contact a nco:PersonContact;
                nco:hasPhoneNumber ?number . 
                OPTIONAL { ?contact a nco:PersonContact ; nco:nameFamily ?name} 
                FILTER (?duration > 0) .}
        ORDER BY desc(?date) LIMIT 1000
        */

        /* Querying the received calls */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                RDFVariable name ("name");
                RDFVariable date ("date");
                RDFVariable number ("number");
                RDFVariable duration ("duration");
                RDFVariable contact ("contact");
                RDFVariable _m;

                _m.isOfType <nmo::Call> ();
                _m.property <nmo::receivedDate> (date);
                _m.property <nmo::duration> (duration);
                _m.property <nmo::to> (contact);
                contact.isOfType <nco::PersonContact> ();
                contact.property <nco::hasPhoneNumber> (number);

                RDFVariable opt_contact = contact.optional();
                opt_contact.isOfType <nco::PersonContact> ();
                opt_contact.property <nco::nameFamily> (name);

                duration.greater (LiteralValue (0));

                query.addColumn (name);
                query.addColumn (date);
                query.addColumn (number);
                query.addColumn (duration);
                query.orderBy (date, RDFSelect::Descending);
                query.limit (1000);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 0) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(0).arg(query_model->rowCount())));
                }
        }
}

void CallsBenchmark::test_calls_04 ()
{
        /*
        SELECT ?name ?date ?number ?duration
                WHERE {?m a nmo:Call;
                nmo:receivedDate ?date ;
                nmo:duration ?duration;
                nmo:from ?contact .
                ?contact a nco:PersonContact;
                nco:hasPhoneNumber ?number .
                OPTIONAL { ?contact a nco:PersonContact ; nco:nameFamily ?name}
                FILTER (?duration > 0) .
        }
        ORDER BY desc(?date) LIMIT 1000
        */

        /* Querying the missed calls */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                RDFVariable name ("name");
                RDFVariable date ("date");
                RDFVariable number ("number");
                RDFVariable duration ("duration");
                RDFVariable contact ("contact");
                RDFVariable _m;

                _m.isOfType <nmo::Call> ();
                _m.property <nmo::receivedDate> (date);
                _m.property <nmo::duration> (duration);
                _m.property <nmo::to> (contact);
                contact.isOfType <nco::PersonContact> ();
                contact.property <nco::hasPhoneNumber> (number);

                RDFVariable opt_contact = contact.optional();
                opt_contact.isOfType <nco::PersonContact> ();
                opt_contact.property <nco::nameFamily> (name);

                duration.greater (LiteralValue (0));

                query.addColumn (name);
                query.addColumn (date);
                query.addColumn (number);
                query.addColumn (duration);
                query.orderBy (date, RDFSelect::Descending);
                query.limit (1000);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 0) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(0).arg(query_model->rowCount())));
                }
        }
}

QTEST_MAIN(CallsBenchmark)
