#include "GridStorage.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"
#include "grid-files/common/AutoWriteLock.h"
#include "grid-files/common/AutoReadLock.h"



namespace SmartMet
{
namespace DataServer
{



GridStorage::GridStorage()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





GridStorage::~GridStorage()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}






void GridStorage::addFile(GRID::GridFile *gribFile)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);

    GRID::GridFile_sptr ptr;
    ptr.reset(gribFile);

    if (mFileList.size() == 0)
    {
      mFileList.push_back(ptr);
      return;
    }

    int idx = getClosestFileIndexByIdNoLock(gribFile->getFileId());
    if (idx < 0)
    {
      mFileList.insert(mFileList.begin(),ptr);
      return;
    }

    if (idx >= (int)mFileList.size())
    {
      mFileList.push_back(ptr);
      return;
    }

    mFileList.insert(mFileList.begin()+idx+1,ptr);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void GridStorage::clear()
{
  try
  {
    AutoWriteLock lock(&mModificationLock);
    mFileList.clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void GridStorage::deleteFile(GRID::GridFile *gribFile)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);
    std::size_t sz = mFileList.size();
    for (std::size_t t=0; t<sz; t++)
    {
      if (mFileList[t].get() == gribFile)
      {
        mFileList.erase(mFileList.begin() + t);
        return;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void GridStorage::deleteFileById(uint fileId)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);

    int idx = getClosestFileIndexByIdNoLock(fileId);
    if (idx >= 0  &&  idx < (int)mFileList.size()  && mFileList[idx]->getFileId() == fileId)
      mFileList.erase(mFileList.begin() + idx);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void GridStorage::deleteFileByIndex(std::size_t fileIndex)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);

    if (fileIndex >= mFileList.size())
      return;

    mFileList.erase(mFileList.begin() + fileIndex);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void GridStorage::deleteFilesByGroupFlags(uint groupFlags)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);

    int sz = (int)mFileList.size() - 1;

    for (int t=sz; t>=0; t--)
    {
      if ((mFileList[t]->getGroupFlags() & groupFlags) != 0)
        mFileList.erase(mFileList.begin() + t);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void GridStorage::deleteFilesByProducerId(uint producerId)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);

    int sz = (int)mFileList.size() - 1;

    for (int t=sz; t>=0; t--)
    {
      if (mFileList[t]->getProducerId() == producerId)
        mFileList.erase(mFileList.begin() + t);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void GridStorage::deleteFilesByGenerationId(uint generationId)
{
  try
  {
    AutoWriteLock lock(&mModificationLock);

    int sz = (int)mFileList.size() - 1;

    for (int t=sz; t>=0; t--)
    {
      if (mFileList[t]->getGenerationId() == generationId)
        mFileList.erase(mFileList.begin() + t);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





GRID::GridFile_sptr GridStorage::getFileById(uint fileId)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    int idx = getClosestFileIndexByIdNoLock(fileId);
    if (idx >= 0  &&  idx < (int)mFileList.size()  && mFileList[idx]->getFileId() == fileId)
    {
      GRID::GridFile_sptr gribFile = mFileList[idx];
      if (!gribFile->isMemoryMapped())
        gribFile->read(gribFile->getFileName());
      return mFileList[idx];
    }
    return NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





GRID::GridFile_sptr GridStorage::getFileByIdNoMapping(uint fileId)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    int idx = getClosestFileIndexByIdNoLock(fileId);
    if (idx >= 0  &&  idx < (int)mFileList.size()  && mFileList[idx]->getFileId() == fileId)
    {
      GRID::GridFile_sptr gribFile = mFileList[idx];
      return mFileList[idx];
    }
    return NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





GRID::GridFile_sptr GridStorage::getFileByIndex(std::size_t fileIndex)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    if (fileIndex >= mFileList.size())
      return NULL;

    return mFileList[fileIndex];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int GridStorage::getClosestFileIndexById(uint fileId)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    return getClosestFileIndexByIdNoLock(fileId);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




int GridStorage::getClosestFileIndexByIdNoLock(uint fileId)
{
  try
  {
    int high = (int)mFileList.size()-1;
    //printf("COMPARE %d (%u)\n",high,fileId);
    if (high < 0)
      return -1;

    int low = 0;

    while (low <= high)
    {
      int mid = low + (high-low) / 2;
      //printf(" --  %d < %d < %d\n",low,mid,high);
      GRID::GridFile_sptr file = mFileList[mid];

      uint id = file->getFileId();
      if (id == fileId)
        return mid;

      if (fileId > id)
        low = mid + 1;
      else
      if (fileId < id)
        high = mid-1;
    }

    if (low >= high)
      return high;

    while (low >= 0)
    {
      GRID::GridFile_sptr file = mFileList[low];
      if (file->getFileId() < fileId)
        return low;
      low--;
    }
    return -1;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




std::size_t GridStorage::getFileCount()
{
  try
  {
    return mFileList.size();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void GridStorage::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    stream << space(level) << "GridStorage\n";
    stream << space(level) << "- numberOfFiles = " << getFileCount() << "\n";

    std::size_t sz = mFileList.size();
    for (std::size_t t=0; t<sz; t++)
    {
      mFileList[t]->print(stream,level+1,optionFlags);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}


}
}
