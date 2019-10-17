#pragma once

#include "ContentInfo.h"
#include "ContentInfoList.h"
#include "FileAndContent.h"
#include "FileInfo.h"
#include "FileInfoList.h"
#include "EventInfo.h"
#include "EventInfoList.h"
#include "ForecastTime.h"
#include "ServerInfo.h"
#include "ServerInfoList.h"
#include "ProducerInfo.h"
#include "ProducerInfoList.h"
#include "GenerationInfo.h"
#include "GenerationInfoList.h"
#include "ServiceResults.h"
#include "RequestFlags.h"

#include <grid-files/common/Log.h>
#include <grid-files/grid/Typedefs.h>
#include <set>



namespace SmartMet
{

namespace ContentServer
{

enum class Implementation
{
  Interface = 0,
  Redis = 1,
  Cache = 2,
  CorbaClient = 3,
  HttpClient = 4,
  Memory = 5
};


class ServiceInterface
{
  public:
                    ServiceInterface();
     virtual        ~ServiceInterface();

     Implementation getImplementationType();

     virtual void   setDebugLog(Log *debugLog);
     virtual void   setProcessingLog(Log *processingLog);
     virtual void   shutdown();

     virtual int    clear(T::SessionId sessionId);
     virtual int    reload(T::SessionId sessionId);

     virtual int    addDataServerInfo(T::SessionId sessionId,T::ServerInfo& serverInfo);
     virtual int    deleteDataServerInfoById(T::SessionId sessionId,uint serverId);
     virtual int    getDataServerInfoById(T::SessionId sessionId,uint serverId,T::ServerInfo& serverInfo);
     virtual int    getDataServerInfoByName(T::SessionId sessionId,std::string serverName,T::ServerInfo& serverInfo);
     virtual int    getDataServerInfoByIor(T::SessionId sessionId,std::string serverIor,T::ServerInfo& serverInfo);
     virtual int    getDataServerInfoList(T::SessionId sessionId,T::ServerInfoList& serverInfoList);
     virtual int    getDataServerInfoCount(T::SessionId sessionId,uint& count);

