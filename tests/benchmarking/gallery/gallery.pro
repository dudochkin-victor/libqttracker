include(../../../shared.pri)

QMAKE_CFLAGS+=-pg
QMAKE_CXXFLAGS+=-pg
QMAKE_LFLAGS+=-pg

TEMPLATE = app
CONFIG += qtestlib
TARGET = gallery-benchmark

LIBS += -lqttracker
QMAKE_RPATHDIR = $$top_srcdir/lib $$QMAKE_RPATHDIR

HEADERS += gallery-benchmark.h

SOURCES += gallery-benchmark.cpp

target.path = $$PREFIX/share/libqttracker-tests/benchmarking/gallery/

INSTALLS += target
