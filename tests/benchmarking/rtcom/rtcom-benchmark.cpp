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

#include <rtcom-benchmark.h>

RTComBenchmark::RTComBenchmark ()
{
    ;
}

void RTComBenchmark::test_rtcom_04 ()
{
        /* 
         * Complete select
         *
         * SELECT ?channel
         *        ?participant
         *        nco:fullname(?participant)
         *        ?last_date
         *        nie:plainTextContent(?last_message)
         *        (
         *              SELECT COUNT(?message) AS message_count WHERE {
         *                      ?message nmo:communicationChannel ?channel
         *              }
         *        )
         *        (
         *              SELECT COUNT(?message) AS message_count WHERE {
         *                      ?message nmo:communicationChannel ?channel .
         *                      ?message nmo:isRead true
         *              }
         *        )
         *        WHERE {
         *              SELECT ?channel
         *                     ?participant
         *                     ?last_date
         *                     (
         *                              SELECT ?message WHERE {
         *                                      ?message nmo:communicationChannel ?channel .
         *                                      ?message nmo:sentDate ?date .
         *                                      } ORDER BY DESC (?date) LIMIT 1
         *                     ) AS ?last_message
         *              WHERE {
         *                      ?channel a nmo:CommunicationChannel .
         *                      ?channel nmo:lastMessageDate ?last_date .
         *                      ?channel nmo:hasParticipant ?participant .
         *                      FILTER (?participant != nco:default-contact_me)
         *              }
         *        }
         */
        /*
 "
SELECT ?_channel ?_participant 
       <http://www.semanticdesktop.org/ontologies/2007/03/22/nco#fullname>(?_participant) AS ?_fullname 
       ?_last_date 
       ( SELECT COUNT(?_message_1)
         AS ?_message_count_1 
         WHERE {
            ?_message_1 <http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#communicationChannel> ?_channel .
         }) AS ?_all_message_count 
       ( SELECT 
         COUNT(?_message)
         AS ?_message_count 
         WHERE {
             ?_message <http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#communicationChannel> ?_channel .
             ?_message <http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#isRead> "true"^^<http://www.w3.org/2001/XMLSchema#string> .
       }) AS ?_read_message_count 
     <http://www.semanticdesktop.org/ontologies/2007/01/19/nie#plainTextContent>(?_last_message_1) 
  AS ?_plain_text_content 
WHERE
{
  {
    SELECT ?_channel ?_participant ?_last_date 
         ( SELECT ?_last_message 
  WHERE {
  
    ?_last_message <http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#communicationChannel> ?_channel .
    ?_last_message <http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#sentDate> ?_date .
  }
    ORDER BY DESC(?_date)
    LIMIT 1)
 
      AS ?_last_message_1  ?_last_message
    WHERE
    {
      ?_channel <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#CommunicationChannel> .
      ?_channel <http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#lastMessageDate> ?_last_date .
      ?_channel <http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#hasParticipant> ?_participant .
      FILTER((?_participant != <http://www.semanticdesktop.org/ontologies/2007/03/22/nco#default-contact-me>)) .
    }
  }
}"
*/
  //  ::tracker()->setVerbosity (2);
    QBENCHMARK {

        RDFSelect query;

        RDFVariable channel = query.newColumn ("channel");
        RDFVariable participant = query.newColumn ("participant");
        query.addColumn ("fullname", participant.function <nco::fullname> ());
        RDFVariable last_date = query.newColumn ("last_date");
        RDFVariable last_message ("last_message");

                /* ------------------------ */

                RDFSubSelect all_msg_query;

                RDFVariable all_msg_message ("message");
                RDFVariable all_msg_channel = all_msg_query.variable (channel);
                all_msg_message.property <nmo::communicationChannel> (all_msg_channel);
                all_msg_query.addCountColumn ("message_count", all_msg_message);

                /* ------------------------ */

        query.addColumn ("all_message_count", all_msg_query.asExpression());

                /* ------------------------ */

                RDFSubSelect read_msg_query;

                RDFVariable read_msg_message ("message");
                RDFVariable read_msg_channel = read_msg_query.variable (channel);
                read_msg_message.property <nmo::communicationChannel> (read_msg_channel);
                read_msg_message.property <nmo::isRead> (LiteralValue ("true"));
                read_msg_query.addCountColumn ("message_count", read_msg_message);

                /* ------------------------ */

        query.addColumn ("read_message_count", read_msg_query.asExpression());

                /* ------------------------ */

                RDFSubSelect inner; 

                RDFVariable inner_channel = inner.newColumnAs (channel);
                RDFVariable inner_participant = inner.newColumnAs (participant);
                RDFVariable inner_last_date = inner.newColumnAs (last_date);
                RDFVariable inner_last_message = inner.variable (last_message);

                inner_channel.isOfType <nmo::CommunicationChannel>();
                inner_channel.property <nmo::lastMessageDate> (inner_last_date);
                inner_channel.property <nmo::hasParticipant> (inner_participant);
                inner_participant.notEqual(nco::default_contact_me::iri());

                        /* ------------------------ */

                        RDFSubSelect last_msg_query;

                        RDFVariable last_msg_channel = last_msg_query.variable (inner_channel);
                        RDFVariable last_msg_message = last_msg_query.variable (inner_last_message);
                        RDFVariable last_msg_date ("date");

                        last_msg_message.property <nmo::communicationChannel> (last_msg_channel);
                        last_msg_message.property <nmo::sentDate> (last_msg_date);

                        last_msg_query.addColumn ("last_message", last_msg_message);

                        last_msg_query.orderBy (last_msg_date, RDFSelect::Descending);
                        last_msg_query.limit (1);

                        /* ------------------------ */

                inner.addColumnAs (last_msg_query.asExpression(), inner_last_message);

                /* ------------------------ */

        RDFVariable plain_text_content = last_message.function<nie::plainTextContent>();
        query.addColumn("plain_text_content", plain_text_content);

        LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 200) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(200).arg(query_model->rowCount())));
                }

    }
}

