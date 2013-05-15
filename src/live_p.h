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
 * live.h
 *
 *  Created on: Jan 19, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_LIVE_P_H_
#define SOPRANOLIVE_LIVE_P_H_

#include "utils/visualizer.h"
#include "include/sopranolive/liveautogenerationbase.h"

namespace SopranoLive
{
	using namespace Ontologies::Core;

	class TripleModel;
	typedef QSharedPointer<TripleModel> TripleModelPtr;

	class RowStoreModel;
	typedef QSharedPointer<RowStoreModel> RowStoreModelPtr;

	enum StaticMode { NonStatic, StaticConstruct, StaticDestruct };

	//! true when performing static resource construction or deletion
	extern int static_phase_g;

		template<typename Resource_>
	struct SharedNull
		: Resource_
	{
		/* VZR_CLASS(SharedNull, (Resource_));*/

		SharedNull(int = (SopranoLive::static_phase_g = StaticConstruct))
			: Resource_()
		{
			// we are automatic, but we inherit from QSharedData.
			// Prevent destruction.
			Resource_::ref.ref();

			SopranoLive::static_phase_g = NonStatic;
		}
		~SharedNull()
		{
			SopranoLive::static_phase_g = StaticDestruct;
		}
	private:
		SharedNull(SharedNull const &cp);
		void *operator new(size_t);
	};

	extern QStringList const rdf_type_as_list_g;

	namespace Contexts
	{
		SOPRANOLIVE_DECLARE_IRI_CLASS(NoChecks, "http://www.sopranolive.org/contexts/NoChecks/");
	}

	template<typename T> T const &constify(T &val) { return val; }
}

#endif /* SOPRANO_LIVE_H_ */
