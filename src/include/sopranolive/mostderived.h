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
 * qmostderived.h
 *
 *  Created on: Mar 30, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef QMOSTDERIVED_H_
#define QMOSTDERIVED_H_

	template<typename MostDerived>
struct Q_DECL_EXPORT MostDerivedFromThis
{
	MostDerived &mostDerived() { return static_cast<MostDerived &>(*this); }
	MostDerived const &mostDerived() const { return static_cast<MostDerived const &>(*this); }
};

	template<typename This_, typename MostDerived_>
struct Q_DECL_EXPORT MostDerivedHelper
{
	typedef MostDerived_ Type;
};

	template<typename This_>
struct Q_DECL_EXPORT MostDerivedHelper<This_, void>
{
	typedef This_ Type;
};

	template<typename Parent_>
struct Q_DECL_EXPORT MostDerivedPolicy
{
protected:
		template<typename MostDerived_>
	MostDerived_ &md() { return static_cast<MostDerived_ &>(static_cast<Parent_ &>(*this)); }

		template<typename MostDerived_>
	MostDerived_ const &md() const { return static_cast<MostDerived_ const &>(static_cast<Parent_ const &>(*this)); }
};


#endif /* QMOSTDERIVED_H_ */
