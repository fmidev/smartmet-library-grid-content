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
    mLastModified = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





AliasFile::AliasFile(const std::string& filename)
{
  try
  {
    mFilename = filename;
    mDuplicatesAllowed = false;
    mLastModified = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





AliasFile::AliasFile(const std::string& filename,bool duplicatesAllowed)
{
  try
  {
    mFilename = filename;
    mDuplicatesAllowed = duplicatesAllowed;
    mLastModified = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





AliasFile::AliasFile(const AliasFile& aliasFile)
{
  try
  {
    mFilename = aliasFile.mFilename;
    mAliasList = aliasFile.mAliasList;
    mDuplicatesAllowed  = aliasFile.mDuplicatesAllowed;
    mLastModified = aliasFile.mLastModified;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




AliasFile::~AliasFile()
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





void AliasFile::init()
{
  try
  {
    if (mAliasList.size() > 0)
      return; // Already initialized

    AutoWriteLock lock(&mModificationLock);
    loadFile();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void AliasFile::init(const std::string& filename)
{
  try
  {
    mFilename = filename;
    init();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void AliasFile::init(const std::string& filename,bool duplicatesAllowed)
{
  try
  {
    mDuplicatesAllowed  = duplicatesAllowed;
    mFilename = filename;
    init();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool AliasFile::checkUpdates()
{
  try
  {
    time_t tt = getFileModificationTime(mFilename.c_str());
    if (tt == mLastModified  ||  (tt+3) >= time(nullptr))
      return false;

    AutoWriteLock lock(&mModificationLock);
    if (tt != mLastModified  &&  (tt+3) < time(nullptr))
    {
      loadFile();
      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




bool AliasFile::getAlias(const std::string& name,std::string& alias)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool AliasFile::replaceAlias(const std::string& name,std::string& alias)
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





void AliasFile::getAliasList(const std::string& name,std::vector<std::string>& aliasList)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void AliasFile::loadFile()
{
  try
  {
    FILE *file = fopen(mFilename.c_str(),"re");
    if (file == nullptr)
    {
      Fmi::Exception exception(BCP,"Cannot open the alias file!");
      exception.addParameter("Filename",mFilename);
      throw exception;
    }

    mAliasList.clear();
    mAliasVector.clear();

    char st[30000];

    uint lineCount = 0;
    while (!feof(file))
    {
      char *pp = st;
      bool ind = true;
      while (ind)
      {
        ind = false;
        if (fgets(pp,30000,file) != nullptr)
        {
          lineCount++;
          char *p = strstr(pp,"\n");
          if (p != nullptr  &&  p > pp)
          {
            p--;
            if (*p == '\\')
            {
              pp = p;
              ind = true;
            }
          }
        }
        else
        {
          pp[0] = '\n';
        }
      }

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
                //std::cout << "  " << CODE_LOCATION << "\n\n";
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
    fclose(file);

    mLastModified = getFileModificationTime(mFilename.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




}  // namespace QueryServer
}  // namespace SmartMet
