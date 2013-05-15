#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nmm.h>

using namespace SopranoLive;
using namespace SopranoLive::Ontologies;

int main(int /*argc*/, char */*argv*/[])
{
	::tracker()->setVerbosity(3);

	Live<nmm::MusicPiece> piece = ::tracker()->createLiveResource<nmm::MusicPiece>();
	piece->getPerformer()->setArtistName("MaginaryArtist");

	QUrl piece_url = piece.uri();

	piece = LiveNode();

	qDebug() << ::tracker()->liveResource<nmm::MusicPiece>(piece_url)->getPerformer()->getArtistName();
}
