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
 * triplemodel_p.h
 *
 *  Created on: Jun 3, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_TRIPLEMODEL_P_H_
#define SOPRANOLIVE_TRIPLEMODEL_P_H_

#include "rowstoremodel_p.h"

namespace SopranoLive
{
	class RDFGraphCache;
	class ResourceOfGraphCache;

		template<typename Base>
	struct TripleModelIndex;
	typedef TripleModelIndex<QPersistentModelIndex> PersistentTripleIndex;
	typedef TripleModelIndex<QModelIndex> TripleIndex;

		template<typename Base>
	struct TripleModelIndex
		: public Base
	{
		TripleModelIndex() {}
		~TripleModelIndex() { /*if(TripleModel *m = model()) m->indexRemoved(*this); */}

		TripleModel *model() { return const_cast<TripleModel *>(static_cast<TripleModel const *>(Base::model())); }
		TripleModel const *model() const { return static_cast<TripleModel const *>(Base::model());; }

		bool isWholeIndex() const { return model() ? model()->isWholeIndex(*this) : false; }

		RDFStrategyFlags strategy() const
		{
			return model() ? model()->childModelStrategy(*this) : unsigned(RDFStrategy::Disabled);
		}
		LiveNodeModelBasePtr childModel()
		{
			return model()
					? model()->childModel(*this).template staticCast<LiveNodeModelBase>()
					: LiveNodeModelBasePtr();
		}
		QModelIndex index(RawTripleData const &triple_data) const
		{
			return model() ? model()->index(triple_data, *this) : QModelIndex();
		}
		QModelIndex index(RawPropertyData const &property_data) const
		{
			return model() ? model()->index(property_data, *this) : QModelIndex();
		}

		TripleModelIndex(TripleModelIndex const &cp) : Base(cp) { indexAdded(); }
		TripleModelIndex(QModelIndex const &index) : Base(index) { indexAdded(); }
		TripleModelIndex(QPersistentModelIndex const &other) : Base(other) { indexAdded(); }
		void indexAdded() { /*if(TripleModel *m = model()) m->indexAdded(*this); */ }
	};

	class TripleModel
		: public RowStoreModel
		, public CompositorInterface<TripleUpdateReceiver, TripleModel, LiveNodeModelBase>
	{
		/* VZR_CLASS(TripleModel
				, (RowStoreModel)
				, (CompositorInterface<TripleUpdateReceiver, TripleModel, LiveNodeModelBase>));*/

	public:
		enum StrategyFlags
		{	offset_NoFalsePositives = offsetEnd_RowStoreModelFlags, offset_NoFalseNegatives
		, 	offsetEnd_TripleModelFlags

		,	NoFalsePositives = 1 << offset_NoFalsePositives
		,	NoFalseNegatives = 1 << offset_NoFalseNegatives
		,	Definite = NoFalsePositives | NoFalseNegatives
		};

		~TripleModel();

		// Methods overridden from LiveNodeModel

		inline TripleModule *tripleModule() const;

		bool changeTripleModule(TripleModulePtr const &new_module = TripleModulePtr());

		LiveNodeModelPtr childModel(QModelIndex const &index, RDFStrategyFlags flags = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());

		// Methods locally added

		ModuleHandle triple_module_handle_; // VZR_MEMBER

		void registerInto(RDFGraphCache &cache);

		virtual bool hasTriple(RawTripleData const &triple) const;

		using RowStoreModel::index;

		QModelIndex index
				( RawTripleData const &triple = RawTripleData(), QModelIndex const &current = QModelIndex()) const;
		QModelIndex index(RawPropertyData const &property_data, QModelIndex const &current = QModelIndex()) const;

		RDFStrategyFlags childModelStrategy(QModelIndex const &index) const;

//		bool transitionRows
		//		( int row, int count, LiveNodes const &rows, bool smooth_transition = true
		//		, QModelIndex const &parent = QModelIndex());

		bool tripleUpdates_chain
				( RawTriplesData const &triples, TripleUpdateContextConstPtr const &context
				, InterfaceChain<TripleUpdateReceiver> chain);

	protected:
		friend class QSharedCreator<TripleModel>;
		friend class TripleModule;
		TripleModel(QSharedPointer<RDFGraphImplBase> const &graph, RDFStrategyFlags strategy = RDFStrategy::Writable);

		void indexAdded(PersistentTripleIndex const &index) const;
		void indexRemoved(PersistentTripleIndex const &index) const;
	};

	class TripleModule
		: public ModuleOfDerivedModel<TripleModel>
		, public ModuleInterface<ModelCommons, TripleModel, TripleModule>
	{
		/* VZR_CLASS(TripleModule
				, (ModuleOfDerivedModel<TripleModel>)
				, (ModuleInterface<ModelCommons, TripleModel, TripleModule>));*/

	public:
		typedef ModuleOfDerivedModel<TripleModel> Base;

		RowStore * rows_; // VZR_MEMBER

		TripleModule()
			: Base(), rows_(0) {}

		TripleModule(TripleModule const &cp)
			: Base(cp), ModuleInterface<ModelCommons, TripleModel, TripleModule>(cp), rows_(0) {}

		static RowStore &rowCache(RowStoreModel &model) { return model.row_store; }
		static RowStore const &rowCache(RowStoreModel const &model) { return model.row_store; }
		RowStore &rowCache() { return *rows_; }
		RowStore const &rowCache() const { return *rows_; }

		void attached(TripleModel &model);

		virtual void registerInto(RDFGraphCache &cache) = 0;

		virtual RDFStrategyFlags strategy_chain
				( QModelIndex const &index, InterfaceChain<ModelCommons const> chain) const;

		virtual QModelIndex index(RawTripleData const &triple, QModelIndex const &current) const
		{	return QModelIndex();  Q_UNUSED(triple); Q_UNUSED(current); }

		virtual QModelIndex index(RawPropertyData const &property_data, QModelIndex const &current) const
		{	return index(property_data.toRawTripleData(), current); }

		virtual RDFStrategyFlags childModelStrategy(QModelIndex const &index) const
		{	return model()->columns_[index.column()].strategy(); }

		virtual LiveNodeModelPtr childModel
				( QModelIndex const &index, RDFStrategyFlags flags = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())
		{	return LiveNodeModelPtr();  Q_UNUSED(index); Q_UNUSED(flags); Q_UNUSED(parent); }

			template<typename P3>
		PersistentTripleIndex createTripleIndex(int row, int column, P3 const &p3) const;

			template<typename P3>
		QModelIndex createIndex(int row, int column, P3 const &p3) const;
	};

	inline TripleModule *TripleModel::tripleModule() const
	{ return static_cast<TripleModule *>(triple_module_handle_->data()); }

		template<typename P3>
	PersistentTripleIndex TripleModule::createTripleIndex(int row, int column, P3 const &p3) const
	{	return model()->createIndex(row, column, quint32(p3)); }

		template<typename P3>
	QModelIndex TripleModule::createIndex(int row, int column, P3 const &p3) const
	{	return model()->createIndex(row, column, quint32(p3)); }
}

#endif /* SOPRANOLIVE_TRIPLEMODEL_P_H_ */
