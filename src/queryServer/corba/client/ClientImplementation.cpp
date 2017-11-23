#include "ClientImplementation.h"
#include "queryServer/corba/convert/Converter.h"
#include "contentServer/corba/convert/Converter.h"
#include "contentServer/definition/ServiceInterface.h"
#include <grid-files/common/Exception.h>


namespace SmartMet
{
namespace QueryServer
{
namespace Corba
{


ClientImplementation::ClientImplementation()
{
  try
  {
    mInitialized = false;
    mService = NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}






ClientImplementation::~ClientImplementation()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ClientImplementation::init(std::string serviceIor)
{
  try
  {
    try
    {
      mServiceIor = serviceIor;

      int argc = 2;
      char *argv[] =
      { const_cast<char*>("-ORBgiopMaxMsgSize"), const_cast<char*>("250000000") };
      CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

      CORBA::Object_var obj;
      obj = orb->string_to_object(serviceIor.c_str());

      mService = QueryServer::Corba::ServiceInterface::_narrow(obj);

      if (CORBA::is_nil(mService))
      {
        SmartMet::Spine::Exception exception(BCP, "Can't narrow reference to type QueryServer::Corba::ServiceInterace (or it was nil)!");
        throw exception;
      }

      omniORB::setClientCallTimeout(3600000);

      mInitialized = true;
    }
    catch (CORBA::TRANSIENT&)
    {
      SmartMet::Spine::Exception exception(BCP, "Caught system exception TRANSIENT -- unable to contact the server!");
      throw exception;
    }
    catch (CORBA::SystemException& ex)
    {
      char msg[500];
      sprintf(msg, "Caught a CORBA::%s\n", ex._name());
      SmartMet::Spine::Exception exception(BCP, msg);
      throw exception;
    }
    catch (CORBA::Exception& ex)
    {
      char msg[500];
      sprintf(msg, "Exception CORBA::%s\n", ex._name());
      SmartMet::Spine::Exception exception(BCP, msg);
      throw exception;
    }
    catch (omniORB::fatalException& fe)
    {
      char msg[500];
      sprintf(msg, "Caught omniORB::fatalException:%s\n", fe.errmsg());
      SmartMet::Spine::Exception exception(BCP, msg);
      throw exception;
    }
    catch (SmartMet::Spine::Exception& e)
    {
      SmartMet::Spine::Exception exception(BCP, exception_operation_failed, &e);
      throw exception;
    }
    catch (...)
    {
      SmartMet::Spine::Exception exception(BCP, "Unexpected exception!", NULL);
      throw exception;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, exception_operation_failed, NULL);
  }
}





int ClientImplementation::_executeQuery(T::SessionId sessionId,Query& query)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    QueryServer::Corba::CorbaQuery_var corbaQuery = new QueryServer::Corba::CorbaQuery();
    QueryServer::Corba::Converter::convert(query, corbaQuery);

    int result = mService->executeQuery(sessionId, corbaQuery);

    if (result == 0)
      QueryServer::Corba::Converter::convert(corbaQuery, query);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, exception_operation_failed, NULL);
  }
}





int ClientImplementation::_getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::GridPointValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    QueryServer::Corba::CorbaGridPointValueList_var corbaValueList = new QueryServer::Corba::CorbaGridPointValueList();
    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();

    ContentServer::Corba::Converter::convert(contentInfoList, corbaContentInfoList);

    int result = mService->getValuesByGridPoint(sessionId,corbaContentInfoList,(::CORBA::Octet)coordinateType,x,y,(::CORBA::Octet)interpolationMethod,corbaValueList);

    if (result == 0)
    {
      QueryServer::Corba::Converter::convert(corbaValueList, valueList);
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, exception_operation_failed, NULL);
  }
}


}
}
}
