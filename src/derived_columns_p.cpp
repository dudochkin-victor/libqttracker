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
 * derived_columns_p.cpp
 *
 *  Created on: Jul 22, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include <typeinfo>
#include "derived_columns_p.h"
#include "subrowstoremodel_p.h"
#include "rdfderivedselect_p.h"
#include "util_p.h"

namespace SopranoLive
{
	DerivedColumnModule::DerivedColumnModule(RDFGraphImplBasePtr const &graph)
		: graph_(graph)
	{}

	QModelIndex DerivedColumnModule::index
			( RawPropertyData const &property, QModelIndex const &current) const
	{
		if(!current.isValid())
		{
			debug(2, model()) << "invalid current index" << current;
			return QModelIndex();
		}

		// TODO: fix so that internal access to RDFDerivedSelectPrivate is not needed
		RDFDerivedColumnPrivate::Deriveds const &deps = RDFDerivedColumnPrivate::cd_func
				((*columns_)[current.column()])->deriveds();
		RDFDerivedColumnPrivate::Deriveds::const_iterator cipi = deps.find(property);
		if(cipi != deps.end())
			//return createIndex(current.row(), *cipi, (*columns_)[*cipi].strategy());
			return createIndex(current.row(), *cipi, 0);
		return QModelIndex();
		// TODO: handle types
		// if(property.predicate() == rdf_type_as_list_g[0])
		// TODO: replace strategy with something else
	}

	RDFStrategyFlags DerivedColumnModule::refreshModel_chain
			(LiveNodeModel::RefreshMode mode, InterfaceChain<ModelCommons> chain)
	{
		RDFStrategyFlags ret = ~RDFStrategyFlags();

		LiveNodeModel::RefreshMode derived_mode = mode & LiveNodeModel::CoverDerivedProperties
				? mode
				: mode &= ~LiveNodeModel::Block;

		for(Models::const_iterator cmi = column_models.begin(), cmiend = column_models.end()
				; cmi != cmiend; ++cmi)
		{
			LiveNodeModelBasePtr model = *cmi;
			if(!model->refreshModel(derived_mode))
				ret &= ~RDFStrategy::DerivedsReady;
		}

		return ret & TripleModule::refreshModel_chain(mode, chain);
	}


	static int rowCompare( QVector<int> const &indices
			, RowStore::const_iterator const &lhs, RowStore::const_iterator const &rhs)
	{
		for(int ci = indices.size(); ci--;)
			if(int diff = QString::compare((*lhs)[indices[ci]], (*rhs)[indices[ci]]))
				return diff;
		return 0;
	}

	int DerivedColumnModule::expandBinarySearchEnd(RowStore::const_iterator const &base_i, int column) const
	{
		QVector<int> indices;
		indices.reserve(column - 1);

		while((column = (*columns_)[column].parent()) != RDFDerivedColumn::NullIndex)
			indices.push_back(column);

		int relevant_column_width = indices.first();
		RowStore::const_iterator cur_i = base_i;
		int low = cur_i.key(), step = 1;
		// expand search phase
		while((cur_i += step).isValid()
				&& cur_i->size() > relevant_column_width
				&& !rowCompare(indices, base_i, cur_i))
		{
			low += step;
			step *= 2;
		};
		// binary search phase
		int high = cur_i.key();
		int old_mid = high;
		while(low != high)
		{
			int mid = ((low + high) >> 1);
			cur_i += mid - old_mid;
			if(cur_i.isValid()
					&& cur_i->size() > relevant_column_width
					&& !rowCompare(indices, base_i, cur_i))
				low = mid + 1;
			else
				high = mid;
			old_mid = mid;
		};
		return low;
	}

