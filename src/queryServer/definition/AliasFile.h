#pragma once

#include <grid-files/grid/Typedefs.h>
#include <grid-files/common/AutoThreadLock.h>
#include "Alias.h"


namespace SmartMet
{
namespace QueryServer
{


class AliasFile
{
  public:
                  AliasFile(std::string filename);
                  AliasFile(const AliasFile& aliasList);
    virtual       ~AliasFile();

    virtual void  init();
    virtual void  checkUpdates();
    virtual bool  getAlias(std::string& name,std::string& alias);
    virtual void  print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    virtual void  loadFile();

    std::string   mFilename;
    time_t        mLastModified;
    Alias_map     mAliasList;
    ThreadLock    mThreadLock;
};



typedef std::vector<AliasFile> AliasFile_vec;


}  // namespace QueryServer
}  // namespace SmartMet
