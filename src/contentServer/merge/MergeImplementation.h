#pragma once

#include "../definition/ServiceInterface.h"
#include "../definition/ContentListStorage.h"
#include "../definition/FileListStorage.h"
#include "../cache/CacheImplementation.h"

#include <grid-files/common/ModificationLock.h>
#include <pthread.h>
#include <atomic>
#include <memory>
#include <shared_mutex>
#include <boost/smart_ptr/atomic_shared_ptr.hpp>




namespace SmartMet
{
namespace ContentServer
{


//#define CONTENT_LIST_COUNT 2

using ContentTimeCache = std::map<T::GenerationId,std::set<std::string>>;  //!< Map from generation id to a set of forecast time strings.

using ContentServer_sptr = std::shared_ptr<ContentServer::ServiceInterface>;      //!< Shared pointer to a ServiceInterface backend.
using ContentServer_sptr_vec = std::vector<ContentServer_sptr>;                    //!< Ordered collection of ServiceInterface backends.

// ====================================================================================
/*! \brief Tracks the sync state for one backend ServiceInterface in a merge scenario.
 *
 *  Holds the shared pointer to the backend, the start time used to detect restarts,
 *  the last event id processed from that backend, and a content hash for change detection. */
// ====================================================================================
class ContentSource
{
  public:
    ContentSource()
    = default;

    ContentServer_sptr     mContentStorage;          //!< Shared pointer to the backend ServiceInterface.
    time_t                 mContentStorageStartTime{0}; //!< Start time of the backend, used to detect restarts.
    T::EventId             mLastProcessedEventId{0};    //!< Highest event id already processed from this backend.
    std::size_t            mHash{0};                    //!< Content hash used to detect bulk changes.
};

using ContentSource_vec = std::vector<ContentSource>;  //!< Collection of content sources managed by the merge layer.

// ====================================================================================
/*! \brief Merges content from multiple ServiceInterface backends into a unified view.
 *
 *  Subscribes to events from each registered backend and maintains a single merged
 *  in-memory index of producers, generations, geometries, files and content records.
 *  Read queries are served from the merged index; write operations are forwarded to
 *  the appropriate backend. */
// ====================================================================================
class MergeImplementation : public ServiceInterface
{
  public:
                    MergeImplementation();
    ~MergeImplementation() override;
    MergeImplementation(const MergeImplementation&) = delete;
    MergeImplementation& operator=(const MergeImplementation&) = delete;

    virtual void    init(T::SessionId sessionId,T::SessionId dataServerSessionId,ContentServer_sptr_vec& contentStorages);
    bool    isReady() override;
    virtual void    startEventProcessing();
    virtual void    setEventListMaxLength(uint maxLength);
    virtual void    setContentSwap(uint fileCacheMaxFirstWaitTime,uint fileCacheMaxWaitTime);
    virtual void    setContentUpdateInterval(uint intervalInSec);
    void    shutdown() override;

    void    getCacheStats(Fmi::Cache::CacheStatistics& statistics) const override;
    void    getStateAttributes(std::shared_ptr<T::AttributeNode> parent) override;

    virtual void    eventProcessingThread();

  protected:

    int     _clear(T::SessionId sessionId) override;
    int     _reload(T::SessionId sessionId) override;
    int     _getContentChangeTime(T::SessionId sessionId,time_t& changeTime) override;

