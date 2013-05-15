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
 * qsharedfromthis.h
 *
 *  Created on: Jan 12, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_QSHAREDOBJECT_H_
#define SOPRANOLIVE_QSHAREDOBJECT_H_

#include <QtCore/QSharedPointer>
#include <QtCore/QObject>
#include <QtCore/QVariant>

class Q_DECL_EXPORT QSharedEmptyBase {};
class Q_DECL_EXPORT QSharedAbstractBase { public: virtual ~QSharedAbstractBase() {} };

typedef QSharedPointer<QObject> QSharedObjectPtr;
typedef QSharedPointer<QObject> QSharedConstObjectPtr;
typedef QWeakPointer<QObject> QWeakObjectPtr;

Q_DECLARE_METATYPE(QSharedObjectPtr);
Q_DECLARE_METATYPE(QWeakObjectPtr);

namespace QtSharedObject
{
	Q_DECL_EXPORT const char *outputDeletedShared();
	Q_DECL_EXPORT void internalUnInitializedSharedFromThis();

	struct Q_DECL_EXPORT NullDeleter
	{
		void operator()(void *) {}
	};

	struct Q_DECL_EXPORT QObjectDeleter
	{
		void operator()(QObject *object);
	};

		template<typename Type_>
	struct Q_DECL_EXPORT SimpleDeleter
		: QObjectDeleter
	{
		using QObjectDeleter::operator();

			template<typename Type2_>
		void operator()(Type2_ *object)
		{
#if !defined(QT_NO_DEBUG) && (QT_VERSION < 0x040600)
			QtSharedPointer::internalSafetyCheckRemove(object);
#endif
			delete object;
		}
	};


	//! \sa QSharedFromThis::aggregateFromThis
		template<typename AggregatorHolder_>
	struct Q_DECL_EXPORT AggregateDeleter
	{
		AggregateDeleter(AggregatorHolder_ const &aggregator)
			: aggregator_(aggregator) {}

		AggregatorHolder_ aggregator_;

		void operator()(void *)
		{
			aggregator_ = AggregatorHolder_();
		}
	};

	struct Q_DECL_EXPORT SharedParentDeleter
	{
			template<typename Type_>
		void operator()(Type_ *object)
		{
			// dont delete the object, just removed shared parent pointer.
			// if the resulting release cascade reaches the topmost parent, all
			// children get deleted.
			object->setProperty("p_q_shared_from_this", QVariant());
			object->setProperty("p_q_shared_parent", QVariant());
		}
	};

	Q_DECL_EXPORT QSharedObjectPtr sharedFromObject(QObject *object);
	Q_DECL_EXPORT void setSharedParent(QObject *object, QObject *parent);
}

	template<typename Derived_, typename Base = QSharedEmptyBase>
