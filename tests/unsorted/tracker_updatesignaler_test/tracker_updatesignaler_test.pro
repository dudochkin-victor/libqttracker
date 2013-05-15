include(../tests_common.pri)
TEMPLATE = app
TARGET = tracker_updatesignaler_test
SOURCEDIR = .
DEPENDPATH += .

INCLUDEPATH += . ../../../src ../../../src/qt4 ../../../src/include
LIBS += -L../../../lib/ -lqttracker

QT += core
CONFIG += qdbus

# Input
HEADERS += tracker_updatesignaler_test.h
SOURCES += tracker_updatesignaler_test.cpp \
    main.cpp
