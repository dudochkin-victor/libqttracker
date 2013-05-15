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
 * rowstoremodel_p.h
 *
 *  Created on: May 4, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_ROWSTOREMODEL_P_H_
#define SOPRANOLIVE_ROWSTOREMODEL_P_H_

#include "livenodemodelbase_p.h"
#include "rowstoreblock_p.h"

namespace SopranoLive
{
	class SubRowStoreModel;

	typedef QSparseVector<QStringList, RowStoreBlock> RowStore;
	typedef Range<RowStore> RowStoreRange;
	typedef ConstRange<RowStore const> ConstRowStoreRange;

	struct ContentModule;
	typedef QSharedPointer<ContentModule> ContentModulePtr;

	class RowStoreModel
		: public LiveNodeModelBase
	{
		Q_OBJECT

		/* VZR_CLASS(RowStoreModel, (LiveNodeModelBase));*/

	public:

		QModelIndex wholeIndex() const { return createIndex(0, 0, quint32(strategy_ | WholeIndex)); }

		bool changeContentModule(ContentModulePtr const &new_module = ContentModulePtr());

		RowStore::iterator createRowImpl(RowStore &cache, ContentModule *strategy, unsigned row) const;

		static RowStore cacheFromLiveNodes(LiveNodes const &livenodes);

		virtual QVariant dataImpl
				(const QModelIndex &index, int role, int &role_flags, RowStore::const_iterator const &irow) const;
		virtual bool setDataImpl(QModelIndex const &index, QVariant const &value, int role);

		virtual QMap<int, QVariant> itemDataImpl(const QModelIndex & index) const;
		virtual bool setItemDataImpl(QModelIndex const &index, QMap<int, QVariant> const &roles);

		virtual void insertRowsImpl(int row, int gap_count);
		virtual void insertRowImpl(int row, QStringList const &new_row);
		virtual void insertRowsImpl(int row, ConstRowStoreRange const &new_rows);
		virtual void appendRowsImpl(int gap_count);
		virtual void appendRowsImpl(QStringList const &new_row);
		virtual void appendRowsImpl(ConstRowStoreRange const &new_rows);
		virtual void clearRowsImpl(int row, int gap_count);
		virtual void replaceRowImpl(int row, QStringList const &new_row);
		virtual void replaceRowsImpl(int row, ConstRowStoreRange const &new_rows);
		virtual void removeRowsImpl(int row, int row_count);
		virtual RowStore::iterator eraseRowsImpl(RowStoreRange const &range);
		virtual bool hardTransitionImpl(int first_row, int row_count, ConstRowStoreRange const &new_rows);
		virtual bool transitionRowsImpl
				(int row, int row_count, ConstRowStoreRange const &new_rows, bool smooth = true);

		virtual bool assignModelImpl(RowStoreModel const &source, bool smooth_transition = true);
		virtual LiveNodeModelPtr subModelImpl
				( RDFStrategyFlags strategy
				, QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive
				, QList<int> const &columns_mapping);

//		bool smoothTransitionImpl(RowStoreModel const &source);
//		bool hardTransitionImpl(RowStoreModel const &source);

		void setCache(RowStore const &new_cache = RowStore()) { row_store = new_cache; }

		~RowStoreModel();

		// interface overrides

		// Methods overridden from QAbstractItemModel
		QModelIndex index(int row, int column, QModelIndex const &parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex &child) const;

		QVariant parseString(QString data, int row) const;

		int rowCount(QModelIndex const &parent = QModelIndex()) const;
		QVariant data(QModelIndex const &index, int role = Qt::DisplayRole) const;
		QMap<int, QVariant> itemData(QModelIndex const &index) const;

		bool insertRow(int row, QModelIndex const &parent = QModelIndex());
		bool insertRows(int row, int count, QModelIndex const &parent = QModelIndex());

		bool removeRow(int row, QModelIndex const &parent = QModelIndex());
		bool removeRows(int row, int count, QModelIndex const &parent = QModelIndex());

		bool setData(QModelIndex const &index, QVariant const &value, int role = Qt::EditRole);
		bool setItemData(QModelIndex const &index, QMap<int, QVariant> const &roles);

		// Methods overridden from LiveNodeModel

		bool clearRows(int row, int count, QModelIndex const &parent = QModelIndex());

		bool insertRows(int row, LiveNodes const &rows, QModelIndex const &parent = QModelIndex());
		bool replaceRows(int row, LiveNodes const &rows, QModelIndex const &parent = QModelIndex());
		bool transitionRows
				( int row, int count, LiveNodes const &rows, bool smooth_transition = true
				, QModelIndex const &parent = QModelIndex());

		bool insertRowsRaw(int row, QVector<QStringList> const &raw_rows, QModelIndex const &parent = QModelIndex());
		bool replaceRowsRaw(int row, QVector<QStringList> const &raw_rows, QModelIndex const &parent = QModelIndex());
		bool transitionRowsRaw
				( int row, int count, QVector<QStringList> const &rows, bool smooth_transition = true
				, QModelIndex const &parent = QModelIndex());

		bool assignModel(LiveNodeModel const &model, bool smooth_transition = true);

		LiveNodeModelPtr subModel
				( QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive
				, QList<int> const &columns_mapping);

		QStringList rawRow(int row, QModelIndex const &parent = QModelIndex()) const;

	public Q_SLOTS:
		void refreshModelSignal() { refreshModel(LiveNodeModel::Poll); }

	protected:
		friend class QSharedCreator<RowStoreModel>;
		RowStoreModel
				(QSharedPointer<RDFGraphImplBase> const &graph, RDFStrategyFlags strategy = RDFStrategy::Writable);

	public:
		mutable RowStore row_store;

		enum RoleStateFlags
		{
			HasDefault = 1 << 1
		,	ClearRoleOnChange = 1 << 2
		};
		typedef unsigned RoleState;
		typedef QMap<quint64, RoleState> RoleStates;

		RoleStates default_role_states_;
		QVector<QMap<int, QVariant> > default_role_values_;
		QStringList default_empty_row_;

		void setColumnInformation(RDFDerivedSelect const &columns);

		// TODO: create the meta information lookup structure
		// for user defined per-row custom properties, efficient update signaling, etc.

		ModuleHandle content_module_handle_; // VZR_MEMBER
		inline ContentModulePtr contentModule() const;

		friend class SmoothTransitionModule;
		friend class SubRowStoreModel;
	};
	typedef QSharedPointer<RowStoreModel> RowStoreModelPtr;

