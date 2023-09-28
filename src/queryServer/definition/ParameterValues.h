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
                            //ParameterValues(const ParameterValues& parameterValues);
    virtual                 ~ParameterValues();

    void                    clear();
    void                    print(std::ostream& stream,uint level,uint optionFlags);

    //std::string             mForecastTime;
    time_t                  mForecastTimeUTC;
    uint                    mProducerId;
    uint                    mGenerationId;
    ulonglong               mGenerationFlags;
    uint                    mGeometryId;
    uint                    mFileId[4];
    uint                    mMessageIndex[4];
    std::string             mAnalysisTime;
    time_t                  mModificationTime;

    T::ParamKeyType         mParameterKeyType;
    T::ParamId              mParameterKey;
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
        static const uint  InternalAggregationValue              = 1 << 5;  // Value is added internally for aggregation purposes
    };

};


typedef std::shared_ptr<ParameterValues> ParameterValues_sptr;
typedef std::vector<ParameterValues> ParameterValues_vec;
typedef std::vector<ParameterValues_sptr> ParameterValues_sptr_vec;


}  // namespace QueryServer
}  // namespace SmartMet

