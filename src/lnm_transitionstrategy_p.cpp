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
 * lnm_transitionstrategy_p.cpp
 *
 *  Created on: May 12, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include <algorithm>
#include <limits>
#include "lnm_transitionstrategy_p.h"


uint qHash(QVector<QString> const &strvec)
{
	uint ret = 0;
	for(int i = 0, iend = strvec.size(); i < iend; ++i)
		ret ^= qHash(strvec[i]);
	return ret;
}

namespace SopranoLive
{
/*
	QVariant DelegatorModule::data(const QModelIndex &index, int role, int &role_flags
			, RowStore::const_iterator const &irow) const
	{
		DelegateLookup::const_iterator cdi = delegates_.lowerBound(row);
		return cdi->data
				( index.sibling(index.row() + cdi->modelToModuleRow(index.row()), index.column())
				, role, role_flags, irow);
	}
*/
		template<typename Ptr>
	class DataThief : public Ptr
	{
	public:
		void *stealData() const { return this->d; }
	};

	template<typename Ptr>
	void *stealData(Ptr const &ptr)
	{
		return static_cast<DataThief<Ptr> const &>(ptr).stealData();
	}

	SmoothTransitionModule::SmoothTransitionModule
			( RowStoreModel *rowstoremodel
			, ContentModulePtr new_module_
			, RDFSelect::OrderByInfo const &identity_columns_)
		: ContentModule()
		, old_row_store_(rowCache(*rowstoremodel))
		, old_module_(rowstoremodel->contentModule()), new_module_(new_module_)
		, old_rows_(old_row_store_)
		, start_row_(std::numeric_limits<int>::max())
		, identity_columns_(identity_columns_)
		, finalization_ready_(false)
		, no_delegation_(false)
	{
		compositor_ = rowstoremodel;
		lookup.reserve(64); // TODO: verify, this stetson-sleeve value
	}

	SmoothTransitionModule::~SmoothTransitionModule()
	{
		if(finalization_ready_ && new_module_)
			model()->changeContentModule(new_module_);
	}

	RowStore::iterator SmoothTransitionModule::createRow(int row)
	{
		if(no_delegation_)
			return old_module_->createRow(row);

		if(row < start_row_)
			return model()->createRowImpl(old_rows_.container(), old_module_.data(), row);
		unsigned source_row = row + to_new_mid_offset_;
		if(source_row < current_source_row_)
			return model()->createRowImpl
					(new_rows_.container(), new_module_.data(), source_row);
		return model()->createRowImpl
				(old_rows_.container(), old_module_.data(), row + to_old_upper_offset_);
	}

	void SmoothTransitionModule::update()
	{
		// TODO not fully thought out yet, is anything necessary here?
		return;
	}

	QVector<QString> SmoothTransitionModule::identityVector(RowStore::value_type const &row)
	{
		QVector<QString> ret;
		int identity_columns = identity_columns_.size();
		ret.resize(identity_columns);
		for(int i = 0; i < identity_columns; ++i)
			ret[i] = row[identity_columns_[i].first];
		return ret;
	}

	bool SmoothTransitionModule::prepareLookup()
	{
		RowStore::const_iterator rci = si_, rciend = siend_;
		rciend.skipGap();
		SimpleRange dummy;

		QVector<QString> active_index;
		SimpleRange *active_range = 0;

		while(rci.skipGap() != rciend)
		{
			for( ; rci != rciend && !rci.isGap(); ++rci)
			{
				QVector<QString> index = identityVector(*rci);
				if(index != active_index)
				{
					active_range = &lookup[index];
					if(active_range->count_)
						active_range = &dummy;
					active_range->row_ = rci;
				}
				++active_range->count_;
			}
			active_index.clear();
		}
		return true;
	}

	int SmoothTransitionModule::rowIdentityCompare(QStringList const &lhs, QStringList const &rhs)
	{
		for(QVector<QPair<int, bool> >::iterator i = identity_columns_.begin()
				, iend = identity_columns_.end()
				; i != iend; ++i)
			if(int diff = lhs[i->first].compare(rhs[i->first]))
				return i->second ? diff : -diff;
		return 0;
	}

