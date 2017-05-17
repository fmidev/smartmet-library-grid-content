#include "GridData.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"


namespace SmartMet
{
namespace T
{



GridData::GridData()
{
  try
  {
    mServerId = 0;
    mGroupFlags = 0;
    mProducerId = 0;
    mGenerationId = 0;
    mFileId = 0;
    mFileType = T::FileType::Unknown;
    mMessageIndex = 0;
    mFmiParameterLevelId = 0;
    mGrib1ParameterLevelId = 0;
    mGrib2ParameterLevelId = 0;
    mParameterLevel = 0;
    mProjection = 0;
    mColumns = 0;
    mRows = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





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
    mStartTime = gridData.mStartTime;
    mEndTime = gridData.mEndTime;
    mGribParameterId = gridData.mGribParameterId;
    mGribParameterName = gridData.mGribParameterName;
    mGribParameterDescription = gridData.mGribParameterDescription;
    mGribParameterUnits = gridData.mGribParameterUnits;
    mGrib1ParameterLevelId = gridData.mGrib1ParameterLevelId;
    mGrib2ParameterLevelId = gridData.mGrib2ParameterLevelId;
    mFmiParameterId = gridData.mFmiParameterId;
    mFmiParameterLevelId = gridData.mFmiParameterLevelId;
    mFmiParameterName = gridData.mFmiParameterName;
    mFmiParameterDescription = gridData.mFmiParameterDescription;
    mFmiParameterUnits = gridData.mFmiParameterUnits;
    mCdmParameterId = gridData.mCdmParameterId;
    mCdmParameterName = gridData.mCdmParameterName;
    mNewbaseParameterId = gridData.mNewbaseParameterId;
    mNewbaseParameterName = gridData.mNewbaseParameterName;
    mParameterLevel = gridData.mParameterLevel;
    mProjection = gridData.mProjection;
    mColumns = gridData.mColumns;
    mRows = gridData.mRows;
    mProjectionAttributes = gridData.mProjectionAttributes;
    mValues = gridData.mValues;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





GridData::~GridData()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void GridData::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "GridData\n";
    stream << space(level) << "- mServerId                 = " << mServerId << "\n";
    stream << space(level) << "- mGroupFlags               = " << mGroupFlags << "\n";
    stream << space(level) << "- mProducerId               = " << mProducerId << "\n";
    stream << space(level) << "- mGenerationId             = " << mGenerationId << "\n";
    stream << space(level) << "- mFileId                   = " << mFileId << "\n";
    stream << space(level) << "- mFileType                 = " << (uint)mFileType << "\n";
    stream << space(level) << "- mFileName                 = " << mFileName << "\n";
    stream << space(level) << "- mMessageIndex             = " << mMessageIndex << "\n";
    stream << space(level) << "- mStartTime                = " << mStartTime << "\n";
    stream << space(level) << "- mEndTime                  = " << mEndTime << "\n";
    stream << space(level) << "- mGribParameterId          = " << mGribParameterId << "\n";
    stream << space(level) << "- mGrib1ParameterLevelId    = " << (uint)mGrib1ParameterLevelId << "\n";
    stream << space(level) << "- mGrib2ParameterLevelId    = " << (uint)mGrib2ParameterLevelId << "\n";
    stream << space(level) << "- mGribParameterName        = " << mGribParameterName << "\n";
    stream << space(level) << "- mGribParameterDescription = " << mGribParameterDescription << "\n";
    stream << space(level) << "- mGribParameterUnits       = " << mGribParameterUnits << "\n";
    stream << space(level) << "- mFmiParameterId           = " << mFmiParameterId << "\n";
    stream << space(level) << "- mFmiParameterLevelId      = " << (uint)mFmiParameterLevelId << "\n";
    stream << space(level) << "- mFmiParameterName         = " << mFmiParameterName << "\n";
    stream << space(level) << "- mFmiParameterDescription  = " << mFmiParameterDescription << "\n";
    stream << space(level) << "- mFmiParameterUnits        = " << mFmiParameterUnits << "\n";
    stream << space(level) << "- mCdmParameterId           = " << mCdmParameterId << "\n";
    stream << space(level) << "- mCdmParameterName         = " << mCdmParameterName << "\n";
    stream << space(level) << "- mNewbaseParameterId       = " << mNewbaseParameterId << "\n";
    stream << space(level) << "- mNewbaseParameterName     = " << mNewbaseParameterName << "\n";
    stream << space(level) << "- mParameterLevel           = " << mParameterLevel << "\n";
    stream << space(level) << "- mProjection               = " << mProjection << "\n";
    stream << space(level) << "- mColumns                  = " << mColumns << "\n";
    stream << space(level) << "- mRows                     = " << mRows << "\n";
    stream << space(level) << "- mProjectionAttributes     = \n";
    mProjectionAttributes.print(stream,level+2,optionFlags);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




}
}  // namespace SmartMet
