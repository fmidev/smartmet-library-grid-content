#include "GenerationInfoList.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoThreadLock.h>
#include <grid-files/common/ShowFunction.h>

#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace T
{


GenerationInfoList::GenerationInfoList()
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





GenerationInfoList::GenerationInfoList(GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    generationInfoList.lock();
    uint sz = generationInfoList.getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = generationInfoList.getGenerationInfoByIndexNoCheck(t);
      if (info != nullptr)
        mList.push_back(info->duplicate());
    }
    generationInfoList.unlock();
  }
  catch (...)
  {
    generationInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





GenerationInfoList::~GenerationInfoList()
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





GenerationInfoList& GenerationInfoList::operator=(GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (&generationInfoList == this)
      return *this;

    clear();

    AutoThreadLock lock(&mThreadLock);
    uint sz = generationInfoList.getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = generationInfoList.getGenerationInfoByIndexNoCheck(t);
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





void GenerationInfoList::addGenerationInfo(GenerationInfo *generationInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    mList.push_back(generationInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GenerationInfoList::clear()
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





bool GenerationInfoList::deleteGenerationInfoById(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mGenerationId == generationId)
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





void GenerationInfoList::deleteGenerationInfoListByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    int sz = getLength()-1;
    for (int t=sz; t>=0; t--)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mProducerId == producerId)
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




GenerationInfo* GenerationInfoList::getGenerationInfoById(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mGenerationId == generationId)
        return info;
    }
    return nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GenerationInfoList::deleteGenerationInfoListBySourceId(uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    int sz = getLength()-1;
    for (int t=sz; t>=0; t--)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
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





GenerationInfo* GenerationInfoList::getGenerationInfoByIndex(uint index)
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





GenerationInfo* GenerationInfoList::getGenerationInfoByIndexNoCheck(uint index)
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





GenerationInfo* GenerationInfoList::getGenerationInfoByName(std::string generationName)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != nullptr  &&  strcasecmp(info->mName.c_str(),generationName.c_str()) == 0)
        return info;
    }
    return nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}






GenerationInfo* GenerationInfoList::getGenerationInfoByAnalysisTime(std::string analysisTime)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != nullptr  &&  strcasecmp(info->mAnalysisTime.c_str(),analysisTime.c_str()) == 0)
        return info;
    }
    return nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}






void GenerationInfoList::getGenerationInfoListByProducerId(uint producerId,GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mProducerId == producerId)
        generationInfoList.addGenerationInfo(info->duplicate());
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




GenerationInfo* GenerationInfoList::getLastGenerationInfoByProducerIdAndStatus(uint producerId,uchar generationStatus)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    T::GenerationInfo *generationInfo = nullptr;
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mProducerId == producerId  &&  info->mStatus == generationStatus)
      {
        if (generationInfo == nullptr  ||  generationInfo->mName < info->mName)
          generationInfo = info;
      }
    }
    return generationInfo;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





GenerationInfo* GenerationInfoList::getLastGenerationInfoByProducerId(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    T::GenerationInfo *generationInfo = nullptr;
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mProducerId == producerId)
      {
        if (generationInfo == nullptr  ||  generationInfo->mName < info->mName)
          generationInfo = info;
      }
    }
    return generationInfo;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





GenerationInfo*  GenerationInfoList::getPrevGenerationInfoByProducerId(uint producerId,std::string nextGenerationName)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    T::GenerationInfo *generationInfo = nullptr;
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mProducerId == producerId)
      {
        if ((generationInfo == nullptr  &&  info->mName < nextGenerationName)  ||  (generationInfo != nullptr  &&  generationInfo->mName < info->mName  &&  info->mName < nextGenerationName))
          generationInfo = info;
      }
    }
    return generationInfo;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GenerationInfoList::getGenerationInfoListBySourceId(uint sourceId,GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mSourceId == sourceId)
        generationInfoList.addGenerationInfo(info->duplicate());
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




uint GenerationInfoList::getLength()
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





void GenerationInfoList::getAnalysisTimes(std::vector<std::string>& analysisTimes,bool reverseOrder)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    std::set<std::string> newList;

    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      GenerationInfo *info = getGenerationInfoByIndexNoCheck(t);
      newList.insert(info->mAnalysisTime);
    }

    if (!reverseOrder)
    {
      for (auto it = newList.begin(); it != newList.end(); ++it)
      {
        analysisTimes.push_back(*it);
      }
    }
    else
    {
      for (auto it = newList.rbegin(); it != newList.rend(); ++it)
      {
        analysisTimes.push_back(*it);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GenerationInfoList::lock()
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





void GenerationInfoList::unlock()
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





void GenerationInfoList::writeToFile(std::string filename)
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





void GenerationInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  FUNCTION_TRACE
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




}
}
