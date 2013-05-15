#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QFile>
#include <QTextStream>

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nao.h>

using namespace SopranoLive;

int main (int argc, char** argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    QString keyword("nmo:Email");
    RDFSelect select;
    qDebug() << "Finding tags for:" << keyword;
    RDFVariable file = select.newColumn("file");

    file.type() = QUrl(keyword);
    RDFVariable tagvar = file.property<nao::hasTag>().property<nao::prefLabel>();
    select.addColumn("tag", tagvar);

    LiveNodes model_ = ::tracker()->modelQuery(select);

    qDebug() << "Done";
    return 0;
}
