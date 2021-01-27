#include "ProducerInfo.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{




ProducerInfo::ProducerInfo()
{
  try
  {
    mProducerId = 0;
    mFlags = 0;
    mSourceId = 0;
    mHash = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ProducerInfo::ProducerInfo(const ProducerInfo& producerInfo)
{
  try
  {
    mProducerId = producerInfo.mProducerId;
    mName = producerInfo.mName;
    mTitle = producerInfo.mTitle;
    mDescription = producerInfo.mDescription;
    mFlags = producerInfo.mFlags;
    mSourceId = producerInfo.mSourceId;
    mHash = producerInfo.mHash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ProducerInfo::ProducerInfo(const char *csv)
{
  try
  {
    mProducerId = 0;
    mFlags = 0;
    mSourceId = 0;
    mHash = 0;
    setCsv(csv);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




ProducerInfo::~ProducerInfo()
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





ProducerInfo& ProducerInfo::operator=(const ProducerInfo& producerInfo)
{
  try
  {
    if (&producerInfo == this)
      return *this;

    mProducerId = producerInfo.mProducerId;
    mName = producerInfo.mName;
    mTitle = producerInfo.mTitle;
    mDescription = producerInfo.mDescription;
    mFlags = producerInfo.mFlags;
    mSourceId = producerInfo.mSourceId;
    mHash = producerInfo.mHash;

    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::string ProducerInfo::getCsv()
{
  try
  {
    char st[1000];
    sprintf(st,"%u;%s;%s;%s;%u;%u",
        mProducerId,
        mName.c_str(),
        mTitle.c_str(),
        mDescription.c_str(),
        mFlags,
        mSourceId);

    return std::string(st);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::string ProducerInfo::getCsvHeader()
{
  try
  {
    std::string header = "producerId;name;title;description;flags;producerId";
    return header;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ProducerInfo::setCsv(const char *csv)
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

    if (c >= 5)
    {
       mProducerId = toUInt32(field[0]);
       mName = field[1];
       mTitle = field[2];
       mDescription = field[3];
       mFlags = toUInt32(field[4]);
       mSourceId = toUInt32(field[5]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ProducerInfo::setCsv(const std::string& csv)
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





ProducerInfo* ProducerInfo::duplicate()
{
  try
  {
    return new ProducerInfo(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ProducerInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ProducerInfo\n";
    stream << space(level) << "- mProducerId  = " << mProducerId << "\n";
    stream << space(level) << "- mName        = " << mName << "\n";
    stream << space(level) << "- mTitle       = " << mTitle << "\n";
    stream << space(level) << "- mDescription = " << mDescription << "\n";
    stream << space(level) << "- mFlags       = " << mFlags << "\n";
    stream << space(level) << "- mSourceId    = " << mSourceId << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



}
}
