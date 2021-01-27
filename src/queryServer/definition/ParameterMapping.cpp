#include "ParameterMapping.h"
#include <grid-files/common/GeneralFunctions.h>
#include <boost/functional/hash.hpp>


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
    boost::hash_combine(seed,mProducerName);
    boost::hash_combine(seed,mParameterName);
    boost::hash_combine(seed,C_INT(mParameterKeyType));
    boost::hash_combine(seed,mParameterKey);
    boost::hash_combine(seed,mGeometryId);
    boost::hash_combine(seed,C_INT(mParameterLevelIdType));
    boost::hash_combine(seed,C_INT(mParameterLevelId));
    boost::hash_combine(seed,mParameterLevel);
    boost::hash_combine(seed,mAreaInterpolationMethod);
    boost::hash_combine(seed,mTimeInterpolationMethod);
    boost::hash_combine(seed,mLevelInterpolationMethod);
    boost::hash_combine(seed,mGroupFlags);
    boost::hash_combine(seed,mSearchEnabled);
    boost::hash_combine(seed,mIgnore);
    boost::hash_combine(seed,mConversionFunction);
    boost::hash_combine(seed,mReverseConversionFunction);
    boost::hash_combine(seed,mDefaultPrecision);
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

    boost::hash_combine(mParameterKeyHash,C_INT(mParameterKeyType));
    boost::hash_combine(mParameterKeyHash,mParameterKey);

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
    stream << space(level) << "- mParameterLevelIdType      = " << C_INT(mParameterLevelIdType) << "\n";
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
