#include "ProducerInfoList.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoThreadLock.h>
#include <grid-files/common/ShowFunction.h>
#include <boost/functional/hash.hpp>

#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace T
{


ProducerInfoList::ProducerInfoList()
{
  FUNCTION_TRACE
  try
  {
    mModificationLock.setLockingEnabled(false);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ProducerInfoList::ProducerInfoList(ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    mModificationLock.setLockingEnabled(false);
    producerInfoList.lock();
    uint sz = producerInfoList.getLength();
    for (uint t=0; t<sz; t++)
    {
      ProducerInfo *info = producerInfoList.getProducerInfoByIndexNoCheck(t);
      if (info != nullptr)
        mList.push_back(info->duplicate());
    }
    producerInfoList.unlock();
  }
  catch (...)
  {
    producerInfoList.unlock();
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ProducerInfoList::~ProducerInfoList()
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    std::size_t sz = mList.size();
    for (std::size_t t=0; t<sz; t++)
      delete(mList[t]);

    mList.clear();
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





ProducerInfoList& ProducerInfoList::operator=(ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (&producerInfoList == this)
      return *this;

    clear();

    AutoWriteLock lock(&mModificationLock);
    uint sz = producerInfoList.getLength();
    for (uint t=0; t<sz; t++)
    {
      ProducerInfo *info = producerInfoList.getProducerInfoByIndexNoCheck(t);
      if (info != nullptr)
        mList.push_back(info->duplicate());
    }
    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ProducerInfoList::addProducerInfo(ProducerInfo *producerInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    mList.push_back(producerInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ProducerInfoList::clear()
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    std::size_t sz = mList.size();
    for (std::size_t t=0; t<sz; t++)
      delete(mList[t]);

    mList.clear();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool ProducerInfoList::deleteProducerInfoById(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mProducerId == producerId)
      {
        delete mList[t];
        mList.erase(mList.begin() + t);
        return true;
      }
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ProducerInfoList::deleteProducerInfoListBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    int sz = getLength()-1;
    for (int t=sz; t>=0; t--)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mSourceId == sourceId)
      {
        delete mList[t];
        mList.erase(mList.begin() + t);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ProducerInfo* ProducerInfoList::getProducerInfoById(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mProducerId == producerId)
        return info;
    }
    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ProducerInfo* ProducerInfoList::getProducerInfoByName(std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != nullptr  &&  strcasecmp(info->mName.c_str(),producerName.c_str()) == 0)
        return info;
    }
    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::size_t ProducerInfoList::getHash()
{
  FUNCTION_TRACE
  try
  {
    std::size_t hash = 0;

    AutoReadLock lock(&mModificationLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != nullptr)
      {
        boost::hash_combine(hash,info->mProducerId);
        boost::hash_combine(hash,info->mName);
        boost::hash_combine(hash,info->mTitle);
        boost::hash_combine(hash,info->mDescription);
        boost::hash_combine(hash,info->mFlags);
        boost::hash_combine(hash,info->mSourceId);
      }
    }
    return hash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



ProducerInfo* ProducerInfoList::getProducerInfoByIndex(uint index)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    if (index >= mList.size())
      return nullptr;

    return mList.at(index);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ProducerInfo* ProducerInfoList::getProducerInfoByIndexNoCheck(uint index)
{
  FUNCTION_TRACE
  try
  {
    return mList.at(index);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ProducerInfoList::getProducerInfoListBySourceId(uint sourceId,ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    producerInfoList.clear();

    AutoReadLock lock(&mModificationLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mSourceId == sourceId)
      {
        producerInfoList.addProducerInfo(info->duplicate());
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint ProducerInfoList::getLength()
{
  FUNCTION_TRACE
  try
  {
    return mList.size();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ProducerInfoList::sortByName()
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    std::vector<ProducerInfo*>  newList;
    std::set<std::string> nameList;

    for (auto it = mList.begin(); it != mList.end(); ++it)
    {
      nameList.insert((*it)->mName);
    }

    for (auto n = nameList.begin(); n != nameList.end(); ++n)
    {
      for (auto it = mList.begin(); it != mList.end(); ++it)
      {
        if ((*it)->mName == *n)
        {
          newList.push_back(*it);
        }
      }
    }
    mList = newList;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ProducerInfoList::lock()
{
  FUNCTION_TRACE
  try
  {
    mModificationLock.lock();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ProducerInfoList::unlock()
{
  FUNCTION_TRACE
  try
  {
    mModificationLock.unlock();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ProducerInfoList::setLockingEnabled(bool lockingEnabled)
{
  FUNCTION_TRACE
  try
  {
    mModificationLock.setLockingEnabled(lockingEnabled);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ProducerInfoList::writeToFile(std::string filename)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    FILE *file = fopen(filename.c_str(),"we");
    if (file == nullptr)
    {
      Fmi::Exception exception(BCP,"Cannot create the file!");
      exception.addParameter("Filename",filename);
      throw exception;
    }

    std::size_t sz = mList.size();
    for (std::size_t t=0; t<sz; t++)
    {
      fprintf(file,"%s\n",mList[t]->getCsv().c_str());
    }

    fclose(file);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ProducerInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    stream << space(level) << "ProducerInfoList\n";
    std::size_t sz = mList.size();
    for (std::size_t t=0; t<sz; t++)
      mList[t]->print(stream,level+1,optionFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




}
}
