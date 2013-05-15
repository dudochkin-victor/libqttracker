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
 * subrowstoremodel_p.cpp
 *
 *  Created on: Jul 26, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include "subrowstoremodel_p.h"
#include "util_p.h"

namespace SopranoLive
{
	SubRowStoreModel::SubRowStoreModel(RowStoreModelPtr row_model
			, RDFStrategyFlags strategy
			, QModelIndex const &topleft_incl
			, QModelIndex const &bottomright_excl
			, QModelIndex const &root
			, QList<int> const &columns_mapping)
		: LiveNodeModelBase(row_model->graph_, strategy)
		, row_model_(row_model)
		, root_(root), topleft_incl_(topleft_incl), bottomright_excl_(bottomright_excl)
		, columns_mapping_(0)
	{
		RDFVariableList vars = row_model_->sourceVariables();
		if(columns_mapping.size())
		{
			int column_count = columns_mapping.size();
			int backend_column_count = row_model_->columns_.size();
			columns_mapping_ = new int[backend_column_count + column_count] + backend_column_count;
			for(int i = 0; i < column_count; ++i)
			{
				int mapped = columns_mapping[i];
				columns_mapping_[i] = mapped;
				columns_mapping_[-1 - mapped] = i;
				this->columns_.addColumn(vars[mapped]);
			}
		} else
		{
			RDFDerivedColumnList const &cols = row_model_->columns_.columns();
			int icd = topleft_incl_.column();
			int icdend = bottomright_excl_.column();
			for(; icd != icdend; ++icd)
				this->columns_.addColumn(cols[icd].variable());
		}
	}

	SubRowStoreModel::~SubRowStoreModel()
	{
		if(columns_mapping_)
			delete[] (columns_mapping_ - row_model_->columns_.size());
	}

	int dummy_column = 0;
	int dummy_row = 0;

	QModelIndex SubRowStoreModel::index(int row, int column, const QModelIndex &parent) const
	{
		if(parent.isValid())
			return row_model_->index(row, column, backendIndex(parent));
		return createIndex(row, column, row_model_->index(backendRow(row), backendColumn(column), parent).internalPointer());
	}

	QModelIndex SubRowStoreModel::parent(const QModelIndex &child) const
	{
		return frontendIndex(row_model_->parent(backendIndex(child)));
	}

	int SubRowStoreModel::rowCount(const QModelIndex &parent) const
	{
		if(!parent.isValid())
			return bottomright_excl_.row() - topleft_incl_.row();
		return row_model_->rowCount(backendIndex(parent));
	}

	int SubRowStoreModel::columnCount(const QModelIndex &parent) const
	{
		if(!parent.isValid())
			return columns_.visibleColumnCount();
		return row_model_->columnCount(backendIndex(parent));
	}
	bool SubRowStoreModel::hasChildren(const QModelIndex &parent) const
	{
		if(!parent.isValid())
			return (bottomright_excl_.row() - topleft_incl_.row()) != 0
				&& columns_.visibleColumnCount() != 0;
		return row_model_->hasChildren(backendIndex(parent));
	}

	QVariant SubRowStoreModel::data(const QModelIndex &index, int role) const
	{
		return row_model_->data(backendIndex(index), role);
	}
	bool SubRowStoreModel::setData(const QModelIndex &index, const QVariant &value, int role)
	{
		return row_model_->setData(backendIndex(index), value, role);
	}

	QVariant SubRowStoreModel::headerData(int section, Qt::Orientation orientation, int role) const
	{
		return row_model_->headerData
			((orientation == Qt::Vertical
				? backendRow(section)
				: backendColumn(section))
			, orientation, role);
	}
	bool SubRowStoreModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
	{
		return row_model_->setHeaderData
			((orientation == Qt::Vertical
				? backendRow(section)
				: backendColumn(section))
			, orientation, value, role);
	}

	QMap<int, QVariant> SubRowStoreModel::itemData(const QModelIndex &index) const
	{
		return row_model_->itemData(backendIndex(index));
	}
	bool SubRowStoreModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
	{
		return row_model_->setItemData(backendIndex(index), roles);
	}

	QStringList SubRowStoreModel::mimeTypes() const
	{
		return row_model_->mimeTypes();
	}
	QMimeData *SubRowStoreModel::mimeData(const QModelIndexList &indexes) const
	{
		QModelIndexList backendlist;
		for(QModelIndexList::const_iterator ii = indexes.begin(), iend = indexes.end(); ii != iend; ++ii)
			backendlist.push_back(backendIndex(*ii));
		return row_model_->mimeData(backendlist);
	}
	bool SubRowStoreModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
	{
		QModelIndex p = backendParentInPlace(row, column, parent);
		return row_model_->dropMimeData(data, action, row, column, p);
	}

	Qt::DropActions SubRowStoreModel::supportedDropActions() const
	{
		return row_model_->supportedDropActions();
	}

