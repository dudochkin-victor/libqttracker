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
 * transitionmodel.h
 *
 *  Created on: Apr 17, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_TRANSITIONMODEL_H_
#define SOPRANOLIVE_TRANSITIONMODEL_H_

#include <QtGui/QAbstractProxyModel>

namespace SopranoLive
{
	class Q_DECL_EXPORT QTransitionProxyModel
		: QAbstractProxyModel
	{
	public:
		/*!
		 * \param identity_row_index specifies the column that is used as the identity for transitioning.
		 * \param initial_model the initial proxied model.
		 */
		QTransitionProxyModel(int identity_column = 0, QAbstractItemModel *initial_model = 0);

		/*!
		 * Overridden from QAbstractItemModel
		 */
		int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
		QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
		QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
		QModelIndex parent ( const QModelIndex & index ) const;
		int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

		/*!
		 * Overridden from QAbstractProxyModel
		 */
		QModelIndex mapToSource ( const QModelIndex & proxyIndex ) const;

		/*!
		 * Transitions from the old contents to the new contents,
		 * using update for rows for which the identity is the same.
		 */
		void setSourceModel ( QAbstractItemModel * sourceModel ) const;
	private:
		struct Data;
		Data *d;
		friend class Data;
	};
}

#endif /* SOPRANOLIVE_TRANSITIONMODEL_H_ */
