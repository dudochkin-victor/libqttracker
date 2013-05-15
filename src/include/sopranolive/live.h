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
 * liveresource.h
 *
 *  Created on: Dec 25, 2008
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */
#ifndef SOPRANOLIVE_LIVE_H
#define SOPRANOLIVE_LIVE_H

#include <QtCore>
#include "Node"
#include "mostderived.h"
#include "qsharedobject.h"

namespace SopranoLive
{
	using Soprano::Node;
	using Soprano::LiteralValue;

	class LiveResource;
	typedef QSharedPointer<LiveResource> LiveResourcePtr;
	typedef QSharedPointer<LiveResource const> LiveResourceConstPtr;

		template< typename Base_ = LiveResource, typename Backend_ = Base_>
	class LiveResourceBridgeMixin;

	class ValueOfLiveResource_Policy;

		template< typename Resource_
		        , typename NodePolicy_ = ValueOfLiveResource_Policy
		        , typename MostDerived_ = void>
	class ValueOfLiveResource;

		template< typename Resource_>
	class Live;
		template< typename Resource_>
	class StrictLive;
	class LiveNode;

	/*!
	 * \ingroup soprano_live
	 * \anchor LiveResourceNode
	 * Live or non-live entity.
	 * A semantically correct alias for \ref LiveNode. LiveNode is the short-hand version.
	 */
	typedef LiveNode LiveResourceNode;


	class LiveNodeModel;
	/* \ingroup soprano_live
	 * \anchor LiveNodeModelPtr
	 * A shared pointer to a LiveNodeModel.
	 * The LiveNodeModel is destructed only once the last LiveNodeModelPtr pointing to it is destructed.
	 * LiveNodeModel * can be retrieved from it with data() member.
	 * \sa \ref livenodemodelptr_how_and_why
	 */
	typedef QSharedPointer<LiveNodeModel> LiveNodeModelPtr;
	typedef QSharedPointer<const LiveNodeModel> ConstLiveNodeModelPtr;

	class NodeListPolicy;

		template< typename Node_
		        , typename BagPolicy_ = NodeListPolicy>
	class LiveBag;

	/*!
	 * \ingroup soprano_live
	 * \anchor LiveNodes
	 * Live or non-live entity.
	 * A typedef for a \ref LiveBag containing \ref LiveNode objects using \ref NodeListPolicy.
	 * \sa \ref layer_bound
	 */
#ifndef GENERATING_DOXYGEN_DOCUMENTATION
	typedef LiveBag<LiveNode, NodeListPolicy> LiveNodes;
#else
	class LiveNodes;
#endif

	typedef QList<LiveNode> LiveNodeList;


	class RDFVariable;
	typedef QList<RDFVariable> RDFVariableList;

	class RDFPropertyData;
	typedef RDFPropertyData RDFProperty;

	typedef QList<RDFProperty> RDFPropertyList;

	class RDFGraph;
	typedef QSharedPointer<RDFGraph> RDFGraphPtr;

	class RDFService;
	typedef QSharedPointer<RDFService> RDFServicePtr;

	//! Any flag type for which the 16 most significant bits are left for backend to define.
	typedef quint64 RDFFlagsWithBackEndRange;

	enum {
			offset_BackEndFlags = 48
		,	BackEndFlagsMask = ~((RDFFlagsWithBackEndRange(1) << offset_BackEndFlags) - 1)
	};


	typedef RDFFlagsWithBackEndRange RDFStrategyFlags;


	namespace Detail
	{
		struct Q_DECL_EXPORT EmptyBase
		{};

		Q_DECL_EXPORT QUrl *urlFromEncodedString(QString const &encoded);
	}

	/*!
	 * Iri class base class. Should be inherited by an iri class which gives
	 * itself as the template parameter. The iri class should then go on to
	 * define either encodedIri or iri function. This base class forwards
	 * the remaining functions to that definition.
	 */
		template<typename IriClassName_, typename Base_ = Detail::EmptyBase>
	struct Q_DECL_EXPORT IriClassBase
		: Base_
	{
		static const char *encodedIri()
		{
			return IriClassName_::iri().toString();
		};
		static const QUrl &iri()
		{
			static QUrl *ret = 0;
			if(!ret)
				ret = Detail::urlFromEncodedString(IriClassName_::encodedIri());
			return *ret;
		}
		static const QList<QUrl> &iriList()
		{
			static QList<QUrl> *ret = 0;
			if(!ret)
				ret = &(*new QList<QUrl>() << IriClassName_::iri());
			return *ret;
		}
	};

	/*!
	 * Macro which defines an iri convenience class called \a IriClassName_.
	 * The iri class can then be passed as a template argument to
	 * convenience functions to represent the given \a IriEncodedString_ .
	 */
#define SOPRANOLIVE_DECLARE_IRI_CLASS(IriClassName_, IriEncodedString_) \
	struct Q_DECL_EXPORT IriClassName_ \
		: IriClassBase<IriClassName_> \
	{ \
		static const char *encodedIri() { return IriEncodedString_; }; \
	};

	namespace Contexts
	{
		SOPRANOLIVE_DECLARE_IRI_CLASS(Core, "http://www.sopranolive.org/contexts/core/");
		SOPRANOLIVE_DECLARE_IRI_CLASS(Generic, "http://www.sopranolive.org/contexts/generic/");
	}

