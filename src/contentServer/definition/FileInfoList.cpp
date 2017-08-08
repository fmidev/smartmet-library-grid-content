#include "FileInfoList.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"
#include "grid-files/common/AutoWriteLock.h"
#include "grid-files/common/AutoReadLock.h"
#include "grid-files/common/AutoThreadLock.h"


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
    mComparisonMethod = FileInfo::ComparisonMethod::fileId;
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void FileInfoList::operator=(FileInfoList& fileInfoList)
{
  try
  {
    if (&fileInfoList == this)
      return;

    clear();

    AutoWriteLock lock(&mModificationLock);

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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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

    AutoWriteLock lock(&mModificationLock);

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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void FileInfoList::clear()
{
  try
  {
    AutoWriteLock lock(&mModificationLock);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void FileInfoList::increaseSize(uint newSize)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





FileInfo* FileInfoList::getFileInfoById(uint fileId)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    FileInfo search;
    search.mFileId = fileId;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileId,search);
    if (idx < 0  ||  idx >= (int)getLength())
      return NULL;

    FileInfo *info = getFileInfoByIndexNoCheck(idx);
    if (info != NULL  &&  info->mFileId == fileId)
      return info;

    return NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int FileInfoList::getClosestIndex(FileInfo::ComparisonMethod comparisonMethod,FileInfo& fileInfo)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    return getClosestIndexNoLock(comparisonMethod,fileInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
        while (mid < (int)mSize  &&  mArray[mid] != NULL  &&   mArray[mid]->compare(comparisonMethod,&fileInfo) < 0)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





FileInfo* FileInfoList::getFileInfoByName(std::string filename)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    FileInfo search;
    search.mName = filename;
    int idx = getClosestIndexNoLock(FileInfo::ComparisonMethod::fileName,search);
    if (idx < 0  ||  idx >= (int)getLength())
      return NULL;

    FileInfo *info = getFileInfoByIndexNoCheck(idx);
    if (info != NULL  &&  info->mName == filename)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





FileInfo* FileInfoList::getFileInfoByIndex(uint index)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    if (index > mLength)
      return NULL;

    return mArray[index];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void FileInfoList::getFileInfoList(uint startFileId,uint maxRecords,FileInfoList& fileInfoList)
{
  try
  {
    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      printf("%s : Not supported when the records are not ordered by the 'fileId' field!\n",__FUNCTION__);
      return;
    }

    AutoReadLock lock(&mModificationLock);
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
      if (info != NULL  &&  info->mFileId >= startFileId)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void FileInfoList::getFileInfoListByProducerId(uint producerId,uint startFileId,uint maxRecords,FileInfoList& fileInfoList)
{
  try
  {
    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      printf("%s : Not supported when the records are not ordered by the 'fileId' field!\n",__FUNCTION__);
      return;
    }

    AutoReadLock lock(&mModificationLock);
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
      if (info != NULL  &&  info->mFileId >= startFileId  &&  info->mProducerId == producerId)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void FileInfoList::getFileInfoListByGenerationId(uint generationId,uint startFileId,uint maxRecords,FileInfoList& fileInfoList)
{
  try
  {
    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      printf("%s : Not supported when the records are not ordered by the 'fileId' field!\n",__FUNCTION__);
      return;
    }

    AutoReadLock lock(&mModificationLock);
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
      if (info != NULL  &&  info->mFileId >= startFileId  &&  info->mGenerationId == generationId)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void FileInfoList::getFileInfoListByGroupFlags(uint groupFlags,uint startFileId,uint maxRecords,FileInfoList& fileInfoList)
{
  try
  {
    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      printf("%s : Not supported when the records are not ordered by the 'fileId' field!\n",__FUNCTION__);
      return;
    }

    AutoReadLock lock(&mModificationLock);
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
      if (info != NULL  &&  info->mFileId >= startFileId  &&  (info->mGroupFlags & groupFlags) != 0)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void FileInfoList::getFileInfoListBySourceId(uint sourceId,uint startFileId,uint maxRecords,FileInfoList& fileInfoList)
{
  try
  {
    if (mComparisonMethod != FileInfo::ComparisonMethod::fileId)
    {
      printf("%s : Not supported when the records are not ordered by the 'fileId' field!\n",__FUNCTION__);
      return;
    }

    AutoReadLock lock(&mModificationLock);
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
      if (info != NULL  &&  info->mFileId >= startFileId  &&  info->mSourceId == sourceId)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





bool FileInfoList::deleteFileInfoById(uint fileId)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);

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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





uint FileInfoList::deleteFileInfoByGroupFlags(uint groupFlags)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL)
      {
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
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




uint FileInfoList::deleteFileInfoByProducerId(uint producerId)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL)
      {
        if (info->mProducerId == producerId)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





uint FileInfoList::deleteFileInfoByGenerationId(uint generationId)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL)
      {
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
    }
    mLength = p;
    return count;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





bool FileInfoList::deleteFileInfoByIndex(uint index)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);

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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





uint FileInfoList::deleteFileInfoBySourceId(uint sourceId)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint count = 0;
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      FileInfo *info = mArray[t];
      mArray[t] = NULL;
      if (info != NULL)
      {
        if (info->mSourceId == sourceId)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void FileInfoList::setComparisonMethod(FileInfo::ComparisonMethod comparisonMethod)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);
    mComparisonMethod = comparisonMethod;
    if (mLength == 0)
      return;

    fileInfo_comparisonMethod = comparisonMethod;

    qsort(mArray,mLength,sizeof(FileInfoPtr),fileInfo_compare);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void FileInfoList::sort(FileInfo::ComparisonMethod comparisonMethod)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);
    mComparisonMethod = comparisonMethod;

    AutoThreadLock globalLock(&FileInfoList_sortLock);

    fileInfo_comparisonMethod = comparisonMethod;

    qsort(mArray,mLength,sizeof(FileInfoPtr),fileInfo_compare);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void FileInfoList::writeToFile(std::string filename,const char *filemode)
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
      fprintf(file,"%s\n",mArray[t]->getCsv().c_str());
    }

    fclose(file);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void FileInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    stream << space(level) << "FileInfoList\n";
    for (uint t=0; t<mLength; t++)
      mArray[t]->print(stream,level+1,optionFlags);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




}
}
