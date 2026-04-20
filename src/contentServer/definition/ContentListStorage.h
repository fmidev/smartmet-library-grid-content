#pragma once

#include "ContentInfoList.h"
#include "GenerationInfoList.h"
#include "ParameterLevelInfoList.h"
#include "LevelInfoList.h"

#include <grid-files/common/ModificationLock.h>
#include <vector>
#include <set>


namespace SmartMet
{
namespace T
{

#define MAX_CONTENT_SOURCES 10


class ContentListStorage
{
  public:
                        ContentListStorage();
                        ContentListStorage(const ContentListStorage& contentListStorage);
    virtual             ~ContentListStorage();

    ContentInfo*        addContentInfo(ContentInfo *contentInfo);
    void                clear();
    uint                deleteContentInfoByFileId(T::FileId fileId);
    uint                deleteContentInfoByFileIdAndMessageIndex(T::FileId fileId,T::MessageIndex messageIndex);
    uint                deleteContentInfoByProducerId(T::ProducerId producerId);

    uint                deleteContentInfoByGenerationId(T::GenerationId generationId);
    uint                deleteContentInfoBySourceId(T::SourceId sourceId);
    uint                deleteContentInfoByStorageId(T::StorageId storageId);
    uint                deleteMarkedContent();

    uint                markDeletedByFileId(T::FileId fileId);
    uint                markDeletedByFileIdAndMessageIndex(T::FileId fileId,T::MessageIndex messageIndex);
    uint                markDeletedByGenerationId(T::GenerationId generationId);
    uint                markDeletedByGenerationAndGeometryId(T::GenerationId generationId,T::GeometryId geometryId);
    uint                markDeletedByProducerId(T::ProducerId producerId);
    uint                markDeletedBySourceId(T::SourceId sourceId);
    uint                markDeletedByStorageId(T::StorageId storageId);

    ContentInfo*        getContentInfoByIndex(uint index) const;
    ContentInfo*        getContentInfoByFileIdAndMessageIndex(T::FileId fileId,T::MessageIndex messageIndex);
    bool                getContentInfoByFileIdAndMessageIndex(T::FileId fileId,T::MessageIndex messageIndex,ContentInfo& contentInfo);

    void                getContentInfoList(ContentInfoList& contentInfoList);
    void                getContentInfoList(T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,ContentInfoList& contentInfoList);
    void                getContentInfoListByGeometryId(T::GeometryId geometryId,ContentInfoList& contentInfoList);
    void                getContentInfoListByFileId(T::FileId fileId,ContentInfoList& contentInfoList);

    void                getContentInfoListByGenerationId(T::ProducerId producerId,T::GenerationId generationId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,ContentInfoList& contentInfoList);
    void                getContentInfoListByGenerationId(T::ProducerId producerId,T::GenerationId generationId,const std::string& startTime,const std::string& endTime,ContentInfoList& contentInfoList);
    void                getContentInfoListByGenerationId(T::ProducerId producerId,T::GenerationId generationId,time_t startTimeUTC,time_t endTimeUTC,ContentInfoList& contentInfoList);
    void                getContentInfoListByGenerationAndGeometryId(T::ProducerId producerId,T::GenerationId generationId,T::GeometryId geometryId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,ContentInfoList& contentInfoList);
    void                getContentInfoListByProducerId(T::ProducerId producerId,ContentInfoList& contentInfoList);
    void                getContentInfoListByProducerId(T::ProducerId producerId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,ContentInfoList& contentInfoList);
    void                getContentInfoListBySourceId(T::SourceId sourceId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,ContentInfoList& contentInfoList);

    std::size_t         getHash();
    std::size_t         getHashByProducerId(T::ProducerId producerId);
    uint                getLength()const;

    void                print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    uint                getFileStorageIndex(T::FileId fileId);
    uint                getGenerationStorageIndex(T::GenerationId generationId);
    uint                getProducerStorageIndex(T::ProducerId producerId);
    uint                getSourceStorageIndex(T::SourceId sourceId);


    ContentInfoList     mContentLists[MAX_CONTENT_SOURCES];
    ModificationLock    mModificationLock;

};



}
}
