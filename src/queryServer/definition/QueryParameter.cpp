#include "QueryParameter.h"
#include "grid-files/common/GeneralFunctions.h"

namespace SmartMet
{
namespace QueryServer
{


QueryParameter::QueryParameter()
{
  try
  {
    mId = 0;
    mParameterKeyType = T::ParamKeyType::UNKNOWN;
    mParameterLevelIdType = T::ParamLevelIdType::ANY;
    mParameterLevelId = 0;
    mParameterLevel = 0;
    mForecastType = -1;
    mForecastNumber = -1;
    mInterpolationMethod = T::InterpolationMethod::Linear;
    mTemporary = false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





QueryParameter::QueryParameter(const QueryParameter& queryParameter)
{
  try
  {
    mId = queryParameter.mId;
    mParam = queryParameter.mParam;
    mSymbolicName = queryParameter.mSymbolicName;
    mParameterKeyType = queryParameter.mParameterKeyType;
    mParameterKey = queryParameter.mParameterKey;
    mParameterLevelIdType = queryParameter.mParameterLevelIdType;
    mParameterLevelId = queryParameter.mParameterLevelId;
    mParameterLevel = queryParameter.mParameterLevel;
    mForecastType = queryParameter.mForecastType;
    mForecastNumber = queryParameter.mForecastNumber;
    mInterpolationMethod = queryParameter.mInterpolationMethod;
    mValueList = queryParameter.mValueList;
    mTemporary = queryParameter.mTemporary;
    mFunction = queryParameter.mFunction;
    mFunctionParams = queryParameter.mFunctionParams;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





QueryParameter::~QueryParameter()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





QueryParameter* QueryParameter::duplicate()
{
  try
  {
    return new QueryParameter(*this);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void QueryParameter::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "QueryParameter\n";
    stream << space(level) << "- mId                   = " << mId << "\n";
    stream << space(level) << "- mParam                = " << mParam << "\n";
    stream << space(level) << "- mSymbolicName         = " << mSymbolicName << "\n";
    stream << space(level) << "- mParameterKeyType     = " << (uint)mParameterKeyType << "\n";
    stream << space(level) << "- mParameterKey         = " << mParameterKey << "\n";
    stream << space(level) << "- mParameterLevelIdType = " << (uint)mParameterLevelIdType << "\n";
    stream << space(level) << "- mParameterLevelId     = " << (int)mParameterLevelId << "\n";
    stream << space(level) << "- mParameterLevel       = " << mParameterLevel << "\n";
    stream << space(level) << "- mForecastType         = " << (int)mForecastType << "\n";
    stream << space(level) << "- mForecastNumber       = " << (int)mForecastNumber << "\n";
    stream << space(level) << "- mInterpolationMethod  = " << (uint)mInterpolationMethod << "\n";
    stream << space(level) << "- mTemporary            = " << (int)mTemporary << "\n";
    stream << space(level) << "- mFunction             = " << mFunction << "\n";

    stream << space(level) << "- mFunctionParams       = \n";
    for (auto it = mFunctionParams.begin();  it != mFunctionParams.end(); ++it)
    {
      stream << space(level) << "   * " << it->first << ":" << it->second << "\n";
    }

    stream << space(level) << "- mValueList            = \n";

    for (auto it = mValueList.begin();  it != mValueList.end(); ++it)
    {
      it->print(stream,level+2,optionFlags);
    }

  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}






}  // namespace QueryServer
}  // namespace SmartMet

