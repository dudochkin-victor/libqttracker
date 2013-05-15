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
 * rdfunbound_p.h
 *
 *  Created on: Dec 25, 2008
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_RDFUNBOUND_P_H_
#define SOPRANOLIVE_RDFUNBOUND_P_H_

#include <algorithm>
#include <QtCore/QPair>
#include <QtCore/QLinkedList>

#include "qhasproperty.h"
#include "qfifoset.h"
#include "include/sopranolive/rdfunbound.h"
#include "rowstoremodel_p.h"

//#define QRDFUNBOUND_DEBUG

#define Q_D_C(Class) Class##Private const *const d = d_func()
//#define Q_D(Class) Class##Private *const d = d_func()

namespace SopranoLive
{
	struct Unboundtor;
	typedef QExplicitlySharedDataPointer<Unboundtor> ShUnboundtor;

		template<typename Id>
	struct HasParentUnboundtor
		: QHasBackReferencedProperty<HasParentUnboundtor<Id>, Unboundtor>
	{
		virtual ~HasParentUnboundtor() {}
		HasParentUnboundtor(Unboundtor *unboundtor = 0)
			: QHasBackReferencedProperty<HasParentUnboundtor, Unboundtor>(unboundtor) {}
		Unboundtor *unboundtor() { return this->property(); }
		void setUnboundtor(Unboundtor *new_unboundtor) { this->setProperty(new_unboundtor); }
	};

	struct HasVariables
		: HasParentUnboundtor<HasVariables>::BackRef
	{
		typedef QSet<HasParentUnboundtor<HasVariables> *> Set;
		Set const &variables() const { return backrefs(); }
	};

	struct HasPatterns
		: HasParentUnboundtor<HasPatterns>::BackRef
	{
		typedef QSet<HasParentUnboundtor<HasPatterns> *> Set;
		Set const &patterns() const { return backrefs(); }
	};

	struct SharesAnUnboundtor
		: QHasBackReferencedProperty<SharesAnUnboundtor, Unboundtor, SharesAnUnboundtor *, ShUnboundtor>
	{
		SharesAnUnboundtor(ShUnboundtor unboundtor = ShUnboundtor())
			: QHasBackReferencedProperty<SharesAnUnboundtor, Unboundtor, SharesAnUnboundtor *, ShUnboundtor>(unboundtor) {}
		ShUnboundtor unboundtorShare() const { return property(); }
		void setUnboundtorShare(ShUnboundtor new_unboundtor) { setProperty(new_unboundtor); }
	};

	struct TripleSubject;
	struct TriplePredicate;
	struct TripleObject;

