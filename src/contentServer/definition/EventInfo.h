#pragma once

#include <string>


namespace SmartMet
{

namespace ContentServer
{

enum class EventType
{
  UNKNOWN                                 = 0,
  CLEAR                                   = 1,
  CONTENT_SERVER_RELOAD                   = 2,
  PRODUCER_ADDED                          = 10,
  PRODUCER_DELETED                        = 11,
  GENERATION_ADDED                        = 20,
  GENERATION_DELETED                      = 21,
  GENERATION_STATUS_CHANGED               = 22,
  GENERATION_LIST_DELETED_BY_PRODUCER_ID  = 23,
  FILE_ADDED                              = 30,
  FILE_DELETED                            = 31,
  FILE_UPDATED                            = 32,
  FILE_LIST_DELETED_BY_GROUP_FLAGS        = 33,
  FILE_LIST_DELETED_BY_PRODUCER_ID        = 34,
  FILE_LIST_DELETED_BY_GENERATION_ID      = 35,
  CONTENT_LIST_DELETED_BY_GROUP_FLAGS     = 36,
  CONTENT_LIST_DELETED_BY_PRODUCER_ID     = 37,
  CONTENT_LIST_DELETED_BY_GENERATION_ID   = 38,
  CONTENT_LIST_DELETED_BY_FILE_ID         = 39,
  DATA_SERVER_ADDED                       = 40,
  DATA_SERVER_DELETED                     = 41,
  CONTENT_ADDED                           = 60,
  CONTENT_DELETED                         = 61,
  CONTENT_REGISTERED                      = 62
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
                              EventInfo(time_t serverTime,EventId eventId,ContentServer::EventType eventType,uint id1,uint id2,uint id3,unsigned long long flags);
                              EventInfo(const char *csv);
     virtual                  ~EventInfo();

     std::string              getCsv();
     std::string              getCsvHeader();
     void                     setCsv(const char *csv);
     void                     setCsv(std::string csv);

     void                     operator=(const EventInfo& dataEventInfo);
     EventInfo*               duplicate();
     void                     print(std::ostream& stream,uint level,uint optionFlags);

     EventId                  mEventId;
     time_t                   mEventTime;
     time_t                   mServerTime;
     ContentServer::EventType mType;
     uint                     mId1;
     uint                     mId2;
     uint                     mId3;
     unsigned long long       mFlags;
     std::string              mNote;

     EventInfo*               nextItem;
     EventInfo*               prevItem;
};


}
}
