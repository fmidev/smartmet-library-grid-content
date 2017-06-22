#include "ContentInfo.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"

#include <string.h>


namespace SmartMet
{
namespace T
{


ContentInfo::ContentInfo()
{
  try
  {
    mServerFlags = 0;
    mGroupFlags = 0;
    mProducerId = 0;
    mGenerationId = 0;
    mFileType = T::FileType::Unknown;
    mFileId = 0;
    mMessageIndex = 0;
    mFmiParameterLevelId = 0;
    mGrib1ParameterLevelId = 0;
    mGrib2ParameterLevelId = 0;
    mParameterLevel = 0;
    mFlags = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ContentInfo::ContentInfo(ContentInfo& contentInfo)
{
  try
  {
    mServerFlags = contentInfo.mServerFlags;
    mGroupFlags = contentInfo.mGroupFlags;
    mProducerId = contentInfo.mProducerId;
    mGenerationId = contentInfo.mGenerationId;
    mFileType = contentInfo.mFileType;
    mFileId = contentInfo.mFileId;
    mMessageIndex = contentInfo.mMessageIndex;
    mStartTime = contentInfo.mStartTime;
    mEndTime = contentInfo.mEndTime;
    mFmiParameterId = contentInfo.mFmiParameterId;
    mFmiParameterName = contentInfo.mFmiParameterName;
    mGribParameterId = contentInfo.mGribParameterId;
    mCdmParameterId = contentInfo.mCdmParameterId;
    mCdmParameterName = contentInfo.mCdmParameterName;
    mNewbaseParameterId = contentInfo.mNewbaseParameterId;
    mNewbaseParameterName = contentInfo.mNewbaseParameterName;
    mFmiParameterLevelId = contentInfo.mFmiParameterLevelId;
    mGrib1ParameterLevelId = contentInfo.mGrib1ParameterLevelId;
    mGrib2ParameterLevelId = contentInfo.mGrib2ParameterLevelId;
    mParameterLevel = contentInfo.mParameterLevel;
    mFmiParameterUnits = contentInfo.mFmiParameterUnits;
    mGribParameterUnits = contentInfo.mGribParameterUnits;
    mFlags = contentInfo.mFlags;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ContentInfo::ContentInfo(const char *csv)
{
  try
  {
    setCsv(csv);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




ContentInfo::~ContentInfo()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfo::operator=(ContentInfo& contentInfo)
{
  try
  {
    if (&contentInfo == this)
      return;

    mServerFlags = contentInfo.mServerFlags;
    mGroupFlags = contentInfo.mGroupFlags;
    mProducerId = contentInfo.mProducerId;
    mGenerationId = contentInfo.mGenerationId;
    mFileType = contentInfo.mFileType;
    mFileId = contentInfo.mFileId;
    mMessageIndex = contentInfo.mMessageIndex;
    mStartTime = contentInfo.mStartTime;
    mEndTime = contentInfo.mEndTime;
    mFmiParameterId = contentInfo.mFmiParameterId;
    mFmiParameterName = contentInfo.mFmiParameterName;
    mGribParameterId = contentInfo.mGribParameterId;
    mCdmParameterId = contentInfo.mCdmParameterId;
    mCdmParameterName = contentInfo.mCdmParameterName;
    mNewbaseParameterId = contentInfo.mNewbaseParameterId;
    mNewbaseParameterName = contentInfo.mNewbaseParameterName;
    mFmiParameterLevelId = contentInfo.mFmiParameterLevelId;
    mGrib1ParameterLevelId = contentInfo.mGrib1ParameterLevelId;
    mGrib2ParameterLevelId = contentInfo.mGrib2ParameterLevelId;
    mParameterLevel = contentInfo.mParameterLevel;
    mFmiParameterUnits = contentInfo.mFmiParameterUnits;
    mGribParameterUnits = contentInfo.mGribParameterUnits;
    mFlags = contentInfo.mFlags;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





std::string ContentInfo::getCsv()
{
  try
  {
    char st[1000];
    sprintf(st,"%u;%u;%u;%u;%u;%u;%s;%s;%s;%s;%s;%s;%s;%s;%s;%u;%u;%u;%u;%s;%s;%llu;%u",
        mFileId,
        mMessageIndex,
        (uint)mFileType,
        mProducerId,
        mGenerationId,
        mGroupFlags,
        mStartTime.c_str(),
        mEndTime.c_str(),
        mFmiParameterId.c_str(),
        mFmiParameterName.c_str(),
        mGribParameterId.c_str(),
        mCdmParameterId.c_str(),
        mCdmParameterName.c_str(),
        mNewbaseParameterId.c_str(),
        mNewbaseParameterName.c_str(),
        mFmiParameterLevelId,
        mGrib1ParameterLevelId,
        mGrib2ParameterLevelId,
        mParameterLevel,
        mFmiParameterUnits.c_str(),
        mGribParameterUnits.c_str(),
        mServerFlags,
        mFlags);

    return std::string(st);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





std::string ContentInfo::getCsvHeader()
{
  try
  {
    std::string header = "fileId;messageIndex;fileType;producerId;generationId;groupFlags;startTime;endTime;fmiParameterId;fmiParameterName;gribParameterId;cdmParameterId;cdmParameterName;newbaseParameterId;newbaseParameterName;fmiParameterLevelId;grib1ParameterLevelId;grib2ParameterLevelId;parameterLevel;fmiParameterUnits;gribParameterUnits;serverFlags;flags";
    return header;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfo::setCsv(const char *csv)
{
  try
  {
    char *field[100];
    char st[1000];
    strcpy(st,csv);

    uint c = 0;
    field[0] = st;
    char *p = st;
    while (*p != '\0'  &&  c < 100)
    {
      if (*p == ';'  ||  *p == '\n')
      {
        *p = '\0';
        p++;
        c++;
        field[c] = p;
      }
      else
      {
        p++;
      }
    }

    if (c >= 22)
    {
      mFileId = (uint)atoll(field[0]);
      mMessageIndex = (uint)atoll(field[1]);
      mFileType = (T::FileType)atoll(field[2]);
      mProducerId = (uint)atoll(field[3]);
      mGenerationId = (uint)atoll(field[4]);
      mGroupFlags = (uint)atoll(field[5]);
      mStartTime = field[6];
      mEndTime = field[7];
      mFmiParameterId = field[8];
      mFmiParameterName = field[9];
      mGribParameterId = field[10];
      mCdmParameterId = field[11];
      mCdmParameterName = field[12];
      mNewbaseParameterId = field[13];
      mNewbaseParameterName = field[14];
      mFmiParameterLevelId = (T::ParamLevelId)atoll(field[15]);
      mGrib1ParameterLevelId = (T::ParamLevelId)atoll(field[16]);
      mGrib2ParameterLevelId = (T::ParamLevelId)atoll(field[17]);
      mParameterLevel = (uint)atoll(field[18]);
      mFmiParameterUnits = field[19];
      mGribParameterUnits = field[20];
      mServerFlags = (unsigned long long)atoll(field[21]);
      mFlags = (uint)atoll(field[22]);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfo::setCsv(std::string csv)
{
  try
  {
    setCsv(csv.c_str());
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





bool ContentInfo::hasKey(T::ParamKeyType parameterKeyType,std::string parameterKey)
{
  try
  {
    switch (parameterKeyType)
    {
      case T::ParamKeyType::FMI_ID:
        if (strcasecmp(mFmiParameterId.c_str(),parameterKey.c_str()) == 0)
          return true;
        else
          return false;

      case T::ParamKeyType::FMI_NAME:
        if (strcasecmp(mFmiParameterName.c_str(),parameterKey.c_str()) == 0)
          return true;
        else
          return false;

      case T::ParamKeyType::GRIB_ID:
        if (strcasecmp(mGribParameterId.c_str(),parameterKey.c_str()) == 0)
          return true;
        else
          return false;

      case T::ParamKeyType::NEWBASE_ID:
        if (strcasecmp(mNewbaseParameterId.c_str(),parameterKey.c_str()) == 0)
          return true;
        else
          return false;

      case T::ParamKeyType::NEWBASE_NAME:
        if (strcasecmp(mNewbaseParameterName.c_str(),parameterKey.c_str()) == 0)
          return true;
        else
          return false;

      case T::ParamKeyType::CDM_ID:
        if (strcasecmp(mCdmParameterId.c_str(),parameterKey.c_str()) == 0)
          return true;
        else
          return false;

      case T::ParamKeyType::CDM_NAME:
        if (strcasecmp(mCdmParameterName.c_str(),parameterKey.c_str()) == 0)
          return true;
        else
          return false;
    }
    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ContentInfo::compare(ComparisonMethod comparisonMethod,ContentInfo *contentInfo)
{
  try
  {
    if (contentInfo == NULL)
      return 0;

    int res = 0;

    switch (comparisonMethod)
    {
      case ContentInfo::ComparisonMethod::file_message:
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        res = uint_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::gribId_level_starttime_file_message:
        res = strcasecmp(mGribParameterId.c_str(),contentInfo->mGribParameterId.c_str());
        if (res != 0)
          return res;
        res = uint64_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcasecmp(mStartTime.c_str(),contentInfo->mStartTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return uint_compare(mMessageIndex,contentInfo->mMessageIndex);

      case ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message:
        res = strcasecmp(mFmiParameterId.c_str(),contentInfo->mFmiParameterId.c_str());
        if (res != 0)
          return res;
        res = uint64_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcasecmp(mStartTime.c_str(),contentInfo->mStartTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return uint_compare(mMessageIndex,contentInfo->mMessageIndex);

      case ContentInfo::ComparisonMethod::fmiName_level_starttime_file_message:
        res = strcasecmp(mFmiParameterName.c_str(),contentInfo->mFmiParameterName.c_str());
        if (res != 0)
          return res;
        res = uint64_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcasecmp(mStartTime.c_str(),contentInfo->mStartTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return uint_compare(mMessageIndex,contentInfo->mMessageIndex);

      case ContentInfo::ComparisonMethod::newbaseId_level_starttime_file_message:
        res = strcasecmp(mNewbaseParameterId.c_str(),contentInfo->mNewbaseParameterId.c_str());
        if (res != 0)
          return res;
        res = uint64_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcasecmp(mStartTime.c_str(),contentInfo->mStartTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return uint_compare(mMessageIndex,contentInfo->mMessageIndex);

      case ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message:
        res = strcasecmp(mNewbaseParameterName.c_str(),contentInfo->mNewbaseParameterName.c_str());
        if (res != 0)
          return res;
        res = uint64_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcasecmp(mStartTime.c_str(),contentInfo->mStartTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return uint_compare(mMessageIndex,contentInfo->mMessageIndex);

      case ContentInfo::ComparisonMethod::cdmId_level_starttime_file_message:
        res = strcasecmp(mCdmParameterId.c_str(),contentInfo->mCdmParameterId.c_str());
        if (res != 0)
          return res;
        res = uint64_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcasecmp(mStartTime.c_str(),contentInfo->mStartTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return uint_compare(mMessageIndex,contentInfo->mMessageIndex);

      case ContentInfo::ComparisonMethod::cdmName_level_starttime_file_message:
        res = strcasecmp(mCdmParameterName.c_str(),contentInfo->mCdmParameterName.c_str());
        if (res != 0)
          return res;
        res = uint64_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcasecmp(mStartTime.c_str(),contentInfo->mStartTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return uint_compare(mMessageIndex,contentInfo->mMessageIndex);

      case ContentInfo::ComparisonMethod::starttime_file_message:
        res = strcasecmp(mStartTime.c_str(),contentInfo->mStartTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return uint_compare(mMessageIndex,contentInfo->mMessageIndex);

      case ContentInfo::ComparisonMethod::fmiName_starttime_level_file_message:
        res = strcasecmp(mFmiParameterName.c_str(),contentInfo->mFmiParameterName.c_str());
        if (res != 0)
          return res;
        res = strcasecmp(mStartTime.c_str(),contentInfo->mStartTime.c_str());
        if (res != 0)
          return res;
        res = uint64_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return uint_compare(mMessageIndex,contentInfo->mMessageIndex);

      case ContentInfo::ComparisonMethod::fmiId_fmiLevelId_level_starttime_file_message:
        res = strcasecmp(mFmiParameterId.c_str(),contentInfo->mFmiParameterId.c_str());
        if (res != 0)
          return res;
        res = uint64_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = uint64_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcasecmp(mStartTime.c_str(),contentInfo->mStartTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return uint_compare(mMessageIndex,contentInfo->mMessageIndex);

      case ContentInfo::ComparisonMethod::starttime_fmiId_fmiLevelId_level_file_message:
        res = strcasecmp(mStartTime.c_str(),contentInfo->mStartTime.c_str());
        if (res != 0)
          return res;
        res = strcasecmp(mFmiParameterId.c_str(),contentInfo->mFmiParameterId.c_str());
        if (res != 0)
          return res;
        res = uint64_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = uint64_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return uint_compare(mMessageIndex,contentInfo->mMessageIndex);

      default:
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        res = uint_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0)
          return res;
        return 0;
    }
    return 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ContentInfo* ContentInfo::duplicate()
{
  try
  {
    return new ContentInfo(*this);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ContentInfo\n";
    stream << space(level) << "- mFileId                = " << mFileId << "\n";
    stream << space(level) << "- mFileType              = " << (uint)mFileType << "\n";
    stream << space(level) << "- mMessageIndex          = " << mMessageIndex << "\n";
    stream << space(level) << "- mProducerId            = " << mProducerId << "\n";
    stream << space(level) << "- mGenerationId          = " << mGenerationId << "\n";
    stream << space(level) << "- mGroupFlags            = " << mGroupFlags << "\n";
    stream << space(level) << "- mStartTime             = " << mStartTime << "\n";
    stream << space(level) << "- mEndTime               = " << mEndTime << "\n";
    stream << space(level) << "- mFmiParameterId        = " << mFmiParameterId << "\n";
    stream << space(level) << "- mFmiParameterName      = " << mFmiParameterName << "\n";
    stream << space(level) << "- mGribParameterId       = " << mGribParameterId << "\n";
    stream << space(level) << "- mCdmParameterId        = " << mCdmParameterId << "\n";
    stream << space(level) << "- mCdmParameterName      = " << mCdmParameterName << "\n";
    stream << space(level) << "- mNewbaseParameterId    = " << mNewbaseParameterId << "\n";
    stream << space(level) << "- mNewbaseParameterName  = " << mNewbaseParameterName << "\n";
    stream << space(level) << "- mFmiParameterLevelId   = " << (uint)mFmiParameterLevelId << "\n";
    stream << space(level) << "- mGrib1ParameterLevelId = " << (uint)mGrib1ParameterLevelId << "\n";
    stream << space(level) << "- mGrib2ParameterLevelId = " << (uint)mGrib2ParameterLevelId << "\n";
    stream << space(level) << "- mParameterLevel        = " << mParameterLevel << "\n";
    stream << space(level) << "- mFmiParameterUnits     = " << mFmiParameterUnits << "\n";
    stream << space(level) << "- mGribParameterUnits    = " << mGribParameterUnits << "\n";
    stream << space(level) << "- mServerFlags           = " << mServerFlags << "\n";
    stream << space(level) << "- mFlags                 = " << mFlags << "\n";
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}


}
}
