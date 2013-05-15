/*
 * DummyResource_p.cpp
 *
 *  Created on: Sep 28, 2009
 *      Author: iridian
 */

#include "mafw_p.h"
#include "../../include/QtMafw/dummyresource.h"

namespace SopranoLive
{
	namespace BackEnds
	{
		namespace Mafw
		{
			const uint OPERATION_ID = 2;

			uint DummyResource::query(QString query, QObject* receiver, const char* resultSlot,
									  const char* errorSlot)
			{
				debug(2) << query;

				m_lastCall = QList<QVariant>() << "query" << query;

				m_queryReceiver = receiver;
				m_queryResultSlot = resultSlot;
				m_queryErrorSlot = errorSlot;
				m_cancelled = false;

				if( m_nextQueryRows.size() && m_nextQueryRows[0][0]==FAILURE_TOKEN_IMMEDIATE )
				{
					return 0;
				}

				QTimer::singleShot(100, this, SLOT(asyncQueryResult()));

				return OPERATION_ID;
			}

			void DummyResource::cancelQuery( uint queryId )
			{
				Q_UNUSED( queryId );
				m_cancelled = true;
			}

			uint DummyResource::browse( const QString objectId, const QString predicate, const QStringList additionalPredicates,
										uint skipCount, uint itemCount,
										QObject* receiver, const char* resultSlot,
										const char* errorSlot )
			{
				m_lastCall = QList<QVariant>() << "browse" << objectId << predicate << additionalPredicates << skipCount << itemCount;

				debug(2) << objectId << predicate << additionalPredicates;

				m_browseReceiver = receiver;
				m_browseResultSlot = resultSlot;
				m_browseErrorSlot = errorSlot;
				m_cancelled = false;

				QTimer::singleShot(100, this, SLOT(asyncBrowseResult()));

				return OPERATION_ID;
			}

			void DummyResource::cancelBrowse( uint browseId )
			{
				Q_UNUSED( browseId );
				m_cancelled = true;
			}

			void DummyResource::asyncQueryResult()
			{
				debug(2) << "asyncQueryResult";

				if( m_cancelled )
				{
					QMetaMethod method;
					bool errorMethodFound = getCallbackMethod( m_queryReceiver, m_queryErrorSlot.toAscii(), method );
					if ( !errorMethodFound || method.invoke( m_queryReceiver,
															 Q_ARG(uint, OPERATION_ID),
															 Q_ARG(int, 1),
															 Q_ARG(QString, "aborted") ) == false)
					{
						critical() << "asyncQueryResult error slot invocation failed!";
					}
					return;
				}

				QMetaMethod method;
				bool methodFound = getCallbackMethod( m_queryReceiver, m_queryResultSlot.toAscii(), method );

				for( int i=0; i<m_nextQueryRows.size(); i++ )
				{
					QStringList row=m_nextQueryRows[i];
					if( row[0]==FAILURE_TOKEN )
					{
						bool errorMethodFound = getCallbackMethod( m_queryReceiver, m_queryErrorSlot.toAscii(), method );
						if ( !errorMethodFound || method.invoke( m_queryReceiver,
																 Q_ARG(uint, OPERATION_ID),
																 Q_ARG(int, 1),
																 Q_ARG(QString, "erska") ) == false)
						{
							critical() << "asyncQueryResult error slot invocation failed!";
						}
					}
					else
					{
						if ( !methodFound || method.invoke( m_queryReceiver,
															Q_ARG(uint, OPERATION_ID),
															Q_ARG(QStringList, row),
															Q_ARG(uint, i),
															Q_ARG(int, m_nextQueryRows.size()-i-1) ) == false)
						{
							critical() << "asyncQueryResult slot invocation failed!";
						}
					}
				}
			}

			void DummyResource::asyncBrowseResult()
			{
				debug(2) << "DummyResource::asyncBrowseResult";

				if( m_cancelled )
				{
					QMetaMethod method;
					bool errorMethodFound = getCallbackMethod( m_browseReceiver, m_browseErrorSlot.toAscii(), method );
					if ( !errorMethodFound || method.invoke( m_browseReceiver,
															 Q_ARG(uint, OPERATION_ID),
															 Q_ARG(int, 1),
															 Q_ARG(QString, "aborted") ) == false)
					{
						critical() << "asyncBrowseResult error slot invocation failed!";
					}
					return;
				}

				QMetaMethod method;
				bool methodFound = getCallbackMethod( m_browseReceiver, m_browseResultSlot.toAscii(), method );

				for( int i=0; i<m_nextBrowseObjIds.size(); i++ )
				{
					debug(2) << "DummyResource::asyncBrowseResult row: object:"
							 << m_nextBrowseObjIds[i] << ", subprops:" << m_nextBrowseMetamaps[i];
					if ( !methodFound || method.invoke( m_browseReceiver,
														Q_ARG( uint, OPERATION_ID ),
														Q_ARG( QString, m_nextBrowseObjIds[i] ),
														Q_ARG( MetaMap, m_nextBrowseMetamaps[i] ),
														Q_ARG( uint, i ),
														Q_ARG( int, m_nextBrowseObjIds.size()-i-1 ))==FALSE )
					{
						critical() << "asyncBrowseResult slot invocation failed!";
					}
				}
			}

			void DummyResource::setNextQueryResult( QList<QStringList> rows ) { m_nextQueryRows = rows; }

			void DummyResource::addNextBrowseResult( QString id, MetaMap metamap ) { m_nextBrowseObjIds.append( id ); m_nextBrowseMetamaps.append( metamap ); }

			void DummyResource::resetNextBrowseResult() { m_nextBrowseObjIds.clear(); m_nextBrowseMetamaps.clear(); }

			bool DummyResource::isCancelled() { return m_cancelled; }

			QList<QVariant> DummyResource::getLastCall() { return m_lastCall; }

			/********************************************************************
			 * DummyResource::getCallbackMethod
			 ********************************************************************/
			bool DummyResource::getCallbackMethod( QPointer<QObject> targetObject, const char* slotSignature, QMetaMethod& methodOut )
			{
				if( targetObject.isNull() )
				{
					critical() << "Target object is null";
					return false;
				}

				const QMetaObject* receiver = targetObject->metaObject();
				if ( receiver == 0 )
				{
					critical() << "Failed to get metaobject";
					return false;
				}

				// +1 comes from the preceding "1" caused by the use of SLOT macro in the interface ("1example_slot(uint))
				int sindex = receiver->indexOfSlot(QMetaObject::normalizedSignature(slotSignature + 1).data());
				if ( sindex < 0 )
				{
					critical() << "No such slot";
					return false;
				}

				methodOut = receiver->method(sindex);
				QList<QByteArray> parameterTypes = methodOut.parameterTypes();
				if ( parameterTypes.isEmpty() )
				{
					critical() << "Parameter list of callback method empty";
					return false;
				}

				return true;
			}
		}
	}
}
