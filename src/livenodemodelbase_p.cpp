/*
 * livenodemodelbase_p.cpp
 *
 *  Created on: Oct 17, 2009
 *      Author: iridian
 */

#include "triplemodel_p.h"
#include "livenodemodelbridge_p.h"
#include "util_p.h"

namespace SopranoLive
{
	bool LiveNodeModel::matchStrategy
			( RDFStrategyFlags lhs, RDFStrategyFlags rhs, RDFStrategyFlags mask)
	{
		lhs ^= rhs;
		mask &= ~(((lhs & AllowsMask) >> offsetMark_RequestAllowFlags) | AllowsMask);
		return (lhs & mask) == 0;
	}

	QModelIndex LiveNodeModel::wholeIndex() const
	{
		//return createIndex(0,0, quint32(strategy() | LiveNodeModelBase::WholeIndex));
		return createIndex(0,0, quint32(LiveNodeModelBase::WholeIndex));
	}

	bool LiveNodeModel::isWholeIndex(QModelIndex const &index) const
	{
		return ((index.model() == this) && (index.internalId() & LiveNodeModelBase::WholeIndex))
				|| index == QModelIndex();
	}

	LiveNode LiveNodeModel::liveNode
			( QModelIndex const &index, LiveNode const &type, bool strict) const
	{
		LiveNode ret = data(index, LiveNodeRole).value<LiveNode>();
		if(type.isValid() && !columnHasType(index.column(), type))
		{
			if(strict && !ret->hasType(type))
				return LiveNode();
			else
			{
				ret->beginInterfaceModifications();
				QUrl uri = type.uri();
				ret->attachInterface(uri, *ret.entity());
				ret->endInterfaceModifications();
			}
		}
		return ret;
	}

	LiveNode LiveNodeModel::liveNode(QModelIndex const &index) const
	{
		return data(index, LiveNodeRole).value<LiveNode>();
	}

	LiveNode LiveNodeModel::liveNode(int row, int column, QModelIndex const &parent) const
	{
		return data(index(row, column, parent), LiveNodeRole).value<LiveNode>();
	}

	LiveNode LiveNodeModel::firstNode(int column, QModelIndex const &parent) const
	{
		if(rowCount())
			return data(index(0, column, parent), LiveNodeRole).value<LiveNode>();
		return LiveNode();
	}

	void LiveNodeModel::setAsModelInto(QAbstractItemView *view)
	{
		if(!view)
			return;
		view->setModel(this);
		sharedBy(view);
	}

	void LiveNodeModel::setAsModelInto(QAbstractProxyModel *proxy)
	{
		if(!proxy)
			return;
		proxy->setSourceModel(this);
		sharedBy(proxy);
	}
	void LiveNodeModel::sharedBy(QObject *sharer)
	{
		if(!sharer)
			return;
		sharer->setProperty
				( "SopranoLive_SharesModel"
				, QVariant::fromValue(sharedFromThis<LiveNodeModel>()));
	}
	void LiveNodeModel::noLongerSharedBy(QObject *sharer)
	{
		if(!sharer)
			return;
		QVariant var = sharer->property("SopranoLive_SharesModel");
		QSharedPointer<LiveNodeModel> model = var.value<QSharedPointer<LiveNodeModel> >();
		if(model == this)
			removeShare(sharer);
	}
	void LiveNodeModel::removeShare(QObject *sharer)
	{
		if(!sharer)
			return;
		sharer->setProperty("SopranoLive_SharesModel", QVariant());
	}

	QList<int> LiveNodeModel::findColumnsByName(QStringList const &column_headers) const
	{
		QList<int> ret;
		QHash<QString, int> lookup;
		for(int i = 0, iend = columnCount(); i != iend; ++i)
			lookup[headerData(i, Qt::Horizontal).toString()] = i;
		for(QStringList::const_iterator ci = column_headers.begin(), ciend = column_headers.end()
				; ci != ciend; ++ci)
		{
			QHash<QString, int>::const_iterator hit = lookup.find(*ci);
			ret.push_back(hit == lookup.end() ? -1 : *hit);
		}
		return ret;
	}

