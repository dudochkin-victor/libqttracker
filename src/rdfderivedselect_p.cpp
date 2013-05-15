/*
 * rdfderivedselect_p.cpp
 *
 *  Created on: Dec 2, 2009
 *      Author: "Iridian Kiiskinen"
 */

#include "rdfderivedselect_p.h"
#include "rdfunbound_p.h"
#include "util_p.h"

namespace SopranoLive
{
	SharedNull<RDFDerivedColumn::Private> RDFDerivedColumn::Private::shared_null;

	RDFDerivedColumn::RDFDerivedColumn()
		: RDFSelectColumn(Private::shared_null)
	{};

	RDFDerivedColumn::RDFDerivedColumn(Private &d)
		: RDFSelectColumn(d)
	{}

	RDFDerivedColumn::RDFDerivedColumn(RDFSelectColumn const &cp)
		: RDFSelectColumn(cp)
	{
		if(!dynamic_cast<Private const *>(cp.d.data()))
			d = &Private::shared_null;
	}

	RDFDerivedColumn RDFDerivedColumn::Private::create
			( RDFProperty const &column_data, short index, short parent)
	{
		return RDFDerivedColumn(*new Private(index, parent, column_data));
	}

	RDFDerivedColumn RDFDerivedColumn::Private::create
			( RDFSelectColumn const &select_column, short index, short parent)
	{
		return RDFDerivedColumn(*new Private(index, parent, select_column));
	}

	RDFDerivedColumn RDFDerivedColumn::deepCopy(RDFVariable::DeepCopier const &copier) const
	{ return *d_func()->deepCopy(copier); }

	void RDFDerivedColumn::swap(RDFDerivedColumn &other) { qSwap(d, other.d); }

	int RDFDerivedColumn::index() const { return d_func()->index_; }

	int RDFDerivedColumn::parent() const { return d_func()->parent_; }

	RDFProperty RDFDerivedColumn::propertyData() const { return d_func()->property_data_; }
	RDFProperty &RDFDerivedColumn::refPropertyData() { return d_func()->property_data_; }

	void RDFDerivedColumn::setName(QString const &name)
	{ d_func()->outer_variable_.metaSetIdentifier(name); }
	QString RDFDerivedColumn::name() const
	{ return d_func()->outer_variable_.metaIdentifier(); }

	void RDFDerivedColumn::setVariable(RDFVariable const &var)
	{ d_func()->var_.metaAssign(var); }
	RDFVariable RDFDerivedColumn::variable() const
	{ return d_func()->var_; }

	RDFStrategyFlags RDFDerivedColumn::strategy() const
	{
		return d_func()->column_strategy_
				| (d_func()->var_.metaStrategy() & ~RDFStrategy::ContextSpecificMask);
	}
	RDFStrategyFlags RDFDerivedColumn::columnStrategy() const
	{ return d_func()->column_strategy_; }
	void RDFDerivedColumn::setColumnStrategy(RDFStrategyFlags flags)
	{ d_func()->column_strategy_ = flags; }
	void RDFDerivedColumn::enableColumnStrategyFlags(RDFStrategyFlags flags)
	{ d_func()->column_strategy_ |= flags; }
	void RDFDerivedColumn::disableColumnStrategyFlags(RDFStrategyFlags flags)
	{ d_func()->column_strategy_ &= ~flags; }

	void RDFDerivedColumn::markColumnType(RDFStrategyFlags type)
	{
		if(type & RDFDerivedColumn::DerivedVirtualColumn)
			type |= RDFStrategy::ChildedColumn;
		enableColumnStrategyFlags(type);
	}

