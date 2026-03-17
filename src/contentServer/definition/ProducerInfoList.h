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
    bool              deleteProducerInfoById(T::ProducerId producerId);
    uint              deleteProducerInfoListBySourceId(T::SourceId sourceId);
    uint              deleteProducerInfoListByStorageId(T::StorageId storageId);
    uint              deleteMarkedProducers();
    ProducerInfo*     getProducerInfoById(T::ProducerId producerId);
    bool              getProducerInfoById(T::ProducerId producerId,ProducerInfo& producerInfo);
    ProducerInfo*     getProducerInfoByName(const std::string& producerName);
    bool              getProducerInfoByName(const std::string& producerName,ProducerInfo& producerInfo);
    ProducerInfo*     getProducerInfoByIndex(uint index);
    ProducerInfo*     getProducerInfoByIndexNoCheck(uint index);
    void              getProducerInfoListBySourceId(T::SourceId sourceId,ProducerInfoList& producerInfoList);
    uint              getLength();
    std::size_t       getHash();
    void              setLockingEnabled(bool lockingEnabled);
    void              sortByName();
    uint              markDeletedById(T::ProducerId producerId);
    uint              markDeletedBySourceId(T::SourceId sourceId);
    uint              markDeletedByStorageId(T::StorageId storageId);
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
