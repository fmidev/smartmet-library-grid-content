#pragma once

#include "../../definition/ServiceInterface.h"
#include "../stubs/ContentServer_serviceInterface_SK.h"


// This file contains the actual implementation of the CORBA server interface. Every time
// you modify the IDL file, a new skeleton will be generated into the "stubs" directory.
// Copy the new service interface (or its changes) from the "*_i.cc" file into this file and
// write the actual implementation. The point is that we do not use "*_i.cc" file in our build,
// because it is overwritten every time when the IDL file is changes.


namespace SmartMet
{
namespace ContentServer
{
namespace Corba
{


class ServerInterface : public POA_SmartMet::ContentServer::Corba::ServiceInterface
{
  public:
                  ServerInterface();
    virtual       ~ServerInterface();

    void          init(ContentServer::ServiceInterface *service);

    SmartMet::C::Result clear(SmartMet::C::SessionId sessionId);
    SmartMet::C::Result reload(SmartMet::C::SessionId sessionId);
    SmartMet::C::Result getContentChangeTime(SmartMet::C::SessionId sessionId, SmartMet::C::Timestamp& changeTime);
    SmartMet::C::Result addProducerInfo(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerInfo& producerInfo);
    SmartMet::C::Result deleteProducerInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId);
    SmartMet::C::Result deleteProducerInfoByName(SmartMet::C::SessionId sessionId, const char* producerName);
    SmartMet::C::Result deleteProducerInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId);
    SmartMet::C::Result getProducerInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::ProducerInfo_out producerInfo);
    SmartMet::C::Result getProducerInfoByName(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::ProducerInfo_out producerInfo);
    SmartMet::C::Result getProducerInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerInfoList_out producerInfoList);
    SmartMet::C::Result getProducerInfoListByParameter(SmartMet::C::SessionId sessionId, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::ProducerInfoList_out producerInfoList);
    SmartMet::C::Result getProducerInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::ProducerInfoList_out producerInfoList);
    SmartMet::C::Result getProducerInfoCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count);
    SmartMet::C::Result getProducerNameAndGeometryList(SmartMet::C::SessionId sessionId, SmartMet::C::StringList_out list);
    SmartMet::C::Result getProducerParameterList(SmartMet::C::SessionId sessionId, SmartMet::C::ParameterKeyType sourceParameterKeyType, SmartMet::C::ParameterKeyType targetParameterKeyType, SmartMet::C::StringList_out list);
    SmartMet::C::Result getProducerParameterListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::ParameterKeyType sourceParameterKeyType, SmartMet::C::ParameterKeyType targetParameterKeyType, SmartMet::C::StringList_out list);
    SmartMet::C::Result setProducerInfo(SmartMet::C::SessionId sessionId, const SmartMet::C::ProducerInfo& producerInfo);
    SmartMet::C::Result addGenerationInfo(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationInfo& generationInfo);
    SmartMet::C::Result deleteGenerationInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId);
    SmartMet::C::Result deleteGenerationInfoByName(SmartMet::C::SessionId sessionId, const char* generationName);
    SmartMet::C::Result deleteGenerationInfoListByIdList(SmartMet::C::SessionId sessionId, const SmartMet::C::GenerationIdList& generationIdList);
    SmartMet::C::Result deleteGenerationInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId);
    SmartMet::C::Result deleteGenerationInfoListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName);
    SmartMet::C::Result deleteGenerationInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId);
    SmartMet::C::Result getGenerationIdGeometryIdAndForecastTimeList(SmartMet::C::SessionId sessionId, SmartMet::C::StringList_out list);
    SmartMet::C::Result getGenerationInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GenerationInfo_out generationInfo);
    SmartMet::C::Result getGenerationInfoByName(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::GenerationInfo_out generationInfo);
    SmartMet::C::Result getGenerationInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationInfoList_out generationInfoList);
    SmartMet::C::Result getGenerationInfoListByGeometryId(SmartMet::C::SessionId sessionId, SmartMet::C::GeometryId geometryId, SmartMet::C::GenerationInfoList_out generationInfoList);
    SmartMet::C::Result getGenerationInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::GenerationInfoList_out generationInfoList);
    SmartMet::C::Result getGenerationInfoListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::GenerationInfoList_out generationInfoList);
    SmartMet::C::Result getGenerationInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::GenerationInfoList_out generationInfoList);
    SmartMet::C::Result getLastGenerationInfoByProducerIdAndStatus(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::Status generationStatus, SmartMet::C::GenerationInfo_out generationInfo);
    SmartMet::C::Result getLastGenerationInfoByProducerNameAndStatus(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::Status generationStatus, SmartMet::C::GenerationInfo_out generationInfo);
    SmartMet::C::Result getGenerationInfoCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count);
    SmartMet::C::Result setGenerationInfo(SmartMet::C::SessionId sessionId, const SmartMet::C::GenerationInfo& generationInfo);
    SmartMet::C::Result setGenerationInfoStatusById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::Status status);
    SmartMet::C::Result setGenerationInfoStatusByName(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::Status status);
    SmartMet::C::Result addGeometryInfo(SmartMet::C::SessionId sessionId, SmartMet::C::GeometryInfo& geometryInfo);
    SmartMet::C::Result deleteGeometryInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId);
    SmartMet::C::Result deleteGeometryInfoListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId);
    SmartMet::C::Result deleteGeometryInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId);
    SmartMet::C::Result deleteGeometryInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId);
    SmartMet::C::Result getGeometryInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId, SmartMet::C::GeometryInfo& geometryInfo);
    SmartMet::C::Result getGeometryInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::GeometryInfoList_out geometryInfoList);
    SmartMet::C::Result getGeometryInfoListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryInfoList_out geometryInfoList);
    SmartMet::C::Result getGeometryInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::GeometryInfoList_out geometryInfoList);
    SmartMet::C::Result getGeometryInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::GeometryInfoList_out geometryInfoList);
    SmartMet::C::Result getGeometryInfoCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count);
    SmartMet::C::Result setGeometryInfo(SmartMet::C::SessionId sessionId, const SmartMet::C::GeometryInfo& geometryInfo);
    SmartMet::C::Result setGeometryInfoStatusById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId, SmartMet::C::Status status);
    SmartMet::C::Result addFileInfo(SmartMet::C::SessionId sessionId, SmartMet::C::FileInfo& fileInfo);
    SmartMet::C::Result addFileInfoWithContentList(SmartMet::C::SessionId sessionId, SmartMet::C::FileInfo& fileInfo, SmartMet::C::ContentInfoList& contentInfoList);
    SmartMet::C::Result addFileInfoListWithContent(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32 requestFlags, SmartMet::C::FileContentList& fileContentList);
    SmartMet::C::Result deleteFileInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId);
    SmartMet::C::Result deleteFileInfoByName(SmartMet::C::SessionId sessionId, const char* filename);
    SmartMet::C::Result deleteFileInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId);
    SmartMet::C::Result deleteFileInfoListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName);
    SmartMet::C::Result deleteFileInfoListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId);
    SmartMet::C::Result deleteFileInfoListByGenerationIdAndForecastTime(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::Result geometryId, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::Timestamp forecastTime);
    SmartMet::C::Result deleteFileInfoListByForecastTimeList(SmartMet::C::SessionId sessionId, const SmartMet::C::ForecastTimeInfoList& forecastTimeList);
    SmartMet::C::Result deleteFileInfoListByGenerationName(SmartMet::C::SessionId sessionId, const char* generationName);
    SmartMet::C::Result deleteFileInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId);
    SmartMet::C::Result deleteFileInfoListByFileIdList(SmartMet::C::SessionId sessionId, const SmartMet::C::FileIdList& fileIdList);
    SmartMet::C::Result getFileInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::FileInfo_out fileInfo);
    SmartMet::C::Result getFileInfoByName(SmartMet::C::SessionId sessionId, const char* filename, SmartMet::C::FileInfo_out fileInfo);
    SmartMet::C::Result getFileInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList);
    SmartMet::C::Result getFileInfoListByFileIdList(SmartMet::C::SessionId sessionId, const SmartMet::C::FileIdList& fileIdList, SmartMet::C::FileInfoList_out fileInfoList);
    SmartMet::C::Result getFileInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList);
    SmartMet::C::Result getFileInfoListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList);
    SmartMet::C::Result getFileInfoListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList);
    SmartMet::C::Result getFileInfoListByGenerationName(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList);
    SmartMet::C::Result getFileInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList);
    SmartMet::C::Result getFileInfoCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count);
    SmartMet::C::Result getFileInfoCountByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::UInt32& count);
    SmartMet::C::Result getFileInfoCountByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::UInt32& count);
    SmartMet::C::Result getFileInfoCountBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::UInt32& count);
    SmartMet::C::Result setFileInfo(SmartMet::C::SessionId sessionId, const SmartMet::C::FileInfo& fileInfo);
    SmartMet::C::Result addEventInfo(SmartMet::C::SessionId sessionId, SmartMet::C::EventInfo& eventInfo);
    SmartMet::C::Result getLastEventInfo(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32 requestingServerId, SmartMet::C::EventInfo_out eventInfo);
    SmartMet::C::Result getEventInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32 requestingServerId, SmartMet::C::EventId startEventId, SmartMet::C::Int32 maxRecords, SmartMet::C::EventInfoList_out eventInfoList);
    SmartMet::C::Result getEventInfoCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count);
    SmartMet::C::Result addContentInfo(SmartMet::C::SessionId sessionId, SmartMet::C::ContentInfo& contentInfo);
    SmartMet::C::Result addContentList(SmartMet::C::SessionId sessionId, SmartMet::C::ContentInfoList& contentInfoList);
    SmartMet::C::Result deleteContentInfo(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex);
    SmartMet::C::Result deleteContentListByFileId(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId);
    SmartMet::C::Result deleteContentListByFileName(SmartMet::C::SessionId sessionId, const char* filename);
    SmartMet::C::Result deleteContentListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId);
    SmartMet::C::Result deleteContentListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName);
    SmartMet::C::Result deleteContentListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId);
    SmartMet::C::Result deleteContentListByGenerationName(SmartMet::C::SessionId sessionId, const char* generationName);
    SmartMet::C::Result deleteContentListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId);
    SmartMet::C::Result getContentInfo(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::ContentInfo_out contentInfo);
    SmartMet::C::Result getContentList(SmartMet::C::SessionId sessionId, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListByFileId(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListByFileIdList(SmartMet::C::SessionId sessionId, const SmartMet::C::FileIdList& fileIdList, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListByFileName(SmartMet::C::SessionId sessionId, const char* filename, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListByGenerationName(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListByGenerationIdAndTimeRange(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListByGenerationNameAndTimeRange(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListByParameter(SmartMet::C::SessionId sessionId, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level minLevel, SmartMet::C::Level maxLevel, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListByParameterAndGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level minLevel, SmartMet::C::Level maxLevel, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListByParameterAndGenerationName(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level minLevel, SmartMet::C::Level maxLevel, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListByParameterAndProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level minLevel, SmartMet::C::Level maxLevel, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListByParameterAndProducerName(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level minLevel, SmartMet::C::Level maxLevel, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListByParameterGenerationIdAndForecastTime(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level level, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp forecastTime, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentListOfInvalidIntegrity(SmartMet::C::SessionId sessionId, SmartMet::C::ContentInfoList_out contentInfoList);
    SmartMet::C::Result getContentGeometryIdListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryIdList_out geometryIdList);
    SmartMet::C::Result getContentLevelListByGenerationGeometryAndLevelId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId, SmartMet::C::LevelList_out contentLevelList);
    SmartMet::C::Result getContentLevelListByParameterGenerationGeometryAndLevelId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::LevelList_out contentLevelList);
    SmartMet::C::Result getContentParamListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::ContentInfoList_out contentParamList);
    SmartMet::C::Result getContentParamKeyListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::ParameterKeyType parameterKeyType, SmartMet::C::StringList_out paramKeyList);
    SmartMet::C::Result getContentParamKeyListByGenerationAndGeometryId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::ParameterKeyType parameterKeyType, SmartMet::C::StringList_out paramKeyList);
    SmartMet::C::Result getContentParamKeyListByGenerationGeometryAndLevelId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId, SmartMet::C::ParameterKeyType parameterKeyType, SmartMet::C::StringList_out paramKeyList);
    SmartMet::C::Result getContentTimeListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::TimeStringList_out contentTimeList);
    SmartMet::C::Result getContentTimeListByGenerationAndGeometryId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::TimeStringList_out contentTimeList);
    SmartMet::C::Result getContentTimeListByGenerationGeometryAndLevelId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId, SmartMet::C::TimeStringList_out contentTimeList);
    SmartMet::C::Result getContentTimeListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::TimeStringList_out contentTimeList);
    SmartMet::C::Result getContentTimeRangeByProducerAndGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::GenerationId generationId, SmartMet::C::Timestamp& startTime, SmartMet::C::Timestamp& endTime);
    SmartMet::C::Result getContentTimeRangeByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::Timestamp& startTime, SmartMet::C::Timestamp& endTime);
    SmartMet::C::Result getContentCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count);
    SmartMet::C::Result setContentInfo(SmartMet::C::SessionId sessionId, const SmartMet::C::ContentInfo& contentInfo);
    SmartMet::C::Result getHashByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::Hash& hash);
    SmartMet::C::Result getLevelInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::LevelInfoList_out levelInfoList);

  protected:

    ContentServer::ServiceInterface *mService;
};


}
}
}
