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
 * livenodemodelbridge_p.h
 *
 *  Created on: Jun 30, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_LIVENODEMODELBRIDGE_P_H_
#define SOPRANOLIVE_LIVENODEMODELBRIDGE_P_H_


#include "include/sopranolive/liveresource.h"
#include "livenodemodelbase_p.h"

namespace SopranoLive
{

	// TODO: finalize properly, current solution results in model->index(..).parent() != model


	struct LiveNodeModelBridgeDetail
	{
		enum Signals
		{
#define ENUM_FLAG_RANGE(begin, end) (((end - 1) << 1) - (begin ? ((begin - 1) << 1) : 1))
				marker_first,

			// QAbstractItemModel signals
		    sig_headerDataChanged = 1 << 0,
		    sig_layoutChanged = 1 << 1,
		    sig_layoutAboutToBeChanged = 1 << 2,

				marker_metadata_changes,
		    metadata_signals = ENUM_FLAG_RANGE(marker_first, marker_metadata_changes),

		    sig_dataChanged = 1 << 3,

				marker_data_changes,
			data_signals = ENUM_FLAG_RANGE(marker_metadata_changes, marker_data_changes),

		    sig_rowsAboutToBeInserted = 1 << 4,
		    sig_rowsInserted = 1 << 5,

		    sig_rowsAboutToBeRemoved = 1 << 6,
		    sig_rowsRemoved = 1 << 7,

				marker_row_changes,
			row_signals = ENUM_FLAG_RANGE(marker_data_changes, marker_row_changes),

		    sig_columnsAboutToBeInserted = 1 << 8,
		    sig_columnsInserted = 1 << 9,

		    sig_columnsAboutToBeRemoved = 1 << 10,
		    sig_columnsRemoved = 1 << 11,

				marker_column_changes,
			column_signals = ENUM_FLAG_RANGE(marker_row_changes, marker_column_changes),

		    sig_modelAboutToBeReset = 1 << 12,
		    sig_modelReset = 1 << 13,

				marker_content,
		    content_signals = ENUM_FLAG_RANGE(marker_metadata_changes, marker_content),

			sig_modelUpdated = 1 << 14,
			sig_derivedsUpdated = 1 << 15,
			sig_rowsUpdated = 1 << 16,

				marker_livenodemodel,
			livenodemodel_signals = ENUM_FLAG_RANGE(marker_content, marker_livenodemodel),

			all_signals = ENUM_FLAG_RANGE(marker_first, marker_livenodemodel)
		};

		void connectLiveNodeModelBridge(QObject *backend, QObject *bridge, unsigned connect_signals);
	};

		template<typename Base = LiveNodeModel>
	class LiveNodeModelBridge
		: public Base
		, public LiveNodeModelBridgeDetail
	{
		/* VZR_CLASS(LiveNodeModelBridge
				, (Base)
				, (LiveNodeModelBridgeDetail));*/

	protected:
		QSharedPointer<Base> backend_; // VZR_MEMBER


	public:
		LiveNodeModelBridge() {}

		LiveNodeModelBridge(QSharedPointer<Base> const &backend, unsigned connect_signals = all_signals)
			: Base()
		{
			initialize(backend, connect_signals);
		}
			template<typename T>
		LiveNodeModelBridge(T const &p1, QSharedPointer<Base> const &backend, unsigned connect_signals = all_signals)
			: Base(p1)
		{
			initialize(backend, connect_signals);
		}

		void initialize(QSharedPointer<Base> const &backend, unsigned connect_signals = all_signals)
		{
			backend_ = backend;
			connectLiveNodeModelBridge(backend.data(), this, connect_signals);
		}

		QModelIndex index(int row, int column, const QModelIndex &parent) const { return backend_->index(row, column, parent); }

		QModelIndex parent(const QModelIndex &child) const { return backend_->parent(child); }

		int rowCount(const QModelIndex &parent) const { return backend_->rowCount(parent); }
		int columnCount(const QModelIndex &parent) const { return backend_->columnCount(parent); }
		bool hasChildren(const QModelIndex &parent) const { return backend_->hasChildren(parent); }

		QVariant data(const QModelIndex &index, int role) const { return backend_->data(index, role); }
		bool setData(const QModelIndex &index, const QVariant &value, int role) { return backend_->setData(index, value, role); }

		QVariant headerData(int section, Qt::Orientation orientation, int role) const { return backend_->headerData(section, orientation, role); }
		bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) { return backend_->setHeaderData(section, orientation, value, role); }