	struct RDFStrategy
	{
		// this enum contains the offsets for StrategyFlags entries
		enum Flags_offsets
		{
			offsetMark_StateFlags // 0
		,	offset_Streaming = offsetMark_StateFlags, offset_Windowed, offset_Live, offset_Writable, offset_Cached
		,	offset_Subject, offset_Predicate, offset_Object
		,	offset_NonMultipleValued, offset_NonOptionalValued
		,	offset_InverseNonMultipleValued, offset_InverseNonOptionalValued
		,	offset_Ascending, offset_Descending
		,	offsetMark_RequestFlags // 14
		,	offset_BypassCache = offsetMark_RequestFlags, offset_NoCachedProperties, offset_ExposeDerivedProperties
		,	offset_DerivedProperty, offset_ReverseDerivedProperty
		,	offsetMark_RequestAllowFlags // 19
		,	offset_AllowStreaming = offsetMark_RequestAllowFlags + offset_Streaming
		,	offset_AllowWindowed = offsetMark_RequestAllowFlags + offset_Windowed
		,	offset_AllowLive = offsetMark_RequestAllowFlags + offset_Live
		,	offset_AllowWritable = offsetMark_RequestAllowFlags + offset_Writable
		,	offset_AllowCached = offsetMark_RequestAllowFlags + offset_Cached
		,	offsetMark_LiveNodeStrategyFlagsEnd // 24

		,	offset_Distinct
		,	offset_AggregateColumnMode
		, 	value_AggregateCount = 1, value_AggregateSum, value_AggregateAverage, value_AggregateMinimum, value_AggregateMaximum
		,	value_AggregateFirst
		, 	value_CustomColumn = 15
		,	offsetMark_AggregateColumnMode = offset_AggregateColumnMode + 4

		,	offset_Running = offsetMark_AggregateColumnMode // 30 ABI break reorder
		,	offset_IdentityColumn // ABI break reorder
		,	offset_CacheDataRoles // ABI break reorder
		,	offset_HiddenColumn // ABI break reorder
		,	offset_Literal // ABI break reorder
		,	offset_ChildedColumn // ABI break reorder
		,	offset_ExpandDerivedProperties // ABI break reorder
		,	offset_BoundProperty // ABI break reorder
		,	offset_UseOnlyCache // ABI break reorder
		,	offset_ChainedProperty // ABI break reorder
		,	offset_ModelReady // 40 ABI break reorder
		,	offset_DerivedsReady // ABI break reorder
		,	offset_DiscardUpdateResults // ABI break reorder
		,	offset_UpdateProperty // ABI break reorder
		,	offset_Owned // ABI break reorder
		,	offset_Shared // ABI break reorder
		// 46

		,	offset_ContextSpecificFlags = offset_BackEndFlags
		,	offsetMark_ContextSpecificFlagsEnd

		,	offset_ErrorContext = offset_ContextSpecificFlags
		,	offset_AlterStrategyError
		,	offset_ErrorType
		,	offsetMark_ErrorType = offset_ErrorType + 2

		};

		/*! \enum RDFStrategyFlags
		 * The strategies the model uses with query execution and caching policies.
		 */
		// TODO: on ABI break, requires refactoring. Three different use cases exist: model flags, property flags and variable/column flags.
		enum Flags
		{
			Disabled = 0

			/*!
			 * Streaming and AllowStreaming:
			 * Model is immediately ready and no methods will block, but contents will
			 * be added to it piece by piece as they arrive. Without this flag, the model
			 * behaves as if it contains all the data, but calls to data retrieval functions
			 * might block. During streaming, writes and live state are disabled. After
			 * the streaming content has been received, modelUpdated signal is emitted, and
			 * model is returned non-streaming state and possible write and live states are enabled.
			 */
		,	Streaming = 1 << offset_Streaming // 0x1
		,	RequireStreaming = Streaming // 0x1
		,	AllowStreaming = Streaming | 1 << offset_AllowStreaming // 0x80001
		, 	StreamingMask = RequireStreaming | AllowStreaming

			//! Whole model will not be requested, but instead small windows on demand.
			// Accessing a non up to date row will block. These can controlled with refreshRows method and rowsUpdated signal
		,	Windowed = 1 << offset_Windowed // 0x2
		,	RequireWindowed = Windowed // 0x2
		,	AllowWindowed = Windowed | 1 << offset_AllowWindowed // 0x100002
		, 	WindowedMask = RequireWindowed | AllowWindowed

			//! Model is live, and will reflect remote changes locally.
		,	Live = 1 << offset_Live // 0x4
		,	RequireLive = Live // 0x4
		,	AllowLive = Live | 1 << offset_AllowLive // 0x200004
		, 	LiveMask = RequireLive | AllowLive

			//! Model contents are writable. If live, these changes are also reflected remotely.
		,	Writable = 1 << offset_Writable // 0x8
		, 	RequireWritable = Writable // 0x8
		, 	AllowWritable = Writable | 1 << offset_AllowWritable // 0x400008
		, 	WritableMask = RequireWritable | AllowWritable

			//! The model contents and all expanded derived properties are registered with the caching system.
		,	Cached = 1 << offset_Cached // 0x10
		, 	RequireCached = Cached // 0x10
		, 	AllowCached = Cached | 1 << offset_AllowCached // 0x800010
		, 	CachedMask = RequireCached | AllowCached

			/*!
			 * Set when operation(s) against backend are running. If disabled by alterStrategy,
			 * backend strategy shall cancel all pending operations.
			 */
		,	Running = RDFStrategyFlags(1) << offset_Running

			//! Set when all primary model contents are locally accessible without blocking.
		,	ModelReady = RDFStrategyFlags(1) << offset_ModelReady