	/**
	 * Owns all objects that are brought into the scope of the query
	 */
	struct Unboundtor
		: QSharedData
		, HasVariables
		, HasPatterns
		, SharesAnUnboundtor::BackRef
	{

		typedef QVector<RDFSelect::Data *> SubQueries;
		SubQueries subqueries_;

		struct Variable;
		struct Filter;
		struct VariableLink;
		struct Pattern;

		ShUnboundtor propertyValue(ShUnboundtor */*dummy*/ = 0) { return ShUnboundtor(this); }
		Unboundtor *propertyValue(Unboundtor **/*dummy*/ = 0) { return this; }

		struct HasVariable
			: QHasBackReferencedProperty<HasVariable, Variable>
		{
			HasVariable(Variable *variable = 0) : QHasBackReferencedProperty<HasVariable, Variable>(variable) {}
			Variable *variable() const { return property(); }
			void setVariable(Variable *new_variable) { setProperty(new_variable); }
		};
			template<typename Role>
		struct HasVariableInRole
			: HasVariable
		{
			HasVariableInRole(Variable *variable = 0) : HasVariable(variable) {}
		};
		struct HasPattern
			: QHasBackReferencedProperty<HasPattern, Pattern>
		{
			HasPattern(Pattern *pattern = 0) : QHasBackReferencedProperty<HasPattern, Pattern>(pattern) {}
			Pattern *pattern() const { return property(); }
			void setPattern(Pattern *new_pattern) { setProperty(new_pattern); }
		};
			template<typename Role>
		struct HasPatternInRole
			: HasPattern
		{
			HasPatternInRole(Pattern *pattern = 0) : HasPattern(pattern) {}
		};

		typedef QSet<Pattern const *> Patterns;
		typedef QLinkedList<Pattern const *> PatternList;

		struct DeepCopier
		{
			DeepCopier() : new_unboundtor(0) {}

			void initialize(Unboundtor const &deepCopyd, RDFVariable::DeepCopier const *owner);
			RDFVariable copy(RDFVariable const &var);
			RDFPattern copy(RDFPattern const &pat);
			RDFSubSelect copy(RDFSubSelect const &pat);
			RDFVariable::DeepCopier const *owner_;
			QExplicitlySharedDataPointer<Unboundtor> new_unboundtor;
			typedef QHash<Variable const *, Variable *> Vars;
			typedef QHash<Pattern const *, Pattern *> Pats;
			typedef QHash<Patterns const *, QSharedPointer<Patterns> > PatSets;
			typedef QHash<PatternList const *, QSharedPointer<PatternList> > PatLists;
			typedef QHash<RDFQuery::Data const *, RDFSubSelect::Data *> SubSels;
			Vars vars;
			Pats pats;
			PatSets patsets;
			PatLists patlists;
			SubSels subsels;
		};

		void merge(Unboundtor &other);

		struct Variable
			: HasParentUnboundtor<HasVariables>
			, HasVariable::BackRef
		{
			LiveNode node;
			CppType type;
			enum StateFlags
			{	begin_SF = 1
			,	Unconnected = begin_SF << 0, Strict = begin_SF << 1
			,	Explicit = begin_SF << 2, Definite = Explicit | (begin_SF << 3)
			,	Blank = begin_SF << 4
			,	Special = begin_SF << 5, Filter = Special | (begin_SF << 6)
			,	Link = Special | (begin_SF << 7)
			, 	Empty = begin_SF << 8
			,	end_SF = begin_SF << 9
			};
			RDFStrategyFlags state_, strategy_;
			QVector<RDFProperty> derived_properties_;
			int use_count_;
			QVariantMap attributes_;

			Variable( LiveNode const &node = LiveNode(), Unboundtor *unboundtor = 0
					, RDFStrategyFlags flags = Unconnected);
			virtual ~Variable();

			Variable(Variable const &cp);
			virtual Variable *deepCopy() const;
			virtual void deepCopyInitialize(Variable const &original, DeepCopier &deepcopier);

			void setIdentifier(QString const &name);

			void setAlias(Unboundtor::Variable &alias);

			RDFStrategyFlags strategy() { return strategy_; }
			void setStrategy(RDFStrategyFlags strategy) { strategy_ = strategy; }

			bool isEmpty() { return node.isEmpty() && ((state_ & (Empty | Unconnected)) == (Empty | Unconnected)); }
			void setEmpty(bool empty = true) { if(empty) state_ |= (Unconnected | Empty); else state_ &= ~Empty; }

			bool isUnconstrainedBlank() { return isBlank() && (state_ & Unconnected); }
			bool isConstrainedBlank() { return isBlank() && !(state_ & Unconnected); }

			bool isUnconnected() { return state_ & Unconnected; }
			void setUnconnected(bool unknown = true) { if(unknown) state_ |= Unconnected; else state_ &= ~Unconnected; }

			bool isStrict() const { return state_ & Strict; }
			void setStrict(bool strict_ = true) { if(strict_) state_ |= Strict; else state_ &= ~Strict; }

			bool isExplicit() const { return state_ & Explicit; }
			void setExplicit(bool explicit_ = true) { if(explicit_) state_ |= Explicit; else state_ &= ~Explicit; }

			bool isDefinite() const { return (state_ & Definite) == Definite; }
			void setDefinite(bool bound_ = true) { if(bound_) state_ |= Definite; else state_ &= ~Definite; }

			bool isBlank() const { return state_ & Blank; }
			void setBlank(bool blank_ = true) { if(blank_) state_ |= Blank; else state_ &= ~Blank; }

			bool isSpecial() { return state_ & Special; }
			void setSpecial(bool special = true) { if(special) state_ |= Special; else state_ &= ~Special; }

			void markUse() { use_count_++; }
			int uses() const { return use_count_; }

			bool isExpression() { return (state_ & Filter) == Filter; }

			void isOfType(CppType new_type, bool new_strict);

			virtual Unboundtor::Filter *getFilter() { return 0; }
			virtual Unboundtor::Filter const *getFilter() const { return 0; }
			virtual void disableFilter() const  {}
			virtual RDFVariable::Data *linkTarget() { return 0; }
			virtual RDFVariable::Data const *constLinkTarget() const { return 0; }
			virtual Pattern *linkDomainPattern() { return 0; }
			virtual QVector<RDFProperty> derivedProperties() { return derived_properties_; }

			// returns the dominant variable in the filter expression, whose pattern is used by default
			virtual Unboundtor::Variable const *getDominant() const { return isDefinite() ? 0 : this; }
			Unboundtor::Variable *getDominantOrThis()
			{
				if(Variable *ret = const_cast<Variable *>(getDominant()))
					return ret;
				return this;
			}

			void registerDerivedAsOutboundRef(RDFProperty &derived_property, Unboundtor *ubtor);
			void unregisterDerivedAsOutboundRef(RDFProperty &derived_property, Unboundtor *ubtor);
			void addDerivedProperty(RDFProperty &derived_property);
			void registerAllDerivedsAsOutboundRefsIn(Unboundtor *ubtor);
			void unregisterAllDerivedsAsOutboundRefsIn(Unboundtor *ubtor);
			void setDerivedProperties(QVector<RDFProperty> const &derived_properties);

			virtual QList<QVariant> metaAttribute(QString const &attribute_name) const;
			virtual void metaInsertAttribute(QString const &attribute_name, QVariant const &value);
			virtual void metaRemoveAttribute(QString const &attribute_name);
			virtual QVariantMap metaAttributeMap() const;
			virtual void metaSetAttributeMap(QVariantMap const &attribute_map);
		};

		struct Triple
			: HasVariableInRole<TripleSubject>
			, HasVariableInRole<TriplePredicate>
			, HasVariableInRole<TripleObject>
		{
			Triple(Variable *subject = 0, Variable *predicate = 0, Variable *object = 0);
			void deepCopyInitialize(Triple const &original, DeepCopier &deepcopier);
				template<typename Role>
			Unboundtor::Variable *property() { return HasVariableInRole<Role>::property(); }
				template<typename Role>
			void setProperty(Unboundtor::Variable *new_) { return HasVariableInRole<Role>::setProperty(new_); }
			Unboundtor::Variable *subject() const;
			Unboundtor::Variable *predicate() const;
			Unboundtor::Variable *object() const;
		};
		typedef QLinkedList<Triple> Triples;

		struct Filter
			: Variable
		{
			enum Type
			{	none
			,	identity
			, 	beginUnaryOps
			,	not_ = beginUnaryOps, inv
			,	suffixStringUnaryOpsBegin
			,	hasPrefix = suffixStringUnaryOpsBegin, hasSuffix, contains, matchesRegexp
			,	suffixStringUnaryOpsEnd
			,	endUnaryOps = suffixStringUnaryOpsEnd
			,	beginBinaryOps = endUnaryOps
			,	and_ = beginBinaryOps, or_
			,	equal, notEqual, less, greater, lessOrEqual, greaterOrEqual
			,	add, sub, mul, div
			,	endBinaryOps
			,	beginMultiOps = endBinaryOps
			,	isMemberOf = beginMultiOps
			,	function
			,	endMultiOps
			,	subSelectExpression
			,	patternExpression
			};

			Type filtertype_;
			Variable const *dominant_;
			enum { Active = Variable::end_SF, end_SF = Active << 1 };

			bool isActive() const { return state_ & Active; }
			void setActive(bool active = true) { if(active) state_ |= Active; else state_ &= ~Active; }

			Filter(Type type = none, bool active = true, Variable const *dominant = 0);
			//! Constructor for literal constant filter.
			Filter(LiveNode const &node);
			Filter(Filter const &cp);
			Filter *deepCopy() { return new Filter(*this); }

			Filter *getFilter() { return this; }
			Filter const *getFilter() const { return this; }
			void disableFilter() const { const_cast<Filter &>(*this).setActive(false); }

			Unboundtor::Variable const *getDominant() const { return dominant_; }

			void useFilterIn(Pattern *);
		};
		typedef QLinkedList<Filter *> Filters;

		struct UnaryFilter
			: Filter, HasVariable
		{
			UnaryFilter(Filter::Type type, Unboundtor::Variable *var, bool active = true);
			UnaryFilter(UnaryFilter const &cp);
			UnaryFilter *deepCopy() const;
			void deepCopyInitialize(Variable const &original, DeepCopier &deepcopier);
		};
		struct UnarySuffixFilter
			: UnaryFilter
		{
			QString suffix;
			UnarySuffixFilter(Filter::Type type, Unboundtor::Variable *var, QString suffix, bool active = true);
			UnarySuffixFilter(UnarySuffixFilter const &cp);
			UnarySuffixFilter *deepCopy() const;
		};
		struct BinaryFilterLHS;
		struct BinaryFilterRHS;
		struct BinaryFilter
			: Filter
			, HasVariableInRole<BinaryFilterLHS>
			, HasVariableInRole<BinaryFilterRHS>
		{
			Unboundtor::Variable *lhs() const { return HasVariableInRole<BinaryFilterLHS>::variable(); }
			Unboundtor::Variable *rhs() const { return HasVariableInRole<BinaryFilterRHS>::variable(); }

			BinaryFilter(Filter::Type type, Unboundtor::Variable *lhs, Unboundtor::Variable *rhs, bool active = true);
			BinaryFilter(BinaryFilter const &cp);
			BinaryFilter *deepCopy() const;
			void deepCopyInitialize(Variable const &original, DeepCopier &deepcopier);
		};
		struct PolyFilter
			: Filter
		{
			QLinkedList<HasVariable> arguments;
			PolyFilter(Filter::Type type, QList<Unboundtor::Variable *> const &args, bool active = true);
			PolyFilter(PolyFilter const &cp);
			PolyFilter *deepCopy() const;
			void deepCopyInitialize(Variable const &original, DeepCopier &deepcopier);
		};

		struct MemberFilterMember;
		struct MemberFilter
			: PolyFilter
			, HasVariableInRole<MemberFilterMember>
		{
			Unboundtor::Variable *member() const { return HasVariableInRole<MemberFilterMember>::variable(); }
			MemberFilter(Unboundtor::Variable *member, QList<Unboundtor::Variable *> const &set, bool active = true);
			MemberFilter(MemberFilter const &cp);
			MemberFilter *deepCopy() const;
			void deepCopyInitialize(Variable const &original, DeepCopier &deepcopier);
		};

		struct FunctionFilterName;
		struct FunctionFilter
			: PolyFilter
			, HasVariableInRole<FunctionFilterName>
		{
			Unboundtor::Variable *name() const { return HasVariableInRole<FunctionFilterName>::variable(); }
			FunctionFilter(Unboundtor::Variable *function_name, QList<Unboundtor::Variable *> const &args, bool active = true);
			FunctionFilter(FunctionFilter const &cp);
			FunctionFilter *deepCopy() const;
			void deepCopyInitialize(Variable const &original, DeepCopier &deepcopier);
		};

		struct SubSelectPattern;
		struct SubSelectFilter
			: Filter
			, HasPatternInRole<SubSelectPattern>
		{
			Pattern *subSelectPattern() const { return HasPatternInRole<SubSelectPattern>::pattern(); }
			SubSelectFilter(Unboundtor::Pattern *subselect_pattern, bool active = true);
			SubSelectFilter(SubSelectFilter const &cp);
			SubSelectFilter *deepCopy() const;
			void deepCopyInitialize(Variable const &original, DeepCopier &deepcopier);
		};

		struct FilterPattern;
		struct PatternFilter
			: Filter
			, HasPatternInRole<FilterPattern>
		{
			Pattern *filterPattern() const { return HasPatternInRole<FilterPattern>::pattern(); }
			PatternFilter(Unboundtor::Pattern *pattern, bool active = true);
			PatternFilter(PatternFilter const &cp);
			PatternFilter *deepCopy() const;
			void deepCopyInitialize(Variable const &original, DeepCopier &deepcopier);
		};

		enum PatternFlags
		{
			Explicit = 1 << 0,	Optional = 1 << 1
		,	Linked = 1 << 2
		, 	SubSelect = 1 << 3, ExpressionSubSelect = 1 << 4
		,	Exists = 1 << 5, NotExists = 1 << 6
		};

		typedef unsigned PatternFlags;

		struct GraphVariable;

		struct Pattern
			: HasParentUnboundtor<HasPatterns>
			, HasVariable::BackRef
			, HasPattern::BackRef
			, HasVariableInRole<GraphVariable>
		{
			PatternFlags flags_;

			Triples triples;
			Filters filters;

			mutable QSharedPointer<PatternList> unions_, subpatterns_, siblings_;

			Pattern *parent_pattern;
			typedef QSet<QExplicitlySharedDataPointer<RDFSubSelect::Data> > SubSelects;
			SubSelects subselects_;

			QSharedPointer<PatternList> unions() const;
			QSharedPointer<PatternList> siblings() const;
			QSharedPointer<PatternList> subpatterns() const;

			Unboundtor::Pattern *parent(int index);

			Pattern const *findRoot() const;

			void merge(Pattern &pattern, bool not_in_same_tree = false);
			void connectTo(Pattern &pattern);

			Pattern(Unboundtor *query_ = 0, PatternFlags flags = 0);
			~Pattern();

			Pattern(Pattern const &cp);
			Pattern *deepCopy() const;
			void deepCopyInitialize(Pattern const &original, DeepCopier &deepcopier);

			bool isExplicit() const { return flags_ & Explicit; }
			bool isOptional() const { return flags_ & Optional; }
			bool isLinked() const { return flags_ & Linked; }
			bool isSubSelect() const { return flags_ & SubSelect; }
			bool isExpressionSubSelect() const { return flags_ & ExpressionSubSelect; }
			bool isExists() const { return flags_ & Exists; }
			bool isNotExists() const { return flags_ & NotExists; }

			bool isFilterPattern() const { return flags_ & (ExpressionSubSelect | Exists | NotExists); }

			void setExplicit(bool value = true) { if(!value) flags_ &= ~Explicit; else flags_ |= Explicit; }
			void setOptional(bool value = true) { if(!value) flags_ &= ~Optional; else flags_ |= Optional; }
			void setLinked(bool value = true) { if(!value) flags_ &= ~Linked; else flags_ |= Linked; }
			void setSubSelect(bool value = true) { if(!value) flags_ &= ~SubSelect; else flags_ |= SubSelect; }
			void setExpressionSubSelect(bool value = true) { if(!value) flags_ &= ~ExpressionSubSelect; else flags_ |= ExpressionSubSelect; }
			void setExists(bool value = true) { flags_ &= ~(Exists | NotExists); if(value) flags_ |= Exists; }
			void setNotExists(bool value = true) { flags_ &= ~(Exists | NotExists); if(value) flags_ |= NotExists; }

			void detachAsFilterPattern();

			Pattern *child(Pattern *ret = 0, PatternFlags flags = 0, bool not_in_same_tree = false);
			Pattern *union_(Pattern *ret = 0);
			Pattern *optional(Pattern *ret = 0);
			Pattern *graph(Variable *graph_variable, Pattern *ret = 0);

			Variable *existingGraphVariable() const
			{ return HasVariableInRole<GraphVariable>::variable(); }
			Variable *graphVariable(Variable *graph_variable = 0);

			RDFSubSelect subQuery();
		};

		struct VariableLink
			: Variable
			, HasPattern
		{
			RDFVariable link;
			VariableLink(LiveNode const &node = LiveNode());
			~VariableLink();
			VariableLink(VariableLink const &cp);
			VariableLink *deepCopy() const;
			void deepCopyInitialize(Variable const &original, DeepCopier &deepcopier);

			bool isSpecial();
			RDFVariable::Data *linkTarget();
			RDFVariable::Data const *constLinkTarget() const;
			void setLinkTarget(RDFVariable const &new_target);
			Pattern *linkDomainPattern();
			void setLinkDomainPattern(Pattern *domain_pattern, bool register_outbound);
			QVector<RDFProperty> derivedProperties()
			{
				QVector<RDFProperty> link_cacheds = link.derivedProperties();
				if(!link_cacheds.size())
					return derived_properties_;
				if(derived_properties_.size())
					link_cacheds += derived_properties_;
				return link_cacheds;
			}
			Variable const *getDominant() const;
		};

		Unboundtor();
		virtual ~Unboundtor();

		typedef QMap<RDFVariable *, int> OutboundRefs;
		OutboundRefs outbound_refs_;

		void registerOutboundRef(RDFVariable *outbound_ref);
		void unregisterOutboundRef(RDFVariable *outbound_ref);
	};
	typedef Unboundtor::Filter Filter;


