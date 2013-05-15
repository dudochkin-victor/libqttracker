#include "test.h"

void TestModelObjects::displayModel(LiveNodes const &objects)
{
	QStringList items;
	foreach(LiveNode node, objects.nodes())
		items << node.toString();
	qDebug() << "  Contained items: " << items.join(", ");
}

void TestModelObjects::objectsModelUpdated()
{
	qDebug() << "Objects model updated.";
	displayModel(objects);
}

void TestModelObjects::objectRowsInserted(const QModelIndex &/*parent*/, int start, int end)
{
	qDebug() << "Objects rows" << start << "-" << end << "inserted";
	displayModel(objects);
}

void TestModelObjects::objectRowsRemoved(const QModelIndex &/*parent*/, int start, int end)
{
	qDebug() << "Objects rows" << start << "-" << end << "removed";
	displayModel(objects);
}

void TestModelObjects::objectDataChanged(const QModelIndex &topleft, const QModelIndex &bottomright)
{
	qDebug() << "Objects rows" << topleft.row() << "-" << bottomright.row() << "changed"
		<< " (columns" << topleft.column() << "-" << bottomright.column() << ")";
	displayModel(objects);
}

void TestModelObjects::run()
{
	//::tracker()->setVerbosity(3);
	Live<nfo::BookmarkFolder> folder = ::tracker()->createLiveNode();
	qDebug() << "Created empty nfo:BookmarkFolder, getting nfo:containsBookmarks:";
	displayModel(folder->getContainsBookmarks());
	qDebug() << "Adding nfo:containsBookmarks:" << folder->addContainsBookmark().toString();
	qDebug() << "Adding nfo:containsBookmarks:" << folder->addContainsBookmark().toString();
	qDebug() << "Retrieving live bookmarks the first time";
	objects = folder->liveContainsBookmarks();
	qDebug() << "Retrieving live bookmarks the second time";
	LiveNodes objects2 = folder->liveContainsBookmarks();

	QObject::connect( objects.model(), SIGNAL(rowsInserted(const QModelIndex &, int, int))
	                , this, SLOT(objectRowsInserted(const QModelIndex &, int, int)));
	QObject::connect( objects.model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int))
	                , this, SLOT(objectRowsRemoved(const QModelIndex &, int, int)));
	QObject::connect( objects.model(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &))
	                , this, SLOT(objectDataChanged(const QModelIndex &, const QModelIndex &)));

	displayModel(objects);

	qDebug() << "Retrieving live bookmarks the third time";
	LiveNodes objects3 = folder->liveContainsBookmarks();

	qDebug() << "Displaying first set:";
	displayModel(objects);
	qDebug() << "Displaying second set:";
	displayModel(objects2);
	qDebug() << "Displaying third set:";
	displayModel(objects3);

	qDebug() << "Adding two more nfo:containsBookmarks:";
	qDebug() << "Adding nfo:containsBookmarks:" << folder->addContainsBookmark().toString();
	qDebug() << "Adding nfo:containsBookmarks:" << folder->addContainsBookmark().toString();

	displayModel(folder->getContainsBookmarks());

	qDebug() << "Displaying first set:";
	displayModel(objects);
	qDebug() << "Displaying second set:";
	displayModel(objects2);
	qDebug() << "Displaying third set:";
	displayModel(objects3);

	objects = objects2 = objects3 = LiveNodes();

	objects = folder->liveContainsBookmarks();

	qDebug() << "Displaying first set after resetting:";
	displayModel(objects);

	QObject::connect( objects.model(), SIGNAL(rowsInserted(const QModelIndex &, int, int))
	                , this, SLOT(objectRowsInserted(const QModelIndex &, int, int)));
	QObject::connect( objects.model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int))
	                , this, SLOT(objectRowsRemoved(const QModelIndex &, int, int)));
	QObject::connect( objects.model(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &))
	                , this, SLOT(objectDataChanged(const QModelIndex &, const QModelIndex &)));

	qDebug() << "Adding nfo:containsBookmarks:" << folder->addContainsBookmark().toString();
	qDebug() << "Adding nfo:containsBookmarks:" << folder->addContainsBookmark().toString();

	displayModel(folder->getContainsBookmarks());
//	qDebug() << "Removing nfo:containsBookmarks:" << objects->liveNode(1).toString();
//	folder->removeContainsBookmark(objects->liveNode(1));
	qDebug() << "Removing nfo:containsBookmarks:" << objects->liveNode(0).toString();
	folder->removeContainsBookmark(objects->liveNode(0));

    qDebug() << "Done";
}

int main(int argc, char *argv[])
{
    QCoreApplication core(argc, argv);
    TestModelObjects tc;
    tc.run();
    return core.exec();
}

//QTEST_APPLESS_MAIN(TestModelObjects)
