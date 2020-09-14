#pragma once

#include "../definition/ServiceInterface.h"
#include "../definition/AliasFileCollection.h"
#include "../definition/ParameterMappingFile.h"
#include "../../dataServer/definition/ServiceInterface.h"
#include "../../contentServer/definition/ServiceInterface.h"
#include "../../lua/LuaFileCollection.h"
#include "../../functions/FunctionCollection.h"



namespace SmartMet
{
namespace QueryServer
{

#define CONTENT_CACHE_SIZE 1000

typedef std::vector<std::pair<std::string,T::GeometryId>>   Producer_vec;
typedef std::map<std::string,T::ProducerInfo>               Producer_map;
typedef ContentServer::ServiceInterface*                    ContentServer_ptr;
typedef DataServer::ServiceInterface*                       DataServer_ptr;
typedef std::vector<std::pair<std::string,int>>             LevelHeightCache;
typedef std::pair<std::size_t,ParameterMapping_vec>         ParameterMappingCacheRec;
typedef std::list<ParameterMappingCacheRec>                 ParameterMappingCache;
typedef std::shared_ptr<std::vector<std::string>>           StringVector_sptr;

class CacheEntry
{
  public:
    T::GenerationInfoList_sptr generationInfoList;
    StringVector_sptr          analysisTimes;
};

typedef std::shared_ptr<CacheEntry>                         CacheEntry_sptr;
typedef std::map<uint,CacheEntry_sptr>                      ProducerGenarationListCache;

class ServiceImplementation : public ServiceInterface
{
  public:
                    ServiceImplementation();
     virtual        ~ServiceImplementation();

     virtual void   init(
                       ContentServer::ServiceInterface *contentServerPtr,
                       DataServer::ServiceInterface *dataServerPtr,
                       std::string gridConfigFile,
                       string_vec& parameterMappingFiles,
                       string_vec& aliasFiles,
                       std::string producerFile,
                       string_vec& producerAliasFiles,
                       string_vec& luaFileNames);

     virtual void   shutdown();

     virtual void   setDem(boost::shared_ptr<Fmi::DEM> dem);
     virtual void   setLandCover(boost::shared_ptr<Fmi::LandCover> landCover);


  protected:

     // Methods that can be called through the service interface

     virtual int    _executeQuery(T::SessionId sessionId,Query& query);

     virtual int    _getProducerList(T::SessionId sessionId,string_vec& producerList);

     virtual int    _getValuesByGridPoint(
                       T::SessionId sessionId,
                       T::ContentInfoList& contentInfoList,
                       T::CoordinateType coordinateType,
                       double x,
                       double y,
                       short areaInterpolationMethod,
                       T::GridPointValueList& valueList);

  private:

     // Private methods

     void           checkConfigurationUpdates();
     bool           getAlias(std::string& name,std::string& alias);
     void           executeQueryFunctions(Query& query);
     int            executeTimeRangeQuery(Query& query);
     int            executeTimeStepQuery(Query& query);

     void           getGridValues(
                       uchar queryType,
                       Producer_vec& producers,
                       std::set<T::GeometryId>& geometryIdList,
                       uint producerId,
                       std::string& analysisTime,
                       uint generationFlags,
                       bool reverseGenerations,
                       std::string& parameterKey,
                       T::ParamLevelId paramLevelId,
                       T::ParamLevel paramLevel,
                       T::ForecastType forecastType,
                       T::ForecastNumber forecastNumber,
                       uint queryFlags,
                       uint parameterFlags,
                       short areaInterpolationMethod,
                       short timeInterpolationMethod,
                       short levelInterpolationMethod,
                       std::string forecastTime,
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
                       T::Coordinate_vec& coordinates);

     void           getGridValues(
                       uchar queryType,
                       Producer_vec& producers,
                       std::set<T::GeometryId>& geometryIdList,
                       uint producerId,
                       std::string& analysisTime,
                       uint generationFlags,
                       std::string& parameterKey,
                       T::ParamLevelId paramLevelId,
                       T::ParamLevel paramLevel,
                       T::ForecastType forecastType,
                       T::ForecastNumber forecastNumber,
                       uint queryFlags,
                       uint parameterFlags,
                       short areaInterpolationMethod,
                       short timeInterpolationMethod,
                       short levelInterpolationMethod,
                       std::string startTime,
                       std::string endTime,
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
                       ParameterValues_vec& valueList,
                       T::Coordinate_vec& coordinates);

     void           getParameterStringInfo(
                       std::string& param,
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
                       short& levelInterpolationMethod);

     void           getParameterStringInfo(
                       std::string& param,
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
                       short& levelInterpolationMethod);

     void          getParameterMappings(
                       std::string& producerName,
                       std::string& parameterName,
                       T::GeometryId geometryId,
                       bool onlySearchEnabled,
                       ParameterMapping_vec& mappings);

     void          getParameterMappings(
                       std::string& producerName,
                       std::string& parameterName,
                       T::GeometryId geometryId,
                       T::ParamLevelIdType levelIdType,
                       T::ParamLevelId levelId,
                       T::ParamLevel level,
                       bool onlySearchEnabled,
                       ParameterMapping_vec& mappings);

     bool           getFunctionParams(
                       std::string& functionParamsStr,
                       FunctionParam_vec& functionParams);

     bool           getParameterFunctionInfo(
                       std::string& paramStr,
                       std::string& function,
                       std::string& functionParams);

     void           getProducers(
                       Query& query,
                       Producer_vec& producers);

