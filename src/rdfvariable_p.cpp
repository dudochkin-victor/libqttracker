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
 * rdfvariable_p.cpp
 *
 *  Created on: Jul 21, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include "rdfunbound_p.h"
#include "include/sopranolive/rdfservice.h"
#include "util_p.h"

namespace SopranoLive
{
	Unboundtor::Variable::Variable
			( LiveNode const &node, Unboundtor *unboundtor_, RDFStrategyFlags state)
		: HasParentUnboundtor<HasVariables>(unboundtor_)
		, node(node), state_(state), strategy_(), use_count_(0)
	{
		switch(node.type())
		{
		case Node::ResourceNode: state_ |= Definite; type = CppType::of<LiveNode>(); break;
		case Node::LiteralNode: state_ |= Definite; type = CppType(node.literal().type()); break;
		case Node::EmptyNode: state_ |= Empty;
		case Node::BlankNode: state_ |= Blank; break;
		}
#ifdef QRDFUNBOUND_DEBUG
		if(state & Variable::Link)
			debug(2) << "Unboundtor::Variable created: \tUVarL(" << this << "):"
					 << node.toN3() << ", state:" << QString::number(state_, 16);
		else
			debug(2) << "Unboundtor::Variable created: \tUVar(" << this << "):"
					 << node.toN3() << ", state:" << QString::number(state_, 16)
					 << "\n                              \tUVar(" << this << ")->UBtor("
					 << unboundtor_ << ")";
#endif
	}

