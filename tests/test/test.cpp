#include <QtTest>
#include <QObject>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QtTracker/Tracker>
/*
#include <QtTracker/ontologies/nmm.h>
#include <QtTracker/ontologies/nmo.h>
#include <QtTracker/ontologies/nfo.h>
#include <QtTracker/ontologies/nco.h>
#include <QtTracker/ontologies/mto.h>
*/
#include <QtTracker/ontologies/mfo.h>
#include <QtTracker/ontologies/tracker.h>
#define VISUALIZER_ENABLE
#include "../../src/utils/visualizer.h"
#include "../../src/utils/visualizer_p.cpp"
#include "../../src/lnm_transitionstrategy_p.h"

using namespace SopranoLive;
using namespace SopranoLive::Ontologies;


struct LiveNodeModelDebugger
	: public QObject
{
	Q_OBJECT

public:
	LiveNodes model_;

	LiveNodeModelDebugger(LiveNodes model)
		: model_(model)
	{
		connect(model_.model(), SIGNAL(modelUpdated()), this, SLOT(modelUpdated()));
		connect(model_.model(), SIGNAL(derivedsUpdated()), this, SLOT(derivedsUpdated()));

		connect(model_.model(), SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int))
				, this, SLOT(rowsAboutToBeInserted(const QModelIndex &, int, int)));
		connect(model_.model(), SIGNAL(rowsInserted(const QModelIndex &, int, int))
				, this, SLOT(rowsInserted(const QModelIndex &, int, int)));

		connect(model_.model(), SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int))
				, this, SLOT(rowsAboutToBeRemoved(const QModelIndex &, int, int)));
		connect(model_.model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int))
				, this, SLOT(rowsRemoved(const QModelIndex &, int, int)));

		connect(model_.model(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &))
				, this, SLOT(dataChanged(const QModelIndex &, const QModelIndex &)));
	}

	void waitForModelUpdated(bool derived_update = true)
	{
		QSignalSpy spy(model_.model()
				, derived_update ? SIGNAL(derivedsUpdated()) : SIGNAL(modelUpdated()));
		while (QCoreApplication::hasPendingEvents() ||
			   spy.count() < 1)
		{
			QCoreApplication::sendPostedEvents();
			QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
			QCoreApplication::processEvents();
		}
	}

public slots:

	void modelUpdated()
	{
		qDebug() << "Model LNM(" << model_.model() << ") updated:";
		qDebug() << *model_.model();
	}
	void derivedsUpdated()
	{
		qDebug() << "Model LNM(" << model_.model() << ") deriveds updated";
		qDebug() << *model_.model();
	}

	void rowsAboutToBeInserted(const QModelIndex &, int start, int end)
	{
		qDebug() << "Model LNM(" << model_.model() << ") rows about to be inserted: "
				<< start << end;
	}

	void rowsInserted(const QModelIndex &, int start, int end)
	{
		qDebug() << "Model LNM(" << model_.model() << ") rows inserted: "
				<< start << end;
	}

	void rowsAboutToBeRemoved(const QModelIndex &, int start, int end)
	{
		qDebug() << "Model LNM(" << model_.model() << ") rows about to be removed: "
				<< start << end;
	}

	void rowsRemoved(const QModelIndex &, int start, int end)
	{
		qDebug() << "Model LNM(" << model_.model() << ") rows inserted: "
				<< start << end;
	}

	void dataChanged(const QModelIndex &topleft_inc, const QModelIndex &botright_inc)
	{
		qDebug() << "Model LNM(" << model_.model() << ") data changed: "
				<< topleft_inc << botright_inc;
	}

};

struct Test
	: public QObject
{
	Q_OBJECT

public:
	bool simple_signal_flag;
	Test() : simple_signal_flag(false) {}

public slots:
	void ready()
	{
		simple_signal_flag = true;
		qDebug() << "ready";
	}

	void test()
	{
		qDebug() << "entering test";
/*
		LiveNodes res = ::tracker()->modelVariable(RDFVariable::fromType<nmo::Email>());
		QObject::connect(res.model(), SIGNAL(modelUpdated()), this, SLOT(ready()));

		QTime timer;
		timer.start();
		int msec_left = 0;

		while((msec_left = 5000 - timer.elapsed()) > 0 && !simple_signal_flag)
			QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);

		if(!simple_signal_flag)
			qDebug() << "timeouted";

*/
		qDebug() << "exiting test";
	}
};

