#pragma once

#include "../definition/ServiceInterface.h"

#include <grid-files/common/ModificationLock.h>
#include <pthread.h>

// To avoid dependency escalation to other modules we forward declare. However, we
// cannot forward declare a PGconn struct, since it is a typedef:
//
//    typedef struct pg_conn PGconn;
//
// Hence we also have to change the data type from PGconn to pg_conn, and hope that future
// changes do not break things. The only way to fix this would be to hide the PGconn object
// behind an implementation pointer so that the libpq-fe.h header would have to be only
// included in the cpp file and the "proper" PGconn object could be used. For now the code
// relies on the compiler to understand the objects to be really the same, but just not in
// this header file. Future compilers might require a reintrerpret_cast.

struct pg_conn;

namespace SmartMet
{
namespace ContentServer
{


class PostgresqlImplementation : public ServiceInterface
{
  public:
                    PostgresqlImplementation();
     virtual        ~PostgresqlImplementation();

     virtual void   init(const char *primaryConnectionString,const char *secondaryConnectionString,bool tableCreationAllowed);
     virtual void   shutdown();

   protected:

     virtual int    _clear(T::SessionId sessionId);
     virtual int    _reload(T::SessionId sessionId);
     virtual int    _getContentChangeTime(T::SessionId sessionId,time_t& changeTime);

