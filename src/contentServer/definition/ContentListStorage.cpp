#include "ContentListStorage.h"


namespace SmartMet
{
namespace T
{


/*! \brief Default constructor for ContentListStorage. */

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




/*! \brief Copy constructor for ContentListStorage. */

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




/*! \brief Destructor for ContentListStorage. */

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





/*! \brief Add a ContentInfo record into the appropriate sub-list. */

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




/*! \brief Remove all entries from every sub-list. */

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




/*! \brief Delete all entries whose file id matches the given value. */

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




/*! \brief Delete the entry matching the given file id and message index. */

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




/*! \brief Delete all entries whose producer id matches the given value. */

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




/*! \brief Delete all entries whose generation id matches the given value. */

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




/*! \brief Delete all entries whose source id matches the given value. */

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





/*! \brief Delete all entries whose storage id matches the given value. */

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




/*! \brief Remove all entries previously flagged for deletion. */

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




/*! \brief Flag entries whose file id matches the given value as deleted. */

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




/*! \brief Flag the entry matching the given file id and message index as deleted. */

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




/*! \brief Flag entries whose generation id matches the given value as deleted. */

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




/*! \brief Flag entries matching the given generation and geometry ids as deleted. */

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




/*! \brief Flag entries whose producer id matches the given value as deleted. */

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




/*! \brief Flag entries whose source id matches the given value as deleted. */

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




/*! \brief Flag entries whose storage id matches the given value as deleted. */

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




/*! \brief Return the ContentInfo at the given global index across all sub-lists. */

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




/*! \brief Return the entry matching the given file id and message index. */

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




/*! \brief Copy the entry matching the given file id and message index into the output ContentInfo. */

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




/*! \brief Copy the entire combined list into the given target list. */

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




/*! \brief Copy a paginated slice of the combined list starting from the given file id and message index. */

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




/*! \brief Append entries matching the given geometry id to the target list. */

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




/*! \brief Append entries matching the given file id to the target list. */

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




/*! \brief Append a paginated slice of entries matching the given generation id. */

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




/*! \brief Append entries matching the given generation id and string time range. */

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




/*! \brief Append entries matching the given generation id and UTC time range. */

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




/*! \brief Append a paginated slice of entries matching the given generation and geometry ids. */

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




/*! \brief Append all entries matching the given producer id to the target list. */

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




/*! \brief Append a paginated slice of entries matching the given producer id. */

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




/*! \brief Append a paginated slice of entries matching the given source id. */

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



/*! \brief Return a hash value computed from all entries across the storage. */

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





/*! \brief Return a hash value for entries belonging to the given storage id. */

std::size_t ContentListStorage::getHashByStorageId(T::StorageId storageId)
{
  try
  {
    if (storageId < MAX_CONTENT_SOURCES)
      return mContentLists[storageId].getHash();

    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return a hash value for entries belonging to the given producer id. */

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




/*! \brief Return the total number of entries across all sub-lists. */

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




/*! \brief Print the storage contents to the given stream. */

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




/*! \brief Return the sub-list index for the given producer id. */

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



/*! \brief Return the sub-list index for the given source id. */

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




/*! \brief Return the sub-list index for the given file id. */

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




/*! \brief Return the sub-list index for the given generation id. */

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
