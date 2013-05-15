#!/usr/bin/python
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

import ConfigParser, os
import sys
import getopt

from xml.sax import saxutils
from rdf_namespaces import *


# In addition, you might want to add commonly used namespaces to namespaces list
def usage():
    print "Usage: python service2rdfxml.py --ontology=ONTOLOGY --metadata=ONTOLOGY.metadata --service=ONTOLOGY.service [--uri=URI] [--namespace=ns=URI ] [--verbose]"

def main():
    try:
        default_ns = ""
        metadatafs = list()
        servicefs = list()
        
        verbosity = 1
        
        ontology = ""
    
        opts, args = getopt.getopt(sys.argv[1:], "hvu:vo:va:vm:vs:vn:v", ["help", "verbose", "uri=", "ontology=", "metadata=", "service=", "namespace="])
    
        for o, a in opts:
            if o in ("-u", "--uri"):
                default_ns = a
            elif o in ("-o", "--ontology"):
                ontology = a
            elif o in ("-m", "--metadata"):
                metadatafs.append(a)
            elif o in ("-s", "--service"):
                servicefs.append(a)
            elif o in ("-n", "--namespace"):
                namespaces[a.split("=",1)[0]] = a.split("=",1)[1]
            elif o in ("-h", "--help"):
                usage()
                sys.exit()
            elif o in ("-v", "--verbose"):
                verbosity += 1
    
        if default_ns == "":
            default_ns = "http://www.tracker-project.org/ontologies/tracker#"
    
        if not len(metadatafs) and not len(servicefs):
            usage()
            sys.exit()
          
        
        print "<rdf:RDF"
        for ns, url in namespaces.iteritems():
            print "  xmlns:" + ns + "=\"" + url + "\""
        print ">"
        print ""
        
        registered_klasses = {}
    
        for servicef in servicefs:
            service = ConfigParser.ConfigParser()
            service.readfp(open(servicef))
    
            for klass in service.sections():
                try:
                    klass_uri, prefix, suffix = namespaceItem(klass, default_ns);
                except:
                    sys.stderr.write("Dropping a class with unrecognized namespace: " + klass + "\n")
                    continue
                    
                registered_klasses[klass_uri] = True
                print "\t<rdfs:Class rdf:about=\"" + klass_uri + "\">"
                print "\t\t<rdfs:label>" + suffix + "</rdfs:label>"
      
                for name, value in service.items(klass):
                    if name == "superclasses":
                        superclasses = value.split (";")
                        for superclass in superclasses:
                            if len(superclass.strip()):
                                print "\t\t<rdfs:subClassOf>"
                                print "\t\t\t<rdfs:Class rdf:about=\"" + namespaceItem(superclass, default_ns)[0] + "\"/>"
                                print "\t\t</rdfs:subClassOf>"
                print "\t</rdfs:Class>"
    
        for metadataf in metadatafs:
            metadata = ConfigParser.ConfigParser()
            metadata.readfp(open(metadataf))
     
            for mdata in metadata.sections():
                try:
                    property_uri, mns, mfragment = namespaceItem(mdata, default_ns)
                except:
                    if verbosity >= 2:
                        sys.stderr.write("Dropping a property with unrecognized namespace: " + mdata + "\n")
                    continue
                
                nodes = []
                domain = None
                multiplevalues = False
      
                for name, value in metadata.items(mdata):
                    if name == "displayname":
                        None
                        
                    elif name == "multiplevalues" and value == "true":
                        multiplevalues = True
                        
                    elif name == "datatype":
                        if value != "resource": # TODO: remove temp hack, 0.2.4 specific code
                            nodes.append("\t\t<rdfs:range rdf:resource=\"" + namespaceItem(value, default_ns)[0] + "\"/>")
      
                    elif name == "domain":
                        domain = namespaceItem(value, default_ns)[0]
                        nodes.append("\t\t<rdfs:domain rdf:resource=\"" + domain + "\"/>")
      
                    elif name == "parent":
                        nodes.append("\t\t<rdfs:subPropertyOf rdf:resource=\"" + namespaceItem(value, default_ns)[0] + "\"/>")
          
                    elif name == "multiplevalues" and value == "true":
                        multiplevalues = True
                        
                    elif name == "superproperties":
                        superproperties = value.split (";")
                        for superproperty in superproperties:
                            if len(superproperty):
                                nodes.append("\t\t<rdfs:subPropertyOf rdf:resource=\"" + namespaceItem(superproperty, default_ns)[0] + "\"/>")
                    else: 
                        nodes.append("\t\t<rdfs:comment>" + name + ": " + saxutils.escape(value) + "</rdfs:comment>")
                        
                if domain in registered_klasses or namespaceOfItem(property_uri)[0] == ontology:
                    print "\t<rdf:Property rdf:about=\"" + property_uri + "\">"
                    
                    print "\t\t<rdfs:label>" + mfragment + "</rdfs:label>"
                    
                    for node in nodes:
                        print node
                        
                    if not multiplevalues:
                        print "\t\t<nrl:MaxCardinality>1</nrl:MaxCardinality>"
                    print "\t</rdf:Property>"
      
        print "</rdf:RDF>"
    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(2)

if __name__ == "__main__":
    main()
