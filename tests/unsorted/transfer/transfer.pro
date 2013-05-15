TEMPLATE = app
TARGET = QTrackerTransfer
DEPENDPATH += .

INCLUDEPATH += . ../../../src ../../../src/qt4 ../../../src/include
LIBS += -L../../../lib/ -lqttracker

# Input
HEADERS += transfer.h
SOURCES += transfer.cpp 

CONFIG += qt debug

