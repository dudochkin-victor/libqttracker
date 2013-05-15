#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QFile>
#include <QTextStream>

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nco.h>

using namespace SopranoLive;

int main (int argc, char** argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    QString group_name = "test group";
	Live<nco::ContactGroup> groupNode = ::tracker()->createLiveNode();
	qDebug() << "New group object: " << groupNode->node().toString();
	groupNode->setContactGroupName(group_name);
	qDebug() << "Set group name to: " << group_name;
	qDebug() << "Retrieved the name: " << groupNode->getContactGroupName();

	QList<LiveNode> groupsList = ::tracker()->modelVariable(RDFVariable::fromType<nco::ContactGroup>());
	qDebug() << "Iterating all ContactGroup objects";
	foreach(Live<nco::ContactGroup> group, groupsList)
		qDebug() << "Id:" << group.toString() << ", name:" << group->getContactGroupName();

    qDebug() << "Done";
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
