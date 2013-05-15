include(../../../shared.pri)

QMAKE_CFLAGS+=-pg
QMAKE_CXXFLAGS+=-pg
QMAKE_LFLAGS+=-pg

TEMPLATE = app
CONFIG += qtestlib
TARGET = gallery-profiling

LIBS += -lqttracker
QMAKE_RPATHDIR = $$top_srcdir/lib $$QMAKE_RPATHDIR

HEADERS += gallery-profiling.h \
           streaming-mode-wait.h

SOURCES += gallery-profiling.cpp

target.path = $$PREFIX/bin

INSTALLS += target
