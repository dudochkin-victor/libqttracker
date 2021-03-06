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
 * rdfservice.h
 *
 *  Created on: Mar 30, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_RDFSERVICE_H_
#define SOPRANOLIVE_RDFSERVICE_H_

#include "liveresource.h"

namespace SopranoLive
{
	/*!
	 * A pure virtual interface to a specific graph within an rdf service.
	 * With \ref RDFService, acts as the main nexus between the \ref layer_bound "bound" 
	 * and \ref layer_unbound "unbound" parts of the base service layer.
	 */
	class Q_DECL_EXPORT RDFGraph
		: public QSharedObject<>
	{
	public:
        /**
         * \name General state functions
         */
        //@{

		/*!
		 * \return the parent service of this graph.
		 */
		virtual RDFServicePtr service() const = 0;

#if (QTTRACKER_SUPPORT_VER <= 3)
		/*!
		 * \deprecated Support version 0.3.x. Use \ref service()
		 */
		SOPRANO_DEPRECATED RDFServicePtr getService() const 
		{ qDebug() << "getService: DEPRECATED 0.3.x"; return service(); }

		/*!
		 * \deprecated Support version 0.3.x, use \ref graphIri()
		 */
		SOPRANO_DEPRECATED QUrl getGraphIri() const 
		{ qDebug() << "getGraphIri: DEPRECATED 0.3.x"; return graphIri(); }
#endif

		/*!
		 * \return graph iri.
		 */
		virtual QUrl graphIri() const = 0;


		/*!
		 * \return a unique iri generated by the service.
		 */
		virtual QUrl createUniqueIri(QString stem_text = QString()) = 0;

#ifdef GENERATING_DOXYGEN_DOCUMENTATION
		/*!
		 * Adds a prefix to the prefix set of this service.
		 */
		virtual void addPrefix(QString prefix, QUrl value) = 0;
		/*!
		 * \return a QUrl constructed by extending the given \a prefix in this set with the 
		 * given \a suffix.
		 */
		virtual QUrl prefixSuffix(QString prefix, QString suffix) = 0;
#endif // GENERATING_DOXYGEN_DOCUMENTATION

        //@}

        /**
         * \name Live resource access
         */
        //@{

		/*!
		 * \return a default constructed \ref LiveNode of the given \a type using the construction
		 * scheme of the service of this graph. The most typical use of this is the default 
		 * version, which generates a new, unique rdf resource with the type rdfs:Resource.
		 * In some problem domains, the uri should be of some canonical form, in which case the 
		 * uri should be created by the user, and passed to liveNode function instead, which 
		 * performs the creation. The \a type can be used to generate default dates using 
		 * CppType::of<QDateTime>() and other literals.
		 */
		virtual LiveNode createLiveNode(CppType type = CppType::of<QUrl>()) = 0;

		/*!
		 * \return a default constructed \ref LiveNode of the given \a type using the construction
		 * scheme of the service of this graph.
		 */
			template<typename Type_>
		Live<Type_> createLiveResource()
		{ return createLiveNode(); }

		// the implementation function (see \ref RDFService::implLiveNode)
		// for liveNode class of functions.
		// \return a LiveNode on this graph corresponding to given \a node
		// \param strict if true, no changes are performed but if requested
		// object doesnt exist or doesnt have the requested \a type, null
		// LiveNode is returned. If false, the requested object and \a type
		// are added to the graph, and valid reference is returned.
		// \param resource_contexts determines the definition contexts of
		// the requested \a node. If the backend has implemented any of the
		// contexts, it can be assume that the resource exists and has the
		// requested type without doing any further checking or updating.
		// On ABI break, relocate virtual RDFService::implLiveNode here
		LiveNode implLiveNode(LiveNode const &node, LiveNode const &type = LiveNode()
				, bool strict = false, QList<QUrl> const &resource_contexts = QList<QUrl>());

		/*!
		 * \return a \ref LiveNode object bound to this graph, corresponding to given \a node.
		 */
		virtual LiveNode liveNode(LiveNode const &node, bool strict = false);

		//! \overload
		LiveNode liveNode
				( LiveNode const &node, LiveNode const &type
				, QList<QUrl> const &resource_contexts = QList<QUrl>())
		{ return implLiveNode(node, type, false, resource_contexts); }

		/*!
		 * \return a LiveNodes model constructed explicitly from the given \a
		 * urls.  The model will contain the single-valued derived properties
		 * of \a object_info as columns.  The given \a urls will be updated to
		 * have the same type(s) as the \a object_info variable.
		 */
		LiveNodes liveNodes
			( LiveNodeList const &urls, RDFVariable const &object_info = RDFVariable()
			, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
			, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());

		/*!
		 * \return a LiveNodes model constructed explicitly from the given \a
		 * urls.  The model will contain the single-valued derived properties
		 * of \a object_info as columns.  If some URLs don't have the same
		 * type(s) as the \a object_info variable, they are not included in
		 * the returned model.
		 */
		LiveNodes strictLiveNodes
			( LiveNodeList const &urls, RDFVariable const &object_info = RDFVariable()
			, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
			, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());

		LiveNodes implLiveNodes
			( LiveNodeList const &urls, RDFVariable const &object_info
			, RDFStrategyFlags model_strategy, bool strict
			, QSharedPointer<QObject> const &parent);

		/*!
		 * \return a live resource object of a given \a LiveResource_ type bound to this graph, 
		 * corresponding to the given \a node.
		 */
			template<typename Type_>
		inline Live<Type_> liveResource
				( LiveNode const &node, QList<QUrl> const &resource_contexts = QList<QUrl>())
		{ return Live<Type_>(implLiveNode(node, Type_::iri(), false, resource_contexts), true); }

		/*!
		 * \return a live resource object of a given \a Resource_ type bound to this graph, 
		 * corresponding to the given \a node.
		 */
			template<typename Type_, typename OtherType_>
		inline Live<Type_> liveResource
				( Live<OtherType_> const &node
				, QList<QUrl> const &resource_contexts = QList<QUrl>())
		{ return Live<Type_>(implLiveNode(node, Type_::iri(), false, resource_contexts), true); }

		//! \return a live resource bound to this graph, corresponding to given \a Resource_
			template<typename Resource_>
		inline Live<typename Resource_::ResourceType> liveResource()
		{ 	
			return Live<typename Resource_::ResourceType>(implLiveNode
						(Resource_::iri(), Resource_::ResourceType::iri(), false
						, Resource_::ResourceContext::iriList()
					), true);
		}

		/*!
		 * \return a \ref LiveNode object bound to this graph, corresponding to the given \a node.
		 */
		LiveNode strictLiveNode(LiveNode const &node)
		{ return implLiveNode(node, LiveNode(), true); }

		//! \overload
		LiveNode strictLiveNode
				( LiveNode const &node, LiveNode const &type
				, QList<QUrl> const &resource_contexts = QList<QUrl>())
		{ return implLiveNode(node, type, true, resource_contexts); }

		/*!
		 * \return a strict live resource object of a given \a LiveResource_ type bound to this 
		 * graph, corresponding to the given \a node.
		 */
			template<typename Type_>
		inline StrictLive<Type_> strictLiveResource
				( LiveNode const &node, QList<QUrl> const &resource_contexts = QList<QUrl>())
		{ 
			return StrictLive<Type_>(implLiveNode
					(node, Type_::iri(), true, resource_contexts)
				, true); 
		}

		/*!
		 * \return a strict live resource object of a given \a Resource_ type bound to this graph,
		 * corresponding to the given \a node.
		 */
			template<typename Type_, typename OtherType_>
		inline StrictLive<Type_> strictLiveResource
				( Live<OtherType_> const &node
				, QList<QUrl> const &resource_contexts = QList<QUrl>())
		{ 
			return StrictLive<Type_>(implLiveNode
					(node, Type_::iri(), true, resource_contexts)
				, true); 
		}

		//! \return a strict live resource bound to this graph, corresponding to 
		//! given \a Resource_
			template<typename Resource_>
		inline Live<Resource_> strictLiveResource()
		{ 	
			return Live<typename Resource_::Type>(implLiveNode
					( Resource_::iri(), Resource_::ResourceType::iri(), true
					, Resource_::ResourceContext::iriList())
				, true);
		}

		LiveNode firstNode(RDFVariable const &var)
		{
			return modelVariable(var)->firstNode();
		}

			template<typename Type_>
		Live<Type_> firstResource(RDFVariable const &var = RDFVariable())
		{
			return OmniTypeLiveNode(firstNode(var.isOfType<Type_>()));
		}

		// TODO: on ABI break, move to correct place, earlier in this file, for higher 
		// documentation cohesion.
#ifndef GENERATING_DOXYGEN_DOCUMENTATION
		/*!
		 * Adds a prefix to the prefix set of this service.
		 */
		virtual void addPrefix(QString prefix, QUrl value) = 0;
		/*!
		 * \return a QUrl constructed by extending the given \a prefix in this set with the 
		 * given \a suffix.
		 */
		virtual QUrl prefixSuffix(QString prefix, QString suffix) = 0;
#endif // GENERATING_DOXYGEN_DOCUMENTATION

		/*!
		 * \return a \ref LiveNode object bound to this graph, using the given \a prefix of this 
		 * graph extended with the given \a suffix.
		 */
		inline LiveNode liveNode(QString prefix, QString suffix)
		{ 	return implLiveNode(prefixSuffix(prefix, suffix), LiveNode(), true); }

		/*!
		 * \return a live resource object of a given \a LiveResource_ type bound to this graph,
		 * using the given \a prefix of this graph extended with the given \a suffix.
		 */
			template<typename Resource_>
		inline Live<Resource_> liveResource(QString prefix, QString suffix)
		{	return Live<Resource_>(implLiveNode
					(prefixSuffix(prefix, suffix), Resource_::iri())
				, true); 
		}

		/*!
		 * \return a \ref LiveNode object bound to this graph, using the given \a prefix of this 
		 * graph extended with the given \a suffix.
		 */
		inline LiveNode strictLiveNode(QString prefix, QString suffix)
		{	return implLiveNode(prefixSuffix(prefix, suffix), LiveNode(), true); }

		/*!
		 * \return a strict live resource object of a given \a LiveResource_ type bound to this 
		 * graph, using the given \a prefix of this graph extended with the given \a suffix.
		 */
			template<typename Resource_>
		inline StrictLive<Resource_> strictLiveResource(QString prefix, QString suffix)
		{	return StrictLive<Resource_>(implLiveNode
					(prefixSuffix(prefix, suffix), Resource_::iri(), true)
				, true); 
		}

        /**
         * \name Direct graph manipulation
         */
        //@{

		/*!
		 * Inserts a triple defined by \a subject, \a predicate, and \a object in the graph.
		 */
		virtual void insertTriple
				( LiveNode const &subject, LiveNode const &predicate, LiveNode const &object) = 0;
		/*!
		 * Deletes a triple defined by \a subject, \a predicate, and \a object from the graph.
		 */
		virtual void deleteTriple
				( LiveNode const &subject, LiveNode const &predicate, LiveNode const &object) = 0;

		//! removes all resources constrained by the given \a variable from the graph
		void removeResources(RDFVariable const &variable);
		//@}

        /**
         * \name Query and variable modeling functions
         */
        //@{

		/*!
		 * Models the given \a var variable into a \a LiveNodeModel through this graph.
		 * \return a read-only \ref LiveNodes model of the variable in this graph.
		 */
		virtual LiveNodes modelVariable
				( RDFVariable const &var
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());

		/*!
		 * Models the given \a var variable into a \a LiveNodeModel through this graph.
		 * The template parameter \a Type_ is the the type the model attempts to use when asked 
		 * for data with \ref LiveNodeRole role.
		 * Side-effect: constraint var.isOfType<Type_> is set implicitly on \a var.
		 * \return a read-only LiveNodes to the model.
		 */
			template<typename Type_>
		LiveNodes modelVariable
				( RDFVariable const &var
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());

		/*!
		 * Models the given set of \a variables into a \a LiveNodeModel through this graph.
		 * \return a read-only \ref LiveNodes model of the variable in this graph.
		 */
		LiveNodes modelVariables
				( RDFVariableList const &variables
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());

		/*!
		 * Models the given \a select query string into a model through this graph.
		 * The headers for columns of the returned model will have be supplied separately by the 
		 * given \a columns parameter. The given \a model_strategy defines the execution and 
		 * caching policy of the returned model.
		 * /return a model of the query result set.
		 */
		virtual LiveNodes modelQuery
				( QString const &select, QStringList const &columns
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) = 0;

		/*!
		 * Models the given \a select query object into a model through this graph. The 
		 * given \a model_strategy defines the execution and caching policy of the returned model.
		 * /return a model of the query result set.
		 */
		virtual LiveNodes modelQuery
				( RDFSelect const &select
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) = 0;

		/*!
		 * Executes the given \a query through this graph. The given \a model_strategy defines 
		 * the execution and caching policy of the returned models.
		 * /return a list of models of the results of the subqueries of the query. See the 
		 * particular query documentation for the definition of subquery.
		 */
		virtual QList<LiveNodes> executeQuery
				( RDFQuery const &query
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) = 0;

        //@}

        /**
         * \name Typed resource modeling functions
         */
        //@{

		/*!
		 * \return a snapshot model of all resources of given \a type in this graph
		 * \param strategy \ref RDFStrategyFlags
		 * \param parent the aggregating parent object
		 */
		LiveNodes modelResources
				( LiveNode const &type, RDFStrategyFlags strategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());
		LiveNodes modelResources(LiveNode const &type)
		{
			return modelResources(type, RDFStrategy::DefaultStrategy);
		}

		/*!
		 * \return a snapshot model of a resources of given \a Resource_ type in this graph
		 * \param resource_info additional constraints and meta information on the requested 
		 * resources
		 * \param strategy optional \ref RDFStrategyFlags
		 * \param parent the aggregating parent object
		 */
			template<typename Resource_>
		inline LiveNodes modelResources
				( RDFVariable const &resource_info
				, RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())
		{
			RDFProperty resource_data(Resource_::iri(), resource_info, strategy, parent);
			return implModelResources(resource_data);
		}

		//! \overload with default empty resource_info
			template<typename Resource_>
		inline LiveNodes modelResources
				( RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())
		{
			return modelResources<Resource_>(RDFVariable(), strategy, parent);
		}


		/*!
		 * \return a live model of all resources of given \a type in this graph
		 * \param strategy \ref RDFStrategyFlags
		 * \param parent the aggregating parent object
		 */
		LiveNodes liveResources
				( LiveNode const &type, RDFStrategyFlags strategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());
		LiveNodes liveResources(LiveNode const &type)
		{
			RDFProperty resource_data(type, RDFVariable(), RDFStrategy::LiveStrategy);
			return implModelResources(resource_data);
		}

		/*!
		 * \return a snapshot model of a resources of given \a Resource_ type in this graph
		 * \param resource_info additional constraints and meta information on the requested 
		 * resources
		 * \param strategy optional \ref RDFStrategyFlags
		 * \param parent the aggregating parent object
		 */
			template<typename Resource_>
		inline LiveNodes liveResources
				( RDFVariable const &resource_info
				, RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())
		{
			RDFProperty resource_data
					( Resource_::iri(), resource_info
					, (strategy & ~LiveNodeModel::AllowLive) | LiveNodeModel::RequireLive
					, parent);
			return implModelResources(resource_data);
		}

		//! \overload with default empty resource_info
			template<typename Resource_>
		inline LiveNodes liveResources
				( RDFStrategyFlags strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>())
		{
			return liveResources<Resource_>(RDFVariable(), strategy, parent);
		}

		/*!
		 * Implementation function for modelResources class of functions
		 * \param resource_info callee-own resorce information structure, with
		 * a domain specific convention of using the predicate as the type of
		 * the requested resource.
		 */
		LiveNodes implModelResources(RDFProperty &resource_info);

        //@}

/* TODO: iterator design
		LiveNodes restNodes(RDFVariable const &var)
		{
			return modelVariable(var)->firstNode();
		}

			template<typename Type_>
		LiveNodes restResources(RDFVariable const &var = RDFVariable())
		{
			return OmniTypeLiveNode(firstNode(var.isOfType<Type_>()));
		}
 */
	};

