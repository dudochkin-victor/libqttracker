#include <QtDBus>

Q_DECLARE_METATYPE(QVector<QStringList>)

int main(int argc, char *argv[])
{
	if (argc < 2)
		return 1;

	qRegisterMetaType<QVector<QStringList> >();
	qDBusRegisterMetaType<QVector<QStringList> >();
	QDBusInterface tracker(QLatin1String("org.freedesktop.Tracker1"),
						   QLatin1String("/org/freedesktop/Tracker1/Resources"),
						   QLatin1String("org.freedesktop.Tracker1.Resources"));
	QVariantList args;
	args << QString::fromLatin1(argv[1]);
	QDBusMessage m = tracker.callWithArgumentList(QDBus::Block, "SparqlQuery", args);
	if (m.type() != QDBusMessage::ReplyMessage)
		return 2;
    QDBusReply<QVector<QStringList> > r(m);
    qDebug() << r.value();
	return 0;
}
