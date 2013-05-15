/*
 * This file is part of libqttracker project
 *
 * Copyright (C) 2010, Nokia
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
#include "unit-tests.h"

#include "../../src/soprano/vocabulary/xsd.h"
#include "../../src/soprano/vocabulary/rdf.h"
#include <soprano/languagetag.h>
#include <soprano/literalvalue.h>
#include <QUrl>
#include <QLocale>
#include <QHash>
#include <QDebug>

using namespace Soprano::Vocabulary::XMLSchema;
using namespace Soprano::Vocabulary::RDF;
using namespace Soprano;

void UnitTests::soprano_vocabulary_xsd() {
	QCOMPARE( xsdNamespace(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#", QUrl::StrictMode ) );
	QCOMPARE( xsdInt(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#int", QUrl::StrictMode ) );
	QCOMPARE( integer(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#integer", QUrl::StrictMode ) );
	QCOMPARE( negativeInteger(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#negativeInteger", QUrl::StrictMode ) );
	QCOMPARE( nonNegativeInteger(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#nonNegativeInteger", QUrl::StrictMode ) );
	QCOMPARE( decimal(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#decimal", QUrl::StrictMode ) );
	QCOMPARE( xsdShort(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#short", QUrl::StrictMode ) );
	QCOMPARE( xsdLong(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#long", QUrl::StrictMode ) );
	QCOMPARE( unsignedInt(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#unsignedInt", QUrl::StrictMode ) );
	QCOMPARE( unsignedShort(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#unsignedShort", QUrl::StrictMode ) );
	QCOMPARE( unsignedLong(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#unsignedLong", QUrl::StrictMode ) );
	QCOMPARE( boolean(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#boolean", QUrl::StrictMode ) );
	QCOMPARE( xsdDouble(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#double", QUrl::StrictMode ) );
	QCOMPARE( xsdFloat(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#float", QUrl::StrictMode ) );
	QCOMPARE( string(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#string", QUrl::StrictMode ) );
	QCOMPARE( date(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#date", QUrl::StrictMode ) );
	QCOMPARE( time(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#time", QUrl::StrictMode ) );
	QCOMPARE( dateTime(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#dateTime", QUrl::StrictMode ) );
	QCOMPARE( base64Binary(), QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#base64Binary", QUrl::StrictMode ) );
}

void UnitTests::soprano_vocabulary_rdf() {
	QCOMPARE( rdfNamespace(), QUrl::fromEncoded( "http://www.w3.org/1999/02/22-rdf-syntax-ns#", QUrl::StrictMode ) );
	QCOMPARE( Alt(), QUrl::fromEncoded( "http://www.w3.org/1999/02/22-rdf-syntax-ns#Alt", QUrl::StrictMode ) );
	QCOMPARE( Bag(), QUrl::fromEncoded( "http://www.w3.org/1999/02/22-rdf-syntax-ns#Bag", QUrl::StrictMode ) );
	QCOMPARE( List(), QUrl::fromEncoded( "http://www.w3.org/1999/02/22-rdf-syntax-ns#List", QUrl::StrictMode ) );
	QCOMPARE( Property(), QUrl::fromEncoded( "http://www.w3.org/1999/02/22-rdf-syntax-ns#Property", QUrl::StrictMode ) );
	QCOMPARE( Seq(), QUrl::fromEncoded( "http://www.w3.org/1999/02/22-rdf-syntax-ns#Seq", QUrl::StrictMode ) );
	QCOMPARE( Statement(), QUrl::fromEncoded( "http://www.w3.org/1999/02/22-rdf-syntax-ns#Statement", QUrl::StrictMode ) );
	QCOMPARE( XMLLiteral(), QUrl::fromEncoded( "http://www.w3.org/1999/02/22-rdf-syntax-ns#XMLLiteral", QUrl::StrictMode ) );
	QCOMPARE( first(), QUrl::fromEncoded( "http://www.w3.org/1999/02/22-rdf-syntax-ns#first", QUrl::StrictMode ) );
	QCOMPARE( nil(), QUrl::fromEncoded( "http://www.w3.org/1999/02/22-rdf-syntax-ns#nil", QUrl::StrictMode ) );
	QCOMPARE( object(), QUrl::fromEncoded( "http://www.w3.org/1999/02/22-rdf-syntax-ns#object", QUrl::StrictMode ) );
	QCOMPARE( predicate(), QUrl::fromEncoded( "http://www.w3.org/1999/02/22-rdf-syntax-ns#predicate", QUrl::StrictMode ) );
	QCOMPARE( rest(), QUrl::fromEncoded( "http://www.w3.org/1999/02/22-rdf-syntax-ns#rest", QUrl::StrictMode ) );
	QCOMPARE( subject(), QUrl::fromEncoded( "http://www.w3.org/1999/02/22-rdf-syntax-ns#subject", QUrl::StrictMode ) );
	QCOMPARE( type(), QUrl::fromEncoded( "http://www.w3.org/1999/02/22-rdf-syntax-ns#type", QUrl::StrictMode ) );
	QCOMPARE( value(), QUrl::fromEncoded( "http://www.w3.org/1999/02/22-rdf-syntax-ns#value", QUrl::StrictMode ) );
}

void UnitTests::soprano_languagetag_constructors() {
	Soprano::LanguageTag empty;
	Soprano::LanguageTag empty2(empty);
	QCOMPARE(empty.isEmpty(), true);
	QCOMPARE(empty.toString(), QString());
	QCOMPARE(empty2.isEmpty(), true);

	Soprano::LanguageTag tag("pl_PL");
	QCOMPARE(tag.isEmpty(), false);
	QCOMPARE(tag.toString(), QString("pl_pl"));

	tag = Soprano::LanguageTag( QLatin1String("en_GB"));
	QCOMPARE(tag.toString(), QString("en_gb"));

	tag = Soprano::LanguageTag( QString("pl_PL"));
	QCOMPARE(tag.toString(), QString("pl_pl"));

	QLocale swiss("de_CH");
	tag = Soprano::LanguageTag(swiss);
	QCOMPARE(tag.toString(), QString("de-ch"));

	QLocale polish("pl_PL");
	tag = Soprano::LanguageTag(polish.language(), polish.country());
	QCOMPARE(tag.toString(), QString("pl-pl"));
}

void UnitTests::soprano_languagetag_operators() {
	QLocale swiss("de_CH");
	QLocale polish("pl_PL");
	Soprano::LanguageTag tagSwiss(swiss);
	Soprano::LanguageTag tagSwiss2(swiss);
	Soprano::LanguageTag tagPolish(polish);

	QCOMPARE( tagSwiss == tagPolish, false);
	QCOMPARE( tagSwiss == tagSwiss2, true);
	QCOMPARE( tagSwiss != tagPolish, true);
	QCOMPARE( tagSwiss != tagSwiss2, false);

	QCOMPARE ( tagSwiss < tagPolish, true);
	QCOMPARE ( tagSwiss <= tagPolish, true);
	QCOMPARE ( tagSwiss > tagPolish, false);
	QCOMPARE ( tagSwiss >= tagPolish, false);

	QCOMPARE ( tagSwiss.isValid(), true);
	Soprano::LanguageTag tag("good");
	QCOMPARE ( tag.isValid(), true);
	tag = Soprano::LanguageTag("wrong2");
	QCOMPARE ( tag.isValid(), false);
	tag = Soprano::LanguageTag();
	QCOMPARE ( tag.isValid(), false);
	tag = Soprano::LanguageTag("pl-PL-pl-PL-pl-PL-pl-PL");
	QCOMPARE ( tag.isValid(), true);
	tag = Soprano::LanguageTag("23-PL-pl-PL-pl-PL-pl-PL");
	QCOMPARE ( tag.isValid(), false);
	tag = Soprano::LanguageTag("pl-Plongerthaneightchars");
	QCOMPARE ( tag.isValid(), false);
	tag = Soprano::LanguageTag("en-x-i");
	QCOMPARE ( tag.isValid(), true);
	tag = Soprano::LanguageTag("en-x-abcd-i");
	QCOMPARE ( tag.isValid(), true);
	tag = Soprano::LanguageTag("en-x");
	QCOMPARE ( tag.isValid(), false);
	tag = Soprano::LanguageTag("en-i");
	QCOMPARE ( tag.isValid(), false);
	tag = Soprano::LanguageTag("en-i-abcd-g");
	QCOMPARE ( tag.isValid(), false);

	tag = Soprano::LanguageTag("PL-en-GB-De-fOuR-XxX");
	QCOMPARE ( tag.toPrettyString(), QString("pl-EN-GB-DE-Four-xxx"));

	tag = Soprano::LanguageTag(polish);
	QCOMPARE ( tag.toLocale().name(), polish.name());
	tag = Soprano::LanguageTag("*");
	QCOMPARE ( tag.toLocale().name(), QLocale().name());

	// just check if it is possible to use it with QHash
	QHash<Soprano::LanguageTag, int> hash;
	hash.insert(tag, 2);
	QCOMPARE(hash[tag], 2);

	// check if it is working with QTextStream properly
	tag = Soprano::LanguageTag(polish);
	QString str;
	QTextStream stream(&str, QIODevice::ReadWrite );
	stream << tag;
	QCOMPARE(str, QString("pl-pl"));
	str.clear();
	Soprano::LanguageTag empty;
	stream << empty;
	QCOMPARE(str, QString("(empty)"));

	// check if it is working with qDebug
	qDebug() << empty;
	qDebug() << tag;
}

void UnitTests::soprano_languagetag_lookup() {
	Soprano::LanguageTag tag(QLocale("pl_PL"));
	QList<Soprano::LanguageTag> choices;
	QList<Soprano::LanguageTag> priorities;
	QString priority("pl");
	choices << tag;
	priorities << Soprano::LanguageTag(priority);

	int ret = tag.lookup(choices, Soprano::LanguageTag(priority));
	QCOMPARE(ret, 0);

	ret = tag.lookup(choices, Soprano::LanguageTag(priority),
	                          Soprano::LanguageTag::LookupFlagNone,
	                          Soprano::LanguageTag::MatchFilterExtended );
	QCOMPARE(ret, 0);
}

void UnitTests::soprano_languagetag_matches() {
	Soprano::LanguageTag tag(QLocale("pl-GB-US"));
	Soprano::LanguageTag polish(QLocale("pl_PL"));

	//basic
	QCOMPARE( tag.matches(Soprano::LanguageTag("en-gb-de")), false);
	QCOMPARE( tag.matches(Soprano::LanguageTag("en-gb-de-pl")), false);
	QCOMPARE( tag.matches(Soprano::LanguageTag("en-gb-de-pl-PL")), false);
	QCOMPARE( polish.matches(Soprano::LanguageTag("pl")), true);
	QCOMPARE( tag.matches(Soprano::LanguageTag("*")), true);
	QCOMPARE( polish.matches(Soprano::LanguageTag("*")), true);
	QCOMPARE( polish.matches(Soprano::LanguageTag("pl-PL")), true);

	// ecxtended
	QCOMPARE( tag.matches(Soprano::LanguageTag("en-gb-de"), Soprano::LanguageTag::MatchFilterExtended), false);
	QCOMPARE( polish.matches(Soprano::LanguageTag("pl-en-gb-de"), Soprano::LanguageTag::MatchFilterExtended), false);
	QCOMPARE( tag.matches(Soprano::LanguageTag("en"), Soprano::LanguageTag::MatchFilterExtended), false);
	QCOMPARE( tag.matches(Soprano::LanguageTag("pl"), Soprano::LanguageTag::MatchFilterExtended), false);
	QCOMPARE( polish.matches(Soprano::LanguageTag("pl"), Soprano::LanguageTag::MatchFilterExtended), true);
	QCOMPARE( polish.matches(Soprano::LanguageTag("*-en-pl"), Soprano::LanguageTag::MatchFilterExtended), false);

	// check default branch in case statement
	QCOMPARE( tag.matches(Soprano::LanguageTag("pl-PL"), (Soprano::LanguageTag::MatchFilter)3), false);
}

void UnitTests::soprano_literalvalue_defaults() {
	QCOMPARE(LiteralValue::typeFromDataTypeUri(xsdInt()), QVariant::Int);
	QCOMPARE(LiteralValue::typeFromDataTypeUri(base64Binary()), QVariant::ByteArray);
	QCOMPARE(LiteralValue::typeFromDataTypeUri(xsdFloat()), QVariant::Double);
}

void UnitTests::soprano_literalvalue_operators() {
	QHash<LiteralValue, int> hash;
	hash.insert(LiteralValue(8), 8);
	hash.insert(LiteralValue(), 5);
	QCOMPARE(hash[LiteralValue(8)], 8);
	QCOMPARE(hash[LiteralValue()], 5);

	LiteralValue val = LiteralValue::fromString(QString("13"), xsdInt());
	QCOMPARE(val.isValid(), true);
	QCOMPARE(val.isInt(), true);
	QCOMPARE(val.isUnsignedInt(), false);

	LiteralValue val2 = LiteralValue::createPlainLiteral(QString("value"), LanguageTag() );
	QCOMPARE(val2.isInt(), false);
	QCOMPARE(val2.isPlain(), true);
	QCOMPARE(val2.language(), LanguageTag());

	LiteralValue valInt = LiteralValue::fromString(QString("13"), xsdInt());
	LiteralValue valStr = LiteralValue::fromString(QString("13"), string());
	QCOMPARE(valInt == valStr, false);
	QCOMPARE(valInt != valStr, true);
	QCOMPARE(valInt == LiteralValue(13), true);
	valStr = valInt;
	QCOMPARE(valInt == valStr, true);
	valStr = (unsigned)13;
	QCOMPARE(valInt == valStr, false);

	// check if it is not crashing with any of supported types
	val = (int)3;
	val = (uint)3;
	{
		uint x = val.toUnsignedInt();
		QCOMPARE(x, (uint)3);
	}
	val = (qulonglong)3;
	{
		qulonglong x = val.toUnsignedInt64();
		QCOMPARE(x, (qulonglong)3);
	}
	val = (qlonglong)3;
	{
		qlonglong x = val.toInt64();
		QCOMPARE(x, (qlonglong)3);
	}
	val = (bool)true;
	val = (double)1.2;
	val = (float)1.2;
	val = QString("2");
	QCOMPARE(val.isString(), true);
	val = QLatin1String("2");
	val = QDate();
	val = QTime();
	val = QDateTime();
	val = QByteArray();
}

void UnitTests::soprano_literalvalue_debug() {
	LiteralValue val1("string");
	LiteralValue val2(12);
	LiteralValue val3(12.5);
	qDebug() << val1 << val2 << val3;
}