	/*!
	 * A RAII idiom class for managing transactions.
	 * If the transaction is active by the time it is destroyed, an automatic rollback is
	 * performed, unless CommitOnDestroy is specified. The ownership of this object is managed
	 * using shared pointers, through typedef RDFTransactionPtr.
	 */
	class Q_DECL_EXPORT RDFTransaction
		: public QSharedFromThis<RDFTransaction, QObject>
	{
		Q_OBJECT
	public:
		typedef RDFFlagsWithBackEndRange Mode;

		enum ModeFlag
		{
			Default = 0
			//! Transaction cannot be shared with others
		,	Exclusive = 1 << 1
			//! Transaction is not active
		,	Inactive = 1 << 2
			//! Transaction is active but pending rollback
		,	PendingRollback = 1 << 3
			//! Transaction is active and there are other sharing transactions
		,	OtherActive = 1 << 4
			//! RAII idiom: the transaction will be committed on destroy, instead of rollback
		,	CommitOnDestroy = 1 << 5
		};

		/*!
		 * Performs a \ref rollback if the transaction is still pending.
		 */
		virtual ~RDFTransaction() {}

		virtual void commit() { commit(false); }

		virtual void rollback() { rollback(false); }

		// use isActive instead
		SOPRANO_DEPRECATED virtual bool isPending() const = 0;

		//! \return true if this transaction is active.
		bool isActive() const;

		/*!
		 * \return the service this transaction is associated.
		 */
		virtual RDFServicePtr service() const = 0;

		/*!
		 * Activates this transaction on the service.
		 * \return true if succesfull; false otherwise.
		 * Reasons for failure are:
		 * \n 1. This transaction is already active.
		 * \n or, an active transaction exists on the service, and:
		 * \n 2. This transaction is exclusive
		 * \n 3. The active transaction is exclusive.
		 * \n 4. The active transaction is pending rollback.
		 * \n 5. This transaction has incompatible backend modes with the active transaction.
		 */
		virtual bool initiate() { return reinitiate(); }

		//! \deprecated Use \ref commitAndReinitiate
		SOPRANO_DEPRECATED virtual bool commitAndInitiate() { return commitAndReinitiate(); }

		//! \deprecated Use \ref rollbackAndReinitiate
		SOPRANO_DEPRECATED virtual bool rollbackAndInitiate() { return rollbackAndReinitiate(); }

		/*!
		 * \return true if this transaction is an exclusive transaction.
		 * When a transaction is exclusive and active, no other in-place transactions can be
		 * made on the associated service.
		 */
		virtual bool isExclusive() const = 0;

		//! \deprecated Use \ref isOtherActive instead
		SOPRANO_DEPRECATED virtual bool isInPlacePending() const;

		//! \return true if some other in-place transaction is active.
		bool isOtherActive() const;

		//! \deprecated Use \ref isPendingRollback instead
		SOPRANO_DEPRECATED virtual bool isInPlacePendingRollback() const = 0;

		//! \return true if any transaction is pending a rollback.
		bool isPendingRollback() const;

		/*!
		 * If the transaction is pending, perform a commit on the transaction,
		 * transmitting the transaction diff set forward to the service.
		 * \n Resets this transaction to non-pending state.
		 * \n This is a no-op if the transaction is not pending.
		 * \param is_blocking if true, wait for the commit to finish before returning
		 * \return true if the rollback was executed; false if another in-place
		 * transaction is still pending and commit was postponed, or if a postponed
		 * rollback was executed instead of a commit.
		 */
		virtual bool commit(bool is_blocking) = 0;
		/*!
		 * If a transaction is pending, perform a rollback on the transaction,
		 * discarding the diff set and resetting the local states of all variables
		 * attached to the service of this transaction back to what they were
		 * when the transaction was initiated.
		 * \n Resets this transaction to non-pending state.
		 * \n This is a no-op if the transaction is not pending.
		 * \param is_blocking if true, wait for the rollback to finish before returning
		 * \return true if the rollback was executed; false if another in-place
		 * transaction is still pending and rollback was postponed.
		 */
		virtual bool rollback(bool is_blocking) = 0;

		//! \alias for \ref RDFTransaction::initiate
		virtual bool reinitiate() = 0;

		/*!
		 * If a transaction is pending, perform a commit on the transaction,
		 * transmitting the transaction diff set forward to the service.
		 * \n As a second step, creates a new, sequential transaction using
		 * the same transaction context, leaving the transaction in a pending state.
		 * \return true if both steps were successfully executed as if calling
		 * commit(is_blocking) && reinitiate(); false otherwise.
		 * If false, it can be assumed that the commit was called but reinitiate not
		 * (this behaviour might be subject to change in the future).
		 * \param is_blocking if true, wait for the commit to finish before returning
		 */
		virtual bool commitAndReinitiate(bool is_blocking = false) = 0;

		/*!
		 * If the transaction is pending, perform a rollback on the transaction,
		 * discarding the diff set and resetting the local states of all variables
		 * attached to the service of this transaction back to what they were
		 * when the transaction was initiated. If no transaction is pending, does
		 * nothing here.
		 * \n As a second step, creates a new, sequential transaction using the
		 * same transaction context, leaving the transaction in a pending state.
		 * \return true if both steps were successfully executed as if calling
		 * rollback(is_blocking) && reinitiate(); false otherwise.
		 * If false, it can be assumed that the rollback was called but reinitiate not
		 * (this behaviour might be subject to change in the future).
		 * \param is_blocking if true, wait for the rollback to finish before returning
		 */
		virtual bool rollbackAndReinitiate(bool is_blocking = false) = 0;

		//! \return the mode flags of the transaction.
		virtual Mode mode() const = 0;

	Q_SIGNALS:
		//! emitted when the commit request finishes on the database.
		void commitFinished();
		//! emitted on rollback
		void rollbackFinished();
		//! emitted when a commit request encounters an error and fails.
		void commitError(QString message);
	};
	typedef QSharedPointer<RDFTransaction> RDFTransactionPtr;

