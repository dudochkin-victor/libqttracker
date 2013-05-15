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
 *  Created on: May 9, 2010
 *      Author: "Iridian Kiiskinen"
 */

#ifndef SOPRANOLIVE_UNBOUND_RDFDERIVEDSELECT_H_
#define SOPRANOLIVE_UNBOUND_RDFDERIVEDSELECT_H_

#include "../rdfunbound.h"

namespace SopranoLive
{
	class RDFDerivedSelect;

	class RDFDerivedColumn
		: public RDFSelectColumn
	{
	public:
		struct Private;
		enum DerivedType
		{ 	NullIndex = -1
		,	BaseColumn
				= (RDFStrategyFlags(1) << RDFStrategy::offset_ContextSpecificFlags)
		,	DerivedRealColumn
				= (RDFStrategyFlags(1) << (RDFStrategy::offset_ContextSpecificFlags + 1))
		,	DerivedVirtualColumn
				= (RDFStrategyFlags(1) << (RDFStrategy::offset_ContextSpecificFlags + 2))
				//! HiddenColumn is a column containing a query variable which isn't shown to user
		,	HiddenColumn
				= (RDFStrategyFlags(1) << (RDFStrategy::offset_ContextSpecificFlags + 3))
		, 	ModelCachePending
				= (RDFStrategyFlags(1) << (RDFStrategy::offset_ContextSpecificFlags + 4))
		, 	CacheExpanded
				= (RDFStrategyFlags(1) << (RDFStrategy::offset_ContextSpecificFlags + 5))
		,	PrimaryBaseColumn = BaseColumn
				| (RDFStrategyFlags(1) << (RDFStrategy::offset_ContextSpecificFlags + 6))
				//! MuteColumn is a place-holder column which should contain dummy values in the
				//! result set, and should not add or remove solutions
		,	MuteColumn
				= (RDFStrategyFlags(1) << (RDFStrategy::offset_ContextSpecificFlags + 7))
		};

		RDFDerivedColumn();
		RDFDerivedColumn(Private &d);
		RDFDerivedColumn(RDFSelectColumn const &cp);

		RDFDerivedColumn deepCopy
				( RDFVariable::DeepCopier const &copier = RDFVariable::DeepCopier()) const;

		void swap(RDFDerivedColumn &other);

		//! \return the column index
		int index() const;

		//! \return the parent column index if column is a derived column; -1 otherwise
		int parent() const;

		//! \return the property object linking the parent to this. If this column has no parent,
		//! the property is invalid
		RDFProperty propertyData() const;
		RDFProperty &refPropertyData();

		void setName(QString const &name);
		QString name() const;

		void setVariable(RDFVariable const &var);
		RDFVariable variable() const;

		RDFStrategyFlags strategy() const;
		RDFStrategyFlags columnStrategy() const;
		void setColumnStrategy(RDFStrategyFlags flags);
		void enableColumnStrategyFlags(RDFStrategyFlags flags);
		void disableColumnStrategyFlags(RDFStrategyFlags flags);

		void markColumnType(RDFStrategyFlags type);

		void markAsBaseColumn(bool is_identity_too = false);
		void markAsDerivedRealColumn();
		void markAsDerivedVirtualColumn();

	protected:
	    Private *d_func();
	    Private const *d_func() const;
	    friend class RDFDerivedSelect;
	};
	typedef RDFDerivedColumn::Private RDFDerivedColumnPrivate;
	typedef QList<RDFDerivedColumn> RDFDerivedColumnList;

