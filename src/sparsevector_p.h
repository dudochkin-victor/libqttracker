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
 * sparsevector_p.h
 *
 *  Created on: Jan 7, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef SOPRANOLIVE_SPARSEVECTOR_P_H_
#define SOPRANOLIVE_SPARSEVECTOR_P_H_

#include <QPair>
#include <QVector>
#include <QDebug>
#include <iterator>

namespace SopranoLive
{

	/*!
	 * A hybrid map/vector from indexes to objects of given \a Type_.
	 * Supports very fast fast large batch inserts of given \a Block_ type
	 * which must be a vector container of Type_ objects.
	 * Internal implementation is a map from indices to Block_ objects,
	 * with Block_ object contents being implicitly shared.
	 * As a starting point, respects QMap static API.
	 */
		template<typename Type_, typename Block_ = QVector<Type_> >
	struct QSparseVector
	{
		struct BlockNode
		{
			int range, offset;
			Block_ block;
			BlockNode(int range, int offset, Block_ const &block)
				: range(range), offset(offset), block(block) {}
			BlockNode(int range = 0, Block_ const &block = Block_())
				: range(range), offset(0), block(block) {}
		};
		typedef QMap<int, BlockNode> BlockNodes;

		typedef unsigned size_type;
		typedef int difference_type;
		typedef typename Block_::value_type value_type;
		typedef typename Block_::pointer pointer;
		typedef typename Block_::const_pointer const_pointer;
		typedef typename Block_::reference reference;
		typedef typename Block_::const_reference const_reference;

		BlockNodes block_nodes;

