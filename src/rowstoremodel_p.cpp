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
 * rowstoremodel_p.cpp
 *
 *  Created on: May 4, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include "rowstoremodel_p.h"
#include "subrowstoremodel_p.h"
#include "lnm_transitionstrategy_p.h"
#include "util_p.h"

namespace SopranoLive
{
	ContentModule::ContentModule(ChangePolicy change_policy)
		: Base()
		, rows_(0), change_policy(change_policy)
	{
		//debug(4) << "ContentModule(" << this << ") constructed with change_policy:" << change_policy;
	}

	ContentModule::ContentModule(ContentModule const &cp)
		: Base(cp)
		, ModuleInterface<ModelCommons, RowStoreModel, ContentModule>(cp)
		, rows_(0), change_policy(cp.change_policy)
	{
		//debug(4) << "ContentModule(" << this << ") copied with change_policy:" << change_policy;
	}

	ContentModule::~ContentModule()
	{
		//debug(4) << "ContentModule(" << this << ") destroyed";
	}

	RowStore &ContentModule::rowCache(RowStoreModel &model) { return model.row_store; }
	RowStore const &ContentModule::rowCache(RowStoreModel const &model) { return model.row_store; }

	ContentModule::ModulePtr ContentModule::attach(LiveNodeModelBase &model)
	{
		if(!Base::attach(model))
			return ModelModulePtr();
		// if rows exists and is different from model.row_store, perform a smooth transition
		if(rows_)
		{
			if(rows_ != &this->model()->row_store)
				transitionRows(0, model.rowCount(), *rows_);
		} else
			model.removeRows(0, model.rowCount());
		return sharedFromThis();
	}

	void ContentModule::attached(RowStoreModel &model)
	{
		Base::attached(model);
		rows_ = &model.row_store;
	}

	void ContentModule::detached(RowStoreModel &model)
	{
		rows_ = 0;
		Base::detached(model);
	}

	RowStore::iterator ContentModule::createRow(int row)
	{
		model()->refreshRows(row, 1, QModelIndex(), LiveNodeModel::Block);
		return rowCache().find(row);
	}

	QSharedPointer<ContentModule> ContentModule::copy() const
	{
		return QSharedPointer<ContentModule>();
	}

	bool ContentModule::aboutToChange(int /*row*/, int /*count*/)
	{
		model()->refreshModel(LiveNodeModel::Block | LiveNodeModel::CoverDerivedProperties);
		return this->model()->changeContentModule();
	}


	RowStoreModel::RowStoreModel(QSharedPointer<RDFGraphImplBase> const &graph, RDFStrategyFlags strategy)
		: LiveNodeModelBase(graph, strategy)
		, row_store()
		, content_module_handle_(attachFrontModule())
	{
		//debug(4) << "RowStoreModel RSM(" << this << ") created";
	}

	void detailPrimeDefaultRoleStates(int c, RowStoreModel::RoleStates &role_states)
	{
		quint64 col = c;
		enum
		{ 	HasDefault = RowStoreModel::HasDefault
		,	ClearRoleOnChange = RowStoreModel::ClearRoleOnChange
		};
		role_states[(col << 32) + Qt::DisplayRole] = HasDefault | ClearRoleOnChange;
		role_states[(col << 32) + Qt::DecorationRole] = HasDefault;
		role_states[(col << 32) + Qt::EditRole] = HasDefault | ClearRoleOnChange;
		role_states[(col << 32) + Qt::ToolTipRole] = HasDefault;
		role_states[(col << 32) + Qt::StatusTipRole] = HasDefault;
		role_states[(col << 32) + Qt::WhatsThisRole] = HasDefault;
		role_states[(col << 32) + Qt::SizeHintRole] = HasDefault;
		role_states[(col << 32) + Qt::FontRole] = HasDefault;
		role_states[(col << 32) + Qt::TextAlignmentRole] = HasDefault;
		role_states[(col << 32) + Qt::BackgroundColorRole] = HasDefault;
		role_states[(col << 32) + Qt::TextColorRole] = HasDefault;
		role_states[(col << 32) + Qt::CheckStateRole] = HasDefault | ClearRoleOnChange;
		role_states[(col << 32) + Qt::AccessibleTextRole] = HasDefault | ClearRoleOnChange;
		role_states[(col << 32) + Qt::AccessibleDescriptionRole] = HasDefault;

		role_states[(col << 32) + LiveNodeModel::LiveNodeRole] = HasDefault | ClearRoleOnChange;
		role_states[(col << 32) + LiveNodeModel::NodeRole] = HasDefault | ClearRoleOnChange;
		role_states[(col << 32) + LiveNodeModel::RawDataRole] = HasDefault | ClearRoleOnChange;
		role_states[(col << 32) + LiveNodeModel::ChildModelQObjectRole] = HasDefault | ClearRoleOnChange;
		role_states[(col << 32) + LiveNodeModel::ChildModelRole] = HasDefault | ClearRoleOnChange;
	}

