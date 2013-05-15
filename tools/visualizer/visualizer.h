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
 * visualizer.h
 *
 *  Created on: Oct 18, 2010
 *      Author: iridian
 */

#ifndef SOPRANOLIVE_VISUALIZER_H_
#define SOPRANOLIVE_VISUALIZER_H_

#include <QDebug>
#include <QLinkedList>
#include <QApplication>
#include <QSocketNotifier>
#include <QFile>
#include <QTextStream>
#include <QTableView>
#include <QTreeView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QVBoxLayout>
#include <QtGui>

namespace SopranoLive
{
	typedef QMap<QString, QVariant> Qualities;

	enum Categories { Referers, MostDerived, CategoryCount };
	extern QString categoryName[];

	enum { Raw_Prefix, Raw_Last_Op, Raw_ID, Raw_Type, Raw_Name, Raw_Qualities, Raw_Value };
	enum { H_Name, H_Value, H_Last_Op, H_Modified, H_Type, H_Qualities, H_ID, H_Created, HeaderCount };

	enum { TreeItemRole = Qt::UserRole + 2 };

	enum ObjectState
	{	S_Leaf = 1024
	,	S_Root
	,	S_TypedObject
	,	S_UntypedObject
	,	S_Association
	,	S_TopmostOwner, S_Owner, S_Sharer, S_Referer
	,	S_Deleted = 1 << 8
	,	ObjectTypeMask = 1024 | 0xf
	};

	class Visualizer;

	class LazyStandardItemModel
		: public QStandardItemModel
	{
	public:
		Visualizer *visualizer_;
		LazyStandardItemModel(Visualizer *visualizer)
			: visualizer_(visualizer) {}

		//! overridden to implement lazy expansion of cyclic branches
		bool hasChildren(const QModelIndex &parent) const;
		bool canFetchMore(QModelIndex const &parent) const;
		void fetchMore(QModelIndex const &parent);
	};

	struct Rule
	{
		enum
		{ 	Reject = -1, Next = 0, Accept = 1, Require = 2
		,	Not = 4
		,	Exists = 8, Equal = 16, Less = 32, Greater = 64
		};

		QString key_;
		int condition_;
		QVariant value_;
		Rule(QString key = QString(), int condition = Next, QVariant value = QVariant());

		int check(Qualities const &qualities) const;
	};
	QDebug operator<<(QDebug d, Rule const &t);

	class FilterRulesModel
		: public QStandardItemModel
	{
		Q_OBJECT

	public:
		FilterRulesModel();

		mutable QMap<QString, bool> existing_keys_;
		mutable QVector<Rule> rule_cache_;
		void checkKey(QString const &key);
		QVector<Rule> rules() const;

		void addFilterRow(QString const &key);

	public Q_SLOTS:
		void filterRulesRowsAffected(const QModelIndex &, int, int);
		void filterRulesDataChanged(const QModelIndex &, const QModelIndex &);

	Q_SIGNALS:
		void filterRulesInvalidated();
	};

	class SortFilterProxyModel
		: public QSortFilterProxyModel
	{
		Q_OBJECT

	public:
		Visualizer *visualizer_;
		SortFilterProxyModel(Visualizer *visualizer, FilterRulesModel *filter_model);

		FilterRulesModel *filter_model_;

		bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

	public Q_SLOTS:
		void filterInvalidated();
	};

	struct Element;
	struct TreeItem;

	typedef QLinkedList<TreeItem *> TreeNodeList;

	typedef QSharedPointer<Element> ElementPtr;
	typedef QMap<QString, ElementPtr> ElementChildren;
	typedef QLinkedList<int> ReverseRefIndices;

	typedef QVector<QMap<int, QVariant> > ItemDataLine;

	struct TreeItem
		: QStandardItem
	{
		TreeItem(QString const &item_text, Element *object);
		~TreeItem();
		void destroy();

		void updateDatas(ItemDataLine const &values);
		void cloneChildrenFrom(QStandardItem *source);

		QStandardItem *clone() const;

		QStandardItem *tndParent() const;

		int type() const;

		Element *object_;
		TreeNodeList::iterator self_ref_;
	};

	struct LeafTreeItem
		: TreeItem
	{
		LeafTreeItem(QString const &item_text, Element *object);

		QStandardItem *clone() const;

		int type() const { return S_Leaf; }

		void retarget(Element *object);
		void setExpanded(bool expanded);

		bool is_expanded_;
	};

	struct Element
	{
	protected:
		// root constructor
		Element(Visualizer *visualizer, int state);
		Element(Element *parent, int state);
	public:
		~Element();

		Element *parent_;
		Visualizer *visualizer_;
		int state_;
		TreeNodeList node_list_;
		ElementChildren children_;