void RTComBenchmark::test_rtcom_01 ()
{

    QBENCHMARK {
        /*
        SELECT ?msg ?date ?text ?contact
        WHERE {
                ?msg nmo:communicationChannel <urn:channel:1> ;
                nmo:sentDate ?date ;
                nie:plainTextContent ?text .
                <urn:channel:1> nmo:hasParticipant ?contact .
        } ORDER BY DESC(?date) LIMIT 50"
        */

        RDFSelect conversation_view;
        RDFVariable msg = conversation_view.newColumn("msg");
        RDFVariable date = conversation_view.newColumn("date");
        RDFVariable text = conversation_view.newColumn("text");
        RDFVariable contact = conversation_view.newColumn("contact");
        msg.property <nmo::communicationChannel>(QUrl("urn:channel:1"));
        msg.property <nmo::sentDate>(date);
        msg.property <nie::plainTextContent>(text);
        contact.subject <nmo::hasParticipant>(QUrl("urn:channel:1"));

        conversation_view.orderBy(date, RDFSelect::Descending);
        conversation_view.limit(50);

        /* ------------------------ */

        LiveNodes query_model = ::tracker()->modelQuery (conversation_view,
                                                         SopranoLive::RDFStrategy::LiveStrategy);

                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
    }
}

void RTComBenchmark::test_rtcom_05 ()
{
    QBENCHMARK {
        /*
        SELECT ?msg ?date ?text ?contact
        WHERE {
                ?msg nmo:communicationChannel <urn:channel:1> ;
                nmo:sentDate ?date ;
                nie:plainTextContent ?text ;
                nmo:from [ nco:hasIMAddress ?fromAddress ] .
                <urn:channel:1> nmo:hasParticipant ?contact .
                ?contact nco:hasIMAddress ?fromAddress .
        } ORDER BY DESC(?date) LIMIT 50"
        */

        /*
        SELECT ?msg ?date ?text ?contact \
        WHERE { \
                ?msg nmo:communicationChannel <urn:channel:1> ; \
                nmo:sentDate ?date ; \
                nie:plainTextContent ?text ; \
                nmo:from [ nco:hasIMAddress ?fromAddress ] . \
                <urn:channel:1> nmo:hasParticipant ?contact . \
                ?contact nco:hasIMAddress ?fromAddress . \
        } ORDER BY DESC(?date) LIMIT 50"
        */

        //::tracker()->setVerbosity(2);

        RDFSelect conversation_view;
        RDFVariable msg = conversation_view.newColumn("msg");
        RDFVariable date = conversation_view.newColumn("date");
        RDFVariable text = conversation_view.newColumn("text");
        RDFVariable contact = conversation_view.newColumn("contact");
        RDFVariable fromAddress;
        RDFVariable blank;

        msg.property <nmo::communicationChannel>(QUrl("urn:channel:1"));
        msg.property <nmo::sentDate>(date);
        msg.property <nie::plainTextContent>(text);
        blank.property <nco::hasIMAddress>(fromAddress);
        msg.property <nmo::from>(blank);

        contact.property <nco::hasIMAddress>(fromAddress);
        contact.subject <nmo::hasParticipant>(QUrl("urn:channel:1"));

        conversation_view.orderBy(date, RDFSelect::Descending);
        conversation_view.limit(50);

        /* ------------------------ */

        LiveNodes query_model = ::tracker()->modelQuery (conversation_view,
                                                         SopranoLive::RDFStrategy::LiveStrategy);

                if (query_model->rowCount() != 3) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(3).arg(query_model->rowCount())));
                }
    }
}

