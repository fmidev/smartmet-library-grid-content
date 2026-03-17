#pragma once

#include "../definition/ServiceInterface.h"
#include "../definition/AliasFileCollection.h"
#include "../definition/UnitConversion.h"
#include "../definition/ParameterMappingFile.h"
#include "../../dataServer/definition/ServiceInterface.h"
#include "../../contentServer/definition/ServiceInterface.h"
#include "../../lua/LuaFileCollection.h"
#include "../../functions/FunctionCollection.h"

#include <macgyver/Cache.h>
#include <boost/unordered_map.hpp>
#include <unordered_map>
#include <atomic>


namespace SmartMet
{
namespace QueryServer
{

typedef std::vector<std::pair<std::string,T::GeometryId>>     Producer_vec;
typedef std::unordered_map<std::string,T::ProducerInfo>       Producer_map;
typedef std::shared_ptr<Producer_map>                         Producer_map_sptr;
typedef ContentServer::ServiceInterface*                      ContentServer_ptr;
typedef DataServer::ServiceInterface*                         DataServer_ptr;
typedef std::shared_ptr<ParameterMapping_vec>                 ParameterMapping_vec_sptr;
typedef boost::unordered_map<size_t,ParameterMapping_vec_sptr>  ParameterMappingCache;
typedef std::shared_ptr<std::vector<std::string>>             StringVector_sptr;
typedef std::unordered_map<std::string,Producer_vec>          Altenative_map;


class CacheEntry
{
  public:
    T::GenerationInfoList_sptr generationInfoList;
    StringVector_sptr          analysisTimes;
    UInt64                     producerHash;
};

typedef std::shared_ptr<CacheEntry> CacheEntry_sptr;
typedef std::unordered_map<T::ProducerId,CacheEntry_sptr> ProducerGenarationListCache;

class ContentCacheEntry
{
  public:
    T::ContentInfoList contentInfoList;
    UInt64 producerHash;
    T::GenerationId generationId;
};

typedef std::shared_ptr<ContentCacheEntry> ContentCacheEntry_sptr;

typedef std::unordered_map<std::size_t,ContentCacheEntry_sptr> ContentCache;



class ContentSearchCacheEntry
{
  public:
    std::shared_ptr<T::ContentInfoList> contentInfoList;
    UInt64 producerHash;
    T::GenerationId generationId;
};
typedef std::unordered_map<std::size_t,ContentSearchCacheEntry> ContentSearchCache;


class HeightRec
{
  public:
    HeightRec() {levelId = 0; time1 = 0;level1_1 = 0; level1_2 = 0; height1_1 = 0; height1_2 = 0;time2 = 0; level2_1 = 0; level2_2 = 0;height2_1 = 0; height2_2 = 0;};

    T::ParamLevelId levelId;
    time_t time1;
    T::ParamLevel level1_1;
    T::ParamLevel level1_2;
    T::ParamValue height1_1;
    T::ParamValue height1_2;

    time_t time2;
    T::ParamLevel level2_1;
    T::ParamLevel level2_2;
    T::ParamValue height2_1;
    T::ParamValue height2_2;
};

typedef std::vector<HeightRec> HeightRec_vec;

typedef Fmi::Cache::Cache<std::size_t,HeightRec> HeightCache;
typedef Fmi::Cache::Cache<std::size_t,std::size_t> LevelCache;
typedef Fmi::Cache::Cache<std::size_t,std::shared_ptr<T::ParamValue_vec>> HeightVecCache;
typedef Fmi::Cache::Cache<std::size_t,ContentCacheEntry_sptr> LevelContentCache;



class HeightConversion
{
  public:
    HeightConversion() {producerId = 0; heightProducerId = 0; multiplier = 1.0;}

    T::ProducerId   producerId;
    T::ParamLevelId levelId;
    T::ProducerId   heightProducerId;
    std::string     heightParameter;
    double          multiplier;
};

typedef std::unordered_map<std::size_t,HeightConversion> HeightConversionMap;



class ServiceImplementation : public ServiceInterface
{
  public:
                    ServiceImplementation();
     virtual        ~ServiceImplementation();

