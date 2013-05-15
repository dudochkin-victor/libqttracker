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

#include <location-benchmark.h>

LocationBenchmark::LocationBenchmark ()
{
    ;
}

void LocationBenchmark::test_location_01 ()
{
/*
SELECT \
  ?urn \
  ?cLat ?cLon ?cAlt ?cRad \
  ?nwLat ?nwLon ?nwAlt \
  ?seLat ?seLon ?seAlt \
  ?country ?district ?city ?street ?postalcode \
  nie:title(?urn) \
  nie:description(?urn) \
  mlo:belongsToCategory(?urn) \
WHERE { \
  ?urn a mlo:Landmark . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asPostalAddress \
            [ \
              a nco:PostalAddress ; \
              nco:country ?country ; \
              nco:region ?district ; \
              nco:locality ?city ; \
              nco:streetAddress ?street ; \
              nco:postalcode ?postalcode \
            ] \
        ] \
    } . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asBoundingBox \
            [ \
              a mlo:GeoBoundingBox ; \
              mlo:bbNorthWest \
                [ \
                  a mlo:GeoPoint ; \
                  mlo:latitude ?nwLat ; \
                  mlo:longitude ?nwLon ; \
                  mlo:altitude ?nwAlt \
                ] ; \
              mlo:bbSouthEast \
                [ \
                  a mlo:GeoPoint ; \
                  mlo:latitude ?seLat ; \
                  mlo:longitude ?seLon ; \
                  mlo:altitude ?seAlt \
                ] \
            ] \
        ] \
    } . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asGeoPoint \
            [ \
              a mlo:GeoPoint ; \
              mlo:latitude ?cLat ; \
              mlo:longitude ?cLon \
            ] \
        ] \
    } . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asGeoPoint \
            [ \
              a mlo:GeoPoint ; \
              mlo:altitude ?cAlt \
            ] \
        ] \
    } . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asGeoPoint \
            [ \
              a mlo:GeoPoint ; \
              mlo:radius ?cRad \
            ] \
        ] \
    } \
} ORDER BY ASC(?name) LIMIT 50 \
*/
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;

                RDFVariable urn ("urn");
                RDFVariable cLat ("cLat");
                RDFVariable cLon ("cLon");
                RDFVariable cAlt ("cAlt");
                RDFVariable cRad ("cRad");
                RDFVariable nwLat ("nwLat");
                RDFVariable nwLon ("nwLon");
                RDFVariable nwAlt ("nwAlt");
                RDFVariable seLat ("seLat");
                RDFVariable seLon ("seLon");
                RDFVariable seAlt ("seAlt");
                RDFVariable country ("country");
                RDFVariable district ("district");
                RDFVariable city ("city");
                RDFVariable street ("street");
                RDFVariable postalcode ("postalcode");

                urn.isOfType <mlo::Landmark> ();

                /* ---------- */

                RDFVariable oa_urn = urn.optional ();
                RDFVariable oa_country = oa_urn.variable (country);
                RDFVariable oa_district = oa_urn.variable (district);
                RDFVariable oa_city = oa_urn.variable (city);
                RDFVariable oa_street = oa_urn.variable (street);
                RDFVariable oa_postalcode = oa_urn.variable (postalcode);

                RDFVariable oa_anon1;
                RDFVariable oa_anon2;

                oa_urn.property <mlo::location> (oa_anon1);
                oa_anon1.isOfType <mlo::GeoLocation> ();
                oa_anon1.property <mlo::asPostalAddress> (oa_anon2);
                oa_anon2.isOfType <nco::PostalAddress> ();
                oa_anon2.property <nco::country> (oa_country);
                oa_anon2.property <nco::region> (oa_district);
                oa_anon2.property <nco::locality> (oa_city);
                oa_anon2.property <nco::streetAddress> (oa_street);
                oa_anon2.property <nco::postalcode> (oa_postalcode);

                /* ---------- */

                RDFVariable ob_urn = urn.optional ();
                RDFVariable ob_nwLat = ob_urn.variable (nwLat);
                RDFVariable ob_nwLon = ob_urn.variable (nwLon);
                RDFVariable ob_nwAlt = ob_urn.variable (nwAlt);
                RDFVariable ob_seLat = ob_urn.variable (seLat);
                RDFVariable ob_seLon = ob_urn.variable (seLon);
                RDFVariable ob_seAlt = ob_urn.variable (seAlt);
                RDFVariable ob_anon1;
                RDFVariable ob_anon2;
                RDFVariable ob_anon_nw;
                RDFVariable ob_anon_se;

                ob_urn.property <mlo::location> (ob_anon1);
                ob_anon1.isOfType <mlo::GeoLocation> ();
                ob_anon1.property <mlo::asBoundingBox> (ob_anon2);
                ob_anon2.isOfType <mlo::GeoBoundingBox> ();
                ob_anon2.property <mlo::bbNorthWest> (ob_anon_nw);
                ob_anon2.property <mlo::bbSouthEast> (ob_anon_se);

                ob_anon_nw.isOfType <mlo::GeoPoint> ();
                ob_anon_nw.property <mlo::latitude> (ob_nwLat);
                ob_anon_nw.property <mlo::longitude> (ob_nwLon);
                ob_anon_nw.property <mlo::altitude> (ob_nwAlt);

                ob_anon_se.isOfType <mlo::GeoPoint> ();
                ob_anon_se.property <mlo::latitude> (ob_seLat);
                ob_anon_se.property <mlo::longitude> (ob_seLon);
                ob_anon_se.property <mlo::altitude> (ob_seAlt);

                /* ---------- */

                RDFVariable oc_urn = urn.optional ();
                RDFVariable oc_cLat = oc_urn.variable (cLat);
                RDFVariable oc_cLon = oc_urn.variable (cLon);
                RDFVariable oc_anon1;
                RDFVariable oc_anon2;

                oc_urn.property <mlo::location> (oc_anon1);
                oc_anon1.isOfType <mlo::GeoLocation> ();
                oc_anon1.property <mlo::asGeoPoint> (oc_anon2);
                oc_anon2.isOfType <mlo::GeoPoint> ();
                oc_anon2.property <mlo::latitude> (oc_cLat);
                oc_anon2.property <mlo::longitude> (oc_cLon);

                /* ---------- */

                RDFVariable od_urn = urn.optional ();
                RDFVariable od_cAlt = od_urn.variable (cAlt);
                RDFVariable od_anon1;
                RDFVariable od_anon2;

                od_urn.property <mlo::location> (od_anon1);
                od_anon1.isOfType <mlo::GeoLocation> ();
                od_anon1.property <mlo::asGeoPoint> (od_anon2);
                od_anon2.isOfType <mlo::GeoPoint> ();
                od_anon2.property <mlo::altitude> (od_cAlt);

                /* ---------- */

                RDFVariable oe_urn = urn.optional ();
                RDFVariable oe_cRad = oe_urn.variable (cRad);
                RDFVariable oe_anon1;
                RDFVariable oe_anon2;

                oe_urn.property <mlo::location> (oe_anon1);
                oe_anon1.isOfType <mlo::GeoLocation> ();
                oe_anon1.property <mlo::asGeoPoint> (oe_anon2);
                oe_anon2.isOfType <mlo::GeoPoint> ();
                oe_anon2.property <mlo::radius> (oe_cRad);

                /* ---------- */

                query.addColumn (urn);
                query.addColumn (cLat);
                query.addColumn (cLon);
                query.addColumn (cAlt);
                query.addColumn (cRad);
                query.addColumn (nwLat);
                query.addColumn (nwLon);
                query.addColumn (nwAlt);
                query.addColumn (seLat);
                query.addColumn (seLon);
                query.addColumn (seAlt);
                query.addColumn (country);
                query.addColumn (district);
                query.addColumn (city);
                query.addColumn (street);
                query.addColumn (postalcode);
                query.addColumn (urn.function <nie::title> ());
                query.addColumn (urn.function <nie::description> ());
                query.addColumn (urn.function <mlo::belongsToCategory> ());

                /* ---------- */

                query.limit (50);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}