			//! Set when all derived model contents are locally accessible without blocking.
		,	DerivedsReady = RDFStrategyFlags(1) << offset_DerivedsReady

			//! A primary column in a derived query build.
		,	IdentityColumn = RDFStrategyFlags(1) << offset_IdentityColumn

			//! A childed column. Entries of this column have children.
		,	ChildedColumn = RDFStrategyFlags(1) << offset_ChildedColumn

			//! A hidden column in a derived query build.
		, 	HiddenColumn = RDFStrategyFlags(1) << offset_HiddenColumn

			//! The property is registered as a cached property
		,	CacheProperty = 1 << offset_DerivedProperty // 0x20000
		,	DerivedProperty = 1 << offset_DerivedProperty // 0x20000

			//! An already bound property
		,	BoundProperty = RDFStrategyFlags(1) << offset_BoundProperty

			//! An already bound derived property.
		,	BoundDerivedProperty = DerivedProperty | BoundProperty

			//! Chained property
		,	ChainedProperty = RDFStrategyFlags(1) << offset_ChainedProperty

			//! Update property
		,	UpdateProperty = RDFStrategyFlags(1) << offset_UpdateProperty

			//! The contents are ordered in an ascending order
		,	Ascending = 1 << offset_Ascending // 0x1000

			//! The contents are ordered in a descending order
		,	Descending = 1 << offset_Descending // 0x2000

			//! The contents are ordered in a custom order
		,	CustomOrdering = Ascending | Descending // 0x3000

		 	//! Mask used to determine if there is an ordering
		,	Ordered = CustomOrdering // 0x3000

			//! The contents are distinct
		,	Distinct = 1 << offset_Distinct // 0x1000000

			//! The model contents will not be retrieved from the cache graph.
		,	BypassCache = 1 << offset_BypassCache // 0x4000

			//! The model contents will be retrieved only from the cache graph.
		,	UseOnlyCache = RDFStrategyFlags(1) << offset_UseOnlyCache

			//! The reverse property is of this property is also cached
		,	ReverseDerivedProperty = 1 << offset_ReverseDerivedProperty // 0x40000

			//! The model will not used registered cached properties even if requested
			// Note that this flag is implied if Cached is not set.
		,	NoCachedProperties = 1 << offset_NoCachedProperties // 0x8000

			//! All derived columns will be expanded as columns
		,	ExpandDerivedProperties = RDFStrategyFlags(1) << offset_ExpandDerivedProperties

			//! The model will expose single valued cached properties in the model
		,	ExposeDerivedProperties = 1 << offset_ExposeDerivedProperties // 0x10000
		,	ExposeCachedProperties = ExposeDerivedProperties // 0x10000

			//! The model will cache all Qt::ItemDataRole values that are requested
		,	CacheDataRoles = RDFStrategyFlags(1) << offset_CacheDataRoles

			//! Discard update query results
		,	DiscardUpdateResults = RDFStrategyFlags(1) << offset_DiscardUpdateResults

			// TODO: change to "The entity contains subject information"
			//! The model represents a subject set
		, 	Subject = 1 << offset_Subject // 0x20

			// TODO: change to "The entity contains predicate information"
			//! The model represents a predicate set
		,	Predicate = 1 << offset_Predicate // 0x40

			// TODO: change to "The entity contains object information"
			//! The model represents a object set
		,	Object = 1 << offset_Object // 0x80

		,	SubjectPredicate = Subject | Predicate // 0x60
		,	PredicateObject = Predicate | Object // 0xC0
		,	SubjectObject = Subject | Object // 0xA0
		,	SubjectPredicateObject = Subject | Predicate | Object // 0xE0

			//! The mask for property: either subject or object.
		,	PropertyMask = Subject | Object // 0xA0

			//! The mask for triple information
		, 	TripleMask = Subject | Predicate | Object // 0xE0

		 	//! The entity is a literal value
		,	Literal = RDFStrategyFlags(1) << offset_Literal

			//! The model will not be multiple valued (max cardinality 1)
		,	NonMultipleValued = 1 << offset_NonMultipleValued // 0x100

			//! The model will contain at least one value (min cardinality 1)
		,	NonOptionalValued = 1 << offset_NonOptionalValued // 0x200

			//! The model will be single valued (cardinality 1)
		,	SingleValued = NonMultipleValued | NonOptionalValued // 0x300

			//! The model will be optionally single valued (cardinality 0-1)
		,	OptionalSingleValued = NonMultipleValued // 0x100

			//! Functional property is a the general alias for optional single valued properties.
		,	Functional = OptionalSingleValued // 0x100

			//! The inverse relation of the model will not be multiple valued (max cardinality 1)
		,	InverseNonMultipleValued = 1 << offset_InverseNonMultipleValued // 0x400

			//! The inverse relation of the model will contain at least one value (min cardinality 1)
		,	InverseNonOptionalValued = 1 << offset_InverseNonOptionalValued // 0x800

			//! The inverse relation of the model will be single valued (cardinality 1)
		,	InverseSingleValued = InverseNonMultipleValued | InverseNonOptionalValued // 0xC00

			//! The inverse relation of the model will be optionally single valued (cardinality 0-1)
		,	InverseOptionalSingleValued = InverseNonMultipleValued // 0x400

			//! Inverse functional property is the general alias for inverse optional single valued properties.
		,	InverseFunctional = InverseOptionalSingleValued // 0x400

