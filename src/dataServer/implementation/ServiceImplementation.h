#pragma once

#include "GridStorage.h"
#include "dataServer/definition/ServiceInterface.h"
#include "grid-files/common/AttributeList.h"
#include <pthread.h>


namespace SmartMet
{
namespace DataServer
{



class ServiceImplementation : public ServiceInterface
{
  public:
                    ServiceImplementation();
     virtual        ~ServiceImplementation();

     virtual void   init(T::SessionId serverSessionId,uint serverId,std::string serverName,std::string serverIor,std::string dataDir,ContentServer::ServiceInterface *contentServer);
     virtual void   startEventProcessing();
     virtual void   shutdown();

     virtual void   eventProcessingThread();

  protected:

     virtual int    _getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList);
     virtual int    _getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridCoordinates& coordinates);
     virtual int    _getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data);
     virtual int    _getGridValue(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::ParamValue& value);
     virtual int    _getGridValueList(T::SessionId sessionId,T::ValueRecordList& valueRecordList);
     virtual int    _getGridValuesByArea(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,T::InterpolationMethod interpolationMethod,T::ParamValue_vec& values);

  protected:

     virtual void   event_clear(T::EventInfo& eventInfo);
     virtual void   event_contentServerReload(T::EventInfo& eventInfo);
     virtual void   event_producerAdded(T::EventInfo& eventInfo);
     virtual void   event_producerDeleted(T::EventInfo& eventInfo);
     virtual void   event_generationAdded(T::EventInfo& eventInfo);
     virtual void   event_generationDeleted(T::EventInfo& eventInfo);
     virtual void   event_generationStatusChanged(T::EventInfo& eventInfo);
     virtual void   event_generationListDeletedByProducerId(T::EventInfo& eventInfo);
     virtual void   event_fileAdded(T::EventInfo& eventInfo);
     virtual void   event_fileDeleted(T::EventInfo& eventInfo);
     virtual void   event_fileUpdated(T::EventInfo& eventInfo);
     virtual void   event_fileListDeletedByGroupFlags(T::EventInfo& eventInfo);
     virtual void   event_fileListDeletedByProducerId(T::EventInfo& eventInfo);
     virtual void   event_fileListDeletedByGenerationId(T::EventInfo& eventInfo);
     virtual void   event_contentListDeletedByFileId(T::EventInfo& eventInfo);
     virtual void   event_contentListDeletedByGroupFlags(T::EventInfo& eventInfo);
     virtual void   event_contentListDeletedByProducerId(T::EventInfo& eventInfo);
     virtual void   event_contentListDeletedByGenerationId(T::EventInfo& eventInfo);
     virtual void   event_dataServerAdded(T::EventInfo& eventInfo);
     virtual void   event_dataServerDeleted(T::EventInfo& eventInfo);
     virtual void   event_contentAdded(T::EventInfo& eventInfo);
     virtual void   event_contentDeleted(T::EventInfo& eventInfo);
     virtual void   event_contentRegistered(T::EventInfo& eventInfo);

     virtual void   addFile(T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList);
     virtual void   checkServerRegistration();
     virtual void   fullUpdate();
     virtual void   processEvent(T::EventInfo& eventInfo);
     virtual void   processEvents();

     T::EventId     mLastProcessedEventId;
     bool           mShutdownRequested;
     bool           mFullUpdateRequired;
     bool           mEventProcessingActive;
     bool           mContentRegistrationEnabled;

     T::SessionId   mServerSessionId;
     uint           mServerId;
     std::string    mServerName;
     std::string    mServerIor;
     std::string    mDataDir;
     pthread_t      mThread;
     time_t         mContentServerStartTime;
     GridStorage    mGridStorage;

     ContentServer::ServiceInterface* mContentServer;

};


}
}

