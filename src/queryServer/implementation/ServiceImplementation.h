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
#include <unordered_set>
#include <atomic>


namespace SmartMet
{
namespace GRID
{
class GridFile;
class Message;
}

namespace QueryServer
{

using Producer_vec = std::vector<std::pair<std::string,T::GeometryId>>;               //!< Ordered list of (producer name, geometry id) pairs.
using Producer_map = std::unordered_map<std::string,T::ProducerInfo>;               //!< Map from producer name to ProducerInfo.
using Producer_map_sptr = std::shared_ptr<Producer_map>;          //!< Shared pointer to a Producer_map.
using ContentServer_ptr = ContentServer::ServiceInterface*;          //!< Non-owning pointer to a ContentServer service.
using DataServer_ptr = DataServer::ServiceInterface*;             //!< Non-owning pointer to a DataServer service.
using ParameterMapping_vec_sptr = std::shared_ptr<ParameterMapping_vec>;  //!< Shared pointer to a ParameterMapping vector.
using ParameterMappingCache = boost::unordered_map<size_t,ParameterMapping_vec_sptr>;   //!< Hash cache from query hash to resolved ParameterMapping vector.
using StringVector_sptr = std::shared_ptr<std::vector<std::string>>;          //!< Shared pointer to a string vector.
using Altenative_map = std::unordered_map<std::string,Producer_vec>;             //!< Map from producer name to its alternative (concatenated) producers.


// ====================================================================================
/*! \brief Internal cache entry holding generation info for one producer. */
// ====================================================================================
class CacheEntry
{
  public:
    T::GenerationInfoList_sptr generationInfoList;  //!< Cached list of generations for this producer.
    StringVector_sptr          analysisTimes;        //!< Cached list of analysis time strings.
    UInt64                     producerHash = 0;         //!< Hash of the producer state when this entry was built.
};

using CacheEntry_sptr = std::shared_ptr<CacheEntry>;                                          //!< Shared pointer to a CacheEntry.
using ProducerGenarationListCache = std::unordered_map<T::ProducerId,CacheEntry_sptr>;        //!< Cache of CacheEntry records keyed by producer id.

// ====================================================================================
/*! \brief Internal cache entry holding a ContentInfoList for a specific query. */
// ====================================================================================
class ContentCacheEntry
{
  public:
    T::ContentInfoList contentInfoList;  //!< Cached content records matching the query.
    UInt64 producerHash = 0;                 //!< Producer state hash when this entry was built.
    T::GenerationId generationId = 0;        //!< Generation identifier for which the content was cached.
};

using ContentCacheEntry_sptr = std::shared_ptr<ContentCacheEntry>;                            //!< Shared pointer to a ContentCacheEntry.

using ContentCache = std::unordered_map<std::size_t,ContentCacheEntry_sptr>;                  //!< Hash cache from query hash to ContentCacheEntry.


// ====================================================================================
/*! \brief Internal cache entry for content search results. */
// ====================================================================================
class ContentSearchCacheEntry
{
  public:
    std::shared_ptr<T::ContentInfoList> contentInfoList;  //!< Shared cached content list from the search.
    UInt64 producerHash = 0;                                   //!< Producer state hash when this entry was built.
    T::GenerationId generationId = 0;                          //!< Generation identifier for this cache entry.
};
using ContentSearchCache = std::unordered_map<std::size_t,ContentSearchCacheEntry>;           //!< Hash cache from query hash to ContentSearchCacheEntry.


// ====================================================================================
/*! \brief Height interpolation bracket: two level/height pairs at two time steps. */
// ====================================================================================
class HeightRec
{
  public:
    HeightRec()   { level1_1 = 0; level1_2 = 0; height1_1 = 0; height1_2 = 0;time2 = 0; level2_1 = 0; level2_2 = 0;height2_1 = 0; height2_2 = 0;}

    T::ParamLevelId levelId{0};     //!< Level type identifier for this height bracket.
    time_t time1{0};                //!< Forecast time of the first bracketing level pair.
    T::ParamLevel level1_1;      //!< Lower level at time1.
    T::ParamLevel level1_2;      //!< Upper level at time1.
    T::ParamValue height1_1;     //!< Height (m) corresponding to level1_1 at time1.
    T::ParamValue height1_2;     //!< Height (m) corresponding to level1_2 at time1.

