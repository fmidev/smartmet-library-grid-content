#include "ServiceImplementation.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/InterpolationFunctions.h>
#include <grid-files/common/ShowFunction.h>
#include <grid-files/identification/GridDef.h>
#include <macgyver/StringConversion.h>

#define FUNCTION_TRACE FUNCTION_TRACE_OFF

namespace SmartMet
{
namespace QueryServer
{
ServiceImplementation::ServiceImplementation()
{
  FUNCTION_TRACE
  try
  {
    mContentServerPtr = nullptr;
    mDataServerPtr = nullptr;
    mFunctionParamId = 0;
    mProducerFileModificationTime = 0;
    mLastConfiguratonCheck = 0;
    mCheckInterval = 5;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, exception_operation_failed, nullptr);
  }
}

ServiceImplementation::~ServiceImplementation()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, exception_operation_failed, nullptr);
  }
}

void ServiceImplementation::init(ContentServer::ServiceInterface* contentServerPtr,
                                 DataServer::ServiceInterface* dataServerPtr,
                                 std::string gridConfigFile,
                                 string_vec& parameterMappingFiles,
                                 string_vec& aliasFiles,
                                 std::string producerFile,
                                 std::string producerAliasFile,
                                 string_vec& luaFiles)
{
  FUNCTION_TRACE
  try
  {
    mContentServerPtr = contentServerPtr;
    mDataServerPtr = dataServerPtr;

    mGridConfigFile = gridConfigFile;
    SmartMet::Identification::gridDef.init(mGridConfigFile.c_str());

    mProducerFile = producerFile;
    loadProducerFile();

    mProducerAliasFile.init(producerAliasFile, true);

    mAliasFileCollection.init(aliasFiles);
    mLuaFileCollection.init(luaFiles);

    mParameterMappingFiles = parameterMappingFiles;

    for (auto it = mParameterMappingFiles.begin(); it != mParameterMappingFiles.end(); ++it)
    {
      ParameterMappingFile mapping(*it);
      mParameterMappings.push_back(mapping);
    }

    for (auto it = mParameterMappings.begin(); it != mParameterMappings.end(); ++it)
    {
      it->init();
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, exception_operation_failed, nullptr);
  }
}

void ServiceImplementation::shutdown()
{
  FUNCTION_TRACE
  try
  {
    exit(0);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, exception_operation_failed, nullptr);
  }
}

