#include "ContentInfoList.h"
#include "RequestFlags.h"

#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoThreadLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/ShowFunction.h>
#include <boost/functional/hash.hpp>

#define FUNCTION_TRACE FUNCTION_TRACE_OFF



namespace SmartMet
{

namespace T
{


ThreadLock ContentInfoList_sortLock;

uint contentInfo_comparisonMethod = ContentInfo::ComparisonMethod::none;



int contentInfo_compare(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(contentInfo_comparisonMethod,(*obj2));
  }
  return 0;
}




ContentInfoList::ContentInfoList()
{
  FUNCTION_TRACE
  try
  {
    mModificationLockPtr = &mModificationLock;
    mReleaseObjects = true;
    mComparisonMethod = ContentInfo::ComparisonMethod::none;
    mSize = 100;
    mLength = 0;
    mArray = new ContentInfoPtr[100];

    for (uint t=0; t<100; t++)
    {
      mArray[t] = nullptr;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ContentInfoList::ContentInfoList(ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    mModificationLockPtr = &mModificationLock;
    if (contentInfoList.getModificationLockPtr() != mModificationLockPtr)
      contentInfoList.lock();

    mReleaseObjects = true;
    mArray = nullptr;
    mSize = contentInfoList.getSize();
    mLength = contentInfoList.getLength();
    mComparisonMethod = contentInfoList.mComparisonMethod;
    if (mSize > 0)
    {
      mArray = new ContentInfoPtr[mSize];

      for (uint t=0; t<mSize; t++)
      {
        ContentInfo *info = contentInfoList.getContentInfoByIndexNoCheck(t);
        if (info != nullptr)
        {
          mArray[t] = info->duplicate();
        }
        else
        {
          mArray[t] = nullptr;
        }
      }
    }
    if (contentInfoList.getModificationLockPtr() != mModificationLockPtr)
      contentInfoList.unlock();
  }
  catch (...)
  {
    contentInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ContentInfoList::ContentInfoList(const ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    mModificationLockPtr = &mModificationLock;
    //contentInfoList.lock();

    mReleaseObjects = true;
    mArray = nullptr;
    mSize = contentInfoList.getSize();
    mLength = contentInfoList.getLength();
    mComparisonMethod = contentInfoList.mComparisonMethod;
    if (mSize > 0)
    {
      mArray = new ContentInfoPtr[mSize];

      for (uint t=0; t<mSize; t++)
      {
        ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (info != nullptr)
        {
          mArray[t] = info->duplicate();
        }
        else
        {
          mArray[t] = nullptr;
        }
      }
    }
    //contentInfoList.unlock();
  }
  catch (...)
  {
    //contentInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ContentInfoList::~ContentInfoList()
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    if (mArray != nullptr)
    {
      for (uint t=0; t<mSize; t++)
      {
        if (mArray[t] != nullptr  &&  mReleaseObjects)
          delete mArray[t];

        mArray[t] = nullptr;
      }

      delete[] mArray;
    }

    mArray = nullptr;
    mSize = 0;
    mLength = 0;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





ContentInfoList& ContentInfoList::operator=(ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (&contentInfoList == this)
      return *this;

    clear();

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (contentInfoList.getModificationLockPtr() != mModificationLockPtr)
      contentInfoList.lock();

    mArray = nullptr;
    mSize = contentInfoList.getSize();
    mLength = contentInfoList.getLength();
    mComparisonMethod = contentInfoList.mComparisonMethod;
    if (mSize > 0)
    {
      mArray = new ContentInfoPtr[mSize];

      for (uint t=0; t<mSize; t++)
      {
        ContentInfo *info = contentInfoList.getContentInfoByIndexNoCheck(t);
        if (info != nullptr)
        {
          if (mReleaseObjects)
            mArray[t] = info->duplicate();
          else
            mArray[t] = info;
        }
        else
        {
          mArray[t] = nullptr;
        }
      }
    }
    if (contentInfoList.getModificationLockPtr() != mModificationLockPtr)
      contentInfoList.unlock();

    return *this;
  }
  catch (...)
  {
    contentInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ContentInfoList& ContentInfoList::operator=(const ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (&contentInfoList == this)
      return *this;

    clear();

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    mArray = nullptr;
    mSize = contentInfoList.getSize();
    mLength = contentInfoList.getLength();
    mComparisonMethod = contentInfoList.mComparisonMethod;
    if (mSize > 0)
    {
      mArray = new ContentInfoPtr[mSize];

      for (uint t=0; t<mSize; t++)
      {
        ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (info != nullptr)
        {
          if (mReleaseObjects)
            mArray[t] = info->duplicate();
          else
            mArray[t] = info;
        }
        else
        {
          mArray[t] = nullptr;
        }
      }
    }
    return *this;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ContentInfo* ContentInfoList::addContentInfo(ContentInfo *contentInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mArray == nullptr  ||  mLength == mSize)
    {
      increaseSize(mSize + mSize/5 + 10);
    }

    if (mComparisonMethod == ContentInfo::ComparisonMethod::none)
    {
      mArray[mLength] = contentInfo;
      mLength++;
      return contentInfo;
    }

    int idx = getClosestIndexNoLock(mComparisonMethod,*contentInfo);

    if (idx < C_INT(mLength)  &&  mArray[idx] != nullptr  &&   mArray[idx]->compare(mComparisonMethod,contentInfo) == 0)
    {
      // If content with the same id exists, we should not add the new contet, because other contentLists might point
      // the existing content record;

      return mArray[idx];
    }

    while (idx < C_INT(mLength)  &&  mArray[idx] != nullptr  &&   mArray[idx]->compare(mComparisonMethod,contentInfo) < 0)
    {
      idx++;
    }

    if (idx == C_INT(mLength))
    {
      mArray[mLength] = contentInfo;
      mLength++;
      return contentInfo;
    }

    if (idx < C_INT(mLength))
      memmove(&mArray[idx+1],&mArray[idx],sizeof(void*)*(mLength-idx));

    mArray[idx] = contentInfo;
    mLength++;
    return contentInfo;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::addContentInfoList(ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    addContentInfoListNoLock(contentInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::addContentInfoListNoLock(ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    uint len1 = mLength;
    uint len2 = contentInfoList.getLength();

    // printf("Add content info list %llu  %u %u\n",(unsigned long long)this,len1,len2);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::none)
    {
      if (mArray == nullptr  ||  mLength == mSize  ||  (mLength + len2) > mSize)
      {
        increaseSize(len1 + len2 + mSize/5);
      }

      for (uint t=0; t<len2; t++)
      {
        ContentInfo *cInfo = contentInfoList.getContentInfoByIndex(t);
        ContentInfo *contentInfo = cInfo;

        if (mReleaseObjects)
          contentInfo = cInfo->duplicate();

        mArray[mLength] = contentInfo;
        mLength++;
      }
      return;
    }

    uint newSize = len1 + len2 + 100;
    ContentInfoPtr *newArray = new ContentInfoPtr[newSize];
    uint a = 0;
    uint b = 0;

    contentInfoList.sort(mComparisonMethod);

    uint cnt = 0;

    for (uint t=0; t<newSize; t++)
    {
      ContentInfo *cInfo1 = nullptr;
      ContentInfo *cInfo2 = nullptr;

      if (a < len1)
        cInfo1 = mArray[a];

      if (b < len2)
        cInfo2 = contentInfoList.getContentInfoByIndex(b);

      if (cInfo1 == nullptr  &&  cInfo2 == nullptr)
      {
        newArray[t] = nullptr;
      }
      else
      if (cInfo1 != nullptr  &&  cInfo2 == nullptr)
      {
        newArray[t] = cInfo1;
        cnt++;
        a++;
      }
      else
      if (cInfo1 == nullptr  &&  cInfo2 != nullptr)
      {
        if (mReleaseObjects)
          newArray[t] = cInfo2->duplicate();
        else
          newArray[t] = cInfo2;

        cnt++;
        b++;
      }
      else
      if (cInfo1 != nullptr  &&  cInfo2 != nullptr)
      {
        //if (cInfo1->compare(mComparisonMethod,cInfo2) <= 0)
        if (cInfo2->compare(mComparisonMethod,cInfo1) > 0)
        {
          newArray[t] = cInfo1;
          a++;
        }
        else
        {
          if (mReleaseObjects)
            newArray[t] = cInfo2->duplicate();
          else
            newArray[t] = cInfo2;

          b++;
        }
        cnt++;
      }
    }

    mSize = newSize;
    mLength = cnt;

    delete[] mArray;
    mArray = newArray;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::increaseSize(uint newSize)
{
  FUNCTION_TRACE
  try
  {
//    printf("SET SIZE %u => %u\n",mSize,newSize);
    if (mArray == nullptr)
    {
      mSize = newSize;
      mLength = 0;
      mArray = new ContentInfoPtr[mSize];

      for (uint t=0; t<mSize; t++)
      {
        mArray[t] = nullptr;
      }
      return;
    }

    ContentInfoPtr *newArray = new ContentInfoPtr[newSize];
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::clear()
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    if (mArray != nullptr)
    {
      for (uint t=0; t<mSize; t++)
      {
        if (mArray[t] != nullptr  &&  mReleaseObjects)
          delete mArray[t];

        mArray[t] = nullptr;
      }

      delete[] mArray;
    }

    mArray = nullptr;
    mSize = 0;
    mLength = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





bool ContentInfoList::containsSameForecastTimes()
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return false;

    std::set<std::string> timeList;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        if (timeList.find(info->mForecastTime) != timeList.end())
          return true;

        timeList.insert(info->mForecastTime);
      }
    }

    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::deleteContentInfoByFileId(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  &&  (info->mFileId == fileId  ||  (info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0))
      {
        if (mReleaseObjects)
          delete info;

        count++;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::deleteMarkedContent()
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0)
      {
        if (mReleaseObjects)
          delete info;

        count++;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::markDeletedByFileId(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    uint cnt = 0;
    if (mComparisonMethod == ContentInfo::ComparisonMethod::file_message)
    {
      ContentInfo info;
      info.mFileId = fileId;
      info.mMessageIndex = 0;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);
      uint t = 0;
      if (idx >= 0)
        t = idx;

      while (t < mLength)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr)
        {
          if (info->mFileId > fileId)
            return cnt;

          if (info->mFileId == fileId)
          {
            info->mFlags = info->mFlags | T::ContentInfo::Flags::DeletedContent;
            cnt++;
          }
        }
        t++;
      }
      return cnt;
    }


    // The array is not sorted, so we should just remove the content.

    return deleteContentInfoByFileId(fileId);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::deleteContentInfo(ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    int idx = getClosestIndexNoLock(mComparisonMethod,contentInfo);

    uint startP = 0;
    if (idx >= 0  &&  idx < C_INT(mLength)  &&  mArray[idx] != nullptr  &&
        mArray[idx]->mFileId == contentInfo.mFileId  &&  mArray[idx]->mMessageIndex == contentInfo.mMessageIndex)
    {
      startP = idx;
    }

    uint p = startP;
    uint count = 0;
    for (uint t=startP; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  &&  ((info->mFileId == contentInfo.mFileId  &&  info->mMessageIndex == contentInfo.mMessageIndex)  ||  (info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0))
      {
        if (mReleaseObjects)
          delete info;

        count++;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::deleteContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  &&  ((info->mFileId == fileId  &&  info->mMessageIndex == messageIndex)  ||  (info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0))
      {
        if (mReleaseObjects)
          delete info;

        count++;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




uint ContentInfoList::deleteContentInfoByGroupFlags(uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  &&  ((info->mGroupFlags & groupFlags) != 0  ||  (info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0))
      {
        if (mReleaseObjects)
          delete info;

        count++;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::deleteContentInfoByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  &&  (info->mProducerId == producerId  ||  (info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0))
      {
        if (mReleaseObjects)
          delete info;

        count++;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getLevelInfoList(T::LevelInfoList& levelInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    std::set<std::size_t> list;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        std::size_t seed = 0;
        boost::hash_combine(seed,info->mProducerId);
        boost::hash_combine(seed,info->mFmiParameterName);
        boost::hash_combine(seed,info->mFmiParameterLevelId);
        boost::hash_combine(seed,info->mGrib1ParameterLevelId);
        boost::hash_combine(seed,info->mGrib2ParameterLevelId);
        boost::hash_combine(seed,info->mParameterLevel);

        if (list.find(seed) == list.end())
        {
          list.insert(seed);
          levelInfoList.addLevelInfo(new T::LevelInfo(info->mProducerId,info->mFmiParameterName,info->mFmiParameterLevelId,info->mGrib1ParameterLevelId,info->mGrib2ParameterLevelId,info->mParameterLevel));
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::deleteContentInfoByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  &&  (info->mGenerationId == generationId  ||  (info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0))
      {
        if (mReleaseObjects)
          delete info;

        count++;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::deleteContentInfoByGenerationIdList(std::set<uint>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  &&  (generationIdList.find(info->mGenerationId) != generationIdList.end()  ||  (info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0))
      {
        if (mReleaseObjects)
          delete info;

        count++;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::deleteContentInfoByGenerationAndGeometry(uint generationId,T::GeometryId geometryId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  &&  ((info->mGenerationId == generationId  &&  info->mGeometryId == geometryId)  ||  (info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0))
      {
        if (mReleaseObjects)
          delete info;

        count++;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::deleteContentInfoByGenerationGeometryAndForecastTime(uint generationId,T::GeometryId geometryId,std::string forecastTime)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  &&  ((info->mGenerationId == generationId  &&  info->mGeometryId == geometryId  &&  info->mForecastTime == forecastTime)  ||  (info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0))
      {
        if (mReleaseObjects)
          delete info;

        count++;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::deleteContentInfoBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr &&  (info->mSourceId == sourceId  ||  (info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0))
      {
        if (mReleaseObjects)
          delete info;

        count++;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::deleteContentInfoByFileIdList(std::set<uint>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  &&  (fileIdList.find(info->mFileId) != fileIdList.end()  ||  (info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0))
      {
        if (mReleaseObjects)
          delete info;

        count++;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::deleteVirtualContent()
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  &&  (info->mFileType == T::FileTypeValue::Virtual  ||  (info->mFlags & T::ContentInfo::Flags::VirtualContent) != 0))
      {
        if (mReleaseObjects)
          delete info;

        count++;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::keepContentInfoByGeometryIdList(std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr &&  ((info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0  ||  geometryIdList.find(info->mGeometryId) == geometryIdList.end()))
      {
        if (mReleaseObjects)
          delete info;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::keepContentInfoByGeometryId(T::GeometryId geometryId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr &&  ((info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0 ||  info->mGeometryId != geometryId))
      {
        if (mReleaseObjects)
          delete info;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::keepContentInfoByGroupFlags(uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  && ((info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0 || (info->mGroupFlags & groupFlags) == 0))
      {
        if (mReleaseObjects)
          delete info;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::keepContentInfoByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  &&  ((info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0 || info->mProducerId != producerId))
      {
        if (mReleaseObjects)
          delete info;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::keepContentInfoByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  &&  ((info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0 || info->mGenerationId != generationId))
      {
        if (mReleaseObjects)
          delete info;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::keepContentInfoBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  &&  ((info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0 ||  info->mSourceId != sourceId))
      {
        if (mReleaseObjects)
          delete info;
      }
      else
      {
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




uint ContentInfoList::registerContentInfoByServerAndFileId(uint serverId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    unsigned long long sf = 0;
    if (serverId >= 1  &&  serverId <= 64)
      sf = (unsigned long long)1 << (serverId-1);

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    ContentInfo contentInfo;
    contentInfo.mFileId = fileId;
    contentInfo.mMessageIndex = 0;

    int idx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (idx < 0 ||  C_UINT(idx) >= mLength)
      return 0;

    uint count = 0;
    for (uint t=idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  info->mFileId == fileId)
      {
        info->mServerFlags = info->mServerFlags | sf;
        count++;
      }
      else
      {
        if (mComparisonMethod == ContentInfo::ComparisonMethod::file_message)
          return count;
      }
    }
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::unregisterContentInfoByServerId(uint serverId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    unsigned long long sf = 0;
    if (serverId >= 1  &&  serverId <= 64)
      sf = (unsigned long long)1 << (serverId-1);

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mServerFlags & sf) != 0)
      {
        info->mServerFlags = info->mServerFlags - sf;
        count++;
      }
    }
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::unregisterContentInfoByServerAndFileId(uint serverId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    unsigned long long sf = 0;
    if (serverId >= 1  &&  serverId <= 64)
      sf = (unsigned long long)1 << (serverId-1);

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    ContentInfo contentInfo;
    contentInfo.mFileId = fileId;
    contentInfo.mMessageIndex = 0;

    int idx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (idx < 0 ||  C_UINT(idx) >= mLength)
      return 0;

    uint count = 0;
    for (uint t=idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  info->mFileId == fileId)
      {
        if ((info->mServerFlags & sf) != 0)
        {
          info->mServerFlags = info->mServerFlags - sf;
          count++;
        }
      }
      else
      {
        if (mComparisonMethod == ContentInfo::ComparisonMethod::file_message)
          return count;
      }
    }
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentListByForecastTime(std::string forecastTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    ContentInfo *prevInfo = nullptr;
    ContentInfo *nextInfo = nullptr;


    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        if (info->mForecastTime <= forecastTime)
        {
          if (prevInfo == nullptr  ||  info->mForecastTime > prevInfo->mForecastTime)
            prevInfo = info;
        }
        else
        {
          if (nextInfo == nullptr  ||  info->mForecastTime < nextInfo->mForecastTime)
            nextInfo = info;
        }
      }
    }

    if (prevInfo != nullptr  &&  (nextInfo != nullptr  ||  prevInfo->mForecastTime == forecastTime))
    {
      if (contentInfoList.getReleaseObjects())
        contentInfoList.addContentInfo(prevInfo->duplicate());
      else
        contentInfoList.addContentInfo(prevInfo);

      if (nextInfo != nullptr  &&  prevInfo->mForecastTime != forecastTime)
      {
        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(nextInfo->duplicate());
        else
          contentInfoList.addContentInfo(nextInfo);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}






int ContentInfoList::getClosestIndex(uint comparisonMethod,ContentInfo& contentInfo)
{
  //FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    return getClosestIndexNoLock(comparisonMethod,contentInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ContentInfoList::getClosestIndexNoLock(uint comparisonMethod,ContentInfo& contentInfo)
{
  //FUNCTION_TRACE
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
          int res = mArray[t]->compare(comparisonMethod,&contentInfo);
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

      if (mArray[mid] == nullptr)
        return 0;

      int res = mArray[mid]->compare(comparisonMethod,&contentInfo);

      if (res == 0)
      {
        while (mid > 0  &&  mArray[mid-1] != nullptr  &&  mArray[mid-1]->compare(comparisonMethod,&contentInfo) == 0)
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
      if (mArray[mid] != nullptr  &&  mArray[mid]->compare(comparisonMethod,&contentInfo) < 0)
      {
        while (mid < C_INT(mLength)  &&  mArray[mid] != nullptr  &&   mArray[mid]->compare(comparisonMethod,&contentInfo) < 0)
          mid++;

        return mid-1;
      }
      else
      {
        while (mid > 0  &&  mArray[mid] != nullptr  &&   mArray[mid]->compare(comparisonMethod,&contentInfo) > 0)
          mid--;

        return mid;
      }
    }
    return 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ContentInfo* ContentInfoList::getContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex)
{
  //FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return nullptr;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    ContentInfo contentInfo;
    contentInfo.mFileId = fileId;
    contentInfo.mMessageIndex = messageIndex;

    int idx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (idx < 0  ||  C_UINT(idx) >= mLength)
      return nullptr;

    T::ContentInfo *info = mArray[idx];
    if (info != nullptr &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mFileId == fileId  &&  info->mMessageIndex == messageIndex)
      return mArray[idx];

    return nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ContentInfo* ContentInfoList::getContentInfoByFileIdAndMessageIndexNoLock(uint fileId,uint messageIndex)
{
  //FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return nullptr;

    ContentInfo contentInfo;
    contentInfo.mFileId = fileId;
    contentInfo.mMessageIndex = messageIndex;

    int idx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (idx < 0  ||  C_UINT(idx) >= mLength)
      return nullptr;

    T::ContentInfo *info = mArray[idx];
    if (info != nullptr &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mFileId == fileId  &&  info->mMessageIndex == messageIndex)
      return mArray[idx];

    return nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





bool ContentInfoList::getContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex,ContentInfo& contentInfo)
{
  //FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return false;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    ContentInfo contentInfo;
    contentInfo.mFileId = fileId;
    contentInfo.mMessageIndex = messageIndex;

    int idx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (idx < 0 ||  C_UINT(idx) >= mLength)
      return false;

    T::ContentInfo *info = mArray[idx];
    if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mFileId == fileId  &&  info->mMessageIndex == messageIndex  )
    {
      contentInfo = *info;
      return true;
    }

    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}






ContentInfo* ContentInfoList::getContentInfoByIndex(uint index) const
{
  //FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return nullptr;

    //AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    if (index >= mLength)
      return nullptr;

    return mArray[index];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ContentInfo* ContentInfoList::getContentInfoByIndexNoCheck(uint index)
{
  //FUNCTION_TRACE
  try
  {
    return mArray[index];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ContentInfo* ContentInfoList::getContentInfoByParameterLevelInfo(T::ParameterLevelInfo& levelInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return nullptr;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mParameterLevel == levelInfo.mLevel)
      {
        if ((levelInfo.mParameterKeyType == T::ParamKeyTypeValue::FMI_ID  &&  info->mFmiParameterId == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyTypeValue::FMI_NAME  &&  info->mFmiParameterName == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID  &&  info->mNewbaseParameterId == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME  &&  info->mNewbaseParameterName == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyTypeValue::CDM_ID  &&  info->mCdmParameterId == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyTypeValue::CDM_NAME  &&  info->mCdmParameterName == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyTypeValue::GRIB_ID  &&  info->mGribParameterId == levelInfo.mParameterKey))
        {
          if ((levelInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::ANY || levelInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI) &&
              info->mFmiParameterLevelId == levelInfo.mParameterLevelId)
            return info;

          if ((levelInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::ANY || levelInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1) &&
              info->mGrib1ParameterLevelId == levelInfo.mParameterLevelId)
            return info;

          if ((levelInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::ANY || levelInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2) &&
              info->mGrib2ParameterLevelId == levelInfo.mParameterLevelId)
            return info;
        }
      }
    }
    return nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByParameterLevelInfo(T::ParameterLevelInfo& levelInfo,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mParameterLevel == levelInfo.mLevel)
      {
        if ((levelInfo.mParameterKeyType == T::ParamKeyTypeValue::FMI_ID  &&  info->mFmiParameterId == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyTypeValue::FMI_NAME  &&  info->mFmiParameterName == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID  &&  info->mNewbaseParameterId == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME  &&  info->mNewbaseParameterName == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyTypeValue::CDM_ID  &&  info->mCdmParameterId == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyTypeValue::CDM_NAME  &&  info->mCdmParameterName == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyTypeValue::GRIB_ID  &&  info->mGribParameterId == levelInfo.mParameterKey))
        {
          if ((levelInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::ANY || levelInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::FMI) &&
              info->mFmiParameterLevelId == levelInfo.mParameterLevelId)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);
          }

          if ((levelInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::ANY || levelInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1) &&
              info->mGrib1ParameterLevelId == levelInfo.mParameterLevelId)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);
          }

          if ((levelInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::ANY || levelInfo.mParameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2) &&
              info->mGrib2ParameterLevelId == levelInfo.mParameterLevelId)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);
          }
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoList(uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      // If the records are not sorted according to fileId and messageIndex then the startFileId parameter
      // is used as the start index in the list.

      for (uint t=startFileId; t<mLength; t++)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);

          if (contentInfoList.getLength() >= maxRecords)
            return;
        }
      }
      return;
    }

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (startIdx < 0)
      startIdx = 0;

    for (uint t=startIdx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        if (info->mFileId > startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex >= startMessageIndex))
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);

          if (contentInfoList.getLength() >= maxRecords)
            return;
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByGroupFlags(uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      // If the records are not sorted according to fileId and messageIndex then the startFileId parameter
      // is used as the start index in the list.

      for (uint t=startFileId; t<mLength; t++)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (info->mGroupFlags & groupFlags) != 0)
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);

          if (contentInfoList.getLength() >= maxRecords)
            return;
        }
      }
      return;
    }

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (startIdx < 0)
      startIdx = 0;

    for (uint t=startIdx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];

      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        if ((info->mGroupFlags & groupFlags) != 0  &&  (info->mFileId > startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex >= startMessageIndex)))
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);

          if (contentInfoList.getLength() >= maxRecords)
            return;
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentParamKeyListByGenerationId(uint producerId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    paramKeyList.clear();

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];

      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGenerationId == generationId)
      {
        switch (parameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            if (info->mFmiParameterId > " "  &&  paramKeyList.find(info->mFmiParameterId) == paramKeyList.end())
              paramKeyList.insert(info->mFmiParameterId);
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            if (info->mFmiParameterName > " ")
            {
              if (paramKeyList.find(info->mFmiParameterName) == paramKeyList.end())
                paramKeyList.insert(info->mFmiParameterName);
            }
            else
            if (info->mNewbaseParameterId > " ")
            {
              std::string id = "NB-" + info->mNewbaseParameterId;
              if (paramKeyList.find(id) == paramKeyList.end())
                paramKeyList.insert(id);
            }
            else
            if (info->mGribParameterId > " ")
            {
              std::string id = "GRIB-" + info->mGribParameterId;
              if (paramKeyList.find(id) == paramKeyList.end())
                paramKeyList.insert(id);
            }
            break;

          case T::ParamKeyTypeValue::GRIB_ID:
            if (info->mGribParameterId > " "  &&  paramKeyList.find(info->mGribParameterId) == paramKeyList.end())
              paramKeyList.insert(info->mGribParameterId);
            break;

          case T::ParamKeyTypeValue::NEWBASE_ID:
            if (info->mNewbaseParameterId > " "  &&  paramKeyList.find(info->mNewbaseParameterId) == paramKeyList.end())
              paramKeyList.insert(info->mNewbaseParameterName);
            break;

          case T::ParamKeyTypeValue::NEWBASE_NAME:
            if (info->mNewbaseParameterName > " "  &&  paramKeyList.find(info->mNewbaseParameterName) == paramKeyList.end())
              paramKeyList.insert(info->mNewbaseParameterName);
            break;

          case T::ParamKeyTypeValue::CDM_ID:
            if (info->mCdmParameterId > " " &&  paramKeyList.find(info->mCdmParameterId) == paramKeyList.end())
              paramKeyList.insert(info->mCdmParameterId);
            break;

          case T::ParamKeyTypeValue::CDM_NAME:
            if (info->mCdmParameterName > " " &&  paramKeyList.find(info->mCdmParameterName) == paramKeyList.end())
              paramKeyList.insert(info->mCdmParameterName);
            break;

          default:
            return;
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentGeometryIdListByGenerationId(uint producerId,uint generationId,std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    geometryIdList.clear();

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];

      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGenerationId == generationId)
      {
        if (geometryIdList.find(info->mGeometryId) == geometryIdList.end())
        {
          geometryIdList.insert(info->mGeometryId);
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentGeometryIdList(std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    geometryIdList.clear();

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];

      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        if (geometryIdList.find(info->mGeometryId) == geometryIdList.end())
        {
          geometryIdList.insert(info->mGeometryId);
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getGenerationIdListByGeometryId(T::GeometryId geometryId,std::set<uint>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    generationIdList.clear();

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];

      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGeometryId == geometryId)
      {
        if (generationIdList.find(info->mGenerationId) == generationIdList.end())
        {
          generationIdList.insert(info->mGenerationId);
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByFileId(uint fileId,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::file_message)
    {
      ContentInfo info;
      info.mFileId = fileId;
      info.mMessageIndex = 0;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);
      uint t = 0;
      if (idx >= 0)
        t = idx;

      while (t < mLength)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mFileId > fileId)
            return;

          if (info->mFileId == fileId)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);
          }
        }
        t++;
      }
      return;
    }


    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mFileId == fileId)
      {
        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(info->duplicate());
        else
          contentInfoList.addContentInfo(info);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByGeometryId(T::GeometryId geometryId,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGeometryId == geometryId)
      {
        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(info->duplicate());
        else
          contentInfoList.addContentInfo(info);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByRequestCounterKey(ulonglong key,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->getRequestCounterKey() == key)
      {
        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(info->duplicate());
        else
          contentInfoList.addContentInfo(info);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByForecastTime(std::string forecastTime,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::starttime_fmiId_fmiLevelId_level_file_message)
    {
      ContentInfo info;
      info.mForecastTime = forecastTime;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);
      uint t = 0;
      if (idx >= 0)
        t = idx;

      while (t < mLength)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mForecastTime > forecastTime)
            return;

          if (info->mForecastTime == forecastTime)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);
          }
        }
        t++;
      }
      return;
    }


    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mForecastTime == forecastTime)
      {
        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(info->duplicate());
        else
          contentInfoList.addContentInfo(info);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void ContentInfoList::getContentInfoListByGribParameterId(T::ParamId gribParameterId,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGribParameterId == gribParameterId)
      {
        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(info->duplicate());
        else
          contentInfoList.addContentInfo(info);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByGribParameterId(T::ParamId gribParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::gribId_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mGribParameterId = gribParameterId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mGribParameterId.c_str(),gribParameterId.c_str()) != 0)
        idx++;

      uint t = idx;

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mGribParameterId.c_str(),gribParameterId.c_str()) == 0)
          {
            if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
            {
              if (geometryId < 0  ||  info->mGeometryId == geometryId)
              {
                if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                    (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                    (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                    (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                {
                  if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                  {
                    if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                      next = info;

                    if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                    {
                      if (contentInfoList.getReleaseObjects())
                        contentInfoList.addContentInfo(info->duplicate());
                      else
                        contentInfoList.addContentInfo(info);
                    }
                  }
                }
              }
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  && (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  && (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::gribId_producer_generation_level_time);
      list.getContentInfoListByGribParameterId(gribParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByGribParameterIdAndGenerationId(uint producerId,uint generationId,T::ParamId gribParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::gribId_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mGribParameterId = gribParameterId;
      info.mProducerId = producerId;
      info.mGenerationId = generationId;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mGribParameterId.c_str(),gribParameterId.c_str()) != 0)
        idx++;

      uint t = idx;

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mGribParameterId.c_str(),gribParameterId.c_str()) == 0)
          {
            if (info->mGenerationId == generationId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                  {
                    if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                        next = info;

                      if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);
                      }
                    }
                  }
                }
              }
            }
            else
            {
              if (t > C_UINT(idx))
                t = mLength;
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  && (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0 &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  && (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::gribId_producer_generation_level_time);
      list.getContentInfoListByGribParameterIdAndGenerationId(producerId,generationId,gribParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByGribParameterIdAndProducerId(uint producerId,T::ParamId gribParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::gribId_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mGribParameterId = gribParameterId;
      info.mProducerId = producerId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mGribParameterId.c_str(),gribParameterId.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mGribParameterId.c_str(),gribParameterId.c_str()) == 0)
          {
            if (info->mProducerId == producerId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                  {
                    if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                        next = info;

                      if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);
                      }
                    }
                  }
                }
              }
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::gribId_producer_generation_level_time);
      list.getContentInfoListByGribParameterIdAndProducerId(producerId,gribParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterId(T::ParamId fmiParameterId,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mFmiParameterId == fmiParameterId)
      {
        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(info->duplicate());
        else
          contentInfoList.addContentInfo(info);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterId(T::ParamId fmiParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterId = fmiParameterId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mFmiParameterId.c_str(),fmiParameterId.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mFmiParameterId.c_str(),fmiParameterId.c_str()) == 0)
          {
            if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
            {
              if (geometryId < 0  ||  info->mGeometryId == geometryId)
              {
                if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                    (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                    (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                    (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                {
                  if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                  {
                    if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                      next = info;

                    if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                    {
                      if (contentInfoList.getReleaseObjects())
                        contentInfoList.addContentInfo(info->duplicate());
                      else
                        contentInfoList.addContentInfo(info);
                    }
                  }
                }
              }
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
          // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time);
      list.getContentInfoListByFmiParameterId(fmiParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterIdAndGenerationId(uint producerId,uint generationId,T::ParamId fmiParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterId = fmiParameterId;
      info.mProducerId = producerId;
      info.mGenerationId = generationId;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mFmiParameterId.c_str(),fmiParameterId.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mFmiParameterId.c_str(),fmiParameterId.c_str()) == 0)
          {
            if (info->mGenerationId == generationId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                  {
                    if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                        next = info;

                      if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);
                      }
                    }
                  }
                }
              }
            }
            else
            {
              if (t > C_UINT(idx))
                t = mLength;
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time);
      list.getContentInfoListByFmiParameterIdAndGenerationId(producerId,generationId,fmiParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterIdAndProducerId(uint producerId,T::ParamId fmiParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterId = fmiParameterId;
      info.mProducerId = producerId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mFmiParameterId.c_str(),fmiParameterId.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mFmiParameterId.c_str(),fmiParameterId.c_str()) == 0)
          {
            if (info->mProducerId == producerId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                  {
                    if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                        next = info;

                      if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);
                      }
                    }
                  }
                }
              }
            }
            else
              if (t > C_UINT(idx))
                t = mLength;
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time);
      list.getContentInfoListByFmiParameterIdAndProducerId(producerId,fmiParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void ContentInfoList::getContentInfoListByFmiParameterName(std::string fmiParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterName = fmiParameterName;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mFmiParameterName.c_str(),fmiParameterName.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      //printf("INDEX %u %s %u\n",idx,mArray[t]->mFmiParameterName.c_str(),mArray[t]->mParameterLevel);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          //printf("-- INDEX %u %s %u\n",idx,mArray[t]->mFmiParameterName.c_str(),mArray[t]->mParameterLevel);
          if (strcasecmp(info->mFmiParameterName.c_str(),fmiParameterName.c_str()) == 0)
          {
            if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
            {
              if (geometryId < 0  ||  info->mGeometryId == geometryId)
              {
                if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                    (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                    (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                    (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                {
                  if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                  {
                    if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                      next = info;

                    if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                    {
                      if (contentInfoList.getReleaseObjects())
                        contentInfoList.addContentInfo(info->duplicate());
                      else
                        contentInfoList.addContentInfo(info);
                    }
                  }
                }
              }
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time);
      list.getContentInfoListByFmiParameterName(fmiParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




ContentInfo* ContentInfoList::getContentInfoByFmiParameterNameAndGenerationId(uint producerId,uint generationId,std::string fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string forecastTime)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return nullptr;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    ContentInfo info;
    info.mProducerId = producerId;
    info.mGenerationId = generationId;
    info.mProducerId = producerId;
    info.mForecastTime = forecastTime;
    info.mFmiParameterName = fmiParameterName;
    info.mFmiParameterLevelId = parameterLevelId;
    info.mParameterLevel = level;
    info.mForecastType = forecastType;
    info.mForecastNumber = forecastNumber;
    info.mGeometryId = geometryId;

    //info.print(std::cout,0,0);
/*
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *cInfo = mArray[t];
      std::cout << cInfo->mFmiParameterName << ":" << cInfo->mParameterLevel << ":" << cInfo->mForecastTime << ":" << cInfo->mGenerationId << ":" << cInfo->mForecastType << ":" << cInfo->mForecastNumber << ":" << cInfo->mGeometryId << "\n";
    }

    exit(0);
*/
    int idx = getClosestIndexNoLock(mComparisonMethod,info);
    for (int t=0; t < 2; t++)
    {
      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr)
      {
        ContentInfo *cInfo = mArray[idx];
        //cInfo->print(std::cout,0,0);

        if (info.mGenerationId == cInfo->mGenerationId &&
            info.mForecastTime == cInfo->mForecastTime &&
            info.mFmiParameterName == cInfo->mFmiParameterName &&
            info.mFmiParameterLevelId == cInfo->mFmiParameterLevelId &&
            info.mParameterLevel == cInfo->mParameterLevel &&
            (info.mForecastType < 0 || info.mForecastType == cInfo->mForecastType) &&
            (info.mForecastNumber < 0 || info.mForecastNumber == cInfo->mForecastNumber) &&
            (info.mGeometryId < 0 || info.mGeometryId == cInfo->mGeometryId))
          return cInfo;

        idx++;
      }
    }

    return nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterNameAndGenerationId(uint producerId,uint generationId,std::string fmiParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string forecastTime,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterName = fmiParameterName;
      info.mProducerId = producerId;
      info.mGenerationId = generationId;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mFmiParameterName.c_str(),fmiParameterName.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prevTimeSameLevel = nullptr;
      ContentInfo *nextTimeSameLevel = nullptr;

      ContentInfo *sameTimePrevLevel = nullptr;
      ContentInfo *sameTimeNextLevel = nullptr;
      ContentInfo *prevTimePrevLevel = nullptr;
      ContentInfo *prevTimeNextLevel = nullptr;
      ContentInfo *nextTimePrevLevel = nullptr;
      ContentInfo *nextTimeNextLevel = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mFmiParameterName.c_str(),fmiParameterName.c_str()) == 0)
          {
            if (info->mGenerationId == generationId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                  {
                    if (info->mForecastTime < forecastTime)
                    {
                      if (info->mParameterLevel == level || parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE)
                      {
                        if (prevTimeSameLevel == nullptr || prevTimeSameLevel->mForecastTime < info->mForecastTime)
                        {
                          prevTimeSameLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel < level)
                      {
                        if (prevTimePrevLevel == nullptr || prevTimePrevLevel->mForecastTime < info->mForecastTime  || (prevTimePrevLevel->mForecastTime == info->mForecastTime  &&  prevTimePrevLevel->mParameterLevel < info->mParameterLevel))
                        {
                          prevTimePrevLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel > level)
                      {
                        if (prevTimeNextLevel == nullptr || prevTimeNextLevel->mForecastTime < info->mForecastTime  ||  (prevTimeNextLevel->mForecastTime == info->mForecastTime &&  prevTimeNextLevel->mParameterLevel > info->mParameterLevel))
                        {
                          prevTimeNextLevel = info;
                        }
                      }
                    }
                    else
                    if (info->mForecastTime > forecastTime)
                    {
                      if (info->mParameterLevel == level || parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE)
                      {
                        if (nextTimeSameLevel == nullptr)
                        {
                          nextTimeSameLevel = info;
                          t = mLength;
                        }
                      }
                      else
                      if (info->mParameterLevel < level)
                      {
                        if (nextTimePrevLevel == nullptr || nextTimePrevLevel->mForecastTime > info->mForecastTime || (nextTimePrevLevel->mForecastTime == info->mForecastTime  &&  nextTimePrevLevel->mParameterLevel < info->mParameterLevel))
                        {
                          nextTimePrevLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel > level)
                      {
                        if (nextTimeNextLevel == nullptr || nextTimePrevLevel->mForecastTime > info->mForecastTime || (nextTimeNextLevel->mForecastTime == info->mForecastTime &&  nextTimeNextLevel->mParameterLevel > info->mParameterLevel))
                        {
                          nextTimeNextLevel = info;
                          t = mLength;
                        }
                      }
                    }
                    else
                    if (info->mForecastTime == forecastTime)
                    {
                      if (info->mParameterLevel == level || parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);
                      }
                      else
                      if (info->mParameterLevel < level)
                      {
                        if (sameTimePrevLevel == nullptr || sameTimePrevLevel->mParameterLevel < info->mParameterLevel)
                        {
                          sameTimePrevLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel > level)
                      {
                        if (sameTimeNextLevel == nullptr || sameTimeNextLevel->mParameterLevel > info->mParameterLevel)
                        {
                          sameTimeNextLevel = info;
                          t = mLength;
                        }
                      }

                    }
                  }
                }
              }
            }
            else
            {
              if (t > C_UINT(idx))
                t = mLength;
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() > 0)
        return;


      if (prevTimeSameLevel != nullptr  &&  (prevTimeSameLevel->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prevTimeSameLevel->duplicate());
        else
          contentInfoList.addContentInfo(prevTimeSameLevel);
      }

      if (nextTimeSameLevel != nullptr  &&  (nextTimeSameLevel->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(nextTimeSameLevel->duplicate());
        else
          contentInfoList.addContentInfo(nextTimeSameLevel);
      }

      if (contentInfoList.getLength() == 2)
        return;


      if (sameTimePrevLevel != nullptr  && sameTimeNextLevel != nullptr)
      {
        if (sameTimePrevLevel != nullptr  &&  (sameTimePrevLevel->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          // We need to add the previous entry before the start time.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(sameTimePrevLevel->duplicate());
          else
            contentInfoList.addContentInfo(sameTimePrevLevel);
        }

        if (sameTimeNextLevel != nullptr  &&  (sameTimeNextLevel->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          // We need to add the next entry after the end time.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(sameTimeNextLevel->duplicate());
          else
            contentInfoList.addContentInfo(sameTimeNextLevel);
        }
      }

      if (contentInfoList.getLength() == 2)
        return;


      if (prevTimePrevLevel != nullptr  &&  prevTimeNextLevel != nullptr)
      {
        if (prevTimePrevLevel != nullptr  &&  (prevTimePrevLevel->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          // We need to add the previous entry before the start time.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prevTimePrevLevel->duplicate());
          else
            contentInfoList.addContentInfo(prevTimePrevLevel);
        }

        if (prevTimeNextLevel != nullptr  &&  (prevTimeNextLevel->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          // We need to add the previous entry before the start time.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prevTimeNextLevel->duplicate());
          else
            contentInfoList.addContentInfo(prevTimeNextLevel);
        }
      }

      if (nextTimePrevLevel != nullptr  &&  nextTimeNextLevel != nullptr)
      {
        if (nextTimePrevLevel != nullptr  &&  (nextTimePrevLevel->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          // We need to add the next entry after the end time.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(nextTimePrevLevel->duplicate());
          else
            contentInfoList.addContentInfo(nextTimePrevLevel);
        }

        if (nextTimeNextLevel != nullptr  &&  (nextTimeNextLevel->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          // We need to add the next entry after the end time.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(nextTimeNextLevel->duplicate());
          else
            contentInfoList.addContentInfo(nextTimeNextLevel);
        }
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time);
      list.getContentInfoListByFmiParameterNameAndGenerationId(producerId,generationId,fmiParameterName,parameterLevelIdType,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterNameAndGenerationId(uint producerId,uint generationId,std::string fmiParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterName = fmiParameterName;
      info.mGenerationId = generationId;
      info.mProducerId = producerId;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mFmiParameterName.c_str(),fmiParameterName.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mFmiParameterName.c_str(),fmiParameterName.c_str()) == 0)
          {
            if (info->mGenerationId == generationId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                  {
                    if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                        next = info;

                      if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);
                      }
                    }
                  }
                }
              }
            }
            else
            {
              if (t > C_UINT(idx))
                t = mLength;
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time);
      list.getContentInfoListByFmiParameterNameAndGenerationId(producerId,generationId,fmiParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterNameAndProducerId(uint producerId,std::string fmiParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterName = fmiParameterName;
      info.mProducerId = producerId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mFmiParameterName.c_str(),fmiParameterName.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mFmiParameterName.c_str(),fmiParameterName.c_str()) == 0)
          {
            if (info->mProducerId == producerId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                  {
                    if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                        next = info;

                      if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);
                      }
                    }
                  }
                }
              }
            }
            else
            {
              if (t > C_UINT(idx))
                t = mLength;
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time);
      list.getContentInfoListByFmiParameterNameAndProducerId(producerId,fmiParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByNewbaseParameterId(T::ParamId newbaseParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseId_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterId = newbaseParameterId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) == 0)
          {
            if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
            {
              if (geometryId < 0  ||  info->mGeometryId == geometryId)
              {
                if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                    (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                    (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                    (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                {
                  if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                  {
                    if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                      next = info;

                    if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                    {
                      if (contentInfoList.getReleaseObjects())
                        contentInfoList.addContentInfo(info->duplicate());
                      else
                        contentInfoList.addContentInfo(info);
                    }
                  }
                }
              }
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::newbaseId_producer_generation_level_time);
      list.getContentInfoListByNewbaseParameterId(newbaseParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByNewbaseParameterIdAndGenerationId(uint producerId,uint generationId,T::ParamId newbaseParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseId_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterId = newbaseParameterId;
      info.mGenerationId = generationId;
      info.mProducerId = producerId;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) == 0)
          {
            if (info->mGenerationId == generationId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                  {
                    if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                        next = info;

                      if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);
                      }
                    }
                  }
                }
              }
            }
            else
            {
              if (t > C_UINT(idx))
                t = mLength;
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::newbaseId_producer_generation_level_time);
      list.getContentInfoListByNewbaseParameterIdAndGenerationId(producerId,generationId,newbaseParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByNewbaseParameterIdAndProducerId(uint producerId,T::ParamId newbaseParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseId_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterId = newbaseParameterId;
      info.mProducerId = producerId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) == 0)
          {
            if (info->mProducerId == producerId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                  {
                    if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                        next = info;

                      if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);
                      }
                    }
                  }
                }
              }
            }
            else
            {
              if (t > C_UINT(idx))
                t = mLength;
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::newbaseId_producer_generation_level_time);
      list.getContentInfoListByNewbaseParameterIdAndProducerId(producerId,newbaseParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void ContentInfoList::getContentInfoListByNewbaseParameterName(std::string newbaseParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseName_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterName = newbaseParameterName;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) == 0)
          {
            if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
            {
              if (geometryId < 0  ||  info->mGeometryId == geometryId)
              {
                if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                    (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                    (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                    (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                {
                  if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                  {
                    if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                      next = info;

                    if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                    {
                      if (contentInfoList.getReleaseObjects())
                        contentInfoList.addContentInfo(info->duplicate());
                      else
                        contentInfoList.addContentInfo(info);
                    }
                  }
                }
              }
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::newbaseName_producer_generation_level_time);
      list.getContentInfoListByNewbaseParameterName(newbaseParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByNewbaseParameterNameAndGenerationId(uint producerId,uint generationId,std::string newbaseParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseName_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterName = newbaseParameterName;
      info.mProducerId = producerId;
      info.mGenerationId = generationId;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) == 0)
          {
            if (info->mGenerationId == generationId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                  {
                    if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                        next = info;

                      if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);
                      }
                    }
                  }
                }
              }
            }
            else
            {
              if (t > C_UINT(idx))
                t = mLength;
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::newbaseName_producer_generation_level_time);
      list.getContentInfoListByNewbaseParameterNameAndGenerationId(producerId,generationId,newbaseParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByNewbaseParameterNameAndProducerId(uint producerId,std::string newbaseParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseName_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterName = newbaseParameterName;
      info.mProducerId = producerId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) == 0)
          {
            if (info->mProducerId == producerId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                  {
                    if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                        next = info;

                      if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);
                      }
                    }
                  }
                }
              }
            }
            else
            {
              if (t > C_UINT(idx))
                t = mLength;
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::newbaseName_producer_generation_level_time);
      list.getContentInfoListByNewbaseParameterNameAndProducerId(producerId,newbaseParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByCdmParameterId(T::ParamId cdmParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmId_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterId = cdmParameterId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mCdmParameterId.c_str(),cdmParameterId.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mCdmParameterId.c_str(),cdmParameterId.c_str()) == 0)
          {
            if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
            {
              if (geometryId < 0  ||  info->mGeometryId == geometryId)
              {
                if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                    (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                    (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                    (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                {
                  if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                  {
                    if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                      next = info;

                    if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                    {
                      if (contentInfoList.getReleaseObjects())
                        contentInfoList.addContentInfo(info->duplicate());
                      else
                        contentInfoList.addContentInfo(info);
                    }
                  }
                }
              }
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::cdmId_producer_generation_level_time);
      list.getContentInfoListByCdmParameterId(cdmParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByCdmParameterIdAndGenerationId(uint producerId,uint generationId,T::ParamId cdmParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmId_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterId = cdmParameterId;
      info.mProducerId = producerId;
      info.mGenerationId = generationId;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mCdmParameterId.c_str(),cdmParameterId.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mCdmParameterId.c_str(),cdmParameterId.c_str()) == 0)
          {
            if (info->mGenerationId == generationId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                  {
                    if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                        next = info;

                      if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);
                      }
                    }
                  }
                }
              }
            }
            else
            {
              if (t > C_UINT(idx))
                t = mLength;
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::cdmId_producer_generation_level_time);
      list.getContentInfoListByCdmParameterIdAndGenerationId(producerId,generationId,cdmParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByCdmParameterIdAndProducerId(uint producerId,T::ParamId cdmParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmId_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterId = cdmParameterId;
      info.mProducerId = producerId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mCdmParameterId.c_str(),cdmParameterId.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mCdmParameterId.c_str(),cdmParameterId.c_str()) == 0)
          {
            if (info->mProducerId == producerId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                  {
                    if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                        next = info;

                      if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);
                      }
                    }
                  }
                }
              }
            }
            else
            {
              if (t > C_UINT(idx))
                t = mLength;
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time. Notice that this must be
        // done in all requested levels.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time. Notice that this must be
        // done in all requested levels.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::cdmId_producer_generation_level_time);
      list.getContentInfoListByCdmParameterIdAndProducerId(producerId,cdmParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByCdmParameterName(std::string cdmParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmName_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterName = cdmParameterName;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mCdmParameterName.c_str(),cdmParameterName.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mCdmParameterName.c_str(),cdmParameterName.c_str()) == 0)
          {
            if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
            {
              if (geometryId < 0  ||  info->mGeometryId == geometryId)
              {
                if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                    (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                    (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                    (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                {
                  if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                  {
                    if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                      next = info;

                    if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                    {
                      if (contentInfoList.getReleaseObjects())
                        contentInfoList.addContentInfo(info->duplicate());
                      else
                        contentInfoList.addContentInfo(info);
                    }
                  }
                }
              }
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  && (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::cdmName_producer_generation_level_time);
      list.getContentInfoListByCdmParameterName(cdmParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByCdmParameterNameAndGenerationId(uint producerId,uint generationId,std::string cdmParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmName_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterName = cdmParameterName;
      info.mProducerId = producerId;
      info.mGenerationId = generationId;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mCdmParameterName.c_str(),cdmParameterName.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mCdmParameterName.c_str(),cdmParameterName.c_str()) == 0)
          {
            if (info->mGenerationId == generationId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                  {
                    if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                        next = info;

                      if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);
                      }
                    }
                  }
                }
              }
            }
            else
            {
              if (t > C_UINT(idx))
                t = mLength;
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  &&  (prev->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  &&  (next->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::cdmName_producer_generation_level_time);
      list.getContentInfoListByCdmParameterNameAndGenerationId(producerId,generationId,cdmParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByCdmParameterNameAndProducerId(uint producerId,std::string cdmParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmName_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterName = cdmParameterName;
      info.mProducerId = producerId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcasecmp(mArray[idx]->mCdmParameterName.c_str(),cdmParameterName.c_str()) != 0)
        idx++;

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcasecmp(info->mCdmParameterName.c_str(),cdmParameterName.c_str()) == 0)
          {
            if (info->mProducerId == producerId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE ||  parameterLevelId == 0) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdTypeValue::FMI || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB1 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdTypeValue::GRIB2 || parameterLevelIdType == T::ParamLevelIdTypeValue::ANY)))
                  {
                    if (parameterLevelIdType == T::ParamLevelIdTypeValue::IGNORE || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTime < startTime  && (prev == nullptr || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == nullptr || next->mForecastTime > info->mForecastTime))
                        next = info;

                      if (info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);
                      }
                    }
                  }
                }
              }
            }
            else
            {
              if (t > C_UINT(idx))
                t = mLength;
            }
          }
          else
          {
            if (t > C_UINT(idx))
              t = mLength;
          }
        }
        t++;
      }

      if (contentInfoList.getLength() == 0)
        return;

      if (prev != nullptr  && (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
      {
        // We need to add the previous entry before the start time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(prev->duplicate());
        else
          contentInfoList.addContentInfo(prev);
      }

      if (next != nullptr  && (requestFlags & ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
      {
        // We need to add the next entry after the end time.

        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(next->duplicate());
        else
          contentInfoList.addContentInfo(next);
      }
    }
    else
    {
      // ### The content list is not sorted as we want. Let's take a copy of the list and sort it so that
      // ### we can execute the search. This is not the most efficient way to do it, but this functionality
      // ### should not be needed very often.

      ContentInfoList list;
      list.setReleaseObjects(false);
      list = *this;
      list.sort(ContentInfo::ComparisonMethod::cdmName_producer_generation_level_time);
      list.getContentInfoListByCdmParameterNameAndProducerId(producerId,cdmParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByProducerId(uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      // If the records are not sorted according to fileId and messageIndex then the startFileId parameter
      // is used as the start index in the list.

      for (uint t=startFileId; t<mLength; t++)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mProducerId == producerId)
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);

          if (contentInfoList.getLength() >= maxRecords)
            return;
        }
      }
      return;
    }

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (startIdx < 0)
      startIdx = 0;

    for (uint t=startIdx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mProducerId == producerId && (info->mFileId > startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex >= startMessageIndex)))
      {
        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(info->duplicate());
        else
          contentInfoList.addContentInfo(info);

        if (contentInfoList.getLength() >= maxRecords)
          return;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByProducerId(uint producerId,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    if (mComparisonMethod != ContentInfo::ComparisonMethod::producer_file_message)
    {
      // If the records are not sorted according to fileId and messageIndex then the startFileId parameter
      // is used as the start index in the list.

      for (uint t=0; t<mLength; t++)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mProducerId == producerId)
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);
        }
      }
      return;
    }

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    ContentInfo contentInfo;
    contentInfo.mProducerId = producerId;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (startIdx < 0)
      startIdx = 0;

    for (uint t=startIdx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        if (info->mProducerId == producerId)
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);
        }
        else
        {
          if (C_INT(t) > startIdx)
            return;
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByGenerationId(uint producerId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      // If the records are not sorted according to fileId and messageIndex then the startFileId parameter
      // is used as the start index in the list.

      for (uint t=startFileId; t<mLength; t++)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGenerationId == generationId)
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);

          if (contentInfoList.getLength() >= maxRecords)
            return;
        }
      }
      return;
    }

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (startIdx < 0)
      startIdx = 0;

    for (uint t=startIdx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGenerationId == generationId && (info->mFileId > startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex >= startMessageIndex)))
      {
        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(info->duplicate());
        else
          contentInfoList.addContentInfo(info);

        if (contentInfoList.getLength() >= maxRecords)
          return;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByGenerationAndGeometryId(uint producerId,uint generationId,T::GeometryId geometryId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      // If the records are not sorted according to fileId and messageIndex then the startFileId parameter
      // is used as the start index in the list.

      for (uint t=startFileId; t<mLength; t++)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGenerationId == generationId  &&  info->mGeometryId == geometryId)
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);

          if (contentInfoList.getLength() >= maxRecords)
            return;
        }
      }
      return;
    }

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (startIdx < 0)
      startIdx = 0;

    for (uint t=startIdx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGenerationId == generationId  &&  info->mGeometryId == geometryId && (info->mFileId > startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex >= startMessageIndex)))
      {
        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(info->duplicate());
        else
          contentInfoList.addContentInfo(info);

        if (contentInfoList.getLength() >= maxRecords)
          return;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByGenerationId(uint producerId,uint generationId,std::string startTime,std::string endTime,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGenerationId == generationId  &&  info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
      {
        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(info->duplicate());
        else
          contentInfoList.addContentInfo(info);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByServerId(uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    unsigned long long sf = 0;
    if (serverId >= 1  &&  serverId <= 64)
      sf = (unsigned long long)1 << (serverId-1);

    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      // If the records are not sorted according to fileId and messageIndex then the startFileId parameter
      // is used as the start index in the list.

      for (uint t=startFileId; t<mLength; t++)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (info->mServerFlags & sf) != 0)
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);

          if (contentInfoList.getLength() >= maxRecords)
            return;
        }
      }
      return;
    }


    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (startIdx < 0)
      startIdx = 0;

    for (uint t=startIdx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  (info->mServerFlags & sf) != 0 && (info->mFileId > startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex >= startMessageIndex)))
      {
        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(info->duplicate());
        else
          contentInfoList.addContentInfo(info);

        if (contentInfoList.getLength() >= maxRecords)
          return;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListBySourceId(uint sourceId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      // If the records are not sorted according to fileId and messageIndex then the startFileId parameter
      // is used as the start index in the list.

      for (uint t=startFileId; t<mLength; t++)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mSourceId == sourceId)
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);

          if (contentInfoList.getLength() >= maxRecords)
            return;
        }
      }
    }


    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (startIdx < 0)
      startIdx = 0;

    for (uint t=startIdx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mSourceId == sourceId && (info->mFileId > startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex >= startMessageIndex)))
      {
        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(info->duplicate());
        else
          contentInfoList.addContentInfo(info);

        if (contentInfoList.getLength() >= maxRecords)
          return;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getContentInfoListByServerAndFileId(uint serverId,uint fileId,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    unsigned long long sf = 0;
    if (serverId >= 1  &&  serverId <= 64)
      sf = (unsigned long long)1 << (serverId-1);

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::file_message)
    {
      ContentInfo contentInfo;
      contentInfo.mFileId = fileId;
      contentInfo.mMessageIndex = 0;
      int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
      if (startIdx < 0)
        startIdx = 0;

      for (uint t=startIdx; t<mLength; t++)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  info->mFileId > fileId)
          return;

        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mFileId == fileId  &&  (info->mServerFlags & sf) != 0)
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);
        }
      }
      return;
    }


    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mFileId == fileId  &&  (info->mServerFlags & sf) != 0)
      {
        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(info->duplicate());
        else
          contentInfoList.addContentInfo(info);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getFmiParamLevelIdListByFmiParameterId(T::ParamId fmiParameterId,std::vector<T::ParamLevelId>& paramLevelIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        uint vLen = paramLevelIdList.size();
        uint c = 0;
        while (c < vLen)
        {
          if (info->mFmiParameterId == fmiParameterId)
          {
            if (paramLevelIdList[c] == info->mFmiParameterLevelId)
            {
              c = vLen;
            }
            else
            if (paramLevelIdList[c] > info->mFmiParameterLevelId)
            {
              paramLevelIdList.insert(paramLevelIdList.begin() + c,info->mFmiParameterLevelId);
              c = vLen;
            }
          }
          c++;
        }
        if (c == vLen)
        {
          if (info->mFmiParameterId == fmiParameterId)
            paramLevelIdList.push_back(info->mFmiParameterLevelId);
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getParamLevelListByFmiLevelId(T::ParamLevelId paramLevelId,std::set<T::ParamLevel>& paramLevelList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mFmiParameterLevelId == paramLevelId)
      {
        if (paramLevelList.find(info->mParameterLevel) == paramLevelList.end())
          paramLevelList.insert(info->mParameterLevel);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getParamLevelInfoListByFmiParameterId(T::ParamId fmiParameterId,ParameterLevelInfoList& parameterLevelInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mFmiParameterId == fmiParameterId)
      {
        ParameterLevelInfo *pInfo = parameterLevelInfoList.getParameterLevelInfo(T::ParamKeyTypeValue::FMI_ID,
            info->mFmiParameterId,T::ParamLevelIdTypeValue::FMI,info->mFmiParameterLevelId,info->mParameterLevel);

        if (pInfo == nullptr)
        {
          pInfo = new ParameterLevelInfo(T::ParamKeyTypeValue::FMI_ID,info->mFmiParameterId,T::ParamLevelIdTypeValue::FMI,info->mFmiParameterLevelId,info->mParameterLevel);
          parameterLevelInfoList.addParameterLevelInfo(pInfo);
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getForecastTimeList(std::set<std::string>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        if (info != nullptr)
        {
          if (forecastTimeList.find(info->mForecastTime) == forecastTimeList.end())
          {
            forecastTimeList.insert(info->mForecastTime);
          }
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getForecastTimeListByGenerationId(uint producerId,uint generationId,std::set<std::string>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGenerationId == generationId)
        {
          if (forecastTimeList.find(info->mForecastTime) == forecastTimeList.end())
          {
            forecastTimeList.insert(info->mForecastTime);
          }
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getForecastTimeListByGenerationAndGeometry(uint producerId,uint generationId,T::GeometryId geometryId,std::set<std::string>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGenerationId == generationId  &&  info->mGeometryId == geometryId)
        {
          if (forecastTimeList.find(info->mForecastTime) == forecastTimeList.end())
          {
            forecastTimeList.insert(info->mForecastTime);
          }
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::getForecastTimeListByProducerId(uint producerId,std::set<std::string>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mProducerId == producerId)
        {
          if (forecastTimeList.find(info->mForecastTime) == forecastTimeList.end())
          {
            forecastTimeList.insert(info->mForecastTime);
          }
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::getLength() const
{
  FUNCTION_TRACE
  try
  {
    return mLength;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::setModificationLockPtr(ModificationLock* modificationLockPtr)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ModificationLock*  ContentInfoList::getModificationLockPtr()
{
  FUNCTION_TRACE
  try
  {
    return mModificationLockPtr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::getSize() const
{
  FUNCTION_TRACE
  try
  {
    return mSize;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::lock()
{
  FUNCTION_TRACE
  try
  {
    return mModificationLockPtr->lock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void ContentInfoList::unlock()
{
  FUNCTION_TRACE
  try
  {
    return mModificationLockPtr->unlock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





bool ContentInfoList::getReleaseObjects()
{
  FUNCTION_TRACE
  try
  {
    return mReleaseObjects;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::setReleaseObjects(bool releaseObjects)
{
  FUNCTION_TRACE
  try
  {
    mReleaseObjects = releaseObjects;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint ContentInfoList::getComparisonMethod()
{
  FUNCTION_TRACE
  try
  {
    return mComparisonMethod;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::setComparisonMethod(uint comparisonMethod)
{
  FUNCTION_TRACE
  try
  {
    if (mComparisonMethod == comparisonMethod)
      return;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    mComparisonMethod = comparisonMethod;
    if (mArray == nullptr ||  mLength == 0)
      return;

    contentInfo_comparisonMethod = comparisonMethod;

    qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void ContentInfoList::sort(uint comparisonMethod)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr || mLength == 0)
      return;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    mComparisonMethod = comparisonMethod;

    AutoThreadLock globalLock(&ContentInfoList_sortLock);
    contentInfo_comparisonMethod = comparisonMethod;

    qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::writeToFile(std::string filename)
{
  FUNCTION_TRACE
  try
  {
    writeToFile(filename,"we");
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::writeToFile(std::string filename,const char *filemode)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    FILE *file = fopen(filename.c_str(),filemode);
    if (file == nullptr)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot create the file!");
      exception.addParameter("Filename",filename);
      throw exception;
    }

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        fprintf(file,"%s\n",info->getCsv().c_str());
    }

    fclose(file);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ContentInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    stream << space(level) << "ContentInfoList\n";
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        info->print(stream,level+1,optionFlags);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




}
}
