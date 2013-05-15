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
 * rawdatatypes_p.h
 *
 *  Created on: Nov 25, 2009
 *      Author: "Iridian Kiiskinen"
 */

#ifndef SOPRANOLIVE_RAWDATATYPES_P_H_
#define SOPRANOLIVE_RAWDATATYPES_P_H_

#include "live_p.h"
#include "include/sopranolive/rdfunbound.h"

namespace SopranoLive
{
	struct RawTripleData;

	// Semantic conversion between RawPropertyData and RawTripleData is direct
	// mapping between predicates, and strategy == mask ^ RDFStrategy::SubjectObject
	// with subject and object values being irrelevant.
	struct RawPropertyData
	{
		RDFStrategyFlags strategy_;
		QString predicate_;
		RawPropertyData(RDFStrategyFlags strategy = RDFStrategy::Disabled, QString const &predicate = QString())
			: strategy_(strategy), predicate_(predicate)
		{}
		RawPropertyData(RDFProperty const &predicate_data)
			: strategy_(predicate_data.strategy())
			, predicate_(predicate_data.predicate().metaValue().toString())
		{
			if(!predicate_.size())
				strategy_ = RDFStrategy::Disabled;
		}
		bool isEmpty() const { return !strategy_; }
		operator bool() { return !isEmpty(); }
		RDFStrategyFlags &strategy() { return strategy_; }
		RDFStrategyFlags strategy() const { return strategy_; }
		QString &predicate() { return predicate_; }
		QString const &predicate() const { return predicate_; }
		inline RawTripleData toRawTripleData() const;
		static RawPropertyData fromSubject(QString const &predicate) { return RawPropertyData(RDFStrategy::SubjectPredicate, predicate); }
		static RawPropertyData fromObject(QString const &predicate) { return RawPropertyData(RDFStrategy::PredicateObject, predicate); }
		bool operator!=(RawPropertyData const &rhs) const { return strategy_ != rhs.strategy_ || predicate_ != rhs.predicate_; }
		bool operator==(RawPropertyData const &rhs) const { return strategy_ == rhs.strategy_ && predicate_ == rhs.predicate_; }
		bool operator<(RawPropertyData const &rhs) const { return strategy_ < rhs.strategy_ || predicate_ < rhs.predicate_; }
		friend uint qHash(RawPropertyData const &raw) { return raw.strategy_ ^ qHash(raw.predicate_); }
		friend QDebug operator<<(QDebug s, RawPropertyData const &v)
		{
			s.nospace() << "RawPropertyData(" << v.predicate_ << " / "
					<< QString::number(v.strategy_, 16) << ")";
			return s;
		}
	};

		template<RDFStrategyFlags equality_mask>
	struct RawPropertyDataWithEqualityMask
		: public RawPropertyData
	{
		RawPropertyDataWithEqualityMask() : RawPropertyData() {}
		RawPropertyDataWithEqualityMask(RawPropertyData const &cp) : RawPropertyData(cp) {}
		RawPropertyDataWithEqualityMask(RDFProperty const &prop) : RawPropertyData(prop) {}
		bool operator!=(RawPropertyData const &rhs) const
		{
			return (equality_mask & (strategy_ ^ rhs.strategy_)) || predicate_ != rhs.predicate_;
		}
		bool operator==(RawPropertyData const &rhs) const { return !operator!=(rhs); }
		bool operator<(RawPropertyData const &rhs) const
		{
			return (equality_mask & strategy_) < (equality_mask & rhs.strategy_) || predicate_ < rhs.predicate_;
		}
		friend uint qHash(RawPropertyDataWithEqualityMask const &raw)
		{
			return (equality_mask & raw.strategy_) ^ qHash(raw.predicate_);
		}
	};

	enum
	{
		RawEqualityMask = RDFStrategy::TripleMask
	,	RawStrategyMask = RDFStrategy::Streaming | RDFStrategy::Windowed | RDFStrategy::Live | RDFStrategy::Writable
	};

	struct RawTriple
	{
		QString subject_, predicate_, object_;
		RawTriple(QString const &s, QString const &p, QString const &o)
			: subject_(s), predicate_(p), object_(o)
		{}
		bool operator<(RawTriple const &other) const
		{
			return subject_ < other.subject_ || (subject_ == other.subject_ && (predicate_ < other.predicate_ || (predicate_ == other.predicate_ && object_ < other.object_)));
		}
		friend QDebug operator<<(QDebug s, RawTriple const &v)
		{
			s.nospace() << "RawTriple("
					<< v.subject_ << ", " << v.predicate_  << ", " << v.object_ << ")";
			return s;
		}
	};
	typedef QMap<RawTriple, int> CanonicalRawTriples;

