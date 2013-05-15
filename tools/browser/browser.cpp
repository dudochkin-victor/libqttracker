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
 * browser.cpp
 *
 *  Created on: Jan 7, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include <QtGui/QTableView>
#include <QtGui/QApplication>
#include <QtDebug>

#include "browser.h"
#include "getopt.h"

#include <QtTracker/ontologies/nmo.h>

namespace TrackerBrowser
{
	void prepareQuery_Resources(RDFSelect &query)
	{
		query.newColumn("Resource id").isOfType<rdfs::Resource>();
	}

	void prepareQuery_Classes(RDFSelect &query)
	{
		query.newColumn("Class id").isOfType<rdfs::Class>();
	}

//	void prepareQuery_Properties(RDFSelect &query)
//	{
//		RDFVariable property = query.newColumn("Property");
//		property.isOfType<rdfs::Property>();
//		query.addColumn("Domain", property.property<rdfs::domain>());
//		query.addColumn("Range", property.property<rdfs::range>());
//	}


	void prepareQuery_Emails(RDFSelect &query)
	{
		RDFVariable email = RDFVariable::fromType<nmo::Email>();
		query.addColumn("Email id", email);
		query.addColumn("Subject", email.optional().property<nmo::messageSubject>());
		query.addColumn("Sender", email.optional().property<nmo::from>().property<nco::fullname>());
	}

	void prepareQuery_Contacts(RDFSelect &query)
	{
		RDFVariable contact = RDFVariable::fromType<nco::Contact>();
		query.addColumn("Sender id", contact);
		query.addColumn("Name", contact.optional().property<nco::nameGiven>());
		query.addColumn("Phone", contact.optional().property<nco::hasPhoneNumber>().property<nco::phoneNumber>());
		query.addColumn("Email", contact.optional().property<nco::emailAddress>().property<nco::emailAddress>());
	}

	void prepareQuery_Tags(RDFSelect &query)
	{
		RDFVariable element = query.newColumn("Element Id");
		query.addColumn("Tag", element.property<nao::hasTag>().property<nao::prefLabel>());
	}

	void prepareQuery_Documents(RDFSelect &query)
	{
		RDFVariable document = RDFVariable::fromType<nfo::Document>();
		query.addColumn("Document id", document);
		query.addColumn("Subject", document.optional().property<nie::subject>());
		query.addColumn("Mime", document.optional().property<nie::mimeType>());
	}

	void prepareQuery_Messages(RDFSelect &query)
	{
		RDFVariable message = RDFVariable::fromType<nmo::IMMessage>();
		query.addColumn("Message id", message);
		query.addColumn("Sender", message.property<nmo::from>());
	}

	void prepareQuery_Media(RDFSelect &query)
	{
		RDFVariable media = RDFVariable::fromType<nfo::Media>();
		query.addColumn("Media id", media);
	}

	void prepareQuery_BookmarkFolders(RDFSelect &query)
	{
		RDFVariable media = RDFVariable::fromType<nfo::BookmarkFolder>();
		query.addColumn("Bookmark folder id", media);
	}

	/*

	SELECT ?channel ?participant nco:fullname(?participant) ?date ?text ?message_count (?message_count - ?read_message_count)
	WHERE {
		?channel a nmo:CommunicationChannel ;
			nmo:hasParticipant ?participant .
		{
			SELECT ?date ?text
			WHERE {
				?message nmo:conversation [ nmo:communicationChannel ?channel ] ;
					nmo:sentDate ?date ;
					nie:plainTextContent ?text .
			} ORDER BY DESC(?date) LIMIT 1
		}
		{
			SELECT COUNT(?message) AS ?message_count
			WHERE { ?message nmo:conversation [ nmo:communicationChannel ?channel ] }
		}
		{
			SELECT COUNT(?message) AS ?read_message_count
			WHERE { ?message nmo:conversation [ nmo:communicationChannel ?channel ] ; nmo:isRead true }
		}
	}

	*/

