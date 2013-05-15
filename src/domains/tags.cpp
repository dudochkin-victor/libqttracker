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
 * tags.cpp
 *
 *  Created on: May 15, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */


#include "../include/QtTracker/domains/tags.h"
#include "../include/QtTracker/tracker.h"


namespace SopranoLive
{
	namespace Domains
	{
		namespace Tags
		{
			using namespace BackEnds;

			void hasTags(RDFVariable const &resource, QStringList const &tag_list)
			{
				resource.property<nao::hasTag>().property<nao::prefLabel>() = RDFVariable::fromContainer(tag_list);
			}

			RDFFilter hasAnyTag(RDFVariable const &resource, QStringList const &tag_list)
			{
				RDFVariable label = resource.optional().property<nao::hasTag>().property<nao::prefLabel>();
				label.isMemberOf(tag_list);
				return label.exists();
			}

			RDFFilter hasAllTags(RDFVariable const &resource, QStringList const &tag_list)
			{
				RDFVariable res = resource.optional();
				foreach(QString tag, tag_list)
					res.property<nao::hasTag>().property<nao::prefLabel>() == LiteralValue(tag);
				// TODO: fix bug
				return res.exists();
			}

			QStringList getTags(RDFVariable const &info_element)
			{
				QStringList ret;
				foreach(LiveNode tag, Tracker::tracker()->modelVariable(info_element.property<nao::hasTag>().property<nao::prefLabel>()))
					ret.push_back(tag.toString());
				return ret;
			}

			void addTags(RDFVariable const &info_elements, QStringList const &tags)
			{
				if(!tags.size())
					return;

				QStringList missing_tags = tags;

				// query for exising tags from the set tags
				RDFVariable labelVar = RDFVariable::fromType<nao::Tag>().property<nao::prefLabel>();
				labelVar.isMemberOf(tags);
			    foreach(LiveNode label, Tracker::tracker()->modelVariable(labelVar))
					// generate missing_tags by remove existing tags from the supplied tags list
			    	missing_tags.removeAll(label.toString());

			    // create new tags to Tracker::tracker for the missing tags
			    foreach(QString tag, missing_tags)
			    	Live<nao::Tag>(Tracker::tracker()->createLiveNode())->setPrefLabel(tag);

			    // add tags to the objects
			    Tracker::tracker()->executeQuery(RDFUpdate().addInsertion
			    	( RDFStatementList()
			    		<< RDFStatement(info_elements, rdf::type::iri(), rdfs::Resource::iri())
			    		<< RDFStatement(info_elements, nao::hasTag::iri(), RDFVariable::fromContainer(tags).subject<nao::prefLabel>())
					));
			}

			void removeTags(RDFVariable const &info_elements, QStringList const &tags)
			{
				if(!tags.size())
					return;

				Tracker::tracker()->executeQuery(RDFUpdate().addDeletion
						( info_elements
						, nao::hasTag::iri()
						, RDFVariable::fromContainer(tags).subject<nao::prefLabel>()
						));

				// remove tags with no references left
				RDFSelect sel;
				RDFVariable existingTagsVar = sel.newColumn<nao::Tag>();
				existingTagsVar.property<nao::prefLabel>().isMemberOf(tags);

				sel.groupBy(existingTagsVar);
				sel.addCountColumn(existingTagsVar.subject<nao::hasTag>());

				LiveNodes livetags = Tracker::tracker()->modelQuery(sel);

				for(int i = 0; i < livetags->rowCount(); ++i)
					if(livetags->data(livetags->index(i, 1)).toInt() == 0)
						livetags->liveNode(livetags->index(i, 0))->remove();
				return;
			}

		}
	}
}