		void addNodeDatas(ItemDataLine const &values);
		void updateNodeDatas(ItemDataLine const &values);
		void removeNodeDatas();

		void createLeafChild(int child, Element *new_element);
		void retargetLeafChild(int child, Element *new_element);
		void removeLeafChild(int child);

		virtual bool updateName(QString raw_name, ItemDataLine *update_data_line);
		virtual bool updateValue(QString raw_value, ItemDataLine *update_data_line);
		virtual Qualities updateQualities
				(Qualities const &q, ItemDataLine *update_data_line);
		virtual void updateModified(int event, ItemDataLine *update_data_line);

			template<class T>
		ElementChildren::iterator child(QString name = QString())
		{
			if(!name.size())
				name = T::name();
			ElementChildren::iterator childi = children_.find(name);
			if(childi == children_.end())
			{
				childi = children_.insert(name, ElementPtr());

				T::template addAsChild<T>(childi, name, this);
				ItemDataLine item_datas(HeaderCount);
				item_datas[H_Name][Qt::DisplayRole] = name;
				(*childi)->addNodeDatas(item_datas);
			}
			return childi;
		}

			template<class T_>
		static void addAsChild(ElementChildren::iterator childi, QString name, Element *obj)
		{
			Q_UNUSED(name);
			QSharedPointer<T_> t = QSharedPointer<T_>(new T_(obj));
			*childi = t;
		}

		QString name_, value_;
		QString qualities_text_;
		bool is_reference_;
		Qualities qualities_;

		ReverseRefIndices reverse_refs_;

		ReverseRefIndices::iterator addReferer(Element *object);
		void removeReferer(ReverseRefIndices::iterator ref);

	};

	struct RootElement : Element
	{
		RootElement(Visualizer *visualizer)
			: Element(visualizer, S_Root)
		{
			visualizer_ = visualizer;
		}
	};

		template<class MostDerived_>
	struct Child : Element
	{
		Child(Element *parent, int state = S_UntypedObject)
			: Element(parent, state)
		{}
	};

	struct Object
		: Child<Object>
	{
		Object(Element *parent)
			: Child<Object>(parent, S_TypedObject) {}
		static QString name() { return ""; }
	};

	struct Association
		: Child<Association>
	{
		Association(Element *parent)
			: Child<Association>(parent, S_Association)
			, has_reference_leaf_(false), referee_(0), referee_self_iter_() {}
		static QString name() { return ""; }
		bool updateValue(QString raw_value, ItemDataLine *update_data_line);
		Qualities updateQualities(Qualities const &q, ItemDataLine *update_data_line);
		bool has_reference_leaf_;
		Element *referee_;
		ReverseRefIndices::iterator referee_self_iter_;
	};

	struct TopmostOwner
		: Child<TopmostOwner>
	{
		TopmostOwner(Element *parent)
			: Child<TopmostOwner>(parent, S_TopmostOwner) {}
		static QString name() { return "TopmostOwner"; };
	};

	struct Owner
		: Child<Owner>
	{
		Owner(Element *parent)
			: Child<Owner>(parent, S_Owner) {}
		static QString name() { return "Owner"; };
	};

	struct Sharer
		: Child<Sharer>
	{
		Sharer(Element *parent)
			: Child<Sharer>(parent, S_Sharer) {}
		static QString name() { return "Sharers"; };
	};

	struct Referer
		: Child<Referer>
	{
		Referer(Element *parent)
			: Child<Referer>(parent, S_Referer) {}
		static QString name() { return "Referers"; };
	};

	class Visualizer
		: public QWidget
	{
		Q_OBJECT

	public:
		Visualizer();
		~Visualizer();

	public Q_SLOTS:
		void processInput(int socket);

	public:

		QVBoxLayout *root_vertical_;
		QSplitter *splitter_;
		QTreeView *tree_;
		QTreeView *filters_;

		LazyStandardItemModel model_;

		FilterRulesModel filter_model_;
		SortFilterProxyModel sort_proxy_model_;

		QStandardItem *tndParent(QStandardItem *item) const
		{
			QStandardItem *ret = item->parent();
			if(!ret)
				ret = model_.invisibleRootItem();
			return ret;
		}

	public:

		QFile *stdin_;
		QSocketNotifier *stdin_read_;

		int event_id_;

		RootElement root_element_;

		Qualities parseQualities(QString raw_qualities);
		void processLifetimeLine(QStringList const &array);
		void processAssociationLine(QStringList const &array);
	};
}
Q_DECLARE_METATYPE(QSharedPointer<SopranoLive::TreeItem>);

#endif /* SOPRANOLIVE_VISUALIZER_H_ */
