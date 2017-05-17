#include "ServiceInterface.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"


namespace SmartMet
{
namespace ContentServer
{


ServiceInterface::ServiceInterface()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ServiceInterface::~ServiceInterface()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceInterface::shutdown()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::clear(T::SessionId sessionId)
{
  try
  {
    int result = _clear(sessionId);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::reload(T::SessionId sessionId)
{
  try
  {
    int result = _reload(sessionId);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::addDataServerInfo(T::SessionId sessionId,T::ServerInfo& serverInfo)
{
  try
  {
    int result = _addDataServerInfo(sessionId,serverInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteDataServerInfoById(T::SessionId sessionId,uint serverId)
{
  try
  {
    int result = _deleteDataServerInfoById(sessionId,serverId);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getDataServerInfoById(T::SessionId sessionId,uint serverId,T::ServerInfo& serverInfo)
{
  try
  {
    int result = _getDataServerInfoById(sessionId,serverId,serverInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getDataServerInfoByName(T::SessionId sessionId,std::string serverName,T::ServerInfo& serverInfo)
{
  try
  {
    int result = _getDataServerInfoByName(sessionId,serverName,serverInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getDataServerInfoByIor(T::SessionId sessionId,std::string serverIor,T::ServerInfo& serverInfo)
{
  try
  {
    int result = _getDataServerInfoByIor(sessionId,serverIor,serverInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getDataServerInfoList(T::SessionId sessionId,T::ServerInfoList& serverInfoList)
{
  try
  {
    int result = _getDataServerInfoList(sessionId,serverInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  try
  {
    int result = _addProducerInfo(sessionId,producerInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteProducerInfoById(T::SessionId sessionId,uint producerId)
{
  try
  {
    int result = _deleteProducerInfoById(sessionId,producerId);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteProducerInfoByName(T::SessionId sessionId,std::string producerName)
{
  try
  {
    int result = _deleteProducerInfoByName(sessionId,producerName);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo)
{
  try
  {
    int result = _getProducerInfoById(sessionId,producerId,producerInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getProducerInfoByName(T::SessionId sessionId,std::string producerName,T::ProducerInfo& producerInfo)
{
  try
  {
    int result = _getProducerInfoByName(sessionId,producerName,producerInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  try
  {
    int result = _getProducerInfoList(sessionId,producerInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  try
  {
    int result = _addGenerationInfo(sessionId,generationInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteGenerationInfoById(T::SessionId sessionId,uint generationId)
{
  try
  {
    int result = _deleteGenerationInfoById(sessionId,generationId);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteGenerationInfoByName(T::SessionId sessionId,std::string generationName)
{
  try
  {
    int result = _deleteGenerationInfoByName(sessionId,generationName);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  try
  {
    int result = _deleteGenerationInfoListByProducerId(sessionId,producerId);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  try
  {
    int result = _deleteGenerationInfoListByProducerName(sessionId,producerName);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo)
{
  try
  {
    int result = _getGenerationInfoById(sessionId,generationId,generationInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGenerationInfoByName(T::SessionId sessionId,std::string generationName,T::GenerationInfo& generationInfo)
{
  try
  {
    int result = _getGenerationInfoByName(sessionId,generationName,generationInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  try
  {
    int result = _getGenerationInfoList(sessionId,generationInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList)
{
  try
  {
    int result = _getGenerationInfoListByProducerId(sessionId,producerId,generationInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName,T::GenerationInfoList& generationInfoList)
{
  try
  {
    int result = _getGenerationInfoListByProducerName(sessionId,producerName,generationInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,T::GenerationStatus generationStatus,T::GenerationInfo& generationInfo)
{
  try
  {
    int result = _getLastGenerationInfoByProducerIdAndStatus(sessionId,producerId,generationStatus,generationInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,std::string producerName,T::GenerationStatus generationStatus,T::GenerationInfo& generationInfo)
{
  try
  {
    int result = _getLastGenerationInfoByProducerNameAndStatus(sessionId,producerName,generationStatus,generationInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,T::GenerationStatus status)
{
  try
  {
    int result = _setGenerationInfoStatusById(sessionId,generationId,status);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::setGenerationInfoStatusByName(T::SessionId sessionId,std::string generationName,T::GenerationStatus status)
{
  try
  {
    int result = _setGenerationInfoStatusByName(sessionId,generationName,status);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  try
  {
    int result = _addFileInfo(sessionId,fileInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _addFileInfoWithContentList(sessionId,fileInfo,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoById(T::SessionId sessionId,uint fileId)
{
  try
  {
    int result = _deleteFileInfoById(sessionId,fileId);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoByName(T::SessionId sessionId,std::string filename)
{
  try
  {
    int result = _deleteFileInfoByName(sessionId,filename);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  try
  {
    int result = _deleteFileInfoListByGroupFlags(sessionId,groupFlags);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  try
  {
    int result = _deleteFileInfoListByProducerId(sessionId,producerId);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  try
  {
    int result = _deleteFileInfoListByProducerName(sessionId,producerName);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  try
  {
    int result = _deleteFileInfoListByGenerationId(sessionId,generationId);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  try
  {
    int result = _deleteFileInfoListByGenerationName(sessionId,generationName);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo)
{
  try
  {
    int result = _getFileInfoById(sessionId,fileId,fileInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoByName(T::SessionId sessionId,std::string filename,T::FileInfo& fileInfo)
{
  try
  {
    int result = _getFileInfoByName(sessionId,filename,fileInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoList(T::SessionId sessionId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  try
  {
    int result = _getFileInfoList(sessionId,startFileId,maxRecords,fileInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  try
  {
    int result = _getFileInfoListByProducerId(sessionId,producerId,startFileId,maxRecords,fileInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  try
  {
    int result = _getFileInfoListByProducerName(sessionId,producerName,startFileId,maxRecords,fileInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  try
  {
    int result = _getFileInfoListByGenerationId(sessionId,generationId,startFileId,maxRecords,fileInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  try
  {
    int result = _getFileInfoListByGenerationName(sessionId,generationName,startFileId,maxRecords,fileInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




/*
int ServiceInterface::getFileInfoListByServerId(T::SessionId sessionId,uint serverId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  try
  {
    int result = _getFileInfoListByServerId(sessionId,serverId,startFileId,maxRecords,fileInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}
*/




int ServiceInterface::getFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  try
  {
    int result = _getFileInfoListByGroupFlags(sessionId,groupFlags,startFileId,maxRecords,fileInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  try
  {
    int result = _addEventInfo(sessionId,eventInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  try
  {
    int result = _getLastEventInfo(sessionId,requestingServerId,eventInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,uint maxRecords,T::EventInfoList& eventInfoList)
{
  try
  {
    int result = _getEventInfoList(sessionId,requestingServerId,startEventId,maxRecords,eventInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}






int ServiceInterface::addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  try
  {
    int result = _addContentInfo(sessionId,contentInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _addContentList(sessionId,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex)
{
  try
  {
    int result = _deleteContentInfo(sessionId,fileId,messageIndex);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentListByFileId(T::SessionId sessionId,uint fileId)
{
  try
  {
    int result = _deleteContentListByFileId(sessionId,fileId);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentListByFileName(T::SessionId sessionId,std::string filename)
{
  try
  {
    int result = _deleteContentListByFileName(sessionId,filename);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  try
  {
    int result = _deleteContentListByGroupFlags(sessionId,groupFlags);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentListByProducerId(T::SessionId sessionId,uint producerId)
{
  try
  {
    int result = _deleteContentListByProducerId(sessionId,producerId);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentListByProducerName(T::SessionId sessionId,std::string producerName)
{
  try
  {
    int result = _deleteContentListByProducerName(sessionId,producerName);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentListByGenerationId(T::SessionId sessionId,uint generationId)
{
  try
  {
    int result = _deleteContentListByGenerationId(sessionId,generationId);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  try
  {
    int result = _deleteContentListByGenerationName(sessionId,generationName);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::registerContentList(T::SessionId sessionId,uint serverId,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _registerContentList(sessionId,serverId,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::registerContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  try
  {
    int result = _registerContentListByFileId(sessionId,serverId,fileId);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::unregisterContentList(T::SessionId sessionId,uint serverId)
{
  try
  {
    int result = _unregisterContentList(sessionId,serverId);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::unregisterContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  try
  {
    int result = _unregisterContentListByFileId(sessionId,serverId,fileId);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  try
  {
    int result = _getContentInfo(sessionId,fileId,messageIndex,contentInfo);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _getContentList(sessionId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _getContentListByFileId(sessionId,fileId,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByFileName(T::SessionId sessionId,std::string filename,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _getContentListByFileName(sessionId,filename,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByServerId(T::SessionId sessionId,uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _getContentListByServerId(sessionId,serverId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _getContentListByGroupFlags(sessionId,groupFlags,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _getContentListByProducerId(sessionId,producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _getContentListByProducerName(sessionId,producerName,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _getContentListByGenerationId(sessionId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _getContentListByGenerationName(sessionId,generationName,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTime,endTime,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,std::string generationName,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _getContentListByGenerationNameAndTimeRange(sessionId,generationName,startTime,endTime,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _getContentListByParameter(sessionId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByParameterAndGenerationName(T::SessionId sessionId,std::string generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByParameterAndProducerName(T::SessionId sessionId,std::string producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    int result = _getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::_clear(T::SessionId sessionId)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_reload(T::SessionId sessionId)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_addDataServerInfo(T::SessionId sessionId,T::ServerInfo& serverInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteDataServerInfoById(T::SessionId sessionId,uint serverId)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getDataServerInfoById(T::SessionId sessionId,uint serverId,T::ServerInfo& serverInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getDataServerInfoByName(T::SessionId sessionId,std::string serverName,T::ServerInfo& serverInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getDataServerInfoByIor(T::SessionId sessionId,std::string serverIor,T::ServerInfo& serverInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getDataServerInfoList(T::SessionId sessionId,T::ServerInfoList& serverInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}




int ServiceInterface::_addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteProducerInfoById(T::SessionId sessionId,uint producerId)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteProducerInfoByName(T::SessionId sessionId,std::string producerName)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getProducerInfoByName(T::SessionId sessionId,std::string producerName,T::ProducerInfo& producerInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteGenerationInfoById(T::SessionId sessionId,uint generationId)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteGenerationInfoByName(T::SessionId sessionId,std::string generationName)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getGenerationInfoByName(T::SessionId sessionId,std::string generationName,T::GenerationInfo& generationInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName,T::GenerationInfoList& generationInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,T::GenerationStatus generationStatus,T::GenerationInfo& generationInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,std::string producerName,T::GenerationStatus generationStatus,T::GenerationInfo& generationInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,T::GenerationStatus status)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_setGenerationInfoStatusByName(T::SessionId sessionId,std::string generationName,T::GenerationStatus status)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteFileInfoById(T::SessionId sessionId,uint fileId)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteFileInfoByName(T::SessionId sessionId,std::string filename)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteFileInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getFileInfoByName(T::SessionId sessionId,std::string filename,T::FileInfo& fileInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getFileInfoList(T::SessionId sessionId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getFileInfoListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,uint maxRecords,T::EventInfoList& eventInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_registerContentList(T::SessionId sessionId,uint serverId,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_registerContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_unregisterContentList(T::SessionId sessionId,uint serverId)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_unregisterContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteContentListByFileId(T::SessionId sessionId,uint fileId)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteContentListByFileName(T::SessionId sessionId,std::string filename)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteContentListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteContentListByProducerId(T::SessionId sessionId,uint producerId)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteContentListByProducerName(T::SessionId sessionId,std::string producerName)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteContentListByGenerationId(T::SessionId sessionId,uint generationId)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteContentListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentListByFileName(T::SessionId sessionId,std::string filename,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentListByServerId(T::SessionId sessionId,uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,std::string generationName,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentListByParameterAndGenerationName(T::SessionId sessionId,std::string generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentListByParameterAndProducerName(T::SessionId sessionId,std::string producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}









}
}
