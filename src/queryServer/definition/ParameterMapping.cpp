#include "ParameterMapping.h"
#include <grid-files/common/GeneralFunctions.h>


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
    mSearchEnabled = false;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





ParameterMapping::ParameterMapping(const ParameterMapping& mapping)
{
  try
  {
    mProducerName = mapping.mProducerName;
    mParameterName = mapping.mParameterName;
    mParameterKeyType = mapping.mParameterKeyType;
    mParameterKey = mapping.mParameterKey;
    mParameterLevelIdType = mapping.mParameterLevelIdType;
    mParameterLevelId = mapping.mParameterLevelId;
    mParameterLevel = mapping.mParameterLevel;
    mAreaInterpolationMethod = mapping.mAreaInterpolationMethod;
    mTimeInterpolationMethod = mapping.mTimeInterpolationMethod;
    mLevelInterpolationMethod = mapping.mLevelInterpolationMethod;
    mGroupFlags = mapping.mGroupFlags;
    mSearchEnabled = mapping.mSearchEnabled;
    mConversionFunction = mapping.mConversionFunction;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





ParameterMapping::~ParameterMapping()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ParameterMapping::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ParameterMapping\n";
    stream << space(level) << "- mProducerName             = " << mProducerName << "\n";
    stream << space(level) << "- mParameterName            = " << mParameterName << "\n";
    stream << space(level) << "- mParameterKeyType         = " << mParameterKeyType << "\n";
    stream << space(level) << "- mParameterKey             = " << mParameterKey << "\n";
    stream << space(level) << "- mParameterLevelIdType     = " << mParameterLevelIdType << "\n";
    stream << space(level) << "- mParameterLevelId         = " << mParameterLevelId << "\n";
    stream << space(level) << "- mParameterLevel           = " << mParameterLevel << "\n";
    stream << space(level) << "- mAreaInterpolationMethod  = " << mAreaInterpolationMethod << "\n";
    stream << space(level) << "- mTimeInterpolationMethod  = " << mTimeInterpolationMethod << "\n";
    stream << space(level) << "- mLevelInterpolationMethod = " << mLevelInterpolationMethod << "\n";
    stream << space(level) << "- mGroupFlags               = " << mGroupFlags << "\n";
    stream << space(level) << "- mSearchEnabled            = " << mSearchEnabled << "\n";
    stream << space(level) << "- mConversionFunction       = " << mConversionFunction << "\n";
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}



}  // namespace QueryServer
}  // namespace SmartMet