     void           getProducers(
                       std::string& producerName,
                       Producer_vec& producers);

     void           getGeometryIdListByCoordinates(
                     Producer_vec& producers,
                     T::AreaCoordinates& coordinates,
                     std::set<T::GeometryId>& geometryIdList);
/*
     void           getGeometryIdListByCoordinates(
                       T::AreaCoordinates& coordinates,
                       std::set<T::GeometryId>& geometryIdList);
*/
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
                       std::string& paramStr,
                       std::string& function,
                       FunctionParam_vec& functionParams,
                       uint recursionCounter,
                       std::vector<QueryParameter>& additionalParameterList);

     void           updateQueryParameters(Query& query);

     bool           conversionFunction(
                       std::string& conversionFunction,
                       std::string& function,
                       std::vector<std::string>& functionParams);

     void           executeConversion(
                       std::string& function,
                       std::vector<std::string>& functionParams,
                       std::string& forecastTime,
                       T::GridValueList& valueList);

     void           executeConversion(
                       std::string& function,
                       std::vector<std::string>& functionParams,
                       T::ParamValue_vec& valueList,
                       T::ParamValue_vec& newValueList);

     void           executeConversion(
                       std::string& function,
                       std::vector<std::string>& functionParams,
                       std::string& forecastTime,
                       T::Coordinate_vec& coordinates,
                       T::ParamValue_vec& valueList,
                       T::ParamValue_vec& newValueList);

     bool           getPointValuesByHeight(
                       T::ProducerInfo& producerInfo,
                       T::GeometryId producerGeometryId,
                       uint generationId,
                       std::string& analysisTime,
                       uint generationFlags,
                       ParameterMapping& pInfo,
                       std::string& forecastTime,
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
                       std::string& analysisTime,
                       uint generationFlags,
                       ParameterMapping& pInfo,
                       std::string& forecastTime,
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
                       std::string& analysisTime,
                       uint generationFlags,
                       ParameterMapping& pInfo,
                       std::string& forecastTime,
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
                       std::string& analysisTime,
                       uint generationFlags,
                       ParameterMapping& pInfo,
                       std::string& forecastTime,
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
                       std::string& analysisTime,
                       uint generationFlags,
                       ParameterMapping& pInfo,
                       std::string& forecastTime,
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
                       std::string& analysisTime,
                       uint generationFlags,
                       ParameterMapping& pInfo,
                       std::string& forecastTime,
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
                       std::string& analysisTime,
                       uint generationFlags,
                       ParameterMapping& pInfo,
                       std::string& forecastTime,
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
                       std::string& analysisTime,
                       uint generationFlags,
                       ParameterMapping& pInfo,
                       std::string& forecastTime,
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
                       std::string& analysisTime,
                       uint generationFlags,
                       ParameterMapping& pInfo,
                       std::string& forecastTime,
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
                       std::string& parameterName,
                       std::string& forecastTime,
                       double x,
                       double y);

     void           getAdditionalValues(
                       std::string& parameterName,
                       uchar coordinateType,
                       T::Coordinate_vec& coordinates,
                       ParameterValues& values);

     bool           getProducerInfoByName(std::string& name,T::ProducerInfo& info);

     int            getContentListByParameterGenerationIdAndForecastTime(
                       T::SessionId sessionId,
                       uint producerId,
                       uint generationId,
                       T::ParamKeyType parameterKeyType,
                       std::string& parameterKey,
                       T::ParamLevelIdType parameterLevelIdType,
                       T::ParamLevelId parameterLevelId,
                       T::ParamLevel level,
                       T::ForecastType forecastType,
                       T::ForecastNumber forecastNumber,
                       T::GeometryId geometryId,
                       std::string& forecastTime,
                       T::ContentInfoList& contentInfoList);

     CacheEntry_sptr getGenerationInfoListByProducerId(uint producerId);


  private:

     // Private attributes

     AliasFileCollection    mAliasFileCollection;
     uint                   mFunctionParamId;
     std::string            mGridConfigFile;
     time_t                 mLastConfiguratonCheck;
     Lua::LuaFileCollection mLuaFileCollection;
     string_vec             mParameterMappingFiles;
     ParamMappingFile_vec   mParameterMappings;
     ParameterMappingCache  mParameterMappingCache;
     std::string            mProducerFile;
     time_t                 mProducerFileModificationTime;
     Producer_vec           mProducerList;
     T::GenerationInfoList  mGenerationInfoList;
     time_t                 mGenerationInfoListUpdateTime;
     Producer_map           mProducerMap;
     time_t                 mProducerMap_updateTime;
     AliasFileCollection    mProducerAliasFileCollection;
     ThreadLock             mThreadLock;
     ContentServer_ptr      mContentServerPtr;
     DataServer_ptr         mDataServerPtr;
     uint                   mCheckInterval;
     LevelHeightCache       mLevelHeightCache;
     ThreadLock             mHeightCacheThreadLock;
     ThreadLock             mParameterMappingCacheThreadLock;

     T::ContentInfoList     mCacheContentInfoList[CONTENT_CACHE_SIZE];
     std::size_t            mContentCacheKey[CONTENT_CACHE_SIZE];
     time_t                 mContentCacheTime[CONTENT_CACHE_SIZE];
     uint                   mContentCacheKeyIdx;

     Functions::FunctionCollection     mFunctionCollection;
     boost::shared_ptr<Fmi::LandCover> mLandCover;
     boost::shared_ptr<Fmi::DEM>       mDem;
     ProducerGenarationListCache       mProducerGenerationListCache;

};


}
}

