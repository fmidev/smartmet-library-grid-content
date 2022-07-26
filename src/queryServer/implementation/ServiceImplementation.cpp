#include "ServiceImplementation.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/InterpolationFunctions.h>
#include <grid-files/common/ShowFunction.h>
#include <grid-files/common/MemoryWriter.h>
#include <grid-files/common/FileWriter.h>
#include <grid-files/identification/GridDef.h>
#include <grid-files/grid/GridFile.h>
#include <macgyver/StringConversion.h>
#include <macgyver/TimeParser.h>
#include <macgyver/Astronomy.h>
#include <macgyver/CharsetTools.h>
#include <boost/functional/hash.hpp>
#include <unordered_set>
#include "../../functions/Function_add.h"
#include "../../functions/Function_avg.h"
#include "../../functions/Function_div.h"
#include "../../functions/Function_feelsLike.h"
#include "../../functions/Function_min.h"
#include "../../functions/Function_max.h"
#include "../../functions/Function_mul.h"
#include "../../functions/Function_multiply.h"
#include "../../functions/Function_sequence.h"
#include "../../functions/Function_sum.h"
#include "../../functions/Function_hypotenuse.h"

#define FUNCTION_TRACE FUNCTION_TRACE_OFF

namespace SmartMet
{
namespace QueryServer
{

boost::local_time::time_zone_ptr tz_utc(new boost::local_time::posix_time_zone("UTC"));


thread_local static ContentCache mContentCache;
thread_local static std::size_t mContentCache_records;

thread_local static ContentSearchCache mContentSearchCache;
thread_local static std::size_t mContentSearchCache_records;


thread_local static ParameterMappingCache mParameterMappingCache;
thread_local static time_t mParameterMappingCache_clearTime;

thread_local static std::set<ulonglong> mReadyGeometryList;

thread_local static ProducerGenarationListCache mProducerGenerationListCache;
thread_local static time_t mProducerGenerationListCache_clearTime;



static void* queryServer_updateThread(void *arg)
{
  try
  {
    ServiceImplementation *service = static_cast<ServiceImplementation*>(arg);
    service->updateProcessing();
    return nullptr;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Operation failed!",nullptr);
    exception.printError();
    exit(-1);
  }
}




ServiceImplementation::ServiceImplementation()
{
  FUNCTION_TRACE
  try
  {
    mContentServerPtr = nullptr;
    mDataServerPtr = nullptr;
    mFunctionParamId = 1000;
    mProducerFile_modificationTime = 0;
    mParameterMapping_checkTime = 0;
    mProducerFile_checkTime = 0;
    mProducerFile_checkInterval = 30;
    mParameterMapping_checkInterval = 30;
    mGenerationInfoList_checkInterval = 120;
    mProducerMap_checkInterval = 120;
    mGenerationInfoList_checkTime = 0;
    mProducerMap_updateTime = 0;
    mShutdownRequested = false;
    mContentServerStartTime = 0;
    mContentCache_maxRecords = 500000;
    mContentCache_records = 0;
    mContentSearchCache_maxRecords = 500000;
    mContentSearchCache_records = 0;
    mCheckGeometryStatus = false;
    mParameterMappingCache_clearTime = time(nullptr);
    mProducerGenerationListCache_clearTime  = time(nullptr);
    mProducerGenerationListCache_clearRequired = 0;
    mUpdateProcessingActive = false;
    mDataServerMethodsEnabled = false;

    GRID::Operation::getOperatorNames(mOperationNames);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





ServiceImplementation::~ServiceImplementation()
{
  FUNCTION_TRACE
  try
  {
    mShutdownRequested = true;
    pthread_join(mThread, nullptr);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void ServiceImplementation::init(
    ContentServer::ServiceInterface* contentServerPtr,
    DataServer::ServiceInterface* dataServerPtr,
    const std::string& gridConfigFile,
    string_vec& parameterMappingFiles,
    string_vec& aliasFiles,
    const std::string& producerFile,
    string_vec& producerAliasFiles,
    string_vec& luaFiles,
    bool checkGeometryStatus,
    bool dataServerMethodsEnabled)
{
  FUNCTION_TRACE
  try
  {
    mContentServerPtr = contentServerPtr;
    mDataServerPtr = dataServerPtr;

    mCheckGeometryStatus = checkGeometryStatus;
    mDataServerMethodsEnabled = dataServerMethodsEnabled;

    mGridConfigFile = gridConfigFile;
    SmartMet::Identification::gridDef.init(mGridConfigFile.c_str());

    mProducerFile = producerFile;
    loadProducerFile();

    mProducerAliasFileCollection.init(producerAliasFiles, true);

    mAliasFileCollection.init(aliasFiles);
    mLuaFileCollection.init(luaFiles);

    mFunctionCollection.addFunction("K2C",new Functions::Function_add(-273.15));
    Functions::Function_sequence *k2f = new Functions::Function_sequence();
    k2f->addFunction(new Functions::Function_add(-273.15));
    k2f->addFunction(new Functions::Function_multiply(1.8));
    k2f->addFunction(new Functions::Function_add(32.0));
    mFunctionCollection.addFunction("K2F",k2f);

    mFunctionCollection.addFunction("SUM",new Functions::Function_sum());
    mFunctionCollection.addFunction("DIV",new Functions::Function_div());
    mFunctionCollection.addFunction("MUL",new Functions::Function_mul());

    mFunctionCollection.addFunction("AVG",new Functions::Function_avg());
    mFunctionCollection.addFunction("MIN",new Functions::Function_min());
    mFunctionCollection.addFunction("MAX",new Functions::Function_max());

    // Radians to degrees
    mFunctionCollection.addFunction("RAD2DEG",new Functions::Function_multiply((360.0/2*3.1415926535)));

    // Degrees to radians
    mFunctionCollection.addFunction("DEG2RAD",new Functions::Function_multiply((2*3.1415926535/360.0)));

    mFunctionCollection.addFunction("FEELS_LIKE",new Functions::Function_feelsLike());

    mParameterMappingFiles = parameterMappingFiles;

    for (auto it = mParameterMappingFiles.begin(); it != mParameterMappingFiles.end(); ++it)
    {
      ParameterMappingFile mapping(*it);
      mParameterMappings.emplace_back(mapping);
    }

    for (auto it = mParameterMappings.begin(); it != mParameterMappings.end(); ++it)
    {
      it->init();
    }

    startUpdateProcessing();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::setLandCover(boost::shared_ptr<Fmi::LandCover> landCover)
{
  try
  {
    mLandCover = landCover;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP, "Operation failed!", nullptr);
    throw exception;
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
    Fmi::Exception exception(BCP, "Operation failed!", nullptr);
    throw exception;
  }
}





void ServiceImplementation::shutdown()
{
  FUNCTION_TRACE
  try
  {
    mShutdownRequested = true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getProducerInfoByName(const std::string& name,T::ProducerInfo& info)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mProducerMap_modificationLock);

    if (!mProducerMap)
      return false;

    auto it = mProducerMap->find(toUpperString(name));
    if (it != mProducerMap->end())
    {
      info = it->second;
      return true;
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




void ServiceImplementation::getGenerationTimeRangeByProducerAndGenerationId(uint producerId,uint generationId,time_t& startTime,time_t& endTime)
{
  FUNCTION_TRACE
  try
  {
    startTime = 0;
    endTime = 0;
    mContentServerPtr->getContentTimeRangeByProducerAndGenerationId(0,producerId,generationId,startTime,endTime);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





CacheEntry_sptr ServiceImplementation::getGenerationInfoListByProducerId(uint producerId,bool acceptNotReadyGenerations)
{
  FUNCTION_TRACE
  try
  {
    if (mProducerGenerationListCache_clearRequired > mProducerGenerationListCache_clearTime)
    {
      mProducerGenerationListCache.clear();
      mProducerGenerationListCache_clearTime = mProducerGenerationListCache_clearRequired;
    }

    uint key = producerId;
    if (acceptNotReadyGenerations)
      key = producerId | 0x80000000;

    auto gl = mProducerGenerationListCache.find(key);
    if (gl != mProducerGenerationListCache.end())
      return gl->second;

    CacheEntry_sptr cacheEntry(new CacheEntry);
    if (!mGenerationInfoList)
      return cacheEntry;

    T::GenerationInfoList_sptr generationInfoList(new T::GenerationInfoList());
    mGenerationInfoList->getGenerationInfoListByProducerIdAndStatus(producerId,*generationInfoList,T::GenerationInfo::Status::Ready);

    if (acceptNotReadyGenerations)
    {
      T::GenerationInfoList gList;
      mGenerationInfoList->getGenerationInfoListByProducerIdAndStatus(producerId,gList,T::GenerationInfo::Status::Running);
      uint len = gList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::GenerationInfo *gInfo = gList.getGenerationInfoByIndex(t);
        generationInfoList->addGenerationInfo(gInfo->duplicate());
      }
    }


    StringVector_sptr analysisTimes(new std::vector<std::string>());
    generationInfoList->sort(T::GenerationInfo::ComparisonMethod::analysisTime);
    generationInfoList->getAnalysisTimes(*analysisTimes,true);

    cacheEntry->generationInfoList = generationInfoList;
    cacheEntry->analysisTimes = analysisTimes;

    mProducerGenerationListCache.insert(std::pair<uint,CacheEntry_sptr>(key,cacheEntry));

    return cacheEntry;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






int ServiceImplementation::_executeQuery(T::SessionId sessionId, Query& query)
{
  FUNCTION_TRACE
  try
  {
    if (mShutdownRequested)
      return 0;

    // query.print(std::cout,0,0);

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
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

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      PRINT_DATA(mDebugLog, "\n############################### QUERY EXECUTION END ###################\n\n");
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






int ServiceImplementation::_getProducerList(T::SessionId sessionId, string_vec& producerList)
{
  FUNCTION_TRACE
  try
  {
    if (mShutdownRequested)
      return 0;

    producerList.clear();
    std::string prev;

    AutoReadLock lock(&mProducerList_modificationLock);
    for (auto it = mProducerList.begin(); it != mProducerList.end(); ++it)
    {
      if (it->first != prev)
        producerList.emplace_back(it->first);

      prev = it->first;
    }
    return QueryServer::Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::loadProducerFile()
{
  FUNCTION_TRACE
  try
  {
    if (mShutdownRequested)
      return;

    AutoWriteLock lock(&mProducerList_modificationLock);

    if (mProducerFile_modificationTime == getFileModificationTime(mProducerFile.c_str()))
      return;

    FILE* file = fopen(mProducerFile.c_str(), "re");
    if (file == nullptr)
    {
      Fmi::Exception exception(BCP, "Cannot open the producer file!");
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
        c--;

        std::string key;
        for (uint t=0; t<c; t++)
        {
          if (field[t][0] != '\0')
          {
            std::vector<std::string> parts;
            splitString(field[t],':',parts);
            if (parts.size() == 2)
            {
              if (t == 0)
              {
                key = toUpperString(field[0]);
                mProducerList.emplace_back(std::pair<std::string, T::GeometryId>(toUpperString(parts[0]), toInt32(parts[1])));
              }
              else
              if (t == 1)
              {
                Producer_vec vec;
                vec.emplace_back(std::pair<std::string, T::GeometryId>(toUpperString(parts[0]), toInt32(parts[1])));
                mProducerConcatMap.insert(std::pair<std::string,Producer_vec>(key,vec));
              }
              else
              {
                auto vec = mProducerConcatMap.find(key);
                if (vec != mProducerConcatMap.end())
                  vec->second.emplace_back(std::pair<std::string, T::GeometryId>(toUpperString(parts[0]), toInt32(parts[1])));
              }
            }
          }
        }
      }
    }
    fclose(file);

    mProducerFile_modificationTime = getFileModificationTime(mProducerFile.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::getGeometryIdListByCoordinates(Producer_vec& producers,T::AreaCoordinates& coordinates, std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    for (auto prod = producers.begin(); prod != producers.end(); ++prod)
    {
      bool match = true;
      for (auto cList = coordinates.begin(); cList != coordinates.end(); ++cList)
      {
        for (auto coordinate = cList->begin(); coordinate != cList->end(); ++coordinate)
        {
          double grid_i = ParamValueMissing;
          double grid_j = ParamValueMissing;

          if (!Identification::gridDef.getGridPointByGeometryIdAndLatLonCoordinates(prod->second,coordinate->y(), coordinate->x(),grid_i,grid_j))
            match = false;
        }
      }
      if (match)
        geometryIdList.insert(prod->second);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
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
      std::set<T::GeometryId> idList;
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
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
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

    auto def1 = Identification::gridDef.getGrib1DefinitionByGeometryId(gridGeometryId);
    if (def1)
    {
      T::Dimensions d = def1->getGridDimensions();
      width = d.nx();
      height = d.ny();

      double lat = 0;
      double lon = 0;

      for (uint x=0; x<width; x=x+100)
      {
        def1->getGridLatLonCoordinatesByGridPoint(x,0,lat,lon);
        coordinates.emplace_back(T::Coordinate(lon,lat));

        def1->getGridLatLonCoordinatesByGridPoint(x,height-1,lat,lon);
        coordinates.emplace_back(T::Coordinate(lon,lat));
      }

      for (uint y=0; y<height; y=y+100)
      {
        def1->getGridLatLonCoordinatesByGridPoint(0,y,lat,lon);
        coordinates.emplace_back(T::Coordinate(lon,lat));

        def1->getGridLatLonCoordinatesByGridPoint(width-1,y,lat,lon);
        coordinates.emplace_back(T::Coordinate(lon,lat));
      }


      def1->getGridLatLonCoordinatesByGridPoint(width-1,0,lat,lon);
      coordinates.emplace_back(T::Coordinate(lon,lat));

      def1->getGridLatLonCoordinatesByGridPoint(width-1,height-1,lat,lon);
      coordinates.emplace_back(T::Coordinate(lon,lat));

      getGeometryIdListByCoordinates(width,height,coordinates,geometryIdList);
      return;
    }

    auto def2 = Identification::gridDef.getGrib2DefinitionByGeometryId(gridGeometryId);
    if (def2)
    {
      T::Dimensions d = def2->getGridDimensions();
      width = d.nx();
      height = d.ny();

      double lat = 0;
      double lon = 0;

      for (uint x=0; x<width; x=x+100)
      {
        def2->getGridLatLonCoordinatesByGridPoint(x,0,lat,lon);
        coordinates.emplace_back(T::Coordinate(lon,lat));

        def2->getGridLatLonCoordinatesByGridPoint(x,height-1,lat,lon);
        coordinates.emplace_back(T::Coordinate(lon,lat));
      }

      for (uint y=0; y<height; y=y+100)
      {
        def2->getGridLatLonCoordinatesByGridPoint(0,y,lat,lon);
        coordinates.emplace_back(T::Coordinate(lon,lat));

        def2->getGridLatLonCoordinatesByGridPoint(width-1,y,lat,lon);
        coordinates.emplace_back(T::Coordinate(lon,lat));
      }


      def2->getGridLatLonCoordinatesByGridPoint(width-1,0,lat,lon);
      coordinates.emplace_back(T::Coordinate(lon,lat));

      def2->getGridLatLonCoordinatesByGridPoint(width-1,height-1,lat,lon);
      coordinates.emplace_back(T::Coordinate(lon,lat));


      getGeometryIdListByCoordinates(width,height,coordinates,geometryIdList);
      return;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getParameterFunctionInfo(const std::string& paramStr, std::string& function,std::string& functionParams)
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getFunctionParams(const std::string& functionParamsStr, FunctionParam_vec& functionParams)
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
        mFunctionParamId++;
        functionParams.emplace_back(std::pair<uint, std::string>(mFunctionParamId, std::string(tmp)));
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
      mFunctionParamId++;
      functionParams.emplace_back(std::pair<uint, std::string>(mFunctionParamId, std::string(tmp)));
    }

    if (mFunctionParamId > 4000000000)
      mFunctionParamId = 1000;

    return containsFunction;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::getParameterMappings(const std::string& producerName,uint producerId,const std::string& parameterName,std::size_t parameterHash,T::GeometryId geometryId, bool onlySearchEnabled, ParameterMapping_vec_sptr& mappings)
{
  try
  {
    std::size_t hash = 0;

    if (producerId != 0)
      boost::hash_combine(hash,producerId);
    else
      boost::hash_combine(hash,producerName);

    if (parameterHash != 0)
      boost::hash_combine(hash,parameterHash);
    else
      boost::hash_combine(hash,parameterName);

    boost::hash_combine(hash,geometryId);
    boost::hash_combine(hash,onlySearchEnabled);

    if (mParameterMappingCache_clearTime < mParameterMappingCache_clearRequired)
    {
      mParameterMappingCache.clear();
      mParameterMappingCache_clearTime = mParameterMappingCache_clearRequired;
    }


    {
      //AutoReadLock lock(&mParameterMappingCache_modificationLock);
      auto it = mParameterMappingCache.find(hash);
      if (it != mParameterMappingCache.end())
      {
        mappings = it->second;
        return;
      }

      mappings.reset(new ParameterMapping_vec);

      for (auto m = mParameterMappings.begin(); m != mParameterMappings.end(); ++m)
      {
        m->getMappings(producerName, parameterName, geometryId, onlySearchEnabled, *mappings);
      }
    }

    //AutoWriteLock lock(&mParameterMappingCache_modificationLock);
    if (mParameterMappingCache.find(hash) == mParameterMappingCache.end())
      mParameterMappingCache.insert(std::pair<std::size_t, ParameterMapping_vec_sptr>(hash, mappings));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::getParameterMappings(
    const std::string& producerName,
    uint producerId,
    const std::string& parameterName,
    std::size_t parameterHash,
    T::GeometryId geometryId,
    T::ParamLevelId levelId,
    T::ParamLevel level,
    bool onlySearchEnabled,
    ParameterMapping_vec_sptr& mappings)
{
  try
  {
    std::size_t hash = 0;

    if (producerId != 0)
      boost::hash_combine(hash,producerId);
    else
      boost::hash_combine(hash,producerName);

    if (parameterHash != 0)
      boost::hash_combine(hash,parameterHash);
    else
      boost::hash_combine(hash,parameterName);

    boost::hash_combine(hash,geometryId);
    boost::hash_combine(hash,levelId);
    boost::hash_combine(hash,level);
    boost::hash_combine(hash,onlySearchEnabled);

    if (mParameterMappingCache_clearTime < mParameterMappingCache_clearRequired)
    {
      mParameterMappingCache.clear();
      mParameterMappingCache_clearTime = mParameterMappingCache_clearRequired;
    }

    {
      //AutoReadLock lock(&mParameterMappingCache_modificationLock);
      auto it = mParameterMappingCache.find(hash);
      if (it != mParameterMappingCache.end())
      {
        mappings = it->second;
        return;
      }

      mappings.reset(new ParameterMapping_vec);

      for (auto m = mParameterMappings.begin(); m != mParameterMappings.end(); ++m)
      {
        //m->getMappings(producerName, parameterName, geometryId, onlySearchEnabled, mappings);
        m->getMappings(producerName, parameterName, geometryId, levelId, level, onlySearchEnabled, *mappings);
      }
    }

    //AutoWriteLock lock(&mParameterMappingCache_modificationLock);
    if (mParameterMappingCache.find(hash) == mParameterMappingCache.end())
      mParameterMappingCache.insert(std::pair<std::size_t, ParameterMapping_vec_sptr>(hash, mappings));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::getParameterStringInfo(
    const std::string& param,
    char fieldSeparator,
    std::string& key,
    T::GeometryId& geometryId,
    T::ParamLevelId& paramLevelId,
    T::ParamLevel& paramLevel,
    T::ForecastType& forecastType,
    T::ForecastNumber& forecastNumber,
    std::string& producerName,
    uint& producerId,
    ulonglong& generationFlags,
    short& areaInterpolationMethod,
    short& timeInterpolationMethod,
    short& levelInterpolationMethod)

{
  FUNCTION_TRACE
  try
  {
    std::vector<T::ForecastNumber> forecastNumberVec;
    getParameterStringInfo(param, fieldSeparator, key, geometryId,paramLevelId, paramLevel, forecastType, forecastNumberVec, producerName, producerId, generationFlags, areaInterpolationMethod,
        timeInterpolationMethod, levelInterpolationMethod);

    if (forecastNumberVec.size() > 0)
      forecastNumber = forecastNumberVec[0];

  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::getParameterStringInfo(
    const std::string& param,
    char fieldSeparator,
    std::string& key,
    T::GeometryId& geometryId,
    T::ParamLevelId& paramLevelId,
    T::ParamLevel& paramLevel,
    T::ForecastType& forecastType,
    std::vector<T::ForecastNumber>& forecastNumberVec,
    std::string& producerName,
    uint& producerId,
    ulonglong& generationFlags,
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

      if (level == 0 && *p == fieldSeparator)
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

    key = field[0];
    std::string alias;
    if (getAlias(key, alias))
    {
      // Alias cannot be a function.
      if (strchr(alias.c_str(),'{') == nullptr)
        key = alias;
    }
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
        geometryId = toInt32(field[2]);
    }

    if (c > 3)
    {
      if (field[3][0] != '\0')
        paramLevelId = toUInt8(field[3]);
    }

    if (c > 4)
    {
      if (field[4][0] != '\0')
        paramLevel = toInt32(field[4]);
    }

    if (c > 5)
    {
      if (field[5][0] != '\0')
        forecastType = toInt16(field[5]);
    }

    if (c > 6)
    {
      if (field[6][0] != '\0')
      {
        std::vector<std::string> partList;
        splitString(field[6], '-', partList);
        size_t sz = partList.size();
        if (sz == 1)
        {
          forecastNumberVec.emplace_back(toInt16(partList[0].c_str()));
        }
        else if (sz == 2)
        {
          auto start = toInt16(partList[0].c_str());
          auto end = toInt16(partList[1].c_str());
          if (start < end && (end - start) <= 200)
          {
            for (auto t = start; t <= end; t++)
              forecastNumberVec.emplace_back((T::ForecastNumber) t);
          }
        }
      }
    }

    if (c > 7)
    {
      if (field[7][0] != '\0')
        generationFlags = toUInt64(field[7]);
    }

    if (c > 8)
    {
      if (field[8][0] != '\0')
        areaInterpolationMethod = toInt16(field[8]);
    }

    if (c > 9)
    {
      if (field[9][0] != '\0')
        timeInterpolationMethod = toInt16(field[9]);
    }

    if (c > 10)
    {
      if (field[10][0] != '\0')
        levelInterpolationMethod = toInt16(field[10]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getAlias(const std::string& name, std::string& alias)
{
  FUNCTION_TRACE
  try
  {
    return mAliasFileCollection.getAlias(name, alias);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}



bool ServiceImplementation::isGeometryValid(int geometryId,std::vector<std::vector<T::Coordinate>>& polygonPath)
{
  try
  {
    for (auto cList = polygonPath.begin(); cList != polygonPath.end(); ++cList)
    {
      for (auto coordinate = cList->begin(); coordinate != cList->end(); ++coordinate)
      {
        double grid_i = ParamValueMissing;
        double grid_j = ParamValueMissing;
        if (!Identification::gridDef.getGridPointByGeometryIdAndLatLonCoordinates(geometryId, coordinate->y(), coordinate->x(), grid_i, grid_j))
          return false;
      }
    }
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::isGeometryReady(uint generationId,int geometryId,T::ParamLevelId levelId)
{
  try
  {
    if (!mCheckGeometryStatus)
      return true;

    ulonglong key = ((ulonglong)generationId << 32) + ((ulonglong)geometryId << 8) + levelId;

    if (mReadyGeometryList.find(key) != mReadyGeometryList.end())
      return true;

    T::GeometryInfo geometryInfo;
    int result = mContentServerPtr->getGeometryInfoById(0,generationId,geometryId,levelId,geometryInfo);
    if (result == 0  &&  geometryInfo.mStatus == T::GeometryInfo::Status::Ready)
    {
      if (mReadyGeometryList.size() > 100000)
        mReadyGeometryList.clear();

      mReadyGeometryList.insert(key);
      return true;
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::parseFunction(
    QueryParameter& queryParam,
    const std::string& paramStr,
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
      Fmi::Exception exception(BCP, "The recursion is too deep!");
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
          newParam.mType = queryParam.mType;
          newParam.mLocationType = queryParam.mLocationType;
          newParam.mFlags = queryParam.mFlags;
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
            getParameterStringInfo(fParam->second, ':', paramName, newParam.mGeometryId, newParam.mParameterLevelId, newParam.mParameterLevel, newParam.mForecastType, newParam.mForecastNumber,
                newParam.mProducerName,newParam.mProducerId, newParam.mGenerationFlags, newParam.mAreaInterpolationMethod, newParam.mTimeInterpolationMethod, newParam.mLevelInterpolationMethod);
          }

          parseFunction(newParam, newParam.mParam, newParam.mFunction, newParam.mFunctionParams, recursionCounter + 2, additionalParameterList);

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
      getParameterStringInfo(paramStr, ':', paramName, queryParam.mGeometryId, queryParam.mParameterLevelId, queryParam.mParameterLevel, queryParam.mForecastType, forecastNumberVec, producerName,
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
          p += sprintf(p, "%s:%s:%d:%d:%d:%d:%d:%llu", paramName.c_str(), producerName.c_str(), queryParam.mGeometryId, queryParam.mParameterLevelId, queryParam.mParameterLevel, queryParam.mForecastType,
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
    Fmi::Exception exception(BCP, "Parsing failed!", nullptr);
    exception.addParameter("ParamStr", paramStr);
    throw exception;
  }
}






void ServiceImplementation::getProducers(Query& query, Producer_vec& producers)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mProducerList_modificationLock);

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
          producers.emplace_back(std::pair<std::string, T::GeometryId>(it->first, it->second));
        }
        else
        {
          std::string alias;
          if (mProducerAliasFileCollection.getAlias(*pName, alias))
          {
            if (strcasecmp(alias.c_str(), it->first.c_str()) == 0)
            {
              producers.emplace_back(std::pair<std::string, T::GeometryId>(it->first, it->second));
            }
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::getProducers(const std::string& producerName, Producer_vec& producers)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mProducerList_modificationLock);
    for (auto it = mProducerList.begin(); it != mProducerList.end(); ++it)
    {
      if (strcasecmp(producerName.c_str(), it->first.c_str()) == 0)
      {
        producers.emplace_back(std::pair<std::string, T::GeometryId>(it->first, it->second));
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
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

        std::vector<T::ForecastNumber> forecastNumberVec;

        getParameterStringInfo(qParam->mParam, ':', qParam->mParameterKey, qParam->mGeometryId, qParam->mParameterLevelId, qParam->mParameterLevel, qParam->mForecastType, forecastNumberVec, qParam->mProducerName,
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
            p += sprintf(p, "%s:%s:%d:%d:%d:%d:%d:%llu", qParam->mParameterKey.c_str(), qParam->mProducerName.c_str(), qParam->mGeometryId, qParam->mParameterLevelId, qParam->mParameterLevel, qParam->mForecastType,
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
        }

        if (qParam->mParameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID)
        {
          Identification::NewbaseParameterDef paramDef;
          if (Identification::gridDef.getNewbaseParameterDefById(toUInt32(qParam->mParameterKey), paramDef))
          {
            PRINT_DATA(mDebugLog, "   + Newbase name: %s\n", paramDef.mParameterName.c_str());
            qParam->mParam = paramDef.mParameterName;
          }
        }

        // If the parameter is a symbolic name (defined in the configuration file) then
        // we should use its definition.

        std::string alias;
        if (getAlias(qParam->mParameterKey, alias))
        {
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
        }
      }

      // Adding parameters that are required by functions into the query.

      for (auto p = additionalParameterList.begin(); p != additionalParameterList.end(); ++p)
        query.mQueryParameterList.emplace_back(*p);

      PRINT_DATA(mDebugLog, "\n");
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::executeQueryFunctions(Query& query)
{
  FUNCTION_TRACE
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
          std::shared_ptr<ParameterValues> pValues = std::make_shared<ParameterValues>();
          pValues->mForecastTimeUTC = *tt;

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
                  if (producerId == 0 && q->mValueList[tCount]->mProducerId > 0)
                  {
                    producerId = q->mValueList[tCount]->mProducerId;
                    generationId = q->mValueList[tCount]->mGenerationId;
                    geometryId = q->mValueList[tCount]->mGeometryId;
                  }

                  T::GridValue rec;
                  if (q->mValueList[tCount]->mValueList.getGridValueByIndex(v,rec))
                  {
                    if (rec.mValueString.length() > 0)
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
                parameters.emplace_back(a);
                areaParameters.emplace_back(a);

                if (areaCnt && v == 0)
                  extParameters.emplace_back(a);
              }
              else
              {
                for (auto aa = valueList.begin(); aa != valueList.end(); ++aa)
                {
                  parameters.emplace_back(*aa);
                  areaParameters.emplace_back(*aa);

                  if (areaCnt && v == 0)
                    extParameters.emplace_back(*aa);
                }
              }
            }

            std::string f = qParam->mFunction.substr(0, 1);

            if (f != "@")
            {
              auto functionPtr = mFunctionCollection.getFunction(qParam->mFunction);
              if (functionPtr)
              {
                double val = functionPtr->executeFunctionCall1(parameters);
                T::GridValue rec(lastRec.mX, lastRec.mY, val);
                pValues->mValueList.addGridValue(rec);
              }
              else
              {
                std::string function;
                uint type = mLuaFileCollection.getFunction(qParam->mFunction, function);
                switch (type)
                {
                  case 1:
                  {
                    double val = mLuaFileCollection.executeFunctionCall1(function, parameters);
                    T::GridValue rec(lastRec.mX, lastRec.mY, val);
                    pValues->mValueList.addGridValue(rec);
                  }
                  break;

                  case 5:
                  {
                    std::string val = mLuaFileCollection.executeFunctionCall5(function, query.mLanguage, parameters);
                    T::GridValue rec(lastRec.mX, lastRec.mY, val);
                    pValues->mValueList.addGridValue(rec);
                  }
                  break;
                }
              }

              pValues->mProducerId = producerId;
              pValues->mGenerationId = generationId;
              pValues->mGeometryId = geometryId;
            }
          }

          std::string f = qParam->mFunction.substr(0, 1);
          if (areaCnt && f != "@")
          {
            double val = mLuaFileCollection.executeFunctionCall1(qParam->mFunction, extParameters);
            T::GridValue rec(-1000, -1000, val);
            pValues->mValueList.addGridValue(rec);
          }

          if (qParam->mFunction.substr(0, 1) == "@")
          {
            std::string func = qParam->mFunction.substr(1);

            auto functionPtr = mFunctionCollection.getFunction(func);
            if (functionPtr)
            {
              double val = functionPtr->executeFunctionCall1(areaParameters);
              T::GridValue rec(-1000, -1000, val);
              pValues->mValueList.addGridValue(rec);
            }
            else
            {
              double val = mLuaFileCollection.executeFunctionCall1(func, areaParameters);
              T::GridValue rec(-1000, -1000, val);
              pValues->mValueList.addGridValue(rec);
            }
          }

          if (areaCnt)
          {
            if (pValues->mValueList.getLength() > 0)
            {
              T::GridValue rec;
              pValues->mValueList.getGridValueByIndex(0,rec);
              T::ParamValue val = rec.mValue;
              pValues->mValueList.clear();
              T::GridValue rec2(-1000, -1000, val);
              pValues->mValueList.addGridValue(rec2);
            }
          }
          qParam->mValueList.emplace_back(pValues);

          tCount++;
        }
      }
    }

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      query.print(stream, 0, 0);
      PRINT_DATA(mDebugLog, "\nQuery -object after function execution:\n");
      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
    }

    query.removeTemporaryParameters();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





int ServiceImplementation::executeTimeRangeQuery(Query& query)
{
  FUNCTION_TRACE
  try
  {
    Producer_vec producers;
    getProducers(query, producers);
    T::Coordinate_vec coordinates;
    std::string analysisTime = query.mAnalysisTime;
    std::unordered_set<uint> alternativeRequired;
    uint queryFlags = query.mFlags;
    int globalGeometryId = 0;
    bool acceptNotReadyGenerations = false;

    if (producers.size() == 0)
      return Result::NO_PRODUCERS_FOUND;

    // Getting geometries that support support the given coordinates.

    std::set<T::GeometryId> geometryIdList;

    if (query.mFlags & Query::Flags::AcceptNotReadyGenerations)
      acceptNotReadyGenerations = true;

    if (query.mGeometryIdList.size() > 0)
    {
      // The query defines the geometries that we need to use.
      geometryIdList = query.mGeometryIdList;
    }
    else
    if (query.mFlags & Query::Flags::GeometryHitNotRequired)
    {
      // The query defines that the requested coordinates does not need to
      // hit the area. So, all geometries are acceptable.
      Identification::gridDef.getGeometryIdList(geometryIdList);
    }
    else
    {
      const char *geometryIdStr = query.mAttributeList.getAttributeValue("grid.geometryId");
      if (geometryIdStr != nullptr)
      {
        // Selecting geometries which cover the requested geometry.
        getGeometryIdListByGeometryId(toInt32(geometryIdStr),geometryIdList);
      }
      else
      {
        const char *gridUrnStr = query.mAttributeList.getAttributeValue("grid.urn");
        if (gridUrnStr != nullptr)
        {
          // All geometries are allowed.
          Identification::gridDef.getGeometryIdList(geometryIdList);
        }
        else
        {
          // Selecting valid geometries according to given coordiantes.
          getGeometryIdListByCoordinates(producers,query.mAreaCoordinates, geometryIdList);
        }
      }
    }

    std::unordered_map<std::string,uint > parameterProducers;

    // Parsing parameters and functions in the query.

    updateQueryParameters(query);

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
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

      if (qParam->mFunction.length() == 0 && ((qParam->mFlags & QueryParameter::Flags::AlternativeParameter) == 0 || alternativeRequired.find(qParam->mId) != alternativeRequired.end()))
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
        ulonglong generationFlags = qParam->mGenerationFlags;
        int geometryId = qParam->mGeometryId;
        if (generationFlags == 0)
          generationFlags = query.mGenerationFlags;

        bool useAlternative = false;

        getParameterStringInfo(qParam->mParam, ':', paramName, geometryId, paramLevelId, paramLevel, forecastType, forecastNumber, producerName,producerId, generationFlags, areaInterpolationMethod,
            timeInterpolationMethod, levelInterpolationMethod);

        if (paramName.c_str()[0] == '$')
          paramName = paramName.c_str()+1;

        try
        {
          std::size_t paramHash = 0;
          boost::hash_combine(paramHash,paramName);

          if (qParam->mAlternativeParamId != 0  &&  geometryId > 0 && !isGeometryValid(geometryId,query.mAreaCoordinates))
          {
            useAlternative = true;
            //if (alternativeRequired.find(qParam->mAlternativeParamId) == alternativeRequired.end())
              alternativeRequired.insert(qParam->mAlternativeParamId);
          }

          if (qParam->mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN  &&  !useAlternative)
          {
            time_t startTime = query.mStartTime;
            time_t endTime = query.mEndTime;

            if (qParam->mTimestepsBefore > 0)
            {
              startTime = startTime - ((qParam->mTimestepsBefore + 1) * qParam->mTimestepSizeInMinutes * 60);
            }

            if (qParam->mTimestepsAfter > 0)
            {
              endTime = endTime + (qParam->mTimestepsAfter * qParam->mTimestepSizeInMinutes * 60);
            }

            if ((query.mFlags & Query::Flags::StartTimeFromData) != 0)
            {
              startTime = 0;  // Start time is the start time of the data
            }

            if ((query.mFlags & Query::Flags::EndTimeFromData) != 0)
            {
              endTime = 0xFFFFFFFF;    // End time is the end time of the data
            }

            if (producerId == 0)
            {
              auto it = parameterProducers.find(paramName + ":" + producerName);
              if (it != parameterProducers.end())
              {
                producerId = it->second;
              }
            }

            Producer_vec tmpProducers;
            if (producerName > " ")
            {
              if (geometryId > 0)
                tmpProducers.emplace_back(std::pair<std::string, T::GeometryId>(producerName, geometryId));
              else
                getProducers(producerName,tmpProducers);
            }
            else
              tmpProducers = producers;

            std::set<T::GeometryId> geomIdList;
            if (geometryId > 0)
              geomIdList.insert(geometryId);
            else
            if (globalGeometryId > 0  &&  producerName.empty()  &&  qParam->mProducerId == 0)
              geomIdList.insert(globalGeometryId);
            else
              geomIdList = geometryIdList;

            if (qParam->mType != QueryParameter::Type::PointValues &&  geomIdList.size() == 0)
            {
              for (auto pp = tmpProducers.begin(); pp != tmpProducers.end(); ++pp)
              {
                geomIdList.insert(pp->second);
              }
            }

            std::string producerStr;

            getGridValues(qParam->mType,tmpProducers, geomIdList, producerId, analysisTime, generationFlags, acceptNotReadyGenerations, paramName, paramHash, paramLevelId, paramLevel, forecastType,
                forecastNumber, queryFlags, parameterFlags, areaInterpolationMethod, timeInterpolationMethod, levelInterpolationMethod, startTime, endTime, query.mTimesteps,
                query.mTimestepSizeInMinutes,qParam->mLocationType, query.mCoordinateType, query.mAreaCoordinates, qParam->mContourLowValues, qParam->mContourHighValues, query.mAttributeList,
                query.mRadius, query.mMaxParameterValues, qParam->mPrecision, qParam->mValueList,qParam->mCoordinates,producerStr,0);

            if (qParam->mValueList.size() > 0 /*|| ((parameterFlags & QueryParameter::Flags::NoReturnValues) != 0  &&  (qParam->mValueList[0].mFlags & ParameterValues::Flags::DataAvailable) != 0)*/)
            {
              if (producerId == 0 && qParam->mValueList[0]->mProducerId != 0)
              {
                parameterProducers.insert(std::pair<std::string, uint>(paramName + ":" + producerName, qParam->mValueList[0]->mProducerId));
              }

              // If there are no geometry defined, then we should not change the first geometry we'll get

              if (geometryId <= 0  &&  qParam->mValueList[0]->mGeometryId > 0)
                globalGeometryId = qParam->mValueList[0]->mGeometryId;


              if ((query.mFlags & Query::Flags::SameAnalysisTime) != 0  &&  analysisTime != qParam->mValueList[0]->mAnalysisTime)
              {
                // The query requires that we use the same analysis time with all parameters
                analysisTime = qParam->mValueList[0]->mAnalysisTime;
                queryFlags = queryFlags | Query::Flags::AnalysisTimeMatchRequired;
              }

              uint len = qParam->mValueList[0]->mValueList.getLength();
              if (coordinates.size() == 0  &&  len > 0)
              {
                for (uint s = 0; s<len; s++)
                {
                  T::GridValue *val = qParam->mValueList[0]->mValueList.getGridValuePtrByIndex(s);
                  coordinates.emplace_back(T::Coordinate(val->mX,val->mY));
                }
              }

              if (len == 0  || ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0  &&  (qParam->mValueList[0]->mFlags & ParameterValues::Flags::DataAvailable) == 0))
              {
                alternativeRequired.insert(qParam->mAlternativeParamId);
              }
            }
            else
            {
              alternativeRequired.insert(qParam->mAlternativeParamId);
            }
          }
        }
        catch (...)
        {
          Fmi::Exception exception(BCP, "Operation failed!", nullptr);
          exception.printError();
        }
      }
    }

    // Finding out which forecast time are found from the forecast data. The point is that different
    // parameters might contain different forecast times, and we want a list of all forecast times.

    std::set<time_t> timeList;
    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
    {
      for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end(); ++it)
      {
        timeList.insert((*it)->mForecastTimeUTC);
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
                && (((*it)->mForecastTimeUTC < query.mStartTime || (*it)->mForecastTimeUTC > query.mEndTime) ||
                    ((*it)->mForecastTimeUTC == query.mStartTime &&  (query.mFlags & Query::Flags::StartTimeNotIncluded) != 0))
            )
              (*it)->mFlags = (*it)->mFlags | QueryServer::ParameterValues::Flags::AggregationValue;

            if ((*it)->mForecastTimeUTC < *tt)
              cnt++;
            else
            if ((*it)->mForecastTimeUTC == *tt)
            {
              found = true;

              if ((*it)->mValueList.getLength() == 0)
              {
                getAdditionalValues(qParam->mSymbolicName,query.mCoordinateType,coordinates,*(*it));
              }
            }
          }

          if (!found)
          {
            // The forecast time was not found from the current parameter. Adding the forecast time
            // with an empty value list.

            std::shared_ptr<ParameterValues> pValues = std::make_shared<ParameterValues>();
            pValues->mForecastTimeUTC = *tt;

            if (qParam->mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN)
            {
              pValues->mFlags = pValues->mFlags | QueryServer::ParameterValues::Flags::AdditionalValue;
            }

            getAdditionalValues(qParam->mSymbolicName,query.mCoordinateType,coordinates,*pValues);

            if ((query.mFlags & Query::Flags::StartTimeFromData) == 0 && (*tt < query.mStartTime || *tt > query.mEndTime))
              pValues->mFlags = pValues->mFlags | QueryServer::ParameterValues::Flags::AggregationValue;

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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





int ServiceImplementation::executeTimeStepQuery(Query& query)
{
  FUNCTION_TRACE
  try
  {
    Producer_vec producers;
    getProducers(query, producers);
    T::Coordinate_vec coordinates;
    std::string analysisTime = query.mAnalysisTime;
    uint queryFlags = query.mFlags;
    int globalGeometryId = 0;
    std::unordered_set<uint> alternativeRequired;
    bool acceptNotReadyGenerations = false;

    if (query.mFlags & Query::Flags::AcceptNotReadyGenerations)
      acceptNotReadyGenerations = true;

    if (producers.size() == 0)
      return Result::NO_PRODUCERS_FOUND;

    std::set<T::GeometryId> geometryIdList;

    if (query.mGeometryIdList.size() > 0)
    {
      geometryIdList = query.mGeometryIdList;
    }
    else
    if ((query.mFlags & Query::Flags::GeometryHitNotRequired))
    {
      Identification::gridDef.getGeometryIdList(geometryIdList);
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
          getGeometryIdListByCoordinates(producers,query.mAreaCoordinates, geometryIdList);
        }
      }
    }

    //std::unordered_map<std::string,uint> parameterProducers;

    // Parsing parameters and functions in the query.

    updateQueryParameters(query);

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
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


    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
    {
      if (qParam->mFunction.length() == 0 && ((qParam->mFlags & QueryParameter::Flags::AlternativeParameter) == 0 || alternativeRequired.find(qParam->mId) != alternativeRequired.end()))
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
        ulonglong generationFlags = qParam->mGenerationFlags;
        int geometryId = qParam->mGeometryId;
        if (generationFlags == 0)
          generationFlags = query.mGenerationFlags;

        bool useAlternative = false;

        getParameterStringInfo(qParam->mParam, ':', paramName, geometryId,paramLevelId, paramLevel, forecastType, forecastNumber, producerName, producerId, generationFlags, areaInterpolationMethod,
            timeInterpolationMethod, levelInterpolationMethod);

        if (paramName.c_str()[0] == '$')
          paramName = paramName.c_str() + 1;

        std::size_t paramHash = 0;
        boost::hash_combine(paramHash,paramName);

        if (qParam->mAlternativeParamId != 0  &&  geometryId > 0 && !isGeometryValid(geometryId,query.mAreaCoordinates))
        {
          useAlternative = true;
          alternativeRequired.insert(qParam->mAlternativeParamId);
        }

        std::set<time_t> *forecastTimeList = &query.mForecastTimeList;
        std::set<time_t> forecastTimeList2;

        if (qParam->mTimestepsBefore > 0 || qParam->mTimestepsAfter > 0)
        {
          forecastTimeList = &forecastTimeList2;
          forecastTimeList2 = query.mForecastTimeList;
          for (const auto &fTime : query.mForecastTimeList)
          {
            if (qParam->mTimestepsBefore > 0)
            {
              time_t ss = fTime - ((qParam->mTimestepsBefore + 1) * qParam->mTimestepSizeInMinutes * 60);

              for (uint t = 0; t < qParam->mTimestepsBefore; t++)
              {
                ss = ss + (qParam->mTimestepSizeInMinutes * 60);
                forecastTimeList2.insert(ss);
              }
            }

            if (qParam->mTimestepsAfter > 0)
            {
              time_t ss = fTime;

              for (uint t = 0; t < qParam->mTimestepsAfter; t++)
              {
                ss = ss + (qParam->mTimestepSizeInMinutes * 60);
                forecastTimeList2.insert(ss);
              }
            }
          }
        }


        if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
        {
          std::stringstream stream;
          qParam->print(stream, 5, 0);
          PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
        }


        // Processing time steps in reverse order. The idea is that we want to
        // find a generation that contains the newest timestep if the generation
        // is not defined. It is possible that the newest generation do not contain
        // the requested timestep when the generation is still under construction.

        ulonglong gflags = generationFlags;

        int ftLen = forecastTimeList->size();
        qParam->mValueList.reserve(ftLen);

        std::vector<std::shared_ptr<ParameterValues>> tmpValueList;
        tmpValueList.reserve(ftLen);

        Producer_vec tmpProducers;
        std::set<T::GeometryId> tmpGeomIdList;


        for (auto fTime = forecastTimeList->rbegin(); fTime != forecastTimeList->rend(); ++fTime)
        {
          std::shared_ptr<ParameterValues> valueList = std::make_shared<ParameterValues>();
          tmpValueList.emplace_back(valueList);

          try
          {
            /*
            if (producerId == 0)
            {
              auto it = parameterProducers.find(paramName + ":" + producerName);
              if (it != parameterProducers.end())
              {
                producerId = it->second;
              }
            }
            */

            if (qParam->mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN  &&  !useAlternative)
            {
              Producer_vec *producersPtr = &producers;
              std::set<T::GeometryId> *geomIdListPtr = &geometryIdList;

              if (producerName > " ")
              {
                if (tmpProducers.size() == 0)
                {
                  if (geometryId > 0)
                    tmpProducers.emplace_back(std::pair<std::string, T::GeometryId>(producerName, geometryId));
                  else
                    getProducers(producerName,tmpProducers);
                }
              }

              if (tmpProducers.size() > 0)
                producersPtr = &tmpProducers;

              if (geometryId > 0)
              {
                if (tmpGeomIdList.size() == 0)
                  tmpGeomIdList.insert(geometryId);
              }
              else
              if (globalGeometryId > 0  &&  producerName.empty()  &&  qParam->mProducerId == 0)
                tmpGeomIdList.insert(globalGeometryId);

              if (qParam->mType != QueryParameter::Type::PointValues &&  tmpGeomIdList.size() == 0)
              {
                for (auto pp = tmpProducers.begin(); pp != tmpProducers.end(); ++pp)
                {
                  tmpGeomIdList.insert(pp->second);
                }
              }

              if (tmpGeomIdList.size() > 0)
                geomIdListPtr = &tmpGeomIdList;

              std::string producerStr;

              //printf("SEARCH %s %u\n",producerStr.c_str(),producerId);

              getGridValues(qParam->mType,*producersPtr, *geomIdListPtr, producerId, analysisTime, gflags, reverseGenerations, acceptNotReadyGenerations, paramName, paramHash, paramLevelId, paramLevel, forecastType,
                  forecastNumber,queryFlags,parameterFlags, areaInterpolationMethod, timeInterpolationMethod, levelInterpolationMethod, *fTime, false, qParam->mLocationType,
                  query.mCoordinateType, query.mAreaCoordinates, qParam->mContourLowValues, qParam->mContourHighValues, query.mAttributeList,query.mRadius,qParam->mPrecision,*valueList,qParam->mCoordinates,producerStr);

              /*
              if (producerId == 0 && valueList->mProducerId != 0)
              {
                parameterProducers.insert(std::pair<std::string, uint>(paramName + ":" + producerName, valueList->mProducerId));
              }
              */

              //printf("SEARCH RESULT %s %u %u %u %u\n",producerStr.c_str(),producerId,valueList->mValueList.getLength(),parameterFlags,valueList->mFlags);
              bool concat = false;
              if (producerId == 0 && producersPtr->size() > 1 && (valueList->mValueList.getLength() == 0 && (valueList->mFlags & ParameterValues::Flags::DataAvailable) == 0))
              {
                //printf("CONCAT SEARCH %s\n",producerStr.c_str());
                auto vec = mProducerConcatMap.find(producerStr);
                if (vec != mProducerConcatMap.end()  &&  vec->second.size() > 0)
                {
                  //printf("CONCAT SEARCH %s\n",vec->second[0].first.c_str());
                  tmpGeomIdList.clear();
                  for (auto gg = vec->second.begin(); gg != vec->second.end(); ++gg)
                    tmpGeomIdList.insert(gg->second);

                  auto p1 = producersPtr;
                  auto p2 = geomIdListPtr;
                  producersPtr = &(vec->second);
                  geomIdListPtr = &tmpGeomIdList;
                  getGridValues(qParam->mType,*producersPtr, *geomIdListPtr, producerId, analysisTime, gflags, reverseGenerations, acceptNotReadyGenerations, paramName, paramHash, paramLevelId, paramLevel, forecastType,
                      forecastNumber,queryFlags,parameterFlags, areaInterpolationMethod, timeInterpolationMethod, levelInterpolationMethod, *fTime, false, qParam->mLocationType,
                      query.mCoordinateType, query.mAreaCoordinates, qParam->mContourLowValues, qParam->mContourHighValues, query.mAttributeList,query.mRadius,qParam->mPrecision,*valueList,qParam->mCoordinates,producerStr);

                  tmpGeomIdList.clear();
                  producersPtr = p1;
                  geomIdListPtr = p2;
                  /*
                  if ((qParam->mLocationType == QueryParameter::LocationType::Polygon || qParam->mLocationType == QueryParameter::LocationType::Circle || qParam->mLocationType == QueryParameter::LocationType::Path) &&
                      (valueList->mValueList.getLength() > 0 || ((parameterFlags & QueryParameter::Flags::NoReturnValues) != 0  &&  (valueList->mFlags & ParameterValues::Flags::DataAvailable) != 0)))
                  {
                    producerId = valueList->mProducerId;
                    globalGeometryId = valueList->mGeometryId;
                  }
                  */

                  concat = true;
                }
              }

              if (valueList->mValueList.getLength() == 0  || ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0  &&  (valueList->mFlags & ParameterValues::Flags::DataAvailable) == 0))
              {
                alternativeRequired.insert(qParam->mAlternativeParamId);
              }

              //printf("SEARCH END %s %u %u %d\n",producerStr.c_str(),producerId,valueList->mValueList.getLength(),(int)concat);

              if (valueList->mValueList.getLength() > 0  || ((parameterFlags & QueryParameter::Flags::NoReturnValues) != 0  &&  (valueList->mFlags & ParameterValues::Flags::DataAvailable) != 0))
              {
                if (!concat)
                {

                  // We have got some values. We should not change the producer anymore.
                  if (producerId == 0)
                    producerId = valueList->mProducerId;

                  // If there are no geometry defined, then we should not change the first geometry we'll get

                  if (geometryId <= 0  &&  valueList->mGeometryId > 0)
                    globalGeometryId = valueList->mGeometryId;

                  if ((query.mFlags & Query::Flags::SameAnalysisTime) != 0  &&  analysisTime != valueList->mAnalysisTime)
                  {
                    // The query requires that we use the same analysis time with all parameters
                    analysisTime = valueList->mAnalysisTime;
                    queryFlags = queryFlags | Query::Flags::AnalysisTimeMatchRequired;
                  }

                  if (timeInterpolationMethod == T::TimeInterpolationMethod::Forbidden || (qParam->mFlags & QueryParameter::Flags::SameAnalysisTime) != 0 || (query.mFlags & Query::Flags::SameAnalysisTime) != 0 )
                  {
                    // If the time interpolation is forbidden or same analysisTime is required, we should no change the generation.
                    gflags = valueList->mGenerationFlags;
                  }
                  else
                  {
                    // We should use this generation or older.
                    if (generationFlags == 0)
                      gflags = valueList->mGenerationFlags + (valueList->mGenerationFlags << 1);
                  }
                }

                // We should not change the forecast number.
                if (forecastType < 0  &&  forecastNumber < 0)
                {
                  forecastType = valueList->mForecastType;
                  forecastNumber = valueList->mForecastNumber;
                }

                uint len = valueList->mValueList.getLength();
                if (coordinates.size() == 0  &&  len > 0)
                {
                  for (uint s = 0; s<len; s++)
                  {
                    T::GridValue *val = valueList->mValueList.getGridValuePtrByIndex(s);
                    coordinates.emplace_back(T::Coordinate(val->mX,val->mY));
                  }
                }

              }
            }
          }
          catch (...)
          {
            Fmi::Exception exception(BCP, "Operation failed!", nullptr);
            exception.printError();
          }

          if (valueList->mValueList.getLength() == 0)
          {
            //valueList->mForecastTime = utcTimeFromTimeT(*fTime);
            valueList->mForecastTimeUTC = *fTime;
          }

          if (query.mForecastTimeList.find(*fTime) == query.mForecastTimeList.end())
            valueList->mFlags = valueList->mFlags | QueryServer::ParameterValues::Flags::AggregationValue;
        }

        for (auto aa = tmpValueList.rbegin(); aa != tmpValueList.rend(); aa++)
        {
          qParam->mValueList.emplace_back(*aa);
        }

      }
    }


    // Finding out which forecast time are found from the forecast data. The point is that different
    // parameters might contain different forecast times, and we want a list of all forecast times.

    std::set<time_t> timeList;
    std::unordered_set<time_t> additionalTimeList;

    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
    {
      for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end(); ++it)
      {
        timeList.insert((*it)->mForecastTimeUTC);

        if (((*it)->mFlags & QueryServer::ParameterValues::Flags::AggregationValue) != 0)
          additionalTimeList.insert((*it)->mForecastTimeUTC);
      }
    }

    // Going through all the found forecast times and making sure that each parameter contains the
    // current forecast time. If not, then the forecast time is added to the parameters's forecast
    // time list, but the actual value list of the current forecast time will be empty.

    query.mForecastTimeList.clear();

    for (const auto &tt : timeList)
    {
      query.mForecastTimeList.insert(tt);

      for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
      {
        if (qParam->mFunction.length() == 0)
        {
          bool found = false;
          uint cnt = 0;
          for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end() && !found; ++it)
          {
            if ((*it)->mForecastTimeUTC < tt)
              cnt++;
            else
            if ((*it)->mForecastTimeUTC == tt)
            {
              found = true;
              if ((*it)->mValueList.getLength() == 0)
              {
                getAdditionalValues(qParam->mSymbolicName,query.mCoordinateType,coordinates,*(*it));
              }
            }
          }

          if (!found)
          {
            // The forecast time was not found from the current parameter. Adding the forecast time
            // with an empty value list.

            std::shared_ptr<ParameterValues> pValues = std::make_shared<ParameterValues>();
            pValues->mForecastTimeUTC = tt;
            //pValues->mForecastTime = utcTimeFromTimeT(tt);

            if (qParam->mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN)
              pValues->mFlags = pValues->mFlags | QueryServer::ParameterValues::Flags::AdditionalValue;

            getAdditionalValues(qParam->mSymbolicName,query.mCoordinateType,coordinates,*pValues);

            if (additionalTimeList.find(tt) != additionalTimeList.end())
              pValues->mFlags = pValues->mFlags | QueryServer::ParameterValues::Flags::AggregationValue;

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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
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
    uint modificationOperation = 0;
    double_vec modificationParameters;
    uint len = contentInfoList.getLength();
    for (uint c = 0; c < len; c++)
    {
      T::ContentInfo* contentInfo = contentInfoList.getContentInfoByIndex(c);
      T::ParamValue value = 0;
      int res = mDataServerPtr->getGridValueByPoint(sessionId, contentInfo->mFileId, contentInfo->mMessageIndex, coordinateType, x, y, areaInterpolationMethod,modificationOperation,modificationParameters,value);
      if (res == DataServer::Result::OK)
      {
        valueList.addGridPointValue(new T::GridPointValue(contentInfo->mFileId, contentInfo->mMessageIndex, x, y, contentInfo->mParameterLevel, contentInfo->mForecastTimeUTC, value));
      }
      else
      {
        valueList.addGridPointValue(new T::GridPointValue(contentInfo->mFileId, contentInfo->mMessageIndex, x, y, contentInfo->mParameterLevel, contentInfo->mForecastTimeUTC, ParamValueMissing));
      }
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::conversionFunction(const std::string& conversionFunction, std::string& function, std::vector<std::string>& functionParams)
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::executeConversion(const std::string& function, std::vector<std::string>& functionParams,time_t forecastTime,T::Coordinate_vec& coordinates,T::ParamValue_vec& valueList, T::ParamValue_vec& newValueList)
{
  try
  {
    if (valueList.size() != coordinates.size())
    {
      Fmi::Exception exception(BCP, "The number of values is not the same as the number of coordinates!");
      exception.addParameter("valueList.size",Fmi::to_string(valueList.size()));
      exception.addParameter("coordinates.size",Fmi::to_string(coordinates.size()));
      throw exception;
    }

    auto functionPtr = mFunctionCollection.getFunction(function);

    int size = valueList.size();
    newValueList.reserve(size);
    for (int t=0; t<size; t++)
    {
      std::vector<double> parameters;
      for (auto fp = functionParams.begin(); fp != functionParams.end(); ++fp)
      {
        std::string f = fp->substr(0, 1);
        std::string p = fp->substr(1);

        if (*fp == "$")
          parameters.emplace_back(valueList[t]);
        else
        if (f == "$"  &&  !p.empty())
        {
          T::ParamValue val = getAdditionalValue(p,forecastTime,coordinates[t].x(), coordinates[t].y());
          parameters.emplace_back(val);
        }
        else
          parameters.emplace_back(toDouble(fp->c_str()));
      }

      T::ParamValue newValue = 0;
      if (functionPtr)
        newValue = functionPtr->executeFunctionCall1(parameters);
      else
        newValue = mLuaFileCollection.executeFunctionCall1(function, parameters);

      newValueList.emplace_back(newValue);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






void ServiceImplementation::executeConversion(const std::string& function, std::vector<std::string>& functionParams,T::ParamValue_vec& valueList, T::ParamValue_vec& newValueList)
{
  try
  {
    auto functionPtr = mFunctionCollection.getFunction(function);
    int size = valueList.size();
    newValueList.reserve(size);
    for (int t=0; t<size; t++)
    {
      std::vector<double> parameters;
      for (auto fp = functionParams.begin(); fp != functionParams.end(); ++fp)
      {
        if (*fp == "$")
          parameters.emplace_back(valueList[t]);
        else
          parameters.emplace_back(toDouble(fp->c_str()));
      }
      T::ParamValue newValue = 0;
      if (functionPtr)
        newValue = functionPtr->executeFunctionCall1(parameters);
      else
        newValue = mLuaFileCollection.executeFunctionCall1(function, parameters);

      newValueList.emplace_back(newValue);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::executeConversion(const std::string& function, std::vector<std::string>& functionParams,time_t forecastTime,T::GridValueList& valueList)
{
  try
  {
    // ******************** TODO: Make this conversion better ******************
    boost::local_time::local_date_time utcTime(toTimeStamp(utcTimeFromTimeT(forecastTime)), nullptr);
    auto functionPtr = mFunctionCollection.getFunction(function);

    uint vLen = valueList.getLength();
    for (uint i = 0; i < vLen; i++)
    {
      auto gv = valueList.getGridValuePtrByIndex(i);
      if (gv != nullptr)
      {
        std::vector<double> parameters;
        for (auto fp = functionParams.begin(); fp != functionParams.end(); ++fp)
        {
          std::string f = fp->substr(0, 1);
          std::string p = fp->substr(1);

          if (*fp == "$")
            parameters.emplace_back(gv->mValue);
          else
          if (f == "$"  &&  !p.empty())
          {
            T::ParamValue val = getAdditionalValue(p,forecastTime,gv->mX, gv->mY);
            parameters.emplace_back(val);
          }
          else
            parameters.emplace_back(toDouble(fp->c_str()));
        }
        if (functionPtr)
          gv->mValue = functionPtr->executeFunctionCall1(parameters);
        else
          gv->mValue = mLuaFileCollection.executeFunctionCall1(function, parameters);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





ulonglong ServiceImplementation::getProducerHash(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    time_t currentTime = time(nullptr);
    ulonglong hash = 0;

    {
      AutoReadLock lock(&mProducerHashMap_modificationLock);
      auto rec = mProducerHashMap.find(producerId);
      if (rec != mProducerHashMap.end())
      {
        if ((currentTime - rec->second.checkTime) > 120)
        {
          rec->second.checkTime = currentTime;
          int result = mContentServerPtr->getHashByProducerId(0,producerId,hash);
          if (result == 0)
            rec->second.hash = hash;
          else
            rec->second.hash = 0;
        }
        return rec->second.hash;
      }
    }

    int result = mContentServerPtr->getHashByProducerId(0,producerId,hash);
    if (result == 0)
    {
      HashRec hrec;
      hrec.checkTime = currentTime;
      hrec.hash = hash;

      AutoWriteLock lock(&mProducerHashMap_modificationLock);
      mProducerHashMap.insert(std::pair<uint,HashRec>(producerId,hrec));
      return hash;
    }

    return 0;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP, "Operation failed!", nullptr);
    throw exception;
  }
}





int ServiceImplementation::getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint producerId,ulonglong producerHash,uint generationId,T::ParamKeyType parameterKeyType,const std::string& parameterKey,std::size_t parameterKeyHash,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,std::shared_ptr<T::ContentInfoList>& contentInfoList)
{
  try
  {
    uint fmiId = 0;
    if (parameterKeyType == T::ParamKeyTypeValue::FMI_NAME  &&  strncasecmp(parameterKey.c_str(),"FMI-",4) == 0)
    {
      parameterKeyType = T::ParamKeyTypeValue::FMI_ID;
      fmiId = toUInt32(parameterKey.c_str()+4);
    }

    std::size_t hash = 0;
    boost::hash_combine(hash,producerId);
    boost::hash_combine(hash,generationId);
//    boost::hash_combine(hash,parameterKeyType);
    //boost::hash_combine(hash,parameterKey);
    boost::hash_combine(hash,parameterKeyHash);
    boost::hash_combine(hash,parameterLevelId);
    boost::hash_combine(hash,forecastType);
    boost::hash_combine(hash,forecastNumber);
    boost::hash_combine(hash,geometryId);
    std::size_t hash3 = hash;
    boost::hash_combine(hash,level);
    boost::hash_combine(hash3,999999999);

    std::size_t hash2 = hash;
    boost::hash_combine(hash2,forecastTime);
    if (producerHash == 0)
      producerHash = getProducerHash(producerId);

    {
      //AutoReadLock readLock(&mContentSearchCache_modificationLock);
      auto it = mContentSearchCache.find(hash2);
      if (it != mContentSearchCache.end())
      {
        if (it->second.producerHash[0] == producerHash)
        {
          contentInfoList = it->second.contentInfoList[0];
          return Result::OK;
        }

        if (it->second.producerHash[1] == producerHash)
        {
          contentInfoList = it->second.contentInfoList[1];
          return Result::OK;
        }

        if (it->second.producerHash[2] == producerHash)
        {
          contentInfoList = it->second.contentInfoList[2];
          return Result::OK;
        }
      }
    }

    ContentCacheEntry_sptr entry;
    bool newEntry = false;

    {
      //AutoReadLock readLock(&mContentCache_modificationLock);
      auto cc = mContentCache.find(hash);
      auto cc3 = cc;

      if (cc != mContentCache.end())
      {
        entry = cc->second;
      }
      else
      {
        cc3 = mContentCache.find(hash3);
        if (cc3 != mContentCache.end())
        {
          entry = cc3->second;
          hash = hash3;
        }
        else
        {
          // No cache entry available

          entry.reset(new ContentCacheEntry());
          newEntry = true;

          time_t startTime = 0;
          time_t endTime = 0xFFFFFFFF;

          mContentServerPtr->getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,level,level,forecastType,forecastNumber,geometryId,startTime,endTime,0,entry->contentInfoList);

          if (entry->contentInfoList.getLength() == 0)
          {
            mContentServerPtr->getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,0,1000000000,forecastType,forecastNumber,geometryId,startTime,endTime,0,entry->contentInfoList);
            if (entry->contentInfoList.getLength() == 0)
              hash = hash3;
          }

          entry->generationId = generationId;
          entry->producerHash = producerHash;

          switch (parameterKeyType)
          {
            case T::ParamKeyTypeValue::FMI_ID:
              break;

            case T::ParamKeyTypeValue::FMI_NAME:
              entry->contentInfoList.setComparisonMethod(T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time);
              break;
          }
        }
      }
    }

    if (!newEntry && entry->producerHash != producerHash)
    {
      // Producer hash does not match
      //printf("*** UPDATE \n");
      //AutoWriteLock lock(&mContentCache_modificationLock);
      if (entry->producerHash != producerHash)
      {
        time_t startTime = 0;
        time_t endTime = 0xFFFFFFFF;

        mContentServerPtr->getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,level,level,forecastType,forecastNumber,geometryId,startTime,endTime,0,entry->contentInfoList);

        entry->producerHash = producerHash;
        entry->generationId = generationId;

        switch (parameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            entry->contentInfoList.setComparisonMethod(T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time);
            break;
        }
      }
    }

    std::shared_ptr<T::ContentInfoList> cList = std::make_shared<T::ContentInfoList>();
    {
      //AutoReadLock readLock(&mContentCache_modificationLock);
      switch (parameterKeyType)
      {
        case T::ParamKeyTypeValue::FMI_ID:
        {
          if (fmiId == 0)
            fmiId = toUInt32(parameterKey);

          entry->contentInfoList.getContentInfoListByFmiParameterIdAndGenerationId2(producerId,generationId,fmiId,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,*cList);
          if (cList->getLength() == 0)
            entry->contentInfoList.getContentInfoListByFmiParameterIdAndGenerationId(producerId,generationId,fmiId,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,*cList);
        }
        break;

        case T::ParamKeyTypeValue::FMI_NAME:
        {
          entry->contentInfoList.getContentInfoListByFmiParameterNameAndGenerationId2(producerId,generationId,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,*cList);
          if (cList->getLength() == 0)
            entry->contentInfoList.getContentInfoListByFmiParameterNameAndGenerationId(producerId,generationId,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,*cList);
        }
        break;
      }

      contentInfoList = cList;
      //AutoWriteLock lock(&mContentSearchCache_modificationLock);

      if (mContentSearchCache_records >= mContentSearchCache_maxRecords)
      {
        //printf("CLEAR CONTENT SEARCH CACHE %u %ld %ld\n",mActiveContentSearchCache,mContentSearchCache_records,mContentSearchCache[mActiveContentSearchCache].size());
        mContentSearchCache.clear();
        mContentSearchCache_records = 0;
      }

      auto rr = mContentSearchCache.find(hash2);
      if (rr == mContentSearchCache.end())
      {
        // Curren cache record is new. We should add in into the cache.

        ContentSearchCacheEntry rc;
        rc.contentInfoList[0] = cList;
        rc.producerHash[0] = producerHash;
        rc.producerHash[1] = 0;
        rc.producerHash[2] = 0;
        rc.generationId = generationId;
        mContentSearchCache.insert(std::pair<std::size_t,ContentSearchCacheEntry>(hash2,rc));
        mContentSearchCache_records += cList->getLength();
      }
      else
      {
        // Current cache record is already in use, but it has old information. We should update this information.
        if (rr->second.producerHash[1] == 0  &&  rr->second.producerHash[0] != producerHash)
        {
          rr->second.contentInfoList[1] = cList;
          rr->second.producerHash[1] = producerHash;
        }
        else
        if (rr->second.producerHash[2] == 0  &&  rr->second.producerHash[0] != producerHash &&  rr->second.producerHash[1] != producerHash)
        {
          rr->second.contentInfoList[2] = cList;
          rr->second.producerHash[2] = producerHash;
        }
        else
        {
          // We need to clear the cache

          mContentSearchCache_records = mContentSearchCache_maxRecords;
        }
      }
    }

    if (newEntry)
    {
      //AutoWriteLock lock(&mContentCache_modificationLock);
      if (mContentCache_records > mContentCache_maxRecords)
      {
        //printf("CLEAR CONTENT CACHE\n");
        mContentCache.clear();
        mContentCache_records = 0;
      }

      if (mContentCache.find(hash) == mContentCache.end())
      {
        //printf("INSERT %ld\n",mContentCache.size());
        mContentCache.insert(std::pair<std::size_t,ContentCacheEntry_sptr>(hash,entry));
        mContentCache_records += entry->contentInfoList.getLength();
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    //mContentCache_modificationLock.writeUnlock();
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getSpecialValues(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
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
    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction = conversionFunction(pInfo.mConversionFunction, function, functionParams);

    if (conversionByFunction && mDataServerMethodsEnabled)
    {
      std::string f = toUpperString(function);
      auto p = mOperationNames.find(f);
      if (p != mOperationNames.end())
      {
        modificationOperation = p->second;
        for (auto it = functionParams.begin(); it != functionParams.end(); ++it)
        {
          if (*it != "$")
            modificationParameters.push_back(atof(it->c_str()));
        }
        conversionByFunction = false;
      }
    }

    std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
    time_t fTime = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,
        paramLevelId,paramLevel,forecastType,forecastNumber,parameterFlags,contentList);

    PRINT_DATA(mDebugLog, "         + Found %u content records (fTime = %ld)\n", contentList->getLength(),fTime);

    if (fTime == 0)
      return false;

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      contentList->print(stream, 0, 4);
      PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
    }

    uint contentLen = contentList->getLength();
    if (contentLen == 0)
      return false;

    T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelId = paramLevelId;
    //valueList.mForecastTime = utcTimeFromTimeT(forecastTime);
    valueList.mForecastTimeUTC = forecastTime;
    valueList.mProducerId = contentInfo1->mProducerId;
    valueList.mGenerationId = contentInfo1->mGenerationId;
    valueList.mGeometryId = contentInfo1->mGeometryId;
    valueList.mFileId[0] = contentInfo1->mFileId;
    valueList.mMessageIndex[0] = contentInfo1->mMessageIndex;
    valueList.mModificationTime = contentInfo1->mModificationTime;
    valueList.mAnalysisTime = analysisTime;
    valueList.mForecastType = contentInfo1->mForecastType;
    valueList.mForecastNumber = contentInfo1->mForecastNumber;
    valueList.mParameterLevel = contentInfo1->mParameterLevel;
    valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC == fTime)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        valueList.mFlags = ParameterValues::Flags::DataAvailable;

        double_vec valueVector;
        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueVectorByPoint(0, contentInfo1->mFileId, contentInfo1->mMessageIndex, coordinateType, x, y, areaInterpolationMethod,modificationOperation,modificationParameters, valueVector);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueVectorByPoint");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }
        }

        uint vSize = valueVector.size();
        PRINT_DATA(mDebugLog, "-- Value vector size %u\n",vSize);
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
            executeConversion(function, functionParams, forecastTime, list);

          for (uint vv = 0; vv <= sLen; vv++)
            p += sprintf(p, "%f;", valueVector[vv]);

          uint len = list.getLength();
          for (uint vv = 0; vv < len; vv++)
          {
            T::GridValue rec;
            list.getGridValueByIndex(vv,rec);
            p += sprintf(p, "%f;", rec.mValue);
          }

          T::GridValue rec(x, y, tmp);
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
        valueList.clear();
        valueList.mProducerId = producerInfo.mProducerId;
        valueList.mParameterKeyType = pInfo.mParameterKeyType;
        valueList.mParameterKey = pInfo.mParameterKey;
        valueList.mForecastTimeUTC = forecastTime;
        return false;
      }
    }

    if (contentLen == 2  &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden)
    {
      if (!(contentInfo1->mForecastTimeUTC < fTime && contentInfo2->mForecastTimeUTC > fTime))
      {
        Fmi::Exception exception(BCP, "Unexpected result!");
        exception.addDetail("The given forecast time should been between the found content times.");
        exception.addParameter("Content 1 ForecastTime", contentInfo1->getForecastTime());
        exception.addParameter("Content 2 ForecastTime", contentInfo2->getForecastTime());
        exception.addParameter("Request ForecastTime", utcTimeFromTimeT(fTime));
        throw exception;
      }

      // We did not find a grid with the exact forecast time, but we find grids that
      // are before and after the current forecast time. This means that we should do
      // some time interpolation.

      // Fetching data from the grids.

      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

      double_vec valueVector1;
      double_vec valueVector2;

      if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
      {
        int result1 = mDataServerPtr->getGridValueVectorByPoint(0, contentInfo1->mFileId, contentInfo1->mMessageIndex, coordinateType, x, y, areaInterpolationMethod,modificationOperation,modificationParameters, valueVector1);
        if (result1 != 0)
        {
          Fmi::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueBlockByPoint");
          exception.addParameter("Message", DataServer::getResultString(result1));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        int result2 = mDataServerPtr->getGridValueVectorByPoint(0, contentInfo2->mFileId, contentInfo2->mMessageIndex, coordinateType, x, y, areaInterpolationMethod,modificationOperation,modificationParameters, valueVector2);
        if (result2 != 0)
        {
          Fmi::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueVectorByPoint");
          exception.addParameter("Message", DataServer::getResultString(result2));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }
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
        timeInterpolation(list1,list2,contentInfo1->mForecastTimeUTC, contentInfo2->mForecastTimeUTC,fTime,timeInterpolationMethod,list);

        if (conversionByFunction)
          executeConversion(function, functionParams, forecastTime, list);

        for (uint s = 0; s <= sLen; s++)
          p += sprintf(p, "%f;", valueVector1[s]);

        uint len = list.getLength();
        for (uint s = 0; s < len; s++)
        {
          T::GridValue rec;
           list.getGridValueByIndex(s,rec);
          p += sprintf(p, "%f;", rec.mValue);
        }

        T::GridValue rec(x, y, tmp);
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getValueVectors(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
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
    T::AttributeList& queryAttributeList,
    uint& newProducerId,
    short& precision,
    ParameterValues& valueList,
    T::Coordinate_vec& coordinates)
{
  FUNCTION_TRACE
  try
  {
    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction = conversionFunction(pInfo.mConversionFunction, function, functionParams);
    T::ParamValue_vec valueVector;

    if (conversionByFunction && mDataServerMethodsEnabled)
    {
      std::string f = toUpperString(function);
      auto p = mOperationNames.find(f);
      if (p != mOperationNames.end())
      {
        modificationOperation = p->second;
        for (auto it = functionParams.begin(); it != functionParams.end(); ++it)
        {
          if (*it != "$")
            modificationParameters.push_back(atof(it->c_str()));
        }
        conversionByFunction = false;
      }
    }

    std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
    time_t fTime = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,
        paramLevelId,paramLevel,forecastType,forecastNumber,parameterFlags,contentList);

    PRINT_DATA(mDebugLog, "         + Found %u content records (fTime = %ld)\n", contentList->getLength(),fTime);

    if (fTime == 0)
      return false;

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      contentList->print(stream, 0, 4);
      PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
    }

    uint contentLen = contentList->getLength();
    if (contentLen == 0)
      return false;

    T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList->getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList->getContentInfoByIndex(3);

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelId = paramLevelId;
    valueList.mForecastTimeUTC = forecastTime;
    valueList.mProducerId = contentInfo1->mProducerId;
    valueList.mGenerationId = contentInfo1->mGenerationId;
    valueList.mGenerationFlags = generationFlags;
    valueList.mGeometryId = contentInfo1->mGeometryId;
    valueList.mFileId[0] = contentInfo1->mFileId;
    valueList.mMessageIndex[0] = contentInfo1->mMessageIndex;
    valueList.mModificationTime = contentInfo1->mModificationTime;
    valueList.mAnalysisTime = analysisTime;
    valueList.mForecastType = contentInfo1->mForecastType;
    valueList.mForecastNumber = contentInfo1->mForecastNumber;
    valueList.mParameterLevel = contentInfo1->mParameterLevel;
    valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;

    queryAttributeList.setAttribute("grid.timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
    queryAttributeList.setAttribute("grid.areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));
    queryAttributeList.setAttribute("grid.levelInterpolationMethod",Fmi::to_string(levelInterpolationMethod));

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC == fTime)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        valueList.mFlags = ParameterValues::Flags::DataAvailable;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;

          switch (locationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridValueVectorByGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,queryAttributeList,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((parameterFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  uint width = 0;
                  uint height = 0;
                  T::Coordinate_svec sCoordinates(new T::Coordinate_vec());
                  Identification::gridDef.getGridLatLonCoordinatesByGeometry(queryAttributeList,sCoordinates,width,height);
                  coordinates = *sCoordinates;
                }
              }
              break;

            case QueryParameter::LocationType::Grid:
              result = mDataServerPtr->getGridValueVectorByCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,coordinateType,gridCoordinates,areaInterpolationMethod,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((parameterFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  coordinates = gridCoordinates;
                }
              }
              break;

            default:
              result = mDataServerPtr->getGridValueVector(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((parameterFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  T::GridCoordinates coord;
                  if (mDataServerPtr->getGridCoordinates(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,T::CoordinateTypeValue::LATLON_COORDINATES,coord) == 0)
                    coordinates = coord.mCoordinateList;
                }
              }
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueVector");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }
          if (conversionByFunction)
             executeConversion(function, functionParams, forecastTime, coordinates, valueVector, valueList.mValueVector);
          else
            valueList.mValueVector = valueVector;
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        if ((parameterFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) == 0)
          coordinates.clear();

        return true;
      }
      else
      {
        // There is one content record in place, but its time does not match to
        // the requested forecast time. This is used for indicating that there
        // are content records available, but not for the requested time.
        // So, we should use this producer.

        newProducerId = producerInfo.mProducerId;
        valueList.clear();
        valueList.mProducerId = producerInfo.mProducerId;
        valueList.mParameterKeyType = pInfo.mParameterKeyType;
        valueList.mParameterKey = pInfo.mParameterKey;
        valueList.mForecastTimeUTC = forecastTime;
        return false;
      }
    }

    if (contentLen == 2)
    {
      if (contentInfo1->mForecastTimeUTC < fTime && contentInfo2->mForecastTimeUTC > fTime  &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden)
      {
        // We did not find a grid with the exact forecast time, but we find grids that
        // are before and after the current forecast time. This means that we should do
        // some time interpolation.

        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;

          switch (locationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridValueVectorByTimeAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,queryAttributeList,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((parameterFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  uint width = 0;
                  uint height = 0;
                  T::Coordinate_svec sCoordinates(new T::Coordinate_vec());
                  Identification::gridDef.getGridLatLonCoordinatesByGeometry(queryAttributeList,sCoordinates,width,height);
                  coordinates = *sCoordinates;
                }
              }
              break;

            case QueryParameter::LocationType::Grid:
              result = mDataServerPtr->getGridValueVectorByTimeAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,coordinateType,gridCoordinates,queryAttributeList,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((parameterFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  coordinates = gridCoordinates;
                }
              }
              break;

            default:
              result = mDataServerPtr->getGridValueVectorByTime(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,timeInterpolationMethod,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((parameterFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  T::GridCoordinates coord;
                  if (mDataServerPtr->getGridCoordinates(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,T::CoordinateTypeValue::LATLON_COORDINATES,coord) == 0)
                    coordinates = coord.mCoordinateList;
                }
              }
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueVectorByTimeAndGeometry");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
             executeConversion(function, functionParams, forecastTime, coordinates, valueVector, valueList.mValueVector);
          else
            valueList.mValueVector = valueVector;
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        if ((parameterFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) == 0)
          coordinates.clear();

        return true;
      }

      if (contentInfo1->mForecastTimeUTC == fTime  &&  contentInfo2->mForecastTimeUTC == fTime && contentInfo1->mParameterLevel < paramLevel && contentInfo2->mParameterLevel > paramLevel  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
      {
        // We did not find a grid with the exact level but we find grids that
        // are before and after the current level. This means that we should do
        // some level interpolation.

        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByLevelInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          valueList.mParameterLevel = paramLevel;

          int result = 0;

          switch (locationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridValueVectorByLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,queryAttributeList,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((parameterFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  uint width = 0;
                  uint height = 0;
                  T::Coordinate_svec sCoordinates(new T::Coordinate_vec());
                  Identification::gridDef.getGridLatLonCoordinatesByGeometry(queryAttributeList,sCoordinates,width,height);
                  coordinates = *sCoordinates;
                }
              }
              break;

            case QueryParameter::LocationType::Grid:
              result = mDataServerPtr->getGridValueVectorByLevelAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,coordinateType,gridCoordinates,queryAttributeList,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((parameterFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  coordinates = gridCoordinates;
                }
              }
              break;

            default:
              result = mDataServerPtr->getGridValueVectorByLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,levelInterpolationMethod,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((parameterFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  T::GridCoordinates coord;
                  if (mDataServerPtr->getGridCoordinates(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,T::CoordinateTypeValue::LATLON_COORDINATES,coord) == 0)
                    coordinates = coord.mCoordinateList;
                }
              }
              break;
          }

          if (conversionByFunction)
             executeConversion(function, functionParams, forecastTime, coordinates, valueVector, valueList.mValueVector);
          else
            valueList.mValueVector = valueVector;
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        if ((parameterFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) == 0)
          coordinates.clear();

        return true;
      }
    }

    if (contentLen == 4)
    {
      if (contentInfo1->mForecastTimeUTC < fTime  &&  contentInfo2->mForecastTimeUTC < fTime  &&  contentInfo3->mForecastTimeUTC > fTime  &&  contentInfo4->mForecastTimeUTC > fTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&  contentInfo3->mParameterLevel < paramLevel  &&  contentInfo4->mParameterLevel > paramLevel
          &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden   &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
      {
        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation | ParameterValues::Flags::DataAvailableByLevelInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
        valueList.mFileId[2] = contentInfo3->mFileId;
        valueList.mMessageIndex[2] = contentInfo3->mMessageIndex;
        valueList.mFileId[3] = contentInfo4->mFileId;
        valueList.mMessageIndex[3] = contentInfo4->mMessageIndex;
        valueList.mParameterLevel = paramLevel;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;

          switch (locationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridValueVectorByTimeLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,queryAttributeList,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((parameterFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  uint width = 0;
                  uint height = 0;
                  T::Coordinate_svec sCoordinates(new T::Coordinate_vec());
                  Identification::gridDef.getGridLatLonCoordinatesByGeometry(queryAttributeList,sCoordinates,width,height);
                  coordinates = *sCoordinates;
                }
              }
              break;

            case QueryParameter::LocationType::Grid:
              result = mDataServerPtr->getGridValueVectorByTimeLevelAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,coordinateType,gridCoordinates,queryAttributeList,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((parameterFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  coordinates = gridCoordinates;
                }
              }
              break;

            default:
              result = mDataServerPtr->getGridValueVectorByTimeAndLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((parameterFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  T::GridCoordinates coord;
                  if (mDataServerPtr->getGridCoordinates(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,T::CoordinateTypeValue::LATLON_COORDINATES,coord) == 0)
                    coordinates = coord.mCoordinateList;
                }
              }
              break;
          }

          if (conversionByFunction)
             executeConversion(function, functionParams, forecastTime, coordinates, valueVector, valueList.mValueVector);
          else
            valueList.mValueVector = valueVector;
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;


        if ((parameterFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) == 0)
          coordinates.clear();

        return true;
      }
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getGridFiles(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
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
    T::AttributeList& queryAttributeList,
    uint& newProducerId,
    short& precision,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    // **************** DISABLED *************************
#if 0
    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction = conversionFunction(pInfo.mConversionFunction, function, functionParams);
    T::ParamValue_vec valueVector;
    T::ParamValue_vec newValueVector;

    std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
    int result = getContentListByParameterGenerationIdAndForecastTime(0, producerInfo.mProducerId, producerInfo.mHash, generationId, pInfo.mParameterKeyType, pInfo.mParameterKey, pInfo.getKeyHash(),
        paramLevelId, paramLevel, forecastType, forecastNumber, producerGeometryId, forecastTime, contentList);

    if (result != 0)
    {
      Fmi::Exception exception(BCP, "ContentServer returns an error!");
      exception.addParameter("Service", "getContentListByParameterGenerationIdAndForecastTime");
      exception.addParameter("Message", ContentServer::getResultString(result));
      throw exception;
    }

    PRINT_DATA(mDebugLog, "         + Found %u content records\n", contentList->getLength());

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      contentList->print(stream, 0, 4);
      PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
    }

    uint contentLen = contentList->getLength();
    if (contentLen == 0)
      return false;

    T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList->getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList->getContentInfoByIndex(3);

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelId = paramLevelId;
    valueList.mForecastTimeUTC = forecastTime;
    valueList.mProducerId = contentInfo1->mProducerId;
    valueList.mGenerationId = contentInfo1->mGenerationId;
    valueList.mGenerationFlags = generationFlags;
    valueList.mGeometryId = contentInfo1->mGeometryId;
    valueList.mModificationTime = contentInfo1->mModificationTime;
    valueList.mAnalysisTime = analysisTime;
    valueList.mForecastType = contentInfo1->mForecastType;
    valueList.mForecastNumber = contentInfo1->mForecastNumber;
    valueList.mParameterLevel = contentInfo1->mParameterLevel;
    valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;


    T::AttributeList attrList;
    result = mDataServerPtr->getGridAttributeList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,attrList);
    if (result != 0)
    {
      Fmi::Exception exception(BCP, "DataServer returns an error!");
      exception.addParameter("Service", "getGridAttributeList");
      exception.addParameter("Message", DataServer::getResultString(result));
      std::string errorMsg = exception.getStackTrace();
      PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
      return false;
    }

    attrList.setCaseSensitive(false);


    std::string ftime = utcTimeFromTimeT(forecastTime);

    int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
    splitTimeString(ftime,year,month,day,hour,minute,second);

    const char *version1 = queryAttributeList.getAttributeValue("Grib1.IndicatorSection.EditionNumber");
    const char *version2 = queryAttributeList.getAttributeValue("Grib2.IndicatorSection.EditionNumber");

    GRID::GridFile newGridFile;
    GRID::Message *newMessage = nullptr;

    if (version1 != nullptr &&  strcmp(version1,"1") == 0)
    {
      PRINT_DATA(mDebugLog, "-- GRIB version 1\n");

      using namespace SmartMet::GRIB1;

      //newGridFile.setGridFile(T::FileTypeValue::Grib1);
      newMessage = newGridFile.newMessage(T::FileTypeValue::Grib1);

      // ### Setting default values for the parameters

      //newMessage->setProperty(Property::ProductSection::TableVersion,128);
      //newMessage->setProperty(Property::ProductSection::Centre,98);
      //newMessage->setProperty(Property::ProductSection::GeneratingProcessIdentifier,149);
      //newMessage->setProperty(Property::ProductSection::GridDefinitionNumber,255);
      newMessage->setProperty(Property::ProductSection::SectionFlags,128);
      // newMessage->setProperty(Property::ProductSection::IndicatorOfParameter,49);
      newMessage->setProperty(Property::ProductSection::IndicatorOfTypeOfLevel,contentInfo1->mGrib1ParameterLevelId);
      newMessage->setProperty(Property::ProductSection::Level,paramLevel);
      newMessage->setProperty(Property::ProductSection::YearOfCentury,year % 100);
      newMessage->setProperty(Property::ProductSection::Month,month);
      newMessage->setProperty(Property::ProductSection::Day,day);
      newMessage->setProperty(Property::ProductSection::Hour,hour);
      newMessage->setProperty(Property::ProductSection::Minute,minute);
      newMessage->setProperty(Property::ProductSection::UnitOfTimeRange,1);
      newMessage->setProperty(Property::ProductSection::P1,0);
      newMessage->setProperty(Property::ProductSection::P2,0);
      newMessage->setProperty(Property::ProductSection::TimeRangeIndicator,0);
      //newMessage->setProperty(Property::ProductSection::NumberIncludedInAverage,0);
      //newMessage->setProperty(Property::ProductSection::NumberMissingFromAveragesOrAccumulations,0);
      newMessage->setProperty(Property::ProductSection::CenturyOfReferenceTimeOfData,(year/100)+1);
      //newMessage->setProperty(Property::ProductSection::SubCentre,0);
      //newMessage->setProperty(Property::ProductSection::DecimalScaleFactor,0);
      newMessage->setProperty(Property::ProductSection::ForecastType,forecastType);
      newMessage->setProperty(Property::ProductSection::ForecastNumber,forecastNumber);

      newMessage->setProperty(Property::DataSection::Flags,8);
      newMessage->setProperty(Property::DataSection::BinaryScaleFactor,-5);
      newMessage->setProperty(Property::DataSection::ReferenceValue,0.0);
      newMessage->setProperty(Property::DataSection::BitsPerValue,32);
      newMessage->setProperty(Property::DataSection::PackingMethod,0);


      // ### Trying to get parameter values accoding to parameter mapping definitions

      Identification::Grib1ParameterDef def;

      if (Identification::gridDef.getGrib1ParameterDefByFmiId(contentInfo1->mFmiParameterId,def))
      {
        newMessage->setProperty(Property::ProductSection::TableVersion,def.mTable2Version);
        newMessage->setProperty(Property::ProductSection::Centre,def.mCentre);
        //newMessage->setProperty(Property::ProductSection::GeneratingProcessIdentifier,def.mGeneratingProcessIdentifier);
        newMessage->setProperty(Property::ProductSection::IndicatorOfParameter,def.mIndicatorOfParameter);

        if (contentInfo1->mGrib1ParameterLevelId == 0)
        {
          Identification::FmiLevelId_grib def2;
          if (Identification::gridDef.getGrib1LevelDef(contentInfo1->mFmiParameterLevelId,0,def.mCentre,def2))
          {
            newMessage->setProperty(Property::ProductSection::IndicatorOfTypeOfLevel,def2.mGribLevelId);
          }
          else
          {
            if (Identification::gridDef.getGrib1LevelDef(contentInfo1->mFmiParameterLevelId,def2))
              newMessage->setProperty(Property::ProductSection::IndicatorOfTypeOfLevel,def2.mGribLevelId);
          }
        }
      }

      // ### Trying to get parameter values from the original grib

      T::Attribute *attr = attrList.getAttributeByNameEnd("product.tableVersion");
      if (attr != nullptr)
        newMessage->setProperty(Property::ProductSection::TableVersion,atoi(attr->mValue.c_str()));

      attr = attrList.getAttributeByNameEnd("centre.id");
      if (attr != nullptr)
        newMessage->setProperty(Property::ProductSection::Centre,atoi(attr->mValue.c_str()));

      attr = attrList.getAttributeByNameEnd("subCentre");
      if (attr != nullptr)
        newMessage->setProperty(Property::ProductSection::SubCentre,0);

      attr = attrList.getAttributeByNameEnd("generatingProcessIdentifier");
      if (attr != nullptr)
        newMessage->setProperty(Property::ProductSection::GeneratingProcessIdentifier,atoi(attr->mValue.c_str()));

      attr = attrList.getAttributeByNameEnd("indicatorOfParameter");
      if (attr != nullptr)
        newMessage->setProperty(Property::ProductSection::IndicatorOfParameter,atoi(attr->mValue.c_str()));

      attr = attrList.getAttributeByNameEnd("indicatorOfTypeOfLevel");
      if (attr != nullptr)
        newMessage->setProperty(Property::ProductSection::IndicatorOfTypeOfLevel,atoi(attr->mValue.c_str()));

      attr = attrList.getAttributeByNameEnd("BinaryScaleFactor");
      if (attr != nullptr)
        newMessage->setProperty(Property::DataSection::BinaryScaleFactor,atoi(attr->mValue.c_str()));

      //attr = attrList.getAttributeByNameEnd("DecimalScaleFactor");
      //if (attr != nullptr)
      //  newMessage->setProperty(Property::ProductSection::DecimalScaleFactor,atoi(attr->mValue.c_str()));

      attr = attrList.getAttributeByNameEnd("BitsPerValue");
      if (attr != nullptr)
        newMessage->setProperty(Property::DataSection::BitsPerValue,atoi(attr->mValue.c_str()));


      // ### Setting parameter values according to the attribute list

      uint len = queryAttributeList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::Attribute *attr = queryAttributeList.getAttributeByIndex(t);
        if (attr != nullptr &&  strncasecmp(attr->mName.c_str(),"Grib1.",6) == 0)
        {
          if (!newMessage->setProperty(attr->mName.c_str(),toInt64(attr->mValue)))
          {
            if (!newMessage->setProperty(attr->mName.c_str(),toDouble(attr->mValue)))
              PRINT_DATA(mDebugLog, "-- Attribute set failed [%s][%s]\n",attr->mName.c_str(),attr->mValue.c_str());
          }
        }
      }
    }
    else
    if (version2 != nullptr &&  strcmp(version2,"2") == 0)
    {
      PRINT_DATA(mDebugLog, "-- GRIB version 2\n");

      using namespace SmartMet::GRIB2;

      //newGridFile.setGridFile(T::FileTypeValue::Grib2);
      newMessage = newGridFile.newMessage(T::FileTypeValue::Grib2);

      // ### Setting default values for the parameters

      // ### INDICATOR SECTION ###

      newMessage->setProperty(Property::IndicatorSection::Discipline,0LL);


      // ### IDENTIFICATION SECTION ###

      //newMessage->setProperty(Property::IdentificationSection::Centre,78);
      //newMessage->setProperty(Property::IdentificationSection::SubCentre,255);
      //newMessage->setProperty(Property::IdentificationSection::TablesVersion,15);
      //newMessage->setProperty(Property::IdentificationSection::LocalTablesVersion,1);
      newMessage->setProperty(Property::IdentificationSection::SignificanceOfReferenceTime,1);
      newMessage->setProperty(Property::IdentificationSection::Year,year);
      newMessage->setProperty(Property::IdentificationSection::Month,month);
      newMessage->setProperty(Property::IdentificationSection::Day,day);
      newMessage->setProperty(Property::IdentificationSection::Hour,hour);
      newMessage->setProperty(Property::IdentificationSection::Minute,minute);
      newMessage->setProperty(Property::IdentificationSection::Second,second);
      newMessage->setProperty(Property::IdentificationSection::ProductionStatusOfProcessedData,0LL);
      newMessage->setProperty(Property::IdentificationSection::TypeOfProcessedData,1);


      // ### PRODUCT SECTION ###

      newMessage->setProperty(Property::ProductSection::ProductDefinitionTemplateNumber,ProductSection::Template::NormalProduct);
      newMessage->setProperty(Property::ProductSection::NV,0LL);

      //newMessage->setProperty(Property::ProductSection::ParameterSettings::ParameterCategory,2);
      //newMessage->setProperty(Property::ProductSection::ParameterSettings::ParameterNumber,0);
      //newMessage->setProperty(Property::ProductSection::ParameterSettings::TypeOfGeneratingProcess,2);
      //newMessage->setProperty(Property::ProductSection::ParameterSettings::BackgroundProcess,0LL);
      //newMessage->setProperty(Property::ProductSection::ParameterSettings::GeneratingProcessIdentifier,181);
      newMessage->setProperty(Property::ProductSection::ParameterSettings::HoursAfterDataCutoff,0LL);
      newMessage->setProperty(Property::ProductSection::ParameterSettings::MinutesAfterDataCutoff,0LL);
      newMessage->setProperty(Property::ProductSection::ParameterSettings::IndicatorOfUnitOfTimeRange,0LL);
      newMessage->setProperty(Property::ProductSection::ParameterSettings::ForecastTime,0LL);

      newMessage->setProperty(Property::ProductSection::HorizontalSettings::TypeOfFirstFixedSurface,contentInfo1->mGrib2ParameterLevelId);
      newMessage->setProperty(Property::ProductSection::HorizontalSettings::ScaleFactorOfFirstFixedSurface,0LL);
      newMessage->setProperty(Property::ProductSection::HorizontalSettings::ScaledValueOfFirstFixedSurface,paramLevel);

      // ### REPRESENTATION SECTION ###

      newMessage->setProperty(Property::RepresentationSection::RepresentationTemplateNumber,RepresentationSection::Template::GridDataRepresentation);
      newMessage->setProperty(Property::RepresentationSection::Packing::BinaryScaleFactor,-5);
      newMessage->setProperty(Property::RepresentationSection::Packing::DecimalScaleFactor,0LL);
      newMessage->setProperty(Property::RepresentationSection::Packing::BitsPerValue,32);

      newMessage->setProperty(Property::RepresentationSection::OriginalValues::TypeOfOriginalFieldValues,0LL);


      // ### Trying to get parameter values accoding to parameter mapping definitions

      Identification::Grib2ParameterDef def;

      if (Identification::gridDef.getGrib2ParameterDefByFmiId(contentInfo1->mFmiParameterId,def))
      {
        newMessage->setProperty(Property::IndicatorSection::Discipline,def.mDiscipline);
        newMessage->setProperty(Property::IdentificationSection::Centre,*(def.mCentre));
        //newMessage->setProperty(Property::IdentificationSection::SubCentre,255);
        //newMessage->setProperty(Property::IdentificationSection::TablesVersion,def.mGribTableVersion);


        newMessage->setProperty(Property::ProductSection::ParameterSettings::ParameterCategory,def.mParameterCategory);
        newMessage->setProperty(Property::ProductSection::ParameterSettings::ParameterNumber,def.mParameterNumber);
        //newMessage->setProperty(Property::ProductSection::ParameterSettings::TypeOfGeneratingProcess,2);
        //newMessage->setProperty(Property::ProductSection::ParameterSettings::BackgroundProcess,0LL);
        //newMessage->setProperty(Property::ProductSection::ParameterSettings::GeneratingProcessIdentifier,def.mGeneratingProcessIdentifier);

        if (contentInfo1->mGrib2ParameterLevelId == 0)
        {
          Identification::FmiLevelId_grib def2;
          if (Identification::gridDef.getGrib2LevelDef(contentInfo1->mFmiParameterLevelId,def2))
          {
            newMessage->setProperty(Property::ProductSection::HorizontalSettings::TypeOfFirstFixedSurface,def2.mGribLevelId);
          }
        }
      }


      // ### Trying to get parameter values from the original grib

      T::Attribute *attr = attrList.getAttributeByNameEnd("identification.tablesVersion");
      if (attr != nullptr)
        newMessage->setProperty(Property::IdentificationSection::TablesVersion,atoi(attr->mValue.c_str()));

      attr = attrList.getAttributeByNameEnd("centre.id");
      if (attr != nullptr)
        newMessage->setProperty(Property::IdentificationSection::Centre,atoi(attr->mValue.c_str()));

      attr = attrList.getAttributeByNameEnd("subCentre");
      if (attr != nullptr)
        newMessage->setProperty(Property::IdentificationSection::SubCentre,0);

      attr = attrList.getAttributeByNameEnd("GeneratingProcessIdentifier");
      if (attr != nullptr)
        newMessage->setProperty(Property::ProductSection::ParameterSettings::GeneratingProcessIdentifier,atoi(attr->mValue.c_str()));

      attr = attrList.getAttributeByNameEnd("ParameterCategory");
      if (attr != nullptr)
        newMessage->setProperty(Property::ProductSection::ParameterSettings::ParameterCategory,atoi(attr->mValue.c_str()));

      attr = attrList.getAttributeByNameEnd("ParameterNumber");
      if (attr != nullptr)
        newMessage->setProperty(Property::ProductSection::ParameterSettings::ParameterNumber,atoi(attr->mValue.c_str()));

      attr = attrList.getAttributeByNameEnd("TypeOfFirstFixedSurface");
      if (attr != nullptr)
        newMessage->setProperty(Property::ProductSection::HorizontalSettings::TypeOfFirstFixedSurface,atoi(attr->mValue.c_str()));

      attr = attrList.getAttributeByNameEnd("BinaryScaleFactor");
      if (attr != nullptr)
        newMessage->setProperty(Property::RepresentationSection::Packing::BinaryScaleFactor,atoi(attr->mValue.c_str()));

      //attr = attrList.getAttributeByNameEnd("DecimalScaleFactor");
      //if (attr != nullptr)
      //  newMessage->setProperty(Property::RepresentationSection::Packing::DecimalScaleFactor,atoi(attr->mValue.c_str()));

      attr = attrList.getAttributeByNameEnd("BitsPerValue");
      if (attr != nullptr)
        newMessage->setProperty(Property::RepresentationSection::Packing::BitsPerValue,atoi(attr->mValue.c_str()));


      // ### Setting parameter values according to the attribute list

      uint len = queryAttributeList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::Attribute *attr = queryAttributeList.getAttributeByIndex(t);
        if (attr != nullptr &&  strncasecmp(attr->mName.c_str(),"Grib2.",6) == 0)
        {
          if (!newMessage->setProperty(attr->mName.c_str(),toInt64(attr->mValue)))
          {
            if (!newMessage->setProperty(attr->mName.c_str(),toDouble(attr->mValue)))
              PRINT_DATA(mDebugLog, "-- Attribute set failed [%s][%s]\n",attr->mName.c_str(),attr->mValue.c_str());
          }
        }
      }

    }
    else
    {
      Fmi::Exception exception(BCP, "No valid grib type and version found!");
      std::string errorMsg = exception.getStackTrace();
      PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
      return false;
    }

    newMessage->initSpatialReference();

    T::Coordinate_svec coordinates = newMessage->getGridLatLonCoordinates();
    if (coordinates->size() == 0)
    {
      PRINT_DATA(mDebugLog, "-- No coordinates available\n");
      return false;
    }

    T::Dimensions d = newMessage->getGridDimensions();

    uint width = d.nx();
    uint height = d.ny();
    uint numOfValues = width*height;
    uint numOfBytes = 4*numOfValues;
    ulonglong sz = 2 * numOfBytes;


    queryAttributeList.setAttribute("grid.timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
    queryAttributeList.setAttribute("grid.areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));
    queryAttributeList.setAttribute("grid.levelInterpolationMethod",Fmi::to_string(levelInterpolationMethod));

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC == forecastTime /*|| timeInterpolationMethod == T::TimeInterpolationMethod::None || timeInterpolationMethod == T::TimeInterpolationMethod::Nearest*/)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        int result = mDataServerPtr->getGridValueVectorByCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,coordinateType,*coordinates,areaInterpolationMethod,modificationOperation,modificationParameters,valueVector);

        if (result != 0)
        {
          Fmi::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueVectorByCoordinateList");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        if (conversionByFunction)
        {
           executeConversion(function, functionParams, forecastTime, *coordinates, valueVector, newValueVector);
           newMessage->setGridValues(newValueVector);
        }
        else
        {
          newMessage->setGridValues(valueVector);
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        uchar *data = new uchar[sz];
        MemoryWriter memoryWriter(data,sz,true);
        newGridFile.write(memoryWriter);
        ulonglong fsize = memoryWriter.getMaxWritePosition();

        std::vector<uchar> dataFile;
        dataFile.reserve(fsize);
        for (ulonglong i=0; i <fsize; i++)
          dataFile.emplace_back(data[i]);

        valueList.mValueData.emplace_back(dataFile);

        //newGridFile.write("/tmp/test.grib");

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
      if (contentInfo1->mForecastTimeUTC < forecastTime && contentInfo2->mForecastTimeUTC > forecastTime  &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden)
      {
        // We did not find a grid with the exact forecast time, but we find grids that
        // are before and after the current forecast time. This means that we should do
        // some time interpolation.

        int result = mDataServerPtr->getGridValueVectorByTimeAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,forecastTime,coordinateType,*coordinates,queryAttributeList,modificationOperation,modificationParameters,valueVector);

        if (result != 0)
        {
          Fmi::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueVectorByTimeAndCoordinateList");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        if (conversionByFunction)
        {
           executeConversion(function, functionParams, forecastTime, *coordinates, valueVector, newValueVector);
           newMessage->setGridValues(newValueVector);
        }
        else
        {
          newMessage->setGridValues(valueVector);
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        uchar *data = new uchar[sz];
        MemoryWriter memoryWriter(data,sz,true);
        newGridFile.write(memoryWriter);
        ulonglong fsize = memoryWriter.getMaxWritePosition();

        std::vector<uchar> dataFile;
        dataFile.reserve(fsize);
        for (ulonglong i=0; i <fsize; i++)
          dataFile.emplace_back(data[i]);

        valueList.mValueData.emplace_back(dataFile);

        //newGridFile.write("/tmp/test.grib");

        return true;
      }

      if (contentInfo1->mForecastTimeUTC == forecastTime  &&  contentInfo2->mForecastTimeUTC == forecastTime && contentInfo1->mParameterLevel < paramLevel && contentInfo2->mParameterLevel > paramLevel  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
      {
        // We did not find a grid with the exact level but we find grids that
        // are before and after the current level. This means that we should do
        // some level interpolation.

        int result = mDataServerPtr->getGridValueVectorByLevelAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,coordinateType,*coordinates,queryAttributeList,modificationOperation,modificationParameters,valueVector);
        if (result != 0)
        {
          Fmi::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueVectorByLevelAndCoordinateList");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        valueList.mParameterLevel = paramLevel;

        if (conversionByFunction)
        {
           executeConversion(function, functionParams, forecastTime, *coordinates, valueVector, newValueVector);
           newMessage->setGridValues(newValueVector);
        }
        else
        {
          newMessage->setGridValues(valueVector);
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        uchar *data = new uchar[sz];
        MemoryWriter memoryWriter(data,sz,true);
        newGridFile.write(memoryWriter);
        ulonglong fsize = memoryWriter.getMaxWritePosition();

        std::vector<uchar> dataFile;
        dataFile.reserve(fsize);
        for (ulonglong i=0; i <fsize; i++)
          dataFile.emplace_back(data[i]);

        valueList.mValueData.emplace_back(dataFile);

        //newGridFile.write("/tmp/test.grib");

        return true;
      }
    }

    if (contentLen == 4)
    {
      if (contentInfo1->mForecastTimeUTC < forecastTime  &&  contentInfo2->mForecastTimeUTC < forecastTime  &&  contentInfo3->mForecastTimeUTC > forecastTime  &&  contentInfo4->mForecastTimeUTC > forecastTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&  contentInfo3->mParameterLevel < paramLevel  &&  contentInfo4->mParameterLevel > paramLevel
          &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
      {
        int result = mDataServerPtr->getGridValueVectorByTimeLevelAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,forecastTime,paramLevel,coordinateType,*coordinates,queryAttributeList,modificationOperation,modificationParameters,valueVector);
        if (result != 0)
        {
          Fmi::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueVectorByTimeLevelAndCoordinateList");
          exception.addParameter("Message", DataServer::getResultString(result));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        valueList.mParameterLevel = paramLevel;

        if (conversionByFunction)
        {
           executeConversion(function, functionParams, forecastTime, *coordinates, valueVector, newValueVector);
           newMessage->setGridValues(newValueVector);
        }
        else
        {
          newMessage->setGridValues(valueVector);
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        uchar *data = new uchar[sz];
        MemoryWriter memoryWriter(data,sz,true);
        newGridFile.write(memoryWriter);
        ulonglong fsize = memoryWriter.getMaxWritePosition();

        std::vector<uchar> dataFile;
        dataFile.reserve(fsize);
        for (ulonglong i=0; i <fsize; i++)
          dataFile.emplace_back(data[i]);

        valueList.mValueData.emplace_back(dataFile);

        //newGridFile.write("/tmp/test.grib");

        return true;
      }
    }
#endif
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getPointValuesByHeight(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    T::ForecastType forecastType,
    T::ForecastNumber forecastNumber,
    uint parameterFlags,
    short areaInterpolationMethod,
    short timeInterpolationMethod,
    short levelInterpolationMethod,
    uchar coordinateType,
    T::AreaCoordinates& areaCoordinates,
    uint& newProducerId,
    short& precision,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getPointValuesByHeight()\n");
    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction = conversionFunction(pInfo.mConversionFunction, function, functionParams);

    if (conversionByFunction && mDataServerMethodsEnabled)
    {
      std::string f = toUpperString(function);
      auto p = mOperationNames.find(f);
      if (p != mOperationNames.end())
      {
        modificationOperation = p->second;
        for (auto it = functionParams.begin(); it != functionParams.end(); ++it)
        {
          if (*it != "$")
            modificationParameters.push_back(atof(it->c_str()));
        }
        conversionByFunction = false;
      }
    }

    time_t fTime = forecastTime;
    time_t startTime = forecastTime -4*3600;
    time_t endTime = forecastTime + 4*3600;

    T::ContentInfoList tmpContentList;

    int result = mContentServerPtr->getContentListByParameterAndGenerationId(0,generationId,pInfo.mParameterKeyType, pInfo.mParameterKey,-1,-1000000000,1000000000,forecastType,forecastNumber,producerGeometryId,startTime,endTime,0,tmpContentList);
    if (result != 0)
    {
      Fmi::Exception exception(BCP, "ContentServer returns an error!");
      exception.addParameter("Service", "getContentListByParameterGenerationId");
      exception.addParameter("Message", ContentServer::getResultString(result));
      throw exception;
    }

    uint tmpContentLen = tmpContentList.getLength();
    if (tmpContentLen == 0)
      return false;

    PRINT_DATA(mDebugLog, "         + Found %u tmp content records\n", tmpContentList.getLength());

    tmpContentList.sort(T::ContentInfo::ComparisonMethod::starttime_fmiName_fmiLevelId_level_file_message);

    std::set<time_t> forecastTimeList;

    tmpContentList.getForecastTimeList(forecastTimeList);

    T::ContentInfoList tmpContentList2;
    time_t prev = 0;
    time_t next = 0;

    bool timeMatch = false;
    if (forecastTimeList.find(forecastTime) != forecastTimeList.end())
    {
      tmpContentList.getContentInfoListByForecastTime(forecastTime,tmpContentList2);
      PRINT_DATA(mDebugLog, "         + Time match\n");
      timeMatch = true;
    }
    else
    {
      for (auto it = forecastTimeList.begin(); it != forecastTimeList.end(); ++it)
      {
        if (*it < forecastTime)
          prev = *it;
        if (*it > forecastTime  &&  (next == 0 ||  *it < next))
          next = *it;
      }

      PRINT_DATA(mDebugLog, "         + Times : %s  %s\n",utcTimeFromTimeT(prev).c_str(),utcTimeFromTimeT(next).c_str());
      tmpContentList.getContentInfoListByForecastTime(prev,tmpContentList2);

      T::ContentInfoList tmpContentList4;
      tmpContentList.getContentInfoListByForecastTime(next,tmpContentList4);
      tmpContentList2.addContentInfoList(tmpContentList4);
    }

    PRINT_DATA(mDebugLog, "         + Found %u tmp2 content records\n", tmpContentList2.getLength());
    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      tmpContentList2.print(stream, 0, 4);
      PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
    }

    for (auto coordinateList = areaCoordinates.begin(); coordinateList != areaCoordinates.end(); ++coordinateList)
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
          if (cInfo->mForecastTimeUTC == prev)
          {
            if (cInfo->mParameterLevel <= paramLevel)
              prevContent = cInfo;

            if (cInfo->mParameterLevel >= paramLevel  &&  nextContent == nullptr )
              nextContent = cInfo;
          }
        }

        if (prevContent != nullptr)
          contentList.addContentInfo(prevContent->duplicate());

        if (nextContent != nullptr)
          contentList.addContentInfo(nextContent->duplicate());

        if (!timeMatch)
        {
          prevContent = nullptr;
          nextContent = nullptr;

          for (uint t=0; t<cLen; t++)
          {
            T::ContentInfo *cInfo = tmpContentList3.getContentInfoByIndex(t);
            if (cInfo->mForecastTimeUTC == next)
            {
              if (cInfo->mParameterLevel <= paramLevel)
                prevContent = cInfo;

              if (cInfo->mParameterLevel >= paramLevel  &&  nextContent == nullptr)
                nextContent = cInfo;
            }
          }

          if (prevContent != nullptr)
            contentList.addContentInfo(prevContent->duplicate());

          if (nextContent != nullptr)
            contentList.addContentInfo(nextContent->duplicate());
        }

        PRINT_DATA(mDebugLog, "         + Found %u content records\n", contentList.getLength());
        if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
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
          if ((parameterFlags & QueryServer::QueryParameter::Flags::HeightLevelInterpolation) != 0)
            iplMethod = T::LevelInterpolationMethod::Linear;
          else
            iplMethod = T::LevelInterpolationMethod::Logarithmic;
        }
        PRINT_DATA(mDebugLog, "         + Level interpolation method = %d\n",iplMethod);


        T::ContentInfo* contentInfo1 = contentList.getContentInfoByIndex(0);
        T::ContentInfo* contentInfo2 = contentList.getContentInfoByIndex(1);
        T::ContentInfo* contentInfo3 = contentList.getContentInfoByIndex(2);
        T::ContentInfo* contentInfo4 = contentList.getContentInfoByIndex(3);

        valueList.mParameterKeyType = pInfo.mParameterKeyType;
        valueList.mParameterKey = pInfo.mParameterKey;
        valueList.mParameterLevelId = paramLevelId;
        valueList.mForecastTimeUTC = forecastTime;
        valueList.mProducerId = contentInfo1->mProducerId;
        valueList.mGenerationId = contentInfo1->mGenerationId;
        valueList.mGenerationFlags = generationFlags;
        valueList.mGeometryId = contentInfo1->mGeometryId;
        valueList.mFileId[0] = contentInfo1->mFileId;
        valueList.mMessageIndex[0] = contentInfo1->mMessageIndex;
        valueList.mModificationTime = contentInfo1->mModificationTime;
        valueList.mAnalysisTime = analysisTime;
        valueList.mForecastType = contentInfo1->mForecastType;
        valueList.mForecastNumber = contentInfo1->mForecastNumber;
        valueList.mParameterLevel = contentInfo1->mParameterLevel;

        valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;

        if (contentLen == 1)
        {
          if (contentInfo1->mForecastTimeUTC == fTime /*|| timeInterpolationMethod == T::TimeInterpolationMethod::None || timeInterpolationMethod == T::TimeInterpolationMethod::Nearest*/)
          {
            // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

            valueList.mFlags = ParameterValues::Flags::DataAvailable;

            if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
            {
              T::ParamValue value = 0;
              int result = mDataServerPtr->getGridValueByPoint(0, contentInfo1->mFileId, contentInfo1->mMessageIndex, coordinateType,coordinate->x(),coordinate->y(),areaInterpolationMethod,modificationOperation,modificationParameters,value);
              if (result != 0)
              {
                Fmi::Exception exception(BCP, "DataServer returns an error!");
                exception.addParameter("Service", "getGridValueListByPointList");
                exception.addParameter("Message", DataServer::getResultString(result));
                std::string errorMsg = exception.getStackTrace();
                PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
                value = ParamValueMissing;
              }

              T::GridValue rec(coordinate->x(),coordinate->y(),value);
              valueList.mValueList.addGridValue(rec);
            }

            valueList.mParameterLevel = paramLevel;
          }
          else
          {
            // There is one content record in place, but its time does not match to
            // the requested forecast time. This is used for indicating that there
            // are content records available, but not for the requested time.
            // So, we should use this producer.

            newProducerId = producerInfo.mProducerId;
            valueList.clear();
            valueList.mProducerId = producerInfo.mProducerId;
            valueList.mParameterKeyType = pInfo.mParameterKeyType;
            valueList.mParameterKey = pInfo.mParameterKey;
            valueList.mForecastTimeUTC = forecastTime;
          }
        }


        if (contentLen == 2)
        {
          if (contentInfo1->mForecastTimeUTC == fTime  &&  contentInfo2->mForecastTimeUTC == fTime  &&
              contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&
              levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
          {
            // Content records have the same time but different levels, so we need to level interpolation.

            short iplMethod = levelInterpolationMethod;
            if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
            {
              if ((parameterFlags & QueryServer::QueryParameter::Flags::HeightLevelInterpolation) != 0)
                iplMethod = T::LevelInterpolationMethod::Linear;
              else
                iplMethod = T::LevelInterpolationMethod::Logarithmic;
            }

            valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByLevelInterpolation;
            valueList.mFileId[1] = contentInfo2->mFileId;
            valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

            if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
            {
              T::ParamValue value = 0;
              int result = mDataServerPtr->getGridValueByLevelAndPoint(0,
                  contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo1->mParameterLevel,
                  contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo2->mParameterLevel,
                  paramLevel,coordinateType,coordinate->x(),coordinate->y(),areaInterpolationMethod,iplMethod,modificationOperation,modificationParameters,value);

              if (result != 0)
              {
                Fmi::Exception exception(BCP, "DataServer returns an error!");
                exception.addParameter("Service", "getGridValueListByLevelAndPointList");
                exception.addParameter("Message", DataServer::getResultString(result));
                std::string errorMsg = exception.getStackTrace();
                PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
                value = ParamValueMissing;
              }

              T::GridValue rec(coordinate->x(),coordinate->y(),value);
              valueList.mValueList.addGridValue(rec);
            }

            valueList.mParameterLevel = paramLevel;
          }
        }


        if (contentLen == 4)
        {
          if (contentInfo1->mForecastTimeUTC < fTime  &&  contentInfo2->mForecastTimeUTC < fTime  &&  contentInfo3->mForecastTimeUTC > fTime  &&  contentInfo4->mForecastTimeUTC > fTime  &&
              contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&  contentInfo3->mParameterLevel < paramLevel  &&  contentInfo4->mParameterLevel > paramLevel
              &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
          {
            valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation | ParameterValues::Flags::DataAvailableByLevelInterpolation;
            valueList.mFileId[1] = contentInfo2->mFileId;
            valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
            valueList.mFileId[2] = contentInfo3->mFileId;
            valueList.mMessageIndex[2] = contentInfo3->mMessageIndex;
            valueList.mFileId[3] = contentInfo4->mFileId;
            valueList.mMessageIndex[3] = contentInfo4->mMessageIndex;

            if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
            {
              T::ParamValue value = 0;
              int result = mDataServerPtr->getGridValueByTimeLevelAndPoint(0,
                  contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo1->mParameterLevel,
                  contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo2->mParameterLevel,
                  contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo3->mParameterLevel,
                  contentInfo4->mFileId, contentInfo4->mMessageIndex,contentInfo4->mParameterLevel,
                  fTime,paramLevel,coordinateType,coordinate->x(),coordinate->y(),
                  areaInterpolationMethod,timeInterpolationMethod,iplMethod,modificationOperation,modificationParameters,value);

              if (result != 0)
              {
                Fmi::Exception exception(BCP, "DataServer returns an error!");
                exception.addParameter("Service", "getGridValueListByLevelAndPointList");
                exception.addParameter("Message", DataServer::getResultString(result));
                std::string errorMsg = exception.getStackTrace();
                PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
                value = ParamValueMissing;
              }

              T::GridValue rec(coordinate->x(),coordinate->y(),value);
              valueList.mValueList.addGridValue(rec);
            }
            valueList.mParameterLevel = paramLevel;
          }
        }
      }
    }


    if (valueList.mValueList.getLength() > 0)
    {
      if (conversionByFunction)
        executeConversion(function, functionParams, forecastTime, valueList.mValueList);

      if (precision < 0)
        precision = pInfo.mDefaultPrecision;

      return true;
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





time_t ServiceImplementation::getContentList(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    T::ForecastType forecastType,
    T::ForecastNumber forecastNumber,
    uint parameterFlags,
    std::shared_ptr<T::ContentInfoList>& contentList)
{
  FUNCTION_TRACE
  try
  {
    time_t fTime = forecastTime;

    if ((pInfo.mGroupFlags & (QueryServer::ParameterMapping::GroupFlags::global |
                              QueryServer::ParameterMapping::GroupFlags::climatology |
                              QueryServer::ParameterMapping::GroupFlags::bias)) != 0)
    {
      T::ContentInfoList contentList2;
      int result = mContentServerPtr->getContentListByParameterAndGenerationId(0, generationId, pInfo.mParameterKeyType, pInfo.mParameterKey, -1,
          0, 0, forecastType, forecastNumber, producerGeometryId, "19000101T000000", "30000101T000000", 0, contentList2);

      if (result != 0)
      {
        Fmi::Exception exception(BCP, "ContentServer returns an error!");
        exception.addParameter("Service", "getContentListByParameterAndGenerationId");
        exception.addParameter("Message", ContentServer::getResultString(result));
        throw exception;
      }

      if (contentList2.getLength() == 0)
        return 0;

      T::ContentInfo* cInfo = contentList2.getContentInfoByIndex(0);

      if ((pInfo.mGroupFlags & QueryServer::ParameterMapping::GroupFlags::climatology) != 0)
      {
        std::string ft = cInfo->getForecastTime();
        std::string tmp1 = utcTimeFromTimeT(forecastTime);
        std::string tmp2 = ft.substr(0, 4) + tmp1.substr(4);
        fTime = utcTimeToTimeT(tmp2);
      }
      else
      if ((pInfo.mGroupFlags & QueryServer::ParameterMapping::GroupFlags::global) != 0)
      {
        T::ContentInfo *cInfo2 = cInfo->duplicate();
        cInfo2->setForecastTime(forecastTime);
        contentList->addContentInfo(cInfo2);
      }
      else
      if ((pInfo.mGroupFlags & QueryServer::ParameterMapping::GroupFlags::bias) != 0)
      {
        std::string ft = cInfo->getForecastTime();
        std::string tmp1 = utcTimeFromTimeT(forecastTime);
        std::string tmp2 = tmp1.substr(0,6) + ft.substr(6);
        fTime = utcTimeToTimeT(tmp2);
      }

    }

    if (contentList->getLength() == 0)
    {
      int result = getContentListByParameterGenerationIdAndForecastTime(0, producerInfo.mProducerId, producerInfo.mHash, generationId, pInfo.mParameterKeyType, pInfo.mParameterKey, pInfo.getKeyHash(),
          paramLevelId, paramLevel, forecastType, forecastNumber, producerGeometryId, fTime, contentList);

      if (result != 0)
      {
        Fmi::Exception exception(BCP, "ContentServer returns an error!");
        exception.addParameter("Service", "getContentListByParameterGenerationIdAndForecastTime");
        exception.addParameter("Message", ContentServer::getResultString(result));
        throw exception;
      }
    }
    return fTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




bool ServiceImplementation::getPointValues(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    T::ForecastType forecastType,
    T::ForecastNumber forecastNumber,
    uint parameterFlags,
    short areaInterpolationMethod,
    short timeInterpolationMethod,
    short levelInterpolationMethod,
    uchar coordinateType,
    T::AreaCoordinates& areaCoordinates,
    uint& newProducerId,
    short& precision,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getPointValues()\n");

    if ((parameterFlags & QueryServer::QueryParameter::Flags::HeightLevelInterpolation) != 0  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
    {
      return getPointValuesByHeight(producerInfo,producerGeometryId,generationId,analysisTime,generationFlags,pInfo,forecastTime,
          paramLevelId,paramLevel,forecastType,forecastNumber,parameterFlags,areaInterpolationMethod,timeInterpolationMethod,
          levelInterpolationMethod,coordinateType,areaCoordinates,newProducerId,precision,valueList);
    }

    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction = conversionFunction(pInfo.mConversionFunction, function, functionParams);

    if (conversionByFunction && mDataServerMethodsEnabled)
    {
      std::string f = toUpperString(function);
      auto p = mOperationNames.find(f);
      if (p != mOperationNames.end())
      {
        modificationOperation = p->second;
        for (auto it = functionParams.begin(); it != functionParams.end(); ++it)
        {
          if (*it != "$")
            modificationParameters.push_back(atof(it->c_str()));
        }
        conversionByFunction = false;
      }
    }

    std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
    time_t fTime = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,
        paramLevelId,paramLevel,forecastType,forecastNumber,parameterFlags,contentList);

    PRINT_DATA(mDebugLog, "         + Found %u content records (fTime = %ld)\n", contentList->getLength(),fTime);

    if (fTime == 0)
      return false;

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      contentList->print(stream, 0, 4);
      PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
    }

    uint contentLen = contentList->getLength();
    if (contentLen == 0)
      return false;

    short iplMethod = levelInterpolationMethod;
    if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
    {
      if ((parameterFlags & QueryServer::QueryParameter::Flags::HeightLevelInterpolation) != 0)
        iplMethod = T::LevelInterpolationMethod::Linear;
      else
        iplMethod = T::LevelInterpolationMethod::Logarithmic;
    }

    T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList->getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList->getContentInfoByIndex(3);

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelId = paramLevelId;
    valueList.mForecastTimeUTC = forecastTime;
    valueList.mProducerId = contentInfo1->mProducerId;
    valueList.mGenerationId = contentInfo1->mGenerationId;
    valueList.mGenerationFlags = generationFlags;
    valueList.mGeometryId = contentInfo1->mGeometryId;
    valueList.mFileId[0] = contentInfo1->mFileId;
    valueList.mMessageIndex[0] = contentInfo1->mMessageIndex;
    valueList.mModificationTime = contentInfo1->mModificationTime;
    valueList.mAnalysisTime = analysisTime;
    valueList.mForecastType = contentInfo1->mForecastType;
    valueList.mForecastNumber = contentInfo1->mForecastNumber;
    valueList.mParameterLevel = contentInfo1->mParameterLevel;
    valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC == fTime)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        valueList.mFlags = ParameterValues::Flags::DataAvailable;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByPointList(0, contentInfo1->mFileId, contentInfo1->mMessageIndex, coordinateType,areaCoordinates[0],areaInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByPointList");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
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
        valueList.clear();
        valueList.mProducerId = producerInfo.mProducerId;
        valueList.mParameterKeyType = pInfo.mParameterKeyType;
        valueList.mParameterKey = pInfo.mParameterKey;
        valueList.mForecastTimeUTC = forecastTime;
        return false;
      }
    }


    if (contentLen == 2)
    {
      if (contentInfo1->mForecastTimeUTC == fTime  &&  contentInfo2->mForecastTimeUTC == fTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&
          levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
      {
        // Content records have the same time but different levels, so we need to level interpolation.

        short iplMethod = levelInterpolationMethod;
        if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
        {
          if ((parameterFlags & QueryServer::QueryParameter::Flags::HeightLevelInterpolation) != 0)
            iplMethod = T::LevelInterpolationMethod::Linear;
          else
            iplMethod = T::LevelInterpolationMethod::Logarithmic;
        }

        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByLevelInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mParameterLevel = paramLevel;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByLevelAndPointList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,coordinateType,areaCoordinates[0],areaInterpolationMethod,iplMethod,modificationOperation,modificationParameters,valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByLevelAndPointList");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }


      if (contentInfo1->mForecastTimeUTC < fTime  &&  contentInfo2->mForecastTimeUTC > fTime  &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden)
      {
        // Content records have different times, but most likely the same levels, so we need to do time interpolation.

        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByTimeAndPointList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,coordinateType,areaCoordinates[0],areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByTimeAndPointList");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }
    }


    if (contentLen == 4)
    {
      if (contentInfo1->mForecastTimeUTC < fTime  &&  contentInfo2->mForecastTimeUTC < fTime  &&  contentInfo3->mForecastTimeUTC > fTime  &&  contentInfo4->mForecastTimeUTC > fTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&  contentInfo3->mParameterLevel < paramLevel  &&  contentInfo4->mParameterLevel > paramLevel
          &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
      {
        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation | ParameterValues::Flags::DataAvailableByLevelInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
        valueList.mFileId[2] = contentInfo3->mFileId;
        valueList.mMessageIndex[2] = contentInfo3->mMessageIndex;
        valueList.mFileId[3] = contentInfo4->mFileId;
        valueList.mMessageIndex[3] = contentInfo4->mMessageIndex;
        valueList.mParameterLevel = paramLevel;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByTimeLevelAndPointList(0,
              contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,
              contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,
              fTime,paramLevel,coordinateType,areaCoordinates[0],areaInterpolationMethod,timeInterpolationMethod,iplMethod,modificationOperation,modificationParameters,valueList.mValueList);

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByLevelAndPointList");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getCircleValues(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
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
    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction = conversionFunction(pInfo.mConversionFunction, function, functionParams);

    if (conversionByFunction && mDataServerMethodsEnabled)
    {
      std::string f = toUpperString(function);
      auto p = mOperationNames.find(f);
      if (p != mOperationNames.end())
      {
        modificationOperation = p->second;
        for (auto it = functionParams.begin(); it != functionParams.end(); ++it)
        {
          if (*it != "$")
            modificationParameters.push_back(atof(it->c_str()));
        }
        conversionByFunction = false;
      }
    }

    std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
    time_t fTime = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,
        paramLevelId,paramLevel,forecastType,forecastNumber,parameterFlags,contentList);

    PRINT_DATA(mDebugLog, "         + Found %u content records\n", contentList->getLength());

    if (fTime == 0)
      return false;

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      contentList->print(stream, 0, 4);
      PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
    }

    uint contentLen = contentList->getLength();

    if (contentLen == 0)
      return false;

    short iplMethod = levelInterpolationMethod;
    if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
    {
      if ((parameterFlags & QueryServer::QueryParameter::Flags::HeightLevelInterpolation) != 0)
        iplMethod = T::LevelInterpolationMethod::Linear;
      else
        iplMethod = T::LevelInterpolationMethod::Logarithmic;
    }

    T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList->getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList->getContentInfoByIndex(3);

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelId = pInfo.mParameterLevelId;
    valueList.mForecastTimeUTC = forecastTime;
    valueList.mProducerId = contentInfo1->mProducerId;
    valueList.mGenerationId = contentInfo1->mGenerationId;
    valueList.mGenerationFlags = generationFlags;
    valueList.mGeometryId = contentInfo1->mGeometryId;
    valueList.mFileId[0] = contentInfo1->mFileId;
    valueList.mMessageIndex[0] = contentInfo1->mMessageIndex;
    valueList.mModificationTime = contentInfo1->mModificationTime;
    valueList.mAnalysisTime = analysisTime;
    valueList.mForecastType = contentInfo1->mForecastType;
    valueList.mForecastNumber = contentInfo1->mForecastNumber;
    valueList.mParameterLevel = contentInfo1->mParameterLevel;
    valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC == fTime /*|| timeInterpolationMethod == T::TimeInterpolationMethod::None || timeInterpolationMethod == T::TimeInterpolationMethod::Nearest*/)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        valueList.mFlags = ParameterValues::Flags::DataAvailable;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByCircle(0, contentInfo1->mFileId, contentInfo1->mMessageIndex, coordinateType, x, y, radius,modificationOperation,modificationParameters, valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByCircle");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
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
        valueList.clear();
        valueList.mProducerId = producerInfo.mProducerId;
        valueList.mParameterKeyType = pInfo.mParameterKeyType;
        valueList.mParameterKey = pInfo.mParameterKey;
        valueList.mForecastTimeUTC = forecastTime;
        return false;
      }
    }

    if (contentLen == 2)
    {
      if (contentInfo1->mForecastTimeUTC < fTime && contentInfo2->mForecastTimeUTC > fTime  &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden)
      {
        // We did not find a grid with the exact forecast time, but we find grids that
        // are before and after the current forecast time. This means that we should do
        // some time interpolation.

        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          // Fetching data from the grids.
          int result = mDataServerPtr->getGridValueListByTimeAndCircle(0,contentInfo1->mFileId,contentInfo1->mMessageIndex,contentInfo2->mFileId,contentInfo2->mMessageIndex,fTime,coordinateType,x,y,radius,timeInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByTimeAndCircle");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }

      if (contentInfo1->mForecastTimeUTC == fTime  &&  contentInfo2->mForecastTimeUTC == fTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel
          &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
      {
        // Content records have the same time but different levels, so we need to level interpolation.
        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByLevelInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
        valueList.mParameterLevel = paramLevel;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByLevelAndCircle(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,coordinateType,x,y,radius,iplMethod,modificationOperation,modificationParameters,valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByLevelAndCircle");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }
    }


    if (contentLen == 4)
    {
      if (contentInfo1->mForecastTimeUTC < fTime  &&  contentInfo2->mForecastTimeUTC < fTime  &&  contentInfo3->mForecastTimeUTC > fTime  &&  contentInfo4->mForecastTimeUTC > fTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&  contentInfo3->mParameterLevel < paramLevel  &&  contentInfo4->mParameterLevel > paramLevel
          &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
      {
        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation | ParameterValues::Flags::DataAvailableByLevelInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
        valueList.mFileId[2] = contentInfo3->mFileId;
        valueList.mMessageIndex[2] = contentInfo3->mMessageIndex;
        valueList.mFileId[3] = contentInfo4->mFileId;
        valueList.mMessageIndex[3] = contentInfo4->mMessageIndex;
        valueList.mParameterLevel = paramLevel;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByTimeLevelAndCircle(0,
              contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,
              contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,
              fTime,paramLevel,coordinateType,x,y,radius,timeInterpolationMethod,iplMethod,modificationOperation,modificationParameters,valueList.mValueList);

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByTimeLevelAndCircle");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getPolygonValues(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    T::ForecastType forecastType,
    T::ForecastNumber forecastNumber,
    uint parameterFlags,
    short timeInterpolationMethod,
    short levelInterpolationMethod,
    uchar coordinateType,
    T::AreaCoordinates& areaCoordinates,
    uint& newProducerId,
    short& precision,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction = conversionFunction(pInfo.mConversionFunction, function, functionParams);

    if (conversionByFunction && mDataServerMethodsEnabled)
    {
      std::string f = toUpperString(function);
      auto p = mOperationNames.find(f);
      if (p != mOperationNames.end())
      {
        modificationOperation = p->second;
        for (auto it = functionParams.begin(); it != functionParams.end(); ++it)
        {
          if (*it != "$")
            modificationParameters.push_back(atof(it->c_str()));
        }
        conversionByFunction = false;
      }
    }

    std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
    time_t fTime = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,
        paramLevelId,paramLevel,forecastType,forecastNumber,parameterFlags,contentList);

    PRINT_DATA(mDebugLog, "         + Found %u content records\n", contentList->getLength());

    if (fTime == 0)
      return false;

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      contentList->print(stream, 0, 4);
      PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
    }

    uint contentLen = contentList->getLength();

    if (contentLen == 0)
      return false;

    short iplMethod = levelInterpolationMethod;
    if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
    {
      if ((parameterFlags & QueryServer::QueryParameter::Flags::HeightLevelInterpolation) != 0)
        iplMethod = T::LevelInterpolationMethod::Linear;
      else
        iplMethod = T::LevelInterpolationMethod::Logarithmic;
    }


    // We found content information close to the current forecast time

    T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList->getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList->getContentInfoByIndex(3);

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelId = paramLevelId;
    valueList.mForecastTimeUTC = forecastTime;
    valueList.mProducerId = contentInfo1->mProducerId;
    valueList.mGenerationId = contentInfo1->mGenerationId;
    valueList.mGenerationFlags = generationFlags;
    valueList.mGeometryId = contentInfo1->mGeometryId;
    valueList.mFileId[0] = contentInfo1->mFileId;
    valueList.mMessageIndex[0] = contentInfo1->mMessageIndex;
    valueList.mModificationTime = contentInfo1->mModificationTime;
    valueList.mAnalysisTime = analysisTime;
    valueList.mForecastType = contentInfo1->mForecastType;
    valueList.mForecastNumber = contentInfo1->mForecastNumber;
    valueList.mParameterLevel = contentInfo1->mParameterLevel;
    valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC == fTime)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        valueList.mFlags = ParameterValues::Flags::DataAvailable;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByPolygonPath(0, contentInfo1->mFileId, contentInfo1->mMessageIndex, coordinateType, areaCoordinates,modificationOperation,modificationParameters,valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByPolygonPath");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
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
        valueList.clear();
        valueList.mProducerId = producerInfo.mProducerId;
        valueList.mParameterKeyType = pInfo.mParameterKeyType;
        valueList.mParameterKey = pInfo.mParameterKey;
        valueList.mForecastTimeUTC = forecastTime;
        return false;
      }
    }


    if (contentLen == 2)
    {
      if (contentInfo1->mForecastTimeUTC < fTime  &&  contentInfo2->mForecastTimeUTC > fTime  &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden)
      {
        // Content records have different times, but most likely the same levels, so we need to do time interpolation.

        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByTimeAndPolygonPath(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,coordinateType,areaCoordinates,timeInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByTimeAndPolygonPath");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }

      if (contentInfo1->mForecastTimeUTC == fTime  &&  contentInfo2->mForecastTimeUTC == fTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&
          levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
      {
        // Content records have the same time but different levels, so we need to level interpolation.

        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByLevelInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
        valueList.mParameterLevel = paramLevel;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByLevelAndPolygonPath(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,coordinateType,areaCoordinates,iplMethod,modificationOperation,modificationParameters,valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByLevelAndPolygonPath");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }
    }


    if (contentLen == 4)
    {
      if (contentInfo1->mForecastTimeUTC < fTime  &&  contentInfo2->mForecastTimeUTC < fTime  &&  contentInfo3->mForecastTimeUTC > fTime  &&  contentInfo4->mForecastTimeUTC > fTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&  contentInfo3->mParameterLevel < paramLevel  &&  contentInfo4->mParameterLevel > paramLevel
          &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
      {
        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation | ParameterValues::Flags::DataAvailableByLevelInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
        valueList.mFileId[2] = contentInfo3->mFileId;
        valueList.mMessageIndex[2] = contentInfo3->mMessageIndex;
        valueList.mFileId[3] = contentInfo4->mFileId;
        valueList.mMessageIndex[3] = contentInfo4->mMessageIndex;
        valueList.mParameterLevel = paramLevel;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByTimeLevelAndPolygonPath(0,
              contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,
              contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,
              fTime,paramLevel,coordinateType,areaCoordinates,timeInterpolationMethod,iplMethod,modificationOperation,modificationParameters,valueList.mValueList);

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByTimeLevelAndPolygonPath");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getIsolineValues(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
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
    T::AttributeList& queryAttributeList,
    uint& newProducerId,
    short& precision,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    // ### Converting contour values if necessary:

    T::ParamValue_vec newContourValues;

    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    if (conversionFunction(pInfo.mReverseConversionFunction, function, functionParams))
      executeConversion(function,functionParams,contourValues,newContourValues);
    else
      newContourValues = contourValues;

    const char *gridWidthStr = queryAttributeList.getAttributeValue("grid.width");
    const char *gridHeightStr = queryAttributeList.getAttributeValue("grid.height");
/*
    std::shared_ptr<T::ContentInfoList> contentList(new T::ContentInfoList());
    int result = getContentListByParameterGenerationIdAndForecastTime(0, producerInfo.mProducerId, producerInfo.mHash, generationId, pInfo.mParameterKeyType, pInfo.mParameterKey, pInfo.getKeyHash(),
        paramLevelId, paramLevel, forecastType, forecastNumber, producerGeometryId, forecastTime, contentList);

    if (result != 0)
    {
      Fmi::Exception exception(BCP, "ContentServer returns an error!");
      exception.addParameter("Service", "getContentListByParameterGenerationIdAndForecastTime");
      exception.addParameter("Message", ContentServer::getResultString(result));
      throw exception;
    }
*/
    std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
    time_t fTime = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,
        paramLevelId,paramLevel,forecastType,forecastNumber,parameterFlags,contentList);

    PRINT_DATA(mDebugLog, "         + Found %u content records\n", contentList->getLength());

    if (fTime == 0)
      return false;

    PRINT_DATA(mDebugLog, "         + Found %u content records\n", contentList->getLength());

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      contentList->print(stream, 0, 4);
      PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
    }

    uint contentLen = contentList->getLength();
    if (contentLen == 0)
      return false;

    T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList->getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList->getContentInfoByIndex(3);

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelId = paramLevelId;

    //valueList.mForecastTime = utcTimeFromTimeT(forecastTime);
    valueList.mForecastTimeUTC = forecastTime;
    valueList.mProducerId = contentInfo1->mProducerId;
    valueList.mGenerationId = contentInfo1->mGenerationId;
    valueList.mGenerationFlags = generationFlags;
    valueList.mGeometryId = contentInfo1->mGeometryId;
    valueList.mFileId[0] = contentInfo1->mFileId;
    valueList.mMessageIndex[0] = contentInfo1->mMessageIndex;
    valueList.mModificationTime = contentInfo1->mModificationTime;
    valueList.mAnalysisTime = analysisTime;
    valueList.mForecastType = contentInfo1->mForecastType;
    valueList.mForecastNumber = contentInfo1->mForecastNumber;
    valueList.mParameterLevel = contentInfo1->mParameterLevel;
    valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;

    queryAttributeList.setAttribute("grid.timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
    queryAttributeList.setAttribute("grid.areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));


    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC == fTime)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        valueList.mFlags = ParameterValues::Flags::DataAvailable;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;

          switch (locationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridIsolinesByGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourValues,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridIsolinesByGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),gridCoordinates,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridIsolines(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourValues,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridIsolinesByGeometry / getGridIsolinesByGrid / getGridIsolines");
            exception.addParameter("LocationType",Fmi::to_string(locationType));
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
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
        valueList.clear();
        valueList.mProducerId = producerInfo.mProducerId;
        valueList.mParameterKeyType = pInfo.mParameterKeyType;
        valueList.mParameterKey = pInfo.mParameterKey;
        valueList.mForecastTimeUTC = forecastTime;
        return false;
      }
    }

    if (contentLen == 2)
    {
      if (contentInfo1->mForecastTimeUTC < fTime && contentInfo2->mForecastTimeUTC > fTime  &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden)
      {
        // We did not find a grid with the exact forecast time, but we find grids that
        // are before and after the current forecast time. This means that we should do
        // some time interpolation.

        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;
          switch (locationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridIsolinesByTimeAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourValues,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridIsolinesByTimeAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),gridCoordinates,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridIsolinesByTime(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourValues,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridIsolinesByTimeAndGeometry / getGridIsolinesByTimeAndGrid / getGridIsolinesByTime");
            exception.addParameter("LocationType",Fmi::to_string(locationType));
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }

      if (contentInfo1->mParameterLevel < paramLevel && contentInfo2->mParameterLevel > paramLevel  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
      {
        // We did not find a grid with the exact level but we find grids that
        // are before and after the current level. This means that we should do
        // some level interpolation.

        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByLevelInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;
          switch (locationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridIsolinesByLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourValues,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridIsolinesByLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),gridCoordinates,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridIsolinesByLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourValues,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridIsolinesByLevelAndGeometry / getGridIsolinesByLevelAndGrid / getGridIsolinesByLevel");
            exception.addParameter("LocationType",Fmi::to_string(locationType));
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
        }

        valueList.mParameterLevel = paramLevel;

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }
    }

    if (contentLen == 4)
    {
      if (contentInfo1->mForecastTimeUTC < fTime  &&  contentInfo2->mForecastTimeUTC < fTime  &&  contentInfo3->mForecastTimeUTC > fTime  &&  contentInfo4->mForecastTimeUTC > fTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&  contentInfo3->mParameterLevel < paramLevel  &&  contentInfo4->mParameterLevel > paramLevel
          &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
      {
        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation | ParameterValues::Flags::DataAvailableByLevelInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
        valueList.mFileId[2] = contentInfo3->mFileId;
        valueList.mMessageIndex[2] = contentInfo3->mMessageIndex;
        valueList.mFileId[3] = contentInfo4->mFileId;
        valueList.mMessageIndex[3] = contentInfo4->mMessageIndex;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;
          switch (locationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridIsolinesByTimeLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourValues,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridIsolinesByTimeLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),gridCoordinates,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridIsolinesByTimeAndLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourValues,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridIsolinesByTimeLevelAndGeometry / getGridIsolinesByTimeLevelAndGrid / getGridIsolinesByTimeAndLevel");
            exception.addParameter("LocationType",Fmi::to_string(locationType));
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getIsobandValues(
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
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
    T::AttributeList& queryAttributeList,
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

    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
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

    const char *gridWidthStr = queryAttributeList.getAttributeValue("grid.width");
    const char *gridHeightStr = queryAttributeList.getAttributeValue("grid.height");

    /*
    std::shared_ptr<T::ContentInfoList> contentList(new T::ContentInfoList());
    int result = getContentListByParameterGenerationIdAndForecastTime(0, producerInfo.mProducerId, producerInfo.mHash, generationId, pInfo.mParameterKeyType, pInfo.mParameterKey, pInfo.getKeyHash(),
        paramLevelId, paramLevel, forecastType, forecastNumber, producerGeometryId, forecastTime, contentList);

    if (result != 0)
    {
      Fmi::Exception exception(BCP, "ContentServer returns an error!");
      exception.addParameter("Service", "getContentListByParameterGenerationIdAndForecastTime");
      exception.addParameter("Message", ContentServer::getResultString(result));
      throw exception;
    }
    */

    std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
    time_t fTime = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,
        paramLevelId,paramLevel,forecastType,forecastNumber,parameterFlags,contentList);

    PRINT_DATA(mDebugLog, "         + Found %u content records\n", contentList->getLength());

    if (fTime == 0)
      return false;

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      contentList->print(stream, 0, 4);
      PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
    }

    uint contentLen = contentList->getLength();
    if (contentLen == 0)
      return false;

    T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList->getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList->getContentInfoByIndex(3);

    valueList.mParameterKeyType = pInfo.mParameterKeyType;
    valueList.mParameterKey = pInfo.mParameterKey;
    valueList.mParameterLevelId = paramLevelId;
    valueList.mForecastTimeUTC = forecastTime;
    valueList.mProducerId = contentInfo1->mProducerId;
    valueList.mGenerationId = contentInfo1->mGenerationId;
    valueList.mGenerationFlags = generationFlags;
    valueList.mGeometryId = contentInfo1->mGeometryId;
    valueList.mFileId[0] = contentInfo1->mFileId;
    valueList.mMessageIndex[0] = contentInfo1->mMessageIndex;
    valueList.mModificationTime = contentInfo1->mModificationTime;
    valueList.mAnalysisTime = analysisTime;
    valueList.mForecastType = contentInfo1->mForecastType;
    valueList.mForecastNumber = contentInfo1->mForecastNumber;
    valueList.mParameterLevel = contentInfo1->mParameterLevel;
    valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;

    queryAttributeList.setAttribute("grid.timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
    queryAttributeList.setAttribute("grid.areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC == fTime)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        valueList.mFlags = ParameterValues::Flags::DataAvailable;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;

          switch (locationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridIsobandsByGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourLowValues,newContourHighValues,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridIsobandsByGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourLowValues,newContourHighValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),gridCoordinates,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridIsobands(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourLowValues,newContourHighValues,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridIsobandsByGeometry / getGridIsobandsByGrid / getGridIsobands");
            exception.addParameter("LocationType",Fmi::to_string(locationType));
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
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
        valueList.clear();
        valueList.mProducerId = producerInfo.mProducerId;
        valueList.mParameterKeyType = pInfo.mParameterKeyType;
        valueList.mParameterKey = pInfo.mParameterKey;
        valueList.mForecastTimeUTC = forecastTime;
        return false;
      }
    }

    if (contentLen == 2)
    {
      if (contentInfo1->mForecastTimeUTC < fTime && contentInfo2->mForecastTimeUTC > fTime  &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden)
      {
        // We did not find a grid with the exact forecast time, but we find grids that
        // are before and after the current forecast time. This means that we should do
        // some time interpolation.

        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;
          switch (locationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridIsobandsByTimeAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourLowValues,newContourHighValues,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridIsobandsByTimeAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourLowValues,newContourHighValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),gridCoordinates,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridIsobandsByTime(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourLowValues,newContourHighValues,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridIsobandsByTimeAndGeometry / getGridIsobandsByTimeAndGrid / getGridIsobandsByTime");
            exception.addParameter("LocationType",Fmi::to_string(locationType));
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
        }

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }


      if (contentInfo1->mParameterLevel < paramLevel && contentInfo2->mParameterLevel > paramLevel  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
      {
        // We did not find a grid with the exact level but we find grids that
        // are before and after the current level. This means that we should do
        // some level interpolation.

        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByLevelInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;
          switch (locationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridIsobandsByLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourLowValues,newContourHighValues,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridIsobandsByLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourLowValues,newContourHighValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),gridCoordinates,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridIsobandsByLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourLowValues,newContourHighValues,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridIsobandsByLevelAndGeometry / getGridIsobandsByLevelAndGrid / getGridIsobandsByLevel");
            exception.addParameter("LocationType",Fmi::to_string(locationType));
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
        }

        valueList.mParameterLevel = paramLevel;

        if (precision < 0)
          precision = pInfo.mDefaultPrecision;

        return true;
      }
    }

    if (contentLen == 4)
    {
      if (contentInfo1->mForecastTimeUTC < fTime  &&  contentInfo2->mForecastTimeUTC < fTime  &&  contentInfo3->mForecastTimeUTC > fTime  &&  contentInfo4->mForecastTimeUTC > fTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&  contentInfo3->mParameterLevel < paramLevel  &&  contentInfo4->mParameterLevel > paramLevel
          &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
      {
        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation | ParameterValues::Flags::DataAvailableByLevelInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
        valueList.mFileId[2] = contentInfo3->mFileId;
        valueList.mMessageIndex[2] = contentInfo3->mMessageIndex;
        valueList.mFileId[3] = contentInfo4->mFileId;
        valueList.mMessageIndex[3] = contentInfo4->mMessageIndex;

        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;
          switch (locationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridIsobandsByTimeLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourLowValues,newContourHighValues,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridIsobandsByTimeLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourLowValues,newContourHighValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),gridCoordinates,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridIsobandsByTimeAndLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourLowValues,newContourHighValues,queryAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridIsobandsByTimeLevelAndGeometry / getGridIsobandsByTimeLevelAndGrid / getGridIsobandsByTimeAndLevel");
            exception.addParameter("LocationType",Fmi::to_string(locationType));
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::getGridValues(
    uchar queryType,
    Producer_vec& producers,
    std::set<T::GeometryId>& geometryIdList,
    uint producerId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    bool reverseGenerations,
    bool acceptNotReadyGenerations,
    const std::string& parameterKey,
    std::size_t parameterHash,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    T::ForecastType forecastType,
    T::ForecastNumber forecastNumber,
    uint queryFlags,
    uint parameterFlags,
    short areaInterpolationMethod,
    short timeInterpolationMethod,
    short levelInterpolationMethod,
    time_t forecastTime,
    bool timeMatchRequired,
    uchar locationType,
    uchar coordinateType,
    T::AreaCoordinates& areaCoordinates,
    T::ParamValue_vec& contourLowValues,
    T::ParamValue_vec& contourHighValues,
    T::AttributeList& queryAttributeList,
    double radius,
    short& precision,
    ParameterValues& valueList,
    T::Coordinate_vec& coordinates,
    std::string& producerStr)
{
  FUNCTION_TRACE
  try
  {
    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      PRINT_DATA(mDebugLog, "\nMETHOD getGridValues()\n");
      PRINT_DATA(mDebugLog, "  - queryType                : %u\n", queryType);
      PRINT_DATA(mDebugLog, "  - producers                : %lu items\n", producers.size());
      for (auto it = producers.begin(); it != producers.end(); ++it)
        PRINT_DATA(mDebugLog, "    * %s:%d\n", it->first.c_str(), it->second);
      PRINT_DATA(mDebugLog, "  - geometryIdList           : %lu items\n", geometryIdList.size());
      for (auto it = geometryIdList.begin(); it != geometryIdList.end(); ++it)
        PRINT_DATA(mDebugLog, "    * %d\n",* it);
      PRINT_DATA(mDebugLog, "  - producerId               : %u\n", producerId);
      PRINT_DATA(mDebugLog, "  - analysisTime             : %s\n", analysisTime.c_str());
      PRINT_DATA(mDebugLog, "  - generationFlags          : %llu\n", generationFlags);
      PRINT_DATA(mDebugLog, "  - reverseGenerations       : %d\n", reverseGenerations);
      PRINT_DATA(mDebugLog, "  - parameterKey             : %s\n", parameterKey.c_str());
      PRINT_DATA(mDebugLog, "  - paramLevelId             : %d\n", paramLevelId);
      PRINT_DATA(mDebugLog, "  - paramLevel               : %d\n", paramLevel);
      PRINT_DATA(mDebugLog, "  - forecastType             : %d\n", forecastType);
      PRINT_DATA(mDebugLog, "  - forecastNumber           : %d\n", forecastNumber);
      PRINT_DATA(mDebugLog, "  - queryFlags               : %04x\n", queryFlags);
      PRINT_DATA(mDebugLog, "  - parameterFlags           : %04x\n", parameterFlags);
      PRINT_DATA(mDebugLog, "  - areaInterpolationMethod  : %d\n", areaInterpolationMethod);
      PRINT_DATA(mDebugLog, "  - timeInterpolationMethod  : %d\n", timeInterpolationMethod);
      PRINT_DATA(mDebugLog, "  - levelInterpolationMethod : %d\n", levelInterpolationMethod);
      PRINT_DATA(mDebugLog, "  - forecastTime             : %s\n", utcTimeFromTimeT(forecastTime).c_str());
      PRINT_DATA(mDebugLog, "  - forecastTimeT            : %ld\n", forecastTime);
      PRINT_DATA(mDebugLog, "  - timeMatchRequired        : %d\n", timeMatchRequired);
      PRINT_DATA(mDebugLog, "  - locationType             : %d\n", locationType);
      PRINT_DATA(mDebugLog, "  - coordinateType           : %d\n", coordinateType);
      PRINT_DATA(mDebugLog, "  - areaCoordinates          : %lu vectors\n", areaCoordinates.size());
      PRINT_DATA(mDebugLog, "  - contourLowValues         : %lu values\n",contourLowValues.size());
      PRINT_DATA(mDebugLog, "  - contourHighValues        : %lu values\n",contourHighValues.size());
      PRINT_DATA(mDebugLog, "  - radius                   : %f\n", radius);
    }

    if (queryType != QueryParameter::Type::GridFile  &&  areaCoordinates.size() == 0  &&  queryAttributeList.getAttributeValue("grid.geometryId") == nullptr  &&  queryAttributeList.getAttributeValue("grid.geometryString") == nullptr  &&  queryAttributeList.getAttributeValue("grid.urn") == nullptr  &&  queryAttributeList.getAttributeValue("grid.crs") == nullptr)
    {
      Fmi::Exception exception(BCP, "No areaCoordinates defined!");
      throw exception;
    }

    if (locationType == QueryParameter::LocationType::Polygon)
    {
      for (size_t t = 0; t < areaCoordinates.size(); t++)
      {
        if (areaCoordinates[t].size() < 3)
        {
          Fmi::Exception exception(BCP, "Polygon definition requires at least 3 areaCoordinates!");
          throw exception;
        }
      }
    }

    time_t requiredAccessTime = time(nullptr) + 120;

    // Going through the producer list.

    for (auto it = producers.begin(); it != producers.end(); ++it)
    {
      std::string producerName = it->first;
      T::GeometryId producerGeometryId = it->second;

      PRINT_DATA(mDebugLog, "  - Producer and Geometry : %s:%d\n", producerName.c_str(), producerGeometryId);

      if (geometryIdList.find(producerGeometryId) != geometryIdList.end())
      {
        bool producerOk = true;
        if (producers.size() > 1 && (locationType == QueryParameter::LocationType::Polygon || locationType == QueryParameter::LocationType::Circle || locationType == QueryParameter::LocationType::Path))
        {
          // We do not accept data concatenation if the data is requested from an area and
          // if geometries are different.

          std::string pStr = toUpperString(producerName) + ":" + std::to_string(producerGeometryId);

          auto rec = mProducerConcatMap.find(pStr);
          if (rec != mProducerConcatMap.end()  &&  rec->second.size() > 0)
          {
            for (auto gg = rec->second.begin(); gg != rec->second.end() && producerOk; ++gg)
            {
              if (producerGeometryId != gg->second)
                producerOk = false;
            }
          }
        }

        // The current producer supports a geometry where the current areaCoordinates can be found.

        T::ProducerInfo producerInfo;
        if (producerOk &&  getProducerInfoByName(producerName, producerInfo) && (producerId == 0 || producerInfo.mProducerId == producerId))
        {
          PRINT_DATA(mDebugLog, "  - The producer and the geometry are acceptable!\n");

          // Reading generations supported by the current producer.

          auto cacheEntry = getGenerationInfoListByProducerId(producerInfo.mProducerId,true);

          uint gLen = cacheEntry->generationInfoList->getLength();
          if (gLen == 0)
            PRINT_DATA(mDebugLog, "    - No generations found for the current producer!\n");

          // Sorting generation analysis times.

          std::vector<std::string> analysisTimesVec;
          std::vector<std::string> *analysisTimes = cacheEntry->analysisTimes.get();
          if (reverseGenerations)
          {
            cacheEntry->generationInfoList->getAnalysisTimes(analysisTimesVec,false);
            analysisTimes = &analysisTimesVec;
          }


          // Going through all the parameter mapping files, until we find a match.

          ParameterMapping_vec_sptr mappings;
          if (paramLevelId > 0 || paramLevel > 0)
          {
            getParameterMappings(producerInfo.mName,producerInfo.mProducerId,parameterKey,parameterHash,producerGeometryId, paramLevelId, paramLevel, false, mappings);
            if (mappings->size() == 0)
            {
              // Getting a mapping by using the levelId. This probably returns all levels belonging
              // to the current levelId. We need just one for mapping.
              ParameterMapping_vec_sptr tmpMappings;
              getParameterMappings(producerInfo.mName, producerInfo.mProducerId, parameterKey, parameterHash, producerGeometryId, paramLevelId, -1,false, tmpMappings);
              if (tmpMappings->size() > 0)
                mappings->emplace_back((*tmpMappings)[0]);
            }
          }
          else
          {
            getParameterMappings(producerInfo.mName, producerInfo.mProducerId, parameterKey, parameterHash, producerGeometryId, true, mappings);
          }

          if (mappings->size() == 0 &&  strncasecmp(parameterKey.c_str(),"FMI-",4) == 0)
          {
            ParameterMapping mp;
            mp.mProducerName = producerInfo.mName;
            mp.mParameterName = parameterKey;
            mp.mParameterKeyType = T::ParamKeyTypeValue::FMI_ID;
            mp.mParameterKey = parameterKey.c_str() + 4;
            mp.mGeometryId = producerGeometryId;
            mp.mParameterLevelId = paramLevelId;
            mp.mParameterLevel = paramLevel;
            mappings->emplace_back(mp);
          }

          if (mappings->size() == 0)
            PRINT_DATA(mDebugLog, "    - No parameter mappings '%s:%s:%d:%d:%d' found!\n", producerInfo.mName.c_str(), parameterKey.c_str(), producerGeometryId, paramLevelId, paramLevel);


          T::GenerationInfo* origGenerationInfo = nullptr;
          if (!analysisTime.empty())
            origGenerationInfo = cacheEntry->generationInfoList->getGenerationInfoByAnalysisTime(analysisTime);

          if (queryFlags & Query::Flags::LatestGeneration)
          {
            if (acceptNotReadyGenerations)
              origGenerationInfo = cacheEntry->generationInfoList->getLastGenerationInfoByAnalysisTime();
            else
              origGenerationInfo = cacheEntry->generationInfoList->getLastGenerationInfoByAnalysisTime(T::GenerationInfo::Status::Ready);
          }

          if (queryFlags & Query::Flags::OldestGeneration)
          {
            if (acceptNotReadyGenerations)
              origGenerationInfo = cacheEntry->generationInfoList->getFirstGenerationInfoByAnalysisTime();
            else
              origGenerationInfo = cacheEntry->generationInfoList->getFirstGenerationInfoByAnalysisTime(T::GenerationInfo::Status::Ready);
          }

          bool climatologicalMappings = false;
          if (mappings->size() > 0)
          {
            for (auto pInfo = mappings->begin(); pInfo != mappings->end() && !climatologicalMappings; ++pInfo)
            {
              if (pInfo->mGroupFlags & (ParameterMapping::GroupFlags::climatology | ParameterMapping::GroupFlags::global))
              {
                climatologicalMappings = true;
                PRINT_DATA(mDebugLog, "    - Climatological mappings found!\n");
              }
            }

            PRINT_DATA(mDebugLog, "  - Going through the generations from the newest to the oldest.\n");


            uint gLen = analysisTimes->size();
            for (uint g = 0; g < gLen; g++)
            {
              if (origGenerationInfo != nullptr)
              {
                g = gLen; // Making sure that the loop is executed only once
                PRINT_DATA(mDebugLog, "    * %s\n", analysisTime.c_str());
              }
              else
              {
                PRINT_DATA(mDebugLog, "    * %s\n", (*analysisTimes)[g].c_str());
              }

              uint gflags = 1 << g;

              bool generationValid = false;

              if (!analysisTime.empty()  &&  origGenerationInfo == nullptr)
              {
                if (analysisTime == (*analysisTimes)[g])
                {
                  generationValid = true;
                }
                else
                if ((queryFlags & Query::Flags::AnalysisTimeMatchRequired) == 0)
                {
                  if (g == 0  &&  (*analysisTimes)[g] < analysisTime)
                  {
                    generationValid = true;
                  }
                  else
                  if ((g+1) == gLen  &&  (*analysisTimes)[g] > analysisTime)
                  {
                    generationValid = true;
                  }
                  else
                  if ((*analysisTimes)[g] > analysisTime  &&  (g+1) < gLen  &&  (*analysisTimes)[g+1] < analysisTime)
                  {
                    generationValid = true;
                  }
                }
              }
              else
              {
                if (generationFlags == 0 || (generationFlags & gflags) != 0)
                  generationValid = true;
              }

              if (!generationValid  &&  climatologicalMappings)
                generationValid = true;

              // ### Checking if the generation is going to be deleted in few minutes

              T::GenerationInfo *generationInfo = origGenerationInfo;
              if (origGenerationInfo == nullptr)
                generationInfo = cacheEntry->generationInfoList->getGenerationInfoByAnalysisTime((*analysisTimes)[g]);

              if (generationInfo == nullptr || (generationInfo != nullptr && generationInfo->mDeletionTime > 0 && generationInfo->mDeletionTime < requiredAccessTime))
                generationValid = false;

              if (generationInfo != nullptr  &&  !acceptNotReadyGenerations)
              {
                // Checking if the geometry is ready.
                if (mCheckGeometryStatus)
                {
                  if (!isGeometryReady(generationInfo->mGenerationId,producerGeometryId,0))
                    generationValid = false;
                }
                else
                {
                  if (generationInfo->mStatus != T::GenerationInfo::Status::Ready)
                    generationValid = false;
                }
              }

              if (generationInfo != nullptr  &&  !climatologicalMappings  &&  producerId != 0)
              {
                // Checking if the requested forecast time is outside of the generation's time range.
                time_t startTime = 0;
                time_t endTime = 0;

                getGenerationTimeRangeByProducerAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,startTime,endTime);
                if (forecastTime < startTime || forecastTime > endTime)
                  generationValid = false;
              }

              if (generationValid)
              {
                producerStr = toUpperString(producerInfo.mName) + ":" + std::to_string(producerGeometryId);

                PRINT_DATA(mDebugLog, "      - Going through the parameter mappings\n");
                for (auto pInfo = mappings->begin(); pInfo != mappings->end(); ++pInfo)
                {
                  if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
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

                  if (queryType == QueryParameter::Type::PointValues)
                  {

                    // ### LOCATION TYPE : Polygon

                    if (locationType == QueryParameter::LocationType::Polygon)
                    {
                      // ### Simple interpolation

                      if (getPolygonValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                          parameterFlags, timeInterpolation, levelInterpolation, coordinateType, areaCoordinates, producerId, precision, valueList))
                        return;
                    }


                    // ### LOCATION TYPE : Circle

                    if (locationType == QueryParameter::LocationType::Circle && areaCoordinates.size() == 1 && areaCoordinates[0].size() == 1)
                    {
                      // ### Simple interpolation

                      double x = areaCoordinates[0][0].x();
                      double y = areaCoordinates[0][0].y();

                      if (getCircleValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                          parameterFlags, timeInterpolation, levelInterpolation, coordinateType, x, y, radius, producerId, precision, valueList))
                        return;
                    }


                    // ### LOCATION TYPE : Path

                    if (locationType == QueryParameter::LocationType::Path)
                    {
                      if (areaInterpolation < T::AreaInterpolationMethod::List)
                      {
                        // ### Simple interpolation

                        if (getPointValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                            parameterFlags, areaInterpolation, timeInterpolation, levelInterpolation, coordinateType, areaCoordinates, producerId,precision,  valueList))
                          return;
                      }

                      if (areaInterpolation >= T::AreaInterpolationMethod::List && areaInterpolation < T::AreaInterpolationMethod::External && areaCoordinates.size() == 1  && areaCoordinates[0].size() == 1)
                      {
                        // ### No actual intepolation. Fetching special value for external interpolation

                        double x = areaCoordinates[0][0].x();
                        double y = areaCoordinates[0][0].y();

                        if (getSpecialValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                            parameterFlags, areaInterpolation, timeInterpolation, levelInterpolation, coordinateType, x, y, producerId, precision, valueList))
                          return;
                      }
                    }


                    // ### LOCATION TYPE : Point

                    if (locationType == QueryParameter::LocationType::Point)
                    {
                      if (areaInterpolation == T::AreaInterpolationMethod::Landscape && areaCoordinates.size() == 1 && areaCoordinates[0].size() == 1)
                      {
                        double x = areaCoordinates[0][0].x();
                        double y = areaCoordinates[0][0].y();

                        if (getSpecialValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                            parameterFlags, T::AreaInterpolationMethod::List, timeInterpolationMethod, levelInterpolationMethod, coordinateType, x, y, producerId, precision, valueList))
                        {
                          Producer_vec pList;
                          std::set<T::GeometryId> gList;

                          pList.emplace_back(std::pair<std::string,T::GeometryId>(it->first,it->second));
                          gList.insert(it->second);

                          std::string param = "GeopHeight;" + it->first + ";" + std::to_string(it->second);
                          std::string paramStr;

                          if (!getAlias(param,paramStr))
                            paramStr = param;


                          std::string x_param;
                          T::GeometryId x_geometryId = it->second;
                          T::ParamLevelId x_paramLevelId = 0;
                          T::ParamLevel x_paramLevel = 0;
                          T::ForecastType x_forecastType = 1;
                          T::ForecastNumber x_forecastNumber = -1;
                          std::string x_producerName;
                          uint x_producerId = 0;
                          ulonglong x_generationFlags = 0;
                          short x_areaInterpolationMethod = 500;
                          short x_timeInterpolationMethod = 1;
                          short x_levelInterpolationMethod = 1;

                          getParameterStringInfo(paramStr,';',x_param,x_geometryId,x_paramLevelId,x_paramLevel,x_forecastType,x_forecastNumber,x_producerName,
                            x_producerId,x_generationFlags,x_areaInterpolationMethod,x_timeInterpolationMethod,x_levelInterpolationMethod);

                          if (strcasecmp(x_producerName.c_str(),it->first.c_str()) != 0)
                          {
                            pList.emplace_back(std::pair<std::string,T::GeometryId>(x_producerName,x_geometryId));
                            gList.insert(x_geometryId);
                          }

                          ParameterValues zhValueList;
                          getGridValues(queryType,pList,gList,x_producerId,
                            analysisTime,generationFlags,reverseGenerations,acceptNotReadyGenerations,x_param,0,x_paramLevelId,x_paramLevel,
                            x_forecastType,x_forecastNumber,queryFlags,parameterFlags,x_areaInterpolationMethod,x_timeInterpolationMethod,
                            x_levelInterpolationMethod,forecastTime,timeMatchRequired,locationType,
                            coordinateType,areaCoordinates,contourLowValues,contourHighValues,
                            queryAttributeList,radius,precision,zhValueList,coordinates,producerStr);

                          param = "LapseRate;" + it->first + ";" + std::to_string(it->second);
                          if (!getAlias(param,paramStr))
                            paramStr = param;

                          x_paramLevelId = 0;
                          x_paramLevel = 0;

                          getParameterStringInfo(paramStr,';',x_param,x_geometryId,x_paramLevelId,x_paramLevel,x_forecastType,x_forecastNumber,x_producerName,
                            x_producerId,x_generationFlags,x_areaInterpolationMethod,x_timeInterpolationMethod,x_levelInterpolationMethod);

                          if (strcasecmp(x_producerName.c_str(),it->first.c_str()) != 0)
                          {
                            pList.emplace_back(std::pair<std::string,T::GeometryId>(x_producerName,x_geometryId));
                            gList.insert(x_geometryId);
                          }


                          ParameterValues lrValueList;
                          getGridValues(queryType,pList,gList,x_producerId,
                            analysisTime,generationFlags,reverseGenerations,acceptNotReadyGenerations,x_param,0,x_paramLevelId,x_paramLevel,
                            x_forecastType,x_forecastNumber,queryFlags,parameterFlags,x_areaInterpolationMethod,x_timeInterpolationMethod,
                            x_levelInterpolationMethod,forecastTime,timeMatchRequired,locationType,
                            coordinateType,areaCoordinates,contourLowValues,contourHighValues,
                            queryAttributeList,radius,precision,lrValueList,coordinates,producerStr);

                          param = "LandSeaMask;" + it->first + ";" + std::to_string(it->second);
                          if (!getAlias(param,paramStr))
                            paramStr = param;

                          x_paramLevelId = 0;
                          x_paramLevel = 0;

                          getParameterStringInfo(paramStr,';',x_param,x_geometryId,x_paramLevelId,x_paramLevel,x_forecastType,x_forecastNumber,x_producerName,
                            x_producerId,x_generationFlags,x_areaInterpolationMethod,x_timeInterpolationMethod,x_levelInterpolationMethod);

                          if (strcasecmp(x_producerName.c_str(),it->first.c_str()) != 0)
                          {
                            pList.emplace_back(std::pair<std::string,T::GeometryId>(x_producerName,x_geometryId));
                            gList.insert(x_geometryId);
                          }
                          ParameterValues lsValueList;
                          getGridValues(queryType,pList,gList,x_producerId,
                            analysisTime,generationFlags,reverseGenerations,acceptNotReadyGenerations,x_param,0,x_paramLevelId,x_paramLevel,
                            x_forecastType,x_forecastNumber,queryFlags,parameterFlags,x_areaInterpolationMethod,x_timeInterpolationMethod,
                            x_levelInterpolationMethod,forecastTime,timeMatchRequired,locationType,
                            coordinateType,areaCoordinates,contourLowValues,contourHighValues,
                            queryAttributeList,radius,precision,lsValueList,coordinates,producerStr);

                          std::vector<double> vList;
                          std::vector<double> zhList;
                          std::vector<double> lrList;
                          std::vector<double> lsList;
                          std::vector<double> missingList = {ParamValueMissing,ParamValueMissing,ParamValueMissing,ParamValueMissing,ParamValueMissing,ParamValueMissing,ParamValueMissing};

                          double dem = getAdditionalValue("dem",forecastTime,x,y);
                          int coverType = getAdditionalValue("coverType",forecastTime,x,y);

                          if (zhValueList.mValueList.getLength() == 1)
                          {
                            T::GridValue *v = zhValueList.mValueList.getGridValuePtrByIndex(0);
                            if (v != nullptr)
                              splitString(v->mValueString,';',zhList);
                          }
                          if (zhList.size() == 0)
                            zhList = missingList;

                          if (lrValueList.mValueList.getLength() == 1)
                          {
                            T::GridValue *v = lrValueList.mValueList.getGridValuePtrByIndex(0);
                            if (v != nullptr)
                              splitString(v->mValueString,';',lrList);
                          }
                          if (lrList.size() == 0)
                            lrList = missingList;

                          if (lsValueList.mValueList.getLength() == 1)
                          {
                            T::GridValue *v = lsValueList.mValueList.getGridValuePtrByIndex(0);
                            if (v != nullptr)
                              splitString(v->mValueString,';',lsList);
                          }
                          if (lsList.size() == 0)
                            lsList = missingList;


                          if (valueList.mValueList.getLength() == 1)
                          {
                            T::GridValue *v = valueList.mValueList.getGridValuePtrByIndex(0);
                            if (v != nullptr)
                            {
                              splitString(v->mValueString,';',vList);
                              if (vList.size() == 0)
                                return;

                              PRINT_DATA(mDebugLog, "  - Landscape interpolation : %f,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
                                  dem,coverType,
                                  vList[1],vList[2],vList[3],vList[4],vList[5],vList[6],
                                  zhList[3],zhList[4],zhList[5],zhList[6],
                                  lrList[3],lrList[4],lrList[5],lrList[6],
                                  lsList[3],lsList[4],lsList[5],lsList[6]);

                              double val = landscapeInterpolation(
                                  dem,coverType,
                                  vList[1],vList[2],vList[3],vList[4],vList[5],vList[6],
                                  zhList[3],zhList[4],zhList[5],zhList[6],
                                  lrList[3],lrList[4],lrList[5],lrList[6],
                                  lsList[3],lsList[4],lsList[5],lsList[6]);

                              v->mValueString = "";
                              v->mValue = val;
                              return;
                            }
                          }
                        }
                        return;
                      }


                      if (areaInterpolation < T::AreaInterpolationMethod::List)
                      {
                        // ### Simple interpolation

                        if (getPointValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                            parameterFlags, areaInterpolation, timeInterpolation, levelInterpolation, coordinateType, areaCoordinates, producerId, precision, valueList))
                          return;
                      }


                      if (areaInterpolation >= T::AreaInterpolationMethod::List && areaInterpolation < T::AreaInterpolationMethod::External && areaCoordinates.size() == 1 && areaCoordinates[0].size() == 1)
                      {
                        // ### No actual intepolation. Fetching special value for external interpolation

                        double x = areaCoordinates[0][0].x();
                        double y = areaCoordinates[0][0].y();

                        if (getSpecialValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                            parameterFlags, areaInterpolation, timeInterpolationMethod, levelInterpolationMethod, coordinateType, x, y, producerId, precision, valueList))
                          return;
                      }


                      if (areaInterpolation >= T::AreaInterpolationMethod::External && areaCoordinates.size() == 1 && areaCoordinates[0].size() == 1)
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
                            pInfo->mParameterKey, pLevelId, pLevel, forecastType, forecastNumber, areaInterpolation);

                        valueList.mForecastTimeUTC = forecastTime;
                        valueList.mProducerId = producerInfo.mProducerId;
                        valueList.mGenerationId = generationInfo->mGenerationId;
                        valueList.mAnalysisTime = generationInfo->mAnalysisTime;
                        valueList.mGenerationFlags = gflags;
                        valueList.mGeometryId = producerGeometryId;

                        valueList.mParameterKeyType = pInfo->mParameterKeyType;
                        valueList.mParameterKey = pInfo->mParameterKey;
                        valueList.mParameterLevelId = pInfo->mParameterLevelId;
                        valueList.mParameterLevel = pInfo->mParameterLevel;
                        valueList.mForecastType = forecastType;
                        valueList.mForecastNumber = forecastNumber;

                        std::vector<double> constParams;
                        std::vector<ParameterValues> valueListVec;

                        std::vector <std::string> paramVec;
                        splitString(paramList, ';', paramVec);

                        // ### Processing interpolation instructions and fetching required data

                        for (auto param = paramVec.begin(); param != paramVec.end(); ++param)
                        {
                          uint a_producerId = producerId;
                          std::string a_analysisTime = analysisTime;
                          ulonglong a_generationFlags = generationFlags;
                          bool a_reverseGenerations = reverseGenerations;
                          bool a_acceptNotReadyGenerations = acceptNotReadyGenerations;
                          std::string a_parameterKey = parameterKey;
                          T::ParamLevelId a_paramLevelId = paramLevelId;
                          T::ParamLevel a_paramLevel = paramLevel;
                          T::ForecastType a_forecastType = forecastType;
                          T::ForecastNumber a_forecastNumber = forecastNumber;
                          uint a_queryFlags = queryFlags;
                          uint a_parameterFlags = parameterFlags;
                          short a_areaInterpolationMethod = areaInterpolationMethod;
                          short a_timeInterpolationMethod = timeInterpolationMethod;
                          short a_levelInterpolationMethod = levelInterpolationMethod;
                          time_t a_forecastTime = forecastTime;
                          bool a_timeMatchRequired = timeMatchRequired;
                          uchar a_locationType = locationType;
                          double a_radius = radius;

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
                              double vv = ParamValueMissing;
                              const char *ss = queryAttributeList.getAttributeValue(paramParts[1].c_str());
                              if (ss != nullptr)
                                vv = toDouble(ss);
                              else
                                vv = getAdditionalValue(paramParts[1],forecastTime,areaCoordinates[0][0].x(),areaCoordinates[0][0].y());

                              constParams.emplace_back(vv);
                            }
                            if (paramParts[0] == "C")
                            {
                              constParams.emplace_back(toDouble(paramParts[1].c_str()));
                            }
                            else if (paramParts[0] == "Q" ||  paramParts[0] == "P")
                            {
                              std::vector<std::string> pv;
                              splitString(paramParts[1], ',', pv);

                              if (pv.size() == 8)
                              {
                                if (!pv[0].empty())
                                  a_parameterKey = pv[0];

                                if (!pv[1].empty())
                                  a_paramLevelId = toUInt8(pv[1].c_str());

                                if (!pv[2].empty())
                                  a_paramLevel = toInt32(pv[2].c_str());

                                if (!pv[3].empty())
                                  a_forecastType = toInt16(pv[3].c_str());

                                if (!pv[4].empty())
                                  a_forecastNumber = toInt16(pv[4].c_str());

                                if (!pv[5].empty())
                                  a_areaInterpolationMethod = toInt16(pv[5].c_str());

                                if (!pv[6].empty() && toInt64(pv[6].c_str()) == 1)
                                {
                                  T::ContentInfoList contentList;
                                  if (mContentServerPtr->getContentListByParameterAndGenerationId(0, generationInfo->mGenerationId, T::ParamKeyTypeValue::FMI_NAME, a_parameterKey,
                                      a_paramLevelId, a_paramLevel, a_paramLevel, a_forecastType, a_forecastNumber, producerGeometryId, 0,
                                      0xFFFFFFFF, 0, contentList) == 0)
                                  {
                                    if (contentList.getLength() > 0)
                                    {
                                      T::ContentInfo* cInfo = contentList.getContentInfoByIndex(0);
                                      a_forecastTime = cInfo->mForecastTimeUTC;
                                    }
                                  }
                                }

                                if (!pv[7].empty() && toInt64(pv[7].c_str()) == 1)
                                  a_producerId = 0;

                                // We should search data only from similar geometries. Otherwise
                                // the size and location of the grid cells are different.

                                std::set<T::GeometryId> geomIdList;
                                geomIdList.insert(producerGeometryId);

                                ParameterValues valList;
                                getGridValues(queryType,producers, geomIdList, a_producerId, a_analysisTime, a_generationFlags, a_reverseGenerations, a_acceptNotReadyGenerations, a_parameterKey, 0, a_paramLevelId,
                                    a_paramLevel, a_forecastType, a_forecastNumber, a_queryFlags, a_parameterFlags, a_areaInterpolationMethod, a_timeInterpolationMethod,
                                    a_levelInterpolationMethod, a_forecastTime, a_timeMatchRequired, a_locationType, coordinateType, areaCoordinates, contourLowValues, contourHighValues,
                                    queryAttributeList,a_radius, precision, valList,coordinates,producerStr);

                                valueList.mFlags = valList.mFlags;

                                if (paramParts[0] == "Q")
                                  valList.mFlags |= (1 << 31);

                                valueListVec.emplace_back(valList);
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

                                if (vList->mFlags & (1 << 31))
                                  params.emplace_back(vec.size());

                                if (vec.size() > 0)
                                {
                                  for (auto it = vec.begin(); it != vec.end(); ++it)
                                    params.emplace_back(*it);
                                }
                              }
                              else
                              {
                                if (vList->mFlags & (1 << 31))
                                  params.emplace_back(1);

                                params.emplace_back(v.mValue);
                              }
                            }
                            else
                            {
                              if (vList->mFlags & (1 << 31))
                                params.emplace_back(1);

                              params.emplace_back(ParamValueMissing);
                            }
                          }

                          // ### Calling the LUA interpolation function.

                          double val = mLuaFileCollection.executeFunctionCall1(interpolationFunction, params);
                          T::GridValue rec(vx, vy, val);
                          valueList.mValueList.addGridValue(rec);

                          std::string function;
                          std::vector<std::string> functionParams;
                          bool conversionByFunction = conversionFunction(pInfo->mConversionFunction, function, functionParams);
                          if (conversionByFunction)
                            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
                        }
                        return;
                      }
                    }

                  }

                  // ### ISOLINE QUERY

                  if (queryType == QueryParameter::Type::Isoline)
                  {
                    if (getIsolineValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                      parameterFlags, areaInterpolation, timeInterpolation, levelInterpolation, locationType, coordinateType, areaCoordinates[0], contourLowValues, queryAttributeList, producerId, precision, valueList))
                    {
                      return;
                    }
                  }


                  // ### ISOBAND QUERY

                  if (queryType == QueryParameter::Type::Isoband)
                  {
                    if (getIsobandValues(producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                        parameterFlags, areaInterpolation, timeInterpolation, levelInterpolation, locationType, coordinateType, areaCoordinates[0], contourLowValues, contourHighValues, queryAttributeList, producerId, precision, valueList))
                    {
                      return;
                    }
                  }

                  // ### VECTOR QUERY

                  if (queryType == QueryParameter::Type::Vector)
                  {
                    if (getValueVectors(producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                        parameterFlags, areaInterpolation, timeInterpolation, levelInterpolation, locationType, coordinateType, areaCoordinates[0], queryAttributeList, producerId, precision, valueList, coordinates))
                    {
                      return;
                    }
                  }

                  // ### GRID FILE QUERY

                  if (queryType == QueryParameter::Type::GridFile)
                  {
                    if (getGridFiles(producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo, forecastTime, pLevelId, pLevel, forecastType, forecastNumber,
                        parameterFlags, areaInterpolation, timeInterpolation, levelInterpolation, locationType, coordinateType, areaCoordinates[0], queryAttributeList, producerId, precision, valueList))
                    {
                      return;
                    }
                  }

                }
              }
            }
          }
        }
        else
        {
          if (!producerOk)
          {
            PRINT_DATA(mDebugLog, "  - Not a valid producer for area search '%s'!\n", producerName.c_str());
          }
          else
          {
            PRINT_DATA(mDebugLog, "  - Not a valid producer '%s'!\n", producerName.c_str());
          }
        }
      }
      else
      {
        PRINT_DATA(mDebugLog, "  - Producer's '%s' geometry '%d' does not cover all requested areaCoordinates!\n", producerName.c_str(), producerGeometryId);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::getGridValues(
    uchar queryType,
    Producer_vec& producers,
    std::set<T::GeometryId>& geometryIdList,
    uint producerId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    bool acceptNotReadyGenerations,
    const std::string& parameterKey,
    std::size_t parameterHash,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    T::ForecastType forecastType,
    T::ForecastNumber forecastNumber,
    uint queryFlags,
    uint parameterFlags,
    short areaInterpolationMethod,
    short timeInterpolationMethod,
    short levelInterpolationMethod,
    time_t startTime,
    time_t endTime,
    uint timesteps,
    uint timestepSizeInMinutes,
    uchar locationType,
    uchar coordinateType,
    T::AreaCoordinates& areaCoordinates,
    T::ParamValue_vec& contourLowValues,
    T::ParamValue_vec& contourHighValues,
    T::AttributeList& queryAttributeList,
    double radius,
    uint maxValues,
    short& precision,
    ParameterValues_sptr_vec& valueList,
    T::Coordinate_vec& coordinates,
    std::string& producerStr,
    uint recursionCounter)
{
  FUNCTION_TRACE
  try
  {
    if (recursionCounter > 5)
      return;

    //printf("  - producers                : %lu items\n", producers.size());
    //for (auto it = producers.begin(); it != producers.end(); ++it)
    //  printf("    * %s:%d\n", it->first.c_str(), it->second);


    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      PRINT_DATA(mDebugLog, "\nMETHOD getGridValues()\n");
      PRINT_DATA(mDebugLog, "  - queryType                : %u\n", queryType);
      PRINT_DATA(mDebugLog, "  - producers                : %lu items\n", producers.size());
      for (auto it = producers.begin(); it != producers.end(); ++it)
        PRINT_DATA(mDebugLog, "    * %s:%d\n", it->first.c_str(), it->second);
      PRINT_DATA(mDebugLog, "  - geometryIdList           : %lu items\n", geometryIdList.size());
      for (auto it = geometryIdList.begin(); it != geometryIdList.end(); ++it)
        PRINT_DATA(mDebugLog, "    * %d\n",* it);
      PRINT_DATA(mDebugLog, "  - producerId               : %u\n", producerId);
      PRINT_DATA(mDebugLog, "  - analysisTime             : %s\n", analysisTime.c_str());
      PRINT_DATA(mDebugLog, "  - generationFlags          : %llu\n", generationFlags);
      PRINT_DATA(mDebugLog, "  - parameterKey             : %s\n", parameterKey.c_str());
      PRINT_DATA(mDebugLog, "  - paramLevelId             : %d\n", paramLevelId);
      PRINT_DATA(mDebugLog, "  - paramLevel               : %d\n", paramLevel);
      PRINT_DATA(mDebugLog, "  - forecastType             : %d\n", forecastType);
      PRINT_DATA(mDebugLog, "  - forecastNumber           : %d\n", forecastNumber);
      PRINT_DATA(mDebugLog, "  - queryFlags               : %04x\n", queryFlags);
      PRINT_DATA(mDebugLog, "  - parameterFlags           : %04x\n", parameterFlags);
      PRINT_DATA(mDebugLog, "  - areaInterpolationMethod  : %d\n", areaInterpolationMethod);
      PRINT_DATA(mDebugLog, "  - timeInterpolationMethod  : %d\n", timeInterpolationMethod);
      PRINT_DATA(mDebugLog, "  - levelInterpolationMethod : %d\n", levelInterpolationMethod);
      PRINT_DATA(mDebugLog, "  - startTime                : %s\n", utcTimeFromTimeT(startTime).c_str());
      PRINT_DATA(mDebugLog, "  - endTime                  : %s\n", utcTimeFromTimeT(endTime).c_str());
      PRINT_DATA(mDebugLog, "  - timesteps                : %u\n", timesteps);
      PRINT_DATA(mDebugLog, "  - timestepSizeInMinutes    : %u\n", timestepSizeInMinutes);
      PRINT_DATA(mDebugLog, "  - locationType             : %d\n", locationType);
      PRINT_DATA(mDebugLog, "  - coordinateType           : %d\n", coordinateType);
      PRINT_DATA(mDebugLog, "  - areaCoordinates          : %lu vectors\n", areaCoordinates.size());
      PRINT_DATA(mDebugLog, "  - radius                   : %f\n", radius);
      PRINT_DATA(mDebugLog, "  - contourLowValues         : %lu values\n",contourLowValues.size());
      PRINT_DATA(mDebugLog, "  - contourHighValues        : %lu values\n",contourHighValues.size());
      PRINT_DATA(mDebugLog, "  - maxValues                : %u\n\n", maxValues);
      PRINT_DATA(mDebugLog, "  - recursionCounter         : %u\n\n", recursionCounter);
    }

    if (queryType != QueryParameter::Type::GridFile  &&  areaCoordinates.size() == 0  &&  queryAttributeList.getAttributeValue("grid.geometryId") == nullptr  &&  queryAttributeList.getAttributeValue("grid.geometryString") == nullptr  &&  queryAttributeList.getAttributeValue("grid.urn") == nullptr  &&  queryAttributeList.getAttributeValue("grid.crs") == nullptr)
    {
      Fmi::Exception exception(BCP, "No areaCoordinates defined!");
      throw exception;
    }

    if (locationType == QueryParameter::LocationType::Polygon)
    {
      for (size_t t = 0; t < areaCoordinates.size(); t++)
      {
        if (areaCoordinates[t].size() < 3)
        {
          Fmi::Exception exception(BCP, "Area definition requires at least 3 areaCoordinates!");
          throw exception;
        }
      }
    }

    time_t requiredAccessTime = time(nullptr) + 120;

    bool ignoreStartTimeValue = false;
    if ((queryFlags & Query::Flags::StartTimeNotIncluded) != 0)
      ignoreStartTimeValue = true;

    bool reverseGenerations = false;
    if ((queryFlags & Query::Flags::ReverseGenerationFlags) != 0)
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
        // The current producer supports a geometry where the current areaCoordinates can be found.

        bool producerOk = true;
        if (producers.size() > 1 && (locationType == QueryParameter::LocationType::Polygon || locationType == QueryParameter::LocationType::Circle || locationType == QueryParameter::LocationType::Path))
        {
          // We do not accept data concatenation if the data is requested from an area and
          // if geometries are different.

          std::string pStr = toUpperString(producerName) + ":" + std::to_string(producerGeometryId);

          auto rec = mProducerConcatMap.find(pStr);
          if (rec != mProducerConcatMap.end()  &&  rec->second.size() > 0)
          {
            for (auto gg = rec->second.begin(); gg != rec->second.end() && producerOk; ++gg)
            {
              if (producerGeometryId != gg->second)
                producerOk = false;
            }
          }
        }

        T::ProducerInfo producerInfo;
        if (producerOk &&  getProducerInfoByName(producerName, producerInfo) && (producerId == 0 || producerInfo.mProducerId == producerId))
        {
          PRINT_DATA(mDebugLog, "  - The producer and the geometry are acceptable!\n");

          // Reading generations supported by the current producer.

          auto cacheEntry = getGenerationInfoListByProducerId(producerInfo.mProducerId,true);
          uint gLen = cacheEntry->generationInfoList->getLength();

          if (gLen > 0)
          {
            ParameterMapping_vec_sptr mappings;
            if (paramLevelId > 0 || paramLevel > 0)
              getParameterMappings(producerInfo.mName, producerInfo.mProducerId, parameterKey, parameterHash, producerGeometryId, paramLevelId, paramLevel, false, mappings);
            else
              getParameterMappings(producerInfo.mName, producerInfo.mProducerId, parameterKey, parameterHash, producerGeometryId, true, mappings);

            if (mappings->size() > 0)
            {
              for (auto pInfo = mappings->begin(); pInfo != mappings->end(); ++pInfo)
              {
                if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
                {
                  std::stringstream stream;
                  pInfo->print(stream, 5, 0);
                  PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
                }

                std::map<time_t,std::string> contentTimeList;
                T::ContentInfoList contentInfoList;

                T::GenerationInfo* generationInfo = nullptr;
                if (!analysisTime.empty())
                {
                  generationInfo = cacheEntry->generationInfoList->getGenerationInfoByAnalysisTime(analysisTime);
                  if (generationInfo != nullptr)
                  {
                    int result = mContentServerPtr->getContentListByParameterAndGenerationId(0, generationInfo->mGenerationId, pInfo->mParameterKeyType, pInfo->mParameterKey,
                        pInfo->mParameterLevelId, pInfo->mParameterLevel, pInfo->mParameterLevel, forecastType, forecastNumber, producerGeometryId,
                        std::string("19000101T000000"), std::string("30000101T000000"), 0, contentInfoList);

                    if (result != 0)
                    {
                      Fmi::Exception exception(BCP, "ContentServer returns an error!");
                      exception.addParameter("Service", "getContentListByParameterAndGenerationId");
                      exception.addParameter("Message", ContentServer::getResultString(result));
                      throw exception;
                    }
                  }
                }

                if (generationInfo == nullptr)
                {
                  int result = mContentServerPtr->getContentListByParameterAndProducerId(0, producerInfo.mProducerId, pInfo->mParameterKeyType, pInfo->mParameterKey,
                      pInfo->mParameterLevelId, pInfo->mParameterLevel, pInfo->mParameterLevel, forecastType, forecastNumber, producerGeometryId,
                      std::string("19000101T000000"), std::string("30000101T000000"), 0, contentInfoList);

                  if (result != 0)
                  {
                    Fmi::Exception exception(BCP, "ContentServer returns an error!");
                    exception.addParameter("Service", "getContentListByParameterAndProducerId");
                    exception.addParameter("Message", ContentServer::getResultString(result));
                    throw exception;
                  }
                }

                int cLen = contentInfoList.getLength();
                PRINT_DATA(mDebugLog, "         + Found %u content records\n", cLen);


                if (cLen > 0)
                {
                  contentInfoList.sort(T::ContentInfo::ComparisonMethod::starttime_generationId_file_message);
                  T::ContentInfo* cFirst = contentInfoList.getContentInfoByIndex(0);
                  time_t firstTime = 0xFFFFFFFF;
                  time_t lastTime = startTime;

                  uint c = 0;
                  for (int g=gLen-1; g>=0; g--)
                  {
                    ulonglong gFlags = 1 << c;
                    c++;

                    bool generationValid = false;
                    T::GenerationInfo *gInfo = nullptr;
                    T::GenerationInfo *gNext = nullptr;

                    if (generationInfo != nullptr)
                    {
                      gInfo = generationInfo;
                      generationValid = true;
                      g = 0;
                    }
                    else
                    {
                      gInfo = cacheEntry->generationInfoList->getGenerationInfoByIndex(g);
                      gNext = cacheEntry->generationInfoList->getGenerationInfoByIndex(g+1);

                      if (!analysisTime.empty() && generationInfo == nullptr)
                      {
                        if (analysisTime == gInfo->mAnalysisTime)
                        {
                          generationValid = true;
                        }
                        else
                        if ((queryFlags & Query::Flags::AnalysisTimeMatchRequired) == 0)
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
                      }
                      else
                      {
                        if (generationFlags == 0 || (generationFlags & gFlags) != 0)
                        {
                          generationValid = true;
                        }
                      }
                    }

                    if (gInfo == nullptr || (gInfo != nullptr &&  gInfo->mDeletionTime > 0 && gInfo->mDeletionTime < requiredAccessTime))
                      generationValid = false;

                    if (generationValid)
                    {
                      for (int t = cLen-1; t >= 0; t--)
                      {
                        T::ContentInfo* cInfo = contentInfoList.getContentInfoByIndex(t);
                        if (cInfo != nullptr &&  cInfo->mGenerationId == gInfo->mGenerationId)
                        {
                          if (cInfo->mForecastTimeUTC >= startTime  &&  cInfo->mForecastTimeUTC <= endTime  &&  cInfo->mForecastType == cFirst->mForecastType  &&  cInfo->mForecastNumber == cFirst->mForecastNumber)
                          {
                            if (cInfo->mForecastTimeUTC > lastTime)
                            {
                              contentTimeList.clear();
                              lastTime = cInfo->mForecastTimeUTC;
                              firstTime = 0xFFFFFFFF;
                            }

                            if (cInfo->mForecastTimeUTC < firstTime)
                            {
                              contentTimeList.insert(std::pair<time_t,std::string>(cInfo->mForecastTimeUTC,gInfo->mAnalysisTime));
                              firstTime = cInfo->mForecastTimeUTC;
                            }
                          }
                        }
                      }

                      if ((parameterFlags & QueryParameter::Flags::SameAnalysisTime) != 0 || (queryFlags & Query::Flags::SameAnalysisTime) != 0)
                      {
                        // The requested time interval should contain data only from the same generation.
                        if (contentTimeList.size() > 0)
                          g = 0;
                      }
                    }
                  }

                  if (contentTimeList.size() > 0)
                  {
                    Producer_vec producers2;
                    producers2.emplace_back(*it);

                    std::set<T::GeometryId> geometryIdList2;
                    geometryIdList2.insert(producerGeometryId);

                    std::map<time_t,std::string> contentTimeList2;

                    if (timestepSizeInMinutes > 0 && (queryFlags & Query::Flags::StartTimeFromData) != 0  &&  (queryFlags & Query::Flags::EndTimeFromData) == 0)
                    {
                      auto it = contentTimeList.begin();
                      time_t  tt = it->first;
                      time_t  et = tt + timesteps * timestepSizeInMinutes*60;

                      if (timesteps == 0)
                        et = endTime;

                      while (tt <= et)
                      {
                        //std::string ts = utcTimeFromTimeT(tt);
                        contentTimeList2.insert(std::pair<time_t,std::string>(tt,""));
                        tt = tt + timestepSizeInMinutes*60;
                      }
                    }
                    else
                    if (timestepSizeInMinutes > 0 && (queryFlags & Query::Flags::StartTimeFromData) == 0  &&  (queryFlags & Query::Flags::EndTimeFromData) != 0)
                    {
                      auto it = contentTimeList.rbegin();
                      time_t  tt = it->first;

                      time_t  et = tt - timesteps * timestepSizeInMinutes*60;

                      if (timesteps == 0)
                        et = startTime;

                      while (tt >= et)
                      {
                        contentTimeList2.insert(std::pair<time_t,std::string>(tt,""));
                        tt = tt - timestepSizeInMinutes*60;
                      }
                    }
                    else
                    if (timestepSizeInMinutes > 0 && (queryFlags & Query::Flags::StartTimeFromData) != 0  &&  (queryFlags & Query::Flags::EndTimeFromData) != 0)
                    {
                      auto it1 = contentTimeList.begin();
                      time_t tt = it1->first;

                      auto it2 = contentTimeList.rbegin();
                      time_t et = it2->first;

                      while (tt <= et)
                      {
                        contentTimeList2.insert(std::pair<time_t,std::string>(tt,""));
                        tt = tt + timestepSizeInMinutes*60;
                      }
                    }
                    else
                    {
                      for (auto it1 = contentTimeList.begin(); it1 != contentTimeList.end(); ++it1)
                      {
                        contentTimeList2.insert(std::pair<time_t,std::string>(it1->first,it1->second));
                      }
                    }

                    time_t a_lastTime = startTime;

                    for (auto forecastTime = contentTimeList2.begin(); forecastTime != contentTimeList2.end(); ++forecastTime)
                    {
                      if (((forecastTime->first == startTime && !ignoreStartTimeValue) || (forecastTime->first > startTime && forecastTime->first <= endTime)))
                      {
                        std::shared_ptr<ParameterValues> valList = std::make_shared<ParameterValues>();

                        getGridValues(queryType,producers2, geometryIdList2, producerInfo.mProducerId, forecastTime->second, 0, reverseGenerations, acceptNotReadyGenerations, parameterKey, parameterHash, paramLevelId, paramLevel,
                            forecastType, forecastNumber, queryFlags, parameterFlags, areaInterpolationMethod, timeInterpolationMethod, levelInterpolationMethod, forecastTime->first, true,
                            locationType, coordinateType, areaCoordinates, contourLowValues, contourHighValues, queryAttributeList, radius, precision, *valList, coordinates,producerStr);

                        if (valList->mValueList.getLength() > 0 || valList->mValueData.size() > 0 || valList->mValueVector.size() > 0)
                        {
                          if (valList->mForecastTimeUTC == 0)
                            valList->mForecastTimeUTC = forecastTime->first;

                          a_lastTime = valList->mForecastTimeUTC;
                          valueList.emplace_back(valList);
                        }

                        if (valueList.size() == maxValues)
                          return;
                      }
                    }

                    PRINT_DATA(mDebugLog, "    - Value list size = %ld\n\n",valueList.size());

                    if (producers.size() > 1)
                    {
                      auto rec = mProducerConcatMap.find(producerStr);
                      if (rec != mProducerConcatMap.end()  &&  rec->second.size() > 0)
                      {
                        //printf("CONCAT SEARCH %s  %s\n",producerStr.c_str(),rec->second[0].first.c_str());
                        std::set<T::GeometryId> tmpGeomIdList;

                        for (auto gg = rec->second.begin(); gg != rec->second.end(); ++gg)
                        {
                          //printf("  ** SEARCH %ld %d\n",rec->second.size(),gg->second);
                          tmpGeomIdList.insert(gg->second);
                        }

                        producerStr = "";

                        getGridValues(queryType,rec->second, tmpGeomIdList, 0, analysisTime, generationFlags, acceptNotReadyGenerations, parameterKey, parameterHash, paramLevelId, paramLevel, forecastType,
                            forecastNumber,queryFlags,parameterFlags, areaInterpolationMethod, timeInterpolationMethod, levelInterpolationMethod, a_lastTime+1,endTime, timesteps, timestepSizeInMinutes, locationType,
                            coordinateType, areaCoordinates, contourLowValues, contourHighValues, queryAttributeList,radius,maxValues,precision,valueList,coordinates,producerStr,recursionCounter+1);
                      }
                    }
                    return;
                  }
                }
              }
            }
            else
            {
              PRINT_DATA(mDebugLog, "    - No mappings found (%s,%s,%d,%d,%d)!\n",producerInfo.mName.c_str(),parameterKey.c_str(),producerGeometryId,paramLevelId,paramLevel);
            }
          }
          else
          {
            PRINT_DATA(mDebugLog, "    - No generations found for the current producer!\n");
          }
        }
        else
        {
          if (!producerOk)
          {
            PRINT_DATA(mDebugLog, "  - Not a valid producer for area search '%s'!\n", producerName.c_str());
          }
          else
          {
            PRINT_DATA(mDebugLog, "  - Not a valid producer '%s'!\n", producerName.c_str());
          }
        }
      }
      else
      {
        PRINT_DATA(mDebugLog, "  - Producer's '%s' geometry '%d' does not cover all requested areaCoordinates!\n", producerName.c_str(), producerGeometryId);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::getAdditionalValues(
    const std::string& parameterName,
    uchar coordinateType,
    T::Coordinate_vec& coordinates,
    ParameterValues& values)
{
  FUNCTION_TRACE
  try
  {
    std::string param = toLowerString(parameterName);

    auto dt = boost::posix_time::from_time_t(values.mForecastTimeUTC);
    boost::local_time::local_date_time utcTime(dt, tz_utc);

    if (param == "dem")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        if (mDem)
        {
          val.mValue = mDem->elevation(val.mX, val.mY);
          values.mValueList.addGridValue(val);
        }
      }
      return;
    }

    if (param == "covertype")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        if (mLandCover)
        {
          val.mValue = mLandCover->coverType(val.mX, val.mY);
          values.mValueList.addGridValue(val);
        }
      }
      return;
    }

    if (param == "dark")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::solar_position_t sp = Fmi::Astronomy::solar_position(utcTime, val.mX, val.mY);
        val.mValue = (double)sp.dark();
        values.mValueList.addGridValue(val);
      }
      return;

    }

    if (param == "sunelevation")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::solar_position_t sp = Fmi::Astronomy::solar_position(utcTime, val.mX, val.mY);
        val.mValue = (double)sp.elevation;
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "sundeclination")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::solar_position_t sp = Fmi::Astronomy::solar_position(utcTime, val.mX, val.mY);
        val.mValue = (double)sp.declination;
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "sunatzimuth")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::solar_position_t sp = Fmi::Astronomy::solar_position(utcTime, val.mX, val.mY);
        val.mValue = (double)sp.azimuth;
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "moonphase")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        val.mValue = (double)Fmi::Astronomy::moonphase(utcTime.utc_time());
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "moonrise_utc")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        auto lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_iso_string(lt.moonrise.local_time());
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "moonrise2_utc")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);

        if (lt.moonrise2_today())
          val.mValueString = Fmi::to_iso_string(lt.moonrise2.local_time());
        else
          val.mValueString = std::string("");

        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "moonset_utc")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_iso_string(lt.moonset.local_time());
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "moonset2_utc")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);

        if (lt.moonset2_today())
          val.mValueString = Fmi::to_iso_string(lt.moonset2.local_time());
        else
          val.mValueString = "";
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "moonrisetoday_utc")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(lt.moonrise_today());
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "moonrise2today_utc")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(lt.moonrise2_today());
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "moonsettoday_utc")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(lt.moonset_today());
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "moonset2today_utc")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(lt.moonset2_today());
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "moonup24h")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY,true);
        val.mValue = (double)lt.above_horizont_24h();
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "moondown24h")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValue = (double)(!lt.moonrise_today() && !lt.moonset_today() && !lt.above_horizont_24h());
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "sunrise_utc")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_iso_string(st.sunrise.local_time());
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "sunset_utc")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_iso_string(st.sunset.local_time());
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "noon_utc")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_iso_string(st.noon.local_time());
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "sunrisetoday_utc")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(st.sunrise_today());
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "sunsettoday_utc")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(st.sunset_today());
        values.mValueList.addGridValue(val);
      }
      return;
    }

    if (param == "daylength")
    {
      for (auto c = coordinates.begin(); c != coordinates.end(); ++c)
      {
        T::GridValue val;
        val.mX = c->x();
        val.mY = c->y();

        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        auto seconds = st.daylength().total_seconds();
        auto minutes = boost::numeric_cast<long>(round(seconds / 60.0));
        val.mValue = minutes;
        values.mValueList.addGridValue(val);
      }
      return;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





T::ParamValue ServiceImplementation::getAdditionalValue(
    const std::string& parameterName,
    time_t forecastTime,
    double x,
    double y)
{
  FUNCTION_TRACE
  try
  {
    std::string param = toLowerString(parameterName);
    auto dt = boost::posix_time::from_time_t(forecastTime);
    boost::local_time::local_date_time utcTime(dt, tz_utc);

    if (param == "dem")
    {
      if (mDem)
        return mDem->elevation(x,y);

      return ParamValueMissing;
    }

    if (param == "covertype")
    {
      if (mLandCover)
        return mLandCover->coverType(x,y);

      return ParamValueMissing;
    }

    if (param == "dark")
    {
      Fmi::Astronomy::solar_position_t sp = Fmi::Astronomy::solar_position(utcTime, x,y);
      return (double)sp.dark();
    }

    if (param == "sunelevation")
    {
      Fmi::Astronomy::solar_position_t sp = Fmi::Astronomy::solar_position(utcTime, x,y);
      return (double)sp.elevation;
    }

    if (param == "sundeclination")
    {
      Fmi::Astronomy::solar_position_t sp = Fmi::Astronomy::solar_position(utcTime, x,y);
      return (double)sp.declination;
    }

    if (param == "sunatzimuth")
    {
      Fmi::Astronomy::solar_position_t sp = Fmi::Astronomy::solar_position(utcTime, x,y);
      return (double)sp.azimuth;
    }

    if (param == "moonphase")
    {
      return (double)Fmi::Astronomy::moonphase(utcTime.utc_time());
    }

    if (param == "moonup24h")
    {
      Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, x,y);
      return (double)lt.above_horizont_24h();
    }

    if (param == "moondown24h")
    {
      Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, x,y);
      return (double)(!lt.moonrise_today() && !lt.moonset_today() && !lt.above_horizont_24h());
    }

    if (param == "daylength")
    {
      Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, x,y);
      auto seconds = st.daylength().total_seconds();
      auto minutes = boost::numeric_cast<long>(round(seconds / 60.0));
      return (double)minutes;
    }

    return ParamValueMissing;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::convertLevelsToHeights(T::ContentInfoList& contentList,uchar coordinateType,double x,double y,T::ContentInfoList& newContentList)
{
  FUNCTION_TRACE
  try
  {
    uint modificationOperation = 0;
    double_vec modificationParameters;
    PRINT_DATA(mDebugLog, "convertLevelsToHeights()\n");
    uint contentLen = contentList.getLength();
    for (uint t=0; t<contentLen; t++)
    {
      T::ContentInfo* contentInfo = contentList.getContentInfoByIndex(t);

      bool found = false;
      std::string cacheKey = Fmi::to_string(contentInfo->mGenerationId) + ":" + std::string(contentInfo->getForecastTime()) + ":" + Fmi::to_string(contentInfo->mFmiParameterLevelId) + ":" + Fmi::to_string(contentInfo->mParameterLevel) + Fmi::to_string(x) + Fmi::to_string(y);
      if (mLevelHeightCache.size() > 0)
      {
        AutoReadLock lock(&mHeightCache_modificationLock);

        for (auto cc = mLevelHeightCache.rbegin(); cc != mLevelHeightCache.rend(); ++cc)
        {
          if (cc->first == cacheKey)
          {
            T::ContentInfo *newContentInfo = contentInfo->duplicate();
            newContentInfo->mFmiParameterLevelId = 0;
            newContentInfo->mParameterLevel = cc->second;
            newContentList.addContentInfo(newContentInfo);
            found = true;
          }
        }
      }

      if (!found)
      {
        T::ParamKeyType paramKeyType = T::ParamKeyTypeValue::FMI_NAME;
        std::string paramKey = "Z-M2S2";
        T::ParamLevelId levelId = contentInfo->mFmiParameterLevelId;
        T::ParamLevel level = contentInfo->mParameterLevel;

        std::shared_ptr<T::ContentInfoList> cList = std::make_shared<T::ContentInfoList>();

        int result = getContentListByParameterGenerationIdAndForecastTime(0, contentInfo->mProducerId, 0, contentInfo->mGenerationId, paramKeyType, paramKey, 0xFFFFFFFF,
          levelId, level, -1, -1, contentInfo->mGeometryId, contentInfo->mForecastTimeUTC, cList);

        if (result != 0)
        {
          Fmi::Exception exception(BCP, "ContentServer returns an error!");
          exception.addParameter("Service", "getContentListByParameterGenerationIdAndForecastTime");
          exception.addParameter("Message", ContentServer::getResultString(result));
          throw exception;
        }

        if (cList->getLength() == 1)
        {
          T::ContentInfo* cInfo = cList->getContentInfoByIndex(0);
          PRINT_DATA(mDebugLog, "  -- Height was found\n");

          T::ParamValue value = 0;
          int result = mDataServerPtr->getGridValueByPoint(0, cInfo->mFileId, cInfo->mMessageIndex, coordinateType,x,y,T::AreaInterpolationMethod::Linear,modificationOperation,modificationParameters,value);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueByPoint");
            exception.addParameter("Message", DataServer::getResultString(result));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          }
          else
          {
            T::ContentInfo *newContentInfo = contentInfo->duplicate();

            newContentInfo->mFmiParameterLevelId = 0;
            newContentInfo->mParameterLevel = (T::ParamLevel)(value/9.80665);
            newContentList.addContentInfo(newContentInfo);

            AutoWriteLock lock(&mHeightCache_modificationLock);
            if (mLevelHeightCache.size() >= 5000)
              mLevelHeightCache.erase(mLevelHeightCache.begin(), mLevelHeightCache.begin() + 1000);

            mLevelHeightCache.emplace_back(std::pair<std::string,int>(cacheKey,newContentInfo->mParameterLevel));
          }
        }
        else
        {
          T::ParamKeyType paramKeyType = T::ParamKeyTypeValue::FMI_NAME;
          std::string paramKey = "ZH-GPM";
          T::ParamLevelId levelId = contentInfo->mFmiParameterLevelId;
          T::ParamLevel level = contentInfo->mParameterLevel;

          std::shared_ptr<T::ContentInfoList> cList = std::make_shared<T::ContentInfoList>();

          int result = getContentListByParameterGenerationIdAndForecastTime(0, contentInfo->mProducerId, 0, contentInfo->mGenerationId, paramKeyType, paramKey, 0xFFFFFFFE,
            levelId, level, -1, -1, contentInfo->mGeometryId, contentInfo->mForecastTimeUTC, cList);

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "ContentServer returns an error!");
            exception.addParameter("Service", "getContentListByParameterGenerationIdAndForecastTime");
            exception.addParameter("Message", ContentServer::getResultString(result));
            throw exception;
          }

          if (cList->getLength() == 1)
          {
            T::ContentInfo* cInfo = cList->getContentInfoByIndex(0);
            PRINT_DATA(mDebugLog, "  -- Height was found\n");

            T::ParamValue value = 0;
            int result = mDataServerPtr->getGridValueByPoint(0, cInfo->mFileId, cInfo->mMessageIndex, coordinateType,x,y,T::AreaInterpolationMethod::Linear,modificationOperation,modificationParameters,value);
            if (result != 0)
            {
              Fmi::Exception exception(BCP, "DataServer returns an error!");
              exception.addParameter("Service", "getGridValueByPoint");
              exception.addParameter("Message", DataServer::getResultString(result));
              std::string errorMsg = exception.getStackTrace();
              PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            }
            else
            {
              T::ContentInfo *newContentInfo = contentInfo->duplicate();

              newContentInfo->mFmiParameterLevelId = 0;
              newContentInfo->mParameterLevel = (T::ParamLevel)(value);
              newContentList.addContentInfo(newContentInfo);

              AutoWriteLock lock(&mHeightCache_modificationLock);
              if (mLevelHeightCache.size() >= 5000)
                mLevelHeightCache.erase(mLevelHeightCache.begin(), mLevelHeightCache.begin() + 1000);

              mLevelHeightCache.emplace_back(std::pair<std::string,int>(cacheKey,newContentInfo->mParameterLevel));
            }
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::checkProducerMapUpdates()
{
  FUNCTION_TRACE
  try
  {
    time_t currentTime = time(nullptr);
    if ((currentTime - mProducerMap_updateTime) < mProducerMap_checkInterval)
      return;

    T::ProducerInfoList producerInfoList;
    if (mContentServerPtr->getProducerInfoList(0,producerInfoList) == 0)
    {
      Producer_map *prodMap = new Producer_map();
      uint len = producerInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ProducerInfo *pinfo = producerInfoList.getProducerInfoByIndex(t);
        if (pinfo != nullptr)
          prodMap->insert(std::pair<std::string,T::ProducerInfo>(toUpperString(pinfo->mName),T::ProducerInfo(*pinfo)));
      }

      {
        AutoWriteLock lock(&mProducerMap_modificationLock);
        mProducerMap.reset(prodMap);
      }

      mProducerMap_updateTime = time(nullptr);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::checkGenerationUpdates()
{
  FUNCTION_TRACE
  try
  {
    time_t currentTime = time(nullptr);
    if ((currentTime - mGenerationInfoList_checkTime) < mGenerationInfoList_checkInterval)
      return;

    T::GenerationInfoList *generationInfoList = new T::GenerationInfoList();
    if (mContentServerPtr->getGenerationInfoList(0,*generationInfoList) == 0)
    {
      generationInfoList->sort(T::GenerationInfo::ComparisonMethod::producerId);
      {
        AutoWriteLock lock(&mGenerationInfoList_modificationLock);
        mGenerationInfoList.reset(generationInfoList);
      }
      mGenerationInfoList_checkTime = time(nullptr);

      mProducerGenerationListCache_clearRequired = time(nullptr);
    }
    else
    {
      delete generationInfoList;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::checkProducerFileUpdates()
{
  FUNCTION_TRACE
  try
  {
    time_t currentTime = time(nullptr);
    if ((currentTime - mProducerFile_checkTime) < mProducerFile_checkInterval)
      return;

    mProducerFile_checkTime = time(nullptr);
    time_t t1 = getFileModificationTime(mProducerFile.c_str());

    if (mProducerFile_modificationTime != t1 && (mProducerFile_checkTime - t1) > 3)
    {
      loadProducerFile();
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::checkParameterMappingUpdates()
{
  FUNCTION_TRACE
  try
  {
    time_t currentTime = time(nullptr);
    if ((currentTime - mParameterMapping_checkTime) < mParameterMapping_checkInterval)
      return;

    mParameterMapping_checkTime = time(nullptr);
    for (auto it = mParameterMappings.begin(); it != mParameterMappings.end(); ++it)
    {
      if (!mShutdownRequested  &&  it->checkUpdates())
      {
        //AutoWriteLock lock(&mParameterMappingCache_modificationLock);
        mParameterMappingCache_clearRequired = time(nullptr);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::updateProcessing()
{
  try
  {
    mUpdateProcessingActive = true;
    while (!mShutdownRequested)
    {

      try
      {
        T::EventInfo eventInfo;
        int result = mContentServerPtr->getLastEventInfo(0,0,eventInfo);
        if (result == Result::DATA_NOT_FOUND || result == Result::OK)
        {
          if (eventInfo.mServerTime > mContentServerStartTime)
          {
            if (mContentServerStartTime > 0)
            {
              // Content Server has been restarted. We should delete all cached information.

              PRINT_DATA(mDebugLog, "#### Content server restart detected, clearing cached information #######\n");

              mProducerGenerationListCache_clearRequired = time(nullptr);
              mContentSearchCache_records = mContentSearchCache_maxRecords;
              mContentCache_records = mContentCache_maxRecords;

              {
                AutoWriteLock lock(&mHeightCache_modificationLock);
                mLevelHeightCache.clear();
              }

              {
                AutoWriteLock lock(&mProducerHashMap_modificationLock);
                mProducerHashMap.clear();
              }


              mProducerMap_updateTime = 0;
              mGenerationInfoList_checkTime = 0;
            }
          }
          mContentServerStartTime = eventInfo.mServerTime;
        }
      }
      catch (...)
      {
      }

      try
      {
        checkProducerMapUpdates();
      }
      catch (...)
      {
      }

      try
      {
        checkGenerationUpdates();
      }
      catch (...)
      {
      }

      try
      {
        checkProducerFileUpdates();
      }
      catch (...)
      {
      }

      try
      {
        checkParameterMappingUpdates();
      }
      catch (...)
      {
      }

      try
      {
        mLuaFileCollection.checkUpdates(false);
      }
      catch (...)
      {
      }

      try
      {
        mProducerAliasFileCollection.checkUpdates(false);
      }
      catch (...)
      {
      }

      try
      {
        mAliasFileCollection.checkUpdates(false);
      }
      catch (...)
      {
      }

      if (!mShutdownRequested)
        sleep(1);
    }
    mUpdateProcessingActive = false;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP, "Operation failed!", nullptr);
    throw exception;
  }
}





void ServiceImplementation::startUpdateProcessing()
{
  try
  {
    pthread_create(&mThread,nullptr,queryServer_updateThread,this);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP, "Operation failed!", nullptr);
    throw exception;
  }
}

}  // namespace QueryServer
}  // namespace SmartMet
