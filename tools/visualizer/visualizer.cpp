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
 * visualizer.cpp
 *
 *  Created on: Oct 18, 2010
 *      Author: iridian
 */

#include <stdio.h>
#include <QHeaderView>
#include "visualizer.h"

using namespace Qt;

namespace SopranoLive
{

	QString categoryName[] = { "Bases", "Members", "Referers" };

	bool LazyStandardItemModel::hasChildren(const QModelIndex &parent) const
	{
		QStandardItem *item = visualizer_->model_.itemFromIndex(parent);
		return (item && item->type() == S_Leaf)
				? true
				: QStandardItemModel::hasChildren(parent);
	}


	bool LazyStandardItemModel::canFetchMore(QModelIndex const &parent) const
	{
		QStandardItem *item = visualizer_->model_.itemFromIndex(parent);

		/*qDebug() << __func__ << parent
				<< item
				<< (item ? item->type() : 0)
				<< (item && item->type() == S_Leaf ? static_cast<LeafTreeItem *>(item)->is_expanded_ : false);
		 */

		return item
				&& item->type() == S_Leaf
				&& !static_cast<LeafTreeItem *>(item)->is_expanded_;
	}
	void LazyStandardItemModel::fetchMore(QModelIndex const &parent)
	{
		QStandardItemModel *m = &visualizer_->model_;
		QStandardItem *item = m->itemFromIndex(parent);
		if(item && item->type() == S_Leaf)
		{
			LeafTreeItem *tnd = static_cast<LeafTreeItem *>(item);
			if(tnd->object_)
				tnd->cloneChildrenFrom(tnd->object_->node_list_.front());
			tnd->setExpanded(true);
		}
	}

	Rule::Rule(QString key, int condition, QVariant value)
		: key_(key), condition_(condition), value_(value)
	{

	}

	int Rule::check(Qualities const &qualities) const
	{
		Qualities::const_iterator cq = qualities.find(key_);
		bool satisfies_operation = false;
		if(cq != qualities.end())
		{
			bool numbers = (cq->type() == QVariant::Double && value_.type() == QVariant::Double);

			if(condition_ & Exists)
				satisfies_operation = true;
			else if(condition_ & Equal)
				satisfies_operation = (cq.value() == value_);
			else if(condition_ & Less)
				satisfies_operation = (numbers
						? cq->toDouble() < value_.toDouble()
						: cq->toString() < value_.toString());
			else if(condition_ & Greater)
				satisfies_operation = (numbers
						? cq->toDouble() > value_.toDouble()
						: cq->toString() > value_.toString());
		}
		if(condition_ & Not)
			satisfies_operation = !satisfies_operation;
		if(condition_ & Accept)
			return satisfies_operation ? Accept : Next;
		// else condition_ & Require
		return satisfies_operation ? Next : Reject;
	}
	QDebug operator<<(QDebug d, Rule const &r)
	{
		d.nospace() << "Rule(" << r.key_;
		if(r.condition_ & Rule::Require)
			d.nospace() << "Require ";
		if(r.condition_ & Rule::Accept)
			d.nospace() << "Accept ";
		if(r.condition_ & Rule::Not)
			d.nospace() << "Not ";
		if(r.condition_ & Rule::Exists)
			d.nospace() << "Exists ";
		if(r.condition_ & Rule::Equal)
			d.nospace() << "Equal ";
		if(r.condition_ & Rule::Less)
			d.nospace() << "Less ";
		if(r.condition_ & Rule::Greater)
			d.nospace() << "Greater ";
		d.nospace() << r.value_ << ")";
		return d;
	}