	/*!
	 * A pure virtual interface to a specific rdf service and its default graph.
	 * With \ref RDFGraph, acts as the main nexus between the \ref layer_bound "bound" 
	 * and \ref layer_unbound "unbound" parts of the base service layer.
	 * For the low level, raw access methods no prefix expansion is performed.
	 * Also, the strings are supplied as utf-8 iris, without encoding.
	 */
	class Q_DECL_EXPORT RDFService
		: public RDFGraph
	{
	public:
		//! \deprecated Use \ref createTransaction instead.
		SOPRANO_DEPRECATED virtual RDFTransactionPtr initiateTransaction()
		{
			return createTransaction(RDFTransaction::Default);
		}

		/*!
		 * NOT IMPLEMENTED
		 * \return a new proxy sub-service object as a child of this service.
		 * The sub-service has its own set of prefixes and transactions.
		 * In addition to these, the transaction locks and prefixes of all the
		 * recursive parent service(s) apply.
		 */
		virtual RDFServicePtr createChildService() = 0;


		/*!
		 * Executes a SPARQL query in the service.
		 * \param type specifies the type of the query, RDFQuery::SPARQL for a select query, 
		 * RDFQuery::SPARQLUpdate for the update extension queries.
		 * \return a vector of string lists, where vector items correspond to rows and list 
		 * items correspond to columns in the result set.
		 */
		virtual QVector<QStringList> rawExecuteQuery
				( QString const &query, int type = RDFQuery::SPARQL) = 0;
		//! calls \ref RDFGraph::rawExecuteQuery with SPARQL as query type
		QVector<QStringList> rawSparqlQuery(QString const &query) 
		{ return rawExecuteQuery(query, RDFQuery::SPARQL); }
		//! calls \ref RDFGraph::rawExecuteQuery with SPARQLUpdate as query type
		QVector<QStringList> rawSparqlUpdateQuery(QString const &query) 
		{ return rawExecuteQuery(query, RDFQuery::SPARQLUpdate); }

		/*!
		 * Inserts a triple defined by \a subject, \a predicate, and \a object into the graph.
		 */
		virtual void rawInsertTriple
				( QString const &subject, QString const &predicate, QString const &object) = 0;
		/*!
		 * Deletes a triple defined by \a subject, \a predicate, and \a object from the graph.
		 */
		virtual void rawDeleteTriple
				( QString const &subject, QString const &predicate, QString const &object) = 0;
		/*!
		 * Loads a turtle file defined by the full file \a uri.
		 */
		virtual void rawLoad(QUrl const &uri) = 0;

		/*!
		 * Constructs a non-live \ref LiveNode with given \a type from given \a string, using 
		 * service specified conversion conventions. Most typical use case is to convert date 
		 * fields retrieved by rawExecuteQuery to \ref LiveNode objects.
		 */
		virtual Node rawNodeFromString(QString string, CppType type) = 0;
		/*!
		 * Returns a QString from a given \a node, using service specified conversion conventions.
		 * Most typical use case is to convert a date \a node to be given to other raw functions.
		 */
		virtual QString rawStringFromNode(Node const &node) = 0;

		// TODO pending ABI break: remove useless declaration, we get this from RDFGraph already
		virtual QUrl graphIri() const = 0;

		//! \return the service identification uri
		virtual QUrl serviceIri() const = 0;

		// TODO: pending ABI break: remove useless declaration, we get this from RDFGraph already
		LiveNodes modelVariable
				( RDFVariable const &var
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>());

		// \param variable_data callee-own variable information.
		//virtual LiveNodes modelVariable(RDFProperty &variable_data) = 0;

		virtual LiveNode liveNode(LiveNode const &node, LiveNode const &strict_type = LiveNode());

		// TODO: pending ABI break: move to RDFGraph
		virtual LiveNodes modelVariables
				( RDFVariableList const &variables
				, RDFStrategyFlags model_strategy = RDFStrategy::DefaultStrategy
				, QSharedPointer<QObject> const &parent = QSharedPointer<QObject>()) = 0;

		/*!
		 * Note! Only create_new_layer = false is currently supported on tracker.
		 * \return a new pending transaction on the service, or null
		 * if there already was a transaction pending or if the service
		 * doesnt support in-place transactions. \n
		 * When a transaction is pending on the service, all manipulations
		 * through this service object will not be propagated forward,
		 * but a diff set of changes in memory is created instead.
		 * Commits and rollbacks of the transaction are handled through the returned object.
		 * \deprecated Use \ref createTransaction
		 */
		SOPRANO_DEPRECATED virtual RDFTransactionPtr initiateTransaction(bool create_new_layer)
		{ return create_new_layer
				? createChildService()->createTransaction(RDFTransaction::Exclusive)
				: createTransaction(RDFTransaction::Exclusive); }

		//! \return the pending transaction; null if no transactions are pending.
		virtual RDFTransactionPtr pendingTransaction() const = 0;

		virtual void removeResources(RDFVariable const &variable) = 0;

		virtual LiveNodes implModelResources(RDFProperty &property_info) = 0;

		//! \deprecated Use \ref createTransaction
		SOPRANO_DEPRECATED virtual RDFTransactionPtr inPlaceTransaction(bool exclusive = false);

		// see RDFGraph::implLiveNode for documentation
		virtual LiveNode implLiveNode(LiveNode const &node, LiveNode const &type = LiveNode()
				, bool strict = false, QList<QUrl> const &resource_contexts = QList<QUrl>()) = 0;

		//! \return a full transaction with its own context.
		//! Tracker backend: NOT IMPLEMENTED
		//! * \deprecated Use \ref createChildService()->initiateTransaction(RDFTransaction::Mode)
		SOPRANO_DEPRECATED RDFTransactionPtr fullTransaction(bool exclusive = true)
		{ return createChildService()->createTransaction
					(exclusive ? RDFTransaction::Exclusive : RDFTransaction::Default);
		}

		//! \return service attribute data, this function is a generic
		//! way to obtain backend specific information
		virtual QVariant serviceAttribute(QString const &attribute_name) const = 0;
		//! \return all service attributes.
		virtual QVariantMap serviceAttributes() const = 0;
		//! Sets the given \a service_attribute to given \a value.
		//! \return true if the attribute was set, false otherwise.
		virtual bool setServiceAttribute
				(QString const &service_attribute, QVariant const &value) = 0;

		/*!
		 * Creates a new in-place transaction on the service of the requested \a mode.
		 * A transaction can be requested many times, in which case the actual transaction is
		 * shared. It is finalized only when all sharing transactions have committed with
		 * \ref RDFTransaction::commit or rolled back with \ref RDFTransaction::rollback.
		 * If any sharing transaction is rolled back, then the whole shared transaction is going
		 * to be rolled back on finalize, otherwise it is committed.
		 * \n Sharing can be prevented with RDFTransaction::Exclusive \a mode flag.
		 * \n
		 * \n If RDFTransaction::Inactive \a mode flag is specified, a valid but inactive
		 * transaction object will always be returned. User must then call
		 * \ref RDFTransaction::initiate manually to activate the transaction. Otherwise,
		 * initiate will be called implicitly and if it fails, null transaction is returned.
		 *
		 * \return the transaction object if successful; null otherwise.
		 */
		virtual RDFTransactionPtr createTransaction
				(RDFTransaction::Mode mode = RDFTransaction::Default) = 0;

		virtual LiveNodes implLiveNodes
			( LiveNodeList const &urls, RDFVariable const &object_info
			, RDFStrategyFlags model_strategy, bool strict
			, QSharedPointer<QObject> const &parent) = 0;

		//! \return the query text for given \a query object with backend specific optimizations.
		virtual QString rawQueryString(RDFQuery const &query) const = 0;

		/// use SopranoLive::setVerbosity() instead
		virtual void setVerbosity(int level) = 0;
	};

