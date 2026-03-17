#include "EventInfo.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{



EventInfo::EventInfo()
{
  try
  {
    mEventId = 0;
    mEventTime = time(nullptr);
    mServerTime = 0;
    mType = ContentServer::EventType::UNKNOWN;
    mId1 = 0;
    mId2 = 0;
    mId3 = 0;
    mFlags = 0;

    nextItem = nullptr;
    prevItem = nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





EventInfo::EventInfo(const EventInfo& dataEventInfo)
{
  try
  {
    mEventId = dataEventInfo.mEventId;
    mEventTime = dataEventInfo.mEventTime;
    mServerTime = dataEventInfo.mServerTime;
    mType = dataEventInfo.mType;
    mId1 = dataEventInfo.mId1;
    mId2 = dataEventInfo.mId2;
    mId3 = dataEventInfo.mId3;
    mFlags = dataEventInfo.mFlags;
    mEventData = dataEventInfo.mEventData;

    nextItem = nullptr;
    prevItem = nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





EventInfo::EventInfo(time_t serverTime,EventId eventId,uint eventType,UInt64 id1,UInt64 id2,UInt64 id3,UInt64 flags,const char *eventData)
{
  try
  {
    mEventId = eventId;
    mEventTime = time(nullptr);
    mServerTime = serverTime;
    mType = eventType;
    mId1 = id1;
    mId2 = id2;
    mId3 = id3;
    mFlags = flags;
    mEventData = eventData;

    nextItem = nullptr;
    prevItem = nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





EventInfo::EventInfo(const char *csv)
{
  try
  {
    mEventId = 0;
    mEventTime = time(nullptr);
    mServerTime = 0;
    mType = ContentServer::EventType::UNKNOWN;
    mId1 = 0;
    mId2 = 0;
    mId3 = 0;
    mFlags = 0;
    nextItem = nullptr;
    prevItem = nullptr;
    setCsv(csv);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




EventInfo::~EventInfo()
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





EventInfo& EventInfo::operator=(const EventInfo& dataEventInfo)
{
  try
  {
    if (&dataEventInfo == this)
      return *this;

    mEventId = dataEventInfo.mEventId;
    mEventTime = dataEventInfo.mEventTime;
    mServerTime = dataEventInfo.mServerTime;
    mType = dataEventInfo.mType;
    mId1 = dataEventInfo.mId1;
    mId2 = dataEventInfo.mId2;
    mId3 = dataEventInfo.mId3;
    mFlags = dataEventInfo.mFlags;
    mEventData = dataEventInfo.mEventData;

    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::string EventInfo::getCsv()
{
  try
  {
    char st[1000];
    sprintf(st,"%lu;%lu;%lu;%u;%lu;%lu;%lu;%lu;%s",
        mEventId,
        mEventTime,
        mServerTime,
        mType,
        mId1,
        mId2,
        mId3,
        mFlags,
        mEventData.c_str());

    return std::string(st);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::string EventInfo::getCsvHeader()
{
  try
  {
    std::string header = "eventId;eventTime;serverTime;type;id1;id2;id3;flags;note";
    return header;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void EventInfo::setCsv(const char *csv)
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
      mEventId = toUInt64(field[0]);
      mEventTime = (time_t)toInt64(field[1]);
      mServerTime = (time_t)toInt64(field[2]);
      mType = toUInt32(field[3]);
      mId1 = toUInt64(field[4]);
      mId2 = toUInt64(field[5]);
      mId3 = toUInt64(field[6]);
      mFlags = toUInt64(field[7]);
      mEventData = field[8];
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void EventInfo::setCsv(const std::string& csv)
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





EventInfo* EventInfo::duplicate()
{
  try
  {
    return new EventInfo(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void EventInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "EventInfo\n";
    stream << space(level) << "- mEventId      = " << mEventId << "\n";
    stream << space(level) << "- mEventTime    = " << C_INT64(mEventTime) << "\n";
    stream << space(level) << "- mServerTime   = " << C_INT64(mServerTime) << "\n";
    stream << space(level) << "- mType         = " << mType << "\n";
    stream << space(level) << "- mId1          = " << mId1 << "\n";
    stream << space(level) << "- mId2          = " << mId2 << "\n";
    stream << space(level) << "- mId3          = " << mId3 << "\n";
    stream << space(level) << "- mFlags        = " << mFlags << "\n";
    stream << space(level) << "- mEventData    = " << mEventData << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



}
}