struct O {};
struct A : O
{
	  VZRX_CLASS(A, (O)); 
VZR_MEMBER_M_T_CV_N_C(,	int,, a, O); // VZRX_MEMBER 

};
struct B
{
	  VZRX_CLASS(B); 
VZR_MEMBER_M_T_CV_N_C(,	int,, b, O); // VZRX_MEMBER 

};

template<typename T>
struct D
{
	  VZRX_CLASS(D); 
};

template<typename Compositor_>
struct Interfaces
{
  VZRX_CLASS(Interfaces); 

Interfaces() {}
Interfaces(Interfaces const &)  {}

};

struct C : A, B, D<int>
{
	  VZRX_CLASS(C, (A), (B), (D<int>)); 
VZR_MEMBER_M_T_CV_N_C(,	B,, owned_b, &Relation=Owns); // VZRX_MEMBER 

VZR_MEMBER_M_T_CV_N_C(,	B * ,,pointed_b, &Relation=Refers); // VZRX_MEMBER 
VZR_MEMBER_M_T_CV_N_C(,	QSharedPointer<B> ,,shared_b, &Relation=Shares); // VZRX_MEMBER 

};

namespace SopranoLive
{
	int oro();
}

struct Bar;

void bar(Bar);

int main(int argc, char *argv[])
{
	RDFUpdate query;
	RDFVariable imContact(QUrl("ororo:1"));

	query.addDeletion(imContact, nco::hasAffiliation::iri(), RDFVariable(), QUrl("graph:1"));

	qDebug() << ::tracker()->rawQueryString(query);

	return 0;
#if 0

	QCoreApplication app(argc, argv);

	B b[2];
	QSharedPointer<B> sb = QSharedPointer<B>(new B);
	C c;
	c.pointed_b = &b[0];
	++c.pointed_b;
	c.shared_b = sb;
	return 0;


	RDFSelect sel;
	RDFVariable feed_channel = sel.newColumn<mfo::FeedChannel>();
	RDFPattern exists_pattern;
	exists_pattern
			.variable(feed_channel)
			.subject<nie::hasLogicalPart>().isOfType<mfo::FeedMessage>()
			.subject<nie::hasLogicalPart>().isOfType<mfo::Enclosure>()
			.property<mfo::optional>(LiteralValue(true));
	sel.addColumn(exists_pattern.exists());

	qDebug() << sel.getQuery();

	return 0;
	setVerbosity(5);

	RDFVariable var;
	var.pattern().useFilter(LiteralValue(false));
	qDebug() << RDFSelect().addColumn(var).getQuery();

	return 0;


	RDFVariable emails = RDFVariable::fromType<nmo::Email>("emails");
	RDFVariable recipients = emails.property<nmo::to>(RDFVariable("recipients"));

	RDFSelect primary_select;
	primary_select.addColumn(emails).addColumn(recipients);


	RDFSelect by_me = primary_select;
	by_me.variable(RDFVariableLink(emails)) == nco::default_contact_me::iri();


	RDFSelect by_mailing_list = primary_select;
	by_mailing_list.variable(RDFVariableLink(emails)).property<nie::title>().contains("[list]");

	qDebug() << by_me.getQuery();
	qDebug() << by_mailing_list.getQuery();

	return 0;

	RDFSelect sel;
	RDFVariable var = sel.newColumn("col");
	var.isOfType<nmo::Email>();

	RDFPattern top;
	RDFPattern exists_pattern = top.child();
	exists_pattern.exists();
	RDFVariable link_var = exists_pattern.variable(RDFPattern().variable(RDFVariableLink(var)));

	link_var.isOfType<mfo::FeedMessage>();
	sel.variable(RDFVariable(top.variable(link_var)));

	qDebug() << sel.getQuery();


	return 0;
    RDFSelect query;

    // bind a contact
////////////////////////////////////////////////////////////////////////////
    RDFVariable contact(RDFVariable::fromType<nco::PersonContact>());
    RDFVariable contactId(contact.function<nco::contactLocalUID>());
    RDFPattern group(contact.pattern().child());
    query.addColumn("contact", contact);
    query.addColumn("cid", contactId);

    // bind data column
//////////////////////////////////////////////////////////////////////////
    RDFVariable entity("PhoneNumber");
    query.addColumn(entity);

    RDFVariable value("PhoneNumber_PhoneNumber");
    query.addColumn(value);

    // bind subtype column
///////////////////////////////////////////////////////////////////////

    RDFSubSelect subQuery = query.pattern().subQuery();

    subQuery.addColumn("", // explicitly create anonymous variable

subQuery.pattern().variable(query.pattern().variable(entity)).

property(rdfs::type::iri()).function(tracker::iri("id")).
                       filter("GROUP_CONCAT", LiteralValue(":")));

    query.addColumn("PhoneNumber_SubTypes", subQuery.asExpression());

    // bind home properties
//////////////////////////////////////////////////////////////////////

group.variable(contact).property<nco::hasPhoneNumber>(group.variable(entity));
    group.variable(entity).property<nco::phoneNumber>(group.variable(value));

    // finish current union group
////////////////////////////////////////////////////////////////
    group = group.union_();

    // bind work properties
//////////////////////////////////////////////////////////////////////
    RDFVariable
affiliation(group.variable(contact).property<nco::hasAffiliation>());
    query.addColumn("affiliation_IsBound", affiliation.isBound());
    affiliation.metaSetIdentifier("affiliation");


group.variable(affiliation).property<nco::hasPhoneNumber>(group.variable(entity));
    group.variable(entity).property<nco::phoneNumber>(group.variable(value));

    qDebug() << query.getQuery();

    return 0;
	QSharedPointer<B> sb;
	qDebug() << sb.staticCast<C>().data();

	//QSKIP("needs libqttracker streaming fix", SkipSingle);

	::tracker()->setVerbosity(5);

	const int normalChunkSize = 4;
	const int firstChunkSize = 2;

	::tracker()->setServiceAttribute("streaming_first_block_size", QVariant(firstChunkSize));
	::tracker()->setServiceAttribute("streaming_block_size", QVariant(normalChunkSize));

	LiveNodes model = ::tracker()->modelVariable
			(RDFVariable::fromType<nmm::Photo>());
	LiveNodes streamModel = ::tracker()->modelVariable
			(RDFVariable::fromType<nmm::Photo>(), RDFStrategy::StreamingStrategy);

	qRegisterMetaType<QModelIndex>("QModelIndex");
	QSignalSpy rowsInserted(streamModel.model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)));
	QSignalSpy modelReady(streamModel.model(), SIGNAL(modelUpdated()));

	QTime timer;
	int count = 0;
	int chunkSize = firstChunkSize;
	int total = model->rowCount();
	qDebug() << "total: " << total;
	while (count < total) {
		qDebug() << "Count: " << count;
		if (count > 0)
			chunkSize = normalChunkSize;

		qDebug() << "chunk size is " << chunkSize;
		timer.start();
		while (timer.elapsed() < 5000 && rowsInserted.isEmpty())
			QCoreApplication::processEvents();

		QList<QVariant> args = rowsInserted.takeFirst();
		qDebug() << "start index from rowsInserted signal is " << args.at(1).toInt();
		qDebug() << "start index from count is " << count;
		qDebug() << "end index from rowsInserted signal is " << args.at(2).toInt();
		qDebug() << "end index from count is " << count + chunkSize - 1;

		streamModel->fetchMore(QModelIndex());
		count += chunkSize;
	}
	timer.start();
	while (timer.elapsed() < 5000 && modelReady.isEmpty())
		QCoreApplication::processEvents();

	return 0;

    RDFSelect query;
    query.distinct (true);

    RDFVariable glob_url ("glob_url");

    RDFVariableList alternatives = glob_url.unionChildren (2);

    /* ---- */

    RDFSubSelect ua_query;
    alternatives[0].pattern().child(ua_query.pattern());
    RDFVariable ua_url = ua_query.variable (alternatives[0]);

    ua_url.isOfType <nmo::Message> ();
    ua_url.matchesRegexp (QString ("fami*"));

    ua_query.addColumn (ua_url);

    /* ---- */

    RDFSubSelect ub_query;
    alternatives[1].pattern().child(ub_query.pattern());
    RDFVariable ub_url = ub_query.variable (alternatives[1]);
    RDFVariable ub_from ("from");

    ub_url.isOfType <nmo::Message> ();
    ub_url.property <nmo::from> (ub_from);
    ub_from.matchesRegexp (QString ("fami*"));

    ub_query.addColumn (ub_url);

    /* ---- */

    query.addColumn (glob_url);
    query.limit (100);

    qDebug() << query.getQuery();
    return 0;

    QString my_uuid("urn:uuid:55322");
	::tracker()->setVerbosity(5);
	QString path = ::tracker()->firstNode(RDFVariable(QUrl(my_uuid)).property<nie::url>()).toString();

	RDFSelect s1, s2;
	s1.addColumn("foo", LiteralValue(0));
	s2.addColumn("bar", RDFVariable(QUrl("foo:1")).isOfType<nmo::Email>());
	qDebug() << s1.getQuery();
	qDebug() << s2.getQuery();

	::tracker()->modelQuery(s1);
	::tracker()->modelQuery(s2);

	return 0;

	/*!
	 * write all phone numbers on one query to tracker
	 * TODO this is temporary code for creating new, saving contacts need to handle only what was
	 * changed.
	 */
	RDFServicePtr service = ::tracker();

	RDFTransactionPtr tx = service->createTransaction();

	RDFUpdate up;

	::tracker()->setVerbosity(5);
	service->executeQuery(RDFUpdate());

	tx->commit();

	return 0;

    RDFVariable album = RDFVariable::fromType<nmm::MusicAlbum>();
    RDFVariable albumTitle = album.optional().property<nmm::albumTitle>();

    RDFVariable song = album.subject<nmm::musicAlbum>();

    RDFVariable artist = song.optional().property<nmm::performer>();

    RDFVariable artistTitle = artist.optional().property<nmm::artistName>();

    RDFSelect select;
    select.addColumn("album title", albumTitle);
    select.addColumn("artist title", artistTitle);
    select.addCountColumn("album track count", song);

    RDFSubSelect subSelect;
    RDFVariable inner_song = subSelect.variable(album).subject<nmm::musicAlbum>();
    RDFVariable inner_artist = inner_song.function<nmm::performer>();
    subSelect.addCountDistinctColumnAs(inner_artist, RDFVariable());
    subSelect.groupBy(album);
    select.addColumn("artist count", subSelect.asExpression());

    select.addColumn("album", album);

    select.groupBy(album);
    select.orderBy(albumTitle);

    select.distinct(true);

    qDebug() << select.getQuery();

	RDFVariable channels = RDFVariable::fromType<mfo::FeedChannel>("channel");

	channels.addDerivedObject<nie::title>();
	channels.addDerivedObject<mfo::image>();
	channels.addDerivedObject<nie::contentLastModified>();

	RDFVariable channelLink = channels.optional().property<nie::links>(RDFVariable("links"));
	RDFVariable channelTitle = channels.optional().property<nie::title>(RDFVariable("title"));
	RDFVariable channelDescription = channels.optional().property<nie::description>(RDFVariable("description"));
	RDFVariable channelImage = channels.optional().property<mfo::image>(RDFVariable("image"));
	RDFVariable channelLastModified = channels.optional().property<nie::contentLastModified>(RDFVariable("last modified"));
	RDFVariable channelSubscribedTime = channels.optional().property<nie::created>(RDFVariable("created"));

	RDFSelect sel;
	sel.addColumn(channels);
	sel.addColumn(channelLink);
	sel.addColumn(channelTitle);
	sel.addColumn(channelDescription);
	sel.addColumn(channelImage);
	sel.addColumn(channelLastModified);
	sel.addColumn(channelSubscribedTime);

	::tracker()->setVerbosity(5);

	RDFServicePtr backend = ::tracker();

	backend->setServiceAttribute("streaming_first_block_size", QVariant(100));

	LiveNodeModelDebugger md(backend->modelQuery
			(sel, RDFStrategy::Live | RDFStrategy::StreamingStrategy));

	md.waitForModelUpdated();

	RDFTransactionPtr tx = ::tracker()->createTransaction();
	Live<mfo::FeedChannel> new_chan = ::tracker()->createLiveResource<mfo::FeedChannel>();
	new_chan->setTitle("title");
	tx->commit();

	md.waitForModelUpdated();

	return 0;
#endif

}
//#include "test.moc"
