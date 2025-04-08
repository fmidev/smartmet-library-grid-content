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
#include <macgyver/NumericCast.h>
#include <boost/functional/hash.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <unordered_set>
#include <deque>
#include "../../functions/Function_add.h"
#include "../../functions/Function_acos.h"
#include "../../functions/Function_and.h"
#include "../../functions/Function_asin.h"
#include "../../functions/Function_atan.h"
#include "../../functions/Function_avg.h"
#include "../../functions/Function_change.h"
#include "../../functions/Function_cos.h"
#include "../../functions/Function_div.h"
#include "../../functions/Function_diff.h"
#include "../../functions/Function_eq.h"
#include "../../functions/Function_gt.h"
#include "../../functions/Function_gte.h"
#include "../../functions/Function_hypotenuse.h"
#include "../../functions/Function_if.h"
#include "../../functions/Function_in.h"
#include "../../functions/Function_inCount.h"
#include "../../functions/Function_inPrcnt.h"
#include "../../functions/Function_limit.h"
#include "../../functions/Function_lt.h"
#include "../../functions/Function_lte.h"
#include "../../functions/Function_median.h"
#include "../../functions/Function_min.h"
#include "../../functions/Function_max.h"
#include "../../functions/Function_mode.h"
#include "../../functions/Function_mul.h"
#include "../../functions/Function_multiply.h"
#include "../../functions/Function_not.h"
#include "../../functions/Function_or.h"
#include "../../functions/Function_out.h"
#include "../../functions/Function_outCount.h"
#include "../../functions/Function_outPrcnt.h"
#include "../../functions/Function_round.h"
#include "../../functions/Function_sdev.h"
#include "../../functions/Function_sdevDir.h"
#include "../../functions/Function_sequence.h"
#include "../../functions/Function_sin.h"
#include "../../functions/Function_smedian.h"
#include "../../functions/Function_sub.h"
#include "../../functions/Function_sum.h"
#include "../../functions/Function_tan.h"
#include "../../functions/Function_valid.h"
#include "../../functions/Function_variance.h"
#include "../../functions/Function_windDir.h"
#include "../../functions/Function_vectorU.h"
#include "../../functions/Function_vectorV.h"
#include "../../functions/Function_feelsLike.h"
#include "../../functions/Function_replace.h"


#define FUNCTION_TRACE FUNCTION_TRACE_OFF

namespace SmartMet
{
namespace QueryServer
{

const Fmi::TimeZonePtr& tz_utc = (Fmi::TimeZonePtr::utc);


thread_local static ContentCache mContentCache;
thread_local static std::size_t mContentCache_records;
thread_local static time_t mContentCache_clearRequiredTime;
thread_local static time_t mContentCache_clearTime;
thread_local static bool thread_init = false;

thread_local static ContentSearchCache mContentSearchCache;
thread_local static std::size_t mContentSearchCache_records;
thread_local static time_t mContentSearchCache_clearRequiredTime;
thread_local static time_t mContentSearchCache_clearTime;



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
    mGenerationInfoList_checkInterval = 60;
    mProducerMap_checkInterval = 60;
    mGenerationInfoList_checkTime = 0;
    mProducerMap_updateTime = 0;
    mShutdownRequested = false;
    mContentServerStartTime = 0;
    mContentCache_maxRecords = 0;
    mContentCache_maxRecordsPerThread = 500000;
    mContentCache_stats.size = 0;
    mContentCache_stats.starttime = Fmi::SecondClock::universal_time();
    mContentCache_records = 0;
    mContentSearchCache_maxRecords = 0;
    mContentSearchCache_maxRecordsPerThread = 500000;
    mContentSearchCache_records = 0;
    mContentSearchCache_stats.size = 0;
    mContentSearchCache_stats.starttime = Fmi::SecondClock::universal_time();
    mCheckGeometryStatus = false;
    mParameterMappingCache_clearTime = time(nullptr);
    mProducerGenerationListCache_clearTime  = time(nullptr);
    mProducerGenerationListCache_clearRequired = 0;
    mUpdateProcessingActive = false;
    mDataServerMethodsEnabled = false;
    mContentCache_stats.maxsize = 0;
    mContentSearchCache_stats.maxsize = 0;
    mContentCache_size = 0;
    mContentCache_hits = 0;
    mContentCache_misses = 0;
    mContentCache_inserts = 0;
    mContentCache_clearInterval = 3600 * 24;
    mContentSearchCache_size = 0;
    mContentSearchCache_hits = 0;
    mContentSearchCache_misses = 0;
    mContentSearchCache_inserts = 0;
    mContentSearchCache_clearInterval = 3600 * 24;

    mContentSearchCache_globalClearRequiredTime = 0;
    mContentSearchCache_clearRequiredTime = 0;
    mContentSearchCache_clearTime = 0;

    mContentCache_globalClearRequiredTime = 0;
    mContentCache_clearRequiredTime = 0;
    mContentCache_clearTime = 0;

    mHeightConversionsFile_modificationTime = 0;
    mHeightConversionsFile_checkTime = 0;

    mHeightVecCache.resize(2000);
    mPointHeightCache.resize(10000000);
    mLevelRangeCache.resize(10000);
    mLevelContentCache.resize(10000);

    mRequestCounter = 0;
    mRequestsPerSecond = 0;

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
    const std::string& heightConversionFile,
    string_vec& parameterMappingFiles,
    const std::string& unitConversionFile,
    string_vec& parameterMappingAliasFiles,
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

    mHeightConversionsFile = heightConversionFile;
    loadHeightConversionFile();

    mProducerAliasFileCollection.init(producerAliasFiles, true);

    mAliasFileCollection.init(aliasFiles);
    mLuaFileCollection.init(luaFiles);

    mFunctionCollection.addFunction("K2C",new Functions::Function_add(-273.15));
    Functions::Function_sequence *k2f = new Functions::Function_sequence();
    k2f->addFunction(new Functions::Function_add(-273.15));
    k2f->addFunction(new Functions::Function_multiply(1.8));
    k2f->addFunction(new Functions::Function_add(32.0));
    mFunctionCollection.addFunction("K2F",k2f);

    mFunctionCollection.addFunction("ADD",new Functions::Function_add());       // add const
    mFunctionCollection.addFunction("AND",new Functions::Function_and());
    mFunctionCollection.addFunction("ASIN",new Functions::Function_asin());
    mFunctionCollection.addFunction("ACOS",new Functions::Function_acos());
    mFunctionCollection.addFunction("ATAN",new Functions::Function_atan());
    mFunctionCollection.addFunction("AVG",new Functions::Function_avg());
    mFunctionCollection.addFunction("CHANGE",new Functions::Function_change());
    mFunctionCollection.addFunction("DIFF",new Functions::Function_diff());
    mFunctionCollection.addFunction("DIV",new Functions::Function_div());
    mFunctionCollection.addFunction("EQ",new Functions::Function_eq());
    mFunctionCollection.addFunction("GT",new Functions::Function_gt());
    mFunctionCollection.addFunction("GTE",new Functions::Function_gte());
    mFunctionCollection.addFunction("IF",new Functions::Function_if());
    mFunctionCollection.addFunction("IN",new Functions::Function_in());
    mFunctionCollection.addFunction("IN_COUNT",new Functions::Function_inCount());
    mFunctionCollection.addFunction("IN_PRCNT",new Functions::Function_inPrcnt());
    mFunctionCollection.addFunction("LIMIT",new Functions::Function_limit());
    mFunctionCollection.addFunction("LT",new Functions::Function_lt());
    mFunctionCollection.addFunction("LTE",new Functions::Function_lte());
    mFunctionCollection.addFunction("MAX",new Functions::Function_max());
    mFunctionCollection.addFunction("MEDIAN",new Functions::Function_median());
    mFunctionCollection.addFunction("MIN",new Functions::Function_min());
    mFunctionCollection.addFunction("MODE",new Functions::Function_mode());
    mFunctionCollection.addFunction("MUL",new Functions::Function_mul());
    mFunctionCollection.addFunction("NOT",new Functions::Function_not());
    mFunctionCollection.addFunction("OR",new Functions::Function_or());
    mFunctionCollection.addFunction("OUT",new Functions::Function_out());
    mFunctionCollection.addFunction("ROUND",new Functions::Function_round());
    mFunctionCollection.addFunction("SDEV",new Functions::Function_sdev());
    mFunctionCollection.addFunction("SDEV_DIR",new Functions::Function_sdevDir());
    mFunctionCollection.addFunction("SIN",new Functions::Function_sin());
    mFunctionCollection.addFunction("SMEDIAN",new Functions::Function_smedian());
    mFunctionCollection.addFunction("COS",new Functions::Function_cos());
    mFunctionCollection.addFunction("TAN",new Functions::Function_tan());
    mFunctionCollection.addFunction("VARIANCE",new Functions::Function_variance());

    mFunctionCollection.addFunction("OUT_COUNT",new Functions::Function_outCount());
    mFunctionCollection.addFunction("OUT_PRCNT",new Functions::Function_outPrcnt());
    mFunctionCollection.addFunction("REPLACE",new Functions::Function_replace());

    mFunctionCollection.addFunction("SUM",new Functions::Function_sum());
    mFunctionCollection.addFunction("SUB",new Functions::Function_sub());
    mFunctionCollection.addFunction("VALID",new Functions::Function_valid());


    mFunctionCollection.addFunction("FEELS_LIKE",new Functions::Function_feelsLike());

    // Radians to degrees
    mFunctionCollection.addFunction("RAD2DEG",new Functions::Function_multiply((360.0/2*3.1415926535)));

    // Degrees to radians
    mFunctionCollection.addFunction("DEG2RAD",new Functions::Function_multiply((2*3.1415926535/360.0)));

    mFunctionCollection.addFunction("WIND_SPEED",new Functions::Function_hypotenuse());
    mFunctionCollection.addFunction("WIND_DIR",new Functions::Function_windDir());
    mFunctionCollection.addFunction("WIND_V",new Functions::Function_vectorV());
    mFunctionCollection.addFunction("WIND_U",new Functions::Function_vectorU());


    mUnitConversionFile = unitConversionFile;
    if (!mUnitConversionFile.empty())
      loadUnitConversionFile();

    mParameterMappingFiles = parameterMappingFiles;

    for (auto it = mParameterMappingFiles.begin(); it != mParameterMappingFiles.end(); ++it)
    {
      ParameterMappingFile mapping(*it);
      mParameterMappings.emplace_back(mapping);
    }

    for (auto it = mParameterMappings.begin(); it != mParameterMappings.end(); ++it)
    {
      it->init();

      for (auto itm = parameterMappingAliasFiles.begin(); itm != parameterMappingAliasFiles.end(); ++itm)
      {
        if (*itm == it->getFilename())
        {
          ParameterMapping_vec aliasMappings;
          it->getAliasMappings(aliasMappings,mUnitConversions);

          ParameterMappingFile mapping;
          mapping.setParameterMappings(aliasMappings);
          mParameterAliasMappings.emplace_back(mapping);
        }
      }
    }

    startUpdateProcessing();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::initContentCache(std::size_t maxRecordsPerThread,uint clearIntervalInSeconds)
{
  try
  {
    mContentCache_maxRecordsPerThread = maxRecordsPerThread;
    mContentCache_clearInterval = clearIntervalInSeconds;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP, "Operation failed!", nullptr);
    throw exception;
  }
}




void ServiceImplementation::initContentSearchCache(std::size_t maxRecordsPerThread,uint clearIntervalInSeconds)
{
  try
  {
    mContentSearchCache_maxRecordsPerThread = maxRecordsPerThread;
    mContentSearchCache_clearInterval = clearIntervalInSeconds;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP, "Operation failed!", nullptr);
    throw exception;
  }
}





void ServiceImplementation::getStateAttributes(std::shared_ptr<T::AttributeNode> parent)
{
  try
  {
    auto processing = parent->addAttribute("Procesing Statistics");
    processing->addAttribute("Request Counter",mRequestCounter);
    processing->addAttribute("Requests/Second",mRequestsPerSecond);

    auto cache = parent->addAttribute("Cache Hit Percent");

    char tmp[30];

    double hitPercent1 = 0;
    double hitPercent2 = 0;

    if ((mContentCache_hits + mContentCache_misses))
      hitPercent1 = (double)(100 * mContentCache_hits) / (double)(mContentCache_hits + mContentCache_misses);

    if ((mContentSearchCache_hits + mContentSearchCache_misses))
      hitPercent2 = (double)(100 * mContentSearchCache_hits) / (double)(mContentSearchCache_hits + mContentSearchCache_misses);

    sprintf(tmp,"%.1f",hitPercent1);
    cache->addAttribute("Content cache",tmp);

    sprintf(tmp,"%.1f",hitPercent2);
    cache->addAttribute("Content search cache",tmp);

    ServiceInterface::getStateAttributes(parent);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP, "Operation failed!", nullptr);
    throw exception;
  }
}





void ServiceImplementation::getCacheStats(Fmi::Cache::CacheStatistics& statistics) const
{
  try
  {
    mContentCache_stats.size = mContentCache_size;
    mContentCache_stats.hits = mContentCache_hits;
    mContentCache_stats.misses = mContentCache_misses;
    mContentCache_stats.inserts = mContentCache_inserts;
    mContentCache_stats.maxsize = mContentCache_maxRecords;
    mContentSearchCache_stats.size = mContentSearchCache_size;
    mContentSearchCache_stats.hits = mContentSearchCache_hits;
    mContentSearchCache_stats.misses = mContentSearchCache_misses;
    mContentSearchCache_stats.inserts = mContentSearchCache_inserts;
    mContentSearchCache_stats.maxsize = mContentSearchCache_maxRecords;

    statistics.insert(std::make_pair("Grid::QueryServer::content_cache", mContentCache_stats));
    statistics.insert(std::make_pair("Grid::QueryServer::content_search_cache", mContentSearchCache_stats));
  }
  catch (...)
  {
    Fmi::Exception exception(BCP, "Operation failed!", nullptr);
    throw exception;
  }
}





void ServiceImplementation::setLandCover(std::shared_ptr<Fmi::LandCover> landCover)
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





void ServiceImplementation::setDem(std::shared_ptr<Fmi::DEM> dem)
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





