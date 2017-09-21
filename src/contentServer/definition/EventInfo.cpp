#include "EventInfo.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"


namespace SmartMet
{
namespace T
{



EventInfo::EventInfo()
{
  try
  {
    mEventId = 0;
    mEventTime = time(0);
    mServerTime = 0;
    mType = ContentServer::EventType::UNKNOWN;
    mId1 = 0;
    mId2 = 0;
    mId3 = 0;
    mFlags = 0;

    nextItem = NULL;
    prevItem = NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    mNote = dataEventInfo.mNote;

    nextItem = NULL;
    prevItem = NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





EventInfo::EventInfo(time_t serverTime,EventId eventId,ContentServer::EventType eventType,uint id1,uint id2,uint id3,unsigned long long flags)
{
  try
  {
    mEventId = eventId;
    mEventTime = time(0);
    mServerTime = serverTime;
    mType = eventType;
    mId1 = id1;
    mId2 = id2;
    mId3 = id3;
    mFlags = flags;

    nextItem = NULL;
    prevItem = NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





EventInfo::EventInfo(const char *csv)
{
  try
  {
    setCsv(csv);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




EventInfo::~EventInfo()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void EventInfo::operator=(const EventInfo& dataEventInfo)
{
  try
  {
    if (&dataEventInfo == this)
      return;

    mEventId = dataEventInfo.mEventId;
    mEventTime = dataEventInfo.mEventTime;
    mServerTime = dataEventInfo.mServerTime;
    mType = dataEventInfo.mType;
    mId1 = dataEventInfo.mId1;
    mId2 = dataEventInfo.mId2;
    mId3 = dataEventInfo.mId3;
    mFlags = dataEventInfo.mFlags;
    mNote = dataEventInfo.mNote;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





std::string EventInfo::getCsv()
{
  try
  {
    char st[1000];
    sprintf(st,"%llu;%u;%u;%u;%u;%u;%u;%llu;%s",
        mEventId,
        (uint)mEventTime,
        (uint)mServerTime,
        (uint)mType,
        mId1,
        mId2,
        mId3,
        mFlags,
        mNote.c_str());

    return std::string(st);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
      mEventId = (EventId)atoll(field[0]);
      mEventTime = (uint)atoll(field[1]);
      mServerTime = (uint)atoll(field[2]);
      mType = (ContentServer::EventType)atoll(field[3]);
      mId1 = (uint)atoll(field[4]);
      mId2 = (uint)atoll(field[5]);
      mId3 = (uint)atoll(field[6]);
      mFlags = (unsigned long long)atoll(field[7]);
      mNote = field[8];
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void EventInfo::setCsv(std::string csv)
{
  try
  {
    setCsv(csv.c_str());
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void EventInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "EventInfo\n";
    stream << space(level) << "- mEventId      = " << mEventId << "\n";
    stream << space(level) << "- mEventTime    = " << (uint)mEventTime << "\n";
    stream << space(level) << "- mServerTime   = " << (uint)mServerTime << "\n";
    stream << space(level) << "- mType         = " << (int)mType << "\n";
    stream << space(level) << "- mId1          = " << mId1 << "\n";
    stream << space(level) << "- mId2          = " << mId2 << "\n";
    stream << space(level) << "- mId3          = " << mId3 << "\n";
    stream << space(level) << "- mFlags        = " << mFlags << "\n";
    stream << space(level) << "- mNote         = " << mNote << "\n";
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}



}
}
