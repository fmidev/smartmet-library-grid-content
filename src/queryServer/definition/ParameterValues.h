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
    uint                    mGenerationFlags;
    uint                    mGeometryId;
    std::string             mAnalysisTime;
    std::string             mModificationTime;

    T::ParamKeyType         mParameterKeyType;
    T::ParamId              mParameterKey;
    T::ParamLevelIdType     mParameterLevelIdType;
    T::ParamLevelId         mParameterLevelId;
    T::ParamLevel           mParameterLevel;
    T::ForecastType         mForecastType;
    T::ForecastNumber       mForecastNumber;
    uint                    mFlags;
    T::GridValueList        mValueList;
    T::ParamValue_vec       mValueVector;
    T::ByteData_vec         mValueData;


    class Flags
    {
      public:
        static const uint  AdditionalValue                       = 1 << 0;  // Value is added in order to fill missing times
        static const uint  AggregationValue                      = 1 << 1;  // Value is added for aggregation purposes
        static const uint  DataAvailable                         = 1 << 2;
        static const uint  DataAvailableByTimeInterpolation      = 1 << 3;
        static const uint  DataAvailableByLevelInterpolation     = 1 << 4;
    };

};


typedef std::vector<ParameterValues> ParameterValues_vec;


}  // namespace QueryServer
}  // namespace SmartMet

