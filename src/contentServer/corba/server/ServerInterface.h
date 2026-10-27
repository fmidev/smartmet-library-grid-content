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

// ====================================================================================
/*! \brief CORBA servant that exposes a ContentServer ServiceInterface over the network.
 *
 *  Inherits from the omniORB-generated skeleton class and delegates every inbound
 *  CORBA call to the local ServiceInterface set via init().  Type conversions between
 *  IDL types and C++ domain types are performed by the Converter helper class. */
// ====================================================================================
class ServerInterface : public POA_SmartMet::ContentServer::Corba::ServiceInterface
{
  public:
                  ServerInterface();
    ~ServerInterface() override;
    ServerInterface(const ServerInterface&) = default;
    ServerInterface& operator=(const ServerInterface&) = default;

    void          init(ContentServer::ServiceInterface *service);

    SmartMet::C::Result clear(SmartMet::C::SessionId sessionId) override;
    SmartMet::C::Result reload(SmartMet::C::SessionId sessionId) override;
    SmartMet::C::Result getContentChangeTime(SmartMet::C::SessionId sessionId, SmartMet::C::Timestamp& changeTime) override;
    SmartMet::C::Result addProducerInfo(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerInfo& producerInfo) override;
    SmartMet::C::Result deleteProducerInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId) override;
    SmartMet::C::Result deleteProducerInfoByName(SmartMet::C::SessionId sessionId, const char* producerName) override;
    SmartMet::C::Result deleteProducerInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId) override;
    SmartMet::C::Result getProducerInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::ProducerInfo_out producerInfo) override;
    SmartMet::C::Result getProducerInfoByName(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::ProducerInfo_out producerInfo) override;
    SmartMet::C::Result getProducerInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerInfoList_out producerInfoList) override;
    SmartMet::C::Result getProducerInfoListByParameter(SmartMet::C::SessionId sessionId, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::ProducerInfoList_out producerInfoList) override;
    SmartMet::C::Result getProducerInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::ProducerInfoList_out producerInfoList) override;
    SmartMet::C::Result getProducerInfoCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count) override;
    SmartMet::C::Result getProducerNameAndGeometryList(SmartMet::C::SessionId sessionId, SmartMet::C::StringList_out list) override;
    SmartMet::C::Result getProducerParameterList(SmartMet::C::SessionId sessionId, SmartMet::C::ParameterKeyType sourceParameterKeyType, SmartMet::C::ParameterKeyType targetParameterKeyType, SmartMet::C::StringList_out list) override;
    SmartMet::C::Result getProducerParameterListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::ParameterKeyType sourceParameterKeyType, SmartMet::C::ParameterKeyType targetParameterKeyType, SmartMet::C::StringList_out list) override;
    SmartMet::C::Result setProducerInfo(SmartMet::C::SessionId sessionId, const SmartMet::C::ProducerInfo& producerInfo) override;
    SmartMet::C::Result addGenerationInfo(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationInfo& generationInfo) override;
    SmartMet::C::Result deleteGenerationInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId) override;
    SmartMet::C::Result deleteGenerationInfoByName(SmartMet::C::SessionId sessionId, const char* generationName) override;
    SmartMet::C::Result deleteGenerationInfoListByIdList(SmartMet::C::SessionId sessionId, const SmartMet::C::GenerationIdList& generationIdList) override;
    SmartMet::C::Result deleteGenerationInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId) override;
    SmartMet::C::Result deleteGenerationInfoListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName) override;
    SmartMet::C::Result deleteGenerationInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId) override;
    SmartMet::C::Result getGenerationIdGeometryIdAndForecastTimeList(SmartMet::C::SessionId sessionId, SmartMet::C::StringList_out list) override;
    SmartMet::C::Result getGenerationInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GenerationInfo_out generationInfo) override;
    SmartMet::C::Result getGenerationInfoByName(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::GenerationInfo_out generationInfo) override;
    SmartMet::C::Result getGenerationInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationInfoList_out generationInfoList) override;
    SmartMet::C::Result getGenerationInfoListByGeometryId(SmartMet::C::SessionId sessionId, SmartMet::C::GeometryId geometryId, SmartMet::C::GenerationInfoList_out generationInfoList) override;
    SmartMet::C::Result getGenerationInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::GenerationInfoList_out generationInfoList) override;
    SmartMet::C::Result getGenerationInfoListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::GenerationInfoList_out generationInfoList) override;
    SmartMet::C::Result getGenerationInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::GenerationInfoList_out generationInfoList) override;
    SmartMet::C::Result getLastGenerationInfoByProducerIdAndStatus(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::Status generationStatus, SmartMet::C::GenerationInfo_out generationInfo) override;
    SmartMet::C::Result getLastGenerationInfoByProducerNameAndStatus(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::Status generationStatus, SmartMet::C::GenerationInfo_out generationInfo) override;
    SmartMet::C::Result getGenerationInfoCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count) override;
    SmartMet::C::Result setGenerationInfo(SmartMet::C::SessionId sessionId, const SmartMet::C::GenerationInfo& generationInfo) override;
    SmartMet::C::Result setGenerationInfoStatusById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::Status status) override;
    SmartMet::C::Result setGenerationInfoStatusByName(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::Status status) override;
    SmartMet::C::Result addGeometryInfo(SmartMet::C::SessionId sessionId, SmartMet::C::GeometryInfo& geometryInfo) override;
    SmartMet::C::Result deleteGeometryInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId) override;
    SmartMet::C::Result deleteGeometryInfoListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId) override;
    SmartMet::C::Result deleteGeometryInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId) override;
    SmartMet::C::Result deleteGeometryInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId) override;
    SmartMet::C::Result getGeometryInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId, SmartMet::C::GeometryInfo& geometryInfo) override;
    SmartMet::C::Result getGeometryInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::GeometryInfoList_out geometryInfoList) override;
    SmartMet::C::Result getGeometryInfoListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryInfoList_out geometryInfoList) override;
    SmartMet::C::Result getGeometryInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::GeometryInfoList_out geometryInfoList) override;
    SmartMet::C::Result getGeometryInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::GeometryInfoList_out geometryInfoList) override;
    SmartMet::C::Result getGeometryInfoCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count) override;
    SmartMet::C::Result setGeometryInfo(SmartMet::C::SessionId sessionId, const SmartMet::C::GeometryInfo& geometryInfo) override;
    SmartMet::C::Result setGeometryInfoStatusById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId, SmartMet::C::Status status) override;
    SmartMet::C::Result addFileInfo(SmartMet::C::SessionId sessionId, SmartMet::C::FileInfo& fileInfo) override;
    SmartMet::C::Result addFileInfoWithContentList(SmartMet::C::SessionId sessionId, SmartMet::C::FileInfo& fileInfo, SmartMet::C::ContentInfoList& contentInfoList) override;
    SmartMet::C::Result addFileInfoListWithContent(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32 requestFlags, SmartMet::C::FileContentList& fileContentList) override;
    SmartMet::C::Result deleteFileInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId) override;
    SmartMet::C::Result deleteFileInfoByName(SmartMet::C::SessionId sessionId, const char* filename) override;
    SmartMet::C::Result deleteFileInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId) override;
    SmartMet::C::Result deleteFileInfoListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName) override;
    SmartMet::C::Result deleteFileInfoListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId) override;
    SmartMet::C::Result deleteFileInfoListByGenerationIdAndForecastTime(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::Result geometryId, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::Timestamp forecastTime) override;
    SmartMet::C::Result deleteFileInfoListByForecastTimeList(SmartMet::C::SessionId sessionId, const SmartMet::C::ForecastTimeInfoList& forecastTimeList) override;
    SmartMet::C::Result deleteFileInfoListByGenerationName(SmartMet::C::SessionId sessionId, const char* generationName) override;
    SmartMet::C::Result deleteFileInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId) override;
    SmartMet::C::Result deleteFileInfoListByFileIdList(SmartMet::C::SessionId sessionId, const SmartMet::C::FileIdList& fileIdList) override;
    SmartMet::C::Result getFileInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::FileInfo_out fileInfo) override;
    SmartMet::C::Result getFileInfoByName(SmartMet::C::SessionId sessionId, const char* filename, SmartMet::C::FileInfo_out fileInfo) override;
    SmartMet::C::Result getFileInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList) override;
    SmartMet::C::Result getFileInfoListByFileIdList(SmartMet::C::SessionId sessionId, const SmartMet::C::FileIdList& fileIdList, SmartMet::C::FileInfoList_out fileInfoList) override;
    SmartMet::C::Result getFileInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList) override;
    SmartMet::C::Result getFileInfoListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList) override;
    SmartMet::C::Result getFileInfoListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList) override;
    SmartMet::C::Result getFileInfoListByGenerationName(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList) override;
    SmartMet::C::Result getFileInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList) override;
    SmartMet::C::Result getFileInfoCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count) override;
    SmartMet::C::Result getFileInfoCountByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::UInt32& count) override;
    SmartMet::C::Result getFileInfoCountByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::UInt32& count) override;
    SmartMet::C::Result getFileInfoCountBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::UInt32& count) override;
    SmartMet::C::Result setFileInfo(SmartMet::C::SessionId sessionId, const SmartMet::C::FileInfo& fileInfo) override;
    SmartMet::C::Result addEventInfo(SmartMet::C::SessionId sessionId, SmartMet::C::EventInfo& eventInfo) override;
    SmartMet::C::Result getLastEventInfo(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32 requestingServerId, SmartMet::C::EventInfo_out eventInfo) override;
    SmartMet::C::Result getEventInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32 requestingServerId, SmartMet::C::EventId startEventId, SmartMet::C::Int32 maxRecords, SmartMet::C::EventInfoList_out eventInfoList) override;
    SmartMet::C::Result getEventInfoCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count) override;
    SmartMet::C::Result addContentInfo(SmartMet::C::SessionId sessionId, SmartMet::C::ContentInfo& contentInfo) override;
    SmartMet::C::Result addContentList(SmartMet::C::SessionId sessionId, SmartMet::C::ContentInfoList& contentInfoList) override;
    SmartMet::C::Result deleteContentInfo(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex) override;
    SmartMet::C::Result deleteContentListByFileId(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId) override;
    SmartMet::C::Result deleteContentListByFileName(SmartMet::C::SessionId sessionId, const char* filename) override;
    SmartMet::C::Result deleteContentListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId) override;
    SmartMet::C::Result deleteContentListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName) override;
    SmartMet::C::Result deleteContentListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId) override;
    SmartMet::C::Result deleteContentListByGenerationName(SmartMet::C::SessionId sessionId, const char* generationName) override;
    SmartMet::C::Result deleteContentListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId) override;
    SmartMet::C::Result getContentInfo(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::ContentInfo_out contentInfo) override;
    SmartMet::C::Result getContentList(SmartMet::C::SessionId sessionId, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListByFileId(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListByFileIdList(SmartMet::C::SessionId sessionId, const SmartMet::C::FileIdList& fileIdList, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListByFileName(SmartMet::C::SessionId sessionId, const char* filename, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListByGenerationName(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListByGenerationIdAndTimeRange(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListByGenerationNameAndTimeRange(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListByParameter(SmartMet::C::SessionId sessionId, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level minLevel, SmartMet::C::Level maxLevel, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListByParameterAndGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level minLevel, SmartMet::C::Level maxLevel, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListByParameterAndGenerationName(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level minLevel, SmartMet::C::Level maxLevel, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListByParameterAndProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level minLevel, SmartMet::C::Level maxLevel, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListByParameterAndProducerName(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level minLevel, SmartMet::C::Level maxLevel, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListByParameterGenerationIdAndForecastTime(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level level, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp forecastTime, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentListOfInvalidIntegrity(SmartMet::C::SessionId sessionId, SmartMet::C::ContentInfoList_out contentInfoList) override;
    SmartMet::C::Result getContentGeometryIdListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryIdList_out geometryIdList) override;
    SmartMet::C::Result getContentLevelListByGenerationGeometryAndLevelId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId, SmartMet::C::LevelList_out contentLevelList) override;
    SmartMet::C::Result getContentLevelListByParameterGenerationGeometryAndLevelId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::LevelList_out contentLevelList) override;
    SmartMet::C::Result getContentParamListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::ContentInfoList_out contentParamList) override;
    SmartMet::C::Result getContentParamKeyListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::ParameterKeyType parameterKeyType, SmartMet::C::StringList_out paramKeyList) override;
    SmartMet::C::Result getContentParamKeyListByGenerationAndGeometryId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::ParameterKeyType parameterKeyType, SmartMet::C::StringList_out paramKeyList) override;
    SmartMet::C::Result getContentParamKeyListByGenerationGeometryAndLevelId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId, SmartMet::C::ParameterKeyType parameterKeyType, SmartMet::C::StringList_out paramKeyList) override;
    SmartMet::C::Result getContentTimeListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::TimeStringList_out contentTimeList) override;
    SmartMet::C::Result getContentTimeListByGenerationAndGeometryId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::TimeStringList_out contentTimeList) override;
    SmartMet::C::Result getContentTimeListByGenerationGeometryAndLevelId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId, SmartMet::C::TimeStringList_out contentTimeList) override;
    SmartMet::C::Result getContentTimeListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::TimeStringList_out contentTimeList) override;
    SmartMet::C::Result getContentTimeRangeByProducerAndGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::GenerationId generationId, SmartMet::C::Timestamp& startTime, SmartMet::C::Timestamp& endTime) override;
    SmartMet::C::Result getContentTimeRangeByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::Timestamp& startTime, SmartMet::C::Timestamp& endTime) override;
    SmartMet::C::Result getContentCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count) override;
    SmartMet::C::Result setContentInfo(SmartMet::C::SessionId sessionId, const SmartMet::C::ContentInfo& contentInfo) override;
    SmartMet::C::Result getHashByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::Hash& hash) override;
    SmartMet::C::Result getLevelInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::LevelInfoList_out levelInfoList) override;

  protected:

    ContentServer::ServiceInterface *mService;  //!< Non-owning pointer to the local service implementation.
};


}
}
}