			template<typename BlockNodeIter_, typename Container_, typename This_>
		struct iterator_base
			: std::iterator_traits<BlockNodeIter_>
		{
			BlockNodeIter_ block_node_i_;
			int index_;
			Container_ *cont_;

			iterator_base() : block_node_i_(), index_(0), cont_(0) {}

			iterator_base(iterator_base const &cp)
				: block_node_i_(cp.block_node_i_), index_(cp.index_), cont_(cp.cont_) {}

			iterator_base(BlockNodeIter_ const &bi, int index, Container_ &cont)
				: block_node_i_(bi), index_(index), cont_(&cont)
			{}

			inline bool checkForOverflowAndUnderflow()
			{
				if(index_ < 0)
					blockUnderflow();
				else if(index_ >= block_node_i_->range)
					blockOverflow();
				else return false;
				return true;
			}

			void blockOverflow()
			{
				for( BlockNodeIter_ end(cont_->block_nodes.end())
					; (block_node_i_ != end || (index_ = 0)) && index_ >= block_node_i_->range
					; ++block_node_i_)
					index_ -= block_node_i_->range;
			}
			void blockUnderflow()
			{
				for( BlockNodeIter_ begin(cont_->block_nodes.begin())
					; (block_node_i_ != begin || (index_ = 0)) && index_ < 0
					; )
					index_ += (--block_node_i_)->range;
			}

			/*!
			 * preincrement, goes to the next key or end(), valid or not
			 */
			This_ &operator++()
			{
				if(++index_ >= block_node_i_->range)
					blockOverflow();
				return static_cast<This_ &>(*this);
			}

			/*!
			 * postincrement, goes to the next key or end(), valid or not
			 */
			This_ operator++(int) { This_ ret(*this); ++*this; return ret; }

			/*!
			 * advance the iterator with the given \a offset, possibly pointing within a gap region
			 */
			This_ &operator+=(difference_type offset)
			{
				if((index_ += offset) < 0)
					blockUnderflow();
				else if(index_ >= block_node_i_->range)
					blockOverflow();
				return static_cast<This_ &>(*this);
			}
			/*!
			 * retreat the iterator with the given \a offset, possibly pointing within a gap region
			 */
			This_ &operator-=(difference_type offset) { return *this += -offset; }
			/*!
			 * \return an iterator copy which is advanced with the given \a offset, possibly pointing within a gap region
			 */
			This_ operator+(difference_type offset) const { This_ ret(*this); ret += offset; return ret; }
			/*!
			 * \return an iterator copy which is retreated with the given \a offset, possibly pointing within a gap region
			 */
			This_ operator-(difference_type offset) const { This_ ret(*this); ret -= offset; return ret; }

			/*!
			 *  \return true if currently pointing to a gap; false if pointing to valid item, undefined if end
			 */
			bool isGap() const { return !block_node_i_->block.size(); }

			/*!
			 *  \return true if currently pointing to a valid item; false if pointing to gap or end
			 */
			bool isValid() const { return cont_->block_nodes.end() != block_node_i_ && block_node_i_->block.size(); }




			/*!
			 * Advances the iterator to first valid item or end
			 */
			This_ &skipGap()
			{
				for(BlockNodeIter_ end = cont_->block_nodes.end(); (block_node_i_ != end || (index_ = 0)) && isGap(); ++block_node_i_)
					index_ = 0;
				return static_cast<This_ &>(*this);
			}

			/*!
			 * Advances the iterator to first valid item from given \a offset or end
			 */
			This_ &skipGap(difference_type offset_forward)
			{
				index_ += offset_forward;
				if(checkForOverflowAndUnderflow() || isGap())
					skipGap();
				return static_cast<This_ &>(*this);
			}

			/*!
			 * Retreats the iterator to first valid item searching backwards or begin
			 */
			This_ &retreatGap()
			{
				for(BlockNodeIter_ begin = ++cont_->block_nodes.begin(); (block_node_i_ != begin || (index_ = 0)) && isGap(); )
					index_ = (--block_node_i_)->range - 1;
				return static_cast<This_ &>(*this);
			}

			/*!
			 * Retreats the iterator to first valid item searching backwards or begin
			 */
			This_ &retreatGap(difference_type offset_backward)
			{
				index_ -= offset_backward;
				if(checkForOverflowAndUnderflow() || isGap())
					retreatGap();
				return static_cast<This_ &>(*this);
			}

			/*!
			 *  \return key of the iterated item, valid even for end
			 */
			unsigned key() const { return index_ + (block_node_i_ - 1).key(); }

			/*!
			 *  \return value of the iterated item. Accessing the value is undefined if the iterator is in a gap region.
			 */
				template<typename OtherIter_, typename OtherContainer_, typename Other_>
			bool operator!=(iterator_base<OtherIter_, OtherContainer_, Other_> const &other) const { return block_node_i_ != other.block_node_i_ || index_ != other.index_; }

				template<typename OtherIter_, typename OtherContainer_, typename Other_>
			bool operator==(iterator_base<OtherIter_, OtherContainer_, Other_> const &other) const { return !(*this != other); }

				template<typename OtherIter_, typename OtherContainer_, typename Other_>
			bool operator<(iterator_base<OtherIter_, OtherContainer_, Other_> const &other) const { return key() < other.key(); }

			value_type const &value() const { return block_node_i_->block[block_node_i_->offset + index_]; }
			value_type const &operator*() const { return value(); }
			value_type const *operator->() const { return &value(); }
			Container_ const &container() const { return *cont_; }

			Block_ const &block() const { return block_node_i_->block; }
			unsigned blockIndex() const { return block_node_i_->offset + index_; }

			//! Advances the iterator by given \a offset.
			//! \return true if the advance doesn't under/overflow, false otherwise.
			bool blockAdvance(difference_type offset)
			{
				if((index_ += offset) < 0)
					blockUnderflow();
				else if(index_ >= block_node_i_->range)
					blockOverflow();
				else
					return true;
				return false;
			}
		};

		struct iterator
			: iterator_base<typename BlockNodes::iterator, QSparseVector, iterator>
		{
			typedef iterator_base<typename BlockNodes::iterator, QSparseVector, iterator> Base;
			using Base::block_node_i_;

			iterator(Base const &cp = Base()) : Base(cp) {}
			iterator(typename BlockNodes::iterator const &bi, int index, QSparseVector &cont) : Base(bi, index, cont) {}

			iterator(unsigned row, typename BlockNodes::iterator const &bi, QSparseVector &cont)
				: Base(bi, row - bi.key() + bi->range, cont)
			{
				this->checkForOverflowAndUnderflow();
			}

			using Base::value;
			value_type &value() { return block_node_i_->block[block_node_i_->offset + Base::index_]; }
			value_type &operator*() { return value(); }
			value_type *operator->() { return &value(); }
			using Base::container;
			QSparseVector &container() { return *Base::cont_; }
			using Base::block;
			Block_ &block() { return block_node_i_->block; }
		};

