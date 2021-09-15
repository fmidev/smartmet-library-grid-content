#include "ContentInfo.h"

#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/GeneralDefinitions.h>
#include <grid-files/common/StringFactory.h>
#include <string.h>


namespace SmartMet
{
namespace T
{

ContentInfo::ContentInfo()
{
  try
  {
    mProducerId = 0;
    mGenerationId = 0;
    mFileType = T::FileTypeValue::Unknown;
    mFileId = 0;
    mMessageIndex = 0;
    mFilePosition = 0;
    mMessageSize = 0;
    mFmiParameterId = 0;
    mGribParameterId =  0;
    mNewbaseParameterId = 0;
    mFmiParameterLevelId = 0;
    mGrib1ParameterLevelId = 0;
    mGrib2ParameterLevelId = 0;
    mParameterLevel = 0;
    mForecastType = 0;
    mForecastNumber = 0;
    mFlags = 0;
    mSourceId = 0;
    mGeometryId = 0;
    mForecastTimeUTC = 0;
    mModificationTime = 0;
    mDeletionTime = 0;
    mFmiParameterName = 0;
    mNewbaseParameterName = 0;
    mNetCdfParameterName = 0;
    mForecastTime = 0;
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
    mProducerId = contentInfo.mProducerId;
    mGenerationId = contentInfo.mGenerationId;
    mFileType = contentInfo.mFileType;
    mFileId = contentInfo.mFileId;
    mMessageIndex = contentInfo.mMessageIndex;
    mFilePosition = contentInfo.mFilePosition;
    mMessageSize = contentInfo.mMessageSize;
    mForecastTimeUTC = contentInfo.mForecastTimeUTC;
    mFmiParameterId = contentInfo.mFmiParameterId;
    mFmiParameterName = contentInfo.mFmiParameterName;
    mGribParameterId = contentInfo.mGribParameterId;
    mNewbaseParameterId = contentInfo.mNewbaseParameterId;
    mNewbaseParameterName = contentInfo.mNewbaseParameterName;
    mFmiParameterLevelId = contentInfo.mFmiParameterLevelId;
    mGrib1ParameterLevelId = contentInfo.mGrib1ParameterLevelId;
    mGrib2ParameterLevelId = contentInfo.mGrib2ParameterLevelId;
    mParameterLevel = contentInfo.mParameterLevel;
    mForecastType = contentInfo.mForecastType;
    mForecastNumber = contentInfo.mForecastNumber;
    mFlags = contentInfo.mFlags;
    mSourceId = contentInfo.mSourceId;
    mGeometryId = contentInfo.mGeometryId;
    mModificationTime = contentInfo.mModificationTime;
    mDeletionTime = contentInfo.mDeletionTime;

    mFmiParameterName = contentInfo.mFmiParameterName;
    mNewbaseParameterName = contentInfo.mNewbaseParameterName;
    mNetCdfParameterName = contentInfo.mNetCdfParameterName;
    mForecastTime = contentInfo.mForecastTime;
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
    mProducerId = 0;
    mGenerationId = 0;
    mFileType = T::FileTypeValue::Unknown;
    mFileId = 0;
    mMessageIndex = 0;
    mFilePosition = 0;
    mMessageSize = 0;
    mFmiParameterId = 0;
    mGribParameterId =  0;
    mNewbaseParameterId = 0;
    mFmiParameterLevelId = 0;
    mFmiParameterName = 0;
    mNewbaseParameterName = 0;
    mNetCdfParameterName = 0;
    mGrib1ParameterLevelId = 0;
    mGrib2ParameterLevelId = 0;
    mParameterLevel = 0;
    mForecastType = 0;
    mForecastNumber = 0;
    mFlags = 0;
    mSourceId = 0;
    mGeometryId = 0;
    mForecastTime = 0;
    mForecastTimeUTC = 0;
    mModificationTime = 0;
    mDeletionTime = 0;
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

    mProducerId = contentInfo.mProducerId;
    mGenerationId = contentInfo.mGenerationId;
    mFileType = contentInfo.mFileType;
    mFileId = contentInfo.mFileId;
    mMessageIndex = contentInfo.mMessageIndex;
    mFilePosition = contentInfo.mFilePosition;
    mMessageSize = contentInfo.mMessageSize;
    mForecastTimeUTC = contentInfo.mForecastTimeUTC;
    mFmiParameterId = contentInfo.mFmiParameterId;
    mFmiParameterName = contentInfo.mFmiParameterName;
    mGribParameterId = contentInfo.mGribParameterId;
    mNewbaseParameterId = contentInfo.mNewbaseParameterId;
    mFmiParameterLevelId = contentInfo.mFmiParameterLevelId;
    mGrib1ParameterLevelId = contentInfo.mGrib1ParameterLevelId;
    mGrib2ParameterLevelId = contentInfo.mGrib2ParameterLevelId;
    mParameterLevel = contentInfo.mParameterLevel;
    mForecastType = contentInfo.mForecastType;
    mForecastNumber = contentInfo.mForecastNumber;
    mFlags = contentInfo.mFlags;
    mSourceId = contentInfo.mSourceId;
    mGeometryId = contentInfo.mGeometryId;
    mModificationTime = contentInfo.mModificationTime;
    mDeletionTime = contentInfo.mDeletionTime;

    mForecastTime = contentInfo.mForecastTime;
    mFmiParameterName = contentInfo.mFmiParameterName;
    mNewbaseParameterName = contentInfo.mNewbaseParameterName;
    mNetCdfParameterName = contentInfo.mNetCdfParameterName;

    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





char* ContentInfo::getFmiParameterName()
{
  try
  {
    return stringFactory[mFmiParameterName];
    //return mFmiParameterName;
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
    uint len = strlen(name);
    char buf[200];
    for (uint t=0; t<=len; t++)
      buf[t] = toupper(name[t]);

    mFmiParameterName = stringFactory.create(buf);
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
    setFmiParameterName(name.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





char* ContentInfo::getNewbaseParameterName()
{
  try
  {
    return stringFactory[mNewbaseParameterName];
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfo::setNewbaseParameterName(const char *name)
{
  try
  {
    uint len = strlen(name);
    char buf[200];
    for (uint t=0; t<=len; t++)
      buf[t] = toupper(name[t]);

    mNewbaseParameterName = stringFactory.create(buf);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfo::setNewbaseParameterName(const std::string& name)
{
  try
  {
    setNewbaseParameterName(name.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





char* ContentInfo::getNetCdfParameterName()
{
  try
  {
    return stringFactory[mNetCdfParameterName];
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfo::setNetCdfParameterName(const char *name)
{
  try
  {
    uint len = strlen(name);
    char buf[200];
    for (uint t=0; t<=len; t++)
      buf[t] = toupper(name[t]);

    mNetCdfParameterName = stringFactory.create(buf);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfo::setNetCdfParameterName(const std::string& name)
{
  try
  {
    setNetCdfParameterName(name.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





char* ContentInfo::getForecastTime()
{
  try
  {
    return stringFactory[mForecastTime];
    //return mForecastTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfo::setForecastTime(const char *ftime)
{
  try
  {
    mForecastTime = stringFactory.create(ftime);
/*

    if (mForecastTime != EMPTY_STRING)
    {
      delete[] mForecastTime;
      mForecastTime = (char*)EMPTY_STRING;
    }

    if (ftime != nullptr)
    {
      uint len = strlen(ftime);
      mForecastTime = new char[len+1];
      strcpy(mForecastTime,ftime);
    }
*/
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfo::setForecastTime(const std::string& ftime)
{
  try
  {
    setForecastTime(ftime.c_str());
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
    sprintf(st,"%u;%u;%u;%llu;%u;%u;%u;;%s;%u;%s;%u;%s;;%u;%s;%u;%u;%u;%d;;;%d;%d;;%u;%u;%u;%ld;%ld",
        mFileId,
        mMessageIndex,
        mFileType,
        mFilePosition,
        mMessageSize,
        mProducerId,
        mGenerationId,
        //mGroupFlags,
        getForecastTime(),
        mFmiParameterId,
        getFmiParameterName(),
        mGribParameterId,
        getNetCdfParameterName(),
        //mCdmParameterName.c_str(),
        mNewbaseParameterId,
        getNewbaseParameterName(),
        mFmiParameterLevelId,
        mGrib1ParameterLevelId,
        mGrib2ParameterLevelId,
        mParameterLevel,
        //mFmiParameterUnits.c_str(),
        //mGribParameterUnits.c_str(),
        mForecastType,
        mForecastNumber,
        //mServerFlags,
        mFlags,
        mSourceId,
        mGeometryId,
        mModificationTime,
        mDeletionTime
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
    ulonglong paramId = mFmiParameterId;
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
    std::string header = "fileId;messageIndex;fileType;filePosition;messageSize;producerId;generationId;;startTime;fmiParameterId;fmiParameterName;gribParameterId;netCdfParameterName;;newbaseParameterId;newbaseParameterName;fmiParameterLevelId;grib1ParameterLevelId;grib2ParameterLevelId;parameterLevel;;;mForecastType;mForecastNumber;serverFlags;flags;sourceId;geometryId;modificationTimeT;deletionTimeT";
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
      mFileId = toUInt32(field[0]);
      mMessageIndex = toUInt32(field[1]);
      mFileType = toUInt8(field[2]);
      mFilePosition = toUInt64(field[3]);
      mMessageSize = toUInt32(field[4]);
      mProducerId = toUInt32(field[5]);
      mGenerationId = toUInt32(field[6]);
      //mGroupFlags = toUInt32(field[7]);
      setForecastTime(field[8]);
      mForecastTimeUTC = utcTimeToTimeT(getForecastTime());
      mFmiParameterId = toUInt32(field[9]);
      setFmiParameterName(field[10]);
      mGribParameterId = toUInt32(field[11]);
      setNetCdfParameterName(field[12]);
      //mCdmParameterName = field[13];
      mNewbaseParameterId = toUInt32(field[14]);
      setNewbaseParameterName(field[15]);
      mFmiParameterLevelId = toUInt8(field[16]);
      mGrib1ParameterLevelId = toUInt8(field[17]);
      mGrib2ParameterLevelId = toUInt8(field[18]);
      mParameterLevel = toInt32(field[19]);
      //mFmiParameterUnits = field[20];
      //mGribParameterUnits = field[21];
      mForecastType = toInt16(field[22]);
      mForecastNumber = toInt16(field[23]);
      //mServerFlags = toUInt64(field[24]);
      mFlags = toUInt32(field[25]);
      mSourceId = toUInt32(field[26]);
      mGeometryId = toInt32(field[27]);
      if (c >= 28)
        mModificationTime = toInt64(field[28]);
      if (c >= 29)
        mDeletionTime = toInt64(field[29]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfo::setCsv(const std::string& csv)
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





bool ContentInfo::hasKey(T::ParamKeyType parameterKeyType,const std::string& parameterKey)
{
  try
  {
    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        if (mFmiParameterId != 0 && mFmiParameterId == toUInt32(parameterKey.c_str()))
          return true;
        else
          return false;

      case T::ParamKeyTypeValue::FMI_NAME:
        if (strcasecmp(getFmiParameterName(),parameterKey.c_str()) == 0)
          return true;
        else
          return false;

      case T::ParamKeyTypeValue::GRIB_ID:
        if (mGribParameterId != 0  &&  mGribParameterId == toUInt32(parameterKey))
          return true;
        else
          return false;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        if (mNewbaseParameterId != 0 && mNewbaseParameterId == toUInt32(parameterKey.c_str()))
          return true;
        else
          return false;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        if (strcasecmp(getNewbaseParameterName(),parameterKey.c_str()) == 0)
          return true;
        else
          return false;

      case T::ParamKeyTypeValue::NETCDF_NAME:
        if (strcasecmp(getNetCdfParameterName(),parameterKey.c_str()) == 0)
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
        res = num_compare(mProducerId,contentInfo->mProducerId);
        if (res != 0)
          return res;
        res = num_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = num_compare(mGribParameterId,contentInfo->mGribParameterId);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
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
        res = num_compare(mProducerId,contentInfo->mProducerId);
        if (res != 0)
          return res;
        res = num_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = num_compare(mFmiParameterId,contentInfo->mFmiParameterId);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
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
        res = num_compare(mProducerId,contentInfo->mProducerId);
        if (res != 0)
          return res;
        res = num_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = num_compare(mFmiParameterName,contentInfo->mFmiParameterName);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
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
        res = num_compare(mProducerId,contentInfo->mProducerId);
        if (res != 0)
          return res;
        res = num_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = num_compare(mNewbaseParameterId,contentInfo->mNewbaseParameterId);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
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
        res = num_compare(mProducerId,contentInfo->mProducerId);
        if (res != 0)
          return res;
        res = num_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = num_compare(mNewbaseParameterName,contentInfo->mNewbaseParameterName);
        if (res != 0)
          return res;
        res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
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

      case ContentInfo::ComparisonMethod::netCdfName_producer_generation_level_time:
        res = num_compare(mProducerId,contentInfo->mProducerId);
        if (res != 0)
          return res;
        res = num_compare(mGenerationId,contentInfo->mGenerationId);
        if (res != 0)
          return res;
        res = num_compare(mParameterLevel,contentInfo->mParameterLevel);
        if (res != 0)
          return res;
        res = num_compare(mNetCdfParameterName,contentInfo->mNetCdfParameterName);
        if (res != 0)
          return res;
        res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
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
        res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
        if (res != 0)
          return res;
        res = num_compare(mFileId,contentInfo->mFileId);
        if (res != 0)
          return res;
        return num_compare(mMessageIndex,contentInfo->mMessageIndex);

      case ContentInfo::ComparisonMethod::fmiName_starttime_level_file_message:
        res = num_compare(mFmiParameterName,contentInfo->mFmiParameterName);
        if (res != 0)
          return res;
        res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
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
        res = num_compare(mFmiParameterId,contentInfo->mFmiParameterId);
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
        res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
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
        res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
        if (res != 0)
          return res;
        res = num_compare(mFmiParameterId,contentInfo->mFmiParameterId);
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
        res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
        if (res != 0)
          return res;
        res = num_compare(mFmiParameterName,contentInfo->mFmiParameterName);
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
        res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
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
        res = num_compare(mFmiParameterName,contentInfo->mFmiParameterName);
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
        res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
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
        res = time_compare(mForecastTimeUTC,contentInfo->mForecastTimeUTC);
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
    stream << space(level) << "- mForecastTime           = " << getForecastTime() << "\n";
    stream << space(level) << "- mForecastTimeUTC        = " << mForecastTimeUTC << "\n";
    stream << space(level) << "- mFmiParameterId         = " << mFmiParameterId << "\n";
    stream << space(level) << "- mFmiParameterName       = " << getFmiParameterName() << "\n";
    stream << space(level) << "- mGribParameterId        = " << mGribParameterId << "\n";
    stream << space(level) << "- mNetCdfParameterName    = " << getNetCdfParameterName() << "\n";
    stream << space(level) << "- mNewbaseParameterId     = " << mNewbaseParameterId << "\n";
    stream << space(level) << "- mNewbaseParameterName   = " << getNewbaseParameterName() << "\n";
    stream << space(level) << "- mFmiParameterLevelId    = " << C_INT(mFmiParameterLevelId) << "\n";
    stream << space(level) << "- mGrib1ParameterLevelId  = " << C_INT(mGrib1ParameterLevelId) << "\n";
    stream << space(level) << "- mGrib2ParameterLevelId  = " << C_INT(mGrib2ParameterLevelId) << "\n";
    stream << space(level) << "- mParameterLevel         = " << mParameterLevel << "\n";
    stream << space(level) << "- mForecastType           = " << mForecastType << "\n";
    stream << space(level) << "- mForecastNumber         = " << mForecastNumber << "\n";
    stream << space(level) << "- mFlags                  = " << mFlags << "\n";
    stream << space(level) << "- mSourceId               = " << mSourceId << "\n";
    stream << space(level) << "- mGeometryId             = " << mGeometryId << "\n";
    stream << space(level) << "- mModificationTime       = " << utcTimeFromTimeT(mModificationTime) << "\n";
    stream << space(level) << "- mDeletionTime           = " << utcTimeFromTimeT(mDeletionTime) << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}


}
}
