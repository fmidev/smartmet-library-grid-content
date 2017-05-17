#include "ServiceImplementation.h"
#include "grid-files/common/Exception.h"
#include "dataServer/corba/client/ClientImplementation.h"


namespace SmartMet
{
namespace QueryServer
{


ServiceImplementation::ServiceImplementation()
{
  try
  {
    mContentServer = NULL;
    mDataServer = NULL;
    for (uint t=0; t<64; t++)
      mDataServerClient[t] = NULL;

    mServerCounter = 0;
    mServerCheckTime = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ServiceImplementation::~ServiceImplementation()
{
  try
  {
    if (mDataServer == NULL)
    {
      for (uint t=0; t<64; t++)
      {
        if (mDataServerClient[t] != NULL)
          delete mDataServerClient[t];
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::init (ContentServer::ServiceInterface *contentServer,DataServer::ServiceInterface *dataServer)
{
  try
  {
    mContentServer = contentServer;
    mDataServer = dataServer;

    if (mDataServer != NULL)
    {
      for (uint t=0; t<64; t++)
      {
        mDataServerClient[t] = mDataServer;
      }
    }
    else
    {
      checkDataServerConnections();
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::shutdown()
{
  try
  {
    exit(0);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::checkDataServerConnections()
{
  try
  {
    mServerCheckTime = time(0);
    if (mDataServer == NULL)
    {
      T::ServerInfoList serverInfoList;
      mContentServer->getDataServerInfoList(0,serverInfoList);
      uint len = serverInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ServerInfo *serverInfo = serverInfoList.getServerInfoByIndex(t);
        if (mDataServerClient[serverInfo->mServerId - 1] == NULL)
        {
          if (serverInfo->mServerId > 0  &&  serverInfo->mServerId < 65)
          {
            DataServer::Corba::ClientImplementation *client = new DataServer::Corba::ClientImplementation();
            try
            {
              client->init(serverInfo->mServerIor.c_str());
              mDataServerClient[serverInfo->mServerId - 1] = client;
            }
            catch (...)
            {
              delete client;
            }

          }
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceImplementation::_getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::GridPointValueList& valueList)
{
  try
  {
    uint len = contentInfoList.getLength();
    for (uint c=0; c<len; c++)
    {
      T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(c);
      DataServer::ServiceInterface *dataServer = getServerPtr(contentInfo->mServerFlags);
      if (dataServer)
      {
        T::ParamValue value = 0;
        int res = dataServer->getGridValue(sessionId,contentInfo->mFileId,contentInfo->mMessageIndex,coordinateType,x,y,interpolationMethod,value);
        if (res == DataServer::Result::OK)
        {
          valueList.addGridPointValue(new T::GridPointValue(contentInfo->mFileId,contentInfo->mMessageIndex,x,y,contentInfo->mParameterLevel,contentInfo->mStartTime,value));
        }
        else
        {
          valueList.addGridPointValue(new T::GridPointValue(contentInfo->mFileId,contentInfo->mMessageIndex,x,y,contentInfo->mParameterLevel,contentInfo->mStartTime,ParamValueMissing));
        }
      }
      else
      {
        printf("No data server %llu\n",contentInfo->mServerFlags);
        valueList.addGridPointValue(new T::GridPointValue(contentInfo->mFileId,contentInfo->mMessageIndex,x,y,contentInfo->mParameterLevel,contentInfo->mStartTime,ParamValueMissing));
      }
    }
    return 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





DataServer::ServiceInterface* ServiceImplementation::getServerPtr(unsigned long long serverFlags)
{
  try
  {
    if (mDataServer != NULL)
      return mDataServer;

    if ((time(0) - mServerCheckTime) > 10)
      checkDataServerConnections();

    for (uint t=0; t<64; t++)
    {
      uint idx = mServerCounter % 64;
      mServerCounter++;

      unsigned long long sf = (unsigned long long)1 << idx;

      if (mDataServerClient[idx] != NULL  &&  (serverFlags == 0  ||  (serverFlags & sf) != 0))
        return mDataServerClient[idx];
    }

    checkDataServerConnections();

    for (uint t=0; t<64; t++)
    {
      uint idx = mServerCounter % 64;
      mServerCounter++;

      unsigned long long sf = (unsigned long long)1 << idx;

      if (mDataServerClient[idx] != NULL  &&  (serverFlags == 0  ||  (serverFlags & sf) != 0))
        return mDataServerClient[idx];
    }

    return NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}


}
}

