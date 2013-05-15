#include <QString>

#include "mafwsuite.h"

using namespace SopranoLive;
using namespace SopranoLive::BackEnds::Mafw;

MafwSuite::MafwSuite()
{
	qRegisterMetaType<SopranoLive::RDFStrategyFlags>("RDFStrategyFlags");
	qRegisterMetaType<QModelIndex>("QModelIndex");
}

void MafwSuite::initTestCase()
{
	m_dummyResource = new DummyResource();
}

void MafwSuite::testCreation()
{
	RDFServicePtr service=SopranoLive::BackEnds::Mafw::getRDFService(*m_dummyResource);
	QCOMPARE( service->serviceIri().toString(), QString("dummy") );
}

void MafwSuite::testBasicQuery()
{
	// prepare query

	RDFSelect select2;
	RDFVariable media=RDFVariable::fromType<nie::DataObject>();
	RDFVariable title=media.optional().property<nie::title>();
	select2.addColumn("node", media);
	select2.addColumn("title", title);

	// prepare result

	QList<QStringList> resultRows;
	QStringList row1, row2, row3, row4;
	row1 << "file://musa/biisi1.wav" << "Biisi 1";
	row2 << "file://musa/biisi2.wav" << "Biisi 2";
	row3 << "file://musa/biisi3.wav" << "Biisi 3";
	row4 << "file://musa/biisi4.wav" << "Biisi 4";
	resultRows << row1 << row2 << row3 << row4;
	m_dummyResource->setNextQueryResult( resultRows );

	resetSignalStatus();

	// body

	RDFServicePtr service = SopranoLive::BackEnds::Mafw::getRDFService(*m_dummyResource);
	m_resultNodes = service->modelQuery(select2, LiveNodeModel::Streaming);

	connect( m_resultNodes.model(), SIGNAL( dataChanged(const QModelIndex&, const QModelIndex&) ), this, SLOT( dataChangedSlot(const QModelIndex&, const QModelIndex&) ) );
	connect( m_resultNodes.model(), SIGNAL( rowsInserted( const QModelIndex&, int, int) ), this, SLOT( rowsInsertedSlot( const QModelIndex &, int, int) ) );
	connect( m_resultNodes.model(), SIGNAL( modelUpdated() ), this, SLOT( modelUpdatedSlot() ) );

	QTest::qWait(200);

	//QCOMPARE( m_dataChangedSignals, 1 );
	QCOMPARE( m_rowsInsertedSignals, 4 );
	QCOMPARE( m_rowsInsertedSignalStarts[0], 0 );
	QCOMPARE( m_rowsInsertedSignalEnds[3], 3 );
	QCOMPARE( m_modelUpdatedSignals, 1 );
}

void MafwSuite::testBasicQueryResultNodes()
{
	QList<LiveNode> list_view = m_resultNodes.nodes();

	int count=1;


	for(QList<LiveNode>::iterator i = list_view.begin(); i != list_view.end(); ++i) {
		QCOMPARE( i->toString(), QString("file://musa/biisi")+QString::number(count)+".wav" );
		LiveNode node=*i;

		// title is browsed from backend (no caching in use here)
		m_dummyResource->resetNextBrowseResult();
		QMap<QString,QVariant> metamap;
		metamap.insert( nie::title::iri().toString(), "Biisi "+QString::number(count) );
		m_dummyResource->addNextBrowseResult( QString("file://musa/biisi")+QString::number(count)+".wav", metamap );

		QCOMPARE( Live<nie::InformationElement>(*i)->getTitle(), QString("Biisi ")+QString::number(count) );

		count++;
	}
}

