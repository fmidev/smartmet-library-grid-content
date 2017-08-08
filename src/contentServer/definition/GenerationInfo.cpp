#include "GenerationInfo.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"


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
    mStatus = T::GenerationStatus::STATUS_DISABLED;
    mFlags = 0;
    mSourceId = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





GenerationInfo::GenerationInfo(GenerationInfo& generationInfo)
{
  try
  {
    mGenerationId = generationInfo.mGenerationId;
    mGenerationType = generationInfo.mGenerationType;
    mProducerId = generationInfo.mProducerId;
    mName = generationInfo.mName;
    mDescription = generationInfo.mDescription;
    mStatus = generationInfo.mStatus;
    mFlags = generationInfo.mFlags;
    mSourceId = generationInfo.mSourceId;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





GenerationInfo::~GenerationInfo()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void GenerationInfo::operator=(GenerationInfo& generationInfo)
{
  try
  {
    if (&generationInfo == this)
      return;

    mGenerationId = generationInfo.mGenerationId;
    mGenerationType = generationInfo.mGenerationType;
    mProducerId = generationInfo.mProducerId;
    mName = generationInfo.mName;
    mDescription = generationInfo.mDescription;
    mStatus = generationInfo.mStatus;
    mFlags = generationInfo.mFlags;
    mSourceId = generationInfo.mSourceId;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





std::string GenerationInfo::getCsv()
{
  try
  {
    char st[1000];
    sprintf(st,"%u;%u;%u;%s;%s;%u;%u;%u",
        mGenerationId,
        mGenerationType,
        mProducerId,
        mName.c_str(),
        mDescription.c_str(),
        (uint)mStatus,
        mFlags,
        mSourceId);

    return std::string(st);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





std::string GenerationInfo::getCsvHeader()
{
  try
  {
    std::string header = "generationId;generationType;producerId;name;description;status;flags;sourceId";
    return header;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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

    if (c >= 7)
    {
      mGenerationId = (uint)atoll(field[0]);
      mGenerationType = (uint)atoll(field[1]);
      mProducerId = (uint)atoll(field[2]);
      mName = field[3];
      mDescription = field[4];
      mStatus = (GenerationStatus)atoll(field[5]);
      mFlags = (uint)atoll(field[6]);
      mSourceId = (uint)atoll(field[7]);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    stream << space(level) << "- mStatus         = " << (int)mStatus << "\n";
    stream << space(level) << "- mFlags          = " << mFlags << "\n";
    stream << space(level) << "- mSourceId       = " << mSourceId << "\n";
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




}
}