void LocationBenchmark::test_location_02 ()
{
/*
SELECT \
  ?urn \
  ?cLat ?cLon ?cAlt ?cRad \
  ?nwLat ?nwLon ?nwAlt \
  ?seLat ?seLon ?seAlt \
  ?country ?district ?city ?street ?postalcode \
  nie:title(?urn) \
  nie:description(?urn) \
  mlo:belongsToCategory(?urn) \
WHERE { \
  ?urn a mlo:Landmark . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asPostalAddress \
            [ \
              a nco:PostalAddress ; \
              nco:country ?country ; \
              nco:region ?district ; \
              nco:locality ?city ; \
              nco:streetAddress ?street ; \
              nco:postalcode ?postalcode \
            ] \
        ] \
    } . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asBoundingBox \
            [ \
              a mlo:GeoBoundingBox ; \
              mlo:bbNorthWest \
                [ \
                  a mlo:GeoPoint ; \
                  mlo:latitude ?nwLat ; \
                  mlo:longitude ?nwLon ; \
                  mlo:altitude ?nwAlt \
                ] ; \
              mlo:bbSouthEast \
                [ \
                  a mlo:GeoPoint ; \
                  mlo:latitude ?seLat ; \
                  mlo:longitude ?seLon ; \
                  mlo:altitude ?seAlt \
                ] \
            ] \
        ] \
    } . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asGeoPoint \
            [ \
              a mlo:GeoPoint ; \
              mlo:latitude ?cLat ; \
              mlo:longitude ?cLon \
            ] \
        ] \
    } . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asGeoPoint \
            [ \
              a mlo:GeoPoint ; \
              mlo:altitude ?cAlt \
            ] \
        ] \
    } . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asGeoPoint \
            [ \
              a mlo:GeoPoint ; \
              mlo:radius ?cRad \
            ] \
        ] \
    } \
  FILTER(?cLat >= 39.16 && ?cLat <= 40.17 && ?cLon >= 63.94 && ?cLon <= 64.96) \
} ORDER BY ASC(?name) LIMIT \
*/
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;

                RDFVariable urn ("urn");
                RDFVariable cLat ("cLat");
                RDFVariable cLon ("cLon");
                RDFVariable cAlt ("cAlt");
                RDFVariable cRad ("cRad");
                RDFVariable nwLat ("nwLat");
                RDFVariable nwLon ("nwLon");
                RDFVariable nwAlt ("nwAlt");
                RDFVariable seLat ("seLat");
                RDFVariable seLon ("seLon");
                RDFVariable seAlt ("seAlt");
                RDFVariable country ("country");
                RDFVariable district ("district");
                RDFVariable city ("city");
                RDFVariable street ("street");
                RDFVariable postalcode ("postalcode");

                urn.isOfType <mlo::Landmark> ();

                /* ---------- */

                RDFVariable oa_urn = urn.optional ();
                RDFVariable oa_country = oa_urn.variable (country);
                RDFVariable oa_district = oa_urn.variable (district);
                RDFVariable oa_city = oa_urn.variable (city);
                RDFVariable oa_street = oa_urn.variable (street);
                RDFVariable oa_postalcode = oa_urn.variable (postalcode);

                RDFVariable oa_anon1;
                RDFVariable oa_anon2;

                oa_urn.property <mlo::location> (oa_anon1);
                oa_anon1.isOfType <mlo::GeoLocation> ();
                oa_anon1.property <mlo::asPostalAddress> (oa_anon2);
                oa_anon2.isOfType <nco::PostalAddress> ();
                oa_anon2.property <nco::country> (oa_country);
                oa_anon2.property <nco::region> (oa_district);
                oa_anon2.property <nco::locality> (oa_city);
                oa_anon2.property <nco::streetAddress> (oa_street);
                oa_anon2.property <nco::postalcode> (oa_postalcode);

                /* ---------- */

                RDFVariable ob_urn = urn.optional ();
                RDFVariable ob_nwLat = ob_urn.variable (nwLat);
                RDFVariable ob_nwLon = ob_urn.variable (nwLon);
                RDFVariable ob_nwAlt = ob_urn.variable (nwAlt);
                RDFVariable ob_seLat = ob_urn.variable (seLat);
                RDFVariable ob_seLon = ob_urn.variable (seLon);
                RDFVariable ob_seAlt = ob_urn.variable (seAlt);
                RDFVariable ob_anon1;
                RDFVariable ob_anon2;
                RDFVariable ob_anon_nw;
                RDFVariable ob_anon_se;

                ob_urn.property <mlo::location> (ob_anon1);
                ob_anon1.isOfType <mlo::GeoLocation> ();
                ob_anon1.property <mlo::asBoundingBox> (ob_anon2);
                ob_anon2.isOfType <mlo::GeoBoundingBox> ();
                ob_anon2.property <mlo::bbNorthWest> (ob_anon_nw);
                ob_anon2.property <mlo::bbSouthEast> (ob_anon_se);

                ob_anon_nw.isOfType <mlo::GeoPoint> ();
                ob_anon_nw.property <mlo::latitude> (ob_nwLat);
                ob_anon_nw.property <mlo::longitude> (ob_nwLon);
                ob_anon_nw.property <mlo::altitude> (ob_nwAlt);

                ob_anon_se.isOfType <mlo::GeoPoint> ();
                ob_anon_se.property <mlo::latitude> (ob_seLat);
                ob_anon_se.property <mlo::longitude> (ob_seLon);
                ob_anon_se.property <mlo::altitude> (ob_seAlt);

                /* ---------- */

                RDFVariable oc_urn = urn.optional ();
                RDFVariable oc_cLat = oc_urn.variable (cLat);
                RDFVariable oc_cLon = oc_urn.variable (cLon);
                RDFVariable oc_anon1;
                RDFVariable oc_anon2;

                oc_urn.property <mlo::location> (oc_anon1);
                oc_anon1.isOfType <mlo::GeoLocation> ();
                oc_anon1.property <mlo::asGeoPoint> (oc_anon2);
                oc_anon2.isOfType <mlo::GeoPoint> ();
                oc_anon2.property <mlo::latitude> (oc_cLat);
                oc_anon2.property <mlo::longitude> (oc_cLon);

                /* ---------- */

                RDFVariable od_urn = urn.optional ();
                RDFVariable od_cAlt = od_urn.variable (cAlt);
                RDFVariable od_anon1;
                RDFVariable od_anon2;

                od_urn.property <mlo::location> (od_anon1);
                od_anon1.isOfType <mlo::GeoLocation> ();
                od_anon1.property <mlo::asGeoPoint> (od_anon2);
                od_anon2.isOfType <mlo::GeoPoint> ();
                od_anon2.property <mlo::altitude> (od_cAlt);

                /* ---------- */

                RDFVariable oe_urn = urn.optional ();
                RDFVariable oe_cRad = oe_urn.variable (cRad);
                RDFVariable oe_anon1;
                RDFVariable oe_anon2;

                oe_urn.property <mlo::location> (oe_anon1);
                oe_anon1.isOfType <mlo::GeoLocation> ();
                oe_anon1.property <mlo::asGeoPoint> (oe_anon2);
                oe_anon2.isOfType <mlo::GeoPoint> ();
                oe_anon2.property <mlo::radius> (oe_cRad);

                /* ---------- */

                cLat.greaterOrEqual (LiteralValue (39.16));
                cLat.lessOrEqual (LiteralValue (40.17));
                cLon.greaterOrEqual (LiteralValue (63.42));
                cLon.lessOrEqual (LiteralValue (64.96));

                /* ---------- */

                query.addColumn (urn);
                query.addColumn (cLat);
                query.addColumn (cLon);
                query.addColumn (cAlt);
                query.addColumn (cRad);
                query.addColumn (nwLat);
                query.addColumn (nwLon);
                query.addColumn (nwAlt);
                query.addColumn (seLat);
                query.addColumn (seLon);
                query.addColumn (seAlt);
                query.addColumn (country);
                query.addColumn (district);
                query.addColumn (city);
                query.addColumn (street);
                query.addColumn (postalcode);
                query.addColumn (urn.function <nie::title> ());
                query.addColumn (urn.function <nie::description> ());
                query.addColumn (urn.function <mlo::belongsToCategory> ());

                /* ---------- */

                query.limit (50);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}

