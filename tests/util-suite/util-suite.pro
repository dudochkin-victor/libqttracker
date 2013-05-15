include(../../shared.pri)

TEMPLATE = app
CONFIG += qtestlib
TARGET = qttracker-util-tests

INCLUDEPATH += $$top_srcdir/src

LIBS += -lqttracker
QMAKE_RPATHDIR = $$top_srcdir/lib $$QMAKE_RPATHDIR

# Test sources
HEADERS = trackerutil-suite.h
SOURCES = trackerutil-suite.cpp

# Units under test
SOURCES += $$top_srcdir/src/backends/tracker/trackerutil.cpp

target.path = $$PREFIX/bin

INSTALLS += target

check.depends = $$TARGET
check.commands += ./qttracker-util-tests
QMAKE_EXTRA_TARGETS += check