	typedef QList<RDFFilter::Data *> Filters;

	/**
	 * Private shared data structure for RDFVariable.
	 * In turn holds a shared reference to an Unboundtor aggregator structure,
	 * as well as direct pointers to a pattern and variable structure inside
	 * the unboundtor.
	 * As a special case, has a null pattern and unboundtor pointers for
	 * trivial, unconnected variables.
	 */
	struct RDFVariable::Data
		: QSharedData
		, SharesAnUnboundtor
		, Unboundtor::HasVariable
		, Unboundtor::HasPattern
	{
		//CppType cpp_type;

		Data(Unboundtor::Variable *variable_ = 0, Unboundtor::Pattern *pattern_ = 0);
		Data(LiveNode const &node, Unboundtor::Pattern *pattern_ = 0);
		Data(Data const &cp, Unboundtor::Pattern *pattern_ = 0);
		virtual ~Data();

		virtual Data *clone() const { return new Data(*this); }

		// used by deep copier.
		// var must be correct, matching type with the class inheriting RDFVariable::Data,
		// as the derived class will be static_cast:ed internally
		virtual RDFVariable::Data *prototype(Unboundtor::Variable *var = 0, Unboundtor::Pattern *pat = 0) const
		{ return new RDFVariable::Data(var, pat); }

		CppType cppType() const;

		virtual void setPattern(Unboundtor::Pattern *to = new Unboundtor::Pattern());
		Unboundtor::Pattern *assertPattern() const;

		static RDFVariable createVariable(RDFVariable::Data *data) { return RDFVariable(data); }

		RDFVariable operator[](QString predicate);

		RDFVariable::Data &property(RDFVariable::Data const &predicate, RDFVariable::Data const &object, bool use_object_pattern) const;

		void connectTo(RDFVariable::Data const &other, bool mark_as_used = true, bool mark_other_as_used = true) const;
		void setAlias(RDFVariable::Data const &alias) const;

		/*!
		 *  Called when given \a variable is made an outbound reference in
		 *  given \a ubtor or if the ubtor is merged with another.
		 *  \return true when self-ref, false if outbound ref
		 */
		static bool containingUnboundtorCheck(RDFVariable *variable, Unboundtor const *new_ubtor);
	};

