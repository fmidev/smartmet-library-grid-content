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

    virtual std::size_t         getHash();

    void                        print(std::ostream& stream,uint level,uint optionFlags);

    std::string                 mProducerName;
    std::string                 mParameterName;
    T::ParamKeyType             mParameterKeyType;
    T::ParamId                  mParameterKey;
    T::GeometryId               mGeometryId;
    T::ParamLevelIdType         mParameterLevelIdType;
    T::ParamLevelId             mParameterLevelId;
    T::ParamLevel               mParameterLevel;
    short                       mAreaInterpolationMethod;
    short                       mTimeInterpolationMethod;
    short                       mLevelInterpolationMethod;
    uint                        mGroupFlags;
    bool                        mSearchEnabled;
    bool                        mIgnore;
    std::string                 mConversionFunction;
    std::string                 mReverseConversionFunction;
    short                       mDefaultPrecision;


    class GroupFlags
    {
      public:
        static const uint       climatology = 1 << 0;
    };
};





typedef std::vector<ParameterMapping> ParameterMapping_vec;


}  // namespace QueryServer
}  // namespace SmartMet
