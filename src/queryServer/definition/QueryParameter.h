#pragma once

#include "ParameterValues.h"


namespace SmartMet
{
namespace QueryServer
{


typedef std::pair<uint,std::string> FunctionParam;
typedef std::vector<FunctionParam> FunctionParam_vec;

class QueryParameter
{
  public:
                            QueryParameter();
                            QueryParameter(const QueryParameter& queryParameter);
    virtual                 ~QueryParameter();

    QueryParameter*         duplicate();
    void                    print(std::ostream& stream,uint level,uint optionFlags);

    uint                    mId;
    std::string             mParam;
    std::string             mOrigParam;
    std::string             mSymbolicName;
    T::ParamKeyType         mParameterKeyType;
    T::ParamId              mParameterKey;
    T::ParamLevelIdType     mParameterLevelIdType;
    T::ParamLevelId         mParameterLevelId;
    T::ParamLevel           mParameterLevel;
    T::ForecastType         mForecastType;
    T::ForecastNumber       mForecastNumber;
    T::InterpolationMethod  mInterpolationMethod;
    uint                    mProducerId;
    uint                    mGenerationFlags;
    bool                    mTemporary;
    std::string             mFunction;
    FunctionParam_vec       mFunctionParams;
    ParameterValues_vec     mValueList;
};


typedef std::vector<QueryParameter> QueryParameter_vec;

}  // namespace QueryServer
}  // namespace SmartMet