	Unboundtor::Variable::Variable(Variable const &cp)
		: HasParentUnboundtor<HasVariables>(0)
		, HasVariable::BackRef()
		, node(cp.node), type(cp.type), state_(cp.state_), strategy_(cp.strategy_)
		// TODO: incorrectly handles self-referring links (they remain pointing to previous
		// targets)
		, derived_properties_(cp.derived_properties_)
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(2) << "Unboundtor::Variable copied: \tUVar(" << this << "):"
				 << node.toN3() << "state" << QString::number(state_, 16)
				 << "\n                             \tUVar(" << this << ")"
			"->UBtor(" << 0 << ")";
#endif
	}

	Unboundtor::Variable *Unboundtor::Variable::deepCopy() const
	{ return new Variable(*this); }
	void Unboundtor::Variable::deepCopyInitialize
			( Variable const &/*original*/, DeepCopier &deepcopier)
	{
		setUnboundtor(deepcopier.new_unboundtor.data());
	}

	Unboundtor::Variable::~Variable()
	{
#ifdef QRDFUNBOUND_DEBUG
		if(static_phase_g != StaticDestruct)
			debug(2) << "Unboundtor::Variable destroyed: \tUVar(" << this << "):"
					 << node.toN3();
#endif
	}


	RDFVariable::Data::Data(LiveNode const &node, Unboundtor::Pattern *pattern_)
		: QSharedData()
		, SharesAnUnboundtor()
		, Unboundtor::HasVariable(new Unboundtor::Variable(node))
		, Unboundtor::HasPattern()
	{
		setPattern(pattern_);
#ifdef QRDFUNBOUND_DEBUG
		debug(2) << "RDFVariable::Data created: \tVarD(" << this << ")"
			"->UVar(" << variable() << "):" << variable()->node.toN3()
				 << "\n                           \tVarD(" << this << ")"
			"->UPat(" << pattern() << ")";
#endif
	}

	RDFVariable::Data::Data(Data const &cp, Unboundtor::Pattern *pattern_)
		: QSharedData()
		, SharesAnUnboundtor(ShUnboundtor(cp.variable()->unboundtor()))
		, Unboundtor::HasVariable(cp.variable())
		, Unboundtor::HasPattern(cp.pattern())
	//		, cpp_type(cp.cpp_type)
	{
		setPattern(pattern_);
		connectTo(const_cast<Data &>(cp), false, false);
#ifdef QRDFUNBOUND_DEBUG
		debug(2) << "RDFVariable::Data copied: \tVarD(" << this << ")"
			"->UVar(" << variable() << "):" << variable()->node.toN3()
				 << "\n                          \tVarD(" << this << ")"
			"->UPat(" << pattern() << ")";
#endif
	}

	RDFVariable::Data::Data(Unboundtor::Variable *variable_, Unboundtor::Pattern *pattern_)
		: QSharedData()
		, SharesAnUnboundtor(ShUnboundtor(variable_ ? variable_->unboundtor() : 0))
		, Unboundtor::HasVariable(variable_ ? variable_ : new Unboundtor::Variable(LiveNode()))
		, Unboundtor::HasPattern()
	//		, cpp_type()
	{
		setPattern(pattern_);
#ifdef QRDFUNBOUND_DEBUG
		debug(2) << "RDFVariable::Data copied: \tVarD(" << this << ")"
			"->UVar(" << variable() << "):" << variable()->node.toN3()
				 << "\n                          \tVarD(" << this << ")"
			"->UPat(" << pattern() << ")";
#endif
	}

	RDFVariable::Data::~Data()
	{
#ifdef QRDFUNBOUND_DEBUG
		if(static_phase_g != StaticDestruct)
			debug(2) << "RDFVariable::Data destroyed: \tVarD(" << this << ")"
				"->UVar(" << variable() << "):" << variable()->node.toN3();
#endif

		// TODO: fix hack fix. variable() should never be 0. But a bug is caused by that.
		if(!variable())
			warning() << "RDFVariable::Data::~Data(): internal error: variable() is null";
		else
		// this serves a dual purpose: deletes the Unboundtor::Variable when
		// variable is not yet associated with an unboundtor, and deletes an
		// outbound referring variable on unboundtor destruction.
		if(!variable()->unboundtor())
			delete variable();
	}

	CppType RDFVariable::Data::cppType() const
	{
		return variable()->type;
	}

	void RDFVariable::Data::setPattern(Unboundtor::Pattern *to)
	{
		if(!to)
			return;
		if(Unboundtor *u = variable()->unboundtor())
		{
			if(u != to->unboundtor())
				u->merge(*to->unboundtor());
		} else
			variable()->registerAllDerivedsAsOutboundRefsIn(to->unboundtor());
		this->Unboundtor::HasPattern::setPattern(to);
#ifdef QRDFUNBOUND_DEBUG
		debug(2) << "RDFVariable::Data pattern set:\tVarD(" << this << ")"
			"->UPat(" << pattern() << ")";
#endif
		setUnboundtorShare(ShUnboundtor(to->unboundtor()));
		this->variable()->setUnboundtor(to->unboundtor());
	}

	Unboundtor::Pattern *RDFVariable::Data::assertPattern() const
	{
		if(!pattern())
			const_cast<Data *>(this)->setPattern();
		return pattern();
	}

	void RDFVariable::Data::connectTo(RDFVariable::Data const &other
			, bool mark_as_used, bool mark_other_as_used) const
	{
		Unboundtor::Variable *v = variable(), *ov = other.variable();

		if(!pattern())
			const_cast<Data &>(*this).setPattern(other.assertPattern());
		else if(!other.pattern())
			const_cast<Data &>(other).setPattern(pattern());
		else if(pattern()->unboundtor() != other.pattern()->unboundtor())
			pattern()->merge(*other.pattern());
		if(!ov->isUnconnected())
			v->setUnconnected(false);
		else if(!v->isUnconnected())
			ov->setUnconnected(false);
		else
		{
			v->setEmpty(false);
			ov->setEmpty(false);
		}
		if(mark_as_used)
		{
#ifdef QRDFUNBOUND_DEBUG
			debug(5) << "Marking VarD(" << this << ")->UVar(" << v << ") "
				"connect use:" << v->node;
#endif
			v->markUse();
		}
		if(mark_other_as_used)
		{
#ifdef QRDFUNBOUND_DEBUG
			debug(5) << "Marking VarD(" << &other << ")->UVar(" << ov << ") "
				"connect use:" << ov->node;
#endif
			ov->markUse();
		}
		if(Unboundtor::Filter *f = v->getFilter())
			f->disableFilter();
		if(Unboundtor::Filter *of = ov->getFilter())
			of->disableFilter();
	}

	bool RDFVariable::Data::containingUnboundtorCheck
			( RDFVariable *variable, Unboundtor const *new_ubtor)
	{
		if(new_ubtor != const_cast<RDFVariable const *>(variable)->d->variable()->unboundtor())
			return false;
		// variable would end up being a self-reference. Detach/clone the
		// RDFVariable::Data and clear the unboundtor shared reference
		RDFVariable::Data *vard = variable->d.data();
#ifdef QRDFUNBOUND_DEBUG
		debug(3) << "VarDL(" << vard << ")->UVarL(" << vard->variable() << ") "
			"containing unboundtor check: self-link, removing self-share";
#endif
		vard->setUnboundtorShare(ShUnboundtor());
		return true;
	}

	void Unboundtor::Variable::setIdentifier(QString const &identifier)
	{
		if(!identifier.size())
			return;
		if(node.isEmpty() || (node.isBlank() && !isExplicit()))
		{
			node = Node(identifier);
			setBlank();
			setExplicit();
		} else
			warning() << "Trying to reset the identifier of an explicit or "
				"non-blank variable, no effect";
	}

	void Unboundtor::Variable::setAlias(Unboundtor::Variable &alias)
	{
		if(this == &alias)
			return;

#ifdef QRDFUNBOUND_DEBUG
		debug(2) << "Unboundtor::Variable::setAlias:\t"
			"UVar(" << this << ") = UVar(" << &alias << ")";
#endif
		if(alias.isSpecial())
		{
			if(isSpecial())
				warning() << "Trying to alias two special variables "
					"(RDFFilter's or RDFVariableLink's), no aliasing performed";
			else
				alias.setAlias(*this);
			return;
		}

		if(alias.isExplicit() && isExplicit())
		{
			warning() << "Trying to alias an explicit variable with special variable, "
				"no aliasing performed";
			return;
		}

#ifdef QRDFUNBOUND_DEBUG
		debug(2) << "                    ::setNode: \t" << node.toN3();
#endif

		if(alias.node.isValid())
		{
			if(alias.node.isBlank() && node.isBlank() && !isExplicit() && !alias.isExplicit())
				node = LiveNode(node.toString() + alias.node.toString());
			else
				node = alias.node;

			if(alias.isDefinite())
			{
				setDefinite();
				setBlank(false);
			} else
			if(alias.isBlank() && !isDefinite())
				setBlank();
		}
		if(alias.isExplicit())
			setExplicit();
		if(!alias.isUnconnected())
			setUnconnected(false);

		if(alias.type.metaTypeId())
			isOfType(alias.type, alias.isStrict());

		if(alias.derived_properties_.size())
			derived_properties_ += alias.derived_properties_;

		// FIXME: bug, this line should be uncommented but it breaks property chains.
		// TODO: revisit property/variable flag passing schemes
		// strategy_ |= alias.strategy_;

		mergeFrom(alias);
	}

	void RDFVariable::Data::setAlias(RDFVariable::Data const &alias) const
	{
	#ifdef QRDFUNBOUND_DEBUG
		debug(2) << "RDFVariable::Data::setAlias: \t"
			"VarD(" << this << ")->UVar(" << variable() << ") "
			"== VarD(" << &alias << ")->UVar(" << alias.variable() << ")";
	#endif
		connectTo(alias, false, false);
		variable()->setAlias(*alias.variable());
#ifdef QRDFUNBOUND_DEBUG
		debug(2) << "RDFVariable::Data::setAlias: \t"
			"VarD(" << this << ")->UVar(" << variable() << ") "
			"== VarD(" << &alias << ")->UVar(" << alias.variable() << ")";
#endif
	}

	void Unboundtor::Variable::isOfType(CppType new_type, bool new_strict)
	{
	#ifndef QT_NO_DEBUG
		if(int previous_constraint = type.metaTypeId())
		{
			if(previous_constraint != new_type.metaTypeId())
			{
				warning() << "SopranoLive::RDFVariable::isOfType: "
					"overriding a previous type constraint(" <<
					QString::number(previous_constraint) << ") with a new constraint (" <<
					new_type.metaTypeId() << ")";
			} else
			if(isStrict() > new_strict)
				return;
		}
	#else
		if (isStrict() && !new_strict)
			return;
	#endif
		type = new_type;
		setStrict(new_strict);
	}

	void Unboundtor::Variable::registerDerivedAsOutboundRef
			(RDFProperty &derived_property, Unboundtor *ubtor)
	{
		RDFProperty::Data *pd = RDFProperty::Data::d_func(derived_property);
		ubtor->registerOutboundRef(&pd->source_);
		ubtor->registerOutboundRef(&pd->predicate_);
		ubtor->registerOutboundRef(&pd->target_);
	}

	void Unboundtor::Variable::unregisterDerivedAsOutboundRef
			(RDFProperty &derived_property, Unboundtor *ubtor)
	{
		RDFProperty::Data *pd = RDFProperty::Data::d_func(derived_property);
		ubtor->unregisterOutboundRef(&pd->source_);
		ubtor->unregisterOutboundRef(&pd->predicate_);
		ubtor->unregisterOutboundRef(&pd->target_);
	}

	void Unboundtor::Variable::addDerivedProperty(RDFProperty &derived_property)
	{
		// mark the derived property variables as outbound references, so
		// that their shared references gets nulled if it becomes a self-ref
		if(Unboundtor *ubtor = unboundtor())
			registerDerivedAsOutboundRef(derived_property, ubtor);
		derived_properties_.push_back(derived_property);
	}

	void Unboundtor::Variable::setDerivedProperties(QVector<RDFProperty> const &derived_properties)
	{
		Unboundtor *ubtor = unboundtor();
		if(ubtor)
			unregisterAllDerivedsAsOutboundRefsIn(ubtor);
		derived_properties_ = derived_properties;
		if(ubtor)
			registerAllDerivedsAsOutboundRefsIn(ubtor);

	}
	void Unboundtor::Variable::unregisterAllDerivedsAsOutboundRefsIn(Unboundtor *ubtor)
	{
		for(QVector<RDFProperty>::iterator dpi = derived_properties_.begin()
					, dpiend = derived_properties_.end()
				; dpi != dpiend; ++dpi)
			unregisterDerivedAsOutboundRef(*dpi, ubtor);
	}
	void Unboundtor::Variable::registerAllDerivedsAsOutboundRefsIn(Unboundtor *ubtor)
	{
		for(QVector<RDFProperty>::iterator dpi = derived_properties_.begin()
					, dpiend = derived_properties_.end()
				; dpi != dpiend; ++dpi)
			registerDerivedAsOutboundRef(*dpi, ubtor);
	}

	QList<QVariant> Unboundtor::Variable::metaAttribute(QString const &attribute_name) const
	{
		return attributes_.values(attribute_name);
	}
	void Unboundtor::Variable::metaInsertAttribute(QString const &attribute_name, QVariant const &value)
	{
		attributes_.insertMulti(attribute_name, value);
	}
	void Unboundtor::Variable::metaRemoveAttribute(QString const &attribute_name)
	{
		attributes_.remove(attribute_name);
	}
	QVariantMap Unboundtor::Variable::metaAttributeMap() const
	{
		return attributes_;
	}
	void Unboundtor::Variable::metaSetAttributeMap(QVariantMap const &attribute_map)
	{
		attributes_ = attribute_map;
	}



	RDFVariable::RDFVariable()
		: d(new RDFVariable::Data(LiveNode()))
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFVariable Var(" << this << ")"
			"->VarD(" << const_cast<RDFVariable const *>(this)->d.data() << ") created";
#endif
	}
	RDFVariable::RDFVariable(LiveNode const &node)
		: d(new RDFVariable::Data(node))
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFVariable Var(" << this << ")"
			"->VarD(" << const_cast<RDFVariable const *>(this)->d.data() << ") created:"
				 << node;
