#pragma once

#include "FileInfo.h"
#include "ContentInfoList.h"



namespace SmartMet
{
namespace T
{

// ====================================================================================
/*! \brief Aggregate holding a FileInfo record together with all of its ContentInfo records.
 *
 *  FileAndContent is used when the caller needs both the file metadata and the
 *  associated content (GRIB message index) in a single transfer object, avoiding
 *  separate round-trips to the Content Server. */
// ====================================================================================

class FileAndContent
{
  public:

                        FileAndContent();
                        FileAndContent(const FileAndContent& fileAndContent);
     virtual            ~FileAndContent();

     FileAndContent*    duplicate();
     void               print(std::ostream& stream,uint level,uint optionFlags);


     FileInfo           mFileInfo;         //!< Metadata describing the grid file.
     ContentInfoList    mContentInfoList;  //!< All content records belonging to the file.

};


}
}
