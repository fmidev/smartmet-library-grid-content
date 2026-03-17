#pragma once

#include "../../contentServer/definition/ServiceInterface.h"

#include <grid-files/grid/GridFile.h>
#include <grid-files/common/ModificationLock.h>
#include <grid-files/common/AttributeNode.h>
#include <vector>
#include <unordered_map>

namespace SmartMet
{
namespace DataServer
{


class GridFileManager
{
  public:
                        GridFileManager();
    virtual             ~GridFileManager();

    void                init(T::SessionId serverSessionId,ContentServer::ServiceInterface* contentServer);

    void                addFile(GRID::GridFile *gridFile);
    void                clear();
    void                deleteFile(GRID::GridFile *gridFile);
    void                deleteFileById(T::FileId fileId);
    void                deleteFileByIndex(std::size_t fileIndex);
    void                deleteFilesByProducerId(T::ProducerId producerId);
    void                deleteFilesByGenerationId(T::GenerationId generationId);
    void                deleteFilesBySourceId(T::SourceId sourceId);
    void                deleteFilesByStorageId(T::StorageId storageId);
    void                deleteFilesByAccessTime(time_t accessTime);
    void                deleteFilesByCheckTime(time_t checkTime);
    void                deleteFilesByDeletionTime(time_t deletionTime);
    GRID::GridFile_sptr getFileById(T::FileId fileId);
    GRID::GridFile_sptr getFileByIdNoMapping(T::FileId fileId);
    std::size_t         getFileCount();
    void                getFilesToBeCached(std::map<T::FileId,std::string>& filenames);
    void                getFilesInCache(std::map<T::FileId,std::string>& filenames);
    void                getRequestCounters(GRID::RequestCounters& requestCounters,bool diskFiles,bool networkFiles);
    void                getStateAttributes(std::shared_ptr<T::AttributeNode> parent);
    void                resetRequestCounters();
    void                print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    void                deleteFileNoLock(T::FileId fileId,bool sentMessageToContentServer);
    void                deleteFilesNoLock(std::vector<T::FileId>& fileIdList,bool sentMessageToContentServer);
    void                deleteFilesNoLock(std::set<T::FileId>& fileIdList,bool sentMessageToContentServer);

  private:

    ModificationLock                                  mModificationLock;
    std::unordered_map<T::FileId,GRID::GridFile_sptr> mFileList;
    ContentServer::ServiceInterface*                  mContentServer;
    T::SessionId                                      mServerSessionId;
};



}
}
