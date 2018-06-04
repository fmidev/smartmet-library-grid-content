#pragma once

#include "../../contentServer/definition/ServiceInterface.h"

#include <grid-files/grid/GridFile.h>
#include <grid-files/common/ModificationLock.h>
#include <vector>
#include <map>

namespace SmartMet
{
namespace DataServer
{


class GridFileManager
{
  public:
                        GridFileManager();
    virtual             ~GridFileManager();

    void                init(ContentServer::ServiceInterface* contentServer);

    void                addFile(GRID::GridFile *gridFile);
    void                addFileUser(uint fileId,uint userFileId);
    void                clear();
    void                deleteFile(GRID::GridFile *gridFile);
    void                deleteFileById(uint fileId);
    void                deleteFileByIndex(std::size_t fileIndex);
    void                deleteFilesByGroupFlags(uint groupFlags);
    void                deleteFilesByProducerId(uint producerId);
    void                deleteFilesByGenerationId(uint generationId);
    void                deleteFilesBySourceId(uint sourceId);
    void                deleteFilesByCheckTime(time_t checkTime);
    void                deleteVirtualFiles();
    GRID::GridFile_sptr getFileById(uint fileId);
    GRID::GridFile_sptr getFileByIdNoMapping(uint fileId);
    std::size_t         getFileCount();
    void                print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    void                deleteFileNoLock(uint fileId,bool sentMessageToContentServer);
    void                deleteFilesNoLock(std::vector<uint>& fileIdList,bool sentMessageToContentServer);
    void                deleteFilesNoLock(std::set<uint>& fileIdList,bool sentMessageToContentServer);

  private:

    ModificationLock                   mModificationLock;
    std::map<uint,GRID::GridFile_sptr> mFileList;
    ContentServer::ServiceInterface*   mContentServer;
};



}
}
