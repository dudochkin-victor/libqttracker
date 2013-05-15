/*
 * rowstoreblock_p.cpp
 *
 *  Created on: Oct 29, 2009
 *      Author: iridian
 */

#include <QStringList>
#include "rowstoreblock_p.h"

namespace SopranoLive
{
	RowStoreBlock::RowStoreBlock() : Base() {}
	RowStoreBlock::RowStoreBlock(int size) : Base(size) {}
	RowStoreBlock::RowStoreBlock(int size, QStringList const &value) : Base(size, value) {}
	RowStoreBlock::RowStoreBlock(QVector<Type> const &other) : Base(other) {}
	RowStoreBlock::RowStoreBlock(RowStoreBlock const &other) : Base(other), data_roles(other.data_roles) {}

	RowStoreBlock::DataRoles::const_iterator RowStoreBlock::rolesBegin(int column) const
	{
		return data_roles.lowerBound(quint64(column) << 32);
	}
	RowStoreBlock::DataRoles::const_iterator RowStoreBlock::rolesEnd(int column) const
	{
		return data_roles.upperBound((quint64(column) << 32) + 0xffffffff);
	}

	int RowStoreBlock::roleFromIterator(DataRoles::const_iterator const &iter)
	{
		return iter.key() & 0xffffffff;
	}


	QVariant const *RowStoreBlock::dataRole(int row, int column, int role) const
	{
		DataRoles::const_iterator cidr = data_roles.find((quint64(column) << 32) + role);
		if(cidr == data_roles.end() || cidr->size() <= row)
			return 0;
		QVariant const &ret = (*cidr)[row];
		if(!ret.isValid())
			return 0;
		return &ret;
	}
	void RowStoreBlock::setDataRole(int row, int column, QVariant const &data, int role)
	{
		QVector<QVariant> &vec = data_roles[(quint64(column) << 32) + role];
		if(vec.size() <= row)
		{
			if(vec.capacity() < size())
				vec.reserve(size());
			vec.resize(row + 1);
		}
		vec[row] = data;
	}

	QMap<int, QVariant> &RowStoreBlock::dataRoles(int row, int column, QMap<int, QVariant> &ret) const
	{
		for(DataRoles::const_iterator cri = data_roles.lowerBound(quint64(column) << 32), criend = data_roles.lowerBound(quint64(column + 1) << 32)
				; cri != criend; ++cri)
			if(cri->size() >= row)
				ret.insert(cri.key() & 0xFFFFFFFF, (*cri)[row]);
		return ret;
	}

	void RowStoreBlock::setDataRoles(int row, int column, QMap<int, QVariant> const &roles)
	{
		for(QMap<int, QVariant>::const_iterator cri = roles.begin(), criend = roles.end(); cri != criend; ++cri)
			setDataRole(row, column, *cri, cri.key());
	}

	int RowStoreBlock::clearDataRoleRowRange(int row, int row_count, int column, int role)
	{
		DataRoles::iterator dri = data_roles.find((quint64(column) << 32) + role);
		int cleared_rows = 0;
		if(dri != data_roles.end() && dri->size() <= row)
		{
			cleared_rows = dri->size() - row;
			if(cleared_rows <= row_count)
				dri->resize(row);
			else
			{
				cleared_rows = row_count;
				for(int i = row, iend = row + cleared_rows; i < iend; ++i)
					(*dri)[i] = QVariant();
			}
		}
		return cleared_rows;
	}


	RowStoreBlock::iterator RowStoreBlock::erase(iterator pos)
	{
		int index = pos - Base::begin();
		for(DataRoles::iterator di = data_roles.begin(), diend = data_roles.end(); di != diend; ++di)
			if(index < di->size())
				di->erase(di->begin() + index);
		return Base::erase(pos);
	}
	RowStoreBlock::iterator RowStoreBlock::erase(iterator begin, iterator end)
	{
		int begin_index = begin - Base::begin(), end_index = end - Base::begin();
		for(DataRoles::iterator di = data_roles.begin(), diend = data_roles.end(); di != diend; ++di)
			if(begin_index < di->size())
				di->erase(di->begin() + begin_index, (end_index < di->size() ? di->begin() + end_index : di->end()));
		return Base::erase(begin, end);
	}
	RowStoreBlock &RowStoreBlock::fill(const Type &value, int size)
	{
		data_roles.clear();
		Base::fill(value, size);
		return *this;
	}
	void RowStoreBlock::insert(int i, const Type &value)
	{
		insert(i, 1, value);
	}