     void           init(
                       ContentServer::ServiceInterface *contentServerPtr,
                       DataServer::ServiceInterface *dataServerPtr,
                       const std::string& gridConfigFile,
                       const std::string& heightConversionFile,
                       string_vec& parameterMappingFiles,
                       const std::string& unitConversionFile,
                       string_vec& parameterMappingAliasFiles,
                       string_vec& aliasFiles,
                       const std::string& producerFile,
                       string_vec& producerAliasFiles,
                       string_vec& luaFileNames,
                       bool checkGeometryStatus,
                       bool dataServerMethodsEnabled);

     void           getCacheStats(Fmi::Cache::CacheStatistics& statistics) const;
     void           getStateAttributes(std::shared_ptr<T::AttributeNode> parent);
     void           initContentCache(std::size_t maxRecordsPerThread,uint clearIntervalInSeconds);
     void           initContentSearchCache(std::size_t maxRecordsPerThread,uint clearIntervalInSeconds);

     void           shutdown();
     void           setDem(std::shared_ptr<Fmi::DEM> dem);
     void           setLandCover(std::shared_ptr<Fmi::LandCover> landCover);
     void           updateProcessing();

  protected:

     // Methods that can be called through the service interface

     int            _executeQuery(T::SessionId sessionId,Query& query);

     int            _getProducerList(T::SessionId sessionId,string_vec& producerList);

     int            _getValuesByGridPoint(
                       T::SessionId sessionId,
                       T::ContentInfoList& contentInfoList,
                       T::CoordinateType coordinateType,
                       double x,
                       double y,
                       short areaInterpolationMethod,
                       T::GridPointValueList& valueList);

  private:

     // Private methods

     void           checkProducerMapUpdates();
     void           checkGenerationUpdates();
     void           checkProducerFileUpdates();
     void           checkParameterMappingUpdates();

     bool           getAlias(const std::string& name,std::string& alias);
     void           executeQueryFunctions_vector(Query& query);
     void           executeQueryFunctions(Query& query);
     int            executeTimeRangeQuery(Query& query);
     int            executeTimeStepQuery(Query& query);
     void           postProcessQuery(Query& query);

     void           startUpdateProcessing();

     void           getGridValues(
                       Query& query,
                       QueryParameter& qParam,
                       Producer_vec& producers,
                       std::set<T::GeometryId>& geometryIdList,
                       T::ProducerId producerId,
                       const std::string& analysisTime,
                       const std::string& maxAnalysisTime,
                       UInt64 generationFlags,
                       const std::string& parameterKey,
                       std::size_t parameterHash,
                       uint queryFlags,
                       time_t forecastTime,
                       bool timeMatchRequired,
                       ParameterValues& valueList,
                       std::string& producerStr);

     void           getGridValues(
                       Query& query,
                       QueryParameter& qParam,
                       Producer_vec& producers,
                       std::set<T::GeometryId>& geometryIdList,
                       T::ProducerId producerId,
                       const std::string& analysisTime,
                       UInt64 generationFlags,
                       const std::string& parameterKey,
                       std::size_t parameterHash,
                       uint queryFlags,
                       time_t startTime,
                       time_t endTime,
                       std::string& producerStr,
                       uint valueCounter,
                       uint recursionCounter);

     void           getParameterStringInfo(
                       const std::string& param,
                       char fieldSeparator,
                       std::string& key,
                       T::GeometryId& geometryId,
                       T::ParamLevelId& paramLevelId,
                       T::ParamLevel& paramLevel,
                       T::ForecastType& forecastType,
                       T::ForecastNumber& forecastNumber,
                       std::string& producerName,
                       T::ProducerId& producerId,
                       UInt64 & generationFlags,
                       short& areaInterpolationMethod,
                       short& timeInterpolationMethod,
                       short& levelInterpolationMethod,
                       uint& flags);

     void           getParameterStringInfo(
                       const std::string& param,
                       char fieldSeparator,
                       std::string& key,
                       T::GeometryId& geometryId,
                       T::ParamLevelId& paramLevelId,
                       std::vector<T::ParamLevel>& paramLevelVec,
                       T::ForecastType& forecastType,
                       std::vector<T::ForecastNumber>& forecastNumberVec,
                       std::string& producerName,
                       T::ProducerId& producerId,
                       UInt64 & generationFlags,
                       short& areaInterpolationMethod,
                       short& timeInterpolationMethod,
                       short& levelInterpolationMethod,
                       uint& flags);

