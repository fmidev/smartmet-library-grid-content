#pragma once

#include "ContentInfo.h"
#include "ContentInfoList.h"
#include "FileAndContent.h"
#include "FileInfo.h"
#include "FileInfoList.h"
#include "EventInfo.h"
#include "EventInfoList.h"
#include "ForecastTime.h"
#include "ProducerInfo.h"
#include "ProducerInfoList.h"
#include "GenerationInfo.h"
#include "GenerationInfoList.h"
#include "GeometryInfo.h"
#include "GeometryInfoList.h"
#include "ServiceResults.h"
#include "RequestFlags.h"

#include <grid-files/common/Log.h>
#include <grid-files/common/AttributeNode.h>
#include <grid-files/grid/Typedefs.h>
#include <macgyver/CacheStats.h>
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
  Memory = 5,
  Postgres = 6
};


class ServiceInterface
{
  public:
                    ServiceInterface();
     virtual        ~ServiceInterface();

     Implementation getImplementationType();

     virtual Log*   getDebugLog();
     virtual Log*   getProcessingLog();
     virtual void   setDebugLog(Log *debugLog);
     virtual void   setProcessingLog(Log *processingLog);
     virtual void   shutdown();
     virtual void   getCacheStats(Fmi::Cache::CacheStatistics& statistics) const;
     virtual time_t getContentChangeTime();
     virtual void   getStateAttributes(std::shared_ptr<T::AttributeNode> parent);

     virtual void   setEnabled(bool enabled);
     virtual bool   isReady();
     virtual bool   isEnabled();

     virtual std::string&   getSourceInfo();

     virtual int    clear(T::SessionId sessionId);
     virtual int    reload(T::SessionId sessionId);
     virtual int    getContentChangeTime(T::SessionId sessionId,time_t& changeTime);

