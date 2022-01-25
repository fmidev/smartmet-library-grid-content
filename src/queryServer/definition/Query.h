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
                        //Query(const Query& query);
    virtual             ~Query();

    bool                parameterInQuery(const std::string& param);
    QueryParameter*     getQueryParameterPtr(const std::string& param);
    QueryParameter*     getQueryParameterPtr(uint id);
    uint                getValuesPerTimeStep();
    std::size_t         getHash();
    void                getResultProducerIdList(std::set<uint>& producerIdList);
    void                removeTemporaryParameters();
    void                print(std::ostream& stream,uint level,uint optionFlags);

    uchar               mSearchType;
    string_vec          mProducerNameList;
    std::set<time_t>    mForecastTimeList; // UTC times in time_t
    T::AttributeList    mAttributeList;
    T::CoordinateType   mCoordinateType;
    T::AreaCoordinates  mAreaCoordinates;
    double              mRadius;
    QueryParameter_vec  mQueryParameterList;
    std::string         mTimezone;            // Needed when UTC times are converted back to local times.
    time_t              mStartTime;           // UTC time in time_t
    time_t              mEndTime;             // UTC time in time_t
    uint                mTimesteps;
    uint                mTimestepSizeInMinutes;
    std::string         mAnalysisTime;
    T::GeometryId_set   mGeometryIdList;
    std::string         mLanguage;
    ulonglong           mGenerationFlags;
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
        static const uint StartTimeFromData               = 1 << 0;
        static const uint EndTimeFromData                 = 1 << 1;
        static const uint StartTimeNotIncluded            = 1 << 2;
        static const uint EndTimeNotIncluded              = 1 << 3;
        static const uint ReverseGenerationFlags          = 1 << 4;
        static const uint GeometryHitNotRequired          = 1 << 5;
        static const uint SameAnalysisTime                = 1 << 6;
        static const uint AnalysisTimeMatchRequired       = 1 << 7;
        static const uint AcceptNotReadyGenerations       = 1 << 8;
    };

};


typedef std::vector<Query> Query_vec;


}  // namespace QueryServer
}  // namespace SmartMet

