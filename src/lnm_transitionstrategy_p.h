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
 * lnm_transitionstrategy_p.h
 *
 *  Created on: May 12, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_LNM_TRANSITIONSTRATEGY_P_H_
#define SOPRANOLIVE_LNM_TRANSITIONSTRATEGY_P_H_

#include "triplemodel_p.h"

namespace SopranoLive
{
/*
	class DelegatorModule
		: public ContentModule
	{
		struct DelegateInfo
		{
			RowStore *rows_;
			ContentModulePtr module_;
			int model_to_module_offset_;

			int modelToModuleRow(int model_row) { return model_row + model_to_module_offset_; }
			int moduleToModelRow(int module_row) { return module_row - model_to_module_offset_; }
		};
		typedef int UpperBoundOfDelegate;
		typedef QMap<UpperBoundOfDelegate, DelegateInfo> DelegateMap;
		DelegateMap delegates_;
		void addDelegate(RowStore *rows, ContentModulePtr const &module)

		QVariant data(QModelIndex const &index, int role, int &role_flags
				, RowStore::const_iterator const &irow) const;
	};
*/
	class SmoothTransitionModule
		 : public ContentModule
	{
		/* VZR_CLASS(SmoothTransitionModule, (ContentModule));*/

	public:
		RowStore old_row_store_; // VZR_MEMBER

		RowStore new_row_store_; // VZR_MEMBER

		ContentModulePtr old_module_; // VZR_MEMBER
		ContentModulePtr new_module_; // VZR_MEMBER


		RowStoreRange old_rows_; // VZR_MEMBER
		int start_row_; // VZR_MEMBER



		RDFSelect::OrderByInfo identity_columns_; // VZR_MEMBER

		bool finalization_ready_; // VZR_MEMBER
		bool commit_; // VZR_MEMBER

		bool no_delegation_; // VZR_MEMBER

		// migration time variables
		RowStoreRange new_rows_; // VZR_MEMBER

		int to_new_mid_offset_; // VZR_MEMBER
		int to_old_upper_offset_; // VZR_MEMBER

		int count_; // VZR_MEMBER

		struct SimpleRange
		{
			RowStore::const_iterator row_;
			int count_;
			SimpleRange() : row_(), count_(0) {}

			friend QDebug operator<<(QDebug s, SimpleRange const &range)
			{
				return s.nospace() << "Range(" << range.row_.key() << ", " << range.count_ << ")";
			}
		};

		typedef QHash<QVector<QString>, SimpleRange> Lookup;
		Lookup lookup; // VZR_MEMBER


		unsigned current_target_row_; // VZR_MEMBER
		unsigned current_source_row_; // VZR_MEMBER

		unsigned equal_source_row_; // VZR_MEMBER
		unsigned equal_target_row_; // VZR_MEMBER

		unsigned changed_target_row_; // VZR_MEMBER
		unsigned changed_source_row_; // VZR_MEMBER


		RowStore::iterator ti_; // VZR_MEMBER
		RowStore::iterator tiend_; // VZR_MEMBER


		RowStore::const_iterator si_; // VZR_MEMBER
		RowStore::const_iterator siend_; // VZR_MEMBER


		RowStore::iterator createRow(int row);

		void update();
		QVector<QString> identityVector(RowStore::value_type const &row);
		bool prepareLookup();
		int rowIdentityCompare(QStringList const &lhs, QStringList const &rhs);
		typedef QVector<bool> ChangedColumns;
		int firstRowDataChangedCompare
				( QStringList const &lhs, QStringList const &rhs
				, ChangedColumns *changed_columns);
		bool continuousRowDataChangedCheck
				( QStringList const &lhs, QStringList const &rhs
				, ChangedColumns *changed_columns);
		RowStore::const_iterator iterateTargetToIdentitySource(RowStore::iterator &ti);
		bool verifyRowAdvancement(bool return_true_on_gap);

		bool migrate(int start_row, int count, ConstRowStoreRange const &new_row_range);

		typedef int UpdatedColumn;
		typedef QVector<QPair<QSet<QString>, UpdatedColumn> > UpdateKeys;

		bool isUpdatedRow(QStringList const &row_data, UpdateKeys const &update_keys);
		bool update
			( int start_row, int count, ConstRowStoreRange const &update_row_range
			, UpdateKeys const &update_keys);
		bool finalize();

		~SmoothTransitionModule();

		SmoothTransitionModule
			( RowStoreModel *model
			, ContentModulePtr new_module = ContentModulePtr()
			, RDFSelect::OrderByInfo const &identity_columns = (RDFSelect::OrderByInfo() << qMakePair(0, true)));
	};
}

#endif /* SOPRANOLIVE_LNM_TRANSITIONSTRATEGY_P_H_ */
