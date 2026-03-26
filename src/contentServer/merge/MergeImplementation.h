#pragma once

#include "../definition/ServiceInterface.h"
#include "../definition/ContentListStorage.h"
#include "../definition/FileListStorage.h"
#include "../cache/CacheImplementation.h"

#include <grid-files/common/ModificationLock.h>
#include <pthread.h>
#include <atomic>
#include <boost/smart_ptr/atomic_shared_ptr.hpp>




namespace SmartMet
{
namespace ContentServer
{


//#define CONTENT_LIST_COUNT 2

typedef std::map<T::GenerationId,std::set<std::string>> ContentTimeCache;

typedef std::shared_ptr<ContentServer::ServiceInterface> ContentServer_sptr;
typedef std::vector<ContentServer_sptr> ContentServer_sptr_vec;


class ContentSource
{
  public:
    ContentSource()
    {
      mContentStorageStartTime = 0;
      mLastProcessedEventId = 0;
    }

    ContentServer_sptr     mContentStorage;
    time_t                 mContentStorageStartTime;
    T::EventId             mLastProcessedEventId;
};

using ContentSource_vec = std::vector<ContentSource>;


class MergeImplementation : public ServiceInterface
{
  public:
                    MergeImplementation();
    virtual         ~MergeImplementation();

    virtual void    init(T::SessionId sessionId,T::SessionId dataServerSessionId,ContentServer_sptr_vec& contentStorages);
    virtual bool    isReady();
    virtual void    startEventProcessing();
    virtual void    setEventListMaxLength(uint maxLength);
    virtual void    setContentSwap(uint fileCacheMaxFirstWaitTime,uint fileCacheMaxWaitTime);
    virtual void    setContentUpdateInterval(uint intervalInSec);
    virtual void    shutdown();
    //std::string&    getSourceInfo();

    virtual void    getCacheStats(Fmi::Cache::CacheStatistics& statistics) const;
    virtual void    getStateAttributes(std::shared_ptr<T::AttributeNode> parent);

    virtual void    eventProcessingThread();

  protected:

    virtual int     _clear(T::SessionId sessionId);
    virtual int     _reload(T::SessionId sessionId);
    virtual int     _getContentChangeTime(T::SessionId sessionId,time_t& changeTime);