	LiveNodeModelPtr LiveNodeModel::subModel
			( QModelIndex const &topleft_inclusive
			, QModelIndex const &bottomright_exclusive) const
	{
		return const_cast<LiveNodeModel *>(this)->subModel
				(topleft_inclusive, bottomright_exclusive, QList<int>());
	}

	LiveNodeModelPtr LiveNodeModel::subModel
			( QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive
			, QStringList const &column_headers_mapping)
	{
		return subModel
				( topleft_inclusive, bottomright_exclusive
				, findColumnsByName(column_headers_mapping));
	}

	LiveNodeModelPtr LiveNodeModel::subModel(QStringList const &column_headers_mapping)
	{
		return subModel
				( createIndex(0,0), createIndex(rowCount(), columnCount())
				, findColumnsByName(column_headers_mapping));
	}

	ConstLiveNodeModelPtr LiveNodeModel::subModel
			( QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive
			, QList<int> const &columns_mapping) const
	{
		return const_cast<LiveNodeModel *>(this)->subModel
				( topleft_inclusive, bottomright_exclusive, columns_mapping);
	}

	ConstLiveNodeModelPtr LiveNodeModel::subModel
			( QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive
			, QStringList const &column_headers_mapping) const
	{
		return const_cast<LiveNodeModel *>(this)->subModel
				( topleft_inclusive, bottomright_exclusive
				, findColumnsByName(column_headers_mapping));
	}

	ConstLiveNodeModelPtr LiveNodeModel::subModel
			( QStringList const &column_headers_mapping) const
	{
		return const_cast<LiveNodeModel *>(this)->subModel
				( createIndex(0,0), createIndex(rowCount(), columnCount())
				, findColumnsByName(column_headers_mapping));
	}

	LiveNodeModel const *LiveNodeModel::plainModel() const
	{
		return const_cast<LiveNodeModel *>(this)->plainModel();
	}

	ConstLiveNodeModelPtr LiveNodeModel::childModel
			( QModelIndex const &index, RDFStrategyFlags flags
			, QSharedPointer<QObject> const &parent) const
	{
		return const_cast<LiveNodeModel *>(this)->childModel(index, flags, parent);
	}

	bool LiveNodeModel::columnHasType(int column, LiveNode const &type) const
	{
		Q_UNUSED(column); Q_UNUSED(type);
		return false;
	}

	bool LiveNodeModel::refreshRows(int row, int count, QModelIndex const &parent)
	{
		return refreshRows(row, count, parent, Flush);
	}
	bool LiveNodeModel::refreshModel()
	{
		return refreshModel(Flush);
	}

	QDebug operator<<(QDebug s, LiveNodeModel const &value)
	{
		{
			QStringList list;
			s << "LiveNodeModel(" << &value << "):\n";
			for(int h = 0, hend = value.columnCount(); h != hend; ++h)
				list.push_back(value.headerData(h, Qt::Horizontal).toString());
			s << "\t" << list.join("\t|") << "\n";
		}
		int rend = 0;
		if(RowStoreModel const *rsm = dynamic_cast<RowStoreModel const *>(&value))
			rend = rsm->row_store.size();
		else
			rend = value.rowCount();
		for(int r = 0; r != rend; ++r)
		{
			QStringList list;
			for(int c = 0, cend = value.columnCount(); c != cend; ++c)
				list.push_back(value.data(value.index(r, c)).toString());
			s << "\t" << list.join("\t|") << "\n";
		}
		return s;
	}


	bool ModelCommons::isAdoptable_chain
			( QSharedPointer<RDFGraphImplBase> const &new_model_parent
			, InterfaceChain<ModelCommons const> chain) const
	{
		return chain.advance(*this)
				? chain->isAdoptable_chain(new_model_parent, chain)
				: false;
	}

	void ModelCommons::adopted_chain
			( QSharedPointer<RDFGraphImplBase> const &new_model_parent
			, InterfaceChain<ModelCommons> chain)
	{
		if(chain.advance(*this))
			chain->adopted_chain(new_model_parent, chain);
	}

