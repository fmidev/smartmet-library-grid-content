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
  QF_TIME_RANGE_QUERY  = 1 << 0,
  QF_AREA_QUERY        = 1 << 1
};



class Query
{
  public:
                              Query();
                              Query(Query& query);
    virtual                   ~Query();

    bool                      parameterInQuery(std::string param);
    QueryParameter*           getQueryParameterPtr(std::string param);
    QueryParameter*           getQueryParameterPtr(uint id);
    uint                      getValuesPerTimeStep();
    void                      removeTemporaryParameters();
    void                      print(std::ostream& stream,uint level,uint optionFlags);

    std::vector<std::string>  mProducerNameList;
    std::vector<std::string>  mForecastTimeList;
    QueryCoordinates          mCoordinateList;
    QueryParameter_vec        mQueryParameterList;
    std::string               mStartTime;
    std::string               mEndTime;
    uint                      mGenerationFlags;
    uint                      mFlags;
};


}  // namespace QueryServer
}  // namespace SmartMet

