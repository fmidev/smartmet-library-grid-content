#pragma once

#include "FileInfo.h"

#include <grid-files/common/ModificationLock.h>
#include <vector>
#include <set>


namespace SmartMet
{
namespace T
{

// ====================================================================================
/*! \brief Sorted, indexed container of FileInfo pointers with thread-safe access.
 *
 *  FileInfoList stores file metadata records in a heap-allocated array and keeps them
 *  sorted so that binary-search lookups by file id or file name are efficient. It
 *  supports partitioned queries by producer, generation, and source id. */
// ====================================================================================

class FileInfoList
{
  public:
                   FileInfoList();
                   FileInfoList(FileInfoList& fileInfoList);
    virtual        ~FileInfoList();

    FileInfoList&  operator=(FileInfoList& fileInfoList);

    void           addFileInfo(FileInfo *fileInfo);
    void           addFileInfoList(FileInfoList& fileInfoList);
    void           addFileInfoListNoLock(FileInfoList& fileInfoList);
    void           clear();
    bool           deleteFileInfoById(T::FileId fileId);
    uint           deleteFileInfoByProducerId(T::ProducerId producerId);
    uint           deleteFileInfoByGenerationId(T::GenerationId generationId);
    uint           deleteFileInfoByGenerationIdList(std::set<T::GenerationId>& generationIdList);
    bool           deleteFileInfoByIndex(uint index);
    bool           deleteFileInfoByName(const std::string& filename);
    uint           deleteFileInfoBySourceId(T::SourceId sourceId);
    uint           deleteFileInfoByStorageId(T::StorageId storageId);
    uint           deleteFileInfoByFileIdList(std::set<T::FileId>& fileIdList);
    uint           deleteMarkedFiles();
    int            getClosestIndex(uint comparisonMethod,FileInfo& fileInfo);
    int            getClosestIndexNoLock(uint comparisonMethod,FileInfo& fileInfo);
    FileInfo*      getFileInfoById(T::FileId fileId);
    bool           getFileInfoById(T::FileId fileId,FileInfo& fileInfo);
    FileInfo*      getFileInfoByIdNoLock(T::FileId fileId);
    FileInfo*      getFileInfoByName(const std::string& filename);
    bool           getFileInfoByName(const std::string& filename,FileInfo& fileInfo);
    FileInfo*      getFileInfoByIndex(uint index);
    FileInfo*      getFileInfoByIndexNoCheck(uint index);
    uint           getFileInfoCountByProducerId(T::ProducerId producerId);
    uint           getFileInfoCountByGenerationId(T::GenerationId generationId);
    uint           getFileInfoCountBySourceId(T::SourceId sourceId);
    void           getFileInfoList(FileInfoList& fileInfoList);
    void           getFileInfoListNoClear(FileInfoList& fileInfoList);
    void           getFileInfoList(T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList);
    void           getFileInfoListNoClear(T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList);
    void           getFileInfoListByProducerId(T::ProducerId producerId,FileInfoList& fileInfoList);
    void           getFileInfoListByProducerId(T::ProducerId producerId,T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList);
    void           getFileInfoListByGenerationId(T::GenerationId generationId,FileInfoList& fileInfoList);
    void           getFileInfoListByGenerationId(T::GenerationId generationId,T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList);
    void           getFileInfoListBySourceId(T::SourceId sourceId,FileInfoList& fileInfoList);
    void           getFileInfoListBySourceId(T::SourceId sourceId,T::FileId startFileId,int maxRecords,FileInfoList& fileInfoList);
    std::size_t    getHash();
    std::size_t    getHashByProducerId(T::ProducerId producerId);
    std::size_t    getHashByGenerationId(T::GenerationId generationId);
    time_t         getLastFileDeletionTimeByGenerationId(T::GenerationId generationId);
    uint           getLength();
    bool           getReleaseObjects();
    void           increaseSize(uint newSize);
    void           increaseSizeNoLock(uint newSize);
    uint           markDeleted();
    uint           markDeletedById(T::FileId fileId);
    uint           markDeletedByProducerId(T::ProducerId producerId);
    uint           markDeletedByGenerationId(T::GenerationId generationId);
    uint           markDeletedBySourceId(T::SourceId sourceId);
    uint           markDeletedByStorageId(T::StorageId storageId);
    void           setLockingEnabled(bool lockingEnabled);
    void           setComparisonMethod(uint comparisonMethod);
    void           setReleaseObjects(bool releaseObjects);
    void           sort(uint comparisonMethod);
    void           lock();
    void           unlock();
    void           print(std::ostream& stream,uint level,uint optionFlags);

    void           writeToFile(const std::string& filename);
    void           writeToFile(const std::string& filename,const char *filemode);

    ModificationLock*  getModificationLockPtr();
    void               setModificationLockPtr(ModificationLock* modificationLockPtr);

  protected:

    FileInfoPtr        *mArray;             //!< Heap-allocated array of FileInfo pointers.
    uint               mSize;              //!< Allocated capacity of mArray.
    uint               mLength;            //!< Number of valid entries currently stored.
    bool               mReleaseObjects;    //!< If true, destructor deletes the pointed-to FileInfo objects.
    ModificationLock   mModificationLock;  //!< Owned modification lock used when no external lock is set.
    ModificationLock*  mModificationLockPtr; //!< Pointer to the active modification lock (may be external).
    uint               mComparisonMethod;  //!< Current sort order (one of FileInfo::ComparisonMethod constants).
};



}
}
