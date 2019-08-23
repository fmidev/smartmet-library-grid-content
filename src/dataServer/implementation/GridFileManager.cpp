#include "GridFileManager.h"
#include <grid-files/common/Exception.h>
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void GridFileManager::init(ContentServer::ServiceInterface* contentServer)
{
  FUNCTION_TRACE
  try
  {
    mContentServer = contentServer;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GridFileManager::addFile(GRID::GridFile *gridFile)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    gridFile->setCheckTime(time(nullptr));

    GRID::GridFile_sptr ptr;
    ptr.reset(gridFile);

    mFileList.insert(std::pair<uint,GRID::GridFile_sptr>(gridFile->getFileId(),ptr));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GridFileManager::addFileUser(uint fileId,uint userFileId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    auto it = mFileList.find(fileId);
    if (it != mFileList.end())
      it->second->addUser(userFileId);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GridFileManager::clear()
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    mFileList.clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GridFileManager::deleteFile(GRID::GridFile *gridFile)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    deleteFileNoLock(gridFile->getFileId(),true);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GridFileManager::deleteFileById(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    deleteFileNoLock(fileId,true);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GridFileManager::deleteFilesByGroupFlags(uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    std::vector<uint> idList;

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      if ((it->second->getGroupFlags() & groupFlags) != 0)
        idList.push_back(it->first);
    }

    deleteFilesNoLock(idList,false);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GridFileManager::deleteFilesByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    std::vector<uint> idList;

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      if (it->second->getProducerId() == producerId)
        idList.push_back(it->first);
    }

    deleteFilesNoLock(idList,false);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GridFileManager::deleteFilesByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    std::vector<uint> idList;

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      if (it->second->getGenerationId() == generationId)
        idList.push_back(it->first);
    }

    deleteFilesNoLock(idList,false);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GridFileManager::deleteFilesBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    std::vector<uint> idList;

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      if (it->second->getSourceId() == sourceId)
        idList.push_back(it->first);
    }

    deleteFilesNoLock(idList,false);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GridFileManager::deleteFilesByCheckTime(time_t checkTime)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    std::vector<uint> idList;

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      if (it->second->getCheckTime() < checkTime)
        idList.push_back(it->first);
    }

    deleteFilesNoLock(idList,true);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GridFileManager::deleteVirtualFiles()
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    std::vector<uint> idList;

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      if (it->second->isVirtual())
      {
        idList.push_back(it->first);
      }
      else
      {
        it->second->deleteUsers();
      }
    }

    deleteFilesNoLock(idList,false);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GridFileManager::getVirtualFiles(std::set<uint>& idList)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      if (it->second->isVirtual())
      {
        idList.insert(it->first);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




GRID::GridFile_sptr GridFileManager::getFileById(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





GRID::GridFile_sptr GridFileManager::getFileByIdNoMapping(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    auto it = mFileList.find(fileId);

    if (it != mFileList.end())
      return it->second;

    return nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GridFileManager::clearCachedValues(uint hitsRequired,uint timePeriod)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      if (it->second->isMemoryMapped())
      {
        std::size_t sz = it->second->getNumberOfMessages();
        for (std::size_t t=0; t< sz; t++)
        {
          GRID::Message *msg = it->second->getMessageByIndex(t);
          if (msg != nullptr)
            msg->clearCachedValues(hitsRequired,timePeriod);
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GridFileManager::print(std::ostream& stream,uint level,uint optionFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);
    stream << space(level) << "GridFileManager\n";
    stream << space(level) << "- numberOfFiles = " << getFileCount() << "\n";
    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      it->second->print(stream,level+1,optionFlags);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GridFileManager::deleteFileNoLock(uint fileId,bool sentMessageToContentServer)
{
  FUNCTION_TRACE
  try
  {
    //printf("--- deleteFileNoLock %u\n",fileId);

    auto gridRec = mFileList.find(fileId);

    if (gridRec != mFileList.end())
    {
      std::set<uint> userList;
      gridRec->second->getUsers(userList);
      for (auto it = userList.begin();it != userList.end(); ++it)
      {
        deleteFileNoLock(*it,sentMessageToContentServer);
      }

      if (sentMessageToContentServer &&  gridRec->second->isVirtual())
      {
        // We should tell the content server when virtual files are removed.
        mContentServer->deleteFileInfoById(0,fileId);
      }

      mFileList.erase(fileId);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}


}
}
