#include "perf-tests.h"
#include <stdlib.h>

void PerfTests::testSuiteProcessEvents()
{
	while (QCoreApplication::hasPendingEvents())
	{
		QCoreApplication::sendPostedEvents();
		QCoreApplication::processEvents();
	}
}

void PerfTests::init()
{
	testSuiteProcessEvents();
}

void PerfTests::cleanup()
{
	testSuiteProcessEvents();
	PERF_EVENT("--");
	fsync(PERF_LOG_FD);
}

void PerfTests::initTestCase()
{
	qRegisterMetaType<QVector<QStringList> >();
	qDBusRegisterMetaType<QVector<QStringList> >();
	iterations = 1;
	if (char *i = getenv("ITERS")) {
		iterations = strtol(i, NULL, 10);
		if (iterations <= 0)
			iterations = 1;
	}
}

void PerfTests::cleanupTestCase()
{
	testSuiteProcessEvents();
	SopranoLive::BackEnds::Tracker::trackerRelease();
}

void PerfTests::waitForSignal(QObject *obj, char const *signal)
{
	QSignalSpy spy(obj, signal);
	while (QCoreApplication::hasPendingEvents() ||
		   spy.count() < 1)
	{
		QCoreApplication::sendPostedEvents();
		QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
		QCoreApplication::processEvents();
	}
}

QTEST_MAIN(PerfTests);
