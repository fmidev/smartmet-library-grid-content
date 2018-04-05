#pragma once

#include "queryServer/definition/ServiceInterface.h"
#include "queryServer/definition/AliasFileCollection.h"
#include "queryServer/definition/ParameterMappingFile.h"
#include "dataServer/definition/ServiceInterface.h"
#include "contentServer/definition/ServiceInterface.h"
#include "lua/LuaFileCollection.h"


namespace SmartMet
{
namespace QueryServer
{

typedef std::vector<std::pair<std::string,T::GeometryId>> Producer_vec;
typedef ContentServer::ServiceInterface* ContentServer_ptr;
typedef DataServer::ServiceInterface* DataServer_ptr;
typedef std::pair<int,double> LevelHeight;
typedef std::vector<LevelHeight> LevelHeight_vec;
typedef std::vector<std::pair<std::string,LevelHeight_vec>> LevelHeightCache;

typedef std::pair<std::string,ParameterMapping_vec> ParameterMappingCacheRec;
typedef std::list<ParameterMappingCacheRec> ParameterMappingCache;


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
                       std::string prdoucerAliasFile,
                       string_vec& luaFileNames);

     virtual void   shutdown();

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
     bool           getAlias(std::string name,std::string& alias);
     void           executeQueryFunctions(Query& query);
     int            executeTimeRangeQuery(Query& query);
     int            executeTimeStepQuery(Query& query);

     void           getGridValues(
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
                       QuerySearchType searchType,
                       QueryCoordinates& coordinates,
                       double radius,
                       double dem,
                       ushort coverType,
                       ParameterValues& valueList);

     void           getGridValues(
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
                       std::string startTime,
                       std::string endTime,
                       bool ignoreStartTimeValue,
                       QuerySearchType searchType,
                       QueryCoordinates& coordinates,
                       double radius,
                       double dem,
                       ushort coverType,
                       uint maxValues,
                       ParameterValues_vec& valueList);

     void           getParameterStringInfo(
                       std::string param,
                       std::string& key,
                       T::ParamLevelId& paramLevelId,
                       T::ParamLevel& paramLevel,
                       T::ForecastType& forecastType,
                       T::ForecastNumber& forecastNumber,
                       uint& producerId,
                       uint& generationFlags,
                       short& areaInterpolationMethod,
                       short& timeInterpolationMethod,
                       short& levelInterpolationMethod);

     void          getParameterMappings(
                       std::string producerName,
                       std::string parameterName,
                       bool onlySearchEnabled,
                       ParameterMapping_vec& mappings);

     void          getParameterMappings(
                       std::string producerName,
                       std::string parameterName,
                       T::ParamLevelIdType levelIdType,
                       T::ParamLevelId levelId,
                       T::ParamLevel level,
                       bool onlySearchEnabled,
                       ParameterMapping_vec& mappings);

     bool           getFunctionParams(
                       std::string functionParamsStr,
                       FunctionParam_vec& functionParams);

     bool           getParameterFunctionInfo(
                       std::string paramStr,
                       std::string& function,
                       std::string& functionParams);

     void           getProducerList(
                       Query& query,
                       Producer_vec& producers);

     void           getGeometryIdListByCoordinates(
                       QueryCoordinates& coordinates,
                       std::set<T::GeometryId>& geometryIdList);

     void           loadProducerFile();

     bool           parseFunction(
                       QueryParameter& queryParam,
                       std::string paramStr,
                       std::string& function,
                       FunctionParam_vec& functionParams,
                       uint recursionCounter,
                       std::vector<QueryParameter>& additionalParameterList);

     void           updateQueryParameters(Query& query);

     bool           conversionFunction(std::string& conversionFunction,std::string& function,std::vector<std::string>& functionParams);
     void           executeConversion(std::string& function,std::vector<std::string>& functionParams,T::GridValueList& valueList);

     void           timeInterpolation(
                       short timeInterpolationMethod,
                       std::string forecastTimeStr,
                       std::string forecastTimeStr1,
                       std::string forecastTimeStr2,
                       T::GridValueList& list1,
                       T::GridValueList& list2,
                       T::GridValueList& valueList);

     void           levelInterpolation(
                       short levelInterpolationMethod,
                       double level,
                       double level1,
                       double level2,
                       ParameterValues& parameterValues1,
                       ParameterValues& parameterValues2,
                       ParameterValues& valueList);


     bool           getPolygonValues(
                       T::ProducerInfo& producerInfo,
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
                       ParameterValues& valueList);

     bool           getCircleValues(
                       T::ProducerInfo& producerInfo,
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
                       ParameterValues& valueList);

     bool           getPointValues(
                       T::ProducerInfo& producerInfo,
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
                       ParameterValues& valueList);

     bool           getSpecialValues(
                       T::ProducerInfo& producerInfo,
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
                       ParameterValues& valueList);

     bool           getPressureLevelsAndHeights(T::ProducerInfo& producerInfo,uint generationId,std::string forecastTime,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,double x,double y,int height,int& lowerPressure,int& higherPressure,double& lowerHeight,double& higherHeight);


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
     AliasFile              mProducerAliasFile;
     ThreadLock             mThreadLock;
     ContentServer_ptr      mContentServerPtr;
     DataServer_ptr         mDataServerPtr;
     uint                   mCheckInterval;
     LevelHeightCache       mLevelHeightCache;
     ThreadLock             mCacheThreadLock;
     ThreadLock             mParameterMappingCacheThreadLock;
};


}
}