void MafwSuite::testComplexQuery()
{
	// prepare query

	RDFSelect select;
	RDFVariable song = RDFVariable::fromType<nmm::MusicPiece>();
	song.property<nie::title>() = LiteralValue("songname");
	select.addColumn("Song", song);

	RDFVariable artist = RDFVariable::fromType<nmm::Artist>();
	artist.property<nie::title>() = LiteralValue("artistname");
	song[nmm::performer::iri()]=artist;

	RDFVariable genre=song.property<nfo::genre>();
	select.addColumn("Genre", genre);
	RDFVariable mime=song.property<nie::mimeType>();
	select.addColumn("Mime", mime);
	RDFVariable track=song.property<nmm::trackNumber>();
	select.addColumn("Track", track);

	// prepare result

	QList<QStringList> resultRows;
	QStringList row1, row2, row3, row4;
	row1 << "file://musa/biisi1.wav" << "Genre 1" << "mime1" << "1";
	row2 << "file://musa/biisi2.wav" << "Genre 2" << "mime2" << "2";
	row3 << "file://musa/biisi3.wav" << "Genre 3" << "mime3" << "3";
	row4 << "file://musa/biisi4.wav" << "Genre 4" << "mime4" << "4";
	resultRows << row1 << row2 << row3 << row4;
	m_dummyResource->setNextQueryResult( resultRows );

	resetSignalStatus();

	// actual case

	m_resultNodes = SopranoLive::BackEnds::Mafw::getRDFService(*m_dummyResource)->modelQuery(select);

	connect( m_resultNodes.model(), SIGNAL( dataChanged(const QModelIndex&, const QModelIndex&) ), this, SLOT( dataChangedSlot(const QModelIndex&, const QModelIndex&) ) );
	connect( m_resultNodes.model(), SIGNAL( rowsInserted( const QModelIndex&, int, int) ), this, SLOT( rowsInsertedSlot( const QModelIndex &, int, int) ) );
	connect( m_resultNodes.model(), SIGNAL( modelUpdated() ), this, SLOT( modelUpdatedSlot() ) );

	QTest::qWait(200);

	//QCOMPARE( m_dataChangedSignals, 1 );
	QCOMPARE( m_rowsInsertedSignals, 4 );
	QCOMPARE( m_rowsInsertedSignalStarts[0], 0 );
	QCOMPARE( m_rowsInsertedSignalEnds[3], 3 );
	QCOMPARE( m_modelUpdatedSignals, 1 );
}

void MafwSuite::testQueryFailure()
{
	// preamble

	RDFSelect select;
	RDFVariable song = RDFVariable::fromType<nmm::MusicPiece>();
	song.property<nie::title>() = LiteralValue("songname");
	select.addColumn("Song", song);

	QList<QStringList> resultRows;
	QStringList row1;
	row1 << FAILURE_TOKEN;
	resultRows << row1;
	m_dummyResource->setNextQueryResult( resultRows );

	resetSignalStatus();

	// body

	m_resultNodes = SopranoLive::BackEnds::Mafw::getRDFService(*m_dummyResource)->modelQuery(select);
	connect(m_resultNodes.model(), SIGNAL(error(QString const&, RDFStrategyFlags, RDFStrategyFlags, QModelIndex const&)),
			this, SLOT(modelErrorSlot(QString const&, RDFStrategyFlags, RDFStrategyFlags, QModelIndex const&)));

	QTest::qWait(200);

	QCOMPARE( m_modelErrorSignals, 1 );
}

void MafwSuite::testQueryImmediateFailure()
{
	// preamble

	RDFSelect select;
	RDFVariable song = RDFVariable::fromType<nmm::MusicPiece>();
	song.property<nie::title>() = LiteralValue("songname");
	select.addColumn("Song", song);

	QList<QStringList> resultRows;
	QStringList row1;
	row1 << FAILURE_TOKEN_IMMEDIATE;
	resultRows << row1;
	m_dummyResource->setNextQueryResult( resultRows );

	// body

	m_resultNodes = SopranoLive::BackEnds::Mafw::getRDFService(*m_dummyResource)->modelQuery(select);
	QSignalSpy spy(m_resultNodes.model(), SIGNAL(error(QString const &, RDFStrategyFlags, RDFStrategyFlags, QModelIndex const&)) );

	QTest::qWait(200);

	QCOMPARE( spy.size(), 1 );
}

