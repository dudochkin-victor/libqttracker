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
import os
import sys
import getopt
import re
import datetime
import xml.etree.ElementTree as ET
from rdf_namespaces import *

# default context, can be overridden with "-c"
context = "sopranolive"

def usage():
    print "Usage: python rdfxml2convenience.py --in=rdfschema.xml --context=context"
    print "Note that the input xml format is not full rdf-xml, but a limited subset"

reservedwords = {
# c++ reserved words
   "asm": 1,  "auto": 1,  "break": 1,  "case": 1
,  "catch": 1,  "char": 1,  "class": 1,  "const": 1
,  "continue": 1,  "default": 1,  "delete": 1,  "do": 1
,  "double": 1,  "else": 1,  "enum": 1,  "extern": 1
,  "float": 1,  "for": 1,  "friend": 1,  "goto": 1
,  "if": 1,  "inline": 1,  "int": 1,  "long": 1
,  "new": 1,  "operator": 1,  "private": 1,  "protected": 1
,  "public": 1,  "register": 1,  "return": 1,  "short": 1
,  "signed": 1,  "sizeof": 1,  "static": 1,  "struct": 1
,  "switch": 1,  "template": 1,  "this": 1,  "throw": 1
,  "try": 1,  "typedef": 1,  "union": 1,  "unsigned": 1
,  "virtual": 1,  "void": 1,  "volatile": 1,  "while": 1
# SopranoLive reserved words
,  "ResourceContext": 1, "ResourceType": 1, "Ontology": 1
,  "Domain": 1, "Range": 1, "RDFDomain": 1, "RDFRange": 1
,  "BaseStrategy": 1
}

ontology_identifier_mapping = {}

def underscorify(str):
    return "_" * len(str.group(0))



#! \return a valid non-reserved c++ identifier name from given \a id which 
# doesnt conflict with other identifier names in given \a ontology. Given 
# \a id's which are not keywords and contain only ascii alphanumeric 
# characters, excluding underscore, are translated and returned directly. 
# Other ids go through mangling where all non-alphanumeric characters are 
# converted to underscores. After this, if an identical identifier exists,
# the first free prefix "_#" where # is an integer is appended. The mapping
# between ns/id -> identifier is then stored.
def cppName(ns_and_id):
    identifier = ns_and_id[1]
    if not len(identifier):
        return "";
    onto_ids = ontology_identifier_mapping.setdefault(ns_and_id[0], {})
    ret = onto_ids.get(identifier)
    if ret is None or not len(ret):
        if identifier in reservedwords:
            ret = identifier + "_"
        else:
            ret = re.match(r"[a-zA-Z][a-zA-Z0-9]*", identifier).group(0)
            if len(ret) != len(identifier):
                ret = "_" if len(re.match("\D", identifier).group(0)) == 0 else ""
                ret += re.sub(r"\W", underscorify, identifier)
        
        if onto_ids.get(ret) is not None:
            i = 1
            while onto_ids.get(ret + str(i)) is not None:
                ++i
            ret += str(i)
        # marks the id "ret" as used
        onto_ids[ret] = ""
        # set the identifier name to ret.
        # order is relevant, if ret == identifier
        onto_ids[identifier] = ret
    return ret

builtintypes = { "bool": 1
                , "int": 1, "short": 1, "long": 1
                , "char": 1
                , "float": 1, "double": 1
                , "unsigned": 1, "signed": 1 
                }

def builtinCppType(type):
    return builtintypes.get(type, False)

known_cpp_mappings = { "xsd:allNNI": ["unsigned", "unsigned"]
                     , "xsd:boolean": ["bool", "bool"]
                     , "xsd:string": ["QString", "QString"]
                     , "xsd:float": ["float", "float"]
                     , "xsd:double": ["double", "double"]
                     , "xsd:decimal": ["double", "double"]
                     , "xsd:duration": ["double", "double"]
                     , "xsd:time": ["QTime", "QTime"]
                     , "xsd:date": ["QDate", "QDate"]
                     , "xsd:gYearMonth": ["QDate", "QDate"]
                     , "xsd:gYear": ["QDate", "QDate"]
                     , "xsd:gMonthDay": ["QDate", "QDate"]
                     , "xsd:gMonth": ["QDate", "QDate"]
                     , "xsd:gDay": ["QDate", "QDate"]
                     , "xsd:dateTime": ["QDateTime", "QDateTime"]
                     , "xsd:hexBinary": ["QString", "QString"]
                     , "xsd:base64Binary": ["QString", "QString"]
                     , "xsd:anyURI": ["QUrl", "QUrl"]
                     , "xsd:QName": ["QString", "QString"]
                     , "xsd:normalizedString": ["QString", "QString"]
                     , "xsd:token": ["QString", "QString"]
                     , "xsd:language": ["QString", "QString"]
                     , "xsd:IDREFS": ["QString", "QString"]
                     , "xsd:ENTITIES": ["QString", "QString"]
                     , "xsd:NMTOKENS": ["QString", "QString"]
                     , "xsd:Name": ["QString", "QString"]
                     , "xsd:NCName": ["QString", "QString"]
                     , "xsd:ID": ["QString", "QString"]
                     , "xsd:IDREF": ["QString", "QString"]
                     , "xsd:ENTITY": ["QString", "QString"]
                     , "xsd:integer": ["qint64", "qint64"]
                     , "xsd:nonPositiveInteger": ["qint64", "qint64"]
                     , "xsd:negativeInteger": ["qint64", "qint64"]
                     , "xsd:long": ["qint64", "qint64"]
                     , "xsd:int": ["qint32", "qint32"]
                     , "xsd:short": ["qint16", "qint16"]
                     , "xsd:byte": ["qint8", "qint8"]
                     , "xsd:nonNegativeInteger": ["quint64", "quint64"]
                     , "xsd:unsignedLong": ["quint64", "quint64"]
                     , "xsd:unsignedInt": ["quint32", "quint32"]
                     , "xsd:unsignedShort": ["quint16", "quint16"]
                     , "xsd:unsignedByte": ["quint8", "quint8"]
                     , "xsd:positiveInteger": ["quint64", "quint64"]
                     }

