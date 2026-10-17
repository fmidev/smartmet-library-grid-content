#include "ServiceImplementation.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/InterpolationFunctions.h>
#include <grid-files/common/ShowFunction.h>
#include <grid-files/common/MemoryWriter.h>
#include <grid-files/common/FileWriter.h>
#include <grid-files/identification/GridDef.h>
#include <grid-files/grid/GridFile.h>
#include <grid-files/common/GraphFunctions.h>
#include <macgyver/StringConversion.h>
#include <macgyver/ThreadName.h>
#include <macgyver/TimeParser.h>
#include <macgyver/Astronomy.h>
#include <macgyver/CharsetTools.h>
#include <macgyver/NumericCast.h>
#include <macgyver/Hash.h>
#include <fmt/format.h>
#include <cmath>
#include <algorithm>
#include <boost/numeric/conversion/cast.hpp>
#include <chrono>
#include <deque>
#include <memory>
#include <thread>
#include <unordered_set>
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
#include "../../functions/Function_windU.h"
#include "../../functions/Function_windV.h"
#include "../../functions/Function_streamU.h"
#include "../../functions/Function_streamV.h"
#include "../../functions/Function_streamDir.h"
#include "../../functions/Function_feelsLike.h"
#include "../../functions/Function_replace.h"
#include <cstdlib>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF

namespace SmartMet
{
namespace QueryServer
{

namespace
{
const Fmi::TimeZonePtr& tz_utc = (Fmi::TimeZonePtr::utc);



/*! \brief Split the line into ';'-separated fields (separators within quotes are ignored). Returns the number of fields. */

uint splitFields(char* st,char** field)
{
  bool ind = false;
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
  return c;
}



/*! \brief Append the values of the grid value list (missing values for missing records). */

template <class V>
void appendGridValues(T::GridValueList& valList,V& valueVector)
{
  uint len = valList.getLength();
  for (uint t=0; t<len; t++)
  {
    T::GridValue *rec = valList.getGridValuePtrByIndex(t);
    if (rec)
      valueVector.push_back(rec->mValue);
    else
      valueVector.push_back(ParamValueMissing);
  }
}



/*! \brief Call the data server service of the location type: by geometry, by grid (with the grid width and height,
           or no call if they are missing) or by the grid of the content. Returns the result of the call (0 without a call). */

template <class ByGeometry,class ByGrid,class ByDefault>
int fetchByLocationType(uchar locationType,const char *gridWidthStr,const char *gridHeightStr,ByGeometry byGeometry,ByGrid byGrid,ByDefault byDefault)
{
  switch (locationType)
  {
    case QueryParameter::LocationType::Geometry:
      return byGeometry();

    case QueryParameter::LocationType::Grid:
      if (gridWidthStr != nullptr &&  gridHeightStr != nullptr)
        return byGrid(toUInt32(gridWidthStr),toUInt32(gridHeightStr));
      return 0;

    default:
      return byDefault();
  }
}



/*! \brief Return the polygon path in the grid coordinates of the grid definition. The path is in lat/lon or (with original) in the original coordinates. */

T::AreaCoordinates toGridPolygonPath(GRIB2::GridDefinition& def,const T::AreaCoordinates& polygonPath,bool original)
{
  T::AreaCoordinates newPolygonPath;
  for (const auto& mAreaCoordinate : polygonPath)
  {
    std::vector<T::Coordinate> newPolygonPoints;
    for (auto it = mAreaCoordinate.begin(); it != mAreaCoordinate.end(); ++it)
    {
      double grid_i = 0;
      double grid_j = 0;
      if (original)
        def.getGridPointByOriginalCoordinates(it->x(),it->y(),grid_i,grid_j);
      else
        def.getGridPointByLatLonCoordinates(it->y(),it->x(),grid_i,grid_j);
      newPolygonPoints.emplace_back(grid_i,grid_j);
    }
    newPolygonPath.emplace_back(newPolygonPoints);
  }
  return newPolygonPath;
}



/*! \brief Return the lat/lon coordinates of the grid points inside the polygon path given in the grid coordinates. */

T::Coordinate_vec getGridPointCoordinatesInside(GRIB2::GridDefinition& def,uint cols,uint rows,T::AreaCoordinates& gridPolygonPath)
{
  std::vector<T::Point> gridPoints;
  getPointsInsidePolygonPath(cols,rows,gridPolygonPath,gridPoints);

  T::Coordinate_vec coordinates;
  for (auto& gridPoint : gridPoints)
  {
    double lat = 0;
    double lon = 0;
    if (def.getGridLatLonCoordinatesByGridPoint(gridPoint.x(),gridPoint.y(),lat,lon))
    {
      coordinates.emplace_back(lon,lat);
    }
  }
  return coordinates;
}



/*! \brief Return true if the levels of the query parameter are heights and level interpolation is allowed. */

bool isMetricLevelQuery(const QueryParameter& qParam)
{
  return (qParam.mFlags & QueryServer::QueryParameter::Flags::MetricLevels)  &&  qParam.mLevelInterpolationMethod != T::LevelInterpolationMethod::Forbidden;
}



/*! \brief Use the producer, but indicate that it has no values for the forecast time. */

void setProducerWithoutValues(const T::ProducerInfo& producerInfo,const ParameterMapping& pInfo,time_t forecastTime,T::ProducerId& newProducerId,ParameterValues& valueList)
{
  newProducerId = producerInfo.mProducerId;
  valueList.clear();
  valueList.mProducerId = producerInfo.mProducerId;
  valueList.mParameterKeyType = pInfo.mParameterKeyType;
  valueList.mParameterKey = pInfo.mParameterKey;
  valueList.mForecastTimeUTC = forecastTime;
}



/*! \brief Return the area interpolation method of the query parameter, or the default one of the parameter mapping. */

short getAreaInterpolationMethod(const QueryParameter& qParam,const ParameterMapping& pInfo)
{
  short areaInterpolationMethod = qParam.mAreaInterpolationMethod;
  if (areaInterpolationMethod == T::AreaInterpolationMethod::Undefined)
    areaInterpolationMethod = pInfo.mAreaInterpolationMethod;
  return areaInterpolationMethod;
}



/*! \brief Return the time interpolation method of the query parameter, or the default one of the parameter mapping. */

short getTimeInterpolationMethod(const QueryParameter& qParam,const ParameterMapping& pInfo)
{
  short timeInterpolationMethod = qParam.mTimeInterpolationMethod;
  if (timeInterpolationMethod == T::TimeInterpolationMethod::Undefined)
    timeInterpolationMethod = pInfo.mTimeInterpolationMethod;
  return timeInterpolationMethod;
}



/*! \brief Return true if the contents are of the forecast time and on both sides of the level, and level interpolation is allowed. */

bool isLevelInterpolationPair(const T::ContentInfo& contentInfo1,const T::ContentInfo& contentInfo2,time_t fTime,T::ParamLevel paramLevel,short levelInterpolationMethod)
{
  return contentInfo1.mForecastTimeUTC == fTime  &&  contentInfo2.mForecastTimeUTC == fTime  &&
         contentInfo1.mParameterLevel < paramLevel  &&  contentInfo2.mParameterLevel > paramLevel  &&
         levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden;
}



/*! \brief Return true if the contents are on both sides of the forecast time, and time interpolation is allowed. */

bool isTimeInterpolationPair(const T::ContentInfo& contentInfo1,const T::ContentInfo& contentInfo2,time_t fTime,short timeInterpolationMethod)
{
  return contentInfo1.mForecastTimeUTC < fTime  &&  contentInfo2.mForecastTimeUTC > fTime  &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden;
}



/*! \brief Return true if the contents are the level pairs of both sides of the forecast time, and time and level interpolations are allowed. */

bool isTimeLevelInterpolationQuad(const T::ContentInfo& contentInfo1,const T::ContentInfo& contentInfo2,const T::ContentInfo& contentInfo3,const T::ContentInfo& contentInfo4,time_t fTime,T::ParamLevel paramLevel,short timeInterpolationMethod,short levelInterpolationMethod)
{
  return contentInfo1.mForecastTimeUTC < fTime  &&  contentInfo2.mForecastTimeUTC < fTime  &&  contentInfo3.mForecastTimeUTC > fTime  &&  contentInfo4.mForecastTimeUTC > fTime  &&
         contentInfo1.mParameterLevel < paramLevel  &&  contentInfo2.mParameterLevel > paramLevel  &&  contentInfo3.mParameterLevel < paramLevel  &&  contentInfo4.mParameterLevel > paramLevel  &&
         timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden;
}



/*! \brief Use the default precision of the parameter mapping if the query parameter has no precision. */

void setDefaultPrecision(QueryParameter& qParam,const ParameterMapping& pInfo)
{
  if (qParam.mPrecision < 0)
    qParam.mPrecision = pInfo.mDefaultPrecision;
}



/*! \brief Throw an exception if the forecast time is not between the times of the contents. */

void checkInterpolationTimes(const T::ContentInfo& contentInfo1,const T::ContentInfo& contentInfo2,time_t fTime)
{
  if (contentInfo1.mForecastTimeUTC >= fTime || contentInfo2.mForecastTimeUTC <= fTime)
  {
    Fmi::Exception exception(BCP, "Unexpected result!");
    exception.addDetail("The given forecast time should been between the found content times.");
    exception.addParameter("Content 1 ForecastTime", contentInfo1.getForecastTime());
    exception.addParameter("Content 2 ForecastTime", contentInfo2.getForecastTime());
    exception.addParameter("Request ForecastTime", utcTimeFromTimeT(fTime));
    throw exception;
  }
}



/*! \brief Set the content information of the value list. */

void setValueListContentInfo(ParameterValues& valueList,const ParameterMapping& pInfo,T::ParamLevelId paramLevelId,time_t forecastTime,const std::string& analysisTime,const T::ContentInfo& contentInfo1)
{
  valueList.mParameterKeyType = pInfo.mParameterKeyType;
  valueList.mParameterKey = pInfo.mParameterKey;
  valueList.mParameterLevelId = paramLevelId;
  //valueList.mForecastTime = utcTimeFromTimeT(forecastTime);
  valueList.mForecastTimeUTC = forecastTime;
  valueList.mProducerId = contentInfo1.mProducerId;
  valueList.mGenerationId = contentInfo1.mGenerationId;
  valueList.mGeometryId = contentInfo1.mGeometryId;
  valueList.mFileId[0] = contentInfo1.mFileId;
  valueList.mMessageIndex[0] = contentInfo1.mMessageIndex;
  valueList.mModificationTime = contentInfo1.mModificationTime;
  valueList.mAnalysisTime = analysisTime;
  valueList.mForecastType = contentInfo1.mForecastType;
  valueList.mForecastNumber = contentInfo1.mForecastNumber;
  valueList.mParameterLevel = contentInfo1.mParameterLevel;
  valueList.mParameterLevelId = contentInfo1.mFmiParameterLevelId;
}



/*! \brief Set the content information of the value list of the level (if there is a content). */

void setLevelValueListContentInfo(ParameterValues& valueList,const ParameterMapping& pInfo,T::ParamLevelId paramLevelId,time_t forecastTime,const std::string& analysisTime,UInt64 generationFlags,T::ParamLevel parameterLevel,const T::ContentInfo *contentInfo1)
{
  if (contentInfo1 == nullptr)
    return;

  setValueListContentInfo(valueList,pInfo,paramLevelId,forecastTime,analysisTime,*contentInfo1);
  valueList.mGenerationFlags = generationFlags;
  valueList.mParameterLevel = parameterLevel;
}



/*! \brief Collect the levels of the height records (the two given level fields) and replace the levels in the records
           with their indexes in the ordered level set. Returns the level set. */

std::set<T::ParamLevel> indexHeightLevels(HeightRec_vec& recs,T::ParamLevel HeightRec::*levelA,T::ParamLevel HeightRec::*levelB)
{
  std::set<T::ParamLevel> levels;
  for (auto& rec : recs)
  {
    levels.insert(rec.*levelA);
    levels.insert(rec.*levelB);
  }

  // The indexes are looked up from the original levels. Replacing the levels in place level by level would
  // replace an already written index again if it equals a later level (possible with negative levels).

  std::map<T::ParamLevel,T::ParamLevel> indexes;
  T::ParamLevel idx = 0;
  for (int lev : levels)
  {
    indexes[lev] = idx;
    idx++;
  }

  for (auto& rec : recs)
  {
    rec.*levelA = indexes[rec.*levelA];
    rec.*levelB = indexes[rec.*levelB];
  }
  return levels;
}



/*! \brief Split the function name of the form "function:subfunction" (the name is the function otherwise). */

void splitSubfunction(const std::string& name,std::string& function,std::string& subfunction)
{
  function = name;
  std::vector<std::string> subVec;
  splitString(function,':',subVec);
  if (subVec.size() == 2)
  {
    function = subVec[0];
    subfunction = subVec[1];
  }
}



/*! \brief Get the row range (the second and the third function parameter) and the constant parameters (from the fifth
           function parameter on) of the row range function of the query parameter. */

template <class P>
void getRowRangeFunctionParameters(const QueryParameter& qParam,int& startIdx,int& endIdx,P& parameters)
{
  int fpLen = qParam.mFunctionParams.size();

  if (fpLen > 1)
    startIdx = toInt32(qParam.mFunctionParams[1].second);

  if (fpLen > 2)
    endIdx = toInt32(qParam.mFunctionParams[2].second);

  for (int t=4; t<fpLen; t++)
    parameters.emplace_back(toDouble(qParam.mFunctionParams[t].second));
}



/*! \brief Source information of the function values: taken from the first function parameter with a producer. */

struct FunctionSource
{
  T::ProducerId producerId = 0;
  T::GenerationId generationId = 0;
  uint geometryId = 0;
  uint flags = 0;
};



/*! \brief Collect the value vectors of the function parameters for the time step (the constant parameters into the
           parameter list) and the source information. Coordinates are copied from the first parameter that has them. */

void collectFunctionInputVectors(Query& query,QueryParameter& qParam,int tCount,std::vector<T::ParamValue_vec>& inputVectors,std::vector<double>& parameters,FunctionSource& source)
{
  for (auto it = qParam.mFunctionParams.begin(); it != qParam.mFunctionParams.end(); ++it)
  {
    double a = toDouble(it->second.c_str());
    QueryParameter* q = query.getQueryParameterPtr(it->first);
    if (q == nullptr)
    {
      parameters.emplace_back(a);
      continue;
    }

    if ((std::size_t)tCount >= q->mValueList.size())
      continue;

    if (source.producerId == 0 && q->mValueList[tCount]->mProducerId > 0)
    {
      source.producerId = q->mValueList[tCount]->mProducerId;
      source.generationId = q->mValueList[tCount]->mGenerationId;
      source.geometryId = q->mValueList[tCount]->mGeometryId;

      if (q->mValueList[tCount]->mFlags & ParameterValues::Flags::AggregationValue)
        source.flags |= ParameterValues::Flags::AggregationValue;

      if (q->mValueList[tCount]->mFlags & ParameterValues::Flags::InternalAggregationValue)
        source.flags |= ParameterValues::Flags::InternalAggregationValue;
    }

    inputVectors.emplace_back(q->mValueList[tCount]->mValueVector);
    if (!q->mCoordinates.empty()  &&  qParam.mCoordinates.empty())
      qParam.mCoordinates = q->mCoordinates;
  }
}



/*! \brief Throw an exception if the maximum processing time of the query has been reached. */

void checkMaxProcessingTime(const Query& query,const char *parameterName)
{
  if ((query.mProcessingStartTime + query.mMaxProcessingTimeInSeconds) < time(nullptr))
  {
    Fmi::Exception exception(BCP, "Maximum processing time reached - processing halted!");
    exception.addParameter(parameterName, std::to_string(query.mMaxProcessingTimeInSeconds));
    throw exception;
  }
}



/*! \brief Throw an exception if the query has no area (coordinates, geometry, grid or crs) for the parameter, or if a
           polygon has less than 3 coordinates. */

void checkGridValueArea(const Query& query,const QueryParameter& qParam,const char *polygonError)
{
  if (qParam.mType != QueryParameter::Type::GridFile  &&  query.mAreaCoordinates.empty()  &&
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
    for (const auto& mAreaCoordinate : query.mAreaCoordinates)
    {
      if (mAreaCoordinate.size() < 3)
      {
        Fmi::Exception exception(BCP, polygonError);
        throw exception;
      }
    }
  }
}



/*! \brief Return the special value list (the values of the value string) of the single value, or the missing list. */

std::vector<double> getSpecialValueList(ParameterValues& values,const std::vector<double>& missingList)
{
  std::vector<double> list;
  if (values.mValueList.getLength() == 1)
  {
    T::GridValue *v = values.mValueList.getGridValuePtrByIndex(0);
    if (v != nullptr)
      splitString(v->mValueString,';',list);
  }
  if (list.empty())
    list = missingList;

  return list;
}



/*! \brief Return true if the parameter is a data parameter (not a function) whose values must be fetched: not an
           alternative parameter, or an alternative parameter that is required. */

bool isRequiredDataParameter(const QueryParameter& qParam,const std::unordered_set<uint>& alternativeRequired)
{
  return qParam.mFunction.empty() && ((qParam.mFlags & QueryParameter::Flags::AlternativeParameter) == 0 || alternativeRequired.find(qParam.mId) != alternativeRequired.end());
}



/*! \brief Use the analysis time of the values with all parameters if the query requires the same analysis time. */

void updateSameAnalysisTime(const Query& query,const ParameterValues& values,QueryFetchState& state)
{
  if ((query.mFlags & Query::Flags::SameAnalysisTime) != 0  &&  state.analysisTime != values.mAnalysisTime)
  {
    // The query requires that we use the same analysis time with all parameters
    state.analysisTime = values.mAnalysisTime;
    state.queryFlags = state.queryFlags | Query::Flags::AnalysisTimeMatchRequired;
  }
}



/*! \brief Take the coordinates of the values if there are no coordinates yet. */

void addValueCoordinates(const ParameterValues& values,T::Coordinate_vec& coordinates)
{
  uint len = values.mValueList.getLength();
  if (coordinates.empty()  &&  len > 0)
  {
    for (uint s = 0; s<len; s++)
    {
      T::GridValue *val = values.mValueList.getGridValuePtrByIndex(s);
      coordinates.emplace_back(val->mX,val->mY);
    }
  }
}



/*! \brief Mark the values as aggregation values (and internal aggregation values if the query contains them). */

void markAggregationValue(ParameterValues& values,bool containsInternalAggregation)
{
  values.mFlags = values.mFlags | QueryServer::ParameterValues::Flags::AggregationValue;

  if (containsInternalAggregation)
    values.mFlags = values.mFlags | QueryServer::ParameterValues::Flags::InternalAggregationValue;
}



/*! \brief Return true if the forecast time is outside the time range of the query (and the start time does not come
           from the data). */

bool isOutsideQueryTimeRange(const Query& query,time_t forecastTime)
{
  return (query.mFlags & Query::Flags::StartTimeFromData) == 0
      && ((forecastTime < query.mStartTime || forecastTime > query.mEndTime) ||
          (forecastTime == query.mStartTime &&  (query.mFlags & Query::Flags::StartTimeNotIncluded) != 0));
}



/*! \brief Collect the forecast times of the parameter values: all times, and the original times (not temporary or
           internal aggregation values). */

void collectTimeRangeForecastTimes(const Query& query,std::set<time_t>& originalTimeList,std::set<time_t>& timeList)
{
  for (const auto& qParam : query.mQueryParameterList)
  {
    for (auto it = qParam.mValueList.begin(); it != qParam.mValueList.end(); ++it)
    {
      if (!qParam.mTemporary  &&  ((*it)->mFlags & ParameterValues::Flags::InternalAggregationValue) == 0)
      {
        originalTimeList.insert((*it)->mForecastTimeUTC);
        // std::cout << "INSERT " << qParam->mParam  << " " << utcTimeFromTimeT((*it)->mForecastTimeUTC) << "\n";
      }

      timeList.insert((*it)->mForecastTimeUTC);
    }
  }
}



/*! \brief Return true if the values are not found (no values and no data available). */

bool hasNoValues(const ParameterValues& valueList)
{
  return valueList.mValueList.getLength() == 0 && (valueList.mFlags & ParameterValues::Flags::DataAvailable) == 0;
}



/*! \brief Select the geometries of the concatenated producers: the geometry of the producer string for the area
           location types (returns false if it is not a geometry of the producers), all geometries otherwise. */

bool selectConcatGeometries(const QueryParameter& qParam,const Producer_vec& producers,int gid,std::set<T::GeometryId>& geomIdList)
{
  if (qParam.mLocationType == QueryParameter::LocationType::Polygon || qParam.mLocationType == QueryParameter::LocationType::Circle || qParam.mLocationType == QueryParameter::LocationType::Path)
  {
    for (auto gg = producers.begin(); gg != producers.end(); ++gg)
    {
      if (gid == gg->second)
      {
        geomIdList.insert(gid);
        return true;
      }
    }
    return false;
  }

  for (auto gg = producers.begin(); gg != producers.end(); ++gg)
    geomIdList.insert(gg->second);

  return true;
}



/*! \brief Select the generations for the next time steps by the generation of the values found. */

void updateTimeStepGenerationFlags(const Query& query,const QueryParameter& qParam,UInt64 generationFlags,const ParameterValues& valueList,UInt64& gflags)
{
  if (qParam.mTimeInterpolationMethod == T::TimeInterpolationMethod::Forbidden || (qParam.mFlags & QueryParameter::Flags::SameAnalysisTime) != 0 || (query.mFlags & Query::Flags::SameAnalysisTime) != 0 )
  {
    // If the time interpolation is forbidden or same analysisTime is required, we should no change the generation.
    gflags = valueList.mGenerationFlags;
  }
  else
  {
    // We should use this generation or older.
    if (generationFlags == 0)
      gflags = valueList.mGenerationFlags + (valueList.mGenerationFlags << 1);
  }
}



/*! \brief Add the time steps before and after the query forecast times needed by the parameter into the forecast time
           list (a copy of the query forecast times). New times are added to the aggregation times of the query. */

void addAggregationTimeSteps(Query& query,const QueryParameter& qParam,std::set<time_t>& forecastTimeList)
{
  forecastTimeList = query.mForecastTimeList;
  for (const auto &fTime : query.mForecastTimeList)
  {
    if (qParam.mTimestepsBefore > 0)
    {
      time_t ss = fTime - ((qParam.mTimestepsBefore + 1) * qParam.mTimestepSizeInMinutes * 60);

      for (uint t = 0; t < qParam.mTimestepsBefore; t++)
      {
        ss = ss + (qParam.mTimestepSizeInMinutes * 60);
        forecastTimeList.insert(ss);
        if (query.mForecastTimeList.find(ss) == query.mForecastTimeList.end())
          query.mAggregationTimes.insert(ss);
      }
    }

    if (qParam.mTimestepsAfter > 0)
    {
      time_t ss = fTime;

      for (uint t = 0; t < qParam.mTimestepsAfter; t++)
      {
        ss = ss + (qParam.mTimestepSizeInMinutes * 60);
        forecastTimeList.insert(ss);
        if (query.mForecastTimeList.find(ss) == query.mForecastTimeList.end())
          query.mAggregationTimes.insert(ss);
      }
    }
  }
}



/*! \brief Collect the forecast times of the parameter values. The values whose times are not query forecast times are
           marked as aggregation values and their times collected as additional times. */

void collectTimeStepForecastTimes(Query& query,bool containsInternalAggregation,std::set<time_t>& timeList,std::unordered_set<time_t>& additionalTimeList)
{
  for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
  {
    for (auto& it : qParam->mValueList)
    {
      timeList.insert(it->mForecastTimeUTC);

      if (query.mForecastTimeList.find(it->mForecastTimeUTC) == query.mForecastTimeList.end())
      {
        markAggregationValue(*it,containsInternalAggregation);
        additionalTimeList.insert(it->mForecastTimeUTC);
      }
    }
  }
}



/*! \brief Size, bounding box and grid steps of the area of the grid file. */

struct GridFileArea
{
  Int64  gridWidth = 0;
  Int64  gridHeight = 0;

  double lon1 = 0;
  double lat1 = 0;
  double lon2 = 0;
  double lat2 = 0;
  double xStepMetric = 0;
  double yStepMetric = 0;
  double xStepDegrees = 0;
  double yStepDegrees = 0;
};



/*! \brief Projection parameters of the crs of the grid file (missing values if not defined). */

struct ProjectionParameters
{
  std::string projectionStr;
  //float lat_0 = ParamValueMissing;
  float lat_1 = ParamValueMissing;
  float lat_2 = ParamValueMissing;
  float laD = ParamValueMissing;
  float lon_0 = ParamValueMissing;
  //float x_0 = ParamValueMissing;
  //float y_0 = ParamValueMissing;
};



/*! \brief Return the GRIB version of the grid properties (0 if not found). */

uint getGribVersion(const std::map<uint,Int64 >& oldProperties)
{
  uint version = 0;
  const int id = GRIB1::Property::IndicatorSection::EditionNumber;
  auto v1 = oldProperties.find(id);
  if (v1 != oldProperties.end())
    version = 1;

  if (version == 0)
  {
    const int id = GRIB2::Property::IndicatorSection::EditionNumber;
    auto v2 = oldProperties.find(id);
    if (v2 != oldProperties.end())
      version = 2;
  }

  return version;
}



/*! \brief Set the message properties: the new properties, and the old properties outside the grid section. */

void setMessageProperties(GRID::Message& newMessage,T::PropertyVec& props,uint firstGridProperty,uint lastGridProperty,const std::map<uint,Int64 >& newProperties,const std::map<uint,Int64 >& oldProperties)
{
  for (auto& prop : props)
  {
    auto np = newProperties.find(prop.second);

    if (np != newProperties.end())
    {
      // printf("REPLACE %s (%u) %lld\n",it->first.c_str(),it->second,np->second);
      newMessage.setProperty(prop.second,np->second);
    }
    else
    {
      if (prop.second < firstGridProperty || prop.second > lastGridProperty)
      {
        auto op = oldProperties.find(prop.second);
        if (op != oldProperties.end())
        {
          // printf("SET %s (%u) %lld\n",it->first.c_str(),it->second,op->second);
          newMessage.setProperty(prop.second,op->second);
        }
      }
    }
  }
}



/*! \brief Get the size, the bounding box and the grid steps of the grid file area from the query attributes. */

void getGridFileArea(const Query& query,const char *gridSizeStr,const char *bboxStr,GridFileArea& a)
{
  if (gridSizeStr)
  {
    std::vector<double> v;
    splitString(gridSizeStr,',',v);
    if (v.size() == 2)
    {
      a.gridWidth = (Int64 )v[0];
      a.gridHeight = (Int64 )v[1];
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
        latLon_bboxByCenter(query.mAreaCoordinates[0][0].x(),query.mAreaCoordinates[0][0].y(),1000*v[0],1000*v[1],1000*v[2],1000*v[3],a.lon1,a.lat1,a.lon2,a.lat2);

        a.xStepMetric = (double)1000*(v[0]+v[2]) / (double)a.gridWidth;
        a.yStepMetric = (double)1000*(v[1]+v[3]) / (double)a.gridHeight;
      }
    }
    else
    if (gridMetricSizeStr)
    {
      splitString(gridMetricSizeStr,',',v);
      if (v.size() == 2)
      {
        latLon_bboxByCenter(query.mAreaCoordinates[0][0].x(),query.mAreaCoordinates[0][0].y(),1000*v[0],1000*v[1],a.lon1,a.lat1,a.lon2,a.lat2);

        a.xStepMetric = (double)1000*(v[0]) / (double)a.gridWidth;
        a.yStepMetric = (double)1000*(v[1]) / (double)a.gridHeight;
      }
    }
  }


  std::vector<double> aa;
  if (bboxStr)
  {
    splitString(bboxStr,',',aa);
    if (aa.size() == 4)
    {
      a.lon1 = aa[0];
      a.lat1 = aa[1];
      a.lon2 = aa[2];
      a.lat2 = aa[3];
    }
  }

  //printf("COORDINATES %f,%f,%f,%f\n",a.lon1,a.lat1,a.lon2,a.lat2);
  double xDegrees = a.lon2-a.lon1;
  double yDegrees = a.lat2-a.lat1;

  a.xStepDegrees = xDegrees / (double)a.gridWidth;
  a.yStepDegrees = yDegrees / (double)a.gridHeight;
}



/*! \brief Parse the projection parameters from the PROJ.4 string of the crs. Throws an exception for an invalid crs. */

void getProjectionParameters(const char *crsStr,ProjectionParameters& pp)
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

  for (auto& it : partList)
  {
    std::vector<std::string> attr;
    splitString(it.c_str(),'=',attr);
    if (attr.size() == 2)
    {
      // printf("[%s][%s]\n",attr[0].c_str(),attr[1].c_str());
      if (attr[0] == "+proj")
        pp.projectionStr = attr[1];

      //if (attr[0] == "+lat_0")
      //  lat_0 = toDouble(attr[1]);

      if (attr[0] == "+lat_1")
        pp.lat_1 = toDouble(attr[1]);

      if (attr[0] == "+lat_2")
        pp.lat_2 = toDouble(attr[1]);

      if (attr[0] == "+lat_ts")
        pp.laD = toDouble(attr[1]);

      if (attr[0] == "+lon_0")
        pp.lon_0 = toDouble(attr[1]);

      //if (attr[0] == "+x_0")
      //  x_0 = toDouble(attr[1]);

      //if (attr[0] == "+y_0")
      //  y_0 = toDouble(attr[1]);
    }
  }
}



/*! \brief Add the GRIB1 latlon grid properties of the grid file area. */

void addGrib1LatLonProperties(const GridFileArea& a,std::map<uint,Int64>& newProperties)
{
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::DataRepresentationType,0LL));

  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::LatLon::Ni,a.gridWidth));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::LatLon::Nj,a.gridHeight));

  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::LatLon::IDirectionIncrement,(Int64 )(a.xStepDegrees*1000)));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::LatLon::JDirectionIncrement,(Int64 )(a.yStepDegrees*1000)));

  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::GridArea::LatitudeOfFirstGridPoint,(Int64 )(a.lat1*1000)));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::GridArea::LongitudeOfFirstGridPoint,(Int64 )(a.lon1*1000)));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::GridArea::LatitudeOfLastGridPoint,(Int64 )(a.lat2*1000)));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::GridArea::LongitudeOfLastGridPoint,(Int64 )(a.lon2*1000)));

  if (a.lat1 < a.lat2)
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::ScanningMode::ScanMode,(Int64 )0x40));
  else
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::ScanningMode::ScanMode,(Int64 )0));
}



/*! \brief Add the GRIB1 polar stereographic grid properties of the grid file area. */

void addGrib1PolarStereographicProperties(const GridFileArea& a,const ProjectionParameters& pp,std::map<uint,Int64>& newProperties)
{
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::DataRepresentationType,5LL));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::PolarStereographic::Nx,a.gridWidth));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::PolarStereographic::Ny,a.gridHeight));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::PolarStereographic::LatitudeOfFirstGridPoint,(Int64 )(a.lat1*1000)));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::PolarStereographic::LongitudeOfFirstGridPoint,(Int64 )(a.lon1*1000)));

  if (pp.lon_0 != ParamValueMissing)
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::PolarStereographic::OrientationOfTheGrid,(Int64 )(pp.lon_0*1000)));

  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::PolarStereographic::DxInMetres,(Int64 )a.xStepMetric));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::PolarStereographic::DyInMetres,(Int64 )a.yStepMetric));
  //newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::PolarStereographic::ProjectionCentreFlag:

  if (a.lat1 < a.lat2)
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::ScanningMode::ScanMode,(Int64 )0x40));
  else
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::ScanningMode::ScanMode,(Int64 )0));
}



/*! \brief Add the GRIB1 lambert conformal grid properties of the grid file area. */

void addGrib1LambertConformalProperties(const GridFileArea& a,const ProjectionParameters& pp,std::map<uint,Int64>& newProperties)
{
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::DataRepresentationType,3LL));

  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::LambertConformal::Nx,a.gridWidth));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::LambertConformal::Ny,a.gridHeight));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::LambertConformal::LatitudeOfFirstGridPoint,(Int64 )(a.lat1*1000)));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::LambertConformal::LongitudeOfFirstGridPoint,(Int64 )(a.lon1*1000)));

  if (pp.lon_0 != ParamValueMissing)
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::LambertConformal::LoV,(Int64 )(pp.lon_0*1000)));

  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::LambertConformal::DxInMetres,(Int64 )a.xStepMetric));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::LambertConformal::DyInMetres,(Int64 )a.yStepMetric));
  //newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::LambertConformal::ProjectionCentreFlag:

  if (pp.lat_1 != ParamValueMissing)
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::LambertConformal::Latin1,(Int64 )(pp.lat_1*1000)));

  if (pp.lat_2 != ParamValueMissing)
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::LambertConformal::Latin2,(Int64 )(pp.lat_2*1000)));

  //newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::LambertConformal::LatitudeOfSouthernPole:
  //newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::LambertConformal::LongitudeOfSouthernPole:


  if (a.lat1 < a.lat2)
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::ScanningMode::ScanMode,(Int64 )0x40));
  else
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB1::Property::GridSection::ScanningMode::ScanMode,(Int64 )0));
}



/*! \brief Add the GRIB2 latlon grid properties of the grid file area. */

void addGrib2LatLonProperties(const GridFileArea& a,std::map<uint,Int64>& newProperties)
{
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::GridDefinitionTemplateNumber,(Int64 )GRIB2::GridSection::Template::LatLon));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::NumberOfGridPoints,a.gridWidth*a.gridHeight));

  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::Grid::Ni,a.gridWidth));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::Grid::Nj,a.gridHeight));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::Grid::LatitudeOfFirstGridPoint,(Int64 )(a.lat1*1000000)));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::Grid::LongitudeOfFirstGridPoint,(Int64 )(a.lon1*1000000)));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::Grid::LatitudeOfLastGridPoint,(Int64 )(a.lat2*1000000)));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::Grid::LongitudeOfLastGridPoint,(Int64 )(a.lon2*1000000)));

  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LatLon::IDirectionIncrement,(Int64 )(a.xStepDegrees*1000000)));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LatLon::JDirectionIncrement,(Int64 )(a.yStepDegrees*1000000)));

  if (a.lat1 < a.lat2)
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LatLon::ScanningMode,(Int64 )0x40));
  else
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LatLon::ScanningMode,(Int64 )0x0));
}



/*! \brief Add the GRIB2 polar stereographic grid properties of the grid file area. */

void addGrib2PolarStereographicProperties(const GridFileArea& a,const ProjectionParameters& pp,std::map<uint,Int64>& newProperties)
{
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::GridDefinitionTemplateNumber,(Int64 )GRIB2::GridSection::Template::PolarStereographic));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::NumberOfGridPoints,a.gridWidth*a.gridHeight));

  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::PolarStereographic::Nx,a.gridWidth));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::PolarStereographic::Ny,a.gridHeight));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::PolarStereographic::LatitudeOfFirstGridPoint,(Int64 )(a.lat1*1000000)));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::PolarStereographic::LongitudeOfFirstGridPoint,(Int64 )(a.lon1*1000000)));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::PolarStereographic::ResolutionAndComponentFlags,48));
  if (pp.laD != ParamValueMissing)
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::PolarStereographic::LaD,(Int64 )(pp.laD*1000000)));
  else
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::PolarStereographic::LaD,60000000));

  if (pp.lon_0 != ParamValueMissing)
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::PolarStereographic::OrientationOfTheGrid,(Int64 )(pp.lon_0*1000000)));

  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::PolarStereographic::Dx,(Int64 )(a.xStepMetric*1000)));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::PolarStereographic::Dy,(Int64 )(a.yStepMetric*1000)));
  //newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::PolarStereographic::ProjectionCentreFlag:

  if (a.lat1 < a.lat2)
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::PolarStereographic::ScanningMode,(Int64 )0x40));
  else
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::PolarStereographic::ScanningMode,(Int64 )0x0));
}



/*! \brief Add the GRIB2 lambert conformal grid properties of the grid file area. */

void addGrib2LambertConformalProperties(const GridFileArea& a,const ProjectionParameters& pp,std::map<uint,Int64>& newProperties)
{
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::GridDefinitionTemplateNumber,(Int64 )GRIB2::GridSection::Template::LambertConformal));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::NumberOfGridPoints,a.gridWidth*a.gridHeight));

  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::Nx,a.gridWidth));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::Ny,a.gridHeight));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::LatitudeOfFirstGridPoint,(Int64 )(a.lat1*1000000)));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::LongitudeOfFirstGridPoint,(Int64 )(a.lon1*1000000)));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::ResolutionAndComponentFlags,48));

  if (pp.laD != ParamValueMissing)
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::LaD,(Int64 )(pp.laD*1000000)));
  else
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::LaD,60000000));

  if (pp.lon_0 != ParamValueMissing)
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::LoV,(Int64 )(pp.lon_0*1000000)));

  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::Dx,(Int64 )(a.xStepMetric*1000)));
  newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::Dy,(Int64 )(a.yStepMetric*1000)));
  //newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::ProjectionCentreFlag:

  if (pp.lat_1 != ParamValueMissing)
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::Latin1,(Int64 )(pp.lat_1*1000000)));

  if (pp.lat_2 != ParamValueMissing)
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::Latin2,(Int64 )(pp.lat_2*1000000)));

  //newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::LatitudeOfSouthernPole:
  //newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::LongitudeOfSouthernPole:

  if (a.lat1 < a.lat2)
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::ScanningMode,(Int64 )0x40));
  else
    newProperties.insert(std::pair<uint,Int64 >((uint)GRIB2::Property::GridSection::LambertConformal::ScanningMode,(Int64 )0x0));
}



/*! \brief Add the grid properties of the projection (of the GRIB version) for the grid file area. */

