#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nmm.h>

using namespace SopranoLive;
using namespace SopranoLive::Ontologies;

int main(int /*argc*/, char */*argv*/[])
{
	::tracker()->setVerbosity(3);

	RDFSelect select;
	RDFVariable song = select.newColumn("song").isOfType<nmm::MusicPiece>();
	RDFVariable usage = song.optional().property<nie::usageCounter>();
	song.varPattern().variable(usage).isBound().not_();

    qDebug() << select.getQuery();

    LiveNode test_node = ::tracker()->createLiveNode();
    qDebug() << test_node.uri();

    QUrl test_url("urn:uuid:1235987");
    LiveNode test_node2 = ::tracker()->liveNode(test_url);

    foreach(LiveNode res, ::tracker()->modelQuery(select))
		qDebug() << res.toString();
}
