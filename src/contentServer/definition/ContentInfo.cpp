#include "ContentInfo.h"

#include <macgyver/Exception.h>
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
    mFilePosition = 0;
    mMessageSize = 0;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ContentInfo::ContentInfo(const ContentInfo& contentInfo)
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
    mFilePosition = contentInfo.mFilePosition;
    mMessageSize = contentInfo.mMessageSize;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




ContentInfo::~ContentInfo()
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





ContentInfo& ContentInfo::operator=(const ContentInfo& contentInfo)
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
    mFilePosition = contentInfo.mFilePosition;
    mMessageSize = contentInfo.mMessageSize;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





const std::string& ContentInfo::getFmiParameterName()
{
  try
  {
    return mFmiParameterName;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfo::setFmiParameterName(const char *name)
{
  try
  {
    mFmiParameterName = toUpperString(name);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfo::setFmiParameterName(const std::string& name)
{
  try
  {
    mFmiParameterName = toUpperString(name);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::string ContentInfo::getCsv()
{
  try
  {
    char st[1000];
    sprintf(st,"%u;%u;%u;%llu;%u;%u;%u;%u;%s;%s;%s;%s;%s;%s;%s;%s;%u;%u;%u;%d;%s;%s;%d;%d;%llu;%u;%u;%u;%s;",
        mFileId,
        mMessageIndex,
        mFileType,
        mFilePosition,
        mMessageSize,
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ulonglong ContentInfo::getRequestCounterKey()
{
  try
  {
    ulonglong geometryId = C_UINT64(mGeometryId);
    ulonglong level = C_UINT64(mParameterLevel);
    ulonglong levelId = C_UINT64(mFmiParameterLevelId);
    ulonglong paramId = toUInt64(mFmiParameterId);
    ulonglong key = (geometryId << 48) + (levelId << 40) + (level << 20) + paramId;

    ulonglong producerId = C_UINT64(mProducerId);
    if (producerId > 0)
      key = key * producerId;

    return key;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::string ContentInfo::getCsvHeader()
{
  try
  {
    std::string header = "fileId;messageIndex;fileType;filePosition;messageSize;producerId;generationId;groupFlags;startTime;fmiParameterId;fmiParameterName;gribParameterId;cdmParameterId;cdmParameterName;newbaseParameterId;newbaseParameterName;fmiParameterLevelId;grib1ParameterLevelId;grib2ParameterLevelId;parameterLevel;fmiParameterUnits;gribParameterUnits;mForecastType;mForecastNumber;serverFlags;flags;sourceId;geometryId;modificationTime";
    return header;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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


    if (c >= 27)
    {
      mFileId = toInt64(field[0]);
      mMessageIndex = toInt64(field[1]);
      mFileType = toInt64(field[2]);
      mFilePosition = toInt64(field[3]);
      mMessageSize = toInt64(field[4]);
      mProducerId = toInt64(field[5]);
      mGenerationId = toInt64(field[6]);
      mGroupFlags = toInt64(field[7]);
      mForecastTime = field[8];
      mFmiParameterId = field[9];
      mFmiParameterName = toUpperString(field[10]);
      mGribParameterId = field[11];
      mCdmParameterId = field[12];
      mCdmParameterName = field[13];
      mNewbaseParameterId = field[14];
      mNewbaseParameterName = field[15];
      mFmiParameterLevelId = toInt64(field[16]);
      mGrib1ParameterLevelId = toInt64(field[17]);
      mGrib2ParameterLevelId = toInt64(field[18]);
      mParameterLevel = (T::ParamLevel)toInt64(field[19]);
      mFmiParameterUnits = field[20];
      mGribParameterUnits = field[21];
      mForecastType = (short)toInt64(field[22]);
      mForecastNumber = (short)toInt64(field[23]);
      mServerFlags = (unsigned long long)toInt64(field[24]);
      mFlags = toInt64(field[25]);
      mSourceId = toInt64(field[26]);
      mGeometryId = toInt64(field[27]);
      if (c >= 28)
        mModificationTime = field[28];
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return num_compare(mMessageIndex,contentInfo->mMessageIndex);

      case ContentInfo::ComparisonMethod::gribId_producer_generation_level_time:
        res = strcmp(mGribParameterId.c_str(),contentInfo->mGribParameterId.c_str());
        if (res != 0)
          return res;
        res = num_compare(mProducerId,contentInfo->mProducerId);
        if (res != 0)
          return res;
        res = num_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = num_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0)
          return res;
        res = num_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0)
          return res;
        res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0)
          return res;
        res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time:
        res = strcmp(mFmiParameterId.c_str(),contentInfo->mFmiParameterId.c_str());
        if (res != 0)
          return res;
        res = num_compare(mProducerId,contentInfo->mProducerId);
        if (res != 0)
          return res;
        res = num_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = num_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0)
          return res;
        res = num_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0)
          return res;
        res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0)
          return res;
        res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return num_compare(mMessageIndex,contentInfo->mMessageIndex);

      case ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time:
        res = strcmp(mFmiParameterName.c_str(),contentInfo->mFmiParameterName.c_str());
        if (res != 0)
          return res;
        res = num_compare(mProducerId,contentInfo->mProducerId);
        if (res != 0)
          return res;
        res = num_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = num_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0)
          return res;
        res = num_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0)
          return res;
        res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0)
          return res;
        res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::newbaseId_producer_generation_level_time:
        res = strcmp(mNewbaseParameterId.c_str(),contentInfo->mNewbaseParameterId.c_str());
        if (res != 0)
          return res;
        res = num_compare(mProducerId,contentInfo->mProducerId);
        if (res != 0)
          return res;
        res = num_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = num_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0)
          return res;
        res = num_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0)
          return res;
        res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0)
          return res;
        res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::newbaseName_producer_generation_level_time:
        res = strcasecmp(mNewbaseParameterName.c_str(),contentInfo->mNewbaseParameterName.c_str());
        if (res != 0)
          return res;
        res = num_compare(mProducerId,contentInfo->mProducerId);
        if (res != 0)
          return res;
        res = num_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = num_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0)
          return res;
        res = num_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0)
          return res;
        res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0)
          return res;
        res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::cdmId_producer_generation_level_time:
        res = strcasecmp(mCdmParameterId.c_str(),contentInfo->mCdmParameterId.c_str());
        if (res != 0)
          return res;
        res = num_compare(mProducerId,contentInfo->mProducerId);
        if (res != 0)
          return res;
        res = num_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = num_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0)
          return res;
        res = num_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0)
          return res;
        res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0)
          return res;
        res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::cdmName_producer_generation_level_time:
        res = strcasecmp(mCdmParameterName.c_str(),contentInfo->mCdmParameterName.c_str());
        if (res != 0)
          return res;
        res = num_compare(mProducerId,contentInfo->mProducerId);
        if (res != 0)
          return res;
        res = num_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = strcmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = num_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0)
          return res;
        res = num_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0)
          return res;
        res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0)
          return res;
        res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::starttime_file_message:
        res = strcmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return num_compare(mMessageIndex,contentInfo->mMessageIndex);

      case ContentInfo::ComparisonMethod::fmiName_starttime_level_file_message:
        res = strcmp(mFmiParameterName.c_str(),contentInfo->mFmiParameterName.c_str());
        if (res != 0)
          return res;
        res = strcmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::fmiId_fmiLevelId_level_starttime_file_message:
        res = strcmp(mFmiParameterId.c_str(),contentInfo->mFmiParameterId.c_str());
        if (res != 0)
          return res;
        res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = num_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0)
          return res;
        res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0)
          return res;
        res = strcmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = num_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::starttime_fmiId_fmiLevelId_level_file_message:
        res = strcmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = strcmp(mFmiParameterId.c_str(),contentInfo->mFmiParameterId.c_str());
        if (res != 0)
          return res;
        res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = num_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0)
          return res;
        res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0)
          return res;
        res = num_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::starttime_fmiName_fmiLevelId_level_file_message:
        res = strcmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = strcmp(mFmiParameterName.c_str(),contentInfo->mFmiParameterName.c_str());
        if (res != 0)
          return res;
        res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = num_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0)
          return res;
        res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0)
          return res;
        res = num_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::generationId_starttime_file_message:
        res = num_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = strcmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::fmiName_fmiLevelId_level_starttime_file_message:
        res = strcmp(mFmiParameterName.c_str(),contentInfo->mFmiParameterName.c_str());
        if (res != 0)
          return res;
        res = num_compare(mFmiParameterLevelId,contentInfo->mFmiParameterLevelId);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = num_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0)
          return res;
        res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0)
          return res;
        res = num_compare(mGeometryId,contentInfo->mGeometryId);
        if (res != 0)
          return res;
        res = strcmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::starttime_generationId_file_message:
        res = strcmp(mForecastTime.c_str(),contentInfo->mForecastTime.c_str());
        if (res != 0)
          return res;
        res = num_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = num_compare(mForecastType,contentInfo->mForecastType);
        if (res != 0)
          return res;
        res = num_compare(mForecastNumber,contentInfo->mForecastNumber);
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0)
          return res;
        return 0;

      case ContentInfo::ComparisonMethod::producer_file_message:
        res = num_compare(mProducerId,contentInfo->mProducerId);
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        res = num_compare(mMessageIndex,contentInfo->mMessageIndex);
        if (res != 0)
          return res;
        return 0;


      default:
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return num_compare(mMessageIndex,contentInfo->mMessageIndex);
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ContentInfo\n";
    stream << space(level) << "- mFileId                 = " << mFileId << "\n";
    stream << space(level) << "- mFileType               = " << C_INT(mFileType) << "\n";
    stream << space(level) << "- mMessageIndex           = " << mMessageIndex << "\n";
    stream << space(level) << "- mFilePosition           = " << mFilePosition << "\n";
    stream << space(level) << "- mMessageSize            = " << mMessageSize << "\n";
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
    stream << space(level) << "- mFmiParameterLevelId    = " << C_INT(mFmiParameterLevelId) << "\n";
    stream << space(level) << "- mGrib1ParameterLevelId  = " << C_INT(mGrib1ParameterLevelId) << "\n";
    stream << space(level) << "- mGrib2ParameterLevelId  = " << C_INT(mGrib2ParameterLevelId) << "\n";
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}


}
}