    virtual int     _addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo);
    virtual int     _deleteProducerInfoById(T::SessionId sessionId,T::ProducerId producerId);
    virtual int     _deleteProducerInfoByName(T::SessionId sessionId,const std::string& producerName);
    virtual int     _deleteProducerInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId);
    virtual int     _getProducerInfoById(T::SessionId sessionId,T::ProducerId producerId,T::ProducerInfo& producerInfo);
    virtual int     _getProducerInfoByName(T::SessionId sessionId,const std::string& producerName,T::ProducerInfo& producerInfo);
    virtual int     _getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList);
    virtual int     _getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList);
    virtual int     _getProducerInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::ProducerInfoList& producerInfoList);
    virtual int     _getProducerInfoCount(T::SessionId sessionId,uint& count);
    virtual int     _getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list);
    virtual int     _getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list);
    virtual int     _getProducerParameterListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list);
    virtual int     _setProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo);

    virtual int     _addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo);
    virtual int     _deleteGenerationInfoById(T::SessionId sessionId,T::GenerationId generationId);
    virtual int     _deleteGenerationInfoByName(T::SessionId sessionId,const std::string& generationName);
    virtual int     _deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<T::GenerationId>& generationIdList);
    virtual int     _deleteGenerationInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId);
    virtual int     _deleteGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName);
    virtual int     _deleteGenerationInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId);
    virtual int     _getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list);
    virtual int     _getGenerationInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GenerationInfo& generationInfo);
    virtual int     _getGenerationInfoByName(T::SessionId sessionId,const std::string& generationName,T::GenerationInfo& generationInfo);
    virtual int     _getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList);
    virtual int     _getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList);
    virtual int     _getGenerationInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::GenerationInfoList& generationInfoList);
    virtual int     _getGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::GenerationInfoList& generationInfoList);
    virtual int     _getGenerationInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::GenerationInfoList& generationInfoList);
    virtual int     _getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,T::ProducerId producerId,uchar generationStatus,T::GenerationInfo& generationInfo);
    virtual int     _getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,const std::string& producerName,uchar generationStatus,T::GenerationInfo& generationInfo);
    virtual int     _getGenerationInfoCount(T::SessionId sessionId,uint& count);
    virtual int     _setGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo);
    virtual int     _setGenerationInfoStatusById(T::SessionId sessionId,T::GenerationId generationId,uchar status);
    virtual int     _setGenerationInfoStatusByName(T::SessionId sessionId,const std::string& generationName,uchar status);

    virtual int     _addGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo);
    virtual int     _deleteGeometryInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId);
    virtual int     _deleteGeometryInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId);
    virtual int     _deleteGeometryInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId);
    virtual int     _deleteGeometryInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId);
    virtual int     _getGeometryInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo);
    virtual int     _getGeometryInfoList(T::SessionId sessionId,T::GeometryInfoList& geometryInfoList);
    virtual int     _getGeometryInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryInfoList& geometryInfoList);
    virtual int     _getGeometryInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::GeometryInfoList& geometryInfoList);
    virtual int     _getGeometryInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::GeometryInfoList& geometryInfoList);
    virtual int     _getGeometryInfoCount(T::SessionId sessionId,uint& count);
    virtual int     _setGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo);
    virtual int     _setGeometryInfoStatusById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,uchar status);

    virtual int     _addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo);
    virtual int     _addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList);
    virtual int     _addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList);
    virtual int     _deleteFileInfoById(T::SessionId sessionId,T::FileId fileId);
    virtual int     _deleteFileInfoByName(T::SessionId sessionId,const std::string& filename);
    virtual int     _deleteFileInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId);
    virtual int     _deleteFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName);
    virtual int     _deleteFileInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId);
    virtual int     _deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,time_t forecastTime);
    virtual int     _deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList);
    virtual int     _deleteFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName);
    virtual int     _deleteFileInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId);
    virtual int     _deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<T::FileId>& fileIdList);
    virtual int     _getFileInfoById(T::SessionId sessionId,T::FileId fileId,T::FileInfo& fileInfo);
    virtual int     _getFileInfoByName(T::SessionId sessionId,const std::string& filename,T::FileInfo& fileInfo);
    virtual int     _getFileInfoList(T::SessionId sessionId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);
    virtual int     _getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<T::FileId>& fileIdList,T::FileInfoList& fileInfoList);
    virtual int     _getFileInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);
    virtual int     _getFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);
    virtual int     _getFileInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);
    virtual int     _getFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);
    virtual int     _getFileInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);
    virtual int     _getFileInfoCount(T::SessionId sessionId,uint& count);
    virtual int     _getFileInfoCountByProducerId(T::SessionId sessionId,T::ProducerId producerId,uint& count);
    virtual int     _getFileInfoCountByGenerationId(T::SessionId sessionId,T::GenerationId generationId,uint& count);
    virtual int     _getFileInfoCountBySourceId(T::SessionId sessionId,T::SourceId sourceId,uint& count);
    virtual int     _setFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo);

    virtual int     _addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo);
    virtual int     _getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo);
    virtual int     _getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList);
    virtual int     _getEventInfoCount(T::SessionId sessionId,uint& count);

    virtual int     _addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo);
    virtual int     _setContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo);
    virtual int     _addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList);
    virtual int     _deleteContentInfo(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex);
    virtual int     _deleteContentListByFileId(T::SessionId sessionId,T::FileId fileId);
    virtual int     _deleteContentListByFileName(T::SessionId sessionId,const std::string& filename);
    virtual int     _deleteContentListByProducerId(T::SessionId sessionId,T::ProducerId producerId);
    virtual int     _deleteContentListByProducerName(T::SessionId sessionId,const std::string& producerName);
    virtual int     _deleteContentListByGenerationId(T::SessionId sessionId,T::GenerationId generationId);
    virtual int     _deleteContentListByGenerationName(T::SessionId sessionId,const std::string& generationName);
    virtual int     _deleteContentListBySourceId(T::SessionId sessionId,T::SourceId sourceId);

    virtual int     _getContentInfo(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::ContentInfo& contentInfo);
    virtual int     _getContentList(T::SessionId sessionId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByFileId(T::SessionId sessionId,T::FileId fileId,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByFileIdList(T::SessionId sessionId,std::vector<T::FileId>& fileIdList,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByFileName(T::SessionId sessionId,const std::string& filename,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByProducerName(T::SessionId sessionId,const std::string& producerName,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByGenerationName(T::SessionId sessionId,const std::string& generationName,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,T::GenerationId generationId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);

    virtual int     _getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByParameterAndGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByParameterAndProducerId(T::SessionId sessionId,T::ProducerId producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,T::ContentInfoList& contentInfoList);

    virtual int     _getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList);

    virtual int     _getContentGeometryIdListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,std::set<T::GeometryId>& geometryIdList);
    virtual int     _getContentLevelListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList);
    virtual int     _getContentLevelListByParameterGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,std::string parameterKey,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList);
    virtual int     _getContentParamListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ContentInfoList& contentParamList);
    virtual int     _getContentParamKeyListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
    virtual int     _getContentParamKeyListByGenerationAndGeometryId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
    virtual int     _getContentParamKeyListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
    virtual int     _getContentTimeListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,std::set<std::string>& contentTimeList);
    virtual int     _getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList);
    virtual int     _getContentTimeListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<std::string>& contentTimeList);
    virtual int     _getContentTimeListByProducerId(T::SessionId sessionId,T::ProducerId producerId,std::set<std::string>& contentTimeList);
    virtual int     _getContentTimeRangeByProducerAndGenerationId(T::SessionId sessionId,T::ProducerId producerId,T::GenerationId generationId,time_t& startTime,time_t& endTime);
    virtual int     _getContentTimeRangeByGenerationId(T::SessionId sessionId,T::GenerationId generationId,time_t& startTime,time_t& endTime);

    virtual int     _getContentCount(T::SessionId sessionId,uint& count);
    virtual int     _getHashByProducerId(T::SessionId sessionId,T::ProducerId producerId,UInt64 & hash);

    virtual int     _getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList);

    virtual int     _updateCachedFiles(T::SessionId sessionId,std::set<T::FileId>& fileIdList);

  protected:

    T::EventId      addEvent(uint eventType,UInt64 id1,UInt64 id2,UInt64 id3,UInt64 flags);
    virtual bool    isSessionValid(T::SessionId sessionId);

    virtual void    readContentList(uint contentStorageIndex,ContentServer_sptr contentStorage);
    virtual void    readFileList(uint contentStorageIndex,ContentServer_sptr contentStorage);
    virtual void    readProducerList(uint contentStorageIndex,ContentServer_sptr contentStorage);
    virtual void    readGenerationList(uint contentStorageIndex,ContentServer_sptr contentStorage);
    virtual void    readGeometryList(uint contentStorageIndex,ContentServer_sptr contentStorage);

    virtual void    event_clear(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_updateLoopStart(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_updateLoopEnd(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_contentServerReload(uint contentStorageIndex,T::EventInfo& eventInfo);

    virtual void    event_producerAdded(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_producerDeleted(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_producerUpdated(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_producerListDeletedBySourceId(uint contentStorageIndex,T::EventInfo& eventInfo);

    virtual void    event_generationAdded(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_generationDeleted(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_generationStatusChanged(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_generationUpdated(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_generationListDeletedByProducerId(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_generationListDeletedBySourceId(uint contentStorageIndex,T::EventInfo& eventInfo);

    virtual void    event_geometryAdded(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_geometryDeleted(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_geometryStatusChanged(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_geometryUpdated(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_geometryListDeletedByProducerId(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_geometryListDeletedByGenerationId(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_geometryListDeletedBySourceId(uint contentStorageIndex,T::EventInfo& eventInfo);

    virtual void    event_fileAdded(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_fileDeleted(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_fileUpdated(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_fileListDeletedByProducerId(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_fileListDeletedByGenerationId(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_fileListDeletedBySourceId(uint contentStorageIndex,T::EventInfo& eventInfo);

    virtual void    event_contentListDeletedByFileId(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_contentListDeletedByProducerId(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_contentListDeletedByGenerationId(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_contentListDeletedBySourceId(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_contentAdded(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_contentDeleted(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    event_contentUpdated(uint contentStorageIndex,T::EventInfo& eventInfo);

    virtual void    processEvent(uint contentStorageIndex,T::EventInfo& eventInfo);
    virtual void    processEvents(bool eventThread);

    virtual void    reloadData();

    virtual void    updateContent();

    bool                   mReloadActivated;
    bool                   mShutdownRequested;
    bool                   mUpdateInProgress;
    bool                   mRequestForwardEnabled;
    T::SessionId           mSessionId;
    T::SessionId           mDataServerSessionId;
    T::FileListStorage     mFileInfoList;
    T::ProducerInfoList    mProducerInfoList;
    T::GenerationInfoList  mGenerationInfoList;
    T::GeometryInfoList    mGeometryInfoList;
    T::ContentListStorage  mContentInfoList;
    T::EventInfoList       mEventInfoList;
    time_t                 mStartTime;
    T::EventId             mEventCounter;
    pthread_t              mThread;
    ThreadLock             mEventProcessingLock;
    ModificationLock       mModificationLock;
    ModificationLock       mSearchModificationLock;
    ContentSource_vec      mContentSources;
    time_t                 mContentChangeTime;
    uint                   mFileDeleteCounter;
    uint                   mContentDeleteCounter;
    uint                   mProducerDeleteCounter;
    uint                   mGenerationDeleteCounter;
    uint                   mGeometryDeleteCounter;
    uint                   mActiveSearchStructure;
    SearchStructure*       mSearchStructurePtr[2];
    time_t                 mContentUpdateTime;
    uint                   mContentUpdateInterval;
    bool                   mContentSwapEnabled;
    uint                   mContentSwapCounter;
    ContentTimeCache       mContentTimeCache;
    ModificationLock       mContentTimeCache_modificationLock;
    std::set<uint>         mCachedFiles;
    uint                   mCachedFiles_waitTime;
    uint                   mCachedFiles_totalWaitTime;
    uint                   mCachedFiles_maxWaitTime;
    uint                   mCachedFiles_maxFirstWaitTime;
};


}
}
