#include "ContentInfoList.h"
#include "RequestFlags.h"

#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/GeneralDefinitions.h>
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


int contentInfo_compare_1(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(1,(*obj2));
  }
  return 0;
}



int contentInfo_compare_2(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(2,(*obj2));
  }
  return 0;
}



int contentInfo_compare_3(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(3,(*obj2));
  }
  return 0;
}


int contentInfo_compare_4(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(4,(*obj2));
  }
  return 0;
}


int contentInfo_compare_5(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(5,(*obj2));
  }
  return 0;
}



int contentInfo_compare_6(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(6,(*obj2));
  }
  return 0;
}



int contentInfo_compare_7(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(7,(*obj2));
  }
  return 0;
}



int contentInfo_compare_8(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(8,(*obj2));
  }
  return 0;
}




int contentInfo_compare_9(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(9,(*obj2));
  }
  return 0;
}



int contentInfo_compare_10(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(10,(*obj2));
  }
  return 0;
}




int contentInfo_compare_11(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(11,(*obj2));
  }
  return 0;
}



int contentInfo_compare_12(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(12,(*obj2));
  }
  return 0;
}



int contentInfo_compare_13(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(13,(*obj2));
  }
  return 0;
}


int contentInfo_compare_14(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(14,(*obj2));
  }
  return 0;
}


int contentInfo_compare_15(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(15,(*obj2));
  }
  return 0;
}



int contentInfo_compare_16(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(16,(*obj2));
  }
  return 0;
}



int contentInfo_compare_17(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(17,(*obj2));
  }
  return 0;
}



int contentInfo_compare_18(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(18,(*obj2));
  }
  return 0;
}




int contentInfo_compare_19(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(19,(*obj2));
  }
  return 0;
}



int contentInfo_compare_20(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    ContentInfoPtr *obj1 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val1));
    ContentInfoPtr *obj2 = const_cast<ContentInfoPtr*>(reinterpret_cast<const ContentInfoPtr *>(_val2));

    return (*obj1)->compare(20,(*obj2));
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