int ServiceImplementation::_executeQuery(T::SessionId sessionId, Query& query)
{
  FUNCTION_TRACE
  try
  {
    // Checking if the configuration files have changed. If so, the
    // changes will be loaded automatically.

    checkConfigurationUpdates();

    // query.print(std::cout,0,0);

    if (mDebugLog != nullptr)
    {
      std::stringstream stream;
      query.print(stream, 0, 0);
      PRINT_DATA(mDebugLog,
                 "\n############################## QUERY EXECUTION START ##################\n\n");
      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
    }

    int result = 0;
    if ((query.mFlags & QF_TIME_RANGE_QUERY) != 0)
      result = executeTimeRangeQuery(query);
    else
      result = executeTimeStepQuery(query);

    if (mDebugLog != nullptr)
    {
      PRINT_DATA(mDebugLog,
                 "\n############################### QUERY EXECUTION END ###################\n\n");
    }

    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

int ServiceImplementation::_getProducerList(T::SessionId sessionId, string_vec& producerList)
{
  FUNCTION_TRACE
  try
  {
    std::string prev;
    for (auto it = mProducerList.begin(); it != mProducerList.end(); ++it)
    {
      if (it->first != prev) producerList.push_back(it->first);

      prev = it->first;
    }
    return QueryServer::Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

void ServiceImplementation::loadProducerFile()
{
  FUNCTION_TRACE
  try
  {
    FILE* file = fopen(mProducerFile.c_str(), "re");
    if (file == nullptr)
    {
      SmartMet::Spine::Exception exception(BCP, "Cannot open the producer file!");
      exception.addParameter("Filename", mProducerFile);
      throw exception;
    }

    mProducerList.clear();

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st, 1000, file) != nullptr && st[0] != '#')
      {
        bool ind = false;
        char* field[100];
        uint c = 1;
        field[0] = st;
        char* p = st;
        while (*p != '\0' && c < 100)
        {
          if (*p == '"') ind = !ind;

          if ((*p == ';' || *p == '\n') && !ind)
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

        if (c >= 2 && field[0][0] != '\0' && field[1][0] != '\0')
        {
          mProducerList.push_back(
              std::pair<std::string, T::GeometryId>(std::string(field[0]), toInt64(field[1])));
        }
      }
    }
    fclose(file);

    mProducerFileModificationTime = getFileModificationTime(mProducerFile.c_str());
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, exception_operation_failed, nullptr);
  }
}

void ServiceImplementation::getGeometryIdListByCoordinates(QueryCoordinates& coordinates,
                                                           std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    std::map<T::GeometryId, uint> countList;
    uint maxCount = 0;

    for (auto cList = coordinates.begin(); cList != coordinates.end(); ++cList)
    {
      for (auto coordinate = cList->begin(); coordinate != cList->end(); ++coordinate)
      {
        std::set<T::GeometryId> idList;
        Identification::gridDef.getGeometryIdListByLatLon(coordinate->y(), coordinate->x(), idList);

        for (auto g = idList.begin(); g != idList.end(); ++g)
        {
          auto c = countList.find(*g);
          if (c != countList.end())
          {
            c->second++;
            if (c->second > maxCount) maxCount = c->second;
          }
          else
          {
            if (maxCount == 0) maxCount = 1;
            countList.insert(std::pair<T::GeometryId, uint>(*g, 1));
          }
        }
      }
    }

    for (auto it = countList.begin(); it != countList.end(); ++it)
    {
      if (it->second == maxCount)
      {
        geometryIdList.insert(it->first);
        // printf("Supported geometry %u\n",it->first);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, exception_operation_failed, nullptr);
  }
}

bool ServiceImplementation::getParameterFunctionInfo(std::string paramStr,
                                                     std::string& function,
                                                     std::string& functionParams)
{
  FUNCTION_TRACE
  try
  {
    char buf[10000];
    char tmp[10000];

    uint c = 0;
    strcpy(buf, paramStr.c_str());
    char* p = buf;

    while (*p != '\0' && *p != '{')
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
            // printf("FUNCTION [%s] [%s]\n",buf,tmp);
            return true;
          }
        }
        else if (*p == '{')
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
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

bool ServiceImplementation::getFunctionParams(std::string functionParamsStr,
                                              FunctionParam_vec& functionParams)
{
  FUNCTION_TRACE
  try
  {
    bool containsFunction = false;
    char buf[10000];
    char tmp[10000];
    uint c = 0;
    strcpy(buf, functionParamsStr.c_str());

    char* p = buf;
    uint level = 0;
    while (*p != '\0')
    {
      if (*p == '{')
      {
        containsFunction = true;
        level++;
      }

      if (*p == '}') level--;

      if (*p == ';' && level == 0)
      {
        tmp[c] = '\0';
        // printf("PARAM [%s]\n",tmp);
        mFunctionParamId++;
        functionParams.push_back(std::pair<uint, std::string>(mFunctionParamId, std::string(tmp)));
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
      // printf("PARAM [%s]\n",tmp);
      mFunctionParamId++;
      functionParams.push_back(std::pair<uint, std::string>(mFunctionParamId, std::string(tmp)));
    }
    return containsFunction;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

void ServiceImplementation::getParameterMappings(std::string producerName,
                                                 std::string parameterName,
                                                 bool onlySearchEnabled,
                                                 ParameterMapping_vec& mappings)
{
  try
  {
    std::string key =
        producerName + ":" + parameterName + ":" + std::to_string(C_INT(onlySearchEnabled));

    AutoThreadLock lock(&mParameterMappingCacheThreadLock);

    for (auto it = mParameterMappingCache.begin(); it != mParameterMappingCache.end(); ++it)
    {
      if (it->first == key)
      {
        mappings = it->second;
        // printf("Cache match %s %s %d
        // %d\n",key.c_str(),it->first.c_str(),mappings.size(),mParameterMappingCache.size());
        return;
      }
    }

    for (auto m = mParameterMappings.begin(); m != mParameterMappings.end(); ++m)
    {
      m->getMappings(producerName, parameterName, onlySearchEnabled, mappings);
    }

    mParameterMappingCache.push_front(std::pair<std::string, ParameterMapping_vec>(key, mappings));
    // printf("Cache add %s %d\n",key.c_str(),mParameterMappingCache.size());
    if (mParameterMappingCache.size() > 20) mParameterMappingCache.pop_back();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

void ServiceImplementation::getParameterMappings(std::string producerName,
                                                 std::string parameterName,
                                                 T::ParamLevelIdType levelIdType,
                                                 T::ParamLevelId levelId,
                                                 T::ParamLevel level,
                                                 bool onlySearchEnabled,
                                                 ParameterMapping_vec& mappings)
{
  try
  {
    std::string key = producerName + ":" + parameterName + ":" + std::to_string(levelIdType) + ":" +
                      std::to_string(levelId) + ":" + std::to_string(level) + ":" +
                      std::to_string(onlySearchEnabled);

    AutoThreadLock lock(&mParameterMappingCacheThreadLock);

    for (auto it = mParameterMappingCache.begin(); it != mParameterMappingCache.end(); ++it)
    {
      if (it->first == key)
      {
        mappings = it->second;
        // printf("Cache match %s %s %d
        // %d\n",key.c_str(),it->first.c_str(),mappings.size(),mParameterMappingCache.size());
        return;
      }
    }

    for (auto m = mParameterMappings.begin(); m != mParameterMappings.end(); ++m)
    {
      m->getMappings(
          producerName, parameterName, levelIdType, levelId, level, onlySearchEnabled, mappings);
    }

    mParameterMappingCache.push_front(std::pair<std::string, ParameterMapping_vec>(key, mappings));
    // printf("Cache add %s %d\n",key.c_str(),mParameterMappingCache.size());

    if (mParameterMappingCache.size() > 20) mParameterMappingCache.pop_back();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

void ServiceImplementation::getParameterStringInfo(std::string param,
                                                   std::string& key,
                                                   T::ParamLevelId& paramLevelId,
                                                   T::ParamLevel& paramLevel,
                                                   T::ForecastType& forecastType,
                                                   T::ForecastNumber& forecastNumber,
                                                   uint& producerId,
                                                   uint& generationFlags,
                                                   short& areaInterpolationMethod,
                                                   short& timeInterpolationMethod,
                                                   short& levelInterpolationMethod)

{
  FUNCTION_TRACE
  try
  {
    std::vector<T::ForecastNumber>forecastNumberVec;
    std::string producerName;
    getParameterStringInfo(param,key,paramLevelId,paramLevel,forecastType,forecastNumberVec,producerName,
        producerId,generationFlags,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod);

    if (forecastNumberVec.size() > 0)
      forecastNumber = forecastNumberVec[0];

  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::getParameterStringInfo(std::string param,
                                                   std::string& key,
                                                   T::ParamLevelId& paramLevelId,
                                                   T::ParamLevel& paramLevel,
                                                   T::ForecastType& forecastType,
                                                   std::vector<T::ForecastNumber>& forecastNumberVec,
                                                   std::string& producerName,
                                                   uint& producerId,
                                                   uint& generationFlags,
                                                   short& areaInterpolationMethod,
                                                   short& timeInterpolationMethod,
                                                   short& levelInterpolationMethod)

{
  FUNCTION_TRACE
  try
  {
    char buf[10000];
    strcpy(buf, param.c_str());

    char* field[100];
    uint c = 1;
    field[0] = buf;
    char* p = buf;
    int level = 0;
    while (*p != '\0' && c < 100)
    {
      if (*p == '{') level++;

      if (*p == '}') level--;

      if (level == 0 && *p == ':')
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

    // for (uint t=0; t<c; t++)
    //  printf("FIELD (%s) %u [%s]\n",buf,t,field[t]);

    key = field[0];

    if (c > 1)
    {
      if (field[1][0] != '\0')
      {
        producerName = field[1];
        T::ProducerInfo producerInfo;
        if (mContentServerPtr->getProducerInfoByName(0, producerName, producerInfo) == 0)
          producerId = producerInfo.mProducerId;
      }
    }

    if (c > 2)
    {
      if (field[2][0] != '\0') paramLevelId = toInt64(field[2]);
    }

    if (c > 3)
    {
      if (field[3][0] != '\0') paramLevel = toInt64(field[3]);
    }

    if (c > 4)
    {
      if (field[4][0] != '\0') forecastType = (T::ForecastType)toInt64(field[4]);
    }

    if (c > 5)
    {
      if (field[5][0] != '\0')
      {
        std::vector<std::string> partList;
        splitString(field[5],'-',partList);
        size_t sz = partList.size();
        if (sz == 1)
        {
          forecastNumberVec.push_back((T::ForecastNumber)toInt64(partList[0].c_str()));
        }
        else
        if (sz == 2)
        {
          auto start = toInt64(partList[0].c_str());
          auto end = toInt64(partList[1].c_str());
          if (start < end  &&  (end-start) <= 200)
          {
            for (auto t=start; t<=end; t++)
              forecastNumberVec.push_back((T::ForecastNumber)t);
          }
        }
      }
    }

    if (c > 6)
    {
      if (field[6][0] != '\0') generationFlags = toInt64(field[6]);
    }

    if (c > 7)
    {
      if (field[7][0] != '\0') areaInterpolationMethod = (short)toInt64(field[7]);
    }

    if (c > 8)
    {
      if (field[8][0] != '\0') timeInterpolationMethod = (short)toInt64(field[8]);
    }

    if (c > 9)
    {
      if (field[9][0] != '\0') levelInterpolationMethod = (short)toInt64(field[9]);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

bool ServiceImplementation::getAlias(std::string name, std::string& alias)
{
  FUNCTION_TRACE
  try
  {
    return mAliasFileCollection.getAlias(name, alias);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

bool ServiceImplementation::parseFunction(QueryParameter& queryParam,
                                          std::string paramStr,
                                          std::string& function,
                                          FunctionParam_vec& functionParams,
                                          uint recursionCounter,
                                          std::vector<QueryParameter>& additionalParameterList)
{
  FUNCTION_TRACE
  try
  {
    if (recursionCounter > 20)
    {
      SmartMet::Spine::Exception exception(BCP, "The recursion is too deep!");
      exception.addDetail("It seems that the function parsing is in a continuous loop.");
      exception.addDetail(
          "The reason for this is most likely that some aliases refer to each other.");
      exception.addParameter("ParamStr", paramStr);
      throw exception;
    }

    PRINT_DATA(mDebugLog,
               "%s   + Parse parameter : %s\n",
               space(recursionCounter).c_str(),
               paramStr.c_str());
    std::string functionParamsStr;
    if (getParameterFunctionInfo(paramStr, function, functionParamsStr))
    {
      PRINT_DATA(
          mDebugLog, "%s     - Function: %s\n", space(recursionCounter).c_str(), function.c_str());
      PRINT_DATA(mDebugLog,
                 "%s     - Params  : %s\n",
                 space(recursionCounter).c_str(),
                 functionParamsStr.c_str());

      getFunctionParams(functionParamsStr, functionParams);

      for (auto fParam = functionParams.begin(); fParam != functionParams.end(); ++fParam)
      {
        if (fParam->second != "0" && toDouble(fParam->second.c_str()) == 0)
        {
          QueryParameter newParam;
          newParam.mId = fParam->first;
          newParam.mParam = fParam->second;
          newParam.mOrigParam = paramStr + " => " + fParam->second;
          newParam.mSymbolicName = fParam->second;
          newParam.mParameterLevelId = queryParam.mParameterLevelId;
          newParam.mParameterLevel = queryParam.mParameterLevel;
          newParam.mForecastType = queryParam.mForecastType;
          newParam.mForecastNumber = queryParam.mForecastNumber;
          newParam.mAreaInterpolationMethod = queryParam.mAreaInterpolationMethod;
          newParam.mProducerId = queryParam.mProducerId;
          newParam.mGenerationFlags = queryParam.mGenerationFlags;
          newParam.mTimestepsBefore = queryParam.mTimestepsBefore;
          newParam.mTimestepsAfter = queryParam.mTimestepsAfter;
          newParam.mTimestepSizeInMinutes = queryParam.mTimestepSizeInMinutes;
          newParam.mTemporary = true;

          std::string alias;
          if (getAlias(fParam->second, alias))
          {
            newParam.mParam = alias;
            newParam.mOrigParam = newParam.mOrigParam + " => " + alias;
            std::string paramName;
            getParameterStringInfo(fParam->second,
                                   paramName,
                                   newParam.mParameterLevelId,
                                   newParam.mParameterLevel,
                                   newParam.mForecastType,
                                   newParam.mForecastNumber,
                                   newParam.mProducerId,
                                   newParam.mGenerationFlags,
                                   newParam.mAreaInterpolationMethod,
                                   newParam.mTimeInterpolationMethod,
                                   newParam.mLevelInterpolationMethod);
          }
          // newParam.print(std::cout,0,0);

          parseFunction(newParam,
                        newParam.mParam,
                        newParam.mFunction,
                        newParam.mFunctionParams,
                        recursionCounter + 2,
                        additionalParameterList);

          additionalParameterList.insert(additionalParameterList.begin(), newParam);
        }
      }
      return true;
    }
    else
    {
      std::vector<T::ForecastNumber> forecastNumberVec;
      std::string producerName;

      std::string paramName;
      getParameterStringInfo(paramStr,
                             paramName,
                             queryParam.mParameterLevelId,
                             queryParam.mParameterLevel,
                             queryParam.mForecastType,
                             forecastNumberVec,
                             producerName,
                             queryParam.mProducerId,
                             queryParam.mGenerationFlags,
                             queryParam.mAreaInterpolationMethod,
                             queryParam.mTimeInterpolationMethod,
                             queryParam.mLevelInterpolationMethod);

      size_t sz = forecastNumberVec.size();
      if (sz == 1)
        queryParam.mForecastNumber = forecastNumberVec[0];
      else
      if (sz > 1)
      {
        char buf[10000];
        char *p = buf;
        p += sprintf(p,"LIST{");
        for (size_t t = 0; t<sz; t++)
        {
          p += sprintf(p,"%s:%s:%d:%d:%d:%d:%u",paramName.c_str(),producerName.c_str(),
              queryParam.mParameterLevelId,queryParam.mParameterLevel,queryParam.mForecastType,forecastNumberVec[t],
              queryParam.mGenerationFlags);

          if (queryParam.mAreaInterpolationMethod >= 0)
            p += sprintf(p,":%d",queryParam.mAreaInterpolationMethod);
          else
            p += sprintf(p,":");

          if (queryParam.mTimeInterpolationMethod >= 0)
            p += sprintf(p,":%d",queryParam.mTimeInterpolationMethod);
          else
            p += sprintf(p,":");

          if (queryParam.mLevelInterpolationMethod >= 0)
            p += sprintf(p,":%d",queryParam.mLevelInterpolationMethod);
          else
            p += sprintf(p,":");

          if ((t+1) < sz)
            p += sprintf(p,";");
        }
        p += sprintf(p,"}");
        queryParam.mParam = buf;

        return parseFunction(queryParam,queryParam.mParam,function,functionParams,recursionCounter+1,additionalParameterList);
      }

      std::string alias;
      if (getAlias(paramName, alias))
      {
        if (alias != paramStr)
        {
          if (!parseFunction(queryParam,
                             alias,
                             function,
                             functionParams,
                             recursionCounter + 1,
                             additionalParameterList))
            queryParam.mParam = alias;
        }
      }
      return false;
    }
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP, "Parsing failed!", nullptr);
    exception.addParameter("ParamStr", paramStr);
    throw exception;
  }
}

void ServiceImplementation::checkConfigurationUpdates()
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if ((time(nullptr) - mLastConfiguratonCheck) > mCheckInterval)
    {
      mLastConfiguratonCheck = time(nullptr);
      time_t t1 = getFileModificationTime(mProducerFile.c_str());

      if (mProducerFileModificationTime != t1 && (mLastConfiguratonCheck - t1) > 3)
        loadProducerFile();

      for (auto it = mParameterMappings.begin(); it != mParameterMappings.end(); ++it)
      {
        if (it->checkUpdates())
        {
          AutoThreadLock lock(&mParameterMappingCacheThreadLock);
          mParameterMappingCache.clear();
        }
      }
    }

    mAliasFileCollection.checkUpdates(false);
    mLuaFileCollection.checkUpdates(false);
    mProducerAliasFile.checkUpdates();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

void ServiceImplementation::getProducers(Query& query, Producer_vec& producers)
{
  FUNCTION_TRACE
  try
  {
    if (query.mProducerNameList.size() == 0)
    {
      producers = mProducerList;
      return;
    }

    for (auto pName = query.mProducerNameList.begin(); pName != query.mProducerNameList.end();
         ++pName)
    {
      for (auto it = mProducerList.begin(); it != mProducerList.end(); ++it)
      {
        if (strcasecmp(pName->c_str(), it->first.c_str()) == 0)
        {
          producers.push_back(std::pair<std::string, T::GeometryId>(it->first, it->second));
        }
        else
        {
          std::string alias;
          if (mProducerAliasFile.getAlias(*pName, alias))
          {
            if (strcasecmp(alias.c_str(), it->first.c_str()) == 0)
            {
              producers.push_back(std::pair<std::string, T::GeometryId>(it->first, it->second));
            }
          }
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

void ServiceImplementation::updateQueryParameters(Query& query)
{
  FUNCTION_TRACE
  try
  {
    bool updateRequired = true;
    uint updateCounter = 0;

    while (updateRequired && updateCounter < 10)
    {
      updateRequired = false;
      updateCounter++;

      PRINT_DATA(mDebugLog, "UPDATE QUERY PARAMETERS\n");

      std::vector<QueryParameter> additionalParameterList;

      // Going through all the query parameters.

      for (auto qParam = query.mQueryParameterList.begin();
           qParam != query.mQueryParameterList.end();
           ++qParam)
      {
        // The parameter can contain some level information even if the parameter is an alias
        // or a function. In this case this level information should be inherited to the alias
        // definition or to the function parameters.

        PRINT_DATA(mDebugLog, " * PARAMETER: %s\n", qParam->mSymbolicName.c_str());

        std::vector<T::ForecastNumber> forecastNumberVec;
        std::string producerName;

        getParameterStringInfo(qParam->mParam,
                               qParam->mParameterKey,
                               qParam->mParameterLevelId,
                               qParam->mParameterLevel,
                               qParam->mForecastType,
                               forecastNumberVec,
                               producerName,
                               qParam->mProducerId,
                               qParam->mGenerationFlags,
                               qParam->mAreaInterpolationMethod,
                               qParam->mTimeInterpolationMethod,
                               qParam->mLevelInterpolationMethod);

        size_t sz = forecastNumberVec.size();
        if (sz == 1)
          qParam->mForecastNumber = forecastNumberVec[0];
        else
        if (sz > 1)
        {
          char buf[10000];
          char *p = buf;
          p += sprintf(p,"LIST{");
          for (size_t t = 0; t<sz; t++)
          {
            p += sprintf(p,"%s:%s:%d:%d:%d:%d:%u",qParam->mParameterKey.c_str(),producerName.c_str(),
                qParam->mParameterLevelId,qParam->mParameterLevel,qParam->mForecastType,forecastNumberVec[t],
                qParam->mGenerationFlags);

            if (qParam->mAreaInterpolationMethod >= 0)
              p += sprintf(p,":%d",qParam->mAreaInterpolationMethod);
            else
              p += sprintf(p,":");

            if (qParam->mTimeInterpolationMethod >= 0)
              p += sprintf(p,":%d",qParam->mTimeInterpolationMethod);
            else
              p += sprintf(p,":");

            if (qParam->mLevelInterpolationMethod >= 0)
              p += sprintf(p,":%d",qParam->mLevelInterpolationMethod);
            else
              p += sprintf(p,":");

            if ((t+1) < sz)
              p += sprintf(p,";");
          }
          p += sprintf(p,"}");
          qParam->mParam = buf;
          //updateRequired = true;
        }

        if (qParam->mParameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID)
        {
          Identification::NewbaseParameterDef paramDef;
          if (Identification::gridDef.getNewbaseParameterDefById(qParam->mParameterKey, paramDef))
          {
            // std::cout << qParam->mSymbolicName << "  => " << alias << " ( " << qParam->mParam <<
            // " )\n";
            PRINT_DATA(mDebugLog, "   + Newbase name: %s\n", paramDef.mParameterName.c_str());

            qParam->mParam = paramDef.mParameterName;
            // qParam->mParameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME;
          }
        }

        // If the parameter is a symbolic name (defined in the configuration file) then
        // we should use its definition.

        std::string alias;
        if (getAlias(qParam->mParameterKey, alias))
        {
          // std::cout << qParam->mSymbolicName << "  => " << alias << " ( " << qParam->mParam << "
          // )\n";
          PRINT_DATA(mDebugLog, "   + Alias: %s\n", alias.c_str());

          qParam->mParam = alias;

          // When an alias is replaced by its definition, it is possible that this definition
          // contains other aliases. That's why we need to repeat this "updateQueryParameters"
          // loop. The 'updateCounter' variable is used in order to ensure that the loop
          // will end if the aliases are referencing to each other (=> eternal loop).

          updateRequired = true;
        }

        // If the parameter contains a function then we should parse it and make sure that
        // we fetch the required parameter values for it.

        if (qParam->mFunction.length() == 0)
        {
          parseFunction(*qParam,
                        qParam->mParam,
                        qParam->mFunction,
                        qParam->mFunctionParams,
                        0,
                        additionalParameterList);
          //updateRequired = true;
        }
      }

      // Adding parameters that are required by functions into the query.

      for (auto p = additionalParameterList.begin(); p != additionalParameterList.end(); ++p)
        query.mQueryParameterList.push_back(*p);

      PRINT_DATA(mDebugLog, "\n");
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

void ServiceImplementation::executeQueryFunctions(Query& query)
{
  FUNCTION_TRACE
  try
  {
    //query.print(std::cout,0,0);
    uint valueCount = query.getValuesPerTimeStep();

    for (auto qParam = query.mQueryParameterList.rbegin();
         qParam != query.mQueryParameterList.rend();
         ++qParam)
    {
      if (qParam->mFunction.length() > 0)
      {
        // std::cout << "*** FUNCTION " << qParam->mFunction << "\n";
        uint tCount = 0;
        for (auto tt = query.mForecastTimeList.begin(); tt != query.mForecastTimeList.end(); ++tt)
        {
          ParameterValues pValues;
          pValues.mForecastTime = *tt;

          std::vector<double> areaParameters;
          std::vector<double> extParameters;

          bool areaCnt = false;
          T::GridValue lastRec;
          for (uint v = 0; v < valueCount; v++)
          {
            std::vector<double> parameters;

            uint producerId = 0;
            uint generationId = 0;
            uint geometryId = 0;

            for (auto it = qParam->mFunctionParams.begin(); it != qParam->mFunctionParams.end();
                 ++it)
            {
              double a = toDouble(it->second.c_str());
              QueryParameter* q = query.getQueryParameterPtr(it->first);
              std::vector<double> valueList;
              if (q != nullptr)
              {
                if (tCount < q->mValueList.size())
                {
                  if (producerId == 0 && q->mValueList[tCount].mProducerId > 0)
                  {
                    producerId = q->mValueList[tCount].mProducerId;
                    generationId = q->mValueList[tCount].mGenerationId;
                    geometryId = q->mValueList[tCount].mGeometryId;
                  }

                  T::GridValue* rec = q->mValueList[tCount].mValueList.getGridValueByIndex(v);
                  if (rec != nullptr)
                  {
                    if (/*qParam->mAreaInterpolationMethod == T::AreaInterpolationMethod::External
                           &&*/
                        rec->mValueString.length() > 0)
                    {
                      splitString(rec->mValueString, ';', valueList);
                    }
                    else
                    {
                      a = rec->mValue;
                    }

                    if (rec->mX == -1000 && rec->mY == -1000)
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
                    // printf("NoValue[%u] %u\n",tCount,v);
                    // q->mValueList[tCount].mValueList.print(std::cout,0,0);
                    a = ParamValueMissing;
                  }
                }
                else
                {
                  areaCnt = true;
                }
              }

              if (valueList.size() == 0)
              {
                //printf("** PUSH %f\n",a);
                parameters.push_back(a);
                areaParameters.push_back(a);

                if (areaCnt && v == 0) extParameters.push_back(a);
              }
              else
              {
                for (auto aa = valueList.begin(); aa != valueList.end(); ++aa)
                {
                  //printf("-- PUSH %f\n",*aa);
                  parameters.push_back(*aa);
                  areaParameters.push_back(*aa);

                  if (areaCnt && v == 0) extParameters.push_back(*aa);
                }
              }
            }

            std::string f = qParam->mFunction.substr(0, 1);

            if (f != "@")
            {
              std::string function;
              uint type = mLuaFileCollection.getFunction(qParam->mFunction, function);
              // printf("CALL %s\n",function.c_str());
              switch (type)
              {
                case 1:
                {
                  double val = mLuaFileCollection.executeFunctionCall1(function, parameters);
                  // printf("-- result %f\n",val);
                  pValues.mValueList.addGridValue(new T::GridValue(lastRec.mX, lastRec.mY, val));
                }
                break;

                case 5:
                {
                  std::string val = mLuaFileCollection.executeFunctionCall5(function, query.mLanguage, parameters);
                  // std::cout << "** result " << val << "\n";
                  pValues.mValueList.addGridValue(new T::GridValue(lastRec.mX, lastRec.mY, val));
                }
                break;
              }

              pValues.mProducerId = producerId;
              pValues.mGenerationId = generationId;
              pValues.mGeometryId = geometryId;
            }
          }

          std::string f = qParam->mFunction.substr(0, 1);
          if (areaCnt && f != "@")
          {
            // double val = executeFunctionCall1(qParam->mFunction,extParameters);
            double val = mLuaFileCollection.executeFunctionCall1(qParam->mFunction, extParameters);
            pValues.mValueList.addGridValue(new T::GridValue(-1000, -1000, val));
          }

          if (qParam->mFunction.substr(0, 1) == "@")
          {
            // T::ParamValue val = executeAreaFunction(qParam->mFunction,areaParameters);
            std::string func = qParam->mFunction.substr(1);
            double val = mLuaFileCollection.executeFunctionCall1(func, areaParameters);
            // double val = executeFunctionCall1(func,areaParameters);
            pValues.mValueList.addGridValue(new T::GridValue(-1000, -1000, val));
          }

          if (areaCnt)
          {
            if (pValues.mValueList.getLength() > 0)
            {
              T::GridValue* rec = pValues.mValueList.getGridValueByIndex(0);
              T::ParamValue val = rec->mValue;
              pValues.mValueList.clear();
              pValues.mValueList.addGridValue(new T::GridValue(-1000, -1000, val));
            }
          }
          qParam->mValueList.push_back(pValues);

          tCount++;
        }
      }
    }

    if (mDebugLog != nullptr)
    {
      std::stringstream stream;
      query.print(stream, 0, 0);
      PRINT_DATA(mDebugLog, "\nQuery -object after function execution:\n");
      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
    }

    // query.print(std::cout,0,0);

    query.removeTemporaryParameters();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

int ServiceImplementation::executeTimeRangeQuery(Query& query)
{
  FUNCTION_TRACE
  try
  {
    // Fetching valid producers.

    Producer_vec producers;
    getProducers(query, producers);

    if (producers.size() == 0) return Result::NO_PRODUCERS_FOUND;

    // Getting geometries that support support the given coordinates.

    std::set<T::GeometryId> geometryIdList;
    getGeometryIdListByCoordinates(query.mCoordinateList, geometryIdList);

    std::map<std::string, uint> parameterProducers;

    // Parsing parameters and functions in the query.

    updateQueryParameters(query);

    if (mDebugLog != nullptr)
    {
      std::stringstream stream;
      query.print(stream, 0, 0);
      PRINT_DATA(mDebugLog, "\nQuery -object after parameter update:\n");
      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
    }

    bool ignoreStartTimeValue = false;
    if ((query.mFlags & QF_START_TIME_NOT_INCLUDED) != 0) ignoreStartTimeValue = true;

    bool reverseGenerations = false;
    if ((query.mFlags & QF_REVERSE_GENERATION_FLAGS) != 0) reverseGenerations = true;

    // Fetching parameter data according to the given time range and the coordinate list. Notice
    // that the coordinate list can be used in two ways. It can 1) contain coordinate points
    // where the data will be fetched or 2) it can define an area (= polygon) where the grid
    // points are fetched. The area can be defined by using single or multiple polygons.
    // That's why the coordinates are defined as a vector of coordinate vectors.

    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end();
         ++qParam)
    {
      // Checking that the parameter is "a data parameter" - not a function.

      if (qParam->mFunction.length() == 0)
      {
        std::string paramName = qParam->mParam;
        T::ParamLevelId paramLevelId = qParam->mParameterLevelId;
        T::ParamLevel paramLevel = qParam->mParameterLevel;
        T::ForecastType forecastType = qParam->mForecastType;
        T::ForecastNumber forecastNumber = qParam->mForecastNumber;
        short areaInterpolationMethod = qParam->mAreaInterpolationMethod;
        short timeInterpolationMethod = qParam->mTimeInterpolationMethod;
        short levelInterpolationMethod = qParam->mLevelInterpolationMethod;
        uint parameterFlags = qParam->mFlags;
        uint producerId = qParam->mProducerId;
        uint generationFlags = qParam->mGenerationFlags;
        if (generationFlags == 0) generationFlags = query.mGenerationFlags;

        // qParam->print(std::cout,0,0);

        getParameterStringInfo(qParam->mParam,
                               paramName,
                               paramLevelId,
                               paramLevel,
                               forecastType,
                               forecastNumber,
                               producerId,
                               generationFlags,
                               areaInterpolationMethod,
                               timeInterpolationMethod,
                               levelInterpolationMethod);

        // printf("**** PARAM %s  => %d %d\n",qParam->mParam.c_str(),paramLevelId,paramLevel);

        if (paramName.substr(0, 1) == "$") paramName = paramName.substr(1);

        try
        {
          if (qParam->mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN)
          {
            std::string startTime = query.mStartTime;
            std::string endTime = query.mEndTime;

            if (qParam->mTimestepsBefore > 0)
            {
              auto ss = boost::posix_time::from_iso_string(startTime) -
                        boost::posix_time::seconds(
                            ((qParam->mTimestepsBefore + 1) * qParam->mTimestepSizeInMinutes * 60));
              startTime = Fmi::to_iso_string(ss);

              // time_t ss = utcTimeToTimeT(startTime) - ((qParam->mTimestepsBefore+1) *
              // qParam->mTimestepSizeInMinutes * 60); startTime = utcTimeFromTimeT(ss);
            }

            if (qParam->mTimestepsAfter > 0)
            {
              auto ss = boost::posix_time::from_iso_string(endTime) -
                        boost::posix_time::seconds(
                            (qParam->mTimestepsAfter * qParam->mTimestepSizeInMinutes * 60));
              endTime = Fmi::to_iso_string(ss);

              // time_t ss = utcTimeToTimeT(endTime) + (qParam->mTimestepsAfter *
              // qParam->mTimestepSizeInMinutes * 60); endTime = utcTimeFromTimeT(ss);
            }

            if ((query.mFlags & QF_START_TIME_DATA) != 0)
            {
              startTime = "15000101T000000";  // Start time is the start time of the data
              endTime = "30000101T000000";    // End time is the end time of the data
            }

            if (producerId == 0)
            {
              auto it = parameterProducers.find(paramName);
              if (it != parameterProducers.end()) producerId = it->second;
            }

            getGridValues(producers,
                          geometryIdList,
                          producerId,
                          query.mAnalysisTime,
                          generationFlags,
                          reverseGenerations,
                          paramName,
                          paramLevelId,
                          paramLevel,
                          forecastType,
                          forecastNumber,
                          parameterFlags,
                          areaInterpolationMethod,
                          timeInterpolationMethod,
                          levelInterpolationMethod,
                          startTime,
                          endTime,
                          ignoreStartTimeValue,
                          query.mSearchType,
                          query.mCoordinateList,
                          query.mRadius,
                          query.mDem,
                          query.mCoverType,
                          query.mMaxParameterValues,
                          qParam->mValueList);

            if (producerId == 0 && qParam->mValueList.size() > 0 &&
                qParam->mValueList[0].mProducerId != 0)
            {
              parameterProducers.insert(
                  std::pair<std::string, uint>(paramName, qParam->mValueList[0].mProducerId));
            }
          }
        }
        catch (...)
        {
          SmartMet::Spine::Exception exception(BCP, "Operation failed!", nullptr);
          exception.printError();
        }
      }
    }

    // Finding out which forecast time are found from the forecast data. The point is that different
    // parameters might contain different forecast times, and we want a list of all forecast times.

    std::set<std::string> timeList;
    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end();
         ++qParam)
    {
      for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end(); ++it)
      {
        if (timeList.find(it->mForecastTime) == timeList.end()) timeList.insert(it->mForecastTime);
      }
    }

    // Going through all the found forecast times and making sure that each parameter contains the
    // current forecast time. If not, then the forecast time is added to the parameters's forecast
    // time list, but the actual value list of the current forecast time will be empty.

    for (auto tt = timeList.begin(); tt != timeList.end(); ++tt)
    {
      query.mForecastTimeList.insert(*tt);

      for (auto qParam = query.mQueryParameterList.begin();
           qParam != query.mQueryParameterList.end();
           ++qParam)
      {
        if (qParam->mFunction.length() == 0)
        {
          bool found = false;
          uint cnt = 0;
          for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end() && !found; ++it)
          {
            if ((query.mFlags & QF_START_TIME_DATA) == 0 &&
                ((ignoreStartTimeValue && it->mForecastTime == query.mStartTime) ||
                 it->mForecastTime < query.mStartTime || it->mForecastTime > query.mEndTime))
              it->mFlags = it->mFlags | QueryServer::QPF_AGGREGATION_VALUE;

            if (it->mForecastTime < *tt)
              cnt++;
            else if (it->mForecastTime == *tt)
              found = true;
          }

          if (!found)
          {
            // The forecast time was not found from the current parameter. Adding the forecast time
            // with an empty value list.

            ParameterValues pValues;
            pValues.mForecastTime = *tt;
            pValues.mFlags = pValues.mFlags | QueryServer::QPF_ADDITIONAL_VALUE;

            if ((query.mFlags & QF_START_TIME_DATA) == 0 &&
                (*tt < query.mStartTime || *tt > query.mEndTime))
              pValues.mFlags = pValues.mFlags | QueryServer::QPF_AGGREGATION_VALUE;

            qParam->mValueList.insert(qParam->mValueList.begin() + cnt, pValues);
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
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

int ServiceImplementation::executeTimeStepQuery(Query& query)
{
  FUNCTION_TRACE
  try
  {
    // Fetching valid producers.

    Producer_vec producers;
    getProducers(query, producers);

    if (producers.size() == 0) return Result::NO_PRODUCERS_FOUND;

    std::set<T::GeometryId> geometryIdList;
    getGeometryIdListByCoordinates(query.mCoordinateList, geometryIdList);

    std::map<std::string, uint> parameterProducers;

    // Parsing parameters and functions in the query.

    updateQueryParameters(query);

    if (mDebugLog != nullptr)
    {
      std::stringstream stream;
      query.print(stream, 0, 0);
      PRINT_DATA(mDebugLog, "\nQuery -object after parameter update:\n");
      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
    }

    bool reverseGenerations = false;
    if ((query.mFlags & QF_REVERSE_GENERATION_FLAGS) != 0) reverseGenerations = true;

    // Fetching parameter data according to the given timesteps and the coordinate list. Notice
    // that the coordinate list can be used in two ways. It can 1) contain coordinate points
    // where the data will be fetched or 2) it can define an area (= polygon) where the grid
    // points are fetched. The area can be defined by using single or multiple polygons.
    // That's why the coordinates are defined as a vector of coordinate vectors.

    std::set<std::string> timeList;
    for (auto fTime = query.mForecastTimeList.begin(); fTime != query.mForecastTimeList.end();
         ++fTime)
    {
      timeList.insert(*fTime);
    }

    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end();
         ++qParam)
    {
      if (qParam->mFunction.length() == 0)
      {
        std::string paramName = qParam->mParam;
        T::ParamLevelId paramLevelId = qParam->mParameterLevelId;
        T::ParamLevel paramLevel = qParam->mParameterLevel;
        T::ForecastType forecastType = qParam->mForecastType;
        T::ForecastNumber forecastNumber = qParam->mForecastNumber;
        short areaInterpolationMethod = qParam->mAreaInterpolationMethod;
        short timeInterpolationMethod = qParam->mTimeInterpolationMethod;
        short levelInterpolationMethod = qParam->mLevelInterpolationMethod;
        uint parameterFlags = qParam->mFlags;
        uint producerId = qParam->mProducerId;
        uint generationFlags = qParam->mGenerationFlags;
        if (generationFlags == 0) generationFlags = query.mGenerationFlags;

        getParameterStringInfo(qParam->mParam,
                               paramName,
                               paramLevelId,
                               paramLevel,
                               forecastType,
                               forecastNumber,
                               producerId,
                               generationFlags,
                               areaInterpolationMethod,
                               timeInterpolationMethod,
                               levelInterpolationMethod);

        if (paramName.substr(0, 1) == "$") paramName = paramName.substr(1);

        std::set<std::string> forecastTimeList = timeList;
        std::set<std::string> additionalTimeList;

        if (qParam->mTimestepsBefore > 0 || qParam->mTimestepsAfter > 0)
        {
          for (auto fTime = timeList.begin(); fTime != timeList.end(); ++fTime)
          {
            if (qParam->mTimestepsBefore > 0)
            {
              // time_t ss = utcTimeToTimeT(*fTime) - ((qParam->mTimestepsBefore+1) *
              // qParam->mTimestepSizeInMinutes * 60);
              auto ss = boost::posix_time::from_iso_string(*fTime) -
                        boost::posix_time::seconds(
                            ((qParam->mTimestepsBefore + 1) * qParam->mTimestepSizeInMinutes * 60));

              for (uint t = 0; t < qParam->mTimestepsBefore; t++)
              {
                ss = ss + boost::posix_time::seconds(qParam->mTimestepSizeInMinutes * 60);
                std::string str = Fmi::to_iso_string(ss);
                // std::string str = utcTimeFromTimeT(ss);
                if (forecastTimeList.find(str) == forecastTimeList.end())
                {
                  additionalTimeList.insert(str);
                  forecastTimeList.insert(str);
                }
              }
            }

            if (qParam->mTimestepsAfter > 0)
            {
              // time_t ss = utcTimeToTimeT(*fTime);
              auto ss = boost::posix_time::from_iso_string(*fTime);

              for (uint t = 0; t < qParam->mTimestepsAfter; t++)
              {
                ss = ss + boost::posix_time::seconds(qParam->mTimestepSizeInMinutes * 60);
                // std::string str = utcTimeFromTimeT(ss);
                std::string str = Fmi::to_iso_string(ss);
                if (forecastTimeList.find(str) == forecastTimeList.end())
                {
                  additionalTimeList.insert(str);
                  forecastTimeList.insert(str);
                }
              }
            }
          }
        }

        for (auto fTime = forecastTimeList.begin(); fTime != forecastTimeList.end(); ++fTime)
        {
          ParameterValues valueList;
          try
          {
            if (producerId == 0)
            {
              auto it = parameterProducers.find(paramName);
              if (it != parameterProducers.end()) producerId = it->second;
            }

            if (qParam->mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN)
            {
              getGridValues(producers,
                            geometryIdList,
                            producerId,
                            query.mAnalysisTime,
                            generationFlags,
                            reverseGenerations,
                            paramName,
                            paramLevelId,
                            paramLevel,
                            forecastType,
                            forecastNumber,
                            parameterFlags,
                            areaInterpolationMethod,
                            timeInterpolationMethod,
                            levelInterpolationMethod,
                            *fTime,
                            false,
                            query.mSearchType,
                            query.mCoordinateList,
                            query.mRadius,
                            query.mDem,
                            query.mCoverType,
                            valueList);

              if (producerId == 0 && valueList.mProducerId != 0)
              {
                parameterProducers.insert(
                    std::pair<std::string, uint>(paramName, valueList.mProducerId));
              }
            }
          }
          catch (...)
          {
            SmartMet::Spine::Exception exception(BCP, "Operation failed!", nullptr);
            exception.printError();
          }

          if (valueList.mValueList.getLength() == 0) valueList.mForecastTime = *fTime;

          if (additionalTimeList.find(*fTime) != additionalTimeList.end())
            valueList.mFlags = valueList.mFlags | QueryServer::QPF_AGGREGATION_VALUE;

          qParam->mValueList.push_back(valueList);
        }
      }
    }

    // Finding out which forecast time are found from the forecast data. The point is that different
    // parameters might contain different forecast times, and we want a list of all forecast times.

    timeList.clear();
    std::set<std::string> additionalTimeList;

    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end();
         ++qParam)
    {
      for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end(); ++it)
      {
        if (timeList.find(it->mForecastTime) == timeList.end()) timeList.insert(it->mForecastTime);

        if ((it->mFlags & QueryServer::QPF_AGGREGATION_VALUE) != 0 &&
            additionalTimeList.find(it->mForecastTime) == additionalTimeList.end())
          additionalTimeList.insert(it->mForecastTime);
      }
    }

    // Going through all the found forecast times and making sure that each parameter contains the
    // current forecast time. If not, then the forecast time is added to the parameters's forecast
    // time list, but the actual value list of the current forecast time will be empty.

    query.mForecastTimeList.clear();

    for (auto tt = timeList.begin(); tt != timeList.end(); ++tt)
    {
      query.mForecastTimeList.insert(*tt);

      for (auto qParam = query.mQueryParameterList.begin();
           qParam != query.mQueryParameterList.end();
           ++qParam)
      {
        if (qParam->mFunction.length() == 0)
        {
          bool found = false;
          uint cnt = 0;
          for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end() && !found; ++it)
          {
            if (it->mForecastTime < *tt)
              cnt++;
            else if (it->mForecastTime == *tt)
              found = true;
          }

          if (!found)
          {
            // The forecast time was not found from the current parameter. Adding the forecast time
            // with an empty value list.

            ParameterValues pValues;
            pValues.mForecastTime = *tt;
            pValues.mFlags = pValues.mFlags | QueryServer::QPF_ADDITIONAL_VALUE;
            if (additionalTimeList.find(*tt) != additionalTimeList.end())
              pValues.mFlags = pValues.mFlags | QueryServer::QPF_AGGREGATION_VALUE;

            qParam->mValueList.insert(qParam->mValueList.begin() + cnt, pValues);
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
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

int ServiceImplementation::_getValuesByGridPoint(T::SessionId sessionId,
                                                 T::ContentInfoList& contentInfoList,
                                                 T::CoordinateType coordinateType,
                                                 double x,
                                                 double y,
                                                 short areaInterpolationMethod,
                                                 T::GridPointValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    uint flags = 0;
    uint len = contentInfoList.getLength();
    for (uint c = 0; c < len; c++)
    {
      T::ContentInfo* contentInfo = contentInfoList.getContentInfoByIndex(c);
      T::ParamValue value = 0;
      int res = mDataServerPtr->getGridValueByPoint(sessionId,
                                                    contentInfo->mFileId,
                                                    contentInfo->mMessageIndex,
                                                    flags,
                                                    coordinateType,
                                                    x,
                                                    y,
                                                    areaInterpolationMethod,
                                                    value);
      if (res == DataServer::Result::OK)
      {
        valueList.addGridPointValue(new T::GridPointValue(contentInfo->mFileId,
                                                          contentInfo->mMessageIndex,
                                                          x,
                                                          y,
                                                          contentInfo->mParameterLevel,
                                                          contentInfo->mForecastTime,
                                                          value));
      }
      else
      {
        valueList.addGridPointValue(new T::GridPointValue(contentInfo->mFileId,
                                                          contentInfo->mMessageIndex,
                                                          x,
                                                          y,
                                                          contentInfo->mParameterLevel,
                                                          contentInfo->mForecastTime,
                                                          ParamValueMissing));
      }
    }
    return 0;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, exception_operation_failed, nullptr);
  }
}

bool ServiceImplementation::conversionFunction(std::string& conversionFunction,
                                               std::string& function,
                                               std::vector<std::string>& functionParams)
{
  try
  {
    if (conversionFunction.length() > 0)
    {
      std::string functionParamsStr;
      if (getParameterFunctionInfo(conversionFunction, function, functionParamsStr))
      {
        splitString(functionParamsStr, ',', functionParams);
        return true;
      }
    }
    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

void ServiceImplementation::executeConversion(std::string& function,
                                              std::vector<std::string>& functionParams,
                                              T::GridValueList& valueList)
{
  try
  {
    uint vLen = valueList.getLength();
    for (uint i = 0; i < vLen; i++)
    {
      T::GridValue* gv = valueList.getGridValueByIndex(i);
      if (gv != nullptr)
      {
        std::vector<double> parameters;
        for (auto fp = functionParams.begin(); fp != functionParams.end(); ++fp)
        {
          if (*fp == "$")
            parameters.push_back(gv->mValue);
          else
            parameters.push_back(toDouble(fp->c_str()));
        }
        gv->mValue = mLuaFileCollection.executeFunctionCall1(function, parameters);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

void ServiceImplementation::timeInterpolation(short timeInterpolationMethod,
                                              std::string forecastTimeStr,
                                              std::string forecastTimeStr1,
                                              std::string forecastTimeStr2,
                                              T::GridValueList& list1,
                                              T::GridValueList& list2,
                                              T::GridValueList& valueList)
{
  try
  {
    boost::posix_time::ptime forecastTime = toTimeStamp(forecastTimeStr);
    boost::posix_time::ptime forecastTime1 = toTimeStamp(forecastTimeStr1);
    boost::posix_time::ptime forecastTime2 = toTimeStamp(forecastTimeStr2);

    uint len1 = list1.getLength();
    uint len2 = list2.getLength();

    if (len1 == len2)
    {
      switch (timeInterpolationMethod)
      {
        default:
        case T::TimeInterpolationMethod::Linear:
        {
          time_t diff = toTimeT(forecastTime) - toTimeT(forecastTime1);
          time_t ttDiff = toTimeT(forecastTime2) - toTimeT(forecastTime1);

          for (uint t = 0; t < len1; t++)
          {
            T::GridValue* val1 = list1.getGridValueByIndex(t);
            T::GridValue* val2 = list2.getGridValueByIndex(t);

            T::ParamValue valueDiff = val2->mValue - val1->mValue;
            T::ParamValue valueStep = valueDiff / ttDiff;

            T::GridValue* rec = new T::GridValue();
            rec->mX = val1->mX;
            rec->mY = val1->mY;
            rec->mValue = val1->mValue + (diff * valueStep);
            valueList.addGridValue(rec);
          }
        }
        break;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

void ServiceImplementation::levelInterpolation(short levelInterpolationMethod,
                                               double level,
                                               double level1,
                                               double level2,
                                               ParameterValues& parameterValues1,
                                               ParameterValues& parameterValues2,
                                               ParameterValues& valueList)
{
  try
  {
    // std::cout << "LEVEL INTERPOLATION " << level1 << " " << level << " " << level2 << "   METHOD
    // " << levelInterpolationMethod << "\n";

    // parameterValues1.print(std::cout,0,0);
    // parameterValues2.print(std::cout,0,0);

    if (level == level1)
    {
      valueList = parameterValues1;
      return;
    }

    if (level == level2)
    {
      valueList = parameterValues2;
      return;
    }

    if (parameterValues1.mValueList.getLength() != parameterValues2.mValueList.getLength())
    {
      std::cout << CODE_LOCATION << ": Lengths do not match\n";
      return;
    }

    valueList.mForecastTime = parameterValues1.mForecastTime;
    valueList.mProducerId = parameterValues1.mProducerId;
    valueList.mGenerationId = parameterValues1.mGenerationId;
    valueList.mGeometryId = parameterValues1.mGeometryId;

    valueList.mParameterKeyType = parameterValues1.mParameterKeyType;
    valueList.mParameterKey = parameterValues1.mParameterKey;
    valueList.mParameterLevelIdType = parameterValues1.mParameterLevelIdType;
    valueList.mParameterLevelId = parameterValues1.mParameterLevelId;
    valueList.mParameterLevel = level;
    valueList.mForecastType = parameterValues1.mForecastType;
    valueList.mForecastNumber = parameterValues1.mForecastNumber;
    valueList.mFlags = parameterValues1.mFlags;

    double dist1 = level - level1;
    double dist2 = level2 - level;

    uint len = parameterValues1.mValueList.getLength();
    for (uint t = 0; t < len; t++)
    {
      T::GridValue* val1 = parameterValues1.mValueList.getGridValueByIndex(t);
      T::GridValue* val2 = parameterValues2.mValueList.getGridValueByIndex(t);

      if (val1->mX != val2->mX || val1->mY != val1->mY)
      {
        std::cout << CODE_LOCATION << " : Coordinates do not match\n";
      }

      switch (levelInterpolationMethod)
      {
        case T::LevelInterpolationMethod::Nearest:
        {
          if (dist1 <= dist2)
            valueList.mValueList.addGridValue(new T::GridValue(val1->mX, val1->mX, val1->mValue));
          else
            valueList.mValueList.addGridValue(new T::GridValue(val2->mX, val2->mX, val2->mValue));
        }
        break;

        case T::LevelInterpolationMethod::Linear:
        {
          double newValue = linearInterpolation(level, level1, level2, val1->mValue, val2->mValue);
          valueList.mValueList.addGridValue(new T::GridValue(val1->mX, val1->mX, newValue));
        }
        break;

        default:
        case T::LevelInterpolationMethod::Logarithmic:
        {
          double newValue =
              logarithmicInterpolation(level, level1, level2, val1->mValue, val2->mValue);
          valueList.mValueList.addGridValue(new T::GridValue(val1->mX, val1->mX, newValue));
        }
        break;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

bool ServiceImplementation::getSpecialValues(T::ProducerInfo& producerInfo,
                                             T::GeometryId producerGeometryId,
                                             uint& generationId,
                                             ParameterMapping& pInfo,
                                             std::string forecastTime,
                                             T::ParamLevelId paramLevelId,
                                             T::ParamLevel paramLevel,
                                             T::ForecastType forecastType,
                                             T::ForecastNumber forecastNumber,
                                             uint parameterFlags,
                                             short areaInterpolationMethod,
                                             short timeInterpolationMethod,
                                             short levelInterpolationMethod,
                                             double x,
                                             double y,
                                             uint& newProducerId,
                                             ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    std::string startTime = forecastTime;
    std::string endTime = forecastTime;
    uint flags = 0;

    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction =
        conversionFunction(pInfo.mConversionFunction, function, functionParams);

    /*
    if (paramLevelId > 0 || paramLevel != 0)
    {
      pInfo.mParameterLevelId = paramLevelId;
      pInfo.mParameterLevel = paramLevel;
    }

    PRINT_DATA(mDebugLog,"         + %s:%d:%d:%d:%d\n",pInfo.mParameterKey.c_str(),
      parameterLevelId,parameterLevel,
      forecastType,forecastNumber);
    */

    T::ContentInfoList contentList;
    int result = mContentServerPtr->getContentListByParameterGenerationIdAndForecastTime(
        0,
        generationId,
        pInfo.mParameterKeyType,
        pInfo.mParameterKey,
        pInfo.mParameterLevelIdType,
        paramLevelId,
        paramLevel,
        forecastType,
        forecastNumber,
        producerGeometryId,
        forecastTime,
        contentList);
    if (result != 0)
    {
      SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
      exception.addParameter("Service", "getContentListByParameterGenerationIdAndForecastTime");
      exception.addParameter("Message", ContentServer::getResultString(result));
      throw exception;
    }

    PRINT_DATA(mDebugLog, "         + Found %u content records\n", contentList.getLength());

    if (mDebugLog != nullptr)
    {
      std::stringstream stream;
      contentList.print(stream, 0, 4);
      PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
    }

    uint contentLen = contentList.getLength();
    bool multipleValues = contentList.containsSameForecastTimes();
    if (multipleValues)
    {
      PRINT_DATA(mDebugLog,
                 "         + Content records contains multiple values with the same timestep\n");
      return false;
    }

    if (contentLen == 0) return false;

    // We found content information close to the current forecast time

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelIdType = pInfo.mParameterLevelIdType;
    valueList.mParameterLevelId = paramLevelId;

    if (contentLen == 1)
    {
      T::ContentInfo* contentInfo = contentList.getContentInfoByIndex(0);
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
        if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
          valueList.mParameterLevelId = contentInfo->mFmiParameterLevelId;
        else
          valueList.mParameterLevelId = pInfo.mParameterLevelId;

        double_vec valueVector;
        int result =
            mDataServerPtr->getGridValueVectorByPoint(0,
                                                      contentInfo->mFileId,
                                                      contentInfo->mMessageIndex,
                                                      flags,
                                                      T::CoordinateTypeValue::LATLON_COORDINATES,
                                                      x,
                                                      y,
                                                      areaInterpolationMethod,
                                                      valueVector);
        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueVectorByPoint");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          // throw exception;
        }

        uint vSize = valueVector.size();
        if (vSize > 0)
        {
          // The first number indicates the number of the constant values
          uint sLen = valueVector[0];

          char tmp[1000];
          tmp[0] = '\0';
          char* p = tmp;

          T::GridValueList list;
          for (uint vv = sLen + 1; vv < vSize; vv++)
            list.addGridValue(new T::GridValue(x, y, valueVector[vv]));

          if (conversionByFunction) executeConversion(function, functionParams, list);

          for (uint vv = 0; vv <= sLen; vv++)
            p += sprintf(p, "%f;", valueVector[vv]);

          uint len = list.getLength();
          for (uint vv = 0; vv < len; vv++)
          {
            T::GridValue* rec = list.getGridValueByIndex(vv);
            p += sprintf(p, "%f;", rec->mValue);
          }

          valueList.mValueList.addGridValue(new T::GridValue(x, y, std::string(tmp)));
        }

        return true;
      }
      else
      {
        // There is one content record in place, but its time does not match to
        // the requested forecast time. This is used for indicating that there
        // are content records available, but not for the requested time.
        // So, we should use this producer.

        newProducerId = producerInfo.mProducerId;
        valueList.mProducerId = producerInfo.mProducerId;
        return false;
      }
    }

    if (contentLen == 2)
    {
      T::ContentInfo* contentInfo1 = contentList.getContentInfoByIndex(0);
      T::ContentInfo* contentInfo2 = contentList.getContentInfoByIndex(1);
      if (!(contentInfo1->mForecastTime < forecastTime &&
            contentInfo2->mForecastTime > forecastTime))
      {
        SmartMet::Spine::Exception exception(BCP, "Unexpected result!");
        exception.addDetail("The given forecast time should been between the found content times.");
        exception.addParameter("Content 1 ForecastTime", contentInfo1->mForecastTime);
        exception.addParameter("Content 2 ForecastTime", contentInfo2->mForecastTime);
        exception.addParameter("Request ForecastTime", forecastTime);
        throw exception;
      }

      // We did not find a grid with the exact forecast time, but we find grids that
      // are before and after the current forecast time. This means that we should do
      // some time interpolation.

      valueList.mForecastTime = forecastTime;
      valueList.mProducerId = contentInfo1->mProducerId;
      valueList.mGenerationId = contentInfo1->mGenerationId;
      valueList.mGeometryId = contentInfo1->mGeometryId;
      valueList.mForecastType = contentInfo1->mForecastType;
      valueList.mForecastNumber = contentInfo1->mForecastNumber;
      valueList.mParameterLevel = contentInfo1->mParameterLevel;
      if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
        valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;
      else
        valueList.mParameterLevelId = pInfo.mParameterLevelId;

      // Fetching data from the grids.

      double_vec valueVector1;
      double_vec valueVector2;

      int result1 =
          mDataServerPtr->getGridValueVectorByPoint(0,
                                                    contentInfo1->mFileId,
                                                    contentInfo1->mMessageIndex,
                                                    flags,
                                                    T::CoordinateTypeValue::LATLON_COORDINATES,
                                                    x,
                                                    y,
                                                    areaInterpolationMethod,
                                                    valueVector1);
      if (result1 != 0)
      {
        SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
        exception.addParameter("Service", "getGridValueBlockByPoint");
        exception.addParameter("Message", DataServer::getResultString(result1));
        std::string errorMsg = exception.getStackTrace();
        PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
        // throw exception;
      }

      int result2 =
          mDataServerPtr->getGridValueVectorByPoint(0,
                                                    contentInfo2->mFileId,
                                                    contentInfo2->mMessageIndex,
                                                    flags,
                                                    T::CoordinateTypeValue::LATLON_COORDINATES,
                                                    x,
                                                    y,
                                                    areaInterpolationMethod,
                                                    valueVector2);
      if (result2 != 0)
      {
        SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
        exception.addParameter("Service", "getGridValueVectorByPoint");
        exception.addParameter("Message", DataServer::getResultString(result2));
        std::string errorMsg = exception.getStackTrace();
        PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
        // throw exception;
      }

      char tmp[1000];
      tmp[0] = '\0';
      char* p = tmp;

      uint vLen1 = valueVector1.size();
      uint vLen2 = valueVector2.size();

      if (vLen1 == vLen2 && vLen1 > 1)
      {
        // The first number indicates the number of the constant values
        uint sLen = valueVector1[0];

        T::GridValueList list1;
        T::GridValueList list2;
        for (uint vv = sLen + 1; vv < vLen1; vv++)
        {
          list1.addGridValue(new T::GridValue(x, y, valueVector1[vv]));
          list2.addGridValue(new T::GridValue(x, y, valueVector2[vv]));
        }

        T::GridValueList list;
        timeInterpolation(timeInterpolationMethod,
                          forecastTime,
                          contentInfo1->mForecastTime,
                          contentInfo2->mForecastTime,
                          list1,
                          list2,
                          list);

        if (conversionByFunction) executeConversion(function, functionParams, list);

        for (uint s = 0; s <= sLen; s++)
          p += sprintf(p, "%f;", valueVector1[s]);

        uint len = list.getLength();
        for (uint s = 0; s < len; s++)
        {
          T::GridValue* rec = list.getGridValueByIndex(s);
          p += sprintf(p, "%f;", rec->mValue);
        }

        valueList.mValueList.addGridValue(new T::GridValue(x, y, std::string(tmp)));
        return true;
      }
    }

    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

bool ServiceImplementation::getPointValues(T::ProducerInfo& producerInfo,
                                           T::GeometryId producerGeometryId,
                                           uint& generationId,
                                           ParameterMapping& pInfo,
                                           std::string forecastTime,
                                           T::ParamLevelId paramLevelId,
                                           T::ParamLevel paramLevel,
                                           T::ForecastType forecastType,
                                           T::ForecastNumber forecastNumber,
                                           uint parameterFlags,
                                           short areaInterpolationMethod,
                                           short timeInterpolationMethod,
                                           short levelInterpolationMethod,
                                           QueryCoordinates& coordinates,
                                           uint& newProducerId,
                                           ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    bool climatologyParam = false;
    if ((pInfo.mGroupFlags & QueryServer::ParameterMapping::GroupFlags::climatology) != 0)
      climatologyParam = true;

    std::string startTime = forecastTime;
    std::string endTime = forecastTime;

    uint flags = 0;

    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction =
        conversionFunction(pInfo.mConversionFunction, function, functionParams);

    std::string fTime = forecastTime;

    T::ContentInfoList contentList;
    if (climatologyParam)
    {
      T::ContentInfoList contentList2;
      int result = mContentServerPtr->getContentListByParameterAndGenerationId(
          0,
          generationId,
          pInfo.mParameterKeyType,
          pInfo.mParameterKey,
          T::ParamLevelIdTypeValue::IGNORE,
          -1,
          0,
          0,
          forecastType,
          forecastNumber,
          producerGeometryId,
          "19000101T000000",
          "30000101T000000",
          0,
          contentList2);
      if (result != 0)
      {
        SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
        exception.addParameter("Service", "getContentListByParameterAndGenerationId");
        exception.addParameter("Message", ContentServer::getResultString(result));
        throw exception;
      }

      if (contentList2.getLength() == 0) return false;

      T::ContentInfo* cInfo = contentList2.getContentInfoByIndex(0);
      fTime = cInfo->mForecastTime.substr(0, 4) + forecastTime.substr(4);
    }

    int result = mContentServerPtr->getContentListByParameterGenerationIdAndForecastTime(
        0,
        generationId,
        pInfo.mParameterKeyType,
        pInfo.mParameterKey,
        pInfo.mParameterLevelIdType,
        paramLevelId,
        paramLevel,
        forecastType,
        forecastNumber,
        producerGeometryId,
        fTime,
        contentList);
    if (result != 0)
    {
      SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
      exception.addParameter("Service", "getContentListByParameterGenerationIdAndForecastTime");
      exception.addParameter("Message", ContentServer::getResultString(result));
      throw exception;
    }

    PRINT_DATA(mDebugLog, "         + Found %u content records\n", contentList.getLength());

    if (mDebugLog != nullptr)
    {
      std::stringstream stream;
      contentList.print(stream, 0, 4);
      PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
    }

    uint contentLen = contentList.getLength();
    bool multipleValues = contentList.containsSameForecastTimes();
    if (multipleValues)
    {
      PRINT_DATA(mDebugLog,
                 "         + Content records contains multiple values with the same timestep\n");
      return false;
    }

    if (contentLen == 0 && ((parameterFlags & QPF_PRESSURE_LEVEL_INTERPOLATION_ENABLED) != 0 ||
                            (parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0))
    {
      // It seems that there are no content available for the requested level. Let's try
      // to find some other levels.

      int lowerLevel = -1000000000;
      int higherLevel = 1000000000;
      double lowerHeight = -1000000000;
      double higherHeight = 1000000000;
      T::ParamLevelId pLevelId = paramLevelId;

      int result = mContentServerPtr->getContentListByParameterAndGenerationId(
          0,
          generationId,
          pInfo.mParameterKeyType,
          pInfo.mParameterKey,
          T::ParamLevelIdTypeValue::IGNORE,
          -1,
          0,
          0,
          forecastType,
          forecastNumber,
          producerGeometryId,
          "19000101T000000",
          "30000101T000000",
          0,
          contentList);
      if (result != 0)
      {
        SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
        exception.addParameter("Service", "getContentListByParameterAndGenerationId");
        exception.addParameter("Message", ContentServer::getResultString(result));
        throw exception;
      }

      uint contentLen = contentList.getLength();
      if (contentLen > 0)
      {
        // It seems that there is some content available for other levels.

        std::set<int> levelIdList;

        for (uint t = 0; t < contentLen; t++)
        {
          T::ContentInfo* cInfo = contentList.getContentInfoByIndex(t);

          if (cInfo->mParameterLevel != 0)
          {
            if (levelIdList.find(cInfo->mFmiParameterLevelId) == levelIdList.end())
              levelIdList.insert(cInfo->mFmiParameterLevelId);

            if ((parameterFlags & QPF_PRESSURE_LEVEL_INTERPOLATION_ENABLED) != 0 &&
                cInfo->mFmiParameterLevelId == 2)
            {
              // The data was requested by using pressure levels.

              if (cInfo->mParameterLevel < paramLevel && cInfo->mParameterLevel > lowerLevel)
                lowerLevel = cInfo->mParameterLevel;

              if (cInfo->mParameterLevel > paramLevel && cInfo->mParameterLevel < higherLevel)
                higherLevel = cInfo->mParameterLevel;
            }

            if ((parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0 &&
                cInfo->mFmiParameterLevelId == 3)
            {
              // The data was requested by using metric levels.

              if (cInfo->mParameterLevel < paramLevel && cInfo->mParameterLevel > lowerHeight)
                lowerHeight = cInfo->mParameterLevel;

              if (cInfo->mParameterLevel > paramLevel && cInfo->mParameterLevel < higherHeight)
                higherHeight = cInfo->mParameterLevel;
            }
          }
        }

        if ((parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0)
        {
          // The data was requested by using metric levels

          if (levelIdList.find(3) != levelIdList.end())
          {
            // There are metric levels available
            lowerLevel = lowerHeight;
            higherLevel = higherHeight;
            pLevelId = 3;
          }
          else if (levelIdList.find(2) != levelIdList.end())
          {
            // The data is not available on the metric level, but it can be found from the pressure
            // level.
            getPressureLevelsAndHeights(producerInfo,
                                        generationId,
                                        forecastTime,
                                        forecastType,
                                        forecastNumber,
                                        producerGeometryId,
                                        coordinates[0][0].x(),
                                        coordinates[0][0].y(),
                                        paramLevel,
                                        lowerLevel,
                                        higherLevel,
                                        lowerHeight,
                                        higherHeight);
            pLevelId = 2;
          }
        }

        if (lowerLevel != -1000000000 && higherLevel != 1000000000)
        {
          // We found lower and upper levels. Let's fetch data from the both levels and count
          // new values by interpolating.

          ParameterValues parameterValues1;
          ParameterValues parameterValues2;

          if (!getPointValues(producerInfo,
                              producerGeometryId,
                              generationId,
                              pInfo,
                              forecastTime,
                              pLevelId,
                              lowerLevel,
                              forecastType,
                              forecastNumber,
                              0,
                              areaInterpolationMethod,
                              timeInterpolationMethod,
                              levelInterpolationMethod,
                              coordinates,
                              newProducerId,
                              parameterValues1))
            return false;

          if (!getPointValues(producerInfo,
                              producerGeometryId,
                              generationId,
                              pInfo,
                              forecastTime,
                              pLevelId,
                              higherLevel,
                              forecastType,
                              forecastNumber,
                              0,
                              areaInterpolationMethod,
                              timeInterpolationMethod,
                              levelInterpolationMethod,
                              coordinates,
                              newProducerId,
                              parameterValues2))
            return false;

          short iplMethod = levelInterpolationMethod;
          if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
          {
            if ((parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0)
              iplMethod = T::LevelInterpolationMethod::Linear;
            else
              iplMethod = T::LevelInterpolationMethod::Logarithmic;
          }

          if ((parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0)
            levelInterpolation(iplMethod,
                               paramLevel,
                               lowerHeight,
                               higherHeight,
                               parameterValues1,
                               parameterValues2,
                               valueList);
          else
            levelInterpolation(iplMethod,
                               paramLevel,
                               lowerLevel,
                               higherLevel,
                               parameterValues1,
                               parameterValues2,
                               valueList);

          return true;
        }
      }
#if 0
      if (lowerLevel != -1000000000)
      {
        // We found only a lower level. Let's use it.

        return getPointValues(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,pLevelId,lowerLevel,forecastType,forecastNumber,0,
                              areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,coordinates,newProducerId,valueList);
      }

      if (higherLevel != 1000000000)
      {
        // We found only an upper level. Let's use it.
        return getPointValues(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,pLevelId,higherLevel,forecastType,forecastNumber,0,
                              areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,coordinates,newProducerId,valueList);
      }
#endif
      return false;
    }

    if (contentLen == 0) return false;

    // We found content information close to the current forecast time

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelIdType = pInfo.mParameterLevelIdType;
    valueList.mParameterLevelId = paramLevelId;

    // contentList.print(std::cout,0,0);

    if (contentLen == 1)
    {
      T::ContentInfo* contentInfo = contentList.getContentInfoByIndex(0);
      if (contentInfo->mForecastTime == fTime)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time.

        valueList.mForecastTime = forecastTime;
        valueList.mProducerId = contentInfo->mProducerId;
        valueList.mGenerationId = contentInfo->mGenerationId;
        valueList.mGeometryId = contentInfo->mGeometryId;
        valueList.mForecastType = contentInfo->mForecastType;
        valueList.mForecastNumber = contentInfo->mForecastNumber;
        valueList.mParameterLevel = contentInfo->mParameterLevel;
        if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
          valueList.mParameterLevelId = contentInfo->mFmiParameterLevelId;
        else
          valueList.mParameterLevelId = pInfo.mParameterLevelId;

        int result =
            mDataServerPtr->getGridValueListByPointList(0,
                                                        contentInfo->mFileId,
                                                        contentInfo->mMessageIndex,
                                                        flags,
                                                        T::CoordinateTypeValue::LATLON_COORDINATES,
                                                        coordinates[0],
                                                        areaInterpolationMethod,
                                                        valueList.mValueList);
        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueListByPointList");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          // throw exception;
        }

        if (conversionByFunction) executeConversion(function, functionParams, valueList.mValueList);

        return true;
      }
      else
      {
        // There is one content record in place, but its time does not match to
        // the requested forecast time. This is used for indicating that there
        // are content records available, but not for the requested time.
        // So, we should use this producer.

        newProducerId = producerInfo.mProducerId;
        valueList.mProducerId = producerInfo.mProducerId;
        return false;
      }
    }

    if (contentLen == 2)
    {
      T::ContentInfo* contentInfo1 = contentList.getContentInfoByIndex(0);
      T::ContentInfo* contentInfo2 = contentList.getContentInfoByIndex(1);
      if (!(contentInfo1->mForecastTime < fTime && contentInfo2->mForecastTime > fTime))
      {
        SmartMet::Spine::Exception exception(BCP, "Unexpected result!");
        exception.addDetail("The given forecast time should been between the found content times.");
        exception.addParameter("Content 1 ForecastTime", contentInfo1->mForecastTime);
        exception.addParameter("Content 2 ForecastTime", contentInfo2->mForecastTime);
        exception.addParameter("Request ForecastTime", forecastTime);
        throw exception;
      }

      // We did not find a grid with the exact forecast time, but we find grids that
      // are before and after the current forecast time. This means that we should do
      // some time interpolation.

      valueList.mForecastTime = forecastTime;
      valueList.mProducerId = contentInfo1->mProducerId;
      valueList.mGenerationId = contentInfo1->mGenerationId;
      valueList.mGeometryId = contentInfo1->mGeometryId;
      valueList.mForecastType = contentInfo1->mForecastType;
      valueList.mForecastNumber = contentInfo1->mForecastNumber;
      valueList.mParameterLevel = contentInfo1->mParameterLevel;
      if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
        valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;
      else
        valueList.mParameterLevelId = pInfo.mParameterLevelId;

      // Fetching data from the grids.

      T::GridValueList list1;
      int result1 =
          mDataServerPtr->getGridValueListByPointList(0,
                                                      contentInfo1->mFileId,
                                                      contentInfo1->mMessageIndex,
                                                      flags,
                                                      T::CoordinateTypeValue::LATLON_COORDINATES,
                                                      coordinates[0],
                                                      areaInterpolationMethod,
                                                      list1);
      if (result1 != 0)
      {
        SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
        exception.addParameter("Service", "getGridValueListByPointList");
        exception.addParameter("Message", DataServer::getResultString(result1));
        std::string errorMsg = exception.getStackTrace();
        PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
        // throw exception;
      }

      T::GridValueList list2;
      int result2 =
          mDataServerPtr->getGridValueListByPointList(0,
                                                      contentInfo2->mFileId,
                                                      contentInfo2->mMessageIndex,
                                                      flags,
                                                      T::CoordinateTypeValue::LATLON_COORDINATES,
                                                      coordinates[0],
                                                      areaInterpolationMethod,
                                                      list2);
      if (result2 != 0)
      {
        SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
        exception.addParameter("Service", "getGridValueListByPointList");
        exception.addParameter("Message", DataServer::getResultString(result2));
        std::string errorMsg = exception.getStackTrace();
        PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
        // throw exception;
      }

      if (result1 == 0 && result2 == 0)
      {
        timeInterpolation(timeInterpolationMethod,
                          forecastTime,
                          contentInfo1->mForecastTime,
                          contentInfo2->mForecastTime,
                          list1,
                          list2,
                          valueList.mValueList);

        if (conversionByFunction) executeConversion(function, functionParams, valueList.mValueList);

        return true;
      }
    }
    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

bool ServiceImplementation::getCircleValues(T::ProducerInfo& producerInfo,
                                            T::GeometryId producerGeometryId,
                                            uint& generationId,
                                            ParameterMapping& pInfo,
                                            std::string forecastTime,
                                            T::ParamLevelId paramLevelId,
                                            T::ParamLevel paramLevel,
                                            T::ForecastType forecastType,
                                            T::ForecastNumber forecastNumber,
                                            uint parameterFlags,
                                            short timeInterpolationMethod,
                                            short levelInterpolationMethod,
                                            double x,
                                            double y,
                                            double radius,
                                            uint& newProducerId,
                                            ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    std::string startTime = forecastTime;
    std::string endTime = forecastTime;
    uint flags = 0;

    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction =
        conversionFunction(pInfo.mConversionFunction, function, functionParams);

    T::ContentInfoList contentList;
    int result = mContentServerPtr->getContentListByParameterGenerationIdAndForecastTime(
        0,
        generationId,
        pInfo.mParameterKeyType,
        pInfo.mParameterKey,
        pInfo.mParameterLevelIdType,
        paramLevelId,
        paramLevel,
        forecastType,
        forecastNumber,
        producerGeometryId,
        forecastTime,
        contentList);
    if (result != 0)
    {
      SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
      exception.addParameter("Service", "getContentListByParameterGenerationIdAndForecastTime");
      exception.addParameter("Message", ContentServer::getResultString(result));
      throw exception;
    }

    PRINT_DATA(mDebugLog, "         + Found %u content records\n", contentList.getLength());

    if (mDebugLog != nullptr)
    {
      std::stringstream stream;
      contentList.print(stream, 0, 4);
      PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
    }

    uint contentLen = contentList.getLength();
    bool multipleValues = contentList.containsSameForecastTimes();
    if (multipleValues)
    {
      PRINT_DATA(mDebugLog,
                 "         + Content records contains multiple values with the same timestep\n");
      return false;
    }

    if (contentLen == 0 && ((parameterFlags & QPF_PRESSURE_LEVEL_INTERPOLATION_ENABLED) != 0 ||
                            (parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0))
    {
      // It seems that there are no content available for the requested level. Let's try
      // to find some other levels.

      int lowerLevel = -1000000000;
      int higherLevel = 1000000000;
      double lowerHeight = 0;
      double higherHeight = 0;
      T::ParamLevelId pLevelId = paramLevelId;

      int result =
          mContentServerPtr->getContentListByParameterAndGenerationId(0,
                                                                      generationId,
                                                                      pInfo.mParameterKeyType,
                                                                      pInfo.mParameterKey,
                                                                      pInfo.mParameterLevelIdType,
                                                                      paramLevelId,
                                                                      -1000000000,
                                                                      1000000000,
                                                                      forecastType,
                                                                      forecastNumber,
                                                                      producerGeometryId,
                                                                      "19000101T000000",
                                                                      "30000101T000000",
                                                                      0,
                                                                      contentList);
      if (result != 0)
      {
        SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
        exception.addParameter("Service", "getContentListByParameterAndGenerationId");
        exception.addParameter("Message", ContentServer::getResultString(result));
        throw exception;
      }

      uint contentLen = contentList.getLength();
      if (contentLen > 0)
      {
        std::set<int> levelIdList;

        for (uint t = 0; t < contentLen; t++)
        {
          T::ContentInfo* cInfo = contentList.getContentInfoByIndex(t);

          if (cInfo->mParameterLevel != 0)
          {
            if (levelIdList.find(cInfo->mFmiParameterLevelId) == levelIdList.end())
              levelIdList.insert(cInfo->mFmiParameterLevelId);

            if ((parameterFlags & QPF_PRESSURE_LEVEL_INTERPOLATION_ENABLED) != 0 &&
                cInfo->mFmiParameterLevelId == 2)
            {
              // The data was requested by using pressure levels.

              if (cInfo->mParameterLevel < paramLevel && cInfo->mParameterLevel > lowerLevel)
                lowerLevel = cInfo->mParameterLevel;

              if (cInfo->mParameterLevel > paramLevel && cInfo->mParameterLevel < higherLevel)
                higherLevel = cInfo->mParameterLevel;
            }

            if ((parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0 &&
                cInfo->mFmiParameterLevelId == 3)
            {
              // The data was requested by using metric levels.

              if (cInfo->mParameterLevel < paramLevel && cInfo->mParameterLevel > lowerHeight)
                lowerHeight = cInfo->mParameterLevel;

              if (cInfo->mParameterLevel > paramLevel && cInfo->mParameterLevel < higherHeight)
                higherHeight = cInfo->mParameterLevel;
            }
          }
        }

        if ((parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0)
        {
          // The data was requested by using metric levels

          if (levelIdList.find(3) != levelIdList.end())
          {
            // There are metric levels available
            lowerLevel = lowerHeight;
            higherLevel = higherHeight;
            pLevelId = 3;
          }
          else if (levelIdList.find(2) != levelIdList.end())
          {
            // The data is not available on the metric level, but it can be found from the pressure
            // level.
            getPressureLevelsAndHeights(producerInfo,
                                        generationId,
                                        forecastTime,
                                        forecastType,
                                        forecastNumber,
                                        producerGeometryId,
                                        x,
                                        y,
                                        paramLevel,
                                        lowerLevel,
                                        higherLevel,
                                        lowerHeight,
                                        higherHeight);
            pLevelId = 2;
          }
        }

        if (lowerLevel != -1000000000 && higherLevel != 1000000000)
        {
          // We found lower and upper levels. Let's fetch data from the both levels and count
          // new values by interpolating.

          ParameterValues parameterValues1;
          ParameterValues parameterValues2;

          if (!getCircleValues(producerInfo,
                               producerGeometryId,
                               generationId,
                               pInfo,
                               forecastTime,
                               pLevelId,
                               lowerLevel,
                               forecastType,
                               forecastNumber,
                               0,
                               timeInterpolationMethod,
                               levelInterpolationMethod,
                               x,
                               y,
                               radius,
                               newProducerId,
                               parameterValues1))
            return false;

          if (!getCircleValues(producerInfo,
                               producerGeometryId,
                               generationId,
                               pInfo,
                               forecastTime,
                               pLevelId,
                               higherLevel,
                               forecastType,
                               forecastNumber,
                               0,
                               timeInterpolationMethod,
                               levelInterpolationMethod,
                               x,
                               y,
                               radius,
                               newProducerId,
                               parameterValues2))
            return false;

          short iplMethod = levelInterpolationMethod;
          if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
          {
            if ((parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0)
              iplMethod = T::LevelInterpolationMethod::Linear;
            else
              iplMethod = T::LevelInterpolationMethod::Logarithmic;
          }

          if ((parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0)
            levelInterpolation(iplMethod,
                               paramLevel,
                               lowerHeight,
                               higherHeight,
                               parameterValues1,
                               parameterValues2,
                               valueList);
          else
            levelInterpolation(iplMethod,
                               paramLevel,
                               lowerLevel,
                               higherLevel,
                               parameterValues1,
                               parameterValues2,
                               valueList);

          // levelInterpolation(levelInterpolationMethod,paramLevel,parameterValues1.mParameterLevel,parameterValues2.mParameterLevel,parameterValues1,parameterValues2,valueList);
          return true;
        }
#if 0
        if (lowerLevel != -1000000000)
        {
          // We found only a lower level. Let's use it.

          return getCircleValues(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,pLevelId,lowerLevel,forecastType,forecastNumber,0,
                                timeInterpolationMethod,levelInterpolationMethod,x,y,radius,newProducerId,valueList);
        }

        if (higherLevel != 1000000000)
        {
          // We found only an upper level. Let's use it.
          return getCircleValues(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,pLevelId,higherLevel,forecastType,forecastNumber,0,
                                timeInterpolationMethod,levelInterpolationMethod,x,y,radius,newProducerId,valueList);
        }
#endif
        return false;
      }
    }

    if (contentLen == 0) return false;

    // We found content information close to the current forecast time

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelIdType = pInfo.mParameterLevelIdType;
    valueList.mParameterLevelId = pInfo.mParameterLevelId;

    if (contentLen == 1)
    {
      T::ContentInfo* contentInfo = contentList.getContentInfoByIndex(0);
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
        if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
          valueList.mParameterLevelId = contentInfo->mFmiParameterLevelId;
        else
          valueList.mParameterLevelId = pInfo.mParameterLevelId;

        int result =
            mDataServerPtr->getGridValueListByCircle(0,
                                                     contentInfo->mFileId,
                                                     contentInfo->mMessageIndex,
                                                     flags,
                                                     T::CoordinateTypeValue::LATLON_COORDINATES,
                                                     x,
                                                     y,
                                                     radius,
                                                     valueList.mValueList);
        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueListByCircle");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          // throw exception;
        }

        if (conversionByFunction) executeConversion(function, functionParams, valueList.mValueList);

        return true;
      }
      else
      {
        // There is one content record in place, but its time does not match to
        // the requested forecast time. This is used for indicating that there
        // are content records available, but not for the requested time.
        // So, we should use this producer.

        newProducerId = producerInfo.mProducerId;
        valueList.mProducerId = producerInfo.mProducerId;
        return false;
      }
    }

    if (contentLen == 2)
    {
      T::ContentInfo* contentInfo1 = contentList.getContentInfoByIndex(0);
      T::ContentInfo* contentInfo2 = contentList.getContentInfoByIndex(1);
      if (!(contentInfo1->mForecastTime < forecastTime &&
            contentInfo2->mForecastTime > forecastTime))
      {
        SmartMet::Spine::Exception exception(BCP, "Unexpected result!");
        exception.addDetail("The given forecast time should been between the found content times.");
        exception.addParameter("Content 1 ForecastTime", contentInfo1->mForecastTime);
        exception.addParameter("Content 2 ForecastTime", contentInfo2->mForecastTime);
        exception.addParameter("Request ForecastTime", forecastTime);
        throw exception;
      }

      // We did not find a grid with the exact forecast time, but we find grids that
      // are before and after the current forecast time. This means that we should do
      // some time interpolation.

      valueList.mForecastTime = forecastTime;
      valueList.mProducerId = contentInfo1->mProducerId;
      valueList.mGenerationId = contentInfo1->mGenerationId;
      valueList.mGeometryId = contentInfo1->mGeometryId;
      valueList.mForecastType = contentInfo1->mForecastType;
      valueList.mForecastNumber = contentInfo1->mForecastNumber;
      valueList.mParameterLevel = contentInfo1->mParameterLevel;
      if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
        valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;
      else
        valueList.mParameterLevelId = pInfo.mParameterLevelId;

      // Fetching data from the grids.

      T::GridValueList list1;
      int result1 =
          mDataServerPtr->getGridValueListByCircle(0,
                                                   contentInfo1->mFileId,
                                                   contentInfo1->mMessageIndex,
                                                   flags,
                                                   T::CoordinateTypeValue::LATLON_COORDINATES,
                                                   x,
                                                   y,
                                                   radius,
                                                   list1);
      if (result1 != 0)
      {
        SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
        exception.addParameter("Service", "getGridValueListByCircle");
        exception.addParameter("Message", DataServer::getResultString(result1));
        std::string errorMsg = exception.getStackTrace();
        PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
        // throw exception;
      }

      T::GridValueList list2;
      int result2 =
          mDataServerPtr->getGridValueListByCircle(0,
                                                   contentInfo2->mFileId,
                                                   contentInfo2->mMessageIndex,
                                                   flags,
                                                   T::CoordinateTypeValue::LATLON_COORDINATES,
                                                   x,
                                                   y,
                                                   radius,
                                                   list2);
      if (result2 != 0)
      {
        SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
        exception.addParameter("Service", "getGridValueListByCircle");
        exception.addParameter("Message", DataServer::getResultString(result2));
        std::string errorMsg = exception.getStackTrace();
        PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
        // throw exception;
      }

      if (result1 == 0 && result2 == 0)
      {
        timeInterpolation(timeInterpolationMethod,
                          forecastTime,
                          contentInfo1->mForecastTime,
                          contentInfo2->mForecastTime,
                          list1,
                          list2,
                          valueList.mValueList);

        if (conversionByFunction) executeConversion(function, functionParams, valueList.mValueList);

        return true;
      }
    }
    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

bool ServiceImplementation::getPolygonValues(T::ProducerInfo& producerInfo,
                                             T::GeometryId producerGeometryId,
                                             uint& generationId,
                                             ParameterMapping& pInfo,
                                             std::string forecastTime,
                                             T::ParamLevelId paramLevelId,
                                             T::ParamLevel paramLevel,
                                             T::ForecastType forecastType,
                                             T::ForecastNumber forecastNumber,
                                             uint parameterFlags,
                                             short timeInterpolationMethod,
                                             short levelInterpolationMethod,
                                             QueryCoordinates& coordinates,
                                             uint& newProducerId,
                                             ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    std::string startTime = forecastTime;
    std::string endTime = forecastTime;
    uint flags = 0;

    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction =
        conversionFunction(pInfo.mConversionFunction, function, functionParams);

    T::ContentInfoList contentList;
    int result = mContentServerPtr->getContentListByParameterGenerationIdAndForecastTime(
        0,
        generationId,
        pInfo.mParameterKeyType,
        pInfo.mParameterKey,
        pInfo.mParameterLevelIdType,
        paramLevelId,
        paramLevel,
        forecastType,
        forecastNumber,
        producerGeometryId,
        forecastTime,
        contentList);
    if (result != 0)
    {
      SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
      exception.addParameter("Service", "getContentListByParameterGenerationIdAndForecastTime");
      exception.addParameter("Message", ContentServer::getResultString(result));
      throw exception;
    }

    PRINT_DATA(mDebugLog, "         + Found %u content records\n", contentList.getLength());

    if (mDebugLog != nullptr)
    {
      std::stringstream stream;
      contentList.print(stream, 0, 4);
      PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
    }

    uint contentLen = contentList.getLength();
    bool multipleValues = contentList.containsSameForecastTimes();
    if (multipleValues)
    {
      PRINT_DATA(mDebugLog,
                 "         + Content records contains multiple values with the same timestep\n");
      return false;
    }

    if (contentLen == 0) return false;

    // We found content information close to the current forecast time

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelIdType = pInfo.mParameterLevelIdType;
    valueList.mParameterLevelId = paramLevelId;

    if (contentLen == 1)
    {
      T::ContentInfo* contentInfo = contentList.getContentInfoByIndex(0);
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
        if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
          valueList.mParameterLevelId = contentInfo->mFmiParameterLevelId;
        else
          valueList.mParameterLevelId = pInfo.mParameterLevelId;

        int result = mDataServerPtr->getGridValueListByPolygonPath(
            0,
            contentInfo->mFileId,
            contentInfo->mMessageIndex,
            flags,
            T::CoordinateTypeValue::LATLON_COORDINATES,
            coordinates,
            valueList.mValueList);
        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueListByPolygonPath");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          // throw exception;
        }

        if (conversionByFunction) executeConversion(function, functionParams, valueList.mValueList);

        return true;
      }
      else
      {
        // There is one content record in place, but its time does not match to
        // the requested forecast time. This is used for indicating that there
        // are content records available, but not for the requested time.
        // So, we should use this producer.

        newProducerId = producerInfo.mProducerId;
        valueList.mProducerId = producerInfo.mProducerId;
        return false;
      }
    }

    if (contentLen == 2)
    {
      T::ContentInfo* contentInfo1 = contentList.getContentInfoByIndex(0);
      T::ContentInfo* contentInfo2 = contentList.getContentInfoByIndex(1);
      if (!(contentInfo1->mForecastTime < forecastTime &&
            contentInfo2->mForecastTime > forecastTime))
      {
        SmartMet::Spine::Exception exception(BCP, "Unexpected result!");
        exception.addDetail("The given forecast time should been between the found content times.");
        exception.addParameter("Content 1 ForecastTime", contentInfo1->mForecastTime);
        exception.addParameter("Content 2 ForecastTime", contentInfo2->mForecastTime);
        exception.addParameter("Request ForecastTime", forecastTime);
        throw exception;
      }

      // We did not find a grid with the exact forecast time, but we find grids that
      // are before and after the current forecast time. This means that we should do
      // some time interpolation.

      valueList.mForecastTime = forecastTime;
      valueList.mProducerId = contentInfo1->mProducerId;
      valueList.mGenerationId = contentInfo1->mGenerationId;
      valueList.mGeometryId = contentInfo1->mGeometryId;
      valueList.mForecastType = contentInfo1->mForecastType;
      valueList.mForecastNumber = contentInfo1->mForecastNumber;
      valueList.mParameterLevel = contentInfo1->mParameterLevel;
      if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
        valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;
      else
        valueList.mParameterLevelId = pInfo.mParameterLevelId;

      // Fetching data from the grids.

      T::GridValueList list1;
      int result1 =
          mDataServerPtr->getGridValueListByPolygonPath(0,
                                                        contentInfo1->mFileId,
                                                        contentInfo1->mMessageIndex,
                                                        flags,
                                                        T::CoordinateTypeValue::LATLON_COORDINATES,
                                                        coordinates,
                                                        list1);
      if (result1 != 0)
      {
        SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
        exception.addParameter("Service", "getGridValueListByPolygonPath");
        exception.addParameter("Message", DataServer::getResultString(result1));
        std::string errorMsg = exception.getStackTrace();
        PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
        // throw exception;
      }

      T::GridValueList list2;
      int result2 =
          mDataServerPtr->getGridValueListByPolygonPath(0,
                                                        contentInfo2->mFileId,
                                                        contentInfo2->mMessageIndex,
                                                        flags,
                                                        T::CoordinateTypeValue::LATLON_COORDINATES,
                                                        coordinates,
                                                        list2);
      if (result2 != 0)
      {
        SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
        exception.addParameter("Service", "getGridValueListByPolygonPath");
        exception.addParameter("Message", DataServer::getResultString(result2));
        std::string errorMsg = exception.getStackTrace();
        PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
        // throw exception;
      }

      if (result1 == 0 && result2 == 0)
      {
        timeInterpolation(timeInterpolationMethod,
                          forecastTime,
                          contentInfo1->mForecastTime,
                          contentInfo2->mForecastTime,
                          list1,
                          list2,
                          valueList.mValueList);

        if (conversionByFunction) executeConversion(function, functionParams, valueList.mValueList);

        return true;
      }
    }
    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

void ServiceImplementation::getGridValues(Producer_vec& producers,
                                          std::set<T::GeometryId>& geometryIdList,
                                          uint producerId,
                                          std::string analysisTime,
                                          uint generationFlags,
                                          bool reverseGenerations,
                                          std::string parameterKey,
                                          T::ParamLevelId paramLevelId,
                                          T::ParamLevel paramLevel,
                                          T::ForecastType forecastType,
                                          T::ForecastNumber forecastNumber,
                                          uint parameterFlags,
                                          short areaInterpolationMethod,
                                          short timeInterpolationMethod,
                                          short levelInterpolationMethod,
                                          std::string forecastTime,
                                          bool timeMatchRequired,
                                          QuerySearchType searchType,
                                          QueryCoordinates& coordinates,
                                          double radius,
                                          double dem,
                                          ushort coverType,
                                          ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (mDebugLog != nullptr)
    {
      PRINT_DATA(mDebugLog, "\nMETHOD getGridValues()\n");
      PRINT_DATA(mDebugLog, "  - producers                : %lu items\n", producers.size());
      PRINT_DATA(mDebugLog, "  - geometryIdList           : %lu items\n", geometryIdList.size());
      PRINT_DATA(mDebugLog, "  - producerId               : %u\n", producerId);
      PRINT_DATA(mDebugLog, "  - analysisTime             : %s\n", analysisTime.c_str());
      PRINT_DATA(mDebugLog, "  - generationFlags          : %08x\n", generationFlags);
      PRINT_DATA(mDebugLog, "  - reverseGenerations       : %d\n", reverseGenerations);
      PRINT_DATA(mDebugLog, "  - parameterKey             : %s\n", parameterKey.c_str());
      PRINT_DATA(mDebugLog, "  - paramLevelId             : %d\n", paramLevelId);
      PRINT_DATA(mDebugLog, "  - paramLevel               : %d\n", paramLevel);
      PRINT_DATA(mDebugLog, "  - forecastType             : %d\n", forecastType);
      PRINT_DATA(mDebugLog, "  - forecastNumber           : %d\n", forecastNumber);
      PRINT_DATA(mDebugLog, "  - parameterFlags           : %u\n", parameterFlags);
      PRINT_DATA(mDebugLog, "  - areaInterpolationMethod  : %d\n", areaInterpolationMethod);
      PRINT_DATA(mDebugLog, "  - timeInterpolationMethod  : %d\n", timeInterpolationMethod);
      PRINT_DATA(mDebugLog, "  - levelInterpolationMethod : %d\n", levelInterpolationMethod);
      PRINT_DATA(mDebugLog, "  - forecastTime             : %s\n", forecastTime.c_str());
      PRINT_DATA(mDebugLog, "  - timeMatchRequired        : %d\n", timeMatchRequired);
      PRINT_DATA(mDebugLog, "  - searchType               : %d\n", searchType);
      PRINT_DATA(mDebugLog, "  - coordinates              : %lu vectors\n", coordinates.size());
      PRINT_DATA(mDebugLog, "  - radius                   : %f\n", radius);
      PRINT_DATA(mDebugLog, "  - dem                      : %f\n", dem);
      PRINT_DATA(mDebugLog, "  - coverType                : %u\n", coverType);
    }

    if (coordinates.size() == 0)
    {
      SmartMet::Spine::Exception exception(BCP, "No coordinates defined!");
      throw exception;
    }

    if (searchType == QST_POLYGON)
    {
      for (size_t t = 0; t < coordinates.size(); t++)
      {
        if (coordinates[t].size() < 3)
        {
          SmartMet::Spine::Exception exception(
              BCP, "Polygon definition requires at least 3 coordinates!");
          throw exception;
        }
      }
    }

    std::string startTime = forecastTime;
    std::string endTime = forecastTime;

    // Going through the producer list.

    for (auto it = producers.begin(); it != producers.end(); ++it)
    {
      std::string producerName = it->first;
      T::GeometryId producerGeometryId = it->second;

      PRINT_DATA(mDebugLog,
                 "  - Producer and Geometry : %s:%d\n",
                 producerName.c_str(),
                 producerGeometryId);

      // Checking if the current geometry can be found from the acceptable geometry list

      if (geometryIdList.find(producerGeometryId) != geometryIdList.end())
      {
        // The current producer supports a geometry where the current coordinates can be found.

        T::ProducerInfo producerInfo;
        if (mContentServerPtr->getProducerInfoByName(0, producerName, producerInfo) == 0 &&
            (producerId == 0 || producerInfo.mProducerId == producerId))
        {
          PRINT_DATA(mDebugLog, "  - The producer and the geometry are acceptable!\n");

          // Reading generations supported by the current producer.

          T::GenerationInfoList generationInfoList;
          int result = mContentServerPtr->getGenerationInfoListByProducerId(
              0, producerInfo.mProducerId, generationInfoList);
          if (result != 0)
          {
            SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
            exception.addParameter("Service", "getGenerationInfoListByProducerId");
            exception.addParameter("Message", ContentServer::getResultString(result));
            throw exception;
          }

          uint gLen = generationInfoList.getLength();
          if (gLen == 0)
            PRINT_DATA(mDebugLog, "    - No generations found for the current producer!\n");

          // Sorting generation analysis times.

          std::vector<std::string> analysisTimes;
          generationInfoList.getAnalysisTimes(analysisTimes, !reverseGenerations);

          // Going through all the parameter mapping files, until we find a match.

          // for (auto m = mParameterMappings.begin(); m != mParameterMappings.end(); ++m)
          {
            ParameterMapping_vec mappings;
            if (paramLevelId > 0 || paramLevel > 0)
              getParameterMappings(producerInfo.mName,
                                   parameterKey,
                                   T::ParamLevelIdTypeValue::ANY,
                                   paramLevelId,
                                   paramLevel,
                                   false,
                                   mappings);
            else
              getParameterMappings(producerInfo.mName, parameterKey, true, mappings);

            /*
                 ParameterMapping_vec mappings;

                 if (paramLevelId > 0 ||  paramLevel > 0)
                   m->getMappings(producerInfo.mName,parameterKey,T::ParamLevelIdTypeValue::ANY,paramLevelId,paramLevel,false,mappings);
                 else
                   m->getMappings(producerInfo.mName,parameterKey,true,mappings);

                  if (mappings.size() == 0)
                    PRINT_DATA(mDebugLog,"    - Parameter mappings '%s:%s' not found from the file
               '%s'!\n",producerInfo.mName.c_str(),parameterKey.c_str(),m->getFilename().c_str());
             */

            if (mappings.size() == 0)
              PRINT_DATA(mDebugLog,
                         "    - No parameter mappings '%s:%s:%d:%d' found!\n",
                         producerInfo.mName.c_str(),
                         parameterKey.c_str(),
                         paramLevelId,
                         paramLevel);

            if (mappings.size() > 0)
            {
              // PRINT_DATA(mDebugLog,"    - Parameter mappings '%s:%s' found from the file
              // '%s'!\n",producerInfo.mName.c_str(),parameterKey.c_str(),m->getFilename().c_str());

              uint gCount = 0;

              PRINT_DATA(mDebugLog,
                         "  - Going through the generations from the newest to the oldest.\n");

              for (auto gTime = analysisTimes.begin(); gTime != analysisTimes.end(); ++gTime)
              {
                PRINT_DATA(mDebugLog, "    * %s\n", gTime->c_str());

                uint gflags = 1 << gCount;

                bool generationValid = false;

                if (generationFlags == 0 || (generationFlags & gflags) != 0) generationValid = true;

                if (analysisTime.length() > 0 && analysisTime != *gTime) generationValid = false;

                if (generationValid)
                {
                  T::GenerationInfo* generationInfo =
                      generationInfoList.getGenerationInfoByAnalysisTime(*gTime);

                  PRINT_DATA(mDebugLog, "      - Going through the parameter mappings\n");
                  for (auto pInfo = mappings.begin(); pInfo != mappings.end(); ++pInfo)
                  {
                    if (mDebugLog != nullptr)
                    {
                      std::stringstream stream;
                      pInfo->print(stream, 5, 0);
                      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
                    }

                    short areaInterpolation = areaInterpolationMethod;
                    if (areaInterpolation == T::AreaInterpolationMethod::Undefined)
                      areaInterpolation = pInfo->mAreaInterpolationMethod;

                    short timeInterpolation = timeInterpolationMethod;
                    if (timeInterpolation == T::TimeInterpolationMethod::Undefined)
                      timeInterpolation = pInfo->mTimeInterpolationMethod;

                    short levelInterpolation = levelInterpolationMethod;
                    if (levelInterpolation == T::LevelInterpolationMethod::Undefined)
                      levelInterpolation = pInfo->mLevelInterpolationMethod;

                    T::ParamLevelId pLevelId = pInfo->mParameterLevelId;
                    T::ParamLevel pLevel = pInfo->mParameterLevel;

                    if (paramLevelId > 0) pLevelId = paramLevelId;

                    if (paramLevel != 0) pLevel = paramLevel;

                    if (searchType == QST_POLYGON)
                    {
                      if (getPolygonValues(producerInfo,
                                           producerGeometryId,
                                           generationInfo->mGenerationId,
                                           *pInfo,
                                           forecastTime,
                                           pLevelId,
                                           pLevel,
                                           forecastType,
                                           forecastNumber,
                                           parameterFlags,
                                           timeInterpolation,
                                           levelInterpolation,
                                           coordinates,
                                           producerId,
                                           valueList))
                        return;
                    }

                    if (searchType == QST_POINT)
                    {
                      if (areaInterpolation < T::AreaInterpolationMethod::List)
                      {
                        // List of points
                        if (getPointValues(producerInfo,
                                           producerGeometryId,
                                           generationInfo->mGenerationId,
                                           *pInfo,
                                           forecastTime,
                                           pLevelId,
                                           pLevel,
                                           forecastType,
                                           forecastNumber,
                                           parameterFlags,
                                           areaInterpolation,
                                           timeInterpolation,
                                           levelInterpolation,
                                           coordinates,
                                           producerId,
                                           valueList))
                          return;
                      }

                      if (areaInterpolation >= T::AreaInterpolationMethod::External &&
                          coordinates.size() == 1 && coordinates[0].size() == 1)
                      {
                        // double x = coordinates[0][0].x();
                        // double y = coordinates[0][0].y();

                        ParameterValues valList;
                        ParameterValues geopHeightList;
                        ParameterValues lapserateList;
                        ParameterValues landList;

                        std::string infoFunction = "getAreaInterpolationInfo";
                        std::string interpolationFunction;

                        std::string paramList =
                            mLuaFileCollection.executeFunctionCall6(infoFunction,
                                                                    producerInfo.mName,
                                                                    pInfo->mParameterName,
                                                                    pInfo->mParameterKeyType,
                                                                    pInfo->mParameterKey,
                                                                    pInfo->mParameterLevelIdType,
                                                                    pLevelId,
                                                                    pLevel,
                                                                    forecastType,
                                                                    forecastNumber,
                                                                    areaInterpolation);

                        valueList.mForecastTime = forecastTime;
                        valueList.mProducerId = producerInfo.mProducerId;
                        valueList.mGenerationId = generationInfo->mGenerationId;
                        valueList.mGeometryId = producerGeometryId;

                        valueList.mParameterKeyType = pInfo->mParameterKeyType;
                        valueList.mParameterKey = pInfo->mParameterKey;
                        valueList.mParameterLevelIdType = pInfo->mParameterLevelIdType;
                        valueList.mParameterLevelId = pInfo->mParameterLevelId;
                        valueList.mParameterLevel = pInfo->mParameterLevel;
                        valueList.mForecastType = forecastType;
                        valueList.mForecastNumber = forecastNumber;

                        std::vector<double> constParams;
                        std::vector<ParameterValues> valueListVec;

                        // std::cout << "LUA_PARAMETERS : " << paramList << "\n";

                        std::vector<std::string> paramVec;
                        splitString(paramList, ';', paramVec);

                        for (auto param = paramVec.begin(); param != paramVec.end(); ++param)
                        {
                          uint a_producerId = producerId;
                          std::string a_analysisTime = analysisTime;
                          uint a_generationFlags = generationFlags;
                          bool a_reverseGenerations = reverseGenerations;
                          std::string a_parameterKey = parameterKey;
                          T::ParamLevelId a_paramLevelId = paramLevelId;
                          T::ParamLevel a_paramLevel = paramLevel;
                          T::ForecastType a_forecastType = forecastType;
                          T::ForecastNumber a_forecastNumber = forecastNumber;
                          uint a_parameterFlags = parameterFlags;
                          short a_areaInterpolationMethod = areaInterpolationMethod;
                          short a_timeInterpolationMethod = timeInterpolationMethod;
                          short a_levelInterpolationMethod = levelInterpolationMethod;
                          std::string a_forecastTime = forecastTime;
                          bool a_timeMatchRequired = timeMatchRequired;
                          QuerySearchType a_searchType = searchType;
                          double a_radius = radius;
                          double a_dem = dem;
                          ushort a_coverType = coverType;

                          // std::cout << "* PARAM " << *param << "\n";
                          std::vector<std::string> paramParts;
                          splitString(*param, ':', paramParts);

                          if (paramParts.size() == 2)
                          {
                            if (paramParts[0] == "F")
                            {
                              interpolationFunction = paramParts[1];
                            }
                            else if (paramParts[0] == "V")
                            {
                              if (paramParts[1] == "dem")
                                constParams.push_back(C_DOUBLE(dem));
                              else if (paramParts[1] == "coverType")
                                constParams.push_back(C_DOUBLE(coverType));
                            }
                            if (paramParts[0] == "C")
                            {
                              constParams.push_back(toDouble(paramParts[1].c_str()));
                            }
                            else if (paramParts[0] == "Q")
                            {
                              std::vector<std::string> pv;
                              splitString(paramParts[1], ',', pv);

                              if (pv.size() == 8)
                              {
                                if (!pv[0].empty()) a_parameterKey = pv[0];

                                if (!pv[1].empty()) a_paramLevelId = toInt64(pv[1].c_str());

                                if (!pv[2].empty()) a_paramLevel = toInt64(pv[2].c_str());

                                if (!pv[3].empty()) a_forecastType = toInt64(pv[3].c_str());

                                if (!pv[4].empty()) a_forecastNumber = toInt64(pv[4].c_str());

                                if (!pv[5].empty())
                                  a_areaInterpolationMethod = toInt64(pv[5].c_str());

                                if (!pv[6].empty() && toInt64(pv[6].c_str()) == 1)
                                {
                                  T::ContentInfoList contentList;
                                  if (mContentServerPtr->getContentListByParameterAndGenerationId(
                                          0,
                                          generationInfo->mGenerationId,
                                          T::ParamKeyTypeValue::FMI_NAME,
                                          a_parameterKey,
                                          T::ParamLevelIdTypeValue::FMI,
                                          a_paramLevel,
                                          a_paramLevel,
                                          a_paramLevel,
                                          a_forecastType,
                                          a_forecastNumber,
                                          -1,
                                          "19000101T000000",
                                          "30000101T000000",
                                          0,
                                          contentList) == 0)
                                  {
                                    if (contentList.getLength() > 0)
                                    {
                                      contentList.print(std::cout, 0, 0);
                                      T::ContentInfo* cInfo = contentList.getContentInfoByIndex(0);
                                      a_forecastTime = cInfo->mForecastTime;
                                    }
                                  }
                                }

                                if (!pv[7].empty() && toInt64(pv[7].c_str()) == 1) a_producerId = 0;

                                // We should search data only from similar geometries. Otherwise
                                // the size and location of the grid cells are different.

                                std::set<T::GeometryId> geomIdList;
                                geomIdList.insert(producerGeometryId);

                                ParameterValues valList;
                                getGridValues(producers,
                                              geomIdList,
                                              a_producerId,
                                              a_analysisTime,
                                              a_generationFlags,
                                              a_reverseGenerations,
                                              a_parameterKey,
                                              a_paramLevelId,
                                              a_paramLevel,
                                              a_forecastType,
                                              a_forecastNumber,
                                              a_parameterFlags,
                                              a_areaInterpolationMethod,
                                              a_timeInterpolationMethod,
                                              a_levelInterpolationMethod,
                                              a_forecastTime,
                                              a_timeMatchRequired,
                                              a_searchType,
                                              coordinates,
                                              a_radius,
                                              a_dem,
                                              a_coverType,
                                              valList);

                                // printf("PARAM : %s
                                // %d\n",a_parameterKey.c_str(),valList.mValueList.getLength());
                                // valList.mValueList.print(std::cout,0,0);
                                valueList.mFlags = valList.mFlags;
                                valueListVec.push_back(valList);
                              }
                            }
                          }
                        }

                        if (valueListVec.size() == 0) return;

                        uint vLen1 = valueListVec[0].mValueList.getLength();

                        for (uint t = 0; t < vLen1; t++)
                        {
                          std::vector<double> params = constParams;

                          double vx = ParamValueMissing;
                          double vy = ParamValueMissing;

                          for (auto vList = valueListVec.begin(); vList != valueListVec.end();
                               ++vList)
                          {
                            uint vLen2 = vList->mValueList.getLength();

                            if (vLen1 == vLen2)
                            {
                              T::GridValue* v = vList->mValueList.getGridValueByIndex(t);
                              if (vx == ParamValueMissing)
                              {
                                vx = v->mX;
                                vy = v->mY;
                              }

                              if (!v->mValueString.empty())
                              {
                                std::vector<double> vec;
                                splitString(v->mValueString, ';', vec);

                                params.push_back(vec.size());
                                if (vec.size() > 0)
                                {
                                  for (auto it = vec.begin(); it != vec.end(); ++it)
                                    params.push_back(*it);
                                }
                              }
                              else
                              {
                                params.push_back(1);
                                params.push_back(v->mValue);
                              }
                            }
                          }
                          double val = mLuaFileCollection.executeFunctionCall1(
                              interpolationFunction, params);
                          // printf("#### VALUE [%f]\n",val);
                          valueList.mValueList.addGridValue(new T::GridValue(vx, vy, val));

                          std::string function;
                          std::vector<std::string> functionParams;
                          bool conversionByFunction = conversionFunction(
                              pInfo->mConversionFunction, function, functionParams);
                          if (conversionByFunction)
                            executeConversion(function, functionParams, valueList.mValueList);
                        }

                        return;
                      }

                      if (areaInterpolation >= T::AreaInterpolationMethod::List &&
                          areaInterpolation < T::AreaInterpolationMethod::External &&
                          coordinates.size() == 1 && coordinates[0].size() == 1)
                      {
                        double x = coordinates[0][0].x();
                        double y = coordinates[0][0].y();

                        // Special value fetching for external interpolation
                        if (getSpecialValues(producerInfo,
                                             producerGeometryId,
                                             generationInfo->mGenerationId,
                                             *pInfo,
                                             forecastTime,
                                             pLevelId,
                                             pLevel,
                                             forecastType,
                                             forecastNumber,
                                             parameterFlags,
                                             areaInterpolation,
                                             timeInterpolationMethod,
                                             levelInterpolationMethod,
                                             x,
                                             y,
                                             producerId,
                                             valueList))
                          return;
                      }
                    }

                    if (searchType == QST_PATH)
                    {
                      if (areaInterpolation < T::AreaInterpolationMethod::List)
                      {
                        // List of points
                        if (getPointValues(producerInfo,
                                           producerGeometryId,
                                           generationInfo->mGenerationId,
                                           *pInfo,
                                           forecastTime,
                                           pLevelId,
                                           pLevel,
                                           forecastType,
                                           forecastNumber,
                                           parameterFlags,
                                           areaInterpolation,
                                           timeInterpolation,
                                           levelInterpolation,
                                           coordinates,
                                           producerId,
                                           valueList))
                          return;
                      }

                      if (areaInterpolation >= T::AreaInterpolationMethod::List &&
                          areaInterpolation < T::AreaInterpolationMethod::External &&
                          coordinates.size() == 1 && coordinates[0].size() == 1)
                      {
                        double x = coordinates[0][0].x();
                        double y = coordinates[0][0].y();

                        // Special value fetching for external interpolation
                        if (getSpecialValues(producerInfo,
                                             producerGeometryId,
                                             generationInfo->mGenerationId,
                                             *pInfo,
                                             forecastTime,
                                             pLevelId,
                                             pLevel,
                                             forecastType,
                                             forecastNumber,
                                             parameterFlags,
                                             areaInterpolation,
                                             timeInterpolation,
                                             levelInterpolation,
                                             x,
                                             y,
                                             producerId,
                                             valueList))
                          return;
                      }
                    }

                    if (searchType == QST_CIRCLE && coordinates.size() == 1 &&
                        coordinates[0].size() == 1)
                    {
                      double x = coordinates[0][0].x();
                      double y = coordinates[0][0].y();

                      // Values inside of a circle
                      if (getCircleValues(producerInfo,
                                          producerGeometryId,
                                          generationInfo->mGenerationId,
                                          *pInfo,
                                          forecastTime,
                                          pLevelId,
                                          pLevel,
                                          forecastType,
                                          forecastNumber,
                                          parameterFlags,
                                          timeInterpolation,
                                          levelInterpolation,
                                          x,
                                          y,
                                          radius,
                                          producerId,
                                          valueList))
                        return;
                    }
                  }
                }
                gCount++;
              }
            }
          }
        }
        else
        {
          PRINT_DATA(mDebugLog, "  - Not a valid producer '%s'!\n", producerName.c_str());
        }
      }
      else
      {
        PRINT_DATA(mDebugLog,
                   "  - Producer's '%s' geometry '%d' does not cover all requested coordinates!\n",
                   producerName.c_str(),
                   producerGeometryId);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

void ServiceImplementation::getGridValues(Producer_vec& producers,
                                          std::set<T::GeometryId>& geometryIdList,
                                          uint producerId,
                                          std::string analysisTime,
                                          uint generationFlags,
                                          bool reverseGenerations,
                                          std::string parameterKey,
                                          T::ParamLevelId paramLevelId,
                                          T::ParamLevel paramLevel,
                                          T::ForecastType forecastType,
                                          T::ForecastNumber forecastNumber,
                                          uint parameterFlags,
                                          short areaInterpolationMethod,
                                          short timeInterpolationMethod,
                                          short levelInterpolationMethod,
                                          std::string startTime,
                                          std::string endTime,
                                          bool ignoreStartTimeValue,
                                          QuerySearchType searchType,
                                          QueryCoordinates& coordinates,
                                          double radius,
                                          double dem,
                                          ushort coverType,
                                          uint maxValues,
                                          ParameterValues_vec& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (mDebugLog != nullptr)
    {
      PRINT_DATA(mDebugLog, "\nMETHOD getGridValues()\n");
      PRINT_DATA(mDebugLog, "  - producers                : %lu items\n", producers.size());
      PRINT_DATA(mDebugLog, "  - geometryIdList           : %lu items\n", geometryIdList.size());
      PRINT_DATA(mDebugLog, "  - producerId               : %u\n", producerId);
      PRINT_DATA(mDebugLog, "  - analysisTime             : %s\n", analysisTime.c_str());
      PRINT_DATA(mDebugLog, "  - generationFlags          : %08x\n", generationFlags);
      PRINT_DATA(mDebugLog, "  - reverseGenerations       : %d\n", reverseGenerations);
      PRINT_DATA(mDebugLog, "  - parameterKey             : %s\n", parameterKey.c_str());
      PRINT_DATA(mDebugLog, "  - paramLevelId             : %d\n", paramLevelId);
      PRINT_DATA(mDebugLog, "  - paramLevel               : %d\n", paramLevel);
      PRINT_DATA(mDebugLog, "  - forecastType             : %d\n", forecastType);
      PRINT_DATA(mDebugLog, "  - forecastNumber           : %d\n", forecastNumber);
      PRINT_DATA(mDebugLog, "  - parameterFlags           : %u\n", parameterFlags);
      PRINT_DATA(mDebugLog, "  - areaInterpolationMethod  : %d\n", areaInterpolationMethod);
      PRINT_DATA(mDebugLog, "  - timeInterpolationMethod  : %d\n", timeInterpolationMethod);
      PRINT_DATA(mDebugLog, "  - levelInterpolationMethod : %d\n", levelInterpolationMethod);
      PRINT_DATA(mDebugLog, "  - startTime                : %s\n", startTime.c_str());
      PRINT_DATA(mDebugLog, "  - endTime                  : %s\n", endTime.c_str());
      PRINT_DATA(mDebugLog, "  - ignoreStartTimeValue     : %d\n", ignoreStartTimeValue);
      PRINT_DATA(mDebugLog, "  - searchType               : %d\n", searchType);
      PRINT_DATA(mDebugLog, "  - coordinates              : %lu vectors\n", coordinates.size());
      PRINT_DATA(mDebugLog, "  - radius                   : %f\n", radius);
      PRINT_DATA(mDebugLog, "  - dem                      : %f\n", dem);
      PRINT_DATA(mDebugLog, "  - coverType                : %u\n", coverType);
      PRINT_DATA(mDebugLog, "  - maxValues                : %u\n\n", maxValues);
    }

    if (coordinates.size() == 0)
    {
      SmartMet::Spine::Exception exception(BCP, "No coordinates defined!");
      throw exception;
    }

    if (searchType == QST_POLYGON)
    {
      for (size_t t = 0; t < coordinates.size(); t++)
      {
        if (coordinates[t].size() < 3)
        {
          SmartMet::Spine::Exception exception(BCP,
                                               "Area definition requires at least 3 coordinates!");
          throw exception;
        }
      }
    }

    // short areaInterpolation = areaInterpolationMethod;
    std::string minStartTime = "30000101T000000";
    std::string maxEndTime = "15000101T000000";
    // uint flags = 0;

    // Going through the producer list.

    for (auto it = producers.begin(); it != producers.end(); ++it)
    {
      std::string producerName = it->first;
      T::GeometryId producerGeometryId = it->second;

      PRINT_DATA(mDebugLog,
                 "  - Producer and Geometry : %s:%d\n",
                 producerName.c_str(),
                 producerGeometryId);

      // Checking if the current geometry can be found from the acceptable geometry list

      if (geometryIdList.find(producerGeometryId) != geometryIdList.end())
      {
        // The current producer supports a geometry where the current coordinates can be found.

        T::ProducerInfo producerInfo;
        if (mContentServerPtr->getProducerInfoByName(0, producerName, producerInfo) == 0 &&
            (producerId == 0 || producerInfo.mProducerId == producerId))
        {
          PRINT_DATA(mDebugLog, "  - The producer and the geometry are acceptable!\n");

          // Reading generations supported by the current producer.

          T::GenerationInfoList generationInfoList;
          int result = mContentServerPtr->getGenerationInfoListByProducerId(
              0, producerInfo.mProducerId, generationInfoList);
          if (result != 0)
          {
            SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
            exception.addParameter("Service", "getGenerationInfoListByProducerId");
            exception.addParameter("Message", ContentServer::getResultString(result));
            throw exception;
          }

          uint gLen = generationInfoList.getLength();

          if (gLen == 0)
            PRINT_DATA(mDebugLog, "    - No generations found for the current producer!\n");

          // Sorting generation analysis times.

          std::vector<std::string> analysisTimes;
          generationInfoList.getAnalysisTimes(analysisTimes, !reverseGenerations);

          // Going through all the parameter mapping files, until we find a match.

          //          for (auto m = mParameterMappings.begin(); m != mParameterMappings.end(); ++m)
          {
            ParameterMapping_vec mappings;
            if (paramLevelId > 0 || paramLevel > 0)
              getParameterMappings(producerInfo.mName,
                                   parameterKey,
                                   T::ParamLevelIdTypeValue::ANY,
                                   paramLevelId,
                                   paramLevel,
                                   false,
                                   mappings);
            else
              getParameterMappings(producerInfo.mName, parameterKey, true, mappings);
            /*
                        ParameterMapping_vec mappings;

                        if (paramLevelId > 0 ||  paramLevel > 0)
                          m->getMappings(producerInfo.mName,parameterKey,T::ParamLevelIdTypeValue::ANY,paramLevelId,paramLevel,false,mappings);
                        else
                          m->getMappings(producerInfo.mName,parameterKey,true,mappings);

                        if (mappings.size() == 0)
                          PRINT_DATA(mDebugLog,"    - Parameter mappings '%s:%s' not found from the
               file
               '%s'!\n",producerInfo.mName.c_str(),parameterKey.c_str(),m->getFilename().c_str());
            */
            if (mappings.size() > 0)
            {
              //              PRINT_DATA(mDebugLog,"    - Parameter mappings '%s:%s' found from the
              //              file
              //              '%s'!\n",producerInfo.mName.c_str(),parameterKey.c_str(),m->getFilename().c_str());

              uint gCount = 0;

              if (reverseGenerations)
              {
                PRINT_DATA(mDebugLog,
                           "    - Going through the generations from the oldest to the newest.\n");
              }
              else
              {
                PRINT_DATA(mDebugLog,
                           "    - Going through the generations from the newest to the oldest.\n");
              }

              for (auto gTime = analysisTimes.begin(); gTime != analysisTimes.end(); ++gTime)
              {
                PRINT_DATA(mDebugLog, "    * %s\n", gTime->c_str());

                uint gflags = 1 << gCount;

                bool generationValid = false;

                if (generationFlags == 0 || (generationFlags & gflags) != 0) generationValid = true;

                if (analysisTime.length() > 0 && analysisTime != *gTime) generationValid = false;

                if (generationValid)
                {
                  T::GenerationInfo* generationInfo =
                      generationInfoList.getGenerationInfoByAnalysisTime(*gTime);

                  PRINT_DATA(mDebugLog, "      - Going through the parameter mappings\n");
                  for (auto pInfo = mappings.begin(); pInfo != mappings.end(); ++pInfo)
                  {
                    std::set<std::string> contentTimeList;
                    // int result =
                    // mContentServerPtr->getContentTimeListByGenerationId(0,generationInfo->mGenerationId,contentTimeList);

                    T::ContentInfoList contentInfoList;
                    int result = mContentServerPtr->getContentListByParameterAndGenerationId(
                        0,
                        generationInfo->mGenerationId,
                        pInfo->mParameterKeyType,
                        pInfo->mParameterKey,
                        pInfo->mParameterLevelIdType,
                        pInfo->mParameterLevelId,
                        paramLevel,
                        paramLevel,
                        forecastType,
                        forecastNumber,
                        producerGeometryId,
                        std::string("19000101T000000"),
                        std::string("30000101T000000"),
                        0,
                        contentInfoList);

                    if (result != 0)
                    {
                      SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
                      exception.addParameter("Service", "getContentListByParameterAndGenerationId");
                      exception.addParameter("Message", ContentServer::getResultString(result));
                      throw exception;
                    }

                    uint cLen = contentInfoList.getLength();
                    for (uint t = 0; t < cLen; t++)
                    {
                      T::ContentInfo* cInfo = contentInfoList.getContentInfoByIndex(t);
                      if (cInfo != NULL)
                      {
                        if (contentTimeList.find(cInfo->mForecastTime) == contentTimeList.end())
                          contentTimeList.insert(cInfo->mForecastTime);
                      }
                    }

                    for (auto forecastTime = contentTimeList.begin();
                         forecastTime != contentTimeList.end();
                         ++forecastTime)
                    {
                      if (((*forecastTime == startTime && !ignoreStartTimeValue) ||
                           (*forecastTime > startTime && *forecastTime <= endTime)) &&
                          (*forecastTime < minStartTime || *forecastTime > maxEndTime))
                      {
                        ParameterValues valList;
                        getGridValues(producers,
                                      geometryIdList,
                                      generationInfo->mProducerId,
                                      analysisTime,
                                      gflags,
                                      reverseGenerations,
                                      parameterKey,
                                      paramLevelId,
                                      paramLevel,
                                      forecastType,
                                      forecastNumber,
                                      parameterFlags,
                                      areaInterpolationMethod,
                                      timeInterpolationMethod,
                                      levelInterpolationMethod,
                                      *forecastTime,
                                      true,
                                      searchType,
                                      coordinates,
                                      radius,
                                      dem,
                                      coverType,
                                      valList);

                        if (valList.mValueList.getLength() > 0)
                        {
                          if (valList.mForecastTime <= " ") valList.mForecastTime = *forecastTime;

                          if (*forecastTime < minStartTime)
                          {
                            minStartTime = *forecastTime;
                            valueList.insert(valueList.begin(), valList);
                          }

                          if (*forecastTime > maxEndTime && minStartTime != *forecastTime)
                          {
                            maxEndTime = *forecastTime;
                            valueList.push_back(valList);
                          }
                        }

                        if (valueList.size() == maxValues) return;

                        if (minStartTime <= startTime && maxEndTime >= endTime) return;
                      }
                    }
                  }
                }
                else
                {
                  PRINT_DATA(mDebugLog, "      - Generation not included into the search\n");
                }
                gCount++;
              }
            }
          }
        }
        else
        {
          PRINT_DATA(mDebugLog, "  - Not a valid producer '%s'!\n", producerName.c_str());
        }
      }
      else
      {
        PRINT_DATA(mDebugLog,
                   "  - Producer's '%s' geometry '%d' does not cover all requested coordinates!\n",
                   producerName.c_str(),
                   producerGeometryId);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

bool ServiceImplementation::getPressureLevelsAndHeights(T::ProducerInfo& producerInfo,
                                                        uint generationId,
                                                        std::string forecastTime,
                                                        T::ForecastType forecastType,
                                                        T::ForecastNumber forecastNumber,
                                                        T::GeometryId geometryId,
                                                        double x,
                                                        double y,
                                                        int height,
                                                        int& lowerPressure,
                                                        int& higherPressure,
                                                        double& lowerHeight,
                                                        double& higherHeight)
{
  FUNCTION_TRACE
  try
  {
    std::string cacheKey = std::to_string(generationId) + ":" + forecastTime + ":" +
                           std::to_string(x) + std::to_string(y);
    LevelHeight_vec levelHeights;

    if (mLevelHeightCache.size() > 0)
    {
      AutoThreadLock lock(&mCacheThreadLock);

      for (auto cc = mLevelHeightCache.rbegin();
           cc != mLevelHeightCache.rend() && levelHeights.size() == 0;
           ++cc)
      {
        if (cc->first == cacheKey)
        {
          levelHeights = cc->second;
          // std::cout << "*** CACHE MATCH : " << cacheKey << "\n";
        }
      }
    }

    if (levelHeights.size() == 0)
    {
      T::ParamKeyType paramKeyType = T::ParamKeyTypeValue::FMI_NAME;
      std::string paramKey = "Z-M2S2";
      T::ParamLevelIdType levelIdType = T::ParamLevelIdTypeValue::FMI;
      T::ParamLevelId levelId = 2;

      ParameterMapping_vec mappings;
      for (auto m = mParameterMappings.begin();
           m != mParameterMappings.end() && mappings.size() == 0;
           ++m)
      {
        m->getMappings(producerInfo.mName, paramKey, true, mappings);
      }

      if (mappings.size() == 0) return false;

      T::ContentInfoList contentList;
      int result = mContentServerPtr->getContentListByParameterAndGenerationId(0,
                                                                               generationId,
                                                                               paramKeyType,
                                                                               paramKey,
                                                                               levelIdType,
                                                                               levelId,
                                                                               0,
                                                                               1000000000,
                                                                               forecastType,
                                                                               forecastNumber,
                                                                               geometryId,
                                                                               "19000101T000000",
                                                                               "30000101T000000",
                                                                               0,
                                                                               contentList);
      if (result != 0)
      {
        SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
        exception.addParameter("Service", "getContentListByParameterAndGenerationId");
        exception.addParameter("Message", ContentServer::getResultString(result));
        throw exception;
      }

      uint contentLen = contentList.getLength();
      std::set<int> levels;
      if (contentLen > 0)
      {
        for (uint t = 0; t < contentLen; t++)
        {
          T::ContentInfo* cInfo = contentList.getContentInfoByIndex(t);
          if (levels.find(cInfo->mParameterLevel) == levels.end())
            levels.insert(cInfo->mParameterLevel);
        }

        QueryCoordinates coordinates;
        Coordinate_vec coordinateVec;
        coordinateVec.push_back(T::Coordinate(x, y));
        coordinates.push_back(coordinateVec);

        uint newProducerId = 0;
        for (auto level = levels.begin(); level != levels.end(); level++)
        {
          ParameterValues valueList;
          if (getPointValues(producerInfo,
                             geometryId,
                             generationId,
                             mappings[0],
                             forecastTime,
                             levelId,
                             *level,
                             forecastType,
                             forecastNumber,
                             0,
                             T::AreaInterpolationMethod::Linear,
                             T::TimeInterpolationMethod::Linear,
                             T::LevelInterpolationMethod::Linear,
                             coordinates,
                             newProducerId,
                             valueList))
          {
            // valueList.print(std::cout,0,0);
            if (valueList.mValueList.getLength() == 1)
            {
              T::GridValue* rec = valueList.mValueList.getGridValueByIndex(0);
              levelHeights.push_back(std::pair<int, double>(*level, rec->mValue));
            }
          }
        }

        if (levelHeights.size() > 0)
        {
          AutoThreadLock lock(&mCacheThreadLock);
          if (mLevelHeightCache.size() >= 5000)
            mLevelHeightCache.erase(mLevelHeightCache.begin(), mLevelHeightCache.begin() + 1000);

          mLevelHeightCache.push_back(
              std::pair<std::string, LevelHeight_vec>(cacheKey, levelHeights));
        }
      }
    }

    lowerPressure = -1000000000;
    higherPressure = 1000000000;
    lowerHeight = -1000000000;
    higherHeight = 1000000000;

    for (auto it = levelHeights.begin(); it != levelHeights.end(); ++it)
    {
      // std::cout << "LEVEL HEIGHTS: " << it->first << " : " << it->second << "\n";

      if (it->second <= height && it->second > lowerHeight)
      {
        lowerPressure = it->first;
        lowerHeight = it->second;
      }

      if (it->second >= height && it->second < higherHeight)
      {
        higherPressure = it->first;
        higherHeight = it->second;
      }
    }

    if (lowerHeight != -1000000000 && higherHeight != 1000000000) return true;

    return false;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, exception_operation_failed, nullptr);
  }
}

}  // namespace QueryServer
}  // namespace SmartMet