	/*
	void prepareQuery_Channels(RDFSelect &query)
	{
		RDFVariable channel = query.newColumn<nmo::CommunicationChannel>("channel");
		RDFVariable participant = channel.property<nmo::hasParticipant>(query.newColumn("participant"));
		participant.optional().property<nco::hasIMAccount>(query.newColumn("IM Account"));

		RDFSelect inner1 = query.subQuery();

		RDFVariable message;
		RDFVariable date = inner1.newColumn("date");
		RDFVariable text = inner1.newColumn("text");
		message.property<nmo::receivedDate>(date);
		message.property<nie::plainTextContent>(text);
		message.property<nmo::conversation>().property<nmo::communicationChannel>(channel);
		inner1.orderBy(date, RDFSelect::Descending);
		inner1.limit(1);
		query.addColumn(date);
		query.addColumn(text);

		RDFVariable message_count = query.subQuery().newCountColumn("message_count");
		message_count.property<nmo::conversation>().property<nmo::communicationChannel>(channel);
		query.addColumn(message_count);

		RDFVariable read_message_count = query.subQuery().newCountColumn("read_message_count");
		read_message_count.property<nmo::conversation>().property<nmo::communicationChannel>(channel);
		read_message_count.property<nmo::isRead>(LiteralValue(true));
		query.addColumn(read_message_count);

		query.groupBy(channel);
		query.orderBy(date);
	}
	*/

	void loadQueries(QStandardItemModel &queries)
	{
		struct QueryBuilder
		{
			QueryBuilderPtr test;
			const char *name;
		};

		QueryBuilder builders[] =
		{
			{ &prepareQuery_Resources, "Resources - querybuilder" },
			{ &prepareQuery_Classes, "Classes - querybuilder" },
//			{ &prepareQuery_Properties, "Properties - querybuilder" },
			{ &prepareQuery_Emails, "Emails - querybuilder" },
			{ &prepareQuery_Contacts, "Contacts - querybuilder" },
			{ &prepareQuery_Tags, "Tags - querybuilder" },
			{ &prepareQuery_Documents, "Documents - querybuilder" },
			{ &prepareQuery_Messages, "Messages - querybuilder" },
			{ &prepareQuery_Media, "Media - querybuilder" },
			{ &prepareQuery_BookmarkFolders, "BookmarkFolders - querybuilder" },
//			{ &prepareQuery_Channels, "Communication channels - querybuilder" },
		};

		for(QueryBuilder *bi = builders; bi != builders + sizeof(builders) / sizeof(QueryBuilder); ++bi)
		{
			QStandardItem &item = *new QStandardItem(bi->name);
			item.setData(QVariant::fromValue(bi->test));
			//qDebug() << "Gained a follower: \"" << bi->name << "\"";
			queries.appendRow(&item);
		}
	}

	PropertyNode::PropertyNode(QString name, LiveNode resource, LiveNodes propertymodel, Node *parent, int parent_row, bool resource_is_subject)
		: Node(name, parent, parent_row)
		, resource(resource)
		, propertymodel(propertymodel)
		, resource_is_subject(resource_is_subject)
	{
		//qDebug() << "PropertyNode" << name << " property count = " << propertymodel->rowCount();
	}

	bool PropertyNode::prepare() const
	{
		if(Node::prepare())
			return true;
		PropertyNode *noncthis = const_cast<PropertyNode *>(this);
		LiveNodeList pl = propertymodel;
		for(LiveNodeList::iterator pi = pl.begin(), piend = pl.end(); pi != piend; ++pi)
		{
			//qDebug() << "Property: " << pi->toN3();
			properties.push_back(new LiveNodeModel_Node( pi->toString()
			                                           , resource_is_subject ? resource->getObjects(*pi) : resource->getSubjects(*pi)
			                                           , noncthis
			                                           , properties.size()
			                                           , "property metadata: "
			                                           , *pi));
		}
		return false;
	}
	int PropertyNode::childCount() const
	{
		prepare();
		//qDebug() << "PropertyNode(" << name() << ")->childCount = " << properties.size();
		return properties.size();
	}
	int PropertyNode::columnCount() const
	{
		//qDebug() << "PropertyNode(" << name() << ")->columnCount = " << 1;
		return 1;
	}

