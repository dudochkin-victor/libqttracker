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
 * livenode.h
 *
 *  Created on: Apr 1, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_LIVENODE_H_
#define SOPRANOLIVE_LIVENODE_H_

#include "livenodemodel.h"

namespace SopranoLive
{
	/*!
	 * By-value, Live or Non-live entity.
	 * A local \ref Node for a remote entity \ref LiveResource using default policy \ref LiveResourceNodePolicy
	 * \sa \ref layer_bound
	 */
	class Q_DECL_EXPORT LiveNode
		: public ValueOfLiveResource<LiveResource, ValueOfLiveResource_Policy, LiveNode>
	{
		typedef ValueOfLiveResource<LiveResource, ValueOfLiveResource_Policy, LiveNode> Base;
	public:
		/*!
		 * \ref live_entities "Non-blocking".
		 * Default initialize to non-live empty state.
		 */
		inline LiveNode();

		/*!
		 * \ref live_entities "Non-blocking".
		 * Copy initialize to given \a cp.
		 */
		inline LiveNode(LiveNode const &cp);

		/*!
		 * \ref live_entities "Non-blocking".
		 * Copy initialize to given \a cp.
		 */
			template<typename ResourceType_, typename Policy_, typename MD_>
		inline LiveNode(ValueOfLiveResource<ResourceType_, Policy_, MD_> const &cp);

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to live state to given \a resource.
		 */
		inline LiveNode(LiveResourcePtr resource);

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to live state to given \a resource.
		 */
			template<typename Other_>
		inline LiveNode(QSharedVirtualInheritEnabledPointer<Other_, LiveResourcePtr> const &resource);

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to live state to given \a resource.
		 */
			template<typename Other_>
		inline LiveNode(QSharedPointer<Other_> const &resource);

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to live state to given \a resource.
		 */
			template<typename Other_>
		inline LiveNode(QWeakPointer<Other_> const &resource);

		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to non-live state by given \a node.
		 */
		inline LiveNode(Node const &node);
		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to non-live state by given \a iri.
		 */
		inline LiveNode(QUrl const &iri);
		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to non-live blank state identified by given \a string.
		 * Note: this cannot be used to initialize LiveNode by iri string. Use the QUrl or LiteralValue constructors for that.
		 */
		explicit inline LiveNode(QString const &string);
		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to non-live state by given \a value.
		 */
		inline LiveNode(LiteralValue const &value);
		/*!
		 * \ref live_entities "Non-blocking".
		 * Initialize to non-live state by given \a value and \a language.
         * \deprecated Use Soprano::LiteralValue::createPlainLiteral( const QString&, const LanguageTag& )
		 */
		SOPRANO_DEPRECATED LiveNode(LiteralValue const &value, QString const &language);

        /*!
		 * \ref live_entities "Non-live blocking".
         */
		inline bool operator!=(LiveNode const &other) const;
		/*!
		 * \ref live_entities "Non-live blocking".
         */
		inline bool operator==(LiveNode const &other) const;

		inline LiveNode &operator=(LiveNode const &other);
	};

	struct Q_DECL_EXPORT ValueOfOmniTypeLiveResource_Policy
		: ValueOfLiveResource_Policy
	{
		bool hasType(LiveNode const &type) const { Q_UNUSED(type); return true; }

			template<typename Type_>
		bool hasType() const { return true; }
	};

	struct Q_DECL_EXPORT OmniTypeLiveNode
		: ValueOfLiveResource<LiveResource, ValueOfOmniTypeLiveResource_Policy, OmniTypeLiveNode>
	{
		inline OmniTypeLiveNode(LiveNode const &livenode);
	};


	inline LiveNode LiveNodeModel::liveNode(QModelIndex const &index, LiveNode const &ensured_type) const
	{	return liveNode(index, ensured_type, false); }

	inline LiveNode LiveNodeModel::liveNode(int row, int column, QModelIndex const &parent, LiveNode const &ensured_type) const
	{	return liveNode(index(row, column, parent), ensured_type, false); }

		template<typename Type_>
	inline SopranoLive::Live<Type_> LiveNodeModel::liveResource(QModelIndex const &index, bool statically_known_type) const
	{	return SopranoLive::Live<Type_>(liveNode(index, statically_known_type ? LiveNode() : Type_::iri(), false), true); }

		template<typename Type_>
	inline SopranoLive::Live<Type_> LiveNodeModel::liveResource(int row, int column, QModelIndex const &parent, bool statically_known_type) const
	{	return SopranoLive::Live<Type_>(liveNode(index(row, column, parent), statically_known_type ? LiveNode() : Type_::iri(), false), true); }


	inline LiveNode LiveNodeModel::strictLiveNode(QModelIndex const &index, LiveNode const &strict_type) const
	{	return liveNode(index, strict_type, true); }

	inline LiveNode LiveNodeModel::strictLiveNode(int row, int column, QModelIndex const &parent, LiveNode const &strict_type) const
	{	return liveNode(index(row, column, parent), strict_type, true); }

		template<typename Type_>
	inline StrictLive<Type_> LiveNodeModel::strictLiveResource(QModelIndex const &index, bool statically_known_type) const
	{	return StrictLive<Type_>(liveNode(index, statically_known_type ? LiveNode() : Type_::iri(), true), true); }

		template<typename Type_>
	inline StrictLive<Type_> LiveNodeModel::strictLiveResource(int row, int column, QModelIndex const &parent, bool statically_known_type) const
	{	return StrictLive<Type_>(liveNode(index(row, column, parent), statically_known_type ? LiveNode() : Type_::iri(), true), true); }

}
Q_DECLARE_METATYPE(SopranoLive::LiveNode)

#endif /* SOPRANOLIVE_LIVENODE_H_ */