     void          getParameterMappings(
                       const std::string& producerName,
                       T::ProducerId producerId,
                       const std::string& parameterName,
                       std::size_t parameterHash,
                       T::GeometryId geometryId,
                       bool onlySearchEnabled,
                       ParameterMapping_vec_sptr& mappings);

     void          getParameterMappings(
                       const std::string& producerName,
                       T::ProducerId producerId,
                       const std::string& parameterName,
                       std::size_t parameterHash,
                       T::GeometryId geometryId,
                       T::ParamLevelId levelId,
                       T::ParamLevel level,
                       bool onlySearchEnabled,
                       ParameterMapping_vec_sptr& mappings);

     bool           getFunctionParams(
                       const std::string& functionParamsStr,
                       FunctionParam_vec& functionParams);

     bool           getParameterFunctionInfo(
                       const std::string& paramStr,
                       std::string& function,
                       std::string& functionParams);

     void           getProducers(
                       Query& query,
                       Producer_vec& producers);

     void           getProducers(
                       const std::string& producerName,
                       Producer_vec& producers);

     void           getGeometryIdListByCoordinates(
                     Producer_vec& producers,
                     T::AreaCoordinates& coordinates,
                     std::set<T::GeometryId>& geometryIdList);

     void           getGeometryIdListByCoordinates(
                       uint gridWidth,
                       uint gridHeight,
                       std::vector<T::Coordinate>& coordinates,
                       std::set<T::GeometryId>& geometryIdList);

     void           getGeometryIdListByGeometryId(
                       T::GeometryId gridGeometryId,
                       std::set<T::GeometryId>& geometryIdList);

     void           loadProducerFile();
     void           loadHeightConversionFile();
     void           loadUnitConversionFile();


     bool           parseFunction(
                       Query& query,
                       QueryParameter& queryParam,
                       const std::string& paramStr,
                       std::string& function,
                       FunctionParam_vec& functionParams,
                       uint recursionCounter,
                       std::vector<QueryParameter>& additionalParameterList);

     void           updateQueryParameters(Query& query);

     bool           conversionFunction(
                       const std::string& conversionFunction,
                       std::string& function,
                       std::vector<std::string>& functionParams);

     void           executeConversion(
                       const std::string& function,
                       std::vector<std::string>& functionParams,
                       time_t forecastTime,
                       T::GridValueList& valueList);

     void           executeConversion(
                       const std::string& function,
                       std::vector<std::string>& functionParams,
                       T::ParamValue_vec& valueList,
                       T::ParamValue_vec& newValueList);

     void           executeConversion(
                       const std::string& function,
                       std::vector<std::string>& functionParams,
                       time_t forecastTime,
                       T::Coordinate_vec& coordinates,
                       T::ParamValue_vec& valueList,
                       T::ParamValue_vec& newValueList);

     bool           getPolygonValues(
                       Query& query,
                       QueryParameter& qParam,
                       T::ProducerInfo& producerInfo,
                       T::GeometryId producerGeometryId,
                       T::GenerationId generationId,
                       const std::string& analysisTime,
                       UInt64 generationFlags,
                       ParameterMapping& pInfo,
                       time_t forecastTime,
                       T::ParamLevelId paramLevelId,
                       T::ParamLevel paramLevel,
                       T::ProducerId& newProducerId,
                       ParameterValues& valueList);

     bool           getCircleValues(
                       Query& query,
                       QueryParameter& qParam,
                       T::ProducerInfo& producerInfo,
                       T::GeometryId producerGeometryId,
                       T::GenerationId generationId,
                       const std::string& analysisTime,
                       UInt64 generationFlags,
                       ParameterMapping& pInfo,
                       time_t forecastTime,
                       T::ParamLevelId paramLevelId,
                       T::ParamLevel paramLevel,
                       uchar coordinateType,
                       double x,
                       double y,
                       double radius,
                       T::ProducerId& newProducerId,
                       ParameterValues& valueList);

