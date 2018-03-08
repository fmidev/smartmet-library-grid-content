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
    mParameterKeyType = T::ParamKeyType::FMI_ID;
    mParameterLevelId = 0;
    mParameterLevel = 0;
    mAreaInterpolationMethod = T::AreaInterpolationMethod::Linear;
    mTimeInterpolationMethod = T::TimeInterpolationMethod::Linear;
    mLevelInterpolationMethod = T::LevelInterpolationMethod::Linear;
    mSearchEnabled = false;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
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
    mSearchEnabled = mapping.mSearchEnabled;
    mConversionFunction = mapping.mConversionFunction;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





ParameterMapping::~ParameterMapping()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ParameterMapping::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ParameterMapping\n";
    stream << space(level) << "- mProducerName             = " << mProducerName << "\n";
    stream << space(level) << "- mParameterName            = " << mParameterName << "\n";
    stream << space(level) << "- mParameterKeyType         = " << (int)mParameterKeyType << "\n";
    stream << space(level) << "- mParameterKey             = " << mParameterKey << "\n";
    stream << space(level) << "- mParameterLevelIdType     = " << (int)mParameterLevelIdType << "\n";
    stream << space(level) << "- mParameterLevelId         = " << (int)mParameterLevelId << "\n";
    stream << space(level) << "- mParameterLevel           = " << mParameterLevel << "\n";
    stream << space(level) << "- mAreaInterpolationMethod  = " << (int)mAreaInterpolationMethod << "\n";
    stream << space(level) << "- mTimeInterpolationMethod  = " << (int)mTimeInterpolationMethod << "\n";
    stream << space(level) << "- mLevelInterpolationMethod = " << (int)mLevelInterpolationMethod << "\n";
    stream << space(level) << "- mSearchEnabled            = " << mSearchEnabled << "\n";
    stream << space(level) << "- mConversionFunction       = " << mConversionFunction << "\n";
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}



}  // namespace QueryServer
}  // namespace SmartMet
