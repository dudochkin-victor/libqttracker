TEMPLATE = app
TARGET = setandget
DEPENDPATH += .

INCLUDEPATH += . ../../../src ../../../src/qt4 ../../../src/include
LIBS += -L../../../lib/ -lqttracker

# Input
HEADERS += 
SOURCES += setandget.cpp 

CONFIG += qt debug