     bool           getPointValuesByHeight(
                       Query& query,
                       T::CoordinateType coordinateType,
                       T::Coordinate_vec& coordinates,
                       QueryParameter& qParam,
                       T::ProducerInfo& producerInfo,
                       T::GeometryId producerGeometryId,
                       T::GenerationId generationId,
                       const std::string& analysisTime,
                       UInt64 generationFlags,
                       ParameterMapping& pInfo,
                       time_t forecastTime,
                       T::ParamLevelId paramLevelId,
                       T::ParamLevel paramLevel,
                       T::ProducerId& newProducerId,
                       ParameterValues& valueList);

     bool           getPointValues(
                       Query& query,
                       QueryParameter& qParam,
                       T::ProducerInfo& producerInfo,
                       T::GeometryId producerGeometryId,
                       T::GenerationId generationId,
                       const std::string& analysisTime,
                       UInt64 generationFlags,
                       ParameterMapping& pInfo,
                       time_t forecastTime,
                       T::ParamLevelId paramLevelId,
                       T::ParamLevel paramLevel,
                       T::ProducerId& newProducerId,
                       ParameterValues& valueList);

     bool           getSpecialValues(
                       Query& query,
                       QueryParameter& qParam,
                       T::ProducerInfo& producerInfo,
                       T::GeometryId producerGeometryId,
                       T::GenerationId generationId,
                       const std::string& analysisTime,
                       UInt64 generationFlags,
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
                       T::ProducerId& newProducerId,
                       ParameterValues& valueList);

     bool           getStreamlineValues(
                       Query& query,
                       QueryParameter& qParam,
                       T::ProducerInfo& producerInfo,
                       T::GeometryId producerGeometryId,
                       T::GenerationId generationId,
                       const std::string& analysisTime,
                       UInt64 generationFlags,
                       ParameterMapping& pInfo,
                       time_t forecastTime,
                       T::ParamLevelId paramLevelId,
                       T::ParamLevel paramLevel,
                       T::ProducerId& newProducerId,
                       ParameterValues& valueList);


     bool           getIsolineValues(
                       Query& query,
                       QueryParameter& qParam,
                       T::ProducerInfo& producerInfo,
                       T::GeometryId producerGeometryId,
                       T::GenerationId generationId,
                       const std::string& analysisTime,
                       UInt64 generationFlags,
                       ParameterMapping& pInfo,
                       time_t forecastTime,
                       T::ParamLevelId paramLevelId,
                       T::ParamLevel paramLevel,
                       T::ProducerId& newProducerId,
                       ParameterValues& valueList);


     bool           getIsobandValues(
                       Query& query,
                       QueryParameter& qParam,
                       T::ProducerInfo& producerInfo,
                       T::GeometryId producerGeometryId,
                       T::GenerationId generationId,
                       const std::string& analysisTime,
                       UInt64 generationFlags,
                       ParameterMapping& pInfo,
                       time_t forecastTime,
                       T::ParamLevelId paramLevelId,
                       T::ParamLevel paramLevel,
                       T::ProducerId& newProducerId,
                       ParameterValues& valueList);

     bool           getValueVectorsByHeight(
                       Query& query,
                       T::CoordinateType coordinateType,
                       T::Coordinate_vec& coordinates,
                       QueryParameter& qParam,
                       T::ProducerInfo& producerInfo,
                       T::GeometryId producerGeometryId,
                       T::GenerationId generationId,
                       const std::string& analysisTime,
                       UInt64 generationFlags,
                       ParameterMapping& pInfo,
                       time_t forecastTime,
                       T::ParamLevelId paramLevelId,
                       T::ParamLevel paramLevel,
                       T::ProducerId& newProducerId,
                       ParameterValues& valueList);

     bool           getValueVectors(
                       Query& query,
                       QueryParameter& qParam,
                       T::ProducerInfo& producerInfo,
                       T::GeometryId producerGeometryId,
                       T::GenerationId generationId,
                       const std::string& analysisTime,
                       UInt64 generationFlags,
                       ParameterMapping& pInfo,
                       time_t forecastTime,
                       T::ParamLevelId paramLevelId,
                       T::ParamLevel paramLevel,
                       T::ProducerId& newProducerId,
                       ParameterValues& valueList,
                       T::Coordinate_vec& coordinates);

