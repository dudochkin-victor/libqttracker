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
 * subrowstoremodel_p.h
 *
 *  Created on: Jul 26, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_SUBROWSTOREMODEL_P_H_
#define SOPRANOLIVE_SUBROWSTOREMODEL_P_H_

#include "rowstoremodel_p.h"

namespace SopranoLive
{

	//

	// can be generalizable. Requires ownership management strategy, and a
	// mechanism for the submodel to access createIndex of the target model.
	// caveat: will not work with target models which, inside parent function
	// use row/column property information to determine the parent, as these
	// will be incorrect for the top level entries.
	class SubRowStoreModel
		: public LiveNodeModelBase
	{
		/* VZR_CLASS(SubRowStoreModel, (LiveNodeModelBase));*/

	public:
		RowStoreModelPtr row_model_; // VZR_MEMBER

		QPersistentModelIndex root_; // VZR_MEMBER
		QPersistentModelIndex topleft_incl_; // VZR_MEMBER

		QPersistentModelIndex bottomright_excl_; // VZR_MEMBER
		// contains both forward and backward conversion lookups.
		// backward conversion resides before zero in inverse order
		int * columns_mapping_; // VZR_MEMBER


		SubRowStoreModel(RowStoreModelPtr row_model
				, RDFStrategyFlags strategy
				, QModelIndex const &topleft_incl
				, QModelIndex const &bottom_right_excl
				, QModelIndex const &root = QModelIndex()
				, QList<int> const &columns_mapping = QList<int>());

		~SubRowStoreModel();

		int backendRow(int frontend_row) const { return frontend_row + topleft_incl_.row(); }
		int backendColumn(int frontend_column) const
		{
			if(!columns_mapping_)
				return frontend_column + topleft_incl_.column();
			return (frontend_column > columns_.visibleColumnCount())
					? -1
					: columns_mapping_[frontend_column];
		}
		int frontendRow(int backend_row) const { return backend_row - topleft_incl_.row(); }
		int frontendColumn(int backend_column) const
		{
			if(!columns_mapping_)
				return backend_column - topleft_incl_.column();
			return columns_mapping_[-1 - backend_column];
		}

		QModelIndex backendIndex(QModelIndex const &frontend_index) const
		{
			if(frontend_index.model() == this)
				return row_model_->createIndex(backendRow(frontend_index.row())
						, backendColumn(frontend_index.column())
						, int(frontend_index.internalId() & ~RowStoreModel::WholeIndex));
			if(frontend_index.isValid())
				return frontend_index;
			return root_;
		}

		QModelIndex frontendIndex(QModelIndex const &backend_index) const
		{
			if(backend_index == root_)
				return QModelIndex();
			if(row_model_->parent(backend_index).isValid())
				return backend_index;
			return createIndex(frontendRow(backend_index.row()), frontendColumn(backend_index.column()), backend_index.internalPointer());
		}

		QModelIndex backendParentInPlace(int &row, int &column, QModelIndex const &parent)
		{
			if(parent.isValid())
				return backendIndex(parent);
			row = backendRow(row);
			column = backendColumn(column);
			return root_;
		}

		QModelIndex index(int row, int column, const QModelIndex &parent) const;

		QModelIndex parent(const QModelIndex &child) const;

		int rowCount(const QModelIndex &parent) const;
		int columnCount(const QModelIndex &parent) const;
		bool hasChildren(const QModelIndex &parent) const;

		QVariant data(const QModelIndex &index, int role) const;
		bool setData(const QModelIndex &index, const QVariant &value, int role);

		QVariant headerData(int section, Qt::Orientation orientation, int role) const;
		bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role);

		QMap<int, QVariant> itemData(const QModelIndex &index) const;
		bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles);

		QStringList mimeTypes() const;
		QMimeData *mimeData(const QModelIndexList &indexes) const;
		bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
		Qt::DropActions supportedDropActions() const;

		bool insertRows(int row, int count, const QModelIndex &parent);
		bool insertColumns(int column, int count, const QModelIndex &parent);
		bool removeRows(int row, int count, const QModelIndex &parent);
		bool removeColumns(int column, int count, const QModelIndex &parent);

		void fetchMore(const QModelIndex &parent);
		bool canFetchMore(const QModelIndex &parent) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		void sort(int column, Qt::SortOrder order);
		QModelIndex buddy(const QModelIndex &index) const;

		bool matchContains(QModelIndex backend_index, QModelIndex const &topleft, QModelIndex const &bottomright) const;
		void matchProcessIndexList(QModelIndexList &list, QModelIndex const &topleft, QModelIndex const &bottomright) const;
		QModelIndexList match(const QModelIndex &start, int role, const QVariant &value, int hits, Qt::MatchFlags flags) const;

		QSize span(const QModelIndex &index) const;

		bool submit();
		void revert();

		RDFStrategyFlags strategy() const;
		RDFStrategyFlags strategy(QModelIndex const &index) const;
		RDFVariableList sourceVariables() const;

		bool columnHasType(int column, LiveNode const &type) const;

		QModelIndex wholeIndex() const;

		bool clearRows(int row, int count, QModelIndex const &parent);
		bool insertRows(int row, LiveNodes const &rows, QModelIndex const &parent);
		bool replaceRows(int row, LiveNodes const &rows, QModelIndex const &parent);
		bool transitionRows(int row, int count, LiveNodes const &rows, bool smooth_transition, QModelIndex const &parent);
		bool insertRowsRaw(int row, QVector<QStringList> const &raw_rows, QModelIndex const &parent);
		bool replaceRowsRaw(int row, QVector<QStringList> const &raw_rows, QModelIndex const &parent);
		bool transitionRowsRaw(int row, int count, QVector<QStringList> const &raw_rows, bool smooth_transition, QModelIndex const &parent);

		bool assignModel(LiveNodeModel const &model, bool smooth_transition);

		bool refreshRows(int row, int count, QModelIndex const &parent, RefreshMode mode);
		bool refreshModel(RefreshMode mode);

		LiveNodeModelPtr subModel(QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive, QList<int> const &columns_mapping);

		QStringList rawRow(int row, QModelIndex const &parent = QModelIndex()) const;
	};
}

#endif /* SOPRANOLIVE_SUBROWSTOREMODEL_P_H_ */
