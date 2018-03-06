#include "QueryParameter.h"
#include <grid-files/common/GeneralFunctions.h>


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
    mAreaInterpolationMethod = T::AreaInterpolationMethod::Undefined;
    mTimeInterpolationMethod = T::TimeInterpolationMethod::Undefined;
    mLevelInterpolationMethod = T::LevelInterpolationMethod::Undefined;
    mProducerId = 0;
    mGenerationFlags = 0;
    mPrecision = 0;
    mTemporary = false;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





QueryParameter::QueryParameter(const QueryParameter& queryParameter)
{
  try
  {
    mId = queryParameter.mId;
    mParam = queryParameter.mParam;
    mOrigParam = queryParameter.mOrigParam;
    mSymbolicName = queryParameter.mSymbolicName;
    mParameterKeyType = queryParameter.mParameterKeyType;
    mParameterKey = queryParameter.mParameterKey;
    mParameterLevelIdType = queryParameter.mParameterLevelIdType;
    mParameterLevelId = queryParameter.mParameterLevelId;
    mParameterLevel = queryParameter.mParameterLevel;
    mForecastType = queryParameter.mForecastType;
    mForecastNumber = queryParameter.mForecastNumber;
    mAreaInterpolationMethod = queryParameter.mAreaInterpolationMethod;
    mTimeInterpolationMethod = queryParameter.mTimeInterpolationMethod;
    mLevelInterpolationMethod = queryParameter.mLevelInterpolationMethod;
    mProducerId = queryParameter.mProducerId;
    mGenerationFlags = queryParameter.mGenerationFlags;
    mAdditionalTimeList = queryParameter.mAdditionalTimeList;
    mValueList = queryParameter.mValueList;
    mPrecision = queryParameter.mPrecision;
    mTemporary = queryParameter.mTemporary;
    mFunction = queryParameter.mFunction;
    mFunctionParams = queryParameter.mFunctionParams;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





QueryParameter::~QueryParameter()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void QueryParameter::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "QueryParameter\n";
    stream << space(level) << "- mId                       = " << mId << "\n";
    stream << space(level) << "- mParam                    = " << mParam << "\n";
    stream << space(level) << "- mOrigParam                = " << mOrigParam << "\n";
    stream << space(level) << "- mSymbolicName             = " << mSymbolicName << "\n";
    stream << space(level) << "- mParameterKeyType         = " << (uint)mParameterKeyType << "\n";
    stream << space(level) << "- mParameterKey             = " << mParameterKey << "\n";
    stream << space(level) << "- mParameterLevelIdType     = " << (uint)mParameterLevelIdType << "\n";
    stream << space(level) << "- mParameterLevelId         = " << (int)mParameterLevelId << "\n";
    stream << space(level) << "- mParameterLevel           = " << mParameterLevel << "\n";
    stream << space(level) << "- mForecastType             = " << (int)mForecastType << "\n";
    stream << space(level) << "- mForecastNumber           = " << (int)mForecastNumber << "\n";
    stream << space(level) << "- mAreaInterpolationMethod      = " << (uint)mAreaInterpolationMethod << "\n";
    stream << space(level) << "- mTimeInterpolationMethod  = " << (uint)mTimeInterpolationMethod << "\n";
    stream << space(level) << "- mLevelInterpolationMethod = " << (uint)mLevelInterpolationMethod << "\n";
    stream << space(level) << "- mProducerId               = " << mProducerId << "\n";
    stream << space(level) << "- mGenerationFlags          = " << mGenerationFlags << "\n";
    stream << space(level) << "- mTemporary                = " << (int)mTemporary << "\n";
    stream << space(level) << "- mPrecision                = " << mPrecision << "\n";
    stream << space(level) << "- mFunction                 = " << mFunction << "\n";

    stream << space(level) << "- mFunctionParams           = \n";
    for (auto it = mFunctionParams.begin();  it != mFunctionParams.end(); ++it)
    {
      stream << space(level) << "   * " << it->first << ":" << it->second << "\n";
    }

    stream << space(level) << "- mAdditionalTimeList       = \n";

    for (auto it = mAdditionalTimeList.begin();  it != mAdditionalTimeList.end(); ++it)
    {
      stream << space(level) << "   * " << *it << "\n";
    }

    stream << space(level) << "- mValueList                = (" << mValueList.size() << " items)\n";

    for (auto it = mValueList.begin();  it != mValueList.end(); ++it)
    {
      it->print(stream,level+2,optionFlags);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}






}  // namespace QueryServer
}  // namespace SmartMet

