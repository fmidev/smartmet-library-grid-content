#pragma once

#include "ParameterValues.h"


namespace SmartMet
{
namespace QueryServer
{

typedef std::pair<uint,std::string> FunctionParam;
typedef std::vector<FunctionParam> FunctionParam_vec;

enum QueryParameterFlags
{
  QPF_PRESSURE_LEVEL_INTERPOLATION_ENABLED   = 1 << 0,
  QPF_HEIGHT_LEVEL_INTERPOLATION_ENABLED     = 1 << 1
};



class QueryParameter
{
  public:
                                QueryParameter();
                                QueryParameter(const QueryParameter& queryParameter);
    virtual                     ~QueryParameter();

    QueryParameter*             duplicate();
    void                        print(std::ostream& stream,uint level,uint optionFlags);

    uint                        mId;
    std::string                 mParam;
    std::string                 mOrigParam;
    std::string                 mSymbolicName;
    T::ParamKeyType             mParameterKeyType;
    T::ParamId                  mParameterKey;
    T::ParamLevelIdType         mParameterLevelIdType;
    T::ParamLevelId             mParameterLevelId;
    T::ParamLevel               mParameterLevel;
    T::ForecastType             mForecastType;
    T::ForecastNumber           mForecastNumber;
    T::AreaInterpolationMethod  mAreaInterpolationMethod;
    T::TimeInterpolationMethod  mTimeInterpolationMethod;
    T::LevelInterpolationMethod mLevelInterpolationMethod;
    uint                        mProducerId;
    uint                        mGenerationFlags;
    int                         mPrecision;
    bool                        mTemporary;
    std::string                 mFunction;
    FunctionParam_vec           mFunctionParams;
    uint                        mTimestepsBefore;
    uint                        mTimestepsAfter;
    uint                        mTimestepSizeInMinutes;
    uint                        mFlags;
    ParameterValues_vec         mValueList;
};


typedef std::vector<QueryParameter> QueryParameter_vec;

}  // namespace QueryServer
}  // namespace SmartMet

