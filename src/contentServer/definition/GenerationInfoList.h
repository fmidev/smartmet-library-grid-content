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
    uint                deleteGenerationInfoListByProducerId(uint producerId);
    uint                deleteGenerationInfoListBySourceId(uint producerId);
    uint                deleteMarkedGenerations();
    void                getAnalysisTimes(std::vector<std::string>& analysisTimes,bool reverseOrder);
    int                 getClosestIndex(uint comparisonMethod,GenerationInfo& generationInfo);
    int                 getClosestIndexNoLock(uint comparisonMethod,GenerationInfo& generationInfo);
    GenerationInfo*     getGenerationInfoById(uint generationId);
    bool                getGenerationInfoById(uint generationId,GenerationInfo& generationInfo);
    GenerationInfo*     getGenerationInfoByIndex(uint index);
    GenerationInfo*     getGenerationInfoByIndexNoCheck(uint index);
    GenerationInfo*     getGenerationInfoByName(const std::string& generationName);
    bool                getGenerationInfoByName(const std::string& generationName,GenerationInfo& generationInfo);
    GenerationInfo*     getGenerationInfoByAnalysisTime(const std::string& analysisTime);
    GenerationInfo*     getGenerationInfoByAnalysisTime(uint generationId,const std::string& analysisTime);
    int                 getGenerationInfoIndexByAnalysisTime(const std::string& analysisTime);
    int                 getGenerationInfoIndexByAnalysisTime(uint generationId,const std::string& analysisTime);
    int                 getGenerationInfoIndexByGenerationId(uint generationId);
    void                getGenerationInfoListByAnalysisTime(const std::string& analysisTime,GenerationInfoList& generationInfoList);
    void                getGenerationInfoListByProducerId(uint producerId,GenerationInfoList& generationInfoList);
    void                getGenerationInfoListByProducerIdAndStatus(uint producerId,GenerationInfoList& generationInfoList,uchar generationStatus);
    void                getGenerationInfoListBySourceId(uint sourceId,GenerationInfoList& generationInfoList);
    GenerationInfo*     getPrevGenerationInfoByProducerId(uint producerId,const std::string& nextGenerationName);
    GenerationInfo*     getFirstGenerationInfoByAnalysisTime();
    GenerationInfo*     getFirstGenerationInfoByAnalysisTime(uchar generationStatus);
    GenerationInfo*     getLastGenerationInfoByAnalysisTime();
    GenerationInfo*     getLastGenerationInfoByAnalysisTime(uchar generationStatus);
    GenerationInfo*     getLastGenerationInfoByProducerId(uint producerId);
    GenerationInfo*     getLastGenerationInfoByProducerIdAndStatus(uint producerId,uchar generationStatus);
    bool                getLastGenerationInfoByProducerIdAndStatus(uint producerId,uchar generationStatus,T::GenerationInfo& generationInfo);
    GenerationInfo*     getLastGenerationInfoByContentTime(uint producerId);
    GenerationInfo*     getLastGenerationInfoByContentTimeAndStatus(uint producerId,uchar generationStatus);
    uint                getLength();
    std::size_t         getHash();
    std::size_t         getHashByProducerId(uint producerId);
    ModificationLock*   getModificationLockPtr();
    bool                getReleaseObjects();
    uint                getSize() const;
    uint                markDeleted();
    uint                markDeletedById(uint generationId);
    uint                markDeletedByProducerId(uint producerId);
    uint                markDeletedBySourceId(uint producerId);

    void                setLockingEnabled(bool lockingEnabled);
    void                setModificationLockPtr(ModificationLock* modificationLockPtr);
    void                setReleaseObjects(bool releaseObjects);
    void                lock();
    void                unlock();
    void                setComparisonMethod(uint comparisonMethod);
    void                sort(uint comparisonMethod);
    void                increaseSize(uint newSize);
    void                print(std::ostream& stream,uint level,uint optionFlags);
    void                writeToFile(const std::string& filename);



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
