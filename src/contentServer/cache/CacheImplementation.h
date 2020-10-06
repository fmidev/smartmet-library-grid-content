#pragma once

#include "../definition/ServiceInterface.h"

#include <grid-files/common/ModificationLock.h>
#include <pthread.h>





namespace SmartMet
{
namespace ContentServer
{

#define CONTENT_LIST_COUNT 8

typedef std::map<uint,std::set<std::string>> ContentTimeCache;


class SearchStructure
{
  public:
    T::ProducerInfoList    mProducerInfoList;
    T::GenerationInfoList  mGenerationInfoList;
    T::FileInfoList        mFileInfoList;
    T::FileInfoList        mFileInfoListByName;
    T::ContentInfoList     mContentInfoList[CONTENT_LIST_COUNT];
};

typedef boost::shared_ptr<SearchStructure> SearchStructure_sptr;



class CacheImplementation : public ServiceInterface
{
  public:
                    CacheImplementation();
    virtual         ~CacheImplementation();

    virtual void    init(T::SessionId sessionId,ServiceInterface *contentStorage,uint contentSortingFlags);
    virtual void    startEventProcessing();
    virtual void    setEventListMaxLength(uint maxLength);
    virtual void    setRequestForwardEnabled(bool enabled);
    virtual void    shutdown();
    virtual void    synchronize();

    virtual void    eventProcessingThread();

  protected:

    virtual int     _clear(T::SessionId sessionId);
    virtual int     _reload(T::SessionId sessionId);

