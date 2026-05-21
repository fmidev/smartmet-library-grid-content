#include "GenerationInfo.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{


/*! \brief Default constructor for GenerationInfo. */

GenerationInfo::GenerationInfo()
{
  try
  {
    mGenerationId = 0;
    mGenerationType = 0;
    mProducerId = 0;
    mStatus = Status::Disabled;
    mFlags = 0;
    mStorageId = 0;
    mSourceId = 0;
    mDeletionTime = 0;
    mModificationTime = 0;
    mContentStartTime = 0;
    mContentEndTime = 0;
    mContentHash = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Copy constructor for GenerationInfo. */

GenerationInfo::GenerationInfo(const GenerationInfo& generationInfo)
{
  try
  {
    mGenerationId = generationInfo.mGenerationId;
    mGenerationType = generationInfo.mGenerationType;
    mProducerId = generationInfo.mProducerId;
    mName = generationInfo.mName;
    mDescription = generationInfo.mDescription;
    mAnalysisTime = generationInfo.mAnalysisTime;
    mStatus = generationInfo.mStatus;
    mFlags = generationInfo.mFlags;
    mStorageId = generationInfo.mStorageId;
    mSourceId = generationInfo.mSourceId;
    mDeletionTime = generationInfo.mDeletionTime;
    mModificationTime = generationInfo.mModificationTime;
    mContentStartTime = generationInfo.mContentStartTime;
    mContentEndTime = generationInfo.mContentEndTime;
    mContentHash = generationInfo.mContentHash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Construct GenerationInfo from a CSV-formatted string. */

GenerationInfo::GenerationInfo(const char *csv)
{
  try
  {
    mGenerationId = 0;
    mGenerationType = 0;
    mProducerId = 0;
    mStatus = Status::Disabled;
    mFlags = 0;
    mStorageId = 0;
    mSourceId = 0;
    mDeletionTime = 0;
    mModificationTime = 0;
    setCsv(csv);
    mContentStartTime = 0;
    mContentEndTime = 0;
    mContentHash = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Destructor for GenerationInfo. */

GenerationInfo::~GenerationInfo()
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





/*! \brief Copy assignment operator for GenerationInfo. */

GenerationInfo& GenerationInfo::operator=(const GenerationInfo& generationInfo)
{
  try
  {
    if (&generationInfo == this)
      return *this;

    mGenerationId = generationInfo.mGenerationId;
    mGenerationType = generationInfo.mGenerationType;
    mProducerId = generationInfo.mProducerId;
    mName = generationInfo.mName;
    mDescription = generationInfo.mDescription;
    mAnalysisTime = generationInfo.mAnalysisTime;
    mStatus = generationInfo.mStatus;
    mFlags = generationInfo.mFlags;
    mStorageId = generationInfo.mStorageId;
    mSourceId = generationInfo.mSourceId;
    mDeletionTime = generationInfo.mDeletionTime;
    mModificationTime = generationInfo.mModificationTime;
    mContentStartTime = generationInfo.mContentStartTime;
    mContentEndTime = generationInfo.mContentEndTime;
    mContentHash = generationInfo.mContentHash;

    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return this record formatted as a CSV row. */

std::string GenerationInfo::getCsv()
{
  try
  {
    char st[1000];
    snprintf(st,sizeof(st),"%lu;%u;%u;%s;%s;%s;%u;%u;%u;%ld;%ld;%ld;%ld;%ld;%u",
        mGenerationId,
        mGenerationType,
        mProducerId,
        mName.c_str(),
        mDescription.c_str(),
        mAnalysisTime.c_str(),
        mStatus,
        mFlags,
        mSourceId,
        mDeletionTime,
        mModificationTime,
        mContentStartTime,
        mContentEndTime,
        mContentHash,
        mStorageId
    );

    return std::string(st);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return the CSV header row describing the fields. */

std::string GenerationInfo::getCsvHeader()
{
  try
  {
    std::string header = "generationId;generationType;producerId;name;description;analysisTime;status;flags;sourceId;deletionTimeT;mModificationTimeT;contentStartTime;contentEndTime;contentHash;mStorageId";
    return header;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Populate this record from a CSV row C string. */

void GenerationInfo::setCsv(const char *csv)
{
  try
  {
    char *field[100];
    std::string tmp(csv);

    uint c = 0;
    field[0] = tmp.data();
    char *p = tmp.data();
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
      mGenerationId = toUInt64(field[0]);
      mGenerationType = toUInt32(field[1]);
      mProducerId = toUInt32(field[2]);
      mName = field[3];
      mDescription = field[4];
      mAnalysisTime = field[5];
      mStatus = toInt64(field[6]);
      mFlags = toInt64(field[7]);
      mSourceId = toInt64(field[8]);
      if (c >= 9)
        mDeletionTime = toInt64(field[9]);
      if (c >= 10)
        mModificationTime = toInt64(field[10]);
      if (c >= 11)
        mContentStartTime = toInt64(field[11]);
      if (c >= 12)
        mContentEndTime = toInt64(field[12]);
      if (c >= 13)
        mContentHash = toInt64(field[13]);
      if (c >= 14)
        mStorageId = toUInt64(field[14]);
    }

  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Populate this record from a CSV row std::string. */

void GenerationInfo::setCsv(const std::string& csv)
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





/*! \brief Compare two records according to the selected comparison method. */

int GenerationInfo::compare(uint comparisonMethod,GenerationInfo *generationInfo)
{
  try
  {
    if (generationInfo == nullptr)
      return 0;

    int res = 0;

    switch (comparisonMethod)
    {
      case GenerationInfo::ComparisonMethod::generationId:
        return num_compare(mGenerationId,generationInfo->mGenerationId);

      case GenerationInfo::ComparisonMethod::generationName:
        return strcasecmp(mName.c_str(),generationInfo->mName.c_str());

      case GenerationInfo::ComparisonMethod::analysisTime_generationId:
        res = strcmp(mAnalysisTime.c_str(),generationInfo->mAnalysisTime.c_str());
        if (res != 0)
          return res;
        return num_compare(mGenerationId,generationInfo->mGenerationId);

      case GenerationInfo::ComparisonMethod::producerId:
        res = num_compare(mProducerId,generationInfo->mProducerId);
        if (res != 0)
          return res;
        return num_compare(mGenerationId,generationInfo->mGenerationId);

      case GenerationInfo::ComparisonMethod::analysisTime:
        return strcmp(mAnalysisTime.c_str(),generationInfo->mAnalysisTime.c_str());

      default:
        return num_compare(mGenerationId,generationInfo->mGenerationId);
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return a deep copy of this GenerationInfo. */

GenerationInfo* GenerationInfo::duplicate()
{
  try
  {
    return new GenerationInfo(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Print the record contents to the given stream. */

void GenerationInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "GenerationInfo\n";
    stream << space(level) << "- mGenerationId     = " << mGenerationId << "\n";
    stream << space(level) << "- mGenerationType   = " << mGenerationType << "\n";
    stream << space(level) << "- mProducerId       = " << mProducerId << "\n";
    stream << space(level) << "- mName             = " << mName << "\n";
    stream << space(level) << "- mDescription      = " << mDescription << "\n";
    stream << space(level) << "- mAnalysisTime     = " << mAnalysisTime << "\n";
    stream << space(level) << "- mStatus           = " << C_INT(mStatus) << "\n";
    stream << space(level) << "- mFlags            = " << mFlags << "\n";
    stream << space(level) << "- mStorageId        = " << mStorageId << "\n";
    stream << space(level) << "- mSourceId         = " << mSourceId << "\n";
    stream << space(level) << "- mDeletionTime     = " << utcTimeFromTimeT(mDeletionTime) << "\n";
    stream << space(level) << "- mModificationTime = " << utcTimeFromTimeT(mModificationTime) << "\n";
    stream << space(level) << "- mContentStartTime = " << utcTimeFromTimeT(mContentStartTime) << "\n";
    stream << space(level) << "- mContentEndTime   = " << utcTimeFromTimeT(mContentEndTime) << "\n";
    stream << space(level) << "- mContentHash      = " << mContentHash << "\n";

  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




}
}
