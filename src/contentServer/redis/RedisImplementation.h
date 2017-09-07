#pragma once

#include "contentServer/definition/ServiceInterface.h"
#include <hiredis/hiredis.h>
#include <pthread.h>



namespace SmartMet
{
namespace ContentServer
{

class RedisImplementation : public ServiceInterface
{
  public:
                    RedisImplementation();
     virtual        ~RedisImplementation();

     virtual void   init(const char *redisAddress,int redisPort,const char *tablePrefix);
     virtual void   shutdown();

   protected:

     virtual int    _clear(T::SessionId sessionId);
     virtual int    _reload(T::SessionId sessionId);

     virtual int    _addDataServerInfo(T::SessionId sessionId,T::ServerInfo& serverInfo);
     virtual int    _deleteDataServerInfoById(T::SessionId sessionId,uint serverId);
     virtual int    _getDataServerInfoById(T::SessionId sessionId,uint serverId,T::ServerInfo& serverInfo);
     virtual int    _getDataServerInfoByName(T::SessionId sessionId,std::string serverName,T::ServerInfo& serverInfo);
     virtual int    _getDataServerInfoByIor(T::SessionId sessionId,std::string serverIor,T::ServerInfo& serverInfo);
     virtual int    _getDataServerInfoList(T::SessionId sessionId,T::ServerInfoList& serverInfoList);
     virtual int    _getDataServerInfoCount(T::SessionId sessionId,uint& count);

