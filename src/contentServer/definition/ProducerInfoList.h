#pragma once

#include "ProducerInfo.h"

#include <grid-files/common/ThreadLock.h>
#include <vector>


namespace SmartMet
{
namespace T
{


class ProducerInfoList
{
  public:
                    ProducerInfoList();
                    ProducerInfoList(ProducerInfoList& producerInfoList);
     virtual        ~ProducerInfoList();

     void           operator=(ProducerInfoList& producerInfoList);

     void           addProducerInfo(ProducerInfo *producerInfo);
     void           clear();
     bool           deleteProducerInfoById(uint producerId);
     void           deleteProducerInfoListBySourceId(uint sourceId);
     ProducerInfo*  getProducerInfoById(uint producerId);
     ProducerInfo*  getProducerInfoByName(std::string producerName);
     ProducerInfo*  getProducerInfoByIndex(uint index);
     ProducerInfo*  getProducerInfoByIndexNoCheck(uint index);
     void           getProducerInfoListBySourceId(uint sourceId,ProducerInfoList& producerInfoList);
     uint           getLength();
     void           sortByName();
     void           lock();
     void           unlock();
     void           print(std::ostream& stream,uint level,uint optionFlags);

     void           writeToFile(std::string filename);

  protected:

     ThreadLock     mThreadLock;

     std::vector<ProducerInfo*>  mList;
};



}
}
