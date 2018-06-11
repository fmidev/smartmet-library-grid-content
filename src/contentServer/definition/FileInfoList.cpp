#include "FileInfoList.h"

#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/common/AutoThreadLock.h>


namespace SmartMet
{
namespace T
{


ThreadLock FileInfoList_sortLock;

FileInfo::ComparisonMethod fileInfo_comparisonMethod = FileInfo::ComparisonMethod::fileId;



int fileInfo_compare(const void *_val1,const void *_val2)
{
  if (_val1 != NULL  &&  _val2 != NULL)
  {
    FileInfoPtr *obj1 = const_cast<FileInfoPtr*>(reinterpret_cast<const FileInfoPtr *>(_val1));
    FileInfoPtr *obj2 = const_cast<FileInfoPtr*>(reinterpret_cast<const FileInfoPtr *>(_val2));

    return (*obj1)->compare(fileInfo_comparisonMethod,(*obj2));
  }
  return 0;
}




FileInfoList::FileInfoList()
{
  try
  {
    mComparisonMethod = FileInfo::ComparisonMethod::none;
    mReleaseObjects = true;
    mSize = 100;
    mLength = 0;
    mArray = new FileInfoPtr[100];

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





FileInfoList::FileInfoList(FileInfoList& fileInfoList)
{
  try
  {
    mReleaseObjects = true;
    fileInfoList.lock();
    mSize = fileInfoList.mSize;
    mLength = fileInfoList.mLength;
    mArray = new FileInfoPtr[mSize];
    for (uint t=0; t<mSize; t++)
    {
      FileInfo *info = fileInfoList.mArray[t];
      if (info != NULL)
        mArray[t] = info->duplicate();
      else
        mArray[t] = NULL;
    }
    mComparisonMethod = fileInfoList.mComparisonMethod;
    fileInfoList.unlock();
  }
  catch (...)
  {
    fileInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





FileInfoList::~FileInfoList()
{
  try
  {
    clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::operator=(FileInfoList& fileInfoList)
{
  try
  {
    if (&fileInfoList == this)
      return;

    clear();

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    fileInfoList.lock();
    mSize = fileInfoList.mSize;
    mLength = fileInfoList.mLength;
    mArray = new FileInfoPtr[mSize];
    for (uint t=0; t<mSize; t++)
    {
      FileInfo *info = fileInfoList.mArray[t];
      if (info != NULL  &&  mReleaseObjects)
        mArray[t] = info->duplicate();
      else
        mArray[t] = info;
    }
    mComparisonMethod = fileInfoList.mComparisonMethod;
    fileInfoList.unlock();
  }
  catch (...)
  {
    fileInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::addFileInfo(FileInfo *fileInfo)
{
  try
  {
    if (mArray == NULL  ||  mLength == mSize)
    {
      increaseSize(mSize + mSize/5 + 10);
    }

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    if (mComparisonMethod == FileInfo::ComparisonMethod::none)
    {
      mArray[mLength] = fileInfo;
      mLength++;
      return;
    }

    int idx = getClosestIndexNoLock(mComparisonMethod,*fileInfo);

    while (idx < (int)mLength  &&  mArray[idx] != NULL  &&   mArray[idx]->compare(mComparisonMethod,fileInfo) < 0)
    {
      idx++;
    }

    if (idx == (int)mLength)
    {
      mArray[mLength] = fileInfo;
      mLength++;
      return;
    }

    if (idx < (int)mLength)
      memmove(&mArray[idx+1],&mArray[idx],sizeof(void*)*(mLength-idx));

    mArray[idx] = fileInfo;
    mLength++;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::addFileInfoList(FileInfoList& fileInfoList)
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    uint len1 = mLength;
    uint len2 = fileInfoList.getLength();

    if (mComparisonMethod == FileInfo::ComparisonMethod::none)
    {
      if (mArray == NULL  ||  mLength == mSize  ||  (mLength + len2) > mSize)
      {
        increaseSize(len1 + len2);
      }

      for (uint t=0; t<len2; t++)
      {
        FileInfo *fInfo = fileInfoList.getFileInfoByIndex(t);
        FileInfo *fileInfo = fInfo;

        if (mReleaseObjects)
          fileInfo = fInfo->duplicate();

        mArray[mLength] = fileInfo;
        mLength++;
      }
      return;
    }

    uint newSize = len1 + len2;
    FileInfoPtr *newArray = new FileInfoPtr[newSize + 100];
    uint a = 0;
    uint b = 0;

    fileInfoList.sort(mComparisonMethod);

    for (uint t=0; t<newSize; t++)
    {
      FileInfo *fInfo1 = NULL;
      FileInfo *fInfo2 = NULL;

      if (a < len1)
        fInfo1 = mArray[a];

      if (b < len2)
        fInfo2 = fileInfoList.getFileInfoByIndex(b);

      if (fInfo1 != NULL  &&  fInfo2 == NULL)
      {
        newArray[t] = fInfo1;
        a++;
      }
      else
      if (fInfo1 == NULL  &&  fInfo2 != NULL)
      {
        if (mReleaseObjects)
          newArray[t] = fInfo2->duplicate();
        else
          newArray[t] = fInfo2;
        b++;
      }
      else
      if (fInfo1 != NULL  &&  fInfo2 != NULL)
      {
        if (fInfo1->compare(mComparisonMethod,fInfo2) <= 0)
        {
          newArray[t] = fInfo1;
          a++;
        }
        else
        {
          if (mReleaseObjects)
            newArray[t] = fInfo2->duplicate();
          else
            newArray[t] = fInfo2;

          b++;
        }
      }
    }

    mSize = newSize+100;
    mLength = newSize;

    delete mArray;
    mArray = newArray;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void FileInfoList::clear()
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    if (mArray != NULL)
    {
      for (uint t=0; t<mLength; t++)
      {
        if (mArray[t] != NULL  &&  mReleaseObjects)
        {
          delete(mArray[t]);
          mArray[t] = NULL;
        }
      }
      delete mArray;
    }

    mSize = 100;
    mLength = 0;
    mArray = new FileInfoPtr[100];

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




void FileInfoList::increaseSize(uint newSize)
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    if (mArray == NULL)
    {
      mSize = newSize;
      mLength = 0;
      mArray = new FileInfoPtr[mSize];

      for (uint t=0; t<mSize; t++)
      {
        mArray[t] = NULL;
      }
      return;
    }

    FileInfoPtr *newArray = new FileInfoPtr[newSize];
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





FileInfo* FileInfoList::getFileInfoById(uint fileId)
{
  try
  {
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    FileInfo search;
    search.mFileId = fileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx < 0  ||  idx >= (int)getLength())
      return NULL;

    FileInfo *info = getFileInfoByIndexNoCheck(idx);
    if (info != NULL  &&  info->mFileId == fileId  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
      return info;

    return NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





FileInfo* FileInfoList::getFileInfoByIdNoLock(uint fileId)
{
  try
  {
    FileInfo search;
    search.mFileId = fileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx < 0  ||  idx >= (int)getLength())
      return NULL;

    FileInfo *info = getFileInfoByIndexNoCheck(idx);
    if (info != NULL  &&  info->mFileId == fileId  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
      return info;

    return NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::markFileInfoDeletedById(uint fileId)
{
  try
  {
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    FileInfo search;
    search.mFileId = fileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx < 0  ||  idx >= (int)getLength())
      return;

    FileInfo *info = getFileInfoByIndexNoCheck(idx);
    if (info != NULL  &&  info->mFileId == fileId)
    {
      info->mFlags = info->mFlags | T::FileInfo::Flags::DeletedFile;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int FileInfoList::getClosestIndex(FileInfo::ComparisonMethod comparisonMethod,FileInfo& fileInfo)
{
  try
  {
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);
    return getClosestIndexNoLock(comparisonMethod,fileInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int FileInfoList::getClosestIndexNoLock(FileInfo::ComparisonMethod comparisonMethod,FileInfo& fileInfo)
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
          int res = mArray[t]->compare(comparisonMethod,&fileInfo);
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
      int res = mArray[mid]->compare(comparisonMethod,&fileInfo);

      if (res == 0)
      {
        while (mid > 0  &&  mArray[mid-1]->compare(comparisonMethod,&fileInfo) == 0)
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
      if (mArray[mid]->compare(comparisonMethod,&fileInfo) < 0)
      {
        while (mid < (int)mLength  &&  mArray[mid] != NULL  &&   mArray[mid]->compare(comparisonMethod,&fileInfo) < 0)
          mid++;

        return mid-1;
      }
      else
      {
        while (mid > 0  &&  mArray[mid] != NULL  &&   mArray[mid]->compare(comparisonMethod,&fileInfo) > 0)
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





FileInfo* FileInfoList::getFileInfoByName(std::string filename)
{
  try
  {
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    FileInfo search;
    search.mName = filename;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileName,search);
    if (idx < 0  ||  idx >= (int)getLength())
      return NULL;

    FileInfo *info = getFileInfoByIndexNoCheck(idx);
    if (info != NULL  &&  info->mName == filename  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
      return info;

    return NULL;
    /*
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = getFileInfoByIndexNoCheck(t);
      if (info != NULL  &&  info->mName == filename)
        return info;
    }
    return NULL;
    */
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





FileInfo* FileInfoList::getFileInfoByIndex(uint index)
{
  try
  {
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);
    if (index > mLength)
      return NULL;

    if (mArray[index] == NULL || (mArray[index]->mFlags & T::FileInfo::Flags::DeletedFile) != 0)
      return NULL;

    return mArray[index];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void FileInfoList::getFileInfoList(uint startFileId,uint maxRecords,FileInfoList& fileInfoList)
{
  try
  {
    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      std::cout << CODE_LOCATION << " : Not supported when the records are not ordered by the 'fileId' field!\n";
      return;
    }

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);
    uint startIdx = 0;
    uint sz = getLength();

    FileInfo search;
    search.mFileId = startFileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx >= 0)
      startIdx = (uint)idx;

    for (uint t=startIdx; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId >= startFileId)
      {
        if (fileInfoList.getReleaseObjects())
          fileInfoList.addFileInfo(info->duplicate());
        else
          fileInfoList.addFileInfo(info);

        if (fileInfoList.getLength() == maxRecords)
          return;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::getFileInfoListByProducerId(uint producerId,FileInfoList& fileInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);
    uint sz = getLength();

    for (uint t=0; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mProducerId == producerId)
      {
        if (fileInfoList.getReleaseObjects())
          fileInfoList.addFileInfo(info->duplicate());
        else
          fileInfoList.addFileInfo(info);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::getFileInfoListByProducerId(uint producerId,uint startFileId,uint maxRecords,FileInfoList& fileInfoList)
{
  try
  {
    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      std::cout << CODE_LOCATION << " : Not supported when the records are not ordered by the 'fileId' field!\n";
      return;
    }

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);
    uint startIdx = 0;
    uint sz = getLength();

    FileInfo search;
    search.mFileId = startFileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx >= 0)
      startIdx = (uint)idx;

    for (uint t=startIdx; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId >= startFileId  &&  info->mProducerId == producerId)
      {
        if (fileInfoList.getReleaseObjects())
          fileInfoList.addFileInfo(info->duplicate());
        else
          fileInfoList.addFileInfo(info);

        if (fileInfoList.getLength() == maxRecords)
          return;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::getFileInfoListByGenerationId(uint generationId,FileInfoList& fileInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);
    uint sz = getLength();

    for (uint t=0; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mGenerationId == generationId)
      {
        if (fileInfoList.getReleaseObjects())
          fileInfoList.addFileInfo(info->duplicate());
        else
          fileInfoList.addFileInfo(info);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::getFileInfoListByGenerationId(uint generationId,uint startFileId,uint maxRecords,FileInfoList& fileInfoList)
{
  try
  {
    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      std::cout << CODE_LOCATION << " : Not supported when the records are not ordered by the 'fileId' field!\n";
      return;
    }

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);
    uint startIdx = 0;
    uint sz = getLength();

    FileInfo search;
    search.mFileId = startFileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx >= 0)
      startIdx = (uint)idx;

    for (uint t=startIdx; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId >= startFileId  &&  info->mGenerationId == generationId)
      {
        if (fileInfoList.getReleaseObjects())
          fileInfoList.addFileInfo(info->duplicate());
        else
          fileInfoList.addFileInfo(info);

        if (fileInfoList.getLength() == maxRecords)
          return;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::getFileInfoListByGroupFlags(uint groupFlags,FileInfoList& fileInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);
    uint sz = getLength();

    for (uint t=0; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  (info->mGroupFlags & groupFlags) != 0)
      {
        if (fileInfoList.getReleaseObjects())
          fileInfoList.addFileInfo(info->duplicate());
        else
          fileInfoList.addFileInfo(info);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::getFileInfoListByGroupFlags(uint groupFlags,uint startFileId,uint maxRecords,FileInfoList& fileInfoList)
{
  try
  {
    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      std::cout << CODE_LOCATION << " : Not supported when the records are not ordered by the 'fileId' field!\n";
      return;
    }

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);
    uint startIdx = 0;
    uint sz = getLength();

    FileInfo search;
    search.mFileId = startFileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx >= 0)
      startIdx = (uint)idx;

    for (uint t=startIdx; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId >= startFileId  &&  (info->mGroupFlags & groupFlags) != 0)
      {
        if (fileInfoList.getReleaseObjects())
          fileInfoList.addFileInfo(info->duplicate());
        else
          fileInfoList.addFileInfo(info);

        if (fileInfoList.getLength() == maxRecords)
          return;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::getFileInfoListBySourceId(uint sourceId,FileInfoList& fileInfoList)
{
  try
  {
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);
    uint sz = getLength();

    for (uint t=0; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mSourceId == sourceId)
      {
        if (fileInfoList.getReleaseObjects())
          fileInfoList.addFileInfo(info->duplicate());
        else
          fileInfoList.addFileInfo(info);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::getFileInfoListBySourceId(uint sourceId,uint startFileId,uint maxRecords,FileInfoList& fileInfoList)
{
  try
  {
    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      std::cout << CODE_LOCATION << " : Not supported when the records are not ordered by the 'fileId' field!\n";
      return;
    }

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);
    uint startIdx = 0;
    uint sz = getLength();

    FileInfo search;
    search.mFileId = startFileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx >= 0)
      startIdx = (uint)idx;

    for (uint t=startIdx; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId >= startFileId  &&  info->mSourceId == sourceId)
      {
        if (fileInfoList.getReleaseObjects())
          fileInfoList.addFileInfo(info->duplicate());
        else
          fileInfoList.addFileInfo(info);

        if (fileInfoList.getLength() == maxRecords)
          return;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint FileInfoList::getFileInfoCountByProducerId(uint producerId)
{
  try
  {
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
      {
        if (info->mProducerId == producerId)
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





uint FileInfoList::getFileInfoCountByGenerationId(uint generationId)
{
  try
  {
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
      {
        if (info->mGenerationId == generationId)
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





uint FileInfoList::getFileInfoCountBySourceId(uint sourceId)
{
  try
  {
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      if (info != NULL  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
      {
        if (info->mSourceId == sourceId)
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





FileInfo* FileInfoList::getFileInfoByIndexNoCheck(uint index)
{
  try
  {
    return mArray[index];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




uint FileInfoList::getLength()
{
  try
  {
    return mLength;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool FileInfoList::deleteFileInfoById(uint fileId)
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    FileInfo search;
    search.mFileId = fileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx < 0 ||  idx >= (int)mLength)
      return false;

    FileInfo *info = mArray[idx];
    if (info->mFileId != fileId)
      return false;

    for (uint t=(uint)idx+1; t<mLength; t++)
    {
      mArray[t-1] = mArray[t];
      mArray[t] = NULL;
    }

    if (mReleaseObjects)
      delete info;

    mLength--;
    return true;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool FileInfoList::deleteFileInfoByName(std::string filename)
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    FileInfo search;
    search.mName = filename;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileName,search);
    if (idx < 0 ||  idx >= (int)mLength)
      return false;

    FileInfo *info = mArray[idx];
    if (info->mName != filename)
      return false;

    for (uint t=(uint)idx+1; t<mLength; t++)
    {
      mArray[t-1] = mArray[t];
      mArray[t] = NULL;
    }

    if (mReleaseObjects)
      delete info;

    mLength--;
    return true;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




uint FileInfoList::deleteFileInfoByGroupFlags(uint groupFlags)
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL)
      {
        if ((info->mGroupFlags & groupFlags) != 0 || (info->mFlags & T::FileInfo::Flags::DeletedFile) != 0)
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
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




uint FileInfoList::deleteFileInfoByProducerId(uint producerId)
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL)
      {
        if (info->mProducerId == producerId || (info->mFlags & T::FileInfo::Flags::DeletedFile) != 0)
        {
          mArray[t] = NULL;
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
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint FileInfoList::deleteMarkedFiles()
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL)
      {
        if (info->mFlags & T::FileInfo::Flags::DeletedFile)
        {
          mArray[t] = NULL;
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
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint FileInfoList::deleteFileInfoByGenerationId(uint generationId)
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL)
      {
        if (info->mGenerationId == generationId || (info->mFlags & T::FileInfo::Flags::DeletedFile) != 0)
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
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint FileInfoList::deleteFileInfoByGenerationIdList(std::set<uint>& generationIdList)
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL)
      {
        if (generationIdList.find(info->mGenerationId) != generationIdList.end() || (info->mFlags & T::FileInfo::Flags::DeletedFile) != 0)
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
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool FileInfoList::deleteFileInfoByIndex(uint index)
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    if (index >= mLength)
      return false;

    FileInfo *info = mArray[index];

    for (uint t=(uint)index+1; t<mSize; t++)
    {
      mArray[t-1] = mArray[t];
      mArray[t] = NULL;
    }

    if (mReleaseObjects)
      delete info;

    mLength--;
    return true;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint FileInfoList::deleteFileInfoBySourceId(uint sourceId)
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL)
      {
        if (info->mSourceId == sourceId || (info->mFlags & T::FileInfo::Flags::DeletedFile) != 0)
        {
          mArray[t] = NULL;
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
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint FileInfoList::deleteFileInfoByFileIdList(std::set<uint>& fileIdList)
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL)
      {
        if (fileIdList.find(info->mFileId) != fileIdList.end() || (info->mFlags & T::FileInfo::Flags::DeletedFile) != 0)
        {
          mArray[t] = NULL;
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
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





uint FileInfoList::deleteVirtualFiles()
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL)
      {
        if (info->mFileType == T::FileType::Virtual || (info->mFlags & T::FileInfo::Flags::VirtualContent) != 0  || (info->mFlags & T::FileInfo::Flags::DeletedFile) != 0)
        {
          mArray[t] = NULL;
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
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ModificationLock* FileInfoList::getModificationLockPtr()
{
  try
  {
    return &mModificationLock;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::lock()
{
  try
  {
    return mModificationLock.lock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::unlock()
{
  try
  {
    return mModificationLock.unlock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool FileInfoList::getReleaseObjects()
{
  try
  {
    return mReleaseObjects;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void FileInfoList::setReleaseObjects(bool releaseObjects)
{
  try
  {
    mReleaseObjects = releaseObjects;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::setComparisonMethod(FileInfo::ComparisonMethod comparisonMethod)
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    mComparisonMethod = comparisonMethod;
    if (mLength == 0)
      return;

    fileInfo_comparisonMethod = comparisonMethod;

    qsort(mArray,mLength,sizeof(FileInfoPtr),fileInfo_compare);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::sort(FileInfo::ComparisonMethod comparisonMethod)
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    mComparisonMethod = comparisonMethod;

    AutoThreadLock globalLock(&FileInfoList_sortLock);

    fileInfo_comparisonMethod = comparisonMethod;

    qsort(mArray,mLength,sizeof(FileInfoPtr),fileInfo_compare);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::writeToFile(std::string filename)
{
  try
  {
    writeToFile(filename,"w");
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void FileInfoList::writeToFile(std::string filename,const char *filemode)
{
  try
  {
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    FILE *file = fopen(filename.c_str(),filemode);
    if (file == NULL)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot create the file!");
      exception.addParameter("Filename",filename);
      throw exception;
    }

    for (uint t=0; t<mLength; t++)
    {
      fprintf(file,"%s\n",mArray[t]->getCsv().c_str());
    }

    fclose(file);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void FileInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    stream << space(level) << "FileInfoList\n";
    for (uint t=0; t<mLength; t++)
      mArray[t]->print(stream,level+1,optionFlags);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




}
}
