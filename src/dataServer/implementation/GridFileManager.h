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
    void                deleteFileById(uint fileId);
    void                deleteFileByIndex(std::size_t fileIndex);
    void                deleteFilesByProducerId(uint producerId);
    void                deleteFilesByGenerationId(uint generationId);
    void                deleteFilesBySourceId(uint sourceId);
    void                deleteFilesByAccessTime(time_t accessTime);
    void                deleteFilesByCheckTime(time_t checkTime);
    void                deleteFilesByDeletionTime(time_t deletionTime);
    GRID::GridFile_sptr getFileById(uint fileId);
    GRID::GridFile_sptr getFileByIdNoMapping(uint fileId);
    std::size_t         getFileCount();
    void                getFilesToBeCached(std::map<uint,std::string>& filenames);
    void                getFilesInCache(std::map<uint,std::string>& filenames);
    void                getRequestCounters(GRID::RequestCounters& requestCounters,bool diskFiles,bool networkFiles);
    void                getStateAttributes(std::shared_ptr<T::AttributeNode> parent);
    void                resetRequestCounters();
    void                print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    void                deleteFileNoLock(uint fileId,bool sentMessageToContentServer);
    void                deleteFilesNoLock(std::vector<uint>& fileIdList,bool sentMessageToContentServer);
    void                deleteFilesNoLock(std::set<uint>& fileIdList,bool sentMessageToContentServer);

  private:

    ModificationLock                             mModificationLock;
    std::unordered_map<uint,GRID::GridFile_sptr> mFileList;
    ContentServer::ServiceInterface*             mContentServer;
    T::SessionId                                 mServerSessionId;
};



}
}
