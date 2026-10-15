#pragma once

#include "../definition/ServiceInterface.h"

#include <grid-files/common/ModificationLock.h>
#include <pthread.h>



namespace SmartMet
{
namespace ContentServer
{

#define CONTENT_LIST_COUNT 2

// ====================================================================================
/*! \brief Lightweight pure in-memory ContentServer implementation without event sync.
 *
 *  Stores producers, generations, geometries, files and content records entirely in RAM.
 *  Unlike CacheImplementation it does not monitor an external master; it can optionally
 *  load initial data from disk, save periodic snapshots, and synchronise content between
 *  its internal structures.  Suitable for small standalone deployments or unit tests. */
// ====================================================================================
class MemoryImplementation : public ServiceInterface
{
  public:
                    MemoryImplementation();
     ~MemoryImplementation() override;
     MemoryImplementation(const MemoryImplementation&) = delete;
     MemoryImplementation& operator=(const MemoryImplementation&) = delete;

     virtual void   init(bool contentLoadEnabled,bool contentSaveEnabled,bool contentSyncEnabled,bool eventsEnabled,const std::string& contentDir,uint contentSaveInterval);
     void   shutdown() override;
     virtual void   startSyncProcessing();
     virtual void   syncProcessingThread();
     virtual void   setEventListMaxLength(uint maxLength);

   protected:

     int    _clear(T::SessionId sessionId) override;
     int    _reload(T::SessionId sessionId) override;
     int    _getContentChangeTime(T::SessionId sessionId,time_t& changeTime) override;