void MafwSuite::testBrowseTypes()
{
	// preamble

	m_dummyResource->resetNextBrowseResult();
	QMap<QString,QVariant> metamap;
	metamap.insert( rdf::type::iri().toString(), "weirdtype" );
	m_dummyResource->addNextBrowseResult( "0", metamap );

	// body

	QSharedPointer<RDFService> service = SopranoLive::BackEnds::Mafw::getRDFService(*m_dummyResource);
	LiveNode rootNode = SopranoLive::BackEnds::Mafw::rootNode(service);

	Live<rdfs::Resource> resource = rootNode;
	m_resultNodes = resource->getRdfTypes();

	qDebug() << "testBrowseTypes rowCount" << m_resultNodes.model()->rowCount();
	QCOMPARE( m_resultNodes.model()->rowCount(), 1 );
	QCOMPARE( m_resultNodes.model()->data( m_resultNodes.model()->index(0, 0), Qt::DisplayRole ).toString(), QString("weirdtype") );
}

void MafwSuite::testBrowseLogicalParts()
{
	// preamble

	m_dummyResource->resetNextBrowseResult();
	QMap<QString,QVariant> metamap;
	m_dummyResource->addNextBrowseResult( "obj1", metamap );
	m_dummyResource->addNextBrowseResult( "obj$2", metamap ); // have special character

	// body

	QSharedPointer<RDFService> service = SopranoLive::BackEnds::Mafw::getCachedRDFService(*m_dummyResource);

	LiveNode rootNode = SopranoLive::BackEnds::Mafw::rootNode(service);
	Live<nie::InformationElement> ie = rootNode;

	m_resultNodes = ie->getHasLogicalParts();

	QTest::qWait(200);

	QCOMPARE( m_resultNodes.nodes().size(), 2 );
	QCOMPARE( m_resultNodes.model()->data( m_resultNodes.model()->index(0, 0) ).toString(), QString("obj1" ) );
}

void MafwSuite::testBrowseTypesCached()
{
	// preamble

	m_dummyResource->resetNextBrowseResult();
	QMap<QString,QVariant> metamap;
	metamap.insert( nie::title::iri().toString(), "Title 1" );
	metamap.insert( rdf::type::iri().toString(), "type1" );
	m_dummyResource->addNextBrowseResult( "obj1", metamap );
	metamap.clear();
	metamap.insert( nie::title::iri().toString(), "Title 2" );
	metamap.insert( rdf::type::iri().toString(), "type2" );
	m_dummyResource->addNextBrowseResult( "obj$2", metamap ); // have special character

	// temp res for type query
	QList<QStringList> resultRows;
	QStringList row1, row2;
	row1 << "obj1" << "http://fancytype";
	row2 << "obj$2" << "http://fancytype";
	resultRows << row1 << row2;
	m_dummyResource->setNextQueryResult( resultRows );

	// body

	QSharedPointer<RDFService> service = SopranoLive::BackEnds::Mafw::getCachedRDFService(*m_dummyResource);

	LiveNode rootNode = SopranoLive::BackEnds::Mafw::rootNode(service);
	Live<nie::InformationElement> ie = rootNode;

	RDFVariable logicalparts_info;
	logicalparts_info.addDerivedObject<nie::title>();
	logicalparts_info.addDerivedObject<rdf::type>();

	m_resultNodes = ie->getHasLogicalParts(logicalparts_info);

	QTest::qWait(200);

	QList<LiveNode> list = m_resultNodes.nodes();
	int count=1;
	for(QList<LiveNode>::iterator i = list.begin(); i != list.end(); ++i) {
		QCOMPARE( Live<nie::InformationElement>(*i)->getTitle(), QString("Title ")+QString::number(count) );
		qDebug() << Live<nie::InformationElement>(*i)->getMimeType();
		QString type=Live<rdfs::Resource>(*i)->firstRdfType().toString();
		qDebug() << "type:" << type;
		QCOMPARE( type, QString("http://fancytype") );
		count++;
	}

	/*
	m_window = new QWidget;
	m_window->resize( 600, 400 );
	m_layout = new QVBoxLayout;
	m_window->setLayout(m_layout);
	m_view=new QListView;
	m_view->setModel(m_query_model.model());
	m_view->setModelColumn(1);
	m_layout->addWidget(m_view);
	m_window->show();
	connect( m_view, SIGNAL( doubleClicked(const QModelIndex&) ), this, SLOT( doubleClickedBrowse(const QModelIndex&) ) );
	*/

	// further browse

	QVariant node = m_resultNodes.model()->data( m_resultNodes.model()->index(0,0), LiveNodeModel::LiveNodeRole);
	Live<nie::InformationElement> liveNode=node.value<LiveNode>();

	RDFVariable cachedProps;
	cachedProps.addDerivedObject<nie::title>();

	metamap.clear();
	metamap.insert( nie::title::iri().toString(), "Title 3" );
	metamap.insert( rdf::type::iri().toString(), "type3" );
	m_dummyResource->resetNextBrowseResult();
	m_dummyResource->addNextBrowseResult( "obj3", metamap );

	m_resultNodes = liveNode->getHasLogicalParts(cachedProps);

	QCOMPARE( m_resultNodes.nodes().size(), 1 );
	foreach(LiveNode node, m_resultNodes)
	{
		QCOMPARE( Live<nie::InformationElement>(node)->getTitle(), QString("Title 3") );
	}
}

