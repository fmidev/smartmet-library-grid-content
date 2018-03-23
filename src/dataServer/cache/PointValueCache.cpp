#include "PointValueCache.h"
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/common/AutoWriteLock.h>

namespace SmartMet
{
namespace DataServer
{


PointValueCache::PointValueCache()
{
  try
  {
    mMaxSize = 10000000;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





PointValueCache::~PointValueCache()
{
  try
  {
    clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void PointValueCache::addValue(uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::ParamValue value)
{
  try
  {
    uint maxAge = 300;
    while (mValueList.size() >= mMaxSize)
    {
      deleteValuesByAge(maxAge);
      maxAge = maxAge - 30;
    }

    std::size_t key = getKey(fileId,messageIndex,flags,coordinateType,x,y,interpolationMethod);
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    auto it = mValueList.find(key);

    if (it != mValueList.end())
    {
      it->second->fileId = fileId;
      it->second->value = value;
      it->second->lastAccessTime = time(0);
      return;
    }

    CacheValueRec_ptr rec = new CacheValueRec();
    rec->fileId = fileId;
    rec->value = value;
    rec->lastAccessTime = time(0);

    mValueList.insert(std::pair<std::size_t,CacheValueRec_ptr>(key,rec));

  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool PointValueCache::getValue(uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::ParamValue& value)
{
  try
  {
    std::size_t key = getKey(fileId,messageIndex,flags,coordinateType,x,y,interpolationMethod);
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    auto it = mValueList.find(key);

    if (it != mValueList.end())
    {
      it->second->lastAccessTime = time(0);
      value = it->second->value;
      return true;
    }
    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void PointValueCache::clear()
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (auto it=mValueList.begin(); it != mValueList.end(); ++it)
    {
      delete it->second;
    }
    mValueList.clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void PointValueCache::deleteValue(uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,short interpolationMethod)
{
  try
  {
    std::size_t key = getKey(fileId,messageIndex,flags,coordinateType,x,y,interpolationMethod);
    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    auto it = mValueList.find(key);

    if (it != mValueList.end())
    {
      delete it->second;
      mValueList.erase(it);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void PointValueCache::deleteValue(std::size_t key)
{
  try
  {
    auto it = mValueList.find(key);

    if (it != mValueList.end())
    {
      delete it->second;
      mValueList.erase(it);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void PointValueCache::deleteValuesByFileId(uint fileId)
{
  try
  {
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    std::vector<std::size_t> keyList;

    for (auto it=mValueList.begin(); it != mValueList.end(); ++it)
    {
      if (it->second->fileId == fileId)
        keyList.push_back(it->first);
    }

    for (auto it=keyList.begin(); it != keyList.end(); ++it)
    {
      deleteValue(*it);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void PointValueCache::deleteValuesByAge(uint maxAge)
{
  try
  {
    time_t lastAccess = time(0) - maxAge;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    std::vector<std::size_t> keyList;

    for (auto it=mValueList.begin(); it != mValueList.end(); ++it)
    {
      if (it->second->lastAccessTime < lastAccess)
        keyList.push_back(it->first);
    }

    for (auto it=keyList.begin(); it != keyList.end(); ++it)
    {
      deleteValue(*it);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





std::size_t PointValueCache::getKey(uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,short interpolationMethod)
{
  try
  {
    char buf[100];
    sprintf(buf,"%08x%04x%08x%u%f%f%u",fileId,messageIndex,flags,(uint)coordinateType,x,y,(uint)interpolationMethod);
    std::size_t str_hash = std::hash<std::string>{}(std::string(buf));
    return str_hash;
    /*
    std::size_t seed = 0;
    boost::hash_combine(seed,fileId);
    boost::hash_combine(seed,messageIndex);
    boost::hash_combine(seed,(std::uint32_t)coordinateType);
    boost::hash_combine(seed,x);
    boost::hash_combine(seed,y);
    boost::hash_combine(seed,(std::uint32_t)interpolationMethod);
    return seed;
    */
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void PointValueCache::setMaxSize(std::size_t maxSize)
{
  try
  {
    mMaxSize = maxSize;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}



}
}
