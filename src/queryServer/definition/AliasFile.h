#pragma once

#include <grid-files/grid/Typedefs.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include "Alias.h"


namespace SmartMet
{
namespace QueryServer
{

// ====================================================================================
/*! \brief Loads and caches a file of parameter name alias definitions.
 *
 *  Reads alias name → expansion string pairs from a text file and provides
 *  getAlias() / replaceAlias() lookup.  checkUpdates() hot-reloads the file when
 *  its mtime changes. */
// ====================================================================================

class AliasFile
{
  public:
                  AliasFile();
                  AliasFile(const std::string& filename);
                  AliasFile(const std::string& filename,bool duplicatesAllowed);
                  //AliasFile(const AliasFile& aliasFile);
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

    std::string       mFilename;          //!< Path to the alias definition file on disk.
    time_t            mLastModified;      //!< File mtime at last load, used for change detection.
    Alias_map         mAliasList;         //!< Map for O(1) unique-key alias lookups.
    Alias_vec         mAliasVector;       //!< Ordered list used when duplicates are allowed.
    ModificationLock  mModificationLock;  //!< Lock protecting the alias data structures.
    bool              mDuplicatesAllowed; //!< When true, multiple aliases with the same name are kept.
};



typedef std::vector<AliasFile> AliasFile_vec;  //!< Ordered collection of AliasFile objects.


}  // namespace QueryServer
}  // namespace SmartMet
