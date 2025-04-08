#include "ParameterMappingFile.h"
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoThreadLock.h>
#include <macgyver/StringConversion.h>


namespace SmartMet
{
namespace QueryServer
{


ParameterMappingFile::ParameterMappingFile()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




ParameterMappingFile::ParameterMappingFile(const std::string& filename)
{
  try
  {
    mFilename = filename;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




ParameterMappingFile::~ParameterMappingFile()
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





void ParameterMappingFile::init()
{
  try
  {
    loadFile();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool ParameterMappingFile::checkUpdates()
{
  try
  {
    time_t tt = getFileModificationTime(mFilename.c_str());
    long fs = getFileSize(mFilename.c_str());

    if (tt == mLastModified  ||  (tt+10) >= time(nullptr) || fs < 10)
      return false;

    if (tt != mLastModified  &&  (tt+10) < time(nullptr))
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





std::string ParameterMappingFile::getFilename()
{
  try
  {
    return mFilename;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





uint ParameterMappingFile::getNumberOfMappings()
{
  try
  {
    if (!mMappingSearch)
      return 0;

    AutoReadLock lock(&mModificationLock);
    uint cnt = 0;
    for (auto it = mMappingSearch->begin(); it != mMappingSearch->end(); ++it)
    {
      cnt = cnt + it->second.size();
    }
    return cnt;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





ParameterMapping* ParameterMappingFile::getMapping(ParameterMapping& mapping)
{
  try
  {
    if (!mMappingSearch)
      return nullptr;

    AutoReadLock lock(&mModificationLock);
    std::string key = toLowerString(mapping.mProducerName + ":" + mapping.mParameterName + ":" + Fmi::to_string(mapping.mGeometryId));

    auto s = mMappingSearch->find(key);
    if (s == mMappingSearch->end())
      return nullptr;

    for (auto it = s->second.begin(); it != s->second.end(); ++it)
    {
      if (it->mProducerName == mapping.mProducerName  &&
        it->mParameterName == mapping.mParameterName  &&
        it->mGeometryId == mapping.mGeometryId &&
        it->mParameterKeyType == mapping.mParameterKeyType &&
        it->mParameterKey == mapping.mParameterKey &&
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ParameterMappingFile::getMappings(const std::string& producerName,const std::string& parameterName,T::GeometryId geometryId,bool onlySearchEnabled,ParameterMapping_vec& mappings)
{
  try
  {
    if (!mMappingSearch)
      return;

    AutoReadLock lock(&mModificationLock);
    std::string key = toLowerString(producerName + ":" + parameterName + ":" + Fmi::to_string(geometryId));

    auto s = mMappingSearch->find(key);
    if (s == mMappingSearch->end())
      return;

    for (auto it = s->second.begin(); it != s->second.end(); ++it)
    {
      if (!it->mIgnore  &&  (geometryId < 0 ||  it->mGeometryId == geometryId))
      {
        if (onlySearchEnabled)
        {
          if (it->mSearchEnabled)
            mappings.emplace_back(*it);
        }
        else
        {
          mappings.emplace_back(*it);
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ParameterMappingFile::getMappings(const std::string& producerName,const std::string& parameterName,bool onlySearchEnabled,ParameterMapping_vec& mappings)
{
  try
  {
    if (!mMappingSearch)
      return;

    AutoReadLock lock(&mModificationLock);

    std::string key = toLowerString(producerName + ":" + parameterName + ":");
    int len = strlen(key.c_str());

    auto s = mMappingSearch->lower_bound(key);
    if (s == mMappingSearch->end())
      return;

    while (s != mMappingSearch->end()  &&  strncasecmp(s->first.c_str(),key.c_str(),len) == 0)
    {
      for (auto it = s->second.begin(); it != s->second.end(); ++it)
      {
        if (!it->mIgnore)
        {
          if (onlySearchEnabled)
          {
            if (it->mSearchEnabled)
              mappings.emplace_back(*it);
          }
          else
          {
            mappings.emplace_back(*it);
          }
        }
      }
      s++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ParameterMappingFile::getMappings(const std::string& producerName,const std::string& parameterName,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamLevel level,bool onlySearchEnabled,ParameterMapping_vec& mappings)
{
  try
  {
    if (!mMappingSearch)
      return;

    AutoReadLock lock(&mModificationLock);

    std::string key = toLowerString(producerName + ":" + parameterName + ":" + Fmi::to_string(geometryId));

    auto s = mMappingSearch->find(key);
    if (s == mMappingSearch->end())
      return;


    for (auto it = s->second.begin(); it != s->second.end(); ++it)
    {
      if (!it->mIgnore)
      {
        if (levelId <= 0 || it->mParameterLevelId == levelId)
        {
          if (levelId <= 0 || level < 0  || it->mParameterLevel == level)
          {
            if (onlySearchEnabled)
            {
              if (it->mSearchEnabled)
                mappings.emplace_back(*it);
            }
            else
            {
              mappings.emplace_back(*it);
            }
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




void ParameterMappingFile::getAliasMappings(ParameterMapping_vec& mappings,UnitConversion_vec& unitConversions)
{
  try
  {
    if (!mMappingSearch)
      return;

    AutoReadLock lock(&mModificationLock);

    for (auto it = mMappingSearch->begin(); it != mMappingSearch->end(); ++it)
    {
      for (auto m = it->second.begin(); m != it->second.end(); ++m)
      {
        std::vector<std::string> partList;
        splitString(m->mParameterName,'-',partList);

        uint sz = partList.size();
        if (sz > 0)
        {
          const char *unit = partList[sz-1].c_str();

          for (auto uIt = unitConversions.begin(); uIt != unitConversions.end(); ++uIt)
          {
            if (strcasecmp(unit,uIt->mSourceUnit.c_str()) == 0)
            {
              ParameterMapping newMapping = *m;
              partList[sz-1] = uIt->mTargetUnit;
              char newName[100];
              char *p = newName;
              p += sprintf(p,"%s",partList[0].c_str());
              for (uint t=1; t<sz; t++)
              {
                p += sprintf(p,"-%s",partList[t].c_str());
              }
              newMapping.mParameterName = newName;
              newMapping.mConversionFunction = uIt->mConversionFunction;
              newMapping.mReverseConversionFunction = uIt->mReverseConversionFunction;
              mappings.push_back(newMapping);
            }
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ParameterMappingFile::getMappings(const std::string& producerName,const std::string& parameterName,T::ParamLevelId levelId,T::ParamLevel level,bool onlySearchEnabled,ParameterMapping_vec& mappings)
{
  try
  {
    if (!mMappingSearch)
      return;

    AutoReadLock lock(&mModificationLock);

    std::string key = toLowerString(producerName + ":" + parameterName + ":");
    int len = strlen(key.c_str());

    auto s = mMappingSearch->lower_bound(key);
    if (s == mMappingSearch->end())
      return;

    while (s != mMappingSearch->end()  &&  strncasecmp(s->first.c_str(),key.c_str(),len) == 0)
    {
      for (auto it = s->second.begin(); it != s->second.end(); ++it)
      {
        if (!it->mIgnore)
        {
          if (levelId <= 0 || it->mParameterLevelId == levelId)
          {
            if (levelId <= 0 || level < 0  || it->mParameterLevel == level)
            {
              if (onlySearchEnabled)
              {
                if (it->mSearchEnabled)
                  mappings.emplace_back(*it);
              }
              else
              {
                mappings.emplace_back(*it);
              }
            }
          }
        }
      }
      s++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ParameterMappingFile::getMappingsByParamKey(const std::string& producerName,T::ParamKeyType parameterKeyType,const std::string& parameterKey,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamLevel level,ParameterMapping_vec& mappings)
{
  try
  {
    if (!mMappingSearch)
      return;

    AutoReadLock lock(&mModificationLock);

    std::string key = toLowerString(producerName + ":" + parameterKey);

    auto s = mMappingReverseSearch->find(key);
    if (s != mMappingReverseSearch->end())
    {
      for (auto it = s->second.begin(); it != s->second.end(); ++it)
      {
        if (!it->mIgnore)
        {
          if (parameterKeyType <= 0 || it->mParameterKeyType == parameterKeyType)
          {
            if (geometryId <= 0 || it->mGeometryId == geometryId)
            {
              if (levelId <= 0 || it->mParameterLevelId == levelId)
              {
                if (levelId <= 0 || level < 0  || it->mParameterLevel == level)
                {
                  mappings.emplace_back(*it);
                }
              }
            }
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ParameterMappingFile::setParameterMappings(ParameterMapping_vec& mappings)
{
  try
  {
    MappingSearch *msearch = new MappingSearch();
    MappingSearch *mreverseSearch = new MappingSearch();

    for (auto it = mappings.begin(); it != mappings.end(); ++it)
    {
      std::string key = toLowerString(it->mProducerName + ":" + it->mParameterName + ":" + Fmi::to_string(it->mGeometryId));

      auto s = msearch->find(key);
      if (s != msearch->end())
      {
        s->second.emplace_back(*it);
      }
      else
      {
        ParameterMapping_vec vec;
        vec.push_back(*it);
        msearch->insert(std::pair<std::string,ParameterMapping_vec>(key,vec));
      }

      std::string rkey = toLowerString(it->mProducerName + ":" + it->mParameterKey);
      auto ss = mreverseSearch->find(rkey);
      if (ss != mreverseSearch->end())
      {
        ss->second.emplace_back(*it);
      }
      else
      {
        ParameterMapping_vec vec;
        vec.push_back(*it);
        mreverseSearch->insert(std::pair<std::string,ParameterMapping_vec>(rkey,vec));
      }
    }

    AutoWriteLock lock(&mModificationLock);
    mMappingSearch.reset(msearch);
    mMappingReverseSearch.reset(mreverseSearch);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




void ParameterMappingFile::loadFile()
{
  try
  {
    AutoThreadLock fLock(&mFileReadLock);

    time_t modTime = getFileModificationTime(mFilename.c_str());
    if (mLastModified == modTime)
      return;

    FILE *file = fopen(mFilename.c_str(),"re");
    if (file == nullptr)
    {
      Fmi::Exception exception(BCP,"Cannot open the mapping file!");
      exception.addParameter("Filename",mFilename);
      throw exception;
    }

    MappingSearch *msearch = new MappingSearch();
    MappingSearch *mreverseSearch = new MappingSearch();

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
            rec.mParameterKeyType = toInt32(field[2]);

          if (field[3][0] != '\0')
            rec.mParameterKey = field[3];

          if (field[4][0] != '\0')
            rec.mGeometryId = toInt32(field[4]);

          //if (field[5][0] != '\0')
          //  rec.mParameterLevelIdType = toInt32(field[5]);

          if (field[6][0] != '\0')
            rec.mParameterLevelId = toInt32(field[6]);

          if (field[7][0] != '\0')
            rec.mParameterLevel = toInt32(field[7]);

          if (field[8][0] != '\0')
            rec.mAreaInterpolationMethod = toInt16(field[8]);

          if (field[9][0] != '\0')
            rec.mTimeInterpolationMethod = toInt16(field[9]);

          if (field[10][0] != '\0')
            rec.mLevelInterpolationMethod = toInt16(field[10]);

          if (field[11][0] != '\0')
            rec.mGroupFlags = toInt64(field[11]);

          if (field[12][0] == 'E')
            rec.mSearchEnabled = true;
          else
            rec.mSearchEnabled = false;

          if (field[12][0] == 'I')
            rec.mIgnore = true;

          if (field[13][0] != '\0')
            rec.mConversionFunction = field[13];

          if (c > 14  &&  field[14][0] > ' ')
            rec.mReverseConversionFunction = field[14];

          if (c > 15  &&  field[15][0] > ' ')
            rec.mDefaultPrecision = toInt16(field[15]);

          std::string key = toLowerString(rec.mProducerName + ":" + rec.mParameterName + ":" + Fmi::to_string(rec.mGeometryId));

          auto s = msearch->find(key);
          if (s != msearch->end())
          {
            s->second.emplace_back(rec);
          }
          else
          {
            ParameterMapping_vec vec;
            vec.push_back(rec);
            msearch->insert(std::pair<std::string,ParameterMapping_vec>(key,vec));
          }

          std::string rkey = toLowerString(rec.mProducerName + ":" + rec.mParameterKey);
          auto ss = mreverseSearch->find(rkey);
          if (ss != mreverseSearch->end())
          {
            ss->second.emplace_back(rec);
          }
          else
          {
            ParameterMapping_vec vec;
            vec.push_back(rec);
            mreverseSearch->insert(std::pair<std::string,ParameterMapping_vec>(rkey,vec));
          }
        }
      }
    }
    fclose(file);

    AutoWriteLock lock(&mModificationLock);
    mMappingSearch.reset(msearch);
    mMappingReverseSearch.reset(mreverseSearch);

    mLastModified = modTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ParameterMappingFile::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    for (auto it = mMappingSearch->begin(); it != mMappingSearch->end(); ++it)
    {
      stream << it->first << "\n";
      for (auto s = it->second.begin(); s != it->second.end(); ++s)
      {
        s->print(stream,level+2,optionFlags);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




}  // namespace QueryServer
}  // namespace SmartMet