	LiveNodeModelPtr DerivedColumnModule::childModel
			( QModelIndex const &index, RDFStrategyFlags flags, QSharedPointer<QObject> const &parent)
	{
		// TODO: handle these
		Q_UNUSED(flags);
		Q_UNUSED(parent);

		if(index.column() >= columns_->size() || index.row() >= model()->rowCount())
		{
			warning(model()) << "Out of bounds: Model index("
							 << index << ") outside (0 <= row <"
							 << model()->rowCount() << ", 0 <= column <" << columns_->size() << "), returning null";
			return LiveNodeModelPtr();
		}

		RDFDerivedColumn const &info = (*columns_)[index.column()];

		RowStore::const_iterator top_row_i = rows_->find(index.row());
		if(!top_row_i.isValid())
		{
			warning(model()) << "Trying to access a blank model region at"
							 << index << ", returning null";
			return LiveNodeModelPtr();
		}

//		if(info.variable().metaStrategy() & RDFStrategy::ChildedColumn)


		Models::const_iterator cmi = column_models.find(info.index());
		if(cmi != column_models.end())
		{
			LiveNodeModel *column_model = cmi->data();
			if(TripleModel *child_model = dynamic_cast<TripleModel *>(column_model))
			{
				RawPropertyData prop(info.propertyData());
				return child_model->childModel(
						child_model->index(
							prop.strategy() & RDFStrategy::Subject
								? RawTripleData::fromPredicateObject(prop.predicate(), (*top_row_i)[info.parent()])
								: RawTripleData::fromSubjectPredicate((*top_row_i)[info.parent()], prop.predicate())
						)
						, flags, parent);
			} else
			{
				//if(!info.model.data())
				//	model()->warning(1, 0, 0, index) << "DerivedColumnModule::childModel: "
				//			"column is ChildedColumn, but child model does not exist";
				//else
				warning(model()) << "column is ChildedColumn, but child model is not a TripleModel";
				return LiveNodeModelPtr();
			}

		}

		// return a sequential block sharing the same parent in the parent column

		RDFStrategyFlags sub_strat = childModelStrategy(index);

		int end_row = (sub_strat & LiveNodeModel::NonMultipleValued)
			? index.row() + 1
			: expandBinarySearchEnd(top_row_i, index.column());

		return QSharedCreator<SubRowStoreModel>::create
			( model()->sharedFromThis<RowStoreModel>()
			, sub_strat, index, index.sibling(end_row, columns_->visibleColumnCount()));
	}

	LiveNodeModelBasePtr DerivedColumnModule::modelColumnVariableImpl
			(RDFDerivedColumn &column_info, TripleModulePtr column_module)
	{
		RDFDerivedSelect sel = model()->graph()->service_context_data_->select();
		RDFDerivedColumn dep = column_info;
		int col = 0;
		int parent_index = column_info.parent();
		if(parent_index != RDFDerivedColumn::NullIndex)
		{
			RDFDerivedColumn parent = (*columns_)[parent_index];
			sel.addColumn(parent, RDFDerivedColumn::PrimaryBaseColumn);
			sel.addColumn(dep, RDFDerivedColumn::PrimaryBaseColumn, 0);
			++col;
		} else
			sel.addColumn(dep, RDFDerivedColumn::PrimaryBaseColumn);

		sel.processDerivedColumns(col);
		if(parent_index != RDFDerivedColumn::NullIndex)
			sel.insertOrderBy(0, 0);
		sel.orderBy(col);

		return graph_->baseImplModelQuery(&sel
				, (column_info.propertyData().strategy()
					& ~(RDFStrategy::ExpandDerivedProperties | RDFStrategy::Cached))
					| (model()->strategy_ & (RDFStrategy::Streaming | RDFStrategy::Windowed))
				, column_info.propertyData().parent()
				, column_module);
	}

	void DerivedColumnModule::attached(TripleModel &model)
	{
		TripleModule::attached(model);
		columns_ = &model.columns_;
		RDFDerivedColumnList &cols = columns_->refColumns();
		for(int i = columns_->visibleColumnCount(), iend = columns_->realColumnCount()
				; i-- != iend; )
			if(LiveNodeModelBasePtr column_model = modelColumnVariable(cols[i]))
			{
				column_models[i] = column_model;
				connect( column_model.data(), SIGNAL(derivedsUpdated())
				       , &model, SLOT(refreshModelSignal()));
			}
			else
				warning(&model) << "failed to model column(" << i << ")";
	}

	bool DerivedColumnModule::updateFrom_chain
			( UpdateContext const &update_context, InterfaceChain<ModelCommons> chain)
	{
		if(!update_context.model_->updateTargetModule(this, update_context))
			warning(model()) << "DerivedColumnModule not updated as part of model update operation";

		return TripleModule::updateFrom_chain(update_context, chain);
	}

