#include "ParameterValues.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace QueryServer
{


ParameterValues::ParameterValues()
{
  try
  {
    mProducerId = 0;
    mGenerationId = 0;
    mGenerationFlags = 0;
    mGeometryId = 0;
    mParameterKeyType = T::ParamKeyTypeValue::UNKNOWN;
    mParameterLevelId = 0;
    mParameterLevel = 0;
    mForecastType = -1;
    mForecastNumber = -1;
    mFlags = 0;
    mForecastTimeUTC = 0;
    mModificationTime = 0;

    for (uint t=0;t<4; t++)
    {
      mFileId[t] = 0;
      mMessageIndex[t] = 0;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}



/*
ParameterValues::ParameterValues(const ParameterValues& parameterValues)
{
  try
  {
    mForecastTime = parameterValues.mForecastTime;
    mProducerId = parameterValues.mProducerId;
    mGenerationId = parameterValues.mGenerationId;
    mGenerationFlags = parameterValues.mGenerationFlags;
    mGeometryId = parameterValues.mGeometryId;
    mModificationTime = parameterValues.mModificationTime;
    mAnalysisTime = parameterValues.mAnalysisTime;

    mParameterKeyType = parameterValues.mParameterKeyType;
    mParameterKey = parameterValues.mParameterKey;
    mParameterLevelId = parameterValues.mParameterLevelId;
    mParameterLevel = parameterValues.mParameterLevel;
    mForecastType = parameterValues.mForecastType;
    mForecastNumber = parameterValues.mForecastNumber;
    mFlags = parameterValues.mFlags;

    mValueList = parameterValues.mValueList;
    mValueVector = parameterValues.mValueVector;
    mValueData = parameterValues.mValueData;

    for (uint t=0;t<4; t++)
    {
      mFileId[t] = parameterValues.mFileId[t];
      mMessageIndex[t] = parameterValues.mMessageIndex[t];
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}
*/



ParameterValues::~ParameterValues()
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





void ParameterValues::clear()
{
  try
  {
    mProducerId = 0;
    mGenerationId = 0;
    mGenerationFlags = 0;
    mGeometryId = 0;
    mParameterKeyType = T::ParamKeyTypeValue::UNKNOWN;
    mParameterLevelId = 0;
    mParameterLevel = 0;
    mForecastType = -1;
    mForecastNumber = -1;
    mFlags = 0;
    mForecastTimeUTC = 0;
    mModificationTime = 0;

    for (uint t=0;t<4; t++)
    {
      mFileId[t] = 0;
      mMessageIndex[t] = 0;
    }

    mValueList.clear();
    mValueVector.clear();
    mValueData.clear();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




void ParameterValues::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ParameterValues\n";
    stream << space(level) << "- mForecastTime         = " << utcTimeFromTimeT(mForecastTimeUTC) << "\n";
    stream << space(level) << "- mForecastTimeUTC      = " << mForecastTimeUTC << "\n";
    stream << space(level) << "- mProducerId           = " << mProducerId << "\n";
    stream << space(level) << "- mGenerationId         = " << mGenerationId << "\n";
    stream << space(level) << "- mGenerationFlags      = " << mGenerationFlags << "\n";
    stream << space(level) << "- mGeometryId           = " << mGeometryId << "\n";

    for (uint t=0;t<4; t++)
    {
      if (mFileId[t] > 0)
      {
        stream << space(level) << "- mFileId               = " << mFileId[t] << "\n";
        stream << space(level) << "- mMessageIndex         = " << mMessageIndex[t] << "\n";
      }
    }

    stream << space(level) << "- mModificationTime     = " << utcTimeFromTimeT(mModificationTime) << "\n";
    stream << space(level) << "- mAnalysisTime         = " << mAnalysisTime << "\n";
    stream << space(level) << "- mParameterKeyType     = " << C_INT(mParameterKeyType) << "\n";
    stream << space(level) << "- mParameterKey         = " << mParameterKey << "\n";
    stream << space(level) << "- mParameterLevelId     = " << C_INT(mParameterLevelId) << "\n";
    stream << space(level) << "- mParameterLevel       = " << mParameterLevel << "\n";
    stream << space(level) << "- mForecastType         = " << mForecastType << "\n";
    stream << space(level) << "- mForecastNumber       = " << mForecastNumber << "\n";
    stream << space(level) << "- mFlags                = " << mFlags << "\n";
    stream << space(level) << "- mValueData            = (" << mValueData.size() << " arrays)\n";
    stream << space(level) << "- mValueVector          = (" << mValueVector.size() << " values)\n";
    stream << space(level) << "- mValueList            = (" << mValueList.getLength() << " records)\n";
    mValueList.print(stream,level+2,optionFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}



}  // namespace QueryServer
}  // namespace SmartMet

