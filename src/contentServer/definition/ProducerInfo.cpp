#include "ProducerInfo.h"
#include <grid-files/common/Exception.h>
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
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ProducerInfo::ProducerInfo(ProducerInfo& producerInfo)
{
  try
  {
    mProducerId = producerInfo.mProducerId;
    mName = producerInfo.mName;
    mTitle = producerInfo.mTitle;
    mDescription = producerInfo.mDescription;
    mFlags = producerInfo.mFlags;
    mSourceId = producerInfo.mSourceId;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ProducerInfo::ProducerInfo(const char *csv)
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




ProducerInfo::~ProducerInfo()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ProducerInfo& ProducerInfo::operator=(ProducerInfo& producerInfo)
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

    return *this;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
       mProducerId = toInt64(field[0]);
       mName = field[1];
       mTitle = field[2];
       mDescription = field[3];
       mFlags = toInt64(field[4]);
       mSourceId = toInt64(field[5]);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ProducerInfo::setCsv(std::string csv)
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





ProducerInfo* ProducerInfo::duplicate()
{
  try
  {
    return new ProducerInfo(*this);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}



}
}