	QVariant PropertyNode::data(int row, int column, int role) const
	{
		//qDebug() << "PropertyNode(" << name() << ")->data(" << row << ", " << column << ", " << role << ")";
		if(column == 0 && row < properties.size())
		{
			if(role == Qt::DisplayRole)
				return "property: " + properties[row]->name();
			else if(role == Qt::ForegroundRole)
				return Qt::magenta;
		}
		return QVariant();
	}
	QModelIndex PropertyNode::child(BrowserTreeModel const &tree, int row, int column) const
	{
		//qDebug() << "PropertyNode(" << name() << ")->child(" << row << ", " << column << ")";
		if(properties.size() > row)
			return tree.createIndex(row, column, static_cast<Node *>(properties[row]));
		return QModelIndex();
	}

	LiveNodes InterfaceNode::createModel(QUrl interface)
	{
		RDFSelect select;
		select.newColumn("predicate").property<rdfs::domain>() = interface;
		return ::tracker()->modelQuery(select);
	}

	InterfaceNode::InterfaceNode(LiveNode resource, QUrl interface, Node *parent, int parent_row)
		: PropertyNode("has interface: " + interface.toString(), resource
		         , createModel(interface), parent, parent_row)
		, interface(interface)
	{}

	bool InterfaceNode::prepare() const
	{
		if(PropertyNode::prepare())
			return true;
		properties.push_back(new ResourceNode( ::tracker()->liveNode(interface)
		                                     , const_cast<InterfaceNode *>(this)
		                                     , properties.size()));
		return false;
	}
	QVariant InterfaceNode::data(int row, int column, int role) const
	{
		if(row + 1 == properties.size())
		{
			if(role == Qt::DisplayRole)
				return "interface metadata: " + interface.toString();
			else if(role == Qt::ForegroundRole)
				return Qt::red;
		}
		return PropertyNode::data(row, column, role);
	}

	ResourceNode::ResourceNode(LiveNode resource, Node *parent, int parent_row, QString footer)
		: Node(footer + resource.toString(), parent, parent_row)
		, resource(::tracker()->liveNode(QUrl(resource.toString())))
		, interfaces(
			::tracker()->modelQuery(
				RDFSelect().addColumn("interfaces", RDFVariable(resource).type())
			))
	{
		//qDebug() << this->resource.toN3();
		//qDebug() << "Resource(" << name() << ")-> interface count = " << interfaces->rowCount();
	}


	LiveNodes createSubjectOfModel(LiveNode subject)
	{
		RDFSelect select;
		RDFVariable pred = select.newColumn("predicate");
		select.groupBy(pred);
		RDFVariable(subject).property(pred);
		return ::tracker()->modelQuery(select);
	}

	LiveNodes createObjectOfModel(LiveNode object)
	{
		RDFSelect select;
		RDFVariable pred = select.newColumn("predicate");
		select.groupBy(pred);
		RDFVariable().property(pred) = object;
		return ::tracker()->modelQuery(select);
	}

	bool ResourceNode::prepare() const
	{
		if(Node::prepare())
			return true;
		ResourceNode *noncthis = const_cast<ResourceNode *>(this);
		categories.push_back(new PropertyNode
		        ( "is subject of", resource
		        , createSubjectOfModel(resource)
		        , noncthis, categories.size()));

		// TODO: report tracker bug with SELECT ?p WHERE { ?s ?p <url> }
/*		categories.push_back(new PropertyNode
		        ( "is object of", resource
		        , createObjectOfModel(resource)
		        , noncthis, categories.size()
		        , false));
*/
		LiveNodeList iflist = interfaces;
		for(LiveNodeList::iterator ifi = iflist.begin(); ifi != iflist.end(); ++ifi)
			categories.push_back(new InterfaceNode(resource, ifi->uri(), noncthis, categories.size()));
		return false;
	}
	int ResourceNode::childCount() const
	{
		prepare();
		//qDebug() << "ResourceNode(" << name() << ")->childCount = " << categories.size();
		return categories.size();
	}
	int ResourceNode::columnCount() const
	{
		//qDebug() << "ResourceNode(" << name() << ")->columnCount = " << 1;
		return 1;
	}
	QVariant ResourceNode::data(int row, int column, int role) const
	{
		//qDebug() << "ResourceNode(" << name() << ")->data(" << row << ", " << column << ", " << role << ")";
		if(column == 0 && row < categories.size())
		{
			if(role == Qt::DisplayRole)
				return categories[row]->name();
			else if(role == Qt::ForegroundRole)
				return Qt::blue;
		}
		return QVariant();
	}
	QModelIndex ResourceNode::child(BrowserTreeModel const &tree, int row, int column) const
	{
		//qDebug() << "ResourceNode(" << name() << ")->child(" << row << ", " << column << ")";
		if(categories.size() > row)
			return tree.createIndex(row, column, static_cast<Node *>(categories[row]));
		return QModelIndex();
	}