#endif
	}
	RDFVariable::RDFVariable(Node const &node)
		: d(new RDFVariable::Data(node))
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFVariable Var(" << this << ")"
			"->VarD(" << const_cast<RDFVariable const *>(this)->d.data() << ") created:"
				 << node;
#endif
	}
	RDFVariable::RDFVariable(QUrl const &iri)
		: d(new RDFVariable::Data(iri))
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFVariable Var(" << this << ")"
			"->VarD(" << const_cast<RDFVariable const *>(this)->d.data() << ") created:"
				 << iri;
#endif
	}
	RDFVariable::RDFVariable(LiteralValue const &literal)
		: d(new RDFVariable::Data(literal))
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFVariable Var(" << this << ")"
			"->VarD(" << const_cast<RDFVariable const *>(this)->d.data() << ") created:"
				 << literal;
#endif
	}
	RDFVariable::RDFVariable(LiveNodes const &set)
		: d(new RDFVariable::Data(LiveNode()))
	{
		isMemberOf(set);
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFVariable Var(" << this << ")"
			"->VarD(" << const_cast<RDFVariable const *>(this)->d.data() << ") created:"
				 << set;
#endif
	}

	RDFVariable::RDFVariable(RDFStrategyFlags const &strategy, QString const &identifier)
		: d(new RDFVariable::Data(Node(identifier)))
	{
		if(identifier.size())
			metaSetExplicit();
		if(strategy)
			metaSetStrategy(strategy);
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFVariable Var(" << this << ")"
			"->VarD(" << const_cast<RDFVariable const *>(this)->d.data() << ") created:"
				 << strategy << ", " << identifier;
#endif
	}

	RDFVariable::RDFVariable(QString const &identifier)
		: d(new RDFVariable::Data(Node(identifier)))
	{
		if(identifier.size())
			metaSetExplicit();
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFVariable Var(" << this << ")"
			"->VarD(" << const_cast<RDFVariable const *>(this)->d.data() << ") created:"
				 << identifier;
#endif
	}

	RDFVariable::RDFVariable(const char *identifier)
		: d(new RDFVariable::Data(Node(QString(identifier))))
	{
		if(*identifier != 0)
			metaSetExplicit();
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFVariable Var(" << this << ")"
			"->VarD(" << const_cast<RDFVariable const *>(this)->d.data() << ") created:"
				<< identifier;
#endif
	}

	/*	RDFVariable::RDFVariable(LiveNodes const &iri)
		: d(new RDFVariable::Data(node))
	{}
	*/
	RDFVariable::RDFVariable(RDFVariable const &cp)
		: d(cp.d->unboundtorShare() || !cp.d->variable()->unboundtor()
				? cp.d
				: QSharedDataPointer<Data>(new RDFVariable::Data(*cp.d.data())))
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFVariable Var(" << this << ")"
			"->VarD(" << const_cast<RDFVariable const *>(this)->d.data() << ") copied "
			"from Var(" << &cp << ")";
