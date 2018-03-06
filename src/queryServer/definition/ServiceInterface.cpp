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
    mDebugLog = NULL;
    mProcessingLog = NULL;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,Query);result %d;time %f;",__FUNCTION__,sessionId,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,producer[%u]);result %d;time %f;",__FUNCTION__,sessionId,(uint)producerList.size(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int ServiceInterface::getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,T::AreaInterpolationMethod interpolationMethod,T::GridPointValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getValuesByGridPoint(sessionId,contentInfoList,coordinateType,x,y,interpolationMethod,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,ContentInfo[%u],%u,%f,%f,%u,GridPointValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,contentInfoList.getLength(),(uint)coordinateType,x,y,(uint)interpolationMethod,valueList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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





int ServiceInterface::_getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,T::AreaInterpolationMethod interpolationMethod,T::GridPointValueList& valueList)
{
  throw Spine::Exception(BCP,exception_implementation_required);
}


}
}