void MafwSuite::testBrowseAsynch()
{
	// preamble

	m_dummyResource->resetNextBrowseResult();
	QMap<QString,QVariant> metamap;
	metamap.insert( nie::title::iri().toString(), "Title 1" );
	m_dummyResource->addNextBrowseResult( "obj1", metamap );
	metamap.clear();
	metamap.insert( nie::title::iri().toString(), "Title 2" );
	m_dummyResource->addNextBrowseResult( "obj$2", metamap ); // have special character

	// body

	QSharedPointer<RDFService> service = SopranoLive::BackEnds::Mafw::getCachedRDFService(*m_dummyResource);
	LiveNode rootNode = SopranoLive::BackEnds::Mafw::rootNode(service);
	Live<nie::InformationElement> ie( rootNode, true );

	RDFVariable logicalparts_info;
	logicalparts_info.addDerivedObject<nie::title>();
	m_resultNodes = rootNode->getObjects<nie::hasLogicalPart>(logicalparts_info, LiveNodeModel::RequireStreaming|LiveNodeModel::RequireCached|LiveNodeModel::DefaultStrategy); // browse in streaming mode

	QAbstractItemModel* model=m_resultNodes.model();
	QCOMPARE( model->rowCount(), 0 );
	QTest::qWait(400);
	QCOMPARE( model->rowCount(), 2 );

	// check content (from cache)
	QList<QStringList> resultRows;
	m_dummyResource->setNextQueryResult( resultRows );

	QList<LiveNode> list = m_resultNodes.nodes();
	int count=1;
	for(QList<LiveNode>::iterator i = list.begin(); i != list.end(); ++i) {
		QCOMPARE( Live<nie::InformationElement>(*i)->getTitle(), QString("Title ")+QString::number(count) );
		count++;
	}

}

