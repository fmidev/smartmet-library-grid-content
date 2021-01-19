#pragma once

#include "EventInfo.h"

#include <grid-files/common/ThreadLock.h>
#include <list>


namespace SmartMet
{
namespace T
{


class EventInfoList
{
  public:
                     EventInfoList();
                     EventInfoList(EventInfoList& eventInfoList);
    virtual         ~EventInfoList();

    EventInfoList&  operator=(EventInfoList& eventInfoList);

    void            addEventInfo(EventInfo* dataEventInfo);
    void            addEventInfoToBegin(EventInfo *dataEventInfo);
    void            deleteFirstEvent();
    void            deleteLastEvent();
    void            clear();
    EventInfo*      getFirstEvent();
    EventInfo*      getLastEvent();
    void            getEventInfoList(T::EventId startEventId,uint maxRecords,T::EventInfoList& eventInfoList);
    EventInfo*      getEventInfo(unsigned long long eventId);
    uint            getLength();
    uint            getMaxLength();
    void            lock();
    void            unlock();
    void            setMaxLength(uint maxLength);
    void            print(std::ostream& stream,uint level,uint optionFlags);
    void            writeToFile(const std::string& filename);

  protected:

    EventInfo       *firstItem;
    EventInfo       *lastItem;
    uint            mLength;
    uint            mMaxLength;
    ThreadLock      mThreadLock;
};



}
}