			//! Whenever owner, or the subject of an owning property, is destroyed,
			//! all owned resources, or the objects of the owning property, should be
			//! destroyed as well.
		,	Owns = RDFStrategyFlags(1) << offset_Owned // ABI break reorder
			//! Whenever sharer, or the subject of an sharing property, is destroyed,
			//! all shared resources, or the objects of the sharing property, should be
			//! destroyed if no other sharer or owner remains.
		,	Shares = RDFStrategyFlags(1) << offset_Shared // ABI break reorder

			//! Aggregation and custom column policy options

			//! Aggregate count
		, 	AggregateCount = value_AggregateCount << offset_AggregateColumnMode // 0x2000000

			//! Aggregate sum
		,	AggregateSum = value_AggregateSum << offset_AggregateColumnMode // 0x4000000

			//! Aggregate average
		,	AggregateAverage = value_AggregateAverage << offset_AggregateColumnMode // 0x6000000

			//! Aggregate minimum
		,	AggregateMinimum = value_AggregateMinimum << offset_AggregateColumnMode // 0x8000000

			//! Aggregate maximum
		,	AggregateMaximum = value_AggregateMaximum << offset_AggregateColumnMode // 0xA000000

		,	AggregateFirst = value_AggregateFirst << offset_AggregateColumnMode // 0xC000000

			//! Custom information column
		,	CustomColumn = value_CustomColumn << offset_AggregateColumnMode // 0x3E000000

		// TODO: these should be split to separate property and column flags
		,	CountColumn = AggregateCount | NonMultipleValued
		,	SumColumn = AggregateSum | NonMultipleValued
		,	AvgColumn = AggregateAverage | NonMultipleValued
		,	MinColumn = AggregateMinimum | NonMultipleValued
		,	MaxColumn = AggregateMaximum | NonMultipleValued
		,	FirstColumn = AggregateFirst | NonMultipleValued

		, 	AggregateColumnMask = (1 << offsetMark_AggregateColumnMode) - (1 << offset_AggregateColumnMode) // 0xE0000000

		,	ContextSpecificMask = BackEndFlagsMask


		// some aggregated values
		, 	DefaultStrategy = AllowCached | ExpandDerivedProperties // 0x900012
		,	WritableStrategy = RequireWritable | DefaultStrategy // 0x90001A
		,	LiveStrategy = RequireLive | WritableStrategy // 0x90001C
		, 	StreamingStrategy = AllowStreaming | DefaultStrategy // 0x880011
		, 	StreamingWritableStrategy = RequireWritable | StreamingStrategy // 0x880019
		, 	StreamingUpdaterModule = RequireLive | StreamingWritableStrategy // 0x88001C
		, 	WindowedStrategy = AllowWindowed | DefaultStrategy // 0x900012
		, 	WindowedWritableStrategy = RequireWritable | WindowedStrategy // 0x90001A
		, 	WindowedUpdaterModule = RequireLive | WindowedWritableStrategy // 0x90001E



		,	RequiredsMask
			= RequireStreaming | RequireWindowed | RequireLive | RequireWritable | RequireCached // 0x1F

		,	AllowsMask = (AllowStreaming | AllowWindowed | AllowLive | AllowWritable | AllowCached)
			& ~RequiredsMask // 0xF80000

		,	RequiredsAllowsMask = RequiredsMask | AllowsMask // 0xF8001F

		// governs which flags are transferred from property information to column information
		, 	PropertyToColumnFlagsMask
				// transfer all.
				= ~RDFStrategyFlags()
				// = RequiredsAllowsMask | AggregateColumnMask | IdentityColumn | ChildedColumn
				// 	| CustomOrdering | ChainedProperty

		,	ModelFlagsMask =
			  Streaming | Windowed | Live | Writable | Cached
			| AllowStreaming | AllowWindowed | AllowLive | AllowWritable | AllowCached
			| Running
			| ExposeDerivedProperties | ExpandDerivedProperties
			| CacheDataRoles | UseOnlyCache
			| DiscardUpdateResults
		};

		enum ErrorFlags
		{
			//! Set in error signal mask, when error is due to a state change request
			AlterStrategyError = RDFStrategyFlags(1) << offset_AlterStrategyError

		,	Warning = RDFStrategyFlags(1) << offset_ErrorType
		,	Debug = RDFStrategyFlags(2) << offset_ErrorType
		,	Critical = RDFStrategyFlags(3) << offset_ErrorType
		};
	};

	namespace BackEnds
	{
		namespace Local
		{
			Q_DECL_EXPORT RDFServicePtr local();
			Q_DECL_EXPORT RDFServicePtr localRelease();
			Q_DECL_EXPORT RDFServicePtr service();
		}
	}

	/*!
	 * Denotes a specific local type with run-time type information, using Qt metainformation infrastructure.
	 * Used by various operations returning a LiveNodeModel to instruct what kind of LiveNodes it should use internally.
	 * \sa
	 */
	struct Q_DECL_EXPORT CppType
	{
		/*! \enum ConstraintId
		 * Introduces a \a None enum to denote no constraint
		 */
		// uses the Qt meta type id system
		enum ConstraintId { None = QVariant::Invalid };
	private:
		int type_id;
	public:
		explicit CppType(int type_id = None)
			: type_id(type_id)
		{}

		int metaTypeId() const { return type_id; }

		static CppType of(CppType type) { return type; }
		static CppType of(QUrl const &/*url*/) { return CppType(QVariant::Url); }
			template<typename Type_>
		static CppType of();

