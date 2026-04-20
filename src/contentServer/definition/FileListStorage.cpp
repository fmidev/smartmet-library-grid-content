#include "FileListStorage.h"

namespace SmartMet
{
namespace T
{


FileListStorage::FileListStorage()
{
  try
  {
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
    {
      mFileLists[t].setComparisonMethod(T::FileInfo::ComparisonMethod::fileId);

    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Constructor failed!",nullptr);
  }
}




FileListStorage::FileListStorage(FileListStorage& fileListStorage)
{
  try
  {
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
    {
      mFileLists[t] = fileListStorage.mFileLists[t];
      mFileLists[t].setComparisonMethod(T::FileInfo::ComparisonMethod::fileId);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Copy Constructor failed!",nullptr);
  }
}




FileListStorage::~FileListStorage()
{
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void FileListStorage::addFileInfo(FileInfo *fileInfo)
{
  try
  {
    if (fileInfo == nullptr)
      return;

    uint idx = getFileStorageIndex(fileInfo->mFileId);
    return mFileLists[idx].addFileInfo(fileInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void FileListStorage::clear()
{
  try
  {
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
      mFileLists[t].clear();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool FileListStorage::deleteFileInfoById(T::FileId fileId)
{
  try
  {
    uint idx = getFileStorageIndex(fileId);
    return mFileLists[idx].deleteFileInfoById(fileId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint FileListStorage::deleteFileInfoByProducerId(T::ProducerId producerId)
{
  try
  {
    uint idx = getProducerStorageIndex(producerId);
    return mFileLists[idx].deleteFileInfoByProducerId(producerId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint FileListStorage::deleteFileInfoByGenerationId(T::GenerationId generationId)
{
  try
  {
    uint idx = getGenerationStorageIndex(generationId);
    return mFileLists[idx].deleteFileInfoByGenerationId(generationId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool FileListStorage::deleteFileInfoByIndex(uint index)
{
  try
  {
    uint len = 0;
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
    {
      len = mFileLists[t].getLength();
      if (index < len)
        return mFileLists[t].deleteFileInfoByIndex(index);

      index -= len;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool FileListStorage::deleteFileInfoByName(const std::string& filename)
{
  try
  {
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
    {
      if (mFileLists[t].deleteFileInfoByName(filename))
        return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint FileListStorage::deleteFileInfoBySourceId(T::SourceId sourceId)
{
  try
  {
    uint idx = getSourceStorageIndex(sourceId);
    return mFileLists[idx].deleteFileInfoBySourceId(sourceId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint FileListStorage::deleteFileInfoByStorageId(T::StorageId storageId)
{
  try
  {
    if (storageId >= MAX_CONTENT_SOURCES)
    {
      Fmi::Exception exception(BCP,"Invalid storage index!",nullptr);
      exception.addParameter("storageId",std::to_string(storageId));
      exception.addParameter("maxContentSources",std::to_string(MAX_CONTENT_SOURCES));
      throw exception;
    }
    return mFileLists[storageId].deleteFileInfoByStorageId(storageId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint FileListStorage::deleteMarkedFiles()
{
  try
  {
    uint cnt = 0;
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
      cnt += mFileLists[t].deleteMarkedFiles();

    return cnt;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




FileInfo* FileListStorage::getFileInfoById(T::FileId fileId)
{
  try
  {
    uint idx = getFileStorageIndex(fileId);
    return mFileLists[idx].getFileInfoById(fileId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool FileListStorage::getFileInfoById(T::FileId fileId,FileInfo& fileInfo)
{
  try
  {
    uint idx = getFileStorageIndex(fileId);
    return mFileLists[idx].getFileInfoById(fileId,fileInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




FileInfo* FileListStorage::getFileInfoByName(const std::string& filename)
{
  try
  {
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
    {
      FileInfo *info = mFileLists[t].getFileInfoByName(filename);
      if (info)
        return info;
    }
    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool FileListStorage::getFileInfoByName(const std::string& filename,FileInfo& fileInfo)
{
  try
  {
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
    {
      if (mFileLists[t].getFileInfoByName(filename,fileInfo))
        return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




FileInfo* FileListStorage::getFileInfoByIndex(uint index)
{
  try
  {
    uint len = 0;
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
    {
      len = mFileLists[t].getLength();
      if (index < len)
        return mFileLists[t].getFileInfoByIndex(index);

      index -= len;
    }
    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint FileListStorage::getFileInfoCountByProducerId(T::ProducerId producerId)
{
  try
  {
    uint idx = getProducerStorageIndex(producerId);
    return mFileLists[idx].getFileInfoCountByProducerId(producerId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint FileListStorage::getFileInfoCountByGenerationId(T::GenerationId generationId)
{
  try
  {
    uint idx = getGenerationStorageIndex(generationId);
    return mFileLists[idx].getFileInfoCountByGenerationId(generationId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint FileListStorage::getFileInfoCountBySourceId(T::SourceId sourceId)
{
  try
  {
    uint idx = getSourceStorageIndex(sourceId);
    return mFileLists[idx].getFileInfoCountBySourceId(sourceId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void FileListStorage::getFileInfoList(FileInfoList& fileInfoList)
{
  try
  {
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
    {
      mFileLists[t].getFileInfoListNoClear(fileInfoList);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void FileListStorage::getFileInfoList(T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList)
{
  try
  {
    uint idx = getFileStorageIndex(startFileId);
    while (idx < MAX_CONTENT_SOURCES)
    {
      mFileLists[idx].getFileInfoListNoClear(startFileId,maxRecords,fileInfoList);

      int len = fileInfoList.getLength();
      if (len >= maxRecords)
        return;

      maxRecords -= len;
      idx++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void FileListStorage::getFileInfoListByProducerId(T::ProducerId producerId,FileInfoList& fileInfoList)
{
  try
  {
    uint idx = getProducerStorageIndex(producerId);
    mFileLists[idx].getFileInfoListByProducerId(producerId,fileInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void FileListStorage::getFileInfoListByProducerId(T::ProducerId producerId,T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList)
{
  try
  {
    uint idx = getProducerStorageIndex(producerId);
    mFileLists[idx].getFileInfoListByProducerId(producerId,startFileId,maxRecords,fileInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void FileListStorage::getFileInfoListByGenerationId(T::GenerationId generationId,FileInfoList& fileInfoList)
{
  try
  {
    uint idx = getGenerationStorageIndex(generationId);
    mFileLists[idx].getFileInfoListByGenerationId(generationId,fileInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void FileListStorage::getFileInfoListByGenerationId(T::GenerationId generationId,T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList)
{
  try
  {
    uint idx = getGenerationStorageIndex(generationId);
    mFileLists[idx].getFileInfoListByGenerationId(generationId,startFileId,maxRecords,fileInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void FileListStorage::getFileInfoListBySourceId(T::SourceId sourceId,FileInfoList& fileInfoList)
{
  try
  {
    uint idx = getSourceStorageIndex(sourceId);
    mFileLists[idx].getFileInfoListBySourceId(sourceId,fileInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void FileListStorage::getFileInfoListBySourceId(T::SourceId sourceId,T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList)
{
  try
  {
    uint idx = getSourceStorageIndex(sourceId);
    mFileLists[idx].getFileInfoListBySourceId(sourceId,startFileId,maxRecords,fileInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




std::size_t FileListStorage::getHash()
{
  try
  {
    std::size_t hash = 0;
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
      hash += mFileLists[t].getHash();

    return hash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




std::size_t FileListStorage::getHashByProducerId(T::ProducerId producerId)
{
  try
  {
    uint idx = getProducerStorageIndex(producerId);
    return mFileLists[idx].getHashByProducerId(producerId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




std::size_t FileListStorage::getHashByGenerationId(T::GenerationId generationId)
{
  try
  {
    uint idx = getGenerationStorageIndex(generationId);
    return mFileLists[idx].getHashByGenerationId(generationId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint FileListStorage::getLength()
{
  try
  {
    uint len = 0;
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
      len += mFileLists[t].getLength();

    return len;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint FileListStorage::markDeleted()
{
  try
  {
    uint cnt = 0;
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
      cnt += mFileLists[t].markDeleted();

    return cnt;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint FileListStorage::markDeletedById(T::FileId fileId)
{
  try
  {
    uint idx = getFileStorageIndex(fileId);
    return mFileLists[idx].markDeletedById(fileId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint FileListStorage::markDeletedByProducerId(T::ProducerId producerId)
{
  try
  {
    uint idx = getProducerStorageIndex(producerId);
    return mFileLists[idx].markDeletedByProducerId(producerId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint FileListStorage::markDeletedByGenerationId(T::GenerationId generationId)
{
  try
  {
    uint idx = getGenerationStorageIndex(generationId);
    return mFileLists[idx].markDeletedByGenerationId(generationId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint FileListStorage::markDeletedBySourceId(T::SourceId sourceId)
{
  try
  {
    uint idx = getSourceStorageIndex(sourceId);
    return mFileLists[idx].markDeletedBySourceId(sourceId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint FileListStorage::markDeletedByStorageId(T::StorageId storageId)
{
  try
  {
    if (storageId >= MAX_CONTENT_SOURCES)
    {
      Fmi::Exception exception(BCP,"Invalid storage index!",nullptr);
      exception.addParameter("storageId",std::to_string(storageId));
      exception.addParameter("maxContentSources",std::to_string(MAX_CONTENT_SOURCES));
      throw exception;
    }
    return mFileLists[storageId].markDeletedByStorageId(storageId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void FileListStorage::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
      mFileLists[t].print(stream,level,optionFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint FileListStorage::getProducerStorageIndex(T::ProducerId producerId)
{
  try
  {
    uint idx = (producerId & 0xFF000000) >> 24;
    if (idx >= MAX_CONTENT_SOURCES)
    {
      Fmi::Exception exception(BCP,"Invalid storage index!",nullptr);
      exception.addParameter("producerId",std::to_string(producerId));
      exception.addParameter("index",std::to_string(idx));
      exception.addParameter("maxContentSources",std::to_string(MAX_CONTENT_SOURCES));
      throw exception;
    }
    return idx;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint FileListStorage::getSourceStorageIndex(T::SourceId sourceId)
{
  try
  {
    uint idx = (sourceId & 0xFF000000) >> 24;
    if (idx >= MAX_CONTENT_SOURCES)
    {
      Fmi::Exception exception(BCP,"Invalid storage index!",nullptr);
      exception.addParameter("sourceId",std::to_string(sourceId));
      exception.addParameter("index",std::to_string(idx));
      exception.addParameter("maxContentSources",std::to_string(MAX_CONTENT_SOURCES));
      throw exception;
    }
    return idx;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint FileListStorage::getFileStorageIndex(T::FileId fileId)
{
  try
  {
    uint idx = (fileId & 0xFFFFFFFF00000000) >> 32;
    if (idx >= MAX_CONTENT_SOURCES)
    {
      Fmi::Exception exception(BCP,"Invalid storage index!",nullptr);
      exception.addParameter("fileId",std::to_string(fileId));
      exception.addParameter("index",std::to_string(idx));
      exception.addParameter("maxContentSources",std::to_string(MAX_CONTENT_SOURCES));
      throw exception;
    }
    return idx;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint FileListStorage::getGenerationStorageIndex(T::GenerationId generationId)
{
  try
  {
    uint idx = (generationId & 0xFFFFFFFF00000000) >> 32;
    if (idx >= MAX_CONTENT_SOURCES)
    {
      Fmi::Exception exception(BCP,"Invalid storage index!",nullptr);
      exception.addParameter("generationId",std::to_string(generationId));
      exception.addParameter("index",std::to_string(idx));
      exception.addParameter("maxContentSources",std::to_string(MAX_CONTENT_SOURCES));
      throw exception;
    }
    return idx;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



}
}