     int    _addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo) override;
     int    _deleteProducerInfoById(T::SessionId sessionId,T::ProducerId producerId) override;
     int    _deleteProducerInfoByName(T::SessionId sessionId,const std::string& producerName) override;
     int    _deleteProducerInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId) override;
     int    _getProducerInfoById(T::SessionId sessionId,T::ProducerId producerId,T::ProducerInfo& producerInfo) override;
     int    _getProducerInfoByName(T::SessionId sessionId,const std::string& producerName,T::ProducerInfo& producerInfo) override;
     int    _getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList) override;
     int    _getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList) override;
     int    _getProducerInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::ProducerInfoList& producerInfoList) override;
     int    _getProducerInfoCount(T::SessionId sessionId,uint& count) override;
     int    _getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list) override;
     int    _getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list) override;
     int    _getProducerParameterListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list) override;
     int    _setProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo) override;

     int    _addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo) override;
     int    _deleteGenerationInfoById(T::SessionId sessionId,T::GenerationId generationId) override;
     int    _deleteGenerationInfoByName(T::SessionId sessionId,const std::string& generationName) override;
     int    _deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<T::GenerationId>& generationIdList) override;
     int    _deleteGenerationInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId) override;
     int    _deleteGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName) override;
     int    _deleteGenerationInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId) override;
     int    _getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list) override;
     int    _getGenerationInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GenerationInfo& generationInfo) override;
     int    _getGenerationInfoByName(T::SessionId sessionId,const std::string& generationName,T::GenerationInfo& generationInfo) override;
     int    _getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList) override;
     int    _getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList) override;
     int    _getGenerationInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::GenerationInfoList& generationInfoList) override;
     int    _getGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::GenerationInfoList& generationInfoList) override;
     int    _getGenerationInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::GenerationInfoList& generationInfoList) override;
     int    _getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,T::ProducerId producerId,uchar generationStatus,T::GenerationInfo& generationInfo) override;
     int    _getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,const std::string& producerName,uchar generationStatus,T::GenerationInfo& generationInfo) override;
     int    _getGenerationInfoCount(T::SessionId sessionId,uint& count) override;
     int    _setGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo) override;
     int    _setGenerationInfoStatusById(T::SessionId sessionId,T::GenerationId generationId,uchar status) override;
     int    _setGenerationInfoStatusByName(T::SessionId sessionId,const std::string& generationName,uchar status) override;

     int    _addGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo) override;
     int    _deleteGeometryInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId) override;
     int    _deleteGeometryInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId) override;
     int    _deleteGeometryInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId) override;
     int    _deleteGeometryInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId) override;
     int    _getGeometryInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo) override;
     int    _getGeometryInfoList(T::SessionId sessionId,T::GeometryInfoList& geometryInfoList) override;
     int    _getGeometryInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryInfoList& geometryInfoList) override;
     int    _getGeometryInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::GeometryInfoList& geometryInfoList) override;
     int    _getGeometryInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::GeometryInfoList& geometryInfoList) override;
     int    _getGeometryInfoCount(T::SessionId sessionId,uint& count) override;
     int    _setGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo) override;
     int    _setGeometryInfoStatusById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,uchar status) override;

     int    _addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo) override;
     int    _addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList) override;
     int    _addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList) override;
     int    _deleteFileInfoById(T::SessionId sessionId,T::FileId fileId) override;
     int    _deleteFileInfoByName(T::SessionId sessionId,const std::string& filename) override;
     int    _deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList) override;
     int    _deleteFileInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId) override;
     int    _deleteFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName) override;
     int    _deleteFileInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId) override;
     int    _deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,time_t forecastTime) override;
     int    _deleteFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName) override;
     int    _deleteFileInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId) override;
     int    _deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<T::FileId>& fileIdList) override;
     int    _getFileInfoById(T::SessionId sessionId,T::FileId fileId,T::FileInfo& fileInfo) override;
     int    _getFileInfoByName(T::SessionId sessionId,const std::string& filename,T::FileInfo& fileInfo) override;
     int    _getFileInfoList(T::SessionId sessionId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList) override;
     int    _getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<T::FileId>& fileIdList,T::FileInfoList& fileInfoList) override;
     int    _getFileInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList) override;
     int    _getFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList) override;
     int    _getFileInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList) override;
     int    _getFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList) override;
     int    _getFileInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList) override;
     int    _getFileInfoCount(T::SessionId sessionId,uint& count) override;
     int    _getFileInfoCountByProducerId(T::SessionId sessionId,T::ProducerId producerId,uint& count) override;
     int    _getFileInfoCountByGenerationId(T::SessionId sessionId,T::GenerationId generationId,uint& count) override;
     int    _getFileInfoCountBySourceId(T::SessionId sessionId,T::SourceId sourceId,uint& count) override;
     int    _setFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo) override;

     int    _addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo) override;
     int    _getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo) override;
     int    _getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList) override;
     int    _getEventInfoCount(T::SessionId sessionId,uint& count) override;

     int    _addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo) override;
     int    _setContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo) override;
     int    _addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList) override;
     int    _deleteContentInfo(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex) override;
     int    _deleteContentListByFileId(T::SessionId sessionId,T::FileId fileId) override;
     int    _deleteContentListByFileName(T::SessionId sessionId,const std::string& filename) override;
     int    _deleteContentListByProducerId(T::SessionId sessionId,T::ProducerId producerId) override;
     int    _deleteContentListByProducerName(T::SessionId sessionId,const std::string& producerName) override;
     int    _deleteContentListByGenerationId(T::SessionId sessionId,T::GenerationId generationId) override;
     int    _deleteContentListByGenerationName(T::SessionId sessionId,const std::string& generationName) override;
     int    _deleteContentListBySourceId(T::SessionId sessionId,T::SourceId sourceId) override;

     int    _getContentInfo(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::ContentInfo& contentInfo) override;
     int    _getContentList(T::SessionId sessionId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList) override;
     int    _getContentListByFileId(T::SessionId sessionId,T::FileId fileId,T::ContentInfoList& contentInfoList) override;
     int    _getContentListByFileIdList(T::SessionId sessionId,std::vector<T::FileId>& fileIdList,T::ContentInfoList& contentInfoList) override;
     int    _getContentListByFileName(T::SessionId sessionId,const std::string& filename,T::ContentInfoList& contentInfoList) override;
     int    _getContentListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList) override;
     int    _getContentListByProducerName(T::SessionId sessionId,const std::string& producerName,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList) override;
     int    _getContentListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList) override;
     int    _getContentListByGenerationName(T::SessionId sessionId,const std::string& generationName,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList) override;
     int    _getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,T::GenerationId generationId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList) override;
     int    _getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList) override;
     int    _getContentListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList) override;

     int    _getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList) override;
     int    _getContentListByParameterAndGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList) override;
     int    _getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList) override;
     int    _getContentListByParameterAndProducerId(T::SessionId sessionId,T::ProducerId producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList) override;
     int    _getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList) override;
     int    _getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,T::ContentInfoList& contentInfoList) override;

     int    _getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList) override;

     int    _getContentGeometryIdListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,std::set<T::GeometryId>& geometryIdList) override;
     int    _getContentLevelListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList) override;
     int    _getContentLevelListByParameterGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,std::string parameterKey,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList) override;
     int    _getContentParamListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ContentInfoList& contentParamList) override;
     int    _getContentParamKeyListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList) override;
     int    _getContentParamKeyListByGenerationAndGeometryId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList) override;
     int    _getContentParamKeyListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList) override;
     int    _getContentTimeListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,std::set<std::string>& contentTimeList) override;
     int    _getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList) override;
     int    _getContentTimeListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<std::string>& contentTimeList) override;
     int    _getContentTimeListByProducerId(T::SessionId sessionId,T::ProducerId producerId,std::set<std::string>& contentTimeList) override;
     int    _getContentTimeRangeByProducerAndGenerationId(T::SessionId sessionId,T::ProducerId producerId,T::GenerationId generationId,time_t& startTime,time_t& endTime) override;
     int    _getContentTimeRangeByGenerationId(T::SessionId sessionId,T::GenerationId generationId,time_t& startTime,time_t& endTime) override;

     int    _getContentCount(T::SessionId sessionId,uint& count) override;
     int    _getHashByProducerId(T::SessionId sessionId,T::ProducerId producerId,UInt64 & hash) override;

     int    _getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList) override;

   

     virtual bool   isSessionValid(T::SessionId sessionId);
     T::EventId     addEvent(uint eventType,UInt64 id1,UInt64 id2,UInt64 id3,UInt64 flags);
     virtual void   saveData();
     UInt64         updateProducerHashNoLock(T::ProducerId producerId);


     virtual void   readContentList();
     virtual void   readFileList();
     virtual void   readProducerList();
     virtual void   readGenerationList();
     virtual void   readGeometryList();

     virtual bool   syncProducerList();
     virtual bool   syncGenerationList();
     virtual bool   syncGeometryList();
     virtual bool   syncFileList();
     virtual bool   syncContentList();

     bool                   mReloadActivated;    //!< True when a full data reload has been requested.
     bool                   mShutdownRequested;  //!< True after shutdown() has been called.
     bool                   mUpdateInProgress;   //!< True while a bulk content update is running.
     T::SessionId           mSessionId = 0;          //!< Session identifier used for internal operations.

     T::FileInfoList        mFileInfoList;        //!< File records indexed by file id.
     T::FileInfoList        mFileInfoListByName;  //!< File records indexed by filename.
     T::ProducerInfoList    mProducerInfoList;    //!< In-memory producer records.
     T::GenerationInfoList  mGenerationInfoList;  //!< In-memory generation records.
     T::GeometryInfoList    mGeometryInfoList;    //!< In-memory geometry records.
     T::EventInfoList       mEventInfoList;       //!< In-memory event log.
     T::ContentInfoList     mContentInfoList[CONTENT_LIST_COUNT];  //!< Double-buffered content records.

     time_t                 mStartTime;           //!< Unix timestamp when the implementation was initialised.
     pthread_t              mThread;              //!< Handle of the optional background sync thread.
     ThreadLock             mEventProcessingLock; //!< Prevents concurrent event-processing runs.
     ModificationLock       mModificationLock;    //!< Guards mutable list members.
     bool                   mEventsEnabled;       //!< True if the event log is maintained.

     bool                   mContentLoadEnabled;  //!< True if initial content is loaded from disk on startup.
     bool                   mContentSaveEnabled;  //!< True if periodic snapshots are written to disk.
     bool                   mContentSyncEnabled;  //!< True if synchronisation between internal lists is active.
     std::string            mContentDir;          //!< Directory used for persistent snapshots.
     time_t                 mLastSaveTime;        //!< Unix timestamp of the most recent snapshot save.
     time_t                 mContentChangeTime;   //!< Time of the most recent content change.
     uint                   mContentSaveInterval; //!< Minimum seconds between successive disk snapshots.

     uint                   mProducerCount;       //!< Total number of stored producers.
     uint                   mGenerationCount;     //!< Total number of stored generations.
     uint                   mGeometryCount;       //!< Total number of stored geometries.
     uint                   mFileCount;           //!< Total number of stored files.
     uint                   mContentCount;        //!< Total number of stored content records.

     T::ProducerId          mMaxProducerId;       //!< Highest producer id assigned so far.
     T::GenerationId        mMaxGenerationId;     //!< Highest generation id assigned so far.
     T::FileId              mMaxFileId;           //!< Highest file id assigned so far.
     T::EventId             mMaxEventId;          //!< Highest event id assigned so far.

     time_t                 mProducerStorage_modificationTime;   //!< Modification time of the on-disk producer snapshot.
     time_t                 mGenerationStorage_modificationTime; //!< Modification time of the on-disk generation snapshot.
     time_t                 mGeometryStorage_modificationTime;   //!< Modification time of the on-disk geometry snapshot.
     time_t                 mFileStorage_modificationTime;       //!< Modification time of the on-disk file snapshot.
     time_t                 mContentStorage_modificationTime;    //!< Modification time of the on-disk content snapshot.
};


}
}