	//! \return diff if the lines differ and sets all differing columns in changed_columns to true
	int SmoothTransitionModule::firstRowDataChangedCompare
			( QStringList const &lhs, QStringList const &rhs, ChangedColumns *changed_columns)
	{
		int ret = 0, diff = 0;
		int iend = std::min(lhs.size(), rhs.size());
		changed_columns->clear();
		changed_columns->resize(iend);
		for(int i = 0; i < iend; ++i)
		{
			(*changed_columns)[i] = (diff = lhs[i].compare(rhs[i]));
			if(diff && !ret)
				ret = diff;
		}
		return ret
				? ret
				: lhs.size() - rhs.size();
	}

	//! \return true if the way lhs differs from rhs is different than specified in changed_columns
	bool SmoothTransitionModule::continuousRowDataChangedCheck
			( QStringList const &lhs, QStringList const &rhs, ChangedColumns *changed_columns)
	{
		int iend = std::min(lhs.size(), rhs.size());
		if(changed_columns->size() != iend)
			return false;
		for(int i = 0; i < iend; ++i)
			if((lhs[i].compare(rhs[i]) != 0) != (*changed_columns)[i])
				return false;
		return true;
	}

	RowStore::const_iterator SmoothTransitionModule::iterateTargetToIdentitySource(RowStore::iterator &ti_)
	{
		for(; ti_.skipGap() != old_rows_.second; ++ti_)
		{
			Lookup::iterator iter = lookup.find(identityVector(*ti_));
			// debug(5) << "At old line" << ti_.key() << "looking for new line" << (*ti_)[identity_columns_[0].first];
			if(iter == lookup.end())
				continue;
			RowStore::const_iterator fsi = iter->row_;
			// debug(5) << "Found identity from source" << fsi.key() << "while current source is" << current_source_row_;

			int diff = current_source_row_ - fsi.key();
			if(diff < iter->count_)
			{
				if(diff > 0)
					fsi += diff;
				return fsi;
			}
		}
		return siend_;
	}

	bool SmoothTransitionModule::verifyRowAdvancement(bool return_true_on_gap)
	{
		if(ti_ == tiend_ || si_ == siend_)
			return false;

		if(!ti_.isGap() && !si_.isGap())
			return true;

		int ti_key = ti_.skipGap().key();
		int si_key = si_.skipGap().key();

		if(ti_key + to_new_mid_offset_ != si_key)
			return false;

		changed_target_row_ = ti_key;
		changed_source_row_ = si_key;

		return return_true_on_gap;
	}

