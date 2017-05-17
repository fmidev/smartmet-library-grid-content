#include "ServerInterface.h"
#include "queryServer/corba/convert/Converter.h"
#include "contentServer/corba/convert/Converter.h"
#include "grid-files/common/Exception.h"


namespace SmartMet
{
namespace QueryServer
{
namespace Corba
{


ServerInterface::ServerInterface()
{
  try
  {
    mService = NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ServerInterface::~ServerInterface()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::init(QueryServer::ServiceInterface *service)
{
  try
  {
    mService = service;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





::CORBA::Long ServerInterface::getValuesByGridPoint(::CORBA::LongLong sessionId, const SmartMet::ContentServer::Corba::CorbaContentInfoList& contentInfoList, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Octet interpolationMethod, SmartMet::QueryServer::Corba::CorbaGridPointValueList_out valueList)
{
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

