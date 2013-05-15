/*
 * This file is part of LibQtTracker project
 *
 * Copyright (C) 2009, Nokia
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QtGui/QTableView>
#include <QtGui/QApplication>
#include <QtDebug>
#include <QSet>
#include <QtXml/QDomCharacterData>

#include "tracker-export-ontologies.h"
#include "getopts.h"

Importer::Importer()
{
	primeLiteralTypes();
	primeResourceTypes();
	primeAnnotations();
}

void Importer::primeLiteralTypes()
{
	literaltypes.insert("http://www.w3.org/2001/XMLSchema#string");
	literaltypes.insert("http://www.w3.org/2001/XMLSchema#integer");
	literaltypes.insert("http://www.w3.org/2001/XMLSchema#boolean");
	literaltypes.insert("http://www.w3.org/2001/XMLSchema#dateTime");
}

void Importer::primeResourceTypes()
{
	resourcetypes.insert(QUrl("http://www.w3.org/2000/01/rdf-schema#Resource"));
	resourcetypes.insert(QUrl("http://www.w3.org/2000/01/rdf-schema#Class"));
	resourcetypes.insert(QUrl("http://www.w3.org/1999/02/22-rdf-syntax-ns#Property"));
	resourcetypes.insert(QUrl("http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#InverseFunctionalProperty"));
	resourcetypes.insert(QUrl("http://www.tracker-project.org/ontologies/tracker#Namespace"));
}

void Importer::primeAnnotations()
{
	annotations["tracker:notify"] = "<sopranolive:BaseStrategyFlag>"
			"::SopranoLive::BackEnds::Tracker::Notify</sopranolive:BaseStrategyFlag>";
}

void Importer::loadResources()
{
	RDFVariable res;
	RDFVariable type = res.derivedObject<rdfs::type>();
	RDFVariable subclasses = type.optional().subject<rdfs::subClassOf>();
	subclasses.subject<rdfs::type>(res);
	subclasses.doesntExist() && !type.isMemberOf(resourcetypes);

	::tracker()->setVerbosity(3);
	resources_types = ::tracker()->modelVariable(res);
	::tracker()->setVerbosity(1);

	foreach(LiveNode res, resources_types)
		resources[res.toString()] = res->firstObject<rdfs::type>();
}

void Importer::loadClasses()
{
	foreach(LiveNode clss, ::tracker()->modelVariable(RDFVariable::fromType<rdfs::Class>()))
		classes[clss.toString()] = clss;
}

void Importer::loadProperties()
{
	foreach(LiveNode property, ::tracker()->modelVariable(RDFVariable::fromType<rdf::Property>()))
		properties[property.toString()] = property;
}

QString Importer::prefixify(QString resource) const
{
	OntologyToPrefix::const_iterator lower = ontologytoprefix.upperBound(resource);
	if(lower == ontologytoprefix.begin())
		return resource;
	--lower;
	return resource.left(lower.key().size()).replace(lower.key(), lower.value() + ":")
		+ resource.right(resource.size() - lower.key().size());
}

bool Importer::isResource(QString type) const
{
	if(literaltypes.find(type) != literaltypes.end())
		return false;
	if(resourcetypes.find(type) != resourcetypes.end())
		return true;
	qDebug() << "Unrecognized type: " << type;
	exit(1);
}

void Importer::outputMetaProperties(QTextStream &out, RDFVariable entry)
{
	RDFSelect sel;
	RDFVariable propvar = sel.newColumn("property");
	sel.addColumn("value", entry.property(propvar));
	sel.addColumn("type", propvar.property<rdfs::range>());
	LiveNodes model = ::tracker()->modelQuery(sel);
	for(int row = 0; row != model->rowCount(); ++row)
	{
		QString prop = prefixify(model->data(model->index(row, 0)).toString())
			  , value = model->data(model->index(row, 1)).toString()
			  , type = model->data(model->index(row, 2)).toString();
		if(prop == "rdf:type")
			continue;
		out << "\t\t<" << prop;
		if(isResource(type))
			out << " rdf:resource=\"" << value << "\"/>\n";
		else
		{
			out << ">";
			domdoc.createTextNode(value).save(out, 4);
			out << "</" << prop << ">\n";
			Annotations::const_iterator cia = annotations.find(prop);
			if(cia != annotations.end())
				out << "\t\t" << *cia << "\n";
		}
	}
}

void Importer::outputProperty(QTextStream &out, LiveNode const &property)
{
	QString property_name = property.toString();
	if(added_properties.find(property_name) != added_properties.end())
		return;
	added_properties.insert(property_name);
	qDebug() << "  Adding property: " << prefixify(property_name);
	out << "\t<rdf:Property rdf:about=\"" << property_name << "\">\n";
	outputMetaProperties(out, property);
	out << "\t</rdf:Property>\n";
}

void Importer::processOntology(QString prefix, QString ontology, QString filename)
{
	QFile file(filename);
	file.open(QIODevice::WriteOnly | QIODevice::Truncate);
	qDebug() << "Processing " << prefix << " = " << ontology << " to " << filename;

	QTextStream out(&file);
	// headers
	out << QString("<rdf:RDF\n");
	for(OntologyToPrefix::iterator i = ontologytoprefix.begin(); i != ontologytoprefix.end(); ++i)
		out << "\txmlns:" << i.value() << "=\"" << i.key() << "\"\n";
	out << "\txmlns:sopranolive=\"http://purl.org/sopranolive/ontologies/core#\"\n";
	out << ">\n";

	added_properties.clear();

	for(LiveNodeMap::iterator ci = classes.lowerBound(ontology)
	                      , cend = classes.lowerBound(ontology + QChar((unsigned short)32767))
		; ci != cend
		; ++ci)
	{
		qDebug() << "  Adding class: " << prefixify(ci.key());
		out << "\t<rdfs:Class rdf:about=\"" << ci->toString() << "\">\n";
		outputMetaProperties(out, *ci);
		out << "\t</rdfs:Class>\n";

		RDFVariable membervar;
		membervar.property<rdfs::domain>() = *ci;
		LiveNodeList members = ::tracker()->modelVariable(membervar);
		foreach(LiveNode member, members)
			outputProperty(out, member);
	}

	for(LiveNodeMap::iterator pi = properties.lowerBound(ontology)
	                       , pend = properties.lowerBound(ontology + QChar((unsigned short)32767))
	   ; pi != pend
	   ; ++pi)
		outputProperty(out, pi.value());

	for(LiveNodeMap::iterator ri = resources.lowerBound(ontology)
	                       , rend = resources.lowerBound(ontology + QChar((unsigned short)32767))
	   ; ri != rend
	   ; ++ri)
	{
		qDebug() << "  Adding builtin resource: " << prefixify(ri.key());
		out << "\t<" << prefixify(ri->toString()) << " rdf:about=\"" << ri.key() << "\">\n";
		out << "\t</" << prefixify(ri->toString()) << " >\n";
	}

	out << QString("</rdf:RDF>\n");
}

void Importer::processOntologies(QSet<QString> ontologynames, QString outdirectory)
{
	RDFSelect sel;
	RDFVariable prefix = sel.newColumn("prefix");
	sel.addColumn("ontology", prefix.subject(SopranoLive::Ontologies::tracker::iri("prefix")));
	QVector<QStringList> results = ::tracker()->rawExecuteQuery(sel.getQuery());
	foreach(QStringList ontology, results)
	{
		qDebug() << "Found ontology: " << ontology[0] << " = " << ontology[1];
		ontologytoprefix[ontology[1]] = ontology[0];
	}
	foreach(QStringList ontology, results)
		if(ontologynames.size() == 0 || ontologynames.find(ontology[0]) != ontologynames.end())
			processOntology(ontology[0], ontology[1], outdirectory + "/" + ontology[0] + ".xml");
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	GetOpt opts(argc, argv);

	QString verbosity;
	opts.addOptionalOption('v', "verbose", &verbosity, "1");

	QStringList ontologies;
	opts.addRemainingPositionalArguments("ontologies", &ontologies);

	QString outdirectory;
	opts.addOptionalOption('o', "out", &outdirectory, ".");

	if(!opts.parse())
	{
		qWarning() << "Usage:";
		qWarning() << "import-ontologies [-o outdirectory] [onto1 [onto2 [...]]]";
		return 1;
	}

	QSet<QString> ontoset;
	foreach(QString prefix, ontologies)
		ontoset.insert(prefix);

	::tracker()->setVerbosity(verbosity.toInt());

	Importer importer;
	importer.loadResources();
	importer.loadClasses();
	importer.loadProperties();
	importer.processOntologies(ontoset, outdirectory);
}