	/*!
	 * Smoothly and fully migrates the given \a count of rows starting from given \a start_row
	 * to given \a new_row_range. The migration is done incrementally sequentially top to bottom,
	 * with proper rowsInserteed, dataChanged and rowsDeleted signals emitted during the process.
	 */
	bool SmoothTransitionModule::migrate
			( int start_row, int count, ConstRowStoreRange const &new_row_range)
	{
		if(finalization_ready_)
			return false;

		LiveNodeModelPtr model_keepalive = model()->sharedFromThis<LiveNodeModel>();

		/*
				localDebug() << "Migrating TM(" << model() << "):" << *model();
				localDebug() << "         ordering:" << identity_columns_;
				localDebug() << "         at: [" << start_row << "," << start_row + count << "[";
				localDebug() << "         with:" << new_row_range;
		 //*/

		start_row_ = start_row;
		no_delegation_ = false;
		old_rows_ = RowStoreRange(old_row_store_, start_row, count);
		new_row_store_.clear();
		new_row_store_.append(new_row_range.first, new_row_range.second);
		new_rows_ = new_row_store_;

		si_ = new_rows_.begin();
		siend_ = new_rows_.end();
		ti_ = old_rows_.begin();
		tiend_ = old_rows_.end();

		if(!prepareLookup())
			return false;

		model()->setCache(RowStore(old_row_store_.size()));
		old_module_->rows_ = &old_row_store_;
		*model()->content_module_handle_ = sharedFromThis();

		to_new_mid_offset_ = new_rows_.first.key() - start_row;
		to_old_upper_offset_ = 0;

		current_target_row_ = start_row;
		current_source_row_ = new_rows_.first.key();
		equal_source_row_ = start_row;
		equal_target_row_ = current_source_row_;
		changed_target_row_ = start_row;
		changed_source_row_ = current_source_row_;

		QVector<bool> update_mask;

		while(ti_ != tiend_)
		{
			bool row_by_row_advancing = verifyRowAdvancement(false);
			while(row_by_row_advancing)
			{
				equal_source_row_ = current_source_row_;
				equal_target_row_ = current_target_row_;

				row_by_row_advancing = verifyRowAdvancement(false);

				while(row_by_row_advancing && (*ti_ == *si_))
				{
					++equal_source_row_, ++equal_target_row_, ++ti_, ++si_;
					row_by_row_advancing = verifyRowAdvancement(false);
				}

				if(changed_target_row_ > equal_target_row_)
					// set by verifyRowAdvancement
					row_by_row_advancing = true;
				else
				{
					changed_source_row_ = equal_source_row_;
					changed_target_row_ = equal_target_row_;
				}

				current_source_row_ = changed_source_row_;
				current_target_row_ = changed_target_row_;

				update_mask.clear();
				if(row_by_row_advancing
						&& !rowIdentityCompare(*ti_, *si_)
						&& firstRowDataChangedCompare(*ti_, *si_, &update_mask))
				{
					do
						++changed_source_row_, ++changed_target_row_, ++ti_, ++si_;
					while((row_by_row_advancing = verifyRowAdvancement(true))
							&& continuousRowDataChangedCheck(*ti_, *si_, &update_mask));

					for(int i = 0, j = 0, jiend = update_mask.size(); i < jiend; i = j)
					{
						for( ; i < jiend && !update_mask[i]; ++i)
							;
						j = i;
						for( ; j < jiend && update_mask[j]; ++j);
						if(j > i) Q_EMIT model()->dataChanged
								( model()->index
										( current_source_row_ - to_new_mid_offset_, i)
								, model()->index
										( changed_source_row_ - to_new_mid_offset_ - 1, j - 1));
					}
					current_source_row_ = changed_source_row_;
					current_target_row_ = changed_target_row_;
				} else
					break;
			}

			si_ = iterateTargetToIdentitySource(ti_);
			if(ti_ == tiend_)
			{
				changed_target_row_ = old_rows_.begin().key() + old_rows_.size();
				changed_source_row_ = new_rows_.size();
			} else
			{
				changed_target_row_ = ti_.key();
				changed_source_row_ = si_.key();
			}

			if(int removed_rows = changed_target_row_ - current_target_row_)
			{
				model()->beginRemoveRows(QModelIndex()
						, current_source_row_ - to_new_mid_offset_
						, current_source_row_ - to_new_mid_offset_ + removed_rows - 1);

				current_target_row_ = changed_target_row_;

				to_old_upper_offset_ += removed_rows;
				model()->row_store.resize(old_row_store_.size() - to_old_upper_offset_);
				model()->endRemoveRows();
			}

			if(int inserted_rows = changed_source_row_ - current_source_row_)
			{
				model()->beginInsertRows(QModelIndex()
						, current_source_row_ - to_new_mid_offset_
						, changed_source_row_ - to_new_mid_offset_ - 1);
				current_source_row_ = changed_source_row_;
				to_old_upper_offset_ -= inserted_rows;
				model()->row_store.resize(old_row_store_.size() - to_old_upper_offset_);
				model()->endInsertRows();
			}

		}

		old_row_store_.remove(start_row, count);
		old_row_store_.insert(start_row, new_rows_.begin(), new_rows_.end());

		*model()->content_module_handle_ = old_module_;
		model()->setCache(old_row_store_);

		return finalization_ready_ = true;
	}