# returns is_resource_type, cpptype, full_cpptype
def toCppType(typens, type, onto):
    if typens == onto:
        return True, type, type
    builtin_types = known_cpp_mappings.get(typens + ":" + type, None)
    if builtin_types:
        return [False] + builtin_types
    return True, type, "::SopranoLive::Ontologies::" + typens + "::" + type

class NodeInfo(object):
    def __init__(self, class_list = None, properties = None, id = None):
        self.class_list = class_list if class_list is not None else []
        self.properties = properties if properties is not None else {}
        self.comments = []
        self.member_classes = []
        self.attributes = {}
        self.traversed = False
        self.id = id
        self.deprecated = False
        self.strategy_flags = []
            
    def postProcess(self):
        None
        
    def namespace(self):
        return namespaceOfItem(self.id)[0]
    
    def name(self):
        return cppName(namespaceOfItem(self.id))
    
    def prefixedName(self):
        return ":".join(namespaceOfItem(self.id))
        
    def doxygenAnchor(self):
        return "\\anchor " + namespaceOfItem(self.id)[0] + "_" + self.name()
    
    def doxygenRef(self):
        return "\\ref " + namespaceOfItem(self.id)[0] + "_" + self.name() + " \"" + self.prefixedName() + "\""
    
    def cppIdentifier(self, current_ontology = None):
        return self.name() if current_ontology == self.namespace() else self.namespace() + "::" + self.name();
    
    def fullCppIdentifier(self, current_ontology = None):
        return "::SopranoLive::Ontologies::" + self.cppIdentifier(current_ontology)
    
    def cppConvenience(self, current_ontology = None):
        return self.cppIdentifier(current_ontology)
    
class RootInfo(NodeInfo):
    def __init__(self):
        self.defined_classes = []
        self.defined_class_index = {}
        self.dependent_class_index = {}
        
        self.forward_depends_class_index = {}
        self.property_depends_class_index = {}
        self.class_depends_class_index = {}
        
        self.forward_depends_namespaces = {}
        self.property_depends_namespaces = {}
        self.class_depends_namespaces = {}
        
        self.dependent_namespaces = {}
        self.declared_properties = {}

        self.resource_list = []

        NodeInfo.__init__(self, self.defined_classes)

root = RootInfo()
    

class ClassInfo(NodeInfo):
    def __init__(self, parent, id):
        self.inherits = []
	self.reverse_properties = {}
	self.resources = {}
        NodeInfo.__init__(self, parent.member_classes, id=id)
        
    def interfaceName(self):
        return self.name()
    
    def cppConvenience(self, current_ontology = None):
        return "::SopranoLive::Live<" + self.cppIdentifier(current_ontology) + ">"

class Subclasses(NodeInfo):
    def __init__(self, classinfo):
        NodeInfo.__init__(self, classinfo.inherits)

class Property(NodeInfo):
    def __init__(self, domainclassinfo, id = None):
        NodeInfo.__init__(self, id=id)
        self.domain = domainclassinfo
        self.range = None
        domainclassinfo.properties[self.id] = self
        self.min_cardinality = None
        self.max_cardinality = None
        
    def postProcess(self):
        self.is_singular = (self.max_cardinality == 1)
        
        if self.max_cardinality == 1:
            self.strategy_flags.append("::SopranoLive::RDFStrategy::NonMultipleValued")
        
        if self.min_cardinality == 1:
            self.strategy_flags.append("::SopranoLive::RDFStrategy::NonOptionalValued")
            
        name = cppName(namespaceOfItem(self.id))
        self.lctext = name[0:1].lower() + name[1:]
        self.uctext = name[0:1].upper() + name[1:]
                
        ns, text = namespaceOfItem(self.id)
        if ns != self.domain.namespace():
            self.lctext = ns + self.uctext
            self.uctext = ns[0:1].upper() + ns[1:] + self.uctext

        if not self.range:
            self.is_resource_type = True
            self.local_cppnode = self.local_cpptype = "LiveNode"
            self.full_cppnode = self.full_cpptype = "LiveNode"
            self.full_range = "::SopranoLive::LiveNode"
            self.full_rdf_range = "::SopranoLive::Ontologies::Rdfs::Resource"
        else:
            typens, type = namespaceOfItem(self.range.id)
            self.is_resource_type, self.local_cpptype, self.full_cpptype = toCppType(typens, type, root.namespace())
            if self.local_cpptype != self.full_cpptype:
                self.full_cpptype = self.range.cppIdentifier(root.namespace())
            self.full_range = self.full_cpptype;
            self.local_cppnode = self.local_cpptype
            self.full_cppnode = self.full_cpptype
            if self.is_resource_type:
                self.full_rdf_range = self.full_range
            else:
                self.full_rdf_range = "::SopranoLive::Ontologies::" + typens + "::" + self.range.name()
            if(self.is_resource_type):
                self.local_cppnode = "::SopranoLive::Live< " + self.local_cppnode + " >"
                self.full_cppnode = "::SopranoLive::Live< " + self.full_cppnode + " >"
                self.range.reverse_properties[self.id] = self

        if not self.is_resource_type:
            self.strategy_flags.append("::SopranoLive::RDFStrategy::Literal")
       
        self.property_type = self.local_cppnode
        self.gettype = self.local_cppnode
        self.settype = self.local_cppnode
        self.propertys_type = "LiveNodes"
        self.getstype = "LiveNodes"
        self.setstype = "LiveNodes"        
        
        self.cpp_iri = self.cppIdentifier() + "::iri()"
    