	QMap<int, QVariant> detailPrimeDefaultRoleValues()
	{
		QMap<int, QVariant> ret;

		ret[Qt::DisplayRole] = QVariant("");
		ret[Qt::DecorationRole] = QVariant();
		ret[Qt::EditRole] = QVariant("");
		ret[Qt::ToolTipRole] = QVariant();
		ret[Qt::StatusTipRole] = QVariant();
		ret[Qt::WhatsThisRole] = QVariant();
		ret[Qt::SizeHintRole] = QVariant();
		ret[Qt::FontRole] = QVariant();
		ret[Qt::TextAlignmentRole] = QVariant();
		ret[Qt::BackgroundColorRole] = QVariant();
		ret[Qt::TextColorRole] = QVariant();
		ret[Qt::CheckStateRole] = QVariant();
		ret[Qt::AccessibleTextRole] = QVariant();
		ret[Qt::AccessibleDescriptionRole] = QVariant();

		ret[LiveNodeModel::LiveNodeRole] = QVariant();
		ret[LiveNodeModel::NodeRole] = QVariant();
		ret[LiveNodeModel::RawDataRole] = QVariant();
		ret[LiveNodeModel::ChildModelQObjectRole] = QVariant();
		ret[LiveNodeModel::ChildModelRole] = QVariant();

		return ret;
	}

	QMap<int, QVariant> global_default_role_values_ = detailPrimeDefaultRoleValues();

	void RowStoreModel::setColumnInformation(RDFDerivedSelect const &columns)
	{
		LiveNodeModelBase::setColumnInformation(columns);
		default_empty_row_ = QStringList();
		for(int i = columns_.size(); i--; )
		{
			default_role_values_.push_back(global_default_role_values_);
			detailPrimeDefaultRoleStates(i, default_role_states_);
			// TODO: column specific non-empty default values?
			default_empty_row_ << QString();
		}
	}

	RowStoreModel::~RowStoreModel()
	{
		detachAllCompositorModules();
		//debug(4) << "RowStoreModel RSM(" << this << ") destroyed";
	}

	RowStore::iterator RowStoreModel::createRowImpl
			( RowStore &cache, ContentModule *content_module, unsigned row) const
	{
		RowStore::iterator ret;
		// if valid hit in row store, return it
		if((row < cache.size() && (ret = cache.find(row)).isValid())
				// otherwise, if the content_module gives us a valid iterator, return it
				|| (content_module && (ret = content_module->createRow(row)).isValid()))
			return ret;

		// if not within row store area, report error and return null
		if(row >= cache.size())
		{
			warning() << "Model row index out of bounds; requested row(" << row
					  << ") >= row count(" << cache.size() << ")";
			return RowStore::iterator();
		}
		// create and return an empty row store entry
		return cache.replace(row, QStringList());
	}

	bool RowStoreModel::changeContentModule(ContentModulePtr const &new_module)
	{
		return replaceModule(content_module_handle_, new_module) != modules().end();
	}

	QModelIndex RowStoreModel::index(int row, int column, const QModelIndex &parent) const
	{
		// TODO: figure out a better strategy:
		// SubRowStoreModel uses out-of-bounds indices as non-inclusive area boundary
		// delimiters on right and down edges, so dont check here
		// return hasIndex(row, column, parent) ? createIndex(row, column, 0) : QModelIndex();
		Q_UNUSED(parent);
		if(row < 0 || column < 0)
			return QModelIndex();
		return createIndex(row, column, 0);
	}
	QModelIndex RowStoreModel::parent(const QModelIndex &/*child*/) const
	{
		return QModelIndex();
	}

// LCOV_EXCL_START
	QVariant RowStoreModel::parseString(QString data, int /*row*/) const
	{
		qWarning() << "BEWARE: untested code";
		QUrl url(data, QUrl::StrictMode);
		if(url.isValid())
			return url;
		return data;
	}
// LCOV_EXCL_STOP

