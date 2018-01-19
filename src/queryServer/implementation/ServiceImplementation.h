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
typedef ContentServer::ServiceInterface*                  ContentServer_ptr;
typedef DataServer::ServiceInterface*                     DataServer_ptr;



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

     virtual int    _getValuesByGridPoint(
                       T::SessionId sessionId,
                       T::ContentInfoList& contentInfoList,
                       T::CoordinateType coordinateType,
                       double x,
                       double y,
                       T::InterpolationMethod interpolationMethod,
                       T::GridPointValueList& valueList);

  private:

     // Private methods

     void           checkConfigurationUpdates();
     bool           getAlias(std::string name,std::string& alias);
     double         executeFunctionCall1(std::string& function,std::vector<double>& parameters);
     void           executeQueryFunctions(Query& query);
     int            executeTimeRangeQuery(Query& query);
     int            executeTimeStepQuery(Query& query);

     void           getGridValues(
                       Producer_vec& producers,
                       uint producerId,
                       uint generationFlags,
                       std::string parameterKey,
                       T::ParamLevelId paramLevelId,
                       T::ParamLevel paramLevel,
                       T::ForecastType forecastType,
                       T::ForecastNumber forecastNumber,
                       std::string forecastTime,
                       bool timeMatchRequired,
                       QueryCoordinates& coordinates,
                       bool areaSearch,
                       ParameterValues& valueList);

     void           getGridValues(
                       Producer_vec& producers,
                       uint producerId,
                       uint generationFlags,
                       std::string parameterKey,
                       T::ParamLevelId paramLevelId,
                       T::ParamLevel paramLevel,
                       T::ForecastType forecastType,
                       T::ForecastNumber forecastNumber,
                       std::string startTime,
                       std::string endTime,
                       QueryCoordinates& coordinates,
                       bool areaSearch,
                       ParameterValues_vec& valueList);

     void           getParameterStringInfo(
                       std::string param,
                       std::string& key,
                       T::ParamLevelId& paramLevelId,
                       T::ParamLevel& paramLevel,
                       T::ForecastType& forecastType,
                       T::ForecastNumber& forecastNumber,
                       uint& producerId,
                       uint& generationFlags);

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

  private:

     // Private attributes

     AliasFileCollection    mAliasFileCollection;
     uint                   mFunctionParamId;
     std::string            mGridConfigFile;
     time_t                 mLastConfiguratonCheck;
     Lua::LuaFileCollection mLuaFileCollection;
     string_vec             mParameterMappingFiles;
     ParamMappingFile_vec   mParameterMappings;
     std::string            mProducerFile;
     time_t                 mProducerFileModificationTime;
     Producer_vec           mProducerList;
     AliasFile              mProducerAliasFile;
     ThreadLock             mThreadLock;
     ContentServer_ptr      mContentServerPtr;
     DataServer_ptr         mDataServerPtr;
     uint                   mCheckInterval;
};


}
}