    time_t time2;                //!< Forecast time of the second bracketing level pair.
    T::ParamLevel level2_1;      //!< Lower level at time2.
    T::ParamLevel level2_2;      //!< Upper level at time2.
    T::ParamValue height2_1;     //!< Height (m) corresponding to level2_1 at time2.
    T::ParamValue height2_2;     //!< Height (m) corresponding to level2_2 at time2.
};

using HeightRec_vec = std::vector<HeightRec>;                                                 //!< Ordered list of HeightRec records.



// ====================================================================================
/*! \brief Search criteria of the height parameter contents used in the height to level conversions. */
// ====================================================================================
class HeightQuery
{
  public:
    T::ProducerId   heightProducerId = 0;       //!< Producer of the height parameter.
    std::string     heightParameter;            //!< Name of the height parameter.
    T::GenerationId heightGenerationId = 0;     //!< Generation of the height producer.
    int             geometryId = 0;             //!< Geometry of the contents.
    T::ParamLevelId paramLevelId = 0;           //!< Level type of the contents.
    int             forecastType = 0;           //!< Forecast type of the contents.
    int             forecastNumber = 0;         //!< Forecast number of the contents.
    uint            firstLevel = 0xFFFFFFFF;    //!< First level of the height parameter.
    uint            lastLevel = 0xFFFFFFFF;     //!< Last level of the height parameter.
};

using HeightCache = Fmi::Cache::Cache<std::size_t,HeightRec>;                                 //!< LRU cache from query hash to HeightRec.
using LevelCache = Fmi::Cache::Cache<std::size_t,std::size_t>;                                //!< LRU cache from query hash to resolved level index.
using HeightVecCache = Fmi::Cache::Cache<std::size_t,std::shared_ptr<T::ParamValue_vec>>;     //!< LRU cache from query hash to height value vector.
using LevelContentCache = Fmi::Cache::Cache<std::size_t,ContentCacheEntry_sptr>;              //!< LRU cache from query hash to ContentCacheEntry for level queries.


// ====================================================================================
/*! \brief Rule for converting model levels to heights via an auxiliary height parameter. */
// ====================================================================================
class HeightConversion
{
  public:
    HeightConversion()   { multiplier = 1.0;}

    T::ProducerId   producerId{0};       //!< Producer whose levels are converted.
    T::ParamLevelId levelId = 0;          //!< Level type to which this conversion applies.
    T::ProducerId   heightProducerId{0}; //!< Producer that supplies the height field.
    std::string     heightParameter;  //!< Parameter name of the auxiliary height field.
    double          multiplier;       //!< Scale factor applied to the height values.
};

using HeightConversionMap = std::unordered_map<std::size_t,HeightConversion>;                 //!< Map from hash key to HeightConversion rule.


// ====================================================================================
/*! \brief State shared by the parameters of a time range or time step query while their values are fetched. */
// ====================================================================================
class QueryFetchState
{
  public:
    Producer_vec              producers;                            //!< Producers of the query.
    std::set<T::GeometryId>   geometryIdList;                       //!< Geometries that support the query coordinates.
    T::Coordinate_vec         coordinates;                          //!< Coordinates of the first values found.
    std::string               analysisTime;                         //!< Analysis time required (or selected) for the parameters.
    std::unordered_set<uint>  alternativeRequired;                  //!< Ids of the alternative parameters that must be fetched.
    uint                      queryFlags = 0;                       //!< Query flags (the analysis time match requirement can be added).
    int                       globalGeometryId = 0;                 //!< Geometry of the first values of a parameter without a geometry.
    bool                      containsInternalAggregation = false;  //!< Whether the query has internal aggregation parameters.
};


// ====================================================================================
/*! \brief Parsed data parameter of a query: the name and the producer and generation selections. */
// ====================================================================================
class ParameterRequest
{
  public:
    std::string     paramName;                //!< Parameter name (without the '$' prefix).
    std::string     producerName;             //!< Requested producer name (or empty).
    T::ProducerId   producerId = 0;           //!< Requested (or selected) producer.
    int             geometryId = 0;           //!< Requested geometry (or 0).
    UInt64          generationFlags = 0;      //!< Requested generations (the query generations if 0).
    std::size_t     paramHash = 0;            //!< Hash of the parameter name.
    bool            useAlternative = false;   //!< Whether the alternative parameter is used (the geometry does not cover the area).
};


// ====================================================================================
/*! \brief Arguments of the grid value request of a query parameter for a forecast time (see getGridValues). */
// ====================================================================================
class GridValueRequest
{
  public:
    Query&              query;                      //!< The query.
    QueryParameter&     qParam;                     //!< The query parameter.
    T::ProducerId&      producerId;                 //!< Requested producer (updated by the value searches).
    const std::string&  analysisTime;               //!< Requested analysis time (or empty).
    const std::string&  maxAnalysisTime;            //!< Maximum analysis time (or empty).
    UInt64              generationFlags;            //!< Accepted generations (0 = all).
    const std::string&  parameterKey;               //!< Parameter key.
    std::size_t         parameterHash;              //!< Hash of the parameter key.
    uint                queryFlags;                 //!< Query flags.
    time_t              forecastTime;               //!< Requested forecast time.
    bool                timeMatchRequired;          //!< Whether the forecast time must match.
    ParameterValues&    valueList;                  //!< Result values.
    std::string&        producerStr;                //!< Producer and geometry of the values ("PRODUCER:geometry").
    time_t              requiredAccessTime;         //!< The generation must not be deleted before this time.
    bool                acceptNotReadyGenerations;  //!< Whether generations that are not ready are accepted.
};


// ====================================================================================
/*! \brief Arguments and state of the grid value request of a query parameter for a time range (see getGridValues). */
// ====================================================================================
class TimeRangeValueRequest
{
  public:
    Query&              query;                            //!< The query.
    QueryParameter&     qParam;                           //!< The query parameter.
    T::ProducerId       producerId;                       //!< Requested producer (or 0).
    const std::string&  analysisTime;                     //!< Requested analysis time (or empty).
    UInt64              generationFlags;                  //!< Accepted generations (0 = all).
    const std::string&  parameterKey;                     //!< Parameter key.
    std::size_t         parameterHash;                    //!< Hash of the parameter key.
    uint                queryFlags;                       //!< Query flags.
    time_t              startTime;                        //!< Start time of the time range.
    time_t              endTime;                          //!< End time of the time range.
    std::string&        producerStr;                      //!< Producer and geometry of the values ("PRODUCER:geometry").
    uint                valueCounter;                     //!< Number of the time steps selected so far.
    uint                recursionCounter;                 //!< Recursion depth of the concatenated producer searches.
    std::string         maxAnalysisTime;                  //!< Maximum analysis time (or empty).
    uint                queryTimestepSizeInSeconds;       //!< Time step of the query.
    uint                parameterTimestepSizeInSeconds;   //!< Time step of the aggregation of the parameter.
    time_t              requiredAccessTime;               //!< The generation must not be deleted before this time.
    bool                ignoreStartTimeValue;             //!< Whether the value of the start time is left out.
    bool                timeStepIsData;                   //!< Whether the time steps are the data times.
    bool                acceptNotReadyGenerations;        //!< Whether generations that are not ready are accepted.
};


// ====================================================================================
/*! \brief Main implementation of the QueryServer ServiceInterface.
 *
 *  Combines a ContentServer and DataServer to execute named-parameter queries with
 *  time ranges and coordinate specifications.  Manages parameter mapping files,
 *  alias files, Lua scripts, and multiple levels of internal caches.  A background
 *  thread handles periodic reloads of configuration files and producer map updates. */
// ====================================================================================

class ServiceImplementation : public ServiceInterface
{
  public:
                    ServiceImplementation();
     ~ServiceImplementation() override;
     ServiceImplementation(const ServiceImplementation&) = delete;
     ServiceImplementation& operator=(const ServiceImplementation&) = delete;

