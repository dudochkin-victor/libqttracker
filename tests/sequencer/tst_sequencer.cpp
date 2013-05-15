/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QDebug>


#include "../../src/sequencer.h"

//TESTED_CLASS=
//TESTED_FILES=

using namespace SopranoLive;

class Signaler : public QObject
{
	Q_OBJECT
public:
	Signaler()
		: started_(false) {}

	bool isCycling() { return started_; }

	void emitCycleSuccess()
	{
		emit cycleSucceeded();
	}
	void emitCycleFailure()
	{
		emit cycleFailed();
	}

	void sequenceCycle
			( Sequencer *seq, int id, QSet<QString> const &qualities, DependencyChain const &dependencies)
	{
		seq->addOperation
				( qualities, dependencies
				, SequencerSlotCall(this, "cycleStart") << Q_ARG(int, id)
				, SequencerSignal(this, SIGNAL(cycleSucceeded()))
				, SequencerSignal(this, SIGNAL(cycleFailed())));
	}

Q_SIGNALS:
	void cycleSucceeded();
	void cycleFailed();

public Q_SLOTS:
	void cycleStart(int id)
	{
		qDebug() << "Started sequence #" << id;
		started_ = true;
	}

private:
	bool started_;
};

class tst_Sequencer : public QObject
{
    Q_OBJECT

public:
    tst_Sequencer()
    {}

public slots:
    void init()
    {}

    void opStateChange(SequencedOperation *op)
    {
    	qDebug() << "Operation state change:" << op << op->state();
    }

private slots:
	void dummy()
	{}

    void basic()
    {
		qDebug() << "------------------------ Step 1";
        Sequencer seq;

        connect(&seq, SIGNAL(started(SequencedOperation *)), SLOT(opStateChange(SequencedOperation *)));
        connect(&seq, SIGNAL(succeeded(SequencedOperation *)), SLOT(opStateChange(SequencedOperation *)));
        connect(&seq, SIGNAL(failed(SequencedOperation *)), SLOT(opStateChange(SequencedOperation *)));
        {

			Signaler signalers[10];
			qDebug() << "------------------------ Step 2";

			signalers[0].sequenceCycle
					( &seq, 0, QSet<QString>() << "read"
					, DependencyChain().latestStarted("read").allFinished("write"));
			qDebug() << "------------------------ Step 3";
			signalers[1].sequenceCycle
					( &seq, 1, QSet<QString>() << "write"
					, DependencyChain().latestStarted("write").allFinished("read"));
			signalers[2].sequenceCycle
					( &seq, 2, QSet<QString>() << "write"
					, DependencyChain().latestStarted("write").allFinished("read"));
			signalers[3].sequenceCycle
					( &seq, 3, QSet<QString>() << "write"
					, DependencyChain().latestStarted("write").allFinished("read"));
			signalers[4].sequenceCycle
					( &seq, 4, QSet<QString>() << "read"
					, DependencyChain().latestStarted("read").allFinished("write"));
			signalers[5].sequenceCycle
					( &seq, 5, QSet<QString>() << "read"
					, DependencyChain().latestStarted("read").allFinished("write"));
			signalers[6].sequenceCycle
					( &seq, 6, QSet<QString>() << "write"
					, DependencyChain().latestStarted("write").allFinished("read"));
			signalers[7].sequenceCycle
					( &seq, 7, QSet<QString>() << "read"
					, DependencyChain().latestStarted("read").allFinished("write"));
			signalers[8].sequenceCycle
					( &seq, 8, QSet<QString>() << "read"
					, DependencyChain().latestStarted("read").allFinished("write"));
			signalers[9].sequenceCycle
					( &seq, 9, QSet<QString>() << "write"
					, DependencyChain().latestStarted("write").allFinished("read"));

			QCOMPARE(signalers[0].isCycling(), true);

			QSignalSpy spy(&signalers[0], SIGNAL(cycleSucceeded()));
			qDebug() << "------------------------ Step 4";

			signalers[0].emitCycleSuccess();

			QCOMPARE(spy.size(), 1);

			QCOMPARE(signalers[1].isCycling(), true);
			QCOMPARE(signalers[2].isCycling(), true);
			QCOMPARE(signalers[3].isCycling(), true);
			QCOMPARE(signalers[4].isCycling(), false);
			QCOMPARE(signalers[6].isCycling(), false);

			signalers[1].emitCycleSuccess();

			QCOMPARE(signalers[4].isCycling(), false);
			QCOMPARE(signalers[6].isCycling(), false);

			signalers[2].emitCycleSuccess();

			QCOMPARE(signalers[4].isCycling(), false);
			QCOMPARE(signalers[6].isCycling(), false);

			signalers[3].emitCycleSuccess();

			QCOMPARE(signalers[4].isCycling(), true);
			QCOMPARE(signalers[5].isCycling(), true);
			QCOMPARE(signalers[6].isCycling(), false);
			QCOMPARE(signalers[7].isCycling(), false);
			QCOMPARE(signalers[8].isCycling(), false);
			QCOMPARE(signalers[8].isCycling(), false);

			signalers[5].emitCycleFailure();

			QCOMPARE(signalers[6].isCycling(), false);
			QCOMPARE(signalers[7].isCycling(), false);
			QCOMPARE(signalers[8].isCycling(), false);
			QCOMPARE(signalers[9].isCycling(), false);

			signalers[4].emitCycleSuccess();

			QCOMPARE(signalers[6].isCycling(), true);
			QCOMPARE(signalers[7].isCycling(), false);
			QCOMPARE(signalers[8].isCycling(), false);
			QCOMPARE(signalers[9].isCycling(), false);

			signalers[6].emitCycleFailure();

			QCOMPARE(signalers[7].isCycling(), true);
			QCOMPARE(signalers[8].isCycling(), true);
			QCOMPARE(signalers[9].isCycling(), false);

			signalers[8].emitCycleFailure();

			QCOMPARE(signalers[9].isCycling(), false);

			signalers[7].emitCycleFailure();

			QCOMPARE(signalers[9].isCycling(), true);

			qDebug() << "------------------------ Step 5";
        }
		qDebug() << "------------------------ Step 6";
    }
};
QTEST_APPLESS_MAIN(tst_Sequencer)
#include "tst_sequencer.moc"
