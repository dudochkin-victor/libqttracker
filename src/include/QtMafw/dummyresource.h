/*
 * DummyResource.h
 *
 *  Created on: Sep 28, 2009
 *      Author: iridian
 */

#ifndef SOPRANOLIVE_QTMAFW_DUMMYRESOURCE_H_
#define SOPRANOLIVE_QTMAFW_DUMMYRESOURCE_H_

#include "mafw.h"

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Mafw
		{
			const QString FAILURE_TOKEN="FAILURE";
			const QString FAILURE_TOKEN_IMMEDIATE="IMMEDIATE_FAILURE";

			typedef QMap<QString,QVariant> MetaMap;

			class Q_DECL_EXPORT DummyResource : public QObject, public MafwRdfResource
			{
				Q_OBJECT

			public:
				DummyResource() : m_cancelled(false) {}

				QString uuid() const { return "dummy"; }

				uint query(QString query, QObject* receiver, const char* resultSlot,
						   const char* errorSlot);
				void cancelQuery( uint queryId );

				uint browse( const QString objectId, const QString predicate, const QStringList additionalPredicates,
							 uint skipCount, uint itemCount,
							 QObject* receiver, const char* resultSlot,
							 const char* errorSlot );
				void cancelBrowse( uint browseId );

				void setNextQueryResult( QList<QStringList> rows );
				void addNextBrowseResult( QString id, MetaMap metamap );
				void resetNextBrowseResult();
				bool isCancelled();
				QList<QVariant> getLastCall();

			private slots:
				void asyncQueryResult();
				void asyncBrowseResult();

			private:
				bool getCallbackMethod( QPointer<QObject> targetObject, const char* slotSignature, QMetaMethod& methodOut );

				QPointer<QObject> m_queryReceiver;
				QString m_queryResultSlot;
				QString m_queryErrorSlot;

				QPointer<QObject> m_browseReceiver;
				QString m_browseResultSlot;
				QString m_browseErrorSlot;

				QList<QStringList> m_nextQueryRows;
				QStringList m_nextBrowseObjIds;
				QList<MetaMap> m_nextBrowseMetamaps;
				bool m_cancelled;
				QList<QVariant> m_lastCall;
			};
		}
	}
}

#endif /* SOPRANOLIVE_MAFW_DummyResource_H_ */