void MafwSuite::testBrowseEncoding()
{
	// case A) browse from browse result set, encoded character

	// preamble

	m_dummyResource->resetNextBrowseResult();
	QMap<QString,QVariant> metamap;
	metamap.insert( nie::mimeType::iri().toString(), "image/jpeg" );
	m_dummyResource->addNextBrowseResult( "obj%201*", metamap ); // have special character

	QSharedPointer<RDFService> service = SopranoLive::BackEnds::Mafw::getCachedRDFService(*m_dummyResource);
	Live<nie::InformationElement> rootNode = SopranoLive::BackEnds::Mafw::rootNode(service);

	// body

	resetSignalStatus();
	m_resultNodes = rootNode->getHasLogicalParts();
	connect( m_resultNodes.model(), SIGNAL( modelUpdated() ), this, SLOT( modelUpdatedSlot() ) );

	QTest::qWait(200);
	QCOMPARE( m_modelUpdatedSignals, 1 );

	QCOMPARE( m_resultNodes.nodes().size(), 1 );
	QCOMPARE( m_resultNodes.model()->data( m_resultNodes.model()->index(0, 0) ).toString(), QString("obj 1*" ) );
	
	Live<nie::InformationElement>( m_resultNodes.nodes()[0] )->getMimeType();

	QCOMPARE( m_dummyResource->getLastCall()[1].toString(), QString("obj%201*") );

	// case B) browse from query result set, reserved character
	
	// prepare query

	RDFSelect select;
	RDFVariable media=RDFVariable::fromType<nfo::Media>();
	select.addColumn("node", media);

	// prepare result

	QList<QStringList> resultRows;
	QStringList row1;
	row1 << "obj$1%24";
	resultRows << row1;
	m_dummyResource->setNextQueryResult( resultRows );

	// body

	resetSignalStatus();
	m_resultNodes = service->modelQuery(select);
	connect( m_resultNodes.model(), SIGNAL( modelUpdated() ), this, SLOT( modelUpdatedSlot() ) );

	QTest::qWait(200);
	QCOMPARE( m_modelUpdatedSignals, 1 );

	QCOMPARE( m_resultNodes.nodes().size(), 1 );
	QCOMPARE( m_resultNodes.model()->data( m_resultNodes.model()->index(0, 0) ).toString(), QString("obj$1$" ) );

	Live<nie::InformationElement>( m_resultNodes.nodes()[0] )->getMimeType();

	QCOMPARE( m_dummyResource->getLastCall()[1].toString(), QString("obj$1%24") );
}

void MafwSuite::testRandomCrash()
{
	RDFSelect select2;
	RDFVariable media=RDFVariable::fromType<nie::DataObject>();
	RDFVariable title=media.optional().property<nie::title>();
	select2.addColumn("node", media);
	select2.addColumn("title", title);

	// prepare result

	QList<QStringList> resultRows;
	QStringList row1, row2, row3, row4;
	row1 << "file://musa/biisi1.wav" << "Biisi 1";
	row2 << "file://musa/biisi2.wav" << "Biisi 2";
	row3 << "file://musa/biisi3.wav" << "Biisi 3";
	row4 << "file://musa/biisi4.wav" << "Biisi 4";
	resultRows << row1 << row2 << row3 << row4;
	m_dummyResource->setNextQueryResult( resultRows );

	// body

	m_resultNodes = SopranoLive::BackEnds::Mafw::getRDFService(*m_dummyResource)->modelQuery(select2);

	QTest::qWait(200);

	for( int i=0; i<2; i++ )
	{
		foreach(LiveNode node, m_resultNodes)
		{
			qDebug() << "testRandomCrash" << node.toString();
		}
		qDebug() << "size" << m_resultNodes.nodes().size();
	}

	m_resultNodes = LiveNodes();
}

