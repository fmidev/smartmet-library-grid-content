#include "GridStorage.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"
#include "grid-files/common/AutoWriteLock.h"
#include "grid-files/common/AutoReadLock.h"
#include "grid-files/common/ShowFunction.h"


#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace DataServer
{



GridStorage::GridStorage()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





GridStorage::~GridStorage()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}






void GridStorage::addFile(GRID::GridFile *gribFile)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    GRID::GridFile_sptr ptr;
    ptr.reset(gribFile);

    mFileList.insert(std::pair<uint,GRID::GridFile_sptr>(gribFile->getFileId(),ptr));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GridStorage::clear()
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    mFileList.clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GridStorage::deleteFile(GRID::GridFile *gribFile)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    mFileList.erase(gribFile->getFileId());
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GridStorage::deleteFileById(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    mFileList.erase(fileId);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GridStorage::deleteFilesByGroupFlags(uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    std::vector<uint> idList;

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      if ((it->second->getGroupFlags() & groupFlags) != 0)
        idList.push_back(it->first);
    }

    deleteFilesNoLock(idList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GridStorage::deleteFilesByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    std::vector<uint> idList;

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      if (it->second->getProducerId() == producerId)
        idList.push_back(it->first);
    }

    deleteFilesNoLock(idList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GridStorage::deleteFilesByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    std::vector<uint> idList;

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      if (it->second->getGenerationId() == generationId)
        idList.push_back(it->first);
    }

    deleteFilesNoLock(idList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GridStorage::deleteFilesBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    std::vector<uint> idList;

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      if (it->second->getSourceId() == sourceId)
        idList.push_back(it->first);
    }

    deleteFilesNoLock(idList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GridStorage::deleteFilesByCheckTime(time_t checkTime)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    std::vector<uint> idList;

    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      if (it->second->getCheckTime() < checkTime)
        idList.push_back(it->first);
    }

    deleteFilesNoLock(idList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





GRID::GridFile_sptr GridStorage::getFileById(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    auto it = mFileList.find(fileId);

    if (it != mFileList.end())
    {
      GRID::GridFile_sptr gribFile = it->second;
      if (!gribFile->isMemoryMapped())
        gribFile->read(gribFile->getFileName());

      return it->second;
    }
    return NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





GRID::GridFile_sptr GridStorage::getFileByIdNoMapping(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    auto it = mFileList.find(fileId);

    if (it != mFileList.end())
      return it->second;

    return NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





std::size_t GridStorage::getFileCount()
{
  FUNCTION_TRACE
  try
  {
    return mFileList.size();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GridStorage::print(std::ostream& stream,uint level,uint optionFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    stream << space(level) << "GridStorage\n";
    stream << space(level) << "- numberOfFiles = " << getFileCount() << "\n";
    for ( auto it = mFileList.begin(); it != mFileList.end(); ++it  )
    {
      it->second->print(stream,level+1,optionFlags);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GridStorage::deleteFilesNoLock(std::vector<uint>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    for (auto it = fileIdList.begin(); it != fileIdList.end(); ++it)
    {
      mFileList.erase(*it);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}


}
}