	struct RDFVariable::DeepCopier::Data
	{
		mutable QMap<Unboundtor *, Unboundtor::DeepCopier> copiers_;
	};


	struct RDFVariableLink::Data
		: RDFVariable::Data
	{
		Data(Unboundtor::VariableLink *link = 0, Unboundtor::Pattern *pat = 0);
		void setPattern(Unboundtor::Pattern *to = new Unboundtor::Pattern());
		Data *clone() const { return new Data(*this); }
		Data *prototype(Unboundtor::Variable *variable_ = 0, Unboundtor::Pattern *pat = 0) const
		{	return new Data(static_cast<Unboundtor::VariableLink *>(variable_), pat); }
	};

	struct RDFFilter::Data
		: RDFVariable::Data
	{
		Data(Unboundtor::Filter *filter = 0, Unboundtor::Pattern *pat = 0);
		Data(Unboundtor::Filter *filter, RDFVariable::Data const &connect_target);
		Data(Unboundtor::Filter::Type type, RDFVariable::Data const &unary, bool active = true);
		Data(Unboundtor::Filter::Type type, RDFVariable::Data const &unary, QString suffix, bool active = true);
		Data(Unboundtor::Filter::Type type, RDFVariable::Data const &lhs, RDFVariable::Data const &rhs, bool active = true);
		~Data();
		Data *clone() const { return new Data(*this); }
		Data *prototype(Unboundtor::Variable *variable_ = 0, Unboundtor::Pattern *pat = 0) const
		{	return new RDFFilter::Data(static_cast<Unboundtor::Filter *>(variable_), pat); }
		static RDFFilter createFilter(RDFFilter::Data *data) { return RDFFilter(data); }

		void registerFilter() const;
	};


