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
 * rdfunbound_p.cpp
 *
 *  Created on: Dec 25, 2008
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include <QtCore>
#include <QtDBus>
#include <QByteArray>
#include <QtDebug>
#include <QThreadStorage>
#include <QHash>
#include <QTime>
#include <algorithm>
#include <QScopedPointer>
#include "rdfunbound_p.h"
#include "include/sopranolive/rdfservice.h"
#include "util_p.h"

namespace SopranoLive
{
	Unboundtor::Unboundtor()
	{
#ifdef QRDFUNBOUND_DEBUG
		if(static_phase_g != StaticDestruct)
			debug(4) << "Unboundtor created:          \tUBtor(" << this << ")";
#endif
	}

	Unboundtor::~Unboundtor()
	{
#ifdef QRDFUNBOUND_DEBUG
		if(static_phase_g != StaticDestruct)
			debug(4) << "Deleting UBtor(" << this << ")";
#endif

		QSet<HasParentUnboundtor<HasVariables> *> const &vars = variables();
		QSet<HasParentUnboundtor<HasPatterns> *> const &pats = patterns();

		for(OutboundRefs::iterator ori = outbound_refs_.begin(), oriend = outbound_refs_.end(); ori != oriend; ++ori)
		{
#ifdef QRDFUNBOUND_DEBUG
			RDFVariable const *cori = ori.key();
			if(static_phase_g != StaticDestruct)
				debug(4) << "UBtor(" << this << "): Zeroing outbound reference Var(" << ori.key() << ")"
						"->VarD(" << cori->d.data() << ")"
						"->UVar(" << cori->d->variable() << ")";
#endif
			ori.key()->d = 0;
		}

		while(vars.size())
		{
#ifdef QRDFUNBOUND_DEBUG
			if(static_phase_g != StaticDestruct)
				debug(4) << "UBtor(" << this << "): Removing child variable: " << *vars.begin();
#endif
			delete (*vars.begin());
		}
		while(pats.size())
		{
#ifdef QRDFUNBOUND_DEBUG
			if(static_phase_g != StaticDestruct)
				debug(4) << "UBtor(" << this << "): Removing child pattern: " << *pats.begin();
#endif
			delete (*pats.begin());
		}
	}

	void Unboundtor::registerOutboundRef(RDFVariable *outbound_ref)
	{
#ifdef QRDFUNBOUND_DEBUG
		xdebug(4) {
			RDFVariable const *coref = outbound_ref;
			log << "UBtor(" << this << "): Registering outbound reference Var(" << coref << ")"
					"->VarD(" << coref->d.data() << ")"
					"->UVar(" << coref->d->variable() << ")";
		}
#endif
		outbound_refs_[outbound_ref]++;
		RDFVariable::Data::containingUnboundtorCheck(outbound_ref, this);
	}
	void Unboundtor::unregisterOutboundRef(RDFVariable *outbound_ref)
	{
#ifdef QRDFUNBOUND_DEBUG
		xdebug(4) {
			RDFVariable const *coref = outbound_ref;
			log << "UBtor(" << this << "): Unregistering outbound reference Var(" << coref << ")"
				"->VarD(" << coref->d.data() << ")"
				"->UVar(" << coref->d->variable() << ")";
		}
#endif
		OutboundRefs::iterator out = outbound_refs_.find(outbound_ref);
		if(out != outbound_refs_.end() && --out.value() <= 0)
			outbound_refs_.erase(out);
	}

	void Unboundtor::merge(Unboundtor &other)
	{
		if(this == &other)
			return;
		ShUnboundtor this_holder(this), other_holder(&other);
#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "Merging Unboundtor lhs(" << this << ") with Unboundtor rhs(" << &other << ")";
		debug(5) << " \tlhs had" << HasVariables::backrefs().size() << "vars and" << HasPatterns::backrefs().size() << "pats before merge";
		debug(5) << " \trhs had" << other.HasVariables::backrefs().size() << "vars and" << other.HasPatterns::backrefs().size() << "pats before merge";
#endif

		// merge variable, patterns and reference holders from other to this
		// Their back references will automatically be removed from the other
		this->HasVariables::mergeFrom(other);
		this->HasPatterns::mergeFrom(other);
		this->SharesAnUnboundtor::BackRef::mergeFrom(other);

		// merge outbound references from other to this.
		{
			for(OutboundRefs::const_iterator coori = other.outbound_refs_.constBegin()
					, cooriend = other.outbound_refs_.constEnd(); coori != cooriend; ++coori)
				outbound_refs_[coori.key()] += coori.value();
			for(OutboundRefs::const_iterator cori = outbound_refs_.constBegin(), coriend = outbound_refs_.constEnd()
					; cori != coriend; ++cori)
				RDFVariable::Data::containingUnboundtorCheck(cori.key(), this);
			// manually clear the other outbound refs as there is no automation here.
			other.outbound_refs_.clear();
		}

#ifdef QRDFUNBOUND_DEBUG
		debug(5) << " \tresult has" << HasVariables::backrefs().size() << "vars and" << HasPatterns::backrefs().size() << "pats after merge";
#endif
	}

	void Unboundtor::DeepCopier::initialize(Unboundtor const &deepcopied
			, RDFVariable::DeepCopier const *owner)
	{
		new_unboundtor = new Unboundtor();
		owner_ = owner;

#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "Copying UBtor(" << new_unboundtor << ") from "
				"UBtor(" << &deepcopied << "): with" << deepcopied.variables().size()
				<< "variables and" << deepcopied.patterns().size() << "patterns";
#endif
		for(HasVariables::Set::const_iterator vi = deepcopied.variables().begin(), viend = deepcopied.variables().end(); vi != viend; ++vi)
		{
			Variable *var = static_cast<Variable *>(*vi);
			vars[var] = var->deepCopy();
#ifdef QRDFUNBOUND_DEBUG
			debug(4) << " \tCopied UVar(" << var << ") into UVar(" << vars[var] << ")";
#endif
		}

		for(HasPatterns::Set::const_iterator pi = deepcopied.patterns().begin(), piend = deepcopied.patterns().end(); pi != piend; ++pi)
		{
			Pattern *pat = static_cast<Pattern *>(*pi);
			pats[pat] = pat->deepCopy();
#ifdef QRDFUNBOUND_DEBUG
			debug(4) << " \tCopied UPat(" << pat << ") into UPat(" << pats[pat] << ")";
#endif
		}

		for(Pats::iterator cpi = pats.begin(), cpiend = pats.end(); cpi != cpiend; ++cpi)
		{
#ifdef QRDFUNBOUND_DEBUG
			debug(4) << " \tIterating UPat(" << cpi.value() << ")<-UPat(" << cpi.key() << ")";
#endif
			cpi.value()->deepCopyInitialize(*cpi.key(), *this);
		}

		for(Vars::iterator cvi = vars.begin(), cviend = vars.end(); cvi != cviend; ++cvi)
		{
#ifdef QRDFUNBOUND_DEBUG
			debug(4) << " \tIterating UVar(" << cvi.value() << ")<-UVar(" << cvi.key() << ")";
#endif
			cvi.value()->deepCopyInitialize(*cvi.key(), *this);
		}
	}

	RDFVariable Unboundtor::DeepCopier::copy(RDFVariable const &var)
	{
		return RDFVariable::Data::createVariable(var.d->prototype(vars[var.d->variable()], pats[var.d->pattern()]));
	}

	RDFPattern Unboundtor::DeepCopier::copy(RDFPattern const &pat)
	{
		return RDFPattern::Data::create(new RDFPattern::Data(pats[pat.d->pattern()]));
	}

	RDFSubSelect Unboundtor::DeepCopier::copy(RDFSubSelect const &subsel)
	{
		return RDFSubSelect::Data::create(subsels[subsel.d.data()]);
	}

	Unboundtor::Triple::Triple(Variable *subject, Variable *predicate, Variable *object)
		: HasVariableInRole<TripleSubject>(subject)
		, HasVariableInRole<TriplePredicate>(predicate)
		, HasVariableInRole<TripleObject>(object)
	{}

	void Unboundtor::Triple::deepCopyInitialize(Triple const &original, DeepCopier &deepcopier)
	{
		HasVariableInRole<TripleSubject>::setProperty(deepcopier.vars[original.subject()]);
		HasVariableInRole<TriplePredicate>::setProperty(deepcopier.vars[original.predicate()]);
		HasVariableInRole<TripleObject>::setProperty(deepcopier.vars[original.object()]);
	}

	Unboundtor::Variable *Unboundtor::Triple::subject() const { return HasVariableInRole<TripleSubject>::variable(); }
	Unboundtor::Variable *Unboundtor::Triple::predicate() const { return HasVariableInRole<TriplePredicate>::variable(); }
	Unboundtor::Variable *Unboundtor::Triple::object() const { return HasVariableInRole<TripleObject>::variable(); }


	Unboundtor::Pattern::Pattern(Unboundtor *unboundtor_, PatternFlags flags)
		: HasParentUnboundtor<HasPatterns>(unboundtor_ ? unboundtor_ : new Unboundtor())
		, flags_(flags)
		, parent_pattern(0)
		, subselects_()
	{
#ifdef QRDFUNBOUND_DEBUG
		if(static_phase_g != StaticDestruct)
			debug(4) << "Unboundtor::Pattern created: \tUPat(" << this << ")";
#endif
	}

	Unboundtor::Pattern::~Pattern()
	{
#ifdef QRDFUNBOUND_DEBUG
		if(static_phase_g != StaticDestruct)
			debug(4) << "Unboundtor::Pattern destroyed: \tUPat(" << this << ")";
#endif
	}

	Unboundtor::Pattern::Pattern(Pattern const &cp)
		: HasParentUnboundtor<HasPatterns>()
		, HasVariable::BackRef()
		, HasPattern::BackRef()
		, HasVariableInRole<GraphVariable>()
		, flags_(cp.flags_)
		, parent_pattern(0)
		, subselects_()
	{
#ifdef QRDFUNBOUND_DEBUG
		if(static_phase_g != StaticDestruct)
			debug(4) << "Unboundtor::Pattern copied: \tUPat(" << this << ") from UPat(" << &cp << ")";
#endif
	}

	Unboundtor::Pattern *Unboundtor::Pattern::deepCopy() const { return new Pattern(*this); }
	void Unboundtor::Pattern::deepCopyInitialize(Pattern const &original, DeepCopier &deepcopier)
	{
		setUnboundtor(deepcopier.new_unboundtor.data());

		for(Triples::const_iterator ti = original.triples.begin(), tiend = original.triples.end(); ti != tiend; ++ti)
		{
			triples.push_back(Triple());
			triples.back().deepCopyInitialize(*ti, deepcopier);
		}
		for(Filters::const_iterator fi = original.filters.begin(), fiend = original.filters.end(); fi != fiend; ++fi)
			filters.push_back(static_cast<Filter *>(deepcopier.vars[*fi]));

		if(original.unions_)
			if(!(unions_ = deepcopier.patlists[original.unions_.data()]))
			{
				unions_ = deepcopier.patlists[original.unions_.data()] = QSharedPointer<PatternList>(new PatternList());
				for(PatternList::iterator pi = original.unions_->begin(), piend = original.unions_->end(); pi != piend; ++pi)
				{
#ifdef QRDFUNBOUND_DEBUG
					if(!deepcopier.pats[*pi])
						debug(2) << "Invalid pattern!";
#endif
					unions_->push_back(deepcopier.pats[*pi]);
				}
			}
		if(original.subpatterns_)
			if(!(subpatterns_ = deepcopier.patlists[original.subpatterns_.data()]))
			{
				subpatterns_ = deepcopier.patlists[original.subpatterns_.data()] = QSharedPointer<PatternList>(new PatternList());
				for(PatternList::iterator pi = original.subpatterns_->begin(), piend = original.subpatterns_->end(); pi != piend; ++pi)
					subpatterns_->push_back(deepcopier.pats[*pi]);
			}
		if(original.siblings_)
			if(!(siblings_ = deepcopier.patlists[original.siblings_.data()]))
			{
				siblings_ = deepcopier.patlists[original.siblings_.data()] = QSharedPointer<PatternList>(new PatternList());
				for(PatternList::iterator pi = original.siblings_->begin(), piend = original.siblings_->end(); pi != piend; ++pi)
					siblings_->push_back(deepcopier.pats[*pi]);
			}

		if(original.parent_pattern)
			parent_pattern = deepcopier.pats[original.parent_pattern];

		if(original.subselects_.size())
			for(SubSelects::const_iterator cssi = original.subselects_.begin()
					, cssiend = original.subselects_.end(); cssi != cssiend; ++cssi)
			{
				RDFSubSelect::Data *&subsel = deepcopier.subsels[cssi->data()];
				if(!subsel)
					subsel = cssi->data()->deepCopy(*deepcopier.owner_);
			}

		if(Variable *graph_var = original.existingGraphVariable())
			HasVariableInRole<GraphVariable>::setVariable(deepcopier.vars[graph_var]);
	}

	SharedNull<RDFProperty::Data> RDFProperty::Data::shared_null;

	RDFPattern::Data::Data(Data const &cp)
		: QSharedData()
		, SharesAnUnboundtor(cp.unboundtorShare())
		, Unboundtor::HasPattern(cp.pattern())
	{
		if(!cp.pattern())
		{
			ShUnboundtor ubtor(new Unboundtor());
			setUnboundtorShare(ubtor);
			setPattern(new Unboundtor::Pattern(ubtor.data()));
		}
#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "RDFPattern::Data PatD(" << this << ") "
				"copied from" << (&cp == &shared_null ? "shared null" : "")
				<< " PatD(" << &cp << ")";
		debug(5) << "                 PatD(" << this << ")->UPat(" << pattern() << ")";
#endif
	}

	RDFPattern::Data::Data(Unboundtor::Pattern *pattern)
		: SharesAnUnboundtor(ShUnboundtor(pattern->unboundtor()))
		, Unboundtor::HasPattern(pattern)
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "RDFPattern::Data PatD(" << this << ") "
				"initialized from UPatD(" << pattern << ")";
		debug(5) << "                 PatD(" << this << ")->UPat(" << pattern << ")";
#endif
	}

	RDFPattern::Data::Data(Unboundtor *unboundtor_, Unboundtor::PatternFlags flags_)
		: SharesAnUnboundtor(ShUnboundtor(unboundtor_ ? unboundtor_ : new Unboundtor()))
		, Unboundtor::HasPattern(new Unboundtor::Pattern(unboundtorShare().data(), flags_))
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "RDFPattern::Data PatD(" << this << ") "
				"initialized from UBtor(" << unboundtor_ << ")";
		debug(5) << "                 PatD(" << this << ")->UPat(" << pattern() << ")";