#endif
	}
	RDFVariable::RDFVariable(RDFVariable::Data *data)
		: d(data)
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFVariable Var(" << this
				 << ")->VarD(" << const_cast<RDFVariable const *>(this)->d.data() << ") created";
#endif
	}
	RDFVariable::~RDFVariable()
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFVariable Var(" << this
				 << ")->VarD(" << const_cast<RDFVariable const *>(this)->d.data() << ") destroyed";
#endif
	}

	void RDFVariable::swap(RDFVariable &other)
	{
		qSwap(d, other.d);
	}

	RDFVariable RDFVariable::fromInstance(LiveNode const &node)
	{
		return RDFVariable(node);
	}

	RDFVariable RDFVariable::fromInstance(LiveNodes const &nodes)
	{
		return nodes;
	}

	RDFVariable RDFVariable::createEmpty()
	{
		RDFVariable ret;
		ret.d->variable()->setEmpty();
		return ret;
	}

	bool RDFVariable::metaIsEmpty() const
	{
		return d->variable()->isEmpty();
	}

	bool RDFVariable::metaIsUnconstrained() const
	{
		return d->variable()->isUnconnected();
	}

	bool RDFVariable::metaIsUnconstrainedBlank() const
	{
		return d->variable()->isUnconstrainedBlank();
	}

	bool RDFVariable::metaIsConstrainedBlank() const
	{
		return d->variable()->isConstrainedBlank();
	}

	bool RDFVariable::metaIsExplicit() const
	{
		return d->variable()->isExplicit();
	}

	void RDFVariable::metaSetExplicit(bool explicit_)
	{
		return d->variable()->setExplicit(explicit_);
	}

	bool RDFVariable::metaIsDefinite() const
	{
		return d->variable()->isDefinite();
	}

	bool RDFVariable::metaIsBlank() const
	{
		return d->variable()->isBlank();
	}

	bool RDFVariable::metaIsExpression() const
	{
		return d->variable()->isExpression();
	}

	void RDFVariable::metaClear()
	{
		d = d->prototype();
	}

	void RDFVariable::metaAssign(RDFVariable const &rhs)
	{
		d = rhs.d;
	}

	LiveNode RDFVariable::metaValue() const
	{
		return d->variable()->node;
	}

	LiveNode RDFVariable::metaExplicitValue() const
	{
		return d->variable()->node;
	}

	QString RDFVariable::metaIdentifier() const
	{
		LiveNode const &node = d->variable()->node;
		return node.isBlank() ? node.toString() : QString();
	}

	void RDFVariable::metaSetIdentifier(QString const &identifier)
	{
		d->variable()->setIdentifier(identifier);
	}

	QList<QVariant> RDFVariable::metaAttribute(QString const &attribute_name) const
	{
		return d->variable()->metaAttribute(attribute_name);
	}

	void RDFVariable::metaInsertAttribute(QString const &attribute_name, QVariant const &value)
	{
		return d->variable()->metaInsertAttribute(attribute_name, value);
	}

	void RDFVariable::metaRemoveAttribute(QString const &attribute_name)
	{
		return d->variable()->metaRemoveAttribute(attribute_name);
	}

	QVariantMap RDFVariable::metaAttributeMap() const
	{
		return d->variable()->metaAttributeMap();
	}

	void RDFVariable::metaSetAttributeMap(QVariantMap const &attribute_map)
	{
		return d->variable()->metaSetAttributeMap(attribute_map);
	}

	RDFVariable::DeepCopier::DeepCopier()
		: d(new DeepCopier::Data())
	{}

	RDFVariable::DeepCopier::DeepCopier(DeepCopier const &cp)
		: d(new DeepCopier::Data(*cp.d))
	{}


	RDFVariable::DeepCopier::~DeepCopier()
	{
		delete d;
	}

	RDFVariable RDFVariable::DeepCopier::copy(RDFVariable const &copy) const
	{
		RDFVariable::Data const *cd = copy.d.data();
		Unboundtor::Variable *var;
		if(!cd || !(var = cd->variable()))
			return copy;
		Unboundtor *ubtor = var->unboundtor();
		if(!ubtor)
			return RDFVariable(cd->prototype(cd->variable()->deepCopy()));
		Unboundtor::DeepCopier &copier = d->copiers_[ubtor];
		if(!copier.new_unboundtor)
			copier.initialize(*ubtor, this);
		return copier.copy(copy);
	}

	RDFPattern RDFVariable::DeepCopier::copy(RDFPattern const &copy) const
	{
		RDFPattern::Data const *cd = copy.d.data();
		// null or shared_null
		Unboundtor::Pattern *pat;
		if(!cd || !(pat = cd->pattern()))
			return copy;
		Unboundtor *ubtor = pat->unboundtor();
		Unboundtor::DeepCopier &copier = d->copiers_[ubtor];
		if(!copier.new_unboundtor)
			copier.initialize(*ubtor, this);
		return copier.copy(copy);
	}

	RDFSubSelect RDFVariable::DeepCopier::copy(RDFSubSelect const &copy) const
	{
		RDFSubSelect::Data const *cd = static_cast<RDFSubSelect::Data const *>(copy.d.data());
		// null or shared_null
		if(cd == &RDFSubSelect::Data::shared_null)
			return RDFSubSelect::Data::create(&RDFSubSelect::Data::shared_null);
		Unboundtor *ubtor = cd->pattern_.d->unboundtorShare().data();
		Unboundtor::DeepCopier &copier = d->copiers_[ubtor];
		if(!copier.new_unboundtor)
			copier.initialize(*ubtor, this);
		return copier.copy(copy);
	}

	RDFVariableList RDFVariable::DeepCopier::copy(RDFVariableList const &variables) const
	{
		return copySequence<RDFVariableList>(variables);
	}

	RDFVariable RDFVariable::deepCopy(DeepCopier const &copier) const
	{
		return copier.copy(*this);
	}

	RDFVariableList RDFVariable::deepCopy
			(RDFVariableList const &variables, DeepCopier const &copier)
	{
		return copier.copy(variables);
	}

	RDFPattern RDFVariable::pattern() const
	{
		return RDFPattern(new RDFPattern::Data(d->assertPattern()));
	}

	void RDFVariable::setPattern(RDFPattern const &pattern)
	{
		d->setPattern(pattern.d->pattern());
	}

	RDFVariable::Data &RDFVariable::Data::property
			( RDFVariable::Data const &predicate, RDFVariable::Data const &object
			, bool use_object_pattern) const
	{
		QString hint = predicate.variable()->node.toString();

		connectTo(predicate);
		connectTo(object, false);

		(use_object_pattern
			? object.pattern()
			: pattern())
				->triples.push_back(Unboundtor::Triple
						(variable(), predicate.variable(), object.variable()));
#ifdef QRDFUNBOUND_DEBUG
		xdebug(2) {
			log << "RDFPattern::Data triple insert:\tUPat(" << pattern() << "):";
			log << "           subject:\tUVar(" << variable() << "):"
				<< variable()->node.toN3();
			log << "         predicate:\tUVar(" << predicate.variable() << "):"
				<< predicate.variable()->node.toN3();
			log << "            object:\tUVar(" << object.variable() << "):"
				<< object.variable()->node.toN3();
		}
#endif
		variable()->isOfType(CppType::of<LiveNode>(), false);
		predicate.variable()->isOfType(CppType::of<LiveNode>(), false);
		variable()->setUnconnected(false);
		predicate.variable()->setUnconnected(false);
		object.variable()->setUnconnected(false);
		// hack..
		return const_cast<RDFVariable::Data &>(object);
	}


	RDFVariable RDFVariable::property(RDFVariable const &predicate) const
	{
	#ifndef QT_NO_DEBUG
		QString hint = predicate.d->variable()->node.toString();
		hint = "a_" + hint.mid(hint.lastIndexOf("/") + 1);
		return RDFVariable(&d->property(*predicate.d, *new RDFVariable::Data
				(LiveNode(hint), d->pattern()), false));
	#else
		return RDFVariable(&d->property(*predicate.d, *new RDFVariable::Data
				(LiveNode(), d->pattern()), false));
	#endif
	}

	RDFVariable RDFVariable::bindProperty(RDFProperty &property_data) const
	{
		if(property_data.strategy() & RDFStrategy::Object)
			return bindObject(property_data);
		if(property_data.strategy() & RDFStrategy::Subject)
			return bindSubject(property_data);
		// TODO: should we emit a warning here?
		return property_data.target();
	}
	RDFVariable RDFVariable::property(RDFProperty &property_data) const
	{
		return bindProperty(property_data);
	}
	RDFVariable RDFVariable::property(RDFProperty const &object_data) const
	{
		RDFProperty prop = object_data.deepCopy();
		return bindProperty(prop);
	}

	RDFVariable RDFVariable::bindObject(RDFProperty &object_data) const
	{
		RDFStrategyFlags prop_strat = object_data.strategy();
		RDFVariable const object = object_data.target();
		RDFVariable const pred = object_data.predicate();
		RDFVariable(&d->property(*pred.d, *object.d, false));
		RDFVariable(object).metaEnableStrategyFlags
				(prop_strat & RDFStrategy::PropertyToColumnFlagsMask);
		if(prop_strat & RDFStrategy::DerivedProperty)
			addDerivedProperty(object_data);
		return object;
	}
	RDFVariable RDFVariable::object(RDFProperty &object_data) const
	{
		return bindObject(object_data);
	}
	RDFVariable RDFVariable::object(RDFProperty const &object_data) const
	{
		RDFProperty prop(object_data);
		return bindObject(prop);
	}

	RDFVariable RDFVariable::bindSubject(RDFProperty &subject_data) const
	{
		RDFStrategyFlags prop_strat = subject_data.strategy();
		RDFVariable const subject = subject_data.target();
		RDFVariable const pred = subject_data.predicate();
		subject.d->property(*pred.d, *d, true);
		RDFVariable(subject).metaEnableStrategyFlags
				(prop_strat & RDFStrategy::PropertyToColumnFlagsMask);
		if(prop_strat & RDFStrategy::DerivedProperty)
			addDerivedProperty(subject_data);
		return subject;
	}
	RDFVariable RDFVariable::subject(RDFProperty &subject_data) const
	{
		return bindSubject(subject_data);
	}
	RDFVariable RDFVariable::subject(RDFProperty const &subject_data) const
	{
		RDFProperty prop(subject_data);
		return bindSubject(prop);
	}


	RDFVariable RDFVariable::type() const
	{
		return type(RDFStrategy::BoundDerivedProperty);
	}
	RDFVariable RDFVariable::type(RDFStrategyFlags strategy) const
	{
		return type(RDFVariable(), strategy);
	}
	RDFVariable RDFVariable::type(RDFVariable const &type, RDFStrategyFlags strategy) const
	{
		RDFProperty type_prop = RDFProperty::fromObjectOf(rdf::type::iri(), type, strategy);
		RDFVariable const ret =
				((strategy & RDFStrategy::BoundDerivedProperty) == RDFStrategy::DerivedProperty)
			? addDerivedProperty(type_prop)
			: bindObject(type_prop);
		d->variable()->isOfType(CppType::of<LiveNode>(), false);
		ret.d->variable()->isOfType(CppType::of<LiveNode>(), false);
		return ret;
	}

	RDFVariable RDFVariable::isOfType(CppType type, bool strict) const
	{
		// TODO: pending abi break: change to non-const
		// const_cast<QSharedDataPointer<RDFVariable::Data> &>(d).detach();
		d->variable()->isOfType(type, strict);
		return *this;
	}

	CppType RDFVariable::varCppType() const
	{
		return d->cppType();
	}

	bool RDFVariable::metaIsResource() const
	{
		return varCppType().metaTypeId() == CppType::of<LiveNode>().metaTypeId();
	}

	RDFVariable RDFVariable::addDerivedProperty(RDFProperty &property_data) const
	{
		d->variable()->addDerivedProperty(property_data);
		return property_data.target();
	}
	RDFVariable RDFVariable::addDerivedProperty(RDFProperty const &property_data) const
	{
		RDFProperty prop(property_data);
		return addDerivedProperty(prop);
	}

	RDFVariable RDFVariable::addDerivedObject
			( RDFVariable const &predicate, RDFVariable const &object_info
			, RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent) const
	{
		RDFProperty prop
				( predicate, object_info, strategy | RDFStrategy::PredicateObject, parent);
		return addDerivedProperty(prop);
	}

	RDFVariable RDFVariable::addDerivedSubject
			( RDFVariable const &predicate, RDFVariable const &subject_info
			, RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent) const
	{
		RDFProperty prop
				( predicate, subject_info, strategy | RDFStrategy::SubjectPredicate, parent);
		return addDerivedProperty(prop);
	}

	RDFVariable RDFVariable::addUpdateObject
			( RDFVariable const &predicate, RDFVariable const &object_info
			, RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent) const
	{
		RDFProperty prop
				( predicate, object_info
				, strategy | RDFStrategy::PredicateObject | RDFStrategy::UpdateProperty, parent);
		return addDerivedProperty(prop);
	}

	RDFVariable RDFVariable::addUpdateSubject
			( RDFVariable const &predicate, RDFVariable const &subject_info
			, RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent) const
	{
		RDFProperty prop
				( predicate, subject_info
				, strategy | RDFStrategy::SubjectPredicate | RDFStrategy::UpdateProperty, parent);
		return addDerivedProperty(prop);
	}

	QVector<RDFProperty> RDFVariable::derivedProperties() const
	{
		return d->variable()->derivedProperties();
	}

	void RDFVariable::setDerivedProperties(QVector<RDFProperty> const &derived_properties) const
	{
		d->variable()->setDerivedProperties(derived_properties);
	}

