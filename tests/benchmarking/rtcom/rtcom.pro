include(../../../shared.pri)

QMAKE_CFLAGS+=-pg
QMAKE_CXXFLAGS+=-pg
QMAKE_LFLAGS+=-pg

TEMPLATE = app
CONFIG += qtestlib
TARGET = rtcom-benchmark

LIBS += -lqttracker
QMAKE_RPATHDIR = $$top_srcdir/lib $$QMAKE_RPATHDIR

HEADERS += rtcom-benchmark.h

SOURCES += rtcom-benchmark.cpp

target.path = $$PREFIX/share/libqttracker-tests/benchmarking/rtcom/

INSTALLS += target
