#include "QueryConfigurator.h"
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AdditionalParameters.h>
#include <grid-files/common/ShowFunction.h>
#include <time.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace QueryServer
{


QueryConfigurator::QueryConfigurator()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





QueryConfigurator::~QueryConfigurator()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Operation failed!",nullptr);
    exception.printError();
  }
}





void QueryConfigurator::configure(Query& query,T::AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    attributeList.setCaseSensitive(false);

    configureProducer(query,attributeList);
    configureGeneration(query,attributeList);
    configureLocation(query,attributeList);
    configureTime(query,attributeList);
    configureParameters(query,attributeList);
    configureMisc(query,attributeList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void QueryConfigurator::configureMisc(Query& query,T::AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    const char *languageStr = attributeList.getAttributeValue("language");
    if (languageStr != nullptr)
      query.mLanguage = languageStr;

  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void QueryConfigurator::configureProducer(Query& query,T::AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    const char *producerStr = attributeList.getAttributeValue("producer");
    if (producerStr != nullptr)
    {
      splitString(producerStr,',',query.mProducerNameList);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void QueryConfigurator::configureGeneration(Query& query,T::AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    const char *analysisTimeStr = attributeList.getAttributeValue("analysisTime");
    if (analysisTimeStr != nullptr)
    {
      query.mAnalysisTime = analysisTimeStr;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void QueryConfigurator::configureTime(Query& query,T::AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    char tmp[1000];
    time_t currentTime = time(0);

    time_t startTime = currentTime;
    int startStep = 0;
    int timestep = 3600;
    long timesteps = 24;
    time_t endTime = startTime + (timesteps-1) * timestep;
    std::set<std::string> timeList;

    const char *timestepStr = attributeList.getAttributeValue("timestep");
    const char *timestepsStr = attributeList.getAttributeValue("timesteps");
    const char *timezoneStr = attributeList.getAttributeValue("timezone");
    const char *startStepStr = attributeList.getAttributeValue("startstep");
    const char *startTimeStr = attributeList.getAttributeValue("starttime");
    const char *endTimeStr = attributeList.getAttributeValue("endtime");
    const char *hourStr = attributeList.getAttributeValue("hour");
    const char *timeStr = attributeList.getAttributeValue("time");
    const char *timeListStr = attributeList.getAttributeValue("timelist");

    if (timestepStr != nullptr)
    {
      if (strcasecmp(timestepStr,"data") == 0)
        query.mSearchType = Query::SearchType::TimeRange;
      else
        timestep = timePeriodToSeconds(timestepStr);
    }

    if (timezoneStr != nullptr)
    {
      query.mTimezone = timezoneStr;
    }


    if (timestepsStr != nullptr)
    {
      timesteps = toInt32(timestepsStr);
    }


    if (startStepStr != nullptr)
    {
      startStep = toInt32(startStepStr);
    }


    if (hourStr != nullptr)
    {
      std::vector<std::string> partList;
      splitString(hourStr,',',partList);
      for (auto it = partList.begin(); it != partList.end(); ++it)
      {
        sprintf(tmp,"%02d00",toInt32(it->c_str()));
        timeList.insert(tmp);
      }
    }


    if (timeStr != nullptr)
    {
      splitString(timeStr,',',timeList);
    }


    if (startTimeStr != nullptr)
    {
      if (strcasecmp(startTimeStr,"data") == 0)
      {
        startTime = 0;
        query.mFlags |= Query::Flags::StartTimeFromData;
        query.mSearchType = Query::SearchType::TimeRange;
      }
      else
      if (strcasecmp(startTimeStr,"now") == 0)
        startTime = currentTime;
      else
      {
        if (timezoneStr != nullptr)
          startTime = localTimeToTimeT(startTimeStr,timezoneStr);
        else
          startTime = localTimeToTimeT(startTimeStr,"");
      }
    }


    if (endTimeStr != nullptr)
    {
      if (strcasecmp(endTimeStr,"data") == 0)
      {
        endTime = 0;
        query.mFlags |= Query::Flags::EndTimeFromData;
        query.mSearchType = Query::SearchType::TimeRange;
      }
      else
      {
        if (timezoneStr != nullptr)
          endTime = localTimeToTimeT(endTimeStr,timezoneStr);
        else
          endTime = localTimeToTimeT(endTimeStr,"");
      }
    }


    if (timeListStr != nullptr)
    {
      std::vector<std::string> tList;
      splitString(timeListStr,',',tList);

      for (auto it = tList.begin(); it != tList.end(); ++it)
      {
        time_t tim = 0;
        if (timezoneStr != nullptr)
          tim = localTimeToTimeT(it->c_str(),timezoneStr);
        else
          tim = localTimeToTimeT(it->c_str(),"");

        struct tm tt;
        gmtime_r(&tim,&tt);
        sprintf(tmp,"%04d%02d%02dT%02d%02d%02d",tt.tm_year + 1900,tt.tm_mon + 1,tt.tm_mday,tt.tm_hour,tt.tm_min,tt.tm_sec);
        query.mForecastTimeList.insert(std::string(tmp));
      }
      startTime = 0;
      endTime = 0;
      timesteps = 0;
    }

    if (startTime > 0)
    {
      struct tm tt;
      gmtime_r(&startTime,&tt);

      sprintf(tmp,"%04d%02d%02dT000000",tt.tm_year + 1900,tt.tm_mon + 1,tt.tm_mday);
      time_t dayStartTime = localTimeToTimeT(tmp,"UTC");

      long diff = startTime - dayStartTime;
      long steps = (diff / timestep);
      if ((diff % timestep) != 0)
        startTime = dayStartTime + (steps+1)*timestep;

      if (endTimeStr == nullptr)
        endTime = startTime + (timesteps-1) * timestep;
      else
      if (timestepsStr == nullptr)
        timesteps = ((endTime-startTime) / timestep) + 1;

      gmtime_r(&startTime,&tt);
      sprintf(tmp,"%04d%02d%02dT%02d%02d%02d",tt.tm_year + 1900,tt.tm_mon + 1,tt.tm_mday,tt.tm_hour,tt.tm_min,tt.tm_sec);
      query.mStartTime = tmp;
    }
    else
    {
      query.mStartTime = "15000101T000000";
    }

    if (endTime > 0)
    {
      struct tm tt;
      gmtime_r(&endTime,&tt);
      sprintf(tmp,"%04d%02d%02dT%02d%02d%02d",tt.tm_year + 1900,tt.tm_mon + 1,tt.tm_mday,tt.tm_hour,tt.tm_min,tt.tm_sec);
      query.mEndTime = tmp;
    }
    else
    {
      query.mEndTime = "30000101T000000";
    }


    if (query.mSearchType != Query::SearchType::TimeRange  &&  timesteps > 0)
    {
      // This is a time step query. We should generate timeteps.

      time_t s = startTime;
      for (int t=0; t<timesteps; t++)
      {
        if (t >= startStep)
        {
          struct tm tt;
          gmtime_r(&s,&tt);

          sprintf(tmp,"%02d%02d",tt.tm_hour,tt.tm_min);
          if (timeList.size() == 0  ||  timeList.find(tmp) != timeList.end())
          {
            sprintf(tmp,"%04d%02d%02dT%02d%02d%02d",tt.tm_year + 1900,tt.tm_mon + 1,tt.tm_mday,tt.tm_hour,tt.tm_min,tt.tm_sec);
            query.mForecastTimeList.insert(std::string(tmp));
          }
        }
        s += timestep;
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void QueryConfigurator::configureLocation(Query& query,T::AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    const char *lonlatStr = attributeList.getAttributeValue("lonlat");
    const char *latlonStr = attributeList.getAttributeValue("latlon");
    const char *radiusStr = attributeList.getAttributeValue("radius");
    const char *coordinatesStr = attributeList.getAttributeValue("coordinates");

    if (coordinatesStr != nullptr)
    {
      parseCoordinates(coordinatesStr,',',';',query.mAreaCoordinates);
    }

    if (lonlatStr != nullptr)
    {
      std::vector<double> coordinateList;
      splitString(lonlatStr,',',coordinateList);
      if (coordinateList.size() != 2)
        throw Fmi::Exception(BCP, "Invalid number of coordinates in 'lonlat' parameter!");

      T::Coordinate_vec coordinates;
      coordinates.emplace_back(T::Coordinate(coordinateList[0],coordinateList[1]));
      query.mAreaCoordinates.emplace_back(coordinates);
    }

    if (latlonStr != nullptr)
    {
      std::vector<double> coordinateList;
      splitString(latlonStr,',',coordinateList);
      if (coordinateList.size() != 2)
        throw Fmi::Exception(BCP, "Invalid number of coordinates in 'latlon' parameter!");

      T::Coordinate_vec coordinates;
      coordinates.emplace_back(T::Coordinate(coordinateList[1],coordinateList[0]));
      query.mAreaCoordinates.emplace_back(coordinates);
    }

    if (radiusStr != nullptr)
    {
      query.mRadius = toDouble(radiusStr);

      //if (query.mAreaCoordinates.size() == 1  &&  query.mAreaCoordinates[0].size() == 1)
      //  query.mLocationType = QueryParameter::LocationType::Circle;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void QueryConfigurator::configureParameters(Query& query,T::AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    const char *levelsStr = attributeList.getAttributeValue("levels");
    const char *paramStr = attributeList.getAttributeValue("param");
    const char *typeStr = attributeList.getAttributeValue("type");
    const char *locationTypeStr = attributeList.getAttributeValue("locationType");
    const char *areaInterpolationMethodStr = attributeList.getAttributeValue("areaInterpolationMethod");
    const char *timeInterpolationMethodStr = attributeList.getAttributeValue("timeIntepolationMethod");
    const char *levelInterpolationMethodStr = attributeList.getAttributeValue("levelInterpolationMethod");


    std::set<double> levels;
    if (levelsStr != nullptr)
    {
      splitString(levelsStr,',',levels);
    }

    if (levels.size() == 0)
      levels.insert(0);

    uint levelCount = 0;
    for (auto level = levels.rbegin(); level != levels.rend(); ++level)
    {
      if (paramStr != nullptr)
      {
        std::vector<std::string> paramList;
        splitString(paramStr,',',paramList);

        for (auto p = paramList.begin(); p != paramList.end(); ++p)
        {
          QueryParameter param;
          //param.mId;
          if (typeStr != nullptr)
            param.mType = toUInt8(typeStr);

          if (locationTypeStr != nullptr)
            param.mLocationType = toUInt8(locationTypeStr);

          param.mParam = *p;
          param.mOrigParam = *p;
          //param.mSymbolicName = *p;
          //param.mParameterKeyType;
          //param.mParameterKey = *p;
          //param.mParameterLevelIdType;
          //param.mParameterLevelId;
          param.mParameterLevel = *level;
          //param.mForecastType;
          //param.mForecastNumber;
          if (areaInterpolationMethodStr)
            param.mAreaInterpolationMethod = toInt16(areaInterpolationMethodStr);

          if (timeInterpolationMethodStr)
            param.mTimeInterpolationMethod = toInt16(timeInterpolationMethodStr);

          if (levelInterpolationMethodStr)
            param.mLevelInterpolationMethod = toInt16(levelInterpolationMethodStr);

          //param.mContourLowValues;
          //param.mContourHighValues;
          //param.mContourSmoothSize;
          //param.mContourSmoothDegree;
          //param.mProducerId;
          //param.mGenerationFlags;
          //param.mPrecision;
          //param.mTemporary;
          //param.mFunction;
          //param.mFunctionParams;
          //param.mTimestepsBefore;
          //param.mTimestepsAfter;
          //param.mTimestepSizeInMinutes;
          //param.mFlags;

          if (AdditionalParameters::isAdditionalParameter(p->c_str()))
          {
            if (levelCount == 0)
            {
              param.mParameterKeyType = T::ParamKeyTypeValue::BUILD_IN;
              query.mQueryParameterList.emplace_back(param);
            }
          }
          else
          {
            configureParameter(query,param,attributeList);
            query.mQueryParameterList.emplace_back(param);
          }
        }
      }
      levelCount++;
    }

  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void QueryConfigurator::configureParameter(Query& query,QueryParameter& param,T::AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    char buf[1000];
    strcpy(buf,param.mOrigParam.c_str());

    char *p1 = strstr(buf,"(");
    if (p1 != nullptr)
    {
      p1++;
      char *p2 = strstr(p1,")");
      if (p2 != nullptr)
      {
        // The parameter is a timeseries function (but not a grid query function).

        *p2 = '\0';
        std::vector<std::string> partList;
        splitString(p1,':',partList);

        param.mParam = partList[0];
        param.mParameterKey = partList[0];
        param.mSymbolicName = partList[0];

        if (partList.size() > 1)
        {
          param.mTimestepSizeInMinutes = 60;
          param.mTimestepsBefore = (timePeriodToSeconds(partList[1].c_str()) / 3600);
        }

        if (partList.size() > 2)
        {
          param.mTimestepSizeInMinutes = 60;
          param.mTimestepsAfter = (timePeriodToSeconds(partList[2].c_str()) / 3600);
        }

        return;
      }
    }

    param.mParam = param.mOrigParam;
    param.mParameterKey = param.mOrigParam;
    param.mSymbolicName = param.mOrigParam;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}


}  // namespace QueryServer
}  // namespace SmartMet

