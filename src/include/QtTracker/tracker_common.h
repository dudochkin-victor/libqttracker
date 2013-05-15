/*
 * This file is part of LibQtTracker project
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
 * tracker_common.h
 *
 *  Created on: Jan 29, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef QTTRACKER_TRACKER_COMMON_H_
#define QTTRACKER_TRACKER_COMMON_H_

#include <QString>
#include <QDateTime>
#include "QLiveAutogenerationBase"

namespace SopranoLive
{
	namespace Ontologies
	{
		namespace tracker
		{
		// taken from tracker/src/libtracker-common/tracker-property.h
			typedef QString string;
			typedef bool boolean;
			typedef int integer;
			typedef double double_;
			typedef QDateTime date;
			typedef QString blob;
			typedef LiveNode resource;
			typedef QString fulltext;
		}
	}

	namespace Contexts
	{
		struct Q_DECL_EXPORT QtTracker
		{
			static const char *encodedIri() { return "http://www.sopranolive.org/contexts/tracker/"; };
			static const QUrl &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
			static const QList<QUrl> &iriList() { static QList<QUrl> *ret = 0; if(ret) return *ret; ret = new QList<QUrl>(); *ret << iri(); return *ret; }
		};
	}

	namespace BackEnds
	{
		namespace Tracker
		{
			enum
			{
				BatchMode = (RDFStrategyFlags(1) << (offset_BackEndFlags + 0))
			,	Notify = (RDFStrategyFlags(1) << (offset_BackEndFlags + 1))
			,	SyncOnCommit = (RDFStrategyFlags(1) << (offset_BackEndFlags + 2))
			,	RemoveSubjectProperty = (RDFStrategyFlags(1) << (offset_BackEndFlags + 3))
			};
		}
	}
}

#endif /* TRACKER_COMMON_H_ */
