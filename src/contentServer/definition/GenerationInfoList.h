#pragma once

#include "GenerationInfo.h"

#include <grid-files/common/ModificationLock.h>
#include <vector>
#include <set>

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
    bool                deleteGenerationInfoById(T::GenerationId generationId);
    uint                deleteGenerationInfoListByProducerId(T::ProducerId producerId);
    uint                deleteGenerationInfoListBySourceId(T::SourceId sourceId);
    uint                deleteGenerationInfoListByStorageId(T::StorageId storageId);
    uint                deleteMarkedGenerations();
    void                getAnalysisTimes(std::vector<std::string>& analysisTimes,bool reverseOrder);
    int                 getClosestIndex(uint comparisonMethod,GenerationInfo& generationInfo);
    int                 getClosestIndexNoLock(uint comparisonMethod,GenerationInfo& generationInfo);
    void                getGenerationIdListByStatus(uchar generationStatus,std::set<T::GenerationId>& idList);
    GenerationInfo*     getGenerationInfoById(T::GenerationId generationId);
    bool                getGenerationInfoById(T::GenerationId generationId,GenerationInfo& generationInfo);
    GenerationInfo*     getGenerationInfoByIndex(uint index);
    GenerationInfo*     getGenerationInfoByIndexNoCheck(uint index);
    GenerationInfo*     getGenerationInfoByName(const std::string& generationName);
    bool                getGenerationInfoByName(const std::string& generationName,GenerationInfo& generationInfo);
    GenerationInfo*     getGenerationInfoByAnalysisTime(const std::string& analysisTime);
    GenerationInfo*     getGenerationInfoByAnalysisTime(T::GenerationId generationId,const std::string& analysisTime);
    int                 getGenerationInfoIndexByAnalysisTime(const std::string& analysisTime);
    int                 getGenerationInfoIndexByAnalysisTime(T::GenerationId generationId,const std::string& analysisTime);
    int                 getGenerationInfoIndexByGenerationId(T::GenerationId generationId);
    void                getGenerationInfoListByAnalysisTime(const std::string& analysisTime,GenerationInfoList& generationInfoList);
    void                getGenerationInfoListByProducerId(T::ProducerId producerId,GenerationInfoList& generationInfoList);
    bool                getGenerationInfoByProducerIdAndAnalysisTime(T::ProducerId producerId,const std::string& analysisTime,GenerationInfo& generationInfo);
    void                getGenerationInfoListByProducerIdAndStatus(T::ProducerId producerId,GenerationInfoList& generationInfoList,uchar generationStatus);
    void                getGenerationInfoListBySourceId(T::SourceId sourceId,GenerationInfoList& generationInfoList);
    GenerationInfo*     getPrevGenerationInfoByProducerId(T::ProducerId producerId,const std::string& nextGenerationName);
    GenerationInfo*     getFirstGenerationInfoByAnalysisTime();
    GenerationInfo*     getFirstGenerationInfoByAnalysisTime(uchar generationStatus);
    GenerationInfo*     getLastGenerationInfoByAnalysisTime();
    GenerationInfo*     getLastGenerationInfoByAnalysisTime(uchar generationStatus);
    GenerationInfo*     getLastGenerationInfoByProducerId(T::ProducerId producerId);
    GenerationInfo*     getLastGenerationInfoByProducerIdAndStatus(T::ProducerId producerId,uchar generationStatus);
    bool                getLastGenerationInfoByProducerIdAndStatus(T::ProducerId producerId,uchar generationStatus,T::GenerationInfo& generationInfo);
    GenerationInfo*     getLastGenerationInfoByContentTime(T::ProducerId producerId);
    GenerationInfo*     getLastGenerationInfoByContentTimeAndStatus(T::ProducerId producerId,uchar generationStatus);
    uint                getLength();
    std::size_t         getHash();
    std::size_t         getHashByProducerId(T::ProducerId producerId);
    ModificationLock*   getModificationLockPtr();
    bool                getReleaseObjects();
    uint                getSize() const;
    uint                markDeleted();
    uint                markDeletedById(T::GenerationId generationId);
    uint                markDeletedByProducerId(T::ProducerId producerId);
    uint                markDeletedBySourceId(T::SourceId sourceId);
    uint                markDeletedByStorageId(T::StorageId storageId);

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
