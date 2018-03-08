#pragma once

#include <grid-files/grid/Typedefs.h>


namespace SmartMet
{
namespace QueryServer
{


class ParameterMapping
{
  public:
                                ParameterMapping();
                                ParameterMapping(const ParameterMapping& mapping);
    virtual                     ~ParameterMapping();

    void                        print(std::ostream& stream,uint level,uint optionFlags);

    std::string                 mProducerName;
    std::string                 mParameterName;
    T::ParamKeyType             mParameterKeyType;
    T::ParamId                  mParameterKey;
    T::ParamLevelIdType         mParameterLevelIdType;
    T::ParamLevelId             mParameterLevelId;
    T::ParamLevel               mParameterLevel;
    T::AreaInterpolationMethod  mAreaInterpolationMethod;
    T::TimeInterpolationMethod  mTimeInterpolationMethod;
    T::LevelInterpolationMethod mLevelInterpolationMethod;
    bool                        mSearchEnabled;
    std::string                 mConversionFunction;
};



typedef std::vector<ParameterMapping> ParameterMapping_vec;


}  // namespace QueryServer
}  // namespace SmartMet
