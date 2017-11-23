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
    mParameterKeyType = T::ParamKeyType::FMI_ID;
    mParameterLevelIdType = T::ParamLevelIdType::ANY;
    mParameterLevelId = 0;
    mLevel = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ParameterLevelInfo::~ParameterLevelInfo()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ParameterLevelInfo::operator=(ParameterLevelInfo& levelInfo)
{
  try
  {
    if (&levelInfo == this)
      return;

    mParameterKeyType = levelInfo.mParameterKeyType;
    mParameterKey = levelInfo.mParameterKey;
    mParameterLevelIdType = levelInfo.mParameterLevelIdType;
    mParameterLevelId = levelInfo.mParameterLevelId;
    mLevel = levelInfo.mLevel;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void ParameterLevelInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ParameterLevelInfo\n";
    stream << space(level) << "- mParameterKeyType = " << (int)mParameterKeyType << "\n";
    stream << space(level) << "- mParameterKey = " << mParameterKey << "\n";
    stream << space(level) << "- mParameterLevelIdType = " << (int)mParameterLevelIdType << "\n";
    stream << space(level) << "- mParameterLevelId = " << (int)mParameterLevelId << "\n";
    stream << space(level) << "- mLevel = " << mLevel << "\n";
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




}
}