	bool SubRowStoreModel::insertRows(int row, int count, const QModelIndex &parent)
	{
		QModelIndex p = backendParentInPlace(row, dummy_column, parent);
		return row_model_->insertRows(row, count, p);
	}
	bool SubRowStoreModel::insertColumns(int column, int count, const QModelIndex &parent)
	{
		QModelIndex p = backendParentInPlace(dummy_row, column, parent);
		return row_model_->insertColumns(column, count, p);
	}
	bool SubRowStoreModel::removeRows(int row, int count, const QModelIndex &parent)
	{
		if(columns_mapping_)
			return false;
		QModelIndex p = backendParentInPlace(row, dummy_column, parent);
		return row_model_->removeRows(row, count, p);
	}
	bool SubRowStoreModel::removeColumns(int column, int count, const QModelIndex &parent)
	{
		if(columns_mapping_)
			return false;
		QModelIndex p = backendParentInPlace(dummy_row, column, parent);
		return row_model_->removeColumns(column, count, p);
	}

	void SubRowStoreModel::fetchMore(const QModelIndex &parent)
	{
		return row_model_->fetchMore(backendIndex(parent));
	}
	bool SubRowStoreModel::canFetchMore(const QModelIndex &parent) const
	{
		return row_model_->canFetchMore(backendIndex(parent));
	}

	Qt::ItemFlags SubRowStoreModel::flags(const QModelIndex &index) const
	{
		return row_model_->flags(backendIndex(index));
	}

	void SubRowStoreModel::sort(int column, Qt::SortOrder order)
	{
		Q_UNUSED(column);
		Q_UNUSED(order);
		// TODO: this could bork things, what to do?
		warning() << "sub model sorting not supported";
	}

	QModelIndex SubRowStoreModel::buddy(const QModelIndex &index) const
	{
		return frontendIndex(row_model_->buddy(backendIndex(index)));
	}

	bool SubRowStoreModel::matchContains(QModelIndex backend_index, QModelIndex const &topleft, QModelIndex const &bottomright) const
	{
		while(backend_index.isValid())
		{
			QModelIndex parent = row_model_->parent(backend_index);
			if(parent == root_
					&& (unsigned(backend_index.row() - topleft.row()) < unsigned(bottomright.row()))
					&& (unsigned(backend_index.column() - topleft.column()) < unsigned(bottomright.column())))
				return true;
			backend_index = parent;
		}
		return false;
	}

	void SubRowStoreModel::matchProcessIndexList(QModelIndexList &list, QModelIndex const &topleft, QModelIndex const &bottomright) const
	{
		int upper = list.size() - 1;
		if(upper < 0 || matchContains(list[upper], topleft, bottomright))
			return;
		// binary search last backend index not contained in this model
		int lower = 0;
		do
		{
			int mid = (lower + upper) >> 1;
			if(matchContains(list[mid], topleft, bottomright))
				lower = mid + 1;
			else
				upper = mid;
		} while(lower < upper);
		list.erase(list.begin() + upper, list.end());
	}

	QModelIndexList SubRowStoreModel::match(const QModelIndex &start, int role, const QVariant &value, int hits, Qt::MatchFlags flags) const
	{
		bool wrap = flags & Qt::MatchWrap;
		flags &= ~Qt::MatchWrap;
		QModelIndex backend_start = backendIndex(start);

		QModelIndexList res = row_model_->match(backend_start, role, value, hits, flags);
		matchProcessIndexList(res, backend_start, bottomright_excl_);

		if(wrap && (0 < (hits -= res.size())))
		{
			QModelIndexList omstart = row_model_->match(topleft_incl_, role, value, hits, flags);
			matchProcessIndexList(omstart, topleft_incl_,
			                      backend_start.sibling(backend_start.row(),
			                                            bottomright_excl_.column()));
			res += omstart;
		}

		for(QModelIndexList::iterator ii = res.begin(), iend = res.end(); ii != iend; ++ii)
			*ii = frontendIndex(*ii);

		return res;
	}

	QSize SubRowStoreModel::span(const QModelIndex &index) const
	{	return row_model_->span(backendIndex(index)); }

	bool SubRowStoreModel::submit()
	{
		return row_model_->submit();
	}
	void SubRowStoreModel::revert()
	{
		return row_model_->revert();
	}

	RDFStrategyFlags SubRowStoreModel::strategy(QModelIndex const &index) const
	{
		if(index.isValid())
			return row_model_->strategy(backendIndex(index));
		return strategy_;
	}
	RDFVariableList SubRowStoreModel::sourceVariables() const
	{
		return columns_.variables();
	}

