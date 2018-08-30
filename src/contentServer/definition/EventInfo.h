#pragma once

#include <grid-files/grid/Typedefs.h>
#include <string>


namespace SmartMet
{

namespace ContentServer
{

class EventType
{
  public:
    static const uint UNKNOWN                                 = 0;
    static const uint CLEAR                                   = 1;
    static const uint CONTENT_SERVER_RELOAD                   = 2;
    static const uint PRODUCER_ADDED                          = 10;
    static const uint PRODUCER_DELETED                        = 11;
    static const uint PRODUCER_LIST_DELETED_BY_SOURCE_ID      = 12;
    static const uint GENERATION_ADDED                        = 20;
    static const uint GENERATION_DELETED                      = 21;
    static const uint GENERATION_STATUS_CHANGED               = 22;
    static const uint GENERATION_LIST_DELETED_BY_PRODUCER_ID  = 23;
    static const uint GENERATION_LIST_DELETED_BY_SOURCE_ID    = 24;
    static const uint FILE_ADDED                              = 30;
    static const uint FILE_DELETED                            = 31;
    static const uint FILE_UPDATED                            = 32;
    static const uint FILE_LIST_DELETED_BY_GROUP_FLAGS        = 33;
    static const uint FILE_LIST_DELETED_BY_PRODUCER_ID        = 34;
    static const uint FILE_LIST_DELETED_BY_GENERATION_ID      = 35;
    static const uint FILE_LIST_DELETED_BY_SOURCE_ID          = 36;
    static const uint CONTENT_ADDED                           = 40;
    static const uint CONTENT_DELETED                         = 41;
    static const uint CONTENT_REGISTERED                      = 42;
    static const uint CONTENT_LIST_DELETED_BY_GROUP_FLAGS     = 43;
    static const uint CONTENT_LIST_DELETED_BY_PRODUCER_ID     = 44;
    static const uint CONTENT_LIST_DELETED_BY_GENERATION_ID   = 45;
    static const uint CONTENT_LIST_DELETED_BY_FILE_ID         = 46;
    static const uint CONTENT_LIST_DELETED_BY_SOURCE_ID       = 47;
    static const uint DATA_SERVER_ADDED                       = 50;
    static const uint DATA_SERVER_DELETED                     = 51;
    static const uint DELETE_VIRTUAL_CONTENT                  = 60;
    static const uint UPDATE_VIRTUAL_CONTENT                  = 61;
};

}


namespace T
{

typedef unsigned long long EventId;


class EventInfo
{
  public:
                   EventInfo();
                   EventInfo(const EventInfo& dataEventInfo);
                   EventInfo(time_t serverTime,EventId eventId,uint eventType,uint id1,uint id2,uint id3,unsigned long long flags);
                   EventInfo(const char *csv);
    virtual        ~EventInfo();

    EventInfo&     operator=(const EventInfo& dataEventInfo);

    std::string    getCsv();
    std::string    getCsvHeader();
    void           setCsv(const char *csv);
    void           setCsv(std::string csv);
    EventInfo*     duplicate();
    void           print(std::ostream& stream,uint level,uint optionFlags);

    EventId        mEventId;
    time_t         mEventTime;
    time_t         mServerTime;
    uint           mType;
    uint           mId1;
    uint           mId2;
    uint           mId3;
    ulonglong      mFlags;
    std::string    mNote;

    EventInfo*     nextItem;
    EventInfo*     prevItem;
};


}
}