void LocationBenchmark::test_location_03 ()
{
/*
SELECT \
  ?urn \
  ?cLat ?cLon ?cAlt ?cRad \
  ?nwLat ?nwLon ?nwAlt \
  ?seLat ?seLon ?seAlt \
  ?country ?district ?city ?street ?postalcode \
  nie:title(?urn) \
  nie:description(?urn) \
  mlo:belongsToCategory(?urn) \
  tracker:haversine-distance(xsd:double(?cLat),xsd:double(39.50),xsd:double(?cLon),xsd:double(64.50)) as ?distance \
WHERE { \
  ?urn a mlo:Landmark . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asPostalAddress \
            [ \
              a nco:PostalAddress ; \
              nco:country ?country ; \
              nco:region ?district ; \
              nco:locality ?city ; \
              nco:streetAddress ?street ; \
              nco:postalcode ?postalcode \
            ] \
        ] \
    } . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asBoundingBox \
            [ \
              a mlo:GeoBoundingBox ; \
              mlo:bbNorthWest \
                [ \
                  a mlo:GeoPoint ; \
                  mlo:latitude ?nwLat ; \
                  mlo:longitude ?nwLon ; \
                  mlo:altitude ?nwAlt \
                ] ; \
              mlo:bbSouthEast \
                [ \
                  a mlo:GeoPoint ; \
                  mlo:latitude ?seLat ; \
                  mlo:longitude ?seLon ; \
                  mlo:altitude ?seAlt \
                ] \
            ] \
        ] \
    } . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asGeoPoint \
            [ \
              a mlo:GeoPoint ; \
              mlo:latitude ?cLat ; \
              mlo:longitude ?cLon \
            ] \
        ] \
    } . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asGeoPoint \
            [ \
              a mlo:GeoPoint ; \
              mlo:altitude ?cAlt \
            ] \
        ] \
    } . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asGeoPoint \
            [ \
              a mlo:GeoPoint ; \
              mlo:radius ?cRad \
            ] \
        ] \
    } \
  FILTER(?cLat >= 39.16 && ?cLat <= 40.17 && \
         ?cLon >= 63.94 && ?cLon <= 64.96 && \
  	 tracker:haversine-distance(xsd:double(?cLat),xsd:double(39.50),xsd:double(?cLon),xsd:double(64.50)) <= 25000) \
} ORDER BY ASC(?distance) LIMIT 50 \
*/
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;

                RDFVariable urn ("urn");
                RDFVariable cLat ("cLat");
                RDFVariable cLon ("cLon");
                RDFVariable cAlt ("cAlt");
                RDFVariable cRad ("cRad");
                RDFVariable nwLat ("nwLat");
                RDFVariable nwLon ("nwLon");
                RDFVariable nwAlt ("nwAlt");
                RDFVariable seLat ("seLat");
                RDFVariable seLon ("seLon");
                RDFVariable seAlt ("seAlt");
                RDFVariable country ("country");
                RDFVariable district ("district");
                RDFVariable city ("city");
                RDFVariable street ("street");
                RDFVariable postalcode ("postalcode");

                urn.isOfType <mlo::Landmark> ();

                /* ---------- */

                RDFVariable oa_urn = urn.optional ();
                RDFVariable oa_country = oa_urn.variable (country);
                RDFVariable oa_district = oa_urn.variable (district);
                RDFVariable oa_city = oa_urn.variable (city);
                RDFVariable oa_street = oa_urn.variable (street);
                RDFVariable oa_postalcode = oa_urn.variable (postalcode);

                RDFVariable oa_anon1;
                RDFVariable oa_anon2;

                oa_urn.property <mlo::location> (oa_anon1);
                oa_anon1.isOfType <mlo::GeoLocation> ();
                oa_anon1.property <mlo::asPostalAddress> (oa_anon2);
                oa_anon2.isOfType <nco::PostalAddress> ();
                oa_anon2.property <nco::country> (oa_country);
                oa_anon2.property <nco::region> (oa_district);
                oa_anon2.property <nco::locality> (oa_city);
                oa_anon2.property <nco::streetAddress> (oa_street);
                oa_anon2.property <nco::postalcode> (oa_postalcode);

                /* ---------- */

                RDFVariable ob_urn = urn.optional ();
                RDFVariable ob_nwLat = ob_urn.variable (nwLat);
                RDFVariable ob_nwLon = ob_urn.variable (nwLon);
                RDFVariable ob_nwAlt = ob_urn.variable (nwAlt);
                RDFVariable ob_seLat = ob_urn.variable (seLat);
                RDFVariable ob_seLon = ob_urn.variable (seLon);
                RDFVariable ob_seAlt = ob_urn.variable (seAlt);
                RDFVariable ob_anon1;
                RDFVariable ob_anon2;
                RDFVariable ob_anon_nw;
                RDFVariable ob_anon_se;

                ob_urn.property <mlo::location> (ob_anon1);
                ob_anon1.isOfType <mlo::GeoLocation> ();
                ob_anon1.property <mlo::asBoundingBox> (ob_anon2);
                ob_anon2.isOfType <mlo::GeoBoundingBox> ();
                ob_anon2.property <mlo::bbNorthWest> (ob_anon_nw);
                ob_anon2.property <mlo::bbSouthEast> (ob_anon_se);

                ob_anon_nw.isOfType <mlo::GeoPoint> ();
                ob_anon_nw.property <mlo::latitude> (ob_nwLat);
                ob_anon_nw.property <mlo::longitude> (ob_nwLon);
                ob_anon_nw.property <mlo::altitude> (ob_nwAlt);

                ob_anon_se.isOfType <mlo::GeoPoint> ();
                ob_anon_se.property <mlo::latitude> (ob_seLat);
                ob_anon_se.property <mlo::longitude> (ob_seLon);
                ob_anon_se.property <mlo::altitude> (ob_seAlt);

                /* ---------- */

                RDFVariable oc_urn = urn.optional ();
                RDFVariable oc_cLat = oc_urn.variable (cLat);
                RDFVariable oc_cLon = oc_urn.variable (cLon);
                RDFVariable oc_anon1;
                RDFVariable oc_anon2;

                oc_urn.property <mlo::location> (oc_anon1);
                oc_anon1.isOfType <mlo::GeoLocation> ();
                oc_anon1.property <mlo::asGeoPoint> (oc_anon2);
                oc_anon2.isOfType <mlo::GeoPoint> ();
                oc_anon2.property <mlo::latitude> (oc_cLat);
                oc_anon2.property <mlo::longitude> (oc_cLon);

                /* ---------- */

                RDFVariable od_urn = urn.optional ();
                RDFVariable od_cAlt = od_urn.variable (cAlt);
                RDFVariable od_anon1;
                RDFVariable od_anon2;

                od_urn.property <mlo::location> (od_anon1);
                od_anon1.isOfType <mlo::GeoLocation> ();
                od_anon1.property <mlo::asGeoPoint> (od_anon2);
                od_anon2.isOfType <mlo::GeoPoint> ();
                od_anon2.property <mlo::altitude> (od_cAlt);

                /* ---------- */

                RDFVariable oe_urn = urn.optional ();
                RDFVariable oe_cRad = oe_urn.variable (cRad);
                RDFVariable oe_anon1;
                RDFVariable oe_anon2;

                oe_urn.property <mlo::location> (oe_anon1);
                oe_anon1.isOfType <mlo::GeoLocation> ();
                oe_anon1.property <mlo::asGeoPoint> (oe_anon2);
                oe_anon2.isOfType <mlo::GeoPoint> ();
                oe_anon2.property <mlo::radius> (oe_cRad);

                /* ---------- */

                cLat.greaterOrEqual (LiteralValue (39.16));
                cLat.lessOrEqual (LiteralValue (40.17));
                cLon.greaterOrEqual (LiteralValue (63.42));
                cLon.lessOrEqual (LiteralValue (64.96));

                RDFVariable dLat = cLat.function <xsd::double_> ();
                RDFVariable dLon = cLon.function <xsd::double_> ();
                RDFVariable constLat = LiteralValue (39.50);
                RDFVariable constLon = LiteralValue (64.50);
                RDFFilter haversine = dLat.filter ("tracker:haversine-distance", RDFVariableList () << constLat << dLon << constLon);

                haversine.lessOrEqual (LiteralValue (25000));

                /* ---------- */

                query.addColumn (urn);
                query.addColumn (cLat);
                query.addColumn (cLon);
                query.addColumn (cAlt);
                query.addColumn (cRad);
                query.addColumn (nwLat);
                query.addColumn (nwLon);
                query.addColumn (nwAlt);
                query.addColumn (seLat);
                query.addColumn (seLon);
                query.addColumn (seAlt);
                query.addColumn (country);
                query.addColumn (district);
                query.addColumn (city);
                query.addColumn (street);
                query.addColumn (postalcode);
                query.addColumn (urn.function <nie::title> ());
                query.addColumn (urn.function <nie::description> ());
                query.addColumn (urn.function <mlo::belongsToCategory> ());
                query.addColumn ("distance", haversine);

                /* ---------- */

                query.limit (50);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}