	bool SmoothTransitionModule::isUpdatedRow
			( QStringList const &row_data, UpdateKeys const &update_keys)
	{
		for(UpdateKeys::const_iterator cuki = update_keys.begin(), cukiend = update_keys.end()
				; cuki != cukiend; ++cuki)
			if(cuki->first.contains(row_data[cuki->second]))
				return true;
		return false;
	}

	bool SmoothTransitionModule::update
			( int start_row, int count, ConstRowStoreRange const &update_row_range
			, UpdateKeys const &update_keys)
	{
		if(finalization_ready_)
			return false;

		LiveNodeModelPtr model_keepalive = model()->sharedFromThis<LiveNodeModel>();

/*
		debug(5) << "Updating TM(" << model() << "):" << *model()
		<< "         ordering:" << identity_columns_
		<< "         keys:" << update_keys
		<< "         at: [" << start_row << "," << start_row + count << "["
		<< "         with:" << update_row_range;
//*/
		// do not set start_row. All access will be forwarded to old_row_store_
		no_delegation_ = true;
		new_row_store_.clear();
		new_row_store_.append(update_row_range.first, update_row_range.second);
		new_rows_ = new_row_store_;

		RowStore &target_store = rowCache(*model());

		old_rows_ = RowStoreRange(target_store, start_row, count);

		si_ = new_rows_.begin();
		siend_ = new_rows_.end();
		ti_ = old_rows_.begin();
		tiend_ = old_rows_.end();

		if(!prepareLookup())
			return false;

		*model()->content_module_handle_ = sharedFromThis();

		current_target_row_ = start_row;
		current_source_row_ = new_rows_.first.key();
		equal_source_row_ = start_row;
		equal_target_row_ = current_source_row_;
		changed_target_row_ = start_row;
		changed_source_row_ = current_source_row_;

		QVector<bool> update_mask;

		do
		{
			bool row_by_row_advancing = verifyRowAdvancement(false);
			while(row_by_row_advancing)
			{
				equal_target_row_ = current_target_row_;
				equal_source_row_ = current_source_row_;

				row_by_row_advancing = verifyRowAdvancement(false);

				while(row_by_row_advancing && (*ti_ == *si_))
				{
					++equal_source_row_, ++equal_target_row_, ++ti_, ++si_;
					row_by_row_advancing = verifyRowAdvancement(false);
				}

				if(changed_target_row_ > equal_target_row_)
					// set by verifyRowAdvancement
					row_by_row_advancing = true;
				else
				{
					current_source_row_ = changed_source_row_ = equal_source_row_;
					current_target_row_ = changed_target_row_ = equal_target_row_;
				}

				if(row_by_row_advancing
						&& !rowIdentityCompare(*ti_, *si_)
						&& firstRowDataChangedCompare(*ti_, *si_, &update_mask))
				{
					bool block_boundary_crossed;
					do
					{
						RowStore::iterator tci_ = ti_;
						block_boundary_crossed = false;
						do
						{
							*ti_ = *si_;
							++changed_source_row_, ++changed_target_row_, ++si_;
							block_boundary_crossed = !ti_.blockAdvance(1);
						}while(!block_boundary_crossed
								&& si_ != siend_
								&& continuousRowDataChangedCheck(*ti_, *si_, &update_mask));
						// Clears the roles of updated cells marked with ClearRoleOnChange

						// the rows differ. Iterate all updated columns
						for(int col = 0, cend = update_mask.size(); col < cend; ++col)
							// do the column primary values differ?
							if(update_mask[col])
								// iterate all the roles for this column
								for(RowStoreBlock::DataRoles::const_iterator
											  cri = tci_.block().rolesBegin(col)
											, criend = tci_.block().rolesEnd(col)
										; cri != criend; ++cri)
								{
									RowStoreModel::RoleStates::const_iterator crdi
											= model()->default_role_states_.find(cri.key());
									// check if we should reset this role
									if(crdi != model()->default_role_states_.end()
											&& *crdi & RowStoreModel::ClearRoleOnChange)
										tci_.block().clearDataRoleRowRange
												( tci_.blockIndex()
												, block_boundary_crossed
														? tci_.block().size()
														: ti_.blockIndex()
												, col
												, cri.key() & 0xFFFFFFFF);

								}

					} while(block_boundary_crossed
							// if block boundary was not crossed, we are definitely both
							// row_by_row_advancing and not continuous row check advancing
							&& (row_by_row_advancing = verifyRowAdvancement(true))
							&& continuousRowDataChangedCheck(*ti_, *si_, &update_mask));

					for(int i = 0, j = 0, jiend = update_mask.size(); i < jiend; i = j)
					{
						for( ; i < jiend && !update_mask[i]; ++i)
							;
						j = i;
						for( ; j < jiend && update_mask[j]; ++j);
						if(i < j) Q_EMIT model()->dataChanged
								( model()->index( current_target_row_, i)
								, model()->index( changed_target_row_ - 1, j - 1));
					}
					current_source_row_ = changed_source_row_;
					current_target_row_ = changed_target_row_;
				} else
					break;
			}

			si_ = iterateTargetToIdentitySource(ti_);
			if(ti_ == tiend_)
			{
				changed_target_row_ = old_rows_.begin().key() + old_rows_.size();
				changed_source_row_ = new_rows_.size();
			} else
			{
				changed_target_row_ = ti_.key();
				changed_source_row_ = si_.key();
			}

			int removed_rows = changed_target_row_ - current_target_row_;
			int inserted_rows = changed_source_row_ - current_source_row_;

			ti_ -= removed_rows;
			si_ -= inserted_rows;

			while(removed_rows || inserted_rows)
			{
				while(removed_rows)
				{
					bool at_end_of_source = (si_ == siend_);
					// as we're just updating target, dont remove rows that are not in the update list
					while(true)
					{
						if(!removed_rows || (!at_end_of_source && rowIdentityCompare(*ti_, *si_) > 0))
							goto STM_update_double_break_1;
						if(isUpdatedRow(*ti_, update_keys))
							break;
						--removed_rows, ++ti_;
						++current_target_row_;
					}

					int actual_removes = removed_rows;

					// if we get here, there's always going to be at least one actual remove
					do
						--removed_rows, ++ti_;
					while(removed_rows
							&& (at_end_of_source || rowIdentityCompare(*ti_, *si_) <= 0)
							&& isUpdatedRow(*ti_, update_keys));

					actual_removes -= removed_rows;

					int tiend_key = tiend_.key() - actual_removes;

					model()->beginRemoveRows(QModelIndex()
							, current_target_row_
							, current_target_row_ + actual_removes - 1);

					target_store.remove(current_target_row_, actual_removes);

					changed_target_row_ -= actual_removes;

					ti_ = target_store.find(current_target_row_);
					tiend_ = target_store.find(tiend_key);

					model()->endRemoveRows();
				}

STM_update_double_break_1:

				if(inserted_rows)
				{
					bool at_end_of_target = (ti_ == tiend_);

					int current_inserts = inserted_rows;

					do
						--inserted_rows, ++si_;
					while(inserted_rows
							&& (at_end_of_target || rowIdentityCompare(*ti_, *si_) > 0));

					current_inserts -= inserted_rows;

					int tiend_key = tiend_.key() + current_inserts;

					model()->beginInsertRows(QModelIndex()
							, current_target_row_
							, current_target_row_ + current_inserts - 1);

					target_store.insert(current_target_row_, si_ - current_inserts, si_);

					current_source_row_ += current_inserts;
					current_target_row_ += current_inserts;

					ti_ = target_store.find(current_target_row_);
					tiend_ = target_store.find(tiend_key);

					model()->endInsertRows();
				}
			}
		} while(ti_ != tiend_);


		*model()->content_module_handle_ = old_module_;

		/*
				localDebug() << "Updated TM(" << model() << "):" << *model();
		 //*/

		return finalization_ready_ = true;
	}

	bool SmoothTransitionModule::finalize()
	{
		return finalization_ready_;
	}
}
