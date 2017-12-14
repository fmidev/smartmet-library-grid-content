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
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





AliasFile::AliasFile(std::string filename)
{
  try
  {
    mFilename = filename;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





AliasFile::AliasFile(const AliasFile& aliasFile)
{
  try
  {
    mFilename = aliasFile.mFilename;
    mAliasList = aliasFile.mAliasList;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





AliasFile::~AliasFile()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void AliasFile::checkUpdates()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    time_t tt = getFileModificationTime(mFilename.c_str());

    if (tt != mLastModified  &&  (tt+3) < time(0))
      loadFile();
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




bool AliasFile::getAlias(std::string& name,std::string& alias)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    auto a = mAliasList.find(toLowerString(name));
    if (a != mAliasList.end())
    {
      alias = a->second.mAliasString;
      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void AliasFile::loadFile()
{
  try
  {
    FILE *file = fopen(mFilename.c_str(),"r");
    if (file == NULL)
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
      if (fgets(st,1000,file) != NULL)
      {
        lineCount++;
        if (st[0] != '#')
        {
          char *p = strstr(st,"\n");
          if (p != NULL)
          {
            while (*p < ' '  &&  p >= st)
            {
              *p = '\0';
              p--;
            }
          }

          p = strstr(st,":");
          if (p != NULL)
          {
            *p = '\0';
            p++;
            Alias rec;

            rec.mName = st;
            rec.mAliasString = p;

            if (rec.mName.length() > 0  &&  rec.mAliasString.length() > 0)
            {
              auto alias = mAliasList.find(toLowerString(rec.mName));
              if (alias != mAliasList.end())
              {
                printf("### ALIAS '%s' ALREADY DEFINED (%s:%u)!\n",rec.mName.c_str(),mFilename.c_str(),lineCount);
                //rec.print(std::cout,0,0);
              }
              else
              {
                //rec.print(std::cout,0,0);
                mAliasList.insert(std::pair<std::string,Alias>(toLowerString(rec.mName),rec));
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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void AliasFile::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "AliasFileCollection\n";
    for (auto it = mAliasList.begin(); it != mAliasList.end(); ++it)
    {
      it->second.print(stream,level+1,optionFlags);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




}  // namespace QueryServer
}  // namespace SmartMet
