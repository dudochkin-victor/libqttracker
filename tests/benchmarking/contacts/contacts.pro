include(../../../shared.pri)

QMAKE_CFLAGS+=-pg
QMAKE_CXXFLAGS+=-pg
QMAKE_LFLAGS+=-pg

TEMPLATE = app
CONFIG += qtestlib
TARGET = contacts-benchmark

LIBS += -lqttracker
QMAKE_RPATHDIR = $$top_srcdir/lib $$QMAKE_RPATHDIR

HEADERS += contacts-benchmark.h

SOURCES += contacts-benchmark.cpp

target.path = $$PREFIX/share/libqttracker-tests/benchmarking/contacts/

INSTALLS += target
