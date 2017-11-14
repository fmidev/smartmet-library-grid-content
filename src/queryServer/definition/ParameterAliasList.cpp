#include "ParameterAliasList.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace QueryServer
{


ParameterAliasList::ParameterAliasList(std::string filename)
{
  try
  {
    mFilename = filename;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





ParameterAliasList::ParameterAliasList(const ParameterAliasList& aliasList)
{
  try
  {
    mFilename = aliasList.mFilename;
    mAliasList = aliasList.mAliasList;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





ParameterAliasList::~ParameterAliasList()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ParameterAliasList::init()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    load();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ParameterAliasList::checkUpdates()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    time_t tt = getFileModificationTime(mFilename.c_str());

    if (tt != mLastModified  &&  (tt-5) > time(0))
      load();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




bool ParameterAliasList::getAlias(std::string name,std::string& alias)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    auto a = mAliasList.find(toLowerString(name));
    if (a != mAliasList.end())
    {
      alias = a->second.mParameterString;
      return true;
    }
    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ParameterAliasList::load()
{
  try
  {
    FILE *file = fopen(mFilename.c_str(),"r");
    if (file == NULL)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot open the alias file!");
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
          int ind = 0;
          char *field[100];
          uint c = 1;
          field[0] = st;
          char *p = st;
          while (*p != '\0'  &&  c < 100)
          {
            if (*p == '{')
              ind++;

            if (*p == '}')
              ind--;

            if ((*p == ';'  || *p == '\n')  &&  ind == 0)
            {
              *p = '\0';
              p++;
              field[c] = p;
              c++;
            }
            else
            {
              p++;
            }
          }

          if (c > 1)
          {
            ParameterAlias rec;

            if (field[0][0] != '\0')
              rec.mName = field[0];

            if (field[1][0] != '\0')
              rec.mParameterString = field[1];

            if (rec.mName.length() > 0  &&  rec.mParameterString.length() > 0)
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
                mAliasList.insert(std::pair<std::string,ParameterAlias>(toLowerString(rec.mName),rec));
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
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ParameterAliasList::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




}  // namespace QueryServer
}  // namespace SmartMet
