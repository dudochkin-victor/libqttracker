include(../../../shared.pri)

TEMPLATE = app
TARGET = tracker-export-ontologies
LIBS = -lqttracker
HEADERS = tracker-export-ontologies.h \
          getopts.h
SOURCES = tracker-export-ontologies.cpp \
          getopts.cpp

INCLUDEPATH += ../../../src/include
QMAKE_LIBDIR = ../../../lib
QMAKE_RPATHDIR = ../../../lib $$QMAKE_RPATHDIR