	RDFStrategyFlags ModelCommons::strategy_chain
			( QModelIndex const &index, InterfaceChain<ModelCommons const> chain) const
	{
		return chain.advance(*this)
				? chain->strategy_chain(index, chain)
				: RDFStrategy::Disabled;
	}

	/*!
	 * Implementation function for \ref LiveNodeModel::alterStrategy. The implementation
	 * guideline for overrides has three phases: Diagnostics, Delegation and Implementation.
	 * \n In Diagnostics phase, the override checks if the \a mask has any strategies the
	 * module is responsible for. Then for all responsibilities, it checks respective \a flags
	 * for if it can implement the requested changes. If not, a warning should be emitted with
	 * \ref LiveNodeModel::warning containing the offending mask and flag, and the override
	 * must return false at the end of diagnostics phase. If all requests the
	 * override is responsible can be implemented, the override now commits to them.
	 * \n In Delegation phase, override delegates the remaining state change requests to
	 * the base class alterStrategy function, with the responsibilities masked out from the
	 * \a mask. Thus further modules will not try to implement them. If the delegated call
	 * returns false, the override must return false as well. This means that the requested
	 * state change could not be fully implemented. If the call returns true, the override
	 * carries out its commitment to implement its own responsibilities.
	 * \n In Execution phase, the override makes all necessary changes to implement its own
	 * responsibilities, and returns true.
	 * \code
bool MMS_Simple_TextQuery::alterStrategy_chain
( RDFStrategyFlags flags, RDFStrategyFlags mask, QModelIndex const &index
, InterfaceChain<ModelCommons> chain)
{
    debug(2, model()) << model()->strategy();

// diagnostics
RDFStrategyFlags running_resp = mask & RDFStrategy::Running;

if(running_resp & flags & ~model()->strategy(index))
{
    warning(model()) << running_resp << (flags & running_resp) << index << "re-enabling running not supported";
	return false;
}

// delegation
if(!ModelContentStrategy_Simple::alterStrategy_chain
		(flags, mask & ~running_resp, index, chain))
	return false;

// implementation
if(running_resp & ~flags & model()->strategy(index))
{
    debug(2, model()) << flags << mask << "turning off Running";

	model()->doEnableStrategyFlags(RDFStrategy::Running);

	if( ongoingID_ )
	{
        debug(2, service_.data()) << flags << mask
				<< "do abortion for query ID" << ongoingID_;
		service_->resource.cancelQuery( ongoingID_ );
	}
}
return true;
}
	 * \endcode
	 */
	bool ModelCommons::alterStrategy_chain
			( RDFStrategyFlags flags, RDFStrategyFlags mask, QModelIndex const &index
			, InterfaceChain<ModelCommons> chain)
	{
		return chain.advance(*this)
				? chain->alterStrategy_chain(flags, mask, index, chain)
				: mask == 0;
	}

	enum
	{
		ModelReady = RDFStrategy::ModelReady
	,	DerivedsReady = RDFStrategy::DerivedsReady
	,	AllReady = ModelReady | DerivedsReady
	};
	typedef RDFStrategyFlags ReadyFlags;

	ReadyFlags ModelCommons::refreshModel_chain(LiveNodeModel::RefreshMode mode
			, InterfaceChain<ModelCommons> chain)
	{
		return chain.advance(*this)
				? chain->refreshModel_chain(mode, chain)
				: ReadyFlags(AllReady);
	}

	ReadyFlags ModelCommons::refreshRows_chain(int row, int count, QModelIndex const &parent
			, LiveNodeModel::RefreshMode mode, InterfaceChain<ModelCommons> chain)
	{
		return chain.advance(*this)
				? chain->refreshRows_chain(row, count, parent, mode, chain)
				: ReadyFlags(AllReady);
	}

	/*!
	 * Called when the given \a update_context performs an update with this model as target.
	 * Modules should override this function and make sure they are updated as well, and call
	 * the \ref updateTargetModule on themselves against the model inside the update_context.
	 */
	bool ModelCommons::updateFrom_chain
			( UpdateContext const &update_context, InterfaceChain<ModelCommons> chain)
	{
		return chain.advance(*this)
				? chain->updateFrom_chain(update_context, chain)
				: false;
	}

