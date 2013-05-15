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
/*
 * tracker_queries.h
 *
 *  Created on: Jan 7, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef IMPORT_ONTOLOGIES_H_
#define IMPORT_ONTOLOGIES_H_

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/rdf.h>
#include <QtTracker/ontologies/rdfs.h>
#include <functional>

using namespace SopranoLive;

class Importer
{
public:
	typedef QMap<QString, QString> OntologyToPrefix;
	typedef QMap<QString, LiveNode> LiveNodeMap;

	QSet<QString> literaltypes;
	QSet<QUrl> resourcetypes;
	typedef QMap<QString, QString> Annotations;
	Annotations annotations;

	OntologyToPrefix ontologytoprefix;
	LiveNodeMap properties;
	LiveNodeMap classes;
	LiveNodes resources_types;
	LiveNodeMap resources;
	QDomDocument domdoc;

	QSet<QString> added_properties;

	Importer();
	void primeLiteralTypes();
	void primeResourceTypes();
	void primeAnnotations();
	bool isResource(QString type) const;
	void loadResources();
	void loadClasses();
	void loadProperties();

	QString prefixify(QString resource) const;

	void outputMetaProperties(QTextStream &out, RDFVariable entry);
	void outputProperty(QTextStream &out, LiveNode const &property);
	void processOntology(QString prefix, QString ontology, QString filename);
	void processOntologies(QSet<QString> ontologies, QString outdirectory);
};
#endif /* IMPORT_ONTOLOGIES_H_ */
