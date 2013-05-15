TEMPLATE = app
TARGET = QTrackerEmail-Test
DEPENDPATH += .

INCLUDEPATH += . ../../../src ../../../src/qt4 ../../../src/include
LIBS += -L../../../lib/ -lqttracker

# Input
HEADERS += email.h
SOURCES += email.cpp 

CONFIG += qt debug
