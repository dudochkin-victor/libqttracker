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
 * tags.h
 *
 *  Created on: May 15, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_DOMAIN_TAGS_H_
#define SOPRANOLIVE_DOMAIN_TAGS_H_

#include "../RDFService"
#include "../ontologies/nao.h"

namespace SopranoLive
{
	namespace Domains
	{
		namespace Tags
		{
			SOPRANO_DEPRECATED void hasTags(RDFVariable const &resource, QStringList const &tag_labels);

			RDFFilter Q_DECL_EXPORT hasAnyTag(RDFVariable const &resource, QStringList const &tag_labels);
			RDFFilter Q_DECL_EXPORT hasAllTags(RDFVariable const &resource, QStringList const &tag_labels);

			QStringList Q_DECL_EXPORT getTags(RDFVariable const &info_elements);
			void Q_DECL_EXPORT addTags(RDFVariable const &info_elements, QStringList const &tag_labels);
			void Q_DECL_EXPORT removeTags(RDFVariable const &info_elements, QStringList const &tag_labels);
		}
	}
}

#endif /* SOPRANOLIVE_DOMAIN_TAGS_H_ */