		template<typename Predicate_>
	typename AutoGen::Property<Predicate_>::Type LiveResource::baseFirstObject() const
	{
		return graph()->service()->createTransaction(RDFTransaction::CommitOnDestroy)
				, AutoGen::Property<Predicate_>::fromNode(firstObject
						(Predicate_::iri(), AutoGen::Property<Predicate_>::metaType()));
	}

		template<typename Predicate_>
	typename AutoGen::Property<Predicate_>::Type LiveResource::baseGetObject() const
	{
		return graph()->service()->createTransaction(RDFTransaction::CommitOnDestroy)
				, AutoGen::Property<Predicate_>::fromNode(getObject
						(Predicate_::iri(), AutoGen::Property<Predicate_>::metaType()));
	}

		template<typename Predicate_>
	typename AutoGen::Property<Predicate_>::Type LiveResource::baseSetObject
			( typename AutoGen::Property<Predicate_>::Type const &object) const
	{
		return graph()->service()->createTransaction(RDFTransaction::CommitOnDestroy)
				, AutoGen::Property<Predicate_>::fromNode(setObject
						(Predicate_::iri(), AutoGen::Property<Predicate_>::toNode(object)));
	}

		template<typename Predicate_>
	typename AutoGen::Property<Predicate_>::Type LiveResource::baseAddObject
			( typename AutoGen::Property<Predicate_>::Type const &object) const
	{
		return graph()->service()->createTransaction(RDFTransaction::CommitOnDestroy)
				, AutoGen::Property<Predicate_>::fromNode(addObject
						(Predicate_::iri(), AutoGen::Property<Predicate_>::toNode(object)));
	}

