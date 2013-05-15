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
 * qsharedobject.cpp
 *
 *  Created on: Jan 29, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include <QDebug>
#include <QAbstractEventDispatcher>
#include "include/sopranolive/qsharedobject.h"

namespace QtSharedObject
{
	const char *outputdeletedshared = "Debug deleted shared";
	const char *outputDeletedShared()
	{
		return outputdeletedshared;
	}


	void internalUnInitializedSharedFromThis()
	{
		qFatal("QSharedObject: sharedFromThis accessed before initialSharedFromThis was called");
	}

	void QObjectDeleter::operator()(QObject *object)
	{
#if !defined(QT_NO_DEBUG) && (QT_VERSION < 0x040600)
		QtSharedPointer::internalSafetyCheckRemove(object);
#endif

#define SOPRANOLIVE_IMMEDIATE_DELETION 0

		// once a QObject is no longer associated with thread
		// (if it was released as part of a transferred shared pointer)
		// or if the thread does't have an event dispatcher (which is
		// the original reason for using deleteLater in the first place)
		// delete object directly.
		QThread *t = 0;;
		if(SOPRANOLIVE_IMMEDIATE_DELETION
				|| !(t = object->thread())
				|| !QAbstractEventDispatcher::instance(t))
			delete object;
		else
			object->deleteLater();
	}

	QSharedObjectPtr sharedFromObject(QObject *object)
	{
		QVariant shared_from_this = object->property("p_q_shared_from_this");
		if(shared_from_this.isValid())
			return shared_from_this.value<QWeakObjectPtr>();

		QObject *parent = object->parent();

		QSharedObjectPtr shared_to_this;
		if(!parent)
			// by default, use direct deleter. Make sure objects are kept alive for the
			// duration of signal emissions, on the emitting site.
			shared_to_this = QSharedObjectPtr(object);
			// this object isn't shared yet. Share it using qobject deleter
			// shared_to_this = QSharedObjectPtr(object, QObjectDeleter());
		else
		{
			// Share the parent, and hold a shared pointer from \a object to it.
			object->setProperty("p_q_shared_parent", QVariant::fromValue(sharedFromObject(parent)));

			// we have a shared parent, use shared parent deleter
			shared_to_this = QSharedObjectPtr(object, SharedParentDeleter());
		}
		object->setProperty("p_q_shared_from_this", QVariant::fromValue(QWeakObjectPtr(shared_to_this)));
		return shared_to_this;
	}

	void setSharedParent(QObject *object, QObject *parent)
	{
		QVariant shared_from_this = object->property("p_q_shared_from_this");
		if(shared_from_this.isValid())
		{
			if(object->parent())
			{
				qCritical() << "Cannot set an already shared, top-level (no parent) object as a child inside a shared object hierarchy";
				return;
			}
			// we are part of a shared qobject hierarchy, switch our parent shared pointer to the new one.
			object->setProperty("p_q_shared_parent", QVariant::fromValue(sharedFromObject(parent)));
		}
		object->setParent(parent);
	}

}
