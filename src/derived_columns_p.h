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
 * derived_columns_p.h
 *
 *  Created on: Jul 22, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_DEPENDENT_COLUMNS_P_H_
#define SOPRANOLIVE_DEPENDENT_COLUMNS_P_H_

#include "livenodemodelbridge_p.h"
#include "triplemodel_p.h"

namespace SopranoLive
{
	class DerivedColumnModule
		: public QObject
		, public TripleModule
	{
		/* VZR_CLASS(DerivedColumnModule, (QObject), (TripleModule));*/

	protected:

		struct DerivedRealColumnInfo;

		typedef QMap<int, LiveNodeModelBasePtr> Models;
		Models column_models; // VZR_MEMBER


		RDFDerivedSelect * columns_; // VZR_MEMBER
		RDFGraphImplBasePtr graph_; // VZR_MEMBER


		QModelIndex index(RawPropertyData const &property, QModelIndex const &current) const;

		RDFStrategyFlags refreshModel_chain(LiveNodeModel::RefreshMode mode, InterfaceChain<ModelCommons> chain);

		RDFVariableList columnVariables() { return columns_->variables(); }

		RDFVariableList joinLinkedColumnVariables() const;

		LiveNodeModelBasePtr modelColumnVariableImpl(RDFDerivedColumn &column_info, TripleModulePtr column_module);
		virtual LiveNodeModelBasePtr modelColumnVariable(RDFDerivedColumn &column_info) = 0;

		void attached(TripleModel &model);

		struct DerivedUpdate : UpdateContext
		{
			/* VZR_CLASS(DerivedUpdate, (UpdateContext));*/

			DerivedUpdate(ConstLiveNodeModelBasePtr const &model) : UpdateContext(model) {}
			bool updateModel(LiveNodeModelBase *target) const;
		};

		bool updateFrom_chain(UpdateContext const &update_context, InterfaceChain<ModelCommons> chain);
		bool updateTargetModule_chain
				( ModelModule *target_module, UpdateContext const &update_context
				, InterfaceChain<ModelCommons const> chain) const;

	public:
		DerivedColumnModule(RDFGraphImplBasePtr const &graph);

		RDFDerivedSelect &columns() { return *columns_; }
		RDFDerivedSelect const &columns() const { return *columns_; }
		virtual bool isCompatibleWith(TripleModel *triple_model) const;

	protected:

		int expandBinarySearchEnd(RowStore::const_iterator const &base_i, int column) const;
	public:
		LiveNodeModelPtr childModel
				( QModelIndex const &index, RDFStrategyFlags flags, QSharedPointer<QObject> const &parent);
	};
	typedef QSharedPointer<DerivedColumnModule> DerivedColumnModulePtr;

	class ItemsAsColumnModule
	{
		/* VZR_CLASS(ItemsAsColumnModule);*/

	public:
		void addSetItems(QStringList adds, RowStoreModel *model) const;
		void removeSetItems(QStringList removes, RowStoreModel *model) const;
	};

}
#endif /* DEPENDENT_COLUMNS_P_H_ */
