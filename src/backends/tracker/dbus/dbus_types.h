#ifndef __TRACKERTYPES
#define __TRACKERTYPES

#include <QtCore/QMetaType>
#include <QtCore/QVector>
#include <QtCore/QStringList>
#include <QtCore/QMap>

Q_DECLARE_METATYPE(QVector<QStringList>)

/*
    Allows a metatype to be declared for a type containing commas.
    For example:
	Q_DECLARE_METATYPE_COMMA(QList<QPair<QByteArray,QByteArray> >)
*/
#define Q_DECLARE_METATYPE_COMMA(...)								\
	QT_BEGIN_NAMESPACE                                                  \
	template <>                                                         \
	struct QMetaTypeId< __VA_ARGS__ >                                   \
	{                                                                   \
		enum { Defined = 1 };                                           \
		static int qt_metatype_id()                                     \
		{																\
			static QBasicAtomicInt metatype_id = Q_BASIC_ATOMIC_INITIALIZER(0); \
			if (!metatype_id)											\
				metatype_id = qRegisterMetaType< __VA_ARGS__ >( #__VA_ARGS__ \
					, reinterpret_cast< __VA_ARGS__ *>(quintptr(-1)));  \
			return metatype_id;											\
		}																\
	};                                                                  \
	QT_END_NAMESPACE

Q_DECLARE_METATYPE_COMMA(QMap<QString, QString>)
Q_DECLARE_METATYPE_COMMA(QVector<QMap<QString, QString> >)
Q_DECLARE_METATYPE_COMMA(QVector<QVector<QMap<QString, QString> > >)

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Tracker
		{
			int registerQTrackerDbusTypes();
		}
	}
}

#endif