void RTComBenchmark::test_rtcom_02 ()
{
    QBENCHMARK {
        /*
        SELECT ?message ?date ?from ?to
             rdf:type(?message)
             tracker:coalesce(fn:concat(nco:nameGiven(?contact), ' ', nco:nameFamily(?contact)), nco:nickname(?contact))
             nco:contactUID(?contact)
             nmo:communicationChannel(?message)
             nmo:isSent(?message)
             nmo:isDraft(?message)
             nmo:isRead(?message)
             nmo:isAnswered(?message)
             nmo:isDeleted(?message)
             nmo:messageId(?message)
             nmo:smsId(?message)
             nmo:sentDate(?message)
             nmo:receivedDate(?message)
             nie:contentLastModified(?message)
             nmo:messageSubject(?message)
             nie:plainTextContent(?message)
             nmo:deliveryStatus(?message)
             nmo:reportDelivery(?message)
             nie:url(?message)
             nfo:fileName(nmo:fromVCard(?message))
             rdfs:label(nmo:fromVCard(?message))
             nfo:fileName(nmo:toVCard(?message))
             rdfs:label(nmo:toVCard(?message))
             nmo:encoding(?message)
             nie:characterSet(?message)
             nie:language(?message)
        WHERE
        {
          SELECT
             ?message ?date ?from ?to
             (SELECT ?contact
              WHERE
              {
                  {
                    <urn:channel:1> nmo:hasParticipant ?participant .
                    ?contact a nco:PersonContact .
                    ?participant nco:hasIMAddress ?imaddress .
                    ?contact nco:hasIMAddress ?imaddress .
                  }
                  UNION
                  {
                    <urn:channel:1> nmo:hasParticipant ?participant .
                    ?contact a nco:PersonContact .
                    ?participant nco:hasPhoneNumber ?participantNumber .
                    ?participantNumber maemo:localPhoneNumber ?number .
                    ?contact nco:hasPhoneNumber ?contactNumber .
                    ?contactNumber maemo:localPhoneNumber ?number .
                  }
              }
             ) AS ?contact
          WHERE
          {
            ?message a nmo:Message .
            ?message nmo:isDraft false .
            ?message nmo:isDeleted false .
            ?message nmo:sentDate ?date .
            ?message nmo:from ?fromContact .
            ?message nmo:to ?toContact .
            ?fromContact nco:hasContactMedium ?from .
            ?toContact nco:hasContactMedium ?to .
            ?message nmo:communicationChannel <urn:channel:1> .
          }
          ORDER BY DESC(?date)
        }
        LIMIT 50
        */

        /* ------------------------ */

        //::tracker()->setVerbosity(2);

        RDFSelect conversation_view;

        RDFVariable msg     = conversation_view.newColumn("msg");
        RDFVariable date    = conversation_view.newColumn("date");
        RDFVariable from    = conversation_view.newColumn("from");
        RDFVariable to      = conversation_view.newColumn("to");

        RDFVariable contact;

        conversation_view.addColumn ("type", msg.function<rdf::type>());
        RDFVariable nameGiven = contact.function <nco::nameGiven>();
        RDFVariable nameFamily = contact.function <nco::nameFamily>();
        RDFVariable nickname = contact.function <nco::nickname>();

        RDFFilter concat = nameGiven.filter("fn:concat", RDFVariableList () << LiteralValue (" ") << nameFamily);
        RDFFilter coalesce = concat.filter("tracker:coalesce", RDFVariableList () <<  nickname);

        conversation_view.addColumn ("coalesce", coalesce);
        conversation_view.addColumn ("contactUID", contact.function<nco::contactUID>());
        conversation_view.addColumn ("communicationChannel", msg.function<nmo::communicationChannel>());
        conversation_view.addColumn ("isSent", msg.function<nmo::isSent>());
        conversation_view.addColumn ("isDraft", msg.function<nmo::isDraft>());
        conversation_view.addColumn ("isRead", msg.function<nmo::isRead>());
        conversation_view.addColumn ("isAnswered", msg.function<nmo::isAnswered>());
        conversation_view.addColumn ("isDeleted", msg.function<nmo::isDeleted>());
        conversation_view.addColumn ("messageId", msg.function<nmo::messageId>());
        conversation_view.addColumn ("smsId", msg.function<nmo::smsId>());
        conversation_view.addColumn ("sentDate", msg.function<nmo::sentDate>());
        conversation_view.addColumn ("receivedDate", msg.function<nmo::receivedDate>());
        conversation_view.addColumn ("contentLastModified", msg.function<nie::contentLastModified>());
        conversation_view.addColumn ("messageSubject", msg.function<nmo::messageSubject>());
        conversation_view.addColumn ("plainTextContent", msg.function<nie::plainTextContent>());
        conversation_view.addColumn ("deliveryStatus", msg.function<nmo::deliveryStatus>());
        conversation_view.addColumn ("reportDelivery", msg.function<nmo::reportDelivery>());
        conversation_view.addColumn ("url", msg.function<nie::url>());
        conversation_view.addColumn ("fromFileName", msg.function<nmo::fromVCard>().function<nfo::fileName>());
        conversation_view.addColumn ("fromLabel", msg.function<nmo::fromVCard>().function<rdfs::label>());
        conversation_view.addColumn ("toFileName", msg.function<nmo::toVCard>().function<nfo::fileName>());
        conversation_view.addColumn ("toLabel", msg.function<nmo::toVCard>().function<rdfs::label>());
        conversation_view.addColumn ("encoding", msg.function<nmo::encoding>());
        conversation_view.addColumn ("characterSet", msg.function<nie::characterSet>());
        conversation_view.addColumn ("language", msg.function<nie::language>());

                /* ------------------------ */

                RDFSubSelect inner;

                RDFVariable inner_msg     = inner.newColumnAs (msg);
                RDFVariable inner_date    = inner.newColumnAs (date);
                RDFVariable inner_from    = inner.newColumnAs (from);
                RDFVariable inner_to      = inner.newColumnAs (to);
                RDFVariable inner_contact = inner.variable (contact);
                RDFVariable fromContact;
                RDFVariable toContact;

                inner_msg.isOfType <nmo::Message>();
                inner_msg.property <nmo::isDraft>(LiteralValue("false"));
                inner_msg.property <nmo::isDeleted>(LiteralValue("false"));
                inner_msg.property <nmo::sentDate>(inner_date);
                inner_msg.property <nmo::from>(fromContact);
                inner_msg.property <nmo::to>(toContact);

                fromContact.property <nco::hasContactMedium> (from);
                toContact.property <nco::hasContactMedium> (to);

                inner_msg.property <nmo::communicationChannel>(QUrl("urn:channel:1"));

                inner.orderBy(inner_date, RDFSelect::Descending);

                        /* ------------------------ */

                        RDFSubSelect expression;

                                /* ------------------------ */

                                RDFVariable ua_participant;
                                ua_participant.subject <nmo::hasParticipant> (QUrl("urn:channel:1"));

                                RDFVariable ua_contact = RDFVariable::fromType <nco::PersonContact>();

                                RDFVariable ua_imaddress;
                                ua_participant.property <nco::hasIMAddress> (ua_imaddress);
                                ua_contact.property <nco::hasIMAddress> (ua_imaddress);

                                /* ------------------------ */

                                RDFVariable ub_participant;
                                ub_participant.subject <nmo::hasParticipant> (QUrl("urn:channel:1"));

                                RDFVariable ub_contact = RDFVariable::fromType <nco::PersonContact>();

                                RDFVariable ub_participantNumber;
                                ub_participant.property <nco::hasPhoneNumber> (ub_participantNumber);
                                RDFVariable ub_number;
                                ub_participantNumber.property <maemo::localPhoneNumber> (ub_number);

                                RDFVariable ub_contactNumber;
                                ub_contact.property <maemo::hasPhoneNumber> (ub_contactNumber);
                                ub_contactNumber.property <maemo::localPhoneNumber> (ub_number);

                                /* ------------------------ */

                        RDFVariable expression_union;
                        expression_union.unionMerge (RDFVariableList() << ua_contact << ub_contact);

                        expression.addColumn (expression_union);

                        /* ------------------------ */

                inner.addColumnAs (expression.asExpression(), inner_contact);

                /* ------------------------ */

        conversation_view.limit(50);

        /* ------------------------ */

        LiveNodes query_model = ::tracker()->modelQuery (conversation_view,
                                                         SopranoLive::RDFStrategy::LiveStrategy);

                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
    }
}

