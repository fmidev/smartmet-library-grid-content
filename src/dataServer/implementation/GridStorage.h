#pragma once

#include "grid-files/grid/GridFile.h"
#include "grid-files/common/ModificationLock.h"
#include <vector>

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
    GRID::GridFile_sptr getFileById(uint fileId);
    GRID::GridFile_sptr getFileByIdNoMapping(uint fileId);
    GRID::GridFile_sptr getFileByIndex(std::size_t fileIndex);
    std::size_t         getFileCount();
    int                 getClosestFileIndexById(uint fileId);

    void                print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    int                 getClosestFileIndexByIdNoLock(uint fileId);

    ModificationLock    mModificationLock;

    std::vector<GRID::GridFile_sptr> mFileList;
};



}
}