    int     _addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo) override;
    int     _deleteProducerInfoById(T::SessionId sessionId,T::ProducerId producerId) override;
    int     _deleteProducerInfoByName(T::SessionId sessionId,const std::string& producerName) override;
    int     _deleteProducerInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId) override;
    int     _getProducerInfoById(T::SessionId sessionId,T::ProducerId producerId,T::ProducerInfo& producerInfo) override;
    int     _getProducerInfoByName(T::SessionId sessionId,const std::string& producerName,T::ProducerInfo& producerInfo) override;
    int     _getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList) override;
    int     _getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList) override;
    int     _getProducerInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::ProducerInfoList& producerInfoList) override;
    int     _getProducerInfoCount(T::SessionId sessionId,uint& count) override;
    int     _getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list) override;
    int     _getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list) override;
    int     _getProducerParameterListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list) override;
    int     _setProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo) override;

    int     _addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo) override;
    int     _deleteGenerationInfoById(T::SessionId sessionId,T::GenerationId generationId) override;
    int     _deleteGenerationInfoByName(T::SessionId sessionId,const std::string& generationName) override;
    int     _deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<T::GenerationId>& generationIdList) override;
    int     _deleteGenerationInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId) override;
    int     _deleteGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName) override;
    int     _deleteGenerationInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId) override;
    int     _getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list) override;
    int     _getGenerationInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GenerationInfo& generationInfo) override;
    int     _getGenerationInfoByName(T::SessionId sessionId,const std::string& generationName,T::GenerationInfo& generationInfo) override;
    int     _getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList) override;
    int     _getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList) override;
    int     _getGenerationInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::GenerationInfoList& generationInfoList) override;
    int     _getGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::GenerationInfoList& generationInfoList) override;
    int     _getGenerationInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::GenerationInfoList& generationInfoList) override;
    int     _getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,T::ProducerId producerId,uchar generationStatus,T::GenerationInfo& generationInfo) override;
    int     _getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,const std::string& producerName,uchar generationStatus,T::GenerationInfo& generationInfo) override;
    int     _getGenerationInfoCount(T::SessionId sessionId,uint& count) override;
    int     _setGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo) override;
    int     _setGenerationInfoStatusById(T::SessionId sessionId,T::GenerationId generationId,uchar status) override;
    int     _setGenerationInfoStatusByName(T::SessionId sessionId,const std::string& generationName,uchar status) override;

    int     _addGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo) override;
    int     _deleteGeometryInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId) override;
    int     _deleteGeometryInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId) override;
    int     _deleteGeometryInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId) override;
    int     _deleteGeometryInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId) override;
    int     _getGeometryInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo) override;
    int     _getGeometryInfoList(T::SessionId sessionId,T::GeometryInfoList& geometryInfoList) override;
    int     _getGeometryInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryInfoList& geometryInfoList) override;
    int     _getGeometryInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::GeometryInfoList& geometryInfoList) override;
    int     _getGeometryInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::GeometryInfoList& geometryInfoList) override;
    int     _getGeometryInfoCount(T::SessionId sessionId,uint& count) override;
    int     _setGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo) override;
    int     _setGeometryInfoStatusById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,uchar status) override;

    int     _addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo) override;
    int     _addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList) override;
    int     _addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList) override;
    int     _deleteFileInfoById(T::SessionId sessionId,T::FileId fileId) override;
    int     _deleteFileInfoByName(T::SessionId sessionId,const std::string& filename) override;
    int     _deleteFileInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId) override;
    int     _deleteFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName) override;
    int     _deleteFileInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId) override;
    int     _deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,time_t forecastTime) override;
    int     _deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList) override;
    int     _deleteFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName) override;
    int     _deleteFileInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId) override;
    int     _deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<T::FileId>& fileIdList) override;
    int     _getFileInfoById(T::SessionId sessionId,T::FileId fileId,T::FileInfo& fileInfo) override;
    int     _getFileInfoByName(T::SessionId sessionId,const std::string& filename,T::FileInfo& fileInfo) override;
    int     _getFileInfoList(T::SessionId sessionId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList) override;
    int     _getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<T::FileId>& fileIdList,T::FileInfoList& fileInfoList) override;
    int     _getFileInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList) override;
    int     _getFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList) override;
    int     _getFileInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList) override;
    int     _getFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList) override;
    int     _getFileInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList) override;
    int     _getFileInfoCount(T::SessionId sessionId,uint& count) override;
    int     _getFileInfoCountByProducerId(T::SessionId sessionId,T::ProducerId producerId,uint& count) override;
    int     _getFileInfoCountByGenerationId(T::SessionId sessionId,T::GenerationId generationId,uint& count) override;
    int     _getFileInfoCountBySourceId(T::SessionId sessionId,T::SourceId sourceId,uint& count) override;
    int     _setFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo) override;

    int     _addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo) override;
    int     _getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo) override;
    int     _getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList) override;
    int     _getEventInfoCount(T::SessionId sessionId,uint& count) override;

    int     _addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo) override;
    int     _setContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo) override;
    int     _addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList) override;
    int     _deleteContentInfo(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex) override;
    int     _deleteContentListByFileId(T::SessionId sessionId,T::FileId fileId) override;
    int     _deleteContentListByFileName(T::SessionId sessionId,const std::string& filename) override;
    int     _deleteContentListByProducerId(T::SessionId sessionId,T::ProducerId producerId) override;
    int     _deleteContentListByProducerName(T::SessionId sessionId,const std::string& producerName) override;
    int     _deleteContentListByGenerationId(T::SessionId sessionId,T::GenerationId generationId) override;
    int     _deleteContentListByGenerationName(T::SessionId sessionId,const std::string& generationName) override;
    int     _deleteContentListBySourceId(T::SessionId sessionId,T::SourceId sourceId) override;

    int     _getContentInfo(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::ContentInfo& contentInfo) override;
    int     _getContentList(T::SessionId sessionId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList) override;
    int     _getContentListByFileId(T::SessionId sessionId,T::FileId fileId,T::ContentInfoList& contentInfoList) override;
    int     _getContentListByFileIdList(T::SessionId sessionId,std::vector<T::FileId>& fileIdList,T::ContentInfoList& contentInfoList) override;
    int     _getContentListByFileName(T::SessionId sessionId,const std::string& filename,T::ContentInfoList& contentInfoList) override;
    int     _getContentListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList) override;
    int     _getContentListByProducerName(T::SessionId sessionId,const std::string& producerName,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList) override;
    int     _getContentListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList) override;
    int     _getContentListByGenerationName(T::SessionId sessionId,const std::string& generationName,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList) override;
    int     _getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,T::GenerationId generationId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList) override;
    int     _getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList) override;
    int     _getContentListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList) override;

    int     _getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList) override;
    int     _getContentListByParameterAndGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList) override;
    int     _getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList) override;
    int     _getContentListByParameterAndProducerId(T::SessionId sessionId,T::ProducerId producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList) override;
    int     _getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList) override;
    int     _getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,T::ContentInfoList& contentInfoList) override;

    int     _getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList) override;

    int     _getContentGeometryIdListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,std::set<T::GeometryId>& geometryIdList) override;
    int     _getContentLevelListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList) override;
    int     _getContentLevelListByParameterGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,std::string parameterKey,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList) override;
    int     _getContentParamListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ContentInfoList& contentParamList) override;
    int     _getContentParamKeyListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList) override;
    int     _getContentParamKeyListByGenerationAndGeometryId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList) override;
    int     _getContentParamKeyListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList) override;
    int     _getContentTimeListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,std::set<std::string>& contentTimeList) override;
    int     _getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList) override;
    int     _getContentTimeListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<std::string>& contentTimeList) override;
    int     _getContentTimeListByProducerId(T::SessionId sessionId,T::ProducerId producerId,std::set<std::string>& contentTimeList) override;
    int     _getContentTimeRangeByProducerAndGenerationId(T::SessionId sessionId,T::ProducerId producerId,T::GenerationId generationId,time_t& startTime,time_t& endTime) override;
    int     _getContentTimeRangeByGenerationId(T::SessionId sessionId,T::GenerationId generationId,time_t& startTime,time_t& endTime) override;

    int     _getContentCount(T::SessionId sessionId,uint& count) override;
    int     _getHashByProducerId(T::SessionId sessionId,T::ProducerId producerId,UInt64 & hash) override;

    int     _getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList) override;

    int     _updateCachedFiles(T::SessionId sessionId,std::set<T::FileId>& fileIdList) override;

  

    T::EventId      addEvent(uint eventType,UInt64 id1,UInt64 id2,UInt64 id3,UInt64 flags);
    /*! \brief Returns the active search structure. The returned reference keeps it alive even if
        a content update replaces it meanwhile. */
    SearchStructure_sptr activeSearchStructure() const;
    bool                 hasInvalidIntegrity(SearchStructure& ss,T::ContentInfo& cInfo);
    void                 addMergedContent(T::ContentInfo *contentInfo);
    void                 addEventFileAndContents(uint contentStorageIndex,T::FileId newFileId,const std::string& eventData);
    void                 addStoredFileAndContents(uint contentStorageIndex,T::FileId newFileId,T::EventInfo& eventInfo);

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

    bool                   mReloadActivated;              //!< True when a full reload from all backends has been triggered.
    bool                   mShutdownRequested;            //!< True after shutdown() has been called.
    bool                   mUpdateInProgress;             //!< True while a bulk content update is running.
    bool                   mRequestForwardEnabled = false;        //!< When true, write operations are forwarded to the backends.
    T::SessionId           mSessionId;                    //!< Session identifier used for backend queries.
    T::SessionId           mDataServerSessionId = 0;          //!< Session identifier passed to the data server.
    T::FileListStorage     mFileInfoList;                 //!< Merged file records.
    T::ProducerInfoList    mProducerInfoList;             //!< Merged producer records.
    T::GenerationInfoList  mGenerationInfoList;           //!< Merged generation records.
    T::GeometryInfoList    mGeometryInfoList;             //!< Merged geometry records.
    T::ContentListStorage  mContentInfoList;              //!< Merged content records.
    T::EventInfoList       mEventInfoList;                //!< Local event log aggregated from all backends.
    time_t                 mStartTime;                    //!< Unix timestamp when the implementation was initialised.
    T::EventId             mEventCounter;                 //!< Counter for locally generated event ids.
    pthread_t              mThread;                       //!< Handle of the background event-processing thread.
    ThreadLock             mEventProcessingLock;          //!< Prevents concurrent event-processing runs.
    ModificationLock       mModificationLock;             //!< Guards mutable list members during updates.
    ModificationLock       mSearchModificationLock;       //!< Guards the active search-structure pointer swap.
    ContentSource_vec      mContentSources;               //!< Registered backends and their per-source sync state.
    time_t                 mContentChangeTime;            //!< Time of the most recent merged content change.
    uint                   mFileDeleteCounter;            //!< Running count of file deletions since last reload.
    uint                   mContentDeleteCounter;         //!< Running count of content deletions since last reload.
    uint                   mProducerDeleteCounter;        //!< Running count of producer deletions since last reload.
    uint                   mGenerationDeleteCounter;      //!< Running count of generation deletions since last reload.
    uint                   mGeometryDeleteCounter;        //!< Running count of geometry deletions since last reload.
    uint                   mActiveSearchStructure;        //!< Index (0 or 1) of the currently active SearchStructure.
    SearchStructure_sptr   mSearchStructurePtr[2];        //!< Double-buffered search structures. Readers hold a reference for the duration of a request.
    mutable std::shared_mutex mSearchStructurePtrLock;    //!< Guards mSearchStructurePtr and mActiveSearchStructure (held only while copying or replacing a pointer).
    time_t                 mContentUpdateTime;            //!< Time of the last content update.
    bool                   mContentUpdateRequired;        //!< True when a content refresh is pending.
    uint                   mContentUpdateInterval;        //!< Minimum seconds between successive content updates.
    bool                   mContentSwapEnabled = false;           //!< True when double-buffered search-structure swapping is active.
    uint                   mContentSwapCounter;           //!< Counter used to throttle search-structure swaps.
    ContentTimeCache       mContentTimeCache;             //!< Per-generation cache of available forecast time strings.
    ModificationLock       mContentTimeCache_modificationLock;  //!< Guards mContentTimeCache during updates.
    std::set<uint>         mCachedFiles;                  //!< Set of file ids whose content is explicitly cached.
    uint                   mCachedFiles_waitTime;         //!< Current wait time before rechecking a cached file.
    uint                   mCachedFiles_totalWaitTime;    //!< Accumulated wait time for all cached files.
    uint                   mCachedFiles_maxWaitTime;      //!< Maximum allowed wait time between cached-file rechecks.
    uint                   mCachedFiles_maxFirstWaitTime; //!< Maximum wait time before the first recheck of a cached file.
};


}
}
