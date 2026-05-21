#pragma once

#include "GeometryInfo.h"

#include <grid-files/common/ModificationLock.h>
#include <vector>


namespace SmartMet
{
namespace T
{


typedef std::vector<GeometryInfo*> GeometryInfo_pvec;  //!< Convenience vector of GeometryInfo pointers.

// ====================================================================================
/*! \brief Sorted, indexed container of GeometryInfo pointers with thread-safe access.
 *
 *  GeometryInfoList stores geometry-generation association records in a sorted
 *  heap-allocated array and supports lookup and enumeration by generation id,
 *  producer id, and source id. */
// ====================================================================================

class GeometryInfoList
{
  public:
                        GeometryInfoList();
                        GeometryInfoList(GeometryInfoList& geometryInfoList);
    virtual             ~GeometryInfoList();

    GeometryInfoList&   operator=(GeometryInfoList& geometryInfoList);

    GeometryInfo*       addGeometryInfo(GeometryInfo *geometryInfo);
    void                clear();
    bool                deleteGeometryInfoById(T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId);
    uint                deleteGeometryInfoListByProducerId(T::ProducerId producerId);
    uint                deleteGeometryInfoListByGenerationId(T::GenerationId generationId);
    uint                deleteGeometryInfoListByGenerationIdList(std::set<T::GenerationId>& generationIdList);
    uint                deleteGeometryInfoListBySourceId(T::SourceId sourceId);
    uint                deleteGeometryInfoListByStorageId(T::StorageId storageId);
    uint                deleteMarkedGeometries();
    int                 getClosestIndex(uint comparisonMethod,GeometryInfo& geometryInfo);
    int                 getClosestIndexNoLock(uint comparisonMethod,GeometryInfo& geometryInfo);
    GeometryInfo*       getGeometryInfoById(T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId);
    bool                getGeometryInfoById(T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,GeometryInfo& geometryInfo);
    GeometryInfo*       getGeometryInfoByIndex(uint index);
    GeometryInfo*       getGeometryInfoByIndexNoCheck(uint index);
    void                getGeometryInfoListByGenerationId(T::GenerationId generationId,GeometryInfoList& geometryInfoList);
    void                getGeometryInfoListByGenerationIdAndStatus(T::GenerationId generationId,GeometryInfoList& geometryInfoList,uchar geometryStatus);
    void                getGeometryInfoListByProducerId(T::ProducerId producerId,GeometryInfoList& geometryInfoList);
    void                getGeometryInfoListByProducerIdAndStatus(T::ProducerId producerId,GeometryInfoList& geometryInfoList,uchar geometryStatus);
    void                getGeometryInfoListBySourceId(T::SourceId sourceId,GeometryInfoList& geometryInfoList);
    uint                getLength();
    std::size_t         getHash();
    std::size_t         getHashByProducerId(T::ProducerId producerId);
    std::size_t         getHashByStorageId(T::StorageId storageId);
    ModificationLock*   getModificationLockPtr();
    bool                getReleaseObjects();
    uint                getSize() const;
    uint                markDeleted();
    uint                markDeletedById(T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId);
    uint                markDeletedByGenerationId(T::GenerationId generationId);
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

    GeometryInfoPtr     *mArray;             //!< Heap-allocated array of GeometryInfo pointers.
    uint                mSize;              //!< Allocated capacity of mArray.
    uint                mLength;            //!< Number of valid entries currently stored.
    ModificationLock    mModificationLock;  //!< Owned modification lock used when no external lock is set.
    ModificationLock*   mModificationLockPtr; //!< Pointer to the active modification lock (may be external).
    bool                mReleaseObjects;    //!< If true, destructor deletes the pointed-to GeometryInfo objects.
    uint                mComparisonMethod;  //!< Current sort order (one of GeometryInfo::ComparisonMethod constants).
};

typedef std::shared_ptr<GeometryInfoList> GeometryInfoList_sptr;  //!< Shared-ownership pointer to a GeometryInfoList.


}
}