	/*!
	 * Called when the compositor model of given \a target_module is being updated with this
	 * model as the source. Any attached modules able to perform per-module updates override
	 * this and check if they can update the target. Most typical scenario is that a module of
	 * source model updates the module of the same type in target model. Notably used by
	 * DerivedColumnStrategy to distribute the update to all child models.
	 */
	bool ModelCommons::updateTargetModule_chain
			( ModelModule *target_module, UpdateContext const &update_context
			, InterfaceChain<ModelCommons const> chain) const
	{
		return chain.advance(*this)
				? chain->updateTargetModule_chain(target_module, update_context, chain)
				: false;
	}

	QVariant ModelCommons::modelAttribute_chain(QString const &attribute_name
			, InterfaceChain<ModelCommons const> chain) const
	{
		return chain.advance(*this)
				? chain->modelAttribute_chain(attribute_name, chain)
				: QVariant();
	}
	bool ModelCommons::setModelAttribute_chain(QString const &attribute_name
			, QVariant const &value, InterfaceChain<ModelCommons> chain)
	{
		return chain.advance(*this)
				? chain->setModelAttribute_chain(attribute_name, value, chain)
				: false;
	}



	LiveNodeModelBase::LiveNodeModelBase
			( QSharedPointer<RDFGraphImplBase> const &graph, RDFStrategyFlags strategy)
		: graph_(graph)
		, strategy_(strategy & ModelStrategyStateMask)
	{
		//debug(4, graph_.data()) << "LiveNodeModelBase LNMB(" << this << ") created";
	}

	LiveNodeModelBase::~LiveNodeModelBase()
	{
		detachAllCompositorModules();
		//debug(4, graph_.data()) << "LiveNodeModelBase LNMB(" << this << ") destroyed";
	}


	int LiveNodeModelBase::columnCount(const QModelIndex &) const
	{
		return columns_.visibleColumnCount();
	}

	QVariant LiveNodeModelBase::headerData
			( int section, Qt::Orientation orientation, int role) const
	{
		if(orientation == Qt::Horizontal)
		{
			if(role == Qt::DisplayRole || role >= Qt::UserRole)
				return columns_[section].name();
		}
		return QVariant();
	}

	bool LiveNodeModelBase::canFetchMore(QModelIndex const &parent) const
	{
		return !(strategy(parent) & RDFStrategy::ModelReady);
	}

	void LiveNodeModelBase::fetchMore(QModelIndex const &parent)
	{
		if(enableStrategyFlags(RDFStrategy::Running, parent))
			refreshRows(rowCount(parent), 1, parent, LiveNodeModel::Block);
	}

	RDFGraphImplBase *LiveNodeModelBase::graph() const
	{
		return graph_.data();
	}

	RDFVariableList LiveNodeModelBase::sourceVariables() const
	{
		// TODO: perform column hiding
		return columns_.variables();
	}

	bool LiveNodeModelBase::columnHasType(int column, LiveNode const &type) const
	{
		return column < column_types_.size()
				&& column_types_[column].contains(type.toString());
	}

	RDFStrategyFlags LiveNodeModelBase::strategy_chain
			( QModelIndex const &index, InterfaceChain<ModelCommons const> chain) const
	{
		CallChainCache chain_cache = chain.primeCompositorCall(this);
		if(chain_cache.size())
			return ModelCommons::strategy_chain(index, chain);

		return isWholeIndex(index)
				? unsigned(strategy_)
				: unsigned(RDFStrategy::Disabled);
	}
	RDFStrategyFlags LiveNodeModelBase::strategy(QModelIndex const &index) const
	{
		return strategy_chain(index);
	}

	bool LiveNodeModelBase::alterStrategy_chain
			( RDFStrategyFlags flags, RDFStrategyFlags mask, QModelIndex const &index
			, InterfaceChain<ModelCommons> chain)
	{
		CallChainCache chain_cache = chain.primeCompositorCall(this);
		if(chain_cache.size())
			return ModelCommons::alterStrategy_chain(flags, mask, index, chain);

		if(((strategy(index) ^ flags) & mask) == 0)
			return true;
		warning(this, "LiveNodeModel::alterStrategy") << "requested state change not supported";
		return false;
	}
	bool LiveNodeModelBase::alterStrategy
			( RDFStrategyFlags flags, RDFStrategyFlags mask, QModelIndex const &index)
	{
		return alterStrategy_chain(flags, mask, index);
	}

