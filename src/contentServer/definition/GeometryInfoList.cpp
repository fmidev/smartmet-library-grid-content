#include "GeometryInfoList.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoThreadLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/common/ShowFunction.h>
#include <boost/functional/hash.hpp>

#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace T
{



int geometryInfo_compare_1(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    GeometryInfoPtr *obj1 = const_cast<GeometryInfoPtr*>(reinterpret_cast<const GeometryInfoPtr *>(_val1));
    GeometryInfoPtr *obj2 = const_cast<GeometryInfoPtr*>(reinterpret_cast<const GeometryInfoPtr *>(_val2));

    return (*obj1)->compare(1,(*obj2));
  }
  return 0;
}



int geometryInfo_compare_2(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    GeometryInfoPtr *obj1 = const_cast<GeometryInfoPtr*>(reinterpret_cast<const GeometryInfoPtr *>(_val1));
    GeometryInfoPtr *obj2 = const_cast<GeometryInfoPtr*>(reinterpret_cast<const GeometryInfoPtr *>(_val2));

    return (*obj1)->compare(2,(*obj2));
  }
  return 0;
}



int geometryInfo_compare_3(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    GeometryInfoPtr *obj1 = const_cast<GeometryInfoPtr*>(reinterpret_cast<const GeometryInfoPtr *>(_val1));
    GeometryInfoPtr *obj2 = const_cast<GeometryInfoPtr*>(reinterpret_cast<const GeometryInfoPtr *>(_val2));

    return (*obj1)->compare(3,(*obj2));
  }
  return 0;
}



int geometryInfo_compare_4(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    GeometryInfoPtr *obj1 = const_cast<GeometryInfoPtr*>(reinterpret_cast<const GeometryInfoPtr *>(_val1));
    GeometryInfoPtr *obj2 = const_cast<GeometryInfoPtr*>(reinterpret_cast<const GeometryInfoPtr *>(_val2));

    return (*obj1)->compare(4,(*obj2));
  }
  return 0;
}




int geometryInfo_compare_5(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    GeometryInfoPtr *obj1 = const_cast<GeometryInfoPtr*>(reinterpret_cast<const GeometryInfoPtr *>(_val1));
    GeometryInfoPtr *obj2 = const_cast<GeometryInfoPtr*>(reinterpret_cast<const GeometryInfoPtr *>(_val2));

    return (*obj1)->compare(5,(*obj2));
  }
  return 0;
}