	LiveNodeModel_Node::LiveNodeModel_Node(QString name, LiveNodes model_, Node *parent, int parent_row, QString footer_name, LiveNode footer)
		: Node(name, parent, parent_row)
		, model(model_, Creator(bind1st(mem_fun(&LiveNodeModel_Node::createNode), this)))
		, footer_name(footer_name)
		, footer(footer)
		, footer_node(0)
	{
		//qDebug() << "LiveNodeModel(" << name << ")->item count = " << model->rowCount();
	}

	bool LiveNodeModel_Node::prepare() const
	{
		if(Node::prepare())
			return true;
		children = model;
		if(children.size() && footer.isValid())
			footer_node = new ResourceNode(footer, const_cast<LiveNodeModel_Node *>(this), children.size(), footer_name);
		//qDebug() << "children of " << name_ << ": " << children.size();
		return false;
	}
	int LiveNodeModel_Node::childCount() const
	{
		prepare();
		//qDebug() << "LiveNodeModel(" << name() << ")->childCount = " << children.size() + (footer_node ? 1 : 0);
		return children.size() + (footer_node ? 1 : 0);
	}
	int LiveNodeModel_Node::columnCount() const
	{
		//qDebug() << "LiveNodeModel(" << name() << ")->columnCount = " << model->columnCount();
		return model->columnCount();
	}
	QVariant LiveNodeModel_Node::data(int row, int column, int role) const
	{
		//qDebug() << "LiveNodeModel(" << name() << ")->data(" << row << ", " << column << ", " << role << ")" << var.toString();
		if(children.size() > row)
		{
			if(role == Qt::ForegroundRole)
				return Qt::red;
			else
				return model->data(model->index(row, column), role);
		}
		if(footer_node && row == children.size())
		{
			if(role == Qt::DisplayRole)
				return footer_node->name();
			else if(role == Qt::ForegroundRole)
				return Qt::red;
		}
		return QVariant();
	}
	QModelIndex LiveNodeModel_Node::child(BrowserTreeModel const &tree, int row, int column) const
	{
		//qDebug() << "LiveNodeModel(" << name() << ")->child(" << row << ", " << column << ")";
		if(children.size() > row)
			return tree.createIndex(row, column, static_cast<Node *>(children[row]));
		if(footer_node && row == children.size())
			return tree.createIndex(children.size(), column, footer_node);
		return QModelIndex();
	}
	ResourceNode *LiveNodeModel_Node::createNode(NodeRow noderow)
	{
		return new ResourceNode(noderow.first, this, noderow.second);
	}

	BrowserTreeModel::BrowserTreeModel(LiveNodes query_results, QObject */*parent*/)
		: root("Root", query_results, 0, -1)
	{}

	BrowserTreeModel::~BrowserTreeModel() {}