     bool           getVerticalValueVectors(
                       Query& query,
                       QueryParameter& qParam,
                       T::ProducerInfo& producerInfo,
                       T::GeometryId producerGeometryId,
                       T::GenerationId generationId,
                       const std::string& analysisTime,
                       UInt64 generationFlags,
                       ParameterMapping& pInfo,
                       time_t forecastTime,
                       T::ParamLevelId paramLevelId,
                       std::vector<T::ParamLevel>& parameterLevels,
                       T::ProducerId& newProducerId,
                       ParameterValues& valueList);

     bool           getGridFiles(
                       Query& query,
                       QueryParameter& qParam,
                       T::ProducerInfo& producerInfo,
                       T::GeometryId producerGeometryId,
                       T::GenerationId generationId,
                       const std::string& analysisTime,
                       UInt64 generationFlags,
                       ParameterMapping& pInfo,
                       time_t forecastTime,
                       T::ParamLevelId paramLevelId,
                       T::ParamLevel paramLevel,
                       T::ProducerId& newProducerId,
                       ParameterValues& valueList);

     short          getLevelInterpolationMethod(
                       T::ParamLevelId paramLevelId,
                       short levelInterpolationMethod,
                       uint flags);

     bool           getClosestLevelsByHeight(
                       T::ProducerId producerId,
                       T::GenerationId generationId,
                       int geometryId,
                       T::ParamLevelId paramLevelId,
                       int forecastType,
                       int forecastNumber,
                       time_t forecastTime,
                       uchar coordinateType,
                       double x,
                       double y,
                       int height,
                       HeightRec& rec);

     bool           getClosestLevelsByHeight(
                       T::ProducerId producerId,
                       T::GenerationId generationId,
                       int geometryId,
                       T::ParamLevelId paramLevelId,
                       int forecastType,
                       int forecastNumber,
                       time_t forecastTime,
                       uchar coordinateType,
                       std::vector<T::Coordinate>& coordinates,
                       int height,
                       HeightRec_vec& recs);

     bool           getClosestLevelsByHeight(
                       T::ContentInfoList& contentInfoList,
                       uchar coordinateType,
                       std::vector<T::Coordinate>& coordinates,
                       uint coordinateIndex,
                       std::size_t coordinateHash,
                       int height,
                       int& level1,
                       float& height1,
                       int& level2,
                       float& height2);


     bool           getClosestLevelsByHeight(
                       T::ContentInfoList& contentInfoList,
                       uchar coordinateType,
                       double x,
                       double y,
                       int height,
                       int& level1,
                       float& height1,
                       int& level2,
                       float& height2);

     T::ParamValue  getAdditionalValue(
                       const std::string& parameterName,
                       time_t forecastTime,
                       double x,
                       double y);

     void           getAdditionalValues(
                       const std::string& parameterName,
                       uchar coordinateType,
                       T::Coordinate_vec& coordinates,
                       ParameterValues& values);

     bool           getProducerInfoByName(const std::string& name,T::ProducerInfo& info);

     time_t         getContentList(
                      T::ProducerInfo& producerInfo,
                      T::GeometryId producerGeometryId,
                      T::GenerationId generationId,
                      ParameterMapping& pInfo,
                      time_t forecastTime,
                      T::ParamLevelId paramLevelId,
                      T::ParamLevel paramLevel,
                      T::ForecastType forecastType,
                      T::ForecastNumber forecastNumber,
                      uint parameterFlags,
                      std::shared_ptr<T::ContentInfoList>& contentList);

     int            getContentListByParameterGenerationIdAndForecastTime(
                       T::SessionId sessionId,
                       T::ProducerId producerId,
                       UInt64 producerHash,
                       T::GenerationId generationId,
                       T::ParamKeyType parameterKeyType,
                       const std::string& parameterKey,
                       std::size_t parameterKeyHash,
                       T::ParamLevelId parameterLevelId,
                       T::ParamLevel level,
                       T::ForecastType forecastType,
                       T::ForecastNumber forecastNumber,
                       T::GeometryId geometryId,
                       time_t forecastTime,
                       std::shared_ptr<T::ContentInfoList>& contentInfoList);

