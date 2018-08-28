#include "AliasFile.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace QueryServer
{


AliasFile::AliasFile()
{
  try
  {
    mDuplicatesAllowed = false;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





AliasFile::AliasFile(std::string filename)
{
  try
  {
    mFilename = filename;
    mDuplicatesAllowed = false;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





AliasFile::AliasFile(const AliasFile& aliasFile)
{
  try
  {
    mFilename = aliasFile.mFilename;
    mAliasList = aliasFile.mAliasList;
    mDuplicatesAllowed  = aliasFile.mDuplicatesAllowed;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





AliasFile::~AliasFile()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void AliasFile::init()
{
  try
  {
    if (mAliasList.size() > 0)
      return; // Already initialized

    AutoThreadLock lock(&mThreadLock);
    loadFile();
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void AliasFile::init(std::string filename)
{
  try
  {
    mFilename = filename;
    init();
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void AliasFile::init(std::string filename,bool duplicatesAllowed)
{
  try
  {
    mDuplicatesAllowed  = duplicatesAllowed;
    mFilename = filename;
    init();
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool AliasFile::checkUpdates()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    time_t tt = getFileModificationTime(mFilename.c_str());

    if (tt != mLastModified  &&  (tt+3) < time(0))
    {
      loadFile();
      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}




bool AliasFile::getAlias(std::string& name,std::string& alias)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!mDuplicatesAllowed)
    {
      auto a = mAliasList.find(toLowerString(name));
      if (a != mAliasList.end())
      {
        alias = a->second.mAliasString;
        return true;
      }
      return false;
    }

    for (auto it = mAliasVector.begin(); it != mAliasVector.end(); ++it)
    {
      if (strcasecmp(it->mName.c_str(),name.c_str()) == 0)
      {
        alias = it->mAliasString;
        return true;
      }
    }
    return false;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void AliasFile::getAliasList(std::string& name,std::vector<std::string>& aliasList)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (!mDuplicatesAllowed)
    {
      auto a = mAliasList.find(toLowerString(name));
      if (a != mAliasList.end())
      {
        aliasList.push_back(a->second.mAliasString);
      }
      return;
    }

    for (auto it = mAliasVector.begin(); it != mAliasVector.end(); ++it)
    {
      if (strcasecmp(it->mName.c_str(),name.c_str()) == 0)
        aliasList.push_back(it->mAliasString);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void AliasFile::loadFile()
{
  try
  {
    FILE *file = fopen(mFilename.c_str(),"r");
    if (file == nullptr)
    {
      Spine::Exception exception(BCP,"Cannot open the alias file!");
      exception.addParameter("Filename",mFilename);
      throw exception;
    }

    mAliasList.clear();

    char st[1000];

    uint lineCount = 0;
    while (!feof(file))
    {
      if (fgets(st,1000,file) != nullptr)
      {
        lineCount++;
        if (st[0] != '#')
        {
          char *p = strstr(st,"\n");
          if (p != nullptr)
          {
            while (*p < ' '  &&  p >= st)
            {
              *p = '\0';
              p--;
            }
          }

          p = strstr(st,":");
          if (p != nullptr)
          {
            *p = '\0';
            p++;
            Alias rec;

            rec.mName = st;
            rec.mAliasString = p;

            if (rec.mName.length() > 0  &&  rec.mAliasString.length() > 0)
            {
              if (!mDuplicatesAllowed)
              {
                auto alias = mAliasList.find(toLowerString(rec.mName));
                if (alias != mAliasList.end())
                {
                  std::cout << "#### ALIAS '" << rec.mName << "' ALREADY DEFINED (" << mFilename << ":" << lineCount << ")\n";
                  std::cout << "  " << CODE_LOCATION << "\n\n";
                  //rec.print(std::cout,0,0);
                }
                else
                {
                  //rec.print(std::cout,0,0);
                  mAliasList.insert(std::pair<std::string,Alias>(toLowerString(rec.mName),rec));
                }
              }
              else
              {
                mAliasVector.push_back(rec);
              }
            }
          }
        }
      }
    }
    fclose(file);

    mLastModified = getFileModificationTime(mFilename.c_str());
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void AliasFile::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "AliasFileCollection\n";
    for (auto it = mAliasList.begin(); it != mAliasList.end(); ++it)
      it->second.print(stream,level+1,optionFlags);

    for (auto it = mAliasVector.begin(); it != mAliasVector.end(); ++it)
      it->print(stream,level+1,optionFlags);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}




}  // namespace QueryServer
}  // namespace SmartMet
