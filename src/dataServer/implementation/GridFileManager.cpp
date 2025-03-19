#include "GridFileManager.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/common/ShowFunction.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace DataServer
{



GridFileManager::GridFileManager()
{
  FUNCTION_TRACE
  try
  {
    mContentServer = nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





GridFileManager::~GridFileManager()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void GridFileManager::init(T::SessionId serverSessionId,ContentServer::ServiceInterface* contentServer)
{
  FUNCTION_TRACE
  try
  {
    mServerSessionId = serverSessionId;
    mContentServer = contentServer;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::addFile(GRID::GridFile *gridFile)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    gridFile->setCheckTime(time(nullptr));

    GRID::GridFile_sptr ptr;
    ptr.reset(gridFile);

    mFileList.insert(std::pair<uint,GRID::GridFile_sptr>(gridFile->getFileId(),ptr));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::clear()
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    mFileList.clear();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::deleteFile(GRID::GridFile *gridFile)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    deleteFileNoLock(gridFile->getFileId(),true);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::deleteFileById(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    deleteFileNoLock(fileId,true);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::deleteFilesByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    std::vector<uint> idList;
    {
      AutoReadLock lock(&mModificationLock);
      for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
      {
        if (it->second->getProducerId() == producerId)
          idList.emplace_back(it->first);
      }
    }

    if (idList.size() > 0)
    {
      AutoWriteLock lock(&mModificationLock);
      deleteFilesNoLock(idList,false);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::deleteFilesByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    std::vector<uint> idList;
    {
      AutoReadLock lock(&mModificationLock);
      for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
      {
        if (it->second->getGenerationId() == generationId)
          idList.emplace_back(it->first);
      }
    }

    if (idList.size() > 0)
    {
      AutoWriteLock lock(&mModificationLock);
      deleteFilesNoLock(idList,false);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::deleteFilesBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    std::vector<uint> idList;
    {
      AutoReadLock lock(&mModificationLock);
      for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
      {
        if (it->second->getSourceId() == sourceId)
          idList.emplace_back(it->first);
      }
    }

    if (idList.size() > 0)
    {
      AutoWriteLock lock(&mModificationLock);
      deleteFilesNoLock(idList,false);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::deleteFilesByAccessTime(time_t accessTime)
{
  FUNCTION_TRACE
  try
  {
    std::vector<uint> idList;
    {
      AutoReadLock lock(&mModificationLock);
      for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
      {
        if (it->second->getAccessTime() < accessTime  &&
            (it->second->getFlags() &  GRID::GridFile::Flags::LocalCacheExpected) == 0  &&
            (it->second->getFlags() &  GRID::GridFile::Flags::LocalCacheReady) == 0)
          idList.emplace_back(it->first);
      }
    }

    if (idList.size() > 0)
    {
      AutoWriteLock lock(&mModificationLock);
      deleteFilesNoLock(idList,false);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::deleteFilesByCheckTime(time_t checkTime)
{
  FUNCTION_TRACE
  try
  {
    std::vector<uint> idList;
    {
      AutoReadLock lock(&mModificationLock);
      for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
      {
        if (it->second->getCheckTime() < checkTime)
          idList.emplace_back(it->first);
      }
    }

    if (idList.size() > 0)
    {
      AutoWriteLock lock(&mModificationLock);
      deleteFilesNoLock(idList,false);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::deleteFilesByDeletionTime(time_t deletionTime)
{
  FUNCTION_TRACE
  try
  {
    std::vector<uint> idList;
    {
      AutoReadLock lock(&mModificationLock);
      for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
      {
        if (it->second->getDeletionTime() != 0  &&  it->second->getDeletionTime() < deletionTime)
          idList.emplace_back(it->first);
      }
    }

    if (idList.size() > 0)
    {
      AutoWriteLock lock(&mModificationLock);
      deleteFilesNoLock(idList,false);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





GRID::GridFile_sptr GridFileManager::getFileById(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    auto it = mFileList.find(fileId);

    if (it != mFileList.end())
    {
      GRID::GridFile_sptr gridFile = it->second;
      if (!gridFile->isMemoryMapped())
        gridFile->mapToMemory();
//        gridFile->read(gridFile->getFileName());

      return it->second;
    }
    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void GridFileManager::getFilesToBeCached(std::map<uint,std::string>& filenames)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfoList generationInfoList;
    mContentServer->getGenerationInfoList(mServerSessionId,generationInfoList);

    std::set<uint> idList;
    generationInfoList.getGenerationIdListByStatus(T::GenerationInfo::Status::Ready,idList);

    time_t deletionTime = time(0) + 180;
    AutoReadLock lock(&mModificationLock);

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      if (!it->second->isNetworkFile()  &&  (it->second->getFlags() &  GRID::GridFile::Flags::LocalCacheExpected)  &&
          (it->second->getFlags() &  GRID::GridFile::Flags::LocalCacheReady) == 0 &&
          (it->second->getDeletionTime() == 0 || it->second->getDeletionTime() > deletionTime))
      {
        if (idList.find(it->second->getGenerationId()) != idList.end())
          filenames.insert(std::pair<uint,std::string>(it->second->getFileId(),it->second->getFileName()));
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::getFilesInCache(std::map<uint,std::string>& filenames)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      if (!it->second->isNetworkFile()  &&  (it->second->getFlags() &  GRID::GridFile::Flags::LocalCacheExpected)  &&  (it->second->getFlags() &  GRID::GridFile::Flags::LocalCacheReady))
      {
        filenames.insert(std::pair<uint,std::string>(it->second->getFileId(),it->second->getFileName()));
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::getRequestCounters(GRID::RequestCounters& requestCounters,bool diskFiles,bool networkFiles)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      if (it->second->isNetworkFile())
      {
        if (networkFiles)
          it->second->getRequestCounters(requestCounters);
      }
      else
      {
        if (diskFiles)
          it->second->getRequestCounters(requestCounters);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






void GridFileManager::resetRequestCounters()
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      //if (it->second->isNetworkFile())
        it->second->resetRequestCounters();
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





GRID::GridFile_sptr GridFileManager::getFileByIdNoMapping(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    auto it = mFileList.find(fileId);

    if (it != mFileList.end())
    {
      //printf("Found file %u  %d\n",fileId,it->second->hasMessagePositionError());
      return it->second;
    }

    //printf("NOT Found file %u\n",fileId);
    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::size_t GridFileManager::getFileCount()
{
  FUNCTION_TRACE
  try
  {
    return mFileList.size();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::getStateAttributes(std::shared_ptr<T::AttributeNode> parent)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    uint mmapCount = 0;
    uint cacheExpectedCount = 0;
    uint cachedCount = 0;
    time_t deletionTime = time(0) + 180;

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      GRID::GridFile_sptr gridFile = it->second;
      if (gridFile->isMemoryMapped())
        mmapCount++;

      if (!gridFile->isNetworkFile()  &&  (gridFile->getFlags() &  GRID::GridFile::Flags::LocalCacheExpected)  &&
          (gridFile->getDeletionTime() == 0 || gridFile->getDeletionTime() > deletionTime))
      {
        cacheExpectedCount++;

        if ((gridFile->getFlags() &  GRID::GridFile::Flags::LocalCacheReady) != 0)
          cachedCount++;
      }
    }

    auto files = parent->addAttribute("Files");
    files->addAttribute("Total",mFileList.size());
    files->addAttribute("Memory mapped",mmapCount);
    files->addAttribute("Local caching expected",cacheExpectedCount);
    files->addAttribute("Local caching ready",cachedCount);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::print(std::ostream& stream,uint level,uint optionFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    stream << space(level) << "GridFileManager\n";
    stream << space(level) << "- numberOfFiles = " << getFileCount() << "\n";
    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      it->second->print(stream,level+1,optionFlags);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::deleteFileNoLock(uint fileId,bool sentMessageToContentServer)
{
  FUNCTION_TRACE
  try
  {
    auto gridRec = mFileList.find(fileId);

    if (gridRec != mFileList.end())
    {
      mFileList.erase(fileId);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::deleteFilesNoLock(std::vector<uint>& fileIdList,bool sentMessageToContentServer)
{
  FUNCTION_TRACE
  try
  {
    for (auto it = fileIdList.begin(); it != fileIdList.end(); ++it)
    {
      deleteFileNoLock(*it,sentMessageToContentServer);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridFileManager::deleteFilesNoLock(std::set<uint>& fileIdList,bool sentMessageToContentServer)
{
  FUNCTION_TRACE
  try
  {
    for (auto it = fileIdList.begin(); it != fileIdList.end(); ++it)
    {
      deleteFileNoLock(*it,sentMessageToContentServer);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}


}
}
