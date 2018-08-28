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
    mTimestepsBefore = 0;
    mTimestepsAfter = 0;
    mTimestepSizeInMinutes = 0;
    mTemporary = false;
    mFlags = 0;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
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
    mTimestepsBefore = queryParameter.mTimestepsBefore;
    mTimestepsAfter = queryParameter.mTimestepsAfter;
    mTimestepSizeInMinutes = queryParameter.mTimestepSizeInMinutes;
    mValueList = queryParameter.mValueList;
    mPrecision = queryParameter.mPrecision;
    mTemporary = queryParameter.mTemporary;
    mFunction = queryParameter.mFunction;
    mFunctionParams = queryParameter.mFunctionParams;
    mFlags = queryParameter.mFlags;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





QueryParameter::~QueryParameter()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
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
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
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
    stream << space(level) << "- mAreaInterpolationMethod  = " << mAreaInterpolationMethod << "\n";
    stream << space(level) << "- mTimeInterpolationMethod  = " << mTimeInterpolationMethod << "\n";
    stream << space(level) << "- mLevelInterpolationMethod = " << mLevelInterpolationMethod << "\n";
    stream << space(level) << "- mProducerId               = " << mProducerId << "\n";
    stream << space(level) << "- mGenerationFlags          = " << mGenerationFlags << "\n";
    stream << space(level) << "- mTimestepsBefore          = " << mTimestepsBefore << "\n";
    stream << space(level) << "- mTimestepsAfter           = " << mTimestepsAfter << "\n";
    stream << space(level) << "- mTimestepSizeInMinutes    = " << mTimestepSizeInMinutes << "\n";
    stream << space(level) << "- mTemporary                = " << (int)mTemporary << "\n";
    stream << space(level) << "- mPrecision                = " << mPrecision << "\n";
    stream << space(level) << "- mFunction                 = " << mFunction << "\n";
    stream << space(level) << "- mFlags                    = " << mFlags << "\n";

    stream << space(level) << "- mFunctionParams           = \n";
    for (auto it = mFunctionParams.begin();  it != mFunctionParams.end(); ++it)
    {
      stream << space(level) << "   * " << it->first << ":" << it->second << "\n";
    }

    stream << space(level) << "- mValueList                = (" << mValueList.size() << " items)\n";

    for (auto it = mValueList.begin();  it != mValueList.end(); ++it)
    {
      it->print(stream,level+2,optionFlags);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace QueryServer
}  // namespace SmartMet