    virtual int     _addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo);
    virtual int     _deleteProducerInfoById(T::SessionId sessionId,uint producerId);
    virtual int     _deleteProducerInfoByName(T::SessionId sessionId,std::string producerName);
    virtual int     _deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId);
    virtual int     _getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo);
    virtual int     _getProducerInfoByName(T::SessionId sessionId,std::string producerName,T::ProducerInfo& producerInfo);
    virtual int     _getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList);
    virtual int     _getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList);
    virtual int     _getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList);
    virtual int     _getProducerInfoCount(T::SessionId sessionId,uint& count);
    virtual int     _getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list);
    virtual int     _getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list);
    virtual int     _getProducerParameterListByProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list);

    virtual int     _addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo);
    virtual int     _deleteGenerationInfoById(T::SessionId sessionId,uint generationId);
    virtual int     _deleteGenerationInfoByName(T::SessionId sessionId,std::string generationName);
    virtual int     _deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<uint>& generationIdList);
    virtual int     _deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId);
    virtual int     _deleteGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName);
    virtual int     _deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId);
    virtual int     _getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list);
    virtual int     _getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo);
    virtual int     _getGenerationInfoByName(T::SessionId sessionId,std::string generationName,T::GenerationInfo& generationInfo);
    virtual int     _getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList);
    virtual int     _getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList);
    virtual int     _getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList);
    virtual int     _getGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName,T::GenerationInfoList& generationInfoList);
    virtual int     _getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList);
    virtual int     _getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,uchar generationStatus,T::GenerationInfo& generationInfo);
    virtual int     _getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,std::string producerName,uchar generationStatus,T::GenerationInfo& generationInfo);
    virtual int     _getGenerationInfoCount(T::SessionId sessionId,uint& count);
    virtual int     _setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,uchar status);
    virtual int     _setGenerationInfoStatusByName(T::SessionId sessionId,std::string generationName,uchar status);

    virtual int     _addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo);
    virtual int     _addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList);
    virtual int     _addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList);
    virtual int     _deleteFileInfoById(T::SessionId sessionId,uint fileId);
    virtual int     _deleteFileInfoByName(T::SessionId sessionId,std::string filename);
    virtual int     _deleteFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags);
    virtual int     _deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId);
    virtual int     _deleteFileInfoListByProducerName(T::SessionId sessionId,std::string producerName);
    virtual int     _deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId);
    virtual int     _deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,std::string forecastTime);
    virtual int     _deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList);
    virtual int     _deleteFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName);
    virtual int     _deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId);
    virtual int     _deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList);
    virtual int     _getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo);
    virtual int     _getFileInfoByName(T::SessionId sessionId,std::string filename,T::FileInfo& fileInfo);
    virtual int     _getFileInfoList(T::SessionId sessionId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
    virtual int     _getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::FileInfoList& fileInfoList);
    virtual int     _getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
    virtual int     _getFileInfoListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
    virtual int     _getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
    virtual int     _getFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
    virtual int     _getFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
    virtual int     _getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList);
    virtual int     _getFileInfoCount(T::SessionId sessionId,uint& count);
    virtual int     _getFileInfoCountByProducerId(T::SessionId sessionId,uint producerId,uint& count);
    virtual int     _getFileInfoCountByGenerationId(T::SessionId sessionId,uint generationId,uint& count);
    virtual int     _getFileInfoCountBySourceId(T::SessionId sessionId,uint sourceId,uint& count);

    virtual int     _addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo);
    virtual int     _getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo);
    virtual int     _getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,uint maxRecords,T::EventInfoList& eventInfoList);
    virtual int     _getEventInfoCount(T::SessionId sessionId,uint& count);

    virtual int     _addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo);
    virtual int     _addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList);
    virtual int     _deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex);
    virtual int     _deleteContentListByFileId(T::SessionId sessionId,uint fileId);
    virtual int     _deleteContentListByFileName(T::SessionId sessionId,std::string filename);
    virtual int     _deleteContentListByGroupFlags(T::SessionId sessionId,uint groupFlags);
    virtual int     _deleteContentListByProducerId(T::SessionId sessionId,uint producerId);
    virtual int     _deleteContentListByProducerName(T::SessionId sessionId,std::string producerName);
    virtual int     _deleteContentListByGenerationId(T::SessionId sessionId,uint generationId);
    virtual int     _deleteContentListByGenerationName(T::SessionId sessionId,std::string generationName);
    virtual int     _deleteContentListBySourceId(T::SessionId sessionId,uint sourceId);

    virtual int     _getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo);
    virtual int     _getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByFileName(T::SessionId sessionId,std::string filename,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,std::string generationName,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList);

    virtual int     _getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByParameterAndGenerationName(T::SessionId sessionId,std::string generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByParameterAndProducerName(T::SessionId sessionId,std::string producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string forecastTime,T::ContentInfoList& contentInfoList);
    virtual int     _getContentListByRequestCounterKey(T::SessionId sessionId,ulonglong key,T::ContentInfoList& contentInfoList);

    virtual int     _getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList);

    virtual int     _getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<T::GeometryId>& geometryIdList);
    virtual int     _getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList);
    virtual int     _getContentParamKeyListByGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
    virtual int     _getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::set<std::string>& contentTimeList);
    virtual int     _getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList);
    virtual int     _getContentTimeListByProducerId(T::SessionId sessionId,uint producerId,std::set<std::string>& contentTimeList);

    virtual int     _getContentCount(T::SessionId sessionId,uint& count);
    virtual int     _getHashByProducerId(T::SessionId sessionId,uint producerId,ulonglong& hash);

    virtual int     _getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList);

    virtual int     _deleteVirtualContent(T::SessionId sessionId);
    virtual int     _updateVirtualContent(T::SessionId sessionId);

  protected:

    virtual bool    isSessionValid(T::SessionId sessionId);

    virtual void    readContentList();
    virtual void    readFileList();
    virtual void    readProducerList();
    virtual void    readGenerationList();

    virtual void    event_clear(T::EventInfo& eventInfo);
    virtual void    event_contentServerReload(T::EventInfo& eventInfo);
    virtual void    event_producerAdded(T::EventInfo& eventInfo);
    virtual void    event_producerDeleted(T::EventInfo& eventInfo);
    virtual void    event_producerListDeletedBySourceId(T::EventInfo& eventInfo);
    virtual void    event_generationAdded(T::EventInfo& eventInfo);
    virtual void    event_generationDeleted(T::EventInfo& eventInfo);
    virtual void    event_generationStatusChanged(T::EventInfo& eventInfo);
    virtual void    event_generationListDeletedByProducerId(T::EventInfo& eventInfo);
    virtual void    event_generationListDeletedBySourceId(T::EventInfo& eventInfo);
    virtual void    event_fileAdded(T::EventInfo& eventInfo);
    virtual void    event_fileDeleted(T::EventInfo& eventInfo);
    virtual void    event_fileUpdated(T::EventInfo& eventInfo);
    virtual void    event_fileListDeletedByGroupFlags(T::EventInfo& eventInfo);
    virtual void    event_fileListDeletedByProducerId(T::EventInfo& eventInfo);
    virtual void    event_fileListDeletedByGenerationId(T::EventInfo& eventInfo);
    virtual void    event_fileListDeletedBySourceId(T::EventInfo& eventInfo);
    virtual void    event_contentListDeletedByFileId(T::EventInfo& eventInfo);
    virtual void    event_contentListDeletedByGroupFlags(T::EventInfo& eventInfo);
    virtual void    event_contentListDeletedByProducerId(T::EventInfo& eventInfo);
    virtual void    event_contentListDeletedByGenerationId(T::EventInfo& eventInfo);
    virtual void    event_contentListDeletedBySourceId(T::EventInfo& eventInfo);
    virtual void    event_contentAdded(T::EventInfo& eventInfo);
    virtual void    event_contentDeleted(T::EventInfo& eventInfo);
    virtual void    event_deleteVirtualContent(T::EventInfo& eventInfo);
    virtual void    event_updateVirtualContent(T::EventInfo& eventInfo);

    virtual void    processEvent(T::EventInfo& eventInfo);
    virtual void    processEvents(bool eventThread);

    virtual void    reloadData();
    virtual void    saveData();

    virtual void    swapData();

    bool                   mReloadActivated;
    bool                   mShutdownRequested;
    bool                   mUpdateInProgress;
    bool                   mRequestForwardEnabled;
    T::SessionId           mSessionId;
    T::EventId             mLastProcessedEventId;
    T::FileInfoList        mFileInfoList;
    T::ProducerInfoList    mProducerInfoList;
    T::GenerationInfoList  mGenerationInfoList;
    T::ContentInfoList     mContentInfoList;
    T::EventInfoList       mEventInfoList;
    bool                   mContentInfoListEnabled[CONTENT_LIST_COUNT];
    time_t                 mStartTime;
    pthread_t              mThread;
    ThreadLock             mEventProcessingLock;
    ModificationLock       mModificationLock;
    ServiceInterface*      mContentStorage;
    time_t                 mContentStorageStartTime;
    bool                   mSaveEnabled;
    std::string            mSaveDir;
    uint                   mContentSortingFlags;
    uint                   mProducerCount;
    uint                   mGenerationCount;
    uint                   mFileCount;
    uint                   mContentCount;
    uint                   mContentDeleteCount;
    uint                   mFileDeleteCount;
    ContentTimeCache       mContentTimeCache;
    SearchStructure_sptr   mSearchStructureSptr;
    time_t                 mDataSwapTime;
};


}
}