#endif
	}

	RDFPattern::Data::~Data()
	{
#ifdef QRDFUNBOUND_DEBUG
		if(static_phase_g != StaticDestruct)
			debug(4) << "RDFPattern::Data destroyed: PatD(" << this << ")->UPat(" << pattern() << ")";
#endif
	}

	SharedNull<RDFPattern::Data> RDFPattern::Data::shared_null;

	QSharedPointer<Unboundtor::PatternList> Unboundtor::Pattern::unions() const
	{
		if(!unions_)
		{
			unions_ = QSharedPointer<PatternList>(new PatternList);
			unions_->push_back(this);
		}
		return unions_;
	}
	QSharedPointer<Unboundtor::PatternList> Unboundtor::Pattern::siblings() const
	{
		if(!siblings_)
			siblings_ = QSharedPointer<PatternList>(new PatternList);

		return siblings_;
	}
	QSharedPointer<Unboundtor::PatternList> Unboundtor::Pattern::subpatterns() const
	{
		if(!subpatterns_)
			subpatterns_ = QSharedPointer<PatternList>(new PatternList);
		return subpatterns_;
	}

	Unboundtor::Pattern const *Unboundtor::Pattern::findRoot() const
	{
		Unboundtor::Pattern const *root_search = this;
		do
		{
			while(root_search->parent_pattern)
				root_search = root_search->parent_pattern;
			if(root_search->unions_)
				for(Unboundtor::PatternList::const_iterator i = root_search->unions_->begin(), iend = root_search->unions_->end(); i != iend; ++i)
					if((root_search = *i)->parent_pattern)
						break;
		}while(root_search->parent_pattern);
		return root_search;
	}

	void Unboundtor::Pattern::merge(Unboundtor::Pattern &pattern, bool not_in_same_tree)
	{
		if(this == &pattern)
			return;

		if(unboundtor() != pattern.unboundtor())
			unboundtor()->merge(*pattern.unboundtor());

#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "Unboundtor::Pattern::merge between UPat(" << this << ") and UPat(" << &pattern << ")";
#endif
		if(isExplicit() && pattern.isExplicit())
		{
#ifdef QRDFUNBOUND_DEBUG
			debug(4) << "Unboundtor::Pattern::\tboth explicit, made into siblings";
#endif
			pattern.siblings()->push_back(this);
			siblings()->push_back(&pattern);
		} else if(not_in_same_tree || findRoot() != pattern.findRoot())
		{
			flags_ |= pattern.flags_;
			triples += pattern.triples;
			filters += pattern.filters;

			if(pattern.unions_)
			{
				if(!pattern.unions_->count(this))
				{
					unions()->operator+=(*pattern.unions_);
					PatternList const unions = *pattern.unions_;
					for(PatternList::const_iterator iu = unions.begin(), iuend = unions.end()
							; iu != iuend; ++iu)
						(*iu)->unions_ = unions_;
				}
				unions_->removeAll(&pattern);
			}

			if(pattern.siblings_)
			{
				siblings()->operator+=(*pattern.siblings_);
				for(PatternList::iterator si = pattern.siblings_->begin(), siend = pattern.siblings_->end()
						; si != siend; ++si)
					*std::find((*si)->siblings_->begin(), (*si)->siblings_->end(), &pattern) = this;
			}

			if(pattern.subpatterns_)
			{
				subpatterns()->operator+=(*pattern.subpatterns_.data());
				for(PatternList::iterator spi = pattern.subpatterns_->begin(), spiend = pattern.subpatterns_->end()
						; spi != spiend; ++spi)
					if(*spi == (const_cast<Pattern *>(*spi)->parent_pattern = this))
						warning() << "Unboundtor::Pattern::merge: infinite recursion: subpattern equals subpattern parent, UPat(" << *spi << ")";
			}

			if(pattern.parent_pattern)
			{
				if(!parent_pattern)
				{
					if(this == (parent_pattern = pattern.parent_pattern))
						warning() << "Unboundtor::Pattern::merge: infinite recursion: pattern equals parent, UPat(" << this << ")";
					parent_pattern->subpatterns()->push_back(this);
				}
				pattern.parent_pattern->subpatterns()->removeAll(&pattern);
			}

			subselects_ += pattern.subselects_;

			HasVariable::BackRef::mergeFrom(pattern);
			HasPattern::BackRef::mergeFrom(pattern);

#ifdef QRDFUNBOUND_DEBUG
			xdebug(5) {
				log << " \tcarrying out, after merge the resulting UPat(" << this << "):";
				log << " \tis" << (isExplicit() ? "explicit" : "not explicit")
					<< (isOptional() ? ", optional" : ", not optional")
					<< (isLinked() ? ", linked" : ", not linked")
					<< (isSubSelect() ? ", subselect" : ", not subselect");
				log << " \thas" << triples.size() << "triples and" << filters.size() << "filters";
				if(unions_)
				{
					log << " \thas" << unions_->size() << "unions:";
					foreach(Unboundtor::Pattern const *p, *unions_)
						log << " \t\tUPat(" << p << ")";
				}
				if(siblings_)
				{
					log << " \thas" << siblings_->size() << "siblings:";
					foreach(Unboundtor::Pattern const *p, *siblings_)
						log << " \t\tUPat(" << p << ")";
				}
				if(subpatterns_)
				{
					log << " \thas" << subpatterns_->size() << "subpatterns:";
					foreach(Unboundtor::Pattern const *p, *subpatterns_)
						log << " \t\tUPat(" << p << ")";
				}
				if(parent_pattern)
				{
					log << " \thas parent pattern UPat(" << parent_pattern << ")";
				}
			}
#endif

			delete &pattern;
		}
	}

	void Unboundtor::Pattern::connectTo(Unboundtor::Pattern &other)
	{
		if(unboundtor() != other.unboundtor())
			merge(other);
	}

	void Unboundtor::Pattern::detachAsFilterPattern()
	{
		if(parent_pattern)
		{
			parent_pattern->subpatterns()->removeAll(this);
			parent_pattern = 0;
		}
		if(unions_)
		{
			unions_->removeAll(this);
			unions_.clear();
		}
		if(siblings_)
		{
			siblings_->removeAll(this);
			siblings_.clear();
		}
	}

	Unboundtor::Pattern *Unboundtor::Pattern::child(Unboundtor::Pattern *ret, PatternFlags flags, bool not_in_same_tree)
	{
		if(!ret)
			ret = new Unboundtor::Pattern(unboundtor(), flags | Unboundtor::Explicit);
		else
		{
			Unboundtor::Pattern *old_parent = ret->parent_pattern;
			ret->parent_pattern = 0;
			if(!not_in_same_tree && findRoot() == ret)
			{
				ret->parent_pattern = old_parent;
#ifdef QRDFUNBOUND_DEBUG
				debug(2) << "Unboundtor::Pattern::child: cannot make UPat(" << ret << ") child of UPat(" << this << ") as it is direct ancestor";
#endif
				return 0;
			}
			if(old_parent)
				old_parent->subpatterns()->removeAll(ret);
		}
		ret->parent_pattern = this;
		subpatterns()->push_back(ret);
#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "Unboundtor::Pattern::child: made UPat(" << ret << ") child of UPat(" << this << ")";
#endif
		return ret;
	}
	RDFPattern RDFPattern::child() const
	{
		return RDFPattern(new RDFPattern::Data(d->pattern()->child()));
	}
	RDFPattern RDFPattern::child(RDFPattern const &child) const
	{
		d->pattern()->child(child.d->pattern());
		return child;
	}

	Unboundtor::Pattern *Unboundtor::Pattern::union_(Unboundtor::Pattern *ret)
	{
		if(!ret)
			ret = new Unboundtor::Pattern(unboundtor(), Unboundtor::Explicit);
		unions()->push_back(ret);
		ret->unions_ = unions();
		setExplicit();
		return ret;
	}
	RDFPattern RDFPattern::union_() const
	{
		return RDFPattern(new RDFPattern::Data(d->pattern()->union_()));
	}
	RDFPattern RDFPattern::union_(RDFPattern const &union_) const
	{
		return RDFPattern(new RDFPattern::Data(d->pattern()->union_(union_.d->pattern())));
	}

	Unboundtor::Pattern *Unboundtor::Pattern::optional(Unboundtor::Pattern *ret)
	{
		return child(ret, Unboundtor::Optional);
	}
	RDFPattern RDFPattern::optional() const
	{
		return RDFPattern(new RDFPattern::Data(d->pattern()->optional()));
	}
	RDFPattern RDFPattern::optional(RDFPattern const &optional) const
	{
		return RDFPattern(new RDFPattern::Data(d->pattern()->optional(optional.d->pattern())));
	}

	Unboundtor::Pattern *Unboundtor::Pattern::graph(Variable *graph_variable, Pattern *outer_ret)
	{
		Unboundtor::Pattern *ret = child(outer_ret, Unboundtor::Explicit);
		ret->graphVariable(graph_variable);
		return ret;
	}

	RDFVariable RDFPattern::graph() const
	{
		return RDFVariable::Data::createVariable(new RDFVariable::Data
				(d->pattern()->graphVariable(), d->pattern()));
	}

	Unboundtor::Variable *Unboundtor::Pattern::graphVariable(Variable *graph_variable)
	{
		Variable *ret = existingGraphVariable();
		if(!ret)
			HasVariableInRole<GraphVariable>::setVariable(ret = (graph_variable
					? graph_variable
					: new Variable(LiveNode(), unboundtor())));
		else if(graph_variable)
			ret->setAlias(*graph_variable);
		return ret;
	}

	RDFFilter RDFPattern::exists() const
	{
		Unboundtor::Pattern *p = d->pattern();
		RDFFilter::Data *subex = new RDFFilter::Data
				(new Unboundtor::PatternFilter(p), parent().d->pattern());
		p->detachAsFilterPattern();
		p->setExists(true);
		return RDFFilter::Data::createFilter(subex);
	}

	RDFFilter RDFPattern::notExists() const
	{
		Unboundtor::Pattern *p = d->pattern();
		RDFFilter::Data *subex = new RDFFilter::Data
				(new Unboundtor::PatternFilter(p), parent().d->pattern());
		p->detachAsFilterPattern();
		p->setNotExists(true);
		return RDFFilter::Data::createFilter(subex);
	}

	RDFSubSelect Unboundtor::Pattern::subQuery()
	{
		return RDFSubSelect::Data::create(new RDFSubSelect::Data(child(0, SubSelect)));
	}

	RDFSubSelect RDFPattern::subQuery() const
	{
		return d->pattern()->subQuery();
	}

	RDFPattern::RDFPattern()
		: d(new RDFPattern::Data(0, Unboundtor::Explicit))
	{}

	RDFPattern::RDFPattern(RDFPattern const &cp)
		: d(cp.d)
	{}

	RDFPattern::RDFPattern(RDFPattern::Data *data)
		: d(data)
	{}

	RDFPattern::~RDFPattern()
	{}

	RDFPattern RDFPattern::deepCopy(RDFVariable::DeepCopier const &copier) const
	{
		return copier.copy(*this);
	}

	Unboundtor::Pattern *Unboundtor::Pattern::parent(int index)
	{
		Unboundtor::Pattern *ret = this;
		while(index-- && ret->parent_pattern)
			ret = ret->parent_pattern;
		return ret;
	}

	RDFPattern RDFPattern::parent(int index) const
	{
		return RDFPattern(new RDFPattern::Data(d->pattern()->parent(index)));
	}

	RDFVariable RDFPattern::variable(QString explicit_name) const
	{
		return RDFVariable::Data::createVariable(new RDFVariable::Data(LiveNode(explicit_name), this->d->pattern()));
	}

	RDFVariable RDFPattern::variable(RDFVariable const &variable) const
	{
		d->pattern()->connectTo(*variable.d->assertPattern());
		return RDFVariable::Data::createVariable
				( new RDFVariable::Data(variable.d->variable(), d->pattern()));
	}

	void RDFPattern::useFilter(RDFFilter const &filter) const
	{
		d->pattern()->connectTo(*filter.d->assertPattern());
		filter.useFilterIn(this);
	}

	RDFPattern &RDFPattern::operator=(RDFPattern const &rhs)
	{
		d = rhs.d;
		return *this;
	}

	RDFProperty::Data::Data
			( RDFVariable const &source, RDFVariable const &predicate
			, RDFVariable const &target, RDFStrategyFlags strategy
			, QSharedPointer<QObject> const &parent)
		: source_(source), predicate_(predicate), target_(target), parent_(parent)
	{
		target_.metaEnableStrategyFlags(strategy & RDFStrategy::PropertyToColumnFlagsMask);
#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "RDFProperty::Data created: PropD(" << this << ")";
#endif
	}
	RDFProperty::Data::Data(RDFProperty::Data const &cp)
		: QSharedData(), source_(cp.source_), predicate_(cp.predicate_), target_(cp.target_)
		, parent_(cp.parent_)
	{
#ifdef QRDFUNBOUND_DEBUG
		xdebug(4) log << "RDFProperty::Data copied: PropD(" << this << ") from PropD(" << &cp << ")";
#endif
	}
	RDFProperty::Data::~Data()
	{
#ifdef QRDFUNBOUND_DEBUG
		xdebug(4) log << "RDFProperty::Data destroyed: PropD(" << this << ")";
#endif
	}

	RDFPropertyData::RDFPropertyData
            ( RDFVariable const &source, RDFVariable const &predicate, RDFVariable const &target
            , RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent, bool is_triple_bound)
		: d(new RDFProperty::Data( source, predicate, target
		                         , strategy | (is_triple_bound
		                        		 ? RDFStrategyFlags(RDFStrategy::ChainedProperty) : 0)
		                         , parent))
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFProperty(" << this << ")->PropD(" << d.data() << ") created";
#endif
	}
	RDFPropertyData::RDFPropertyData( RDFVariable const &predicate, RDFVariable const &target
	               , RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent)
		: d(new RDFProperty::Data(RDFVariable(), predicate, target, strategy, parent))
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFProperty(" << this << ")->PropD(" << d.data() << ") created";
#endif
	}
	RDFPropertyData::RDFPropertyData(RDFProperty const &cp)
		: d(cp.d)
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFProperty(" << this << ")->PropD(" << cp.d.data() << ") "
				"copied from RDFProperty(" << &cp << ")";
