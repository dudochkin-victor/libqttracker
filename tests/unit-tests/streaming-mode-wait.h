/*
 * Copyright (C) 2010 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */
#ifndef _QTTRACKER_STREAMING_MODE_WAIT_H
#define _QTTRACKER_STREAMING_MODE_WAIT_H

#include "unit-tests.h"

class StreamingModeWait : public QObject
{
	Q_OBJECT;

private:
	LiveNodes model;
	bool      finished;

public:
	StreamingModeWait (LiveNodes model)
		: model (model), finished (false)
	{
		QObject::connect (model.model(), SIGNAL (modelUpdated()), this, SLOT (modelUpdated()));
	}

	void waitOnFinished () {
		QTime timer;
		timer.start();
		while((!finished) && (timer.elapsed() < 5000))
					QCoreApplication::processEvents();
	}

public Q_SLOTS:
	void modelUpdated () {
			if (!(model->strategy() & RDFStrategy::Running))
					finished = true;
	}
};

#endif //_QTTRACKER_STREAMING_MODE_WAIT_H
