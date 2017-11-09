#include "ServiceImplementation.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"
#include "grid-files/identification/GribDef.h"
#include "dataServer/corba/client/ClientImplementation.h"


extern "C"
{
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
}


lua_State *mLuaState = NULL;



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
    mFunctionParamId = 0;
    for (uint t=0; t<64; t++)
      mDataServerClient[t] = NULL;

    mParameterAliasFileModificationTime = 0;
    mParameterFileModificationTime = 0;
    mProducerFileModificationTime = 0;
    mLuaFunctionFileModificationTime = 0;
    mLuaState = NULL;
    mLastConfiguratonCheck = 0;

    mServerCounter = 0;
    mServerCheckTime = 0;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ServiceImplementation::~ServiceImplementation()
{
  try
  {
    if (mLuaState != NULL)
      lua_close(mLuaState);

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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ServiceImplementation::init(
    ContentServer::ServiceInterface *contentServer,
    DataServer::ServiceInterface *dataServer,
    std::string gridConfigDirectory,
    std::string parameterFile,
    std::string parameterAliasFile,
    std::string producerFile,
    std::string luaFunctionFile)
{
  try
  {
    mContentServer = contentServer;
    mDataServer = dataServer;
    mGridConfigDirectory = gridConfigDirectory;
    mParameterFile = parameterFile;
    mParameterAliasFile = parameterAliasFile;
    mProducerFile = producerFile;
    mLuaFunctionFile = luaFunctionFile;

    SmartMet::Identification::gribDef.init(mGridConfigDirectory.c_str());

    loadProducerFile();
    loadParameterFile();
    loadParameterAliasFile();
    loadLuaFunctionFile();

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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int ServiceImplementation::_executeQuery(T::SessionId sessionId,Query& query)
{
  try
  {
    // Checking if the configuration files have changed. If so, the
    // changes will be loaded automatically.

    checkConfigurationUpdates();

    //query.print(std::cout,0,0);

    int result = 0;
    if ((query.mFlags & QF_TIME_RANGE_QUERY) != 0)
      result = executeTimeRangeQuery(query);
    else
      result = executeTimeStepQuery(query);

    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::loadParameterFile()
{
  try
  {

    FILE *file = fopen(mParameterFile.c_str(),"r");
    if (file == NULL)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot open the parameter file!");
      exception.addParameter("Filename",mParameterFile);
      throw exception;
    }

    mParameterList.clear();

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st,1000,file) != NULL  &&  st[0] != '#')
      {
        bool ind = false;
        char *field[100];
        uint c = 1;
        field[0] = st;
        char *p = st;
        while (*p != '\0'  &&  c < 100)
        {
          if (*p == '"')
            ind = !ind;

          if ((*p == ';'  || *p == '\n') && !ind)
          {
            *p = '\0';
            p++;
            field[c] = p;
            c++;
          }
          else
          {
            p++;
          }
        }

        if (c > 6)
        {
          ParameterInfo rec;

          if (field[0][0] != '\0')
            rec.mParameterName = field[0];

          if (field[1][0] != '\0')
            rec.mParameterKeyType = (T::ParamKeyType)atoi(field[1]);

          if (field[2][0] != '\0')
            rec.mParameterKey = field[2];

          if (field[3][0] != '\0')
            rec.mParameterLevelIdType = (T::ParamLevelIdType)atoi(field[3]);

          if (field[4][0] != '\0')
            rec.mParameterLevelId = (T::ParamLevelId)atoi(field[4]);

          if (field[5][0] != '\0')
            rec.mParameterLevel = (T::ParamLevel)atoi(field[5]);

          if (field[6][0] != '\0')
            rec.mInterpolationMethod = (T::InterpolationMethod)atoi(field[6]);

          //rec.print(std::cout,0,0);
          mParameterList.push_back(rec);
        }
      }
    }
    fclose(file);

    mParameterFileModificationTime = getFileModificationTime(mParameterFile.c_str());
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ServiceImplementation::loadParameterAliasFile()
{
  try
  {

    FILE *file = fopen(mParameterAliasFile.c_str(),"r");
    if (file == NULL)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot open the parameter alias file!");
      exception.addParameter("Filename",mParameterAliasFile);
      throw exception;
    }

    mParameterAliasList.clear();

    char st[1000];
    uint lineCount = 0;

    while (!feof(file))
    {
      if (fgets(st,1000,file) != NULL)
      {
        lineCount++;
        if (st[0] != '#')
        {
          int ind = 0;
          char *field[100];
          uint c = 1;
          field[0] = st;
          char *p = st;
          while (*p != '\0'  &&  c < 100)
          {
            if (*p == '{')
              ind++;

            if (*p == '}')
              ind--;

            if ((*p == ';'  || *p == '\n')  &&  ind == 0)
            {
              *p = '\0';
              p++;
              field[c] = p;
              c++;
            }
            else
            {
              p++;
            }
          }

          if (c > 2)
          {
            ParameterAlias rec;

            if (field[0][0] != '\0')
              rec.mName = field[0];

            if (field[1][0] != '\0')
              rec.mTitle = field[1];

            if (field[2][0] != '\0')
              rec.mParameterString = field[2];

            auto alias = mParameterAliasList.find(toLowerString(rec.mName));
            if (alias != mParameterAliasList.end())
            {
              printf("### ALIAS '%s' ALREADY DEFINED (%s:%u)!\n",rec.mName.c_str(),mParameterAliasFile.c_str(),lineCount);
              //rec.print(std::cout,0,0);
            }
            else
            {
              //rec.print(std::cout,0,0);
              mParameterAliasList.insert(std::pair<std::string,ParameterAlias>(toLowerString(rec.mName),rec));
            }
          }
        }
      }
    }
    fclose(file);

    mParameterAliasFileModificationTime = getFileModificationTime(mParameterAliasFile.c_str());
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ServiceImplementation::loadProducerFile()
{
  try
  {
    FILE *file = fopen(mProducerFile.c_str(),"r");
    if (file == NULL)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot open the producer file!");
      exception.addParameter("Filename",mProducerFile);
      throw exception;
    }

    mProducerList.clear();

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st,1000,file) != NULL  &&  st[0] != '#')
      {
        bool ind = false;
        char *field[100];
        uint c = 1;
        field[0] = st;
        char *p = st;
        while (*p != '\0'  &&  c < 100)
        {
          if (*p == '"')
            ind = !ind;

          if ((*p == ';'  || *p == '\n') && !ind)
          {
            *p = '\0';
            p++;
            field[c] = p;
            c++;
          }
          else
          {
            p++;
          }
        }

        if (c >= 2 && field[0][0] != '\0' &&  field[1][0] != '\0')
        {
          mProducerList.push_back(std::pair<std::string,T::GeometryId>(std::string(field[0]),atoi(field[1])));
        }
      }
    }
    fclose(file);

    mProducerFileModificationTime = getFileModificationTime(mProducerFile.c_str());
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ServiceImplementation::loadLuaFunctionFile()
{
  try
  {
    if (mLuaState != NULL)
    {
      lua_close(mLuaState);
      mLuaState = NULL;
    }

    mLuaState = luaL_newstate();
    luaL_openlibs(mLuaState);

    if (luaL_dofile(mLuaState,mLuaFunctionFile.c_str()) != 0)
    {
      SmartMet::Spine::Exception exception(BCP, "Cannot load a LUA file!");
      exception.addParameter("Filename",mLuaFunctionFile);
      exception.addParameter("Lua message",lua_tostring(mLuaState, -1));
      lua_pop(mLuaState, 1);
      throw exception;
    }

    mLuaFunctionFileModificationTime = getFileModificationTime(mLuaFunctionFile.c_str());
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::getParameterInfoList(std::string parameterName,T::ParamLevelIdType paramLevelIdType,T::ParamLevelId paramLevelId,T::ParamLevel paramLevel,ParameterInfo_vec& infoList)
{
  try
  {
    for (auto it=mParameterList.begin(); it != mParameterList.end(); ++it)
    {
      if (strcasecmp(it->mParameterName.c_str(),parameterName.c_str()) == 0)
      {
        ParameterInfo info = *it;
        if (!it->mParameterLevelIdType)
          info.mParameterLevelIdType = paramLevelIdType;

        if (!it->mParameterLevelId)
          info.mParameterLevelId = paramLevelId;

        if (!it->mParameterLevel)
          info.mParameterLevel = paramLevel;

        infoList.push_back(info);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ServiceImplementation::getGeometryIdListByCoordinates(QueryCoordinates& coordinates,std::set<T::GeometryId>& geometryIdList)
{
  try
  {
    std::map<T::GeometryId,uint> countList;
    uint maxCount = 0;

    for (auto cList = coordinates.begin(); cList != coordinates.end(); ++cList)
    {
      for (auto coordinate = cList->begin(); coordinate != cList->end(); ++coordinate)
      {
        std::set<T::GeometryId> idList;
        Identification::gribDef.getGeometryIdListByLatLon(coordinate->y(),coordinate->x(),idList);

        for (auto g = idList.begin(); g != idList.end(); ++g)
        {
          auto c = countList.find(*g);
          if (c != countList.end())
          {
            c->second++;
            if (c->second > maxCount)
              maxCount = c->second;
          }
          else
          {
            if (maxCount == 0)
              maxCount = 1;
            countList.insert(std::pair<T::GeometryId,uint>(*g,1));
          }
        }
      }
    }

    for (auto it = countList.begin(); it != countList.end(); ++it)
    {
      if (it->second == maxCount)
      {
        geometryIdList.insert(it->first);
        //printf("Supported geometry %u\n",it->first);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void ServiceImplementation::getGridValues(
                        Producer_vec& producers,
                        ParameterInfo_vec& parameterInfoList,
                        T::ForecastType forecastType,
                        T::ForecastNumber forecastNumber,
                        std::string forecastTime,
                        bool timeMatchRequired,
                        QueryCoordinates& coordinates,
                        bool areaSearch,
                        ParameterValues& valueList)
{
  try
  {
    if (coordinates.size() == 0)
    {
      SmartMet::Spine::Exception exception(BCP, "No coordinates defined!");
      throw exception;
    }

    if (areaSearch  &&  coordinates.size() < 3)
    {
      SmartMet::Spine::Exception exception(BCP, "Area definition requires at least 3 coordinates!");
      throw exception;
    }

    // Getting geometries that support support the given coordinates

    std::set<T::GeometryId> geometryIdList;
    getGeometryIdListByCoordinates(coordinates,geometryIdList);


    // No producers defined. We should go through the producer list that is defined
    // in the configuration file.

    for (auto it = producers.begin(); it != producers.end(); ++it)
    {
      std::string producerName = it->first;

      T::GeometryId producerGeometryId = it->second;

      // printf("Producer %s:%d\n",producerName.c_str(),producerGeometryId);

      if (geometryIdList.find(producerGeometryId) != geometryIdList.end())
      {
        // The current producer supports a geometry where the current coordinates can be found.
        T::ProducerInfo producerInfo;
        if (mContentServer->getProducerInfoByName(0,producerName,producerInfo) == 0)
        {
          //producerInfo.print(std::cout,0,0);

          T::GenerationInfoList generationInfoList;
          int result = mContentServer->getGenerationInfoListByProducerId(0,producerInfo.mProducerId,generationInfoList);
          if (result != 0)
          {
            SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
            exception.addParameter("Service","getGenerationInfoListByProducerId");
            exception.addParameter("Message",ContentServer::getResultString(result));
            throw exception;
          }

          T::GenerationInfo *generationInfo = generationInfoList.getLastGenerationInfoByProducerId(producerInfo.mProducerId);

          uint gCount = 0;

          while (generationInfo != NULL  &&  gCount < 2)
          {
            // We have now a valid generation

            //generationInfo->print(std::cout,0,0);

            for (auto pInfo = parameterInfoList.begin(); pInfo != parameterInfoList.end(); ++pInfo)
            {
              //pInfo->print(std::cout,0,0);

              T::ContentInfoList contentList;
              int result = mContentServer->getContentListByParameterGenerationIdAndForecastTime(0,generationInfo->mGenerationId,pInfo->mParameterKeyType,pInfo->mParameterKey,*pInfo->mParameterLevelIdType,*pInfo->mParameterLevelId,*pInfo->mParameterLevel,forecastType,forecastNumber,producerGeometryId,forecastTime,contentList);
              if (result != 0)
              {
                SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
                exception.addParameter("Service","getContentListByParameterGenerationIdAndForecastTime");
                exception.addParameter("Message",ContentServer::getResultString(result));
                throw exception;
              }

              bool multipleOptions = contentList.containsSameForecastTimes();
              if (multipleOptions)
                printf("**** CONTAINS MULTIPLE SAME TIMESTEPS ********\n");

//            contentList.print(std::cout,0,0);
              if (contentList.getLength() > 0)
              {
                // We found content information close to the current forecast time

                valueList.mParameterKeyType = pInfo->mParameterKeyType;
                valueList.mParameterKey = pInfo->mParameterKey;
                valueList.mParameterLevelIdType = *pInfo->mParameterLevelIdType;
                valueList.mParameterLevelId = *pInfo->mParameterLevelId;

                if (contentList.getLength() == 1)
                {
                  T::ContentInfo *contentInfo = contentList.getContentInfoByIndex(0);
                  if (contentInfo->mForecastTime == forecastTime)
                  {
                    // We found a grid which forecast time is exactly the same as the requested forecast time.

                    valueList.mForecastTime = forecastTime;
                    valueList.mProducerId = contentInfo->mProducerId;
                    valueList.mGenerationId = contentInfo->mGenerationId;
                    valueList.mGeometryId = contentInfo->mGeometryId;
                    valueList.mForecastType = contentInfo->mForecastType;
                    valueList.mForecastNumber = contentInfo->mForecastNumber;
                    valueList.mParameterLevel = contentInfo->mParameterLevel;

                    if (!areaSearch)
                    {
                      if (!multipleOptions)
                      {
                        int result = mDataServer->getGridValueListByPointList(0,contentInfo->mFileId,contentInfo->mMessageIndex,T::CoordinateType::LATLON_COORDINATES,coordinates[0],pInfo->mInterpolationMethod,valueList.mValueList);
                        if (result != 0)
                        {
                          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                          exception.addParameter("Service","getGridValueListByPointList");
                          exception.addParameter("Message",DataServer::getResultString(result));
                          throw exception;
                        }
                      }
                    }
                    else
                    {
                      if (!multipleOptions)
                      {
                        int result = mDataServer->getGridValueListByPolygonPath(0,contentInfo->mFileId,contentInfo->mMessageIndex,T::CoordinateType::LATLON_COORDINATES,coordinates,valueList.mValueList);
                        if (result != 0)
                        {
                          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                          exception.addParameter("Service","getGridValueListByPolygonPath");
                          exception.addParameter("Message",DataServer::getResultString(result));
                          throw exception;
                        }
                      }
                    }

                    return;
                  }
                  else
                  {
                    SmartMet::Spine::Exception exception(BCP, "Unexpected result!");
                    exception.addDetail("If there is only one content record in place then its forecast time should match to the requested forecast time.");
                    exception.addParameter("Content ForecastTime",contentInfo->mForecastTime);
                    exception.addParameter("Request ForecastTime",forecastTime);
                    throw exception;
                  }
                }

                if (contentList.getLength() == 2 &&  !timeMatchRequired)
                {
                  T::ContentInfo *contentInfo1 = contentList.getContentInfoByIndex(0);
                  T::ContentInfo *contentInfo2 = contentList.getContentInfoByIndex(1);

                  boost::posix_time::ptime forecastTime1 = toTimeStamp(contentInfo1->mForecastTime);
                  boost::posix_time::ptime forecastTime2 = toTimeStamp(contentInfo2->mForecastTime);

                  boost::posix_time::ptime forecastTimeParam = toTimeStamp(forecastTime);


                  if (contentInfo1->mForecastTime < forecastTime  &&  contentInfo2->mForecastTime > forecastTime)
                  {
                    valueList.mForecastTime = forecastTime;
                    valueList.mProducerId = contentInfo1->mProducerId;
                    valueList.mGenerationId = contentInfo1->mGenerationId;
                    valueList.mGeometryId = contentInfo1->mGeometryId;
                    valueList.mForecastType = contentInfo1->mForecastType;
                    valueList.mForecastNumber = contentInfo1->mForecastNumber;
                    valueList.mParameterLevel = contentInfo1->mParameterLevel;

                    T::GridValueList list1;
                    T::GridValueList list2;

                    time_t diff = toTimeT(forecastTimeParam) - toTimeT(forecastTime1);
                    time_t ttDiff = toTimeT(forecastTime2) - toTimeT(forecastTime1);

                    int result1 = 0;
                    int result2 = 0;
                    if (!areaSearch)
                    {
                      if (!multipleOptions)
                      {
                        result1 = mDataServer->getGridValueListByPointList(0,contentInfo1->mFileId,contentInfo1->mMessageIndex,T::CoordinateType::LATLON_COORDINATES,coordinates[0],pInfo->mInterpolationMethod,list1);
                        if (result1 != 0)
                        {
                          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                          exception.addParameter("Service","getGridValueListByPointList");
                          exception.addParameter("Message",DataServer::getResultString(result1));
                          throw exception;
                        }

                        result2 = mDataServer->getGridValueListByPointList(0,contentInfo2->mFileId,contentInfo2->mMessageIndex,T::CoordinateType::LATLON_COORDINATES,coordinates[0],pInfo->mInterpolationMethod,list2);
                        if (result2 != 0)
                        {
                          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                          exception.addParameter("Service","getGridValueListByPointList");
                          exception.addParameter("Message",DataServer::getResultString(result2));
                          throw exception;
                        }
                      }
                    }
                    else
                    {
                      if (!multipleOptions)
                      {
                        result1 = mDataServer->getGridValueListByPolygonPath(0,contentInfo1->mFileId,contentInfo1->mMessageIndex,T::CoordinateType::LATLON_COORDINATES,coordinates,list1);
                        if (result1 != 0)
                        {
                          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                          exception.addParameter("Service","getGridValueListByPointList");
                          exception.addParameter("Message",DataServer::getResultString(result1));
                          throw exception;
                        }

                        result2 = mDataServer->getGridValueListByPolygonPath(0,contentInfo2->mFileId,contentInfo2->mMessageIndex,T::CoordinateType::LATLON_COORDINATES,coordinates,list2);
                        if (result2 != 0)
                        {
                          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                          exception.addParameter("Service","getGridValueListByPolygonPath");
                          exception.addParameter("Message",DataServer::getResultString(result2));
                          throw exception;
                        }
                      }
                    }

                    uint len1 = list1.getLength();
                    uint len2 = list2.getLength();

                    if (result1 == 0  &&  result2 == 0  &&  len1 == len2)
                    {
                      for (uint t=0; t<len1; t++)
                      {
                        T::GridValue *val1 = list1.getGridValueByIndex(t);
                        T::GridValue *val2 = list2.getGridValueByIndex(t);

                        T::ParamValue valueDiff = val2->mValue - val1->mValue;
                        T::ParamValue valueStep = valueDiff / (T::ParamValue)ttDiff;

                        T::GridValue *rec = new T::GridValue();
                        rec->mX = val1->mX;
                        rec->mY = val1->mY;
                        rec->mValue = val1->mValue + (T::ParamValue)(diff * valueStep);
                        valueList.mValueList.addGridValue(rec);
                      }
                    }
                    return;
                  }
                  else
                  {
                    SmartMet::Spine::Exception exception(BCP, "Unexpected result!");
                    exception.addDetail("The given forecast time should been between the found content times.");
                    exception.addParameter("Content 1 ForecastTime",contentInfo1->mForecastTime);
                    exception.addParameter("Content 2 ForecastTime",contentInfo2->mForecastTime);
                    exception.addParameter("Request ForecastTime",forecastTime);
                    throw exception;
                  }
                }
              }
            }
            generationInfo = generationInfoList.getPrevGenerationInfoByProducerId(generationInfo->mProducerId,generationInfo->mName);
            gCount++;
          }
        }
        else
        {
          // printf("Producer '%s' not found\n",producerName.c_str());
        }
      }
      else
      {
        // printf("Producer's '%s' geometry '%d' not supported\n",producerName.c_str(),producerGeometryId);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::getGridValues(
                        Producer_vec& producers,
                        ParameterInfo_vec& parameterInfoList,
                        T::ForecastType forecastType,
                        T::ForecastNumber forecastNumber,
                        std::string startTime,
                        std::string endTime,
                        QueryCoordinates& coordinates,
                        bool areaSearch,
                        ParameterValues_vec& valueList)
{
  try
  {
    if (coordinates.size() == 0)
    {
      SmartMet::Spine::Exception exception(BCP, "No coordinates defined!");
      throw exception;
    }

    if (areaSearch  &&  coordinates.size() < 3)
    {
      SmartMet::Spine::Exception exception(BCP, "Area definition requires at least 3 coordinates!");
      throw exception;
    }

    // Getting geometries that support support the given coordinates

    std::set<T::GeometryId> geometryIdList;
    getGeometryIdListByCoordinates(coordinates,geometryIdList);


    uint requestFlags = (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER;


    // No producers defined. We should go through the producer list that is defined
    // in the configuration file.

    for (auto it = producers.begin(); it != producers.end(); ++it)
    {
      std::string producerName = it->first;

      T::GeometryId producerGeometryId = it->second;

      printf("Producer %s:%d\n",producerName.c_str(),producerGeometryId);

      if (geometryIdList.find(producerGeometryId) != geometryIdList.end())
      {
        // The current producer supports a geometry where the current coordinates can be found.
        T::ProducerInfo producerInfo;
        if (mContentServer->getProducerInfoByName(0,producerName,producerInfo) == 0)
        {
          //producerInfo.print(std::cout,0,0);

          T::GenerationInfoList generationInfoList;
          int result = mContentServer->getGenerationInfoListByProducerId(0,producerInfo.mProducerId,generationInfoList);
          if (result != 0)
          {
            SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
            exception.addParameter("Service","getGenerationInfoListByProducerId");
            exception.addParameter("Message",ContentServer::getResultString(result));
            throw exception;
          }

          T::GenerationInfo *generationInfo = generationInfoList.getLastGenerationInfoByProducerId(producerInfo.mProducerId);

          uint gCount = 0;

          while (generationInfo != NULL  &&  gCount < 2)
          {
            // We have now a valid generation

            //generationInfo->print(std::cout,0,0);

            for (auto pInfo = parameterInfoList.begin(); pInfo != parameterInfoList.end(); ++pInfo)
            {
              //pInfo->print(std::cout,0,0);

              T::ContentInfoList contentList;
              int result = mContentServer->getContentListByParameterAndGenerationId(0,generationInfo->mGenerationId,pInfo->mParameterKeyType,pInfo->mParameterKey,*pInfo->mParameterLevelIdType,*pInfo->mParameterLevelId,*pInfo->mParameterLevel,*pInfo->mParameterLevel,forecastType,forecastNumber,producerGeometryId,startTime,endTime,requestFlags,contentList);
              if (result != 0)
              {
                SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
                exception.addParameter("Service","getContentListByParameterAndGenerationId");
                exception.addParameter("Message",ContentServer::getResultString(result));
                throw exception;
              }

              bool multipleOptions = contentList.containsSameForecastTimes();
              if (multipleOptions)
              {
                printf("**** CONTAINS MULTIPLE SAME TIMESTEPS ********\n");
                contentList.print(std::cout,0,0);
              }

              std::string lastTime = startTime;

              uint clen = contentList.getLength();

              for (uint t=0; t<clen; t++)
              {
                T::ContentInfo *contentInfo = contentList.getContentInfoByIndex(t);
                //printf("%s:%d:%s\n",pInfo->mParameterKey.c_str(),contentInfo->mGeometryId,contentInfo->mForecastTime.c_str());

                if (contentInfo->mForecastTime >= startTime  &&  contentInfo->mForecastTime <= endTime)
                {
                  ParameterValues valList;
                  valList.mForecastTime = contentInfo->mForecastTime;
                  valList.mProducerId = contentInfo->mProducerId;
                  valList.mGenerationId = contentInfo->mGenerationId;
                  valList.mGeometryId = contentInfo->mGeometryId;
                  valList.mParameterKeyType = pInfo->mParameterKeyType;
                  valList.mParameterKey = pInfo->mParameterKey;
                  valList.mParameterLevelIdType = *pInfo->mParameterLevelIdType;
                  valList.mParameterLevelId = *pInfo->mParameterLevelId;
                  valList.mParameterLevel = contentInfo->mParameterLevel;
                  valList.mForecastType = contentInfo->mForecastType;
                  valList.mForecastNumber = contentInfo->mForecastNumber;

                  int result = 0;
                  if (!areaSearch)
                  {
                    if (!multipleOptions)
                    {
                      result = mDataServer->getGridValueListByPointList(0,contentInfo->mFileId,contentInfo->mMessageIndex,T::CoordinateType::LATLON_COORDINATES,coordinates[0],pInfo->mInterpolationMethod,valList.mValueList);
                      if (result != 0)
                      {
                        SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                        exception.addParameter("Service","getGridValueListByPointList");
                        exception.addParameter("Message",DataServer::getResultString(result));
                        exception.printError();
                        //throw exception;
                      }
                    }
                  }
                  else
                  {
                    if (!multipleOptions)
                    {
                      result = mDataServer->getGridValueListByPolygonPath(0,contentInfo->mFileId,contentInfo->mMessageIndex,T::CoordinateType::LATLON_COORDINATES,coordinates,valList.mValueList);
                      if (result != 0)
                      {
                        SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                        exception.addParameter("Service","getGridValueListByPolygonPath");
                        exception.addParameter("Message",DataServer::getResultString(result));
                        exception.printError();
                        //throw exception;
                      }
                    }
                  }

                  if (result == 0)
                  {
                    if (contentInfo->mForecastTime > lastTime)
                      lastTime = contentInfo->mForecastTime;

                    //printf("*** VALUES ****\n");
                    //valList.print(std::cout,0,0);
                    valueList.push_back(valList);
                  }
                }

                if (contentInfo->mForecastTime >= endTime)
                {
                  // We have found the full time range from this generation, so we
                  // should end the search. It is good to notice that the forecast
                  // time of the last content is not necessary the same as the exact
                  // end time.

                  return;
                }

              }

              if (lastTime >= endTime)
                return;

              // We have not found content for the full time range, so we should continue to search.

              if (lastTime > startTime)
              {
                boost::posix_time::ptime tt = toTimeStamp(lastTime) + boost::posix_time::minutes(1);
                startTime = toString(tt);
              }
            }
            generationInfo = generationInfoList.getPrevGenerationInfoByProducerId(generationInfo->mProducerId,generationInfo->mName);
            gCount++;
          }
        }
        else
        {
           printf("Producer '%s' not found\n",producerName.c_str());
        }
      }
      else
      {
        printf("Producer's '%s' geometry '%d' not supported\n",producerName.c_str(),producerGeometryId);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::getParameterInfoList(T::ParamKeyType parameterKeyType,
                                  T::ParamId paramKey,
                                  T::ParamLevelIdType paramLevelIdType,
                                  T::ParamLevelId paramLevelId,
                                  T::ParamLevel paramLevel,
                                  ParameterInfo_vec& parameterInfoList)
{
  try
  {
    getParameterInfoList(paramKey,paramLevelIdType,paramLevelId,paramLevel,parameterInfoList);

    if (parameterInfoList.size() == 0)
    {
      // Parameter not found from the parameter configuration file.

      switch (parameterKeyType)
      {
        case T::ParamKeyType::UNKNOWN:
        {
          // Testing if the parameterKey is a Radon name
          Identification::ParameterDefinition_fmi_cptr parameterDef = SmartMet::Identification::gribDef.mMessageIdentifier_fmi.getParameterDefByName(paramKey);

          // Testing if the parameterKey is a Newbase name
          if (parameterDef == NULL)
            parameterDef = SmartMet::Identification::gribDef.mMessageIdentifier_fmi.getParameterDefByNewbaseName(paramKey);

          if (parameterDef == NULL)
          {
            SmartMet::Spine::Exception exception(BCP, "Unknown parameter!");
            exception.addParameter("Parameter",paramKey);
            throw exception;
          }

          ParameterInfo parameterInfo;
          parameterInfo.mParameterName = paramKey;
          parameterInfo.mParameterKeyType = T::ParamKeyType::FMI_ID;
          parameterInfo.mParameterKey = parameterDef->mFmiParameterId;
          parameterInfo.mParameterLevelIdType = paramLevelIdType;
          parameterInfo.mParameterLevelId = paramLevelId;
          parameterInfo.mParameterLevel = paramLevel;
          parameterInfo.mInterpolationMethod = parameterDef->mInterpolationMethod;
          parameterInfoList.push_back(parameterInfo);
        }
        break;

        case T::ParamKeyType::FMI_ID:
        {
          Identification::ParameterDefinition_fmi_cptr parameterDef = SmartMet::Identification::gribDef.mMessageIdentifier_fmi.getParameterDefById(paramKey);
          if (parameterDef == NULL)
          {
            SmartMet::Spine::Exception exception(BCP, "Unknown Radon parameter id!");
            exception.addParameter("Parameter",paramKey);
            throw exception;
          }

          ParameterInfo parameterInfo;
          parameterInfo.mParameterName = paramKey;
          parameterInfo.mParameterKeyType = T::ParamKeyType::FMI_ID;
          parameterInfo.mParameterKey = parameterDef->mFmiParameterId;
          parameterInfo.mParameterLevelIdType = paramLevelIdType;
          parameterInfo.mParameterLevelId = paramLevelId;
          parameterInfo.mParameterLevel = paramLevel;
          parameterInfo.mInterpolationMethod = parameterDef->mInterpolationMethod;
          parameterInfoList.push_back(parameterInfo);
        }
        break;

        case T::ParamKeyType::FMI_NAME:
        {
          Identification::ParameterDefinition_fmi_cptr parameterDef = SmartMet::Identification::gribDef.mMessageIdentifier_fmi.getParameterDefByName(paramKey);
          if (parameterDef == NULL)
          {
            SmartMet::Spine::Exception exception(BCP, "Unknown Radon parameter name!");
            exception.addParameter("Parameter",paramKey);
            throw exception;
          }

          ParameterInfo parameterInfo;
          parameterInfo.mParameterName = paramKey;
          parameterInfo.mParameterKeyType = T::ParamKeyType::FMI_ID;
          parameterInfo.mParameterKey = parameterDef->mFmiParameterId;
          parameterInfo.mParameterLevelIdType = paramLevelIdType;
          parameterInfo.mParameterLevelId = paramLevelId;
          parameterInfo.mParameterLevel = paramLevel;
          parameterInfo.mInterpolationMethod = parameterDef->mInterpolationMethod;
          parameterInfoList.push_back(parameterInfo);
        }
        break;

        case T::ParamKeyType::GRIB_ID:
          break;

        case T::ParamKeyType::NEWBASE_ID:
        {
          Identification::ParameterDefinition_fmi_cptr parameterDef = SmartMet::Identification::gribDef.mMessageIdentifier_fmi.getParameterDefByNewbaseId(paramKey);
          if (parameterDef == NULL)
          {
            SmartMet::Spine::Exception exception(BCP, "Unknown Newbase parameter id!");
            exception.addParameter("Parameter",paramKey);
            throw exception;
          }

          ParameterInfo parameterInfo;
          parameterInfo.mParameterName = paramKey;
          parameterInfo.mParameterKeyType = T::ParamKeyType::FMI_ID;
          parameterInfo.mParameterKey = parameterDef->mFmiParameterId;
          parameterInfo.mParameterLevelIdType = paramLevelIdType;
          parameterInfo.mParameterLevelId = paramLevelId;
          parameterInfo.mParameterLevel = paramLevel;
          parameterInfo.mInterpolationMethod = parameterDef->mInterpolationMethod;
          parameterInfoList.push_back(parameterInfo);
        }
        break;

        case T::ParamKeyType::NEWBASE_NAME:
        {
          Identification::ParameterDefinition_fmi_cptr parameterDef = SmartMet::Identification::gribDef.mMessageIdentifier_fmi.getParameterDefByNewbaseName(paramKey);
          if (parameterDef == NULL)
          {
            SmartMet::Spine::Exception exception(BCP, "Unknown Newbase parameter name!");
            exception.addParameter("Parameter",paramKey);
            throw exception;
          }

          ParameterInfo parameterInfo;
          parameterInfo.mParameterName = paramKey;
          parameterInfo.mParameterKeyType = T::ParamKeyType::FMI_ID;
          parameterInfo.mParameterKey = parameterDef->mFmiParameterId;
          parameterInfo.mParameterLevelIdType = paramLevelIdType;
          parameterInfo.mParameterLevelId = paramLevelId;
          parameterInfo.mParameterLevel = paramLevel;
          parameterInfo.mInterpolationMethod = parameterDef->mInterpolationMethod;
          parameterInfoList.push_back(parameterInfo);
        }
        break;

        case T::ParamKeyType::CDM_ID:
          break;

        case T::ParamKeyType::CDM_NAME:
          break;


        case T::ParamKeyType::BUILD_IN:
        {
          ParameterInfo parameterInfo;
          parameterInfo.mParameterName = paramKey;
          parameterInfo.mParameterKeyType = T::ParamKeyType::BUILD_IN;
          parameterInfo.mParameterKey = paramKey;
          parameterInfo.mParameterLevelIdType = paramLevelIdType;
          parameterInfo.mParameterLevelId = paramLevelId;
          parameterInfo.mParameterLevel = paramLevel;
          parameterInfoList.push_back(parameterInfo);
        }
        break;

        default:
        {
          SmartMet::Spine::Exception exception(BCP, "Unknown parameter id type!");
          exception.addParameter("ParameterIdType",std::to_string((uint)parameterKeyType));
          throw exception;
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





bool ServiceImplementation::getParameterFunctionInfo(std::string paramStr,std::string& function,std::string& functionParams)
{
  try
  {
    char buf[1000];
    char tmp[1000];

    uint c = 0;
    strcpy(buf,paramStr.c_str());
    char *p = buf;

    while (*p != '\0'  &&  *p != '{')
    {
      p++;
    }

    if (*p == '{')
    {
      *p = '\0';
      p++;
      int level = 1;
      while (*p != '\0')
      {
        if (*p == '}')
        {
          level--;
          if (level == 0)
          {
            tmp[c] = '\0';
            function = buf;
            functionParams = tmp;
            //printf("FUNCTION [%s] [%s]\n",buf,tmp);
            return true;
          }
        }
        else
        if (*p == '{')
          level++;

        tmp[c] = *p;
        c++;
        p++;
      }
    }
    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}






bool ServiceImplementation::getFunctionParams(std::string functionParamsStr,FunctionParam_vec& functionParams)
{
  try
  {
    bool containsFunction = false;
    char buf[1000];
    char tmp[1000];
    uint c = 0;
    strcpy(buf,functionParamsStr.c_str());

    char *p = buf;
    uint level = 0;
    while (*p != '\0')
    {
      if (*p == '{')
      {
        containsFunction = true;
        level++;
      }

      if (*p == '}')
        level--;

      if (*p == ';'  &&  level == 0)
      {
        tmp[c] = '\0';
        //printf("PARAM [%s]\n",tmp);
        mFunctionParamId++;
        functionParams.push_back(std::pair<uint,std::string>(mFunctionParamId,std::string(tmp)));
        c = 0;
      }
      else
      {
        tmp[c] = *p;
        c++;
      }
      p++;
    }

    if (c > 0)
    {
      tmp[c] = '\0';
      //printf("PARAM [%s]\n",tmp);
      mFunctionParamId++;
      functionParams.push_back(std::pair<uint,std::string>(mFunctionParamId,std::string(tmp)));
    }
    return containsFunction;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}






void ServiceImplementation::getParameterLevelInfo(std::string param,std::string& key,T::ParamLevelId& paramLevelId,T::ParamLevel& paramLevel,T::ForecastType& forecastType,T::ForecastNumber& forecastNumber)
{
  try
  {
    char buf[1000];
    strcpy(buf,param.c_str());

    char *field[100];
    uint c = 1;
    field[0] = buf;
    char *p = buf;
    int level = 0;
    while (*p != '\0'  &&  c < 100)
    {
      if (*p == '{')
        level++;

      if (*p == '}')
        level--;

      if (level == 0  &&  *p == ':')
      {
        *p = '\0';
        p++;
        field[c] = p;
        c++;
      }
      else
      {
        p++;
      }
    }

    //for (uint t=0; t<c; t++)
    //  printf("FIELD (%s) %u [%s]\n",buf,t,field[t]);


    key = field[0];

    if (c > 1)
    {
      if (field[1][0] != '\0')
        paramLevelId = (T::ParamLevelId)atoi(field[1]);
    }

    if (c > 2)
    {
      if (field[2][0] != '\0')
        paramLevel = atoi(field[2]);
    }

    if (c > 3)
    {
      if (field[3][0] != '\0')
        forecastType = (T::ForecastType)atoi(field[3]);
    }

    if (c > 4)
    {
      if (field[4][0] != '\0')
        forecastNumber = (T::ForecastNumber)atoi(field[4]);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





bool ServiceImplementation::parseFunction(QueryParameter& queryParam,std::string paramStr,std::string& function,FunctionParam_vec& functionParams,std::vector<QueryParameter>& additionalParameterList)
{
  try
  {
    // printf("PARSE FUNCTION %s\n",paramStr.c_str());
    std::string functionParamsStr;
    if (getParameterFunctionInfo(paramStr,function,functionParamsStr))
    {
      // printf("PARSE %s => %s  %s\n",paramStr.c_str(),function.c_str(),functionParamsStr.c_str());
      getFunctionParams(functionParamsStr,functionParams);

      for (auto fParam = functionParams.begin(); fParam != functionParams.end(); ++fParam)
      {
        if (atof(fParam->second.c_str()) == 0)
        {
          QueryParameter newParam;
          newParam.mId = fParam->first;
          newParam.mParam = fParam->second;
          newParam.mSymbolicName = fParam->second;
          newParam.mParameterLevelId = queryParam.mParameterLevelId;
          newParam.mParameterLevel = queryParam.mParameterLevel;
          newParam.mForecastType = queryParam.mForecastType;
          newParam.mForecastNumber = queryParam.mForecastNumber;
          newParam.mInterpolationMethod = queryParam.mInterpolationMethod;
          newParam.mTemporary = true;

          auto alias = mParameterAliasList.find(toLowerString(fParam->second));
          if (alias != mParameterAliasList.end())
            newParam.mParam = alias->second.mParameterString;

          parseFunction(newParam,newParam.mParam,newParam.mFunction,newParam.mFunctionParams,additionalParameterList);

          additionalParameterList.insert(additionalParameterList.begin(),newParam);
        }
      }
      return true;
    }
    else
    {
      char tmp[1000];
      strcpy(tmp,paramStr.c_str());
      char *p = strstr(tmp,":");
      if (p != NULL)
        *p = '\0';

      auto alias = mParameterAliasList.find(toLowerString(std::string(tmp)));
      if (alias != mParameterAliasList.end())
      {
        if (alias->second.mParameterString != paramStr)
        {
          if (!parseFunction(queryParam,alias->second.mParameterString,function,functionParams,additionalParameterList))
            queryParam.mParam = alias->second.mParameterString;
        }
      }
      return false;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





T::ParamValue ServiceImplementation::executeFunction(std::string& function,std::vector<T::ParamValue>& parameters)
{
  try
  {
    int pLen = (int)parameters.size();

    lua_getglobal(mLuaState,"main");
    lua_pushstring(mLuaState,function.c_str());
    lua_pushinteger(mLuaState,pLen);

    lua_newtable(mLuaState);
    for (int i = 0; i < pLen; i++)
    {
      lua_pushnumber(mLuaState,parameters[i]);
      lua_rawseti(mLuaState,-2,i + 1);
    }

    int res = lua_pcall(mLuaState, 3, 2, 0);
    if (res != 0)
    {
      // LUA ERROR
      SmartMet::Spine::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("Lua message",lua_tostring(mLuaState, -1));
      lua_pop(mLuaState, 1);
      throw exception;
    }
    else
    {
      size_t len = 0;
      const char* cstr = lua_tolstring(mLuaState, -1, &len);
      std::string message(cstr, len);
      lua_pop(mLuaState, 1);  /* pop returned value */

      double value = lua_tonumber(mLuaState,-1);
      lua_pop(mLuaState, 1);  /* pop returned value */

      if (message != "OK")
      {
        printf("*** LUA RESULT %f (%s)\n\n",value,message.c_str());
        value = ParamValueMissing;
      }

      return value;
    }
    return 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::checkConfigurationUpdates()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if ((time(0) - mLastConfiguratonCheck) > 5)
    {
      mLastConfiguratonCheck = time(0);

      time_t t1 = getFileModificationTime(mProducerFile.c_str());
      time_t t2 = getFileModificationTime(mParameterFile.c_str());
      time_t t3 = getFileModificationTime(mParameterAliasFile.c_str());
      time_t t4 = getFileModificationTime(mLuaFunctionFile.c_str());

      if (mProducerFileModificationTime != t1  &&  (mLastConfiguratonCheck - t1) > 5)
        loadProducerFile();

      if (mParameterFileModificationTime != t2  &&  (mLastConfiguratonCheck - t2) > 5)
        loadParameterFile();

      if (mParameterAliasFileModificationTime != t3  &&  (mLastConfiguratonCheck - t3) > 5)
        loadParameterAliasFile();

      if (mLuaFunctionFileModificationTime != t4  &&  (mLastConfiguratonCheck - t4) > 5)
        loadLuaFunctionFile();
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::getProducerList(Query& query,Producer_vec& producers)
{
  try
  {
    if (query.mProducerNameList.size() == 0)
    {
      producers = mProducerList;
      return;
    }

    for (auto pName = query.mProducerNameList.begin(); pName != query.mProducerNameList.end(); ++pName)
    {
      for (auto it = mProducerList.begin(); it != mProducerList.end(); ++it)
      {
        if (strcasecmp(pName->c_str(),it->first.c_str()) == 0)
          producers.push_back(std::pair<std::string,T::GeometryId>(it->first,it->second));
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::updateQueryParameters(Query& query)
{
  try
  {
    bool updateRequired = true;
    uint updateCounter = 0;

    while (updateRequired  &&  updateCounter < 5)
    {
      updateRequired = false;
      updateCounter++;

      std::vector<QueryParameter> additionalParameterList;

      // Going through all the query parameters.

      for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
      {
        // The parameter can contain some level information even if the parameter is an alias
        // or a function. In this case this level information should be inherited to the alias
        // definition or to the function parameters.

        getParameterLevelInfo(qParam->mParam,qParam->mParameterKey,qParam->mParameterLevelId,qParam->mParameterLevel,qParam->mForecastType,qParam->mForecastNumber);

        // If the parameter is a symbolic name (defined in the configuration file) then
        // we should use its definition.

        auto alias = mParameterAliasList.find(toLowerString(qParam->mParameterKey));
        if (alias != mParameterAliasList.end())
        {
          std::cout << qParam->mSymbolicName << "  => " << alias->second.mParameterString << " ( " << qParam->mParam << " )\n";
          qParam->mParam = alias->second.mParameterString;

          // When an alias is replaced by its definition, it is possible that this definition
          // contains other aliases. That's why we need to repeat this "updateQueryParamters"
          // loop. The 'updateCounter' variable is used in order to ensure that the loop
          // will end if the aliases are referencing to each other (=> eternal loop).

          updateRequired = true;
        }

        // If the parameter contains a function then we should parse it and make sure that
        // we fetch the required parameter values for it.

        if (qParam->mFunction.length() == 0)
          parseFunction(*qParam,qParam->mParam,qParam->mFunction,qParam->mFunctionParams,additionalParameterList);
      }

      // Adding parameters that are required by functions into the query.

      for (auto p = additionalParameterList.begin(); p != additionalParameterList.end(); ++p)
        query.mQueryParameterList.push_back(*p);
    }

    query.print(std::cout,0,0);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::executeQueryFunctions(Query& query)
{
  try
  {
    uint valueCount = query.getValuesPerTimeStep();

    for (auto qParam = query.mQueryParameterList.rbegin(); qParam != query.mQueryParameterList.rend(); ++qParam)
    {
      if (qParam->mFunction.length() > 0)
      {
        uint tCount = 0;
        for (auto tt = query.mForecastTimeList.begin(); tt != query.mForecastTimeList.end(); ++tt)
        {
          ParameterValues pValues;
          pValues.mForecastTime = *tt;

          std::vector<T::ParamValue> areaParameters;
          std::vector<T::ParamValue> extParameters;

          bool areaCnt = false;
          T::GridValue lastRec;
          for (uint v=0; v < valueCount; v++)
          {
            std::vector<T::ParamValue> parameters;

            uint producerId = 0;
            uint generationId = 0;
            uint geometryId = 0;

            for (auto it = qParam->mFunctionParams.begin(); it != qParam->mFunctionParams.end(); ++it)
            {
              T::ParamValue a = atof(it->second.c_str());
              QueryParameter *q = query.getQueryParameterPtr(it->first);
              if (q != NULL)
              {
                if (tCount < (uint)q->mValueList.size())
                {
                  if (producerId == 0  &&  q->mValueList[tCount].mProducerId > 0)
                  {
                    producerId = q->mValueList[tCount].mProducerId;
                    generationId = q->mValueList[tCount].mGenerationId;
                    geometryId = q->mValueList[tCount].mGeometryId;
                  }

                  T::GridValue *rec = q->mValueList[tCount].mValueList.getGridValueByIndex(v);
                  if (rec != NULL)
                  {
                    a = rec->mValue;
                    if (rec->mX == -1000  &&  rec->mY == -1000)
                    {
                      areaCnt = true;
                    }
                    else
                    {
                      lastRec = *rec;
                    }
                  }
                  else
                  {
                    a = ParamValueMissing;
                  }
                }
                else
                {
                  areaCnt = true;
                }
              }

              parameters.push_back(a);
              areaParameters.push_back(a);

              if (areaCnt  &&  v == 0)
                extParameters.push_back(a);
            }

            if (qParam->mFunction.substr(0,1) != "@")
            {
              T::ParamValue val = executeFunction(qParam->mFunction,parameters);
              pValues.mValueList.addGridValue(new T::GridValue(lastRec.mX,lastRec.mY,val));
              pValues.mProducerId = producerId;
              pValues.mGenerationId = generationId;
              pValues.mGeometryId = geometryId;
            }
          }

          if (areaCnt  &&  qParam->mFunction.substr(0,1) != "@")
          {
            T::ParamValue val = executeFunction(qParam->mFunction,extParameters);
            pValues.mValueList.addGridValue(new T::GridValue(-1000,-1000,val));
          }

          if (qParam->mFunction.substr(0,1) == "@")
          {
            //T::ParamValue val = executeAreaFunction(qParam->mFunction,areaParameters);
            std::string func = qParam->mFunction.substr(1);
            T::ParamValue val = executeFunction(func,areaParameters);
            pValues.mValueList.addGridValue(new T::GridValue(-1000,-1000,val));
          }

          if (areaCnt)
          {
            if (pValues.mValueList.getLength() > 0)
            {
              T::GridValue *rec = pValues.mValueList.getGridValueByIndex(0);
              T::ParamValue val = rec->mValue;
              pValues.mValueList.clear();
              pValues.mValueList.addGridValue(new T::GridValue(-1000,-1000,val));
            }
          }
          qParam->mValueList.push_back(pValues);

          tCount++;
        }
      }
    }

    query.removeTemporaryParameters();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ServiceImplementation::executeTimeRangeQuery(Query& query)
{
  try
  {
    // Fetching valid producers.

    Producer_vec producers;
    getProducerList(query,producers);

    // Parsing parameters and functions in the query.

    updateQueryParameters(query);

    bool areaQuery = false;
    if ((query.mFlags & QF_AREA_QUERY) != 0)
      areaQuery = true;

    // Fetching parameter data according to the given time range and the coordinate list. Notice
    // that the coordinate list can be used in two ways. It can 1) contain coordinate points
    // where the data will be fetched or 2) it can define an area (= polygon) where the grid
    // points are fetched. The area can be defined by using single or multiple polygons.
    // That's why the coordinates are defined as a vector of coordinate vectors.

    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
    {
      // Checking that the parameter is "a data parameter" - not a function.

      if (qParam->mFunction.length() == 0)
      {
        std::vector<ParameterInfo> parameterInfoList;

        std::string key = qParam->mParam;
        T::ParamLevelId paramLevelId = qParam->mParameterLevelId;
        T::ParamLevel paramLevel = qParam->mParameterLevel;
        T::ForecastType forecastType = qParam->mForecastType;
        T::ForecastNumber forecastNumber = qParam->mForecastNumber;

        qParam->print(std::cout,0,0);

        getParameterLevelInfo(qParam->mParam,key,paramLevelId,paramLevel,forecastType,forecastNumber);

        getParameterInfoList(qParam->mParameterKeyType,key,qParam->mParameterLevelIdType,paramLevelId,paramLevel,parameterInfoList);

        getGridValues(producers,parameterInfoList,forecastType,forecastNumber,query.mStartTime,query.mEndTime,query.mCoordinateList,areaQuery,qParam->mValueList);
      }
    }

    // Finding out which forecast time are found from the forecast data. The point is that different
    // parameters might contain different forecast times, and we want a list of all forecast times.

    std::set<std::string> timeList;

    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
    {
      for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end(); ++it)
      {
        if (timeList.find(it->mForecastTime) == timeList.end())
          timeList.insert(it->mForecastTime);
      }
    }

    // Going through all the found forecast times and making sure that each parameter contains the current
    // forecast time. If not, then the forecast time is added to the parameters's forecast time list, but
    // the actual value list of the current forecast time will be empty.

    for (auto tt = timeList.begin(); tt != timeList.end(); ++tt)
    {
      query.mForecastTimeList.push_back(*tt);

      for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
      {
        if (qParam->mFunction.length() == 0)
        {
          bool found = false;
          uint cnt = 0;
          for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end() &&  !found; ++it)
          {
            if (it->mForecastTime < *tt)
              cnt++;
            else
            if (it->mForecastTime == *tt)
              found = true;
          }

          if (!found)
          {
            // The forecast time was not found from the current parameter. Adding the forecast time
            // with an empty value list.

            ParameterValues pValues;
            pValues.mForecastTime = *tt;
            qParam->mValueList.insert(qParam->mValueList.begin() + cnt,pValues);
          }
        }
      }
    }

    // At this point we have fetched values for all data parameters. Now we are able to
    // execute functions that uses this data.

    executeQueryFunctions(query);
    return 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ServiceImplementation::executeTimeStepQuery(Query& query)
{
  try
  {
    // Fetching valid producers.

    Producer_vec producers;
    getProducerList(query,producers);

    // Parsing parameters and functions in the query.

    updateQueryParameters(query);

    bool areaQuery = false;
    if ((query.mFlags & QF_AREA_QUERY) != 0)
      areaQuery = true;

    // Fetching parameter data according to the given timesteps and the coordinate list. Notice
    // that the coordinate list can be used in two ways. It can 1) contain coordinate points
    // where the data will be fetched or 2) it can define an area (= polygon) where the grid
    // points are fetched. The area can be defined by using single or multiple polygons.
    // That's why the coordinates are defined as a vector of coordinate vectors.

    for (auto fTime = query.mForecastTimeList.begin(); fTime != query.mForecastTimeList.end(); ++fTime)
    {
      for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
      {
        if (qParam->mFunction.length() == 0)
        {
          std::vector<ParameterInfo> parameterInfoList;

          std::string key = qParam->mParam;
          T::ParamLevelId paramLevelId = qParam->mParameterLevelId;
          T::ParamLevel paramLevel = qParam->mParameterLevel;
          T::ForecastType forecastType = qParam->mForecastType;
          T::ForecastNumber forecastNumber = qParam->mForecastNumber;

          getParameterLevelInfo(qParam->mParam,key,paramLevelId,paramLevel,forecastType,forecastNumber);

          getParameterInfoList(qParam->mParameterKeyType,key,qParam->mParameterLevelIdType,paramLevelId,paramLevel,parameterInfoList);

          ParameterValues valueList;
          try
          {
            getGridValues(producers,parameterInfoList,forecastType,forecastNumber,*fTime,false,query.mCoordinateList,areaQuery,valueList);
          }
          catch (...)
          {
            SmartMet::Spine::Exception exception(BCP, "Operation failed!", NULL);
           exception.printError();
          }

          if (valueList.mValueList.getLength() == 0)
            valueList.mForecastTime = *fTime;

          qParam->mValueList.push_back(valueList);
        }
      }
    }

    // At this point we have fetched values for all data parameters. Now we are able to
    // execute functions that uses this data.

    executeQueryFunctions(query);
    return 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
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
        int res = dataServer->getGridValueByPoint(sessionId,contentInfo->mFileId,contentInfo->mMessageIndex,coordinateType,x,y,interpolationMethod,value);
        if (res == DataServer::Result::OK)
        {
          valueList.addGridPointValue(new T::GridPointValue(contentInfo->mFileId,contentInfo->mMessageIndex,x,y,contentInfo->mParameterLevel,contentInfo->mForecastTime,value));
        }
        else
        {
          valueList.addGridPointValue(new T::GridPointValue(contentInfo->mFileId,contentInfo->mMessageIndex,x,y,contentInfo->mParameterLevel,contentInfo->mForecastTime,ParamValueMissing));
        }
      }
      else
      {
        printf("No data server %llu\n",contentInfo->mServerFlags);
        valueList.addGridPointValue(new T::GridPointValue(contentInfo->mFileId,contentInfo->mMessageIndex,x,y,contentInfo->mParameterLevel,contentInfo->mForecastTime,ParamValueMissing));
      }
    }
    return 0;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}


}
}

