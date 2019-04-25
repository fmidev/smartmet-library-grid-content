#pragma once

#include "QueryParameter.h"
#include <grid-files/common/AttributeList.h>
#include <grid-files/common/Coordinate.h>


namespace SmartMet
{
namespace QueryServer
{

class Query
{
  public:
                        Query();
                        Query(const Query& query);
    virtual             ~Query();

    bool                parameterInQuery(const std::string& param);
    QueryParameter*     getQueryParameterPtr(const std::string& param);
    QueryParameter*     getQueryParameterPtr(uint id);
    uint                getValuesPerTimeStep();
    void                removeTemporaryParameters();
    void                print(std::ostream& stream,uint level,uint optionFlags);

    uchar               mSearchType;
    string_vec          mProducerNameList;
    string_set          mForecastTimeList;
    T::AttributeList    mAttributeList;
    T::CoordinateType   mCoordinateType;
    T::AreaCoordinates  mAreaCoordinates;
    double              mRadius;
    QueryParameter_vec  mQueryParameterList;
    std::string         mTimezone;            // Needed when UTC times are converted back to local times.
    std::string         mStartTime;
    std::string         mEndTime;
    uint                mTimesteps;
    uint                mTimestepSizeInMinutes;
    std::string         mAnalysisTime;
    T::GeometryId_set   mGeometryIdList;
    std::string         mLanguage;
    uint                mGenerationFlags;
    uint                mFlags;
    uint                mMaxParameterValues;


    class SearchType
    {
      public:
        static const uchar TimeSteps  = 0;
        static const uchar TimeRange  = 1;
    };


    class Flags
    {
      public:
        static const uint StartTimeFromData       = 1 << 0;
        static const uint EndTimeFromData         = 1 << 1;
        static const uint StartTimeNotIncluded    = 1 << 2;
        static const uint EndTimeNotIncluded      = 1 << 3;
        static const uint ReverseGenerationFlags  = 1 << 4;
    };

};


typedef std::vector<Query> Query_vec;


}  // namespace QueryServer
}  // namespace SmartMet