#if (QTTRACKER_SUPPORT_VER <= 603)
	RDFVariable RDFVariable::metaAddCachedProperty(RDFProperty &property_data) const
	{
		return addDerivedProperty(property_data);
	}
	RDFVariable RDFVariable::metaAddCachedProperty(RDFProperty const &property_data) const
	{
		return addDerivedProperty(property_data);
	}

	QVector<RDFProperty> RDFVariable::metaCachedProperties() const
	{
		return derivedProperties();
	}
#endif

	void RDFVariable::metaSetStrategy(RDFStrategyFlags strategy)
	{
		d->variable()->setStrategy(strategy);
	}

	//! Sets associated strategy flags
	RDFStrategyFlags RDFVariable::metaStrategy() const
	{
		return d->variable()->strategy();
	}

	void RDFVariable::metaEnableStrategyFlags(RDFStrategyFlags strategy)
	{
		RDFVariable::Data const *d = const_cast<RDFVariable const *>(this)->d.data();
		d->variable()->setStrategy(d->variable()->strategy() | strategy);
	}

	void RDFVariable::metaDisableStrategyFlags(RDFStrategyFlags strategy)
	{
		RDFVariable::Data const *d = const_cast<RDFVariable const *>(this)->d.data();
		d->variable()->setStrategy(d->variable()->strategy() & ~strategy);
	}

    RDFVariable RDFVariable::variable(QString explicit_name) const
    {
    	return pattern().variable(explicit_name);
    }

    RDFVariable RDFVariable::variable(RDFVariable const &variable) const
    {
    	return pattern().variable(variable);
    }

	RDFVariable RDFVariable::child() const
	{
		return RDFVariable(new RDFVariable::Data(*d, d->assertPattern()->child()));
	}

	RDFVariable RDFVariable::child(RDFVariable const &variable) const
	{
		d->pattern()->child(variable.d->pattern());
		return variable;
	}

	RDFVariable RDFVariable::union_() const
	{
		return RDFVariable(new RDFVariable::Data(*d, d->assertPattern()->union_()));
	}

	RDFVariable RDFVariable::union_(RDFVariable const &variable) const
	{
		d->pattern()->union_(variable.d->pattern());
		return variable;
	}

	void RDFVariable::unionChildren(RDFVariableList const &children) const
	{
		unionMerge(children);
	}

	void RDFVariable::unionMerge(RDFVariableList const &union_choices) const
	{
		if(!union_choices.size())
			return;
		RDFVariableList::const_iterator ich = union_choices.begin();
		RDFVariableList::const_iterator ichend = union_choices.end();
		RDFVariable first_child = child();
		first_child.merge(*ich);
		while(++ich != ichend)
			first_child.union_().merge(*ich);
	}

	void RDFVariable::mergeUnion(RDFVariableList const &union_choices) const
	{
		return unionMerge(union_choices);
	}


	RDFVariableList RDFVariable::unionChildren(unsigned count) const
	{
		RDFVariableList ret;
		while(count--)
			ret.push_back(RDFVariable());
		unionMerge(ret);
		return ret;
	}

	RDFVariable RDFVariable::optional() const
	{
		return RDFVariable(new RDFVariable::Data(*d, d->assertPattern()->optional()));
	}

	RDFVariable RDFVariable::optional(RDFVariable const &variable) const
	{
		d->assertPattern()->optional(variable.d->assertPattern());
		return variable;
	}

	RDFVariable RDFVariable::inParent() const
	{
		if(d->pattern())
			if(Unboundtor::Pattern *parent = d->pattern()->parent_pattern)
				return RDFVariable::Data::createVariable
						(new RDFVariable::Data(d->variable(), parent));
		warning() << "RDFVariable::inParent: "
			"the pattern of this variable does not have a parent pattern";
		return RDFVariable();
	}

	RDFVariable RDFVariable::inParent(int index) const
	{
		return RDFVariable::Data::createVariable(new RDFVariable::Data
				(d->variable(), d->assertPattern()->parent(index)));
	}

	RDFVariable RDFVariable::inGraph(RDFVariable const &graph) const
	{
		d->connectTo(*graph.d);
		return RDFVariable(new RDFVariable::Data
				(*d, d->assertPattern()->graph(graph.d->variable())));
	}

	RDFVariable RDFVariable::inExists() const
	{
		RDFPattern exists = pattern().child();
		exists.exists();
		return exists.variable(*this);
	}

	RDFVariable RDFVariable::inNotExists() const
	{
		RDFPattern not_exists = pattern().child();
		not_exists.notExists();
		return not_exists.variable(*this);
	}

	RDFVariable RDFVariable::aliasIn(RDFPattern const &pattern) const
	{
		return pattern.variable(*this);
	}


	void RDFVariable::setEqualTo(RDFVariable const &var) const
	{
		d->setAlias(*var.d);
	}

	Unboundtor::Filter::Filter(Type type, bool active, Variable const *dominant)
		: Unboundtor::Variable
				( LiveNode(QString("filter")), 0
				, Variable::Filter | (active ? RDFStrategyFlags(Active) : 0))
		, filtertype_(type), dominant_(dominant)
	{}

	Unboundtor::Filter::Filter(LiveNode const &node)
		: Unboundtor::Variable(node)
		, filtertype_(identity), dominant_()
	{}

	Unboundtor::Filter::Filter(Unboundtor::Filter const &cp)
		: Unboundtor::Variable(cp)
		, filtertype_(cp.filtertype_), dominant_(cp.dominant_)
	{}

	void Unboundtor::Filter::useFilterIn(Unboundtor::Pattern *pattern)
	{
		if(pattern)
		{
#ifdef QRDFUNBOUND_DEBUG
		debug(2) << "UPat(" << pattern << "): filter inserted: UVar(" << this << ")";
#endif

			pattern->filters.append(this);
		}
		disableFilter();
	}

	Unboundtor::UnaryFilter::UnaryFilter
			( Filter::Type type, Unboundtor::Variable *var, bool active)
		: Filter(type, active, var->getDominant())
		, HasVariable(var)
	{
		// TODO: can in principle be removed, connectTo already disables filters
		// not removing this now because I'm time limited and dont want to break anything
			var->disableFilter();
	}

	Unboundtor::UnaryFilter::UnaryFilter(UnaryFilter const &cp) : Filter(cp), HasVariable() {}
	Unboundtor::UnaryFilter *Unboundtor::UnaryFilter::deepCopy() const
	{ return new UnaryFilter(*this); }
	void Unboundtor::UnaryFilter::deepCopyInitialize
			(Variable const &original, DeepCopier &deepcopier)
	{
		Filter::deepCopyInitialize(original, deepcopier);
		setVariable(deepcopier.vars[static_cast<UnaryFilter const &>(original).variable()]);
	}

	Unboundtor::UnarySuffixFilter::UnarySuffixFilter
			( Filter::Type type, Unboundtor::Variable *var, QString suffix, bool active)
		: UnaryFilter(type, var, active), suffix(suffix)
	{}

	Unboundtor::UnarySuffixFilter::UnarySuffixFilter(UnarySuffixFilter const &cp)
		: UnaryFilter(cp), suffix(cp.suffix) {}
	Unboundtor::UnarySuffixFilter *Unboundtor::UnarySuffixFilter::deepCopy() const
	{ return new UnarySuffixFilter(*this); }

	Unboundtor::BinaryFilter::BinaryFilter
			( Filter::Type type, Unboundtor::Variable *lhs, Unboundtor::Variable *rhs
			, bool active)
		: Filter(type, active)
		, HasVariableInRole<BinaryFilterLHS>(lhs)
		, HasVariableInRole<BinaryFilterRHS>(rhs)
	{
		// TODO: can in principle be removed, connectTo already disables filters
		// not removing this now because I'm time limited and dont want to break anything
			lhs->disableFilter();
			rhs->disableFilter();

		if(!(dominant_ = lhs->getDominant()))
			dominant_ = rhs->getDominant();
	}

	Unboundtor::BinaryFilter::BinaryFilter(BinaryFilter const &cp)
		: Filter(cp)
		, HasVariableInRole<BinaryFilterLHS>(0)
		, HasVariableInRole<BinaryFilterRHS>(0)
	{}
	Unboundtor::BinaryFilter *Unboundtor::BinaryFilter::deepCopy() const
	{ return new BinaryFilter(*this); }
	void Unboundtor::BinaryFilter::deepCopyInitialize
			( Variable const &original, DeepCopier &deepcopier)
	{
		Filter::deepCopyInitialize(original, deepcopier);
		BinaryFilter const &orig = static_cast<BinaryFilter const &>(original);
		HasVariableInRole<BinaryFilterLHS>::setVariable(deepcopier.vars[orig.lhs()]);
		HasVariableInRole<BinaryFilterRHS>::setVariable(deepcopier.vars[orig.rhs()]);
	}

	Unboundtor::PolyFilter::PolyFilter(PolyFilter const &cp) : Filter(cp) {}
	Unboundtor::PolyFilter *Unboundtor::PolyFilter::deepCopy() const
	{ return new PolyFilter(*this); }
	void Unboundtor::PolyFilter::deepCopyInitialize
			( Variable const &original, DeepCopier &deepcopier)
	{
		Filter::deepCopyInitialize(original, deepcopier);
		PolyFilter const &orig = static_cast<PolyFilter const &>(original);
		for(QLinkedList<HasVariable>::const_iterator ai = orig.arguments.begin()
				, aiend = orig.arguments.end(); ai != aiend; ++ai)
		{
			arguments.push_back(HasVariable());
			arguments.back().setVariable(deepcopier.vars[ai->variable()]);
		}
	}

	Unboundtor::PolyFilter::PolyFilter
			( Filter::Type type, QList<Unboundtor::Variable *> const &args, bool active)
		: Filter(type, active)
	{
		for(QList<Unboundtor::Variable *>::const_iterator vi = args.begin(), viend = args.end()
				; vi != viend; ++vi)
		{
			// TODO: can in principle be removed, connectTo already disables filters
			// not removing this now because I'm time limited and dont want to break anything
				(*vi)->disableFilter();

			if(!dominant_)
				dominant_ = (*vi)->getDominant();
			arguments.push_back(HasVariable());
			arguments.back().setVariable(*vi);
		}
	}

	Unboundtor::MemberFilter::MemberFilter
			( Unboundtor::Variable *member, QList<Unboundtor::Variable *> const &set, bool active)
		: PolyFilter(Filter::isMemberOf, set, active)
		, HasVariableInRole<MemberFilterMember>(member)
	{
		if(Variable const *new_dominant = member->getDominant())
			dominant_ = new_dominant;
	}

	Unboundtor::MemberFilter::MemberFilter(MemberFilter const &cp)
		: PolyFilter(cp), HasVariableInRole<MemberFilterMember>() {}
	Unboundtor::MemberFilter *Unboundtor::MemberFilter::deepCopy() const
	{ return new MemberFilter(*this); }
	void Unboundtor::MemberFilter::deepCopyInitialize
			( Variable const &original, DeepCopier &deepcopier)
	{
		PolyFilter::deepCopyInitialize(original, deepcopier);
		HasVariableInRole<MemberFilterMember>::setVariable
				( deepcopier.vars[static_cast<MemberFilter const &>(original).member()]);
	}

	Unboundtor::FunctionFilter::FunctionFilter
			( Unboundtor::Variable *function_name, QList<Unboundtor::Variable *> const &args
			, bool active)
		: PolyFilter(Filter::function, args, active)
		, HasVariableInRole<FunctionFilterName>(function_name)
	{}

	Unboundtor::FunctionFilter::FunctionFilter(FunctionFilter const &cp)
		: PolyFilter(cp), HasVariableInRole<FunctionFilterName>() {}
	Unboundtor::FunctionFilter *Unboundtor::FunctionFilter::deepCopy() const
	{ return new FunctionFilter(*this); }
	void Unboundtor::FunctionFilter::deepCopyInitialize
			( Variable const &original, DeepCopier &deepcopier)
	{
		PolyFilter::deepCopyInitialize(original, deepcopier);
		HasVariableInRole<FunctionFilterName>::setVariable
				( deepcopier.vars[static_cast<FunctionFilter const &>(original).name()]);
	}

	Unboundtor::SubSelectFilter::SubSelectFilter
			(Unboundtor::Pattern *subselect_pattern, bool active)
		: Filter(Filter::subSelectExpression, active)
		, HasPatternInRole<SubSelectPattern>(subselect_pattern)
	{}

	Unboundtor::SubSelectFilter::SubSelectFilter(SubSelectFilter const &cp)
		: Filter(cp), HasPatternInRole<SubSelectPattern>()
	{}

	Unboundtor::SubSelectFilter *Unboundtor::SubSelectFilter::deepCopy() const
	{ return new SubSelectFilter(*this); }

	void Unboundtor::SubSelectFilter::deepCopyInitialize
			( Variable const &original, DeepCopier &deepcopier)
	{
		Filter::deepCopyInitialize(original, deepcopier);
		HasPatternInRole<SubSelectPattern>::setPattern
				( deepcopier.pats[static_cast<SubSelectFilter const &>(original)
						.subSelectPattern()]);
	}

	Unboundtor::PatternFilter::PatternFilter
			(Unboundtor::Pattern *pattern, bool active)
		: Filter(Filter::patternExpression, active)
		, HasPatternInRole<FilterPattern>(pattern)
	{}

	Unboundtor::PatternFilter::PatternFilter(PatternFilter const &cp)
		: Filter(cp), HasPatternInRole<FilterPattern>()
	{}

	Unboundtor::PatternFilter *Unboundtor::PatternFilter::deepCopy() const
	{ return new PatternFilter(*this); }

	void Unboundtor::PatternFilter::deepCopyInitialize
			( Variable const &original, DeepCopier &deepcopier)
	{
		Filter::deepCopyInitialize(original, deepcopier);
		HasPatternInRole<FilterPattern>::setPattern
				( deepcopier.pats[static_cast<PatternFilter const &>(original)
						.filterPattern()]);
	}

	Unboundtor::VariableLink::VariableLink(LiveNode const &node)
		: Variable(node, 0, Variable::Link), HasPattern(0)
	{}
	Unboundtor::VariableLink::~VariableLink()
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(2) << "VariableLink destroyed: \tUVarL(" << this << ")";
#endif
	}
	// TODO: incorrectly handles self-referring links (they remain pointing to origin)
	Unboundtor::VariableLink::VariableLink(VariableLink const &cp)
			: Variable(cp), HasPattern(0), link(cp.link) {}
	Unboundtor::VariableLink *Unboundtor::VariableLink::deepCopy() const
	{ return new VariableLink(*this); }
	void Unboundtor::VariableLink::deepCopyInitialize
			( Variable const &original, DeepCopier &deepcopier)
	{
		Unboundtor::Variable::deepCopyInitialize(original, deepcopier);
		if(Unboundtor::Pattern * domain
				= static_cast<Unboundtor::VariableLink const &>(original).pattern())
			setPattern(deepcopier.pats[domain]);
	}

	bool Unboundtor::VariableLink::isSpecial()
	{
		return true;
	}
	RDFVariable::Data *Unboundtor::VariableLink::linkTarget()
	{
		return link.d.data();
	}
	RDFVariable::Data const *Unboundtor::VariableLink::constLinkTarget() const
	{
		return link.d.data();
	}
	void Unboundtor::VariableLink::setLinkTarget(RDFVariable const &new_target)
	{
		if(Unboundtor *ubtor = unboundtor())
		{
			ubtor->unregisterOutboundRef(&link);
			link.d = new_target.d;
			ubtor->registerOutboundRef(&link);
		} else
			link.d = new_target.d;
//		link.metaClear();
//		link = new_target;
//		node = new_target.d->variable()->node;
		type = new_target.d->variable()->type;
		setStrict(new_target.d->variable()->isStrict());
		setUnconnected(new_target.d->variable()->isUnconnected());
	}

	Unboundtor::Pattern *Unboundtor::VariableLink::linkDomainPattern()
	{
		return pattern();
	}
	void Unboundtor::VariableLink::setLinkDomainPattern
			( Pattern *domain_pattern, bool register_outbound)
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(2) << "Unboundtor::VariableLink::"
			"setDomain of UVarL(" << this << ") to UPat(" << domain_pattern << ")";
