#!/usr/bin/python
# CreateQtTrackerTest.py - LibQtTracker QTest skeleton generator 
# Author: Iridian Kiiskinen <ext dash iridian dot kiiskinen at nokia dot com>

# Hi! 
# You can create the test by either just copypasting the stuff below into
# respective cpp and pro files, or then just python run this script with 
# a testname as argument. Either way, edit the test, and send files to me.

# python ./CreateQtTrackerTest.py testname
# edit testnameTest.cpp
# qmake
# make all regression_tests

import sys
if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.stderr.write("Usage: ./CreateQtTrackerTest.sh [testname]");
        sys.exit()
    open(sys.argv[1] + "Test.cpp", "w").write("""
#include <QtTest>
#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nco.h> // modify me

class YourTest : public QObject
{
	Q_OBJECT
private slots:
	void initTestCase() {} // called on init
	void cleanupTestCase() {} // called on cleanup

	void Test1()
	{
		QVERIFY(true); // modify and multiply me
	}
	// other test functions ...
};
QTEST_APPLESS_MAIN(YourTest)
#include "YourTest.moc"
""".replace("Your", sys.argv[1]))
# write YourTest.pro
    open(sys.argv[1] + "Test.pro", "w").write("""
TEMPLATE = app
CONFIG += qtestlib debug
QT -= gui

# Prefer the built libqttracker to the installed one.
# Using LIBS -L../../lib is not enough: qmake adds -L/usr/lib before it.
QMAKE_LIBDIR = ../../lib
QMAKE_RPATHDIR = ../../lib $$QMAKE_RPATHDIR

LIBS += -lqttracker
SOURCES += YourTest.cpp
regression_tests.commands += ./YourTest
QMAKE_EXTRA_TARGETS += regression_tests
""".replace("Your", sys.argv[1]))
