#include "RedisImplementation.h"
#include <macgyver/Exception.h>
#include <grid-files/common/AutoThreadLock.h>
#include <grid-files/common/ShowFunction.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/identification/GridDef.h>
#include <boost/functional/hash.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/permissions.hpp>
#include <boost/thread/thread_time.hpp>
#include <fstream>
#include <iostream>
#include <cstdio>

using namespace boost::interprocess;


#define FUNCTION_TRACE FUNCTION_TRACE_OFF


#define FUNCTION_NAME static_cast<const char*>(__PRETTY_FUNCTION__)


namespace SmartMet
{
namespace ContentServer
{

class RedisProcessLock
{
  public:

    RedisProcessLock(const char *function,uint line,RedisImplementation *redisImplementation)
    {
      FUNCTION_TRACE
      mRedisImplementation = redisImplementation;
      mRedisImplementation->lock(function,line,mKey,180,true);
    }


    RedisProcessLock(const char *function,uint line,RedisImplementation *redisImplementation,uint waitTimeInSec,bool resetLock)
    {
      FUNCTION_TRACE
      mRedisImplementation = redisImplementation;
      mRedisImplementation->lock(function,line,mKey,waitTimeInSec,resetLock);
    }


    virtual ~RedisProcessLock()
    {
      FUNCTION_TRACE
      if (mKey != 0)
        mRedisImplementation->unlock(mKey);
    }

  protected:

