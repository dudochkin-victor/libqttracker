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
 * triplemodel_p.cpp
 *
 *  Created on: Jun 3, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include "triplemodel_p.h"

namespace SopranoLive
{

	// spaghetti. We want TripleModule to be available for base services without actually having base services depend on the header directly.
	LiveNodeModelBasePtr LiveResourceImplBase::implGetProperties(RDFDerivedSelect *columns) const
	{
		return implGetProperties(columns, TripleModulePtr());
	}

	void TripleModule::attached(TripleModel &model)
	{
		Base::attached(model); rows_ = &this->model()->row_store;
	}

	RDFStrategyFlags TripleModule::strategy_chain
			( QModelIndex const &index, InterfaceChain<ModelCommons const> chain) const
	{
		return RDFStrategy::Cached | ModelCommons::strategy_chain(index, chain);
	}

	TripleModel::~TripleModel()
	{
		detachAllCompositorModules();
	}

	bool TripleModel::changeTripleModule(TripleModulePtr const &new_module)
	{
		return replaceModule(triple_module_handle_, new_module) != modules().end();
	}

	RDFStrategyFlags TripleModel::childModelStrategy(QModelIndex const &sub_index) const
	{
		if(sub_index.model() != this)
			return RDFStrategy::Disabled;
		if(tripleModule())
			return tripleModule()->childModelStrategy(sub_index);
		return sub_index.internalId() & ~WholeIndex;
	}

	LiveNodeModelPtr TripleModel::childModel
			( QModelIndex const &index, RDFStrategyFlags flags
			, QSharedPointer<QObject> const &parent)
	{
		if(index.model() == this)
		{
			if(isWholeIndex(index))
				return sharedFromThis<TripleModel>();
			if(tripleModule())
				return tripleModule()->childModel(index, flags, parent);
		}
		return LiveNodeModelPtr();
	}

	void TripleModel::registerInto(RDFGraphCache &cache)
	{
		if(tripleModule())
			tripleModule()->registerInto(cache);
	}

	bool TripleModel::hasTriple(RawTripleData const &/*triple*/) const
	{	return false; }

	QModelIndex TripleModel::index
			( RawTripleData const &triple, QModelIndex const &current) const
	{
		if(tripleModule())
			return tripleModule()->index(triple, current);
		return QModelIndex();
	}

	QModelIndex TripleModel::index
			( RawPropertyData const &property, QModelIndex const &current) const
	{
		if(tripleModule())
			return tripleModule()->index(property, current);
		return QModelIndex();
	}

	// direct data update handlers
	// the triple source is passed the specifically known relevant updates directly
	//
	// all combinations of subject, predicate and object list entries are considered added.
	// however, typically only one of these lists will be multiple valued.

	bool TripleModel::tripleUpdates_chain
			( RawTriplesData const &triples, TripleUpdateContextConstPtr const &context
			, InterfaceChain<TripleUpdateReceiver> chain)
	{
		if(chain.primeCompositeCall(this))
		{
			if(!TripleUpdateReceiver::tripleUpdates_chain(triples, context, chain))
				refreshModel(LiveNodeModel::Flush);
			return true;
		}
		return false;
	}

	TripleModel::TripleModel
			(QSharedPointer<RDFGraphImplBase> const &graph, RDFStrategyFlags strategy)
		: RowStoreModel(graph, strategy), triple_module_handle_(attachFrontModule())
	{}

	void TripleModel::indexAdded(PersistentTripleIndex const &/*index*/) const
	{}

	void TripleModel::indexRemoved(PersistentTripleIndex const &/*index*/) const
	{}

}