	bool LiveNodeModelBase::doAlterStrategy
			( RDFStrategyFlags changed_flags, RDFStrategyFlags changed_mask
			, QModelIndex const &index)
	{
		if(isWholeIndex(index))
		{
			RDFStrategyFlags new_strategy
				= (strategy_ & ~changed_mask) | (changed_flags & changed_mask);
			changed_mask = strategy_ ^ new_strategy;
			changed_flags = new_strategy & changed_mask;
			strategy_ = new_strategy;
		}
		if(!changed_mask)
			return false;

		sharedFromThis(), Q_EMIT strategyAltered(changed_mask, changed_flags, index);
		return true;
	}

	RDFStrategyFlags LiveNodeModelBase::refreshModel_chain
			(LiveNodeModel::RefreshMode mode, InterfaceChain<ModelCommons> chain)
	{
		CallChainCache chain_cache = chain.primeCompositorCall(this);
		if(chain_cache.size())
		{
			// we're just entering the refreshModel_chain, delegate the call to the chain
			RDFStrategyFlags res = ModelCommons::refreshModel_chain(mode, chain);
			if(res & RDFStrategy::ModelReady)
			{
				// if the chain returns ModelReady, reflect it in state.
				// if state changes, emit signals.
				if(doEnableStrategyFlags(RDFStrategy::ModelReady))
				{
					if(mode & LiveNodeModel::Block)
						doEmitModelUpdated();
					else
						QMetaObject::invokeMethod(this, "doEmitModelUpdated", Qt::QueuedConnection);
				}
				if(res & RDFStrategy::DerivedsReady
						&& doEnableStrategyFlags(RDFStrategy::DerivedsReady))
				{
					if(mode & LiveNodeModel::Block)
						doEmitDerivedsUpdated();
					else
						QMetaObject::invokeMethod(this, "doEmitDerivedsUpdated", Qt::QueuedConnection);
				}
			}
			return res & RDFStrategy::ModelReady
				&& (!(mode & CoverDerivedProperties) || res & RDFStrategy::DerivedsReady);
		}
		// Reached the end of the chain.
		// If not flushing the contents, return the current readiness status of the model.
		// Chain overrides may disable or enable these flags, and they will ultimately
		// become the new state of the model, with proper signals emitted.

		if(!(mode & LiveNodeModel::Flush))
			return strategy_ & AllReady;

		// Flushing: disable the ready flags, but return AllReady as well so by default
		// the model becomes ready once refreshModel chain is exited
		doDisableStrategyFlags(AllReady);
		return AllReady;
	}
	bool LiveNodeModelBase::refreshModel(LiveNodeModel::RefreshMode mode)
	{
		return refreshModel_chain(mode);
	}

	RDFStrategyFlags LiveNodeModelBase::refreshRows_chain(int row, int count
			, QModelIndex const &parent, RefreshMode mode, InterfaceChain<ModelCommons> chain)
	{
		CallChainCache chain_cache = chain.primeCompositorCall(this);
		if(chain_cache.size())
			return ModelCommons::refreshRows_chain(row, count, parent, mode, chain);

		warning() << "refreshRows not implemented, falling back to whole model refreshModel";

		return refreshModel(mode);
	}
	bool LiveNodeModelBase::refreshRows
			(int row, int count, QModelIndex const &parent, RefreshMode mode)
	{
		return refreshRows_chain(row, count, parent, mode);
	}