void RTComBenchmark::test_rtcom_06 ()
{
    //TODO There is an issue with the union clause in this case.
    QBENCHMARK {
        /*
        SELECT ?channel
               ?subject
               nie:generator(?channel)
               tracker:coalesce(fn:concat(nco:nameGiven(?contact), 
                                          ' ',
                                          nco:nameFamily(?contact)),
                                          nco:nickname(?contact)) AS ?contactName 
               nco:contactUID(?contact) AS ?contactUID
               ?lastDate
               ?lastMessage nie:plainTextContent(?lastMessage)
               nfo:fileName(nmo:fromVCard(?lastMessage))
               rdfs:label(nmo:fromVCard(?lastMessage))
               ( SELECT COUNT(?message) AS ?total_messages 
                 WHERE { ?message nmo:communicationChannel ?channel . })
               ( SELECT COUNT(?message) AS ?total_unread
                 WHERE { ?message nmo:communicationChannel ?channel . ?message nmo:isRead false  .})
               ( SELECT COUNT(?message) AS ?_total_sent
                 WHERE { ?message nmo:communicationChannel ?channel . ?message nmo:isSent true . })
        WHERE {
                SELECT ?channel  ?subject  ?lastDate
                       ( SELECT ?message
                         WHERE {
                                ?message nmo:communicationChannel ?channel .
                                ?message nmo:sentDate ?sentDate .
                         } ORDER BY DESC(?sentDate) LIMIT 1
                       ) AS ?lastMessage
                       ( SELECT ?contact
                         WHERE {
                                {
                                        ?channel nmo:hasParticipant ?participant .
                                        ?contact a nco:PersonContact .
                                        ?participant nco:hasIMAddress ?imaddress .
                                        ?contact nco:hasIMAddress ?imaddress .
                                }
                                UNION
                                {
                                        ?channel nmo:hasParticipant ?participant .
                                        ?contact a nco:PersonContact .
                                        ?participant nco:hasPhoneNumber ?participantNumber .
                                        ?number maemo:localPhoneNumber ?localNumber .
                                        ?contact nco:hasPhoneNumber ?contactNumber .
                                        ?contactNumber maemo:localPhoneNumber ?localNumber .
                                }
                         }
                        ) AS ?contact
                WHERE {
                        ?channel a nmo:CommunicationChannel .
                        ?channel nie:subject ?subject .
                        ?channel nmo:lastMessageDate ?lastDate .
                }
        }
        ORDER BY DESC(?lastDate) LIMIT 50
        */

        //::tracker()->setVerbosity(2);
        
        /* ------------------------ */

        RDFSelect conversation_list;

        RDFVariable contact("contact");

        RDFVariable channel  = conversation_list.newColumn("channel");
        RDFVariable subject  = conversation_list.newColumn("subject");

        conversation_list.addColumn ("generator", channel.function<nie::generator>());
        // Coalesce
        conversation_list.addColumn ("contactUID", contact.function<nco::contactUID>());
        RDFVariable lastDate = conversation_list.newColumn("lastDate");
        RDFVariable lastMessage = conversation_list.newColumn("lastMessage");
        conversation_list.addColumn ("plainTextContent", lastMessage.function<nie::plainTextContent>());
        RDFVariable fromVCard = lastMessage.function<nmo::fromVCard>();
        conversation_list.addColumn ("fileName", fromVCard.function<nfo::fileName>());
        conversation_list.addColumn ("label", fromVCard.function<rdfs::label>());

                /* ------------------------ */

                RDFSubSelect total_messages;

                RDFVariable total_messages_message;
                RDFVariable total_messages_channel = total_messages.variable (channel);

                total_messages_message.property <nmo::communicationChannel>(total_messages_channel);

                total_messages.addCountColumn ("total_messages", total_messages_message);

                conversation_list.addColumn (total_messages.asExpression());

                /* ------------------------ */

                RDFSubSelect total_unread;

                RDFVariable total_unread_message;
                RDFVariable total_unread_channel = total_unread.variable (channel);

                total_unread_message.property <nmo::communicationChannel>(total_unread_channel);
                total_unread_message.property <nmo::isRead> (LiteralValue ("false"));

                total_unread.addCountColumn ("total_unread", total_unread_message);

                conversation_list.addColumn (total_unread.asExpression());

                /* ------------------------ */

                RDFSubSelect total_sent;

                RDFVariable total_sent_message;
                RDFVariable total_sent_channel = total_sent.variable (channel);

                total_sent_message.property <nmo::communicationChannel>(total_sent_channel);
                total_sent_message.property <nmo::isSent> (LiteralValue ("true"));

                total_sent.addCountColumn ("total_sent", total_sent_message);

                conversation_list.addColumn (total_sent.asExpression());

                /* ------------------------ */

                RDFSubSelect inner;

                RDFVariable inner_channel  = inner.newColumnAs (channel);
                RDFVariable inner_subject  = inner.newColumnAs (subject);
                RDFVariable inner_lastDate = inner.newColumnAs (lastDate);

                inner_channel.isOfType <nmo::CommunicationChannel>();
                inner_channel.property <nie::subject>(inner_subject);
                inner_channel.property <nmo::lastMessageDate>(inner_lastDate);

                        /* ------------------------ */

                        RDFSubSelect inner_message;

                        RDFVariable inner_message_message;
                        RDFVariable inner_message_channel = inner_message.variable (inner_channel);
                        RDFVariable inner_message_sentDate;

                        inner_message_message.property <nmo::communicationChannel> (inner_message_channel);
                        inner_message_message.property <nmo::sentDate> (inner_message_sentDate);

                        inner_message.addColumn ("message", inner_message_message);

                        /* ------------------------ */

                inner.addColumnAs (inner_message.asExpression(), lastMessage);

                        /* ------------------------ */

                        RDFSubSelect inner_contact;
                        RDFVariable inner_contact_contact;
                        RDFVariable inner_contact_binder;
                        RDFVariableList alternatives = inner_contact_binder.unionChildren(2);
 
                        RDFVariable inner_contact_channel = inner_contact.variable (inner_channel);
 
                        RDFVariable inner_contact_contact_a = alternatives[0].subject<nco::hasIMAddress>();
                        inner_contact_contact_a.isOfType<nco::PersonContact>();
                        RDFVariable inner_contact_participant_a = alternatives[0].subject<nco::hasIMAddress>();
                        inner_contact_participant_a.subject<nmo::hasParticipant>() = inner_contact_channel;
 
                        RDFVariable inner_contact_contact_b = alternatives[1].subject<maemo::localPhoneNumber>().subject<nco::hasPhoneNumber>();
                        inner_contact_contact_b.isOfType<nco::PersonContact>();
                        RDFVariable inner_contact_participant_b = alternatives[1].subject<maemo::localPhoneNumber>().subject<nco::hasPhoneNumber>();
                        inner_contact_participant_b.subject<nmo::hasParticipant>() = inner_contact_channel;
 
                        inner_contact_contact_a = inner_contact_contact;
                        inner_contact_contact_b = inner_contact_contact;
   
                        inner_contact.addColumn(inner_contact_contact);

                /*
                        RDFSubSelect inner_contact;
                        RDFVariable inner_contact_contact;
                        RDFVariable inner_contact_binder;
                        RDFVariableList alternatives = inner_contact_binder.unionChildren(2);
 
                        RDFVariable inner_contact_channel = inner_contact.variable (inner_channel);
                        RDFVariable inner_contact_participant = inner_contact_channel.object<nmo::hasParticipant>();
 
 
                        RDFVariable inner_contact_contact_a = alternatives[0].subject<nco::hasIMAddress>();
                        inner_contact_contact_a.isOfType<nco::PersonContact>();
                        alternatives[0].subject<nco::hasIMAddress>() = inner_contact_participant;
 
                        RDFVariable inner_contact_contact_b = alternatives[1].subject<maemo::localPhoneNumber>().subject<nco::hasPhoneNumber>();
                        inner_contact_contact_b.isOfType<nco::PersonContact>();
                        alternatives[1].subject<maemo::localPhoneNumber>().subject<nco::hasPhoneNumber>() = inner_contact_participant;
 
                        inner_contact_contact_a = inner_contact_contact;
                        inner_contact_contact_b = inner_contact_contact;
 
                        inner_contact.addColumn(inner_contact_contact);
                 */

                 /*
                        RDFSubSelect inner_contact;
                        RDFVariable inner_contact_contact = RDFVariable::fromType<nco::PersonContact>();
                        RDFVariable inner_contact_binder;
                        RDFVariableList alternatives = inner_contact_binder.unionChildren(2);
 
                        RDFVariable inner_contact_channel = inner_contact.variable (inner_channel);
                        RDFVariable inner_contact_participant = inner_contact_channel.object<nmo::hasParticipant>();
 
                        alternatives[0].subject<nco::hasIMAddress>(inner_contact_contact);
                        alternatives[0].subject<nco::hasIMAddress>(inner_contact_participant);
 
                        alternatives[1].subject<maemo::localPhoneNumber>().subject<nco::hasPhoneNumber>(inner_contact_contact);
                        alternatives[1].subject<maemo::localPhoneNumber>().subject<nco::hasPhoneNumber>(inner_contact_participant);
 
                        inner_contact.addColumn(inner_contact_contact);
                  */

                        /* ------------------------ */

                inner.addColumnAs (inner_contact.asExpression(), contact);

                /* ------------------------ */

        conversation_list.orderBy(lastDate, RDFSelect::Descending);
        conversation_list.limit(50);

        /* ------------------------ */

        LiveNodes query_model = ::tracker()->modelQuery (conversation_list,
                                                         SopranoLive::RDFStrategy::LiveStrategy);

                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
    }
}

