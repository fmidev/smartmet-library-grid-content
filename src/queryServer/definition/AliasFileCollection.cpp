#include "AliasFileCollection.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace QueryServer
{


AliasFileCollection::AliasFileCollection()
{
  try
  {
    mLastCheck = 0;
    mCheckInterval = 10;
    mDuplicatesAllowed = false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





AliasFileCollection::AliasFileCollection(string_vec& filenames)
{
  try
  {
    mFilenames = filenames;
    mDuplicatesAllowed = false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*
AliasFileCollection::AliasFileCollection(const AliasFileCollection& aliasFileCollection)
{
  try
  {
    mFilenames = aliasFileCollection.mFilenames;
    mAliasFileList = aliasFileCollection.mAliasFileList;
    mDuplicatesAllowed = aliasFileCollection.mDuplicatesAllowed;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}
*/




AliasFileCollection::~AliasFileCollection()
{
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void AliasFileCollection::init()
{
  try
  {
    if (mAliasFileList.size() > 0)
      return; // Already initialized

    AutoWriteLock lock(&mModificationLock);

    for (auto it = mFilenames.begin(); it != mFilenames.end(); ++it)
    {
      mAliasFileList.push_back(AliasFile(*it,mDuplicatesAllowed));
    }

    for (auto it = mAliasFileList.begin(); it != mAliasFileList.end(); ++it)
    {
      it->init();
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void AliasFileCollection::init(string_vec& filenames)
{
  try
  {
    mFilenames = filenames;
    init();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void AliasFileCollection::init(string_vec& filenames,bool duplicatesAllowed)
{
  try
  {
    mFilenames = filenames;
    mDuplicatesAllowed = duplicatesAllowed;
    init();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool AliasFileCollection::checkUpdates(bool force)
{
  try
  {
    bool result = false;
    AutoReadLock lock(&mModificationLock);

    time_t tt = time(nullptr);
    if (force ||  (tt-mLastCheck) > mCheckInterval)
    {
      for (auto it = mAliasFileList.begin(); it != mAliasFileList.end(); ++it)
      {
        if (it->checkUpdates())
          result = true;
      }
      mLastCheck = tt;
    }
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool AliasFileCollection::getAlias(const std::string& name,std::string& alias)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    for (auto it = mAliasFileList.begin(); it != mAliasFileList.end(); ++it)
    {
      if (it->getAlias(name,alias))
        return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void AliasFileCollection::getAliasList(const std::string& name,std::vector<std::string>& aliasList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    for (auto it = mAliasFileList.begin(); it != mAliasFileList.end(); ++it)
    {
      it->getAliasList(name,aliasList);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool AliasFileCollection::replaceAlias(const std::string& name,std::string& alias)
{
  try
  {
    auto p1 = name.find("$(");
    if (p1 == std::string::npos)
      return false;

    auto p2 = name.find(")",p1);
    if (p2 == std::string::npos)
      return false;

    std::string n = name.substr(p1+2,p2-p1-2);
    std::string a;
    if (getAlias(n,a))
    {
      alias = name;
      alias.replace(p1,p2-p1+1,a);

      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void AliasFileCollection::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    stream << space(level) << "AliasFileCollection\n";
    for (auto it = mAliasFileList.begin(); it != mAliasFileList.end(); ++it)
    {
      it->print(stream,level+1,optionFlags);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




}  // namespace QueryServer
}  // namespace SmartMet
