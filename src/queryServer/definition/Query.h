#pragma once

#include "QueryParameter.h"
#include <grid-files/common/AttributeList.h>
#include <grid-files/common/Coordinate.h>


namespace SmartMet
{
namespace QueryServer
{

// ====================================================================================
/*! \brief A complete QueryServer request specifying producers, coordinates, time range, and parameters.
 *
 *  Carries all context needed to execute a single query: the coordinate set
 *  (point, polygon, path, or grid), the requested forecast time range or explicit
 *  time list, the list of QueryParameter specifications, and various control flags
 *  that govern matching behaviour. */
// ====================================================================================

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
    void                getResultProducerIdList(std::set<T::ProducerId>& producerIdList);
    void                removeAggregationValues();
    void                removeInternalAggregationValues();

    void                removeAggregationTimes();

    void                removeTemporaryParameters();
    void                updateForecastTimeList();
    void                print(std::ostream& stream,uint level,uint optionFlags);

    uchar               mSearchType;               //!< Time search mode: SearchType::TimeSteps or TimeRange.
    string_vec          mProducerNameList;          //!< Ordered list of producer names to query, tried in order.
    std::set<time_t>    mForecastTimeList;          //!< Explicit set of UTC forecast times (time_t) to retrieve.
    T::AttributeList    mAttributeList;             //!< Additional key-value attributes controlling query behaviour.
    T::CoordinateType   mCoordinateType;            //!< Coordinate system of mAreaCoordinates (latlon, grid, …).
    T::AreaCoordinates  mAreaCoordinates;           //!< Spatial specification: points, polygon rings, or grid coords.
    double              mRadius;                    //!< Circle radius in km, used when location type is Circle.
    QueryParameter_vec  mQueryParameterList;        //!< List of individual parameter requests within this query.
    std::string         mTimezone;                  //!< Timezone for converting UTC times back to local time strings.
    time_t              mStartTime;                 //!< Start of the requested time range (UTC epoch seconds).
    time_t              mEndTime;                   //!< End of the requested time range (UTC epoch seconds).
    uint                mTimesteps;                 //!< Maximum number of time steps to return.
    uint                mTimestepSizeInMinutes;     //!< Step size in minutes between returned forecast times.
    std::string         mAnalysisTime;              //!< Required analysis (reference) time string, empty = any.
    T::GeometryId_set   mGeometryIdList;            //!< Restrict results to these geometry identifiers, empty = any.
    std::string         mLanguage;                  //!< Language code for localised string output (e.g. symbol names).
    UInt64              mGenerationFlags;           //!< Flags controlling which model generations are considered.
    uint                mFlags;                     //!< Bitmask from Flags controlling time-range and generation matching.
    uint                mMaxParameterValues;        //!< Maximum number of ParameterValues records returned per parameter.
    std::set<time_t>    mAggregationTimes;          //!< Additional time steps inserted for aggregation (not in output).
    time_t              mProcessingStartTime;       //!< Epoch time when query execution began (for timeout tracking).
    uint                mMaxProcessingTimeInSeconds; //!< Maximum allowed query execution time in seconds.



    // ====================================================================================
    /*! \brief Constants for mSearchType. */
    // ====================================================================================
    class SearchType
    {
      public:
        static const uchar TimeSteps  = 0;  //!< Return values at explicit time steps from mForecastTimeList.
        static const uchar TimeRange  = 1;  //!< Return all values within [mStartTime, mEndTime].
    };


    // ====================================================================================
    /*! \brief Bit-flag constants for mFlags. */
    // ====================================================================================
    class Flags
    {
      public:
        static const uint StartTimeFromData               = 1 << 0;   //!< Set mStartTime from the earliest available data time.
        static const uint EndTimeFromData                 = 1 << 1;   //!< Set mEndTime from the latest available data time.
        static const uint StartTimeNotIncluded            = 1 << 2;   //!< Exclude the start time from results.
        static const uint EndTimeNotIncluded              = 1 << 3;   //!< Exclude the end time from results.
        static const uint ReverseGenerationFlags          = 1 << 4;   //!< Invert the generation flag matching logic.
        static const uint GeometryHitNotRequired          = 1 << 5;   //!< Accept results even if coordinates miss the geometry.
        static const uint SameAnalysisTime                = 1 << 6;   //!< All parameters must share the same analysis time.
        static const uint AnalysisTimeMatchRequired       = 1 << 7;   //!< mAnalysisTime must match exactly.
        static const uint AcceptNotReadyGenerations       = 1 << 8;   //!< Include generations not yet marked ready.
        static const uint AcceptNotReadyGeometries        = 1 << 9;   //!< Include geometries not yet marked ready.
        static const uint LatestGeneration                = 1 << 10;  //!< Use only the most recent generation.
        static const uint OldestGeneration                = 1 << 11;  //!< Use only the oldest available generation.
        static const uint TimeStepIsData                  = 1 << 12;  //!< Time steps are taken from available data times.
        static const uint ForceStartTime                  = 1 << 13;  //!< Apply mStartTime even when StartTimeFromData is set.
    };

};


typedef std::vector<Query> Query_vec;  //!< Ordered list of Query objects.


}  // namespace QueryServer
}  // namespace SmartMet

