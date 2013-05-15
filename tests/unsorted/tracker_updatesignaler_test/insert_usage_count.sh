#!/bin/bash
tracker-sparql -u -q "INSERT {<$1> <http://www.semanticdesktop.org/ontologies/2007/01/19/nie#usageCounter> "$2"}"
tracker-info $1 | grep usageCounter
