#include "ParameterMappingFile.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace QueryServer
{


ParameterMappingFile::ParameterMappingFile(std::string filename)
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





ParameterMappingFile::ParameterMappingFile(const ParameterMappingFile& mappingFile)
{
  try
  {
    mFilename = mappingFile.mFilename;
    mMappingSearch = mappingFile.mMappingSearch;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





ParameterMappingFile::~ParameterMappingFile()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ParameterMappingFile::init()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    loadFile();
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





bool ParameterMappingFile::checkUpdates()
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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





std::string ParameterMappingFile::getFilename()
{
  try
  {
    return mFilename;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





uint ParameterMappingFile::getNumberOfMappings()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint cnt = 0;
    for (auto it = mMappingSearch.begin(); it != mMappingSearch.end(); ++it)
    {
      cnt = cnt + (uint)it->second.size();
    }
    return cnt;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




/*
ParameterMapping* ParameterMappingFile::getParameterMappingByIndex(uint index)
{
  try
  {
    if (index < mMappingVector.size())
      return &mMappingVector[index];

    return NULL;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}
*/




ParameterMapping* ParameterMappingFile::getMapping(ParameterMapping& mapping)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    std::string key = toLowerString(mapping.mProducerName + ":" + mapping.mParameterName);

    auto s = mMappingSearch.find(key);
    if (s == mMappingSearch.end())
      return NULL;

    for (auto it = s->second.begin(); it != s->second.end(); ++it)
    {
      if (it->mProducerName == mapping.mProducerName  &&
        it->mParameterName == mapping.mParameterName  &&
        it->mParameterKeyType == mapping.mParameterKeyType &&
        it->mParameterKey == mapping.mParameterKey &&
        it->mParameterLevelIdType == mapping.mParameterLevelIdType &&
        it->mParameterLevelId == mapping.mParameterLevelId &&
        it->mParameterLevel == mapping.mParameterLevel)
      {
        return &(*it);
      }
    }
    return NULL;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ParameterMappingFile::getMappings(std::string producerName,std::string parameterName,bool onlySearchEnabled,ParameterMapping_vec& mappings)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    std::string key = toLowerString(producerName + ":" + parameterName);

    auto s = mMappingSearch.find(key);
    if (s == mMappingSearch.end())
      return;

    for (auto it = s->second.begin(); it != s->second.end(); ++it)
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
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ParameterMappingFile::getMappings(std::string producerName,std::string parameterName,T::ParamLevelIdType levelIdType,T::ParamLevelId levelId,T::ParamLevel level,bool onlySearchEnabled,ParameterMapping_vec& mappings)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    std::string key = toLowerString(producerName + ":" + parameterName);

    auto s = mMappingSearch.find(key);
    if (s == mMappingSearch.end())
      return;

    for (auto it = s->second.begin(); it != s->second.end(); ++it)
    {
      if (levelIdType == T::ParamLevelIdType::ANY || levelIdType == T::ParamLevelIdType::IGNORE ||  it->mParameterLevelIdType == levelIdType)
      {
        if (levelIdType == T::ParamLevelIdType::IGNORE || levelId == 0 || it->mParameterLevelId == levelId)
        {
          if (levelIdType == T::ParamLevelIdType::IGNORE || it->mParameterLevel == level)
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
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ParameterMappingFile::loadFile()
{
  try
  {
    FILE *file = fopen(mFilename.c_str(),"r");
    if (file == NULL)
    {
      Spine::Exception exception(BCP,"Cannot open the mapping file!");
      exception.addParameter("Filename",mFilename);
      throw exception;
    }

    mMappingSearch.clear();

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

        if (c > 12)
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
            rec.mAreaInterpolationMethod = (short)atoi(field[7]);

          if (field[8][0] != '\0')
            rec.mTimeInterpolationMethod = (short)atoi(field[8]);

          if (field[9][0] != '\0')
            rec.mLevelInterpolationMethod = (short)atoi(field[9]);

          if (field[10][0] != '\0')
            rec.mGroupFlags = atoi(field[10]);

          if (field[11][0] == 'E')
            rec.mSearchEnabled = true;
          else
            rec.mSearchEnabled = false;

          if (field[12][0] != '\0')
            rec.mConversionFunction = field[12];

          //rec.print(std::cout,0,0);

          std::string key = toLowerString(rec.mProducerName + ":" + rec.mParameterName);

          auto s = mMappingSearch.find(key);
          if (s != mMappingSearch.end())
          {
            s->second.push_back(rec);
          }
          else
          {
            ParameterMapping_vec vec;
            vec.push_back(rec);
            mMappingSearch.insert(std::pair<std::string,ParameterMapping_vec>(key,vec));
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





void ParameterMappingFile::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




}  // namespace QueryServer
}  // namespace SmartMet