	void RDFDerivedColumn::markAsBaseColumn(bool is_identity_too)
	{
		enableColumnStrategyFlags(RDFDerivedColumn::BaseColumn
						| (is_identity_too
								? RDFStrategy::IdentityColumn
								: RDFStrategyFlags(0)));
	}
	void RDFDerivedColumn::markAsDerivedRealColumn()
	{
		enableColumnStrategyFlags(RDFDerivedColumn::DerivedRealColumn);
	}
	void RDFDerivedColumn::markAsDerivedVirtualColumn()
	{
		enableColumnStrategyFlags(RDFDerivedColumn::DerivedVirtualColumn
						| RDFStrategy::ChildedColumn
						| RDFDerivedColumn::MuteColumn);
	}

	RDFDerivedColumnPrivate::Deriveds &RDFDerivedColumnPrivate::refDeriveds()
	{
		return deriveds_;
	}
	RDFDerivedColumnPrivate::Deriveds RDFDerivedColumnPrivate::deriveds() const
	{
		return deriveds_;
	}

	int RDFDerivedColumnPrivate::derivedColumnCount() const
	{
		return derived_column_count_;
	}
	void RDFDerivedColumnPrivate::setDerivedColumnCount(int count)
	{
		derived_column_count_ = count;
	}
	void RDFDerivedColumnPrivate::addToDerivedColumnCount(int count)
	{
		derived_column_count_ += count;
	}


    RDFDerivedColumn::Private* RDFDerivedColumn::d_func()
    { return static_cast<Private *>(d.data()); }

    const RDFDerivedColumn::Private* RDFDerivedColumn::d_func() const
    { return static_cast<Private const *>(d.data()); }

	SharedNull<RDFDerivedSelect::Private> RDFDerivedSelect::Private::shared_null;

	RDFDerivedSelect::RDFDerivedSelect()
		: RDFSelect(&Private::shared_null)
	{}

	RDFDerivedSelect::RDFDerivedSelect(RDFSelect const &copy)
		: RDFSelect(&Private::shared_null)
	{
		load(copy);
	}

	RDFDerivedSelect::RDFDerivedSelect(Private *priv)
		: RDFSelect(priv)
	{}


	RDFDerivedSelect &RDFDerivedSelect::swap(RDFDerivedSelect &other)
	{ qSwap(d, other.d); return *this; }

	void RDFDerivedSelect::clear() { d = &Private::shared_null; }

	RDFDerivedSelect &RDFDerivedSelect::load(RDFSelect const &select)
	{
		*d_func() = *select.d_func();
		RDFSelectColumnList &cols = RDFSelect::columns();
		cols.clear();

		for(RDFSelectColumnList::const_iterator ci = select.columns().begin()
					, ciend = select.columns().end()
				; ci != ciend; ++ci)
			addColumn(*ci);
		return *this;
	}

	int RDFDerivedSelect::size() const { return d_func()->columns_.size(); }

	RDFDerivedSelect RDFDerivedSelect::deepCopy(RDFVariable::DeepCopier const &copier) const
	{
		RDFDerivedSelect ret(d_func()->clone());
		ret.d_func()->columns_.clear();
		RDFDerivedColumnList const &cols = columns();
		for(RDFDerivedColumnList::const_iterator ci = cols.begin(), ciend = cols.end()
				; ci != ciend; ++ci)
			ret.addColumn(ci->deepCopy(copier));
		return ret;
	}

	RDFDerivedColumnList &RDFDerivedSelect::Private::refColumns()
	{
		// XXX TODO FIXME: ugly ugly hack
		return *reinterpret_cast<RDFDerivedColumnList *>(&columns_);
	}
	RDFDerivedColumnList &RDFDerivedSelect::refColumns() { return d_func()->refColumns(); }
	RDFDerivedColumnList const &RDFDerivedSelect::Private::columns() const
	{
		// XXX TODO FIXME: ugly ugly hack
		return *reinterpret_cast<RDFDerivedColumnList const *>(&columns_);
	}
	RDFDerivedColumnList const &RDFDerivedSelect::columns() const { return d_func()->columns(); }

