#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nmm.h>
#include <QtTracker/ontologies/nmo.h>
#include <QtMafw/QMafw>
#include <QtMafw/dummyresource.h>
#include <QtTest/QtTest>

using SopranoLive::RDFStrategyFlags;

class MafwSuite : public QObject
{
	Q_OBJECT;

public:
	MafwSuite();
private slots: // tests
	void initTestCase();
	void testCreation();
	void testBasicQuery();
	void testBasicQueryResultNodes();
	void testComplexQuery();
	void testQueryFailure();
	void testQueryImmediateFailure();
	void testBrowseTypes();
	void testBrowseLogicalParts();
	void testBrowseTypesCached();
	void testBrowseAsynch();
	void testBrowseEncoding();
	void testRandomCrash();
	void testWrite();
	void testAbortQuery();
	void testAbortBrowse();
	void testCreateLiveNode();
	void testLiveNodeStrange();
	void cleanupTestCase();
protected slots:
	//void doubleClickedBrowse ( const QModelIndex & index );
	void dataChangedSlot(const QModelIndex& tl, const QModelIndex& br);
	void rowsInsertedSlot( const QModelIndex & parent, int start, int end );
	void modelUpdatedSlot();
	void modelErrorSlot(QString const &message, RDFStrategyFlags mask, RDFStrategyFlags flags, QModelIndex const &index);

private:
	void resetSignalStatus();

	SopranoLive::BackEnds::Mafw::DummyResource* m_dummyResource;

	SopranoLive::LiveNodes m_resultNodes;

	// signal monitoring
	int m_dataChangedSignals;
	int m_rowsInsertedSignals;
	QList<int> m_rowsInsertedSignalStarts;
	QList<int> m_rowsInsertedSignalEnds;
	int m_modelUpdatedSignals;
	int m_modelErrorSignals;
	/*
	QListView* m_view;

	QWidget* m_window;
	QVBoxLayout* m_layout;*/
};
