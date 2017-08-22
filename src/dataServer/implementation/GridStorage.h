#pragma once

#include "grid-files/grid/GridFile.h"
#include "grid-files/common/ModificationLock.h"
#include <vector>
#include <map>

namespace SmartMet
{
namespace DataServer
{


class GridStorage
{
  public:
                        GridStorage();
    virtual             ~GridStorage();

    void                addFile(GRID::GridFile *gribFile);
    void                clear();
    void                deleteFile(GRID::GridFile *gribFile);
    void                deleteFileById(uint fileId);
    void                deleteFileByIndex(std::size_t fileIndex);
    void                deleteFilesByGroupFlags(uint groupFlags);
    void                deleteFilesByProducerId(uint producerId);
    void                deleteFilesByGenerationId(uint generationId);
    void                deleteFilesBySourceId(uint sourceId);
    void                deleteFilesByCheckTime(time_t checkTime);
    GRID::GridFile_sptr getFileById(uint fileId);
    GRID::GridFile_sptr getFileByIdNoMapping(uint fileId);
    std::size_t         getFileCount();
    void                print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    void                deleteFilesNoLock(std::vector<uint>& fileIdList);


    ModificationLock    mModificationLock;

    std::map<uint,GRID::GridFile_sptr> mFileList;
};



}
}