	RowStoreBlock::iterator RowStoreBlock::insert(iterator before, int count, const Type &value)
	{
		int index = before - Base::begin();
		for(DataRoles::iterator di = data_roles.begin(), diend = data_roles.end(); di != diend; ++di)
			if(index < di->size())
				di->insert(di->begin() + index, count, QVariant());
		return Base::insert(before, count, value);
	}
	void RowStoreBlock::insert(int i, int count, const Type &value)
	{
		for(DataRoles::iterator di = data_roles.begin(), diend = data_roles.end(); di != diend; ++di)
			if(i < di->size())
				di->insert(di->begin() + i, count, QVariant());
		Base::insert(i, count, value);
	}
	RowStoreBlock::iterator RowStoreBlock::insert(iterator before, const Type &value)
	{
		return insert(before, 1, value);
	}

	RowStoreBlock RowStoreBlock::mid(int pos, int length) const
	{
		RowStoreBlock ret;
		static_cast<Base &>(ret) = Base::mid(pos, length);
		for(DataRoles::const_iterator di = data_roles.begin(), diend = data_roles.end(); di != diend; ++di)
			ret.data_roles.insert(di.key(), (pos < di->size() ? di->mid(pos, length) : QVector<QVariant>()));
		return ret;
	}

	void RowStoreBlock::pop_back()
	{
		int s = size();
		for(DataRoles::iterator di = data_roles.begin(), diend = data_roles.end(); di != diend; ++di)
			if(s < di->size())
				di->pop_back();
		Base::pop_back();
	}
	void RowStoreBlock::pop_front()
	{
		for(DataRoles::iterator di = data_roles.begin(), diend = data_roles.end(); di != diend; ++di)
			if(di->size())
				di->pop_front();
		Base::pop_front();
	}
	void RowStoreBlock::prepend(const Type &value)
	{
		Base::prepend(value);
	}
	void RowStoreBlock::push_front(const Type &value)
	{
		Base::push_front(value);
	}
	void RowStoreBlock::remove(int i)
	{
		for(DataRoles::iterator di = data_roles.begin(), diend = data_roles.end(); di != diend; ++di)
			if(i < di->size())
				di->erase(di->begin() + i);
		return Base::remove(i);
	}
	void RowStoreBlock::remove(int i, int count)
	{
		int end_index = i + count;
		for(DataRoles::iterator di = data_roles.begin(), diend = data_roles.end(); di != diend; ++di)
			if(i < di->size())
				di->erase(di->begin() + i, end_index < di->size() ? di->begin() + end_index : di->end());
		Base::remove(i, count);
	}
	void RowStoreBlock::reserve(int size)
	{
		int cur_size = this->size();
		for(DataRoles::iterator di = data_roles.begin(), diend = data_roles.end(); di != diend; ++di)
			if(cur_size == di->size())
				di->reserve(size);
	}
	void RowStoreBlock::resize(int size)
	{
		if(size < this->size())
			for(DataRoles::iterator di = data_roles.begin(), diend = data_roles.end(); di != diend; ++di)
				if(size < di->size())
					di->resize(size);
		Base::resize(size);
	}

	bool RowStoreBlock::operator!=(const RowStoreBlock &other) const
	{
		return Base::operator!=(other) && data_roles == other.data_roles;
	}
	RowStoreBlock RowStoreBlock::operator+(const RowStoreBlock &other) const
	{
		return (RowStoreBlock(*this) += other);
	}
	RowStoreBlock &RowStoreBlock::operator+=(const RowStoreBlock &other)
	{
		int cur_size = size();
		for(DataRoles::const_iterator cdri = other.data_roles.begin(), cdriend = other.data_roles.end(); cdri != cdriend; ++cdri)
			if(cdri->size())
			{
				QVector<QVariant> &column_role = data_roles[cdri.key()];
				if(int missing_rows = cur_size - column_role.size())
					column_role.insert(column_role.size(), missing_rows, QVariant());
				column_role += *cdri;
			}
		Base::operator+=(other);
		return *this;
	}
	RowStoreBlock &RowStoreBlock::operator<<(const RowStoreBlock &other)
	{
		return operator+=(other);
	}
	RowStoreBlock &RowStoreBlock::operator=(const RowStoreBlock &other)
	{
		Base::operator=(other);
		data_roles = other.data_roles;
		return *this;
	}
	bool RowStoreBlock::operator==(const RowStoreBlock &other) const { return !operator!=(other); }

}
