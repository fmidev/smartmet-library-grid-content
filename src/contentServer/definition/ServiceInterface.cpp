#include "ServiceInterface.h"
#include <macgyver/Exception.h>
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
    mEnabled = true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void ServiceInterface::setEnabled(bool enabled)
{
  FUNCTION_TRACE
  try
  {
    mEnabled = enabled;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool ServiceInterface::isEnabled()
{
  FUNCTION_TRACE
  try
  {
    return mEnabled;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceInterface::getCacheStats(Fmi::Cache::CacheStatistics& statistics) const
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





time_t ServiceInterface::getContentChangeTime()
{
  FUNCTION_TRACE
  try
  {
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





Log* ServiceInterface::getDebugLog()
{
  FUNCTION_TRACE
  try
  {
    return mDebugLog;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




Log* ServiceInterface::getProcessingLog()
{
  FUNCTION_TRACE
  try
  {
    return mProcessingLog;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::string& ServiceInterface::getSourceInfo()
{
  FUNCTION_TRACE
  try
  {
    return mSourceInfo;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::clear(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    int result = _clear(sessionId);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::reload(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    int result = _reload(sessionId);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentChangeTime(T::SessionId sessionId,time_t& changeTime)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    int result = _getContentChangeTime(sessionId,changeTime);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






int ServiceInterface::addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addProducerInfo(sessionId,producerInfo);


    unsigned long long timeStart = getTime();
    int result = _addProducerInfo(sessionId,producerInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::setProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _setProducerInfo(sessionId,producerInfo);

    unsigned long long timeStart = getTime();
    int result = _setProducerInfo(sessionId,producerInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteProducerInfoById(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteProducerInfoById(sessionId,producerId);

    unsigned long long timeStart = getTime();
    int result = _deleteProducerInfoById(sessionId,producerId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteProducerInfoByName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteProducerInfoByName(sessionId,producerName);

    unsigned long long timeStart = getTime();
    int result = _deleteProducerInfoByName(sessionId,producerName);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteProducerInfoListBySourceId(sessionId,sourceId);

    unsigned long long timeStart = getTime();
    int result = _deleteProducerInfoListBySourceId(sessionId,sourceId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerInfoById(sessionId,producerId,producerInfo);

    unsigned long long timeStart = getTime();
    int result = _getProducerInfoById(sessionId,producerId,producerInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getProducerInfoByName(T::SessionId sessionId,const std::string& producerName,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerInfoByName(sessionId,producerName,producerInfo);

    unsigned long long timeStart = getTime();
    int result = _getProducerInfoByName(sessionId,producerName,producerInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerInfoList(sessionId,producerInfoList);

    unsigned long long timeStart = getTime();
    int result = _getProducerInfoList(sessionId,producerInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,ProducerInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerInfoListByParameter(sessionId,parameterKeyType,parameterKey,producerInfoList);

    unsigned long long timeStart = getTime();
    int result = _getProducerInfoListByParameter(sessionId,parameterKeyType,parameterKey,producerInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%s,ProducerInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,parameterKeyType,parameterKey.c_str(),producerInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerInfoListBySourceId(sessionId,sourceId,producerInfoList);

    unsigned long long timeStart = getTime();
    int result = _getProducerInfoListBySourceId(sessionId,sourceId,producerInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,ProducerInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,producerInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getProducerInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerInfoCount(sessionId,count);

    unsigned long long timeStart = getTime();
    int result = _getProducerInfoCount(sessionId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerNameAndGeometryList(sessionId,list);

    unsigned long long timeStart = getTime();
    int result = _getProducerNameAndGeometryList(sessionId,list);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,string[%lu]);result %d;time %f;",__FUNCTION__,sessionId,list.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerParameterList(sessionId,sourceParameterKeyType,targetParameterKeyType,list);

    unsigned long long timeStart = getTime();
    int result = _getProducerParameterList(sessionId,sourceParameterKeyType,targetParameterKeyType,list);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%d,%d,string[%lu]);result %d;time %f;",__FUNCTION__,sessionId,sourceParameterKeyType,targetParameterKeyType,list.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getProducerParameterListByProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerParameterListByProducerId(sessionId,producerId,sourceParameterKeyType,targetParameterKeyType,list);

    unsigned long long timeStart = getTime();
    int result = _getProducerParameterListByProducerId(sessionId,producerId,sourceParameterKeyType,targetParameterKeyType,list);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%d,%d,string[%lu]);result %d;time %f;",__FUNCTION__,sessionId,producerId,sourceParameterKeyType,targetParameterKeyType,list.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addGenerationInfo(sessionId,generationInfo);

    unsigned long long timeStart = getTime();
    int result = _addGenerationInfo(sessionId,generationInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::setGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _setGenerationInfo(sessionId,generationInfo);

    unsigned long long timeStart = getTime();
    int result = _setGenerationInfo(sessionId,generationInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteGenerationInfoById(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGenerationInfoById(sessionId,generationId);

    unsigned long long timeStart = getTime();
    int result = _deleteGenerationInfoById(sessionId,generationId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteGenerationInfoByName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGenerationInfoByName(sessionId,generationName);

    unsigned long long timeStart = getTime();
    int result = _deleteGenerationInfoByName(sessionId,generationName);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<uint>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGenerationInfoListByIdList(sessionId,generationIdList);

    unsigned long long timeStart = getTime();
    int result = _deleteGenerationInfoListByIdList(sessionId,generationIdList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,generationId[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationIdList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGenerationInfoListByProducerId(sessionId,producerId);

    unsigned long long timeStart = getTime();
    int result = _deleteGenerationInfoListByProducerId(sessionId,producerId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGenerationInfoListByProducerName(sessionId,producerName);

    unsigned long long timeStart = getTime();
    int result = _deleteGenerationInfoListByProducerName(sessionId,producerName);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGenerationInfoListBySourceId(sessionId,sourceId);

    unsigned long long timeStart = getTime();
    int result = _deleteGenerationInfoListBySourceId(sessionId,sourceId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationInfoListByGeometryId(sessionId,geometryId,generationInfoList);;

    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoListByGeometryId(sessionId,geometryId,generationInfoList);;
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,generationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,geometryId,generationInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationInfoById(sessionId,generationId,generationInfo);

    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoById(sessionId,generationId,generationInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGenerationInfoByName(T::SessionId sessionId,const std::string& generationName,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationInfoByName(sessionId,generationName,generationInfo);

    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoByName(sessionId,generationName,generationInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationInfoList(sessionId,generationInfoList);

    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoList(sessionId,generationInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,GenerationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationInfoListByProducerId(sessionId,producerId,generationInfoList);

    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoListByProducerId(sessionId,producerId,generationInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,GenerationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,generationInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationInfoListByProducerName(sessionId,producerName,generationInfoList);

    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoListByProducerName(sessionId,producerName,generationInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,GenerationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),generationInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationInfoListBySourceId(sessionId,sourceId,generationInfoList);

    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoListBySourceId(sessionId,sourceId,generationInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,GenerationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,generationInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getLastGenerationInfoByProducerIdAndStatus(sessionId,producerId,generationStatus,generationInfo);

    unsigned long long timeStart = getTime();
    int result = _getLastGenerationInfoByProducerIdAndStatus(sessionId,producerId,generationStatus,generationInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,generationStatus,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,const std::string& producerName,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getLastGenerationInfoByProducerNameAndStatus(sessionId,producerName,generationStatus,generationInfo);

    unsigned long long timeStart = getTime();
    int result = _getLastGenerationInfoByProducerNameAndStatus(sessionId,producerName,generationStatus,generationInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),generationStatus,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGenerationInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationInfoCount(sessionId,count);

    unsigned long long timeStart = getTime();
    int result = _getGenerationInfoCount(sessionId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,uchar status)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _setGenerationInfoStatusById(sessionId,generationId,status);

    unsigned long long timeStart = getTime();
    int result = _setGenerationInfoStatusById(sessionId,generationId,status);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,status,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::setGenerationInfoStatusByName(T::SessionId sessionId,const std::string& generationName,uchar status)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _setGenerationInfoStatusByName(sessionId,generationName,status);

    unsigned long long timeStart = getTime();
    int result = _setGenerationInfoStatusByName(sessionId,generationName,status);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),status,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::addGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addGeometryInfo(sessionId,geometryInfo);

    unsigned long long timeStart = getTime();
    int result = _addGeometryInfo(sessionId,geometryInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,geometryInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGeometryInfoById(sessionId,generationId,geometryId,levelId);

    unsigned long long timeStart = getTime();
    int result = _deleteGeometryInfoById(sessionId,generationId,geometryId,levelId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%d,%d);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,levelId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGeometryInfoListByGenerationId(sessionId,generationId);

    unsigned long long timeStart = getTime();
    int result = _deleteGeometryInfoListByGenerationId(sessionId,generationId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGeometryInfoListByProducerId(sessionId,producerId);

    unsigned long long timeStart = getTime();
    int result = _deleteGeometryInfoListByProducerId(sessionId,producerId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGeometryInfoListBySourceId(sessionId,sourceId);

    unsigned long long timeStart = getTime();
    int result = _deleteGeometryInfoListBySourceId(sessionId,sourceId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGeometryInfoById(sessionId,generationId,geometryId,levelId,geometryInfo);

    unsigned long long timeStart = getTime();
    int result = _getGeometryInfoById(sessionId,generationId,geometryId,levelId,geometryInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%d,%d);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,levelId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGeometryInfoList(T::SessionId sessionId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGeometryInfoList(sessionId,geometryInfoList);

    unsigned long long timeStart = getTime();
    int result = _getGeometryInfoList(sessionId,geometryInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,geometryInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,geometryInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::getGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGeometryInfoListByGenerationId(sessionId,generationId,geometryInfoList);

    unsigned long long timeStart = getTime();
    int result = _getGeometryInfoListByGenerationId(sessionId,generationId,geometryInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,geometryInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::getGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGeometryInfoListByProducerId(sessionId,producerId,geometryInfoList);

    unsigned long long timeStart = getTime();
    int result = _getGeometryInfoListByProducerId(sessionId,producerId,geometryInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,geometryInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,geometryInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::getGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGeometryInfoListBySourceId(sessionId,sourceId,geometryInfoList);

    unsigned long long timeStart = getTime();
    int result = _getGeometryInfoListBySourceId(sessionId,sourceId,geometryInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,geometryInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,geometryInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGeometryInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGeometryInfoCount(sessionId,count);

    unsigned long long timeStart = getTime();
    int result = _getGeometryInfoCount(sessionId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::setGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _setGeometryInfo(sessionId,geometryInfo);

    unsigned long long timeStart = getTime();
    int result = _setGeometryInfo(sessionId,geometryInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,geometryInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::setGeometryInfoStatusById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,uchar status)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _setGeometryInfoStatusById(sessionId,generationId,geometryId,levelId,status);

    unsigned long long timeStart = getTime();
    int result = _setGeometryInfoStatusById(sessionId,generationId,geometryId,levelId,status);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%d,%d,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,levelId,status,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addFileInfo(sessionId,fileInfo);

    unsigned long long timeStart = getTime();
    int result = _addFileInfo(sessionId,fileInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,fileInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::setFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _setFileInfo(sessionId,fileInfo);

    unsigned long long timeStart = getTime();
    int result = _setFileInfo(sessionId,fileInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,fileInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addFileInfoWithContentList(sessionId,fileInfo,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _addFileInfoWithContentList(sessionId,fileInfo,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileInfo.getCsv().c_str(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addFileInfoListWithContent(sessionId,requestFlags,fileAndContentList);

    unsigned long long timeStart = getTime();
    int result = _addFileInfoListWithContent(sessionId,requestFlags,fileAndContentList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,FileAndContent[%lu]);result %d;time %f;",__FUNCTION__,sessionId,requestFlags,fileAndContentList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteFileInfoById(T::SessionId sessionId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoById(sessionId,fileId);

    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoById(sessionId,fileId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteFileInfoByName(T::SessionId sessionId,const std::string& filename)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoByName(sessionId,filename);

    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoByName(sessionId,filename);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,filename.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListByProducerId(sessionId,producerId);

    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByProducerId(sessionId,producerId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListByProducerName(sessionId,producerName);

    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByProducerName(sessionId,producerName);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListByGenerationId(sessionId,generationId);

    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByGenerationId(sessionId,generationId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,const std::string& forecastTime)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListByGenerationIdAndForecastTime(sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime);

    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByGenerationIdAndForecastTime(sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%d,%d,%s);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,time_t forecastTime)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListByGenerationIdAndForecastTime(sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime);

    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByGenerationIdAndForecastTime(sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%d,%d,%lu);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListByForecastTimeList(sessionId,forecastTimeList);

    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByForecastTimeList(sessionId,forecastTimeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,ForecastTime[%lu]);result %d;time %f;",__FUNCTION__,sessionId,forecastTimeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListByGenerationName(sessionId,generationName);

    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByGenerationName(sessionId,generationName);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListBySourceId(sessionId,sourceId);

    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListBySourceId(sessionId,sourceId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListByFileIdList(sessionId,fileIdList);

    unsigned long long timeStart = getTime();
    int result = _deleteFileInfoListByFileIdList(sessionId,fileIdList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,fileId[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileIdList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoById(sessionId,fileId,fileInfo);

    unsigned long long timeStart = getTime();
    int result = _getFileInfoById(sessionId,fileId,fileInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getFileInfoByName(T::SessionId sessionId,const std::string& filename,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoByName(sessionId,filename,fileInfo);

    unsigned long long timeStart = getTime();
    int result = _getFileInfoByName(sessionId,filename,fileInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,filename.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getFileInfoList(T::SessionId sessionId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoList(sessionId,startFileId,maxRecords,fileInfoList);

    unsigned long long timeStart = getTime();
    int result = _getFileInfoList(sessionId,startFileId,maxRecords,fileInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoListByFileIdList(sessionId,fileIdList,fileInfoList);

    unsigned long long timeStart = getTime();
    int result = _getFileInfoListByFileIdList(sessionId,fileIdList,fileInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,fileId[%lu],FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileIdList.size(),fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoListByProducerId(sessionId,producerId,startFileId,maxRecords,fileInfoList);

    unsigned long long timeStart = getTime();
    int result = _getFileInfoListByProducerId(sessionId,producerId,startFileId,maxRecords,fileInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoListByProducerName(sessionId,producerName,startFileId,maxRecords,fileInfoList);

    unsigned long long timeStart = getTime();
    int result = _getFileInfoListByProducerName(sessionId,producerName,startFileId,maxRecords,fileInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoListByGenerationId(sessionId,generationId,startFileId,maxRecords,fileInfoList);

    unsigned long long timeStart = getTime();
    int result = _getFileInfoListByGenerationId(sessionId,generationId,startFileId,maxRecords,fileInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoListByGenerationName(sessionId,generationName,startFileId,maxRecords,fileInfoList);

    unsigned long long timeStart = getTime();
    int result = _getFileInfoListByGenerationName(sessionId,generationName,startFileId,maxRecords,fileInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoListBySourceId(sessionId,sourceId,startFileId,maxRecords,fileInfoList);

    unsigned long long timeStart = getTime();
    int result = _getFileInfoListBySourceId(sessionId,sourceId,startFileId,maxRecords,fileInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getFileInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoCount(sessionId,count);

    unsigned long long timeStart = getTime();
    int result = _getFileInfoCount(sessionId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getFileInfoCountByProducerId(T::SessionId sessionId,uint producerId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoCountByProducerId(sessionId,producerId,count);

    unsigned long long timeStart = getTime();
    int result = _getFileInfoCountByProducerId(sessionId,producerId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getFileInfoCountByGenerationId(T::SessionId sessionId,uint generationId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoCountByGenerationId(sessionId,generationId,count);

    unsigned long long timeStart = getTime();
    int result = _getFileInfoCountByGenerationId(sessionId,generationId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getFileInfoCountBySourceId(T::SessionId sessionId,uint sourceId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoCountBySourceId(sessionId,sourceId,count);

    unsigned long long timeStart = getTime();
    int result = _getFileInfoCountBySourceId(sessionId,sourceId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addEventInfo(sessionId,eventInfo);

    unsigned long long timeStart = getTime();
    int result = _addEventInfo(sessionId,eventInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,eventInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    // ### This method is polled all the time. Let's not write these calls to the processing log.

    //unsigned long long timeStart = getTime();
    int result = _getLastEventInfo(sessionId,requestingServerId,eventInfo);
    //unsigned long requestTime = getTime() - timeStart;

    //PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,requestingServerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getEventInfoList(sessionId,requestingServerId,startEventId,maxRecords,eventInfoList);

    unsigned long long timeStart = getTime();
    int result = _getEventInfoList(sessionId,requestingServerId,startEventId,maxRecords,eventInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%llu,%u,EventInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,requestingServerId,startEventId,maxRecords,eventInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






int ServiceInterface::getEventInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getEventInfoCount(sessionId,count);

    unsigned long long timeStart = getTime();
    int result = _getEventInfoCount(sessionId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addContentInfo(sessionId,contentInfo);

    unsigned long long timeStart = getTime();
    int result = _addContentInfo(sessionId,contentInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,contentInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::setContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _setContentInfo(sessionId,contentInfo);

    unsigned long long timeStart = getTime();
    int result = _setContentInfo(sessionId,contentInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,contentInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addContentList(sessionId,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _addContentList(sessionId,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteContentInfo(sessionId,fileId,messageIndex);

    unsigned long long timeStart = getTime();
    int result = _deleteContentInfo(sessionId,fileId,messageIndex);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteContentListByFileId(T::SessionId sessionId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteContentListByFileId(sessionId,fileId);

    unsigned long long timeStart = getTime();
    int result = _deleteContentListByFileId(sessionId,fileId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteContentListByFileName(T::SessionId sessionId,const std::string& filename)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteContentListByFileName(sessionId,filename);

    unsigned long long timeStart = getTime();
    int result = _deleteContentListByFileName(sessionId,filename);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,filename.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteContentListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteContentListByProducerId(sessionId,producerId);

    unsigned long long timeStart = getTime();
    int result = _deleteContentListByProducerId(sessionId,producerId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteContentListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteContentListByProducerName(sessionId,producerName);

    unsigned long long timeStart = getTime();
    int result = _deleteContentListByProducerName(sessionId,producerName);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteContentListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteContentListByGenerationId(sessionId,generationId);

    unsigned long long timeStart = getTime();
    int result = _deleteContentListByGenerationId(sessionId,generationId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteContentListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteContentListByGenerationName(sessionId,generationName);

    unsigned long long timeStart = getTime();
    int result = _deleteContentListByGenerationName(sessionId,generationName);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::deleteContentListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteContentListBySourceId(sessionId,sourceId);

    unsigned long long timeStart = getTime();
    int result = _deleteContentListBySourceId(sessionId,sourceId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentInfo(sessionId,fileId,messageIndex,contentInfo);

    unsigned long long timeStart = getTime();
    int result = _getContentInfo(sessionId,fileId,messageIndex,contentInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentList(sessionId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentList(sessionId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByFileId(sessionId,fileId,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByFileId(sessionId,fileId,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByFileIdList(sessionId,fileIdList,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByFileIdList(sessionId,fileIdList,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,fileId[%lu],ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileIdList.size(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByFileName(T::SessionId sessionId,const std::string& filename,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByFileName(sessionId,filename,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByFileName(sessionId,filename,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,filename.c_str(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByProducerId(sessionId,producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByProducerId(sessionId,producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByProducerName(sessionId,producerName,startFileId,startMessageIndex,maxRecords,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByProducerName(sessionId,producerName,startFileId,startMessageIndex,maxRecords,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListBySourceId(sessionId,sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListBySourceId(sessionId,sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,int maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByGenerationId(sessionId,generationId,startFileId,startMessageIndex,maxRecords,requestFlags,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByGenerationId(sessionId,generationId,startFileId,startMessageIndex,maxRecords,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,startFileId,startMessageIndex,maxRecords,requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByGenerationName(sessionId,generationName,startFileId,startMessageIndex,maxRecords,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByGenerationName(sessionId,generationName,startFileId,startMessageIndex,maxRecords,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTime,endTime,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTime,endTime,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%s,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,startTime.c_str(),endTime.c_str(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTime,endTime,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTime,endTime,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%lu,%lu,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,startTime,endTime,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByGenerationNameAndTimeRange(sessionId,generationName,startTime,endTime,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByGenerationNameAndTimeRange(sessionId,generationName,startTime,endTime,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%s,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),startTime.c_str(),endTime.c_str(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByGenerationNameAndTimeRange(sessionId,generationName,startTime,endTime,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByGenerationNameAndTimeRange(sessionId,generationName,startTime,endTime,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%lu,%lu,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),startTime,endTime,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameter(sessionId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByParameter(sessionId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%s,%d,%d,%d,%d,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameter(sessionId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByParameter(sessionId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%s,%d,%d,%d,%d,%d,%d,%lu,%lu,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%s,%d,%d,%d,%d,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%s,%d,%d,%d,%d,%d,%d,%lu,%lu,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u,%s,%d,%d,%d,%d,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u,%s,%d,%d,%d,%d,%d,%d,%lu,%lu,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%s,%d,%d,%d,%d,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%s,%d,%d,%d,%d,%d,%d,%lu,%lu,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u,%s,%d,%d,%d,%d,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%u,%s,%d,%d,%d,%d,%d,%d,%lu,%lu,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterGenerationIdAndForecastTime(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterGenerationIdAndForecastTime(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%s,%d,%d,%d,%d,%d,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,parameterKeyType,parameterKey.c_str(),parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime.c_str(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterGenerationIdAndForecastTime(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListByParameterGenerationIdAndForecastTime(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%s,%d,%d,%d,%d,%d,%lu,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,parameterKeyType,parameterKey.c_str(),parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListOfInvalidIntegrity(sessionId,contentInfoList);

    unsigned long long timeStart = getTime();
    int result = _getContentListOfInvalidIntegrity(sessionId,contentInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentGeometryIdListByGenerationId(sessionId,generationId,geometryIdList);

    unsigned long long timeStart = getTime();
    int result = _getContentGeometryIdListByGenerationId(sessionId,generationId,geometryIdList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,geometryId[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryIdList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentLevelListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentLevelListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,contentLevelList);

    unsigned long long timeStart = getTime();
    int result = _getContentLevelListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,contentLevelList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,time[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,levelId,contentLevelList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentParamListByGenerationId(sessionId,generationId,contentParamList);

    unsigned long long timeStart = getTime();
    int result = _getContentParamListByGenerationId(sessionId,generationId,contentParamList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,contentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,contentParamList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentParamKeyListByGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentParamKeyListByGenerationId(sessionId,generationId,parameterKeyType,paramKeyList);

    unsigned long long timeStart = getTime();
    int result = _getContentParamKeyListByGenerationId(sessionId,generationId,parameterKeyType,paramKeyList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,paramKey[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,parameterKeyType,paramKeyList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentParamKeyListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentParamKeyListByGenerationAndGeometryId(sessionId,generationId,geometryId,parameterKeyType,paramKeyList);

    unsigned long long timeStart = getTime();
    int result = _getContentParamKeyListByGenerationAndGeometryId(sessionId,generationId,geometryId,parameterKeyType,paramKeyList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,paramKey[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,parameterKeyType,paramKeyList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentParamKeyListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentParamKeyListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,parameterKeyType,paramKeyList);

    unsigned long long timeStart = getTime();
    int result = _getContentParamKeyListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,parameterKeyType,paramKeyList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,paramKey[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,levelId,parameterKeyType,paramKeyList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentTimeListByGenerationId(sessionId,generationId,contentTimeList);

    unsigned long long timeStart = getTime();
    int result = _getContentTimeListByGenerationId(sessionId,generationId,contentTimeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,time[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,contentTimeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentTimeListByGenerationAndGeometryId(sessionId,generationId,geometryId,contentTimeList);

    unsigned long long timeStart = getTime();
    int result = _getContentTimeListByGenerationAndGeometryId(sessionId,generationId,geometryId,contentTimeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,time[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,contentTimeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentTimeListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentTimeListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,contentTimeList);

    unsigned long long timeStart = getTime();
    int result = _getContentTimeListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,contentTimeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,time[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,levelId,contentTimeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentTimeListByProducerId(T::SessionId sessionId,uint producerId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentTimeListByProducerId(sessionId,producerId,contentTimeList);

    unsigned long long timeStart = getTime();
    int result = _getContentTimeListByProducerId(sessionId,producerId,contentTimeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,time[%lu]);result %d;time %f;",__FUNCTION__,sessionId,producerId,contentTimeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentTimeRangeByProducerAndGenerationId(T::SessionId sessionId,uint producerId,uint generationId,time_t& startTime,time_t& endTime)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentTimeRangeByProducerAndGenerationId(sessionId,producerId,generationId,startTime,endTime);

    unsigned long long timeStart = getTime();
    int result = _getContentTimeRangeByProducerAndGenerationId(sessionId,producerId,generationId,startTime,endTime);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%lu,%lu);result %d;time %f;",__FUNCTION__,sessionId,producerId,generationId,startTime,endTime,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentTimeRangeByGenerationId(T::SessionId sessionId,uint generationId,time_t& startTime,time_t& endTime)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentTimeRangeByGenerationId(sessionId,generationId,startTime,endTime);

    unsigned long long timeStart = getTime();
    int result = _getContentTimeRangeByGenerationId(sessionId,generationId,startTime,endTime);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%lu,%lu);result %d;time %f;",__FUNCTION__,sessionId,generationId,startTime,endTime,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationIdGeometryIdAndForecastTimeList(sessionId,list);

    unsigned long long timeStart = getTime();
    int result = _getGenerationIdGeometryIdAndForecastTimeList(sessionId,list);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,listItem[%lu]);result %d;time %f;",__FUNCTION__,sessionId,list.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getContentCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentCount(sessionId,count);

    unsigned long long timeStart = getTime();
    int result = _getContentCount(sessionId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getHashByProducerId(T::SessionId sessionId,uint producerId,ulonglong& hash)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getHashByProducerId(sessionId,producerId,hash);

    unsigned long long timeStart = getTime();
    int result = _getHashByProducerId(sessionId,producerId,hash);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%llu);result %d;time %f;",__FUNCTION__,sessionId,producerId,hash,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getLevelInfoList(sessionId,levelInfoList);

    unsigned long long timeStart = getTime();
    int result = _getLevelInfoList(sessionId,levelInfoList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,LevelInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,levelInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::updateCachedFiles(T::SessionId sessionId,std::set<uint>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _updateCachedFiles(sessionId,fileIdList);

    unsigned long long timeStart = getTime();
    int result = _updateCachedFiles(sessionId,fileIdList);;
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,fileIdList[%ld]);result %d;time %f;",__FUNCTION__,sessionId,fileIdList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::_clear(T::SessionId sessionId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_reload(T::SessionId sessionId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentChangeTime(T::SessionId sessionId,time_t& changeTime)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_setProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteProducerInfoById(T::SessionId sessionId,uint producerId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteProducerInfoByName(T::SessionId sessionId,const std::string& producerName)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getProducerInfoByName(T::SessionId sessionId,const std::string& producerName,T::ProducerInfo& producerInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getProducerInfoCount(T::SessionId sessionId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getProducerParameterListByProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_setGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteGenerationInfoById(T::SessionId sessionId,uint generationId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteGenerationInfoByName(T::SessionId sessionId,const std::string& generationName)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<uint>& generationIdList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGenerationInfoByName(T::SessionId sessionId,const std::string& generationName,T::GenerationInfo& generationInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::GenerationInfoList& generationInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,const std::string& producerName,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGenerationInfoCount(T::SessionId sessionId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,uchar status)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_setGenerationInfoStatusByName(T::SessionId sessionId,const std::string& generationName,uchar status)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_addGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGeometryInfoList(T::SessionId sessionId,T::GeometryInfoList& geometryInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId,T::GeometryInfoList& geometryInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GeometryInfoList& geometryInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GeometryInfoList& geometryInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGeometryInfoCount(T::SessionId sessionId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_setGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_setGeometryInfoStatusById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,uchar status)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_setFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteFileInfoById(T::SessionId sessionId,uint fileId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteFileInfoByName(T::SessionId sessionId,const std::string& filename)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,const std::string& forecastTime)
try
{
  time_t forecastTimeUTC = utcTimeToTimeT(forecastTime);
  return _deleteFileInfoListByGenerationIdAndForecastTime(sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTimeUTC);
}
catch (...)
{
  throw Fmi::Exception(BCP,"Operation failed!",nullptr);
}





int ServiceInterface::_deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,time_t forecastTime)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getFileInfoByName(T::SessionId sessionId,const std::string& filename,T::FileInfo& fileInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getFileInfoList(T::SessionId sessionId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::FileInfoList& fileInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getFileInfoCount(T::SessionId sessionId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getFileInfoCountByProducerId(T::SessionId sessionId,uint producerId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getFileInfoCountByGenerationId(T::SessionId sessionId,uint generationId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getFileInfoCountBySourceId(T::SessionId sessionId,uint sourceId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getEventInfoCount(T::SessionId sessionId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_setContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteContentListByFileId(T::SessionId sessionId,uint fileId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteContentListByFileName(T::SessionId sessionId,const std::string& filename)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteContentListByProducerId(T::SessionId sessionId,uint producerId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteContentListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteContentListByGenerationId(T::SessionId sessionId,uint generationId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteContentListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_deleteContentListBySourceId(T::SessionId sessionId,uint sourceId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentListByFileName(T::SessionId sessionId,const std::string& filename,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,int maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList)
{
  try
  {
    time_t startTimeUTC = utcTimeToTimeT(startTime);
    time_t endTimeUTC = utcTimeToTimeT(endTime);
    return _getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTimeUTC,endTimeUTC,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList)
{
  try
  {
    time_t startTimeUTC = utcTimeToTimeT(startTime);
    time_t endTimeUTC = utcTimeToTimeT(endTime);
    return _getContentListByGenerationNameAndTimeRange(sessionId,generationName,startTimeUTC,endTimeUTC,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    time_t startTimeUTC = utcTimeToTimeT(startTime);
    time_t endTimeUTC = utcTimeToTimeT(endTime);
    return _getContentListByParameter(sessionId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTimeUTC,endTimeUTC,requestFlags,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    time_t startTimeUTC = utcTimeToTimeT(startTime);
    time_t endTimeUTC = utcTimeToTimeT(endTime);
    return _getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTimeUTC,endTimeUTC,requestFlags,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    time_t startTimeUTC = utcTimeToTimeT(startTime);
    time_t endTimeUTC = utcTimeToTimeT(endTime);
    return _getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTimeUTC,endTimeUTC,requestFlags,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    time_t startTimeUTC = utcTimeToTimeT(startTime);
    time_t endTimeUTC = utcTimeToTimeT(endTime);
    return _getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTimeUTC,endTimeUTC,requestFlags,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    time_t startTimeUTC = utcTimeToTimeT(startTime);
    time_t endTimeUTC = utcTimeToTimeT(endTime);
    return _getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTimeUTC,endTimeUTC,requestFlags,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,T::ContentInfoList& contentInfoList)
{
  try
  {
    time_t forecastTimeUTC = utcTimeToTimeT(forecastTime);
    return _getContentListByParameterGenerationIdAndForecastTime(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTimeUTC,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<T::GeometryId>& geometryIdList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentLevelListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentParamKeyListByGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentParamKeyListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentParamKeyListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::set<std::string>& contentTimeList)
{
  return 0; // throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList)
{
  return 0; // throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentTimeListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<std::string>& contentTimeList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_getContentTimeListByProducerId(T::SessionId sessionId,uint producerId,std::set<std::string>& contentTimeList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentTimeRangeByProducerAndGenerationId(T::SessionId sessionId,uint producerId,uint generationId,time_t& startTime,time_t& endTime)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentTimeRangeByGenerationId(T::SessionId sessionId,uint generationId,time_t& startTime,time_t& endTime)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getContentCount(T::SessionId sessionId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getHashByProducerId(T::SessionId sessionId,uint producerId,ulonglong& hash)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_updateCachedFiles(T::SessionId sessionId,std::set<uint>& fileIdList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




}
}
