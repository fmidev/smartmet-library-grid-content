#include "ContentListStorage.h"


namespace SmartMet
{
namespace T
{


ContentListStorage::ContentListStorage()
{
  try
  {
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
      mContentLists[t].setComparisonMethod(T::ContentInfo::ComparisonMethod::file_message);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Constructor failed!",nullptr);
  }
}




ContentListStorage::ContentListStorage(const ContentListStorage& contentListStorage)
{
  try
  {
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
      mContentLists[t] = contentListStorage.mContentLists[t];

    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
      mContentLists[t].setComparisonMethod(T::ContentInfo::ComparisonMethod::file_message);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Copy Constructor failed!",nullptr);
  }
}




ContentListStorage::~ContentListStorage()
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





ContentInfo* ContentListStorage::addContentInfo(ContentInfo *contentInfo)
{
  try
  {
    if (!contentInfo)
      return nullptr;

    uint idx = getProducerStorageIndex(contentInfo->mProducerId);
    return mContentLists[idx].addContentInfo(contentInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentListStorage::clear()
{
  try
  {
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
      mContentLists[t].clear();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ContentListStorage::deleteContentInfoByFileId(T::FileId fileId)
{
  try
  {
    uint idx = getFileStorageIndex(fileId);
    return mContentLists[idx].deleteContentInfoByFileId(fileId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ContentListStorage::deleteContentInfoByFileIdAndMessageIndex(T::FileId fileId,T::MessageIndex messageIndex)
{
  try
  {
    uint idx = getFileStorageIndex(fileId);
    return mContentLists[idx].deleteContentInfoByFileIdAndMessageIndex(fileId,messageIndex);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ContentListStorage::deleteContentInfoByProducerId(T::ProducerId producerId)
{
  try
  {
    uint idx = getProducerStorageIndex(producerId);
    return mContentLists[idx].deleteContentInfoByProducerId(producerId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ContentListStorage::deleteContentInfoByGenerationId(T::GenerationId generationId)
{
  try
  {
    uint idx = getGenerationStorageIndex(generationId);
    return mContentLists[idx].deleteContentInfoByGenerationId(generationId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ContentListStorage::deleteContentInfoBySourceId(T::SourceId sourceId)
{
  try
  {
    uint idx = getSourceStorageIndex(sourceId);
    return mContentLists[idx].deleteContentInfoBySourceId(sourceId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ContentListStorage::deleteContentInfoByStorageId(T::StorageId storageId)
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
    return mContentLists[storageId].deleteContentInfoByStorageId(storageId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ContentListStorage::deleteMarkedContent()
{
  try
  {
    uint cnt = 0;
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
      cnt += mContentLists[t].deleteMarkedContent();

    return cnt;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ContentListStorage::markDeletedByFileId(T::FileId fileId)
{
  try
  {
    uint idx = getFileStorageIndex(fileId);
    return mContentLists[idx].markDeletedByFileId(fileId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ContentListStorage::markDeletedByFileIdAndMessageIndex(T::FileId fileId,T::MessageIndex messageIndex)
{
  try
  {
    uint idx = getFileStorageIndex(fileId);
    return mContentLists[idx].markDeletedByFileIdAndMessageIndex(fileId,messageIndex);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ContentListStorage::markDeletedByGenerationId(T::GenerationId generationId)
{
  try
  {
    uint idx = getGenerationStorageIndex(generationId);
    return mContentLists[idx].markDeletedByGenerationId(generationId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ContentListStorage::markDeletedByGenerationAndGeometryId(T::GenerationId generationId,T::GeometryId geometryId)
{
  try
  {
    uint idx = getGenerationStorageIndex(generationId);
    return mContentLists[idx].markDeletedByGenerationAndGeometryId(generationId,geometryId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ContentListStorage::markDeletedByProducerId(T::ProducerId producerId)
{
  try
  {
    uint idx = getProducerStorageIndex(producerId);
    return mContentLists[idx].markDeletedByProducerId(producerId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ContentListStorage::markDeletedBySourceId(T::SourceId sourceId)
{
  try
  {
    uint idx = getSourceStorageIndex(sourceId);
    return mContentLists[idx].markDeletedBySourceId(sourceId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ContentListStorage::markDeletedByStorageId(T::StorageId storageId)
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
    return mContentLists[storageId].deleteContentInfoByStorageId(storageId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




ContentInfo* ContentListStorage::getContentInfoByIndex(uint index) const
{
  try
  {
    uint len = 0;
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
    {
      len = mContentLists[t].getLength();
      if (index < len)
        return mContentLists[t].getContentInfoByIndex(index);

      index -= len;
    }
    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




ContentInfo* ContentListStorage::getContentInfoByFileIdAndMessageIndex(T::FileId fileId,T::MessageIndex messageIndex)
{
  try
  {
    uint idx = getFileStorageIndex(fileId);
    return mContentLists[idx].getContentInfoByFileIdAndMessageIndex(fileId,messageIndex);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool ContentListStorage::getContentInfoByFileIdAndMessageIndex(T::FileId fileId,T::MessageIndex messageIndex,ContentInfo& contentInfo)
{
  try
  {
    uint idx = getFileStorageIndex(fileId);
    return mContentLists[idx].getContentInfoByFileIdAndMessageIndex(fileId,messageIndex,contentInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentListStorage::getContentInfoList(ContentInfoList& contentInfoList)
{
  try
  {
    uint len = getLength();
    contentInfoList.clear();
    contentInfoList.setSize(len+10);
    for (uint t=0; t< MAX_CONTENT_SOURCES; t++)
    {
      mContentLists[t].getContentInfoListNoClear(contentInfoList);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentListStorage::getContentInfoList(T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,ContentInfoList& contentInfoList)
{
  try
  {
    uint idx = getFileStorageIndex(startFileId);
    while (idx < MAX_CONTENT_SOURCES)
    {
      mContentLists[idx].getContentInfoList(startFileId,startMessageIndex,maxRecords,contentInfoList);

      int len = contentInfoList.getLength();
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




void ContentListStorage::getContentInfoListByGeometryId(T::GeometryId geometryId,ContentInfoList& contentInfoList)
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentListStorage::getContentInfoListByFileId(T::FileId fileId,ContentInfoList& contentInfoList)
{
  try
  {
    uint idx = getFileStorageIndex(fileId);
    mContentLists[idx].getContentInfoListByFileId(fileId,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentListStorage::getContentInfoListByGenerationId(T::ProducerId producerId,T::GenerationId generationId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,ContentInfoList& contentInfoList)
{
  try
  {
    uint idx = getGenerationStorageIndex(generationId);
    mContentLists[idx].getContentInfoListByGenerationId(producerId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentListStorage::getContentInfoListByGenerationId(T::ProducerId producerId,T::GenerationId generationId,const std::string& startTime,const std::string& endTime,ContentInfoList& contentInfoList)
{
  try
  {
    uint idx = getGenerationStorageIndex(generationId);
    mContentLists[idx].getContentInfoListByGenerationId(producerId,generationId,startTime,endTime,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentListStorage::getContentInfoListByGenerationId(T::ProducerId producerId,T::GenerationId generationId,time_t startTimeUTC,time_t endTimeUTC,ContentInfoList& contentInfoList)
{
  try
  {
    uint idx = getGenerationStorageIndex(generationId);
    mContentLists[idx].getContentInfoListByGenerationId(producerId,generationId,startTimeUTC,endTimeUTC,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentListStorage::getContentInfoListByGenerationAndGeometryId(T::ProducerId producerId,T::GenerationId generationId,T::GeometryId geometryId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,ContentInfoList& contentInfoList)
{
  try
  {
    uint idx = getGenerationStorageIndex(generationId);
    mContentLists[idx].getContentInfoListByGenerationAndGeometryId(producerId,generationId,geometryId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentListStorage::getContentInfoListByProducerId(T::ProducerId producerId,ContentInfoList& contentInfoList)
{
  try
  {
    uint idx = getProducerStorageIndex(producerId);
    mContentLists[idx].getContentInfoListByProducerId(producerId,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentListStorage::getContentInfoListByProducerId(T::ProducerId producerId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,ContentInfoList& contentInfoList)
{
  try
  {
    uint idx = getProducerStorageIndex(producerId);
    mContentLists[idx].getContentInfoListByProducerId(producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentListStorage::getContentInfoListBySourceId(T::SourceId sourceId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,ContentInfoList& contentInfoList)
{
  try
  {
    uint idx = getSourceStorageIndex(sourceId);
    mContentLists[idx].getContentInfoListBySourceId(sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



std::size_t ContentListStorage::getHash()
{
  try
  {
    std::size_t hash = 0;
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
      hash += mContentLists[t].getHash();

    return hash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




std::size_t ContentListStorage::getHashByProducerId(T::ProducerId producerId)
{
  try
  {
    uint idx = getProducerStorageIndex(producerId);
    return mContentLists[idx].getHashByProducerId(producerId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ContentListStorage::getLength()const
{
  try
  {
    uint len = 0;
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
      len += mContentLists[t].getLength();

    return len;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ContentListStorage::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    for (uint t=0; t<MAX_CONTENT_SOURCES; t++)
      mContentLists[t].print(stream,level,optionFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ContentListStorage::getProducerStorageIndex(T::ProducerId producerId)
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



uint ContentListStorage::getSourceStorageIndex(T::SourceId sourceId)
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




uint ContentListStorage::getFileStorageIndex(T::FileId fileId)
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




uint ContentListStorage::getGenerationStorageIndex(T::GenerationId generationId)
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