#endif
	}
	RDFPropertyData::~RDFPropertyData()
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFProperty(" << this << ")->PropD("
				 << const_cast<RDFProperty const *>(this)->d.data() << ") destroyed";
#endif
	}

	void RDFProperty::clear()
	{
		d = new RDFProperty::Data(RDFVariable(), RDFVariable(), RDFVariable());
	}

	void RDFProperty::swap(RDFProperty &other)
	{
		qSwap(d, other.d);
	}


	RDFProperty &RDFProperty::operator=(RDFProperty const &rhs)
	{
		d = rhs.d;
		return *this;
	}

	RDFProperty RDFProperty::fromObjectOf(RDFStatement const &statement
			, RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent)
	{
		return RDFProperty(statement.subject(), statement.predicate(), statement.object()
				, (strategy | RDFStrategy::PredicateObject) & ~RDFStrategy::Subject, parent);
	}

	RDFProperty RDFProperty::fromSubjectOf(RDFStatement const &statement
			, RDFStrategyFlags strategy, QSharedPointer<QObject> const &parent)
	{
		return RDFProperty(statement.subject(), statement.predicate(), statement.object()
				, (strategy | RDFStrategy::SubjectPredicate) & ~RDFStrategy::Object, parent);
	}


	void RDFProperty::setSource(RDFVariable const &source) { d->source_.metaAssign(source); }
	RDFVariable RDFProperty::source() const { return d->source_; }
	void RDFProperty::setPredicate(RDFVariable const &predicate) { d->predicate_.metaAssign(predicate); }
	RDFVariable RDFProperty::predicate() const { return d->predicate_; }
	void RDFProperty::setTarget(RDFVariable const &target)
	{
		RDFStrategyFlags new_property_flags
				= (d->target_.metaStrategy() & RDFStrategy::PropertyToColumnFlagsMask)
				| (target.metaStrategy() & ~RDFStrategy::PropertyToColumnFlagsMask);
		d->target_.metaAssign(target);
		d->target_.metaSetStrategy(new_property_flags);
	}
	RDFVariable RDFProperty::target() const { return d->target_; }

#if (QTTRACKER_SUPPORT_VER <= 603)
	void RDFProperty::setProperty(RDFVariable const &property) { d->target_.metaAssign(property); }
	RDFVariable RDFProperty::property() const { return d->target_; }
	void RDFProperty::setObject(RDFVariable const &object) { d->target_.metaAssign(object); }
	RDFVariable RDFProperty::object() const { return d->target_; }
	void RDFProperty::setSubject(RDFVariable const &subject) { d->target_.metaAssign(subject); }
	RDFVariable RDFProperty::subject() const { return d->target_; }
	void RDFProperty::setVariable(RDFVariable const &variable) { d->target_.metaAssign(variable); }
	RDFVariable RDFProperty::variable() const { return d->target_; }
#endif

	void RDFProperty::setParent(QSharedPointer<QObject> const &parent) { d->parent_ = parent; }
	QSharedPointer<QObject> RDFProperty::parent() const { return d->parent_; }

	void RDFProperty::setStrategy(RDFStrategyFlags const &strategy)
	{
		d->target_.metaSetStrategy(strategy);
	}
	void RDFProperty::enableStrategyFlags(RDFStrategyFlags const &strategy)
	{
		d->target_.metaEnableStrategyFlags(strategy);
	}
	void RDFProperty::disableStrategyFlags(RDFStrategyFlags const &strategy)
	{
		d->target_.metaDisableStrategyFlags(strategy);
	}
	RDFStrategyFlags RDFProperty::strategy() const
	{
		return d->target_.metaStrategy();
	}

	RDFVariable RDFProperty::traverse(RDFVariable const &source) const
	{
		return source.property(*this);
	}

	RDFVariable RDFProperty::bind(RDFVariable const &source)
	{
		source.merge(d->source_);
		if(strategy() & Data::PropertyTripleBound)
			return source.variable(d->target_);
		enableStrategyFlags(Data::PropertyTripleBound);
		return source.bindProperty(*this);
	}

	RDFStatement RDFProperty::asStatement() const
	{
		return (strategy() & RDFStrategy::Object)
			? RDFStatement(source(), predicate(), target())
			: RDFStatement(target(), predicate(), source());
	}

	RDFStatement RDFProperty::asStatementWith(RDFVariable const &source) const
	{
		source.merge(d->source_);
		return (strategy() & RDFStrategy::Object)
			? RDFStatement(source, predicate(), target())
			: RDFStatement(target(), predicate(), source);
	}

	RDFVariable RDFProperty::traverse(RDFPropertyList const &property_chain, RDFVariable const &source)
	{
		RDFVariable ret = source;
		for(RDFPropertyList::const_iterator cpi = property_chain.begin(), cpiend = property_chain.end(); cpi != cpiend; ++cpi)
			ret.metaAssign(cpi->traverse(ret));
		return ret;
	}

	RDFVariable RDFProperty::bind(RDFPropertyList &property_chain, RDFVariable const &source)
	{
		RDFVariable ret = source;
		for(RDFPropertyList::iterator cpi = property_chain.begin(), cpiend = property_chain.end(); cpi != cpiend; ++cpi)
			ret.metaAssign(cpi->bind(ret));
		return ret;
	}

	RDFProperty RDFProperty::deepCopy(RDFVariable::DeepCopier const &copier) const
	{
		return RDFProperty(d->source_.deepCopy(copier), d->predicate_.deepCopy(copier), d->target_.deepCopy(copier), strategy(), d->parent_);
	}

	RDFProperty RDFProperty::linkCopy() const
	{
		return RDFProperty(RDFVariableLink(d->source_), RDFVariableLink(d->predicate_), RDFVariableLink(d->target_), strategy(), d->parent_);
	}

	QDebug operator<<(QDebug s, RDFProperty const &value)
	{
		s.nospace() <<
				(value.strategy() & RDFStrategy::Object ? "RDFProperty" :
				value.strategy() & RDFStrategy::Subject ? "RDFReverseProperty"
						: "RDFNullProperty")
				<< "(" << value.source()
				<< ", " << value.predicate()
				<< ", " << value.target() << ")";
		return s;
	}

/*
	void RDFQuery::Data::Pattern::unionMerge(Pattern &p)
	{
		for(Pattern *pi = next_union; pi != this; pi = pi->next_union)
			if(pi == &p)
				return;
		////debug(4) << "Swap for " << this << " and " << &p;
		std::swap(next_union, p.next_union);
	}
*/

	RDFQuery::Data::Builder::Builder(RDFQuery::Data const &query)
		: query(query), is_update_(false) {}

	QSharedPointer<RDFQuery::Data::Builder> RDFQuery::Data::createSelectBuilder() const
	{
		return QSharedPointer<Builder>(new Builder(*this));
	}

	void RDFQuery::Data::Builder::addUnboundtor(Unboundtor *unboundtor)
	{
		if(!unboundtor)
			return;
#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "Builder: adding Unboundtor " << unboundtor;
#endif
		unboundtors_.insert(unboundtor);
	}


	QString RDFQuery::Data::Builder::nodeString(Soprano::Node const &var) const
	{
		return var.toN3();
	}

	RDFSelectColumnList RDFQuery::Data::Builder::buildSelectColumnRange
			(RDFSelect::Data const &q) const
	{
		return q.columns_;
	}
	bool RDFQuery::Data::Builder::processSelectColumn(RDFSelectColumn &column) const
	{
		RDFVariable cvar = constify(column).variable();
		if(cvar.metaIsDefinite()
				&& (cvar.metaIsResource()
				|| (cvar.varCppType().metaTypeId() == CppType::of<QUrl>().metaTypeId())))
		{
			RDFVariable var_equal = RDFVariable::fromType<rdfs::Resource>();
			var_equal == RDFVariableLink(cvar);
			// TODO: doing this inner_var == outer_var check everywhere is tedious
			if(column.d->outer_variable_.d == column.d->var_.d)
				column.d->outer_variable_.metaAssign(var_equal);
			column.d->var_.metaAssign(var_equal);
			return true;
		}
		return false;
	}
	RDFSelectColumnList RDFQuery::Data::Builder::buildSelectColumns(RDFSelect::Data const &q) const
	{
		RDFSelectColumnList range = buildSelectColumnRange(q);
		for(RDFSelectColumnList::iterator ri = range.begin(), riend = range.end()
				; ri != riend; ++ri)
			processSelectColumn(*ri);
		return range;
	}

	RDFQuery::Data::Variable *RDFQuery::Data::Builder::prepareVariable
			(Variable *v, Unboundtor::Variable *var, bool &is_nonprocessed_blank)
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "Builder: preparing variable UVar(" << var << ")";
#endif
		RDFVariable::Data const *link = var->constLinkTarget();
		if(link)
		{
#ifdef QRDFUNBOUND_DEBUG
			debug(4) << "Builder: \tlinked to VarD(" << link << ")->UVar(" << link->variable() << ")";
			debug(4) << "Builder: \t          VarD(" << link << ")->UPat(" << link->pattern() << ")";
			debug(4) << "Builder: \tdomain    UPat(" << var->linkDomainPattern() << ")";
#endif
			VariableIndex::const_iterator vi = variable_index.find(link->variable());
			if(vi == variable_index.end())
			{
				addUnboundtor(link->variable()->unboundtor());
				v = prepareVariable(v, link->variable(), is_nonprocessed_blank);
			} else
			{
				v = *vi;
				is_nonprocessed_blank = false;
			}
			Unboundtor::Pattern const *lp = link->pattern(), *ldp = var->linkDomainPattern();

			if(lp != ldp && lp)
			{
				pattern_progress_.insert(lp, false);
				if(ldp && !ldp->isFilterPattern())
					pattern_links_.insertMulti(lp, ldp);
			}
			else if(lp == ldp)
				debug(2) << "link build failure, variable link domain pattern is the same as variable target pattern";
		}

		Variable *&lookup_target = variable_index[var];

		if(!link)
		{
			// TODO: decide whether to clean up the dead dominance code
			//if(Unboundtor::Variable *dominant = const_cast<Unboundtor::Variable *>(var->getDominant()))
			//	var = dominant;

			if(Unboundtor::Filter const *filter = var->getFilter())
			{
				filter_variables_[v] = filter;
				*v = QString();
				is_nonprocessed_blank = false;
			} else
			if(is_nonprocessed_blank
					&& (!var->node.isBlank()
							// node represents a blank node inside update statement template
							|| (is_update_ && var->isExplicit() && var->isUnconnected()))
					&& !var->node.isEmpty())
			{
				*v = nodeString(var->node);
				is_nonprocessed_blank = false;
			}
			else
				*v += var->node.toString();
		}

		lookup_target = v;
		return v;
	}

	QString RDFQuery::Data::Builder::uniqueName(QString const &name)
	{
		QString ret;
		if(int &count = used_names[name])
		{
			while(used_names[ret = (name + "_" + QString().setNum(count))])
				++count;
		} else
			ret = name;
		++used_names[ret];
		return ret;
	}

	void RDFQuery::Data::Builder::processVariable(Unboundtor::Variable *var)
	{
		if(variable_index.find(var) != variable_index.end())
		{
#ifdef QRDFUNBOUND_DEBUG
			debug(4) << "Builder: Variable already processed: " << var;
#endif
			return;
		}

		variables.push_back(Variable());
		Variable *new_ = &variables.back();

		bool is_nonprocessed_blank = true;
		Variable *v = prepareVariable(new_, var, is_nonprocessed_blank);
		if(v != new_)
			variables.pop_back();

		if(is_nonprocessed_blank)
		{
			v->replace(QRegExp("[^0-9a-zA-Z_]"), "_");
			if(!v->size())
				*v = "?";
			else
				*v = "?_" + *v;

			*v = uniqueName(*v);
		}

#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "Builder: \tprocessed: UVar(" << var << "):" << *v;
#endif
	}

	void RDFQuery::Data::Builder::processUnboundtor(Unboundtor const &unboundtor)
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "Builder: Processing Unboundtor " << &unboundtor;
#endif
		for(QSet<HasParentUnboundtor<HasVariables> *>::const_iterator
				cvi = unboundtor.variables().constBegin()
				, cviend = unboundtor.variables().constEnd(); cvi != cviend; ++cvi )
			processVariable(static_cast<Unboundtor::Variable *>(*cvi));
	}
	
	void RDFQuery::Data::Builder::processUnboundtors(Unboundtors::iterator current)
	{
		for( ;current != unboundtors_.end(); ++current)
			processUnboundtor(**current);
	}
	void RDFQuery::Data::Builder::processUnboundtors()
	{
		processUnboundtors(unboundtors_.begin());
	}

	void RDFQuery::Data::Builder::registerVariable(RDFVariable::Data const &var)
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "Builder: Registering variable: VarD(" << &var << ")->UVar(" << var.variable() << ")";
#endif
		if(var.pattern())
		{
			addUnboundtor(var.variable()->unboundtor());
			pattern_progress_.insert(var.pattern(), false);
		}
		else
			processVariable(var.variable());