     virtual int    _addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo);
     virtual int    _deleteProducerInfoById(T::SessionId sessionId,T::ProducerId producerId);
     virtual int    _deleteProducerInfoByName(T::SessionId sessionId,const std::string& producerName);
     virtual int    _deleteProducerInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId);
     virtual int    _getProducerInfoById(T::SessionId sessionId,T::ProducerId producerId,T::ProducerInfo& producerInfo);
     virtual int    _getProducerInfoByName(T::SessionId sessionId,const std::string& producerName,T::ProducerInfo& producerInfo);
     virtual int    _getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList);
     virtual int    _getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList);
     virtual int    _getProducerInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::ProducerInfoList& producerInfoList);
     virtual int    _getProducerInfoCount(T::SessionId sessionId,uint& count);
     virtual int    _getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list);
     virtual int    _getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list);
     virtual int    _setProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo);

     virtual int    _addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo);
     virtual int    _deleteGenerationInfoById(T::SessionId sessionId,T::GenerationId generationId);
     virtual int    _deleteGenerationInfoByName(T::SessionId sessionId,const std::string& generationName);
     virtual int    _deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<T::GenerationId>& generationIdList);
     virtual int    _deleteGenerationInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId);
     virtual int    _deleteGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName);
     virtual int    _deleteGenerationInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId);
     virtual int    _getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list);
     virtual int    _getGenerationInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GenerationInfo& generationInfo);
     virtual int    _getGenerationInfoByName(T::SessionId sessionId,const std::string& generationName,T::GenerationInfo& generationInfo);
     virtual int    _getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList);
     virtual int    _getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList);
     virtual int    _getGenerationInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::GenerationInfoList& generationInfoList);
     virtual int    _getGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::GenerationInfoList& generationInfoList);
     virtual int    _getGenerationInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::GenerationInfoList& generationInfoList);
     virtual int    _getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,T::ProducerId producerId,uchar generationStatus,T::GenerationInfo& generationInfo);
     virtual int    _getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,const std::string& producerName,uchar generationStatus,T::GenerationInfo& generationInfo);
     virtual int    _getGenerationInfoCount(T::SessionId sessionId,uint& count);
     virtual int    _setGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo);
     virtual int    _setGenerationInfoStatusById(T::SessionId sessionId,T::GenerationId generationId,uchar status);
     virtual int    _setGenerationInfoStatusByName(T::SessionId sessionId,const std::string& generationName,uchar status);

     virtual int    _addGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo);
     virtual int    _deleteGeometryInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId);
     virtual int    _deleteGeometryInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId);
     virtual int    _deleteGeometryInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId);
     virtual int    _deleteGeometryInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId);
     virtual int    _getGeometryInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo);
     virtual int    _getGeometryInfoList(T::SessionId sessionId,T::GeometryInfoList& geometryInfoList);
     virtual int    _getGeometryInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryInfoList& geometryInfoList);
     virtual int    _getGeometryInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::GeometryInfoList& geometryInfoList);
     virtual int    _getGeometryInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::GeometryInfoList& geometryInfoList);
     virtual int    _getGeometryInfoCount(T::SessionId sessionId,uint& count);
     virtual int    _setGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo);
     virtual int    _setGeometryInfoStatusById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,uchar status);

     virtual int    _addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo);
     virtual int    _addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList);
     virtual int    _addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList);
     virtual int    _deleteFileInfoById(T::SessionId sessionId,T::FileId fileId);
     virtual int    _deleteFileInfoByName(T::SessionId sessionId,const std::string& filename);
     virtual int    _deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList);
     virtual int    _deleteFileInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId);
     virtual int    _deleteFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName);
     virtual int    _deleteFileInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId);
     virtual int    _deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,const std::string& forecastTime);
     virtual int    _deleteFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName);
     virtual int    _deleteFileInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId);
     virtual int    _deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<T::FileId>& fileIdList);
     virtual int    _getFileInfoById(T::SessionId sessionId,T::FileId fileId,T::FileInfo& fileInfo);
     virtual int    _getFileInfoByName(T::SessionId sessionId,const std::string& filename,T::FileInfo& fileInfo);
     virtual int    _getFileInfoList(T::SessionId sessionId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<T::FileId>& fileIdList,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     virtual int    _getFileInfoCount(T::SessionId sessionId,uint& count);
     virtual int    _getFileInfoCountByProducerId(T::SessionId sessionId,T::ProducerId producerId,uint& count);
     virtual int    _getFileInfoCountByGenerationId(T::SessionId sessionId,T::GenerationId generationId,uint& count);
     virtual int    _getFileInfoCountBySourceId(T::SessionId sessionId,T::SourceId sourceId,uint& count);
     virtual int    _setFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo);

     virtual int    _addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo);
     virtual int    _getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo);
     virtual int    _getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList);
     virtual int    _getEventInfoCount(T::SessionId sessionId,uint& count);

     virtual int    _addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo);
     virtual int    _setContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo);
     virtual int    _addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList);
     virtual int    _deleteContentInfo(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex);
     virtual int    _deleteContentListByFileId(T::SessionId sessionId,T::FileId fileId);
     virtual int    _deleteContentListByFileName(T::SessionId sessionId,const std::string& filename);
     virtual int    _deleteContentListByProducerId(T::SessionId sessionId,T::ProducerId producerId);
     virtual int    _deleteContentListByProducerName(T::SessionId sessionId,const std::string& producerName);
     virtual int    _deleteContentListByGenerationId(T::SessionId sessionId,T::GenerationId generationId);
     virtual int    _deleteContentListByGenerationName(T::SessionId sessionId,const std::string& generationName);
     virtual int    _deleteContentListBySourceId(T::SessionId sessionId,T::SourceId sourceId);

     virtual int    _getContentInfo(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::ContentInfo& contentInfo);
     virtual int    _getContentList(T::SessionId sessionId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByFileId(T::SessionId sessionId,T::FileId fileId,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByFileIdList(T::SessionId sessionId,std::vector<T::FileId>& fileIdList,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByFileName(T::SessionId sessionId,const std::string& filename,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByProducerName(T::SessionId sessionId,const std::string& producerName,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationName(T::SessionId sessionId,const std::string& generationName,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,T::GenerationId generationId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);

     virtual int    _getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndProducerId(T::SessionId sessionId,T::ProducerId producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     virtual int    _getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,T::ContentInfoList& contentInfoList);

     virtual int    _getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList);

     virtual int    _getContentGeometryIdListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,std::set<T::GeometryId>& geometryIdList);
     virtual int    _getContentLevelListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList);
     virtual int    _getContentLevelListByParameterGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,std::string parameterKey,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList);
     virtual int    _getContentParamListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ContentInfoList& contentParamList);
     virtual int    _getContentParamKeyListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
     virtual int    _getContentTimeListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,std::set<std::string>& contentTimeList);
     virtual int    _getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList);
     virtual int    _getContentTimeListByProducerId(T::SessionId sessionId,T::ProducerId producerId,std::set<std::string>& contentTimeList);

     virtual int    _getContentCount(T::SessionId sessionId,uint& count);

     virtual int    _getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList);

   protected:

     void           createTables();
     bool           isConnectionValid();
     int            openConnection();
     void           closeConnection();
     bool           isSessionValid(T::SessionId sessionId);

     T::EventId     addEvent(uint eventType,UInt64 id1,UInt64 id2,UInt64 id3,UInt64 flags,const char *eventData);
     int            addEventInfoList(T::EventInfoList& eventInfoList);

     uint           getCount(const char *sql);

     int            deleteProducerById(T::ProducerId producerId,bool deleteGenerations,bool deleteGenerationGeometries,bool deleteFiles,bool deleteContent);
     int            deleteProducerListBySourceId(T::SourceId sourceId,bool deleteGenerations,bool deleteGenerationGeometries,bool deleteFiles,bool deleteContent);
     int            getProducerById(T::ProducerId producerId,T::ProducerInfo& producerInfo);
     int            getProducerByName(std::string producerName,T::ProducerInfo& producerInfo);
     int            getProducerList(T::ProducerInfoList& producerInfoList);
     int            getProducerListBySourceId(T::SourceId sourceId,T::ProducerInfoList& producerInfoList);

     int            deleteGenerationById(T::GenerationId generationId,bool deleteGenerationGeometries,bool deleteFiles,bool deleteContent);
     int            deleteGenerationListByProducerId(T::ProducerId producerId,bool deleteGenerationGeometries,bool deleteFiles,bool deleteContent);
     int            deleteGenerationListBySourceId(T::SourceId sourceId,bool deleteGenerationGeometries,bool deleteFiles,bool deleteContent);
     int            getGenerationById(T::GenerationId generationId,T::GenerationInfo& generationInfo);
     int            getGenerationByName(std::string generationName,T::GenerationInfo& generationInfo);
     int            getGenerationList(T::GenerationInfoList& generationInfoList);
     int            getGenerationListByProducerId(T::ProducerId producerId,T::GenerationInfoList& generationInfoList);
     int            getGenerationListByGeometryId(T::GeometryId geometryId,T::GenerationInfoList& generationInfoList);
     int            getGenerationListBySourceId(T::SourceId sourceId,T::GenerationInfoList& generationInfoList);

     int            addFile(T::FileInfo& fileInfo);
     int            addFileList(T::FileInfoList& fileInfoList);
     int            deleteFileById(T::FileId fileId,bool deleteContent);
     int            deleteFileListByGenerationId(T::GenerationId generationId,bool deleteContent);
     int            deleteFileListByGenerationIdList(std::set<T::GenerationId>& generationIdList,bool deleteContent);
     int            deleteFileListByProducerId(T::ProducerId producerId,bool deleteContent);
     int            deleteFileListBySourceId(T::SourceId sourceId,bool deleteContent);
     int            getFileById(T::FileId fileId,T::FileInfo& fileInfo);
     int            getFileByName(std::string fileName,T::FileInfo& fileInfo);
     int            getFileList(T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     int            getFileListByGenerationId(T::GenerationId generationId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     int            getFileListByGenerationIdList(std::set<T::GenerationId>& generationIdList,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     int            getFileListByProducerId(T::ProducerId producerId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);
     int            getFileListBySourceId(T::SourceId sourceId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList);

     int            addContent(T::ContentInfo& contentInfo);
     int            addContentList(T::ContentInfoList& contentInfoList);
     int            setContent(T::ContentInfo& contentInfo);
     int            deleteContent(T::FileId fileId,T::MessageIndex messageIndex);
     int            deleteContentByFileId(T::FileId fileId);
     int            deleteContentByGenerationId(T::GenerationId generationId);
     int            deleteContentByGenerationIdList(std::set<T::GenerationId>& generationIdList);
     int            deleteContentByProducerId(T::ProducerId producerId);
     int            deleteContentBySourceId(T::SourceId sourceId);
     int            getContent(T::FileId fileId,T::MessageIndex messageIndex,T::ContentInfo& contentInfo);
     int            getContent(T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     int            getContentByFileId(T::FileId fileId,T::ContentInfoList& contentInfoList);
     int            getContentByGenerationId(T::GenerationId generationId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     int            getContentByGenerationIdList(std::set<T::GenerationId>& generationIdList,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     int            getContentByGenerationIdAndTimeRange(T::GenerationId generationId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList);
     int            getContentByForecastTimeList(std::vector<T::ForecastTime>& forecastTimeList,T::ContentInfoList& contentInfoList);
     int            getContentByProducerId(T::ProducerId producerId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     int            getContentBySourceId(T::SourceId sourceId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList);
     int            getContentByParameterId(T::ParamKeyType parameterKeyType,const std::string& parameterKey,T::ContentInfoList& contentInfoList);
     int            getContentByParameterIdAndTimeRange(T::ParamKeyType parameterKeyType,const std::string& parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList);
     int            getContentByParameterIdAndGeneration(T::GenerationId generationId,T::ParamKeyType parameterKeyType,const std::string& parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList);
     int            getContentByParameterIdAndProducer(T::ProducerId producerId,T::ParamKeyType parameterKeyType,const std::string& parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList);
     int            getGenerationTimeAndGeometryList(std::set<std::string>& list);

     void           truncateEvents();

     bool           mShutdownRequested;
     bool           mTableCreationAllowed;
     time_t         mStartTime;
     time_t         mEventTruncateCheckTime;
     ThreadLock     mThreadLock;
     pg_conn *      mConnection;
     std::string    mPrimaryConnectionString;
     std::string    mSecondaryConnectionString;
     T::EventInfo   mLastEvent;
     time_t         mContentChangeTime;
};


}
}
