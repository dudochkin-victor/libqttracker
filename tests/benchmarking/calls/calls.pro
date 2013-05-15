include(../../../shared.pri)

QMAKE_CFLAGS+=-pg
QMAKE_CXXFLAGS+=-pg
QMAKE_LFLAGS+=-pg

TEMPLATE = app
CONFIG += qtestlib
TARGET = calls-benchmark

LIBS += -lqttracker
QMAKE_RPATHDIR = $$top_srcdir/lib $$QMAKE_RPATHDIR

HEADERS += calls-benchmark.h

SOURCES += calls-benchmark.cpp

target.path = $$PREFIX/share/libqttracker-tests/benchmarking/calls/

INSTALLS += target