class Q_DECL_EXPORT QSharedFromThis
	: public Base
{
	mutable QWeakPointer<Derived_> shared_from_this;
protected:
		template<typename SharedFromThisPrimer_>
	inline SharedFromThisPrimer_ const &primeThis(SharedFromThisPrimer_ const &shared_from_this_primer)
	{ shared_from_this = shared_from_this_primer; return shared_from_this_primer; }
public:
	typedef QSharedPointer<Derived_> SharedPointer;
	typedef QSharedPointer<Derived_> ConstSharedPointer;

	inline QSharedPointer<Derived_> sharedFromThis()
	{
		QSharedPointer<Derived_> ret = shared_from_this;
#ifndef QT_NO_DEBUG
		if(!ret)
			QtSharedObject::internalUnInitializedSharedFromThis();
#endif // QT_NO_DEBUG
		return ret;
	}

	inline QSharedPointer<Derived_ const> sharedFromThis() const
	{
		QSharedPointer<Derived_> ret = shared_from_this;
#ifndef QT_NO_DEBUG
		if(!ret)
			QtSharedObject::internalUnInitializedSharedFromThis();
#endif // QT_NO_DEBUG
		return ret;
	}

		template<typename UpCasted_>
	QSharedPointer<UpCasted_> sharedFromThis()
	{
		return qSharedPointerCast<UpCasted_>(sharedFromThis());
	}

		template<typename UpCasted_>
	QSharedPointer<UpCasted_ const> sharedFromThis() const
	{
		return qSharedPointerCast<UpCasted_ const>(sharedFromThis());
	}

		template<typename MostDerived_>
	QSharedPointer<MostDerived_> initialSharedFromThis()
	{
		if(shared_from_this)
			return sharedFromThis<MostDerived_>();
		QSharedPointer<MostDerived_> ret(static_cast<MostDerived_ *>(this), QtSharedObject::SimpleDeleter<MostDerived_>());
		shared_from_this = ret;
		return ret;
	}

		template<typename MostDerived_, typename Deleter_>
	QSharedPointer<MostDerived_> initialSharedFromThis(Deleter_ deleter)
	{
		if(shared_from_this)
			return sharedFromThis<MostDerived_>();
		QSharedPointer<MostDerived_> ret(static_cast<MostDerived_ *>(this), deleter);
		shared_from_this = ret;
		return ret;
	}

		template<typename MostDerived_>
	QSharedPointer<MostDerived_> initialStackShare()
	{
		return initialSharedFromThis<MostDerived_>(QtSharedObject::NullDeleter());
	}

	/*!
	 * \return a shared pointer to this aggregate of the given \a aggregator.
	 * An aggregate is an object whose lifetime is managed by the aggregator,
	 * but which can be shared, forwarding any shared pointers to the
	 * aggregator as well. This guarantees that as long as the aggregated
	 * object is shared, the aggregator will not get destroyed. Once the
	 * aggregate no longer is shared, the AggregateDeleter QSharedPointer
	 * deleter will release the share on the aggregator as well.
	 * Initially and whenever an aggregate is not shared, it shall be accessed
	 * and shared only by the aggregator, using this function.
	 */
		template<typename MostDerived_, typename Aggregator_>
	QSharedPointer<MostDerived_> aggregateFromThis(Aggregator_ *aggregator)
	{
		if(shared_from_this)
			return sharedFromThis<MostDerived_>();
		QSharedPointer<MostDerived_> ret(static_cast<MostDerived_ *>(this),
				QtSharedObject::AggregateDeleter<QSharedPointer<Aggregator_> >(
						aggregator->initialSharedFromThis<Aggregator_>()));
		shared_from_this = ret;
		return ret;
	}
};

	template<typename Base = QObject>
class Q_DECL_EXPORT QSharedObject
	: public QSharedFromThis<QObject, Base>
{
public:
	inline QSharedObjectPtr sharedFromThis()
	{
		return QtSharedObject::sharedFromObject(this);
	}

	inline QSharedConstObjectPtr sharedFromThis() const
	{
		return QtSharedObject::sharedFromObject(const_cast<QSharedObject *>(this));
	}

		template<typename UpCasted_>
	QSharedPointer<UpCasted_> sharedFromThis()
	{
		return qSharedPointerCast<UpCasted_>(this->sharedFromThis());
	}

		template<typename UpCasted_>
	QSharedPointer<UpCasted_ const> sharedFromThis() const
	{
		return qSharedPointerCast<UpCasted_ const>(sharedFromThis());
	}

		template<typename MostDerived_>
	QSharedPointer<MostDerived_> initialSharedFromThis()
	{
		return this->primeThis(qSharedPointerCast<MostDerived_>(QtSharedObject::sharedFromObject(this)));
	}

		template<typename MostDerived_, typename Deleter_>
	QSharedPointer<MostDerived_> initialSharedFromThis(Deleter_ deleter)
	{
		QVariant shared_from_this = this->property("p_q_shared_from_this");
		if(shared_from_this.isValid())
			// perhaps we should whine here? Complain a bit, for we are already shared.
			return qSharedPointerCast<MostDerived_>(shared_from_this.value<QWeakObjectPtr>());

		QSharedPointer<MostDerived_> ret(static_cast<MostDerived_ *>(this), deleter);
		this->setProperty("p_q_shared_from_this", QVariant::fromValue(QSharedObjectPtr(ret)));
		return primeThis(ret);
	}

};

	template<typename Derived_>