	struct RDFProperty::Data
		: QSharedData
	{
		enum { PropertyTripleBound = RDFStrategy::BoundProperty };
		RDFVariable source_, predicate_, target_;
		QSharedPointer<QObject> parent_;

		Data( RDFVariable const &source = RDFVariable()
		    , RDFVariable const &predicate = RDFVariable()
		    , RDFVariable const &target = RDFVariable()
		    , RDFStrategyFlags strategy = RDFStrategy::Disabled
		    , QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());
		Data(RDFProperty::Data const &cp);
		virtual ~Data();

		static RDFProperty::Data *d_func(RDFProperty &property) { return property.d.data(); }
		static RDFProperty::Data const *d_func(RDFProperty const &property) { return property.d.data(); }

		virtual Data *clone() const { return new Data(*this); }
		static SharedNull<Data> shared_null;
	};

	/*
	 * RDFPattern::Data
	 *   doubles as:
	 *   1. explicit D-structure of RDFPattern
	 *   2. implicit data structure for graph patterns generated by default
	 *      constructed RDFVariable:s
	 */
	struct RDFPattern::Data
		: QSharedData
		, SharesAnUnboundtor
		, Unboundtor::HasPattern
	{
		Data(Data const &cp);
		Data(Unboundtor *query_, Unboundtor::PatternFlags flags = 0);
		Data(Unboundtor::Pattern *pattern);
		virtual ~Data();
		static RDFPattern create(RDFPattern::Data *data) { return RDFPattern(data); }

		virtual Data *clone() const { return new Data(*this); }
		static SharedNull<Data> shared_null;
	protected:
		Data() {}
	};


