#pragma once

#include <grid-files/common/Typedefs.h>
#include <grid-files/common/AutoThreadLock.h>
#include "AliasFile.h"


namespace SmartMet
{
namespace QueryServer
{

// ====================================================================================
/*! \brief Manages a hot-reloading collection of AliasFile objects.
 *
 *  Aggregates multiple AliasFile instances and dispatches alias lookups across all
 *  of them in order.  checkUpdates() periodically re-reads changed alias files. */
// ====================================================================================

class AliasFileCollection
{
  public:
                  AliasFileCollection();
                  AliasFileCollection(string_vec& filenames);
                  //AliasFileCollection(const AliasFileCollection& aliasFileCollection);
    virtual       ~AliasFileCollection();

    virtual void  init();
    virtual void  init(string_vec& filenames);
    virtual void  init(string_vec& filenames,bool duplicatesAllowed);
    virtual bool  checkUpdates(bool force);
    virtual bool  getAlias(const std::string& name,std::string& alias);
    virtual void  getAliasList(const std::string& name,std::vector<std::string>& aliasList);
    virtual bool  replaceAlias(const std::string& name,std::string& alias);
    virtual void  print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    AliasFile_vec     mAliasFileList;      //!< Loaded AliasFile instances, one per filename.
    string_vec        mFilenames;          //!< Paths of the alias files managed by this collection.
    ModificationLock  mModificationLock;   //!< Lock protecting the file list during updates.
    time_t            mLastCheck;          //!< Timestamp of the last modification-check pass.
    uint              mCheckInterval;      //!< Minimum seconds between successive modification checks.
    bool              mDuplicatesAllowed;  //!< Propagated to each AliasFile at construction time.
};



}  // namespace QueryServer
}  // namespace SmartMet
