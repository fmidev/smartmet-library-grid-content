#include "ServiceInterface.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"
#include "grid-files/common/ShowFunction.h"


#define FUNCTION_TRACE FUNCTION_TRACE_OFF



namespace SmartMet
{
namespace ContentServer
{


ServiceInterface::ServiceInterface()
{
  FUNCTION_TRACE
  try
  {
    mProcessingLogPointer = NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ServiceInterface::~ServiceInterface()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceInterface::setProcessingLog(Log *processingLogPointer)
{
  FUNCTION_TRACE
  try
  {
    mProcessingLogPointer = processingLogPointer;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceInterface::shutdown()
{
  FUNCTION_TRACE
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
  FUNCTION_TRACE
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
  FUNCTION_TRACE
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
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _addDataServerInfo(sessionId,serverInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,serverInfo.getCsv().c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteDataServerInfoById(T::SessionId sessionId,uint serverId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteDataServerInfoById(sessionId,serverId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,serverId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getDataServerInfoById(T::SessionId sessionId,uint serverId,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getDataServerInfoById(sessionId,serverId,serverInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,serverId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getDataServerInfoByName(T::SessionId sessionId,std::string serverName,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getDataServerInfoByName(sessionId,serverName,serverInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,serverName.c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getDataServerInfoByIor(T::SessionId sessionId,std::string serverIor,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getDataServerInfoByIor(sessionId,serverIor,serverInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,serverIor.c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getDataServerInfoList(T::SessionId sessionId,T::ServerInfoList& serverInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getDataServerInfoList(sessionId,serverInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,ServerInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,serverInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getDataServerInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getDataServerInfoCount(sessionId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _addProducerInfo(sessionId,producerInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerInfo.getCsv().c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteProducerInfoById(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteProducerInfoById(sessionId,producerId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteProducerInfoByName(T::SessionId sessionId,std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteProducerInfoByName(sessionId,producerName);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteProducerInfoListBySourceId(sessionId,sourceId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getProducerInfoById(sessionId,producerId,producerInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getProducerInfoByName(T::SessionId sessionId,std::string producerName,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getProducerInfoByName(sessionId,producerName,producerInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getProducerInfoList(sessionId,producerInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,ProducerInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getProducerInfoListBySourceId(sessionId,sourceId,producerInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,ProducerInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,producerInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getProducerInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getProducerInfoCount(sessionId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _addGenerationInfo(sessionId,generationInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationInfo.getCsv().c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteGenerationInfoById(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteGenerationInfoById(sessionId,generationId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteGenerationInfoByName(T::SessionId sessionId,std::string generationName)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteGenerationInfoByName(sessionId,generationName);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteGenerationInfoListByProducerId(sessionId,producerId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteGenerationInfoListByProducerName(sessionId,producerName);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteGenerationInfoListBySourceId(sessionId,sourceId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGenerationInfoListByGeometryId(T::SessionId sessionId,uint geometryId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoListByGeometryId(sessionId,geometryId,generationInfoList);;
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,generationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,geometryId,(uint)generationInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoById(sessionId,generationId,generationInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGenerationInfoByName(T::SessionId sessionId,std::string generationName,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoByName(sessionId,generationName,generationInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoList(sessionId,generationInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,GenerationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoListByProducerId(sessionId,producerId,generationInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,GenerationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,generationInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoListByProducerName(sessionId,producerName,generationInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s,GenerationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),generationInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoListBySourceId(sessionId,sourceId,generationInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,GenerationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,generationInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,T::GenerationStatus generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getLastGenerationInfoByProducerIdAndStatus(sessionId,producerId,generationStatus,generationInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,generationStatus,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,std::string producerName,T::GenerationStatus generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getLastGenerationInfoByProducerNameAndStatus(sessionId,producerName,generationStatus,generationInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s,%u);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),generationStatus,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGenerationInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoCount(sessionId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,T::GenerationStatus status)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _setGenerationInfoStatusById(sessionId,generationId,status);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,status,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::setGenerationInfoStatusByName(T::SessionId sessionId,std::string generationName,T::GenerationStatus status)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _setGenerationInfoStatusByName(sessionId,generationName,status);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s,%u);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),status,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _addFileInfo(sessionId,fileInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,fileInfo.getCsv().c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _addFileInfoWithContentList(sessionId,fileInfo,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileInfo.getCsv().c_str(),contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoById(T::SessionId sessionId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoById(sessionId,fileId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoByName(T::SessionId sessionId,std::string filename)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoByName(sessionId,filename);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,filename.c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByGroupFlags(sessionId,groupFlags);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,groupFlags,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByProducerId(sessionId,producerId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByProducerName(sessionId,producerName);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByGenerationId(sessionId,generationId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,uint geometryId,short forecastType,short forecastNumber,std::string forecastTime)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByGenerationIdAndForecastTime(sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%d,%d,%s);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime.c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByGenerationName(sessionId,generationName);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListBySourceId(sessionId,sourceId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByFileIdList(sessionId,fileIdList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,fileId[%u]);result %d;time %f;",__FUNCTION__,sessionId,(uint)fileIdList.size(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getFileInfoById(sessionId,fileId,fileInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoByName(T::SessionId sessionId,std::string filename,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getFileInfoByName(sessionId,filename,fileInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,filename.c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoList(T::SessionId sessionId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getFileInfoList(sessionId,startFileId,maxRecords,fileInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,startFileId,maxRecords,fileInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getFileInfoListByProducerId(sessionId,producerId,startFileId,maxRecords,fileInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,startFileId,maxRecords,fileInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getFileInfoListByProducerName(sessionId,producerName,startFileId,maxRecords,fileInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),startFileId,maxRecords,fileInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getFileInfoListByGenerationId(sessionId,generationId,startFileId,maxRecords,fileInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,startFileId,maxRecords,fileInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getFileInfoListByGenerationName(sessionId,generationName,startFileId,maxRecords,fileInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),startFileId,maxRecords,fileInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getFileInfoListByGroupFlags(sessionId,groupFlags,startFileId,maxRecords,fileInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,groupFlags,startFileId,maxRecords,fileInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getFileInfoListBySourceId(sessionId,sourceId,startFileId,maxRecords,fileInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,startFileId,maxRecords,fileInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getFileInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getFileInfoCount(sessionId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _addEventInfo(sessionId,eventInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,eventInfo.getCsv().c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // ### This method is polled all the time. Let's not write these calls to the processing log.

    //unsigned long long timeStart = getTime();
    int result = _getLastEventInfo(sessionId,requestingServerId,eventInfo);
    //unsigned long requestTime = getTime() - timeStart;

    //PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,requestingServerId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,uint maxRecords,T::EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getEventInfoList(sessionId,requestingServerId,startEventId,maxRecords,eventInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%llu,%u,EventInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,requestingServerId,startEventId,maxRecords,eventInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}






int ServiceInterface::getEventInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getEventInfoCount(sessionId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _addContentInfo(sessionId,contentInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,contentInfo.getCsv().c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _addContentList(sessionId,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteContentInfo(sessionId,fileId,messageIndex);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentListByFileId(T::SessionId sessionId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteContentListByFileId(sessionId,fileId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentListByFileName(T::SessionId sessionId,std::string filename)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteContentListByFileName(sessionId,filename);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,filename.c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteContentListByGroupFlags(sessionId,groupFlags);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,groupFlags,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteContentListByProducerId(sessionId,producerId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentListByProducerName(T::SessionId sessionId,std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteContentListByProducerName(sessionId,producerName);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteContentListByGenerationId(sessionId,generationId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteContentListByGenerationName(sessionId,generationName);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::deleteContentListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteContentListBySourceId(sessionId,sourceId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::registerContentList(T::SessionId sessionId,uint serverId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _registerContentList(sessionId,serverId,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,serverId,contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::registerContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _registerContentListByFileId(sessionId,serverId,fileId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,serverId,fileId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::unregisterContentList(T::SessionId sessionId,uint serverId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _unregisterContentList(sessionId,serverId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,serverId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::unregisterContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _unregisterContentListByFileId(sessionId,serverId,fileId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,serverId,fileId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentInfo(sessionId,fileId,messageIndex,contentInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentList(sessionId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByFileId(sessionId,fileId,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByFileName(T::SessionId sessionId,std::string filename,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByFileName(sessionId,filename,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,filename.c_str(),contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByServerId(T::SessionId sessionId,uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByServerId(sessionId,serverId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,serverId,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByGroupFlags(sessionId,groupFlags,startFileId,startMessageIndex,maxRecords,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,groupFlags,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByProducerId(sessionId,producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByProducerName(sessionId,producerName,startFileId,startMessageIndex,maxRecords,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListBySourceId(sessionId,sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByGenerationId(sessionId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByGenerationName(sessionId,generationName,startFileId,startMessageIndex,maxRecords,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTime,endTime,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%s,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,startTime.c_str(),endTime.c_str(),contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,std::string generationName,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByGenerationNameAndTimeRange(sessionId,generationName,startTime,endTime,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s,%s,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),startTime.c_str(),endTime.c_str(),contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByParameter(sessionId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%s,%u,%u,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,(uint)parameterKeyType,parameterKey.c_str(),(uint)parameterLevelIdType,(uint)parameterLevelId,minLevel,maxLevel,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%s,%u,%u,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,(uint)parameterKeyType,parameterKey.c_str(),(uint)parameterLevelIdType,(uint)parameterLevelId,minLevel,maxLevel,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByParameterAndGenerationName(T::SessionId sessionId,std::string generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s,%u,%s,%u,%u,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),(uint)parameterKeyType,parameterKey.c_str(),(uint)parameterLevelIdType,(uint)parameterLevelId,minLevel,maxLevel,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%s,%u,%u,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,(uint)parameterKeyType,parameterKey.c_str(),(uint)parameterLevelIdType,(uint)parameterLevelId,minLevel,maxLevel,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentListByParameterAndProducerName(T::SessionId sessionId,std::string producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%s,%u,%s,%u,%u,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),(uint)parameterKeyType,parameterKey.c_str(),(uint)parameterLevelIdType,(uint)parameterLevelId,minLevel,maxLevel,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<uint>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentGeometryIdListByGenerationId(sessionId,generationId,geometryIdList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,geometryId[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,(uint)geometryIdList.size(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentParamListByGenerationId(sessionId,generationId,contentParamList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,contentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,contentParamList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,uint geometryId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentTimeListByGenerationAndGeometryId(sessionId,generationId,geometryId,contentTimeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,time[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,(uint)contentTimeList.size(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGenerationIdGeometryIdAndForecastTimeList(sessionId,list);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,listItem[%u]);result %d;time %f;",__FUNCTION__,sessionId,(uint)list.size(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getContentCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentCount(sessionId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,(float)requestTime / 1000000);
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





int ServiceInterface::_getDataServerInfoCount(T::SessionId sessionId,uint& count)
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





int ServiceInterface::_deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId)
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





int ServiceInterface::_getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getProducerInfoCount(T::SessionId sessionId,uint& count)
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





int ServiceInterface::_deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId)
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





int ServiceInterface::_getGenerationInfoListByGeometryId(T::SessionId sessionId,uint geometryId,T::GenerationInfoList& generationInfoList)
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





int ServiceInterface::_getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList)
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





int ServiceInterface::_getGenerationInfoCount(T::SessionId sessionId,uint& count)
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





int ServiceInterface::_deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,uint geometryId,short forecastType,short forecastNumber,std::string forecastTime)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList)
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





int ServiceInterface::_getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getFileInfoCount(T::SessionId sessionId,uint& count)
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





int ServiceInterface::_getEventInfoCount(T::SessionId sessionId,uint& count)
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





int ServiceInterface::_deleteContentListBySourceId(T::SessionId sessionId,uint sourceId)
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





int ServiceInterface::_getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
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





int ServiceInterface::_getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<uint>& geometryIdList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,uint geometryId,std::set<std::string>& contentTimeList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getContentCount(T::SessionId sessionId,uint& count)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}



}
}
