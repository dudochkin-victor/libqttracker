#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QFile>
#include <QTextStream>

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nfo.h>

using namespace SopranoLive;

int main (int argc, char** argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    QUrl bookmarkUrl("http://www.abc.com");
    QUrl bookmarkFolder("http://www.abc.com/AbcFolder");

    QUrl bookmark_id;

    {
		// Store the bookmark and folder
		Live<nfo::Bookmark> bookmark = ::tracker()->createLiveNode();
		bookmark_id = bookmark.uri();
		bookmark->addBookmarks(bookmarkUrl);
		Live<nfo::BookmarkFolder> folder = ::tracker()->liveNode(bookmarkFolder);
		folder->addContainsBookmark(bookmark);
    }

    {
        Live<nfo::Bookmark> bookmark = ::tracker()->liveNode(bookmark_id);

        qDebug() << "Bookmark url:" << bookmark->getBookmarks().uri();

        LiveNodes folders = bookmark->LiveResource::getSubjects<nfo::containsBookmark>();

        int rows = folders->rowCount();
        qDebug() << "NUMBER OF FOLDERS: " << rows;
        if(rows)
        	qDebug() << "FOLDER NAME: " << folders->liveNode(0).uri().toString();
    }


    return 0;
    /* should give something like
New group object:  "http://www.example.org/unique/generated/242063264#0"
Set group name to:  "test group"
Retrieved the name:  "test group"
Iterating all ContactGroup objects
Id: "http://www.example.org/unique/generated/242063264#0" , name: "test group"
Done
     */
}
