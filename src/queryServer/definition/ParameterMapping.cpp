#include "ParameterMapping.h"
#include <grid-files/common/GeneralFunctions.h>
#include <macgyver/Hash.h>


namespace SmartMet
{
namespace QueryServer
{



ParameterMapping::ParameterMapping()
{
  try
  {
    mParameterKeyType = T::ParamKeyTypeValue::FMI_ID;
    mParameterLevelId = 0;
    mParameterLevel = 0;
    mAreaInterpolationMethod = T::AreaInterpolationMethod::Undefined;
    mTimeInterpolationMethod = T::TimeInterpolationMethod::Undefined;
    mLevelInterpolationMethod = T::LevelInterpolationMethod::Undefined;
    mGroupFlags = 0;
    mGeometryId = -1;
    mSearchEnabled = false;
    mIgnore = false;
    mDefaultPrecision = -1;
    mParameterKeyHash = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*
ParameterMapping::ParameterMapping(const ParameterMapping& mapping)
{
  try
  {
    mProducerName = mapping.mProducerName;
    mParameterName = mapping.mParameterName;
    mParameterKeyType = mapping.mParameterKeyType;
    mParameterKey = mapping.mParameterKey;
    mGeometryId = mapping.mGeometryId;
    mParameterLevelIdType = mapping.mParameterLevelIdType;
    mParameterLevelId = mapping.mParameterLevelId;
    mParameterLevel = mapping.mParameterLevel;
    mAreaInterpolationMethod = mapping.mAreaInterpolationMethod;
    mTimeInterpolationMethod = mapping.mTimeInterpolationMethod;
    mLevelInterpolationMethod = mapping.mLevelInterpolationMethod;
    mGroupFlags = mapping.mGroupFlags;
    mSearchEnabled = mapping.mSearchEnabled;
    mIgnore = mapping.mIgnore;
    mConversionFunction = mapping.mConversionFunction;
    mReverseConversionFunction = mapping.mReverseConversionFunction;
    mDefaultPrecision = mapping.mDefaultPrecision;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}
*/




ParameterMapping::~ParameterMapping()
{
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





std::size_t ParameterMapping::getHash()
{
  try
  {
    std::size_t seed = 0;
    Fmi::hash_merge(seed,mProducerName);
    Fmi::hash_merge(seed,mParameterName);
    Fmi::hash_merge(seed,C_INT(mParameterKeyType));
    Fmi::hash_merge(seed,mParameterKey);
    Fmi::hash_merge(seed,mGeometryId);
    //Fmi::hash_merge(seed,C_INT(mParameterLevelIdType));
    Fmi::hash_merge(seed,C_INT(mParameterLevelId));
    Fmi::hash_merge(seed,mParameterLevel);
    Fmi::hash_merge(seed,mAreaInterpolationMethod);
    Fmi::hash_merge(seed,mTimeInterpolationMethod);
    Fmi::hash_merge(seed,mLevelInterpolationMethod);
    Fmi::hash_merge(seed,mGroupFlags);
    Fmi::hash_merge(seed,mSearchEnabled);
    Fmi::hash_merge(seed,mIgnore);
    Fmi::hash_merge(seed,mConversionFunction);
    Fmi::hash_merge(seed,mReverseConversionFunction);
    Fmi::hash_merge(seed,mDefaultPrecision);
    return seed;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




std::size_t ParameterMapping::getKeyHash()
{
  try
  {
    if (mParameterKeyHash != 0)
      return mParameterKeyHash;

    Fmi::hash_merge(mParameterKeyHash,C_INT(mParameterKeyType));
    Fmi::hash_merge(mParameterKeyHash,mParameterKey);

    return mParameterKeyHash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ParameterMapping::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ParameterMapping\n";
    stream << space(level) << "- mProducerName              = " << mProducerName << "\n";
    stream << space(level) << "- mParameterName             = " << mParameterName << "\n";
    stream << space(level) << "- mParameterKeyType          = " << C_INT(mParameterKeyType) << "\n";
    stream << space(level) << "- mParameterKey              = " << mParameterKey << "\n";
    stream << space(level) << "- mParameterKeyHash          = " << mParameterKeyHash << "\n";
    stream << space(level) << "- mGeometryId                = " << mGeometryId << "\n";
    //stream << space(level) << "- mParameterLevelIdType      = " << C_INT(mParameterLevelIdType) << "\n";
    stream << space(level) << "- mParameterLevelId          = " << C_INT(mParameterLevelId) << "\n";
    stream << space(level) << "- mParameterLevel            = " << mParameterLevel << "\n";
    stream << space(level) << "- mAreaInterpolationMethod   = " << mAreaInterpolationMethod << "\n";
    stream << space(level) << "- mTimeInterpolationMethod   = " << mTimeInterpolationMethod << "\n";
    stream << space(level) << "- mLevelInterpolationMethod  = " << mLevelInterpolationMethod << "\n";
    stream << space(level) << "- mGroupFlags                = " << mGroupFlags << "\n";
    stream << space(level) << "- mSearchEnabled             = " << mSearchEnabled << "\n";
    stream << space(level) << "- mIgnore                    = " << mIgnore << "\n";
    stream << space(level) << "- mConversionFunction        = " << mConversionFunction << "\n";
    stream << space(level) << "- mReverseConversionFunction = " << mReverseConversionFunction << "\n";
    stream << space(level) << "- mDefaultPrecision          = " << mDefaultPrecision << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



}  // namespace QueryServer
}  // namespace SmartMet
