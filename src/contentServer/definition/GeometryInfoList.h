#pragma once

#include "GeometryInfo.h"

#include <grid-files/common/ModificationLock.h>
#include <vector>


namespace SmartMet
{
namespace T
{


typedef std::vector<GeometryInfo*> GeometryInfo_pvec;


class GeometryInfoList
{
  public:
                        GeometryInfoList();
                        GeometryInfoList(GeometryInfoList& geometryInfoList);
    virtual             ~GeometryInfoList();

    GeometryInfoList&   operator=(GeometryInfoList& geometryInfoList);

    GeometryInfo*       addGeometryInfo(GeometryInfo *geometryInfo);
    void                clear();
    bool                deleteGeometryInfoById(uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId);
    uint                deleteGeometryInfoListByProducerId(uint producerId);
    uint                deleteGeometryInfoListByGenerationId(uint generationId);
    uint                deleteGeometryInfoListByGenerationIdList(std::set<uint>& generationIdList);
    uint                deleteGeometryInfoListBySourceId(uint producerId);
    uint                deleteMarkedGeometries();
    int                 getClosestIndex(uint comparisonMethod,GeometryInfo& geometryInfo);
    int                 getClosestIndexNoLock(uint comparisonMethod,GeometryInfo& geometryInfo);
    GeometryInfo*       getGeometryInfoById(uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId);
    bool                getGeometryInfoById(uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,GeometryInfo& geometryInfo);
    GeometryInfo*       getGeometryInfoByIndex(uint index);
    GeometryInfo*       getGeometryInfoByIndexNoCheck(uint index);
    void                getGeometryInfoListByGenerationId(uint generationId,GeometryInfoList& geometryInfoList);
    void                getGeometryInfoListByGenerationIdAndStatus(uint generationId,GeometryInfoList& geometryInfoList,uchar geometryStatus);
    void                getGeometryInfoListByProducerId(uint producerId,GeometryInfoList& geometryInfoList);
    void                getGeometryInfoListByProducerIdAndStatus(uint producerId,GeometryInfoList& geometryInfoList,uchar geometryStatus);
    void                getGeometryInfoListBySourceId(uint sourceId,GeometryInfoList& geometryInfoList);
    uint                getLength();
    std::size_t         getHash();
    std::size_t         getHashByProducerId(uint producerId);
    ModificationLock*   getModificationLockPtr();
    bool                getReleaseObjects();
    uint                getSize() const;
    uint                markDeleted();
    uint                markDeletedById(uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId);
    uint                markDeletedByGenerationId(uint generationId);
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

    GeometryInfoPtr     *mArray;
    uint                mSize;
    uint                mLength;
    ModificationLock    mModificationLock;
    ModificationLock*   mModificationLockPtr;
    bool                mReleaseObjects;
    uint                mComparisonMethod;
};

typedef std::shared_ptr<GeometryInfoList> GeometryInfoList_sptr;


}
}