void MafwSuite::testWrite()
{
	QSKIP("known to fail", SkipSingle);
	// preamble

	m_dummyResource->resetNextBrowseResult();
	QMap<QString,QVariant> metamap;
	metamap.insert( nie::title::iri().toString(), "Title 1" );
	metamap.insert( rdf::type::iri().toString(), "type1" );
	m_dummyResource->addNextBrowseResult( "obj1", metamap );

	// temp res for type query
	QList<QStringList> resultRows;
	QStringList row1, row2;
	row1 << "obj1" << "http://fancytype";
	resultRows << row1;
	m_dummyResource->setNextQueryResult( resultRows );

	QSharedPointer<RDFService> service = SopranoLive::BackEnds::Mafw::getCachedRDFService(*m_dummyResource);
	//Live<nie::InformationElement> rootIE = SopranoLive::BackEnds::Mafw::rootNode(service);
	LiveNode rootNode = SopranoLive::BackEnds::Mafw::rootNode(service);
	Live<nie::InformationElement> rootIE( rootNode, true );

	RDFVariable logicalparts_info;
	logicalparts_info.addDerivedObject<nie::title>();
	logicalparts_info.addDerivedObject<rdf::type>();
	m_resultNodes = rootIE->getHasLogicalParts(logicalparts_info);

	QTest::qWait(200);

	QVERIFY( m_resultNodes.nodes().size()==1 );

	// body

	Live<nie::InformationElement> nodeIE( m_resultNodes.nodes()[0], true );
	QCOMPARE( nodeIE->getTitle(), QString("Title 1") );
	nodeIE->setTitle( "new title" );
	QCOMPARE( nodeIE->getTitle(), QString("new title") );

	// with transaction
	RDFTransactionPtr transu=service->createTransaction();
	nodeIE->setTitle( "new title 2" );
	transu->commit();
	QCOMPARE( nodeIE->getTitle(), QString("new title 2") );
}

void MafwSuite::testAbortQuery()
{
	// preamble

	RDFSelect select2;
	RDFVariable media=RDFVariable::fromType<nie::DataObject>();
	RDFVariable title=media.optional().property<nie::title>();
	select2.addColumn("node", media);
	select2.addColumn("title", title);


	QList<QStringList> resultRows;
	QStringList row1, row2, row3, row4;
	row1 << "file://musa/biisi1.wav" << "Biisi 1";
	row2 << "file://musa/biisi2.wav" << "Biisi 2";
	row3 << "file://musa/biisi3.wav" << "Biisi 3";
	row4 << "file://musa/biisi4.wav" << "Biisi 4";
	resultRows << row1 << row2 << row3 << row4;
	m_dummyResource->setNextQueryResult( resultRows );

	// body: implicit case

	m_resultNodes = SopranoLive::BackEnds::Mafw::getRDFService(*m_dummyResource)->modelQuery(select2, LiveNodeModel::Streaming);

	m_resultNodes = LiveNodes(); // when application dereferences LiveNodes query shoud get aborted implicitly
	QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);

	QVERIFY( m_dummyResource->isCancelled() );

	// body: explicit case
	
	m_resultNodes = SopranoLive::BackEnds::Mafw::getRDFService(*m_dummyResource)->modelQuery(select2, LiveNodeModel::Streaming);

	QSignalSpy spy(m_resultNodes.model(), SIGNAL(modelUpdated()) );

	m_resultNodes.model()->stopOperations(); // explicit abort

	QVERIFY( m_dummyResource->isCancelled() );

	QTest::qWait(200);
	QCOMPARE( spy.size(), 1 );
}

void MafwSuite::testAbortBrowse()
{
	// preamble

	m_dummyResource->resetNextBrowseResult();
	QMap<QString,QVariant> metamap;
	metamap.insert( nie::title::iri().toString(), "Title 1" );
	m_dummyResource->addNextBrowseResult( "obj1", metamap );
	metamap.clear();
	metamap.insert( nie::title::iri().toString(), "Title 2" );
	m_dummyResource->addNextBrowseResult( "obj$2", metamap ); // have special character

	// body: implicit case

	QSharedPointer<RDFService> service = SopranoLive::BackEnds::Mafw::getCachedRDFService(*m_dummyResource);
	LiveNode rootNode = SopranoLive::BackEnds::Mafw::rootNode(service);
	Live<nie::InformationElement> ie( rootNode, true );

	RDFVariable logicalparts_info;
	logicalparts_info.addDerivedObject<nie::title>();
	m_resultNodes = ie->getHasLogicalParts(logicalparts_info);

	m_resultNodes = LiveNodes(); // when application dereferences LiveNodes query shoud get aborted implicitly
	QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);

	QVERIFY( m_dummyResource->isCancelled() );

	// body: explicit case

	m_resultNodes = rootNode->getObjects<nie::hasLogicalPart>(logicalparts_info, LiveNodeModel::RequireStreaming|LiveNodeModel::RequireCached); // browse in streaming mode

	QSignalSpy spy(m_resultNodes.model(), SIGNAL(modelUpdated()) );

	m_resultNodes.model()->stopOperations(); // explicit abort

	QVERIFY( m_dummyResource->isCancelled() );

	QTest::qWait(200);
	QCOMPARE( spy.size(), 1 );
}