void addProjectionProperties(const Query& query,uint version,const ProjectionParameters& pp,const GridFileArea& a,std::map<uint,Int64>& newProperties)
{
  bool hasArea = !query.mAreaCoordinates.empty()  && !query.mAreaCoordinates[0].empty();

  if (version == 1)
  {
    if (pp.projectionStr == "longlat")
      addGrib1LatLonProperties(a,newProperties);

    if (pp.projectionStr == "stere"  &&  hasArea)
      addGrib1PolarStereographicProperties(a,pp,newProperties);

    if (pp.projectionStr == "lcc"  &&  hasArea)
      addGrib1LambertConformalProperties(a,pp,newProperties);
  }

  if (version == 2)
  {
    if (pp.projectionStr == "longlat")
      addGrib2LatLonProperties(a,newProperties);

    if (pp.projectionStr == "stere"  &&  hasArea)
      addGrib2PolarStereographicProperties(a,pp,newProperties);

    if (pp.projectionStr == "lcc"  &&  hasArea)
      addGrib2LambertConformalProperties(a,pp,newProperties);
  }
}



/*! \brief Create the GRIB1 message of the grid file (the geometry properties, the time and level properties, and the
           other properties from the original message). */

GRID::Message* newGrib1Message(GRID::GridFile& newGridFile,const char *geometryIdStr,time_t forecastTime,T::ParamLevel paramLevel,const T::ContentInfo& contentInfo1,const std::map<uint,Int64 >& oldProperties,std::map<uint,Int64 >& newProperties)
{
  std::string ftime = utcTimeFromTimeT(forecastTime);

  int year = 0;
  int month = 0;
  int day = 0;
  int hour = 0;
  int minute = 0;
  int second = 0;
  splitTimeString(ftime,year,month,day,hour,minute,second);

  using namespace SmartMet::GRIB1;

  if (geometryIdStr)
  {
    GRIB1::GridDef_sptr def = Identification::gridDef.getGrib1DefinitionByGeometryId(toInt32(geometryIdStr));
    if (def)
    {
      newProperties.insert(std::pair<uint,Int64 >((uint)Property::GridSection::DataRepresentationType,def->getTemplateNumber()));

      T::PropertySettingVec tmpProperties;
      def->getProperties(tmpProperties);

      for (auto& tmpPropertie : tmpProperties)
      {
        //std::cout << "NPROPERTY " << it->propertyId << " = " << it->propertyValue << "\n";
        newProperties.insert(std::pair<uint,Int64 >(tmpPropertie.propertyId,toInt64(tmpPropertie.propertyValue.c_str())));
      }
    }
  }

  GRID::Message *newMessage = newGridFile.newMessage(T::FileTypeValue::Grib1);

  newProperties.insert(std::pair<uint,Int64 >((uint)Property::ProductSection::SectionFlags,128));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::ProductSection::Level,paramLevel));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::ProductSection::YearOfCentury,year % 100));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::ProductSection::Month,month));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::ProductSection::Day,day));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::ProductSection::Hour,hour));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::ProductSection::Minute,minute));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::ProductSection::UnitOfTimeRange,1));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::ProductSection::P1,0));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::ProductSection::P2,0));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::ProductSection::TimeRangeIndicator,0));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::ProductSection::CenturyOfReferenceTimeOfData,(year/100)+1));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::ProductSection::ForecastType,contentInfo1.mForecastType));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::ProductSection::ForecastNumber,contentInfo1.mForecastNumber));

  newProperties.insert(std::pair<uint,Int64 >((uint)Property::DataSection::Flags,8));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::DataSection::BinaryScaleFactor,-5));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::DataSection::ReferenceValue,0.0));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::DataSection::BitsPerValue,32));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::DataSection::PackingMethod,0));

  // ### Setting default values for the parameters

  setMessageProperties(*newMessage,GRIB1::gribProperty.getPropertyVector(),Property::GridSection::FirstProperty,Property::GridSection::LastProperty,newProperties,oldProperties);

  return newMessage;
}



/*! \brief Create the GRIB2 message of the grid file (the geometry properties, the time and level properties, and the
           other properties from the original message). */

GRID::Message* newGrib2Message(GRID::GridFile& newGridFile,const char *geometryIdStr,time_t forecastTime,T::ParamLevel paramLevel,const std::map<uint,Int64 >& oldProperties,std::map<uint,Int64 >& newProperties)
{
  std::string ftime = utcTimeFromTimeT(forecastTime);

  int year = 0;
  int month = 0;
  int day = 0;
  int hour = 0;
  int minute = 0;
  int second = 0;
  splitTimeString(ftime,year,month,day,hour,minute,second);

  //PRINT_DATA(mDebugLog, "-- GRIB version 2\n");

  using namespace SmartMet::GRIB2;

  if (geometryIdStr)
  {
    //Identification::gridDef.getGridLatLonCoordinatesByGeometry(query.mAttributeList,latLonCoordinates,width,height);
    GRIB2::GridDef_sptr def = Identification::gridDef.getGrib2DefinitionByGeometryId(toInt32(geometryIdStr));
    if (def)
    {
      //printf("TEMPLATE %d\n",def->getTemplateNumber());

      auto rows = def->getGridRowCount();
      auto cols = def->getGridColumnCount();

      newProperties.insert(std::pair<uint,Int64 >((uint)Property::GridSection::NumberOfGridPoints,cols*rows));
      newProperties.insert(std::pair<uint,Int64 >((uint)Property::GridSection::GridDefinitionTemplateNumber,def->getTemplateNumber()));

      T::PropertySettingVec tmpProperties;
      def->getProperties(tmpProperties);

      for (auto& tmpPropertie : tmpProperties)
      {
        //std::cout << "NPROPERTY " << it->propertyId << " = " << it->propertyValue << "\n";
        newProperties.insert(std::pair<uint,Int64 >(tmpPropertie.propertyId,toInt64(tmpPropertie.propertyValue.c_str())));
      }
    }
  }

  GRID::Message *newMessage = newGridFile.newMessage(T::FileTypeValue::Grib2);

  newProperties.insert(std::pair<uint,Int64 >((uint)Property::IdentificationSection::SignificanceOfReferenceTime,1));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::IdentificationSection::Year,year));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::IdentificationSection::Month,month));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::IdentificationSection::Day,day));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::IdentificationSection::Hour,hour));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::IdentificationSection::Minute,minute));
  newProperties.insert(std::pair<uint,Int64 >((uint)Property::IdentificationSection::Second,second));

  newProperties.insert(std::pair<uint,Int64 >(Property::RepresentationSection::RepresentationTemplateNumber,RepresentationSection::Template::GridDataRepresentation));

  newProperties.insert(std::pair<uint,Int64 >(Property::ProductSection::ParameterSettings::ForecastTime,0));
  newProperties.insert(std::pair<uint,Int64 >(Property::ProductSection::HorizontalSettings::ScaledValueOfFirstFixedSurface,paramLevel));

  // ### Setting default values for the parameters

  setMessageProperties(*newMessage,GRIB2::gribProperty.getPropertyVector(),Property::GridSection::FirstProperty,Property::GridSection::LastProperty,newProperties,oldProperties);

  return newMessage;
}



/*! \brief Call the data server service of the location type of the value vector query: by geometry, by the grid
           coordinates or by the grid of the content. Returns the result of the call. */

template <class ByGeometry,class ByGrid,class ByDefault>
int fetchByValueVectorLocation(uchar locationType,ByGeometry byGeometry,ByGrid byGrid,ByDefault byDefault)
{
  switch (locationType)
  {
    case QueryParameter::LocationType::Geometry:
      return byGeometry();

    case QueryParameter::LocationType::Grid:
      return byGrid();

    default:
      return byDefault();
  }
}



/*! \brief Return the name of the data server service that fetchByValueVectorLocation calls for the location type. */

const char *valueVectorServiceName(uchar locationType,const char *byGeometry,const char *byGrid,const char *byDefault)
{
  switch (locationType)
  {
    case QueryParameter::LocationType::Geometry:
      return byGeometry;

    case QueryParameter::LocationType::Grid:
      return byGrid;

    default:
      return byDefault;
  }
}



/*! \brief Finish the value vectors: set the default precision and clear the coordinates unless they are requested. */

bool finishValueVectors(QueryParameter& qParam,const ParameterMapping& pInfo,T::Coordinate_vec& coordinates)
{
  setDefaultPrecision(qParam,pInfo);

  if ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) == 0)
    coordinates.clear();

  return true;
}



/*! \brief Return the generation flag (the bit of the generation flags) of the generation index. The generations
           after the 64th have no flag and cannot be selected by the flags. */

UInt64 generationFlag(uint index)
{
  if (index >= 64)
    return 0;

  return (UInt64)1 << index;
}



/*! \brief Return true if the generation (of the index in the generation list) is selected by the analysis time of the
           time range request, or (without an analysis time) by the generation flags. */

bool isTimeRangeGenerationSelected(const TimeRangeValueRequest& r,const T::GenerationInfo *gInfo,const T::GenerationInfo *gNext,int g,uint gLen,UInt64 gFlags)
{
  if (r.analysisTime.empty())
    return (r.generationFlags == 0 || (r.generationFlags & gFlags) != 0);

  if (r.analysisTime == gInfo->mAnalysisTime)
    return true;

  if ((r.queryFlags & Query::Flags::AnalysisTimeMatchRequired) != 0)
    return false;

  if (g == 0 &&  r.analysisTime < gInfo->mAnalysisTime)
    return true;

  if (g == C_INT(gLen-1)  &&  r.analysisTime > gInfo->mAnalysisTime)
    return true;

  return (r.analysisTime == gInfo->mAnalysisTime || (gInfo->mAnalysisTime < r.analysisTime  &&  (gNext == nullptr  ||  gNext->mAnalysisTime > r.analysisTime)));
}



/*! \brief Check that the selected generation of the time range request can be used: it is not deleted soon, its
           analysis time is not after the maximum analysis time, it is ready, and only one generation is used for the
           latest generation. Returns the validity of the generation. */

bool isTimeRangeGenerationUsable(const TimeRangeValueRequest& r,const T::GenerationInfo *gInfo,bool generationValid,uint validCount)
{
  if (gInfo == nullptr || (gInfo != nullptr &&  gInfo->mDeletionTime > 0 && gInfo->mDeletionTime < r.requiredAccessTime))
    return false;

  if (!r.maxAnalysisTime.empty() &&  r.maxAnalysisTime < gInfo->mAnalysisTime)
    return false;

  if (gInfo->mStatus != 1  &&  !r.acceptNotReadyGenerations)
    return false;

  if (generationValid  &&  (r.queryFlags & Query::Flags::LatestGeneration)  &&  validCount > 0)
    return false;

  return generationValid;
}



/*! \brief Add the forecast times (with the analysis time of the generation) of the contents of the generation in the
           time range of the request. The times start over when a content extends further than the previous ones. */

void addGenerationContentTimes(const TimeRangeValueRequest& r,const T::GenerationInfo& gInfo,T::ContentInfoList& contentInfoList,const T::ContentInfo& cFirst,time_t& firstTime,time_t& lastTime,std::map<time_t,std::string>& contentTimeList)
{
  int cLen = contentInfoList.getLength();
  for (int t = cLen-1; t >= 0; t--)
  {
    T::ContentInfo* cInfo = contentInfoList.getContentInfoByIndex(t);
    if (cInfo == nullptr  ||  cInfo->mGenerationId != gInfo.mGenerationId)
      continue;

    if (cInfo->mForecastTimeUTC < r.startTime  ||  cInfo->mForecastTimeUTC > r.endTime  ||  cInfo->mForecastType != cFirst.mForecastType  ||  cInfo->mForecastNumber != cFirst.mForecastNumber)
      continue;

    if (cInfo->mForecastTimeUTC > lastTime)
    {
      contentTimeList.clear();
      lastTime = cInfo->mForecastTimeUTC;
      firstTime = 0xFFFFFFFF;
    }

    if (cInfo->mForecastTimeUTC < firstTime)
    {
      contentTimeList.insert(std::pair<time_t,std::string>(cInfo->mForecastTimeUTC,gInfo.mAnalysisTime));
      firstTime = cInfo->mForecastTimeUTC;
    }
  }
}



/*! \brief Add the aggregation time with the analysis time of the content time (or of the data time if there is no
           content for the time), unless the time is already added. */

void addDataAggregationTime(const std::map<time_t,std::string>& contentTimeList,std::map<time_t,std::string>& contentTimeList2,time_t nt,const std::string& dataAnalysisTime)
{
  if (contentTimeList2.find(nt) != contentTimeList2.end())
    return;

  // Required aggregation time might have different analysis time than the actual data time.

  auto at = contentTimeList.find(nt);
  if (at != contentTimeList.end())
    contentTimeList2.insert(std::pair<time_t,std::string>(nt,at->second));
  else
    contentTimeList2.insert(std::pair<time_t,std::string>(nt,dataAnalysisTime));
}



/*! \brief Return the analysis time of the content time, or of the next (or the same) content time, or the given
           analysis time if there is no such content time. */

std::string getNearestAnalysisTime(const std::map<time_t,std::string>& contentTimeList,time_t t,const std::string& analysisTime)
{
  auto at = contentTimeList.find(t);
  if (at != contentTimeList.end())
    return at->second;

  auto upper = contentTimeList.upper_bound(t);
  if (upper != contentTimeList.end())
    return upper->second;

  auto lower = contentTimeList.lower_bound(t);
  if (lower != contentTimeList.end())
    return lower->second;

  return analysisTime;
}



/*! \brief Add the aggregation time with the nearest analysis time, unless the time is already added. */

void addQueryAggregationTime(const std::map<time_t,std::string>& contentTimeList,std::map<time_t,std::string>& contentTimeList2,time_t nt,const std::string& analysisTime)
{
  if (contentTimeList2.find(nt) == contentTimeList2.end())
    contentTimeList2.insert(std::pair<time_t,std::string>(nt,getNearestAnalysisTime(contentTimeList,nt,analysisTime)));
}



/*! \brief Add the data times of the time range (and the aggregation times before and after them) to the selected
           times, until the requested number of time steps is reached. */

void addDataTimeSteps(TimeRangeValueRequest& r,const std::map<time_t,std::string>& contentTimeList,time_t startTime_tt,time_t endTime_tt,std::map<time_t,std::string>& contentTimeList2,std::set<time_t>& originalTimes)
{
  for (auto it1 = contentTimeList.begin(); it1 != contentTimeList.end(); ++it1)
  {
    // We should pick data times that are in the given time range.

    if (it1->first < startTime_tt  ||  it1->first > endTime_tt)
      continue;

    if (r.query.mTimesteps != 0  &&  r.valueCounter >= r.query.mTimesteps)
      continue;

    // If the parameter requires aggregation then we have to pick some additional times.

    for (uint tt=r.qParam.mTimestepsBefore; tt > 0; tt--)
      addDataAggregationTime(contentTimeList,contentTimeList2,it1->first - (tt * r.parameterTimestepSizeInSeconds),it1->second);

    // Adding the data time.

    contentTimeList2.insert(std::pair<time_t,std::string>(it1->first,it1->second));
    originalTimes.insert(it1->first);

    // If the parameter requires aggregation then we have to pick some additional times.

    for (uint tt=1; tt <= r.qParam.mTimestepsAfter; tt++)
      addDataAggregationTime(contentTimeList,contentTimeList2,it1->first + (tt * r.parameterTimestepSizeInSeconds),it1->second);

    r.valueCounter++;
  }
}



/*! \brief Add the query time steps of the time range (and the aggregation times before and after them) to the selected
           times with their nearest analysis times, until the requested number of time steps is reached. */

void addQueryTimeSteps(TimeRangeValueRequest& r,const std::map<time_t,std::string>& contentTimeList,time_t startTime_tt,time_t endTime_tt,std::map<time_t,std::string>& contentTimeList2,std::set<time_t>& originalTimes)
{
  if (r.queryTimestepSizeInSeconds == 0)
    return;

  auto qst = r.query.mTimesteps;
  if (r.query.mTimesteps == 0  && startTime_tt <= endTime_tt)
  {
    auto td = endTime_tt - startTime_tt;
    qst = (td/r.queryTimestepSizeInSeconds) + 1;
  }

  if (qst <= 0  ||  qst >= 10000)
    return;

  auto stt = startTime_tt;
  for (uint ts = 0; ts<qst; ts++)
  {
    if (r.query.mTimesteps != 0  &&  r.valueCounter >= r.query.mTimesteps)
      continue;

    // If the parameter requires aggregation then we have to pick some additional times.

    for (uint tt=r.qParam.mTimestepsBefore; tt > 0; tt--)
      addQueryAggregationTime(contentTimeList,contentTimeList2,stt - (tt * r.parameterTimestepSizeInSeconds),r.analysisTime);

    // Adding the timestep time and its analysis time.

    contentTimeList2.insert(std::pair<time_t,std::string>(stt,getNearestAnalysisTime(contentTimeList,stt,r.analysisTime)));

    originalTimes.insert(stt);

    // If the parameter requires aggregation then we have to pick some additional times.

    for (uint tt=1; tt <= r.qParam.mTimestepsAfter; tt++)
      addQueryAggregationTime(contentTimeList,contentTimeList2,stt + (tt * r.parameterTimestepSizeInSeconds),r.analysisTime);

    stt += r.queryTimestepSizeInSeconds;
    r.valueCounter++;
  }
}



/*! \brief Add the values of the coordinates set by the function. */

template <class F>
void addCoordinateValues(const T::Coordinate_vec& coordinates,ParameterValues& values,F setValue)
{
  for (const auto& coordinate : coordinates)
  {
    T::GridValue val;
    val.mX = coordinate.x();
    val.mY = coordinate.y();

    setValue(val);
    values.mValueList.addGridValue(val);
  }
}



/*! \brief Function setting the astronomical value of the coordinate of the grid value at the given time. */

using CoordinateValueFunction = void (*)(T::GridValue& val,const Fmi::LocalDateTime& utcTime);



/*! \brief Return the functions of the astronomical additional parameters by their (lower case) names. */

const std::map<std::string,CoordinateValueFunction>& astronomyValueFunctions()
{
  static const std::map<std::string,CoordinateValueFunction> functions =
  {
    {"dark",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::solar_position_t sp = Fmi::Astronomy::solar_position(utcTime, val.mX, val.mY);
        val.mValue = (double)sp.dark();
      }},
    {"sunelevation",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::solar_position_t sp = Fmi::Astronomy::solar_position(utcTime, val.mX, val.mY);
        val.mValue = (double)sp.elevation;
      }},
    {"sundeclination",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::solar_position_t sp = Fmi::Astronomy::solar_position(utcTime, val.mX, val.mY);
        val.mValue = (double)sp.declination;
      }},
    {"sunatzimuth",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::solar_position_t sp = Fmi::Astronomy::solar_position(utcTime, val.mX, val.mY);
        val.mValue = (double)sp.azimuth;
      }},
    {"moonphase",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        val.mValue = Fmi::Astronomy::moonphase(utcTime.utc_time());
      }},
    {"moonrise_utc",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        auto lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_iso_string(lt.moonrise.local_time());
      }},
    {"moonrise2_utc",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        if (lt.moonrise2_today())
          val.mValueString = Fmi::to_iso_string(lt.moonrise2.local_time());
        else
          val.mValueString = std::string("");
      }},
    {"moonset_utc",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_iso_string(lt.moonset.local_time());
      }},
    {"moonset2_utc",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        if (lt.moonset2_today())
          val.mValueString = Fmi::to_iso_string(lt.moonset2.local_time());
        else
          val.mValueString = "";
      }},
    {"moonrisetoday_utc",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(lt.moonrise_today());
      }},
    {"moonrise2today_utc",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(lt.moonrise2_today());
      }},
    {"moonsettoday_utc",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(lt.moonset_today());
      }},
    {"moonset2today_utc",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(lt.moonset2_today());
      }},
    {"moonup24h",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY,true);
        val.mValue = (double)lt.above_horizont_24h();
      }},
    {"moondown24h",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::lunar_time_t lt = Fmi::Astronomy::lunar_time(utcTime, val.mX, val.mY);
        val.mValue = (double)(!lt.moonrise_today() && !lt.moonset_today() && !lt.above_horizont_24h());
      }},
    {"sunrise_utc",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_iso_string(st.sunrise.local_time());
      }},
    {"sunset_utc",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_iso_string(st.sunset.local_time());
      }},
    {"noon_utc",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_iso_string(st.noon.local_time());
      }},
    {"sunrisetoday_utc",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(st.sunrise_today());
      }},
    {"sunsettoday_utc",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        val.mValueString = Fmi::to_string(st.sunset_today());
      }},
    {"daylength",[](T::GridValue& val,const Fmi::LocalDateTime& utcTime)
      {
        Fmi::Astronomy::solar_time_t st = Fmi::Astronomy::solar_time(utcTime, val.mX, val.mY);
        auto seconds = st.daylength().total_seconds();
        auto minutes = Fmi::numeric_cast<long>(round(seconds / 60.0));
        val.mValue = minutes;
      }}
  };

  return functions;
}



/*! \brief Run the function and ignore its exceptions. */

template <class F>
void runIgnoringExceptions(F f)
{
  try
  {
    f();
  }
  catch (...)
  {
  }
}



/*! \brief Contour attributes of the query. */

struct ContourAttributes
{
  int gridWidth = 0;
  int gridHeight = 0;
  short interpolationType = 0;
  size_t smoothSize = 0;
  size_t smoothDegree = 0;
  int contourSubdivide = 0;
  int contourThreads = 1;
  Trax::SmoothOptions smoothOptions;
};



/*! \brief Return the contour attributes of the query attributes. */

ContourAttributes getContourAttributes(const T::AttributeList& attributeList)
{
  ContourAttributes ca;

  const char *gridWidthStr = attributeList.getAttributeValue("grid.width");
  if (gridWidthStr != nullptr)
    ca.gridWidth = toInt32(gridWidthStr);

  const char *gridHeightStr = attributeList.getAttributeValue("grid.height");
  if (gridHeightStr != nullptr)
    ca.gridHeight = toInt32(gridHeightStr);

  const char *interpolationTypeStr = attributeList.getAttributeValue("contour.interpolation.type");
  if (interpolationTypeStr != nullptr)
    ca.interpolationType = toInt16(interpolationTypeStr);

  const char *smoothSizeStr = attributeList.getAttributeValue("contour.smooth.size");
  if (smoothSizeStr != nullptr)
    ca.smoothSize = toSize_t(smoothSizeStr);

  const char *smoothDegreeStr = attributeList.getAttributeValue("contour.smooth.degree");
  if (smoothDegreeStr != nullptr)
    ca.smoothDegree = toSize_t(smoothDegreeStr);

  const char *contourSubdivideStr = attributeList.getAttributeValue("contour.subdivide");
  if (contourSubdivideStr != nullptr)
    ca.contourSubdivide = toInt32(contourSubdivideStr);

  const char *contourThreadsStr = attributeList.getAttributeValue("contour.threads");
  if (contourThreadsStr != nullptr)
    ca.contourThreads = toInt32(contourThreadsStr);

  // Opt-in Trax grid smoother (box / median / morphology). Active only when a
  // method is given; otherwise an inactive option is a no-op in grid-files.
  // The legacy contour.smooth.size/degree above remain unused (Savitzky-Golay
  // is not implemented on the grid path).
  const char *smoothMethodStr = attributeList.getAttributeValue("contour.smooth.method");
  if (smoothMethodStr != nullptr)
  {
    ca.smoothOptions.method = Trax::to_smooth_method(smoothMethodStr);
    const char *str = nullptr;
    if ((str = attributeList.getAttributeValue("contour.smooth.radius")) != nullptr)
      ca.smoothOptions.radius = toInt32(str);
    if ((str = attributeList.getAttributeValue("contour.smooth.passes")) != nullptr)
      ca.smoothOptions.passes = toInt32(str);
    if ((str = attributeList.getAttributeValue("contour.smooth.boundary")) != nullptr)
      ca.smoothOptions.boundary = Trax::to_smooth_boundary(str);
    if ((str = attributeList.getAttributeValue("contour.smooth.morphology")) != nullptr)
      ca.smoothOptions.morphology = Trax::to_morphology_op(str);
    if ((str = attributeList.getAttributeValue("contour.smooth.preserve_missing")) != nullptr)
      ca.smoothOptions.preserve_missing = (toInt32(str) != 0);
  }

  return ca;
}



/*! \brief Calculate the isolines or isobands of the values of a function or metric level parameter. */

void contourParameterValues(QueryParameter& qParam,const ContourAttributes& ca)
{
  auto sz = qParam.mCoordinates.size();
  if ((!qParam.mFunction.empty() || (qParam.mFlags & QueryServer::QueryParameter::Flags::MetricLevels))  &&  !qParam.mCoordinates.empty())
  {
    switch (qParam.mType)
    {
      case QueryParameter::Type::Isoline:
      {
        for (auto pValue = qParam.mValueList.begin(); pValue != qParam.mValueList.end(); ++pValue)
        {
          if ((*pValue)->mValueVector.size() == sz)
          {
            getIsolines(
                (*pValue)->mValueVector,
                &qParam.mCoordinates,
                ca.gridWidth,
                ca.gridHeight,
                qParam.mContourLowValues,
                ca.interpolationType,
                ca.smoothSize,
                ca.smoothDegree,
                (*pValue)->mValueData,
                ca.contourSubdivide,
                ca.contourThreads,
                ca.smoothOptions);
          }
        }
      }
      break;

      case QueryParameter::Type::Isoband:
      {
        for (auto pValue = qParam.mValueList.begin(); pValue != qParam.mValueList.end(); ++pValue)
        {
          if ((*pValue)->mValueVector.size() == sz)
          {
            getIsobands(
                (*pValue)->mValueVector,
                &qParam.mCoordinates,
                ca.gridWidth,
                ca.gridHeight,
                qParam.mContourLowValues,
                qParam.mContourHighValues,
                ca.interpolationType,
                ca.smoothSize,
                ca.smoothDegree,
                (*pValue)->mValueData,
                ca.contourSubdivide,
                ca.contourThreads,
                ca.smoothOptions);
          }
        }
      }
      break;

      case QueryParameter::Type::StreamLine:
      {

      }
      break;
      default:
        break;
    }
  }
}



/*! \brief Set the time step attributes of an internal aggregation function ("/..."): the parameters 2-4 are the steps before, the steps after and the step size in minutes. */

void setAggregationParameters(QueryParameter& queryParam,const FunctionParam_vec& functionParams)
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



/*! \brief Split the parameter string into fields at the separators that are not inside functions. Returns the number of fields. */

uint splitParameterFields(char* buf,char fieldSeparator,char** field)
{
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
  return c;
}



/*! \brief Return true if the field of the index exists and is not empty. */

bool hasField(char** field,uint c,uint index)
{
  return c > index  &&  field[index][0] != '\0';
}



/*! \brief Add the comma separated levels of the level field ("M" prefix for metric levels). */

void addLevels(const char *levelField,uint& flags,std::vector<T::ParamLevel>& paramLevelVec)
{
  std::vector<std::string> numbers;
  if (isdigit(levelField[0]))
  {
    splitNumbers(levelField,numbers);
  }
  else
  {
    if (toupper(levelField[0]) == 'M')
    {
      flags |= QueryServer::QueryParameter::Flags::MetricLevels;
      splitNumbers(levelField+1,numbers);
    }
  }
  for (auto& number : numbers)
    paramLevelVec.emplace_back(toInt32(number));
}



/*! \brief Split the function parameters at the ';' separators that are not inside functions. Returns true if the parameters contain a function. */

bool splitFunctionParams(const std::string& functionParamsStr,std::vector<std::string>& pnames)
{
  bool containsFunction = false;
  std::vector<char> tmp(functionParamsStr.size()+1);
  uint c = 0;

  const char* p = functionParamsStr.c_str();
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
      pnames.emplace_back(tmp.data());
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
    pnames.emplace_back(tmp.data());
  }

  return containsFunction;
}



/*! \brief Split the producer file line into ';'-separated fields that end at a '#' comment (separators within quotes are ignored). Returns the number of fields. */

uint splitProducerFields(char* st,char** field)
{
  bool ind = false;
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
  return c;
}



/*! \brief Skip the leading blanks of the field and end it at the next blank. */

void trimField(char*& field)
{
  // The field should start from non-empty char
  while (*field != '\0' &&  *field <= ' ')
    field++;

  // There should be no empty chars in the field

  char *f = field;
  while (*f != '\0')
  {
    if (*f <= ' ')
      *f = '\0';
    else
      f++;
  }
}


thread_local ContentCache mContentCache;
thread_local std::size_t mContentCache_records;
thread_local time_t mContentCache_clearRequiredTime;
thread_local time_t mContentCache_clearTime;
thread_local bool thread_init = false;

thread_local ContentSearchCache mContentSearchCache;
thread_local std::size_t mContentSearchCache_records;
thread_local time_t mContentSearchCache_clearRequiredTime;
thread_local time_t mContentSearchCache_clearTime;



thread_local ParameterMappingCache mParameterMappingCache;
thread_local time_t mParameterMappingCache_clearTime;

thread_local std::set<UInt64 > mReadyGeometryList;

// Geometry ids for which a "no registered definition" warning has already been printed to
// the terminal (=> journal). File-local and mutex-protected so that the warning is emitted
// only once per geometry across all worker threads. The set stays small (one entry per
// misconfigured geometry) and is touched only when such a geometry is actually encountered.
std::set<T::GeometryId> mUnregisteredGeometryWarnings;
ModificationLock mUnregisteredGeometryWarnings_modificationLock;

thread_local ProducerGenarationListCache mProducerGenerationListCache;
thread_local time_t mProducerGenerationListCache_clearTime;





void* queryServer_updateThread(void *arg)
{
  try
  {
    Fmi::set_thread_name("upd-qs");
    auto *service = static_cast<ServiceImplementation*>(arg);
    service->updateProcessing();
    return nullptr;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Operation failed!",nullptr);
    exception.printError();
    // A failed worker thread ends the process. std::_Exit does not run the static
    // destructors that the other threads may still be using, unlike exit().
    std::_Exit(-1);
  }
}
}  // namespace




/*! \brief Query Server: Constructor. */

