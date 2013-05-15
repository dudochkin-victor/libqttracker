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
 * livenodemodel.h
 *
 *  Created on: Feb 6, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_LIVENODEMODEL_H_
#define SOPRANOLIVE_LIVENODEMODEL_H_

#include <QtGui/QAbstractProxyModel>
#include <QtGui/QAbstractItemView>
#include <QtCore/QVector>
#include <QtCore/QPair>

#include "livebag.h"
#include "QSharedObject"

#include <functional>
namespace SopranoLive
{
	/*!
	 *  \ref live_entities "Live entity".
	 *  \n Models any concept which can be expressed as a set of \ref SopranoLive::LiveNode objects. Offers methods to access and manipulate them as a table through a QAbstractItemModel interface.
	 *  Use \ref LiveNodeList for convenient access through a QList -like interface.
	 *	\code
		LiveNodes mydata = node->getObjects();
		LiveNodeList list_view = mydata;
		for(LiveNodeList::iterator i = list_view.begin(); i != list_view.end(); ++i)
			qDebug() << i->toString();
	 *	\endcode
	 *
	 *	LiveNodeModel captures two major concepts: modeling query result sets, and modeling multiple valued LiveNode properties.
	 *	\n In former case the model is read-only. In latter case, model can be altered, and these changes will be readily reflected in the rdf service graph as well (TODO NOT currently IMPLEMENTED, so in fact read-only).
	 *
	 *	LiveNodeModel also uses the Qt signals and slots to report changes that happen on the rdf service graph, for both queries and properties (TODO also NOT currently IMPLEMENTED).
	 *
	 *	Just like \ref LiveResource objects, \ref LiveNodeModel is implemented and returned to the user by the service.
	 *	The service also keeps the ownership of the object, but gives user some control over the lifetime by use of shared pointers.
	 *	\n \ref LiveNodeList class should be used only when really needed.
	 * \sa \ref layer_unbound
	 */
	class Q_DECL_EXPORT LiveNodeModel
		: public QSharedObject<QAbstractItemModel>
		, public RDFStrategy
	{

		Q_OBJECT
	public:

		enum DataRole_offsets
		{
			offset_DataRoleCache = 22, offset_OverrideDataRoleCache
		};

		//! Additional data roles specified by LiveNodeModel
		enum DataRole
		{
			OldLiveNodeRole = Qt::UserRole

			// returns a QVariant::value<LiveNode>()
		,	LiveNodeRole = Qt::UserRole + 0x7f000000

			// returns a QVariant::value<Node>
		,	NodeRole

			// returns QVariant::value<QString> in a raw format returned by the backend.
		,	RawDataRole

			// returns a QVariant::value<QObject *>, containing a pointer to a valid QAbstractItemModel.
			// User handles child model lifetime. Identical to calling ChildLiveNodesRole and plainModel on the returned LiveNodes object.
		, 	ChildModelQObjectRole

			// returns a QVariant::value<LiveNodes>. Model lifetime handled as usual.
		,	ChildModelRole

			// Role flags. These alter internal behaviour of the role but do not affect returned values.

			// model strategy for data role caching is overridden
		,	OverrideDataRoleCache = 1 << offset_OverrideDataRoleCache

			// register this particular data role item in the data role cache
		,	DataRoleCache = OverrideDataRoleCache | (1 << offset_DataRoleCache)

			// do not register this data role item in the data role cache
		,	NoDataRoleCache = OverrideDataRoleCache

			// Flags in the role which alter the behaviour of the role
		,	RoleFlags = DataRoleCache
		, 	OverrideFlags = OverrideDataRoleCache
		};

		static bool matchStrategy(RDFStrategyFlags lhs, RDFStrategyFlags rhs, RDFStrategyFlags mask = ~RDFStrategyFlags(0));

		/*!
		 * \return the parent \ref RDFGraph of this model.
		 */
		virtual RDFGraph *graph() const = 0;


		/*!
		 * \return strategy for the given \a index, or the strategy of the whole model by default.
		 */
		virtual RDFStrategyFlags strategy(QModelIndex const &index = QModelIndex()) const = 0;

		/*!
		 * \return true if strategy allows model strategy allows for multiple rows; false otherwise
		 */
		bool isMultipleValued() const { return (strategy() & NonMultipleValued) != NonMultipleValued; }
		/*!
		 * \return true if strategy allows for model to be empty; false otherwise
		 */
		bool isOptionalValued() const { return (strategy() & NonOptionalValued) != NonOptionalValued; }
		/*!
		 * \return true if strategy specifies that model will contain exactly one row; false otherwise
		 */
		bool isSingleValued() const { return (strategy() & SingleValued) == SingleValued; }
		/*!
		 * \return true if strategy specifies that model will contain zero or one rows; false otherwise
		 */
		bool isOptionalSingleValued() const { return (strategy() & OptionalSingleValued) == OptionalSingleValued; }

		/*!
		 * \return the set of \ref RDFVariable objects that make up this model.
		 * Variables in unknown state mean no source information is known for the column.
		 * The cppType of those variables still applies, however.
		 */
		virtual RDFVariableList sourceVariables() const = 0;

		//! \return index representing the whole model
		QModelIndex wholeIndex() const;

		//! \return true if index is the whole index of this model; false otherwise
		bool isWholeIndex(QModelIndex const &index) const;

		/*!
		 * Parameterized base for liveNode class of functions.
		 * \return a LiveNode corresponding to the given \a index
		 * \param type if null, or if the returned LiveNode has the given
		 * \a type, no effect. Otherwise if \a strict is true, a null LiveNode
		 * is returned, or if false, the \a type is added to the returned resource.
		 */
		LiveNode liveNode(QModelIndex const &index, LiveNode const &type, bool strict) const;

		/*!
		 * \return the LiveNode in the location of given \a index.
		 * This is analoguous to calling data(index, LiveNodeRole) and
		 * getting the LiveNode value from the returned QVariant.
		 */
		LiveNode liveNode(QModelIndex const &index) const;

		//! \overload
		LiveNode liveNode(int row, int column = 0, QModelIndex const &parent = QModelIndex()) const;

		inline LiveNode liveNode(QModelIndex const &index, LiveNode const &ensured_type) const;

		inline LiveNode liveNode(int row, int column, QModelIndex const &parent, LiveNode const &ensured_type) const;
		/*!
		 * \return the LiveNode in the location of given \a index.
		 * This is analoguous to calling data(index, LiveNodeRole) and
		 * getting the LiveNode value from the returned QVariant.
		 */
			template<typename Type_>
		inline SopranoLive::Live<Type_> liveResource(QModelIndex const &index, bool statically_known_type = false) const;

			template<typename Type_>
		inline SopranoLive::Live<Type_> liveResource(int row, int column = 0, QModelIndex const &parent = QModelIndex(), bool statically_known_type = false) const;

		//! \overload
		inline LiveNode strictLiveNode(QModelIndex const &index, LiveNode const &strict_type) const;

		//! \overload
		inline LiveNode strictLiveNode(int row, int column, QModelIndex const &parent, LiveNode const &strict_type) const;

			template<typename Type_>
		inline StrictLive<Type_> strictLiveResource(QModelIndex const &index, bool statically_known_type = false) const;

			template<typename Type_>
		inline StrictLive<Type_> strictLiveResource(int row, int column = 0, QModelIndex const &parent = QModelIndex(), bool statically_known_type = false) const;
		/*!
		 * \return the first LiveNode in the given row, or a invalid LiveNode
		 * if the model is empty.
		 */
		LiveNode firstNode(int column = 0, QModelIndex const &parent = QModelIndex()) const;

		/*!
		 * Sets this model as the model of the given \a view.
		 * Also calls sharedBy with \a view as sharer.
		 */
		void setAsModelInto(QAbstractItemView *view);
		/*!
		 * \overload
		 */
		void setAsModelInto(QAbstractProxyModel *proxymodel);

		/*!
		 * Sets the property "SopranoLive_SharesModel" of the given \a sharer
		 * to a shared pointer containing a reference to this model.
		 */
		void sharedBy(QObject *sharer);

		/*!
		 * If the property "SopranoLive_SharesModel" of the given \a sharer
		 * refers to this, removes the property
		 */
		void noLongerSharedBy(QObject *sharer);

		/*!
		 * Remove the property "SopranoLive_SharesModel" of the given \a sharer
		 */
		static void removeShare(QObject *sharer);


		/*!
		 * Modification functions
		 * @{
		 */

		/*!
		 * On models that support this, clears given \a count of rows starting from the given \a row.
		 * \return true if the nodes were successfully cleared; otherwise returns false.
		 */
		virtual bool clearRows(int row, int count, QModelIndex const &parent = QModelIndex()) = 0;

		using QAbstractItemModel::insertRows;
		/*!
		 * On models that support this, inserts the given \a rows before the given \a row.
		 * \return true if the rows were successfully inserted; otherwise returns false.
		 */
		virtual bool insertRows(int row, LiveNodes const &rows, QModelIndex const &parent = QModelIndex()) = 0;
		/*!
		 * On models that support this, replaces rows starting from the given \a row
		 * with the given \a rows .
		 * \return true if the nodes were successfully replaced; otherwise returns false.
		 */
		virtual bool replaceRows(int row, LiveNodes const &rows, QModelIndex const &parent = QModelIndex()) = 0;
		/*!
		 * On models that support this, transitions the given \a count of rows starting
		 * from the given \a row to the given \a rows .
		 * \param smooth_transition if true, unchanged items are determined with best effort
		 * and appropriate row inserts, removes and updates are emitted correctly; if false,
		 * efficient bulk transition with trivial (remove all, add all) signaling is performed.
		 * \return true if the nodes were successfully transitioned; otherwise returns false.
		 */
		virtual bool transitionRows(int row, int count, LiveNodes const &rows, bool smooth_transition = true, QModelIndex const &parent = QModelIndex()) = 0;

		/*!
		 * On models that support this, inserts the given \a raw_rows before the given \a row.
		 * The items in raw_rows shall be given with their raw string representation, see
		 * \ref RDFService::rawStringFromNode .
		 * \return true if the rows were successfully inserted; otherwise returns false.
		 */
		virtual bool insertRowsRaw(int row, QVector<QStringList> const &raw_rows, QModelIndex const &parent = QModelIndex()) = 0;
		/*!
		 * On models that support this, replaces rows starting from the given \a row
		 * with the given \a raw_rows . The items in raw_rows shall be given with their
		 * raw string representation, see \ref RDFService::rawStringFromNode .
		 * \return true if the rows were successfully replaced; otherwise returns false.
		 */
		virtual bool replaceRowsRaw(int row, QVector<QStringList> const &raw_rows, QModelIndex const &parent = QModelIndex()) = 0;
		/*!
		 * On models that support this, transitions the given \a count of rows starting
		 * from the given \a row to the given \a rows .
		 * The items in raw_rows shall be given with their raw string representation, see
		 * \ref RDFService::rawStringFromNode .
		 * \param smooth_transition if true, unchanged items are determined with best effort
		 * and appropriate row inserts, removes and updates are emitted correctly; if false,
		 * efficient bulk transition with trivial (remove all, add all) signaling is performed.
		 * \return true if the nodes were successfully transitioned; otherwise returns false.
		 */
		virtual bool transitionRowsRaw(int row, int count, QVector<QStringList> const &raw_rows, bool smooth_transition = true, QModelIndex const &parent = QModelIndex()) = 0;

		/*!
		 * Assigns the model data and source from the given \a model.
		 * \param smooth_transition if true, unchanged items are determined and appropriate row
		 * inserts, removes and updates are emitted correctly. If false, efficient bulk
		 * transition with trivial (remove all, add all) signaling is performed.
		 * \return true if transition was successful; otherwise returns false.
		 */
		virtual bool assignModel(LiveNodeModel const &model, bool smooth_transition = true) = 0;

		/*!
		 * @}
		 */

		enum RefreshModeFlags
		{
			//! Polls for the refresh signals to be emitted directly, without
			//! waiting for event queue. Signals will be emitted only once,
			//! but poll will return true if they have been emitted by a
			//! previous call.
			Poll = 0
			//! Flushes the requested content and requests for update.
		,	Flush = 1 << 0
			//! Blocks until the pending results are returned.
		,	Block = 1 << 1
			//! Flushes the requested content and blocks for update.
		,	BlockingFlush = Flush | Block
			//! Derived properties are included in the refresh. If block is specified, refresh
			//! will block until all derived properties are up to date, and return value will
			//! be false if any derived properties are not ready. Flushing will flush derived
			//! properties even if this flag is not set, although blocking and return value
			//! effects will remain.
		,	CoverDerivedProperties = 1 << 2
		};
		typedef int RefreshMode;

		// TODO: ABI break reorder
		/*! \fn virtual bool refreshRows(int row, int count, QModelIndex const &parent = QModelIndex(), RefreshMode mode = Flush) = 0
		 * Requests \a count rows from given \a row and all their derived
		 * properties to be refreshed according to given \a mode. The signal
		 * \ref dataChanged will be emitted once the rows are ready, after
		 * which calling \ref data for these rows will not block. The refresh
		 * may finish during the call and emit the signal immediately.
		 * \return true if the signal was emitted during the call; false
		 * otherwise.
		 */
		virtual bool refreshRows(int row, int count, QModelIndex const &parent = QModelIndex());

		// TODO: ABI break reorder
		/*! \fn virtual bool refreshModel(RefreshMode mode = Flush) = 0
		 * Requests the whole model to be refreshed, according to given
		 * \a mode. The signal \ref modelUpdated will be emitted once the
		 * model is ready, after which calling \ref data will not block. The
		 * refresh may finish during the call and emit signal immediately.
		 * \return true if the model is up to date, implying that signal
		 * has already been emitted during this or earlier calls.
		 */
		virtual bool refreshModel();

		/*!
		 * \return a sub model representing the area between given
		 * \a topleft_inclusive and \a bottomright_exclusive indices
		 * if sub models are supported by the strategy; otherwise null.
		 */
		virtual LiveNodeModelPtr subModel(QModelIndex const &topleft_inclusive = QModelIndex(), QModelIndex const &bottomright_exclusive = QModelIndex()) const;

		//! \return true if the given \a column has the given \a type; false otherwise
		virtual bool columnHasType(int column, LiveNode const &type) const;

		//! \overload
			template<typename Type_>
		bool columnHasType(int column) { return columnHasType(column, Type_::iri()); }


		/*!
		 * Changes strategy flags specified by the given \a mask to the given \a flags.
		 * If successful, properties under \a mask will correspond to the given \a flags.
		 * Properties not masked by \a mask can freely change as a part of a successful operation.
		 * Otherwise, no changes are made and state remains as it was before the call.
		 * Note that if the existing state matches the requested state, the operation will be
		 * successfull even if the state doesn't change.
		 * \return true if operation was successful; false otherwise.
		 */
		virtual bool alterStrategy(RDFStrategyFlags flags, RDFStrategyFlags mask, QModelIndex const &index = QModelIndex()) = 0;

		/*!
		 * Set the exact strategy.
		 * \sa changeStrategy
		 */
		bool setStrategy(RDFStrategyFlags flags, QModelIndex const &index = QModelIndex())
		{ return alterStrategy(flags, ~RDFStrategyFlags(), index); }

		/*!
		 * Enable a set of strategy flags.
		 * \sa changeStrategy
		 */
		bool enableStrategyFlags(RDFStrategyFlags flags, QModelIndex const &index = QModelIndex())
		{ return alterStrategy(flags, flags, index); }

		/*!
		 * Disable a set of strategy flags.
		 * \sa changeStrategy
		 */
		bool disableStrategyFlags(RDFStrategyFlags flags, QModelIndex const &index = QModelIndex())
		{ return alterStrategy(~flags, flags, index); }

		/*!
		 * Stop all running operations. Alias for calling disableStrategyFlags(Running);
		 */
		bool stopOperations()
		{ return disableStrategyFlags(Running); }

		friend QDebug Q_DECL_EXPORT operator<<(QDebug s, LiveNodeModel const &value);

	Q_SIGNALS:
		/*!
		 * Emitted when the model is updated and ready to be used.
		 * All methods except \ref data are guaranteed not to block unless
		 * otherwise specified by the model provider.
		 * \see refreshModel.
		 */
		void modelUpdated();

		/*!
		 * Emitted when given \a count rows beginning from \a row are ready
		 * to be used. Differs from QAbstractItemModel::dataChanged in that
		 * rowsUpdated might be emitted even if no data was actually changed,
		 * in response to refreshRows request, whereas dataChanged will only
		 * be emitted only for actual changes. Postcondition: \ref data is
		 * guaranteed not to block when called for model indexes in this
		 * region.
		 * \see refreshRows.
		 */
		void rowsUpdated(int row, int count, QModelIndex const &parent);

		/*!
		 * Emitted when all requested derived properties are ready to be used.
		 * Specifically, if there are no derived properties for this model,
		 * this is always emitted along modelUpdated. Otherwise, it is
		 * emitted when all models describing derived properties of this
		 * model have emitted derivedsUpdated. Implies modelUpdated has been emitted.
		 */
		void derivedsUpdated();
		// DEPRECATED, use derivedsUpdated instead.
		void cacheUpdated();

		/*!
		 * Emitted for each strategy state change. When a strategy change is requested
		 * using alterStrategy (directly or implicitly), the request might get broken
		 * down to a sequence of state change operations, which in the end result
		 * in the final flag state.
		 * \param changed_mask the strategy flags that were affected
		 * \param changed_flags the new flags state of the affected strategy flags
		 * \param index item that was affected
		 */
		void strategyAltered(RDFStrategyFlags changed_mask, RDFStrategyFlags changed_flags, QModelIndex const &index);

		/*!
		 * Emitted in error circumstances.
		 * \param message the error message
		 * \param mask mask of the strategy flags the error relates to
		 * \param flag strategy flags state of the error. If the error was caused by a state
		 * change request, this flag contains the requested flag that caused the failure.
		 * \param index item that was affected
		 */
		void error(QString const &message, RDFStrategyFlags mask, RDFStrategyFlags flags, QModelIndex const &index);

	public:
		// TODO: ABI break reorder

		//! \return a list of column indices corresponding to requested \a column_headers.
		QList<int> findColumnsByName(QStringList const &column_headers) const;

		/*!
		 * \return a sub model representing the area between given \a topleft_inclusive
		 * and \a bottomright_exclusive indices if sub models are supported by the
		 * strategy; otherwise null.
		 * If any of the column or row parameters are invalid, they are interpreted as end of
		 * the line markers. Any insertions happening on the end of the line in target model
		 * will be visible in the submodel, as opposed to giving specific values.
		 * \param columns_mapping A mapping of columns from the sub model to the target model.
		 * If not specified, no mapping is performed. Each index in the mapping corresponds to a
		 * column in the sub model, with the value of the mapping corresponding to the column in the target.
		 * Overrides the column information in topleft_inclusive and bottomright_exclusive.
		 * \sa findColumnsByName
		 */
		virtual LiveNodeModelPtr subModel(QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive, QList<int> const &columns_mapping) = 0;
		//! \overload
		LiveNodeModelPtr subModel(QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive, QStringList const &column_headers_mapping);
		//! \overload
		LiveNodeModelPtr subModel(QStringList const &column_headers_mapping);
		//! \overload
		ConstLiveNodeModelPtr subModel(QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive, QList<int> const &columns_mapping) const;
		//! \overload
		ConstLiveNodeModelPtr subModel(QModelIndex const &topleft_inclusive, QModelIndex const &bottomright_exclusive, QStringList const &column_headers_mapping) const;
		//! \overload
		ConstLiveNodeModelPtr subModel(QStringList const &column_headers_mapping) const;

		//! \return a non-shared proxy model to this model. User has full control over the model, and must make sure to delete it once done.
		virtual LiveNodeModel *plainModel() = 0;

		//! \overload
		LiveNodeModel const *plainModel() const;

		/*!
		 * \return a child model of the given index. This is identical to calling data()
		 * with ChildModelRole, except with more configurability.
		 * \param strategy flags requested of the model. Of note here is the
		 * RDFStrategy::CacheDataRoles flag which causes the returned model to be cached
		 * in the ChildModelRole role for the given index, thus affecting data() behaviour.
		 * \param parent shared parent
		 */
		virtual LiveNodeModelPtr childModel(QModelIndex const &index
				, RDFStrategyFlags flags = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) = 0;

		//! \overload
		ConstLiveNodeModelPtr childModel(QModelIndex const &index
				, RDFStrategyFlags flags = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) const;

		virtual bool refreshRows(int row, int count, QModelIndex const &parent = QModelIndex(), RefreshMode mode = Flush) = 0;
		virtual bool refreshModel(RefreshMode mode) = 0;

		//! \return service attribute data, this function is a generic
		//! way to obtain backend specific information
		virtual QVariant modelAttribute(QString const &attribute_name) const = 0;
		//! \return all service attributes.
		virtual QVariantMap modelAttributes() const = 0;
		//! Sets the given \a service_attribute to given \a value.
		//! \return true if the attribute was set, false otherwise.
		virtual bool setModelAttribute(QString const &attribute_name, QVariant const &value) = 0;

		//! \return the raw representation of the row data
		virtual QStringList rawRow(int row, QModelIndex const &parent = QModelIndex()) const = 0;
	};

