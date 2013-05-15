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
 * qmappedlinkedlist.h
 *
 *  Created on: Mar 14, 2010
 *      Author: "Iridian Kiiskinen"
 */

#ifndef SOPRANOLIVE_QFIFOSET_H_
#define SOPRANOLIVE_QFIFOSET_H_

#include <QMap>

namespace SopranoLive
{
	/*!
	 * QFifoMap behaves like a QMap, but the items are iterated in the order they were inserted.
	 */
		template<typename Key_, typename Value_>
	class QFifoMap
	{
		struct Node
		{
			Node *next_, *prev_;
			Node() : next_(this), prev_(this) {}
			Node(Node const &) : next_(this), prev_(this) {}
		} s_;
		struct ItemNode : Node { Key_ key_; Value_ value_; };
		typedef QMap<Key_, ItemNode> Map;
		Map map_;
		// copying not implemented
		QFifoMap(QFifoMap const &cp);
	public:
		struct const_iterator
		{ 	Node *n_;
			const_iterator(Node *n = 0) : n_(n) {}
			const_iterator &operator++() { n_ = n_->next_; return *this; }
			const_iterator &operator--() { n_ = n_->prev_; return *this; }
			const_iterator operator++(int) { const_iterator ret(*this); n_ = n_->next_; return ret; }
			const_iterator operator--(int) { const_iterator ret(*this); n_ = n_->prev_; return ret; }
			bool operator==(const_iterator const &rhs) { return n_ == rhs.n_; }
			bool operator!=(const_iterator const &rhs) { return n_ != rhs.n_; }
			Value_ const &value() const { return static_cast<ItemNode const *>(n_)->value_; }
			Key_ const &key() const { return static_cast<ItemNode const *>(n_)->key_; }
			Value_ const &operator*() const { return value(); }
			Value_ const *operator->() const { return &value(); }
		};
		struct iterator : const_iterator
		{
			iterator(Node *n = 0) : const_iterator(n) {}
			using const_iterator::value;
			iterator &operator++() { const_iterator::operator++(); return *this; }
			iterator &operator--() { const_iterator::operator--(); return *this; }
			iterator operator++(int)
			{ iterator ret(*this); const_iterator::operator++(0); return ret; }
			iterator operator--(int)
			{ iterator ret(*this); const_iterator::operator--(0); return ret; }
			Value_ &value() { return static_cast<ItemNode const *>(this->n_)->value_; }
			Value_ &operator*() { return value(); }
			Value_ *operator->() { return &value(); }
		};

		QFifoMap() {}

		iterator begin() { return iterator(s_.next_); }
		iterator end() { return iterator(&s_); }
		const_iterator begin() const { return const_iterator(s_.next_); }
		const_iterator end() const { return const_iterator(&s_); }
		const_iterator constBegin() const { return const_iterator(s_.next_); }
		const_iterator constEnd() const { return const_iterator(&s_); }

		iterator find(Key_ const &key)
		{
			typename Map::const_iterator mi = map_.find(key);
			return iterator(mi != map_.end() ? &mi.value() : 0);
		}
		const_iterator find(Key_ const &key) const
		{
			return const_cast<QFifoMap &>(*this).find(key);
		}

		iterator insert(Key_ const &key, Value_ const &value)
		{
			ItemNode *n = &map_[key];
			if(n->next_ == n)
			{
				n->key_ = key;
				n->value_ = value;
				n->next_ = &s_;
				n->prev_ = s_.prev_;
				s_.prev_ = s_.prev_->next_ = n;
			}
			return iterator(n);
		}
		bool contains(Key_ const &key) { return map_.contains(key); }

		QFifoMap &operator+=(QFifoMap const &other)
		{
			for(const_iterator ci = other.begin(), ciend = other.end(); ci != ciend; ++ci)
				insert(ci.key(), ci.value());
			return *this;
		}
	};

		template<typename Item_>
	class QFifoSet
		: public QFifoMap<Item_, Item_>
	{
	public:
		QFifoSet() {}

		typename QFifoMap<Item_, Item_>::iterator insert(Item_ const& item)
		{ return QFifoMap<Item_, Item_>::insert(item, item); }
	};
}
#endif /* SOPRANOLIVE_QMAPPEDLINKEDLIST_H_ */
