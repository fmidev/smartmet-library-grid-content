#include "ServerInterface.h"
#include "queryServer/corba/convert/Converter.h"
#include "contentServer/corba/convert/Converter.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/ShowFunction.h"


#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace QueryServer
{
namespace Corba
{


ServerInterface::ServerInterface()
{
  FUNCTION_TRACE
  try
  {
    mService = NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ServerInterface::~ServerInterface()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ServerInterface::init(QueryServer::ServiceInterface *service)
{
  FUNCTION_TRACE
  try
  {
    mService = service;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





::CORBA::Long ServerInterface::executeQuery(::CORBA::LongLong sessionId, SmartMet::QueryServer::Corba::CorbaQuery& query)
{
  FUNCTION_TRACE
  try
  {
    if (mService == NULL)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    QueryServer::Query sQuery;
    QueryServer::Corba::Converter::convert(query,sQuery);

    int result = mService->executeQuery(sessionId,sQuery);
    if (result == 0)
      QueryServer::Corba::Converter::convert(sQuery,query);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",NULL);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getValuesByGridPoint(::CORBA::LongLong sessionId, const SmartMet::ContentServer::Corba::CorbaContentInfoList& contentInfoList, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Octet interpolationMethod, SmartMet::QueryServer::Corba::CorbaGridPointValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == NULL)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    T::GridPointValueList sValueList;
    T::ContentInfoList sContentInfoList;

    QueryServer::Corba::CorbaGridPointValueList *corbaValueList = new QueryServer::Corba::CorbaGridPointValueList();
    valueList = corbaValueList;

    ContentServer::Corba::Converter::convert(contentInfoList,sContentInfoList);

    int result = mService->getValuesByGridPoint(sessionId,sContentInfoList,(T::CoordinateType)coordinateType,x,y,(T::InterpolationMethod)interpolationMethod,sValueList);
    if (result == 0)
      QueryServer::Corba::Converter::convert(sValueList,*valueList);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",NULL);
    //exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





}
}
}

