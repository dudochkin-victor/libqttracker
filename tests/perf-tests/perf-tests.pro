include(../../shared.pri)

TEMPLATE = app
CONFIG += qtestlib
TARGET = qttracker-perf-tests
QT += dbus
INCLUDEPATH += $$top_srcdir/src

profile {
	LIBS += $$top_srcdir/lib/libqttracker.a
	QMAKE_LFLAGS += -pg
	QMAKE_CXXFLAGS += -pg
}
!profile {
	QMAKE_RPATHDIR = $$top_srcdir/lib $$QMAKE_RPATHDIR
	LIBS += -lqttracker
}

HEADERS = perf-tests.h
SOURCES += \
	setup.cpp \
	basic.cpp

target.path = $$PREFIX/bin

INSTALLS += target

performance_tests.commands = ./qttracker-perf-tests 5>results.txt
performance_tests.depends = $$TARGET
QMAKE_EXTRA_TARGETS += performance_tests