void RTComBenchmark::test_rtcom_03 ()
{
    QBENCHMARK {
        /*
        SELECT ?call ?date ?from ?to
             rdf:type(?call)
             nmo:isSent(?call)
             nmo:isAnswered(?call)
             nmo:isRead(?call)
             nmo:sentDate(?call)
             nmo:receivedDate(?call)
             nmo:duration(?call)
             nie:contentLastModified(?call)
             (SELECT ?contact
              WHERE
              {
                  {
                      ?contact a nco:PersonContact .
                      ?contact nco:contactUID ?contactId .
                      {
                        ?call nmo:to ?address .
                      }
                      UNION
                      {
                        ?call nmo:from ?address .
                      }
                    ?address nco:hasIMAddress ?imaddress .
                    ?contact nco:hasIMAddress ?imaddress .
                  }
                  UNION
                  {
                      ?contact a nco:PersonContact .
                      ?contact nco:contactUID ?contactId .
                      {
                        ?call nmo:to ?address .
                      }
                      UNION
                      {
                        ?call nmo:from ?address .
                      }
                    ?address nco:hasPhoneNumber ?addressNumber .
                    ?addressNumber maemo:localPhoneNumber ?number .
                    ?contact nco:hasPhoneNumber ?contactNumber .
                    ?contactNumber maemo:localPhoneNumber ?number .
                  }
              })
        WHERE
        {
          {
            ?call a nmo:Call .
            ?call nmo:sentDate ?date .
            ?call nmo:from ?fromContact .
            ?call nmo:to ?toContact .
            ?fromContact nco:hasContactMedium ?from .
            ?toContact nco:hasContactMedium ?to .
          }
        }
        ORDER BY DESC(?date) LIMIT 50
        */

        /* ------------------------ */

        //::tracker()->setVerbosity (2);

        RDFSelect call_history;

        RDFVariable call  = call_history.newColumn("call");
        RDFVariable date  = call_history.newColumn("date");
        RDFVariable from  = call_history.newColumn("from");
        RDFVariable to    = call_history.newColumn("to");

        RDFVariable type = call.function<rdf::type>();
        call_history.addColumn("type", type);
        RDFVariable isSent = call.function<nmo::isSent>();
        call_history.addColumn("isSent", isSent);
        RDFVariable isAnswered = call.function<nmo::isRead>();
        call_history.addColumn("isAnswered", isAnswered);
        RDFVariable isRead = call.function<nmo::isAnswered>();
        call_history.addColumn("isRead", isRead);
        RDFVariable sentDate = call.function<nmo::sentDate>();
        call_history.addColumn("sentDate", sentDate);
        RDFVariable receivedDate = call.function<nmo::receivedDate>();
        call_history.addColumn("receievedDate", receivedDate);
        RDFVariable duration = call.function<nmo::duration>();
        call_history.addColumn("duration", duration);
        RDFVariable contentLastModified = call.function<nie::contentLastModified>();
        call_history.addColumn("contentLastModified", contentLastModified);
        
        RDFVariable fromContact;
        RDFVariable toContact;

        call.isOfType <nmo::Call>();
        call.property <nmo::sentDate>(date);
        call.property <nmo::from>(fromContact);
        call.property <nmo::to>(toContact);

        fromContact.property <nco::hasContactMedium> (from);
        toContact.property <nco::hasContactMedium> (to);
        
                /* ------------------------ */
                
                RDFSubSelect contact_expression;

                RDFVariable contact_expression_call = contact_expression.variable (call);
                RDFVariable contact_expression_binder;
                RDFVariableList contact_expression_alternatives = contact_expression_binder.unionChildren(2);

                        /* ------------------------ */

                        RDFVariable contact_expression_ua_contact = contact_expression_alternatives[0];
                        RDFVariable contact_expression_ua_contactid ("contactID");
                        RDFVariable contact_expression_ua_address ("address");
                        
                        contact_expression_ua_contact.isOfType <nco::PersonContact> ();
                        contact_expression_ua_contact.property <nco::contactUID> (contact_expression_ua_contactid);

                        RDFVariable contact_expression_binder_ua;
                        RDFVariableList alternatives_ua = contact_expression_binder_ua.unionChildren(2);

                        RDFVariable contact_expression_ua_address_a = alternatives_ua[0].subject <nmo::to> (contact_expression_call);
                        RDFVariable contact_expression_ua_address_b = alternatives_ua[1].subject <nmo::from> (contact_expression_call);

                        contact_expression_ua_address_a = contact_expression_ua_address;
                        contact_expression_ua_address_b = contact_expression_ua_address;

                        RDFVariable contact_expression_ua_imaddress ("imaddress");
                        contact_expression_ua_address.property <nco::hasIMAddress> (contact_expression_ua_imaddress);
                        contact_expression_ua_contact.property <nco::hasIMAddress> (contact_expression_ua_imaddress);

                        /* ------------------------ */

#if 0
                        RDFVariable contact_expression_ub_contact = contact_expression_alternatives[1];
                        RDFVariable contact_expression_ub_contactid ("contactID");
                        RDFVariable contact_expression_ub_address ("address");
                        RDFVariable contact_expression_ub_imaddress ("imaddress");

                        contact_expression_ub_contact.isOfType <nco::PersonContact> ();
                        contact_expression_ub_contact.property <nco::contactUID> (contact_expression_ub_contactid);

                        RDFVariable contact_expression_binder_ub;
                        RDFVariableList alternatives_ub = contact_expression_binder_ub.unionChildren(2);

                        RDFVariable contact_expression_ub_address_a = alternatives_ub[0].subject <nmo::to> (contact_expression_call);
                        RDFVariable contact_expression_ub_address_b = alternatives_ub[1].subject <nmo::from> (contact_expression_call);

                        contact_expression_ub_address_a = contact_expression_ub_address;
                        contact_expression_ub_address_b = contact_expression_ub_address;

                        RDFVariable contact_expression_ub_number ("number");
                        RDFVariable contact_expression_ub_addressnumber ("addressNumber");
                        RDFVariable contact_expression_ub_contactnumber ("contactNumber");
                        contact_expression_ub_address.property <nco::hasPhoneNumber> (contact_expression_ub_addressnumber);
                        contact_expression_ub_addressnumber.property <maemo::localPhoneNumber> (contact_expression_ub_number);
                        contact_expression_ub_contact.property <nco::hasPhoneNumber> (contact_expression_ub_contactnumber);
                        contact_expression_ub_contactnumber.property <maemo::localPhoneNumber> (contact_expression_ub_number);
#endif

                        /* ------------------------ */

                contact_expression.addColumn ("contact", contact_expression_binder);
                
                call_history.addColumn ("contact", contact_expression.asExpression ());

                /* ------------------------ */

        call_history.orderBy(date, RDFSelect::Descending);
        call_history.limit(50);

        /* ------------------------ */

        LiveNodes query_model = ::tracker()->modelQuery (call_history,
                                                         SopranoLive::RDFStrategy::LiveStrategy);

                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
    }
}

QTEST_MAIN(RTComBenchmark)