CacheEntry_sptr ServiceImplementation::getGenerationInfoListByProducerId(uint producerId,ulonglong producerHash,bool acceptNotReadyGenerations)
{
  FUNCTION_TRACE
  try
  {
    if (mProducerGenerationListCache_clearRequired > mProducerGenerationListCache_clearTime)
    {
      mProducerGenerationListCache.clear();
      mProducerGenerationListCache_clearTime = time(nullptr);
    }

    uint key = producerId;
    if (acceptNotReadyGenerations)
      key = producerId | 0x80000000;

    auto gl = mProducerGenerationListCache.find(key);
    if (gl != mProducerGenerationListCache.end())
    {
      if (gl->second->producerHash == producerHash)
      {
        // Return valid entry
        return gl->second;
      }
      else
      {
        // Remove old entry
        mProducerGenerationListCache.erase(gl);
      }
    }

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
    cacheEntry->producerHash = producerHash;

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

    query.mProcessingStartTime = time(nullptr);

    mRequestCounter++;

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
    mProducerConcatMap.clear();

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
        while (*p != '\0'  &&  *p != '#'  &&  c < 100)
        {
          if (*p == '"')
            ind = !ind;

          if ((*p == ';' || *p == '\n' || *p == '#') && !ind)
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
          // The field should start from non-empty char
          while (*field[t] != '\0' &&  *field[t] <= ' ')
            field[t]++;

          // There should be no empty chars in the field

          char *f = field[t];
          while (*f != '\0')
          {
            if (*f <= ' ')
              *f = '\0';
            else
              f++;
          }

          //printf("FIELD [%u][%s]\n",t,field[t]);
          if (field[t][0] != '\0')
          {
            std::vector<std::string> parts;
            splitString(field[t],':',parts);
            if (parts.size() == 2)
            {
              if (t == 0)
              {
                key = toUpperString(field[0]);
                mProducerList.emplace_back(toUpperString(parts[0]), toInt32(parts[1]));
              }
              else
              if (t == 1)
              {
                Producer_vec vec;
                vec.emplace_back(toUpperString(parts[0]), toInt32(parts[1]));
                mProducerConcatMap.insert(std::pair<std::string,Producer_vec>(key,vec));
              }
              else
              {
                auto vec = mProducerConcatMap.find(key);
                if (vec != mProducerConcatMap.end())
                  vec->second.emplace_back(toUpperString(parts[0]), toInt32(parts[1]));
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




void ServiceImplementation::loadUnitConversionFile()
{
  FUNCTION_TRACE
  try
  {
    if (mShutdownRequested)
      return;

    FILE* file = fopen(mUnitConversionFile.c_str(), "re");
    if (file == nullptr)
    {
      Fmi::Exception exception(BCP, "Cannot open the unit conversion file!");
      exception.addParameter("Filename", mUnitConversionFile);
      throw exception;
    }

    mUnitConversions.clear();

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

        if (c >= 4)
        {
          UnitConversion rec;

          rec.mSourceUnit = field[0];
          rec.mTargetUnit = field[1];
          rec.mConversionFunction = field[2];
          rec.mReverseConversionFunction = field[3];

          //rec.print(std::cout,0,0);

          mUnitConversions.push_back(rec);
        }
      }
    }
    fclose(file);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







void ServiceImplementation::loadHeightConversionFile()
{
  FUNCTION_TRACE
  try
  {
    if (mShutdownRequested)
      return;

    AutoWriteLock lock(&mHeightConversions_modificationLock);

    if (mHeightConversionsFile_modificationTime == getFileModificationTime(mHeightConversionsFile.c_str()))
      return;

    FILE* file = fopen(mHeightConversionsFile.c_str(), "re");
    if (file == nullptr)
    {
      Fmi::Exception exception(BCP, "Cannot open the height conversion file!");
      exception.addParameter("Filename", mHeightConversionsFile);
      throw exception;
    }

    mHeightConversions.clear();

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

        if (c >= 5)
        {
          HeightConversion rec;

          rec.levelId = toInt32(field[1]);
          rec.heightParameter = field[3];
          rec.multiplier = toFloat(field[4]);

          T::ProducerInfo p1;
          T::ProducerInfo p2;

          std::string producerName1 = field[0];
          std::string producerName2 = field[2];

          if (mContentServerPtr->getProducerInfoByName(0,producerName1,p1) == 0 &&  mContentServerPtr->getProducerInfoByName(0,producerName2,p2) == 0)
          {
            rec.producerId = p1.mProducerId;
            rec.heightProducerId = p2.mProducerId;

            std::size_t hash = 0;
            boost::hash_combine(hash,rec.producerId);
            boost::hash_combine(hash,rec.levelId);

            mHeightConversions.insert(std::pair<std::size_t,HeightConversion>(hash,rec));
          }
        }
      }
    }
    fclose(file);

    mHeightConversionsFile_modificationTime = getFileModificationTime(mHeightConversionsFile.c_str());
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
        coordinates.emplace_back(lon,lat);

        def1->getGridLatLonCoordinatesByGridPoint(x,height-1,lat,lon);
        coordinates.emplace_back(lon,lat);
      }

      for (uint y=0; y<height; y=y+100)
      {
        def1->getGridLatLonCoordinatesByGridPoint(0,y,lat,lon);
        coordinates.emplace_back(lon,lat);

        def1->getGridLatLonCoordinatesByGridPoint(width-1,y,lat,lon);
        coordinates.emplace_back(lon,lat);
      }


      def1->getGridLatLonCoordinatesByGridPoint(width-1,0,lat,lon);
      coordinates.emplace_back(lon,lat);

      def1->getGridLatLonCoordinatesByGridPoint(width-1,height-1,lat,lon);
      coordinates.emplace_back(lon,lat);

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
        coordinates.emplace_back(lon,lat);

        def2->getGridLatLonCoordinatesByGridPoint(x,height-1,lat,lon);
        coordinates.emplace_back(lon,lat);
      }

      for (uint y=0; y<height; y=y+100)
      {
        def2->getGridLatLonCoordinatesByGridPoint(0,y,lat,lon);
        coordinates.emplace_back(lon,lat);

        def2->getGridLatLonCoordinatesByGridPoint(width-1,y,lat,lon);
        coordinates.emplace_back(lon,lat);
      }


      def2->getGridLatLonCoordinatesByGridPoint(width-1,0,lat,lon);
      coordinates.emplace_back(lon,lat);

      def2->getGridLatLonCoordinatesByGridPoint(width-1,height-1,lat,lon);
      coordinates.emplace_back(lon,lat);


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

    std::vector<std::string> pnames;

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
        //mFunctionParamId++;
        //functionParams.emplace_back(std::pair<uint, std::string>(mFunctionParamId, std::string(tmp)));
        pnames.emplace_back(tmp);
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
      //mFunctionParamId++;
      //functionParams.emplace_back(std::pair<uint, std::string>(mFunctionParamId, std::string(tmp)));
      pnames.emplace_back(tmp);
    }

    for (auto it = pnames.begin(); it != pnames.end(); ++it)
    {
      std::string key;
      T::GeometryId geometryId = 0;
      T::ParamLevelId paramLevelId = 0;
      T::ParamLevel paramLevel = 0;
      T::ForecastType forecastType = 1;
      std::vector<T::ParamLevel> paramLevelVec;
      std::vector<T::ForecastNumber> forecastNumberVec;
      std::string producerName;
      uint producerId = 0;
      ulonglong generationFlags = 0;
      short areaInterpolationMethod = 0;
      short timeInterpolationMethod = 0;
      short levelInterpolationMethod = 0;
      uint flags = 0;

      getParameterStringInfo(*it,':',key,geometryId,paramLevelId,paramLevelVec,forecastType,forecastNumberVec,
          producerName,producerId,generationFlags,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,flags);

      size_t lsz = paramLevelVec.size();
      size_t fsz = forecastNumberVec.size();
      if (lsz < 2  &&  fsz < 2)
      {
        mFunctionParamId++;
        functionParams.emplace_back(mFunctionParamId,*it);
      }
      else
      {
        if (lsz < 1)
          paramLevelVec.push_back(paramLevel);

        if (fsz < 1)
          forecastNumberVec.push_back(-1);

        char paramLevelUnit = 0;
        if (flags & QueryServer::QueryParameter::Flags::MetricLevels)
          paramLevelUnit = 'm';

        for (auto lIt = paramLevelVec.begin(); lIt != paramLevelVec.end(); ++lIt)
        {
          paramLevel = *lIt;
          for (auto fIt = forecastNumberVec.begin(); fIt != forecastNumberVec.end(); ++fIt)
          {
            char buf[200];
            if (paramLevelUnit)
              sprintf(buf, "%s:%s:%d:%d:%c%d:%d:%d:%llu", key.c_str(),producerName.c_str(),geometryId,paramLevelId,paramLevelUnit,
                  paramLevel,forecastType,*fIt,generationFlags);
            else
              sprintf(buf, "%s:%s:%d:%d:%d:%d:%d:%llu", key.c_str(),producerName.c_str(),geometryId,paramLevelId,paramLevel,
                  forecastType,*fIt,generationFlags);

            mFunctionParamId++;
            functionParams.emplace_back(mFunctionParamId, std::string(buf));
          }
        }
      }
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

      for (auto m = mParameterAliasMappings.begin(); m != mParameterAliasMappings.end(); ++m)
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
        m->getMappings(producerName, parameterName, geometryId, levelId, level, onlySearchEnabled, *mappings);
      }

      for (auto m = mParameterAliasMappings.begin(); m != mParameterAliasMappings.end(); ++m)
      {
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
    short& levelInterpolationMethod,
    uint& flags)

{
  FUNCTION_TRACE
  try
  {
    std::vector<T::ParamLevel> paramLevelVec;
    std::vector<T::ForecastNumber> forecastNumberVec;
    getParameterStringInfo(param, fieldSeparator, key, geometryId, paramLevelId, paramLevelVec,
        forecastType, forecastNumberVec, producerName, producerId, generationFlags, areaInterpolationMethod,
        timeInterpolationMethod, levelInterpolationMethod,flags);

    if (paramLevelVec.size() > 0)
      paramLevel = paramLevelVec[0];

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
    std::vector<T::ParamLevel>& paramLevelVec,
    T::ForecastType& forecastType,
    std::vector<T::ForecastNumber>& forecastNumberVec,
    std::string& producerName,
    uint& producerId,
    ulonglong& generationFlags,
    short& areaInterpolationMethod,
    short& timeInterpolationMethod,
    short& levelInterpolationMethod,
    uint& flags)

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
      {
        std::vector<std::string> numbers;
        if (isdigit(field[4][0]))
        {
          splitNumbers(field[4],numbers);
        }
        else
        {
          if (toupper(field[4][0]) == 'M')
          {
            flags |= QueryServer::QueryParameter::Flags::MetricLevels;
            splitNumbers(field[4]+1,numbers);
          }
        }
        for (auto n=numbers.begin(); n!=numbers.end(); ++n)
          paramLevelVec.emplace_back(toInt32(*n));
      }
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
        std::vector<std::string> numbers;
        splitNumbers(field[6],numbers);
        for (auto n=numbers.begin(); n!=numbers.end(); ++n)
          forecastNumberVec.emplace_back(toInt16(*n));
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
    Query& query,
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

    const char *contourCoordinateTypeStr = query.mAttributeList.getAttributeValue("contour.coordinateType");

    int contourCoordinateType = 0;
    if (contourCoordinateTypeStr)
      contourCoordinateType = atoi(contourCoordinateTypeStr);


    PRINT_DATA(mDebugLog, "%s   + Parse parameter : %s\n", space(recursionCounter).c_str(), paramStr.c_str());
    std::string functionParamsStr;
    if (getParameterFunctionInfo(paramStr, function, functionParamsStr))
    {
      PRINT_DATA(mDebugLog, "%s     - Function: %s\n", space(recursionCounter).c_str(), function.c_str());
      PRINT_DATA(mDebugLog, "%s     - Params  : %s\n", space(recursionCounter).c_str(), functionParamsStr.c_str());

      getFunctionParams(functionParamsStr, functionParams);

      if (function.substr(0,1) == "/")
      {
        int s1 = 0;
        int s2 = 0;
        queryParam.mTimestepSizeInMinutes = 60;
        queryParam.mFlags |= QueryParameter::Flags::InternalAggregationParameter;

        if (functionParams.size() > 1)
          s1 = toInt32(functionParams[1].second);

        if (functionParams.size() > 2)
          s2 = toInt32(functionParams[2].second);

        if (functionParams.size() > 3)
          queryParam.mTimestepSizeInMinutes = toInt32(functionParams[3].second);

        if (s1 < 0)
          queryParam.mTimestepsBefore = -s1;

        if (s2 > 0)
          queryParam.mTimestepsAfter = s2;
      }

      for (auto fParam = functionParams.begin(); fParam != functionParams.end(); ++fParam)
      {
        if (fParam->second != "0" && toDouble(fParam->second.c_str()) == 0)
        {
          QueryParameter newParam;
          newParam.mId = fParam->first;
          newParam.mParam = fParam->second;
          newParam.mOrigParam = paramStr + " => " + fParam->second;
          newParam.mSymbolicName = fParam->second;
          newParam.mFlags = queryParam.mFlags;

          switch (queryParam.mType)
          {
            case QueryParameter::Type::Isoline:
            case QueryParameter::Type::Isoband:
            case QueryParameter::Type::StreamLine:
              newParam.mType = QueryParameter::Type::Vector;
              newParam.mFlags = queryParam.mFlags | QueryParameter::Flags::ReturnCoordinates;
              if (contourCoordinateType == 3)
                newParam.mFlags |= QueryParameter::Flags::OriginalCoordinates;
              break;

            case QueryParameter::Type::Vector:
            case QueryParameter::Type::PointValues:
            case QueryParameter::Type::GridFile:
            default:
              newParam.mType = queryParam.mType;
              break;
          }

          newParam.mLocationType = queryParam.mLocationType;
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
          //newParam.mFlags = queryParam.mFlags;
          newParam.mTemporary = true;

          std::string alias;
          if (getAlias(fParam->second, alias))
          {
            newParam.mParam = alias;
            newParam.mOrigParam = newParam.mOrigParam + " => " + alias;
            std::string paramName;

            getParameterStringInfo(fParam->second, ':', paramName, newParam.mGeometryId, newParam.mParameterLevelId, newParam.mParameterLevel, newParam.mForecastType, newParam.mForecastNumber,
                newParam.mProducerName,newParam.mProducerId, newParam.mGenerationFlags, newParam.mAreaInterpolationMethod, newParam.mTimeInterpolationMethod, newParam.mLevelInterpolationMethod,newParam.mFlags);
          }

          parseFunction(query,newParam, newParam.mParam, newParam.mFunction, newParam.mFunctionParams, recursionCounter + 2, additionalParameterList);

          additionalParameterList.insert(additionalParameterList.begin(), newParam);
        }
      }
      return true;
    }
    else
    {
      std::string producerName;

      std::string paramName;
      getParameterStringInfo(paramStr, ':', paramName, queryParam.mGeometryId, queryParam.mParameterLevelId, queryParam.mParameterLevel, queryParam.mForecastType, queryParam.mForecastNumber, producerName,
          queryParam.mProducerId, queryParam.mGenerationFlags, queryParam.mAreaInterpolationMethod, queryParam.mTimeInterpolationMethod, queryParam.mLevelInterpolationMethod, queryParam.mFlags);

      std::string alias;
      if (getAlias(paramName, alias))
      {
        if (alias != paramStr)
        {
          if (!parseFunction(query,queryParam, alias, function, functionParams, recursionCounter + 1, additionalParameterList))
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
          producers.emplace_back(it->first, it->second);
        }
        else
        {
          std::string alias;
          if (mProducerAliasFileCollection.getAlias(*pName, alias))
          {
            if (strcasecmp(alias.c_str(), it->first.c_str()) == 0)
            {
              producers.emplace_back(it->first, it->second);
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
        producers.emplace_back(it->first, it->second);
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

        getParameterStringInfo(qParam->mParam, ':', qParam->mParameterKey, qParam->mGeometryId, qParam->mParameterLevelId, qParam->mParameterLevel, qParam->mForecastType, qParam->mForecastNumber, qParam->mProducerName,
            qParam->mProducerId, qParam->mGenerationFlags, qParam->mAreaInterpolationMethod, qParam->mTimeInterpolationMethod, qParam->mLevelInterpolationMethod, qParam->mFlags);

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
          parseFunction(query,*qParam, qParam->mParam, qParam->mFunction, qParam->mFunctionParams, 0, additionalParameterList);
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




void ServiceImplementation::postProcessQuery(Query& query)
{
  FUNCTION_TRACE
  try
  {
    int gridWidth = 0;
    const char *gridWidthStr = query.mAttributeList.getAttributeValue("grid.width");
    if (gridWidthStr != nullptr)
      gridWidth = toInt32(gridWidthStr);

    int gridHeight = 0;
    const char *gridHeightStr = query.mAttributeList.getAttributeValue("grid.height");
    if (gridHeightStr != nullptr)
      gridHeight = toInt32(gridHeightStr);

    short interpolationType = 0;
    const char *interpolationTypeStr = query.mAttributeList.getAttributeValue("contour.interpolation.type");
    if (interpolationTypeStr != nullptr)
      interpolationType = toInt16(interpolationTypeStr);

    size_t smoothSize = 0;
    const char *smoothSizeStr = query.mAttributeList.getAttributeValue("contour.smooth.size");
    if (smoothSizeStr != nullptr)
      smoothSize = toSize_t(smoothSizeStr);

    size_t smoothDegree = 0;
    const char *smoothDegreeStr = query.mAttributeList.getAttributeValue("contour.smooth.degree");
    if (smoothDegreeStr != nullptr)
      smoothDegree = toSize_t(smoothDegreeStr);

    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
    {
      auto sz = qParam->mCoordinates.size();
      if ((!qParam->mFunction.empty() || (qParam->mFlags & QueryServer::QueryParameter::Flags::MetricLevels))  &&  qParam->mCoordinates.size() > 0)
      {
        switch (qParam->mType)
        {
          case QueryParameter::Type::Isoline:
          {
            for (auto pValue = qParam->mValueList.begin(); pValue != qParam->mValueList.end(); ++pValue)
            {
              if ((*pValue)->mValueVector.size() == sz)
              {
                getIsolines(
                    (*pValue)->mValueVector,
                    &qParam->mCoordinates,
                    gridWidth,
                    gridHeight,
                    qParam->mContourLowValues,
                    interpolationType,
                    smoothSize,
                    smoothDegree,
                    (*pValue)->mValueData);
              }
            }
          }
          break;

          case QueryParameter::Type::Isoband:
          {
            for (auto pValue = qParam->mValueList.begin(); pValue != qParam->mValueList.end(); ++pValue)
            {
              if ((*pValue)->mValueVector.size() == sz)
              {
                getIsobands(
                    (*pValue)->mValueVector,
                    &qParam->mCoordinates,
                    gridWidth,
                    gridHeight,
                    qParam->mContourLowValues,
                    qParam->mContourHighValues,
                    interpolationType,
                    smoothSize,
                    smoothDegree,
                    (*pValue)->mValueData);
              }
            }
          }
          break;

          case QueryParameter::Type::StreamLine:
          {

          }
          break;
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::executeQueryFunctions_vector(Query& query)
{
  FUNCTION_TRACE
  try
  {

    //printf("---------------------- BEFORE EXECUTION ------------------\n");
    //query.print(std::cout,0,0);

    const char *gridWidthStr = query.mAttributeList.getAttributeValue("grid.width");
    const char *gridHeightStr = query.mAttributeList.getAttributeValue("grid.height");
    uint columns = toInt32(gridWidthStr);
    uint rows = toInt32(gridHeightStr);

    for (auto qParam = query.mQueryParameterList.rbegin(); qParam != query.mQueryParameterList.rend(); ++qParam)
    {
      std::string function = qParam->mFunction;
      std::string subfunction;
      std::vector<std::string> subVec;
      splitString(function,':',subVec);
      if (subVec.size() == 2)
      {
        function = subVec[0];
        subfunction = subVec[1];
      }
      //printf("FUNCTION [%s][%s][%s]\n",qParam->mFunction.c_str(),function.c_str(),subfunction.c_str());

      if (!function.empty())
      {
        std::string f = function.substr(0, 1);

        int tCount = 0;
        for (auto tt = query.mForecastTimeList.begin(); tt != query.mForecastTimeList.end(); ++tt)
        {
          std::shared_ptr<ParameterValues> pValues = std::make_shared<ParameterValues>();
          pValues->mForecastTimeUTC = *tt;

          std::vector<T::ParamValue_vec> inputVectors;
          std::vector<double> parameters;

          uint producerId = 0;
          uint generationId = 0;
          uint geometryId = 0;
          uint flags = 0;

          for (auto it = qParam->mFunctionParams.begin(); it != qParam->mFunctionParams.end(); ++it)
          {
            double a = toDouble(it->second.c_str());
            QueryParameter* q = query.getQueryParameterPtr(it->first);
            if (q != nullptr)
            {
              if ((std::size_t)tCount < q->mValueList.size())
              {
                if (producerId == 0 && q->mValueList[tCount]->mProducerId > 0)
                {
                  producerId = q->mValueList[tCount]->mProducerId;
                  generationId = q->mValueList[tCount]->mGenerationId;
                  geometryId = q->mValueList[tCount]->mGeometryId;

                  if (q->mValueList[tCount]->mFlags & ParameterValues::Flags::AggregationValue)
                  {
                    flags |= ParameterValues::Flags::AggregationValue;
                  }

                  if (q->mValueList[tCount]->mFlags & ParameterValues::Flags::InternalAggregationValue)
                  {
                    flags |= ParameterValues::Flags::InternalAggregationValue;
                  }
                }

                inputVectors.emplace_back(q->mValueList[tCount]->mValueVector);
                if (q->mCoordinates.size() > 0  &&  qParam->mCoordinates.size() == 0)
                  qParam->mCoordinates = q->mCoordinates;
              }
            }
            else
            {
              parameters.emplace_back(a);
            }
          }

          if (f == "/"  &&  inputVectors.size() > 0)
          {
            pValues->mValueVector = inputVectors[0];
          }
          else
          if (f != "@"  &&  inputVectors.size() > 0)
          {
            //unsigned long long startTime = getTime();

            auto functionPtr = mFunctionCollection.getFunction(function);
            if (functionPtr)
            {
              functionPtr->executeFunctionCall9(columns,rows,inputVectors,parameters,pValues->mValueVector);
            }
            else
            {
              std::string luaFunction;
              uint type = mLuaFileCollection.getFunction(function, luaFunction);
              switch (type)
              {
                case 8:
                  mLuaFileCollection.executeFunctionCall8(luaFunction,subfunction.c_str(),columns,rows,inputVectors,parameters,pValues->mValueVector);
                  break;

                case 9:
                  mLuaFileCollection.executeFunctionCall9(luaFunction,columns,rows,inputVectors,parameters,pValues->mValueVector);
                  break;
              }
            }
            //unsigned long long endTime = getTime();
            //printf("TIME %f\n",(float)(endTime-startTime)/1000000.0);
          }

          pValues->mProducerId = producerId;
          pValues->mGenerationId = generationId;
          pValues->mGeometryId = geometryId;
          pValues->mFlags = flags;
          qParam->mValueList.emplace_back(pValues);
          tCount++;
        }
      }

      if (!qParam->mFunction.empty() > 0  &&  qParam->mFunction.substr(0,1) == "/")
      {
        std::vector<double> parameters;
        std::string function = qParam->mFunction.substr(1);
        std::string subfunction;
        std::vector<std::string> subVec;
        splitString(function,':',subVec);
        if (subVec.size() == 2)
        {
          function = subVec[0];
          subfunction = subVec[1];
        }
        auto functionPtr = mFunctionCollection.getFunction(function);
        uint type = 0;
        std::string luaFunction;
        if (!functionPtr)
          type = mLuaFileCollection.getFunction(function, luaFunction);

        int startIdx = 0;
        int endIdx = 0;

        int fpLen = qParam->mFunctionParams.size();

        if (fpLen > 1)
          startIdx = toInt32(qParam->mFunctionParams[1].second);

        if (fpLen > 2)
          endIdx = toInt32(qParam->mFunctionParams[2].second);

        for (int t=4; t<fpLen; t++)
          parameters.emplace_back(toDouble(qParam->mFunctionParams[t].second));


        int len = qParam->mValueList.size();

        std::vector<std::vector<T::ParamValue>> outputVectors;

        for (int r = 0; r<len; r++)
        {
          T::GridValueList list;

          std::vector<T::ParamValue> outputVector;
          std::vector<std::vector<T::ParamValue>> inputVectors;

          qParam->getValueVectorsByRowRange(r+startIdx,r+endIdx,inputVectors);

          if (inputVectors.size())
          {
            //unsigned long long startTime = getTime();
            if (functionPtr)
            {
              functionPtr->executeFunctionCall9(columns,rows,inputVectors,parameters,outputVector);
            }
            else
            {
              switch (type)
              {
                case 8:
                  mLuaFileCollection.executeFunctionCall8(luaFunction,subfunction.c_str(),columns,rows,inputVectors,parameters,outputVector);
                  break;

                case 9:
                  mLuaFileCollection.executeFunctionCall9(luaFunction,columns,rows,inputVectors,parameters,outputVector);
                  break;
              }
            }
            //unsigned long long endTime = getTime();
            //printf("TIME %f\n",(float)(endTime-startTime)/1000000.0);
          }

          outputVectors.emplace_back(outputVector);
        }

        for (int r = 0; r<len; r++)
        {
          (*qParam->mValueList[r]).mValueVector = outputVectors[r];
          if (query.mAggregationTimes.find((*qParam->mValueList[r]).mForecastTimeUTC) != query.mAggregationTimes.end())
            (*qParam->mValueList[r]).mFlags |= ParameterValues::Flags::InternalAggregationValue;
        }
      }
    }
    query.removeTemporaryParameters();

    //if (containsAggregation)
    {
      query.removeInternalAggregationValues();
      //query.removeAggregationValues();

      //query.removeAggregationTimes();
    }

    query.updateForecastTimeList();

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      query.print(stream, 0, 0);
      PRINT_DATA(mDebugLog, "\nQuery -object before post processing:\n");
      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
    }

    postProcessQuery(query);

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      query.print(stream, 0, 0);
      PRINT_DATA(mDebugLog, "\nQuery -object after post processing:\n");
      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
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
    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      query.print(stream, 0, 0);
      PRINT_DATA(mDebugLog, "\nQuery -object before function execution:\n");
      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
    }


    uint valueCount = query.getValuesPerTimeStep();

    if (valueCount == 0)
    {
      executeQueryFunctions_vector(query);
      return;
    }

    for (auto qParam = query.mQueryParameterList.rbegin(); qParam != query.mQueryParameterList.rend(); ++qParam)
    {
      std::string function = qParam->mFunction;
      if (!function.empty())
      {
        std::string f = function.substr(0, 1);

        int tCount = 0;
        for (auto tt = query.mForecastTimeList.begin(); tt != query.mForecastTimeList.end(); ++tt)
        {
          std::shared_ptr<ParameterValues> pValues = std::make_shared<ParameterValues>();
          pValues->mForecastTimeUTC = *tt;

          std::vector<double> areaParameters;  // If the case of area request, this contain all grid point values.
          std::vector<double> extParameters;

          bool areaCnt = false;
          uint producerId = 0;
          uint generationId = 0;
          uint geometryId = 0;
          uint flags = 0;

          T::GridValue lastRec;
          for (uint v = 0; v < valueCount; v++)
          {
            std::vector<double> parameters;
            std::vector<std::string> strParameters;

            for (auto it = qParam->mFunctionParams.begin(); it != qParam->mFunctionParams.end(); ++it)
            {
              double a = toDouble(it->second.c_str());
              strParameters.push_back(it->second);
              QueryParameter* q = query.getQueryParameterPtr(it->first);
              std::vector<double> valueList;
              if (q != nullptr)
              {
                if ((std::size_t)tCount < q->mValueList.size())
                {
                  if (producerId == 0 && q->mValueList[tCount]->mProducerId > 0)
                  {
                    producerId = q->mValueList[tCount]->mProducerId;
                    generationId = q->mValueList[tCount]->mGenerationId;
                    geometryId = q->mValueList[tCount]->mGeometryId;
                    flags = q->mValueList[tCount]->mFlags;
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
                if (q)
                  areaParameters.emplace_back(a);
                else
                if (v == 0)
                  extParameters.emplace_back(a);
              }
              else
              {
                for (auto aa = valueList.begin(); aa != valueList.end(); ++aa)
                {
                  parameters.emplace_back(*aa);
                  if (q)
                    areaParameters.emplace_back(*aa);
                  else
                  if (v == 0)
                    extParameters.emplace_back(*aa);
                }
              }
            }

            if (f == "/")
            {
              pValues->mFlags = flags;
              if (parameters.size() > 0)
              {
                T::GridValue rec(lastRec.mX, lastRec.mY, parameters[0]);
                pValues->mValueList.addGridValue(rec);
              }
              else
              {
                T::GridValue rec(lastRec.mX, lastRec.mY, ParamValueMissing);
                pValues->mValueList.addGridValue(rec);
              }
            }
            else
            if (f != "@" && !areaCnt)
            {
              // Functions parameter list is a simple value list. If this is an area request
              // then each grid point is calculated separately.

              auto functionPtr = mFunctionCollection.getFunction(function);
              if (functionPtr)
              {
                double val = functionPtr->executeFunctionCall1(parameters);
                T::GridValue rec(lastRec.mX, lastRec.mY, val);
                pValues->mValueList.addGridValue(rec);
              }
              else
              {
                std::string luaFunction;
                uint type = mLuaFileCollection.getFunction(function, luaFunction);
                switch (type)
                {
                  case 1:
                  {
                    double val = mLuaFileCollection.executeFunctionCall1(luaFunction, parameters);
                    T::GridValue rec(lastRec.mX, lastRec.mY, val);
                    pValues->mValueList.addGridValue(rec);
                  }
                  break;

                  case 2:
                  {
                    if (strParameters.size() == 1)
                    {
                      double val = mLuaFileCollection.executeFunctionCall2(luaFunction, strParameters[0].c_str());
                      T::GridValue rec(lastRec.mX, lastRec.mY, val);
                      pValues->mValueList.addGridValue(rec);
                    }
                  }
                  break;

                  case 5:
                  {
                    std::string val = mLuaFileCollection.executeFunctionCall5(luaFunction, query.mLanguage, parameters);
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
          /*

          if (areaCnt && f != "@")
          {
            std::string luaFunction;
            uint type = mLuaFileCollection.getFunction(function, luaFunction);
            switch (type)
            {
              case 1:
              {
                double val = mLuaFileCollection.executeFunctionCall1(luaFunction, extParameters);
                T::GridValue rec(-1000, -1000, val);
                pValues->mValueList.addGridValue(rec);
              }
              break;

              case 5:
              {
                std::string val = mLuaFileCollection.executeFunctionCall5(luaFunction, query.mLanguage, extParameters);
                T::GridValue rec(-1000, -1000, val);
                pValues->mValueList.addGridValue(rec);
              }
              break;
            }

            pValues->mProducerId = producerId;
            pValues->mGenerationId = generationId;
            pValues->mGeometryId = geometryId;
          }
          */

          if (!function.empty()  &&  function.substr(0, 1) == "@")
          {
            // This is an area function call.

            std::string func = function.substr(1);
            for (auto it = areaParameters.begin();it != areaParameters.end();++it)
              extParameters.push_back(*it);

            auto functionPtr = mFunctionCollection.getFunction(func);
            if (functionPtr)
            {
              double val = functionPtr->executeFunctionCall1(extParameters);
              T::GridValue rec(-1000, -1000, val);
              pValues->mValueList.addGridValue(rec);
            }
            else
            {
              std::string luaFunction;
              uint type = mLuaFileCollection.getFunction(func, luaFunction);
              switch (type)
              {
                case 1:
                {
                  double val = mLuaFileCollection.executeFunctionCall1(luaFunction, extParameters);
                  T::GridValue rec(-1000, -1000, val);
                  pValues->mValueList.addGridValue(rec);
                }
                break;

                case 5:
                {
                  std::string val = mLuaFileCollection.executeFunctionCall5(luaFunction, query.mLanguage, extParameters);
                  T::GridValue rec(-1000, -1000, val);
                  pValues->mValueList.addGridValue(rec);
                }
                break;
              }
            }

            pValues->mProducerId = producerId;
            pValues->mGenerationId = generationId;
            pValues->mGeometryId = geometryId;
          }

          if (areaCnt)
          {
            if (pValues->mValueList.getLength() > 1)
            {
              T::GridValue rec;
              pValues->mValueList.getGridValueByIndex(0,rec);
              pValues->mValueList.clear();
              rec.mX = -1000;
              rec.mY = -1000;
              pValues->mValueList.addGridValue(rec);
            }
          }
          qParam->mValueList.emplace_back(pValues);

          tCount++;
        }
      }
      if (!qParam->mFunction.empty() > 0  &&  qParam->mFunction.substr(0,1) == "/")
      {
        std::string function = qParam->mFunction.substr(1);
        auto functionPtr = mFunctionCollection.getFunction(function);
        uint type = 0;
        std::string luaFunction;
        if (!functionPtr)
          type = mLuaFileCollection.getFunction(function, luaFunction);

        int startIdx = 0;
        int endIdx = 0;

        int fpLen = qParam->mFunctionParams.size();
        if (fpLen > 1)
          startIdx = toInt32(qParam->mFunctionParams[1].second);

        if (fpLen > 2)
          endIdx = toInt32(qParam->mFunctionParams[2].second);

        std::vector<T::ParamValue> parameters;
        for (int t=4; t<fpLen; t++)
          parameters.emplace_back(toDouble(qParam->mFunctionParams[t].second));


        int columns = 0;
        int rows = 0;

        std::vector<T::GridValueList> valueList;

        qParam->getValueListSize(columns,rows);

        for (int r = 0; r<rows; r++)
        {
          T::GridValueList list;
          for (int c=0; c<columns; c++)
          {

            T::GridValue *rec = qParam->getValueListRecord(c,r);
            if (rec)
            {
              std::vector<T::ParamValue> values = parameters;
              qParam->getValueListValuesByRowRange(c,r+startIdx,r+endIdx,values);

              if (functionPtr)
              {
                T::GridValue newRec(*rec);
                newRec.mValue = functionPtr->executeFunctionCall1(values);
                list.addGridValue(newRec);
              }
              else
              {
                switch (type)
                {
                  case 1:
                  {
                    T::GridValue newRec(*rec);
                    newRec.mValue = mLuaFileCollection.executeFunctionCall1(luaFunction, values);
                    list.addGridValue(newRec);
                  }
                  break;

                  case 5:
                  {
                    T::GridValue newRec(*rec);
                    newRec.mValueString = mLuaFileCollection.executeFunctionCall5(luaFunction,query.mLanguage,values);
                    list.addGridValue(newRec);
                  }
                  break;
                }
              }
            }
          }
          valueList.emplace_back(list);
        }

        for (int r = 0; r<rows; r++)
        {
          (*qParam->mValueList[r]).mValueList = valueList[r];
          if (query.mAggregationTimes.find((*qParam->mValueList[r]).mForecastTimeUTC) != query.mAggregationTimes.end())
            (*qParam->mValueList[r]).mFlags |= ParameterValues::Flags::InternalAggregationValue;
        }
      }
    }

    query.removeTemporaryParameters();

    //if (containsAggregation)
    {
      //query.removeAggregationValues();
      query.removeInternalAggregationValues();
      //query.removeAggregationTimes();
    }

    query.updateForecastTimeList();

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      query.print(stream, 0, 0);
      PRINT_DATA(mDebugLog, "\nQuery -object after function execution:\n");
      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
    }
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
    bool containsInternalAggregation = false;

    if (producers.size() == 0)
      return Result::NO_PRODUCERS_FOUND;

    // Getting geometries that support support the given coordinates.

    std::set<T::GeometryId> geometryIdList;

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
      if ((query.mProcessingStartTime + query.mMaxProcessingTimeInSeconds) < time(nullptr))
      {
        Fmi::Exception exception(BCP, "Maximum processing time reached - processing halted!");
        exception.addParameter("maxProcessingTimeInSeconds", std::to_string(query.mMaxProcessingTimeInSeconds));
        throw exception;
      }

      // Checking that the parameter is "a data parameter" - not a function.

      if (qParam->mFunction.length() == 0 && ((qParam->mFlags & QueryParameter::Flags::AlternativeParameter) == 0 || alternativeRequired.find(qParam->mId) != alternativeRequired.end()))
      {
        std::string paramName = qParam->mParam;
        std::string producerName = qParam->mProducerName;
        uint producerId = qParam->mProducerId;
        int geometryId = qParam->mGeometryId;
        ulonglong generationFlags = qParam->mGenerationFlags;

        if (qParam->mFlags & QueryParameter::Flags::InternalAggregationParameter)
          containsInternalAggregation = true;

        bool useAlternative = false;

        getParameterStringInfo(qParam->mParam, ':', paramName, geometryId, qParam->mParameterLevelId,
            qParam->mParameterLevel, qParam->mForecastType, qParam->mForecastNumber, producerName,producerId,
            generationFlags,qParam->mAreaInterpolationMethod,qParam->mTimeInterpolationMethod, qParam->mLevelInterpolationMethod, qParam->mFlags);

        if (generationFlags == 0)
          generationFlags = query.mGenerationFlags;

        if (paramName.c_str()[0] == '$')
          paramName = paramName.c_str()+1;

        try
        {
          std::size_t paramHash = 0;
          boost::hash_combine(paramHash,paramName);

          if (qParam->mAlternativeParamId != 0  &&  geometryId > 0 && !isGeometryValid(geometryId,query.mAreaCoordinates))
          {
            useAlternative = true;
            alternativeRequired.insert(qParam->mAlternativeParamId);
          }

          if (qParam->mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN  &&  !useAlternative)
          {
            time_t startTime = query.mStartTime;
            time_t endTime = query.mEndTime;

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
                tmpProducers.emplace_back(producerName, geometryId);
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

            getGridValues(query,*qParam,tmpProducers, geomIdList, producerId, analysisTime, generationFlags, paramName, paramHash, queryFlags, startTime, endTime,producerStr,0,0);

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
                  coordinates.emplace_back(val->mX,val->mY);
                }
              }

              if (len == 0  || ((qParam->mFlags & QueryParameter::Flags::NoReturnValues) == 0  &&  (qParam->mValueList[0]->mFlags & ParameterValues::Flags::DataAvailable) == 0))
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

    std::set<time_t> originalTimeList;
    std::set<time_t> timeList;
    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
    {
      for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end(); ++it)
      {
        if (qParam->mTemporary || ((*it)->mFlags & ParameterValues::Flags::InternalAggregationValue))
        {
        }
        else
        {
          if (originalTimeList.find((*it)->mForecastTimeUTC) == originalTimeList.end())
          {
            originalTimeList.insert((*it)->mForecastTimeUTC);
            // std::cout << "INSERT " << qParam->mParam  << " " << utcTimeFromTimeT((*it)->mForecastTimeUTC) << "\n";
          }
        }

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
            {
              (*it)->mFlags = (*it)->mFlags | QueryServer::ParameterValues::Flags::AggregationValue;

              if (containsInternalAggregation)
                (*it)->mFlags = (*it)->mFlags | QueryServer::ParameterValues::Flags::InternalAggregationValue;
            }

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

            if (originalTimeList.find(*tt) != originalTimeList.end())
            {
              // std::cout << "ADDITIONAL " << qParam->mParam  << " " << utcTimeFromTimeT(*tt) << "\n";
              if (qParam->mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN)
                pValues->mFlags = pValues->mFlags | QueryServer::ParameterValues::Flags::AdditionalValue;
            }
            else
            {
              // std::cout << "AGGREGATION " << qParam->mParam  << " " << utcTimeFromTimeT(*tt) << "\n";
              if (qParam->mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN)
                pValues->mFlags = pValues->mFlags | QueryServer::ParameterValues::Flags::AggregationValue;

              if (containsInternalAggregation)
                pValues->mFlags = pValues->mFlags | QueryServer::ParameterValues::Flags::InternalAggregationValue;
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
    bool containsInternalAggregation = false;

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


    // Parsing parameters and functions in the query.

    updateQueryParameters(query);

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      query.print(stream, 0, 0);
      PRINT_DATA(mDebugLog, "\nQuery -object after parameter update:\n");
      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
    }

    // Fetching parameter data according to the given timesteps and the coordinate list. Notice
    // that the coordinate list can be used in two ways. It can 1) contain coordinate points
    // where the data will be fetched or 2) it can define an area (= polygon) where the grid
    // points are fetched. The area can be defined by using single or multiple polygons.
    // That's why the coordinates are defined as a vector of coordinate vectors.


    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
    {
      if ((query.mProcessingStartTime + query.mMaxProcessingTimeInSeconds) < time(nullptr))
      {
        Fmi::Exception exception(BCP, "Maximum processing time reached - processing halted!");
        exception.addParameter("maxProcessingTimeInSeconds", std::to_string(query.mMaxProcessingTimeInSeconds));
        throw exception;
      }

      if (qParam->mFunction.length() == 0 && ((qParam->mFlags & QueryParameter::Flags::AlternativeParameter) == 0 || alternativeRequired.find(qParam->mId) != alternativeRequired.end()))
      {
        std::string paramName = qParam->mParam;
        std::string producerName = qParam->mProducerName;
        uint producerId = qParam->mProducerId;
        int geometryId = qParam->mGeometryId;
        ulonglong generationFlags = qParam->mGenerationFlags;
        T::ForecastType forecastType = qParam->mForecastType;
        T::ForecastNumber forecastNumber = qParam->mForecastNumber;

        if (qParam->mFlags & QueryParameter::Flags::InternalAggregationParameter)
          containsInternalAggregation = true;

        bool useAlternative = false;

        getParameterStringInfo(qParam->mParam, ':', paramName, geometryId, qParam->mParameterLevelId,
            qParam->mParameterLevel, qParam->mForecastType, qParam->mForecastNumber, producerName,producerId,
            generationFlags,qParam->mAreaInterpolationMethod,qParam->mTimeInterpolationMethod, qParam->mLevelInterpolationMethod, qParam->mFlags);

        if (generationFlags == 0)
          generationFlags = query.mGenerationFlags;

        if (paramName.c_str()[0] == '$')
          paramName = paramName.c_str()+1;

        std::size_t paramHash = 0;
        boost::hash_combine(paramHash,paramName);

        if (qParam->mAlternativeParamId != 0  &&  geometryId > 0 && !isGeometryValid(geometryId,query.mAreaCoordinates))
        {
          useAlternative = true;
          alternativeRequired.insert(qParam->mAlternativeParamId);
        }

        std::set<time_t> *forecastTimeList = &query.mForecastTimeList;
        std::set<time_t> forecastTimeList2;

        if (qParam->mFlags & QueryParameter::Flags::InternalAggregationParameter)
          containsInternalAggregation = true;

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
                if (query.mForecastTimeList.find(ss) == query.mForecastTimeList.end())
                  query.mAggregationTimes.insert(ss);
              }
            }

            if (qParam->mTimestepsAfter > 0)
            {
              time_t ss = fTime;

              for (uint t = 0; t < qParam->mTimestepsAfter; t++)
              {
                ss = ss + (qParam->mTimestepSizeInMinutes * 60);
                forecastTimeList2.insert(ss);
                if (query.mForecastTimeList.find(ss) == query.mForecastTimeList.end())
                  query.mAggregationTimes.insert(ss);
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
            if (qParam->mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN  &&  !useAlternative)
            {
              Producer_vec *producersPtr = &producers;
              std::set<T::GeometryId> *geomIdListPtr = &geometryIdList;

              if (producerName > " ")
              {
                if (tmpProducers.size() == 0)
                {
                  if (geometryId > 0)
                    tmpProducers.emplace_back(producerName, geometryId);
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
              std::string maxAnalysisTime;

              //printf("SEARCH %s %u\n",producerStr.c_str(),producerId);

              getGridValues(query,*qParam,*producersPtr, *geomIdListPtr, producerId, analysisTime, maxAnalysisTime, gflags, paramName, paramHash,
                  queryFlags, *fTime, false, *valueList, producerStr);

              //printf("SEARCH RESULT %s %u %u %u\n",producerStr.c_str(),producerId,valueList->mValueList.getLength(),valueList->mFlags);
              bool concat = false;
              if (producerId == 0 && (valueList->mValueList.getLength() == 0 && (valueList->mFlags & ParameterValues::Flags::DataAvailable) == 0))
              {
                std::string prodName = producerStr;
                auto p1 = producersPtr;
                auto p2 = geomIdListPtr;

                while (!prodName.empty() &&  valueList->mValueList.getLength() == 0 && (valueList->mFlags & ParameterValues::Flags::DataAvailable) == 0)
                {
                  //printf("CONCAT SEARCH %s\n",prodName.c_str());
                  auto vec = mProducerConcatMap.find(prodName);
                  if (vec != mProducerConcatMap.end()  &&  vec->second.size() > 0)
                  {
                    int gid = 0;
                    const char *gidStr = strstr(prodName.c_str(),":");
                    if (gidStr)
                      gid = toInt32(gidStr+1);

                    tmpGeomIdList.clear();
                    prodName = vec->second[0].first + ":" + std::to_string(vec->second[0].second);

                    bool prodOk = true;
                    if (qParam->mLocationType == QueryParameter::LocationType::Polygon || qParam->mLocationType == QueryParameter::LocationType::Circle || qParam->mLocationType == QueryParameter::LocationType::Path)
                    {
                      prodOk = false;
                      for (auto gg = vec->second.begin(); gg != vec->second.end() && !prodOk; ++gg)
                      {
                        if (gid == gg->second)
                        {
                          prodOk = true;
                          tmpGeomIdList.insert(gid);
                        }
                      }
                    }
                    else
                    {
                      for (auto gg = vec->second.begin(); gg != vec->second.end(); ++gg)
                        tmpGeomIdList.insert(gg->second);
                    }


                    if (prodOk)
                    {
                      producersPtr = &(vec->second);
                      geomIdListPtr = &tmpGeomIdList;
                      std::string maxAnalysisTime;
                      getGridValues(query,*qParam,*producersPtr, *geomIdListPtr, producerId, analysisTime, maxAnalysisTime, gflags, paramName, paramHash,
                          queryFlags, *fTime, false, *valueList, producerStr);

                      concat = true;
                    }
                  }
                  else
                    prodName = "";
                }
                tmpGeomIdList.clear();
                producersPtr = p1;
                geomIdListPtr = p2;
              }

              if (valueList->mValueList.getLength() == 0  || ((qParam->mFlags & QueryParameter::Flags::NoReturnValues) == 0  &&  (valueList->mFlags & ParameterValues::Flags::DataAvailable) == 0))
              {
                alternativeRequired.insert(qParam->mAlternativeParamId);
              }

              //printf("SEARCH END %s %u %u %d\n",producerStr.c_str(),producerId,valueList->mValueList.getLength(),(int)concat);

              if (valueList->mValueList.getLength() > 0  || ((qParam->mFlags & QueryParameter::Flags::NoReturnValues) != 0  &&  (valueList->mFlags & ParameterValues::Flags::DataAvailable) != 0))
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

                  if (qParam->mTimeInterpolationMethod == T::TimeInterpolationMethod::Forbidden || (qParam->mFlags & QueryParameter::Flags::SameAnalysisTime) != 0 || (query.mFlags & Query::Flags::SameAnalysisTime) != 0 )
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
                    coordinates.emplace_back(val->mX,val->mY);
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

          //if (query.mForecastTimeList.find(*fTime) == query.mForecastTimeList.end())
          //  valueList->mFlags = valueList->mFlags | QueryServer::ParameterValues::Flags::AggregationValue;
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

        if (query.mForecastTimeList.find((*it)->mForecastTimeUTC) == query.mForecastTimeList.end())
        {
          (*it)->mFlags |= QueryServer::ParameterValues::Flags::AggregationValue;

          if (containsInternalAggregation)
            (*it)->mFlags |= QueryServer::ParameterValues::Flags::InternalAggregationValue;

          additionalTimeList.insert((*it)->mForecastTimeUTC);
        }
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
            {
              pValues->mFlags = pValues->mFlags | QueryServer::ParameterValues::Flags::AdditionalValue;
            }

            getAdditionalValues(qParam->mSymbolicName,query.mCoordinateType,coordinates,*pValues);

            if (additionalTimeList.find(tt) != additionalTimeList.end())
            {
              pValues->mFlags = pValues->mFlags | QueryServer::ParameterValues::Flags::AggregationValue;
              if (containsInternalAggregation)
                pValues->mFlags = pValues->mFlags | QueryServer::ParameterValues::Flags::InternalAggregationValue;
            }

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
    std::vector<double> parameters;
    int fps = functionParams.size();
    parameters.reserve(fps);
    for (int p = 0; p<fps; p++)
      parameters.emplace_back(toDouble(functionParams[p].c_str()));

    if (functionPtr)
    {
      for (int t=0; t<size; t++)
      {
        if (valueList[t] == ParamValueMissing)
        {
          newValueList.emplace_back(ParamValueMissing);
        }
        else
        {
          for (int p = 0; p<fps; p++)
          {
            const char *f = functionParams[p].c_str();

            if (f[0] == '$')
            {
              if (f[1] == '\0')
              {
                parameters[p] = valueList[t];
              }
              else
              {
                T::ParamValue val = getAdditionalValue(f+1,forecastTime,coordinates[t].x(), coordinates[t].y());
                parameters[p] = val;
              }
            }
          }

          T::ParamValue newValue = functionPtr->executeFunctionCall1(parameters);
          newValueList.emplace_back(newValue);
        }
      }
      return;
    }

    for (int t=0; t<size; t++)
    {
      if (valueList[t] == ParamValueMissing)
      {
        newValueList.emplace_back(ParamValueMissing);
      }
      else
      {
        for (int p = 0; p<fps; p++)
        {
          const char *f = functionParams[p].c_str();

          if (f[0] == '$')
          {
            if (f[1] == '\0')
            {
              parameters[p] = valueList[t];
            }
            else
            {
              T::ParamValue val = getAdditionalValue(f+1,forecastTime,coordinates[t].x(), coordinates[t].y());
              parameters[p] = val;
            }
          }
        }

        T::ParamValue newValue = mLuaFileCollection.executeFunctionCall1(function, parameters);
        newValueList.emplace_back(newValue);
      }
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
#if 0
    auto functionPtr = mFunctionCollection.getFunction(function);
    int size = valueList.size();
    newValueList.reserve(size);
    for (int t=0; t<size; t++)
    {
      if (valueList[t] == ParamValueMissing)
      {
        newValueList.emplace_back(ParamValueMissing);
      }
      else
      {
        std::vector<double> parameters;
        parameters.reserve(functionParams.size());
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
#endif

    auto functionPtr = mFunctionCollection.getFunction(function);

    int size = valueList.size();
    newValueList.reserve(size);
    std::vector<double> parameters;
    int fps = functionParams.size();
    parameters.reserve(fps);
    for (int p = 0; p<fps; p++)
      parameters.emplace_back(toDouble(functionParams[p].c_str()));

    if (functionPtr)
    {
      for (int t=0; t<size; t++)
      {
        if (valueList[t] == ParamValueMissing)
        {
          newValueList.emplace_back(ParamValueMissing);
        }
        else
        {
          for (int p = 0; p<fps; p++)
          {
            const char *f = functionParams[p].c_str();
            if (f[0] == '$'  &&  f[1] == '\0')
              parameters[p] = valueList[t];
          }

          T::ParamValue newValue = functionPtr->executeFunctionCall1(parameters);
          newValueList.emplace_back(newValue);
        }
      }
      return;
    }

    for (int t=0; t<size; t++)
    {
      if (valueList[t] == ParamValueMissing)
      {
        newValueList.emplace_back(ParamValueMissing);
      }
      else
      {
        for (int p = 0; p<fps; p++)
        {
          const char *f = functionParams[p].c_str();
          if (f[0] == '$'  &&  f[1] == '\0')
            parameters[p] = valueList[t];
        }

        T::ParamValue newValue = mLuaFileCollection.executeFunctionCall1(function, parameters);
        newValueList.emplace_back(newValue);
      }
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
    Fmi::LocalDateTime utcTime(toTimeStamp(utcTimeFromTimeT(forecastTime)), nullptr);
    auto functionPtr = mFunctionCollection.getFunction(function);

    uint vLen = valueList.getLength();
    for (uint i = 0; i < vLen; i++)
    {
      auto gv = valueList.getGridValuePtrByIndex(i);
      if (gv != nullptr  &&  gv->mValue != ParamValueMissing)
      {
        std::vector<double> parameters;
        parameters.reserve(functionParams.size());
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

    if (!thread_init)
    {
      thread_init = true;
      mContentCache_maxRecords += mContentCache_maxRecordsPerThread;
      mContentSearchCache_maxRecords += mContentSearchCache_maxRecordsPerThread;
    }

    time_t currentTime = time(nullptr);
    if ((mContentCache_clearTime + mContentCache_clearInterval) < currentTime)
      mContentCache_clearRequiredTime = currentTime;

    if ((mContentSearchCache_clearTime + mContentSearchCache_clearInterval) < currentTime)
      mContentSearchCache_clearRequiredTime = currentTime;

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
    {
      //producerHash = getProducerHash(producerId);
      printf("%s:%d:%s: The producer hash should be never zero!\n",BCP);
    }

    // ### Checking if we have the result for this search already in the cache

    auto it = mContentSearchCache.find(hash2);
    if (it != mContentSearchCache.end())
    {
      mContentSearchCache_hits++;
      if (it->second.producerHash == producerHash)
      {
        // The result was found from the cache

        contentInfoList = it->second.contentInfoList;
        return Result::OK;
      }
    }
    else
    {
      mContentSearchCache_misses++;
    }


    // ### Checking if we have already cached the required content information.

    ContentCacheEntry_sptr entry;
    bool newEntry = false;

    auto cc = mContentCache.find(hash);
    auto cc3 = cc;

    if (cc != mContentCache.end())
    {
      // The required content information is was found from the cache

      mContentCache_hits++;
      entry = cc->second;
    }
    else
    {
      mContentCache_misses++;

      // Trying to find the required content with another hash value

      cc3 = mContentCache.find(hash3);
      if (cc3 != mContentCache.end())
      {
        // The required content information was found from the cache

        mContentCache_hits++;
        entry = cc3->second;
        hash = hash3;
      }
      else
      {
        // No cache entry available. We have to fetch the required content
        // information from the contentServer.

        mContentCache_misses++;

        entry.reset(new ContentCacheEntry());
        newEntry = true;
      }
    }


    if (newEntry || entry->producerHash != producerHash)
    {
      // The content entry in the cache seems to be old or missing. That's why we should update this entry.

      time_t startTime = 0;
      time_t endTime = 0xFFFFFFFF;

      mContentServerPtr->getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,level,level,forecastType,forecastNumber,geometryId,startTime,endTime,0,entry->contentInfoList);
      if (entry->contentInfoList.getLength() == 0)
      {
        // It seems that the content information on the required level is not available. Let's ignore
        // the level requirement.

        mContentServerPtr->getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,0,1000000000,forecastType,forecastNumber,geometryId,startTime,endTime,0,entry->contentInfoList);
        if (entry->contentInfoList.getLength() == 0)
          hash = hash3;
      }

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


    // ### Now we should have the required content information ready for the search.

    std::shared_ptr<T::ContentInfoList> cList = std::make_shared<T::ContentInfoList>();
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

    if (mContentSearchCache_clearTime < mContentSearchCache_clearRequiredTime ||
        mContentSearchCache_clearTime <  mContentSearchCache_globalClearRequiredTime ||
        mContentSearchCache_records >= mContentSearchCache_maxRecordsPerThread)
    {
      //printf("CLEAR CONTENT SEARCH CACHE %ld %ld %ld\n",mContentSearchCache_records,mContentSearchCache_maxRecords,(std::size_t)mContentSearchCache_size);
      mContentSearchCache_size -= mContentSearchCache_records;
      mContentSearchCache.clear();
      mContentSearchCache_records = 0;
      mContentSearchCache_clearTime = time(nullptr);
      mContentSearchCache_stats.starttime = Fmi::SecondClock::universal_time();
      mContentSearchCache_hits = 0;
      mContentSearchCache_misses = 0;
      mContentSearchCache_inserts = 0;
    }

    auto rr = mContentSearchCache.find(hash2);
    if (rr == mContentSearchCache.end())
    {
      // Current search result is new. We should add in into the cache.

      mContentSearchCache_misses++;
      ContentSearchCacheEntry rc;
      rc.contentInfoList = cList;
      rc.producerHash = producerHash;
      rc.generationId = generationId;
      mContentSearchCache.insert(std::pair<std::size_t,ContentSearchCacheEntry>(hash2,rc));

      auto len = cList->getLength();
      if (!len)
        len = 1;

      mContentSearchCache_records += len;
      mContentSearchCache_inserts++;
      mContentSearchCache_size += len;
    }
    else
    {
      // Current search result is already in use, but it has old information. We should update this information.
      mContentSearchCache_hits++;

      rr->second.contentInfoList = cList;
      rr->second.producerHash = producerHash;
      rr->second.generationId = generationId;
    }

    if (newEntry)
    {
      if (mContentCache_clearTime < mContentCache_clearRequiredTime ||
          mContentCache_clearTime < mContentCache_globalClearRequiredTime ||
          mContentCache_records > mContentCache_maxRecordsPerThread)
      {
        //printf("CLEAR CONTENT CACHE %ld %ld %ld\n",mContentCache_records,mContentCache_maxRecordsPerThread,(std::size_t)mContentCache_size);
        mContentCache_size -= mContentCache_records;
        mContentCache.clear();
        mContentCache_hits = 0;
        mContentCache_misses = 0;
        mContentCache_inserts = 0;
        mContentCache_records = 0;
        mContentCache_clearTime = time(nullptr);
        mContentCache_stats.starttime = Fmi::SecondClock::universal_time();

        mLevelContentCache.clear();
      }

      if (mContentCache.find(hash) == mContentCache.end())
      {
        mContentCache.insert(std::pair<std::size_t,ContentCacheEntry_sptr>(hash,entry));

        auto len = entry->contentInfoList.getLength();
        if (!len)
          len = 1;

        mContentCache_records += len;
        mContentCache_inserts++;
        mContentCache_size += len;
      }
      else
      {
        printf("%s:%d:%s: The code should never reach this point!\n",BCP);
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getSpecialValues(
    Query& query,
    QueryParameter& qParam,
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
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex", std::to_string(contentInfo1->mMessageIndex));
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

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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
          exception.addParameter("Service", "getGridValueVectorByPoint");
          exception.addParameter("ErrorMessage", DataServer::getResultString(result1));
          exception.addParameter("FileId", std::to_string(contentInfo1->mFileId));
          exception.addParameter("MessageIndex", std::to_string(contentInfo1->mMessageIndex));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }

        int result2 = mDataServerPtr->getGridValueVectorByPoint(0, contentInfo2->mFileId, contentInfo2->mMessageIndex, coordinateType, x, y, areaInterpolationMethod,modificationOperation,modificationParameters, valueVector2);
        if (result2 != 0)
        {
          Fmi::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueVectorByPoint");
          exception.addParameter("ErrorMessage", DataServer::getResultString(result2));
          exception.addParameter("FileId", std::to_string(contentInfo2->mFileId));
          exception.addParameter("MessageIndex", std::to_string(contentInfo2->mMessageIndex));
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

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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
    Query& query,
    QueryParameter& qParam,
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    uint& newProducerId,
    ParameterValues& valueList,
    T::Coordinate_vec& coordinates)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getValueVectors()\n");

    if ((qParam.mFlags & QueryServer::QueryParameter::Flags::MetricLevels)  &&  qParam.mLevelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
    {
      switch (qParam.mLocationType)
      {
        case QueryParameter::LocationType::Geometry:
        {
          uint width = 0;
          uint height = 0;
          T::Coordinate_svec sCoordinates(new T::Coordinate_vec());

          Identification::gridDef.getGridLatLonCoordinatesByGeometry(query.mAttributeList,sCoordinates,width,height);

          if (sCoordinates)
          {
            bool res = getValueVectorsByHeight(query,query.mCoordinateType,*sCoordinates,qParam,producerInfo,producerGeometryId,generationId,
              analysisTime,generationFlags,pInfo,forecastTime,paramLevelId,paramLevel,newProducerId,valueList);

          //if (((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0))
            coordinates = *sCoordinates;

            return res;
          }
          else
          {
            return false;
          }
        }
        break;

        case QueryParameter::LocationType::Grid:
        {
          bool res = getValueVectorsByHeight(query,query.mCoordinateType,query.mAreaCoordinates[0],qParam,producerInfo,producerGeometryId,generationId,
            analysisTime,generationFlags,pInfo,forecastTime,paramLevelId,paramLevel,newProducerId,valueList);

          coordinates = query.mAreaCoordinates[0];
          return res;

        }
        break;
      }
    }


    short areaInterpolationMethod = qParam.mAreaInterpolationMethod;
    if (areaInterpolationMethod == T::AreaInterpolationMethod::Undefined)
      areaInterpolationMethod = pInfo.mAreaInterpolationMethod;

    short timeInterpolationMethod = qParam.mTimeInterpolationMethod;
    if (timeInterpolationMethod == T::TimeInterpolationMethod::Undefined)
      timeInterpolationMethod = pInfo.mTimeInterpolationMethod;

    //short levelInterpolationMethod = qParam.mLevelInterpolationMethod;
    //if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
    //  levelInterpolationMethod = pInfo.mLevelInterpolationMethod;

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
        paramLevelId,paramLevel,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList);

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

    query.mAttributeList.setAttribute("grid.timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
    query.mAttributeList.setAttribute("grid.areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));
    //query.mAttributeList.setAttribute("grid.levelInterpolationMethod",Fmi::to_string(levelInterpolationMethod));

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC == fTime)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        valueList.mFlags = ParameterValues::Flags::DataAvailable;

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;

          switch (qParam.mLocationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridValueVectorByGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,query.mAttributeList,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  uint width = 0;
                  uint height = 0;
                  T::Coordinate_svec sCoordinates(new T::Coordinate_vec());

                  if (qParam.mFlags & QueryServer::QueryParameter::Flags::OriginalCoordinates)
                  {
                    T::Coordinate_svec latlonCoordinates(new T::Coordinate_vec());
                    Identification::gridDef.getGridOriginalCoordinatesByGeometry(query.mAttributeList,latlonCoordinates,3,sCoordinates,width,height);
                  }
                  else
                  {
                    Identification::gridDef.getGridLatLonCoordinatesByGeometry(query.mAttributeList,sCoordinates,width,height);
                  }
                  if (sCoordinates)
                    coordinates = *sCoordinates;
                }
              }
              break;

            case QueryParameter::LocationType::Grid:
              result = mDataServerPtr->getGridValueVectorByCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,query.mCoordinateType,query.mAreaCoordinates[0],areaInterpolationMethod,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  coordinates = query.mAreaCoordinates[0];
                }
              }
              break;

            default:
              result = mDataServerPtr->getGridValueVector(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
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
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex", std::to_string(contentInfo1->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
             executeConversion(function, functionParams, forecastTime, coordinates, valueVector, valueList.mValueVector);
          else
            valueList.mValueVector = valueVector;
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

        if ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) == 0)
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

    short levelInterpolationMethod = getLevelInterpolationMethod(paramLevelId,qParam.mLevelInterpolationMethod,qParam.mFlags);

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;

          switch (qParam.mLocationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridValueVectorByTimeAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,query.mAttributeList,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  uint width = 0;
                  uint height = 0;
                  T::Coordinate_svec sCoordinates(new T::Coordinate_vec());
                  if (qParam.mFlags & QueryServer::QueryParameter::Flags::OriginalCoordinates)
                  {
                    T::Coordinate_svec latlonCoordinates(new T::Coordinate_vec());
                    Identification::gridDef.getGridOriginalCoordinatesByGeometry(query.mAttributeList,latlonCoordinates,3,sCoordinates,width,height);
                  }
                  else
                  {
                    Identification::gridDef.getGridLatLonCoordinatesByGeometry(query.mAttributeList,sCoordinates,width,height);
                  }
                  if (sCoordinates)
                    coordinates = *sCoordinates;
                }
              }
              break;

            case QueryParameter::LocationType::Grid:
              result = mDataServerPtr->getGridValueVectorByTimeAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,query.mCoordinateType,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  coordinates = query.mAreaCoordinates[0];
                }
              }
              break;

            default:
              result = mDataServerPtr->getGridValueVectorByTime(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,timeInterpolationMethod,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
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
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex", std::to_string(contentInfo1->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
             executeConversion(function, functionParams, forecastTime, coordinates, valueVector, valueList.mValueVector);
          else
            valueList.mValueVector = valueVector;
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

        if ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) == 0)
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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          valueList.mParameterLevel = paramLevel;

          int result = 0;

          switch (qParam.mLocationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridValueVectorByLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,query.mAttributeList,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  uint width = 0;
                  uint height = 0;
                  T::Coordinate_svec sCoordinates(new T::Coordinate_vec());
                  if (qParam.mFlags & QueryServer::QueryParameter::Flags::OriginalCoordinates)
                  {
                    T::Coordinate_svec latlonCoordinates(new T::Coordinate_vec());
                    Identification::gridDef.getGridOriginalCoordinatesByGeometry(query.mAttributeList,latlonCoordinates,3,sCoordinates,width,height);
                  }
                  else
                  {
                    Identification::gridDef.getGridLatLonCoordinatesByGeometry(query.mAttributeList,sCoordinates,width,height);
                  }
                  if (sCoordinates)
                    coordinates = *sCoordinates;
                }
              }
              break;

            case QueryParameter::LocationType::Grid:
              result = mDataServerPtr->getGridValueVectorByLevelAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,query.mCoordinateType,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  coordinates = query.mAreaCoordinates[0];
                }
              }
              break;

            default:
              result = mDataServerPtr->getGridValueVectorByLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,levelInterpolationMethod,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
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

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

        if ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) == 0)
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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;

          switch (qParam.mLocationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridValueVectorByTimeLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,query.mAttributeList,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  uint width = 0;
                  uint height = 0;
                  T::Coordinate_svec sCoordinates(new T::Coordinate_vec());
                  if (qParam.mFlags & QueryServer::QueryParameter::Flags::OriginalCoordinates)
                  {
                    T::Coordinate_svec latlonCoordinates(new T::Coordinate_vec());
                    Identification::gridDef.getGridOriginalCoordinatesByGeometry(query.mAttributeList,latlonCoordinates,3,sCoordinates,width,height);
                  }
                  else
                  {
                    Identification::gridDef.getGridLatLonCoordinatesByGeometry(query.mAttributeList,sCoordinates,width,height);
                  }
                  if (sCoordinates)
                    coordinates = *sCoordinates;
                }
              }
              break;

            case QueryParameter::LocationType::Grid:
              result = mDataServerPtr->getGridValueVectorByTimeLevelAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,query.mCoordinateType,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
                {
                  coordinates = query.mAreaCoordinates[0];
                }
              }
              break;

            default:
              result = mDataServerPtr->getGridValueVectorByTimeAndLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueVector);
              if (result == 0)
              {
                if (coordinates.size() == 0 && ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0  || conversionByFunction))
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

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

        if ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) == 0)
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
    Query& query,
    QueryParameter& qParam,
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    uint& newProducerId,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (query.mAreaCoordinates.size() == 0)
      return false;

    short areaInterpolationMethod = qParam.mAreaInterpolationMethod;
    if (areaInterpolationMethod == T::AreaInterpolationMethod::Undefined)
      areaInterpolationMethod = pInfo.mAreaInterpolationMethod;

    short timeInterpolationMethod = qParam.mTimeInterpolationMethod;
    if (timeInterpolationMethod == T::TimeInterpolationMethod::Undefined)
      timeInterpolationMethod = pInfo.mTimeInterpolationMethod;

    short levelInterpolationMethod = qParam.mLevelInterpolationMethod;
    if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
      levelInterpolationMethod = pInfo.mLevelInterpolationMethod;


    //query.print(std::cout,0,0);

    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction = conversionFunction(pInfo.mConversionFunction, function, functionParams);
    T::ParamValue_vec valueVector;
    T::ParamValue_vec newValueVector;

    std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
    int result = getContentListByParameterGenerationIdAndForecastTime(0, producerInfo.mProducerId, producerInfo.mHash, generationId, pInfo.mParameterKeyType, pInfo.mParameterKey, pInfo.getKeyHash(),
        paramLevelId, paramLevel, qParam.mForecastType,qParam.mForecastNumber,producerGeometryId, forecastTime, contentList);

    if (result != 0)
    {
      Fmi::Exception exception(BCP, "ContentServer returns an error!");
      exception.addParameter("Service", "getContentListByParameterGenerationIdAndForecastTime");
      exception.addParameter("ErrorMessage", ContentServer::getResultString(result));
      exception.addParameter("producerId", std::to_string(producerInfo.mProducerId));
      exception.addParameter("parameter", pInfo.mParameterKey);
      exception.addParameter("levelId", std::to_string(paramLevelId));
      exception.addParameter("level", std::to_string(paramLevel));
      exception.addParameter("geometryId", std::to_string(producerGeometryId));
      exception.addParameter("forecastTime", utcTimeFromTimeT(forecastTime));
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


    T::PropertySettingVec properties;
    result = mDataServerPtr->getGridProperties(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,properties);
    if (result != 0)
    {
      Fmi::Exception exception(BCP, "DataServer returns an error!");
      exception.addParameter("Service", "getGridProperties");
      exception.addParameter("ErrorMessage", DataServer::getResultString(result));
      exception.addParameter("FileId", std::to_string(contentInfo1->mFileId));
      exception.addParameter("MessageIndex", std::to_string(contentInfo1->mMessageIndex));
      std::string errorMsg = exception.getStackTrace();
      PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
      return false;
    }

    std::map<uint,long long> oldProperties;
    std::map<uint,long long> newProperties;

    for (auto it = properties.begin(); it != properties.end();++it)
    {
      //std::cout << "PROPERTY " << it->propertyId << " = " << it->propertyValue << "\n";
      oldProperties.insert(std::pair<uint,long long>(it->propertyId,atoll(it->propertyValue.c_str())));
    }

    uint version = 0;
    const int id = GRIB1::Property::IndicatorSection::EditionNumber;
    auto v1 = oldProperties.find(id);
    if (v1 != oldProperties.end())
      version = 1;

    if (version == 0)
    {
      const int id = GRIB1::Property::IndicatorSection::EditionNumber;
      auto v2 = oldProperties.find(id);
      if (v2 != oldProperties.end())
        version = 2;
    }

    if (version == 0)
      return false;


    std::string ftime = utcTimeFromTimeT(forecastTime);

    int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
    splitTimeString(ftime,year,month,day,hour,minute,second);

    GRID::GridFile newGridFile;
    GRID::Message *newMessage = nullptr;

    const char *geometryIdStr = query.mAttributeList.getAttributeValue("grid.geometryId");
    const char *crsStr = query.mAttributeList.getAttributeValue("grid.crs");
    const char *bboxStr = query.mAttributeList.getAttributeValue("grid.bbox");
    const char *gridSizeStr = query.mAttributeList.getAttributeValue("grid.size");

    long long gridWidth = 0;
    long long gridHeight = 0;

    double lon1 = 0;
    double lat1 = 0;
    double lon2 = 0;
    double lat2 = 0;
    double xStepMetric = 0;
    double yStepMetric = 0;

    if (gridSizeStr)
    {
      std::vector<double> v;
      splitString(gridSizeStr,',',v);
      if (v.size() == 2)
      {
        gridWidth = (long long)v[0];
        gridHeight = (long long)v[1];
      }
    }


    if (query.mAreaCoordinates[0].size() == 1)
    {
      const char *gridMetricDistanceStr = query.mAttributeList.getAttributeValue("grid.metricDistance");
      const char *gridMetricSizeStr = query.mAttributeList.getAttributeValue("grid.metricSize");

      std::vector<double> v;
      if (gridMetricDistanceStr)
      {
        splitString(gridMetricDistanceStr,',',v);
        if (v.size() == 4)
        {
          latLon_bboxByCenter(query.mAreaCoordinates[0][0].x(),query.mAreaCoordinates[0][0].y(),1000*v[0],1000*v[1],1000*v[2],1000*v[3],lon1,lat1,lon2,lat2);

          xStepMetric = (double)1000*(v[0]+v[2]) / (double)gridWidth;
          yStepMetric = (double)1000*(v[1]+v[3]) / (double)gridHeight;
        }
      }
      else
      if (gridMetricSizeStr)
      {
        splitString(gridMetricSizeStr,',',v);
        if (v.size() == 2)
        {
          latLon_bboxByCenter(query.mAreaCoordinates[0][0].x(),query.mAreaCoordinates[0][0].y(),1000*v[0],1000*v[1],lon1,lat1,lon2,lat2);

          xStepMetric = (double)1000*(v[0]) / (double)gridWidth;
          yStepMetric = (double)1000*(v[1]) / (double)gridHeight;
        }
      }
    }


    std::vector<double> aa;
    if (bboxStr)
    {
      splitString(bboxStr,',',aa);
      if (aa.size() == 4)
      {
        lon1 = aa[0];
        lat1 = aa[1];
        lon2 = aa[2];
        lat2 = aa[3];
      }
    }

    //printf("COORDINATES %f,%f,%f,%f\n",lon1,lat1,lon2,lat2);
    double xDegrees = lon2-lon1;
    double yDegrees = lat2-lat1;

    double xStepDegrees = xDegrees / (double)gridWidth;
    double yStepDegrees = yDegrees / (double)gridHeight;

    float lat_0 = ParamValueMissing;
    float lat_1 = ParamValueMissing;
    float lat_2 = ParamValueMissing;
    float laD = ParamValueMissing;
    float lon_0 = ParamValueMissing;
    float x_0 = ParamValueMissing;
    float y_0 = ParamValueMissing;


    if (!geometryIdStr  &&  crsStr  &&  gridSizeStr)
    {
      OGRSpatialReference sr;

      if (sr.SetFromUserInput(crsStr) != OGRERR_NONE)
        throw Fmi::Exception(BCP, "Invalid crs '" + std::string(crsStr) + "'!");

      // sr.dumpReadable();

      std::vector<std::string> partList;

      char *out = nullptr;
      sr.exportToProj4(&out);
      //printf("%s\n",out);
      splitString(out,' ',partList);
      CPLFree(out);

      std::string projectionStr;

      for (auto it=partList.begin(); it!=partList.end(); ++it)
      {
        std::vector<std::string> attr;
        splitString(it->c_str(),'=',attr);
        if (attr.size() == 2)
        {
          // printf("[%s][%s]\n",attr[0].c_str(),attr[1].c_str());
          if (attr[0] == "+proj")
            projectionStr = attr[1];

          if (attr[0] == "+lat_0")
            lat_0 = toDouble(attr[1]);

          if (attr[0] == "+lat_1")
            lat_1 = toDouble(attr[1]);

          if (attr[0] == "+lat_2")
            lat_2 = toDouble(attr[1]);

          if (attr[0] == "+lat_ts")
            laD = toDouble(attr[1]);

          if (attr[0] == "+lon_0")
            lon_0 = toDouble(attr[1]);

          if (attr[0] == "+x_0")
            x_0 = toDouble(attr[1]);

          if (attr[0] == "+y_0")
            y_0 = toDouble(attr[1]);
        }
      }

      if (version == 1)
      {
        if (projectionStr == "longlat")
        {
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::DataRepresentationType,0LL));

          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::LatLon::Ni,gridWidth));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::LatLon::Nj,gridHeight));

          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::LatLon::IDirectionIncrement,(long long)(xStepDegrees*1000)));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::LatLon::JDirectionIncrement,(long long)(yStepDegrees*1000)));

          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::GridArea::LatitudeOfFirstGridPoint,(long long)(lat1*1000)));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::GridArea::LongitudeOfFirstGridPoint,(long long)(lon1*1000)));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::GridArea::LatitudeOfLastGridPoint,(long long)(lat2*1000)));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::GridArea::LongitudeOfLastGridPoint,(long long)(lon2*1000)));

          if (lat1 < lat2)
            newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::ScanningMode::ScanMode,(long long)0x40));
          else
            newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::ScanningMode::ScanMode,(long long)0));
        }


        if (projectionStr == "stere"  && query.mAreaCoordinates.size() > 0  && query.mAreaCoordinates[0].size() > 0)
        {
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::DataRepresentationType,5LL));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::PolarStereographic::Nx,gridWidth));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::PolarStereographic::Ny,gridHeight));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::PolarStereographic::LatitudeOfFirstGridPoint,(long long)(lat1*1000)));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::PolarStereographic::LongitudeOfFirstGridPoint,(long long)(lon1*1000)));

          if (lon_0 != ParamValueMissing)
            newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::PolarStereographic::OrientationOfTheGrid,(long long)(lon_0*1000)));

          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::PolarStereographic::DxInMetres,(long long)xStepMetric));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::PolarStereographic::DyInMetres,(long long)yStepMetric));
          //newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::PolarStereographic::ProjectionCentreFlag:

          if (lat1 < lat2)
            newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::ScanningMode::ScanMode,(long long)0x40));
          else
            newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::ScanningMode::ScanMode,(long long)0));
        }


        if (projectionStr == "lcc"  && query.mAreaCoordinates.size() > 0  && query.mAreaCoordinates[0].size() > 0)
        {
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::DataRepresentationType,3LL));

          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::LambertConformal::Nx,gridWidth));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::LambertConformal::Ny,gridHeight));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::LambertConformal::LatitudeOfFirstGridPoint,(long long)(lat1*1000)));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::LambertConformal::LongitudeOfFirstGridPoint,(long long)(lon1*1000)));

          if (lon_0 != ParamValueMissing)
            newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::LambertConformal::LoV,(long long)(lon_0*1000)));

          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::LambertConformal::DxInMetres,(long long)xStepMetric));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::LambertConformal::DyInMetres,(long long)yStepMetric));
          //newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::LambertConformal::ProjectionCentreFlag:

          if (lat_1 != ParamValueMissing)
            newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::LambertConformal::Latin1,(long long)(lat_1*1000)));

          if (lat_2 != ParamValueMissing)
            newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::LambertConformal::Latin2,(long long)(lat_2*1000)));

          //newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::LambertConformal::LatitudeOfSouthernPole:
          //newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::LambertConformal::LongitudeOfSouthernPole:


          if (lat1 < lat2)
            newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::ScanningMode::ScanMode,(long long)0x40));
          else
            newProperties.insert(std::pair<uint,long long>((uint)GRIB1::Property::GridSection::ScanningMode::ScanMode,(long long)0));
        }
      }


      if (version == 2)
      {
        if (projectionStr == "longlat")
        {
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::GridDefinitionTemplateNumber,(long long)GRIB2::GridSection::Template::LatLon));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::NumberOfGridPoints,gridWidth*gridHeight));

          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::Grid::Ni,gridWidth));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::Grid::Nj,gridHeight));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::Grid::LatitudeOfFirstGridPoint,(long long)(lat1*1000000)));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::Grid::LongitudeOfFirstGridPoint,(long long)(lon1*1000000)));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::Grid::LatitudeOfLastGridPoint,(long long)(lat2*1000000)));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::Grid::LongitudeOfLastGridPoint,(long long)(lon2*1000000)));

          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LatLon::IDirectionIncrement,(long long)(xStepDegrees*1000000)));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LatLon::JDirectionIncrement,(long long)(yStepDegrees*1000000)));

          if (lat1 < lat2)
            newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LatLon::ScanningMode,(long long)0x40));
          else
            newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LatLon::ScanningMode,(long long)0x0));
        }


        if (projectionStr == "stere"  && query.mAreaCoordinates.size() > 0  && query.mAreaCoordinates[0].size() > 0)
        {
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::GridDefinitionTemplateNumber,(long long)GRIB2::GridSection::Template::PolarStereographic));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::NumberOfGridPoints,gridWidth*gridHeight));

          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::PolarStereographic::Nx,gridWidth));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::PolarStereographic::Ny,gridHeight));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::PolarStereographic::LatitudeOfFirstGridPoint,(long long)(lat1*1000000)));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::PolarStereographic::LongitudeOfFirstGridPoint,(long long)(lon1*1000000)));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::PolarStereographic::ResolutionAndComponentFlags,48));
          if (laD != ParamValueMissing)
            newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::PolarStereographic::LaD,(long long)(laD*1000000)));
          else
            newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::PolarStereographic::LaD,60000000));

          if (lon_0 != ParamValueMissing)
            newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::PolarStereographic::OrientationOfTheGrid,(long long)(lon_0*1000000)));

          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::PolarStereographic::Dx,(long long)xStepMetric*1000));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::PolarStereographic::Dy,(long long)yStepMetric*1000));
          //newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::PolarStereographic::ProjectionCentreFlag:

          if (lat1 < lat2)
            newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::PolarStereographic::ScanningMode,(long long)0x40));
          else
            newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::PolarStereographic::ScanningMode,(long long)0x0));
        }


        if (projectionStr == "lcc"  && query.mAreaCoordinates.size() > 0  && query.mAreaCoordinates[0].size() > 0)
        {
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::GridDefinitionTemplateNumber,(long long)GRIB2::GridSection::Template::LambertConformal));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::NumberOfGridPoints,gridWidth*gridHeight));

          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::Nx,gridWidth));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::Ny,gridHeight));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::LatitudeOfFirstGridPoint,(long long)(lat1*1000000)));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::LongitudeOfFirstGridPoint,(long long)(lon1*1000000)));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::ResolutionAndComponentFlags,48));

          if (laD != ParamValueMissing)
            newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::LaD,(long long)(laD*1000000)));
          else
            newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::LaD,60000000));

          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::LoV,(long long)(lon_0*1000000)));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::Dx,(long long)xStepMetric*1000));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::Dy,(long long)yStepMetric*1000));
          //newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::ProjectionCentreFlag:
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::Latin1,(long long)(lat_1*1000000)));
          newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::Latin2,(long long)(lat_2*1000000)));
          //newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::LatitudeOfSouthernPole:
          //newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::LongitudeOfSouthernPole:

          if (lat1 < lat2)
            newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::ScanningMode,(long long)0x40));
          else
            newProperties.insert(std::pair<uint,long long>((uint)GRIB2::Property::GridSection::LambertConformal::ScanningMode,(long long)0x0));
        }

      }
    }


    if (version == 1)
    {
      using namespace SmartMet::GRIB1;

      if (geometryIdStr)
      {
        GRIB1::GridDef_sptr def = Identification::gridDef.getGrib1DefinitionByGeometryId(atoi(geometryIdStr));
        if (def)
        {
          newProperties.insert(std::pair<uint,long long>((uint)Property::GridSection::DataRepresentationType,def->getTemplateNumber()));

          T::PropertySettingVec tmpProperties;
          def->getProperties(tmpProperties);

          for (auto it = tmpProperties.begin(); it != tmpProperties.end();++it)
          {
            //std::cout << "NPROPERTY " << it->propertyId << " = " << it->propertyValue << "\n";
            newProperties.insert(std::pair<uint,long long>(it->propertyId,atoll(it->propertyValue.c_str())));
          }
        }
      }

      newMessage = newGridFile.newMessage(T::FileTypeValue::Grib1);

      newProperties.insert(std::pair<uint,long long>((uint)Property::ProductSection::SectionFlags,128));
      newProperties.insert(std::pair<uint,long long>((uint)Property::ProductSection::Level,paramLevel));
      newProperties.insert(std::pair<uint,long long>((uint)Property::ProductSection::YearOfCentury,year % 100));
      newProperties.insert(std::pair<uint,long long>((uint)Property::ProductSection::Month,month));
      newProperties.insert(std::pair<uint,long long>((uint)Property::ProductSection::Day,day));
      newProperties.insert(std::pair<uint,long long>((uint)Property::ProductSection::Hour,hour));
      newProperties.insert(std::pair<uint,long long>((uint)Property::ProductSection::Minute,minute));
      newProperties.insert(std::pair<uint,long long>((uint)Property::ProductSection::UnitOfTimeRange,1));
      newProperties.insert(std::pair<uint,long long>((uint)Property::ProductSection::P1,0));
      newProperties.insert(std::pair<uint,long long>((uint)Property::ProductSection::P2,0));
      newProperties.insert(std::pair<uint,long long>((uint)Property::ProductSection::TimeRangeIndicator,0));
      newProperties.insert(std::pair<uint,long long>((uint)Property::ProductSection::CenturyOfReferenceTimeOfData,(year/100)+1));
      newProperties.insert(std::pair<uint,long long>((uint)Property::ProductSection::ForecastType,contentInfo1->mForecastType));
      newProperties.insert(std::pair<uint,long long>((uint)Property::ProductSection::ForecastNumber,contentInfo1->mForecastNumber));

      newProperties.insert(std::pair<uint,long long>((uint)Property::DataSection::Flags,8));
      newProperties.insert(std::pair<uint,long long>((uint)Property::DataSection::BinaryScaleFactor,-5));
      newProperties.insert(std::pair<uint,long long>((uint)Property::DataSection::ReferenceValue,0.0));
      newProperties.insert(std::pair<uint,long long>((uint)Property::DataSection::BitsPerValue,32));
      newProperties.insert(std::pair<uint,long long>((uint)Property::DataSection::PackingMethod,0));

      // ### Setting default values for the parameters

      auto props = GRIB1::gribProperty.getPropertyVector();

      for (auto it = props.begin(); it != props.end();++it)
      {
        auto np = newProperties.find(it->second);

        if (np != newProperties.end())
        {
          // printf("REPLACE %s (%u) %lld\n",it->first.c_str(),it->second,np->second);
          newMessage->setProperty(it->second,np->second);
        }
        else
        {
          if (it->second < Property::GridSection::FirstProperty || it->second > Property::GridSection::LastProperty)
          {
            auto op = oldProperties.find(it->second);
            if (op != oldProperties.end())
            {
              // printf("SET %s (%u) %lld\n",it->first.c_str(),it->second,op->second);
              newMessage->setProperty(it->second,op->second);
            }
          }
        }
      }
    }
    else
    if (version == 2)
    {
      //PRINT_DATA(mDebugLog, "-- GRIB version 2\n");

      using namespace SmartMet::GRIB2;

      if (geometryIdStr)
      {
        //Identification::gridDef.getGridLatLonCoordinatesByGeometry(query.mAttributeList,latLonCoordinates,width,height);
        GRIB2::GridDef_sptr def = Identification::gridDef.getGrib2DefinitionByGeometryId(atoi(geometryIdStr));
        if (def)
        {
          //printf("TEMPLATE %d\n",def->getTemplateNumber());

          auto rows = def->getGridRowCount();
          auto cols = def->getGridColumnCount();

          newProperties.insert(std::pair<uint,long long>((uint)Property::GridSection::NumberOfGridPoints,cols*rows));
          newProperties.insert(std::pair<uint,long long>((uint)Property::GridSection::GridDefinitionTemplateNumber,def->getTemplateNumber()));

          T::PropertySettingVec tmpProperties;
          def->getProperties(tmpProperties);

          for (auto it = tmpProperties.begin(); it != tmpProperties.end();++it)
          {
            //std::cout << "NPROPERTY " << it->propertyId << " = " << it->propertyValue << "\n";
            newProperties.insert(std::pair<uint,long long>(it->propertyId,atoll(it->propertyValue.c_str())));
          }
        }
      }

      newMessage = newGridFile.newMessage(T::FileTypeValue::Grib2);

      newProperties.insert(std::pair<uint,long long>((uint)Property::IdentificationSection::SignificanceOfReferenceTime,1));
      newProperties.insert(std::pair<uint,long long>((uint)Property::IdentificationSection::Year,year));
      newProperties.insert(std::pair<uint,long long>((uint)Property::IdentificationSection::Month,month));
      newProperties.insert(std::pair<uint,long long>((uint)Property::IdentificationSection::Day,day));
      newProperties.insert(std::pair<uint,long long>((uint)Property::IdentificationSection::Hour,hour));
      newProperties.insert(std::pair<uint,long long>((uint)Property::IdentificationSection::Minute,minute));
      newProperties.insert(std::pair<uint,long long>((uint)Property::IdentificationSection::Second,second));

      newProperties.insert(std::pair<uint,long long>(Property::RepresentationSection::RepresentationTemplateNumber,RepresentationSection::Template::GridDataRepresentation));

      newProperties.insert(std::pair<uint,long long>(Property::ProductSection::ParameterSettings::ForecastTime,0));
      newProperties.insert(std::pair<uint,long long>(Property::ProductSection::HorizontalSettings::ScaledValueOfFirstFixedSurface,paramLevel));

      // ### Setting default values for the parameters

      auto props = GRIB2::gribProperty.getPropertyVector();

      for (auto it = props.begin(); it != props.end();++it)
      {
        auto np = newProperties.find(it->second);

        if (np != newProperties.end())
        {
          // printf("REPLACE %s (%u) %lld\n",it->first.c_str(),it->second,np->second);
          newMessage->setProperty(it->second,np->second);
        }
        else
        {
          if (it->second < Property::GridSection::FirstProperty || it->second > Property::GridSection::LastProperty)
          {
            auto op = oldProperties.find(it->second);
            if (op != oldProperties.end())
            {
              // printf("SET %s (%u) %lld\n",it->first.c_str(),it->second,op->second);
              newMessage->setProperty(it->second,op->second);
            }
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


    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC == forecastTime /*|| timeInterpolationMethod == T::TimeInterpolationMethod::None || timeInterpolationMethod == T::TimeInterpolationMethod::Nearest*/)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        int result = mDataServerPtr->getGridValueVectorByCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,query.mCoordinateType,*coordinates,areaInterpolationMethod,modificationOperation,modificationParameters,valueVector);

        if (result != 0)
        {
          Fmi::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueVectorByCoordinateList");
          exception.addParameter("ErrorMessage", DataServer::getResultString(result));
          exception.addParameter("FileId", std::to_string(contentInfo1->mFileId));
          exception.addParameter("MessageIndex", std::to_string(contentInfo1->mMessageIndex));
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

        // newMessage->print(std::cout,0,0);

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

        int result = mDataServerPtr->getGridValueVectorByTimeAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,forecastTime,query.mCoordinateType,*coordinates,query.mAttributeList,modificationOperation,modificationParameters,valueVector);

        if (result != 0)
        {
          Fmi::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueVectorByTimeAndCoordinateList");
          exception.addParameter("ErrorMessage", DataServer::getResultString(result));
          exception.addParameter("FileId", std::to_string(contentInfo1->mFileId));
          exception.addParameter("MessageIndex", std::to_string(contentInfo1->mMessageIndex));
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

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

        int result = mDataServerPtr->getGridValueVectorByLevelAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,query.mCoordinateType,*coordinates,query.mAttributeList,modificationOperation,modificationParameters,valueVector);
        if (result != 0)
        {
          Fmi::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueVectorByLevelAndCoordinateList");
          exception.addParameter("ErrorMessage", DataServer::getResultString(result));
          exception.addParameter("FileId", std::to_string(contentInfo1->mFileId));
          exception.addParameter("MessageIndex", std::to_string(contentInfo1->mMessageIndex));
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

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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
        int result = mDataServerPtr->getGridValueVectorByTimeLevelAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,forecastTime,paramLevel,query.mCoordinateType,*coordinates,query.mAttributeList,modificationOperation,modificationParameters,valueVector);
        if (result != 0)
        {
          Fmi::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueVectorByTimeLevelAndCoordinateList");
          exception.addParameter("ErrorMessage", DataServer::getResultString(result));
          exception.addParameter("FileId", std::to_string(contentInfo1->mFileId));
          exception.addParameter("MessageIndex", std::to_string(contentInfo1->mMessageIndex));
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

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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
        exception.addParameter("ErrorMessage", ContentServer::getResultString(result));
        exception.addParameter("generationId", std::to_string(generationId));
        exception.addParameter("parameter", pInfo.mParameterKey);
        exception.addParameter("geometryId", std::to_string(producerGeometryId));
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

        // It might be that there is no climatological data avainlable on xxxx-02-29, maybe
        // we should use xxxx-02-28 instead.

        if (tmp1.substr(4,4) == "0229")
          tmp2 = ft.substr(0, 4) + "0228" + tmp1.substr(8);

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
        exception.addParameter("ErrorMessage", ContentServer::getResultString(result));
        exception.addParameter("producerId", std::to_string(producerInfo.mProducerId));
        exception.addParameter("parameter", pInfo.mParameterKey);
        exception.addParameter("levelId", std::to_string(paramLevelId));
        exception.addParameter("level", std::to_string(paramLevel));
        exception.addParameter("geometryId", std::to_string(producerGeometryId));
        exception.addParameter("forecastTime", utcTimeFromTimeT(fTime));
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





bool ServiceImplementation::getPointValuesByHeight(
    Query& query,
    T::CoordinateType coordinateType,
    T::Coordinate_vec& coordinates,
    QueryParameter& qParam,
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    uint& newProducerId,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getPointValuesByHeight(%d)\n",paramLevel);

    short areaInterpolationMethod = qParam.mAreaInterpolationMethod;
    if (areaInterpolationMethod == T::AreaInterpolationMethod::Undefined)
      areaInterpolationMethod = pInfo.mAreaInterpolationMethod;

    short timeInterpolationMethod = qParam.mTimeInterpolationMethod;
    if (timeInterpolationMethod == T::TimeInterpolationMethod::Undefined)
      timeInterpolationMethod = pInfo.mTimeInterpolationMethod;

    short levelInterpolationMethod = qParam.mLevelInterpolationMethod;
    if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
      levelInterpolationMethod = pInfo.mLevelInterpolationMethod;

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

    HeightRec_vec recs;
    if (getClosestLevelsByHeight(producerInfo.mProducerId,generationId,producerGeometryId,qParam.mParameterLevelId,qParam.mForecastType,qParam.mForecastNumber,forecastTime,coordinateType,coordinates,paramLevel,recs))
    {
      uint rlen = recs.size();
      if (rlen == coordinates.size())
      {
        std::shared_ptr<T::ContentInfoList> prevContentList1;
        std::shared_ptr<T::ContentInfoList> prevContentList2;
        T::ParamLevel prevLevel1 = 0;
        T::ParamLevel prevLevel2 = 0;
        time_t prevTime1 = 0;
        time_t prevTime2 = 0;

        for (uint r=0; r<rlen; r++)
        {
          std::shared_ptr<T::ContentInfoList> contentList1 = std::make_shared<T::ContentInfoList>();
          time_t fTime1 = 0;

          if (prevContentList1  &&  prevLevel1 == recs[r].level1_1)
          {
            contentList1 = prevContentList1;
            fTime1 = prevTime1;
          }
          else
          {
            fTime1 = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,recs[r].levelId,recs[r].level1_1,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList1);
            prevContentList1 = contentList1;
            prevLevel1 = recs[r].level1_1;
            prevTime1 = fTime1;
          }

          PRINT_DATA(mDebugLog, "         + Found %u content records (fTime = %ld)\n", contentList1->getLength(),fTime1);

          if (fTime1 == 0)
            return false;

          if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
          {
            std::stringstream stream;
            contentList1->print(stream, 0, 4);
            PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
          }

          uint contentLen1 = contentList1->getLength();
          if (contentLen1 == 0)
            return false;

          std::shared_ptr<T::ContentInfoList> contentList2 = std::make_shared<T::ContentInfoList>();
          time_t fTime2 = 0;

          if (prevContentList2  &&  prevLevel2 == recs[r].level1_2)
          {
            contentList2 = prevContentList2;
            fTime2 = prevTime2;
          }
          else
          {
            fTime2 = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,recs[r].levelId,recs[r].level1_2,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList2);
            prevContentList2 = contentList2;
            prevLevel2 = recs[r].level1_2;
            prevTime2 = fTime2;
          }

          PRINT_DATA(mDebugLog, "         + Found %u content records (fTime = %ld)\n", contentList2->getLength(),fTime2);

          if (fTime2 == 0)
            return false;

          if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
          {
            std::stringstream stream;
            contentList2->print(stream, 0, 4);
            PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
          }

          uint contentLen2 = contentList2->getLength();
          if (contentLen2 == 0)
            return false;

          if (contentLen1 != contentLen2 || fTime1 != fTime2)
            return false;

          T::ContentInfo* contentInfo1_1 = contentList1->getContentInfoByIndex(0);
          T::ContentInfo* contentInfo1_2 = contentList1->getContentInfoByIndex(1);
          T::ContentInfo* contentInfo2_1 = contentList2->getContentInfoByIndex(0);
          T::ContentInfo* contentInfo2_2 = contentList2->getContentInfoByIndex(1);

          valueList.mParameterKeyType = pInfo.mParameterKeyType;
          valueList.mParameterKey = pInfo.mParameterKey;
          valueList.mParameterLevelId = paramLevelId;
          valueList.mForecastTimeUTC = forecastTime;
          valueList.mProducerId = contentInfo1_1->mProducerId;
          valueList.mGenerationId = contentInfo1_1->mGenerationId;
          valueList.mGenerationFlags = generationFlags;
          valueList.mGeometryId = contentInfo1_1->mGeometryId;
          valueList.mFileId[0] = contentInfo1_1->mFileId;
          valueList.mMessageIndex[0] = contentInfo1_1->mMessageIndex;
          valueList.mModificationTime = contentInfo1_1->mModificationTime;
          valueList.mAnalysisTime = analysisTime;
          valueList.mForecastType = contentInfo1_1->mForecastType;
          valueList.mForecastNumber = contentInfo1_1->mForecastNumber;
          valueList.mParameterLevel = paramLevel;
          valueList.mParameterLevelId = contentInfo1_1->mFmiParameterLevelId;

          if (contentLen1 == 1)
          {
            if (contentInfo1_1->mForecastTimeUTC == fTime1  &&  recs[r].time1 == fTime1)
            {
              // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

              valueList.mFlags = ParameterValues::Flags::DataAvailable;

              if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
              {
                T::ParamValue value1;
                int result1 = mDataServerPtr->getGridValueByPoint(0, contentInfo1_1->mFileId, contentInfo1_1->mMessageIndex,query.mCoordinateType,coordinates[r].x(),coordinates[r].y(),areaInterpolationMethod,modificationOperation,modificationParameters,value1);
                if (result1 != 0)
                {
                  Fmi::Exception exception(BCP, "DataServer returns an error!");
                  exception.addParameter("Service", "getGridValueByPoint");
                  exception.addParameter("ErrorMessage", DataServer::getResultString(result1));
                  exception.addParameter("FileId", std::to_string(contentInfo1_1->mFileId));
                  exception.addParameter("MessageIndex", std::to_string(contentInfo1_1->mMessageIndex));
                  std::string errorMsg = exception.getStackTrace();
                  PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
                  return false;
                }

                T::ParamValue value2;
                int result2 = mDataServerPtr->getGridValueByPoint(0, contentInfo2_1->mFileId, contentInfo2_1->mMessageIndex,query.mCoordinateType,coordinates[r].x(),coordinates[r].y(),areaInterpolationMethod,modificationOperation,modificationParameters,value2);
                if (result2 != 0)
                {
                  Fmi::Exception exception(BCP, "DataServer returns an error!");
                  exception.addParameter("Service", "getGridValueByPoint");
                  exception.addParameter("ErrorMessage", DataServer::getResultString(result2));
                  exception.addParameter("FileId", std::to_string(contentInfo2_1->mFileId));
                  exception.addParameter("MessageIndex", std::to_string(contentInfo2_1->mMessageIndex));
                  std::string errorMsg = exception.getStackTrace();
                  PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
                  return false;
                }


                T::ParamValue val = levelInterpolation(value1,value2,recs[r].height1_1,recs[r].height1_2,paramLevel,levelInterpolationMethod);
                T::GridValue v(coordinates[r].x(),coordinates[r].y(),val);
                valueList.mValueList.addGridValue(v);
              }
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

          if (contentLen1 == 2)
          {
            if (contentInfo1_1->mForecastTimeUTC < fTime1  &&  contentInfo1_2->mForecastTimeUTC > fTime1  &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden)
            {
              // Content records have different times, but most likely the same levels, so we need to do time interpolation.

              valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;

              if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0  &&  recs[r].time1 == contentInfo1_1->mForecastTimeUTC  &&  recs[r].time2 == contentInfo1_2->mForecastTimeUTC)
              {
                T::ParamValue value1_1;
                int result1_1 = mDataServerPtr->getGridValueByPoint(0, contentInfo1_1->mFileId, contentInfo1_1->mMessageIndex,query.mCoordinateType,coordinates[r].x(),coordinates[r].y(),areaInterpolationMethod,modificationOperation,modificationParameters,value1_1);
                if (result1_1 != 0)
                {
                  Fmi::Exception exception(BCP, "DataServer returns an error!");
                  exception.addParameter("Service", "getGridValueByPoint");
                  exception.addParameter("ErrorMessage", DataServer::getResultString(result1_1));
                  exception.addParameter("FileId", std::to_string(contentInfo1_1->mFileId));
                  exception.addParameter("MessageIndex", std::to_string(contentInfo1_1->mMessageIndex));
                  std::string errorMsg = exception.getStackTrace();
                  PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
                  return false;
                }

                T::ParamValue value1_2;
                int result1_2 = mDataServerPtr->getGridValueByPoint(0, contentInfo1_2->mFileId, contentInfo1_2->mMessageIndex,query.mCoordinateType,coordinates[r].x(),coordinates[r].y(),areaInterpolationMethod,modificationOperation,modificationParameters,value1_2);
                if (result1_2 != 0)
                {
                  Fmi::Exception exception(BCP, "DataServer returns an error!");
                  exception.addParameter("Service", "getGridValueByPoint");
                  exception.addParameter("ErrorMessage", DataServer::getResultString(result1_2));
                  exception.addParameter("FileId", std::to_string(contentInfo1_2->mFileId));
                  exception.addParameter("MessageIndex", std::to_string(contentInfo1_2->mMessageIndex));
                  std::string errorMsg = exception.getStackTrace();
                  PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
                  return false;
                }

                T::ParamValue value2_1;
                int result2_1 = mDataServerPtr->getGridValueByPoint(0, contentInfo2_1->mFileId, contentInfo2_1->mMessageIndex,query.mCoordinateType,coordinates[r].x(),coordinates[r].y(),areaInterpolationMethod,modificationOperation,modificationParameters,value2_1);
                if (result2_1 != 0)
                {
                  Fmi::Exception exception(BCP, "DataServer returns an error!");
                  exception.addParameter("Service", "getGridValueByPoint");
                  exception.addParameter("ErrorMessage", DataServer::getResultString(result2_1));
                  exception.addParameter("FileId", std::to_string(contentInfo2_1->mFileId));
                  exception.addParameter("MessageIndex", std::to_string(contentInfo2_1->mMessageIndex));
                  std::string errorMsg = exception.getStackTrace();
                  PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
                  return false;
                }

                T::ParamValue value2_2;
                int result2_2 = mDataServerPtr->getGridValueByPoint(0, contentInfo2_2->mFileId, contentInfo2_2->mMessageIndex,query.mCoordinateType,coordinates[r].x(),coordinates[r].y(),areaInterpolationMethod,modificationOperation,modificationParameters,value2_2);
                if (result2_2 != 0)
                {
                  Fmi::Exception exception(BCP, "DataServer returns an error!");
                  exception.addParameter("Service", "getGridValueByPoint");
                  exception.addParameter("ErrorMessage", DataServer::getResultString(result2_2));
                  exception.addParameter("FileId", std::to_string(contentInfo2_2->mFileId));
                  exception.addParameter("MessageIndex", std::to_string(contentInfo2_2->mMessageIndex));
                  std::string errorMsg = exception.getStackTrace();
                  PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
                  return false;
                }

                T::ParamValue val1 = levelInterpolation(value1_1,value2_1,recs[r].height1_1,recs[r].height1_2,paramLevel,levelInterpolationMethod);
                T::ParamValue val2 = levelInterpolation(value1_2,value2_2,recs[r].height2_1,recs[r].height2_2,paramLevel,levelInterpolationMethod);
                T::ParamValue val = timeInterpolation(val1,val2,recs[r].time1,recs[r].time2,forecastTime,timeInterpolationMethod);

                T::GridValue v(coordinates[r].x(),coordinates[r].y(),val);
                valueList.mValueList.addGridValue(v);
              }
            }
          }
        }
      }
    }

    if (conversionByFunction)
      executeConversion(function, functionParams, forecastTime, valueList.mValueList);

    if (qParam.mPrecision < 0)
      qParam.mPrecision = pInfo.mDefaultPrecision;

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






bool ServiceImplementation::getValueVectorsByHeight(
    Query& query,
    T::CoordinateType coordinateType,
    T::Coordinate_vec& coordinates,
    QueryParameter& qParam,
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    uint& newProducerId,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getValueVectorsByHeight(%d)\n",paramLevel);

    short areaInterpolationMethod = qParam.mAreaInterpolationMethod;
    if (areaInterpolationMethod == T::AreaInterpolationMethod::Undefined)
      areaInterpolationMethod = pInfo.mAreaInterpolationMethod;

    short timeInterpolationMethod = qParam.mTimeInterpolationMethod;
    if (timeInterpolationMethod == T::TimeInterpolationMethod::Undefined)
      timeInterpolationMethod = pInfo.mTimeInterpolationMethod;

    short levelInterpolationMethod = qParam.mLevelInterpolationMethod;
    if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
      levelInterpolationMethod = pInfo.mLevelInterpolationMethod;

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

    T::ParamValue_vec valueVector;
    HeightRec_vec recs;
    recs.reserve(coordinates.size());
    valueVector.reserve(coordinates.size());

    if (!getClosestLevelsByHeight(producerInfo.mProducerId,generationId,producerGeometryId,qParam.mParameterLevelId,qParam.mForecastType,qParam.mForecastNumber,forecastTime,coordinateType,coordinates,paramLevel,recs))
      return false;

    uint rlen = recs.size();
    if (rlen != coordinates.size())
      return false;

    std::set<T::ParamLevel> levels1;
    for (uint r=0; r<rlen; r++)
    {
      levels1.insert(recs[r].level1_1);
      levels1.insert(recs[r].level1_2);
    }

    T::ParamLevel idx = 0;
    for (auto lev = levels1.begin(); lev != levels1.end(); ++lev)
    {
      for (uint r=0; r<rlen; r++)
      {
        if (recs[r].level1_1 == *lev)
          recs[r].level1_1 = idx;

        if (recs[r].level1_2 == *lev)
          recs[r].level1_2 = idx;

      }
      idx++;
    }


    std::vector<std::shared_ptr<T::ParamValue_vec>> valueList1;

    for (auto lev = levels1.begin(); lev != levels1.end(); ++lev)
    {
      std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
      time_t fTime1 = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,recs[0].levelId,*lev,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList);

      //PRINT_DATA(mDebugLog, "         + Found %u content records (fTime = %ld)\n", contentList1->getLength(),fTime1);

      if (fTime1 == 0)
        return false;

      uint contentLen1 = contentList->getLength();
      if (contentLen1 == 0)
        return false;

      T::ContentInfo* contentInfo = contentList->getContentInfoByIndex(0);

      std::shared_ptr<T::ParamValue_vec> values(new T::ParamValue_vec());
      int result1 = mDataServerPtr->getGridValueVectorByCoordinateList(0,contentInfo->mFileId,contentInfo->mMessageIndex,query.mCoordinateType,coordinates,areaInterpolationMethod,0,modificationParameters,*values);
      if (result1 != 0)
      {
        Fmi::Exception exception(BCP, "DataServer returns an error!");
        exception.addParameter("Service", "getGridValueVectorByCoordinateList");
        exception.addParameter("ErrorMessage", DataServer::getResultString(result1));
        exception.addParameter("FileId", std::to_string(contentInfo->mFileId));
        exception.addParameter("MessageIndex", std::to_string(contentInfo->mMessageIndex));
        std::string errorMsg = exception.getStackTrace();
        PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
        return false;
      }

      valueList1.push_back(values);
    }

    if (recs[0].time1 == recs[0].time2)
    {
      valueVector.reserve(rlen);
      for (uint r=0; r<rlen; r++)
      {
        T::ParamValue val = levelInterpolation((*valueList1[recs[r].level1_1])[r],(*valueList1[recs[r].level1_2])[r],recs[r].height1_1,recs[r].height1_2,paramLevel,levelInterpolationMethod);
        valueVector.push_back(val);
      }

      if (conversionByFunction  &&  (qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
         executeConversion(function, functionParams, forecastTime, coordinates, valueVector, valueList.mValueVector);
      else
        valueList.mValueVector = valueVector;

      if (qParam.mPrecision < 0)
        qParam.mPrecision = pInfo.mDefaultPrecision;

      return true;
    }


    if (recs[0].time1 == recs[0].time2)
      return false;


    std::set<T::ParamLevel> levels2;
    for (uint r=0; r<rlen; r++)
    {
      levels2.insert(recs[r].level2_1);
      levels2.insert(recs[r].level2_2);
    }

    idx = 0;
    for (auto lev = levels2.begin(); lev != levels2.end(); ++lev)
    {
      for (uint r=0; r<rlen; r++)
      {
        if (recs[r].level2_1 == *lev)
          recs[r].level2_1 = idx;

        if (recs[r].level2_2 == *lev)
          recs[r].level2_2 = idx;

      }
      idx++;
    }



    std::vector<std::shared_ptr<T::ParamValue_vec>> valueList2;

    for (auto lev = levels2.begin(); lev != levels2.end(); ++lev)
    {
      std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
      time_t fTime2 = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,recs[0].levelId,*lev,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList);

      //PRINT_DATA(mDebugLog, "         + Found %u content records (fTime = %ld)\n", contentList1->getLength(),fTime1);

      if (fTime2 == 0)
        return false;

      uint contentLen = contentList->getLength();
      if (contentLen == 0)
        return false;

      T::ContentInfo* contentInfo = contentList->getContentInfoByIndex(0);

      std::shared_ptr<T::ParamValue_vec> values(new T::ParamValue_vec());
      int result1 = mDataServerPtr->getGridValueVectorByCoordinateList(0,contentInfo->mFileId,contentInfo->mMessageIndex,query.mCoordinateType,coordinates,areaInterpolationMethod,0,modificationParameters,*values);
      if (result1 != 0)
      {
        Fmi::Exception exception(BCP, "DataServer returns an error!");
        exception.addParameter("Service", "getGridValueVectorByCoordinateList");
        exception.addParameter("ErrorMessage", DataServer::getResultString(result1));
        exception.addParameter("FileId", std::to_string(contentInfo->mFileId));
        exception.addParameter("MessageIndex", std::to_string(contentInfo->mMessageIndex));
        std::string errorMsg = exception.getStackTrace();
        PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
        return false;
      }

      valueList2.push_back(values);
    }

    valueVector.reserve(rlen);
    for (uint r=0; r<rlen; r++)
    {
      T::ParamValue val1 = levelInterpolation((*valueList1[recs[r].level1_1])[r],(*valueList2[recs[r].level2_1])[r],recs[r].height1_1,recs[r].height1_2,paramLevel,levelInterpolationMethod);
      T::ParamValue val2 = levelInterpolation((*valueList1[recs[r].level1_2])[r],(*valueList2[recs[r].level2_2])[r],recs[r].height2_1,recs[r].height2_2,paramLevel,levelInterpolationMethod);
      T::ParamValue val = timeInterpolation(val1,val2,recs[r].time1,recs[r].time2,forecastTime,timeInterpolationMethod);

      valueVector.push_back(val);
    }

    if (conversionByFunction  &&  (qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
       executeConversion(function, functionParams, forecastTime, coordinates, valueVector, valueList.mValueVector);
    else
      valueList.mValueVector = valueVector;

    if (qParam.mPrecision < 0)
      qParam.mPrecision = pInfo.mDefaultPrecision;

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





#if 0  // Working

bool ServiceImplementation::getValueVectorsByHeight(
    Query& query,
    T::CoordinateType coordinateType,
    T::Coordinate_vec& coordinates,
    QueryParameter& qParam,
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    uint& newProducerId,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getValueVectorsByHeight(%d)\n",paramLevel);

    short areaInterpolationMethod = qParam.mAreaInterpolationMethod;
    if (areaInterpolationMethod == T::AreaInterpolationMethod::Undefined)
      areaInterpolationMethod = pInfo.mAreaInterpolationMethod;

    short timeInterpolationMethod = qParam.mTimeInterpolationMethod;
    if (timeInterpolationMethod == T::TimeInterpolationMethod::Undefined)
      timeInterpolationMethod = pInfo.mTimeInterpolationMethod;

    short levelInterpolationMethod = qParam.mLevelInterpolationMethod;
    if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
      levelInterpolationMethod = pInfo.mLevelInterpolationMethod;

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

    //qParam.print(std::cout,0,0);

    T::ParamValue_vec valueVector;
    HeightRec_vec recs;
    if (getClosestLevelsByHeight(producerInfo.mProducerId,generationId,producerGeometryId,qParam.mParameterLevelId,qParam.mForecastType,qParam.mForecastNumber,forecastTime,coordinateType,coordinates,paramLevel,recs))
    {
      //printf("HEIGHTS %ld  %ld\n",coordinates.size(),recs.size());
      uint rlen = recs.size();
      if (rlen == coordinates.size())
      {
        std::shared_ptr<T::ContentInfoList> prevContentList1;
        std::shared_ptr<T::ContentInfoList> prevContentList2;
        T::ParamLevel prevLevel1 = 0;
        T::ParamLevel prevLevel2 = 0;
        time_t prevTime1 = 0;
        time_t prevTime2 = 0;

        for (uint r=0; r<rlen; r++)
        {
          std::shared_ptr<T::ContentInfoList> contentList1 = std::make_shared<T::ContentInfoList>();
          time_t fTime1 = 0;

          if (prevContentList1  &&  prevLevel1 == recs[r].level1_1)
          {
            contentList1 = prevContentList1;
            fTime1 = prevTime1;
          }
          else
          {
            fTime1 = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,recs[r].levelId,recs[r].level1_1,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList1);
            prevContentList1 = contentList1;
            prevLevel1 = recs[r].level1_1;
            prevTime1 = fTime1;
          }

          //printf("+ Found %u content records (fTime = %ld)\n", contentList1->getLength(),fTime1);
          //PRINT_DATA(mDebugLog, "         + Found %u content records (fTime = %ld)\n", contentList1->getLength(),fTime1);

          if (fTime1 == 0)
            return false;

          /*
          if (mDebugLog != nullptr &&  mDebugLog->isEnabled()  &&  contentList1->getLength() < 10)
          {
            std::stringstream stream;
            contentList1->print(stream, 0, 4);
            PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
          }
          */

          uint contentLen1 = contentList1->getLength();
          if (contentLen1 == 0)
            return false;

          std::shared_ptr<T::ContentInfoList> contentList2 = std::make_shared<T::ContentInfoList>();
          time_t fTime2 = 0;

          if (prevContentList2  &&  prevLevel2 == recs[r].level1_2)
          {
            contentList2 = prevContentList2;
            fTime2 = prevTime2;
          }
          else
          {
            fTime2 = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,recs[r].levelId,recs[r].level1_2,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList2);
            prevContentList2 = contentList2;
            prevLevel2 = recs[r].level1_2;
            prevTime2 = fTime2;
          }

          //printf(" * Found %u content records (fTime = %ld)\n", contentList2->getLength(),fTime2);
          //PRINT_DATA(mDebugLog, "         + Found %u content records (fTime = %ld)\n", contentList2->getLength(),fTime2);

          if (fTime2 == 0)
            return false;

          /*
          if (mDebugLog != nullptr &&  mDebugLog->isEnabled()  &&  contentList2->getLength() < 10)
          {
            std::stringstream stream;
            contentList2->print(stream, 0, 4);
            PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
          }
          */

          uint contentLen2 = contentList2->getLength();
          if (contentLen2 == 0)
            return false;

          if (contentLen1 != contentLen2 || fTime1 != fTime2)
            return false;

          T::ContentInfo* contentInfo1_1 = contentList1->getContentInfoByIndex(0);
          T::ContentInfo* contentInfo1_2 = contentList1->getContentInfoByIndex(1);
          T::ContentInfo* contentInfo2_1 = contentList2->getContentInfoByIndex(0);
          T::ContentInfo* contentInfo2_2 = contentList2->getContentInfoByIndex(1);

          valueList.mParameterKeyType = pInfo.mParameterKeyType;
          valueList.mParameterKey = pInfo.mParameterKey;
          valueList.mParameterLevelId = paramLevelId;
          valueList.mForecastTimeUTC = forecastTime;
          valueList.mProducerId = contentInfo1_1->mProducerId;
          valueList.mGenerationId = contentInfo1_1->mGenerationId;
          valueList.mGenerationFlags = generationFlags;
          valueList.mGeometryId = contentInfo1_1->mGeometryId;
          valueList.mFileId[0] = contentInfo1_1->mFileId;
          valueList.mMessageIndex[0] = contentInfo1_1->mMessageIndex;
          valueList.mModificationTime = contentInfo1_1->mModificationTime;
          valueList.mAnalysisTime = analysisTime;
          valueList.mForecastType = contentInfo1_1->mForecastType;
          valueList.mForecastNumber = contentInfo1_1->mForecastNumber;
          valueList.mParameterLevel = paramLevel;
          valueList.mParameterLevelId = contentInfo1_1->mFmiParameterLevelId;

          if (contentLen1 == 1)
          {
            if (contentInfo1_1->mForecastTimeUTC == fTime1  &&  recs[r].time1 == fTime1)
            {
              // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

              valueList.mFlags = ParameterValues::Flags::DataAvailable;

              if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
              {
                T::ParamValue value1;
                int result1 = mDataServerPtr->getGridValueByPoint(0, contentInfo1_1->mFileId, contentInfo1_1->mMessageIndex,query.mCoordinateType,coordinates[r].x(),coordinates[r].y(),areaInterpolationMethod,modificationOperation,modificationParameters,value1);
                if (result1 != 0)
                {
                  Fmi::Exception exception(BCP, "DataServer returns an error!");
                  exception.addParameter("Service", "getGridValueByPoint");
                  exception.addParameter("ErrorMessage", DataServer::getResultString(result1));
                  exception.addParameter("FileId", std::to_string(contentInfo1_1->mFileId));
                  exception.addParameter("MessageIndex", std::to_string(contentInfo1_1->mMessageIndex));
                  std::string errorMsg = exception.getStackTrace();
                  PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
                  return false;
                }

                T::ParamValue value2;
                int result2 = mDataServerPtr->getGridValueByPoint(0, contentInfo2_1->mFileId, contentInfo2_1->mMessageIndex,query.mCoordinateType,coordinates[r].x(),coordinates[r].y(),areaInterpolationMethod,modificationOperation,modificationParameters,value2);
                if (result2 != 0)
                {
                  Fmi::Exception exception(BCP, "DataServer returns an error!");
                  exception.addParameter("Service", "getGridValueByPoint");
                  exception.addParameter("ErrorMessage", DataServer::getResultString(result2));
                  exception.addParameter("FileId", std::to_string(contentInfo2_1->mFileId));
                  exception.addParameter("MessageIndex", std::to_string(contentInfo2_1->mMessageIndex));
                  std::string errorMsg = exception.getStackTrace();
                  PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
                  return false;
                }


                T::ParamValue val = levelInterpolation(value1,value2,recs[r].height1_1,recs[r].height1_2,paramLevel,levelInterpolationMethod);
                valueVector.push_back(val);
              }
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

          if (contentLen1 == 2)
          {
            if (contentInfo1_1->mForecastTimeUTC < fTime1  &&  contentInfo1_2->mForecastTimeUTC > fTime1  &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden)
            {
              // Content records have different times, but most likely the same levels, so we need to do time interpolation.

              valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;

              if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0  &&  recs[r].time1 == contentInfo1_1->mForecastTimeUTC  &&  recs[r].time2 == contentInfo1_2->mForecastTimeUTC)
              {
                T::ParamValue value1_1;
                int result1_1 = mDataServerPtr->getGridValueByPoint(0, contentInfo1_1->mFileId, contentInfo1_1->mMessageIndex,query.mCoordinateType,coordinates[r].x(),coordinates[r].y(),areaInterpolationMethod,modificationOperation,modificationParameters,value1_1);
                if (result1_1 != 0)
                {
                  Fmi::Exception exception(BCP, "DataServer returns an error!");
                  exception.addParameter("Service", "getGridValueByPoint");
                  exception.addParameter("ErrorMessage", DataServer::getResultString(result1_1));
                  exception.addParameter("FileId", std::to_string(contentInfo1_1->mFileId));
                  exception.addParameter("MessageIndex", std::to_string(contentInfo1_1->mMessageIndex));
                  std::string errorMsg = exception.getStackTrace();
                  PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
                  return false;
                }

                T::ParamValue value1_2;
                int result1_2 = mDataServerPtr->getGridValueByPoint(0, contentInfo1_2->mFileId, contentInfo1_2->mMessageIndex,query.mCoordinateType,coordinates[r].x(),coordinates[r].y(),areaInterpolationMethod,modificationOperation,modificationParameters,value1_2);
                if (result1_2 != 0)
                {
                  Fmi::Exception exception(BCP, "DataServer returns an error!");
                  exception.addParameter("Service", "getGridValueByPoint");
                  exception.addParameter("ErrorMessage", DataServer::getResultString(result1_2));
                  exception.addParameter("FileId", std::to_string(contentInfo1_2->mFileId));
                  exception.addParameter("MessageIndex", std::to_string(contentInfo1_2->mMessageIndex));
                  std::string errorMsg = exception.getStackTrace();
                  PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
                  return false;
                }

                T::ParamValue value2_1;
                int result2_1 = mDataServerPtr->getGridValueByPoint(0, contentInfo2_1->mFileId, contentInfo2_1->mMessageIndex,query.mCoordinateType,coordinates[r].x(),coordinates[r].y(),areaInterpolationMethod,modificationOperation,modificationParameters,value2_1);
                if (result2_1 != 0)
                {
                  Fmi::Exception exception(BCP, "DataServer returns an error!");
                  exception.addParameter("Service", "getGridValueByPoint");
                  exception.addParameter("ErrorMessage", DataServer::getResultString(result2_1));
                  exception.addParameter("FileId", std::to_string(contentInfo2_1->mFileId));
                  exception.addParameter("MessageIndex", std::to_string(contentInfo2_1->mMessageIndex));
                  std::string errorMsg = exception.getStackTrace();
                  PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
                  return false;
                }

                T::ParamValue value2_2;
                int result2_2 = mDataServerPtr->getGridValueByPoint(0, contentInfo2_2->mFileId, contentInfo2_2->mMessageIndex,query.mCoordinateType,coordinates[r].x(),coordinates[r].y(),areaInterpolationMethod,modificationOperation,modificationParameters,value2_2);
                if (result2_2 != 0)
                {
                  Fmi::Exception exception(BCP, "DataServer returns an error!");
                  exception.addParameter("Service", "getGridValueByPoint");
                  exception.addParameter("ErrorMessage", DataServer::getResultString(result2_2));
                  exception.addParameter("FileId", std::to_string(contentInfo2_2->mFileId));
                  exception.addParameter("MessageIndex", std::to_string(contentInfo2_2->mMessageIndex));
                  std::string errorMsg = exception.getStackTrace();
                  PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
                  return false;
                }

                T::ParamValue val1 = levelInterpolation(value1_1,value2_1,recs[r].height1_1,recs[r].height1_2,paramLevel,levelInterpolationMethod);
                T::ParamValue val2 = levelInterpolation(value1_2,value2_2,recs[r].height2_1,recs[r].height2_2,paramLevel,levelInterpolationMethod);
                T::ParamValue val = timeInterpolation(val1,val2,recs[r].time1,recs[r].time2,forecastTime,timeInterpolationMethod);

                //printf("levelInterpolation1(%f,%f,%f,%f,%d,%d) = %f\n",value1_1,value2_1,rec.height1_1,rec.height1_2,paramLevel,levelInterpolationMethod,val1);
                //printf("levelInterpolation2(%f,%f,%f,%f,%d,%d) = %f\n",value1_2,value2_2,rec.height2_1,rec.height2_2,paramLevel,levelInterpolationMethod,val2);
                //printf("timeInterpolation(%f,%f,%ld,%ld,%ld,%d) = %f\n",val1,val2,rec.time1,rec.time2,forecastTime,timeInterpolationMethod,val);

                valueVector.push_back(val);
              }
            }
          }
        }
      }
    }

    //printf("VALUES %ld\n",valueVector.size());

    if (conversionByFunction  &&  (qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
       executeConversion(function, functionParams, forecastTime, coordinates, valueVector, valueList.mValueVector);
    else
      valueList.mValueVector = valueVector;

    if (qParam.mPrecision < 0)
      qParam.mPrecision = pInfo.mDefaultPrecision;

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}

#endif






short ServiceImplementation::getLevelInterpolationMethod(T::ParamLevelId paramLevelId,short levelInterpolationMethod,uint flags)
{
  try
  {
    if (levelInterpolationMethod != T::LevelInterpolationMethod::Linear  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Undefined)
      return levelInterpolationMethod;

    SmartMet::Identification::LevelDef levelDef;
    if (SmartMet::Identification::gridDef.getFmiLevelDef(paramLevelId,levelDef))
    {
      switch (levelDef.mType)
      {
        case T::ParamLevelType::Linear:
          return T::LevelInterpolationMethod::Linear;

        case T::ParamLevelType::Logarithmic:
          if (flags & QueryServer::QueryParameter::Flags::PressureLevels)
            return T::LevelInterpolationMethod::Logarithmic;
          else
            return T::LevelInterpolationMethod::Forbidden;

        case T::ParamLevelType::Number:
          return T::LevelInterpolationMethod::Forbidden;
      }
    }
    return T::LevelInterpolationMethod::Nearest;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getPointValues(
    Query& query,
    QueryParameter& qParam,
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    uint& newProducerId,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getPointValues()\n");

    if ((qParam.mFlags & QueryServer::QueryParameter::Flags::MetricLevels) &&  qParam.mLevelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
    {
      return getPointValuesByHeight(query,query.mCoordinateType,query.mAreaCoordinates[0],qParam,producerInfo,producerGeometryId,generationId,analysisTime,generationFlags,pInfo,forecastTime,
          paramLevelId,paramLevel,newProducerId,valueList);
    }


    short areaInterpolationMethod = qParam.mAreaInterpolationMethod;
    if (areaInterpolationMethod == T::AreaInterpolationMethod::Undefined)
      areaInterpolationMethod = pInfo.mAreaInterpolationMethod;

    short timeInterpolationMethod = qParam.mTimeInterpolationMethod;
    if (timeInterpolationMethod == T::TimeInterpolationMethod::Undefined)
      timeInterpolationMethod = pInfo.mTimeInterpolationMethod;

    //if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
    //  levelInterpolationMethod = pInfo.mLevelInterpolationMethod;

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
        paramLevelId,paramLevel,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList);

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

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC == fTime)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        valueList.mFlags = ParameterValues::Flags::DataAvailable;

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByPointList(0, contentInfo1->mFileId, contentInfo1->mMessageIndex,query.mCoordinateType,query.mAreaCoordinates[0],areaInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByPointList");
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex", std::to_string(contentInfo1->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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


    short levelInterpolationMethod = getLevelInterpolationMethod(paramLevelId,qParam.mLevelInterpolationMethod,qParam.mFlags);

    if (contentLen == 2)
    {
      if (contentInfo1->mForecastTimeUTC == fTime  &&  contentInfo2->mForecastTimeUTC == fTime  &&
          contentInfo1->mParameterLevel < paramLevel  &&  contentInfo2->mParameterLevel > paramLevel  &&
          levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
      {
        // Content records have the same time but different levels, so we need to level interpolation.

        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByLevelInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mParameterLevel = paramLevel;

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByLevelAndPointList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,query.mCoordinateType,query.mAreaCoordinates[0],areaInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByLevelAndPointList");
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

        return true;
      }


      if (contentInfo1->mForecastTimeUTC < fTime  &&  contentInfo2->mForecastTimeUTC > fTime  &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden)
      {
        // Content records have different times, but most likely the same levels, so we need to do time interpolation.

        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByTimeAndPointList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,query.mCoordinateType,query.mAreaCoordinates[0],areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByTimeAndPointList");
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByTimeLevelAndPointList(0,
              contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,
              contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,
              fTime,paramLevel,query.mCoordinateType,query.mAreaCoordinates[0],areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList);

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByLevelAndPointList");
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            exception.addParameter("FileId3", std::to_string(contentInfo3->mFileId));
            exception.addParameter("MessageIndex3", std::to_string(contentInfo3->mMessageIndex));
            exception.addParameter("FileId4", std::to_string(contentInfo4->mFileId));
            exception.addParameter("MessageIndex4", std::to_string(contentInfo4->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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
    Query& query,
    QueryParameter& qParam,
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    uchar coordinateType,
    double x,
    double y,
    double radius,
    uint& newProducerId,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getCircleValues()\n");

    short areaInterpolationMethod = qParam.mAreaInterpolationMethod;
    if (areaInterpolationMethod == T::AreaInterpolationMethod::Undefined)
      areaInterpolationMethod = pInfo.mAreaInterpolationMethod;

    short timeInterpolationMethod = qParam.mTimeInterpolationMethod;
    if (timeInterpolationMethod == T::TimeInterpolationMethod::Undefined)
      timeInterpolationMethod = pInfo.mTimeInterpolationMethod;

    //if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
    //  levelInterpolationMethod = pInfo.mLevelInterpolationMethod;

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
        paramLevelId,paramLevel,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList);

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByCircle(0, contentInfo1->mFileId, contentInfo1->mMessageIndex, coordinateType, x, y, radius,modificationOperation,modificationParameters, valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByCircle");
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex", std::to_string(contentInfo1->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

    short levelInterpolationMethod = getLevelInterpolationMethod(paramLevelId,qParam.mLevelInterpolationMethod,qParam.mFlags);

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          // Fetching data from the grids.
          int result = mDataServerPtr->getGridValueListByTimeAndCircle(0,contentInfo1->mFileId,contentInfo1->mMessageIndex,contentInfo2->mFileId,contentInfo2->mMessageIndex,fTime,coordinateType,x,y,radius,timeInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByTimeAndCircle");
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex", std::to_string(contentInfo1->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByLevelAndCircle(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,coordinateType,x,y,radius,levelInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByLevelAndCircle");
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByTimeLevelAndCircle(0,
              contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,
              contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,
              fTime,paramLevel,coordinateType,x,y,radius,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList);

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByTimeLevelAndCircle");
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            exception.addParameter("FileId3", std::to_string(contentInfo3->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo3->mMessageIndex));
            exception.addParameter("FileId4", std::to_string(contentInfo4->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo4->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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
    Query& query,
    QueryParameter& qParam,
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    uint& newProducerId,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getPolygonValues()\n");

    if ((qParam.mFlags & QueryServer::QueryParameter::Flags::MetricLevels) &&  qParam.mLevelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
    {
      GRIB2::GridDef_sptr def = Identification::gridDef.getGrib2DefinitionByGeometryId(producerGeometryId);
      if (!def)
        return false;

      T::Dimensions d = def->getGridDimensions();
      uint cols = d.nx();
      uint rows = d.ny();

      switch (query.mCoordinateType)
      {
        case T::CoordinateTypeValue::UNKNOWN:
        case T::CoordinateTypeValue::LATLON_COORDINATES:
        {
          std::vector<std::vector<T::Coordinate>> newPolygonPath;

          for (auto polygonPoints = query.mAreaCoordinates.begin(); polygonPoints != query.mAreaCoordinates.end(); ++polygonPoints)
          {
            std::vector<T::Coordinate> newPolygonPoints;

            for (auto it = polygonPoints->begin(); it != polygonPoints->end(); ++it)
            {
              double grid_i = 0;
              double grid_j = 0;
              def->getGridPointByLatLonCoordinates(it->y(),it->x(),grid_i,grid_j);
              newPolygonPoints.emplace_back(grid_i,grid_j);
            }
            newPolygonPath.emplace_back(newPolygonPoints);
          }

          std::vector<T::Point> gridPoints;
          getPointsInsidePolygonPath(cols,rows,newPolygonPath,gridPoints);

          T::Coordinate_vec coordinates;
          for (auto it=gridPoints.begin(); it != gridPoints.end(); ++it)
          {
            double lat = 0;
            double lon = 0;
            if (def->getGridLatLonCoordinatesByGridPoint(it->x(),it->y(),lat,lon))
            {
              coordinates.emplace_back(lon,lat);
            }
          }
          return getPointValuesByHeight(query,query.mCoordinateType,coordinates,qParam,producerInfo,producerGeometryId,generationId,analysisTime,generationFlags,pInfo,forecastTime,
             paramLevelId,paramLevel,newProducerId,valueList);
        }
        break;


        case T::CoordinateTypeValue::GRID_COORDINATES:
        {
          std::vector<T::Point> gridPoints;
          getPointsInsidePolygonPath(cols,rows,query.mAreaCoordinates,gridPoints);

          T::Coordinate_vec coordinates;
          for (auto it=gridPoints.begin(); it != gridPoints.end(); ++it)
          {
            double lat = 0;
            double lon = 0;
            if (def->getGridLatLonCoordinatesByGridPoint(it->x(),it->y(),lat,lon))
            {
              coordinates.emplace_back(lon,lat);
            }
          }
          return getPointValuesByHeight(query,query.mCoordinateType,coordinates,qParam,producerInfo,producerGeometryId,generationId,analysisTime,generationFlags,pInfo,forecastTime,
             paramLevelId,paramLevel,newProducerId,valueList);
        }
        break;


        case T::CoordinateTypeValue::ORIGINAL_COORDINATES:
        {
          std::vector<std::vector<T::Coordinate>> newPolygonPath;

          for (auto polygonPoints = query.mAreaCoordinates.begin(); polygonPoints != query.mAreaCoordinates.end(); ++polygonPoints)
          {
            std::vector<T::Coordinate> newPolygonPoints;

            for (auto it = polygonPoints->begin(); it != polygonPoints->end(); ++it)
            {
              double grid_i = 0;
              double grid_j = 0;
              def->getGridPointByOriginalCoordinates(it->x(),it->y(),grid_i,grid_j);
              newPolygonPoints.emplace_back(grid_i,grid_j);
            }
            newPolygonPath.emplace_back(newPolygonPoints);
          }

          std::vector<T::Point> gridPoints;
          getPointsInsidePolygonPath(cols,rows,newPolygonPath,gridPoints);

          T::Coordinate_vec coordinates;
          for (auto it=gridPoints.begin(); it != gridPoints.end(); ++it)
          {
            double lat = 0;
            double lon = 0;
            if (def->getGridLatLonCoordinatesByGridPoint(it->x(),it->y(),lat,lon))
            {
              coordinates.emplace_back(lon,lat);
            }
          }
          return getPointValuesByHeight(query,query.mCoordinateType,coordinates,qParam,producerInfo,producerGeometryId,generationId,analysisTime,generationFlags,pInfo,forecastTime,
             paramLevelId,paramLevel,newProducerId,valueList);
        }
        break;
      }

      return true;
    }


    short areaInterpolationMethod = qParam.mAreaInterpolationMethod;
    if (areaInterpolationMethod == T::AreaInterpolationMethod::Undefined)
      areaInterpolationMethod = pInfo.mAreaInterpolationMethod;

    short timeInterpolationMethod = qParam.mTimeInterpolationMethod;
    if (timeInterpolationMethod == T::TimeInterpolationMethod::Undefined)
      timeInterpolationMethod = pInfo.mTimeInterpolationMethod;

    //short levelInterpolationMethod = qParam.mLevelInterpolationMethod;
    //if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
    //  levelInterpolationMethod = pInfo.mLevelInterpolationMethod;

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
        paramLevelId,paramLevel,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList);

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByPolygonPath(0, contentInfo1->mFileId, contentInfo1->mMessageIndex, query.mCoordinateType, query.mAreaCoordinates,modificationOperation,modificationParameters,valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByPolygonPath");
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex", std::to_string(contentInfo1->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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


    short levelInterpolationMethod = getLevelInterpolationMethod(paramLevelId,qParam.mLevelInterpolationMethod,qParam.mFlags);

    if (contentLen == 2)
    {
      if (contentInfo1->mForecastTimeUTC < fTime  &&  contentInfo2->mForecastTimeUTC > fTime  &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden)
      {
        // Content records have different times, but most likely the same levels, so we need to do time interpolation.

        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByTimeAndPolygonPath(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,query.mCoordinateType,query.mAreaCoordinates,timeInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByTimeAndPolygonPath");
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByLevelAndPolygonPath(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,query.mCoordinateType,query.mAreaCoordinates,levelInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList);
          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByLevelAndPolygonPath");
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = mDataServerPtr->getGridValueListByTimeLevelAndPolygonPath(0,
              contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,
              contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,
              fTime,paramLevel,query.mCoordinateType,query.mAreaCoordinates,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList);

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridValueListByTimeLevelAndPolygonPath");
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            exception.addParameter("FileId3", std::to_string(contentInfo3->mFileId));
            exception.addParameter("MessageIndex3", std::to_string(contentInfo3->mMessageIndex));
            exception.addParameter("FileId4", std::to_string(contentInfo4->mFileId));
            exception.addParameter("MessageIndex4", std::to_string(contentInfo4->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            return false;
          }

          if (conversionByFunction)
            executeConversion(function, functionParams, forecastTime, valueList.mValueList);
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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





bool ServiceImplementation::getStreamlineValues(
    Query& query,
    QueryParameter& qParam,
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    //uchar locationType,
    //uchar coordinateType,
    //T::Coordinate_vec& gridCoordinates,
    //T::AttributeList& queryAttributeList,
    uint& newProducerId,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getStreamlineValues()\n");

    short areaInterpolationMethod = qParam.mAreaInterpolationMethod;
    if (areaInterpolationMethod == T::AreaInterpolationMethod::Undefined)
      areaInterpolationMethod = pInfo.mAreaInterpolationMethod;

    short timeInterpolationMethod = qParam.mTimeInterpolationMethod;
    if (timeInterpolationMethod == T::TimeInterpolationMethod::Undefined)
      timeInterpolationMethod = pInfo.mTimeInterpolationMethod;

    short levelInterpolationMethod = qParam.mLevelInterpolationMethod;
    if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
      levelInterpolationMethod = pInfo.mLevelInterpolationMethod;

    uint modificationOperation = 0;
    double_vec modificationParameters;

    const char *gridWidthStr = query.mAttributeList.getAttributeValue("grid.width");
    const char *gridHeightStr = query.mAttributeList.getAttributeValue("grid.height");

    std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
    time_t fTime = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,
        paramLevelId,paramLevel,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList);

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

    query.mAttributeList.setAttribute("grid.timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
    query.mAttributeList.setAttribute("grid.areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));


    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC == fTime)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        valueList.mFlags = ParameterValues::Flags::DataAvailable;

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;

          switch (qParam.mLocationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridStreamlinesByGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridStreamlinesByGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,toUInt32(gridWidthStr),toUInt32(gridHeightStr),query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridStreamlines(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridStreamlinesByGeometry / getGridStreamlinesByGrid / getGridStreamlines");
            exception.addParameter("LocationType",Fmi::to_string(qParam.mLocationType));
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex", std::to_string(contentInfo1->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;
          switch (qParam.mLocationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridStreamlinesByTimeAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridStreamlinesByTimeAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,toUInt32(gridWidthStr),toUInt32(gridHeightStr),query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridStreamlinesByTime(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridStreamlinesByTimeAndGeometry / getGridStreamlinesByTimeAndGrid / getGridStreamlinesByTime");
            exception.addParameter("LocationType",Fmi::to_string(qParam.mLocationType));
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;
          switch (qParam.mLocationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridStreamlinesByLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridStreamlinesByLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,toUInt32(gridWidthStr),toUInt32(gridHeightStr),query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridStreamlinesByLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridStreamlinesByLevelAndGeometry / getGridStreamlinesByLevelAndGrid / getGridStreamlinesByLevel");
            exception.addParameter("LocationType",Fmi::to_string(qParam.mLocationType));
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
        }

        valueList.mParameterLevel = paramLevel;

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;
          switch (qParam.mLocationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridStreamlinesByTimeLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridStreamlinesByTimeLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,toUInt32(gridWidthStr),toUInt32(gridHeightStr),query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridStreamlinesByTimeAndLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridStreamlinesByTimeLevelAndGeometry / getGridStreamlinesByTimeLevelAndGrid / getGridStreamlinesByTimeAndLevel");
            exception.addParameter("LocationType",Fmi::to_string(qParam.mLocationType));
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            exception.addParameter("FileId3", std::to_string(contentInfo3->mFileId));
            exception.addParameter("MessageIndex3", std::to_string(contentInfo3->mMessageIndex));
            exception.addParameter("FileId4", std::to_string(contentInfo4->mFileId));
            exception.addParameter("MessageIndex4", std::to_string(contentInfo4->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
        }

        valueList.mParameterLevel = paramLevel;

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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
    Query& query,
    QueryParameter& qParam,
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    uint& newProducerId,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getIsolineValues()\n");

    // ### Converting contour values if necessary:

    short areaInterpolationMethod = qParam.mAreaInterpolationMethod;
    if (areaInterpolationMethod == T::AreaInterpolationMethod::Undefined)
      areaInterpolationMethod = pInfo.mAreaInterpolationMethod;

    short timeInterpolationMethod = qParam.mTimeInterpolationMethod;
    if (timeInterpolationMethod == T::TimeInterpolationMethod::Undefined)
      timeInterpolationMethod = pInfo.mTimeInterpolationMethod;
/*
    short levelInterpolationMethod = qParam.mLevelInterpolationMethod;
    if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
      levelInterpolationMethod = pInfo.mLevelInterpolationMethod;
*/
    T::ParamValue_vec newContourValues;

    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    if (conversionFunction(pInfo.mReverseConversionFunction, function, functionParams))
      executeConversion(function,functionParams,qParam.mContourLowValues,newContourValues);
    else
      newContourValues = qParam.mContourLowValues;

    const char *gridWidthStr = query.mAttributeList.getAttributeValue("grid.width");
    const char *gridHeightStr = query.mAttributeList.getAttributeValue("grid.height");
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
        paramLevelId,paramLevel,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList);

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

    query.mAttributeList.setAttribute("grid.timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
    query.mAttributeList.setAttribute("grid.areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));


    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC == fTime)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        valueList.mFlags = ParameterValues::Flags::DataAvailable;

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;

          switch (qParam.mLocationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridIsolinesByGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridIsolinesByGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridIsolines(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridIsolinesByGeometry / getGridIsolinesByGrid / getGridIsolines");
            exception.addParameter("LocationType",Fmi::to_string(qParam.mLocationType));
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex", std::to_string(contentInfo1->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

    short levelInterpolationMethod = getLevelInterpolationMethod(paramLevelId,qParam.mLevelInterpolationMethod,qParam.mFlags);
    query.mAttributeList.setAttribute("grid.levelInterpolationMethod",Fmi::to_string(levelInterpolationMethod));

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;
          switch (qParam.mLocationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridIsolinesByTimeAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridIsolinesByTimeAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridIsolinesByTime(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridIsolinesByTimeAndGeometry / getGridIsolinesByTimeAndGrid / getGridIsolinesByTime");
            exception.addParameter("LocationType",Fmi::to_string(qParam.mLocationType));
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;
          switch (qParam.mLocationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridIsolinesByLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridIsolinesByLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridIsolinesByLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridIsolinesByLevelAndGeometry / getGridIsolinesByLevelAndGrid / getGridIsolinesByLevel");
            exception.addParameter("LocationType",Fmi::to_string(qParam.mLocationType));
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
        }

        valueList.mParameterLevel = paramLevel;

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;
          switch (qParam.mLocationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridIsolinesByTimeLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridIsolinesByTimeLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridIsolinesByTimeAndLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridIsolinesByTimeLevelAndGeometry / getGridIsolinesByTimeLevelAndGrid / getGridIsolinesByTimeAndLevel");
            exception.addParameter("LocationType",Fmi::to_string(qParam.mLocationType));
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            exception.addParameter("FileId3", std::to_string(contentInfo3->mFileId));
            exception.addParameter("MessageIndex3", std::to_string(contentInfo3->mMessageIndex));
            exception.addParameter("FileId4", std::to_string(contentInfo4->mFileId));
            exception.addParameter("MessageIndex4", std::to_string(contentInfo4->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
        }

        valueList.mParameterLevel = paramLevel;

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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
    Query& query,
    QueryParameter& qParam,
    T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    uint generationId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    uint& newProducerId,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getIsobandValues()\n");

    short areaInterpolationMethod = qParam.mAreaInterpolationMethod;
    if (areaInterpolationMethod == T::AreaInterpolationMethod::Undefined)
      areaInterpolationMethod = pInfo.mAreaInterpolationMethod;

    short timeInterpolationMethod = qParam.mTimeInterpolationMethod;
    if (timeInterpolationMethod == T::TimeInterpolationMethod::Undefined)
      timeInterpolationMethod = pInfo.mTimeInterpolationMethod;
/*
    short levelInterpolationMethod = qParam.mLevelInterpolationMethod;
    if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
      levelInterpolationMethod = pInfo.mLevelInterpolationMethod;
*/
    // ### Converting contour values if necessary:

    T::ParamValue_vec newContourLowValues;
    T::ParamValue_vec newContourHighValues;

    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    if (conversionFunction(pInfo.mReverseConversionFunction, function, functionParams))
    {
      executeConversion(function,functionParams,qParam.mContourLowValues,newContourLowValues);
      executeConversion(function,functionParams,qParam.mContourHighValues,newContourHighValues);
    }
    else
    {
      newContourLowValues = qParam.mContourLowValues;
      newContourHighValues = qParam.mContourHighValues;
    }

    const char *gridWidthStr = query.mAttributeList.getAttributeValue("grid.width");
    const char *gridHeightStr = query.mAttributeList.getAttributeValue("grid.height");

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
        paramLevelId,paramLevel,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList);

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

    query.mAttributeList.setAttribute("grid.timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
    query.mAttributeList.setAttribute("grid.areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC == fTime)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        valueList.mFlags = ParameterValues::Flags::DataAvailable;

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;

          switch (qParam.mLocationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridIsobandsByGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourLowValues,newContourHighValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridIsobandsByGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourLowValues,newContourHighValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridIsobands(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourLowValues,newContourHighValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridIsobandsByGeometry / getGridIsobandsByGrid / getGridIsobands");
            exception.addParameter("LocationType",Fmi::to_string(qParam.mLocationType));
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex", std::to_string(contentInfo1->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

    short levelInterpolationMethod = getLevelInterpolationMethod(paramLevelId,qParam.mLevelInterpolationMethod,qParam.mFlags);
    query.mAttributeList.setAttribute("grid.levelInterpolationMethod",Fmi::to_string(levelInterpolationMethod));

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;
          switch (qParam.mLocationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridIsobandsByTimeAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourLowValues,newContourHighValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridIsobandsByTimeAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourLowValues,newContourHighValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridIsobandsByTime(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourLowValues,newContourHighValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridIsobandsByTimeAndGeometry / getGridIsobandsByTimeAndGrid / getGridIsobandsByTime");
            exception.addParameter("LocationType",Fmi::to_string(qParam.mLocationType));
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
        }

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;
          switch (qParam.mLocationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridIsobandsByLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourLowValues,newContourHighValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridIsobandsByLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourLowValues,newContourHighValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridIsobandsByLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourLowValues,newContourHighValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridIsobandsByLevelAndGeometry / getGridIsobandsByLevelAndGrid / getGridIsobandsByLevel");
            exception.addParameter("LocationType",Fmi::to_string(qParam.mLocationType));
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
        }

        valueList.mParameterLevel = paramLevel;

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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

        if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
        {
          int result = 0;
          switch (qParam.mLocationType)
          {
            case QueryParameter::LocationType::Geometry:
              result = mDataServerPtr->getGridIsobandsByTimeLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourLowValues,newContourHighValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            case QueryParameter::LocationType::Grid:
              if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
                result = mDataServerPtr->getGridIsobandsByTimeLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourLowValues,newContourHighValues,toUInt32(gridWidthStr),toUInt32(gridHeightStr),query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;

            default:
              result = mDataServerPtr->getGridIsobandsByTimeAndLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourLowValues,newContourHighValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData);
              break;
          }

          if (result != 0)
          {
            Fmi::Exception exception(BCP, "DataServer returns an error!");
            exception.addParameter("Service", "getGridIsobandsByTimeLevelAndGeometry / getGridIsobandsByTimeLevelAndGrid / getGridIsobandsByTimeAndLevel");
            exception.addParameter("LocationType",Fmi::to_string(qParam.mLocationType));
            exception.addParameter("ErrorMessage", DataServer::getResultString(result));
            exception.addParameter("FileId1", std::to_string(contentInfo1->mFileId));
            exception.addParameter("MessageIndex1", std::to_string(contentInfo1->mMessageIndex));
            exception.addParameter("FileId2", std::to_string(contentInfo2->mFileId));
            exception.addParameter("MessageIndex2", std::to_string(contentInfo2->mMessageIndex));
            exception.addParameter("FileId3", std::to_string(contentInfo3->mFileId));
            exception.addParameter("MessageIndex3", std::to_string(contentInfo3->mMessageIndex));
            exception.addParameter("FileId4", std::to_string(contentInfo4->mFileId));
            exception.addParameter("MessageIndex4", std::to_string(contentInfo4->mMessageIndex));
            std::string errorMsg = exception.getStackTrace();
            PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
            // throw exception;
          }
        }

        valueList.mParameterLevel = paramLevel;

        if (qParam.mPrecision < 0)
          qParam.mPrecision = pInfo.mDefaultPrecision;

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
    Query& query,
    QueryParameter& qParam,
    Producer_vec& producers,
    std::set<T::GeometryId>& geometryIdList,
    uint producerId,
    const std::string& analysisTime,
    const std::string& maxAnalysisTime,
    ulonglong generationFlags,
    const std::string& parameterKey,
    std::size_t parameterHash,
    uint queryFlags,
    time_t forecastTime,
    bool timeMatchRequired,
    ParameterValues& valueList,
    std::string& producerStr)
{
  FUNCTION_TRACE
  try
  {
    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      PRINT_DATA(mDebugLog, "\nMETHOD getGridValues()\n");
      PRINT_DATA(mDebugLog, "  - producers                : %lu items\n", producers.size());
      for (auto it = producers.begin(); it != producers.end(); ++it)
        PRINT_DATA(mDebugLog, "    * %s:%d\n", it->first.c_str(), it->second);
      PRINT_DATA(mDebugLog, "  - geometryIdList           : %lu items\n", geometryIdList.size());
      for (auto it = geometryIdList.begin(); it != geometryIdList.end(); ++it)
        PRINT_DATA(mDebugLog, "    * %d\n",* it);
      PRINT_DATA(mDebugLog, "  - producerId               : %u\n", producerId);
      PRINT_DATA(mDebugLog, "  - analysisTime             : %s\n", analysisTime.c_str());
      PRINT_DATA(mDebugLog, "  - maxAnalysisTime          : %s\n", maxAnalysisTime.c_str());
      PRINT_DATA(mDebugLog, "  - generationFlags          : %llu\n", generationFlags);
      PRINT_DATA(mDebugLog, "  - parameterKey             : %s\n", parameterKey.c_str());
      PRINT_DATA(mDebugLog, "  - queryFlags               : %04x\n", queryFlags);
      PRINT_DATA(mDebugLog, "  - forecastTime             : %s\n", utcTimeFromTimeT(forecastTime).c_str());
      PRINT_DATA(mDebugLog, "  - forecastTimeT            : %ld\n", forecastTime);
      PRINT_DATA(mDebugLog, "  - timeMatchRequired        : %d\n", timeMatchRequired);
    }

    if ((query.mProcessingStartTime + query.mMaxProcessingTimeInSeconds) < time(nullptr))
    {
      Fmi::Exception exception(BCP, "Maximum processing time reached - processing halted!");
      exception.addParameter("mMaxProcessingTimeInSeconds", std::to_string(query.mMaxProcessingTimeInSeconds));
      throw exception;
    }

    if (qParam.mType != QueryParameter::Type::GridFile  &&  query.mAreaCoordinates.size() == 0  &&
        query.mAttributeList.getAttributeValue("grid.geometryId") == nullptr  &&
        query.mAttributeList.getAttributeValue("grid.geometryString") == nullptr  &&
        query.mAttributeList.getAttributeValue("grid.urn") == nullptr  &&
        query.mAttributeList.getAttributeValue("grid.crs") == nullptr)
    {
      Fmi::Exception exception(BCP, "No query.mAreaCoordinates defined!");
      throw exception;
    }

    if (qParam.mLocationType == QueryParameter::LocationType::Polygon)
    {
      for (size_t t = 0; t < query.mAreaCoordinates.size(); t++)
      {
        if (query.mAreaCoordinates[t].size() < 3)
        {
          Fmi::Exception exception(BCP, "Polygon definition requires at least 3 query.mAreaCoordinates!");
          throw exception;
        }
      }
    }

    time_t requiredAccessTime = time(nullptr) + 120;

    bool acceptNotReadyGenerations = false;
    if (query.mFlags & Query::Flags::AcceptNotReadyGenerations)
      acceptNotReadyGenerations = true;

    // Going through the producer list.

    for (auto it = producers.begin(); it != producers.end(); ++it)
    {
      std::string producerName = it->first;
      T::GeometryId producerGeometryId = it->second;

      PRINT_DATA(mDebugLog, "  - Producer and Geometry : %s:%d\n", producerName.c_str(), producerGeometryId);

      if (geometryIdList.find(producerGeometryId) != geometryIdList.end())
      {
        bool producerOk = true;
        if (producers.size() > 1 && (qParam.mLocationType == QueryParameter::LocationType::Polygon || qParam.mLocationType == QueryParameter::LocationType::Circle || qParam.mLocationType == QueryParameter::LocationType::Path))
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

        // The current producer supports a geometry where the current query.mAreaCoordinates can be found.

        T::ProducerInfo producerInfo;
        if (producerOk &&  getProducerInfoByName(producerName, producerInfo) && (producerId == 0 || producerInfo.mProducerId == producerId))
        {
          PRINT_DATA(mDebugLog, "  - The producer and the geometry are acceptable!\n");

          // Reading generations supported by the current producer.

          auto cacheEntry = getGenerationInfoListByProducerId(producerInfo.mProducerId,producerInfo.mHash,true);

          uint gLen = cacheEntry->generationInfoList->getLength();
          if (gLen == 0)
            PRINT_DATA(mDebugLog, "    - No generations found for the current producer!\n");

          // Sorting generation analysis times.

          std::vector<std::string> analysisTimesVec;
          std::vector<std::string> *analysisTimes = cacheEntry->analysisTimes.get();
          if (queryFlags & Query::Flags::ReverseGenerationFlags)
          {
            cacheEntry->generationInfoList->getAnalysisTimes(analysisTimesVec,false);
            analysisTimes = &analysisTimesVec;
          }


          // Going through all the parameter mapping files, until we find a match.

          ParameterMapping_vec_sptr mappings;
          if (qParam.mParameterLevelId > 0 || qParam.mParameterLevel > 0)
          {
            getParameterMappings(producerInfo.mName,producerInfo.mProducerId,parameterKey,parameterHash,producerGeometryId, qParam.mParameterLevelId, qParam.mParameterLevel, false, mappings);
            if (mappings->size() == 0)
            {
              // Getting a mapping by using the levelId. This probably returns all levels belonging
              // to the current levelId. We need just one for mapping.
              ParameterMapping_vec_sptr tmpMappings;
              getParameterMappings(producerInfo.mName, producerInfo.mProducerId, parameterKey, parameterHash, producerGeometryId, qParam.mParameterLevelId, -1,false, tmpMappings);
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
            mp.mParameterLevelId = qParam.mParameterLevelId;
            mp.mParameterLevel = qParam.mParameterLevel;
            mappings->emplace_back(mp);
          }

          if (mappings->size() == 0)
            PRINT_DATA(mDebugLog, "    - No parameter mappings '%s:%s:%d:%d:%d' found!\n", producerInfo.mName.c_str(), parameterKey.c_str(), producerGeometryId, qParam.mParameterLevelId, qParam.mParameterLevel);


          T::GenerationInfo* origGenerationInfo = nullptr;
          if (!analysisTime.empty())
          {
            origGenerationInfo = cacheEntry->generationInfoList->getGenerationInfoByAnalysisTime(analysisTime);
            if (mDebugLog != nullptr &&  mDebugLog->isEnabled()  &&  origGenerationInfo)
            {
              PRINT_DATA(mDebugLog, "    - generation found by analysis time (%s)!\n",analysisTime.c_str());
            }
          }

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

            uint gLen = analysisTimes->size();

            PRINT_DATA(mDebugLog, "  - Going through the generations from the newest to the oldest (generationCount = %u).\n",gLen);

            for (uint g = 0; g < gLen; g++)
            {
              bool generationValid = false;

              if (origGenerationInfo != nullptr)
              {
                generationValid = true;
                g = gLen; // Making sure that the loop is executed only once
                PRINT_DATA(mDebugLog, "    * %s (requested generation)\n", analysisTime.c_str());
              }
              else
              {
                PRINT_DATA(mDebugLog, "    * %s\n", (*analysisTimes)[g].c_str());
              }

              uint gflags = 1 << g;

              if (!analysisTime.empty()  &&  origGenerationInfo == nullptr)
              {
                if (analysisTime == (*analysisTimes)[g])
                {
                  PRINT_DATA(mDebugLog, "      - Analysis time equals\n");
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
              {
                PRINT_DATA(mDebugLog, "      - Generation deletion time is close\n");
                generationValid = false;
              }

              if (generationValid  &&  generationInfo != nullptr  &&  !acceptNotReadyGenerations)
              {
                // Checking if the geometry is ready.
                if (mCheckGeometryStatus)
                {
                  if (!isGeometryReady(generationInfo->mGenerationId,producerGeometryId,0))
                  {
                    PRINT_DATA(mDebugLog, "      - Geometry is not ready\n");
                    generationValid = false;
                  }
                }
                else
                {
                  if (generationInfo->mStatus != T::GenerationInfo::Status::Ready)
                  {
                    PRINT_DATA(mDebugLog, "      - Generation is not ready\n");
                    generationValid = false;
                  }
                }
              }

              if (generationValid  &&  !maxAnalysisTime.empty()  &&  generationInfo->mAnalysisTime > maxAnalysisTime)
              {
                PRINT_DATA(mDebugLog, "      - Generation analysisTime > maxAnalysisTime (%s)\n",maxAnalysisTime.c_str());
                generationValid = false;
              }

              if (generationValid  &&  generationInfo != nullptr  &&  !climatologicalMappings  &&  producerId != 0)
              {
                // Checking if the requested forecast time is outside of the generation's time range.

                if (generationInfo->mContentStartTime == 0 || generationInfo->mContentEndTime == 0)
                  getGenerationTimeRangeByProducerAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,generationInfo->mContentStartTime,generationInfo->mContentEndTime);

                if (forecastTime < generationInfo->mContentStartTime || forecastTime > generationInfo->mContentEndTime)
                {
                  PRINT_DATA(mDebugLog, "      - Requested time is outside of the generation's time range\n");
                  generationValid = false;
                }
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
                  short areaInterpolation = qParam.mAreaInterpolationMethod;
                  if (areaInterpolation == T::AreaInterpolationMethod::Undefined)
                    areaInterpolation = pInfo->mAreaInterpolationMethod;

                  /*
                  short timeInterpolation = qParam.mTimeInterpolationMethod;
                  if (timeInterpolation == T::TimeInterpolationMethod::Undefined)
                    timeInterpolation = pInfo->mTimeInterpolationMethod;

                  short levelInterpolation = qParam.mLevelInterpolationMethod;
                  if (levelInterpolation == T::LevelInterpolationMethod::Undefined)
                    levelInterpolation = pInfo->mLevelInterpolationMethod;
*/
                  T::ParamLevelId pLevelId = pInfo->mParameterLevelId;
                  T::ParamLevel pLevel = pInfo->mParameterLevel;

                  if (qParam.mParameterLevelId > 0)
                  {
                    pLevelId = qParam.mParameterLevelId;
                    if (qParam.mParameterLevel >= 0)
                      pLevel = qParam.mParameterLevel;
                  }

                  if (qParam.mParameterLevel > 0)
                    pLevel = qParam.mParameterLevel;


                  // ### QUERY TYPE : PointValues

                  if (qParam.mType == QueryParameter::Type::PointValues)
                  {

                    // ### LOCATION TYPE : Polygon

                    if (qParam.mLocationType == QueryParameter::LocationType::Polygon)
                    {
                      // ### Simple interpolation

                      if (getPolygonValues(query,qParam,producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags,
                          *pInfo, forecastTime, pLevelId, pLevel,producerId, valueList))
                        return;
                    }


                    // ### LOCATION TYPE : Circle

                    if (qParam.mLocationType == QueryParameter::LocationType::Circle && query.mAreaCoordinates.size() == 1 && query.mAreaCoordinates[0].size() == 1)
                    {
                      // ### Simple interpolation

                      double x = query.mAreaCoordinates[0][0].x();
                      double y = query.mAreaCoordinates[0][0].y();

                      if (getCircleValues(query,qParam,producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo, forecastTime, pLevelId, pLevel,
                          query.mCoordinateType, x, y, query.mRadius, producerId, valueList))
                        return;
                    }


                    // ### LOCATION TYPE : Path

                    if (qParam.mLocationType == QueryParameter::LocationType::Path)
                    {
                      if (areaInterpolation < T::AreaInterpolationMethod::List)
                      {
                        // ### Simple interpolation

                        if (getPointValues(query,qParam,producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags,
                            *pInfo, forecastTime, pLevelId, pLevel, producerId, valueList))
                          return;
                      }

                      if (areaInterpolation >= T::AreaInterpolationMethod::List && areaInterpolation < T::AreaInterpolationMethod::External && query.mAreaCoordinates.size() == 1  && query.mAreaCoordinates[0].size() == 1)
                      {
                        // ### No actual intepolation. Fetching special value for external interpolation

                        double x = query.mAreaCoordinates[0][0].x();
                        double y = query.mAreaCoordinates[0][0].y();

                        if (getSpecialValues(query,qParam,producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo, forecastTime, pLevelId, pLevel, qParam.mForecastType, qParam.mForecastNumber,
                            qParam.mFlags, qParam.mAreaInterpolationMethod, qParam.mTimeInterpolationMethod, qParam.mLevelInterpolationMethod, query.mCoordinateType, x, y, producerId, valueList))
                          return;
                      }
                    }


                    // ### LOCATION TYPE : Point

                    if (qParam.mLocationType == QueryParameter::LocationType::Point)
                    {
                      if (areaInterpolation == T::AreaInterpolationMethod::Landscape && query.mAreaCoordinates.size() == 1 && query.mAreaCoordinates[0].size() == 1)
                      {
                        double x = query.mAreaCoordinates[0][0].x();
                        double y = query.mAreaCoordinates[0][0].y();

                        if (getSpecialValues(query,qParam,producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo, forecastTime, pLevelId, pLevel, qParam.mForecastType, qParam.mForecastNumber,
                            qParam.mFlags, T::AreaInterpolationMethod::List, qParam.mTimeInterpolationMethod, qParam.mLevelInterpolationMethod, query.mCoordinateType, x, y, producerId, valueList))
                        {
                          Producer_vec pList;
                          std::set<T::GeometryId> gList;

                          pList.emplace_back(it->first,it->second);
                          gList.insert(it->second);

                          std::string param = "GeopHeight;" + it->first + ";" + std::to_string(it->second);
                          std::string paramStr;

                          if (!getAlias(param,paramStr))
                            paramStr = param;

                          QueryParameter zhParam = qParam;
                          zhParam.mGeometryId = it->second;
                          zhParam.mAreaInterpolationMethod = 500;

                          getParameterStringInfo(paramStr,';',zhParam.mParam,zhParam.mGeometryId,zhParam.mParameterLevelId,
                              zhParam.mParameterLevel,zhParam.mForecastType,zhParam.mForecastNumber,zhParam.mProducerName,zhParam.mProducerId,zhParam.mGenerationFlags,
                              zhParam.mAreaInterpolationMethod,zhParam.mTimeInterpolationMethod,zhParam.mLevelInterpolationMethod,zhParam.mFlags);

                          if (strcasecmp(zhParam.mProducerName.c_str(),it->first.c_str()) != 0)
                          {
                            pList.emplace_back(zhParam.mProducerName,zhParam.mGeometryId);
                            gList.insert(zhParam.mGeometryId);
                          }

                          ParameterValues zhValueList;
                          getGridValues(query,zhParam,pList,gList,zhParam.mProducerId,analysisTime,maxAnalysisTime,
                            generationFlags,zhParam.mParam,0,
                            queryFlags,forecastTime,timeMatchRequired,zhValueList,producerStr);

                          param = "LapseRate;" + it->first + ";" + std::to_string(it->second);
                          if (!getAlias(param,paramStr))
                            paramStr = param;

                          QueryParameter lrParam = qParam;
                          lrParam.mGeometryId = it->second;
                          lrParam.mAreaInterpolationMethod = 500;

                          getParameterStringInfo(paramStr,';',lrParam.mParam,lrParam.mGeometryId,lrParam.mParameterLevelId,
                              lrParam.mParameterLevel,lrParam.mForecastType,lrParam.mForecastNumber,lrParam.mProducerName,lrParam.mProducerId,lrParam.mGenerationFlags,
                              lrParam.mAreaInterpolationMethod,lrParam.mTimeInterpolationMethod,lrParam.mLevelInterpolationMethod,lrParam.mFlags);

                          if (strcasecmp(lrParam.mProducerName.c_str(),it->first.c_str()) != 0)
                          {
                            pList.emplace_back(lrParam.mProducerName,lrParam.mGeometryId);
                            gList.insert(lrParam.mGeometryId);
                          }


                          ParameterValues lrValueList;
                          getGridValues(query,lrParam,pList,gList,lrParam.mProducerId,
                            analysisTime,maxAnalysisTime,generationFlags,lrParam.mParam,0,
                            queryFlags,forecastTime,timeMatchRequired,lrValueList,producerStr);

                          param = "LandSeaMask;" + it->first + ";" + std::to_string(it->second);
                          if (!getAlias(param,paramStr))
                            paramStr = param;

                          QueryParameter lsParam = qParam;
                          lsParam.mGeometryId = it->second;
                          lsParam.mAreaInterpolationMethod = 500;

                          getParameterStringInfo(paramStr,';',lsParam.mParam,lsParam.mGeometryId,lsParam.mParameterLevelId,
                              lsParam.mParameterLevel,lsParam.mForecastType,lsParam.mForecastNumber,lsParam.mProducerName,lsParam.mProducerId,lsParam.mGenerationFlags,
                              lsParam.mAreaInterpolationMethod,lsParam.mTimeInterpolationMethod,lsParam.mLevelInterpolationMethod,lsParam.mFlags);

                          if (strcasecmp(lsParam.mProducerName.c_str(),it->first.c_str()) != 0)
                          {
                            pList.emplace_back(lsParam.mProducerName,lsParam.mGeometryId);
                            gList.insert(lsParam.mGeometryId);
                          }

                          ParameterValues lsValueList;
                          getGridValues(query,lsParam,pList,gList,lsParam.mProducerId,
                            analysisTime,maxAnalysisTime,generationFlags,lsParam.mParam,0,
                            queryFlags,forecastTime,timeMatchRequired,lsValueList,producerStr);

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

                        if (getPointValues(query,qParam,producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags,
                            *pInfo, forecastTime, pLevelId, pLevel, producerId, valueList))
                          return;
                      }


                      if (areaInterpolation >= T::AreaInterpolationMethod::List && areaInterpolation < T::AreaInterpolationMethod::External && query.mAreaCoordinates.size() == 1 && query.mAreaCoordinates[0].size() == 1)
                      {
                        // ### No actual intepolation. Fetching special value for external interpolation

                        double x = query.mAreaCoordinates[0][0].x();
                        double y = query.mAreaCoordinates[0][0].y();

                        if (getSpecialValues(query,qParam,producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo, forecastTime, pLevelId, pLevel, qParam.mForecastType, qParam.mForecastNumber,
                            qParam.mFlags, qParam.mAreaInterpolationMethod, qParam.mTimeInterpolationMethod, qParam.mLevelInterpolationMethod, query.mCoordinateType, x, y, producerId, valueList))
                          return;
                      }

                    }

                  }

                  if ((qParam.mFlags & QueryServer::QueryParameter::Flags::MetricLevels)  &&  (qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
                  {
                    // This is metric query. We should post-process isobands, isolines, etc.

                    if (getValueVectors(query,qParam,producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo,
                        forecastTime, pLevelId, pLevel,producerId, valueList, qParam.mCoordinates))
                    {
                      return;
                    }
                  }
                  else
                  {

                    // ### STREAMLINE QUERY

                    if (qParam.mType == QueryParameter::Type::StreamLine)
                    {
                      if (getStreamlineValues(query,qParam,producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo, forecastTime, pLevelId, pLevel,
                          producerId, valueList))
                      {
                        return;
                      }
                    }

                    // ### ISOLINE QUERY

                    if (qParam.mType == QueryParameter::Type::Isoline)
                    {
                      if (getIsolineValues(query,qParam,producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags,
                          *pInfo, forecastTime, pLevelId, pLevel, producerId, valueList))
                      {
                        return;
                      }
                    }


                    // ### ISOBAND QUERY

                    if (qParam.mType == QueryParameter::Type::Isoband)
                    {
                      if (getIsobandValues(query,qParam,producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags,
                          *pInfo, forecastTime, pLevelId, pLevel, producerId, valueList))
                      {
                        return;
                      }
                    }

                    // ### VECTOR QUERY

                    if (qParam.mType == QueryParameter::Type::Vector)
                    {
                      if (getValueVectors(query,qParam,producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags, *pInfo,
                          forecastTime, pLevelId, pLevel,producerId, valueList, qParam.mCoordinates))
                      {
                        return;
                      }
                    }

                    // ### GRID FILE QUERY

                    if (qParam.mType == QueryParameter::Type::GridFile)
                    {
                      if (getGridFiles(query,qParam,producerInfo, producerGeometryId, generationInfo->mGenerationId, generationInfo->mAnalysisTime, gflags,
                          *pInfo, forecastTime, pLevelId, pLevel,producerId, valueList))
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
        PRINT_DATA(mDebugLog, "  - Producer's '%s' geometry '%d' does not cover all requested query.mAreaCoordinates!\n", producerName.c_str(), producerGeometryId);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ServiceImplementation::getGridValues(
    Query& query,
    QueryParameter& qParam,
    Producer_vec& producers,
    std::set<T::GeometryId>& geometryIdList,
    uint producerId,
    const std::string& analysisTime,
    ulonglong generationFlags,
    const std::string& parameterKey,
    std::size_t parameterHash,
    uint queryFlags,
    time_t startTime,
    time_t endTime,
    std::string& producerStr,
    uint valueCounter,
    uint recursionCounter)
{
  FUNCTION_TRACE
  try
  {
    if (recursionCounter > 5)
      return;

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      PRINT_DATA(mDebugLog, "\nMETHOD getGridValues()\n");
      PRINT_DATA(mDebugLog, "  - producers                      : %lu items\n", producers.size());
      for (auto it = producers.begin(); it != producers.end(); ++it)
        PRINT_DATA(mDebugLog, "    * %s:%d\n", it->first.c_str(), it->second);
      PRINT_DATA(mDebugLog, "  - geometryIdList                 : %lu items\n", geometryIdList.size());
      for (auto it = geometryIdList.begin(); it != geometryIdList.end(); ++it)
        PRINT_DATA(mDebugLog, "    * %d\n",* it);
      PRINT_DATA(mDebugLog, "  - producerId                     : %u\n", producerId);
      PRINT_DATA(mDebugLog, "  - analysisTime                   : %s\n", analysisTime.c_str());
      PRINT_DATA(mDebugLog, "  - generationFlags                : %llu\n", generationFlags);
      PRINT_DATA(mDebugLog, "  - parameterKey                   : %s\n", parameterKey.c_str());
      PRINT_DATA(mDebugLog, "  - queryFlags                     : %04x\n", queryFlags);
      //PRINT_DATA(mDebugLog, "  - parameterFlags                 : %04x\n", parameterFlags);
      PRINT_DATA(mDebugLog, "  - startTime                      : %s\n", utcTimeFromTimeT(startTime).c_str());
      PRINT_DATA(mDebugLog, "  - endTime                        : %s\n", utcTimeFromTimeT(endTime).c_str());
      PRINT_DATA(mDebugLog, "  - valueCounter                   : %u\n", valueCounter);
      PRINT_DATA(mDebugLog, "  - recursionCounter               : %u\n", recursionCounter);
    }

    if ((query.mProcessingStartTime + query.mMaxProcessingTimeInSeconds) < time(nullptr))
    {
      Fmi::Exception exception(BCP, "Maximum processing time reached - processing halted!");
      exception.addParameter("mMaxProcessingTimeInSeconds", std::to_string(query.mMaxProcessingTimeInSeconds));
      throw exception;
    }

    if (qParam.mType != QueryParameter::Type::GridFile  &&  query.mAreaCoordinates.size() == 0  &&
        query.mAttributeList.getAttributeValue("grid.geometryId") == nullptr  &&
        query.mAttributeList.getAttributeValue("grid.geometryString") == nullptr  &&
        query.mAttributeList.getAttributeValue("grid.urn") == nullptr  &&
        query.mAttributeList.getAttributeValue("grid.crs") == nullptr)
    {
      Fmi::Exception exception(BCP, "No query.mAreaCoordinates defined!");
      throw exception;
    }

    if (qParam.mLocationType == QueryParameter::LocationType::Polygon)
    {
      for (size_t t = 0; t < query.mAreaCoordinates.size(); t++)
      {
        if (query.mAreaCoordinates[t].size() < 3)
        {
          Fmi::Exception exception(BCP, "Area definition requires at least 3 query.mAreaCoordinates!");
          throw exception;
        }
      }
    }

    std::string maxAnalysisTime;

    uint queryTimestepSizeInSeconds = query.mTimestepSizeInMinutes*60;
    uint parameterTimestepSizeInSeconds = qParam.mTimestepSizeInMinutes*60;

    time_t requiredAccessTime = time(nullptr) + 120;

    bool ignoreStartTimeValue = false;
    if ((queryFlags & Query::Flags::StartTimeNotIncluded) != 0)
      ignoreStartTimeValue = true;

    bool timeStepIsData = false;
    if (queryFlags & Query::Flags::TimeStepIsData)
      timeStepIsData = true;

    bool acceptNotReadyGenerations = false;
    if (query.mFlags & Query::Flags::AcceptNotReadyGenerations)
      acceptNotReadyGenerations = true;

    // Going through the producer list.

    for (auto it = producers.begin(); it != producers.end(); ++it)
    {
      std::string producerName = it->first;
      T::GeometryId producerGeometryId = it->second;

      PRINT_DATA(mDebugLog, "  - Producer and Geometry : %s:%d\n", producerName.c_str(), producerGeometryId);

      // Checking if the current geometry can be found from the acceptable geometry list

      if (geometryIdList.find(producerGeometryId) != geometryIdList.end())
      {
        // The current producer supports a geometry where the current query.mAreaCoordinates can be found.

        bool producerOk = true;
        if (producers.size() > 1 && (qParam.mLocationType == QueryParameter::LocationType::Polygon || qParam.mLocationType == QueryParameter::LocationType::Circle || qParam.mLocationType == QueryParameter::LocationType::Path))
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

          auto cacheEntry = getGenerationInfoListByProducerId(producerInfo.mProducerId,producerInfo.mHash,true);
          uint gLen = cacheEntry->generationInfoList->getLength();

          if (gLen > 0)
          {
            ParameterMapping_vec_sptr mappings;
            if (qParam.mParameterLevelId > 0 || qParam.mParameterLevel > 0)
            {
              getParameterMappings(producerInfo.mName, producerInfo.mProducerId, parameterKey, parameterHash, producerGeometryId, qParam.mParameterLevelId, qParam.mParameterLevel, false, mappings);
              if (mappings->size() == 0)
              {
                // Getting a mapping by using the levelId. This probably returns all levels belonging
                // to the current levelId. We need just one for mapping.
                ParameterMapping_vec_sptr tmpMappings;
                getParameterMappings(producerInfo.mName, producerInfo.mProducerId, parameterKey, parameterHash, producerGeometryId, qParam.mParameterLevelId, -1,false, tmpMappings);
                if (tmpMappings->size() > 0)
                {
                  ParameterMapping m((*tmpMappings)[0]);
                  m.mParameterLevel = qParam.mParameterLevel;
                  mappings->emplace_back(m);
                }
              }
            }
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
                        pInfo->mParameterLevelId, pInfo->mParameterLevel, pInfo->mParameterLevel, qParam.mForecastType, qParam.mForecastNumber, producerGeometryId,
                        std::string("19000101T000000"), std::string("30000101T000000"), 0, contentInfoList);

                    if (result != 0)
                    {
                      Fmi::Exception exception(BCP, "ContentServer returns an error!");
                      exception.addParameter("Service", "getContentListByParameterAndGenerationId");
                      exception.addParameter("ErrorMessage", ContentServer::getResultString(result));
                      exception.addParameter("generationId", std::to_string(generationInfo->mGenerationId));
                      exception.addParameter("parameter", pInfo->mParameterKey);
                      exception.addParameter("levelId", std::to_string(pInfo->mParameterLevelId));
                      exception.addParameter("level", std::to_string(pInfo->mParameterLevel));
                      exception.addParameter("geometryId", std::to_string(producerGeometryId));
                      throw exception;
                    }
                  }
                }

                if (generationInfo == nullptr)
                {
                  int result = mContentServerPtr->getContentListByParameterAndProducerId(0, producerInfo.mProducerId, pInfo->mParameterKeyType, pInfo->mParameterKey,
                      pInfo->mParameterLevelId, pInfo->mParameterLevel, pInfo->mParameterLevel, qParam.mForecastType, qParam.mForecastNumber, producerGeometryId,
                      std::string("19000101T000000"), std::string("30000101T000000"), 0, contentInfoList);

                  if (result != 0)
                  {
                    Fmi::Exception exception(BCP, "ContentServer returns an error!");
                    exception.addParameter("Service", "getContentListByParameterAndProducerId");
                    exception.addParameter("ErrorMessage", ContentServer::getResultString(result));
                    exception.addParameter("producerId", std::to_string(producerInfo.mProducerId));
                    exception.addParameter("parameter", pInfo->mParameterKey);
                    exception.addParameter("levelId", std::to_string(pInfo->mParameterLevelId));
                    exception.addParameter("level", std::to_string(pInfo->mParameterLevel));
                    exception.addParameter("geometryId", std::to_string(producerGeometryId));
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

                  if (analysisTime.empty())
                  {
                    T::GenerationInfo *lastGeneration = cacheEntry->generationInfoList->getLastGenerationInfoByContentTime(producerInfo.mProducerId);
                    if (lastGeneration)
                      maxAnalysisTime = lastGeneration->mAnalysisTime;
                  }

                  uint c = 0;
                  uint validCount = 0;
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

                    if (gInfo != nullptr  &&  !maxAnalysisTime.empty() &&  maxAnalysisTime < gInfo->mAnalysisTime)
                      generationValid = false;

                    if (gInfo != nullptr  &&  gInfo->mStatus != 1  &&  !acceptNotReadyGenerations)
                      generationValid = false;

                    if (generationValid  &&  (queryFlags & Query::Flags::LatestGeneration)  &&  validCount > 0)
                      generationValid = false;

                    if (generationValid)
                    {
                      validCount++;
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

                      if ((qParam.mFlags & QueryParameter::Flags::SameAnalysisTime) != 0 || (queryFlags & Query::Flags::SameAnalysisTime) != 0)
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

                    std::set<time_t> originalTimes;
                    std::map<time_t,std::string> contentTimeList2;

                    time_t startTime_tt = startTime;
                    time_t endTime_tt = endTime;

                    auto dataStartTime_tt = contentTimeList.begin()->first;
                    auto dataEndTime_tt = contentTimeList.rbegin()->first;

                    if ((queryFlags & Query::Flags::StartTimeFromData) != 0)
                    {
                      startTime_tt = dataStartTime_tt;
                    }
                    else
                    {
                      // Finding first valid start time

                      time_t  tt = 0;
                      std::string atime;
                      for (auto tm = contentTimeList.begin(); tm != contentTimeList.end()  &&  tt == 0; ++tm)
                      {
                        if (tm->first >= startTime)
                        {
                          tt = tm->first;
                          atime = tm->second;
                        }
                      }

                      if (queryFlags & Query::Flags::ForceStartTime)
                      {
                        // We have to use the given starttime.
                        contentTimeList.insert(std::pair<time_t,std::string>(startTime_tt,atime));
                      }
                      else
                      {
                        if (tt > 0)
                          startTime_tt = tt;
                      }
                    }

                    if ((queryFlags & Query::Flags::EndTimeFromData) != 0)
                      endTime_tt = dataEndTime_tt;

                    //if (startTime_tt < dataStartTime_tt)
                    //  startTime_tt = dataStartTime_tt;

                    if (endTime_tt > dataEndTime_tt)
                      endTime_tt = dataEndTime_tt;


                    PRINT_DATA(mDebugLog," -- TIME RANGE %s - %s\n",utcTimeFromTimeT(startTime_tt).c_str(),utcTimeFromTimeT(endTime_tt).c_str());

                    if (timeStepIsData)
                    {
                      for (auto it1 = contentTimeList.begin(); it1 != contentTimeList.end(); ++it1)
                      {
                        // We should pick data times that are in the given time range.

                        if (it1->first >= startTime_tt  &&  it1->first <= endTime_tt)
                        {
                          if (query.mTimesteps == 0 ||  valueCounter < query.mTimesteps)
                          {
                            // If the parameter requires aggregation then we have to pick some additional times.

                            for (uint tt=qParam.mTimestepsBefore; tt > 0; tt--)
                            {
                              time_t nt = it1->first - (tt * parameterTimestepSizeInSeconds);
                              if (contentTimeList2.find(nt) == contentTimeList2.end())
                              {
                                // Required aggregation time might have different analysis time than the actual data time.

                                auto at = contentTimeList.find(nt);
                                if (at != contentTimeList.end())
                                  contentTimeList2.insert(std::pair<time_t,std::string>(nt,at->second));
                                else
                                  contentTimeList2.insert(std::pair<time_t,std::string>(nt,it1->second));
                              }
                            }

                            // Adding the data time.

                            contentTimeList2.insert(std::pair<time_t,std::string>(it1->first,it1->second));
                            originalTimes.insert(it1->first);

                            // If the parameter requires aggregation then we have to pick some additional times.

                            for (uint tt=1; tt <= qParam.mTimestepsAfter; tt++)
                            {
                              time_t nt = it1->first + (tt * parameterTimestepSizeInSeconds);
                              if (contentTimeList2.find(nt) == contentTimeList2.end())
                              {
                                // Required aggregation time might have different analysis time than the actual data time.

                                auto at = contentTimeList.find(nt);
                                if (at != contentTimeList.end())
                                  contentTimeList2.insert(std::pair<time_t,std::string>(nt,at->second));
                                else
                                  contentTimeList2.insert(std::pair<time_t,std::string>(nt,it1->second));
                              }
                            }

                            valueCounter++;
                          }
                        }
                      }
                    }
                    else
                    {
                      if (queryTimestepSizeInSeconds > 0)
                      {
                        auto qst = query.mTimesteps;
                        if (query.mTimesteps == 0  && startTime_tt <= endTime_tt)
                        {
                          auto td = endTime_tt - startTime_tt;
                          qst = (td/queryTimestepSizeInSeconds) + 1;
                        }

                        if (qst > 0  &&  qst < 10000)
                        {
                          auto stt = startTime_tt;
                          for (uint ts = 0; ts<qst; ts++)
                          {
                            if (query.mTimesteps == 0 ||  valueCounter < query.mTimesteps)
                            {
                              // If the parameter requires aggregation then we have to pick some additional times.

                              for (uint tt=qParam.mTimestepsBefore; tt > 0; tt--)
                              {
                                time_t nt = stt - (tt * parameterTimestepSizeInSeconds);
                                if (contentTimeList2.find(nt) == contentTimeList2.end())
                                {
                                  auto at = contentTimeList.find(nt);
                                  if (at != contentTimeList.end())
                                    contentTimeList2.insert(std::pair<time_t,std::string>(nt,at->second));
                                  else
                                  {
                                    auto upper = contentTimeList.upper_bound(nt);
                                    if (upper != contentTimeList.end())
                                      contentTimeList2.insert(std::pair<time_t,std::string>(nt,upper->second));
                                    else
                                    {
                                      auto lower = contentTimeList.lower_bound(nt);
                                      if (lower != contentTimeList.end())
                                        contentTimeList2.insert(std::pair<time_t,std::string>(nt,lower->second));
                                      else
                                        contentTimeList2.insert(std::pair<time_t,std::string>(nt,analysisTime));
                                    }
                                  }
                                }
                              }

                              // Adding the timestep time and its analysis time.

                              auto at = contentTimeList.find(stt);
                              if (at != contentTimeList.end())
                                contentTimeList2.insert(std::pair<time_t,std::string>(stt,at->second));
                              else
                              {
                                auto upper = contentTimeList.upper_bound(stt);
                                if (upper != contentTimeList.end())
                                  contentTimeList2.insert(std::pair<time_t,std::string>(stt,upper->second));
                                else
                                {
                                  auto lower = contentTimeList.lower_bound(stt);
                                  if (lower != contentTimeList.end())
                                    contentTimeList2.insert(std::pair<time_t,std::string>(stt,lower->second));
                                  else
                                    contentTimeList2.insert(std::pair<time_t,std::string>(stt,analysisTime));
                                }
                              }

                              originalTimes.insert(stt);

                              // If the parameter requires aggregation then we have to pick some additional times.

                              for (uint tt=1; tt <= qParam.mTimestepsAfter; tt++)
                              {
                                time_t nt = stt + (tt * parameterTimestepSizeInSeconds);
                                if (contentTimeList2.find(nt) == contentTimeList2.end())
                                {
                                  auto at = contentTimeList.find(nt);
                                  if (at != contentTimeList.end())
                                    contentTimeList2.insert(std::pair<time_t,std::string>(nt,at->second));
                                  else
                                  {
                                    auto upper = contentTimeList.upper_bound(nt);
                                    if (upper != contentTimeList.end())
                                      contentTimeList2.insert(std::pair<time_t,std::string>(nt,upper->second));
                                    else
                                    {
                                      auto lower = contentTimeList.lower_bound(nt);
                                      if (lower != contentTimeList.end())
                                        contentTimeList2.insert(std::pair<time_t,std::string>(nt,lower->second));
                                      else
                                        contentTimeList2.insert(std::pair<time_t,std::string>(nt,analysisTime));
                                    }
                                  }
                                }
                              }

                              stt += queryTimestepSizeInSeconds;
                              valueCounter++;
                            }
                          }
                        }
                      }
                    }


                    time_t a_lastTime = startTime;
                    for (auto forecastTime = contentTimeList2.begin(); forecastTime != contentTimeList2.end(); ++forecastTime)
                    {
                      if (((forecastTime->first == startTime && !ignoreStartTimeValue) || (forecastTime->first > startTime && forecastTime->first <= endTime)))
                      {
                        std::shared_ptr<ParameterValues> valList = std::make_shared<ParameterValues>();

                        getGridValues(query,qParam,producers2, geometryIdList2, producerInfo.mProducerId, forecastTime->second, maxAnalysisTime, 0, parameterKey, parameterHash,
                            queryFlags, forecastTime->first, true, *valList, producerStr);

                        if (valList->mValueList.getLength() > 0 || valList->mValueData.size() > 0 || valList->mValueVector.size() > 0)
                        {
                          if (valList->mForecastTimeUTC == 0)
                            valList->mForecastTimeUTC = forecastTime->first;

                          if (a_lastTime < valList->mForecastTimeUTC)
                            a_lastTime = valList->mForecastTimeUTC;

                          if (originalTimes.size() && originalTimes.find(valList->mForecastTimeUTC) == originalTimes.end())
                          {
                            valList->mFlags |= ParameterValues::Flags::AggregationValue;
                            query.mAggregationTimes.insert(valList->mForecastTimeUTC);

                            PRINT_DATA(mDebugLog," ##### set aggregationFlag (%ld) %s\n",originalTimes.size(),utcTimeFromTimeT(valList->mForecastTimeUTC).c_str());
                          }

                          qParam.mValueList.emplace_back(valList);
                        }
                      }
                    }

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
                      getGridValues(query,qParam,rec->second, tmpGeomIdList, 0, analysisTime, generationFlags, parameterKey, parameterHash,
                          queryFlags, a_lastTime+1,endTime,producerStr,valueCounter,recursionCounter+1);
                    }
                    return;
                  }
                }
              }
            }
            else
            {
              PRINT_DATA(mDebugLog, "    - No mappings found (%s,%s,%d,%d,%d)!\n",producerInfo.mName.c_str(),parameterKey.c_str(),producerGeometryId,qParam.mParameterLevelId,qParam.mParameterLevel);
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
        PRINT_DATA(mDebugLog, "  - Producer's '%s' geometry '%d' does not cover all requested query.mAreaCoordinates!\n", producerName.c_str(), producerGeometryId);
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

    auto dt = Fmi::date_time::from_time_t(values.mForecastTimeUTC);
    Fmi::LocalDateTime utcTime(dt, tz_utc);

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
        auto minutes = Fmi::numeric_cast<long>(round(seconds / 60.0));
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
    auto dt =Fmi::date_time::from_time_t(forecastTime);
    Fmi::LocalDateTime utcTime(dt, tz_utc);

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
      auto minutes = Fmi::numeric_cast<long>(round(seconds / 60.0));
      return (double)minutes;
    }

    return ParamValueMissing;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getClosestLevelsByHeight(uint producerId,uint generationId,int geometryId,T::ParamLevelId paramLevelId,int forecastType,int forecastNumber,time_t forecastTime,uchar coordinateType,double x,double y,int height,HeightRec& rec)
{
  FUNCTION_TRACE
  try
  {
    //PRINT_DATA(mDebugLog,"getClosestLevelsByHeight (height = %d)\n",height);

    double mul = 1.0;
    uint heightProducerId = 0;
    std::string heightParameter;

    std::size_t hash = 0;
    boost::hash_combine(hash,producerId);
    boost::hash_combine(hash,paramLevelId);
    //std::size_t hashConversion = hash;
    uint heightGenerationId = generationId;

    {
      AutoReadLock lock(&mHeightConversions_modificationLock);
      auto conv = mHeightConversions.find(hash);
      if (conv == mHeightConversions.end())
        return false; // No conversion defined for the current producer and level id

      mul = conv->second.multiplier;
      heightProducerId = conv->second.heightProducerId;
      heightParameter = conv->second.heightParameter;
    }


    boost::hash_combine(hash,generationId);
    boost::hash_combine(hash,geometryId);
    boost::hash_combine(hash,forecastType);
    boost::hash_combine(hash,forecastNumber);
    boost::hash_combine(hash,forecastTime);
    boost::hash_combine(hash,coordinateType);
    std::size_t levelContentHash = hash;

    boost::hash_combine(hash,height);
    boost::hash_combine(hash,x);
    boost::hash_combine(hash,y);

    T::ParamKeyType paramKeyType = T::ParamKeyTypeValue::FMI_NAME;

    auto it = mPointHeightCache.find(hash);
    if (it)
    {
      // Cache match
      rec = *it;
      return true;
    }

    if (producerId != heightProducerId)
    {
      // Height producer is different than the requested producer. If we want to
      // use it then the producer should have a generation with the same analysis time.

      T::GenerationInfo gInfo1;
      if (!mGenerationInfoList->getGenerationInfoById(generationId,gInfo1))
        return false;

      T::GenerationInfo gInfo2;
      if (!mGenerationInfoList->getGenerationInfoByProducerIdAndAnalysisTime(heightProducerId,gInfo1.mAnalysisTime,gInfo2))
        return false;

      heightGenerationId = gInfo2.mGenerationId;
    }

    std::size_t levelHash = 0;
    boost::hash_combine(levelHash,heightGenerationId);
    boost::hash_combine(levelHash,geometryId);
    boost::hash_combine(levelHash,paramLevelId);
    boost::hash_combine(levelHash,heightProducerId);


    uint firstLevel = 0xFFFFFFFF;
    uint lastLevel = 0xFFFFFFFF;

    auto itm = mLevelRangeCache.find(levelHash);
    if (itm)
    {
      firstLevel = (*itm >> 32) & 0xFFFFFFFF;
      lastLevel = (*itm) & 0xFFFFFFFF;
    }

    if (firstLevel == 0xFFFFFFFF ||  lastLevel == 0xFFFFFFFF)
    {
      std::set<T::ParamLevel> levels;
      int result = mContentServerPtr->getContentLevelListByGenerationGeometryAndLevelId(0,heightGenerationId,geometryId,paramLevelId,levels);
      if (result != 0 || levels.size() == 0)
      {
        return false;
      }

      firstLevel = *levels.begin();
      lastLevel = *levels.rbegin();

      std::size_t lev = ((std::size_t)firstLevel << 32) + (std::size_t)lastLevel;
      mLevelRangeCache.insert(levelHash,lev);
    }

    ContentCacheEntry_sptr entry;
    auto cc = mLevelContentCache.find(levelContentHash);
    if (cc)
    {
      // The required content information was found from the cache
      entry = *cc;
    }
    else
    {
      entry.reset(new ContentCacheEntry());
      mContentServerPtr->getContentListByParameterAndGenerationId(0,heightGenerationId,paramKeyType,heightParameter,paramLevelId,firstLevel,lastLevel,forecastType,forecastNumber,geometryId,forecastTime,forecastTime,0,entry->contentInfoList);
      entry->generationId = heightGenerationId;
      entry->producerHash = heightProducerId;

      auto len = entry->contentInfoList.getLength();
      if (!len)
        len = 1;

      mLevelContentCache.insert(levelContentHash,entry);
    }

    int len = entry->contentInfoList.getLength();

    rec.levelId = paramLevelId;
    rec.time1 = forecastTime;

    if (len > 0)
    {
      if (getClosestLevelsByHeight(entry->contentInfoList,coordinateType,x,y,height/mul,rec.level1_1,rec.height1_1,rec.level1_2,rec.height1_2))
      {
        if (mul != 1.0)
        {
          rec.height1_1 = rec.height1_1 * mul;
          rec.height1_2 = rec.height1_2 * mul;
          rec.height2_1 = rec.height2_1 * mul;
          rec.height2_2 = rec.height2_2 * mul;
        }

        mPointHeightCache.insert(hash,rec);
        return true;
      }
      return false;
    }


    T::ContentInfoList tmpContentList;
    mContentServerPtr->getContentListByParameterAndGenerationId(0,heightGenerationId,paramKeyType,heightParameter,paramLevelId,firstLevel,lastLevel,forecastType,forecastNumber,geometryId,forecastTime,forecastTime,ContentServer::RequestFlags::INCLUDE_TIME_BEFORE | ContentServer::RequestFlags::INCLUDE_TIME_AFTER,tmpContentList);

    if (tmpContentList.getLength() == 2)
    {
      T::ContentInfo *prev = tmpContentList.getContentInfoByIndex(0);
      T::ContentInfo *next = tmpContentList.getContentInfoByIndex(1);

      T::ContentInfoList contentInfoList1;
      mContentServerPtr->getContentListByParameterAndGenerationId(0,heightGenerationId,paramKeyType,heightParameter,paramLevelId,firstLevel,lastLevel,forecastType,forecastNumber,geometryId,prev->mForecastTimeUTC,prev->mForecastTimeUTC,0,contentInfoList1);
      if (contentInfoList1.getLength() == 0)
        return false;

      T::ContentInfoList contentInfoList2;
      mContentServerPtr->getContentListByParameterAndGenerationId(0,heightGenerationId,paramKeyType,heightParameter,paramLevelId,firstLevel,lastLevel,forecastType,forecastNumber,geometryId,next->mForecastTimeUTC,next->mForecastTimeUTC,0,contentInfoList2);
      if (contentInfoList2.getLength() == 0)
        return false;

      if (contentInfoList1.getLength() != contentInfoList2.getLength())
        return false;

      rec.time1 = prev->mForecastTimeUTC;
      rec.time2 = next->mForecastTimeUTC;

      if (getClosestLevelsByHeight(contentInfoList1,coordinateType,x,y,height/mul,rec.level1_1,rec.height1_1,rec.level1_2,rec.height1_2) &&
          getClosestLevelsByHeight(contentInfoList2,coordinateType,x,y,height/mul,rec.level2_1,rec.height2_1,rec.level2_2,rec.height2_2))
      {
        if (mul != 1.0)
        {
          rec.height1_1 = rec.height1_1 * mul;
          rec.height1_2 = rec.height1_2 * mul;
          rec.height2_1 = rec.height2_1 * mul;
          rec.height2_2 = rec.height2_2 * mul;
        }

        mPointHeightCache.insert(hash,rec);
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





bool ServiceImplementation::getClosestLevelsByHeight(T::ContentInfoList& contentInfoList,uchar coordinateType,double x,double y,int height,int& level1,float& height1,int& level2,float& height2)
{
  FUNCTION_TRACE
  try
  {
    //PRINT_DATA(mDebugLog,"getClosestLevelsByHeight (height = %d)\n",height);

    int len = contentInfoList.getLength();
    if (len == 0)
      return false;

    int firstIndex = 0;
    int lastIndex = len-1;

    T::ContentInfo *first = contentInfoList.getContentInfoByIndex(firstIndex);
    T::ContentInfo *last = contentInfoList.getContentInfoByIndex(lastIndex);

    double_vec modificationParameters;
    T::ParamValue firstValue = 0;
    T::ParamValue lastValue = 0;
    T::ParamValue middleValue = 0;
    int res1 = mDataServerPtr->getGridValueByPoint(0,first->mFileId,first->mMessageIndex,coordinateType,x,y,1,0,modificationParameters,firstValue);
    if (res1 == 0)
    {
      int res2 = mDataServerPtr->getGridValueByPoint(0,last->mFileId,last->mMessageIndex,coordinateType,x,y,1,0,modificationParameters,lastValue);
      if (res2 == 0)
      {
        bool reverse = false;
        if (firstValue > lastValue)
          reverse = true;


        while (firstIndex < lastIndex)
        {
          int middleIndex = (firstIndex + lastIndex) / 2;
          T::ContentInfo *mid = contentInfoList.getContentInfoByIndex(middleIndex);
          if (mid == nullptr)
            return false;

          int res3 = mDataServerPtr->getGridValueByPoint(0,mid->mFileId,mid->mMessageIndex,coordinateType,x,y,1,0,modificationParameters,middleValue);
          if (res3 == 0)
          {
            if (height == middleValue)
            {
              level1 = mid->mParameterLevel;
              level2 = mid->mParameterLevel;
              height1 = middleValue;
              height2 = middleValue;
              return true;
            }

            if (reverse)
            {
              if (height < middleValue)
              {
                first = mid;
                firstIndex = middleIndex;
                firstValue = middleValue;
              }
              else
              {
                last = mid;
                lastIndex = middleIndex;
                lastValue = middleValue;
              }
            }
            else
            {
              if (height > middleValue)
              {
                first = mid;
                firstIndex = middleIndex;
                firstValue = middleValue;
              }
              else
              {
                last = mid;
                lastIndex = middleIndex;
                lastValue = middleValue;
              }
            }

            if ((firstIndex+1) == lastIndex)
            {
              if (reverse)
              {
                level1 = first->mParameterLevel;
                level2 = last->mParameterLevel;
                height1 = firstValue;
                height2 = lastValue;
              }
              else
              {
                level1 = last->mParameterLevel;
                level2 = first->mParameterLevel;
                height1 = lastValue;
                height2 = firstValue;
              }
              return true;
            }
          }
        }
      }
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ServiceImplementation::getClosestLevelsByHeight(T::ContentInfoList& contentInfoList,uchar coordinateType,std::vector<T::Coordinate>& coordinates,uint coordinateIndex,std::size_t coordinateHash,int height,int& level1,float& height1,int& level2,float& height2)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"getClosestLevelsByHeight (height = %d)\n",height);

    short areaInterpolationMethod = T::AreaInterpolationMethod::Linear;

    int len = contentInfoList.getLength();
    if (len == 0)
      return false;

    int firstIndex = 0;
    int lastIndex = len-1;

    T::ContentInfo *first = contentInfoList.getContentInfoByIndex(firstIndex);
    T::ContentInfo *last = contentInfoList.getContentInfoByIndex(lastIndex);

    double_vec modificationParameters;
    T::ParamValue firstValue = 0;
    T::ParamValue lastValue = 0;
    T::ParamValue middleValue = 0;


    std::size_t firstKey = ((std::size_t)first->mFileId << 32) + first->mMessageIndex + coordinateHash;
    std::size_t lastKey = ((std::size_t)last->mFileId << 32) + last->mMessageIndex + coordinateHash;

    std::shared_ptr<T::ParamValue_vec> firstValueVec;
    std::shared_ptr<T::ParamValue_vec> lastValueVec;
    std::shared_ptr<T::ParamValue_vec> middleValueVec;


    auto it = mHeightVecCache.find(firstKey);
    if (it)
    {
      firstValueVec = *it;
    }
    else
    {
      firstValueVec.reset(new T::ParamValue_vec());
      int result1 = mDataServerPtr->getGridValueVectorByCoordinateList(0,first->mFileId,first->mMessageIndex,coordinateType,coordinates,areaInterpolationMethod,0,modificationParameters,*firstValueVec);
      if (result1 != 0)
      {
        Fmi::Exception exception(BCP, "DataServer returns an error!");
        exception.addParameter("Service", "getGridValueVectorByCoordinateList");
        exception.addParameter("ErrorMessage", DataServer::getResultString(result1));
        exception.addParameter("FileId", std::to_string(first->mFileId));
        exception.addParameter("MessageIndex", std::to_string(first->mMessageIndex));
        std::string errorMsg = exception.getStackTrace();
        PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
        return false;
      }

      mHeightVecCache.insert(firstKey,firstValueVec);
    }

    firstValue = (*firstValueVec)[coordinateIndex];


    it = mHeightVecCache.find(lastKey);
    if (it)
    {
      lastValueVec = *it;
    }
    else
    {
      lastValueVec.reset(new T::ParamValue_vec());
      int result1 = mDataServerPtr->getGridValueVectorByCoordinateList(0,last->mFileId,last->mMessageIndex,coordinateType,coordinates,areaInterpolationMethod,0,modificationParameters,*lastValueVec);
      if (result1 != 0)
      {
        Fmi::Exception exception(BCP, "DataServer returns an error!");
        exception.addParameter("Service", "getGridValueVectorByCoordinateList");
        exception.addParameter("ErrorMessage", DataServer::getResultString(result1));
        exception.addParameter("FileId", std::to_string(last->mFileId));
        exception.addParameter("MessageIndex", std::to_string(last->mMessageIndex));
        std::string errorMsg = exception.getStackTrace();
        PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
        return false;
      }
      mHeightVecCache.insert(lastKey,lastValueVec);
    }

    lastValue = (*lastValueVec)[coordinateIndex];

    bool reverse = false;
    if (firstValue > lastValue)
      reverse = true;

    while (firstIndex < lastIndex)
    {
      int middleIndex = (firstIndex + lastIndex) / 2;
      T::ContentInfo *mid = contentInfoList.getContentInfoByIndex(middleIndex);
      if (mid == nullptr)
        return false;

      std::size_t middleKey = ((std::size_t)mid->mFileId << 32) + mid->mMessageIndex + coordinateHash;

      auto it = mHeightVecCache.find(middleKey);
      if (it)
      {
        middleValueVec = *it;
      }
      else
      {
        middleValueVec.reset(new T::ParamValue_vec());
        int result1 = mDataServerPtr->getGridValueVectorByCoordinateList(0,mid->mFileId,mid->mMessageIndex,coordinateType,coordinates,areaInterpolationMethod,0,modificationParameters,*middleValueVec);
        if (result1 != 0)
        {
          Fmi::Exception exception(BCP, "DataServer returns an error!");
          exception.addParameter("Service", "getGridValueVectorByCoordinateList");
          exception.addParameter("ErrorMessage", DataServer::getResultString(result1));
          exception.addParameter("FileId", std::to_string(mid->mFileId));
          exception.addParameter("MessageIndex", std::to_string(mid->mMessageIndex));
          std::string errorMsg = exception.getStackTrace();
          PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
          return false;
        }
        mHeightVecCache.insert(middleKey,middleValueVec);
      }

      middleValue = (*middleValueVec)[coordinateIndex];

      if (height == middleValue)
      {
        level1 = mid->mParameterLevel;
        level2 = mid->mParameterLevel;
        height1 = middleValue;
        height2 = middleValue;
        return true;
      }

      if (reverse)
      {
        if (height < middleValue)
        {
          first = mid;
          firstIndex = middleIndex;
          firstValue = middleValue;
        }
        else
        {
          last = mid;
          lastIndex = middleIndex;
          lastValue = middleValue;
        }
      }
      else
      {
        if (height > middleValue)
        {
          first = mid;
          firstIndex = middleIndex;
          firstValue = middleValue;
        }
        else
        {
          last = mid;
          lastIndex = middleIndex;
          lastValue = middleValue;
        }
      }

      if ((firstIndex+1) == lastIndex)
      {
        if (reverse)
        {
          level1 = first->mParameterLevel;
          level2 = last->mParameterLevel;
          height1 = firstValue;
          height2 = lastValue;
        }
        else
        {
          level1 = last->mParameterLevel;
          level2 = first->mParameterLevel;
          height1 = lastValue;
          height2 = firstValue;
        }
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





bool ServiceImplementation::getClosestLevelsByHeight(uint producerId,uint generationId,int geometryId,T::ParamLevelId paramLevelId,int forecastType,int forecastNumber,time_t forecastTime,uchar coordinateType,std::vector<T::Coordinate>& coordinates,int height,HeightRec_vec& recs)
{
  FUNCTION_TRACE
  try
  {
    //PRINT_DATA(mDebugLog,"getClosestLevelsByHeight (height = %d)\n",height);

    double mul = 1.0;
    uint heightProducerId = 0;
    std::string heightParameter;

    std::size_t hash = 0;
    boost::hash_combine(hash,producerId);
    boost::hash_combine(hash,paramLevelId);
    std::size_t hashConversion = hash;
    uint heightGenerationId = generationId;

    {
      AutoReadLock lock(&mHeightConversions_modificationLock);
      auto conv = mHeightConversions.find(hash);
      if (conv == mHeightConversions.end())
      {
        PRINT_DATA(mDebugLog,"  ** No height conversion defined\n");
        return false; // No conversion defined for the current producer and level id
      }

      mul = conv->second.multiplier;
      heightProducerId = conv->second.heightProducerId;
      heightParameter = conv->second.heightParameter;
    }


    boost::hash_combine(hash,generationId);
    boost::hash_combine(hash,geometryId);
    boost::hash_combine(hash,forecastType);
    boost::hash_combine(hash,forecastNumber);
    boost::hash_combine(hash,forecastTime);
    boost::hash_combine(hash,coordinateType);
    std::size_t levelContentHash = hash;



    std::size_t coordinateHash = 0;
    uint clen = coordinates.size();
    for (uint t=0; t<clen; t = t + 100)
    {
      boost::hash_combine(coordinateHash,clen);
      boost::hash_combine(coordinateHash,coordinates[t].x());
      boost::hash_combine(coordinateHash,coordinates[t].y());
    }

    T::ParamKeyType paramKeyType = T::ParamKeyTypeValue::FMI_NAME;

    if (producerId != heightProducerId)
    {
      // Height producer is different than the requested producer. If we want to
      // use it then the producer should have a generation with the same analysis time.

      T::GenerationInfo gInfo1;
      if (!mGenerationInfoList->getGenerationInfoById(generationId,gInfo1))
        return false;

      T::GenerationInfo gInfo2;
      if (!mGenerationInfoList->getGenerationInfoByProducerIdAndAnalysisTime(heightProducerId,gInfo1.mAnalysisTime,gInfo2))
        return false;

      heightGenerationId = gInfo2.mGenerationId;
    }

    std::size_t levelHash = 0;
    boost::hash_combine(levelHash,heightGenerationId);
    boost::hash_combine(levelHash,geometryId);
    boost::hash_combine(levelHash,paramLevelId);
    boost::hash_combine(levelHash,heightProducerId);

    uint firstLevel = 0xFFFFFFFF;
    uint lastLevel = 0xFFFFFFFF;

    auto itm = mLevelRangeCache.find(levelHash);
    if (itm)
    {
      firstLevel = (*itm >> 32) & 0xFFFFFFFF;
      lastLevel = (*itm) & 0xFFFFFFFF;
    }

    if (firstLevel == 0xFFFFFFFF ||  lastLevel == 0xFFFFFFFF)
    {
      std::set<T::ParamLevel> levels;
      int result = mContentServerPtr->getContentLevelListByGenerationGeometryAndLevelId(0,heightGenerationId,geometryId,paramLevelId,levels);
      if (levels.size() == 0)
      {
        PRINT_DATA(mDebugLog,"  ** No levels found\n");
        return false;
      }

      firstLevel = *levels.begin();
      lastLevel = *levels.rbegin();

      //AutoWriteLock lock(&mLevelRangeCache_modificationLock);

      std::size_t lev = ((std::size_t)firstLevel << 32) + (std::size_t)lastLevel;
      mLevelRangeCache.insert(levelHash,lev);
    }


    ContentCacheEntry_sptr entry;
    auto cc = mLevelContentCache.find(levelContentHash);
    if (cc)
    {
      // The required content information was found from the cache
      entry = *cc;
    }
    else
    {
      //mContentCache_misses++;
      entry.reset(new ContentCacheEntry());
      mContentServerPtr->getContentListByParameterAndGenerationId(0,heightGenerationId,paramKeyType,heightParameter,paramLevelId,firstLevel,lastLevel,forecastType,forecastNumber,geometryId,forecastTime,forecastTime,0,entry->contentInfoList);
      entry->generationId = heightGenerationId;
      entry->producerHash = heightProducerId;

      auto len = entry->contentInfoList.getLength();
      if (!len)
        len = 1;

      mLevelContentCache.insert(levelContentHash,entry);
    }


    int len = entry->contentInfoList.getLength();
    if (len > 0)
    {
      for (uint t=0; t<clen; t++)
      {
        HeightRec rec;
        rec.levelId = paramLevelId;
        rec.time1 = forecastTime;
        rec.time2 = forecastTime;

        //if (getClosestLevelsByHeight(entry->contentInfoList,coordinateType,coordinates[t].x(),coordinates[t].y(),height/mul,rec.level1_1,rec.height1_1,rec.level1_2,rec.height1_2))
        if (getClosestLevelsByHeight(entry->contentInfoList,coordinateType,coordinates,t,coordinateHash,height/mul,rec.level1_1,rec.height1_1,rec.level1_2,rec.height1_2))
        {
          if (mul != 1.0)
          {
            rec.height1_1 = rec.height1_1 * mul;
            rec.height1_2 = rec.height1_2 * mul;
            rec.height2_1 = rec.height2_1 * mul;
            rec.height2_2 = rec.height2_2 * mul;
          }
          recs.push_back(rec);
        }
        else
        {
          return false;
        }
      }
      return true;
    }


    T::ContentInfoList tmpContentList;
    mContentServerPtr->getContentListByParameterAndGenerationId(0,heightGenerationId,paramKeyType,heightParameter,paramLevelId,firstLevel,lastLevel,forecastType,forecastNumber,geometryId,forecastTime,forecastTime,ContentServer::RequestFlags::INCLUDE_TIME_BEFORE | ContentServer::RequestFlags::INCLUDE_TIME_AFTER,tmpContentList);
    //printf("tlen = %d\n",tmpContentList.getLength());

    if (tmpContentList.getLength() == 2)
    {
      T::ContentInfo *prev = tmpContentList.getContentInfoByIndex(0);
      T::ContentInfo *next = tmpContentList.getContentInfoByIndex(1);

      T::ContentInfoList contentInfoList1;
      mContentServerPtr->getContentListByParameterAndGenerationId(0,heightGenerationId,paramKeyType,heightParameter,paramLevelId,firstLevel,lastLevel,forecastType,forecastNumber,geometryId,prev->mForecastTimeUTC,prev->mForecastTimeUTC,0,contentInfoList1);
      if (contentInfoList1.getLength() == 0)
        return false;

      T::ContentInfoList contentInfoList2;
      mContentServerPtr->getContentListByParameterAndGenerationId(0,heightGenerationId,paramKeyType,heightParameter,paramLevelId,firstLevel,lastLevel,forecastType,forecastNumber,geometryId,next->mForecastTimeUTC,next->mForecastTimeUTC,0,contentInfoList2);
      if (contentInfoList2.getLength() == 0)
        return false;

      if (contentInfoList1.getLength() != contentInfoList2.getLength())
        return false;

      for (int t=0;t<clen; t++)
      {
        HeightRec rec;
        rec.levelId = paramLevelId;
        rec.time1 = prev->mForecastTimeUTC;
        rec.time2 = next->mForecastTimeUTC;

        //if (getClosestLevelsByHeight(contentInfoList1,coordinateType,coordinates[t].x(),coordinates[t].y(),height/mul,rec.level1_1,rec.height1_1,rec.level1_2,rec.height1_2) &&
        //    getClosestLevelsByHeight(contentInfoList2,coordinateType,coordinates[t].x(),coordinates[t].y(),height/mul,rec.level2_1,rec.height2_1,rec.level2_2,rec.height2_2))
        if (getClosestLevelsByHeight(contentInfoList1,coordinateType,coordinates,t,coordinateHash,height/mul,rec.level1_1,rec.height1_1,rec.level1_2,rec.height1_2) &&
            getClosestLevelsByHeight(contentInfoList2,coordinateType,coordinates,t,coordinateHash,height/mul,rec.level2_1,rec.height2_1,rec.level2_2,rec.height2_2))
        {
          if (mul != 1.0)
          {
            rec.height1_1 = rec.height1_1 * mul;
            rec.height1_2 = rec.height1_2 * mul;
            rec.height2_1 = rec.height2_1 * mul;
            rec.height2_2 = rec.height2_2 * mul;
          }
          recs.push_back(rec);
        }
        else
        {
          return false;
        }
      }
    }

    return true;
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
    time_t changeTime = 0;
    if (mContentServerPtr->getContentChangeTime(0,changeTime) == 0)
    {
      // Force updata if the content information is changed
      if (changeTime >  mProducerMap_updateTime)
        mProducerMap_updateTime = 0;
    }


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

    time_t prevTime = time(nullptr);
    long long prevCounter = mRequestCounter;

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
              mContentSearchCache_globalClearRequiredTime = time(nullptr);
              mContentCache_globalClearRequiredTime = time(nullptr);

              mPointHeightCache.clear();
              mLevelRangeCache.clear();
              mLevelContentCache.clear();

              mProducerMap_updateTime = 0;
              mGenerationInfoList_checkTime = 0;
              mHeightConversionsFile_checkTime = 0;
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

      try
      {
        time_t currentTime = time(nullptr);

        time_t dt = currentTime - prevTime;
        if (dt >= 10)
        {
          mRequestsPerSecond = (mRequestCounter - prevCounter) / dt;
          prevCounter = mRequestCounter;
          prevTime = currentTime;
        }

        if ((currentTime - mHeightConversionsFile_checkTime) > 60)
        {
          loadHeightConversionFile();
          mHeightConversionsFile_checkTime = currentTime;
        }

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