	int RowStoreModel::rowCount(const QModelIndex &index) const
	{
		if(index.isValid())
			return 0;

		// if running, but not streaming nor windowed, do blocking refresh.
		if(((strategy(index) ^ RDFStrategy::Running)
			& (RDFStrategy::Running | RDFStrategy::Streaming | RDFStrategy::Windowed)) == 0)
			const_cast<RowStoreModel *>(this)->refreshModel
				(LiveNodeModel::Block | LiveNodeModel::CoverDerivedProperties);

		return row_store.size();
	}

	QVariant RowStoreModel::dataImpl
			( QModelIndex const &index, int role, int &role_flags
			, RowStore::const_iterator const &crowi) const
	{
		if(role == (ChildModelQObjectRole | ChildModelRole))
		{
			// with QAbstractItemModel::data const, we can never achiever pure const correctness
			LiveNodes child_model = const_cast<RowStoreModel *>(this)->childModel(index,
					(role_flags & DataRoleCache ? CacheDataRoles : RDFStrategyFlags()));
			// adding to role cache is done by childModel
			role_flags &=  ~DataRoleCache;
			return (role == ChildModelRole)
				? QVariant::fromValue(child_model)
				: QVariant::fromValue(static_cast<QObject *>(child_model->plainModel()));
		}
		if(unsigned(index.row()) >= row_store.size()
				|| unsigned(index.column()) >= unsigned(columns_.visibleColumnCount()))
			warning(this, "QAbstractItemModel::data") << "Out of bounds: Model index("
						  << index << ") outside (0 <= row <" << row_store.size()
						  << ", 0 <= column <" << columns_.visibleColumnCount() << ")";
		else if(crowi == row_store.end())
			warning(this, "QAbstractItemModel::data") << "Model index(" << index << ") is in an empty region";
		else
		{
			RoleStates::const_iterator crsi = default_role_states_.find
					((quint64(index.column()) << 32) + role);
			if(crsi != default_role_states_.end() && *crsi & HasDefault)
				return default_role_values_[index.column()][role];
			warning(this, "QAbstractItemModel::data") << "No role(" << role << ") exists for index(" << index << ")";
		}

		return QVariant();
	}

	QVariant ContentModule::data
			( QModelIndex const &index, int role, int &role_flags, RowStore::const_iterator const &crowi) const
	{
		return model()->dataImpl(index, role, role_flags, crowi);
	}

	QVariant RowStoreModel::data(const QModelIndex &index, int role) const
	{
		int column = index.column(), row = index.row();

		int role_flags = role & LiveNodeModel::RoleFlags;
		role &= ~LiveNodeModel::RoleFlags;
		// TODO: flag parallelize and thus remove the conditional
		// (realign RDFStrategy flags and mask overridden ones out)
		if(!(role_flags & LiveNodeModel::OverrideDataRoleCache))
			role_flags |= (strategy_ & RDFStrategy::CacheDataRoles)
					>> (RDFStrategy::offset_CacheDataRoles - offset_DataRoleCache);

		// TODO: const-ify me
		RowStore::iterator crowi = row_store.find(row);

		QVariant ret;

		ContentModule *ct = contentModule().data();

		if(crowi != row_store.end()
			|| (ct && (crowi = ct->createRow(row)).isValid()))
		{
			QVariant const *cached_data_role
					= crowi.block().dataRole(crowi.blockIndex(), column, role);
			if(cached_data_role && cached_data_role->isValid())
				return *cached_data_role;
			else if(column < crowi->size())
			{
				QString const &str = (*crowi)[column];
				if(role == LiveNodeModel::RawDataRole)
					ret = QVariant(str);
				else if(role == Qt::DisplayRole || role == Qt::EditRole)
					// TODO: can be optimized: use column-based conversion callback instead,
					// which are shared per RDFService.
					ret = QVariant(graph_->service()
							->rawNodeFromString(str, columns_.variable(column).varCppType()).toString());
				else if((role == LiveNodeModel::LiveNodeRole || role == LiveNodeModel::OldLiveNodeRole))
				{
					CppType type = columns_.variable(column).varCppType();
					Node node = graph_->service()->rawNodeFromString(str, type);
					ret = QVariant::fromValue(graph_->RDFGraph::liveNode
							(node, LiveNode(), Contexts::NoChecks::iriList()));
				}
				else
					goto rowstoremodel_data__multilevel_else;
			} else
				goto rowstoremodel_data__multilevel_else;
		} else
		{
rowstoremodel_data__multilevel_else:
			if(ct)
				ret = ct->data(index, role, role_flags, crowi);
		}

		// insert the value to role cache if the CacheDataRoles is enabled
		if((role_flags & DataRoleCache) && ret.isValid() && crowi.isValid())
			crowi.container().find(crowi.key()).block().setDataRole(crowi.blockIndex(), column, ret, role);

		return ret;
	}