	/*!
	 * A \ref LiveBag policy, which models a specific, run-time determined column to a list.
	 */
		template<typename Base = ValueOfLiveEntity_Policy>
	struct Q_DECL_EXPORT ListPolicy_ColumnDynamic
		: Base
	{

			template<typename This_>
		void destruct()
		{
			if(this->template md<This_>().value_)
			{
				delete this->template md<This_>().value_;
				this->template md<This_>().value_ = typename This_::ValuePointer();
			}
			this->template md<This_>().entity_ = typename This_::EntityPointer();
		}

			template<typename This_>
		int defaultColumn() const { return column; }

			template<typename This_, typename Result_, typename Inserter_>
		Result_ evaluateColumn(int column, Inserter_ inserter) const
		{
			Result_ ret;
			if(typename This_::EntityPointer model = this->template md<This_>().entity())
				for(int ri = 0, rend = model->rowCount(); ri != rend; ++ri)
					inserter(this->template md<This_>(), ret, model->liveNode(model->index(ri, column)), ri);
			return ret;
		}

		ListPolicy_ColumnDynamic(int column) : column(column) {}

		int column;
	};

	/*!
	 * A \ref LiveBag policy, which models a specific, compile-time determined column to a list.
	 */
		template<int column__, typename Base = ValueOfLiveEntity_Policy>
	struct Q_DECL_EXPORT ListPolicy_Column
		: ListPolicy_ColumnDynamic<Base>
	{	ListPolicy_Column() : ListPolicy_ColumnDynamic<Base>(column__) {} };


