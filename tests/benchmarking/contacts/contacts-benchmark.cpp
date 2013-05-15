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

#include <contacts-benchmark.h>

ContactsBenchmark::ContactsBenchmark ()
{
    ;
}

#if 0
void ContactsBenchmark::test_contacts_01 ()
{
        /*
        SELECT DISTINCT
          ?_contact
          ?_Avatar_ImageUrl
          ?_Birthday_Birthday
          bound(?_Gender_Gender) AS ?_Gender_Gender_IsBound
          (?_Gender_Gender = nco:gender-female) AS ?_Gender_Gender_IsEqual_Female
          (?_Gender_Gender = nco:gender-male) AS ?_Gender_Gender_IsEqual_Male
          ?_Guid_Guid
          ?_Name_Prefix
          ?_Name_FirstName
          ?_Name_MiddleName
          ?_Name_LastName
          ?_Name_Suffix
          ?_Nickname_Nickname
          ?_Note_Note
          ?_Timestamp_CreationTimestamp
          ?_Timestamp_ModificationTimestamp
        WHERE
        {
          {
            ?_contact rdf:type nco:PersonContact .
            OPTIONAL { ?_contact nco:photo ?__1 . ?__1 nfo:fileUrl ?_Avatar_ImageUrl . }
            OPTIONAL { ?_contact nco:birthDate ?_Birthday_Birthday . }
            OPTIONAL { ?_contact nco:gender ?_Gender_Gender . }
            OPTIONAL { ?_contact nco:contactUID ?_Guid_Guid . }
            OPTIONAL { ?_contact nco:nameHonorificPrefix ?_Name_Prefix . }
            OPTIONAL { ?_contact nco:nameGiven ?_Name_FirstName . }
            OPTIONAL { ?_contact nco:nameAdditional ?_Name_MiddleName . }
            OPTIONAL { ?_contact nco:nameFamily ?_Name_LastName . }
            OPTIONAL { ?_contact nco:nameHonorificSuffix ?_Name_Suffix . }
            OPTIONAL { ?_contact nco:nickname ?_Nickname_Nickname . }
            OPTIONAL { ?_contact nco:note ?_Note_Note . }
            OPTIONAL { ?_contact nie:contentCreated ?_Timestamp_CreationTimestamp . }
            OPTIONAL { ?_contact nie:contentLastModified ?_Timestamp_ModificationTimestamp . }
          }
        }
        ORDER BY ?_contact LIMIT 50
        */
        QBENCHMARK {
                RDFSelect query;
                query.distinct (true);

                RDFVariable contact ("contact");
                RDFVariable imageurl ("imageurl");
                RDFVariable birthday = query.newColumn ("birthday");

                RDFVariable gender ("gender");

                RDFVariable gender_isbound ("gender_isbound");
                query.addColumnAs (gender.isBound(), gender_isbound);

                RDFVariable gender_isfemale ("gender_isfemale");
                query.addColumnAs(gender == nco::gender_female::iri(), gender_isfemale);

                RDFVariable gender_ismale ("gender_ismale");
                query.addColumnAs(gender == nco::gender_male::iri(), gender_ismale);

                RDFVariable guid = query.newColumn ("guid");
                RDFVariable prefix = query.newColumn ("prefix");
                RDFVariable firstname = query.newColumn ("firstname");
                RDFVariable middlename = query.newColumn ("middlename");
                RDFVariable lastname = query.newColumn ("lastname");
                RDFVariable suffix = query.newColumn ("suffix");
                RDFVariable nickname = query.newColumn ("nickname");
                RDFVariable note = query.newColumn ("note");
                RDFVariable create_time = query.newColumn ("create_time");
                RDFVariable modify_time = query.newColumn ("modify_time");

                /* WHERE CLAUSE */

                contact.isOfType <nco::PersonContact> ();

                RDFVariable anon_1;
                contact.optional().property <nco::photo> (anon_1);
                anon_1.property <nfo::fileUrl> (imageurl);

                contact.optional().property <nco::birthDate> (birthday);
                contact.optional().property <nco::gender> (gender);
                contact.optional().property <nco::contactUID> (guid);
                contact.optional().property <nco::nameHonorificPrefix> (prefix);
                contact.optional().property <nco::nameGiven> (firstname);
                contact.optional().property <nco::nameAdditional> (middlename);
                contact.optional().property <nco::nameFamily> (lastname);
                contact.optional().property <nco::nameHonorificSuffix> (suffix);
                contact.optional().property <nco::nickname> (nickname);
                contact.optional().property <nco::note> (note);
                contact.optional().property <nie::contentCreated> (create_time);
                contact.optional().property <nie::contentLastModified> (modify_time);

                query.addColumn (contact);
                query.addColumn (imageurl);

                query.orderBy (contact, RDFSelect::Descending);
                query.limit (50);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}
#endif
        
void ContactsBenchmark::test_contacts_02 ()
{
        /*
        SELECT DISTINCT
          ?_contact
          ?_Avatar_ImageUrl
          nco:birthDate(?_contact)
          bound(?_Gender_Gender)
          (?_Gender_Gender = nco:gender-female)
          (?_Gender_Gender = nco:gender-male)
          nco:contactUID(?_contact)
          nco:nameHonorificPrefix(?_contact)
          nco:nameGiven(?_contact)
          nco:nameAdditional(?_contact)
          nco:nameFamily(?_contact)
          nco:nameHonorificSuffix(?_contact)
          nco:nickname(?_contact)
          nco:note(?_contact)
          nie:contentCreated(?_contact)
          nie:contentLastModified(?_contact)
        WHERE
        {
          {
            ?_contact rdf:type nco:PersonContact .
            OPTIONAL { ?_contact nco:photo ?__1 . ?__1 nfo:fileUrl ?_Avatar_ImageUrl . }
            OPTIONAL { ?_contact nco:gender ?_Gender_Gender . }
          }
        }
        ORDER BY ?_contact LIMIT 50
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                query.distinct (true);

                RDFVariable contact ("contact");
                RDFVariable imageurl ("imageurl");

                query.addColumn (contact.function <nco::birthDate> ());

                RDFVariable gender ("gender");

                RDFVariable gender_isbound ("gender_isbound");
                query.addColumnAs (gender.isBound(), gender_isbound);

                RDFVariable gender_isfemale ("gender_isfemale");
                query.addColumnAs(gender == nco::gender_female::iri(), gender_isfemale);
                RDFVariable gender_ismale ("gender_ismale");
                query.addColumnAs(gender == nco::gender_male::iri(), gender_ismale);

                query.addColumn (contact.function <nco::contactUID> ());
                query.addColumn (contact.function <nco::nameHonorificPrefix> ());
                query.addColumn (contact.function <nco::nameGiven> ());
                query.addColumn (contact.function <nco::nameAdditional> ());
                query.addColumn (contact.function <nco::nameFamily> ());
                query.addColumn (contact.function <nco::nameHonorificSuffix> ());
                query.addColumn (contact.function <nco::nickname> ());
                query.addColumn (contact.function <nco::note> ());
                query.addColumn (contact.function <nie::contentCreated> ());
                query.addColumn (contact.function <nie::contentLastModified> ());

                /* WHERE */

                contact.isOfType <nco::PersonContact> ();

                RDFVariable anon_1;
                contact.optional().property <nco::photo> (anon_1);
                anon_1.property <nfo::fileUrl> (imageurl);

                contact.optional().property <nco::gender> (gender);

                query.addColumn (contact);
                query.addColumn (imageurl);

                query.orderBy (contact, RDFSelect::Descending);
                query.limit (50);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}
        
void ContactsBenchmark::test_contacts_03 ()
{
        /*
        SELECT DISTINCT
          ?_contact
          ?_Address_Country
          ?_Address_Locality
          ?_Address_PostOfficeBox
          ?_Address_Postcode
          ?_Address_Region
          ?_Address_Street
          bound(?_Address_SubTypes_Domestic) AS ?_Address_SubTypes_Domestic_IsBound
          bound(?_Address_SubTypes_International) AS ?_Address_SubTypes_International_IsBound
          bound(?_Address_SubTypes_Parcel) AS ?_Address_SubTypes_Parcel_IsBound
          bound(?_Address_SubTypes_Postal) AS ?_Address_SubTypes_Postal_IsBound
          bound(?_Address_Context_Work) AS ?_Address_Context_Work_IsBound
        WHERE
        {
          {
            ?_contact rdf:type nco:PersonContact .
            {
              ?_contact nco:hasPostalAddress ?__1 .
              ?__1 nco:country ?_Address_Country .
              ?__1 nco:locality ?_Address_Locality .
              ?__1 nco:pobox ?_Address_PostOfficeBox .
              ?__1 nco:postalcode ?_Address_Postcode .
              ?__1 nco:region ?_Address_Region .
              ?__1 nco:streetAddress ?_Address_Street .
              OPTIONAL {
                ?__1 rdf:type ?_Address_SubTypes_Domestic .
                FILTER((?_Address_SubTypes_Domestic = nco:DomesticDeliveryAddress)) .
              }
              OPTIONAL {
                ?__1 rdf:type ?_Address_SubTypes_International .
                FILTER((?_Address_SubTypes_International = nco:InternationalDeliveryAddress)) .
              }
              OPTIONAL {
                ?__1 rdf:type ?_Address_SubTypes_Parcel .
                FILTER((?_Address_SubTypes_Parcel = nco:ParcelDeliveryAddress)) .
              }
              OPTIONAL {
                ?__1 rdf:type ?_Address_SubTypes_Postal .
                FILTER((?_Address_SubTypes_Postal = nco:PostalAddress)) .
              }
            }
            UNION
            {
              ?_contact nco:hasAffiliation ?_Address_Context_Work .
              ?_Address_Context_Work nco:hasPostalAddress ?__2 .
              ?__2 nco:country ?_Address_Country .
              ?__2 nco:locality ?_Address_Locality .
              ?__2 nco:pobox ?_Address_PostOfficeBox .
              ?__2 nco:postalcode ?_Address_Postcode .
              ?__2 nco:region ?_Address_Region .
              ?__2 nco:streetAddress ?_Address_Street .
              OPTIONAL {
                ?__2 rdf:type ?_Address_SubTypes_Domestic .
                FILTER((?_Address_SubTypes_Domestic = nco:DomesticDeliveryAddress)) .
              }
              OPTIONAL {
                ?__2 rdf:type ?_Address_SubTypes_International .
                FILTER((?_Address_SubTypes_International = nco:InternationalDeliveryAddress)) .
              }
              OPTIONAL {
                ?__2 rdf:type ?_Address_SubTypes_Parcel .
                FILTER((?_Address_SubTypes_Parcel = nco:ParcelDeliveryAddress)) .
              }
              OPTIONAL {
                ?__2 rdf:type ?_Address_SubTypes_Postal .
                FILTER((?_Address_SubTypes_Postal = nco:PostalAddress)) .
              }
            }
          }
        }
        ORDER BY ?_contact LIMIT 50
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                query.distinct (true);

                RDFVariable contact ("contact");
                RDFVariable a_Country ("address_Country");
                RDFVariable a_Locality ("address_Locality");
                RDFVariable a_PostOfficeBox ("address_PostOfficeBox");
                RDFVariable a_Postcode ("address_Postcode");
                RDFVariable a_Region ("address_Region");
                RDFVariable a_Street ("address_Street");
                RDFVariable asub_Domestic ("address_subtypes_Domestic");
                RDFVariable asub_International ("address_subtypes_International");
                RDFVariable asub_Parcel ("address_subtypes_Parcel");
                RDFVariable asub_Postal ("address_subtypes_Postal");
                RDFVariable a_ContextWork ("address_ContextWork");

                contact.isOfType <nco::PersonContact> ();

                RDFVariableList alternatives = contact.unionChildren (2);

                /* ---- */

                RDFVariable ua_contact = alternatives[0];
                RDFVariable ua_a_Country = ua_contact.variable (a_Country);
                RDFVariable ua_a_Locality = ua_contact.variable (a_Locality);
                RDFVariable ua_a_PostOfficeBox = ua_contact.variable (a_PostOfficeBox);
                RDFVariable ua_a_Postcode = ua_contact.variable (a_Postcode);
                RDFVariable ua_a_Region = ua_contact.variable (a_Region);
                RDFVariable ua_a_Street = ua_contact.variable (a_Street);
                RDFVariable ua_anon;

                ua_contact.property <nco::hasPostalAddress> (ua_anon);
                ua_anon.property <nco::country> (ua_a_Country);
                ua_anon.property <nco::locality> (ua_a_Locality);
                ua_anon.property <nco::pobox> (ua_a_PostOfficeBox);
                ua_anon.property <nco::postalcode> (ua_a_Postcode);
                ua_anon.property <nco::region> (ua_a_Region);
                ua_anon.property <nco::streetAddress> (ua_a_Street);

                RDFVariable ua_anon_o1 = ua_anon.optional();
                RDFVariable ua_asub_Domestic_opt = ua_anon_o1.variable (asub_Domestic);
                ua_anon_o1.property <rdf::type> (ua_asub_Domestic_opt);
                ua_asub_Domestic_opt  == nco::DomesticDeliveryAddress::iri();

                RDFVariable ua_anon_o2 = ua_anon.optional();
                RDFVariable ua_asub_International_opt = ua_anon_o2.variable (asub_International);
                ua_anon_o2.property <rdf::type> (ua_asub_International_opt);
                ua_asub_International_opt  == nco::InternationalDeliveryAddress::iri();

                RDFVariable ua_anon_o3 = ua_anon.optional();
                RDFVariable ua_asub_Parcel_opt = ua_anon_o3.variable (asub_Parcel);
                ua_anon_o3.property <rdf::type> (ua_asub_Parcel_opt);
                ua_asub_Parcel_opt  == nco::ParcelDeliveryAddress::iri();

                RDFVariable ua_anon_o4 = ua_anon.optional();
                RDFVariable ua_asub_Postal_opt = ua_anon_o4.variable (asub_Postal);
                ua_anon_o4.property <rdf::type> (ua_asub_Postal_opt);
                ua_asub_Postal_opt  == nco::PostalAddress::iri();

                /* ---- */

                RDFVariable ub_contact = alternatives[1];
                RDFVariable ub_a_Country = ub_contact.variable (a_Country);
                RDFVariable ub_a_Locality = ub_contact.variable (a_Locality);
                RDFVariable ub_a_PostOfficeBox = ub_contact.variable (a_PostOfficeBox);
                RDFVariable ub_a_Postcode = ub_contact.variable (a_Postcode);
                RDFVariable ub_a_Region = ub_contact.variable (a_Region);
                RDFVariable ub_a_Street = ub_contact.variable (a_Street);
                RDFVariable ub_a_ContextWork = ub_contact.variable (a_ContextWork);
                RDFVariable ub_anon;

                ub_contact.property <nco::hasAffiliation> (ub_a_ContextWork);
                ub_a_ContextWork.property <nco::hasPostalAddress> (ub_anon);
                ub_anon.property <nco::country> (ub_a_Country);
                ub_anon.property <nco::locality> (ub_a_Locality);
                ub_anon.property <nco::pobox> (ub_a_PostOfficeBox);
                ub_anon.property <nco::postalcode> (ub_a_Postcode);
                ub_anon.property <nco::region> (ub_a_Region);
                ub_anon.property <nco::streetAddress> (ub_a_Street);

                RDFVariable ub_anon_o1 = ub_anon.optional();
                RDFVariable ub_asub_Domestic_opt = ub_anon_o1.variable (asub_Domestic);
                ub_anon_o1.property <rdf::type> (ub_asub_Domestic_opt);
                ub_asub_Domestic_opt  == nco::DomesticDeliveryAddress::iri();

                RDFVariable ub_anon_o2 = ub_anon.optional();
                RDFVariable ub_asub_International_opt = ub_anon_o2.variable (asub_International);
                ub_anon_o2.property <rdf::type> (ub_asub_International_opt);
                ub_asub_International_opt  == nco::InternationalDeliveryAddress::iri();

                RDFVariable ub_anon_o3 = ub_anon.optional();
                RDFVariable ub_asub_Parcel_opt = ub_anon_o3.variable (asub_Parcel);
                ub_anon_o3.property <rdf::type> (ub_asub_Parcel_opt);
                ub_asub_Parcel_opt  == nco::ParcelDeliveryAddress::iri();

                RDFVariable ub_anon_o4 = ub_anon.optional();
                RDFVariable ub_asub_Postal_opt = ub_anon_o4.variable (asub_Postal);
                ub_anon_o4.property <rdf::type> (ub_asub_Postal_opt);
                ub_asub_Postal_opt  == nco::PostalAddress::iri();

                /* ---- */

                query.addColumn (contact);
                query.addColumn (a_Country);
                query.addColumn (a_Locality);
                query.addColumn (a_PostOfficeBox);
                query.addColumn (a_Postcode);
                query.addColumn (a_Region);
                query.addColumn (a_Street);
                query.addColumn ("address_subtypes_Domestic_isBound", asub_Domestic.isBound());
                query.addColumn ("address_subtypes_International_isBound", asub_International.isBound());
                query.addColumn ("address_subtypes_Parcel_isBound", asub_Parcel.isBound());
                query.addColumn ("address_subtypes_Postal_isBound", asub_Postal.isBound());
                query.addColumn ("address_ContextWork_isBound", a_ContextWork.isBound());

                query.orderBy (contact);
                query.limit (50);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}
        
void ContactsBenchmark::test_contacts_04 ()
{
        /*
        SELECT
          ?contact
          nco:country(?postal)
          nco:locality(?postal)
          nco:pobox(?postal)
          nco:postalcode(?postal)
          nco:region(?postal)
          nco:streetAddress(?postal)
          bound(?work)
        WHERE
        {
          ?contact rdf:type nco:PersonContact .
          { ?contact nco:hasPostalAddress ?postal . }
          UNION
          { ?contact nco:hasAffiliation ?work .
              ?work nco:hasPostalAddress ?postal .
          }
        }
        ORDER BY ?contact LIMIT 50
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;

                RDFVariable contact ("contact");
                RDFVariable postal  ("postal");
                RDFVariable work ("work");

                /* -------- */

                contact.isOfType <nco::PersonContact> ();

                RDFVariableList alternatives = contact.unionChildren(2);

                RDFVariable ua_contact = alternatives[0];
                RDFVariable ua_postal  = alternatives[0].variable (postal); 
                ua_contact.property <nco::hasPostalAddress> (ua_postal);

                RDFVariable ub_contact = alternatives[1];
                RDFVariable ub_postal  = alternatives[1].variable (postal); 
                RDFVariable ub_work  = alternatives[1].variable (work); 
                ub_contact.property <nco::hasAffiliation> (ub_work);
                ub_work.property <nco::hasPostalAddress> (ub_postal);

                /* -------- */

                query.addColumn (postal.function <nco::country> ());
                query.addColumn (postal.function <nco::locality> ());
                query.addColumn (postal.function <nco::pobox> ());
                query.addColumn (postal.function <nco::postalcode> ());
                query.addColumn (postal.function <nco::region> ());
                query.addColumn (postal.function <nco::streetAddress> ());

                RDFVariable work_isbound ("work_isbound");
                query.addColumnAs (work.isBound(), work_isbound);

                query.addColumn (contact);
                query.addColumn (postal);

                /* -------- */

                query.orderBy (contact, RDFSelect::Descending);
                query.limit (50);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}
        
void ContactsBenchmark::test_contacts_05 ()
{
        /*
        SELECT DISTINCT
          ?_contact
          ?_EmailAddress
          ?_EmailAddress_EmailAddress
          bound(?_EmailAddress_Context_Work) AS ?_EmailAddress_Context_Work_IsBound
        WHERE
        {
          {
            ?_contact rdf:type nco:PersonContact .
            {
              ?_contact nco:hasEmailAddress ?_EmailAddress .
              ?_EmailAddress nco:emailAddress ?_EmailAddress_EmailAddress .
            }
            UNION
            {
              ?_contact nco:hasAffiliation ?_EmailAddress_Context_Work .
              ?_EmailAddress_Context_Work nco:hasEmailAddress ?_EmailAddress .
              ?_EmailAddress nco:emailAddress ?_EmailAddress_EmailAddress .
            }
          }
        }
        ORDER BY ?_contact LIMIT 50
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                query.distinct (true);

                RDFVariable contact ("contact");
                RDFVariable email ("email");
                RDFVariable emailtext ("emailtext");
                RDFVariable workemail ("workemail");

                /* -------- */

                contact.isOfType <nco::PersonContact> ();

                RDFVariableList alternatives = contact.unionChildren (2);
                RDFVariable ua_contact = alternatives [0];
                RDFVariable ub_contact = alternatives [1];

                RDFVariable ua_email = ua_contact.variable (email);
                RDFVariable ua_emailtext = ua_contact.variable (emailtext);
                ua_contact.property <nco::hasEmailAddress> (ua_email);
                ua_email.property <nco::emailAddress> (ua_emailtext);

                RDFVariable ub_email = ub_contact.variable (email);
                RDFVariable ub_emailtext = ub_contact.variable (emailtext);
                RDFVariable ub_workemail = ub_contact.variable (workemail);
                ub_contact.property <nco::hasAffiliation> (ub_workemail);
                ub_workemail.property <nco::hasEmailAddress> (ub_email);
                ub_email.property <nco::emailAddress> (ub_emailtext);

                /* -------- */

                query.addColumn (contact);
                query.addColumn (email);
                query.addColumn (emailtext);
                query.addColumn ("workemail_isbound", workemail.isBound());
                query.orderBy (contact);
                query.limit (50);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}
        
void ContactsBenchmark::test_contacts_06 ()
{
        /*
        SELECT
          ?contact
          ?email
          nco:emailAddress(?email)
          bound(?work)
        WHERE
        {
          {
            ?contact rdf:type nco:PersonContact .
            {
              ?contact nco:hasEmailAddress ?email .
            }
            UNION
            {
              ?contact nco:hasAffiliation ?work .
              ?work nco:hasEmailAddress ?email .
            }
          }
        }
        ORDER BY ?_contact LIMIT 50
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;

                RDFVariable contact ("contact");
                RDFVariable email ("email");
                RDFVariable work ("work");

                contact.isOfType <nco::PersonContact> ();

                RDFVariableList alternatives = contact.unionChildren(2);

                RDFVariable ua_contact = alternatives[0];
                RDFVariable ua_email  = alternatives[0].variable (email); 
                ua_contact.property <nco::hasEmailAddress> (ua_email);

                RDFVariable ub_contact = alternatives[1];
                RDFVariable ub_email  = alternatives[1].variable (email); 
                RDFVariable ub_work  = alternatives[1].variable (work); 
                ub_contact.property <nco::hasAffiliation> (ub_work);
                ub_work.property <nco::hasEmailAddress> (ub_email);

                query.addColumn (contact);
                query.addColumn (email);
                query.addColumn (email.function <nco::emailAddress> ());
                query.addColumn (work.isBound ());

                query.orderBy (contact);
                query.limit (50);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}
        
void ContactsBenchmark::test_contacts_07 ()
{
        /*
        SELECT DISTINCT
          ?_contact
          ?_OnlineAccount
          ?_OnlineAccount_AccountUri
          ?_OnlineAccount_ServiceProvider
          bound(?_OnlineAccount_Capabilities)
          AS ?_OnlineAccount_Capabilities_IsBound
          (?_OnlineAccount_Capabilities = nco:im-capability-text-chat)
          AS ?_OnlineAccount_Capabilities_IsEqual_TextChat
          (?_OnlineAccount_Capabilities = nco:im-capability-media-calls)
          AS ?_OnlineAccount_Capabilities_IsEqual_MediaCalls
          (?_OnlineAccount_Capabilities = nco:im-capability-audio-calls)
          AS ?_OnlineAccount_Capabilities_IsEqual_AudioCalls
          (?_OnlineAccount_Capabilities = nco:im-capability-video-calls)
          AS ?_OnlineAccount_Capabilities_IsEqual_VideoCalls
          (?_OnlineAccount_Capabilities = nco:im-capability-upgrading-calls)
          AS ?_OnlineAccount_Capabilities_IsEqual_UpgradingCalls
          (?_OnlineAccount_Capabilities = nco:im-capability-file-transfers)
          AS ?_OnlineAccount_Capabilities_IsEqual_FileTransfers
          (?_OnlineAccount_Capabilities = nco:im-capability-stream-tubes)
          AS ?_OnlineAccount_Capabilities_IsEqual_StreamTubes
          (?_OnlineAccount_Capabilities = nco:im-capability-dbus-tubes)
          AS ?_OnlineAccount_Capabilities_IsEqual_DBusTubes
          bound(?_OnlineAccount_Context_Work)
          AS ?_OnlineAccount_Context_Work_IsBound
        WHERE
        {
          {
            ?_contact rdf:type nco:PersonContact .
              {
                ?_contact nco:hasIMAddress ?_OnlineAccount .
                ?_OnlineAccount nco:imID ?_OnlineAccount_AccountUri .
                ?_OnlineAccount nco:imCapability ?_OnlineAccount_Capabilities .
                OPTIONAL { ?_OnlineAccount_ServiceProvider nco:hasIMContact ?_OnlineAccount . }
              }
              UNION
              {
                ?_contact nco:hasAffiliation ?_OnlineAccount_Context_Work .
                ?_OnlineAccount_Context_Work nco:hasIMAddress ?_OnlineAccount .\
                ?_OnlineAccount nco:imID ?_OnlineAccount_AccountUri .
                ?_OnlineAccount nco:imCapability ?_OnlineAccount_Capabilities .
                OPTIONAL { ?_OnlineAccount_ServiceProvider nco:hasIMContact ?_OnlineAccount . }
              }
          }
        }
        ORDER BY ?_contact LIMIT 50
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                query.distinct (true);

                RDFVariable contact ("contact");
                RDFVariable oa ("oa");
                RDFVariable oa_AccountUri ("oa_AccountUri");
                RDFVariable oa_ServiceProvider ("oa_ServiceProvider");
                RDFVariable oa_Capabilities ("oa_Capabilities");
                RDFVariable oa_ContextWork ("oa_ContextWork");

                contact.isOfType <nco::PersonContact> ();

                RDFVariableList alternatives = contact.unionChildren (2);

                RDFVariable ua_contact = alternatives[0];
                RDFVariable ua_oa = ua_contact.variable (oa);
                RDFVariable ua_oa_AccountUri = ua_contact.variable (oa_AccountUri);
                RDFVariable ua_oa_Capabilities = ua_contact.variable (oa_Capabilities);
                RDFVariable ua_oa_ServiceProvider = ua_contact.variable (oa_ServiceProvider);
                
                ua_contact.property <nco::hasIMAddress> (ua_oa);
                ua_oa.property <nco::imID> (ua_oa_AccountUri);
                ua_oa.property <nco::imCapability> (ua_oa_Capabilities);
                ua_oa_ServiceProvider.optional().property <nco::hasIMContact> (ua_oa);

                RDFVariable ub_contact = alternatives[1];
                RDFVariable ub_oa = ub_contact.variable (oa);
                RDFVariable ub_oa_AccountUri = ub_contact.variable (oa_AccountUri);
                RDFVariable ub_oa_Capabilities = ub_contact.variable (oa_Capabilities);
                RDFVariable ub_oa_ServiceProvider = ub_contact.variable (oa_ServiceProvider);
                RDFVariable ub_oa_ContextWork = ub_contact.variable (oa_ContextWork);
                
                ub_contact.property <nco::hasAffiliation> (ub_oa_ContextWork);
                ub_oa_ContextWork.property <nco::hasIMAddress> (ub_oa);
                ub_oa.property <nco::imID> (ub_oa_AccountUri);
                ub_oa.property <nco::imCapability> (ub_oa_Capabilities);
                ub_oa_ServiceProvider.optional().property <nco::hasIMContact> (ub_oa);

                query.addColumn (contact);
                query.addColumn (oa);
                query.addColumn (oa_AccountUri);
                query.addColumn (oa_ServiceProvider);
                query.addColumn ("oa_Capabilities_isBound", oa_Capabilities.isBound());
                query.addColumn ("oa_Capabilities_isEqual_TextChat", oa_Capabilities == nco::im_capability_text_chat::iri());
                query.addColumn ("oa_Capabilities_isEqual_MediaCalls", oa_Capabilities == nco::im_capability_media_calls::iri());
                query.addColumn ("oa_Capabilities_isEqual_AudioCalls", oa_Capabilities == nco::im_capability_audio_calls::iri());
                query.addColumn ("oa_Capabilities_isEqual_VideoCalls", oa_Capabilities == nco::im_capability_video_calls::iri());
                query.addColumn ("oa_Capabilities_isEqual_UpgradingCalls", oa_Capabilities == nco::im_capability_upgrading_calls::iri());
                query.addColumn ("oa_Capabilities_isEqual_FileTransfers", oa_Capabilities == nco::im_capability_file_transfers::iri());
                query.addColumn ("oa_Capabilities_isEqual_StreamTubes", oa_Capabilities == nco::im_capability_stream_tubes::iri());
                query.addColumn ("oa_Capabilities_isEqual_DBusTubes", oa_Capabilities == nco::im_capability_dbus_tubes::iri());
                query.addColumn ("oa_ContextWork_isBound", oa_ContextWork.isBound());

                query.orderBy (contact);
                query.limit (50);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}
        
void ContactsBenchmark::test_contacts_08 ()
{
        /*
        SELECT DISTINCT
          ?_contact
          ?_OnlineAccount
          ?_OnlineAccount_AccountUri
          ?_OnlineAccount_ServiceProvider
          bound(?ork)
        WHERE
        {
            ?_contact rdf:type nco:PersonContact .
              {
                ?_contact nco:hasIMAddress ?_OnlineAccount .
                ?_OnlineAccount nco:imID ?_OnlineAccount_AccountUri .
                OPTIONAL { ?_OnlineAccount_ServiceProvider nco:hasIMContact ?_OnlineAccount . }
              }
              UNION
              {
                ?_contact nco:hasAffiliation ?_OnlineAccount_Context_Work .
                ?_OnlineAccount_Context_Work nco:hasIMAddress ?_OnlineAccount .
                ?_OnlineAccount nco:imID ?_OnlineAccount_AccountUri .
                OPTIONAL { ?_OnlineAccount_ServiceProvider nco:hasIMContact ?_OnlineAccount . }
              }
        }
        ORDER BY ?_contact LIMIT 50
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                query.distinct (true);

                RDFVariable contact ("contact");
                RDFVariable oa ("oa");
                RDFVariable oa_AccountUri ("oa_AccountUri");
                RDFVariable oa_ServiceProvider ("oa_ServiceProvider");
                RDFVariable oa_ContextWork ("oa_ContextWork");

                contact.isOfType <nco::PersonContact> ();

                RDFVariableList alternatives = contact.unionChildren (2);

                RDFVariable ua_contact = alternatives[0];
                RDFVariable ua_oa = ua_contact.variable (oa);
                RDFVariable ua_oa_AccountUri = ua_contact.variable (oa_AccountUri);
                RDFVariable ua_oa_ServiceProvider = ua_contact.variable (oa_ServiceProvider);
                
                ua_contact.property <nco::hasIMAddress> (ua_oa);
                ua_oa.property <nco::imID> (ua_oa_AccountUri);
                ua_oa_ServiceProvider.optional().property <nco::hasIMContact> (ua_oa);

                RDFVariable ub_contact = alternatives[1];
                RDFVariable ub_oa = ub_contact.variable (oa);
                RDFVariable ub_oa_AccountUri = ub_contact.variable (oa_AccountUri);
                RDFVariable ub_oa_ServiceProvider = ub_contact.variable (oa_ServiceProvider);
                RDFVariable ub_oa_ContextWork = ub_contact.variable (oa_ContextWork);
                
                ub_contact.property <nco::hasAffiliation> (ub_oa_ContextWork);
                ub_oa_ContextWork.property <nco::hasIMAddress> (ub_oa);
                ub_oa.property <nco::imID> (ub_oa_AccountUri);
                ub_oa_ServiceProvider.optional().property <nco::hasIMContact> (ub_oa);

                query.addColumn (contact);
                query.addColumn (oa);
                query.addColumn (oa_AccountUri);
                query.addColumn (oa_ServiceProvider);
                query.addColumn (oa_ContextWork.isBound());

                query.orderBy (contact);
                query.limit (50);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}
        
#if 0
void ContactsBenchmark::test_contacts_09 ()
{
        /*
        SELECT DISTINCT
          ?_contact ?_PhoneNumber ?_PhoneNumber_PhoneNumber
          bound(?_PhoneNumber_SubTypes_BulletinBoardSystem) AS ?_PhoneNumber_SubTypes_BulletinBoardSystem_IsBound
          bound(?_PhoneNumber_SubTypes_Car) AS ?_PhoneNumber_SubTypes_Car_IsBound
          bound(?_PhoneNumber_SubTypes_Fax) AS ?_PhoneNumber_SubTypes_Fax_IsBound
          bound(?_PhoneNumber_SubTypes_MessagingCapable) AS ?_PhoneNumber_SubTypes_MessagingCapable_IsBound
          bound(?_PhoneNumber_SubTypes_Mobile) AS ?_PhoneNumber_SubTypes_Mobile_IsBound
          bound(?_PhoneNumber_SubTypes_Modem) AS ?_PhoneNumber_SubTypes_Modem_IsBound
          bound(?_PhoneNumber_SubTypes_Pager) AS ?_PhoneNumber_SubTypes_Pager_IsBound
          bound(?_PhoneNumber_SubTypes_Video) AS ?_PhoneNumber_SubTypes_Video_IsBound
          bound(?_PhoneNumber_SubTypes_Voice) AS ?_PhoneNumber_SubTypes_Voice_IsBound
          bound(?_PhoneNumber_Context_Work) AS ?_PhoneNumber_Context_Work_IsBound
        WHERE
        {
          {
            ?_contact rdf:type nco:PersonContact .
              {
                ?_contact nco:hasPhoneNumber ?_PhoneNumber .
                ?_PhoneNumber nco:phoneNumber ?_PhoneNumber_PhoneNumber .
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_BulletinBoardSystem .
                    FILTER((?_PhoneNumber_SubTypes_BulletinBoardSystem = nco:BbsNumber)) .
                  }
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_Car .
                    FILTER((?_PhoneNumber_SubTypes_Car = nco:CarPhoneNumber)) .
                  }
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_Fax .
                    FILTER((?_PhoneNumber_SubTypes_Fax = nco:FaxNumber)) .
                  }
                  OPTIONAL
                  {
                     ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_MessagingCapable .
                     FILTER((?_PhoneNumber_SubTypes_MessagingCapable = nco:MessagingNumber)) .
                  }
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_Mobile .
                    FILTER((?_PhoneNumber_SubTypes_Mobile = nco:CellPhoneNumber)) .
                  }
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_Modem .
                    FILTER((?_PhoneNumber_SubTypes_Modem = nco:ModemNumber)) .
                  }
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_Pager .
                    FILTER((?_PhoneNumber_SubTypes_Pager = nco:PagerNumber)) .
                  }
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_Video .
                    FILTER((?_PhoneNumber_SubTypes_Video = nco:VideoTelephoneNumber)) .
                  }
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_Voice .
                    FILTER((?_PhoneNumber_SubTypes_Voice = nco:VoicePhoneNumber)) .
                  }
              }
              UNION
              {
                ?_contact nco:hasAffiliation ?_PhoneNumber_Context_Work .
                ?_PhoneNumber_Context_Work nco:hasPhoneNumber ?_PhoneNumber .
                ?_PhoneNumber nco:phoneNumber ?_PhoneNumber_PhoneNumber .
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_BulletinBoardSystem .
                    FILTER((?_PhoneNumber_SubTypes_BulletinBoardSystem = nco:BbsNumber)) .
                  }
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_Car .
                    FILTER((?_PhoneNumber_SubTypes_Car = nco:CarPhoneNumber)) .
                  }
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_Fax .
                    FILTER((?_PhoneNumber_SubTypes_Fax = nco:FaxNumber)) .
                  }
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_MessagingCapable .
                    FILTER((?_PhoneNumber_SubTypes_MessagingCapable = nco:MessagingNumber)) .
                  }
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_Mobile .
                    FILTER((?_PhoneNumber_SubTypes_Mobile = nco:CellPhoneNumber)) .
                  }
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_Modem .
                    FILTER((?_PhoneNumber_SubTypes_Modem = nco:ModemNumber)) .
                  }
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_Pager .
                    FILTER((?_PhoneNumber_SubTypes_Pager = nco:PagerNumber)) .
                  }
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_Video .
                    FILTER((?_PhoneNumber_SubTypes_Video = nco:VideoTelephoneNumber)) .
                  }
                  OPTIONAL
                  {
                    ?_PhoneNumber rdf:type ?_PhoneNumber_SubTypes_Voice .
                    FILTER((?_PhoneNumber_SubTypes_Voice = nco:VoicePhoneNumber)) .
                  }
              }
          }
        }
        ORDER BY ?_contact LIMIT 50
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                query.distinct (true);

                RDFVariable contact ("contact");
                RDFVariable phoneNumber ("phoneNumber");
                RDFVariable phoneNumber_number ("phoneNumber_number"); 
                RDFVariable phoneNumber_sub_BulletinBoardSystem ("phoneNumber_sub_BulletinBoardSystem");
                RDFVariable phoneNumber_sub_Car ("phoneNumber_sub_Car");
                RDFVariable phoneNumber_sub_Fax ("phoneNumber_sub_Fax");
                RDFVariable phoneNumber_sub_MessagingCapable ("phoneNumber_sub_MessagingCapable");
                RDFVariable phoneNumber_sub_Mobile ("phoneNumber_sub_Mobile");
                RDFVariable phoneNumber_sub_Modem ("phoneNumber_sub_Modem");
                RDFVariable phoneNumber_sub_Pager ("phoneNumber_sub_Pager");
                RDFVariable phoneNumber_sub_Video ("phoneNumber_sub_Video");
                RDFVariable phoneNumber_sub_Voice ("phoneNumber_sub_Voice");
                RDFVariable phoneNumber_ContextWork ("phoneNumber_ContextWork");

                contact.isOfType <nco::PersonContact> ();

                RDFVariableList alternatives = contact.unionChildren (2);

                /* ---- */
                RDFVariable ua_contact = alternatives[0];
                RDFVariable ua_phoneNumber = ua_contact.variable (phoneNumber);
                RDFVariable ua_phoneNumber_number = ua_contact.variable (phoneNumber_number);

                ua_contact.property <nco::hasPhoneNumber> (ua_phoneNumber);
                ua_phoneNumber.property <nco::phoneNumber> (ua_phoneNumber_number);

                RDFVariable ua_phoneNumber_o1 = ua_phoneNumber.optional();
                RDFVariable ua_phoneNumber_sub_BulletinBoardSystem = ua_phoneNumber_o1.variable (phoneNumber_sub_BulletinBoardSystem);
                ua_phoneNumber_o1.property <rdf::type> (ua_phoneNumber_sub_BulletinBoardSystem);
                ua_phoneNumber_sub_BulletinBoardSystem == nco::BbsNumber::iri();

                RDFVariable ua_phoneNumber_o2 = ua_phoneNumber.optional();
                RDFVariable ua_phoneNumber_sub_Car = ua_phoneNumber_o2.variable (phoneNumber_sub_Car);
                ua_phoneNumber_o2.property <rdf::type> (ua_phoneNumber_sub_Car);
                ua_phoneNumber_sub_Car == nco::CarPhoneNumber::iri();

                RDFVariable ua_phoneNumber_o3 = ua_phoneNumber.optional();
                RDFVariable ua_phoneNumber_sub_Fax = ua_phoneNumber_o3.variable (phoneNumber_sub_Fax);
                ua_phoneNumber_o3.property <rdf::type> (ua_phoneNumber_sub_Fax);
                ua_phoneNumber_sub_Fax == nco::FaxNumber::iri();

                RDFVariable ua_phoneNumber_o4 = ua_phoneNumber.optional();
                RDFVariable ua_phoneNumber_sub_MessagingCapable = ua_phoneNumber_o4.variable (phoneNumber_sub_MessagingCapable);
                ua_phoneNumber_o4.property <rdf::type> (ua_phoneNumber_sub_MessagingCapable);
                ua_phoneNumber_sub_MessagingCapable == nco::MessagingNumber::iri();

                RDFVariable ua_phoneNumber_o5 = ua_phoneNumber.optional();
                RDFVariable ua_phoneNumber_sub_Mobile = ua_phoneNumber_o5.variable (phoneNumber_sub_Mobile);
                ua_phoneNumber_o5.property <rdf::type> (ua_phoneNumber_sub_Mobile);
                ua_phoneNumber_sub_Mobile == nco::CellPhoneNumber::iri();

                RDFVariable ua_phoneNumber_o6 = ua_phoneNumber.optional();
                RDFVariable ua_phoneNumber_sub_Modem = ua_phoneNumber_o6.variable (phoneNumber_sub_Modem);
                ua_phoneNumber_o6.property <rdf::type> (ua_phoneNumber_sub_Modem);
                ua_phoneNumber_sub_Modem == nco::ModemNumber::iri();

                RDFVariable ua_phoneNumber_o7 = ua_phoneNumber.optional();
                RDFVariable ua_phoneNumber_sub_Pager = ua_phoneNumber_o7.variable (phoneNumber_sub_Pager);
                ua_phoneNumber_o7.property <rdf::type> (ua_phoneNumber_sub_Pager);
                ua_phoneNumber_sub_Pager == nco::PagerNumber::iri();

                RDFVariable ua_phoneNumber_o8 = ua_phoneNumber.optional();
                RDFVariable ua_phoneNumber_sub_Video = ua_phoneNumber_o8.variable (phoneNumber_sub_Video);
                ua_phoneNumber_o8.property <rdf::type> (ua_phoneNumber_sub_Video);
                ua_phoneNumber_sub_Video == nco::VideoTelephoneNumber::iri();

                RDFVariable ua_phoneNumber_o9 = ua_phoneNumber.optional();
                RDFVariable ua_phoneNumber_sub_Voice = ua_phoneNumber_o9.variable (phoneNumber_sub_Voice);
                ua_phoneNumber_o9.property <rdf::type> (ua_phoneNumber_sub_Voice);
                ua_phoneNumber_sub_Voice == nco::VoicePhoneNumber::iri();

                /* ---- */

                RDFVariable ub_contact = alternatives[1];
                RDFVariable ub_phoneNumber = ub_contact.variable (phoneNumber);
                RDFVariable ub_phoneNumber_number = ub_contact.variable (phoneNumber_number);
                RDFVariable ub_phoneNumber_ContextWork = ub_contact.variable (phoneNumber_ContextWork);

                ub_contact.property <nco::hasAffiliation> (ub_phoneNumber_ContextWork);
                ub_phoneNumber_ContextWork.property <nco::hasPhoneNumber> (ub_phoneNumber);
                ub_phoneNumber.property <nco::phoneNumber> (ub_phoneNumber_number);

                RDFVariable ub_phoneNumber_o1 = ub_phoneNumber.optional();
                RDFVariable ub_phoneNumber_sub_BulletinBoardSystem = ub_phoneNumber_o1.variable (phoneNumber_sub_BulletinBoardSystem);
                ub_phoneNumber_o1.property <rdf::type> (ub_phoneNumber_sub_BulletinBoardSystem);
                ub_phoneNumber_sub_BulletinBoardSystem == nco::BbsNumber::iri();

                RDFVariable ub_phoneNumber_o2 = ub_phoneNumber.optional();
                RDFVariable ub_phoneNumber_sub_Car = ub_phoneNumber_o2.variable (phoneNumber_sub_Car);
                ub_phoneNumber_o2.property <rdf::type> (ub_phoneNumber_sub_Car);
                ub_phoneNumber_sub_Car == nco::CarPhoneNumber::iri();

                RDFVariable ub_phoneNumber_o3 = ub_phoneNumber.optional();
                RDFVariable ub_phoneNumber_sub_Fax = ub_phoneNumber_o3.variable (phoneNumber_sub_Fax);
                ub_phoneNumber_o3.property <rdf::type> (ub_phoneNumber_sub_Fax);
                ub_phoneNumber_sub_Fax == nco::FaxNumber::iri();

                RDFVariable ub_phoneNumber_o4 = ub_phoneNumber.optional();
                RDFVariable ub_phoneNumber_sub_MessagingCapable = ub_phoneNumber_o4.variable (phoneNumber_sub_MessagingCapable);
                ub_phoneNumber_o4.property <rdf::type> (ub_phoneNumber_sub_MessagingCapable);
                ub_phoneNumber_sub_MessagingCapable == nco::MessagingNumber::iri();

                RDFVariable ub_phoneNumber_o5 = ub_phoneNumber.optional();
                RDFVariable ub_phoneNumber_sub_Mobile = ub_phoneNumber_o5.variable (phoneNumber_sub_Mobile);
                ub_phoneNumber_o5.property <rdf::type> (ub_phoneNumber_sub_Mobile);
                ub_phoneNumber_sub_Mobile == nco::CellPhoneNumber::iri();

                RDFVariable ub_phoneNumber_o6 = ub_phoneNumber.optional();
                RDFVariable ub_phoneNumber_sub_Modem = ub_phoneNumber_o6.variable (phoneNumber_sub_Modem);
                ub_phoneNumber_o6.property <rdf::type> (ub_phoneNumber_sub_Modem);
                ub_phoneNumber_sub_Modem == nco::ModemNumber::iri();

                RDFVariable ub_phoneNumber_o7 = ub_phoneNumber.optional();
                RDFVariable ub_phoneNumber_sub_Pager = ub_phoneNumber_o7.variable (phoneNumber_sub_Pager);
                ub_phoneNumber_o7.property <rdf::type> (ub_phoneNumber_sub_Pager);
                ub_phoneNumber_sub_Pager == nco::PagerNumber::iri();

                RDFVariable ub_phoneNumber_o8 = ub_phoneNumber.optional();
                RDFVariable ub_phoneNumber_sub_Video = ub_phoneNumber_o8.variable (phoneNumber_sub_Video);
                ub_phoneNumber_o8.property <rdf::type> (ub_phoneNumber_sub_Video);
                ub_phoneNumber_sub_Video == nco::VideoTelephoneNumber::iri();

                RDFVariable ub_phoneNumber_o9 = ub_phoneNumber.optional();
                RDFVariable ub_phoneNumber_sub_Voice = ub_phoneNumber_o9.variable (phoneNumber_sub_Voice);
                ub_phoneNumber_o9.property <rdf::type> (ub_phoneNumber_sub_Voice);
                ub_phoneNumber_sub_Voice == nco::VoicePhoneNumber::iri();

                /* ---- */
                
                query.addColumn (contact);
                query.addColumn (phoneNumber);
                query.addColumn (phoneNumber_number);
                query.addColumn ("phoneNumber_sub_BulletinBoardSystem_isBound", phoneNumber_sub_BulletinBoardSystem.isBound());
                query.addColumn ("phoneNumber_sub_Car_isBound", phoneNumber_sub_Car.isBound());
                query.addColumn ("phoneNumber_sub_Fax_isBound", phoneNumber_sub_Fax.isBound());
                query.addColumn ("phoneNumber_sub_MessagingCapable_isBound", phoneNumber_sub_MessagingCapable.isBound());
                query.addColumn ("phoneNumber_sub_Mobile_isBound", phoneNumber_sub_Mobile.isBound());
                query.addColumn ("phoneNumber_sub_Modem_isBound", phoneNumber_sub_Modem.isBound());
                query.addColumn ("phoneNumber_sub_Pager_isBound", phoneNumber_sub_Pager.isBound());
                query.addColumn ("phoneNumber_sub_Video_isBound", phoneNumber_sub_Video.isBound());
                query.addColumn ("phoneNumber_sub_Voice_isBound", phoneNumber_sub_Voice.isBound());
                query.addColumn ("phoneNumber_ContextWork_isBound", phoneNumber_ContextWork.isBound());

                query.orderBy (contact);
                query.limit (50);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}
#endif

void ContactsBenchmark::test_contacts_10 ()
{
        /*
        SELECT DISTINCT
          ?contact
          ?phoneNumber
          nco:phoneNumber(?phoneNumber)
          bound(?work)
        WHERE
        {
            ?contact rdf:type nco:PersonContact .
            {
                ?contact nco:hasPhoneNumber ?phoneNumber .
            }
            UNION
            {
                ?contact nco:hasAffiliation ?work .
                ?work nco:hasPhoneNumber ?phoneNumber .
            }
        }
        ORDER BY ?_contact LIMIT 50
        */
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                query.distinct (true);

                RDFVariable contact ("contact");
                RDFVariable phoneNumber ("phoneNumber");
                RDFVariable work ("work");

                contact.isOfType <nco::PersonContact> ();

                RDFVariableList alternatives = contact.unionChildren (2);

                RDFVariable ua_contact = alternatives[0];
                RDFVariable ua_phoneNumber = ua_contact.variable (phoneNumber);

                ua_contact.property <nco::hasPhoneNumber> (ua_phoneNumber);

                RDFVariable ub_contact = alternatives[1];
                RDFVariable ub_phoneNumber = ub_contact.variable (phoneNumber);
                RDFVariable ub_work = ub_contact.variable (work);

                ub_contact.property <nco::hasAffiliation> (ub_work);
                ub_work.property <nco::hasPhoneNumber> (ub_phoneNumber);

                query.addColumn (contact);
                query.addColumn (phoneNumber);
                query.addColumn (phoneNumber.function <nco::phoneNumber> ());
                query.addColumn (work.isBound());
                query.orderBy (contact);
                query.limit (50);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}

QTEST_MAIN(ContactsBenchmark)