void LocationBenchmark::test_location_04 ()
{
/*
SELECT \
  ?urn \
  ?cLat ?cLon ?cAlt ?cRad \
  ?nwLat ?nwLon ?nwAlt \
  ?seLat ?seLon ?seAlt \
  ?country ?district ?city ?street ?postalcode \
  nie:title(?urn) \
  nie:description(?urn) \
  mlo:belongsToCategory(?urn) \
  tracker:haversine-distance(xsd:double(?cLat),xsd:double(39.50),xsd:double(?cLon),xsd:double(64.50)) as ?distance \
WHERE { \
  ?urn a mlo:Landmark . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asPostalAddress \
            [ \
              a nco:PostalAddress ; \
              nco:country ?country ; \
              nco:region ?district ; \
              nco:locality ?city ; \
              nco:streetAddress ?street ; \
              nco:postalcode ?postalcode \
            ] \
        ] \
    } . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asBoundingBox \
            [ \
              a mlo:GeoBoundingBox ; \
              mlo:bbNorthWest \
                [ \
                  a mlo:GeoPoint ; \
                  mlo:latitude ?nwLat ; \
                  mlo:longitude ?nwLon ; \
                  mlo:altitude ?nwAlt \
                ] ; \
              mlo:bbSouthEast \
                [ \
                  a mlo:GeoPoint ; \
                  mlo:latitude ?seLat ; \
                  mlo:longitude ?seLon ; \
                  mlo:altitude ?seAlt \
                ] \
            ] \
        ] \
    } . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asGeoPoint \
            [ \
              a mlo:GeoPoint ; \
              mlo:latitude ?cLat ; \
              mlo:longitude ?cLon \
            ] \
        ] \
    } . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asGeoPoint \
            [ \
              a mlo:GeoPoint ; \
              mlo:altitude ?cAlt \
            ] \
        ] \
    } . \
  OPTIONAL \
    { \
      ?urn mlo:location \
        [ \
          a mlo:GeoLocation ; \
          mlo:asGeoPoint \
            [ \
              a mlo:GeoPoint ; \
              mlo:radius ?cRad \
            ] \
        ] \
    } \
  FILTER(tracker:haversine-distance(xsd:double(?cLat),xsd:double(39.50),xsd:double(?cLon),xsd:double(64.50)) <= 25000) \
} ORDER BY ASC(?distance) LIMIT 50 \
*/
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;

                RDFVariable urn ("urn");
                RDFVariable cLat ("cLat");
                RDFVariable cLon ("cLon");
                RDFVariable cAlt ("cAlt");
                RDFVariable cRad ("cRad");
                RDFVariable nwLat ("nwLat");
                RDFVariable nwLon ("nwLon");
                RDFVariable nwAlt ("nwAlt");
                RDFVariable seLat ("seLat");
                RDFVariable seLon ("seLon");
                RDFVariable seAlt ("seAlt");
                RDFVariable country ("country");
                RDFVariable district ("district");
                RDFVariable city ("city");
                RDFVariable street ("street");
                RDFVariable postalcode ("postalcode");

                urn.isOfType <mlo::Landmark> ();

                /* ---------- */

                RDFVariable oa_urn = urn.optional ();
                RDFVariable oa_country = oa_urn.variable (country);
                RDFVariable oa_district = oa_urn.variable (district);
                RDFVariable oa_city = oa_urn.variable (city);
                RDFVariable oa_street = oa_urn.variable (street);
                RDFVariable oa_postalcode = oa_urn.variable (postalcode);

                RDFVariable oa_anon1;
                RDFVariable oa_anon2;

                oa_urn.property <mlo::location> (oa_anon1);
                oa_anon1.isOfType <mlo::GeoLocation> ();
                oa_anon1.property <mlo::asPostalAddress> (oa_anon2);
                oa_anon2.isOfType <nco::PostalAddress> ();
                oa_anon2.property <nco::country> (oa_country);
                oa_anon2.property <nco::region> (oa_district);
                oa_anon2.property <nco::locality> (oa_city);
                oa_anon2.property <nco::streetAddress> (oa_street);
                oa_anon2.property <nco::postalcode> (oa_postalcode);

                /* ---------- */

                RDFVariable ob_urn = urn.optional ();
                RDFVariable ob_nwLat = ob_urn.variable (nwLat);
                RDFVariable ob_nwLon = ob_urn.variable (nwLon);
                RDFVariable ob_nwAlt = ob_urn.variable (nwAlt);
                RDFVariable ob_seLat = ob_urn.variable (seLat);
                RDFVariable ob_seLon = ob_urn.variable (seLon);
                RDFVariable ob_seAlt = ob_urn.variable (seAlt);
                RDFVariable ob_anon1;
                RDFVariable ob_anon2;
                RDFVariable ob_anon_nw;
                RDFVariable ob_anon_se;

                ob_urn.property <mlo::location> (ob_anon1);
                ob_anon1.isOfType <mlo::GeoLocation> ();
                ob_anon1.property <mlo::asBoundingBox> (ob_anon2);
                ob_anon2.isOfType <mlo::GeoBoundingBox> ();
                ob_anon2.property <mlo::bbNorthWest> (ob_anon_nw);
                ob_anon2.property <mlo::bbSouthEast> (ob_anon_se);

                ob_anon_nw.isOfType <mlo::GeoPoint> ();
                ob_anon_nw.property <mlo::latitude> (ob_nwLat);
                ob_anon_nw.property <mlo::longitude> (ob_nwLon);
                ob_anon_nw.property <mlo::altitude> (ob_nwAlt);

                ob_anon_se.isOfType <mlo::GeoPoint> ();
                ob_anon_se.property <mlo::latitude> (ob_seLat);
                ob_anon_se.property <mlo::longitude> (ob_seLon);
                ob_anon_se.property <mlo::altitude> (ob_seAlt);

                /* ---------- */

                RDFVariable oc_urn = urn.optional ();
                RDFVariable oc_cLat = oc_urn.variable (cLat);
                RDFVariable oc_cLon = oc_urn.variable (cLon);
                RDFVariable oc_anon1;
                RDFVariable oc_anon2;

                oc_urn.property <mlo::location> (oc_anon1);
                oc_anon1.isOfType <mlo::GeoLocation> ();
                oc_anon1.property <mlo::asGeoPoint> (oc_anon2);
                oc_anon2.isOfType <mlo::GeoPoint> ();
                oc_anon2.property <mlo::latitude> (oc_cLat);
                oc_anon2.property <mlo::longitude> (oc_cLon);

                /* ---------- */

                RDFVariable od_urn = urn.optional ();
                RDFVariable od_cAlt = od_urn.variable (cAlt);
                RDFVariable od_anon1;
                RDFVariable od_anon2;

                od_urn.property <mlo::location> (od_anon1);
                od_anon1.isOfType <mlo::GeoLocation> ();
                od_anon1.property <mlo::asGeoPoint> (od_anon2);
                od_anon2.isOfType <mlo::GeoPoint> ();
                od_anon2.property <mlo::altitude> (od_cAlt);

                /* ---------- */

                RDFVariable oe_urn = urn.optional ();
                RDFVariable oe_cRad = oe_urn.variable (cRad);
                RDFVariable oe_anon1;
                RDFVariable oe_anon2;

                oe_urn.property <mlo::location> (oe_anon1);
                oe_anon1.isOfType <mlo::GeoLocation> ();
                oe_anon1.property <mlo::asGeoPoint> (oe_anon2);
                oe_anon2.isOfType <mlo::GeoPoint> ();
                oe_anon2.property <mlo::radius> (oe_cRad);

                /* ---------- */

                RDFVariable dLat = cLat.function <xsd::double_> ();
                RDFVariable dLon = cLon.function <xsd::double_> ();
                RDFVariable constLat = LiteralValue (39.50);
                RDFVariable constLon = LiteralValue (64.50);
                RDFFilter haversine = dLat.filter ("tracker:haversine-distance", RDFVariableList () << constLat << dLon << constLon);

                haversine.lessOrEqual (LiteralValue (25000));

                /* ---------- */

                query.addColumn (urn);
                query.addColumn (cLat);
                query.addColumn (cLon);
                query.addColumn (cAlt);
                query.addColumn (cRad);
                query.addColumn (nwLat);
                query.addColumn (nwLon);
                query.addColumn (nwAlt);
                query.addColumn (seLat);
                query.addColumn (seLon);
                query.addColumn (seAlt);
                query.addColumn (country);
                query.addColumn (district);
                query.addColumn (city);
                query.addColumn (street);
                query.addColumn (postalcode);
                query.addColumn (urn.function <nie::title> ());
                query.addColumn (urn.function <nie::description> ());
                query.addColumn (urn.function <mlo::belongsToCategory> ());
                query.addColumn ("distance", haversine);

                /* ---------- */

                query.limit (50);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}