class ResourceInfo(NodeInfo):
    def __init__(self, typeinfo, id):
        NodeInfo.__init__(self, id=id)
        self.type = typeinfo
        self.type.resources[self.id] = self

def traverse(nodeinfo, child):
    
    outer_nodeinfo = nodeinfo
    tag = child.tag if child.tag else "Unknown tag"
    text = child.text if child.text else "Unknown text"
    
    if tag == "{http://www.w3.org/1999/02/22-rdf-syntax-ns#}RDF":
        None
    elif tag == "{http://www.w3.org/2000/01/rdf-schema#}Datatype":
        return
    elif tag == "{http://www.w3.org/2000/01/rdf-schema#}Class" or tag == "{http://www.w3.org/2000/01/rdf-schema#}subClassOf":
        baseclass_uri = child.get("{http://www.w3.org/1999/02/22-rdf-syntax-ns#}about")
        if tag == "{http://www.w3.org/2000/01/rdf-schema#}subClassOf":
            nodeinfo = Subclasses(nodeinfo)
            baseclass_uri = child.get("{http://www.w3.org/1999/02/22-rdf-syntax-ns#}resource")
        if baseclass_uri:
            classinfo = root.dependent_class_index.setdefault(baseclass_uri, ClassInfo(root, baseclass_uri))
            if nodeinfo is not root:
                root.class_depends_class_index[baseclass_uri] = classinfo
            nodeinfo.class_list.append(classinfo)
            nodeinfo = classinfo
    elif tag == "{http://www.w3.org/2000/01/rdf-schema#}label":
        nodeinfo.label = text
        nodeinfo.attributes["rdfs:label"] = text
    elif tag == "{http://www.w3.org/1999/02/22-rdf-syntax-ns#}Property":
        uri = child.get("{http://www.w3.org/1999/02/22-rdf-syntax-ns#}about")
        domainnode = child.find("{http://www.w3.org/2000/01/rdf-schema#}domain")
        if domainnode is None:
            return
        domainuri = domainnode.get("{http://www.w3.org/1999/02/22-rdf-syntax-ns#}resource", None)
        domainclassinfo = root.dependent_class_index.setdefault(domainuri, ClassInfo(root, domainuri))
        nodeinfo = Property(domainclassinfo, uri)
        root.declared_properties.setdefault(nodeinfo.namespace(), []).append(nodeinfo)
        root.property_depends_class_index[domainuri] = domainclassinfo
        root.property_depends_namespaces[nodeinfo.namespace()] = True
    elif tag == "{http://www.w3.org/2000/01/rdf-schema#}range":
        uri = child.get("{http://www.w3.org/1999/02/22-rdf-syntax-ns#}resource", None)
        rangeclassinfo = root.dependent_class_index.setdefault(uri, ClassInfo(root, uri))
        nodeinfo.range = rangeclassinfo
        nodeinfo.attributes["rdfs:range"] = ":".join(namespaceOfItem(uri))
        root.property_depends_class_index[uri] = rangeclassinfo
        root.property_depends_namespaces[nodeinfo.namespace()] = True
    elif tag == "{http://www.w3.org/2000/01/rdf-schema#}domain":
         # already defined with Property
        nodeinfo.attributes["rdfs:domain"] = ":".join(namespaceOfItem(child.get("{http://www.w3.org/1999/02/22-rdf-syntax-ns#}resource", "")))
    elif tag == "{http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#}maxCardinality":
        nodeinfo.max_cardinality = int(text)
        nodeinfo.attributes["nrl:maxCardinality"] = text
    elif tag == "{http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#}minCardinality":
        nodeinfo.min_cardinality = int(text)
        nodeinfo.attributes["nrl:minCardinality"] = text
    elif tag == "{http://www.w3.org/2000/01/rdf-schema#}subPropertyOf":
        nodeinfo.attributes["rdfs:subPropertyOf"] = ":".join(namespaceOfItem(child.get("{http://www.w3.org/1999/02/22-rdf-syntax-ns#}resource", "")))
    elif tag == "{http://www.w3.org/2000/01/rdf-schema#}comment":
#        nodeinfo.attributes["rdfs:comment"] = text
        nodeinfo.comments.append(text)
    elif tag == "{http://www.semanticdesktop.org/ontologies/2007/08/15/nao#}deprecated":
        nodeinfo.comments.append("This class/property has been deprecated. Other comments should contain information on the replacing practice.")
        nodeinfo.deprecated = True
    elif tag == "{http://purl.org/sopranolive/ontologies/core#}BaseStrategyFlag":
        nodeinfo.strategy_flags.append(text);
    else:
        matcher = re.match(r"{(.*)}(.*)", tag)
        if nodeinfo == root:
            class_uri = matcher.group(1) + matcher.group(2)
            class_info = root.dependent_class_index.setdefault(class_uri, ClassInfo(root, class_uri))
            res_info = ResourceInfo(class_info, child.get("{http://www.w3.org/1999/02/22-rdf-syntax-ns#}about"))
            nodeinfo.resource_list.append(res_info)
            nodeinfo = res_info
        else:
            nodeinfo.attributes[reverse_namespaces[matcher.group(1)] + ":" + matcher.group(2)] = text
#        nodeinfo.comments.append(tag + " " + text)
        
    for subchild in child:
        traverse(nodeinfo, subchild)
    
    if outer_nodeinfo != nodeinfo:
        nodeinfo.postProcess()