	/*!
	 * Update a given \a target derived model with its derived
	 * counterpart inside target.
	 */
	bool DerivedColumnModule::DerivedUpdate::updateModel(LiveNodeModelBase *target) const
	{
		// TODO: implement
		Q_UNUSED(target);
		warning(model_.data()) << "Implement me";
		return false;
	};

	/*!
	 * Handle \a target_module updates, and if we match the target,
	 * distribute the ongoing model update to all child models of the
	 * target.
	 */
	bool DerivedColumnModule::updateTargetModule_chain
			( ModelModule *target_module, UpdateContext const &update_context
			, InterfaceChain<ModelCommons const> chain) const
	{
		if(typeid(target_module) != typeid(this))
		{
			DerivedColumnModule *derived_module = static_cast<DerivedColumnModule *>(target_module);
			// for now, assume that this and the target are matching
			for(Models::iterator tcmi = derived_module->column_models.begin()
					, tcmiend = derived_module->column_models.end(); tcmi != tcmiend; ++tcmi)
			{
				Models::const_iterator cscmi = column_models.find(tcmi.key());
				if(*tcmi && cscmi != column_models.end())
					(*tcmi)->updateFrom(DerivedUpdate(*cscmi));
			}
		}
		return TripleModule::updateTargetModule_chain(target_module, update_context, chain);
	}


	bool DerivedColumnModule::isCompatibleWith(TripleModel *triple_model) const
	{
		if(!triple_model->tripleModule())
			return true;
		if(DerivedColumnModule *depstr = dynamic_cast<DerivedColumnModule *>(triple_model->tripleModule()))
		{
			if(depstr->columns_->size() < columns_->size())
				return false;
			for(int i = 0, iend = columns_->size(); i != iend; ++i)
				// TODO: fix so that internal access to RDFDerivedSelectPrivate is not needed
				if(RDFDerivedColumnPrivate::cd_func((*columns_)[i])->deriveds()
						!= RDFDerivedColumnPrivate::cd_func((*depstr->columns_)[i])->deriveds())
					return false;
			return true;
		}
		return false;
	}


/*
	void DerivedColumnExpander::addSingleColumn(int parent, RDFProperty const &property)
	{
		dcs->columns_.push_back(ColumnInfo(dcs->columns_.size(), parent, property));

		if(RawPropertyData raw_property = RawPropertyData(property))
			dcs->columns_[parent].properties[raw_property] = &dcs->columns_.back();
		else
			model()->warning(2) << "DerivedColumnModule::addSingleColumn(" << parent
				<< ", property): given property does not evaluate to valid explicit raw property";
	}
	void DerivedColumnExpander::addMultiColumn(int parent, RDFProperty const &property, TripleModelPtr const &model)
	{
		dcs->multivalued_columns.push_back(ColumnInfo(dcs->multivalued_columns.size(), parent, property, model));

		if(RawPropertyData raw_property = RawPropertyData(property))
			dcs->columns_[parent].properties[raw_property] = &dcs->multivalued_columns.back();
		else
			model()->warning(2) << "DerivedColumnModule::addMultiColumn(" << parent
				<< ", property, model): given property does not evaluate to valid explicit raw property";
	}
*/

	typedef RowStore RowStore;
	void ItemsAsColumnModule::addSetItems(QStringList adds, RowStoreModel *model) const
	{
		// TODO: inefficient, improve from O(existing_nodes*added_nodes)
		for(RowStore::iterator i = model->row_store.begin(), iend = model->row_store.end()
				; i != iend; ++i)
			if(i.isValid() && i->size())
			{
				adds.removeAll((*i)[0]);
				if(!adds.size())
					return;
			}

		QVector<QStringList> new_rows;

		for(QStringList::const_iterator si = adds.begin(), siend = adds.end(); si != siend; ++si)
			new_rows.push_back(QStringList() << *si);
		model->appendRowsImpl(RowStore(new_rows));
	}
	void ItemsAsColumnModule::removeSetItems(QStringList removes, RowStoreModel *model) const
	{
		for(RowStore::iterator i = model->row_store.begin(), iend = model->row_store.end()
				; removes.size() && i != iend; ++i)
		{
			RowStore::iterator cur = i;
			while(cur.isValid() && cur->size() && removes.removeAll((*cur)[0])
					&& ++cur != iend) {}
			if(cur != i)
			{
				i = model->eraseRowsImpl(RowStoreRange(i, cur));
				iend = model->row_store.end();
			}
		}
	}
}
