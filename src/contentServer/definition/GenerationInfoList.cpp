#include "GenerationInfoList.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoThreadLock.h>


namespace SmartMet
{
namespace T
{


GenerationInfoList::GenerationInfoList()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





GenerationInfoList::GenerationInfoList(GenerationInfoList& generationInfoList)
{
  try
  {
    generationInfoList.lock();
    uint sz = generationInfoList.getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = generationInfoList.getGenerationInfoByIndexNoCheck(t);
      if (info != NULL)
        mList.push_back(info->duplicate());
    }
    generationInfoList.unlock();
  }
  catch (...)
  {
    generationInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





GenerationInfoList::~GenerationInfoList()
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





void GenerationInfoList::operator=(GenerationInfoList& generationInfoList)
{
  try
  {
    if (&generationInfoList == this)
      return;

    clear();

    AutoThreadLock lock(&mThreadLock);
    uint sz = generationInfoList.getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = generationInfoList.getGenerationInfoByIndexNoCheck(t);
      if (info != NULL)
        mList.push_back(info->duplicate());
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GenerationInfoList::addGenerationInfo(GenerationInfo *generationInfo)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    mList.push_back(generationInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GenerationInfoList::clear()
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





bool GenerationInfoList::deleteGenerationInfoById(uint generationId)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != NULL  &&  info->mGenerationId == generationId)
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





void GenerationInfoList::deleteGenerationInfoListByProducerId(uint producerId)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    int sz = (int)getLength()-1;
    for (int t=sz; t>=0; t--)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != NULL  &&  info->mProducerId == producerId)
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




GenerationInfo* GenerationInfoList::getGenerationInfoById(uint generationId)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != NULL  &&  info->mGenerationId == generationId)
        return info;
    }
    return NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GenerationInfoList::deleteGenerationInfoListBySourceId(uint sourceId)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    int sz = (int)getLength()-1;
    for (int t=sz; t>=0; t--)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
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





GenerationInfo* GenerationInfoList::getGenerationInfoByIndex(uint index)
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





GenerationInfo* GenerationInfoList::getGenerationInfoByIndexNoCheck(uint index)
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





GenerationInfo* GenerationInfoList::getGenerationInfoByName(std::string generationName)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != NULL  &&  strcasecmp(info->mName.c_str(),generationName.c_str()) == 0)
        return info;
    }
    return NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}






void GenerationInfoList::getGenerationInfoListByProducerId(uint producerId,GenerationInfoList& generationInfoList)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != NULL  &&  info->mProducerId == producerId)
        generationInfoList.addGenerationInfo(info->duplicate());
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




GenerationInfo* GenerationInfoList::getLastGenerationInfoByProducerIdAndStatus(uint producerId,T::GenerationStatus generationStatus)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    T::GenerationInfo *generationInfo = NULL;
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != NULL  &&  info->mProducerId == producerId  &&  info->mStatus == generationStatus)
      {
        if (generationInfo == NULL  ||  generationInfo->mName < info->mName)
          generationInfo = info;
      }
    }
    return generationInfo;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





GenerationInfo* GenerationInfoList::getLastGenerationInfoByProducerId(uint producerId)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    T::GenerationInfo *generationInfo = NULL;
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != NULL  &&  info->mProducerId == producerId)
      {
        if (generationInfo == NULL  ||  generationInfo->mName < info->mName)
          generationInfo = info;
      }
    }
    return generationInfo;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





GenerationInfo*  GenerationInfoList::getPrevGenerationInfoByProducerId(uint producerId,std::string nextGenerationName)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    T::GenerationInfo *generationInfo = NULL;
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != NULL  &&  info->mProducerId == producerId)
      {
        if ((generationInfo == NULL  &&  info->mName < nextGenerationName)  ||  (generationInfo != NULL  &&  generationInfo->mName < info->mName  &&  info->mName < nextGenerationName))
          generationInfo = info;
      }
    }
    return generationInfo;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GenerationInfoList::getGenerationInfoListBySourceId(uint sourceId,GenerationInfoList& generationInfoList)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != NULL  &&  info->mSourceId == sourceId)
        generationInfoList.addGenerationInfo(info->duplicate());
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




uint GenerationInfoList::getLength()
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





void GenerationInfoList::lock()
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





void GenerationInfoList::unlock()
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





void GenerationInfoList::writeToFile(std::string filename)
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





void GenerationInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    stream << space(level) << "GenerationInfoList\n";
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
