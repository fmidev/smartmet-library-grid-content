#pragma once

#include <string>
#include <grid-files/grid/Typedefs.h>



namespace SmartMet
{
namespace T
{

// ====================================================================================
/*! \brief Metadata record describing a data producer registered in the Content Server.
 *
 *  A producer represents an organisation or model system that generates grid data.
 *  All generations (model runs) and their associated files and content records are
 *  owned by a producer. */
// ====================================================================================

class ProducerInfo
{
  public:
                    ProducerInfo();
                    ProducerInfo(const ProducerInfo& producerInfo);
                    ProducerInfo(const char *csv);
    virtual         ~ProducerInfo();

    std::string     getCsv();
    std::string     getCsvHeader();
    void            setCsv(const char *csv);
    void            setCsv(const std::string& csv);

    ProducerInfo&   operator=(const ProducerInfo& producerInfo);
    ProducerInfo*   duplicate();
    void            print(std::ostream& stream,uint level,uint optionFlags);

    T::ProducerId   mProducerId;    //!< Unique numeric identifier of the producer.
    std::string     mName;          //!< Short machine-readable name of the producer (e.g. "ECMWF").
    std::string     mTitle;         //!< Human-readable title of the producer.
    std::string     mDescription;   //!< Longer description of the producer and its data.
    uint            mFlags;         //!< Bitmask flags (see ProducerInfo::Flags constants).
    T::StorageId    mStorageId;     //!< Storage partition identifier.
    T::SourceId     mSourceId;      //!< Source system from which this producer record was ingested.
    uchar           mStatus;        //!< Lifecycle status of the producer.

    UInt64          mHash;          //!< Hash used for change detection across synchronised instances.

  public:

    /*! \brief Bitmask flags for producer lifecycle management. */
    class Flags
    {
      public:
        static const uint DeletedProducer  = 4; //!< Producer record is logically deleted.
    };

};



}
}
