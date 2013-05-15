#include "perf-tests.h"

static QString const simpleQuery("select ?id ?url ?mime ?width ?height ?cam ?exp ?flash ?wb { "
								 "?id a nmm:Photo ; "
								 "nie:url ?url ; "
								 "nie:mimeType ?mime ; "
								 "nfo:width ?width ; "
								 "nfo:height ?height ; "
								 "nmm:camera ?cam ; "
								 "nmm:exposureTime ?exp ; "
								 "nmm:flash ?flash ; "
								 "nmm:whiteBalance ?wb . "
								 "}");

void PerfTests::basic_plainQuery_qtdbus()
{
	QDBusInterface tracker(QLatin1String("org.freedesktop.Tracker1"),
						   QLatin1String("/org/freedesktop/Tracker1/Resources"),
						   QLatin1String("org.freedesktop.Tracker1.Resources"));
	QVariantList args;
	args << simpleQuery;
	PERF_EVENT("pre_basic_plainQuery_qtdbus");
	QDBusMessage m = tracker.callWithArgumentList(QDBus::Block, "SparqlQuery", args);
    QDBusReply<QVector<QStringList> > r(m);
    QVector<QStringList> demarshalled(r.value());
	PERF_EVENT("done");
}

void PerfTests::basic_plainQuery_raw()
{
	PERF_EVENT("pre_basic_plainQuery_raw");
	for (int i = 0; i < iterations; ++i) {
		QVector<QStringList> res = ::tracker()->rawSparqlQuery(simpleQuery);
	}
	PERF_EVENT("done");
}

void PerfTests::basic_plainQuery()
{
	RDFVariable photo = RDFVariable::fromType<nmm::Photo>();
	RDFSelect sel;
	sel.addColumn(photo.object<nie::url>());
	sel.addColumn(photo.object<nie::mimeType>());
	sel.addColumn(photo.object<nfo::width>());
	sel.addColumn(photo.object<nfo::height>());
	sel.addColumn(photo.object<nmm::camera>());
	sel.addColumn(photo.object<nmm::exposureTime>());
	sel.addColumn(photo.object<nmm::flash>());
	sel.addColumn(photo.object<nmm::whiteBalance>());
	PERF_EVENT("pre_basic_plainQuery");
	for (int i = 0; i < iterations; ++i) {
		LiveNodes res = ::tracker()->modelQuery(sel);
		waitForSignal(res.model(), SIGNAL(derivedsUpdated()));
	}
	PERF_EVENT("done");
}

void PerfTests::complex_albumsForArtist_raw()
{
	QString query = "SELECT nmm:artistName(?artist) GROUP_CONCAT(nie:title(?album),'|') "
		"WHERE "
		"{ "
		"?song a nmm:MusicPiece . "
		"?song nmm:performer ?artist . "
		"?song nmm:musicAlbum ?album . "
		"} GROUP BY ?artist";
	PERF_EVENT("pre_complex_albumsForArtist_raw");
	for (int i = 0; i < iterations; ++i) {
		QVector<QStringList> res = ::tracker()->rawSparqlQuery(query);
	}
	PERF_EVENT("done");
}

void PerfTests::complex_albumsForArtist()
{
	RDFVariable artist = RDFVariable::fromType<nmm::Artist>();
	RDFVariable album;
	typedef RDFReverseProperty<nmm::performer>::Property<nmm::musicAlbum> AlbumOfArtist;
	artist.addDerivedObject<AlbumOfArtist>();
	RDFSelect sel;
	sel.addColumn("artist", artist);
	sel.orderBy(artist);
	PERF_EVENT("pre_complex_albumsForArtist");
	for (int i = 0; i < iterations; ++i) {
		LiveNodes res = ::tracker()->modelQuery(sel);
		connect(res.model(), SIGNAL(derivedsUpdated()), this, SLOT(dup()));
		waitForSignal(res.model(), SIGNAL(derivedsUpdated()));
	}
	PERF_EVENT("done");
}

void PerfTests::complex_gallery()
{
    SopranoLive::RDFVariable photos_and_videos_var(SopranoLive::RDFVariable::fromType<SopranoLive::nfo::Visual>());
    photos_and_videos_var.isOfType<nmm::Photo>(false, RDFStrategy::DerivedProperty);
    photos_and_videos_var.isOfType<nmm::Video>(false, RDFStrategy::DerivedProperty);

    SopranoLive::RDFSelect query;
    query.addColumn ("url", photos_and_videos_var.property<SopranoLive::nie::url>());
    query.addColumn ("filename", photos_and_videos_var.property<SopranoLive::nfo::fileName>());
    RDFVariable modified = photos_and_videos_var.property<SopranoLive::nfo::fileLastModified>();
    query.addColumn ("modified", modified);
    query.addColumn ("width", photos_and_videos_var.optional().property<SopranoLive::nfo::width>());
    query.addColumn ("height", photos_and_videos_var.optional().property<SopranoLive::nfo::height>());
    query.addColumn ("mimetype", photos_and_videos_var.property<SopranoLive::nie::mimeType>());
    query.addColumn ("duration", photos_and_videos_var.function<SopranoLive::nfo::duration>());
    query.addColumn ("photosandvideo", photos_and_videos_var );
    query.orderBy(modified, true);

	PERF_EVENT("pre_complex_gallery");
	for (int i = 0; i < iterations; ++i) {
		LiveNodes res = ::tracker()->modelQuery(query);
		waitForSignal(res.model(), SIGNAL(derivedsUpdated()));
	}
	PERF_EVENT("done");
}