	struct ContentModule
		: ModuleOfDerivedModel<RowStoreModel>
		, ModuleInterface<ModelCommons, RowStoreModel, ContentModule>
	{
		/* VZR_CLASS(ContentModule
				, (ModuleOfDerivedModel<RowStoreModel>)
				, (ModuleInterface<ModelCommons, RowStoreModel, ContentModule>));*/

		typedef ModuleOfDerivedModel<RowStoreModel> Base;
		enum ChangePolicy { DenyChanges, AllowChanges, ReportChanges };

	protected:
		friend class QSharedCreator<ContentModule>;
		ContentModule(ChangePolicy change_policy = ReportChanges);
		ContentModule(ContentModule const &cp);

		static RowStore &rowCache(RowStoreModel &model);
		static RowStore const &rowCache(RowStoreModel const &model);
		RowStore &rowCache() { return *rows_; }
		RowStore const &rowCache() const { return *rows_; }

		ModulePtr attach(LiveNodeModelBase &model);
		void attached(RowStoreModel &model);
		void detached(RowStoreModel &model);

		friend class RowStoreModel;
		friend class SmoothTransitionModule;

		RowStore * rows_; // VZR_MEMBER

	public:
		virtual ~ContentModule();

		virtual RowStore::iterator createRow(int row);

		ChangePolicy change_policy; // VZR_MEMBER

		RDFStrategyFlags strategy_chain(QModelIndex const &index
				, InterfaceChain<ModelCommons const> chain) const;

		inline bool onChange(int row, int count)
		{
			switch(change_policy)
			{
			case DenyChanges: return false;
			case AllowChanges: return true;
			case ReportChanges: return aboutToChange(row, count);
			}
			return false;
		}

		virtual QSharedPointer<ContentModule> copy() const;

		virtual bool aboutToChange(int row, int count);

		// aggregated calls from the model

		virtual QVariant data
				( const QModelIndex &index, int role, int &role_flags
				, RowStore::const_iterator const &irow) const;
		virtual bool setData(QModelIndex const &index, QVariant const &value, int role);

		virtual QMap<int, QVariant> itemData(const QModelIndex & index) const;
		virtual bool setItemData(QModelIndex const &index, QMap<int, QVariant> const &roles);

		virtual bool insertRows(int row, int gap_count);
		virtual bool insertRow(int row, QStringList const &new_row);
		virtual bool insertRows(int row, ConstRowStoreRange const &new_rows);
		virtual bool clearRows(int row, int gap_count);
		virtual bool replaceRow(int row, QStringList const &new_row);
		virtual bool replaceRows(int row, ConstRowStoreRange const &new_rows);
		virtual bool removeRows(int row, int row_count);
		virtual bool transitionRows(int row, int row_count, ConstRowStoreRange const &new_rows, bool smooth = true);

		virtual bool assignModel(RowStoreModel const &source, bool smooth_transition = true);

		virtual LiveNodeModelPtr subModel
				( QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive
				, QList<int> const &columns_mapping);
	};
	typedef QSharedPointer<ContentModule> ContentModulePtr;

	inline ContentModulePtr RowStoreModel::contentModule() const
	{
		return content_module_handle_->staticCast<ContentModule>();
	}

	struct ContentModule_Simple
		: ContentModule
	{
		/* VZR_CLASS(ContentModule_Simple, (ContentModule));*/

		bool update_pending_; // VZR_MEMBER

		ContentModule_Simple(ChangePolicy change_policy);
		ContentModule_Simple(ContentModule_Simple const &cp);

		ModulePtr attach(RowStoreModel &model);

		RowStore::iterator createRow(unsigned row);

		bool updatePending();
		void setUpdatePending(bool pending = true);
		bool running();
		bool setRunning(bool running);
	};
}

#endif /* SOPRANOLIVE_ROWSTOREMODEL_P_H_ */
