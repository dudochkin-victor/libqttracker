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
 * live.cpp
 *
 *  Created on: Jan 19, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include "rowstoremodel_p.h"
#include "include/sopranolive/liveautogenerationbase.h"
#include "include/sopranolive/ontologies/core/xsd.h"

namespace SopranoLive
{
	using namespace Ontologies::Core;

	QUrl *Detail::urlFromEncodedString(QString const &encoded)
	{
		return new QUrl(QUrl::fromEncoded(encoded.toAscii(), QUrl::StrictMode));
	}

	QUrl CppType::rdfTypeof(CppType type)
	{
		switch(type.type_id)
		{
		case CppType::None: return QUrl();
		case QVariant::Bool: return xsd::boolean::iri();
		case QVariant::Int:
		case QVariant::UInt:
		case QVariant::LongLong:
		case QVariant::ULongLong: return xsd::integer::iri();
		case QVariant::Double: return xsd::double_::iri();
		case QVariant::Char: return xsd::integer::iri();
		case QVariant::Map: return rdfs::Resource::iri(); // TODO
		case QVariant::List: return rdfs::Resource::iri(); // TODO
		case QVariant::String: return xsd::string::iri();
		case QVariant::StringList: return rdfs::Resource::iri(); // TODO
		case QVariant::ByteArray: return xsd::string::iri();
		case QVariant::BitArray: return xsd::string::iri();
		case QVariant::Date: return xsd::date::iri();
		case QVariant::Time: return xsd::dateTime::iri();
		case QVariant::DateTime: return xsd::dateTime::iri();
		case QVariant::Url: return rdfs::Resource::iri(); // TODO
		}
		return QUrl();
	}

	LiveNode::LiveNode(LiteralValue const &value, QString const &language)
		: Base(LiteralValue::createPlainLiteral(value.toString(), language))
	{}

	int static_phase_g = NonStatic;

		template<>
	void AutoGen::BaseClassInfo::addBaseClassList<Detail::EmptyBase>()
	{}

	QStringList const rdf_type_as_list_g = QStringList(QString
			(rdf::type::encodedIri()));
}