		static QUrl rdfTypeof(CppType type);
		static QUrl rdfTypeof(QUrl const &type) { return type; }
			template<typename Type_>
		static QUrl rdfTypeOf();
	};

	namespace Detail
	{
			template<typename Type_>
		CppType localTypeHelper(void *) { return CppType(qMetaTypeId<Type_>()); }

			template<typename Type_>
		QUrl localTypeHelper(LiveResource *) { return Type_::iri(); }

			template< typename Type_
					, typename Resource_
			        , typename NodePolicy_
			        , typename MostDerived_>
		QUrl localTypeHelper(ValueOfLiveResource<Resource_, NodePolicy_, MostDerived_> *)
		{ return Resource_::iri(); }
	}

		template<typename Type_>
	CppType CppType::of() { return CppType::of(Detail::localTypeHelper<Type_>((Type_ *)0)); }

		template<typename Type_>
	QUrl CppType::rdfTypeOf() { return CppType::rdfTypeOf(Detail::localTypeHelper<Type_>((Type_ *)0)); }

	/*!
	 * A base class for all by-value, singular and set live resources, such as LiveNode and LiveNodes.
	 * When live, the resource has an active connection proxy entity, which can be retrieved with
	 * \ref ValueOfLiveEntity::entity. This entity mediates the connection of the resource
	 * to a backend graph, which can be retrieved with \ref ValueOfLiveEntity::graph.
	 * Also, a local value for the resource can be retrieved with \ref ValueOfLiveEntity::value.
	 * In case of a LiveNode class of objects, this value is Soprano::Node, and in case of
	 * LiveNodes class of objects, this value typically is a QList<LiveNode>.
	 */
		template<typename Entity_, typename ValuePolicy_ = typename Entity_::ValuePolicy, typename MostDerived_ = void>
	class Q_DECL_EXPORT ValueOfLiveEntity
		: public ValuePolicy_
	{
	public:
		typedef Entity_ Entity;
		typedef ValuePolicy_ ValuePolicy;
		typedef typename MostDerivedHelper<ValueOfLiveEntity, MostDerived_>::Type MD;
		typedef typename ValuePolicy::template Defines<Entity_> Defines;
		typedef typename Defines::Value Value;
		typedef typename Defines::EntityPointer EntityPointer;
		typedef typename Defines::ValuePointer ValuePointer;

		/*!
		 * \ref live_entities "Non-live non-blocking".
		 * \return if live, a shared pointer to the entity, if non-live, a null pointer.
		 */
		EntityPointer const &entity() const
		{ return entity_; }

		/*! \fn Value const &value() const
		 * \ref live_entities "Non-live blocking".
		 * \return the local value as a \a Value defined by the \a ValuePolicy.
		 */
		Value const &value() const
		{
			if(!value_)
				value_ = this->template evaluateValue<MD>();
			return *value_;
		}

		RDFGraph *graph() const
		{
			return entity_ ? entity_->graph() : 0;
		}

		/*!
		 * \ref live_entities "Non-live non-blocking".
		 * \return if live, a pointer to the proxy interface, if non-live, a null pointer.
		 */
		Entity *operator->() const { return &*entity_; }

		/*!
		 * \ref live_entities "Non-live non-blocking".
		 * \return if live, a pointer to the proxy interface, if non-live, a null pointer.
		 */
		Entity &operator*() const { return *entity_; }

		/*!
		 * \ref live_entities "Non-live blocking".
		 * \return the local value.
		 */
		operator Value const &() const { return value(); }

		/*!
		 * \ref live_entities "Non-live blocking".
		 * \return an RDFVariable representing the value/values of this entity
		 */
		// defined in rdfunbound.h
		inline RDFVariable variable() const;

		/**
		 * \ref live_entities "Non-live non-blocking".
		 * \return true if the node is live.
		 */
		bool isLive() const { return entity_ != 0; }

		/*!
		 * alias for isLive
		 */
		operator bool() const { return isLive(); }

        /*!
		 * \ref live_entities "Non-live blocking".
         */
		bool operator!=(ValueOfLiveEntity const &other) const
		{ return this->template notEqual<MD>(other); }
        /*!
		 * \ref live_entities "Non-live blocking".
         */
		bool operator==(ValueOfLiveEntity const &other) const
		{ return !operator!=(other); }

        /*!
		 * \ref live_entities "Non-live blocking".
         */
			template<typename OtherEntity_, typename MD_>
		bool operator!=(ValueOfLiveEntity<OtherEntity_, ValuePolicy, MD_> const &other) const
		{ return this->template notEqual<MD>(other); }
        /*!
		 * \ref live_entities "Non-live blocking".
         */
			template<typename OtherEntity_, typename MD_>
		bool operator==(ValueOfLiveEntity<OtherEntity_, ValuePolicy, MD_> const &other) const
		{ return !operator!=(other); }

		ValueOfLiveEntity &operator=(ValueOfLiveEntity const &other)
		{
			this->template assign<MD>(other);
			return *this;
		}

	//protected:
	public:
		EntityPointer entity_;
		mutable ValuePointer value_;

	protected:
		ValueOfLiveEntity( EntityPointer const &entity = EntityPointer()
		                 , ValuePointer const &value = ValuePointer()
		                 , ValuePolicy const &value_policy = ValuePolicy())
			: ValuePolicy(value_policy), entity_(entity), value_(value) {}

		~ValueOfLiveEntity()
		{
			this->template destruct<MD>();
		}
	};