		QMap<int, QVariant> itemData(const QModelIndex &index) const { return backend_->itemData(index); }
		bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) { return backend_->setItemData(index, roles); }

		QStringList mimeTypes() const { return backend_->mimeTypes(); }
		QMimeData *mimeData(const QModelIndexList &indexes) const { return backend_->mimeData(indexes); }
		bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) { return backend_->dropMimeData(data, action, row, column, parent); }
		Qt::DropActions supportedDropActions() const { return backend_->supportedDropActions(); }

		bool insertRows(int row, int count, const QModelIndex &parent) { return backend_->insertRows(row, count, parent); }
		bool insertColumns(int column, int count, const QModelIndex &parent) { return backend_->insertColumns(column, count, parent); }
		bool removeRows(int row, int count, const QModelIndex &parent) { return backend_->removeRows(row, count, parent); }
		bool removeColumns(int column, int count, const QModelIndex &parent) { return backend_->removeColumns(column, count, parent); }

		void fetchMore(const QModelIndex &parent) { return backend_->fetchMore(parent); }
		bool canFetchMore(const QModelIndex &parent) const { return backend_->canFetchMore(parent); }
		Qt::ItemFlags flags(const QModelIndex &index) const { return backend_->flags(index); }
		void sort(int column, Qt::SortOrder order) { return backend_->sort(column, order); }
		QModelIndex buddy(const QModelIndex &index) const { return backend_->buddy(index); }
		QModelIndexList match(const QModelIndex &start, int role, const QVariant &value, int hits, Qt::MatchFlags flags) const { return backend_->match(start, role, value, hits, flags); }
		QSize span(const QModelIndex &index) const { return backend_->span(index); }

		bool submit() { return backend_->submit(); }
		void revert() { return backend_->revert(); }

		bool columnHasType(int column, LiveNode const &type) const { return backend_->columnHasType(column, type); }

		RDFGraphImplBase *graph() const { return static_cast<RDFGraphImplBase *>(backend_->graph()); /* FIXME: hack */ }
		RDFStrategyFlags strategy(QModelIndex const &index = QModelIndex()) const { return backend_->strategy(index); }
		bool alterStrategy(RDFStrategyFlags flags, RDFStrategyFlags mask, QModelIndex const &index = QModelIndex()) { return backend_->alterStrategy(flags, mask, index); }
		RDFVariableList sourceVariables() const { return backend_->sourceVariables(); }

		bool clearRows(int row, int count, QModelIndex const &parent) { return backend_->clearRows(row, count, parent); }
		bool insertRows(int row, LiveNodes const &rows, QModelIndex const &parent) { return backend_->insertRows(row, rows, parent); }
		bool replaceRows(int row, LiveNodes const &rows, QModelIndex const &parent) { return backend_->replaceRows(row, rows, parent); }
		bool transitionRows(int row, int count, LiveNodes const &rows, bool smooth_transition, QModelIndex const &parent) { return backend_->transitionRows(row, count, rows, smooth_transition, parent); }
		bool insertRowsRaw(int row, QVector<QStringList> const &raw_rows, QModelIndex const &parent) { return backend_->insertRowsRaw(row, raw_rows, parent); }
		bool replaceRowsRaw(int row, QVector<QStringList> const &raw_rows, QModelIndex const &parent) { return backend_->replaceRowsRaw(row, raw_rows, parent); }
		bool transitionRowsRaw(int row, int count, QVector<QStringList> const &raw_rows, bool smooth_transition, QModelIndex const &parent) { return backend_->transitionRowsRaw(row, count, raw_rows, smooth_transition, parent); }

		bool assignModel(LiveNodeModel const &model, bool smooth_transition) { return backend_->assignModel(model, smooth_transition); }

		bool refreshModel(LiveNodeModel::RefreshMode mode = LiveNodeModel::Flush) { return backend_->refreshModel(mode); }
		bool refreshRows(int row, int count, QModelIndex const &parent, LiveNodeModel::RefreshMode mode = LiveNodeModel::Flush) { return backend_->refreshRows(row, count, parent, mode); }

		LiveNodeModelPtr subModel(QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive, QList<int> const &columns_mapping) { return backend_->subModel(topleft_inclusive, bottomright_exclusive, columns_mapping); }
		LiveNodeModel *plainModel() { return backend_->plainModel(); }
		LiveNodeModelPtr childModel(QModelIndex const &index, RDFStrategyFlags flags = RDFStrategy::DefaultStrategy, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) { return backend_->childModel(index, flags, parent); }

		QVariant modelAttribute(QString const &attribute_name) const { return backend_->modelAttribute(attribute_name); }
		QVariantMap modelAttributes() const { return backend_->modelAttributes(); }
		bool setModelAttribute(QString const &attribute_name, QVariant const &value) { return backend_->setModelAttribute(attribute_name, value); }

		QStringList rawRow(int row, QModelIndex const &parent) const { return backend_->rawRow(row, parent); }
	};

	// implement LiveNodeModelOfGraphBridge

	class LiveNodeModelOfGraphBridge
		: public EntityOfGraph<LiveNodeModelBridge<> >
	{
		/* VZR_CLASS(LiveNodeModelOfGraphBridge, (EntityOfGraph<LiveNodeModelBridge<> >));*/

	public:
		typedef EntityOfGraph<LiveNodeModelBridge<> > Base;
		LiveNodeModelOfGraphBridge(QSharedPointer<RDFGraphImplBase> const &graph, LiveNodeModelPtr const &backend)
			: Base(graph, backend)
		{}

		QVariant data(const QModelIndex &index, int role) const
		{
			QVariant ret = backend_->data(index, role);
			if(role == LiveNodeModel::LiveNodeRole || role == LiveNodeModel::OldLiveNodeRole)
				return QVariant::fromValue(graph()->wrapLiveNode(ret.value<LiveNode>()));
			return ret;
		}
		LiveNodeModelPtr subModel(QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive, QList<int> const &columns_mapping) const
		{
			return graph()->wrapLiveNodes(backend_->subModel(topleft_inclusive, bottomright_exclusive, columns_mapping)).entity();
		}
	};

	class LiveNodeModelBaseBridge
		: public LiveNodeModelBridge<LiveNodeModelBase>
	{
		/* VZR_CLASS(LiveNodeModelBaseBridge, (LiveNodeModelBridge<LiveNodeModelBase>));*/

	public:
		LiveNodeModelBaseBridge(QSharedPointer<RDFGraphImplBase> const &graph, LiveNodeModelBasePtr const &backend)
			: LiveNodeModelBridge<LiveNodeModelBase>(graph, backend)
		{}

		QVariant data(const QModelIndex &index, int role) const
		{
			QVariant ret = backend_->data(index, role);
			if(role == LiveNodeModel::LiveNodeRole || role == LiveNodeModel::OldLiveNodeRole)
				return QVariant::fromValue(graph()->wrapLiveNode(ret.value<LiveNode>()));
			return ret;
		}
		LiveNodeModelPtr subModel(QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive, QList<int> const &columns_mapping) const
		{
			return graph()->wrapLiveNodes(backend_->subModel(topleft_inclusive, bottomright_exclusive, columns_mapping)).entity();
		}

		bool updateFrom_chain
				( UpdateContext const &update_context
				, InterfaceChain<ModelCommons> chain = InterfaceChain<ModelCommons>())
		{
			if(chain.isFirstCompositeCall())
				return LiveNodeModelBase::updateFrom_chain(update_context, chain);
			return backend_->updateFrom(update_context);
		}

		bool updateTargetModule_chain
				( ModelModule *target_module, UpdateContext const &update_context
				, InterfaceChain<ModelCommons const> chain
						= InterfaceChain<ModelCommons const>()) const
		{
			if(chain.isFirstCompositeCall())
				return LiveNodeModelBase::updateTargetModule_chain
						( target_module, update_context, chain);
			return backend_->updateTargetModule(target_module, update_context);
		}
	};
}


#endif /* SOPRANOLIVE_LIVENODEMODELBRIDGE_P_H_ */
