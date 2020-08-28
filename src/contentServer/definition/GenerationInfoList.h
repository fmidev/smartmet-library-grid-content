#pragma once

#include "GenerationInfo.h"

#include <grid-files/common/ModificationLock.h>
#include <vector>


namespace SmartMet
{
namespace T
{


typedef std::vector<GenerationInfo*> GenerationInfo_pvec;


class GenerationInfoList
{
  public:
                        GenerationInfoList();
                        GenerationInfoList(GenerationInfoList& generationInfoList);
    virtual             ~GenerationInfoList();

    GenerationInfoList& operator=(GenerationInfoList& generationInfoList);

    GenerationInfo*     addGenerationInfo(GenerationInfo *generationInfo);
    void                clear();
    bool                deleteGenerationInfoById(uint generationId);
    void                deleteGenerationInfoListByProducerId(uint producerId);
    void                deleteGenerationInfoListBySourceId(uint producerId);
    void                getAnalysisTimes(std::vector<std::string>& analysisTimes,bool reverseOrder);
    int                 getClosestIndex(uint comparisonMethod,GenerationInfo& generationInfo);
    int                 getClosestIndexNoLock(uint comparisonMethod,GenerationInfo& generationInfo);
    GenerationInfo*     getGenerationInfoById(uint generationId);
    GenerationInfo*     getGenerationInfoByIndex(uint index);
    GenerationInfo*     getGenerationInfoByIndexNoCheck(uint index);
    GenerationInfo*     getGenerationInfoByName(std::string generationName);
    GenerationInfo*     getGenerationInfoByAnalysisTime(std::string analysisTime);
    int                 getGenerationInfoIndexByAnalysisTime(std::string analysisTime);
    int                 getGenerationInfoIndexByGenerationId(uint generationId);
    void                getGenerationInfoListByAnalysisTime(std::string analysisTime,GenerationInfoList& generationInfoList);
    void                getGenerationInfoListByProducerId(uint producerId,GenerationInfoList& generationInfoList);
    void                getGenerationInfoListByProducerIdAndStatus(uint producerId,GenerationInfoList& generationInfoList,uchar generationStatus);
    void                getGenerationInfoListBySourceId(uint sourceId,GenerationInfoList& generationInfoList);
    GenerationInfo*     getPrevGenerationInfoByProducerId(uint producerId,std::string nextGenerationName);
    GenerationInfo*     getLastGenerationInfoByAnalysisTime();
    GenerationInfo*     getLastGenerationInfoByProducerId(uint producerId);
    GenerationInfo*     getLastGenerationInfoByProducerIdAndStatus(uint producerId,uchar generationStatus);
    uint                getLength();
    std::size_t         getHashByProducerId(uint producerId);
    ModificationLock*   getModificationLockPtr();
    bool                getReleaseObjects();
    uint                getSize() const;
    void                setModificationLockPtr(ModificationLock* modificationLockPtr);
    void                setReleaseObjects(bool releaseObjects);
    void                lock();
    void                unlock();
    void                setComparisonMethod(uint comparisonMethod);
    void                sort(uint comparisonMethod);
    void                increaseSize(uint newSize);
    void                print(std::ostream& stream,uint level,uint optionFlags);
    void                writeToFile(std::string filename);



  protected:

    GenerationInfoPtr   *mArray;
    uint                mSize;
    uint                mLength;
    ModificationLock    mModificationLock;
    ModificationLock*   mModificationLockPtr;
    bool                mReleaseObjects;
    uint                mComparisonMethod;

};

typedef std::shared_ptr<GenerationInfoList> GenerationInfoList_sptr;


}
}