	/*!
	 * A base class for all by-value, live resources, such as LiveNode and LiveNodes.
	 */
	struct Q_DECL_EXPORT ValueOfLiveEntity_Policy
		: MostDerivedPolicy<ValueOfLiveEntity_Policy>
	{
			template<typename This_>
		void defaultConstruct()
		{
			md<This_>().value_ = new typename This_::Value();
		}

			template<typename This_, typename Other_>
		void copyConstruct(Other_ const &other, bool = false)
		{
			if(!other.entity_)
			{
				md<This_>().entity_ = typename This_::EntityPointer();
				md<This_>().value_ = md<This_>().template copyValue<This_>(other.value());
			} else
			{
				md<This_>().entity_ = other.entity()->template sharedFromThis<typename This_::Entity>();
				md<This_>().value_ = typename This_::ValuePointer();
			}
		}

			template<typename This_>
		void destruct()
		{
			if(!md<This_>().entity_)
				delete md<This_>().value_;
			else
				md<This_>().entity_ = typename This_::EntityPointer();
			md<This_>().value_ = typename This_::ValuePointer();
		}

			template<typename This_, typename Value_>
		typename This_::ValuePointer copyValue(Value_ const &value) const
		{
			return new typename This_::Value(value);
		}

			template<typename This_, typename Other_>
		void assign(Other_ const &other)
		{
			md<This_>().template destruct<This_>();
			md<This_>().template copyConstruct<This_>(other);
		}

			template<typename This_>
		typename This_::ValuePointer evaluateValue() const
		{
			return md<This_>().entity()->value();
		}

			template<typename This_, typename Other_>
		bool notEqual(Other_ const &other_) const
		{
			// note, may give false negatives.
			// different entities do not mean different values
			if(md<This_>().entity() && md<This_>().entity() == other_.entity())
				return false;
			// no else, see above
			if(md<This_>().value() == other_.value())
				return false;
			return true;
		}

			template<typename This_>
		bool hasTypeImpl(LiveNode const &type) const
		{
			return md<This_>().entity_->hasType(type);
		}

			template<typename This_, typename Type_>
		bool hasTypeImpl() const
		{
			return md<This_>().entity_->template hasType<Type_>();
		}
	};

