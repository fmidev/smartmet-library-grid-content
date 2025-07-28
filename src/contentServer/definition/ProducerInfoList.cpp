#include "ProducerInfoList.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoThreadLock.h>
#include <grid-files/common/ShowFunction.h>
#include <macgyver/Hash.h>

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
      if (info != nullptr  &&  (info->mFlags & T::ProducerInfo::Flags::DeletedProducer) == 0)
        mList.emplace_back(info->duplicate());
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
      if (info != nullptr  &&  (info->mFlags & T::ProducerInfo::Flags::DeletedProducer) == 0)
        mList.emplace_back(info->duplicate());
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
    if (producerInfo == nullptr)
      return;

    ProducerInfo pInfo;
    if (getProducerInfoById(producerInfo->mProducerId))
      return; // Id exists

    if (getProducerInfoByName(producerInfo->mName))
      return; // Name exists

    AutoWriteLock lock(&mModificationLock);
    mList.emplace_back(producerInfo);
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





uint ProducerInfoList::deleteProducerInfoListBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint cnt = 0;
    int sz = getLength()-1;
    for (int t=sz; t>0; t--)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mSourceId == sourceId)
      {
        cnt++;
        delete mList[t];
        mList.erase(mList.begin() + t);
      }
    }
    return cnt;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ProducerInfoList::deleteMarkedProducers()
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint cnt = 0;
    int sz = getLength()-1;
    for (int t=sz; t>0; t--)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != nullptr  &&  (info->mFlags & T::ProducerInfo::Flags::DeletedProducer))
      {
        cnt++;
        delete mList[t];
        mList.erase(mList.begin() + t);
      }
    }
    return cnt;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ProducerInfoList::markDeletedById(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    ProducerInfo *info = getProducerInfoById(producerId);
    if (info != nullptr  &&  info->mProducerId == producerId)
    {
      info->mFlags |= T::ProducerInfo::Flags::DeletedProducer;
      return 1;
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ProducerInfoList::markDeletedBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    uint cnt = 0;
    long sz = getLength()-1;
    for (long t=sz; t>=0; t--)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mSourceId == sourceId)
      {
        info->mFlags |= T::ProducerInfo::Flags::DeletedProducer;
        cnt++;
      }
    }
    return cnt;
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
      if (info != nullptr  &&  info->mProducerId == producerId  &&  (info->mFlags & T::ProducerInfo::Flags::DeletedProducer) == 0)
        return info;
    }
    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool ProducerInfoList::getProducerInfoById(uint producerId,ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mProducerId == producerId  &&  (info->mFlags & T::ProducerInfo::Flags::DeletedProducer) == 0)
      {
        producerInfo = *info;
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





ProducerInfo* ProducerInfoList::getProducerInfoByName(const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != nullptr  &&  strcasecmp(info->mName.c_str(),producerName.c_str()) == 0  &&  (info->mFlags & T::ProducerInfo::Flags::DeletedProducer) == 0)
        return info;
    }
    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool ProducerInfoList::getProducerInfoByName(const std::string& producerName,ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != nullptr  &&  strcasecmp(info->mName.c_str(),producerName.c_str()) == 0  &&  (info->mFlags & T::ProducerInfo::Flags::DeletedProducer) == 0)
      {
        producerInfo = *info;
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
        Fmi::hash_merge(hash,info->mProducerId);
        Fmi::hash_merge(hash,info->mName);
        Fmi::hash_merge(hash,info->mTitle);
        Fmi::hash_merge(hash,info->mDescription);
        Fmi::hash_merge(hash,info->mFlags);
        Fmi::hash_merge(hash,info->mSourceId);
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
      if (info != nullptr  &&  info->mSourceId == sourceId  &&  (info->mFlags & T::ProducerInfo::Flags::DeletedProducer) == 0)
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
          newList.emplace_back(*it);
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




void ProducerInfoList::writeToFile(const std::string& filename)
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