	void LiveNodeModelBase::doEmitModelUpdated()
	{
		sharedFromThis(), Q_EMIT modelUpdated();
	}
	void LiveNodeModelBase::doEmitDerivedsUpdated()
	{
		sharedFromThis()
				, Q_EMIT derivedsUpdated()
				, Q_EMIT cacheUpdated(); // on ABI break: this should be removed along with the public API
	}
	void LiveNodeModelBase::doEmitError(QString const &message, RDFStrategyFlags mask
			, RDFStrategyFlags flags, QModelIndex const &index)
	{
		sharedFromThis(), Q_EMIT error(message, mask, flags, index);
	}

	void LiveNodeModelBase::emitErrorImpl
			( QString const &message, RDFStrategyFlags mask
			, QModelIndex const &index, bool queued)
	{
		emitErrorImpl(message, mask, strategy() & mask, index, queued);
	}

	void LiveNodeModelBase::emitErrorImpl
			( QString const &message, RDFStrategyFlags mask, RDFStrategyFlags flags
			, QModelIndex const &index, bool queued)
	{
		if( queued )
		{
			QMetaObject::invokeMethod(this, "doEmitError", Qt::QueuedConnection,
									  Q_ARG(QString, message),
									  Q_ARG(RDFStrategyFlags, mask),
									  Q_ARG(RDFStrategyFlags, flags),
									  Q_ARG(QModelIndex, index));
		}
		else
			doEmitError(message, mask, flags, index);
	}

	void LiveNodeModelBase::emitAlterStrategyError
			( QString const &message, RDFStrategyFlags mask, RDFStrategyFlags flags
			, QModelIndex const &index)
	{
		doEmitError(message, mask | LiveNodeModel::AlterStrategyError, flags, index);
	}

	LiveNodeModel *LiveNodeModelBase::plainModel()
	{
		return new LiveNodeModelBridge<>(sharedFromThis<LiveNodeModel>());
	}

	LiveNodeModelPtr LiveNodeModelBase::childModel
			( QModelIndex const &index, RDFStrategyFlags flags
			, QSharedPointer<QObject> const &parent)
	{
		// TODO: unused params
		Q_UNUSED(index); Q_UNUSED(flags); Q_UNUSED(parent);
		return LiveNodeModelPtr();
	}

	QVariant LiveNodeModelBase::modelAttribute_chain(QString const &attribute_name
			, InterfaceChain<ModelCommons const> chain) const
	{
		CallChainCache chain_cache = chain.primeCompositorCall(this);
		if(chain_cache.size())
			return ModelCommons::modelAttribute_chain(attribute_name, chain);

		QVariantMap::const_iterator cia = model_attributes_.find(attribute_name);
		if(cia != model_attributes_.end())
			// found property, return it
			return *cia;
		// otherwise, see if the service has a default for us
		return graph()->service()->serviceAttribute(attribute_name);
	}
	QVariant LiveNodeModelBase::modelAttribute(QString const &attribute_name) const
	{
		return modelAttribute_chain(attribute_name);
	}

	QVariantMap LiveNodeModelBase::modelAttributes() const
	{
		return model_attributes_;
	}

	bool LiveNodeModelBase::setModelAttribute_chain(QString const &attribute_name
			, QVariant const &value, InterfaceChain<ModelCommons> chain)
	{
		CallChainCache chain_cache = chain.primeCompositorCall(this);
		if(chain_cache.size())
			return ModelCommons::setModelAttribute_chain(attribute_name, value, chain);

		setProperty(("SopranoLive::" + attribute_name).toAscii().data(), value);
		model_attributes_[attribute_name] = value;
		return true;
	}
	bool LiveNodeModelBase::setModelAttribute
			(QString const &attribute_name, QVariant const &value)
	{
		return setModelAttribute_chain(attribute_name, value, InterfaceChain<ModelCommons>());
	}


	bool LiveNodeModelBase::isAdoptable_chain
			( QSharedPointer<RDFGraphImplBase> const &new_parent
			, InterfaceChain<ModelCommons const> chain) const
	{
		CallChainCache chain_cache = chain.primeCompositorCall(this);
		if(chain_cache.size())
			return ModelCommons::isAdoptable_chain(new_parent, chain);
		return true;
	}

