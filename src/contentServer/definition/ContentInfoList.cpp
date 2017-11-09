#include "ContentInfoList.h"
#include "RequestFlags.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"
#include "grid-files/common/AutoThreadLock.h"
#include "grid-files/common/AutoReadLock.h"
#include "grid-files/common/AutoWriteLock.h"
#include "grid-files/common/ShowFunction.h"

#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace T
{


ThreadLock ContentInfoList_sortLock;

ContentInfo::ComparisonMethod contentInfo_comparisonMethod = ContentInfo::ComparisonMethod::none;



int contentInfo_compare(const void *_val1,const void *_val2)
{
  if (_val1 != NULL  &&  _val2 != NULL)
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
    mReleaseObjects = true;
    mComparisonMethod = ContentInfo::ComparisonMethod::none;
    mSize = 100;
    mLength = 0;
    mArray = new ContentInfoPtr[100];

    for (uint t=0; t<100; t++)
    {
      mArray[t] = NULL;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ContentInfoList::ContentInfoList(ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    contentInfoList.lock();

    mReleaseObjects = true;
    mArray = NULL;
    mSize = contentInfoList.getSize();
    mLength = contentInfoList.getLength();
    mComparisonMethod = contentInfoList.mComparisonMethod;
    if (mSize > 0)
    {
      mArray = new ContentInfoPtr[mSize];

      for (uint t=0; t<mSize; t++)
      {
        ContentInfo *info = contentInfoList.getContentInfoByIndexNoCheck(t);
        if (info != NULL)
        {
          mArray[t] = info->duplicate();
        }
        else
        {
          mArray[t] = NULL;
        }
      }
    }
    contentInfoList.unlock();
  }
  catch (...)
  {
    contentInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ContentInfoList::~ContentInfoList()
{
  FUNCTION_TRACE
  try
  {
    clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::operator=(ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (&contentInfoList == this)
      return;

    clear();

    AutoWriteLock lock(&mModificationLock);
    contentInfoList.lock();
    mArray = NULL;
    mSize = contentInfoList.getSize();
    mLength = contentInfoList.getLength();
    mComparisonMethod = contentInfoList.mComparisonMethod;
    if (mSize > 0)
    {
      mArray = new ContentInfoPtr[mSize];

      for (uint t=0; t<mSize; t++)
      {
        ContentInfo *info = contentInfoList.getContentInfoByIndexNoCheck(t);
        if (info != NULL)
        {
          if (mReleaseObjects)
            mArray[t] = info->duplicate();
          else
            mArray[t] = info;
        }
        else
        {
          mArray[t] = NULL;
        }
      }
    }
    contentInfoList.unlock();
  }
  catch (...)
  {
    contentInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::addContentInfo(ContentInfo *contentInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    if (mArray == NULL  ||  mLength == mSize)
    {
      increaseSize(mSize + mSize/5 + 10);
    }

    if (mComparisonMethod == ContentInfo::ComparisonMethod::none)
    {
      mArray[mLength] = contentInfo;
      mLength++;
      return;
    }

    int idx = getClosestIndexNoLock(mComparisonMethod,*contentInfo);

    while (idx < (int)mLength  &&  mArray[idx] != NULL  &&   mArray[idx]->compare(mComparisonMethod,contentInfo) < 0)
    {
      idx++;
    }

    if (idx == (int)mLength)
    {
      mArray[mLength] = contentInfo;
      mLength++;
      return;
    }

    if (idx < (int)mLength)
      memmove(&mArray[idx+1],&mArray[idx],sizeof(void*)*(mLength-idx));

    mArray[idx] = contentInfo;
    mLength++;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::addContentInfoList(ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint len1 = mLength;
    uint len2 = contentInfoList.getLength();

    if (mComparisonMethod == ContentInfo::ComparisonMethod::none)
    {
      if (mArray == NULL  ||  mLength == mSize  ||  (mLength + len2) > mSize)
      {
        increaseSize(len1 + len2);
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

    uint newSize = len1 + len2;
    ContentInfoPtr *newArray = new ContentInfoPtr[newSize];
    uint a = 0;
    uint b = 0;

    contentInfoList.sort(mComparisonMethod);

    for (uint t=0; t<newSize; t++)
    {
      ContentInfo *cInfo1 = NULL;
      ContentInfo *cInfo2 = NULL;

      if (a < len1)
        cInfo1 = mArray[a];

      if (b < len2)
        cInfo2 = contentInfoList.getContentInfoByIndex(b);

      if (mReleaseObjects  && cInfo2 != NULL)
        cInfo2 = cInfo2->duplicate();

      if (cInfo1 != NULL  &&  cInfo2 == NULL)
      {
        newArray[t] = cInfo1;
        a++;
      }
      else
      if (cInfo1 == NULL  &&  cInfo2 != NULL)
      {
        newArray[t] = cInfo2;
        b++;
      }
      else
      if (cInfo1 != NULL  &&  cInfo2 != NULL)
      {
        if (cInfo1->compare(mComparisonMethod,cInfo2) <= 0)
        {
          newArray[t] = cInfo1;
          a++;
        }
        else
        {
          newArray[t] = cInfo2;
          b++;
        }
      }
    }

    mSize = newSize;
    mLength = newSize;

    delete mArray;
    mArray = newArray;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::increaseSize(uint newSize)
{
  FUNCTION_TRACE
  try
  {
//    printf("SET SIZE %u => %u\n",mSize,newSize);
    if (mArray == NULL)
    {
      mSize = newSize;
      mLength = 0;
      mArray = new ContentInfoPtr[mSize];

      for (uint t=0; t<mSize; t++)
      {
        mArray[t] = NULL;
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
        newArray[t] = NULL;
      }
    }

    delete mArray;
    mArray = newArray;
    mSize = newSize;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::clear()
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    if (mArray != NULL)
    {
      for (uint t=0; t<mSize; t++)
      {
        if (mArray[t] != NULL  &&  mReleaseObjects)
          delete mArray[t];

        mArray[t] = NULL;
      }

      delete mArray;
    }

    mArray = NULL;
    mSize = 0;
    mLength = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool ContentInfoList::containsSameForecastTimes()
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> timeList;

    AutoReadLock lock(&mModificationLock);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint ContentInfoList::deleteContentInfoByFileId(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL  &&  (info->mFileId == fileId  ||  (info->mFlags & CONTENT_INFO_DELETED) != 0))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint ContentInfoList::deleteMarkedContent()
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) != 0)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint ContentInfoList::markDeletedByFileId(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    uint cnt = 0;
    if (mComparisonMethod == ContentInfo::ComparisonMethod::file_message)
    {
      ContentInfo info;
      info.mFileId = fileId;
      info.mMessageIndex = 0;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);
      uint t = 0;
      if (idx >= 0)
        t = (uint)idx;

      while (t < mLength)
      {
        ContentInfo *info = mArray[t];
        if (info != NULL)
        {
          if (info->mFileId > fileId)
            return cnt;

          if (info->mFileId == fileId)
          {
            info->mFlags = info->mFlags | CONTENT_INFO_DELETED;
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




uint ContentInfoList::deleteContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL  &&  ((info->mFileId == fileId  &&  info->mMessageIndex == messageIndex)  ||  (info->mFlags & CONTENT_INFO_DELETED) != 0))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




uint ContentInfoList::deleteContentInfoByGroupFlags(uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL  &&  ((info->mGroupFlags & groupFlags) != 0  ||  (info->mFlags & CONTENT_INFO_DELETED) != 0))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint ContentInfoList::deleteContentInfoByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL  &&  (info->mProducerId == producerId  ||  (info->mFlags & CONTENT_INFO_DELETED) != 0))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint ContentInfoList::deleteContentInfoByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL  &&  (info->mGenerationId == generationId  ||  (info->mFlags & CONTENT_INFO_DELETED) != 0))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint ContentInfoList::deleteContentInfoByGenerationAndGeometry(uint generationId,T::GeometryId geometryId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL  &&  ((info->mGenerationId == generationId  &&  info->mGeometryId == geometryId)  ||  (info->mFlags & CONTENT_INFO_DELETED) != 0))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint ContentInfoList::deleteContentInfoByGenerationGeometryAndStartTime(uint generationId,T::GeometryId geometryId,std::string startTime)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL  &&  ((info->mGenerationId == generationId  &&  info->mGeometryId == geometryId  &&  info->mForecastTime == startTime)  ||  (info->mFlags & CONTENT_INFO_DELETED) != 0))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint ContentInfoList::deleteContentInfoBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL &&  (info->mSourceId == sourceId  ||  (info->mFlags & CONTENT_INFO_DELETED) != 0))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint ContentInfoList::deleteContentInfoByFileIdList(std::set<uint>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL  &&  (fileIdList.find(info->mFileId) != fileIdList.end()  ||  (info->mFlags & CONTENT_INFO_DELETED) != 0))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::keepContentInfoByGeometryIdList(std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL &&  ((info->mFlags & CONTENT_INFO_DELETED) != 0  ||  geometryIdList.find(info->mGeometryId) == geometryIdList.end()))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::keepContentInfoByGeometryId(T::GeometryId geometryId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL &&  ((info->mFlags & CONTENT_INFO_DELETED) != 0 ||  info->mGeometryId != geometryId))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::keepContentInfoByGroupFlags(uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL  && ((info->mFlags & CONTENT_INFO_DELETED) != 0 || (info->mGroupFlags & groupFlags) == 0))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::keepContentInfoByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL  &&  ((info->mFlags & CONTENT_INFO_DELETED) != 0 || info->mProducerId != producerId))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::keepContentInfoByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL  &&  ((info->mFlags & CONTENT_INFO_DELETED) != 0 || info->mGenerationId != generationId))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::keepContentInfoBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL  &&  ((info->mFlags & CONTENT_INFO_DELETED) != 0 ||  info->mSourceId != sourceId))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




uint ContentInfoList::registerContentInfoByServerAndFileId(uint serverId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long sf = 0;
    if (serverId >= 1  &&  serverId <= 64)
      sf = (unsigned long long)1 << (serverId-1);

    AutoWriteLock lock(&mModificationLock);

    ContentInfo contentInfo;
    contentInfo.mFileId = fileId;
    contentInfo.mMessageIndex = 0;

    int idx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (idx < 0 ||  (uint)idx >= mLength)
      return 0;

    uint count = 0;
    for (uint t=(uint)idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  info->mFileId == fileId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint ContentInfoList::unregisterContentInfoByServerId(uint serverId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long sf = 0;
    if (serverId >= 1  &&  serverId <= 64)
      sf = (unsigned long long)1 << (serverId-1);

    AutoWriteLock lock(&mModificationLock);
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mServerFlags & sf) != 0)
      {
        info->mServerFlags = info->mServerFlags - sf;
        count++;
      }
    }
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint ContentInfoList::unregisterContentInfoByServerAndFileId(uint serverId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long sf = 0;
    if (serverId >= 1  &&  serverId <= 64)
      sf = (unsigned long long)1 << (serverId-1);

    AutoWriteLock lock(&mModificationLock);

    ContentInfo contentInfo;
    contentInfo.mFileId = fileId;
    contentInfo.mMessageIndex = 0;

    int idx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (idx < 0 ||  (uint)idx >= mLength)
      return 0;

    uint count = 0;
    for (uint t=(uint)idx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  info->mFileId == fileId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentListByForecastTime(std::string forecastTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    ContentInfo *prevInfo = NULL;
    ContentInfo *nextInfo = NULL;

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0)
      {
        if (info->mForecastTime <= forecastTime)
        {
          if (prevInfo == NULL  ||  info->mForecastTime > prevInfo->mForecastTime)
            prevInfo = info;
        }
        else
        {
          if (nextInfo == NULL  ||  info->mForecastTime < nextInfo->mForecastTime)
            nextInfo = info;
        }
      }
    }

    if (prevInfo != NULL  &&  (nextInfo != NULL  ||  prevInfo->mForecastTime == forecastTime))
    {
      if (contentInfoList.getReleaseObjects())
        contentInfoList.addContentInfo(prevInfo->duplicate());
      else
        contentInfoList.addContentInfo(prevInfo);

      if (nextInfo != NULL  &&  prevInfo->mForecastTime != forecastTime)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}






int ContentInfoList::getClosestIndex(ContentInfo::ComparisonMethod comparisonMethod,ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    return getClosestIndexNoLock(comparisonMethod,contentInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int ContentInfoList::getClosestIndexNoLock(ContentInfo::ComparisonMethod comparisonMethod,ContentInfo& contentInfo)
{
  try
  {
    FUNCTION_TRACE
    if (mArray == NULL  ||  mLength == 0)
          return 0;

    if (comparisonMethod != mComparisonMethod)
    {
      for (uint t=0; t<mLength; t++)
      {
        if (mArray[t] != NULL)
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
    int high = (int)mLength - 1;
    int mid = 0;

    while (low <= high)
    {
      mid = (low + high) / 2;

      if (mArray[mid] == NULL)
        return 0;

      int res = mArray[mid]->compare(comparisonMethod,&contentInfo);

      if (res == 0)
      {
        while (mid > 0  &&  mArray[mid-1] != NULL  &&  mArray[mid-1]->compare(comparisonMethod,&contentInfo) == 0)
          mid--;

        return mid;
      }

      if (res < 0)
        low = mid + 1;
      else
        high = mid - 1;
    }

    if (mid >= 0  &&  mid < (int)mLength)
    {
      if (mArray[mid]->compare(comparisonMethod,&contentInfo) < 0)
      {
        while (mid < (int)mSize  &&  mArray[mid] != NULL  &&   mArray[mid]->compare(comparisonMethod,&contentInfo) < 0)
          mid++;

        return mid-1;
      }
      else
      {
        while (mid > 0  &&  mArray[mid] != NULL  &&   mArray[mid]->compare(comparisonMethod,&contentInfo) > 0)
          mid--;

        return mid;
      }
    }
    return 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ContentInfo* ContentInfoList::getContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    ContentInfo contentInfo;
    contentInfo.mFileId = fileId;
    contentInfo.mMessageIndex = messageIndex;

    int idx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (idx < 0  ||  (uint)idx >= mLength)
      return NULL;

    T::ContentInfo *info = mArray[idx];
    if (info != NULL &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mFileId == fileId  &&  info->mMessageIndex == messageIndex)
      return mArray[idx];

    return NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool ContentInfoList::getContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex,ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    ContentInfo contentInfo;
    contentInfo.mFileId = fileId;
    contentInfo.mMessageIndex = messageIndex;

    int idx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (idx < 0 ||  (uint)idx >= mLength)
      return NULL;

    T::ContentInfo *info = mArray[idx];
    if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mFileId == fileId  &&  info->mMessageIndex == messageIndex  )
    {
      contentInfo = *info;
      return true;
    }

    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}






ContentInfo* ContentInfoList::getContentInfoByIndex(uint index)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    if (index >= mLength)
      return NULL;

    return mArray[index];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ContentInfo* ContentInfoList::getContentInfoByIndexNoCheck(uint index)
{
  FUNCTION_TRACE
  try
  {
    return mArray[index];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ContentInfo* ContentInfoList::getContentInfoByParameterLevelInfo(T::ParameterLevelInfo& levelInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mParameterLevel == levelInfo.mLevel)
      {
        if ((levelInfo.mParameterKeyType == T::ParamKeyType::FMI_ID  &&  info->mFmiParameterId == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyType::FMI_NAME  &&  info->mFmiParameterName == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyType::NEWBASE_ID  &&  info->mNewbaseParameterId == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyType::NEWBASE_NAME  &&  info->mNewbaseParameterName == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyType::CDM_ID  &&  info->mCdmParameterId == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyType::CDM_NAME  &&  info->mCdmParameterName == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyType::GRIB_ID  &&  info->mGribParameterId == levelInfo.mParameterKey))
        {
          if ((levelInfo.mParameterLevelIdType == T::ParamLevelIdType::ANY || levelInfo.mParameterLevelIdType == T::ParamLevelIdType::FMI) &&
              info->mFmiParameterLevelId == levelInfo.mParameterLevelId)
            return info;

          if ((levelInfo.mParameterLevelIdType == T::ParamLevelIdType::ANY || levelInfo.mParameterLevelIdType == T::ParamLevelIdType::GRIB1) &&
              info->mGrib1ParameterLevelId == levelInfo.mParameterLevelId)
            return info;

          if ((levelInfo.mParameterLevelIdType == T::ParamLevelIdType::ANY || levelInfo.mParameterLevelIdType == T::ParamLevelIdType::GRIB2) &&
              info->mGrib2ParameterLevelId == levelInfo.mParameterLevelId)
            return info;
        }
      }
    }
    return NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByParameterLevelInfo(T::ParameterLevelInfo& levelInfo,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mParameterLevel == levelInfo.mLevel)
      {
        if ((levelInfo.mParameterKeyType == T::ParamKeyType::FMI_ID  &&  info->mFmiParameterId == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyType::FMI_NAME  &&  info->mFmiParameterName == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyType::NEWBASE_ID  &&  info->mNewbaseParameterId == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyType::NEWBASE_NAME  &&  info->mNewbaseParameterName == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyType::CDM_ID  &&  info->mCdmParameterId == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyType::CDM_NAME  &&  info->mCdmParameterName == levelInfo.mParameterKey) ||
            (levelInfo.mParameterKeyType == T::ParamKeyType::GRIB_ID  &&  info->mGribParameterId == levelInfo.mParameterKey))
        {
          if ((levelInfo.mParameterLevelIdType == T::ParamLevelIdType::ANY || levelInfo.mParameterLevelIdType == T::ParamLevelIdType::FMI) &&
              info->mFmiParameterLevelId == levelInfo.mParameterLevelId)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);
          }

          if ((levelInfo.mParameterLevelIdType == T::ParamLevelIdType::ANY || levelInfo.mParameterLevelIdType == T::ParamLevelIdType::GRIB1) &&
              info->mGrib1ParameterLevelId == levelInfo.mParameterLevelId)
          {
            if (contentInfoList.getReleaseObjects())
              contentInfoList.addContentInfo(info->duplicate());
            else
              contentInfoList.addContentInfo(info);
          }

          if ((levelInfo.mParameterLevelIdType == T::ParamLevelIdType::ANY || levelInfo.mParameterLevelIdType == T::ParamLevelIdType::GRIB2) &&
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoList(uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      printf("%s: Method not supported when records are not sorted by the fileId and the messageIndex fields!\n",__FUNCTION__);
      return;
    }

    AutoReadLock lock(&mModificationLock);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (startIdx < 0)
      startIdx = 0;

    for (uint t=(uint)startIdx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByGroupFlags(uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      printf("%s: Method not supported when records are not sorted by the fileId and the messageIndex fields!\n",__FUNCTION__);
      return;
    }

    AutoReadLock lock(&mModificationLock);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (startIdx < 0)
      startIdx = 0;

    for (uint t=(uint)startIdx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];

      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentParamKeyListByGenerationId(uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    paramKeyList.clear();

    AutoReadLock lock(&mModificationLock);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];

      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mGenerationId == generationId)
      {
        switch (parameterKeyType)
        {
          case T::ParamKeyType::FMI_ID:
            if (info->mFmiParameterId > " "  &&  paramKeyList.find(info->mFmiParameterId) == paramKeyList.end())
              paramKeyList.insert(info->mFmiParameterId);
            break;

          case T::ParamKeyType::FMI_NAME:
            if (info->mFmiParameterName > " "  &&  paramKeyList.find(info->mFmiParameterName) == paramKeyList.end())
              paramKeyList.insert(info->mFmiParameterName);
            break;

          case T::ParamKeyType::GRIB_ID:
            if (info->mGribParameterId > " "  &&  paramKeyList.find(info->mGribParameterId) == paramKeyList.end())
              paramKeyList.insert(info->mGribParameterId);
            break;

          case T::ParamKeyType::NEWBASE_ID:
            if (info->mNewbaseParameterId > " "  &&  paramKeyList.find(info->mNewbaseParameterId) == paramKeyList.end())
              paramKeyList.insert(info->mNewbaseParameterName);
            break;

          case T::ParamKeyType::NEWBASE_NAME:
            if (info->mNewbaseParameterName > " "  &&  paramKeyList.find(info->mNewbaseParameterName) == paramKeyList.end())
              paramKeyList.insert(info->mNewbaseParameterName);
            break;

          case T::ParamKeyType::CDM_ID:
            if (info->mCdmParameterId > " " &&  paramKeyList.find(info->mCdmParameterId) == paramKeyList.end())
              paramKeyList.insert(info->mCdmParameterId);
            break;

          case T::ParamKeyType::CDM_NAME:
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentGeometryIdListByGenerationId(uint generationId,std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    geometryIdList.clear();

    AutoReadLock lock(&mModificationLock);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];

      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mGenerationId == generationId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentGeometryIdList(std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    geometryIdList.clear();

    AutoReadLock lock(&mModificationLock);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];

      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getGenerationIdListByGeometryId(T::GeometryId geometryId,std::set<uint>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    generationIdList.clear();

    AutoReadLock lock(&mModificationLock);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];

      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mGeometryId == geometryId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByFileId(uint fileId,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::file_message)
    {
      ContentInfo info;
      info.mFileId = fileId;
      info.mMessageIndex = 0;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);
      uint t = 0;
      if (idx >= 0)
        t = (uint)idx;

      while (t < mLength)
      {
        ContentInfo *info = mArray[t];
        if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0)
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
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mFileId == fileId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByGeometryId(T::GeometryId geometryId,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mGeometryId == geometryId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByStartTime(std::string startTime,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::starttime_fmiId_fmiLevelId_level_file_message)
    {
      ContentInfo info;
      info.mForecastTime = startTime;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);
      uint t = 0;
      if (idx >= 0)
        t = (uint)idx;

      while (t < mLength)
      {
        ContentInfo *info = mArray[t];
        if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0)
        {
          if (info->mForecastTime > startTime)
            return;

          if (info->mForecastTime == startTime)
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
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mForecastTime == startTime)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void ContentInfoList::getContentInfoListByGribParameterId(T::ParamId gribParameterId,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mGribParameterId == gribParameterId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByGribParameterId(T::ParamId gribParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::gribId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mGribParameterId = gribParameterId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mGribParameterId.c_str(),gribParameterId.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (strcasecmp(info->mGribParameterId.c_str(),gribParameterId.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                  {
                    if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
            else
            {
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  && (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  && (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::gribId_level_starttime_file_message);
      list.getContentInfoListByGribParameterId(gribParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByGribParameterIdAndGenerationId(uint generationId,T::ParamId gribParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::gribId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mGribParameterId = gribParameterId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mGribParameterId.c_str(),gribParameterId.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (strcasecmp(info->mGribParameterId.c_str(),gribParameterId.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if (info->mGenerationId == generationId)
                  {
                    if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                        (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                        (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                        (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                    {
                      if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  && (prev->mFlags & CONTENT_INFO_DELETED) == 0 &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  && (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::gribId_level_starttime_file_message);
      list.getContentInfoListByGribParameterIdAndGenerationId(generationId,gribParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByGribParameterIdAndProducerId(uint producerId,T::ParamId gribParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::gribId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mGribParameterId = gribParameterId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mGribParameterId.c_str(),gribParameterId.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (info->mProducerId == producerId && strcasecmp(info->mGribParameterId.c_str(),gribParameterId.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                  {
                    if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
            else
            {
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::gribId_level_starttime_file_message);
      list.getContentInfoListByGribParameterIdAndProducerId(producerId,gribParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterId(T::ParamId fmiParameterId,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mFmiParameterId == fmiParameterId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterId(T::ParamId fmiParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterId = fmiParameterId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mFmiParameterId.c_str(),fmiParameterId.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (strcasecmp(info->mFmiParameterId.c_str(),fmiParameterId.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                  {
                    if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
            else
            {
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message);
      list.getContentInfoListByFmiParameterId(fmiParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterIdAndGenerationId(uint generationId,T::ParamId fmiParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterId = fmiParameterId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mFmiParameterId.c_str(),fmiParameterId.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (strcasecmp(info->mFmiParameterId.c_str(),fmiParameterId.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if (info->mGenerationId == generationId)
                  {
                    if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                        (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                        (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                        (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                    {
                      if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message);
      list.getContentInfoListByFmiParameterIdAndGenerationId(generationId,fmiParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void ContentInfoList::getContentInfoListByFmiParameterIdAndProducerId(uint producerId,T::ParamId fmiParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterId = fmiParameterId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mFmiParameterId.c_str(),fmiParameterId.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (info->mProducerId == producerId  &&  strcasecmp(info->mFmiParameterId.c_str(),fmiParameterId.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                  {
                    if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
            else
            {
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message);
      list.getContentInfoListByFmiParameterIdAndProducerId(producerId,fmiParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void ContentInfoList::getContentInfoListByFmiParameterName(std::string fmiParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterName = fmiParameterName;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mFmiParameterName.c_str(),fmiParameterName.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      //printf("INDEX %u %s %u\n",idx,mArray[t]->mFmiParameterName.c_str(),mArray[t]->mParameterLevel);

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            //printf("-- INDEX %u %s %u\n",idx,mArray[t]->mFmiParameterName.c_str(),mArray[t]->mParameterLevel);
            if (strcasecmp(info->mFmiParameterName.c_str(),fmiParameterName.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                  {
                    if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
            else
            {
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::fmiName_level_starttime_file_message);
      list.getContentInfoListByFmiParameterName(fmiParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void ContentInfoList::getContentInfoListByFmiParameterNameAndGenerationId(uint generationId,std::string fmiParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterName = fmiParameterName;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mFmiParameterName.c_str(),fmiParameterName.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (strcasecmp(info->mFmiParameterName.c_str(),fmiParameterName.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if (info->mGenerationId == generationId)
                  {
                    if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                        (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                        (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                        (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                    {
                      if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::fmiName_level_starttime_file_message);
      list.getContentInfoListByFmiParameterNameAndGenerationId(generationId,fmiParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterNameAndProducerId(uint producerId,std::string fmiParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterName = fmiParameterName;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mFmiParameterName.c_str(),fmiParameterName.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (info->mProducerId == producerId && strcasecmp(info->mFmiParameterName.c_str(),fmiParameterName.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                  {
                    if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
            else
            {
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::fmiName_level_starttime_file_message);
      list.getContentInfoListByFmiParameterNameAndProducerId(producerId,fmiParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByNewbaseParameterId(T::ParamId newbaseParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterId = newbaseParameterId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (strcasecmp(info->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                  {
                    if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
            else
            {
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::newbaseId_level_starttime_file_message);
      list.getContentInfoListByNewbaseParameterId(newbaseParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByNewbaseParameterIdAndGenerationId(uint generationId,T::ParamId newbaseParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterId = newbaseParameterId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (strcasecmp(info->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if (info->mGenerationId == generationId)
                  {
                    if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                        (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                        (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                        (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                    {
                      if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::newbaseId_level_starttime_file_message);
      list.getContentInfoListByNewbaseParameterIdAndGenerationId(generationId,newbaseParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByNewbaseParameterIdAndProducerId(uint producerId,T::ParamId newbaseParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterId = newbaseParameterId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (info->mProducerId == producerId  &&  strcasecmp(info->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                  {
                    if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
            else
            {
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::newbaseId_level_starttime_file_message);
      list.getContentInfoListByNewbaseParameterIdAndProducerId(producerId,newbaseParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void ContentInfoList::getContentInfoListByNewbaseParameterName(std::string newbaseParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterName = newbaseParameterName;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (strcasecmp(info->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                  {
                    if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
            else
            {
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message);
      list.getContentInfoListByNewbaseParameterName(newbaseParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByNewbaseParameterNameAndGenerationId(uint generationId,std::string newbaseParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterName = newbaseParameterName;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (strcasecmp(info->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if (info->mGenerationId == generationId)
                  {
                    if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                        (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                        (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                        (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                    {
                      if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message);
      list.getContentInfoListByNewbaseParameterNameAndGenerationId(generationId,newbaseParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByNewbaseParameterNameAndProducerId(uint producerId,std::string newbaseParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterName = newbaseParameterName;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (info->mProducerId == producerId &&  strcasecmp(info->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                  {
                    if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
            else
            {
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message);
      list.getContentInfoListByNewbaseParameterNameAndProducerId(producerId,newbaseParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByCdmParameterId(T::ParamId cdmParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterId = cdmParameterId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mCdmParameterId.c_str(),cdmParameterId.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (strcasecmp(info->mCdmParameterId.c_str(),cdmParameterId.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                  {
                    if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
            else
            {
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::cdmId_level_starttime_file_message);
      list.getContentInfoListByCdmParameterId(cdmParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByCdmParameterIdAndGenerationId(uint generationId,T::ParamId cdmParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterId = cdmParameterId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mCdmParameterId.c_str(),cdmParameterId.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (strcasecmp(info->mCdmParameterId.c_str(),cdmParameterId.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if (info->mGenerationId == generationId)
                  {
                    if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                        (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                        (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                        (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                    {
                      if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::cdmId_level_starttime_file_message);
      list.getContentInfoListByCdmParameterIdAndGenerationId(generationId,cdmParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByCdmParameterIdAndProducerId(uint producerId,T::ParamId cdmParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterId = cdmParameterId;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mCdmParameterId.c_str(),cdmParameterId.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (info->mProducerId == producerId  &&  strcasecmp(info->mCdmParameterId.c_str(),cdmParameterId.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                  {
                    if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
            else
            {
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::cdmId_level_starttime_file_message);
      list.getContentInfoListByCdmParameterIdAndProducerId(producerId,cdmParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByCdmParameterName(std::string cdmParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterName = cdmParameterName;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mCdmParameterName.c_str(),cdmParameterName.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (strcasecmp(info->mCdmParameterName.c_str(),cdmParameterName.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                  {
                    if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
            else
            {
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  && (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::cdmName_level_starttime_file_message);
      list.getContentInfoListByCdmParameterName(cdmParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByCdmParameterNameAndGenerationId(uint generationId,std::string cdmParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterName = cdmParameterName;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mCdmParameterName.c_str(),cdmParameterName.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (strcasecmp(info->mCdmParameterName.c_str(),cdmParameterName.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if (info->mGenerationId == generationId)
                  {
                    if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                        (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                        (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                        (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                    {
                      if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                        prev = info;

                      if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  &&  (prev->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  &&  (next->mFlags & CONTENT_INFO_DELETED) == 0  &&  (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::cdmName_level_starttime_file_message);
      list.getContentInfoListByCdmParameterNameAndGenerationId(generationId,cdmParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByCdmParameterNameAndProducerId(uint producerId,std::string cdmParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterName = cdmParameterName;
      //info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if ((uint)idx < mLength  &&  mArray[idx] != NULL  &&  strcasecmp(mArray[idx]->mCdmParameterName.c_str(),cdmParameterName.c_str()) != 0)
        idx++;

      uint t = (uint)idx;

      if (parameterLevelIdType != T::ParamLevelIdType::IGNORE)
      {
        if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_BEFORE) != 0)
        {
          // Finding the start position of the closest level that is smaller than the "minLevel".
          // We have to include the content from this level too, because this information is needed
          // during the vertical interpolation.

          if (t > 0)
          {
            if (mArray[t] != NULL  &&  mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength  &&  mArray[t] != NULL)
      {
        ContentInfo *prev = NULL;
        ContentInfo *next = NULL;

        uint level = mArray[t]->mParameterLevel;
        if (parameterLevelIdType != T::ParamLevelIdType::IGNORE  &&  level > maxLevel)
        {
          // Checking if we need to include the content that is one level above the "maxLevel".
          // This information might be needed  during the vertical interpolation.

          levelAfterCount++;
          if ((requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_LEVEL_AFTER) == 0  ||  levelAfterCount > 1)
            return;
        }

        while (t < mLength  &&  mArray[t] != NULL  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if ((info->mFlags & CONTENT_INFO_DELETED) == 0)
          {
            if (info->mProducerId == producerId  &&  strcasecmp(info->mCdmParameterName.c_str(),cdmParameterName.c_str()) == 0)
            {
              if (forecastType < 0 || (info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber))
              {
                if (geometryId < 0  ||  info->mGeometryId == geometryId)
                {
                  if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                      (info->mFmiParameterLevelId == parameterLevelId  &&  (parameterLevelIdType == T::ParamLevelIdType::FMI || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib1ParameterLevelId == parameterLevelId && (parameterLevelIdType == T::ParamLevelIdType::GRIB1 || parameterLevelIdType == T::ParamLevelIdType::ANY)) ||
                      (info->mGrib2ParameterLevelId == parameterLevelId  && (parameterLevelIdType == T::ParamLevelIdType::GRIB2 || parameterLevelIdType == T::ParamLevelIdType::ANY)))
                  {
                    if (info->mForecastTime < startTime  && (prev == NULL || prev->mForecastTime < info->mForecastTime))
                      prev = info;

                    if (info->mForecastTime > endTime  && (next == NULL || next->mForecastTime > info->mForecastTime))
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
            else
            {
              if (t > (uint)idx)
                t = mLength;
            }
          }
          t++;
        }

        if (prev != NULL  && (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
        {
          // We need to add the previous entry before the start time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(prev->duplicate());
          else
            contentInfoList.addContentInfo(prev);
        }

        if (next != NULL  && (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
        {
          // We need to add the next entry after the end time. Notice that this must be
          // done in all requested levels.

          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(next->duplicate());
          else
            contentInfoList.addContentInfo(next);
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
      list.sort(ContentInfo::ComparisonMethod::cdmName_level_starttime_file_message);
      list.getContentInfoListByCdmParameterNameAndProducerId(producerId,cdmParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByProducerId(uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      printf("%s: Method not supported when records are not sorted by the fileId and the messageIndex fields!\n",__FUNCTION__);
      return;
    }

    AutoReadLock lock(&mModificationLock);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (startIdx < 0)
      startIdx = 0;

    for (uint t=(uint)startIdx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mProducerId == producerId && (info->mFileId > startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex >= startMessageIndex)))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByGenerationId(uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      printf("%s: Method not supported when records are not sorted by the fileId and the messageIndex fields!\n",__FUNCTION__);
      return;
    }

    AutoReadLock lock(&mModificationLock);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (startIdx < 0)
      startIdx = 0;

    for (uint t=(uint)startIdx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mGenerationId == generationId && (info->mFileId > startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex >= startMessageIndex)))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByGenerationAndGeometryId(uint generationId,T::GeometryId geometryId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      printf("%s: Method not supported when records are not sorted by the fileId and the messageIndex fields!\n",__FUNCTION__);
      return;
    }

    AutoReadLock lock(&mModificationLock);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (startIdx < 0)
      startIdx = 0;

    for (uint t=(uint)startIdx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mGenerationId == generationId  &&  info->mGeometryId == geometryId && (info->mFileId > startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex >= startMessageIndex)))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByGenerationId(uint generationId,std::string startTime,std::string endTime,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mGenerationId == generationId  &&  info->mForecastTime >= startTime  &&  info->mForecastTime <= endTime)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByServerId(uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      printf("%s: Method not supported when records are not sorted by the fileId and the messageIndex fields!\n",__FUNCTION__);
      return;
    }

    unsigned long long sf = 0;
    if (serverId >= 1  &&  serverId <= 64)
      sf = (unsigned long long)1 << (serverId-1);

    AutoReadLock lock(&mModificationLock);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (startIdx < 0)
      startIdx = 0;

    for (uint t=(uint)startIdx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  (info->mServerFlags & sf) != 0 && (info->mFileId > startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex >= startMessageIndex)))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListBySourceId(uint sourceId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mComparisonMethod != ContentInfo::ComparisonMethod::file_message)
    {
      printf("%s: Method not supported when records are not sorted by the fileId and the messageIndex fields!\n",__FUNCTION__);
      return;
    }


    AutoReadLock lock(&mModificationLock);
    ContentInfo contentInfo;
    contentInfo.mFileId = startFileId;
    contentInfo.mMessageIndex = startMessageIndex;

    int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (startIdx < 0)
      startIdx = 0;

    for (uint t=(uint)startIdx; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mSourceId == sourceId && (info->mFileId > startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex >= startMessageIndex)))
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getContentInfoListByServerAndFileId(uint serverId,uint fileId,ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long sf = 0;
    if (serverId >= 1  &&  serverId <= 64)
      sf = (unsigned long long)1 << (serverId-1);

    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::file_message)
    {
      ContentInfo contentInfo;
      contentInfo.mFileId = fileId;
      contentInfo.mMessageIndex = 0;
      int startIdx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
      if (startIdx < 0)
        startIdx = 0;

      for (uint t=(uint)startIdx; t<mLength; t++)
      {
        ContentInfo *info = mArray[t];
        if (info != NULL  &&  info->mFileId > fileId)
          return;

        if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mFileId == fileId  &&  (info->mServerFlags & sf) != 0)
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
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mFileId == fileId  &&  (info->mServerFlags & sf) != 0)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getFmiParamLevelIdListByFmiParameterId(T::ParamId fmiParameterId,std::vector<T::ParamLevelId>& paramLevelIdList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0)
      {
        uint vLen = (uint)paramLevelIdList.size();
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}






void ContentInfoList::getParamLevelListByFmiLevelId(T::ParamLevelId paramLevelId,std::vector<T::ParamLevel>& paramLevelList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0)
      {
        uint vLen = (uint)paramLevelList.size();
        uint c = 0;
        while (c < vLen)
        {
          if (info->mFmiParameterLevelId == paramLevelId)
          {
            if (paramLevelList[c] == info->mParameterLevel)
            {
              c = vLen;
            }
            else
            if (paramLevelList[c] > info->mParameterLevel)
            {
              paramLevelList.insert(paramLevelList.begin() + c,info->mParameterLevel);
              c = vLen;
            }
          }
          c++;
        }
        if (c == vLen)
        {
          if (info->mFmiParameterLevelId == paramLevelId)
          paramLevelList.push_back(info->mParameterLevel);
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getParamLevelInfoListByFmiParameterId(T::ParamId fmiParameterId,ParameterLevelInfoList& parameterLevelInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mFmiParameterId == fmiParameterId)
      {
        ParameterLevelInfo *pInfo = parameterLevelInfoList.getParameterLevelInfo(T::ParamKeyType::FMI_ID,
            info->mFmiParameterId,T::ParamLevelIdType::FMI,info->mFmiParameterLevelId,info->mParameterLevel);

        if (pInfo == NULL)
        {
          pInfo = new ParameterLevelInfo(T::ParamKeyType::FMI_ID,info->mFmiParameterId,T::ParamLevelIdType::FMI,info->mFmiParameterLevelId,info->mParameterLevel);
          parameterLevelInfoList.addParameterLevelInfo(pInfo);
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getStartTimeList(std::set<std::string>& startTimeList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0)
      {
        if (info != NULL)
        {
          if (startTimeList.find(info->mForecastTime) == startTimeList.end())
          {
            startTimeList.insert(info->mForecastTime);
          }
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::getStartTimeListByGenerationAndGeometry(uint generationId,T::GeometryId geometryId,std::set<std::string>& startTimeList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL)
      {
        if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0  &&  info->mGenerationId == generationId  &&  info->mGeometryId == geometryId)
        {
          if (startTimeList.find(info->mForecastTime) == startTimeList.end())
          {
            startTimeList.insert(info->mForecastTime);
          }
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint ContentInfoList::getLength()
{
  FUNCTION_TRACE
  try
  {
    return mLength;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint ContentInfoList::getSize()
{
  FUNCTION_TRACE
  try
  {
    return mSize;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::lock()
{
  FUNCTION_TRACE
  try
  {
    return mModificationLock.lock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void ContentInfoList::unlock()
{
  FUNCTION_TRACE
  try
  {
    return mModificationLock.unlock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::setComparisonMethod(ContentInfo::ComparisonMethod comparisonMethod)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    mComparisonMethod = comparisonMethod;
    if (mLength == 0)
      return;

    contentInfo_comparisonMethod = comparisonMethod;

    qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void ContentInfoList::sort(ContentInfo::ComparisonMethod comparisonMethod)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    mComparisonMethod = comparisonMethod;

    AutoThreadLock globalLock(&ContentInfoList_sortLock);
    contentInfo_comparisonMethod = comparisonMethod;

    qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::writeToFile(std::string filename)
{
  FUNCTION_TRACE
  try
  {
    writeToFile(filename,"w");
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::writeToFile(std::string filename,const char *filemode)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    FILE *file = fopen(filename.c_str(),filemode);
    if (file == NULL)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot create the file!");
      exception.addParameter("Filename",filename);
      throw exception;
    }

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0)
        fprintf(file,"%s\n",info->getCsv().c_str());
    }

    fclose(file);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    stream << space(level) << "ContentInfoList\n";
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & CONTENT_INFO_DELETED) == 0)
        info->print(stream,level+1,optionFlags);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




}
}