ContentInfoList::ContentInfoList(ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    mModificationLockPtr = &mModificationLock;
    mModificationLock.setLockingEnabled(false);
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
        mArray[t] = nullptr;
        ContentInfo *info = contentInfoList.getContentInfoByIndexNoCheck(t);
        if (info != nullptr)
          mArray[t] = info->duplicate();
      }
    }
    if (contentInfoList.getModificationLockPtr() != mModificationLockPtr)
      contentInfoList.unlock();
  }
  catch (...)
  {
    contentInfoList.unlock();
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ContentInfoList::ContentInfoList(const ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    mModificationLockPtr = &mModificationLock;
    mModificationLock.setLockingEnabled(false);

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
        mArray[t] = nullptr;
        ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (info != nullptr)
          mArray[t] = info->duplicate();
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ContentInfoList::~ContentInfoList()
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr)
      return;

    AutoWriteLock lock(mModificationLockPtr);
    if (mReleaseObjects  && mLength > 0)
    {
      for (uint t=0; t<mLength; t++)
      {
        if (mArray[t] != nullptr)
          delete mArray[t];
      }
    }

    delete[] mArray;
    mLength = 0;
    mArray = nullptr;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
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

    AutoWriteLock lock(mModificationLockPtr);

    if (contentInfoList.getModificationLockPtr() != mModificationLockPtr)
      contentInfoList.lock();

    mArray = nullptr;
    mSize = contentInfoList.getSize();
    mLength = contentInfoList.getLength();
    mComparisonMethod = contentInfoList.mComparisonMethod;
    uint p = 0;
    if (mSize > 0)
    {
      mArray = new ContentInfoPtr[mSize];
      for (uint t=0; t<mSize; t++)
      {
        mArray[t] = nullptr;
        ContentInfo *info = contentInfoList.getContentInfoByIndexNoCheck(t);
        if (info != nullptr && (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (mReleaseObjects)
            mArray[p] = info->duplicate();
          else
            mArray[p] = info;

          p++;
        }
      }
    }
    if (contentInfoList.getModificationLockPtr() != mModificationLockPtr)
      contentInfoList.unlock();

    mLength = p;
    return *this;
  }
  catch (...)
  {
    contentInfoList.unlock();
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    AutoWriteLock lock(mModificationLockPtr);
    mArray = nullptr;
    mSize = contentInfoList.getSize();
    mLength = contentInfoList.getLength();
    mComparisonMethod = contentInfoList.mComparisonMethod;
    uint p = 0;
    if (mSize > 0)
    {
      mArray = new ContentInfoPtr[mSize];
      for (uint t=0; t<mSize; t++)
      {
        mArray[t] = nullptr;
        ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (info != nullptr && (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (mReleaseObjects)
            mArray[t] = info->duplicate();
          else
            mArray[t] = info;

          p++;
        }
      }
    }
    mLength = p;
    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ContentInfo* ContentInfoList::addContentInfo(ContentInfo *contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (contentInfo == nullptr)
      throw Fmi::Exception(BCP,"The 'contentInfo' parameter points to NULL!");

    if (contentInfo->mForecastTimeUTC == 0)
    {
      try
      {
        contentInfo->mForecastTimeUTC = utcTimeToTimeT(contentInfo->getForecastTime());
      }
      catch (...)
      {
      }
    }

    AutoWriteLock lock(mModificationLockPtr);

    if (mArray == nullptr  ||  mLength == mSize)
    {
      increaseSize(mSize + mSize/4 + 100);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::addContentInfoList(ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr);
    addContentInfoListNoLock(contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::addContentInfoListNoLock(ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    uint len1 = mLength;
    uint len2 = contentInfoList.getLength();

    for (uint t=0; t<len2; t++)
    {
      ContentInfo *cInfo = contentInfoList.getContentInfoByIndex(t);
      if (cInfo  && cInfo->mForecastTimeUTC == 0)
      {
        try
        {
          cInfo->mForecastTimeUTC = utcTimeToTimeT(cInfo->getForecastTime());
        }
        catch (...)
        {
        }
      }
    }

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

    uint c = 0;

    for (uint t=0; t<newSize; t++)
    {
      newArray[t] = nullptr;
      ContentInfo *cInfo1 = nullptr;
      ContentInfo *cInfo2 = nullptr;

      if (a < len1)
        cInfo1 = mArray[a];

      if (b < len2)
        cInfo2 = contentInfoList.getContentInfoByIndex(b);

      if (cInfo1 == nullptr  &&  cInfo2 == nullptr)
      {
        a++;
        b++;
        newArray[t] = nullptr;
      }
      else
      if (cInfo1 != nullptr  &&  cInfo2 == nullptr)
      {
        newArray[c] = cInfo1;
        c++;
        a++;
      }
      else
      if (cInfo1 == nullptr  &&  cInfo2 != nullptr)
      {
        if (mReleaseObjects)
          newArray[c] = cInfo2->duplicate();
        else
          newArray[c] = cInfo2;

        c++;
        b++;
      }
      else
      if (cInfo1 != nullptr  &&  cInfo2 != nullptr)
      {
        if (cInfo2->compare(mComparisonMethod,cInfo1) > 0)
        {
          newArray[c] = cInfo1;
          a++;
          c++;
        }
        else
        {
          if (mReleaseObjects)
            newArray[c] = cInfo2->duplicate();
          else
            newArray[c] = cInfo2;

          b++;
          c++;
        }
      }
    }

    mSize = newSize;
    mLength = c;
    delete[] mArray;
    mArray = newArray;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::increaseSize(uint newSize)
{
  FUNCTION_TRACE
  try
  {
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::clear()
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr)
      return;

    AutoWriteLock lock(mModificationLockPtr);
    if (mReleaseObjects &&  mLength > 0)
    {
      for (uint t=0; t<mSize; t++)
      {
        if (mArray[t] != nullptr)
        {
          delete mArray[t];
          mArray[t] = nullptr;
        }
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





bool ContentInfoList::containsSameForecastTimes()
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return false;

    std::set<time_t> timeList;

    AutoReadLock lock(mModificationLockPtr);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        if (timeList.find(info->mForecastTimeUTC) != timeList.end())
          return true;

        timeList.insert(info->mForecastTimeUTC);
      }
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ContentInfoList::deleteContentInfoByFileId(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ContentInfoList::deleteMarkedContent()
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ContentInfoList::markDeletedByFileId(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoReadLock lock(mModificationLockPtr);

    uint cnt = 0;
    if (mComparisonMethod == ContentInfo::ComparisonMethod::file_message)
    {
      ContentInfo searchInfo;
      searchInfo.mFileId = fileId;
      searchInfo.mMessageIndex = 0;
      int idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);
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


    // The array is not sorted.

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mFileId == fileId)
        {
          info->mFlags = info->mFlags | T::ContentInfo::Flags::DeletedContent;
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





uint ContentInfoList::markDeletedByFileIdAndMessageIndex(uint fileId,uint messageIndex)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr);

    ContentInfo *info = getContentInfoByFileIdAndMessageIndexNoLock(fileId,messageIndex);
    if (info != nullptr)
    {
      info->mFlags = info->mFlags | T::ContentInfo::Flags::DeletedContent;
      return 1;
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






uint ContentInfoList::markDeleted()
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoReadLock lock(mModificationLockPtr);

    uint cnt = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        info->mFlags = info->mFlags | T::ContentInfo::Flags::DeletedContent;
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





uint ContentInfoList::markDeletedByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoReadLock lock(mModificationLockPtr);

    uint cnt = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mGenerationId == generationId)
        {
          info->mFlags = info->mFlags | T::ContentInfo::Flags::DeletedContent;
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





uint ContentInfoList::markDeletedByGenerationAndGeometryId(uint generationId,T::GeometryId geometryId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoReadLock lock(mModificationLockPtr);

    uint cnt = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mGenerationId == generationId &&  info->mGeometryId == geometryId)
        {
          info->mFlags = info->mFlags | T::ContentInfo::Flags::DeletedContent;
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




uint ContentInfoList::markDeletedByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoReadLock lock(mModificationLockPtr);

    uint cnt = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mProducerId == producerId)
        {
          info->mFlags = info->mFlags | T::ContentInfo::Flags::DeletedContent;
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





uint ContentInfoList::markDeletedBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoReadLock lock(mModificationLockPtr);

    uint cnt = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mSourceId == sourceId)
        {
          info->mFlags = info->mFlags | T::ContentInfo::Flags::DeletedContent;
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





uint ContentInfoList::deleteContentInfo(ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ContentInfoList::deleteContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ContentInfoList::deleteContentInfoByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getLevelInfoList(T::LevelInfoList& levelInfoList)
{
  FUNCTION_TRACE
  try
  {
    levelInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoWriteLock lock(mModificationLockPtr);

    std::set<std::size_t> list;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        std::size_t seed = 0;
        boost::hash_combine(seed,info->mProducerId);
        boost::hash_combine(seed,info->getFmiParameterName());
        boost::hash_combine(seed,info->mFmiParameterLevelId);
        //boost::hash_combine(seed,info->mGrib1ParameterLevelId);
        //boost::hash_combine(seed,info->mGrib2ParameterLevelId);
        boost::hash_combine(seed,info->mParameterLevel);

        if (list.find(seed) == list.end())
        {
          list.insert(seed);
          levelInfoList.addLevelInfo(new T::LevelInfo(info->mProducerId,info->getFmiParameterName(),info->mFmiParameterLevelId,info->mParameterLevel));
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ContentInfoList::deleteContentInfoByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ContentInfoList::deleteContentInfoByGenerationIdList(std::set<uint>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ContentInfoList::deleteContentInfoByGenerationAndGeometry(uint generationId,T::GeometryId geometryId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ContentInfoList::deleteContentInfoByGenerationGeometryAndForecastTime(uint generationId,T::GeometryId geometryId,const std::string& forecastTime)
{
  FUNCTION_TRACE
  try
  {
    time_t forecastTimeUTC = utcTimeToTimeT(forecastTime);
    return deleteContentInfoByGenerationGeometryAndForecastTime(generationId,geometryId,forecastTimeUTC);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ContentInfoList::deleteContentInfoByGenerationGeometryAndForecastTime(uint generationId,T::GeometryId geometryId,time_t forecastTimeUTC)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr  &&  ((info->mGenerationId == generationId  &&  info->mGeometryId == geometryId  &&  info->mForecastTimeUTC == forecastTimeUTC)  ||  (info->mFlags & T::ContentInfo::Flags::DeletedContent) != 0))
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ContentInfoList::deleteContentInfoBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ContentInfoList::deleteContentInfoByFileIdList(std::set<uint>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::keepContentInfoByGeometryIdList(std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoWriteLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::keepContentInfoByGeometryId(T::GeometryId geometryId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoWriteLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::keepContentInfoByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoWriteLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::keepContentInfoByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoWriteLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::keepContentInfoBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoWriteLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentInfoList::getContentListByForecastTime(const std::string& forecastTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    time_t forecastTimeUTC = utcTimeToTimeT(forecastTime);
    getContentListByForecastTime(forecastTimeUTC,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentListByForecastTime(time_t forecastTimeUTC,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    ContentInfo *prevInfo = nullptr;
    ContentInfo *nextInfo = nullptr;


    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        if (info->mForecastTimeUTC <= forecastTimeUTC)
        {
          if (prevInfo == nullptr  ||  info->mForecastTimeUTC > prevInfo->mForecastTimeUTC)
            prevInfo = info;
        }
        else
        {
          if (nextInfo == nullptr  ||  info->mForecastTimeUTC < nextInfo->mForecastTimeUTC)
            nextInfo = info;
        }
      }
    }

    if (prevInfo != nullptr  &&  (nextInfo != nullptr  ||  prevInfo->mForecastTimeUTC == forecastTimeUTC))
    {
      if (contentInfoList.getReleaseObjects())
        contentInfoList.addContentInfo(prevInfo->duplicate());
      else
        contentInfoList.addContentInfo(prevInfo);

      if (nextInfo != nullptr  &&  prevInfo->mForecastTimeUTC != forecastTimeUTC)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






int ContentInfoList::getClosestIndex(uint comparisonMethod,ContentInfo& contentInfo)
{
  //FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoReadLock lock(mModificationLockPtr);
    return getClosestIndexNoLock(comparisonMethod,contentInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ContentInfo* ContentInfoList::getContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex)
{
  //FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return nullptr;

    AutoReadLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool ContentInfoList::getContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex,ContentInfo& contentInfo)
{
  //FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return false;

    AutoReadLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






ContentInfo* ContentInfoList::getContentInfoByIndex(uint index) const
{
  //FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return nullptr;

    //AutoReadLock lock(mModificationLockPtr);
    if (index >= mLength)
      return nullptr;

    return mArray[index];
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ContentInfo* ContentInfoList::getContentInfoByParameterLevelInfo(T::ParameterLevelInfo& levelInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return nullptr;

    auto pkey = toUInt32(levelInfo.mParameterKey.c_str());

    AutoReadLock lock(mModificationLockPtr);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mParameterLevel == levelInfo.mLevel)
      {
        if ((levelInfo.mParameterKeyType == T::ParamKeyTypeValue::FMI_ID  &&  info->mFmiParameterId != 0  &&  info->mFmiParameterId == pkey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyTypeValue::FMI_NAME  &&  strcmp(info->getFmiParameterName(),levelInfo.mParameterKey.c_str()) == 0))
        {
          if (info->mFmiParameterLevelId == levelInfo.mParameterLevelId)
            return info;
        }
      }
    }
    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByParameterLevelInfo(T::ParameterLevelInfo& levelInfo,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    auto pkey = toUInt32(levelInfo.mParameterKey.c_str());

    AutoReadLock lock(mModificationLockPtr);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mParameterLevel == levelInfo.mLevel)
      {
        if ((levelInfo.mParameterKeyType == T::ParamKeyTypeValue::FMI_ID  &&  info->mFmiParameterId != 0  &&  info->mFmiParameterId == pkey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyTypeValue::FMI_NAME  &&  strcmp(info->getFmiParameterName(),levelInfo.mParameterKey.c_str()) == 0))
        {
          if (info->mFmiParameterLevelId == levelInfo.mParameterLevelId)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoList(uint startFileId,uint startMessageIndex,int maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    uint max = (uint)abs(maxRecords);
    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      // If the records are not sorted according to fileId and messageIndex then the startFileId parameter
      // is used as the start index in the list.

      if (maxRecords >= 0)
      {
        for (uint t=startFileId; t<mLength; t++)
        {
          ContentInfo *info = mArray[t];
          if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);

            if (contentInfoList.getLength() >= max)
              return;
          }
        }
      }
      else
      {
        for (int t=startFileId; t>=0; t--)
        {
          ContentInfo *info = mArray[t];
          if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);

            if (contentInfoList.getLength() >= max)
              return;
          }
        }
      }
      return;
    }

    AutoReadLock lock(mModificationLockPtr);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);

    if (maxRecords >= 0)
    {
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

            if (contentInfoList.getLength() >= max)
              return;
          }
        }
      }
    }
    else
    {
      if (startIdx < 0)
        startIdx = mLength-1;

      for (int t=startIdx; t>=0; t--)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mFileId < startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex <= startMessageIndex))
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);

            if (contentInfoList.getLength() >= max)
              return;
          }
        }
      }
    }

  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentParamKeyListByGenerationId(uint producerId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    paramKeyList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;


    AutoReadLock lock(mModificationLockPtr);
    time_t timeLimit = time(nullptr) + 120;

    ContentInfo searchInfo;
    searchInfo.mProducerId = producerId;
    searchInfo.mGenerationId = generationId;

    int idx = 0;
    if (mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time ||
        mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time)
      idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

    for (uint t=idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGenerationId == generationId  && (info->mDeletionTime == 0 || info->mDeletionTime > timeLimit))
      {
        switch (parameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            if (info->mFmiParameterId > 0)
              paramKeyList.insert(std::to_string(info->mFmiParameterId));
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            if (info->getFmiParameterName()[0] != '\0')
            {
              paramKeyList.insert(info->getFmiParameterName());
            }
            else
            if (info->mFmiParameterId > 0)
            {
              std::string id = "FMI-" + std::to_string(info->mFmiParameterId);
              paramKeyList.insert(id);
            }
            break;

          default:
            return;
        }
      }
      if ((mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time ||
          mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time) &&
          info != nullptr && (info->mProducerId > producerId || (info->mProducerId == producerId && info->mGenerationId > generationId)))
        return;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentParamKeyListByGenerationAndGeometryId(uint producerId,uint generationId,T::GeometryId geometryId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    paramKeyList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;


    AutoReadLock lock(mModificationLockPtr);
    time_t timeLimit = time(nullptr) + 120;

    ContentInfo searchInfo;
    searchInfo.mProducerId = producerId;
    searchInfo.mGenerationId = generationId;

    int idx = 0;
    if (mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time ||
        mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time)
      idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

    for (uint t=idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&
          info->mGenerationId == generationId && info->mGeometryId == geometryId  &&
          (info->mDeletionTime == 0 || info->mDeletionTime > timeLimit))
      {
        switch (parameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            if (info->mFmiParameterId > 0)
              paramKeyList.insert(std::to_string(info->mFmiParameterId));
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            if (info->getFmiParameterName()[0] != '\0')
            {
              paramKeyList.insert(info->getFmiParameterName());
            }
            else
            if (info->mFmiParameterId > 0)
            {
              std::string id = "FMI-" + std::to_string(info->mFmiParameterId);
              paramKeyList.insert(id);
            }
            break;

          default:
            return;
        }
      }
      if ((mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time ||
          mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time) &&
          info != nullptr && (info->mProducerId > producerId || (info->mProducerId == producerId && info->mGenerationId > generationId)))
        return;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentParamKeyListByGenerationGeometryAndLevelId(uint producerId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    paramKeyList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);
    time_t timeLimit = time(nullptr) + 120;

    ContentInfo searchInfo;
    searchInfo.mProducerId = producerId;
    searchInfo.mGenerationId = generationId;

    int idx = 0;
    if (mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time ||
        mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time)
      idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

    for (uint t=idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&
          info->mGenerationId == generationId && info->mGeometryId == geometryId  &&
          info->mFmiParameterLevelId == levelId && (info->mDeletionTime == 0 || info->mDeletionTime > timeLimit))
      {
        switch (parameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            if (info->mFmiParameterId > 0)
              paramKeyList.insert(std::to_string(info->mFmiParameterId));
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            if (info->getFmiParameterName()[0] != '\0')
            {
              paramKeyList.insert(info->getFmiParameterName());
            }
            else
            if (info->mFmiParameterId > 0)
            {
              std::string id = "FMI-" + std::to_string(info->mFmiParameterId);
              paramKeyList.insert(id);
            }
            break;

          default:
            return;
        }
      }
      if ((mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time ||
          mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time) &&
          info != nullptr && (info->mProducerId > producerId || (info->mProducerId == producerId && info->mGenerationId > generationId)))
        return;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentLevelListByGenerationGeometryAndLevelId(uint producerId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList)
{
  FUNCTION_TRACE
  try
  {
    contentLevelList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);
    time_t timeLimit = time(nullptr) + 120;

    ContentInfo searchInfo;
    searchInfo.mProducerId = producerId;
    searchInfo.mGenerationId = generationId;

    int idx = 0;
    if (mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time ||
        mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time)
      idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

    for (uint t=idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&
          info->mGenerationId == generationId && info->mGeometryId == geometryId  &&
          info->mFmiParameterLevelId == levelId && (info->mDeletionTime == 0 || info->mDeletionTime > timeLimit))
      {
        contentLevelList.insert(info->mParameterLevel);
      }
      if ((mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time ||
          mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time) &&
          info != nullptr && (info->mProducerId > producerId || (info->mProducerId == producerId && info->mGenerationId > generationId)))
        return;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentInfoList::getContentGeometryIdListByGenerationId(uint producerId,uint generationId,std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    geometryIdList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    ContentInfo searchInfo;
    searchInfo.mProducerId = producerId;
    searchInfo.mGenerationId = generationId;

    int idx = 0;
    bool sorted = false;

    if (mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time ||
        mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      sorted = true;
    }

    if (sorted)
      idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);


    for (uint t=(uint)idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];

      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        if (info->mGenerationId == generationId)
        {
          geometryIdList.insert(info->mGeometryId);
        }
        else
        {
          if (sorted && t > (uint)idx)
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





void ContentInfoList::getContentGeometryIdList(std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    geometryIdList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];

      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        geometryIdList.insert(info->mGeometryId);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getGenerationIdListByGeometryId(T::GeometryId geometryId,std::set<uint>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    generationIdList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];

      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGeometryId == geometryId)
      {
        generationIdList.insert(info->mGenerationId);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFileId(uint fileId,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::file_message)
    {
      ContentInfo searchInfo;
      searchInfo.mFileId = fileId;
      searchInfo.mMessageIndex = 0;
      int idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByGeometryId(T::GeometryId geometryId,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByRequestCounterKey(ulonglong key,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByForecastTime(const std::string& forecastTime,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    time_t forecastTimeUTC = utcTimeToTimeT(forecastTime);
    getContentInfoListByForecastTime(forecastTimeUTC,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByForecastTime(time_t forecastTimeUTC,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::starttime_fmiId_fmiLevelId_level_file_message)
    {
      ContentInfo searchInfo;
      searchInfo.mForecastTimeUTC = forecastTimeUTC;
      int idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);
      uint t = 0;
      if (idx >= 0)
        t = idx;

      while (t < mLength)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mForecastTimeUTC > forecastTimeUTC)
            return;

          if (info->mForecastTimeUTC == forecastTimeUTC)
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
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mForecastTimeUTC == forecastTimeUTC)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterId(T::FmiParamId fmiParameterId,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterId(T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    time_t startTimeUTC = utcTimeToTimeT(startTime);
    time_t endTimeUTC = utcTimeToTimeT(endTime);
    getContentInfoListByFmiParameterId(fmiParameterId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTimeUTC,endTimeUTC,requestFlags,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterId(T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr || mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    for (uint t = 0; t < mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        if (info->mFmiParameterId == fmiParameterId)
        {
          if (forecastType < 0 || (info->mForecastType == forecastType && (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
          {
            if (geometryId < 0 || info->mGeometryId == geometryId)
            {
              if (parameterLevelId < 0 || info->mFmiParameterLevelId == parameterLevelId)
              {
                if (parameterLevelId < 0 || (info->mParameterLevel >= minLevel && info->mParameterLevel <= maxLevel))
                {
                  if (info->mForecastTimeUTC >= startTimeUTC && info->mForecastTimeUTC <= endTimeUTC)
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
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterIdAndGenerationId(uint producerId,uint generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    time_t startTimeUTC = utcTimeToTimeT(startTime);
    time_t endTimeUTC = utcTimeToTimeT(endTime);
    getContentInfoListByFmiParameterIdAndGenerationId(producerId,generationId,fmiParameterId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTimeUTC,endTimeUTC,requestFlags,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterIdAndGenerationId(uint producerId,uint generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo searchInfo;
      searchInfo.mProducerId = producerId;
      searchInfo.mGenerationId = generationId;
      searchInfo.mFmiParameterId = fmiParameterId;
      int idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mGenerationId == generationId)
          {
            if (info->mFmiParameterId == fmiParameterId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if (parameterLevelId < 0 || info->mFmiParameterLevelId == parameterLevelId)
                  {
                    if (parameterLevelId < 0 || (info->mParameterLevel >= minLevel && info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTimeUTC < startTimeUTC  && (prev == nullptr || prev->mForecastTimeUTC < info->mForecastTimeUTC))
                        prev = info;

                      if (info->mForecastTimeUTC > endTimeUTC  && (next == nullptr || next->mForecastTimeUTC > info->mForecastTimeUTC))
                        next = info;

                      if (info->mForecastTimeUTC >= startTimeUTC  &&  info->mForecastTimeUTC <= endTimeUTC)
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

      if (contentInfoList.getLength() != 0)
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
      list.getContentInfoListByFmiParameterIdAndGenerationId(producerId,generationId,fmiParameterId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTimeUTC,endTimeUTC,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterIdAndGenerationId(uint producerId,uint generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    time_t forecastTimeUTC = utcTimeToTimeT(forecastTime);
    getContentInfoListByFmiParameterIdAndGenerationId(producerId,generationId,fmiParameterId,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTimeUTC,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterIdAndGenerationId(uint producerId,uint generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTimeUTC,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    //std::cout << "SEARCH : " << forecastTime << " " << level << "\n";

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo searchInfo;
      searchInfo.mProducerId = producerId;
      searchInfo.mGenerationId = generationId;
      searchInfo.mFmiParameterId = fmiParameterId;
      int idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

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
          if (info->mGenerationId == generationId)
          {
            if (info->mFmiParameterId == searchInfo.mFmiParameterId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if (parameterLevelId < 0 || info->mFmiParameterLevelId == parameterLevelId)
                  {
                    //std::cout << info->mForecastTime << " " << info->mParameterLevel << " ------ \n";
                    if (info->mForecastTimeUTC < forecastTimeUTC)
                    {
                      if (info->mParameterLevel == level)
                      {
                        if (prevTimeSameLevel == nullptr || prevTimeSameLevel->mForecastTimeUTC < info->mForecastTimeUTC)
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " prev time same level\n";
                          prevTimeSameLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel < level)
                      {
                        if (prevTimePrevLevel == nullptr || prevTimePrevLevel->mForecastTimeUTC < info->mForecastTimeUTC  || (prevTimePrevLevel->mForecastTimeUTC == info->mForecastTimeUTC  &&  prevTimePrevLevel->mParameterLevel < info->mParameterLevel))
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " prev time prev level\n";
                          prevTimePrevLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel > level)
                      {
                        if (prevTimeNextLevel == nullptr || prevTimeNextLevel->mForecastTimeUTC < info->mForecastTimeUTC  ||  (prevTimeNextLevel->mForecastTimeUTC == info->mForecastTimeUTC &&  prevTimeNextLevel->mParameterLevel > info->mParameterLevel))
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " prev time next level\n";
                          prevTimeNextLevel = info;
                        }
                      }
                    }
                    else
                    if (info->mForecastTimeUTC > forecastTimeUTC)
                    {
                      if (info->mParameterLevel == level || parameterLevelId < 0)
                      {
                        if (nextTimeSameLevel == nullptr)
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " next time same level\n";
                          nextTimeSameLevel = info;
                          t = mLength;
                        }
                      }
                      else
                      if (info->mParameterLevel < level)
                      {
                        if (nextTimePrevLevel == nullptr || nextTimePrevLevel->mForecastTimeUTC > info->mForecastTimeUTC || (nextTimePrevLevel->mForecastTimeUTC == info->mForecastTimeUTC &&  nextTimePrevLevel->mParameterLevel < info->mParameterLevel))
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " next time prev level\n";
                          nextTimePrevLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel > level)
                      {
                        if (nextTimeNextLevel == nullptr || (nextTimePrevLevel != nullptr && nextTimePrevLevel->mForecastTimeUTC > info->mForecastTimeUTC) || (nextTimeNextLevel->mForecastTimeUTC == info->mForecastTimeUTC &&  nextTimeNextLevel->mParameterLevel > info->mParameterLevel))
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << "next time next level\n";
                          nextTimeNextLevel = info;
                          t = mLength;
                        }
                      }
                    }
                    else
                    if (info->mForecastTimeUTC == forecastTimeUTC)
                    {
                      if (info->mParameterLevel == level || parameterLevelId < 0)
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
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " same time prev level\n";
                          sameTimePrevLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel > level)
                      {
                        if (sameTimeNextLevel == nullptr || sameTimeNextLevel->mParameterLevel > info->mParameterLevel)
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " same time next level\n";
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
      list.sort(ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time);
      list.getContentInfoListByFmiParameterIdAndGenerationId(producerId,generationId,fmiParameterId,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTimeUTC,contentInfoList);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterIdAndGenerationId2(uint producerId,uint generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    time_t forecastTimeUTC = utcTimeToTimeT(forecastTime);
    getContentInfoListByFmiParameterIdAndGenerationId2(producerId,generationId,fmiParameterId,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTimeUTC,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterIdAndGenerationId2(uint producerId,uint generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTimeUTC,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    //std::cout << "SEARCH2 : " << forecastTime << " " << level << "\n";

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo searchInfo;
      searchInfo.mProducerId = producerId;
      searchInfo.mGenerationId = generationId;
      searchInfo.mFmiParameterId = fmiParameterId;
      searchInfo.mParameterLevel = level;
      searchInfo.mForecastTimeUTC = forecastTimeUTC;

      int idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

      if ( mArray[idx] != nullptr  &&  mArray[idx]->mParameterLevel < level)
      {
        int lev = mArray[idx]->mParameterLevel;
        while (C_UINT(idx) > 0  &&  mArray[idx-1] != nullptr  &&  mArray[idx-1]->mParameterLevel == lev)
          idx--;
      }

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  mArray[idx]->mFmiParameterId < searchInfo.mFmiParameterId)
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
        //std::cout << info->mForecastTime << " " << info->mParameterLevel << " ***** \n";
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mFmiParameterId == searchInfo.mFmiParameterId)
          {
            if (info->mGenerationId == generationId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if (parameterLevelId < 0 || info->mFmiParameterLevelId == parameterLevelId)
                  {
                    //std::cout << info->mForecastTime << " " << info->mParameterLevel << " ------ \n";
                    if (info->mForecastTimeUTC == forecastTimeUTC)
                    {
                      if (info->mParameterLevel == level)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);

                        return;
                      }
                      else
                      if (info->mParameterLevel < level)
                      {
                        if (sameTimePrevLevel == nullptr || sameTimePrevLevel->mParameterLevel < info->mParameterLevel)
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " same time prev level\n";
                          sameTimePrevLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel > level)
                      {
                        if (sameTimeNextLevel == nullptr || sameTimeNextLevel->mParameterLevel > info->mParameterLevel)
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " same time next level\n";
                          sameTimeNextLevel = info;
                          t = mLength;
                        }
                      }
                    }
                    else
                    if (info->mForecastTimeUTC < forecastTimeUTC)
                    {
                      if (info->mParameterLevel == level || parameterLevelId < 0)
                      {
                        if (prevTimeSameLevel == nullptr || prevTimeSameLevel->mForecastTimeUTC < info->mForecastTimeUTC)
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " prev time same level\n";
                          prevTimeSameLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel < level)
                      {
                        if (prevTimePrevLevel == nullptr || prevTimePrevLevel->mForecastTimeUTC < info->mForecastTimeUTC  || (prevTimePrevLevel->mForecastTimeUTC == info->mForecastTimeUTC  &&  prevTimePrevLevel->mParameterLevel < info->mParameterLevel))
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " prev time prev level\n";
                          prevTimePrevLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel > level)
                      {
                        if (prevTimeNextLevel == nullptr || prevTimeNextLevel->mForecastTimeUTC < info->mForecastTimeUTC  ||  (prevTimeNextLevel->mForecastTimeUTC == info->mForecastTimeUTC &&  prevTimeNextLevel->mParameterLevel > info->mParameterLevel))
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " prev time next level\n";
                          prevTimeNextLevel = info;
                        }
                      }
                    }
                    else
                    if (info->mForecastTimeUTC > forecastTimeUTC)
                    {
                      if (info->mParameterLevel == level || parameterLevelId < 0)
                      {
                        if (nextTimeSameLevel == nullptr)
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " next time same level\n";
                          nextTimeSameLevel = info;
                          t = mLength;
                        }
                      }
                      else
                      if (info->mParameterLevel < level)
                      {
                        if (nextTimePrevLevel == nullptr || nextTimePrevLevel->mForecastTimeUTC > info->mForecastTimeUTC || (nextTimePrevLevel->mForecastTimeUTC == info->mForecastTimeUTC  &&  nextTimePrevLevel->mParameterLevel < info->mParameterLevel))
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " next time prev level\n";
                          nextTimePrevLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel > level)
                      {
                        if (nextTimeNextLevel == nullptr || (nextTimePrevLevel != nullptr && nextTimePrevLevel->mForecastTimeUTC > info->mForecastTimeUTC) || (nextTimeNextLevel->mForecastTimeUTC == info->mForecastTimeUTC &&  nextTimeNextLevel->mParameterLevel > info->mParameterLevel))
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " next time next level\n";
                          nextTimeNextLevel = info;
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
      list.sort(ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time);
      list.getContentInfoListByFmiParameterIdAndGenerationId2(producerId,generationId,fmiParameterId,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTimeUTC,contentInfoList);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterIdAndProducerId(uint producerId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    time_t startTimeUTC = utcTimeToTimeT(startTime);
    time_t endTimeUTC = utcTimeToTimeT(endTime);
    getContentInfoListByFmiParameterIdAndProducerId(producerId,fmiParameterId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTimeUTC,endTimeUTC,requestFlags,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentInfoList::getContentInfoListByFmiParameterIdAndProducerId(uint producerId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo searchInfo;
      searchInfo.mProducerId = producerId;
      int idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mProducerId == producerId)
          {
            if (info->mFmiParameterId == fmiParameterId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if (parameterLevelId < 0 || info->mFmiParameterLevelId == parameterLevelId)
                  {
                    if (parameterLevelId < 0 || (info->mParameterLevel >= minLevel && info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTimeUTC < startTimeUTC  && (prev == nullptr || prev->mForecastTimeUTC < info->mForecastTimeUTC))
                        prev = info;

                      if (info->mForecastTimeUTC > endTimeUTC  && (next == nullptr || next->mForecastTimeUTC > info->mForecastTimeUTC))
                        next = info;

                      if (info->mForecastTimeUTC >= startTimeUTC  &&  info->mForecastTimeUTC <= endTimeUTC)
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
      list.sort(ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time);
      list.getContentInfoListByFmiParameterIdAndProducerId(producerId,fmiParameterId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTimeUTC,endTimeUTC,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentInfoList::getContentInfoListByFmiParameterName(const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    time_t startTimeUTC = utcTimeToTimeT(startTime);
    time_t endTimeUTC = utcTimeToTimeT(endTime);
    getContentInfoListByFmiParameterName(fmiParameterName,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTimeUTC,endTimeUTC,requestFlags,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterName(const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr || mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    for (uint t = 0; t < mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        if (strcmp(info->getFmiParameterName(), fmiParameterName.c_str()) == 0)
        {
          if (forecastType < 0 || (info->mForecastType == forecastType && (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
          {
            if (geometryId < 0 || info->mGeometryId == geometryId)
            {
              if (parameterLevelId < 0 || info->mFmiParameterLevelId == parameterLevelId)
              {
                if (parameterLevelId < 0 || (info->mParameterLevel >= minLevel && info->mParameterLevel <= maxLevel))
                {
                  if (info->mForecastTimeUTC >= startTimeUTC && info->mForecastTimeUTC <= endTimeUTC)
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
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




ContentInfo* ContentInfoList::getContentInfoByFmiParameterNameAndGenerationId(uint producerId,uint generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime)
{
  FUNCTION_TRACE
  try
  {
    time_t forecastTimeUTC = utcTimeToTimeT(forecastTime);
    return getContentInfoByFmiParameterNameAndGenerationId(producerId,generationId,fmiParameterName,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTimeUTC);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ContentInfo* ContentInfoList::getContentInfoByFmiParameterNameAndGenerationId(uint producerId,uint generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTimeUTC)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return nullptr;

    AutoReadLock lock(mModificationLockPtr);


    ContentInfo searchInfo;
    searchInfo.mProducerId = producerId;
    searchInfo.mGenerationId = generationId;
    searchInfo.mProducerId = producerId;
    searchInfo.mForecastTimeUTC = forecastTimeUTC;
    searchInfo.setFmiParameterName(fmiParameterName);
    searchInfo.mFmiParameterLevelId = parameterLevelId;
    searchInfo.mParameterLevel = level;
    searchInfo.mForecastType = forecastType;
    searchInfo.mForecastNumber = forecastNumber;
    searchInfo.mGeometryId = geometryId;

    int idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);
    for (int t=0; t < 2; t++)
    {
      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr)
      {
        ContentInfo *cInfo = mArray[idx];

        if (searchInfo.mGenerationId == cInfo->mGenerationId &&
            searchInfo.mForecastTimeUTC == cInfo->mForecastTimeUTC &&
            searchInfo.getFmiParameterName() == cInfo->getFmiParameterName() &&
            searchInfo.mFmiParameterLevelId == cInfo->mFmiParameterLevelId &&
            searchInfo.mParameterLevel == cInfo->mParameterLevel &&
            (searchInfo.mForecastType < 0 || searchInfo.mForecastType == cInfo->mForecastType) &&
            (searchInfo.mForecastNumber < 0 || searchInfo.mForecastNumber == cInfo->mForecastNumber) &&
            (searchInfo.mGeometryId < 0 || searchInfo.mGeometryId == cInfo->mGeometryId))
          return cInfo;

        idx++;
      }
    }

    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




ContentInfo* ContentInfoList::getContentInfoByFmiParameterIdAndGenerationId(uint producerId,uint generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime)
{
  FUNCTION_TRACE
  try
  {
    time_t forecastTimeUTC = utcTimeToTimeT(forecastTime);
    return getContentInfoByFmiParameterIdAndGenerationId(producerId,generationId,fmiParameterId,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTimeUTC);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ContentInfo* ContentInfoList::getContentInfoByFmiParameterIdAndGenerationId(uint producerId,uint generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTimeUTC)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return nullptr;

    AutoReadLock lock(mModificationLockPtr);


    ContentInfo searchInfo;
    searchInfo.mProducerId = producerId;
    searchInfo.mGenerationId = generationId;
    searchInfo.mProducerId = producerId;
    searchInfo.mForecastTimeUTC = forecastTimeUTC;
    searchInfo.mFmiParameterId = fmiParameterId;
    searchInfo.mFmiParameterLevelId = parameterLevelId;
    searchInfo.mParameterLevel = level;
    searchInfo.mForecastType = forecastType;
    searchInfo.mForecastNumber = forecastNumber;
    searchInfo.mGeometryId = geometryId;

    int idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);
    for (int t=0; t < 2; t++)
    {
      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr)
      {
        ContentInfo *cInfo = mArray[idx];

        if (searchInfo.mGenerationId == cInfo->mGenerationId &&
            searchInfo.mForecastTimeUTC == cInfo->mForecastTimeUTC &&
            searchInfo.mFmiParameterId == cInfo->mFmiParameterId &&
            searchInfo.mFmiParameterLevelId == cInfo->mFmiParameterLevelId &&
            searchInfo.mParameterLevel == cInfo->mParameterLevel &&
            (searchInfo.mForecastType < 0 || searchInfo.mForecastType == cInfo->mForecastType) &&
            (searchInfo.mForecastNumber < 0 || searchInfo.mForecastNumber == cInfo->mForecastNumber) &&
            (searchInfo.mGeometryId < 0 || searchInfo.mGeometryId == cInfo->mGeometryId))
          return cInfo;

        idx++;
      }
    }

    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentInfoList::getContentInfoListByFmiParameterNameAndGenerationId(uint producerId,uint generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    time_t forecastTimeUTC = utcTimeToTimeT(forecastTime);
    getContentInfoListByFmiParameterNameAndGenerationId(producerId,generationId,fmiParameterName,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTimeUTC,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterNameAndGenerationId(uint producerId,uint generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTimeUTC,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    //std::cout << "SEARCH : " << forecastTime << " " << level << "\n";

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo searchInfo;
      searchInfo.mProducerId = producerId;
      searchInfo.mGenerationId = generationId;
      searchInfo.setFmiParameterName(fmiParameterName);
      int idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

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
          if (info->mGenerationId == generationId)
          {
            if (strcmp(info->getFmiParameterName(),searchInfo.getFmiParameterName()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if (parameterLevelId < 0 || info->mFmiParameterLevelId == parameterLevelId)
                  {
                    //std::cout << info->mForecastTime << " " << info->mParameterLevel << " ------ \n";
                    if (info->mForecastTimeUTC < forecastTimeUTC)
                    {
                      if (info->mParameterLevel == level)
                      {
                        if (prevTimeSameLevel == nullptr || prevTimeSameLevel->mForecastTimeUTC < info->mForecastTimeUTC)
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " prev time same level\n";
                          prevTimeSameLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel < level)
                      {
                        if (prevTimePrevLevel == nullptr || prevTimePrevLevel->mForecastTimeUTC < info->mForecastTimeUTC  || (prevTimePrevLevel->mForecastTimeUTC == info->mForecastTimeUTC  &&  prevTimePrevLevel->mParameterLevel < info->mParameterLevel))
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " prev time prev level\n";
                          prevTimePrevLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel > level)
                      {
                        if (prevTimeNextLevel == nullptr || prevTimeNextLevel->mForecastTimeUTC < info->mForecastTimeUTC  ||  (prevTimeNextLevel->mForecastTimeUTC == info->mForecastTimeUTC &&  prevTimeNextLevel->mParameterLevel > info->mParameterLevel))
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " prev time next level\n";
                          prevTimeNextLevel = info;
                        }
                      }
                    }
                    else
                    if (info->mForecastTimeUTC > forecastTimeUTC)
                    {
                      if (info->mParameterLevel == level || parameterLevelId < 0)
                      {
                        if (nextTimeSameLevel == nullptr)
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " next time same level\n";
                          nextTimeSameLevel = info;
                          t = mLength;
                        }
                      }
                      else
                      if (info->mParameterLevel < level)
                      {
                        if (nextTimePrevLevel == nullptr || nextTimePrevLevel->mForecastTimeUTC > info->mForecastTimeUTC || (nextTimePrevLevel->mForecastTimeUTC == info->mForecastTimeUTC &&  nextTimePrevLevel->mParameterLevel < info->mParameterLevel))
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " next time prev level\n";
                          nextTimePrevLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel > level)
                      {
                        if (nextTimeNextLevel == nullptr || (nextTimePrevLevel != nullptr && nextTimePrevLevel->mForecastTimeUTC > info->mForecastTimeUTC) || (nextTimeNextLevel->mForecastTimeUTC == info->mForecastTimeUTC &&  nextTimeNextLevel->mParameterLevel > info->mParameterLevel))
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << "next time next level\n";
                          nextTimeNextLevel = info;
                          t = mLength;
                        }
                      }
                    }
                    else
                    if (info->mForecastTimeUTC == forecastTimeUTC)
                    {
                      if (info->mParameterLevel == level || parameterLevelId < 0)
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
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " same time prev level\n";
                          sameTimePrevLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel > level)
                      {
                        if (sameTimeNextLevel == nullptr || sameTimeNextLevel->mParameterLevel > info->mParameterLevel)
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " same time next level\n";
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
      list.getContentInfoListByFmiParameterNameAndGenerationId(producerId,generationId,fmiParameterName,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTimeUTC,contentInfoList);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterNameAndGenerationId2(uint producerId,uint generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    time_t forecastTimeUTC = utcTimeToTimeT(forecastTime);
    getContentInfoListByFmiParameterNameAndGenerationId2(producerId,generationId,fmiParameterName,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTimeUTC,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterNameAndGenerationId2(uint producerId,uint generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTimeUTC,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    //std::cout << "SEARCH2 : " << forecastTime << " " << level << "\n";

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo searchInfo;
      searchInfo.mProducerId = producerId;
      searchInfo.mGenerationId = generationId;
      searchInfo.setFmiParameterName(fmiParameterName);
      searchInfo.mParameterLevel = level;
      searchInfo.mForecastTimeUTC = forecastTimeUTC;

      int idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

      if ( mArray[idx] != nullptr  &&  mArray[idx]->mParameterLevel < level)
      {
        int lev = mArray[idx]->mParameterLevel;
        while (C_UINT(idx) > 0  &&  mArray[idx-1] != nullptr  &&  mArray[idx-1]->mParameterLevel == lev)
          idx--;
      }

      if (C_UINT(idx) < mLength  &&  mArray[idx] != nullptr  &&  strcmp(mArray[idx]->getFmiParameterName(),searchInfo.getFmiParameterName()) < 0)
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
        //std::cout << info->mForecastTime << " " << info->mParameterLevel << " ***** \n";
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (strcmp(info->getFmiParameterName(),searchInfo.getFmiParameterName()) == 0)
          {
            if (info->mGenerationId == generationId)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if (parameterLevelId < 0 || info->mFmiParameterLevelId == parameterLevelId)
                  {
                    //std::cout << info->mForecastTime << " " << info->mParameterLevel << " ------ \n";
                    if (info->mForecastTimeUTC == forecastTimeUTC)
                    {
                      if (info->mParameterLevel == level)
                      {
                        if (contentInfoList.getReleaseObjects())
                          contentInfoList.addContentInfo(info->duplicate());
                        else
                          contentInfoList.addContentInfo(info);

                        return;
                      }
                      else
                      if (info->mParameterLevel < level)
                      {
                        if (sameTimePrevLevel == nullptr || sameTimePrevLevel->mParameterLevel < info->mParameterLevel)
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " same time prev level\n";
                          sameTimePrevLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel > level)
                      {
                        if (sameTimeNextLevel == nullptr || sameTimeNextLevel->mParameterLevel > info->mParameterLevel)
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " same time next level\n";
                          sameTimeNextLevel = info;
                          t = mLength;
                        }
                      }
                    }
                    else
                    if (info->mForecastTimeUTC < forecastTimeUTC)
                    {
                      if (info->mParameterLevel == level || parameterLevelId < 0)
                      {
                        if (prevTimeSameLevel == nullptr || prevTimeSameLevel->mForecastTimeUTC < info->mForecastTimeUTC)
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " prev time same level\n";
                          prevTimeSameLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel < level)
                      {
                        if (prevTimePrevLevel == nullptr || prevTimePrevLevel->mForecastTimeUTC < info->mForecastTimeUTC  || (prevTimePrevLevel->mForecastTimeUTC == info->mForecastTimeUTC  &&  prevTimePrevLevel->mParameterLevel < info->mParameterLevel))
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " prev time prev level\n";
                          prevTimePrevLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel > level)
                      {
                        if (prevTimeNextLevel == nullptr || prevTimeNextLevel->mForecastTimeUTC < info->mForecastTimeUTC  ||  (prevTimeNextLevel->mForecastTimeUTC == info->mForecastTimeUTC &&  prevTimeNextLevel->mParameterLevel > info->mParameterLevel))
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " prev time next level\n";
                          prevTimeNextLevel = info;
                        }
                      }
                    }
                    else
                    if (info->mForecastTimeUTC > forecastTimeUTC)
                    {
                      if (info->mParameterLevel == level || parameterLevelId < 0)
                      {
                        if (nextTimeSameLevel == nullptr)
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " next time same level\n";
                          nextTimeSameLevel = info;
                          t = mLength;
                        }
                      }
                      else
                      if (info->mParameterLevel < level)
                      {
                        if (nextTimePrevLevel == nullptr || nextTimePrevLevel->mForecastTimeUTC > info->mForecastTimeUTC || (nextTimePrevLevel->mForecastTimeUTC == info->mForecastTimeUTC  &&  nextTimePrevLevel->mParameterLevel < info->mParameterLevel))
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " next time prev level\n";
                          nextTimePrevLevel = info;
                        }
                      }
                      else
                      if (info->mParameterLevel > level)
                      {
                        if (nextTimeNextLevel == nullptr || (nextTimePrevLevel != nullptr && nextTimePrevLevel->mForecastTimeUTC > info->mForecastTimeUTC) || (nextTimeNextLevel->mForecastTimeUTC == info->mForecastTimeUTC &&  nextTimeNextLevel->mParameterLevel > info->mParameterLevel))
                        {
                          //std::cout << info->mForecastTime << " " << info->mParameterLevel << " next time next level\n";
                          nextTimeNextLevel = info;
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
      list.getContentInfoListByFmiParameterNameAndGenerationId2(producerId,generationId,fmiParameterName,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTimeUTC,contentInfoList);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterNameAndGenerationId(uint producerId,uint generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    time_t startTimeUTC = utcTimeToTimeT(startTime);
    time_t endTimeUTC = utcTimeToTimeT(endTime);
    getContentInfoListByFmiParameterNameAndGenerationId(producerId,generationId,fmiParameterName,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTimeUTC,endTimeUTC,requestFlags,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterNameAndGenerationId(uint producerId,uint generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo searchInfo;
      searchInfo.mGenerationId = generationId;
      searchInfo.mProducerId = producerId;
      searchInfo.setFmiParameterName(fmiParameterName);
      int idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

      uint t = C_UINT(idx);

      ContentInfo *prev = nullptr;
      ContentInfo *next = nullptr;

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        //std::cout << info->mForecastTime << " " << info->mParameterLevel << "\n";
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mGenerationId == generationId)
          {
            if (strcmp(info->getFmiParameterName(),searchInfo.getFmiParameterName()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if (parameterLevelId < 0 || info->mFmiParameterLevelId == parameterLevelId)
                  {
                    if (parameterLevelId < 0 || (info->mParameterLevel >= minLevel && info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTimeUTC < startTimeUTC  && (prev == nullptr || prev->mForecastTimeUTC < info->mForecastTimeUTC))
                        prev = info;

                      if (info->mForecastTimeUTC > endTimeUTC  && (next == nullptr || next->mForecastTimeUTC > info->mForecastTimeUTC))
                        next = info;

                      if (info->mForecastTimeUTC >= startTimeUTC  &&  info->mForecastTimeUTC <= endTimeUTC)
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
      list.getContentInfoListByFmiParameterNameAndGenerationId(producerId,generationId,fmiParameterName,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTimeUTC,endTimeUTC,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterNameAndGenerationList(T::GenerationInfoList& generationList,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr || mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      uint glen = generationList.getLength();
      for (uint g = 0; g < glen; g++)
      {
        T::GenerationInfo *gInfo = generationList.getGenerationInfoByIndex(g);

        ContentInfo searchInfo;
        searchInfo.mProducerId = gInfo->mProducerId;
        searchInfo.mGenerationId = gInfo->mGenerationId;
        searchInfo.setFmiParameterName(fmiParameterName);
        int idx = getClosestIndexNoLock(mComparisonMethod, searchInfo);

        uint t = C_UINT(idx);

        while (t < mLength && mArray[t] != nullptr)
        {
          ContentInfo *info = mArray[t];
          //std::cout << info->mForecastTime << " " << info->mParameterLevel << "\n";
          if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
          {
            if (info->mGenerationId == gInfo->mGenerationId)
            {
              if (strcmp(info->getFmiParameterName(), searchInfo.getFmiParameterName()) == 0)
              {
                if (forecastType < 0 || (info->mForecastType == forecastType && (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
                {
                  if (geometryId < 0 || info->mGeometryId == geometryId)
                  {
                    if (parameterLevelId < 0 || info->mFmiParameterLevelId == parameterLevelId)
                    {
                      if (parameterLevelId < 0 || (info->mParameterLevel >= minLevel && info->mParameterLevel <= maxLevel))
                      {
                        if (info->mForecastTimeUTC >= startTimeUTC && info->mForecastTimeUTC <= endTimeUTC)
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
      list.getContentInfoListByFmiParameterNameAndGenerationList(generationList, fmiParameterName, parameterLevelId, minLevel, maxLevel,
          forecastType, forecastNumber, geometryId, startTimeUTC, endTimeUTC, requestFlags, contentInfoList);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterNameAndProducerId(uint producerId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    time_t startTimeUTC = utcTimeToTimeT(startTime);
    time_t endTimeUTC = utcTimeToTimeT(endTime);
    getContentInfoListByFmiParameterNameAndProducerId(producerId,fmiParameterName,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTimeUTC,endTimeUTC,requestFlags,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterNameAndProducerId(uint producerId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo searchInfo;
      searchInfo.mProducerId = producerId;
      int idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

      uint t = C_UINT(idx);

      while (t < mLength  &&  mArray[t] != nullptr)
      {
        ContentInfo *info = mArray[t];
        if ((info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mProducerId == producerId)
          {
            if (strcmp(info->getFmiParameterName(),fmiParameterName.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  (info->mForecastNumber == forecastNumber || forecastNumber < 0)))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if (parameterLevelId < 0 || info->mFmiParameterLevelId == parameterLevelId)
                  {
                    if (parameterLevelId < 0 || (info->mParameterLevel >= minLevel && info->mParameterLevel <= maxLevel))
                    {
                      if (info->mForecastTimeUTC >= startTimeUTC  &&  info->mForecastTimeUTC <= endTimeUTC)
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
      list.getContentInfoListByFmiParameterNameAndProducerId(producerId,fmiParameterName,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTimeUTC,endTimeUTC,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByProducerId(uint producerId,uint startFileId,uint startMessageIndex,int maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    uint max = (uint)abs(maxRecords);
    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      // If the records are not sorted according to fileId and messageIndex then the startFileId parameter
      // is used as the start index in the list.

      if (maxRecords >= 0)
      {
        for (uint t=startFileId; t<mLength; t++)
        {
          ContentInfo *info = mArray[t];
          if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mProducerId == producerId)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);

            if (contentInfoList.getLength() >= max)
              return;
          }
        }
      }
      else
      {
        for (int t=startFileId; t>=0; t--)
        {
          ContentInfo *info = mArray[t];
          if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mProducerId == producerId)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);

            if (contentInfoList.getLength() >= max)
              return;
          }
        }
      }

      return;
    }

    AutoReadLock lock(mModificationLockPtr);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);

    if (maxRecords >= 0)
    {
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

          if (contentInfoList.getLength() >= max)
            return;
        }
      }
    }
    else
    {
      if (startIdx < 0)
        startIdx = mLength-1;

      for (int t=startIdx; t>=0; t--)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mProducerId == producerId && (info->mFileId < startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex <= startMessageIndex)))
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);

          if (contentInfoList.getLength() >= max)
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





void ContentInfoList::getContentInfoListByProducerId(uint producerId,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

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

    AutoReadLock lock(mModificationLockPtr);
    ContentInfo contentInfo;
    contentInfo.mProducerId = producerId;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::producer_file_message,contentInfo);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByGenerationId(uint producerId,uint generationId,uint startFileId,uint startMessageIndex,int maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    uint max = (uint)abs(maxRecords);

    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      // If the records are not sorted according to fileId and messageIndex then the startFileId parameter
      // is used as the start index in the list.

      if (maxRecords >= 0)
      {
        for (uint t=startFileId; t<mLength; t++)
        {
          ContentInfo *info = mArray[t];
          if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGenerationId == generationId)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);

            if (contentInfoList.getLength() >= max)
              return;
          }
        }
      }
      else
      {
        for (int t=startFileId; t>=0; t--)
        {
          ContentInfo *info = mArray[t];
          if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGenerationId == generationId)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);

            if (contentInfoList.getLength() >= max)
              return;
          }
        }
      }
      return;
    }

    AutoReadLock lock(mModificationLockPtr);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);

    if (maxRecords >= 0)
    {
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

          if (contentInfoList.getLength() >= max)
            return;
        }
      }
    }
    else
    {
      if (startIdx < 0)
        startIdx = mLength-1;

      for (int t=startIdx; t>=0; t--)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGenerationId == generationId && (info->mFileId < startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex <= startMessageIndex)))
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);

          if (contentInfoList.getLength() >= max)
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





std::size_t ContentInfoList::getHash()
{
  FUNCTION_TRACE
  try
  {
    std::size_t hash = 0;

    if (mArray == nullptr ||  mLength == 0)
      return hash;

    AutoReadLock lock(mModificationLockPtr);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        boost::hash_combine(hash,info->mFileId);
        boost::hash_combine(hash,info->mMessageIndex);
      }
    }
    return hash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::size_t ContentInfoList::getHashByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    std::size_t hash = 0;

    if (mArray == nullptr ||  mLength == 0)
      return hash;

    if (mComparisonMethod != ContentInfo::ComparisonMethod::producer_file_message)
    {
      // If the records are not sorted according to fileId and messageIndex then the startFileId parameter
      // is used as the start index in the list.

      for (uint t=0; t<mLength; t++)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mProducerId == producerId)
        {
          boost::hash_combine(hash,info->mFileId);
          boost::hash_combine(hash,info->mMessageIndex);
        }
      }
      return hash;
    }

    AutoReadLock lock(mModificationLockPtr);
    ContentInfo contentInfo;
    contentInfo.mProducerId = producerId;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::producer_file_message,contentInfo);
    if (startIdx < 0)
      startIdx = 0;

    for (uint t=startIdx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        if (info->mProducerId == producerId)
        {
          boost::hash_combine(hash,info->mFileId);
          boost::hash_combine(hash,info->mMessageIndex);
        }
        else
        {
          if (C_INT(t) > startIdx)
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





void ContentInfoList::getContentInfoListByGenerationAndGeometryId(uint producerId,uint generationId,T::GeometryId geometryId,uint startFileId,uint startMessageIndex,int maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    uint max = (uint)abs(maxRecords);
    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      // If the records are not sorted according to fileId and messageIndex then the startFileId parameter
      // is used as the start index in the list.

      if (maxRecords >= 0)
      {
        for (uint t=startFileId; t<mLength; t++)
        {
          ContentInfo *info = mArray[t];
          if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGenerationId == generationId  &&  info->mGeometryId == geometryId)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);

            if (contentInfoList.getLength() >= max)
              return;
          }
        }
      }
      else
      {
        for (int t=startFileId; t>=0; t--)
        {
          ContentInfo *info = mArray[t];
          if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGenerationId == generationId  &&  info->mGeometryId == geometryId)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);

            if (contentInfoList.getLength() >= max)
              return;
          }
        }
      }
      return;
    }

    AutoReadLock lock(mModificationLockPtr);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);

    if (maxRecords >= 0)
    {
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

          if (contentInfoList.getLength() >= max)
            return;
        }
      }
    }
    else
    {
      if (startIdx < 0)
        startIdx = mLength-1;

      for (int t=startIdx; t>=0; t--)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mGenerationId == generationId  &&  info->mGeometryId == geometryId && (info->mFileId < startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex <= startMessageIndex)))
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);

          if (contentInfoList.getLength() >= max)
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





void ContentInfoList::getContentInfoListByGenerationId(uint producerId,uint generationId,const std::string& startTime,const std::string& endTime,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    time_t startTimeUTC = utcTimeToTimeT(startTime);
    time_t endTimeUTC = utcTimeToTimeT(endTime);
    getContentInfoListByGenerationId(producerId,generationId,startTimeUTC,endTimeUTC,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getContentInfoListByGenerationId(uint producerId,uint generationId,time_t startTimeUTC,time_t endTimeUTC,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    ContentInfo searchInfo;
    searchInfo.mProducerId = producerId;
    searchInfo.mGenerationId = generationId;

    int idx = 0;
    bool sorted = false;
    if (mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time ||
        mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      sorted = true;
    }

    if (sorted)
      idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

    for (uint t=(uint)idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        if (info->mGenerationId == generationId)
        {
          if (info->mForecastTimeUTC >= startTimeUTC  &&  info->mForecastTimeUTC <= endTimeUTC)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);
          }
        }
        else
        {
          if (sorted && t > (uint)idx)
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





void ContentInfoList::getContentInfoListBySourceId(uint sourceId,uint startFileId,uint startMessageIndex,int maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    uint max = (uint)abs(maxRecords);
    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      // If the records are not sorted according to fileId and messageIndex then the startFileId parameter
      // is used as the start index in the list.

      if (maxRecords >= 0)
      {
        for (uint t=startFileId; t<mLength; t++)
        {
          ContentInfo *info = mArray[t];
          if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mSourceId == sourceId)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);

            if (contentInfoList.getLength() >= max)
              return;
          }
        }
      }
      else
      {
        for (int t=startFileId; t>=0; t--)
        {
          ContentInfo *info = mArray[t];
          if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mSourceId == sourceId)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);

            if (contentInfoList.getLength() >= max)
              return;
          }
        }
      }
      return;
    }


    AutoReadLock lock(mModificationLockPtr);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);

    if (maxRecords >= 0)
    {
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

          if (contentInfoList.getLength() >= max)
            return;
        }
      }
    }
    else
    {
      if (startIdx < 0)
        startIdx = mLength-1;

      for (int t=startIdx; t>=0; t--)
      {
        ContentInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mSourceId == sourceId && (info->mFileId < startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex <= startMessageIndex)))
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);

          if (contentInfoList.getLength() >= max)
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





void ContentInfoList::getFmiParamLevelIdListByFmiParameterId(T::FmiParamId fmiParameterId,std::vector<T::ParamLevelId>& paramLevelIdList)
{
  FUNCTION_TRACE
  try
  {
    paramLevelIdList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);
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
            paramLevelIdList.emplace_back(info->mFmiParameterLevelId);
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getParamLevelListByFmiLevelId(T::ParamLevelId paramLevelId,std::set<T::ParamLevel>& paramLevelList)
{
  FUNCTION_TRACE
  try
  {
    paramLevelList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mFmiParameterLevelId == paramLevelId)
      {
        //if (paramLevelList.find(info->mParameterLevel) == paramLevelList.end())
          paramLevelList.insert(info->mParameterLevel);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getParamLevelInfoListByFmiParameterId(T::FmiParamId fmiParameterId,ParameterLevelInfoList& parameterLevelInfoList)
{
  FUNCTION_TRACE
  try
  {
    parameterLevelInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0  &&  info->mFmiParameterId == fmiParameterId)
      {
        auto id = std::to_string(info->mFmiParameterId);
        ParameterLevelInfo *pInfo = parameterLevelInfoList.getParameterLevelInfo(T::ParamKeyTypeValue::FMI_ID,
            id,info->mFmiParameterLevelId,info->mParameterLevel);

        if (pInfo == nullptr)
        {
          pInfo = new ParameterLevelInfo(T::ParamKeyTypeValue::FMI_ID,id,info->mFmiParameterLevelId,info->mParameterLevel);
          parameterLevelInfoList.addParameterLevelInfo(pInfo);
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getForecastTimeList(std::set<std::string>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    forecastTimeList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        if (info != nullptr)
        {
          //if (forecastTimeList.find(info->mForecastTime) == forecastTimeList.end())
          {
            forecastTimeList.insert(info->getForecastTime());
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getForecastTimeList(std::set<time_t>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    forecastTimeList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        if (info != nullptr)
        {
          //if (forecastTimeList.find(info->mForecastTimeUTC) == forecastTimeList.end())
          {
            forecastTimeList.insert(info->mForecastTimeUTC);
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getForecastTimeListByGenerationId(uint producerId,uint generationId,std::set<std::string>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    forecastTimeList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    ContentInfo searchInfo;
    searchInfo.mProducerId = producerId;
    searchInfo.mGenerationId = generationId;

    int idx = 0;
    bool sorted = false;
    if (mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time ||
        mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      sorted = true;
    }

    if (sorted)
      idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);


    for (uint t=(uint)idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mGenerationId == generationId)
          {
            forecastTimeList.insert(info->getForecastTime());
          }
          else
          {
            if (sorted && t > (uint)idx)
              return;
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getForecastTimeListByGenerationId(uint producerId,uint generationId,std::set<time_t>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    forecastTimeList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    ContentInfo searchInfo;
    searchInfo.mProducerId = producerId;
    searchInfo.mGenerationId = generationId;

    int idx = 0;
    bool sorted = false;
    if (mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time ||
        mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      sorted = true;
    }

    if (sorted)
      idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);


    for (uint t=(uint)idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mGenerationId == generationId)
          {
            forecastTimeList.insert(info->mForecastTimeUTC);
          }
          else
          {
            if (sorted && t > (uint)idx)
              return;
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getForecastTimeRangeByGenerationId(uint producerId,uint generationId,time_t& startTime,time_t& endTime)
{
  try
  {
    std::size_t hash = 0;
    getForecastTimeRangeByGenerationId(producerId,generationId,startTime,endTime,hash);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentInfoList::getForecastTimeRangeByGenerationId(uint producerId,uint generationId,time_t& startTime,time_t& endTime,std::size_t& hash)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    hash = 0;
    AutoReadLock lock(mModificationLockPtr);

    ContentInfo searchInfo;
    searchInfo.mProducerId = producerId;
    searchInfo.mGenerationId = generationId;

    int idx = 0;
    bool sorted = false;
    if (mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time ||
        mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      sorted = true;
    }

    if (sorted)
      idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

    startTime = 0;
    endTime = 0;

    for (uint t=(uint)idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mGenerationId == generationId)
          {
            boost::hash_combine(hash,info->mFileId);
            boost::hash_combine(hash,info->mMessageIndex);

            if (startTime == 0 || info->mForecastTimeUTC < startTime)
              startTime = info->mForecastTimeUTC;

            if (endTime == 0 || info->mForecastTimeUTC > endTime)
              endTime = info->mForecastTimeUTC;
          }
          else
          {
            if (sorted && t > (uint)idx)
              return;
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentInfoList::getForecastTimeListByGenerationAndGeometry(uint producerId,uint generationId,T::GeometryId geometryId,std::set<std::string>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    forecastTimeList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    ContentInfo searchInfo;
    searchInfo.mProducerId = producerId;
    searchInfo.mGenerationId = generationId;

    int idx = 0;
    bool sorted = false;
    if (mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time ||
        mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      sorted = true;
    }

    if (sorted)
      idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

    for (uint t=(uint)idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mGenerationId == generationId)
          {
            if (info->mGeometryId == geometryId)
            {
              //if (forecastTimeList.find(info->mForecastTime) == forecastTimeList.end())
              {
                forecastTimeList.insert(info->getForecastTime());
              }
            }
          }
          else
          {
            if (sorted && t > (uint)idx)
              return;
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getForecastTimeListByGenerationGeometryAndLevelId(uint producerId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<std::string>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    forecastTimeList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    ContentInfo searchInfo;
    searchInfo.mProducerId = producerId;
    searchInfo.mGenerationId = generationId;

    int idx = 0;
    bool sorted = false;
    if (mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time ||
        mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      sorted = true;
    }

    if (sorted)
      idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

    for (uint t=(uint)idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mGenerationId == generationId)
          {
            if (info->mGeometryId == geometryId)
            {
              if (info->mFmiParameterLevelId == levelId)
              {
                forecastTimeList.insert(info->getForecastTime());
              }
            }
          }
          else
          {
            if (sorted && t > (uint)idx)
              return;
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getForecastTimeListByGenerationAndGeometry(uint producerId,uint generationId,T::GeometryId geometryId,std::set<time_t>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    forecastTimeList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    ContentInfo searchInfo;
    searchInfo.mProducerId = producerId;
    searchInfo.mGenerationId = generationId;

    int idx = 0;
    bool sorted = false;
    if (mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time ||
        mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      sorted = true;
    }

    if (sorted)
      idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

    for (uint t=(uint)idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mGenerationId == generationId)
          {
            if (info->mGeometryId == geometryId)
            {
              //if (forecastTimeList.find(info->mForecastTime) == forecastTimeList.end())
              {
                forecastTimeList.insert(info->mForecastTimeUTC);
              }
            }
          }
          else
          {
            if (sorted && t > (uint)idx)
              return;
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getForecastTimeListByProducerId(uint producerId,std::set<std::string>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    forecastTimeList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    ContentInfo searchInfo;
    searchInfo.mProducerId = producerId;

    int idx = 0;
    bool sorted = false;
    if (mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time ||
        mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      sorted = true;
    }

    if (sorted)
      idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

    for (uint t=(uint)idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mProducerId == producerId)
          {
            forecastTimeList.insert(info->getForecastTime());
          }
          else
          {
            if (sorted && t > (uint)idx)
              return;
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::getForecastTimeListByProducerId(uint producerId,std::set<time_t>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    forecastTimeList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    ContentInfo searchInfo;
    searchInfo.mProducerId = producerId;

    int idx = 0;
    bool sorted = false;
    if (mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time ||
        mComparisonMethod == T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time)
    {
      sorted = true;
    }

    if (sorted)
      idx = getClosestIndexNoLock(mComparisonMethod,searchInfo);

    for (uint t=(uint)idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info != nullptr  &&  (info->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
        {
          if (info->mProducerId == producerId)
          {
            forecastTimeList.insert(info->mForecastTimeUTC);
          }
          else
          {
            if (sorted && t > (uint)idx)
              return;
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ContentInfoList::getLength() const
{
  //FUNCTION_TRACE
  try
  {
    return mLength;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::setLockingEnabled(bool lockingEnabled)
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





void ContentInfoList::setComparisonMethod(uint comparisonMethod)
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




void ContentInfoList::sort(uint comparisonMethod)
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

    switch (mComparisonMethod)
    {
      case 1:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_1);
        break;
      case 2:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_2);
        break;
      case 3:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_3);
        break;
      case 4:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_4);
        break;
      case 5:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_5);
        break;
      case 6:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_6);
        break;
      case 7:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_7);
        break;
      case 8:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_8);
        break;
      case 9:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_9);
        break;
      case 10:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_10);
        break;
      case 11:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_11);
        break;
      case 12:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_12);
        break;
      case 13:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_13);
        break;
      case 14:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_14);
        break;
      case 15:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_15);
        break;
      case 16:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_16);
        break;
      case 17:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_17);
        break;
      case 18:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_18);
        break;
      case 19:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_19);
        break;
      case 20:
        qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare_20);
        break;
    }

  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::writeToFile(const std::string& filename)
{
  FUNCTION_TRACE
  try
  {
    writeToFile(filename,"we");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::writeToFile(const std::string& filename,const char *filemode)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr);

    FILE *file = fopen(filename.c_str(),filemode);
    if (file == nullptr)
    {
      Fmi::Exception exception(BCP,"Cannot create the file!");
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ContentInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




}
}
