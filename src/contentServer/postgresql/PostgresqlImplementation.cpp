#include "PostgresqlImplementation.h"
#include <macgyver/Exception.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/common/ShowFunction.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/identification/GridDef.h>
#include <macgyver/Hash.h>
#include <libpq-fe.h>

#define FUNCTION_TRACE FUNCTION_TRACE_OFF



namespace SmartMet
{
namespace ContentServer
{



PostgresqlImplementation::PostgresqlImplementation()
{
  FUNCTION_TRACE
  try
  {
    mImplementationType = Implementation::Postgres;
    mStartTime = time(nullptr);
    mShutdownRequested = false;
    mConnection = nullptr;
    mEventTruncateCheckTime = 0;
    mTableCreationAllowed = false;
    mContentChangeTime = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





PostgresqlImplementation::~PostgresqlImplementation()
{
  FUNCTION_TRACE
  try
  {
    if (mConnection != nullptr)
    {
      PQfinish(mConnection);
      mConnection = nullptr;
    }
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void PostgresqlImplementation::init(const char *primaryConnectionString,const char *secondaryConnectionString,bool tableCreationAllowed)
{
  FUNCTION_TRACE
  try
  {
    mPrimaryConnectionString = primaryConnectionString;
    mSecondaryConnectionString = secondaryConnectionString;
    mTableCreationAllowed = tableCreationAllowed;

    openConnection();

    //mConnection = PQconnectdb(primaryConnectionString);
    if (mConnection == nullptr)
    {
      Fmi::Exception exception(BCP,"Cannot connect to the database!");
      throw exception;
    }

    createTables();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






void PostgresqlImplementation::createTables()
{
  FUNCTION_TRACE
  try
  {
    if (!mTableCreationAllowed)
      return;

    AutoThreadLock lock(&mThreadLock);

    if (!isConnectionValid())
      return;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"CREATE TABLE IF NOT EXISTS producer (\n");
    p += sprintf(p,"  producerId serial PRIMARY KEY,\n");
    p += sprintf(p,"  name VARCHAR (40) NOT NULL,\n");
    p += sprintf(p,"  title VARCHAR (50) NOT NULL,\n");
    p += sprintf(p,"  description VARCHAR (100) NOT NULL,\n");
    p += sprintf(p,"  flags INTEGER NOT NULL, \n");
    p += sprintf(p,"  sourceId INTEGER NOT NULL,\n");
    p += sprintf(p,"  status SMALLINT NOT NULL\n");
    p += sprintf(p,");\n");

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    p = sql;
    p += sprintf(p,"CREATE TABLE IF NOT EXISTS generation (\n");
    p += sprintf(p,"  generationId serial PRIMARY KEY,\n");
    p += sprintf(p,"  generationType INTEGER NOT NULL,\n");
    p += sprintf(p,"  producerId INTEGER NOT NULL,\n");
    p += sprintf(p,"  name VARCHAR (50) UNIQUE NOT NULL,\n");
    p += sprintf(p,"  description VARCHAR (100) NOT NULL,\n");
    p += sprintf(p,"  analysisTime TIMESTAMP NOT NULL,\n");
    p += sprintf(p,"  flags INTEGER NOT NULL,\n");
    p += sprintf(p,"  sourceId INTEGER NOT NULL,\n");
    p += sprintf(p,"  modificationTime TIMESTAMP,\n");
    p += sprintf(p,"  deletionTime TIMESTAMP,\n");
    p += sprintf(p,"  status SMALLINT NOT NULL,\n");
    p += sprintf(p,"  FOREIGN KEY (producerId) REFERENCES producer (producerId)\n");
    p += sprintf(p,"  );\n");

    res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    p = sql;
    p += sprintf(p,"CREATE TABLE IF NOT EXISTS file (\n");
    p += sprintf(p,"  fileId serial PRIMARY KEY,\n");
    p += sprintf(p,"  producerId INTEGER NOT NULL,\n");
    p += sprintf(p,"  generationId INTEGER NOT NULL,\n");
    p += sprintf(p,"  protocol SMALLINT NOT NULL,\n");
    p += sprintf(p,"  serverType SMALLINT NOT NULL,\n");
    p += sprintf(p,"  server VARCHAR (50) NOT NULL,\n");
    p += sprintf(p,"  fileType SMALLINT NOT NULL,\n");
    p += sprintf(p,"  fileName VARCHAR (200) UNIQUE NOT NULL,\n");
    p += sprintf(p,"  flags INTEGER NOT NULL,\n");
    p += sprintf(p,"  sourceId INTEGER NOT NULL,\n");
    p += sprintf(p,"  modificationTime TIMESTAMP,\n");
    p += sprintf(p,"  deletionTime TIMESTAMP,\n");
    p += sprintf(p,"  fileSize BIGINT NOT NULL,\n");
    p += sprintf(p,"  status SMALLINT NOT NULL,\n");
    p += sprintf(p,"  FOREIGN KEY (producerId) REFERENCES producer (producerId),\n");
    p += sprintf(p,"  FOREIGN KEY (generationId) REFERENCES generation (generationId)\n");
    p += sprintf(p,");\n");

    res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    p = sql;
    p += sprintf(p,"CREATE TABLE IF NOT EXISTS generationGeometry (\n");
    p += sprintf(p,"  producerId INTEGER NOT NULL,\n");
    p += sprintf(p,"  generationId INTEGER NOT NULL,\n");
    p += sprintf(p,"  geometryId INTEGER NOT NULL,\n");
    p += sprintf(p,"  levelId SMALLINT NOT NULL,\n");
    p += sprintf(p,"  flags INTEGER NOT NULL,\n");
    p += sprintf(p,"  sourceId INTEGER NOT NULL,\n");
    p += sprintf(p,"  modificationTime TIMESTAMP,\n");
    p += sprintf(p,"  deletionTime TIMESTAMP,\n");
    p += sprintf(p,"  status SMALLINT NOT NULL,\n");
    p += sprintf(p,"  PRIMARY KEY (generationId,geometryId,levelId),\n");
    p += sprintf(p,"  FOREIGN KEY (producerId) REFERENCES producer (producerId),\n");
    p += sprintf(p,"  FOREIGN KEY (generationId) REFERENCES generation (generationId)\n");
    p += sprintf(p,");\n");


    res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    p = sql;
    p += sprintf(p,"CREATE TABLE IF NOT EXISTS content (\n");
    p += sprintf(p,"  fileId INTEGER NOT NULL,\n");
    p += sprintf(p,"  messageIndex INTEGER NOT NULL,\n");
    p += sprintf(p,"  fileType SMALLINT NOT NULL,\n");
    p += sprintf(p,"  filePosition BIGINT NOT NULL,\n");
    p += sprintf(p,"  messageSize INTEGER NOT NULL,\n");
    p += sprintf(p,"  producerId INTEGER NOT NULL,\n");
    p += sprintf(p,"  generationId INTEGER NOT NULL,\n");
    p += sprintf(p,"  geometryId INTEGER NOT NULL,\n");
    p += sprintf(p,"  parameterId INTEGER NOT NULL,\n");
    p += sprintf(p,"  parameterName VARCHAR (50) NOT NULL,\n");
    p += sprintf(p,"  forecastTime TIMESTAMP NOT NULL,\n");
    p += sprintf(p,"  levelId SMALLINT NOT NULL,\n");
    p += sprintf(p,"  level INTEGER NOT NULL,\n");
    p += sprintf(p,"  forecastType SMALLINT,\n");
    p += sprintf(p,"  foracastNumber INTEGER,\n");
    p += sprintf(p,"  flags INTEGER NOT NULL,\n");
    p += sprintf(p,"  sourceId INTEGER NOT NULL,\n");
    p += sprintf(p,"  modificationTime TIMESTAMP,\n");
    p += sprintf(p,"  deletionTime TIMESTAMP,\n");
    p += sprintf(p,"  aggregationId INTEGER,\n");
    p += sprintf(p,"  aggregationPeriod INTEGER,\n");
    p += sprintf(p,"  processingTypeId INTEGER,\n");
    p += sprintf(p,"  processingTypeValue1 NUMERIC,\n");
    p += sprintf(p,"  processingTypeValue2 NUMERIC,\n");
    p += sprintf(p,"  PRIMARY KEY (fileId, messageIndex),\n");
    p += sprintf(p,"  FOREIGN KEY (fileId) REFERENCES file (fileId),\n");
    p += sprintf(p,"  FOREIGN KEY (producerId) REFERENCES producer (producerId),\n");
    p += sprintf(p,"  FOREIGN KEY (generationId) REFERENCES generation (generationId)\n");
    p += sprintf(p,");\n");

    int                mAggregationId;
    int                mAggregationPeriod;
    int                mProcessingTypeId;
    float              mProcessingTypeValue1;
    float              mProcessingTypeValue2;

    res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    p = sql;
    p += sprintf(p,"CREATE TABLE IF NOT EXISTS event (\n");
    p += sprintf(p,"  eventId serial PRIMARY KEY,\n");
    p += sprintf(p,"  eventTime TIMESTAMP,\n");
    p += sprintf(p,"  eventType SMALLINT NOT NULL,\n");
    p += sprintf(p,"  id1 INTEGER,\n");
    p += sprintf(p,"  id2 INTEGER,\n");
    p += sprintf(p,"  id3 INTEGER,\n");
    p += sprintf(p,"  flags INTEGER,\n");
    p += sprintf(p,"  eventData VARCHAR (300)\n");
    p += sprintf(p,");\n");

    res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int PostgresqlImplementation::openConnection()
{
  FUNCTION_TRACE
  try
  {
    if (mConnection != nullptr)
    {
      PQfinish(mConnection);
      mConnection = nullptr;
    }

    mConnection = PQconnectdb(mPrimaryConnectionString.c_str());
    if (PQstatus(mConnection) != CONNECTION_OK)
    {
      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.printError();

      if (!mSecondaryConnectionString.empty())

      {
        mConnection = PQconnectdb(mSecondaryConnectionString.c_str());
        if (PQstatus(mConnection) != CONNECTION_OK)
        {
          Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
          exception.printError();
          return Result::PERMANENT_STORAGE_ERROR;
        }
      }
      else
      {
        return Result::PERMANENT_STORAGE_ERROR;
      }
    }

    mStartTime = time(nullptr);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void PostgresqlImplementation::closeConnection()
{
  FUNCTION_TRACE
  try
  {
    if (mConnection != nullptr)
    {
      PQfinish(mConnection);
      mConnection = nullptr;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void PostgresqlImplementation::shutdown()
{
  FUNCTION_TRACE
  try
  {
    //exit(0);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool PostgresqlImplementation::isSessionValid(T::SessionId sessionId)
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





bool PostgresqlImplementation::isConnectionValid()
{
  FUNCTION_TRACE
  try
  {
    if (mConnection == nullptr  &&  openConnection() != Result::OK)
      return false;

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_clear(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    PGresult *res = PQexec(mConnection,"TRUNCATE TABLE event;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      //PQclear(res);
      //throw exception;
    }
    PQclear(res);

    res = PQexec(mConnection,"TRUNCATE TABLE content CASCADE;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      //PQclear(res);
      //throw exception;
    }
    PQclear(res);

    res = PQexec(mConnection,"TRUNCATE TABLE file CASCADE;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      //PQclear(res);
      //throw exception;
    }
    PQclear(res);

    res = PQexec(mConnection,"TRUNCATE TABLE generation CASCADE;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      //PQclear(res);
      //throw exception;
    }
    PQclear(res);

    res = PQexec(mConnection,"TRUNCATE TABLE producer CASCADE;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      //PQclear(res);
      //throw exception;
    }
    PQclear(res);

    addEvent(EventType::CLEAR,0,0,0,0,"");

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_reload(T::SessionId sessionId)
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





int PostgresqlImplementation::_getContentChangeTime(T::SessionId sessionId,time_t& changeTime)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    changeTime = mContentChangeTime;

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::ProducerInfo pInfo;
    if (getProducerByName(producerInfo.mName,pInfo) == Result::OK)
      return Result::PRODUCER_NAME_ALREADY_REGISTERED;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"INSERT INTO producer\n");
    p += sprintf(p,"(\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  name,\n");
    p += sprintf(p,"  title,\n");
    p += sprintf(p,"  description,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,")\n");
    p += sprintf(p,"VALUES \n");
    p += sprintf(p,"(\n");
    p += sprintf(p,"  DEFAULT,\n");
    p += sprintf(p,"  '%s',\n",producerInfo.mName.c_str());
    p += sprintf(p,"  '%s',\n",producerInfo.mTitle.c_str());
    p += sprintf(p,"  '%s',\n",producerInfo.mDescription.c_str());
    p += sprintf(p,"  %u,\n",producerInfo.mFlags);
    p += sprintf(p,"  %u,\n",producerInfo.mSourceId);
    p += sprintf(p,"  %u\n",(uint)producerInfo.mStatus);
    p += sprintf(p,");\n");

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    int result = getProducerByName(producerInfo.mName,producerInfo);
    if (result != Result::OK)
      return result;

    addEvent(EventType::PRODUCER_ADDED,producerInfo.mProducerId,0,0,0,producerInfo.getCsv().c_str());

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteProducerInfoById(T::SessionId sessionId,uint producerId)
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

    int result = deleteProducerById(producerInfo.mProducerId,true,true,true,true);

    if (result == Result::OK)
      addEvent(EventType::PRODUCER_DELETED,producerId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteProducerInfoByName(T::SessionId sessionId,const std::string& producerName)
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


    int result = deleteProducerById(producerInfo.mProducerId,true,true,true,true);

    if (result == Result::OK)
      addEvent(EventType::PRODUCER_DELETED,producerInfo.mProducerId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    int result = deleteProducerListBySourceId(sourceId,true,true,true,true);

    if (result == Result::OK)
      addEvent(EventType::PRODUCER_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getProducerInfoByName(T::SessionId sessionId,const std::string& producerName,T::ProducerInfo& producerInfo)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    producerInfoList.clear();
    char st[100];
    strcpy(st,parameterKey.c_str());

    if (parameterKeyType == T::ParamKeyTypeValue::FMI_NAME || parameterKeyType == T::ParamKeyTypeValue::GRIB_ID || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID  || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME)
    {
      if (strncasecmp(st,"GRIB-",5) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::GRIB_ID;
        parameterKey = st+5;
      }
      else
      if (strncasecmp(st,"NB-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_ID;
        parameterKey = st+3;
      }
      else
      if (strncasecmp(st,"NBN-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_NAME;
        parameterKey = st+4;
      }
    }

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





int PostgresqlImplementation::_getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getProducerInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    count = 0;

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    count = getCount("SELECT COUNT(*) FROM producer");

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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
          Fmi::hash_merge(seed,producerInfo->mName);
          Fmi::hash_merge(seed,sourceParamKey);
          Fmi::hash_merge(seed,targetParameterKeyType);
          Fmi::hash_merge(seed,targetParamKey);
          Fmi::hash_merge(seed,contentInfo->mGeometryId);
          Fmi::hash_merge(seed,paramLevelId);
          Fmi::hash_merge(seed,contentInfo->mParameterLevel);
          Fmi::hash_merge(seed,contentInfo->mForecastType);
          Fmi::hash_merge(seed,contentInfo->mForecastNumber);


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





int PostgresqlImplementation::_setProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"UPDATE producer\n");
    p += sprintf(p,"SET\n");
    p += sprintf(p,"  name = '%s',\n",producerInfo.mName.c_str());
    p += sprintf(p,"  title = '%s',\n",producerInfo.mTitle.c_str());
    p += sprintf(p,"  description = '%s',\n",producerInfo.mDescription.c_str());
    p += sprintf(p,"  flags = %u,\n",producerInfo.mFlags);
    p += sprintf(p,"  sourceId = %u,\n",producerInfo.mSourceId);
    p += sprintf(p,"  status = %u\n",producerInfo.mStatus);
    p += sprintf(p,"WHERE \n");
    p += sprintf(p, "  producerId = %u;\n", producerInfo.mProducerId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    addEvent(EventType::PRODUCER_UPDATED,producerInfo.mProducerId,0,0,0,"");

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int PostgresqlImplementation::_addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
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

    T::GenerationInfo gInfo;
    if (getGenerationByName(generationInfo.mName,gInfo) == Result::OK)
      return Result::GENERATION_NAME_ALREADY_REGISTERED;

    std::string modificationTime = utcTimeFromTimeT(generationInfo.mModificationTime);
    std::string deletionTime = utcTimeFromTimeT(generationInfo.mDeletionTime);

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"INSERT INTO generation\n");
    p += sprintf(p,"(\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  generationType,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  name,\n");
    p += sprintf(p,"  description,\n");
    p += sprintf(p,"  analysisTime,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  modificationTime,\n");
    p += sprintf(p,"  deletionTime,\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,")\n");
    p += sprintf(p,"VALUES \n");
    p += sprintf(p,"(\n");
    p += sprintf(p,"  DEFAULT,\n");
    p += sprintf(p,"  %u,\n",generationInfo.mGenerationType);
    p += sprintf(p,"  %u,\n",generationInfo.mProducerId);
    p += sprintf(p,"  '%s',\n",generationInfo.mName.c_str());
    p += sprintf(p,"  '%s',\n",generationInfo.mDescription.c_str());
    p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",generationInfo.mAnalysisTime.c_str());
    p += sprintf(p,"  %u,\n",generationInfo.mFlags);
    p += sprintf(p,"  %u,\n",generationInfo.mSourceId);
    p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",modificationTime.c_str());
    p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",deletionTime.c_str());
    p += sprintf(p,"  %u\n",generationInfo.mStatus);
    p += sprintf(p,");\n");

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    int result = getGenerationByName(generationInfo.mName,generationInfo);
    if (result != Result::OK)
      return result;

    addEvent(EventType::GENERATION_ADDED,generationInfo.mGenerationId,0,0,0,generationInfo.getCsv().c_str());

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteGenerationInfoById(T::SessionId sessionId,uint generationId)
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

    int result = deleteGenerationById(generationInfo.mGenerationId,true,true,true);

    if (result == Result::OK)
      addEvent(EventType::GENERATION_DELETED,generationId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteGenerationInfoByName(T::SessionId sessionId,const std::string& generationName)
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


    int result = deleteGenerationById(generationInfo.mGenerationId,true,true,true);

    if (result == Result::OK)
      addEvent(EventType::GENERATION_DELETED,generationInfo.mGenerationId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<uint>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    for (auto it = generationIdList.begin(); it != generationIdList.end(); ++it)
    {
      deleteGenerationById(*it,true,true,true);
      addEvent(EventType::GENERATION_DELETED,*it,0,0,0,"");
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId)
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

    int result = deleteGenerationListByProducerId(producerId,true,true,true);

    if (result == Result::OK)
      addEvent(EventType::GENERATION_LIST_DELETED_BY_PRODUCER_ID,producerId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
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

    int result = deleteGenerationListByProducerId(producerInfo.mProducerId,true,true,true);

    if (result == Result::OK)
      addEvent(EventType::GENERATION_LIST_DELETED_BY_PRODUCER_ID,producerInfo.mProducerId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    int result = deleteGenerationListBySourceId(sourceId,true,true,true);

    if (result == Result::OK)
      addEvent(EventType::GENERATION_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getGenerationInfoByName(T::SessionId sessionId,const std::string& generationName,T::GenerationInfo& generationInfo)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,uchar generationStatus,T::GenerationInfo& generationInfo)
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




int PostgresqlImplementation::_getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,const std::string& producerName,uchar generationStatus,T::GenerationInfo& generationInfo)
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




int PostgresqlImplementation::_getGenerationInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    count = 0;

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    count = getCount("SELECT COUNT(*) FROM generation");

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,uchar status)
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

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"UPDATE generation\n");
    p += sprintf(p,"SET\n");
    p += sprintf(p,"  status = %u\n",status);
    p += sprintf(p,"WHERE \n");
    p += sprintf(p,"  generationId = %u;\n",generationInfo.mGenerationId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    addEvent(EventType::GENERATION_STATUS_CHANGED,generationInfo.mGenerationId,status,0,0,"");

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_setGenerationInfoStatusByName(T::SessionId sessionId,const std::string& generationName,uchar status)
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

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"UPDATE generation\n");
    p += sprintf(p,"SET\n");
    p += sprintf(p,"  status = %u\n",status);
    p += sprintf(p,"WHERE \n");
    p += sprintf(p,"  generationId = %u;\n",generationInfo.mGenerationId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    addEvent(EventType::GENERATION_STATUS_CHANGED,generationInfo.mGenerationId,status,0,0,"");

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






int PostgresqlImplementation::_setGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
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

    std::string modificationTime = utcTimeFromTimeT(generationInfo.mModificationTime);
    std::string deletionTime = utcTimeFromTimeT(generationInfo.mDeletionTime);

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"UPDATE generation\n");
    p += sprintf(p,"SET\n");
    p += sprintf(p,"  generationType = %u,\n",generationInfo.mGenerationType);
    p += sprintf(p,"  producerId = %u,\n",generationInfo.mProducerId);
    p += sprintf(p,"  name = '%s',\n",generationInfo.mName.c_str());
    p += sprintf(p,"  description = '%s',\n",generationInfo.mDescription.c_str());
    p += sprintf(p,"  analysisTime = TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",generationInfo.mAnalysisTime.c_str());
    p += sprintf(p,"  flags = %u,\n",generationInfo.mFlags);
    p += sprintf(p,"  sourceId = %u,\n",generationInfo.mSourceId);
    p += sprintf(p,"  modificationTime = TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",modificationTime.c_str());
    p += sprintf(p,"  deletionTime = TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",deletionTime.c_str());
    p += sprintf(p,"  status = %u\n",generationInfo.mStatus);
    p += sprintf(p,"WHERE \n");
    p += sprintf(p,"  generationId = %u;\n",generationInfo.mGenerationId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    int result = getGenerationByName(generationInfo.mName,generationInfo);
    if (result != Result::OK)
      return result;

    addEvent(EventType::GENERATION_UPDATED,generationInfo.mGenerationId,0,0,0,generationInfo.getCsv().c_str());

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_addGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    std::string modificationTime = utcTimeFromTimeT(geometryInfo.mModificationTime);
    std::string deletionTime = utcTimeFromTimeT(geometryInfo.mDeletionTime);

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"INSERT INTO generationGeometry\n");
    p += sprintf(p,"(\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  modificationTime,\n");
    p += sprintf(p,"  deletionTime,\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,")\n");
    p += sprintf(p,"VALUES \n");
    p += sprintf(p,"(\n");
    p += sprintf(p,"  %u,\n",geometryInfo.mProducerId);
    p += sprintf(p,"  %u,\n",geometryInfo.mGenerationId);
    p += sprintf(p,"  %u,\n",geometryInfo.mGeometryId);
    p += sprintf(p,"  %u,\n",geometryInfo.mLevelId);
    p += sprintf(p,"  %u,\n",geometryInfo.mFlags);
    p += sprintf(p,"  %u,\n",geometryInfo.mSourceId);
    p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",modificationTime.c_str());
    p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",deletionTime.c_str());
    p += sprintf(p,"  %u\n",(uint)geometryInfo.mStatus);
    p += sprintf(p,");\n");

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);


    addEvent(EventType::GEOMETRY_ADDED,geometryInfo.mGenerationId,geometryInfo.mGeometryId,geometryInfo.mLevelId,0,geometryInfo.getCsv().c_str());

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






int PostgresqlImplementation::_deleteGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[100];
    sprintf(sql,"DELETE FROM generationGeometry WHERE generationId=%u AND geometryId=%u AND levelId=%u;",generationId,geometryId,levelId);
    PGresult *res = PQexec(mConnection,sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    addEvent(EventType::GEOMETRY_DELETED,generationId,geometryId,levelId,0,"");

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[100];
    sprintf(sql,"DELETE FROM generationGeometry WHERE generationId=%u;",generationId);
    PGresult *res = PQexec(mConnection,sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[100];
    sprintf(sql,"DELETE FROM generationGeometry WHERE producerId=%u;",producerId);
    PGresult *res = PQexec(mConnection,sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[100];
    sprintf(sql,"DELETE FROM generationGeometry WHERE sourceId=%u;",sourceId);
    PGresult *res = PQexec(mConnection,sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  generationGeometry\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  generationId=%u AND geometryId=%u AND levelId=%u;\n",generationId,geometryId,levelId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);
    if (rowCount == 0)
      return Result::DATA_NOT_FOUND;

    for (int i = 0; i < rowCount; i++)
    {
      geometryInfo.mProducerId = atoi(PQgetvalue(res, i, 0));
      geometryInfo.mGenerationId = atoi(PQgetvalue(res, i, 1));
      geometryInfo.mGeometryId = atoi(PQgetvalue(res, i, 2));
      geometryInfo.mLevelId = atoi(PQgetvalue(res, i, 3));
      geometryInfo.mFlags = atoi(PQgetvalue(res, i, 4));
      geometryInfo.mSourceId = atoi(PQgetvalue(res, i, 5));

      std::string modificationTime = PQgetvalue(res, i, 6);
      geometryInfo.mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 7);
      geometryInfo.mDeletionTime = utcTimeToTimeT(deletionTime);

      geometryInfo.mStatus = atoi(PQgetvalue(res, i, 8));
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getGeometryInfoList(T::SessionId sessionId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    geometryInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  generationGeometry;\n");

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::GeometryInfo *geometryInfo = new T::GeometryInfo();

      geometryInfo->mProducerId = atoi(PQgetvalue(res, i, 0));
      geometryInfo->mGenerationId = atoi(PQgetvalue(res, i, 1));
      geometryInfo->mGeometryId = atoi(PQgetvalue(res, i, 2));
      geometryInfo->mLevelId = atoi(PQgetvalue(res, i, 3));
      geometryInfo->mFlags = atoi(PQgetvalue(res, i, 4));
      geometryInfo->mSourceId = atoi(PQgetvalue(res, i, 5));

      std::string modificationTime = PQgetvalue(res, i, 6);
      geometryInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 7);
      geometryInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      geometryInfo->mStatus = atoi(PQgetvalue(res, i, 8));

      geometryInfoList.addGeometryInfo(geometryInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    geometryInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  generationGeometry\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  generationId=%u;\n",generationId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::GeometryInfo *geometryInfo = new T::GeometryInfo();

      geometryInfo->mProducerId = atoi(PQgetvalue(res, i, 0));
      geometryInfo->mGenerationId = atoi(PQgetvalue(res, i, 1));
      geometryInfo->mGeometryId = atoi(PQgetvalue(res, i, 2));
      geometryInfo->mLevelId = atoi(PQgetvalue(res, i, 3));
      geometryInfo->mFlags = atoi(PQgetvalue(res, i, 4));
      geometryInfo->mSourceId = atoi(PQgetvalue(res, i, 5));

      std::string modificationTime = PQgetvalue(res, i, 6);
      geometryInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 7);
      geometryInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      geometryInfo->mStatus = atoi(PQgetvalue(res, i, 8));

      geometryInfoList.addGeometryInfo(geometryInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    geometryInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  generationGeometry;\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  producerId=%u;\n",producerId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::GeometryInfo *geometryInfo = new T::GeometryInfo();

      geometryInfo->mProducerId = atoi(PQgetvalue(res, i, 0));
      geometryInfo->mGenerationId = atoi(PQgetvalue(res, i, 1));
      geometryInfo->mGeometryId = atoi(PQgetvalue(res, i, 2));
      geometryInfo->mLevelId = atoi(PQgetvalue(res, i, 3));
      geometryInfo->mFlags = atoi(PQgetvalue(res, i, 4));
      geometryInfo->mSourceId = atoi(PQgetvalue(res, i, 5));

      std::string modificationTime = PQgetvalue(res, i, 6);
      geometryInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 7);
      geometryInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      geometryInfo->mStatus = atoi(PQgetvalue(res, i, 8));

      geometryInfoList.addGeometryInfo(geometryInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    geometryInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  generationGeometry;\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  sourceId=%u;\n",sourceId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::GeometryInfo *geometryInfo = new T::GeometryInfo();

      geometryInfo->mProducerId = atoi(PQgetvalue(res, i, 0));
      geometryInfo->mGenerationId = atoi(PQgetvalue(res, i, 1));
      geometryInfo->mGeometryId = atoi(PQgetvalue(res, i, 2));
      geometryInfo->mLevelId = atoi(PQgetvalue(res, i, 3));
      geometryInfo->mFlags = atoi(PQgetvalue(res, i, 4));
      geometryInfo->mSourceId = atoi(PQgetvalue(res, i, 5));

      std::string modificationTime = PQgetvalue(res, i, 6);
      geometryInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 7);
      geometryInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      geometryInfo->mStatus = atoi(PQgetvalue(res, i, 8));

      geometryInfoList.addGeometryInfo(geometryInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getGeometryInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    count = 0;

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    count = getCount("SELECT COUNT(*) FROM generationGeometry");

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_setGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    std::string modificationTime = utcTimeFromTimeT(geometryInfo.mModificationTime);
    std::string deletionTime = utcTimeFromTimeT(geometryInfo.mDeletionTime);

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"UPDATE generationGeometry\n");
    p += sprintf(p,"SET\n");
    p += sprintf(p,"  flags = %u,\n",geometryInfo.mFlags);
    p += sprintf(p,"  sourceId = %u,\n",geometryInfo.mSourceId);
    p += sprintf(p,"  modificationTime = TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",modificationTime.c_str());
    p += sprintf(p,"  deletionTime = TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",deletionTime.c_str());
    p += sprintf(p,"  status = %u\n",geometryInfo.mStatus);
    p += sprintf(p,"WHERE \n");
    p += sprintf(p,"  generationId=%u AND geometryId=%u AND levelId=%u;\n",geometryInfo.mGenerationId,geometryInfo.mGeometryId,geometryInfo.mLevelId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    addEvent(EventType::GEOMETRY_UPDATED,geometryInfo.mGenerationId,geometryInfo.mGeometryId,geometryInfo.mLevelId,0,geometryInfo.getCsv().c_str());

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_setGeometryInfoStatusById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,uchar status)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[1000];
    char *p = sql;

    p += sprintf(p,"UPDATE generationGeometry\n");
    p += sprintf(p,"SET\n");
    p += sprintf(p,"  status = %u\n",status);
    p += sprintf(p,"WHERE \n");
    p += sprintf(p,"  generationId=%u AND geometryId=%u AND levelId=%u;\n",generationId,geometryId,levelId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    addEvent(EventType::GEOMETRY_STATUS_CHANGED,generationId,geometryId,levelId,status,"");

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    int result = addFile(fileInfo);

    if (result != Result::OK)
      return result;

    addEvent(EventType::FILE_ADDED,fileInfo.mFileId,fileInfo.mFileType,0,0,fileInfo.getCsv().c_str());

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    int result = addFile(fileInfo);

    if (result != Result::OK)
      return result;

    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
      contentInfo->mFileId = fileInfo.mFileId;
    }

    addContentList(contentInfoList);

    addEvent(EventType::FILE_ADDED,fileInfo.mFileId,fileInfo.mFileType,contentInfoList.getLength(),0,fileInfo.getCsv().c_str());

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::FileInfoList fileInfoList;

    for (auto ff = fileAndContentList.begin();ff != fileAndContentList.end(); ++ff)
    {
      //ff->mFileInfo.print(std::cout,0,0);
      fileInfoList.addFileInfo(new T::FileInfo(ff->mFileInfo));
    }

    T::EventInfoList eventInfoList;

    addFileList(fileInfoList);

    T::ContentInfoList contentInfoList;
    for (auto ff = fileAndContentList.begin();ff != fileAndContentList.end(); ++ff)
    {
      uint len = ff->mContentInfoList.getLength();
      if (len > 0)
      {
        T::FileInfo fileInfo;
        int result = getFileByName(ff->mFileInfo.mName,fileInfo);
        if (result == Result::OK)
        {
          ff->mFileInfo.mFileId = fileInfo.mFileId;

          for (uint t=0; t<len; t++)
          {
            T::ContentInfo *cInfo = ff->mContentInfoList.getContentInfoByIndex(t);

            T::ContentInfo *contentInfo = new T::ContentInfo(*cInfo);
            contentInfo->mFileId = fileInfo.mFileId;
            contentInfoList.addContentInfo(contentInfo);
          }
        }
        //eventInfoList.addEventInfo(new EventInfo(time(0),EventType::FILE_ADDED,ff->mFileInfo.mFileId,ff->mFileInfo.mFileType,ff->mContentInfoList.getLength(),0));
        //addEvent(EventType::FILE_ADDED,ff->mFileInfo.mFileId,ff->mFileInfo.mFileType,ff->mContentInfoList.getLength(),0);
      }
    }

    addContentList(contentInfoList);

    uint len = fileInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::FileInfo *fileInfo = fileInfoList.getFileInfoByIndex(t);
      //addEvent(EventType::FILE_ADDED,fileInfo->mFileId,fileInfo->mFileType,0,0);
      eventInfoList.addEventInfo(new T::EventInfo(time(0),0,EventType::FILE_ADDED,fileInfo->mFileId,fileInfo->mFileType,0,0,fileInfo->getCsv().c_str()));
    }

    addEventInfoList(eventInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteFileInfoById(T::SessionId sessionId,uint fileId)
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

    int result = deleteFileById(fileId,true);

    if (result == Result::OK)
      addEvent(EventType::FILE_DELETED,fileId,fileInfo.mFileType,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteFileInfoByName(T::SessionId sessionId,const std::string& filename)
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
    getFileByName(filename,fileInfo);
    if (fileInfo.mFileId == 0)
      return Result::UNKNOWN_FILE_NAME;

    int result = deleteFileById(fileInfo.mFileId,true);

    if (result == Result::OK)
    {
      addEvent(EventType::FILE_DELETED,fileInfo.mFileId,0,0,0,"");
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId)
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
      addEvent(EventType::FILE_LIST_DELETED_BY_PRODUCER_ID,producerId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
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
      addEvent(EventType::FILE_LIST_DELETED_BY_PRODUCER_ID,producerInfo.mProducerId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId)
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
      addEvent(EventType::FILE_LIST_DELETED_BY_GENERATION_ID,generationId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,const std::string& forecastTime)
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
          deleteFileById(*it,false);
          addEvent(EventType::FILE_DELETED,*it,fileInfo.mFileType,0,0,"");
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





int PostgresqlImplementation::_deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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
        deleteFileById(info->mFileId,false);
        addEvent(EventType::FILE_DELETED,info->mFileId,fileInfo.mFileType,0,0,"");
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName)
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
      addEvent(EventType::FILE_LIST_DELETED_BY_GENERATION_ID,generationInfo.mGenerationId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    int result = deleteFileListBySourceId(sourceId,true);

    if (result == Result::OK)
      addEvent(EventType::FILE_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    for (auto it=fileIdList.begin(); it!=fileIdList.end(); ++it)
    {
      T::FileInfo fileInfo;
      if (getFileById(*it,fileInfo) == Result::OK)
      {
        deleteFileById(*it,true);
        addEvent(EventType::FILE_DELETED,*it,0,0,0,"");
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getFileInfoByName(T::SessionId sessionId,const std::string& filename,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return getFileByName(filename,fileInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getFileInfoList(T::SessionId sessionId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getFileInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    count = 0;

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    count = getCount("SELECT COUNT(*) FROM file");

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getFileInfoCountByProducerId(T::SessionId sessionId,uint producerId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    count = 0;
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[100];
    sprintf(sql,"SELECT COUNT(*) FROM file WHERE producerId=%u",producerId);
    count = getCount(sql);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getFileInfoCountByGenerationId(T::SessionId sessionId,uint generationId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    count = 0;
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[100];
    sprintf(sql,"SELECT COUNT(*) FROM file WHERE generationId=%u",generationId);
    count = getCount(sql);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getFileInfoCountBySourceId(T::SessionId sessionId,uint sourceId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    count = 0;
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[100];
    sprintf(sql,"SELECT COUNT(*) FROM file WHERE sourceId=%u",sourceId);
    count = getCount(sql);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_setFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    std::string modificationTime = utcTimeFromTimeT(fileInfo.mModificationTime);
    std::string deletionTime = utcTimeFromTimeT(fileInfo.mDeletionTime);

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"UPDATE file\n");
    p += sprintf(p,"SET\n");
    p += sprintf(p,"  producerId = %u,\n",fileInfo.mProducerId);
    p += sprintf(p,"  generationId = %u,\n",fileInfo.mGenerationId);
    p += sprintf(p,"  protocol = %u,\n",(uint)fileInfo.mProtocol);
    p += sprintf(p,"  serverType = %u,\n",(uint)fileInfo.mServerType);
    p += sprintf(p,"  server = '%s',\n",fileInfo.mServer.c_str());
    p += sprintf(p,"  fileType = %u,\n",(uint)fileInfo.mFileType);
    p += sprintf(p,"  fileName = '%s',\n",fileInfo.mName.c_str());
    p += sprintf(p,"  flags = %u,\n",fileInfo.mFlags);
    p += sprintf(p,"  sourceId = %u,\n",fileInfo.mSourceId);
    p += sprintf(p,"  modificationTime = TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",modificationTime.c_str());
    p += sprintf(p,"  deletionTime = TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",deletionTime.c_str());
    p += sprintf(p,"  fileSize = %llu,\n",fileInfo.mSize);
    p += sprintf(p,"  status = %u\n",fileInfo.mStatus);
    p += sprintf(p,"WHERE \n");
    p += sprintf(p, "  fileId = %u;\n", fileInfo.mFileId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    addEvent(EventType::FILE_UPDATED,fileInfo.mFileId,0,0,0,"");

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int PostgresqlImplementation::_addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    eventInfo.mEventId = addEvent(eventInfo.mType,eventInfo.mId1,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags,eventInfo.mEventData.c_str());
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
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

    PGresult *res = PQexec(mConnection, "SELECT MAX(eventId) FROM event;");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);
    if (rowCount == 0)
      return 0;

    eventId = atoll(PQgetvalue(res, 0, 0));

    PQclear(res);

    if (eventId == 0)
      return Result::DATA_NOT_FOUND;

    if (mLastEvent.mEventId == eventId)
    {
      eventInfo = mLastEvent;
      eventInfo.mServerTime = mStartTime;
      return Result::OK;
    }


    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  eventId,\n");
    p += sprintf(p,"  eventType,\n");
    p += sprintf(p,"  to_char(eventTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  id1,\n");
    p += sprintf(p,"  id2,\n");
    p += sprintf(p,"  id3,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  eventData\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  event\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  eventId=%llu;\n",eventId);

    res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    rowCount = PQntuples(res);
    if (rowCount != 1)
      return Result::DATA_NOT_FOUND;

    mLastEvent.mEventId = atoll(PQgetvalue(res, 0, 0));
    mLastEvent.mType = atoll(PQgetvalue(res, 0, 1));
    std::string eventTime = PQgetvalue(res, 0, 2);
    mLastEvent.mEventTime = utcTimeToTimeT(eventTime);
    mLastEvent.mServerTime = mStartTime;
    mLastEvent.mId1 = atoll(PQgetvalue(res, 0, 3));
    mLastEvent.mId2 = atoll(PQgetvalue(res, 0, 4));
    mLastEvent.mId3 = atoll(PQgetvalue(res, 0, 5));
    mLastEvent.mFlags = atoll(PQgetvalue(res, 0, 6));
    mLastEvent.mEventData = PQgetvalue(res, 0, 7);

    PQclear(res);

    eventInfo = mLastEvent;
    eventInfo.mServerTime = mStartTime;

    //eventInfo.print(std::cout,0,0);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    eventInfoList.clear();

    if (maxRecords > 10000)
      maxRecords = 10000;

    eventInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    //printf("GET EVENT LIST %llu\n",startEventId);

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  eventId,\n");
    p += sprintf(p,"  eventType,\n");
    p += sprintf(p,"  to_char(eventTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  id1,\n");
    p += sprintf(p,"  id2,\n");
    p += sprintf(p,"  id3,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  eventData\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  event\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  eventId>=%llu\n",startEventId);
    p += sprintf(p,"ORDER BY\n");
    p += sprintf(p,"  eventId;\n");

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    //printf("GET EVENT LIST %llu  => %d\n",startEventId,rowCount);

    if (rowCount > maxRecords)
      rowCount = maxRecords;

    for (int i = 0; i < rowCount; i++)
    {
      T::EventInfo *eventInfo = new T::EventInfo();
      eventInfo->mEventId = atoll(PQgetvalue(res, i, 0));
      eventInfo->mType = atoll(PQgetvalue(res, i, 1));
      std::string eventTime = PQgetvalue(res, i, 2);
      eventInfo->mEventTime = utcTimeToTimeT(eventTime);
      eventInfo->mServerTime = mStartTime;
      eventInfo->mId1 = atoll(PQgetvalue(res, i, 3));
      eventInfo->mId2 = atoll(PQgetvalue(res, i, 4));
      eventInfo->mId3 = atoll(PQgetvalue(res, i, 5));
      eventInfo->mFlags = atoll(PQgetvalue(res, i, 6));
      eventInfo->mEventData = PQgetvalue(res, i, 7);

      eventInfoList.addEventInfo(eventInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getEventInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    count = 0;

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    count = getCount("SELECT COUNT(*) FROM event");

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    int res = addContent(contentInfo);
    if (res != 0)
      return res;

    addEvent(EventType::CONTENT_ADDED,contentInfo.mFileId,contentInfo.mMessageIndex,0,0,contentInfo.getCsv().c_str());

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_setContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    std::string forecastTime = utcTimeFromTimeT(contentInfo.mForecastTimeUTC);
    std::string modificationTime = utcTimeFromTimeT(contentInfo.mModificationTime);
    std::string deletionTime = utcTimeFromTimeT(contentInfo.mDeletionTime);

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"UPDATE content\n");
    p += sprintf(p,"SET\n");
    p += sprintf(p,"  fileType = %u,\n",contentInfo.mFileType);
    p += sprintf(p,"  filePosition = %llu,\n",contentInfo.mFilePosition);
    p += sprintf(p,"  messageSize = %u,\n",contentInfo.mMessageSize);
    p += sprintf(p,"  producerId = %u,\n",contentInfo.mProducerId);
    p += sprintf(p,"  generationId = %u,\n",contentInfo.mGenerationId);
    p += sprintf(p,"  geometryId = %u,\n",contentInfo.mGeometryId);
    p += sprintf(p,"  parameterId = %d,\n",contentInfo.mFmiParameterId);
    p += sprintf(p,"  parameterName = '%s',\n",contentInfo.getFmiParameterName());
    p += sprintf(p,"  forecastTime = TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",forecastTime.c_str());
    p += sprintf(p,"  levelId = %d,\n",contentInfo.mFmiParameterLevelId);
    p += sprintf(p,"  level = %d,\n",contentInfo.mParameterLevel);
    p += sprintf(p,"  forecastType = %d,\n",contentInfo.mForecastType);
    p += sprintf(p,"  foracastNumber = %d,\n",contentInfo.mForecastNumber);
    p += sprintf(p,"  flags = %u,\n",contentInfo.mFlags);
    p += sprintf(p,"  sourceId = %u,\n",contentInfo.mSourceId);
    p += sprintf(p,"  aggregationId = %d,\n",contentInfo.mAggregationId);
    p += sprintf(p,"  aggregationPeriod = %d,\n",contentInfo.mAggregationPeriod);
    p += sprintf(p,"  processingTypeId = %d,\n",contentInfo.mProcessingTypeId);
    p += sprintf(p,"  processingTypeValue1 = %f,\n",contentInfo.mProcessingTypeValue1);
    p += sprintf(p,"  processingTypeValue2 = %f,\n",contentInfo.mProcessingTypeValue2);
    p += sprintf(p,"  modificationTime = TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",modificationTime.c_str());
    p += sprintf(p,"  deletionTime = TO_TIMESTAMP('%s','yyyymmddThh24MISS')\n",deletionTime.c_str());
    p += sprintf(p,"WHERE \n");
    p += sprintf(p, "  fileId = %u AND messageIndex = %u;\n",contentInfo.mFileId,contentInfo.mMessageIndex);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    addEvent(EventType::CONTENT_UPDATED,contentInfo.mFileId,contentInfo.mMessageIndex,0,0,"");

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int PostgresqlImplementation::_addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    addContentList(contentInfoList);

    T::EventInfoList eventInfoList;

    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      //addEvent(EventType::CONTENT_ADDED,info->mFileId,info->mMessageIndex,0,0);
      eventInfoList.addEventInfo(new T::EventInfo(time(0),0,EventType::CONTENT_ADDED,info->mFileId,info->mMessageIndex,0,0,info->getCsv().c_str()));
    }
    addEventInfoList(eventInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex)
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
      addEvent(EventType::CONTENT_DELETED,fileId,messageIndex,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteContentListByFileId(T::SessionId sessionId,uint fileId)
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
      addEvent(EventType::CONTENT_LIST_DELETED_BY_FILE_ID,fileId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteContentListByFileName(T::SessionId sessionId,const std::string& filename)
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
    getFileByName(filename,fileInfo);
    if (fileInfo.mFileId == 0)
      return Result::UNKNOWN_FILE_NAME;

    int result = deleteContentByFileId(fileInfo.mFileId);

    if (result == Result::OK)
      addEvent(EventType::CONTENT_LIST_DELETED_BY_FILE_ID,fileInfo.mFileId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteContentListByProducerId(T::SessionId sessionId,uint producerId)
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
      addEvent(EventType::CONTENT_LIST_DELETED_BY_PRODUCER_ID,producerId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteContentListByProducerName(T::SessionId sessionId,const std::string& producerName)
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
      addEvent(EventType::CONTENT_LIST_DELETED_BY_PRODUCER_ID,producerInfo.mProducerId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int PostgresqlImplementation::_deleteContentListByGenerationId(T::SessionId sessionId,uint generationId)
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
      addEvent(EventType::CONTENT_LIST_DELETED_BY_GENERATION_ID,generationId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteContentListByGenerationName(T::SessionId sessionId,const std::string& generationName)
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
      addEvent(EventType::CONTENT_LIST_DELETED_BY_GENERATION_ID,generationInfo.mGenerationId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_deleteContentListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    int result = deleteContentBySourceId(sourceId);

    if (result == Result::OK)
      addEvent(EventType::CONTENT_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0,"");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getContentListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getContentListByFileName(T::SessionId sessionId,const std::string& filename,T::ContentInfoList& contentInfoList)
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
    getFileByName(filename,fileInfo);
    if (fileInfo.mFileId == 0)
      return Result::UNKNOWN_FILE_NAME;

    contentInfoList.clear();

    return getContentByFileId(fileInfo.mFileId,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
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

    contentInfoList.clear();

    return getContentByProducerId(producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
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

    contentInfoList.clear();

    return getContentByProducerId(producerInfo.mProducerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,int maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList)
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

    contentInfoList.clear();

    return getContentByGenerationId(generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






int PostgresqlImplementation::_getContentListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
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

    contentInfoList.clear();

    return getContentByGenerationId(generationInfo.mGenerationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList)
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

    contentInfoList.clear();

    return getContentByGenerationIdAndTimeRange(generationId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList)
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

    contentInfoList.clear();

    return getContentByGenerationIdAndTimeRange(generationInfo.mGenerationId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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





int PostgresqlImplementation::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    contentInfoList.clear();

    char st[100];
    strcpy(st,parameterKey.c_str());

    if (parameterKeyType == T::ParamKeyTypeValue::FMI_NAME || parameterKeyType == T::ParamKeyTypeValue::GRIB_ID || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID  || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME)
    {
      if (strncasecmp(st,"GRIB-",5) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::GRIB_ID;
        parameterKey = st+5;
      }
      else
      if (strncasecmp(st,"NB-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_ID;
        parameterKey = st+3;
      }
      else
      if (strncasecmp(st,"NBN-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_NAME;
        parameterKey = st+4;
      }
    }

    return getContentByParameterIdAndTimeRange(parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
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

    contentInfoList.clear();

    char st[100];
    strcpy(st,parameterKey.c_str());

    if (parameterKeyType == T::ParamKeyTypeValue::FMI_NAME || parameterKeyType == T::ParamKeyTypeValue::GRIB_ID || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID  || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME)
    {
      if (strncasecmp(st,"GRIB-",5) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::GRIB_ID;
        parameterKey = st+5;
      }
      else
      if (strncasecmp(st,"NB-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_ID;
        parameterKey = st+3;
      }
      else
      if (strncasecmp(st,"NBN-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_NAME;
        parameterKey = st+4;
      }
    }

    return getContentByParameterIdAndGeneration(generationInfo.mGenerationId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
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

    contentInfoList.clear();

    char st[100];
    strcpy(st,parameterKey.c_str());

    if (parameterKeyType == T::ParamKeyTypeValue::FMI_NAME || parameterKeyType == T::ParamKeyTypeValue::GRIB_ID || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID  || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME)
    {
      if (strncasecmp(st,"GRIB-",5) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::GRIB_ID;
        parameterKey = st+5;
      }
      else
      if (strncasecmp(st,"NB-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_ID;
        parameterKey = st+3;
      }
      else
      if (strncasecmp(st,"NBN-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_NAME;
        parameterKey = st+4;
      }
    }

    return getContentByParameterIdAndGeneration(generationInfo.mGenerationId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
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

    char st[100];
    strcpy(st,parameterKey.c_str());

    if (parameterKeyType == T::ParamKeyTypeValue::FMI_NAME || parameterKeyType == T::ParamKeyTypeValue::GRIB_ID || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID  || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME)
    {
      if (strncasecmp(st,"GRIB-",5) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::GRIB_ID;
        parameterKey = st+5;
      }
      else
      if (strncasecmp(st,"NB-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_ID;
        parameterKey = st+3;
      }
      else
      if (strncasecmp(st,"NBN-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_NAME;
        parameterKey = st+4;
      }
    }

    contentInfoList.clear();

    return getContentByParameterIdAndProducer(producerInfo.mProducerId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,T::ContentInfoList& contentInfoList)
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

    char st[100];
    strcpy(st,parameterKey.c_str());

    if (parameterKeyType == T::ParamKeyTypeValue::FMI_NAME || parameterKeyType == T::ParamKeyTypeValue::GRIB_ID || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID  || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME)
    {
      if (strncasecmp(st,"GRIB-",5) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::GRIB_ID;
        parameterKey = st+5;
      }
      else
      if (strncasecmp(st,"NB-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_ID;
        parameterKey = st+3;
      }
      else
      if (strncasecmp(st,"NBN-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_NAME;
        parameterKey = st+4;
      }
    }

    contentInfoList.clear();

    std::string startTime = "19000101T000000";
    std::string endTime = "23000101T000000";

    T::ContentInfoList contentList;
    int res = getContentByParameterIdAndGeneration(generationInfo.mGenerationId,parameterKeyType,parameterKey,parameterLevelId,level,level,forecastType,forecastNumber,geometryId,startTime,endTime,contentList);

//    contentInfoList = contentList;
//#if 0
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
//#endif
    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
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

    char st[100];
    strcpy(st,parameterKey.c_str());

    if (parameterKeyType == T::ParamKeyTypeValue::FMI_NAME || parameterKeyType == T::ParamKeyTypeValue::GRIB_ID || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID  || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME)
    {
      if (strncasecmp(st,"GRIB-",5) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::GRIB_ID;
        parameterKey = st+5;
      }
      else
      if (strncasecmp(st,"NB-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_ID;
        parameterKey = st+3;
      }
      else
      if (strncasecmp(st,"NBN-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_NAME;
        parameterKey = st+4;
      }
    }

    contentInfoList.clear();

    return getContentByParameterIdAndProducer(producerInfo.mProducerId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    contentInfoList.clear();

    T::ProducerInfoList producerInfoList;
    getProducerList(producerInfoList);

    T::GenerationInfoList generationInfoList;
    getGenerationList(generationInfoList);

    T::FileInfoList fileInfoList;
    getFileList(0,10000000,fileInfoList);
    fileInfoList.sort(T::FileInfo::ComparisonMethod::fileId);

    T::ContentInfoList contentList;
    getContent(0,0,10000000,contentList);

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





int PostgresqlImplementation::_getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<T::GeometryId>& geometryIdList)
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

    geometryIdList.clear();

    T::ContentInfoList contentInfoList;
    int res = getContentByGenerationId(generationInfo.mGenerationId,0,0,1000000,contentInfoList);

    contentInfoList.getContentGeometryIdListByGenerationId(generationInfo.mProducerId,generationId,geometryIdList);
    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList)
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

    contentParamList.clear();

    T::ContentInfoList contentInfoList;
    int res = getContentByGenerationId(generationInfo.mGenerationId,0,0,1000000,contentInfoList);

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





int PostgresqlImplementation::_getContentParamKeyListByGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
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

    paramKeyList.clear();

    T::ContentInfoList contentInfoList;
    int res = getContentByGenerationId(generationInfo.mGenerationId,0,0,1000000,contentInfoList);
    contentInfoList.getContentParamKeyListByGenerationId(generationInfo.mProducerId,generationId,parameterKeyType,paramKeyList);

    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::set<std::string>& contentTimeList)
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

    contentTimeList.clear();

    T::ContentInfoList contentInfoList;
    int res = getContentByGenerationId(generationInfo.mGenerationId,0,0,1000000,contentInfoList);
    contentInfoList.getForecastTimeListByGenerationId(generationInfo.mProducerId,generationId,contentTimeList);

    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList)
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

    contentTimeList.clear();

    T::ContentInfoList contentInfoList;
    int res = getContentByGenerationId(generationInfo.mGenerationId,0,0,1000000,contentInfoList);
    contentInfoList.getForecastTimeListByGenerationAndGeometry(generationInfo.mProducerId,generationId,geometryId,contentTimeList);
    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getContentTimeListByProducerId(T::SessionId sessionId,uint producerId,std::set<std::string>& contentTimeList)
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

    contentTimeList.clear();

    T::ContentInfoList contentInfoList;
    int res = getContentByProducerId(producerId,0,0,1000000,contentInfoList);
    contentInfoList.getForecastTimeListByProducerId(producerId,contentTimeList);

    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::_getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

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





int PostgresqlImplementation::_getContentCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    count = 0;

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    count = getCount("SELECT COUNT(*) FROM content");

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int PostgresqlImplementation::_getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ContentInfoList contentList;
    getContent(0,0,10000000,contentList);

    levelInfoList.clear();

    contentList.getLevelInfoList(levelInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





// *****************************************************************************************************************************************
// *****************************************************************************************************************************************
// *****************************************************************************************************************************************



uint PostgresqlImplementation::getCount(const char *sql)
{
  FUNCTION_TRACE
  try
  {
    uint count = 0;

    if (!isConnectionValid())
      return 0;

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);
    if (rowCount == 0)
      return 0;

    count = atoll(PQgetvalue(res, 0, 0));

    PQclear(res);

    return count;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::deleteProducerById(uint producerId,bool deleteGenerations,bool deleteGenerationGeometries,bool deleteFiles,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[1000];

    /*
    PGresult *res = PQexec(mConnection,"BEGIN TRANSACTION;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      PQclear(res);
      throw exception;
    }
    PQclear(res);
    */
    if (deleteContent)
    {
      sprintf(sql,"DELETE FROM content WHERE producerId=%u;",producerId);
      PGresult *res = PQexec(mConnection,sql);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
        //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
        //PQclear(res);
        //throw exception;
      }
      PQclear(res);
    }

    if (deleteFiles)
    {
      sprintf(sql,"DELETE FROM file WHERE producerId=%u;",producerId);
      PGresult *res = PQexec(mConnection,sql);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
        //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
        //PQclear(res);
        //throw exception;
      }
      PQclear(res);
    }

    if (deleteGenerationGeometries)
    {
      sprintf(sql,"DELETE FROM generationGeometry WHERE producerId=%u;",producerId);
      PGresult *res = PQexec(mConnection,sql);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
        //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
        //PQclear(res);
        //throw exception;
      }
      PQclear(res);
    }

    if (deleteGenerations)
    {
      sprintf(sql,"DELETE FROM generation WHERE producerId=%u;",producerId);
      PGresult *res = PQexec(mConnection,sql);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
        //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
        //PQclear(res);
        //throw exception;
      }
      PQclear(res);
    }

    sprintf(sql,"DELETE FROM producer WHERE producerId=%u;",producerId);
    PGresult *res = PQexec(mConnection,sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      //PQclear(res);
      //throw exception;
    }
    PQclear(res);

    /*
    res = PQexec(mConnection,"COMMIT TRANSACTION;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      PQclear(res);
      throw exception;
    }
    PQclear(res);
    */
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::deleteProducerListBySourceId(uint sourceId,bool deleteGenerations,bool deleteGenerationGeometries,bool deleteFiles,bool deleteContent)
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
      deleteProducerById(producerInfo->mProducerId,deleteGenerations,deleteGenerationGeometries,deleteFiles,deleteContent);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getProducerById(uint producerId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  name,\n");
    p += sprintf(p,"  title,\n");
    p += sprintf(p,"  description,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  producer\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  producerId=%u;\n",producerId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);
    if (rowCount == 0)
      return Result::DATA_NOT_FOUND;

    for (int i = 0; i < rowCount; i++)
    {
      producerInfo.mProducerId = atoi(PQgetvalue(res, i, 0));
      producerInfo.mName = PQgetvalue(res, i, 1);
      producerInfo.mTitle = PQgetvalue(res, i, 2);
      producerInfo.mDescription = PQgetvalue(res, i, 3);
      producerInfo.mFlags = atoi(PQgetvalue(res, i, 4));
      producerInfo.mSourceId = atoi(PQgetvalue(res, i, 5));
      producerInfo.mStatus = atoi(PQgetvalue(res, i, 6));
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getProducerByName(std::string producerName,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  name,\n");
    p += sprintf(p,"  title,\n");
    p += sprintf(p,"  description,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  producer\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  LOWER(name)=LOWER('%s');\n",producerName.c_str());

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);
    if (rowCount == 0)
      return Result::DATA_NOT_FOUND;

    for (int i = 0; i < rowCount; i++)
    {
      producerInfo.mProducerId = atoi(PQgetvalue(res, i, 0));
      producerInfo.mName = PQgetvalue(res, i, 1);
      producerInfo.mTitle = PQgetvalue(res, i, 2);
      producerInfo.mDescription = PQgetvalue(res, i, 3);
      producerInfo.mFlags = atoi(PQgetvalue(res, i, 4));
      producerInfo.mSourceId = atoi(PQgetvalue(res, i, 5));
      producerInfo.mStatus = atoi(PQgetvalue(res, i, 6));
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getProducerList(T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    producerInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  name,\n");
    p += sprintf(p,"  title,\n");
    p += sprintf(p,"  description,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  producer;\n");

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::ProducerInfo *producerInfo = new T::ProducerInfo();
      producerInfo->mProducerId = atoi(PQgetvalue(res, i, 0));
      producerInfo->mName = PQgetvalue(res, i, 1);
      producerInfo->mTitle = PQgetvalue(res, i, 2);
      producerInfo->mDescription = PQgetvalue(res, i, 3);
      producerInfo->mFlags = atoi(PQgetvalue(res, i, 4));
      producerInfo->mSourceId = atoi(PQgetvalue(res, i, 5));
      producerInfo->mStatus = atoi(PQgetvalue(res, i, 6));
      producerInfoList.addProducerInfo(producerInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getProducerListBySourceId(uint sourceId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    producerInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;


    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  name,\n");
    p += sprintf(p,"  title,\n");
    p += sprintf(p,"  description,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  producer\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  sourceId=%u;\n",sourceId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      PQclear(res);
      exception.addParameter("sql",sql);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::ProducerInfo *producerInfo = new T::ProducerInfo();
      producerInfo->mProducerId = atoi(PQgetvalue(res, i, 0));
      producerInfo->mName = PQgetvalue(res, i, 1);
      producerInfo->mTitle = PQgetvalue(res, i, 2);
      producerInfo->mDescription = PQgetvalue(res, i, 3);
      producerInfo->mFlags = atoi(PQgetvalue(res, i, 4));
      producerInfo->mSourceId = atoi(PQgetvalue(res, i, 5));
      producerInfo->mStatus = atoi(PQgetvalue(res, i, 6));
      producerInfoList.addProducerInfo(producerInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getGenerationById(uint generationId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  generationType,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  name,\n");
    p += sprintf(p,"  description,\n");
    p += sprintf(p,"  to_char(analysisTime, 'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  generation\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  generationId=%u;\n",generationId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);
    if (rowCount == 0)
      return Result::DATA_NOT_FOUND;

    for (int i = 0; i < rowCount; i++)
    {
      generationInfo.mGenerationId = atoi(PQgetvalue(res, i, 0));
      generationInfo.mGenerationType = atoi(PQgetvalue(res, i, 1));
      generationInfo.mProducerId = atoi(PQgetvalue(res, i, 2));
      generationInfo.mName = PQgetvalue(res, i, 3);
      generationInfo.mDescription = PQgetvalue(res, i, 4);
      generationInfo.mAnalysisTime = PQgetvalue(res, i, 5);
      generationInfo.mFlags = atoi(PQgetvalue(res, i, 6));
      generationInfo.mSourceId = atoi(PQgetvalue(res, i, 7));

      std::string modificationTime = PQgetvalue(res, i, 8);
      generationInfo.mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 9);
      generationInfo.mDeletionTime = utcTimeToTimeT(deletionTime);

      generationInfo.mStatus = atoi(PQgetvalue(res, i, 10));
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getGenerationByName(std::string generationName,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  generationType,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  name,\n");
    p += sprintf(p,"  description,\n");
    p += sprintf(p,"  to_char(analysisTime, 'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  generation\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  LOWER(name)=LOWER('%s');\n",generationName.c_str());

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);
    if (rowCount == 0)
      return Result::DATA_NOT_FOUND;

    for (int i = 0; i < rowCount; i++)
    {
      generationInfo.mGenerationId = atoi(PQgetvalue(res, i, 0));
      generationInfo.mGenerationType = atoi(PQgetvalue(res, i, 1));
      generationInfo.mProducerId = atoi(PQgetvalue(res, i, 2));
      generationInfo.mName = PQgetvalue(res, i, 3);
      generationInfo.mDescription = PQgetvalue(res, i, 4);
      generationInfo.mAnalysisTime = PQgetvalue(res, i, 5);
      generationInfo.mFlags = atoi(PQgetvalue(res, i, 6));
      generationInfo.mSourceId = atoi(PQgetvalue(res, i, 7));

      std::string modificationTime = PQgetvalue(res, i, 8);
      generationInfo.mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 9);
      generationInfo.mDeletionTime = utcTimeToTimeT(deletionTime);

      generationInfo.mStatus = atoi(PQgetvalue(res, i, 10));
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getGenerationList(T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    generationInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  generationType,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  name,\n");
    p += sprintf(p,"  description,\n");
    p += sprintf(p,"  to_char(analysisTime, 'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  generation\n");

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::GenerationInfo *generationInfo = new T::GenerationInfo();

      generationInfo->mGenerationId = atoi(PQgetvalue(res, i, 0));
      generationInfo->mGenerationType = atoi(PQgetvalue(res, i, 1));
      generationInfo->mProducerId = atoi(PQgetvalue(res, i, 2));
      generationInfo->mName = PQgetvalue(res, i, 3);
      generationInfo->mDescription = PQgetvalue(res, i, 4);
      generationInfo->mAnalysisTime = PQgetvalue(res, i, 5);
      generationInfo->mFlags = atoi(PQgetvalue(res, i, 6));
      generationInfo->mSourceId = atoi(PQgetvalue(res, i, 7));

      std::string modificationTime = PQgetvalue(res, i, 8);
      generationInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 9);
      generationInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      generationInfo->mStatus = atoi(PQgetvalue(res, i, 10));

      generationInfoList.addGenerationInfo(generationInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getGenerationListByGeometryId(T::GeometryId geometryId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    generationInfoList.clear();

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getGenerationListByProducerId(uint producerId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    generationInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  generationType,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  name,\n");
    p += sprintf(p,"  description,\n");
    p += sprintf(p,"  to_char(analysisTime, 'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  generation\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  producerId=%u;\n",producerId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::GenerationInfo *generationInfo = new T::GenerationInfo();

      generationInfo->mGenerationId = atoi(PQgetvalue(res, i, 0));
      generationInfo->mGenerationType = atoi(PQgetvalue(res, i, 1));
      generationInfo->mProducerId = atoi(PQgetvalue(res, i, 2));
      generationInfo->mName = PQgetvalue(res, i, 3);
      generationInfo->mDescription = PQgetvalue(res, i, 4);
      generationInfo->mAnalysisTime = PQgetvalue(res, i, 5);
      generationInfo->mFlags = atoi(PQgetvalue(res, i, 6));
      generationInfo->mSourceId = atoi(PQgetvalue(res, i, 7));

      std::string modificationTime = PQgetvalue(res, i, 8);
      generationInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 9);
      generationInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      generationInfo->mStatus = atoi(PQgetvalue(res, i, 10));

      generationInfoList.addGenerationInfo(generationInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getGenerationListBySourceId(uint sourceId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    generationInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  generationType,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  name,\n");
    p += sprintf(p,"  description,\n");
    p += sprintf(p,"  to_char(analysisTime, 'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  generation\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  sourceId=%u\n",sourceId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::GenerationInfo *generationInfo = new T::GenerationInfo();

      generationInfo->mGenerationId = atoi(PQgetvalue(res, i, 0));
      generationInfo->mGenerationType = atoi(PQgetvalue(res, i, 1));
      generationInfo->mProducerId = atoi(PQgetvalue(res, i, 2));
      generationInfo->mName = PQgetvalue(res, i, 3);
      generationInfo->mDescription = PQgetvalue(res, i, 4);
      generationInfo->mAnalysisTime = PQgetvalue(res, i, 5);
      generationInfo->mFlags = atoi(PQgetvalue(res, i, 6));
      generationInfo->mSourceId = atoi(PQgetvalue(res, i, 7));

      std::string modificationTime = PQgetvalue(res, i, 8);
      generationInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 9);
      generationInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      generationInfo->mStatus = atoi(PQgetvalue(res, i, 10));

      generationInfoList.addGenerationInfo(generationInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::deleteGenerationById(uint generationId,bool deleteGenerationGeometries,bool deleteFiles,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[1000];
/*
    PGresult *res = PQexec(mConnection,"BEGIN TRANSACTION;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      throw exception;
    }
    PQclear(res);
*/
    if (deleteContent)
    {
      sprintf(sql,"DELETE FROM content WHERE generationId=%u;",generationId);
      PGresult *res = PQexec(mConnection,sql);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
        //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
        //PQclear(res);
        //throw exception;
      }
      PQclear(res);
    }

    if (deleteFiles)
    {
      sprintf(sql,"DELETE FROM file WHERE generationId=%u;",generationId);
      PGresult *res = PQexec(mConnection,sql);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
        //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
        //PQclear(res);
        //throw exception;
      }
      PQclear(res);
    }

    if (deleteGenerationGeometries)
    {
      sprintf(sql,"DELETE FROM generationGeometry WHERE generationId=%u;",generationId);
      PGresult *res = PQexec(mConnection,sql);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
        //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
        //PQclear(res);
        //throw exception;
      }
      PQclear(res);
    }

    sprintf(sql,"DELETE FROM generation WHERE generationId=%u;",generationId);
    PGresult *res = PQexec(mConnection,sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      //PQclear(res);
      //throw exception;
    }
    PQclear(res);
/*
    res = PQexec(mConnection,"COMMIT TRANSACTION;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      PQclear(res);
      throw exception;
    }
    PQclear(res);
*/
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::addFile(T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;
/*
    T::ProducerInfo producerInfo;
    if (getProducerById(fileInfo.mProducerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo generationInfo;
    if (getGenerationById(fileInfo.mGenerationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    if (producerInfo.mProducerId != generationInfo.mProducerId)
      return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;

    T::FileInfo fInfo;
    if (getFileByName(fileInfo.mName,fInfo) == Result::OK)
      return Result::FILE_NAME_ALREADY_REGISTERED;

*/
    char sql[10000];
    char *p = sql;

    std::string modificationTime = utcTimeFromTimeT(fileInfo.mModificationTime);
    std::string deletionTime = utcTimeFromTimeT(fileInfo.mDeletionTime);

    p += sprintf(p,"INSERT INTO file\n");
    p += sprintf(p,"(\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  protocol,\n");
    p += sprintf(p,"  serverType,\n");
    p += sprintf(p,"  server,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  fileName,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  modificationTime,\n");
    p += sprintf(p,"  deletionTime,\n");
    p += sprintf(p,"  fileSize,\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,")\n");
    p += sprintf(p,"VALUES \n");
    p += sprintf(p,"(\n");
    p += sprintf(p,"  DEFAULT,\n");
    p += sprintf(p,"  %u,\n",fileInfo.mProducerId);
    p += sprintf(p,"  %u,\n",fileInfo.mGenerationId);
    p += sprintf(p,"  %u,\n",fileInfo.mProtocol);
    p += sprintf(p,"  %u,\n",fileInfo.mServerType);
    p += sprintf(p,"  '%s',\n",fileInfo.mServer.c_str());
    p += sprintf(p,"  %u,\n",fileInfo.mFileType);
    p += sprintf(p,"  '%s',\n",fileInfo.mName.c_str());
    p += sprintf(p,"  %u,\n",fileInfo.mFlags);
    p += sprintf(p,"  %u,\n",fileInfo.mSourceId);
    p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",modificationTime.c_str());
    p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",deletionTime.c_str());
    p += sprintf(p,"  %llu,\n",fileInfo.mSize);
    p += sprintf(p,"  %u\n",fileInfo.mStatus);
    p += sprintf(p,");\n");

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    int result = getFileByName(fileInfo.mName,fileInfo);
    if (result != Result::OK)
      return result;

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::addFileList(T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;
/*
    T::ProducerInfo producerInfo;
    if (getProducerById(fileInfo.mProducerId,producerInfo) != Result::OK)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo generationInfo;
    if (getGenerationById(fileInfo.mGenerationId,generationInfo) != Result::OK)
      return Result::UNKNOWN_GENERATION_ID;

    if (producerInfo.mProducerId != generationInfo.mProducerId)
      return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;

    T::FileInfo fInfo;
    if (getFileByName(fileInfo.mName,fInfo) == Result::OK)
      return Result::FILE_NAME_ALREADY_REGISTERED;

*/
    uint len = fileInfoList.getLength();
    uint c = 0;


    /*
    PGresult *res = PQexec(mConnection,"BEGIN TRANSACTION;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      PQclear(res);
      throw exception;
    }
    PQclear(res);
    */

    PGresult *res = PQexec(mConnection,"SELECT MAX(fileId) FROM file;");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      PQclear(res);
      throw exception;
    }

    uint maxFileId = 0;
    int rowCount = PQntuples(res);
    if (rowCount > 0)
      maxFileId = atoi(PQgetvalue(res, 0, 0));

    PQclear(res);


    char sql[1000000];
    char *p = sql;

    p += sprintf(p,"INSERT INTO file\n");
    p += sprintf(p,"(\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  protocol,\n");
    p += sprintf(p,"  serverType,\n");
    p += sprintf(p,"  server,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  fileName,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  modificationTime,\n");
    p += sprintf(p,"  deletionTime,\n");
    p += sprintf(p,"  fileSize,\n");
    p += sprintf(p,"  status\n");
    p += sprintf(p,")\n");
    p += sprintf(p,"VALUES \n");

    char *pp = p;
    while (c < len)
    {
      maxFileId++;
      T::FileInfo *fileInfo  = fileInfoList.getFileInfoByIndex(c);
      fileInfo->mFileId = maxFileId;

      std::string modificationTime = utcTimeFromTimeT(fileInfo->mModificationTime);
      std::string deletionTime = utcTimeFromTimeT(fileInfo->mDeletionTime);

      p += sprintf(p,"(\n");
      p += sprintf(p,"  %u,\n",fileInfo->mFileId);
      p += sprintf(p,"  %u,\n",fileInfo->mProducerId);
      p += sprintf(p,"  %u,\n",fileInfo->mGenerationId);
      p += sprintf(p,"  %u,\n",fileInfo->mProtocol);
      p += sprintf(p,"  %u,\n",fileInfo->mServerType);
      p += sprintf(p,"  '%s',\n",fileInfo->mServer.c_str());
      p += sprintf(p,"  %u,\n",fileInfo->mFileType);
      p += sprintf(p,"  '%s',\n",fileInfo->mName.c_str());
      p += sprintf(p,"  %u,\n",fileInfo->mFlags);
      p += sprintf(p,"  %u,\n",fileInfo->mSourceId);
      p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",modificationTime.c_str());
      p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",deletionTime.c_str());
      p += sprintf(p,"  %llu,\n",fileInfo->mSize);
      p += sprintf(p,"  %u\n",fileInfo->mStatus);
      p += sprintf(p,")\n");

      if ((p-sql) > 990000 || (c+1) == len)
      {
        PGresult *res = PQexec(mConnection, sql);
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
          if (PQresultStatus(res) == PGRES_FATAL_ERROR)
            closeConnection();

          /*
          Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
          exception.addParameter("sql",sql);
          PQclear(res);
          throw exception;
          */
        }
        PQclear(res);
        p = pp;
      }
      else
      {
        p += sprintf(p,",\n");
      }
      c++;
    }

    /*
    res = PQexec(mConnection,"COMMIT TRANSACTION;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      PQclear(res);
      throw exception;
    }
    PQclear(res);
    */

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::deleteGenerationListByProducerId(uint producerId,bool deleteGenerationGeometries,bool deleteFiles,bool deleteContent)
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
      deleteGenerationById(generationInfo->mGenerationId,deleteGenerationGeometries,deleteFiles,deleteContent);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::deleteGenerationListBySourceId(uint sourceId,bool deleteGenerationGeometries,bool deleteFiles,bool deleteContent)
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
      deleteGenerationById(generationInfo->mGenerationId,deleteGenerationGeometries,deleteFiles,deleteContent);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::deleteFileById(uint fileId,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[1000];

    /*
    PGresult *res = PQexec(mConnection,"BEGIN TRANSACTION;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      PQclear(res);
      throw exception;
    }
    PQclear(res);
    */

    if (deleteContent)
    {
      sprintf(sql,"DELETE FROM content WHERE fileId=%u;",fileId);
      PGresult *res = PQexec(mConnection,sql);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
        //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
        //PQclear(res);
        //throw exception;
      }
      PQclear(res);
    }

    sprintf(sql,"DELETE FROM file WHERE fileId=%u;",fileId);
    PGresult *res = PQexec(mConnection,sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      //PQclear(res);
      //throw exception;
    }
    PQclear(res);

    /*
    res = PQexec(mConnection,"COMMIT TRANSACTION;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      throw exception;
    }
    PQclear(res);
    */

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::deleteFileListByGenerationId(uint generationId,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[1000];

    /*
    PGresult *res = PQexec(mConnection,"BEGIN TRANSACTION;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      PQclear(res);
      throw exception;
    }
    PQclear(res);
    */

    if (deleteContent)
    {
      sprintf(sql,"DELETE FROM content WHERE generationId=%u;",generationId);
      PGresult *res = PQexec(mConnection,sql);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
        //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
        //PQclear(res);
        //throw exception;
      }
      PQclear(res);
    }

    sprintf(sql,"DELETE FROM file WHERE generationId=%u;",generationId);
    PGresult *res = PQexec(mConnection,sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      //PQclear(res);
      //throw exception;
    }
    PQclear(res);

    /*
    res = PQexec(mConnection,"COMMIT TRANSACTION;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      PQclear(res);
      throw exception;
    }
    PQclear(res);
    */

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::deleteFileListByGenerationIdList(std::set<uint>& generationIdList,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    for (auto it = generationIdList.begin(); it != generationIdList.begin(); ++it)
      deleteFileListByGenerationId(*it,deleteContent);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::deleteFileListByProducerId(uint producerId,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[1000];

    /*
    PGresult *res = PQexec(mConnection,"BEGIN TRANSACTION;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      PQclear(res);
      throw exception;
    }
    PQclear(res);
    */

    if (deleteContent)
    {
      sprintf(sql,"DELETE FROM content WHERE producerId=%u;",producerId);
      PGresult *res = PQexec(mConnection,sql);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
        //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
        //exception.addParameter("sql",sql);
        //PQclear(res);
        //throw exception;
      }
      PQclear(res);
    }

    sprintf(sql,"DELETE FROM file WHERE producerId=%u;",producerId);
    PGresult *res = PQexec(mConnection,sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      //exception.addParameter("sql",sql);
      //PQclear(res);
      //throw exception;
    }
    PQclear(res);

    /*
    res = PQexec(mConnection,"COMMIT TRANSACTION;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      PQclear(res);
      throw exception;
    }
    PQclear(res);
    */
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::deleteFileListBySourceId(uint sourceId,bool deleteContent)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[1000];

    /*
    PGresult *res = PQexec(mConnection,"BEGIN TRANSACTION;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      PQclear(res);
      throw exception;
    }
    PQclear(res);
    */

    if (deleteContent)
    {
      sprintf(sql,"DELETE FROM content WHERE sourceId=%u;",sourceId);
      PGresult *res = PQexec(mConnection,sql);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
        //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
        //exception.addParameter("sql",sql);
        //PQclear(res);
        //throw exception;
      }
      PQclear(res);
    }

    sprintf(sql,"DELETE FROM file WHERE sourceId=%u;",sourceId);
    PGresult *res = PQexec(mConnection,sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      //exception.addParameter("sql",sql);
      //PQclear(res);
      //throw exception;
    }
    PQclear(res);

    /*
    res = PQexec(mConnection,"COMMIT TRANSACTION;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      PQclear(res);
      throw exception;
    }
    PQclear(res);
    */

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getFileById(uint fileId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  protocol,\n");
    p += sprintf(p,"  serverType,\n");
    p += sprintf(p,"  server,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  fileName,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  file\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  fileId=%u;\n",fileId);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);
    if (rowCount == 0)
      return Result::DATA_NOT_FOUND;

    for (int i = 0; i < rowCount; i++)
    {
      fileInfo.mFileId = atoi(PQgetvalue(res, i, 0));
      fileInfo.mProducerId = atoi(PQgetvalue(res, i, 1));
      fileInfo.mGenerationId = atoi(PQgetvalue(res, i, 2));
      fileInfo.mProtocol = atoi(PQgetvalue(res, i, 3));
      fileInfo.mServerType = atoi(PQgetvalue(res, i, 4));
      fileInfo.mServer = PQgetvalue(res, i, 5);
      fileInfo.mFileType = atoi(PQgetvalue(res, i, 6));
      fileInfo.mName = PQgetvalue(res, i, 7);
      fileInfo.mFlags = atoi(PQgetvalue(res, i, 8));
      fileInfo.mSourceId = atoi(PQgetvalue(res, i, 9));

      std::string modificationTime = PQgetvalue(res, i, 10);
      fileInfo.mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 11);
      fileInfo.mDeletionTime = utcTimeToTimeT(deletionTime);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getFileByName(std::string fileName,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  protocol,\n");
    p += sprintf(p,"  serverType,\n");
    p += sprintf(p,"  server,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  fileName,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  file\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  fileName='%s';\n",fileName.c_str());

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);
    if (rowCount == 0)
      return Result::DATA_NOT_FOUND;

    for (int i = 0; i < rowCount; i++)
    {
      fileInfo.mFileId = atoi(PQgetvalue(res, i, 0));
      fileInfo.mProducerId = atoi(PQgetvalue(res, i, 1));
      fileInfo.mGenerationId = atoi(PQgetvalue(res, i, 2));
      fileInfo.mProtocol = atoi(PQgetvalue(res, i, 3));
      fileInfo.mServerType = atoi(PQgetvalue(res, i, 4));
      fileInfo.mServer = PQgetvalue(res, i, 5);
      fileInfo.mFileType = atoi(PQgetvalue(res, i, 6));
      fileInfo.mName = PQgetvalue(res, i, 7);
      fileInfo.mFlags = atoi(PQgetvalue(res, i, 8));
      fileInfo.mSourceId = atoi(PQgetvalue(res, i, 9));

      std::string modificationTime = PQgetvalue(res, i, 10);
      fileInfo.mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 11);
      fileInfo.mDeletionTime = utcTimeToTimeT(deletionTime);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int PostgresqlImplementation::getFileList(uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  protocol,\n");
    p += sprintf(p,"  serverType,\n");
    p += sprintf(p,"  server,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  fileName,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  file\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  fileId >= %u\n",startFileId);
    p += sprintf(p,"ORDER BY\n");
    p += sprintf(p,"  fileId\n");
    p += sprintf(p,"LIMIT\n");
    p += sprintf(p,"  %u\n",maxRecords);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::FileInfo *fileInfo = new T::FileInfo();
      fileInfo->mFileId = atoi(PQgetvalue(res, i, 0));
      fileInfo->mProducerId = atoi(PQgetvalue(res, i, 1));
      fileInfo->mGenerationId = atoi(PQgetvalue(res, i, 2));
      fileInfo->mProtocol = atoi(PQgetvalue(res, i, 3));
      fileInfo->mServerType = atoi(PQgetvalue(res, i, 4));
      fileInfo->mServer = PQgetvalue(res, i, 5);
      fileInfo->mFileType = atoi(PQgetvalue(res, i, 6));
      fileInfo->mName = PQgetvalue(res, i, 7);
      fileInfo->mFlags = atoi(PQgetvalue(res, i, 8));
      fileInfo->mSourceId = atoi(PQgetvalue(res, i, 9));

      std::string modificationTime = PQgetvalue(res, i, 10);
      fileInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 11);
      fileInfo->mDeletionTime = utcTimeToTimeT(deletionTime);
      fileInfoList.addFileInfo(fileInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getFileListByGenerationId(uint generationId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  protocol,\n");
    p += sprintf(p,"  serverType,\n");
    p += sprintf(p,"  server,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  fileName,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  file\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  generationId=%u AND fileId >= %u\n",generationId,startFileId);
    p += sprintf(p,"ORDER BY\n");
    p += sprintf(p,"  fileId\n");
    p += sprintf(p,"LIMIT\n");
    p += sprintf(p,"  %u\n",maxRecords);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::FileInfo *fileInfo = new T::FileInfo();
      fileInfo->mFileId = atoi(PQgetvalue(res, i, 0));
      fileInfo->mProducerId = atoi(PQgetvalue(res, i, 1));
      fileInfo->mGenerationId = atoi(PQgetvalue(res, i, 2));
      fileInfo->mProtocol = atoi(PQgetvalue(res, i, 3));
      fileInfo->mServerType = atoi(PQgetvalue(res, i, 4));
      fileInfo->mServer = PQgetvalue(res, i, 5);
      fileInfo->mFileType = atoi(PQgetvalue(res, i, 6));
      fileInfo->mName = PQgetvalue(res, i, 7);
      fileInfo->mFlags = atoi(PQgetvalue(res, i, 8));
      fileInfo->mSourceId = atoi(PQgetvalue(res, i, 9));

      std::string modificationTime = PQgetvalue(res, i, 10);
      fileInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 11);
      fileInfo->mDeletionTime = utcTimeToTimeT(deletionTime);
      fileInfoList.addFileInfo(fileInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getFileListByGenerationIdList(std::set<uint>& generationIdList,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    /*
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
          T::FileInfo *fileInfo = new T::FileInfo();
          fileInfo->setCsv(reply->element[t]->str);

          if (fileInfo->mFileId >= startFileId)
          {
            startFileId = fileInfo->mFileId + 1;
            if (generationIdList.find(fileInfo->mGenerationId) != generationIdList.end())
              fileInfoList.addFileInfo(fileInfo);
            else
              delete fileInfo;

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
        freeReplyObject(reply);
      }
      else
      {
        freeReplyObject(reply);
        return Result::OK;
      }
    }
    */
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getFileListByProducerId(uint producerId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  protocol,\n");
    p += sprintf(p,"  serverType,\n");
    p += sprintf(p,"  server,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  fileName,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  file\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  producerId=%u AND fileId >= %u\n",producerId,startFileId);
    p += sprintf(p,"ORDER BY\n");
    p += sprintf(p,"  fileId\n");
    p += sprintf(p,"LIMIT\n");
    p += sprintf(p,"  %u\n",maxRecords);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::FileInfo *fileInfo = new T::FileInfo();
      fileInfo->mFileId = atoi(PQgetvalue(res, i, 0));
      fileInfo->mProducerId = atoi(PQgetvalue(res, i, 1));
      fileInfo->mGenerationId = atoi(PQgetvalue(res, i, 2));
      fileInfo->mProtocol = atoi(PQgetvalue(res, i, 3));
      fileInfo->mServerType = atoi(PQgetvalue(res, i, 4));
      fileInfo->mServer = PQgetvalue(res, i, 5);
      fileInfo->mFileType = atoi(PQgetvalue(res, i, 6));
      fileInfo->mName = PQgetvalue(res, i, 7);
      fileInfo->mFlags = atoi(PQgetvalue(res, i, 8));
      fileInfo->mSourceId = atoi(PQgetvalue(res, i, 9));

      std::string modificationTime = PQgetvalue(res, i, 10);
      fileInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 11);
      fileInfo->mDeletionTime = utcTimeToTimeT(deletionTime);
      fileInfoList.addFileInfo(fileInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getFileListBySourceId(uint sourceId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  protocol,\n");
    p += sprintf(p,"  serverType,\n");
    p += sprintf(p,"  server,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  fileName,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  file\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  sourceId=%u AND fileId >= %u\n",sourceId,startFileId);
    p += sprintf(p,"ORDER BY\n");
    p += sprintf(p,"  fileId\n");
    p += sprintf(p,"LIMIT\n");
    p += sprintf(p,"  %u\n",maxRecords);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::FileInfo *fileInfo = new T::FileInfo();
      fileInfo->mFileId = atoi(PQgetvalue(res, i, 0));
      fileInfo->mProducerId = atoi(PQgetvalue(res, i, 1));
      fileInfo->mGenerationId = atoi(PQgetvalue(res, i, 2));
      fileInfo->mProtocol = atoi(PQgetvalue(res, i, 3));
      fileInfo->mServerType = atoi(PQgetvalue(res, i, 4));
      fileInfo->mServer = PQgetvalue(res, i, 5);
      fileInfo->mFileType = atoi(PQgetvalue(res, i, 6));
      fileInfo->mName = PQgetvalue(res, i, 7);
      fileInfo->mFlags = atoi(PQgetvalue(res, i, 8));
      fileInfo->mSourceId = atoi(PQgetvalue(res, i, 9));

      std::string modificationTime = PQgetvalue(res, i, 10);
      fileInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 11);
      fileInfo->mDeletionTime = utcTimeToTimeT(deletionTime);
      fileInfoList.addFileInfo(fileInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::addContent(T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;
/*
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
*/
    char sql[10000];
    char *p = sql;

    std::string forecastTime = utcTimeFromTimeT(contentInfo.mForecastTimeUTC);
    std::string modificationTime = utcTimeFromTimeT(contentInfo.mModificationTime);
    std::string deletionTime = utcTimeFromTimeT(contentInfo.mDeletionTime);

    p += sprintf(p,"INSERT INTO content\n");
    p += sprintf(p,"(\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  messageIndex,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  filePosition,\n");
    p += sprintf(p,"  messageSize,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  parameterId,\n");
    p += sprintf(p,"  parameterName,\n");
    p += sprintf(p,"  forecastTime,\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  level,\n");
    p += sprintf(p,"  forecastType,\n");
    p += sprintf(p,"  foracastNumber,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  aggregationId,\n");
    p += sprintf(p,"  aggregationPeriod,\n");
    p += sprintf(p,"  processingTypeId,\n");
    p += sprintf(p,"  processingTypeValue1,\n");
    p += sprintf(p,"  processingTypeValue2,\n");
    p += sprintf(p,"  modificationTime,\n");
    p += sprintf(p,"  deletionTime\n");
    p += sprintf(p,")\n");
    p += sprintf(p,"VALUES \n");
    p += sprintf(p,"(\n");
    p += sprintf(p,"  %u,\n",contentInfo.mFileId);
    p += sprintf(p,"  %u,\n",contentInfo.mMessageIndex);
    p += sprintf(p,"  %u,\n",contentInfo.mFileType);
    p += sprintf(p,"  %llu,\n",contentInfo.mFilePosition);
    p += sprintf(p,"  %u,\n",contentInfo.mMessageSize);
    p += sprintf(p,"  %u,\n",contentInfo.mProducerId);
    p += sprintf(p,"  %u,\n",contentInfo.mGenerationId);
    p += sprintf(p,"  %u,\n",contentInfo.mGeometryId);
    p += sprintf(p,"  %d,\n",contentInfo.mFmiParameterId);
    p += sprintf(p,"  '%s',\n",contentInfo.getFmiParameterName());
    p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",forecastTime.c_str());
    p += sprintf(p,"  %d,\n",contentInfo.mFmiParameterLevelId);
    p += sprintf(p,"  %d,\n",contentInfo.mParameterLevel);
    p += sprintf(p,"  %d,\n",contentInfo.mForecastType);
    p += sprintf(p,"  %d,\n",contentInfo.mForecastNumber);
    p += sprintf(p,"  %u,\n",contentInfo.mFlags);
    p += sprintf(p,"  %u,\n",contentInfo.mSourceId);
    p += sprintf(p,"  %d,\n",contentInfo.mAggregationId);
    p += sprintf(p,"  %d,\n",contentInfo.mAggregationPeriod);
    p += sprintf(p,"  %d,\n",contentInfo.mProcessingTypeId);
    p += sprintf(p,"  %f,\n",contentInfo.mProcessingTypeValue1);
    p += sprintf(p,"  %f,\n",contentInfo.mProcessingTypeValue2);
    p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",modificationTime.c_str());
    p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS')\n",deletionTime.c_str());
    p += sprintf(p,");\n");

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      //exception.printError();
      PQclear(res);
      return Result::CONTENT_ADDITION_FAILED;
    }
    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int PostgresqlImplementation::addContentList(T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint len = contentInfoList.getLength();
    uint c = 0;

    //PQexec(mConnection,"ALTER TABLE content SET UNLOGGED;");

    char sql[1000000];
    char *p = sql;

    p += sprintf(p,"INSERT INTO content\n");
    p += sprintf(p,"(\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  messageIndex,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  filePosition,\n");
    p += sprintf(p,"  messageSize,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  parameterId,\n");
    p += sprintf(p,"  parameterName,\n");
    p += sprintf(p,"  forecastTime,\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  level,\n");
    p += sprintf(p,"  forecastType,\n");
    p += sprintf(p,"  foracastNumber,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  aggregationId,\n");
    p += sprintf(p,"  aggregationPeriod,\n");
    p += sprintf(p,"  processingTypeId,\n");
    p += sprintf(p,"  processingTypeValue1,\n");
    p += sprintf(p,"  processingTypeValue2,\n");
    p += sprintf(p,"  modificationTime,\n");
    p += sprintf(p,"  deletionTime\n");
    p += sprintf(p,")\n");
    p += sprintf(p,"VALUES \n");

    char *pp = p;

    while (c < len)
    {
      T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(c);

      std::string forecastTime = utcTimeFromTimeT(contentInfo->mForecastTimeUTC);
      std::string modificationTime = utcTimeFromTimeT(contentInfo->mModificationTime);
      std::string deletionTime = utcTimeFromTimeT(contentInfo->mDeletionTime);

      p += sprintf(p,"(\n");
      p += sprintf(p,"  %u,\n",contentInfo->mFileId);
      p += sprintf(p,"  %u,\n",contentInfo->mMessageIndex);
      p += sprintf(p,"  %u,\n",contentInfo->mFileType);
      p += sprintf(p,"  %llu,\n",contentInfo->mFilePosition);
      p += sprintf(p,"  %u,\n",contentInfo->mMessageSize);
      p += sprintf(p,"  %u,\n",contentInfo->mProducerId);
      p += sprintf(p,"  %u,\n",contentInfo->mGenerationId);
      p += sprintf(p,"  %u,\n",contentInfo->mGeometryId);
      p += sprintf(p,"  %d,\n",contentInfo->mFmiParameterId);
      p += sprintf(p,"  '%s',\n",contentInfo->getFmiParameterName());
      p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",forecastTime.c_str());
      p += sprintf(p,"  %d,\n",contentInfo->mFmiParameterLevelId);
      p += sprintf(p,"  %d,\n",contentInfo->mParameterLevel);
      p += sprintf(p,"  %d,\n",contentInfo->mForecastType);
      p += sprintf(p,"  %d,\n",contentInfo->mForecastNumber);
      p += sprintf(p,"  %u,\n",contentInfo->mFlags);
      p += sprintf(p,"  %u,\n",contentInfo->mSourceId);
      p += sprintf(p,"  %d,\n",contentInfo->mAggregationId);
      p += sprintf(p,"  %d,\n",contentInfo->mAggregationPeriod);
      p += sprintf(p,"  %d,\n",contentInfo->mProcessingTypeId);
      p += sprintf(p,"  %f,\n",contentInfo->mProcessingTypeValue1);
      p += sprintf(p,"  %f,\n",contentInfo->mProcessingTypeValue2);
      p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",modificationTime.c_str());
      p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS')\n",deletionTime.c_str());
      p += sprintf(p,")\n");

      if (p-sql > 990000 || (c+1) == len)
      {
        PGresult *res = PQexec(mConnection, sql);
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
          if (PQresultStatus(res) == PGRES_FATAL_ERROR)
            closeConnection();

          Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
          //exception.addParameter("sql",sql);
          exception.printError();
          PQclear(res);
          return Result::CONTENT_ADDITION_FAILED;
        }
        PQclear(res);
        p = pp;
      }
      else
      {
        p += sprintf(p,",\n");
      }
      c++;
    }

    //PQexec(mConnection,"ALTER TABLE content SET LOGGED;");

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int PostgresqlImplementation::deleteContent(uint fileId,uint messageIndex)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[1000];

    sprintf(sql,"DELETE FROM content WHERE fileId=%u AND messageIndex=%u;",fileId,messageIndex);
    PGresult *res = PQexec(mConnection,sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      //exception.addParameter("sql",sql);
      //PQclear(res);
      //throw exception;
    }
    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::deleteContentByFileId(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[1000];

    sprintf(sql,"DELETE FROM content WHERE fileId=%u;",fileId);
    PGresult *res = PQexec(mConnection,sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      //exception.addParameter("sql",sql);
      //PQclear(res);
      //throw exception;
    }
    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::deleteContentByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[1000];

    sprintf(sql,"DELETE FROM content WHERE producerId=%u;",producerId);
    PGresult *res = PQexec(mConnection,sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      //exception.addParameter("sql",sql);
      //PQclear(res);
      //throw exception;
    }
    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::deleteContentByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[1000];

    sprintf(sql,"DELETE FROM content WHERE generationId=%u;",generationId);
    PGresult *res = PQexec(mConnection,sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      //exception.addParameter("sql",sql);
      //PQclear(res);
      //throw exception;
    }
    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::deleteContentByGenerationIdList(std::set<uint>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    for (auto it = generationIdList.begin(); it != generationIdList.end(); ++it)
    {
      deleteContentByGenerationId(*it);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::deleteContentBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[1000];

    sprintf(sql,"DELETE FROM content WHERE sourceId=%u;",sourceId);
    PGresult *res = PQexec(mConnection,sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      //Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      //exception.addParameter("sql",sql);
      //PQclear(res);
      //throw exception;
    }
    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::setContent(T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    /*
    //contentInfo.print(std::cout,0,0);
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
*/
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getContent(uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  messageIndex,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  filePosition,\n");
    p += sprintf(p,"  messageSize,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  parameterId,\n");
    p += sprintf(p,"  parameterName,\n");
    p += sprintf(p,"  to_char(forecastTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  level,\n");
    p += sprintf(p,"  forecastType,\n");
    p += sprintf(p,"  foracastNumber,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  aggregationId,\n");
    p += sprintf(p,"  aggregationPeriod,\n");
    p += sprintf(p,"  processingTypeId,\n");
    p += sprintf(p,"  processingTypeValue1,\n");
    p += sprintf(p,"  processingTypeValue2,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  content\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  fileId=%u AND messageIndex=%u;\n",fileId,messageIndex);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);
    if (rowCount == 0)
      return Result::DATA_NOT_FOUND;

    contentInfo.mFileId = atoi(PQgetvalue(res, 0, 0));
    contentInfo.mMessageIndex = atoi(PQgetvalue(res, 0, 1));
    contentInfo.mFileType = atoi(PQgetvalue(res, 0, 2));
    contentInfo.mFilePosition = atoll(PQgetvalue(res, 0, 3));
    contentInfo.mMessageSize = atoi(PQgetvalue(res, 0, 4));
    contentInfo.mProducerId = atoi(PQgetvalue(res, 0, 5));
    contentInfo.mGenerationId = atoi(PQgetvalue(res, 0, 6));
    contentInfo.mGeometryId = atoi(PQgetvalue(res, 0, 7));
    contentInfo.mFmiParameterId = atoi(PQgetvalue(res, 0, 8));
    contentInfo.setFmiParameterName(PQgetvalue(res, 0, 9));
    contentInfo.setForecastTime(PQgetvalue(res, 0, 10));
    contentInfo.mFmiParameterLevelId = atoi(PQgetvalue(res, 0, 11));
    contentInfo.mParameterLevel = atoi(PQgetvalue(res, 0, 12));
    contentInfo.mForecastType = atoi(PQgetvalue(res, 0, 13));
    contentInfo.mForecastNumber = atoi(PQgetvalue(res, 0, 14));
    contentInfo.mFlags = atoi(PQgetvalue(res, 0, 15));
    contentInfo.mSourceId = atoi(PQgetvalue(res, 0, 16));
    contentInfo.mAggregationId = atoi(PQgetvalue(res, 0, 17));
    contentInfo.mAggregationPeriod = atoi(PQgetvalue(res, 0, 18));
    contentInfo.mProcessingTypeId = atoi(PQgetvalue(res, 0, 19));
    contentInfo.mProcessingTypeValue1 = atof(PQgetvalue(res, 0, 20));
    contentInfo.mProcessingTypeValue2 = atof(PQgetvalue(res, 0, 21));

    std::string modificationTime = PQgetvalue(res, 0, 22);
    contentInfo.mModificationTime = utcTimeToTimeT(modificationTime);

    std::string deletionTime = PQgetvalue(res, 0, 23);
    contentInfo.mDeletionTime = utcTimeToTimeT(deletionTime);

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getContent(uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  messageIndex,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  filePosition,\n");
    p += sprintf(p,"  messageSize,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  parameterId,\n");
    p += sprintf(p,"  parameterName,\n");
    p += sprintf(p,"  to_char(forecastTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  level,\n");
    p += sprintf(p,"  forecastType,\n");
    p += sprintf(p,"  foracastNumber,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  aggregationId,\n");
    p += sprintf(p,"  aggregationPeriod,\n");
    p += sprintf(p,"  processingTypeId,\n");
    p += sprintf(p,"  processingTypeValue1,\n");
    p += sprintf(p,"  processingTypeValue2,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  modificationTime,\n");
    p += sprintf(p,"  deletionTime,\n");
    p += sprintf(p,"  forecastTime\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  content\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  (fileId=%u AND messageIndex>=%u) OR fileId > %u\n",startFileId,startMessageIndex,startFileId);
    p += sprintf(p,"ORDER BY\n");
    p += sprintf(p,"  fileId,messageIndex\n");
    p += sprintf(p,"LIMIT\n");
    p += sprintf(p,"  %u\n",maxRecords);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::ContentInfo *contentInfo = new T::ContentInfo();

      contentInfo->mFileId = atoi(PQgetvalue(res, i, 0));
      contentInfo->mMessageIndex = atoi(PQgetvalue(res, i, 1));
      contentInfo->mFileType = atoi(PQgetvalue(res, i, 2));
      contentInfo->mFilePosition = atoll(PQgetvalue(res, i, 3));
      contentInfo->mMessageSize = atoi(PQgetvalue(res, i, 4));
      contentInfo->mProducerId = atoi(PQgetvalue(res, i, 5));
      contentInfo->mGenerationId = atoi(PQgetvalue(res, i, 6));
      contentInfo->mGeometryId = atoi(PQgetvalue(res, i, 7));
      contentInfo->mFmiParameterId = atoi(PQgetvalue(res, i, 8));
      contentInfo->setFmiParameterName(PQgetvalue(res, i, 9));
      contentInfo->setForecastTime(PQgetvalue(res, i, 10));
      contentInfo->mFmiParameterLevelId = atoi(PQgetvalue(res, i, 11));
      contentInfo->mParameterLevel = atoi(PQgetvalue(res, i, 12));
      contentInfo->mForecastType = atoi(PQgetvalue(res, i, 13));
      contentInfo->mForecastNumber = atoi(PQgetvalue(res, i, 14));
      contentInfo->mFlags = atoi(PQgetvalue(res, i, 15));
      contentInfo->mSourceId = atoi(PQgetvalue(res, i, 16));
      contentInfo->mAggregationId = atoi(PQgetvalue(res, i, 17));
      contentInfo->mAggregationPeriod = atoi(PQgetvalue(res, i, 18));
      contentInfo->mProcessingTypeId = atoi(PQgetvalue(res, i, 19));
      contentInfo->mProcessingTypeValue1 = atof(PQgetvalue(res, i, 20));
      contentInfo->mProcessingTypeValue2 = atof(PQgetvalue(res, i, 21));

      std::string modificationTime = PQgetvalue(res, i, 22);
      contentInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 23);
      contentInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      contentInfoList.addContentInfo(contentInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getGenerationTimeAndGeometryList(std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    list.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    /*
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
          sprintf(st,"%u;%u;%u;%d;%d;%s;%s;",contentInfo.mSourceId,contentInfo.mGenerationId,contentInfo.mGeometryId,contentInfo.mForecastType,contentInfo.mForecastNumber,contentInfo.mForecastTime.c_str(),contentInfo.mModificationTime.c_str());
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
    */
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getContentByGenerationId(uint generationId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  messageIndex,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  filePosition,\n");
    p += sprintf(p,"  messageSize,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  parameterId,\n");
    p += sprintf(p,"  parameterName,\n");
    p += sprintf(p,"  to_char(forecastTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  level,\n");
    p += sprintf(p,"  forecastType,\n");
    p += sprintf(p,"  foracastNumber,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  aggregationId,\n");
    p += sprintf(p,"  aggregationPeriod,\n");
    p += sprintf(p,"  processingTypeId,\n");
    p += sprintf(p,"  processingTypeValue1,\n");
    p += sprintf(p,"  processingTypeValue2,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  content\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  generationId=%u AND ((fileId=%u AND messageIndex>=%u) OR fileId > %u)\n",generationId,startFileId,startMessageIndex,startFileId);
    p += sprintf(p,"ORDER BY\n");
    p += sprintf(p,"  fileId,messageIndex\n");
    p += sprintf(p,"LIMIT\n");
    p += sprintf(p,"  %u\n",maxRecords);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::ContentInfo *contentInfo = new T::ContentInfo();

      contentInfo->mFileId = atoi(PQgetvalue(res, i, 0));
      contentInfo->mMessageIndex = atoi(PQgetvalue(res, i, 1));
      contentInfo->mFileType = atoi(PQgetvalue(res, i, 2));
      contentInfo->mFilePosition = atoll(PQgetvalue(res, i, 3));
      contentInfo->mMessageSize = atoi(PQgetvalue(res, i, 4));
      contentInfo->mProducerId = atoi(PQgetvalue(res, i, 5));
      contentInfo->mGenerationId = atoi(PQgetvalue(res, i, 6));
      contentInfo->mGeometryId = atoi(PQgetvalue(res, i, 7));
      contentInfo->mFmiParameterId = atoi(PQgetvalue(res, i, 8));
      contentInfo->setFmiParameterName(PQgetvalue(res, i, 9));
      contentInfo->setForecastTime(PQgetvalue(res, i, 10));
      contentInfo->mFmiParameterLevelId = atoi(PQgetvalue(res, i, 11));
      contentInfo->mParameterLevel = atoi(PQgetvalue(res, i, 12));
      contentInfo->mForecastType = atoi(PQgetvalue(res, i, 13));
      contentInfo->mForecastNumber = atoi(PQgetvalue(res, i, 14));
      contentInfo->mFlags = atoi(PQgetvalue(res, i, 15));
      contentInfo->mSourceId = atoi(PQgetvalue(res, i, 16));
      contentInfo->mAggregationId = atoi(PQgetvalue(res, i, 17));
      contentInfo->mAggregationPeriod = atoi(PQgetvalue(res, i, 18));
      contentInfo->mProcessingTypeId = atoi(PQgetvalue(res, i, 19));
      contentInfo->mProcessingTypeValue1 = atof(PQgetvalue(res, i, 20));
      contentInfo->mProcessingTypeValue2 = atof(PQgetvalue(res, i, 21));

      std::string modificationTime = PQgetvalue(res, i, 22);
      contentInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 23);
      contentInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      contentInfoList.addContentInfo(contentInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getContentByGenerationIdList(std::set<uint>& generationIdList,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[100000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  messageIndex,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  filePosition,\n");
    p += sprintf(p,"  messageSize,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  parameterId,\n");
    p += sprintf(p,"  parameterName,\n");
    p += sprintf(p,"  to_char(forecastTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  level,\n");
    p += sprintf(p,"  forecastType,\n");
    p += sprintf(p,"  foracastNumber,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  aggregationId,\n");
    p += sprintf(p,"  aggregationPeriod,\n");
    p += sprintf(p,"  processingTypeId,\n");
    p += sprintf(p,"  processingTypeValue1,\n");
    p += sprintf(p,"  processingTypeValue2,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  content\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  generationId IN (");

    char *pp = p;
    for (auto it = generationIdList.begin(); it != generationIdList.end(); ++it)
    {
      if (p != pp)
        p += sprintf(p,",");

      p += sprintf(p,"%u",*it);
    }

    p += sprintf(p,") AND ((fileId=%u AND messageIndex>=%u) OR fileId > %u)\n",startFileId,startMessageIndex,startFileId);

    p += sprintf(p,"ORDER BY\n");
    p += sprintf(p,"  fileId,messageIndex\n");
    p += sprintf(p,"LIMIT\n");
    p += sprintf(p,"  %u\n",maxRecords);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::ContentInfo *contentInfo = new T::ContentInfo();

      contentInfo->mFileId = atoi(PQgetvalue(res, i, 0));
      contentInfo->mMessageIndex = atoi(PQgetvalue(res, i, 1));
      contentInfo->mFileType = atoi(PQgetvalue(res, i, 2));
      contentInfo->mFilePosition = atoll(PQgetvalue(res, i, 3));
      contentInfo->mMessageSize = atoi(PQgetvalue(res, i, 4));
      contentInfo->mProducerId = atoi(PQgetvalue(res, i, 5));
      contentInfo->mGenerationId = atoi(PQgetvalue(res, i, 6));
      contentInfo->mGeometryId = atoi(PQgetvalue(res, i, 7));
      contentInfo->mFmiParameterId = atoi(PQgetvalue(res, i, 8));
      contentInfo->setFmiParameterName(PQgetvalue(res, i, 9));
      contentInfo->setForecastTime(PQgetvalue(res, i, 10));
      contentInfo->mFmiParameterLevelId = atoi(PQgetvalue(res, i, 11));
      contentInfo->mParameterLevel = atoi(PQgetvalue(res, i, 12));
      contentInfo->mForecastType = atoi(PQgetvalue(res, i, 13));
      contentInfo->mForecastNumber = atoi(PQgetvalue(res, i, 14));
      contentInfo->mFlags = atoi(PQgetvalue(res, i, 15));
      contentInfo->mSourceId = atoi(PQgetvalue(res, i, 16));
      contentInfo->mAggregationId = atoi(PQgetvalue(res, i, 17));
      contentInfo->mAggregationPeriod = atoi(PQgetvalue(res, i, 18));
      contentInfo->mProcessingTypeId = atoi(PQgetvalue(res, i, 19));
      contentInfo->mProcessingTypeValue1 = atof(PQgetvalue(res, i, 20));
      contentInfo->mProcessingTypeValue2 = atof(PQgetvalue(res, i, 21));

      std::string modificationTime = PQgetvalue(res, i, 22);
      contentInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 23);
      contentInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      contentInfoList.addContentInfo(contentInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getContentByParameterId(T::ParamKeyType parameterKeyType,const std::string& parameterKey,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::FmiParamId paramId = 0;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        paramId = atoi(parameterKey.c_str());
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        paramId = Identification::gridDef.getFmiParameterIdByFmiName(parameterKey);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        paramId = Identification::gridDef.getFmiParameterIdByGribId(atoi(parameterKey.c_str()));
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        paramId = Identification::gridDef.getFmiParameterIdByNewbaseId(atoi(parameterKey.c_str()));
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        paramId = Identification::gridDef.getFmiParameterIdByNewbaseName(parameterKey);
        break;

      default:
        paramId = atoi(parameterKey.c_str());
        break;
    }


    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  messageIndex,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  filePosition,\n");
    p += sprintf(p,"  messageSize,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  parameterId,\n");
    p += sprintf(p,"  parameterName,\n");
    p += sprintf(p,"  to_char(forecastTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  level,\n");
    p += sprintf(p,"  forecastType,\n");
    p += sprintf(p,"  foracastNumber,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  aggregationId,\n");
    p += sprintf(p,"  aggregationPeriod,\n");
    p += sprintf(p,"  processingTypeId,\n");
    p += sprintf(p,"  processingTypeValue1,\n");
    p += sprintf(p,"  processingTypeValue2,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  content\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  parameterId=%d;\n",paramId);


    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::ContentInfo *contentInfo = new T::ContentInfo();

      contentInfo->mFileId = atoi(PQgetvalue(res, i, 0));
      contentInfo->mMessageIndex = atoi(PQgetvalue(res, i, 1));
      contentInfo->mFileType = atoi(PQgetvalue(res, i, 2));
      contentInfo->mFilePosition = atoll(PQgetvalue(res, i, 3));
      contentInfo->mMessageSize = atoi(PQgetvalue(res, i, 4));
      contentInfo->mProducerId = atoi(PQgetvalue(res, i, 5));
      contentInfo->mGenerationId = atoi(PQgetvalue(res, i, 6));
      contentInfo->mGeometryId = atoi(PQgetvalue(res, i, 7));
      contentInfo->mFmiParameterId = atoi(PQgetvalue(res, i, 8));
      contentInfo->setFmiParameterName(PQgetvalue(res, i, 9));
      contentInfo->setForecastTime(PQgetvalue(res, i, 10));
      contentInfo->mFmiParameterLevelId = atoi(PQgetvalue(res, i, 11));
      contentInfo->mParameterLevel = atoi(PQgetvalue(res, i, 12));
      contentInfo->mForecastType = atoi(PQgetvalue(res, i, 13));
      contentInfo->mForecastNumber = atoi(PQgetvalue(res, i, 14));
      contentInfo->mFlags = atoi(PQgetvalue(res, i, 15));
      contentInfo->mSourceId = atoi(PQgetvalue(res, i, 16));
      contentInfo->mAggregationId = atoi(PQgetvalue(res, i, 17));
      contentInfo->mAggregationPeriod = atoi(PQgetvalue(res, i, 18));
      contentInfo->mProcessingTypeId = atoi(PQgetvalue(res, i, 19));
      contentInfo->mProcessingTypeValue1 = atof(PQgetvalue(res, i, 20));
      contentInfo->mProcessingTypeValue2 = atof(PQgetvalue(res, i, 21));

      std::string modificationTime = PQgetvalue(res, i, 22);
      contentInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 23);
      contentInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      contentInfoList.addContentInfo(contentInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getContentByParameterIdAndTimeRange(T::ParamKeyType parameterKeyType,const std::string& parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::FmiParamId paramId = 0;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        paramId = atoi(parameterKey.c_str());
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        paramId = Identification::gridDef.getFmiParameterIdByFmiName(parameterKey);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        paramId = Identification::gridDef.getFmiParameterIdByGribId(atoi(parameterKey.c_str()));
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        paramId = Identification::gridDef.getFmiParameterIdByNewbaseId(atoi(parameterKey.c_str()));
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        paramId = Identification::gridDef.getFmiParameterIdByNewbaseName(parameterKey);
        break;

      default:
        paramId = atoi(parameterKey.c_str());
        break;
    }

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  messageIndex,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  filePosition,\n");
    p += sprintf(p,"  messageSize,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  parameterId,\n");
    p += sprintf(p,"  parameterName,\n");
    p += sprintf(p,"  to_char(forecastTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  level,\n");
    p += sprintf(p,"  forecastType,\n");
    p += sprintf(p,"  foracastNumber,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  aggregationId,\n");
    p += sprintf(p,"  aggregationPeriod,\n");
    p += sprintf(p,"  processingTypeId,\n");
    p += sprintf(p,"  processingTypeValue1,\n");
    p += sprintf(p,"  processingTypeValue2,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  content\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  forecastTime >= '%s' AND forecastTime <= '%s'\n",startTime.c_str(),endTime.c_str());
    p += sprintf(p,"AND\n");
    p += sprintf(p,"  parameterId=%d\n",paramId);
    p += sprintf(p,"AND  parameterLevel >= %d AND parameterLevel <= %d\n",minLevel,maxLevel);

    if (geometryId > 0)
      p += sprintf(p,"AND geometryId = %d\n",geometryId);

    if (forecastType > 0)
    {
      p += sprintf(p,"AND forecastType = %d\n",forecastType);

      if (forecastNumber > 0)
        p += sprintf(p,"AND forecastNumber = %d\n",forecastNumber);
    }

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::ContentInfo *contentInfo = new T::ContentInfo();

      contentInfo->mFileId = atoi(PQgetvalue(res, i, 0));
      contentInfo->mMessageIndex = atoi(PQgetvalue(res, i, 1));
      contentInfo->mFileType = atoi(PQgetvalue(res, i, 2));
      contentInfo->mFilePosition = atoll(PQgetvalue(res, i, 3));
      contentInfo->mMessageSize = atoi(PQgetvalue(res, i, 4));
      contentInfo->mProducerId = atoi(PQgetvalue(res, i, 5));
      contentInfo->mGenerationId = atoi(PQgetvalue(res, i, 6));
      contentInfo->mGeometryId = atoi(PQgetvalue(res, i, 7));
      contentInfo->mFmiParameterId = atoi(PQgetvalue(res, i, 8));
      contentInfo->setFmiParameterName(PQgetvalue(res, i, 9));
      contentInfo->setForecastTime(PQgetvalue(res, i, 10));
      contentInfo->mFmiParameterLevelId = atoi(PQgetvalue(res, i, 11));
      contentInfo->mParameterLevel = atoi(PQgetvalue(res, i, 12));
      contentInfo->mForecastType = atoi(PQgetvalue(res, i, 13));
      contentInfo->mForecastNumber = atoi(PQgetvalue(res, i, 14));
      contentInfo->mFlags = atoi(PQgetvalue(res, i, 15));
      contentInfo->mSourceId = atoi(PQgetvalue(res, i, 16));
      contentInfo->mAggregationId = atoi(PQgetvalue(res, i, 17));
      contentInfo->mAggregationPeriod = atoi(PQgetvalue(res, i, 18));
      contentInfo->mProcessingTypeId = atoi(PQgetvalue(res, i, 19));
      contentInfo->mProcessingTypeValue1 = atof(PQgetvalue(res, i, 20));
      contentInfo->mProcessingTypeValue2 = atof(PQgetvalue(res, i, 21));

      std::string modificationTime = PQgetvalue(res, i, 22);
      contentInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 23);
      contentInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      contentInfoList.addContentInfo(contentInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getContentByParameterIdAndGeneration(uint generationId,T::ParamKeyType parameterKeyType,const std::string& parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::FmiParamId paramId = 0;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        paramId = atoi(parameterKey.c_str());
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        paramId = Identification::gridDef.getFmiParameterIdByFmiName(parameterKey);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        paramId = Identification::gridDef.getFmiParameterIdByGribId(atoi(parameterKey.c_str()));
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        paramId = Identification::gridDef.getFmiParameterIdByNewbaseId(atoi(parameterKey.c_str()));
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        paramId = Identification::gridDef.getFmiParameterIdByNewbaseName(parameterKey);
        break;

      default:
        paramId = atoi(parameterKey.c_str());
        break;
    }

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  messageIndex,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  filePosition,\n");
    p += sprintf(p,"  messageSize,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  parameterId,\n");
    p += sprintf(p,"  parameterName,\n");
    p += sprintf(p,"  to_char(forecastTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  level,\n");
    p += sprintf(p,"  forecastType,\n");
    p += sprintf(p,"  foracastNumber,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  aggregationId,\n");
    p += sprintf(p,"  aggregationPeriod,\n");
    p += sprintf(p,"  processingTypeId,\n");
    p += sprintf(p,"  processingTypeValue1,\n");
    p += sprintf(p,"  processingTypeValue2,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  content\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  to_char(forecastTime,'yyyymmddThh24MISS') >= '%s' AND to_char(forecastTime,'yyyymmddThh24MISS') <= '%s'\n",startTime.c_str(),endTime.c_str());
    p += sprintf(p,"AND\n");
    p += sprintf(p,"  generationId=%u AND parameterId=%d\n",generationId,paramId);
    p += sprintf(p,"AND  parameterLevel >= %d AND parameterLevel <= %d\n",minLevel,maxLevel);

    if (geometryId > 0)
      p += sprintf(p,"AND geometryId = %d\n",geometryId);

    if (forecastType > 0)
    {
      p += sprintf(p,"AND forecastType = %d\n",forecastType);

      if (forecastNumber > 0)
        p += sprintf(p,"AND forecastNumber = %d\n",forecastNumber);
    }

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::ContentInfo *contentInfo = new T::ContentInfo();

      contentInfo->mFileId = atoi(PQgetvalue(res, i, 0));
      contentInfo->mMessageIndex = atoi(PQgetvalue(res, i, 1));
      contentInfo->mFileType = atoi(PQgetvalue(res, i, 2));
      contentInfo->mFilePosition = atoll(PQgetvalue(res, i, 3));
      contentInfo->mMessageSize = atoi(PQgetvalue(res, i, 4));
      contentInfo->mProducerId = atoi(PQgetvalue(res, i, 5));
      contentInfo->mGenerationId = atoi(PQgetvalue(res, i, 6));
      contentInfo->mGeometryId = atoi(PQgetvalue(res, i, 7));
      contentInfo->mFmiParameterId = atoi(PQgetvalue(res, i, 8));
      contentInfo->setFmiParameterName(PQgetvalue(res, i, 9));
      contentInfo->setForecastTime(PQgetvalue(res, i, 10));
      contentInfo->mFmiParameterLevelId = atoi(PQgetvalue(res, i, 11));
      contentInfo->mParameterLevel = atoi(PQgetvalue(res, i, 12));
      contentInfo->mForecastType = atoi(PQgetvalue(res, i, 13));
      contentInfo->mForecastNumber = atoi(PQgetvalue(res, i, 14));
      contentInfo->mFlags = atoi(PQgetvalue(res, i, 15));
      contentInfo->mSourceId = atoi(PQgetvalue(res, i, 16));
      contentInfo->mAggregationId = atoi(PQgetvalue(res, i, 17));
      contentInfo->mAggregationPeriod = atoi(PQgetvalue(res, i, 18));
      contentInfo->mProcessingTypeId = atoi(PQgetvalue(res, i, 19));
      contentInfo->mProcessingTypeValue1 = atof(PQgetvalue(res, i, 20));
      contentInfo->mProcessingTypeValue2 = atof(PQgetvalue(res, i, 21));

      std::string modificationTime = PQgetvalue(res, i, 22);
      contentInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 23);
      contentInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      contentInfoList.addContentInfo(contentInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int PostgresqlImplementation::getContentByParameterIdAndProducer(uint producerId,T::ParamKeyType parameterKeyType,const std::string& parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    T::FmiParamId paramId = 0;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        paramId = atoi(parameterKey.c_str());
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        paramId = Identification::gridDef.getFmiParameterIdByFmiName(parameterKey);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        paramId = Identification::gridDef.getFmiParameterIdByGribId(atoi(parameterKey.c_str()));
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        paramId = Identification::gridDef.getFmiParameterIdByNewbaseId(atoi(parameterKey.c_str()));
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        paramId = Identification::gridDef.getFmiParameterIdByNewbaseName(parameterKey);
        break;

      default:
        paramId = atoi(parameterKey.c_str());
        break;
    }

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  messageIndex,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  filePosition,\n");
    p += sprintf(p,"  messageSize,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  parameterId,\n");
    p += sprintf(p,"  parameterName,\n");
    p += sprintf(p,"  to_char(forecastTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  level,\n");
    p += sprintf(p,"  forecastType,\n");
    p += sprintf(p,"  foracastNumber,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  aggregationId,\n");
    p += sprintf(p,"  aggregationPeriod,\n");
    p += sprintf(p,"  processingTypeId,\n");
    p += sprintf(p,"  processingTypeValue1,\n");
    p += sprintf(p,"  processingTypeValue2,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  content\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  to_char(forecastTime,'yyyymmddThh24MISS') >= '%s' AND to_char(forecastTime,'yyyymmddThh24MISS') <= '%s'\n",startTime.c_str(),endTime.c_str());
    p += sprintf(p,"AND\n");
    p += sprintf(p,"  producerId=%u AND parameterId=%d\n",producerId,paramId);
    p += sprintf(p,"AND  parameterLevel >= %d AND parameterLevel <= %d\n",minLevel,maxLevel);

    if (geometryId > 0)
      p += sprintf(p,"AND geometryId = %d\n",geometryId);

    if (forecastType > 0)
    {
      p += sprintf(p,"AND forecastType = %d\n",forecastType);

      if (forecastNumber > 0)
        p += sprintf(p,"AND forecastNumber = %d\n",forecastNumber);
    }

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::ContentInfo *contentInfo = new T::ContentInfo();

      contentInfo->mFileId = atoi(PQgetvalue(res, i, 0));
      contentInfo->mMessageIndex = atoi(PQgetvalue(res, i, 1));
      contentInfo->mFileType = atoi(PQgetvalue(res, i, 2));
      contentInfo->mFilePosition = atoll(PQgetvalue(res, i, 3));
      contentInfo->mMessageSize = atoi(PQgetvalue(res, i, 4));
      contentInfo->mProducerId = atoi(PQgetvalue(res, i, 5));
      contentInfo->mGenerationId = atoi(PQgetvalue(res, i, 6));
      contentInfo->mGeometryId = atoi(PQgetvalue(res, i, 7));
      contentInfo->mFmiParameterId = atoi(PQgetvalue(res, i, 8));
      contentInfo->setFmiParameterName(PQgetvalue(res, i, 9));
      contentInfo->setForecastTime(PQgetvalue(res, i, 10));
      contentInfo->mFmiParameterLevelId = atoi(PQgetvalue(res, i, 11));
      contentInfo->mParameterLevel = atoi(PQgetvalue(res, i, 12));
      contentInfo->mForecastType = atoi(PQgetvalue(res, i, 13));
      contentInfo->mForecastNumber = atoi(PQgetvalue(res, i, 14));
      contentInfo->mFlags = atoi(PQgetvalue(res, i, 15));
      contentInfo->mSourceId = atoi(PQgetvalue(res, i, 16));
      contentInfo->mAggregationId = atoi(PQgetvalue(res, i, 17));
      contentInfo->mAggregationPeriod = atoi(PQgetvalue(res, i, 18));
      contentInfo->mProcessingTypeId = atoi(PQgetvalue(res, i, 19));
      contentInfo->mProcessingTypeValue1 = atof(PQgetvalue(res, i, 20));
      contentInfo->mProcessingTypeValue2 = atof(PQgetvalue(res, i, 21));

      std::string modificationTime = PQgetvalue(res, i, 22);
      contentInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 23);
      contentInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      contentInfoList.addContentInfo(contentInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int PostgresqlImplementation::getContentByGenerationIdAndTimeRange(uint generationId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  messageIndex,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  filePosition,\n");
    p += sprintf(p,"  messageSize,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  parameterId,\n");
    p += sprintf(p,"  parameterName,\n");
    p += sprintf(p,"  to_char(forecastTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  level,\n");
    p += sprintf(p,"  forecastType,\n");
    p += sprintf(p,"  foracastNumber,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  aggregationId,\n");
    p += sprintf(p,"  aggregationPeriod,\n");
    p += sprintf(p,"  processingTypeId,\n");
    p += sprintf(p,"  processingTypeValue1,\n");
    p += sprintf(p,"  processingTypeValue2,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  content\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  generationId=%u AND forecastTime >= TO_TIMESTAMP('%s','yyyymmddThh24MISS') AND forecastTime <= TO_TIMESTAMP('%s','yyyymmddThh24MISS')\n",generationId,startTime.c_str(),endTime.c_str());
    p += sprintf(p,"ORDER BY\n");
    p += sprintf(p,"  fileId,messageIndex\n");

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::ContentInfo *contentInfo = new T::ContentInfo();

      contentInfo->mFileId = atoi(PQgetvalue(res, i, 0));
      contentInfo->mMessageIndex = atoi(PQgetvalue(res, i, 1));
      contentInfo->mFileType = atoi(PQgetvalue(res, i, 2));
      contentInfo->mFilePosition = atoll(PQgetvalue(res, i, 3));
      contentInfo->mMessageSize = atoi(PQgetvalue(res, i, 4));
      contentInfo->mProducerId = atoi(PQgetvalue(res, i, 5));
      contentInfo->mGenerationId = atoi(PQgetvalue(res, i, 6));
      contentInfo->mGeometryId = atoi(PQgetvalue(res, i, 7));
      contentInfo->mFmiParameterId = atoi(PQgetvalue(res, i, 8));
      contentInfo->setFmiParameterName(PQgetvalue(res, i, 9));
      contentInfo->setForecastTime(PQgetvalue(res, i, 10));
      contentInfo->mFmiParameterLevelId = atoi(PQgetvalue(res, i, 11));
      contentInfo->mParameterLevel = atoi(PQgetvalue(res, i, 12));
      contentInfo->mForecastType = atoi(PQgetvalue(res, i, 13));
      contentInfo->mForecastNumber = atoi(PQgetvalue(res, i, 14));
      contentInfo->mFlags = atoi(PQgetvalue(res, i, 15));
      contentInfo->mSourceId = atoi(PQgetvalue(res, i, 16));
      contentInfo->mAggregationId = atoi(PQgetvalue(res, i, 17));
      contentInfo->mAggregationPeriod = atoi(PQgetvalue(res, i, 18));
      contentInfo->mProcessingTypeId = atoi(PQgetvalue(res, i, 19));
      contentInfo->mProcessingTypeValue1 = atof(PQgetvalue(res, i, 20));
      contentInfo->mProcessingTypeValue2 = atof(PQgetvalue(res, i, 21));

      std::string modificationTime = PQgetvalue(res, i, 22);
      contentInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 23);
      contentInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      contentInfoList.addContentInfo(contentInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getContentByForecastTimeList(std::vector<T::ForecastTime>& forecastTimeList,T::ContentInfoList& contentInfoList)
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

    char sql[100000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  messageIndex,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  filePosition,\n");
    p += sprintf(p,"  messageSize,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  parameterId,\n");
    p += sprintf(p,"  parameterName,\n");
    p += sprintf(p,"  to_char(forecastTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  level,\n");
    p += sprintf(p,"  forecastType,\n");
    p += sprintf(p,"  foracastNumber,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  aggregationId,\n");
    p += sprintf(p,"  aggregationPeriod,\n");
    p += sprintf(p,"  processingTypeId,\n");
    p += sprintf(p,"  processingTypeValue1,\n");
    p += sprintf(p,"  processingTypeValue2,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  content\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  generationId IN (");

    char *pp = p;
    for (auto it = forecastTimeList.begin(); it != forecastTimeList.end(); ++it)
    {
      if (p != pp)
        p += sprintf(p,",");

      p += sprintf(p,"%u",it->mGenerationId);
    }

    p += sprintf(p,")\n");
    p += sprintf(p,"ORDER BY\n");
    p += sprintf(p,"  fileId,messageIndex\n");

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      sprintf(tmp,"%s;%s;%s;%s;%s",PQgetvalue(res, i, 6),PQgetvalue(res, i, 7),PQgetvalue(res, i, 13),PQgetvalue(res, i, 14),PQgetvalue(res, i, 10));
      if (searchList.find(tmp) != searchList.end())
      {
        T::ContentInfo *contentInfo = new T::ContentInfo();

        contentInfo->mFileId = atoi(PQgetvalue(res, i, 0));
        contentInfo->mMessageIndex = atoi(PQgetvalue(res, i, 1));
        contentInfo->mFileType = atoi(PQgetvalue(res, i, 2));
        contentInfo->mFilePosition = atoll(PQgetvalue(res, i, 3));
        contentInfo->mMessageSize = atoi(PQgetvalue(res, i, 4));
        contentInfo->mProducerId = atoi(PQgetvalue(res, i, 5));
        contentInfo->mGenerationId = atoi(PQgetvalue(res, i, 6));
        contentInfo->mGeometryId = atoi(PQgetvalue(res, i, 7));
        contentInfo->mFmiParameterId = atoi(PQgetvalue(res, i, 8));
        contentInfo->setFmiParameterName(PQgetvalue(res, i, 9));
        contentInfo->setForecastTime(PQgetvalue(res, i, 10));
        contentInfo->mFmiParameterLevelId = atoi(PQgetvalue(res, i, 11));
        contentInfo->mParameterLevel = atoi(PQgetvalue(res, i, 12));
        contentInfo->mForecastType = atoi(PQgetvalue(res, i, 13));
        contentInfo->mForecastNumber = atoi(PQgetvalue(res, i, 14));
        contentInfo->mFlags = atoi(PQgetvalue(res, i, 15));
        contentInfo->mSourceId = atoi(PQgetvalue(res, i, 16));
        contentInfo->mAggregationId = atoi(PQgetvalue(res, i, 17));
        contentInfo->mAggregationPeriod = atoi(PQgetvalue(res, i, 18));
        contentInfo->mProcessingTypeId = atoi(PQgetvalue(res, i, 19));
        contentInfo->mProcessingTypeValue1 = atof(PQgetvalue(res, i, 20));
        contentInfo->mProcessingTypeValue2 = atof(PQgetvalue(res, i, 21));

        std::string modificationTime = PQgetvalue(res, i, 22);
        contentInfo->mModificationTime = utcTimeToTimeT(modificationTime);

        std::string deletionTime = PQgetvalue(res, i, 23);
        contentInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

        contentInfoList.addContentInfo(contentInfo);
      }
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getContentByProducerId(uint producerId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  messageIndex,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  filePosition,\n");
    p += sprintf(p,"  messageSize,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  parameterId,\n");
    p += sprintf(p,"  parameterName,\n");
    p += sprintf(p,"  to_char(forecastTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  level,\n");
    p += sprintf(p,"  forecastType,\n");
    p += sprintf(p,"  foracastNumber,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  aggregationId,\n");
    p += sprintf(p,"  aggregationPeriod,\n");
    p += sprintf(p,"  processingTypeId,\n");
    p += sprintf(p,"  processingTypeValue1,\n");
    p += sprintf(p,"  processingTypeValue2,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  content\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  producerId=%u AND ((fileId=%u AND messageIndex>=%u) OR fileId > %u)\n",producerId,startFileId,startMessageIndex,startFileId);
    p += sprintf(p,"ORDER BY\n");
    p += sprintf(p,"  fileId,messageIndex\n");
    p += sprintf(p,"LIMIT\n");
    p += sprintf(p,"  %u\n",maxRecords);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::ContentInfo *contentInfo = new T::ContentInfo();

      contentInfo->mFileId = atoi(PQgetvalue(res, i, 0));
      contentInfo->mMessageIndex = atoi(PQgetvalue(res, i, 1));
      contentInfo->mFileType = atoi(PQgetvalue(res, i, 2));
      contentInfo->mFilePosition = atoll(PQgetvalue(res, i, 3));
      contentInfo->mMessageSize = atoi(PQgetvalue(res, i, 4));
      contentInfo->mProducerId = atoi(PQgetvalue(res, i, 5));
      contentInfo->mGenerationId = atoi(PQgetvalue(res, i, 6));
      contentInfo->mGeometryId = atoi(PQgetvalue(res, i, 7));
      contentInfo->mFmiParameterId = atoi(PQgetvalue(res, i, 8));
      contentInfo->setFmiParameterName(PQgetvalue(res, i, 9));
      contentInfo->setForecastTime(PQgetvalue(res, i, 10));
      contentInfo->mFmiParameterLevelId = atoi(PQgetvalue(res, i, 11));
      contentInfo->mParameterLevel = atoi(PQgetvalue(res, i, 12));
      contentInfo->mForecastType = atoi(PQgetvalue(res, i, 13));
      contentInfo->mForecastNumber = atoi(PQgetvalue(res, i, 14));
      contentInfo->mFlags = atoi(PQgetvalue(res, i, 15));
      contentInfo->mSourceId = atoi(PQgetvalue(res, i, 16));
      contentInfo->mAggregationId = atoi(PQgetvalue(res, i, 17));
      contentInfo->mAggregationPeriod = atoi(PQgetvalue(res, i, 18));
      contentInfo->mProcessingTypeId = atoi(PQgetvalue(res, i, 19));
      contentInfo->mProcessingTypeValue1 = atof(PQgetvalue(res, i, 20));
      contentInfo->mProcessingTypeValue2 = atof(PQgetvalue(res, i, 21));

      std::string modificationTime = PQgetvalue(res, i, 22);
      contentInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 23);
      contentInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      contentInfoList.addContentInfo(contentInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getContentBySourceId(uint sourceId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  messageIndex,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  filePosition,\n");
    p += sprintf(p,"  messageSize,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  parameterId,\n");
    p += sprintf(p,"  parameterName,\n");
    p += sprintf(p,"  to_char(forecastTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  level,\n");
    p += sprintf(p,"  forecastType,\n");
    p += sprintf(p,"  foracastNumber,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  aggregationId,\n");
    p += sprintf(p,"  aggregationPeriod,\n");
    p += sprintf(p,"  processingTypeId,\n");
    p += sprintf(p,"  processingTypeValue1,\n");
    p += sprintf(p,"  processingTypeValue2,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  content\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  sourceId=%u AND ((fileId=%u AND messageIndex>=%u) OR fileId > %u)\n",sourceId,startFileId,startMessageIndex,startFileId);
    p += sprintf(p,"ORDER BY\n");
    p += sprintf(p,"  fileId,messageIndex\n");
    p += sprintf(p,"LIMIT\n");
    p += sprintf(p,"  %u\n",maxRecords);

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::ContentInfo *contentInfo = new T::ContentInfo();

      contentInfo->mFileId = atoi(PQgetvalue(res, i, 0));
      contentInfo->mMessageIndex = atoi(PQgetvalue(res, i, 1));
      contentInfo->mFileType = atoi(PQgetvalue(res, i, 2));
      contentInfo->mFilePosition = atoll(PQgetvalue(res, i, 3));
      contentInfo->mMessageSize = atoi(PQgetvalue(res, i, 4));
      contentInfo->mProducerId = atoi(PQgetvalue(res, i, 5));
      contentInfo->mGenerationId = atoi(PQgetvalue(res, i, 6));
      contentInfo->mGeometryId = atoi(PQgetvalue(res, i, 7));
      contentInfo->mFmiParameterId = atoi(PQgetvalue(res, i, 8));
      contentInfo->setFmiParameterName(PQgetvalue(res, i, 9));
      contentInfo->setForecastTime(PQgetvalue(res, i, 10));
      contentInfo->mFmiParameterLevelId = atoi(PQgetvalue(res, i, 11));
      contentInfo->mParameterLevel = atoi(PQgetvalue(res, i, 12));
      contentInfo->mForecastType = atoi(PQgetvalue(res, i, 13));
      contentInfo->mForecastNumber = atoi(PQgetvalue(res, i, 14));
      contentInfo->mFlags = atoi(PQgetvalue(res, i, 15));
      contentInfo->mSourceId = atoi(PQgetvalue(res, i, 16));
      contentInfo->mAggregationId = atoi(PQgetvalue(res, i, 17));
      contentInfo->mAggregationPeriod = atoi(PQgetvalue(res, i, 18));
      contentInfo->mProcessingTypeId = atoi(PQgetvalue(res, i, 19));
      contentInfo->mProcessingTypeValue1 = atof(PQgetvalue(res, i, 20));
      contentInfo->mProcessingTypeValue2 = atof(PQgetvalue(res, i, 21));

      std::string modificationTime = PQgetvalue(res, i, 22);
      contentInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 23);
      contentInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      contentInfoList.addContentInfo(contentInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::getContentByFileId(uint fileId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    p += sprintf(p,"SELECT\n");
    p += sprintf(p,"  fileId,\n");
    p += sprintf(p,"  messageIndex,\n");
    p += sprintf(p,"  fileType,\n");
    p += sprintf(p,"  filePosition,\n");
    p += sprintf(p,"  messageSize,\n");
    p += sprintf(p,"  producerId,\n");
    p += sprintf(p,"  generationId,\n");
    p += sprintf(p,"  geometryId,\n");
    p += sprintf(p,"  parameterId,\n");
    p += sprintf(p,"  parameterName,\n");
    p += sprintf(p,"  to_char(forecastTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  levelId,\n");
    p += sprintf(p,"  level,\n");
    p += sprintf(p,"  forecastType,\n");
    p += sprintf(p,"  foracastNumber,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  sourceId,\n");
    p += sprintf(p,"  aggregationId,\n");
    p += sprintf(p,"  aggregationPeriod,\n");
    p += sprintf(p,"  processingTypeId,\n");
    p += sprintf(p,"  processingTypeValue1,\n");
    p += sprintf(p,"  processingTypeValue2,\n");
    p += sprintf(p,"  to_char(modificationTime,'yyyymmddThh24MISS'),\n");
    p += sprintf(p,"  to_char(deletionTime,'yyyymmddThh24MISS')\n");
    p += sprintf(p,"FROM\n");
    p += sprintf(p,"  content\n");
    p += sprintf(p,"WHERE\n");
    p += sprintf(p,"  fileId=%u\n",fileId);
    p += sprintf(p,"ORDER BY\n");
    p += sprintf(p,"  fileId,messageIndex\n");

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQresultErrorMessage(res));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }

    int rowCount = PQntuples(res);

    for (int i = 0; i < rowCount; i++)
    {
      T::ContentInfo *contentInfo = new T::ContentInfo();

      contentInfo->mFileId = atoi(PQgetvalue(res, i, 0));
      contentInfo->mMessageIndex = atoi(PQgetvalue(res, i, 1));
      contentInfo->mFileType = atoi(PQgetvalue(res, i, 2));
      contentInfo->mFilePosition = atoll(PQgetvalue(res, i, 3));
      contentInfo->mMessageSize = atoi(PQgetvalue(res, i, 4));
      contentInfo->mProducerId = atoi(PQgetvalue(res, i, 5));
      contentInfo->mGenerationId = atoi(PQgetvalue(res, i, 6));
      contentInfo->mGeometryId = atoi(PQgetvalue(res, i, 7));
      contentInfo->mFmiParameterId = atoi(PQgetvalue(res, i, 8));
      contentInfo->setFmiParameterName(PQgetvalue(res, i, 9));
      contentInfo->setForecastTime(PQgetvalue(res, i, 10));
      contentInfo->mFmiParameterLevelId = atoi(PQgetvalue(res, i, 11));
      contentInfo->mParameterLevel = atoi(PQgetvalue(res, i, 12));
      contentInfo->mForecastType = atoi(PQgetvalue(res, i, 13));
      contentInfo->mForecastNumber = atoi(PQgetvalue(res, i, 14));
      contentInfo->mFlags = atoi(PQgetvalue(res, i, 15));
      contentInfo->mSourceId = atoi(PQgetvalue(res, i, 16));
      contentInfo->mAggregationId = atoi(PQgetvalue(res, i, 17));
      contentInfo->mAggregationPeriod = atoi(PQgetvalue(res, i, 18));
      contentInfo->mProcessingTypeId = atoi(PQgetvalue(res, i, 19));
      contentInfo->mProcessingTypeValue1 = atof(PQgetvalue(res, i, 20));
      contentInfo->mProcessingTypeValue2 = atof(PQgetvalue(res, i, 21));

      std::string modificationTime = PQgetvalue(res, i, 22);
      contentInfo->mModificationTime = utcTimeToTimeT(modificationTime);

      std::string deletionTime = PQgetvalue(res, i, 23);
      contentInfo->mDeletionTime = utcTimeToTimeT(deletionTime);

      contentInfoList.addContentInfo(contentInfo);
    }

    PQclear(res);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





T::EventId PostgresqlImplementation::addEvent(uint eventType,uint id1,uint id2,uint id3,unsigned long long flags,const char *eventData)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    char sql[10000];
    char *p = sql;

    time_t ct = time(nullptr);
    std::string currentTime = utcTimeFromTimeT(ct);

    p += sprintf(p,"INSERT INTO event\n");
    p += sprintf(p,"(\n");
    p += sprintf(p,"  eventId,\n");
    p += sprintf(p,"  eventType,\n");
    p += sprintf(p,"  eventTime,\n");
    p += sprintf(p,"  id1,\n");
    p += sprintf(p,"  id2,\n");
    p += sprintf(p,"  id3,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  eventData\n");
    p += sprintf(p,")\n");
    p += sprintf(p,"VALUES \n");
    p += sprintf(p,"(\n");
    p += sprintf(p,"  DEFAULT,\n");
    p += sprintf(p,"  %u,\n",eventType);
    p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",currentTime.c_str());
    p += sprintf(p,"  %u,\n",id1);
    p += sprintf(p,"  %u,\n",id2);
    p += sprintf(p,"  %u,\n",id3);
    p += sprintf(p,"  %llu,\n",flags);
    if (eventData[0] == '\0')
      p += sprintf(p,"  ''\n");
    else
      p += sprintf(p,"  '%s\n'\n",eventData);
    p += sprintf(p,");\n");

    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);

    if ((ct - mEventTruncateCheckTime) > 600)
    {
      truncateEvents();
      mEventTruncateCheckTime = ct;
      sleep(1);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int PostgresqlImplementation::addEventInfoList(T::EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    uint len = eventInfoList.getLength();
    if (len == 0)
      return 0;

    uint c = 0;
    time_t ct = time(nullptr);

    char sql[1000000];
    char *p = sql;

    p += sprintf(p,"INSERT INTO event\n");
    p += sprintf(p,"(\n");
    p += sprintf(p,"  eventId,\n");
    p += sprintf(p,"  eventType,\n");
    p += sprintf(p,"  eventTime,\n");
    p += sprintf(p,"  id1,\n");
    p += sprintf(p,"  id2,\n");
    p += sprintf(p,"  id3,\n");
    p += sprintf(p,"  flags,\n");
    p += sprintf(p,"  eventData\n");
    p += sprintf(p,")\n");
    p += sprintf(p,"VALUES \n");

    char *pp = p;

    T::EventInfo *event = eventInfoList.getFirstEvent();
    while (event != nullptr)
    {
      std::string eventTime = utcTimeFromTimeT(event->mEventTime);

      p += sprintf(p,"(\n");
      p += sprintf(p,"  DEFAULT,\n");
      p += sprintf(p,"  %u,\n",event->mType);
      p += sprintf(p,"  TO_TIMESTAMP('%s','yyyymmddThh24MISS'),\n",eventTime.c_str());
      p += sprintf(p,"  %u,\n",event->mId1);
      p += sprintf(p,"  %u,\n",event->mId2);
      p += sprintf(p,"  %u,\n",event->mId3);
      p += sprintf(p,"  %llu,\n",event->mFlags);
      if (event->mEventData.empty())
        p += sprintf(p,"  ''\n");
      else
        p += sprintf(p,"  '%s\n'\n",event->mEventData.c_str());
      p += sprintf(p,")\n");

      if (p-sql > 990000 || (c+1) == len)
      {
        PGresult *res = PQexec(mConnection, sql);
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
          if (PQresultStatus(res) == PGRES_FATAL_ERROR)
            closeConnection();

          Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
          //exception.addParameter("sql",sql);
          //exception.printError();
          PQclear(res);
          return Result::CONTENT_ADDITION_FAILED;
        }
        PQclear(res);
        p = pp;
      }
      else
      {
        p += sprintf(p,",\n");
      }
      c++;
      event = event->nextItem;
    }

    if ((ct - mEventTruncateCheckTime) > 600)
    {
      truncateEvents();
      mEventTruncateCheckTime = ct;
      sleep(1);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void PostgresqlImplementation::truncateEvents()
{
  FUNCTION_TRACE
  try
  {
    if (!isConnectionValid())
      return;

    std::string deletionTime = utcTimeFromTimeT(time(nullptr)-1200);

    char sql[10000];
    sprintf(sql,"DELETE FROM event WHERE eventTime < TO_TIMESTAMP('%s','yyyymmddThh24MISS')\n",deletionTime.c_str());
    PGresult *res = PQexec(mConnection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      if (PQresultStatus(res) == PGRES_FATAL_ERROR)
        closeConnection();

      Fmi::Exception exception(BCP,PQerrorMessage(mConnection));
      exception.addParameter("sql",sql);
      PQclear(res);
      throw exception;
    }
    PQclear(res);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





}
}
