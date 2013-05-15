include(../../shared.pri)

TEMPLATE = app
TARGET = mafwsuite
CONFIG += qtestlib

QMAKE_RPATHDIR = $$top_srcdir/lib $$QMAKE_RPATHDIR

HEADERS = mafwsuite.h
SOURCES = mafwsuite.cpp
LIBS += -lqttracker

target.path = $$PREFIX/bin

INSTALLS += target

check.depends = $$TARGET
check.commands += ./mafwsuite
QMAKE_EXTRA_TARGETS += check
