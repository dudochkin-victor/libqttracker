TEMPLATE = app
TARGET = Test
DEPENDPATH += .

INCLUDEPATH += . ../../src ../../src/qt4 ../../src/include
# Prefer the built libqttracker to the installed one.
# Using LIBS -L../../lib is not enough: qmake adds -L/usr/lib before it.
QMAKE_LIBDIR = ../../lib
QMAKE_RPATHDIR = ../../lib $$QMAKE_RPATHDIR

LIBS += -lqttracker

# Input
HEADERS += 
SOURCES += test.cpp 

CONFIG += qt debug qtestlib
QT += dbus