	short RDFDerivedSelect::baseColumnCount() const
	{ return d_func()->base_columns_; }
	short RDFDerivedSelect::derivedRealColumnCount() const
	{ return d_func()->real_columns_ - d_func()->base_columns_; }
	short RDFDerivedSelect::derivedVirtualColumnCount() const
	{ return d_func()->columns_.size() - d_func()->real_columns_; }
	short RDFDerivedSelect::derivedColumnCount() const
	{ return d_func()->columns_.size() - d_func()->base_columns_; }
	short RDFDerivedSelect::hiddenColumnCount() const
	{ return d_func()->columns_.size() - d_func()->visible_columns_; }
	short RDFDerivedSelect::realColumnCount() const
	{ return d_func()->real_columns_; }
	short RDFDerivedSelect::visibleColumnCount() const
	{ return d_func()->visible_columns_; }

	/*!
	 * Add the given \a full_info column as a derived column with
	 * given RDFDerivedColumn::Type \a type and given \a parent column
	 */
	RDFDerivedColumn &RDFDerivedSelect::Private::addColumn
			(RDFDerivedColumn &full_info, RDFStrategyFlags type, int parent)
	{
		RDFDerivedColumn::Private *fd = full_info.d_func();

		int index = columns_.size();
		fd->index_ = index;
		fd->parent_ = parent;

		if(parent >= index)
			warning("RDFDerivedSelect::addColumn")
				<< "parent(" << parent << ") >= index(" << index <<")";

		if(type & RDFDerivedColumn::BaseColumn)
		{
			++base_columns_;
			++real_columns_;
			++visible_columns_;
			full_info.markAsBaseColumn(type & RDFStrategy::IdentityColumn);
		}
		else
		{
			if(type & RDFDerivedColumn::DerivedRealColumn)
			{
				++real_columns_;
				++visible_columns_;
				full_info.markAsDerivedRealColumn();
			} else if(type & RDFDerivedColumn::DerivedVirtualColumn)
			{
				++visible_columns_;
				full_info.markAsDerivedVirtualColumn();
			} else if(!(type & RDFDerivedColumn::HiddenColumn))
				warning() << "RDFDerivedSelect::addColumn: "
						"no column type flag specified, treating column as hidden column";

			if(parent != RDFDerivedColumn::NullIndex)
			{
				RDFDerivedColumn &p = refColumns()[parent];
				p.d_func()->refDeriveds()[RawPropertyData(fd->property_data_)] = fd->index_;
				p.d_func()->derived_column_count_ +=
						(type & RDFDerivedColumn::DerivedRealColumn ? 1 : 0);
			}
		}
		full_info.markColumnType(type);
		//d->preprocessNewColumnVariables()

		// prevent implicit detach later on, push_back and zero full_info
		columns_.push_back(full_info);
		full_info.d = 0;
		return static_cast<RDFDerivedColumn &>(columns_.back());
	}
	RDFDerivedSelect &RDFDerivedSelect::addColumn
			(RDFProperty const &property, RDFStrategyFlags type, int parent)
	{
		RDFDerivedColumn dep = RDFDerivedColumn::Private::create(property);
		d_func()->addColumn(dep, type, parent);
		return *this;
	}
	RDFDerivedSelect &RDFDerivedSelect::addColumn
			( RDFSelectColumn const &select_column, RDFStrategyFlags type, int parent)
	{
		RDFDerivedColumn dep = RDFDerivedColumn::Private::create(select_column);
		d_func()->addColumn(dep, type, parent);
		return *this;
	}
	RDFDerivedSelect &RDFDerivedSelect::addColumn
			( RDFVariable const &column_variable, RDFStrategyFlags type, int parent)
	{
		addColumn(RDFProperty::fromVariable(column_variable), type, parent);
		return *this;
	}
	RDFDerivedSelect &RDFDerivedSelect::addColumn
			( QString const &name, RDFVariable const &column_variable, RDFStrategyFlags type
			, int parent)
	{
		RDFVariable col_var(column_variable);
		if(name.size())
			col_var.metaSetIdentifier(name);
		addColumn(col_var, type, parent);
		return *this;
	}