def printDoxygenInfo(lines, indent = 4):
    print ("\t" * indent) + "/*!"
    for line in lines:
        print ("\t" * (indent + 1)) + line
    print ("\t" * indent) + " */"
        
    
def printClassesInDependencyOrder(classinfo):
    if classinfo.traversed or classinfo.id not in root.defined_class_index:
        return
    
    classinfo.traversed = True
    
    subclassstring = ""
    doxysubclassstring = ""
    
    onto = root.ontology
    interface = classinfo.interfaceName()
    classname = classinfo.name()
    
    basenamelist = []
    
    for subclass in classinfo.inherits:
        basename = subclass.interfaceName()
        if subclass.namespace() == onto:
            printClassesInDependencyOrder(subclass)
        else:
            basename = subclass.namespace() + "::" + basename
        basenamelist.append(basename)
        
    if not len(basenamelist):
        basenamelist = ["LiveResourceBridge"]
    
    print "\t\t\t/*! Class generated from", classinfo.prefixedName()
    print "\t\t\t * \\ingroup onto_" + onto + "_classes"
    for comment in classinfo.comments:
        print "\t\t\t * " + comment 
    for attrib, value in classinfo.attributes.iteritems():
        print "\t\t\t * \n",
        if attrib == "rdfs:range":
            print attribvalue + "= \ref " + value.replace(":", "_") + " \"" + value + "\"";
        elif attrib == "rdfs:domain":
            print attribvalue + "= \ref " + value.replace(":", "_") + " \"" + value + "\"";
        else:
            print "\t\t\t * " + attrib + " = " + value
    if len(classinfo.resources):
        print "\t\t\t *" 
        print "\t\t\t * \t\\b Static \\b resources \\b of \\b this \\b class \\b type \\n"
        for id, resource in classinfo.resources.iteritems():
            print "\t\t\t * \t\t", resource.doxygenRef()
    if len(classinfo.reverse_properties):
        print "\t\t\t *" 
        print "\t\t\t * \t\\b Properties \\b having \\b this \\b class \\b as \\b Range \\n"
        for id, resource in classinfo.reverse_properties.iteritems():
            print "\t\t\t * \t\t", resource.doxygenRef()
    print "\t\t\t */" 
    print "\t\t\tclass", interface
#    baseclass = "AutoGen::LiveResourceMixin<" + interface + ", " + basenamelist.join(", ") + " >"
#    print "#ifndef GENERATING_DOXYGEN_DOCUMENTATION"
#    print "\t\t\t\t: public", baseclass
#    print "#else"
    print "\t\t\t\t: public virtual", ", public virtual ".join(basenamelist)
#    print "#endif /* GENERATING_DOXYGEN_DOCUMENTATION */"
    print
    print "\t\t\t{"
    print "//\t\t\tQ_OBJECT"
    print
    print "\t\t\tpublic:"
    if classinfo.prefixedName() != "rdfs:Resource":
        print "\t\t\t\t/*! \\return encoded iri of this class */"
        print "\t\t\t\tstatic const char *encodedIri() { return \"" + classinfo.id + "\"; }"
        print "\t\t\t\t/*! \\return iri of this class */"
        print "\t\t\t\tstatic QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }"
        if not len(basenamelist):
            print "\t\t\t\ttypedef ::SopranoLive::Detail::EmptyBase BaseClasses;"
        else:
            print ("\t\t\t\ttypedef QPair<" 
                   + ", QPair<".join(basenamelist) 
                   + ", ::SopranoLive::Detail::EmptyBase" 
                   + " >" * len(basenamelist) 
                   + " BaseClasses;");
    print
    print "\t\t\t\t//! The defining context of this class"
    print "\t\t\t\ttypedef ::SopranoLive::Contexts::" + context, "ResourceContext;"
    print
    print "\t\t\t\t//! When used as a resource, this class has a type rdfs:Class"
    print "\t\t\t\ttypedef rdfs::Class ResourceType;"
    print
    print "\t\t\t\t//! the ontology of this class"
    print "\t\t\t\ttypedef Ontology_" + onto + " Ontology;"
    print
    print "\t\t\t\t/// The base strategy of this class"
    print "\t\t\t\tenum { BaseStrategy =", (" | ".join(classinfo.strategy_flags) if len(classinfo.strategy_flags) else "0"), "};"
    print
    print "\t\t\t\t/// The base classes of this class"
    print
    print "\t\t\t\t/*!"
    print "\t\t\t\t * A smart, by-value convenience smart \\ref SopranoLive::Live pointer to this class" 
    print "\t\t\t\t */"
    print "\t\t\t\ttypedef ::SopranoLive::Live<" + interface + "> Live;"
