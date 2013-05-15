#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QFile>
#include <QTextStream>

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nao.h>
#include <QtTracker/ontologies/nfo.h>
#include <QtTracker/ontologies/ncal.h>
#include <QtTracker/ontologies/nid3.h>

using namespace SopranoLive;

void test1()
{
    QUrl uri = QString("http://test.com/uri");
    QString tag = "favorite";
    QList<QUrl> types;
    types << nfo::Image::iri() << ncal::Event::iri() << nid3::ID3Audio::iri();

    RDFSelect content_query;

    RDFVariable content_uri;
    content_uri.notEqual(uri);
    content_uri.property<nao::hasTag>().property<nao::prefLabel>() = LiteralValue(tag);

    RDFPattern current_union = content_uri.pattern().child();
    RDFVariable label;

    foreach(QUrl type, types)
    {
        RDFVariable label_in_union = current_union.variable(label);
        RDFVariable content_uri_in_union = current_union.variable(content_uri);
        content_uri_in_union.isOfType(type,false);
        content_uri_in_union.property<nao::hasTag>() = label_in_union;

        current_union = current_union.union_();
    }

    content_query.addColumn("content_uri", content_uri);
    content_query.addCountColumn("label_count", label);
    content_query.addColumn("type", content_uri.type());

    content_query.groupBy(content_uri);

    qDebug() << content_query.getQuery();
}

void test2()
{
	QString tag = "favorite";
	QUrl type = nfo::Image::iri();
    QUrl uri = QString("http://test.com/uri");

    /* ****** SPARQL Query ****************
    SELECT ?content_uri WHERE {
        ?content_uri a ?content_type ;
            nao:hasTag [ nao:prefLabel ?tag];
    }
       ****** SPARQL Query **************** */

    RDFSelect content_query;

    RDFVariable content_uri;
    content_uri.isOfType(QUrl(type),false);
    content_uri.notEqual(uri);
    content_uri[nao::iri("hasTag")][nao::iri("prefLabel")] = LiteralValue(tag);

    content_query.addColumn(QString("content_uri"),content_uri);

    qDebug() << content_query.getQuery();
}

void test3()
{
	QStringList tags;
	tags << "favorite" << "new" << "cool";
	QUrl type = nfo::Image::iri();
    QUrl uri = QString("http://test.com/uri");

    RDFSelect content_query;

    RDFVariable content_uri;
    content_uri.isOfType(QUrl(type),false);
    content_uri.notEqual(uri);

    RDFPattern current_union = content_uri.pattern().child();
    RDFVariable label;

    foreach(QString tag, tags)
    {
        RDFVariable label_in_union = current_union.variable(label);
        RDFVariable content_uri_in_union = current_union.variable(content_uri);

        label_in_union.property<nao::prefLabel>() = LiteralValue(tag);
        content_uri_in_union.property<nao::hasTag>() = label_in_union;
        current_union = current_union.union_();
    }

    content_query.addColumn("content_uri", content_uri);
    content_query.addCountColumn("label_count", label);
    content_query.groupBy(content_uri);

    qDebug() << content_query.getQuery();
}

int main (int argc, char** argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    test1();
    test2();
    test3();

    qDebug() << "Done";
    return 0;
}