	struct RDFQuery::Data
		: QSharedData
	{
	protected:
		Data() : prefixes(), known_graphs() {}

	public:

		typedef QString Variable;
/*
		struct Pattern
		{
			Pattern *next_union;
			typedef QSet<RDFPattern::Data *> Subsets;
			Subsets subsets;
			QSet<Pattern *> subpatterns, siblings;
			Pattern *parent;
			bool optional;
			bool merged;
			bool traversed;
			Pattern() : next_union(this), optional(false), traversed(false) {}
			Pattern(Pattern const &cp)
				: next_union(cp.next_union == &cp ? this : cp.next_union)
				, parent(0)
				, optional(cp.optional), merged(false), traversed(false)
			{}
			void unionMerge(Pattern &p);
		};

		typedef QSet<Pattern *> Patterns;
*/
		typedef QMap<QString, QUrl> Prefixes;
		Prefixes prefixes;

		typedef QMap<RDFGraph *, QPair<QUrl, QSharedPointer<RDFGraph> > > KnownGraphs;
		KnownGraphs known_graphs;

		Data &operator=(Data const &other);

		virtual Data *clone() const = 0;

		virtual ~Data() {}

		virtual bool isEmpty() const = 0;

		virtual Data *deepCopy(RDFVariable::DeepCopier const &copier) const;

		QString generatePrefixes() const;

		struct WhereContext
		{
			typedef QMap<Variable *, Variable> OuterVariableNames;
			typedef QPair<int, QSet<Unboundtor::Variable const *> > UseData;
			typedef QMap<QString, UseData> VariableUses;
			typedef bool PatternProcessed;
			OuterVariableNames outer_variable_names_remapping_;
			// cache for bound variables in the selection clause
			RDFSelectColumnList select_columns_;
			VariableUses variable_uses_;

			WhereContext pushWhereContext()
			{
				WhereContext ret(*this);
				*this = WhereContext();
				return ret;
			}
			void popWhereContext(WhereContext &stacked)
			{
				for(VariableUses::const_iterator cvui = variable_uses_.constBegin()
						, cvuiend = variable_uses_.constEnd(); cvui != cvuiend; ++cvui)
					popVariableUsesEntry(stacked, cvui);
				*this = stacked;
				stacked = WhereContext();
			}
			void popVariableUsesEntry(WhereContext &target
					, VariableUses::const_iterator const &cvui)
			{
				UseData &use_data = target.variable_uses_[cvui.key()];
				use_data.first += cvui->first;
				use_data.second |= cvui->second;
			}
		};

		struct Builder
			: WhereContext
		{

			typedef QLinkedList<Variable> VariableStorage;
			typedef QMap<Unboundtor::Variable const *, Variable *> VariableIndex;
//			typedef QLinkedList<Pattern> PatternStorage;
//			typedef QMap<Unboundtor::Pattern const *, Pattern *> PatternIndex;
			//			PatternIndex pattern_index;

			Builder(RDFQuery::Data const &query);
			RDFQuery::Data const &query;
			bool is_update_;
			VariableStorage variables;
			VariableIndex variable_index;
			QMap<QString, int> used_names;
			QMap<QString, QString> predicate_conversions;
			QList<RDFSelectColumnList> select_column_cache_;

			typedef QFifoSet<Unboundtor const *> Unboundtors;
			Unboundtors unboundtors_;

			typedef QFifoMap<Unboundtor::Pattern const *, PatternProcessed> PatternProgress;
			PatternProgress pattern_progress_;

			typedef QMap<Unboundtor::Pattern const *, Unboundtor::Pattern const *> PatternLinks;
			PatternLinks pattern_links_;

			typedef QMap<Variable *, Unboundtor::Filter const *> FilterMap;
			FilterMap filter_variables_;

			void prepareQuery(QStringList const &reserved_words = QStringList());

			void addUnboundtor(Unboundtor *query);
			QString uniqueName(QString const &name);
			Variable *prepareVariable(Variable *v, Unboundtor::Variable *var, bool &is_blank);
			void processVariable(Unboundtor::Variable *var);

			void processUnboundtor(Unboundtor const &unboundtor);
			void processUnboundtors(Unboundtors::iterator current);
			void processUnboundtors();

			void registerVariable(RDFVariable::Data const &var);
			void registerVariable(RDFVariable const &var) { return registerVariable(*var.d); }

			virtual QString recurseFilter(Unboundtor::Filter const &filter, int indentation = 0);

			QString *variableName(RDFVariable const &var, bool use = true);
			QString *variableName(RDFVariable::Data const *var, bool use = true);
			QString *variableName(Unboundtor::Variable const *var, bool use = true);

			QString &outerVariableName(RDFVariable const &, bool use = true);

			QString indent(int indentation);
			void recurseLinkedPatterns(PatternProgress::iterator pattern
					, QList<Unboundtor::Pattern const *> &results);

			CanonicalRawTriples prepareTriples
					(RDFStatementList const &statements, bool hide_if_empty = false);
			QString generateTriples(CanonicalRawTriples const &triples, int indentation = 0);

			QString generatePatternClause(PatternProgress::iterator current_pattern
					, int indentation = 0, bool filter_pattern = false);
			QString generateWhereClause(bool is_update, int indentation = 0);

			void prepareSelect(RDFSelect::Data const &query, QStringList const &reserved_words = QStringList());
			WhereContext prepareSubSelect(RDFSelect::Data const &query);
			void unwindSubSelect(WhereContext &stacked_context);

			typedef QString OuterName;
			typedef QString InnerName;
			typedef QPair<OuterName, InnerName> ColumnVariableNames;
			ColumnVariableNames columnVariableNames
					( RDFSelectColumn const &col, int indentation = 0);
			QString generateSelectClause(RDFSelect::Data const &query, int indentation = 0);
			QString generateSolutionModifiers(RDFSelect::Data const &query, int indentation = 0);

			//! Overridable function for returning a query string out of given \a node
			virtual QString nodeString(Soprano::Node const &node) const;

			virtual RDFSelectColumnList buildSelectColumnRange(RDFSelect::Data const &q) const;
			virtual bool processSelectColumn(RDFSelectColumn &column) const;
			//! \return a list of columns used for building the actual query
			RDFSelectColumnList buildSelectColumns(RDFSelect::Data const &q) const;
		};

		virtual QSharedPointer<Builder> createSelectBuilder() const;

	};