     CacheEntry_sptr getGenerationInfoListByProducerId(T::ProducerId producerId,UInt64 producerHash,bool acceptNotReadyGenerations);

     void            getGenerationTimeRangeByProducerAndGenerationId(T::ProducerId producerId,T::GenerationId generationId,time_t& startTime,time_t& endTime);

     bool            isGeometryReady(T::GenerationId generationId,int geometryId,T::ParamLevelId levelId);
     bool            isGeometryValid(int geometryId,std::vector<std::vector<T::Coordinate>>& polygonPath);


  private:

     // Private attributes

     AliasFileCollection        mAliasFileCollection;
     AliasFileCollection        mProducerAliasFileCollection;
     uint                       mFunctionParamId;
     std::string                mGridConfigFile;
     Lua::LuaFileCollection     mLuaFileCollection;
     string_vec                 mParameterMappingFiles;

     std::string                mProducerFile;
     time_t                     mProducerFile_modificationTime;
     time_t                     mProducerFile_checkTime;
     uint                       mProducerFile_checkInterval;

     Producer_vec               mProducerList;
     ModificationLock           mProducerList_modificationLock;
     Altenative_map             mProducerConcatMap;

     Producer_map_sptr          mProducerMap;
     ModificationLock           mProducerMap_modificationLock;
     time_t                     mProducerMap_updateTime;
     uint                       mProducerMap_checkInterval;

     T::GenerationInfoList_sptr mGenerationInfoList;
     ModificationLock           mGenerationInfoList_modificationLock;
     time_t                     mGenerationInfoList_checkTime;
     uint                       mGenerationInfoList_checkInterval;

     time_t                     mContentServerStartTime;
     ContentServer_ptr          mContentServerPtr;
     DataServer_ptr             mDataServerPtr;

     HeightCache                mPointHeightCache;
     LevelCache                 mLevelRangeCache;
     LevelContentCache          mLevelContentCache;
     HeightVecCache             mHeightVecCache;

     ParamMappingFile_vec       mParameterMappings;
     ParamMappingFile_vec       mParameterAliasMappings;
     std::string                mUnitConversionFile;
     UnitConversion_vec         mUnitConversions;
     time_t                     mParameterMapping_checkTime;
     uint                       mParameterMapping_checkInterval;
     std::size_t                mContentCache_maxRecords;
     std::size_t                mContentCache_maxRecordsPerThread;
     uint                       mContentCache_clearInterval;
     time_t                     mContentCache_globalClearRequiredTime;
     time_t                     mContentSearchCache_globalClearRequiredTime;
     std::size_t                mContentSearchCache_maxRecords;
     std::size_t                mContentSearchCache_maxRecordsPerThread;
     uint                       mContentSearchCache_clearInterval;
     time_t                     mParameterMappingCache_clearRequired;

     HeightConversionMap        mHeightConversions;
     ModificationLock           mHeightConversions_modificationLock;
     std::string                mHeightConversionsFile;
     time_t                     mHeightConversionsFile_modificationTime;
     time_t                     mHeightConversionsFile_checkTime;

     pthread_t                  mThread;
     bool                       mShutdownRequested;
     bool                       mUpdateProcessingActive;
     bool                       mCheckGeometryStatus;
     bool                       mDataServerMethodsEnabled;
     time_t                     mProducerGenerationListCache_clearRequired;

     mutable Fmi::Cache::CacheStats mContentCache_stats;
     std::atomic<Int64 >            mContentCache_size;
     Int64                          mContentCache_hits;
     Int64                          mContentCache_misses;
     Int64                          mContentCache_inserts;

     mutable Fmi::Cache::CacheStats mContentSearchCache_stats;
     std::atomic<Int64 >            mContentSearchCache_size;
     Int64                          mContentSearchCache_hits;
     Int64                          mContentSearchCache_misses;
     Int64                          mContentSearchCache_inserts;

     long                           mRequestsPerSecond;
     Int64                          mRequestCounter;

     std::map<std::string,uint>        mOperationNames;
     Functions::FunctionCollection     mFunctionCollection;
     std::shared_ptr<Fmi::LandCover>   mLandCover;
     std::shared_ptr<Fmi::DEM>         mDem;
};


}
}

