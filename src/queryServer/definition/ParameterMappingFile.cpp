#include "ParameterMappingFile.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace QueryServer
{


ParameterMappingFile::ParameterMappingFile(const std::string& filename)
{
  try
  {
    mFilename = filename;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
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
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





ParameterMappingFile::~ParameterMappingFile()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
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
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ParameterMappingFile::checkUpdates()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    time_t tt = getFileModificationTime(mFilename.c_str());

    if (tt != mLastModified  &&  (tt+3) < time(nullptr))
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





std::string ParameterMappingFile::getFilename()
{
  try
  {
    return mFilename;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
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
      cnt = cnt + it->second.size();
    }
    return cnt;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





ParameterMapping* ParameterMappingFile::getMapping(ParameterMapping& mapping)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    std::string key = toLowerString(mapping.mProducerName + ":" + mapping.mParameterName);

    auto s = mMappingSearch.find(key);
    if (s == mMappingSearch.end())
      return nullptr;

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
    return nullptr;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ParameterMappingFile::getMappings(const std::string& producerName,const std::string& parameterName,bool onlySearchEnabled,ParameterMapping_vec& mappings)
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
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ParameterMappingFile::getMappings(const std::string& producerName,const std::string& parameterName,T::ParamLevelIdType levelIdType,T::ParamLevelId levelId,T::ParamLevel level,bool onlySearchEnabled,ParameterMapping_vec& mappings)
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
      if (levelIdType == T::ParamLevelIdTypeValue::ANY || levelIdType == T::ParamLevelIdTypeValue::IGNORE ||  it->mParameterLevelIdType == levelIdType)
      {
        if (levelIdType == T::ParamLevelIdTypeValue::IGNORE || levelId == 0 || it->mParameterLevelId == levelId)
        {
          if (levelIdType == T::ParamLevelIdTypeValue::IGNORE || it->mParameterLevel == level)
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
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ParameterMappingFile::loadFile()
{
  try
  {
    FILE *file = fopen(mFilename.c_str(),"re");
    if (file == nullptr)
    {
      Spine::Exception exception(BCP,"Cannot open the mapping file!");
      exception.addParameter("Filename",mFilename);
      throw exception;
    }

    mMappingSearch.clear();

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st,1000,file) != nullptr  &&  st[0] != '#')
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
            rec.mParameterKeyType = toInt64(field[2]);

          if (field[3][0] != '\0')
            rec.mParameterKey = field[3];

          if (field[4][0] != '\0')
            rec.mParameterLevelIdType = toInt64(field[4]);

          if (field[5][0] != '\0')
            rec.mParameterLevelId = toInt64(field[5]);

          if (field[6][0] != '\0')
            rec.mParameterLevel = (T::ParamLevel)toInt64(field[6]);

          if (field[7][0] != '\0')
            rec.mAreaInterpolationMethod = (short)toInt64(field[7]);

          if (field[8][0] != '\0')
            rec.mTimeInterpolationMethod = (short)toInt64(field[8]);

          if (field[9][0] != '\0')
            rec.mLevelInterpolationMethod = (short)toInt64(field[9]);

          if (field[10][0] != '\0')
            rec.mGroupFlags = toInt64(field[10]);

          if (field[11][0] == 'E')
            rec.mSearchEnabled = true;
          else
            rec.mSearchEnabled = false;

          if (field[12][0] != '\0')
            rec.mConversionFunction = field[12];

          if (c > 13  &&  field[13][0] != '\0')
            rec.mReverseConversionFunction = field[13];

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
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ParameterMappingFile::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}




}  // namespace QueryServer
}  // namespace SmartMet