#endif
		if(register_outbound)
			unboundtor()->registerOutboundRef(&link);
		setPattern(domain_pattern);
		domain_pattern->setLinked();
	}


	Unboundtor::Variable const *Unboundtor::VariableLink::getDominant() const
	{
		return link.d->variable()->getDominant();
	}

	RDFFilter::Data::Data(Unboundtor::Filter *filter, Unboundtor::Pattern *pat)
		: RDFVariable::Data(filter ? filter : new Unboundtor::Filter(), pat)
	{
		static_cast<Unboundtor::Filter *>(variable())->setActive();
	}

	RDFFilter::Data::Data(Unboundtor::Filter *filter, RDFVariable::Data const &connect_target)
		: RDFVariable::Data(filter ? filter : new Unboundtor::Filter())
	{
		connectTo(connect_target);
		static_cast<Unboundtor::Filter *>(variable())->setActive();
	}


	RDFFilter::Data::Data
			( Unboundtor::Filter::Type type, RDFVariable::Data const &unary, bool active)
		: RDFVariable::Data(new Unboundtor::UnaryFilter(type, unary.variable(), active))
	{
		connectTo(unary);
		static_cast<Unboundtor::Filter *>(variable())->setActive();
	}
	RDFFilter::Data::Data
			( Unboundtor::Filter::Type type, RDFVariable::Data const &unary, QString suffix
			, bool active)
		: RDFVariable::Data(new Unboundtor::UnarySuffixFilter(type, unary.variable(), suffix, active))
	{
		connectTo(unary);
		static_cast<Unboundtor::Filter *>(variable())->setActive();
	}
	RDFFilter::Data::Data
			( Unboundtor::Filter::Type type, RDFVariable::Data const &lhs
			, RDFVariable::Data const &rhs, bool active)
		: RDFVariable::Data(new Unboundtor::BinaryFilter
				(type, lhs.variable(), rhs.variable(), active))
	{
		lhs.connectTo(rhs, false);
		connectTo(lhs);
		static_cast<Unboundtor::Filter *>(variable())->setActive();
	}
	RDFFilter::Data::~Data()
	{
		registerFilter();
	}

	void RDFFilter::Data::registerFilter() const
	{
		if(static_cast<Unboundtor::Filter *>(variable())->isActive() && pattern())
			static_cast<Unboundtor::Filter *>(variable())->useFilterIn(pattern());
	}

	RDFFilter::RDFFilter(LiteralValue const &constant)
		: RDFVariable(new RDFFilter::Data(new Unboundtor::Filter(constant)))
	{}

	RDFFilter::RDFFilter(RDFVariable const &constant)
		: RDFVariable(new RDFFilter::Data(Unboundtor::Filter::identity, *constant.d))
	{}

	RDFFilter::RDFFilter(RDFFilter const &cp)
		: RDFVariable(cp)
	{}

	RDFFilter::RDFFilter(RDFFilter::Data *d)
		: RDFVariable(d)
	{}

	RDFFilter::~RDFFilter()
	{}

	void RDFFilter::metaAssign(RDFFilter const &rhs)
	{
		d = rhs.d;
	}

	void RDFFilter::useFilterIn(RDFPattern const *pattern) const
	{
		static_cast<Unboundtor::Filter *>(d->variable())->useFilterIn
				(pattern ? pattern->d->pattern() : 0);
	}
	void RDFFilter::disable() const
	{
		// TODO implement
	}
	void RDFFilter::enable() const
	{
		// TODO implement
	}

	RDFVariable RDFFilter::firstVar() const
	{
		static_cast<RDFFilter::Data const &>(*d).registerFilter();
		return RDFVariable::Data::createVariable
				(new RDFVariable::Data(d->variable()->getDominantOrThis(), d->pattern()));
	}




	RDFVariableLink::Data::Data(Unboundtor::VariableLink *link, Unboundtor::Pattern *pat)
		: RDFVariable::Data(link ? link : new Unboundtor::VariableLink(), pat)
	{}

	void RDFVariableLink::Data::setPattern(Unboundtor::Pattern *to)
	{
		bool register_outbound = !variable()->unboundtor();
		RDFVariable::Data::setPattern(to);
		static_cast<Unboundtor::VariableLink *>(variable())->setLinkDomainPattern
				(to, register_outbound);
	}

	RDFVariableLink::RDFVariableLink()
		: RDFVariable(new RDFVariableLink::Data())
	{}

	RDFVariableLink::RDFVariableLink(RDFVariable const &alias)
		: RDFVariable(new RDFVariableLink::Data())
	{
		switchTarget(alias);
	}

	RDFVariableLink::RDFVariableLink(RDFVariableLink const &cp)
		: RDFVariable(cp)
	{}

	void RDFVariableLink::metaAssign(RDFVariableLink const &rhs)
	{
		d = rhs.d;
	}

	void RDFVariableLink::switchTarget(RDFVariable const &new_link_target)
	{
#ifdef QRDFUNBOUND_DEBUG
		xdebug(2) {
			log << "RDFVariableLink::switchTarget:\t"
				"VarDL(" << d.data() << ")->UVarL(" << d->variable() << "): ";
			log << "                             :\t"
				"VarDL(" << d.data() << ")->UPat(" << d->pattern() << "): ";
			log << "                       target: "
				"-> VarD(" << new_link_target.d.data() << ")->UVarL("
				<< new_link_target.d->variable() << ")";
			log << "                             : "
				"-> VarD(" << new_link_target.d.data() << ")->UPat("
				<< new_link_target.d->pattern() << ")";
		}
#endif
		static_cast<Unboundtor::VariableLink *>(d->variable())->setLinkTarget(new_link_target);
	}

	RDFFilter RDFVariable::isTrue() const
	{
		return RDFFilter(new RDFFilter::Data(Filter::identity, *d));
	}

	void setEscaped(unsigned char *lookup, unsigned char char_)
	{ lookup[char_ >> 3] |= (1 << (char_ & 7)); }
	bool checkEscaped(unsigned char *lookup, unsigned char char_)
	{ return (lookup[char_ >> 3] & (1 << (char_ & 7))) != 0; }
	bool checkEscaped(unsigned char *lookup, QCharRef char_)
	{ return checkEscaped(lookup, char_.toAscii()); }

