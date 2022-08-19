#pragma once

#include "../definition/ServiceInterface.h"
#include "../definition/AliasFileCollection.h"
#include "../definition/ParameterMappingFile.h"
#include "../../dataServer/definition/ServiceInterface.h"
#include "../../contentServer/definition/ServiceInterface.h"
#include "../../lua/LuaFileCollection.h"
#include "../../functions/FunctionCollection.h"
#include <boost/unordered_map.hpp>
#include <unordered_map>


namespace SmartMet
{
namespace QueryServer
{

typedef std::vector<std::pair<std::string,T::GeometryId>>     Producer_vec;
typedef std::unordered_map<std::string,T::ProducerInfo>       Producer_map;
typedef std::shared_ptr<Producer_map>                         Producer_map_sptr;
typedef ContentServer::ServiceInterface*                      ContentServer_ptr;
typedef DataServer::ServiceInterface*                         DataServer_ptr;
typedef std::vector<std::pair<std::string,int>>               LevelHeightCache;
typedef std::shared_ptr<ParameterMapping_vec>                 ParameterMapping_vec_sptr;
typedef boost::unordered_map<size_t,ParameterMapping_vec_sptr>  ParameterMappingCache;
typedef std::shared_ptr<std::vector<std::string>>             StringVector_sptr;
typedef std::unordered_map<std::string,Producer_vec>          Altenative_map;


class CacheEntry
{
  public:
    T::GenerationInfoList_sptr generationInfoList;
    StringVector_sptr          analysisTimes;
};

typedef std::shared_ptr<CacheEntry> CacheEntry_sptr;
typedef std::unordered_map<uint,CacheEntry_sptr> ProducerGenarationListCache;

class ContentCacheEntry
{
  public:
    T::ContentInfoList contentInfoList;
    ulonglong producerHash;
    uint generationId;
};

typedef std::shared_ptr<ContentCacheEntry> ContentCacheEntry_sptr;

typedef std::unordered_map<std::size_t,ContentCacheEntry_sptr> ContentCache;


class ContentSearchCacheEntry
{
  public:
    std::shared_ptr<T::ContentInfoList> contentInfoList[3];
    ulonglong producerHash[3];
    uint generationId;
};
typedef std::unordered_map<std::size_t,ContentSearchCacheEntry> ContentSearchCache;


struct HashRec
{
  time_t checkTime;
  ulonglong hash;
};

typedef std::unordered_map<uint,HashRec> ProducerHash_map;



class ServiceImplementation : public ServiceInterface
{
  public:
                    ServiceImplementation();
     virtual        ~ServiceImplementation();

     void           init(
                       ContentServer::ServiceInterface *contentServerPtr,
                       DataServer::ServiceInterface *dataServerPtr,
                       const std::string& gridConfigFile,
                       string_vec& parameterMappingFiles,
                       string_vec& aliasFiles,
                       const std::string& producerFile,
                       string_vec& producerAliasFiles,
                       string_vec& luaFileNames,
                       bool checkGeometryStatus,
                       bool dataServerMethodsEnabled);

     void           getCacheStats(Fmi::Cache::CacheStatistics& statistics) const;

     void           shutdown();
     void           setDem(boost::shared_ptr<Fmi::DEM> dem);
     void           setLandCover(boost::shared_ptr<Fmi::LandCover> landCover);
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
     void           executeQueryFunctions(Query& query);
     int            executeTimeRangeQuery(Query& query);
     int            executeTimeStepQuery(Query& query);
     void           startUpdateProcessing();

     void           getGridValues(
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
                       std::string& producerStr);

     void           getGridValues(
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
                       uint& producerId,
                       ulonglong& generationFlags,
                       short& areaInterpolationMethod,
                       short& timeInterpolationMethod,
                       short& levelInterpolationMethod);

     void           getParameterStringInfo(
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
                       short& levelInterpolationMethod);

     void          getParameterMappings(
                       const std::string& producerName,
                       uint producerId,
                       const std::string& parameterName,
                       std::size_t parameterHash,
                       T::GeometryId geometryId,
                       bool onlySearchEnabled,
                       ParameterMapping_vec_sptr& mappings);

     void          getParameterMappings(
                       const std::string& producerName,
                       uint producerId,
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

     bool           parseFunction(
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

     bool           getPointValuesByHeight(
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
                       ParameterValues& valueList);

     bool           getPolygonValues(
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
                       ParameterValues& valueList);

     bool           getCircleValues(
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
                       ParameterValues& valueList);

     bool           getPointValues(
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
                       ParameterValues& valueList);

     bool           getSpecialValues(
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
                       ParameterValues& valueList);

     bool           getIsolineValues(
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
                       ParameterValues& valueList);


     bool           getIsobandValues(
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
                       ParameterValues& valueList);

     bool           getValueVectors(
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
                       T::Coordinate_vec& coordinates);

     bool           getGridFiles(
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
                       ParameterValues& valueList);

     void           convertLevelsToHeights(
                       T::ContentInfoList& contentList,
                       uchar coordinateType,
                       double x,
                       double y,
                       T::ContentInfoList& newContentList);

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

     ulonglong      getProducerHash(uint producerId);

     time_t         getContentList(
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
                      std::shared_ptr<T::ContentInfoList>& contentList);

     int            getContentListByParameterGenerationIdAndForecastTime(
                       T::SessionId sessionId,
                       uint producerId,
                       ulonglong producerHash,
                       uint generationId,
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

     CacheEntry_sptr getGenerationInfoListByProducerId(uint producerId,bool acceptNotReadyGenerations);

     void            getGenerationTimeRangeByProducerAndGenerationId(uint producerId,uint generationId,time_t& startTime,time_t& endTime);

     bool            isGeometryReady(uint generationId,int geometryId,T::ParamLevelId levelId);
     bool            isGeometryValid(int geometryId,std::vector<std::vector<T::Coordinate>>& polygonPath);


  private:

     // Private attributes

     AliasFileCollection        mAliasFileCollection;
     AliasFileCollection        mProducerAliasFileCollection;
     uint                       mFunctionParamId;
     std::string                mGridConfigFile;
     Lua::LuaFileCollection     mLuaFileCollection;
     string_vec                 mParameterMappingFiles;
     ProducerHash_map           mProducerHashMap;
     ModificationLock           mProducerHashMap_modificationLock;

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
     LevelHeightCache           mLevelHeightCache;
     ModificationLock           mHeightCache_modificationLock;

     ParamMappingFile_vec       mParameterMappings;
     time_t                     mParameterMapping_checkTime;
     uint                       mParameterMapping_checkInterval;
     std::size_t                mContentCache_maxRecords;
     std::size_t                mContentCache_maxRecordsPerThread;
     std::size_t                mContentSearchCache_maxRecords;
     std::size_t                mContentSearchCache_maxRecordsPerThread;
     time_t                     mParameterMappingCache_clearRequired;

     pthread_t                  mThread;
     bool                       mShutdownRequested;
     bool                       mUpdateProcessingActive;
     bool                       mCheckGeometryStatus;
     bool                       mDataServerMethodsEnabled;
     time_t                     mProducerGenerationListCache_clearRequired;

     mutable Fmi::Cache::CacheStats    mContentCache_stats;
     mutable Fmi::Cache::CacheStats    mContentSearchCache_stats;


     std::map<std::string,uint>        mOperationNames;
     Functions::FunctionCollection     mFunctionCollection;
     boost::shared_ptr<Fmi::LandCover> mLandCover;
     boost::shared_ptr<Fmi::DEM>       mDem;
};


}
}