		struct const_iterator
			: iterator_base<typename BlockNodes::const_iterator, QSparseVector const, const_iterator>
		{
			typedef iterator_base<typename BlockNodes::const_iterator, QSparseVector const, const_iterator> Base;

			const_iterator(Base const &cp = Base()) : Base(cp) {}
			const_iterator(iterator const &cp) : Base(cp.block_node_i_, cp.index_, *cp.cont_) {}
			const_iterator(typename BlockNodes::const_iterator const &bi, int index, QSparseVector const &cont) : Base(bi, index, cont) {}

			const_iterator(unsigned row, typename BlockNodes::const_iterator const &bi, QSparseVector const &cont)
				: Base(bi, row - bi.key() + bi->range, cont)
			{
				this->checkForOverflowAndUnderflow();
			}
		};

			/*!
			 * Return the distance between the two iterators
			 */
			template<typename LI_, typename LV_, typename L_, typename RI_, typename RV_, typename R_>
		friend difference_type operator-(iterator_base<LI_, LV_, L_> const &lhs, iterator_base<RI_, RV_, R_> const &rhs)
		{ return lhs.key() - rhs.key(); }

		const_iterator begin() const { return const_iterator(++block_nodes.begin(), 0, *this); }
		const_iterator end() const { return const_iterator(block_nodes.end(), 0, *this); }
		const_iterator constBegin() const { return const_iterator(++block_nodes.begin(), 0, *this); }
		const_iterator constEnd() const { return const_iterator(block_nodes.end(), 0, *this); }
		iterator begin() { return iterator(++block_nodes.begin(), 0, *this); }
		iterator end() { return iterator(block_nodes.end(), 0, *this); }

		const_iterator lowerBound(size_type key) const { return const_iterator(key, block_nodes.upperBound(key), *this); }
		const_iterator upperBound(size_type key) const { return const_iterator(key + 1, block_nodes.upperBound(key + 1), *this); }
		const_iterator lowerBound(size_type key) { return iterator(key, block_nodes.upperBound(key), *this); }
		const_iterator upperBound(size_type key) { return iterator(key + 1, block_nodes.upperBound(key + 1), *this); }

		/* how to do this?
		iterator nonConstIterator(const_iterator const &iter)
		{
			return iterator(iter.block_node_i, iter.index_, iter.cont_)
		}*/

		const_iterator find(size_type key) const
		{
			typename BlockNodes::const_iterator cibn = block_nodes.upperBound(key);
			if(cibn == block_nodes.end() || !cibn->block.size())
				return end();
			return const_iterator(cibn, key - cibn.key() + cibn->range, *this);
		}

		const_iterator constFind(size_type key) const
		{
			return find(key);
		}

		iterator find(size_type key)
		{
			typename BlockNodes::iterator ibn = block_nodes.upperBound(key);
			if(ibn == block_nodes.end() || !ibn->block.size())
				return end();
			return iterator(ibn, key - ibn.key() + ibn->range, *this);
		}

		QSparseVector()
		{
			block_nodes[0];
		}
		QSparseVector(size_type gap_size)
		{
			block_nodes[0];
			insertGap(0, gap_size);
		}
		QSparseVector(Block_ const &initial)
		{
			block_nodes[0];
			block_nodes[initial.size()] = BlockNode(initial.size(), initial);
		}

		QSparseVector(QSparseVector const &cp)
			: block_nodes(cp.block_nodes)
		{}

		QSparseVector &operator=(QSparseVector const &rvalue)
		{
			block_nodes = rvalue.block_nodes;
			return *this;
		}

		const_pointer index(size_type row) const
		{
			typename BlockNodes::const_iterator ei = block_nodes.upperBound(row);
			if(ei != block_nodes.end() && ei->block.size())
				return &ei->block[row - ei.key() + ei->range + ei->offset];
			return 0;
		}

		pointer index(size_type row)
		{
			typename BlockNodes::iterator ei = block_nodes.upperBound(row);
			if(ei != block_nodes.end() && ei->block.size())
				return &ei->block[row - ei.key() + ei->range + ei->offset];
			return 0;
		}

