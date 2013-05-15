#
# This file is part of LibQtTracker project
#
# Copyright (C) 2009, Nokia
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with this library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301, USA.
#
# Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>

 
 
namespaces = {}
reverse_namespaces = {}

# Adding stuff here no longer necessary!
# scripts automatically read ontologies now from the rdf xml.
# if there are problems, rename below line to "namespaces = \" and remove above line
namespaces_disabled = \
{ "rdf": "http://www.w3.org/1999/02/22-rdf-syntax-ns#"
, "rdfs": "http://www.w3.org/2000/01/rdf-schema#"
, "xsd": "http://www.w3.org/2001/XMLSchema#"
, "dc": "http://purl.org/dc/elements/1.1/"
, "nrl": "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#"
, "nie": "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#"
, "nae": "http://maemo.org/ontologies/nae#"
, "nao": "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#"
, "nco": "http://www.semanticdesktop.org/ontologies/2007/03/22/nco#"
, "nfo": "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#"
, "nmo": "http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#"
, "ncal": "http://www.semanticdesktop.org/ontologies/2007/04/02/ncal#"
, "nid3": "http://www.semanticdesktop.org/ontologies/2007/05/10/nid3#"
, "nmm": "http://www.tracker-project.org/temp/nmm#"
, "mfo": "http://www.tracker-project.org/temp/mfo#"
, "mlo": "http://www.tracker-project.org/temp/mlo#"
, "mto": "http://www.tracker-project.org/temp/mto#"
, "tracker": "http://www.tracker-project.org/ontologies/tracker#"
, "maemo": "http://maemo.org/ontologies/tracker#"
, "fts": "http://www.tracker-project.org/ontologies/fts#"
, "poi": "http://www.tracker-project.org/ontologies/poi#"
, "exif": "http://www.kanzaki.com/ns/exif#"
, "protege": "http://protege.stanford.edu/system#"
, "dcterms": "http://purl.org/dc/terms/"
, "pimo": "http://www.semanticdesktop.org/ontologies/2007/11/01/pimo#"
, "geo": "http://www.w3.org/2003/01/geo/wgs84_pos#"
, "tmo": "http://www.semanticdesktop.org/ontologies/2008/05/20/tmo#"
, "nexif": "http://www.semanticdesktop.org/ontologies/2007/05/10/nexif#"
}

trackertypes = { "string": ["xsd", "string"]
                , "boolean": ["xsd", "boolean"]
                , "double": ["xsd", "decimal"]
                , "integer": ["xsd", "integer"]
                , "date": ["xsd", "dateTime"]
                , "blob": ["xsd", "string"]
                , "resource": ["rdfs", "Resource"]
                , "fulltext": ["xsd", "string"]
                }

def namespace(ns, ns_default_uri = None):
    if(ns.lower() not in namespaces):
        if(ns_default_uri is None):
            raise Exception("rdf_namespaces: unrecognized namespace " + ns)
        else:
            return ns_default_uri
    return namespaces[ns.lower()]
    
def namespaceItem(name, ns_default_uri = None):
    split = name.split(":")
    if len(split) == 1:
        ns, frag = trackertypes.get(name, [None, name])
        if not ns:
            if not ns_default_uri:
                raise Exception("service2rdfxml: no default namespace defined while supplying an item with no namespace suffix and item is not a tracker builtin")
            return ns_default_uri + name, "", name
    else:
        ns, frag = split
    return namespace(ns) + frag, ns, frag

def namespaceOfItem(name, default = ""):
    res_ns = default
    res_uri = ""
    for ns, uri in namespaces.iteritems():
        if name[0:len(uri)] == uri and len(res_uri) < len(uri):
            res_ns = ns
            res_uri = uri
    return res_ns, name[len(res_uri):]