     void           init(
                       ContentServer::ServiceInterface *contentServerPtr,
                       DataServer::ServiceInterface *dataServerPtr,
                       const std::string& gridConfigFile,
                       const std::string& heightConversionFile,
                       const string_vec& parameterMappingFiles,
                       const std::string& unitConversionFile,
                       const string_vec& parameterMappingAliasFiles,
                       string_vec& aliasFiles,
                       const std::string& producerFile,
                       string_vec& producerAliasFiles,
                       const string_vec& luaFileNames,
                       bool checkGeometryStatus,
                       bool dataServerMethodsEnabled);

     void           getCacheStats(Fmi::Cache::CacheStatistics& statistics) const override;
     void           getStateAttributes(std::shared_ptr<T::AttributeNode> parent) override;
     void           initContentCache(std::size_t maxRecordsPerThread,uint clearIntervalInSeconds);
     void           initContentSearchCache(std::size_t maxRecordsPerThread,uint clearIntervalInSeconds);

     void           shutdown() override;
     void           setDem(std::shared_ptr<Fmi::DEM> dem) override;
     void           setLandCover(std::shared_ptr<Fmi::LandCover> landCover) override;
     void           updateProcessing();

  protected:

     // Methods that can be called through the service interface

     int            _executeQuery(T::SessionId sessionId,Query& query) override;

     int            _getProducerList(T::SessionId sessionId,string_vec& producerList) override;

     int            _getValuesByGridPoint(
                       T::SessionId sessionId,
                       T::ContentInfoList& contentInfoList,
                       T::CoordinateType coordinateType,
                       double x,
                       double y,
                       short areaInterpolationMethod,
                       T::GridPointValueList& valueList) override;

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
     void           printQueryStart(Query& query);
     void           printQueryEnd();
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

     void          checkParameterMappingCacheGeneration();

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
                       const Query& query,
                       Producer_vec& producers);

     void           getProducers(
                       const std::string& producerName,
                       Producer_vec& producers);

     void           reportUnregisteredGeometry(T::GeometryId geometryId);

     void           getGeometryIdListByCoordinates(
                     Producer_vec& producers,
                     T::AreaCoordinates& coordinates,
                     std::set<T::GeometryId>& geometryIdList);

     void           getGeometryIdListByCoordinates(
                       uint gridWidth,
                       uint gridHeight,
                       const std::vector<T::Coordinate>& coordinates,
                       std::set<T::GeometryId>& geometryIdList);

     void           getGeometryIdListByGeometryId(
                       T::GeometryId gridGeometryId,
                       std::set<T::GeometryId>& geometryIdList);

