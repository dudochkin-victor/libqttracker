#!/bin/sh
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
#
#
# import_tracker_ontologies.sh
# Will generate ontology rdf-xml from tracker rdf introspection
# must be run in a deployed libqttracker/ base directory 

(test -d "data/rdf-xml/QtTracker") || (test -d "data/rdf-xml/soprano") || {
  echo "Error: export_tracker_ontologies.sh not run in fully deployed libqttracker base directory"
  echo "Reason: no data/rdf-xml/QtTracker or data/rdf-xml/soprano found"
  exit 1
}

echo "Generating rdf-xml ontology files from tracker introspection"

cd tools/build/tracker-export-ontologies
qmake && make
cd ../../..
tools/build/tracker-export-ontologies/tracker-export-ontologies -o data/rdf-xml/QtTracker/ontologies

echo "Done generating rdf-xml ontology files"