	/*!
	 * Represents a selection query containing derived property dependency information.
	 */
	class RDFDerivedSelect
		: public RDFSelect
	{
	public:
		struct Private;
		RDFDerivedSelect();
		RDFDerivedSelect(RDFSelect const &copy);
		RDFDerivedSelect(Private *);

		RDFDerivedSelect &swap(RDFDerivedSelect &other);
		void clear();

		RDFDerivedSelect &load(RDFSelect const &select);

		int size() const;

		RDFDerivedSelect deepCopy
				( RDFVariable::DeepCopier const &copier = RDFVariable::DeepCopier()) const;

		//! \return a reference to an editable list of columns of this select
		RDFDerivedColumnList &refColumns();
		//! \return list of the columns of this select
		RDFDerivedColumnList const &columns() const;

		//! \return count of primary columns
		short baseColumnCount() const;

		//! \return count of real derived columns, typically single-valued properties
		short derivedRealColumnCount() const;

		//! \return count of virtual derived columns, typically multiple-valued properties
		short derivedVirtualColumnCount() const;

		//! \return count of hidden columns
		short hiddenColumnCount() const;

		//! \return count of all derived columns
		//! equal to derivedRealColumnCount() + derivedVirtualColumnCount()
		short derivedColumnCount() const;

		//! \return count of all real columns
		//! equal to baseColumnCount() + derivedRealColumnCount()
		short realColumnCount() const;

		//! \return count of all real columns
		//! equal to baseColumnCount() + derivedColumnCount()
		short visibleColumnCount() const;

		/*!
		 * Add a new column from the given \a property with given
		 * RDFDerivedColumn::Type \a type and given \a parent column
		 */
		RDFDerivedSelect &addColumn
				( RDFProperty const &property, RDFStrategyFlags type
				, int parent = RDFDerivedColumn::NullIndex);

		/*!
		 * Add a new column from the given \a column with given
		 * RDFDerivedColumn::Type \a type and given \a parent column
		 */
		RDFDerivedSelect &addColumn
				( RDFSelectColumn const &column
				, RDFStrategyFlags type = RDFDerivedColumn::BaseColumn
				, int parent = RDFDerivedColumn::NullIndex);

		/*!
		 * Add a new column from the given \a column_variable with given
		 * RDFDerivedColumn::Type \a type and given \a parent column
		 */
		RDFDerivedSelect &addColumn
				( RDFVariable const &column_variable = RDFVariable()
				, RDFStrategyFlags type = RDFDerivedColumn::BaseColumn
				, int parent = RDFDerivedColumn::NullIndex);

		/*!
		 * Create and add a column from the given \a name and given \a column_variable
		 * with given RDFDerivedColumn::Type \a type and given \a parent column
		 */
		RDFDerivedSelect &addColumn
				( QString const &name, RDFVariable const &column_variable
				, RDFStrategyFlags type = RDFDerivedColumn::BaseColumn
				, int parent = RDFDerivedColumn::NullIndex);

		/*!
		 * Create and add a column from the given \a name and given \a column_variable
		 * with given RDFDerivedColumn::Type \a type and given \a parent column
		 */
		RDFDerivedSelect &addColumns
				( RDFVariableList const &column_variables
				, RDFStrategyFlags type = RDFDerivedColumn::BaseColumn
				, int parent = RDFDerivedColumn::NullIndex);

		//! \return the derived column at the location given by \a index
		RDFDerivedColumn &operator[](int index);

		//! \return the derived column at the location given by \a index
		RDFDerivedColumn const &operator[](int index) const;

		enum { Linked, Flat };

		/*!
		 * Processes derived columns by iterating columns from given \a starting_index to the
		 * last real derived column in order, with following rules:
		 * 1. If given \a expand is true, for each column C_i, all derived properties are
		 *    expanded and added as new columns with the C_i as their parent column
		 * 1.1. Single-valued derived properties are added as new derived real columns in front of
		 *      existing derived virtual columns
		 * 1.2. Multiple-valued derived properties are added as new derived virtual columns
		 *      to the end
		 * 2. TODO: add docs describing order by/group by treatment
		 */
		int processDerivedColumns
				( int starting_index = 0, bool expand = true, bool join = true
				, bool flat = false, int primaries = -1);

	protected:
	    Private *d_func();
	    Private const *d_func() const;
	};
	typedef RDFDerivedSelect::Private RDFDerivedSelectPrivate;

}

QT_BEGIN_NAMESPACE

Q_INLINE_TEMPLATE void qSwap(SopranoLive::RDFDerivedColumn &a, SopranoLive::RDFDerivedColumn &b)
{ a.swap(b); }
Q_INLINE_TEMPLATE void qSwap(SopranoLive::RDFDerivedSelect &a, SopranoLive::RDFDerivedSelect &b)
{ a.swap(b); }

QT_END_NAMESPACE

#endif /* SOPRANOLIVE_UNBOUND_RDFDERIVEDSELECT_H_ */
