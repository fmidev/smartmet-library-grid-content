#pragma once

#include "FileInfoList.h"
#include "ContentListStorage.h"

#include <grid-files/common/ModificationLock.h>
#include <vector>
#include <set>


namespace SmartMet
{
namespace T
{


class FileListStorage
{
  public:
                   FileListStorage();
                   FileListStorage(FileListStorage& fileListStorage);
    virtual        ~FileListStorage();

    void           addFileInfo(FileInfo *fileInfo);
    void           clear();
    bool           deleteFileInfoById(T::FileId fileId);
    uint           deleteFileInfoByProducerId(T::ProducerId producerId);
    uint           deleteFileInfoByGenerationId(T::GenerationId generationId);
    bool           deleteFileInfoByIndex(uint index);
    bool           deleteFileInfoByName(const std::string& filename);
    uint           deleteFileInfoBySourceId(T::SourceId sourceId);
    uint           deleteFileInfoByStorageId(T::StorageId storageId);
    uint           deleteMarkedFiles();
    FileInfo*      getFileInfoById(T::FileId fileId);
    bool           getFileInfoById(T::FileId fileId,FileInfo& fileInfo);
    FileInfo*      getFileInfoByName(const std::string& filename);
    bool           getFileInfoByName(const std::string& filename,FileInfo& fileInfo);
    FileInfo*      getFileInfoByIndex(uint index);
    uint           getFileInfoCountByProducerId(T::ProducerId producerId);
    uint           getFileInfoCountByGenerationId(T::GenerationId generationId);
    uint           getFileInfoCountBySourceId(T::SourceId sourceId);
    void           getFileInfoList(FileInfoList& fileInfoList);
    void           getFileInfoList(T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList);
    void           getFileInfoListByProducerId(T::ProducerId producerId,FileInfoList& fileInfoList);
    void           getFileInfoListByProducerId(T::ProducerId producerId,T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList);
    void           getFileInfoListByGenerationId(T::GenerationId generationId,FileInfoList& fileInfoList);
    void           getFileInfoListByGenerationId(T::GenerationId generationId,T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList);
    void           getFileInfoListBySourceId(T::SourceId sourceId,FileInfoList& fileInfoList);
    void           getFileInfoListBySourceId(T::SourceId sourceId,T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList);
    std::size_t    getHash();
    std::size_t    getHashByProducerId(T::ProducerId producerId);
    std::size_t    getHashByGenerationId(T::GenerationId generationId);
    uint           getLength();
    uint           markDeleted();
    uint           markDeletedById(T::FileId fileId);
    uint           markDeletedByProducerId(T::ProducerId producerId);
    uint           markDeletedByGenerationId(T::GenerationId generationId);
    uint           markDeletedBySourceId(T::SourceId sourceId);
    uint           markDeletedByStorageId(T::StorageId storageId);

    void           print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    uint           getFileStorageIndex(T::FileId fileId);
    uint           getGenerationStorageIndex(T::GenerationId generationId);
    uint           getProducerStorageIndex(T::ProducerId producerId);
    uint           getSourceStorageIndex(T::SourceId sourceId);

    FileInfoList      mFileLists[MAX_CONTENT_SOURCES];
    ModificationLock  mModificationLock;
};



}
}