void LocationBenchmark::test_location_05 ()
{
/*
SELECT \
  ?urn \
  mlo:latitude(?point) mlo:longitude(?point) mlo:altitude(?point) mlo:radius(?point) \
  nie:title(?urn) \
  nie:description(?urn) \
  mlo:belongsToCategory(?urn) \
WHERE { \
  ?urn a mlo:Landmark . \
  ?urn mlo:location ?location . \
  ?location mlo:asGeoPoint ?point . \
} ORDER BY ASC(?name) LIMIT 50 \
*/
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                RDFVariable urn ("urn");
                RDFVariable point ("point");
                RDFVariable location ("location");

                urn.isOfType <mlo::Landmark> ();
                urn.property <mlo::location> (location);
                location.property <mlo::asGeoPoint> (point);

                query.addColumn (point.function <mlo::latitude> ());
                query.addColumn (point.function <mlo::longitude> ());
                query.addColumn (point.function <mlo::altitude> ());
                query.addColumn (point.function <mlo::radius> ());
                query.addColumn (urn.function <nie::title> ());
                query.addColumn (urn.function <nie::description> ());
                query.addColumn (urn.function <mlo::belongsToCategory> ());

                query.limit (50);
                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
}

void LocationBenchmark::test_location_06 ()
{
/*
SELECT \
  ?urn \
  ?cLat ?cLon mlo:altitude(?point) mlo:radius(?point) \
  nie:title(?urn) \
  nie:description(?urn) \
  mlo:belongsToCategory(?urn) \
WHERE { \
  ?urn a mlo:Landmark . \
  ?urn mlo:location ?location . \
  ?location mlo:asGeoPoint ?point . \
  ?point mlo:latitude ?cLat . \
  ?point mlo:longitude ?cLon . \
  FILTER(?cLat >= 39.16 && ?cLat <= 40.17 && ?cLon >= 63.42 && ?cLon <= 64.96) \
} ORDER BY ASC(?name) LIMIT 50 \
*/
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                RDFVariable urn ("urn");
                RDFVariable cLat ("cLat");
                RDFVariable cLon ("cLon");
                RDFVariable point ("point");
                RDFVariable location ("location");

                urn.isOfType <mlo::Landmark> ();
                urn.property <mlo::location> (location);
                location.property <mlo::asGeoPoint> (point);
                point.property <mlo::latitude> (cLat);
                point.property <mlo::longitude> (cLon);

                cLat.greaterOrEqual (LiteralValue (39.16));
                cLat.lessOrEqual (LiteralValue (40.17));
                cLon.greaterOrEqual (LiteralValue (63.42));
                cLon.lessOrEqual (LiteralValue (64.96));

                query.addColumn (urn);
                query.addColumn (cLat);
                query.addColumn (cLon);
                query.addColumn (point.function <mlo::altitude> ());
                query.addColumn (point.function <mlo::radius> ());
                query.addColumn (urn.function <nie::title> ());
                query.addColumn (urn.function <nie::description> ());
                query.addColumn (urn.function <mlo::belongsToCategory> ());

                query.limit (50);
                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 14) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(14).arg(query_model->rowCount())));
                }
        }
}