#    print "\t\t\t\t" + interface + "(LiveResourcePtr const &proxy = LiveResourcePtr())"
#    print "\t\t\t\t\t: Mixin(proxy) {}"
    print "\t\t\t\t" + interface + "() { this->attachInterfaceHelper<", interface, ">(); }"
    print "\t\t\t\t~" + interface + "() { this->detachInterfaceHelper<", interface, ">(); }"
    print
    
    property_typedefs = {}
    
    print "\t\t\t\t/*!"
    print "\t\t\t\t * \\name Property accessor and manipulator methods"
    print "\t\t\t\t */"
    print "\t\t\t\t// @{"
        
    for name, prop in classinfo.properties.iteritems():
                
        print
        print "\t\t\t\t/* Property generated from", prop.prefixedName()
        for comment in prop.comments:
            print "\t\t\t\t * \\n", comment 
        for attrib, value in prop.attributes.iteritems():
            print "\t\t\t\t * \\n", attrib + "=" + value 
        print "\t\t\t\t */" 
        #printDoxygenInfo(["\\name Property " + prop.prefixedName(), "", "\n\t\t\t\t\\n".join(prop.comments)])
        #print "\t\t\t\t// @{"

        if prop.local_cpptype != prop.full_cpptype and prop.local_cpptype not in property_typedefs:
            print "// property api changing \t\t\ttypedef", prop.full_cpptype, prop.local_cpptype + ";"
            property_typedefs[prop.local_cpptype] = prop.full_cpptype
        
        paramname = prop.lctext + "_"
        cpptype = prop.local_cpptype if prop.namespace == onto else prop.full_cpptype
        cppnode = prop.local_cppnode if prop.namespace == onto else prop.full_cppnode  
        
        get = "get" + prop.uctext
        gets = get + "s"
        set = "set" + prop.uctext
        sets = set + "s"
        remove = "remove" + prop.uctext
	removes = remove + "s"

        first = "first" + prop.uctext
        live = "live" + prop.uctext + "s"
        add = "add" + prop.uctext
        
        getsig = cppnode + " " + get + "()"
        firstsig = cppnode + " " + first + "()"
        setsig = "void " + set + "(" + cppnode + " const &" + paramname + ")"
        rsetsig = cppnode + " " + set + "(" + cppnode + " const &" + paramname + ")"
        getssig = prop.getstype + " " + gets + "(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())"
        setssig = "void " + sets + "(" + prop.setstype + " const &" + paramname + ")"
        removessig = "void " + removes + "(RDFVariable const &object_info = RDFVariable())"
        livesig = prop.getstype + " " + live + "(RDFVariable const &object_info = RDFVariable(), RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())"
        
        if prop.is_resource_type: 
            adddefsig = cppnode + " " + add + "()"
            addsig = cppnode + " " + add + "(" + cppnode + " const &" + paramname + ")"
            removesig = "void " + remove + "(" + cppnode + " const &" + paramname + ")"
        else:
            addsig = "void " + add + "(" + cppnode + " const &" + paramname + ")"
            removesig = "void " + remove + "(" + cppnode + " const &" + paramname + ")"

        if prop.deprecated:
            dep_sig = "SOPRANO_DEPRECATED "
            dep_doc_line = "\\deprecated See \\ref " + prop.cppIdentifier() + " for replacement"
        else:
            dep_sig = ""
            dep_doc_line = ""



        #print "// property api changing \t\t\tQ_PROPERTY(" + prop.property_type, paramname, "READ", get, "WRITE", set + ")"
        #print "// property api changing \t\t\tQ_SIGNAL void", prop.lctext + "Added(" + cppnode + ");"
        #print "// property api changing \t\t\tQ_SIGNAL void", prop.lctext + "Removed(" + cppnode + ");"
        #print "// signal api changing \t\t\tQ_SIGNAL void", prop.lctext + "Changed(" + cppnode + ");"

        printDoxygenInfo(["\\fn " + livesig, "", dep_doc_line, "Returns a live model of property " + prop.doxygenRef() + " set.", "\sa LiveResource::liveObjects"])
        print "\t\t\t\t" + dep_sig + livesig + " { return baseLiveObjects<", prop.cppIdentifier(), ">(object_info, strategy, parent); }"

        if prop.min_cardinality != 1 or prop.max_cardinality != 1:
            printDoxygenInfo(["\\fn " + getssig, "", dep_doc_line, "Returns all current values of property " + prop.doxygenRef() + " set.", "\sa LiveResource::getObjects"])
            print "\t\t\t\t" + dep_sig + getssig + " { return baseGetObjects<", prop.cppIdentifier(), ">(object_info, strategy, parent); }"
            printDoxygenInfo(["\\fn " + setssig, "", dep_doc_line, "Sets the property " + prop.doxygenRef() + " set to the given \\a " + paramname + " set.", "\sa LiveResource::setObjects"])
            print "\t\t\t\t" + dep_sig + setssig + " { baseSetObjects(" + prop.cpp_iri + ", " + paramname + "); }"
            printDoxygenInfo(["\\fn " + removessig, "", dep_doc_line, "Removes the properties from property " + prop.doxygenRef() + " set which match the given \\a object_info .", "\sa LiveResource::removeObjects"])
            print "\t\t\t\t" + dep_sig + removessig + " { baseRemoveObjects(" + prop.cpp_iri + ", object_info); }"
        
        if prop.max_cardinality == 1:
            if prop.is_resource_type: 
                printDoxygenInfo(["\\fn " + getsig, "", dep_doc_line, "Returns the property " + prop.doxygenRef() + ". If the property didn't exist, it is set to a new unique resource and returned.", "\sa LiveResource::getObject"])
                print "\t\t\t\t" + dep_sig + getsig + " { return baseGetObject<", prop.cppIdentifier(), ">(); }"
                printDoxygenInfo(["\\fn " + rsetsig, "", dep_doc_line, "Sets the property " + prop.doxygenRef() + " to the given \\a " + prop.lctext + ".", "\sa LiveResource::setObject"])
                print "\t\t\t\t" + dep_sig + rsetsig + " { return baseSetObject<", prop.cppIdentifier(), ">(", paramname, "); }"
            else:
                printDoxygenInfo(["\\fn " + getsig, "", dep_doc_line, "Returns the property " + prop.doxygenRef() + ". If the property didn't exist, it is set to a default initialized value of type " + prop.gettype + " and returned.", "\sa LiveResource::getObject"])
                print "\t\t\t\t" + dep_sig + getsig + " { return baseGetObject<", prop.cppIdentifier(), ">(); }"
                printDoxygenInfo(["\\fn " + setsig, "", dep_doc_line, "Sets the property " + prop.doxygenRef() + " to the given \\a " + prop.lctext + ".", "\sa LiveResource::setObject"])
                print "\t\t\t\t" + dep_sig + setsig + " { baseSetObject<", prop.cppIdentifier(), ">(", paramname, "); }"

        if not prop.max_cardinality or prop.min_cardinality != prop.max_cardinality: 
            if prop.is_resource_type: 
                printDoxygenInfo(["\\fn " + firstsig, "", dep_doc_line, "Returns the first property " + prop.doxygenRef() + ". If the property didn't exist, a null LiveNode is returned.", "\sa LiveResource::firstObject"])
                print "\t\t\t\t" + dep_sig + firstsig + " { return baseFirstObject<", prop.cppIdentifier(), ">(); }"
                
                printDoxygenInfo(["\\fn " + adddefsig, "", dep_doc_line, "Adds a new uniquely named resource to the property " + prop.doxygenRef() + " set and returns a live version of it.", "\sa LiveResource::addObject"])
                print "\t\t\t\t" + dep_sig + adddefsig + " { return baseAddObject<", prop.cppIdentifier(), ">(); }"
                
                printDoxygenInfo(["\\fn " + addsig, "", dep_doc_line, "Adds the given \\a " + paramname + " to the property " + prop.doxygenRef() + " set and returns a live version of it.", "\sa LiveResource::addObject"])
                print "\t\t\t\t" + dep_sig + addsig + " { return baseAddObject<", prop.cppIdentifier(), ">(", paramname, "); }"
                
                printDoxygenInfo(["\\fn " + removesig, "", dep_doc_line, "Removes the given \\a " + paramname + " from the property " + prop.doxygenRef() + " set.", "\sa LiveResource::removeObject"])
                print "\t\t\t\t" + dep_sig + removesig + " { baseRemoveObject<", prop.cppIdentifier(), ">(" + paramname + "); }"
            else:
                printDoxygenInfo(["\\fn " + firstsig, "", dep_doc_line, "Returns the first property " + prop.doxygenRef() + ". If the property didn't exist, a default value is returned.", "\sa LiveResource::firstObject"])
                print "\t\t\t\t" + dep_sig + firstsig + " { return baseFirstObject<", prop.cppIdentifier(), ">(); }"
                
                printDoxygenInfo(["\\fn " + addsig, "", dep_doc_line, "Adds the given \\a " + paramname + " to the property " + prop.doxygenRef() + " set.", "\sa LiveResource::addObject"])
                print "\t\t\t\t" + dep_sig + addsig + " { baseAddObject<", prop.cppIdentifier(), ">(", paramname, "); }"
                
                printDoxygenInfo(["\\fn " + removesig, "", dep_doc_line, "Removes the given \\a " + paramname + " from the property " + prop.doxygenRef() + " set.", "\sa LiveResource::removeObject"])
                print "\t\t\t\t" + dep_sig + removesig + " { baseRemoveObject<", prop.cppIdentifier(), ">(" + paramname + "); }"
                
    print "\t\t\t\t //@}"
    print "\t\t\t};"
    print

