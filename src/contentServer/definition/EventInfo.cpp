#include "EventInfo.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{



/*! \brief Default constructor for EventInfo. */

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





/*! \brief Copy constructor for EventInfo. */

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





/*! \brief Construct EventInfo from the given event fields. */

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





/*! \brief Construct EventInfo from a CSV-formatted string. */

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




/*! \brief Destructor for EventInfo. */

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





/*! \brief Copy assignment operator for EventInfo. */

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





/*! \brief Return this event formatted as a CSV row. */

std::string EventInfo::getCsv()
{
  try
  {
    return
      std::to_string(mEventId) + ";" +
      std::to_string(mEventTime) + ";" +
      std::to_string(mServerTime) + ";" +
      std::to_string(mType) + ";" +
      std::to_string(mId1) + ";" +
      std::to_string(mId2) + ";" +
      std::to_string(mId3) + ";" +
      std::to_string(mFlags) + ";" +
      mEventData;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return the CSV header row describing the fields. */

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





/*! \brief Populate this event from a CSV row C string. */

void EventInfo::setCsv(const char *csv)
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





/*! \brief Populate this event from a CSV row std::string. */

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





/*! \brief Return a deep copy of this EventInfo. */

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





/*! \brief Print the event contents to the given stream. */

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