GeometryInfoList::GeometryInfoList()
{
  FUNCTION_TRACE
  try
  {
    mModificationLockPtr = &mModificationLock;
    mReleaseObjects = true;
    mComparisonMethod = GeometryInfo::ComparisonMethod::none;
    mSize = 0;
    mLength = 0;
    mArray = nullptr;
    mModificationLock.setLockingEnabled(false);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





GeometryInfoList::GeometryInfoList(GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    mModificationLock.setLockingEnabled(false);
    mModificationLockPtr = &mModificationLock;
    if (geometryInfoList.getModificationLockPtr() != mModificationLockPtr)
      geometryInfoList.lock();

    mReleaseObjects = true;
    mArray = nullptr;
    mSize = geometryInfoList.getSize();
    mLength = geometryInfoList.getLength();
    mComparisonMethod = geometryInfoList.mComparisonMethod;
    if (mSize > 0)
    {
      mArray = new GeometryInfoPtr[mSize];

      for (uint t=0; t<mSize; t++)
      {
        GeometryInfo *info = geometryInfoList.getGeometryInfoByIndexNoCheck(t);
        if (info != nullptr && (info->mFlags & T::GeometryInfo::Flags::DeletedGeometry) == 0)
        {
          mArray[t] = info->duplicate();
        }
        else
        {
          mArray[t] = nullptr;
        }
      }
    }
    if (geometryInfoList.getModificationLockPtr() != mModificationLockPtr)
      geometryInfoList.unlock();
  }
  catch (...)
  {
    geometryInfoList.unlock();
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





GeometryInfoList::~GeometryInfoList()
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr)
      return;

    AutoWriteLock lock(mModificationLockPtr);
    if (mReleaseObjects && mLength > 0)
    {
      for (uint t=0; t<mLength; t++)
      {
        if (mArray[t] != nullptr  &&  mReleaseObjects)
          delete mArray[t];
      }
    }
    delete[] mArray;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





GeometryInfoList& GeometryInfoList::operator=(GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (&geometryInfoList == this)
      return *this;

    clear();

    AutoWriteLock lock(mModificationLockPtr);

    if (geometryInfoList.getModificationLockPtr() != mModificationLockPtr)
      geometryInfoList.lock();

    mArray = nullptr;
    uint p = 0;
    mSize = geometryInfoList.getSize();
    mLength = geometryInfoList.getLength();
    mComparisonMethod = geometryInfoList.mComparisonMethod;
    if (mSize > 0)
    {
      mArray = new GeometryInfoPtr[mSize];

      for (uint t=0; t<mSize; t++)
      {
        mArray[t] = nullptr;
        GeometryInfo *info = geometryInfoList.getGeometryInfoByIndexNoCheck(t);
        if (info != nullptr && (info->mFlags & T::GeometryInfo::Flags::DeletedGeometry) == 0)
        {
          if (mReleaseObjects)
            mArray[p] = info->duplicate();
          else
            mArray[p] = info;

          p++;
        }
      }
    }
    if (geometryInfoList.getModificationLockPtr() != mModificationLockPtr)
      geometryInfoList.unlock();

    mLength = p;
    return *this;
  }
  catch (...)
  {
    geometryInfoList.unlock();
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfoList::setModificationLockPtr(ModificationLock* modificationLockPtr)
{
  FUNCTION_TRACE
  try
  {
    if (modificationLockPtr != nullptr)
      mModificationLockPtr = modificationLockPtr;
    else
      mModificationLockPtr = &mModificationLock;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ModificationLock* GeometryInfoList::getModificationLockPtr()
{
  FUNCTION_TRACE
  try
  {
    return mModificationLockPtr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfoList::lock()
{
  FUNCTION_TRACE
  try
  {
    return mModificationLockPtr->lock();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void GeometryInfoList::unlock()
{
  FUNCTION_TRACE
  try
  {
    return mModificationLockPtr->unlock();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





GeometryInfo* GeometryInfoList::addGeometryInfo(GeometryInfo *geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (geometryInfo == nullptr)
      return nullptr;

    AutoWriteLock lock(mModificationLockPtr);

    if (mArray == nullptr  ||  mLength == mSize)
    {
      increaseSize(mSize + mSize/5 + 10);
    }

    if (mComparisonMethod == GeometryInfo::ComparisonMethod::none)
    {
      mArray[mLength] = geometryInfo;
      mLength++;
      return geometryInfo;
    }

    int idx = getClosestIndexNoLock(mComparisonMethod,*geometryInfo);

    if (idx < C_INT(mLength)  &&  mArray[idx] != nullptr  &&   mArray[idx]->compare(mComparisonMethod,geometryInfo) == 0)
    {
      // If geometry with the same id exists, we should not add the new content, because other geometryLists might point
      // the existing geometry record;

      return mArray[idx];
    }

    while (idx < C_INT(mLength)  &&  mArray[idx] != nullptr  &&   mArray[idx]->compare(mComparisonMethod,geometryInfo) < 0)
    {
      idx++;
    }

    if (idx == C_INT(mLength))
    {
      mArray[mLength] = geometryInfo;
      mLength++;
      return geometryInfo;
    }

    if (idx < C_INT(mLength))
      memmove(&mArray[idx+1],&mArray[idx],sizeof(void*)*(mLength-idx));

    mArray[idx] = geometryInfo;
    mLength++;
    return geometryInfo;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfoList::increaseSize(uint newSize)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr)
    {
      mSize = newSize;
      mLength = 0;
      mArray = new GeometryInfoPtr[mSize];

      for (uint t=0; t<mSize; t++)
      {
        mArray[t] = nullptr;
      }
      return;
    }

    GeometryInfoPtr *newArray = new GeometryInfoPtr[newSize];
    for (uint t=0; t<newSize; t++)
    {
      if (t < mSize)
      {
        newArray[t] = mArray[t];
      }
      else
      {
        newArray[t] = nullptr;
      }
    }

    delete[] mArray;
    mArray = newArray;
    mSize = newSize;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void GeometryInfoList::clear()
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr)
      return;

    AutoWriteLock lock(mModificationLockPtr);
    if (mReleaseObjects && mLength > 0)
    {
      for (uint t=0; t<mLength; t++)
      {
        if (mArray[t] != nullptr)
          delete mArray[t];

        mArray[t] = nullptr;
      }
    }
    delete[] mArray;
    mArray = nullptr;
    mSize = 0;
    mLength = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int GeometryInfoList::getClosestIndex(uint comparisonMethod,GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoReadLock lock(mModificationLockPtr);
    return getClosestIndexNoLock(comparisonMethod,geometryInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int GeometryInfoList::getClosestIndexNoLock(uint comparisonMethod,GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr  ||  mLength == 0)
      return 0;

    if (comparisonMethod != mComparisonMethod)
    {
      for (uint t=0; t<mLength; t++)
      {
        if (mArray[t] != nullptr)
        {
          int res = mArray[t]->compare(comparisonMethod,&geometryInfo);
          if (res == 0)
          {
            return t;
          }
        }
      }
      return 0;
    }

    int low = 0;
    int high = C_INT(mLength) - 1;
    int mid = 0;

    while (low <= high)
    {
      mid = (low + high) / 2;
      int res = mArray[mid]->compare(comparisonMethod,&geometryInfo);

      if (res == 0)
      {
        while (mid > 0  &&  mArray[mid-1]->compare(comparisonMethod,&geometryInfo) == 0)
          mid--;

        return mid;
      }

      if (res < 0)
        low = mid + 1;
      else
        high = mid - 1;
    }

    if (mid >= 0  &&  mid < C_INT(mLength))
    {
      if (mArray[mid]->compare(comparisonMethod,&geometryInfo) < 0)
      {
        while (mid < C_INT(mLength)  &&  mArray[mid] != nullptr  &&   mArray[mid]->compare(comparisonMethod,&geometryInfo) < 0)
          mid++;

        return mid-1;
      }
      else
      {
        while (mid > 0  &&  mArray[mid] != nullptr  &&   mArray[mid]->compare(comparisonMethod,&geometryInfo) > 0)
          mid--;

        return mid;
      }
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool GeometryInfoList::deleteGeometryInfoById(uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return false;

    AutoWriteLock lock(mModificationLockPtr);

    GeometryInfo search;
    search.mGenerationId = generationId;
    search.mGeometryId = geometryId;
    search.mLevelId = levelId;
    int idx = getClosestIndexNoLock(GeometryInfo::ComparisonMethod::generationId,search);
    if (idx < 0 ||  idx >= C_INT(mLength))
      return false;

    GeometryInfo *info = mArray[idx];
    if (info->mGenerationId != generationId || info->mGeometryId != geometryId || info->mLevelId != levelId)
      return false;

    for (uint t=C_UINT(idx)+1; t<mLength; t++)
    {
      mArray[t-1] = mArray[t];
      mArray[t] = nullptr;
    }

    if (mReleaseObjects)
      delete info;

    mLength--;
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint GeometryInfoList::deleteGeometryInfoListByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
    uint p = 0;
    uint cnt = 0;
    for (uint t=0; t<mLength; t++)
    {
      GeometryInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
      {
        if (info->mProducerId == producerId)
        {
          cnt++;
          if (mReleaseObjects)
            delete info;
        }
        else
        {
          mArray[p] = info;
          p++;
        }
      }
    }
    mLength = p;
    return cnt;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint GeometryInfoList::deleteGeometryInfoListByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
    uint p = 0;
    uint cnt = 0;
    for (uint t=0; t<mLength; t++)
    {
      GeometryInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
      {
        if (info->mGenerationId == generationId)
        {
          cnt++;
          if (mReleaseObjects)
            delete info;
        }
        else
        {
          mArray[p] = info;
          p++;
        }
      }
    }
    mLength = p;
    return cnt;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint GeometryInfoList::deleteGeometryInfoListByGenerationIdList(std::set<uint>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
    uint p = 0;
    uint cnt = 0;
    for (uint t=0; t<mLength; t++)
    {
      GeometryInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
      {
        if (generationIdList.find(info->mGenerationId) != generationIdList.end())
        {
          cnt++;
          if (mReleaseObjects)
            delete info;
        }
        else
        {
          mArray[p] = info;
          p++;
        }
      }
    }
    mLength = p;
    return cnt;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint GeometryInfoList::deleteMarkedGeometries()
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
    uint p = 0;
    uint cnt = 0;
    for (uint t=0; t<mLength; t++)
    {
      GeometryInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
      {
        if (info->mFlags & T::GeometryInfo::Flags::DeletedGeometry)
        {
          cnt++;
          if (mReleaseObjects)
            delete info;
        }
        else
        {
          mArray[p] = info;
          p++;
        }
      }
    }
    mLength = p;
    return cnt;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint GeometryInfoList::markDeleted()
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
    uint cnt = 0;
    for (uint t=0; t<mLength; t++)
    {
      GeometryInfo *info = mArray[t];
      if (info != nullptr)
      {
        info->mFlags |= T::GeometryInfo::Flags::DeletedGeometry;
        cnt++;
      }
    }
    return cnt;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint GeometryInfoList::markDeletedByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
    uint cnt = 0;
    for (uint t=0; t<mLength; t++)
    {
      GeometryInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mGenerationId == generationId)
        {
          info->mFlags |= T::GeometryInfo::Flags::DeletedGeometry;
          cnt++;
        }
      }
    }
    return cnt;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint GeometryInfoList::markDeletedByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
    uint cnt = 0;
    for (uint t=0; t<mLength; t++)
    {
      GeometryInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mProducerId == producerId)
        {
          info->mFlags |= T::GeometryInfo::Flags::DeletedGeometry;
          cnt++;
        }
      }
    }
    return cnt;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint GeometryInfoList::markDeletedBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
    uint cnt = 0;
    for (uint t=0; t<mLength; t++)
    {
      GeometryInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mSourceId == sourceId)
        {
          info->mFlags |= T::GeometryInfo::Flags::DeletedGeometry;
          cnt++;
        }
      }
    }
    return cnt;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint GeometryInfoList::markDeletedById(uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr);

    GeometryInfo *info  = getGeometryInfoById(generationId,geometryId,levelId);
    if (info != nullptr)
    {
      info->mFlags |= T::GeometryInfo::Flags::DeletedGeometry;
      return 1;
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




GeometryInfo* GeometryInfoList::getGeometryInfoById(uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return nullptr;

    AutoReadLock lock(mModificationLockPtr);

    GeometryInfo search;
    search.mGenerationId = generationId;
    search.mGeometryId = geometryId;
    search.mLevelId = levelId;
    int idx = getClosestIndexNoLock(GeometryInfo::ComparisonMethod::generationId,search);
    if (idx < 0  ||  C_UINT(idx) >= getLength())
      return nullptr;

    GeometryInfo *info = getGeometryInfoByIndexNoCheck(idx);
    if (info != nullptr  &&  info->mGenerationId == generationId &&  info->mGeometryId == geometryId &&
        info->mLevelId == levelId &&  (info->mFlags & T::GeometryInfo::Flags::DeletedGeometry) == 0)
      return info;

    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool GeometryInfoList::getGeometryInfoById(uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return false;

    AutoReadLock lock(mModificationLockPtr);

    GeometryInfo search;
    search.mGenerationId = generationId;
    search.mGeometryId = geometryId;
    search.mLevelId = levelId;
    int idx = getClosestIndexNoLock(GeometryInfo::ComparisonMethod::generationId,search);
    if (idx < 0  ||  C_UINT(idx) >= getLength())
      return false;

    GeometryInfo *info = getGeometryInfoByIndexNoCheck(idx);
    if (info != nullptr  &&  info->mGenerationId == generationId &&  info->mGeometryId == geometryId &&
        info->mLevelId == levelId && (info->mFlags & T::GeometryInfo::Flags::DeletedGeometry) == 0)
    {
      geometryInfo = *info;
      return true;
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint GeometryInfoList::deleteGeometryInfoListBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
    uint p = 0;
    uint cnt = 0;
    for (uint t=0; t<mLength; t++)
    {
      GeometryInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
      {
        if (info->mSourceId == sourceId)
        {
          cnt++;
          if (mReleaseObjects)
            delete info;
        }
        else
        {
          mArray[p] = info;
          p++;
        }
      }
    }
    mLength = p;
    return cnt;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





GeometryInfo* GeometryInfoList::getGeometryInfoByIndex(uint index)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return nullptr;

    AutoReadLock lock(mModificationLockPtr);
    if (index >= mLength)
      return nullptr;

    return mArray[index];
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





GeometryInfo* GeometryInfoList::getGeometryInfoByIndexNoCheck(uint index)
{
  FUNCTION_TRACE
  try
  {
    return mArray[index];
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfoList::getGeometryInfoListByGenerationId(uint generationId,GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    geometryInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    int len =  getLength();
    int idx = 0;

    if (mComparisonMethod == GeometryInfo::ComparisonMethod::generationId)
    {
      GeometryInfo search;
      search.mGenerationId = generationId;

      idx = getClosestIndexNoLock(GeometryInfo::ComparisonMethod::generationId,search);
      if (idx < 0  || idx >= len)
        return;

    }

    for (int t=idx; t<len; t++)
    {
      GeometryInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mGenerationId == generationId && (info->mFlags & T::GeometryInfo::Flags::DeletedGeometry) == 0)
        {
          if (geometryInfoList.getReleaseObjects())
            geometryInfoList.addGeometryInfo(info->duplicate());
          else
            geometryInfoList.addGeometryInfo(info);
        }
        else
        {
          if (mComparisonMethod == GeometryInfo::ComparisonMethod::generationId  &&  info->mGenerationId > generationId)
            return;
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfoList::getGeometryInfoListByProducerId(uint producerId,GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    geometryInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    int len =  getLength();
    int idx = 0;

    if (mComparisonMethod == GeometryInfo::ComparisonMethod::producerId)
    {
      GeometryInfo search;
      search.mProducerId = producerId;

      idx = getClosestIndexNoLock(GeometryInfo::ComparisonMethod::producerId,search);
      if (idx < 0  || idx >= len)
        return;

    }

    for (int t=idx; t<len; t++)
    {
      GeometryInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mProducerId == producerId && (info->mFlags & T::GeometryInfo::Flags::DeletedGeometry) == 0)
        {
          if (geometryInfoList.getReleaseObjects())
            geometryInfoList.addGeometryInfo(info->duplicate());
          else
            geometryInfoList.addGeometryInfo(info);
        }
        else
        {
          if (mComparisonMethod == GeometryInfo::ComparisonMethod::producerId  &&  info->mProducerId > producerId)
            return;
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::size_t GeometryInfoList::getHash()
{
  FUNCTION_TRACE
  try
  {
    std::size_t hash = 0;
    if (mArray == nullptr ||  mLength == 0)
      return hash;

    AutoReadLock lock(mModificationLockPtr);

    int len = getLength();
    for (int t=0; t<len; t++)
    {
      GeometryInfo *info = mArray[t];
      if (info != nullptr && (info->mFlags & T::GeometryInfo::Flags::DeletedGeometry) == 0)
      {
        boost::hash_combine(hash,info->mGenerationId);
        boost::hash_combine(hash,info->mGeometryId);
        boost::hash_combine(hash,info->mLevelId);
        boost::hash_combine(hash,info->mStatus);
      }
    }
    return hash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::size_t GeometryInfoList::getHashByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    std::size_t hash = 0;
    if (mArray == nullptr ||  mLength == 0)
      return hash;

    AutoReadLock lock(mModificationLockPtr);

    int len =  getLength();
    int idx = 0;

    if (mComparisonMethod == GeometryInfo::ComparisonMethod::producerId)
    {
      GeometryInfo search;
      search.mProducerId = producerId;

      idx = getClosestIndexNoLock(GeometryInfo::ComparisonMethod::producerId,search);
      if (idx < 0  || idx >= len)
        return hash;

    }

    for (int t=idx; t<len; t++)
    {
      GeometryInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mProducerId == producerId && (info->mFlags & T::GeometryInfo::Flags::DeletedGeometry) == 0)
        {
          boost::hash_combine(hash,info->mGenerationId);
          boost::hash_combine(hash,info->mGeometryId);
          boost::hash_combine(hash,info->mLevelId);
          boost::hash_combine(hash,info->mStatus);
        }
        else
        {
          if (mComparisonMethod == GeometryInfo::ComparisonMethod::producerId  &&  info->mProducerId > producerId)
            return hash;
        }
      }
    }
    return hash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfoList::getGeometryInfoListByGenerationIdAndStatus(uint generationId,GeometryInfoList& geometryInfoList,uchar geometryStatus)
{
  FUNCTION_TRACE
  try
  {
    geometryInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    int len =  getLength();
    int idx = 0;

    if (mComparisonMethod == GeometryInfo::ComparisonMethod::generationId)
    {
      GeometryInfo search;
      search.mGenerationId = generationId;

      idx = getClosestIndexNoLock(GeometryInfo::ComparisonMethod::generationId,search);
      if (idx < 0  || idx >= len)
        return;

    }

    for (int t=idx; t<len; t++)
    {
      GeometryInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mGenerationId == generationId  &&  info->mStatus == geometryStatus && (info->mFlags & T::GeometryInfo::Flags::DeletedGeometry) == 0)
        {
          if (geometryInfoList.getReleaseObjects())
            geometryInfoList.addGeometryInfo(info->duplicate());
          else
            geometryInfoList.addGeometryInfo(info);
        }
        else
        {
          if (mComparisonMethod == GeometryInfo::ComparisonMethod::generationId  &&  info->mGenerationId > generationId)
            return;
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfoList::getGeometryInfoListByProducerIdAndStatus(uint producerId,GeometryInfoList& geometryInfoList,uchar geometryStatus)
{
  FUNCTION_TRACE
  try
  {
    geometryInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    int len =  getLength();
    int idx = 0;

    if (mComparisonMethod == GeometryInfo::ComparisonMethod::producerId)
    {
      GeometryInfo search;
      search.mProducerId = producerId;

      idx = getClosestIndexNoLock(GeometryInfo::ComparisonMethod::producerId,search);
      if (idx < 0  || idx >= len)
        return;

    }

    for (int t=idx; t<len; t++)
    {
      GeometryInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mProducerId == producerId  &&  info->mStatus == geometryStatus && (info->mFlags & T::GeometryInfo::Flags::DeletedGeometry) == 0)
        {
          if (geometryInfoList.getReleaseObjects())
            geometryInfoList.addGeometryInfo(info->duplicate());
          else
            geometryInfoList.addGeometryInfo(info);
        }
        else
        {
          if (mComparisonMethod == GeometryInfo::ComparisonMethod::producerId  &&  info->mProducerId > producerId)
            return;
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfoList::getGeometryInfoListBySourceId(uint sourceId,GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    geometryInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);
    for (uint t=0; t<mLength; t++)
    {
      GeometryInfo *info = mArray[t];
      if (info != nullptr  &&  info->mSourceId == sourceId && (info->mFlags & T::GeometryInfo::Flags::DeletedGeometry) == 0)
      {
        if (geometryInfoList.getReleaseObjects())
          geometryInfoList.addGeometryInfo(info->duplicate());
        else
          geometryInfoList.addGeometryInfo(info);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint GeometryInfoList::getLength()
{
  FUNCTION_TRACE
  try
  {
    return mLength;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint GeometryInfoList::getSize() const
{
  FUNCTION_TRACE
  try
  {
    return mSize;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool GeometryInfoList::getReleaseObjects()
{
  FUNCTION_TRACE
  try
  {
    return mReleaseObjects;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfoList::setReleaseObjects(bool releaseObjects)
{
  FUNCTION_TRACE
  try
  {
    mReleaseObjects = releaseObjects;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfoList::setLockingEnabled(bool lockingEnabled)
{
  FUNCTION_TRACE
  try
  {
    mModificationLock.setLockingEnabled(lockingEnabled);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfoList::setComparisonMethod(uint comparisonMethod)
{
  FUNCTION_TRACE
  try
  {
    if (mComparisonMethod == comparisonMethod)
      return;

    sort(comparisonMethod);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfoList::sort(uint comparisonMethod)
{
  FUNCTION_TRACE
  try
  {
    if (mComparisonMethod == comparisonMethod)
      return;

    AutoWriteLock lock(mModificationLockPtr);
    mComparisonMethod = comparisonMethod;

    if (mArray == nullptr || mLength < 2)
      return;

    switch (comparisonMethod)
    {
      case 1:
        qsort(mArray,mLength,sizeof(GeometryInfoPtr),geometryInfo_compare_1);
        break;
      case 2:
        qsort(mArray,mLength,sizeof(GeometryInfoPtr),geometryInfo_compare_2);
        break;
      case 3:
        qsort(mArray,mLength,sizeof(GeometryInfoPtr),geometryInfo_compare_3);
        break;
      case 4:
        qsort(mArray,mLength,sizeof(GeometryInfoPtr),geometryInfo_compare_4);
        break;
      case 5:
        qsort(mArray,mLength,sizeof(GeometryInfoPtr),geometryInfo_compare_5);
        break;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfoList::writeToFile(const std::string& filename)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr);

    FILE *file = fopen(filename.c_str(),"we");
    if (file == nullptr)
    {
      Fmi::Exception exception(BCP,"Cannot create the file!");
      exception.addParameter("Filename",filename);
      throw exception;
    }

    for (uint t=0; t<mLength; t++)
    {
      GeometryInfo *info = mArray[t];
      if (info != nullptr && (info->mFlags & T::GeometryInfo::Flags::DeletedGeometry) == 0)
        fprintf(file,"%s\n",info->getCsv().c_str());
    }
    fclose(file);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GeometryInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr);

    stream << space(level) << "GeometryInfoList\n";
    stream << space(level) << "- mComparisonMethod  = " << mComparisonMethod << "\n";
    for (uint t=0; t<mLength; t++)
    {
      GeometryInfo *info = mArray[t];
      if (info != nullptr)
        info->print(stream,level+1,optionFlags);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




}
}