     virtual int    addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo);
     virtual int    deleteProducerInfoById(T::SessionId sessionId,uint producerId);
     virtual int    deleteProducerInfoByName(T::SessionId sessionId,const std::string& producerName);
     virtual int    deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo);
     virtual int    getProducerInfoByName(T::SessionId sessionId,const std::string& producerName,T::ProducerInfo& producerInfo);
     virtual int    getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList);
     virtual int    getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList);
     virtual int    getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList);
     virtual int    getProducerInfoCount(T::SessionId sessionId,uint& count);
     virtual int    getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list);
     virtual int    getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list);
     virtual int    getProducerParameterListByProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list);
     virtual int    setProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo);

     virtual int    addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo);
     virtual int    deleteGenerationInfoById(T::SessionId sessionId,uint generationId);
     virtual int    deleteGenerationInfoByName(T::SessionId sessionId,const std::string& generationName);
     virtual int    deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<uint>& generationIdList);
     virtual int    deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId);
     virtual int    deleteGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName);
     virtual int    deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list);
     virtual int    getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo);
     virtual int    getGenerationInfoByName(T::SessionId sessionId,const std::string& generationName,T::GenerationInfo& generationInfo);
     virtual int    getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList);
     virtual int    getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList);
     virtual int    getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList);
     virtual int    getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList);
     virtual int    getGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::GenerationInfoList& generationInfoList);
     virtual int    getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,uchar generationStatus,T::GenerationInfo& generationInfo);
     virtual int    getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,const std::string& producerName,uchar generationStatus,T::GenerationInfo& generationInfo);
     virtual int    getGenerationInfoCount(T::SessionId sessionId,uint& count);
     virtual int    setGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo);
     virtual int    setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,uchar status);
     virtual int    setGenerationInfoStatusByName(T::SessionId sessionId,const std::string& generationName,uchar status);

     virtual int    addGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo);
     virtual int    deleteGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId);
     virtual int    deleteGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId);
     virtual int    deleteGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId);
     virtual int    deleteGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    getGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo);
     virtual int    getGeometryInfoList(T::SessionId sessionId,T::GeometryInfoList& geometryInfoList);
     virtual int    getGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId,T::GeometryInfoList& geometryInfoList);
     virtual int    getGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GeometryInfoList& geometryInfoList);
     virtual int    getGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GeometryInfoList& geometryInfoList);
     virtual int    getGeometryInfoCount(T::SessionId sessionId,uint& count);
     virtual int    setGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo);
     virtual int    setGeometryInfoStatusById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,uchar status);

     virtual int    addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo);
     virtual int    addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList);
     virtual int    addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList);
     virtual int    deleteFileInfoById(T::SessionId sessionId,uint fileId);
     virtual int    deleteFileInfoByName(T::SessionId sessionId,const std::string& filename);
     virtual int    deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId);
     virtual int    deleteFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName);
     virtual int    deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId);
     virtual int    deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,const std::string& forecastTime);
     virtual int    deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,time_t forecastTime);
     virtual int    deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList);
     virtual int    deleteFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName);
     virtual int    deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList);
     virtual int    getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo);
     virtual int    getFileInfoByName(T::SessionId sessionId,const std::string& filename,T::FileInfo& fileInfo);
     virtual int    getFileInfoList(T::SessionId sessionId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::FileInfoList& fileInfoList);
     virtual int    getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    getFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    getFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    getFileInfoCount(T::SessionId sessionId,uint& count);
     virtual int    getFileInfoCountByProducerId(T::SessionId sessionId,uint producerId,uint& count);
     virtual int    getFileInfoCountByGenerationId(T::SessionId sessionId,uint generationId,uint& count);
     virtual int    getFileInfoCountBySourceId(T::SessionId sessionId,uint sourceId,uint& count);
     virtual int    setFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo);

     virtual int    addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo);
     virtual int    getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo);
     virtual int    getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList);
     virtual int    getEventInfoCount(T::SessionId sessionId,uint& count);

     virtual int    addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo);
     virtual int    addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList);
     virtual int    deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex);
     virtual int    deleteContentListByFileId(T::SessionId sessionId,uint fileId);
     virtual int    deleteContentListByFileName(T::SessionId sessionId,const std::string& filename);
     virtual int    deleteContentListByProducerId(T::SessionId sessionId,uint producerId);
     virtual int    deleteContentListByProducerName(T::SessionId sessionId,const std::string& producerName);
     virtual int    deleteContentListByGenerationId(T::SessionId sessionId,uint generationId);
     virtual int    deleteContentListByGenerationName(T::SessionId sessionId,const std::string& generationName);
     virtual int    deleteContentListBySourceId(T::SessionId sessionId,uint sourceId);

     virtual int    getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo);
     virtual int    setContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo);
     virtual int    getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByFileName(T::SessionId sessionId,const std::string& filename,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,int maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList);

     virtual int    getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,T::ContentInfoList& contentInfoList);
     virtual int    getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,T::ContentInfoList& contentInfoList);

     virtual int    getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList);

     virtual int    getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<T::GeometryId>& geometryIdList);
     virtual int    getContentLevelListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList);
     virtual int    getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList);
     virtual int    getContentParamKeyListByGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
     virtual int    getContentParamKeyListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
     virtual int    getContentParamKeyListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
     virtual int    getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::set<std::string>& contentTimeList);
     virtual int    getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList);
     virtual int    getContentTimeListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<std::string>& contentTimeList);
     virtual int    getContentTimeListByProducerId(T::SessionId sessionId,uint producerId,std::set<std::string>& contentTimeList);
     virtual int    getContentTimeRangeByProducerAndGenerationId(T::SessionId sessionId,uint producerId,uint generationId,time_t& startTime,time_t& endTime);
     virtual int    getContentTimeRangeByGenerationId(T::SessionId sessionId,uint generationId,time_t& startTime,time_t& endTime);

     virtual int    getContentCount(T::SessionId sessionId,uint& count);
     virtual int    getHashByProducerId(T::SessionId sessionId,uint producerId,ulonglong& hash);

     virtual int    getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList);

     virtual int    updateCachedFiles(T::SessionId sessionId,std::set<uint>& fileIdList);

   protected:

     virtual int    _clear(T::SessionId sessionId);
     virtual int    _reload(T::SessionId sessionId);
     virtual int    _getContentChangeTime(T::SessionId sessionId,time_t& changeTime);

     virtual int    _addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo);
     virtual int    _deleteProducerInfoById(T::SessionId sessionId,uint producerId);
     virtual int    _deleteProducerInfoByName(T::SessionId sessionId,const std::string& producerName);
     virtual int    _deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    _getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo);
     virtual int    _getProducerInfoByName(T::SessionId sessionId,const std::string& producerName,T::ProducerInfo& producerInfo);
     virtual int    _getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList);
     virtual int    _getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList);
     virtual int    _getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList);
     virtual int    _getProducerInfoCount(T::SessionId sessionId,uint& count);
     virtual int    _getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list);
     virtual int    _getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list);
     virtual int    _getProducerParameterListByProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list);
     virtual int    _setProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo);

     virtual int    _addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo);
     virtual int    _deleteGenerationInfoById(T::SessionId sessionId,uint generationId);
     virtual int    _deleteGenerationInfoByName(T::SessionId sessionId,const std::string& generationName);
     virtual int    _deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<uint>& generationIdList);
     virtual int    _deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId);
     virtual int    _deleteGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName);
     virtual int    _deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    _getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list);
     virtual int    _getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo);
     virtual int    _getGenerationInfoByName(T::SessionId sessionId,const std::string& generationName,T::GenerationInfo& generationInfo);
     virtual int    _getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList);
     virtual int    _getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList);
     virtual int    _getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList);
     virtual int    _getGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::GenerationInfoList& generationInfoList);
     virtual int    _getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList);
     virtual int    _getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,uchar generationStatus,T::GenerationInfo& generationInfo);
     virtual int    _getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,const std::string& producerName,uchar generationStatus,T::GenerationInfo& generationInfo);
     virtual int    _getGenerationInfoCount(T::SessionId sessionId,uint& count);
     virtual int    _setGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo);
     virtual int    _setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,uchar status);
     virtual int    _setGenerationInfoStatusByName(T::SessionId sessionId,const std::string& generationName,uchar status);

     virtual int    _addGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo);
     virtual int    _deleteGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId);
     virtual int    _deleteGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId);
     virtual int    _deleteGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId);
     virtual int    _deleteGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    _getGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo);
     virtual int    _getGeometryInfoList(T::SessionId sessionId,T::GeometryInfoList& geometryInfoList);
     virtual int    _getGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId,T::GeometryInfoList& geometryInfoList);
     virtual int    _getGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GeometryInfoList& geometryInfoList);
     virtual int    _getGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GeometryInfoList& geometryInfoList);
     virtual int    _getGeometryInfoCount(T::SessionId sessionId,uint& count);
     virtual int    _setGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo);
     virtual int    _setGeometryInfoStatusById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,uchar status);

     virtual int    _addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo);
     virtual int    _addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList);
     virtual int    _addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList);
     virtual int    _deleteFileInfoById(T::SessionId sessionId,uint fileId);
     virtual int    _deleteFileInfoByName(T::SessionId sessionId,const std::string& filename);
     virtual int    _deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId);
     virtual int    _deleteFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName);
     virtual int    _deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId);
     virtual int    _deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,const std::string& forecastTime);
     virtual int    _deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,time_t forecastTime);
     virtual int    _deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList);
     virtual int    _deleteFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName);
     virtual int    _deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId);
     virtual int    _deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList);
     virtual int    _getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo);
     virtual int    _getFileInfoByName(T::SessionId sessionId,const std::string& filename,T::FileInfo& fileInfo);
     virtual int    _getFileInfoList(T::SessionId sessionId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoCount(T::SessionId sessionId,uint& count);
     virtual int    _getFileInfoCountByProducerId(T::SessionId sessionId,uint producerId,uint& count);
     virtual int    _getFileInfoCountByGenerationId(T::SessionId sessionId,uint generationId,uint& count);
     virtual int    _getFileInfoCountBySourceId(T::SessionId sessionId,uint sourceId,uint& count);
     virtual int    _setFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo);

     virtual int    _addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo);
     virtual int    _getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo);
     virtual int    _getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList);
     virtual int    _getEventInfoCount(T::SessionId sessionId,uint& count);

     virtual int    _addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo);
     virtual int    _setContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo);
     virtual int    _addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList);
     virtual int    _deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex);
     virtual int    _deleteContentListByFileId(T::SessionId sessionId,uint fileId);
     virtual int    _deleteContentListByFileName(T::SessionId sessionId,const std::string& filename);
     virtual int    _deleteContentListByProducerId(T::SessionId sessionId,uint producerId);
     virtual int    _deleteContentListByProducerName(T::SessionId sessionId,const std::string& producerName);
     virtual int    _deleteContentListByGenerationId(T::SessionId sessionId,uint generationId);
     virtual int    _deleteContentListByGenerationName(T::SessionId sessionId,const std::string& generationName);
     virtual int    _deleteContentListBySourceId(T::SessionId sessionId,uint sourceId);

     virtual int    _getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo);
     virtual int    _getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByFileName(T::SessionId sessionId,const std::string& filename,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,int maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList);

     virtual int    _getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,T::ContentInfoList& contentInfoList);

     virtual int    _getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList);

     virtual int    _getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<T::GeometryId>& geometryIdList);
     virtual int    _getContentLevelListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList);
     virtual int    _getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList);
     virtual int    _getContentParamKeyListByGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
     virtual int    _getContentParamKeyListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
     virtual int    _getContentParamKeyListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
     virtual int    _getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::set<std::string>& contentTimeList);
     virtual int    _getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList);
     virtual int    _getContentTimeListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<std::string>& contentTimeList);
     virtual int    _getContentTimeListByProducerId(T::SessionId sessionId,uint producerId,std::set<std::string>& contentTimeList);
     virtual int    _getContentTimeRangeByProducerAndGenerationId(T::SessionId sessionId,uint producerId,uint generationId,time_t& startTime,time_t& endTime);
     virtual int    _getContentTimeRangeByGenerationId(T::SessionId sessionId,uint generationId,time_t& startTime,time_t& endTime);

     virtual int    _getContentCount(T::SessionId sessionId,uint& count);
     virtual int    _getHashByProducerId(T::SessionId sessionId,uint producerId,ulonglong& hash);

     virtual int    _getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList);

     virtual int    _updateCachedFiles(T::SessionId sessionId,std::set<uint>& fileIdList);

     bool           mEnabled;
     Log            *mDebugLog;
     Log            *mProcessingLog;
     Implementation mImplementationType;
     std::string    mSourceInfo;
};



}
}
