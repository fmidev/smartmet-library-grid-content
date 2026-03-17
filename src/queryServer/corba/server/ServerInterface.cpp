#include "ServerInterface.h"
#include "../convert/Converter.h"
#include "../../../contentServer/corba/convert/Converter.h"

#include <macgyver/Exception.h>
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




SmartMet::C::Result ServerInterface::executeQuery(SmartMet::C::SessionId sessionId, SmartMet::C::Query& query)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    QueryServer::Query sQuery;
    QueryServer::Corba::Converter::convert(query,sQuery);

    int result = mService->executeQuery(sessionId,sQuery);
    if (result == 0)
      QueryServer::Corba::Converter::convert(sQuery,query);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getProducerList(SmartMet::C::SessionId sessionId, SmartMet::C::StringList_out producerList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    string_vec sProducerList;

    C::StringList *corbaProducerList = new C::StringList();
    producerList = corbaProducerList;

    int result = mService->getProducerList(sessionId,sProducerList);
    if (result == 0)
      QueryServer::Corba::Converter::convert(sProducerList,*producerList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    //exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




SmartMet::C::Result ServerInterface::getValuesByGridPoint(SmartMet::C::SessionId sessionId, const SmartMet::C::ContentInfoList& contentInfoList, SmartMet::C::UInt8 coordinateType, SmartMet::C::Float64 x, SmartMet::C::Float64 y, SmartMet::C::Int16 interpolationMethod, SmartMet::C::GridPointValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::GridPointValueList sValueList;
    T::ContentInfoList sContentInfoList;

    C::GridPointValueList *corbaValueList = new C::GridPointValueList();
    valueList = corbaValueList;

    ContentServer::Corba::Converter::convert(contentInfoList,sContentInfoList);

    int result = mService->getValuesByGridPoint(sessionId,sContentInfoList,coordinateType,x,y,(short)interpolationMethod,sValueList);
    if (result == 0)
      QueryServer::Corba::Converter::convert(sValueList,*valueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    //exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





}
}
}

