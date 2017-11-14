#pragma once

#include "queryServer/definition/ServiceInterface.h"
#include "queryServer/definition/ParameterAliasList.h"
#include "queryServer/definition/ParameterMappingList.h"
#include "contentServer/definition/ServiceInterface.h"
#include "dataServer/definition/ServiceInterface.h"


namespace SmartMet
{
namespace QueryServer
{

typedef std::vector<std::pair<std::string,T::GeometryId>> Producer_vec;



class ServiceImplementation : public ServiceInterface
{
  public:
                        ServiceImplementation();
     virtual            ~ServiceImplementation();

     virtual void       init (
                             ContentServer::ServiceInterface *contentServer,
                             DataServer::ServiceInterface *dataServer,
                             std::string gridConfigDirectory,
                             std::vector<std::string> parameterMappingFiles,
                             std::vector<std::string> parameterAliasFiles,
                             std::string producerFile,
                             std::string luaFunctionFile);

     virtual void       shutdown();

  protected:

     virtual int        _executeQuery(T::SessionId sessionId,Query& query);

     virtual int        _getValuesByGridPoint(
                             T::SessionId sessionId,
                             T::ContentInfoList& contentInfoList,
                             T::CoordinateType coordinateType,
                             double x,
                             double y,
                             T::InterpolationMethod interpolationMethod,
                             T::GridPointValueList& valueList);


  protected:

     bool               getAlias(std::string name,std::string& alias);
     void               checkConfigurationUpdates();
     T::ParamValue      executeFunction(std::string& function,std::vector<T::ParamValue>& parameters);
     void               executeQueryFunctions(Query& query);

     int                executeTimeRangeQuery(Query& query);
     int                executeTimeStepQuery(Query& query);

     void               getGridValues(
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

     void               getGridValues(
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

     void               getParameterStringInfo(
                             std::string param,
                             std::string& key,
                             T::ParamLevelId& paramLevelId,
                             T::ParamLevel& paramLevel,
                             T::ForecastType& forecastType,
                             T::ForecastNumber& forecastNumber,
                             uint& producerId,
                             uint& generationFlags);

     bool               getFunctionParams(
                             std::string functionParamsStr,
                             FunctionParam_vec& functionParams);

     bool               getParameterFunctionInfo(
                             std::string paramStr,
                             std::string& function,
                             std::string& functionParams);

     void               getProducerList(
                             Query& query,
                             Producer_vec& producers);

     void               getGeometryIdListByCoordinates(
                             QueryCoordinates& coordinates,
                             std::set<T::GeometryId>& geometryIdList);

     bool               parseFunction(
                             QueryParameter& queryParam,
                             std::string paramStr,
                             std::string& function,
                             FunctionParam_vec& functionParams,
                             std::vector<QueryParameter>& additionalParameterList);

     void               loadLuaFunctionFile();
     void               loadProducerFile();

     void               updateQueryParameters(Query& query);


  private:

     std::string        mProducerFile;
     time_t             mProducerFileModificationTime;
     Producer_vec       mProducerList;

     std::string        mLuaFunctionFile;
     time_t             mLuaFunctionFileModificationTime;

     time_t             mLastConfiguratonCheck;
     ThreadLock         mThreadLock;

     std::string        mGridConfigDirectory;
     uint               mFunctionParamId;

  private:

     DataServer::ServiceInterface*      getServerPtr(unsigned long long serverFlags);
     void                               checkDataServerConnections();

     ContentServer::ServiceInterface*   mContentServer;
     DataServer::ServiceInterface*      mDataServer;
     DataServer::ServiceInterface*      mDataServerClient[64];
     uint                               mServerCounter;
     time_t                             mServerCheckTime;

     std::vector<std::string>           mParameterMappingFiles;
     std::vector<ParameterMappingList>  mParameterMappings;

     std::vector<std::string>           mParameterAliasFiles;
     std::vector<ParameterAliasList>    mParameterAliases;
};


}
}

