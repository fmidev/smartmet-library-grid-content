#include "ContentInfo.h"

#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
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
    mFileType = T::FileTypeValue::Unknown;
    mFileId = 0;
    mMessageIndex = 0;
    mFmiParameterLevelId = 0;
    mGrib1ParameterLevelId = 0;
    mGrib2ParameterLevelId = 0;
    mParameterLevel = 0;
    mForecastType = 0;
    mForecastNumber = 0;
    mFlags = 0;
    mSourceId = 0;
    mGeometryId = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    mForecastTime = contentInfo.mForecastTime;
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
    mForecastType = contentInfo.mForecastType;
    mForecastNumber = contentInfo.mForecastNumber;
    mFlags = contentInfo.mFlags;
    mSourceId = contentInfo.mSourceId;
    mGeometryId = contentInfo.mGeometryId;
    mModificationTime = contentInfo.mModificationTime;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




ContentInfo::~ContentInfo()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ContentInfo& ContentInfo::operator=(ContentInfo& contentInfo)
{
  try
  {
    if (&contentInfo == this)
      return *this;

    mServerFlags = contentInfo.mServerFlags;
    mGroupFlags = contentInfo.mGroupFlags;
    mProducerId = contentInfo.mProducerId;
    mGenerationId = contentInfo.mGenerationId;
    mFileType = contentInfo.mFileType;
    mFileId = contentInfo.mFileId;
    mMessageIndex = contentInfo.mMessageIndex;
    mForecastTime = contentInfo.mForecastTime;
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
    mForecastType = contentInfo.mForecastType;
    mForecastNumber = contentInfo.mForecastNumber;
    mFlags = contentInfo.mFlags;
    mSourceId = contentInfo.mSourceId;
    mGeometryId = contentInfo.mGeometryId;
    mModificationTime = contentInfo.mModificationTime;

    return *this;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





std::string ContentInfo::getCsv()
{
  try
  {
    char st[1000];
    sprintf(st,"%u;%u;%u;%u;%u;%u;%s;%s;%s;%s;%s;%s;%s;%s;%u;%u;%u;%d;%s;%s;%d;%d;%llu;%u;%u;%u;%s;",
        mFileId,
        mMessageIndex,
        mFileType,
        mProducerId,
        mGenerationId,
        mGroupFlags,
        mForecastTime.c_str(),
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
        mForecastType,
        mForecastNumber,
        mServerFlags,
        mFlags,
        mSourceId,
        mGeometryId,
        mModificationTime.c_str()
        );

    return std::string(st);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





std::string ContentInfo::getCsvHeader()
{
  try
  {
    std::string header = "fileId;messageIndex;fileType;producerId;generationId;groupFlags;startTime;fmiParameterId;fmiParameterName;gribParameterId;cdmParameterId;cdmParameterName;newbaseParameterId;newbaseParameterName;fmiParameterLevelId;grib1ParameterLevelId;grib2ParameterLevelId;parameterLevel;fmiParameterUnits;gribParameterUnits;mForecastType;mForecastNumber;serverFlags;flags;sourceId;geometryId;modificationTime";
    return header;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfo::setCsv(const char *csv)
{
  try
  {
    char empty = '\0';
    char *field[100] = {&empty};
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


    if (c >= 25)
    {
      mFileId = toInt64(field[0]);
      mMessageIndex = toInt64(field[1]);
      mFileType = toInt64(field[2]);
      mProducerId = toInt64(field[3]);
      mGenerationId = toInt64(field[4]);
      mGroupFlags = toInt64(field[5]);
      mForecastTime = field[6];
      mFmiParameterId = field[7];
      mFmiParameterName = field[8];
      mGribParameterId = field[9];
      mCdmParameterId = field[10];
      mCdmParameterName = field[11];
      mNewbaseParameterId = field[12];
      mNewbaseParameterName = field[13];
      mFmiParameterLevelId = toInt64(field[14]);
      mGrib1ParameterLevelId = toInt64(field[15]);
      mGrib2ParameterLevelId = toInt64(field[16]);
      mParameterLevel = (T::ParamLevel)toInt64(field[17]);
      mFmiParameterUnits = field[18];
      mGribParameterUnits = field[19];
      mForecastType = (short)toInt64(field[20]);
      mForecastNumber = (short)toInt64(field[21]);
      mServerFlags = (unsigned long long)toInt64(field[22]);
      mFlags = toInt64(field[23]);
      mSourceId = toInt64(field[24]);
      mGeometryId = toInt64(field[25]);
      if (c >= 26)
        mModificationTime = field[26];
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





bool ContentInfo::hasKey(T::ParamKeyType parameterKeyType,std::string parameterKey)
{
  try
  {
    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        if (strcasecmp(mFmiParameterId.c_str(),parameterKey.c_str()) == 0)
          return true;
        else
          return false;

      case T::ParamKeyTypeValue::FMI_NAME:
        if (strcasecmp(mFmiParameterName.c_str(),parameterKey.c_str()) == 0)
          return true;
        else
          return false;

      case T::ParamKeyTypeValue::GRIB_ID:
        if (strcasecmp(mGribParameterId.c_str(),parameterKey.c_str()) == 0)
          return true;
        else
          return false;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        if (strcasecmp(mNewbaseParameterId.c_str(),parameterKey.c_str()) == 0)
          return true;
        else
          return false;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        if (strcasecmp(mNewbaseParameterName.c_str(),parameterKey.c_str()) == 0)
          return true;
        else
          return false;

      case T::ParamKeyTypeValue::CDM_ID:
        if (strcasecmp(mCdmParameterId.c_str(),parameterKey.c_str()) == 0)
          return true;
        else
          return false;

      case T::ParamKeyTypeValue::CDM_NAME:
        if (strcasecmp(mCdmParameterName.c_str(),parameterKey.c_str()) == 0)
          return true;
        else
          return false;

      default:
        return false;
    }
    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ContentInfo::compare(uint comparisonMethod,ContentInfo *contentInfo)
{
  try
  {
    if (contentInfo == nullptr)
      return 0;

    int res = 0;

    switch (comparisonMethod)
    {
      case ContentInfo::ComparisonMethod::file_message:
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return uint_compare(mMessageIndex,contentInfo->mMessageIndex);

      case ContentInfo::ComparisonMethod::gribId_level_starttime_file_message:
        res = strcasecmp(mGribParameterId.c_str(),contentInfo->mGribParameterId.c_str());
        if (res != 0)
          return res;
        res = int_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcasecmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = int_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0  && contentInfo->mForecastType >= -1)
          return res;
        res = int_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0  &&  contentInfo->mForecastNumber >= -1)
          return res;
        res = int_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0  &&  contentInfo->mGeometryId >= -1)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0  &&  contentInfo->mFileId != 0xFFFFFFFF)
          return res;
        res = uint_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0  &&  contentInfo->mMessageIndex != 0xFFFFFFFF)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message:
        res = strcasecmp(mFmiParameterId.c_str(),contentInfo->mFmiParameterId.c_str());
        if (res != 0)
          return res;
        res = int_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcasecmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = int_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0  && contentInfo->mForecastType >= -1)
          return res;
        res = int_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0  &&  contentInfo->mForecastNumber >= -1)
          return res;
        res = int_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0  &&  contentInfo->mGeometryId >= -1)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return uint_compare(mMessageIndex,contentInfo->mMessageIndex);

      case ContentInfo::ComparisonMethod::fmiName_level_starttime_file_message:
        res = strcasecmp(mFmiParameterName.c_str(),contentInfo->mFmiParameterName.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = int_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcasecmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = int_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0  && contentInfo->mForecastType >= -1)
        //if (res != 0)
          return res;
        res = int_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0  &&  contentInfo->mForecastNumber >= -1)
        //if (res != 0)
          return res;
        res = int_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0  &&  contentInfo->mGeometryId >= -1)
        //if (res != 0)
          return res;
        res = uint_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0  &&  contentInfo->mFileId != 0xFFFFFFFF)
        //if (res != 0)
          return res;
        res = uint_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0  &&  contentInfo->mMessageIndex != 0xFFFFFFFF)
        //if (res != 0)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::newbaseId_level_starttime_file_message:
        res = strcasecmp(mNewbaseParameterId.c_str(),contentInfo->mNewbaseParameterId.c_str());
        if (res != 0)
          return res;
        res = int_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcasecmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = int_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0  && contentInfo->mForecastType >= -1)
          return res;
        res = int_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0  &&  contentInfo->mForecastNumber >= -1)
          return res;
        res = int_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0  &&  contentInfo->mGeometryId >= -1)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0  &&  contentInfo->mFileId != 0xFFFFFFFF)
          return res;
        res = uint_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0  &&  contentInfo->mMessageIndex != 0xFFFFFFFF)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message:
        res = strcasecmp(mNewbaseParameterName.c_str(),contentInfo->mNewbaseParameterName.c_str());
        if (res != 0)
          return res;
        res = int_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcasecmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = int_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0  && contentInfo->mForecastType >= -1)
          return res;
        res = int_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0  &&  contentInfo->mForecastNumber >= -1)
          return res;
        res = int_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0  &&  contentInfo->mGeometryId >= -1)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0  &&  contentInfo->mFileId != 0xFFFFFFFF)
          return res;
        res = uint_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0  &&  contentInfo->mMessageIndex != 0xFFFFFFFF)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::cdmId_level_starttime_file_message:
        res = strcasecmp(mCdmParameterId.c_str(),contentInfo->mCdmParameterId.c_str());
        if (res != 0)
          return res;
        res = int_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcasecmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = int_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0  && contentInfo->mForecastType >= -1)
          return res;
        res = int_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0  &&  contentInfo->mForecastNumber >= -1)
          return res;
        res = int_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0  &&  contentInfo->mGeometryId >= -1)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0  &&  contentInfo->mFileId != 0xFFFFFFFF)
          return res;
        res = uint_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0  &&  contentInfo->mMessageIndex != 0xFFFFFFFF)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::cdmName_level_starttime_file_message:
        res = strcasecmp(mCdmParameterName.c_str(),contentInfo->mCdmParameterName.c_str());
        if (res != 0)
          return res;
        res = int_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcasecmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = int_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0  && contentInfo->mForecastType >= -1)
          return res;
        res = int_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0  &&  contentInfo->mForecastNumber >= -1)
          return res;
        res = int_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0  &&  contentInfo->mGeometryId >= -1)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0  &&  contentInfo->mFileId != 0xFFFFFFFF)
          return res;
        res = uint_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0  &&  contentInfo->mMessageIndex != 0xFFFFFFFF)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::starttime_file_message:
        res = strcasecmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
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
        res = strcasecmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = int_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0  &&  contentInfo->mFileId != 0xFFFFFFFF)
          return res;
        res = uint_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0  &&  contentInfo->mMessageIndex != 0xFFFFFFFF)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::fmiId_fmiLevelId_level_starttime_file_message:
        res = strcasecmp(mFmiParameterId.c_str(),contentInfo->mFmiParameterId.c_str());
        if (res != 0)
          return res;
        res = int_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = int_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = int_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0  && contentInfo->mForecastType >= -1)
          return res;
        res = int_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0  &&  contentInfo->mForecastNumber >= -1)
          return res;
        res = strcasecmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = int_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0  &&  contentInfo->mGeometryId >= -1)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0  &&  contentInfo->mFileId != 0xFFFFFFFF)
          return res;
        res = uint_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0  &&  contentInfo->mMessageIndex != 0xFFFFFFFF)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::starttime_fmiId_fmiLevelId_level_file_message:
        res = strcasecmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = strcasecmp(mFmiParameterId.c_str(),contentInfo->mFmiParameterId.c_str());
        if (res != 0)
          return res;
        res = int_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = int_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = int_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0  && contentInfo->mForecastType >= -1)
          return res;
        res = int_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0  &&  contentInfo->mForecastNumber >= -1)
          return res;
        res = int_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0  &&  contentInfo->mGeometryId >= -1)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0  &&  contentInfo->mFileId != 0xFFFFFFFF)
          return res;
        res = uint_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0  &&  contentInfo->mMessageIndex != 0xFFFFFFFF)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::starttime_fmiName_fmiLevelId_level_file_message:
        res = strcasecmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = strcasecmp(mFmiParameterName.c_str(),contentInfo->mFmiParameterName.c_str());
        if (res != 0)
          return res;
        res = int_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = int_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = int_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0  && contentInfo->mForecastType >= -1)
          return res;
        res = int_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0  &&  contentInfo->mForecastNumber >= -1)
          return res;
        res = int_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0  &&  contentInfo->mGeometryId >= -1)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0  &&  contentInfo->mFileId != 0xFFFFFFFF)
          return res;
        res = uint_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0  &&  contentInfo->mMessageIndex != 0xFFFFFFFF)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::generationId_starttime_file_message:
        res = uint_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = strcasecmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0  &&  contentInfo->mFileId != 0xFFFFFFFF)
          return res;
        res = uint_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0  &&  contentInfo->mMessageIndex != 0xFFFFFFFF)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::fmiName_fmiLevelId_level_starttime_file_message:
        res = strcasecmp(mFmiParameterName.c_str(),contentInfo->mFmiParameterName.c_str());
        if (res != 0)
          return res;
        res = int_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = int_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = int_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0  && contentInfo->mForecastType >= -1)
          return res;
        res = int_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0  &&  contentInfo->mForecastNumber >= -1)
          return res;
        res = int_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0  &&  contentInfo->mGeometryId >= -1)
          return res;
        res = strcasecmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0  &&  contentInfo->mFileId != 0xFFFFFFFF)
          return res;
        res = uint_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0  &&  contentInfo->mMessageIndex != 0xFFFFFFFF)
          return res;
        return 0;

      default:
        res = uint_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return uint_compare(mMessageIndex,contentInfo->mMessageIndex);
    }
    return 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ContentInfo\n";
    stream << space(level) << "- mFileId                 = " << mFileId << "\n";
    stream << space(level) << "- mFileType               = " << mFileType << "\n";
    stream << space(level) << "- mMessageIndex           = " << mMessageIndex << "\n";
    stream << space(level) << "- mProducerId             = " << mProducerId << "\n";
    stream << space(level) << "- mGenerationId           = " << mGenerationId << "\n";
    stream << space(level) << "- mGroupFlags             = " << mGroupFlags << "\n";
    stream << space(level) << "- mForecastTime           = " << mForecastTime << "\n";
    stream << space(level) << "- mFmiParameterId         = " << mFmiParameterId << "\n";
    stream << space(level) << "- mFmiParameterName       = " << mFmiParameterName << "\n";
    stream << space(level) << "- mGribParameterId        = " << mGribParameterId << "\n";
    stream << space(level) << "- mCdmParameterId         = " << mCdmParameterId << "\n";
    stream << space(level) << "- mCdmParameterName       = " << mCdmParameterName << "\n";
    stream << space(level) << "- mNewbaseParameterId     = " << mNewbaseParameterId << "\n";
    stream << space(level) << "- mNewbaseParameterName   = " << mNewbaseParameterName << "\n";
    stream << space(level) << "- mFmiParameterLevelId    = " << mFmiParameterLevelId << "\n";
    stream << space(level) << "- mGrib1ParameterLevelId  = " << mGrib1ParameterLevelId << "\n";
    stream << space(level) << "- mGrib2ParameterLevelId  = " << mGrib2ParameterLevelId << "\n";
    stream << space(level) << "- mParameterLevel         = " << mParameterLevel << "\n";
    stream << space(level) << "- mFmiParameterUnits      = " << mFmiParameterUnits << "\n";
    stream << space(level) << "- mGribParameterUnits     = " << mGribParameterUnits << "\n";
    stream << space(level) << "- mForecastType           = " << mForecastType << "\n";
    stream << space(level) << "- mForecastNumber         = " << mForecastNumber << "\n";
    stream << space(level) << "- mServerFlags            = " << mServerFlags << "\n";
    stream << space(level) << "- mFlags                  = " << mFlags << "\n";
    stream << space(level) << "- mSourceId               = " << mSourceId << "\n";
    stream << space(level) << "- mGeometryId             = " << mGeometryId << "\n";
    stream << space(level) << "- mModificationTime       = " << mModificationTime << "\n";
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}


}
}
