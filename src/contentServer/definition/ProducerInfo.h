#pragma once

#include <string>
#include <grid-files/grid/Typedefs.h>



namespace SmartMet
{
namespace T
{



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

    T::ProducerId   mProducerId;
    std::string     mName;
    std::string     mTitle;
    std::string     mDescription;
    uint            mFlags;
    T::StorageId    mStorageId;
    T::SourceId     mSourceId;
    uchar           mStatus;

    UInt64          mHash;

  public:

    class Flags
    {
      public:
        static const uint DeletedProducer  = 4;
    };

};



}
}
