/*
 * livenodemodelbase_p.h
 *
 *  Created on: Oct 17, 2009
 *      Author: iridian
 */

#ifndef SOPRANOLIVE_LIVENODEMODELBASE_P_H_
#define SOPRANOLIVE_LIVENODEMODELBASE_P_H_

#include "liveresource_p.h"
#include "rdfservicebase_p.h"
#include "compositemodule_p.h"
#include "util_p.h"

namespace SopranoLive
{
	/*!
	 * Content update context.
	 * Implemented by live update, streaming update, and write update contexts.
	 */
	struct UpdateContext
		: QSharedFromThis<UpdateContext>
	{
		/* VZR_CLASS(UpdateContext, (QSharedFromThis<UpdateContext>));*/

		ConstLiveNodeModelBasePtr model_; // VZR_MEMBER

		UpdateContext(ConstLiveNodeModelBasePtr const &model) : model_(model) {}
		virtual bool updateModel(LiveNodeModelBase *target) const = 0;
	};
	typedef QSharedPointer<UpdateContext> UpdateContextPtr;

	// a module of LiveNodeModelBase
	struct ModelModule
		: SingleModule<LiveNodeModelBase>
	{
		/* VZR_CLASS(ModelModule, (SingleModule<LiveNodeModelBase>));*/

		ModelModule(LiveNodeModelBase *model = 0) : SingleModule<LiveNodeModelBase>(model) {}

		LiveNodeModelBase *model() const { return this->compositor(); }
	};
	typedef QSharedPointer<ModelModule> ModelModulePtr;

	struct ModelCommons
	{
		/* VZR_CLASS(ModelCommons);*/

		virtual bool isAdoptable_chain
				( QSharedPointer<RDFGraphImplBase> const &new_model_parent
				, InterfaceChain<ModelCommons const> chain) const;
		virtual void adopted_chain
				( QSharedPointer<RDFGraphImplBase> const &new_model_parent
				, InterfaceChain<ModelCommons> chain);

		virtual RDFStrategyFlags strategy_chain
				( QModelIndex const &index, InterfaceChain<ModelCommons const> chain) const;
		virtual bool alterStrategy_chain
				( RDFStrategyFlags flags, RDFStrategyFlags mask, QModelIndex const &index
				, InterfaceChain<ModelCommons> chain);
		enum
		{
			NothingReady = 0
		,	ModelReady = RDFStrategy::ModelReady
		,	DerivedsReady = RDFStrategy::DerivedsReady
		,	AllReady = ModelReady | DerivedsReady
		};
		typedef RDFStrategyFlags ReadyFlags;

		virtual ReadyFlags refreshModel_chain
				(LiveNodeModel::RefreshMode mode, InterfaceChain<ModelCommons> chain);
		virtual ReadyFlags refreshRows_chain
				( int row, int count, QModelIndex const &parent, LiveNodeModel::RefreshMode mode
				, InterfaceChain<ModelCommons> chain);

		virtual bool updateFrom_chain
				( UpdateContext const &update_context, InterfaceChain<ModelCommons> chain);
		virtual bool updateTargetModule_chain
				( ModelModule *target_module, UpdateContext const &update_context
				, InterfaceChain<ModelCommons const> chain) const;

		virtual QVariant modelAttribute_chain
				( QString const &attribute_name, InterfaceChain<ModelCommons const> chain) const;
		virtual bool setModelAttribute_chain
				( QString const &attribute_name, QVariant const &value
				, InterfaceChain<ModelCommons> chain);
	};

