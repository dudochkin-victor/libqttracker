/*
 * rowstoreblock_p.h
 *
 *  Created on: Oct 29, 2009
 *      Author: iridian
 */

#ifndef SOPRANOLIVE_ROWSTOREBLOCK_P_H_
#define SOPRANOLIVE_ROWSTOREBLOCK_P_H_

#include <QVariant>
#include <QMap>
#include <QString>
#include <QList>
#include "sparsevector_p.h"

namespace SopranoLive
{
	struct RowStoreBlock
		: QVector<QStringList>
	{
		typedef QVector<QStringList> Base;

		typedef QStringList Type;

		// high 32 bits: the column index, low 32 bits: the role.
		typedef QMap<quint64, QVector<QVariant> > DataRoles;
	private:
		DataRoles data_roles;
	public:
		DataRoles const &dataRoles() const { return data_roles; }
		DataRoles &dataRoles() { return data_roles; }

		DataRoles::const_iterator rolesBegin(int column = 0) const;
		DataRoles::const_iterator rolesEnd(int column = -1) const;

		static int roleFromIterator(DataRoles::const_iterator const &iter);

		QVariant const *dataRole(int row, int column, int role) const;
		void setDataRole(int row, int column, QVariant const &data, int role);

		int clearDataRoleRowRange(int row_begin, int row_end, int column, int role);

		QMap<int, QVariant> &dataRoles(int row, int column, QMap<int, QVariant> &accumulator) const;
		QMap<int, QVariant> dataRoles(int row, int column) const { QMap<int, QVariant> ret; return dataRoles(row, column, ret); }
		void setDataRoles(int row, int column, QMap<int, QVariant> const &roles);

		// the actual display role row data is indexed by [row][column]
		// data_roles are indexed [column][row]

		RowStoreBlock();
		RowStoreBlock(int size);
		RowStoreBlock(int size, QStringList const &value);
		RowStoreBlock(QVector<Type> const &other);
		RowStoreBlock(RowStoreBlock const &other);

		iterator erase(iterator pos);
		iterator erase(iterator begin, iterator end);
		RowStoreBlock &fill(const Type &value, int size = -1);
		void insert(int i, const Type &value);
		iterator insert(iterator before, int count, const Type &value);
		void insert(int i, int count, const Type &value);
		iterator insert(iterator before, const Type &value);
		RowStoreBlock mid(int pos, int length = -1) const;
		void pop_back();
		void pop_front();
		void prepend(const Type &value);
		void push_front(const Type &value);
		void remove(int i);
		void remove(int i, int count);
		void reserve(int size);
		void resize(int size);

		bool operator!=(const RowStoreBlock &other) const;
		RowStoreBlock operator+(const RowStoreBlock &other) const;
		RowStoreBlock &operator+=(const RowStoreBlock &other);
		RowStoreBlock &operator<<(const RowStoreBlock &other);
		RowStoreBlock &operator=(const RowStoreBlock &other);
		bool operator==(const RowStoreBlock &other) const;
	};
}

#endif /* SOPRANOLIVE_ROWSTOREBLOCK_P_H_ */