	QVariant BrowserTreeModel::data(const QModelIndex &index, int role) const
	{
		Node *node = static_cast<Node *>(index.internalPointer());

		QVariant ret = node->parent_->data(index.row(), index.column(), role);
/*		qDebug() << node->name() << "->data(" << index.row() <<
		                                  ", " << index.column() <<
		                                  ", " << role <<
		                                  ") = "<< ret.toString();
*/		return ret;
	}
	Qt::ItemFlags BrowserTreeModel::flags(const QModelIndex &index) const
	{
	    if (!index.isValid())
	         return 0;
	     return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
	QVariant BrowserTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
	{
	     if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	         return root.model->headerData(section, orientation, role);
	     return QVariant();
	}
	QModelIndex BrowserTreeModel::index(int row, int column, const QModelIndex &parent) const
	{
		Node const *node = !parent.isValid() ? &root : static_cast<Node *>(parent.internalPointer());
		QModelIndex mi = node->child(*this, row, column);
		//qDebug() << node->name() << "->index(" << row << ", " << column << ") ="
		//         << (mi.isValid() ? static_cast<Node *>(mi.internalPointer())->name() : "invalid");
		return mi;
	}
	QModelIndex BrowserTreeModel::parent(const QModelIndex &index) const
	{
		if(index.isValid())
		{
			Node *node = static_cast<Node *>(index.internalPointer());
			//qDebug() << node->name() << "->parent() = " << node->parent_->name();
			if(node->parent_ != &root)
				return createIndex(node->parent_->parent_row, 0, node->parent_);
		}
		// else qDebug() << "root->parent() = None";
		return QModelIndex();
	}
	int BrowserTreeModel::rowCount(const QModelIndex &parent) const
	{
		Node const *node = !parent.isValid() ? &root : static_cast<Node *>(parent.internalPointer());
//		qDebug() << node->name() << "->childCount() = " << node->childCount();
		return node->childCount();
	}
	int BrowserTreeModel::columnCount(const QModelIndex &parent) const
	{
		Node const *node = !parent.isValid() ? &root : static_cast<Node *>(parent.internalPointer());
//		qDebug() << node->name() << "->columnCount() = " << node->columnCount();
		return node->columnCount();
	}



	Browser::Browser()
		: QSplitter(Qt::Vertical)
	{
		addWidget(top_split = new QSplitter(Qt::Horizontal));
		addWidget(bottom_area = new QWidget());
		bottom_area->setLayout(right_vertical = new QVBoxLayout());
		right_vertical->addLayout(text_filter_row = new QHBoxLayout());
		text_filter_row->addWidget(new QLabel("Filter in column"));
		text_filter_row->addWidget(column_menu = new QComboBox());
		text_filter_row->addWidget(new QLabel("by prefix:"));
		text_filter_row->addWidget(text_filter_edit = new QLineEdit());
		text_filter_edit->connect(text_filter_edit, SIGNAL(textChanged(const QString &)), this, SLOT(filterEdited()));

		right_vertical->addWidget(trackertable = new TrackerTable());
		setStretchFactor(0, 0);
		setStretchFactor(1, 1);

		top_split->addWidget(query_select_widget = new QWidget());
		top_split->addWidget(query_view_widget = new QWidget());

		query_select_widget->setLayout(query_select_box = new QVBoxLayout());
		query_view_widget->setLayout(query_view_box = new QVBoxLayout());

		query_select_box->addWidget(query_select = new QListView());

		select = 0;

		queries = new QStandardItemModel(this);
		queries->setHorizontalHeaderItem(0, new QStandardItem("RDFQuery"));
		loadQueries(*queries);
		query_select->setSelectionMode(QAbstractItemView::SingleSelection);
		query_select->setModel(queries);
		//qDebug() << "Gained" << queries->rowCount() << "followers";

		query_select_box->addLayout(select_viewer_buttons = new QHBoxLayout());

		select_viewer_buttons->addWidget(load_button = new QPushButton("&Load query"));
		load_button->connect(load_button, SIGNAL(clicked(bool)), this, SLOT(loadQuery()));

		select_viewer_buttons->addWidget(save_button = new QPushButton("&Save query"));
		save_button->connect(save_button, SIGNAL(clicked()), this, SLOT(saveQuery()));

		query_view_box->addWidget(new QLabel("Generated SPARQL:"));
		query_view_box->addWidget(query_viewer = new QPlainTextEdit());
		query_viewer->connect(query_viewer, SIGNAL(textChanged()), this, SLOT(queryEdited()));

		query_view_box->addWidget(fields_viewer = new QLineEdit());

		query_view_box->addWidget(execute_button = new QPushButton("&Model query"));
		execute_button->connect(execute_button, SIGNAL(clicked()), this, SLOT(executeQuery()));

		updateGeometry();
	}

	Browser::~Browser()
	{
		setSelect(0);
	}

	void Browser::setSelect(RDFSelect *select)
	{
		if(this->select)
			delete this->select;
		this->select = select;
	}

	void Browser::refreshQuery()
	{
		setSelect(new RDFSelect());
		(*query_builder)(*select);
		//qDebug() << select->getQuery();
	}

	void Browser::loadQuery()
	{
		//qDebug() << "Dragons ahead...";
		QString text, columns;
		QList<QModelIndex> selection = query_select->selectionModel()->selectedIndexes();
		if(!selection.size())
		{
			//qDebug() << "I alone am mighty!";
			return;
		}
		QVariant data = selection.front().data(Qt::UserRole + 1);
		//qDebug() << "I choose " << selection.front().data().toString() << "!";

		if((query_builder = data.value<QueryBuilderPtr>()))
		{
			column_menu->clear();
			refreshQuery();
			text = select->getQuery();
			columns = QString();
			for(RDFSelectColumnList::iterator fi = select->columns().begin(); fi != select->columns().end(); ++fi)
			{
				column_menu->addItem(fi->name());
				columns += fi->name() + ";";
			}
			columns.resize(columns.size() - 1);
		} else
		{
			setSelect(0);
			text = data.toString();
			columns = selection.front().data(Qt::UserRole + 2).toString();
		}

		//qDebug() << "Brandishing my weapons: " << columns.split(";").join(", ");

		query_viewer->blockSignals(true);
		query_viewer->setPlainText(text);
		query_viewer->blockSignals(false);
		fields_viewer->setText(columns);
		executeQuery();
	}

	void Browser::saveQuery()
	{}

	void Browser::queryEdited()
	{
		setSelect(0);
		column_menu->clear();
		text_filter_edit->clear();
		//qDebug() << "We Strike As One!";
	}

	void Browser::filterEdited()
	{
		//qDebug() << "Waving my finger";
		if(!query_builder || !column_menu->count())
		{
			//qDebug() << "Got hit";
			text_filter_edit->blockSignals(true);
			text_filter_edit->clear();
			text_filter_edit->blockSignals(false);
			return;
		}
		refreshQuery();
		if(text_filter_edit->text().size())
			select->columns()[column_menu->currentIndex()].variable().contains(text_filter_edit->text());
		query_viewer->blockSignals(true);
		query_viewer->setPlainText(select->getQuery());
		query_viewer->blockSignals(false);
		executeQuery();
	}

	void Browser::executeQuery()
	{
		LiveNodes model;
		if(select)
			model = ::tracker()->modelQuery(*select);
		else
		{
			QString query = query_viewer->toPlainText();
			if(query.mid(0, 6) == "SELECT")
				model = ::tracker()->modelQuery(query, fields_viewer->text().split(";"));
			else
			{
				::tracker()->rawSparqlUpdateQuery(query);
				return;
			}
		}

		trackermodel = initialSharedFromThis(new BrowserTreeModel(model));
		trackertable->setModel(trackermodel.data());
	}

}

using namespace TrackerBrowser;

int main(int argc, char *argv[])
{
	qsrand(QDateTime::currentDateTime().toTime_t());

	if(argc == 3 && QString(argv[1]) == "-v")
		Tracker::setTrackerVerbosity(QString(argv[2]).toInt());
	else
		Tracker::setTrackerVerbosity(1);

	//qDebug() << "We live!";

	QApplication app(argc, argv);

	//qDebug() << "Go to town.";

	Browser frame;

	//qDebug() << "Past the gates";

	frame.show();

	//qDebug() << "And to the wilderness!";

	return app.exec();
}

