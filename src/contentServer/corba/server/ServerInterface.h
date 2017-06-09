#pragma once

#include "contentServer/definition/ServiceInterface.h"
#include "contentServer/corba/stubs/ContentServer_serviceInterface_SK.h"

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

    ::CORBA::Long clear(::CORBA::LongLong sessionId);
    ::CORBA::Long reload(::CORBA::LongLong sessionId);
    ::CORBA::Long addDataServerInfo(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaServerInfo& serverInfo);
    ::CORBA::Long deleteDataServerInfoById(::CORBA::LongLong sessionId, ::CORBA::ULong serverId);
    ::CORBA::Long getDataServerInfoById(::CORBA::LongLong sessionId, ::CORBA::ULong serverId, SmartMet::ContentServer::Corba::CorbaServerInfo_out serverInfo);
    ::CORBA::Long getDataServerInfoByName(::CORBA::LongLong sessionId, const char* serverName, SmartMet::ContentServer::Corba::CorbaServerInfo_out serverInfo);
    ::CORBA::Long getDataServerInfoByIor(::CORBA::LongLong sessionId, const char* serverIor, SmartMet::ContentServer::Corba::CorbaServerInfo_out serverInfo);
    ::CORBA::Long getDataServerInfoList(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaServerInfoList_out serverInfoList);
    ::CORBA::Long getDataServerInfoCount(::CORBA::LongLong sessionId, ::CORBA::ULong& count);
    ::CORBA::Long addProducerInfo(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaProducerInfo& producerInfo);
    ::CORBA::Long deleteProducerInfoById(::CORBA::LongLong sessionId, ::CORBA::ULong producerId);
    ::CORBA::Long deleteProducerInfoByName(::CORBA::LongLong sessionId, const char* producerName);
    ::CORBA::Long getProducerInfoById(::CORBA::LongLong sessionId, ::CORBA::ULong producerId, SmartMet::ContentServer::Corba::CorbaProducerInfo_out producerInfo);
    ::CORBA::Long getProducerInfoByName(::CORBA::LongLong sessionId, const char* producerName, SmartMet::ContentServer::Corba::CorbaProducerInfo_out producerInfo);
    ::CORBA::Long getProducerInfoList(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaProducerInfoList_out producerInfoList);
    ::CORBA::Long getProducerInfoCount(::CORBA::LongLong sessionId, ::CORBA::ULong& count);
    ::CORBA::Long addGenerationInfo(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaGenerationInfo& generationInfo);
    ::CORBA::Long deleteGenerationInfoById(::CORBA::LongLong sessionId, ::CORBA::ULong generationId);
    ::CORBA::Long deleteGenerationInfoByName(::CORBA::LongLong sessionId, const char* generationName);
    ::CORBA::Long deleteGenerationInfoListByProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId);
    ::CORBA::Long deleteGenerationInfoListByProducerName(::CORBA::LongLong sessionId, const char* producerName);
    ::CORBA::Long getGenerationInfoById(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, SmartMet::ContentServer::Corba::CorbaGenerationInfo_out generationInfo);
    ::CORBA::Long getGenerationInfoByName(::CORBA::LongLong sessionId, const char* generationName, SmartMet::ContentServer::Corba::CorbaGenerationInfo_out generationInfo);
    ::CORBA::Long getGenerationInfoList(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaGenerationInfoList_out generationInfoList);
    ::CORBA::Long getGenerationInfoListByProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId, SmartMet::ContentServer::Corba::CorbaGenerationInfoList_out generationInfoList);
    ::CORBA::Long getGenerationInfoListByProducerName(::CORBA::LongLong sessionId, const char* producerName, SmartMet::ContentServer::Corba::CorbaGenerationInfoList_out generationInfoList);
    ::CORBA::Long getLastGenerationInfoByProducerIdAndStatus(::CORBA::LongLong sessionId, ::CORBA::ULong producerId, ::CORBA::Octet generationStatus, SmartMet::ContentServer::Corba::CorbaGenerationInfo_out generationInfo);
    ::CORBA::Long getLastGenerationInfoByProducerNameAndStatus(::CORBA::LongLong sessionId, const char* producerName, ::CORBA::Octet generationStatus, SmartMet::ContentServer::Corba::CorbaGenerationInfo_out generationInfo);
    ::CORBA::Long getGenerationInfoCount(::CORBA::LongLong sessionId, ::CORBA::ULong& count);
    ::CORBA::Long setGenerationInfoStatusById(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, ::CORBA::Octet status);
    ::CORBA::Long setGenerationInfoStatusByName(::CORBA::LongLong sessionId, const char* generationName, ::CORBA::Octet status);
    ::CORBA::Long addFileInfo(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaFileInfo& fileInfo);
    ::CORBA::Long addFileInfoWithContentList(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaFileInfo& fileInfo, SmartMet::ContentServer::Corba::CorbaContentInfoList& contentInfoList);
    ::CORBA::Long deleteFileInfoById(::CORBA::LongLong sessionId, ::CORBA::ULong fileId);
    ::CORBA::Long deleteFileInfoByName(::CORBA::LongLong sessionId, const char* filename);
    ::CORBA::Long deleteFileInfoListByGroupFlags(::CORBA::LongLong sessionId, ::CORBA::ULong groupFlags);
    ::CORBA::Long deleteFileInfoListByProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId);
    ::CORBA::Long deleteFileInfoListByProducerName(::CORBA::LongLong sessionId, const char* producerName);
    ::CORBA::Long deleteFileInfoListByGenerationId(::CORBA::LongLong sessionId, ::CORBA::ULong generationId);
    ::CORBA::Long deleteFileInfoListByGenerationName(::CORBA::LongLong sessionId, const char* generationName);
    ::CORBA::Long getFileInfoById(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, SmartMet::ContentServer::Corba::CorbaFileInfo_out fileInfo);
    ::CORBA::Long getFileInfoByName(::CORBA::LongLong sessionId, const char* filename, SmartMet::ContentServer::Corba::CorbaFileInfo_out fileInfo);
    ::CORBA::Long getFileInfoList(::CORBA::LongLong sessionId, ::CORBA::ULong startFileId, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaFileInfoList_out fileInfoList);
    ::CORBA::Long getFileInfoListByProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId, ::CORBA::ULong startFileId, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaFileInfoList_out fileInfoList);
    ::CORBA::Long getFileInfoListByProducerName(::CORBA::LongLong sessionId, const char* producerName, ::CORBA::ULong startFileId, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaFileInfoList_out fileInfoList);
    ::CORBA::Long getFileInfoListByGenerationId(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, ::CORBA::ULong startFileId, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaFileInfoList_out fileInfoList);
    ::CORBA::Long getFileInfoListByGenerationName(::CORBA::LongLong sessionId, const char* generationName, ::CORBA::ULong startFileId, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaFileInfoList_out fileInfoList);
    ::CORBA::Long getFileInfoListByGroupFlags(::CORBA::LongLong sessionId, ::CORBA::ULong groupFlags, ::CORBA::ULong startFileId, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaFileInfoList_out fileInfoList);
    ::CORBA::Long getFileInfoCount(::CORBA::LongLong sessionId, ::CORBA::ULong& count);
    ::CORBA::Long addEventInfo(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaEventInfo& eventInfo);
    ::CORBA::Long getLastEventInfo(::CORBA::LongLong sessionId, ::CORBA::ULong requestingServerId,SmartMet::ContentServer::Corba::CorbaEventInfo_out eventInfo);
    ::CORBA::Long getEventInfoList(::CORBA::LongLong sessionId, ::CORBA::ULong requestingServerId, ::CORBA::ULongLong startEventId, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaEventInfoList_out eventInfoList);
    ::CORBA::Long getEventInfoCount(::CORBA::LongLong sessionId, ::CORBA::ULong& count);
    ::CORBA::Long addContentInfo(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaContentInfo& contentInfo);
    ::CORBA::Long addContentList(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaContentInfoList& contentInfoList);
    ::CORBA::Long deleteContentInfo(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex);
    ::CORBA::Long deleteContentListByFileId(::CORBA::LongLong sessionId, ::CORBA::ULong fileId);
    ::CORBA::Long deleteContentListByFileName(::CORBA::LongLong sessionId, const char* filename);
    ::CORBA::Long deleteContentListByGroupFlags(::CORBA::LongLong sessionId, ::CORBA::ULong groupFlags);
    ::CORBA::Long deleteContentListByProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId);
    ::CORBA::Long deleteContentListByProducerName(::CORBA::LongLong sessionId, const char* producerName);
    ::CORBA::Long getContentListByProducerName(::CORBA::LongLong sessionId, const char* producerName, ::CORBA::ULong startFileId, ::CORBA::ULong startMessageIndex, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList);
    ::CORBA::Long deleteContentListByGenerationId(::CORBA::LongLong sessionId, ::CORBA::ULong generationId);
    ::CORBA::Long deleteContentListByGenerationName(::CORBA::LongLong sessionId, const char* generationName);
    ::CORBA::Long registerContentList(::CORBA::LongLong sessionId, ::CORBA::ULong serverId, SmartMet::ContentServer::Corba::CorbaContentInfoList& contentInfoList);
    ::CORBA::Long registerContentListByFileId(::CORBA::LongLong sessionId, ::CORBA::ULong serverId, ::CORBA::ULong fileId);
    ::CORBA::Long unregisterContentList(::CORBA::LongLong sessionId, ::CORBA::ULong serverId);
    ::CORBA::Long unregisterContentListByFileId(::CORBA::LongLong sessionId, ::CORBA::ULong serverId, ::CORBA::ULong fileId);
    ::CORBA::Long getContentInfo(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, SmartMet::ContentServer::Corba::CorbaContentInfo_out contentInfo);
    ::CORBA::Long getContentList(::CORBA::LongLong sessionId, ::CORBA::ULong startFileId, ::CORBA::ULong startMessageIndex, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList);
    ::CORBA::Long getContentListByFileId(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList);
    ::CORBA::Long getContentListByFileName(::CORBA::LongLong sessionId, const char* filename, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList);
    ::CORBA::Long getContentListByGroupFlags(::CORBA::LongLong sessionId, ::CORBA::ULong groupFlags, ::CORBA::ULong startFileId, ::CORBA::ULong startMessageIndex, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList);
    ::CORBA::Long getContentListByProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId, ::CORBA::ULong startFileId, ::CORBA::ULong startMessageIndex, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList);
    ::CORBA::Long getContentListByServerId(::CORBA::LongLong sessionId, ::CORBA::ULong serverId, ::CORBA::ULong startFileId, ::CORBA::ULong startMessageIndex, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList);
    ::CORBA::Long getContentListByGenerationId(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, ::CORBA::ULong startFileId, ::CORBA::ULong startMessageIndex, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList);
    ::CORBA::Long getContentListByGenerationName(::CORBA::LongLong sessionId, const char* generationName, ::CORBA::ULong startFileId, ::CORBA::ULong startMessageIndex, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList);
    ::CORBA::Long getContentListByGenerationIdAndTimeRange(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, const char* startTime, const char* endTime, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList);
    ::CORBA::Long getContentListByGenerationNameAndTimeRange(::CORBA::LongLong sessionId, const char* generationName, const char* startTime, const char* endTime, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList);
    ::CORBA::Long getContentListByParameter(::CORBA::LongLong sessionId, ::CORBA::Octet parameterKeyType, const char* parameterKey, ::CORBA::Octet parameterLevelIdType, ::CORBA::ULong parameterLevelId, ::CORBA::ULong minLevel, ::CORBA::ULong maxLevel, const char* startTime, const char* endTime, ::CORBA::ULong requestFlags, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList);
    ::CORBA::Long getContentListByParameterAndGenerationId(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, ::CORBA::Octet parameterKeyType, const char* parameterKey, ::CORBA::Octet parameterLevelIdType, ::CORBA::ULong parameterLevelId, ::CORBA::ULong minLevel, ::CORBA::ULong maxLevel, const char* startTime, const char* endTime, ::CORBA::ULong requestFlags, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList);
    ::CORBA::Long getContentListByParameterAndGenerationName(::CORBA::LongLong sessionId, const char* generationName, ::CORBA::Octet parameterKeyType, const char* parameterKey, ::CORBA::Octet parameterLevelIdType, ::CORBA::ULong parameterLevelId, ::CORBA::ULong minLevel, ::CORBA::ULong maxLevel, const char* startTime, const char* endTime, ::CORBA::ULong requestFlags, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList);
    ::CORBA::Long getContentListByParameterAndProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId, ::CORBA::Octet parameterKeyType, const char* parameterKey, ::CORBA::Octet parameterLevelIdType, ::CORBA::ULong parameterLevelId, ::CORBA::ULong minLevel, ::CORBA::ULong maxLevel, const char* startTime, const char* endTime, ::CORBA::ULong requestFlags, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList);
    ::CORBA::Long getContentListByParameterAndProducerName(::CORBA::LongLong sessionId, const char* producerName, ::CORBA::Octet parameterKeyType, const char* parameterKey, ::CORBA::Octet parameterLevelIdType, ::CORBA::ULong parameterLevelId, ::CORBA::ULong minLevel, ::CORBA::ULong maxLevel, const char* startTime, const char* endTime, ::CORBA::ULong requestFlags, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList);
    ::CORBA::Long getContentCount(::CORBA::LongLong sessionId, ::CORBA::ULong& count);

  protected:

    ContentServer::ServiceInterface *mService;
};


}
}
}