	bool RowStoreModel::setDataImpl(QModelIndex const &index, QVariant const &value, int role)
	{
		int row = index.row(), column = index.column();
		if(unsigned(row) >= unsigned(row_store.size())
				|| unsigned(column) >= unsigned(columns_.visibleColumnCount()))
		{
			warning(this, "QAbstractItemModel::setData")
				<< "Out of bounds: Model index(" << index << ") outside (0 <= row <"
				<< row_store.size() << ", 0 <= column <" << columns_.visibleColumnCount() << ")";
			return false;
		}

		RowStore::iterator rowi = row_store.find(row);
		// if row is pointing to a gap region, initialize it
		if(rowi == row_store.end())
			rowi = row_store.replace(row, default_empty_row_);

		if(role == Qt::DisplayRole || role == Qt::EditRole)
			(*rowi)[column] = value.toString();
		else
			rowi.block().setDataRole(rowi.blockIndex(), column, value, role);

		sharedFromThis(), Q_EMIT dataChanged(index, index);

		return true;
	}
	bool ContentModule::setData(QModelIndex const &index, QVariant const &value, int role)
	{
		RowStoreModel *model = this->model();
		if(!onChange(index.row(), 1)) return false;
		return model->setDataImpl(index, value, role);
	}
	bool RowStoreModel::setData(QModelIndex const &index, QVariant const &value, int role)
	{
		if(ContentModule *ct = contentModule().data())
			return ct->setData(index, value, role);
		return setDataImpl(index, value, role);
	}

	QMap<int, QVariant> RowStoreModel::itemDataImpl(QModelIndex const &index) const
	{

		if(unsigned(index.row()) >= row_store.size()
				|| unsigned(index.column()) >= unsigned(columns_.visibleColumnCount()))
			warning(this, "QAbstractItemModel::itemData")
				<< "Out of bounds: Model index(" << index
				<< ") outside (0 <= row <" << row_store.size()
				<< ", 0 <= column <" << columns_.visibleColumnCount() << ")";
		else
		{
			RowStore::const_iterator crowi = row_store.constFind(index.row());
			if(crowi == row_store.end())
				warning(this, "QAbstractItemModel::itemData")
					<< "Model index(" << index << ") is in an empty region";
			else
			{
				QMap<int, QVariant> ret = default_role_values_[index.column()];
				crowi.block().dataRoles(crowi.blockIndex(), index.column(), ret);
				ret.insert(Qt::DisplayRole, *crowi);
				ret.insert(Qt::EditRole, *crowi);
				return ret;
			}
		}
		return QMap<int, QVariant>();
	}

	QMap<int, QVariant> ContentModule::itemData(QModelIndex const &index) const
	{
		return model()->itemDataImpl(index);
	}

	QMap<int, QVariant> RowStoreModel::itemData(QModelIndex const &index) const
	{
		if(ContentModule *ct = contentModule().data())
			return ct->itemData(index);
		return itemDataImpl(index);
	}

