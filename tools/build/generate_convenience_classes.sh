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
# generate_convenience_classes.h
# Will process all rdf-xml files *.xml under
# data/rdf-xml/QtTracker/ontologies and data/rdf-xml/soprano/ontologies
# and generate convenience classes under
# src/include/QtTracker/ontologies and src/include/soprano/ontologies resp.

# not that as only these two directories are installed
# (under <QtTracker/ontologies/*>), support for other directories is not given

(test -d "data/rdf-xml/QtTracker/ontologies") || (test -d "data/rdf-xml/soprano/ontologies") || {
  echo "Usage: generate_convenience_classes.sh"
  echo "Error: generate_convenience_classes.sh not run in fully deployed libqttracker base directory"
  echo "Reason: no data/rdf-xml/QtTracker/ontologies or data/rdf-xml/soprano/ontologies found"
  exit 1
}

(test -d "src/include/QtTracker/ontologies") || (test -d "src/include/soprano/ontologies") || {
  echo "Usage: generate_convenience_classes.sh"
  echo "Error: generate_convenience_classes.sh not run in fully deployed libqttracker base directory"
  echo "Reason: no src/include/QtTracker/ontologies or src/include/soprano/ontologies found"
  exit 1
}

(test -d "tools/build/detail") || {
  echo "Usage: generate_convenience_classes.sh"
  echo "Error: generate_convenience_classes.sh not run in fully deployed libqttracker base directory"
  echo "Reason: no tools/build/detail found"
  exit 1
}

echo "Generating convenience classes from rdf-xml ontology files"

echo "Processing data/rdf-xml/QtTracker/ontologies/*.xml"

# hack for scratchbox environment, which by default links to too-old-for-us python2.3
if [ -f /usr/bin/python2.5 ]; then
  echo "/usr/bin/python2.5 found, using it specifically"
  PYTHON=/usr/bin/python2.5
else
  echo "/usr/bin/python2.5 not found, Using python"
  PYTHON=python
fi

export SBOX_REDIRECT_IGNORE=/usr/bin/python

for ONTOLOGY_XML in data/rdf-xml/QtTracker/ontologies/*.xml
do
  TARGET=`echo $ONTOLOGY_XML | sed "s#data/rdf-xml\(.*\)\.xml#src/include\1.h#"`
  echo "Creating $TARGET"
  $PYTHON tools/build/detail/rdfxml2convenience.py -i $ONTOLOGY_XML -c QtTracker > $TARGET
done

echo "Done generating convenience classes"
  