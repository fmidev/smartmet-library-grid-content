#pragma once

#include "GenerationInfo.h"
#include "grid-files/common/ThreadLock.h"
#include <vector>


namespace SmartMet
{
namespace T
{


class GenerationInfoList
{
  public:
                      GenerationInfoList();
                      GenerationInfoList(GenerationInfoList& generationInfoList);
     virtual          ~GenerationInfoList();

     void             operator=(GenerationInfoList& generationInfoList);

     void             addGenerationInfo(GenerationInfo *generationInfo);
     void             clear();
     bool             deleteGenerationInfoById(uint generationId);
     void             deleteGenerationInfoListByProducerId(uint producerId);
     void             deleteGenerationInfoListBySourceId(uint producerId);
     GenerationInfo*  getGenerationInfoById(uint generationId);
     GenerationInfo*  getGenerationInfoByIndex(uint index);
     GenerationInfo*  getGenerationInfoByIndexNoCheck(uint index);
     GenerationInfo*  getGenerationInfoByName(std::string generationName);
     void             getGenerationInfoListByProducerId(uint producerId,GenerationInfoList& generationInfoList);
     void             getGenerationInfoListBySourceId(uint sourceId,GenerationInfoList& generationInfoList);
     GenerationInfo*  getLastGenerationInfoByProducerId(uint producerId);
     GenerationInfo*  getLastGenerationInfoByProducerIdAndStatus(uint producerId,T::GenerationStatus generationStatus);
     uint             getLength();
     void             lock();
     void             unlock();
     void             print(std::ostream& stream,uint level,uint optionFlags);

     void             writeToFile(std::string filename);

  protected:

     ThreadLock       mThreadLock;

     std::vector<GenerationInfo*>  mList;
};



}
}
