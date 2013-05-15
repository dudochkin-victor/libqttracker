include(../../shared.pri)

TEMPLATE = app
CONFIG += qt qtestlib debug
SOURCES  += tst_sequencer.cpp

LIBS += -lqttracker
QMAKE_RPATHDIR = $$top_srcdir/lib $$QMAKE_RPATHDIR

