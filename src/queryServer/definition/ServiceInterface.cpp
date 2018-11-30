#include "ServiceInterface.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/ShowFunction.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace QueryServer
{


ServiceInterface::ServiceInterface()
{
  FUNCTION_TRACE
  try
  {
    mDebugLog = nullptr;
    mProcessingLog = nullptr;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    Spine::Exception exception(BCP,exception_operation_failed,nullptr);
    exception.printError();
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::executeQuery(T::SessionId sessionId,Query& query)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _executeQuery(sessionId,query);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,Query);result %d;time %f;",__FUNCTION__,sessionId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getProducerList(T::SessionId sessionId,string_vec& producerList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getProducerList(sessionId,producerList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,producer[%lu]);result %d;time %f;",__FUNCTION__,sessionId,producerList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::GridPointValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getValuesByGridPoint(sessionId,contentInfoList,coordinateType,x,y,interpolationMethod,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,ContentInfo[%u],%u,%f,%f,%u,GridPointValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,contentInfoList.getLength(),coordinateType,x,y,interpolationMethod,valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::_executeQuery(T::SessionId sessionId,Query& query)
{
  throw Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getProducerList(T::SessionId sessionId,string_vec& producerList)
{
  throw Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::GridPointValueList& valueList)
{
  throw Spine::Exception(BCP,exception_implementation_required);
}


}
}

