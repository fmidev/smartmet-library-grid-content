#pragma once

#include "FileInfo.h"
#include "grid-files/common/ModificationLock.h"
#include <vector>


namespace SmartMet
{
namespace T
{


class FileInfoList
{
  public:
                FileInfoList();
                FileInfoList(FileInfoList& fileInfoList);
     virtual    ~FileInfoList();

     void       operator=(FileInfoList& fileInfoList);

     void       addFileInfo(FileInfo *fileInfo);
     void       clear();
     bool       deleteFileInfoById(uint fileId);
     uint       deleteFileInfoByGroupFlags(uint groupFlags);
     uint       deleteFileInfoByProducerId(uint producerId);
     uint       deleteFileInfoByGenerationId(uint generationId);
     bool       deleteFileInfoByIndex(uint index);
     uint       deleteFileInfoBySourceId(uint sourceId);
     int        getClosestIndex(FileInfo::ComparisonMethod comparisonMethod,FileInfo& fileInfo);
     int        getClosestIndexNoLock(FileInfo::ComparisonMethod comparisonMethod,FileInfo& fileInfo);
     FileInfo*  getFileInfoById(uint fileId);
     FileInfo*  getFileInfoByName(std::string filename);
     FileInfo*  getFileInfoByIndex(uint index);
     FileInfo*  getFileInfoByIndexNoCheck(uint index);
     void       getFileInfoList(uint startFileId,uint maxRecords,FileInfoList& fileInfoList);
     void       getFileInfoListByGroupFlags(uint groupFlags,uint startFileId,uint maxRecords,FileInfoList& fileInfoList);
     void       getFileInfoListByProducerId(uint producerId,uint startFileId,uint maxRecords,FileInfoList& fileInfoList);
     void       getFileInfoListByGenerationId(uint generationId,uint startFileId,uint maxRecords,FileInfoList& fileInfoList);
     void       getFileInfoListBySourceId(uint sourceId,uint startFileId,uint maxRecords,FileInfoList& fileInfoList);
     uint       getLength();
     bool       getReleaseObjects();
     void       increaseSize(uint newSize);
     void       setComparisonMethod(FileInfo::ComparisonMethod comparisonMethod);
     void       setReleaseObjects(bool releaseObjects);
     void       sort(FileInfo::ComparisonMethod comparisonMethod);
     void       lock();
     void       unlock();
     void       print(std::ostream& stream,uint level,uint optionFlags);

     void       writeToFile(std::string filename);
     void       writeToFile(std::string filename,const char *filemode);

  protected:

     FileInfoPtr                *mArray;
     uint                       mSize;
     uint                       mLength;
     bool                       mReleaseObjects;
     ModificationLock           mModificationLock;
     FileInfo::ComparisonMethod mComparisonMethod;
};



}
}