	struct RDFSelectColumn::Data
		: QSharedData
	{
		int mode_;
		RDFVariable var_;
		RDFVariable outer_variable_;
		Data(int mode = Normal, RDFVariable const &var = RDFVariable(), RDFVariable const &outer_variable = RDFVariable());

		static SharedNull<Data> shared_null;
		virtual Data *clone() const { return new Data(*this); }
		virtual ~Data() {}
		virtual Data *deepCopy(RDFVariable::DeepCopier const &copier) const;
	};

	struct RDFSelect::Data
		: RDFQuery::Data
	{
		typedef RDFSelectColumnList Columns;
		Columns columns_;
		int offset, limit;
		typedef QVector<int> GroupBy;
		GroupBy group_by;
		typedef QVector<QPair<int, bool> > OrderBy;
		OrderBy order_by;
		RDFVariableList noncolumn_variables;
		enum SelectModifier { NoModifier, Distinct, Reduced };
		int select_modifier;

		mutable RDFPattern pattern_;

		Data(Unboundtor::Pattern *pattern = 0);
		Data(Data const &cp);
		Data(RDFSelectColumnList const &columns);
public:
		~Data();
		static SharedNull<Data> shared_null;
		Data *clone() const { return new Data(*this); }
		Data *deepCopy(RDFVariable::DeepCopier const &copier) const;

		static RDFSelect create(Data *data) { return RDFSelect(data); }

		RDFPattern pattern() const;

		RDFVariable variable(QString const &explicit_name) const;
		RDFVariable variable(RDFVariable const &variable) const;
		RDFVariable variable(int column) const;

		virtual void addColumn(RDFVariable const &column_variable
				, RDFVariable const &outer_identity, RDFSelectColumn::Mode mode);

		virtual void insertGroupBy(unsigned column, unsigned group_by_priority);
		virtual void insertOrderBy(unsigned column, bool ascending, unsigned order_by_priority);

		void preprocessNewColumnVariables
				(RDFVariable::Data const &column_variable
				, RDFVariable::Data const &outer_identity, RDFSelectColumn::Mode mode);

		bool isEmpty() const;

		QString getSPARQLQuery(QStringList const &reserved_words = QStringList()) const;
	};
	typedef RDFSelect::Data RDFSelectPrivate;