     virtual int    _addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo);
     virtual int    _deleteProducerInfoById(T::SessionId sessionId,uint producerId);
     virtual int    _deleteProducerInfoByName(T::SessionId sessionId,std::string producerName);
     virtual int    _deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    _getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo);
     virtual int    _getProducerInfoByName(T::SessionId sessionId,std::string producerName,T::ProducerInfo& producerInfo);
     virtual int    _getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList);
     virtual int    _getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList);
     virtual int    _getProducerInfoCount(T::SessionId sessionId,uint& count);

     virtual int    _addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo);
     virtual int    _deleteGenerationInfoById(T::SessionId sessionId,uint generationId);
     virtual int    _deleteGenerationInfoByName(T::SessionId sessionId,std::string generationName);
     virtual int    _deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId);
     virtual int    _deleteGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName);
     virtual int    _deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    _getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo);
     virtual int    _getGenerationInfoByName(T::SessionId sessionId,std::string generationName,T::GenerationInfo& generationInfo);
     virtual int    _getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList);
     virtual int    _getGenerationInfoListByGeometryId(T::SessionId sessionId,uint geometryId,T::GenerationInfoList& generationInfoList);
     virtual int    _getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList);
     virtual int    _getGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName,T::GenerationInfoList& generationInfoList);
     virtual int    _getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList);
     virtual int    _getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,T::GenerationStatus generationStatus,T::GenerationInfo& generationInfo);
     virtual int    _getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,std::string producerName,T::GenerationStatus generationStatus,T::GenerationInfo& generationInfo);
     virtual int    _getGenerationInfoCount(T::SessionId sessionId,uint& count);
     virtual int    _setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,T::GenerationStatus status);
     virtual int    _setGenerationInfoStatusByName(T::SessionId sessionId,std::string generationName,T::GenerationStatus status);

     virtual int    _addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo);
     virtual int    _addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList);
     virtual int    _deleteFileInfoById(T::SessionId sessionId,uint fileId);
     virtual int    _deleteFileInfoByName(T::SessionId sessionId,std::string filename);
     virtual int    _deleteFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags);
     virtual int    _deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId);
     virtual int    _deleteFileInfoListByProducerName(T::SessionId sessionId,std::string producerName);
     virtual int    _deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId);
     virtual int    _deleteFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName);
     virtual int    _deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    _deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList);
     virtual int    _getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo);
     virtual int    _getFileInfoByName(T::SessionId sessionId,std::string filename,T::FileInfo& fileInfo);
     virtual int    _getFileInfoList(T::SessionId sessionId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoCount(T::SessionId sessionId,uint& count);

     virtual int    _addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo);
     virtual int    _getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo);
     virtual int    _getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,uint maxRecords,T::EventInfoList& eventInfoList);
     virtual int    _getEventInfoCount(T::SessionId sessionId,uint& count);

     virtual int    _addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo);
     virtual int    _addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList);
     virtual int    _deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex);
     virtual int    _deleteContentListByFileId(T::SessionId sessionId,uint fileId);
     virtual int    _deleteContentListByFileName(T::SessionId sessionId,std::string filename);
     virtual int    _deleteContentListByGroupFlags(T::SessionId sessionId,uint groupFlags);
     virtual int    _deleteContentListByProducerId(T::SessionId sessionId,uint producerId);
     virtual int    _deleteContentListByProducerName(T::SessionId sessionId,std::string producerName);
     virtual int    _deleteContentListByGenerationId(T::SessionId sessionId,uint generationId);
     virtual int    _deleteContentListByGenerationName(T::SessionId sessionId,std::string generationName);
     virtual int    _deleteContentListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    _registerContentList(T::SessionId sessionId,uint serverId,T::ContentInfoList& contentInfoList);
     virtual int    _registerContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId);
     virtual int    _unregisterContentList(T::SessionId sessionId,uint serverId);
     virtual int    _unregisterContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId);

     virtual int    _getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo);
     virtual int    _getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByFileName(T::SessionId sessionId,std::string filename,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByServerId(T::SessionId sessionId,uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,std::string generationName,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);

     virtual int    _getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndGenerationName(T::SessionId sessionId,std::string generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndProducerName(T::SessionId sessionId,std::string producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);

     virtual int    _getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<uint>& geometryIdList);
     virtual int    _getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList);
     virtual int    _getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::vector<std::string>& contentTimeList);

     virtual int    _getContentCount(T::SessionId sessionId,uint& count);

   protected:

     bool           isConnectionValid();
     int            openConnection();
     void           closeConnection();
     bool           isSessionValid(T::SessionId sessionId);

     T::EventId     addEvent(EventType eventType,uint id1,uint id2,uint id3,unsigned long long flags);

     int            deleteDataServerById(uint serverId);
     int            getDataServerById(uint serverId,T::ServerInfo& serverInfo);
     int            getDataServerList(T::ServerInfoList& serverInfoList);

     int            deleteContentServerById(uint serverId);
     int            getContentServerById(uint serverId,T::ServerInfo& serverInfo);
     int            getContentServerList(T::ServerInfoList& serverInfoList);

     int            deleteProducerById(uint producerId,bool deleteGenerations,bool deleteFiles,bool deleteContent);
     int            deleteProducerListBySourceId(uint sourceId,bool deleteGenerations,bool deleteFiles,bool deleteContent);
     int            getProducerById(uint producerId,T::ProducerInfo& producerInfo);
     int            getProducerByName(std::string producerName,T::ProducerInfo& producerInfo);
     int            getProducerList(T::ProducerInfoList& producerInfoList);
     int            getProducerListBySourceId(uint sourceId,T::ProducerInfoList& producerInfoList);

     int            deleteGenerationById(uint generationId,bool deleteFiles,bool deleteContent);
     int            deleteGenerationListByProducerId(uint producerId,bool deleteFiles,bool deleteContent);
     int            deleteGenerationListBySourceId(uint sourceId,bool deleteFiles,bool deleteContent);
     int            getGenerationById(uint generationId,T::GenerationInfo& generationInfo);
     int            getGenerationByName(std::string generationName,T::GenerationInfo& generationInfo);
     int            getGenerationList(T::GenerationInfoList& generationInfoList);
     int            getGenerationListByProducerId(uint producerId,T::GenerationInfoList& generationInfoList);
     int            getGenerationListByGeometryId(uint geometryId,T::GenerationInfoList& generationInfoList);
     int            getGenerationListBySourceId(uint sourceId,T::GenerationInfoList& generationInfoList);

     int            deleteFileById(uint fileId,bool deleteContent);
     int            deleteFileListByGenerationId(uint generationId,bool deleteContent);
     int            deleteFileListByGroupFlags(uint groupFlags,bool deleteContent);
     int            deleteFileListByProducerId(uint producerId,bool deleteContent);
     int            deleteFileListBySourceId(uint sourceId,bool deleteContent);
     int            getFileById(uint fileId,T::FileInfo& fileInfo);
     int            getFileList(uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     int            getFileListByGenerationId(uint generationId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     int            getFileListByGroupFlags(uint groupFlags,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     int            getFileListByProducerId(uint producerId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     int            getFileListBySourceId(uint sourceId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);

     int            setContent(T::ContentInfo& contentInfo);
     int            deleteContent(uint fileId,uint messageIndex);
     int            deleteContentByFileId(uint fileId);
     int            deleteContentByGenerationId(uint generationId);
     int            deleteContentByGroupFlags(uint groupFlags);
     int            deleteContentByProducerId(uint producerId);
     int            deleteContentBySourceId(uint sourceId);
     int            getContent(uint fileId,uint messageIndex,T::ContentInfo& contentInfo);
     int            getContent(uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     int            getContentByFileId(uint fileId,T::ContentInfoList& contentInfoList);
     int            getContentByGenerationId(uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     int            getContentByGenerationIdAndTimeRange(uint generationId,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList);
     int            getContentByGroupFlags(uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     int            getContentByProducerId(uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     int            getContentByServerId(uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     int            getContentBySourceId(uint sourceId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     int            getContentByParameterIdAndTimeRange(T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList);
     int            getContentByParameterIdAndGeneration(uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList);
     int            getContentByParameterIdAndProducer(uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList);
     void           truncateEvents();
     int            unregisterContent(uint serverId);
     void           resetContentRegistrations();

     int            addFilename(std::string filename,uint fileId);
     int            deleteFilename(std::string filename);
     uint           getFileId(std::string filename);

     ThreadLock     mThreadLock;
     redisContext*  mContext;
     std::string    mTablePrefix;
     time_t         mStartTime;
     T::EventInfo   mLastEvent;
     std::string    mRedisAddress;
     int            mRedisPort;
};


}
}