void LocationBenchmark::test_location_07 ()
{
/*
SELECT \
  ?urn \
  ?cLat ?cLon mlo:altitude(?point) mlo:radius(?point) \
  nie:title(?urn) \
  nie:description(?urn) \
  mlo:belongsToCategory(?urn) \
  tracker:haversine-distance(xsd:double(?cLat),xsd:double(39.50),xsd:double(?cLon),xsd:double(64.50)) as ?distance \
WHERE { \
  ?urn a mlo:Landmark . \
  ?urn mlo:location ?location . \
  ?location mlo:asGeoPoint ?point . \
  ?point mlo:latitude ?cLat . \
  ?point mlo:longitude ?cLon . \
  FILTER(?cLat >= 39.16 && ?cLat <= 40.17 && \
         ?cLon >= 63.94 && ?cLon <= 64.96 && \
  	 tracker:haversine-distance(xsd:double(?cLat),xsd:double(39.50),xsd:double(?cLon),xsd:double(64.50)) <= 25000) \
} ORDER BY ASC(?distance) LIMIT 50 \
*/
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                RDFVariable urn ("urn");
                RDFVariable cLat ("cLat");
                RDFVariable cLon ("cLon");
                RDFVariable point ("point");
                RDFVariable location ("location");

                urn.isOfType <mlo::Landmark> ();
                urn.property <mlo::location> (location);
                location.property <mlo::asGeoPoint> (point);
                point.property <mlo::latitude> (cLat);
                point.property <mlo::longitude> (cLon);

                cLat.greaterOrEqual (LiteralValue (39.16));
                cLat.lessOrEqual (LiteralValue (40.17));
                cLon.greaterOrEqual (LiteralValue (63.42));
                cLon.lessOrEqual (LiteralValue (64.96));

                RDFVariable dLat = cLat.function <xsd::double_> ();
                RDFVariable dLon = cLon.function <xsd::double_> ();
                RDFVariable constLat = LiteralValue (39.50);
                RDFVariable constLon = LiteralValue (64.50);
                RDFFilter haversine = dLat.filter ("tracker:haversine-distance", RDFVariableList () << constLat << dLon << constLon);

                haversine.lessOrEqual (LiteralValue (25000));

                query.addColumn (urn);
                query.addColumn (cLat);
                query.addColumn (cLon);
                query.addColumn (point.function <mlo::altitude> ());
                query.addColumn (point.function <mlo::radius> ());
                query.addColumn (urn.function <nie::title> ());
                query.addColumn (urn.function <nie::description> ());
                query.addColumn (urn.function <mlo::belongsToCategory> ());
                query.addColumn ("distance", haversine);

                query.limit (50);
                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 14) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(14).arg(query_model->rowCount())));
                }
        }
}

