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
 * rdftrackerselect_p.cpp
 *
 *  Created on: May 10, 2010
 *      Author: "Iridian Kiiskinen"
 */

#include "rdftrackerselect_p.h"
#include "../../include/QtTracker/ontologies/tracker.h"

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			SharedNull<RDFTrackerSelectPrivate> RDFTrackerSelectPrivate::shared_null;

			unsigned RDFTrackerSelectPrivate::columnSubstituent(unsigned column)
			{
				QMap<ResourceColumn, TrackerIDColumn>::const_iterator tidc
						= resource_to_tracker_id.find(column);
				if(tidc != resource_to_tracker_id.end())
					return *tidc;

				RDFDerivedColumn const &col = columns()[column];
				if(!col.variable().metaIsResource())
					return column;

				RDFVariable var = col.variable().function(tracker::iri("id"));
				var.isOfType<int>();

				RDFDerivedColumn prop = RDFDerivedColumn::Private::create
						(RDFProperty::fromVariable(var));

				return resource_to_tracker_id[column]
						= addColumn(prop, RDFDerivedColumn::HiddenColumn, column).index();
			}

			void RDFTrackerSelectPrivate::insertGroupBy
					( unsigned column, unsigned group_by_priority)
			{
				RDFDerivedSelect::Private::insertGroupBy
						( columnSubstituent(column), group_by_priority);
			}
			void RDFTrackerSelectPrivate::insertOrderBy
					( unsigned column, bool ascending, unsigned order_by_priority)
			{
				RDFDerivedSelect::Private::insertOrderBy
						( columnSubstituent(column), ascending, order_by_priority);
			}

			RDFTrackerSelectPrivate::Builder::Builder(RDFTrackerSelectPrivate const &query)
				: RDFDerivedSelect::Private::Builder(query)
			{};

			//! Override for custom node strings. Converts QDate into QDateTime for tracker.
			QString RDFTrackerSelectPrivate::Builder::nodeString(Soprano::Node const &node) const
			{
				// duplicate code with RDFTrackerUpdatePrivate in rdftrackerupdate_p.cpp
				if(node.isLiteral())
				{
					LiteralValue l = node.literal();
					if(l.isDate())
						return RDFDerivedSelect::Private::Builder::nodeString
								(LiteralValue(QDateTime(l.toDate(), QTime(), Qt::UTC)));
				}
				return RDFDerivedSelect::Private::Builder::nodeString(node);
			}

			QString RDFTrackerSelectPrivate::Builder::recurseFilter
					(Unboundtor::Filter const &filter, int indentation)
			{
				if(filter.filtertype_ == Filter::isMemberOf)
				{
					Unboundtor::FunctionFilter const &ffilter
							= static_cast<Unboundtor::FunctionFilter const &>(filter);
					QStringList multis;
					for(QLinkedList<Unboundtor::HasVariable>::const_iterator vi = ffilter.arguments.begin()
							, viend = ffilter.arguments.end(); vi != viend; ++vi)
						multis.push_back(*variableName(vi->variable()));
					QString member = *variableName
							(static_cast<Unboundtor::MemberFilter const &>(filter).member());
					return "(" + member + " IN (" + multis.join(",") + "))";
				}
				return RDFDerivedSelect::Private::Builder::recurseFilter(filter, indentation);
			}

			RDFSelectColumnList RDFTrackerSelectPrivate::Builder::buildSelectColumnRange() const
			{
				if(!query().resource_to_tracker_id.size())
					// if no tracker id columns, generate just the real columns
					return RDFDerivedSelect::Private::Builder::buildSelectColumnRange();
				// otherwise, generate all columns but add placeholders for virtual columns
				return query().columns_;
			}
			bool RDFTrackerSelectPrivate::Builder::processSelectColumn(RDFSelectColumn &column) const
			{
				if(RDFDerivedColumn(column).strategy() & RDFDerivedColumn::MuteColumn)
				{
					RDFDerivedColumn &dcol = static_cast<RDFDerivedColumn &>(column);
					// Add QString("") as hole placeholders for mute columns
					RDFVariable var = LiteralValue("");
					dcol.d->var_.metaAssign(var);
					dcol.d->outer_variable_.metaAssign(var);
					return true;
				}
				return RDFDerivedSelect::Private::Builder::processSelectColumn(column);
			}

			RDFDerivedColumn &RDFTrackerSelectPrivate::joinColumn
					( RDFDerivedColumn &column, bool flat) const
			{
				RDFStrategyFlags s = column.strategy();

				bool non_optional = s &
						(RDFStrategy::NonOptionalValued | RDFDerivedColumn::DerivedVirtualColumn);

				RDFVariable parent_var = variable(column.parent());

				if(!flat)
					parent_var.metaAssign(RDFVariableLink(parent_var));

				RDFDerivedColumn::Private *col_d = RDFDerivedColumn::Private::d_func(column);

				RDFProperty prop = col_d->property_data_;
				if(!non_optional
						&& !(s & RDFStrategy::ChainedProperty)
						&& col_d->property_data_.target().metaIsUnconstrained()
						&& !col_d->refDeriveds().size()
						&& !parent_var.metaIsDefinite()
						)
				{
					RDFVariable column_variable =
							parent_var.function(prop.predicate().metaValue());
					column_variable.isOfType(prop.target().varCppType());

					// TODO: duplicate logic. Assignment of these should be hidden.
					column.d->var_.metaAssign(column_variable);
					column.d->outer_variable_.metaAssign(col_d->property_data_.target());

				} else
				{
					RDFProperty new_prop = prop.deepCopy();

					RDFVariable source = non_optional
							? parent_var.child()
							: parent_var.optional();

					if(s & RDFStrategy::ChainedProperty)
						source.merge(new_prop.source());
					else
						source.bindProperty(new_prop);

					// TODO: duplicate logic. Assignment of these should be hidden.
					column.d->var_.metaAssign(new_prop.target());
					column.d->outer_variable_.metaAssign(new_prop.target());
				}


				return column;
			}
		}
	}
}