	RDFDerivedSelect &RDFDerivedSelect::addColumns
			( RDFVariableList const &column_variables, RDFStrategyFlags type, int parent)
	{
		if(column_variables.size())
			for(RDFVariableList::const_iterator cvi = column_variables.begin()
						, cviend = column_variables.end()
					; cvi != cviend; ++cvi)
				addColumn(*cvi, type, parent);
		return *this;
	}

	RDFDerivedColumn &RDFDerivedSelect::operator[](int index)
	{ return static_cast<RDFDerivedColumn &>(d_func()->columns_[index]); }

	RDFDerivedColumn const &RDFDerivedSelect::operator[](int index) const
	{ return static_cast<RDFDerivedColumn const &>(d_func()->columns_[index]); }

	/*!
	 * Private base implementation function for RDFDerivedSelect::processDerivedColumns
	 * TODO: add doc
	 */
	int RDFDerivedSelect::Private::processDerivedColumns
			( int index, bool expand, bool join, bool flat, int primaries)
	{
		if(index >= real_columns_)
			return 0;

		int ret = 0;

		RDFDerivedColumnList &cols = refColumns();

		bool has_aggregations = false;

		bool has_identity_columns = false;
		// first pass, expand derived columns recursively so that
		// single valued columns appear before multiple valued model columns
		for(int i = index; i < real_columns_; ++i)
		{
			RDFDerivedColumn &col = cols[i];
			RDFVariable const &var = col.variable();

			if(expand)
			{
				QVector<RDFProperty> props = var.derivedProperties();
				for(QVector<RDFProperty>::const_iterator cpi = props.constBegin()
							, cpiend = props.constEnd()
						; cpi != cpiend; ++cpi)
				{
					RDFStrategyFlags s = cpi->strategy();
					if(s & RDFStrategy::UpdateProperty)
						col.d_func()->refDeriveds()[*cpi] = RDFDerivedColumn::NullIndex;
					else
					if(!cpi->target().metaIsDefinite())
					{
						RDFDerivedColumn dep = RDFDerivedColumn::Private::create
								(*cpi, RDFDerivedColumn::NullIndex, i);

						int insert_loc = (s & RDFStrategy::NonMultipleValued)
							? (dep.markAsDerivedRealColumn(), real_columns_++)
							: (dep.markAsDerivedVirtualColumn(), visible_columns_);
						visible_columns_++;

						if(RDFSelectColumn::Mode mode = RDFSelectColumn::Mode(
								(s & (RDFStrategy::AggregateColumnMask))
									>> RDFStrategy::offset_AggregateColumnMode))
						{
							dep.d_func()->mode_ = mode;
							has_aggregations = true;
						}
						cols.insert(insert_loc, dep);
						++ret;
					}
				}
			}

			RDFStrategyFlags strat = var.metaStrategy();

			if(strat & RDFStrategy::IdentityColumn)
				has_identity_columns = true;

			if(strat & (RDFStrategy::Ascending | RDFStrategy::Descending))
				order_by.push_back(qMakePair(i, !(strat & RDFStrategy::Descending)));
		}

		// second pass.
		if(!has_identity_columns)
			for(int i = index; i < real_columns_; ++i)
				cols[i].enableColumnStrategyFlags(RDFStrategy::IdentityColumn);

		int i = index;

		if(has_aggregations)
		{
			// If there are aggregations, set groupings and ordering for base columns
			for( ; i < base_columns_; ++i)
				// only group/order by the identity columns
				if(cols[i].strategy() & RDFStrategy::IdentityColumn)
				{
					group_by.push_back(i);
					if(!(cols[i].variable().metaStrategy()
							& (RDFStrategy::Ascending | RDFStrategy::Descending)))
						order_by.push_back(qMakePair(i, true));
				}
		}
		else
			i = std::max(i, int(base_columns_));

		// finalize indices and set derived forward references
		for(int j = i, jend = visible_columns_; j < jend; ++j)
		{
			RDFDerivedColumn &col = cols[j];
			col.d_func()->index_ = j;

			int parent_index = col.parent();
			if(parent_index >= j)
				warning("RDFDerivedSelect::processDerivedColumns")
					<< "parent(" << parent_index << ") >= index(" << j << ")";

			RDFDerivedColumn &parent = cols[col.parent()];
			parent.d_func()->refDeriveds()[col.propertyData()] = col.index();

			// for columns only, ie. single-valued derivedproperties, increase count
			if(j < real_columns_)
				parent.d_func()->addToDerivedColumnCount(1);
		}

		// third pass, join columns together
		if(join)
			for(int j = i, jend = visible_columns_; j < jend; ++j)
				if(!(cols[j].strategy() & RDFStrategy::BoundProperty))
					joinColumn(cols[j], flat);

		return ret;
	}

