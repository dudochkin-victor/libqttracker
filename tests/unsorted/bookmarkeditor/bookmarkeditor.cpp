#include "bookmarkeditor.h"

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/rdf.h>
#include <QtTracker/ontologies/nfo.h>
#include <QtTracker/ontologies/nie.h>

using namespace SopranoLive;
using namespace SopranoLive::Ontologies;

BookmarkEditor::BookmarkEditor(QWidget * parent) :
    QWidget(parent)
{
    m_ui.setupUi(this);

    connect(m_ui.startButton, SIGNAL(clicked()), this, SLOT(insertItem()));

    RDFSelect select;

    RDFVariable entry   = RDFVariable::fromType<nfo::WebHistory>();
    RDFVariable uri     = entry.property<nfo::uri>();
    RDFVariable count   = entry.property<nie::usageCounter>();
    RDFVariable refresh = entry.optional().property<nie::lastRefreshed>();

    select.addColumn("IRI", entry);
    select.addColumn("URI", uri);
    select.addColumn("Count", count);
    select.addColumn("Refreshed", refresh);

    SopranoLive::BackEnds::Tracker::setTrackerVerbosity(255);
    m_model = ::tracker()->
        modelQuery(select, SopranoLive::LiveNodeModel::StreamingWritableStrategy);
    m_ui.bookmarkView->setModel(m_model.model());
}

BookmarkEditor::~BookmarkEditor()
{ }

void BookmarkEditor::insertItem()
{
    qDebug() << "Inserting...";

    int index = m_model.model()->rowCount();

    QString id("urn:uuid:" + QUuid::createUuid().toString());
    id.replace(QChar('{'), "");
    id.replace(QChar('}'), "");
    uint timestamp = QDateTime::currentDateTime().addMonths(-1).addSecs(index).toTime_t();
    QString uri(QString("http://secret%1.com/index.html").arg(index));

    LiveNodeList row;

    row << Soprano::Node::createResourceNode(QUrl(id));
    row << Soprano::Node::createResourceNode(QUrl(uri));
    row << Soprano::Node::createLiteralNode(Soprano::LiteralValue(1), QString());
    row << Soprano::Node::createLiteralNode(Soprano::LiteralValue(timestamp), QString());

    m_model->insertRows(0, row);
}
