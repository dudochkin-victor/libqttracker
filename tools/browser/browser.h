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
 * browser.h
 *
 *  Created on: Jan 7, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef TRACKER_BROWSER_H_
#define TRACKER_BROWSER_H_

#include <QtGui>
#include <QtTracker/Tracker>
#include <QtTracker/ontologies/rdf.h>
#include <QtTracker/ontologies/rdfs.h>
#include <functional>

using namespace SopranoLive;
using namespace SopranoLive::BackEnds;
using namespace std;

typedef void(*QueryBuilderPtr)(RDFSelect &);
Q_DECLARE_METATYPE(QueryBuilderPtr)

namespace TrackerBrowser
{
	typedef QTreeView TrackerTable;

	class BrowserTreeModel;

	class Node
		: public QObject
	{
	public:
		Node(QString name, Node *parent, int parent_row)
			: QObject(), name_(name), parent_(parent), parent_row(parent_row), is_prepared(false) {}
		virtual QString name() const { return name_; }
		virtual bool prepare() const { return is_prepared ? true : ((is_prepared = true), false); }
		virtual int childCount() const = 0;
		virtual int columnCount() const = 0;
		virtual QVariant data(int row, int column, int role) const = 0;
		virtual QModelIndex child(BrowserTreeModel const &tree, int row, int column) const = 0;

		QString name_;
		Node *parent_;
		int parent_row;
		mutable bool is_prepared;
	};

	class PropertyNode
		: public Node
	{
	public:
		LiveNode resource;
		PropertyNode(QString name, LiveNode resource, LiveNodes propertymodel, Node *parent, int parent_row, bool resource_is_subject = true);
		bool prepare() const;
		int childCount() const;
		int columnCount() const;
		QVariant data(int row, int column, int role) const;
		QModelIndex child(BrowserTreeModel const &tree, int row, int column) const;
	protected:
		mutable QList<Node *> properties;
		LiveNodes propertymodel;
		bool resource_is_subject;
	};

	class InterfaceNode
		: public PropertyNode
	{
		static LiveNodes createModel(QUrl interface);
		QUrl interface;
		Node *interface_as_value;
	public:
		InterfaceNode(LiveNode resource, QUrl interface, Node *parent, int parent_row);
		bool prepare() const;
		QVariant data(int row, int column, int role) const;
	};

	class ResourceNode
		: public Node
	{
	public:
		LiveNode resource;
		ResourceNode(LiveNode resource, Node *parent, int parent_row, QString header = QString());
		bool prepare() const;
		int childCount() const;
		int columnCount() const;
		QVariant data(int row, int column, int role) const;
		QModelIndex child(BrowserTreeModel const &tree, int row, int column) const;
	private:
		mutable QList<Node *> categories;
		LiveNodes interfaces;
	};


	class LiveNodeModel_Node
		: public Node
	{
	public:

		typedef QPair<LiveNode, int> NodeRow;
		typedef binder1st<mem_fun1_t<ResourceNode *, LiveNodeModel_Node, NodeRow> > Creator;
		typedef NodeCreatorFunctorPolicy<Creator> ChildPolicy;

		ChildPolicy helper(int parent_row);

		LiveNodeModel_Node(QString name, LiveNodes model
				, Node *parent, int parent_row
				, QString header_name = QString(), LiveNode footer = LiveNode());
		bool prepare() const;
		int childCount() const;
		int columnCount() const;
		QVariant data(int row, int column, int role) const;
		QModelIndex child(BrowserTreeModel const &tree, int row, int column) const;
		ResourceNode *createNode(NodeRow node);

		LiveBag<ResourceNode *, ChildPolicy> model;
		mutable QList<ResourceNode *> children;
		QString footer_name;
		LiveNode footer;
		mutable ResourceNode *footer_node;
	};

	class BrowserTreeModel
		: public QSharedFromThis<BrowserTreeModel, QAbstractItemModel>
	{
		Q_OBJECT

	public:
		using QAbstractItemModel::createIndex;


	public:
		BrowserTreeModel(LiveNodes query_results, QObject *parent = 0);
		~BrowserTreeModel();

		QVariant data(const QModelIndex &index, int role) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		QVariant headerData(int section, Qt::Orientation orientation,
							int role = Qt::DisplayRole) const;
		QModelIndex index(int row, int column,
						  const QModelIndex &parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex &index) const;
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount(const QModelIndex &parent = QModelIndex()) const;

	private:
		LiveNodeModel_Node root;
	};


	class Browser
		: public QSplitter
	{
		Q_OBJECT

		QWidget *bottom_area;
		QVBoxLayout *right_vertical;
		QHBoxLayout *text_filter_row;
		QComboBox *column_menu;
		QLineEdit *text_filter_edit;

		QueryBuilderPtr query_builder;
		QSharedPointer<BrowserTreeModel> trackermodel;
		TrackerTable *trackertable;
		QSplitter *top_split;
		QWidget *query_select_widget, *query_view_widget;
		QVBoxLayout *query_select_box, *query_view_box;

		QListView *query_select;
		RDFSelect *select;
		QStandardItemModel *queries;
		QHBoxLayout *select_viewer_buttons;
		QPushButton *load_button, *save_button;
		QPlainTextEdit *query_viewer;
		QLineEdit *fields_viewer;
		QPushButton *execute_button;

	public:
		Browser();
		~Browser();

		void setSelect(RDFSelect *select);
		void refreshQuery();

	public Q_SLOTS:
		void loadQuery();
		void saveQuery();
		void executeQuery();
		void queryEdited();
		void filterEdited();
	};


}
#endif /* TRACKER_BROWSER_H_ */