		pointer index(iterator const &hint, size_type row) const
		{
			typename BlockNodes::iterator const &bi = hint.block_node_i_;
			unsigned ind = (unsigned)row - bi.key() + bi->range;
			if(ind < bi->block.size() - bi->offset)
				return &bi->block[ind + bi->offset];
			return index(row);
		}

		reference operator[](size_type row)
		{
			if(pointer ret = index(row))
				return *ret;
			return *insert(row, value_type());
		}
		const_reference operator[](size_type row) const
		{
			// TODO: might not be semantically appropriate, alters contents even when const
			return const_cast<QSparseVector &>(*this)[row];
		}

		/*!
		 * Resets a range and ensures the given location contains a block of given length
		 * \return an iterator to the block
		 */
		typename BlockNodes::iterator resetImpl(int position, int range, Block_ const *replace = 0)
		{
			typename BlockNodes::iterator ret;
			typename BlockNodes::iterator overlapped_block = block_nodes.upperBound(position);
			int entry_position = position + range;
			if(overlapped_block != block_nodes.end())
			{
				int overl_bottom_visible = position - (overlapped_block.key() - overlapped_block->range);

				if(overl_bottom_visible)
					block_nodes[position] = BlockNode
							( overl_bottom_visible, overlapped_block->offset, overlapped_block->block);
				// overl_bottom_visible is never negative
				else if(!range)
					return --overlapped_block;

				int overl_top_visible = overlapped_block.key() - entry_position;

				if(overl_top_visible >= 0)
				{
					if(!overl_bottom_visible && !overl_top_visible)
					{
						overlapped_block->block = replace ? *replace : Block_();
						overlapped_block->offset = 0;
						return overlapped_block;
					}
					if(replace && overlapped_block->block.size())
						// is a replace and the replaced region is populated and continuous, perform copy
						// TODO: if contents get shared, this should not perform an overwrite, as it might
						// cause a huge underlying shared copy!
					{
						typename Block_::const_iterator ui = replace->begin(), uiend = replace->end();
						typename Block_::iterator ti = overlapped_block->block.begin() + overl_bottom_visible;
						while(ui != uiend)
							*ti++ = *ui++;
						return overlapped_block;
					}
				} else
					do if((overlapped_block = block_nodes.erase(overlapped_block)) == block_nodes.end())
						goto reset_precise_top_match;
					while ((overl_top_visible = overlapped_block.key() - entry_position) < 0);
				if(int up_removes = overlapped_block->range - overl_top_visible)
				{
					overlapped_block->range -= up_removes;
					overlapped_block->offset += up_removes;
				}
			} else if(int empty_range = position - (--overlapped_block).key())
				block_nodes.insert(position, BlockNode(empty_range));
			reset_precise_top_match:
			return range
				? block_nodes.insert(entry_position, BlockNode(range, replace ? *replace : Block_()))
				: block_nodes.find(position);
		}

		/*!
		 * Resets the area from given \a position of given \a size with a gap.
		 */
		iterator reset(int position, int gap_range)
		{
			return iterator(resetImpl(position, gap_range), 0, *this);
		}

