#include "ContentInfoList.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"
#include "grid-files/common/AutoThreadLock.h"
#include "grid-files/common/AutoReadLock.h"
#include "grid-files/common/AutoWriteLock.h"
#include "RequestFlags.h"


namespace SmartMet
{
namespace T
{



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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ContentInfoList::ContentInfoList(ContentInfoList& contentInfoList)
{
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ContentInfoList::~ContentInfoList()
{
  try
  {
    clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::operator=(ContentInfoList& contentInfoList)
{
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::addContentInfo(ContentInfo *contentInfo)
{
  try
  {
    if (mArray == NULL  ||  mLength == mSize)
    {
      increaseSize(mSize + mSize/5 + 10);
    }

    AutoWriteLock lock(&mModificationLock);

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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::increaseSize(uint newSize)
{
  try
  {
//    printf("SET SIZE %u => %u\n",mSize,newSize);
    AutoWriteLock lock(&mModificationLock);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::clear()
{
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





uint ContentInfoList::deleteContentInfoByFileId(uint fileId)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);

    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info->mFileId == fileId)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





uint ContentInfoList::deleteContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);

    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info->mFileId == fileId  &&  info->mMessageIndex == messageIndex)
      {
        if (mReleaseObjects)
          delete info;

        mArray[t] = NULL;
        count++;
      }
      else
      {
        mArray[t] = NULL;
        mArray[p] = info;
        p++;
      }
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




uint ContentInfoList::deleteContentInfoByGroupFlags(uint groupFlags)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if ((info->mGroupFlags & groupFlags) != 0)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





uint ContentInfoList::deleteContentInfoByProducerId(uint producerId)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info->mProducerId == producerId)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





uint ContentInfoList::deleteContentInfoByGenerationId(uint generationId)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint p = 0;
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info->mGenerationId == generationId)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





uint ContentInfoList::registerContentInfoByServerAndFileId(uint serverId,uint fileId)
{
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
      if (info->mFileId == fileId)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





uint ContentInfoList::unregisterContentInfoByServerId(uint serverId)
{
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
      if ((info->mServerFlags & sf) != 0)
      {
        info->mServerFlags = info->mServerFlags - sf;
        count++;
      }
    }
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





uint ContentInfoList::unregisterContentInfoByServerAndFileId(uint serverId,uint fileId)
{
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
      if (info->mFileId == fileId)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ContentInfoList::getClosestIndex(ContentInfo::ComparisonMethod comparisonMethod,ContentInfo& contentInfo)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    return getClosestIndexNoLock(comparisonMethod,contentInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ContentInfoList::getClosestIndexNoLock(ContentInfo::ComparisonMethod comparisonMethod,ContentInfo& contentInfo)
{
  try
  {
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
      int res = mArray[mid]->compare(comparisonMethod,&contentInfo);

      if (res == 0)
      {
        while (mid > 0  &&  mArray[mid-1]->compare(comparisonMethod,&contentInfo) == 0)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ContentInfo* ContentInfoList::getContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    ContentInfo contentInfo;
    contentInfo.mFileId = fileId;
    contentInfo.mMessageIndex = messageIndex;

    int idx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (idx < 0 ||  (uint)idx >= mLength)
      return NULL;

    if (mArray[idx]->mFileId == fileId  &&  mArray[idx]->mMessageIndex == messageIndex)
      return mArray[idx];

    return NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





bool ContentInfoList::getContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex,ContentInfo& contentInfo)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    ContentInfo contentInfo;
    contentInfo.mFileId = fileId;
    contentInfo.mMessageIndex = messageIndex;

    int idx = getClosestIndexNoLock(ContentInfo::ComparisonMethod::file_message,contentInfo);
    if (idx < 0 ||  (uint)idx >= mLength)
      return NULL;

    if (mArray[idx]->mFileId == fileId  &&  mArray[idx]->mMessageIndex == messageIndex)
    {
      contentInfo = *mArray[idx];
      return true;
    }

    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}



ContentInfo* ContentInfoList::getContentInfoByIndex(uint index)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    if (index >= mLength)
      return NULL;

    return mArray[index];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ContentInfo* ContentInfoList::getContentInfoByIndexNoCheck(uint index)
{
  try
  {
    return mArray[index];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoList(uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
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
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByGroupFlags(uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
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

      if ((info->mGroupFlags & groupFlags) != 0 && (info->mFileId > startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex >= startMessageIndex)))
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByFileId(uint fileId,ContentInfoList& contentInfoList)
{
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
        if (info->mFileId > fileId)
          return;

        if (info->mFileId == fileId)
        {
          if (contentInfoList.getReleaseObjects())
            contentInfoList.addContentInfo(info->duplicate());
          else
            contentInfoList.addContentInfo(info);
        }
        t++;
      }
      return;
    }


    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info->mFileId == fileId)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ContentInfoList::getContentInfoListByGribParameterId(T::ParamId gribParameterId,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info->mGribParameterId == gribParameterId)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByGribParameterId(T::ParamId gribParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::gribId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mGribParameterId = gribParameterId;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mGribParameterId.c_str(),gribParameterId.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (strcasecmp(info->mGribParameterId.c_str(),gribParameterId.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::gribId_level_starttime_file_message);
      list.getContentInfoListByGribParameterId(gribParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByGribParameterIdAndGenerationId(uint generationId,T::ParamId gribParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::gribId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mGribParameterId = gribParameterId;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mGribParameterId.c_str(),gribParameterId.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (info->mGenerationId == generationId && strcasecmp(info->mGribParameterId.c_str(),gribParameterId.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::gribId_level_starttime_file_message);
      list.getContentInfoListByGribParameterIdAndGenerationId(generationId,gribParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByGribParameterIdAndProducerId(uint producerId,T::ParamId gribParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::gribId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mGribParameterId = gribParameterId;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mGribParameterId.c_str(),gribParameterId.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (info->mProducerId == producerId && strcasecmp(info->mGribParameterId.c_str(),gribParameterId.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::gribId_level_starttime_file_message);
      list.getContentInfoListByGribParameterIdAndProducerId(producerId,gribParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterId(T::ParamId fmiParameterId,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info->mFmiParameterId == fmiParameterId)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterId(T::ParamId fmiParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterId = fmiParameterId;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mFmiParameterId.c_str(),fmiParameterId.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (strcasecmp(info->mFmiParameterId.c_str(),fmiParameterId.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message);
      list.getContentInfoListByFmiParameterId(fmiParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByFmiParameterIdAndGenerationId(uint generationId,T::ParamId fmiParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterId = fmiParameterId;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mFmiParameterId.c_str(),fmiParameterId.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (info->mGenerationId == generationId && strcasecmp(info->mFmiParameterId.c_str(),fmiParameterId.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message);
      list.getContentInfoListByFmiParameterIdAndGenerationId(generationId,fmiParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ContentInfoList::getContentInfoListByFmiParameterIdAndProducerId(uint producerId,T::ParamId fmiParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterId = fmiParameterId;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mFmiParameterId.c_str(),fmiParameterId.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (info->mProducerId == producerId  &&  strcasecmp(info->mFmiParameterId.c_str(),fmiParameterId.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message);
      list.getContentInfoListByFmiParameterIdAndProducerId(producerId,fmiParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ContentInfoList::getContentInfoListByFmiParameterName(std::string fmiParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterName = fmiParameterName;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mFmiParameterName.c_str(),fmiParameterName.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          //printf("-- INDEX %u %s %u\n",idx,mArray[t]->mFmiParameterName.c_str(),mArray[t]->mParameterLevel);
          if (strcasecmp(info->mFmiParameterName.c_str(),fmiParameterName.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::fmiName_level_starttime_file_message);
      list.getContentInfoListByFmiParameterName(fmiParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ContentInfoList::getContentInfoListByFmiParameterNameAndGenerationId(uint generationId,std::string fmiParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterName = fmiParameterName;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mFmiParameterName.c_str(),fmiParameterName.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (info->mGenerationId == generationId  &&  strcasecmp(info->mFmiParameterName.c_str(),fmiParameterName.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::fmiName_level_starttime_file_message);
      list.getContentInfoListByFmiParameterNameAndGenerationId(generationId,fmiParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ContentInfoList::getContentInfoListByFmiParameterNameAndProducerId(uint producerId,std::string fmiParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::fmiName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mFmiParameterName = fmiParameterName;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mFmiParameterName.c_str(),fmiParameterName.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (info->mProducerId == producerId && strcasecmp(info->mFmiParameterName.c_str(),fmiParameterName.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::fmiName_level_starttime_file_message);
      list.getContentInfoListByFmiParameterNameAndProducerId(producerId,fmiParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByNewbaseParameterId(T::ParamId newbaseParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterId = newbaseParameterId;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (strcasecmp(info->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::newbaseId_level_starttime_file_message);
      list.getContentInfoListByNewbaseParameterId(newbaseParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByNewbaseParameterIdAndGenerationId(uint generationId,T::ParamId newbaseParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterId = newbaseParameterId;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (info->mGenerationId == generationId &&  strcasecmp(info->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::newbaseId_level_starttime_file_message);
      list.getContentInfoListByNewbaseParameterIdAndGenerationId(generationId,newbaseParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByNewbaseParameterIdAndProducerId(uint producerId,T::ParamId newbaseParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterId = newbaseParameterId;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (info->mProducerId == producerId  &&  strcasecmp(info->mNewbaseParameterId.c_str(),newbaseParameterId.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::newbaseId_level_starttime_file_message);
      list.getContentInfoListByNewbaseParameterIdAndProducerId(producerId,newbaseParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ContentInfoList::getContentInfoListByNewbaseParameterName(std::string newbaseParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterName = newbaseParameterName;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (strcasecmp(info->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message);
      list.getContentInfoListByNewbaseParameterName(newbaseParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




#if 0
void ContentInfoList::getContentInfoListByNewbaseParameterName(std::string newbaseParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    ContentInfo *prev = NULL;
    ContentInfo *next = NULL;

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message)
    {
      ContentInfo info;
      info.mNewbaseParameterName = newbaseParameterName;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      for (uint t=(uint)idx; t<mLength; t++)
      {
        ContentInfo *info = mArray[t];
        if (strcasecmp(info->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) == 0  && ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel)))
        {
          if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
              (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
              (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
              (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
              (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
          {
            if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
              prev = info;

            if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
              next = info;

            if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
            {
              if (contentInfoList.getReleaseObjects())
                contentInfoList.addContentInfo(info->duplicate());
              else
                contentInfoList.addContentInfo(info);
            }
          }
        }

        if (t > (uint)idx)
        {
          if (strcasecmp(info->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) != 0  || (strcasecmp(info->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) == 0  &&  info->mParameterLevel > maxLevel))
          {
            t = mLength;
          }
        }
      }
    }
    else
    {

      for (uint t=0; t<mLength; t++)
      {
        ContentInfo *info = mArray[t];
        if (strcasecmp(info->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) == 0  && ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) || (info->mParameterLevel >= minLevel  &&  info->mParameterLevel <= maxLevel)))
        {
          if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
            (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
            (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
            (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
            (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
          {
            if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
              prev = info;

            if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
              next = info;

            if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
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

    if (prev != NULL  && (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_BEFORE))
    {
      if (contentInfoList.getReleaseObjects())
        contentInfoList.addContentInfo(prev->duplicate());
      else
        contentInfoList.addContentInfo(prev);
    }

    if (next != NULL  && (requestFlags & (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER))
    {
      if (contentInfoList.getReleaseObjects())
        contentInfoList.addContentInfo(next->duplicate());
      else
        contentInfoList.addContentInfo(next);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}
#endif



void ContentInfoList::getContentInfoListByNewbaseParameterNameAndGenerationId(uint generationId,std::string newbaseParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterName = newbaseParameterName;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (info->mGenerationId == generationId  &&  strcasecmp(info->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message);
      list.getContentInfoListByNewbaseParameterNameAndGenerationId(generationId,newbaseParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByNewbaseParameterNameAndProducerId(uint producerId,std::string newbaseParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mNewbaseParameterName = newbaseParameterName;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (info->mProducerId == producerId &&  strcasecmp(info->mNewbaseParameterName.c_str(),newbaseParameterName.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message);
      list.getContentInfoListByNewbaseParameterNameAndProducerId(producerId,newbaseParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByCdmParameterId(T::ParamId cdmParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterId = cdmParameterId;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mCdmParameterId.c_str(),cdmParameterId.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (strcasecmp(info->mCdmParameterId.c_str(),cdmParameterId.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::cdmId_level_starttime_file_message);
      list.getContentInfoListByCdmParameterId(cdmParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByCdmParameterIdAndGenerationId(uint generationId,T::ParamId cdmParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterId = cdmParameterId;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mCdmParameterId.c_str(),cdmParameterId.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (info->mGenerationId == generationId && strcasecmp(info->mCdmParameterId.c_str(),cdmParameterId.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::cdmId_level_starttime_file_message);
      list.getContentInfoListByCdmParameterIdAndGenerationId(generationId,cdmParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ContentInfoList::getContentInfoListByCdmParameterIdAndProducerId(uint producerId,T::ParamId cdmParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmId_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterId = cdmParameterId;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mCdmParameterId.c_str(),cdmParameterId.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (info->mProducerId == producerId  &&  strcasecmp(info->mCdmParameterId.c_str(),cdmParameterId.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.sort(ContentInfo::ComparisonMethod::cdmId_level_starttime_file_message);
      list.getContentInfoListByCdmParameterIdAndProducerId(producerId,cdmParameterId,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ContentInfoList::getContentInfoListByCdmParameterName(std::string cdmParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterName = cdmParameterName;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mCdmParameterName.c_str(),cdmParameterName.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (strcasecmp(info->mCdmParameterName.c_str(),cdmParameterName.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.getContentInfoListByCdmParameterName(cdmParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ContentInfoList::getContentInfoListByCdmParameterNameAndGenerationId(uint generationId,std::string cdmParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterName = cdmParameterName;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mCdmParameterName.c_str(),cdmParameterName.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (info->mGenerationId == generationId  &&  strcasecmp(info->mCdmParameterName.c_str(),cdmParameterName.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.getContentInfoListByCdmParameterNameAndGenerationId(generationId,cdmParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByCdmParameterNameAndProducerId(uint producerId,std::string cdmParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    if (mComparisonMethod == ContentInfo::ComparisonMethod::cdmName_level_starttime_file_message)
    {
      // ### This search is possible only if the content list is sorted as we want.

      ContentInfo info;
      info.mCdmParameterName = cdmParameterName;
      info.mParameterLevel = minLevel;
      int idx = getClosestIndexNoLock(mComparisonMethod,info);

      if (strcasecmp(mArray[idx]->mCdmParameterName.c_str(),cdmParameterName.c_str()) != 0)
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
            if (mArray[t]->mParameterLevel == minLevel)
              t--;

            while (t > 0  &&  mArray[t]->mParameterLevel == mArray[t-1]->mParameterLevel)
              t--;
          }
        }
        else
        {
          // Finding the start position of the level that is equal or bigger than the "minLevel".

          while (t < mLength  &&  mArray[t]->mParameterLevel < minLevel)
            t++;
        }
      }

      uint levelAfterCount = 0;

      while (t < mLength)
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

        while (t < mLength  &&  mArray[t]->mParameterLevel == level)
        {
          ContentInfo *info = mArray[t];
          if (info->mProducerId == producerId  &&  strcasecmp(info->mCdmParameterName.c_str(),cdmParameterName.c_str()) == 0)
          {
            if ((parameterLevelIdType == T::ParamLevelIdType::IGNORE) ||
                (parameterLevelIdType == T::ParamLevelIdType::ANY) ||
                (parameterLevelIdType == T::ParamLevelIdType::FMI  &&  info->mFmiParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB1 &&  info->mGrib1ParameterLevelId == parameterLevelId) ||
                (parameterLevelIdType == T::ParamLevelIdType::GRIB2 &&  info->mGrib2ParameterLevelId == parameterLevelId))
            {
              if (info->mStartTime < startTime  && (prev == NULL || prev->mStartTime < info->mStartTime))
                prev = info;

              if (info->mStartTime > endTime  && (next == NULL || next->mStartTime > info->mStartTime))
                next = info;

              if (info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
              {
                if (contentInfoList.getReleaseObjects())
                  contentInfoList.addContentInfo(info->duplicate());
                else
                  contentInfoList.addContentInfo(info);
              }
            }
          }
          else
          {
            if (t > (uint)idx)
              t = mLength;
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
      list.getContentInfoListByCdmParameterNameAndProducerId(producerId,cdmParameterName,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByProducerId(uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
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
      if (info->mProducerId == producerId && (info->mFileId > startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex >= startMessageIndex)))
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByGenerationId(uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
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
      if (info->mGenerationId == generationId && (info->mFileId > startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex >= startMessageIndex)))
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByGenerationId(uint generationId,std::string startTime,std::string endTime,ContentInfoList& contentInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      if (info->mGenerationId == generationId  &&  info->mStartTime >= startTime  &&  info->mStartTime <= endTime)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByServerId(uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList)
{
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
      if ((info->mServerFlags & sf) != 0 && (info->mFileId > startFileId  || (info->mFileId == startFileId  &&  info->mMessageIndex >= startMessageIndex)))
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::getContentInfoListByServerAndFileId(uint serverId,uint fileId,ContentInfoList& contentInfoList)
{
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
        if (info->mFileId > fileId)
          return;

        if (info->mFileId == fileId  &&  (info->mServerFlags & sf) != 0)
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
      if (info->mFileId == fileId  &&  (info->mServerFlags & sf) != 0)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





uint ContentInfoList::getLength()
{
  try
  {
    return mLength;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





uint ContentInfoList::getSize()
{
  try
  {
    return mSize;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ContentInfoList::lock()
{
  try
  {
    return mModificationLock.lock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ContentInfoList::unlock()
{
  try
  {
    return mModificationLock.unlock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





bool ContentInfoList::getReleaseObjects()
{
  try
  {
    return mReleaseObjects;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ContentInfoList::setReleaseObjects(bool releaseObjects)
{
  try
  {
    mReleaseObjects = releaseObjects;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::setComparisonMethod(ContentInfo::ComparisonMethod comparisonMethod)
{
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ContentInfoList::sort(ContentInfo::ComparisonMethod comparisonMethod)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);
    mComparisonMethod = comparisonMethod;

    contentInfo_comparisonMethod = comparisonMethod;

    qsort(mArray,mLength,sizeof(ContentInfoPtr),contentInfo_compare);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::writeToFile(std::string filename)
{
  try
  {
    writeToFile(filename,"w");
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::writeToFile(std::string filename,const char *filemode)
{
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
      fprintf(file,"%s\n",info->getCsv().c_str());
    }

    fclose(file);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ContentInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    stream << space(level) << "ContentInfoList\n";
    for (uint t=0; t<mLength; t++)
    {
      ContentInfo *info = mArray[t];
      info->print(stream,level+1,optionFlags);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




}
}
