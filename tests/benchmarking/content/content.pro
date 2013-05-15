include(../../../shared.pri)

QMAKE_CFLAGS+=-pg
QMAKE_CXXFLAGS+=-pg
QMAKE_LFLAGS+=-pg

TEMPLATE = app
CONFIG += qtestlib
TARGET = content-benchmark

LIBS += -lqttracker
QMAKE_RPATHDIR = $$top_srcdir/lib $$QMAKE_RPATHDIR

HEADERS += content-benchmark.h

SOURCES += content-benchmark.cpp

target.path = $$PREFIX/share/libqttracker-tests/benchmarking/content/

INSTALLS += target