	/*!
	 * A \ref LiveBag policy, offers a custom functor based object construction.
	 */
	/*!
	 * Default \ref LiveBag policy. Maps the first column of the model into a list.
	 */
	struct Q_DECL_EXPORT NodeListPolicy
		: LiveBagPolicy
	{
			template<typename Resource_>
		struct ValuePolicy
			: ListPolicy_Column<0, typename LiveBagPolicy::ValuePolicy<Resource_> >
		{};
	};

		template<typename Functor_, typename BasePolicy_ = NodeListPolicy>
	struct Q_DECL_EXPORT NodeCreatorFunctorPolicy
		: BasePolicy_
	{
		Functor_ creator;

		NodeCreatorFunctorPolicy(Functor_ creator = Functor_()) : creator(creator) {}

			template<typename Resource_>
		struct ValuePolicy
			: BasePolicy_::template ValuePolicy<Resource_>
		{
				template<typename This_, typename Result_>
			Result_ createNode(LiveNode const &node, int row = 0) const
			{
				return this->template md<This_>().creator(qMakePair(node, row));
			}
				template<typename This_, typename Value_>
			typename This_::ValuePointer copyValue(Value_ const &value) const
			{
				typename This_::ValuePointer ret = new typename This_::Value();
				for(typename Value_::const_iterator i = value.constBegin(), iend = value.constEnd(); i != iend; ++i)
					ret->push_back(this->template md<This_>().template createNode<This_, typename This_::Value::value_type>(*i, ret->size()));
				return ret;
			}

		};
	};

#ifdef GENERATING_DOXYGEN_DOCUMENTATION
	/*!
	 * A smart pointer that points to a \ref LiveNodeModel object, and
	 * acts as a QList<LiveNode>. Handles the memory management for the
	 * \ref LiveNodeModel, and the conversion from it to the
	 * QList<LiveNode> sequence when requested. As a smart pointer, the
	 * \ref LiveNodeModel properties are accessed through -> notation, or
	 * the \ref LiveNodeModel can be explicitly retrieved with \ref model
	 * method.
	 */
	class LiveNodes
		: public LiveBag<LiveNode, NodeListPolicy>
	{
	public:
		typedef LiveBag<LiveNode, NodeListPolicy> LiveNodesBase;
		/*!
		 * Non-blocking.
		 * Construct a non-live resource list from the given set of resources.
		 * \param resources the set of resources to be encapsulated.
		 */
		LiveNodes(LiveNodeList const &resource = LiveNodeList())
			: LiveNodesBase(nodes)
		{}

		/*!
		 * Blocking state depends on the given \a policy construct method.
		 * Construct a live resource list from the given \ref LiveNodeModel.
		 * \param model the live model.
		 */
		LiveNodes(LiveNodeModelPtr model)
			: LiveNodesBase(model)
		{}

		/*!
		 * Non-blocking if \a other is non-live, else depends on the given \a policy construct method.
		 * Construct a resource list from an \a other \ref LiveNodes.
		 * The live state will be the same as with the \a other.
		 * \param other the other \ref LiveNodes.
		 * \param policy policy used to map the model into the list.
		 */
			template<typename OtherResourceType_, typename OtherPolicy_>
		LiveNodes(LiveBag<OtherResourceType_, OtherPolicy_> const &other)
			: LiveNodesBase(other)
		{}

		/*!
		 * \ref live_entities "Non-live blocking".
		 * \return the local values of the bag as a \ref QList<LiveNode>
		 */
		QList<LiveNode> const &LiveNodes::value() const { return LiveNodesBase::value(); }

		/*!
		 * \ref live_entities "Non-live blocking".
		 * \return the local values of the bag as a the given \ref Container_ type
		 */
			template<Container_>
		Container_ LiveNodes::toSequence() const { return LiveNodesBase::template toSequence<Container>(); }

		/*!
		 * \ref live_entities "Non-live non-blocking".
		 * \return a direct pointer to the model if live, or a null pointer otherwise.
		 */
		LiveNodeModel *model() const { return LiveNodesBase::model(); }

		/*!
		 * Assigns a live \a model into this resource list, making it live.
		 * Blocking state depends on the active policy reconstruct method.
		 */
		LiveNodes &operator=(LiveNodeModelPtr const &model) { LiveNodesBase::operator=(model); return *this; }

		/*!
		 * Assigns a non-live set \a nodes of items into this node list, making it non-live.
		 */
		LiveNodes &operator=(QList<LiveNode> const &nodes) { LiveNodesBase::operator=(nodes); return *this; }

		/*!
		 * Blocking state depends on the active policy reconstruct method.
		 * Assigns an \a other \ref LiveBag into this resource list.
		 * The resulting live state will be the same as with \a other.
		 * \param other the other list to be assigned.
		 */
			template<typename OtherResourceType_, typename OtherPolicy_>
		LiveNodes &operator=(LiveBag<OtherResourceType_, OtherPolicy_> const &other) { return LiveNodesBase::operator=(other); return *this; }

	};
#endif // Q_MOC_RUN
}
Q_DECLARE_METATYPE(SopranoLive::LiveNodeModelPtr)

#endif
