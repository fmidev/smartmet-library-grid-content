#include "GenerationInfo.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{


GenerationInfo::GenerationInfo()
{
  try
  {
    mGenerationId = 0;
    mGenerationType = 0;
    mProducerId = 0;
    mStatus = Status::Disabled;
    mFlags = 0;
    mSourceId = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





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
    mSourceId = generationInfo.mSourceId;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





GenerationInfo::GenerationInfo(const char *csv)
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





GenerationInfo::~GenerationInfo()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





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
    mSourceId = generationInfo.mSourceId;

    return *this;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





std::string GenerationInfo::getCsv()
{
  try
  {
    char st[1000];
    sprintf(st,"%u;%u;%u;%s;%s;%s;%u;%u;%u",
        mGenerationId,
        mGenerationType,
        mProducerId,
        mName.c_str(),
        mDescription.c_str(),
        mAnalysisTime.c_str(),
        mStatus,
        mFlags,
        mSourceId);

    return std::string(st);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





std::string GenerationInfo::getCsvHeader()
{
  try
  {
    std::string header = "generationId;generationType;producerId;name;description;analysisTime;status;flags;sourceId";
    return header;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GenerationInfo::setCsv(const char *csv)
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
      mGenerationId = toInt64(field[0]);
      mGenerationType = toInt64(field[1]);
      mProducerId = toInt64(field[2]);
      mName = field[3];
      mDescription = field[4];
      mAnalysisTime = field[5];
      mStatus = toInt64(field[6]);
      mFlags = toInt64(field[7]);
      mSourceId = toInt64(field[8]);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GenerationInfo::setCsv(std::string csv)
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
        return uint_compare(mGenerationId,generationInfo->mGenerationId);

      case GenerationInfo::ComparisonMethod::generationName:
        return strcasecmp(mName.c_str(),generationInfo->mName.c_str());

      case GenerationInfo::ComparisonMethod::analysisTime_generationId:
        res = strcasecmp(mAnalysisTime.c_str(),generationInfo->mAnalysisTime.c_str());
        if (res != 0)
          return res;
        return uint_compare(mGenerationId,generationInfo->mGenerationId);

      default:
        return uint_compare(mGenerationId,generationInfo->mGenerationId);
    }
    return 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





GenerationInfo* GenerationInfo::duplicate()
{
  try
  {
    return new GenerationInfo(*this);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GenerationInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "GenerationInfo\n";
    stream << space(level) << "- mGenerationId   = " << mGenerationId << "\n";
    stream << space(level) << "- mGenerationType = " << mGenerationType << "\n";
    stream << space(level) << "- mProducerId     = " << mProducerId << "\n";
    stream << space(level) << "- mName           = " << mName << "\n";
    stream << space(level) << "- mDescription    = " << mDescription << "\n";
    stream << space(level) << "- mAnalysisTime   = " << mAnalysisTime << "\n";
    stream << space(level) << "- mStatus         = " << C_INT(mStatus) << "\n";
    stream << space(level) << "- mFlags          = " << mFlags << "\n";
    stream << space(level) << "- mSourceId       = " << mSourceId << "\n";
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




}
}
