#include "RedisImplementation.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/AutoThreadLock.h"
#include "grid-files/common/ShowFunction.h"


#define FUNCTION_TRACE FUNCTION_TRACE_OFF



namespace SmartMet
{
namespace ContentServer
{


RedisImplementation::RedisImplementation()
{
  FUNCTION_TRACE
  try
  {
    mContext = NULL;
    mStartTime = time(0);
    mRedisPort = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





RedisImplementation::~RedisImplementation()
{
  FUNCTION_TRACE
  try
  {
    if (mContext != NULL)
      redisFree(mContext);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void RedisImplementation::init(const char *redisAddress,int redisPort)
{
  FUNCTION_TRACE
  try
  {
    mRedisAddress = redisAddress;
    mRedisPort = redisPort;

    openConnection();

    if (mContext == NULL)
      throw SmartMet::Spine::Exception(BCP,"Cannot connect to Redis!");

    resetContentRegistrations();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::openConnection()
{
  FUNCTION_TRACE
  try
  {
    if (mContext != NULL)
    {
      redisFree(mContext);
      mContext = NULL;
    }

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds

    mContext = redisConnectWithTimeout(mRedisAddress.c_str(), mRedisPort, timeout);
    if (mContext == NULL || mContext->err)
    {
      if (mContext)
      {
        printf("Redis connection error (%s:%d): %s",mRedisAddress.c_str(), mRedisPort,mContext->errstr);
        redisFree(mContext);
        mContext = NULL;
        return Result::PERMANENT_STORAGE_ERROR;
      }
      else
      {
        printf("Redis connection error (%s:%d): can't allocate redis context\n",mRedisAddress.c_str(), mRedisPort);
        return Result::PERMANENT_STORAGE_ERROR;
      }
    }

    mStartTime = time(0);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void RedisImplementation::closeConnection()
{
  FUNCTION_TRACE
  try
  {
    if (mContext != NULL)
    {
      redisFree(mContext);
      mContext = NULL;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void RedisImplementation::shutdown()
{
  FUNCTION_TRACE
  try
  {
    exit(0);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





bool RedisImplementation::isSessionValid(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    return true;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





bool RedisImplementation::isConnectionValid()
{
  FUNCTION_TRACE
  try
  {
    if (mContext == NULL  &&  openConnection() != Result::OK)
      return false;

    return true;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_clear(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"DEL producerCounter");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(mContext,"DEL producers");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(mContext,"DEL generationCounter");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(mContext,"DEL generations");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(mContext,"DEL fileCounter");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(mContext,"DEL files");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(mContext,"DEL filenames");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(mContext,"DEL contentCounter");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(mContext,"DEL content");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(mContext,"DEL dataServers");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(mContext,"DEL contentServers");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(mContext,"DEL events");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    addEvent(EventType::CLEAR,0,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_reload(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

   return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_addDataServerInfo(T::SessionId sessionId,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    if (serverInfo.mServerId < 1  ||  serverInfo.mServerId > 64)
      return Result::INVALID_SERVER_ID;

    T::ServerInfoList serverInfoList;
    getDataServerList(serverInfoList);

    T::ServerInfo *info = serverInfoList.getServerInfoById(serverInfo.mServerId);
    if (info != NULL)
      return Result::SERVER_ID_ALREADY_REGISTERED;

    info = serverInfoList.getServerInfoByName(serverInfo.mName);
    if (info != NULL)
      return Result::SERVER_NAME_ALREADY_REGISTERED;

    if (serverInfo.mServerIor.length() > 0)
    {
      info = serverInfoList.getServerInfoByIor(serverInfo.mServerIor);
      if (info != NULL)
        return Result::SERVER_IOR_ALREADY_REGISTERED;
    }

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZADD dataServers %u %s", serverInfo.mServerId,serverInfo.getCsv().c_str());
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    addEvent(EventType::DATA_SERVER_ADDED,serverInfo.mServerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteDataServerInfoById(T::SessionId sessionId,uint serverId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ServerInfo serverInfo;
    if (getDataServerById(serverId,serverInfo) != Result::OK)
      return Result::UNKNOWN_SERVER_ID;

    int result = deleteDataServerById(serverInfo.mServerId);

    if (result == Result::OK)
      addEvent(EventType::DATA_SERVER_DELETED,serverId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getDataServerInfoById(T::SessionId sessionId,uint serverId,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getDataServerById(serverId,serverInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getDataServerInfoByName(T::SessionId sessionId,std::string serverName,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ServerInfoList serverInfoList;
    getDataServerList(serverInfoList);

    T::ServerInfo *info = serverInfoList.getServerInfoByName(serverInfo.mName);
    if (info == NULL)
      return Result::DATA_NOT_FOUND;

    serverInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getDataServerInfoByIor(T::SessionId sessionId,std::string serverIor,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ServerInfoList serverInfoList;
    getDataServerList(serverInfoList);

    T::ServerInfo *info = serverInfoList.getServerInfoByIor(serverInfo.mServerIor);
    if (info == NULL)
      return Result::DATA_NOT_FOUND;

    serverInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getDataServerInfoList(T::SessionId sessionId,T::ServerInfoList& serverInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getDataServerList(serverInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfoList producerInfoList;
    getProducerList(producerInfoList);

    T::ProducerInfo *info = producerInfoList.getProducerInfoByName(producerInfo.mName);
    if (info != NULL)
      return Result::PRODUCER_NAME_ALREADY_REGISTERED;

    redisReply *reply = NULL;

    if (producerInfo.mProducerId == 0)
    {
      reply = (redisReply*)redisCommand(mContext,"INCR producerCounter");
      if (reply == NULL)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      producerInfo.mProducerId = (uint)reply->integer;
      freeReplyObject(reply);
    }
    else
    {
      T::ProducerInfo *info = producerInfoList.getProducerInfoById(producerInfo.mProducerId);
      if (info != NULL)
        return Result::PRODUCER_ID_ALREADY_REGISTERED;

      reply = (redisReply*)redisCommand(mContext,"GET producerCounter");
      if (reply == NULL)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      uint id = 0;
      if (reply != NULL  &&  reply->str != NULL)
        id = (uint)atoi(reply->str);
      // printf("**** ID %u\n",id);
      freeReplyObject(reply);

      if (id < producerInfo.mProducerId)
      {
        reply = (redisReply*)redisCommand(mContext,"SET producerCounter %u",producerInfo.mProducerId);
        if (reply == NULL)
        {
          closeConnection();
          return Result::PERMANENT_STORAGE_ERROR;
        }

        freeReplyObject(reply);
        // printf("**** SET ID %u\n",producerInfo.mProducerId);
      }
    }

    reply = (redisReply*)redisCommand(mContext,"ZADD producers %u %s", producerInfo.mProducerId,producerInfo.getCsv().c_str());
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    addEvent(EventType::PRODUCER_ADDED,producerInfo.mProducerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteProducerInfoById(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    int result = deleteProducerById(producerInfo.mProducerId);

    if (result == Result::OK)
      addEvent(EventType::PRODUCER_DELETED,producerId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteProducerInfoByName(T::SessionId sessionId,std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerByName(producerName,producerInfo) != 0)
      return Result::UNKNOWN_PRODUCER_NAME;

    int result = deleteProducerById(producerInfo.mProducerId);

    if (result == Result::OK)
      addEvent(EventType::PRODUCER_DELETED,producerInfo.mProducerId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getProducerById(producerId,producerInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getProducerInfoByName(T::SessionId sessionId,std::string producerName,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getProducerByName(producerName,producerInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getProducerList(producerInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(generationInfo.mProducerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfoList generationInfoList;
    getGenerationList(generationInfoList);

    T::GenerationInfo *info = generationInfoList.getGenerationInfoByName(generationInfo.mName);
    if (info != NULL)
      return Result::GENERATION_NAME_ALREADY_REGISTERED;

    redisReply *reply = (redisReply*)redisCommand(mContext,"INCR generationCounter");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    generationInfo.mGenerationId = (uint)reply->integer;
    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(mContext,"ZADD generations %u %s", generationInfo.mGenerationId,generationInfo.getCsv().c_str());
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    addEvent(EventType::GENERATION_ADDED,generationInfo.mGenerationId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteGenerationInfoById(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    deleteContentByGenerationId(generationInfo.mGenerationId);
    deleteFileListByGenerationId(generationInfo.mGenerationId,false);

    int result = deleteGenerationById(generationInfo.mGenerationId,false,false);

    if (result == Result::OK)
      addEvent(EventType::GENERATION_DELETED,generationId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteGenerationInfoByName(T::SessionId sessionId,std::string generationName)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationByName(generationName,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_NAME;

    deleteContentByGenerationId(generationInfo.mGenerationId);
    deleteFileListByGenerationId(generationInfo.mGenerationId,false);

    int result = deleteGenerationById(generationInfo.mGenerationId,false,false);

    if (result == Result::OK)
      addEvent(EventType::GENERATION_DELETED,generationInfo.mGenerationId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    deleteContentByProducerId(producerInfo.mProducerId);
    deleteFileListByProducerId(producerInfo.mProducerId,false);

    int result = deleteGenerationListByProducerId(producerId,false,false);

    if (result == Result::OK)
      addEvent(EventType::GENERATION_LIST_DELETED_BY_PRODUCER_ID,producerId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerByName(producerName,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    deleteContentByProducerId(producerInfo.mProducerId);
    deleteFileListByProducerId(producerInfo.mProducerId,false);

    int result = deleteGenerationListByProducerId(producerInfo.mProducerId,false,false);

    if (result == Result::OK)
      addEvent(EventType::GENERATION_LIST_DELETED_BY_PRODUCER_ID,producerInfo.mProducerId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getGenerationById(generationId,generationInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getGenerationInfoByName(T::SessionId sessionId,std::string generationName,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getGenerationByName(generationName,generationInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getGenerationList(generationInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    return getGenerationListByProducerId(producerId,generationInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerByName(producerName,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_NAME;

    return getGenerationListByProducerId(producerInfo.mProducerId,generationInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,T::GenerationStatus generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfoList generationInfoList;
    getGenerationListByProducerId(producerId,generationInfoList);

    T::GenerationInfo *info = generationInfoList.getLastGenerationInfoByProducerIdAndStatus(producerInfo.mProducerId,generationStatus);
    if (info == NULL)
      return Result::DATA_NOT_FOUND;

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




int RedisImplementation::_getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,std::string producerName,T::GenerationStatus generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerByName(producerName,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_NAME;

    T::GenerationInfoList generationInfoList;
    getGenerationListByProducerId(producerInfo.mProducerId,generationInfoList);

    T::GenerationInfo *info = generationInfoList.getLastGenerationInfoByProducerIdAndStatus(producerInfo.mProducerId,generationStatus);
    if (info == NULL)
      return Result::DATA_NOT_FOUND;

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




int RedisImplementation::_setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,T::GenerationStatus status)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    generationInfo.mStatus = status;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZREMRANGEBYSCORE generations %u %u",generationInfo.mGenerationId,generationInfo.mGenerationId);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(mContext,"ZADD generations %u %s", generationInfo.mGenerationId,generationInfo.getCsv().c_str());
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    addEvent(EventType::GENERATION_STATUS_CHANGED,generationId,(uint)status,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_setGenerationInfoStatusByName(T::SessionId sessionId,std::string generationName,T::GenerationStatus status)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationByName(generationName,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_NAME;

    generationInfo.mStatus = status;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZREMRANGEBYSCORE generations %u %u",generationInfo.mGenerationId,generationInfo.mGenerationId);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(mContext,"ZADD generations %u %s", generationInfo.mGenerationId,generationInfo.getCsv().c_str());
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    addEvent(EventType::GENERATION_STATUS_CHANGED,generationInfo.mGenerationId,(uint)status,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(fileInfo.mProducerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo generationInfo;
    if (getGenerationById(fileInfo.mGenerationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    if (producerInfo.mProducerId != generationInfo.mProducerId)
      return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;

    // ### Checking if the filename already exists in the database.

    uint fileId = getFileId(fileInfo.mName);
    if (fileId > 0)
    {
      // ### File with the same name already exists. Let's return
      // ### the current file-id.

      fileInfo.mFileId = fileId;

      // ### Adding an event to the event list.

      addEvent(EventType::FILE_UPDATED,fileInfo.mFileId,0,0,0);
    }
    else
    {
      // ### Generating a new file-id.

      redisReply *reply = (redisReply*)redisCommand(mContext,"INCR fileCounter");
      if (reply == NULL)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      fileInfo.mFileId = (uint)reply->integer;
      freeReplyObject(reply);

      // ### Adding the file information into the database.

      reply = (redisReply*)redisCommand(mContext,"ZADD files %u %s", fileInfo.mFileId,fileInfo.getCsv().c_str());
      if (reply == NULL)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      freeReplyObject(reply);

      // ### Adding the filename into the database. This needs to be added to another
      // ### table so that we can fetch it fast.

      addFilename(fileInfo.mName,fileInfo.mFileId);

      // ### Adding an event to the event list.

      addEvent(EventType::FILE_ADDED,fileInfo.mFileId,0,0,0);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(fileInfo.mProducerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo generationInfo;
    if (getGenerationById(fileInfo.mGenerationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    if (producerInfo.mProducerId != generationInfo.mProducerId)
      return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;

    // ### Checking if the filename already exists in the database.

    uint fileId = getFileId(fileInfo.mName);
    if (fileId > 0)
    {
      // ### File with the same name already exists. Let's return
      // ### the current file-id.

      fileInfo.mFileId = fileId;

      // ### Deleting old content information.

      deleteContentByFileId(fileInfo.mFileId);
    }
    else
    {
      // ### Generating a new file-id.

      redisReply *reply = (redisReply*)redisCommand(mContext,"INCR fileCounter");
      if (reply == NULL)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      fileInfo.mFileId = (uint)reply->integer;
      freeReplyObject(reply);

      // ### Adding the file information into the database.

      reply = (redisReply*)redisCommand(mContext,"ZADD files %u %s", fileInfo.mFileId,fileInfo.getCsv().c_str());
      if (reply == NULL)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      freeReplyObject(reply);

      // ### Adding the filename into the database. This needs to be added to another
      // ### table so that we can fetch it fast.

      addFilename(fileInfo.mName,fileInfo.mFileId);
    }


    // ### Adding the content information into the database.

    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      if (info != NULL)
      {
        // ### Making sure that content data matches the file data.

        info->mFileId = fileInfo.mFileId;
        info->mFileType = fileInfo.mFileType;
        info->mProducerId = fileInfo.mProducerId;
        info->mGenerationId = fileInfo.mGenerationId;
        info->mGroupFlags = fileInfo.mGroupFlags;
        info->mFlags = info->mFlags | (uint)T::FileInfoFlags::CONTENT_PREDEFINED;

        // ### Creating a key for the content.

        unsigned long long id = ((unsigned long long)info->mFileId << 32) + info->mMessageIndex;

        // ### Adding the content record into the database.

        redisReply *reply = (redisReply*)redisCommand(mContext,"ZADD content %llu %s",id,info->getCsv().c_str());
        if (reply == NULL)
        {
          closeConnection();
          return Result::PERMANENT_STORAGE_ERROR;
        }

        freeReplyObject(reply);
      }
    }

    // ### Adding an event to the event list.

    if (fileId > 0)
      addEvent(EventType::FILE_UPDATED,fileInfo.mFileId,0,0,0);
    else
      addEvent(EventType::FILE_ADDED,fileInfo.mFileId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteFileInfoById(T::SessionId sessionId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::FileInfo fileInfo;
    if (getFileById(fileId,fileInfo) != Result::OK)
      return Result::UNKNOWN_FILE_ID;

    deleteFilename(fileInfo.mName);
    int result = deleteFileById(fileId,true);

    if (result == Result::OK)
      addEvent(EventType::FILE_DELETED,fileId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteFileInfoByName(T::SessionId sessionId,std::string filename)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint fileId = getFileId(filename);
    if (fileId == 0)
      return Result::UNKNOWN_FILE_NAME;

    deleteFilename(filename);
    int result = deleteFileById(fileId,true);

    if (result == Result::OK)
      addEvent(EventType::FILE_DELETED,fileId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    int result = deleteFileListByGroupFlags(groupFlags,true);

    if (result == Result::OK)
      addEvent(EventType::FILE_LIST_DELETED_BY_GROUP_FLAGS,groupFlags,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    int result = deleteFileListByProducerId(producerInfo.mProducerId,true);

    if (result == Result::OK)
      addEvent(EventType::FILE_LIST_DELETED_BY_PRODUCER_ID,producerId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteFileInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerByName(producerName,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_NAME;

    int result = deleteFileListByProducerId(producerInfo.mProducerId,true);

    if (result == Result::OK)
      addEvent(EventType::FILE_LIST_DELETED_BY_PRODUCER_ID,producerInfo.mProducerId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    int result = deleteFileListByGenerationId(generationInfo.mGenerationId,true);

    if (result == Result::OK)
      addEvent(EventType::FILE_LIST_DELETED_BY_GENERATION_ID,generationId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationByName(generationName,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_NAME;

    int result = deleteFileListByGenerationId(generationInfo.mGenerationId,true);

    if (result == Result::OK)
      addEvent(EventType::FILE_LIST_DELETED_BY_GENERATION_ID,generationInfo.mGenerationId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getFileById(fileId,fileInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getFileInfoByName(T::SessionId sessionId,std::string filename,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint fileId = getFileId(filename);
    if (fileId == 0)
      return Result::DATA_NOT_FOUND;

    return getFileById(fileId,fileInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getFileInfoList(T::SessionId sessionId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getFileList(startFileId,maxRecords,fileInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    return getFileListByProducerId(producerId,startFileId,maxRecords,fileInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getFileInfoListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerByName(producerName,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_NAME;

    return getFileListByProducerId(producerInfo.mProducerId,startFileId,maxRecords,fileInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    return getFileListByGenerationId(generationId,startFileId,maxRecords,fileInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationByName(generationName,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_NAME;

    return getFileListByGenerationId(generationInfo.mGenerationId,startFileId,maxRecords,fileInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




int RedisImplementation::_getFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getFileListByGroupFlags(groupFlags,startFileId,maxRecords,fileInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    eventInfo.mEventId = addEvent(eventInfo.mType,eventInfo.mId1,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::EventId eventId = 0;

    redisReply *reply = (redisReply*)redisCommand(mContext,"GET eventCounter");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->str != NULL)
      eventId = (T::EventId)atoll(reply->str);

    freeReplyObject(reply);

    if (eventId == 0)
      return Result::DATA_NOT_FOUND;

    if (mLastEvent.mEventId == eventId)
    {
      eventInfo = mLastEvent;
      eventInfo.mServerTime = mStartTime;
      return Result::OK;
    }

    reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE events %llu %llu",eventId,eventId);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY  &&  reply->elements == 1)
    {
      mLastEvent.setCsv(reply->element[0]->str);
      freeReplyObject(reply);

      eventInfo = mLastEvent;
      eventInfo.mServerTime = mStartTime;
      return Result::OK;
    }

    freeReplyObject(reply);
    return Result::DATA_NOT_FOUND;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,uint maxRecords,T::EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE events %llu %llu LIMIT 0 %u",startEventId,0xFFFFFFFFFFFF,maxRecords);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        T::EventInfo *eventInfo = new T::EventInfo();
        eventInfo->setCsv(reply->element[t]->str);
        eventInfoList.addEventInfo(eventInfo);
      }
    }
    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(contentInfo.mProducerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo generationInfo;
    if (getGenerationById(contentInfo.mGenerationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    T::FileInfo fileInfo;
    if (getFileById(contentInfo.mFileId,fileInfo) != Result::OK)
      return Result::UNKNOWN_FILE_ID;

    if (producerInfo.mProducerId != generationInfo.mProducerId)
      return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;

    if (producerInfo.mProducerId != fileInfo.mProducerId)
      return Result::PRODUCER_AND_FILE_DO_NOT_MATCH;

    if (generationInfo.mGenerationId != fileInfo.mGenerationId)
      return Result::GENERATION_AND_FILE_DO_NOT_MATCH;


    unsigned long long id = ((unsigned long long)contentInfo.mFileId << 32) + contentInfo.mMessageIndex;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZADD content %llu %s",id,contentInfo.getCsv().c_str());
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    addEvent(EventType::CONTENT_ADDED,contentInfo.mFileId,contentInfo.mMessageIndex,0,contentInfo.mServerFlags);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      T::ContentInfo contentInfo;
      if (getContent(info->mFileId,info->mMessageIndex,contentInfo) == Result::OK)
      {
      }
      else
      {
        /*
        T::ProducerInfo producerInfo;
        if (getProducerById(info->mProducerId,producerInfo) != Result::OK)
          return Result::UNKNOWN_PRODUCER_ID;

        T::GenerationInfo generationInfo;
        if (getGenerationById(contentInfo.mGenerationId,generationInfo) != Result::OK)
          return Result::UNKNOWN_GENERATION_ID;

        T::FileInfo fileInfo;
        if (getFileById(contentInfo.mFileId,fileInfo) != Result::OK)
          return Result::UNKNOWN_FILE_ID;

        if (producerInfo.mProducerId != generationInfo.mProducerId)
          return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;

        if (producerInfo.mProducerId != fileInfo.mProducerId)
          return Result::PRODUCER_AND_FILE_DO_NOT_MATCH;

        if (generationInfo.mGenerationId != fileInfo.mGenerationId)
          return Result::GENERATION_AND_FILE_DO_NOT_MATCH;
        */
        unsigned long long id = ((unsigned long long)info->mFileId << 32) + info->mMessageIndex;

        redisReply *reply = (redisReply*)redisCommand(mContext,"ZADD content %llu %s",id,info->getCsv().c_str());
        if (reply == NULL)
        {
          closeConnection();
          return Result::PERMANENT_STORAGE_ERROR;
        }

        freeReplyObject(reply);

        addEvent(EventType::CONTENT_ADDED,info->mFileId,info->mMessageIndex,0,info->mServerFlags);
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ContentInfo contentInfo;
    if (getContent(fileId,messageIndex,contentInfo) != Result::OK)
      return Result::UNKNOWN_CONTENT;

    int result = deleteContent(fileId,messageIndex);

    if (result == Result::OK)
      addEvent(EventType::CONTENT_DELETED,fileId,messageIndex,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteContentListByFileId(T::SessionId sessionId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::FileInfo fileInfo;
    if (getFileById(fileId,fileInfo) != Result::OK)
      return Result::UNKNOWN_FILE_ID;

    int result = deleteContentByFileId(fileId);

    if (result == Result::OK)
      addEvent(EventType::CONTENT_LIST_DELETED_BY_FILE_ID,fileId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteContentListByFileName(T::SessionId sessionId,std::string filename)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint fileId = getFileId(filename);
    if (fileId == 0)
      return Result::UNKNOWN_FILE_NAME;

    int result = deleteContentByFileId(fileId);

    if (result == Result::OK)
      addEvent(EventType::CONTENT_LIST_DELETED_BY_FILE_ID,fileId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteContentListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    int result = deleteContentByGroupFlags(groupFlags);

    if (result == Result::OK)
      addEvent(EventType::CONTENT_LIST_DELETED_BY_GROUP_FLAGS,groupFlags,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteContentListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    int result = deleteContentByProducerId(producerId);

    if (result == Result::OK)
      addEvent(EventType::CONTENT_LIST_DELETED_BY_PRODUCER_ID,producerId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteContentListByProducerName(T::SessionId sessionId,std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerByName(producerName,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_NAME;

    int result = deleteContentByProducerId(producerInfo.mProducerId);

    if (result == Result::OK)
      addEvent(EventType::CONTENT_LIST_DELETED_BY_PRODUCER_ID,producerInfo.mProducerId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




int RedisImplementation::_deleteContentListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    int result = deleteContentByGenerationId(generationId);

    if (result == Result::OK)
      addEvent(EventType::CONTENT_LIST_DELETED_BY_GENERATION_ID,generationId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_deleteContentListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationByName(generationName,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_NAME;

    int result = deleteContentByGenerationId(generationInfo.mGenerationId);

    if (result == Result::OK)
      addEvent(EventType::CONTENT_LIST_DELETED_BY_GENERATION_ID,generationInfo.mGenerationId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_registerContentList(T::SessionId sessionId,uint serverId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long sf = 0;
    if (serverId > 0)
      sf = (1 << (serverId-1));

    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);

      T::ContentInfo contentInfo;
      if (getContent(info->mFileId,info->mMessageIndex,contentInfo) == Result::OK)
      {
        if (serverId > 0  &&  (info->mServerFlags & sf) == 0)
        {
          info->mServerFlags = contentInfo.mServerFlags | sf;
          setContent(*info);
          addEvent(EventType::CONTENT_REGISTERED,info->mFileId,info->mMessageIndex,serverId,info->mServerFlags);
        }
      }
      else
      {
        unsigned long long id = ((unsigned long long)info->mFileId << 32) + info->mMessageIndex;
        info->mServerFlags = sf;

        redisReply *reply = (redisReply*)redisCommand(mContext,"ZADD content %llu %s",id,info->getCsv().c_str());
        if (reply == NULL)
        {
          closeConnection();
          return Result::PERMANENT_STORAGE_ERROR;
        }

        freeReplyObject(reply);

        addEvent(EventType::CONTENT_ADDED,info->mFileId,info->mMessageIndex,serverId,info->mServerFlags);
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_registerContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long sf = (1 << (serverId-1));

    T::ContentInfoList contentInfoList;
    getContentByFileId(fileId,contentInfoList);
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      if ((info->mServerFlags & sf) == 0)
      {
        info->mServerFlags = info->mServerFlags | sf;
        setContent(*info);
        addEvent(EventType::CONTENT_REGISTERED,info->mFileId,info->mMessageIndex,serverId,info->mServerFlags);
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_unregisterContentList(T::SessionId sessionId,uint serverId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return unregisterContent(serverId);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_unregisterContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long sf = (1 << (serverId-1));
    unsigned long long nsf = ~sf;

    T::ContentInfoList contentInfoList;
    getContentByFileId(fileId,contentInfoList);
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      if ((info->mServerFlags & sf) != 0)
      {
        info->mServerFlags = info->mServerFlags & nsf;
        setContent(*info);
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getContent(fileId,messageIndex,contentInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getContent(startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getContentByFileId(fileId,contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentListByFileName(T::SessionId sessionId,std::string filename,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint fileId = getFileId(filename);
    if (fileId == 0)
      return Result::UNKNOWN_FILE_NAME;

    return getContentByFileId(fileId,contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getContentByGroupFlags(groupFlags,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    return getContentByProducerId(producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerByName(producerName,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_NAME;

    return getContentByProducerId(producerInfo.mProducerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentListByServerId(T::SessionId sessionId,uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getContentByServerId(serverId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    return getContentByGenerationId(generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationByName(generationName,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_NAME;

    return getContentByGenerationId(generationInfo.mGenerationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    return getContentByGenerationIdAndTimeRange(generationId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,std::string generationName,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationByName(generationName,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_NAME;

    return getContentByGenerationIdAndTimeRange(generationInfo.mGenerationId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}



// *****************************************************************************************************************************************
// *****************************************************************************************************************************************
// *****************************************************************************************************************************************



int RedisImplementation::deleteDataServerById(uint serverId)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unregisterContent(serverId);

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZREMRANGEBYSCORE dataServers %u %u",serverId,serverId);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getDataServerById(uint serverId,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE dataServers %u %u",serverId,serverId);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY  &&  reply->elements == 1)
    {
      serverInfo.setCsv(reply->element[0]->str);
      freeReplyObject(reply);
      return Result::OK;
    }

    freeReplyObject(reply);
    return Result::DATA_NOT_FOUND;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getDataServerList(T::ServerInfoList& serverInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGE dataServers 0 -1");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        T::ServerInfo *serverInfo = new T::ServerInfo();
        serverInfo->setCsv(reply->element[t]->str);
        serverInfoList.addServerInfo(serverInfo);
        //printf("%u) %s\n", t, reply->element[t]->str);
      }
    }
    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::deleteProducerById(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    deleteContentByProducerId(producerId);
    deleteFileListByProducerId(producerId,false);
    deleteGenerationListByProducerId(producerId,false,false);

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZREMRANGEBYSCORE producers %u %u",producerId,producerId);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getProducerById(uint producerId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE producers %u %u",producerId,producerId);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY  &&  reply->elements == 1)
    {
      producerInfo.setCsv(reply->element[0]->str);
      freeReplyObject(reply);
      return Result::OK;
    }

    freeReplyObject(reply);
    return Result::DATA_NOT_FOUND;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getProducerByName(std::string producerName,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGE producers 0 -1");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        T::ProducerInfo info;
        info.setCsv(reply->element[t]->str);

        if (strcasecmp(producerName.c_str(),info.mName.c_str()) == 0)
        {
          producerInfo = info;
          freeReplyObject(reply);
          return Result::OK;
        }
      }
    }
    freeReplyObject(reply);

    return Result::DATA_NOT_FOUND;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getProducerList(T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGE producers 0 -1");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        T::ProducerInfo *producerInfo = new T::ProducerInfo();
        producerInfo->setCsv(reply->element[t]->str);
        producerInfoList.addProducerInfo(producerInfo);
        //printf("%u) %s\n", t, reply->element[t]->str);
      }
    }
    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getGenerationById(uint generationId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE generations %u %u",generationId,generationId);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY  &&  reply->elements == 1)
    {
      generationInfo.setCsv(reply->element[0]->str);
      freeReplyObject(reply);
      return Result::OK;
    }

    freeReplyObject(reply);
    return Result::DATA_NOT_FOUND;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getGenerationByName(std::string generationName,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGE generations 0 -1");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        T::GenerationInfo info;
        info.setCsv(reply->element[t]->str);

        if (strcasecmp(generationName.c_str(),info.mName.c_str()) == 0)
        {
          generationInfo = info;
          freeReplyObject(reply);
          return Result::OK;
        }
      }
    }
    freeReplyObject(reply);

    return Result::DATA_NOT_FOUND;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getGenerationList(T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGE generations 0 -1");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        T::GenerationInfo *generationInfo = new T::GenerationInfo();
        generationInfo->setCsv(reply->element[t]->str);
        generationInfoList.addGenerationInfo(generationInfo);
        //printf("%u) %s\n", t, reply->element[t]->str);
      }
    }
    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getGenerationListByProducerId(uint producerId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGE generations 0 -1");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        T::GenerationInfo *generationInfo = new T::GenerationInfo();
        generationInfo->setCsv(reply->element[t]->str);
        if (generationInfo->mProducerId == producerId)
          generationInfoList.addGenerationInfo(generationInfo);
        else
          delete generationInfo;
      }
    }
    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::deleteGenerationById(uint generationId,bool deleteFiles,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    if (deleteContent)
      deleteContentByGenerationId(generationId);

    if (deleteFiles)
      deleteFileListByGenerationId(generationId,deleteContent);

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZREMRANGEBYSCORE generations %u %u",generationId,generationId);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::deleteGenerationListByProducerId(uint producerId,bool deleteFiles,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfoList generationInfoList;
    getGenerationListByProducerId(producerId,generationInfoList);

    uint len = generationInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::GenerationInfo *generationInfo = generationInfoList.getGenerationInfoByIndex(t);
      deleteGenerationById(generationInfo->mGenerationId,deleteFiles,deleteContent);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::deleteFileById(uint fileId,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    if (deleteContent)
      deleteContentByFileId(fileId);

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZREMRANGEBYSCORE files %u %u",fileId,fileId);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::deleteFileListByGroupFlags(uint groupFlags,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    if (deleteContent)
      deleteContentByGroupFlags(groupFlags);

    T::FileInfoList fileInfoList;
    getFileListByGroupFlags(groupFlags,0,0xFFFFFFFF,fileInfoList);
    uint len = fileInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::FileInfo *fileInfo = fileInfoList.getFileInfoByIndex(t);
      deleteFilename(fileInfo->mName);
      deleteFileById(fileInfo->mFileId,false);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::deleteFileListByGenerationId(uint generationId,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    if (deleteContent)
      deleteContentByGenerationId(generationId);

    T::FileInfoList fileInfoList;
    getFileListByGenerationId(generationId,0,0xFFFFFFFF,fileInfoList);
    uint len = fileInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::FileInfo *fileInfo = fileInfoList.getFileInfoByIndex(t);
      deleteFilename(fileInfo->mName);
      deleteFileById(fileInfo->mFileId,false);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getFileById(uint fileId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE files %u %u",fileId,fileId);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY  &&  reply->elements == 1)
    {
      fileInfo.setCsv(reply->element[0]->str);
      freeReplyObject(reply);
      return Result::OK;
    }

    freeReplyObject(reply);
    return Result::DATA_NOT_FOUND;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getFileList(uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE files %u %u LIMIT 0 %u",startFileId,0xFFFFFFFF,maxRecords);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        T::FileInfo *fileInfo = new T::FileInfo();
        fileInfo->setCsv(reply->element[t]->str);
        fileInfoList.addFileInfo(fileInfo);
      }
    }
    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getFileListByGenerationId(uint generationId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    while (true)
    {
      redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE files %u %u LIMIT 0 10000",startFileId,0xFFFFFFFF);
      if (reply == NULL)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      if (reply->type == REDIS_REPLY_ARRAY)
      {
        if (reply->elements == 0)
        {
          freeReplyObject(reply);
          return Result::OK;
        }

        for (uint t = 0; t < reply->elements; t++)
        {
          T::FileInfo *fileInfo = new T::FileInfo();
          fileInfo->setCsv(reply->element[t]->str);
          startFileId = fileInfo->mFileId + 1;
          if (fileInfo->mGenerationId == generationId)
          {
            fileInfoList.addFileInfo(fileInfo);
            if (fileInfoList.getLength() == maxRecords)
            {
              freeReplyObject(reply);
              return Result::OK;
            }
          }
          else
          {
            delete fileInfo;
          }
        }
      }
      freeReplyObject(reply);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getFileListByGroupFlags(uint groupFlags,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    while (true)
    {
      redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE files %u %u LIMIT 0 10000",startFileId,0xFFFFFFFF);
      if (reply == NULL)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      if (reply->type == REDIS_REPLY_ARRAY)
      {
        if (reply->elements == 0)
        {
          freeReplyObject(reply);
          return Result::OK;
        }

        for (uint t = 0; t < reply->elements; t++)
        {
          T::FileInfo *fileInfo = new T::FileInfo();
          fileInfo->setCsv(reply->element[t]->str);
          startFileId = fileInfo->mFileId + 1;
          if ((fileInfo->mGroupFlags & groupFlags) != 0)
          {
            fileInfoList.addFileInfo(fileInfo);
            if (fileInfoList.getLength() == maxRecords)
            {
              freeReplyObject(reply);
              return Result::OK;
            }
          }
          else
          {
            delete fileInfo;
          }
        }
      }
      freeReplyObject(reply);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::deleteFileListByProducerId(uint producerId,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    if (deleteContent)
      deleteContentByProducerId(producerId);

    T::FileInfoList fileInfoList;
    getFileListByProducerId(producerId,0,0xFFFFFFFF,fileInfoList);
    uint len = fileInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::FileInfo *fileInfo = fileInfoList.getFileInfoByIndex(t);
      deleteFilename(fileInfo->mName);
      deleteFileById(fileInfo->mFileId,false);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getFileListByProducerId(uint producerId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    while (true)
    {
      redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE files %u %u LIMIT 0 10000",startFileId,0xFFFFFFFF);
      if (reply == NULL)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      if (reply->type == REDIS_REPLY_ARRAY)
      {
        if (reply->elements == 0)
        {
          freeReplyObject(reply);
          return Result::OK;
        }

        for (uint t = 0; t < reply->elements; t++)
        {
          T::FileInfo *fileInfo = new T::FileInfo();
          fileInfo->setCsv(reply->element[t]->str);
          startFileId = fileInfo->mFileId + 1;
          if (fileInfo->mProducerId == producerId)
          {
            fileInfoList.addFileInfo(fileInfo);
            if (fileInfoList.getLength() == maxRecords)
            {
              freeReplyObject(reply);
              return Result::OK;
            }
          }
          else
          {
            delete fileInfo;
          }
        }
      }
      freeReplyObject(reply);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoThreadLock lock(&mThreadLock);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getContentByParameterIdAndTimeRange(parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoThreadLock lock(&mThreadLock);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    return getContentByParameterIdAndGeneration(generationInfo.mGenerationId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentListByParameterAndGenerationName(T::SessionId sessionId,std::string generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoThreadLock lock(&mThreadLock);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationByName(generationName,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_NAME;

    return getContentByParameterIdAndGeneration(generationInfo.mGenerationId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoThreadLock lock(&mThreadLock);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    return getContentByParameterIdAndProducer(producerInfo.mProducerId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::_getContentListByParameterAndProducerName(T::SessionId sessionId,std::string producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoThreadLock lock(&mThreadLock);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerByName(producerName,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_NAME;

    return getContentByParameterIdAndProducer(producerInfo.mProducerId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::deleteContent(uint fileId,uint messageIndex)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long id = ((unsigned long long)fileId << 32) + messageIndex;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZREMRANGEBYSCORE content %llu %llu",id,id);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::deleteContentByFileId(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ContentInfoList contentInfoList;
    getContentByFileId(fileId,contentInfoList);
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
      deleteContent(contentInfo->mFileId,contentInfo->mMessageIndex);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::deleteContentByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ContentInfoList contentInfoList;
    getContentByProducerId(producerId,0,0,0xFFFFFFFF,contentInfoList);
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
      deleteContent(contentInfo->mFileId,contentInfo->mMessageIndex);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::deleteContentByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ContentInfoList contentInfoList;
    getContentByGenerationId(generationId,0,0,0xFFFFFFFF,contentInfoList);
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
      deleteContent(contentInfo->mFileId,contentInfo->mMessageIndex);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::deleteContentByGroupFlags(uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ContentInfoList contentInfoList;
    getContentByGroupFlags(groupFlags,0,0,0xFFFFFFFF,contentInfoList);
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
      deleteContent(contentInfo->mFileId,contentInfo->mMessageIndex);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::setContent(T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    //contentInfo.print(std::cout,0,0);
    unsigned long long id = ((unsigned long long)contentInfo.mFileId << 32) + contentInfo.mMessageIndex;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZREMRANGEBYSCORE content %llu %llu",id,id);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(mContext,"ZADD content %llu %s",id,contentInfo.getCsv().c_str());
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getContent(uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long id = ((unsigned long long)fileId << 32) + messageIndex;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE content %llu %llu",id,id);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY  &&  reply->elements == 1)
    {
      contentInfo.setCsv(reply->element[0]->str);
      freeReplyObject(reply);
      return Result::OK;
    }

    freeReplyObject(reply);
    return Result::DATA_NOT_FOUND;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getContent(uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long startId = ((unsigned long long)startFileId << 32) + startMessageIndex;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE content %llu %llu LIMIT 0 %u",startId,0xFFFFFFFFFFFFFFFF,maxRecords);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        T::ContentInfo *contentInfo = new T::ContentInfo();
        contentInfo->setCsv(reply->element[t]->str);

        contentInfoList.addContentInfo(contentInfo);
      }
    }
    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getContentByGenerationId(uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    while (true)
    {
      unsigned long long startId = ((unsigned long long)startFileId << 32) + startMessageIndex;

      redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE content %llu %llu LIMIT 0 10000",startId,0xFFFFFFFFFFFFFFFF);
      if (reply == NULL)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      if (reply->elements == 0)
      {
        freeReplyObject(reply);
        return Result::OK;
      }

      if (reply->type == REDIS_REPLY_ARRAY)
      {
        for (uint t = 0; t < reply->elements; t++)
        {
          T::ContentInfo *contentInfo = new T::ContentInfo();
          contentInfo->setCsv(reply->element[t]->str);

          startFileId = contentInfo->mFileId;
          startMessageIndex = contentInfo->mMessageIndex + 1;

          if (contentInfo->mGenerationId == generationId)
          {
            contentInfoList.addContentInfo(contentInfo);
            if (contentInfoList.getLength() == maxRecords)
            {
              freeReplyObject(reply);
              return Result::OK;
            }
          }
          else
          {
            delete contentInfo;
          }
        }
      }
      freeReplyObject(reply);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getContentByGroupFlags(uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    while (true)
    {
      unsigned long long startId = ((unsigned long long)startFileId << 32) + startMessageIndex;

      redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE content %llu %llu LIMIT 0 10000",startId,0xFFFFFFFFFFFFFFFF);
      if (reply == NULL)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      if (reply->elements == 0)
      {
        freeReplyObject(reply);
        return Result::OK;
      }

      if (reply->type == REDIS_REPLY_ARRAY)
      {
        for (uint t = 0; t < reply->elements; t++)
        {
          T::ContentInfo *contentInfo = new T::ContentInfo();
          contentInfo->setCsv(reply->element[t]->str);

          startFileId = contentInfo->mFileId;
          startMessageIndex = contentInfo->mMessageIndex + 1;

          if ((contentInfo->mGroupFlags & groupFlags) != 0)
          {
            contentInfoList.addContentInfo(contentInfo);
            if (contentInfoList.getLength() == maxRecords)
            {
              freeReplyObject(reply);
              return Result::OK;
            }
          }
          else
          {
            delete contentInfo;
          }
        }
      }
      freeReplyObject(reply);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getContentByParameterIdAndTimeRange(T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE content %llu %llu",0,0xFFFFFFFFFFFFFFFF);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(reply->element[t]->str);

        if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
        {
          if (info->hasKey(parameterKeyType,parameterKey) &&  info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              contentInfoList.addContentInfo(info);
              info = NULL;
            }
          }
        }

        if (info != NULL)
          delete info;
      }
    }

    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getContentByParameterIdAndGeneration(uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE content %llu %llu",0,0xFFFFFFFFFFFFFFFF);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(reply->element[t]->str);

        if (info->mGenerationId == generationId)
        {
          if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
          {
            if (info->hasKey(parameterKeyType,parameterKey) &&  info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
            {
              if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                  (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                  (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                  (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                  (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
              {
                contentInfoList.addContentInfo(info);
                info = NULL;
              }
            }
          }
        }

        if (info != NULL)
          delete info;
      }
    }

    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




int RedisImplementation::getContentByParameterIdAndProducer(uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE content %llu %llu",0,0xFFFFFFFFFFFFFFFF);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(reply->element[t]->str);

        if (info->mProducerId == producerId)
        {
          if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
          {
            if (info->hasKey(parameterKeyType,parameterKey) &&  info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
            {
              if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                  (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                  (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                  (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                  (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
              {
                contentInfoList.addContentInfo(info);
                info = NULL;
              }
            }
          }
        }

        if (info != NULL)
          delete info;
      }
    }

    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




int RedisImplementation::getContentByGenerationIdAndTimeRange(uint generationId,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE content %llu %llu",0,0xFFFFFFFFFFFFFFFF);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        T::ContentInfo *contentInfo = new T::ContentInfo();
        contentInfo->setCsv(reply->element[t]->str);

        if (contentInfo->mGenerationId == generationId  &&  contentInfo->mStartTime >= startTime  &&  contentInfo->mStartTime <= endTime)
          contentInfoList.addContentInfo(contentInfo);
        else
          delete contentInfo;
      }
    }

    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getContentByProducerId(uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    while (true)
    {
      unsigned long long startId = ((unsigned long long)startFileId << 32) + startMessageIndex;

      redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE content %llu %llu LIMIT 0 10000",startId,0xFFFFFFFFFFFFFFFF);
      if (reply == NULL)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      if (reply->elements == 0)
      {
        freeReplyObject(reply);
        return Result::OK;
      }

      if (reply->type == REDIS_REPLY_ARRAY)
      {
        for (uint t = 0; t < reply->elements; t++)
        {
          T::ContentInfo *contentInfo = new T::ContentInfo();
          contentInfo->setCsv(reply->element[t]->str);

          startFileId = contentInfo->mFileId;
          startMessageIndex = contentInfo->mMessageIndex + 1;

          if (contentInfo->mProducerId == producerId)
          {
            contentInfoList.addContentInfo(contentInfo);
            if (contentInfoList.getLength() == maxRecords)
            {
              freeReplyObject(reply);
              return Result::OK;
            }
          }
          else
          {
            delete contentInfo;
          }
        }
      }
      freeReplyObject(reply);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getContentByServerId(uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long sf = 1 << (serverId-1);
    while (true)
    {
      unsigned long long startId = ((unsigned long long)startFileId << 32) + startMessageIndex;

      redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE content %llu %llu LIMIT 0 10000",startId,0xFFFFFFFFFFFFFFFF);
      if (reply == NULL)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      if (reply->elements == 0)
      {
        freeReplyObject(reply);
        return Result::OK;
      }

      if (reply->type == REDIS_REPLY_ARRAY)
      {
        for (uint t = 0; t < reply->elements; t++)
        {
          T::ContentInfo *contentInfo = new T::ContentInfo();
          contentInfo->setCsv(reply->element[t]->str);

          startFileId = contentInfo->mFileId;
          startMessageIndex = contentInfo->mMessageIndex + 1;

          if ((contentInfo->mServerFlags & sf) != 0)
          {
            contentInfoList.addContentInfo(contentInfo);
            if (contentInfoList.getLength() == maxRecords)
            {
              freeReplyObject(reply);
              return Result::OK;
            }
          }
          else
          {
            delete contentInfo;
          }
        }
      }
      freeReplyObject(reply);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::getContentByFileId(uint fileId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long startId = ((unsigned long long)fileId << 32);
    unsigned long long endId = ((unsigned long long)fileId << 32) + 0xFFFFFFFF;

    redisReply *reply = (redisReply*)redisCommand(mContext,"ZRANGEBYSCORE content %llu %llu",startId,endId);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        T::ContentInfo *contentInfo = new T::ContentInfo();
        contentInfo->setCsv(reply->element[t]->str);

        contentInfoList.addContentInfo(contentInfo);
      }
    }
    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::unregisterContent(uint serverId)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long sf = (1 << (serverId-1));
    unsigned long long nsf = ~sf;

    T::ContentInfoList contentInfoList;
    getContentByServerId(serverId,0,0,0xFFFFFFFF,contentInfoList);
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      if ((info->mServerFlags & sf) != 0)
      {
        info->mServerFlags = info->mServerFlags & nsf;
        setContent(*info);
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void RedisImplementation::resetContentRegistrations()
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList contentInfoList;
    getContent(0,0,0xFFFFFFFF,contentInfoList);
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      if (info->mServerFlags != 0)
      {
        info->mServerFlags = 0;
        setContent(*info);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





T::EventId RedisImplementation::addEvent(EventType eventType,uint id1,uint id2,uint id3,unsigned long long flags)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"INCR eventCounter");
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    T::EventId eventId = (T::EventId)reply->integer;
    freeReplyObject(reply);

    T::EventInfo eventInfo(mStartTime,eventId,eventType,id1,id2,id3,flags);

    reply = (redisReply*)redisCommand(mContext,"ZADD events %llu %s", eventId,eventInfo.getCsv().c_str());
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return eventId;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




int RedisImplementation::addFilename(std::string filename,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"HSET filenames %s %u", filename.c_str(),fileId);
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





uint RedisImplementation::getFileId(std::string filename)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return 0;

    redisReply *reply = (redisReply*)redisCommand(mContext,"HGET filenames %s", filename.c_str());
    if (reply == NULL)
    {
      closeConnection();
      return 0;
    }

    uint id = 0;
    if (reply->str != NULL)
      id = (uint)atoll(reply->str);

    freeReplyObject(reply);
    return id;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int RedisImplementation::deleteFilename(std::string filename)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = (redisReply*)redisCommand(mContext,"HDEL filenames %s", filename.c_str());
    if (reply == NULL)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}



}
}