def main():
    try:
        uri = ""
        metadatafs = list()
        servicefs = list()
        global context
        
        input = ""
    
        opts, args = getopt.getopt(sys.argv[1:], "hi:vc:v", ["help", "in=", "context="])
        for o, a in opts:
            if o in ("-i", "--in"):
                input = a
            elif o in ("-c", "--context"):
                context = a
            elif o in ("-h", "--help"):
                usage()
                sys.exit()
               
        # get an iterable
        namespace_parsing = ET.iterparse(input, events=("start", "end", "start-ns"))
        
        # turn it into an iterator
        namespace_parsing = iter(namespace_parsing)
        
	# load all namespaces
        for event, elem in namespace_parsing:
            if event == "start-ns":
                namespaces[elem[0]] = elem[1]
		reverse_namespaces[elem[1]] = elem[0]
            
        root.ontology = ontology = re.search(".*/(.*).xml", input).group(1)
        
        root.id = namespace(ontology)
        
        # fix duplicate parsing
        traverse(root, ET.parse(input).getroot())
        #traverse(root, root_elem)
                
        root.property_depends_namespaces["rdf"] = True
        root.dependent_namespaces["rdf"] = True

        for cls in root.class_list:
            root.defined_class_index[cls.id] = True
        
        for uri, dep in root.property_depends_class_index.iteritems():
            root.property_depends_namespaces[namespaceOfItem(uri)[0]] = True
            root.dependent_namespaces[namespaceOfItem(uri)[0]] = True
            
        for uri, dep in root.class_depends_class_index.iteritems():
            root.class_depends_namespaces[namespaceOfItem(uri)[0]] = True
            root.dependent_namespaces[namespaceOfItem(uri)[0]] = True
            
        print "/*"
        print " * " + ontology + ".h, autogenerated"
        print " *"
        print " * Automatically created on: " + str(datetime.datetime.today())
        print " * Automatically created by: " + " ".join(sys.argv)
        print " * DO NOT MAKE CHANGES TO THIS FILE AS THEY MIGHT BE LOST"
        print " */"
        print
        print "#ifndef SOPRANOLIVE_ONTOLOGIES_" + ontology.upper() + "_H_FORWARD_DECLARATIONS" 
        print "#define SOPRANOLIVE_ONTOLOGIES_" + ontology.upper() + "_H_FORWARD_DECLARATIONS"
        print
        print "// Namespace objects and forward declarations section"
        print
        print "#include \"../QLiveAutogenerationBase\""
        print            
        print "namespace SopranoLive"
        print "{"
        print "\tnamespace Ontologies"
        print "\t{"
        print "\t\t/*! \\defgroup onto_" + ontology + "_classes namespace " + ontology + " class definitions"
        print "\t\t *  @{"
        print "\t\t *  @}"
        print "\t\t */"
        print "\t\t/*! \\defgroup onto_" + ontology + "_properties namespace " + ontology + " property definitions"
        print "\t\t *  @{"
        print "\t\t *  @}"
        print "\t\t */"
        print "\t\t/*! \\defgroup onto_" + ontology + "_resources namespace " + ontology + " resource definitions"
        print "\t\t *  @{"
        print "\t\t *  @}"
        print "\t\t */"
        print "\t\tstatic const char " + ontology + "_namespace_prefix[] = \"" + namespaces[ontology.lower()] + "\";"
        print "\t\tstruct Ontology_" + ontology
        print "\t\t{"
        print "\t\t\t//! The defining context of this namespace"
        print "\t\t\ttypedef ::SopranoLive::Contexts::" + context, "ResourceContext;"
        print "\t\t\tstatic QUrl iri(QString suffix = QString())"
        print "\t\t\t{"
        print "\t\t\t\treturn QLatin1String(" + ontology + "_namespace_prefix) + suffix;"
        print "\t\t\t}"
        print "\t\t};"
        print "\t\tnamespace " + ontology
        print "\t\t{"
        print "\t\t\tinline QUrl iri(QString suffix) { return Ontology_" + ontology + "::iri(suffix); }"
        print
        print "\t\t\t/*!"
        print "\t\t\t * \\section classes Classes"
        print "\t\t\t */"
        print "\t\t\t//@{"
        for declare_class in root.defined_classes:
            if declare_class.namespace() == ontology:
                print "\t\t\tclass", declare_class.interfaceName() + ";"
        print "\t\t\t//@}"
        print "\t\t}"
        print "\t}"
        print "}"
        print "#endif /* SOPRANOLIVE_ONTOLOGIES_" + ontology.upper() + "_H_FORWARD_DECLARATIONS */" 
        print
        print
        print "// Property class definitions section"
        print
        print "#if (!defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY) || defined(Q_MOC_RUN))" 
        print "#ifndef SOPRANOLIVE_ONTOLOGIES_" + ontology.upper() + "_H_PROPERTY_DEFINITIONS"
        print "#define SOPRANOLIVE_ONTOLOGIES_" + ontology.upper() + "_H_PROPERTY_DEFINITIONS"
        print
        print "#ifdef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY"
        for ns in root.property_depends_namespaces.iterkeys():
            if ns != ontology:
                print "#include <" + context + "/ontologies/" + ns + ".h>"
        print "#elif defined(SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING)"
        print "#define SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY"
        for ns in root.property_depends_namespaces.iterkeys():
            if ns != ontology:
                print "#include <" + context + "/ontologies/" + ns + ".h>"
        print "#undef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY"
        print "#else"
        print "#define SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING"
        print "#define SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY"
        for ns in root.property_depends_namespaces.iterkeys():
            if ns != ontology:
                print "#include <" + context + "/ontologies/" + ns + ".h>"
        print "#undef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY"
        print "#undef SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING"
        print "#endif // SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY"
        print
        print "namespace SopranoLive"
        print "{"
        print "\tnamespace Ontologies"
        print "\t{"
        print "\t\tnamespace " + ontology
        print "\t\t{"
        print "\t\t\t/*!"
        print "\t\t\t * \\section properties Property description classes"
        print "\t\t\t */"
        print "\t\t\t//@{"
        for declare_property in root.declared_properties.get(ontology, []):
            if declare_property.namespace() == ontology:
                property_source = ":".join(namespaceOfItem(declare_property.id))
                print "\t\t\t/*! Property generated from " + property_source
                print "\t\t\t *", declare_property.doxygenAnchor()
                for comment in declare_property.comments:
                    print "\t\t\t * \\n", comment 
                for attrib, value in declare_property.attributes.iteritems():
                    print "\t\t\t * \\n", attrib + "=" + value 
                print "\t\t\t */" 
                print "\t\t\t/// \\ingroup onto_" + ontology + "_properties"
                print "\t\t\tclass", declare_property.name()
                print "\t\t\t{\tpublic:"
                print "\t\t\t\t/// \\return encoded iri of this property"
                print "\t\t\t\tstatic const char *encodedIri() { return \"" + declare_property.id + "\"; }"
                print "\t\t\t\t/// \\return iri of this property"
                print "\t\t\t\tstatic QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }" 
                print "\t\t\t\t/// The defining context of this property"
                print "\t\t\t\ttypedef ::SopranoLive::Contexts::" + context, "ResourceContext;"
                print "\t\t\t\t/// When this property class is used as a resource it has type rdf:Property"
                print "\t\t\t\ttypedef rdf::Property ResourceType;"
                print "\t\t\t\t//! The ontology of this property"
                print "\t\t\t\ttypedef Ontology_" + ontology + " Ontology;"
                print "\t\t\t\t/// The base strategy of this property. Typically just contains the multiplevalued/optionalvalud attributes."
                print "\t\t\t\tenum { BaseStrategy =", (" | ".join(declare_property.strategy_flags) if len(declare_property.strategy_flags) else "0"), "};"
                print "\t\t\t\t/// The domain/owning class of this property"
                print "\t\t\t\ttypedef", declare_property.domain.cppIdentifier(ontology), "Domain;"
                print "\t\t\t\t/// The domain/owning class of this property (for symmetry with RDFRange)"
                print "\t\t\t\ttypedef", declare_property.domain.cppIdentifier(ontology), "RDFDomain;"
                print "\t\t\t\t/// The localized c++ range type of this property"
                print "\t\t\t\ttypedef", declare_property.full_range, "Range;"
                print "\t\t\t\t/// The full, non-localized rdf range type of this property"
                print "\t\t\t\ttypedef", declare_property.full_rdf_range, "RDFRange;"
                print "\t\t\t};"
        print "\t\t\t//@}"
        print "\t\t\t/*!"
        print "\t\t\t * \\section resources Resource description classes"
        print "\t\t\t */"
        print "\t\t\t//@{"
        for declare_resource in root.resource_list:
            if declare_resource.namespace() == ontology and len(declare_resource.name()):
                resource_source = ":".join(namespaceOfItem(declare_resource.id))
                print "\t\t\t/*! Resource generated from " + resource_source
                print "\t\t\t *", declare_resource.doxygenAnchor()
                for comment in declare_resource.comments:
                    print "\t\t\t * \\n", comment 
                for attrib, value in declare_resource.attributes.iteritems():
                    print "\t\t\t * \\n", attrib + "=" + value 
                print "\t\t\t */" 
                print "\t\t\t/// \\ingroup onto_" + ontology + "_resources"
                print "\t\t\tclass", declare_resource.name()
                print "\t\t\t{\tpublic:"
                print "\t\t\t\t/// \\return encoded iri of this resource"
                print "\t\t\t\tstatic const char *encodedIri() { return \"" + declare_resource.id + "\"; }"
                print "\t\t\t\t/// \\return iri of this resource"
                print "\t\t\t\tstatic QUrl const &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }" 
                print "\t\t\t\t/// The defining context of this resource"
                print "\t\t\t\ttypedef ::SopranoLive::Contexts::" + context, "ResourceContext;"
                print "\t\t\t\t/// The type of this resource"
                print "\t\t\t\ttypedef", declare_resource.type.cppIdentifier(ontology), "ResourceType;"
                print "\t\t\t\t//! The ontology of this resource"
                print "\t\t\t\ttypedef Ontology_" + ontology + " Ontology;"
                print "\t\t\t\t/// The base strategy of this resource"
                print "\t\t\t\tenum { BaseStrategy =", (" | ".join(declare_resource.strategy_flags) if len(declare_resource.strategy_flags) else "0"), "};"
                print "\t\t\t};"
        print "\t\t\t//@}"
        print "\t\t}"
        print "\t}"
        print "}"
        print "#endif /* SOPRANOLIVE_ONTOLOGIES_" + ontology.upper() + "_H_PROPERTY_DEFINITIONS */"
        print "#endif /* (!defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY) || defined(Q_MOC_RUN) */"
        print
        print
        print "#if ( ( !defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY) \\"
        print "      && !defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY) \\"
        print "      ) || defined(Q_MOC_RUN))" 
        print "#ifndef SOPRANOLIVE_ONTOLOGIES_" + ontology.upper() + "_H_CLASS_DEFINITIONS"
        print "#define SOPRANOLIVE_ONTOLOGIES_" + ontology.upper() + "_H_CLASS_DEFINITIONS"
        print
        print "#ifdef SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING"
        print "#define SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY"
        for ns in root.property_depends_namespaces.iterkeys():
            if ns != ontology:
                print "#include <" + context + "/ontologies/" + ns + ".h>"
        print "#undef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY"
        for ns in root.class_depends_namespaces.iterkeys():
            if ns != ontology:
                print "#include <" + context + "/ontologies/" + ns + ".h>"
        print "#else // !defined(SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING)"
        print "#define SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING"
        print "#define SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY"
        for ns in root.property_depends_namespaces.iterkeys():
            if ns != ontology:
                print "#include <" + context + "/ontologies/" + ns + ".h>"
        print "#undef SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY"
        for ns in root.class_depends_namespaces.iterkeys():
            if ns != ontology:
                print "#include <" + context + "/ontologies/" + ns + ".h>"
        print "#undef SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING"
        print "#endif // SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING"
        print
        print "namespace SopranoLive"
        print "{"
        print "\tnamespace Ontologies"
        print "\t{"
        print "\t\tnamespace " + ontology
        print "\t\t{"
        print
        for ns in root.dependent_namespaces.iterkeys():
            if ns != ontology:
                print "\t\t\tusing namespace " + ns + ";"
        print
        for define_class in root.defined_classes:
            if define_class.namespace() == ontology:
                printClassesInDependencyOrder(define_class)
        print
        print "\t\t}"
        print "\t}"
        print "}"
        for define_class in root.defined_classes:
            if define_class.namespace() == ontology:
                print "Q_DECLARE_METATYPE(SopranoLive::Ontologies::" + define_class.namespace() + "::" + define_class.name() + "::Live)"
                
        print "#endif /* SOPRANOLIVE_ONTOLOGIES_" + ontology.upper() + "_H_CLASS_DEFINITIONS */"
        print "#endif /* ( ( !defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_ONLY) && !defined(SOPRANOLIVE_ONTOLOGIES_FORWARD_DECLARATIONS_AND_PROPERTY_DEFINITIONS_ONLY)) || defined(Q_MOC_RUN)) */"
        print
        print
        print "#if !defined(SOPRANOLIVE_ONTOLOGIES_TOP_LEVEL_INCLUDING) && !defined(SOPRANOLIVE_ONTOLOGIES_" + ontology.upper() + "_H_POST_INCLUDES)"
        print "#define SOPRANOLIVE_ONTOLOGIES_" + ontology.upper() + "_H_POST_INCLUDES"
        for ns in root.property_depends_namespaces.iterkeys():
            if ns != ontology:
                print "#include <" + context + "/ontologies/" + ns + ".h>"
        print "#endif"
        
    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(2)

if __name__ == "__main__":
    main()