	int RDFDerivedSelect::processDerivedColumns
			( int index, bool expand, bool join, bool flat, int primaries)
	{
		return d_func()->processDerivedColumns(index, expand, join, flat, primaries);
	}

		QSharedPointer<RDFQuery::Data::Builder>
	RDFDerivedSelect::Private::createSelectBuilder() const
	{
		return QSharedPointer<RDFQuery::Data::Builder>(new Builder(*this));
	}

	//! Connects the column variable to the parent column using the property of the column,
	//! so that a connection appears in the actual resulting query.
	RDFDerivedColumn &RDFDerivedSelect::Private::joinColumn
			( RDFDerivedColumn &column, bool flat) const
	{
		RDFStrategyFlags s = column.strategy();

		RDFVariable parent_var = variable(column.parent());

		// TODO: implement multitarget variables
		//RDFProperty prop = column.refPropertyData().link();
		RDFProperty prop = column.refPropertyData().deepCopy();

		if(!flat)
			parent_var.metaAssign(RDFVariableLink(parent_var));

		RDFVariable source = !(s &
					(RDFStrategy::NonOptionalValued | RDFDerivedColumn::DerivedVirtualColumn))
				? parent_var.optional()
				: parent_var.child();

		if(s & RDFStrategy::ChainedProperty)
			source.merge(prop.source());
		else
			source.bindProperty(prop);

		// TODO: duplicate logic. Assignment of these should be hidden.
		column.d->var_.metaAssign(prop.target());
		column.d->outer_variable_.metaAssign(prop.target());

		return column;
	}

	void RDFDerivedSelect::Private::joinColumnVariables(bool flat)
	{
		for(RDFDerivedColumnList::iterator cii = refColumns().begin(), ciiend = refColumns().end()
				; cii != ciiend; ++cii)
			if(cii->parent() != RDFDerivedColumn::NullIndex)
				joinColumn(*cii, flat);
	}

	RDFDerivedSelect::Private::Builder::Builder(Private const &p)
		: RDFSelect::Data::Builder(p)
	{}

	bool RDFDerivedSelect::Private::Builder::processSelectColumn
			(RDFSelectColumn &column) const
	{
		if(RDFDerivedColumn(column).strategy() & RDFDerivedColumn::MuteColumn)
		{
			RDFDerivedColumn &dcol = static_cast<RDFDerivedColumn &>(column);
			// Add empty variable as hole placeholder for mute columns
			dcol.d->var_.metaAssign(RDFVariable());
			dcol.d->outer_variable_.metaAssign(RDFVariable());
			return true;
		}
		return RDFSelect::Data::Builder::processSelectColumn(column);
	}

    RDFDerivedSelect::Private* RDFDerivedSelect::d_func()
    {	return static_cast<Private *>(d.data()); }

    const RDFDerivedSelect::Private* RDFDerivedSelect::d_func() const
    {	return static_cast<Private const *>(d.data()); }
}
