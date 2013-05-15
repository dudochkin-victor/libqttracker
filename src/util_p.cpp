/*
 * This file is part of LibQtTracker project
 *
 * Copyright (C) 2010 Nokia
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
#include "util_p.h"
#include "include/sopranolive/live.h"
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <QCoreApplication>
// LCOV_EXCL_START
#include <QFile>
#include <QMutexLocker>
#include <QTime>

namespace SopranoLive
{
	enum {
		LogTimestamps = 1 << 0,
		LogFunctions  = 1 << 1,
		LogLocations  = 1 << 2,
		LogColor      = 1 << 3,
	};
	int global_log_verbosity = -1;
	QFile *global_log_file;
	unsigned global_log_details;
	volatile bool global_log_initialized;

	GlobalLogContext Global_log_context;

	// Logs a performance event to file descriptor PERF_LOG_FD.
	void perfEvent(char const *fmt, ...)
	{
		static char tsbuf[32];
		static char msgbuf[512];
		static char const nl = '\n';

		struct timeval tv;
		gettimeofday(&tv, NULL);

		snprintf(tsbuf, sizeof(tsbuf), "%lu\t", tv.tv_sec * 1000 + tv.tv_usec / 1000);
		va_list args;
		va_start(args, fmt);
		vsnprintf(msgbuf, sizeof(msgbuf), fmt, args);
		va_end(args);
		write(PERF_LOG_FD, tsbuf, strlen(tsbuf));
		write(PERF_LOG_FD, msgbuf, strlen(msgbuf));
		write(PERF_LOG_FD, &nl, 1);
	}

	void processEventsAndDeferredDeletes()
	{
		while(QCoreApplication::hasPendingEvents())
		{
			QCoreApplication::sendPostedEvents();
			QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
			QCoreApplication::processEvents();
		}
	}

	/// Set up logging based on the environment: \c QTTRACKER_LOGLEVEL and \c
	/// QTTRACKER_LOGFILE.  The \c QTTRACKER_LOGDETAILS environment variable
	/// can be used to control log message contents by setting it to a
	/// colon-separated combination of \c ts (timestamps), \c func (function
	/// names), \c loc (file/line locations) or \c color.
	void initLogging()
	{
		global_log_initialized = true;
		// set up log details
		global_log_details = 0; // default
		char *logd = getenv("QTTRACKER_LOGDETAIL");
		if (logd) {
			if (strstr(logd, "ts"))
				global_log_details |= LogTimestamps;
			if (strstr(logd, "func"))
				global_log_details |= LogFunctions;
			if (strstr(logd, "loc"))
				global_log_details |= LogLocations;
			if (strstr(logd, "color")) {
				// no colors if stderr is not a tty
				if (isatty(2))
					global_log_details |= LogColor;
			}
			if (strstr(logd, "forcecolor")) {
				global_log_details |= LogColor;
			}
		}
		setVerbosity(-1);
		setLogFile(QString());
	}

	/// Sets log verbosity to \a level, or, if the argument is negative, the
	/// value of the \c QTTRACKER_LOGLEVEL environment variable.  Returns the
	/// previous setting.
	int setVerbosity(int level)
	{
		int prev = global_log_verbosity;
		if (level < 0) {
			char *logv = getenv("QTTRACKER_LOGLEVEL");
			if (logv)
				level = strtoul(logv, NULL, 10);
		}
		global_log_verbosity = level;
		return prev;
	}

	/// Sets the destination file for log messages.  An empty string means to
	/// obey the \c QTTRACKER_LOGFILE environment variable.
	void setLogFile(QString const &fileName)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);

		if (global_log_file) {
			global_log_file->flush();
			global_log_file->close();
			delete global_log_file;
			global_log_file = 0;
		}
		QString fn(fileName);
		if (fileName.isEmpty()) {
			char *logf = getenv("QTTRACKER_LOGFILE");
			if (logf)
				fn = QString::fromLatin1(logf);
			else
				return;
		}
		global_log_details &= ~LogColor;
		global_log_file = new QFile(fn);
		global_log_file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
	}

	static QtMsgType levelToQt(int level)
	{
		switch (level) {
		case LogCritical: return QtCriticalMsg;
		case LogWarning: return QtCriticalMsg;
		case LogDebug: default: return QtDebugMsg;
		}
	}

	static char const *levelToString(int level)
	{
		switch (level) {
		case LogCritical: return "[critical]";
		case LogWarning:  return "[warning]";
		case LogDebug:
		default:          return "[debug]";
		}
	}

	static char const *levelToColor(int level)
	{
		switch (level) {
		case LogCritical: return "\033[1;31m";
		case LogWarning:  return "\033[1;33m";
		case LogDebug:
		default: return "\033[1;36m";
		}
	}

	QDebug LogContext::logMessage(int level, char const *func, char const *loc) const
	{
		return parentContext()->logMessage(level, func, loc);
	}

	LogContext const *LogContext::parentContext() const
	{
		return &Global_log_context;
	}

	QDebug GlobalLogContext::logMessage(int level, char const *func, char const *loc) const
	{
		QDebug log = global_log_file ? QDebug(global_log_file) : QDebug(levelToQt(level));
		if (char const *base = strrchr(loc, '/'))
			loc = base + 1;
        log.nospace();
		if (global_log_details & LogColor)
			log << levelToColor(level);
        if (global_log_details & LogTimestamps) {
            QTime ts = QTime::currentTime();
            log << ts.toString("HH:mm:ss.zzz").toLatin1().constData() << ' ';
        }
        log << levelToString(level);
        if (global_log_details & LogLocations)
            log << ' ' << loc;
        if (global_log_details & LogFunctions)
            log << ' ' << func;
        log << ':';
        if (global_log_details & LogColor)
            log << "\033[0m";
		return log.space();
	}

	QDebug logMessage(int level, char const *func, char const *loc)
	{
		return Global_log_context.logMessage(level, func, loc);
	}

	QDebug logMessage(int level, char const *func, char const *loc, char const *realfunc)
	{
        Q_UNUSED(func);
		return Global_log_context.logMessage(level, realfunc, loc);
	}

	QDebug logMessage(int level, char const *func, char const *loc, LogContext const *ctx)
	{
		return ctx->logMessage(level, func, loc);
	}

	QDebug logMessage(int level, char const *func, char const *loc, LogContext const *ctx, char const *realfunc)
	{
        Q_UNUSED(func);
		return ctx->logMessage(level, realfunc, loc);
	}
}
// LCOV_EXCL_STOP

