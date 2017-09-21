#include "ProducerInfoList.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"
#include "grid-files/common/AutoThreadLock.h"


namespace SmartMet
{
namespace T
{


ProducerInfoList::ProducerInfoList()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ProducerInfoList::ProducerInfoList(ProducerInfoList& producerInfoList)
{
  try
  {
    producerInfoList.lock();
    uint sz = producerInfoList.getLength();
    for (uint t=0; t<sz; t++)
    {
      ProducerInfo *info = producerInfoList.getProducerInfoByIndexNoCheck(t);
      if (info != NULL)
        mList.push_back(info->duplicate());
    }
    producerInfoList.unlock();
  }
  catch (...)
  {
    producerInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ProducerInfoList::~ProducerInfoList()
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





void ProducerInfoList::operator=(ProducerInfoList& producerInfoList)
{
  try
  {
    if (&producerInfoList == this)
      return;

    clear();

    AutoThreadLock lock(&mThreadLock);
    uint sz = producerInfoList.getLength();
    for (uint t=0; t<sz; t++)
    {
      ProducerInfo *info = producerInfoList.getProducerInfoByIndexNoCheck(t);
      if (info != NULL)
        mList.push_back(info->duplicate());
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ProducerInfoList::addProducerInfo(ProducerInfo *producerInfo)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    mList.push_back(producerInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ProducerInfoList::clear()
{
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool ProducerInfoList::deleteProducerInfoById(uint producerId)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != NULL  &&  info->mProducerId == producerId)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ProducerInfoList::deleteProducerInfoListBySourceId(uint sourceId)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    int sz = (int)getLength()-1;
    for (int t=sz; t>=0; t--)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != NULL  &&  info->mSourceId == sourceId)
      {
        delete mList[t];
        mList.erase(mList.begin() + t);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ProducerInfo* ProducerInfoList::getProducerInfoById(uint producerId)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != NULL  &&  info->mProducerId == producerId)
        return info;
    }
    return NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ProducerInfo* ProducerInfoList::getProducerInfoByName(std::string producerName)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != NULL  &&  strcasecmp(info->mName.c_str(),producerName.c_str()) == 0)
        return info;
    }
    return NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ProducerInfo* ProducerInfoList::getProducerInfoByIndex(uint index)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    if (index >= mList.size())
      return NULL;

    return mList.at(index);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ProducerInfo* ProducerInfoList::getProducerInfoByIndexNoCheck(uint index)
{
  try
  {
    return mList.at(index);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ProducerInfoList::getProducerInfoListBySourceId(uint sourceId,ProducerInfoList& producerInfoList)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ProducerInfo *info = getProducerInfoByIndexNoCheck(t);
      if (info != NULL  &&  info->mSourceId == sourceId)
      {
        producerInfoList.addProducerInfo(info->duplicate());
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




uint ProducerInfoList::getLength()
{
  try
  {
    return (uint)mList.size();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ProducerInfoList::lock()
{
  try
  {
    return mThreadLock.lock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ProducerInfoList::unlock()
{
  try
  {
    return mThreadLock.unlock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ProducerInfoList::writeToFile(std::string filename)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    FILE *file = fopen(filename.c_str(),"w");
    if (file == NULL)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void ProducerInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




}
}
