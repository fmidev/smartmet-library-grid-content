#include "ServiceInterface.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/ShowFunction.h>


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
    mDebugLog = nullptr;
    mProcessingLog = nullptr;
    mImplementationType = Implementation::Interface;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





Implementation ServiceInterface::getImplementationType()
{
  FUNCTION_TRACE
  try
  {
    return mImplementationType;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceInterface::setDebugLog(Log *debugLog)
{
  FUNCTION_TRACE
  try
  {
    mDebugLog = debugLog;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceInterface::setProcessingLog(Log *processingLog)
{
  FUNCTION_TRACE
  try
  {
    mProcessingLog = processingLog;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,serverInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,serverId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,serverId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,serverName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,serverIor.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,ServerInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,serverInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,ProducerInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getProducerInfoListByParameter(sessionId,parameterKeyType,parameterKey,producerInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%s,ProducerInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,parameterKeyType,parameterKey.c_str(),producerInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,ProducerInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,producerInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getProducerNameAndGeometryList(sessionId,list);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,string[%lu]);result %d;time %f;",__FUNCTION__,sessionId,list.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getProducerParameterList(sessionId,sourceParameterKeyType,targetParameterKeyType,list);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%d,%d,string[%lu]);result %d;time %f;",__FUNCTION__,sessionId,sourceParameterKeyType,targetParameterKeyType,list.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<uint>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteGenerationInfoListByIdList(sessionId,generationIdList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,generationId[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationIdList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoListByGeometryId(sessionId,geometryId,generationInfoList);;
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,generationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,geometryId,generationInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,GenerationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,GenerationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,generationInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,GenerationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),generationInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,GenerationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,generationInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getLastGenerationInfoByProducerIdAndStatus(sessionId,producerId,generationStatus,generationInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,generationStatus,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,std::string producerName,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getLastGenerationInfoByProducerNameAndStatus(sessionId,producerName,generationStatus,generationInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),generationStatus,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,uchar status)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _setGenerationInfoStatusById(sessionId,generationId,status);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,status,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::setGenerationInfoStatusByName(T::SessionId sessionId,std::string generationName,uchar status)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _setGenerationInfoStatusByName(sessionId,generationName,status);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),status,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,fileInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileInfo.getCsv().c_str(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::addFileInfoListWithContent(T::SessionId sessionId,std::vector<T::FileAndContent>& fileAndContentList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _addFileInfoListWithContent(sessionId,fileAndContentList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,FileAndContent[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileAndContentList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,filename.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,groupFlags,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,std::string forecastTime)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByGenerationIdAndForecastTime(sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%d,%d,%s);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByForecastTimeList(sessionId,forecastTimeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,ForecastTime[%lu]);result %d;time %f;",__FUNCTION__,sessionId,forecastTimeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,fileId[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileIdList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,filename.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,groupFlags,startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getFileInfoCountByProducerId(T::SessionId sessionId,uint producerId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getFileInfoCountByProducerId(sessionId,producerId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getFileInfoCountByGenerationId(T::SessionId sessionId,uint generationId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getFileInfoCountByGenerationId(sessionId,generationId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getFileInfoCountBySourceId(T::SessionId sessionId,uint sourceId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getFileInfoCountBySourceId(sessionId,sourceId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,eventInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    //PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,requestingServerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%llu,%u,EventInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,requestingServerId,startEventId,maxRecords,eventInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,contentInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,filename.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,groupFlags,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,serverId,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,serverId,fileId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,serverId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,serverId,fileId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,filename.c_str(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,serverId,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,groupFlags,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByGenerationId(sessionId,generationId,startFileId,startMessageIndex,maxRecords,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,startFileId,startMessageIndex,maxRecords,requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%s,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,startTime.c_str(),endTime.c_str(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%s,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),startTime.c_str(),endTime.c_str(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByParameter(sessionId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%s,%u,%u,%d,%d,%d,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,parameterKeyType,parameterKey.c_str(),parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%s,%u,%u,%d,%d,%d,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,parameterKeyType,parameterKey.c_str(),parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getContentListByParameterAndGenerationName(T::SessionId sessionId,std::string generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u,%s,%u,%u,%d,%d,%d,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),parameterKeyType,parameterKey.c_str(),parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%s,%u,%u,%d,%d,%d,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,parameterKeyType,parameterKey.c_str(),parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getContentListByParameterAndProducerName(T::SessionId sessionId,std::string producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u,%s,%u,%u,%d,%d,%d,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),parameterKeyType,parameterKey.c_str(),parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string forecastTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterGenerationIdAndForecastTime(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%s,%u,%u,%d,%d,%d,%d,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,parameterKeyType,parameterKey.c_str(),parameterLevelIdType,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime.c_str(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getContentListByRequestCounterKey(T::SessionId sessionId,ulonglong key,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListByRequestCounterKey(sessionId,key,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%llu,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,key,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}






int ServiceInterface::getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentListOfInvalidIntegrity(sessionId,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentGeometryIdListByGenerationId(sessionId,generationId,geometryIdList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,geometryId[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryIdList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,contentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,contentParamList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getContentParamKeyListByGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentParamKeyListByGenerationId(sessionId,generationId,parameterKeyType,paramKeyList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,paramKey[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,parameterKeyType,paramKeyList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




int ServiceInterface::getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentTimeListByGenerationId(sessionId,generationId,contentTimeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,time[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,contentTimeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentTimeListByGenerationAndGeometryId(sessionId,generationId,geometryId,contentTimeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,time[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,contentTimeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getContentTimeListByProducerId(T::SessionId sessionId,uint producerId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getContentTimeListByProducerId(sessionId,producerId,contentTimeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,time[%lu]);result %d;time %f;",__FUNCTION__,sessionId,producerId,contentTimeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,listItem[%lu]);result %d;time %f;",__FUNCTION__,sessionId,list.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getLevelInfoList(sessionId,levelInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,LevelInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,levelInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}






int ServiceInterface::deleteVirtualContent(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _deleteVirtualContent(sessionId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu);result %d;time %f;",__FUNCTION__,sessionId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::updateVirtualContent(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _updateVirtualContent(sessionId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu);result %d;time %f;",__FUNCTION__,sessionId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::_clear(T::SessionId sessionId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_reload(T::SessionId sessionId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_addDataServerInfo(T::SessionId sessionId,T::ServerInfo& serverInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteDataServerInfoById(T::SessionId sessionId,uint serverId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getDataServerInfoById(T::SessionId sessionId,uint serverId,T::ServerInfo& serverInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getDataServerInfoByName(T::SessionId sessionId,std::string serverName,T::ServerInfo& serverInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getDataServerInfoByIor(T::SessionId sessionId,std::string serverIor,T::ServerInfo& serverInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getDataServerInfoList(T::SessionId sessionId,T::ServerInfoList& serverInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getDataServerInfoCount(T::SessionId sessionId,uint& count)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteProducerInfoById(T::SessionId sessionId,uint producerId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteProducerInfoByName(T::SessionId sessionId,std::string producerName)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getProducerInfoByName(T::SessionId sessionId,std::string producerName,T::ProducerInfo& producerInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getProducerInfoCount(T::SessionId sessionId,uint& count)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteGenerationInfoById(T::SessionId sessionId,uint generationId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteGenerationInfoByName(T::SessionId sessionId,std::string generationName)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<uint>& generationIdList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGenerationInfoByName(T::SessionId sessionId,std::string generationName,T::GenerationInfo& generationInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName,T::GenerationInfoList& generationInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,std::string producerName,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGenerationInfoCount(T::SessionId sessionId,uint& count)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,uchar status)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_setGenerationInfoStatusByName(T::SessionId sessionId,std::string generationName,uchar status)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_addFileInfoListWithContent(T::SessionId sessionId,std::vector<T::FileAndContent>& fileAndContentList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteFileInfoById(T::SessionId sessionId,uint fileId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteFileInfoByName(T::SessionId sessionId,std::string filename)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteFileInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,std::string forecastTime)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getFileInfoByName(T::SessionId sessionId,std::string filename,T::FileInfo& fileInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getFileInfoList(T::SessionId sessionId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getFileInfoListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getFileInfoCount(T::SessionId sessionId,uint& count)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getFileInfoCountByProducerId(T::SessionId sessionId,uint producerId,uint& count)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getFileInfoCountByGenerationId(T::SessionId sessionId,uint generationId,uint& count)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getFileInfoCountBySourceId(T::SessionId sessionId,uint sourceId,uint& count)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,uint maxRecords,T::EventInfoList& eventInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getEventInfoCount(T::SessionId sessionId,uint& count)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_registerContentList(T::SessionId sessionId,uint serverId,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_registerContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_unregisterContentList(T::SessionId sessionId,uint serverId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_unregisterContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteContentListByFileId(T::SessionId sessionId,uint fileId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteContentListByFileName(T::SessionId sessionId,std::string filename)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteContentListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteContentListByProducerId(T::SessionId sessionId,uint producerId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteContentListByProducerName(T::SessionId sessionId,std::string producerName)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteContentListByGenerationId(T::SessionId sessionId,uint generationId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteContentListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_deleteContentListBySourceId(T::SessionId sessionId,uint sourceId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByFileName(T::SessionId sessionId,std::string filename,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByServerId(T::SessionId sessionId,uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,std::string generationName,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByParameterAndGenerationName(T::SessionId sessionId,std::string generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByParameterAndProducerName(T::SessionId sessionId,std::string producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string forecastTime,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListByRequestCounterKey(T::SessionId sessionId,ulonglong key,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<T::GeometryId>& geometryIdList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentParamKeyListByGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::set<std::string>& contentTimeList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentTimeListByProducerId(T::SessionId sessionId,uint producerId,std::set<std::string>& contentTimeList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getContentCount(T::SessionId sessionId,uint& count)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}






int ServiceInterface::_deleteVirtualContent(T::SessionId sessionId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_updateVirtualContent(T::SessionId sessionId)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}






}
}