void LocationBenchmark::test_location_08 ()
{
/*
SELECT \
  ?urn \
  ?cLat ?cLon mlo:altitude(?point) mlo:radius(?point) \
  nie:title(?urn) \
  nie:description(?urn) \
  mlo:belongsToCategory(?urn) \
  tracker:haversine-distance(xsd:double(?cLat),xsd:double(39.50),xsd:double(?cLon),xsd:double(64.50)) as ?distance \
WHERE { \
  ?urn a mlo:Landmark . \
  ?urn mlo:location ?location . \
  ?location mlo:asGeoPoint ?point . \
  ?point mlo:latitude ?cLat . \
  ?point mlo:longitude ?cLon . \
  FILTER(tracker:haversine-distance(xsd:double(?cLat),xsd:double(39.50),xsd:double(?cLon),xsd:double(64.50)) <= 25000) \
} ORDER BY ASC(?distance) LIMIT 50 \
*/
        //::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                RDFVariable urn ("urn");
                RDFVariable cLat ("cLat");
                RDFVariable cLon ("cLon");
                RDFVariable point ("point");
                RDFVariable location ("location");

                urn.isOfType <mlo::Landmark> ();
                urn.property <mlo::location> (location);
                location.property <mlo::asGeoPoint> (point);
                point.property <mlo::latitude> (cLat);
                point.property <mlo::longitude> (cLon);

                RDFVariable dLat = cLat.function <xsd::double_> ();
                RDFVariable dLon = cLon.function <xsd::double_> ();
                RDFVariable constLat = LiteralValue (39.50);
                RDFVariable constLon = LiteralValue (64.50);
                RDFFilter haversine = dLat.filter ("tracker:haversine-distance", RDFVariableList () << constLat << dLon << constLon);

                haversine.lessOrEqual (LiteralValue (25000));

                query.addColumn (urn);
                query.addColumn (cLat);
                query.addColumn (cLon);
                query.addColumn (point.function <mlo::altitude> ());
                query.addColumn (point.function <mlo::radius> ());
                query.addColumn (urn.function <nie::title> ());
                query.addColumn (urn.function <nie::description> ());
                query.addColumn (urn.function <mlo::belongsToCategory> ());
                query.addColumn ("distance", haversine);

                query.limit (50);
                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 14) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(14).arg(query_model->rowCount())));
                }
        }
}

QTEST_MAIN(LocationBenchmark)
