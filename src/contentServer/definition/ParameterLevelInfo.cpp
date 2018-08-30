#include "ParameterLevelInfo.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{


ParameterLevelInfo::ParameterLevelInfo()
{
  try
  {
    mParameterKeyType = T::ParamKeyTypeValue::FMI_ID;
    mParameterLevelIdType = T::ParamLevelIdTypeValue::ANY;
    mParameterLevelId = 0;
    mLevel = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ParameterLevelInfo::ParameterLevelInfo(ParameterLevelInfo& levelInfo)
{
  try
  {
    mParameterKeyType = levelInfo.mParameterKeyType;
    mParameterKey = levelInfo.mParameterKey;
    mParameterLevelIdType = levelInfo.mParameterLevelIdType;
    mParameterLevelId = levelInfo.mParameterLevelId;
    mLevel = levelInfo.mLevel;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ParameterLevelInfo::ParameterLevelInfo(T::ParamKeyType parameterKeyType,
                   T::ParamId parameterKey,
                   T::ParamLevelIdType parameterLevelIdType,
                   T::ParamLevelId parameterLevelId,
                   T::ParamLevel level)
{
  try
  {
    mParameterKeyType = parameterKeyType;
    mParameterKey = parameterKey;
    mParameterLevelIdType = parameterLevelIdType;
    mParameterLevelId = parameterLevelId;
    mLevel = level;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ParameterLevelInfo::~ParameterLevelInfo()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ParameterLevelInfo& ParameterLevelInfo::operator=(ParameterLevelInfo& levelInfo)
{
  try
  {
    if (&levelInfo == this)
      return *this;

    mParameterKeyType = levelInfo.mParameterKeyType;
    mParameterKey = levelInfo.mParameterKey;
    mParameterLevelIdType = levelInfo.mParameterLevelIdType;
    mParameterLevelId = levelInfo.mParameterLevelId;
    mLevel = levelInfo.mLevel;

    return *this;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ParameterLevelInfo* ParameterLevelInfo::duplicate()
{
  try
  {
    return new ParameterLevelInfo(*this);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void ParameterLevelInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ParameterLevelInfo\n";
    stream << space(level) << "- mParameterKeyType     = " << mParameterKeyType << "\n";
    stream << space(level) << "- mParameterKey         = " << mParameterKey << "\n";
    stream << space(level) << "- mParameterLevelIdType = " << mParameterLevelIdType << "\n";
    stream << space(level) << "- mParameterLevelId     = " << mParameterLevelId << "\n";
    stream << space(level) << "- mLevel                = " << mLevel << "\n";
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




}
}