	bool RowStoreModel::setItemDataImpl(QModelIndex const &index, QMap<int, QVariant> const &roles)
	{
		int row = index.row(), column = index.column();
		if(unsigned(row) >= unsigned(row_store.size())
				|| unsigned(column) >= unsigned(columns_.visibleColumnCount()))
			return false;

		RowStore::iterator rowi = row_store.find(row);
		// if row is pointing to a gap region, initialize it
		if(rowi == row_store.end())
			rowi = row_store.replace(row, default_empty_row_);

		for(QMap<int, QVariant>::const_iterator cri = roles.begin(), criend = roles.end()
				; cri != criend; ++cri)
			if(cri.key() == Qt::DisplayRole || cri.key() == Qt::EditRole)
				setData(index, cri.value());
			else
				rowi.block().setDataRole(rowi.blockIndex(), column, cri.value(), cri.key());
		return true;
	}
	bool ContentModule::setItemData(QModelIndex const &index, QMap<int, QVariant> const &roles)
	{
		RowStoreModel *model = this->model();
		if(!onChange(index.row(), 1)) return false;
		return model->setItemDataImpl(index, roles);
	}
	bool RowStoreModel::setItemData(QModelIndex const &index, QMap<int, QVariant> const &roles)
	{
		if(ContentModule *ct = contentModule().data())
			return ct->setItemData(index, roles);
		return setItemDataImpl(index, roles);
	}

// LCOV_EXCL_START
	void RowStoreModel::insertRowImpl(int row, QStringList const &new_row)
	{
		qWarning() << "BEWARE: untested code, exists for completeness";
		beginInsertRows(QModelIndex(), row, row);
		row_store.insert(row, new_row);
		endInsertRows();
	}
	bool ContentModule::insertRow(int row, QStringList const &new_row)
	{
		qWarning() << "BEWARE: untested code, exists for completeness";
		RowStoreModel *model = this->model();
		if(!onChange(row, 0)) return false;
		model->insertRowImpl(row, new_row);
		return true;
	}
// LCOV_EXCL_STOP

	void RowStoreModel::insertRowsImpl(int first_row, int gap_count)
	{
		if(gap_count)
		{
			beginInsertRows(QModelIndex(), first_row, first_row + gap_count - 1);
			row_store.insertGap(first_row, gap_count);
			endInsertRows();
		}
	}
	bool ContentModule::insertRows(int first_row, int gap_count)
	{
		RowStoreModel *model = this->model();
		if(!onChange(first_row, 0)) return false;
		model->insertRowsImpl(first_row, gap_count);
		return true;
	}

// LCOV_EXCL_START
	bool RowStoreModel::insertRow(int row, QModelIndex const &/*parent*/)
	{
		qWarning() << "BEWARE: untested code";
		if(ContentModule *ct = contentModule().data())
			return ct->insertRows(row, 1);
		insertRowsImpl(row, 1);
		return true;
	}
// LCOV_EXCL_STOP

	bool RowStoreModel::insertRows(int row, int count, QModelIndex const &/*parent*/)
	{
		if(ContentModule *ct = contentModule().data())
			return ct->insertRows(row, count);
		insertRowsImpl(row, count);
		return true;
	}

	void RowStoreModel::clearRowsImpl(int row, int gap_size)
	{
		replaceRowsImpl(row, RowStore(gap_size));
	}

	void RowStoreModel::replaceRowImpl(int row, QStringList const &new_row)
	{
		if(unsigned(row) >= row_store.size())
		{
			beginInsertRows(QModelIndex(), row_store.size(), row);
			row_store.insert(row, new_row);
			endInsertRows();
		} else
		{
			row_store.replace(row, new_row);
			sharedFromThis(), Q_EMIT dataChanged
					(index(row, 0), index(row, columns_.visibleColumnCount() - 1));
		}
	}

	void RowStoreModel::replaceRowsImpl(int first_row, ConstRowStoreRange const &new_rows)
	{
		int old_last = row_store.size();
		int new_last = first_row + new_rows.size();
		if(new_last > old_last)
		{
			beginInsertRows(QModelIndex(), old_last, new_last - 1);
			row_store.replace(first_row, new_rows.begin(), new_rows.end());
			endInsertRows();
		} else
		{
			row_store.replace(first_row, new_rows.begin(), new_rows.end());
			old_last = new_last;
		}
		if(old_last > first_row)
			sharedFromThis(), Q_EMIT dataChanged
					( index(first_row, 0)
					, index(old_last - 1, columns_.visibleColumnCount() - 1));
	}


	RowStore::iterator RowStoreModel::eraseRowsImpl(RowStoreRange const &range)
	{
		if(int row_count = range.size())
		{
			int first_row = range.begin().key();
			beginRemoveRows(QModelIndex(), first_row, first_row + row_count - 1);
			RowStore::iterator ret = row_store.erase(range.begin(), range.end());
			endRemoveRows();
			return ret;
		}
		return range.begin();
	}

