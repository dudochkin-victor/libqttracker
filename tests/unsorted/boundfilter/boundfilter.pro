TEMPLATE = app
TARGET = boundfilter
DEPENDPATH += .

INCLUDEPATH += . ../../../src ../../../src/qt4 ../../../src/include
LIBS += -L../../../lib/ -lqttracker

# Input
HEADERS += 
SOURCES += boundfilter.cpp 

CONFIG += qt debug