	struct RDFSubSelect::Data
		: RDFSelect::Data
	{
		Data(Unboundtor::Pattern *pattern);
		Data(Data const &cp);
public:
		~Data();
		static SharedNull<Data> shared_null;
		Data *clone() const { return new Data(*this); }
		Data *deepCopy(RDFVariable::DeepCopier const &copier) const;
		static RDFSubSelect create(Data *data) { return RDFSubSelect(data); }
		//! called when there are no more RDFSubSelect objects referring to this
		void detached();

		void addColumn(RDFVariable const &column_variable
				, RDFVariable const &outer_identity, RDFSelectColumn::Mode mode);

protected:
		Data();
	};

	struct RDFUpdate::Data
		: RDFQuery::Data
	{
		RDFUpdate::OperationList operations;

protected:
		Data() {}
public:
		static SharedNull<Data> shared_null;
		Data *clone() const { return new Data(*this); }
		Data *deepCopy(RDFVariable::DeepCopier const &copier) const;

		static RDFUpdate create(Data *data) { return RDFUpdate(data); }

		bool isEmpty() const { return operations.isEmpty(); }

		bool mergeOperations(Operation &left, Operation const &right);

		void load(RDFUpdate::Data const &other);
		void appendUpdate(RDFUpdate::Data const &other);


		struct Builder : RDFQuery::Data::Builder
		{
			RDFVariableList deleteds;
			Builder(RDFUpdate::Data const &query);
			void prepareStatement(RDFStatementList const &statement, bool is_delete);
			QString generateTemplate(RDFStatementList const &statement, bool is_delete);
			QString generateOperation(Operation const &operation, QStringList const &reserved_words = QStringList());
		};

		virtual QSharedPointer<RDFUpdate::Data::Builder> createUpdateBuilder() const;

		QString getSPARQLUpdateQuery(QStringList const &reserved_words = QStringList()) const;
	};

}
#endif /* SOPRANOLIVE_RDFUNBOUND_P_H_ */