	class LiveNodeModelBase
		: public LiveNodeModel
		, public CompositorAndBaseModule<LiveNodeModelBase>
		, public CompositorInterface<ModelCommons, LiveNodeModelBase, LiveNodeModelBase>
		, public LogContext
	{
		Q_OBJECT

		/* VZR_CLASS(LiveNodeModelBase
				, (LiveNodeModel)
				, (CompositorAndBaseModule<LiveNodeModelBase>)
				, (CompositorInterface<ModelCommons, LiveNodeModelBase, LiveNodeModelBase>)
				, (LogContext));*/

	public:
		using LiveNodeModel::sharedFromThis;
		using LiveNodeModel::initialSharedFromThis;

		enum StrategyFlags
		{	offset_WholeIndex = offsetMark_LiveNodeStrategyFlagsEnd
		, 	offsetEnd_RowStoreModelFlags

		,	WholeIndex = 1 << offset_WholeIndex

		,	ModelStrategyStateMask =
				  RDFStrategy::Streaming | RDFStrategy::Windowed | RDFStrategy::Live
				| RDFStrategy::Writable | RDFStrategy::Cached
				| RDFStrategy::Running | ExposeDerivedProperties | RDFStrategy::CacheDataRoles
		};

		QSharedPointer<RDFGraphImplBase> graph_; // VZR_MEMBER
		RDFDerivedSelect columns_; // VZR_MEMBER

		RDFStrategyFlags strategy_; // VZR_MEMBER
		QVariantMap model_attributes_; // VZR_MEMBER

		QVector<SharedTypeSet> column_types_; // VZR_MEMBER

		LiveNodeModelBase
				( QSharedPointer<RDFGraphImplBase> const &graph
				, RDFStrategyFlags strategy = RDFStrategy::Writable);
		~LiveNodeModelBase();

		// default overrides of QAbstractItemModel

		int columnCount(QModelIndex const &parent = QModelIndex()) const;
		QVariant headerData
				(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

		bool canFetchMore(QModelIndex const &parent) const;
		void fetchMore(QModelIndex const &parent);

		// default overrides of LiveNodeModel

		RDFGraphImplBase *graph() const;

		RDFStrategyFlags strategy(QModelIndex const &index = QModelIndex()) const;

		RDFVariableList sourceVariables() const;

		bool columnHasType(int column, LiveNode const &type) const;

		bool alterStrategy
				( RDFStrategyFlags flags, RDFStrategyFlags mask
				, QModelIndex const &index = QModelIndex());

		// change strategy_ and emit strategy change signal, called by implementations
		bool doAlterStrategy
				( RDFStrategyFlags changed_flags, RDFStrategyFlags changed_mask
				, QModelIndex const &index = QModelIndex());

		bool doEnableStrategyFlags
				( RDFStrategyFlags enabled_flags, QModelIndex const &index = QModelIndex())
		{
			return doAlterStrategy(enabled_flags, enabled_flags, index);
		}

		bool doDisableStrategyFlags
				( RDFStrategyFlags disabled_flags, QModelIndex const &index = QModelIndex())
		{
			return doAlterStrategy(~disabled_flags, disabled_flags, index);
		}

		RDFStrategyFlags refreshModel_chain
				( RefreshMode mode
				, InterfaceChain<ModelCommons> chain = InterfaceChain<ModelCommons>());
		bool refreshModel(RefreshMode mode = Flush);

		RDFStrategyFlags refreshRows_chain
				( int row, int count, QModelIndex const &parent, RefreshMode mode
				, InterfaceChain<ModelCommons> chain = InterfaceChain<ModelCommons>());
		bool refreshRows(int row, int count, QModelIndex const &parent = QModelIndex()
				, RefreshMode mode = Flush);

		LiveNodeModel *plainModel();

		LiveNodeModelPtr childModel
				( QModelIndex const &index, RDFStrategyFlags flags = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());

		QVariant modelAttribute(QString const &attribute_name) const;
		QVariantMap modelAttributes() const;
		bool setModelAttribute(QString const &attribute_name, QVariant const &value);

		// default overrides for ModelCommons operations

		bool isAdoptable_chain
				( QSharedPointer<RDFGraphImplBase> const &new_model_parent
				, InterfaceChain<ModelCommons const> chain
						= InterfaceChain<ModelCommons const>()) const;
		void adopted_chain
				( QSharedPointer<RDFGraphImplBase> const &new_model_parent
				, InterfaceChain<ModelCommons> chain = InterfaceChain<ModelCommons>());

		RDFStrategyFlags strategy_chain
				( QModelIndex const &index
				, InterfaceChain<ModelCommons const> chain
						= InterfaceChain<ModelCommons const>()) const;
		bool alterStrategy_chain
				( RDFStrategyFlags flags, RDFStrategyFlags mask, QModelIndex const &index
				, InterfaceChain<ModelCommons> chain = InterfaceChain<ModelCommons>());

		bool updateFrom_chain
				( UpdateContext const &update_context
				, InterfaceChain<ModelCommons> chain = InterfaceChain<ModelCommons>());
		bool updateTargetModule_chain
				( ModelModule *target_module, UpdateContext const &update_context
				, InterfaceChain<ModelCommons const> chain
						= InterfaceChain<ModelCommons const>()) const;

		QVariant modelAttribute_chain
				( QString const &attribute_name
				, InterfaceChain<ModelCommons const> chain
						 = InterfaceChain<ModelCommons const>()) const;
		bool setModelAttribute_chain
				(QString const &attribute_name, QVariant const &value
				, InterfaceChain<ModelCommons> chain = InterfaceChain<ModelCommons>());

		// Locally added methods

		//! change the associated graph to given \a new_parent
		virtual bool adopt(QSharedPointer<RDFGraphImplBase> const &new_parent);

		//! update contents (add, update and remove rows) within given \a update_context.
		virtual bool updateFrom(UpdateContext const &update_context);

		//! when this model is used for updating another, update given \a target_module within
		// given \a update_context
		virtual bool updateTargetModule
				( ModelModule *target_module, UpdateContext const &update_context) const;

		/*!
		 * \return an index that can be outside existing regions, such as negative values
		 * for represent areas (-1 for row represents the whole column, etc.)
		 */
		QModelIndex areaIndex(int row, int column, const QModelIndex &parent = QModelIndex());

		//! Sets the column information structure and all dependent data
		virtual void setColumnInformation(RDFDerivedSelect const &columns);

		LogContext const *parentContext() const;
		QDebug logMessage(int level, char const *func, char const *loc) const;

	public Q_SLOTS:

		void doEmitModelUpdated();
		void doEmitDerivedsUpdated();
		void doEmitError
				( QString const &message
				, RDFStrategyFlags mask, RDFStrategyFlags flags, QModelIndex const &index);

	public:
		void emitErrorImpl
				( QString const &message, RDFStrategyFlags mask = 0
				, QModelIndex const &index = QModelIndex(), bool queued=true);
		void emitErrorImpl
				( QString const &message, RDFStrategyFlags mask, RDFStrategyFlags flags
				, QModelIndex const &index = QModelIndex(), bool queued=true);
		void emitAlterStrategyError
				( QString const &message, RDFStrategyFlags mask, RDFStrategyFlags flags
				, QModelIndex const &index = QModelIndex());
	};

		template<typename DerivedModel_>
	struct ModuleOfDerivedModel
		: SingleModuleOfDerivedCompositor<DerivedModel_, ModelModule>
	{
		/* VZR_CLASS(ModuleOfDerivedModel
				, (SingleModuleOfDerivedCompositor<DerivedModel_, ModelModule>));*/

		ModuleOfDerivedModel(DerivedModel_ *model = 0)
			: SingleModuleOfDerivedCompositor<DerivedModel_, ModelModule>(model) {}

		DerivedModel_ *model() const { return this->compositor(); }
	};
}

#endif /* SOPRANOLIVE_LIVENODEMODELBASE_P_H_ */