     void           loadProducerFile();
     void           addProducerField(uint t,const char *field,std::string& key);
     void           addFunctionParam(const std::string& pname,FunctionParam_vec& functionParams);
     QueryParameter newFunctionParameter(const QueryParameter& queryParam,const std::string& paramStr,const FunctionParam& functionParam,int contourCoordinateType);
     bool           updateQueryParameter(Query& query,QueryParameter& qParam,std::vector<QueryParameter>& additionalParameterList);
     void           checkContentServerRestart();
     void           checkThreadCacheState();
     void           printContentList(T::ContentInfoList& contentList);
     bool           getContentLevels(const T::ProducerInfo& producerInfo,T::GenerationId generationId,T::GeometryId producerGeometryId,const ParameterMapping& pInfo,T::ParamLevelId paramLevelId,std::vector<T::ParamLevel>& parameterLevels);
     bool           getHeightLevelValueVectors(const Query& query,T::Coordinate_vec& coordinates,const T::ProducerInfo& producerInfo,T::GeometryId producerGeometryId,T::GenerationId generationId,ParameterMapping& pInfo,time_t forecastTime,T::ParamLevelId levelId,const std::set<T::ParamLevel>& levels,bool requireExactTime,const QueryParameter& qParam,short areaInterpolationMethod,double_vec& modificationParameters,std::vector<std::shared_ptr<T::ParamValue_vec>>& valueVectors);
     bool           getHeightInterpolatedValueVector(const Query& query,T::Coordinate_vec& coordinates,HeightRec_vec& recs,uint timeIndex,const T::ProducerInfo& producerInfo,T::GeometryId producerGeometryId,T::GenerationId generationId,ParameterMapping& pInfo,time_t forecastTime,T::ParamLevel paramLevel,const QueryParameter& qParam,short areaInterpolationMethod,short levelInterpolationMethod,double_vec& modificationParameters,T::ParamValue_vec& valueVector);
     void           setHeightValueVector(QueryParameter& qParam,const ParameterMapping& pInfo,bool conversionByFunction,const std::string& function,std::vector<std::string>& functionParams,time_t forecastTime,T::Coordinate_vec& coordinates,T::ParamValue_vec& valueVector,ParameterValues& valueList);
     void           printQueryObject(Query& query,const char *title);
     void           executeLuaVectorFunction(uint type,const std::string& luaFunction,const std::string& subfunction,uint columns,uint rows,std::vector<T::ParamValue_vec>& inputVectors,const std::vector<double>& parameters,T::ParamValue_vec& outputVector);
     void           executeVectorFunction(const std::string& function,const std::string& subfunction,uint columns,uint rows,std::vector<T::ParamValue_vec>& inputVectors,const std::vector<double>& parameters,T::ParamValue_vec& outputVector);
     void           executeTimeStepVectorFunction(Query& query,QueryParameter& qParam,const std::string& function,const std::string& subfunction,uint columns,uint rows,int tCount,time_t forecastTime);
     void           executeRowRangeVectorFunction(Query& query,QueryParameter& qParam,uint columns,uint rows);
     time_t         getPreviousOrLevelContentList(const T::ProducerInfo& producerInfo,T::GeometryId producerGeometryId,T::GenerationId generationId,ParameterMapping& pInfo,time_t forecastTime,T::ParamLevelId levelId,T::ParamLevel level,const QueryParameter& qParam,std::shared_ptr<T::ContentInfoList>& prevContentList,T::ParamLevel& prevLevel,time_t& prevTime,std::shared_ptr<T::ContentInfoList>& contentList);
     bool           getHeightPointValue(const T::ContentInfo& contentInfo,T::CoordinateType coordinateType,const T::Coordinate& coordinate,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value);
     bool           addLevelInterpolatedPointValue(const Query& query,const T::Coordinate& coordinate,const HeightRec& rec,const QueryParameter& qParam,const T::ContentInfo& contentInfo1_1,const T::ContentInfo& contentInfo2_1,T::ParamLevel paramLevel,short areaInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,ParameterValues& valueList);
     const T::ContentInfo* getSecondTimeLevelContent(const T::ProducerInfo& producerInfo,T::GeometryId producerGeometryId,T::GenerationId generationId,ParameterMapping& pInfo,time_t forecastTime,T::ParamLevelId levelId,T::ParamLevel level,const QueryParameter& qParam,time_t time2,std::shared_ptr<T::ContentInfoList>& contentList);
     bool           addTimeLevelInterpolatedPointValue(const Query& query,const T::Coordinate& coordinate,const HeightRec& rec,const QueryParameter& qParam,const T::ProducerInfo& producerInfo,T::GeometryId producerGeometryId,T::GenerationId generationId,ParameterMapping& pInfo,const T::ContentInfo& lowerContent1,const T::ContentInfo& upperContent1,time_t forecastTime,T::ParamLevel paramLevel,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,ParameterValues& valueList);
     bool           addPointValuesByHeight(const Query& query,T::Coordinate_vec& coordinates,const HeightRec_vec& recs,QueryParameter& qParam,const T::ProducerInfo& producerInfo,T::GeometryId producerGeometryId,T::GenerationId generationId,const std::string& analysisTime,UInt64 generationFlags,ParameterMapping& pInfo,time_t forecastTime,T::ParamLevelId paramLevelId,T::ParamLevel paramLevel,T::ProducerId& newProducerId,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,ParameterValues& valueList);
     void           getQueryGeometryIdList(Query& query,Producer_vec& producers,std::set<T::GeometryId>& geometryIdList);
     void           parseParameterRequest(const Query& query,QueryParameter& qParam,QueryFetchState& state,ParameterRequest& req);
     void           checkAlternativeRequest(Query& query,const QueryParameter& qParam,QueryFetchState& state,ParameterRequest& req);
     void           fetchTimeRangeParameter(Query& query,QueryParameter& qParam,QueryFetchState& state,std::unordered_map<std::string,T::ProducerId>& parameterProducers);
     void           getTimeRangeProducersAndGeometries(const QueryParameter& qParam,const QueryFetchState& state,const ParameterRequest& req,Producer_vec& tmpProducers,std::set<T::GeometryId>& geomIdList);
     void           fetchTimeRangeValues(Query& query,QueryParameter& qParam,QueryFetchState& state,std::unordered_map<std::string,T::ProducerId>& parameterProducers,ParameterRequest& req);
     void           addMissingTimeRangeValues(const Query& query,QueryParameter& qParam,time_t tt,size_t timeCnt,const std::set<time_t>& originalTimeList,const QueryFetchState& state);
     void           printQueryParameter(QueryParameter& qParam);
     void           fetchTimeStepParameter(Query& query,QueryParameter& qParam,QueryFetchState& state);
     void           selectTimeStepProducersAndGeometries(const QueryParameter& qParam,QueryFetchState& state,const ParameterRequest& req,Producer_vec& tmpProducers,std::set<T::GeometryId>& tmpGeomIdList,Producer_vec*& producersPtr,std::set<T::GeometryId>*& geomIdListPtr);
     bool           fetchConcatenatedProducerValues(Query& query,QueryParameter& qParam,QueryFetchState& state,const ParameterRequest& req,time_t forecastTime,UInt64 gflags,std::string& producerStr,std::set<T::GeometryId>& tmpGeomIdList,ParameterValues& valueList);
     void           fetchTimeStepValues(Query& query,QueryParameter& qParam,QueryFetchState& state,ParameterRequest& req,time_t forecastTime,UInt64& gflags,Producer_vec& tmpProducers,std::set<T::GeometryId>& tmpGeomIdList,ParameterValues& valueList);
     void           updateTimeStepFetchState(const Query& query,const QueryParameter& qParam,QueryFetchState& state,ParameterRequest& req,bool concat,UInt64& gflags,ParameterValues& valueList);
     void           addMissingTimeStepValues(const Query& query,QueryParameter& qParam,time_t tt,const std::unordered_set<time_t>& additionalTimeList,const QueryFetchState& state);
     void           executePointFunction(const Query& query,const std::string& function,std::vector<double>& parameters,const std::vector<std::string>& strParameters,const T::GridValue& lastRec,ParameterValues& values);
     void           executeAreaFunction(const Query& query,const std::string& func,const std::vector<double>& areaParameters,std::vector<double>& extParameters,ParameterValues& values);
     void           executeTimeStepFunction(Query& query,QueryParameter& qParam,const std::string& function,uint valueCount,bool onlyAvailableParams,int tCount,time_t forecastTime);
     void           executeRowRangeFunction(Query& query,QueryParameter& qParam);
     void           executeRowRangeValueFunction(const Query& query,Functions::FunctionPtr functionPtr,uint type,const std::string& luaFunction,std::vector<T::ParamValue>& values,const T::GridValue& rec,T::GridValueList& list);
     bool           getGridFileProperties(const T::ContentInfo& contentInfo1,std::map<uint,Int64 >& oldProperties);
     GRID::Message* newGridFileMessage(const Query& query,uint version,time_t forecastTime,T::ParamLevel paramLevel,const T::ContentInfo& contentInfo1,const std::map<uint,Int64 >& oldProperties,GRID::GridFile& newGridFile);
     bool           addGridFileData(int result,const char *service,const T::ContentInfo& contentInfo1,bool levelInterpolated,T::ParamLevel paramLevel,QueryParameter& qParam,const ParameterMapping& pInfo,bool conversionByFunction,const std::string& function,std::vector<std::string>& functionParams,time_t forecastTime,T::Coordinate_vec& coordinates,T::ParamValue_vec& valueVector,GRID::GridFile& newGridFile,GRID::Message& newMessage,UInt64 sz,ParameterValues& valueList);
     void           printGridValueRequest(const GridValueRequest& r,const Producer_vec& producers,const std::set<T::GeometryId>& geometryIdList);
     bool           isAreaProducerAccepted(std::size_t producerCount,const QueryParameter& qParam,const std::string& producerName,T::GeometryId producerGeometryId);
     ParameterMapping_vec_sptr getGridValueMappings(const T::ProducerInfo& producerInfo,const std::string& parameterKey,std::size_t parameterHash,T::GeometryId producerGeometryId,const QueryParameter& qParam);
     T::GenerationInfo* getRequestedGenerationInfo(const CacheEntry& cacheEntry,const std::string& analysisTime,uint queryFlags,bool acceptNotReadyGenerations);
     bool           hasClimatologicalMappings(const ParameterMapping_vec& mappings);
     bool           isGenerationSelected(const GridValueRequest& r,const std::vector<std::string>& analysisTimes,uint g,UInt64 gflags);
     bool           isGenerationReady(const T::GenerationInfo& generationInfo,T::GeometryId producerGeometryId);
     void           printGeneration(const std::string& analysisTime,const std::vector<std::string>& analysisTimes,uint g,bool requestedGeneration);
     bool           getGenerationsGridValues(GridValueRequest& r,const std::string& producerName,T::GeometryId producerGeometryId,const T::ProducerInfo& producerInfo,const CacheEntry& cacheEntry,const std::vector<std::string>& analysisTimes,T::GenerationInfo* origGenerationInfo,ParameterMapping_vec& mappings,bool climatologicalMappings);
     void           printInvalidProducer(const std::string& producerName,bool producerOk);
     bool           isGenerationUsable(const GridValueRequest& r,T::GenerationInfo *generationInfo,T::GeometryId producerGeometryId,bool climatologicalMappings,bool generationValid);
     bool           getProducerGridValues(GridValueRequest& r,const std::string& producerName,T::GeometryId producerGeometryId,const T::ProducerInfo& producerInfo);
     bool           getMappingGridValues(GridValueRequest& r,const std::string& producerName,T::GeometryId producerGeometryId,const T::ProducerInfo& producerInfo,const T::GenerationInfo& generationInfo,UInt64 gflags,ParameterMapping& pInfo);
     bool           getTypeGridValues(GridValueRequest& r,T::GeometryId producerGeometryId,const T::ProducerInfo& producerInfo,const T::GenerationInfo& generationInfo,UInt64 gflags,ParameterMapping& pInfo,T::ParamLevelId pLevelId,T::ParamLevel pLevel);
     bool           getPointTypeGridValues(GridValueRequest& r,const std::string& producerName,T::GeometryId producerGeometryId,const T::ProducerInfo& producerInfo,const T::GenerationInfo& generationInfo,UInt64 gflags,ParameterMapping& pInfo,short areaInterpolation,T::ParamLevelId pLevelId,T::ParamLevel pLevel);
     bool           getInterpolatedPointValues(GridValueRequest& r,T::GeometryId producerGeometryId,const T::ProducerInfo& producerInfo,const T::GenerationInfo& generationInfo,UInt64 gflags,ParameterMapping& pInfo,short areaInterpolation,T::ParamLevelId pLevelId,T::ParamLevel pLevel);
     void           getLandscapeParameterValues(GridValueRequest& r,const std::string& name,const std::string& producerName,T::GeometryId producerGeometryId,Producer_vec& pList,std::set<T::GeometryId>& gList,ParameterValues& values);
     void           setLandscapeValue(GridValueRequest& r,const std::string& producerName,T::GeometryId producerGeometryId,double x,double y);
     void           printTimeRangeValueRequest(const TimeRangeValueRequest& r,const Producer_vec& producers,const std::set<T::GeometryId>& geometryIdList);
     ParameterMapping_vec_sptr getTimeRangeMappings(const T::ProducerInfo& producerInfo,const std::string& parameterKey,std::size_t parameterHash,T::GeometryId producerGeometryId,const QueryParameter& qParam);
     T::GenerationInfo* getTimeRangeContentList(const TimeRangeValueRequest& r,const T::ProducerInfo& producerInfo,const CacheEntry& cacheEntry,const ParameterMapping& pInfo,T::GeometryId producerGeometryId,T::ContentInfoList& contentInfoList);
     void           collectTimeRangeContentTimes(const TimeRangeValueRequest& r,const CacheEntry& cacheEntry,uint gLen,T::GenerationInfo* generationInfo,T::ContentInfoList& contentInfoList,const T::ContentInfo& cFirst,std::map<time_t,std::string>& contentTimeList);
     void           getTimeRangeDataTimes(const TimeRangeValueRequest& r,std::map<time_t,std::string>& contentTimeList,time_t& startTime_tt,time_t& endTime_tt);
     time_t         fetchTimeRangeTimeValues(TimeRangeValueRequest& r,Producer_vec& producers2,std::set<T::GeometryId>& geometryIdList2,const T::ProducerInfo& producerInfo,const std::map<time_t,std::string>& contentTimeList2,const std::set<time_t>& originalTimes);
     void           fetchTimeRangeContentValues(TimeRangeValueRequest& r,const std::pair<std::string,T::GeometryId>& producer,const T::ProducerInfo& producerInfo,std::map<time_t,std::string>& contentTimeList);
     bool           getMappingTimeRangeValues(TimeRangeValueRequest& r,const std::pair<std::string,T::GeometryId>& producer,const T::ProducerInfo& producerInfo,const CacheEntry& cacheEntry,uint gLen,const ParameterMapping& pInfo);
     bool           getProducerTimeRangeValues(TimeRangeValueRequest& r,const std::pair<std::string,T::GeometryId>& producer,const T::ProducerInfo& producerInfo);
     bool           getValueConversion(const ParameterMapping& pInfo,std::string& function,std::vector<std::string>& functionParams,uint& modificationOperation,double_vec& modificationParameters);
     void           reportDataServerError(const char *service,int result,const std::vector<const T::ContentInfo*>& contents);
     void           reportDataServerError(const char *service,uchar locationType,int result,const std::vector<const T::ContentInfo*>& contents);

