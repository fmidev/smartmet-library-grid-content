#include "ServerInterface.h"
#include "../convert/Converter.h"
#include "../../../contentServer/corba/convert/Converter.h"

#include <grid-files/common/Exception.h>
#include <grid-files/common/ShowFunction.h>


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
    mService = nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





::CORBA::Long ServerInterface::executeQuery(::CORBA::LongLong sessionId, SmartMet::QueryServer::Corba::CorbaQuery& query)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
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
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getProducerList(::CORBA::LongLong sessionId, SmartMet::QueryServer::Corba::CorbaStringList_out producerList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    string_vec sProducerList;

    QueryServer::Corba::CorbaStringList *corbaProducerList = new QueryServer::Corba::CorbaStringList();
    producerList = corbaProducerList;

    int result = mService->getProducerList(sessionId,sProducerList);
    if (result == 0)
      QueryServer::Corba::Converter::convert(sProducerList,*producerList);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    //exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getValuesByGridPoint(::CORBA::LongLong sessionId, const SmartMet::ContentServer::Corba::CorbaContentInfoList& contentInfoList, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Short interpolationMethod, SmartMet::QueryServer::Corba::CorbaGridPointValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    T::GridPointValueList sValueList;
    T::ContentInfoList sContentInfoList;

    QueryServer::Corba::CorbaGridPointValueList *corbaValueList = new QueryServer::Corba::CorbaGridPointValueList();
    valueList = corbaValueList;

    ContentServer::Corba::Converter::convert(contentInfoList,sContentInfoList);

    int result = mService->getValuesByGridPoint(sessionId,sContentInfoList,coordinateType,x,y,(short)interpolationMethod,sValueList);
    if (result == 0)
      QueryServer::Corba::Converter::convert(sValueList,*valueList);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    //exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





}
}
}

