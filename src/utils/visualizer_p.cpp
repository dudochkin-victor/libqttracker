/*
 * This file is part of libqttracker project
 *
 * Copyright (C) 2009, Nokia
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
/*
 * visualizer_p.cpp
 *
 *  Created on: Oct 27, 2010
 *      Author: iridian
 */

#include <QFile>
#include "visualizer.h"

namespace SopranoLive
{
	namespace Visualizer
	{
		struct VisualizerProcess
			: QProcess
		{
			VisualizerProcess(char *logd)
				: QProcess()
			{
				setReadChannelMode(QProcess::ForwardedChannels);
				//setStandardOutputFile("vis.out");
				start(logd);
				waitForStarted();

				QTime timer;
				timer.start();
				while(timer.elapsed() < 1000);
			}

			~VisualizerProcess()
			{
				if(state() == QProcess::Running)
				{
					closeWriteChannel();
					// 1 day. If you're debugging for longer, you have some real issues
					waitForFinished(86400000);
				}
			}
		};

		QIODevice *tryInitVisualizerSingleton()
		{
			if(char *logd = getenv("VISUALIZER"))
			{
				if(QFile(logd).exists())
				{
					static VisualizerProcess visualizer_g(logd);
					return &visualizer_g;
				}
				qWarning() << "Visualizer: specified visualizer does not exist:" << logd;
			} else if(char *logf = getenv("VISUALIZER_FILE"))
			{
				static QFile file_g(logf);
				if(file_g.open(QIODevice::WriteOnly | QIODevice::Text))
					return &file_g;
				qWarning() << "Visualizer: could not open file" << logf << "for writing";
			}
			return 0;
		}

		QIODevice *visualizerDevice()
		{
			static QIODevice *visualizer = tryInitVisualizerSingleton();
			return visualizer;
		}

		void visualizerPostOutput(QIODevice *out_device)
		{
			out_device->waitForBytesWritten(1);
		}

		QDebug outputHeader(QIODevice *out_device, const char *op)
		{
			//while(out_device->bytesToWrite())
			//	out_device->waitForBytesWritten(1);
			return QDebug(out_device).nospace() << "VZR\t" << op << "\t";
		}

	}
}
