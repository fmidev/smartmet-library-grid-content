#pragma once

#include "GridFileManager.h"
#include "VirtualContentManager.h"
#include "dataServer/definition/ServiceInterface.h"
#include "lua/LuaFileCollection.h"

#include <grid-files/common/AttributeList.h>
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

     virtual void   init(
                       T::SessionId serverSessionId,
                       uint serverId,
                       std::string serverName,
                       std::string serverIor,
                       std::string dataDir,
                       ContentServer::ServiceInterface *contentServer,
                       string_vec& luaFileNames);

     virtual void   startEventProcessing();
     virtual void   shutdown();

     virtual void   addVirtualContentFactory(VirtualContentFactory *factory);
     virtual void   enableVirtualContent(bool enabled);
     virtual void   eventProcessingThread();

  protected:

     virtual int    _getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::AttributeList& attributeList);
     virtual int    _getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,T::GridCoordinates& coordinates);
     virtual int    _getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::GridData& data);
     virtual int    _getGridFileCount(T::SessionId sessionId,uint& count);
     virtual int    _getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::ParamValue& value);
     virtual int    _getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList);
     virtual int    _getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,T::InterpolationMethod interpolationMethod,T::GridValueList& valueList);
     virtual int    _getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList);
     virtual int    _getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,T::GridValueList& valueList);
     virtual int    _getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,T::GridValueList& valueList);
     virtual int    _getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,T::InterpolationMethod interpolationMethod,T::ParamValue_vec& values);

     virtual int    _getMultipleGridValues(T::SessionId sessionId,T::ValueRecordList& valueRecordList);

  protected:

     virtual void   event_clear(T::EventInfo& eventInfo);
     virtual void   event_contentServerReload(T::EventInfo& eventInfo);
     virtual void   event_producerAdded(T::EventInfo& eventInfo);
     virtual void   event_producerDeleted(T::EventInfo& eventInfo);
     virtual void   event_producerListDeletedBySourceId(T::EventInfo& eventInfo);
     virtual void   event_generationAdded(T::EventInfo& eventInfo);
     virtual void   event_generationDeleted(T::EventInfo& eventInfo);
     virtual void   event_generationStatusChanged(T::EventInfo& eventInfo);
     virtual void   event_generationListDeletedByProducerId(T::EventInfo& eventInfo);
     virtual void   event_generationListDeletedBySourceId(T::EventInfo& eventInfo);
     virtual void   event_fileAdded(T::EventInfo& eventInfo);
     virtual void   event_fileDeleted(T::EventInfo& eventInfo);
     virtual void   event_fileUpdated(T::EventInfo& eventInfo);
     virtual void   event_fileListDeletedByGroupFlags(T::EventInfo& eventInfo);
     virtual void   event_fileListDeletedByProducerId(T::EventInfo& eventInfo);
     virtual void   event_fileListDeletedByGenerationId(T::EventInfo& eventInfo);
     virtual void   event_fileListDeletedBySourceId(T::EventInfo& eventInfo);
     virtual void   event_contentListDeletedByFileId(T::EventInfo& eventInfo);
     virtual void   event_contentListDeletedByGroupFlags(T::EventInfo& eventInfo);
     virtual void   event_contentListDeletedByProducerId(T::EventInfo& eventInfo);
     virtual void   event_contentListDeletedByGenerationId(T::EventInfo& eventInfo);
     virtual void   event_contentListDeletedBySourceId(T::EventInfo& eventInfo);
     virtual void   event_dataServerAdded(T::EventInfo& eventInfo);
     virtual void   event_dataServerDeleted(T::EventInfo& eventInfo);
     virtual void   event_contentAdded(T::EventInfo& eventInfo);
     virtual void   event_contentDeleted(T::EventInfo& eventInfo);
     virtual void   event_contentRegistered(T::EventInfo& eventInfo);
     virtual void   event_deleteVirtualContent(T::EventInfo& eventInfo);
     virtual void   event_updateVirtualContent(T::EventInfo& eventInfo);

     virtual void   addFile(T::FileInfo& fileInfo,T::ContentInfoList& currentContentList,T::ContentInfoList& contentInfoList);
     virtual void   checkServerRegistration();
     virtual void   fullUpdate();
     virtual void   updateVirtualFiles(T::ContentInfoList fullContentList);
     virtual void   registerVirtualFiles(VirtualGridFilePtr_map& gridFileMap);
     virtual void   processEvent(T::EventInfo& eventInfo);
     virtual void   processEvents();
     virtual void   readContentList(T::ContentInfoList& contentList,bool includePhysicalContent,bool includeVirtualContent);

     GRID::GridFile_sptr  getGridFile(uint fileId);

     T::EventId           mLastProcessedEventId;
     bool                 mShutdownRequested;
     bool                 mFullUpdateRequired;
     bool                 mEventProcessingActive;
     bool                 mContentRegistrationEnabled;
     bool                 mVirtualContentEnabled;

     T::SessionId         mServerSessionId;
     uint                 mServerId;
     std::string          mServerName;
     std::string          mServerIor;
     std::string          mDataDir;
     pthread_t            mThread;
     time_t               mContentServerStartTime;
     GridFileManager      mGridFileManager;

     VirtualContentManager            mVirtualContentManager;
     ContentServer::ServiceInterface* mContentServer;
     Lua::LuaFileCollection           mLuaFileCollection;
     Functions::FunctionCollection    mFunctionCollection;
     VirtualGridFilePtr_map           mGridFileMap;
     time_t                           mLastVirtualFileRegistration;
};


}
}

