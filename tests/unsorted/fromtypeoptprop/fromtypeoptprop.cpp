#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QFile>
#include <QTextStream>

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nmm.h>
#include <QtTracker/ontologies/nfo.h>

using namespace SopranoLive;

int main (int argc, char** argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    RDFSelect select;
    RDFVariable video = RDFVariable::fromType<nmm::Video>();
    RDFVariable title = video.property<nfo::fileName>();
    RDFVariable length = video.optional().property<nmm::length>();
    select.addColumn("Video", video);

    qDebug() << select.getQuery();

    return 0;
}
