#pragma once

#include "FileInfo.h"
#include "ContentInfoList.h"



namespace SmartMet
{
namespace T
{


class FileAndContent
{
  public:

                        FileAndContent();
                        FileAndContent(const FileAndContent& fileAndContent);
     virtual            ~FileAndContent();

     FileAndContent*    duplicate();
     void               print(std::ostream& stream,uint level,uint optionFlags);


     FileInfo           mFileInfo;
     ContentInfoList    mContentInfoList;

};


}
}
