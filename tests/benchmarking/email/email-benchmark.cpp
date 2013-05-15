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

#include <email-benchmark.h>

EmailBenchmark::EmailBenchmark ()
{
    ;
}

void EmailBenchmark::test_email_01 ()
{
        /*
        SELECT ?m ?From  ?date ?email1 WHERE { \
               ?m a  nmo:Email ; \
               nmo:receivedDate ?date ;\
               nmo:from ?From . 
               ?from nco:hasEmailAddress ?email1
        } LIMIT 10000"
        */

        /* Querying for emails */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                RDFVariable m ("m");
                RDFVariable From ("From");
                RDFVariable date ("date");
                RDFVariable email1 ("email1");

                m.isOfType <nmo::Email> ();
                m.property <nmo::receivedDate> (date);
                m.property <nmo::from> (From);
                From.property <nco::hasEmailAddress> (email1);

                query.addColumn (m);
                query.addColumn (From);
                query.addColumn (date);
                query.addColumn (email1);

                query.limit (10000);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 10000) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(10000).arg(query_model->rowCount())));
                }
        }
}

QTEST_MAIN(EmailBenchmark)