	bool RowStoreModel::hardTransitionImpl(int first_row, int row_count, ConstRowStoreRange const &new_rows)
	{
		if(row_store.size() > unsigned(first_row))
			removeRowsImpl(first_row, row_count);

		if(int new_row_count = new_rows.size())
		{
			beginInsertRows(QModelIndex(), first_row, first_row + new_row_count - 1);
			row_store.insert(first_row, new_rows.begin(), new_rows.end());
			endInsertRows();
		}
		return true;
	}
	bool RowStoreModel::transitionRowsImpl
			( int first_row, int row_count, ConstRowStoreRange const &new_rows, bool smooth)
	{
		if(!row_count || unsigned(first_row) >= row_store.size())
			return insertRowsImpl(first_row, new_rows), true;
		if(smooth && new_rows.size()
				&& SmoothTransitionModule(this).initialStackShare<SmoothTransitionModule>()
						->migrate(first_row, row_count, new_rows))
			return true;
		return hardTransitionImpl(first_row, row_count, new_rows);
	}


	void RowStoreModel::removeRowsImpl(int first_row, int row_count)
	{
		if(row_count)
		{
			beginRemoveRows(QModelIndex(), first_row, first_row + row_count - 1);
			row_store.remove(first_row, row_count);
			endRemoveRows();
		}
	}
	bool ContentModule::removeRows(int first_row, int row_count)
	{
		RowStoreModel *model = this->model();
		if(!onChange(first_row, row_count)) return false;
		model->removeRowsImpl(first_row, row_count);
		return true;
	}
	bool RowStoreModel::removeRow(int row, QModelIndex const &/*parent*/)
	{
		if(ContentModule *ct = contentModule().data())
			return ct->removeRows(row, 1);
		removeRowsImpl(row, 1);
		return true;
	}
	bool RowStoreModel::removeRows(int row, int count, QModelIndex const &/*parent*/)
	{
		if(ContentModule *ct = contentModule().data())
			return ct->removeRows(row, count);
		removeRowsImpl(row, count);
		return true;
	}


	RDFStrategyFlags ContentModule::strategy_chain
			( QModelIndex const &index, InterfaceChain<ModelCommons const> chain) const
	{
		RDFStrategyFlags accumulator = ModelCommons::strategy_chain(index, chain);
		return (change_policy == DenyChanges)
				? (accumulator & ~RDFStrategy::Writable)
				: (accumulator | RDFStrategy::Writable);
	}

	RowStore RowStoreModel::cacheFromLiveNodes(LiveNodes const &livenodes)
	{
		if(LiveNodeModel *model = livenodes.model())
		{
			if(RowStoreModel *modeldata = dynamic_cast<RowStoreModel *>(model))
				return modeldata->row_store;
			QVector<QStringList> ret;
			int columns = model->columnCount();
			for(int r = 0, rend = model->rowCount(); r < rend; ++r)
			{
				ret.push_back(QStringList());
				QStringList &b = ret.back();
				for(int c = 0; c < columns; ++c)
					b.push_back(model->data(model->index(r, c), Qt::DisplayRole).toString());
			}
			return RowStore(ret);
		}
		QVector<QStringList> ret;
		QList<LiveNode> const &nodes = livenodes.nodes();
		ret.reserve(nodes.size());
		for(QList<LiveNode>::const_iterator ni = nodes.begin(), niend = nodes.end(); ni != niend; ++ni)
			ret.push_back(QStringList(ni->toString()));
		return RowStore(ret);
	}

	void RowStoreModel::insertRowsImpl(int first_row, ConstRowStoreRange const &new_rows)
	{
		if(int new_row_count = new_rows.size())
		{
			beginInsertRows(QModelIndex(), first_row, first_row + new_row_count - 1);
			row_store.insert(first_row, new_rows.begin(), new_rows.end());
			endInsertRows();
		}
	}
	bool ContentModule::insertRows(int first_row, ConstRowStoreRange const &new_rows)
	{
		RowStoreModel *model = this->model();
		if(!onChange(first_row, 0)) return false;
		model->insertRowsImpl(first_row, new_rows);
		return true;
	}
	bool RowStoreModel::insertRows(int row, LiveNodes const &liverows, QModelIndex const &/*parent*/)
	{
		if(ContentModule *ct = contentModule().data())
			return ct->insertRows(row, cacheFromLiveNodes(liverows));
		insertRowsImpl(row, cacheFromLiveNodes(liverows));
		return true;
	}