QSharedPointer<Derived_> initialSharedFromThis(Derived_ *obj)
{
	return obj->initialSharedFromThis<Derived_>();
}

	template<typename Derived_, typename Deleter_>
QSharedPointer<Derived_> initialSharedFromThis(Derived_ *obj, Deleter_ deleter)
{
	return obj->initialSharedFromThis<Derived_>(deleter);
}

	template<typename Derived_, typename Aggregator_>
QSharedPointer<Derived_> aggregatedSharedFromThis(Derived_ *obj, Aggregator_ *aggregator)
{
	return obj->aggregatedSharedFromThis<Derived_>(aggregator);
}

	template<typename Type_>
class Q_DECL_EXPORT QSharedCreator
{
public:
	static QSharedPointer<Type_> create()
	{
		return (new Type_())->initialSharedFromThis<Type_>();
	}

		template<typename P1_>
	static QSharedPointer<Type_> create(P1_ &p1)
	{
		return (new Type_(p1))->initialSharedFromThis<Type_>();
	}

		template<typename P1_>
	static QSharedPointer<Type_> create(P1_ const &p1)
	{
		return (new Type_(p1))->initialSharedFromThis<Type_>();
	}

		template<typename P1_, typename P2_>
	static QSharedPointer<Type_> create(P1_ &p1, P2_ &p2)
	{
		return (new Type_(p1, p2))->initialSharedFromThis<Type_>();
	}

		template<typename P1_, typename P2_>
	static QSharedPointer<Type_> create(P1_ const &p1, P2_ const &p2)
	{
		return (new Type_(p1, p2))->initialSharedFromThis<Type_>();
	}

		template<typename P1_, typename P2_, typename P3_>
	static QSharedPointer<Type_> create(P1_ &p1, P2_ &p2, P3_ &p3)
	{
		return (new Type_(p1, p2, p3))->initialSharedFromThis<Type_>();
	}

		template<typename P1_, typename P2_, typename P3_>
	static QSharedPointer<Type_> create(P1_ const &p1, P2_ const &p2, P3_ const &p3)
	{
		return (new Type_(p1, p2, p3))->initialSharedFromThis<Type_>();
	}

		template<typename P1_, typename P2_, typename P3_, typename P4_>
	static QSharedPointer<Type_> create(P1_ &p1, P2_ &p2, P3_ &p3, P4_ &p4)
	{
		return (new Type_(p1, p2, p3, p4))->initialSharedFromThis<Type_>();
	}

		template<typename P1_, typename P2_, typename P3_, typename P4_>
	static QSharedPointer<Type_> create(P1_ const &p1, P2_ const &p2, P3_ const &p3, P4_ const &p4)
	{
		return (new Type_(p1, p2, p3, p4))->initialSharedFromThis<Type_>();
	}

		template<typename P1_, typename P2_, typename P3_, typename P4_, typename P5_>
	static QSharedPointer<Type_> create(P1_ &p1, P2_ &p2, P3_ &p3, P4_ &p4, P5_ &p5)
	{
		return (new Type_(p1, p2, p3, p4, p5))->initialSharedFromThis<Type_>();
	}

		template<typename P1_, typename P2_, typename P3_, typename P4_, typename P5_>
	static QSharedPointer<Type_> create(P1_ const &p1, P2_ const &p2, P3_ const &p3, P4_ const &p4, P5_ const &p5)
	{
		return (new Type_(p1, p2, p3, p4, p5))->initialSharedFromThis<Type_>();
	}

