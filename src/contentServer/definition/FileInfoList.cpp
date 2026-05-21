#include "FileInfoList.h"

#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/common/AutoThreadLock.h>
#include <grid-files/common/ShowFunction.h>
#include <iostream>
#include <macgyver/Hash.h>

#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace T
{



/*! \brief Qsort comparator for FileInfo using comparison method 1. */

int fileInfo_compare_1(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    FileInfoPtr *obj1 = const_cast<FileInfoPtr*>(reinterpret_cast<const FileInfoPtr *>(_val1));
    FileInfoPtr *obj2 = const_cast<FileInfoPtr*>(reinterpret_cast<const FileInfoPtr *>(_val2));

    return (*obj1)->compare(1,(*obj2));
  }
  return 0;
}




/*! \brief Qsort comparator for FileInfo using comparison method 2. */

int fileInfo_compare_2(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    FileInfoPtr *obj1 = const_cast<FileInfoPtr*>(reinterpret_cast<const FileInfoPtr *>(_val1));
    FileInfoPtr *obj2 = const_cast<FileInfoPtr*>(reinterpret_cast<const FileInfoPtr *>(_val2));

    return (*obj1)->compare(2,(*obj2));
  }
  return 0;
}




/*! \brief Default constructor for FileInfoList. */

FileInfoList::FileInfoList()
{
  FUNCTION_TRACE
  try
  {
    mModificationLockPtr = &mModificationLock;
    mComparisonMethod = FileInfo::ComparisonMethod::none;
    mReleaseObjects = true;
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





/*! \brief Copy constructor for FileInfoList. */

FileInfoList::FileInfoList(FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    mModificationLockPtr = &mModificationLock;
    mReleaseObjects = true;
    mModificationLock.setLockingEnabled(false);

    if (fileInfoList.getModificationLockPtr() != mModificationLockPtr)
      fileInfoList.lock();

    mSize = fileInfoList.mSize;
    mLength = fileInfoList.mLength;
    mArray = new FileInfoPtr[mSize];
    uint p = 0;
    for (uint t=0; t<mSize; t++)
    {
      mArray[t] = nullptr;
      FileInfo *info = fileInfoList.mArray[t];
      if (info != nullptr)
      {
        mArray[p] = info->duplicate();
        p++;
      }
    }
    mComparisonMethod = fileInfoList.mComparisonMethod;
    mLength = p;

    if (fileInfoList.getModificationLockPtr() != mModificationLockPtr)
      fileInfoList.unlock();
  }
  catch (...)
  {
    fileInfoList.unlock();
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Destructor for FileInfoList. */

FileInfoList::~FileInfoList()
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr)
      return;

    AutoWriteLock lock(mModificationLockPtr);
    if (mReleaseObjects)
    {
      for (uint t=0; t<mLength; t++)
      {
        if (mArray[t] != nullptr)
        {
          delete(mArray[t]);
        }
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





/*! \brief Copy assignment operator for FileInfoList. */

FileInfoList& FileInfoList::operator=(FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (&fileInfoList == this)
      return *this;

    clear();

    AutoWriteLock lock(mModificationLockPtr);

    if (fileInfoList.getModificationLockPtr() != mModificationLockPtr)
      fileInfoList.lock();

    uint p = 0;
    mSize = fileInfoList.mSize;
    mLength = fileInfoList.mLength;
    mArray = new FileInfoPtr[mSize];
    for (uint t=0; t<mSize; t++)
    {
      mArray[t] = nullptr;
      FileInfo *info = fileInfoList.mArray[t];
      if (info != nullptr  && (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
      {
        if (mReleaseObjects)
          mArray[p] = info->duplicate();
        else
          mArray[p] = info;

        p++;
      }
    }
    mComparisonMethod = fileInfoList.mComparisonMethod;

    if (fileInfoList.getModificationLockPtr() != mModificationLockPtr)
      fileInfoList.unlock();

    mLength = p;
    return *this;
  }
  catch (...)
  {
    fileInfoList.unlock();
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Add a FileInfo record to the list. */

void FileInfoList::addFileInfo(FileInfo *fileInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr);

    if (mArray == nullptr  ||  mLength == mSize)
    {
      increaseSizeNoLock(mSize + mSize/5 + 50);
    }


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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Add all entries from another FileInfoList. */

void FileInfoList::addFileInfoList(FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr);
    addFileInfoListNoLock(fileInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Add all entries from another FileInfoList without acquiring the lock. */

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
        increaseSizeNoLock(len1 + len2);
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

    uint newSize = len1 + len2 + 100;
    FileInfoPtr *newArray = new FileInfoPtr[newSize];
    uint a = 0;
    uint b = 0;

    fileInfoList.sort(mComparisonMethod);

    uint c = 0;
    for (uint t=0; t<newSize; t++)
    {
      FileInfo *fInfo1 = nullptr;
      FileInfo *fInfo2 = nullptr;
      newArray[t] = nullptr;

      if (a < len1)
        fInfo1 = mArray[a];

      if (b < len2)
        fInfo2 = fileInfoList.getFileInfoByIndex(b);

      if (fInfo1 == nullptr  &&  fInfo2 == nullptr)
      {
        a++;
        b++;
      }
      else
      if (fInfo1 != nullptr  &&  fInfo2 == nullptr)
      {
        newArray[c] = fInfo1;
        a++;
        c++;
      }
      else
      if (fInfo1 == nullptr  &&  fInfo2 != nullptr)
      {
        if (mReleaseObjects)
          newArray[c] = fInfo2->duplicate();
        else
          newArray[c] = fInfo2;
        b++;
        c++;
      }
      else
      if (fInfo1 != nullptr  &&  fInfo2 != nullptr)
      {
        if (fInfo1->compare(mComparisonMethod,fInfo2) <= 0)
        {
          newArray[c] = fInfo1;
          a++;
          c++;
        }
        else
        {
          if (mReleaseObjects)
            newArray[c] = fInfo2->duplicate();
          else
            newArray[c] = fInfo2;

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




/*! \brief Remove all entries from the list. */

void FileInfoList::clear()
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
          delete(mArray[t]);
      }
    }
    delete[] mArray;
    mSize = 0;
    mLength = 0;
    mArray = nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Increase the capacity of the underlying storage. */

void FileInfoList::increaseSize(uint newSize)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr);
    increaseSizeNoLock(newSize);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Increase the capacity of the underlying storage without acquiring the lock. */

void FileInfoList::increaseSizeNoLock(uint newSize)
{
  FUNCTION_TRACE
  try
  {
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return the FileInfo matching the given file id. */

FileInfo* FileInfoList::getFileInfoById(T::FileId fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return nullptr;

    AutoReadLock lock(mModificationLockPtr);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Copy the FileInfo matching the given file id into the output structure. */

bool FileInfoList::getFileInfoById(T::FileId fileId,FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return false;

    AutoReadLock lock(mModificationLockPtr);

    FileInfo search;
    search.mFileId = fileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx < 0  ||  C_UINT(idx) >= getLength())
      return false;

    FileInfo *info = getFileInfoByIndexNoCheck(idx);
    if (info != nullptr  &&  info->mFileId == fileId  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
    {
      fileInfo = *info;
      return true;
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return the FileInfo matching the given file id without locking. */

FileInfo* FileInfoList::getFileInfoByIdNoLock(T::FileId fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return nullptr;

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Flag the entry matching the given file id as deleted. */

uint FileInfoList::markDeletedById(T::FileId fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr);

    FileInfo *info = getFileInfoByIdNoLock(fileId);
    if (info != nullptr  &&  info->mFileId == fileId)
    {
      info->mFlags |= T::FileInfo::Flags::DeletedFile;
      return 1;
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Flag every entry in the list as deleted. */

uint FileInfoList::markDeleted()
{
  FUNCTION_TRACE
  try
  {
   if (mArray == nullptr ||  mLength == 0)
      return 0;

    uint cnt = 0;
    AutoWriteLock lock(mModificationLockPtr);
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr)
      {
        info->mFlags |= T::FileInfo::Flags::DeletedFile;
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





/*! \brief Flag entries whose producer id matches the given value as deleted. */

uint FileInfoList::markDeletedByProducerId(T::ProducerId producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    uint cnt = 0;
    AutoWriteLock lock(mModificationLockPtr);
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mProducerId == producerId)
        {
          info->mFlags |= T::FileInfo::Flags::DeletedFile;
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





/*! \brief Flag entries whose generation id matches the given value as deleted. */

uint FileInfoList::markDeletedByGenerationId(T::GenerationId generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    uint cnt = 0;
    AutoWriteLock lock(mModificationLockPtr);
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mGenerationId == generationId)
        {
          info->mFlags |= T::FileInfo::Flags::DeletedFile;
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




/*! \brief Flag entries whose source id matches the given value as deleted. */

uint FileInfoList::markDeletedBySourceId(T::SourceId sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    uint cnt = 0;
    AutoWriteLock lock(mModificationLockPtr);
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mSourceId == sourceId)
        {
          info->mFlags |= T::FileInfo::Flags::DeletedFile;
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





/*! \brief Flag entries whose storage id matches the given value as deleted. */

uint FileInfoList::markDeletedByStorageId(T::StorageId storageId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    uint cnt = 0;
    AutoWriteLock lock(mModificationLockPtr);
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mStorageId == storageId)
        {
          info->mFlags |= T::FileInfo::Flags::DeletedFile;
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





/*! \brief Return the index of the entry closest to the given FileInfo. */

int FileInfoList::getClosestIndex(uint comparisonMethod,FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoReadLock lock(mModificationLockPtr);
    return getClosestIndexNoLock(comparisonMethod,fileInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return the index of the entry closest to the given FileInfo without locking. */

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return the FileInfo matching the given filename. */

FileInfo* FileInfoList::getFileInfoByName(const std::string& filename)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return nullptr;

    AutoReadLock lock(mModificationLockPtr);

    FileInfo search;
    search.mName = filename;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileName,search);
    if (idx < 0  ||  C_UINT(idx) >= getLength())
      return nullptr;

    FileInfo *info = getFileInfoByIndexNoCheck(idx);
    if (info != nullptr  &&  info->mName == filename  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
      return info;

    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Copy the FileInfo matching the given filename into the output structure. */

bool FileInfoList::getFileInfoByName(const std::string& filename,FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return false;

    AutoReadLock lock(mModificationLockPtr);

    FileInfo search;
    search.mName = filename;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileName,search);
    if (idx < 0  ||  C_UINT(idx) >= getLength())
      return false;

    FileInfo *info = getFileInfoByIndexNoCheck(idx);
    if (info != nullptr  &&  info->mName == filename  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
    {
      fileInfo = *info;
      return true;
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Return the FileInfo at the given list index. */

FileInfo* FileInfoList::getFileInfoByIndex(uint index)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return nullptr;

    AutoReadLock lock(mModificationLockPtr);
    if (index > mLength)
      return nullptr;

    return mArray[index];
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Copy the entire list into the given target list, replacing its contents. */

void FileInfoList::getFileInfoList(FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();
    getFileInfoListNoClear(fileInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Append the entire list into the given target list without clearing it. */

void FileInfoList::getFileInfoListNoClear(FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);

    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Copy a paginated slice of the list starting from the given file id. */

void FileInfoList::getFileInfoList(T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();
    getFileInfoListNoClear(startFileId,maxRecords,fileInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Append a paginated slice of the list starting from the given file id without clearing the target. */

void FileInfoList::getFileInfoListNoClear(T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return;

    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      std::cerr << __FILE__ << ":" << __LINE__ << ": Not supported when the records are not ordered by the 'fileId' field!\n";
      return;
    }

    AutoReadLock lock(mModificationLockPtr);

    uint max = (uint)abs(maxRecords);
    FileInfo search;
    search.mFileId = startFileId;
    int startIdx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);

    if (maxRecords >= 0)
    {
      if (startIdx < 0)
        startIdx = 0;

      for (uint t=startIdx; t<mLength; t++)
      {
        FileInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId >= startFileId)
        {
          if (fileInfoList.getReleaseObjects())
            fileInfoList.addFileInfo(info->duplicate());
          else
            fileInfoList.addFileInfo(info);

          if (fileInfoList.getLength() == max)
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
        FileInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId <= startFileId)
        {
          if (fileInfoList.getReleaseObjects())
            fileInfoList.addFileInfo(info->duplicate());
          else
            fileInfoList.addFileInfo(info);

          if (fileInfoList.getLength() == max)
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





/*! \brief Append all entries matching the given producer id to the target list. */

void FileInfoList::getFileInfoListByProducerId(T::ProducerId producerId,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Append a paginated slice of entries matching the given producer id. */

void FileInfoList::getFileInfoListByProducerId(T::ProducerId producerId,T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      std::cerr << __FILE__ << ":" << __LINE__ << ": Not supported when the records are not ordered by the 'fileId' field!\n";
      return;
    }

    AutoReadLock lock(mModificationLockPtr);

    uint max = (uint)abs(maxRecords);
    FileInfo search;
    search.mFileId = startFileId;
    int startIdx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);

    if (maxRecords >= 0)
    {
      if (startIdx < 0)
        startIdx = 0;

      for (uint t=startIdx; t<mLength; t++)
      {
        FileInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId >= startFileId  &&  info->mProducerId == producerId)
        {
          if (fileInfoList.getReleaseObjects())
            fileInfoList.addFileInfo(info->duplicate());
          else
            fileInfoList.addFileInfo(info);

          if (fileInfoList.getLength() == max)
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
        FileInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId <= startFileId  &&  info->mProducerId == producerId)
        {
          if (fileInfoList.getReleaseObjects())
            fileInfoList.addFileInfo(info->duplicate());
          else
            fileInfoList.addFileInfo(info);

          if (fileInfoList.getLength() == max)
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





/*! \brief Return a hash value computed from the entire list contents. */

std::size_t FileInfoList::getHash()
{
  FUNCTION_TRACE
  try
  {
    std::size_t hash = 0;

    if (mArray == nullptr ||  mLength == 0)
      return hash;

    AutoReadLock lock(mModificationLockPtr);
    uint sz = getLength();

    for (uint t=0; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0)
      {
        Fmi::hash_merge(hash,info->mFileId);
      }
    }
    return hash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return a hash value computed from entries matching the given producer id. */

std::size_t FileInfoList::getHashByProducerId(T::ProducerId producerId)
{
  FUNCTION_TRACE
  try
  {
    std::size_t hash = 0;

    if (mArray == nullptr ||  mLength == 0)
      return hash;

    AutoReadLock lock(mModificationLockPtr);
    uint sz = getLength();

    for (uint t=0; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mProducerId == producerId)
      {
        Fmi::hash_merge(hash,info->mFileId);
      }
    }
    return hash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return a hash value computed from entries matching the given generation id. */

std::size_t FileInfoList::getHashByGenerationId(T::GenerationId generationId)
{
  FUNCTION_TRACE
  try
  {
    std::size_t hash = 0;

    if (mArray == nullptr ||  mLength == 0)
      return hash;

    AutoReadLock lock(mModificationLockPtr);
    uint sz = getLength();

    for (uint t=0; t<sz; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mGenerationId == generationId)
      {
        Fmi::hash_merge(hash,info->mFileId);
      }
    }
    return hash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Append all entries matching the given generation id to the target list. */

void FileInfoList::getFileInfoListByGenerationId(T::GenerationId generationId,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Append a paginated slice of entries matching the given generation id. */

void FileInfoList::getFileInfoListByGenerationId(T::GenerationId generationId,T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      std::cerr << __FILE__ << ":" << __LINE__ << ": Not supported when the records are not ordered by the 'fileId' field!\n";
      return;
    }

    AutoReadLock lock(mModificationLockPtr);

    uint max = (uint)abs(maxRecords);
    FileInfo search;
    search.mFileId = startFileId;
    int startIdx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);

    if (maxRecords >= 0)
    {
      if (startIdx < 0)
        startIdx = 0;

      for (uint t=startIdx; t<mLength; t++)
      {
        FileInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId >= startFileId  &&  info->mGenerationId == generationId)
        {
          if (fileInfoList.getReleaseObjects())
            fileInfoList.addFileInfo(info->duplicate());
          else
            fileInfoList.addFileInfo(info);

          if (fileInfoList.getLength() == max)
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
        FileInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId <= startFileId  &&  info->mGenerationId == generationId)
        {
          if (fileInfoList.getReleaseObjects())
            fileInfoList.addFileInfo(info->duplicate());
          else
            fileInfoList.addFileInfo(info);

          if (fileInfoList.getLength() == max)
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





/*! \brief Append all entries matching the given source id to the target list. */

void FileInfoList::getFileInfoListBySourceId(T::SourceId sourceId,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    AutoReadLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Append a paginated slice of entries matching the given source id. */

void FileInfoList::getFileInfoListBySourceId(T::SourceId sourceId,T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    fileInfoList.clear();

    if (mArray == nullptr ||  mLength == 0)
      return;

    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      std::cerr << __FILE__ << ":" << __LINE__ << ": Not supported when the records are not ordered by the 'fileId' field!\n";
      return;
    }

    AutoReadLock lock(mModificationLockPtr);

    uint max = (uint)abs(maxRecords);
    FileInfo search;
    search.mFileId = startFileId;
    int startIdx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);

    if (maxRecords >= 0)
    {
      if (startIdx < 0)
        startIdx = 0;

      for (uint t=startIdx; t<mLength; t++)
      {
        FileInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId >= startFileId  &&  info->mSourceId == sourceId)
        {
          if (fileInfoList.getReleaseObjects())
            fileInfoList.addFileInfo(info->duplicate());
          else
            fileInfoList.addFileInfo(info);

          if (fileInfoList.getLength() == max)
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
        FileInfo *info = mArray[t];
        if (info != nullptr  &&  (info->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&  info->mFileId <= startFileId  &&  info->mSourceId == sourceId)
        {
          if (fileInfoList.getReleaseObjects())
            fileInfoList.addFileInfo(info->duplicate());
          else
            fileInfoList.addFileInfo(info);

          if (fileInfoList.getLength() == max)
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





/*! \brief Return the number of entries matching the given producer id. */

uint FileInfoList::getFileInfoCountByProducerId(T::ProducerId producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoReadLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return the number of entries matching the given generation id. */

uint FileInfoList::getFileInfoCountByGenerationId(T::GenerationId generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoReadLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return the number of entries matching the given source id. */

uint FileInfoList::getFileInfoCountBySourceId(T::SourceId sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoReadLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return the FileInfo at the given list index without bounds checking. */

FileInfo* FileInfoList::getFileInfoByIndexNoCheck(uint index)
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




/*! \brief Return the number of entries in the list. */

uint FileInfoList::getLength()
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





/*! \brief Delete the entry matching the given file id. */

bool FileInfoList::deleteFileInfoById(T::FileId fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return false;

    AutoWriteLock lock(mModificationLockPtr);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Delete the entry matching the given filename. */

bool FileInfoList::deleteFileInfoByName(const std::string& filename)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return false;

    AutoWriteLock lock(mModificationLockPtr);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Delete all entries whose producer id matches the given value. */

uint FileInfoList::deleteFileInfoByProducerId(T::ProducerId producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Remove all entries previously flagged for deletion. */

uint FileInfoList::deleteMarkedFiles()
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mSize; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
      {
        if (info->mFlags & T::FileInfo::Flags::DeletedFile)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Delete all entries whose generation id matches the given value. */

uint FileInfoList::deleteFileInfoByGenerationId(T::GenerationId generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Delete all entries whose generation id is in the given set. */

uint FileInfoList::deleteFileInfoByGenerationIdList(std::set<T::GenerationId>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Delete the entry at the given list index. */

bool FileInfoList::deleteFileInfoByIndex(uint index)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return false;

    AutoWriteLock lock(mModificationLockPtr);

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Delete all entries whose source id matches the given value. */

uint FileInfoList::deleteFileInfoBySourceId(T::SourceId sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Delete all entries whose storage id matches the given value. */

uint FileInfoList::deleteFileInfoByStorageId(T::StorageId storageId)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
      {
        if (info->mStorageId == storageId || (info->mFlags & T::FileInfo::Flags::DeletedFile) != 0)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Delete all entries whose file id is in the given set. */

uint FileInfoList::deleteFileInfoByFileIdList(std::set<T::FileId>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoWriteLock lock(mModificationLockPtr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Set the pointer to the modification mutex used by the list. */

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return a pointer to the internal modification mutex. */

ModificationLock* FileInfoList::getModificationLockPtr()
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





/*! \brief Acquire the internal mutex protecting the list. */

void FileInfoList::lock()
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





/*! \brief Release the internal mutex protecting the list. */

void FileInfoList::unlock()
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





/*! \brief Return whether contained objects are owned and released by the list. */

bool FileInfoList::getReleaseObjects()
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




/*! \brief Set whether contained objects are owned and released by the list. */

void FileInfoList::setReleaseObjects(bool releaseObjects)
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





/*! \brief Enable or disable internal locking on the list. */

void FileInfoList::setLockingEnabled(bool lockingEnabled)
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





/*! \brief Set the comparison method used for sorting the list. */

void FileInfoList::setComparisonMethod(uint comparisonMethod)
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





/*! \brief Sort the list using the given comparison method. */

void FileInfoList::sort(uint comparisonMethod)
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

    switch(comparisonMethod)
    {
      case 1:
        qsort(mArray,mLength,sizeof(FileInfoPtr),fileInfo_compare_1);
        break;

      case 2:
        qsort(mArray,mLength,sizeof(FileInfoPtr),fileInfo_compare_2);
    };
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Return the most recent file deletion time for the given generation id. */

time_t FileInfoList::getLastFileDeletionTimeByGenerationId(T::GenerationId generationId)
{
  FUNCTION_TRACE
  try
  {
    time_t lastTime = 0;

    if (mArray == nullptr ||  mLength == 0)
      return 0;

    AutoReadLock lock(mModificationLockPtr);
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      if (info != nullptr)
      {
        if (info->mGenerationId == generationId || (info->mFlags & T::FileInfo::Flags::DeletedFile) != 0)
        {
          if (info->mDeletionTime > lastTime)
            lastTime = info->mDeletionTime;
        }
      }
    }
    return lastTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Write the list contents to the named file. */

void FileInfoList::writeToFile(const std::string& filename)
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





/*! \brief Write the list contents to the named file with the given file mode. */

void FileInfoList::writeToFile(const std::string& filename,const char *filemode)
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
      fprintf(file,"%s\n",mArray[t]->getCsv().c_str());
    }

    fclose(file);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Print the list contents to the given stream. */

void FileInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr);

    stream << space(level) << "FileInfoList\n";
    for (uint t=0; t<mLength; t++)
      mArray[t]->print(stream,level+1,optionFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




}
}
