#pragma once

#include <grid-files/common/Typedefs.h>
#include <grid-files/common/AutoThreadLock.h>
#include "AliasFile.h"


namespace SmartMet
{
namespace QueryServer
{


class AliasFileCollection
{
  public:
                    AliasFileCollection();
                    AliasFileCollection(string_vec& filenames);
                    AliasFileCollection(const AliasFileCollection& aliasFileCollection);
    virtual         ~AliasFileCollection();

    virtual void    init();
    virtual void    init(string_vec& filenames);
    virtual void    checkUpdates();
    virtual bool    getAlias(std::string& name,std::string& alias);
    virtual void    print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    AliasFile_vec   mAliasFileList;
    string_vec      mFilenames;
    ThreadLock      mThreadLock;
};



}  // namespace QueryServer
}  // namespace SmartMet