    RedisImplementation *mRedisImplementation;
    ulonglong mKey;
};




unsigned long long getContentKey(uint fileId,uint messageIndex)
{
  unsigned long long key = fileId;
  key = (key << 24) + messageIndex;
  return key;
}




unsigned long long getGeometryKey(uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId)
{
  unsigned long long key = generationId;
  unsigned long long g = geometryId;
  key = (key << 32) + (g << 8) + levelId;
  return key;
}




RedisImplementation::RedisImplementation()
{
  FUNCTION_TRACE
  try
  {
    mImplementationType = Implementation::Redis;
    mContext = nullptr;
    mStartTime = time(nullptr);
    mRedisPort = 0;
    mRedisSecondaryPort = 0;
    mLine = 0;
    mShutdownRequested = false;
    mDatabaseLockEnabled = false;
    mReloadRequired = false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





RedisImplementation::~RedisImplementation()
{
  FUNCTION_TRACE
  try
  {
    if (mContext != nullptr)
      redisFree(mContext);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}




void RedisImplementation::lock(const char *function,uint line,ulonglong& key,uint waitTimeInSec,bool resetLock)
{
  FUNCTION_TRACE
  try
  {
    key = 0;
    if (!mDatabaseLockEnabled)
    {
      // Thread level locking
      key = 100;
      mThreadLock.lock();
      return;
    }

    AutoThreadLock lock(&mThreadLock);

    mFunction = function;
    mLine = line;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"INCR %slockRequestCounter",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      throw Fmi::Exception(BCP,"Cannot set the lock request counter!");
    }

    ulonglong lockRequestCounter = reply->integer;
    ulonglong lockReleaseCounter = 0;
    freeReplyObject(reply);

    time_t startTime = time(nullptr);

    while (true)
    {
      if (mShutdownRequested)
      {
        key = lockRequestCounter;
        return;
      }

      redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"GET %slockReleaseCounter",mTablePrefix.c_str()));
      if (reply == nullptr)
      {
        closeConnection();
        throw Fmi::Exception(BCP,"Cannot get the lock release counter!");
      }
      lockReleaseCounter = reply->integer;
      if (reply->str != nullptr)
        lockReleaseCounter = toInt64(reply->str);
      freeReplyObject(reply);

      if ((lockReleaseCounter+1) == lockRequestCounter)
      {
        key = lockRequestCounter;
        return;
      }

      time_usleep(0,100);

      time_t currentTime = time(nullptr);
      if ((currentTime-startTime) >= waitTimeInSec || lockReleaseCounter >= lockRequestCounter)
      {
        if (resetLock)
        {
          redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"SET %slockReleaseCounter %llu",mTablePrefix.c_str(),lockRequestCounter-1));
          if (reply == nullptr)
          {
            closeConnection();
            throw Fmi::Exception(BCP,"Cannot set the lock release counter!");
          }

          freeReplyObject(reply);
        }
        else
        {
          return;
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void RedisImplementation::unlock(ulonglong key)
{
  FUNCTION_TRACE
  try
  {
    if (!mDatabaseLockEnabled)
    {
      // Thread level locking
      mThreadLock.unlock();
      return;
    }

    AutoThreadLock lock(&mThreadLock);

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"GET %slockReleaseCounter",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      throw Fmi::Exception(BCP,"Cannot get the lock release counter!");
    }

    ulonglong lockReleaseCounter = reply->integer;
    if (reply->str != nullptr)
      lockReleaseCounter = toInt64(reply->str);

    if (lockReleaseCounter < key)
    {
      redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"SET %slockReleaseCounter %llu",mTablePrefix.c_str(),key));
      if (reply == nullptr)
      {
        closeConnection();
        throw Fmi::Exception(BCP,"Cannot set the lock release counter!");
      }
      freeReplyObject(reply);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void RedisImplementation::init(const char *redisAddress,int redisPort,const char *tablePrefix)
{
  FUNCTION_TRACE
  try
  {
    mRedisAddress = redisAddress;
    mRedisPort = redisPort;
    mTablePrefix = tablePrefix;
    mRedisSecondaryPort = 0;
    mDatabaseLockEnabled = false;

    openConnection();

    if (mContext == nullptr)
      throw Fmi::Exception(BCP,"Cannot connect to Redis!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void RedisImplementation::init(const char *redisAddress,int redisPort,const char *tablePrefix,bool databaseLockEnabled)
{
  FUNCTION_TRACE
  try
  {
    mRedisAddress = redisAddress;
    mRedisPort = redisPort;
    mTablePrefix = tablePrefix;
    mRedisSecondaryPort = 0;
    mDatabaseLockEnabled = databaseLockEnabled;

    openConnection();

    if (mContext == nullptr)
      throw Fmi::Exception(BCP,"Cannot connect to Redis!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void RedisImplementation::init(const char *redisAddress,int redisPort,const char *tablePrefix,const char *redisSecondaryAddress,int redisSecondaryPort,bool databaseLockEnabled,bool reloadRequired)
{
  FUNCTION_TRACE
  try
  {
    mRedisAddress = redisAddress;
    mRedisPort = redisPort;
    mTablePrefix = tablePrefix;
    mRedisSecondaryAddress = redisSecondaryAddress;
    mRedisSecondaryPort = redisSecondaryPort;
    mDatabaseLockEnabled = databaseLockEnabled;
    mReloadRequired = reloadRequired;

    openConnection();

    if (mContext == nullptr)
      throw Fmi::Exception(BCP,"Cannot connect to Redis!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::openConnection()
{
  FUNCTION_TRACE
  try
  {
    if (mContext != nullptr)
      return Result::OK;

    struct timeval timeout = { 3, 0 }; // 3 seconds

    redisContext *context = nullptr;

    context = redisConnectWithTimeout(mRedisAddress.c_str(), mRedisPort, timeout);
    if (context == nullptr || context->err)
    {
      if (context)
      {
        printf("Redis primary connection error (%s:%d): %s\n",mRedisAddress.c_str(), mRedisPort,context->errstr);
        redisFree(context);
        context = nullptr;
      }

      if (mRedisSecondaryPort > 0)
      {
        context = redisConnectWithTimeout(mRedisSecondaryAddress.c_str(), mRedisSecondaryPort, timeout);
        if (context == nullptr || context->err)
        {
          if (context)
          {
            printf("Redis secondary connection error (%s:%d): %s\n",mRedisSecondaryAddress.c_str(), mRedisSecondaryPort,context->errstr);
            redisFree(context);
            context = nullptr;
            return Result::PERMANENT_STORAGE_ERROR;
          }
          else
          {
            printf("Redis secondary connection error (%s:%d): can't allocate redis context\n",mRedisSecondaryAddress.c_str(), mRedisSecondaryPort);
            return Result::PERMANENT_STORAGE_ERROR;
          }
        }
        else
        {
          mSourceInfo = "Redis:" + mRedisSecondaryAddress + ":" + std::to_string(mRedisSecondaryPort);
        }
      }
    }
    else
    {
      mSourceInfo = "Redis:" + mRedisAddress + ":" + std::to_string(mRedisPort);
    }

    if (context == NULL)
    {
      mSourceInfo = "Redis:CONNECTION_ERROR";
      return Result::PERMANENT_STORAGE_ERROR;
    }

    while (!mShutdownRequested)
    {
      redisReply *reply = static_cast<redisReply*>(redisCommand(context,"PING"));
      if (reply != nullptr)
      {
        if (strcasecmp(reply->str,"PONG") == 0)
        {
          mContext = context;
          freeReplyObject(reply);
          if (mStartTime == 0 || mReloadRequired)
            mStartTime = time(nullptr);

          return Result::OK;
        }
        freeReplyObject(reply);
      }
      sleep(1);
    }

    return Result::PERMANENT_STORAGE_ERROR;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void RedisImplementation::closeConnection()
{
  FUNCTION_TRACE
  try
  {
    if (mContext != nullptr)
    {
      redisFree(mContext);
      mContext = nullptr;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void RedisImplementation::shutdown()
{
  FUNCTION_TRACE
  try
  {
    mShutdownRequested = true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool RedisImplementation::isConnectionValid()
{
  FUNCTION_TRACE
  try
  {
    if (mContext == nullptr  &&  openConnection() != Result::OK)
      return false;

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_clear(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"DEL %sproducerCounter",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"DEL %sproducers",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"DEL %sgenerationCounter",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"DEL %sgenerations",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"DEL %sgeometries",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);
    reply = static_cast<redisReply*>(redisCommand(mContext,"DEL %sfileCounter",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"DEL %sfiles",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"DEL %sfilenames",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);


    reply = static_cast<redisReply*>(redisCommand(mContext,"DEL %scontent",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"DEL %sdataServers",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"DEL %scontentServers",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"DEL %sevents",mTablePrefix.c_str()));
    if (reply == nullptr)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfoList producerInfoList;
    getProducerList(producerInfoList);

    T::ProducerInfo *info = producerInfoList.getProducerInfoByName(producerInfo.mName);
    if (info != nullptr)
      return Result::PRODUCER_NAME_ALREADY_REGISTERED;


    redisReply *reply = nullptr;

    if (producerInfo.mProducerId == 0)
    {
      reply = static_cast<redisReply*>(redisCommand(mContext,"INCR %sproducerCounter",mTablePrefix.c_str()));
      if (reply == nullptr)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      producerInfo.mProducerId = reply->integer;
      freeReplyObject(reply);
    }
    else
    {
      T::ProducerInfo *info = producerInfoList.getProducerInfoById(producerInfo.mProducerId);
      if (info != nullptr)
        return Result::PRODUCER_ID_ALREADY_REGISTERED;

      reply = static_cast<redisReply*>(redisCommand(mContext,"GET %sproducerCounter",mTablePrefix.c_str()));
      if (reply == nullptr)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      uint id = 0;
      if (reply != nullptr  &&  reply->str != nullptr)
        id = toInt64(reply->str);

      freeReplyObject(reply);

      if (id < producerInfo.mProducerId)
      {
        reply = static_cast<redisReply*>(redisCommand(mContext,"SET %sproducerCounter %u",mTablePrefix.c_str(),producerInfo.mProducerId));
        if (reply == nullptr)
        {
          closeConnection();
          return Result::PERMANENT_STORAGE_ERROR;
        }

        freeReplyObject(reply);
      }
    }

    reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %sproducers %u %s",mTablePrefix.c_str(),producerInfo.mProducerId,producerInfo.getCsv().c_str()));
    if (reply == nullptr)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_setProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfoList producerInfoList;
    getProducerList(producerInfoList);

    T::ProducerInfo *info = producerInfoList.getProducerInfoByName(producerInfo.mName);
    if (info != nullptr  &&  info->mProducerId != producerInfo.mProducerId)
      return Result::PRODUCER_NAME_ALREADY_REGISTERED;


    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZREMRANGEBYSCORE %sproducers %u %u",mTablePrefix.c_str(),producerInfo.mProducerId,producerInfo.mProducerId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %sproducers %u %s",mTablePrefix.c_str(),producerInfo.mProducerId,producerInfo.getCsv().c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    addEvent(EventType::PRODUCER_UPDATED,producerInfo.mProducerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteProducerInfoById(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;


    int result = deleteProducerById(producerInfo.mProducerId,true,true,true,true);

    if (result == Result::OK)
      addEvent(EventType::PRODUCER_DELETED,producerId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteProducerInfoByName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerByName(producerName,producerInfo) != 0)
      return Result::UNKNOWN_PRODUCER_NAME;


    int result = deleteProducerById(producerInfo.mProducerId,true,true,true,true);

    if (result == Result::OK)
      addEvent(EventType::PRODUCER_DELETED,producerInfo.mProducerId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    deleteContentBySourceId(sourceId);
    deleteFileListBySourceId(sourceId,false);
    deleteGeometryListBySourceId(sourceId,false,false);
    deleteGenerationListBySourceId(sourceId,false,false,false);

    int result = deleteProducerListBySourceId(sourceId,false,false,false,false);

    if (result == Result::OK)
      addEvent(EventType::PRODUCER_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getProducerById(producerId,producerInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getProducerInfoByName(T::SessionId sessionId,const std::string& producerName,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getProducerByName(producerName,producerInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    producerInfoList.clear();

    return getProducerList(producerInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    producerInfoList.clear();

    T::ContentInfoList contentInfoList;
    getContentByParameterId(parameterKeyType,parameterKey,contentInfoList);

    std::set<uint> producerIdList;
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      if (producerIdList.find(info->mProducerId) == producerIdList.end())
      {
        producerIdList.insert(info->mProducerId);

        T::ProducerInfo *producerInfo = new T::ProducerInfo;
        getProducerById(info->mProducerId,*producerInfo);
        producerInfoList.addProducerInfo(producerInfo);
      }
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    producerInfoList.clear();

    return getProducerListBySourceId(sourceId,producerInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getProducerInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    count = 0;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZCOUNT %sproducers 0 %llu",mTablePrefix.c_str(),0xFFFFFFFF));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->str != nullptr)
      count = toInt64(reply->str);
    else
      count = reply->integer;

    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    list.clear();

    T::ProducerInfoList producerInfoList;
    getProducerList(producerInfoList);

    uint pLen = producerInfoList.getLength();

    for (uint p=0; p<pLen; p++)
    {
      T::ProducerInfo *producerInfo = producerInfoList.getProducerInfoByIndex(p);

      std::set<uint> geometryIdList;

      T::ContentInfoList contentInfoList;
      getContentByProducerId(producerInfo->mProducerId,0,0,10000000,contentInfoList);

      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
        if (producerInfo->mProducerId == contentInfo->mProducerId  &&  geometryIdList.find(contentInfo->mGeometryId) == geometryIdList.end())
        {
          char tmp[100];
          sprintf(tmp,"%s;%u",producerInfo->mName.c_str(),contentInfo->mGeometryId);
          list.insert(std::string(tmp));
          geometryIdList.insert(contentInfo->mGeometryId);
        }
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    list.clear();

    T::ContentInfoList cList;
    getContent(0,0,20000000,cList);
    cList.sort(T::ContentInfo::ComparisonMethod::producer_file_message);

    std::set<std::size_t> tmpList;

    uint producerId = 0;
    T::ProducerInfo *producerInfo = nullptr;
    T::ProducerInfo pInfo;

    uint len = cList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = cList.getContentInfoByIndex(t);
      std::string sourceParamKey;
      std::string targetParamKey;
      T::ParamLevelId paramLevelId = contentInfo->mFmiParameterLevelId;

      if (contentInfo->mProducerId != producerId)
      {
        if (getProducerById(contentInfo->mProducerId,pInfo) == 0)
        {
          producerInfo = &pInfo;
          producerId = pInfo.mProducerId;
        }
        else
        {
          producerInfo = nullptr;
        }
      }

      if (producerInfo != nullptr  &&  producerInfo->mProducerId == contentInfo->mProducerId)
      {
        auto fmiId = contentInfo->mFmiParameterId;

        switch (sourceParameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            sourceParamKey = std::to_string(contentInfo->mFmiParameterId);
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            sourceParamKey = contentInfo->getFmiParameterName();
            break;

          case T::ParamKeyTypeValue::GRIB_ID:
          {
            Identification::FmiParameterId_grib def;
            if (Identification::gridDef.getGribParameterMappingByFmiId(fmiId,def))
              sourceParamKey = std::to_string(def.mGribParameterId);
          }
          break;

          case T::ParamKeyTypeValue::NEWBASE_ID:
          {
            Identification::FmiParameterId_newbase def;
            if (Identification::gridDef.getNewbaseParameterMappingByFmiId(fmiId,def))
              sourceParamKey = std::to_string(def.mNewbaseParameterId);
          }
          break;

          case T::ParamKeyTypeValue::NEWBASE_NAME:
          {
            Identification::NewbaseParameterDef def;
            if (Identification::gridDef.getNewbaseParameterDefByFmiId(fmiId,def))
              sourceParamKey = def.mParameterName;
          }
          break;

          case T::ParamKeyTypeValue::NETCDF_NAME:
          {
            Identification::FmiParameterId_netCdf def;
            if (Identification::gridDef.getNetCdfParameterMappingByFmiId(fmiId,def))
              sourceParamKey = def.mNetCdfParameterName;
          }
          break;

          default:
            break;
        }


        switch (targetParameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            targetParamKey = std::to_string(contentInfo->mFmiParameterId);
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            targetParamKey = contentInfo->getFmiParameterName();
            break;

          case T::ParamKeyTypeValue::GRIB_ID:
          {
            Identification::FmiParameterId_grib def;
            if (Identification::gridDef.getGribParameterMappingByFmiId(fmiId,def))
              targetParamKey = std::to_string(def.mGribParameterId);
          }
          break;

          case T::ParamKeyTypeValue::NEWBASE_ID:
          {
            Identification::FmiParameterId_newbase def;
            if (Identification::gridDef.getNewbaseParameterMappingByFmiId(fmiId,def))
              targetParamKey = std::to_string(def.mNewbaseParameterId);
          }
          break;

          case T::ParamKeyTypeValue::NEWBASE_NAME:
          {
            Identification::NewbaseParameterDef def;
            if (Identification::gridDef.getNewbaseParameterDefByFmiId(fmiId,def))
              targetParamKey = def.mParameterName;
          }
          break;

          case T::ParamKeyTypeValue::NETCDF_NAME:
          {
            Identification::FmiParameterId_netCdf def;
            if (Identification::gridDef.getNetCdfParameterMappingByFmiId(fmiId,def))
              targetParamKey = def.mNetCdfParameterName;
          }
          break;

          default:
            break;
        }

        if (sourceParamKey.length() > 0  &&  targetParamKey.length() > 0)
        {
          std::size_t seed = 0;
          boost::hash_combine(seed,producerInfo->mName);
          boost::hash_combine(seed,sourceParamKey);
          boost::hash_combine(seed,targetParameterKeyType);
          boost::hash_combine(seed,targetParamKey);
          boost::hash_combine(seed,contentInfo->mGeometryId);
          boost::hash_combine(seed,paramLevelId);
          boost::hash_combine(seed,contentInfo->mParameterLevel);
          boost::hash_combine(seed,contentInfo->mForecastType);
          boost::hash_combine(seed,contentInfo->mForecastNumber);


          if (tmpList.find(seed) == tmpList.end())
          {
            tmpList.insert(seed);

            char tmp[200];
            char *p = tmp;
            p += sprintf(p,"%s;%s;%d;%s;%d;;%d;%05d;%d;%d",
                  producerInfo->mName.c_str(),
                  sourceParamKey.c_str(),
                  targetParameterKeyType,
                  targetParamKey.c_str(),
                  contentInfo->mGeometryId,
                  //paramLevelIdType,
                  paramLevelId,
                  contentInfo->mParameterLevel,
                  contentInfo->mForecastType,
                  contentInfo->mForecastNumber);

            if ((contentInfo->mFlags & T::ContentInfo::Flags::PreloadRequired) != 0)
              p += sprintf(p,";1");
            else
              p += sprintf(p,";0");

            list.insert(std::string(tmp));
          }
        }
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getProducerParameterListByProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    list.clear();

    T::ContentInfoList cList;
    getContent(0,0,20000000,cList);
    cList.sort(T::ContentInfo::ComparisonMethod::producer_file_message);

    std::set<std::size_t> tmpList;


    uint len = cList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = cList.getContentInfoByIndex(t);
      std::string sourceParamKey;
      std::string targetParamKey;
      T::ParamLevelId paramLevelId = contentInfo->mFmiParameterLevelId;

      if (contentInfo->mProducerId == producerId)
      {
        auto fmiId = contentInfo->mFmiParameterId;

        switch (sourceParameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            sourceParamKey = std::to_string(contentInfo->mFmiParameterId);
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            sourceParamKey = contentInfo->getFmiParameterName();
            break;

          case T::ParamKeyTypeValue::GRIB_ID:
          {
            Identification::FmiParameterId_grib def;
            if (Identification::gridDef.getGribParameterMappingByFmiId(fmiId,def))
              sourceParamKey = std::to_string(def.mGribParameterId);
          }
          break;

          case T::ParamKeyTypeValue::NEWBASE_ID:
          {
            Identification::FmiParameterId_newbase def;
            if (Identification::gridDef.getNewbaseParameterMappingByFmiId(fmiId,def))
              sourceParamKey = std::to_string(def.mNewbaseParameterId);
          }
          break;

          case T::ParamKeyTypeValue::NEWBASE_NAME:
          {
            Identification::NewbaseParameterDef def;
            if (Identification::gridDef.getNewbaseParameterDefByFmiId(fmiId,def))
              sourceParamKey = def.mParameterName;
          }
          break;

          case T::ParamKeyTypeValue::NETCDF_NAME:
          {
            Identification::FmiParameterId_netCdf def;
            if (Identification::gridDef.getNetCdfParameterMappingByFmiId(fmiId,def))
              sourceParamKey = def.mNetCdfParameterName;
          }
          break;

          default:
            break;
        }


        switch (targetParameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            targetParamKey = std::to_string(contentInfo->mFmiParameterId);
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            targetParamKey = contentInfo->getFmiParameterName();
            break;

          case T::ParamKeyTypeValue::GRIB_ID:
          {
            Identification::FmiParameterId_grib def;
            if (Identification::gridDef.getGribParameterMappingByFmiId(fmiId,def))
              targetParamKey = std::to_string(def.mGribParameterId);
          }
          break;

          case T::ParamKeyTypeValue::NEWBASE_ID:
          {
            Identification::FmiParameterId_newbase def;
            if (Identification::gridDef.getNewbaseParameterMappingByFmiId(fmiId,def))
              targetParamKey = std::to_string(def.mNewbaseParameterId);
          }
          break;

          case T::ParamKeyTypeValue::NEWBASE_NAME:
          {
            Identification::NewbaseParameterDef def;
            if (Identification::gridDef.getNewbaseParameterDefByFmiId(fmiId,def))
              targetParamKey = def.mParameterName;
          }
          break;

          case T::ParamKeyTypeValue::NETCDF_NAME:
          {
            Identification::FmiParameterId_netCdf def;
            if (Identification::gridDef.getNetCdfParameterMappingByFmiId(fmiId,def))
              targetParamKey = def.mNetCdfParameterName;
          }
          break;

          default:
            break;
        }

        if (!sourceParamKey.empty()  &&  !targetParamKey.empty())
        {
          std::size_t seed = 0;
          boost::hash_combine(seed,contentInfo->mProducerId);
          boost::hash_combine(seed,sourceParamKey);
          boost::hash_combine(seed,targetParameterKeyType);
          boost::hash_combine(seed,targetParamKey);
          boost::hash_combine(seed,contentInfo->mGeometryId);
          boost::hash_combine(seed,paramLevelId);
          boost::hash_combine(seed,contentInfo->mParameterLevel);
          boost::hash_combine(seed,contentInfo->mForecastType);
          boost::hash_combine(seed,contentInfo->mForecastNumber);


          if (tmpList.find(seed) == tmpList.end())
          {
            tmpList.insert(seed);

            char tmp[200];
            char *p = tmp;
            p += sprintf(p,"%s;%s;%d;%s;%d;;%d;%05d;%d;%d",
                  producerInfo.mName.c_str(),
                  sourceParamKey.c_str(),
                  targetParameterKeyType,
                  targetParamKey.c_str(),
                  contentInfo->mGeometryId,
                  //paramLevelIdType,
                  paramLevelId,
                  contentInfo->mParameterLevel,
                  contentInfo->mForecastType,
                  contentInfo->mForecastNumber);

            if ((contentInfo->mFlags & T::ContentInfo::Flags::PreloadRequired) != 0)
              p += sprintf(p,";1");
            else
              p += sprintf(p,";0");

            list.insert(std::string(tmp));
          }
        }
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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
    if (info != nullptr)
      return Result::GENERATION_NAME_ALREADY_REGISTERED;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"INCR %sgenerationCounter",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }


    generationInfo.mGenerationId = reply->integer;
    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %sgenerations %u %s",mTablePrefix.c_str(),generationInfo.mGenerationId,generationInfo.getCsv().c_str()));
    if (reply == nullptr)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_setGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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
    if (info != nullptr &&  info->mGenerationId != generationInfo.mGenerationId)
      return Result::GENERATION_NAME_ALREADY_REGISTERED;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZREMRANGEBYSCORE %sgenerations %u %u",mTablePrefix.c_str(),generationInfo.mGenerationId,generationInfo.mGenerationId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %sgenerations %u %s",mTablePrefix.c_str(),generationInfo.mGenerationId,generationInfo.getCsv().c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    addEvent(EventType::GENERATION_UPDATED,generationInfo.mGenerationId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteGenerationInfoById(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;


    deleteContentByGenerationId(generationInfo.mGenerationId);
    deleteFileListByGenerationId(generationInfo.mGenerationId,false);
    deleteGeometryListByGenerationId(generationInfo.mGenerationId,false,false);

    int result = deleteGenerationById(generationInfo.mGenerationId,false,false,false);

    if (result == Result::OK)
      addEvent(EventType::GENERATION_DELETED,generationId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteGenerationInfoByName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationByName(generationName,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_NAME;


    deleteContentByGenerationId(generationInfo.mGenerationId);
    deleteFileListByGenerationId(generationInfo.mGenerationId,false);
    deleteGeometryListByGenerationId(generationInfo.mGenerationId,false,false);

    int result = deleteGenerationById(generationInfo.mGenerationId,false,false,false);

    if (result == Result::OK)
      addEvent(EventType::GENERATION_DELETED,generationInfo.mGenerationId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<uint>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    deleteContentByGenerationIdList(generationIdList);
    deleteFileListByGenerationIdList(generationIdList,false);
    deleteGeometryListByGenerationIdList(generationIdList,false,false);

    for (auto it = generationIdList.begin(); it != generationIdList.end(); ++it)
    {
      deleteGenerationById(*it,false,false,false);
      addEvent(EventType::GENERATION_DELETED,*it,0,0,0);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;


    deleteContentByProducerId(producerInfo.mProducerId);
    deleteFileListByProducerId(producerInfo.mProducerId,false);
    deleteGeometryListByProducerId(producerInfo.mProducerId,false,false);

    int result = deleteGenerationListByProducerId(producerId,false,false,false);

    if (result == Result::OK)
      addEvent(EventType::GENERATION_LIST_DELETED_BY_PRODUCER_ID,producerId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerByName(producerName,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;


    deleteContentByProducerId(producerInfo.mProducerId);
    deleteFileListByProducerId(producerInfo.mProducerId,false);
    deleteGeometryListByProducerId(producerInfo.mProducerId,false,false);

    int result = deleteGenerationListByProducerId(producerInfo.mProducerId,false,false,false);

    if (result == Result::OK)
      addEvent(EventType::GENERATION_LIST_DELETED_BY_PRODUCER_ID,producerInfo.mProducerId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    deleteContentBySourceId(sourceId);
    deleteFileListBySourceId(sourceId,false);
    deleteGeometryListBySourceId(sourceId,false,false);

    int result = deleteGenerationListBySourceId(sourceId,false,false,false);

    if (result == Result::OK)
      addEvent(EventType::GENERATION_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getGenerationById(generationId,generationInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getGenerationInfoByName(T::SessionId sessionId,const std::string& generationName,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getGenerationByName(generationName,generationInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    generationInfoList.clear();

    return getGenerationList(generationInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    generationInfoList.clear();

    return getGenerationListByGeometryId(geometryId,generationInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    generationInfoList.clear();

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    return getGenerationListByProducerId(producerId,generationInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    generationInfoList.clear();

    T::ProducerInfo producerInfo;
    if (getProducerByName(producerName,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_NAME;

    return getGenerationListByProducerId(producerInfo.mProducerId,generationInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    generationInfoList.clear();

    return getGenerationListBySourceId(sourceId,generationInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,const std::string& producerName,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_getGenerationInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    count = 0;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZCOUNT %sgenerations 0 %llu",mTablePrefix.c_str(),0xFFFFFFFF));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->str != nullptr)
      count = toInt64(reply->str);
    else
      count = reply->integer;

    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,uchar status)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    generationInfo.mStatus = status;


    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZREMRANGEBYSCORE %sgenerations %u %u",mTablePrefix.c_str(),generationInfo.mGenerationId,generationInfo.mGenerationId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %sgenerations %u %s",mTablePrefix.c_str(),generationInfo.mGenerationId,generationInfo.getCsv().c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    addEvent(EventType::GENERATION_STATUS_CHANGED,generationId,status,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_setGenerationInfoStatusByName(T::SessionId sessionId,const std::string& generationName,uchar status)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationByName(generationName,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_NAME;

    generationInfo.mStatus = status;


    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZREMRANGEBYSCORE %sgenerations %u %u",mTablePrefix.c_str(),generationInfo.mGenerationId,generationInfo.mGenerationId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %sgenerations %u %s",mTablePrefix.c_str(),generationInfo.mGenerationId,generationInfo.getCsv().c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    addEvent(EventType::GENERATION_STATUS_CHANGED,generationInfo.mGenerationId,status,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_addGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(geometryInfo.mProducerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo generationInfo;
    if (getGenerationById(geometryInfo.mGenerationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    if (producerInfo.mProducerId != generationInfo.mProducerId)
      return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;


    unsigned long long id = getGeometryKey(geometryInfo.mGenerationId,geometryInfo.mGeometryId,geometryInfo.mLevelId);

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %sgeometries %llu %s",mTablePrefix.c_str(),id,geometryInfo.getCsv().c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    addEvent(EventType::GEOMETRY_ADDED,geometryInfo.mGenerationId,geometryInfo.mGeometryId,geometryInfo.mLevelId,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_deleteGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GeometryInfo geometryInfo;
    if (getGeometryById(generationId,geometryId,levelId,geometryInfo) != Result::OK)
      return Result::UNKNOWN_GEOMETRY;

    int result = deleteGeometryById(generationId,geometryId,levelId,true,true);

    if (result == Result::OK)
      addEvent(EventType::GEOMETRY_DELETED,generationId,geometryId,levelId,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    //deleteContentByGenerationId(generationId);
    //deleteFileListByGenerationId(generationId,false);

    int result = deleteGeometryListByGenerationId(generationId,false,false);

    if (result == Result::OK)
      addEvent(EventType::GEOMETRY_LIST_DELETED_BY_GENERATION_ID,generationId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    //deleteContentByProducerId(producerId);
    //deleteFileListByProducerId(producerId,false);

    int result = deleteGeometryListByProducerId(producerId,false,false);

    if (result == Result::OK)
      addEvent(EventType::GEOMETRY_LIST_DELETED_BY_PRODUCER_ID,producerId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_deleteGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    //deleteContentBySourceId(sourceId);
    //deleteFileListBySourceId(sourceId,false);

    int result = deleteGeometryListBySourceId(sourceId,false,false);

    if (result == Result::OK)
      addEvent(EventType::GEOMETRY_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_getGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getGeometryById(generationId,geometryId,levelId,geometryInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_getGeometryInfoList(T::SessionId sessionId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    geometryInfoList.clear();

    return getGeometryList(geometryInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    geometryInfoList.clear();

    return getGeometryListByGenerationId(generationId,geometryInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_getGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    geometryInfoList.clear();

    return getGeometryListByProducerId(producerId,geometryInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_getGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    geometryInfoList.clear();

    return getGeometryListBySourceId(sourceId,geometryInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_getGeometryInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    count = 0;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZCOUNT %sgeometries 0 %llu",mTablePrefix.c_str(),0xFFFFFFFFFFFFFFFF));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->str != nullptr)
      count = toInt64(reply->str);
    else
      count = reply->integer;

    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_setGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(geometryInfo.mProducerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo generationInfo;
    if (getGenerationById(geometryInfo.mGenerationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    if (producerInfo.mProducerId != generationInfo.mProducerId)
      return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;

    unsigned long long id = getGeometryKey(geometryInfo.mGenerationId,geometryInfo.mGeometryId,geometryInfo.mLevelId);

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZREMRANGEBYSCORE %sgeometries %llu %llu",mTablePrefix.c_str(),id,id));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %sgeometries %llu %s",mTablePrefix.c_str(),id,geometryInfo.getCsv().c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    addEvent(EventType::GEOMETRY_UPDATED,geometryInfo.mGenerationId,geometryInfo.mGeometryId,geometryInfo.mLevelId,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_setGeometryInfoStatusById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,uchar status)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GeometryInfo geometryInfo;
    if (getGeometryById(generationId,geometryId,levelId,geometryInfo) != Result::OK)
      return Result::UNKNOWN_GEOMETRY;

    geometryInfo.mStatus = status;

    unsigned long long id = getGeometryKey(geometryInfo.mGenerationId,geometryInfo.mGeometryId,geometryInfo.mLevelId);

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZREMRANGEBYSCORE %sgeometries %llu %llu",mTablePrefix.c_str(),id,id));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %sgeometries %llu %s",mTablePrefix.c_str(),id,geometryInfo.getCsv().c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    addEvent(EventType::GEOMETRY_STATUS_CHANGED,generationId,geometryId,levelId,status);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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
      T::FileInfo fInfo;
      if (getFileById(fileId,fInfo) != Result::OK)
      {
        // The filename exists, but the actual FileInfo record does not.
        deleteFilename(fileInfo.mName);
        fileId = 0;
      }
    }

    if (fileId > 0)
    {
      // ### File with the same name already exists. Let's return
      // ### the current file-id.

      fileInfo.mFileId = fileId;

      // ### Adding an event to the event list.

      addEvent(EventType::FILE_UPDATED,fileInfo.mFileId,fileInfo.mFileType,0,0);
    }
    else
    {
      // ### Generating a new file-id.


      redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"INCR %sfileCounter",mTablePrefix.c_str()));
      if (reply == nullptr)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      fileInfo.mFileId = reply->integer;
      freeReplyObject(reply);

      // ### Adding the file information into the database.

      reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %sfiles %u %s",mTablePrefix.c_str(),fileInfo.mFileId,fileInfo.getCsv().c_str()));
      if (reply == nullptr)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      freeReplyObject(reply);

      // ### Adding the filename into the database. This needs to be added to another
      // ### table so that we can fetch it fast.

      addFilename(fileInfo.mName,fileInfo.mFileId);

      // ### Adding an event to the event list.

      addEvent(EventType::FILE_ADDED,fileInfo.mFileId,fileInfo.mFileType,0,0);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_setFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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

    T::FileInfo fInfo;
    if (getFileById(fileInfo.mFileId,fInfo) != Result::OK)
      return Result::UNKNOWN_FILE_ID;

    uint fileId = getFileId(fileInfo.mName);
    if (fileId > 0)
    {
      if (fileId != fileInfo.mFileId)
        return Result::FILE_NAME_ALREADY_REGISTERED;

      if (fileInfo.mName != fInfo.mName)
      {
        // The filename exists, but the actual FileInfo record does not.
        deleteFilename(fInfo.mName);
        addFilename(fileInfo.mName,fileInfo.mFileId);
      }
    }

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZREMRANGEBYSCORE %sfiles %u %u",mTablePrefix.c_str(),fileInfo.mFileId,fileInfo.mFileId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);


    // ### Adding the file information into the database.

    reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %sfiles %u %s",mTablePrefix.c_str(),fileInfo.mFileId,fileInfo.getCsv().c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

      // ### Adding an event to the event list.

    addEvent(EventType::FILE_UPDATED,fileInfo.mFileId,fileInfo.mFileType,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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
      T::FileInfo fInfo;
      if (getFileById(fileId,fInfo) != Result::OK)
      {
        // The filename exists, but the actual FileInfo record does not.
        deleteFilename(fileInfo.mName);
        fileId = 0;
      }
    }

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

        redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"INCR %sfileCounter",mTablePrefix.c_str()));
      if (reply == nullptr)
      {
        closeConnection();
        return Result::PERMANENT_STORAGE_ERROR;
      }

      fileInfo.mFileId = reply->integer;
      fileInfo.mFlags = fileInfo.mFlags;
      freeReplyObject(reply);

      // ### Adding the file information into the database.

      reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %sfiles %u %s",mTablePrefix.c_str(),fileInfo.mFileId,fileInfo.getCsv().c_str()));
      if (reply == nullptr)
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
      if (info != nullptr)
      {
        // ### Making sure that content data matches the file data.

        info->mFileId = fileInfo.mFileId;
        if (info->mFileType == 0)
          info->mFileType = fileInfo.mFileType;

        info->mProducerId = fileInfo.mProducerId;
        info->mGenerationId = fileInfo.mGenerationId;
        info->mFlags = info->mFlags;

        // ### Creating a key for the content.

        unsigned long long id = getContentKey(info->mFileId,info->mMessageIndex);

        // ### Adding the content record into the database.

        redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %scontent %llu %s",mTablePrefix.c_str(),id,info->getCsv().c_str()));
        if (reply == nullptr)
        {
          closeConnection();
          return Result::PERMANENT_STORAGE_ERROR;
        }

        freeReplyObject(reply);
      }
    }

    // ### Adding an event to the event list.

    if (fileId > 0)
    {
      addEvent(EventType::FILE_UPDATED,fileInfo.mFileId,fileInfo.mFileType,contentInfoList.getLength(),0);
    }
    else
    {
      addEvent(EventType::FILE_ADDED,fileInfo.mFileId,fileInfo.mFileType,contentInfoList.getLength(),0);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    T::ProducerInfo producerInfo;
    T::GenerationInfo generationInfo;

    for (auto ff = fileAndContentList.begin();ff != fileAndContentList.end(); ++ff)
    {
      if (ff->mFileInfo.mProducerId != producerInfo.mProducerId &&  getProducerById(ff->mFileInfo.mProducerId,producerInfo) != Result::OK)
        return Result::UNKNOWN_PRODUCER_ID;

      if (ff->mFileInfo.mGenerationId != generationInfo.mGenerationId  && getGenerationById(ff->mFileInfo.mGenerationId,generationInfo) != Result::OK)
        return Result::UNKNOWN_GENERATION_ID;

      if (producerInfo.mProducerId != generationInfo.mProducerId)
        return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;

      // ### Checking if the filename already exists in the database.

      uint fileId = getFileId(ff->mFileInfo.mName);
      if (fileId > 0)
      {
        T::FileInfo fInfo;
        if (getFileById(fileId,fInfo) != Result::OK)
        {
          // The filename exists, but the actual FileInfo record does not.
          deleteFilename(ff->mFileInfo.mName);
          fileId = 0;
        }
      }

      if (fileId > 0)
      {
        // ### File with the same name already exists. Let's return
        // ### the current file-id.

        ff->mFileInfo.mFileId = fileId;

        // ### Deleting old content information.

        if (requestFlags & 0x00000001)
          deleteContentByFileId(ff->mFileInfo.mFileId);
      }
      else
      {
        // ### Generating a new file-id.

        redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"INCR %sfileCounter",mTablePrefix.c_str()));
        if (reply == nullptr)
        {
          closeConnection();
          return Result::PERMANENT_STORAGE_ERROR;
        }

        ff->mFileInfo.mFileId = reply->integer;
        ff->mFileInfo.mFlags = ff->mFileInfo.mFlags;
        freeReplyObject(reply);

        // ### Adding the file information into the database.

        reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %sfiles %u %s",mTablePrefix.c_str(),ff->mFileInfo.mFileId,ff->mFileInfo.getCsv().c_str()));
        if (reply == nullptr)
        {
          closeConnection();
          return Result::PERMANENT_STORAGE_ERROR;
        }

        freeReplyObject(reply);

        // ### Adding the filename into the database. This needs to be added to another
        // ### table so that we can fetch it fast.

        addFilename(ff->mFileInfo.mName,ff->mFileInfo.mFileId);
      }


      // ### Adding the content information into the database.

      uint len = ff->mContentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = ff->mContentInfoList.getContentInfoByIndex(t);
        if (info != nullptr)
        {
          // ### Making sure that content data matches the file data.

          info->mFileId = ff->mFileInfo.mFileId;
          if (info->mFileType == 0)
            info->mFileType = ff->mFileInfo.mFileType;

          info->mProducerId = ff->mFileInfo.mProducerId;
          info->mGenerationId = ff->mFileInfo.mGenerationId;
          info->mFlags = info->mFlags;

          T::ContentInfo contentInfo;
          if (getContent(info->mFileId,info->mMessageIndex,contentInfo) == DATA_NOT_FOUND)
          {
            // ### Creating a key for the content.
            unsigned long long id = getContentKey(info->mFileId,info->mMessageIndex);

            // ### Adding the content record into the database.

            redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %scontent %llu %s",mTablePrefix.c_str(),id,info->getCsv().c_str()));
            if (reply == nullptr)
            {
              closeConnection();
              return Result::PERMANENT_STORAGE_ERROR;
            }

            freeReplyObject(reply);

            if (fileId > 0  &&  (requestFlags & 0x00000001) == 0)
              addEvent(EventType::CONTENT_ADDED,info->mFileId,info->mMessageIndex,0,0);
          }
        }
      }

      // ### Adding an event to the event list.

      if (fileId > 0)
      {
        if (requestFlags & 0x00000001)
          addEvent(EventType::FILE_UPDATED,ff->mFileInfo.mFileId,ff->mFileInfo.mFileType,0,0);
      }
      else
      {
        addEvent(EventType::FILE_ADDED,ff->mFileInfo.mFileId,ff->mFileInfo.mFileType,len,0);
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteFileInfoById(T::SessionId sessionId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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
      addEvent(EventType::FILE_DELETED,fileId,fileInfo.mFileType,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteFileInfoByName(T::SessionId sessionId,const std::string& filename)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint fileId = getFileId(filename);
    if (fileId == 0)
      return Result::UNKNOWN_FILE_NAME;

    deleteFilename(filename);

    T::FileInfo fileInfo;
    if (getFileById(fileId,fileInfo) != Result::OK)
    {
      return Result::UNKNOWN_FILE_ID;
    }

    int result = deleteFileById(fileId,true);

    if (result == Result::OK)
    {
      if (fileInfo.mFileType == T::FileTypeValue::Virtual)
        addEvent(EventType::FILE_DELETED,fileId,T::FileTypeValue::Virtual,0,0);
      else
        addEvent(EventType::FILE_DELETED,fileId,0,0,0);
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    /*
    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;
     */

    int result = deleteFileListByProducerId(producerId,true);

    if (result == Result::OK)
      addEvent(EventType::FILE_LIST_DELETED_BY_PRODUCER_ID,producerId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    /*
    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;
     */

    int result = deleteFileListByGenerationId(generationId,true);

    if (result == Result::OK)
      addEvent(EventType::FILE_LIST_DELETED_BY_GENERATION_ID,generationId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,time_t forecastTime)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;


    std::set<uint> fileIdList;
    T::ContentInfoList contentInfoList;
    int result = getContentByGenerationIdAndTimeRange(generationId,forecastTime,forecastTime,contentInfoList);
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      if (info->mGeometryId == geometryId  &&  info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber)
      {
        deleteContent(info->mFileId,info->mMessageIndex);
        //if (fileIdList.find(info->mFileId) != fileIdList.end())
          fileIdList.insert(info->mFileId);
      }
    }

    for (auto it=fileIdList.begin(); it != fileIdList.end(); ++it)
    {
      T::ContentInfoList contentList;
      getContentByFileId(*it,contentList);
      if (contentList.getLength() == 0)
      {
        T::FileInfo fileInfo;
        if (getFileById(*it,fileInfo) == Result::OK)
        {
          deleteFilename(fileInfo.mName);
          deleteFileById(*it,false);
          addEvent(EventType::FILE_DELETED,*it,fileInfo.mFileType,0,0);
        }
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    T::ContentInfoList contentInfoList;
    int result = getContentByForecastTimeList(forecastTimeList,contentInfoList);
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      deleteContent(info->mFileId,info->mMessageIndex);

      T::FileInfo fileInfo;
      if (getFileById(info->mFileId,fileInfo) == Result::OK)
      {
        deleteFilename(fileInfo.mName);
        deleteFileById(info->mFileId,false);
        addEvent(EventType::FILE_DELETED,info->mFileId,fileInfo.mFileType,0,0);
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    int result = deleteFileListBySourceId(sourceId,true);

    if (result == Result::OK)
      addEvent(EventType::FILE_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    for (auto it=fileIdList.begin(); it!=fileIdList.end(); ++it)
    {
      T::FileInfo fileInfo;
      if (getFileById(*it,fileInfo) == Result::OK)
      {
        deleteFilename(fileInfo.mName);
        deleteFileById(*it,true);
        addEvent(EventType::FILE_DELETED,*it,0,0,0);
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getFileById(fileId,fileInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getFileInfoByName(T::SessionId sessionId,const std::string& filename,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getFileInfoList(T::SessionId sessionId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    fileInfoList.clear();

    return getFileList(startFileId,maxRecords,fileInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    fileInfoList.clear();

    for (auto it = fileIdList.begin(); it != fileIdList.end(); ++it)
    {
      T::FileInfo *fileInfo = new T::FileInfo();
      if (getFileById(*it,*fileInfo) == Result::OK)
        fileInfoList.addFileInfo(fileInfo);
      else
        delete fileInfo;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    fileInfoList.clear();

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    return getFileListByProducerId(producerId,startFileId,maxRecords,fileInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    fileInfoList.clear();

    T::ProducerInfo producerInfo;
    if (getProducerByName(producerName,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_NAME;

    return getFileListByProducerId(producerInfo.mProducerId,startFileId,maxRecords,fileInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    fileInfoList.clear();

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    return getFileListByGenerationId(generationId,startFileId,maxRecords,fileInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    fileInfoList.clear();

    T::GenerationInfo generationInfo;
    if (getGenerationByName(generationName,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_NAME;

    return getFileListByGenerationId(generationInfo.mGenerationId,startFileId,maxRecords,fileInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    fileInfoList.clear();

    return getFileListBySourceId(sourceId,startFileId,maxRecords,fileInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getFileInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    count = 0;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZCOUNT %sfiles 0 %llu",mTablePrefix.c_str(),0xFFFFFFFF));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->str != nullptr)
      count = toInt64(reply->str);
    else
      count = reply->integer;

    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getFileInfoCountByProducerId(T::SessionId sessionId,uint producerId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    count = 0;
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sfiles %u %u",mTablePrefix.c_str(),0,0xFFFFFFFF));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        uint pId = getCsvInt64Field(reply->element[t]->str,3);
        if (pId == producerId)
          count++;
      }
    }
    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getFileInfoCountByGenerationId(T::SessionId sessionId,uint generationId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    count = 0;
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sfiles %u %u",mTablePrefix.c_str(),0,0xFFFFFFFF));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        uint gId = getCsvInt64Field(reply->element[t]->str,4);
        if (gId == generationId)
          count++;
      }
    }
    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getFileInfoCountBySourceId(T::SessionId sessionId,uint sourceId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    count = 0;
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sfiles %u %u",mTablePrefix.c_str(),0,0xFFFFFFFF));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        uint sId = getCsvInt64Field(reply->element[t]->str,7);
        if (sId == sourceId)
          count++;
      }
    }
    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    eventInfo.mEventId = addEvent(eventInfo.mType,eventInfo.mId1,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    eventInfo.mServerTime = mStartTime;

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::EventId eventId = 0;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"GET %seventCounter",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->str != nullptr)
      eventId = (T::EventId)toInt64(reply->str);

    freeReplyObject(reply);

    if (eventId == 0)
      return Result::DATA_NOT_FOUND;

    if (mLastEvent.mEventId == eventId)
    {
      eventInfo = mLastEvent;
      eventInfo.mServerTime = mStartTime;
      return Result::OK;
    }

    reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sevents %llu %llu",mTablePrefix.c_str(),eventId,eventId));
    if (reply == nullptr)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    eventInfoList.clear();

    if (maxRecords > 10000)
      maxRecords = 10000;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sevents %llu %llu LIMIT 0 %u",mTablePrefix.c_str(),startEventId,0xFFFFFFFFFFFF,maxRecords));
    if (reply == nullptr)
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

        if (eventInfo->mType == EventType::FILE_ADDED)
        {
          T::FileInfo fileInfo;

          if (getFileById(eventInfo->mId1,fileInfo) == 0)
          {
            std::ostringstream output;
            output << fileInfo.getCsv() << "\n";

            if (eventInfo->mId3 > 0)
            {
              T::ContentInfoList contentInfoList;
              getContentByFileId(eventInfo->mId1,contentInfoList);

              uint len = contentInfoList.getLength();
              for (uint t=0; t<len; t++)
              {
                T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
                output << contentInfo->getCsv() << "\n";
              }
            }
            eventInfo->mNote = output.str();
          }
          else
          {
            // File not found
          }
        }
        else
        if (eventInfo->mType == EventType::CONTENT_ADDED)
        {
          T::ContentInfo contentInfo;
          if (getContent(eventInfo->mId1,eventInfo->mId2,contentInfo) == 0)
          {
            eventInfo->mNote = contentInfo.getCsv();
          }
          else
          {
           // Content not found
          }
        }

        eventInfoList.addEventInfo(eventInfo);
      }
    }
    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getEventInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    count = 0;

    //RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZCOUNT %sevents 0 %llu",mTablePrefix.c_str(),0xFFFFFFFFFFFFFFFF));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->str != nullptr)
      count = toInt64(reply->str);
    else
      count = reply->integer;

    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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



    unsigned long long id = getContentKey(contentInfo.mFileId,contentInfo.mMessageIndex);

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %scontent %llu %s",mTablePrefix.c_str(),id,contentInfo.getCsv().c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    addEvent(EventType::CONTENT_ADDED,contentInfo.mFileId,contentInfo.mMessageIndex,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_setContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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


    unsigned long long id = getContentKey(contentInfo.mFileId,contentInfo.mMessageIndex);

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZREMRANGEBYSCORE %scontent %llu %llu",mTablePrefix.c_str(),id,id));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %scontent %llu %s",mTablePrefix.c_str(),id,contentInfo.getCsv().c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    addEvent(EventType::CONTENT_UPDATED,contentInfo.mFileId,contentInfo.mMessageIndex,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    T::ProducerInfo producerInfo;
    T::GenerationInfo generationInfo;
    T::FileInfo fileInfo;

    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      T::ContentInfo contentInfo;
      if (getContent(info->mFileId,info->mMessageIndex,contentInfo) == Result::OK)
      {
        // Content already added
      }
      else
      {
        if (info->mProducerId != producerInfo.mProducerId && getProducerById(info->mProducerId,producerInfo) != Result::OK)
          return Result::UNKNOWN_PRODUCER_ID;

        if (info->mGenerationId != generationInfo.mGenerationId &&  getGenerationById(info->mGenerationId,generationInfo) != Result::OK)
          return Result::UNKNOWN_GENERATION_ID;

        if (info->mFileId != fileInfo.mFileId && getFileById(info->mFileId,fileInfo) != Result::OK)
          return Result::UNKNOWN_FILE_ID;

        if (producerInfo.mProducerId != generationInfo.mProducerId)
          return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;

        if (producerInfo.mProducerId != fileInfo.mProducerId)
          return Result::PRODUCER_AND_FILE_DO_NOT_MATCH;

        if (generationInfo.mGenerationId != fileInfo.mGenerationId)
          return Result::GENERATION_AND_FILE_DO_NOT_MATCH;

        unsigned long long id = getContentKey(info->mFileId,info->mMessageIndex);

        redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %scontent %llu %s",mTablePrefix.c_str(),id,info->getCsv().c_str()));
        if (reply == nullptr)
        {
          closeConnection();
          return Result::PERMANENT_STORAGE_ERROR;
        }

        freeReplyObject(reply);

        addEvent(EventType::CONTENT_ADDED,info->mFileId,info->mMessageIndex,0,0);
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteContentListByFileId(T::SessionId sessionId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    /*
    T::FileInfo fileInfo;
    if (getFileById(fileId,fileInfo) != Result::OK)
      return Result::UNKNOWN_FILE_ID;
     */

    int result = deleteContentByFileId(fileId);

    if (result == Result::OK)
      addEvent(EventType::CONTENT_LIST_DELETED_BY_FILE_ID,fileId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteContentListByFileName(T::SessionId sessionId,const std::string& filename)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteContentListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    /*
    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;
     */

    int result = deleteContentByProducerId(producerId);

    if (result == Result::OK)
      addEvent(EventType::CONTENT_LIST_DELETED_BY_PRODUCER_ID,producerId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteContentListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_deleteContentListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    /*
    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;
     */

    int result = deleteContentByGenerationId(generationId);

    if (result == Result::OK)
      addEvent(EventType::CONTENT_LIST_DELETED_BY_GENERATION_ID,generationId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteContentListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteContentListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    int result = deleteContentBySourceId(sourceId);

    if (result == Result::OK)
      addEvent(EventType::CONTENT_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getContent(fileId,messageIndex,contentInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    contentInfoList.clear();

    return getContent(startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    contentInfoList.clear();

    return getContentByFileId(fileId,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    contentInfoList.clear();

    for (auto it = fileIdList.begin(); it != fileIdList.end(); ++it)
    {
      T::ContentInfoList contentList;
      getContentByFileId(*it,contentList);
      contentInfoList.addContentInfoList(contentList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentListByFileName(T::SessionId sessionId,const std::string& filename,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint fileId = getFileId(filename);
    if (fileId == 0)
      return Result::UNKNOWN_FILE_NAME;

    contentInfoList.clear();

    return getContentByFileId(fileId,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    contentInfoList.clear();

    return getContentByProducerId(producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerByName(producerName,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_NAME;

    contentInfoList.clear();

    return getContentByProducerId(producerInfo.mProducerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,int maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    contentInfoList.clear();

    return getContentByGenerationId(generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






int RedisImplementation::_getContentListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationByName(generationName,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_NAME;

    contentInfoList.clear();

    return getContentByGenerationId(generationInfo.mGenerationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    contentInfoList.clear();

    return getContentByGenerationIdAndTimeRange(generationId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationByName(generationName,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_NAME;

    contentInfoList.clear();

    return getContentByGenerationIdAndTimeRange(generationInfo.mGenerationId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    contentInfoList.clear();

    return getContentBySourceId(sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    contentInfoList.clear();

    return getContentByParameterIdAndTimeRange(parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    contentInfoList.clear();

    return getContentByParameterIdAndGeneration(generationInfo.mGenerationId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationByName(generationName,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_NAME;

    contentInfoList.clear();

    return getContentByParameterIdAndGeneration(generationInfo.mGenerationId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    contentInfoList.clear();

    return getContentByParameterIdAndProducer(producerInfo.mProducerId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    contentInfoList.clear();

    time_t startTime = 0;
    time_t endTime = 0xFFFFFFFF;

    T::ContentInfoList contentList;
    int res = getContentByParameterIdAndGeneration(generationInfo.mGenerationId,parameterKeyType,parameterKey,parameterLevelId,level,level,forecastType,forecastNumber,geometryId,startTime,endTime,contentList);

    contentList.getContentListByForecastTime(forecastTime,contentInfoList);

    // If we cannot find any forecast time, lets add at least one
    // time in order to show that there are other times available.

    if (contentInfoList.getLength() == 0  &&  contentList.getLength() > 0)
    {
      T::ContentInfo *info = contentList.getContentInfoByIndex(0);
      if (info != nullptr)
      {
        contentInfoList.addContentInfo(info->duplicate());
      }
    }
    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerByName(producerName,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_NAME;

    contentInfoList.clear();

    return getContentByParameterIdAndProducer(producerInfo.mProducerId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    contentInfoList.clear();

    T::ProducerInfoList producerInfoList;
    getProducerList(producerInfoList);

    T::GenerationInfoList generationInfoList;
    getGenerationList(generationInfoList);

    T::FileInfoList fileInfoList;
    getFileList(0,10000000,fileInfoList);
    fileInfoList.sort(T::FileInfo::ComparisonMethod::fileId);

    T::ContentInfoList contentList;
    getContent(0,0,20000000,contentList);

    uint cLen = contentList.getLength();
    for (uint c=0; c<cLen; c++)
    {
      T::ContentInfo *cInfo = contentList.getContentInfoByIndex(c);
      T::ContentInfo *cError = nullptr;
      if (cInfo != nullptr)
      {
        T::FileInfo *fileInfo = fileInfoList.getFileInfoById(cInfo->mFileId);
        if (fileInfo == nullptr)
        {
          printf("**** INTEGRITY ERROR : File missing (%u)! *****\n",cInfo->mFileId);
          cError = cInfo;
        }

        if (cError == nullptr)
        {
          T::GenerationInfo *generationInfo = generationInfoList.getGenerationInfoById(cInfo->mGenerationId);
          if (generationInfo == nullptr)
          {
            printf("**** INTEGRITY ERROR : Generation missing (%u)! *****\n",cInfo->mGenerationId);
            cError = cInfo;
          }
        }

        if (cError == nullptr)
        {
          T::ProducerInfo *producerInfo = producerInfoList.getProducerInfoById(cInfo->mProducerId);
          if (producerInfo == nullptr)
          {
            printf("**** INTEGRITY ERROR : Producer missing (%u)! *****\n",cInfo->mProducerId);
            cError = cInfo;
          }
        }

        if (cError != nullptr)
          contentInfoList.addContentInfo(cError->duplicate());
      }
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    geometryIdList.clear();

    T::ContentInfoList contentInfoList;
    int res = getContentByGenerationId(generationInfo.mGenerationId,0,0,10000000,contentInfoList);

    contentInfoList.getContentGeometryIdListByGenerationId(generationInfo.mProducerId,generationId,geometryIdList);
    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    contentParamList.clear();

    T::ContentInfoList contentInfoList;
    int res = getContentByGenerationId(generationInfo.mGenerationId,0,0,10000000,contentInfoList);

    contentInfoList.sort(T::ContentInfo::ComparisonMethod::fmiName_fmiLevelId_level_starttime_file_message);
    uint len = contentInfoList.getLength();
    T::ContentInfo *prev = nullptr;
    T::ContentInfo *currentInfo = nullptr;
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);

      if (prev == nullptr ||
          info->getFmiParameterName() != prev->getFmiParameterName() ||
          info->mFmiParameterLevelId != prev->mFmiParameterLevelId ||
          info->mParameterLevel != prev->mParameterLevel ||
          info->mForecastType != prev->mForecastType ||
          info->mForecastNumber != prev->mForecastNumber)
      {
        currentInfo = info->duplicate();
        currentInfo->mMessageIndex = 1;
        contentParamList.addContentInfo(currentInfo);
      }
      else
      {
        currentInfo->mMessageIndex++;
      }
      prev = info;
    }
    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentParamKeyListByGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    paramKeyList.clear();

    T::ContentInfoList contentInfoList;
    int res = getContentByGenerationId(generationInfo.mGenerationId,0,0,10000000,contentInfoList);
    contentInfoList.getContentParamKeyListByGenerationId(generationInfo.mProducerId,generationId,parameterKeyType,paramKeyList);

    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentParamKeyListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    paramKeyList.clear();

    T::ContentInfoList contentInfoList;
    int res = getContentByGenerationId(generationInfo.mGenerationId,0,0,10000000,contentInfoList);
    contentInfoList.getContentParamKeyListByGenerationAndGeometryId(generationInfo.mProducerId,generationId,geometryId,parameterKeyType,paramKeyList);

    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int RedisImplementation::_getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    contentTimeList.clear();

    T::ContentInfoList contentInfoList;
    int res = getContentByGenerationId(generationInfo.mGenerationId,0,0,10000000,contentInfoList);
    contentInfoList.getForecastTimeListByGenerationId(generationInfo.mProducerId,generationId,contentTimeList);

    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentTimeRangeByProducerAndGenerationId(T::SessionId sessionId,uint producerId,uint generationId,time_t& startTime,time_t& endTime)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    T::ContentInfoList contentInfoList;
    int res = getContentByGenerationId(generationInfo.mGenerationId,0,0,10000000,contentInfoList);
    contentInfoList.getForecastTimeRangeByGenerationId(producerId,generationId,startTime,endTime);

    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_getContentTimeRangeByGenerationId(T::SessionId sessionId,uint generationId,time_t& startTime,time_t& endTime)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    T::ContentInfoList contentInfoList;
    int res = getContentByGenerationId(generationInfo.mGenerationId,0,0,10000000,contentInfoList);
    contentInfoList.getForecastTimeRangeByGenerationId(generationInfo.mProducerId,generationId,startTime,endTime);

    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfo generationInfo;
    if (getGenerationById(generationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    contentTimeList.clear();

    T::ContentInfoList contentInfoList;
    int res = getContentByGenerationId(generationInfo.mGenerationId,0,0,10000000,contentInfoList);
    contentInfoList.getForecastTimeListByGenerationAndGeometry(generationInfo.mProducerId,generationId,geometryId,contentTimeList);
    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentTimeListByProducerId(T::SessionId sessionId,uint producerId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo producerInfo;
    if (getProducerById(producerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    contentTimeList.clear();

    T::ContentInfoList contentInfoList;
    int res = getContentByProducerId(producerId,0,0,10000000,contentInfoList);
    contentInfoList.getForecastTimeListByProducerId(producerId,contentTimeList);

    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    list.clear();

    int res = getGenerationTimeAndGeometryList(list);
    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getContentCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    count = 0;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZCOUNT %scontent 0 %llu",mTablePrefix.c_str(),0xFFFFFFFFFFFFFFFF));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->str != nullptr)
      count = toInt64(reply->str);
    else
      count = reply->integer;

    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_getHashByProducerId(T::SessionId sessionId,uint producerId,ulonglong& hash)
{
  FUNCTION_TRACE
  try
  {
    hash = 0;

    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::GenerationInfoList generationInfoList;
    int res = getGenerationListByProducerId(producerId,generationInfoList);
    if (res != Result::OK)
      return res;

    T::FileInfoList fileInfoList;
    res = getFileListByProducerId(producerId,0,10000000,fileInfoList);
    if (res != Result::OK)
      return res;

    T::ContentInfoList contentInfoList;
    res = getContentByProducerId(producerId,0,0,10000000,contentInfoList);
    if (res != Result::OK)
      return res;

    std::size_t generationHash = generationInfoList.getHashByProducerId(producerId);
    std::size_t fileHash = fileInfoList.getHashByProducerId(producerId);
    std::size_t contentHash = contentInfoList.getHashByProducerId(producerId);

    std::size_t h = 0;
    boost::hash_combine(h,generationHash);
    boost::hash_combine(h,fileHash);
    boost::hash_combine(h,contentHash);

    hash = h;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::_getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ContentInfoList contentList;
    getContent(0,0,20000000,contentList);

    levelInfoList.clear();

    contentList.getLevelInfoList(levelInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_deleteVirtualContent(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    int result = deleteVirtualFiles(true);

    if (result == Result::OK)
      addEvent(EventType::DELETE_VIRTUAL_CONTENT,0,0,0,0);

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::_updateVirtualContent(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    int result = deleteVirtualFiles(true);

    if (result == Result::OK)
    {
      addEvent(EventType::UPDATE_VIRTUAL_CONTENT,0,0,0,0);
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





// *****************************************************************************************************************************************
// *****************************************************************************************************************************************
// *****************************************************************************************************************************************



int RedisImplementation::deleteProducerById(uint producerId,bool deleteGenerations,bool deleteGeometries,bool deleteFiles,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    if (deleteContent)
      deleteContentByProducerId(producerId);

    if (deleteFiles)
      deleteFileListByProducerId(producerId,false);

    if (deleteGeometries)
      deleteGeometryListByProducerId(producerId,false,false);

    if (deleteGenerations)
      deleteGenerationListByProducerId(producerId,false,false,false);

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZREMRANGEBYSCORE %sproducers %u %u",mTablePrefix.c_str(),producerId,producerId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::deleteProducerListBySourceId(uint sourceId,bool deleteGenerations,bool deleteGeometries,bool deleteFiles,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfoList producerInfoList;
    getProducerListBySourceId(sourceId,producerInfoList);

    uint len = producerInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ProducerInfo *producerInfo = producerInfoList.getProducerInfoByIndex(t);
      deleteProducerById(producerInfo->mProducerId,deleteGenerations,deleteGeometries,deleteFiles,deleteContent);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getProducerById(uint producerId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sproducers %u %u",mTablePrefix.c_str(),producerId,producerId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY  &&  reply->elements == 1)
    {
      producerInfo.setCsv(reply->element[0]->str);
      freeReplyObject(reply);
      if (producerInfo.mProducerId == producerId)
        return Result::OK;
      else
        return Result::DATA_NOT_FOUND;
    }

    freeReplyObject(reply);
    return Result::DATA_NOT_FOUND;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getProducerByName(std::string producerName,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGE %sproducers 0 -1",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        if (getCsvCaseCompare(reply->element[t]->str,1,producerName.c_str()) == 0)
        {
          producerInfo.setCsv(reply->element[t]->str);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getProducerList(T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    producerInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGE %sproducers 0 -1",mTablePrefix.c_str()));
    if (reply == nullptr)
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
      }
    }
    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getProducerListBySourceId(uint sourceId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    producerInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGE %sproducers 0 -1",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        uint sId = getCsvInt64Field(reply->element[t]->str,5);
        if (sId == sourceId)
        {
          T::ProducerInfo *producerInfo = new T::ProducerInfo();
          producerInfo->setCsv(reply->element[t]->str);
          producerInfoList.addProducerInfo(producerInfo);
        }
      }
    }
    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getGenerationById(uint generationId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sgenerations %u %u",mTablePrefix.c_str(),generationId,generationId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY  &&  reply->elements == 1)
    {
      generationInfo.setCsv(reply->element[0]->str);
      freeReplyObject(reply);
      if (generationInfo.mGenerationId == generationId)
        return Result::OK;
      else
        return Result::DATA_NOT_FOUND;
    }

    freeReplyObject(reply);
    return Result::DATA_NOT_FOUND;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getGenerationByName(std::string generationName,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGE %sgenerations 0 -1",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        if (getCsvCaseCompare(reply->element[t]->str,3,generationName.c_str()) == 0)
        {
          generationInfo.setCsv(reply->element[t]->str);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getGenerationList(T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    generationInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGE %sgenerations 0 -1",mTablePrefix.c_str()));
    if (reply == nullptr)
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
      }
    }
    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getGenerationListByGeometryId(T::GeometryId geometryId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    generationInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu",mTablePrefix.c_str(),0,0xFFFFFFFFFFFFFFFF));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    std::set<uint> idList;

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        T::ContentInfo info;
        info.setCsv(reply->element[t]->str);
        if (info.mGeometryId == geometryId)
        {
          //if (idList.find(info.mGenerationId) == idList.end())
          {
            idList.insert(info.mGenerationId);
          }
        }
      }
    }

    freeReplyObject(reply);

    for (auto it=idList.begin(); it!=idList.end(); ++it)
    {
      T::GenerationInfo *generationInfo = new T::GenerationInfo();
      if (getGenerationById(*it,*generationInfo) == Result::OK)
      {
        generationInfoList.addGenerationInfo(generationInfo);
      }
      else
      {
        delete generationInfo;
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getGenerationListByProducerId(uint producerId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    generationInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGE %sgenerations 0 -1",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        uint prodId = getCsvInt64Field(reply->element[t]->str,2);
        if (prodId == producerId)
        {
          T::GenerationInfo *generationInfo = new T::GenerationInfo();
          generationInfo->setCsv(reply->element[t]->str);
          generationInfoList.addGenerationInfo(generationInfo);
        }
      }
    }
    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getGenerationListBySourceId(uint sourceId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    generationInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGE %sgenerations 0 -1",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        uint sId = getCsvInt64Field(reply->element[t]->str,8);
        if (sId == sourceId)
        {
          T::GenerationInfo *generationInfo = new T::GenerationInfo();
          generationInfo->setCsv(reply->element[t]->str);
          generationInfoList.addGenerationInfo(generationInfo);
        }
      }
    }
    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::deleteGenerationById(uint generationId,bool deleteGeometries,bool deleteFiles,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    if (deleteContent)
      deleteContentByGenerationId(generationId);

    if (deleteFiles)
      deleteFileListByGenerationId(generationId,false);

    if (deleteGeometries)
      deleteGeometryListByGenerationId(generationId,false,false);

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZREMRANGEBYSCORE %sgenerations %u %u",mTablePrefix.c_str(),generationId,generationId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::deleteGenerationListByProducerId(uint producerId,bool deleteGeometries,bool deleteFiles,bool deleteContent)
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
      deleteGenerationById(generationInfo->mGenerationId,deleteGeometries,deleteFiles,deleteContent);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::deleteGenerationListBySourceId(uint sourceId,bool deleteGeometries,bool deleteFiles,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GenerationInfoList generationInfoList;
    getGenerationListBySourceId(sourceId,generationInfoList);

    uint len = generationInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::GenerationInfo *generationInfo = generationInfoList.getGenerationInfoByIndex(t);
      deleteGenerationById(generationInfo->mGenerationId,deleteGeometries,deleteFiles,deleteContent);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZREMRANGEBYSCORE %sfiles %u %u",mTablePrefix.c_str(),fileId,fileId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    getFileListByGenerationId(generationId,0,1000000000,fileInfoList);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::deleteFileListByGenerationIdList(std::set<uint>& generationIdList,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    if (deleteContent)
      deleteContentByGenerationIdList(generationIdList);

    T::FileInfoList fileInfoList;
    getFileListByGenerationIdList(generationIdList,0,1000000000,fileInfoList);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getFileById(uint fileId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sfiles %u %u",mTablePrefix.c_str(),fileId,fileId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY  &&  reply->elements == 1)
    {
      fileInfo.setCsv(reply->element[0]->str);
      freeReplyObject(reply);

      if (fileInfo.mFileId == fileId)
        return Result::OK;
      else
        return Result::DATA_NOT_FOUND;

      return Result::OK;
    }

    freeReplyObject(reply);
    return Result::DATA_NOT_FOUND;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getFileList(uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sfiles %u %u LIMIT 0 %u",mTablePrefix.c_str(),startFileId,0xFFFFFFFF,maxRecords));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        uint fileId = getCsvInt64Field(reply->element[t]->str,0);
        if (fileId >= startFileId)
        {
          T::FileInfo *fileInfo = new T::FileInfo();
          fileInfo->setCsv(reply->element[t]->str);
          fileInfoList.addFileInfo(fileInfo);
        }
      }
    }
    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getFileListByGenerationId(uint generationId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint max = (uint)abs(maxRecords);
    uint prevFileId = 0xFFFFFFFF;

    while (startFileId != prevFileId)
    {
      prevFileId = startFileId;

      redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sfiles %u %u LIMIT 0 10000",mTablePrefix.c_str(),startFileId,0xFFFFFFFF));
      if (reply == nullptr)
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
          uint fileId = getCsvInt64Field(reply->element[t]->str,0);
          if (fileId >= startFileId)
          {
            startFileId = fileId + 1;
            uint genId = getCsvInt64Field(reply->element[t]->str,4);
            if (genId == generationId)
            {
              T::FileInfo *fileInfo = new T::FileInfo();
              fileInfo->setCsv(reply->element[t]->str);
              fileInfoList.addFileInfo(fileInfo);

              if (fileInfoList.getLength() == max)
              {
                freeReplyObject(reply);
                return Result::OK;
              }
            }
          }
        }
        freeReplyObject(reply);
      }
      else
      {
        freeReplyObject(reply);
        return Result::OK;
      }
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getFileListByGenerationIdList(std::set<uint>& generationIdList,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint max = (uint)abs(maxRecords);
    uint prevFileId = 0xFFFFFFFF;

    while (startFileId != prevFileId)
    {
      prevFileId = startFileId;

      redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sfiles %u %u LIMIT 0 10000",mTablePrefix.c_str(),startFileId,0xFFFFFFFF));
      if (reply == nullptr)
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
          uint fileId = getCsvInt64Field(reply->element[t]->str,0);
          if (fileId >= startFileId)
          {
            startFileId = fileId + 1;

            uint genId = getCsvInt64Field(reply->element[t]->str,4);
            if (generationIdList.find(genId) != generationIdList.end())
            {
              T::FileInfo *fileInfo = new T::FileInfo();
              fileInfo->setCsv(reply->element[t]->str);
              fileInfoList.addFileInfo(fileInfo);
              if (fileInfoList.getLength() == max)
              {
                freeReplyObject(reply);
                return Result::OK;
              }
            }
          }
        }
        freeReplyObject(reply);
      }
      else
      {
        freeReplyObject(reply);
        return Result::OK;
      }
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    getFileListByProducerId(producerId,0,1000000000,fileInfoList);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::deleteFileListBySourceId(uint sourceId,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    if (deleteContent)
      deleteContentBySourceId(sourceId);

    T::FileInfoList fileInfoList;
    getFileListBySourceId(sourceId,0,1000000000,fileInfoList);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}









int RedisImplementation::getFileListByProducerId(uint producerId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint max = (uint)abs(maxRecords);
    uint prevFileId = 0xFFFFFFFF;

    while (startFileId != prevFileId)
    {
      prevFileId = startFileId;

      redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sfiles %u %u LIMIT 0 10000",mTablePrefix.c_str(),startFileId,0xFFFFFFFF));
      if (reply == nullptr)
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
          uint fileId = getCsvInt64Field(reply->element[t]->str,0);
          if (fileId >= startFileId)
          {
            startFileId = fileId + 1;

            uint prodId = getCsvInt64Field(reply->element[t]->str,3);
            if (prodId == producerId)
            {
              T::FileInfo *fileInfo = new T::FileInfo();
              fileInfo->setCsv(reply->element[t]->str);
              fileInfoList.addFileInfo(fileInfo);

              if (fileInfoList.getLength() == max)
              {
                freeReplyObject(reply);
                return Result::OK;
              }
            }
          }
        }
        freeReplyObject(reply);
      }
      else
      {
        freeReplyObject(reply);
        return Result::OK;
      }
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getFileListBySourceId(uint sourceId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint max = (uint)abs(maxRecords);
    uint prevFileId = 0xFFFFFFFF;

    while (startFileId != prevFileId)
    {
      prevFileId = startFileId;

      redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sfiles %u %u LIMIT 0 10000",mTablePrefix.c_str(),startFileId,0xFFFFFFFF));
      if (reply == nullptr)
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
          uint fileId = getCsvInt64Field(reply->element[t]->str,0);
          if (fileId >= startFileId)
          {
            startFileId = fileId + 1;

            uint sId = getCsvInt64Field(reply->element[t]->str,7);
            if (sId == sourceId)
            {
              T::FileInfo *fileInfo = new T::FileInfo();
              fileInfo->setCsv(reply->element[t]->str);
              fileInfoList.addFileInfo(fileInfo);

              if (fileInfoList.getLength() == max)
              {
                freeReplyObject(reply);
                return Result::OK;
              }
            }
          }
        }
        freeReplyObject(reply);
      }
      else
      {
        freeReplyObject(reply);
        return Result::OK;
      }
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getVirtualFiles(uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint max = (uint)abs(maxRecords);
    uint prevFileId = 0xFFFFFFFF;

    while (startFileId != prevFileId)
    {
      prevFileId = startFileId;

      redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sfiles %u %u LIMIT 0 10000",mTablePrefix.c_str(),startFileId,0xFFFFFFFF));
      if (reply == nullptr)
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
          uint fileId = getCsvInt64Field(reply->element[t]->str,0);
          if (fileId >= startFileId)
          {
            startFileId = fileId + 1;

            uint type = getCsvInt64Field(reply->element[t]->str,1);
            uint flags = getCsvInt64Field(reply->element[t]->str,6);

            if (type == T::FileTypeValue::Virtual || (flags & T::FileInfo::Flags::VirtualContent) != 0)
            {
              T::FileInfo *fileInfo = new T::FileInfo();
              fileInfo->setCsv(reply->element[t]->str);
              fileInfoList.addFileInfo(fileInfo);

              if (fileInfoList.getLength() == max)
              {
                freeReplyObject(reply);
                return Result::OK;
              }
            }
          }
        }
        freeReplyObject(reply);
      }
      else
      {
        freeReplyObject(reply);
        return Result::OK;
      }
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::deleteVirtualFiles(bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    if (deleteContent)
      removeVirtualContent();

    T::FileInfoList fileInfoList;
    getVirtualFiles(0,1000000000,fileInfoList);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::deleteGeometryById(uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,bool deleteFiles,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long id = getGeometryKey(generationId,geometryId,levelId);

    if (deleteContent)
      deleteContentByGeometry(generationId,geometryId,levelId);

    // ### Cannot delete files if files contain multiple geometries
    //if (deleteFiles)
    //  deleteFileListByGeneationAndGeometryId(generationId,geometryId);

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZREMRANGEBYSCORE %sgeometries %llu %llu",mTablePrefix.c_str(),id,id));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::deleteGeometryListByGenerationId(uint generationId,bool deleteFiles,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    if (deleteContent)
      deleteContentByGenerationId(generationId);

    // ### Cannot delete files if files contain multiple geometries
    //if (deleteFiles)
    //  deleteFileListByGenerationId(generationId,false);

    T::GeometryInfoList geometryInfoList;
    getGeometryListByGenerationId(generationId,geometryInfoList);
    uint len = geometryInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::GeometryInfo *geometryInfo = geometryInfoList.getGeometryInfoByIndex(t);
      deleteGeometryById(geometryInfo->mGenerationId,geometryInfo->mGeometryId,geometryInfo->mLevelId,false,false);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::deleteGeometryListByProducerId(uint producerId,bool deleteFiles,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    if (deleteContent)
      deleteContentByProducerId(producerId);

    // ### Cannot delete files if files contain multiple geometries
    //if (deleteFiles)
    //  deleteFileListByProducerId(producerId,false);

    T::GeometryInfoList geometryInfoList;
    getGeometryListByProducerId(producerId,geometryInfoList);
    uint len = geometryInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::GeometryInfo *geometryInfo = geometryInfoList.getGeometryInfoByIndex(t);
      deleteGeometryById(geometryInfo->mGenerationId,geometryInfo->mGeometryId,geometryInfo->mLevelId,false,false);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::deleteGeometryListBySourceId(uint sourceId,bool deleteFiles,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    if (deleteContent)
      deleteContentBySourceId(sourceId);

    // ### Cannot delete files if files contain multiple geometries
    //if (deleteFiles)
    //  deleteFileListBySourceId(sourceId,false);

    T::GeometryInfoList geometryInfoList;
    getGeometryListBySourceId(sourceId,geometryInfoList);
    uint len = geometryInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::GeometryInfo *geometryInfo = geometryInfoList.getGeometryInfoByIndex(t);
      deleteGeometryById(geometryInfo->mGenerationId,geometryInfo->mGeometryId,geometryInfo->mLevelId,false,false);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::deleteGeometryListByGenerationIdList(std::set<uint>& generationIdList,bool deleteFiles,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::GeometryInfoList geometryInfoList;
    getGeometryListByGenerationIdList(generationIdList,geometryInfoList);
    uint len = geometryInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::GeometryInfo *geometryInfo = geometryInfoList.getGeometryInfoByIndex(t);
      deleteGeometryById(geometryInfo->mGenerationId,geometryInfo->mGeometryId,geometryInfo->mLevelId,deleteFiles,deleteContent);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getGeometryById(uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long id = getGeometryKey(generationId,geometryId,levelId);

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sgeometries %llu %llu",mTablePrefix.c_str(),id,id));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY  &&  reply->elements > 0)
    {
      for (uint t=0; t<reply->elements; t++)
      {
        uint fGeneration = getCsvInt64Field(reply->element[t]->str,0);
        T::GeometryId fGeom = getCsvInt64Field(reply->element[t]->str,1);
        T::ParamLevelId fLevelId = getCsvInt64Field(reply->element[t]->str,2);
        if (fGeneration == generationId  &&  fGeom == geometryId && fLevelId == levelId)
        {
          geometryInfo.setCsv(reply->element[t]->str);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::getGeometryList(T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    geometryInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long startId = 0;
    unsigned long long endId = 0xFFFFFFFFFFFFFFFF;


    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sgeometries %llu %llu",mTablePrefix.c_str(),startId,endId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        T::GeometryInfo *geometryInfo = new T::GeometryInfo();
        geometryInfo->setCsv(reply->element[t]->str);
        geometryInfoList.addGeometryInfo(geometryInfo);
      }
    }
    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::getGeometryListByProducerId(uint producerId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    geometryInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long startId = 0;
    unsigned long long endId = 0xFFFFFFFFFFFFFFFF;


    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sgeometries %llu %llu",mTablePrefix.c_str(),startId,endId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        uint prodId = getCsvInt64Field(reply->element[t]->str,3);

        if (prodId == producerId)
        {
          T::GeometryInfo *geometryInfo = new T::GeometryInfo();
          geometryInfo->setCsv(reply->element[t]->str);
          geometryInfoList.addGeometryInfo(geometryInfo);
        }
      }
    }
    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getGeometryListByGenerationId(uint generationId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    geometryInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long startId = getGeometryKey(generationId,0,0);
    unsigned long long endId = getGeometryKey(generationId,0x00FFFFFF,0xFF);

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sgeometries %llu %llu",mTablePrefix.c_str(),startId,endId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        uint genId = getCsvInt64Field(reply->element[t]->str,0);
        if (genId == generationId)
        {
          T::GeometryInfo *geometryInfo = new T::GeometryInfo();
          geometryInfo->setCsv(reply->element[t]->str);
          geometryInfoList.addGeometryInfo(geometryInfo);
        }
      }
    }
    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::getGeometryListBySourceId(uint sourceId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    geometryInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long startId = 0;
    unsigned long long endId = 0xFFFFFFFFFFFFFFFF;


    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sgeometries %llu %llu",mTablePrefix.c_str(),startId,endId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        uint sId = getCsvInt64Field(reply->element[t]->str,4);

        if (sId == sourceId)
        {
          T::GeometryInfo *geometryInfo = new T::GeometryInfo();
          geometryInfo->setCsv(reply->element[t]->str);
          geometryInfoList.addGeometryInfo(geometryInfo);
        }
      }
    }
    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getGeometryListByGenerationIdList(std::set<uint>& generationIdList,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    geometryInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long startId = 0;
    unsigned long long endId = 0xFFFFFFFFFFFFFFFF;


    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %sgeometries %llu %llu",mTablePrefix.c_str(),startId,endId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        uint genId = getCsvInt64Field(reply->element[t]->str,0);
        if (generationIdList.find(genId) != generationIdList.end())
        {
          T::GeometryInfo *geometryInfo = new T::GeometryInfo();
          geometryInfo->setCsv(reply->element[t]->str);
          geometryInfoList.addGeometryInfo(geometryInfo);
        }
      }
    }
    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::deleteContent(uint fileId,uint messageIndex)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long id = getContentKey(fileId,messageIndex);

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZREMRANGEBYSCORE %scontent %llu %llu",mTablePrefix.c_str(),id,id));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    getContentByProducerId(producerId,0,0,1000000000,contentInfoList);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    getContentByGenerationId(generationId,0,0,1000000000,contentInfoList);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::deleteContentByGeometry(uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ContentInfoList contentInfoList;
    getContentByGenerationId(generationId,0,0,1000000000,contentInfoList);
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
      if (contentInfo->mGeometryId == geometryId  &&  (levelId <= 0 || contentInfo->mFmiParameterLevelId == levelId))
        deleteContent(contentInfo->mFileId,contentInfo->mMessageIndex);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::deleteContentByGenerationIdList(std::set<uint>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ContentInfoList contentInfoList;
    getContentByGenerationIdList(generationIdList,0,0,1000000000,contentInfoList);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::deleteContentBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ContentInfoList contentInfoList;
    getContentBySourceId(sourceId,0,0,1000000000,contentInfoList);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::removeVirtualContent()
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ContentInfoList contentInfoList;
    getVirtualContent(0,0,1000000000,contentInfoList);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::setContent(T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long id = getContentKey(contentInfo.mFileId,contentInfo.mMessageIndex);

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZREMRANGEBYSCORE %scontent %llu %llu",mTablePrefix.c_str(),id,id));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %scontent %llu %s",mTablePrefix.c_str(),id,contentInfo.getCsv().c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getContent(uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long id = getContentKey(fileId,messageIndex);

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu",mTablePrefix.c_str(),id,id));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY  &&  reply->elements > 0)
    {
      for (uint t=0; t<reply->elements; t++)
      {
        uint fId = getCsvInt64Field(reply->element[t]->str,0);
        uint mIndex = getCsvInt64Field(reply->element[t]->str,1);
        if (fId == fileId  &&  mIndex == messageIndex)
        {
          contentInfo.setCsv(reply->element[t]->str);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getContent(uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint max = (uint)abs(maxRecords);
    unsigned long long startId = getContentKey(startFileId,startMessageIndex);

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu LIMIT 0 %u",mTablePrefix.c_str(),startId,0xFFFFFFFFFFFFFFFF,max));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        uint fileId = getCsvInt64Field(reply->element[t]->str,0);
        uint messageIndex = getCsvInt64Field(reply->element[t]->str,1);

        if (fileId > startFileId  ||  (fileId == startFileId   &&  messageIndex >= startMessageIndex))
        {
          T::ContentInfo *contentInfo = new T::ContentInfo();
          contentInfo->setCsv(reply->element[t]->str);
          contentInfoList.addContentInfo(contentInfo);
        }
      }
    }
    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getGenerationTimeAndGeometryList(std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    list.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint startFileId = 0;
    uint startMessageIndex = 0;
    unsigned long long startId = getContentKey(startFileId,startMessageIndex);
    unsigned long long prevStartId = 0xFFFFFFFFFFFFFFFF;

    while (startId != prevStartId)
    {
      prevStartId = startId;

      redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu LIMIT 0 10000",mTablePrefix.c_str(),startId,0xFFFFFFFFFFFFFFFF));
      if (reply == nullptr)
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
          T::ContentInfo contentInfo;
          contentInfo.setCsv(reply->element[t]->str);

          char st[200];
          sprintf(st,"%u;%u;%u;%d;%d;%s;%ld;%ld;",contentInfo.mSourceId,contentInfo.mGenerationId,contentInfo.mGeometryId,contentInfo.mForecastType,contentInfo.mForecastNumber,contentInfo.getForecastTime(),contentInfo.mModificationTime,contentInfo.mDeletionTime);
          std::string str = st;

          if (list.find(str) == list.end())
            list.insert(str);

          startFileId = contentInfo.mFileId;
          startMessageIndex = contentInfo.mMessageIndex + 1;
        }
        freeReplyObject(reply);
      }
      else
      {
        freeReplyObject(reply);
        return Result::OK;
      }
      startId = getContentKey(startFileId,startMessageIndex);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getContentByGenerationId(uint generationId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint max = (uint)abs(maxRecords);
    unsigned long long startId = getContentKey(startFileId,startMessageIndex);
    unsigned long long prevStartId = 0xFFFFFFFFFFFFFFFF;

    while (startId != prevStartId)
    {
      prevStartId = startId;

      redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu LIMIT 0 10000",mTablePrefix.c_str(),startId,0xFFFFFFFFFFFFFFFF));
      if (reply == nullptr)
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
          uint fileId = getCsvInt64Field(reply->element[t]->str,0);
          uint messageIndex = getCsvInt64Field(reply->element[t]->str,1);

          if (fileId > startFileId  ||  (fileId == startFileId   &&  messageIndex >= startMessageIndex))
          {
            startFileId = fileId;
            startMessageIndex = messageIndex + 1;

            uint genId = getCsvInt64Field(reply->element[t]->str,6);

            if (genId == generationId)
            {
              T::ContentInfo *contentInfo = new T::ContentInfo();
              contentInfo->setCsv(reply->element[t]->str);

              contentInfoList.addContentInfo(contentInfo);

              if (contentInfoList.getLength() == max)
              {
                freeReplyObject(reply);
                return Result::OK;
              }
            }
          }
        }
        freeReplyObject(reply);
      }
      else
      {
        freeReplyObject(reply);
        return Result::OK;
      }
      startId = getContentKey(startFileId,startMessageIndex);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getContentByGenerationIdList(std::set<uint>& generationIdList,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint max = (uint)abs(maxRecords);
    unsigned long long startId = getContentKey(startFileId,startMessageIndex);
    unsigned long long prevStartId = 0xFFFFFFFFFFFFFFFF;

    while (startId != prevStartId)
    {
      prevStartId = startId;

      redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu LIMIT 0 10000",mTablePrefix.c_str(),startId,0xFFFFFFFFFFFFFFFF));
      if (reply == nullptr)
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
          uint fileId = getCsvInt64Field(reply->element[t]->str,0);
          uint messageIndex = getCsvInt64Field(reply->element[t]->str,1);

          if (fileId > startFileId  ||  (fileId == startFileId   &&  messageIndex >= startMessageIndex))
          {
            startFileId = fileId;
            startMessageIndex = messageIndex + 1;

            uint genId = getCsvInt64Field(reply->element[t]->str,6);

            if (generationIdList.find(genId) != generationIdList.end())
            {
              T::ContentInfo *contentInfo = new T::ContentInfo();
              contentInfo->setCsv(reply->element[t]->str);

              contentInfoList.addContentInfo(contentInfo);

              if (contentInfoList.getLength() == max)
              {
                freeReplyObject(reply);
                return Result::OK;
              }
            }
          }
        }
        freeReplyObject(reply);
      }
      else
      {
        freeReplyObject(reply);
        return Result::OK;
      }
      startId = getContentKey(startFileId,startMessageIndex);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getContentByRequestCounterKey(ulonglong key,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint startFileId = 0;
    uint startMessageIndex = 0;

    unsigned long long startId = 0;
    unsigned long long prevStartId = 0xFFFFFFFFFFFFFFFF;

    while (startId != prevStartId)
    {
      prevStartId = startId;

      redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu LIMIT 0 10000",mTablePrefix.c_str(),startId,0xFFFFFFFFFFFFFFFF));
      if (reply == nullptr)
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

          if (contentInfo->getRequestCounterKey() == key)
          {
            startFileId = contentInfo->mFileId;
            startMessageIndex = contentInfo->mMessageIndex + 1;

            contentInfoList.addContentInfo(contentInfo);
          }
          else
          {
            delete contentInfo;
          }
        }
        freeReplyObject(reply);
      }
      else
      {
        freeReplyObject(reply);
        return Result::OK;
      }
      startId = getContentKey(startFileId,startMessageIndex);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getVirtualContent(uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint max = (uint)abs(maxRecords);
    unsigned long long startId = getContentKey(startFileId,startMessageIndex);
    unsigned long long prevStartId = 0xFFFFFFFFFFFFFFFF;

    while (startId != prevStartId)
    {
      prevStartId = startId;

      redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu LIMIT 0 10000",mTablePrefix.c_str(),startId,0xFFFFFFFFFFFFFFFF));
      if (reply == nullptr)
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
          uint fileId = getCsvInt64Field(reply->element[t]->str,0);
          uint messageIndex = getCsvInt64Field(reply->element[t]->str,1);

          if (fileId > startFileId  ||  (fileId == startFileId   &&  messageIndex >= startMessageIndex))
          {
            startFileId = fileId;
            startMessageIndex = messageIndex + 1;

            uint type = getCsvInt64Field(reply->element[t]->str,2);
            uint flags = getCsvInt64Field(reply->element[t]->str,25);

            if (type == T::FileTypeValue::Virtual ||  (flags & T::ContentInfo::Flags::VirtualContent) != 0)
            {
              T::ContentInfo *contentInfo = new T::ContentInfo();
              contentInfo->setCsv(reply->element[t]->str);
              contentInfoList.addContentInfo(contentInfo);

              if (contentInfoList.getLength() == max)
              {
                freeReplyObject(reply);
                return Result::OK;
              }
            }
          }
        }
        freeReplyObject(reply);
      }
      else
      {
        freeReplyObject(reply);
        return Result::OK;
      }
      startId = getContentKey(startFileId,startMessageIndex);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getContentByParameterId(T::ParamKeyType parameterKeyType,const std::string& parameterKey,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    T::FmiParamId fmiId = 0;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        fmiId = toUInt32(parameterKey);
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByFmiName(parameterKey);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseName(parameterKey);
        break;

      case T::ParamKeyTypeValue::NETCDF_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNetCdfName(parameterKey);
        break;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"FMI-",4) == 0)
      fmiId = toUInt32(parameterKey.c_str()+4);
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"NB-",3) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey.c_str()+3));
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"GRIB-",5) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey.c_str()+5));
    else
    if (fmiId == 0)
      return Result::OK;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu",mTablePrefix.c_str(),0,0xFFFFFFFFFFFFFFFF));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        uint id = getCsvInt64Field(reply->element[t]->str,9);
        if (id == fmiId)
        {
          T::ContentInfo *info = new T::ContentInfo();
          info->setCsv(reply->element[t]->str);
          contentInfoList.addContentInfo(info);
        }
      }
    }

    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getContentByParameterIdAndTimeRange(T::ParamKeyType parameterKeyType,const std::string& parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    T::FmiParamId fmiId = 0;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        fmiId = toUInt32(parameterKey);
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByFmiName(parameterKey);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseName(parameterKey);
        break;

      case T::ParamKeyTypeValue::NETCDF_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNetCdfName(parameterKey);
        break;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"FMI-",4) == 0)
      fmiId = toUInt32(parameterKey.c_str()+4);
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"NB-",3) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey.c_str()+3));
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"GRIB-",5) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey.c_str()+5));
    else
    if (fmiId == 0)
      return Result::OK;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu",mTablePrefix.c_str(),0,0xFFFFFFFFFFFFFFFF));
    if (reply == nullptr)
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

        if (parameterLevelId < 0 || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
        {
          if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
          {
            if (info->mFmiParameterId == fmiId &&  info->mForecastTimeUTC >= startTime  &&  info->mForecastTimeUTC <= endTime)
            {
              if (parameterLevelId < 0 ||  info->mFmiParameterLevelId == parameterLevelId)
              {
                contentInfoList.addContentInfo(info);
                info = nullptr;
              }
            }
          }
        }

        if (info != nullptr)
          delete info;
      }
    }

    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getContentByParameterIdAndGeneration(uint generationId,T::ParamKeyType parameterKeyType,const std::string& parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    T::FmiParamId fmiId = 0;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        fmiId = toUInt32(parameterKey);
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByFmiName(parameterKey);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseName(parameterKey);
        break;

      case T::ParamKeyTypeValue::NETCDF_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNetCdfName(parameterKey);
        break;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"FMI-",4) == 0)
      fmiId = toUInt32(parameterKey.c_str()+4);
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"NB-",3) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey.c_str()+3));
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"GRIB-",5) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey.c_str()+5));
    else
    if (fmiId == 0)
      return Result::OK;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu",mTablePrefix.c_str(),0,0xFFFFFFFFFFFFFFFF));
    if (reply == nullptr)
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
          if (parameterLevelId < 0 || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
          {
            if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
            {
              if (geometryId < 0  ||  info->mGeometryId == geometryId)
              {
                if (info->mFmiParameterId == fmiId &&  info->mForecastTimeUTC >= startTime  &&  info->mForecastTimeUTC <= endTime)
                {
                  if (parameterLevelId < 0 || info->mFmiParameterLevelId == parameterLevelId)
                  {
                    contentInfoList.addContentInfo(info);
                    info = nullptr;
                  }
                }
              }
            }
          }
        }

        if (info != nullptr)
          delete info;
      }
    }

    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::getContentByParameterIdAndProducer(uint producerId,T::ParamKeyType parameterKeyType,const std::string& parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    T::FmiParamId fmiId = 0;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        fmiId = toUInt32(parameterKey);
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByFmiName(parameterKey);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseName(parameterKey);
        break;

      case T::ParamKeyTypeValue::NETCDF_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNetCdfName(parameterKey);
        break;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"FMI-",4) == 0)
      fmiId = toUInt32(parameterKey.c_str()+4);
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"NB-",3) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey.c_str()+3));
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"GRIB-",5) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey.c_str()+5));
    else
    if (fmiId == 0)
      return Result::OK;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu",mTablePrefix.c_str(),0,0xFFFFFFFFFFFFFFFF));
    if (reply == nullptr)
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
          if (parameterLevelId < 0 || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
          {
            if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
            {
              if (geometryId < 0  ||  info->mGeometryId == geometryId)
              {
                if (info->mFmiParameterId == fmiId &&  info->mForecastTimeUTC >= startTime  &&  info->mForecastTimeUTC <= endTime)
                {
                  if (parameterLevelId < 0 || info->mFmiParameterLevelId == parameterLevelId)
                  {
                    contentInfoList.addContentInfo(info);
                    info = nullptr;
                  }
                }
              }
            }
          }
        }

        if (info != nullptr)
          delete info;
      }
    }

    freeReplyObject(reply);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int RedisImplementation::getContentByGenerationIdAndTimeRange(uint generationId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu",mTablePrefix.c_str(),0,0xFFFFFFFFFFFFFFFF));
    if (reply == nullptr)
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

        if (contentInfo->mGenerationId == generationId  &&  contentInfo->mForecastTimeUTC >= startTime  &&  contentInfo->mForecastTimeUTC <= endTime)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getContentByForecastTimeList(std::vector<T::ForecastTime>& forecastTimeList,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    std::set<std::string> searchList;

    char tmp[200];
    for (auto it = forecastTimeList.begin(); it != forecastTimeList.end(); ++it)
    {
      sprintf(tmp,"%d;%d;%d;%d;%s",it->mGenerationId,it->mGeometryId,it->mForecastType,it->mForecastNumber,it->mForecastTime.c_str());
      std::string st = tmp;
      if (searchList.find(st) == searchList.end())
        searchList.insert(st);
    }


    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu",mTablePrefix.c_str(),0,0xFFFFFFFFFFFFFFFF));
    if (reply == nullptr)
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

        sprintf(tmp,"%d;%d;%d;%d;%s",contentInfo->mGenerationId,contentInfo->mGeometryId,contentInfo->mForecastType,contentInfo->mForecastNumber,contentInfo->getForecastTime());
        if (searchList.find(tmp) != searchList.end())
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getContentByProducerId(uint producerId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint max = (uint)abs(maxRecords);
    unsigned long long startId = getContentKey(startFileId,startMessageIndex);
    unsigned long long prevStartId = 0xFFFFFFFFFFFFFFFF;

    while (startId != prevStartId)
    {
      prevStartId = startId;

      redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu LIMIT 0 10000",mTablePrefix.c_str(),startId,0xFFFFFFFFFFFFFFFF));
      if (reply == nullptr)
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
          uint fileId = getCsvInt64Field(reply->element[t]->str,0);
          uint messageIndex = getCsvInt64Field(reply->element[t]->str,1);

          if (fileId > startFileId  ||  (fileId == startFileId   &&  messageIndex >= startMessageIndex))
          {
            startFileId = fileId;
            startMessageIndex = messageIndex + 1;

            uint prodId = getCsvInt64Field(reply->element[t]->str,5);

            if (producerId == prodId)
            {
              T::ContentInfo *contentInfo = new T::ContentInfo();
              contentInfo->setCsv(reply->element[t]->str);

              contentInfoList.addContentInfo(contentInfo);

              if (contentInfoList.getLength() == max)
              {
                freeReplyObject(reply);
                return Result::OK;
              }
            }
          }
        }
        freeReplyObject(reply);
      }
      else
      {
        freeReplyObject(reply);
        return Result::OK;
      }
      startId = getContentKey(startFileId,startMessageIndex);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getContentBySourceId(uint sourceId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint max = (uint)abs(maxRecords);
    unsigned long long startId = getContentKey(startFileId,startMessageIndex);
    unsigned long long prevStartId = 0xFFFFFFFFFFFFFFFF;

    while (startId != prevStartId)
    {
      prevStartId = startId;

      redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu LIMIT 0 10000",mTablePrefix.c_str(),startId,0xFFFFFFFFFFFFFFFF));
      if (reply == nullptr)
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
          uint fileId = getCsvInt64Field(reply->element[t]->str,0);
          uint messageIndex = getCsvInt64Field(reply->element[t]->str,1);

          if (fileId > startFileId  ||  (fileId == startFileId   &&  messageIndex >= startMessageIndex))
          {
            startFileId = fileId;
            startMessageIndex = messageIndex + 1;

            uint sId = getCsvInt64Field(reply->element[t]->str,26);
            if (sId == sourceId)
            {
              T::ContentInfo *contentInfo = new T::ContentInfo();
              contentInfo->setCsv(reply->element[t]->str);
              contentInfoList.addContentInfo(contentInfo);

              if (contentInfoList.getLength() == max)
              {
                freeReplyObject(reply);
                return Result::OK;
              }
            }
          }
        }
        freeReplyObject(reply);
      }
      else
      {
        freeReplyObject(reply);
        return Result::OK;
      }
      startId = getContentKey(startFileId,startMessageIndex);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::getContentByFileId(uint fileId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long startId = getContentKey(fileId,0);
    unsigned long long endId = getContentKey(fileId,0xFFFFF);

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZRANGEBYSCORE %scontent %llu %llu",mTablePrefix.c_str(),startId,endId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t++)
      {
        uint fId = getCsvInt64Field(reply->element[t]->str,0);
        if (fId == fileId)
        {
          T::ContentInfo *contentInfo = new T::ContentInfo();
          contentInfo->setCsv(reply->element[t]->str);
          contentInfoList.addContentInfo(contentInfo);
        }
      }
      freeReplyObject(reply);
    }
    else
    {
      freeReplyObject(reply);
      return Result::OK;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





T::EventId RedisImplementation::addEvent(uint eventType,uint id1,uint id2,uint id3,unsigned long long flags)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"INCR %seventCounter",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    T::EventId eventId = (T::EventId)reply->integer;
    freeReplyObject(reply);

    T::EventInfo eventInfo(mStartTime,eventId,eventType,id1,id2,id3,flags);

    reply = static_cast<redisReply*>(redisCommand(mContext,"ZADD %sevents %llu %s",mTablePrefix.c_str(),eventId,eventInfo.getCsv().c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    if ((eventId % 10000) == 0)
      truncateEvents();

    return eventId;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void RedisImplementation::truncateEvents()
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return;

    uint maxEvents = 3000000;
    uint count = 0;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"ZCOUNT %sevents 0 %llu",mTablePrefix.c_str(),0xFFFFFFFFFFFFFFFF));
    if (reply == nullptr)
    {
      closeConnection();
      return;
    }

    if (reply->str != nullptr)
      count = toInt64(reply->str);
    else
      count = reply->integer;

    freeReplyObject(reply);

    if (count > maxEvents)
    {
      reply = static_cast<redisReply*>(redisCommand(mContext,"ZREMRANGEBYRANK %sevents 0 %u",mTablePrefix.c_str(),count-maxEvents+10000));
      if (reply == nullptr)
      {
        closeConnection();
        return;
      }
      freeReplyObject(reply);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::addFilename(std::string filename,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"HSET %sfilenames %s %u",mTablePrefix.c_str(),filename.c_str(),fileId));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint RedisImplementation::getFileId(std::string filename)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return 0;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"HGET %sfilenames %s",mTablePrefix.c_str(),filename.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return 0;
    }

    uint id = 0;
    if (reply->str != nullptr)
      id = toInt64(reply->str);

    freeReplyObject(reply);
    return id;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int RedisImplementation::deleteFilename(std::string filename)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"HDEL %sfilenames %s",mTablePrefix.c_str(),filename.c_str()));
    if (reply == nullptr)
    {
      closeConnection();
      return Result::PERMANENT_STORAGE_ERROR;
    }

    freeReplyObject(reply);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






void RedisImplementation::getFilenames(std::map<std::string,uint>& fileList)
{
  FUNCTION_TRACE
  try
  {
    RedisProcessLock redisProcessLock(FUNCTION_NAME,__LINE__,this);
    if (!isConnectionValid())
      return;


    redisReply *reply = static_cast<redisReply*>(redisCommand(mContext,"HGETALL %sfilenames",mTablePrefix.c_str()));
    if (reply == nullptr)
    {
       closeConnection();
      return;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
      for (uint t = 0; t < reply->elements; t=t+2)
      {
        if (reply->element[t] &&  reply->element[t]->str  &&  reply->element[t+1] &&  reply->element[t+1]->str)
        {
          std::string name = reply->element[t]->str;
          uint id = toInt64(reply->element[t+1]->str);
          fileList.insert(std::pair<std::string,uint>(name,id));
        }
      }
    }
    freeReplyObject(reply);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void RedisImplementation::syncFilenames()
{
  FUNCTION_TRACE
  try
  {
    std::map<std::string,uint> fileList;
    getFilenames(fileList);

    for (auto it = fileList.begin(); it != fileList.end(); ++it)
    {
      T::FileInfo fileInfo;
      if (getFileById(it->second,fileInfo) != Result::OK)
      {
        deleteFilename(it->first);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}


}
}