	/*!
	 * A base class for all by-value, singular live resources, such as LiveNode and Live<>.
	 */
		template< typename Resource_
		        , typename NodePolicy_
		        , typename MostDerived_>
	class Q_DECL_EXPORT ValueOfLiveResource
		: public ValueOfLiveEntity< Resource_
								  , NodePolicy_
								  , typename MostDerivedHelper<ValueOfLiveResource<Resource_, NodePolicy_, MostDerived_>, MostDerived_>::Type
								  >
	{
	public:
		typedef NodePolicy_ NodePolicy;
		typedef typename MostDerivedHelper<ValueOfLiveResource<Resource_, NodePolicy_, MostDerived_>, MostDerived_>::Type MD;
		typedef ValueOfLiveEntity< Resource_, NodePolicy_, MD> Base;

		typedef typename Base::Value Value;
		typedef typename Base::Entity Entity;
		typedef typename Base::EntityPointer EntityPointer;
		/*!
		 * \ref live_entities "Non-blocking".
		 */
		ValueOfLiveResource() : Base()
		{ this->template defaultConstruct<MD>(); }

		/*!
		 * \ref live_entities "Non-blocking".
		 */
		ValueOfLiveResource(ValueOfLiveResource const &cp, bool has_type = true) : Base()
		{ this->template copyConstruct<MD>(cp, has_type); }

		/*!
		 * \ref live_entities "Non-blocking".
		 */
			template<typename OtherResourceType_, typename OtherPolicy_, typename OtherMD_>
		ValueOfLiveResource(ValueOfLiveResource<OtherResourceType_, OtherPolicy_, OtherMD_> const &cp, bool has_type = false)
			: Base()
		{ this->template copyConstruct<MD>(cp, has_type); }

		/*!
		 * Non-blocking.
		 * Construct a non-live resource list from the given set of resources.
		 * \param resources the set of resources to be encapsulated.
		 */
		ValueOfLiveResource(typename Base::Value const &node)
			: Base(EntityPointer(), this->template copyValue<MD>(node)) {}

		/*!
		 * Blocking state depends on the given \a policy construct method.
		 * \param model the live model.
		 * \param policy policy used to map the model into the list.
		 */
		explicit ValueOfLiveResource(LiveResourcePtr resource)
			: Base(resource) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 */
			template<typename Other_>
		explicit ValueOfLiveResource(QSharedVirtualInheritEnabledPointer<Other_, LiveResourcePtr> const &resource)
			: Base(resource) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 */
			template<typename Other_>
		explicit ValueOfLiveResource(QSharedPointer<Other_> const &resource)
			: Base(resource) {}

		/*!
		 * \ref live_entities "Non-blocking".
		 */
			template<typename Other_>
		explicit ValueOfLiveResource(QWeakPointer<Other_> const &resource)
			: Base(resource) {}

		/*! \fn Value const &value() const
		 * \ref live_entities "Non-live blocking".
		 * \return the local value of the bag as a \ref QList<Node_>
		 */
		Value const &node() const { return this->value(); }

		/*!
		 * \ref live_entities "Non-live non-blocking".
		 * \return a shared pointer to the model if live, or a null pointer otherwise.
		 */
		EntityPointer const &resource() const { return this->entity_; }

		/*!
		 * \name Type information
		 */
		//@{
		/*!
		 * \ref live_entities "Non-live blocking".
		 * \return The Node type.
		 */
		Node::Type type() const { return node().type(); }

		/**
		 * \ref live_entities "Non-live non-blocking".
		 * \return true if the node is empty.
		 */
		bool isEmpty() const { return isLive() ? false : this->value_->isEmpty(); }

		/**
		 * \ref live_entities "Non-live non-blocking".
		 * \return true if the node is a Resource,Literal or Blank.
		 */
		bool isValid() const { return isLive() ? true : this->value_->isValid(); }

		/**
		 * \ref live_entities "Non-live blocking".
		 * \return true if the node is a Literal.
		 */
		bool isLiteral() const { return node().isLiteral(); }

		/**
		 * \ref live_entities "Non-live blocking".
		 * \return true if the node is a Resource.
		 */
		bool isResource() const { return node().isResource(); }

		/**
		 * \ref live_entities "Non-live non-blocking".
		 * \return true if the node is a Blank node (anonymous).
		 */
		bool isBlank() const { return isLive() ? false : this->value_->isBlank(); }

		/**
		 * \ref live_entities "Non-live non-blocking".
		 * \return true if the node is live.
		 */
		bool isLive() const { return this->entity_.data(); }
		//@}

		/**
		 * \name Resource nodes
		 */
		//@{
		/**
		 * \ref live_entities "Non-live blocking".
		 * \return The URI if the node is a Resource node.
		 *         An null QUrl otherwise.
		 */
		QUrl uri() const { return node().uri(); }
		//@}

		//@{
		/**
		 * \ref live_entities "Non-live blocking".
		 * Alias for \ref uri
		 * \return The IRI if the node is a Resource node.
		 *         An null QUrl otherwise.
		 */
		QUrl iri() const { return node().uri(); }
		//@}

		/**
		 * \name Blank nodes
		 */
		//@{
		/**
		 * Retrieve a blank node's identifier.
		 * \ref live_entities "Non-live non-blocking".
		 * \return The node's identifier if it is a blank node, a null
		 * string otherwise.
		 */
		QString identifier() const { return isLive() ? QString() : node().identifier(); }
		//@}

		/**
		 * \name Literal nodes
		 */
		//@{
		/**
		 * \ref live_entities "Non-live blocking".
		 * \return The Literal value if the node is a Literal node.
		 *         An null QString otherwise.
		 */
		LiteralValue literal() const { return node().literal(); }

		/**
		 * \ref live_entities "Non-live blocking".
		 * \return The datatype URI of a literal node, i.e. the XML schema type
		 *         or an empty value if the node is not a literal.
		 * \sa LiteralValue::dataTypeUri
		 */
		QUrl dataType() const { return node().dataType(); }

		/**
		 * Each literal value can have an associated language, thus each property
		 * can be stored for different languages. An empty language refers to the
		 * default language.
		 *
		 * \ref live_entities "Non-live blocking".
		 * \return A string representing the language of the literal value
		 *         or an empty string if the node is not a literal.
		 */
		QString language() const { return node().language(); }
		//@}

		//@{
		/**
		 * Converts the node to a string.
		 *
		 * \ref live_entities "Non-live blocking".
		 * \return A String representation of the Node, suitable for storage,
		 * not really suitable for user readable strings.
		 *
		 * \sa LiteralValue::toString(), QUrl::toString(), toN3()
		 */
		QString toString() const { return node().toString(); }

		/**
		 * Convert a Node into N3 notation to be used in SPARQL graph patterns.
		 *
		 * \ref live_entities "Non-live blocking".
		 * \return A string representing the node in N3 encoding or an empty
		 * string for invalid nodes.
		 *
		 * \sa toString()
		 *
		 * \since 2.2
		 */
		QString toN3() const { return node().toN3(); }
		//@}


		/*!
		 * \ref live_entities "Non-live blocking".
		 * \return true if the resource has the given \a type.
		 * If non-live, always returns false;
		 */
		bool hasType(LiveNode const &type) const { return isLive() ? this->template hasTypeImpl<MD>(type) : false; }

		/*!
		 * \ref live_entities "Non-live blocking".
		 * \return true if the resource has the given \a Type_.
		 * If non-live, always returns false;
		 */
			template<typename Type_>
		bool hasType() const { return isLive() ? this->template hasTypeImpl<MD, Type_>() : false; }

		/*!
		 * Assigns a live \a model into this resource list, making it live.
		 * Blocking state depends on the active policy reconstruct method.
		 */
		ValueOfLiveResource &operator=(LiveResourcePtr resource)
		{
			this->template destruct<MD>();
			this->entity_ = resource;
			return *this;
		}

		/*!
		 * Assigns a non-live set \a nodes of items into this node list, making it non-live.
		 */
		ValueOfLiveResource &operator=(Value const &node)
		{
			this->template destruct<MD>();
			this->value_ = this->template copyValue<MD>(node);
			return *this;
		}

		/*!
		 * Blocking state depends on the active policy reconstruct method.
		 * Assigns an \a other \ref LiveBag into this resource list.
		 * The resulting live state will be the same as with \a other.
		 * \param other the other list to be assigned.
		 */
			template<typename OtherResourceType_, typename OtherPolicy_>
		ValueOfLiveResource &operator=(ValueOfLiveResource<OtherResourceType_, OtherPolicy_> const &other)
		{
			this->template assign<MD>(other);
			return *this;
		}

		ValueOfLiveResource &operator=(ValueOfLiveResource const &other)
		{
			this->template assign<MD>(other);
			return *this;
		}

		friend QDebug operator<<(QDebug s, ValueOfLiveResource const &value)
		{
			s.nospace() << value.node();
			return s;
		}
	};

