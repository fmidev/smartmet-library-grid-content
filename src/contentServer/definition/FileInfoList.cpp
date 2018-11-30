#include "FileInfoList.h"

#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/common/AutoThreadLock.h>
#include <grid-files/common/ShowFunction.h>

#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace T
{


ThreadLock FileInfoList_sortLock;

uint fileInfo_comparisonMethod = FileInfo::ComparisonMethod::fileId;



int fileInfo_compare(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    FileInfoPtr *obj1 = const_cast<FileInfoPtr*>(reinterpret_cast<const FileInfoPtr *>(_val1));
    FileInfoPtr *obj2 = const_cast<FileInfoPtr*>(reinterpret_cast<const FileInfoPtr *>(_val2));

    return (*obj1)->compare(fileInfo_comparisonMethod,(*obj2));
  }
  return 0;
}




FileInfoList::FileInfoList()
{
  FUNCTION_TRACE
  try
  {
    mModificationLockPtr = &mModificationLock;
    mComparisonMethod = FileInfo::ComparisonMethod::none;
    mReleaseObjects = true;
    mSize = 100;
    mLength = 0;
    mArray = new FileInfoPtr[100];

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





FileInfoList::FileInfoList(FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    mModificationLockPtr = &mModificationLock;
    mReleaseObjects = true;

    if (fileInfoList.getModificationLockPtr() != mModificationLockPtr)
      fileInfoList.lock();

    mSize = fileInfoList.mSize;
    mLength = fileInfoList.mLength;
    mArray = new FileInfoPtr[mSize];
    for (uint t=0; t<mSize; t++)
    {
      FileInfo *info = fileInfoList.mArray[t];
      if (info != nullptr)
        mArray[t] = info->duplicate();
      else
        mArray[t] = nullptr;
    }
    mComparisonMethod = fileInfoList.mComparisonMethod;

    if (fileInfoList.getModificationLockPtr() != mModificationLockPtr)
      fileInfoList.unlock();
  }
  catch (...)
  {
    fileInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





FileInfoList::~FileInfoList()
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    if (mArray != nullptr)
    {
      for (uint t=0; t<mLength; t++)
      {
        if (mArray[t] != nullptr  &&  mReleaseObjects)
        {
          delete(mArray[t]);
          mArray[t] = nullptr;
        }
      }
      delete[] mArray;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





FileInfoList& FileInfoList::operator=(FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (&fileInfoList == this)
      return *this;

    clear();

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (fileInfoList.getModificationLockPtr() != mModificationLockPtr)
      fileInfoList.lock();

    mSize = fileInfoList.mSize;
    mLength = fileInfoList.mLength;
    mArray = new FileInfoPtr[mSize];
    for (uint t=0; t<mSize; t++)
    {
      FileInfo *info = fileInfoList.mArray[t];
      if (info != nullptr  &&  mReleaseObjects)
        mArray[t] = info->duplicate();
      else
        mArray[t] = info;
    }
    mComparisonMethod = fileInfoList.mComparisonMethod;

    if (fileInfoList.getModificationLockPtr() != mModificationLockPtr)
      fileInfoList.unlock();

    return *this;
  }
  catch (...)
  {
    fileInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfoList::addFileInfo(FileInfo *fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr  ||  mLength == mSize)
    {
      increaseSize(mSize + mSize/5 + 10);
    }

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mComparisonMethod == FileInfo::ComparisonMethod::none)
    {
      mArray[mLength] = fileInfo;
      mLength++;
      return;
    }

    int idx = getClosestIndexNoLock(mComparisonMethod,*fileInfo);

    while (idx < C_INT(mLength)  &&  mArray[idx] != nullptr  &&   mArray[idx]->compare(mComparisonMethod,fileInfo) < 0)
    {
      idx++;
    }

    if (idx == C_INT(mLength))
    {
      mArray[mLength] = fileInfo;
      mLength++;
      return;
    }

    if (idx < C_INT(mLength))
      memmove(&mArray[idx+1],&mArray[idx],sizeof(void*)*(mLength-idx));

    mArray[idx] = fileInfo;
    mLength++;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfoList::addFileInfoList(FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    addFileInfoListNoLock(fileInfoList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfoList::addFileInfoListNoLock(FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    uint len1 = mLength;
    uint len2 = fileInfoList.getLength();

    if (mComparisonMethod == FileInfo::ComparisonMethod::none)
    {
      if (mArray == nullptr  ||  mLength == mSize  ||  (mLength + len2) > mSize)
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
      FileInfo *fInfo1 = nullptr;
      FileInfo *fInfo2 = nullptr;

      if (a < len1)
        fInfo1 = mArray[a];

      if (b < len2)
        fInfo2 = fileInfoList.getFileInfoByIndex(b);

      if (fInfo1 != nullptr  &&  fInfo2 == nullptr)
      {
        newArray[t] = fInfo1;
        a++;
      }
      else
      if (fInfo1 == nullptr  &&  fInfo2 != nullptr)
      {
        if (mReleaseObjects)
          newArray[t] = fInfo2->duplicate();
        else
          newArray[t] = fInfo2;
        b++;
      }
      else
      if (fInfo1 != nullptr  &&  fInfo2 != nullptr)
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

    delete[] mArray;
    mArray = newArray;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void FileInfoList::clear()
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    if (mArray != nullptr)
    {
      for (uint t=0; t<mLength; t++)
      {
        if (mArray[t] != nullptr  &&  mReleaseObjects)
        {
          delete(mArray[t]);
          mArray[t] = nullptr;
        }
      }
      delete[] mArray;
    }

    mSize = 100;
    mLength = 0;
    mArray = new FileInfoPtr[100];

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




void FileInfoList::increaseSize(uint newSize)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    if (mArray == nullptr)
    {
      mSize = newSize;
      mLength = 0;
      mArray = new FileInfoPtr[mSize];

      for (uint t=0; t<mSize; t++)
      {
        mArray[t] = nullptr;
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





FileInfo* FileInfoList::getFileInfoById(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    FileInfo search;
    search.mFileId = fileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx < 0  ||  C_UINT(idx) >= getLength())
      return nullptr;

    FileInfo *info = getFileInfoByIndexNoCheck(idx);
    if (info != nullptr  &&  info->mFileId == fileId  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
      return info;

    return nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





FileInfo* FileInfoList::getFileInfoByIdNoLock(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    FileInfo search;
    search.mFileId = fileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx < 0  ||  C_UINT(idx) >= getLength())
      return nullptr;

    FileInfo *info = getFileInfoByIndexNoCheck(idx);
    if (info != nullptr  &&  info->mFileId == fileId  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
      return info;

    return nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfoList::markFileInfoDeletedById(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    FileInfo search;
    search.mFileId = fileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx < 0  ||  C_UINT(idx) >= getLength())
      return;

    FileInfo *info = getFileInfoByIndexNoCheck(idx);
    if (info != nullptr  &&  info->mFileId == fileId)
    {
      info->mFlags = info->mFlags | T::FileInfo::Flags::DeletedFile;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int FileInfoList::getClosestIndex(uint comparisonMethod,FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    return getClosestIndexNoLock(comparisonMethod,fileInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int FileInfoList::getClosestIndexNoLock(uint comparisonMethod,FileInfo& fileInfo)
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
    int high = C_INT(mLength) - 1;
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

    if (mid >= 0  &&  mid < C_INT(mLength))
    {
      if (mArray[mid]->compare(comparisonMethod,&fileInfo) < 0)
      {
        while (mid < C_INT(mLength)  &&  mArray[mid] != nullptr  &&   mArray[mid]->compare(comparisonMethod,&fileInfo) < 0)
          mid++;

        return mid-1;
      }
      else
      {
        while (mid > 0  &&  mArray[mid] != nullptr  &&   mArray[mid]->compare(comparisonMethod,&fileInfo) > 0)
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





FileInfo* FileInfoList::getFileInfoByName(std::string filename)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    FileInfo search;
    search.mName = filename;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileName,search);
    if (idx < 0  ||  C_UINT(idx) >= getLength())
      return nullptr;

    FileInfo *info = getFileInfoByIndexNoCheck(idx);
    if (info != nullptr  &&  info->mName == filename  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
      return info;

    return nullptr;
    /*
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = getFileInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mName == filename)
        return info;
    }
    return nullptr;
    */
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





FileInfo* FileInfoList::getFileInfoByIndex(uint index)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    if (index > mLength)
      return nullptr;

    if (mArray[index] == nullptr || (mArray[index]->mFlags & T::FileInfo::Flags::DeletedFile) != 0)
      return nullptr;

    return mArray[index];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void FileInfoList::getFileInfoList(uint startFileId,uint maxRecords,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      std::cout << CODE_LOCATION << " : Not supported when the records are not ordered by the 'fileId' field!\n";
      return;
    }

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint startIdx = 0;
    uint sz = getLength();

    FileInfo search;
    search.mFileId = startFileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx >= 0)
      startIdx = C_UINT(idx);

    for (uint t=startIdx; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId >= startFileId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfoList::getFileInfoListByProducerId(uint producerId,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint sz = getLength();

    for (uint t=0; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mProducerId == producerId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfoList::getFileInfoListByProducerId(uint producerId,uint startFileId,uint maxRecords,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      std::cout << CODE_LOCATION << " : Not supported when the records are not ordered by the 'fileId' field!\n";
      return;
    }

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint startIdx = 0;
    uint sz = getLength();

    FileInfo search;
    search.mFileId = startFileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx >= 0)
      startIdx = C_UINT(idx);

    for (uint t=startIdx; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId >= startFileId  &&  info->mProducerId == producerId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfoList::getFileInfoListByGenerationId(uint generationId,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint sz = getLength();

    for (uint t=0; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mGenerationId == generationId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfoList::getFileInfoListByGenerationId(uint generationId,uint startFileId,uint maxRecords,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      std::cout << CODE_LOCATION << " : Not supported when the records are not ordered by the 'fileId' field!\n";
      return;
    }

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint startIdx = 0;
    uint sz = getLength();

    FileInfo search;
    search.mFileId = startFileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx >= 0)
      startIdx = C_UINT(idx);

    for (uint t=startIdx; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId >= startFileId  &&  info->mGenerationId == generationId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfoList::getFileInfoListByGroupFlags(uint groupFlags,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint sz = getLength();

    for (uint t=0; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  (info->mGroupFlags & groupFlags) != 0)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfoList::getFileInfoListByGroupFlags(uint groupFlags,uint startFileId,uint maxRecords,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      std::cout << CODE_LOCATION << " : Not supported when the records are not ordered by the 'fileId' field!\n";
      return;
    }

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint startIdx = 0;
    uint sz = getLength();

    FileInfo search;
    search.mFileId = startFileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx >= 0)
      startIdx = C_UINT(idx);

    for (uint t=startIdx; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId >= startFileId  &&  (info->mGroupFlags & groupFlags) != 0)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfoList::getFileInfoListBySourceId(uint sourceId,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint sz = getLength();

    for (uint t=0; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mSourceId == sourceId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfoList::getFileInfoListBySourceId(uint sourceId,uint startFileId,uint maxRecords,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      std::cout << CODE_LOCATION << " : Not supported when the records are not ordered by the 'fileId' field!\n";
      return;
    }

    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint startIdx = 0;
    uint sz = getLength();

    FileInfo search;
    search.mFileId = startFileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx >= 0)
      startIdx = C_UINT(idx);

    for (uint t=startIdx; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId >= startFileId  &&  info->mSourceId == sourceId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint FileInfoList::getFileInfoCountByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
      {
        if (info->mProducerId == producerId)
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





uint FileInfoList::getFileInfoCountByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
      {
        if (info->mGenerationId == generationId)
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





uint FileInfoList::getFileInfoCountBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint count = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
      {
        if (info->mSourceId == sourceId)
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





FileInfo* FileInfoList::getFileInfoByIndexNoCheck(uint index)
{
  FUNCTION_TRACE
  try
  {
    return mArray[index];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




uint FileInfoList::getLength()
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





bool FileInfoList::deleteFileInfoById(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    FileInfo search;
    search.mFileId = fileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx < 0 ||  idx >= C_INT(mLength))
      return false;

    FileInfo *info = mArray[idx];
    if (info->mFileId != fileId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





bool FileInfoList::deleteFileInfoByName(std::string filename)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    FileInfo search;
    search.mName = filename;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileName,search);
    if (idx < 0 ||  idx >= C_INT(mLength))
      return false;

    FileInfo *info = mArray[idx];
    if (info == nullptr || info->mName != filename)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




uint FileInfoList::deleteFileInfoByGroupFlags(uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




uint FileInfoList::deleteFileInfoByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
      {
        if (info->mProducerId == producerId || (info->mFlags & T::FileInfo::Flags::DeletedFile) != 0)
        {
          mArray[t] = nullptr;
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint FileInfoList::deleteMarkedFiles()
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
      {
        if (info->mFlags & T::FileInfo::Flags::DeletedFile)
        {
          mArray[t] = nullptr;
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint FileInfoList::deleteFileInfoByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint FileInfoList::deleteFileInfoByGenerationIdList(std::set<uint>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





bool FileInfoList::deleteFileInfoByIndex(uint index)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (index >= mLength)
      return false;

    FileInfo *info = mArray[index];

    for (uint t=index+1; t<mSize; t++)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint FileInfoList::deleteFileInfoBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
      {
        if (info->mSourceId == sourceId || (info->mFlags & T::FileInfo::Flags::DeletedFile) != 0)
        {
          mArray[t] = nullptr;
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint FileInfoList::deleteFileInfoByFileIdList(std::set<uint>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
      {
        if (fileIdList.find(info->mFileId) != fileIdList.end() || (info->mFlags & T::FileInfo::Flags::DeletedFile) != 0)
        {
          mArray[t] = nullptr;
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint FileInfoList::deleteVirtualFiles()
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
      {
        if (info->mFileType == T::FileTypeValue::Virtual || (info->mFlags & T::FileInfo::Flags::VirtualContent) != 0  || (info->mFlags & T::FileInfo::Flags::DeletedFile) != 0)
        {
          mArray[t] = nullptr;
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfoList::setModificationLockPtr(ModificationLock* modificationLockPtr)
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





ModificationLock* FileInfoList::getModificationLockPtr()
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





void FileInfoList::lock()
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





void FileInfoList::unlock()
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





bool FileInfoList::getReleaseObjects()
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




void FileInfoList::setReleaseObjects(bool releaseObjects)
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





void FileInfoList::setComparisonMethod(uint comparisonMethod)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    mComparisonMethod = comparisonMethod;
    if (mLength == 0)
      return;

    fileInfo_comparisonMethod = comparisonMethod;

    qsort(mArray,mLength,sizeof(FileInfoPtr),fileInfo_compare);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfoList::sort(uint comparisonMethod)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr)
      return;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    mComparisonMethod = comparisonMethod;

    AutoThreadLock globalLock(&FileInfoList_sortLock);

    fileInfo_comparisonMethod = comparisonMethod;

    qsort(mArray,mLength,sizeof(FileInfoPtr),fileInfo_compare);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfoList::writeToFile(std::string filename)
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





void FileInfoList::writeToFile(std::string filename,const char *filemode)
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
      fprintf(file,"%s\n",mArray[t]->getCsv().c_str());
    }

    fclose(file);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void FileInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    stream << space(level) << "FileInfoList\n";
    for (uint t=0; t<mLength; t++)
      mArray[t]->print(stream,level+1,optionFlags);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




}
}
