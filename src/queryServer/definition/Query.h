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
  QF_TIME_RANGE_QUERY                       = 1 << 0,
  QF_START_TIME_DATA                        = 1 << 1,
  QF_REVERSE_GENERATION_FLAGS               = 1 << 2,
  QF_START_TIME_NOT_INCLUDED                = 1 << 3
};


enum QuerySearchType
{
  QST_POINT   = 0,
  QST_POLYGON = 1,
  QST_PATH    = 2,
  QST_CIRCLE  = 3
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
    QuerySearchType     mSearchType;
    QueryCoordinates    mCoordinateList;
    double              mRadius;
    double              mDem;
    ushort              mCoverType;
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