	/*!
	 * Base policy class inherited by all \ref LiveBag policies.
	 */
	struct Q_DECL_EXPORT ValueOfLiveResource_Policy
		: ValueOfLiveEntity_Policy
	{
			template<typename This_>
		typename This_::ValuePointer evaluateValue() const
		{	return &md<This_>().entity()->node(); }

			template<typename This_>
		inline RDFVariable evaluateVariable() const;

			template<typename Resource_>
		struct Defines
		{
			typedef Node Value;
			typedef Node const *ValuePointer;
			typedef Resource_ Entity;
			typedef QSharedPointer<Entity> EntityPointer;
		};
	};

	namespace AutoGen
	{
			template<typename T_>
		struct Q_DECL_EXPORT Value
		{
			typedef T_ Type;
				template<typename R_, typename NP_, typename MD_>
			static inline T_ fromNode(ValueOfLiveResource<R_, NP_, MD_> const &node) { return node.literal().variant().template value<T_>(); }
			static inline T_ fromNode(Node const &node) { return node.literal().variant().template value<T_>(); }
			static inline Node toNode(T_ const &value) { return Node(LiteralValue(value)); }
			static CppType metaType() { return CppType(qMetaTypeId<T_>()); }
		};


			template<>
		struct Q_DECL_EXPORT Value<QString>
		{
			typedef QString Type;
				template<typename R_, typename NP_, typename MD_>
			static inline QString fromNode(ValueOfLiveResource<R_, NP_, MD_> const &node) { return node.toString(); }
			static inline QString fromNode(Node const &node) { return node.toString(); }
			static inline Node toNode(QString const &value) { return Node(LiteralValue(value)); }
			static CppType metaType() { return CppType(QVariant::String); }
		};

			template<>
		struct Q_DECL_EXPORT Value<QUrl>
		{
			typedef QUrl Type;
				template<typename R_, typename NP_, typename MD_>
			static inline QUrl fromNode(ValueOfLiveResource<R_, NP_, MD_> const &node) { return node.uri(); }
			static inline QUrl fromNode(Node const &node) { return node.uri(); }
			static inline Node toNode(QUrl const &value) { return Node(value); }
			static CppType metaType() { return CppType(QVariant::Url); }
		};

			template<>
		struct Q_DECL_EXPORT Value<LiveNode>
		{
			typedef LiveNode Type;
			static inline LiveNode const &fromNode(LiveNode const &node) { return node; }
			static inline LiveNode const &toNode(LiveNode const &value) { return value; }
			static CppType metaType() { return CppType(QVariant::Url); }
		};

			template<typename Interface_>
		struct Q_DECL_EXPORT Value<Live<Interface_> >
		{
			typedef Live<Interface_> Type;
			static inline Live<Interface_> const &fromNode(Live<Interface_> const &node) { return node; }
			static inline Live<Interface_> const &toNode(Live<Interface_> const &value) { return value; }
			static CppType metaType() { return CppType(QVariant::Url); }
		};

			template<typename Interface_>
		struct Q_DECL_EXPORT Value<StrictLive<Interface_> >
		{
			typedef StrictLive<Interface_> Type;
			static inline StrictLive<Interface_> const &fromNode(StrictLive<Interface_> const &node) { return node; }
			static inline StrictLive<Interface_> const &toNode(StrictLive<Interface_> const &value) { return value; }
			static CppType metaType() { return CppType(QVariant::Url); }
		};

			template<typename Property_, bool literal = (Property_::BaseStrategy & RDFStrategy::Literal)>
		struct Q_DECL_EXPORT Property
			: Value<typename Property_::Range>
		{};

			template<typename Property_>
		struct Q_DECL_EXPORT Property<Property_, false>
			: Value<Live<typename Property_::Range> >
		{};

		struct BaseClassInfo;

			template<typename T_>
		Q_DECL_EXPORT BaseClassInfo const &baseClassInfo();

		struct Q_DECL_EXPORT BaseClassInfo
			: QMap<QUrl, BaseClassInfo const *>
		{
				template<typename List_>
			void addBaseClassList()
			{
				(*this)[List_::first_type::iri()] = &baseClassInfo<typename List_::first_type>();
				addBaseClassList<typename List_::second_type>();
			}

		};

			template<>
		Q_DECL_EXPORT void BaseClassInfo::addBaseClassList<Detail::EmptyBase>();

			template<typename T_>
		Q_DECL_EXPORT BaseClassInfo const &baseClassInfo()
		{
			static BaseClassInfo *ret = 0;
			if(!ret)
			{
				ret = new BaseClassInfo();
				ret->addBaseClassList<typename T_::BaseClasses>();
			}
			return *ret;
		}
	}


	// TODO: deprecate these after the ABI break has happened
	struct QDebugHelper : QDebug
	{
		QDebugHelper(QDebug const &cp);
		~QDebugHelper();
	};
	QDebugHelper Q_DECL_EXPORT localCritical(unsigned verbosity);
	QDebugHelper Q_DECL_EXPORT localError(unsigned verbosity);
	QDebugHelper Q_DECL_EXPORT localWarning(unsigned verbosity);

	int Q_DECL_EXPORT setVerbosity(int level = -1);
	void Q_DECL_EXPORT setLogFile(QString const &fileName = QString());
}
Q_DECLARE_METATYPE(SopranoLive::AutoGen::BaseClassInfo);

#endif // SOPRANOLIVE_LIVE_H