#define BYTECOUNT_TO_HOLD_256_BITS 32

	unsigned char single_escape_lookup[BYTECOUNT_TO_HOLD_256_BITS];
	unsigned char regexp_escape_lookup[BYTECOUNT_TO_HOLD_256_BITS];
	unsigned char single_escape_characters[] =
	{ '\t', '\n', '\r', '\b', '\f', '\"', '\'', '\\' };

	unsigned char regexp_escape_characters[] =
	{ '.', '?', '*', '+', '{', '}', '(', ')', '[', ']', '^', '$', '\\' };

	unsigned int initializeEscapeLookups()
	{

		for(unsigned i = 0; i < BYTECOUNT_TO_HOLD_256_BITS; ++i)
		{
			single_escape_lookup[i] = 0;
			regexp_escape_lookup[i] = 0;
		}
		for(unsigned i = 0; i < sizeof(single_escape_characters); ++i)
			setEscaped(single_escape_lookup, single_escape_characters[i]);
		for(unsigned i = 0; i < sizeof(regexp_escape_characters); ++i)
			setEscaped(regexp_escape_lookup, regexp_escape_characters[i]);
		return 0;
	}
	unsigned escape_lookups_initializer = initializeEscapeLookups();

	QString escapeRegexpString(QString string)
	{
		QString ret;
		for(int i = 0; i < string.size(); ++i)
		{
			char ch = string[i].toAscii();
			if(ch == '\\')
				ret += "\\\\";
			else if(checkEscaped(single_escape_lookup, ch))
				(ret += "\\") += ch;
			else if(checkEscaped(regexp_escape_lookup, ch))
				(ret += "\\\\") += ch;
			else
				ret += ch;
		}
		return ret;
	}

	QString escapeString(QString string)
	{
		QString ret;
		for(int i = 0; i < string.size(); ++i)
		{
			char ch = string[i].toAscii();
			if(checkEscaped(single_escape_lookup, ch))
				ret += "\\" + ch;
			else
				ret += ch;
		}
		return ret;
	}

	// TODO: Tracker dependent code
	// implement more efficientlierly?
	// Requires the addition of an extension engine
	RDFFilter RDFVariable::hasPrefix(QString const &prefix) const
	{
		return RDFFilter(new RDFFilter::Data
				(Filter::hasPrefix, *d, ", \"^" + escapeRegexpString(prefix) + "\""));
	}
	RDFFilter RDFVariable::hasSuffix(QString const &suffix) const
	{
		return RDFFilter(new RDFFilter::Data
				(Filter::hasSuffix, *d, ", \"" + escapeRegexpString(suffix) + "$\""));
	}
	RDFFilter RDFVariable::contains(QString const &infix) const
	{
		return RDFFilter(new RDFFilter::Data
				(Filter::contains, *d, ", \"" + escapeRegexpString(infix) + "\""));
	}
	RDFFilter RDFVariable::matchesRegexp(QString const &pattern) const
	{
		return RDFFilter(new RDFFilter::Data
				(Filter::matchesRegexp, *d, ", \"" + escapeString(pattern) + "\""));
	}
	RDFFilter RDFVariable::isMemberOf(RDFVariableList const &set) const
	{
		QList<Unboundtor::Variable *> args;
		for(RDFVariableList::const_iterator vi = set.begin(), viend = set.end()
				; vi != viend; ++vi)
		{
			d->connectTo(*vi->d, false);
			args << vi->d->variable();
		}
		return RDFFilter(new RDFFilter::Data
				(new Unboundtor::MemberFilter(d->variable(), args), *d));
	}

	RDFFilter RDFVariable::not_() const
	{
		return RDFFilter(new RDFFilter::Data(Filter::not_, *d));
	}
	RDFFilter RDFVariable::and_(RDFVariable const &rhs) const
	{
		return RDFFilter(new RDFFilter::Data(Filter::and_, *d, *rhs.d));
	}
	RDFFilter RDFVariable::or_(RDFVariable const &rhs) const
	{
		return RDFFilter(new RDFFilter::Data(Filter::or_, *d, *rhs.d));
	}

	RDFFilter RDFVariable::equal(RDFVariable const &rhs) const
	{
		return RDFFilter(new RDFFilter::Data(Filter::equal, *d, *rhs.d));
	}
	RDFFilter RDFVariable::notEqual(RDFVariable const &rhs) const
	{
		return RDFFilter(new RDFFilter::Data(Filter::notEqual, *d, *rhs.d));
	}
	RDFFilter RDFVariable::less(RDFVariable const &rhs) const
	{
		return RDFFilter(new RDFFilter::Data(Filter::less, *d, *rhs.d));
	}
	RDFFilter RDFVariable::greater(RDFVariable const &rhs) const
	{
		return RDFFilter(new RDFFilter::Data(Filter::greater, *d, *rhs.d));
	}
	RDFFilter RDFVariable::lessOrEqual(RDFVariable const &rhs) const
	{
		return RDFFilter(new RDFFilter::Data(Filter::lessOrEqual, *d, *rhs.d));
	}
	RDFFilter RDFVariable::greaterOrEqual(RDFVariable const &rhs) const
	{
		return RDFFilter(new RDFFilter::Data(Filter::greaterOrEqual, *d, *rhs.d));
	}

	RDFVariable RDFVariable::inv() const
	{
		return RDFFilter(new RDFFilter::Data(Filter::inv, *d, false));
	}
	RDFVariable RDFVariable::add(RDFVariable const &rhs) const
	{
		return RDFFilter(new RDFFilter::Data(Filter::add, *d, *rhs.d, false));
	}
	RDFVariable RDFVariable::sub(RDFVariable const &rhs) const
	{
		return RDFFilter(new RDFFilter::Data(Filter::sub, *d, *rhs.d, false));
	}
	RDFVariable RDFVariable::mul(RDFVariable const &rhs) const
	{
		return RDFFilter(new RDFFilter::Data(Filter::mul, *d, *rhs.d, false));
	}
	RDFVariable RDFVariable::div(RDFVariable const &rhs) const
	{
		return RDFFilter(new RDFFilter::Data(Filter::div, *d, *rhs.d, false));
	}
	RDFFilter RDFVariable::filter(QString const &function_name) const
	{
		return filter(function_name, RDFVariableList());
	}
	RDFFilter RDFVariable::filter(QString const &function_name, RDFVariable const &arg2) const
	{
		return filter(function_name, RDFVariableList() << arg2);
	}
	RDFFilter RDFVariable::filter
			( QString const &function_name, RDFVariable const &arg2
			, RDFVariable const &arg3) const
	{
		return filter(function_name, RDFVariableList() << arg2 << arg3);
	}
	RDFFilter RDFVariable::filter
			( QString const &function_name, RDFVariableList const &args_tail) const
	{
		return function(RDFVariable(function_name), args_tail);
	}

	RDFFilter RDFVariable::function(RDFVariable const &function_name) const
	{
		return function(function_name, RDFVariableList());
	}
	RDFFilter RDFVariable::function
			( RDFVariable const &function_name, RDFVariable const &arg2) const
	{
		return function(function_name, RDFVariableList() << arg2);
	}
	RDFFilter RDFVariable::function
			( RDFVariable const &function_name, RDFVariable const &arg2
			, RDFVariable const &arg3) const
	{
		return function(function_name, RDFVariableList() << arg2 << arg3);
	}
	RDFFilter RDFVariable::function
			( RDFVariable const &function_name, RDFVariableList const &args_tail) const
	{
		QList<Unboundtor::Variable *> args;
		args << d->variable();
		for(RDFVariableList::const_iterator ti = args_tail.begin(), tiend = args_tail.end()
				; ti != tiend; ++ti)
		{
			d->connectTo(*ti->d, false);
			args << ti->d->variable();
		}
		d->connectTo(*function_name.d, false);
		return RDFFilter(new RDFFilter::Data
				(new Unboundtor::FunctionFilter(function_name.d->variable(), args), *d));
	}

	RDFFilter RDFVariable::isBound() const
	{
		return filter("bound");
	}

	QDebug operator<<(QDebug s, RDFVariable const &value)
	{
		s.nospace() << value.metaValue();
		return s;
	}

}
