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
 * local_common.h
 *
 *  Created on: Jan 21, 2010
 *      Author: "Iridian Kiiskinen"
 */

#ifndef SOPRANOLIVE_LOCAL_COMMON_H_
#define SOPRANOLIVE_LOCAL_COMMON_H_

#include "QLiveAutogenerationBase"

namespace SopranoLive
{
	namespace Contexts
	{
		struct Q_DECL_EXPORT local
		{
			static const char *encodedIri() { return "http://www.sopranolive.org/contexts/local/"; };
			static const QUrl &iri() { static QUrl *ret = 0; if(!ret) ret = new QUrl(QUrl::fromEncoded(encodedIri(), QUrl::StrictMode)); return *ret; }
			static const QList<QUrl> &iriList() { static QList<QUrl> *ret = 0; if(ret) return *ret; ret = new QList<QUrl>(); *ret << iri(); return *ret; }
		};
	}
}

#endif /* SOPRANOLIVE_LOCAL_COMMON_H_ */
