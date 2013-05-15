TEMPLATE = app
TARGET = 
DEPENDPATH += .
CONFIG += qtestlib

INCLUDEPATH += . ../../../src ../../../src/qt4 ../../../src/include
LIBS += -L../../../lib/ -lqttracker
	

# Input
SOURCES += signals.cpp
HEADERS += test.h
