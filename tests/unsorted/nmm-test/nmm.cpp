#include <QtTest>
#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nmm.h>
using namespace SopranoLive;

/* /etc/X11/Xsession.d/01dbus ; source /tmp/session_bus_address.user ; /usr/lib/tracker/trackerd -s 1 -r -v 3 */


class TestLiveModel : public QObject
{
	        Q_OBJECT
	       private slots:
	                void set_music_album();
};

void TestLiveModel :: set_music_album()
{
	QString insert_query_1 =
		"INSERT "
		"{	<urn:uuid:1129025374> a nco:Contact" \
		"		; nco:fullname 'Artist_1_update'." \
		"	<urn:album:Trance Time> a nmm:MusicAlbum" \
		"		; nmm:albumArtist <urn:uuid:1129025374>" \
		"		; nie:title \"Trance Time\"." \
		"	<file:///media/PIKKUTIKKU/5000_songs_with_metadata_and_album_arts/Artist_1/1_Album/10_song.mp3> a nmm:MusicPiece,nfo:FileDataObject" \
		"		; nfo:fileName '10_song.mp3'" \
		"		; nfo:fileLastModified '2008-10-23T13:47:02' " \
		"		; nfo:fileCreated '2008-12-16T12:41:20'" \
		"		; nfo:fileSize 17630" \
		"		; nmm:musicAlbum <urn:album:Trance Time>" \
		"		; nie:title \"10_song_update\"" \
		"		;  nmm:trackNumber 10" \
		"		; nmm:genre \"Classic Rock update\"" \
		"		; nmm:length 219252" \
		"		; nmm:performer <urn:uuid:1129025374>." \
		"}";

	::tracker()->rawSparqlUpdateQuery(insert_query_1);

	//QUrl song = nmm::MusicPiece::iri();
	QUrl song = QUrl("file:///media/PIKKUTIKKU/5000_songs_with_metadata_and_album_arts/Artist_1/1_Album/10_song.mp3");
	Live<nmm::MusicPiece> album = ::tracker()->liveNode(song) ;
	qDebug() << album->getMusicAlbum().toString();

        /*
	QStringList uris;
	RDFSelect query;
	RDFVariable  album, artist, musicPiece ;
	musicPiece.property<nmm::musicAlbum>().property<nie::title>() = album;
	musicPiece.property<nmm::performer>().property<nco::fullname>() = artist;
	query.addColumn("Album",album);
	query.addColumn("Artist",artist);
	LiveNodes model = ::tracker()->modelQuery(query);
        for (int i = 0; i < model->rowCount(); i++)
        {
	        QString uri = model->data(model->index(i,0)).toString();
		qDebug() << model->data(model->index(i,1)).toString();
	        uris.append(uri);
	}
	qDebug() << uris;
	*/


}

QTEST_APPLESS_MAIN(TestLiveModel)
#include "nmm.moc"