	void RowStoreModel::appendRowsImpl(int gap_count)
	{
		if(gap_count)
		{
			int end = row_store.size();
			beginInsertRows(QModelIndex(), end, end + gap_count - 1);
			row_store.resize(row_store.size() + gap_count);
			endInsertRows();
		}
	}
	void RowStoreModel::appendRowsImpl(QStringList const &new_row)
	{
		int end = row_store.size();
		beginInsertRows(QModelIndex(), end, end);
		row_store.append(new_row);
		endInsertRows();
	}
	void RowStoreModel::appendRowsImpl(ConstRowStoreRange const &new_rows)
	{
		if(int new_row_count = new_rows.size())
		{
			int end = row_store.size();
			beginInsertRows(QModelIndex(), end, end + new_row_count - 1);
			row_store.append(new_rows.begin(), new_rows.end());
			endInsertRows();
		}
	}

	bool ContentModule::replaceRow(int row, QStringList const &new_row)
	{
		RowStoreModel *model = this->model();
		if(!onChange(row, 1)) return false;
		model->replaceRowImpl(row, new_row);
		return true;
	}

	bool ContentModule::replaceRows(int first_row, ConstRowStoreRange const &new_rows)
	{
		if(int new_row_count = new_rows.size())
		{
			RowStoreModel *model = this->model();
			if(!onChange(first_row, new_row_count)) return false;
			model->replaceRowsImpl(first_row, new_rows);
		}
		return true;
	}
	bool RowStoreModel::replaceRows(int row, LiveNodes const &liverows, QModelIndex const &/*parent*/)
	{
		if(ContentModule *ct = contentModule().data())
			return ct->replaceRows(row, cacheFromLiveNodes(liverows));
		replaceRowsImpl(row, cacheFromLiveNodes(liverows));
		return true;
	}

	bool ContentModule::transitionRows
			( int first_row, int row_count, ConstRowStoreRange const &new_rows, bool smooth)
	{
		RowStoreModel *model = this->model();
		if(!onChange(first_row, row_count)) return false;
		return model->transitionRowsImpl(first_row, row_count, new_rows, smooth);
	}
	bool RowStoreModel::transitionRows
			( int row, int count, LiveNodes const &liverows, bool smooth_transition, QModelIndex const &/*parent*/)
	{
		RowStore rows = cacheFromLiveNodes(liverows);
		if(ContentModule *ct = contentModule().data())
			return ct->transitionRows(row, count, rows, smooth_transition);
		transitionRowsImpl(row, count, rows, smooth_transition);
		return true;
	}

	bool ContentModule::clearRows(int first_row, int gap_count)
	{
		RowStoreModel *model = this->model();
		if(!onChange(first_row, gap_count)) return false;
		model->clearRowsImpl(first_row, gap_count);
		return true;
	}
	bool RowStoreModel::clearRows(int row, int gap_count, QModelIndex const &/*parent*/)
	{
		if(ContentModule *ct = contentModule().data())
			return ct->clearRows(row, gap_count);
		clearRowsImpl(row, gap_count);
		return true;
	}

	bool RowStoreModel::insertRowsRaw(int row, QVector<QStringList> const &raw_rows, QModelIndex const &/*parent*/)
	{
		if(ContentModule *ct = contentModule().data())
			return ct->insertRows(row, RowStore(raw_rows));
		insertRowsImpl(row, RowStore(raw_rows));
		return true;
	}
	bool RowStoreModel::replaceRowsRaw(int row, QVector<QStringList> const &raw_rows, QModelIndex const &/*parent*/)
	{
		if(ContentModule *ct = contentModule().data())
			return ct->replaceRows(row, RowStore(raw_rows));
		replaceRowsImpl(row, RowStore(raw_rows));
		return true;
	}
	bool RowStoreModel::transitionRowsRaw
			( int row, int count, QVector<QStringList> const &raw_rows, bool smooth_transition, QModelIndex const &)
	{
		RowStore rows(raw_rows);
		if(ContentModule *ct = contentModule().data())
			return ct->transitionRows(row, count, rows, smooth_transition);
		transitionRowsImpl(row, count, rows, smooth_transition);
		return true;
	}



