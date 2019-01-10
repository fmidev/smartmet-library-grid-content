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
                  AliasFile();
                  AliasFile(const std::string& filename);
                  AliasFile(const AliasFile& aliasFile);
    virtual       ~AliasFile();

    virtual void  init();
    virtual void  init(const std::string& filename);
    virtual void  init(const std::string& filename,bool duplicatesAllowed);
    virtual bool  checkUpdates();
    virtual bool  getAlias(const std::string& name,std::string& alias);
    virtual bool  replaceAlias(const std::string& name,std::string& alias);
    virtual void  getAliasList(const std::string& name,std::vector<std::string>& aliasList);
    virtual void  print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    virtual void  loadFile();

    std::string   mFilename;
    time_t        mLastModified;
    Alias_map     mAliasList;
    Alias_vec     mAliasVector;
    ThreadLock    mThreadLock;
    bool          mDuplicatesAllowed;
};



typedef std::vector<AliasFile> AliasFile_vec;


}  // namespace QueryServer
}  // namespace SmartMet
