include(../../../shared.pri)

QMAKE_CFLAGS+=-pg
QMAKE_CXXFLAGS+=-pg
QMAKE_LFLAGS+=-pg

TEMPLATE = app
CONFIG += qtestlib
TARGET = email-benchmark

LIBS += -lqttracker
QMAKE_RPATHDIR = $$top_srcdir/lib $$QMAKE_RPATHDIR

HEADERS += email-benchmark.h

SOURCES += email-benchmark.cpp

target.path = $$PREFIX/share/libqttracker-tests/benchmarking/email/

INSTALLS += target
