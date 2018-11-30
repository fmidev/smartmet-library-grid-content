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

    uchar               mType;
    uchar               mSearchType;
    string_vec          mProducerNameList;
    string_set          mForecastTimeList;
    unsigned char       mLocationType;
    T::AttributeList    mAttributeList;
    T::AreaCoordinates  mAreaCoordinates;
    double              mRadius;
    QueryParameter_vec  mQueryParameterList;
    std::string         mTimezone;            // Needed when UTC times are converted back to local times.
    std::string         mStartTime;
    std::string         mEndTime;
    std::string         mAnalysisTime;
    std::string         mLanguage;
    uint                mGenerationFlags;
    uint                mFlags;
    uint                mMaxParameterValues;


    class LocationType
    {
      public:
        static const uchar Point      = 0;  // mAreaCoordinates[0][0]   => Singe x,y -coordinate
        static const uchar Polygon    = 1;  // mAreaCoordinates[0 - N]  => 1-N polygons
        static const uchar Path       = 2;  // mAreaCoordinates[0]      => List of x,y -coordinates
        static const uchar Circle     = 3;  // mAreaCoordinates[0][0] + radius
        static const uchar Grid       = 4;  // mGridWidth + mGridHeight + mAreaCoordinates[0]
        static const uchar GeometryId = 5;  // mGridGeometryId
        static const uchar Box        = 6;  // Bounding box => mAreaCoordinates[0][0] + mAreaCoordinates[0][1]
    };

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
        static const uint ReverseGenerationFlags  = 1 << 3;
    };

    class Type
    {
      public:
        static const uchar Data    = 0;
        static const uchar Isoline = 1;
        static const uchar Isoband = 2;
    };


};


typedef std::vector<Query> Query_vec;


}  // namespace QueryServer
}  // namespace SmartMet

