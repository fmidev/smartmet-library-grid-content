#include "ProducerInfoList.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoThreadLock.h>
#include <grid-files/common/ShowFunction.h>

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
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ProducerInfoList::ProducerInfoList(ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ProducerInfoList::~ProducerInfoList()
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    std::size_t sz = mList.size();
    for (std::size_t t=0; t<sz; t++)
      delete(mList[t]);

    mList.clear();
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
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

    AutoThreadLock lock(&mThreadLock);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ProducerInfoList::addProducerInfo(ProducerInfo *producerInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    mList.push_back(producerInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ProducerInfoList::clear()
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    std::size_t sz = mList.size();
    for (std::size_t t=0; t<sz; t++)
      delete(mList[t]);

    mList.clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





bool ProducerInfoList::deleteProducerInfoById(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ProducerInfoList::deleteProducerInfoListBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ProducerInfo* ProducerInfoList::getProducerInfoById(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ProducerInfo* ProducerInfoList::getProducerInfoByName(std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ProducerInfo* ProducerInfoList::getProducerInfoByIndex(uint index)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    if (index >= mList.size())
      return nullptr;

    return mList.at(index);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ProducerInfoList::getProducerInfoListBySourceId(uint sourceId,ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ProducerInfoList::sortByName()
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ProducerInfoList::lock()
{
  FUNCTION_TRACE
  try
  {
    return mThreadLock.lock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ProducerInfoList::unlock()
{
  FUNCTION_TRACE
  try
  {
    return mThreadLock.unlock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ProducerInfoList::writeToFile(std::string filename)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    FILE *file = fopen(filename.c_str(),"we");
    if (file == nullptr)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot create the file!");
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void ProducerInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    stream << space(level) << "ProducerInfoList\n";
    std::size_t sz = mList.size();
    for (std::size_t t=0; t<sz; t++)
      mList[t]->print(stream,level+1,optionFlags);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




}
}
