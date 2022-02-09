#include "GeometryInfo.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{


GeometryInfo::GeometryInfo()
{
  try
  {
    mGeometryId = 0;
    mProducerId = 0;
    mGenerationId = 0;
    mLevelId = 0;
    mFlags = 0;
    mSourceId = 0;
    mStatus = 0;
    mModificationTime = 0;
    mDeletionTime = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





GeometryInfo::GeometryInfo(const GeometryInfo& geometryInfo)
{
  try
  {
    mGenerationId = geometryInfo.mGenerationId;
    mGeometryId = geometryInfo.mGeometryId;
    mProducerId = geometryInfo.mProducerId;
    mLevelId = geometryInfo.mLevelId;
    mFlags = geometryInfo.mFlags;
    mSourceId = geometryInfo.mSourceId;
    mStatus = geometryInfo.mStatus;
    mModificationTime = geometryInfo.mModificationTime;
    mDeletionTime = geometryInfo.mDeletionTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





GeometryInfo::GeometryInfo(const char *csv)
{
  try
  {
    mGeometryId = 0;
    mProducerId = 0;
    mGenerationId = 0;
    mLevelId = 0;
    mFlags = 0;
    mSourceId = 0;
    mStatus = 0;
    mModificationTime = 0;
    mDeletionTime = 0;
    setCsv(csv);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





GeometryInfo::~GeometryInfo()
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





GeometryInfo& GeometryInfo::operator=(const GeometryInfo& geometryInfo)
{
  try
  {
    if (&geometryInfo == this)
      return *this;

    mGeometryId = geometryInfo.mGeometryId;
    mProducerId = geometryInfo.mProducerId;
    mGenerationId = geometryInfo.mGenerationId;
    mLevelId = geometryInfo.mLevelId;
    mFlags = geometryInfo.mFlags;
    mSourceId = geometryInfo.mSourceId;
    mStatus = geometryInfo.mStatus;
    mModificationTime = geometryInfo.mModificationTime;
    mDeletionTime = geometryInfo.mDeletionTime;

    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




std::string GeometryInfo::getCsv()
{
  try
  {
    char st[1000];
    sprintf(st,"%u;%d;%d;%u;%u;%u;%u;%ld;%ld",
        mGenerationId,
        mGeometryId,
        mLevelId,
        mProducerId,
        mSourceId,
        mFlags,
        (uint)mStatus,
        mModificationTime,
        mDeletionTime);

    return std::string(st);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::string GeometryInfo::getCsvHeader()
{
  try
  {
    std::string header = "generationId;geometryId;mLevelId;producerId;sourceId;flags;status;modificationTimeT;deletionTimeT";
    return header;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfo::setCsv(const char *csv)
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

    if (c >= 8)
    {
      mGenerationId = toUInt32(field[0]);
      mGeometryId = toInt32(field[1]);
      mLevelId = toInt32(field[2]);
      mProducerId = toUInt32(field[3]);
      mSourceId = toUInt32(field[4]);
      mFlags = toUInt32(field[5]);
      mStatus = toUInt32(field[6]);
      mModificationTime = toInt64(field[7]);
      mDeletionTime = toInt64(field[8]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfo::setCsv(const std::string& csv)
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





int GeometryInfo::compare(uint comparisonMethod,GeometryInfo *geometryInfo)
{
  try
  {
    int res = 0;
    switch (comparisonMethod)
    {
      case ComparisonMethod::none:
        return 0;

      case ComparisonMethod::generationId:
        res = num_compare(mGenerationId,geometryInfo->mGenerationId);
        if (res != 0)
          return res;
        res = num_compare(mGeometryId,geometryInfo->mGeometryId);
        if (res != 0)
          return res;
        return num_compare(mLevelId,geometryInfo->mLevelId);

      case ComparisonMethod::producerId:
        res = num_compare(mProducerId,geometryInfo->mProducerId);
        if (res != 0)
          return res;
        res = num_compare(mGeometryId,geometryInfo->mGeometryId);
        if (res != 0)
          return res;
        return num_compare(mLevelId,geometryInfo->mLevelId);
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





GeometryInfo* GeometryInfo::duplicate()
{
  try
  {
    return new GeometryInfo(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "GeometryInfo\n";
    stream << space(level) << "- mGenerationId     = " << mGenerationId << "\n";
    stream << space(level) << "- mGeometryId       = " << mGeometryId << "\n";
    stream << space(level) << "- mLevelId          = " << mLevelId << "\n";
    stream << space(level) << "- mProducerId       = " << mProducerId << "\n";
    stream << space(level) << "- mSourceId         = " << mSourceId << "\n";
    stream << space(level) << "- mFlags            = " << mFlags << "\n";
    stream << space(level) << "- mStatus           = " << (uint)mStatus << "\n";
    stream << space(level) << "- mModificationTime = " << utcTimeFromTimeT(mModificationTime) << "\n";
    stream << space(level) << "- mDeletionTime     = " << utcTimeFromTimeT(mDeletionTime) << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




}
}
