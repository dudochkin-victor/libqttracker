# qmake-time variables:
#   PREFIX CONFIG
# recognized CONFIG options:
#   coverage profile timestamps noopt
# extra top-level targets:
#   coverage import_ontologies autogenerate doc dev-doc check performance_tests
TEMPLATE = subdirs

include(shared.pri)

CONFIG += ordered
SUBDIRS = src 
#tests

include(doc/doc.pri)
include(dev-doc/dev-doc.pri)

# TODO: provide a .prf file

xclean.commands = rm -rf lib/*
xclean.depends = clean

import_ontologies.commands = tools/build/import_tracker_ontologies.sh
autogenerate.commands = tools/build/generate_convenience_classes.sh

check.CONFIG = recursive
check.recurse = tests src

performance_tests.CONFIG = recursive
performance_tests.recurse = tests

QMAKE_EXTRA_TARGETS += xclean import_ontologies autogenerate check performance_tests
coverage {
	coverage-report.CONFIG = recursive
	coverage-report.recurse = src
	QMAKE_EXTRA_TARGETS += coverage-report
}