	FilterRulesModel::FilterRulesModel()
	{
		QString headers[] = { "Key", "Condition", "Value" };

		for(unsigned i = 0; i < sizeof(headers) / sizeof(QString); ++i)
			setHorizontalHeaderItem(i, new QStandardItem(headers[i]));

		connect(this, SIGNAL(rowsInserted(const QModelIndex &, int, int))
				, SLOT(filterRulesRowsAffected(const QModelIndex &, int, int)));
		connect(this, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int))
				, SLOT(filterRulesRowsAffected(const QModelIndex &, int, int)));
		connect(this, SIGNAL(rowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int))
				, SLOT(filterRulesRowsAffected(const QModelIndex &, int, int)));
		connect(this, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &))
				, SLOT(filterRulesDataChanged(const QModelIndex &, const QModelIndex &)));

		connect(this, SIGNAL(modelReset()), SIGNAL(filterRulesInvalidated()));
	}

	QMap<QString, Rule> primeRulePrototypes()
	{
		QMap<QString, Rule> ret;
		ret["exists"] = Rule("", Rule::Exists);
		ret["!exists"] = Rule("", Rule::Not | Rule::Exists);
		ret["=="] = Rule("", Rule::Equal);
		ret["!="] = Rule("", Rule::Not | Rule::Equal);
		ret["<"] = Rule("", Rule::Less);
		ret[">="] = Rule("", Rule::Not | Rule::Less);
		ret[">"] = Rule("", Rule::Greater);
		ret["<="] = Rule("", Rule::Not | Rule::Greater);
		return ret;
	}

	QMap<QString, Rule> rule_prototypes = primeRulePrototypes();

	QVector<Rule> FilterRulesModel::rules() const
	{
		if(rule_cache_.size())
			return rule_cache_;
		for(int i = 0, iend = rowCount(); i < iend; ++i)
		{
			QStandardItem *rule_name = item(i, 0);
			if(Qt::CheckState st = rule_name->checkState())
			{
				Rule proto = rule_prototypes[item(i, 1)->text()];
				proto.key_ = rule_name->text();
				proto.condition_ |= (st == Qt::Checked ? Rule::Require : Rule::Accept);
				QString v = item(i, 2)->text();
				bool is_double;
				double d = v.toDouble(&is_double);
				proto.value_ = is_double ? QVariant(d) : QVariant(v);
				rule_cache_.push_back(proto);
			}
		}
		return rule_cache_;
	}

	void FilterRulesModel::addFilterRow(QString const &key)
	{
		QList<QStandardItem *> row_items;
		row_items << new QStandardItem(key) << new QStandardItem("exists") << new QStandardItem();
		row_items[0]->setCheckable(true);
		row_items[0]->setTristate(true);
		row_items[0]->setCheckState(Qt::Unchecked);
		appendRow(row_items);
	}

	void FilterRulesModel::checkKey(QString const &key)
	{
		bool &initialized = existing_keys_[key];
		if(!initialized)
		{
			initialized = true;
			addFilterRow(key);
		}
	}

	void FilterRulesModel::filterRulesRowsAffected(const QModelIndex &, int top, int bottom)
	{
		while(top <= bottom)
			if(item(top++, 0)->checkState() != Qt::Unchecked)
			{
				QMetaObject::invokeMethod(this, "filterRulesInvalidated", Qt::QueuedConnection);
				return;
			}
	}
	void FilterRulesModel::filterRulesDataChanged(const QModelIndex &tl, const QModelIndex &br)
	{
		if(!tl.column()) // the checked state might have changed
			QMetaObject::invokeMethod(this, "filterRulesInvalidated", Qt::QueuedConnection);
		else
			filterRulesRowsAffected(QModelIndex(), tl.row(), br.row());
	}

	SortFilterProxyModel::SortFilterProxyModel(Visualizer *visualizer, FilterRulesModel *filter_model)
		: visualizer_(visualizer), filter_model_(filter_model)
	{
		connect(filter_model_, SIGNAL(filterRulesInvalidated()), SLOT(filterInvalidated()));
	}

	bool SortFilterProxyModel::filterAcceptsRow
			(int source_row, const QModelIndex &source_parent) const
	{
		// always accept non-toplevel items
		if(source_parent.isValid())
			return true;
		Element *object = static_cast<TreeItem *>
				(visualizer_->model_.item(source_row, 0))->object_;

		//qDebug() << __func__ << source_row << source_parent;
		QVector<Rule> const rules = filter_model_->rules();
		//qDebug() << "Checking" << object->qualities_ << "against" << rules;
		for(int i = 0, iend = rules.size(); i != iend; ++i)
			if(int ret = rules[i].check(object->qualities_))
			{
				//qDebug() << (ret == Rule::Accept ? "\tAccepted" : "\tRejected") << "at rule" << i;
				return (ret == Rule::Accept);
			}
		//qDebug() << "Accepted by default";
		return true;
	}

	void SortFilterProxyModel::filterInvalidated()
	{
		filter_model_->rule_cache_.clear();
		invalidateFilter();
	}

	Visualizer::Visualizer()
		: model_(this)
		, filter_model_(), sort_proxy_model_(this, &filter_model_)
		, stdin_(), stdin_read_()
		, event_id_(0)
		, root_element_(this)
	{
		stdin_= new QFile();
		stdin_->open(0,QIODevice::ReadOnly); // 0 = stdin

		stdin_read_ = new QSocketNotifier(0,QSocketNotifier::Read);

		connect(stdin_read_, SIGNAL(activated(int)), SLOT(processInput(int)));

		QString headers[] =
		{ "Name", "Value", "Last Op", "Modified", "Type", "Qualities", "ID", "Created" };

		for(unsigned i = 0; i < sizeof(headers) / sizeof(QString); ++i)
			model_.setHorizontalHeaderItem(i, new QStandardItem(headers[i]));

		setLayout(root_vertical_ = new QVBoxLayout());

		root_vertical_->addWidget(splitter_ = new QSplitter(this));

		tree_ = new QTreeView();
		tree_->setSortingEnabled(true);
		sort_proxy_model_.setSourceModel(&model_);
		sort_proxy_model_.setDynamicSortFilter(true);
		tree_->setModel(&sort_proxy_model_);
		splitter_->addWidget(tree_);

		filters_ = new QTreeView();
		filters_->setModel(&filter_model_);
		splitter_->addWidget(filters_);

		setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	}

	Visualizer::~Visualizer()
	{

	}

	TreeItem::TreeItem(QString const &item_text, Element *object)
		: QStandardItem(item_text), object_(object)
		, self_ref_(object_
				? object_->node_list_.insert(object_->node_list_.end(), this)
				: TreeNodeList::iterator())
	{
		// qDebug() << "Created TreeItem(" << this << "):" << item_text;
	}
	TreeItem::~TreeItem()
	{
		// qDebug() << "Destroyed TreeItem(" << this << "):" << text();
		if(object_)
			object_->node_list_.erase(self_ref_);
	}
	QStandardItem *TreeItem::clone() const
	{
		return new TreeItem(text(), object_);
	}
	void TreeItem::destroy()
	{
		tndParent()->removeRow(row());
	}
	void TreeItem::updateDatas(ItemDataLine const &values)
	{
		QStandardItem *p = tndParent();
		for(int i = 0, iend = values.size(); i < iend; ++i)
			if(values[i].size())
			{
				if(QStandardItem *child = p->child(row(), i))
					for(QMap<int, QVariant>::const_iterator cvi = values[i].constBegin()
							, cviend = values[i].constEnd(); cvi != cviend; ++cvi)
						child->setData(cvi.value(), cvi.key());
				else
					qWarning() << __func__ << ": invalid child for" << p->index() << "at (" << row() << ", " << i << ")";
			}
	}
	void TreeItem::cloneChildrenFrom(QStandardItem *source)
	{
		// qDebug() << __func__ << source->index() << "to" << index();
		for(int row = 0, rowend = source->rowCount(); row < rowend; ++row)
		{
			QList<QStandardItem *> row_items;
			for(int col = 0, colend = source->columnCount(); col < colend; ++col)
				row_items << source->child(row, col)->clone();
			appendRow(row_items);
			object_->visualizer_->model_.setItemData
					( child(row, 0)->index()
					, object_->visualizer_->model_.itemData(source->child(row, 0)->index()));
			if(row_items[0]->type() > S_Leaf) // do not clone the tree past leaf nodes
				static_cast<TreeItem *>(row_items[0])->cloneChildrenFrom(source->child(row, 0));
		}
	}
	QStandardItem *TreeItem::tndParent() const
	{
		QStandardItem *ret = parent();
		if(!ret)
			ret = object_->visualizer_->model_.invisibleRootItem();
		return ret;
	}

	int TreeItem::type() const { return object_->state_ & ObjectTypeMask; }


	LeafTreeItem::LeafTreeItem(QString const &item_text, Element *object)
		: TreeItem(item_text, object), is_expanded_(false)
	{}

	QStandardItem *LeafTreeItem::clone() const
	{
		return new LeafTreeItem(text(), object_);
	}

	void LeafTreeItem::retarget(Element *object)
	{
		if(object_ == object)
			return;
		bool was_expanded = is_expanded_;
		//qDebug() << __func__ << tndParent()->text() << (object ? object->name_ : "0x0");
		QStandardItemModel *m = 0;
		if(object_)
		{
			m = &object_->visualizer_->model_;
			removeRows(0, rowCount());
			setExpanded(false);

			object_->node_list_.erase(self_ref_);
		}
		object_ = object;
		if(object_)
		{
			m = &object_->visualizer_->model_;
			self_ref_ = object_->node_list_.insert(object_->node_list_.end(), this);

			QModelIndex clone_index = object_->node_list_.first()->index();

			for(int i = 0; i < HeaderCount; ++i)
				m->setItemData
						( index().sibling(row(), i)
						, m->itemData(clone_index.sibling(clone_index.row(), i)));

			if(was_expanded)
			{
				m->fetchMore(index());
				setExpanded(true);
			}
		} else
		{
			QMap<int, QVariant> red_zero;
			red_zero[Qt::DisplayRole] = QVariant("invalid");
			red_zero[Qt::ForegroundRole] = Qt::red;

			m->setItemData(index().sibling(row(), 0), red_zero);
			for(int i = 1; i < HeaderCount; ++i)
				m->setItemData(index().sibling(row(), i), QMap<int, QVariant>());

			setExpanded(false);
		}
	}

	void LeafTreeItem::setExpanded(bool expanded)
	{
		if(is_expanded_ && !expanded)
			is_expanded_ = false;
		else if(!is_expanded_ && expanded)
			is_expanded_ = true;
	}

	Element::Element(Visualizer *visualizer, int state)
		: parent_(0), visualizer_(visualizer), state_(state)
		, children_()
		, is_reference_(false)
	{
		node_list_.push_back(static_cast<TreeItem *>
				(visualizer_->model_.invisibleRootItem()));
	}

	Element::Element(Element *parent, int state)
		: parent_(parent), visualizer_(parent->visualizer_), state_(state)
		, children_()
		, is_reference_(false)
	{}

	Element::~Element() { while(node_list_.size()) node_list_.back()->destroy(); }

	void Element::addNodeDatas(ItemDataLine const &values)
	{
		//qDebug() << __func__ << values;
		for(TreeNodeList::iterator tni = parent_->node_list_.begin()
				, tniend = parent_->node_list_.end(); tni != tniend; ++tni)
			if((*tni)->type() != S_Leaf || static_cast<LeafTreeItem *>(*tni)->is_expanded_)
		{
			//qDebug() << __func__ << *tni;
			QList<QStandardItem *> row_items;
			// TreeItem constructor adds itself to this->node_list_ and
			// stores a backreference iterator to it as well

			for(int i = 0; i < HeaderCount; ++i) // skips H_Name
			{
				row_items << (!i
						? new TreeItem(values[H_Name][Qt::DisplayRole].toString(), this)
						: new QStandardItem());

				for(QMap<int, QVariant>::const_iterator cvi = values[i].constBegin()
						, cviend = values[i].constEnd(); cvi != cviend; ++cvi)
					row_items.back()->setData(cvi.value(), cvi.key());
			}
			(*tni)->appendRow(row_items);
		}
	}

	void Element::updateNodeDatas(ItemDataLine const &values)
	{
		for(TreeNodeList::iterator tni = node_list_.begin()
				, tniend = node_list_.end(); tni != tniend; ++tni)
			(*tni)->updateDatas(values);
	}
	void Element::removeNodeDatas()
	{
		for(TreeNodeList::iterator tni = node_list_.begin()
				, tniend = node_list_.end(); tni != tniend; )
			(*tni++)->destroy(); // the iterator gets invalidated here
	}

	void Element::createLeafChild(int child, Element *new_element)
	{
		//qDebug() << __func__ << child << (new_element ? new_element->value_ : "") << (new_element ? new_element->name_ : "");
		for(TreeNodeList::iterator tni = node_list_.begin()
				, tniend = node_list_.end(); tni != tniend; ++tni)
		{
			QList<QStandardItem *> row_items;
			LeafTreeItem *lti = new LeafTreeItem("", 0);
			row_items << lti;
			for(int i = HeaderCount; --i; ) // skips H_Name
				row_items << new QStandardItem();
			//qDebug() << "Inserting row" << child << "under" << (*tni)->index() ;
			(*tni)->insertRow(child, row_items);
			lti->retarget(new_element);
		}
	}
	void Element::retargetLeafChild(int child, Element *new_element)
	{
		for(TreeNodeList::iterator tni = node_list_.begin()
				, tniend = node_list_.end(); tni != tniend; ++tni)
			if(QStandardItem *item = (*tni)->child(child, 0))
				if(item->type() == S_Leaf)
					static_cast<LeafTreeItem *>(item)->retarget(new_element);
	}
	void Element::removeLeafChild(int child)
	{
		for(TreeNodeList::iterator tni = node_list_.begin()
				, tniend = node_list_.end(); tni != tniend; ++tni)
			(*tni)->removeRow(child);
	}

	Qualities Element::updateQualities
			(Qualities const &q, ItemDataLine *update_data_line)
	{
		Qualities changed;
		for(Qualities::const_iterator cqi = q.begin(), cqiend = q.end()
				; cqi != cqiend; ++cqi)
		{
			Qualities::iterator tqi = qualities_.find(cqi.key());
			if(tqi == qualities_.end())
				qualities_.insert(cqi.key(), cqi.value());
			else if(*tqi != *cqi)
				*tqi = *cqi;
			else
				continue;
			changed.insert(cqi.key(), cqi.value());
		}
		if(changed.size())
		{
			qualities_text_.clear();
			for(Qualities::const_iterator cqi = qualities_.begin()
					, cqiend = qualities_.end(); cqi != cqiend; ++cqi)
				((((qualities_text_ += '&') += cqi.key()) += '=') += cqi->toString());
			(*update_data_line)[H_Qualities][Qt::DisplayRole] = QVariant(qualities_text_);
		}
		return changed;
	}

	bool Element::updateName(QString raw_name, ItemDataLine *update_data_line)
	{
		name_ = raw_name;
		(*update_data_line)[H_Name][Qt::DisplayRole] = name_;
		return true;
	}

	bool Element::updateValue(QString raw_value, ItemDataLine *update_data_line)
	{
		bool changed = false;

		QString new_value;
		if(raw_value.mid(0, 2) == "* ")
		{
			if(!is_reference_)
				changed = true;
			is_reference_ = true;
			new_value = raw_value.mid(2);
		} else
		{
			if(is_reference_)
				changed = true;
			is_reference_ = false;
			new_value = raw_value;
		}
		if(value_ != new_value)
		{
			changed = true;
			value_ = new_value;

			(*update_data_line)[H_Value][Qt::DisplayRole] = QVariant(value_);
		}
		return changed;
	}

	void Element::updateModified(int id, ItemDataLine *update_data_line)
	{
		QVariant modified_id(id);
		for(TreeNodeList::iterator tni = node_list_.begin()
				, tniend = node_list_.end(); tni != tniend; ++tni)
		{
			for(QStandardItem *p = visualizer_->tndParent(*tni)
					, *pend = visualizer_->model_.invisibleRootItem(); p != pend; )
			{
				int row = p->row();
				p = visualizer_->tndParent(p);
				p->child(row, H_Modified)->setData(modified_id, Qt::DisplayRole);
			}
		}
		(*update_data_line)[H_Modified][Qt::DisplayRole] = modified_id;
	}

	ReverseRefIndices::iterator Element::addReferer(Element *object)
	{
		int referer_location = reverse_refs_.size();
		//qDebug() << __func__ << object->value_ << referer_location;
		createLeafChild(referer_location, object);
		return reverse_refs_.insert(reverse_refs_.end(), referer_location);
	}

	void Element::removeReferer(ReverseRefIndices::iterator ref)
	{
		removeLeafChild(*ref);
		//qDebug() << __func__ << *ref << reverse_refs_.size();
		//for(ReverseRefIndices::iterator i = reverse_refs_.begin(); i != reverse_refs_.end(); ++i)
		//	qDebug() << *i;
		ref = reverse_refs_.erase(ref);
		for( ; ref != reverse_refs_.end(); ++ref)
			--*ref;
	}


	bool Association::updateValue(QString raw_value, ItemDataLine *update_data_line)
	{
		if(!Element::updateValue(raw_value, update_data_line))
			return false;

		ElementChildren::iterator obji = visualizer_->root_element_.children_.find(value_);
		bool referee_exists = (obji != visualizer_->root_element_.children_.end());

		// qDebug() << __func__ << raw_value << referee_exists;

		Qualities::const_iterator relationi = qualities_.find("Relation");
		if(relationi != qualities_.end())
		{
			Qualities carry_over;
			if(relationi.value() == "Owns")
				carry_over["Lifetime"] = "Owned";
			else if(relationi.value() == "Shares")
				carry_over["Lifetime"] = "Shared";
			if(referee_exists)
			{
				ItemDataLine carryover_update_data_line_(HeaderCount);
				(*obji)->updateQualities(carry_over, &carryover_update_data_line_);
				(*obji)->updateNodeDatas(carryover_update_data_line_);
			}
		}

		if(has_reference_leaf_ && !is_reference_)
		{
			removeLeafChild(0);
			has_reference_leaf_ = false;
		}
		else if(has_reference_leaf_ && is_reference_)
			retargetLeafChild(0, referee_exists ? obji->data() : 0);
		else if(!has_reference_leaf_ && is_reference_)
		{
			createLeafChild(0, referee_exists ? obji->data() : 0);
			has_reference_leaf_ = true;
		}

		// remove old referer entry
		if(referee_)
		{
			referee_->removeReferer(referee_self_iter_);
			referee_ = 0;
		}

		if(referee_exists)
		{
			referee_ = (*obji)->child<Referer>("<-" + name_.mid(2, name_.size() - 2))->data();
			referee_self_iter_ = referee_->addReferer(parent_);
		}
		return true;
	}

	Qualities Association::updateQualities
			( Qualities const &q, ItemDataLine *update_data_line)
	{
		Qualities changed = Element::updateQualities(q, update_data_line);
		Qualities carry_over;
		Qualities::const_iterator relationi = changed.find("Relation");
		if(relationi != changed.end())
		{
			if(relationi.value() == "Owns")
				carry_over["Lifetime"] = "Owned";
			else if(relationi.value() == "Shares")
				carry_over["Lifetime"] = "Shared";
		}

		if(carry_over.size())
		{
			// we should diagnose ownership type changes here.
			ElementChildren::iterator obji = visualizer_->root_element_.children_.find(value_);
			if(obji != visualizer_->root_element_.children_.end())
			{
				ItemDataLine carry_over_node_vec(HeaderCount);
				(*obji)->updateQualities(carry_over, &carry_over_node_vec);
				(*obji)->updateNodeDatas(carry_over_node_vec);
			}
		}
		return changed;
	}

	Qualities Visualizer::parseQualities(QString raw_qualities)
	{
		Qualities ret;
		QStringList qualities = raw_qualities.split('&');
		for(QStringList::const_iterator cqi = qualities.constBegin()
				, cqiend = qualities.constEnd(); cqi != cqiend; ++cqi)
			if(cqi->size())
			{
				QStringList key_and_value = cqi->split("=");
				QString key = key_and_value.takeFirst();
				filter_model_.checkKey(key);
				QVariant &value = ret[key];
				QString str = key_and_value.join("=");
				bool is_double = false;
				double d = str.toDouble(&is_double);
				value = is_double ? QVariant(d) : QVariant(str);
			}
		return ret;
	}


	void Visualizer::processLifetimeLine(QStringList const &array)
	{
		ElementChildren::iterator obji = root_element_.children_.find(array[Raw_ID]);
		if(array[Raw_Last_Op][0] == 'C')
		{
			ItemDataLine node_datas(HeaderCount);

			if(obji == root_element_.children_.end())
				obji = root_element_.child<Object>(array[Raw_ID]);
			else
			{
				if(!((*obji)->state_ & S_Deleted))
					qWarning() << "Visualizer: Object ID conflict, reconstructing" << array[Raw_ID];
				(*obji)->children_.clear();
				obji = root_element_.child<Object>(array[Raw_ID]);
				for(int i = HeaderCount; i--; )
					node_datas[i][Qt::ForegroundRole] = Qt::black;
			}

			node_datas[H_Last_Op][Qt::DisplayRole] = QVariant(array[Raw_Last_Op]);
			node_datas[H_ID][Qt::DisplayRole] = QVariant(array[Raw_ID]);
			node_datas[H_Type][Qt::DisplayRole] = QVariant(array[Raw_Type]);
			node_datas[H_Created][Qt::DisplayRole] = QVariant(event_id_);
			node_datas[H_Modified][Qt::DisplayRole] = QVariant(event_id_);

			(*obji)->updateName(array[Raw_Name].size() ? array[Raw_Name] : array[Raw_ID], &node_datas);
			(*obji)->updateValue(array[Raw_Value], &node_datas);
			(*obji)->updateQualities(parseQualities(array[Raw_Qualities]), &node_datas);
			(*obji)->updateNodeDatas(node_datas);

		} else if(array[Raw_Last_Op] == "D")
		{
			if(obji == root_element_.children_.end())
				qWarning() << "Visualizer: double deletion of" << array[Raw_ID];
			else
			{
				(*obji)->state_ |= S_Deleted;
				for(TreeNodeList::iterator tni = (*obji)->node_list_.begin()
						, tniend = (*obji)->node_list_.end(); tni != tniend; ++tni)
				{
					QStandardItem *p = (*tni)->tndParent();
					p->child((*tni)->row(), H_Last_Op)->setText(array[Raw_Last_Op]);
					for(int i = HeaderCount; i--; )
						p->child((*tni)->row(), i)
								->setData(Qt::red, Qt::ForegroundRole);
				}
			}
		}
	}

	void Visualizer::processAssociationLine(QStringList const &array)
	{
		ElementChildren::iterator obji = root_element_.children_.find(array[Raw_ID]);

		if(obji == root_element_.children_.end())
		{
			qWarning() << "Visualizer: While accessing member" << array[Raw_Name] << " "
					"undefined object" << array[Raw_ID];
			return;
		}

		QString association_id = "->" + array[Raw_Name];

		ItemDataLine node_datas(HeaderCount);

		ElementChildren::iterator assi = (*obji)->children_.find(association_id);
		if(array[Raw_Last_Op][1] == 'C')
		{
			if(assi == (*obji)->children_.end())
				assi = (*obji)->child<Association>(association_id);
			else qWarning() << "Visualizer: While creating association" << association_id << " "
						"on " << array[Raw_ID] << ": association already exists, duplicating";

			node_datas[H_Last_Op][Qt::DisplayRole] = QVariant(array[Raw_Last_Op]);
			node_datas[H_ID][Qt::DisplayRole] = QVariant(array[Raw_ID]);
			node_datas[H_Type][Qt::DisplayRole] = QVariant(array[Raw_Type]);
			node_datas[H_Created][Qt::DisplayRole] = QVariant(event_id_);

			(*assi)->updateName(association_id, &node_datas);
			(*assi)->updateValue(array[Raw_Value], &node_datas);
			(*assi)->updateQualities(parseQualities(array[Raw_Qualities]), &node_datas);
		} else
		{
			if(assi == (*obji)->children_.end())
			{
				qWarning() << "Visualizer: While deleting or updating association"
						<< association_id << " on " << array[Raw_ID] << ": association doesn't exist";
			}
			else if(array[Raw_Last_Op][1] == 'D') // delete member association
			{
				for(TreeNodeList::iterator tni = (*assi)->node_list_.begin()
						, tniend = (*assi)->node_list_.end(); tni != tniend; ++tni)
				{
					QStandardItem *p = (*tni)->parent();
					p->child((*tni)->row(), H_Last_Op)->setText(array[Raw_Last_Op]);
					for(int i = HeaderCount; i--; )
						p->child((*tni)->row(), i)->setData(Qt::red, Qt::ForegroundRole);
				}
			} else // update member association data
			{
				node_datas[H_Last_Op][Qt::DisplayRole] = QVariant(array[Raw_Last_Op]);
				node_datas[H_Type][Qt::DisplayRole] = QVariant(array[Raw_Type]);

				(*assi)->updateValue(array[Raw_Value], &node_datas);
				(*assi)->updateQualities(parseQualities(array[Raw_Qualities]), &node_datas);
			}
		}
		if(assi != (*obji)->children_.end())
		{
			(*assi)->updateModified(event_id_, &node_datas);
			(*assi)->updateNodeDatas(node_datas);
		}
	}

	void Visualizer::processInput(int)
	{
		stdin_read_->setEnabled(false);
		do
		{
			QStringList lines;

			QString line = stdin_->readLine();
			if(line.size() < 3 || line.mid(0,3) != "VZR")
				continue;
			lines << line;
			while((line = stdin_->readLine()) != "VZREND\n")
				lines << line;

			QStringList chunks = lines.takeFirst().split('\n')[0].split('\t');

			int s = chunks.size();

			if(s != 7)
			{
				qWarning() << "Invalid sized(" << s << ") != 7 row: " << chunks;
				while(s++ < 7)
					chunks << QString();
			}

			while(lines.size())
				(chunks[6] += "\n") += lines.takeFirst().split('\n')[0];

			// qDebug() << "Processing input:" << chunks;

			if(chunks[1][0] == 'M' || chunks[1][0] == 'B')
				processAssociationLine(chunks);
			else
				processLifetimeLine(chunks);

			event_id_++;
		} while(stdin_->bytesAvailable());
		stdin_read_->setEnabled(true);
	}
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	return SopranoLive::Visualizer().show()
	     , app.exec();
}
