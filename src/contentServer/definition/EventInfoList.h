#pragma once

#include "EventInfo.h"

#include <grid-files/common/ThreadLock.h>
#include <list>


namespace SmartMet
{
namespace T
{

// ====================================================================================
/*! \brief Thread-safe doubly-linked list of EventInfo records with optional size cap.
 *
 *  EventInfoList maintains a bounded FIFO of Content Server change events using the
 *  intrusive next/prev pointers of EventInfo. Consumers can retrieve events by
 *  starting event id, enabling incremental polling by cache implementations. */
// ====================================================================================

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
    void            getEventInfoList(T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList);
    EventInfo*      getEventInfo(T::EventId  eventId);
    uint            getLength();
    uint            getMaxLength();
    void            lock();
    void            unlock();
    void            setMaxLength(uint maxLength);
    void            print(std::ostream& stream,uint level,uint optionFlags);
    void            writeToFile(const std::string& filename);

  protected:

    EventInfo       *firstItem;   //!< Pointer to the oldest event in the list.
    EventInfo       *lastItem;    //!< Pointer to the most recent event in the list.
    uint            mLength;      //!< Current number of events stored.
    uint            mMaxLength;   //!< Maximum allowed list length (0 = unlimited); oldest events are dropped when exceeded.
    ThreadLock      mThreadLock;  //!< Mutex protecting all list operations.
};



}
}