		template<typename P1_, typename P2_, typename P3_, typename P4_, typename P5_, typename P6_>
	static QSharedPointer<Type_> create(P1_ &p1, P2_ &p2, P3_ &p3, P4_ &p4, P5_ &p5, P6_ &p6)
	{
		return (new Type_(p1, p2, p3, p4, p5, p6))->initialSharedFromThis<Type_>();
	}

		template<typename P1_, typename P2_, typename P3_, typename P4_, typename P5_, typename P6_>
	static QSharedPointer<Type_> create(P1_ const &p1, P2_ const &p2, P3_ const &p3, P4_ const &p4, P5_ const &p5, P6_ const &p6)
	{
		return (new Type_(p1, p2, p3, p4, p5, p6))->initialSharedFromThis<Type_>();
	}
};


	template<typename Interface_, typename SharedHolder_>
class Q_DECL_EXPORT QSharedVirtualInheritEnabledPointer
{
public:
		// should be private
	Interface_ *pointer;
	SharedHolder_ holder;

	QSharedVirtualInheritEnabledPointer()
		: pointer(0), holder() {}

	explicit QSharedVirtualInheritEnabledPointer(Interface_ *ptr)
		: pointer(ptr), holder(ptr) {}

	QSharedVirtualInheritEnabledPointer(QSharedVirtualInheritEnabledPointer const &cp)
		: pointer(cp.pointer), holder(cp.holder) {}

	QSharedVirtualInheritEnabledPointer(QSharedPointer<Interface_> const &cp)
		: pointer(cp.data()), holder(cp) {}

	QSharedVirtualInheritEnabledPointer(QWeakPointer<Interface_> const &cp)
		: pointer(cp.data()), holder(cp) {}

		template<typename Other_>
	QSharedVirtualInheritEnabledPointer(QSharedPointer<Other_> const &cp)
		: pointer(cp.data()), holder(cp) {}

		template<typename Other_>
	QSharedVirtualInheritEnabledPointer(QWeakPointer<Other_> const &cp)
		: pointer(cp.data()), holder(cp) {}

	QSharedVirtualInheritEnabledPointer(Interface_ *ptr, SharedHolder_ const &holder)
		: pointer(ptr), holder(holder) {}

	Interface_ *operator->() const { return pointer; }
	Interface_ &operator*() const { return *pointer; }

	Interface_ *data() const { return pointer; }

		template<typename X>
	QSharedVirtualInheritEnabledPointer<X, SharedHolder_> dynamicCast() const
	{
		X *ret = dynamic_cast<X *>(pointer);
		if(ret)
			return QSharedVirtualInheritEnabledPointer<X, SharedHolder_>(ret, holder);
		return QSharedVirtualInheritEnabledPointer<X, SharedHolder_>();
	}
	operator bool() const { return pointer; }
	bool operator!() const { return !pointer; }

		template<typename I_, typename Sh_>
	bool operator!=(const QSharedVirtualInheritEnabledPointer<I_, Sh_> &ptr2) const { return pointer != ptr2.pointer; }
		template<typename Ptr_>
	bool operator!= (const Ptr_ *ptr2) const { return pointer != ptr2; }
		template<typename Ptr_>
	friend bool operator!= (const Ptr_ *ptr1, const QSharedVirtualInheritEnabledPointer<Interface_, SharedHolder_> &ptr2) { return ptr1 != ptr2.pointer; }
		template<typename I_, typename Sh_>
	bool operator==(const QSharedVirtualInheritEnabledPointer<I_, Sh_> &ptr2) const { return pointer == ptr2.pointer; }
		template<typename Ptr_>
	bool operator== (const Ptr_ *ptr2) const { return pointer == ptr2; }
		template<typename Ptr_>
	friend bool operator== (const Ptr_ *ptr1, const QSharedVirtualInheritEnabledPointer<Interface_, SharedHolder_> &ptr2) { return ptr1 == ptr2.pointer; }
};

#endif /* SOPRANOLIVE_QSHAREDOBJECT_H_ */