	struct RawTripleData
		: RawTriple
	{
		RDFStrategyFlags mask_;
		RawTripleData(RDFStrategyFlags mask = RDFStrategy::Disabled
				  , QString const &subject = QString(), QString const &predicate = QString(), QString const &object = QString())
			: RawTriple(subject, predicate, object), mask_(mask)
		{}
		RawTripleData(QString const &subject, QString const &predicate, QString const &object)
			: RawTriple(subject, predicate, object), mask_(RDFStrategy::Subject | RDFStrategy::Predicate | RDFStrategy::Object)
		{}
		bool isEmpty() const { return !mask_; }
		operator bool() { return !isEmpty(); }
		RDFStrategyFlags &mask() { return mask_; }
		RDFStrategyFlags mask() const { return mask_; }
		QString &subject() { return subject_; }
		QString const &subject() const { return subject_; }
		QString &predicate() { return predicate_; }
		QString const &predicate() const { return predicate_; }
		QString &object() { return object_; }
		QString const &object() const { return object_; }
		RawPropertyData toRawPropertyData() const
		{
			return RawPropertyData(mask_ ^ RDFStrategy::SubjectObject, predicate());
		}
		static RawTripleData fromSubject(QString const &subject) { return RawTripleData(RDFStrategy::Subject, subject, QString(), QString()); }
		static RawTripleData fromPredicate(QString const &predicate) { return RawTripleData(RDFStrategy::Predicate, QString(), predicate, QString()); }
		static RawTripleData fromObject(QString const &object) { return RawTripleData(RDFStrategy::Object, QString(), QString(), object); }
		static RawTripleData fromSubjectPredicate(QString const &subject, QString const &predicate) { return RawTripleData(RDFStrategy::Subject | RDFStrategy::Predicate, subject, predicate, QString()); }
		static RawTripleData fromSubjectObject(QString const &subject, QString const &object) { return RawTripleData(RDFStrategy::Subject | RDFStrategy::Object, subject, QString(), object); }
		static RawTripleData fromPredicateObject(QString const &predicate, QString const &object) { return RawTripleData(RDFStrategy::Predicate | RDFStrategy::Object, QString(), predicate, object); }
		bool operator!=(RawTripleData const &rhs) const { return mask_ != rhs.mask_ || subject_ != rhs.subject_ || predicate_ != rhs.predicate_ || subject_ != rhs.object_; }
		bool operator==(RawTripleData const &rhs) const { return mask_ == rhs.mask_ && subject_ == rhs.subject_ && predicate_ == rhs.predicate_ && subject_ == rhs.object_; }
		bool operator<(RawTripleData const &rhs) const { return mask_ < rhs.mask_ || subject_ < rhs.subject_ || predicate_ != rhs.predicate_ || subject_ != rhs.object_; }
		friend uint qHash(RawTripleData const &raw) { return raw.mask_ ^ qHash(raw.subject_) ^ qHash(raw.predicate_) ^ qHash(raw.object_); }
		friend QDebug operator<<(QDebug s, RawTripleData const &v)
		{
			s.nospace() << "RawTripleData("
					<< v.subject_ << ", " << v.predicate_  << ", " << v.object_ << " / "
					<< QString::number(v.mask_, 16) << ")";
			return s;
		}

	};

	inline RawTripleData RawPropertyData::toRawTripleData() const
	{
		if(strategy_ & RDFStrategy::Object)
			return RawTripleData::fromObject(predicate());
		if(strategy_ & RDFStrategy::Subject)
			return RawTripleData::fromSubject(predicate());
		return RawTripleData();
	}

	struct RawTriplesData
	{
		RDFStrategyFlags mask_;
		QStringList subjects_, predicates_, objects_;
		RawTriplesData( RDFStrategyFlags mask = RDFStrategy::Disabled
				  , QStringList const &subjects = QStringList()
				  , QStringList const &predicates = QStringList()
				  , QStringList const &objects = QStringList())
			: mask_(mask), subjects_(subjects), predicates_(predicates), objects_(objects)
		{}
		RawTriplesData( QStringList const &subjects, QStringList const &predicates, QStringList const &objects)
			: mask_(RDFStrategy::Subject | RDFStrategy::Predicate | RDFStrategy::Object)
			, subjects_(subjects), predicates_(predicates), objects_(objects)
		{}
		bool isEmpty() const { return !mask_; }
		RDFStrategyFlags &mask() { return mask_; }
		QStringList &subjects() { return subjects_; }
		QStringList &predicates() { return predicates_; }
		QStringList &objects() { return objects_; }
		RDFStrategyFlags mask() const { return mask_; }
		QStringList const &subjects() const { return subjects_; }
		QStringList const &predicates() const { return predicates_; }
		QStringList const &objects() const { return objects_; }

		static RawTriplesData fromSubjects(QStringList const &subjects) { return RawTriplesData(RDFStrategy::Subject, subjects, QStringList(), QStringList()); }
		static RawTriplesData fromPredicates(QStringList const &predicates) { return RawTriplesData(RDFStrategy::Predicate, QStringList(), predicates, QStringList()); }
		static RawTriplesData fromObjects(QStringList const &objects) { return RawTriplesData(RDFStrategy::Object, QStringList(), QStringList(), objects); }
		static RawTriplesData fromSubjectsPredicates(QStringList const &subjects, QStringList const &predicates) { return RawTriplesData(RDFStrategy::Subject | RDFStrategy::Predicate, subjects, predicates, QStringList()); }
		static RawTriplesData fromSubjectsObjects(QStringList const &subjects, QStringList const &objects) { return RawTriplesData(RDFStrategy::Subject | RDFStrategy::Object, subjects, QStringList(), objects); }
		static RawTriplesData fromPredicatesObjects(QStringList const &predicates, QStringList const &objects) { return RawTriplesData(RDFStrategy::Predicate | RDFStrategy::Object, QStringList(), predicates, objects); }
		friend QDebug operator<<(QDebug s, RawTriplesData const &v)
		{
			s.nospace() << "RawTriplesData("
					<< v.subjects_ << ", " << v.predicates_  << ", " << v.objects_ << " / "
					<< QString::number(v.mask_, 16) << ")";
			return s;
		}
	};

}
#endif /* SOPRANOLIVE_RAWDATATYPES_P_H_ */
