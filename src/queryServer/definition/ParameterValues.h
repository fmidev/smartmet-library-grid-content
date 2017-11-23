#pragma once

#include <grid-files/grid/Typedefs.h>
#include <grid-files/grid/GridValueList.h>



namespace SmartMet
{
namespace QueryServer
{


class ParameterValues
{
  public:
                            ParameterValues();
                            ParameterValues(const ParameterValues& parameterValues);
    virtual                 ~ParameterValues();

    void                    print(std::ostream& stream,uint level,uint optionFlags);

    std::string             mForecastTime;
    uint                    mProducerId;
    uint                    mGenerationId;
    uint                    mGeometryId;

    T::ParamKeyType         mParameterKeyType;
    T::ParamId              mParameterKey;
    T::ParamLevelIdType     mParameterLevelIdType;
    T::ParamLevelId         mParameterLevelId;
    T::ParamLevel           mParameterLevel;
    T::ForecastType         mForecastType;
    T::ForecastNumber       mForecastNumber;

    T::GridValueList        mValueList;
};


typedef std::vector<ParameterValues> ParameterValues_vec;


}  // namespace QueryServer
}  // namespace SmartMet

