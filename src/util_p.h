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
#ifndef SOPRANOLIVE_UTIL_P_H_
#define SOPRANOLIVE_UTIL_P_H_

#include <QtGlobal>
#include <QDebug>
#include "utils/visualizer.h"
class QFile;


#ifdef PROFILE_TIMESTAMPS
# define PERF_EVENT(fmt, ...) perfEvent(fmt, ##__VA_ARGS__)
#else
# define PERF_EVENT(...)
#endif

#define PERF_LOG_FD 5

namespace SopranoLive
{
    Q_DECL_EXPORT void perfEvent(char const *fmt, ...);

	enum {
		LogCritical = 0,
		LogWarning  = 1,
		LogDebug    = 2,
	};

	// Components that want to log additional information should inherit from
	// LogContext and override logMessage().  The implementation should first call
	// parentContext()->logMessage(level) to get a QDebug object to use.
	struct LogContext {
		/* VZR_CLASS(LogContext);*/

		virtual QDebug logMessage(int level, char const *func, char const *loc) const;
	protected:
		virtual LogContext const *parentContext() const;
	};

	struct GlobalLogContext: public LogContext {
		/* VZR_CLASS(GlobalLogContext);*/

		QDebug logMessage(int level, char const *func, char const *loc) const;
	};
	extern GlobalLogContext global_log;
	extern int global_log_verbosity;
	extern QFile *global_log_file;
	extern volatile bool global_log_initialized;

	void initLogging();
	inline bool shouldLog(int level)
	{
		if (!global_log_initialized)
			initLogging();
		return level <= global_log_verbosity;
	}

	QDebug logMessage(int level, char const *func, char const *loc);
	QDebug logMessage(int level, char const *func, char const *loc, LogContext const *ctx);
	QDebug logMessage(int level, char const *func, char const *loc, char const *realfunc);
	QDebug logMessage(int level, char const *func, char const *loc, LogContext const *ctx, char const *realfunc);

#define STR2(x) #x
#define STR(x) STR2(x)

// The x* variants are the block-log things: you can write a block after them
// where you can use the `log' object to output messages.

#ifdef QTTRACKER_NODEBUG

#define LQT_LOG(...) for (;false;) qDebug()
#define LQT_XLOG(...) for (;false;)

#else

#define LQT_LOG(lvl, ...)												\
	for (bool __rly_log = shouldLog(lvl); __rly_log;)					\
		for (QDebug log = SopranoLive::logMessage(lvl, __func__, __FILE__ ":" STR(__LINE__), ##__VA_ARGS__); \
			 __rly_log; __rly_log = !__rly_log)							\
			log

#define LQT_XLOG(lvl, ...)												\
	for (bool __rly_log = shouldLog(lvl); __rly_log;)					\
		for (QDebug log = SopranoLive::logMessage(lvl, __func__, __FILE__ ":" STR(__LINE__), ##__VA_ARGS__); \
			 __rly_log; __rly_log = !__rly_log)

#endif

// use these convenience macros

#define critical(...) LQT_LOG(LogCritical, ##__VA_ARGS__)
#define warning(...) LQT_LOG(LogWarning, ##__VA_ARGS__)
#define debug(lvl, ...) LQT_LOG(lvl, ##__VA_ARGS__)

#define xcritical(...) LQT_XLOG(LogCritical, ##__VA_ARGS__)
#define xwarning(...) LQT_XLOG(LogWarning, ##__VA_ARGS__)
#define xdebug(lvl, ...) LQT_XLOG(lvl, ##__VA_ARGS__)

    void perfEvent(char const *fmt, ...);

	void processEventsAndDeferredDeletes();

		template<class T>
	struct TLSDestroyer
		: T
	{
			TLSDestroyer(T const &t)
			: T(t)
		{}

		~TLSDestroyer()
		{
			this->clear();
			processEventsAndDeferredDeletes();
		}
	};
}

#endif
