#include "ClientImplementation.h"
#include "contentServer/corba/convert/Converter.h"
#include "grid-files/common/GeneralFunctions.h"
#include "grid-files/common/Exception.h"

namespace SmartMet
{
namespace ContentServer
{
namespace Corba
{


ClientImplementation::ClientImplementation()
{
  try
  {
    mId = 0;
    mInitialized = false;
    mLastAccessTime = 0;
    mLastErrorTime = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





ClientImplementation::~ClientImplementation()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
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
      { const_cast<char*>("-ORBgiopMaxMsgSize"), const_cast<char*>("100000000") };
      CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

      CORBA::Object_var obj;
      obj = orb->string_to_object(serviceIor.c_str());

      mService = ContentServer::Corba::ServiceInterface::_narrow(obj);

      if (CORBA::is_nil(mService))
      {
        SmartMet::Spine::Exception exception(BCP, "Can't narrow reference to type ContentServer::Corba::ServiceInterace (or it was nil)!");
        throw exception;
      }

      omniORB::setClientCallTimeout(3600000);

      mId = stringToId(serviceIor.c_str(),100);
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
      SmartMet::Spine::Exception exception(BCP, "Operation failed!", &e);
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
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





uint ClientImplementation::getId()
{
  try
  {
    return mId;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ClientImplementation::setId(uint id)
{
  try
  {
    mId = id;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





time_t ClientImplementation::getLastAccessTime()
{
  try
  {
    return mLastAccessTime;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ClientImplementation::setLastAccessTime(time_t accessTime)
{
  try
  {
    mLastAccessTime = accessTime;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





time_t ClientImplementation::getLastErrorTime()
{
  try
  {
    return mLastErrorTime;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ClientImplementation::setLastErrorTime(time_t errorTime)
{
  try
  {
    mLastErrorTime = errorTime;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_clear(T::SessionId sessionId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->clear(sessionId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_reload(T::SessionId sessionId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->reload(sessionId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_addDataServerInfo(T::SessionId sessionId, T::ServerInfo& serverInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaServerInfo_var corbaServerInfo = new ContentServer::Corba::CorbaServerInfo();
    ContentServer::Corba::Converter::convert(serverInfo, corbaServerInfo);

    int result = mService->addDataServerInfo(sessionId,corbaServerInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaServerInfo, serverInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteDataServerInfoById(T::SessionId sessionId, uint serverId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteDataServerInfoById(sessionId, serverId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getDataServerInfoById(T::SessionId sessionId, uint serverId, T::ServerInfo& serverInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaServerInfo_var corbaServerInfo;

    int result = mService->getDataServerInfoById(sessionId, serverId, corbaServerInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaServerInfo, serverInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getDataServerInfoByName(T::SessionId sessionId, std::string serverName, T::ServerInfo& serverInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaServerInfo_var corbaServerInfo;

    int result = mService->getDataServerInfoByName(sessionId, serverName.c_str(), corbaServerInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaServerInfo, serverInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getDataServerInfoByIor(T::SessionId sessionId, std::string serverIor, T::ServerInfo& serverInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaServerInfo_var corbaServerInfo;

    int result = mService->getDataServerInfoByIor(sessionId, serverIor.c_str(), corbaServerInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaServerInfo, serverInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getDataServerInfoList(T::SessionId sessionId, T::ServerInfoList& serverInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaServerInfoList_var corbaServerInfoList;

    int result = mService->getDataServerInfoList(sessionId, corbaServerInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaServerInfoList, serverInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getDataServerInfoCount(T::SessionId sessionId,uint& count)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ::CORBA::ULong corbaCount = 0;

    int result = mService->getDataServerInfoCount(sessionId, corbaCount);

    if (result == 0)
      count = (uint)corbaCount;

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_addProducerInfo(T::SessionId sessionId, T::ProducerInfo& producerInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaProducerInfo_var corbaProducerInfo = new ContentServer::Corba::CorbaProducerInfo();
    ContentServer::Corba::Converter::convert(producerInfo, corbaProducerInfo);

    int result = mService->addProducerInfo(sessionId, corbaProducerInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaProducerInfo, producerInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteProducerInfoById(T::SessionId sessionId, uint producerId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteProducerInfoById(sessionId, producerId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteProducerInfoByName(T::SessionId sessionId,std::string producerName)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteProducerInfoByName(sessionId, producerName.c_str());

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteProducerInfoListBySourceId(sessionId,sourceId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getProducerInfoById(T::SessionId sessionId, uint producerId, T::ProducerInfo& producerInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaProducerInfo_var corbaProducerInfo;

    int result = mService->getProducerInfoById(sessionId, producerId, corbaProducerInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaProducerInfo, producerInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getProducerInfoByName(T::SessionId sessionId, std::string producerName, T::ProducerInfo& producerInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaProducerInfo_var corbaProducerInfo;

    int result = mService->getProducerInfoByName(sessionId, producerName.c_str(), corbaProducerInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaProducerInfo, producerInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getProducerInfoList(T::SessionId sessionId, T::ProducerInfoList& producerInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaProducerInfoList_var corbaProducerInfoList;

    int result = mService->getProducerInfoList(sessionId, corbaProducerInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaProducerInfoList, producerInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaProducerInfoList_var corbaProducerInfoList;

    int result = mService->getProducerInfoListBySourceId(sessionId,sourceId,corbaProducerInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaProducerInfoList, producerInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getProducerInfoCount(T::SessionId sessionId,uint& count)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ::CORBA::ULong corbaCount = 0;

    int result = mService->getProducerInfoCount(sessionId, corbaCount);

    if (result == 0)
      count = (uint)corbaCount;

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_addGenerationInfo(T::SessionId sessionId, T::GenerationInfo& generationInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaGenerationInfo_var corbaGenerationInfo = new ContentServer::Corba::CorbaGenerationInfo();
    ContentServer::Corba::Converter::convert(generationInfo, corbaGenerationInfo);

    int result = mService->addGenerationInfo(sessionId, corbaGenerationInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaGenerationInfo, generationInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteGenerationInfoById(T::SessionId sessionId, uint generationId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteGenerationInfoById(sessionId, generationId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteGenerationInfoByName(T::SessionId sessionId,std::string generationName)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteGenerationInfoByName(sessionId, generationName.c_str());

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteGenerationInfoListByProducerId(sessionId, producerId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteGenerationInfoListByProducerName(sessionId, producerName.c_str());

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteGenerationInfoListBySourceId(sessionId, sourceId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getGenerationInfoById(T::SessionId sessionId, uint generationId, T::GenerationInfo& generationInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaGenerationInfo_var corbaGenerationInfo;

    int result = mService->getGenerationInfoById(sessionId, generationId, corbaGenerationInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaGenerationInfo, generationInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getGenerationInfoByName(T::SessionId sessionId,std::string generationName,T::GenerationInfo& generationInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaGenerationInfo_var corbaGenerationInfo;

    int result = mService->getGenerationInfoByName(sessionId, generationName.c_str(), corbaGenerationInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaGenerationInfo, generationInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getGenerationInfoList(T::SessionId sessionId, T::GenerationInfoList& generationInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaGenerationInfoList_var corbaGenerationInfoList;

    int result = mService->getGenerationInfoList(sessionId, corbaGenerationInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaGenerationInfoList, generationInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getGenerationInfoListByGeometryId(T::SessionId sessionId,uint geometryId,T::GenerationInfoList& generationInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaGenerationInfoList_var corbaGenerationInfoList;

    int result = mService->getGenerationInfoListByGeometryId(sessionId, geometryId, corbaGenerationInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaGenerationInfoList, generationInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getGenerationInfoListByProducerId(T::SessionId sessionId, uint producerId, T::GenerationInfoList& generationInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaGenerationInfoList_var corbaGenerationInfoList;

    int result = mService->getGenerationInfoListByProducerId(sessionId, producerId, corbaGenerationInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaGenerationInfoList, generationInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName,T::GenerationInfoList& generationInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaGenerationInfoList_var corbaGenerationInfoList;

    int result = mService->getGenerationInfoListByProducerName(sessionId, producerName.c_str(), corbaGenerationInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaGenerationInfoList, generationInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getGenerationInfoListBySourceId(T::SessionId sessionId, uint sourceId, T::GenerationInfoList& generationInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaGenerationInfoList_var corbaGenerationInfoList;

    int result = mService->getGenerationInfoListBySourceId(sessionId, sourceId, corbaGenerationInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaGenerationInfoList, generationInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,T::GenerationStatus generationStatus,T::GenerationInfo& generationInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaGenerationInfo_var corbaGenerationInfo;

    int result = mService->getLastGenerationInfoByProducerIdAndStatus(sessionId, producerId, generationStatus, corbaGenerationInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaGenerationInfo, generationInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,std::string producerName,T::GenerationStatus generationStatus,T::GenerationInfo& generationInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaGenerationInfo_var corbaGenerationInfo;

    int result = mService->getLastGenerationInfoByProducerNameAndStatus(sessionId, producerName.c_str(), generationStatus, corbaGenerationInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaGenerationInfo, generationInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getGenerationInfoCount(T::SessionId sessionId,uint& count)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ::CORBA::ULong corbaCount = 0;

    int result = mService->getGenerationInfoCount(sessionId, corbaCount);

    if (result == 0)
      count = (uint)corbaCount;

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,T::GenerationStatus status)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->setGenerationInfoStatusById(sessionId, generationId,  (CORBA::Octet)status);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_setGenerationInfoStatusByName(T::SessionId sessionId,std::string generationName,T::GenerationStatus status)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->setGenerationInfoStatusByName(sessionId, generationName.c_str(),  (CORBA::Octet)status);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_addFileInfo(T::SessionId sessionId, T::FileInfo& fileInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaFileInfo_var corbaFileInfo = new ContentServer::Corba::CorbaFileInfo();
    ContentServer::Corba::Converter::convert(fileInfo, corbaFileInfo);

    int result = mService->addFileInfo(sessionId, corbaFileInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaFileInfo, fileInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaFileInfo_var corbaFileInfo = new ContentServer::Corba::CorbaFileInfo();
    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();

    ContentServer::Corba::Converter::convert(fileInfo, corbaFileInfo);
    ContentServer::Corba::Converter::convert(contentInfoList, corbaContentInfoList);

    int result = mService->addFileInfoWithContentList(sessionId, corbaFileInfo, corbaContentInfoList);

    if (result == 0)
    {
      ContentServer::Corba::Converter::convert(corbaFileInfo, fileInfo);
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);
    }

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteFileInfoById(T::SessionId sessionId, uint fileId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteFileInfoById(sessionId, fileId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteFileInfoByName(T::SessionId sessionId,std::string filename)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteFileInfoByName(sessionId, filename.c_str());

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteFileInfoListByGroupFlags(T::SessionId sessionId, uint groupFlags)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteFileInfoListByGroupFlags(sessionId, groupFlags);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteFileInfoListByProducerId(T::SessionId sessionId, uint producerId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteFileInfoListByProducerId(sessionId, producerId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteFileInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteFileInfoListByProducerName(sessionId, producerName.c_str());

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteFileInfoListByGenerationId(T::SessionId sessionId, uint generationId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteFileInfoListByGenerationId(sessionId, generationId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteFileInfoListByGenerationName(sessionId, generationName.c_str());

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteFileInfoListBySourceId(T::SessionId sessionId, uint sourceId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteFileInfoListBySourceId(sessionId, sourceId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaULongList_var corbaFileIdList = new ContentServer::Corba::CorbaULongList();
    ContentServer::Corba::Converter::convert(fileIdList, corbaFileIdList);

    int result = mService->deleteFileInfoListByFileIdList(sessionId, corbaFileIdList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




int ClientImplementation::_getFileInfoById(T::SessionId sessionId, uint fileId, T::FileInfo& fileInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaFileInfo_var corbaFileInfo;

    int result = mService->getFileInfoById(sessionId, fileId, corbaFileInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaFileInfo, fileInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getFileInfoByName(T::SessionId sessionId,std::string filename,T::FileInfo& fileInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaFileInfo_var corbaFileInfo;

    int result = mService->getFileInfoByName(sessionId, filename.c_str(), corbaFileInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaFileInfo, fileInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getFileInfoList(T::SessionId sessionId, uint startFileId, uint maxRecords, T::FileInfoList& fileInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaFileInfoList_var corbaFileInfoList;

    int result = mService->getFileInfoList(sessionId, startFileId, maxRecords, corbaFileInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaFileInfoList, fileInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getFileInfoListByProducerId(T::SessionId sessionId, uint producerId, uint startFileId, uint maxRecords, T::FileInfoList& fileInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaFileInfoList_var corbaFileInfoList;

    int result = mService->getFileInfoListByProducerId(sessionId, producerId, startFileId, maxRecords, corbaFileInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaFileInfoList, fileInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getFileInfoListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaFileInfoList_var corbaFileInfoList;

    int result = mService->getFileInfoListByProducerName(sessionId, producerName.c_str(), startFileId, maxRecords, corbaFileInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaFileInfoList, fileInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getFileInfoListByGenerationId(T::SessionId sessionId, uint generationId, uint startFileId, uint maxRecords, T::FileInfoList& fileInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaFileInfoList_var corbaFileInfoList;

    int result = mService->getFileInfoListByGenerationId(sessionId, generationId, startFileId, maxRecords, corbaFileInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaFileInfoList, fileInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaFileInfoList_var corbaFileInfoList;

    int result = mService->getFileInfoListByGenerationName(sessionId, generationName.c_str(), startFileId, maxRecords, corbaFileInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaFileInfoList, fileInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getFileInfoListByGroupFlags(T::SessionId sessionId, uint groupFlags, uint startFileId, uint maxRecords, T::FileInfoList& fileInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaFileInfoList_var corbaFileInfoList;

    int result = mService->getFileInfoListByGroupFlags(sessionId, groupFlags, startFileId, maxRecords, corbaFileInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaFileInfoList, fileInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getFileInfoListBySourceId(T::SessionId sessionId, uint sourceId, uint startFileId, uint maxRecords, T::FileInfoList& fileInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaFileInfoList_var corbaFileInfoList;

    int result = mService->getFileInfoListBySourceId(sessionId, sourceId, startFileId, maxRecords, corbaFileInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaFileInfoList, fileInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getFileInfoCount(T::SessionId sessionId,uint& count)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ::CORBA::ULong corbaCount = 0;

    int result = mService->getFileInfoCount(sessionId, corbaCount);

    if (result == 0)
      count = (uint)corbaCount;

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaEventInfo_var corbaEventInfo = new ContentServer::Corba::CorbaEventInfo();
    ContentServer::Corba::Converter::convert(eventInfo, corbaEventInfo);

    int result = mService->addEventInfo(sessionId,corbaEventInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaEventInfo, eventInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}



int ClientImplementation::_getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    if (mLastErrorTime > mLastAccessTime)
    {
      mInitialized = false;
      init(mServiceIor);
    }

    ContentServer::Corba::CorbaEventInfo_var corbaEventInfo;

    int result = mService->getLastEventInfo(sessionId, requestingServerId, corbaEventInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaEventInfo, eventInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getEventInfoList(T::SessionId sessionId, uint requestingServerId, T::EventId startEventId, uint maxRecords, T::EventInfoList& eventInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    if (mLastErrorTime > mLastAccessTime)
    {
      mInitialized = false;
      init(mServiceIor);
    }

    ContentServer::Corba::CorbaEventInfoList_var corbaEventInfoList;

    int result = mService->getEventInfoList(sessionId, requestingServerId, startEventId, maxRecords, corbaEventInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaEventInfoList, eventInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getEventInfoCount(T::SessionId sessionId,uint& count)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ::CORBA::ULong corbaCount = 0;

    int result = mService->getEventInfoCount(sessionId, corbaCount);

    if (result == 0)
      count = (uint)corbaCount;

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_addContentInfo(T::SessionId sessionId, T::ContentInfo& contentInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfo_var corbaContentInfo = new ContentServer::Corba::CorbaContentInfo();
    ContentServer::Corba::Converter::convert(contentInfo, corbaContentInfo);

    int result = mService->addContentInfo(sessionId, corbaContentInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfo, contentInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_addContentList(T::SessionId sessionId, T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    ContentServer::Corba::Converter::convert(contentInfoList, corbaContentInfoList);

    int result = mService->addContentList(sessionId, corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteContentInfo(sessionId,fileId,messageIndex);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteContentListByFileId(T::SessionId sessionId,uint fileId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteContentListByFileId(sessionId, fileId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteContentListByFileName(T::SessionId sessionId,std::string filename)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteContentListByFileName(sessionId, filename.c_str());

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteContentListByGroupFlags(T::SessionId sessionId, uint groupFlags)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteContentListByGroupFlags(sessionId, groupFlags);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteContentListByProducerId(T::SessionId sessionId, uint producerId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteContentListByProducerId(sessionId, producerId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteContentListByProducerName(T::SessionId sessionId,std::string producerName)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteContentListByProducerName(sessionId, producerName.c_str());

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




int ClientImplementation::_deleteContentListByGenerationId(T::SessionId sessionId, uint generationId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteContentListByGenerationId(sessionId, generationId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteContentListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteContentListByGenerationName(sessionId, generationName.c_str());

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_deleteContentListBySourceId(T::SessionId sessionId, uint sourceId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->deleteContentListBySourceId(sessionId, sourceId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_registerContentList(T::SessionId sessionId,uint serverId,T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    ContentServer::Corba::Converter::convert(contentInfoList, corbaContentInfoList);

    int result = mService->registerContentList(sessionId, serverId, corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_registerContentListByFileId(T::SessionId sessionId, uint serverId, uint fileId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->registerContentListByFileId(sessionId, serverId, fileId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_unregisterContentList(T::SessionId sessionId, uint serverId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->unregisterContentList(sessionId, serverId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_unregisterContentListByFileId(T::SessionId sessionId, uint serverId, uint fileId)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    int result = mService->unregisterContentListByFileId(sessionId, serverId, fileId);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfo_var corbaContentInfo;

    int result = mService->getContentInfo(sessionId,fileId,messageIndex, corbaContentInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfo, contentInfo);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentList(T::SessionId sessionId, uint startFileId, uint startMessageIndex, uint maxRecords, T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentList(sessionId, startFileId, startMessageIndex, maxRecords, corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentListByFileId(T::SessionId sessionId, uint fileId, T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentListByFileId(sessionId, fileId, corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentListByFileName(T::SessionId sessionId,std::string filename,T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentListByFileName(sessionId, filename.c_str(), corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




int ClientImplementation::_getContentListByGroupFlags(T::SessionId sessionId, uint groupFlags, uint startFileId, uint startMessageIndex, uint maxRecords, T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentListByGroupFlags(sessionId, groupFlags, startFileId, startMessageIndex, maxRecords, corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentListByProducerId(T::SessionId sessionId, uint producerId, uint startFileId, uint startMessageIndex, uint maxRecords, T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentListByProducerId(sessionId, producerId, startFileId, startMessageIndex, maxRecords, corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentListByProducerName(sessionId, producerName.c_str(), startFileId, startMessageIndex, maxRecords, corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}








int ClientImplementation::_getContentListByServerId(T::SessionId sessionId, uint serverId, uint startFileId, uint startMessageIndex, uint maxRecords, T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentListByServerId(sessionId, serverId, startFileId, startMessageIndex, maxRecords, corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentListByGenerationId(sessionId, generationId, startFileId, startMessageIndex, maxRecords, corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentListByGenerationName(sessionId, generationName.c_str(), startFileId, startMessageIndex, maxRecords, corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentListByGenerationIdAndTimeRange(sessionId, generationId, startTime.c_str(), endTime.c_str(), corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,std::string generationName,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentListByGenerationNameAndTimeRange(sessionId, generationName.c_str(), startTime.c_str(), endTime.c_str(), corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentListBySourceId(T::SessionId sessionId, uint sourceId, uint startFileId, uint startMessageIndex, uint maxRecords, T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentListBySourceId(sessionId, sourceId, startFileId, startMessageIndex, maxRecords, corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentListByParameter(sessionId, (CORBA::Octet)parameterKeyType, parameterKey.c_str(),(CORBA::Octet)parameterLevelIdType,parameterLevelId,minLevel,maxLevel, startTime.c_str(), endTime.c_str(), requestFlags,corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentListByParameterAndGenerationId(sessionId,generationId,(CORBA::Octet)parameterKeyType, parameterKey.c_str(),(CORBA::Octet)parameterLevelIdType,parameterLevelId,minLevel,maxLevel, startTime.c_str(), endTime.c_str(), requestFlags, corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentListByParameterAndGenerationName(T::SessionId sessionId,std::string generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentListByParameterAndGenerationName(sessionId,generationName.c_str(),(CORBA::Octet)parameterKeyType, parameterKey.c_str(),(CORBA::Octet)parameterLevelIdType,parameterLevelId,minLevel,maxLevel, startTime.c_str(), endTime.c_str(), requestFlags, corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




int ClientImplementation::_getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentListByParameterAndProducerId(sessionId,producerId,(CORBA::Octet)parameterKeyType, parameterKey.c_str(),(CORBA::Octet)parameterLevelIdType,parameterLevelId,minLevel,maxLevel, startTime.c_str(), endTime.c_str(), requestFlags, corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




int ClientImplementation::_getContentListByParameterAndProducerName(T::SessionId sessionId,std::string producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentInfoList;

    int result = mService->getContentListByParameterAndProducerName(sessionId,producerName.c_str(),(CORBA::Octet)parameterKeyType, parameterKey.c_str(),(CORBA::Octet)parameterLevelIdType,parameterLevelId,minLevel,maxLevel, startTime.c_str(), endTime.c_str(), requestFlags, corbaContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentInfoList, contentInfoList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<uint>& geometryIdList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaULongList_var corbaGeometryIdList;

    int result = mService->getContentGeometryIdListByGenerationId(sessionId,generationId,corbaGeometryIdList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaGeometryIdList,geometryIdList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




int ClientImplementation::_getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaContentInfoList_var corbaContentParamList;

    int result = mService->getContentParamListByGenerationId(sessionId,generationId,corbaContentParamList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentParamList,contentParamList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::vector<std::string>& contentTimeList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ContentServer::Corba::CorbaStringList_var corbaContentTimeList;

    int result = mService->getContentTimeListByGenerationId(sessionId,generationId,corbaContentTimeList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(corbaContentTimeList,contentTimeList);

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ClientImplementation::_getContentCount(T::SessionId sessionId,uint& count)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP, "The client is not initialized!");

    ::CORBA::ULong corbaCount = 0;

    int result = mService->getContentCount(sessionId, corbaCount);

    if (result == 0)
      count = (uint)corbaCount;

    mLastAccessTime = time(0);
    return result;
  }
  catch (...)
  {
    mLastErrorTime = time(0);
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}


}
}
}

