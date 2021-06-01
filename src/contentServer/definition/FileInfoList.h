#pragma once

#include "FileInfo.h"

#include <grid-files/common/ModificationLock.h>
#include <vector>
#include <set>


namespace SmartMet
{
namespace T
{


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
    bool           deleteFileInfoById(uint fileId);
    uint           deleteFileInfoByProducerId(uint producerId);
    uint           deleteFileInfoByGenerationId(uint generationId);
    uint           deleteFileInfoByGenerationIdList(std::set<uint>& generationIdList);
    bool           deleteFileInfoByIndex(uint index);
    bool           deleteFileInfoByName(const std::string& filename);
    uint           deleteFileInfoBySourceId(uint sourceId);
    uint           deleteFileInfoByFileIdList(std::set<uint>& fileIdList);
    uint           deleteVirtualFiles();
    uint           deleteMarkedFiles();
    int            getClosestIndex(uint comparisonMethod,FileInfo& fileInfo);
    int            getClosestIndexNoLock(uint comparisonMethod,FileInfo& fileInfo);
    FileInfo*      getFileInfoById(uint fileId);
    bool           getFileInfoById(uint fileId,FileInfo& fileInfo);
    FileInfo*      getFileInfoByIdNoLock(uint fileId);
    FileInfo*      getFileInfoByName(const std::string& filename);
    bool           getFileInfoByName(const std::string& filename,FileInfo& fileInfo);
    FileInfo*      getFileInfoByIndex(uint index);
    FileInfo*      getFileInfoByIndexNoCheck(uint index);
    uint           getFileInfoCountByProducerId(uint producerId);
    uint           getFileInfoCountByGenerationId(uint generationId);
    uint           getFileInfoCountBySourceId(uint sourceId);
    void           getFileInfoList(uint startFileId,uint maxRecords,FileInfoList& fileInfoList);
    void           getFileInfoListByProducerId(uint producerId,FileInfoList& fileInfoList);
    void           getFileInfoListByProducerId(uint producerId,uint startFileId,uint maxRecords,FileInfoList& fileInfoList);
    void           getFileInfoListByGenerationId(uint generationId,FileInfoList& fileInfoList);
    void           getFileInfoListByGenerationId(uint generationId,uint startFileId,uint maxRecords,FileInfoList& fileInfoList);
    void           getFileInfoListBySourceId(uint sourceId,FileInfoList& fileInfoList);
    void           getFileInfoListBySourceId(uint sourceId,uint startFileId,uint maxRecords,FileInfoList& fileInfoList);
    std::size_t    getHash();
    std::size_t    getHashByProducerId(uint producerId);
    time_t         getLastFileDeletionTimeByGenerationId(uint generationId);
    uint           getLength();
    bool           getReleaseObjects();
    void           increaseSize(uint newSize);
    void           increaseSizeNoLock(uint newSize);
    void           markFileInfoDeletedById(uint fileId);
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

    FileInfoPtr        *mArray;
    uint               mSize;
    uint               mLength;
    bool               mReleaseObjects;
    ModificationLock   mModificationLock;
    ModificationLock*  mModificationLockPtr;
    uint               mComparisonMethod;
};



}
}