		/*!
		 * Replaces the item at given \a position with given \a value
		 */
		iterator replace(int position, value_type const &value)
		{
			iterator ret = find(position);
			if(ret != end())
				return (*ret = value), ret;
			Block_ block;
			block.push_back(value);
			return iterator(resetImpl(position, 1, &block), 0, *this);
		}
		/*!
		 * Replaces block starting from the given \a position with block from the given \a replace_block
		 */
		iterator replace(int position, Block_ const &replace_block)
		{
			return iterator(resetImpl(position, replace_block.size(), &replace_block), 0, *this);
		}
		/*!
		 * Replaces block starting from the given \a position with block from the given \a replace_vector
		 */
		iterator replace(int position, const_iterator const &replace_begin, const_iterator const &replace_end)
		{
			int cur_loc = position;
			typename BlockNodes::const_iterator tbi = replace_begin.block_node_i_, tbiend = replace_end.block_node_i_;
			if(tbi == tbiend)
			{
				if(int replaced_count = replace_end.index_ - replace_begin.index_)
				{
					Block_ seq = replace_begin.block_node_i_->block;
					if(seq.size())
					{
						seq.resize(replace_end.index_);
						seq.remove(0, replace_begin.index_);
						resetImpl(cur_loc, replaced_count, &seq);
					} else
						resetImpl(cur_loc, replaced_count, 0);
				}
				return find(cur_loc);
			}
			if(int head_skip = replace_begin.index_)
			{
				int head_count = replace_begin.block_node_i_->range - head_skip;
				Block_ seq = replace_begin.block_node_i_->block;
				if(seq.size())
				{
					seq.remove(0, head_skip);
					resetImpl(cur_loc, head_count, &seq);
				} else
					resetImpl(cur_loc, head_count, 0);
				cur_loc += head_count;
				++tbi;
			}

			for(; tbi != tbiend; cur_loc += tbi->range, ++tbi)
				resetImpl(cur_loc, tbi->range, tbi->block.size() ? &tbi->block : 0);

			if(int tail_count = replace_end.index_)
			{
				Block_ seq = replace_end.block_node_i_->block;
				if(seq.size())
				{
					seq.resize(tail_count);
					resetImpl(cur_loc, tail_count, &seq);
				} else
					resetImpl(cur_loc, tail_count, 0);
			}

			return find(position);
		}

		/*!
		 * Inserts a gap of given \a size into the given \a position
		 */
		iterator insertGap(int position, int gap_range)
		{
			typename BlockNodes::iterator rend = block_nodes.find(position);
			if(rend == block_nodes.end())
				rend = resetImpl(position, 0);
			for(typename BlockNodes::iterator ri = --block_nodes.end(); ri != rend; block_nodes.erase(ri--))
				block_nodes.insert(ri.key() + gap_range, ri.value());
			return iterator(block_nodes.insert(position + gap_range, BlockNode(gap_range)), 0, *this);
		}

		/*!
		 * Inserts the given \a value to the given \a position
		 */
		iterator insert(int position, value_type const &value)
		{
			Block_ seq;
			seq.push_back(value);
			return insert(position, seq);
		}
		/*!
		 * Inserts the given \a insert_block to the given \a position
		 */
		iterator insert(int position, Block_ const &insert_block)
		{
			int range = insert_block.size();
			typename BlockNodes::iterator rend = block_nodes.find(position);
			if(rend == block_nodes.end())
				rend = resetImpl(position, 0);
			for(typename BlockNodes::iterator ri = --block_nodes.end(); ri != rend; block_nodes.erase(ri--))
				block_nodes.insert(ri.key() + range, ri.value());
			return iterator(block_nodes.insert(position + range, BlockNode(range, insert_block)), 0, *this);
		}
		/*!
		 * Inserts the given \a insert_vector to the given \a position
		 */
		iterator insert(int position, const_iterator const &insert_begin, const_iterator const &insert_end)
		{

			insertGap(position, insert_end - insert_begin);
			return replace(position, insert_begin, insert_end);
		}

		/*!
		 * Append the given \a value to the sparse vector
		 */
		iterator append(value_type const &value)
		{
			Block_ seq;
			seq.push_back(value);
			return append(seq);
		}
		/*!
		 * Append the given \a append_block to the sparse vector
		 */
		iterator append(Block_ const &append_block)
		{
			int range = append_block.size();
			return iterator(block_nodes.insert(size() + range, BlockNode(range, append_block)), 0, *this);
		}
		/*!
		 * Append the given \a append_vector to the sparse vector
		 */
		iterator append(const_iterator const &append_begin, const_iterator const &append_end)
		{
			// TODO: take advantage of knowing this is append, not insert
			return insert(size(), append_begin, append_end);
		}

		/*!
		 * Removes a given \a range of block from the given \a position
		 */
		void remove(int position, int range)
		{
			typename BlockNodes::iterator ri = resetImpl(position, range);
			while((ri = block_nodes.erase(ri)) != block_nodes.end())
				block_nodes.insert(ri.key() - range, ri.value());
		}

		/*!
		 * Erases the item at given position, and returns an iterator to the next item
		 */
		iterator erase(iterator const &iter)
		{
			// TODO: dummy solution, improve
			int loc = iter.key();
			remove(loc, 1);
			return find(loc);
		}

