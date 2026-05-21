#pragma once

#include <grid-files/grid/Typedefs.h>
#include <string>


namespace SmartMet
{

namespace ContentServer
{

// ====================================================================================
/*! \brief Constants identifying the type of a Content Server change event. */
// ====================================================================================

class EventType
{
  public:
    static const uint UNKNOWN                                 = 0;   //!< Unrecognised event type.
    static const uint CLEAR                                   = 1;   //!< All content cleared.
    static const uint CONTENT_SERVER_RELOAD                   = 2;   //!< Content server reloaded from persistent storage.
    static const uint UPDATE_LOOP_START                       = 3;   //!< Synchronisation update loop started.
    static const uint UPDATE_LOOP_END                         = 4;   //!< Synchronisation update loop finished.
    static const uint STORAGE_CLEAR                           = 5;   //!< In-memory storage cleared.

    static const uint PRODUCER_ADDED                          = 10;  //!< A producer record was added.
    static const uint PRODUCER_DELETED                        = 11;  //!< A producer record was deleted.
    static const uint PRODUCER_UPDATED                        = 13;  //!< A producer record was updated.
    static const uint PRODUCER_LIST_DELETED_BY_SOURCE_ID      = 12;  //!< All producers for a source id were deleted.

    static const uint GENERATION_ADDED                        = 20;  //!< A generation record was added.
    static const uint GENERATION_DELETED                      = 21;  //!< A generation record was deleted.
    static const uint GENERATION_UPDATED                      = 25;  //!< A generation record was updated.
    static const uint GENERATION_STATUS_CHANGED               = 22;  //!< The status of a generation changed.
    static const uint GENERATION_LIST_DELETED_BY_PRODUCER_ID  = 23;  //!< All generations for a producer were deleted.
    static const uint GENERATION_LIST_DELETED_BY_SOURCE_ID    = 24;  //!< All generations for a source id were deleted.

    static const uint GEOMETRY_ADDED                          = 30;  //!< A geometry record was added.
    static const uint GEOMETRY_DELETED                        = 31;  //!< A geometry record was deleted.
    static const uint GEOMETRY_UPDATED                        = 32;  //!< A geometry record was updated.
    static const uint GEOMETRY_STATUS_CHANGED                 = 33;  //!< The status of a geometry changed.
    static const uint GEOMETRY_LIST_DELETED_BY_PRODUCER_ID    = 34;  //!< All geometries for a producer were deleted.
    static const uint GEOMETRY_LIST_DELETED_BY_GENERATION_ID  = 35;  //!< All geometries for a generation were deleted.
    static const uint GEOMETRY_LIST_DELETED_BY_SOURCE_ID      = 36;  //!< All geometries for a source id were deleted.

    static const uint FILE_ADDED                              = 40;  //!< A file record was added.
    static const uint FILE_DELETED                            = 41;  //!< A file record was deleted.
    static const uint FILE_UPDATED                            = 42;  //!< A file record was updated.
    static const uint FILE_LIST_DELETED_BY_PRODUCER_ID        = 43;  //!< All files for a producer were deleted.
    static const uint FILE_LIST_DELETED_BY_GENERATION_ID      = 44;  //!< All files for a generation were deleted.
    static const uint FILE_LIST_DELETED_BY_GEOMETRY           = 45;  //!< All files for a geometry were deleted.
    static const uint FILE_LIST_DELETED_BY_SOURCE_ID          = 46;  //!< All files for a source id were deleted.

    static const uint CONTENT_ADDED                           = 50;  //!< A content record was added.
    static const uint CONTENT_DELETED                         = 51;  //!< A content record was deleted.
    static const uint CONTENT_UPDATED                         = 52;  //!< A content record was updated.
    static const uint CONTENT_LIST_DELETED_BY_PRODUCER_ID     = 53;  //!< All content for a producer was deleted.
    static const uint CONTENT_LIST_DELETED_BY_GENERATION_ID   = 54;  //!< All content for a generation was deleted.
    static const uint CONTENT_LIST_DELETED_BY_GEOMETRY        = 55;  //!< All content for a geometry was deleted.
    static const uint CONTENT_LIST_DELETED_BY_FILE_ID         = 56;  //!< All content for a file id was deleted.
    static const uint CONTENT_LIST_DELETED_BY_SOURCE_ID       = 57;  //!< All content for a source id was deleted.

    static const uint DELETE_VIRTUAL_CONTENT                  = 60;  //!< Virtual (derived) content records were deleted.
    static const uint UPDATE_VIRTUAL_CONTENT                  = 61;  //!< Virtual (derived) content records were updated.
};

}


namespace T
{

// ====================================================================================
/*! \brief Record describing a single change event emitted by the Content Server.
 *
 *  EventInfo objects are produced whenever producer, generation, geometry, file, or
 *  content records are added, updated, or deleted. Cache implementations subscribe to
 *  the event stream to stay synchronised with the master Content Server. */
// ====================================================================================

class EventInfo
{
  public:
                   EventInfo();
                   EventInfo(const EventInfo& dataEventInfo);
                   EventInfo(time_t serverTime,EventId eventId,uint eventType,UInt64 id1,UInt64 id2,UInt64 id3,UInt64 flags,const char *eventData);
                   EventInfo(const char *csv);
    virtual        ~EventInfo();

    EventInfo&     operator=(const EventInfo& dataEventInfo);

    std::string    getCsv();
    std::string    getCsvHeader();
    void           setCsv(const char *csv);
    void           setCsv(const std::string& csv);
    EventInfo*     duplicate();
    void           print(std::ostream& stream,uint level,uint optionFlags);

    EventId        mEventId;    //!< Monotonically increasing identifier assigned by the Content Server.
    time_t         mEventTime;  //!< Wall-clock time when the event was recorded (UTC Unix timestamp).
    time_t         mServerTime; //!< Server-side time supplied at construction (may differ from mEventTime).
    uint           mType;       //!< Event type code (one of ContentServer::EventType constants).
    UInt64         mId1;        //!< Primary entity identifier relevant to the event (e.g. producer id).
    UInt64         mId2;        //!< Secondary entity identifier (e.g. generation id).
    UInt64         mId3;        //!< Tertiary entity identifier (e.g. geometry id).
    UInt64         mFlags;      //!< Event-specific flag bits.
    std::string    mEventData;  //!< Optional serialised payload associated with the event.

    EventInfo*     nextItem;    //!< Intrusive linked-list forward pointer (used by EventInfoList).
    EventInfo*     prevItem;    //!< Intrusive linked-list backward pointer (used by EventInfoList).
};


}
}
