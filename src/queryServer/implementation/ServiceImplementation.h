#pragma once

#include "queryServer/definition/ServiceInterface.h"
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
                             std::string parameterFile,
                             std::string parameterAliasFile,
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

     void               checkConfigurationUpdates();
     T::ParamValue      executeFunction(std::string& function,std::vector<T::ParamValue>& parameters);
     void               executeQueryFunctions(Query& query);

     int                executeTimeRangeQuery(Query& query);
     int                executeTimeStepQuery(Query& query);

     void               getGridValues(
                             Producer_vec& producers,
                             ParameterInfo_vec& parameterInfoList,
                             T::ForecastType forecastType,
                             T::ForecastNumber forecastNumber,
                             std::string forecastTime,
                             bool timeMatchRequired,
                             QueryCoordinates& coordinates,
                             bool areaSearch,
                             ParameterValues& valueList);

     void               getGridValues(
                             Producer_vec& producers,
                             ParameterInfo_vec& parameterInfoList,
                             T::ForecastType forecastType,
                             T::ForecastNumber forecastNumber,
                             std::string startTime,
                             std::string endTime,
                             QueryCoordinates& coordinates,
                             bool areaSearch,
                             ParameterValues_vec& valueList);


     void               getParameterInfoList(
                             T::ParamKeyType parameterKeyType,
                             T::ParamId paramKey,
                             T::ParamLevelIdType paramLevelIdType,
                             T::ParamLevelId paramLevelId,
                             T::ParamLevel paramLevel,
                             ParameterInfo_vec& parameterInfoList);

     void               getParameterLevelInfo(
                             std::string param,
                             std::string& key,
                             T::ParamLevelId& paramLevelId,
                             T::ParamLevel& paramLevel,
                             T::ForecastType& forecastType,
                             T::ForecastNumber& forecastNumber);

     bool               getFunctionParams(
                             std::string functionParamsStr,
                             FunctionParam_vec& functionParams);

     bool               getParameterFunctionInfo(
                             std::string paramStr,
                             std::string& function,
                             std::string& functionParams);

     void               getParameterInfoList(
                             std::string parameterName,
                             T::ParamLevelIdType paramLevelIdType,
                             T::ParamLevelId paramLevelId,
                             T::ParamLevel paramLevel,
                             std::vector<ParameterInfo>& infoList);

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
     void               loadParameterFile();
     void               loadParameterAliasFile();
     void               loadProducerFile();
     void               updateQueryParameters(Query& query);


  private:

     std::string        mGridConfigDirectory;

     std::string        mParameterAliasFile;
     time_t             mParameterAliasFileModificationTime;
     ParameterAlias_vec mParameterAliasList;

     std::string        mParameterFile;
     time_t             mParameterFileModificationTime;
     ParameterInfo_vec  mParameterList;

     std::string        mProducerFile;
     time_t             mProducerFileModificationTime;
     Producer_vec       mProducerList;

     std::string        mLuaFunctionFile;
     time_t             mLuaFunctionFileModificationTime;

     time_t             mLastConfiguratonCheck;
     ThreadLock         mThreadLock;

     uint               mServerCounter;
     time_t             mServerCheckTime;

     uint               mFunctionParamId;

  private:

     DataServer::ServiceInterface*      getServerPtr(unsigned long long serverFlags);
     void                               checkDataServerConnections();

     ContentServer::ServiceInterface*   mContentServer;
     DataServer::ServiceInterface*      mDataServer;
     DataServer::ServiceInterface*      mDataServerClient[64];


};


}
}

