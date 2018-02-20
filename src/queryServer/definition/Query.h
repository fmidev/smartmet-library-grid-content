#pragma once

#include "QueryParameter.h"


namespace SmartMet
{
namespace QueryServer
{

typedef std::vector<T::Coordinate> Coordinate_vec;
typedef std::vector<Coordinate_vec> QueryCoordinates;


enum QueryFlags
{
  QF_TIME_RANGE_QUERY          = 1 << 0,
  QF_AREA_QUERY                = 1 << 1,
  QF_START_TIME_DATA           = 1 << 2,
  QF_REVERSE_GENERATION_FLAGS  = 1 << 3,
  QF_START_TIME_NOT_INCLUDED   = 1 << 4
};



class Query
{
  public:
                        Query();
                        Query(Query& query);
    virtual             ~Query();

    bool                parameterInQuery(std::string param);
    QueryParameter*     getQueryParameterPtr(std::string param);
    QueryParameter*     getQueryParameterPtr(uint id);
    uint                getValuesPerTimeStep();
    void                removeTemporaryParameters();
    void                print(std::ostream& stream,uint level,uint optionFlags);

    string_vec          mProducerNameList;
    string_set          mForecastTimeList;
    QueryCoordinates    mCoordinateList;
    double              mRadius;
    QueryParameter_vec  mQueryParameterList;
    std::string         mStartTime;
    std::string         mEndTime;
    std::string         mAnalysisTime;
    std::string         mLanguage;
    uint                mGenerationFlags;
    uint                mFlags;
    uint                mMaxParameterValues;
};


}  // namespace QueryServer
}  // namespace SmartMet

