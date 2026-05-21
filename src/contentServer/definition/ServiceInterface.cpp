#include "ServiceInterface.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/ShowFunction.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF



namespace SmartMet
{
namespace ContentServer
{


/*! \brief Constructor: initializes log pointers to null, marks implementation type as Interface and enables the service. */

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





/*! \brief Destructor: virtual, catches exceptions and logs them rather than propagating. */

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





/*! \brief Enables or disables the service; disabled services return SERVICE_DISABLED from API calls. */

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




/*! \brief Returns true if the service is currently enabled. */

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





/*! \brief Returns true when the backend is ready to serve requests; base implementation always returns true. */

bool ServiceInterface::isReady()
{
  FUNCTION_TRACE
  try
  {
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Fills in cache statistics for monitoring; base implementation is a no-op. */

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





/*! \brief Appends service state attributes (log file names and enabled flags) to the given attribute tree. */

void ServiceInterface::getStateAttributes(std::shared_ptr<T::AttributeNode> parent)
{
  FUNCTION_TRACE
  try
  {
    if (mProcessingLog || mDebugLog)
    {
      auto cLogs = parent->addAttribute("Logs");
      if (mProcessingLog)
      {
        if (mProcessingLog->isEnabled())
          cLogs->addAttribute("Processing Log",mProcessingLog->getFileName());
        else
          cLogs->addAttribute("Processing Log","Disabled");
      }

      if (mDebugLog)
      {
        if (mDebugLog->isEnabled())
          cLogs->addAttribute("Debug Log",mDebugLog->getFileName());
        else
          cLogs->addAttribute("Debug Log","Disabled");
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Returns the timestamp of the last content change observed by the service; base implementation returns 0. */

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





/*! \brief Returns the implementation type identifier (Interface, Redis, Cache, CorbaClient, HttpClient, Memory, Postgres). */

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





/*! \brief Returns the pointer to the debug log instance, or nullptr if unset. */

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




/*! \brief Returns the pointer to the processing (request/timing) log instance, or nullptr if unset. */

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





/*! \brief Installs an external debug Log instance used by backends for verbose diagnostics. */

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





/*! \brief Installs an external processing Log instance used to record per-call results and timings. */

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




/*! \brief Initiates an orderly shutdown of the service; base implementation is a no-op for backends to override. */

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





/*! \brief Public API: clears all backend state; logs and delegates to the protected hook. */

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





/*! \brief Public API: reloads backend data from its underlying source; logs and delegates to the protected hook. */

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





/*! \brief Public API: returns the latest content-change timestamp via the protected hook. */

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






/*! \brief Public API: registers a new producer; logs and delegates to the protected hook. */

int ServiceInterface::addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addProducerInfo(sessionId,producerInfo);


    UInt64 timeStart = getTime();
    int result = _addProducerInfo(sessionId,producerInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: updates an existing producer record; logs and delegates to the protected hook. */

int ServiceInterface::setProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _setProducerInfo(sessionId,producerInfo);

    UInt64 timeStart = getTime();
    int result = _setProducerInfo(sessionId,producerInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes a producer by ID; logs and delegates to the protected hook. */

int ServiceInterface::deleteProducerInfoById(T::SessionId sessionId,T::ProducerId producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteProducerInfoById(sessionId,producerId);

    UInt64 timeStart = getTime();
    int result = _deleteProducerInfoById(sessionId,producerId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes a producer by name; logs and delegates to the protected hook. */

int ServiceInterface::deleteProducerInfoByName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteProducerInfoByName(sessionId,producerName);

    UInt64 timeStart = getTime();
    int result = _deleteProducerInfoByName(sessionId,producerName);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all producers belonging to the given source; logs and delegates to the protected hook. */

int ServiceInterface::deleteProducerInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteProducerInfoListBySourceId(sessionId,sourceId);

    UInt64 timeStart = getTime();
    int result = _deleteProducerInfoListBySourceId(sessionId,sourceId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns producer info by ID; logs and delegates to the protected hook. */

int ServiceInterface::getProducerInfoById(T::SessionId sessionId,T::ProducerId producerId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerInfoById(sessionId,producerId,producerInfo);

    UInt64 timeStart = getTime();
    int result = _getProducerInfoById(sessionId,producerId,producerInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns producer info by name; logs and delegates to the protected hook. */

int ServiceInterface::getProducerInfoByName(T::SessionId sessionId,const std::string& producerName,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerInfoByName(sessionId,producerName,producerInfo);

    UInt64 timeStart = getTime();
    int result = _getProducerInfoByName(sessionId,producerName,producerInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the full list of producers; logs and delegates to the protected hook. */

int ServiceInterface::getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerInfoList(sessionId,producerInfoList);

    UInt64 timeStart = getTime();
    int result = _getProducerInfoList(sessionId,producerInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,ProducerInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: lists producers that publish the given parameter key; logs and delegates to the protected hook. */

int ServiceInterface::getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerInfoListByParameter(sessionId,parameterKeyType,parameterKey,producerInfoList);

    UInt64 timeStart = getTime();
    int result = _getProducerInfoListByParameter(sessionId,parameterKeyType,parameterKey,producerInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,%s,ProducerInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,parameterKeyType,parameterKey.c_str(),producerInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: lists producers belonging to the given source; logs and delegates to the protected hook. */

int ServiceInterface::getProducerInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerInfoListBySourceId(sessionId,sourceId,producerInfoList);

    UInt64 timeStart = getTime();
    int result = _getProducerInfoListBySourceId(sessionId,sourceId,producerInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,ProducerInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,producerInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the number of producers; logs and delegates to the protected hook. */

int ServiceInterface::getProducerInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerInfoCount(sessionId,count);

    UInt64 timeStart = getTime();
    int result = _getProducerInfoCount(sessionId,count);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns a set of producer-name/geometry-id pairs; logs and delegates to the protected hook. */

int ServiceInterface::getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerNameAndGeometryList(sessionId,list);

    UInt64 timeStart = getTime();
    int result = _getProducerNameAndGeometryList(sessionId,list);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,string[%lu]);result %d;time %f;",__FUNCTION__,sessionId,list.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: lists parameters known across all producers, translated between key types; logs and delegates to the protected hook. */

int ServiceInterface::getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerParameterList(sessionId,sourceParameterKeyType,targetParameterKeyType,list);

    UInt64 timeStart = getTime();
    int result = _getProducerParameterList(sessionId,sourceParameterKeyType,targetParameterKeyType,list);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%d,%d,string[%lu]);result %d;time %f;",__FUNCTION__,sessionId,sourceParameterKeyType,targetParameterKeyType,list.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: lists parameters published by a single producer, translated between key types; logs and delegates to the protected hook. */

int ServiceInterface::getProducerParameterListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getProducerParameterListByProducerId(sessionId,producerId,sourceParameterKeyType,targetParameterKeyType,list);

    UInt64 timeStart = getTime();
    int result = _getProducerParameterListByProducerId(sessionId,producerId,sourceParameterKeyType,targetParameterKeyType,list);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,%d,%d,string[%lu]);result %d;time %f;",__FUNCTION__,sessionId,producerId,sourceParameterKeyType,targetParameterKeyType,list.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: registers a new generation; logs and delegates to the protected hook. */

int ServiceInterface::addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addGenerationInfo(sessionId,generationInfo);

    UInt64 timeStart = getTime();
    int result = _addGenerationInfo(sessionId,generationInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: updates an existing generation; logs and delegates to the protected hook. */

int ServiceInterface::setGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _setGenerationInfo(sessionId,generationInfo);

    UInt64 timeStart = getTime();
    int result = _setGenerationInfo(sessionId,generationInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes a generation by ID; logs and delegates to the protected hook. */

int ServiceInterface::deleteGenerationInfoById(T::SessionId sessionId,T::GenerationId generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGenerationInfoById(sessionId,generationId);

    UInt64 timeStart = getTime();
    int result = _deleteGenerationInfoById(sessionId,generationId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes a generation by name; logs and delegates to the protected hook. */

int ServiceInterface::deleteGenerationInfoByName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGenerationInfoByName(sessionId,generationName);

    UInt64 timeStart = getTime();
    int result = _deleteGenerationInfoByName(sessionId,generationName);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes generations matching the given ID set; logs and delegates to the protected hook. */

int ServiceInterface::deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<T::GenerationId>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGenerationInfoListByIdList(sessionId,generationIdList);

    UInt64 timeStart = getTime();
    int result = _deleteGenerationInfoListByIdList(sessionId,generationIdList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,generationId[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationIdList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all generations of a producer (by ID); logs and delegates to the protected hook. */

int ServiceInterface::deleteGenerationInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGenerationInfoListByProducerId(sessionId,producerId);

    UInt64 timeStart = getTime();
    int result = _deleteGenerationInfoListByProducerId(sessionId,producerId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all generations of a producer (by name); logs and delegates to the protected hook. */

int ServiceInterface::deleteGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGenerationInfoListByProducerName(sessionId,producerName);

    UInt64 timeStart = getTime();
    int result = _deleteGenerationInfoListByProducerName(sessionId,producerName);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all generations belonging to the given source; logs and delegates to the protected hook. */

int ServiceInterface::deleteGenerationInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGenerationInfoListBySourceId(sessionId,sourceId);

    UInt64 timeStart = getTime();
    int result = _deleteGenerationInfoListBySourceId(sessionId,sourceId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: lists generations that contain the given geometry; logs and delegates to the protected hook. */

int ServiceInterface::getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationInfoListByGeometryId(sessionId,geometryId,generationInfoList);;

    UInt64 timeStart = getTime();
    int result = _getGenerationInfoListByGeometryId(sessionId,geometryId,generationInfoList);;
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,generationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,geometryId,generationInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns generation info by ID; logs and delegates to the protected hook. */

int ServiceInterface::getGenerationInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationInfoById(sessionId,generationId,generationInfo);

    UInt64 timeStart = getTime();
    int result = _getGenerationInfoById(sessionId,generationId,generationInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns generation info by name; logs and delegates to the protected hook. */

int ServiceInterface::getGenerationInfoByName(T::SessionId sessionId,const std::string& generationName,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationInfoByName(sessionId,generationName,generationInfo);

    UInt64 timeStart = getTime();
    int result = _getGenerationInfoByName(sessionId,generationName,generationInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the full list of generations; logs and delegates to the protected hook. */

int ServiceInterface::getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationInfoList(sessionId,generationInfoList);

    UInt64 timeStart = getTime();
    int result = _getGenerationInfoList(sessionId,generationInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,GenerationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: lists generations belonging to a producer (by ID); logs and delegates to the protected hook. */

int ServiceInterface::getGenerationInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationInfoListByProducerId(sessionId,producerId,generationInfoList);

    UInt64 timeStart = getTime();
    int result = _getGenerationInfoListByProducerId(sessionId,producerId,generationInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,GenerationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,generationInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: lists generations belonging to a producer (by name); logs and delegates to the protected hook. */

int ServiceInterface::getGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationInfoListByProducerName(sessionId,producerName,generationInfoList);

    UInt64 timeStart = getTime();
    int result = _getGenerationInfoListByProducerName(sessionId,producerName,generationInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s,GenerationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),generationInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: lists generations belonging to the given source; logs and delegates to the protected hook. */

int ServiceInterface::getGenerationInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationInfoListBySourceId(sessionId,sourceId,generationInfoList);

    UInt64 timeStart = getTime();
    int result = _getGenerationInfoListBySourceId(sessionId,sourceId,generationInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,GenerationInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,generationInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the latest generation of a producer (by ID) with the given status; logs and delegates to the protected hook. */

int ServiceInterface::getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,T::ProducerId producerId,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getLastGenerationInfoByProducerIdAndStatus(sessionId,producerId,generationStatus,generationInfo);

    UInt64 timeStart = getTime();
    int result = _getLastGenerationInfoByProducerIdAndStatus(sessionId,producerId,generationStatus,generationInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,generationStatus,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the latest generation of a producer (by name) with the given status; logs and delegates to the protected hook. */

int ServiceInterface::getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,const std::string& producerName,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getLastGenerationInfoByProducerNameAndStatus(sessionId,producerName,generationStatus,generationInfo);

    UInt64 timeStart = getTime();
    int result = _getLastGenerationInfoByProducerNameAndStatus(sessionId,producerName,generationStatus,generationInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s,%u);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),generationStatus,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the number of generations; logs and delegates to the protected hook. */

int ServiceInterface::getGenerationInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationInfoCount(sessionId,count);

    UInt64 timeStart = getTime();
    int result = _getGenerationInfoCount(sessionId,count);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: sets the status of a generation identified by ID; logs and delegates to the protected hook. */

int ServiceInterface::setGenerationInfoStatusById(T::SessionId sessionId,T::GenerationId generationId,uchar status)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _setGenerationInfoStatusById(sessionId,generationId,status);

    UInt64 timeStart = getTime();
    int result = _setGenerationInfoStatusById(sessionId,generationId,status);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,status,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: sets the status of a generation identified by name; logs and delegates to the protected hook. */

int ServiceInterface::setGenerationInfoStatusByName(T::SessionId sessionId,const std::string& generationName,uchar status)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _setGenerationInfoStatusByName(sessionId,generationName,status);

    UInt64 timeStart = getTime();
    int result = _setGenerationInfoStatusByName(sessionId,generationName,status);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s,%u);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),status,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: registers a new geometry record; logs and delegates to the protected hook. */

int ServiceInterface::addGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addGeometryInfo(sessionId,geometryInfo);

    UInt64 timeStart = getTime();
    int result = _addGeometryInfo(sessionId,geometryInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,geometryInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes a geometry record identified by generation/geometry/level; logs and delegates to the protected hook. */

int ServiceInterface::deleteGeometryInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGeometryInfoById(sessionId,generationId,geometryId,levelId);

    UInt64 timeStart = getTime();
    int result = _deleteGeometryInfoById(sessionId,generationId,geometryId,levelId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%d,%d);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,levelId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all geometry records belonging to a generation; logs and delegates to the protected hook. */

int ServiceInterface::deleteGeometryInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGeometryInfoListByGenerationId(sessionId,generationId);

    UInt64 timeStart = getTime();
    int result = _deleteGeometryInfoListByGenerationId(sessionId,generationId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all geometry records of a producer; logs and delegates to the protected hook. */

int ServiceInterface::deleteGeometryInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGeometryInfoListByProducerId(sessionId,producerId);

    UInt64 timeStart = getTime();
    int result = _deleteGeometryInfoListByProducerId(sessionId,producerId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all geometry records belonging to a source; logs and delegates to the protected hook. */

int ServiceInterface::deleteGeometryInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteGeometryInfoListBySourceId(sessionId,sourceId);

    UInt64 timeStart = getTime();
    int result = _deleteGeometryInfoListBySourceId(sessionId,sourceId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the geometry record identified by generation/geometry/level; logs and delegates to the protected hook. */

int ServiceInterface::getGeometryInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGeometryInfoById(sessionId,generationId,geometryId,levelId,geometryInfo);

    UInt64 timeStart = getTime();
    int result = _getGeometryInfoById(sessionId,generationId,geometryId,levelId,geometryInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%d,%d);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,levelId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the full list of geometry records; logs and delegates to the protected hook. */

int ServiceInterface::getGeometryInfoList(T::SessionId sessionId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGeometryInfoList(sessionId,geometryInfoList);

    UInt64 timeStart = getTime();
    int result = _getGeometryInfoList(sessionId,geometryInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,geometryInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,geometryInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Public API: lists geometry records belonging to a generation; logs and delegates to the protected hook. */

int ServiceInterface::getGeometryInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGeometryInfoListByGenerationId(sessionId,generationId,geometryInfoList);

    UInt64 timeStart = getTime();
    int result = _getGeometryInfoListByGenerationId(sessionId,generationId,geometryInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,geometryInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Public API: lists geometry records belonging to a producer; logs and delegates to the protected hook. */

int ServiceInterface::getGeometryInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGeometryInfoListByProducerId(sessionId,producerId,geometryInfoList);

    UInt64 timeStart = getTime();
    int result = _getGeometryInfoListByProducerId(sessionId,producerId,geometryInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,geometryInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,geometryInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Public API: lists geometry records belonging to a source; logs and delegates to the protected hook. */

int ServiceInterface::getGeometryInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGeometryInfoListBySourceId(sessionId,sourceId,geometryInfoList);

    UInt64 timeStart = getTime();
    int result = _getGeometryInfoListBySourceId(sessionId,sourceId,geometryInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,geometryInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,geometryInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the number of geometry records; logs and delegates to the protected hook. */

int ServiceInterface::getGeometryInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGeometryInfoCount(sessionId,count);

    UInt64 timeStart = getTime();
    int result = _getGeometryInfoCount(sessionId,count);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: updates an existing geometry record; logs and delegates to the protected hook. */

int ServiceInterface::setGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _setGeometryInfo(sessionId,geometryInfo);

    UInt64 timeStart = getTime();
    int result = _setGeometryInfo(sessionId,geometryInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,geometryInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Public API: sets the status of a geometry record identified by generation/geometry/level; logs and delegates to the protected hook. */

int ServiceInterface::setGeometryInfoStatusById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,uchar status)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _setGeometryInfoStatusById(sessionId,generationId,geometryId,levelId,status);

    UInt64 timeStart = getTime();
    int result = _setGeometryInfoStatusById(sessionId,generationId,geometryId,levelId,status);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%d,%d,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,levelId,status,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: registers a new file record; logs and delegates to the protected hook. */

int ServiceInterface::addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addFileInfo(sessionId,fileInfo);

    UInt64 timeStart = getTime();
    int result = _addFileInfo(sessionId,fileInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,fileInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: updates an existing file record; logs and delegates to the protected hook. */

int ServiceInterface::setFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _setFileInfo(sessionId,fileInfo);

    UInt64 timeStart = getTime();
    int result = _setFileInfo(sessionId,fileInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,fileInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: registers a file along with all its content records in one call; logs and delegates to the protected hook. */

int ServiceInterface::addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addFileInfoWithContentList(sessionId,fileInfo,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _addFileInfoWithContentList(sessionId,fileInfo,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileInfo.getCsv().c_str(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: batch-registers multiple files together with their content records; logs and delegates to the protected hook. */

int ServiceInterface::addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addFileInfoListWithContent(sessionId,requestFlags,fileAndContentList);

    UInt64 timeStart = getTime();
    int result = _addFileInfoListWithContent(sessionId,requestFlags,fileAndContentList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,FileAndContent[%lu]);result %d;time %f;",__FUNCTION__,sessionId,requestFlags,fileAndContentList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes a file (and its contents) by file ID; logs and delegates to the protected hook. */

int ServiceInterface::deleteFileInfoById(T::SessionId sessionId,T::FileId fileId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoById(sessionId,fileId);

    UInt64 timeStart = getTime();
    int result = _deleteFileInfoById(sessionId,fileId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu);result %d;time %f;",__FUNCTION__,sessionId,fileId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes a file (and its contents) by filename; logs and delegates to the protected hook. */

int ServiceInterface::deleteFileInfoByName(T::SessionId sessionId,const std::string& filename)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoByName(sessionId,filename);

    UInt64 timeStart = getTime();
    int result = _deleteFileInfoByName(sessionId,filename);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,filename.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all files of a producer (by ID); logs and delegates to the protected hook. */

int ServiceInterface::deleteFileInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListByProducerId(sessionId,producerId);

    UInt64 timeStart = getTime();
    int result = _deleteFileInfoListByProducerId(sessionId,producerId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all files of a producer (by name); logs and delegates to the protected hook. */

int ServiceInterface::deleteFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListByProducerName(sessionId,producerName);

    UInt64 timeStart = getTime();
    int result = _deleteFileInfoListByProducerName(sessionId,producerName);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all files of a generation; logs and delegates to the protected hook. */

int ServiceInterface::deleteFileInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListByGenerationId(sessionId,generationId);

    UInt64 timeStart = getTime();
    int result = _deleteFileInfoListByGenerationId(sessionId,generationId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes files matching generation, geometry, forecast type/number and the given forecast time (string form); logs and delegates to the protected hook. */

int ServiceInterface::deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,const std::string& forecastTime)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListByGenerationIdAndForecastTime(sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime);

    UInt64 timeStart = getTime();
    int result = _deleteFileInfoListByGenerationIdAndForecastTime(sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u,%d,%d,%s);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes files matching generation, geometry, forecast type/number and the given forecast time (epoch form); logs and delegates to the protected hook. */

int ServiceInterface::deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,time_t forecastTime)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListByGenerationIdAndForecastTime(sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime);

    UInt64 timeStart = getTime();
    int result = _deleteFileInfoListByGenerationIdAndForecastTime(sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u,%d,%d,%lu);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes files matching a list of forecast-time keys; logs and delegates to the protected hook. */

int ServiceInterface::deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListByForecastTimeList(sessionId,forecastTimeList);

    UInt64 timeStart = getTime();
    int result = _deleteFileInfoListByForecastTimeList(sessionId,forecastTimeList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,ForecastTime[%lu]);result %d;time %f;",__FUNCTION__,sessionId,forecastTimeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all files of a generation identified by name; logs and delegates to the protected hook. */

int ServiceInterface::deleteFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListByGenerationName(sessionId,generationName);

    UInt64 timeStart = getTime();
    int result = _deleteFileInfoListByGenerationName(sessionId,generationName);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all files belonging to the given source; logs and delegates to the protected hook. */

int ServiceInterface::deleteFileInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListBySourceId(sessionId,sourceId);

    UInt64 timeStart = getTime();
    int result = _deleteFileInfoListBySourceId(sessionId,sourceId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes files matching the given ID set; logs and delegates to the protected hook. */

int ServiceInterface::deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<T::FileId>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteFileInfoListByFileIdList(sessionId,fileIdList);

    UInt64 timeStart = getTime();
    int result = _deleteFileInfoListByFileIdList(sessionId,fileIdList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,fileId[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileIdList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns file info by ID; logs and delegates to the protected hook. */

int ServiceInterface::getFileInfoById(T::SessionId sessionId,T::FileId fileId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoById(sessionId,fileId,fileInfo);

    UInt64 timeStart = getTime();
    int result = _getFileInfoById(sessionId,fileId,fileInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu);result %d;time %f;",__FUNCTION__,sessionId,fileId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns file info by filename; logs and delegates to the protected hook. */

int ServiceInterface::getFileInfoByName(T::SessionId sessionId,const std::string& filename,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoByName(sessionId,filename,fileInfo);

    UInt64 timeStart = getTime();
    int result = _getFileInfoByName(sessionId,filename,fileInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,filename.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns a paginated list of files starting from startFileId; logs and delegates to the protected hook. */

int ServiceInterface::getFileInfoList(T::SessionId sessionId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoList(sessionId,startFileId,maxRecords,fileInfoList);

    UInt64 timeStart = getTime();
    int result = _getFileInfoList(sessionId,startFileId,maxRecords,fileInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns file info entries matching the given ID list; logs and delegates to the protected hook. */

int ServiceInterface::getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<T::FileId>& fileIdList,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoListByFileIdList(sessionId,fileIdList,fileInfoList);

    UInt64 timeStart = getTime();
    int result = _getFileInfoListByFileIdList(sessionId,fileIdList,fileInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,fileId[%lu],FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileIdList.size(),fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: paginated list of files for a producer (by ID); logs and delegates to the protected hook. */

int ServiceInterface::getFileInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoListByProducerId(sessionId,producerId,startFileId,maxRecords,fileInfoList);

    UInt64 timeStart = getTime();
    int result = _getFileInfoListByProducerId(sessionId,producerId,startFileId,maxRecords,fileInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,%lu,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: paginated list of files for a producer (by name); logs and delegates to the protected hook. */

int ServiceInterface::getFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoListByProducerName(sessionId,producerName,startFileId,maxRecords,fileInfoList);

    UInt64 timeStart = getTime();
    int result = _getFileInfoListByProducerName(sessionId,producerName,startFileId,maxRecords,fileInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s,%lu,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: paginated list of files for a generation (by ID); logs and delegates to the protected hook. */

int ServiceInterface::getFileInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoListByGenerationId(sessionId,generationId,startFileId,maxRecords,fileInfoList);

    UInt64 timeStart = getTime();
    int result = _getFileInfoListByGenerationId(sessionId,generationId,startFileId,maxRecords,fileInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%lu,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: paginated list of files for a generation (by name); logs and delegates to the protected hook. */

int ServiceInterface::getFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoListByGenerationName(sessionId,generationName,startFileId,maxRecords,fileInfoList);

    UInt64 timeStart = getTime();
    int result = _getFileInfoListByGenerationName(sessionId,generationName,startFileId,maxRecords,fileInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s,%lu,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: paginated list of files for a source; logs and delegates to the protected hook. */

int ServiceInterface::getFileInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoListBySourceId(sessionId,sourceId,startFileId,maxRecords,fileInfoList);

    UInt64 timeStart = getTime();
    int result = _getFileInfoListBySourceId(sessionId,sourceId,startFileId,maxRecords,fileInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,%lu,%u,FileInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,startFileId,maxRecords,fileInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the total number of files; logs and delegates to the protected hook. */

int ServiceInterface::getFileInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoCount(sessionId,count);

    UInt64 timeStart = getTime();
    int result = _getFileInfoCount(sessionId,count);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the number of files belonging to a producer; logs and delegates to the protected hook. */

int ServiceInterface::getFileInfoCountByProducerId(T::SessionId sessionId,T::ProducerId producerId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoCountByProducerId(sessionId,producerId,count);

    UInt64 timeStart = getTime();
    int result = _getFileInfoCountByProducerId(sessionId,producerId,count);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the number of files belonging to a generation; logs and delegates to the protected hook. */

int ServiceInterface::getFileInfoCountByGenerationId(T::SessionId sessionId,T::GenerationId generationId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoCountByGenerationId(sessionId,generationId,count);

    UInt64 timeStart = getTime();
    int result = _getFileInfoCountByGenerationId(sessionId,generationId,count);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,generationId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the number of files belonging to a source; logs and delegates to the protected hook. */

int ServiceInterface::getFileInfoCountBySourceId(T::SessionId sessionId,T::SourceId sourceId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getFileInfoCountBySourceId(sessionId,sourceId,count);

    UInt64 timeStart = getTime();
    int result = _getFileInfoCountBySourceId(sessionId,sourceId,count);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: records a server event into the event log; logs and delegates to the protected hook. */

int ServiceInterface::addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addEventInfo(sessionId,eventInfo);

    UInt64 timeStart = getTime();
    int result = _addEventInfo(sessionId,eventInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,eventInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the most recent event observed by the service; logs and delegates to the protected hook. */

int ServiceInterface::getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    // ### This method is polled all the time. Let's not write these calls to the processing log.

    //UInt64 timeStart = getTime();
    int result = _getLastEventInfo(sessionId,requestingServerId,eventInfo);
    //UInt64 requestTime = getTime() - timeStart;

    //PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,requestingServerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns a paginated batch of events used by clients to follow change notifications; logs and delegates to the protected hook. */

int ServiceInterface::getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getEventInfoList(sessionId,requestingServerId,startEventId,maxRecords,eventInfoList);

    UInt64 timeStart = getTime();
    int result = _getEventInfoList(sessionId,requestingServerId,startEventId,maxRecords,eventInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,%lu,%u,EventInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,requestingServerId,startEventId,maxRecords,eventInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






/*! \brief Public API: returns the total number of events known to the service; logs and delegates to the protected hook. */

int ServiceInterface::getEventInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getEventInfoCount(sessionId,count);

    UInt64 timeStart = getTime();
    int result = _getEventInfoCount(sessionId,count);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: registers a new content (grib message) record; logs and delegates to the protected hook. */

int ServiceInterface::addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addContentInfo(sessionId,contentInfo);

    UInt64 timeStart = getTime();
    int result = _addContentInfo(sessionId,contentInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,contentInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: updates an existing content record; logs and delegates to the protected hook. */

int ServiceInterface::setContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _setContentInfo(sessionId,contentInfo);

    UInt64 timeStart = getTime();
    int result = _setContentInfo(sessionId,contentInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,contentInfo.getCsv().c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: batch-registers a list of content records; logs and delegates to the protected hook. */

int ServiceInterface::addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _addContentList(sessionId,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _addContentList(sessionId,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes a single content record identified by file ID and message index; logs and delegates to the protected hook. */

int ServiceInterface::deleteContentInfo(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteContentInfo(sessionId,fileId,messageIndex);

    UInt64 timeStart = getTime();
    int result = _deleteContentInfo(sessionId,fileId,messageIndex);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all content records belonging to a file (by ID); logs and delegates to the protected hook. */

int ServiceInterface::deleteContentListByFileId(T::SessionId sessionId,T::FileId fileId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteContentListByFileId(sessionId,fileId);

    UInt64 timeStart = getTime();
    int result = _deleteContentListByFileId(sessionId,fileId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu);result %d;time %f;",__FUNCTION__,sessionId,fileId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all content records belonging to a file (by name); logs and delegates to the protected hook. */

int ServiceInterface::deleteContentListByFileName(T::SessionId sessionId,const std::string& filename)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteContentListByFileName(sessionId,filename);

    UInt64 timeStart = getTime();
    int result = _deleteContentListByFileName(sessionId,filename);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,filename.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all content records belonging to a producer (by ID); logs and delegates to the protected hook. */

int ServiceInterface::deleteContentListByProducerId(T::SessionId sessionId,T::ProducerId producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteContentListByProducerId(sessionId,producerId);

    UInt64 timeStart = getTime();
    int result = _deleteContentListByProducerId(sessionId,producerId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,producerId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all content records belonging to a producer (by name); logs and delegates to the protected hook. */

int ServiceInterface::deleteContentListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteContentListByProducerName(sessionId,producerName);

    UInt64 timeStart = getTime();
    int result = _deleteContentListByProducerName(sessionId,producerName);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all content records belonging to a generation (by ID); logs and delegates to the protected hook. */

int ServiceInterface::deleteContentListByGenerationId(T::SessionId sessionId,T::GenerationId generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteContentListByGenerationId(sessionId,generationId);

    UInt64 timeStart = getTime();
    int result = _deleteContentListByGenerationId(sessionId,generationId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu);result %d;time %f;",__FUNCTION__,sessionId,generationId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all content records belonging to a generation (by name); logs and delegates to the protected hook. */

int ServiceInterface::deleteContentListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteContentListByGenerationName(sessionId,generationName);

    UInt64 timeStart = getTime();
    int result = _deleteContentListByGenerationName(sessionId,generationName);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: deletes all content records belonging to a source; logs and delegates to the protected hook. */

int ServiceInterface::deleteContentListBySourceId(T::SessionId sessionId,T::SourceId sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _deleteContentListBySourceId(sessionId,sourceId);

    UInt64 timeStart = getTime();
    int result = _deleteContentListBySourceId(sessionId,sourceId);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,sourceId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns a single content record identified by file ID and message index; logs and delegates to the protected hook. */

int ServiceInterface::getContentInfo(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentInfo(sessionId,fileId,messageIndex,contentInfo);

    UInt64 timeStart = getTime();
    int result = _getContentInfo(sessionId,fileId,messageIndex,contentInfo);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns a paginated batch of content records starting from (file,message); logs and delegates to the protected hook. */

int ServiceInterface::getContentList(T::SessionId sessionId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentList(sessionId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentList(sessionId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns all content records of a single file (by ID); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByFileId(T::SessionId sessionId,T::FileId fileId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByFileId(sessionId,fileId,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByFileId(sessionId,fileId,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns content records for files matching the given ID list; logs and delegates to the protected hook. */

int ServiceInterface::getContentListByFileIdList(T::SessionId sessionId,std::vector<T::FileId>& fileIdList,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByFileIdList(sessionId,fileIdList,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByFileIdList(sessionId,fileIdList,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,fileId[%lu],ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileIdList.size(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns all content records of a single file (by name); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByFileName(T::SessionId sessionId,const std::string& filename,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByFileName(sessionId,filename,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByFileName(sessionId,filename,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,filename.c_str(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: paginated content listing for a producer (by ID); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByProducerId(sessionId,producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByProducerId(sessionId,producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,%lu,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: paginated content listing for a producer (by name); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByProducerName(T::SessionId sessionId,const std::string& producerName,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByProducerName(sessionId,producerName,startFileId,startMessageIndex,maxRecords,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByProducerName(sessionId,producerName,startFileId,startMessageIndex,maxRecords,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s,%lu,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: paginated content listing for a source; logs and delegates to the protected hook. */

int ServiceInterface::getContentListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListBySourceId(sessionId,sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListBySourceId(sessionId,sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,%lu,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: paginated content listing for a generation (by ID), with request flag filtering; logs and delegates to the protected hook. */

int ServiceInterface::getContentListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByGenerationId(sessionId,generationId,startFileId,startMessageIndex,maxRecords,requestFlags,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByGenerationId(sessionId,generationId,startFileId,startMessageIndex,maxRecords,requestFlags,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%lu,%u,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,startFileId,startMessageIndex,maxRecords,requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: paginated content listing for a generation (by name); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByGenerationName(T::SessionId sessionId,const std::string& generationName,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByGenerationName(sessionId,generationName,startFileId,startMessageIndex,maxRecords,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByGenerationName(sessionId,generationName,startFileId,startMessageIndex,maxRecords,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s,%lu,%u,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),startFileId,startMessageIndex,maxRecords,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: content records of a generation (by ID) within a time range (string form); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,T::GenerationId generationId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTime,endTime,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTime,endTime,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%s,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,startTime.c_str(),endTime.c_str(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: content records of a generation (by ID) within a time range (epoch form); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,T::GenerationId generationId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTime,endTime,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTime,endTime,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%lu,%lu,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,startTime,endTime,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: content records of a generation (by name) within a time range (string form); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByGenerationNameAndTimeRange(sessionId,generationName,startTime,endTime,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByGenerationNameAndTimeRange(sessionId,generationName,startTime,endTime,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s,%s,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),startTime.c_str(),endTime.c_str(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: content records of a generation (by name) within a time range (epoch form); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByGenerationNameAndTimeRange(sessionId,generationName,startTime,endTime,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByGenerationNameAndTimeRange(sessionId,generationName,startTime,endTime,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s,%lu,%lu,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),startTime,endTime,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: content records matching parameter/level/forecast/geometry filters and a time range (string form); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameter(sessionId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByParameter(sessionId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,%s,%d,%d,%d,%d,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: content records matching parameter/level/forecast/geometry filters and a time range (epoch form); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameter(sessionId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByParameter(sessionId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,%s,%d,%d,%d,%d,%d,%d,%lu,%lu,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: content records filtered by parameter and generation (by ID), within a time range (string form); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByParameterAndGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u,%s,%d,%d,%d,%d,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: content records filtered by parameter and generation (by ID), within a time range (epoch form); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByParameterAndGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u,%s,%d,%d,%d,%d,%d,%d,%lu,%lu,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: content records filtered by parameter and generation (by name), within a time range (string form); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s,%u,%s,%d,%d,%d,%d,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: content records filtered by parameter and generation (by name), within a time range (epoch form); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s,%u,%s,%d,%d,%d,%d,%d,%d,%lu,%lu,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationName.c_str(),parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: content records filtered by parameter and producer (by ID), within a time range (string form); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByParameterAndProducerId(T::SessionId sessionId,T::ProducerId producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,%u,%s,%d,%d,%d,%d,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: content records filtered by parameter and producer (by ID), within a time range (epoch form); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByParameterAndProducerId(T::SessionId sessionId,T::ProducerId producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,%u,%s,%d,%d,%d,%d,%d,%d,%lu,%lu,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerId,parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: content records filtered by parameter and producer (by name), within a time range (string form); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s,%u,%s,%d,%d,%d,%d,%d,%d,%s,%s,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime.c_str(),endTime.c_str(),requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: content records filtered by parameter and producer (by name), within a time range (epoch form); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%s,%u,%s,%d,%d,%d,%d,%d,%d,%lu,%lu,%u,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,producerName.c_str(),parameterKeyType,parameterKey.c_str(),parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: content records bracketing the given forecast time for parameter/generation (string form); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterGenerationIdAndForecastTime(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByParameterGenerationIdAndForecastTime(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u,%s,%d,%d,%d,%d,%d,%s,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,parameterKeyType,parameterKey.c_str(),parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime.c_str(),contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: content records bracketing the given forecast time for parameter/generation (epoch form); logs and delegates to the protected hook. */

int ServiceInterface::getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListByParameterGenerationIdAndForecastTime(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListByParameterGenerationIdAndForecastTime(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u,%s,%d,%d,%d,%d,%d,%lu,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,parameterKeyType,parameterKey.c_str(),parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns content records that failed integrity checks; logs and delegates to the protected hook. */

int ServiceInterface::getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentListOfInvalidIntegrity(sessionId,contentInfoList);

    UInt64 timeStart = getTime();
    int result = _getContentListOfInvalidIntegrity(sessionId,contentInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,ContentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,contentInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the distinct geometry IDs found in the content of a generation; logs and delegates to the protected hook. */

int ServiceInterface::getContentGeometryIdListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentGeometryIdListByGenerationId(sessionId,generationId,geometryIdList);

    UInt64 timeStart = getTime();
    int result = _getContentGeometryIdListByGenerationId(sessionId,generationId,geometryIdList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,geometryId[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryIdList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the distinct levels found in content matching generation/geometry/level-id; logs and delegates to the protected hook. */

int ServiceInterface::getContentLevelListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentLevelListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,contentLevelList);

    UInt64 timeStart = getTime();
    int result = _getContentLevelListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,contentLevelList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u,%u,time[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,levelId,contentLevelList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the distinct levels found in content matching parameter/generation/geometry/level-id; logs and delegates to the protected hook. */

int ServiceInterface::getContentLevelListByParameterGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,std::string parameterKey,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentLevelListByParameterGenerationGeometryAndLevelId(sessionId,generationId,geometryId,parameterKey,levelId,contentLevelList);

    UInt64 timeStart = getTime();
    int result = _getContentLevelListByParameterGenerationGeometryAndLevelId(sessionId,generationId,geometryId,parameterKey,levelId,contentLevelList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%d,%s,%u,time[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,parameterKey.c_str(),levelId,contentLevelList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Public API: returns one representative content record per distinct parameter in a generation; logs and delegates to the protected hook. */

int ServiceInterface::getContentParamListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ContentInfoList& contentParamList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentParamListByGenerationId(sessionId,generationId,contentParamList);

    UInt64 timeStart = getTime();
    int result = _getContentParamListByGenerationId(sessionId,generationId,contentParamList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,contentInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,generationId,contentParamList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the set of parameter keys (of the requested key type) used in a generation; logs and delegates to the protected hook. */

int ServiceInterface::getContentParamKeyListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentParamKeyListByGenerationId(sessionId,generationId,parameterKeyType,paramKeyList);

    UInt64 timeStart = getTime();
    int result = _getContentParamKeyListByGenerationId(sessionId,generationId,parameterKeyType,paramKeyList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u,paramKey[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,parameterKeyType,paramKeyList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the set of parameter keys for a given generation/geometry; logs and delegates to the protected hook. */

int ServiceInterface::getContentParamKeyListByGenerationAndGeometryId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentParamKeyListByGenerationAndGeometryId(sessionId,generationId,geometryId,parameterKeyType,paramKeyList);

    UInt64 timeStart = getTime();
    int result = _getContentParamKeyListByGenerationAndGeometryId(sessionId,generationId,geometryId,parameterKeyType,paramKeyList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u,paramKey[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,parameterKeyType,paramKeyList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the set of parameter keys for a given generation/geometry/level-id; logs and delegates to the protected hook. */

int ServiceInterface::getContentParamKeyListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentParamKeyListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,parameterKeyType,paramKeyList);

    UInt64 timeStart = getTime();
    int result = _getContentParamKeyListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,parameterKeyType,paramKeyList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u,%u,paramKey[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,levelId,parameterKeyType,paramKeyList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the set of forecast times available in a generation; logs and delegates to the protected hook. */

int ServiceInterface::getContentTimeListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentTimeListByGenerationId(sessionId,generationId,contentTimeList);

    UInt64 timeStart = getTime();
    int result = _getContentTimeListByGenerationId(sessionId,generationId,contentTimeList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,time[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,contentTimeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the set of forecast times available for a generation/geometry; logs and delegates to the protected hook. */

int ServiceInterface::getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentTimeListByGenerationAndGeometryId(sessionId,generationId,geometryId,contentTimeList);

    UInt64 timeStart = getTime();
    int result = _getContentTimeListByGenerationAndGeometryId(sessionId,generationId,geometryId,contentTimeList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u,time[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,contentTimeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the set of forecast times for a given generation/geometry/level-id; logs and delegates to the protected hook. */

int ServiceInterface::getContentTimeListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentTimeListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,contentTimeList);

    UInt64 timeStart = getTime();
    int result = _getContentTimeListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,contentTimeList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%u,%u,time[%lu]);result %d;time %f;",__FUNCTION__,sessionId,generationId,geometryId,levelId,contentTimeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the set of forecast times available across all generations of a producer; logs and delegates to the protected hook. */

int ServiceInterface::getContentTimeListByProducerId(T::SessionId sessionId,T::ProducerId producerId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentTimeListByProducerId(sessionId,producerId,contentTimeList);

    UInt64 timeStart = getTime();
    int result = _getContentTimeListByProducerId(sessionId,producerId,contentTimeList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,time[%lu]);result %d;time %f;",__FUNCTION__,sessionId,producerId,contentTimeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the [start,end] forecast time range for a producer+generation pair; logs and delegates to the protected hook. */

int ServiceInterface::getContentTimeRangeByProducerAndGenerationId(T::SessionId sessionId,T::ProducerId producerId,T::GenerationId generationId,time_t& startTime,time_t& endTime)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentTimeRangeByProducerAndGenerationId(sessionId,producerId,generationId,startTime,endTime);

    UInt64 timeStart = getTime();
    int result = _getContentTimeRangeByProducerAndGenerationId(sessionId,producerId,generationId,startTime,endTime);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,%lu,%lu,%lu);result %d;time %f;",__FUNCTION__,sessionId,producerId,generationId,startTime,endTime,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the [start,end] forecast time range for a generation; logs and delegates to the protected hook. */

int ServiceInterface::getContentTimeRangeByGenerationId(T::SessionId sessionId,T::GenerationId generationId,time_t& startTime,time_t& endTime)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentTimeRangeByGenerationId(sessionId,generationId,startTime,endTime);

    UInt64 timeStart = getTime();
    int result = _getContentTimeRangeByGenerationId(sessionId,generationId,startTime,endTime);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%lu,%lu,%lu);result %d;time %f;",__FUNCTION__,sessionId,generationId,startTime,endTime,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns a flat set of generationId/geometryId/forecastTime triplets; logs and delegates to the protected hook. */

int ServiceInterface::getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getGenerationIdGeometryIdAndForecastTimeList(sessionId,list);

    UInt64 timeStart = getTime();
    int result = _getGenerationIdGeometryIdAndForecastTimeList(sessionId,list);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,listItem[%lu]);result %d;time %f;",__FUNCTION__,sessionId,list.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns the total number of content records; logs and delegates to the protected hook. */

int ServiceInterface::getContentCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getContentCount(sessionId,count);

    UInt64 timeStart = getTime();
    int result = _getContentCount(sessionId,count);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns a content-hash representing the state of a producer; used for cache invalidation; logs and delegates to the protected hook. */

int ServiceInterface::getHashByProducerId(T::SessionId sessionId,T::ProducerId producerId,UInt64 & hash)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getHashByProducerId(sessionId,producerId,hash);

    UInt64 timeStart = getTime();
    int result = _getHashByProducerId(sessionId,producerId,hash);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,%u,%lu);result %d;time %f;",__FUNCTION__,sessionId,producerId,hash,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: returns metadata about all level types/levels currently in the registry; logs and delegates to the protected hook. */

int ServiceInterface::getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _getLevelInfoList(sessionId,levelInfoList);

    UInt64 timeStart = getTime();
    int result = _getLevelInfoList(sessionId,levelInfoList);
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,LevelInfo[%u]);result %d;time %f;",__FUNCTION__,sessionId,levelInfoList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Public API: refreshes cached file metadata for the given file IDs; logs and delegates to the protected hook. */

int ServiceInterface::updateCachedFiles(T::SessionId sessionId,std::set<T::FileId>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    if (mProcessingLog == nullptr || !mProcessingLog->isEnabled())
      return _updateCachedFiles(sessionId,fileIdList);

    UInt64 timeStart = getTime();
    int result = _updateCachedFiles(sessionId,fileIdList);;
    UInt64 requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%lu,fileIdList[%ld]);result %d;time %f;",__FUNCTION__,sessionId,fileIdList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Protected hook for backend implementations of clear() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_clear(T::SessionId sessionId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of reload() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_reload(T::SessionId sessionId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentChangeTime() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentChangeTime(T::SessionId sessionId,time_t& changeTime)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




/*! \brief Protected hook for backend implementations of addProducerInfo() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of setProducerInfo() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_setProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteProducerInfoById() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteProducerInfoById(T::SessionId sessionId,T::ProducerId producerId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteProducerInfoByName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteProducerInfoByName(T::SessionId sessionId,const std::string& producerName)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteProducerInfoListBySourceId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteProducerInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getProducerInfoById() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getProducerInfoById(T::SessionId sessionId,T::ProducerId producerId,T::ProducerInfo& producerInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getProducerInfoByName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getProducerInfoByName(T::SessionId sessionId,const std::string& producerName,T::ProducerInfo& producerInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getProducerInfoList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getProducerInfoListByParameter() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getProducerInfoListBySourceId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getProducerInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::ProducerInfoList& producerInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getProducerInfoCount() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getProducerInfoCount(T::SessionId sessionId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getProducerNameAndGeometryList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getProducerParameterList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getProducerParameterListByProducerId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getProducerParameterListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of addGenerationInfo() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of setGenerationInfo() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_setGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteGenerationInfoById() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteGenerationInfoById(T::SessionId sessionId,T::GenerationId generationId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteGenerationInfoByName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteGenerationInfoByName(T::SessionId sessionId,const std::string& generationName)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteGenerationInfoListByIdList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<T::GenerationId>& generationIdList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteGenerationInfoListByProducerId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteGenerationInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteGenerationInfoListByProducerName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteGenerationInfoListBySourceId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteGenerationInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getGenerationInfoById() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getGenerationInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GenerationInfo& generationInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getGenerationInfoByName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getGenerationInfoByName(T::SessionId sessionId,const std::string& generationName,T::GenerationInfo& generationInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getGenerationInfoList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getGenerationInfoListByGeometryId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getGenerationInfoListByProducerId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getGenerationInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::GenerationInfoList& generationInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getGenerationInfoListByProducerName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::GenerationInfoList& generationInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getGenerationInfoListBySourceId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getGenerationInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::GenerationInfoList& generationInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getLastGenerationInfoByProducerIdAndStatus() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,T::ProducerId producerId,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getLastGenerationInfoByProducerNameAndStatus() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,const std::string& producerName,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getGenerationInfoCount() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getGenerationInfoCount(T::SessionId sessionId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of setGenerationInfoStatusById() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_setGenerationInfoStatusById(T::SessionId sessionId,T::GenerationId generationId,uchar status)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of setGenerationInfoStatusByName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_setGenerationInfoStatusByName(T::SessionId sessionId,const std::string& generationName,uchar status)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of addGeometryInfo() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_addGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteGeometryInfoById() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteGeometryInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteGeometryInfoListByGenerationId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteGeometryInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteGeometryInfoListByProducerId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteGeometryInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteGeometryInfoListBySourceId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteGeometryInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getGeometryInfoById() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getGeometryInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getGeometryInfoList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getGeometryInfoList(T::SessionId sessionId,T::GeometryInfoList& geometryInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getGeometryInfoListByGenerationId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getGeometryInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryInfoList& geometryInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getGeometryInfoListByProducerId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getGeometryInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::GeometryInfoList& geometryInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getGeometryInfoListBySourceId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getGeometryInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::GeometryInfoList& geometryInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getGeometryInfoCount() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getGeometryInfoCount(T::SessionId sessionId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of setGeometryInfo() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_setGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




/*! \brief Protected hook for backend implementations of setGeometryInfoStatusById() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_setGeometryInfoStatusById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,uchar status)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of addFileInfo() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of setFileInfo() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_setFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of addFileInfoWithContentList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of addFileInfoListWithContent() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteFileInfoById() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteFileInfoById(T::SessionId sessionId,T::FileId fileId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteFileInfoByName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteFileInfoByName(T::SessionId sessionId,const std::string& filename)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteFileInfoListByProducerId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteFileInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteFileInfoListByProducerName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteFileInfoListByGenerationId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteFileInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteFileInfoListByGenerationIdAndForecastTime() (string form) — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,const std::string& forecastTime)
try
{
  time_t forecastTimeUTC = utcTimeToTimeT(forecastTime);
  return _deleteFileInfoListByGenerationIdAndForecastTime(sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTimeUTC);
}
catch (...)
{
  throw Fmi::Exception(BCP,"Operation failed!",nullptr);
}





/*! \brief Protected hook for backend implementations of deleteFileInfoListByGenerationIdAndForecastTime() (epoch form) — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,time_t forecastTime)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteFileInfoListByForecastTimeList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteFileInfoListByGenerationName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteFileInfoListBySourceId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteFileInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteFileInfoListByFileIdList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<T::FileId>& fileIdList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getFileInfoById() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getFileInfoById(T::SessionId sessionId,T::FileId fileId,T::FileInfo& fileInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getFileInfoByName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getFileInfoByName(T::SessionId sessionId,const std::string& filename,T::FileInfo& fileInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getFileInfoList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getFileInfoList(T::SessionId sessionId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getFileInfoListByFileIdList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<T::FileId>& fileIdList,T::FileInfoList& fileInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getFileInfoListByProducerId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getFileInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getFileInfoListByProducerName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getFileInfoListByGenerationId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getFileInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getFileInfoListByGenerationName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getFileInfoListBySourceId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getFileInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getFileInfoCount() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getFileInfoCount(T::SessionId sessionId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getFileInfoCountByProducerId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getFileInfoCountByProducerId(T::SessionId sessionId,T::ProducerId producerId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getFileInfoCountByGenerationId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getFileInfoCountByGenerationId(T::SessionId sessionId,T::GenerationId generationId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getFileInfoCountBySourceId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getFileInfoCountBySourceId(T::SessionId sessionId,T::SourceId sourceId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of addEventInfo() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getLastEventInfo() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getEventInfoList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getEventInfoCount() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getEventInfoCount(T::SessionId sessionId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of addContentInfo() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of setContentInfo() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_setContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of addContentList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteContentInfo() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteContentInfo(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteContentListByFileId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteContentListByFileId(T::SessionId sessionId,T::FileId fileId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteContentListByFileName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteContentListByFileName(T::SessionId sessionId,const std::string& filename)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteContentListByProducerId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteContentListByProducerId(T::SessionId sessionId,T::ProducerId producerId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteContentListByProducerName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteContentListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteContentListByGenerationId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteContentListByGenerationId(T::SessionId sessionId,T::GenerationId generationId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteContentListByGenerationName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteContentListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of deleteContentListBySourceId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_deleteContentListBySourceId(T::SessionId sessionId,T::SourceId sourceId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentInfo() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentInfo(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::ContentInfo& contentInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentList(T::SessionId sessionId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentListByFileId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByFileId(T::SessionId sessionId,T::FileId fileId,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentListByFileIdList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByFileIdList(T::SessionId sessionId,std::vector<T::FileId>& fileIdList,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentListByFileName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByFileName(T::SessionId sessionId,const std::string& filename,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentListByProducerId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentListByProducerName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByProducerName(T::SessionId sessionId,const std::string& producerName,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentListBySourceId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentListByGenerationId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentListByGenerationName() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByGenerationName(T::SessionId sessionId,const std::string& generationName,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentListByGenerationIdAndTimeRange() (string form) — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,T::GenerationId generationId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList)
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





/*! \brief Protected hook for backend implementations of getContentListByGenerationIdAndTimeRange() (epoch form) — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,T::GenerationId generationId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




/*! \brief Protected hook for backend implementations of getContentListByGenerationNameAndTimeRange() (string form) — base returns NOT_IMPLEMENTED. */

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





/*! \brief Protected hook for backend implementations of getContentListByGenerationNameAndTimeRange() (epoch form) — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentListByParameter() (string form) — base returns NOT_IMPLEMENTED. */

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





/*! \brief Protected hook for backend implementations of getContentListByParameter() (epoch form) — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentListByParameterAndGenerationId() (string form) — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByParameterAndGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
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





/*! \brief Protected hook for backend implementations of getContentListByParameterAndGenerationId() (epoch form) — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByParameterAndGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentListByParameterAndGenerationName() (string form) — base returns NOT_IMPLEMENTED. */

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





/*! \brief Protected hook for backend implementations of getContentListByParameterAndGenerationName() (epoch form) — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentListByParameterAndProducerId() (string form) — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByParameterAndProducerId(T::SessionId sessionId,T::ProducerId producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
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





/*! \brief Protected hook for backend implementations of getContentListByParameterAndProducerId() (epoch form) — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByParameterAndProducerId(T::SessionId sessionId,T::ProducerId producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentListByParameterAndProducerName() (string form) — base returns NOT_IMPLEMENTED. */

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





/*! \brief Protected hook for backend implementations of getContentListByParameterAndProducerName() (epoch form) — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentListByParameterGenerationIdAndForecastTime() (string form) — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,T::ContentInfoList& contentInfoList)
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





/*! \brief Protected hook for backend implementations of getContentListByParameterGenerationIdAndForecastTime() (epoch form) — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentListOfInvalidIntegrity() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentGeometryIdListByGenerationId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentGeometryIdListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,std::set<T::GeometryId>& geometryIdList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentLevelListByGenerationGeometryAndLevelId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentLevelListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentLevelListByParameterGenerationGeometryAndLevelId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentLevelListByParameterGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,std::string parameterKey,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




/*! \brief Protected hook for backend implementations of getContentParamListByGenerationId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentParamListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ContentInfoList& contentParamList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentParamKeyListByGenerationId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentParamKeyListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentParamKeyListByGenerationAndGeometryId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentParamKeyListByGenerationAndGeometryId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentParamKeyListByGenerationGeometryAndLevelId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentParamKeyListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentTimeListByGenerationId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentTimeListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,std::set<std::string>& contentTimeList)
{
  return 0; // throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentTimeListByGenerationAndGeometryId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList)
{
  return 0; // throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentTimeListByGenerationGeometryAndLevelId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentTimeListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<std::string>& contentTimeList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




/*! \brief Protected hook for backend implementations of getContentTimeListByProducerId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentTimeListByProducerId(T::SessionId sessionId,T::ProducerId producerId,std::set<std::string>& contentTimeList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentTimeRangeByProducerAndGenerationId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentTimeRangeByProducerAndGenerationId(T::SessionId sessionId,T::ProducerId producerId,T::GenerationId generationId,time_t& startTime,time_t& endTime)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentTimeRangeByGenerationId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentTimeRangeByGenerationId(T::SessionId sessionId,T::GenerationId generationId,time_t& startTime,time_t& endTime)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getGenerationIdGeometryIdAndForecastTimeList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getContentCount() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getContentCount(T::SessionId sessionId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getHashByProducerId() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getHashByProducerId(T::SessionId sessionId,T::ProducerId producerId,UInt64 & hash)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





/*! \brief Protected hook for backend implementations of getLevelInfoList() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




/*! \brief Protected hook for backend implementations of updateCachedFiles() — base returns NOT_IMPLEMENTED. */

int ServiceInterface::_updateCachedFiles(T::SessionId sessionId,std::set<T::FileId>& fileIdList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




}
}