	bool SubRowStoreModel::columnHasType(int column, LiveNode const &type) const
	{
		return row_model_->columnHasType(backendColumn(column), type);
	}

// LCOV_EXCL_START
	bool SubRowStoreModel::clearRows(int row, int count, QModelIndex const &parent)
	{
		Q_UNUSED(row);
		Q_UNUSED(count);
		Q_UNUSED(parent);
		warning(this) << "not implemented";
		return false;
	}
// LCOV_EXCL_STOP
// LCOV_EXCL_START
	bool SubRowStoreModel::insertRows(int row, LiveNodes const &rows, QModelIndex const &parent)
	{
		Q_UNUSED(row);
		Q_UNUSED(rows);
		Q_UNUSED(parent);
		warning(this) << "not implemented";
		return false;
	}
// LCOV_EXCL_STOP
// LCOV_EXCL_START
	bool SubRowStoreModel::replaceRows(int row, LiveNodes const &rows, QModelIndex const &parent)
	{
		Q_UNUSED(row);
		Q_UNUSED(rows);
		Q_UNUSED(parent);
		warning(this) << "not implemented";
		return false;
	}
// LCOV_EXCL_STOP
// LCOV_EXCL_START
	bool SubRowStoreModel::transitionRows(int row, int count, LiveNodes const &rows, bool smooth_transition, QModelIndex const &parent)
	{
		Q_UNUSED(row);
		Q_UNUSED(count);
		Q_UNUSED(rows);
		Q_UNUSED(smooth_transition);
		Q_UNUSED(parent);
		warning(this) << "not implemented";
		return false;
	}
// LCOV_EXCL_STOP
// LCOV_EXCL_START
	bool SubRowStoreModel::insertRowsRaw(int row, QVector<QStringList> const &raw_rows, QModelIndex const &parent)
	{
		Q_UNUSED(row);
		Q_UNUSED(raw_rows);
		Q_UNUSED(parent);
		warning(this) << "not implemented";
		return false;
	}
// LCOV_EXCL_STOP
// LCOV_EXCL_START
	bool SubRowStoreModel::replaceRowsRaw(int row, QVector<QStringList> const &raw_rows, QModelIndex const &parent)
	{
		Q_UNUSED(row);
		Q_UNUSED(raw_rows);
		Q_UNUSED(parent);
		warning(this) << "not implemented";
		return false;
	}
// LCOV_EXCL_STOP
// LCOV_EXCL_START
	bool SubRowStoreModel::transitionRowsRaw(int row, int count, QVector<QStringList> const &raw_rows, bool smooth_transition, QModelIndex const &parent)
	{
		Q_UNUSED(row);
		Q_UNUSED(count);
		Q_UNUSED(raw_rows);
		Q_UNUSED(smooth_transition);
		Q_UNUSED(parent);
		warning(this) << "not implemented";
		return false;
	}
// LCOV_EXCL_STOP
// LCOV_EXCL_START
	bool SubRowStoreModel::assignModel(LiveNodeModel const &model, bool smooth_transition)
	{
		Q_UNUSED(model);
		Q_UNUSED(smooth_transition);
		warning() << "not implemented";
		return false;
	}
// LCOV_EXCL_STOP

	bool SubRowStoreModel::refreshModel(RefreshMode mode)
	{
		return row_model_->refreshModel(mode);
	}
	bool SubRowStoreModel::refreshRows(int row, int count, QModelIndex const &parent, RefreshMode mode)
	{
		QModelIndex p = backendParentInPlace(row, dummy_column, parent);
		return row_model_->refreshRows(row, count, p, mode);
	}

	LiveNodeModelPtr SubRowStoreModel::subModel(QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive,
												QList<int> const &columns_mapping)
	{
		QModelIndex tl(topleft_inclusive), br(bottomright_exclusive);
		if (columns_mapping.size()) {
			tl = tl.sibling(tl.row(), 0);
			br = br.sibling(br.row(), 0);
		}
		if(tl.row() > br.row())
			warning(this) << "topleft row greater than bottomright row";
		else if(tl.column() > br.column())
			warning(this) << "topleft column greater than bottomright column";
		else if(tl.column() < 0)
			warning(this) << "topleft.column(" << tl.column() << ") < 0";
		else if(br.column() > columns_.visibleColumnCount())
			warning(this) << "bottomright.column(" << br.column()
					<< ") > column count(" << columns_.visibleColumnCount() << ")";
		else
		{
			QList<int> backend_columns_mapping;
			if(columns_mapping.size())
				for(int i = 0, iend = columns_mapping.size(); i != iend; ++i)
					backend_columns_mapping.append(backendColumn(columns_mapping[i]));
			else if(columns_mapping_)
				for(int i = topleft_inclusive.column(), iend = bottomright_exclusive.column()
						; i != iend; ++i)
					backend_columns_mapping.append(columns_mapping_[i]);
			return row_model_->subModel
					( backendIndex(tl)
					, backendIndex(br), backend_columns_mapping);
		}
		return LiveNodeModelPtr();
	}

	QStringList SubRowStoreModel::rawRow(int row, QModelIndex const &parent) const
	{
		if(parent.isValid())
			return row_model_->rawRow(row, backendIndex(parent));

		QStringList ret;
		QStringList back = row_model_->rawRow(backendRow(row));
		if(!back.size())
			return ret;
		for(int i = 0, iend = columnCount(parent); i < iend; ++i)
		{
			int backend_col = backendColumn(i);
			ret.push_back(backend_col >= back.size() ? QString() : back[backend_col]);
		}
		return ret;
	}

}
