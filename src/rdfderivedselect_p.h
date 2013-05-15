/*
 * This file is part of libqttracker project
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
 * rdfderivedselect.h
 *
 *  Created on: Nov 29, 2009
 *      Author: "Iridian Kiiskinen"
 */

#ifndef SOPRANOLIVE_RDFDERIVEDSELECT_P_H_
#define SOPRANOLIVE_RDFDERIVEDSELECT_P_H_

#include "include/sopranolive/unbound/rdfderivedselect.h"
#include "rdfunbound_p.h"
#include "rawdatatypes_p.h"

namespace SopranoLive
{
	struct RDFDerivedColumn::Private
		: RDFSelectColumn::Data
	{
		typedef RDFDerivedColumn Public;

		static Private const *d_func(Public const &pub) { return pub.d_func(); }
		static Private *d_func(Public &pub) { return pub.d_func(); }
		static Private const *cd_func(Public const &pub) { return d_func(pub); }

		static SharedNull<Private> shared_null;

		Private *clone() const { return new Private(*this); }

		Private(short index = NullIndex, short parent = NullIndex
				, RDFProperty const &column_data = RDFProperty())
			: RDFSelectColumn::Data(Normal, column_data.target(), column_data.target())
			, index_(index)
			, derived_column_count_(0), parent_(parent), property_data_(column_data)
			, column_strategy_()
		{
	#ifdef QRDFUNBOUND_DEBUG
			debug(4) << "RDFDerivedColumn::Private(" << this << ") created: "
				"index(" << index_ << "), parent(" << parent_ << ")";
	#endif
		}

		Private(short index, short parent, RDFSelectColumn const &select_column)
			: RDFSelectColumn::Data(*select_column.d), index_(index)
			, derived_column_count_(0), parent_(parent)
			, property_data_(RDFProperty::fromVariable(select_column.variable()))
			, column_strategy_()
		{
	#ifdef QRDFUNBOUND_DEBUG
			debug(4) << "RDFDerivedColumn::Private(" << this << ") created: "
					"index(" << index_ << "), parent(" << parent_ << ")";
	#endif
		}

		~Private()
		{
	#ifdef QRDFUNBOUND_DEBUG
			debug(4) << "RDFDerivedColumn::Private(" << this << ") destroyed: "
					"index(" << index_ << "), parent(" << parent_ << ")";
	#endif
		}

		static RDFDerivedColumn create
				( RDFProperty const &column_data = RDFProperty()
				, short index = RDFDerivedColumn::NullIndex
				, short parent = RDFDerivedColumn::NullIndex);
		static RDFDerivedColumn create
				( RDFSelectColumn const &select_column
				, short index = RDFDerivedColumn::NullIndex
				, short parent = RDFDerivedColumn::NullIndex);

		Private *deepCopy(RDFVariable::DeepCopier const &copier) const
		{
			Private *copy = new Private(NullIndex, 0, property_data_.deepCopy(copier));
			copy->deriveds_ = deriveds_;
			return copy;
		}


		short index_, derived_column_count_, parent_;
		RDFProperty property_data_;
		RDFStrategyFlags column_strategy_;

		// index from this column through a property data into a child column index
		typedef QHash<RawPropertyDataWithEqualityMask<RawEqualityMask>, int> Deriveds;

		Deriveds deriveds_;

		int derivedColumnCount() const;
		void setDerivedColumnCount(int count);
		void addToDerivedColumnCount(int offset);

		Deriveds deriveds() const;
		Deriveds &refDeriveds();
	};

	struct RDFDerivedSelect::Private
		: RDFSelect::Data
	{
		typedef RDFDerivedSelect Public;

		static Private const *d_func(Public const &pub) { return pub.d_func(); }
		static Private *d_func(Public &pub) { return pub.d_func(); }
		static Private const *cd_func(Public const &pub) { return d_func(pub); }

		static SharedNull<Private> shared_null;

		Private *clone() const { return new Private(*this); }

	protected:
		Private()
			: RDFSelect::Data(), base_columns_(0), real_columns_(0), visible_columns_(0) {}
	public:
		Private(RDFSelect::Data const &select)
			: RDFSelect::Data(select), base_columns_(0), real_columns_(0), visible_columns_(0) {}

		RDFDerivedColumnList &refColumns();
		RDFDerivedColumnList const &columns() const;

		RDFDerivedColumn &addColumn
				( RDFDerivedColumn &full_info, RDFStrategyFlags type = 0
				, int parent = RDFDerivedColumn::NullIndex);

		virtual int processDerivedColumns
				( int index, bool expand, bool join, bool flat, int primaries);

		struct Builder
			: RDFSelect::Data::Builder
		{
			Builder(Private const &p);
			Private const &query() const { return static_cast<Private const &>(this->query()); }

			RDFSelectColumnList buildSelectColumnRange() const
			{ return query().columns_.mid(0, query().real_columns_); }

			bool processSelectColumn(RDFSelectColumn &column) const;
		};

		QSharedPointer<RDFQuery::Data::Builder> createSelectBuilder() const;

		virtual RDFDerivedColumn &joinColumn(RDFDerivedColumn &column, bool flat = Linked) const;

		void joinColumnVariables(bool flat = false);

		short base_columns_, real_columns_, visible_columns_;

		friend class RDFDerivedSelect;
	};
}

#endif /* SOPRANOLIVE_RDFDERIVEDSELECT_P_H_ */