#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "Builder: \tMember of pattern " << var.pattern() << " and unboundtor " << &*var.variable()->unboundtor();
#endif
	}

	void RDFQuery::Data::Builder::prepareQuery(QStringList const &reserved_words)
	{
		used_names["?"] = 1;
		used_names["?_"] = 1;
		foreach(QString reserved_word, reserved_words)
			used_names["?" + reserved_word] = 1;

		// the space before " a" is hack to cause type triples to appear before other
		// triples in the compressed triple forms
		predicate_conversions["<http://www.w3.org/1999/02/22-rdf-syntax-ns#type>"] = " a";
		predicate_conversions["rdf:type"] = " a";
		predicate_conversions["a"] = " a";
	}

	QString *RDFQuery::Data::Builder::variableName(RDFVariable const &var, bool use)
	{
		return variableName(var.d, use);
	}

	QString invalid_variable("uninitialized variable");

	QString *RDFQuery::Data::Builder::variableName(RDFVariable::Data const *var, bool use)
	{
		QString *ret = variableName(var->variable(), use);
#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "Builder: Variable name for VarD(" << var << ")"
				"->UVar(" << var->variable() << ") requested" << (use ? ", using: " : ": ")
				<< *ret;
#endif
		return ret;
	}
	QString *RDFQuery::Data::Builder::variableName(Unboundtor::Variable const *var, bool use)
	{
		QString *ret = variable_index[var];
		if(!ret)
			ret = &invalid_variable;
		else if(!ret->size())
		{
			if(Unboundtor::Filter const *filter = filter_variables_[ret])
				*ret = recurseFilter(*filter);
		}
#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "Builder: Variable name for UVar(" << var << ") requested"
				 << (use && var->node.isBlank() ? ", using: " : ": ") << *ret;
#endif
		if(use && var->node.isBlank())
		{
			UseData &use_data = variable_uses_[*ret];
			use_data.first++;
			use_data.second.insert(var);
		}
		return ret;
	}

	QString &RDFQuery::Data::Builder::outerVariableName(RDFVariable const &var, bool use)
	{
		return outer_variable_names_remapping_[variableName(var, use)];
	}


	QString RDFQuery::Data::Builder::indent(int indentation)
	{
		QString ret("\n");
		while(indentation--)
			ret += "  ";
		return ret;
	}

	QString RDFQuery::Data::Builder::recurseFilter(Unboundtor::Filter const &filter
			, int indentation)
	{
		QString ret;
		typedef Unboundtor::Filter Filter;
		if(filter.filtertype_ == filter.identity)
			return nodeString(filter.node);
		if(filter.filtertype_ >= filter.beginUnaryOps && filter.filtertype_ < filter.endUnaryOps)
		{
			Unboundtor::UnaryFilter const &ufilter
					= static_cast<Unboundtor::UnaryFilter const &>(filter);
			QString text = *variableName(ufilter.variable());
			if(filter.filtertype_ >= filter.suffixStringUnaryOpsBegin
					&& filter.filtertype_ < filter.suffixStringUnaryOpsEnd)
				text += static_cast<Unboundtor::UnarySuffixFilter const &>(filter).suffix;
			switch(filter.filtertype_)
			{
			case Filter::not_: return "!" + text;
			case Filter::inv: return "-" + text;
			case Filter::hasPrefix:
			case Filter::hasSuffix:
			case Filter::contains:
			case Filter::matchesRegexp:
				return "REGEX(" + text + ")";
			default:
				break;
			}
		}
		else if(filter.filtertype_ >= filter.beginBinaryOps
				&& filter.filtertype_ < filter.endBinaryOps)
		{
			Unboundtor::BinaryFilter const &bfilter
					= static_cast<Unboundtor::BinaryFilter const &>(filter);
			QString lhs = *variableName(bfilter.lhs());
			QString rhs = *variableName(bfilter.rhs());
			switch(filter.filtertype_)
			{
			case Filter::and_: return "(" + lhs + " && " + rhs + ")";
			case Filter::or_: return "(" + lhs + " || " + rhs + ")";
			case Filter::equal: return "(" + lhs + " = " + rhs + ")";
			case Filter::notEqual: return "(" + lhs + " != " + rhs + ")";
			case Filter::less: return "(" + lhs + " < " + rhs + ")";
			case Filter::greater: return "(" + lhs + " > " + rhs + ")";
			case Filter::lessOrEqual: return "(" + lhs + " <= " + rhs + ")";
			case Filter::greaterOrEqual: return "(" + lhs + " >= " + rhs + ")";
			case Filter::add: return "(" + lhs + " + " + rhs + ")";
			case Filter::sub: return "(" + lhs + " - " + rhs + ")";
			case Filter::mul: return "(" + lhs + " * " + rhs + ")";
			case Filter::div: return "(" + lhs + " / " + rhs + ")";
			default:
				break;
			}
		}
		else if(filter.filtertype_ >= filter.beginMultiOps
				&& filter.filtertype_ < filter.endMultiOps)
		{
			Unboundtor::FunctionFilter const &ffilter
					= static_cast<Unboundtor::FunctionFilter const &>(filter);
			QStringList multis;
			for(QLinkedList<Unboundtor::HasVariable>::const_iterator vi = ffilter.arguments.begin()
					, viend = ffilter.arguments.end(); vi != viend; ++vi)
				multis.push_back(*variableName(vi->variable()));
			switch(filter.filtertype_)
			{
			case Filter::function:
				return (ffilter.name()->node.isBlank()
						? ffilter.name()->node.toString()
						: nodeString(ffilter.name()->node))
								+ "(" + multis.join(", ") + ")";
			case Filter::isMemberOf:
				{
					QString member = *variableName
							(static_cast<Unboundtor::MemberFilter const &>(filter).member());
					if(multis.size())
						return "((" + member + "=" + multis.join(") || (" + member + "=") + "))";
					else
						return "false";
				}
			default:
				break;
			}
		} else if(filter.filtertype_ == filter.subSelectExpression)
		{
			Unboundtor::Pattern const *pat
					= static_cast<Unboundtor::SubSelectFilter const &>(filter).subSelectPattern();
			if(pat->subselects_.size() == 1)
			{
				RDFSubSelect::Data const *subsel = pat->subselects_.begin()->data();
				PatternProgress::iterator ssi = pattern_progress_.insert(pat, false);
				if(ssi.value())
				{
					warning() << "filter build failure: subselection pattern already generated";
					return "(already generated subselection)";
				}
				PatternProgress::iterator sssiblings = --pattern_progress_.end();
				WhereContext stacked = prepareSubSelect(*subsel);

				QString str = "( " + generateSelectClause(*subsel, indentation + 1);
				str += indent(indentation + 1) + "WHERE {";

				str += generatePatternClause(ssi, indentation + 1, true);
				while(++sssiblings != pattern_progress_.end())
					str += generatePatternClause(sssiblings, indentation + 1, true);

				str += "}" + generateSolutionModifiers(*subsel, indentation + 2);
				str += ")" + indent(indentation);
				unwindSubSelect(stacked);
				return str;
			} else if(pat->subselects_.size() > 1)
				debug(2) << "Internal error: multiple subselects in pattern";
			ret = "(nonexistent subselection)";
		} else if(filter.filtertype_ == filter.patternExpression)
		{
			Unboundtor::Pattern const *pat
					= static_cast<Unboundtor::PatternFilter const &>(filter).filterPattern();
			PatternProgress::iterator psi = pattern_progress_.insert(pat, false);
			if(psi.value())
			{
				warning() << "filter build failure: filter pattern already generated";
				return "(already generated pattern)";
			}
			PatternProgress::iterator spsiblings = --pattern_progress_.end();

			QString str = "( ";

			QString pattern = generatePatternClause(psi, indentation + 1, true);
			if(!pattern.size())
				str += "true";
			else
				str += pattern;
			while(++spsiblings != pattern_progress_.end())
				str += generatePatternClause(spsiblings, indentation + 1, true);

			str += ")" + indent(indentation);
			return str;
		}

		warning() << "filter build failure; unrecognized filter type" << filter.filtertype_;
		return ret;
	}

	void RDFQuery::Data::Builder::recurseLinkedPatterns(PatternProgress::iterator pattern
			, QList<Unboundtor::Pattern const *> &results)
	{
		results.push_back(pattern.key());
		// mark pattern as processed
		*pattern = true;

		PatternProgress::iterator di;
		for(PatternLinks::const_iterator cpli = pattern_links_.lowerBound(pattern.key())
				, cpliend = pattern_links_.upperBound(pattern.key()); cpli != cpliend; ++cpli)
			if(!*cpli)
				debug(2) << "RDFQuery::generatePatternClause: invalid link domain UPat(0)";
			else if((*cpli)->isFilterPattern())
				debug(2) << "RDFQuery::generatePatternClause: skipping filter pattern";
			else if((di = pattern_progress_.insert(*cpli, false)).value())
				debug(2) << "RDFQuery::generatePatternClause: "
						"already processed domain UPat(" << *cpli << ")";
			else
			{
#ifdef QRDFUNBOUND_DEBUG
				debug(4) << "Builder: Recursed linked domain UPat(" << *cpli << ")"
						" for target UPat(" << pattern.key() << ")";
#endif
				recurseLinkedPatterns(di, results);
			}
	}

	CanonicalRawTriples RDFQuery::Data::Builder::prepareTriples
			(RDFStatementList const &statements, bool hide_if_empty)
	{
		CanonicalRawTriples triples_tree;
		for(RDFStatementList::const_iterator vi = statements.begin(), viend = statements.end()
				; vi != viend; ++vi)
			if(hide_if_empty && vi->hasEmpty())
				warning() << "Builder: triple with an empty variable hidden";
			else
			{
				RawTriple trip
						(*variableName(vi->subject())
						, *variableName(vi->predicate())
						, *variableName(vi->object()));
				QMap<QString, QString>::const_iterator predconv
						= predicate_conversions.constFind(trip.predicate_);
				if(predconv != predicate_conversions.end())
					trip.predicate_ = *predconv;
				++triples_tree[trip];
			}
		return triples_tree;
	}

	QString RDFQuery::Data::Builder::generateTriples(CanonicalRawTriples const &triples
			, int indentation)
	{
		QString ret;
		QString p_s, p_p, p_o;
		int triple_count = 0;
		for(CanonicalRawTriples::const_iterator cti = triples.begin(), ctiend = triples.end()
				; cti != ctiend; ++cti)
		{
			QString s = cti.key().subject_, p = cti.key().predicate_, o = cti.key().object_;

			if(cti.key().subject_ != p_s)
				ret +=  indent(indentation) + (triple_count++ ? ". " : "  ")
						+ (p_s = s) + " " + (p_p = p) + " " + (p_o = o);
			else if(cti.key().predicate_ != p_p)
				ret += indent(indentation + 1) + "; " + (p_p = p) + " " + (p_o = o);
			else
				ret += ", " + (p_o = o);
		}
		return ret;
	}

	QString RDFQuery::Data::Builder::generatePatternClause
			( PatternProgress::iterator current_pattern, int indentation, bool filter_pattern)
	{
		QString patternstring;
		Unboundtor::Pattern const &pattern = *current_pattern.key();

#ifndef QRDFUNBOUND_DEBUG
		if(current_pattern.value() ||
				(!filter_pattern && current_pattern.key()->isFilterPattern()))
			return QString();
#else
		debug(4) << "Builder: attempting generation of UPat(" << &pattern << ") clause";
		if(current_pattern.value())
		{
			debug(4) << "Builder: \talready generated, returning";
			return QString();
		}
		if(!filter_pattern && current_pattern.key()->isFilterPattern())
		{
			debug(4) << "Builder: \tnot generating expression pattern from "
					"non-filter context, returning";
			return QString();
		}
		debug(4) << "Builder: \tgenerating union combine UPat(" << &pattern << ")";
#endif

		bool prev_union_with_body = false;

		for(Unboundtor::PatternList::const_iterator cpi = pattern.unions()->begin()
				, cpiend = pattern.unions()->end(); cpi != cpiend; ++cpi)
		{
			// pattern with all patterns being linked to it through RDFVariableLink's
			Unboundtor::Pattern const *base_p = *cpi;
#ifdef QRDFUNBOUND_DEBUG
				debug(4) << "Builder: \tgenerating union UPat(" << base_p << ")";
#endif
			for(Unboundtor::Pattern const *link_iter = base_p
					; link_iter->isLinked() && (link_iter = pattern_links_.key(base_p)); )
				base_p = link_iter;

			if(!filter_pattern && base_p->isFilterPattern())
				continue;

#ifdef QRDFUNBOUND_DEBUG
			if(*cpi != base_p)
				debug(4) << "Builder: \titerated into link target UPat(" << base_p << ")";
#endif

			QString body, subsel_headers;
			QScopedPointer<WhereContext> stacked;

			RDFSelect::Data const *subsel = !filter_pattern && base_p->subselects_.size()
					? base_p->subselects_.begin()->data() : 0;

			if(subsel)
			{
#ifdef QRDFUNBOUND_DEBUG
				debug(4) << "Builder: \tUPat(" << base_p << ") "
						"is a sub selection SelD(" << subsel << ")";
#endif
				stacked.reset(new WhereContext(prepareSubSelect(*subsel)));
				++indentation;
				subsel_headers = indent(indentation) + generateSelectClause(*subsel, indentation);
				body += indent(indentation) + "WHERE";
				body += indent(indentation) + "{";
			}

			QList<Unboundtor::Pattern const *> lps;
			{
				PatternProgress::iterator ibase_p = pattern_progress_.insert(base_p, false);
				if(*ibase_p)
				{
#ifdef QRDFUNBOUND_DEBUG
					debug(4) << "Builder: \tUnion UPat(" << base_p << ") already generated, "
							"skipping to next";
#endif
					continue;
				}
				recurseLinkedPatterns(ibase_p, lps);
			}

			typedef QList<Unboundtor::Pattern const *>::const_iterator LPIter;
			LPIter lps_begin = lps.constBegin(), lps_end = lps.constEnd();

			{
				++indentation;

#ifdef QRDFUNBOUND_DEBUG
				debug(4) << "Builder: \t\tentering pattern triple iteration";
#endif
				for(LPIter lp = lps_begin; lp != lps_end; ++lp)
				{
#ifdef QRDFUNBOUND_DEBUG
					debug(4) << "Builder: \tlinked UPat(" << *lp << ") has"
							<< (*lp)->triples.size() << "triples";
#endif
					for(Unboundtor::Triples::const_iterator ti = (*lp)->triples.begin()
							, tiend = (*lp)->triples.end(); ti != tiend; ++ti)
					{
						body += indent(indentation)
							+ *variableName(ti->subject()) + " "
							+ *variableName(ti->predicate()) + " "
							+ *variableName(ti->object()) + " .";
					}
				}

				// TODO: enforce strict variable types
				for(LPIter lp = lps_begin; lp != lps_end; ++lp)
					if((*lp)->subpatterns_)
						for(Unboundtor::PatternList::const_iterator spi =
								(*lp)->subpatterns_->begin(), spiend = (*lp)->subpatterns_->end()
								; spi != spiend; ++spi)
						{
#ifdef QRDFUNBOUND_DEBUG
							debug(4) << "Builder: \tgenerating subpattern "
									"UPat(" << *spi << ") from linked UPat(" << *lp << ")";
#endif
							body += generatePatternClause
									(pattern_progress_.insert(*spi, false), indentation + 1);
						}

				for(LPIter lp = lps_begin; lp != lps_end; ++lp)
					for(Unboundtor::Filters::const_iterator fi = (*lp)->filters.begin()
							, fiend = (*lp)->filters.end(); fi != fiend; ++fi)
						body += indent(indentation) + "FILTER("
								+ *variableName(*fi) + ") .";

				--indentation;
			}

			for(LPIter lp = lps_begin; lp != lps_end; ++lp)
				if((*lp)->siblings_)
					for(Unboundtor::PatternList::const_iterator cpj = (*lp)->siblings_->begin()
							, cpjend = (*lp)->siblings_->end(); cpj != cpjend; ++cpj)
						{
#ifdef QRDFUNBOUND_DEBUG
				debug(4) << "Builder: \tpending sibling UPat(" << *cpj << ") "
						"from linked UPat(" << *lp << ")";
#endif
							pattern_progress_.insert(*cpj, false);
						}

			if(subsel)
			{
				body += indent(indentation) + "}";
				body += generateSolutionModifiers(*subsel, indentation + 1);

				--indentation;

				QSet<QString> exposed_variables;

				for(RDFSelectColumnList::const_iterator fi = select_columns_.begin()
						, fiend = select_columns_.end(); fi != fiend; ++fi)
				{
					RDFVariable const inner_var = fi->variable();
					RDFVariable const outer_var = fi->outerVariable();
					QString *var_name = variableName(inner_var);
					QString *outer_var_name = 0;

					VariableUses::iterator vui = variable_uses_.find(*var_name);
					if(vui == variable_uses_.end())
					{
						warning() << "Builder: warning: "
							"inner variable VN(" << *var_name << ") not found in index";
					} else
					if(vui->first == 1)
					{
						warning() << "Builder: warning: "
							"inner variable VN(" << *var_name << ") unused in subquery";
#ifdef QRDFUNBOUND_DEBUG
						debug(3) << "              : VarD(" << inner_var.d.data() << ")"
							"->UVar(" << inner_var.d->variable() << ")";
#endif
					}

					if(outer_var.d->variable() != inner_var.d->variable())
					{
						outer_var_name = variableName(outer_var, false);
						VariableUses::iterator ovui = variable_uses_.find(*outer_var_name);

						if(ovui != variable_uses_.end() && ovui->first > 1)
						{
							warning() << "Builder: warning: "
								"outer variable VN(" << *var_name << ") used in subquery";
#ifdef QRDFUNBOUND_DEBUG
							debug(3) << "              : VarD(" << outer_var.d.data() << ")"
								"->UVar(" << outer_var.d->variable() << ")";
#endif
							popVariableUsesEntry(*stacked, ovui);
							variable_uses_.erase(vui);
						}
					} else
						outer_var_name = var_name;

					exposed_variables.insert(!fi->mode() && !inner_var.metaIsExpression()
							? *var_name
							: *outer_var_name);
				}

				while(variable_uses_.size())
				{
					VariableUses::iterator vui = variable_uses_.begin();
					int total_uses = 0;
					for(QSet<Unboundtor::Variable const *>::const_iterator cvai = vui->second.begin()
							, cvaiend = vui->second.end(); cvai != cvaiend; ++cvai)
						total_uses += (*cvai)->uses();
#ifdef QRDFUNBOUND_DEBUG
					if(total_uses < vui->first)
						debug(2) << "Builder: internal error: VN(" << vui.key()
								 << ") actual uses(" << vui->first
								 << ") > expected uses(" << total_uses << ")";
					else
#endif
					if(total_uses > vui->first && !exposed_variables.contains(vui.key()))
					{
#ifdef QRDFUNBOUND_DEBUG
						debug(4) << "Builder: emitting exposition VN(" << vui.key() << ")"
								" with inner uses(" << vui->first << ") "
										"< total uses(" << total_uses << ")";
#endif
						subsel_headers += " " + vui.key();
						popVariableUsesEntry(*stacked, vui);
					}
					variable_uses_.erase(vui);
				}

				subsel_headers += body;
				body = subsel_headers;

				unwindSubSelect(*stacked);
			}

			if(body.size())
			{
#ifdef QRDFUNBOUND_DEBUG
				debug(4) << "Builder: processing UPat(" << &pattern << "), union UPat("
						<< base_p << ")";
#endif

				bool omit_braces = false;

				if(prev_union_with_body)
					patternstring += indent(indentation) + "UNION";
				else if(base_p->isOptional())
					patternstring += indent(indentation) + "OPTIONAL";
				else if(base_p->isExists())
					patternstring += indent(indentation) + "EXISTS";
				else if(base_p->isNotExists())
					patternstring += indent(indentation) + "NOT EXISTS";
				else if(pattern.unions()->size() == 1 && !subsel)
					omit_braces = true;

				if(Unboundtor::Variable *graph_var = pattern.existingGraphVariable())
				{
					patternstring += indent(indentation) + "GRAPH " + *variableName(graph_var);
					omit_braces = false;
				}

				patternstring += indent(indentation) + (omit_braces ? "": "{");
				patternstring += body;
				patternstring += indent(indentation) + (omit_braces ? "": "}");

//				if(optional && prev_union_with_body)
//					patternstring += indent(--indentation) + "}\n";
				prev_union_with_body = true;
			}
		}
#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "Builder: done generating UPat(" << &pattern << ")";
		debug(5) << "\n" << patternstring;
#endif

		return patternstring;
	}

	QString RDFQuery::Data::Builder::generateWhereClause(bool is_update, int ind)
	{
		QString ret;
		for(PatternProgress::iterator i = pattern_progress_.begin()
				; i != pattern_progress_.end(); ++i)
		{
			if(Unboundtor::Pattern const *root = i.key()->findRoot())
				ret += generatePatternClause(pattern_progress_.insert(root, false), ind + 1);
		}
		if(is_update && !ret.size())
			return ret;
		return indent(ind) + "WHERE" + indent(ind) + "{" + ret + indent(ind) + "}";
	}

	RDFQuery::Data *RDFQuery::Data::deepCopy(RDFVariable::DeepCopier const &copier) const
	{
		Q_UNUSED(copier);
		return clone();
	}

	RDFQuery::Data &RDFQuery::Data::operator=(Data const &other)
	{
		prefixes = other.prefixes;
		known_graphs = other.known_graphs;
		return *this;
	}

	QString RDFQuery::Data::generatePrefixes() const
	{
		QString prefixesstring;
		for(QMap<QString, QUrl>::const_iterator ci = prefixes.begin(); ci != prefixes.end(); ++ci)
			prefixesstring += "PREFIX " + ci.key() + ": <" + ci->toString() + ">\n";
		return prefixesstring;
	}

	RDFQuery::RDFQuery(Data *d)
		: d(d)
	{}

	RDFQuery::RDFQuery(RDFQuery const &cp)
		: d(cp.d)
	{}

	RDFQuery::~RDFQuery()
	{}

	RDFQuery &RDFQuery::operator=(RDFQuery const &rhs)
	{
		d = rhs.d;
		return *this;
	}

	bool RDFQuery::isEmpty() const
	{
		return d->isEmpty();
	}

	void RDFQuery::addKnownGraph(QSharedPointer<RDFGraph> graph, QUrl graph_name)
	{
		if(graph_name.isEmpty())
			graph_name = graph->graphIri();
		d->known_graphs[graph.data()] = qMakePair(graph_name, graph);
	}

	void RDFQuery::prefix(QString prefix, QString value)
	{
		d->prefixes[prefix] = value;
	}

	bool RDFQuery::isRDFUpdate() const
	{
		return dynamic_cast<RDFUpdate::Data const *>(d.data());
	}

	bool RDFQuery::isRDFSelect() const
	{
		return dynamic_cast<RDFSelect::Data const *>(d.data());
	}

	RDFUpdate RDFQuery::toRDFUpdate() const
	{
		// TODO: ugly, clean up. Design-wise as well.
		if(RDFUpdate::Data *update_d = const_cast<RDFUpdate::Data *>(dynamic_cast<RDFUpdate::Data const *>(d.data())))
			return RDFUpdate(update_d);
		return RDFUpdate();
	}

	RDFSelect RDFQuery::toRDFSelect() const
	{
		// TODO: ugly, clean up. Design-wise as well.
		if(RDFSelect::Data *select_d = const_cast<RDFSelect::Data *>(dynamic_cast<RDFSelect::Data const *>(d.data())))
			return RDFSelect(select_d);
		return RDFSelect();
	}

	SharedNull<RDFSelect::Data> RDFSelect::Data::shared_null;

	void RDFQuery::Data::Builder::prepareSelect(RDFSelect::Data const &q
			, QStringList const &reserved_words)
	{
		RDFQuery::Data::Builder::prepareQuery(reserved_words);

		select_columns_ = buildSelectColumns(q);

		for(RDFSelectColumnList::const_iterator fi = select_columns_.begin()
				, fiend = select_columns_.end(); fi != fiend; ++fi)
		{
#ifdef QRDFUNBOUND_DEBUG
			debug(4) << "Builder: Registering UVar(" << fi->variable().d->variable() << ")";
#endif
			//registerVariable(*fi->variable().d, (fi->mode() == RDFSelectColumn::Normal ? fi->name() : QString()));
			registerVariable(fi->variable());
			registerVariable(fi->outerVariable());
		}

		if(RDFPattern::Data const *p = q.pattern_.d.data())
		{
			addUnboundtor(p->unboundtorShare().data());
			pattern_progress_.insert(p->pattern(), false);
		}

		RDFQuery::Data::Builder::processUnboundtors();

		for(RDFSelectColumnList::const_iterator fi = select_columns_.begin()
				, fiend = select_columns_.end(); fi != fiend; ++fi)
		{
#ifdef QRDFUNBOUND_DEBUG
				debug(4) << "Builder: Setting VarD(" << fi->variable().d.data()
						<< ")->UVar(" << fi->variable().d->variable() << ") outer name to "
						<< fi->name().replace(QRegExp("[^0-9a-zA-Z_]"), "_");
#endif
			outer_variable_names_remapping_[variableName(fi->variable())]
					= uniqueName("?" + fi->name().replace(QRegExp("[^0-9a-zA-Z_]"), "_"));
		}
	}

	RDFQuery::Data::WhereContext RDFQuery::Data::Builder::prepareSubSelect
			(RDFSelect::Data const &q)
	{
		WhereContext ret = pushWhereContext();

		Unboundtors::iterator current_unboundtor = --unboundtors_.end();

		select_columns_ = buildSelectColumns(q);
		select_column_cache_.push_back(select_columns_);

#ifdef QRDFUNBOUND_DEBUG
			debug(4) << "Builder: preparing sub select clause"
					", " << select_columns_.size() << "/" << q.columns_.size();
#endif

		for(RDFSelectColumnList::const_iterator fi = select_columns_.begin()
				, fiend = select_columns_.end(); fi != fiend; ++fi)
		{
#ifdef QRDFUNBOUND_DEBUG
			debug(4) << "Builder, SubSelect: Registering VarD(" << fi->variable().d.data()
					<< "->UVar(" << fi->variable().d->variable() << ")";
#endif

			RDFVariable const var = fi->variable();
			RDFVariable const outer_var = fi->outerVariable();

			registerVariable(var);
			registerVariable(outer_var);
			processUnboundtors(++current_unboundtor);
			current_unboundtor = --unboundtors_.end();

			bool inner_as_outer_form = fi->mode() || var.metaIsExpression();
			bool inner_not_outer = var.d->variable() != outer_var.d->variable();
			if(inner_as_outer_form ^ inner_not_outer)
			{
				QString &name = *variableName(var, false);
				outer_variable_names_remapping_[&name] = name;
				if(inner_as_outer_form)
				{
					name = uniqueName(name);
				} else
					// alias inner and outer non-intrusively for duration of query building
					name = *variableName(outer_var, false);
			}
		}

		return ret;
	}

	void RDFQuery::Data::Builder::unwindSubSelect(WhereContext &stacked_context)
	{
		for(OuterVariableNames::const_iterator cni = outer_variable_names_remapping_.begin()
				, cniend = outer_variable_names_remapping_.end(); cni != cniend; ++cni)
			if(cni->size())
				*cni.key() = *cni;
		popWhereContext(stacked_context);
	}

	RDFQuery::Data::Builder::ColumnVariableNames RDFQuery::Data::Builder::columnVariableNames
			(RDFSelectColumn const &col, int indentation)
	{
		Q_UNUSED(indentation);
		ColumnVariableNames ret;

		RDFVariable const inner_var = col.variable();

		ret.first = *variableName(inner_var);

		if((col.mode() == RDFSelectColumn::Normal) && !inner_var.metaIsExpression())
			// trivial column: inner variable is also the outer variable
			return ret;

		// AS column. second will contain the expression/aggregation,
		// first will contain the outer identity

		ret.second = ret.first;

		RDFVariable const outer_var = col.outerVariable();

		QString *outer_var_name = outer_var.d->variable() != inner_var.d->variable()
				? variableName(outer_var) : 0;

		ret.first = outer_var_name ? *outer_var_name : outerVariableName(outer_var, false);
		return ret;
	}

	QStringList select_mode_strings_ = QStringList()
			<< " " << "COUNT(" << "SUM(" << "AVG(" << "MIN(" << "MAX(" << "(" << "("
			<< " " << "COUNT(DISTINCT " << "SUM(DISTINCT " << "AVG(DISTINCT "
			<< "MIN(DISTINCT " << "MAX(DISTINCT " << "(" << "(";

	QString RDFQuery::Data::Builder::generateSelectClause(RDFSelect::Data const &q, int indentation)
	{
		QString select("SELECT ");

		if(q.select_modifier == RDFSelect::Data::Distinct)
			select += "DISTINCT ";
		else if(q.select_modifier == RDFSelect::Data::Reduced)
			select += "REDUCED ";

#ifdef QRDFUNBOUND_DEBUG
		debug(2) << "Builder: generating select clause"
			", " << select_columns_.size() << "/" << q.columns_.size();
#endif

		bool prev_normal_nonexpr = true;
		for(RDFSelectColumnList::const_iterator fi = select_columns_.constBegin()
				, fiend = select_columns_.constEnd(); fi != fiend; ++fi)
		{
			RDFSelectColumn::Mode mode = fi->mode();
#ifdef QRDFUNBOUND_DEBUG
			debug(2) << "Builder: generating select variable "
					"VarD(" << fi->variable().d.data() << ""
							"->UVar(" << fi->variable().d->variable() << ")";
#endif
			QPair<QString, QString> names = columnVariableNames(*fi, indentation);

			if(names.second.size())
			{
				select += indent(indentation + 2) + select_mode_strings_[mode];
				select += names.second;
				select += (mode ? ")" : " ") + indent(indentation + 1) + "AS ";

				prev_normal_nonexpr = false;
			}
			else if(!prev_normal_nonexpr)
				select += indent(indentation + 1);

			select += names.first + " ";
		}
		return select;
	}

	QString RDFQuery::Data::Builder::generateSolutionModifiers(RDFSelect::Data const &q
			, int indentation)
	{
		QString modifier;
		if(q.group_by.size())
		{
			modifier += indent(indentation) + "GROUP BY";
			for(RDFSelect::Data::GroupBy::const_iterator ci = q.group_by.begin()
					; ci != q.group_by.end(); ++ci)
			{
				QString var_name = (*ci >= 0
						? columnVariableNames(select_columns_[*ci]).first
						: *variableName(q.noncolumn_variables[-1-*ci]));
				modifier += " " + var_name;
			}
		}
		if(q.order_by.size())
		{
			modifier += indent(indentation) + "ORDER BY";
			for(RDFSelect::Data::OrderBy::const_iterator ci = q.order_by.begin()
					; ci != q.order_by.end(); ++ci)
			{
				QString var_name = (ci->first >= 0
						? columnVariableNames(select_columns_[ci->first]).first
						: *variableName(q.noncolumn_variables[-1-ci->first]));
				modifier += ci->second ? " " + var_name : " DESC(" + var_name + ")";
			}
		}
		if(q.offset != -1)
			modifier += indent(indentation) + "OFFSET " + QString::number(q.offset);
		if(q.limit != -1)
			modifier += indent(indentation) + "LIMIT " + QString::number(q.limit);
		return modifier;
	}

	SharedNull<RDFSelectColumn::Data> RDFSelectColumn::Data::shared_null;

	RDFSelectColumn::Data::Data(int mode, RDFVariable const &var, RDFVariable const &outer_variable)
		: mode_(mode), var_(var), outer_variable_(outer_variable.metaIsEmpty() ? var : outer_variable)
	{}

	RDFSelectColumn::Data *RDFSelectColumn::Data::deepCopy(RDFVariable::DeepCopier const &copier) const
	{	return new Data(mode_, var_.deepCopy(copier), outer_variable_.deepCopy(copier)); }

	RDFSelectColumn::RDFSelectColumn(Data &d)
		: d(&d)
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFSelectColumn SCol(" << this << ")"
				"->SColD(" << const_cast<RDFSelectColumn const *>(this)->d.data() << ") created";
