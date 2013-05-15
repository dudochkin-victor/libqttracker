#include <QApplication>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QFile>
#include <QTextStream>

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nfo.h>

#include "classupdatesignaler.h"

using namespace SopranoLive;
using namespace SopranoLive::BackEnds;

void ClassUpdateSignalerTest::subjectsAdded(const QStringList &subjects)
{
	qDebug() << "Signal: Added subjects:" << subjects;
}

void ClassUpdateSignalerTest::subjectsRemoved(const QStringList &subjects)
{
	qDebug() << "Signal: Removed subjects:" << subjects;
}

void ClassUpdateSignalerTest::subjectsChanged(const QStringList &subjects)
{
	qDebug() << "Signal: Changed subjects:" << subjects;
}


int main(int argc, char** argv)
{
	QApplication main(argc, argv);

	QUrl class_id;
	QUrl property_id;

	if(argc > 1)
	{
		if(argv[1] == QString("-h"))
		{
			qDebug() << "Usage: classupdatesignaler [ns:type] [ns:property]";
			qDebug() << "If no parameters are given, classupdatesignaler nfo:Image nfo:Orientation is used by default";
			qDebug() << "Range of the property should be a resource type, and not a literal value.";
			return 0;
		}
		QStringList class_string = QString(argv[1]).split(":");
		if(class_string.size() != 2)
		{
			qWarning() << "Invalid type supplied, use the form ns:type, like \"nfo:Image\"";
			return 1;
		}
		class_id = ::tracker()->prefixSuffix(class_string[0], class_string[1]);
		qDebug() << "Using" << class_id.toString().toAscii().data() << "as the type";
		if(argc > 2)
		{
			QStringList property_string = QString(argv[2]).split(":");
			if(property_string.size() != 2)
			{
				qWarning() << "Invalid property supplied, use the form ns:property, like \"nfo:Orientation\"";
				return 1;
			}
			property_id = ::tracker()->prefixSuffix(property_string[0], property_string[1]);
			qDebug() << "Using" << property_id.toString().toAscii().data() << "as the property";
		} else
			qDebug() << "No property supplied";
	} else
	{
		class_id = nfo::Image::iri();
		property_id = nfo::Orientation::iri();
		qDebug() << "classupdatesignaler -h for usage.";
		qDebug() << "Using nfo:Image as the type";
		qDebug() << "Using nfo:Orientation as the property";
	}

	qDebug() << "";

	ClassUpdateSignalerTest test;

	Tracker::ClassUpdateSignaler *signaler = Tracker::ClassUpdateSignaler::get(class_id);

	QObject::connect( signaler, SIGNAL(subjectsAdded(const QStringList &))
	                , &test, SLOT(subjectsAdded(const QStringList &)));
	QObject::connect( signaler, SIGNAL(subjectsRemoved(const QStringList &))
	                , &test, SLOT(subjectsRemoved(const QStringList &)));
	QObject::connect( signaler, SIGNAL(subjectsChanged(const QStringList &))
	                , &test, SLOT(subjectsChanged(const QStringList &)));

	LiveNode item = ::tracker()->createLiveNode();
	item->addObject(rdf::type::iri(), class_id);
	qDebug() << "Added" << item.toString().toAscii().data() << "of type" << class_id.toString().toAscii().data();
	if(property_id.isValid())
	{
		LiveNode property_value = ::tracker()->createLiveNode();
		item->addObject(property_id, property_value);
		qDebug() << "Added" << item.toString().toAscii().data() << property_id.toString().toAscii().data() << property_value.toString().toAscii().data();
		property_value->remove();
		qDebug() << "Removed" << property_value.toString().toAscii().data();
	}
	item->remove();
	qDebug() << "Removed" << item.toString().toAscii().data();

	qDebug() << "\nEntering event loop, below should appear three signal notifications. Kill application manually with ctrl-c once done. \n";

	main.exec();

    return 0;
}
