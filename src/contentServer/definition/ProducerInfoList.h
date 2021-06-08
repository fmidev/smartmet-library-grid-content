#pragma once

#include "ProducerInfo.h"

#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <vector>


namespace SmartMet
{
namespace T
{

typedef std::vector<ProducerInfo*> ProducerInfo_pvec;


class ProducerInfoList
{
  public:
                      ProducerInfoList();
                      ProducerInfoList(ProducerInfoList& producerInfoList);
    virtual           ~ProducerInfoList();

    ProducerInfoList& operator=(ProducerInfoList& producerInfoList);

    void              addProducerInfo(ProducerInfo *producerInfo);
    void              clear();
    bool              deleteProducerInfoById(uint producerId);
    uint              deleteProducerInfoListBySourceId(uint sourceId);
    uint              deleteMarkedProducers();
    ProducerInfo*     getProducerInfoById(uint producerId);
    bool              getProducerInfoById(uint producerId,ProducerInfo& producerInfo);
    ProducerInfo*     getProducerInfoByName(const std::string& producerName);
    bool              getProducerInfoByName(const std::string& producerName,ProducerInfo& producerInfo);
    ProducerInfo*     getProducerInfoByIndex(uint index);
    ProducerInfo*     getProducerInfoByIndexNoCheck(uint index);
    void              getProducerInfoListBySourceId(uint sourceId,ProducerInfoList& producerInfoList);
    uint              getLength();
    std::size_t       getHash();
    void              setLockingEnabled(bool lockingEnabled);
    void              sortByName();
    uint              markDeletedById(uint producerId);
    uint              markDeletedBySourceId(uint sourceId);
    void              lock();
    void              unlock();
    void              print(std::ostream& stream,uint level,uint optionFlags);
    void              writeToFile(const std::string& filename);

  protected:

    ModificationLock  mModificationLock;
    ProducerInfo_pvec mList;
};



}
}
