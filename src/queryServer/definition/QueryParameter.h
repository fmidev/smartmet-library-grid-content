#pragma once

#include "ParameterValues.h"
#include <grid-files/common/AttributeList.h>


namespace SmartMet
{
namespace QueryServer
{

typedef std::pair<uint,std::string> FunctionParam;
typedef std::vector<FunctionParam> FunctionParam_vec;
typedef std::vector<uint> Color_vec;

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
    uchar                       mType;
    uchar                       mLocationType;
    std::string                 mParam;
    std::string                 mOrigParam;
    std::string                 mSymbolicName;
    T::ParamKeyType             mParameterKeyType;
    T::ParamId                  mParameterKey;
    std::string                 mProducerName;
    T::GeometryId               mGeometryId;
    T::ParamLevelIdType         mParameterLevelIdType;
    T::ParamLevelId             mParameterLevelId;
    T::ParamLevel               mParameterLevel;
    T::ForecastType             mForecastType;
    T::ForecastNumber           mForecastNumber;
    short                       mAreaInterpolationMethod;
    short                       mTimeInterpolationMethod;
    short                       mLevelInterpolationMethod;
    T::ParamValue_vec           mContourLowValues;
    T::ParamValue_vec           mContourHighValues;
    Color_vec                   mContourColors;
    uint                        mProducerId;
    uint                        mGenerationFlags;
    short                       mPrecision;
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

