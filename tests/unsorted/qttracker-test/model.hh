#include <QObject>
#include <QStringList>
#include <QtTracker/Tracker>
#include <QTimer>
#include <QDebug>

class Model : public QObject
{

   Q_OBJECT

 public:

   Model () : QObject()
     {
	QTimer::singleShot(1000, this, SLOT(fetch()));
     }

      SopranoLive::LiveNodes m_model;

 public slots:

   void fetch ()
     {
	QStringList headings;
	headings << "1" << "2" << "3";
	QString query = "SELECT ?u ?title ?fileName WHERE { ?u a nfo:Software; nie:title ?title; nfo:fileName ?fileName . }";
	m_model = ::tracker()->modelQuery(query, headings);

	qDebug() << m_model.model()->rowCount()
	  << m_model.model()->columnCount();
     }

};
