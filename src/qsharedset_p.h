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
 * qsharedset_p.h
 *
 *  Created on: Mar 8, 2010
 *      Author: "Iridian Kiiskinen"
 */

#ifndef SOPRANOLIVE_QSHAREDSET_P_H_
#define SOPRANOLIVE_QSHAREDSET_P_H_

#include <QSet>
#include <QHash>

namespace SopranoLive
{
	// A QSet-like structure which shares identical set data.
	// requres qHash to be specified for the items.

	//! Shared storage for QSharedSet objects

		template<typename T_>
	struct QSharedSetStorage
	{
		// a single Types instance is shared by all resources having the same set of types
		typedef QHash<uint, QSet<T_> > SetHash;

		SetHash sets_lookup_;
		typedef typename SetHash::const_iterator Iter;

		static uint setHash(QSet<T_> const &set)
		{
			uint hash = 0;
			for(typename QSet<T_>::const_iterator i = set.begin(), iend = set.end()
					; i != iend; ++i)
				hash ^= qHash(*i);
			return hash;
		}

		Iter construct(QSet<T_> &set)
		{
			uint hash = setHash(set);
			Iter it = sets_lookup_.find(hash);
			while(it != sets_lookup_.end() && it.key() == hash)
				if(*it == set)
					return it;
			set.squeeze();
			return sets_lookup_.insertMulti(hash, set);
		}

		Iter construct(QSet<T_> const &set = QSet<T_>())
		{
			uint hash = setHash(set);
			Iter it = sets_lookup_.find(hash);
			while(it != sets_lookup_.end() && it.key() == hash)
				if(*it == set)
					return it;
			QSet<T_> inserted(set);
			// never gonna touch you up, never gonna release you down
			// (we'll never alter or remove inserted unless cache is removed)
			inserted.squeeze();
			return sets_lookup_.insertMulti(hash, inserted);
		}

		Iter subtract(Iter const &active, QSet<T_> const &removes)
		{
			QSet<T_> subtraction = *active - removes;
			if(subtraction.size() == active->size())
				return active;
			return construct(subtraction);
		}

		Iter unite(Iter const &active, QSet<T_> const &adds)
		{
			QSet<T_> unification = *active + adds;
			if(unification.size() == active->size())
				return active;
			return construct(unification);
		}

		Iter intersect(Iter const &active, QSet<T_> const &intersects)
		{
			QSet<T_> intersection = *active & intersects;
			if(intersection.size() == active->size())
				return active;
			return construct(intersection);
		}

		// if transition maps were to be implemented for each set,
		// the performance of these could be considerably improved.
		// As such, though, the performance is likely to be good
		// enough.
		Iter subtract(Iter const &active, T_ const &removed)
		{
			return subtract(active, QSet<T_>() << removed);
		}
		Iter unite(Iter const &active, T_ const &add)
		{
			return unite(active, QSet<T_>() << add);
		}
		Iter intersect(Iter const &active, T_ const &intersect)
		{
			return intersect(active, QSet<T_>() << intersect);
		}
	};

	// Behaves exactly like a QSet<T_> except that the storage must be explicitly given as
	// a ctor argument.
		template<typename T_>
	class QSharedSet
	{
		QSharedSetStorage<T_> *storage_;
		typename QSharedSetStorage<T_>::Iter set_iter_;
		QSharedSet
				( QSharedSetStorage<T_> &storage
				, typename QSharedSetStorage<T_>::Iter const &iter)
			: storage_(&storage), set_iter_(iter) {}
	public:
		/*!
		 * Constructs an invalid QSharedSet, which must be overwritten by assignment of a
		 * valid QSharedSet object. Exists to allow usage with containers requiring default ctor.
		 */
		QSharedSet()
			: storage_(0) {}
		QSharedSet(QSharedSetStorage<T_> &storage, QSet<T_> const &initial = QSet<T_>())
			: storage_(&storage), set_iter_(storage.construct(initial)) {}
		QSharedSet(QSharedSet const &cp) : storage_(cp.storage_), set_iter_(cp.set_iter_) {}

		static QSharedSet embedSet(QSharedSetStorage<T_> &storage, QSet<T_> &initial)
		{
			return QSharedSet(storage, storage.construct(initial));
		}

		typedef typename QSet<T_>::const_iterator const_iterator;
		typedef typename QSet<T_>::const_iterator iterator;