	bool RowStoreModel::assignModelImpl(RowStoreModel const &source, bool smooth_transition)
	{
		ContentModulePtr old_module_ = contentModule();
		if(smooth_transition)
			smooth_transition =
				SmoothTransitionModule(this, source.contentModule())
					.initialStackShare<SmoothTransitionModule>()
					->migrate(0, row_store.size(), source.row_store);

		if(!smooth_transition && !hardTransitionImpl(0, row_store.size(), source.row_store))
			return false;

		changeContentModule(source.contentModule()
				? source.contentModule()->copy()
				: ContentModulePtr());

		graph_ = source.graph_;
		setColumnInformation(source.columns_);
		if(old_module_)
			old_module_->detached(*this);
		return true;
	}
	bool ContentModule::assignModel(RowStoreModel const &source, bool smooth_transition)
	{
		RowStoreModel *model = this->model();
		if(!onChange(0, model->rowCount())) return false;
		return model->assignModelImpl(source, smooth_transition);
	}
	bool RowStoreModel::assignModel(LiveNodeModel const &model, bool smooth_transition)
	{
		if(RowStoreModel const *modeld = dynamic_cast<RowStoreModel const *>(&model))
		{
			if(ContentModule *ct = contentModule().data())
				return ct->assignModel(*modeld, smooth_transition);
			assignModelImpl(*modeld, smooth_transition);
		}
		return false;
	}

	LiveNodeModelPtr RowStoreModel::subModelImpl
			( RDFStrategyFlags strategy
			, QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive
			, QList<int> const &columns_mapping)
	{
		return QSharedCreator<SubRowStoreModel>::create
				( sharedFromThis<RowStoreModel>()
				, strategy & ~RDFStrategy::Writable
				, topleft_inclusive, bottomright_exclusive
				, QModelIndex()
				, columns_mapping);
	}
	LiveNodeModelPtr ContentModule::subModel
			( QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive
			, QList<int> const &columns_mapping)
	{
		return model()->subModelImpl
				(model()->strategy(topleft_inclusive), topleft_inclusive, bottomright_exclusive, columns_mapping);
	}
	LiveNodeModelPtr RowStoreModel::subModel
			( QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive
			, QList<int> const &columns_mapping)
	{
		QModelIndex tl(topleft_inclusive), br(bottomright_exclusive);
		if (columns_mapping.size()) {
			tl = tl.sibling(tl.row(), 0);
			br = br.sibling(br.row(), 0);
		}
		if(tl.row() > br.row())
			warning(this) << "topleft row greater than bottomright row:" << tl;
		else if(tl.column() > br.column())
			warning(this) << "topleft column greater than bottomright column:" << tl;
		else if(tl.column() < 0)
			warning(this) << "topleft.column(" << tl.column() << ") < 0";
		else if(br.column() > columns_.visibleColumnCount())
			warning(this) << "bottomright.column(" << br.column()
						  << ") > column count(" << columns_.visibleColumnCount() << ")";
		else
		{
			if(ContentModule *ct = contentModule().data())
				return ct->subModel(tl, br, columns_mapping);
			// TODO: frak this, figure out const correctness policy
			return const_cast<RowStoreModel *>(this)->subModelImpl
					( strategy(topleft_inclusive), tl, br, columns_mapping);
		}
		return LiveNodeModelPtr();
	}

	QStringList RowStoreModel::rawRow(int row, QModelIndex const &parent) const
	{
		Q_UNUSED(parent);
		RowStore::const_iterator ci = row_store.find(row);
		return ci != row_store.end() ? *ci : QStringList();
	}

	ContentModule_Simple::ContentModule_Simple(ChangePolicy change_policy)
		: ContentModule(change_policy), update_pending_(false)
	{}
	ContentModule_Simple::ContentModule_Simple(ContentModule_Simple const &cp)
		: ContentModule(cp), update_pending_(false)
	{}

	ContentModule_Simple::ModulePtr ContentModule_Simple::attach(RowStoreModel &model)
	{
		// skip ContentModule default attach
		return Base::attach(model);
	}

	RowStore::iterator ContentModule_Simple::createRow(unsigned row)
	{
		model()->refreshRows(row, 1, QModelIndex()
				, LiveNodeModel::Block | LiveNodeModel::CoverDerivedProperties);
		return rowCache().find(row);
	}

	bool ContentModule_Simple::updatePending()
	{
		return update_pending_;
	}
	void ContentModule_Simple::setUpdatePending(bool pending)
	{
		update_pending_ = pending;
	}
	bool ContentModule_Simple::running()
	{
		return (model() ? (model()->strategy_ & RDFStrategy::Running) : false);
	}
	bool ContentModule_Simple::setRunning(bool running)
	{
		return model()
			? model()->doAlterStrategy
					(running ? RDFStrategy::Running : RDFStrategyFlags(0), RDFStrategy::Running)
			: false;
	}

}
