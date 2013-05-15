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
 * qhasproperty.h
 *
 *  Created on: Apr 5, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_QHASPROPERTY_H_
#define SOPRANOLIVE_QHASPROPERTY_H_

#include <QSet>
#include <typeinfo>

	template
	< typename Owner_
	, typename Property_
	, typename OwnerStorage_ = Owner_ *
	, typename PropertyStorage_ = Property_*
	>
struct QHasBackReferencedProperty
{
	struct BackRef
	{
		typedef QSet<OwnerStorage_> BackRefs;
		BackRefs const &backrefs() const { return backrefs_; }

			template<typename OtherBackRef_>
		void mergeFrom(OtherBackRef_ &other)
		{
			typename BackRefs::iterator i = other.BackRef::backrefs_.begin();
			while(i != other.BackRef::backrefs_.end())
			{
				PropertyStorage_ this_as_property = PropertyStorage_(static_cast<Property_ *>(this));
				if((*i)->propertyChanging(this_as_property, *i))
				{
					insertBackRef(*i);
					(*i)->property_ = this_as_property;
					i = other.BackRef::backrefs_.erase(i);
					(*i)->propertyChanged(*i);
				} else
					++i;
			}
		}

		void clear()
		{
			while(backrefs_.size())
				(*backrefs_.begin())->setProperty(PropertyStorage_());
		}
		void removeBackRef(OwnerStorage_ owner)
		{
			backrefs_.remove(owner);
		}
		void insertBackRef(OwnerStorage_ owner)
		{
			backrefs_.insert(owner);
		}
		~BackRef()
		{
			clear();
		}
	private:
		friend struct QHasBackReferencedProperty;
		BackRefs backrefs_;
	};

	QHasBackReferencedProperty(QHasBackReferencedProperty const &cp)
		: property_()
	{
		setProperty(cp.property_);
	}

	QHasBackReferencedProperty(PropertyStorage_ property = 0)
		: property_()
	{
		setProperty(property);
	}

	PropertyStorage_ property() const { return property_; }
	void setProperty(PropertyStorage_ new_)
	{
		OwnerStorage_ this_as_owner = OwnerStorage_(static_cast<Owner_ *>(this));
		if(property_)
		{
#ifdef QHASPROPERTY_DEBUG
			qDebug() << "Removal:" << this_as_owner << " " << typeid(Owner_).name() << &*property_;
#endif
			property_->BackRef::removeBackRef(this_as_owner);
		}
		if((property_ = new_))
		{
#ifdef QHASPROPERTY_DEBUG
			qDebug() << "Insertion:" << this_as_owner << typeid(Owner_).name() << &*property_;
#endif
			property_->BackRef::insertBackRef(this_as_owner);
		}
	}

	~QHasBackReferencedProperty()
	{
		setProperty(PropertyStorage_());
	}
protected:
#ifdef QHASPROPERTY_DEBUG
	bool propertyChanging(PropertyStorage_ new_, Owner_ *)
	{
		qDebug() << "Changing:" << static_cast<Owner_ *>(this) << typeid(Owner_).name()
			<< "from" << &*property_
			<< "to" << &*new_;
#else
	bool propertyChanging(PropertyStorage_, Owner_ *)
	{
#endif
		return true;
	}
	void propertyChanged(Owner_ *) {}
private:
	PropertyStorage_ property_;
};


/* (doxygen disabled) ! Examples
 * \code

#include <QHasProperty>
#include <QString>

struct MyParent;

struct HasMyParent
	: HasBackReferencedProperty<HasMyParent, MyParent *>
{
	MyParent *parent() { return property(); }
	void setParent(MyParent *new_child) { setProperty(new_child); }

	// also purely optional
	HasMyParent(MyParent *parent, QString meta_info)
		: HasBackReferencedProperty<HasMyParent, MyParent *>(parent)
		, meta_info_(meta_info) {}
	QString meta_info_;

	// optionally these could be left out or made non-virtual
	virtual bool propertyChanging(MyParent *new_, HasMyParent *var) { return true; }
	void propertyChanged(HasMyParent *var) { qDebug() << "Property " << meta_info_ << " changed to " << parent()->name; }
};

struct MyParent : HasMyParent::BackRef
{
	QString name;
	MyParent(QString name) : name(name) {}
};

struct MyChild : HasMyParent
{
	MyChild(MyParent *parent = 0) :
		HasMyParent("MyChild::HasMyParent") {}

	bool propertyChanging(MyParent *new_, HasMyParent *property)
	{
		qDebug() << "Allowing " << property->meta_info_
			<< " change from " << property->parent()->name
			<< " to " << new_->name;
		return true;
	}
}

void test()
{
	MyParent alpha("alpha"), beta("beta");
	MyChild kid1(alpha), kid2(beta);
	kid2.setParent(var);
	foreach(HasMyParent *hasparent, var.backrefs())
		qDebug() << hasvar->meta_info_ << " = " << hasparent->parent()->name;
}

 * \endcode
 */

#endif /* SOPRANOLIVE_QHASPROPERTY_H_ */