     /*! \brief Fetch the contours with the data server call (unless the query parameter requests no values) and log the errors. */

     template <class Fetch>
     void           fetchContours(const QueryParameter& qParam,Fetch fetch,const char *service,const std::vector<const T::ContentInfo*>& contents)
     {
       if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) != 0)
         return;

       int result = fetch();
       if (result != 0)
         reportDataServerError(service,qParam.mLocationType,result,contents);
     }

     bool           checkValueResult(int result,const char *service,const std::vector<const T::ContentInfo*>& contents,bool errorStops,bool conversionByFunction,const std::string& function,const std::vector<std::string>& functionParams,time_t forecastTime,const T::GridValueList& valueList);

     /*! \brief Fetch the values with the data server call (unless the query parameter requests no values) and check the result with checkValueResult. */

     template <class Fetch>
     bool           fetchValues(const QueryParameter& qParam,Fetch fetch,const char *service,const std::vector<const T::ContentInfo*>& contents,bool errorStops,bool conversionByFunction,const std::string& function,const std::vector<std::string>& functionParams,time_t forecastTime,const T::GridValueList& valueList)
     {
       if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) != 0)
         return true;

       return checkValueResult(fetch(),service,contents,errorStops,conversionByFunction,function,functionParams,forecastTime,valueList);
     }

     bool           getMetricLevelValueVectors(Query& query,QueryParameter& qParam,const T::ProducerInfo& producerInfo,T::GeometryId producerGeometryId,T::GenerationId generationId,const std::string& analysisTime,UInt64 generationFlags,ParameterMapping& pInfo,time_t forecastTime,T::ParamLevelId paramLevelId,T::ParamLevel paramLevel,T::ProducerId& newProducerId,ParameterValues& valueList,T::Coordinate_vec& coordinates);
     void           getValueVectorCoordinates(Query& query,const QueryParameter& qParam,const T::ContentInfo& contentInfo1,bool conversionByFunction,T::Coordinate_vec& coordinates);
     bool           checkValueVectorResult(int result,Query& query,const QueryParameter& qParam,const char *service,const T::ContentInfo& contentInfo1,bool errorStops,bool conversionByFunction,const std::string& function,std::vector<std::string>& functionParams,time_t forecastTime,T::Coordinate_vec& coordinates,T::ParamValue_vec& valueVector,ParameterValues& valueList);

     /*! \brief Fetch the value vector with the data server call (unless the query parameter requests no values) and check the result with checkValueVectorResult. */

     template <class Fetch>
     bool           fetchValueVector(Query& query,const QueryParameter& qParam,Fetch fetch,const char *service,const T::ContentInfo& contentInfo1,bool errorStops,bool conversionByFunction,const std::string& function,std::vector<std::string>& functionParams,time_t forecastTime,T::Coordinate_vec& coordinates,T::ParamValue_vec& valueVector,ParameterValues& valueList)
     {
       if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) != 0)
         return true;

       return checkValueVectorResult(fetch(),query,qParam,service,contentInfo1,errorStops,conversionByFunction,function,functionParams,forecastTime,coordinates,valueVector,valueList);
     }

     bool           getModificationOperation(const std::string& function,const std::vector<std::string>& functionParams,uint& modificationOperation,double_vec& modificationParameters);
     bool           getPointValueVector(const T::ContentInfo& contentInfo,uchar coordinateType,double x,double y,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,double_vec& valueVector);
     void           addSpecialValue(const double_vec& valueVector,double x,double y,bool conversionByFunction,const std::string& function,const std::vector<std::string>& functionParams,time_t forecastTime,ParameterValues& valueList);
     void           addInterpolatedSpecialValue(const double_vec& valueVector1,const double_vec& valueVector2,const T::ContentInfo& contentInfo1,const T::ContentInfo& contentInfo2,time_t fTime,short timeInterpolationMethod,double x,double y,bool conversionByFunction,const std::string& function,const std::vector<std::string>& functionParams,time_t forecastTime,ParameterValues& valueList);
     ContentCacheEntry_sptr findContentCacheEntry(std::size_t& hash,std::size_t hash3,bool& newEntry);
     void           updateContentCacheEntry(ContentCacheEntry& entry,bool newEntry,T::SessionId sessionId,UInt64 producerHash,T::GenerationId generationId,T::ParamKeyType parameterKeyType,const std::string& parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::size_t& hash,std::size_t hash3);
     void           searchContentList(T::ContentInfoList& contentList,T::ProducerId producerId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,const std::string& parameterKey,uint fmiId,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,T::ContentInfoList& cList);
     void           storeContentSearchResult(std::size_t hash2,const std::shared_ptr<T::ContentInfoList>& cList,UInt64 producerHash,T::GenerationId generationId);
     void           storeContentCacheEntry(std::size_t hash,const ContentCacheEntry_sptr& entry);
     void           updateRequestStatistics(time_t& prevTime,Int64& prevCounter);
     void           setConversionParameters(const std::vector<std::string>& functionParams,T::ParamValue value,time_t forecastTime,const T::Coordinate& coordinate,std::vector<double>& parameters);
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
                       const std::vector<std::string>& functionParams,
                       time_t forecastTime,
                       const T::GridValueList& valueList);

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

     bool           getPolygonValuesByHeight(
                       Query& query,
                       QueryParameter& qParam,
                       const T::ProducerInfo& producerInfo,
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

     bool           getPolygonValues(
                       Query& query,
                       QueryParameter& qParam,
                       const T::ProducerInfo& producerInfo,
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
                       const Query& query,
                       QueryParameter& qParam,
                       const T::ProducerInfo& producerInfo,
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
                       const Query& query,
                       T::CoordinateType coordinateType,
                       T::Coordinate_vec& coordinates,
                       QueryParameter& qParam,
                       const T::ProducerInfo& producerInfo,
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
                       const T::ProducerInfo& producerInfo,
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
                       const Query& query,
                       QueryParameter& qParam,
                       const T::ProducerInfo& producerInfo,
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
                       const T::ProducerInfo& producerInfo,
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
                       const T::ProducerInfo& producerInfo,
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
                       const T::ProducerInfo& producerInfo,
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
                       const Query& query,
                       T::CoordinateType coordinateType,
                       T::Coordinate_vec& coordinates,
                       QueryParameter& qParam,
                       const T::ProducerInfo& producerInfo,
                       T::GeometryId producerGeometryId,
                       T::GenerationId generationId,
                       const std::string& analysisTime,
                       UInt64 generationFlags,
                       ParameterMapping& pInfo,
                       time_t forecastTime,
                       T::ParamLevelId paramLevelId,
                       T::ParamLevel paramLevel,
                       const T::ProducerId& newProducerId,
                       ParameterValues& valueList);

     bool           getValueVectors(
                       Query& query,
                       QueryParameter& qParam,
                       const T::ProducerInfo& producerInfo,
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
                       const T::ProducerInfo& producerInfo,
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
                       const T::ProducerInfo& producerInfo,
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
                       const T::ContentInfoList& contentInfoList,
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
                       const T::ContentInfoList& contentInfoList,
                       uchar coordinateType,
                       double x,
                       double y,
                       int height,
                       int& level1,
                       float& height1,
                       int& level2,
                       float& height2);

     bool           findHeightConversion(std::size_t hash,double& mul,T::ProducerId& heightProducerId,std::string& heightParameter);
     bool           getHeightGenerationId(T::ProducerId producerId,T::GenerationId generationId,HeightQuery& q);
     bool           getHeightLevelRange(HeightQuery& q);
     void           getHeightContentList(const HeightQuery& q,time_t forecastTime,uint requestFlags,T::ContentInfoList& contentInfoList);
     ContentCacheEntry_sptr getHeightContentEntry(std::size_t levelContentHash,const HeightQuery& q,time_t forecastTime);
     bool           getHeightTimeNeighbours(const HeightQuery& q,time_t forecastTime,time_t& time1,time_t& time2);
     bool           getHeightContentListPair(const HeightQuery& q,time_t time1,time_t time2,T::ContentInfoList& contentInfoList1,T::ContentInfoList& contentInfoList2);
     bool           getHeightValueVector(const T::ContentInfo& info,uchar coordinateType,std::vector<T::Coordinate>& coordinates,std::size_t coordinateHash,std::shared_ptr<T::ParamValue_vec>& valueVec);

     T::ParamValue  getAdditionalValue(
                       const std::string& parameterName,
                       time_t forecastTime,
                       double x,
                       double y);

     void           getAdditionalValues(
                       const std::string& parameterName,
                       uchar coordinateType,
                       const T::Coordinate_vec& coordinates,
                       ParameterValues& values);

     bool           getProducerInfoByName(const std::string& name,T::ProducerInfo& info);

     time_t         getContentList(
                      const T::ProducerInfo& producerInfo,
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
     std::atomic<UInt64>        mContentCache_globalGeneration{0};        //!< Incremented to clear the per-thread content caches.
     std::atomic<UInt64>        mContentSearchCache_globalGeneration{0};  //!< Incremented to clear the per-thread content search caches.
     std::size_t                mContentSearchCache_maxRecords;
     std::size_t                mContentSearchCache_maxRecordsPerThread;
     uint                       mContentSearchCache_clearInterval;
     std::atomic<UInt64>        mParameterMappingCache_generation{0};  //!< Incremented when the mapping files change: clears the per-thread mapping caches.

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
     std::atomic<UInt64>        mProducerGenerationListCache_generation{0};  //!< Incremented to clear the per-thread producer generation list caches.

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