		operator QSet<T_> const &() const { return *set_iter_; }
		iterator begin () const { return set_iter_->begin(); }
		int capacity () const { return set_iter_->capacity(); }
		void clear () { set_iter_ = storage_->construct(); }
		const_iterator constBegin () const { return set_iter_->constBegin(); }
		const_iterator constEnd () const { return set_iter_->constEnd(); }
		const_iterator constFind ( const T_ & value ) const { return set_iter_->constFind(value); }
		bool contains ( const T_ & value ) const { return set_iter_->contains(value); }
		int count () const { return set_iter_->count(); }
		bool empty () const { return set_iter_->empty(); }
		iterator end () const { return set_iter_->end(); }
		// unlike QSet, QSharedSet can't give guarantees of not rehashing for erase.
		// Use remove instead.
	private:
		iterator erase ( iterator pos ) { QString &next = *(pos + 1); remove(*pos); return find(next); }
	public:
		const_iterator find ( const T_ & value ) const { return set_iter_->find(value); }
		iterator find ( const T_ & value ) { return set_iter_->find(value); }
		const_iterator insert ( const T_ & value )
		{ set_iter_ = storage_->unite(set_iter_, value); return find(value); }
		QSharedSet & intersect ( const QSet<T_> & other )
		{ set_iter_ = storage_->intersect(set_iter_, intersect); return *this; }
		bool isEmpty () const { return set_iter_->isEmpty(); }
		bool remove ( const T_ & value )
		{
			typename QSharedSetStorage<T_>::Iter prev = set_iter_;
			set_iter_ = storage_->subtract(set_iter_, value);
			return prev != set_iter_;
		}
		void reserve ( int size ) {}
		int size () const { return set_iter_->size(); }
		void squeeze () {}
		QSharedSet & subtract ( const QSet<T_> & other )
		{ set_iter_ = storage_->subtract(set_iter_, other); return *this; }
		QList<T_> toList () const { return set_iter_->toList(); }
		QSharedSet & unite ( const QSet<T_> & other )
		{ set_iter_ = storage_->unite(set_iter_, other); return *this; }
		QList<T_> values () const { return set_iter_->values(); }
		bool operator!= ( const QSet<T_> & other ) const
		{ return set_iter_->operator!=(other); }
		QSharedSet operator& ( const QSet<T_> & other )
		{ return QSharedSet(*this).intersect(other); }
		QSharedSet & operator&= ( const QSet<T_> & other )
		{ return intersect(other); }
		QSharedSet & operator&= ( const T_ & value )
		{ set_iter_ = storage_->intersect(set_iter_, value); return *this; }
		QSharedSet operator+ ( const QSet<T_> & other )
		{ return QSharedSet(*this).unite(other); }
		QSharedSet & operator+= ( const QSet<T_> & other )
		{ return unite(other); }
		QSharedSet & operator+= ( const T_ & value )
		{ set_iter_ = storage_->unite(set_iter_, value); return *this; }
		QSharedSet operator- ( const QSet<T_> & other )
		{ return QSharedSet(*this).subtract(other); }
		QSharedSet & operator-= ( const QSet<T_> & other )
		{ return unite(other); }
		QSharedSet & operator-= ( const T_ & value )
		{ set_iter_ = storage_->subtract(set_iter_, value); return *this; }
		QSharedSet & operator<< ( const T_ & value )
		{ set_iter_ = storage_->unite(set_iter_, value); return *this; }
		QSharedSet & operator= ( const QSet<T_> & other )
		{ set_iter_ = storage_->construct(other); return *this; }
		QSharedSet & operator= ( const QSharedSet & other )
		{
			if(&storage_ != &other.storage_)
				return operator=(*other.set_iter_);
			set_iter_ = other.set_iter_;
			return *this;
		}
		bool operator== ( const QSet<T_> & other ) const
		{ return set_iter_->operator==(other); }
		QSharedSet operator| ( const QSet<T_> & other )
		{ return QSharedSet(*this).unite(other); }
		QSharedSet & operator|= ( const QSet<T_> & other )
		{ return unite(other); }
		QSharedSet & operator|= ( const T_ & value )
		{ set_iter_ = storage_->unite(set_iter_, value); return *this; }

		static QSharedSet fromList(QList<T_> const &list_, QSharedSetStorage<T_> &storage)
		{
			QSet<T_> set = QSet<T_>::fromList(list_);
			return storage.construct(set);
		}
	};

		template<class T>
	QDebug operator<<(QDebug s, QSharedSet<T> const &set)
	{
		return s << set.operator QSet<T> const &();
	}
}
#endif /* SOPRANOLIVE_QSHAREDSET_P_H_ */
