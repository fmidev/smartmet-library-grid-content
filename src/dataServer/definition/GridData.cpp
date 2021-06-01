#include "GridData.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{



GridData::GridData()
{
  try
  {
    mServerId = 0;
    mProducerId = 0;
    mGenerationId = 0;
    mFileId = 0;
    mFileType = T::FileTypeValue::Unknown;
    mMessageIndex = 0;
    mFmiParameterLevelId = 0;
    mGrib1ParameterLevelId = 0;
    mGrib2ParameterLevelId = 0;
    mParameterLevel = 0;
    mGeometryId = 0;
    mProjection = 0;
    mColumns = 0;
    mRows = 0;
    mForecastType = 0;
    mForecastNumber = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



/*

GridData::GridData(GridData& gridData)
{
  try
  {
    mServerId = gridData.mServerId;
    mGroupFlags = gridData.mGroupFlags;
    mProducerId = gridData.mProducerId;
    mGenerationId = gridData.mGenerationId;
    mFileId = gridData.mFileId;
    mFileType = gridData.mFileType;
    mFileName = gridData.mFileName;
    mMessageIndex = gridData.mMessageIndex;
    mForecastTime = gridData.mForecastTime;
    mGribParameterId = gridData.mGribParameterId;
    mGribParameterName = gridData.mGribParameterName;
    mGribParameterUnits = gridData.mGribParameterUnits;
    mGrib1ParameterLevelId = gridData.mGrib1ParameterLevelId;
    mGrib2ParameterLevelId = gridData.mGrib2ParameterLevelId;
    mFmiParameterId = gridData.mFmiParameterId;
    mFmiParameterLevelId = gridData.mFmiParameterLevelId;
    mFmiParameterName = gridData.mFmiParameterName;
    mFmiParameterUnits = gridData.mFmiParameterUnits;
    mCdmParameterId = gridData.mCdmParameterId;
    mCdmParameterName = gridData.mCdmParameterName;
    mNewbaseParameterId = gridData.mNewbaseParameterId;
    mNewbaseParameterName = gridData.mNewbaseParameterName;
    mParameterLevel = gridData.mParameterLevel;
    mGeometryId = gridData.mGeometryId;
    mProjection = gridData.mProjection;
    mColumns = gridData.mColumns;
    mRows = gridData.mRows;
    mForecastType = gridData.mForecastType;
    mForecastNumber = gridData.mForecastNumber;
    mProjectionAttributes = gridData.mProjectionAttributes;
    mValues = gridData.mValues;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}
*/




GridData::~GridData()
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





void GridData::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "GridData\n";
    stream << space(level) << "- mServerId                 = " << mServerId << "\n";
    stream << space(level) << "- mProducerId               = " << mProducerId << "\n";
    stream << space(level) << "- mGenerationId             = " << mGenerationId << "\n";
    stream << space(level) << "- mFileId                   = " << mFileId << "\n";
    stream << space(level) << "- mFileType                 = " << mFileType << "\n";
    stream << space(level) << "- mFileName                 = " << mFileName << "\n";
    stream << space(level) << "- mMessageIndex             = " << mMessageIndex << "\n";
    stream << space(level) << "- mForecastTime             = " << mForecastTime << "\n";
    stream << space(level) << "- mGribParameterId          = " << mGribParameterId << "\n";
    stream << space(level) << "- mGrib1ParameterLevelId    = " << mGrib1ParameterLevelId << "\n";
    stream << space(level) << "- mGrib2ParameterLevelId    = " << mGrib2ParameterLevelId << "\n";
    stream << space(level) << "- mGribParameterName        = " << mGribParameterName << "\n";
    stream << space(level) << "- mFmiParameterId           = " << mFmiParameterId << "\n";
    stream << space(level) << "- mFmiParameterLevelId      = " << mFmiParameterLevelId << "\n";
    stream << space(level) << "- mFmiParameterName         = " << mFmiParameterName << "\n";
    stream << space(level) << "- mNewbaseParameterId       = " << mNewbaseParameterId << "\n";
    stream << space(level) << "- mNewbaseParameterName     = " << mNewbaseParameterName << "\n";
    stream << space(level) << "- mParameterLevel           = " << mParameterLevel << "\n";
    stream << space(level) << "- mGeometryId               = " << mGeometryId << "\n";
    stream << space(level) << "- mProjection               = " << mProjection << "\n";
    stream << space(level) << "- mColumns                  = " << mColumns << "\n";
    stream << space(level) << "- mRows                     = " << mRows << "\n";
    stream << space(level) << "- mForecastType             = " << mForecastType << "\n";
    stream << space(level) << "- mForecastNumber           = " << mForecastNumber << "\n";
    stream << space(level) << "- mProjectionAttributes     = \n";
    mProjectionAttributes.print(stream,level+2,optionFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




}
}  // namespace SmartMet
