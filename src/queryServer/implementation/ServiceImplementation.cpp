#include "ServiceImplementation.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/InterpolationFunctions.h>
#include <grid-files/common/ShowFunction.h>
#include <grid-files/identification/GridDef.h>
#include <macgyver/StringConversion.h>
#include <macgyver/TimeParser.h>
#include <macgyver/Astronomy.h>
#include <macgyver/CharsetTools.h>
#include <boost/functional/hash.hpp>

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
    mGenerationInfoListUpdateTime = 0;
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





void ServiceImplementation::init(
    ContentServer::ServiceInterface* contentServerPtr,
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
      //std::cout << "MAPPING : " << *it << "\n";
      mParameterMappings.push_back(mapping);
    }

    for (auto it = mParameterMappings.begin(); it != mParameterMappings.end(); ++it)
    {
      it->init();
      //it->print(std::cout,0,0);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, exception_operation_failed, nullptr);
  }
}





void ServiceImplementation::setDem(boost::shared_ptr<Fmi::DEM> dem)
{
  try
  {
    mDem = dem;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP, "Operation failed!", nullptr);
    throw exception;
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





bool ServiceImplementation::getProducerInfoByName(std::string& name,T::ProducerInfo& info)
{
  FUNCTION_TRACE
  try
  {
    auto it = mProducerMap.find(name);
    if (it != mProducerMap.end())
    {
      info = it->second;
      return true;
    }

    if (mContentServerPtr->getProducerInfoByName(0, name, info) == 0)
    {
      mProducerMap.insert(std::pair<std::string,T::ProducerInfo>(name,info));
      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, exception_operation_failed, nullptr);
  }
}





void ServiceImplementation::getGenerationInfoListByProducerId(uint producerId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if ((time(0) - mGenerationInfoListUpdateTime) > 120)
    {
      if (mContentServerPtr->getGenerationInfoList(0,mGenerationInfoList) == 0)
      {
        mGenerationInfoList.sort(T::GenerationInfo::ComparisonMethod::producerId);
        mGenerationInfoListUpdateTime = time(0);
      }
    }

    mGenerationInfoList.getGenerationInfoListByProducerId(producerId,generationInfoList);
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
      PRINT_DATA(mDebugLog, "\n############################## QUERY EXECUTION START ##################\n\n");
      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
    }

    int result = 0;
    switch (query.mSearchType)
    {
      case Query::SearchType::TimeSteps:
        result = executeTimeStepQuery(query);
        break;

      case Query::SearchType::TimeRange:
        result = executeTimeRangeQuery(query);
        break;

      default:
        result = executeTimeStepQuery(query);
        break;
    }

    if (mDebugLog != nullptr)
    {
      PRINT_DATA(mDebugLog, "\n############################### QUERY EXECUTION END ###################\n\n");
    }

    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void convertCoordinatesToLatLon(std::string urn,T::AreaCoordinates& coordinates,T::AreaCoordinates& latlonCoordinates)
{
  try
  {
    OGRSpatialReference sr_latlon;
    sr_latlon.importFromEPSG(4326);

    OGRSpatialReference sr;
    if (sr.importFromURN(urn.c_str()) != OGRERR_NONE)
      throw SmartMet::Spine::Exception(BCP, "Invalid crs '" + urn + "'!");

    OGRCoordinateTransformation *tranformation = OGRCreateCoordinateTransformation(&sr,&sr_latlon);
    if (tranformation == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Cannot create coordinate transformation!");

    for (auto cc = coordinates.begin(); cc != coordinates.end(); ++cc)
    {
      T::Coordinate_vec latLon;
      for (auto c = cc->begin(); c != cc->end(); ++c)
      {
        double lon = c->x();
        double lat = c->y();

        tranformation->Transform(1,&lon,&lat);
        latLon.push_back(T::Coordinate(lon,lat));
      }
      latlonCoordinates.push_back(latLon);
    }

    if (tranformation != nullptr)
      OCTDestroyCoordinateTransformation(tranformation);
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
      if (it->first != prev)
        producerList.push_back(it->first);

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
          if (*p == '"')
            ind = !ind;

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
          mProducerList.push_back(std::pair<std::string, T::GeometryId>(std::string(field[0]), toInt64(field[1])));
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





void ServiceImplementation::getGeometryIdListByCoordinates(T::AreaCoordinates& coordinates, std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    std::map < T::GeometryId, uint > countList;
    uint maxCount = 0;

    for (auto cList = coordinates.begin(); cList != coordinates.end(); ++cList)
    {
      for (auto coordinate = cList->begin(); coordinate != cList->end(); ++coordinate)
      {
        std::set < T::GeometryId > idList;
        Identification::gridDef.getGeometryIdListByLatLon(coordinate->y(), coordinate->x(), idList);

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





void ServiceImplementation::getGeometryIdListByCoordinates(uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& coordinates, std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    std::map <T::GeometryId,uint> countList;
    uint maxCount = 0;

    if (coordinates.size() == 0)
      return;

    for (auto coordinate = coordinates.begin(); coordinate != coordinates.end(); ++coordinate)
    {
      std::set < T::GeometryId > idList;
      Identification::gridDef.getGeometryIdListByLatLon(coordinate->y(), coordinate->x(), idList);

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

          countList.insert(std::pair<T::GeometryId, uint>(*g, 1));
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





void ServiceImplementation::getGeometryIdListByGeometryId(T::GeometryId gridGeometryId,std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    geometryIdList.insert(gridGeometryId);

    std::vector<T::Coordinate> coordinates;

    uint width = 0;
    uint height = 0;

    GRIB1::GridDef_ptr def1 = Identification::gridDef.getGrib1DefinitionByGeometryId(gridGeometryId);
    if (def1 != nullptr)
    {
      T::Dimensions d = def1->getGridDimensions();
      width = d.nx();
      height = d.ny();

      double lat = 0;
      double lon = 0;

      for (uint x=0; x<width; x=x+100)
      {
        def1->getGridLatLonCoordinatesByGridPoint(x,0,lat,lon);
        coordinates.push_back(T::Coordinate(lon,lat));

        def1->getGridLatLonCoordinatesByGridPoint(x,height-1,lat,lon);
        coordinates.push_back(T::Coordinate(lon,lat));
      }

      for (uint y=0; y<height; y=y+100)
      {
        def1->getGridLatLonCoordinatesByGridPoint(0,y,lat,lon);
        coordinates.push_back(T::Coordinate(lon,lat));

        def1->getGridLatLonCoordinatesByGridPoint(width-1,y,lat,lon);
        coordinates.push_back(T::Coordinate(lon,lat));
      }


      def1->getGridLatLonCoordinatesByGridPoint(width-1,0,lat,lon);
      coordinates.push_back(T::Coordinate(lon,lat));

      def1->getGridLatLonCoordinatesByGridPoint(width-1,height-1,lat,lon);
      coordinates.push_back(T::Coordinate(lon,lat));

      getGeometryIdListByCoordinates(width,height,coordinates,geometryIdList);
      return;
    }

    GRIB2::GridDef_ptr def2 = Identification::gridDef.getGrib2DefinitionByGeometryId(gridGeometryId);
    if (def2 != nullptr)
    {
      T::Dimensions d = def2->getGridDimensions();
      width = d.nx();
      height = d.ny();

      double lat = 0;
      double lon = 0;

      for (uint x=0; x<width; x=x+100)
      {
        def2->getGridLatLonCoordinatesByGridPoint(x,0,lat,lon);
        coordinates.push_back(T::Coordinate(lon,lat));

        def2->getGridLatLonCoordinatesByGridPoint(x,height-1,lat,lon);
        coordinates.push_back(T::Coordinate(lon,lat));
      }

      for (uint y=0; y<height; y=y+100)
      {
        def2->getGridLatLonCoordinatesByGridPoint(0,y,lat,lon);
        coordinates.push_back(T::Coordinate(lon,lat));

        def2->getGridLatLonCoordinatesByGridPoint(width-1,y,lat,lon);
        coordinates.push_back(T::Coordinate(lon,lat));
      }


      def2->getGridLatLonCoordinatesByGridPoint(width-1,0,lat,lon);
      coordinates.push_back(T::Coordinate(lon,lat));

      def2->getGridLatLonCoordinatesByGridPoint(width-1,height-1,lat,lon);
      coordinates.push_back(T::Coordinate(lon,lat));


      getGeometryIdListByCoordinates(width,height,coordinates,geometryIdList);
      return;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, exception_operation_failed, nullptr);
  }
}





bool ServiceImplementation::getParameterFunctionInfo(std::string paramStr, std::string& function, std::string& functionParams)
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





bool ServiceImplementation::getFunctionParams(std::string functionParamsStr, FunctionParam_vec& functionParams)
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

      if (*p == '}')
        level--;

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





void ServiceImplementation::getParameterMappings(std::string producerName,std::string parameterName,T::GeometryId geometryId, bool onlySearchEnabled, ParameterMapping_vec& mappings)
{
  try
  {
    std::string key = producerName + ":" + parameterName + ":" + std::to_string(geometryId) + ":" + std::to_string(C_INT(onlySearchEnabled));

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
      m->getMappings(producerName, parameterName, geometryId, onlySearchEnabled, mappings);
      //printf("Get mappings [%s][%s][%d] %lu\n",producerName.c_str(),parameterName.c_str(),geometryId,mappings.size());
    }

    mParameterMappingCache.push_front(std::pair<std::string, ParameterMapping_vec>(key, mappings));
    // printf("Cache add %s %d\n",key.c_str(),mParameterMappingCache.size());
    if (mParameterMappingCache.size() > 20)
      mParameterMappingCache.pop_back();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::getParameterMappings(
    std::string producerName,
    std::string parameterName,
    T::GeometryId geometryId,
    T::ParamLevelIdType levelIdType,
    T::ParamLevelId levelId,
    T::ParamLevel level,
    bool onlySearchEnabled,
    ParameterMapping_vec& mappings)
{
  try
  {
    std::string key = producerName + ":" + parameterName + ":" + std::to_string(geometryId) + ":" + std::to_string(levelIdType) + ":" + std::to_string(levelId) + ":" + std::to_string(level) + ":"
        + std::to_string(onlySearchEnabled);

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
      m->getMappings(producerName, parameterName, geometryId, levelIdType, levelId, level, onlySearchEnabled, mappings);
      //printf("Get mappings [%s][%s][%d] [%d][%d][%d] %lu\n",producerName.c_str(),parameterName.c_str(),geometryId,levelIdType, levelId, level,mappings.size());
    }

    mParameterMappingCache.push_front(std::pair<std::string, ParameterMapping_vec>(key, mappings));
    // printf("Cache add %s %d\n",key.c_str(),mParameterMappingCache.size());

    if (mParameterMappingCache.size() > 20)
      mParameterMappingCache.pop_back();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::getParameterStringInfo(
    std::string param,
    std::string& key,
    T::GeometryId& geometryId,
    T::ParamLevelId& paramLevelId,
    T::ParamLevel& paramLevel,
    T::ForecastType& forecastType,
    T::ForecastNumber& forecastNumber,
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
    std::vector < T::ForecastNumber > forecastNumberVec;
    getParameterStringInfo(param, key, geometryId,paramLevelId, paramLevel, forecastType, forecastNumberVec, producerName, producerId, generationFlags, areaInterpolationMethod,
        timeInterpolationMethod, levelInterpolationMethod);

    if (forecastNumberVec.size() > 0)
      forecastNumber = forecastNumberVec[0];

  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::getParameterStringInfo(
    std::string param,
    std::string& key,
    T::GeometryId& geometryId,
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
      if (*p == '{')
        level++;

      if (*p == '}')
        level--;

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
        if (getProducerInfoByName(producerName, producerInfo))
          producerId = producerInfo.mProducerId;
      }
    }

    if (c > 2)
    {
      if (field[2][0] != '\0')
        geometryId = toInt64(field[2]);
    }

    if (c > 3)
    {
      if (field[3][0] != '\0')
        paramLevelId = toInt64(field[3]);
    }

    if (c > 4)
    {
      if (field[4][0] != '\0')
        paramLevel = toInt64(field[4]);
    }

    if (c > 5)
    {
      if (field[5][0] != '\0')
        forecastType = (T::ForecastType) toInt64(field[5]);
    }

    if (c > 6)
    {
      if (field[6][0] != '\0')
      {
        std::vector < std::string > partList;
        splitString(field[6], '-', partList);
        size_t sz = partList.size();
        if (sz == 1)
        {
          forecastNumberVec.push_back((T::ForecastNumber) toInt64(partList[0].c_str()));
        }
        else if (sz == 2)
        {
          auto start = toInt64(partList[0].c_str());
          auto end = toInt64(partList[1].c_str());
          if (start < end && (end - start) <= 200)
          {
            for (auto t = start; t <= end; t++)
              forecastNumberVec.push_back((T::ForecastNumber) t);
          }
        }
      }
    }

    if (c > 7)
    {
      if (field[7][0] != '\0')
        generationFlags = toInt64(field[7]);
    }

    if (c > 8)
    {
      if (field[8][0] != '\0')
        areaInterpolationMethod = (short) toInt64(field[8]);
    }

    if (c > 9)
    {
      if (field[9][0] != '\0')
        timeInterpolationMethod = (short) toInt64(field[9]);
    }

    if (c > 10)
    {
      if (field[10][0] != '\0')
        levelInterpolationMethod = (short) toInt64(field[10]);
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





bool ServiceImplementation::parseFunction(
    QueryParameter& queryParam,
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
      exception.addDetail("The reason for this is most likely that some aliases refer to each other.");
      exception.addParameter("ParamStr", paramStr);
      throw exception;
    }

    PRINT_DATA(mDebugLog, "%s   + Parse parameter : %s\n", space(recursionCounter).c_str(), paramStr.c_str());
    std::string functionParamsStr;
    if (getParameterFunctionInfo(paramStr, function, functionParamsStr))
    {
      PRINT_DATA(mDebugLog, "%s     - Function: %s\n", space(recursionCounter).c_str(), function.c_str());
      PRINT_DATA(mDebugLog, "%s     - Params  : %s\n", space(recursionCounter).c_str(), functionParamsStr.c_str());

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
          newParam.mGeometryId = queryParam.mGeometryId;
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
            getParameterStringInfo(fParam->second, paramName, newParam.mGeometryId, newParam.mParameterLevelId, newParam.mParameterLevel, newParam.mForecastType, newParam.mForecastNumber,
                newParam.mProducerName,newParam.mProducerId, newParam.mGenerationFlags, newParam.mAreaInterpolationMethod, newParam.mTimeInterpolationMethod, newParam.mLevelInterpolationMethod);
          }
          // newParam.print(std::cout,0,0);

          parseFunction(newParam, newParam.mParam, newParam.mFunction, newParam.mFunctionParams, recursionCounter + 2, additionalParameterList);

          additionalParameterList.insert(additionalParameterList.begin(), newParam);
        }
      }
      return true;
    }
    else
    {
      std::vector < T::ForecastNumber > forecastNumberVec;
      std::string producerName;

      std::string paramName;
      getParameterStringInfo(paramStr, paramName, queryParam.mGeometryId, queryParam.mParameterLevelId, queryParam.mParameterLevel, queryParam.mForecastType, forecastNumberVec, producerName,
          queryParam.mProducerId, queryParam.mGenerationFlags, queryParam.mAreaInterpolationMethod, queryParam.mTimeInterpolationMethod, queryParam.mLevelInterpolationMethod);

      size_t sz = forecastNumberVec.size();
      if (sz == 1)
        queryParam.mForecastNumber = forecastNumberVec[0];
      else if (sz > 1)
      {
        char buf[10000];
        char *p = buf;
        p += sprintf(p, "LIST{");
        for (size_t t = 0; t < sz; t++)
        {
          p += sprintf(p, "%s:%s:%d:%d:%d:%d:%d:%u", paramName.c_str(), producerName.c_str(), queryParam.mGeometryId, queryParam.mParameterLevelId, queryParam.mParameterLevel, queryParam.mForecastType,
              forecastNumberVec[t], queryParam.mGenerationFlags);

          if (queryParam.mAreaInterpolationMethod >= 0)
            p += sprintf(p, ":%d", queryParam.mAreaInterpolationMethod);
          else
            p += sprintf(p, ":");

          if (queryParam.mTimeInterpolationMethod >= 0)
            p += sprintf(p, ":%d", queryParam.mTimeInterpolationMethod);
          else
            p += sprintf(p, ":");

          if (queryParam.mLevelInterpolationMethod >= 0)
            p += sprintf(p, ":%d", queryParam.mLevelInterpolationMethod);
          else
            p += sprintf(p, ":");

          if ((t + 1) < sz)
            p += sprintf(p, ";");
        }
        p += sprintf(p, "}");
        queryParam.mParam = buf;

        return parseFunction(queryParam, queryParam.mParam, function, functionParams, recursionCounter + 1, additionalParameterList);
      }

      std::string alias;
      if (getAlias(paramName, alias))
      {
        if (alias != paramStr)
        {
          if (!parseFunction(queryParam, alias, function, functionParams, recursionCounter + 1, additionalParameterList))
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

    for (auto pName = query.mProducerNameList.begin(); pName != query.mProducerNameList.end(); ++pName)
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





void ServiceImplementation::getProducers(std::string producerName, Producer_vec& producers)
{
  FUNCTION_TRACE
  try
  {
    for (auto it = mProducerList.begin(); it != mProducerList.end(); ++it)
    {
      if (strcasecmp(producerName.c_str(), it->first.c_str()) == 0)
      {
        producers.push_back(std::pair<std::string, T::GeometryId>(it->first, it->second));
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

      for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
      {
        // The parameter can contain some level information even if the parameter is an alias
        // or a function. In this case this level information should be inherited to the alias
        // definition or to the function parameters.

        PRINT_DATA(mDebugLog, " * PARAMETER: %s\n", qParam->mSymbolicName.c_str());

        std::vector < T::ForecastNumber > forecastNumberVec;
        //std::string producerName;

        getParameterStringInfo(qParam->mParam, qParam->mParameterKey, qParam->mGeometryId, qParam->mParameterLevelId, qParam->mParameterLevel, qParam->mForecastType, forecastNumberVec, qParam->mProducerName,
            qParam->mProducerId, qParam->mGenerationFlags, qParam->mAreaInterpolationMethod, qParam->mTimeInterpolationMethod, qParam->mLevelInterpolationMethod);

        size_t sz = forecastNumberVec.size();
        if (sz == 1)
          qParam->mForecastNumber = forecastNumberVec[0];
        else if (sz > 1)
        {
          char buf[10000];
          char *p = buf;
          p += sprintf(p, "LIST{");
          for (size_t t = 0; t < sz; t++)
          {
            p += sprintf(p, "%s:%s:%d:%d:%d:%d:%d:%u", qParam->mParameterKey.c_str(), qParam->mProducerName.c_str(), qParam->mGeometryId, qParam->mParameterLevelId, qParam->mParameterLevel, qParam->mForecastType,
                forecastNumberVec[t], qParam->mGenerationFlags);

            if (qParam->mAreaInterpolationMethod >= 0)
              p += sprintf(p, ":%d", qParam->mAreaInterpolationMethod);
            else
              p += sprintf(p, ":");

            if (qParam->mTimeInterpolationMethod >= 0)
              p += sprintf(p, ":%d", qParam->mTimeInterpolationMethod);
            else
              p += sprintf(p, ":");

            if (qParam->mLevelInterpolationMethod >= 0)
              p += sprintf(p, ":%d", qParam->mLevelInterpolationMethod);
            else
              p += sprintf(p, ":");

            if ((t + 1) < sz)
              p += sprintf(p, ";");
          }
          p += sprintf(p, "}");
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
          parseFunction(*qParam, qParam->mParam, qParam->mFunction, qParam->mFunctionParams, 0, additionalParameterList);
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

    for (auto qParam = query.mQueryParameterList.rbegin(); qParam != query.mQueryParameterList.rend(); ++qParam)
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

            for (auto it = qParam->mFunctionParams.begin(); it != qParam->mFunctionParams.end(); ++it)
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

                  T::GridValue rec;
                  if (q->mValueList[tCount].mValueList.getGridValueByIndex(v,rec))
                  {
                    if (/*qParam->mAreaInterpolationMethod == T::AreaInterpolationMethod::External
                     &&*/
                    rec.mValueString.length() > 0)
                    {
                      splitString(rec.mValueString, ';', valueList);
                    }
                    else
                    {
                      a = rec.mValue;
                    }

                    if (rec.mX == -1000 && rec.mY == -1000)
                    {
                      areaCnt = true;
                    }
                    else
                    {
                      lastRec = rec;
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

                if (areaCnt && v == 0)
                  extParameters.push_back(a);
              }
              else
              {
                for (auto aa = valueList.begin(); aa != valueList.end(); ++aa)
                {
                  //printf("-- PUSH %f\n",*aa);
                  parameters.push_back(*aa);
                  areaParameters.push_back(*aa);

                  if (areaCnt && v == 0)
                    extParameters.push_back(*aa);
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
                  T::GridValue rec(lastRec.mX, lastRec.mY, val);
                  pValues.mValueList.addGridValue(rec);
                }
                  break;

                case 5:
                {
                  std::string val = mLuaFileCollection.executeFunctionCall5(function, query.mLanguage, parameters);
                  // std::cout << "** result " << val << "\n";
                  T::GridValue rec(lastRec.mX, lastRec.mY, val);
                  pValues.mValueList.addGridValue(rec);
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
            T::GridValue rec(-1000, -1000, val);
            pValues.mValueList.addGridValue(rec);
          }

          if (qParam->mFunction.substr(0, 1) == "@")
          {
            // T::ParamValue val = executeAreaFunction(qParam->mFunction,areaParameters);
            std::string func = qParam->mFunction.substr(1);
            double val = mLuaFileCollection.executeFunctionCall1(func, areaParameters);
            // double val = executeFunctionCall1(func,areaParameters);
            T::GridValue rec(-1000, -1000, val);
            pValues.mValueList.addGridValue(rec);
          }

          if (areaCnt)
          {
            if (pValues.mValueList.getLength() > 0)
            {
              T::GridValue rec;
              pValues.mValueList.getGridValueByIndex(0,rec);
              T::ParamValue val = rec.mValue;
              pValues.mValueList.clear();
              T::GridValue rec2(-1000, -1000, val);
              pValues.mValueList.addGridValue(rec2);
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
    T::Coordinate_vec coordinates;

    if (producers.size() == 0)
      return Result::NO_PRODUCERS_FOUND;

    // Getting geometries that support support the given coordinates.

    std::set < T::GeometryId > geometryIdList;

    if (query.mGeometryIdList.size() > 0)
    {
      geometryIdList = query.mGeometryIdList;
    }
    else
    {
      const char *geometryIdStr = query.mAttributeList.getAttributeValue("grid.geometryId");
      if (geometryIdStr != nullptr)
      {
        getGeometryIdListByGeometryId(toInt32(geometryIdStr),geometryIdList);
      }
      else
      {
        const char *gridUrnStr = query.mAttributeList.getAttributeValue("grid.urn");
        if (gridUrnStr != nullptr)
        {
          Identification::gridDef.getGeometryIdList(geometryIdList);
        }
        else
        {
          getGeometryIdListByCoordinates(query.mAreaCoordinates, geometryIdList);
        }
      }
    }

    std::map < std::string, uint > parameterProducers;

    // Parsing parameters and functions in the query.

    updateQueryParameters(query);

    if (mDebugLog != nullptr)
    {
      std::stringstream stream;
      query.print(stream, 0, 0);
      PRINT_DATA(mDebugLog, "\nQuery -object after parameter update:\n");
      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
    }

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
        std::string producerName = qParam->mProducerName;
        uint generationFlags = qParam->mGenerationFlags;
        int geometryId = qParam->mGeometryId;
        if (generationFlags == 0)
          generationFlags = query.mGenerationFlags;

        // qParam->print(std::cout,0,0);

        getParameterStringInfo(qParam->mParam, paramName, geometryId, paramLevelId, paramLevel, forecastType, forecastNumber, producerName,producerId, generationFlags, areaInterpolationMethod,
            timeInterpolationMethod, levelInterpolationMethod);

        // printf("**** PARAM %s  => %d %d\n",qParam->mParam.c_str(),paramLevelId,paramLevel);

        if (paramName.substr(0, 1) == "$")
          paramName = paramName.substr(1);

        try
        {
          if (qParam->mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN)
          {
            std::string startTime = query.mStartTime;
            std::string endTime = query.mEndTime;

            if (qParam->mTimestepsBefore > 0)
            {
              auto ss = boost::posix_time::from_iso_string(startTime) - boost::posix_time::seconds(((qParam->mTimestepsBefore + 1) * qParam->mTimestepSizeInMinutes * 60));
              startTime = Fmi::to_iso_string(ss);

              // time_t ss = utcTimeToTimeT(startTime) - ((qParam->mTimestepsBefore+1) *
              // qParam->mTimestepSizeInMinutes * 60); startTime = utcTimeFromTimeT(ss);
            }

            if (qParam->mTimestepsAfter > 0)
            {
              auto ss = boost::posix_time::from_iso_string(endTime) - boost::posix_time::seconds((qParam->mTimestepsAfter * qParam->mTimestepSizeInMinutes * 60));
              endTime = Fmi::to_iso_string(ss);

              // time_t ss = utcTimeToTimeT(endTime) + (qParam->mTimestepsAfter *
              // qParam->mTimestepSizeInMinutes * 60); endTime = utcTimeFromTimeT(ss);
            }

            if ((query.mFlags & Query::Flags::StartTimeFromData) != 0)
            {
              startTime = "15000101T000000";  // Start time is the start time of the data
            }

            if ((query.mFlags & Query::Flags::EndTimeFromData) != 0)
            {
              endTime = "30000101T000000";    // End time is the end time of the data
            }

            if (producerId == 0)
            {
              auto it = parameterProducers.find(paramName + ":" + producerName);
              if (it != parameterProducers.end())
                producerId = it->second;
            }

            Producer_vec tmpProducers;
            if (producerName > " ")
            {
              if (geometryId > 0)
                tmpProducers.push_back(std::pair<std::string, T::GeometryId>(producerName, geometryId));
              else
                getProducers(producerName,tmpProducers);
            }
            else
              tmpProducers = producers;

            std::set < T::GeometryId > geomIdList;
            if (geometryId > 0)
              geomIdList.insert(geometryId);
            else
              geomIdList = geometryIdList;

            getGridValues(qParam->mType,tmpProducers, geomIdList, producerId, query.mAnalysisTime, generationFlags, paramName, paramLevelId, paramLevel, forecastType,
                forecastNumber, parameterFlags, areaInterpolationMethod, timeInterpolationMethod, levelInterpolationMethod, startTime, endTime, query.mFlags, query.mTimesteps,
                query.mTimestepSizeInMinutes,qParam->mLocationType, query.mCoordinateType, query.mAreaCoordinates, qParam->mContourLowValues, qParam->mContourHighValues, query.mAttributeList,
                query.mRadius, query.mMaxParameterValues, qParam->mPrecision, qParam->mValueList);

            if (qParam->mValueList.size() > 0)
            {
              if (producerId == 0 && qParam->mValueList[0].mProducerId != 0)
              {
                parameterProducers.insert(std::pair<std::string, uint>(paramName + ":" + producerName, qParam->mValueList[0].mProducerId));
              }

              uint len = qParam->mValueList[0].mValueList.getLength();
              if (coordinates.size() == 0  &&  len > 0)
              {
                for (uint s = 0; s<len; s++)
                {
                  T::GridValue *val = qParam->mValueList[0].mValueList.getGridValuePtrByIndex(s);
                  coordinates.push_back(T::Coordinate(val->mX,val->mY));
                }
              }
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

    std::set < std::string > timeList;
    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
    {
      for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end(); ++it)
      {
        if (timeList.find(it->mForecastTime) == timeList.end())
          timeList.insert(it->mForecastTime);
      }
    }

    // Going through all the found forecast times and making sure that each parameter contains the
    // current forecast time. If not, then the forecast time is added to the parameters's forecast
    // time list, but the actual value list of the current forecast time will be empty.

    size_t timeCnt = timeList.size();
    for (auto tt = timeList.begin(); tt != timeList.end(); ++tt)
    {
      query.mForecastTimeList.insert(*tt);

      for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
      {
        if (qParam->mFunction.length() == 0)
        {
          bool found = false;
          uint cnt = 0;

          if (qParam->mValueList.size() < timeCnt)
            qParam->mValueList.reserve(timeCnt);

          for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end() && !found; ++it)
          {
            if ((query.mFlags & Query::Flags::StartTimeFromData) == 0
                && (it->mForecastTime < query.mStartTime || it->mForecastTime > query.mEndTime))
              it->mFlags = it->mFlags | QueryServer::QPF_AGGREGATION_VALUE;

            if (it->mForecastTime < *tt)
              cnt++;
            else
            if (it->mForecastTime == *tt)
            {
              found = true;

              if (it->mValueList.getLength() == 0)
              {
                getAdditionalValues(qParam->mSymbolicName,query.mCoordinateType,coordinates,*it);
              }
            }
          }

          if (!found)
          {
            // The forecast time was not found from the current parameter. Adding the forecast time
            // with an empty value list.

            ParameterValues pValues;
            pValues.mForecastTime = *tt;

            if (qParam->mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN)
            {
              pValues.mFlags = pValues.mFlags | QueryServer::QPF_ADDITIONAL_VALUE;
            }

            getAdditionalValues(qParam->mSymbolicName,query.mCoordinateType,coordinates,pValues);

            if ((query.mFlags & Query::Flags::StartTimeFromData) == 0 && (*tt < query.mStartTime || *tt > query.mEndTime))
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
    T::Coordinate_vec coordinates;

    if (producers.size() == 0)
      return Result::NO_PRODUCERS_FOUND;

    std::set < T::GeometryId > geometryIdList;

    if (query.mGeometryIdList.size() > 0)
    {
      geometryIdList = query.mGeometryIdList;
    }
    else
    {
      const char *geometryIdStr = query.mAttributeList.getAttributeValue("grid.geometryId");
      if (geometryIdStr != nullptr)
      {
        getGeometryIdListByGeometryId(toInt32(geometryIdStr),geometryIdList);
      }
      else
      {
        const char *gridUrnStr = query.mAttributeList.getAttributeValue("grid.urn");
        if (gridUrnStr != nullptr)
        {
          Identification::gridDef.getGeometryIdList(geometryIdList);
        }
        else
        {
          getGeometryIdListByCoordinates(query.mAreaCoordinates, geometryIdList);
        }
      }
    }

    std::map < std::string, uint > parameterProducers;

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
    if ((query.mFlags & Query::Flags::ReverseGenerationFlags) != 0)
      reverseGenerations = true;

    // Fetching parameter data according to the given timesteps and the coordinate list. Notice
    // that the coordinate list can be used in two ways. It can 1) contain coordinate points
    // where the data will be fetched or 2) it can define an area (= polygon) where the grid
    // points are fetched. The area can be defined by using single or multiple polygons.
    // That's why the coordinates are defined as a vector of coordinate vectors.

    std::set < std::string > timeList;
    for (auto fTime = query.mForecastTimeList.begin(); fTime != query.mForecastTimeList.end(); ++fTime)
    {
      timeList.insert(*fTime);
    }

    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
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
        std::string producerName = qParam->mProducerName;
        uint producerId = qParam->mProducerId;
        uint generationFlags = qParam->mGenerationFlags;
        int geometryId = qParam->mGeometryId;
        if (generationFlags == 0)
          generationFlags = query.mGenerationFlags;

        getParameterStringInfo(qParam->mParam, paramName, geometryId,paramLevelId, paramLevel, forecastType, forecastNumber, producerName, producerId, generationFlags, areaInterpolationMethod,
            timeInterpolationMethod, levelInterpolationMethod);

        if (paramName.substr(0, 1) == "$")
          paramName = paramName.substr(1);

        std::set < std::string > forecastTimeList = timeList;
        std::set < std::string > additionalTimeList;

        if (qParam->mTimestepsBefore > 0 || qParam->mTimestepsAfter > 0)
        {
          for (auto fTime = timeList.begin(); fTime != timeList.end(); ++fTime)
          {
            if (qParam->mTimestepsBefore > 0)
            {
              // time_t ss = utcTimeToTimeT(*fTime) - ((qParam->mTimestepsBefore+1) *
              // qParam->mTimestepSizeInMinutes * 60);
              auto ss = boost::posix_time::from_iso_string(*fTime) - boost::posix_time::seconds(((qParam->mTimestepsBefore + 1) * qParam->mTimestepSizeInMinutes * 60));

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

        // Processing time steps in reverse order. The idea is that we want to
        // find a generation that contains the newest timestep if the generation
        // is not defined. It is possible that the newest generation do not contain
        // the requested timestep when the generation is still under construction.

        uint gflags = generationFlags;

        for (auto fTime = forecastTimeList.rbegin(); fTime != forecastTimeList.rend(); ++fTime)
        {
          ParameterValues valueList;
          try
          {
            if (producerId == 0)
            {
              auto it = parameterProducers.find(paramName + ":" + producerName);
              if (it != parameterProducers.end())
                producerId = it->second;
            }

            if (qParam->mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN)
            {

              Producer_vec tmpProducers;
              if (producerName > " ")
              {
                if (geometryId > 0)
                  tmpProducers.push_back(std::pair<std::string, T::GeometryId>(producerName, geometryId));
                else
                  getProducers(producerName,tmpProducers);
              }
              else
                tmpProducers = producers;

              std::set < T::GeometryId > geomIdList;
              if (geometryId > 0)
                geomIdList.insert(geometryId);
              else
                geomIdList = geometryIdList;


              getGridValues(qParam->mType,tmpProducers, geomIdList, producerId, query.mAnalysisTime, gflags, reverseGenerations, paramName, paramLevelId, paramLevel, forecastType,
                  forecastNumber, parameterFlags, areaInterpolationMethod, timeInterpolationMethod, levelInterpolationMethod, *fTime, false, qParam->mLocationType,
                  query.mCoordinateType, query.mAreaCoordinates, qParam->mContourLowValues, qParam->mContourHighValues, query.mAttributeList,query.mRadius,qParam->mPrecision,valueList);

              if (producerId == 0 && valueList.mProducerId != 0)
              {
                parameterProducers.insert(std::pair<std::string, uint>(paramName + ":" + producerName, valueList.mProducerId));
              }

              if (valueList.mValueList.getLength() > 0)
              {
                // We have got some values. We should not change the producer anymore.

                if (producerId == 0)
                  producerId = valueList.mProducerId;

                // We should use this generation or older.
                if (generationFlags == 0)
                  gflags = valueList.mGenerationFlags + (valueList.mGenerationFlags << 1);

                // We should not change the forecast number.
                if (forecastType < 0  &&  forecastNumber < 0)
                {
                  forecastType = valueList.mForecastType;
                  forecastNumber = valueList.mForecastNumber;
                }

                uint len = valueList.mValueList.getLength();
                if (coordinates.size() == 0  &&  len > 0)
                {
                  for (uint s = 0; s<len; s++)
                  {
                    T::GridValue *val = valueList.mValueList.getGridValuePtrByIndex(s);
                    coordinates.push_back(T::Coordinate(val->mX,val->mY));
                  }
                }

              }
            }
          }
          catch (...)
          {
            SmartMet::Spine::Exception exception(BCP, "Operation failed!", nullptr);
            exception.printError();
          }

          if (valueList.mValueList.getLength() == 0)
            valueList.mForecastTime = *fTime;

          if (additionalTimeList.find(*fTime) != additionalTimeList.end())
            valueList.mFlags = valueList.mFlags | QueryServer::QPF_AGGREGATION_VALUE;

          qParam->mValueList.insert(qParam->mValueList.begin(),valueList);
          //qParam->mValueList.push_back(valueList);
        }
      }
    }

    // Finding out which forecast time are found from the forecast data. The point is that different
    // parameters might contain different forecast times, and we want a list of all forecast times.

    timeList.clear();
    std::set < std::string > additionalTimeList;

    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
    {
      for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end(); ++it)
      {
        if (timeList.find(it->mForecastTime) == timeList.end())
          timeList.insert(it->mForecastTime);

        if ((it->mFlags & QueryServer::QPF_AGGREGATION_VALUE) != 0 && additionalTimeList.find(it->mForecastTime) == additionalTimeList.end())
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

      for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
      {
        if (qParam->mFunction.length() == 0)
        {
          bool found = false;
          uint cnt = 0;
          for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end() && !found; ++it)
          {
            if (it->mForecastTime < *tt)
              cnt++;
            else
            if (it->mForecastTime == *tt)
            {
              found = true;
              if (it->mValueList.getLength() == 0)
              {
                getAdditionalValues(qParam->mSymbolicName,query.mCoordinateType,coordinates,*it);
              }
            }
          }

          if (!found)
          {
            // The forecast time was not found from the current parameter. Adding the forecast time
            // with an empty value list.

            ParameterValues pValues;
            pValues.mForecastTime = *tt;

            if (qParam->mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN)
              pValues.mFlags = pValues.mFlags | QueryServer::QPF_ADDITIONAL_VALUE;

            getAdditionalValues(qParam->mSymbolicName,query.mCoordinateType,coordinates,pValues);

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





int ServiceImplementation::_getValuesByGridPoint(
    T::SessionId sessionId,
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
    uint len = contentInfoList.getLength();
    for (uint c = 0; c < len; c++)
    {
      T::ContentInfo* contentInfo = contentInfoList.getContentInfoByIndex(c);
      T::ParamValue value = 0;
      int res = mDataServerPtr->getGridValueByPoint(sessionId, contentInfo->mFileId, contentInfo->mMessageIndex, coordinateType, x, y, areaInterpolationMethod, value);
      if (res == DataServer::Result::OK)
      {
        valueList.addGridPointValue(new T::GridPointValue(contentInfo->mFileId, contentInfo->mMessageIndex, x, y, contentInfo->mParameterLevel, contentInfo->mForecastTime, value));
      }
      else
      {
        valueList.addGridPointValue(new T::GridPointValue(contentInfo->mFileId, contentInfo->mMessageIndex, x, y, contentInfo->mParameterLevel, contentInfo->mForecastTime, ParamValueMissing));
      }
    }
    return 0;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, exception_operation_failed, nullptr);
  }
}





bool ServiceImplementation::conversionFunction(std::string& conversionFunction, std::string& function, std::vector<std::string>& functionParams)
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





void ServiceImplementation::executeConversion(std::string& function, std::vector<std::string>& functionParams, T::ParamValue_vec& valueList, T::ParamValue_vec& newValueList)
{
  try
  {
    newValueList.reserve(valueList.size());
    for (auto v = valueList.begin(); v != valueList.end(); ++v)
    {
      std::vector<double> parameters;
      for (auto fp = functionParams.begin(); fp != functionParams.end(); ++fp)
      {
        if (*fp == "$")
          parameters.push_back(*v);
        else
          parameters.push_back(toDouble(fp->c_str()));
      }
      T::ParamValue newValue = mLuaFileCollection.executeFunctionCall1(function, parameters);
      newValueList.push_back(newValue);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::executeConversion(std::string& function, std::vector<std::string>& functionParams, T::GridValueList& valueList)
{
  try
  {
    uint vLen = valueList.getLength();
    for (uint i = 0; i < vLen; i++)
    {
      auto gv = valueList.getGridValuePtrByIndex(i);
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





bool ServiceImplementation::getSpecialValues(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    uint generationFlags,
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
    uchar coordinateType,
    double x,
    double y,
    uint& newProducerId,
    short& precision,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    std::string startTime = forecastTime;
    std::string endTime = forecastTime;

    std::string function;
    std::vector < std::string > functionParams;
    bool conversionByFunction = conversionFunction(pInfo.mConversionFunction, function, functionParams);

    T::ContentInfoList contentList;
    int result = mContentServerPtr->getContentListByParameterGenerationIdAndForecastTime(0, generationId, pInfo.mParameterKeyType, pInfo.mParameterKey, pInfo.mParameterLevelIdType,
        paramLevelId, paramLevel, forecastType, forecastNumber, producerGeometryId, forecastTime, contentList);
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
      PRINT_DATA(mDebugLog, "         + Content records contains multiple values with the same timestep\n");
      return false;
    }

    if (contentLen == 0)
      return false;

    T::ContentInfo* contentInfo1 = contentList.getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList.getContentInfoByIndex(1);

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelIdType = pInfo.mParameterLevelIdType;
    valueList.mParameterLevelId = paramLevelId;
    valueList.mForecastTime = forecastTime;
    valueList.mProducerId = contentInfo1->mProducerId;
    valueList.mGenerationId = contentInfo1->mGenerationId;
    valueList.mGeometryId = contentInfo1->mGeometryId;
    valueList.mModificationTime = contentInfo1->mModificationTime;
    valueList.mForecastType = contentInfo1->mForecastType;
    valueList.mForecastNumber = contentInfo1->mForecastNumber;
    valueList.mParameterLevel = contentInfo1->mParameterLevel;
    if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
      valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;
    else
      valueList.mParameterLevelId = pInfo.mParameterLevelId;

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTime == forecastTime || timeInterpolationMethod == T::TimeInterpolationMethod::None || timeInterpolationMethod == T::TimeInterpolationMethod::Nearest)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        double_vec valueVector;
        int result = mDataServerPtr->getGridValueVectorByPoint(0, contentInfo1->mFileId, contentInfo1->mMessageIndex, coordinateType, x, y, areaInterpolationMethod, valueVector);
        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueVectorByPoint");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
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
          {
            T::GridValue rec(x, y, valueVector[vv]);
            list.addGridValue(rec);
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, list);

          for (uint vv = 0; vv <= sLen; vv++)
            p += sprintf(p, "%f;", valueVector[vv]);

          uint len = list.getLength();
          for (uint vv = 0; vv < len; vv++)
          {
            T::GridValue rec;
            list.getGridValueByIndex(vv,rec);
            p += sprintf(p, "%f;", rec.mValue);
          }

          T::GridValue rec(x, y, std::string(tmp));
          valueList.mValueList.addGridValue(rec);
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

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
      if (!(contentInfo1->mForecastTime < forecastTime && contentInfo2->mForecastTime > forecastTime))
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

      // Fetching data from the grids.

      double_vec valueVector1;
      double_vec valueVector2;

      int result1 = mDataServerPtr->getGridValueVectorByPoint(0, contentInfo1->mFileId, contentInfo1->mMessageIndex, coordinateType, x, y, areaInterpolationMethod, valueVector1);
      if (result1 != 0)
      {
        SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
        exception.addParameter("Service", "getGridValueBlockByPoint");
        exception.addParameter("Message", DataServer::getResultString(result1));
        std::string errorMsg = exception.getStackTrace();
        PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
        return false;
      }

      int result2 = mDataServerPtr->getGridValueVectorByPoint(0, contentInfo2->mFileId, contentInfo2->mMessageIndex, coordinateType, x, y, areaInterpolationMethod, valueVector2);
      if (result2 != 0)
      {
        SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
        exception.addParameter("Service", "getGridValueVectorByPoint");
        exception.addParameter("Message", DataServer::getResultString(result2));
        std::string errorMsg = exception.getStackTrace();
        PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
        return false;
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
          T::GridValue val1(x, y, valueVector1[vv]);
          T::GridValue val2(x, y, valueVector2[vv]);
          list1.addGridValue(val1);
          list2.addGridValue(val2);
        }

        T::GridValueList list;
        timeInterpolation(list1,list2,contentInfo1->mForecastTime, contentInfo2->mForecastTime,forecastTime,timeInterpolationMethod,list);
        //timeInterpolation(timeInterpolationMethod, forecastTime, contentInfo1->mForecastTime, contentInfo2->mForecastTime, list1, list2, list);

        if (conversionByFunction)
          executeConversion(function, functionParams, list);

        for (uint s = 0; s <= sLen; s++)
          p += sprintf(p, "%f;", valueVector1[s]);

        uint len = list.getLength();
        for (uint s = 0; s < len; s++)
        {
          T::GridValue rec;
           list.getGridValueByIndex(s,rec);
          p += sprintf(p, "%f;", rec.mValue);
        }

        T::GridValue rec(x, y, std::string(tmp));
        valueList.mValueList.addGridValue(rec);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

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





bool ServiceImplementation::getValueVectors(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    uint generationFlags,
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
    uchar locationType,
    uchar coordinateType,
    T::Coordinate_vec& gridCoordinates,
    T::AttributeList& attributeList,
    uint& newProducerId,
    short& precision,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    std::string function;
    std::vector < std::string > functionParams;
    bool conversionByFunction = conversionFunction(pInfo.mConversionFunction, function, functionParams);
    T::ParamValue_vec valueVector;

    std::string startTime = forecastTime;
    std::string endTime = forecastTime;

    T::ContentInfoList contentList;
    int result = mContentServerPtr->getContentListByParameterGenerationIdAndForecastTime(0, generationId, pInfo.mParameterKeyType, pInfo.mParameterKey, pInfo.mParameterLevelIdType,
        paramLevelId, paramLevel, forecastType, forecastNumber, producerGeometryId, forecastTime, contentList);

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
      PRINT_DATA(mDebugLog, "         + Content records contains multiple values with the same timestep\n");
      return false;
    }

    if (contentLen == 0)
      return false;

    T::ContentInfo* contentInfo1 = contentList.getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList.getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList.getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList.getContentInfoByIndex(3);

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelIdType = pInfo.mParameterLevelIdType;
    valueList.mParameterLevelId = paramLevelId;
    valueList.mForecastTime = forecastTime;
    valueList.mProducerId = contentInfo1->mProducerId;
    valueList.mGenerationId = contentInfo1->mGenerationId;
    valueList.mGenerationFlags = generationFlags;
    valueList.mGeometryId = contentInfo1->mGeometryId;
    valueList.mModificationTime = contentInfo1->mModificationTime;
    valueList.mForecastType = contentInfo1->mForecastType;
    valueList.mForecastNumber = contentInfo1->mForecastNumber;
    valueList.mParameterLevel = contentInfo1->mParameterLevel;
    if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
      valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;
    else
      valueList.mParameterLevelId = pInfo.mParameterLevelId;

    attributeList.setAttribute("grid.timeInterpolationMethod",std::to_string(timeInterpolationMethod));
    attributeList.setAttribute("grid.areaInterpolationMethod",std::to_string(areaInterpolationMethod));
    attributeList.setAttribute("grid.levelInterpolationMethod",std::to_string(levelInterpolationMethod));

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTime == forecastTime || timeInterpolationMethod == T::TimeInterpolationMethod::None || timeInterpolationMethod == T::TimeInterpolationMethod::Nearest)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        int result = 0;

        switch (locationType)
        {
          case Query::LocationType::Geometry:
            result = mDataServerPtr->getGridValueVectorByGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,attributeList,valueVector);
            break;

          case Query::LocationType::Grid:
            result = mDataServerPtr->getGridValueVectorByCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,coordinateType,gridCoordinates,areaInterpolationMethod,valueVector);
            break;

          default:
            result = mDataServerPtr->getGridValueVector(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,valueVector);
            break;
        }

        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueVector");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }
        //valueList.print(std::cout,0,0);

        if (conversionByFunction)
           executeConversion(function, functionParams, valueVector, valueList.mValueVector);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

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
      if (contentInfo1->mForecastTime < forecastTime && contentInfo2->mForecastTime > forecastTime)
      {
        // We did not find a grid with the exact forecast time, but we find grids that
        // are before and after the current forecast time. This means that we should do
        // some time interpolation.

        switch (locationType)
        {
          case Query::LocationType::Geometry:
            result = mDataServerPtr->getGridValueVectorByTimeAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,forecastTime,attributeList,valueVector);
            break;

          case Query::LocationType::Grid:
            result = mDataServerPtr->getGridValueVectorByTimeAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,forecastTime,coordinateType,gridCoordinates,attributeList,valueVector);
            break;

          default:
            result = mDataServerPtr->getGridValueVectorByTime(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,forecastTime,timeInterpolationMethod,valueVector);
            break;
        }

        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueVectorByTimeAndGeometry");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        if (conversionByFunction)
           executeConversion(function, functionParams, valueVector, valueList.mValueVector);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }

      if (contentInfo1->mParameterLevel < paramLevel && contentInfo2->mParameterLevel > paramLevel)
      {
        // We did not find a grid with the exact level but we find grids that
        // are before and after the current level. This means that we should do
        // some level interpolation.

        switch (locationType)
        {
          case Query::LocationType::Geometry:
            result = mDataServerPtr->getGridValueVectorByLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,attributeList,valueVector);
            break;

          case Query::LocationType::Grid:
            result = mDataServerPtr->getGridValueVectorByLevelAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,coordinateType,gridCoordinates,attributeList,valueVector);
            break;

          default:
            result = mDataServerPtr->getGridValueVectorByLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,levelInterpolationMethod,valueVector);
            break;
        }

        valueList.mParameterLevel = paramLevel;

        if (conversionByFunction)
           executeConversion(function, functionParams, valueVector, valueList.mValueVector);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;


        return true;
      }
    }

    if (contentLen == 4)
    {
      if (contentInfo1->mForecastTime < forecastTime  &&  contentInfo2->mForecastTime < forecastTime  &&  contentInfo3->mForecastTime > forecastTime  &&  contentInfo4->mForecastTime > forecastTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&  contentInfo3->mParameterLevel < paramLevel  &&  contentInfo4->mParameterLevel > paramLevel)
      {
        switch (locationType)
        {
          case Query::LocationType::Geometry:
            result = mDataServerPtr->getGridValueVectorByTimeLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,forecastTime,paramLevel,attributeList,valueVector);
            break;

          case Query::LocationType::Grid:
            result = mDataServerPtr->getGridValueVectorByTimeLevelAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,forecastTime,paramLevel,coordinateType,gridCoordinates,attributeList,valueVector);
            break;

          default:
            result = mDataServerPtr->getGridValueVectorByTimeAndLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,forecastTime,paramLevel,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,valueVector);
            break;
        }

        valueList.mParameterLevel = paramLevel;

        if (conversionByFunction)
           executeConversion(function, functionParams, valueVector, valueList.mValueVector);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;


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





bool ServiceImplementation::getPointValuesByHeight(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    uint generationFlags,
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
    uchar coordinateType,
    T::AreaCoordinates& coordinates,
    uint& newProducerId,
    short& precision,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getPointValuesByHeight()\n");
    std::string function;
    std::vector < std::string > functionParams;
    bool conversionByFunction = conversionFunction(pInfo.mConversionFunction, function, functionParams);

    std::string fTime = forecastTime;
    std::string startTime = addSeconds(forecastTime,-4*3600);
    std::string endTime = addSeconds(forecastTime,4*3600);


    T::ContentInfoList tmpContentList;

    //int result = mContentServerPtr->getContentListByParameterGenerationIdAndForecastTime(0, generationId, pInfo.mParameterKeyType, pInfo.mParameterKey,T::ParamLevelIdTypeValue::IGNORE,
        //0, 0, -1, -1, producerGeometryId, fTime, tmpContentList);


    int result = mContentServerPtr->getContentListByParameterAndGenerationId(0,generationId,pInfo.mParameterKeyType, pInfo.mParameterKey,T::ParamLevelIdTypeValue::IGNORE,0,-1000000000,1000000000,forecastType,forecastNumber,producerGeometryId,startTime,endTime,0,tmpContentList);
    if (result != 0)
    {
      SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
      exception.addParameter("Service", "getContentListByParameterGenerationId");
      exception.addParameter("Message", ContentServer::getResultString(result));
      throw exception;
    }

    uint tmpContentLen = tmpContentList.getLength();
    if (tmpContentLen == 0)
      return false;

    PRINT_DATA(mDebugLog, "         + Found %u tmp content records\n", tmpContentList.getLength());

    tmpContentList.sort(T::ContentInfo::ComparisonMethod::starttime_fmiName_fmiLevelId_level_file_message);

    std::set<std::string> forecastTimeList;

    tmpContentList.getForecastTimeList(forecastTimeList);

    T::ContentInfoList tmpContentList2;
    std::string prev;
    std::string next;

    bool timeMatch = false;
    if (forecastTimeList.find(forecastTime) != forecastTimeList.end())
    {
      tmpContentList.getContentInfoListByForecastTime(forecastTime,tmpContentList2);
      timeMatch = true;
    }
    else
    {
      for (auto it = forecastTimeList.begin(); it != forecastTimeList.end(); ++it)
      {
        if (*it < forecastTime)
          prev = *it;
        if (*it > forecastTime  &&  (next.empty() ||  *it < next))
          next = *it;
      }

      tmpContentList.getContentInfoListByForecastTime(prev,tmpContentList2);
      tmpContentList.getContentInfoListByForecastTime(next,tmpContentList2);
    }

    PRINT_DATA(mDebugLog, "         + Found %u tmp2 content records\n", tmpContentList2.getLength());
    //tmpContentList2.print(std::cout,0,0);

    for (auto coordinateList = coordinates.begin(); coordinateList != coordinates.end(); ++coordinateList)
    {
      for (auto coordinate = coordinateList->begin(); coordinate != coordinateList->end(); ++coordinate)
      {
        T::ContentInfoList contentList;

        T::ContentInfoList tmpContentList3;
        convertLevelsToHeights(tmpContentList2,coordinateType,coordinate->x(),coordinate->y(),tmpContentList3);
        tmpContentList3.sort(T::ContentInfo::ComparisonMethod::starttime_fmiName_fmiLevelId_level_file_message);

        PRINT_DATA(mDebugLog, "         + Found %u tmp3 content records\n", tmpContentList3.getLength());
        uint cLen = tmpContentList3.getLength();
        T::ContentInfo *prevContent = nullptr;
        T::ContentInfo *nextContent = nullptr;

        if (timeMatch)
          prev = forecastTime;

        for (uint t=0; t<cLen; t++)
        {
          T::ContentInfo *cInfo = tmpContentList3.getContentInfoByIndex(t);
          if (cInfo->mForecastTime == prev)
          {
            if (cInfo->mParameterLevel <= paramLevel)
              prevContent = cInfo;

            if (cInfo->mParameterLevel >= paramLevel  &&  nextContent == nullptr )
              nextContent = cInfo;
          }
        }

        if (prevContent != NULL)
          contentList.addContentInfo(prevContent->duplicate());

        if (nextContent != NULL)
          contentList.addContentInfo(nextContent->duplicate());

        if (!timeMatch)
        {
          prevContent = nullptr;
          nextContent = nullptr;

          for (uint t=0; t<cLen; t++)
          {
            T::ContentInfo *cInfo = tmpContentList3.getContentInfoByIndex(t);
            if (cInfo->mForecastTime == next)
            {
              if (cInfo->mParameterLevel <= paramLevel)
                prevContent = cInfo;

              if (cInfo->mParameterLevel >= paramLevel  &&  nextContent == nullptr)
                nextContent = cInfo;
            }
          }

          if (prevContent != NULL)
            contentList.addContentInfo(prevContent->duplicate());

          if (nextContent != NULL)
            contentList.addContentInfo(nextContent->duplicate());
        }

        //T::ContentInfoList tmpContentList3;
        //convertLevelsToHeights(tmpContentList2,coordinateType,23.0,65.0,tmpContentList3);

        //contentList.print(std::cout,0,0);
        //return true;

        PRINT_DATA(mDebugLog, "         + Found %u content records\n", contentList.getLength());
        if (mDebugLog != nullptr)
        {
          std::stringstream stream;
          contentList.print(stream, 0, 4);
          PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
        }


        uint contentLen = contentList.getLength();
        if (contentLen == 0)
          return false;

        short iplMethod = levelInterpolationMethod;
        if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
        {
          if ((parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0)
            iplMethod = T::LevelInterpolationMethod::Linear;
          else
            iplMethod = T::LevelInterpolationMethod::Logarithmic;
        }

        // contentList.print(std::cout,0,0);

        T::ContentInfo* contentInfo1 = contentList.getContentInfoByIndex(0);
        T::ContentInfo* contentInfo2 = contentList.getContentInfoByIndex(1);
        T::ContentInfo* contentInfo3 = contentList.getContentInfoByIndex(2);
        T::ContentInfo* contentInfo4 = contentList.getContentInfoByIndex(3);

        valueList.mParameterKeyType = pInfo.mParameterKeyType;
        valueList.mParameterKey = pInfo.mParameterKey;
        valueList.mParameterLevelIdType = pInfo.mParameterLevelIdType;
        valueList.mParameterLevelId = paramLevelId;
        valueList.mForecastTime = forecastTime;
        valueList.mProducerId = contentInfo1->mProducerId;
        valueList.mGenerationId = contentInfo1->mGenerationId;
        valueList.mGenerationFlags = generationFlags;
        valueList.mGeometryId = contentInfo1->mGeometryId;
        valueList.mModificationTime = contentInfo1->mModificationTime;
        valueList.mForecastType = contentInfo1->mForecastType;
        valueList.mForecastNumber = contentInfo1->mForecastNumber;
        valueList.mParameterLevel = contentInfo1->mParameterLevel;

        if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
          valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;
        else
          valueList.mParameterLevelId = pInfo.mParameterLevelId;

        if (contentLen == 1)
        {
          if (contentInfo1->mForecastTime == fTime || timeInterpolationMethod == T::TimeInterpolationMethod::None || timeInterpolationMethod == T::TimeInterpolationMethod::Nearest)
          {
            // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

            T::ParamValue value = 0;
            int result = mDataServerPtr->getGridValueByPoint(0, contentInfo1->mFileId, contentInfo1->mMessageIndex, coordinateType,coordinate->x(),coordinate->y(),areaInterpolationMethod, value);
            if (result != 0)
            {
              SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
              exception.addParameter("Service", "getGridValueListByPointList");
              exception.addParameter("Message", DataServer::getResultString(result));
              std::string errorMsg = exception.getStackTrace();
              PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
              value = ParamValueMissing;
              //return false;
            }

            T::GridValue rec(coordinate->x(),coordinate->y(),value);
            valueList.mValueList.addGridValue(rec);
            valueList.mParameterLevel = paramLevel;
          }
          else
          {
            // There is one content record in place, but its time does not match to
            // the requested forecast time. This is used for indicating that there
            // are content records available, but not for the requested time.
            // So, we should use this producer.

            newProducerId = producerInfo.mProducerId;
            valueList.mProducerId = producerInfo.mProducerId;
          }
        }


        if (contentLen == 2)
        {
          if (contentInfo1->mForecastTime == fTime  &&  contentInfo2->mForecastTime == fTime  &&  contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel)
          {
            // Content records have the same time but different levels, so we need to level interpolation.

            short iplMethod = levelInterpolationMethod;
            if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
            {
              if ((parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0)
                iplMethod = T::LevelInterpolationMethod::Linear;
              else
                iplMethod = T::LevelInterpolationMethod::Logarithmic;
            }

            T::ParamValue value = 0;
            int result = mDataServerPtr->getGridValueByLevelAndPoint(0,
                contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo1->mParameterLevel,
                contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo2->mParameterLevel,
                paramLevel,coordinateType,coordinate->x(),coordinate->y(),areaInterpolationMethod,iplMethod,value);

            if (result != 0)
            {
              SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
              exception.addParameter("Service", "getGridValueListByLevelAndPointList");
              exception.addParameter("Message", DataServer::getResultString(result));
              std::string errorMsg = exception.getStackTrace();
              PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
              value = ParamValueMissing;
              //return false;
            }

            T::GridValue rec(coordinate->x(),coordinate->y(),value);
            valueList.mValueList.addGridValue(rec);
            valueList.mParameterLevel = paramLevel;
          }
        }


        if (contentLen == 4)
        {
          if (contentInfo1->mForecastTime < fTime  &&  contentInfo2->mForecastTime < fTime  &&  contentInfo3->mForecastTime > fTime  &&  contentInfo4->mForecastTime > fTime  &&
              contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&  contentInfo3->mParameterLevel < paramLevel  &&  contentInfo4->mParameterLevel > paramLevel)
          {
            T::ParamValue value = 0;
            int result = mDataServerPtr->getGridValueByTimeLevelAndPoint(0,
                contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo1->mParameterLevel,
                contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo2->mParameterLevel,
                contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo3->mParameterLevel,
                contentInfo4->mFileId, contentInfo4->mMessageIndex,contentInfo4->mParameterLevel,
                fTime,paramLevel,coordinateType,coordinate->x(),coordinate->y(),
                areaInterpolationMethod,timeInterpolationMethod,iplMethod,value);

            if (result != 0)
            {
              SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
              exception.addParameter("Service", "getGridValueListByLevelAndPointList");
              exception.addParameter("Message", DataServer::getResultString(result));
              std::string errorMsg = exception.getStackTrace();
              PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
              value = ParamValueMissing;
              //return false;
            }

            T::GridValue rec(coordinate->x(),coordinate->y(),value);
            valueList.mValueList.addGridValue(rec);
            valueList.mParameterLevel = paramLevel;
          }
        }
      }
    }


    if (valueList.mValueList.getLength() > 0)
    {
      if (conversionByFunction)
        executeConversion(function, functionParams, valueList.mValueList);

      if (precision < 0)
        precision = pInfo.mDefaultPrecision;

      return true;
    }

    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getPointValues(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    uint generationFlags,
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
    uchar coordinateType,
    T::AreaCoordinates& coordinates,
    uint& newProducerId,
    short& precision,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getPointValues()\n");

    if ((parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0)
    {
      return getPointValuesByHeight(producerInfo,producerGeometryId,generationId,generationFlags,pInfo,forecastTime,
          paramLevelId,paramLevel,forecastType,forecastNumber,parameterFlags,areaInterpolationMethod,timeInterpolationMethod,
          levelInterpolationMethod,coordinateType,coordinates,newProducerId,precision,valueList);
    }

    bool climatologyParam = false;
    if ((pInfo.mGroupFlags & QueryServer::ParameterMapping::GroupFlags::climatology) != 0)
      climatologyParam = true;

    std::string startTime = forecastTime;
    std::string endTime = forecastTime;

    std::string function;
    std::vector < std::string > functionParams;
    bool conversionByFunction = conversionFunction(pInfo.mConversionFunction, function, functionParams);

    std::string fTime = forecastTime;

    T::ContentInfoList contentList;
    if (climatologyParam)
    {
      T::ContentInfoList contentList2;
      int result = mContentServerPtr->getContentListByParameterAndGenerationId(0, generationId, pInfo.mParameterKeyType, pInfo.mParameterKey, T::ParamLevelIdTypeValue::IGNORE, -1,
          0, 0, forecastType, forecastNumber, producerGeometryId, "19000101T000000", "30000101T000000", 0, contentList2);

      if (result != 0)
      {
        SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
        exception.addParameter("Service", "getContentListByParameterAndGenerationId");
        exception.addParameter("Message", ContentServer::getResultString(result));
        throw exception;
      }

      if (contentList2.getLength() == 0)
        return false;

      T::ContentInfo* cInfo = contentList2.getContentInfoByIndex(0);
      fTime = cInfo->mForecastTime.substr(0, 4) + forecastTime.substr(4);
    }

    int result = mContentServerPtr->getContentListByParameterGenerationIdAndForecastTime(0, generationId, pInfo.mParameterKeyType, pInfo.mParameterKey, pInfo.mParameterLevelIdType,
        paramLevelId, paramLevel, forecastType, forecastNumber, producerGeometryId, fTime, contentList);

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
    if (contentLen == 0)
      return false;

    short iplMethod = levelInterpolationMethod;
    if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
    {
      if ((parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0)
        iplMethod = T::LevelInterpolationMethod::Linear;
      else
        iplMethod = T::LevelInterpolationMethod::Logarithmic;
    }

    // contentList.print(std::cout,0,0);

    T::ContentInfo* contentInfo1 = contentList.getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList.getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList.getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList.getContentInfoByIndex(3);

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelIdType = pInfo.mParameterLevelIdType;
    valueList.mParameterLevelId = paramLevelId;
    valueList.mForecastTime = forecastTime;
    valueList.mProducerId = contentInfo1->mProducerId;
    valueList.mGenerationId = contentInfo1->mGenerationId;
    valueList.mGenerationFlags = generationFlags;
    valueList.mGeometryId = contentInfo1->mGeometryId;
    valueList.mModificationTime = contentInfo1->mModificationTime;
    valueList.mForecastType = contentInfo1->mForecastType;
    valueList.mForecastNumber = contentInfo1->mForecastNumber;
    valueList.mParameterLevel = contentInfo1->mParameterLevel;
    if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
      valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;
    else
      valueList.mParameterLevelId = pInfo.mParameterLevelId;

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTime == fTime || timeInterpolationMethod == T::TimeInterpolationMethod::None || timeInterpolationMethod == T::TimeInterpolationMethod::Nearest)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        int result = mDataServerPtr->getGridValueListByPointList(0, contentInfo1->mFileId, contentInfo1->mMessageIndex, coordinateType,coordinates[0],areaInterpolationMethod, valueList.mValueList);
        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueListByPointList");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        if (conversionByFunction)
          executeConversion(function, functionParams, valueList.mValueList);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

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
      if (contentInfo1->mForecastTime == fTime  &&  contentInfo2->mForecastTime == fTime  &&  contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel)
      {
        // Content records have the same time but different levels, so we need to level interpolation.

        short iplMethod = levelInterpolationMethod;
        if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
        {
          if ((parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0)
            iplMethod = T::LevelInterpolationMethod::Linear;
          else
            iplMethod = T::LevelInterpolationMethod::Logarithmic;
        }

        int result = mDataServerPtr->getGridValueListByLevelAndPointList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,coordinateType,coordinates[0],areaInterpolationMethod,iplMethod,valueList.mValueList);
        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueListByLevelAndPointList");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        valueList.mParameterLevel = paramLevel;

        if (conversionByFunction)
          executeConversion(function, functionParams, valueList.mValueList);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }


      if (contentInfo1->mForecastTime < fTime  &&  contentInfo2->mForecastTime > fTime)
      {
        // Content records have different times, but most likely the same levels, so we need to do time interpolation.

        int result = mDataServerPtr->getGridValueListByTimeAndPointList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,forecastTime,coordinateType,coordinates[0],areaInterpolationMethod,timeInterpolationMethod,valueList.mValueList);
        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueListByTimeAndPointList");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        if (conversionByFunction)
          executeConversion(function, functionParams, valueList.mValueList);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }
    }


    if (contentLen == 4)
    {
      if (contentInfo1->mForecastTime < fTime  &&  contentInfo2->mForecastTime < fTime  &&  contentInfo3->mForecastTime > fTime  &&  contentInfo4->mForecastTime > fTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&  contentInfo3->mParameterLevel < paramLevel  &&  contentInfo4->mParameterLevel > paramLevel)
      {
        int result = mDataServerPtr->getGridValueListByTimeLevelAndPointList(0,
            contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,
            contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,
            fTime,paramLevel,coordinateType,coordinates[0],areaInterpolationMethod,timeInterpolationMethod,iplMethod,valueList.mValueList);

        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueListByLevelAndPointList");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        valueList.mParameterLevel = paramLevel;

        if (conversionByFunction)
          executeConversion(function, functionParams, valueList.mValueList);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

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





bool ServiceImplementation::getCircleValues(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    uint generationFlags,
    ParameterMapping& pInfo,
    std::string forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    T::ForecastType forecastType,
    T::ForecastNumber forecastNumber,
    uint parameterFlags,
    short timeInterpolationMethod,
    short levelInterpolationMethod,
    uchar coordinateType,
    double x,
    double y,
    double radius,
    uint& newProducerId,
    short& precision,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    std::string startTime = forecastTime;
    std::string endTime = forecastTime;

    std::string function;
    std::vector < std::string > functionParams;
    bool conversionByFunction = conversionFunction(pInfo.mConversionFunction, function, functionParams);

    T::ContentInfoList contentList;
    int result = mContentServerPtr->getContentListByParameterGenerationIdAndForecastTime(0, generationId, pInfo.mParameterKeyType, pInfo.mParameterKey, pInfo.mParameterLevelIdType,
        paramLevelId, paramLevel, forecastType, forecastNumber, producerGeometryId, forecastTime, contentList);
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

    if (contentLen == 0)
      return false;

    short iplMethod = levelInterpolationMethod;
    if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
    {
      if ((parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0)
        iplMethod = T::LevelInterpolationMethod::Linear;
      else
        iplMethod = T::LevelInterpolationMethod::Logarithmic;
    }

    T::ContentInfo* contentInfo1 = contentList.getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList.getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList.getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList.getContentInfoByIndex(3);

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelIdType = pInfo.mParameterLevelIdType;
    valueList.mParameterLevelId = pInfo.mParameterLevelId;
    valueList.mForecastTime = forecastTime;
    valueList.mProducerId = contentInfo1->mProducerId;
    valueList.mGenerationId = contentInfo1->mGenerationId;
    valueList.mGenerationFlags = generationFlags;
    valueList.mGeometryId = contentInfo1->mGeometryId;
    valueList.mModificationTime = contentInfo1->mModificationTime;
    valueList.mForecastType = contentInfo1->mForecastType;
    valueList.mForecastNumber = contentInfo1->mForecastNumber;
    valueList.mParameterLevel = contentInfo1->mParameterLevel;
    if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
      valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;
    else
      valueList.mParameterLevelId = pInfo.mParameterLevelId;

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTime == forecastTime || timeInterpolationMethod == T::TimeInterpolationMethod::None || timeInterpolationMethod == T::TimeInterpolationMethod::Nearest)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.


        int result = mDataServerPtr->getGridValueListByCircle(0, contentInfo1->mFileId, contentInfo1->mMessageIndex, coordinateType, x, y, radius, valueList.mValueList);
        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueListByCircle");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          // throw exception;
        }

        if (conversionByFunction)
          executeConversion(function, functionParams, valueList.mValueList);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

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
      if (contentInfo1->mForecastTime < forecastTime && contentInfo2->mForecastTime > forecastTime)
      {
        // We did not find a grid with the exact forecast time, but we find grids that
        // are before and after the current forecast time. This means that we should do
        // some time interpolation.

        valueList.mForecastTime = forecastTime;
        valueList.mProducerId = contentInfo1->mProducerId;
        valueList.mGenerationId = contentInfo1->mGenerationId;
        valueList.mGenerationFlags = generationFlags;
        valueList.mGeometryId = contentInfo1->mGeometryId;
        valueList.mModificationTime = contentInfo1->mModificationTime;
        valueList.mForecastType = contentInfo1->mForecastType;
        valueList.mForecastNumber = contentInfo1->mForecastNumber;
        valueList.mParameterLevel = contentInfo1->mParameterLevel;
        if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
          valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;
        else
          valueList.mParameterLevelId = pInfo.mParameterLevelId;

        // Fetching data from the grids.
        int result = mDataServerPtr->getGridValueListByTimeAndCircle(0,contentInfo1->mFileId,contentInfo1->mMessageIndex,contentInfo2->mFileId,contentInfo2->mMessageIndex,forecastTime,coordinateType,x,y,radius,timeInterpolationMethod,valueList.mValueList);
        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueListByTimeAndCircle");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        if (conversionByFunction)
          executeConversion(function, functionParams, valueList.mValueList);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }

      if (contentInfo1->mForecastTime == forecastTime  &&  contentInfo2->mForecastTime == forecastTime  &&  contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel)
      {
        // Content records have the same time but different levels, so we need to level interpolation.

        int result = mDataServerPtr->getGridValueListByLevelAndCircle(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,coordinateType,x,y,radius,iplMethod,valueList.mValueList);
        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueListByLevelAndCircle");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        valueList.mParameterLevel = paramLevel;

        if (conversionByFunction)
          executeConversion(function, functionParams, valueList.mValueList);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }
    }


    if (contentLen == 4)
    {
      if (contentInfo1->mForecastTime < forecastTime  &&  contentInfo2->mForecastTime < forecastTime  &&  contentInfo3->mForecastTime > forecastTime  &&  contentInfo4->mForecastTime > forecastTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&  contentInfo3->mParameterLevel < paramLevel  &&  contentInfo4->mParameterLevel > paramLevel)
      {
        int result = mDataServerPtr->getGridValueListByTimeLevelAndCircle(0,
            contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,
            contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,
            forecastTime,paramLevel,coordinateType,x,y,radius,timeInterpolationMethod,iplMethod,valueList.mValueList);

        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueListByTimeLevelAndCircle");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        valueList.mParameterLevel = paramLevel;

        if (conversionByFunction)
          executeConversion(function, functionParams, valueList.mValueList);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

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





bool ServiceImplementation::getPolygonValues(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    uint generationFlags,
    ParameterMapping& pInfo,
    std::string forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    T::ForecastType forecastType,
    T::ForecastNumber forecastNumber,
    uint parameterFlags,
    short timeInterpolationMethod,
    short levelInterpolationMethod,
    uchar coordinateType,
    T::AreaCoordinates& coordinates,
    uint& newProducerId,
    short& precision,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    std::string startTime = forecastTime;
    std::string endTime = forecastTime;

    std::string function;
    std::vector < std::string > functionParams;
    bool conversionByFunction = conversionFunction(pInfo.mConversionFunction, function, functionParams);

    T::ContentInfoList contentList;
    int result = mContentServerPtr->getContentListByParameterGenerationIdAndForecastTime(0, generationId, pInfo.mParameterKeyType, pInfo.mParameterKey, pInfo.mParameterLevelIdType,
        paramLevelId, paramLevel, forecastType, forecastNumber, producerGeometryId, forecastTime, contentList);
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

    if (contentLen == 0)
      return false;

    short iplMethod = levelInterpolationMethod;
    if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
    {
      if ((parameterFlags & QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED) != 0)
        iplMethod = T::LevelInterpolationMethod::Linear;
      else
        iplMethod = T::LevelInterpolationMethod::Logarithmic;
    }


    // We found content information close to the current forecast time

    T::ContentInfo* contentInfo1 = contentList.getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList.getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList.getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList.getContentInfoByIndex(3);

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelIdType = pInfo.mParameterLevelIdType;
    valueList.mParameterLevelId = paramLevelId;
    valueList.mForecastTime = forecastTime;
    valueList.mProducerId = contentInfo1->mProducerId;
    valueList.mGenerationId = contentInfo1->mGenerationId;
    valueList.mGenerationFlags = generationFlags;
    valueList.mGeometryId = contentInfo1->mGeometryId;
    valueList.mModificationTime = contentInfo1->mModificationTime;
    valueList.mForecastType = contentInfo1->mForecastType;
    valueList.mForecastNumber = contentInfo1->mForecastNumber;
    valueList.mParameterLevel = contentInfo1->mParameterLevel;
    if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
      valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;
    else
      valueList.mParameterLevelId = pInfo.mParameterLevelId;


    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTime == forecastTime || timeInterpolationMethod == T::TimeInterpolationMethod::None || timeInterpolationMethod == T::TimeInterpolationMethod::Nearest)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        int result = mDataServerPtr->getGridValueListByPolygonPath(0, contentInfo1->mFileId, contentInfo1->mMessageIndex, coordinateType, coordinates, valueList.mValueList);
        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueListByPolygonPath");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          // throw exception;
        }

        if (conversionByFunction)
          executeConversion(function, functionParams, valueList.mValueList);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

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
      if (contentInfo1->mForecastTime < forecastTime  &&  contentInfo2->mForecastTime > forecastTime)
      {
        // Content records have different times, but most likely the same levels, so we need to do time interpolation.

        int result = mDataServerPtr->getGridValueListByTimeAndPolygonPath(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,forecastTime,coordinateType,coordinates,timeInterpolationMethod,valueList.mValueList);
        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueListByTimeAndPolygonPath");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        if (conversionByFunction)
          executeConversion(function, functionParams, valueList.mValueList);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }

      if (contentInfo1->mForecastTime == forecastTime  &&  contentInfo2->mForecastTime == forecastTime  &&  contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel)
      {
        // Content records have the same time but different levels, so we need to level interpolation.

        int result = mDataServerPtr->getGridValueListByLevelAndPolygonPath(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,coordinateType,coordinates,iplMethod,valueList.mValueList);
        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueListByLevelAndPolygonPath");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        valueList.mParameterLevel = paramLevel;

        if (conversionByFunction)
          executeConversion(function, functionParams, valueList.mValueList);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }
    }


    if (contentLen == 4)
    {
      if (contentInfo1->mForecastTime < forecastTime  &&  contentInfo2->mForecastTime < forecastTime  &&  contentInfo3->mForecastTime > forecastTime  &&  contentInfo4->mForecastTime > forecastTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&  contentInfo3->mParameterLevel < paramLevel  &&  contentInfo4->mParameterLevel > paramLevel)
      {
        int result = mDataServerPtr->getGridValueListByTimeLevelAndPolygonPath(0,
            contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,
            contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,
            forecastTime,paramLevel,coordinateType,coordinates,timeInterpolationMethod,iplMethod,valueList.mValueList);

        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueListByTimeLevelAndPolygonPath");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        valueList.mParameterLevel = paramLevel;

        if (conversionByFunction)
          executeConversion(function, functionParams, valueList.mValueList);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

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





bool ServiceImplementation::getIsolineValues(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    uint generationFlags,
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
    uchar locationType,
    uchar coordinateType,
    T::Coordinate_vec& gridCoordinates,
    T::ParamValue_vec& contourValues,
    T::AttributeList& attributeList,
    uint& newProducerId,
    short& precision,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    // ### Converting contour values if necessary:

    T::ParamValue_vec newContourValues;

    std::string function;
    std::vector < std::string > functionParams;
    if (conversionFunction(pInfo.mReverseConversionFunction, function, functionParams))
      executeConversion(function,functionParams,contourValues,newContourValues);
    else
      newContourValues = contourValues;

    const char *gridWidthStr = attributeList.getAttributeValue("grid.width");
    const char *gridHeightStr = attributeList.getAttributeValue("grid.height");

    std::string startTime = forecastTime;
    std::string endTime = forecastTime;

    T::ContentInfoList contentList;
    int result = mContentServerPtr->getContentListByParameterGenerationIdAndForecastTime(0, generationId, pInfo.mParameterKeyType, pInfo.mParameterKey, pInfo.mParameterLevelIdType,
        paramLevelId, paramLevel, forecastType, forecastNumber, producerGeometryId, forecastTime, contentList);

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
    if (contentLen == 0)
      return false;

    T::ContentInfo* contentInfo1 = contentList.getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList.getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList.getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList.getContentInfoByIndex(3);

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelIdType = pInfo.mParameterLevelIdType;
    valueList.mParameterLevelId = paramLevelId;

    valueList.mForecastTime = forecastTime;
    valueList.mProducerId = contentInfo1->mProducerId;
    valueList.mGenerationId = contentInfo1->mGenerationId;
    valueList.mGenerationFlags = generationFlags;
    valueList.mGeometryId = contentInfo1->mGeometryId;
    valueList.mModificationTime = contentInfo1->mModificationTime;
    valueList.mForecastType = contentInfo1->mForecastType;
    valueList.mForecastNumber = contentInfo1->mForecastNumber;
    valueList.mParameterLevel = contentInfo1->mParameterLevel;
    if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
      valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;
    else
      valueList.mParameterLevelId = pInfo.mParameterLevelId;

    attributeList.setAttribute("grid.timeInterpolationMethod",std::to_string(timeInterpolationMethod));
    attributeList.setAttribute("grid.areaInterpolationMethod",std::to_string(areaInterpolationMethod));


    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTime == forecastTime || timeInterpolationMethod == T::TimeInterpolationMethod::None || timeInterpolationMethod == T::TimeInterpolationMethod::Nearest)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        int result = 0;

        switch (locationType)
        {
          case Query::LocationType::Geometry:
            result = mDataServerPtr->getGridIsolinesByGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourValues,attributeList,valueList.mWkbList);
            break;

          case Query::LocationType::Grid:
            if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
              result = mDataServerPtr->getGridIsolinesByGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),gridCoordinates,attributeList,valueList.mWkbList);
            break;

          default:
            result = mDataServerPtr->getGridIsolines(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourValues,attributeList,valueList.mWkbList);
            break;
        }

        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridIsolinesByGrid");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          // throw exception;
        }
        //valueList.print(std::cout,0,0);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

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
      if (contentInfo1->mForecastTime < forecastTime && contentInfo2->mForecastTime > forecastTime)
      {
        // We did not find a grid with the exact forecast time, but we find grids that
        // are before and after the current forecast time. This means that we should do
        // some time interpolation.


        switch (locationType)
        {
          case Query::LocationType::Geometry:
            result = mDataServerPtr->getGridIsolinesByTimeAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,forecastTime,newContourValues,attributeList,valueList.mWkbList);
            break;

          case Query::LocationType::Grid:
            if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
              result = mDataServerPtr->getGridIsolinesByTimeAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,forecastTime,newContourValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),gridCoordinates,attributeList,valueList.mWkbList);
            break;

          default:
            result = mDataServerPtr->getGridIsolinesByTime(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,forecastTime,newContourValues,attributeList,valueList.mWkbList);
            break;
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }

      if (contentInfo1->mParameterLevel < paramLevel && contentInfo2->mParameterLevel > paramLevel)
      {
        // We did not find a grid with the exact level but we find grids that
        // are before and after the current level. This means that we should do
        // some level interpolation.

        switch (locationType)
        {
          case Query::LocationType::Geometry:
            result = mDataServerPtr->getGridIsolinesByLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourValues,attributeList,valueList.mWkbList);
            break;

          case Query::LocationType::Grid:
            if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
              result = mDataServerPtr->getGridIsolinesByLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),gridCoordinates,attributeList,valueList.mWkbList);
            break;

          default:
            result = mDataServerPtr->getGridIsolinesByLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourValues,attributeList,valueList.mWkbList);
            break;
        }

        valueList.mParameterLevel = paramLevel;

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }
    }

    if (contentLen == 4)
    {
      if (contentInfo1->mForecastTime < forecastTime  &&  contentInfo2->mForecastTime < forecastTime  &&  contentInfo3->mForecastTime > forecastTime  &&  contentInfo4->mForecastTime > forecastTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&  contentInfo3->mParameterLevel < paramLevel  &&  contentInfo4->mParameterLevel > paramLevel)
      {
        switch (locationType)
        {
          case Query::LocationType::Geometry:
            result = mDataServerPtr->getGridIsolinesByTimeLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,forecastTime,paramLevel,newContourValues,attributeList,valueList.mWkbList);
            break;

          case Query::LocationType::Grid:
            if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
              result = mDataServerPtr->getGridIsolinesByTimeLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,forecastTime,paramLevel,newContourValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),gridCoordinates,attributeList,valueList.mWkbList);
            break;

          default:
            result = mDataServerPtr->getGridIsolinesByTimeAndLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,forecastTime,paramLevel,newContourValues,attributeList,valueList.mWkbList);
            break;
        }

        valueList.mParameterLevel = paramLevel;

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

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





bool ServiceImplementation::getIsobandValues(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    uint generationFlags,
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
    uchar locationType,
    uchar coordinateType,
    T::Coordinate_vec& gridCoordinates,
    T::ParamValue_vec& contourLowValues,
    T::ParamValue_vec& contourHighValues,
    T::AttributeList& attributeList,
    uint& newProducerId,
    short& precision,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    // ### Converting contour values if necessary:

    T::ParamValue_vec newContourLowValues;
    T::ParamValue_vec newContourHighValues;

    std::string function;
    std::vector < std::string > functionParams;
    if (conversionFunction(pInfo.mReverseConversionFunction, function, functionParams))
    {
      executeConversion(function,functionParams,contourLowValues,newContourLowValues);
      executeConversion(function,functionParams,contourHighValues,newContourHighValues);
    }
    else
    {
      newContourLowValues = contourLowValues;
      newContourHighValues = contourHighValues;
    }

    const char *gridWidthStr = attributeList.getAttributeValue("grid.width");
    const char *gridHeightStr = attributeList.getAttributeValue("grid.height");

    std::string startTime = forecastTime;
    std::string endTime = forecastTime;

    T::ContentInfoList contentList;
    int result = mContentServerPtr->getContentListByParameterGenerationIdAndForecastTime(0, generationId, pInfo.mParameterKeyType, pInfo.mParameterKey, pInfo.mParameterLevelIdType,
        paramLevelId, paramLevel, forecastType, forecastNumber, producerGeometryId, forecastTime, contentList);

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
    if (contentLen == 0)
      return false;

    T::ContentInfo* contentInfo1 = contentList.getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList.getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList.getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList.getContentInfoByIndex(3);

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelIdType = pInfo.mParameterLevelIdType;
    valueList.mParameterLevelId = paramLevelId;
    valueList.mForecastTime = forecastTime;
    valueList.mProducerId = contentInfo1->mProducerId;
    valueList.mGenerationId = contentInfo1->mGenerationId;
    valueList.mGenerationFlags = generationFlags;
    valueList.mGeometryId = contentInfo1->mGeometryId;
    valueList.mModificationTime = contentInfo1->mModificationTime;
    valueList.mForecastType = contentInfo1->mForecastType;
    valueList.mForecastNumber = contentInfo1->mForecastNumber;
    valueList.mParameterLevel = contentInfo1->mParameterLevel;
    if (pInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI)
      valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;
    else
      valueList.mParameterLevelId = pInfo.mParameterLevelId;

    attributeList.setAttribute("grid.timeInterpolationMethod",std::to_string(timeInterpolationMethod));
    attributeList.setAttribute("grid.areaInterpolationMethod",std::to_string(areaInterpolationMethod));

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTime == forecastTime || timeInterpolationMethod == T::TimeInterpolationMethod::None || timeInterpolationMethod == T::TimeInterpolationMethod::Nearest)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        int result = 0;

        switch (locationType)
        {
          case Query::LocationType::Geometry:
            result = mDataServerPtr->getGridIsobandsByGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourLowValues,newContourHighValues,attributeList,valueList.mWkbList);
            break;

          case Query::LocationType::Grid:
            if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
              result = mDataServerPtr->getGridIsobandsByGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourLowValues,newContourHighValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),gridCoordinates,attributeList,valueList.mWkbList);
            break;

          default:
            result = mDataServerPtr->getGridIsobands(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourLowValues,newContourHighValues,attributeList,valueList.mWkbList);
            break;
        }

        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridIsobandsByGrid");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          // throw exception;
        }
        //valueList.print(std::cout,0,0);

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

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
      if (contentInfo1->mForecastTime < forecastTime && contentInfo2->mForecastTime > forecastTime)
      {
        // We did not find a grid with the exact forecast time, but we find grids that
        // are before and after the current forecast time. This means that we should do
        // some time interpolation.

        switch (locationType)
        {
          case Query::LocationType::Geometry:
            result = mDataServerPtr->getGridIsobandsByTimeAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,forecastTime,newContourLowValues,newContourHighValues,attributeList,valueList.mWkbList);
            break;

          case Query::LocationType::Grid:
            if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
              result = mDataServerPtr->getGridIsobandsByTimeAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,forecastTime,newContourLowValues,newContourHighValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),gridCoordinates,attributeList,valueList.mWkbList);
            break;

          default:
            result = mDataServerPtr->getGridIsobandsByTime(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,forecastTime,newContourLowValues,newContourHighValues,attributeList,valueList.mWkbList);
            break;
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }


      if (contentInfo1->mParameterLevel < paramLevel && contentInfo2->mParameterLevel > paramLevel)
      {
        // We did not find a grid with the exact level but we find grids that
        // are before and after the current level. This means that we should do
        // some level interpolation.

        switch (locationType)
        {
          case Query::LocationType::Geometry:
            result = mDataServerPtr->getGridIsobandsByLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourLowValues,newContourHighValues,attributeList,valueList.mWkbList);
            break;

          case Query::LocationType::Grid:
            if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
              result = mDataServerPtr->getGridIsobandsByLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourLowValues,newContourHighValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),gridCoordinates,attributeList,valueList.mWkbList);
            break;

          default:
            result = mDataServerPtr->getGridIsobandsByLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourLowValues,newContourHighValues,attributeList,valueList.mWkbList);
            break;
        }

        valueList.mParameterLevel = paramLevel;

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }
    }

    if (contentLen == 4)
    {
      if (contentInfo1->mForecastTime < forecastTime  &&  contentInfo2->mForecastTime < forecastTime  &&  contentInfo3->mForecastTime > forecastTime  &&  contentInfo4->mForecastTime > forecastTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&  contentInfo3->mParameterLevel < paramLevel  &&  contentInfo4->mParameterLevel > paramLevel)
      {
        switch (locationType)
        {
          case Query::LocationType::Geometry:
            result = mDataServerPtr->getGridIsobandsByTimeLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,forecastTime,paramLevel,newContourLowValues,newContourHighValues,attributeList,valueList.mWkbList);
            break;

          case Query::LocationType::Grid:
            if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
              result = mDataServerPtr->getGridIsobandsByTimeLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,forecastTime,paramLevel,newContourLowValues,newContourHighValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),gridCoordinates,attributeList,valueList.mWkbList);
            break;

          default:
            result = mDataServerPtr->getGridIsobandsByTimeAndLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,forecastTime,paramLevel,newContourLowValues,newContourHighValues,attributeList,valueList.mWkbList);
            break;
        }

        valueList.mParameterLevel = paramLevel;

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

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





void ServiceImplementation::getGridValues(
    uchar queryType,
    Producer_vec& producers,
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
    uchar locationType,
    uchar coordinateType,
    T::AreaCoordinates& coordinates,
    T::ParamValue_vec& contourLowValues,
    T::ParamValue_vec& contourHighValues,
    T::AttributeList& attributeList,
    double radius,
    short& precision,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (mDebugLog != nullptr)
    {
      PRINT_DATA(mDebugLog, "\nMETHOD getGridValues()\n");
      PRINT_DATA(mDebugLog, "  - queryType                : %u items\n", queryType);
      PRINT_DATA(mDebugLog, "  - producers                : %lu items\n", producers.size());
      for (auto it = producers.begin(); it != producers.end(); ++it)
        PRINT_DATA(mDebugLog, "    * %s:%d\n", it->first.c_str(), it->second);
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
      PRINT_DATA(mDebugLog, "  - locationType             : %d\n", locationType);
      PRINT_DATA(mDebugLog, "  - coordinateType           : %d\n", coordinateType);
      PRINT_DATA(mDebugLog, "  - coordinates              : %lu vectors\n", coordinates.size());
      PRINT_DATA(mDebugLog, "  - contourLowValues         : %lu values\n",contourLowValues.size());
      PRINT_DATA(mDebugLog, "  - contourHighValues        : %lu values\n",contourHighValues.size());
      PRINT_DATA(mDebugLog, "  - radius                   : %f\n", radius);
    }

    if (coordinates.size() == 0  &&  attributeList.getAttributeValue("grid.geometryId") == nullptr  &&  attributeList.getAttributeValue("grid.geometryString") == nullptr  &&  attributeList.getAttributeValue("grid.urn") == nullptr)
    {
      SmartMet::Spine::Exception exception(BCP, "No coordinates defined!");
      throw exception;
    }

    if (locationType == Query::LocationType::Polygon)
    {
      for (size_t t = 0; t < coordinates.size(); t++)
      {
        if (coordinates[t].size() < 3)
        {
          SmartMet::Spine::Exception exception(BCP, "Polygon definition requires at least 3 coordinates!");
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

      PRINT_DATA(mDebugLog, "  - Producer and Geometry : %s:%d\n", producerName.c_str(), producerGeometryId);

      // Checking if the current geometry can be found from the acceptable geometry list

      if (geometryIdList.find(producerGeometryId) != geometryIdList.end())
      {
        // The current producer supports a geometry where the current coordinates can be found.

        T::ProducerInfo producerInfo;
        if (getProducerInfoByName(producerName, producerInfo) && (producerId == 0 || producerInfo.mProducerId == producerId))
        {
          PRINT_DATA(mDebugLog, "  - The producer and the geometry are acceptable!\n");

          // Reading generations supported by the current producer.

          T::GenerationInfoList generationInfoList;
          getGenerationInfoListByProducerId(producerInfo.mProducerId, generationInfoList);

          uint gLen = generationInfoList.getLength();
          if (gLen == 0)
            PRINT_DATA(mDebugLog, "    - No generations found for the current producer!\n");

          // Sorting generation analysis times.

          std::vector < std::string > analysisTimes;
          generationInfoList.getAnalysisTimes(analysisTimes, !reverseGenerations);

          // Going through all the parameter mapping files, until we find a match.

          // for (auto m = mParameterMappings.begin(); m != mParameterMappings.end(); ++m)
          {
            ParameterMapping_vec mappings;
            if (paramLevelId > 0 || paramLevel > 0)
            {
              getParameterMappings(producerInfo.mName, parameterKey, producerGeometryId, T::ParamLevelIdTypeValue::ANY, paramLevelId, paramLevel, false, mappings);
              if (mappings.size() == 0)
                getParameterMappings(producerInfo.mName, parameterKey, producerGeometryId, T::ParamLevelIdTypeValue::ANY, paramLevelId, -1, false, mappings);
            }
            else
            {
              getParameterMappings(producerInfo.mName, parameterKey, producerGeometryId, true, mappings);
            }

            if (mappings.size() == 0)
              PRINT_DATA(mDebugLog, "    - No parameter mappings '%s:%s:%d:%d:%d' found!\n", producerInfo.mName.c_str(), parameterKey.c_str(), producerGeometryId, paramLevelId, paramLevel);

            if (mappings.size() > 0)
            {
              // PRINT_DATA(mDebugLog,"    - Parameter mappings '%s:%s' found from the file
              // '%s'!\n",producerInfo.mName.c_str(),parameterKey.c_str(),m->getFilename().c_str());

              PRINT_DATA(mDebugLog, "  - Going through the generations from the newest to the oldest.\n");

              uint gLen = analysisTimes.size();
              for (uint g = 0; g < gLen; g++)
              {
                PRINT_DATA(mDebugLog, "    * %s\n", analysisTimes[g].c_str());

                uint gflags = 1 << g;

                bool generationValid = false;

                if (analysisTime.length() > 0)
                {
                  if (analysisTime == analysisTimes[g])
                  {
                    generationValid = true;
                  }
                  else
                  if (g == 0  &&  analysisTimes[g] < analysisTime)
                  {
                    generationValid = true;
                  }
                  else
                  if ((g+1) == gLen  &&  analysisTimes[g] > analysisTime)
                  {
                    generationValid = true;
                  }
                  else
                  if (analysisTimes[g] > analysisTime  &&  (g+1) < gLen  &&  analysisTimes[g+1] < analysisTime)
                  {
                    generationValid = true;
                  }
                }
                else
                {
                  if (generationFlags == 0 || (generationFlags & gflags) != 0)
                    generationValid = true;
                }

                if (generationValid)
                {
                  T::GenerationInfo* generationInfo = generationInfoList.getGenerationInfoByAnalysisTime(analysisTimes[g]);

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

                    if (paramLevelId > 0)
                    {
                      pLevelId = paramLevelId;
                      if (paramLevel >= 0)
                        pLevel = paramLevel;
                    }

                    if (paramLevel > 0)
                      pLevel = paramLevel;


                    // ### QUERY TYPE : PointValues

                    if (queryType == Query::Type::PointValues)
                    {

                      // ### LOCATION TYPE : Polygon

                      if (locationType == Query::LocationType::Polygon)
                      {
                        // ### Simple interpolation

                        if (getPolygonValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                            parameterFlags, timeInterpolation, levelInterpolation, coordinateType, coordinates, producerId, precision, valueList))
                          return;
                      }


                      // ### LOCATION TYPE : Circle

                      if (locationType == Query::LocationType::Circle && coordinates.size() == 1 && coordinates[0].size() == 1)
                      {
                        // ### Simple interpolation

                        double x = coordinates[0][0].x();
                        double y = coordinates[0][0].y();

                        if (getCircleValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                            parameterFlags, timeInterpolation, levelInterpolation, coordinateType, x, y, radius, producerId, precision, valueList))
                          return;
                      }


                      // ### LOCATION TYPE : Path

                      if (locationType == Query::LocationType::Path)
                      {
                        if (areaInterpolation < T::AreaInterpolationMethod::List)
                        {
                          // ### Simple interpolation

                          if (getPointValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                              parameterFlags, areaInterpolation, timeInterpolation, levelInterpolation, coordinateType, coordinates, producerId,precision,  valueList))
                            return;
                        }

                        if (areaInterpolation >= T::AreaInterpolationMethod::List && areaInterpolation < T::AreaInterpolationMethod::External && coordinates.size() == 1  && coordinates[0].size() == 1)
                        {
                          // ### No actual intepolation. Fetching special value for external interpolation

                          double x = coordinates[0][0].x();
                          double y = coordinates[0][0].y();

                          if (getSpecialValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                              parameterFlags, areaInterpolation, timeInterpolation, levelInterpolation, coordinateType, x, y, producerId, precision, valueList))
                            return;
                        }
                      }


                      // ### LOCATION TYPE : Point

                      if (locationType == Query::LocationType::Point)
                      {
                        if (areaInterpolation < T::AreaInterpolationMethod::List)
                        {
                          // ### Simple interpolation

                          if (getPointValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                              parameterFlags, areaInterpolation, timeInterpolation, levelInterpolation, coordinateType, coordinates, producerId, precision, valueList))
                            return;
                        }


                        if (areaInterpolation >= T::AreaInterpolationMethod::List && areaInterpolation < T::AreaInterpolationMethod::External && coordinates.size() == 1 && coordinates[0].size() == 1)
                        {
                          // ### No actual intepolation. Fetching special value for external interpolation

                          double x = coordinates[0][0].x();
                          double y = coordinates[0][0].y();

                          if (getSpecialValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                              parameterFlags, areaInterpolation, timeInterpolationMethod, levelInterpolationMethod, coordinateType, x, y, producerId, precision, valueList))
                            return;
                        }


                        if (areaInterpolation >= T::AreaInterpolationMethod::External && coordinates.size() == 1 && coordinates[0].size() == 1)
                        {
                          // ### External intepolation

                          ParameterValues valList;
                          ParameterValues geopHeightList;
                          ParameterValues lapserateList;
                          ParameterValues landList;

                          // ### Requesting interpolation information from a LUA function.

                          std::string infoFunction = "getAreaInterpolationInfo";
                          std::string interpolationFunction;

                          std::string paramList = mLuaFileCollection.executeFunctionCall6(infoFunction, producerInfo.mName, pInfo->mParameterName, pInfo->mParameterKeyType,
                              pInfo->mParameterKey, pInfo->mParameterLevelIdType, pLevelId, pLevel, forecastType, forecastNumber, areaInterpolation);

                          valueList.mForecastTime = forecastTime;
                          valueList.mProducerId = producerInfo.mProducerId;
                          valueList.mGenerationId = generationInfo->mGenerationId;
                          valueList.mGenerationFlags = gflags;
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

                          //std::cout << "LUA_PARAMETERS : " << paramList << "\n";

                          std::vector < std::string > paramVec;
                          splitString(paramList, ';', paramVec);

                          // ### Processing interpolation instructions and fetching required data

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
                            uchar a_locationType = locationType;
                            double a_radius = radius;
                            double a_dem = ParamValueMissing;
                            const char *demStr = attributeList.getAttributeValue("dem");
                            if (demStr != nullptr)
                              a_dem = toDouble(demStr);

                            ushort a_coverType = 0;
                            const char *coverTypeStr = attributeList.getAttributeValue("coverType");
                            if (coverTypeStr != nullptr)
                              a_coverType = toUInt16(coverTypeStr);

                            // std::cout << "* PARAM " << *param << "\n";
                            std::vector < std::string > paramParts;
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
                                  constParams.push_back(C_DOUBLE(a_dem));
                                else if (paramParts[1] == "coverType")
                                  constParams.push_back(C_DOUBLE(a_coverType));
                              }
                              if (paramParts[0] == "C")
                              {
                                constParams.push_back(toDouble(paramParts[1].c_str()));
                              }
                              else if (paramParts[0] == "Q")
                              {
                                std::vector < std::string > pv;
                                splitString(paramParts[1], ',', pv);

                                if (pv.size() == 8)
                                {
                                  if (!pv[0].empty())
                                    a_parameterKey = pv[0];

                                  if (!pv[1].empty())
                                    a_paramLevelId = toInt64(pv[1].c_str());

                                  if (!pv[2].empty())
                                    a_paramLevel = toInt64(pv[2].c_str());

                                  if (!pv[3].empty())
                                    a_forecastType = toInt64(pv[3].c_str());

                                  if (!pv[4].empty())
                                    a_forecastNumber = toInt64(pv[4].c_str());

                                  if (!pv[5].empty())
                                    a_areaInterpolationMethod = toInt64(pv[5].c_str());

                                  if (!pv[6].empty() && toInt64(pv[6].c_str()) == 1)
                                  {
                                    T::ContentInfoList contentList;
                                    if (mContentServerPtr->getContentListByParameterAndGenerationId(0, generationInfo->mGenerationId, T::ParamKeyTypeValue::FMI_NAME, a_parameterKey,
                                        T::ParamLevelIdTypeValue::FMI, a_paramLevel, a_paramLevel, a_paramLevel, a_forecastType, a_forecastNumber, producerGeometryId, "19000101T000000",
                                        "30000101T000000", 0, contentList) == 0)
                                    {
                                      if (contentList.getLength() > 0)
                                      {
                                        //contentList.print(std::cout, 0, 0);
                                        T::ContentInfo* cInfo = contentList.getContentInfoByIndex(0);
                                        a_forecastTime = cInfo->mForecastTime;
                                      }
                                    }
                                  }

                                  if (!pv[7].empty() && toInt64(pv[7].c_str()) == 1)
                                    a_producerId = 0;

                                  // We should search data only from similar geometries. Otherwise
                                  // the size and location of the grid cells are different.

                                  std::set < T::GeometryId > geomIdList;
                                  geomIdList.insert(producerGeometryId);

                                  ParameterValues valList;
                                  getGridValues(queryType,producers, geomIdList, a_producerId, a_analysisTime, a_generationFlags, a_reverseGenerations, a_parameterKey, a_paramLevelId,
                                      a_paramLevel, a_forecastType, a_forecastNumber, a_parameterFlags, a_areaInterpolationMethod, a_timeInterpolationMethod,
                                      a_levelInterpolationMethod, a_forecastTime, a_timeMatchRequired, a_locationType, coordinateType, coordinates, contourLowValues, contourHighValues,
                                      attributeList,a_radius, precision, valList);

                                  //printf("PARAM : %s %d\n",a_parameterKey.c_str(),valList.mValueList.getLength());
                                  //valList.mValueList.print(std::cout,0,0);
                                  valueList.mFlags = valList.mFlags;
                                  valueListVec.push_back(valList);
                                }
                              }
                            }
                          }

                          if (valueListVec.size() == 0)
                            return;

                          // ### Preparing parameters for the LUA interpolation function.

                          uint vLen1 = valueListVec[0].mValueList.getLength();

                          for (uint t = 0; t < vLen1; t++)
                          {
                            std::vector<double> params = constParams;

                            double vx = ParamValueMissing;
                            double vy = ParamValueMissing;

                            for (auto vList = valueListVec.begin(); vList != valueListVec.end(); ++vList)
                            {
                              uint vLen2 = vList->mValueList.getLength();

                              if (vLen1 == vLen2)
                              {
                                T::GridValue v;
                                vList->mValueList.getGridValueByIndex(t,v);
                                if (vx == ParamValueMissing)
                                {
                                  vx = v.mX;
                                  vy = v.mY;
                                }

                                if (!v.mValueString.empty())
                                {
                                  std::vector<double> vec;
                                  splitString(v.mValueString, ';', vec);

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
                                  params.push_back(v.mValue);
                                }
                              }
                            }

                            // ### Calling the LUA interpolation function.

                            double val = mLuaFileCollection.executeFunctionCall1(interpolationFunction, params);
                            T::GridValue rec(vx, vy, val);
                            valueList.mValueList.addGridValue(rec);

                            std::string function;
                            std::vector < std::string > functionParams;
                            bool conversionByFunction = conversionFunction(pInfo->mConversionFunction, function, functionParams);
                            if (conversionByFunction)
                              executeConversion(function, functionParams, valueList.mValueList);
                          }
                          return;
                        }
                      }

                    }

                    // ### ISOLINE QUERY

                    if (queryType == Query::Type::Isoline)
                    {
                      if (getIsolineValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                        parameterFlags, areaInterpolation, timeInterpolation, levelInterpolation, locationType, coordinateType, coordinates[0], contourLowValues, attributeList, producerId, precision, valueList))
                      {
                        return;
                      }
                    }


                    // ### ISOBAND QUERY

                    if (queryType == Query::Type::Isoband)
                    {
                      if (getIsobandValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                          parameterFlags, areaInterpolation, timeInterpolation, levelInterpolation, locationType, coordinateType, coordinates[0], contourLowValues, contourHighValues, attributeList, producerId, precision, valueList))
                      {
                        return;
                      }
                    }

                    // ### VECTOR QUERY

                    if (queryType == Query::Type::Vector)
                    {
                      if (getValueVectors(producerInfo, producerGeometryId, generationInfo->mGenerationId, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                          parameterFlags, areaInterpolation, timeInterpolation, levelInterpolation, locationType, coordinateType, coordinates[0], attributeList, producerId, precision, valueList))
                      {
                        return;
                      }
                    }

                  }
                }
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
        PRINT_DATA(mDebugLog, "  - Producer's '%s' geometry '%d' does not cover all requested coordinates!\n", producerName.c_str(), producerGeometryId);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::getGridValues(
    uchar queryType,
    Producer_vec& producers,
    std::set<T::GeometryId>& geometryIdList,
    uint producerId,
    std::string analysisTime,
    uint generationFlags,
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
    uint flags,
    uint timesteps,
    uint timestepSizeInMinutes,
    uchar locationType,
    uchar coordinateType,
    T::AreaCoordinates& coordinates,
    T::ParamValue_vec& contourLowValues,
    T::ParamValue_vec& contourHighValues,
    T::AttributeList& attributeList,
    double radius,
    uint maxValues,
    short& precision,
    ParameterValues_vec& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (mDebugLog != nullptr)
    {
      PRINT_DATA(mDebugLog, "\nMETHOD getGridValues()\n");
      PRINT_DATA(mDebugLog, "  - queryType                : %u items\n", queryType);
      PRINT_DATA(mDebugLog, "  - producers                : %lu items\n", producers.size());
      for (auto it = producers.begin(); it != producers.end(); ++it)
        PRINT_DATA(mDebugLog, "    * %s:%d\n", it->first.c_str(), it->second);
      PRINT_DATA(mDebugLog, "  - geometryIdList           : %lu items\n", geometryIdList.size());
      PRINT_DATA(mDebugLog, "  - producerId               : %u\n", producerId);
      PRINT_DATA(mDebugLog, "  - analysisTime             : %s\n", analysisTime.c_str());
      PRINT_DATA(mDebugLog, "  - generationFlags          : %08x\n", generationFlags);
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
      PRINT_DATA(mDebugLog, "  - flags                    : %d\n", flags);
      PRINT_DATA(mDebugLog, "  - timesteps                : %u\n", timesteps);
      PRINT_DATA(mDebugLog, "  - timestepSizeInMinutes    : %u\n", timestepSizeInMinutes);
      PRINT_DATA(mDebugLog, "  - locationType             : %d\n", locationType);
      PRINT_DATA(mDebugLog, "  - coordinateType           : %d\n", coordinateType);
      PRINT_DATA(mDebugLog, "  - coordinates              : %lu vectors\n", coordinates.size());
      PRINT_DATA(mDebugLog, "  - radius                   : %f\n", radius);
      PRINT_DATA(mDebugLog, "  - contourLowValues         : %lu values\n",contourLowValues.size());
      PRINT_DATA(mDebugLog, "  - contourHighValues        : %lu values\n",contourHighValues.size());
      PRINT_DATA(mDebugLog, "  - maxValues                : %u\n\n", maxValues);
    }

    if (coordinates.size() == 0  &&  attributeList.getAttributeValue("grid.geometryId") == nullptr  &&  attributeList.getAttributeValue("grid.geometryString") == nullptr  &&  attributeList.getAttributeValue("grid.urn") == nullptr)
    {
      SmartMet::Spine::Exception exception(BCP, "No coordinates defined!");
      throw exception;
    }

    if (locationType == Query::LocationType::Polygon)
    {
      for (size_t t = 0; t < coordinates.size(); t++)
      {
        if (coordinates[t].size() < 3)
        {
          SmartMet::Spine::Exception exception(BCP, "Area definition requires at least 3 coordinates!");
          throw exception;
        }
      }
    }

    bool ignoreStartTimeValue = false;
    if ((flags & Query::Flags::StartTimeNotIncluded) != 0)
      ignoreStartTimeValue = true;

    bool reverseGenerations = false;
    if ((flags & Query::Flags::ReverseGenerationFlags) != 0)
      reverseGenerations = true;

    // Going through the producer list.

    for (auto it = producers.begin(); it != producers.end(); ++it)
    {
      std::string producerName = it->first;
      T::GeometryId producerGeometryId = it->second;

      PRINT_DATA(mDebugLog, "  - Producer and Geometry : %s:%d\n", producerName.c_str(), producerGeometryId);

      // Checking if the current geometry can be found from the acceptable geometry list

      if (geometryIdList.find(producerGeometryId) != geometryIdList.end())
      {
        // The current producer supports a geometry where the current coordinates can be found.

        T::ProducerInfo producerInfo;
        if (getProducerInfoByName(producerName, producerInfo) && (producerId == 0 || producerInfo.mProducerId == producerId))
        {
          PRINT_DATA(mDebugLog, "  - The producer and the geometry are acceptable!\n");

          // Reading generations supported by the current producer.

          T::GenerationInfoList generationInfoList;
          getGenerationInfoListByProducerId(producerInfo.mProducerId, generationInfoList);

          uint gLen = generationInfoList.getLength();

          //generationInfoList.print(std::cout,0,0);
          if (gLen > 0)
          {
            generationInfoList.sort(T::GenerationInfo::ComparisonMethod::analysisTime_generationId);
            //generationInfoList.print(std::cout,0,0);

            ParameterMapping_vec mappings;
            if (paramLevelId > 0 || paramLevel > 0)
              getParameterMappings(producerInfo.mName, parameterKey, producerGeometryId, T::ParamLevelIdTypeValue::ANY, paramLevelId, paramLevel, false, mappings);
            else
              getParameterMappings(producerInfo.mName, parameterKey, producerGeometryId, true, mappings);

            if (mappings.size() > 0)
            {
              for (auto pInfo = mappings.begin(); pInfo != mappings.end(); ++pInfo)
              {
                if (mDebugLog != nullptr)
                {
                  std::stringstream stream;
                  pInfo->print(stream, 5, 0);
                  PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
                }

                //pInfo->print(std::cout,0,0);
                std::map<std::string,uint> contentTimeList;

                T::ContentInfoList contentInfoList;
                int result = mContentServerPtr->getContentListByParameterAndProducerId(0, producerInfo.mProducerId, pInfo->mParameterKeyType, pInfo->mParameterKey,
                    pInfo->mParameterLevelIdType, pInfo->mParameterLevelId, pInfo->mParameterLevel, pInfo->mParameterLevel, forecastType, forecastNumber, producerGeometryId,
                    std::string("19000101T000000"), std::string("30000101T000000"), 0, contentInfoList);

                if (result != 0)
                {
                  SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
                  exception.addParameter("Service", "getContentListByParameterAndProducerId");
                  exception.addParameter("Message", ContentServer::getResultString(result));
                  throw exception;
                }

                int cLen = contentInfoList.getLength();
                PRINT_DATA(mDebugLog, "         + Found %u content records\n", cLen);


                if (cLen > 0)
                {
                  contentInfoList.sort(T::ContentInfo::ComparisonMethod::starttime_generationId_file_message);
                  //contentInfoList.print(std::cout,0,0);
                  T::ContentInfo* cFirst = contentInfoList.getContentInfoByIndex(0);
                  //cFirst->print(std::cout,0,0);

                  std::string firstTime = "30000101T000000";
                  std::string lastTime = startTime;

                  uint c = 0;
                  for (int g=gLen-1; g>=0; g--)
                  {
                    uint gFlags = 1 << c;
                    c++;

                    bool generationValid = false;
                    T::GenerationInfo* gInfo = generationInfoList.getGenerationInfoByIndex(g);
                    T::GenerationInfo* gNext = generationInfoList.getGenerationInfoByIndex(g+1);

                    if (analysisTime.length() > 0)
                    {
                      if (g == 0 &&  analysisTime < gInfo->mAnalysisTime)
                      {
                        generationValid = true;
                      }
                      else
                      if (g == C_INT(gLen-1)  &&  analysisTime > gInfo->mAnalysisTime)
                      {
                        generationValid = true;
                      }
                      else
                      if (analysisTime == gInfo->mAnalysisTime || (gInfo->mAnalysisTime < analysisTime  &&  (gNext == nullptr  ||  gNext->mAnalysisTime > analysisTime)))
                      {
                        generationValid = true;
                      }
                    }
                    else
                    {
                      if (generationFlags == 0 || (generationFlags & gFlags) != 0)
                      {
                        generationValid = true;
                      }
                    }

                    if (generationValid)
                    {
                      for (int t = cLen-1; t >= 0; t--)
                      {
                        T::ContentInfo* cInfo = contentInfoList.getContentInfoByIndex(t);
                        if (cInfo != nullptr &&  cInfo->mGenerationId == gInfo->mGenerationId)
                        {
                          // cInfo->print(std::cout,0,0);
                          // std::cout << "-- TIME " << firstTime << "  " << lastTime << " " << cFirst->mForecastType << cFirst->mForecastNumber << "\n";

                          if (cInfo->mForecastTime >= startTime  &&  cInfo->mForecastTime <= endTime  &&  cInfo->mForecastType == cFirst->mForecastType  &&  cInfo->mForecastNumber == cFirst->mForecastNumber)
                          {
                            if (cInfo->mForecastTime > lastTime)
                            {
                              contentTimeList.clear();
                              lastTime = cInfo->mForecastTime;
                              firstTime = "30000101T000000";
                            }

                            if (cInfo->mForecastTime < firstTime)
                            {
                              contentTimeList.insert(std::pair<std::string,uint>(cInfo->mForecastTime,gFlags));
                              firstTime = cInfo->mForecastTime;
                            }
                          }
                        }
                      }
                    }
                  }

                  if (contentTimeList.size() > 0)
                  {
                    Producer_vec producers2;
                    producers2.push_back(*it);

                    std::set<T::GeometryId> geometryIdList2;
                    geometryIdList2.insert(producerGeometryId);

                    std::map<std::string,uint> contentTimeList2;

                    if (timestepSizeInMinutes > 0 && (flags & Query::Flags::StartTimeFromData) != 0  &&  (flags & Query::Flags::EndTimeFromData) == 0)
                    {
                      auto it = contentTimeList.begin();
                      time_t  tt = utcTimeToTimeT(it->first);
                      time_t  et = tt + timesteps * timestepSizeInMinutes*60;

                      if (timesteps == 0)
                        et = utcTimeToTimeT(endTime);

                      while (tt <= et)
                      {
                        std::string ts = utcTimeFromTimeT(tt);
                        contentTimeList2.insert(std::pair<std::string,uint>(ts,0xFFFFFFFF));
                        tt = tt + timestepSizeInMinutes*60;
                      }
                    }
                    else
                    if (timestepSizeInMinutes > 0 && (flags & Query::Flags::StartTimeFromData) == 0  &&  (flags & Query::Flags::EndTimeFromData) != 0)
                    {
                      auto it = contentTimeList.rbegin();
                      time_t  tt = utcTimeToTimeT(it->first);

                      time_t  et = tt - timesteps * timestepSizeInMinutes*60;

                      if (timesteps == 0)
                        et = utcTimeToTimeT(startTime);

                      while (tt >= et)
                      {
                        std::string ts = utcTimeFromTimeT(tt);
                        contentTimeList2.insert(std::pair<std::string,uint>(ts,0xFFFFFFFF));
                        tt = tt - timestepSizeInMinutes*60;
                      }
                    }
                    else
                    if (timestepSizeInMinutes > 0 && (flags & Query::Flags::StartTimeFromData) != 0  &&  (flags & Query::Flags::EndTimeFromData) != 0)
                    {
                      auto it1 = contentTimeList.begin();
                      time_t  tt = utcTimeToTimeT(it1->first);

                      auto it2 = contentTimeList.rbegin();
                      time_t  et = utcTimeToTimeT(it2->first);

                      while (tt <= et)
                      {
                        std::string ts = utcTimeFromTimeT(tt);
                        contentTimeList2.insert(std::pair<std::string,uint>(ts,0xFFFFFFFF));
                        tt = tt + timestepSizeInMinutes*60;
                      }
                    }
                    else
                    {
                      contentTimeList2 = contentTimeList;
                    }


                    for (auto forecastTime = contentTimeList2.begin(); forecastTime != contentTimeList2.end(); ++forecastTime)
                    {
                      // std::cout << forecastTime->first << "[" << startTime << " " << endTime << "]\n";
                      if (((forecastTime->first == startTime && !ignoreStartTimeValue) || (forecastTime->first >= startTime && forecastTime->first <= endTime)))
                      {
                        ParameterValues valList;
                        getGridValues(queryType,producers2, geometryIdList2, producerInfo.mProducerId, analysisTime, forecastTime->second, reverseGenerations, parameterKey, paramLevelId, paramLevel,
                            forecastType, forecastNumber, parameterFlags, areaInterpolationMethod, timeInterpolationMethod, levelInterpolationMethod, forecastTime->first, true,
                            locationType, coordinateType, coordinates, contourLowValues, contourHighValues, attributeList, radius, precision, valList);

                        //valList.print(std::cout,0,0);

                        if (valList.mValueList.getLength() > 0 || valList.mWkbList.size() > 0)
                        {
                          if (valList.mForecastTime <= " ")
                            valList.mForecastTime = forecastTime->first;

                            valueList.push_back(valList);
                        }

                        if (valueList.size() == maxValues)
                          return;
                      }
                    }
                    return;
                  }
                }
              }
            }
          }
          else
          {
            PRINT_DATA(mDebugLog, "    - No generations found for the current producer!\n");
          }
        }
        else
        {
          PRINT_DATA(mDebugLog, "  - Not a valid producer '%s'!\n", producerName.c_str());
        }
      }
      else
      {
        PRINT_DATA(mDebugLog, "  - Producer's '%s' geometry '%d' does not cover all requested coordinates!\n", producerName.c_str(), producerGeometryId);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::getAdditionalValues(
    std::string& parameterName,
    uchar coordinateType,
    T::Coordinate_vec& coordinates,
    ParameterValues& values)
{
  try
  {

    std::string param = toLowerString(parameterName);
    boost::local_time::local_date_time utcTime(boost::posix_time::from_iso_string(values.mForecastTime), nullptr);

    for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
    {
      T::GridValue val;
      val.mX = c->x();
      val.mY = c->y();

      if (param == "dem")
      {
        if (mDem)
        {
          val.mValue = mDem->elevation(val.mX, val.mY);
          values.mValueList.addGridValue(val);
        }
      }
      else
      if (param == "dark")
      {
        Fmi::Astronomy::solar_position_t sp = Fmi::Astronomy::solar_position(utcTime, val.mX, val.mY);
        val.mValue = (double)sp.dark();
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "sunelevation")
      {
        Fmi::Astronomy::solar_position_t sp = Fmi::Astronomy::solar_position(utcTime, val.mX, val.mY);
        val.mValue = (double)sp.elevation;
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "sundeclination")
      {
        Fmi::Astronomy::solar_position_t sp = Fmi::Astronomy::solar_position(utcTime, val.mX, val.mY);
        val.mValue = (double)sp.declination;
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "sunatzimuth")
      {
        Fmi::Astronomy::solar_position_t sp = Fmi::Astronomy::solar_position(utcTime, val.mX, val.mY);
        val.mValue = (double)sp.azimuth;
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "moonphase")
      {
        val.mValue = (double)Fmi::Astronomy::moonphase(utcTime.utc_time());
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "moonrise_utc")
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_iso_string(lt.moonrise.local_time());
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "moonrise2_utc")
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);

        if (lt.moonrise2_today())
          val.mValueString = Fmi::to_iso_string(lt.moonrise2.local_time());
        else
          val.mValueString = std::string("");
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "moonset_utc")
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_iso_string(lt.moonset.local_time());
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "moonset2_utc")
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);

        if (lt.moonset2_today())
          val.mValueString = Fmi::to_iso_string(lt.moonset2.local_time());
        else
          val.mValueString = "";
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "moonrisetoday_utc")
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(lt.moonrise_today());
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "moonrise2today_utc")
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(lt.moonrise2_today());
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "moonsettoday_utc")
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(lt.moonset_today());
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "moonset2today_utc")
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(lt.moonset2_today());
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "moonup24h")
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValue = (double)lt.above_horizont_24h();
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "moondown24h")
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValue = (double)(!lt.moonrise_today() && !lt.moonset_today() && !lt.above_horizont_24h());
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "sunrise_utc")
      {
        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_iso_string(st.sunrise.local_time());
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "sunset_utc")
      {
        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_iso_string(st.sunset.local_time());
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "noon_utc")
      {
        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_iso_string(st.noon.local_time());
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "sunrisetoday_utc")
      {
        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(st.sunrise_today());
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "sunsettoday_utc")
      {
        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(st.sunset_today());
        values.mValueList.addGridValue(val);
      }
      else
      if (param == "daylength")
      {
        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        auto seconds = st.daylength().total_seconds();
        auto minutes = boost::numeric_cast<long>(round(seconds / 60.0));
        val.mValue = minutes;
        values.mValueList.addGridValue(val);
      }
    }


  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::convertLevelsToHeights(T::ContentInfoList& contentList,uchar coordinateType,double x,double y,T::ContentInfoList& newContentList)
{
  FUNCTION_TRACE
  try
  {
    uint contentLen = contentList.getLength();
    for (uint t=0; t<contentLen; t++)
    {
      T::ContentInfo* contentInfo = contentList.getContentInfoByIndex(t);

      bool found = false;
      std::string cacheKey = std::to_string(contentInfo->mGenerationId) + ":" + contentInfo->mForecastTime + ":" + std::to_string(contentInfo->mFmiParameterLevelId) + ":" + std::to_string(contentInfo->mParameterLevel) + std::to_string(x) + std::to_string(y);
      if (mLevelHeightCache.size() > 0)
      {
        AutoThreadLock lock(&mHeightCacheThreadLock);

        for (auto cc = mLevelHeightCache.rbegin(); cc != mLevelHeightCache.rend(); ++cc)
        {
          if (cc->first == cacheKey)
          {
            T::ContentInfo *newContentInfo = contentInfo->duplicate();
            newContentInfo->mFmiParameterLevelId = 0;
            newContentInfo->mParameterLevel = cc->second;
            newContentList.addContentInfo(newContentInfo);
            found = true;
            //std::cout << "*** CACHE MATCH : " << cacheKey << "\n";
          }
        }
      }

      if (!found)
      {
        T::ParamKeyType paramKeyType = T::ParamKeyTypeValue::FMI_NAME;
        std::string paramKey = "Z-M2S2";
        T::ParamLevelIdType levelIdType = T::ParamLevelIdTypeValue::FMI;
        T::ParamLevelId levelId = contentInfo->mFmiParameterLevelId;
        T::ParamLevel level = contentInfo->mParameterLevel;

        T::ContentInfoList cList;
        int result = mContentServerPtr->getContentListByParameterGenerationIdAndForecastTime(0, contentInfo->mGenerationId, paramKeyType, paramKey, levelIdType,
          levelId, level, -1, -1, contentInfo->mGeometryId, contentInfo->mForecastTime, cList);

        if (result != 0)
        {
          SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
          exception.addParameter("Service", "getContentListByParameterGenerationIdAndForecastTime");
          exception.addParameter("Message", ContentServer::getResultString(result));
          throw exception;
        }

        //cList.print(std::cout,0,0);

        if (cList.getLength() == 1)
        {
          T::ContentInfo* cInfo = cList.getContentInfoByIndex(0);
          T::ParamValue value;
          int result = mDataServerPtr->getGridValueByPoint(0, cInfo->mFileId, cInfo->mMessageIndex, coordinateType,x,y,T::AreaInterpolationMethod::Linear,value);
          if (result != 0)
          {
            SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueByPoint");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          }
          else
          {
            T::ContentInfo *newContentInfo = contentInfo->duplicate();

            //printf("LEVEL %d  => %d\n",newContentInfo->mParameterLevel,(T::ParamLevel)(value/9.80665));

            newContentInfo->mFmiParameterLevelId = 0;
            newContentInfo->mParameterLevel = (T::ParamLevel)(value/9.80665);
            newContentList.addContentInfo(newContentInfo);

            AutoThreadLock lock(&mHeightCacheThreadLock);
            if (mLevelHeightCache.size() >= 5000)
              mLevelHeightCache.erase(mLevelHeightCache.begin(), mLevelHeightCache.begin() + 1000);

            mLevelHeightCache.push_back(std::pair<std::string,int>(cacheKey,newContentInfo->mParameterLevel));
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, exception_operation_failed, nullptr);
  }
}


}  // namespace QueryServer
}  // namespace SmartMet