     virtual int    addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo);
     virtual int    deleteProducerInfoById(T::SessionId sessionId,uint producerId);
     virtual int    deleteProducerInfoByName(T::SessionId sessionId,std::string producerName);
     virtual int    deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo);
     virtual int    getProducerInfoByName(T::SessionId sessionId,std::string producerName,T::ProducerInfo& producerInfo);
     virtual int    getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList);
     virtual int    getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList);
     virtual int    getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList);
     virtual int    getProducerInfoCount(T::SessionId sessionId,uint& count);
     virtual int    getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list);
     virtual int    getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list);

     virtual int    addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo);
     virtual int    deleteGenerationInfoById(T::SessionId sessionId,uint generationId);
     virtual int    deleteGenerationInfoByName(T::SessionId sessionId,std::string generationName);
     virtual int    deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<uint>& generationIdList);
     virtual int    deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId);
     virtual int    deleteGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName);
     virtual int    deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list);
     virtual int    getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo);
     virtual int    getGenerationInfoByName(T::SessionId sessionId,std::string generationName,T::GenerationInfo& generationInfo);
     virtual int    getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList);
     virtual int    getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList);
     virtual int    getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList);
     virtual int    getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList);
     virtual int    getGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName,T::GenerationInfoList& generationInfoList);
     virtual int    getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,uchar generationStatus,T::GenerationInfo& generationInfo);
     virtual int    getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,std::string producerName,uchar generationStatus,T::GenerationInfo& generationInfo);
     virtual int    getGenerationInfoCount(T::SessionId sessionId,uint& count);
     virtual int    setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,uchar status);
     virtual int    setGenerationInfoStatusByName(T::SessionId sessionId,std::string generationName,uchar status);

     virtual int    addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo);
     virtual int    addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList);
     virtual int    addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList);
     virtual int    deleteFileInfoById(T::SessionId sessionId,uint fileId);
     virtual int    deleteFileInfoByName(T::SessionId sessionId,std::string filename);
     virtual int    deleteFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags);
     virtual int    deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId);
     virtual int    deleteFileInfoListByProducerName(T::SessionId sessionId,std::string producerName);
     virtual int    deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId);
     virtual int    deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,std::string forecastTime);
     virtual int    deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList);
     virtual int    deleteFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName);
     virtual int    deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList);
     virtual int    getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo);
     virtual int    getFileInfoByName(T::SessionId sessionId,std::string filename,T::FileInfo& fileInfo);
     virtual int    getFileInfoList(T::SessionId sessionId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::FileInfoList& fileInfoList);
     virtual int    getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    getFileInfoListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    getFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    getFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    getFileInfoCount(T::SessionId sessionId,uint& count);
     virtual int    getFileInfoCountByProducerId(T::SessionId sessionId,uint producerId,uint& count);
     virtual int    getFileInfoCountByGenerationId(T::SessionId sessionId,uint generationId,uint& count);
     virtual int    getFileInfoCountBySourceId(T::SessionId sessionId,uint sourceId,uint& count);

     virtual int    addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo);
     virtual int    getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo);
     virtual int    getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,uint maxRecords,T::EventInfoList& eventInfoList);
     virtual int    getEventInfoCount(T::SessionId sessionId,uint& count);

     virtual int    addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo);
     virtual int    addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList);
     virtual int    deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex);
     virtual int    deleteContentListByFileId(T::SessionId sessionId,uint fileId);
     virtual int    deleteContentListByFileName(T::SessionId sessionId,std::string filename);
     virtual int    deleteContentListByGroupFlags(T::SessionId sessionId,uint groupFlags);
     virtual int    deleteContentListByProducerId(T::SessionId sessionId,uint producerId);
     virtual int    deleteContentListByProducerName(T::SessionId sessionId,std::string producerName);
     virtual int    deleteContentListByGenerationId(T::SessionId sessionId,uint generationId);
     virtual int    deleteContentListByGenerationName(T::SessionId sessionId,std::string generationName);
     virtual int    deleteContentListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    registerContentList(T::SessionId sessionId,uint serverId,T::ContentInfoList& contentInfoList);
     virtual int    registerContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId);
     virtual int    unregisterContentList(T::SessionId sessionId,uint serverId);
     virtual int    unregisterContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId);

     virtual int    getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo);
     virtual int    getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByFileName(T::SessionId sessionId,std::string filename,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByServerId(T::SessionId sessionId,uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,std::string generationName,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList);

     virtual int    getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByParameterAndGenerationName(T::SessionId sessionId,std::string generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByParameterAndProducerName(T::SessionId sessionId,std::string producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string forecastTime,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByRequestCounterKey(T::SessionId sessionId,ulonglong key,T::ContentInfoList& contentInfoList);

     virtual int    getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList);

     virtual int    getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<T::GeometryId>& geometryIdList);
     virtual int    getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList);
     virtual int    getContentParamKeyListByGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
     virtual int    getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::set<std::string>& contentTimeList);
     virtual int    getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList);
     virtual int    getContentTimeListByProducerId(T::SessionId sessionId,uint producerId,std::set<std::string>& contentTimeList);

     virtual int    getContentCount(T::SessionId sessionId,uint& count);

     virtual int    getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList);

     virtual int    deleteVirtualContent(T::SessionId sessionId);
     virtual int    updateVirtualContent(T::SessionId sessionId);

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
     virtual int    _getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList);
     virtual int    _getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList);
     virtual int    _getProducerInfoCount(T::SessionId sessionId,uint& count);
     virtual int    _getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list);
     virtual int    _getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list);

     virtual int    _addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo);
     virtual int    _deleteGenerationInfoById(T::SessionId sessionId,uint generationId);
     virtual int    _deleteGenerationInfoByName(T::SessionId sessionId,std::string generationName);
     virtual int    _deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<uint>& generationIdList);
     virtual int    _deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId);
     virtual int    _deleteGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName);
     virtual int    _deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    _getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list);
     virtual int    _getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo);
     virtual int    _getGenerationInfoByName(T::SessionId sessionId,std::string generationName,T::GenerationInfo& generationInfo);
     virtual int    _getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList);
     virtual int    _getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList);
     virtual int    _getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList);
     virtual int    _getGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName,T::GenerationInfoList& generationInfoList);
     virtual int    _getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList);
     virtual int    _getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,uchar generationStatus,T::GenerationInfo& generationInfo);
     virtual int    _getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,std::string producerName,uchar generationStatus,T::GenerationInfo& generationInfo);
     virtual int    _getGenerationInfoCount(T::SessionId sessionId,uint& count);
     virtual int    _setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,uchar status);
     virtual int    _setGenerationInfoStatusByName(T::SessionId sessionId,std::string generationName,uchar status);

     virtual int    _addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo);
     virtual int    _addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList);
     virtual int    _addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList);
     virtual int    _deleteFileInfoById(T::SessionId sessionId,uint fileId);
     virtual int    _deleteFileInfoByName(T::SessionId sessionId,std::string filename);
     virtual int    _deleteFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags);
     virtual int    _deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId);
     virtual int    _deleteFileInfoListByProducerName(T::SessionId sessionId,std::string producerName);
     virtual int    _deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId);
     virtual int    _deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,std::string forecastTime);
     virtual int    _deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList);
     virtual int    _deleteFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName);
     virtual int    _deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    _deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList);
     virtual int    _getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo);
     virtual int    _getFileInfoByName(T::SessionId sessionId,std::string filename,T::FileInfo& fileInfo);
     virtual int    _getFileInfoList(T::SessionId sessionId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoCount(T::SessionId sessionId,uint& count);
     virtual int    _getFileInfoCountByProducerId(T::SessionId sessionId,uint producerId,uint& count);
     virtual int    _getFileInfoCountByGenerationId(T::SessionId sessionId,uint generationId,uint& count);
     virtual int    _getFileInfoCountBySourceId(T::SessionId sessionId,uint sourceId,uint& count);

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
     virtual int    _getContentListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByFileName(T::SessionId sessionId,std::string filename,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByServerId(T::SessionId sessionId,uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,std::string generationName,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList);

     virtual int    _getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndGenerationName(T::SessionId sessionId,std::string generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndProducerName(T::SessionId sessionId,std::string producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string forecastTime,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByRequestCounterKey(T::SessionId sessionId,ulonglong key,T::ContentInfoList& contentInfoList);

     virtual int    _getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList);

     virtual int    _getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<T::GeometryId>& geometryIdList);
     virtual int    _getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList);
     virtual int    _getContentParamKeyListByGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
     virtual int    _getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::set<std::string>& contentTimeList);
     virtual int    _getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList);
     virtual int    _getContentTimeListByProducerId(T::SessionId sessionId,uint producerId,std::set<std::string>& contentTimeList);

     virtual int    _getContentCount(T::SessionId sessionId,uint& count);

     virtual int    _getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList);

     virtual int    _deleteVirtualContent(T::SessionId sessionId);
     virtual int    _updateVirtualContent(T::SessionId sessionId);

     Log            *mDebugLog;
     Log            *mProcessingLog;
     Implementation mImplementationType;
};



}
}
