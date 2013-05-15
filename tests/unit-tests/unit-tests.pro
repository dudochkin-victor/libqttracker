include(../../shared.pri)

TEMPLATE = app
CONFIG += qtestlib
TARGET = qttracker-unit-tests

INCLUDEPATH += $$top_srcdir/src

LIBS += -lqttracker
QMAKE_RPATHDIR = $$top_srcdir/lib $$QMAKE_RPATHDIR

# Test sources
HEADERS = unit-tests.h \
	livenodemodeltester.h

SOURCES += unit-tests.cpp \
	sparse-vector.cpp \
	core-suite.cpp \
	stress-suite.cpp \
	unbound-suite.cpp \
	base-suite.cpp \
	conv-suite.cpp \
	domain-suite.cpp \
	soprano-suite.cpp \
	livenodemodeltester.cpp

target.path = $$PREFIX/bin

testdata.target = data/nmm.ttl
testdata.files = $$testdata.target
testdata.depends = $$TARGET
testdata.path = $$PREFIX/share/$$PACKAGENAME-tests/data

testxml.target = tests.xml
testxml.depends = $$TARGET
testxml.commands = testhelper -d . -p $$PACKAGENAME-tests -t $$PREFIX/bin -o $$testxml.target

install_testxml.files = $$testxml.target
install_testxml.path = $$PREFIX/share/$$PACKAGENAME-tests
install_testxml.depends = $$testxml.target
install_testxml.CONFIG = no_check_exist

INSTALLS += target install_testxml testdata

QMAKE_DISTCLEAN += tests.xml

check.depends = $$TARGET
check.commands = ./qttracker-unit-tests --dbus-access && ./qttracker-unit-tests --qsparql-access
QMAKE_EXTRA_TARGETS += check testxml build_src

include(modeltest/modeltest.pri)
