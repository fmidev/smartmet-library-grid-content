#include "ProducerInfo.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{




/*! \brief Default constructor for ProducerInfo. */

ProducerInfo::ProducerInfo()
{
  try
  {
    mProducerId = 0;
    mFlags = 0;
    mStorageId = 0;
    mSourceId = 0;
    mStatus = 0;
    mHash = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Copy constructor for ProducerInfo. */

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
    mStorageId = producerInfo.mStorageId;
    mStatus = producerInfo.mStatus;

    mHash = producerInfo.mHash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Construct ProducerInfo from a CSV-formatted string. */

ProducerInfo::ProducerInfo(const char *csv)
{
  try
  {
    mProducerId = 0;
    mFlags = 0;
    mSourceId = 0;
    mStorageId = 0;
    mStatus = 0;
    mHash = 0;
    setCsv(csv);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Destructor for ProducerInfo. */

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





/*! \brief Copy assignment operator for ProducerInfo. */

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
    mStorageId = producerInfo.mStorageId;
    mStatus = producerInfo.mStatus;

    mHash = producerInfo.mHash;

    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return this record formatted as a CSV row. */

std::string ProducerInfo::getCsv()
{
  try
  {
    char st[1000];
    snprintf(st,sizeof(st),"%u;%s;%s;%s;%u;%u;%u;%u",
        mProducerId,
        mName.c_str(),
        mTitle.c_str(),
        mDescription.c_str(),
        mFlags,
        mSourceId,
        (uint)mStatus,
        mStorageId);

    return std::string(st);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return the CSV header row describing the fields. */

std::string ProducerInfo::getCsvHeader()
{
  try
  {
    std::string header = "producerId;name;title;description;flags;sourceId;status;storageId";
    return header;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Populate this record from a CSV row C string. */

void ProducerInfo::setCsv(const char *csv)
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

    if (c >= 5)
    {
       mProducerId = toUInt32(field[0]);
       mName = field[1];
       mTitle = field[2];
       mDescription = field[3];
       mFlags = toUInt32(field[4]);
       mSourceId = toUInt32(field[5]);
    }

    if (c >= 6)
      mStatus = toInt64(field[6]);

    if (c >= 7)
      mStorageId = toUInt32(field[7]);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Populate this record from a CSV row std::string. */

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





/*! \brief Return a deep copy of this ProducerInfo. */

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





/*! \brief Print the record contents to the given stream. */

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
    stream << space(level) << "- mStorageId   = " << mStorageId << "\n";
    stream << space(level) << "- mSourceId    = " << mSourceId << "\n";
    stream << space(level) << "- mStatus      = " << (uint)mStatus << "\n";
    stream << space(level) << "- mHash        = " << mHash << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



}
}
