#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QFile>
#include <QTextStream>

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nco.h>
#include <QtTracker/ontologies/nie.h>
#include <QtTracker/ontologies/nmo.h>

using namespace SopranoLive;

int main (int argc, char** argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    ::tracker()->setVerbosity(1);
	::tracker()->createLiveResource<nco::PersonContact>()->setContactUID("Foobar");

    RDFVariable RDFContact = RDFVariable::fromType<nco::PersonContact>();
    RDFSelect query;

    query.addColumn("contact_uri", RDFContact);
    query.addColumn("contactId", RDFContact.property<nco::contactUID>());
    LiveNodes ncoContacts = ::tracker()->modelQuery(query);
    foreach( LiveNode p, ncoContacts ) {

        qDebug() << "Contact " << p.uri();

if( p.hasType<nco::PersonContact>() ) qDebug() << " -hasType PersonContact";
if( StrictLive<nco::PersonContact> person = p ) qDebug() << person->getContactUID();
if( StrictLive<nmo::Email> email = p ) qDebug() << email->getMessageSubject(); else qDebug() << " -not an email";
if( p.hasType<nco::Contact>() ) qDebug() << " -hasType Contact";
if( p.hasType<nco::Role>() ) qDebug() << " -hasType Role";
    }
}