		/*!
		 * Erases the range between given \a begin and given \a end
		 * \return iterator to the item after the erased range.
		 */
		iterator erase(iterator const &begin, const_iterator const &end)
		{
			// TODO: dummy solution, improve
			int loc = begin.key();
			remove(loc, end - begin);
			return find(loc);
		}

		void clear()
		{
			block_nodes.clear();
			block_nodes[0];
		}

		size_type size() const
		{
			return (--block_nodes.end()).key();
		}

		void resize(size_type new_size)
		{
			int old_size = size();
			int diff = new_size - old_size;
			if(diff > 0)
				insertGap(size(), diff);
			else if(diff < 0)
				remove(new_size, -diff);
		}

		friend QDebug operator<<(QDebug debug, QSparseVector const &sparse_vector)
		{
			debug.nospace() << "QSparseVector(";
			for(typename BlockNodes::const_iterator bi = sparse_vector.block_nodes.begin(), biend = sparse_vector.block_nodes.end(); bi != biend; ++bi)
			{
				debug << "\n\tBlockNode(" << &bi.value() << ") at " << (bi.key() - bi->range) << ": " << (bi->range) << " items with " << (bi->offset) << " offset:";
				if(bi->range && bi->block.size())
					debug << "\n\t\t" << bi->block.mid(bi->offset, bi->range);
				else
					debug << "\n\t\t" << "Gap region";
			}
			return debug << ")";
		}
	};

		template<typename Vector_>
	struct Range : QPair<typename Vector_::iterator, typename Vector_::iterator>
	{
		typedef typename Vector_::iterator iterator;
		typedef typename Vector_::const_iterator const_iterator;
		typedef QPair<iterator, iterator> PairBase;
		using PairBase::first;
		using PairBase::second;
		Range() {}
		Range(iterator const &t1, iterator const &t2)
			: PairBase(t1, t2) {}
			template<typename OtherVector_>
		Range(OtherVector_ &range)
			: PairBase(range.begin(), range.end()) {}

			template<typename OtherVector_>
		Range(OtherVector_ &range, int first_row, unsigned count)
			: PairBase(range.begin() + first_row, range.begin() + (first_row + count)) {}
		unsigned size() const { return unsigned(second - first); }
		iterator const &begin() const { return first; }
		iterator const &end() const { return second; }
		const_iterator const &constBegin() const { return first; }
		const_iterator const &constEnd() const { return second; }
		Vector_ &container() { return first.container(); }
		Vector_ const &container() const { return first.container(); }

		friend QDebug operator<<(QDebug debug, Range const &range)
		{
			for(iterator ci = range.first; ci != range.second; ++ci)
				debug << *ci;
			return debug;
		}
	};

		template<typename Vector_>
	struct ConstRange : QPair<typename Vector_::const_iterator, typename Vector_::const_iterator>
	{
		typedef typename Vector_::const_iterator iterator;
		typedef typename Vector_::const_iterator const_iterator;
		typedef QPair<const_iterator, const_iterator> PairBase;
		using PairBase::first;
		using PairBase::second;

		ConstRange() {}

		ConstRange(const_iterator const &t1, const_iterator const &t2)
			: PairBase(t1, t2) {}

			template<typename OtherVector_>
		ConstRange(OtherVector_ const &range)
			: PairBase(range.begin(), range.end())
		{}

			template<typename OtherVector_>
		ConstRange(OtherVector_ const &range, int first_row, unsigned count)
			: PairBase(range.begin() + first_row, range.begin() + (first_row + count)) {}

		unsigned size() const { return unsigned(second - first); }
		iterator const &begin() const { return first; }
		iterator const &end() const { return second; }
		const_iterator const &constBegin() const { return first; }
		const_iterator const &constEnd() const { return second; }
		Vector_ const &container() const { return first.container(); }

		friend QDebug operator<<(QDebug debug, ConstRange const &range)
		{
			for(const_iterator ci = range.first; ci != range.second; ++ci)
				debug << *ci;
			return debug;
		}
	};
}
#endif /* SOPRANOLIVE_SPARSEVECTOR_P_H_ */
