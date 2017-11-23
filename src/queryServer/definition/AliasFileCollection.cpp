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
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





AliasFileCollection::AliasFileCollection(string_vec& filenames)
{
  try
  {
    mFilenames = filenames;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





AliasFileCollection::AliasFileCollection(const AliasFileCollection& aliasFileCollection)
{
  try
  {
    mFilenames = aliasFileCollection.mFilenames;
    mAliasFileList = aliasFileCollection.mAliasFileList;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





AliasFileCollection::~AliasFileCollection()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void AliasFileCollection::init()
{
  try
  {
    if (mAliasFileList.size() > 0)
      return; // Already initialized

    AutoThreadLock lock(&mThreadLock);

    for (auto it = mFilenames.begin(); it != mFilenames.end(); ++it)
    {
      mAliasFileList.push_back(AliasFile(*it));
    }

    for (auto it = mAliasFileList.begin(); it != mAliasFileList.end(); ++it)
    {
      it->init();
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void AliasFileCollection::checkUpdates(bool force)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    time_t tt = time(0);
    if (force ||  (tt-mLastCheck) > mCheckInterval)
    {
      for (auto it = mAliasFileList.begin(); it != mAliasFileList.end(); ++it)
      {
        it->checkUpdates();
      }
      mLastCheck = tt;
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





bool AliasFileCollection::getAlias(std::string& name,std::string& alias)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    for (auto it = mAliasFileList.begin(); it != mAliasFileList.end(); ++it)
    {
      if (it->getAlias(name,alias))
        return true;
    }
    return false;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void AliasFileCollection::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "AliasFileCollection\n";
    for (auto it = mAliasFileList.begin(); it != mAliasFileList.end(); ++it)
    {
      it->print(stream,level+1,optionFlags);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




}  // namespace QueryServer
}  // namespace SmartMet