#endif
	}

	RDFSelectColumn::RDFSelectColumn(RDFSelectColumn const &cp)
		: d(cp.d)
	{
#ifdef QRDFUNBOUND_DEBUG
		debug(5) << "RDFSelectColumn SCol(" << this << ")"
				"->SColD(" << const_cast<RDFSelectColumn const *>(this)->d.data() << ") copied from SCol(" << &cp << ")";
#endif
	}

	RDFSelectColumn::~RDFSelectColumn()
	{
#ifdef QRDFUNBOUND_DEBUG
		xdebug(5) {
			RDFSelectColumn::Data const *scold = const_cast<RDFSelectColumn const *>(this)->d.data();
			log << "RDFSelectColumn SCol(" << this << ")->SColD(" << scold << ") destroyed";
		}
#endif
	}

	RDFSelectColumn RDFSelectColumn::deepCopy(RDFVariable::DeepCopier const &copier) const
	{
		return *d->deepCopy(copier);
	}

	RDFSelectColumn RDFSelectColumn::create(RDFVariable const &variable, Mode mode
			, RDFVariable const &outer_variable)
	{
		return RDFSelectColumn(*new RDFSelectColumn::Data(mode, variable, outer_variable));
	}

	int RDFSelectColumn::mode() const
	{
		return d->mode_;
	}
	QString RDFSelectColumn::name() const
	{
		return d->outer_variable_.metaIdentifier();
	}
	RDFVariable &RDFSelectColumn::variable()
	{
		return d->var_;
	}
	RDFVariable const &RDFSelectColumn::variable() const
	{
		return d->var_;
	}

	RDFVariable RDFSelectColumn::outerVariable() const
	{
		return d->outer_variable_;
	}


	RDFSelect::Data::Data(Unboundtor::Pattern *pattern)
		: RDFQuery::Data()
		, offset(-1), limit(-1)
		, group_by(), order_by()
		, select_modifier(NoModifier)
		, pattern_(RDFPattern::Data::create(pattern ? new RDFPattern::Data(pattern) : 0))
	{
#ifdef QRDFUNBOUND_DEBUG
		if(static_phase_g != StaticDestruct)
			debug(2) << "Created RDFSelectD(" << this << ")"
				" initialized from UPat(" << pattern << ")";
#endif
	}

	RDFSelect::Data::Data(Data const &cp)
		: RDFQuery::Data(cp)
		, columns_(cp.columns_)
		, offset(cp.offset), limit(cp.limit)
		, group_by(cp.group_by), order_by(cp.order_by)
		, noncolumn_variables(cp.noncolumn_variables)
		, select_modifier(cp.select_modifier)
		, pattern_(cp.pattern_)
	{
#ifdef QRDFUNBOUND_DEBUG
		if(static_phase_g != StaticDestruct)
		{
			debug(4) << "RDFSelect::Data SelD(" << this << ") "
					"copied from" << (&cp == &shared_null ? "shared null" : "")
					<< " SelD(" << &cp << ")";
			debug(5) << "                SelD(" << this << ")"
					"->pattern UPat(" << constify(pattern_).d.data() << ")";
		}
#endif
	}

	RDFSelect::Data::Data(RDFSelect::Columns const &columns)
		: RDFQuery::Data()
		, columns_(columns)
		, offset(-1), limit(-1)
		, group_by(), order_by()
		, select_modifier(NoModifier)
	{
#ifdef QRDFUNBOUND_DEBUG
		if(static_phase_g != StaticDestruct)
		{
			debug(4) << "RDFSelect::Data SelD(" << this << ") "
					<< " initialized from column set";
			debug(5) << "                SelD(" << this << ")"
					"->pattern UPat(" << constify(pattern_).d.data() << ")";
		}
#endif
	}


	RDFSelect::Data::~Data()
	{
#ifdef QRDFUNBOUND_DEBUG
		if(static_phase_g != StaticDestruct)
			debug(4) << "Deleted RDFSelectD(" << this << ")";
#endif
	}

	RDFSelect::Data *RDFSelect::Data::deepCopy(RDFVariable::DeepCopier const &copier) const
	{
		Data *ret = static_cast<Data *>(RDFQuery::Data::deepCopy(copier));
		ret->columns_ = copier.copySequence(columns_);
		ret->noncolumn_variables = copier.copy(noncolumn_variables);
		ret->pattern_ = copier.copy(pattern_);
		return ret;
	}

	QString RDFSelect::Data::getSPARQLQuery(QStringList const &reserved_words) const
	{
		QString query;

		QSharedPointer<Builder> builder(createSelectBuilder());

		query += generatePrefixes();
		query += "\n";

		builder->prepareSelect(*this, reserved_words);

		query += builder->generateSelectClause(*this);
		query += builder->generateWhereClause(false);
		query += builder->generateSolutionModifiers(*this, 1);

		return query;
	}

	QString RDFSelect::getQuery(int type) const
	{
		if(type == SPARQL)
			return static_cast<RDFSelect::Data const &>(*d).getSPARQLQuery();
		return QString("Requested query type not supported, use SPARQL");
	}
	QString RDFSelect::getQuery(QStringList reserved_words, int type) const
	{
		if(type == SPARQL)
			return static_cast<RDFSelect::Data const &>(*d).getSPARQLQuery(reserved_words);
		return QString("Requested query type not supported, use SPARQL");
	}

	RDFSelect::RDFSelect()
		: RDFQuery(&RDFSelect::Data::shared_null)
	{}

	RDFSelect::RDFSelect(RDFSelect const &cp)
		: RDFQuery(cp)
	{}

	RDFSelect::RDFSelect(Columns const &columns)
		: RDFQuery(new RDFSelect::Data(columns))
	{}

	RDFSelect::Data *RDFSelect::d_func() { return static_cast<Data *>(d.data()); }
	RDFSelect::Data const *RDFSelect::d_func() const { return static_cast<Data const *>(d.data()); }


	RDFSelect::RDFSelect(Data *d)
		: RDFQuery(d)
	{}

	RDFSelect::~RDFSelect()
	{}

	RDFSelect RDFSelect::deepCopy(RDFVariable::DeepCopier const &copier) const
	{
		return RDFSelect(d_func()->deepCopy(copier));
	}


