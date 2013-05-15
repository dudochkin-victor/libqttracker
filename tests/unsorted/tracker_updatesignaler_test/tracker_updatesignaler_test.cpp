#include "tracker_updatesignaler_test.h"
#include <QUrl>
#include <QDebug>

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nmm.h>

using namespace SopranoLive;
using namespace SopranoLive::Ontologies;

TrackerUpdateSignalerTest::TrackerUpdateSignalerTest()
{
    QUrl classId = nmm::MusicPiece::iri();
    //QUrl classId = nie::InformationElement::iri();
    BackEnds::Tracker::ClassUpdateSignaler *signaler = BackEnds::Tracker::ClassUpdateSignaler::get(classId);

        QObject::connect(signaler, SIGNAL(subjectsAdded(const QStringList &)),
                        this, SLOT(contentChanged(const QStringList &)));
        QObject::connect(signaler, SIGNAL(subjectsRemoved(const QStringList &)),
                        this, SLOT(contentChanged(const QStringList &)));
        QObject::connect(signaler, SIGNAL(subjectsChanged(const QStringList &)),
                        this, SLOT(contentChanged(const QStringList &)));
}

TrackerUpdateSignalerTest::~TrackerUpdateSignalerTest()
{}

void TrackerUpdateSignalerTest::contentChanged(const QStringList &)
{
    qDebug()<<"Got contentchanged signal.\n";
}

