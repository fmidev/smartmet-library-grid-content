#include "ParameterMappingList.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace QueryServer
{


ParameterMappingList::ParameterMappingList(std::string filename)
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





ParameterMappingList::ParameterMappingList(const ParameterMappingList& mappingList)
{
  try
  {
    mFilename = mappingList.mFilename;
    mMappingVector = mappingList.mMappingVector;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





ParameterMappingList::~ParameterMappingList()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ParameterMappingList::init()
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





void ParameterMappingList::checkUpdates()
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





void ParameterMappingList::getMappings(std::string producerName,std::string parameterName,bool onlySearchEnabled,ParameterMapping_vec& mappings)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    for (auto it = mMappingVector.begin(); it != mMappingVector.end(); ++it)
    {
      if (it->mProducerName == producerName  &&  it->mParameterName == parameterName)
      {
        if (onlySearchEnabled)
        {
          if (it->mSearchEnabled)
            mappings.push_back(*it);
        }
        else
        {
          mappings.push_back(*it);
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ParameterMappingList::load()
{
  try
  {
    FILE *file = fopen(mFilename.c_str(),"r");
    if (file == NULL)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot open the mapping file!");
      exception.addParameter("Filename",mFilename);
      throw exception;
    }

    mMappingVector.clear();

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st,1000,file) != NULL  &&  st[0] != '#')
      {
        bool ind = false;
        char *field[100];
        uint c = 1;
        field[0] = st;
        char *p = st;
        while (*p != '\0'  &&  c < 100)
        {
          if (*p == '"')
            ind = !ind;

          if ((*p == ';'  || *p == '\n') && !ind)
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

        if (c > 8)
        {
          ParameterMapping rec;

          if (field[0][0] != '\0')
            rec.mProducerName = field[0];

          if (field[1][0] != '\0')
            rec.mParameterName = field[1];

          if (field[2][0] != '\0')
            rec.mParameterKeyType = (T::ParamKeyType)atoi(field[2]);

          if (field[3][0] != '\0')
            rec.mParameterKey = field[3];

          if (field[4][0] != '\0')
            rec.mParameterLevelIdType = (T::ParamLevelIdType)atoi(field[4]);

          if (field[5][0] != '\0')
            rec.mParameterLevelId = (T::ParamLevelId)atoi(field[5]);

          if (field[6][0] != '\0')
            rec.mParameterLevel = (T::ParamLevel)atoi(field[6]);

          if (field[7][0] != '\0')
            rec.mInterpolationMethod = (T::InterpolationMethod)atoi(field[7]);

          if (field[8][0] == 'E')
            rec.mSearchEnabled = true;
          else
            rec.mSearchEnabled = false;

          //rec.print(std::cout,0,0);
          mMappingVector.push_back(rec);
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





void ParameterMappingList::print(std::ostream& stream,uint level,uint optionFlags)
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