void MafwSuite::testCreateLiveNode()
{
	QSharedPointer<RDFService> service = SopranoLive::BackEnds::Mafw::getCachedRDFService(*m_dummyResource);
	LiveNode liveNode = service->liveNode(LiveNode(QUrl("objectId")));
	QCOMPARE( liveNode.iri().toString(), QString("objectId") );
	QVERIFY( liveNode.graph()==service );
}

void MafwSuite::testLiveNodeStrange()
{
	// preamble

	testBasicQuery();

	// body

	QVERIFY(m_resultNodes->rowCount() >= 1);

	LiveNode node=m_resultNodes.nodes()[1];

	int idx=m_resultNodes.nodes().indexOf(node);
	QCOMPARE( idx, 1 );

	idx=m_resultNodes.nodes().indexOf(m_resultNodes.nodes()[1]);
	QCOMPARE( idx, 1 );
}

void MafwSuite::cleanupTestCase()
{
	delete m_dummyResource;
	m_resultNodes = LiveNodes();

	while(QCoreApplication::hasPendingEvents())
	{
		QCoreApplication::sendPostedEvents();
		QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
		QCoreApplication::processEvents();
	}

}

/*
void FirstTest::doubleClickedBrowse ( const QModelIndex & index )
{
	QVariant node = index.sibling( index.row(), 0 ).data(LiveNodeModel::LiveNodeRole);
	LiveNode liveNode=node.value<LiveNode>();
	qDebug() << SopranoLive::BackEnds::Mafw::getObjectId(liveNode);

	Live<nie::InformationElement> ie = liveNode;

	RDFVariable logicalparts_info;
	logicalparts_info.addDerivedObject<nie::title>();
	//logicalparts_info.addDerivedObject<rdf::type>();
	//logicalparts_info.addDerivedObject<nie::mimeType>();

	m_query_model = ie->getHasLogicalParts(logicalparts_info);
	m_view->setModel(m_query_model.model());
}
*/

void MafwSuite::resetSignalStatus()
{
	m_dataChangedSignals = 0;
	m_rowsInsertedSignals = 0;
	m_rowsInsertedSignalStarts.clear();
	m_rowsInsertedSignalEnds.clear();
	m_modelUpdatedSignals = 0;
	m_modelErrorSignals = 0;
}

void MafwSuite::dataChangedSlot(const QModelIndex& tl, const QModelIndex& br)
{
	Q_UNUSED( tl );
	Q_UNUSED( br );
	qDebug() << "MafwSuite::dataChanged";

	m_dataChangedSignals++;
}

void MafwSuite::rowsInsertedSlot( const QModelIndex & parent, int start, int end )
{
	Q_UNUSED( parent );
	qDebug() << "MafwSuite::rowsInserted" << start << end;

	m_rowsInsertedSignals++;
	m_rowsInsertedSignalStarts.append(start);
	m_rowsInsertedSignalEnds.append(end);
}

void MafwSuite::modelUpdatedSlot()
{
	qDebug() << "MafwSuite::modelUpdatedSlot";

	m_modelUpdatedSignals++;
}

void MafwSuite::modelErrorSlot(QString const &message, RDFStrategyFlags mask, RDFStrategyFlags flags, QModelIndex const &index)
{
	Q_UNUSED(message); Q_UNUSED(mask); Q_UNUSED(flags); Q_UNUSED(index);
	qDebug() << "MafwSuite::modelErrorSlot";

	m_modelErrorSignals++;
}

QTEST_MAIN(MafwSuite)