/*	RDFSelect RDFSelect::subQuery()
	{
		return static_cast<RDFSelect::Data *>(d.data())->pattern_.subQuery();
	}
*/

	RDFVariable RDFSelect::newColumn(QString column_name)
	{	RDFVariable ret(column_name); addColumnAs(ret, ret); return ret; }

	RDFVariable RDFSelect::newColumn(QString column_name, RDFSelectColumn::Mode mode)
	{	RDFVariable ret(column_name); addColumnAs(ret, ret, mode); return ret; }

	RDFVariable RDFSelect::newColumn(RDFVariable const &outer_identity, RDFSelectColumn::Mode mode)
	{	RDFVariable ret; addColumnAs(ret, outer_identity, mode); return ret; }

	RDFVariable RDFSelect::newColumnAs(RDFVariable const &outer_identity, RDFSelectColumn::Mode mode)
	{	RDFVariable ret; addColumnAs(ret, outer_identity, mode); return ret; }

	RDFVariable RDFSelect::newCountColumn(QString column_name)
	{	RDFVariable ret(column_name); addCountColumnAs(ret, ret); return ret; }

	RDFVariable RDFSelect::newCountDistinctColumn(QString column_name)
	{	RDFVariable ret(column_name); addCountDistinctColumnAs(ret, ret); return ret; }

	RDFVariable RDFSelect::newSumColumn(QString column_name)
	{	RDFVariable ret(column_name); addSumColumnAs(ret, ret); return ret; }

	RDFVariable RDFSelect::newAvgColumn(QString column_name)
	{	RDFVariable ret(column_name); addAvgColumnAs(ret, ret); return ret; }

	RDFVariable RDFSelect::newMinColumn(QString column_name)
	{	RDFVariable ret(column_name); addMinColumnAs(ret, ret); return ret; }

	RDFVariable RDFSelect::newMaxColumn(QString column_name)
	{	RDFVariable ret(column_name); addMaxColumnAs(ret, ret); return ret; }

	RDFSelect &RDFSelect::addColumn(RDFVariable const &column_variable, RDFSelectColumn::Mode mode)
	{	return addColumnAs(column_variable, column_variable, mode); }

	RDFSelect &RDFSelect::addColumn(RDFVariable const &column_variable, RDFVariable const &outer_identity, RDFSelectColumn::Mode mode = RDFSelectColumn::Normal)
	{	return addColumnAs(column_variable, outer_identity, mode); }


	void RDFSelect::Data::preprocessNewColumnVariables
			( RDFVariable::Data const &column_variable, RDFVariable::Data const &outer_identity
			, RDFSelectColumn::Mode mode)
	{
		if(RDFPattern::Data *patd = pattern_.d.data())
		{
			if(Unboundtor::Pattern *p = column_variable.pattern())
			{
				if(patd->pattern()->findRoot() != p->findRoot())
				{
					patd->pattern()->unboundtor()->merge
							(*column_variable.variable()->unboundtor());
					if(p->isSubSelect())
						patd->pattern()->child(p, true);
					else
						patd->pattern()->merge(*p, true);
				}
			}
			else
				const_cast<RDFVariable::Data &>(column_variable).setPattern(patd->pattern());
		} else
			column_variable.assertPattern()->setExplicit();


		Unboundtor::Variable *column_var = column_variable.variable();
		if(mode == RDFSelectColumn::Normal
				&& !column_var->isExpression()
				&& !column_var->isDefinite()
				&& (column_variable.variable() != outer_identity.variable()))
			outer_identity.setAlias(column_variable);

		if(column_variable.variable() != outer_identity.variable())
		{
			column_variable.connectTo(outer_identity);
			if(outer_identity.variable()->isBlank()
					&& !column_variable.variable()->isExplicit()
					&& !column_variable.variable()->isDefinite())
				column_variable.variable()->node = outer_identity.variable()->node;
		} else
		{
#ifdef QRDFUNBOUND_DEBUG
			debug(5) << "Marking VarD(" << &column_variable << ")"
					"->UVar(" << column_variable.variable() << ") "
					"column use:" << column_variable.variable();
#endif
			column_variable.variable()->markUse();
		}
	}

	void RDFSelect::Data::addColumn(RDFVariable const &column_variable
			, RDFVariable const &outer_identity, RDFSelectColumn::Mode mode)
	{
		preprocessNewColumnVariables
				(*column_variable.d.data(), *outer_identity.d.data(), mode);
		columns_.push_back
				(*new RDFSelectColumn::Data(mode, column_variable, outer_identity));
	}

	RDFSelect &RDFSelect::addColumnAs(RDFVariable const &column_variable
			, RDFVariable const &outer_identity, RDFSelectColumn::Mode mode)
	{
		d_func()->addColumn(column_variable, outer_identity, mode);
		return *this;
	}

	RDFSelect &RDFSelect::addColumn(QString name, RDFVariable const &column_variable)
	{
		return addColumnAs(column_variable, RDFVariable(name), RDFSelectColumn::Normal);
	}
	RDFSelect &RDFSelect::addColumn(QString name, RDFVariable const &column_variable, RDFSelectColumn::Mode mode)
	{
		return addColumnAs(column_variable, RDFVariable(name), mode);
	}

	RDFSelect &RDFSelect::addCountColumn(QString name, RDFVariable const &variable)
	{
		return addColumn(name, variable, RDFSelectColumn::Count);
	}

	RDFSelect &RDFSelect::addCountDistinctColumn(QString name, RDFVariable const &variable)
	{
		return addColumn(name, variable, RDFSelectColumn::CountDistinct);
	}

	RDFSelect &RDFSelect::addSumColumn(QString name, RDFVariable const &variable)
	{
		return addColumn(name, variable, RDFSelectColumn::Sum);
	}

	RDFSelect &RDFSelect::addAvgColumn(QString name, RDFVariable const &variable)
	{
		return addColumn(name, variable, RDFSelectColumn::Avg);
	}

	RDFSelect &RDFSelect::addMinColumn(QString name, RDFVariable const &variable)
	{
		return addColumn(name, variable, RDFSelectColumn::Min);
	}

	RDFSelect &RDFSelect::addMaxColumn(QString name, RDFVariable const &variable)
	{
		return addColumn(name, variable, RDFSelectColumn::Max);
	}

	RDFSelect::Columns &RDFSelect::columns()
	{
		return d_func()->columns_;
	}
	RDFSelect::Columns const &RDFSelect::columns() const
	{
		return d_func()->columns_;
	}

	RDFPattern RDFSelect::Data::pattern() const
	{
		if(!constify(pattern_).d.data())
		{
			pattern_ = RDFPattern();
#ifdef QRDFUNBOUND_DEBUG
			debug(4) << "RDFSelect::variable:  setting pattern for \tSelD(" << this << ")";
			debug(5) << "                                          \t"
					"PatD(" << constify(pattern_).d.data() << ")"
					"->UPat(" << constify(pattern_).d->pattern() << ")";
#endif
		}
		return pattern_;
	}

	RDFPattern RDFSelect::pattern() const
	{
		return d_func()->pattern();
	}

	RDFVariable RDFSelect::Data::variable(QString const &explicit_name) const
	{
		return pattern().variable(explicit_name);
	}
	RDFVariable RDFSelect::variable(QString const &explicit_name) const
	{
		// TODO: on abi break, this function really should be non-const
		return const_cast<RDFSelect *>(this)->d_func()->variable(explicit_name);
	}

	RDFVariable RDFSelect::Data::variable(RDFVariable const &variable) const
	{
		if(!pattern_.d.data())
		{
			pattern_ = RDFPattern();
#ifdef QRDFUNBOUND_DEBUG
			debug(4) << "RDFSelect::variable:  setting pattern for \tSelD(" << this << ")";
			debug(5) << "                                          \t"
					"PatD(" << constify(pattern_).d.data() << ")"
					"->UPat(" << constify(pattern_).d->pattern() << ")";
#endif
		}
		return pattern().variable(variable);
	}
	RDFVariable RDFSelect::variable(RDFVariable const &variable) const
	{
		// TODO: on abi break, this function really should be non-const
		return const_cast<RDFSelect *>(this)->d_func()->variable(variable);
	}


	RDFVariable RDFSelect::Data::variable(int column) const
	{
		if(column < columns_.size())
			return columns_[column].variable();

		warning() << "RDFSelect::variable: column(" << column << ") doesnt exist";
		return RDFVariable();
	}
	RDFVariable RDFSelect::variable(int column) const
	{
		// TODO: on abi break, this function really should be non-const
		return const_cast<RDFSelect *>(this)->d_func()->variable(column);
	}


	RDFVariableList RDFSelect::variables(RDFStrategyFlags flags, RDFStrategyFlags mask) const
	{
		const_cast<RDFSelect *>(this)->d.detach();
		Q_D_C(RDFSelect);
		if(!mask)
			mask = flags;
		RDFVariableList ret;
		for(Columns::const_iterator vi = d->columns_.begin(), viend = d->columns_.end(); vi != viend; ++vi)
		{
			RDFVariable var = vi->variable();
			if((var.metaStrategy() & mask) == flags)
				ret.push_back(var);
		}
		return ret;
	}

	RDFSelect &RDFSelect::distinct(bool is_distinct)
	{
		d_func()->select_modifier = is_distinct ? RDFSelect::Data::Distinct : RDFSelect::Data::NoModifier;
		return *this;
	}
	bool RDFSelect::isDistinct() const
	{
		return static_cast<RDFSelect::Data const &>(*d).select_modifier == RDFSelect::Data::Distinct;
	}

	RDFSelect &RDFSelect::reduced(bool is_reduced)
	{
		d_func()->select_modifier = is_reduced ? RDFSelect::Data::Reduced : RDFSelect::Data::NoModifier;
		return *this;
	}
	bool RDFSelect::isReduced() const
	{
		return static_cast<RDFSelect::Data const &>(*d).select_modifier == RDFSelect::Data::Reduced;
	}

	RDFSelect &RDFSelect::limit(int limit)
	{
		d_func()->limit = limit;
		return *this;
	}
	int RDFSelect::getLimit() const
	{
		return static_cast<RDFSelect::Data const &>(*d).limit;
	}

	RDFSelect &RDFSelect::offset(int offset)
	{
		d_func()->offset = offset;
		return *this;
	}
	int RDFSelect::getOffset() const
	{
		return static_cast<RDFSelect::Data const &>(*d).offset;
	}

	void RDFSelect::Data::insertGroupBy(unsigned column, unsigned group_by_priority)
	{
		group_by.insert(group_by_priority, column);
	}
	RDFSelect &RDFSelect::insertGroupBy(unsigned group_by_loc, unsigned column)
	{
		RDFSelect::Data *d = d_func();
		if(column < unsigned(d->columns_.size()))
			d->insertGroupBy(column, std::min<unsigned>(group_by_loc, d->group_by.size()));
		else
			warning() << "RDFSelect::groupBy: column(" << column << ") doesnt exist";
		return *this;
	}
	RDFSelect &RDFSelect::insertGroupBy(unsigned group_by_loc, RDFVariable const &variable)
	{
		RDFSelect::Data *d = d_func();
		int i = 0;
		group_by_loc = std::min<unsigned>(group_by_loc, d->group_by.size());
		for(Columns::const_iterator ci = d_func()->columns_.begin(), ciend = d_func()->columns_.end()
				; ci != ciend; ++ci, ++i)
			if(ci->d->var_.d->variable() == variable.d->variable())
				return d->insertGroupBy(i, group_by_loc), *this;
		d->noncolumn_variables.push_back(variable);
		d->group_by.insert(group_by_loc, -d->noncolumn_variables.size());
		return *this;
	}

	RDFSelect &RDFSelect::groupBy(unsigned column)
	{
		return insertGroupBy(d_func()->group_by.size(), column);
	}
	RDFSelect &RDFSelect::groupBy(RDFVariable const &variable)
	{
		return insertGroupBy(d_func()->group_by.size(), variable);
	}

	QVector<int> RDFSelect::getGroupBys() const
	{
		return static_cast<RDFSelect::Data const &>(*d).group_by;
	}

	void RDFSelect::setGroupBys(QVector<int> const &group_bys)
	{
		d_func()->group_by = group_bys;
	}

	void RDFSelect::Data::insertOrderBy(unsigned column, bool ascending, unsigned order_by_priority)
	{
		order_by.insert(order_by_priority, qMakePair(int(column), ascending));
	}
	RDFSelect &RDFSelect::insertOrderBy(unsigned order_by_loc, unsigned column, bool ascending)
	{
		RDFSelect::Data *d = d_func();
		if(column < unsigned(d->columns_.size()))
			d->insertOrderBy(column, ascending, std::min<unsigned>(order_by_loc, d->order_by.size()));
		else
			warning() << "RDFSelect::orderBy: column(" << column << ") doesnt exist";
		return *this;
	}
	RDFSelect &RDFSelect::insertOrderBy(unsigned order_by_loc, RDFVariable const &variable, bool ascending)
	{
		RDFSelect::Data *d = d_func();
		int i = 0;
		order_by_loc = std::min<unsigned>(order_by_loc, d->order_by.size());
		for(Columns::const_iterator ci = d->columns_.begin(), ciend = d->columns_.end()
				; ci != ciend; ++ci, ++i)
			if(ci->d->var_.d->variable() == variable.d->variable())
				return d->insertOrderBy(i, ascending, order_by_loc), *this;
		d->noncolumn_variables.push_back(variable);
		d->order_by.insert(order_by_loc, qMakePair(-d->noncolumn_variables.size(), ascending));
		return *this;
	}
	RDFSelect &RDFSelect::orderBy(unsigned column, bool ascending)
	{
		return insertOrderBy(d_func()->order_by.size(), column, ascending);
	}
	RDFSelect &RDFSelect::orderBy(RDFVariable const &variable, bool ascending)
	{
		return insertOrderBy(d_func()->order_by.size(), variable, ascending);
	}
	QVector<QPair<int, bool> > RDFSelect::getOrderBys() const
	{
		return static_cast<RDFSelect::Data const &>(*d).order_by;
	}
	void RDFSelect::setOrderBys(QVector<QPair<int, bool> > const &order_bys)
	{
		d_func()->order_by = order_bys;
	}

	bool RDFSelect::Data::isEmpty() const
	{
		return columns_.isEmpty();
	}


	RDFSubSelect::Data::Data()
		: RDFSelect::Data()
	{}

	RDFSubSelect::Data::Data(Unboundtor::Pattern *pattern)
		: RDFSelect::Data(!pattern->subselects_.size() ? pattern : pattern = new Unboundtor::Pattern())
	{
		pattern->subselects_.insert(QExplicitlySharedDataPointer<RDFSubSelect::Data>(this));
		int ref_count = ref.deref();

		pattern->setSubSelect();
#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "RDFSubSelect::Data SubSelD(" << this << ") created";
		debug(4) << "                   SubSelD(" << this << ")"
				"->PatD(" << constify(pattern_).d.data() << ")->UPat(" << pattern << ")";
		debug(4) << "                   SubSelD(" << this << ")->ref(" << ref_count << ")";
#else
		Q_UNUSED(ref_count);
#endif
	}

	RDFSubSelect::Data::Data(Data const &cp)
		: RDFSelect::Data(cp)
	{
		RDFPattern::Data const *pd = constify(pattern_).d.data();
		if(!pd || !pd->pattern())
			pd = constify(pattern_ = RDFPattern()).d.data();

		pd->pattern()->subselects_.insert(QExplicitlySharedDataPointer<RDFSubSelect::Data>(this));
		int ref_count = ref.deref();

		pd->pattern()->setSubSelect();
#ifdef QRDFUNBOUND_DEBUG
		debug(4) << "RDFSubSelect::Data SubSelD(" << this << ") "
				"copied from" << (&cp == &shared_null ? "shared null" : "")
				<< "SubSelD(" << &cp << ")";
		debug(5) << "                   SubSelD(" << this << ")"
				"->PatD(" << pd << ")->UPat(" << pd->pattern() << ")";
		debug(5) << "                   SubSelD(" << this << ")->ref(" << ref_count << ")";
#else
		Q_UNUSED(ref_count);
#endif
	}

	RDFSubSelect::Data::~Data()
	{
#ifdef QRDFUNBOUND_DEBUG
		if(static_phase_g != StaticDestruct)
		{
			debug(4) << "RDFSubSelect::Data SubSelD(" << this << ") destroyed";
			debug(5) << "                   SubSelD(" << this << ")"
					"->PatD(" << constify(pattern_).d.data() << ")->UPat("
							<< constify(pattern_).d->pattern() << ")";
		}
#endif
	}

	RDFSubSelect::Data *RDFSubSelect::Data::deepCopy(RDFVariable::DeepCopier const &copier) const
	{
		RDFSubSelect::Data *ret = static_cast<Data *>(RDFSelect::Data::deepCopy(copier));
		ret->ref.ref();
		constify(ret->pattern_).d->pattern()->subselects_
				.insert(QExplicitlySharedDataPointer<RDFSubSelect::Data>(ret));
		constify(pattern_).d->pattern()->subselects_
				.remove(QExplicitlySharedDataPointer<RDFSubSelect::Data>(ret));
		ret->ref.deref();
		return ret;
	}

	RDFSubSelect::RDFSubSelect()
		: RDFSelect(&RDFSubSelect::Data::shared_null)
	{}

	SharedNull<RDFSubSelect::Data> RDFSubSelect::Data::shared_null;

	RDFSubSelect::RDFSubSelect(RDFSubSelect const &cp)
		: RDFSelect(cp)
	{}

	RDFSubSelect::RDFSubSelect(Data *d)
		: RDFSelect(d)
	{}

	RDFSubSelect::~RDFSubSelect()
	{
		RDFSubSelect::Data const *cd = static_cast<RDFSubSelect::Data const *>(
				static_cast<RDFSelect const *>(this)->d.data());

		if(RDFPattern::Data const *p = cd->pattern_.d.data())
		{
			Unboundtor *ubtor = p->pattern()->unboundtor();
			// Try if we're last free-standing RDFSubSelect referring the ::Data
			// +1 by this, +1 by p, +1 by ref, -1  by SubSelD::Ctor -> 2
			if(!cd->ref.ref() == 2)
				// we're not. there are others. Release ref.
				cd->ref.deref();
			else
			{
				d = 0;
				// We are last. Mark all column variables as outbound references,
				// to avoid self-references.
				RDFSubSelect::Data *d = const_cast<RDFSubSelect::Data *>(cd);
				for(Columns::iterator ci = d->columns_.begin(), ciend = d->columns_.end(); ci != ciend; ++ci)
				{
					ubtor->registerOutboundRef(&ci->d->var_);
					ubtor->registerOutboundRef(&ci->d->outer_variable_);
				}
				// also, disable the pattern_ self-ref
				d->pattern_.d->setUnboundtorShare(ShUnboundtor());
#ifdef QRDFUNBOUND_DEBUG
				if(static_phase_g != StaticDestruct)
					debug(4) << "SubSel(" << this << ")->SubSelD(" << d << "): last detaching,"
						" owning PatD(" << p << ")->UPat(" << p->pattern() << ") holds last ref " << d->ref;
#endif
			}
		}
	}

	RDFSubSelect RDFSubSelect::deepCopy(RDFVariable::DeepCopier const &copier) const
	{
		return RDFSubSelect(static_cast<Data const *>(d.data())->deepCopy(copier));
	}

	RDFPattern RDFSubSelect::pattern() const
	{
		const_cast<RDFSubSelect *>(this)->d.detach();
		return d_func()->pattern_;
	}

	RDFFilter RDFSubSelect::asExpression() const
	{
		Unboundtor::Pattern *p = d_func()->pattern_.d->pattern();
		RDFFilter::Data *subex = new RDFFilter::Data
				( new Unboundtor::SubSelectFilter(p), p);
		subex->variable()->disableFilter();
		p->setExpressionSubSelect();
		return RDFFilter::Data::createFilter(subex);
	}

	void RDFSubSelect::Data::addColumn(RDFVariable const &column_variable
			, RDFVariable const &outer_identity, RDFSelectColumn::Mode mode)
	{
		RDFVariable var = pattern_.variable(column_variable);
		RDFSelect::Data::addColumn
				( var
				, column_variable.d == outer_identity.d
						? var
						: pattern_.variable(outer_identity)
				, mode);
		//		( pattern_.variable(column_variable), pattern_.variable(outer_identity), mode);
	}


	RDFVariableStatement::RDFVariableStatement(RDFVariable const &subject, RDFVariable const &predicate, RDFVariable const &object)
		: subject_(subject), predicate_(predicate), object_(object)
	{}

	RDFStatement RDFStatement::deepCopy(RDFVariable::DeepCopier const &copier) const
	{
		return RDFStatement(copier.copy(subject_), copier.copy(predicate_), copier.copy(object_));
	}

	RDFStatement RDFStatement::linkCopy() const
	{
		return RDFStatement
				(RDFVariableLink(subject_), RDFVariableLink(predicate_), RDFVariableLink(object_));
	}


	void RDFStatement::setSubject(RDFVariable const &subject)
	{
		subject_ = subject;
	}
	void RDFStatement::setPredicate(RDFVariable const &predicate)
	{
		predicate_ = predicate;
	}
	void RDFStatement::setObject(RDFVariable const &object)
	{
		object_ = object;
	}

	RDFVariable RDFStatement::subject() const
	{
		return subject_;
	}
	RDFVariable RDFStatement::predicate() const
	{
		return predicate_;
	}
	RDFVariable RDFStatement::object() const
	{
		return object_;
	}

	bool RDFStatement::isDefinite() const
	{
		return subject_.metaIsDefinite() && predicate_.metaIsDefinite() && object_.metaIsDefinite();
	}

	bool RDFStatement::hasConstrainedBlanks() const
	{
		return subject_.metaIsConstrainedBlank()
				|| predicate_.metaIsConstrainedBlank()
				|| object_.metaIsConstrainedBlank();
	}

	bool RDFStatement::hasUnconstrainedBlanks() const
	{
		return object_.metaIsUnconstrainedBlank()
				|| predicate_.metaIsUnconstrainedBlank()
				|| subject_.metaIsUnconstrainedBlank();
	}

	bool RDFStatement::hasEmpty() const
	{
		return object_.metaIsEmpty()
				|| predicate_.metaIsEmpty()
				|| subject_.metaIsEmpty();
	}

	QDebug operator<<(QDebug s, RDFStatement const &value)
	{
		s.nospace() << "RDFStatement"
				"(" << value.subject() <<
				", " << value.predicate() <<
				", " << value.object() << ")";
		return s;
	}


	SharedNull<RDFUpdate::Data> RDFUpdate::Data::shared_null;

	bool checkIsDefinite(RDFStatementList const &statements)
	{
		for(RDFStatementList::const_iterator vi = statements.begin(), viend = statements.end(); vi != viend; ++vi)
			if(!vi->isDefinite())
				return false;
		return true;
	}

	bool checkHasConstraints(RDFStatementList const &statements)
	{
		for(RDFStatementList::const_iterator vi = statements.begin(), viend = statements.end(); vi != viend; ++vi)
			if(!vi->subject().metaIsUnconstrained()
				|| !vi->predicate().metaIsUnconstrained()
				|| !vi->object().metaIsUnconstrained())
				return true;
		return false;
	}

	RDFUpdate::Operation::Operation(RDFStatementList const &deletes, RDFStatementList const &inserts, ClauseType type, Node const &graph)
		: type_(checkIsDefinite(deletes) && !checkHasConstraints(inserts) ? ClauseType(Definite | type): type)
		, deletes_(deletes), inserts_(inserts), graph_(graph)
	{}

	RDFUpdate::Operation RDFUpdate::Operation::deepCopy(RDFVariable::DeepCopier const &copier) const
	{
		return Operation(copier.copySequence(deletes_), copier.copySequence(inserts_), type_, graph_);
	}

	bool RDFUpdate::Operation::isDefinite() const
	{
		if((type_ & Definite) && !checkHasConstraints(inserts_))
			return true;
		type_ = ClauseType(type_ & ~Definite);
		return false;
	}

	bool RDFUpdate::Operation::isSilent() const
	{
		return type_ & Silent;
	}

	RDFUpdate::Operation &RDFUpdate::Operation::setSilent(bool silent)
	{
		if(!silent)
			type_ = ClauseType(type_ & ~Silent);
		else
			type_ = ClauseType(type_ | Silent);
		return *this;
	}

	RDFUpdate::Operation::ClauseType RDFUpdate::Operation::type() const
	{
		return ClauseType(type_ & ~(Definite | Silent));
	}
	RDFStatementList RDFUpdate::Operation::deletes() const
	{
		return deletes_;
	}
	RDFStatementList RDFUpdate::Operation::inserts() const
	{
		return inserts_;
	}
	Node RDFUpdate::Operation::graph() const
	{
		return graph_;
	}

	RDFUpdate::Operation RDFUpdate::Operation::insertion
			( RDFStatementList const &inserts, Node const &graph)
	{	return Operation(RDFStatementList(), inserts, Insert, graph); }

	RDFUpdate::Operation RDFUpdate::Operation::deletion
			( RDFStatementList const &deletes, Node const &graph)
	{	return Operation(deletes, RDFStatementList(), Delete, graph); }

	RDFUpdate::Operation RDFUpdate::Operation::modification
			( RDFStatementList const &deletes, RDFStatementList const &inserts, Node const &graph)
	{	return Operation(deletes, inserts, Modify, graph); }

	RDFUpdate::Data::Builder::Builder(RDFUpdate::Data const &query)
		: RDFQuery::Data::Builder(query)
	{
		is_update_ = true;
	}
	void RDFUpdate::Data::Builder::prepareStatement(RDFStatementList const &statements, bool is_delete)
	{
		for(RDFStatementList::const_iterator vi = statements.begin(), viend = statements.end()
				; vi != viend; ++vi)
			if(is_delete || !vi->hasEmpty())
			{
				if(is_delete && vi->hasUnconstrainedBlanks())
				{
					is_update_ = false;
					RDFProperty prop = RDFProperty::fromObjectOf(vi->linkCopy());
					deleteds.push_back(prop.bind(RDFVariable()));
					registerVariable(prop.source());
					registerVariable(prop.predicate());
					registerVariable(prop.target());
				} else
				{
					is_update_ = true;
					registerVariable(vi->subject());
					registerVariable(vi->predicate());
					registerVariable(vi->object());
				}
			}
	}

	QString RDFUpdate::Data::Builder::generateTemplate(RDFStatementList const &statements, bool is_delete)
	{
		return "{" + generateTriples(prepareTriples(statements, !is_delete), 1) + "\n}";
	}

	QString RDFUpdate::Data::Builder::generateOperation(Operation const &operation, QStringList const &reserved_words)
	{
		QString ret;
		unsigned type = operation.type_ & ~(Operation::Definite | Operation::Silent);

		prepareQuery(reserved_words);

		if(type == Operation::Delete || type == Operation::Modify)
			prepareStatement(operation.deletes_, true);
		if(type == Operation::Insert || type == Operation::Modify)
			prepareStatement(operation.inserts_, false);

		RDFQuery::Data::Builder::processUnboundtors();

		QString silent;
		if(operation.type_ & RDFUpdate::Operation::Silent)
			silent = "SILENT ";

		if(type == Operation::Modify)
		{
			if(!operation.graph_.isEmpty())
				((ret += "WITH ") += nodeString(operation.graph_)) += "\n";
			(((ret += "DELETE ") += silent) += generateTemplate(operation.deletes_, true)) += "\n";
			(((ret += "INSERT ") += silent) += generateTemplate(operation.inserts_, false));
		} else if(type == Operation::Delete)
		{
			((ret += "DELETE ") += silent)
					+= /*(concrete ? "DATA " : "")) += */ (operation.graph_.isEmpty()
							? ""
							: "FROM " + nodeString(operation.graph_));
			((ret += " ") += generateTemplate(operation.deletes_, true));
		} else
		{
			((ret += "INSERT ") += silent)
					+= /*(concrete ? "DATA " : "")) += */ (operation.graph_.isEmpty()
							? ""
							: "INTO " + nodeString(operation.graph_));
			((ret += " ") += generateTemplate(operation.inserts_, false));

		}

		if(!operation.isDefinite())
			ret += generateWhereClause(true);

		return ret += "\n";
	}

	QSharedPointer<RDFUpdate::Data::Builder> RDFUpdate::Data::createUpdateBuilder() const
	{
		return QSharedPointer<RDFUpdate::Data::Builder>(new Builder(*this));
	}

	QString RDFUpdate::Data::getSPARQLUpdateQuery(QStringList const &reserved_words) const
	{
		QString query;

		query += generatePrefixes();
		query += "\n";
		for(QList<Operation>::const_iterator i = operations.begin(), iend = operations.end(); i != iend; ++i)
			query += createUpdateBuilder()->generateOperation(*i, reserved_words) + "\n";
		return query;
	}

	RDFUpdate::RDFUpdate()
		: RDFQuery(&RDFUpdate::Data::shared_null)
	{}

	RDFUpdate::RDFUpdate(RDFUpdate const &cp)
		: RDFQuery(cp)
	{}

	RDFUpdate::RDFUpdate(Data *d_)
		: RDFQuery(d_)
	{}

	RDFUpdate RDFUpdate::deepCopy(RDFVariable::DeepCopier const &copier) const
	{
		return RDFUpdate(d_func()->deepCopy(copier));
	}

	RDFUpdate::Data *RDFUpdate::d_func() { return static_cast<Data *>(d.data()); }
	RDFUpdate::Data const *RDFUpdate::d_func() const { return static_cast<Data const *>(d.data()); }

	RDFUpdate::OperationList const &RDFUpdate::operations() const
	{
		return d_func()->operations;
	}

	RDFUpdate &RDFUpdate::setOperations(OperationList const &operations)
	{
		d_func()->operations = operations;
		return *this;
	}

	QString RDFUpdate::getQuery(int type) const
	{
		if(type == SPARQLUpdate)
			return d_func()->getSPARQLUpdateQuery();
		return QString("QtTracker: Requested query type not supported, use SPARQLUpdate");
	}
	QString RDFUpdate::getQuery(QStringList reserved_words, int type) const
	{
		if(type == SPARQLUpdate)
			return d_func()->getSPARQLUpdateQuery(reserved_words);
		return QString("QtTracker: Requested query type not supported, use SPARQLUpdate");
	}

	RDFUpdate &RDFUpdate::addOperation(Operation const &operation)
	{
		RDFUpdate::Data *d = d_func();
		if(!d->operations.size() || !d->mergeOperations(d->operations.back(), operation))
			d->operations.push_back(operation);
		return *this;
	}
	RDFUpdate &RDFUpdate::addInsertion(RDFStatementList const &statements, Node const &graph)
	{
		addOperation(RDFUpdate::Operation::insertion(statements, graph));
		return *this;
	}
	RDFUpdate &RDFUpdate::addDeletion(RDFStatementList const &statements, Node const &graph)
	{
		addOperation(RDFUpdate::Operation::deletion(statements, graph));
		return *this;
	}
	RDFUpdate &RDFUpdate::addModification(RDFStatementList const &deletions, RDFStatementList const &insertions, Node const &graph)
	{
		addOperation(RDFUpdate::Operation::modification(deletions, insertions, graph));
		return *this;
	}

	RDFUpdate &RDFUpdate::appendUpdate(RDFUpdate const &other)
	{
		d_func()->appendUpdate(*other.d_func());
		return *this;
	}

	RDFUpdate::Data *RDFUpdate::Data::deepCopy(RDFVariable::DeepCopier const &copier) const
	{
		Data *ret = static_cast<Data *>(RDFQuery::Data::deepCopy(copier));
		ret->operations = copier.copySequence(operations);
		return ret;
	}

	bool RDFUpdate::Data::mergeOperations(Operation &left, Operation const &right)
	{
		if(((left.type() != Operation::Delete) && (right.type() != Operation::Insert))
			|| !left.isDefinite() || !right.isDefinite()
			|| left.isSilent() || right.isSilent()
			|| left.graph_ != right.graph_)
			return false;
		left.deletes_ += right.deletes_;
		left.inserts_ += right.inserts_;
		left.type_ = Operation::ClauseType(((left.type_ + 1) | (right.type_ + 1)) - 1);
		return true;
	}

	void RDFUpdate::Data::load(RDFUpdate::Data const &other)
	{
		operations = other.operations;
	}

	void RDFUpdate::Data::appendUpdate(RDFUpdate::Data const &other)
	{
		int second_mergeable = (operations.size() && other.operations.size())
				? operations.size() : 0;

		operations += other.operations;

		if(second_mergeable &&
				mergeOperations(operations[second_mergeable - 1], operations[second_mergeable]))
			operations.removeAt(second_mergeable);
	}

	namespace Detail
	{
		void addRdfsResourceIfNode(RDFVariable const &var, RDFVariable const &) { var.isOfType<rdfs::Resource>(); }
	}

}

	template<>
SopranoLive::RDFVariable::Data *QSharedDataPointer<SopranoLive::RDFVariable::Data>::clone()
{
	return d->clone();
}

	template<>
SopranoLive::RDFPattern::Data *QSharedDataPointer<SopranoLive::RDFPattern::Data>::clone()
{
	return d->clone();
}

	template<>
SopranoLive::RDFProperty::Data *QSharedDataPointer<SopranoLive::RDFProperty::Data>::clone()
{
	return d->clone();
}

	template<>
SopranoLive::RDFSelectColumn::Data *QSharedDataPointer<SopranoLive::RDFSelectColumn::Data>::clone()
{
	return d->clone();
}

	template<>
SopranoLive::RDFQuery::Data *QSharedDataPointer<SopranoLive::RDFQuery::Data>::clone()
{
	return d->clone();
}