ServiceImplementation::ServiceImplementation()
{
  FUNCTION_TRACE
  try
  {
    mThread = 0;  // joined in the destructor only if the thread was started
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





/*! \brief Query Server: Destructor. */

ServiceImplementation::~ServiceImplementation()
{
  FUNCTION_TRACE
  try
  {
    mShutdownRequested = true;
    if (mThread != 0)
    {
      pthread_join(mThread, nullptr);
      mThread = 0;
    }
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





/*! \brief Query Server: Init. */

void ServiceImplementation::init(
    ContentServer::ServiceInterface* contentServerPtr,
    DataServer::ServiceInterface* dataServerPtr,
    const std::string& gridConfigFile,
    const std::string& heightConversionFile,
    const string_vec& parameterMappingFiles,
    const std::string& unitConversionFile,
    const string_vec& parameterMappingAliasFiles,
    string_vec& aliasFiles,
    const std::string& producerFile,
    string_vec& producerAliasFiles,
    const string_vec& luaFiles,
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
    auto *k2f = new Functions::Function_sequence();
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
    // (Was 360.0/2*3.1415926535, i.e. 180*pi, due to operator precedence)
    mFunctionCollection.addFunction("RAD2DEG",new Functions::Function_multiply(360.0/(2*3.1415926535)));

    // Degrees to radians
    mFunctionCollection.addFunction("DEG2RAD",new Functions::Function_multiply((2*3.1415926535/360.0)));

    mFunctionCollection.addFunction("WIND_SPEED",new Functions::Function_hypotenuse());
    mFunctionCollection.addFunction("WIND_DIR",new Functions::Function_windDir());
    mFunctionCollection.addFunction("WIND_V",new Functions::Function_windV());
    mFunctionCollection.addFunction("WIND_U",new Functions::Function_windU());

    mFunctionCollection.addFunction("STREAM_SPEED",new Functions::Function_hypotenuse());
    mFunctionCollection.addFunction("STREAM_DIR",new Functions::Function_streamDir());
    mFunctionCollection.addFunction("STREAM_V",new Functions::Function_streamV());
    mFunctionCollection.addFunction("STREAM_U",new Functions::Function_streamU());

    mUnitConversionFile = unitConversionFile;
    if (!mUnitConversionFile.empty())
      loadUnitConversionFile();

    mParameterMappingFiles = parameterMappingFiles;

    for (auto& mParameterMappingFile : mParameterMappingFiles)
    {
      ParameterMappingFile mapping(mParameterMappingFile);
      mParameterMappings.emplace_back(mapping);
    }

    for (auto& mParameterMapping : mParameterMappings)
    {
      mParameterMapping.init();

      for (const auto& parameterMappingAliasFile : parameterMappingAliasFiles)
      {
        if (parameterMappingAliasFile == mParameterMapping.getFilename())
        {
          ParameterMapping_vec aliasMappings;
          mParameterMapping.getAliasMappings(aliasMappings,mUnitConversions);

          ParameterMappingFile mapping;
          mapping.setParameterMappings(aliasMappings);
          mParameterAliasMappings.emplace_back(mapping);
        }
      }
    }

    // Load the producers and generations before accepting queries. Otherwise the queries made
    // right after the start returned no data until the update thread had run for the first time.
    try
    {
      checkProducerMapUpdates();
      checkGenerationUpdates();
    }
    catch (...)
    {
      Fmi::Exception exception(BCP, "Initial producer and generation update failed, retrying later", nullptr);
      exception.printError();
    }

    startUpdateProcessing();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Init content cache. */

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




/*! \brief Query Server: Init content search cache. */

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





/*! \brief Query Server: Get state attributes. */

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





/*! \brief Query Server: Get cache stats. */

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





/*! \brief Query Server: Set land cover. */

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





/*! \brief Query Server: Set dem. */

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





/*! \brief Query Server: Shutdown. */

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





/*! \brief Query Server: Get producer info by name. */

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




/*! \brief Query Server: Get generation time range by producer and generation id. */

void ServiceImplementation::getGenerationTimeRangeByProducerAndGenerationId(T::ProducerId producerId,T::GenerationId generationId,time_t& startTime,time_t& endTime)
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





/*! \brief Query Server: Get generation info list by producer id. */

CacheEntry_sptr ServiceImplementation::getGenerationInfoListByProducerId(T::ProducerId producerId,UInt64 producerHash,bool acceptNotReadyGenerations)
{
  FUNCTION_TRACE
  try
  {
    if (mProducerGenerationListCache_clearRequired > mProducerGenerationListCache_clearTime)
    {
      mProducerGenerationListCache.clear();
      mProducerGenerationListCache_clearTime = time(nullptr);
    }

    T::ProducerId key = producerId;
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
      // Remove old entry
      mProducerGenerationListCache.erase(gl);
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

    mProducerGenerationListCache.insert(std::pair<T::ProducerId,CacheEntry_sptr>(key,cacheEntry));

    return cacheEntry;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






/*! \brief Query Server: Print the query to the debug log when the execution starts. */

void ServiceImplementation::printQueryStart(Query& query)
{
  FUNCTION_TRACE
  try
  {
    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      query.print(stream, 0, 0);
      PRINT_DATA(mDebugLog, "\n############################## QUERY EXECUTION START ##################\n\n");
      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Print the end of the query execution to the debug log. */

void ServiceImplementation::printQueryEnd()
{
  FUNCTION_TRACE
  try
  {
    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      PRINT_DATA(mDebugLog, "\n############################### QUERY EXECUTION END ###################\n\n");
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Protected hook: execute query. */

int ServiceImplementation::_executeQuery(T::SessionId /*sessionId*/, Query& query)
{
  FUNCTION_TRACE
  try
  {
    if (mShutdownRequested)
      return 0;

    // query.print(std::cout,0,0);

    query.mProcessingStartTime = time(nullptr);

    mRequestCounter++;

    printQueryStart(query);

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

    printQueryEnd();

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






/*! \brief Query Server: Protected hook: get producer list. */

int ServiceImplementation::_getProducerList(T::SessionId /*sessionId*/, string_vec& producerList)
{
  FUNCTION_TRACE
  try
  {
    if (mShutdownRequested)
      return 0;

    producerList.clear();
    std::string prev;

    AutoReadLock lock(&mProducerList_modificationLock);
    for (auto& it : mProducerList)
    {
      if (it.first != prev)
        producerList.emplace_back(it.first);

      prev = it.first;
    }
    return QueryServer::Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Add the "producer:geometry" field of the producer file line: the first field is a producer
           (and the key of the line), the next ones are the producers concatenated to it. */

void ServiceImplementation::addProducerField(uint t,const char *field,std::string& key)
{
  FUNCTION_TRACE
  try
  {
    std::vector<std::string> parts;
    splitString(field,':',parts);
    if (parts.size() != 2)
      return;

    if (t == 0)
    {
      key = toUpperString(field);
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
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Query Server: Load producer file. */

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

    while (fgets(st, 1000, file) != nullptr)
    {
      if (st[0] != '#')
      {
        char* field[100];
        uint c = splitProducerFields(st,field);
        c--;

        std::string key;
        for (uint t=0; t<c; t++)
        {
          trimField(field[t]);

          //printf("FIELD [%u][%s]\n",t,field[t]);
          if (field[t][0] != '\0')
            addProducerField(t,field[t],key);
        }
      }
    }
    (void)fclose(file);

    mProducerFile_modificationTime = getFileModificationTime(mProducerFile.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Query Server: Load unit conversion file. */

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

    while (fgets(st, 1000, file) != nullptr)
    {
      if (st[0] != '#')
      {
        char* field[100];
        uint c = splitFields(st,field);
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
    (void)fclose(file);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







/*! \brief Query Server: Load height conversion file. */

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

    while (fgets(st, 1000, file) != nullptr)
    {
      if (st[0] != '#')
      {
        char* field[100];
        uint c = splitFields(st,field);
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
            Fmi::hash_merge(hash,rec.producerId);
            Fmi::hash_merge(hash,rec.levelId);

            mHeightConversions.insert(std::pair<std::size_t,HeightConversion>(hash,rec));
          }
        }
      }
    }
    (void)fclose(file);

    mHeightConversionsFile_modificationTime = getFileModificationTime(mHeightConversionsFile.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




namespace
{

/*! \brief Check that all the coordinates are inside the given geometry. */

bool geometryCoversCoordinates(T::GeometryId geometryId,T::AreaCoordinates& coordinates)
{
  bool match = true;
  for (auto& cList : coordinates)
  {
    for (auto coordinate = cList.begin(); coordinate != cList.end(); ++coordinate)
    {
      double grid_i = ParamValueMissing;
      double grid_j = ParamValueMissing;

      if (!Identification::gridDef.getGridPointByGeometryIdAndLatLonCoordinates(geometryId,coordinate->y(), coordinate->x(),grid_i,grid_j))
        match = false;
    }
  }
  return match;
}

}  // namespace





/*! \brief Query Server: Report a producer geometry which has no registered definition. */

void ServiceImplementation::reportUnregisteredGeometry(T::GeometryId geometryId)
{
  FUNCTION_TRACE
  try
  {
    // The geometry itself has no registered definition, so its coverage cannot be
    // verified. This is different from the case where the geometry is registered
    // but does not cover the requested coordinates. The candidate is still rejected
    // either way, since a data fetch would fail later for the same reason.
    PRINT_DATA(mDebugLog, "  - Geometry '%d' has no registered definition. Coverage cannot be verified!\n", geometryId);

    // The debug log is normally disabled in production, so also warn to the terminal
    // (=> journal) where maintenance can see it. Print only once per geometry to avoid
    // flooding the journal, since this method is called for every query.
    bool firstWarning = false;
    {
      AutoWriteLock lock(&mUnregisteredGeometryWarnings_modificationLock);
      firstWarning = mUnregisteredGeometryWarnings.insert(geometryId).second;
    }
    if (firstWarning)
    {
      Fmi::Exception warning(BCP, "Producer refers to a grid geometry that has no registered definition; check the grid geometry configuration");
      warning.addParameter("GeometryId", std::to_string(geometryId));
      warning.printError();
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get geometry id list by coordinates. */

void ServiceImplementation::getGeometryIdListByCoordinates(Producer_vec& producers,T::AreaCoordinates& coordinates, std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    std::set<T::GeometryId> registeredGeometryIds;
    Identification::gridDef.getGeometryIdList(registeredGeometryIds);

    for (auto prod = producers.begin(); prod != producers.end(); ++prod)
    {
      if (registeredGeometryIds.find(prod->second) == registeredGeometryIds.end())
      {
        reportUnregisteredGeometry(prod->second);
        continue;
      }

      if (geometryCoversCoordinates(prod->second,coordinates))
        geometryIdList.insert(prod->second);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get geometry id list by coordinates. */

void ServiceImplementation::getGeometryIdListByCoordinates(uint /*gridWidth*/,uint /*gridHeight*/,const std::vector<T::Coordinate>& coordinates, std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    std::map <T::GeometryId,uint> countList;
    uint maxCount = 0;

    if (coordinates.empty())
      return;

    for (const auto& coordinate : coordinates)
    {
      std::set<T::GeometryId> idList;
      Identification::gridDef.getGeometryIdListByLatLon(coordinate.y(), coordinate.x(), idList);

      for (int g : idList)
      {
        auto c = countList.find(g);
        if (c != countList.end())
        {
          c->second++;
          maxCount = std::max(c->second, maxCount);
        }
        else
        {
          if (maxCount == 0)
            maxCount = 1;

          countList.insert(std::pair<T::GeometryId, uint>(g, 1));
        }
      }
    }

    for (auto& it : countList)
    {
      if (it.second == maxCount)
      {
        geometryIdList.insert(it.first);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get geometry id list by geometry id. */

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





/*! \brief Query Server: Get parameter function info. */

bool ServiceImplementation::getParameterFunctionInfo(const std::string& paramStr, std::string& function,std::string& functionParams)
{
  FUNCTION_TRACE
  try
  {
    std::string bufStr(paramStr);
    char* buf = bufStr.data();
    std::vector<char> tmp(bufStr.size()+1);

    uint c = 0;
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
            functionParams = tmp.data();
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





/*! \brief Query Server: Add the function parameter, or one parameter for each of its levels and forecast numbers. */

void ServiceImplementation::addFunctionParam(const std::string& pname,FunctionParam_vec& functionParams)
{
  FUNCTION_TRACE
  try
  {
    std::string key;
    T::GeometryId geometryId = 0;
    T::ParamLevelId paramLevelId = 0;
    T::ParamLevel paramLevel = 0;
    T::ForecastType forecastType = 1;
    std::vector<T::ParamLevel> paramLevelVec;
    std::vector<T::ForecastNumber> forecastNumberVec;
    std::string producerName;
    T::ProducerId producerId = 0;
    UInt64 generationFlags = 0;
    short areaInterpolationMethod = 0;
    short timeInterpolationMethod = 0;
    short levelInterpolationMethod = 0;
    uint flags = 0;

    getParameterStringInfo(pname,':',key,geometryId,paramLevelId,paramLevelVec,forecastType,forecastNumberVec,
        producerName,producerId,generationFlags,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,flags);

    size_t lsz = paramLevelVec.size();
    size_t fsz = forecastNumberVec.size();
    if (lsz < 2  &&  fsz < 2)
    {
      mFunctionParamId++;
      functionParams.emplace_back(mFunctionParamId,pname);
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

      for (int & lIt : paramLevelVec)
      {
        paramLevel = lIt;
        for (short & fIt : forecastNumberVec)
        {
          std::string buf;
          if (paramLevelUnit)
            buf = fmt::format("{}:{}:{}:{}:{}{}:{}:{}:{}", key,producerName,geometryId,paramLevelId,paramLevelUnit,
                paramLevel,forecastType,fIt,generationFlags);
          else
            buf = fmt::format("{}:{}:{}:{}:{}:{}:{}:{}", key,producerName,geometryId,paramLevelId,paramLevel,
                forecastType,fIt,generationFlags);

          mFunctionParamId++;
          functionParams.emplace_back(mFunctionParamId, buf);
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get function params. */

bool ServiceImplementation::getFunctionParams(const std::string& functionParamsStr, FunctionParam_vec& functionParams)
{
  FUNCTION_TRACE
  try
  {
    std::vector<std::string> pnames;
    bool containsFunction = splitFunctionParams(functionParamsStr,pnames);

    for (auto& pname : pnames)
      addFunctionParam(pname,functionParams);

    if (mFunctionParamId > 4000000000)
      mFunctionParamId = 1000;

    return containsFunction;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get parameter mappings. */

void ServiceImplementation::getParameterMappings(const std::string& producerName,T::ProducerId producerId,const std::string& parameterName,std::size_t parameterHash,T::GeometryId geometryId, bool onlySearchEnabled, ParameterMapping_vec_sptr& mappings)
{
  try
  {
    std::size_t hash = 0;

    if (producerId != 0)
      Fmi::hash_merge(hash,producerId);
    else
      Fmi::hash_merge(hash,producerName);

    if (parameterHash != 0)
      Fmi::hash_merge(hash,parameterHash);
    else
      Fmi::hash_merge(hash,parameterName);

    Fmi::hash_merge(hash,geometryId);
    Fmi::hash_merge(hash,onlySearchEnabled);

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

      mappings = std::make_shared<ParameterMapping_vec>();

      for (auto& mParameterMapping : mParameterMappings)
      {
        mParameterMapping.getMappings(producerName, parameterName, geometryId, onlySearchEnabled, *mappings);
      }

      for (auto& mParameterAliasMapping : mParameterAliasMappings)
      {
        mParameterAliasMapping.getMappings(producerName, parameterName, geometryId, onlySearchEnabled, *mappings);
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





/*! \brief Query Server: Get parameter mappings. */

void ServiceImplementation::getParameterMappings(
    const std::string& producerName,
    T::ProducerId producerId,
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
      Fmi::hash_merge(hash,producerId);
    else
      Fmi::hash_merge(hash,producerName);

    if (parameterHash != 0)
      Fmi::hash_merge(hash,parameterHash);
    else
      Fmi::hash_merge(hash,parameterName);

    Fmi::hash_merge(hash,geometryId);
    Fmi::hash_merge(hash,levelId);
    Fmi::hash_merge(hash,level);
    Fmi::hash_merge(hash,onlySearchEnabled);

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

      mappings = std::make_shared<ParameterMapping_vec>();

      for (auto& mParameterMapping : mParameterMappings)
      {
        mParameterMapping.getMappings(producerName, parameterName, geometryId, levelId, level, onlySearchEnabled, *mappings);
      }

      for (auto& mParameterAliasMapping : mParameterAliasMappings)
      {
        mParameterAliasMapping.getMappings(producerName, parameterName, geometryId, levelId, level, onlySearchEnabled, *mappings);
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





/*! \brief Query Server: Get parameter string info. */

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
    T::ProducerId& producerId,
    UInt64 & generationFlags,
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

    if (!paramLevelVec.empty())
      paramLevel = paramLevelVec[0];

    if (!forecastNumberVec.empty())
      forecastNumber = forecastNumberVec[0];

  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get parameter string info. */

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
    T::ProducerId& producerId,
    UInt64 & generationFlags,
    short& areaInterpolationMethod,
    short& timeInterpolationMethod,
    short& levelInterpolationMethod,
    uint& flags)

{
  FUNCTION_TRACE
  try
  {
    std::string bufStr(param);
    char* buf = bufStr.data();

    char* field[100];
    uint c = splitParameterFields(buf,fieldSeparator,field);

    key = field[0];
    std::string alias;
    if (getAlias(key, alias))
    {
      // Alias cannot be a function.
      if (strchr(alias.c_str(),'{') == nullptr)
        key = alias;
    }
    if (hasField(field,c,1))
    {
      producerName = field[1];
      T::ProducerInfo producerInfo;
      if (getProducerInfoByName(producerName, producerInfo))
        producerId = producerInfo.mProducerId;
    }

    if (hasField(field,c,2))
      geometryId = toInt32(field[2]);

    if (hasField(field,c,3))
      paramLevelId = toUInt8(field[3]);

    if (hasField(field,c,4))
      addLevels(field[4],flags,paramLevelVec);

    if (hasField(field,c,5))
      forecastType = toInt16(field[5]);

    if (hasField(field,c,6))
    {
      std::vector<std::string> numbers;
      splitNumbers(field[6],numbers);
      for (auto& number : numbers)
        forecastNumberVec.emplace_back(toInt16(number));
    }

    if (hasField(field,c,7))
      generationFlags = toUInt64(field[7]);

    if (hasField(field,c,8))
      areaInterpolationMethod = toInt16(field[8]);

    if (hasField(field,c,9))
      timeInterpolationMethod = toInt16(field[9]);

    if (hasField(field,c,10))
      levelInterpolationMethod = toInt16(field[10]);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get alias. */

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



/*! \brief Query Server: Is geometry valid. */

bool ServiceImplementation::isGeometryValid(int geometryId,std::vector<std::vector<T::Coordinate>>& polygonPath)
{
  try
  {
    for (auto& cList : polygonPath)
    {
      for (auto coordinate = cList.begin(); coordinate != cList.end(); ++coordinate)
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





/*! \brief Query Server: Is geometry ready. */

bool ServiceImplementation::isGeometryReady(T::GenerationId generationId,int geometryId,T::ParamLevelId levelId)
{
  try
  {
    if (!mCheckGeometryStatus)
      return true;

    UInt64 key = ((UInt64 )generationId << 24) + ((UInt64 )geometryId << 8) + levelId;

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





/*! \brief Query Server: Return a temporary query parameter for the function parameter, inheriting the attributes of the query parameter (and of the alias of the parameter). */

QueryParameter ServiceImplementation::newFunctionParameter(const QueryParameter& queryParam,const std::string& paramStr,const FunctionParam& functionParam,int contourCoordinateType)
{
  FUNCTION_TRACE
  try
  {
    QueryParameter newParam;
    newParam.mId = functionParam.first;
    newParam.mParam = functionParam.second;
    newParam.mOrigParam = paramStr + " => " + functionParam.second;
    newParam.mSymbolicName = functionParam.second;
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
    if (getAlias(functionParam.second, alias))
    {
      newParam.mParam = alias;
      newParam.mOrigParam = newParam.mOrigParam + " => " + alias;
      std::string paramName;

      getParameterStringInfo(functionParam.second, ':', paramName, newParam.mGeometryId, newParam.mParameterLevelId, newParam.mParameterLevel, newParam.mForecastType, newParam.mForecastNumber,
          newParam.mProducerName,newParam.mProducerId, newParam.mGenerationFlags, newParam.mAreaInterpolationMethod, newParam.mTimeInterpolationMethod, newParam.mLevelInterpolationMethod,newParam.mFlags);
    }

    return newParam;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Parse function. */

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
      contourCoordinateType = toInt32(contourCoordinateTypeStr);


    PRINT_DATA(mDebugLog, "%s   + Parse parameter : %s\n", space(recursionCounter).c_str(), paramStr.c_str());
    std::string functionParamsStr;
    if (getParameterFunctionInfo(paramStr, function, functionParamsStr))
    {
      PRINT_DATA(mDebugLog, "%s     - Function: %s\n", space(recursionCounter).c_str(), function.c_str());
      PRINT_DATA(mDebugLog, "%s     - Params  : %s\n", space(recursionCounter).c_str(), functionParamsStr.c_str());

      getFunctionParams(functionParamsStr, functionParams);

      if (function.substr(0,1) == "/")
        setAggregationParameters(queryParam,functionParams);

      for (auto& functionParam : functionParams)
      {
        if (functionParam.second != "0" && toDouble(functionParam.second.c_str()) == 0)
        {
          QueryParameter newParam = newFunctionParameter(queryParam,paramStr,functionParam,contourCoordinateType);

          parseFunction(query,newParam, newParam.mParam, newParam.mFunction, newParam.mFunctionParams, recursionCounter + 2, additionalParameterList);

          additionalParameterList.insert(additionalParameterList.begin(), newParam);
        }
      }
      return true;
    }
    std::string producerName;

    std::string paramName;
    getParameterStringInfo(paramStr, ':', paramName, queryParam.mGeometryId, queryParam.mParameterLevelId, queryParam.mParameterLevel, queryParam.mForecastType, queryParam.mForecastNumber, producerName,
        queryParam.mProducerId, queryParam.mGenerationFlags, queryParam.mAreaInterpolationMethod, queryParam.mTimeInterpolationMethod, queryParam.mLevelInterpolationMethod, queryParam.mFlags);

    std::string alias;
    if (getAlias(paramName, alias)  &&  alias != paramStr)
    {
      if (!parseFunction(query,queryParam, alias, function, functionParams, recursionCounter + 1, additionalParameterList))
        queryParam.mParam = alias;
    }
    return false;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP, "Parsing failed!", nullptr);
    exception.addParameter("ParamStr", paramStr);
    throw exception;
  }
}






/*! \brief Query Server: Get producers. */

void ServiceImplementation::getProducers(const Query& query, Producer_vec& producers)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mProducerList_modificationLock);

    if (query.mProducerNameList.empty())
    {
      producers = mProducerList;
      return;
    }

    for (const auto& pName : query.mProducerNameList)
    {
      for (auto& it : mProducerList)
      {
        if (strcasecmp(pName.c_str(), it.first.c_str()) == 0)
        {
          producers.emplace_back(it.first, it.second);
        }
        else
        {
          std::string alias;
          if (mProducerAliasFileCollection.getAlias(pName, alias))
          {
            if (strcasecmp(alias.c_str(), it.first.c_str()) == 0)
            {
              producers.emplace_back(it.first, it.second);
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





/*! \brief Query Server: Get producers. */

void ServiceImplementation::getProducers(const std::string& producerName, Producer_vec& producers)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mProducerList_modificationLock);
    for (auto& it : mProducerList)
    {
      if (strcasecmp(producerName.c_str(), it.first.c_str()) == 0)
      {
        producers.emplace_back(it.first, it.second);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Update the query parameter: inherit its level information, replace an alias by its definition and parse its function.
           Returns true if an alias was replaced (its definition may contain other aliases). */

bool ServiceImplementation::updateQueryParameter(Query& query,QueryParameter& qParam,std::vector<QueryParameter>& additionalParameterList)
{
  FUNCTION_TRACE
  try
  {
    bool updateRequired = false;

    // The parameter can contain some level information even if the parameter is an alias
    // or a function. In this case this level information should be inherited to the alias
    // definition or to the function parameters.

    PRINT_DATA(mDebugLog, " * PARAMETER: %s\n", qParam.mSymbolicName.c_str());

    getParameterStringInfo(qParam.mParam, ':', qParam.mParameterKey, qParam.mGeometryId, qParam.mParameterLevelId, qParam.mParameterLevel, qParam.mForecastType, qParam.mForecastNumber, qParam.mProducerName,
        qParam.mProducerId, qParam.mGenerationFlags, qParam.mAreaInterpolationMethod, qParam.mTimeInterpolationMethod, qParam.mLevelInterpolationMethod, qParam.mFlags);

    if (qParam.mParameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID)
    {
      Identification::NewbaseParameterDef paramDef;
      if (Identification::gridDef.getNewbaseParameterDefById(toUInt32(qParam.mParameterKey), paramDef))
      {
        PRINT_DATA(mDebugLog, "   + Newbase name: %s\n", paramDef.mParameterName.c_str());
        qParam.mParam = paramDef.mParameterName;
      }
    }

    // If the parameter is a symbolic name (defined in the configuration file) then
    // we should use its definition.

    std::string alias;
    if (getAlias(qParam.mParameterKey, alias))
    {
      PRINT_DATA(mDebugLog, "   + Alias: %s\n", alias.c_str());

      qParam.mParam = alias;

      // When an alias is replaced by its definition, it is possible that this definition
      // contains other aliases. That's why we need to repeat this "updateQueryParameters"
      // loop. The 'updateCounter' variable is used in order to ensure that the loop
      // will end if the aliases are referencing to each other (=> eternal loop).

      updateRequired = true;
    }

    // If the parameter contains a function then we should parse it and make sure that
    // we fetch the required parameter values for it.

    if (qParam.mFunction.empty())
    {
      parseFunction(query,qParam, qParam.mParam, qParam.mFunction, qParam.mFunctionParams, 0, additionalParameterList);
    }

    return updateRequired;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Update query parameters. */

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
        if (updateQueryParameter(query,*qParam,additionalParameterList))
          updateRequired = true;
      }

      // Adding parameters that are required by functions into the query.

      for (auto& p : additionalParameterList)
        query.mQueryParameterList.emplace_back(p);

      PRINT_DATA(mDebugLog, "\n");
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Query Server: Post process query. */

void ServiceImplementation::postProcessQuery(Query& query)
{
  FUNCTION_TRACE
  try
  {
    ContourAttributes ca = getContourAttributes(query.mAttributeList);

    for (auto& qParam : query.mQueryParameterList)
      contourParameterValues(qParam,ca);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Print the query object to the debug log (if enabled) with the title. */

void ServiceImplementation::printQueryObject(Query& query,const char *title)
{
  try
  {
    if (mDebugLog == nullptr ||  !mDebugLog->isEnabled())
      return;

    std::stringstream stream;
    query.print(stream, 0, 0);
    PRINT_DATA(mDebugLog, "\n%s\n", title);
    PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Execute the vector function of the Lua function type (8 or 9, other types do nothing). */

void ServiceImplementation::executeLuaVectorFunction(uint type,const std::string& luaFunction,const std::string& subfunction,uint columns,uint rows,std::vector<T::ParamValue_vec>& inputVectors,const std::vector<double>& parameters,T::ParamValue_vec& outputVector)
{
  try
  {
    switch (type)
    {
      case 8:
        mLuaFileCollection.executeFunctionCall8(luaFunction,subfunction.c_str(),columns,rows,inputVectors,parameters,outputVector);
        break;

      case 9:
        mLuaFileCollection.executeFunctionCall9(luaFunction,columns,rows,inputVectors,parameters,outputVector);
        break;
      default:
        break;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Execute the vector function (a built-in function or a Lua function). */

void ServiceImplementation::executeVectorFunction(const std::string& function,const std::string& subfunction,uint columns,uint rows,std::vector<T::ParamValue_vec>& inputVectors,const std::vector<double>& parameters,T::ParamValue_vec& outputVector)
{
  try
  {
    //UInt64 startTime = getTime();

    auto *functionPtr = mFunctionCollection.getFunction(function);
    if (functionPtr)
    {
      functionPtr->executeFunctionCall9(columns,rows,inputVectors,parameters,outputVector);
    }
    else
    {
      std::string luaFunction;
      uint type = mLuaFileCollection.getFunction(function, luaFunction);
      executeLuaVectorFunction(type,luaFunction,subfunction,columns,rows,inputVectors,parameters,outputVector);
    }
    //UInt64 endTime = getTime();
    //printf("TIME %f\n",(float)(endTime-startTime)/1000000.0);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Execute the vector function of the query parameter for the time step and add the result to the
           value list of the parameter. */

void ServiceImplementation::executeTimeStepVectorFunction(Query& query,QueryParameter& qParam,const std::string& function,const std::string& subfunction,uint columns,uint rows,int tCount,time_t forecastTime)
{
  try
  {
    std::string f = function.substr(0, 1);

    std::shared_ptr<ParameterValues> pValues = std::make_shared<ParameterValues>();
    pValues->mForecastTimeUTC = forecastTime;

    std::vector<T::ParamValue_vec> inputVectors;
    std::vector<double> parameters;
    FunctionSource source;

    collectFunctionInputVectors(query,qParam,tCount,inputVectors,parameters,source);

    if (f == "/"  &&  !inputVectors.empty())
    {
      pValues->mValueVector = inputVectors[0];
    }
    else if (f != "@"  &&  !inputVectors.empty())
    {
      executeVectorFunction(function,subfunction,columns,rows,inputVectors,parameters,pValues->mValueVector);
    }

    pValues->mProducerId = source.producerId;
    pValues->mGenerationId = source.generationId;
    pValues->mGeometryId = source.geometryId;
    pValues->mFlags = source.flags;
    qParam.mValueList.emplace_back(pValues);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Execute the vector function ("/function") of the query parameter over the row ranges of its
           value vectors and replace the value vectors with the results. */

void ServiceImplementation::executeRowRangeVectorFunction(Query& query,QueryParameter& qParam,uint columns,uint rows)
{
  try
  {
    std::vector<double> parameters;
    std::string function;
    std::string subfunction;
    splitSubfunction(qParam.mFunction.substr(1),function,subfunction);
    auto *functionPtr = mFunctionCollection.getFunction(function);
    uint type = 0;
    std::string luaFunction;
    if (!functionPtr)
      type = mLuaFileCollection.getFunction(function, luaFunction);

    int startIdx = 0;
    int endIdx = 0;
    getRowRangeFunctionParameters(qParam,startIdx,endIdx,parameters);

    int len = qParam.mValueList.size();

    std::vector<std::vector<T::ParamValue>> outputVectors;

    for (int r = 0; r<len; r++)
    {
      std::vector<T::ParamValue> outputVector;
      std::vector<std::vector<T::ParamValue>> inputVectors;

      qParam.getValueVectorsByRowRange(r+startIdx,r+endIdx,inputVectors);

      if (!inputVectors.empty())
      {
        //UInt64 startTime = getTime();
        if (functionPtr)
          functionPtr->executeFunctionCall9(columns,rows,inputVectors,parameters,outputVector);
        else
          executeLuaVectorFunction(type,luaFunction,subfunction,columns,rows,inputVectors,parameters,outputVector);
        //UInt64 endTime = getTime();
        //printf("TIME %f\n",(float)(endTime-startTime)/1000000.0);
      }

      outputVectors.emplace_back(outputVector);
    }

    for (int r = 0; r<len; r++)
    {
      (*qParam.mValueList[r]).mValueVector = outputVectors[r];
      if (query.mAggregationTimes.find((*qParam.mValueList[r]).mForecastTimeUTC) != query.mAggregationTimes.end())
        (*qParam.mValueList[r]).mFlags |= ParameterValues::Flags::InternalAggregationValue;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Execute query functions vector. */

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
      std::string function;
      std::string subfunction;
      splitSubfunction(qParam->mFunction,function,subfunction);
      //printf("FUNCTION [%s][%s][%s]\n",qParam->mFunction.c_str(),function.c_str(),subfunction.c_str());

      if (!function.empty())
      {
        int tCount = 0;
        for (auto tt = query.mForecastTimeList.begin(); tt != query.mForecastTimeList.end(); ++tt)
        {
          executeTimeStepVectorFunction(query,*qParam,function,subfunction,columns,rows,tCount,*tt);
          tCount++;
        }
      }

      if (!qParam->mFunction.empty()  &&  qParam->mFunction.substr(0,1) == "/")
        executeRowRangeVectorFunction(query,*qParam,columns,rows);
    }
    query.removeTemporaryParameters();

    //if (containsAggregation)
    {
      query.removeInternalAggregationValues();
      //query.removeAggregationValues();

      //query.removeAggregationTimes();
    }

    query.updateForecastTimeList();

    printQueryObject(query,"Query -object before post processing:");

    postProcessQuery(query);

    printQueryObject(query,"Query -object after post processing:");

  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Whether the parameter has data for the time step and its grid covers
           the point. A point outside the grid gets a missing value from the data server, just
           like a missing value inside the grid, so the coverage is checked from the geometry. */

namespace
{
bool hasDataAtPoint(const Query& query, const QueryParameter& qParam, uint timeIndex, uint valueIndex)
{
  try
  {
    if (timeIndex >= qParam.mValueList.size())
      return false;

    const auto& values = *qParam.mValueList[timeIndex];
    T::GridValue rec;
    if (!values.mValueList.getGridValueByIndex(valueIndex, rec))
      return false;

    // Area values (-1000,-1000) and other coordinate types are not checked
    if (query.mCoordinateType != T::CoordinateTypeValue::LATLON_COORDINATES || values.mGeometryId == 0 ||
        (rec.mX == -1000 && rec.mY == -1000))
      return true;

    double grid_i = 0;
    double grid_j = 0;
    return Identification::gridDef.getGridPointByGeometryIdAndLatLonCoordinates(values.mGeometryId, rec.mY, rec.mX, grid_i, grid_j);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}
}  // namespace



namespace
{

/*! \brief Input of the function of the time step collected over the values of the time step. */

struct FunctionInput
{
  std::vector<double> areaParameters;  // If the case of area request, this contain all grid point values.
  std::vector<double> extParameters;
  bool areaCnt = false;
  FunctionSource source;
  T::GridValue lastRec;
};



/*! \brief Get the value of the function parameter (of the parameter values for the time step) at the value index
           into the value (or into the value list for a value string). Updates the source information, the last
           record and the area flag of the input. */

void getFunctionParameterValue(QueryParameter& q,int tCount,uint v,double& a,std::vector<double>& valueList,FunctionInput& input)
{
  if ((std::size_t)tCount >= q.mValueList.size())
  {
    input.areaCnt = true;
    return;
  }

  if (input.source.producerId == 0 && q.mValueList[tCount]->mProducerId > 0)
  {
    input.source.producerId = q.mValueList[tCount]->mProducerId;
    input.source.generationId = q.mValueList[tCount]->mGenerationId;
    input.source.geometryId = q.mValueList[tCount]->mGeometryId;
    input.source.flags = q.mValueList[tCount]->mFlags;
  }

  T::GridValue rec;
  if (!q.mValueList[tCount]->mValueList.getGridValueByIndex(v,rec))
  {
    a = ParamValueMissing;
    return;
  }

  if (!rec.mValueString.empty())
  {
    splitString(rec.mValueString, ';', valueList);
  }
  else
  {
    a = rec.mValue;
  }

  if (rec.mX == -1000 && rec.mY == -1000)
  {
    input.areaCnt = true;
  }
  else
  {
    input.lastRec = rec;
  }
}



/*! \brief Add the function parameter value: to the parameters, and to the area parameters (for a query parameter)
           or to the external parameters (for a constant of the first value). */

void addFunctionParameterValue(double a,bool isQueryParameter,uint v,std::vector<double>& parameters,FunctionInput& input)
{
  parameters.emplace_back(a);
  if (isQueryParameter)
    input.areaParameters.emplace_back(a);
  else
  if (v == 0)
    input.extParameters.emplace_back(a);
}



/*! \brief Collect the values of the function parameters for the time step at the value index. VALID functions skip
           the parameters that have no data at the point. */

void collectFunctionValues(Query& query,QueryParameter& qParam,bool onlyAvailableParams,int tCount,uint v,std::vector<double>& parameters,std::vector<std::string>& strParameters,FunctionInput& input)
{
  for (auto it = qParam.mFunctionParams.begin(); it != qParam.mFunctionParams.end(); ++it)
  {
    double a = toDouble(it->second.c_str());
    QueryParameter* q = query.getQueryParameterPtr(it->first);
    if (onlyAvailableParams && q != nullptr && !hasDataAtPoint(query, *q, tCount, v))
      continue;

    strParameters.push_back(it->second);
    std::vector<double> valueList;
    if (q != nullptr)
      getFunctionParameterValue(*q,tCount,v,a,valueList,input);

    if (valueList.empty())
    {
      addFunctionParameterValue(a,q != nullptr,v,parameters,input);
    }
    else
    {
      for (auto aa = valueList.begin(); aa != valueList.end(); ++aa)
        addFunctionParameterValue(*aa,q != nullptr,v,parameters,input);
    }
  }
}

}  // namespace





/*! \brief Query Server: Execute the point function (a built-in function or a Lua function of the type 1, 2 or 5) with
           the parameters and add the result value at the location of the last record. */

void ServiceImplementation::executePointFunction(const Query& query,const std::string& function,std::vector<double>& parameters,const std::vector<std::string>& strParameters,const T::GridValue& lastRec,ParameterValues& values)
{
  try
  {
    auto *functionPtr = mFunctionCollection.getFunction(function);
    if (functionPtr)
    {
      double val = functionPtr->executeFunctionCall1(parameters);
      T::GridValue rec(lastRec.mX, lastRec.mY, val);
      values.mValueList.addGridValue(rec);
      return;
    }

    std::string luaFunction;
    uint type = mLuaFileCollection.getFunction(function, luaFunction);
    switch (type)
    {
      case 1:
      {
        double val = mLuaFileCollection.executeFunctionCall1(luaFunction, parameters);
        T::GridValue rec(lastRec.mX, lastRec.mY, val);
        values.mValueList.addGridValue(rec);
      }
      break;

      case 2:
      {
        if (strParameters.size() == 1)
        {
          double val = mLuaFileCollection.executeFunctionCall2(luaFunction, strParameters[0].c_str());
          T::GridValue rec(lastRec.mX, lastRec.mY, val);
          values.mValueList.addGridValue(rec);
        }
      }
      break;

      case 5:
      {
        std::string val = mLuaFileCollection.executeFunctionCall5(luaFunction, query.mLanguage, parameters);
        T::GridValue rec(lastRec.mX, lastRec.mY, val);
        values.mValueList.addGridValue(rec);
      }
      break;
      default:
        break;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Execute the area function (a built-in function or a Lua function of the type 1 or 5) with the
           external parameters followed by the area parameters, and add the result as an area value. */

void ServiceImplementation::executeAreaFunction(const Query& query,const std::string& func,const std::vector<double>& areaParameters,std::vector<double>& extParameters,ParameterValues& values)
{
  try
  {
    for (auto it = areaParameters.begin();it != areaParameters.end();++it)
      extParameters.push_back(*it);

    auto *functionPtr = mFunctionCollection.getFunction(func);
    if (functionPtr)
    {
      double val = functionPtr->executeFunctionCall1(extParameters);
      T::GridValue rec(-1000, -1000, val);
      values.mValueList.addGridValue(rec);
      return;
    }

    std::string luaFunction;
    uint type = mLuaFileCollection.getFunction(func, luaFunction);
    switch (type)
    {
      case 1:
      {
        double val = mLuaFileCollection.executeFunctionCall1(luaFunction, extParameters);
        T::GridValue rec(-1000, -1000, val);
        values.mValueList.addGridValue(rec);
      }
      break;

      case 5:
      {
        std::string val = mLuaFileCollection.executeFunctionCall5(luaFunction, query.mLanguage, extParameters);
        T::GridValue rec(-1000, -1000, val);
        values.mValueList.addGridValue(rec);
      }
      break;
      default:
        break;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Execute the function of the query parameter for the time step (for each value of the time step,
           or once for the area) and add the result to the value list of the parameter. */

void ServiceImplementation::executeTimeStepFunction(Query& query,QueryParameter& qParam,const std::string& function,uint valueCount,bool onlyAvailableParams,int tCount,time_t forecastTime)
{
  try
  {
    std::string f = function.substr(0, 1);

    std::shared_ptr<ParameterValues> pValues = std::make_shared<ParameterValues>();
    pValues->mForecastTimeUTC = forecastTime;

    FunctionInput input;

    for (uint v = 0; v < valueCount; v++)
    {
      std::vector<double> parameters;
      std::vector<std::string> strParameters;

      collectFunctionValues(query,qParam,onlyAvailableParams,tCount,v,parameters,strParameters,input);

      if (f == "/")
      {
        pValues->mFlags = input.source.flags;
        T::GridValue rec(input.lastRec.mX, input.lastRec.mY, parameters.empty() ? ParamValueMissing : parameters[0]);
        pValues->mValueList.addGridValue(rec);
      }
      else
      if (f != "@" && !input.areaCnt)
      {
        // Functions parameter list is a simple value list. If this is an area request
        // then each grid point is calculated separately.

        executePointFunction(query,function,parameters,strParameters,input.lastRec,*pValues);

        pValues->mProducerId = input.source.producerId;
        pValues->mGenerationId = input.source.generationId;
        pValues->mGeometryId = input.source.geometryId;
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

      executeAreaFunction(query,function.substr(1),input.areaParameters,input.extParameters,*pValues);

      pValues->mProducerId = input.source.producerId;
      pValues->mGenerationId = input.source.generationId;
      pValues->mGeometryId = input.source.geometryId;
    }

    if (input.areaCnt  &&  pValues->mValueList.getLength() > 1)
    {
      T::GridValue rec;
      pValues->mValueList.getGridValueByIndex(0,rec);
      pValues->mValueList.clear();
      rec.mX = -1000;
      rec.mY = -1000;
      pValues->mValueList.addGridValue(rec);
    }
    qParam.mValueList.emplace_back(pValues);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Execute the row range function ("/function") of the query parameter for each value record and
           replace the value lists with the results. */

void ServiceImplementation::executeRowRangeFunction(Query& query,QueryParameter& qParam)
{
  try
  {
    std::string function = qParam.mFunction.substr(1);
    auto *functionPtr = mFunctionCollection.getFunction(function);
    uint type = 0;
    std::string luaFunction;
    if (!functionPtr)
      type = mLuaFileCollection.getFunction(function, luaFunction);

    int startIdx = 0;
    int endIdx = 0;
    std::vector<T::ParamValue> parameters;
    getRowRangeFunctionParameters(qParam,startIdx,endIdx,parameters);


    int columns = 0;
    int rows = 0;

    std::vector<T::GridValueList> valueList;

    qParam.getValueListSize(columns,rows);

    for (int r = 0; r<rows; r++)
    {
      T::GridValueList list;
      for (int c=0; c<columns; c++)
      {

        T::GridValue *rec = qParam.getValueListRecord(c,r);
        if (rec)
        {
          std::vector<T::ParamValue> values = parameters;
          qParam.getValueListValuesByRowRange(c,r+startIdx,r+endIdx,values);
          executeRowRangeValueFunction(query,functionPtr,type,luaFunction,values,*rec,list);
        }
      }
      valueList.emplace_back(list);
    }

    for (int r = 0; r<rows; r++)
    {
      (*qParam.mValueList[r]).mValueList = valueList[r];
      if (query.mAggregationTimes.find((*qParam.mValueList[r]).mForecastTimeUTC) != query.mAggregationTimes.end())
        (*qParam.mValueList[r]).mFlags |= ParameterValues::Flags::InternalAggregationValue;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Execute the row range function (a built-in function or a Lua function of the type 1 or 5) with
           the values and add the result record (a copy of the record with the result value) to the list. */

void ServiceImplementation::executeRowRangeValueFunction(const Query& query,Functions::FunctionPtr functionPtr,uint type,const std::string& luaFunction,std::vector<T::ParamValue>& values,const T::GridValue& rec,T::GridValueList& list)
{
  try
  {
    if (functionPtr)
    {
      T::GridValue newRec(rec);
      newRec.mValue = functionPtr->executeFunctionCall1(values);
      list.addGridValue(newRec);
      return;
    }

    switch (type)
    {
      case 1:
      {
        T::GridValue newRec(rec);
        newRec.mValue = mLuaFileCollection.executeFunctionCall1(luaFunction, values);
        list.addGridValue(newRec);
      }
      break;

      case 5:
      {
        T::GridValue newRec(rec);
        newRec.mValueString = mLuaFileCollection.executeFunctionCall5(luaFunction,query.mLanguage,values);
        list.addGridValue(newRec);
      }
      break;
      default:
        break;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Execute query functions. */

void ServiceImplementation::executeQueryFunctions(Query& query)
{
  FUNCTION_TRACE
  try
  {
    printQueryObject(query,"Query -object before function execution:");


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
        // VALID selects the first parameter that has data for the time and covers the point, and
        // returns its value as such. A missing value of the preferred parameter (say no
        // precipitation in a nowcast) must not be replaced by the value of a later parameter (say
        // an older forecast), so only the parameters without data or coverage are left out.
        const bool onlyAvailableParams = (strcasecmp(function.c_str(), "VALID") == 0);

        int tCount = 0;
        for (auto tt = query.mForecastTimeList.begin(); tt != query.mForecastTimeList.end(); ++tt)
        {
          executeTimeStepFunction(query,*qParam,function,valueCount,onlyAvailableParams,tCount,*tt);
          tCount++;
        }
      }
      if (!qParam->mFunction.empty()  &&  qParam->mFunction.substr(0,1) == "/")
        executeRowRangeFunction(query,*qParam);
    }

    query.removeTemporaryParameters();

    //if (containsAggregation)
    {
      //query.removeAggregationValues();
      query.removeInternalAggregationValues();
      //query.removeAggregationTimes();
    }

    query.updateForecastTimeList();

    printQueryObject(query,"Query -object after function execution:");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the geometries that support the coordinates (or the geometries) of the query. */

void ServiceImplementation::getQueryGeometryIdList(Query& query,Producer_vec& producers,std::set<T::GeometryId>& geometryIdList)
{
  try
  {
    if (!query.mGeometryIdList.empty())
    {
      // The query defines the geometries that we need to use.
      geometryIdList = query.mGeometryIdList;
      return;
    }

    if (query.mFlags & Query::Flags::GeometryHitNotRequired)
    {
      // The query defines that the requested coordinates does not need to
      // hit the area. So, all geometries are acceptable.
      Identification::gridDef.getGeometryIdList(geometryIdList);
      return;
    }

    const char *geometryIdStr = query.mAttributeList.getAttributeValue("grid.geometryId");
    if (geometryIdStr != nullptr)
    {
      // Selecting geometries which cover the requested geometry.
      getGeometryIdListByGeometryId(toInt32(geometryIdStr),geometryIdList);
      return;
    }

    const char *gridUrnStr = query.mAttributeList.getAttributeValue("grid.urn");
    if (gridUrnStr != nullptr)
    {
      // All geometries are allowed.
      Identification::gridDef.getGeometryIdList(geometryIdList);
      return;
    }

    // Selecting valid geometries according to given coordiantes.
    getGeometryIdListByCoordinates(producers,query.mAreaCoordinates, geometryIdList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Parse the parameter string of the query parameter into the parameter request. */

void ServiceImplementation::parseParameterRequest(const Query& query,QueryParameter& qParam,QueryFetchState& state,ParameterRequest& req)
{
  try
  {
    req.paramName = qParam.mParam;
    req.producerName = qParam.mProducerName;
    req.producerId = qParam.mProducerId;
    req.geometryId = qParam.mGeometryId;
    req.generationFlags = qParam.mGenerationFlags;

    if (qParam.mFlags & QueryParameter::Flags::InternalAggregationParameter)
      state.containsInternalAggregation = true;

    getParameterStringInfo(qParam.mParam, ':', req.paramName, req.geometryId, qParam.mParameterLevelId,
        qParam.mParameterLevel, qParam.mForecastType, qParam.mForecastNumber, req.producerName,req.producerId,
        req.generationFlags,qParam.mAreaInterpolationMethod,qParam.mTimeInterpolationMethod, qParam.mLevelInterpolationMethod, qParam.mFlags);

    if (req.generationFlags == 0)
      req.generationFlags = query.mGenerationFlags;

    if (req.paramName.c_str()[0] == '$')
      req.paramName.erase(0,1);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Calculate the parameter hash and require the alternative parameter if the geometry of the
           parameter does not cover the query area. */

void ServiceImplementation::checkAlternativeRequest(Query& query,const QueryParameter& qParam,QueryFetchState& state,ParameterRequest& req)
{
  try
  {
    req.paramHash = 0;
    Fmi::hash_merge(req.paramHash,req.paramName);

    if (qParam.mAlternativeParamId != 0  &&  req.geometryId > 0 && !isGeometryValid(req.geometryId,query.mAreaCoordinates))
    {
      req.useAlternative = true;
      state.alternativeRequired.insert(qParam.mAlternativeParamId);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Fetch the time range values of the data parameter. Fetch errors are printed, not thrown. */

void ServiceImplementation::fetchTimeRangeParameter(Query& query,QueryParameter& qParam,QueryFetchState& state,std::unordered_map<std::string,T::ProducerId>& parameterProducers)
{
  try
  {
    ParameterRequest req;
    parseParameterRequest(query,qParam,state,req);

    try
    {
      checkAlternativeRequest(query,qParam,state,req);

      if (qParam.mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN  &&  !req.useAlternative)
        fetchTimeRangeValues(query,qParam,state,parameterProducers,req);
    }
    catch (...)
    {
      Fmi::Exception exception(BCP, "Operation failed!", nullptr);
      exception.printError();
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the producers and the geometries for the time range values of the parameter. */

void ServiceImplementation::getTimeRangeProducersAndGeometries(const QueryParameter& qParam,const QueryFetchState& state,const ParameterRequest& req,Producer_vec& tmpProducers,std::set<T::GeometryId>& geomIdList)
{
  try
  {
    if (req.producerName > " ")
    {
      if (req.geometryId > 0)
        tmpProducers.emplace_back(req.producerName, req.geometryId);
      else
        getProducers(req.producerName,tmpProducers);
    }
    else
      tmpProducers = state.producers;

    if (req.geometryId > 0)
      geomIdList.insert(req.geometryId);
    else if (state.globalGeometryId > 0  &&  req.producerName.empty()  &&  qParam.mProducerId == 0)
      geomIdList.insert(state.globalGeometryId);
    else
      geomIdList = state.geometryIdList;

    if (qParam.mType != QueryParameter::Type::PointValues &&  geomIdList.empty())
    {
      for (auto& tmpProducer : tmpProducers)
      {
        geomIdList.insert(tmpProducer.second);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Fetch the time range values of the parameter and update the query state accordingly. */

void ServiceImplementation::fetchTimeRangeValues(Query& query,QueryParameter& qParam,QueryFetchState& state,std::unordered_map<std::string,T::ProducerId>& parameterProducers,ParameterRequest& req)
{
  try
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

    if (req.producerId == 0)
    {
      auto it = parameterProducers.find(req.paramName + ":" + req.producerName);
      if (it != parameterProducers.end())
      {
        req.producerId = it->second;
      }
    }

    Producer_vec tmpProducers;
    std::set<T::GeometryId> geomIdList;
    getTimeRangeProducersAndGeometries(qParam,state,req,tmpProducers,geomIdList);

    std::string producerStr;

    getGridValues(query,qParam,tmpProducers, geomIdList, req.producerId, state.analysisTime, req.generationFlags, req.paramName, req.paramHash, state.queryFlags, startTime, endTime,producerStr,0,0);

    //if (!qParam->mValueList.empty() || ((parameterFlags & QueryParameter::Flags::NoReturnValues) != 0  &&  (qParam->mValueList[0].mFlags & ParameterValues::Flags::DataAvailable) != 0))
    if (qParam.mValueList.empty())
    {
      state.alternativeRequired.insert(qParam.mAlternativeParamId);
      return;
    }

    ParameterValues& values = *qParam.mValueList[0];

    if (req.producerId == 0 && values.mProducerId != 0)
    {
      parameterProducers.insert(std::pair<std::string, T::ProducerId>(req.paramName + ":" + req.producerName, values.mProducerId));
    }

    // If there are no geometry defined, then we should not change the first geometry we'll get

    if (req.geometryId <= 0  &&  values.mGeometryId > 0)
      state.globalGeometryId = values.mGeometryId;

    updateSameAnalysisTime(query,values,state);

    uint len = values.mValueList.getLength();
    addValueCoordinates(values,state.coordinates);

    if (len == 0  || ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0  &&  (values.mFlags & ParameterValues::Flags::DataAvailable) == 0))
    {
      state.alternativeRequired.insert(qParam.mAlternativeParamId);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Make sure that the parameter contains the forecast time of the time range query: mark the values
           outside the query time range as aggregation values, and add empty (additional) values for a missing time. */

void ServiceImplementation::addMissingTimeRangeValues(const Query& query,QueryParameter& qParam,time_t tt,size_t timeCnt,const std::set<time_t>& originalTimeList,const QueryFetchState& state)
{
  try
  {
    bool found = false;
    uint cnt = 0;

    if (qParam.mValueList.size() < timeCnt)
      qParam.mValueList.reserve(timeCnt);

    for (auto it = qParam.mValueList.begin(); it != qParam.mValueList.end() && !found; ++it)
    {
      if (isOutsideQueryTimeRange(query,(*it)->mForecastTimeUTC))
        markAggregationValue(**it,state.containsInternalAggregation);

      if ((*it)->mForecastTimeUTC < tt)
        cnt++;
      else
      if ((*it)->mForecastTimeUTC == tt)
      {
        found = true;

        if ((*it)->mValueList.getLength() == 0)
        {
          getAdditionalValues(qParam.mSymbolicName,query.mCoordinateType,state.coordinates,*(*it));
        }
      }
    }

    if (found)
      return;

    // The forecast time was not found from the current parameter. Adding the forecast time
    // with an empty value list.

    std::shared_ptr<ParameterValues> pValues = std::make_shared<ParameterValues>();
    pValues->mForecastTimeUTC = tt;

    if (originalTimeList.find(tt) != originalTimeList.end())
    {
      // std::cout << "ADDITIONAL " << qParam->mParam  << " " << utcTimeFromTimeT(*tt) << "\n";
      if (qParam.mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN)
        pValues->mFlags = pValues->mFlags | QueryServer::ParameterValues::Flags::AdditionalValue;
    }
    else
    {
      // std::cout << "AGGREGATION " << qParam->mParam  << " " << utcTimeFromTimeT(*tt) << "\n";
      if (qParam.mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN)
        pValues->mFlags = pValues->mFlags | QueryServer::ParameterValues::Flags::AggregationValue;

      if (state.containsInternalAggregation)
        pValues->mFlags = pValues->mFlags | QueryServer::ParameterValues::Flags::InternalAggregationValue;
    }

    getAdditionalValues(qParam.mSymbolicName,query.mCoordinateType,state.coordinates,*pValues);

    if ((query.mFlags & Query::Flags::StartTimeFromData) == 0 && (tt < query.mStartTime || tt > query.mEndTime))
      pValues->mFlags = pValues->mFlags | QueryServer::ParameterValues::Flags::AggregationValue;

    qParam.mValueList.insert(qParam.mValueList.begin() + cnt, pValues);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Execute time range query. */

int ServiceImplementation::executeTimeRangeQuery(Query& query)
{
  FUNCTION_TRACE
  try
  {
    QueryFetchState state;
    getProducers(query, state.producers);
    state.analysisTime = query.mAnalysisTime;
    state.queryFlags = query.mFlags;

    if (state.producers.empty())
      return Result::NO_PRODUCERS_FOUND;

    // Getting geometries that support support the given coordinates.

    getQueryGeometryIdList(query,state.producers,state.geometryIdList);

    std::unordered_map<std::string,T::ProducerId> parameterProducers;

    // Parsing parameters and functions in the query.

    updateQueryParameters(query);

    printQueryObject(query,"Query -object after parameter update:");

    // Fetching parameter data according to the given time range and the coordinate list. Notice
    // that the coordinate list can be used in two ways. It can 1) contain coordinate points
    // where the data will be fetched or 2) it can define an area (= polygon) where the grid
    // points are fetched. The area can be defined by using single or multiple polygons.
    // That's why the coordinates are defined as a vector of coordinate vectors.

    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
    {
      checkMaxProcessingTime(query,"maxProcessingTimeInSeconds");

      // Checking that the parameter is "a data parameter" - not a function.

      if (isRequiredDataParameter(*qParam,state.alternativeRequired))
        fetchTimeRangeParameter(query,*qParam,state,parameterProducers);
    }

    // Finding out which forecast time are found from the forecast data. The point is that different
    // parameters might contain different forecast times, and we want a list of all forecast times.

    std::set<time_t> originalTimeList;
    std::set<time_t> timeList;
    collectTimeRangeForecastTimes(query,originalTimeList,timeList);

    // Going through all the found forecast times and making sure that each parameter contains the
    // current forecast time. If not, then the forecast time is added to the parameters's forecast
    // time list, but the actual value list of the current forecast time will be empty.

    size_t timeCnt = timeList.size();
    for (long tt : timeList)
    {
      query.mForecastTimeList.insert(tt);

      for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
      {
        if (qParam->mFunction.empty())
          addMissingTimeRangeValues(query,*qParam,tt,timeCnt,originalTimeList,state);
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





/*! \brief Query Server: Print the query parameter to the debug log (if enabled). */

void ServiceImplementation::printQueryParameter(QueryParameter& qParam)
{
  try
  {
    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      qParam.print(stream, 5, 0);
      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Fetch the time step values of the data parameter (in reverse time order). Fetch errors are
           printed, not thrown. */

void ServiceImplementation::fetchTimeStepParameter(Query& query,QueryParameter& qParam,QueryFetchState& state)
{
  try
  {
    T::ForecastType forecastType = qParam.mForecastType;
    T::ForecastNumber forecastNumber = qParam.mForecastNumber;

    ParameterRequest req;
    parseParameterRequest(query,qParam,state,req);
    checkAlternativeRequest(query,qParam,state,req);

    std::set<time_t> *forecastTimeList = &query.mForecastTimeList;
    std::set<time_t> forecastTimeList2;

    if (qParam.mTimestepsBefore > 0 || qParam.mTimestepsAfter > 0)
    {
      forecastTimeList = &forecastTimeList2;
      addAggregationTimeSteps(query,qParam,forecastTimeList2);
    }


    printQueryParameter(qParam);


    // Processing time steps in reverse order. The idea is that we want to
    // find a generation that contains the newest timestep if the generation
    // is not defined. It is possible that the newest generation do not contain
    // the requested timestep when the generation is still under construction.

    UInt64 gflags = req.generationFlags;

    int ftLen = forecastTimeList->size();
    qParam.mValueList.reserve(ftLen);

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
        if (qParam.mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN  &&  !req.useAlternative)
          fetchTimeStepValues(query,qParam,state,req,*fTime,gflags,forecastType,forecastNumber,tmpProducers,tmpGeomIdList,*valueList);
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
      qParam.mValueList.emplace_back(*aa);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Select the producers and the geometries for the time step values of the parameter. The
           temporary producer and geometry lists are kept over the time steps of the parameter. */

void ServiceImplementation::selectTimeStepProducersAndGeometries(const QueryParameter& qParam,QueryFetchState& state,const ParameterRequest& req,Producer_vec& tmpProducers,std::set<T::GeometryId>& tmpGeomIdList,Producer_vec*& producersPtr,std::set<T::GeometryId>*& geomIdListPtr)
{
  try
  {
    producersPtr = &state.producers;
    geomIdListPtr = &state.geometryIdList;

    if (req.producerName > " "  &&  tmpProducers.empty())
    {
      if (req.geometryId > 0)
        tmpProducers.emplace_back(req.producerName, req.geometryId);
      else
        getProducers(req.producerName,tmpProducers);
    }

    if (!tmpProducers.empty())
      producersPtr = &tmpProducers;

    if (req.geometryId > 0)
    {
      if (tmpGeomIdList.empty())
        tmpGeomIdList.insert(req.geometryId);
    }
    else if (state.globalGeometryId > 0  &&  req.producerName.empty()  &&  qParam.mProducerId == 0)
      tmpGeomIdList.insert(state.globalGeometryId);

    if (qParam.mType != QueryParameter::Type::PointValues &&  tmpGeomIdList.empty())
    {
      for (auto pp = tmpProducers.begin(); pp != tmpProducers.end(); ++pp)
      {
        tmpGeomIdList.insert(pp->second);
      }
    }

    if (!tmpGeomIdList.empty())
      geomIdListPtr = &tmpGeomIdList;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Fetch the time step values from the concatenated producers of the producer while no values are
           found. Returns true if the values were fetched from a concatenated producer. */

bool ServiceImplementation::fetchConcatenatedProducerValues(Query& query,QueryParameter& qParam,QueryFetchState& state,const ParameterRequest& req,time_t forecastTime,UInt64 gflags,std::string& producerStr,std::set<T::GeometryId>& tmpGeomIdList,ParameterValues& valueList)
{
  try
  {
    bool concat = false;
    std::string prodName = producerStr;

    while (!prodName.empty() &&  hasNoValues(valueList))
    {
      //printf("CONCAT SEARCH %s\n",prodName.c_str());
      auto vec = mProducerConcatMap.find(prodName);
      if (vec == mProducerConcatMap.end()  ||  vec->second.empty())
      {
        prodName = "";
        continue;
      }

      int gid = 0;
      const char *gidStr = strstr(prodName.c_str(),":");
      if (gidStr)
        gid = toInt32(gidStr+1);

      tmpGeomIdList.clear();
      prodName = vec->second[0].first + ":" + std::to_string(vec->second[0].second);

      bool prodOk = selectConcatGeometries(qParam,vec->second,gid,tmpGeomIdList);

      if (prodOk)
      {
        std::string maxAnalysisTime;
        getGridValues(query,qParam,vec->second, tmpGeomIdList, req.producerId, state.analysisTime, maxAnalysisTime, gflags, req.paramName, req.paramHash,
            state.queryFlags, forecastTime, false, valueList, producerStr);

        concat = true;
      }
    }
    tmpGeomIdList.clear();
    return concat;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Fetch the values of the parameter for the time step and update the query state accordingly. */

void ServiceImplementation::fetchTimeStepValues(Query& query,QueryParameter& qParam,QueryFetchState& state,ParameterRequest& req,time_t forecastTime,UInt64& gflags,T::ForecastType& forecastType,T::ForecastNumber& forecastNumber,Producer_vec& tmpProducers,std::set<T::GeometryId>& tmpGeomIdList,ParameterValues& valueList)
{
  try
  {
    Producer_vec *producersPtr = nullptr;
    std::set<T::GeometryId> *geomIdListPtr = nullptr;
    selectTimeStepProducersAndGeometries(qParam,state,req,tmpProducers,tmpGeomIdList,producersPtr,geomIdListPtr);

    std::string producerStr;
    std::string maxAnalysisTime;

    //printf("SEARCH %s %u\n",producerStr.c_str(),producerId);

    getGridValues(query,qParam,*producersPtr, *geomIdListPtr, req.producerId, state.analysisTime, maxAnalysisTime, gflags, req.paramName, req.paramHash,
        state.queryFlags, forecastTime, false, valueList, producerStr);

    //printf("SEARCH RESULT %s %u %u %u\n",producerStr.c_str(),producerId,valueList->mValueList.getLength(),valueList->mFlags);
    bool concat = false;
    if (req.producerId == 0 && hasNoValues(valueList))
      concat = fetchConcatenatedProducerValues(query,qParam,state,req,forecastTime,gflags,producerStr,tmpGeomIdList,valueList);

    if (valueList.mValueList.getLength() == 0  || ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0  &&  (valueList.mFlags & ParameterValues::Flags::DataAvailable) == 0))
    {
      state.alternativeRequired.insert(qParam.mAlternativeParamId);
    }

    //printf("SEARCH END %s %u %u %d\n",producerStr.c_str(),producerId,valueList->mValueList.getLength(),(int)concat);

    if (valueList.mValueList.getLength() > 0  || ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) != 0  &&  (valueList.mFlags & ParameterValues::Flags::DataAvailable) != 0))
      updateTimeStepFetchState(query,qParam,state,req,concat,gflags,forecastType,forecastNumber,valueList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Update the query state and the parameter selections (producer, geometry, analysis time,
           generations, forecast number) by the values found for the time step. */

void ServiceImplementation::updateTimeStepFetchState(const Query& query,const QueryParameter& qParam,QueryFetchState& state,ParameterRequest& req,bool concat,UInt64& gflags,T::ForecastType& forecastType,T::ForecastNumber& forecastNumber,ParameterValues& valueList)
{
  try
  {
    if (!concat)
    {

      // We have got some values. We should not change the producer anymore.
      if (req.producerId == 0)
        req.producerId = valueList.mProducerId;

      // If there are no geometry defined, then we should not change the first geometry we'll get

      if (req.geometryId <= 0  &&  valueList.mGeometryId > 0)
        state.globalGeometryId = valueList.mGeometryId;

      updateSameAnalysisTime(query,valueList,state);

      updateTimeStepGenerationFlags(query,qParam,req.generationFlags,valueList,gflags);
    }

    // We should not change the forecast number.
    if (forecastType < 0  &&  forecastNumber < 0)
    {
      forecastType = valueList.mForecastType;
      forecastNumber = valueList.mForecastNumber;
    }

    addValueCoordinates(valueList,state.coordinates);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Make sure that the parameter contains the forecast time of the time step query: add empty
           (additional) values for a missing time. */

void ServiceImplementation::addMissingTimeStepValues(const Query& query,QueryParameter& qParam,time_t tt,const std::unordered_set<time_t>& additionalTimeList,const QueryFetchState& state)
{
  try
  {
    bool found = false;
    uint cnt = 0;
    for (auto it = qParam.mValueList.begin(); it != qParam.mValueList.end() && !found; ++it)
    {
      if ((*it)->mForecastTimeUTC < tt)
        cnt++;
      else
      if ((*it)->mForecastTimeUTC == tt)
      {
        found = true;
        if ((*it)->mValueList.getLength() == 0)
        {
          getAdditionalValues(qParam.mSymbolicName,query.mCoordinateType,state.coordinates,*(*it));
        }
      }
    }

    if (found)
      return;

    // The forecast time was not found from the current parameter. Adding the forecast time
    // with an empty value list.

    std::shared_ptr<ParameterValues> pValues = std::make_shared<ParameterValues>();
    pValues->mForecastTimeUTC = tt;
    //pValues->mForecastTime = utcTimeFromTimeT(tt);

    if (qParam.mParameterKeyType != T::ParamKeyTypeValue::BUILD_IN)
    {
      pValues->mFlags = pValues->mFlags | QueryServer::ParameterValues::Flags::AdditionalValue;
    }

    getAdditionalValues(qParam.mSymbolicName,query.mCoordinateType,state.coordinates,*pValues);

    if (additionalTimeList.find(tt) != additionalTimeList.end())
      markAggregationValue(*pValues,state.containsInternalAggregation);

    qParam.mValueList.insert(qParam.mValueList.begin() + cnt, pValues);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Execute time step query. */

int ServiceImplementation::executeTimeStepQuery(Query& query)
{
  FUNCTION_TRACE
  try
  {
    QueryFetchState state;
    getProducers(query, state.producers);
    state.analysisTime = query.mAnalysisTime;
    state.queryFlags = query.mFlags;

    if (state.producers.empty())
      return Result::NO_PRODUCERS_FOUND;

    getQueryGeometryIdList(query,state.producers,state.geometryIdList);


    // Parsing parameters and functions in the query.

    updateQueryParameters(query);

    printQueryObject(query,"Query -object after parameter update:");

    // Fetching parameter data according to the given timesteps and the coordinate list. Notice
    // that the coordinate list can be used in two ways. It can 1) contain coordinate points
    // where the data will be fetched or 2) it can define an area (= polygon) where the grid
    // points are fetched. The area can be defined by using single or multiple polygons.
    // That's why the coordinates are defined as a vector of coordinate vectors.


    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
    {
      checkMaxProcessingTime(query,"maxProcessingTimeInSeconds");

      if (isRequiredDataParameter(*qParam,state.alternativeRequired))
        fetchTimeStepParameter(query,*qParam,state);
    }


    // Finding out which forecast time are found from the forecast data. The point is that different
    // parameters might contain different forecast times, and we want a list of all forecast times.

    std::set<time_t> timeList;
    std::unordered_set<time_t> additionalTimeList;
    collectTimeStepForecastTimes(query,state.containsInternalAggregation,timeList,additionalTimeList);

    // Going through all the found forecast times and making sure that each parameter contains the
    // current forecast time. If not, then the forecast time is added to the parameters's forecast
    // time list, but the actual value list of the current forecast time will be empty.

    query.mForecastTimeList.clear();

    for (const auto &tt : timeList)
    {
      query.mForecastTimeList.insert(tt);

      for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
      {
        if (qParam->mFunction.empty())
          addMissingTimeStepValues(query,*qParam,tt,additionalTimeList,state);
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





/*! \brief Query Server: Protected hook: get values by grid point. */

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





/*! \brief Query Server: Conversion function. */

bool ServiceImplementation::conversionFunction(const std::string& conversionFunction, std::string& function, std::vector<std::string>& functionParams)
{
  try
  {
    if (!conversionFunction.empty())
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





/*! \brief Query Server: Set the "$" parameters of the conversion to the value and the "$name" parameters to the additional value of the time and coordinate. */

void ServiceImplementation::setConversionParameters(const std::vector<std::string>& functionParams,T::ParamValue value,time_t forecastTime,const T::Coordinate& coordinate,std::vector<double>& parameters)
{
  try
  {
    int fps = functionParams.size();
    for (int p = 0; p<fps; p++)
    {
      const char *f = functionParams[p].c_str();

      if (f[0] == '$')
      {
        if (f[1] == '\0')
        {
          parameters[p] = value;
        }
        else
        {
          T::ParamValue val = getAdditionalValue(f+1,forecastTime,coordinate.x(), coordinate.y());
          parameters[p] = val;
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Query Server: Execute conversion. */

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

    auto *functionPtr = mFunctionCollection.getFunction(function);

    int size = valueList.size();
    newValueList.reserve(size);
    std::vector<double> parameters;
    int fps = functionParams.size();
    parameters.reserve(fps);
    for (int p = 0; p<fps; p++)
      parameters.emplace_back(toDouble(functionParams[p].c_str()));

    for (int t=0; t<size; t++)
    {
      if (valueList[t] == ParamValueMissing)
      {
        newValueList.emplace_back(ParamValueMissing);
      }
      else
      {
        setConversionParameters(functionParams,valueList[t],forecastTime,coordinates[t],parameters);

        T::ParamValue newValue = (functionPtr ? functionPtr->executeFunctionCall1(parameters) : mLuaFileCollection.executeFunctionCall1(function, parameters));
        newValueList.emplace_back(newValue);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






/*! \brief Query Server: Execute conversion. */

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

    auto *functionPtr = mFunctionCollection.getFunction(function);

    int size = valueList.size();
    newValueList.reserve(size);
    std::vector<double> parameters;
    int fps = functionParams.size();
    parameters.reserve(fps);
    for (int p = 0; p<fps; p++)
      parameters.emplace_back(toDouble(functionParams[p].c_str()));

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

        T::ParamValue newValue = (functionPtr ? functionPtr->executeFunctionCall1(parameters) : mLuaFileCollection.executeFunctionCall1(function, parameters));
        newValueList.emplace_back(newValue);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Execute conversion. */

void ServiceImplementation::executeConversion(const std::string& function, const std::vector<std::string>& functionParams,time_t forecastTime,const T::GridValueList& valueList)
{
  try
  {
    // ******************** TODO: Make this conversion better ******************
    Fmi::LocalDateTime utcTime(toTimeStamp(utcTimeFromTimeT(forecastTime)), nullptr);
    auto *functionPtr = mFunctionCollection.getFunction(function);

    uint vLen = valueList.getLength();
    for (uint i = 0; i < vLen; i++)
    {
      auto *gv = valueList.getGridValuePtrByIndex(i);
      if (gv != nullptr  &&  gv->mValue != ParamValueMissing)
      {
        std::vector<double> parameters;
        parameters.reserve(functionParams.size());
        for (const auto& functionParam : functionParams)
        {
          std::string f = functionParam.substr(0, 1);
          std::string p = functionParam.substr(1);

          if (functionParam == "$")
            parameters.emplace_back(gv->mValue);
          else
          if (f == "$"  &&  !p.empty())
          {
            T::ParamValue val = getAdditionalValue(p,forecastTime,gv->mX, gv->mY);
            parameters.emplace_back(val);
          }
          else
            parameters.emplace_back(toDouble(functionParam.c_str()));
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





/*! \brief Query Server: Register the content caches of the current thread and request their clearing when their clear intervals have passed. */

void ServiceImplementation::checkThreadCacheState()
{
  try
  {
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
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Find the content cache entry of the hash or of the level independent hash3 (then the hash is set to hash3), or return a new entry. */

ContentCacheEntry_sptr ServiceImplementation::findContentCacheEntry(std::size_t& hash,std::size_t hash3,bool& newEntry)
{
  try
  {
    ContentCacheEntry_sptr entry;

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

        entry = std::make_shared<ContentCacheEntry>();
        newEntry = true;
      }
    }

    return entry;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Fetch the contents of the parameter of the generation into the content cache entry (on any level if there are none on the level, then the hash is set to hash3). */

void ServiceImplementation::updateContentCacheEntry(ContentCacheEntry& entry,bool newEntry,T::SessionId sessionId,UInt64 producerHash,T::GenerationId generationId,T::ParamKeyType parameterKeyType,const std::string& parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::size_t& hash,std::size_t hash3)
{
  try
  {
    // Track the old record count so we can adjust the counter for in-place updates
    std::size_t oldLen = newEntry ? 0 : std::max(entry.contentInfoList.getLength(), (uint)1);

    time_t startTime = 0;
    time_t endTime = 0xFFFFFFFF;

    mContentServerPtr->getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,level,level,forecastType,forecastNumber,geometryId,startTime,endTime,0,entry.contentInfoList);
    if (entry.contentInfoList.getLength() == 0)
    {
      // It seems that the content information on the required level is not available. Let's ignore
      // the level requirement.

      mContentServerPtr->getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,0,1000000000,forecastType,forecastNumber,geometryId,startTime,endTime,0,entry.contentInfoList);
      if (entry.contentInfoList.getLength() == 0)
        hash = hash3;
    }

    entry.producerHash = producerHash;
    entry.generationId = generationId;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        entry.contentInfoList.setComparisonMethod(T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time);
        break;
      default:
        break;
    }

    // Adjust the records counter for in-place updates (when the entry already existed)
    if (!newEntry && oldLen > 0)
    {
      std::size_t newLen = std::max(entry.contentInfoList.getLength(), (uint)1);
      if (newLen > oldLen)
      {
        mContentCache_records += (newLen - oldLen);
        mContentCache_size += (newLen - oldLen);
      }
      else if (oldLen > newLen)
      {
        mContentCache_records -= (oldLen - newLen);
        mContentCache_size -= (oldLen - newLen);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Search the contents of the forecast time and level from the content list. */

void ServiceImplementation::searchContentList(T::ContentInfoList& contentList,T::ProducerId producerId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,const std::string& parameterKey,uint fmiId,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,T::ContentInfoList& cList)
{
  try
  {
    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
      {
        if (fmiId == 0)
          fmiId = toUInt32(parameterKey);

        contentList.getContentInfoListByFmiParameterIdAndGenerationId2(producerId,generationId,fmiId,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,cList);
        if (cList.getLength() == 0)
          contentList.getContentInfoListByFmiParameterIdAndGenerationId(producerId,generationId,fmiId,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,cList);
      }
      break;

      case T::ParamKeyTypeValue::FMI_NAME:
      {
        contentList.getContentInfoListByFmiParameterNameAndGenerationId2(producerId,generationId,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,cList);
        if (cList.getLength() == 0)
          contentList.getContentInfoListByFmiParameterNameAndGenerationId(producerId,generationId,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,cList);
      }
      break;
      default:
        break;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Store the search result into the content search cache (clearing the cache first when required). */

void ServiceImplementation::storeContentSearchResult(std::size_t hash2,const std::shared_ptr<T::ContentInfoList>& cList,UInt64 producerHash,T::GenerationId generationId)
{
  try
  {
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
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Store the new entry into the content cache (clearing the cache first when required). */

void ServiceImplementation::storeContentCacheEntry(std::size_t hash,const ContentCacheEntry_sptr& entry)
{
  try
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
      PRINT_DATA(mDebugLog,"%s:%d:%s: The code should never reach this point!\n",BCP);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get content list by parameter generation id and forecast time. */

int ServiceImplementation::getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,T::ProducerId producerId,UInt64 producerHash,T::GenerationId generationId,T::ParamKeyType parameterKeyType,const std::string& parameterKey,std::size_t parameterKeyHash,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,std::shared_ptr<T::ContentInfoList>& contentInfoList)
{
  try
  {
    uint fmiId = 0;
    if (parameterKeyType == T::ParamKeyTypeValue::FMI_NAME  &&  strncasecmp(parameterKey.c_str(),"FMI-",4) == 0)
    {
      parameterKeyType = T::ParamKeyTypeValue::FMI_ID;
      fmiId = toUInt32(parameterKey.c_str()+4);
    }

    checkThreadCacheState();

    std::size_t hash = 0;
    Fmi::hash_merge(hash,producerId);
    Fmi::hash_merge(hash,generationId);
//    Fmi::hash_merge(hash,parameterKeyType);
    //Fmi::hash_merge(hash,parameterKey);
    Fmi::hash_merge(hash,parameterKeyHash);
    Fmi::hash_merge(hash,parameterLevelId);
    Fmi::hash_merge(hash,forecastType);
    Fmi::hash_merge(hash,forecastNumber);
    Fmi::hash_merge(hash,geometryId);
    std::size_t hash3 = hash;
    Fmi::hash_merge(hash,level);
    Fmi::hash_merge(hash3,999999999);

    std::size_t hash2 = hash;
    Fmi::hash_merge(hash2,forecastTime);
    if (producerHash == 0)
    {
      //producerHash = getProducerHash(producerId);
      PRINT_DATA(mDebugLog,"%s:%d:%s: The producer hash should be never zero!\n",BCP);
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

    bool newEntry = false;
    ContentCacheEntry_sptr entry = findContentCacheEntry(hash,hash3,newEntry);


    if (newEntry || entry->producerHash != producerHash)
    {
      // The content entry in the cache seems to be old or missing. That's why we should update this entry.
      updateContentCacheEntry(*entry,newEntry,sessionId,producerHash,generationId,parameterKeyType,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,hash,hash3);
    }


    // ### Now we should have the required content information ready for the search.

    std::shared_ptr<T::ContentInfoList> cList = std::make_shared<T::ContentInfoList>();
    searchContentList(entry->contentInfoList,producerId,generationId,parameterKeyType,parameterKey,fmiId,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,*cList);

    contentInfoList = cList;

    storeContentSearchResult(hash2,cList,producerHash,generationId);

    if (newEntry)
      storeContentCacheEntry(hash,entry);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Log the error of the data server service for the contents (the parameters are numbered when there are several contents). */

void ServiceImplementation::reportDataServerError(const char *service,int result,const std::vector<const T::ContentInfo*>& contents)
{
  try
  {
    Fmi::Exception exception(BCP, "DataServer returns an error!");
    exception.addParameter("Service", service);
    exception.addParameter("ErrorMessage", DataServer::getResultString(result));
    if (contents.size() == 1)
    {
      exception.addParameter("FileId", std::to_string(contents[0]->mFileId));
      exception.addParameter("MessageIndex", std::to_string(contents[0]->mMessageIndex));
    }
    else
    {
      for (std::size_t t = 0; t < contents.size(); t++)
      {
        std::string fileIdName = "FileId" + std::to_string(t+1);
        std::string messageIndexName = "MessageIndex" + std::to_string(t+1);
        exception.addParameter(fileIdName.c_str(), std::to_string(contents[t]->mFileId));
        exception.addParameter(messageIndexName.c_str(), std::to_string(contents[t]->mMessageIndex));
      }
    }
    std::string errorMsg = exception.getStackTrace();
    PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Report the error of the data server call (and return false if the error stops the value fetch), and convert the values with the conversion function. */

bool ServiceImplementation::checkValueResult(int result,const char *service,const std::vector<const T::ContentInfo*>& contents,bool errorStops,bool conversionByFunction,const std::string& function,const std::vector<std::string>& functionParams,time_t forecastTime,const T::GridValueList& valueList)
{
  try
  {
    if (result != 0)
    {
      reportDataServerError(service,result,contents);
      if (errorStops)
        return false;
    }

    if (conversionByFunction)
      executeConversion(function, functionParams, forecastTime, valueList);

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the conversion function of the parameter mapping. Returns true if the values should be converted with
           the function, false if there is no conversion or it is done by the data server as a modification operation. */

bool ServiceImplementation::getValueConversion(const ParameterMapping& pInfo,std::string& function,std::vector<std::string>& functionParams,uint& modificationOperation,double_vec& modificationParameters)
{
  try
  {
    bool conversionByFunction = conversionFunction(pInfo.mConversionFunction, function, functionParams);

    if (conversionByFunction && mDataServerMethodsEnabled  &&  getModificationOperation(function,functionParams,modificationOperation,modificationParameters))
      conversionByFunction = false;

    return conversionByFunction;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Log the error of the data server service of the location type for the contents. */

void ServiceImplementation::reportDataServerError(const char *service,uchar locationType,int result,const std::vector<const T::ContentInfo*>& contents)
{
  try
  {
    Fmi::Exception exception(BCP, "DataServer returns an error!");
    exception.addParameter("Service", service);
    exception.addParameter("LocationType",Fmi::to_string(locationType));
    exception.addParameter("ErrorMessage", DataServer::getResultString(result));
    if (contents.size() == 1)
    {
      exception.addParameter("FileId", std::to_string(contents[0]->mFileId));
      exception.addParameter("MessageIndex", std::to_string(contents[0]->mMessageIndex));
    }
    else
    {
      for (std::size_t t = 0; t < contents.size(); t++)
      {
        std::string fileIdName = "FileId" + std::to_string(t+1);
        std::string messageIndexName = "MessageIndex" + std::to_string(t+1);
        exception.addParameter(fileIdName.c_str(), std::to_string(contents[t]->mFileId));
        exception.addParameter(messageIndexName.c_str(), std::to_string(contents[t]->mMessageIndex));
      }
    }
    std::string errorMsg = exception.getStackTrace();
    PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Print the content list into the debug log (if it is enabled). */

void ServiceImplementation::printContentList(T::ContentInfoList& contentList)
{
  try
  {
    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      contentList.print(stream, 0, 4);
      PRINT_DATA(mDebugLog, "%s", stream.str().c_str());
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Return true if the conversion function is a data server modification operation, and set the operation and its (non "$") parameters. */

bool ServiceImplementation::getModificationOperation(const std::string& function,const std::vector<std::string>& functionParams,uint& modificationOperation,double_vec& modificationParameters)
{
  try
  {
    std::string f = toUpperString(function);
    auto p = mOperationNames.find(f);
    if (p == mOperationNames.end())
      return false;

    modificationOperation = p->second;
    for (const auto& functionParam : functionParams)
    {
      if (functionParam != "$")
        modificationParameters.push_back(toDouble(functionParam.c_str()));
    }
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the value vector of the content at the point from the data server. Errors are logged and return false. */

bool ServiceImplementation::getPointValueVector(const T::ContentInfo& contentInfo,uchar coordinateType,double x,double y,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,double_vec& valueVector)
{
  try
  {
    int result = mDataServerPtr->getGridValueVectorByPoint(0, contentInfo.mFileId, contentInfo.mMessageIndex, coordinateType, x, y, areaInterpolationMethod,modificationOperation,modificationParameters, valueVector);
    if (result != 0)
    {
      reportDataServerError("getGridValueVectorByPoint",result,{&contentInfo});
      return false;
    }
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Add the special value of the value vector (its constants and its converted values as a ";" separated string). */

void ServiceImplementation::addSpecialValue(const double_vec& valueVector,double x,double y,bool conversionByFunction,const std::string& function,const std::vector<std::string>& functionParams,time_t forecastTime,ParameterValues& valueList)
{
  try
  {
    uint vSize = valueVector.size();
    PRINT_DATA(mDebugLog, "-- Value vector size %u\n",vSize);
    if (vSize > 0)
    {
      // The first number indicates the number of the constant values
      uint sLen = valueVector[0];

      std::string tmp;

      T::GridValueList list;
      for (uint vv = sLen + 1; vv < vSize; vv++)
      {
        T::GridValue rec(x, y, valueVector[vv]);
        list.addGridValue(rec);
      }

      if (conversionByFunction)
        executeConversion(function, functionParams, forecastTime, list);

      for (uint vv = 0; vv <= sLen && vv < vSize; vv++)
        tmp += fmt::format("{:f};", valueVector[vv]);

      uint len = list.getLength();
      for (uint vv = 0; vv < len; vv++)
      {
        T::GridValue rec;
        list.getGridValueByIndex(vv,rec);
        tmp += fmt::format("{:f};", rec.mValue);
      }

      T::GridValue rec(x, y, tmp);
      valueList.mValueList.addGridValue(rec);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Add the special value time interpolated from the value vectors (the constants of the first vector and the converted interpolated values as a ";" separated string). */

void ServiceImplementation::addInterpolatedSpecialValue(const double_vec& valueVector1,const double_vec& valueVector2,const T::ContentInfo& contentInfo1,const T::ContentInfo& contentInfo2,time_t fTime,short timeInterpolationMethod,double x,double y,bool conversionByFunction,const std::string& function,const std::vector<std::string>& functionParams,time_t forecastTime,ParameterValues& valueList)
{
  try
  {
    std::string tmp;

    uint vLen1 = valueVector1.size();

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
    timeInterpolation(list1,list2,contentInfo1.mForecastTimeUTC, contentInfo2.mForecastTimeUTC,fTime,timeInterpolationMethod,list);

    if (conversionByFunction)
      executeConversion(function, functionParams, forecastTime, list);

    for (uint s = 0; s <= sLen && s < vLen1; s++)
      tmp += fmt::format("{:f};", valueVector1[s]);

    uint len = list.getLength();
    for (uint s = 0; s < len; s++)
    {
      T::GridValue rec;
       list.getGridValueByIndex(s,rec);
      tmp += fmt::format("{:f};", rec.mValue);
    }

    T::GridValue rec(x, y, tmp);
    valueList.mValueList.addGridValue(rec);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get special values. */

bool ServiceImplementation::getSpecialValues(
    const Query& /*query*/,
    QueryParameter& qParam,
    const T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    T::GenerationId generationId,
    const std::string& analysisTime,
    UInt64 /*generationFlags*/,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    T::ForecastType forecastType,
    T::ForecastNumber forecastNumber,
    uint parameterFlags,
    short areaInterpolationMethod,
    short timeInterpolationMethod,
    short /*levelInterpolationMethod*/,
    uchar coordinateType,
    double x,
    double y,
    T::ProducerId& newProducerId,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction = getValueConversion(pInfo,function,functionParams,modificationOperation,modificationParameters);

    std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
    time_t fTime = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,
        paramLevelId,paramLevel,forecastType,forecastNumber,parameterFlags,contentList);

    PRINT_DATA(mDebugLog, "         + Found %u content records (fTime = %ld)\n", contentList->getLength(),fTime);

    if (fTime == 0)
      return false;

    printContentList(*contentList);

    uint contentLen = contentList->getLength();
    if (contentLen == 0)
      return false;

    T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);

    setValueListContentInfo(valueList,pInfo,paramLevelId,forecastTime,analysisTime,*contentInfo1);

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC == fTime)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        valueList.mFlags = ParameterValues::Flags::DataAvailable;

        double_vec valueVector;
        if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0  &&
            !getPointValueVector(*contentInfo1,coordinateType,x,y,areaInterpolationMethod,modificationOperation,modificationParameters,valueVector))
          return false;

        addSpecialValue(valueVector,x,y,conversionByFunction,function,functionParams,forecastTime,valueList);

        setDefaultPrecision(qParam,pInfo);

        return true;
      }
      // There is one content record in place, but its time does not match to
      // the requested forecast time. This is used for indicating that there
      // are content records available, but not for the requested time.
      // So, we should use this producer.

      setProducerWithoutValues(producerInfo,pInfo,forecastTime,newProducerId,valueList);
      return false;
    }

    if (contentLen == 2  &&  timeInterpolationMethod != T::TimeInterpolationMethod::Forbidden)
    {
      checkInterpolationTimes(*contentInfo1,*contentInfo2,fTime);

      // We did not find a grid with the exact forecast time, but we find grids that
      // are before and after the current forecast time. This means that we should do
      // some time interpolation.

      // Fetching data from the grids.

      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

      double_vec valueVector1;
      double_vec valueVector2;

      if ((parameterFlags & QueryParameter::Flags::NoReturnValues) == 0  &&
          (!getPointValueVector(*contentInfo1,coordinateType,x,y,areaInterpolationMethod,modificationOperation,modificationParameters,valueVector1)  ||
           !getPointValueVector(*contentInfo2,coordinateType,x,y,areaInterpolationMethod,modificationOperation,modificationParameters,valueVector2)))
        return false;

      uint vLen1 = valueVector1.size();
      uint vLen2 = valueVector2.size();

      if (vLen1 == vLen2 && vLen1 > 1)
      {
        addInterpolatedSpecialValue(valueVector1,valueVector2,*contentInfo1,*contentInfo2,fTime,timeInterpolationMethod,x,y,conversionByFunction,function,functionParams,forecastTime,valueList);

        setDefaultPrecision(qParam,pInfo);

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





/*! \brief Query Server: Get the value vectors of the metric level (height) query for the geometry or the grid of the query. */

bool ServiceImplementation::getMetricLevelValueVectors(
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
    T::Coordinate_vec& coordinates)
{
  try
  {
    if (qParam.mLocationType == QueryParameter::LocationType::Geometry)
    {
      uint width = 0;
      uint height = 0;
      T::Coordinate_svec sCoordinates(new T::Coordinate_vec());

      Identification::gridDef.getGridLatLonCoordinatesByGeometry(query.mAttributeList,sCoordinates,width,height);

      if (!sCoordinates)
        return false;

      bool res = getValueVectorsByHeight(query,query.mCoordinateType,*sCoordinates,qParam,producerInfo,producerGeometryId,generationId,
        analysisTime,generationFlags,pInfo,forecastTime,paramLevelId,paramLevel,newProducerId,valueList);

      //if (((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) != 0))
      coordinates = *sCoordinates;

      return res;
    }

    bool res = getValueVectorsByHeight(query,query.mCoordinateType,query.mAreaCoordinates[0],qParam,producerInfo,producerGeometryId,generationId,
      analysisTime,generationFlags,pInfo,forecastTime,paramLevelId,paramLevel,newProducerId,valueList);

    coordinates = query.mAreaCoordinates[0];
    return res;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the coordinates of the value vector (if there are none yet, and they are requested or needed
           by the conversion function): from the query geometry, from the query grid or from the grid of the content. */

void ServiceImplementation::getValueVectorCoordinates(Query& query,const QueryParameter& qParam,const T::ContentInfo& contentInfo1,bool conversionByFunction,T::Coordinate_vec& coordinates)
{
  try
  {
    if (!coordinates.empty() || ((qParam.mFlags & QueryServer::QueryParameter::Flags::ReturnCoordinates) == 0  && !conversionByFunction))
      return;

    switch (qParam.mLocationType)
    {
      case QueryParameter::LocationType::Geometry:
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
      break;

      case QueryParameter::LocationType::Grid:
        coordinates = query.mAreaCoordinates[0];
        break;

      default:
      {
        T::GridCoordinates coord;
        if (mDataServerPtr->getGridCoordinates(0,contentInfo1.mFileId, contentInfo1.mMessageIndex,T::CoordinateTypeValue::LATLON_COORDINATES,coord) == 0)
          coordinates = coord.mCoordinateList;
      }
      break;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Check the result of the value vector fetch: get the coordinates on success, log the error (and
           return false if the error stops), and set the values (converted by the function if requested). */

bool ServiceImplementation::checkValueVectorResult(int result,Query& query,const QueryParameter& qParam,const char *service,const T::ContentInfo& contentInfo1,bool errorStops,bool conversionByFunction,const std::string& function,std::vector<std::string>& functionParams,time_t forecastTime,T::Coordinate_vec& coordinates,T::ParamValue_vec& valueVector,ParameterValues& valueList)
{
  try
  {
    if (result == 0)
      getValueVectorCoordinates(query,qParam,contentInfo1,conversionByFunction,coordinates);

    if (result != 0  &&  errorStops)
    {
      reportDataServerError(service,result,{&contentInfo1});
      return false;
    }

    if (conversionByFunction)
       executeConversion(function, functionParams, forecastTime, coordinates, valueVector, valueList.mValueVector);
    else
      valueList.mValueVector = valueVector;

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get value vectors. */

bool ServiceImplementation::getValueVectors(
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
    T::Coordinate_vec& coordinates)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getValueVectors()\n");

    if (isMetricLevelQuery(qParam)  &&  (qParam.mLocationType == QueryParameter::LocationType::Geometry  ||  qParam.mLocationType == QueryParameter::LocationType::Grid))
      return getMetricLevelValueVectors(query,qParam,producerInfo,producerGeometryId,generationId,analysisTime,generationFlags,pInfo,forecastTime,paramLevelId,paramLevel,newProducerId,valueList,coordinates);


    short areaInterpolationMethod = getAreaInterpolationMethod(qParam,pInfo);

    short timeInterpolationMethod = getTimeInterpolationMethod(qParam,pInfo);

    //short levelInterpolationMethod = qParam.mLevelInterpolationMethod;
    //if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
    //  levelInterpolationMethod = pInfo.mLevelInterpolationMethod;

    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction = getValueConversion(pInfo,function,functionParams,modificationOperation,modificationParameters);
    T::ParamValue_vec valueVector;

    std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
    time_t fTime = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,
        paramLevelId,paramLevel,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList);

    PRINT_DATA(mDebugLog, "         + Found %u content records (fTime = %ld)\n", contentList->getLength(),fTime);

    if (fTime == 0)
      return false;

    printContentList(*contentList);

    uint contentLen = contentList->getLength();
    if (contentLen == 0)
      return false;

    T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList->getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList->getContentInfoByIndex(3);

    setValueListContentInfo(valueList,pInfo,paramLevelId,forecastTime,analysisTime,*contentInfo1);
    valueList.mGenerationFlags = generationFlags;

    query.mAttributeList.setAttribute("grid.timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
    query.mAttributeList.setAttribute("grid.areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));
    //query.mAttributeList.setAttribute("grid.levelInterpolationMethod",Fmi::to_string(levelInterpolationMethod));

    if (contentLen == 1  &&  contentInfo1->mForecastTimeUTC != fTime)
    {
      // There is one content record in place, but its time does not match to
      // the requested forecast time. This is used for indicating that there
      // are content records available, but not for the requested time.
      // So, we should use this producer.

      setProducerWithoutValues(producerInfo,pInfo,forecastTime,newProducerId,valueList);
      return false;
    }

    if (contentLen == 1)
    {
      // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

      valueList.mFlags = ParameterValues::Flags::DataAvailable;

      if (!fetchValueVector(query,qParam,[&]() { return fetchByValueVectorLocation(qParam.mLocationType,
            [&]() { return mDataServerPtr->getGridValueVectorByGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,query.mAttributeList,modificationOperation,modificationParameters,valueVector); },
            [&]() { return mDataServerPtr->getGridValueVectorByCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,query.mCoordinateType,query.mAreaCoordinates[0],areaInterpolationMethod,modificationOperation,modificationParameters,valueVector); },
            [&]() { return mDataServerPtr->getGridValueVector(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,modificationOperation,modificationParameters,valueVector); }); },
          valueVectorServiceName(qParam.mLocationType,"getGridValueVectorByGeometry","getGridValueVectorByCoordinateList","getGridValueVector"),*contentInfo1,true,conversionByFunction,function,functionParams,forecastTime,coordinates,valueVector,valueList))
        return false;

      return finishValueVectors(qParam,pInfo,coordinates);
    }

    short levelInterpolationMethod = getLevelInterpolationMethod(paramLevelId,qParam.mLevelInterpolationMethod,qParam.mFlags);

    if (contentLen == 2  &&  isTimeInterpolationPair(*contentInfo1,*contentInfo2,fTime,timeInterpolationMethod))
    {
      // We did not find a grid with the exact forecast time, but we find grids that
      // are before and after the current forecast time. This means that we should do
      // some time interpolation.

      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

      if (!fetchValueVector(query,qParam,[&]() { return fetchByValueVectorLocation(qParam.mLocationType,
            [&]() { return mDataServerPtr->getGridValueVectorByTimeAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,query.mAttributeList,modificationOperation,modificationParameters,valueVector); },
            [&]() { return mDataServerPtr->getGridValueVectorByTimeAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,query.mCoordinateType,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueVector); },
            [&]() { return mDataServerPtr->getGridValueVectorByTime(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,timeInterpolationMethod,modificationOperation,modificationParameters,valueVector); }); },
          valueVectorServiceName(qParam.mLocationType,"getGridValueVectorByTimeAndGeometry","getGridValueVectorByTimeAndCoordinateList","getGridValueVectorByTime"),*contentInfo1,true,conversionByFunction,function,functionParams,forecastTime,coordinates,valueVector,valueList))
        return false;

      return finishValueVectors(qParam,pInfo,coordinates);
    }

    if (contentLen == 2  &&  isLevelInterpolationPair(*contentInfo1,*contentInfo2,fTime,paramLevel,levelInterpolationMethod))
    {
      // We did not find a grid with the exact level but we find grids that
      // are before and after the current level. This means that we should do
      // some level interpolation.

      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByLevelInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

      // The level is set only when the values are fetched.

      if (!fetchValueVector(query,qParam,[&]() { valueList.mParameterLevel = paramLevel; return fetchByValueVectorLocation(qParam.mLocationType,
            [&]() { return mDataServerPtr->getGridValueVectorByLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,query.mAttributeList,modificationOperation,modificationParameters,valueVector); },
            [&]() { return mDataServerPtr->getGridValueVectorByLevelAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,query.mCoordinateType,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueVector); },
            [&]() { return mDataServerPtr->getGridValueVectorByLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,levelInterpolationMethod,modificationOperation,modificationParameters,valueVector); }); },
          valueVectorServiceName(qParam.mLocationType,"getGridValueVectorByLevelAndGeometry","getGridValueVectorByLevelAndCoordinateList","getGridValueVectorByLevel"),*contentInfo1,true,conversionByFunction,function,functionParams,forecastTime,coordinates,valueVector,valueList))
        return false;

      return finishValueVectors(qParam,pInfo,coordinates);
    }

    if (contentLen == 4  &&  isTimeLevelInterpolationQuad(*contentInfo1,*contentInfo2,*contentInfo3,*contentInfo4,fTime,paramLevel,timeInterpolationMethod,levelInterpolationMethod))
    {
      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation | ParameterValues::Flags::DataAvailableByLevelInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
      valueList.mFileId[2] = contentInfo3->mFileId;
      valueList.mMessageIndex[2] = contentInfo3->mMessageIndex;
      valueList.mFileId[3] = contentInfo4->mFileId;
      valueList.mMessageIndex[3] = contentInfo4->mMessageIndex;
      valueList.mParameterLevel = paramLevel;

      if (!fetchValueVector(query,qParam,[&]() { return fetchByValueVectorLocation(qParam.mLocationType,
            [&]() { return mDataServerPtr->getGridValueVectorByTimeLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,query.mAttributeList,modificationOperation,modificationParameters,valueVector); },
            [&]() { return mDataServerPtr->getGridValueVectorByTimeLevelAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,query.mCoordinateType,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueVector); },
            [&]() { return mDataServerPtr->getGridValueVectorByTimeAndLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueVector); }); },
          valueVectorServiceName(qParam.mLocationType,"getGridValueVectorByTimeLevelAndGeometry","getGridValueVectorByTimeLevelAndCoordinateList","getGridValueVectorByTimeAndLevel"),*contentInfo1,true,conversionByFunction,function,functionParams,forecastTime,coordinates,valueVector,valueList))
        return false;

      return finishValueVectors(qParam,pInfo,coordinates);
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the grid properties of the content (as integers). Errors are logged and return false. */

bool ServiceImplementation::getGridFileProperties(const T::ContentInfo& contentInfo1,std::map<uint,Int64 >& oldProperties)
{
  try
  {
    T::PropertySettingVec properties;
    int result = mDataServerPtr->getGridProperties(0,contentInfo1.mFileId, contentInfo1.mMessageIndex,properties);
    if (result != 0)
    {
      reportDataServerError("getGridProperties",result,{&contentInfo1});
      return false;
    }

    for (auto& propertie : properties)
    {
      //std::cout << "PROPERTY " << it->propertyId << " = " << it->propertyValue << "\n";
      oldProperties.insert(std::pair<uint,Int64 >(propertie.propertyId,toInt64(propertie.propertyValue.c_str())));
    }
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Create the message of the new grid file by the query attributes (geometry, or crs, bounding box
           and size) and the properties of the original message. Returns nullptr for an invalid GRIB version. */

GRID::Message* ServiceImplementation::newGridFileMessage(const Query& query,uint version,time_t forecastTime,T::ParamLevel paramLevel,const T::ContentInfo& contentInfo1,const std::map<uint,Int64 >& oldProperties,GRID::GridFile& newGridFile)
{
  try
  {
    std::map<uint,Int64 > newProperties;

    const char *geometryIdStr = query.mAttributeList.getAttributeValue("grid.geometryId");
    const char *crsStr = query.mAttributeList.getAttributeValue("grid.crs");
    const char *bboxStr = query.mAttributeList.getAttributeValue("grid.bbox");
    const char *gridSizeStr = query.mAttributeList.getAttributeValue("grid.size");

    GridFileArea area;
    getGridFileArea(query,gridSizeStr,bboxStr,area);

    if (!geometryIdStr  &&  crsStr  &&  gridSizeStr)
    {
      ProjectionParameters pp;
      getProjectionParameters(crsStr,pp);
      addProjectionProperties(query,version,pp,area,newProperties);
    }

    if (version == 1)
      return newGrib1Message(newGridFile,geometryIdStr,forecastTime,paramLevel,contentInfo1,oldProperties,newProperties);

    if (version == 2)
      return newGrib2Message(newGridFile,geometryIdStr,forecastTime,paramLevel,oldProperties,newProperties);

    Fmi::Exception exception(BCP, "No valid grib type and version found!");
    std::string errorMsg = exception.getStackTrace();
    PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Set the fetched values (converted by the function if requested) to the message and add the
           grid file data to the value list. Data server errors are logged and return false. */

bool ServiceImplementation::addGridFileData(int result,const char *service,const T::ContentInfo& contentInfo1,bool levelInterpolated,T::ParamLevel paramLevel,QueryParameter& qParam,const ParameterMapping& pInfo,bool conversionByFunction,const std::string& function,std::vector<std::string>& functionParams,time_t forecastTime,T::Coordinate_vec& coordinates,T::ParamValue_vec& valueVector,GRID::GridFile& newGridFile,GRID::Message& newMessage,UInt64 sz,ParameterValues& valueList)
{
  try
  {
    if (result != 0)
    {
      reportDataServerError(service,result,{&contentInfo1});
      return false;
    }

    if (levelInterpolated)
      valueList.mParameterLevel = paramLevel;

    if (conversionByFunction)
    {
       T::ParamValue_vec newValueVector;
       executeConversion(function, functionParams, forecastTime, coordinates, valueVector, newValueVector);
       newMessage.setGridValues(newValueVector);
    }
    else
    {
      newMessage.setGridValues(valueVector);
    }

    // newMessage->print(std::cout,0,0);

    setDefaultPrecision(qParam,pInfo);

    auto *data = new uchar[sz];
    MemoryWriter memoryWriter(data,sz,true);
    newGridFile.write(memoryWriter);
    UInt64 fsize = memoryWriter.getMaxWritePosition();

    std::vector<uchar> dataFile;
    dataFile.reserve(fsize);
    for (UInt64 i=0; i <fsize; i++)
      dataFile.emplace_back(data[i]);

    valueList.mValueData.emplace_back(dataFile);

    //newGridFile.write("/tmp/test.grib");

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get grid files. */

bool ServiceImplementation::getGridFiles(
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
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (query.mAreaCoordinates.empty())
      return false;

    short areaInterpolationMethod = getAreaInterpolationMethod(qParam,pInfo);

    short timeInterpolationMethod = getTimeInterpolationMethod(qParam,pInfo);

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

    printContentList(*contentList);

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


    std::map<uint,Int64 > oldProperties;
    if (!getGridFileProperties(*contentInfo1,oldProperties))
      return false;

    uint version = getGribVersion(oldProperties);
    if (version == 0)
      return false;


    GRID::GridFile newGridFile;
    GRID::Message *newMessage = newGridFileMessage(query,version,forecastTime,paramLevel,*contentInfo1,oldProperties,newGridFile);
    if (newMessage == nullptr)
      return false;

    newMessage->initSpatialReference();

    T::Coordinate_svec coordinates = newMessage->getGridLatLonCoordinates();
    if (coordinates->empty())
    {
      PRINT_DATA(mDebugLog, "-- No coordinates available\n");
      return false;
    }

    T::Dimensions d = newMessage->getGridDimensions();

    uint width = d.nx();
    uint height = d.ny();
    uint numOfValues = width*height;
    uint numOfBytes = 4*numOfValues;
    UInt64 sz = 2 * numOfBytes;


    if (contentLen == 1  &&  contentInfo1->mForecastTimeUTC != forecastTime /*|| timeInterpolationMethod == T::TimeInterpolationMethod::None || timeInterpolationMethod == T::TimeInterpolationMethod::Nearest*/)
    {
      // There is one content record in place, but its time does not match to
      // the requested forecast time. This is used for indicating that there
      // are content records available, but not for the requested time.
      // So, we should use this producer.

      newProducerId = producerInfo.mProducerId;
      valueList.mProducerId = producerInfo.mProducerId;
      return false;
    }

    if (contentLen == 1)
    {
      // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

      result = mDataServerPtr->getGridValueVectorByCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,query.mCoordinateType,*coordinates,areaInterpolationMethod,modificationOperation,modificationParameters,valueVector);

      return addGridFileData(result,"getGridValueVectorByCoordinateList",*contentInfo1,false,paramLevel,qParam,pInfo,conversionByFunction,function,functionParams,forecastTime,*coordinates,valueVector,newGridFile,*newMessage,sz,valueList);
    }

    if (contentLen == 2  &&  isTimeInterpolationPair(*contentInfo1,*contentInfo2,forecastTime,timeInterpolationMethod))
    {
      // We did not find a grid with the exact forecast time, but we find grids that
      // are before and after the current forecast time. This means that we should do
      // some time interpolation.

      result = mDataServerPtr->getGridValueVectorByTimeAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,forecastTime,query.mCoordinateType,*coordinates,query.mAttributeList,modificationOperation,modificationParameters,valueVector);

      return addGridFileData(result,"getGridValueVectorByTimeAndCoordinateList",*contentInfo1,false,paramLevel,qParam,pInfo,conversionByFunction,function,functionParams,forecastTime,*coordinates,valueVector,newGridFile,*newMessage,sz,valueList);
    }

    if (contentLen == 2  &&  isLevelInterpolationPair(*contentInfo1,*contentInfo2,forecastTime,paramLevel,levelInterpolationMethod))
    {
      // We did not find a grid with the exact level but we find grids that
      // are before and after the current level. This means that we should do
      // some level interpolation.

      result = mDataServerPtr->getGridValueVectorByLevelAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,query.mCoordinateType,*coordinates,query.mAttributeList,modificationOperation,modificationParameters,valueVector);

      return addGridFileData(result,"getGridValueVectorByLevelAndCoordinateList",*contentInfo1,true,paramLevel,qParam,pInfo,conversionByFunction,function,functionParams,forecastTime,*coordinates,valueVector,newGridFile,*newMessage,sz,valueList);
    }

    if (contentLen == 4  &&  isTimeLevelInterpolationQuad(*contentInfo1,*contentInfo2,*contentInfo3,*contentInfo4,forecastTime,paramLevel,timeInterpolationMethod,levelInterpolationMethod))
    {
      result = mDataServerPtr->getGridValueVectorByTimeLevelAndCoordinateList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,forecastTime,paramLevel,query.mCoordinateType,*coordinates,query.mAttributeList,modificationOperation,modificationParameters,valueVector);

      return addGridFileData(result,"getGridValueVectorByTimeLevelAndCoordinateList",*contentInfo1,true,paramLevel,qParam,pInfo,conversionByFunction,function,functionParams,forecastTime,*coordinates,valueVector,newGridFile,*newMessage,sz,valueList);
    }
    return false;
  }
  catch (...)
  {

    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get content list. */

time_t ServiceImplementation::getContentList(
    const T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    T::GenerationId generationId,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId paramLevelId,
    T::ParamLevel paramLevel,
    T::ForecastType forecastType,
    T::ForecastNumber forecastNumber,
    uint /*parameterFlags*/,
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





/*! \brief Query Server: Get the content list of the level, or the previous content list if the level is the same as
           the previous level. Returns the forecast time of the contents. */

time_t ServiceImplementation::getPreviousOrLevelContentList(
    const T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    T::GenerationId generationId,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId levelId,
    T::ParamLevel level,
    const QueryParameter& qParam,
    std::shared_ptr<T::ContentInfoList>& prevContentList,
    T::ParamLevel& prevLevel,
    time_t& prevTime,
    std::shared_ptr<T::ContentInfoList>& contentList)
{
  try
  {
    time_t fTime = 0;

    if (prevContentList  &&  prevLevel == level)
    {
      contentList = prevContentList;
      fTime = prevTime;
    }
    else
    {
      contentList = std::make_shared<T::ContentInfoList>();
      fTime = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,levelId,level,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList);
      prevContentList = contentList;
      prevLevel = level;
      prevTime = fTime;
    }

    PRINT_DATA(mDebugLog, "         + Found %u content records (fTime = %ld)\n", contentList->getLength(),fTime);

    return fTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the value of the content at the point from the data server. Errors are logged and return false. */

bool ServiceImplementation::getHeightPointValue(const T::ContentInfo& contentInfo,T::CoordinateType coordinateType,const T::Coordinate& coordinate,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  try
  {
    value = NAN;
    int result = mDataServerPtr->getGridValueByPoint(0, contentInfo.mFileId, contentInfo.mMessageIndex,coordinateType,coordinate.x(),coordinate.y(),areaInterpolationMethod,modificationOperation,modificationParameters,value);
    if (result != 0)
    {
      reportDataServerError("getGridValueByPoint",result,{&contentInfo});
      return false;
    }
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Add the level interpolated value of the point (the contents are at the requested time).
           Returns false on a data server error. */

bool ServiceImplementation::addLevelInterpolatedPointValue(
    const Query& query,
    const T::Coordinate& coordinate,
    const HeightRec& rec,
    const QueryParameter& qParam,
    const T::ContentInfo& contentInfo1_1,
    const T::ContentInfo& contentInfo2_1,
    T::ParamLevel paramLevel,
    short areaInterpolationMethod,
    short levelInterpolationMethod,
    uint modificationOperation,
    double_vec& modificationParameters,
    ParameterValues& valueList)
{
  try
  {
    // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

    valueList.mFlags = ParameterValues::Flags::DataAvailable;

    if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) != 0)
      return true;

    T::ParamValue value1 = NAN;
    if (!getHeightPointValue(contentInfo1_1,query.mCoordinateType,coordinate,areaInterpolationMethod,modificationOperation,modificationParameters,value1))
      return false;

    T::ParamValue value2 = NAN;
    if (!getHeightPointValue(contentInfo2_1,query.mCoordinateType,coordinate,areaInterpolationMethod,modificationOperation,modificationParameters,value2))
      return false;


    T::ParamValue val = levelInterpolation(value1,value2,rec.height1_1,rec.height1_2,paramLevel,levelInterpolationMethod);
    T::GridValue v(coordinate.x(),coordinate.y(),val);
    valueList.mValueList.addGridValue(v);
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Add the time and level interpolated value of the point (if the times of the height record match
           the content times). Returns false on a data server error. */

bool ServiceImplementation::addTimeLevelInterpolatedPointValue(
    const Query& query,
    const T::Coordinate& coordinate,
    const HeightRec& rec,
    const QueryParameter& qParam,
    const T::ContentInfo& contentInfo1_1,
    const T::ContentInfo& contentInfo1_2,
    const T::ContentInfo& contentInfo2_1,
    const T::ContentInfo& contentInfo2_2,
    time_t forecastTime,
    T::ParamLevel paramLevel,
    short areaInterpolationMethod,
    short timeInterpolationMethod,
    short levelInterpolationMethod,
    uint modificationOperation,
    double_vec& modificationParameters,
    ParameterValues& valueList)
{
  try
  {
    // Content records have different times, but most likely the same levels, so we need to do time interpolation.

    valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;

    if ((qParam.mFlags & QueryParameter::Flags::NoReturnValues) != 0  ||  rec.time1 != contentInfo1_1.mForecastTimeUTC  ||  rec.time2 != contentInfo1_2.mForecastTimeUTC)
      return true;

    T::ParamValue value1_1 = NAN;
    if (!getHeightPointValue(contentInfo1_1,query.mCoordinateType,coordinate,areaInterpolationMethod,modificationOperation,modificationParameters,value1_1))
      return false;

    T::ParamValue value1_2 = NAN;
    if (!getHeightPointValue(contentInfo1_2,query.mCoordinateType,coordinate,areaInterpolationMethod,modificationOperation,modificationParameters,value1_2))
      return false;

    T::ParamValue value2_1 = NAN;
    if (!getHeightPointValue(contentInfo2_1,query.mCoordinateType,coordinate,areaInterpolationMethod,modificationOperation,modificationParameters,value2_1))
      return false;

    T::ParamValue value2_2 = NAN;
    if (!getHeightPointValue(contentInfo2_2,query.mCoordinateType,coordinate,areaInterpolationMethod,modificationOperation,modificationParameters,value2_2))
      return false;

    T::ParamValue val1 = levelInterpolation(value1_1,value2_1,rec.height1_1,rec.height1_2,paramLevel,levelInterpolationMethod);
    T::ParamValue val2 = levelInterpolation(value1_2,value2_2,rec.height2_1,rec.height2_2,paramLevel,levelInterpolationMethod);
    T::ParamValue val = timeInterpolation(val1,val2,rec.time1,rec.time2,forecastTime,timeInterpolationMethod);

    T::GridValue v(coordinate.x(),coordinate.y(),val);
    valueList.mValueList.addGridValue(v);
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Add the height interpolated values of the points (one height record per coordinate).
           Returns false if the contents are missing or do not match, or on a data server error. */

bool ServiceImplementation::addPointValuesByHeight(
    const Query& query,
    T::Coordinate_vec& coordinates,
    const HeightRec_vec& recs,
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
    short areaInterpolationMethod,
    short timeInterpolationMethod,
    short levelInterpolationMethod,
    uint modificationOperation,
    double_vec& modificationParameters,
    ParameterValues& valueList)
{
  try
  {
    uint rlen = recs.size();

    std::shared_ptr<T::ContentInfoList> prevContentList1;
    std::shared_ptr<T::ContentInfoList> prevContentList2;
    T::ParamLevel prevLevel1 = 0;
    T::ParamLevel prevLevel2 = 0;
    time_t prevTime1 = 0;
    time_t prevTime2 = 0;

    for (uint r=0; r<rlen; r++)
    {
      std::shared_ptr<T::ContentInfoList> contentList1;
      time_t fTime1 = getPreviousOrLevelContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,recs[r].levelId,recs[r].level1_1,qParam,prevContentList1,prevLevel1,prevTime1,contentList1);

      if (fTime1 == 0)
        return false;

      printContentList(*contentList1);

      uint contentLen1 = contentList1->getLength();
      if (contentLen1 == 0)
        return false;

      std::shared_ptr<T::ContentInfoList> contentList2;
      time_t fTime2 = getPreviousOrLevelContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,recs[r].levelId,recs[r].level1_2,qParam,prevContentList2,prevLevel2,prevTime2,contentList2);

      if (fTime2 == 0)
        return false;

      printContentList(*contentList2);

      uint contentLen2 = contentList2->getLength();
      if (contentLen2 == 0)
        return false;

      if (contentLen1 != contentLen2 || fTime1 != fTime2)
        return false;

      T::ContentInfo* contentInfo1_1 = contentList1->getContentInfoByIndex(0);
      T::ContentInfo* contentInfo1_2 = contentList1->getContentInfoByIndex(1);
      T::ContentInfo* contentInfo2_1 = contentList2->getContentInfoByIndex(0);
      T::ContentInfo* contentInfo2_2 = contentList2->getContentInfoByIndex(1);

      setLevelValueListContentInfo(valueList,pInfo,paramLevelId,forecastTime,analysisTime,generationFlags,paramLevel,contentInfo1_1);

      if (contentLen1 == 1  &&  (contentInfo1_1->mForecastTimeUTC != fTime1  ||  recs[r].time1 != fTime1))
      {
        // There is one content record in place, but its time does not match to
        // the requested forecast time. This is used for indicating that there
        // are content records available, but not for the requested time.
        // So, we should use this producer.

        setProducerWithoutValues(producerInfo,pInfo,forecastTime,newProducerId,valueList);
        return false;
      }

      if (contentLen1 == 1  &&  !addLevelInterpolatedPointValue(query,coordinates[r],recs[r],qParam,*contentInfo1_1,*contentInfo2_1,paramLevel,areaInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList))
        return false;

      if (contentLen1 == 2  &&  isTimeInterpolationPair(*contentInfo1_1,*contentInfo1_2,fTime1,timeInterpolationMethod)  &&
          !addTimeLevelInterpolatedPointValue(query,coordinates[r],recs[r],qParam,*contentInfo1_1,*contentInfo1_2,*contentInfo2_1,*contentInfo2_2,forecastTime,paramLevel,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList))
        return false;
    }
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get point values by height. */

bool ServiceImplementation::getPointValuesByHeight(
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
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getPointValuesByHeight(%d)\n",paramLevel);

    short areaInterpolationMethod = getAreaInterpolationMethod(qParam,pInfo);

    short timeInterpolationMethod = getTimeInterpolationMethod(qParam,pInfo);

    short levelInterpolationMethod = qParam.mLevelInterpolationMethod;
    if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
      levelInterpolationMethod = pInfo.mLevelInterpolationMethod;

    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction = getValueConversion(pInfo,function,functionParams,modificationOperation,modificationParameters);

    HeightRec_vec recs;
    if (getClosestLevelsByHeight(producerInfo.mProducerId,generationId,producerGeometryId,qParam.mParameterLevelId,qParam.mForecastType,qParam.mForecastNumber,forecastTime,coordinateType,coordinates,paramLevel,recs)  &&  recs.size() == coordinates.size())
    {
      if (!addPointValuesByHeight(query,coordinates,recs,qParam,producerInfo,producerGeometryId,generationId,analysisTime,generationFlags,pInfo,forecastTime,paramLevelId,paramLevel,newProducerId,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList))
        return false;
    }

    if (conversionByFunction)
      executeConversion(function, functionParams, forecastTime, valueList.mValueList);

    setDefaultPrecision(qParam,pInfo);

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






/*! \brief Query Server: Get the value vectors of the levels (one vector per level) at the coordinates. Returns false if
           a content is missing or the data server call fails. */

bool ServiceImplementation::getHeightLevelValueVectors(
    const Query& query,
    T::Coordinate_vec& coordinates,
    const T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    T::GenerationId generationId,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId levelId,
    const std::set<T::ParamLevel>& levels,
    const QueryParameter& qParam,
    short areaInterpolationMethod,
    double_vec& modificationParameters,
    std::vector<std::shared_ptr<T::ParamValue_vec>>& valueVectors)
{
  try
  {
    for (int lev : levels)
    {
      std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
      time_t fTime = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,levelId,lev,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList);

      //PRINT_DATA(mDebugLog, "         + Found %u content records (fTime = %ld)\n", contentList1->getLength(),fTime1);

      if (fTime == 0)
        return false;

      uint contentLen = contentList->getLength();
      if (contentLen == 0)
        return false;

      T::ContentInfo* contentInfo = contentList->getContentInfoByIndex(0);

      std::shared_ptr<T::ParamValue_vec> values(new T::ParamValue_vec());
      int result1 = mDataServerPtr->getGridValueVectorByCoordinateList(0,contentInfo->mFileId,contentInfo->mMessageIndex,query.mCoordinateType,coordinates,areaInterpolationMethod,0,modificationParameters,*values);
      if (result1 != 0)
      {
        reportDataServerError("getGridValueVectorByCoordinateList",result1,{contentInfo});
        return false;
      }

      valueVectors.push_back(values);
    }
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Set the height interpolated values to the value list (converted by the function if requested) and the default precision. */

void ServiceImplementation::setHeightValueVector(QueryParameter& qParam,const ParameterMapping& pInfo,bool conversionByFunction,const std::string& function,std::vector<std::string>& functionParams,time_t forecastTime,T::Coordinate_vec& coordinates,T::ParamValue_vec& valueVector,ParameterValues& valueList)
{
  try
  {
    if (conversionByFunction  &&  (qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
       executeConversion(function, functionParams, forecastTime, coordinates, valueVector, valueList.mValueVector);
    else
      valueList.mValueVector = valueVector;

    setDefaultPrecision(qParam,pInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get value vectors by height. */

bool ServiceImplementation::getValueVectorsByHeight(
    const Query& query,
    T::CoordinateType coordinateType,
    T::Coordinate_vec& coordinates,
    QueryParameter& qParam,
    const T::ProducerInfo& producerInfo,
    T::GeometryId producerGeometryId,
    T::GenerationId generationId,
    const std::string& /*analysisTime*/,
    UInt64 /*generationFlags*/,
    ParameterMapping& pInfo,
    time_t forecastTime,
    T::ParamLevelId /*paramLevelId*/,
    T::ParamLevel paramLevel,
    const T::ProducerId& /*newProducerId*/,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getValueVectorsByHeight(%d)\n",paramLevel);

    short areaInterpolationMethod = getAreaInterpolationMethod(qParam,pInfo);

    short timeInterpolationMethod = getTimeInterpolationMethod(qParam,pInfo);

    short levelInterpolationMethod = qParam.mLevelInterpolationMethod;
    if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
      levelInterpolationMethod = pInfo.mLevelInterpolationMethod;

    uint modificationOperation = 0;  // Not used: the data server calls below use no modification operation
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction = getValueConversion(pInfo,function,functionParams,modificationOperation,modificationParameters);

    T::ParamValue_vec valueVector;
    HeightRec_vec recs;
    recs.reserve(coordinates.size());
    valueVector.reserve(coordinates.size());

    if (!getClosestLevelsByHeight(producerInfo.mProducerId,generationId,producerGeometryId,qParam.mParameterLevelId,qParam.mForecastType,qParam.mForecastNumber,forecastTime,coordinateType,coordinates,paramLevel,recs))
      return false;

    uint rlen = recs.size();
    if (rlen != coordinates.size())
      return false;

    std::set<T::ParamLevel> levels1 = indexHeightLevels(recs,&HeightRec::level1_1,&HeightRec::level1_2);

    std::vector<std::shared_ptr<T::ParamValue_vec>> valueList1;

    if (!getHeightLevelValueVectors(query,coordinates,producerInfo,producerGeometryId,generationId,pInfo,forecastTime,recs[0].levelId,levels1,qParam,areaInterpolationMethod,modificationParameters,valueList1))
      return false;

    if (recs[0].time1 == recs[0].time2)
    {
      valueVector.reserve(rlen);
      for (uint r=0; r<rlen; r++)
      {
        T::ParamValue val = levelInterpolation((*valueList1[recs[r].level1_1])[r],(*valueList1[recs[r].level1_2])[r],recs[r].height1_1,recs[r].height1_2,paramLevel,levelInterpolationMethod);
        valueVector.push_back(val);
      }

      setHeightValueVector(qParam,pInfo,conversionByFunction,function,functionParams,forecastTime,coordinates,valueVector,valueList);

      return true;
    }


    std::set<T::ParamLevel> levels2 = indexHeightLevels(recs,&HeightRec::level2_1,&HeightRec::level2_2);

    std::vector<std::shared_ptr<T::ParamValue_vec>> valueList2;

    if (!getHeightLevelValueVectors(query,coordinates,producerInfo,producerGeometryId,generationId,pInfo,forecastTime,recs[0].levelId,levels2,qParam,areaInterpolationMethod,modificationParameters,valueList2))
      return false;

    valueVector.reserve(rlen);
    for (uint r=0; r<rlen; r++)
    {
      T::ParamValue val1 = levelInterpolation((*valueList1[recs[r].level1_1])[r],(*valueList2[recs[r].level2_1])[r],recs[r].height1_1,recs[r].height1_2,paramLevel,levelInterpolationMethod);
      T::ParamValue val2 = levelInterpolation((*valueList1[recs[r].level1_2])[r],(*valueList2[recs[r].level2_2])[r],recs[r].height2_1,recs[r].height2_2,paramLevel,levelInterpolationMethod);
      T::ParamValue val = timeInterpolation(val1,val2,recs[r].time1,recs[r].time2,forecastTime,timeInterpolationMethod);

      valueVector.push_back(val);
    }

    setHeightValueVector(qParam,pInfo,conversionByFunction,function,functionParams,forecastTime,coordinates,valueVector,valueList);

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





#if 0  // Working

/*! \brief Query Server: Get value vectors by height. */

bool ServiceImplementation::getValueVectorsByHeight(
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
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getValueVectorsByHeight(%d)\n",paramLevel);

    short areaInterpolationMethod = getAreaInterpolationMethod(qParam,pInfo);

    short timeInterpolationMethod = getTimeInterpolationMethod(qParam,pInfo);

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
            modificationParameters.push_back(toDouble(it->c_str()));
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
                  reportDataServerError("getGridValueByPoint",result1,{contentInfo1_1});
                  return false;
                }

                T::ParamValue value2;
                int result2 = mDataServerPtr->getGridValueByPoint(0, contentInfo2_1->mFileId, contentInfo2_1->mMessageIndex,query.mCoordinateType,coordinates[r].x(),coordinates[r].y(),areaInterpolationMethod,modificationOperation,modificationParameters,value2);
                if (result2 != 0)
                {
                  reportDataServerError("getGridValueByPoint",result2,{contentInfo2_1});
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

              setProducerWithoutValues(producerInfo,pInfo,forecastTime,newProducerId,valueList);
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
                  reportDataServerError("getGridValueByPoint",result1_1,{contentInfo1_1});
                  return false;
                }

                T::ParamValue value1_2;
                int result1_2 = mDataServerPtr->getGridValueByPoint(0, contentInfo1_2->mFileId, contentInfo1_2->mMessageIndex,query.mCoordinateType,coordinates[r].x(),coordinates[r].y(),areaInterpolationMethod,modificationOperation,modificationParameters,value1_2);
                if (result1_2 != 0)
                {
                  reportDataServerError("getGridValueByPoint",result1_2,{contentInfo1_2});
                  return false;
                }

                T::ParamValue value2_1;
                int result2_1 = mDataServerPtr->getGridValueByPoint(0, contentInfo2_1->mFileId, contentInfo2_1->mMessageIndex,query.mCoordinateType,coordinates[r].x(),coordinates[r].y(),areaInterpolationMethod,modificationOperation,modificationParameters,value2_1);
                if (result2_1 != 0)
                {
                  reportDataServerError("getGridValueByPoint",result2_1,{contentInfo2_1});
                  return false;
                }

                T::ParamValue value2_2;
                int result2_2 = mDataServerPtr->getGridValueByPoint(0, contentInfo2_2->mFileId, contentInfo2_2->mMessageIndex,query.mCoordinateType,coordinates[r].x(),coordinates[r].y(),areaInterpolationMethod,modificationOperation,modificationParameters,value2_2);
                if (result2_2 != 0)
                {
                  reportDataServerError("getGridValueByPoint",result2_2,{contentInfo2_2});
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

    setDefaultPrecision(qParam,pInfo);

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}

#endif






/*! \brief Query Server: Get level interpolation method. */

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
        default:
          break;
      }
    }
    return T::LevelInterpolationMethod::Nearest;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get point values. */

bool ServiceImplementation::getPointValues(
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
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getPointValues()\n");

    if (isMetricLevelQuery(qParam))
    {
      return getPointValuesByHeight(query,query.mCoordinateType,query.mAreaCoordinates[0],qParam,producerInfo,producerGeometryId,generationId,analysisTime,generationFlags,pInfo,forecastTime,
          paramLevelId,paramLevel,newProducerId,valueList);
    }


    short areaInterpolationMethod = getAreaInterpolationMethod(qParam,pInfo);

    short timeInterpolationMethod = getTimeInterpolationMethod(qParam,pInfo);

    //if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
    //  levelInterpolationMethod = pInfo.mLevelInterpolationMethod;

    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction = getValueConversion(pInfo,function,functionParams,modificationOperation,modificationParameters);

    std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
    time_t fTime = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,
        paramLevelId,paramLevel,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList);

    PRINT_DATA(mDebugLog, "         + Found %u content records (fTime = %ld)\n", contentList->getLength(),fTime);

    if (fTime == 0)
      return false;

    printContentList(*contentList);

    uint contentLen = contentList->getLength();
    if (contentLen == 0)
      return false;

    T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList->getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList->getContentInfoByIndex(3);

    setValueListContentInfo(valueList,pInfo,paramLevelId,forecastTime,analysisTime,*contentInfo1);
    valueList.mGenerationFlags = generationFlags;

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC != fTime)
      {
        // There is one content record in place, but its time does not match to
        // the requested forecast time. This is used for indicating that there
        // are content records available, but not for the requested time.
        // So, we should use this producer.

        setProducerWithoutValues(producerInfo,pInfo,forecastTime,newProducerId,valueList);
        return false;
      }

      // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

      valueList.mFlags = ParameterValues::Flags::DataAvailable;

      if (!fetchValues(qParam,[&]() { return mDataServerPtr->getGridValueListByPointList(0, contentInfo1->mFileId, contentInfo1->mMessageIndex,query.mCoordinateType,query.mAreaCoordinates[0],areaInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList); },"getGridValueListByPointList",{contentInfo1},true,conversionByFunction,function,functionParams,forecastTime,valueList.mValueList))
        return false;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }


    short levelInterpolationMethod = getLevelInterpolationMethod(paramLevelId,qParam.mLevelInterpolationMethod,qParam.mFlags);

    if (contentLen == 2  &&  isLevelInterpolationPair(*contentInfo1,*contentInfo2,fTime,paramLevel,levelInterpolationMethod))
    {
      // Content records have the same time but different levels, so we need to level interpolation.

      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByLevelInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mParameterLevel = paramLevel;

      if (!fetchValues(qParam,[&]() { return mDataServerPtr->getGridValueListByLevelAndPointList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,query.mCoordinateType,query.mAreaCoordinates[0],areaInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList); },"getGridValueListByLevelAndPointList",{contentInfo1,contentInfo2},true,conversionByFunction,function,functionParams,forecastTime,valueList.mValueList))
        return false;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    if (contentLen == 2  &&  isTimeInterpolationPair(*contentInfo1,*contentInfo2,fTime,timeInterpolationMethod))
    {
      // Content records have different times, but most likely the same levels, so we need to do time interpolation.

      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

      if (!fetchValues(qParam,[&]() { return mDataServerPtr->getGridValueListByTimeAndPointList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,query.mCoordinateType,query.mAreaCoordinates[0],areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList); },"getGridValueListByTimeAndPointList",{contentInfo1,contentInfo2},true,conversionByFunction,function,functionParams,forecastTime,valueList.mValueList))
        return false;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }


    if (contentLen == 4  &&  isTimeLevelInterpolationQuad(*contentInfo1,*contentInfo2,*contentInfo3,*contentInfo4,fTime,paramLevel,timeInterpolationMethod,levelInterpolationMethod))
    {
      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation | ParameterValues::Flags::DataAvailableByLevelInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
      valueList.mFileId[2] = contentInfo3->mFileId;
      valueList.mMessageIndex[2] = contentInfo3->mMessageIndex;
      valueList.mFileId[3] = contentInfo4->mFileId;
      valueList.mMessageIndex[3] = contentInfo4->mMessageIndex;
      valueList.mParameterLevel = paramLevel;

      if (!fetchValues(qParam,[&]() { return mDataServerPtr->getGridValueListByTimeLevelAndPointList(0,
                contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,
                contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,
                fTime,paramLevel,query.mCoordinateType,query.mAreaCoordinates[0],areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList); },"getGridValueListByLevelAndPointList",{contentInfo1,contentInfo2,contentInfo3,contentInfo4},true,conversionByFunction,function,functionParams,forecastTime,valueList.mValueList))
        return false;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get circle values. */

bool ServiceImplementation::getCircleValues(
    const Query& /*query*/,
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
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getCircleValues()\n");


    short timeInterpolationMethod = getTimeInterpolationMethod(qParam,pInfo);

    //if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
    //  levelInterpolationMethod = pInfo.mLevelInterpolationMethod;

    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction = getValueConversion(pInfo,function,functionParams,modificationOperation,modificationParameters);

    std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
    time_t fTime = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,
        paramLevelId,paramLevel,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList);

    PRINT_DATA(mDebugLog, "         + Found %u content records\n", contentList->getLength());

    if (fTime == 0)
      return false;

    printContentList(*contentList);

    uint contentLen = contentList->getLength();

    if (contentLen == 0)
      return false;

    T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList->getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList->getContentInfoByIndex(3);

    setValueListContentInfo(valueList,pInfo,paramLevelId,forecastTime,analysisTime,*contentInfo1);
    valueList.mGenerationFlags = generationFlags;

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC != fTime /*|| timeInterpolationMethod == T::TimeInterpolationMethod::None || timeInterpolationMethod == T::TimeInterpolationMethod::Nearest*/)
      {
        // There is one content record in place, but its time does not match to
        // the requested forecast time. This is used for indicating that there
        // are content records available, but not for the requested time.
        // So, we should use this producer.

        setProducerWithoutValues(producerInfo,pInfo,forecastTime,newProducerId,valueList);
        return false;
      }

      // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

      valueList.mFlags = ParameterValues::Flags::DataAvailable;

      if (!fetchValues(qParam,[&]() { return mDataServerPtr->getGridValueListByCircle(0, contentInfo1->mFileId, contentInfo1->mMessageIndex, coordinateType, x, y, radius,modificationOperation,modificationParameters, valueList.mValueList); },"getGridValueListByCircle",{contentInfo1},false,conversionByFunction,function,functionParams,forecastTime,valueList.mValueList))
        return false;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    short levelInterpolationMethod = getLevelInterpolationMethod(paramLevelId,qParam.mLevelInterpolationMethod,qParam.mFlags);

    if (contentLen == 2  &&  isTimeInterpolationPair(*contentInfo1,*contentInfo2,fTime,timeInterpolationMethod))
    {
      // We did not find a grid with the exact forecast time, but we find grids that
      // are before and after the current forecast time. This means that we should do
      // some time interpolation.

      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

      // Fetching data from the grids.
      if (!fetchValues(qParam,[&]() { return mDataServerPtr->getGridValueListByTimeAndCircle(0,contentInfo1->mFileId,contentInfo1->mMessageIndex,contentInfo2->mFileId,contentInfo2->mMessageIndex,fTime,coordinateType,x,y,radius,timeInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList); },"getGridValueListByTimeAndCircle",{contentInfo1},true,conversionByFunction,function,functionParams,forecastTime,valueList.mValueList))
        return false;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    if (contentLen == 2  &&  isLevelInterpolationPair(*contentInfo1,*contentInfo2,fTime,paramLevel,levelInterpolationMethod))
    {
      // Content records have the same time but different levels, so we need to level interpolation.
      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByLevelInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
      valueList.mParameterLevel = paramLevel;

      if (!fetchValues(qParam,[&]() { return mDataServerPtr->getGridValueListByLevelAndCircle(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,coordinateType,x,y,radius,levelInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList); },"getGridValueListByLevelAndCircle",{contentInfo1,contentInfo2},true,conversionByFunction,function,functionParams,forecastTime,valueList.mValueList))
        return false;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }


    if (contentLen == 4  &&  isTimeLevelInterpolationQuad(*contentInfo1,*contentInfo2,*contentInfo3,*contentInfo4,fTime,paramLevel,timeInterpolationMethod,levelInterpolationMethod))
    {
      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation | ParameterValues::Flags::DataAvailableByLevelInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
      valueList.mFileId[2] = contentInfo3->mFileId;
      valueList.mMessageIndex[2] = contentInfo3->mMessageIndex;
      valueList.mFileId[3] = contentInfo4->mFileId;
      valueList.mMessageIndex[3] = contentInfo4->mMessageIndex;
      valueList.mParameterLevel = paramLevel;

      if (!fetchValues(qParam,[&]() { return mDataServerPtr->getGridValueListByTimeLevelAndCircle(0,
            contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,
            contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,
            fTime,paramLevel,coordinateType,x,y,radius,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList); },
            "getGridValueListByTimeLevelAndCircle",{contentInfo1,contentInfo2,contentInfo3,contentInfo4},true,conversionByFunction,function,functionParams,forecastTime,valueList.mValueList))
        return false;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the height level values of the grid points inside the polygon path. */

bool ServiceImplementation::getPolygonValuesByHeight(
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
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    GRIB2::GridDef_sptr def = Identification::gridDef.getGrib2DefinitionByGeometryId(producerGeometryId);
    if (!def)
      return false;

    T::Dimensions d = def->getGridDimensions();
    uint cols = d.nx();
    uint rows = d.ny();

    T::AreaCoordinates gridPolygonPath;
    switch (query.mCoordinateType)
    {
      case T::CoordinateTypeValue::UNKNOWN:
      case T::CoordinateTypeValue::LATLON_COORDINATES:
        gridPolygonPath = toGridPolygonPath(*def,query.mAreaCoordinates,false);
        break;

      case T::CoordinateTypeValue::GRID_COORDINATES:
        gridPolygonPath = query.mAreaCoordinates;
        break;

      case T::CoordinateTypeValue::ORIGINAL_COORDINATES:
        gridPolygonPath = toGridPolygonPath(*def,query.mAreaCoordinates,true);
        break;

      default:
        return true;
    }

    T::Coordinate_vec coordinates = getGridPointCoordinatesInside(*def,cols,rows,gridPolygonPath);

    return getPointValuesByHeight(query,query.mCoordinateType,coordinates,qParam,producerInfo,producerGeometryId,generationId,analysisTime,generationFlags,pInfo,forecastTime,
       paramLevelId,paramLevel,newProducerId,valueList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get polygon values. */

bool ServiceImplementation::getPolygonValues(
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
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getPolygonValues()\n");

    if (isMetricLevelQuery(qParam))
      return getPolygonValuesByHeight(query,qParam,producerInfo,producerGeometryId,generationId,analysisTime,generationFlags,pInfo,forecastTime,paramLevelId,paramLevel,newProducerId,valueList);

    short timeInterpolationMethod = getTimeInterpolationMethod(qParam,pInfo);

    //short levelInterpolationMethod = qParam.mLevelInterpolationMethod;
    //if (levelInterpolationMethod == T::LevelInterpolationMethod::Undefined)
    //  levelInterpolationMethod = pInfo.mLevelInterpolationMethod;

    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction = getValueConversion(pInfo,function,functionParams,modificationOperation,modificationParameters);

    std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
    time_t fTime = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,
        paramLevelId,paramLevel,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList);

    PRINT_DATA(mDebugLog, "         + Found %u content records\n", contentList->getLength());

    if (fTime == 0)
      return false;

    printContentList(*contentList);

    uint contentLen = contentList->getLength();

    if (contentLen == 0)
      return false;

    // We found content information close to the current forecast time

    T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList->getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList->getContentInfoByIndex(3);

    setValueListContentInfo(valueList,pInfo,paramLevelId,forecastTime,analysisTime,*contentInfo1);
    valueList.mGenerationFlags = generationFlags;

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC != fTime)
      {
        // There is one content record in place, but its time does not match to
        // the requested forecast time. This is used for indicating that there
        // are content records available, but not for the requested time.
        // So, we should use this producer.

        setProducerWithoutValues(producerInfo,pInfo,forecastTime,newProducerId,valueList);
        return false;
      }

      // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

      valueList.mFlags = ParameterValues::Flags::DataAvailable;

      if (!fetchValues(qParam,[&]() { return mDataServerPtr->getGridValueListByPolygonPath(0, contentInfo1->mFileId, contentInfo1->mMessageIndex, query.mCoordinateType, query.mAreaCoordinates,modificationOperation,modificationParameters,valueList.mValueList); },"getGridValueListByPolygonPath",{contentInfo1},false,conversionByFunction,function,functionParams,forecastTime,valueList.mValueList))
        return false;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }


    short levelInterpolationMethod = getLevelInterpolationMethod(paramLevelId,qParam.mLevelInterpolationMethod,qParam.mFlags);

    if (contentLen == 2  &&  isTimeInterpolationPair(*contentInfo1,*contentInfo2,fTime,timeInterpolationMethod))
    {
      // Content records have different times, but most likely the same levels, so we need to do time interpolation.

      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

      if (!fetchValues(qParam,[&]() { return mDataServerPtr->getGridValueListByTimeAndPolygonPath(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,query.mCoordinateType,query.mAreaCoordinates,timeInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList); },"getGridValueListByTimeAndPolygonPath",{contentInfo1,contentInfo2},true,conversionByFunction,function,functionParams,forecastTime,valueList.mValueList))
        return false;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    if (contentLen == 2  &&  isLevelInterpolationPair(*contentInfo1,*contentInfo2,fTime,paramLevel,levelInterpolationMethod))
    {
      // Content records have the same time but different levels, so we need to level interpolation.

      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByLevelInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
      valueList.mParameterLevel = paramLevel;

      if (!fetchValues(qParam,[&]() { return mDataServerPtr->getGridValueListByLevelAndPolygonPath(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,query.mCoordinateType,query.mAreaCoordinates,levelInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList); },"getGridValueListByLevelAndPolygonPath",{contentInfo1,contentInfo2},true,conversionByFunction,function,functionParams,forecastTime,valueList.mValueList))
        return false;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }


    if (contentLen == 4  &&  isTimeLevelInterpolationQuad(*contentInfo1,*contentInfo2,*contentInfo3,*contentInfo4,fTime,paramLevel,timeInterpolationMethod,levelInterpolationMethod))
    {
      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation | ParameterValues::Flags::DataAvailableByLevelInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
      valueList.mFileId[2] = contentInfo3->mFileId;
      valueList.mMessageIndex[2] = contentInfo3->mMessageIndex;
      valueList.mFileId[3] = contentInfo4->mFileId;
      valueList.mMessageIndex[3] = contentInfo4->mMessageIndex;
      valueList.mParameterLevel = paramLevel;

      if (!fetchValues(qParam,[&]() { return mDataServerPtr->getGridValueListByTimeLevelAndPolygonPath(0,
                contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,
                contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,
                fTime,paramLevel,query.mCoordinateType,query.mAreaCoordinates,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList.mValueList); },"getGridValueListByTimeLevelAndPolygonPath",{contentInfo1,contentInfo2,contentInfo3,contentInfo4},true,conversionByFunction,function,functionParams,forecastTime,valueList.mValueList))
        return false;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get streamline values. */

bool ServiceImplementation::getStreamlineValues(
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
    //uchar locationType,
    //uchar coordinateType,
    //T::Coordinate_vec& gridCoordinates,
    //T::AttributeList& queryAttributeList,
    T::ProducerId& newProducerId,
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getStreamlineValues()\n");

    short areaInterpolationMethod = getAreaInterpolationMethod(qParam,pInfo);

    short timeInterpolationMethod = getTimeInterpolationMethod(qParam,pInfo);

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

    printContentList(*contentList);

    uint contentLen = contentList->getLength();
    if (contentLen == 0)
      return false;

    T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList->getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList->getContentInfoByIndex(3);

    setValueListContentInfo(valueList,pInfo,paramLevelId,forecastTime,analysisTime,*contentInfo1);
    valueList.mGenerationFlags = generationFlags;

    query.mAttributeList.setAttribute("grid.timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
    query.mAttributeList.setAttribute("grid.areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));


    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC != fTime)
      {
        // There is one content record in place, but its time does not match to
        // the requested forecast time. This is used for indicating that there
        // are content records available, but not for the requested time.
        // So, we should use this producer.

        setProducerWithoutValues(producerInfo,pInfo,forecastTime,newProducerId,valueList);
        return false;
      }

      // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

      valueList.mFlags = ParameterValues::Flags::DataAvailable;

      fetchContours(qParam,[&]() {
          return fetchByLocationType(qParam.mLocationType,gridWidthStr,gridHeightStr,
            [&]() { return mDataServerPtr->getGridStreamlinesByGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&](uint width,uint height) { return mDataServerPtr->getGridStreamlinesByGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,width,height,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&]() { return mDataServerPtr->getGridStreamlines(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); }); },
        "getGridStreamlinesByGeometry / getGridStreamlinesByGrid / getGridStreamlines",{contentInfo1});

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    if (contentLen == 2  &&  isTimeInterpolationPair(*contentInfo1,*contentInfo2,fTime,timeInterpolationMethod))
    {
      // We did not find a grid with the exact forecast time, but we find grids that
      // are before and after the current forecast time. This means that we should do
      // some time interpolation.

      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

      fetchContours(qParam,[&]() {
          return fetchByLocationType(qParam.mLocationType,gridWidthStr,gridHeightStr,
            [&]() { return mDataServerPtr->getGridStreamlinesByTimeAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&](uint width,uint height) { return mDataServerPtr->getGridStreamlinesByTimeAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,width,height,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&]() { return mDataServerPtr->getGridStreamlinesByTime(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); }); },
        "getGridStreamlinesByTimeAndGeometry / getGridStreamlinesByTimeAndGrid / getGridStreamlinesByTime",{contentInfo1,contentInfo2});

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    if (contentLen == 2  &&  contentInfo1->mParameterLevel < paramLevel && contentInfo2->mParameterLevel > paramLevel  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
    {
      // We did not find a grid with the exact level but we find grids that
      // are before and after the current level. This means that we should do
      // some level interpolation.

      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByLevelInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

      fetchContours(qParam,[&]() {
          return fetchByLocationType(qParam.mLocationType,gridWidthStr,gridHeightStr,
            [&]() { return mDataServerPtr->getGridStreamlinesByLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&](uint width,uint height) { return mDataServerPtr->getGridStreamlinesByLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,width,height,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&]() { return mDataServerPtr->getGridStreamlinesByLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); }); },
        "getGridStreamlinesByLevelAndGeometry / getGridStreamlinesByLevelAndGrid / getGridStreamlinesByLevel",{contentInfo1,contentInfo2});

      valueList.mParameterLevel = paramLevel;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    if (contentLen == 4  &&  isTimeLevelInterpolationQuad(*contentInfo1,*contentInfo2,*contentInfo3,*contentInfo4,fTime,paramLevel,timeInterpolationMethod,levelInterpolationMethod))
    {
      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation | ParameterValues::Flags::DataAvailableByLevelInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
      valueList.mFileId[2] = contentInfo3->mFileId;
      valueList.mMessageIndex[2] = contentInfo3->mMessageIndex;
      valueList.mFileId[3] = contentInfo4->mFileId;
      valueList.mMessageIndex[3] = contentInfo4->mMessageIndex;

      fetchContours(qParam,[&]() {
          return fetchByLocationType(qParam.mLocationType,gridWidthStr,gridHeightStr,
            [&]() { return mDataServerPtr->getGridStreamlinesByTimeLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&](uint width,uint height) { return mDataServerPtr->getGridStreamlinesByTimeLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,width,height,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&]() { return mDataServerPtr->getGridStreamlinesByTimeAndLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); }); },
        "getGridStreamlinesByTimeLevelAndGeometry / getGridStreamlinesByTimeLevelAndGrid / getGridStreamlinesByTimeAndLevel",{contentInfo1,contentInfo2,contentInfo3,contentInfo4});

      valueList.mParameterLevel = paramLevel;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get isoline values. */

bool ServiceImplementation::getIsolineValues(
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
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getIsolineValues()\n");

    // ### Converting contour values if necessary:

    short areaInterpolationMethod = getAreaInterpolationMethod(qParam,pInfo);

    short timeInterpolationMethod = getTimeInterpolationMethod(qParam,pInfo);
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

    printContentList(*contentList);

    uint contentLen = contentList->getLength();
    if (contentLen == 0)
      return false;

    T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList->getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList->getContentInfoByIndex(3);

    setValueListContentInfo(valueList,pInfo,paramLevelId,forecastTime,analysisTime,*contentInfo1);
    valueList.mGenerationFlags = generationFlags;

    query.mAttributeList.setAttribute("grid.timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
    query.mAttributeList.setAttribute("grid.areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));


    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC != fTime)
      {
        // There is one content record in place, but its time does not match to
        // the requested forecast time. This is used for indicating that there
        // are content records available, but not for the requested time.
        // So, we should use this producer.

        setProducerWithoutValues(producerInfo,pInfo,forecastTime,newProducerId,valueList);
        return false;
      }

      // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

      valueList.mFlags = ParameterValues::Flags::DataAvailable;

      fetchContours(qParam,[&]() {
          return fetchByLocationType(qParam.mLocationType,gridWidthStr,gridHeightStr,
            [&]() { return mDataServerPtr->getGridIsolinesByGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&](uint width,uint height) { return mDataServerPtr->getGridIsolinesByGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourValues,width,height,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&]() { return mDataServerPtr->getGridIsolines(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); }); },
        "getGridIsolinesByGeometry / getGridIsolinesByGrid / getGridIsolines",{contentInfo1});

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    short levelInterpolationMethod = getLevelInterpolationMethod(paramLevelId,qParam.mLevelInterpolationMethod,qParam.mFlags);
    query.mAttributeList.setAttribute("grid.levelInterpolationMethod",Fmi::to_string(levelInterpolationMethod));

    if (contentLen == 2  &&  isTimeInterpolationPair(*contentInfo1,*contentInfo2,fTime,timeInterpolationMethod))
    {
      // We did not find a grid with the exact forecast time, but we find grids that
      // are before and after the current forecast time. This means that we should do
      // some time interpolation.

      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

      fetchContours(qParam,[&]() {
          return fetchByLocationType(qParam.mLocationType,gridWidthStr,gridHeightStr,
            [&]() { return mDataServerPtr->getGridIsolinesByTimeAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&](uint width,uint height) { return mDataServerPtr->getGridIsolinesByTimeAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourValues,width,height,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&]() { return mDataServerPtr->getGridIsolinesByTime(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); }); },
        "getGridIsolinesByTimeAndGeometry / getGridIsolinesByTimeAndGrid / getGridIsolinesByTime",{contentInfo1,contentInfo2});

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    if (contentLen == 2  &&  contentInfo1->mParameterLevel < paramLevel && contentInfo2->mParameterLevel > paramLevel  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
    {
      // We did not find a grid with the exact level but we find grids that
      // are before and after the current level. This means that we should do
      // some level interpolation.

      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByLevelInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

      fetchContours(qParam,[&]() {
          return fetchByLocationType(qParam.mLocationType,gridWidthStr,gridHeightStr,
            [&]() { return mDataServerPtr->getGridIsolinesByLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&](uint width,uint height) { return mDataServerPtr->getGridIsolinesByLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourValues,width,height,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&]() { return mDataServerPtr->getGridIsolinesByLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); }); },
        "getGridIsolinesByLevelAndGeometry / getGridIsolinesByLevelAndGrid / getGridIsolinesByLevel",{contentInfo1,contentInfo2});

      valueList.mParameterLevel = paramLevel;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    if (contentLen == 4  &&  isTimeLevelInterpolationQuad(*contentInfo1,*contentInfo2,*contentInfo3,*contentInfo4,fTime,paramLevel,timeInterpolationMethod,levelInterpolationMethod))
    {
      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation | ParameterValues::Flags::DataAvailableByLevelInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
      valueList.mFileId[2] = contentInfo3->mFileId;
      valueList.mMessageIndex[2] = contentInfo3->mMessageIndex;
      valueList.mFileId[3] = contentInfo4->mFileId;
      valueList.mMessageIndex[3] = contentInfo4->mMessageIndex;

      fetchContours(qParam,[&]() {
          return fetchByLocationType(qParam.mLocationType,gridWidthStr,gridHeightStr,
            [&]() { return mDataServerPtr->getGridIsolinesByTimeLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&](uint width,uint height) { return mDataServerPtr->getGridIsolinesByTimeLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourValues,width,height,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&]() { return mDataServerPtr->getGridIsolinesByTimeAndLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); }); },
        "getGridIsolinesByTimeLevelAndGeometry / getGridIsolinesByTimeLevelAndGrid / getGridIsolinesByTimeAndLevel",{contentInfo1,contentInfo2,contentInfo3,contentInfo4});

      valueList.mParameterLevel = paramLevel;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get isoband values. */

bool ServiceImplementation::getIsobandValues(
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
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getIsobandValues()\n");

    short areaInterpolationMethod = getAreaInterpolationMethod(qParam,pInfo);

    short timeInterpolationMethod = getTimeInterpolationMethod(qParam,pInfo);
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

    printContentList(*contentList);

    uint contentLen = contentList->getLength();
    if (contentLen == 0)
      return false;

    T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
    T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);
    T::ContentInfo* contentInfo3 = contentList->getContentInfoByIndex(2);
    T::ContentInfo* contentInfo4 = contentList->getContentInfoByIndex(3);

    setValueListContentInfo(valueList,pInfo,paramLevelId,forecastTime,analysisTime,*contentInfo1);
    valueList.mGenerationFlags = generationFlags;

    query.mAttributeList.setAttribute("grid.timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
    query.mAttributeList.setAttribute("grid.areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));

    if (contentLen == 1)
    {
      if (contentInfo1->mForecastTimeUTC != fTime)
      {
        // There is one content record in place, but its time does not match to
        // the requested forecast time. This is used for indicating that there
        // are content records available, but not for the requested time.
        // So, we should use this producer.

        setProducerWithoutValues(producerInfo,pInfo,forecastTime,newProducerId,valueList);
        return false;
      }

      // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

      valueList.mFlags = ParameterValues::Flags::DataAvailable;

      fetchContours(qParam,[&]() {
          return fetchByLocationType(qParam.mLocationType,gridWidthStr,gridHeightStr,
            [&]() { return mDataServerPtr->getGridIsobandsByGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourLowValues,newContourHighValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&](uint width,uint height) { return mDataServerPtr->getGridIsobandsByGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourLowValues,newContourHighValues,width,height,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&]() { return mDataServerPtr->getGridIsobands(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,newContourLowValues,newContourHighValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); }); },
        "getGridIsobandsByGeometry / getGridIsobandsByGrid / getGridIsobands",{contentInfo1});

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    short levelInterpolationMethod = getLevelInterpolationMethod(paramLevelId,qParam.mLevelInterpolationMethod,qParam.mFlags);
    query.mAttributeList.setAttribute("grid.levelInterpolationMethod",Fmi::to_string(levelInterpolationMethod));

    if (contentLen == 2  &&  isTimeInterpolationPair(*contentInfo1,*contentInfo2,fTime,timeInterpolationMethod))
    {
      // We did not find a grid with the exact forecast time, but we find grids that
      // are before and after the current forecast time. This means that we should do
      // some time interpolation.

      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

      fetchContours(qParam,[&]() {
          return fetchByLocationType(qParam.mLocationType,gridWidthStr,gridHeightStr,
            [&]() { return mDataServerPtr->getGridIsobandsByTimeAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourLowValues,newContourHighValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&](uint width,uint height) { return mDataServerPtr->getGridIsobandsByTimeAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourLowValues,newContourHighValues,width,height,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&]() { return mDataServerPtr->getGridIsobandsByTime(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,newContourLowValues,newContourHighValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); }); },
        "getGridIsobandsByTimeAndGeometry / getGridIsobandsByTimeAndGrid / getGridIsobandsByTime",{contentInfo1,contentInfo2});

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    if (contentLen == 2  &&  contentInfo1->mParameterLevel < paramLevel && contentInfo2->mParameterLevel > paramLevel  &&  levelInterpolationMethod != T::LevelInterpolationMethod::Forbidden)
    {
      // We did not find a grid with the exact level but we find grids that
      // are before and after the current level. This means that we should do
      // some level interpolation.

      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByLevelInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

      fetchContours(qParam,[&]() {
          return fetchByLocationType(qParam.mLocationType,gridWidthStr,gridHeightStr,
            [&]() { return mDataServerPtr->getGridIsobandsByLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourLowValues,newContourHighValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&](uint width,uint height) { return mDataServerPtr->getGridIsobandsByLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourLowValues,newContourHighValues,width,height,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&]() { return mDataServerPtr->getGridIsobandsByLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,paramLevel,newContourLowValues,newContourHighValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); }); },
        "getGridIsobandsByLevelAndGeometry / getGridIsobandsByLevelAndGrid / getGridIsobandsByLevel",{contentInfo1,contentInfo2});

      valueList.mParameterLevel = paramLevel;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    if (contentLen == 4  &&  isTimeLevelInterpolationQuad(*contentInfo1,*contentInfo2,*contentInfo3,*contentInfo4,fTime,paramLevel,timeInterpolationMethod,levelInterpolationMethod))
    {
      valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation | ParameterValues::Flags::DataAvailableByLevelInterpolation;
      valueList.mFileId[1] = contentInfo2->mFileId;
      valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;
      valueList.mFileId[2] = contentInfo3->mFileId;
      valueList.mMessageIndex[2] = contentInfo3->mMessageIndex;
      valueList.mFileId[3] = contentInfo4->mFileId;
      valueList.mMessageIndex[3] = contentInfo4->mMessageIndex;

      fetchContours(qParam,[&]() {
          return fetchByLocationType(qParam.mLocationType,gridWidthStr,gridHeightStr,
            [&]() { return mDataServerPtr->getGridIsobandsByTimeLevelAndGeometry(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourLowValues,newContourHighValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&](uint width,uint height) { return mDataServerPtr->getGridIsobandsByTimeLevelAndGrid(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourLowValues,newContourHighValues,width,height,query.mAreaCoordinates[0],query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); },
            [&]() { return mDataServerPtr->getGridIsobandsByTimeAndLevel(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,contentInfo3->mFileId, contentInfo3->mMessageIndex,contentInfo4->mFileId, contentInfo4->mMessageIndex,fTime,paramLevel,newContourLowValues,newContourHighValues,query.mAttributeList,modificationOperation,modificationParameters,valueList.mValueData); }); },
        "getGridIsobandsByTimeLevelAndGeometry / getGridIsobandsByTimeLevelAndGrid / getGridIsobandsByTimeAndLevel",{contentInfo1,contentInfo2,contentInfo3,contentInfo4});

      valueList.mParameterLevel = paramLevel;

      setDefaultPrecision(qParam,pInfo);

      return true;
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







/*! \brief Query Server: Add the levels of the parameter contents of the generation and geometry. Returns false on a content server error. */

bool ServiceImplementation::getContentLevels(const T::ProducerInfo& producerInfo,T::GenerationId generationId,T::GeometryId producerGeometryId,const ParameterMapping& pInfo,T::ParamLevelId paramLevelId,std::vector<T::ParamLevel>& parameterLevels)
{
  try
  {
    std::set<T::ParamLevel> levels;
    if (mContentServerPtr->getContentLevelListByParameterGenerationGeometryAndLevelId(producerInfo.mProducerId,generationId,producerGeometryId,pInfo.mParameterKey,paramLevelId,levels) != 0)
    {
      return false; // Error
    }

    for (int level : levels)
    {
      parameterLevels.push_back(level);
      std::cout << "LEVEL " << level << "\n";
    }

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get vertical value vectors. */

bool ServiceImplementation::getVerticalValueVectors(
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
    ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog, "getVerticalValueVectors()\n");

    short areaInterpolationMethod = getAreaInterpolationMethod(qParam,pInfo);

    short timeInterpolationMethod = getTimeInterpolationMethod(qParam,pInfo);

    uint modificationOperation = 0;
    double_vec modificationParameters;
    std::string function;
    std::vector<std::string> functionParams;
    bool conversionByFunction = getValueConversion(pInfo,function,functionParams,modificationOperation,modificationParameters);


    if (parameterLevels.empty()  &&  !getContentLevels(producerInfo,generationId,producerGeometryId,pInfo,paramLevelId,parameterLevels))
      return false; // Error

    if (parameterLevels.empty())
    {
      return false; // No Levels
    }


    for (int & parameterLevel : parameterLevels)
    {
      std::shared_ptr<T::ContentInfoList> contentList = std::make_shared<T::ContentInfoList>();
      time_t fTime = getContentList(producerInfo,producerGeometryId,generationId,pInfo,forecastTime,
          paramLevelId,parameterLevel,qParam.mForecastType,qParam.mForecastNumber,qParam.mFlags,contentList);

      PRINT_DATA(mDebugLog, "         + Found %u content records on level %d (fTime = %ld)\n", contentList->getLength(),parameterLevel,fTime);

      //if (fTime == 0)
      //  return false;

      printContentList(*contentList);

      uint contentLen = contentList->getLength();
      //if (contentLen == 0)
      //  return false;

      T::ContentInfo* contentInfo1 = contentList->getContentInfoByIndex(0);
      T::ContentInfo* contentInfo2 = contentList->getContentInfoByIndex(1);

      setLevelValueListContentInfo(valueList,pInfo,paramLevelId,forecastTime,analysisTime,generationFlags,parameterLevel,contentInfo1);

      if (contentLen == 1  &&  contentInfo1->mForecastTimeUTC != fTime)
      {
        // There is one content record in place, but its time does not match to
        // the requested forecast time. This is used for indicating that there
        // are content records available, but not for the requested time.
        // So, we should use this producer.

        setProducerWithoutValues(producerInfo,pInfo,forecastTime,newProducerId,valueList);
        return false;
      }

      if (contentLen == 1)
      {
        // We found a grid which forecast time is exactly the same as the requested forecast time or time interpolation enables the selection.

        valueList.mFlags = ParameterValues::Flags::DataAvailable;

        T::GridValueList valList;
        if (!fetchValues(qParam,[&]() { return mDataServerPtr->getGridValueListByPointList(0, contentInfo1->mFileId, contentInfo1->mMessageIndex,query.mCoordinateType,query.mAreaCoordinates[0],areaInterpolationMethod,modificationOperation,modificationParameters,valList); },"getGridValueListByPointList",{contentInfo1},true,conversionByFunction,function,functionParams,forecastTime,valList))
          return false;

        appendGridValues(valList,valueList.mValueVector);

        setDefaultPrecision(qParam,pInfo);

        //return true;
      }


      //short levelInterpolationMethod = getLevelInterpolationMethod(paramLevelId,qParam.mLevelInterpolationMethod,qParam.mFlags);

      if (contentLen == 2  &&  contentInfo1->mParameterLevel == parameterLevel  &&  contentInfo2->mParameterLevel == parameterLevel  &&
          isTimeInterpolationPair(*contentInfo1,*contentInfo2,fTime,timeInterpolationMethod))
      {
        // Content records have different times, but the same levels, so we need to do time interpolation.

        valueList.mFlags = ParameterValues::Flags::DataAvailable | ParameterValues::Flags::DataAvailableByTimeInterpolation;
        valueList.mFileId[1] = contentInfo2->mFileId;
        valueList.mMessageIndex[1] = contentInfo2->mMessageIndex;

        T::GridValueList valList;
        if (!fetchValues(qParam,[&]() { return mDataServerPtr->getGridValueListByTimeAndPointList(0,contentInfo1->mFileId, contentInfo1->mMessageIndex,contentInfo2->mFileId, contentInfo2->mMessageIndex,fTime,query.mCoordinateType,query.mAreaCoordinates[0],areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters,valList); },"getGridValueListByTimeAndPointList",{contentInfo1,contentInfo2},true,conversionByFunction,function,functionParams,forecastTime,valList))
          return false;

        appendGridValues(valList,valueList.mValueVector);

        setDefaultPrecision(qParam,pInfo);

        //return true;
      }
    }
    return !valueList.mValueVector.empty();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






/*! \brief Query Server: Print the arguments of the forecast time grid value request to the debug log (if enabled). */

void ServiceImplementation::printGridValueRequest(const GridValueRequest& r,const Producer_vec& producers,const std::set<T::GeometryId>& geometryIdList)
{
  try
  {
    if (mDebugLog == nullptr ||  !mDebugLog->isEnabled())
      return;

    std::string text = "\nMETHOD getGridValues()\n";
    text += fmt::format("  - producers                : {} items\n", producers.size());
    for (auto it = producers.begin(); it != producers.end(); ++it)
      text += fmt::format("    * {}:{}\n", it->first, it->second);
    text += fmt::format("  - geometryIdList           : {} items\n", geometryIdList.size());
    for (int it : geometryIdList)
      text += fmt::format("    * {}\n",it);
    text += fmt::format("  - producerId               : {}\n", r.producerId);
    text += fmt::format("  - analysisTime             : {}\n", r.analysisTime);
    text += fmt::format("  - maxAnalysisTime          : {}\n", r.maxAnalysisTime);
    text += fmt::format("  - generationFlags          : {}\n", r.generationFlags);
    text += fmt::format("  - parameterKey             : {}\n", r.parameterKey);
    text += fmt::format("  - queryFlags               : {:04x}\n", r.queryFlags);
    text += fmt::format("  - forecastTime             : {}\n", utcTimeFromTimeT(r.forecastTime));
    text += fmt::format("  - forecastTimeT            : {}\n", r.forecastTime);
    text += fmt::format("  - timeMatchRequired        : {}\n", static_cast<int>(r.timeMatchRequired));

    PRINT_DATA(mDebugLog, "%s", text.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Print the reason why the producer is not valid to the debug log. */

void ServiceImplementation::printInvalidProducer(const std::string& producerName,bool producerOk)
{
  try
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
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Return false if the data is requested from an area and the concatenated producers of the
           producer have different geometries (the data concatenation is not accepted then). */

bool ServiceImplementation::isAreaProducerAccepted(std::size_t producerCount,const QueryParameter& qParam,const std::string& producerName,T::GeometryId producerGeometryId)
{
  try
  {
    if (producerCount <= 1 || (qParam.mLocationType != QueryParameter::LocationType::Polygon && qParam.mLocationType != QueryParameter::LocationType::Circle && qParam.mLocationType != QueryParameter::LocationType::Path))
      return true;

    // We do not accept data concatenation if the data is requested from an area and
    // if geometries are different.

    std::string pStr = toUpperString(producerName) + ":" + std::to_string(producerGeometryId);

    auto rec = mProducerConcatMap.find(pStr);
    if (rec == mProducerConcatMap.end()  ||  rec->second.empty())
      return true;

    for (auto gg = rec->second.begin(); gg != rec->second.end(); ++gg)
    {
      if (producerGeometryId != gg->second)
        return false;
    }
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the parameter mappings of the parameter for the producer and the geometry (by the level of
           the query parameter if it is defined). An FMI-key parameter without mappings gets a mapping of its own. */

ParameterMapping_vec_sptr ServiceImplementation::getGridValueMappings(const T::ProducerInfo& producerInfo,const std::string& parameterKey,std::size_t parameterHash,T::GeometryId producerGeometryId,const QueryParameter& qParam)
{
  try
  {
    ParameterMapping_vec_sptr mappings;
    if (qParam.mParameterLevelId > 0 || qParam.mParameterLevel > 0)
    {
      getParameterMappings(producerInfo.mName,producerInfo.mProducerId,parameterKey,parameterHash,producerGeometryId, qParam.mParameterLevelId, qParam.mParameterLevel, false, mappings);
      if (mappings->empty())
      {
        // Getting a mapping by using the levelId. This probably returns all levels belonging
        // to the current levelId. We need just one for mapping.
        ParameterMapping_vec_sptr tmpMappings;
        getParameterMappings(producerInfo.mName, producerInfo.mProducerId, parameterKey, parameterHash, producerGeometryId, qParam.mParameterLevelId, -1,false, tmpMappings);
        if (!tmpMappings->empty())
        {
          // The mapping vector may be shared by the mapping cache. It is not modified.
          mappings = std::make_shared<ParameterMapping_vec>();
          mappings->emplace_back((*tmpMappings)[0]);
        }
      }
    }
    else
    {
      getParameterMappings(producerInfo.mName, producerInfo.mProducerId, parameterKey, parameterHash, producerGeometryId, true, mappings);
    }

    if (mappings->empty() &&  strncasecmp(parameterKey.c_str(),"FMI-",4) == 0)
    {
      ParameterMapping mp;
      mp.mProducerName = producerInfo.mName;
      mp.mParameterName = parameterKey;
      mp.mParameterKeyType = T::ParamKeyTypeValue::FMI_ID;
      mp.mParameterKey = parameterKey.c_str() + 4;
      mp.mGeometryId = producerGeometryId;
      mp.mParameterLevelId = qParam.mParameterLevelId;
      mp.mParameterLevel = qParam.mParameterLevel;

      // The mapping vector may be shared by the mapping cache. It is not modified.
      mappings = std::make_shared<ParameterMapping_vec>();
      mappings->emplace_back(mp);
    }

    if (mappings->empty())
      PRINT_DATA(mDebugLog, "    - No parameter mappings '%s:%s:%d:%d:%d' found!\n", producerInfo.mName.c_str(), parameterKey.c_str(), producerGeometryId, qParam.mParameterLevelId, qParam.mParameterLevel);

    return mappings;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the generation requested by the analysis time or by the latest / oldest generation flags
           (nullptr if no generation is requested). */

T::GenerationInfo* ServiceImplementation::getRequestedGenerationInfo(const CacheEntry& cacheEntry,const std::string& analysisTime,uint queryFlags,bool acceptNotReadyGenerations)
{
  try
  {
    T::GenerationInfo* origGenerationInfo = nullptr;
    if (!analysisTime.empty())
    {
      origGenerationInfo = cacheEntry.generationInfoList->getGenerationInfoByAnalysisTime(analysisTime);
      if (mDebugLog != nullptr &&  mDebugLog->isEnabled()  &&  origGenerationInfo)
      {
        PRINT_DATA(mDebugLog, "    - generation found by analysis time (%s)!\n",analysisTime.c_str());
      }
    }

    if (queryFlags & Query::Flags::LatestGeneration)
    {
      if (acceptNotReadyGenerations)
        origGenerationInfo = cacheEntry.generationInfoList->getLastGenerationInfoByAnalysisTime();
      else
        origGenerationInfo = cacheEntry.generationInfoList->getLastGenerationInfoByAnalysisTime(T::GenerationInfo::Status::Ready);
    }

    if (queryFlags & Query::Flags::OldestGeneration)
    {
      if (acceptNotReadyGenerations)
        origGenerationInfo = cacheEntry.generationInfoList->getFirstGenerationInfoByAnalysisTime();
      else
        origGenerationInfo = cacheEntry.generationInfoList->getFirstGenerationInfoByAnalysisTime(T::GenerationInfo::Status::Ready);
    }

    return origGenerationInfo;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Return true if there are climatological (or global) mappings. */

bool ServiceImplementation::hasClimatologicalMappings(const ParameterMapping_vec& mappings)
{
  try
  {
    for (const auto& pInfo : mappings)
    {
      if (pInfo.mGroupFlags & (ParameterMapping::GroupFlags::climatology | ParameterMapping::GroupFlags::global))
      {
        PRINT_DATA(mDebugLog, "    - Climatological mappings found!\n");
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





/*! \brief Query Server: Return true if the generation (of the index in the analysis times) is selected by the analysis
           time of the request, or (without an analysis time) by the generation flags. */

bool ServiceImplementation::isGenerationSelected(const GridValueRequest& r,const std::vector<std::string>& analysisTimes,uint g,UInt64 gflags)
{
  try
  {
    if (r.analysisTime.empty())
      return (r.generationFlags == 0 || (r.generationFlags & gflags) != 0);

    uint gLen = analysisTimes.size();

    if (r.analysisTime == analysisTimes[g])
    {
      PRINT_DATA(mDebugLog, "      - Analysis time equals\n");
      return true;
    }

    if ((r.queryFlags & Query::Flags::AnalysisTimeMatchRequired) != 0)
      return false;

    if (g == 0  &&  analysisTimes[g] < r.analysisTime)
      return true;

    if ((g+1) == gLen  &&  analysisTimes[g] > r.analysisTime)
      return true;

    return (analysisTimes[g] > r.analysisTime  &&  (g+1) < gLen  &&  analysisTimes[g+1] < r.analysisTime);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Return true if the geometry of the generation (or the generation, if the geometry status is
           not checked) is ready. */

bool ServiceImplementation::isGenerationReady(const T::GenerationInfo& generationInfo,T::GeometryId producerGeometryId)
{
  try
  {
    // Checking if the geometry is ready.
    if (mCheckGeometryStatus)
    {
      if (!isGeometryReady(generationInfo.mGenerationId,producerGeometryId,0))
      {
        PRINT_DATA(mDebugLog, "      - Geometry is not ready\n");
        return false;
      }
      return true;
    }

    if (generationInfo.mStatus != T::GenerationInfo::Status::Ready)
    {
      PRINT_DATA(mDebugLog, "      - Generation is not ready\n");
      return false;
    }
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Check that the selected generation can be used: it is not deleted soon, it (or its geometry) is
           ready, its analysis time is not after the maximum analysis time, and (with a producer) its time range
           contains the forecast time. Returns the validity of the generation. */

bool ServiceImplementation::isGenerationUsable(const GridValueRequest& r,T::GenerationInfo *generationInfo,T::GeometryId producerGeometryId,bool climatologicalMappings,bool generationValid)
{
  try
  {
    // ### Checking if the generation is going to be deleted in few minutes

    if (generationInfo == nullptr || (generationInfo != nullptr && generationInfo->mDeletionTime > 0 && generationInfo->mDeletionTime < r.requiredAccessTime))
    {
      PRINT_DATA(mDebugLog, "      - Generation deletion time is close\n");
      return false;
    }

    if (!generationValid)
      return false;

    if (!r.acceptNotReadyGenerations  &&  !isGenerationReady(*generationInfo,producerGeometryId))
      return false;

    if (!r.maxAnalysisTime.empty()  &&  generationInfo->mAnalysisTime > r.maxAnalysisTime)
    {
      PRINT_DATA(mDebugLog, "      - Generation analysisTime > maxAnalysisTime (%s)\n",r.maxAnalysisTime.c_str());
      return false;
    }

    if (climatologicalMappings  ||  r.producerId == 0)
      return true;

    // Checking if the requested forecast time is outside of the generation's time range.

    if (generationInfo->mContentStartTime == 0 || generationInfo->mContentEndTime == 0)
      getGenerationTimeRangeByProducerAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,generationInfo->mContentStartTime,generationInfo->mContentEndTime);

    if (r.forecastTime < generationInfo->mContentStartTime || r.forecastTime > generationInfo->mContentEndTime)
    {
      PRINT_DATA(mDebugLog, "      - Requested time is outside of the generation's time range\n");
      return false;
    }

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the grid values of the producer and the geometry: go through the generations from the
           newest to the oldest and the parameter mappings, until the values are found. Returns true if the values
           were found. */

bool ServiceImplementation::getProducerGridValues(GridValueRequest& r,const std::string& producerName,T::GeometryId producerGeometryId,const T::ProducerInfo& producerInfo)
{
  try
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
    if (r.queryFlags & Query::Flags::ReverseGenerationFlags)
    {
      cacheEntry->generationInfoList->getAnalysisTimes(analysisTimesVec,false);
      analysisTimes = &analysisTimesVec;
    }


    // Going through all the parameter mapping files, until we find a match.

    ParameterMapping_vec_sptr mappings = getGridValueMappings(producerInfo,r.parameterKey,r.parameterHash,producerGeometryId,r.qParam);

    T::GenerationInfo* origGenerationInfo = getRequestedGenerationInfo(*cacheEntry,r.analysisTime,r.queryFlags,r.acceptNotReadyGenerations);

    if (mappings->empty())
      return false;

    bool climatologicalMappings = hasClimatologicalMappings(*mappings);

    return getGenerationsGridValues(r,producerName,producerGeometryId,producerInfo,*cacheEntry,*analysisTimes,origGenerationInfo,*mappings,climatologicalMappings);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Print the analysis time of the generation (or of the requested generation) to the debug log. */

void ServiceImplementation::printGeneration(const std::string& analysisTime,const std::vector<std::string>& analysisTimes,uint g,bool requestedGeneration)
{
  try
  {
    if (requestedGeneration)
    {
      PRINT_DATA(mDebugLog, "    * %s (requested generation)\n", analysisTime.c_str());
    }
    else
    {
      PRINT_DATA(mDebugLog, "    * %s\n", analysisTimes[g].c_str());
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Go through the generations from the newest to the oldest (or the requested generation only)
           and the parameter mappings, until the values are found. Returns true if the values were found. */

bool ServiceImplementation::getGenerationsGridValues(GridValueRequest& r,const std::string& producerName,T::GeometryId producerGeometryId,const T::ProducerInfo& producerInfo,const CacheEntry& cacheEntry,const std::vector<std::string>& analysisTimes,T::GenerationInfo* origGenerationInfo,ParameterMapping_vec& mappings,bool climatologicalMappings)
{
  try
  {
    uint gLen = analysisTimes.size();

    PRINT_DATA(mDebugLog, "  - Going through the generations from the newest to the oldest (generationCount = %u).\n",gLen);

    for (uint g = 0; g < gLen; g++)
    {
      if (origGenerationInfo != nullptr)
        g = gLen; // Making sure that the loop is executed only once

      printGeneration(r.analysisTime,analysisTimes,g,origGenerationInfo != nullptr);

      UInt64 gflags = generationFlag(g);

      bool generationValid = (origGenerationInfo != nullptr  ||  isGenerationSelected(r,analysisTimes,g,gflags));

      if (!generationValid  &&  climatologicalMappings)
        generationValid = true;

      T::GenerationInfo *generationInfo = origGenerationInfo;
      if (origGenerationInfo == nullptr)
        generationInfo = cacheEntry.generationInfoList->getGenerationInfoByAnalysisTime(analysisTimes[g]);

      if (!isGenerationUsable(r,generationInfo,producerGeometryId,climatologicalMappings,generationValid))
        continue;

      r.producerStr = toUpperString(producerInfo.mName) + ":" + std::to_string(producerGeometryId);

      PRINT_DATA(mDebugLog, "      - Going through the parameter mappings\n");
      for (auto& pInfo : mappings)
      {
        if (getMappingGridValues(r,producerName,producerGeometryId,producerInfo,*generationInfo,gflags,pInfo))
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





/*! \brief Query Server: Get the grid values of the parameter mapping by the query type. Returns true if the values were
           found (or the search must end). */

bool ServiceImplementation::getMappingGridValues(GridValueRequest& r,const std::string& producerName,T::GeometryId producerGeometryId,const T::ProducerInfo& producerInfo,const T::GenerationInfo& generationInfo,UInt64 gflags,ParameterMapping& pInfo)
{
  try
  {
    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      pInfo.print(stream, 5, 0);
      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
    }
    short areaInterpolation = r.qParam.mAreaInterpolationMethod;
    if (areaInterpolation == T::AreaInterpolationMethod::Undefined)
      areaInterpolation = pInfo.mAreaInterpolationMethod;

    /*
    short timeInterpolation = qParam.mTimeInterpolationMethod;
    if (timeInterpolation == T::TimeInterpolationMethod::Undefined)
      timeInterpolation = pInfo->mTimeInterpolationMethod;

    short levelInterpolation = qParam.mLevelInterpolationMethod;
    if (levelInterpolation == T::LevelInterpolationMethod::Undefined)
      levelInterpolation = pInfo->mLevelInterpolationMethod;
*/
    T::ParamLevelId pLevelId = pInfo.mParameterLevelId;
    T::ParamLevel pLevel = pInfo.mParameterLevel;

    if (r.qParam.mParameterLevelId > 0)
    {
      pLevelId = r.qParam.mParameterLevelId;
      if (r.qParam.mParameterLevel >= 0)
        pLevel = r.qParam.mParameterLevel;
    }

    if (r.qParam.mParameterLevel > 0)
      pLevel = r.qParam.mParameterLevel;


    // ### QUERY TYPE : PointValues

    if (r.qParam.mType == QueryParameter::Type::PointValues  &&  getPointTypeGridValues(r,producerName,producerGeometryId,producerInfo,generationInfo,gflags,pInfo,areaInterpolation,pLevelId,pLevel))
      return true;


    if (r.qParam.mType == QueryParameter::Type::VerticalVector  &&  r.qParam.mLocationType == QueryParameter::LocationType::Point  &&
        getVerticalValueVectors(r.query,r.qParam,producerInfo, producerGeometryId, generationInfo.mGenerationId, generationInfo.mAnalysisTime, gflags, pInfo,
            r.forecastTime, pLevelId, r.qParam.mParameterLevels,r.producerId, r.valueList))
    {
      return true;
    }


    if ((r.qParam.mFlags & QueryServer::QueryParameter::Flags::MetricLevels)  &&  (r.qParam.mFlags & QueryParameter::Flags::NoReturnValues) == 0)
    {
      // This is metric query. We should post-process isobands, isolines, etc.

      return getValueVectors(r.query,r.qParam,producerInfo, producerGeometryId, generationInfo.mGenerationId, generationInfo.mAnalysisTime, gflags, pInfo,
          r.forecastTime, pLevelId, pLevel,r.producerId, r.valueList, r.qParam.mCoordinates);
    }

    return getTypeGridValues(r,producerGeometryId,producerInfo,generationInfo,gflags,pInfo,pLevelId,pLevel);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the grid values of the streamline, isoline, isoband, vector or grid file query. Returns true
           if the values were found. */

bool ServiceImplementation::getTypeGridValues(GridValueRequest& r,T::GeometryId producerGeometryId,const T::ProducerInfo& producerInfo,const T::GenerationInfo& generationInfo,UInt64 gflags,ParameterMapping& pInfo,T::ParamLevelId pLevelId,T::ParamLevel pLevel)
{
  try
  {
    switch (r.qParam.mType)
    {
      // ### STREAMLINE QUERY

      case QueryParameter::Type::StreamLine:
        return getStreamlineValues(r.query,r.qParam,producerInfo, producerGeometryId, generationInfo.mGenerationId, generationInfo.mAnalysisTime, gflags, pInfo, r.forecastTime, pLevelId, pLevel,
            r.producerId, r.valueList);

      // ### ISOLINE QUERY

      case QueryParameter::Type::Isoline:
        return getIsolineValues(r.query,r.qParam,producerInfo, producerGeometryId, generationInfo.mGenerationId, generationInfo.mAnalysisTime, gflags,
            pInfo, r.forecastTime, pLevelId, pLevel, r.producerId, r.valueList);

      // ### ISOBAND QUERY

      case QueryParameter::Type::Isoband:
        return getIsobandValues(r.query,r.qParam,producerInfo, producerGeometryId, generationInfo.mGenerationId, generationInfo.mAnalysisTime, gflags,
            pInfo, r.forecastTime, pLevelId, pLevel, r.producerId, r.valueList);

      // ### VECTOR QUERY

      case QueryParameter::Type::Vector:
        return getValueVectors(r.query,r.qParam,producerInfo, producerGeometryId, generationInfo.mGenerationId, generationInfo.mAnalysisTime, gflags, pInfo,
            r.forecastTime, pLevelId, pLevel,r.producerId, r.valueList, r.qParam.mCoordinates);

      // ### GRID FILE QUERY

      case QueryParameter::Type::GridFile:
        return getGridFiles(r.query,r.qParam,producerInfo, producerGeometryId, generationInfo.mGenerationId, generationInfo.mAnalysisTime, gflags,
            pInfo, r.forecastTime, pLevelId, pLevel,r.producerId, r.valueList);

      default:
        return false;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the point values of the parameter mapping by the location type. Returns true if the values
           were found (or the search must end). */

bool ServiceImplementation::getPointTypeGridValues(GridValueRequest& r,const std::string& producerName,T::GeometryId producerGeometryId,const T::ProducerInfo& producerInfo,const T::GenerationInfo& generationInfo,UInt64 gflags,ParameterMapping& pInfo,short areaInterpolation,T::ParamLevelId pLevelId,T::ParamLevel pLevel)
{
  try
  {
    Query& query = r.query;
    QueryParameter& qParam = r.qParam;
    bool singlePoint = (query.mAreaCoordinates.size() == 1 && query.mAreaCoordinates[0].size() == 1);

    // ### LOCATION TYPE : Polygon

    // ### Simple interpolation

    if (qParam.mLocationType == QueryParameter::LocationType::Polygon  &&
        getPolygonValues(query,qParam,producerInfo, producerGeometryId, generationInfo.mGenerationId, generationInfo.mAnalysisTime, gflags,
            pInfo, r.forecastTime, pLevelId, pLevel,r.producerId, r.valueList))
      return true;


    // ### LOCATION TYPE : Circle

    // ### Simple interpolation

    if (qParam.mLocationType == QueryParameter::LocationType::Circle && singlePoint  &&
        getCircleValues(query,qParam,producerInfo, producerGeometryId, generationInfo.mGenerationId, generationInfo.mAnalysisTime, gflags, pInfo, r.forecastTime, pLevelId, pLevel,
            query.mCoordinateType, query.mAreaCoordinates[0][0].x(), query.mAreaCoordinates[0][0].y(), query.mRadius, r.producerId, r.valueList))
      return true;


    // ### LOCATION TYPE : Path

    if (qParam.mLocationType == QueryParameter::LocationType::Path  &&  getInterpolatedPointValues(r,producerGeometryId,producerInfo,generationInfo,gflags,pInfo,areaInterpolation,pLevelId,pLevel))
      return true;


    // ### LOCATION TYPE : Point

    if (qParam.mLocationType != QueryParameter::LocationType::Point)
      return false;

    if (areaInterpolation == T::AreaInterpolationMethod::Landscape && singlePoint)
    {
      double x = query.mAreaCoordinates[0][0].x();
      double y = query.mAreaCoordinates[0][0].y();

      if (getSpecialValues(query,qParam,producerInfo, producerGeometryId, generationInfo.mGenerationId, generationInfo.mAnalysisTime, gflags, pInfo, r.forecastTime, pLevelId, pLevel, qParam.mForecastType, qParam.mForecastNumber,
          qParam.mFlags, T::AreaInterpolationMethod::List, qParam.mTimeInterpolationMethod, qParam.mLevelInterpolationMethod, query.mCoordinateType, x, y, r.producerId, r.valueList))
      {
        setLandscapeValue(r,producerName,producerGeometryId,x,y);
      }
      return true;
    }

    return getInterpolatedPointValues(r,producerGeometryId,producerInfo,generationInfo,gflags,pInfo,areaInterpolation,pLevelId,pLevel);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the point values by a simple interpolation, or the special values for an external
           interpolation (for a single point). Returns true if the values were found. */

bool ServiceImplementation::getInterpolatedPointValues(GridValueRequest& r,T::GeometryId producerGeometryId,const T::ProducerInfo& producerInfo,const T::GenerationInfo& generationInfo,UInt64 gflags,ParameterMapping& pInfo,short areaInterpolation,T::ParamLevelId pLevelId,T::ParamLevel pLevel)
{
  try
  {
    Query& query = r.query;
    QueryParameter& qParam = r.qParam;

    // ### Simple interpolation

    if (areaInterpolation < T::AreaInterpolationMethod::List  &&
        getPointValues(query,qParam,producerInfo, producerGeometryId, generationInfo.mGenerationId, generationInfo.mAnalysisTime, gflags,
            pInfo, r.forecastTime, pLevelId, pLevel, r.producerId, r.valueList))
      return true;

    if (areaInterpolation >= T::AreaInterpolationMethod::List && areaInterpolation < T::AreaInterpolationMethod::External && query.mAreaCoordinates.size() == 1  && query.mAreaCoordinates[0].size() == 1)
    {
      // ### No actual intepolation. Fetching special value for external interpolation

      double x = query.mAreaCoordinates[0][0].x();
      double y = query.mAreaCoordinates[0][0].y();

      return getSpecialValues(query,qParam,producerInfo, producerGeometryId, generationInfo.mGenerationId, generationInfo.mAnalysisTime, gflags, pInfo, r.forecastTime, pLevelId, pLevel, qParam.mForecastType, qParam.mForecastNumber,
          qParam.mFlags, qParam.mAreaInterpolationMethod, qParam.mTimeInterpolationMethod, qParam.mLevelInterpolationMethod, query.mCoordinateType, x, y, r.producerId, r.valueList);
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the values of the landscape interpolation parameter (GeopHeight, LapseRate or LandSeaMask)
           of the producer and the geometry. The producer of the parameter is added to the producer and geometry lists. */

void ServiceImplementation::getLandscapeParameterValues(GridValueRequest& r,const std::string& name,const std::string& producerName,T::GeometryId producerGeometryId,Producer_vec& pList,std::set<T::GeometryId>& gList,ParameterValues& values)
{
  try
  {
    std::string param = name + ";" + producerName + ";" + std::to_string(producerGeometryId);
    std::string paramStr;

    if (!getAlias(param,paramStr))
      paramStr = param;

    QueryParameter newParam = r.qParam;
    newParam.mGeometryId = producerGeometryId;
    newParam.mAreaInterpolationMethod = 500;

    getParameterStringInfo(paramStr,';',newParam.mParam,newParam.mGeometryId,newParam.mParameterLevelId,
        newParam.mParameterLevel,newParam.mForecastType,newParam.mForecastNumber,newParam.mProducerName,newParam.mProducerId,newParam.mGenerationFlags,
        newParam.mAreaInterpolationMethod,newParam.mTimeInterpolationMethod,newParam.mLevelInterpolationMethod,newParam.mFlags);

    if (strcasecmp(newParam.mProducerName.c_str(),producerName.c_str()) != 0)
    {
      pList.emplace_back(newParam.mProducerName,newParam.mGeometryId);
      gList.insert(newParam.mGeometryId);
    }

    getGridValues(r.query,newParam,pList,gList,newParam.mProducerId,r.analysisTime,r.maxAnalysisTime,
      r.generationFlags,newParam.mParam,0,
      r.queryFlags,r.forecastTime,r.timeMatchRequired,values,r.producerStr);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Replace the special values of the point with the landscape interpolated value. */

void ServiceImplementation::setLandscapeValue(GridValueRequest& r,const std::string& producerName,T::GeometryId producerGeometryId,double x,double y)
{
  try
  {
    Producer_vec pList;
    std::set<T::GeometryId> gList;

    pList.emplace_back(producerName,producerGeometryId);
    gList.insert(producerGeometryId);

    ParameterValues zhValueList;
    getLandscapeParameterValues(r,"GeopHeight",producerName,producerGeometryId,pList,gList,zhValueList);

    ParameterValues lrValueList;
    getLandscapeParameterValues(r,"LapseRate",producerName,producerGeometryId,pList,gList,lrValueList);

    ParameterValues lsValueList;
    getLandscapeParameterValues(r,"LandSeaMask",producerName,producerGeometryId,pList,gList,lsValueList);

    std::vector<double> vList;
    std::vector<double> missingList = {ParamValueMissing,ParamValueMissing,ParamValueMissing,ParamValueMissing,ParamValueMissing,ParamValueMissing,ParamValueMissing};

    double dem = getAdditionalValue("dem",r.forecastTime,x,y);
    int coverType = getAdditionalValue("coverType",r.forecastTime,x,y);

    std::vector<double> zhList = getSpecialValueList(zhValueList,missingList);
    std::vector<double> lrList = getSpecialValueList(lrValueList,missingList);
    std::vector<double> lsList = getSpecialValueList(lsValueList,missingList);

    if (r.valueList.mValueList.getLength() != 1)
      return;

    T::GridValue *v = r.valueList.mValueList.getGridValuePtrByIndex(0);
    if (v == nullptr)
      return;

    splitString(v->mValueString,';',vList);
    if (vList.empty())
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
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get grid values. */

void ServiceImplementation::getGridValues(
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
    std::string& producerStr)
{
  FUNCTION_TRACE
  try
  {
    GridValueRequest r{query,qParam,producerId,analysisTime,maxAnalysisTime,generationFlags,parameterKey,parameterHash,queryFlags,forecastTime,timeMatchRequired,valueList,producerStr,0,false};

    printGridValueRequest(r,producers,geometryIdList);

    checkMaxProcessingTime(query,"mMaxProcessingTimeInSeconds");

    checkGridValueArea(query,qParam,"Polygon definition requires at least 3 query.mAreaCoordinates!");

    r.requiredAccessTime = time(nullptr) + 120;

    if (query.mFlags & Query::Flags::AcceptNotReadyGenerations)
      r.acceptNotReadyGenerations = true;

    // Going through the producer list.

    for (auto it = producers.begin(); it != producers.end(); ++it)
    {
      std::string producerName = it->first;
      T::GeometryId producerGeometryId = it->second;

      PRINT_DATA(mDebugLog, "  - Producer and Geometry : %s:%d\n", producerName.c_str(), producerGeometryId);

      if (geometryIdList.find(producerGeometryId) == geometryIdList.end())
      {
        PRINT_DATA(mDebugLog, "  - Producer's '%s' geometry '%d' does not cover all requested query.mAreaCoordinates!\n", producerName.c_str(), producerGeometryId);
        continue;
      }

      bool producerOk = isAreaProducerAccepted(producers.size(),qParam,producerName,producerGeometryId);

      // The current producer supports a geometry where the current query.mAreaCoordinates can be found.

      T::ProducerInfo producerInfo;
      if (producerOk &&  getProducerInfoByName(producerName, producerInfo) && (producerId == 0 || producerInfo.mProducerId == producerId))
      {
        if (getProducerGridValues(r,producerName,producerGeometryId,producerInfo))
          return;
      }
      else
      {
        printInvalidProducer(producerName,producerOk);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Print the arguments of the time range grid value request to the debug log (if enabled). */

void ServiceImplementation::printTimeRangeValueRequest(const TimeRangeValueRequest& r,const Producer_vec& producers,const std::set<T::GeometryId>& geometryIdList)
{
  try
  {
    if (mDebugLog == nullptr ||  !mDebugLog->isEnabled())
      return;

    std::string text = "\nMETHOD getGridValues()\n";
    text += fmt::format("  - producers                      : {} items\n", producers.size());
    for (auto it = producers.begin(); it != producers.end(); ++it)
      text += fmt::format("    * {}:{}\n", it->first, it->second);
    text += fmt::format("  - geometryIdList                 : {} items\n", geometryIdList.size());
    for (int it : geometryIdList)
      text += fmt::format("    * {}\n",it);
    text += fmt::format("  - producerId                     : {}\n", r.producerId);
    text += fmt::format("  - analysisTime                   : {}\n", r.analysisTime);
    text += fmt::format("  - generationFlags                : {}\n", r.generationFlags);
    text += fmt::format("  - parameterKey                   : {}\n", r.parameterKey);
    text += fmt::format("  - queryFlags                     : {:04x}\n", r.queryFlags);
    //PRINT_DATA(mDebugLog, "  - parameterFlags                 : %04x\n", parameterFlags);
    text += fmt::format("  - startTime                      : {}\n", utcTimeFromTimeT(r.startTime));
    text += fmt::format("  - endTime                        : {}\n", utcTimeFromTimeT(r.endTime));
    text += fmt::format("  - valueCounter                   : {}\n", r.valueCounter);
    text += fmt::format("  - recursionCounter               : {}\n", r.recursionCounter);

    PRINT_DATA(mDebugLog, "%s", text.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the parameter mappings of the parameter for the producer and the geometry (by the level of
           the query parameter if it is defined) for the time range request. */

ParameterMapping_vec_sptr ServiceImplementation::getTimeRangeMappings(const T::ProducerInfo& producerInfo,const std::string& parameterKey,std::size_t parameterHash,T::GeometryId producerGeometryId,const QueryParameter& qParam)
{
  try
  {
    ParameterMapping_vec_sptr mappings;
    if (qParam.mParameterLevelId > 0 || qParam.mParameterLevel > 0)
    {
      getParameterMappings(producerInfo.mName, producerInfo.mProducerId, parameterKey, parameterHash, producerGeometryId, qParam.mParameterLevelId, qParam.mParameterLevel, false, mappings);
      if (mappings->empty())
      {
        // Getting a mapping by using the levelId. This probably returns all levels belonging
        // to the current levelId. We need just one for mapping.
        ParameterMapping_vec_sptr tmpMappings;
        getParameterMappings(producerInfo.mName, producerInfo.mProducerId, parameterKey, parameterHash, producerGeometryId, qParam.mParameterLevelId, -1,false, tmpMappings);
        if (!tmpMappings->empty())
        {
          ParameterMapping m((*tmpMappings)[0]);
          m.mParameterLevel = qParam.mParameterLevel;

          // The mapping vector may be shared by the mapping cache. It is not modified.
          mappings = std::make_shared<ParameterMapping_vec>();
          mappings->emplace_back(m);
        }
      }
    }
    else
      getParameterMappings(producerInfo.mName, producerInfo.mProducerId, parameterKey, parameterHash, producerGeometryId, true, mappings);

    return mappings;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the contents of the parameter mapping: from the generation of the requested analysis time,
           or from all generations of the producer. Returns the generation of the analysis time (or nullptr). */

T::GenerationInfo* ServiceImplementation::getTimeRangeContentList(const TimeRangeValueRequest& r,const T::ProducerInfo& producerInfo,const CacheEntry& cacheEntry,const ParameterMapping& pInfo,T::GeometryId producerGeometryId,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::GenerationInfo* generationInfo = nullptr;
    if (!r.analysisTime.empty())
    {
      generationInfo = cacheEntry.generationInfoList->getGenerationInfoByAnalysisTime(r.analysisTime);
      if (generationInfo != nullptr)
      {
        int result = mContentServerPtr->getContentListByParameterAndGenerationId(0, generationInfo->mGenerationId, pInfo.mParameterKeyType, pInfo.mParameterKey,
            pInfo.mParameterLevelId, pInfo.mParameterLevel, pInfo.mParameterLevel, r.qParam.mForecastType, r.qParam.mForecastNumber, producerGeometryId,
            std::string("19000101T000000"), std::string("30000101T000000"), 0, contentInfoList);

        if (result != 0)
        {
          Fmi::Exception exception(BCP, "ContentServer returns an error!");
          exception.addParameter("Service", "getContentListByParameterAndGenerationId");
          exception.addParameter("ErrorMessage", ContentServer::getResultString(result));
          exception.addParameter("generationId", std::to_string(generationInfo->mGenerationId));
          exception.addParameter("parameter", pInfo.mParameterKey);
          exception.addParameter("levelId", std::to_string(pInfo.mParameterLevelId));
          exception.addParameter("level", std::to_string(pInfo.mParameterLevel));
          exception.addParameter("geometryId", std::to_string(producerGeometryId));
          throw exception;
        }
      }
    }

    if (generationInfo == nullptr)
    {
      int result = mContentServerPtr->getContentListByParameterAndProducerId(0, producerInfo.mProducerId, pInfo.mParameterKeyType, pInfo.mParameterKey,
          pInfo.mParameterLevelId, pInfo.mParameterLevel, pInfo.mParameterLevel, r.qParam.mForecastType, r.qParam.mForecastNumber, producerGeometryId,
          std::string("19000101T000000"), std::string("30000101T000000"), 0, contentInfoList);

      if (result != 0)
      {
        Fmi::Exception exception(BCP, "ContentServer returns an error!");
        exception.addParameter("Service", "getContentListByParameterAndProducerId");
        exception.addParameter("ErrorMessage", ContentServer::getResultString(result));
        exception.addParameter("producerId", std::to_string(producerInfo.mProducerId));
        exception.addParameter("parameter", pInfo.mParameterKey);
        exception.addParameter("levelId", std::to_string(pInfo.mParameterLevelId));
        exception.addParameter("level", std::to_string(pInfo.mParameterLevel));
        exception.addParameter("geometryId", std::to_string(producerGeometryId));
        throw exception;
      }
    }
    return generationInfo;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Collect the content times (with their analysis times) of the time range from the valid
           generations, from the newest to the oldest (or from the requested generation only). */

void ServiceImplementation::collectTimeRangeContentTimes(const TimeRangeValueRequest& r,const CacheEntry& cacheEntry,uint gLen,T::GenerationInfo* generationInfo,T::ContentInfoList& contentInfoList,const T::ContentInfo& cFirst,std::map<time_t,std::string>& contentTimeList)
{
  try
  {
    time_t firstTime = 0xFFFFFFFF;
    time_t lastTime = r.startTime;

    uint c = 0;
    uint validCount = 0;
    for (int g=gLen-1; g>=0; g--)
    {
      UInt64 gFlags = generationFlag(c);
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
        gInfo = cacheEntry.generationInfoList->getGenerationInfoByIndex(g);
        gNext = cacheEntry.generationInfoList->getGenerationInfoByIndex(g+1);
        generationValid = isTimeRangeGenerationSelected(r,gInfo,gNext,g,gLen,gFlags);
      }

      if (!isTimeRangeGenerationUsable(r,gInfo,generationValid,validCount))
        continue;

      validCount++;
      addGenerationContentTimes(r,*gInfo,contentInfoList,cFirst,firstTime,lastTime,contentTimeList);

      // The requested time interval should contain data only from the same generation.
      if (((r.qParam.mFlags & QueryParameter::Flags::SameAnalysisTime) != 0 || (r.queryFlags & Query::Flags::SameAnalysisTime) != 0)  &&  !contentTimeList.empty())
        g = 0;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Select the start and the end time of the time range by the content times and the query flags. */

void ServiceImplementation::getTimeRangeDataTimes(const TimeRangeValueRequest& r,std::map<time_t,std::string>& contentTimeList,time_t& startTime_tt,time_t& endTime_tt)
{
  try
  {
    auto dataStartTime_tt = contentTimeList.begin()->first;
    auto dataEndTime_tt = contentTimeList.rbegin()->first;

    if ((r.queryFlags & Query::Flags::StartTimeFromData) != 0)
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
        if (tm->first >= r.startTime)
        {
          tt = tm->first;
          atime = tm->second;
        }
      }

      if (r.queryFlags & Query::Flags::ForceStartTime)
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

    if ((r.queryFlags & Query::Flags::EndTimeFromData) != 0)
      endTime_tt = dataEndTime_tt;

    //if (startTime_tt < dataStartTime_tt)
    //  startTime_tt = dataStartTime_tt;

    endTime_tt = std::min(endTime_tt, dataEndTime_tt);


    PRINT_DATA(mDebugLog," -- TIME RANGE %s - %s\n",utcTimeFromTimeT(startTime_tt).c_str(),utcTimeFromTimeT(endTime_tt).c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Fetch the values of the selected times in the time range from the producer and add them to the
           value list of the parameter. The times that are not original times are aggregation times. Returns the
           latest time of the values (or the start time). */

time_t ServiceImplementation::fetchTimeRangeTimeValues(TimeRangeValueRequest& r,Producer_vec& producers2,std::set<T::GeometryId>& geometryIdList2,const T::ProducerInfo& producerInfo,const std::map<time_t,std::string>& contentTimeList2,const std::set<time_t>& originalTimes)
{
  try
  {
    time_t a_lastTime = r.startTime;
    for (const auto& forecastTime : contentTimeList2)
    {
      if (!((forecastTime.first == r.startTime && !r.ignoreStartTimeValue) || (forecastTime.first > r.startTime && forecastTime.first <= r.endTime)))
        continue;

      std::shared_ptr<ParameterValues> valList = std::make_shared<ParameterValues>();

      getGridValues(r.query,r.qParam,producers2, geometryIdList2, producerInfo.mProducerId, forecastTime.second, r.maxAnalysisTime, 0, r.parameterKey, r.parameterHash,
          r.queryFlags, forecastTime.first, true, *valList, r.producerStr);

      if (valList->mValueList.getLength() == 0 && valList->mValueData.empty() && valList->mValueVector.empty())
        continue;

      if (valList->mForecastTimeUTC == 0)
        valList->mForecastTimeUTC = forecastTime.first;

      a_lastTime = std::max(a_lastTime, valList->mForecastTimeUTC);

      if (!originalTimes.empty() && originalTimes.find(valList->mForecastTimeUTC) == originalTimes.end())
      {
        valList->mFlags |= ParameterValues::Flags::AggregationValue;
        r.query.mAggregationTimes.insert(valList->mForecastTimeUTC);

        PRINT_DATA(mDebugLog," ##### set aggregationFlag (%zu) %s\n",originalTimes.size(),utcTimeFromTimeT(valList->mForecastTimeUTC).c_str());
      }

      r.qParam.mValueList.emplace_back(valList);
    }
    return a_lastTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Fetch the values of the content times of the time range from the producer, and continue from
           the concatenated producers after the last time found. */

void ServiceImplementation::fetchTimeRangeContentValues(TimeRangeValueRequest& r,const std::pair<std::string,T::GeometryId>& producer,const T::ProducerInfo& producerInfo,std::map<time_t,std::string>& contentTimeList)
{
  try
  {
    Producer_vec producers2;
    producers2.emplace_back(producer);

    std::set<T::GeometryId> geometryIdList2;
    geometryIdList2.insert(producer.second);

    std::set<time_t> originalTimes;
    std::map<time_t,std::string> contentTimeList2;

    time_t startTime_tt = r.startTime;
    time_t endTime_tt = r.endTime;

    getTimeRangeDataTimes(r,contentTimeList,startTime_tt,endTime_tt);

    if (r.timeStepIsData)
      addDataTimeSteps(r,contentTimeList,startTime_tt,endTime_tt,contentTimeList2,originalTimes);
    else
      addQueryTimeSteps(r,contentTimeList,startTime_tt,endTime_tt,contentTimeList2,originalTimes);


    time_t a_lastTime = fetchTimeRangeTimeValues(r,producers2,geometryIdList2,producerInfo,contentTimeList2,originalTimes);

    auto rec = mProducerConcatMap.find(r.producerStr);
    if (rec != mProducerConcatMap.end()  &&  !rec->second.empty())
    {
      //printf("CONCAT SEARCH %s  %s\n",producerStr.c_str(),rec->second[0].first.c_str());
      std::set<T::GeometryId> tmpGeomIdList;

      for (auto& gg : rec->second)
      {
        //printf("  ** SEARCH %ld %d\n",rec->second.size(),gg->second);
        tmpGeomIdList.insert(gg.second);
      }

      r.producerStr = "";
      getGridValues(r.query,r.qParam,rec->second, tmpGeomIdList, 0, r.analysisTime, r.generationFlags, r.parameterKey, r.parameterHash,
          r.queryFlags, a_lastTime+1,r.endTime,r.producerStr,r.valueCounter,r.recursionCounter+1);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the time range values of the parameter mapping. Returns true if content times were found
           in the time range (the search ends then). */

bool ServiceImplementation::getMappingTimeRangeValues(TimeRangeValueRequest& r,const std::pair<std::string,T::GeometryId>& producer,const T::ProducerInfo& producerInfo,const CacheEntry& cacheEntry,uint gLen,const ParameterMapping& pInfo)
{
  try
  {
    T::GeometryId producerGeometryId = producer.second;

    if (mDebugLog != nullptr &&  mDebugLog->isEnabled())
    {
      std::stringstream stream;
      pInfo.print(stream, 5, 0);
      PRINT_DATA(mDebugLog, "%s\n", stream.str().c_str());
    }

    std::map<time_t,std::string> contentTimeList;
    T::ContentInfoList contentInfoList;

    T::GenerationInfo* generationInfo = getTimeRangeContentList(r,producerInfo,cacheEntry,pInfo,producerGeometryId,contentInfoList);

    int cLen = contentInfoList.getLength();
    PRINT_DATA(mDebugLog, "         + Found %d content records\n", cLen);


    if (cLen <= 0)
      return false;

    contentInfoList.sort(T::ContentInfo::ComparisonMethod::starttime_generationId_file_message);
    T::ContentInfo* cFirst = contentInfoList.getContentInfoByIndex(0);

    // The latest generation is selected by the analysis time. Limiting the generations to the
    // one with the latest content time would reject it if an older generation extends further.

    if (r.analysisTime.empty() && (r.queryFlags & Query::Flags::LatestGeneration) == 0)
    {
      T::GenerationInfo *lastGeneration = cacheEntry.generationInfoList->getLastGenerationInfoByContentTime(producerInfo.mProducerId);
      if (lastGeneration)
        r.maxAnalysisTime = lastGeneration->mAnalysisTime;
    }

    collectTimeRangeContentTimes(r,cacheEntry,gLen,generationInfo,contentInfoList,*cFirst,contentTimeList);

    if (contentTimeList.empty())
      return false;

    fetchTimeRangeContentValues(r,producer,producerInfo,contentTimeList);
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the time range values of the producer and the geometry. Returns true if values were searched
           for a time range (the search ends then). */

bool ServiceImplementation::getProducerTimeRangeValues(TimeRangeValueRequest& r,const std::pair<std::string,T::GeometryId>& producer,const T::ProducerInfo& producerInfo)
{
  try
  {
    T::GeometryId producerGeometryId = producer.second;

    PRINT_DATA(mDebugLog, "  - The producer and the geometry are acceptable!\n");

    // Reading generations supported by the current producer.

    auto cacheEntry = getGenerationInfoListByProducerId(producerInfo.mProducerId,producerInfo.mHash,true);
    uint gLen = cacheEntry->generationInfoList->getLength();

    if (gLen == 0)
    {
      PRINT_DATA(mDebugLog, "    - No generations found for the current producer!\n");
      return false;
    }

    ParameterMapping_vec_sptr mappings = getTimeRangeMappings(producerInfo,r.parameterKey,r.parameterHash,producerGeometryId,r.qParam);

    if (mappings->empty())
    {
      PRINT_DATA(mDebugLog, "    - No mappings found (%s,%s,%d,%d,%d)!\n",producerInfo.mName.c_str(),r.parameterKey.c_str(),producerGeometryId,r.qParam.mParameterLevelId,r.qParam.mParameterLevel);
      return false;
    }

    for (auto& pInfo : *mappings)
    {
      if (getMappingTimeRangeValues(r,producer,producerInfo,*cacheEntry,gLen,pInfo))
        return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get grid values. */

void ServiceImplementation::getGridValues(
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
    uint recursionCounter)
{
  FUNCTION_TRACE
  try
  {
    if (recursionCounter > 5)
      return;

    TimeRangeValueRequest r{query,qParam,producerId,analysisTime,generationFlags,parameterKey,parameterHash,queryFlags,startTime,endTime,producerStr,valueCounter,recursionCounter,std::string(),0,0,0,false,false,false};

    printTimeRangeValueRequest(r,producers,geometryIdList);

    checkMaxProcessingTime(query,"mMaxProcessingTimeInSeconds");

    checkGridValueArea(query,qParam,"Area definition requires at least 3 query.mAreaCoordinates!");

    r.queryTimestepSizeInSeconds = query.mTimestepSizeInMinutes*60;
    r.parameterTimestepSizeInSeconds = qParam.mTimestepSizeInMinutes*60;

    r.requiredAccessTime = time(nullptr) + 120;

    r.ignoreStartTimeValue = ((queryFlags & Query::Flags::StartTimeNotIncluded) != 0);

    r.timeStepIsData = ((queryFlags & Query::Flags::TimeStepIsData) != 0);

    r.acceptNotReadyGenerations = ((query.mFlags & Query::Flags::AcceptNotReadyGenerations) != 0);

    // Going through the producer list.

    for (auto it = producers.begin(); it != producers.end(); ++it)
    {
      std::string producerName = it->first;
      T::GeometryId producerGeometryId = it->second;

      PRINT_DATA(mDebugLog, "  - Producer and Geometry : %s:%d\n", producerName.c_str(), producerGeometryId);

      // Checking if the current geometry can be found from the acceptable geometry list

      if (geometryIdList.find(producerGeometryId) == geometryIdList.end())
      {
        PRINT_DATA(mDebugLog, "  - Producer's '%s' geometry '%d' does not cover all requested query.mAreaCoordinates!\n", producerName.c_str(), producerGeometryId);
        continue;
      }

      // The current producer supports a geometry where the current query.mAreaCoordinates can be found.

      bool producerOk = isAreaProducerAccepted(producers.size(),qParam,producerName,producerGeometryId);

      T::ProducerInfo producerInfo;
      if (producerOk &&  getProducerInfoByName(producerName, producerInfo) && (producerId == 0 || producerInfo.mProducerId == producerId))
      {
        if (getProducerTimeRangeValues(r,*it,producerInfo))
          return;
      }
      else
      {
        printInvalidProducer(producerName,producerOk);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get additional values. */

void ServiceImplementation::getAdditionalValues(
    const std::string& parameterName,
    uchar /*coordinateType*/,
    const T::Coordinate_vec& coordinates,
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
      if (mDem)
        addCoordinateValues(coordinates,values,[this](T::GridValue& val) { val.mValue = mDem->elevation(val.mX, val.mY); });
      return;
    }

    if (param == "covertype")
    {
      if (mLandCover)
        addCoordinateValues(coordinates,values,[this](T::GridValue& val) { val.mValue = mLandCover->coverType(val.mX, val.mY); });
      return;
    }

    const auto& functions = astronomyValueFunctions();
    auto function = functions.find(param);
    if (function != functions.end())
      addCoordinateValues(coordinates,values,[&](T::GridValue& val) { function->second(val,utcTime); });
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get additional value. */

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
      return Fmi::Astronomy::moonphase(utcTime.utc_time());
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





namespace
{

/*! \brief Bisection bracket of the level search: the contents and the heights at both ends. */

struct LevelBracket
{
  T::ContentInfo *first = nullptr;
  T::ContentInfo *last = nullptr;
  int firstIndex = 0;
  int lastIndex = 0;
  T::ParamValue firstValue = 0;
  T::ParamValue lastValue = 0;
};



/*! \brief Narrow the bracket with the height of the middle content. Returns true when the levels (and their heights)
           closest to the requested height are found: the middle content itself or the two contents of a minimal bracket. */

bool bisectLevelBracket(LevelBracket& b,T::ContentInfo *mid,int middleIndex,T::ParamValue middleValue,int height,bool reverse,int& level1,float& height1,int& level2,float& height2)
{
  if (height == middleValue)
  {
    level1 = mid->mParameterLevel;
    level2 = mid->mParameterLevel;
    height1 = middleValue;
    height2 = middleValue;
    return true;
  }

  bool aboveMiddle = (reverse ? height < middleValue : height > middleValue);
  if (aboveMiddle)
  {
    b.first = mid;
    b.firstIndex = middleIndex;
    b.firstValue = middleValue;
  }
  else
  {
    b.last = mid;
    b.lastIndex = middleIndex;
    b.lastValue = middleValue;
  }

  if ((b.firstIndex+1) != b.lastIndex)
    return false;

  if (reverse)
  {
    level1 = b.first->mParameterLevel;
    level2 = b.last->mParameterLevel;
    height1 = b.firstValue;
    height2 = b.lastValue;
  }
  else
  {
    level1 = b.last->mParameterLevel;
    level2 = b.first->mParameterLevel;
    height1 = b.lastValue;
    height2 = b.firstValue;
  }
  return true;
}



/*! \brief Scale the heights of the record with the multiplier of the height conversion. */

void scaleHeights(HeightRec& rec,double mul)
{
  if (mul != 1.0)
  {
    rec.height1_1 = rec.height1_1 * mul;
    rec.height1_2 = rec.height1_2 * mul;
    rec.height2_1 = rec.height2_1 * mul;
    rec.height2_2 = rec.height2_2 * mul;
  }
}

}  // namespace





/*! \brief Query Server: Find the height conversion (multiplier, height producer and height parameter) of the producer and level id hash. */

bool ServiceImplementation::findHeightConversion(std::size_t hash,double& mul,T::ProducerId& heightProducerId,std::string& heightParameter)
{
  try
  {
    AutoReadLock lock(&mHeightConversions_modificationLock);
    auto conv = mHeightConversions.find(hash);
    if (conv == mHeightConversions.end())
      return false; // No conversion defined for the current producer and level id

    mul = conv->second.multiplier;
    heightProducerId = conv->second.heightProducerId;
    heightParameter = conv->second.heightParameter;
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the generation of the height producer. Returns false if the height producer is not the
           requested producer and it has no generation with the same analysis time. */

bool ServiceImplementation::getHeightGenerationId(T::ProducerId producerId,T::GenerationId generationId,HeightQuery& q)
{
  try
  {
    q.heightGenerationId = generationId;
    if (producerId == q.heightProducerId)
      return true;

    // Height producer is different than the requested producer. If we want to
    // use it then the producer should have a generation with the same analysis time.

    T::GenerationInfo gInfo1;
    if (!mGenerationInfoList->getGenerationInfoById(generationId,gInfo1))
      return false;

    T::GenerationInfo gInfo2;
    if (!mGenerationInfoList->getGenerationInfoByProducerIdAndAnalysisTime(q.heightProducerId,gInfo1.mAnalysisTime,gInfo2))
      return false;

    q.heightGenerationId = gInfo2.mGenerationId;
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the first and the last level of the height parameter (cached). Returns false if there are no levels. */

bool ServiceImplementation::getHeightLevelRange(HeightQuery& q)
{
  try
  {
    std::size_t levelHash = 0;
    Fmi::hash_merge(levelHash,q.heightGenerationId);
    Fmi::hash_merge(levelHash,q.geometryId);
    Fmi::hash_merge(levelHash,q.paramLevelId);
    Fmi::hash_merge(levelHash,q.heightProducerId);

    q.firstLevel = 0xFFFFFFFF;
    q.lastLevel = 0xFFFFFFFF;

    auto itm = mLevelRangeCache.find(levelHash);
    if (itm)
    {
      q.firstLevel = (*itm >> 32) & 0xFFFFFFFF;
      q.lastLevel = (*itm) & 0xFFFFFFFF;
    }

    if (q.firstLevel != 0xFFFFFFFF  &&  q.lastLevel != 0xFFFFFFFF)
      return true;

    std::set<T::ParamLevel> levels;
    int result = mContentServerPtr->getContentLevelListByGenerationGeometryAndLevelId(0,q.heightGenerationId,q.geometryId,q.paramLevelId,levels);
    if (result != 0 || levels.empty())
      return false;

    q.firstLevel = *levels.begin();
    q.lastLevel = *levels.rbegin();

    std::size_t lev = ((std::size_t)q.firstLevel << 32) + (std::size_t)q.lastLevel;
    mLevelRangeCache.insert(levelHash,lev);
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the height parameter contents of the time between the first and the last level. */

void ServiceImplementation::getHeightContentList(const HeightQuery& q,time_t forecastTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::ParamKeyType paramKeyType = T::ParamKeyTypeValue::FMI_NAME;
    mContentServerPtr->getContentListByParameterAndGenerationId(0,q.heightGenerationId,paramKeyType,q.heightParameter,q.paramLevelId,q.firstLevel,q.lastLevel,q.forecastType,q.forecastNumber,q.geometryId,forecastTime,forecastTime,requestFlags,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the height parameter contents of the forecast time from the level content cache or from the content server. */

ContentCacheEntry_sptr ServiceImplementation::getHeightContentEntry(std::size_t levelContentHash,const HeightQuery& q,time_t forecastTime)
{
  try
  {
    auto cc = mLevelContentCache.find(levelContentHash);
    if (cc)
    {
      // The required content information was found from the cache
      return *cc;
    }

    auto entry = std::make_shared<ContentCacheEntry>();
    getHeightContentList(q,forecastTime,0,entry->contentInfoList);
    entry->generationId = q.heightGenerationId;
    entry->producerHash = q.heightProducerId;

    mLevelContentCache.insert(levelContentHash,entry);
    return entry;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the closest times before and after the forecast time that have height parameter contents. Returns false if there are no such times. */

bool ServiceImplementation::getHeightTimeNeighbours(const HeightQuery& q,time_t forecastTime,time_t& time1,time_t& time2)
{
  try
  {
    T::ContentInfoList tmpContentList;
    getHeightContentList(q,forecastTime,ContentServer::RequestFlags::INCLUDE_TIME_BEFORE | ContentServer::RequestFlags::INCLUDE_TIME_AFTER,tmpContentList);

    if (tmpContentList.getLength() != 2)
      return false;

    T::ContentInfo *prev = tmpContentList.getContentInfoByIndex(0);
    T::ContentInfo *next = tmpContentList.getContentInfoByIndex(1);

    time1 = prev->mForecastTimeUTC;
    time2 = next->mForecastTimeUTC;
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the height parameter contents of the two times. Returns false if a content list is empty or the lists are of different lengths. */

bool ServiceImplementation::getHeightContentListPair(const HeightQuery& q,time_t time1,time_t time2,T::ContentInfoList& contentInfoList1,T::ContentInfoList& contentInfoList2)
{
  try
  {
    getHeightContentList(q,time1,0,contentInfoList1);
    if (contentInfoList1.getLength() == 0)
      return false;

    getHeightContentList(q,time2,0,contentInfoList2);
    if (contentInfoList2.getLength() == 0)
      return false;

    return contentInfoList1.getLength() == contentInfoList2.getLength();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get closest levels by height. */

bool ServiceImplementation::getClosestLevelsByHeight(T::ProducerId producerId,T::GenerationId generationId,int geometryId,T::ParamLevelId paramLevelId,int forecastType,int forecastNumber,time_t forecastTime,uchar coordinateType,double x,double y,int height,HeightRec& rec)
{
  FUNCTION_TRACE
  try
  {
    //PRINT_DATA(mDebugLog,"getClosestLevelsByHeight (height = %d)\n",height);

    double mul = 1.0;
    HeightQuery q;

    std::size_t hash = 0;
    Fmi::hash_merge(hash,producerId);
    Fmi::hash_merge(hash,paramLevelId);
    //std::size_t hashConversion = hash;

    if (!findHeightConversion(hash,mul,q.heightProducerId,q.heightParameter))
      return false; // No conversion defined for the current producer and level id


    Fmi::hash_merge(hash,generationId);
    Fmi::hash_merge(hash,geometryId);
    Fmi::hash_merge(hash,forecastType);
    Fmi::hash_merge(hash,forecastNumber);
    Fmi::hash_merge(hash,forecastTime);
    Fmi::hash_merge(hash,coordinateType);
    std::size_t levelContentHash = hash;

    Fmi::hash_merge(hash,height);
    Fmi::hash_merge(hash,x);
    Fmi::hash_merge(hash,y);

    auto it = mPointHeightCache.find(hash);
    if (it)
    {
      // Cache match
      rec = *it;
      return true;
    }

    q.geometryId = geometryId;
    q.paramLevelId = paramLevelId;
    q.forecastType = forecastType;
    q.forecastNumber = forecastNumber;

    if (!getHeightGenerationId(producerId,generationId,q)  ||  !getHeightLevelRange(q))
      return false;

    ContentCacheEntry_sptr entry = getHeightContentEntry(levelContentHash,q,forecastTime);

    int len = entry->contentInfoList.getLength();

    rec.levelId = paramLevelId;
    rec.time1 = forecastTime;

    if (len > 0)
    {
      if (getClosestLevelsByHeight(entry->contentInfoList,coordinateType,x,y,height/mul,rec.level1_1,rec.height1_1,rec.level1_2,rec.height1_2))
      {
        scaleHeights(rec,mul);
        mPointHeightCache.insert(hash,rec);
        return true;
      }
      return false;
    }


    time_t time1 = 0;
    time_t time2 = 0;
    T::ContentInfoList contentInfoList1;
    T::ContentInfoList contentInfoList2;
    if (!getHeightTimeNeighbours(q,forecastTime,time1,time2)  ||  !getHeightContentListPair(q,time1,time2,contentInfoList1,contentInfoList2))
      return false;

    rec.time1 = time1;
    rec.time2 = time2;

    if (getClosestLevelsByHeight(contentInfoList1,coordinateType,x,y,height/mul,rec.level1_1,rec.height1_1,rec.level1_2,rec.height1_2) &&
        getClosestLevelsByHeight(contentInfoList2,coordinateType,x,y,height/mul,rec.level2_1,rec.height2_1,rec.level2_2,rec.height2_2))
    {
      scaleHeights(rec,mul);
      mPointHeightCache.insert(hash,rec);
      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get closest levels by height. */

bool ServiceImplementation::getClosestLevelsByHeight(const T::ContentInfoList& contentInfoList,uchar coordinateType,double x,double y,int height,int& level1,float& height1,int& level2,float& height2)
{
  FUNCTION_TRACE
  try
  {
    //PRINT_DATA(mDebugLog,"getClosestLevelsByHeight (height = %d)\n",height);

    int len = contentInfoList.getLength();
    if (len == 0)
      return false;

    LevelBracket b;
    b.firstIndex = 0;
    b.lastIndex = len-1;

    b.first = contentInfoList.getContentInfoByIndex(b.firstIndex);
    b.last = contentInfoList.getContentInfoByIndex(b.lastIndex);
    if (b.first == nullptr  ||  b.last == nullptr)
      return false;

    double_vec modificationParameters;
    T::ParamValue middleValue = 0;
    int res1 = mDataServerPtr->getGridValueByPoint(0,b.first->mFileId,b.first->mMessageIndex,coordinateType,x,y,1,0,modificationParameters,b.firstValue);
    if (res1 != 0)
      return false;

    int res2 = mDataServerPtr->getGridValueByPoint(0,b.last->mFileId,b.last->mMessageIndex,coordinateType,x,y,1,0,modificationParameters,b.lastValue);
    if (res2 != 0)
      return false;

    bool reverse = false;
    if (b.firstValue > b.lastValue)
      reverse = true;


    while (b.firstIndex < b.lastIndex)
    {
      int middleIndex = (b.firstIndex + b.lastIndex) / 2;
      T::ContentInfo *mid = contentInfoList.getContentInfoByIndex(middleIndex);
      if (mid == nullptr)
        return false;

      int res3 = mDataServerPtr->getGridValueByPoint(0,mid->mFileId,mid->mMessageIndex,coordinateType,x,y,1,0,modificationParameters,middleValue);
      if (res3 != 0)
        return false;

      if (bisectLevelBracket(b,mid,middleIndex,middleValue,height,reverse,level1,height1,level2,height2))
        return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get the height values of the content at the coordinates from the height vector cache or from the data server. Errors are logged and return false. */

bool ServiceImplementation::getHeightValueVector(const T::ContentInfo& info,uchar coordinateType,std::vector<T::Coordinate>& coordinates,std::size_t coordinateHash,std::shared_ptr<T::ParamValue_vec>& valueVec)
{
  try
  {
    short areaInterpolationMethod = T::AreaInterpolationMethod::Linear;
    double_vec modificationParameters;

    std::size_t key = ((std::size_t)info.mFileId << 24) + info.mMessageIndex + coordinateHash;

    auto it = mHeightVecCache.find(key);
    if (it)
    {
      valueVec = *it;
      return true;
    }

    valueVec = std::make_shared<T::ParamValue_vec>();
    int result1 = mDataServerPtr->getGridValueVectorByCoordinateList(0,info.mFileId,info.mMessageIndex,coordinateType,coordinates,areaInterpolationMethod,0,modificationParameters,*valueVec);
    if (result1 != 0)
    {
      Fmi::Exception exception(BCP, "DataServer returns an error!");
      exception.addParameter("Service", "getGridValueVectorByCoordinateList");
      exception.addParameter("ErrorMessage", DataServer::getResultString(result1));
      exception.addParameter("FileId", std::to_string(info.mFileId));
      exception.addParameter("MessageIndex", std::to_string(info.mMessageIndex));
      std::string errorMsg = exception.getStackTrace();
      PRINT_DATA(mDebugLog, "%s\n", errorMsg.c_str());
      return false;
    }

    mHeightVecCache.insert(key,valueVec);
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get closest levels by height. */

bool ServiceImplementation::getClosestLevelsByHeight(const T::ContentInfoList& contentInfoList,uchar coordinateType,std::vector<T::Coordinate>& coordinates,uint coordinateIndex,std::size_t coordinateHash,int height,int& level1,float& height1,int& level2,float& height2)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"getClosestLevelsByHeight (height = %d)\n",height);

    int len = contentInfoList.getLength();
    if (len == 0)
      return false;

    LevelBracket b;
    b.firstIndex = 0;
    b.lastIndex = len-1;

    b.first = contentInfoList.getContentInfoByIndex(b.firstIndex);
    b.last = contentInfoList.getContentInfoByIndex(b.lastIndex);
    if (b.first == nullptr  ||  b.last == nullptr)
      return false;

    T::ParamValue middleValue = 0;

    std::shared_ptr<T::ParamValue_vec> firstValueVec;
    std::shared_ptr<T::ParamValue_vec> lastValueVec;
    std::shared_ptr<T::ParamValue_vec> middleValueVec;

    if (!getHeightValueVector(*b.first,coordinateType,coordinates,coordinateHash,firstValueVec))
      return false;

    b.firstValue = (*firstValueVec)[coordinateIndex];

    if (!getHeightValueVector(*b.last,coordinateType,coordinates,coordinateHash,lastValueVec))
      return false;

    b.lastValue = (*lastValueVec)[coordinateIndex];

    bool reverse = false;
    if (b.firstValue > b.lastValue)
      reverse = true;

    while (b.firstIndex < b.lastIndex)
    {
      int middleIndex = (b.firstIndex + b.lastIndex) / 2;
      T::ContentInfo *mid = contentInfoList.getContentInfoByIndex(middleIndex);
      if (mid == nullptr)
        return false;

      if (!getHeightValueVector(*mid,coordinateType,coordinates,coordinateHash,middleValueVec))
        return false;

      middleValue = (*middleValueVec)[coordinateIndex];

      if (bisectLevelBracket(b,mid,middleIndex,middleValue,height,reverse,level1,height1,level2,height2))
        return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Get closest levels by height. */

bool ServiceImplementation::getClosestLevelsByHeight(T::ProducerId producerId,T::GenerationId generationId,int geometryId,T::ParamLevelId paramLevelId,int forecastType,int forecastNumber,time_t forecastTime,uchar coordinateType,std::vector<T::Coordinate>& coordinates,int height,HeightRec_vec& recs)
{
  FUNCTION_TRACE
  try
  {
    //PRINT_DATA(mDebugLog,"getClosestLevelsByHeight (height = %d)\n",height);

    double mul = 1.0;
    HeightQuery q;

    std::size_t hash = 0;
    Fmi::hash_merge(hash,producerId);
    Fmi::hash_merge(hash,paramLevelId);
    //std::size_t hashConversion = hash;

    if (!findHeightConversion(hash,mul,q.heightProducerId,q.heightParameter))
    {
      PRINT_DATA(mDebugLog,"  ** No height conversion defined\n");
      return false; // No conversion defined for the current producer and level id
    }


    Fmi::hash_merge(hash,generationId);
    Fmi::hash_merge(hash,geometryId);
    Fmi::hash_merge(hash,forecastType);
    Fmi::hash_merge(hash,forecastNumber);
    Fmi::hash_merge(hash,forecastTime);
    Fmi::hash_merge(hash,coordinateType);
    std::size_t levelContentHash = hash;



    std::size_t coordinateHash = 0;
    uint clen = coordinates.size();
    for (uint t=0; t<clen; t = t + 100)
    {
      Fmi::hash_merge(coordinateHash,clen);
      Fmi::hash_merge(coordinateHash,coordinates[t].x());
      Fmi::hash_merge(coordinateHash,coordinates[t].y());
    }

    q.geometryId = geometryId;
    q.paramLevelId = paramLevelId;
    q.forecastType = forecastType;
    q.forecastNumber = forecastNumber;

    if (!getHeightGenerationId(producerId,generationId,q))
      return false;

    if (!getHeightLevelRange(q))
    {
      PRINT_DATA(mDebugLog,"  ** No levels found\n");
      return false;
    }


    ContentCacheEntry_sptr entry = getHeightContentEntry(levelContentHash,q,forecastTime);

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
        if (!getClosestLevelsByHeight(entry->contentInfoList,coordinateType,coordinates,t,coordinateHash,height/mul,rec.level1_1,rec.height1_1,rec.level1_2,rec.height1_2))
          return false;

        scaleHeights(rec,mul);
        recs.push_back(rec);
      }
      return true;
    }


    time_t time1 = 0;
    time_t time2 = 0;
    if (!getHeightTimeNeighbours(q,forecastTime,time1,time2))
      return true;

    T::ContentInfoList contentInfoList1;
    T::ContentInfoList contentInfoList2;
    if (!getHeightContentListPair(q,time1,time2,contentInfoList1,contentInfoList2))
      return false;

    for (uint t=0;t<clen; t++)
    {
      HeightRec rec;
      rec.levelId = paramLevelId;
      rec.time1 = time1;
      rec.time2 = time2;

      //if (getClosestLevelsByHeight(contentInfoList1,coordinateType,coordinates[t].x(),coordinates[t].y(),height/mul,rec.level1_1,rec.height1_1,rec.level1_2,rec.height1_2) &&
      //    getClosestLevelsByHeight(contentInfoList2,coordinateType,coordinates[t].x(),coordinates[t].y(),height/mul,rec.level2_1,rec.height2_1,rec.level2_2,rec.height2_2))
      if (!getClosestLevelsByHeight(contentInfoList1,coordinateType,coordinates,t,coordinateHash,height/mul,rec.level1_1,rec.height1_1,rec.level1_2,rec.height1_2) ||
          !getClosestLevelsByHeight(contentInfoList2,coordinateType,coordinates,t,coordinateHash,height/mul,rec.level2_1,rec.height2_1,rec.level2_2,rec.height2_2))
        return false;

      scaleHeights(rec,mul);
      recs.push_back(rec);
    }

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Check producer map updates. */

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
      {
        mProducerMap_updateTime = 0;
        mGenerationInfoList_checkTime = 0;
      }
    }


    time_t currentTime = time(nullptr);
    if ((currentTime - mProducerMap_updateTime) < mProducerMap_checkInterval)
      return;

    T::ProducerInfoList producerInfoList;
    if (mContentServerPtr->getProducerInfoList(0,producerInfoList) == 0)
    {
      auto *prodMap = new Producer_map();
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





/*! \brief Query Server: Check generation updates. */

void ServiceImplementation::checkGenerationUpdates()
{
  FUNCTION_TRACE
  try
  {
    time_t currentTime = time(nullptr);
    if ((currentTime - mGenerationInfoList_checkTime) < mGenerationInfoList_checkInterval)
      return;

    auto *generationInfoList = new T::GenerationInfoList();
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





/*! \brief Query Server: Check producer file updates. */

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





/*! \brief Query Server: Check parameter mapping updates. */

void ServiceImplementation::checkParameterMappingUpdates()
{
  FUNCTION_TRACE
  try
  {
    time_t currentTime = time(nullptr);
    if ((currentTime - mParameterMapping_checkTime) < mParameterMapping_checkInterval)
      return;

    mParameterMapping_checkTime = time(nullptr);
    for (auto& mParameterMapping : mParameterMappings)
    {
      if (!mShutdownRequested  &&  mParameterMapping.checkUpdates())
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





/*! \brief Query Server: Clear the cached information if the content server has been restarted. */

void ServiceImplementation::checkContentServerRestart()
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Update the requests per second statistics every 10 seconds and reload the height conversion file every minute. */

void ServiceImplementation::updateRequestStatistics(time_t& prevTime,Int64& prevCounter)
{
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Query Server: Update processing. */

void ServiceImplementation::updateProcessing()
{
  try
  {
    mUpdateProcessingActive = true;

    time_t prevTime = time(nullptr);
    Int64  prevCounter = mRequestCounter;

    while (!mShutdownRequested)
    {

      runIgnoringExceptions([this]() { checkContentServerRestart(); });
      runIgnoringExceptions([this]() { checkProducerMapUpdates(); });
      runIgnoringExceptions([this]() { checkGenerationUpdates(); });
      runIgnoringExceptions([this]() { checkProducerFileUpdates(); });
      runIgnoringExceptions([this]() { checkParameterMappingUpdates(); });
      runIgnoringExceptions([this]() { mLuaFileCollection.checkUpdates(false); });
      runIgnoringExceptions([this]() { mProducerAliasFileCollection.checkUpdates(false); });
      runIgnoringExceptions([this]() { mAliasFileCollection.checkUpdates(false); });
      runIgnoringExceptions([&]() { updateRequestStatistics(prevTime,prevCounter); });


      if (!mShutdownRequested)
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    mUpdateProcessingActive = false;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP, "Operation failed!", nullptr);
    throw exception;
  }
}





/*! \brief Query Server: Start update processing. */

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