	void LiveNodeModelBase::adopted_chain
			( QSharedPointer<RDFGraphImplBase> const &new_parent
			, InterfaceChain<ModelCommons> chain)
	{
		CallChainCache chain_cache = chain.primeCompositorCall(this);
		if(chain_cache.size())
			return ModelCommons::adopted_chain(new_parent, chain);
		graph_ = new_parent;
	}

	bool LiveNodeModelBase::adopt(QSharedPointer<RDFGraphImplBase> const &new_parent)
	{
		if(!isAdoptable_chain(new_parent))
			return false;
		adopted_chain(new_parent);
		return true;
	}

	//! After all modules are updated, update ourselves as well.
	bool LiveNodeModelBase::updateFrom_chain
			(UpdateContext const &update_context, InterfaceChain<ModelCommons> chain)
	{
		CallChainCache chain_cache = chain.primeCompositorCall(this);
		if(chain_cache.size())
			return ModelCommons::updateFrom_chain(update_context, chain);

		return update_context.updateModel(this);
	}

	bool LiveNodeModelBase::updateFrom(UpdateContext const &update_context)
	{
		return updateFrom_chain(update_context);
	}

	bool LiveNodeModelBase::updateTargetModule_chain
			( ModelModule *target_module, UpdateContext const &update_context
			, InterfaceChain<ModelCommons const> chain) const
	{
		CallChainCache chain_cache = chain.primeCompositorCall(this);
		if(chain_cache.size())
			return ModelCommons::updateTargetModule_chain(target_module, update_context, chain);

		//! No updater found for module
		return false;
	}

	bool LiveNodeModelBase::updateTargetModule
			( ModelModule *target_module, UpdateContext const &update_context) const
	{
		return updateTargetModule_chain(target_module, update_context);
	}

	QModelIndex LiveNodeModelBase::areaIndex(int row, int column, const QModelIndex &parent)
	{
		// TODO: parent unused
		Q_UNUSED(parent);
		return createIndex(row, column, 0);
	}

	//! Registers the given type and its bases to type_set
	void setColumnTypeInfo
			( QSet<QString> &type_set, QString type, AutoGen::BaseClassInfo const &bases)
	{
		if(type_set.contains(type))
			return;
		for(AutoGen::BaseClassInfo::const_iterator cbci = bases.begin(), cbciend = bases.end()
			; cbci != cbciend; ++cbci)
			setColumnTypeInfo(type_set, cbci.key().toString(), *cbci.value());
		type_set.insert(type);
	}

	void LiveNodeModelBase::setColumnInformation(RDFDerivedSelect const &columns)
	{
		int cols = columns.realColumnCount();
		column_types_.clear();
		column_types_.reserve(cols);
		// TODO: this is far too complex code for such a simple task
		for(int i = 0; i != cols; ++i)
		{
			QVector<RDFProperty> const &dps = columns[i].variable().derivedProperties();
			QSet<QString> type_set;
			for(QVector<RDFProperty>::const_iterator cdi = dps.begin(), cdiend = dps.end()
					; cdi != cdiend; ++cdi)
				if( (cdi->strategy() & RDFStrategy::Object)
						&& cdi->target().metaIsDefinite()
						&& cdi->predicate().metaValue().toString()
								== rdf_type_as_list_g[0])
				{
					QVariantList const bcs = cdi->target().metaAttribute
							("SopranoLive::baseClassInfo");
					for(QVariantList::const_iterator cbci = bcs.begin(), cbciend = bcs.end()
							; cbci != cbciend; ++cbci)
						setColumnTypeInfo
								( type_set
								, cdi->target().metaValue().toString()
								, cbci->value<AutoGen::BaseClassInfo>());
				}
			column_types_.push_back(SharedTypeSet::embedSet
					(graph_->service_context_data_->shared_type_storage_, type_set));
		}
		columns_ = columns;
	}

	LogContext const *LiveNodeModelBase::parentContext() const
	{
		return graph();
	}

	QDebug LiveNodeModelBase::logMessage(int level, char const *func, char const *loc) const
	{
		QDebug log = parentContext()->logMessage(level, func, loc);
		log.nospace() << "[LiveNodeModel(" << (void*)this << ") strategy:"
					  << QString::number(strategy_, 16) << "]";
		return log.space();
	}
}
