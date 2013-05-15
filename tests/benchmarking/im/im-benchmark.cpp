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

#include <im-benchmark.h>

IMBenchmark::IMBenchmark ()
{
    ;
}

void IMBenchmark::test_im_01 ()
{
        /*
        SELECT ?message ?from ?date ?content WHERE {
                ?message a nmo:IMMessage ;
                nmo:from ?from ;
                nmo:receivedDate ?date ;
                nie:plainTextContent ?content
        } LIMIT 10000
        */

        /* Querying the messages */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                RDFVariable message ("message");
                RDFVariable from ("from");
                RDFVariable date ("date");
                RDFVariable content ("content");

                message.isOfType <nmo::IMMessage> ();
                message.property <nmo::from> (from);
                message.property <nmo::receivedDate> (date);
                message.property <nie::plainTextContent> (content);

                query.addColumn (message);
                query.addColumn (from);
                query.addColumn (date);
                query.addColumn (content);
                query.limit (10000);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 10000) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(10000).arg(query_model->rowCount())));
                }
        }
}

void IMBenchmark::test_im_02 ()
{
        /* TODO - For some reason this is failing. The query is simple and looks correct. */
        /* 
        SELECT ?contact ?status WHERE {
                ?contact a  nco:IMAccount;
                nco:imPresence ?status
        } LIMIT 10000
        */

        /* Querying the status of contacts every sec */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                RDFVariable contact ("contact");
                RDFVariable status ("status");

                contact.isOfType <nco::IMAccount> ();
                contact.property <nco::imPresence> (status);

                query.addColumn (contact);
                query.addColumn (status);
                query.limit (10000);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 10000) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(10000).arg(query_model->rowCount())));
                }
        }
}

QTEST_MAIN(IMBenchmark)