		template<typename Predicate_>
	typename AutoGen::Property<Predicate_>::Type LiveResource::baseAddObject() const
	{
		return graph()->service()->createTransaction(RDFTransaction::CommitOnDestroy)
				, AutoGen::Property<Predicate_>::fromNode(addObject
						(Predicate_::iri(), AutoGen::Property<Predicate_>::metaType()));
	}

		template<typename Predicate_>
	typename AutoGen::Property<Predicate_>::Type LiveResource::baseFirstSubject() const
	{
		return graph()->service()->createTransaction(RDFTransaction::CommitOnDestroy)
				, AutoGen::Property<Predicate_>::fromNode(firstSubject
						(Predicate_::iri(), AutoGen::Property<Predicate_>::metaType()));
	}

		template<typename Predicate_>
	typename AutoGen::Property<Predicate_>::Type LiveResource::baseGetSubject() const
	{
		return graph()->service()->createTransaction(RDFTransaction::CommitOnDestroy)
				, AutoGen::Property<Predicate_>::fromNode(getSubject
						(Predicate_::iri(), AutoGen::Property<Predicate_>::metaType()));
	}

		template<typename Predicate_>
	typename AutoGen::Property<Predicate_>::Type LiveResource::baseSetSubject
			( typename AutoGen::Property<Predicate_>::Type const &subject) const
	{
		return graph()->service()->createTransaction(RDFTransaction::CommitOnDestroy)
				, AutoGen::Property<Predicate_>::fromNode(setSubject
						(Predicate_::iri(), AutoGen::Property<Predicate_>::toNode(subject)));
	}

		template<typename Predicate_>
	typename AutoGen::Property<Predicate_>::Type LiveResource::baseAddSubject
			( typename AutoGen::Property<Predicate_>::Type const &subject) const
	{
		return graph()->service()->createTransaction(RDFTransaction::CommitOnDestroy)
				, AutoGen::Property<Predicate_>::fromNode(addSubject
						(Predicate_::iri(), AutoGen::Property<Predicate_>::toNode(subject)));
	}

		template<typename Predicate_>
	typename AutoGen::Property<Predicate_>::Type LiveResource::baseAddSubject() const
	{
		return graph()->service()->createTransaction(RDFTransaction::CommitOnDestroy)
				, AutoGen::Property<Predicate_>::fromNode(addSubject
						(Predicate_::iri(), AutoGen::Property<Predicate_>::metaType()));
	}

}
#endif /* SOPRANOLIVE_RDFSERVICE_H_ */
