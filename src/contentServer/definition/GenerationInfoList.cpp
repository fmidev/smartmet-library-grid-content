#include "GenerationInfoList.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoThreadLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/common/ShowFunction.h>

#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace T
{


ThreadLock GenerationInfoList_sortLock;
uint generationInfo_comparisonMethod = 0;



int generationInfo_compare(const void *_val1,const void *_val2)
{
  if (_val1 != nullptr  &&  _val2 != nullptr)
  {
    GenerationInfoPtr *obj1 = const_cast<GenerationInfoPtr*>(reinterpret_cast<const GenerationInfoPtr *>(_val1));
    GenerationInfoPtr *obj2 = const_cast<GenerationInfoPtr*>(reinterpret_cast<const GenerationInfoPtr *>(_val2));

    return (*obj1)->compare(generationInfo_comparisonMethod,(*obj2));
  }
  return 0;
}



GenerationInfoList::GenerationInfoList()
{
  FUNCTION_TRACE
  try
  {
    mModificationLockPtr = &mModificationLock;
    mReleaseObjects = true;
    mComparisonMethod = GenerationInfo::ComparisonMethod::none;
    mSize = 100;
    mLength = 0;
    mArray = new GenerationInfoPtr[100];

    for (uint t=0; t<100; t++)
    {
      mArray[t] = nullptr;
    }
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
    mModificationLockPtr = &mModificationLock;
    if (generationInfoList.getModificationLockPtr() != mModificationLockPtr)
      generationInfoList.lock();

    mReleaseObjects = true;
    mArray = nullptr;
    mSize = generationInfoList.getSize();
    mLength = generationInfoList.getLength();
    mComparisonMethod = generationInfoList.mComparisonMethod;
    if (mSize > 0)
    {
      mArray = new GenerationInfoPtr[mSize];

      for (uint t=0; t<mSize; t++)
      {
        GenerationInfo *info = generationInfoList.getGenerationInfoByIndexNoCheck(t);
        if (info != nullptr)
        {
          mArray[t] = info->duplicate();
        }
        else
        {
          mArray[t] = nullptr;
        }
      }
    }
    if (generationInfoList.getModificationLockPtr() != mModificationLockPtr)
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
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    if (mArray != nullptr)
    {
      for (uint t=0; t<mSize; t++)
      {
        if (mArray[t] != nullptr  &&  mReleaseObjects)
          delete mArray[t];

        mArray[t] = nullptr;
      }

      delete[] mArray;
    }

    mArray = nullptr;
    mSize = 0;
    mLength = 0;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
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

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (generationInfoList.getModificationLockPtr() != mModificationLockPtr)
      generationInfoList.lock();

    mArray = nullptr;
    mSize = generationInfoList.getSize();
    mLength = generationInfoList.getLength();
    mComparisonMethod = generationInfoList.mComparisonMethod;
    if (mSize > 0)
    {
      mArray = new GenerationInfoPtr[mSize];

      for (uint t=0; t<mSize; t++)
      {
        GenerationInfo *info = generationInfoList.getGenerationInfoByIndexNoCheck(t);
        if (info != nullptr)
        {
          if (mReleaseObjects)
            mArray[t] = info->duplicate();
          else
            mArray[t] = info;
        }
        else
        {
          mArray[t] = nullptr;
        }
      }
    }
    if (generationInfoList.getModificationLockPtr() != mModificationLockPtr)
      generationInfoList.unlock();

    return *this;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GenerationInfoList::setModificationLockPtr(ModificationLock* modificationLockPtr)
{
  FUNCTION_TRACE
  try
  {
    if (modificationLockPtr != nullptr)
      mModificationLockPtr = modificationLockPtr;
    else
      mModificationLockPtr = &mModificationLock;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ModificationLock*  GenerationInfoList::getModificationLockPtr()
{
  FUNCTION_TRACE
  try
  {
    return mModificationLockPtr;
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
    return mModificationLockPtr->lock();
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
    return mModificationLockPtr->unlock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





GenerationInfo* GenerationInfoList::addGenerationInfo(GenerationInfo *generationInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    if (mArray == nullptr  ||  mLength == mSize)
    {
      increaseSize(mSize + mSize/5 + 10);
    }

    if (mComparisonMethod == GenerationInfo::ComparisonMethod::none)
    {
      mArray[mLength] = generationInfo;
      mLength++;
      return generationInfo;
    }

    int idx = getClosestIndexNoLock(mComparisonMethod,*generationInfo);

    if (idx < C_INT(mLength)  &&  mArray[idx] != nullptr  &&   mArray[idx]->compare(mComparisonMethod,generationInfo) == 0)
    {
      // If generation with the same id exists, we should not add the new contet, because other generationLists might point
      // the existing generation record;

      return mArray[idx];
    }

    while (idx < C_INT(mLength)  &&  mArray[idx] != nullptr  &&   mArray[idx]->compare(mComparisonMethod,generationInfo) < 0)
    {
      idx++;
    }

    if (idx == C_INT(mLength))
    {
      mArray[mLength] = generationInfo;
      mLength++;
      return generationInfo;
    }

    if (idx < C_INT(mLength))
      memmove(&mArray[idx+1],&mArray[idx],sizeof(void*)*(mLength-idx));

    mArray[idx] = generationInfo;
    mLength++;
    return generationInfo;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GenerationInfoList::increaseSize(uint newSize)
{
  FUNCTION_TRACE
  try
  {
//    printf("SET SIZE %u => %u\n",mSize,newSize);
    if (mArray == nullptr)
    {
      mSize = newSize;
      mLength = 0;
      mArray = new GenerationInfoPtr[mSize];

      for (uint t=0; t<mSize; t++)
      {
        mArray[t] = nullptr;
      }
      return;
    }

    GenerationInfoPtr *newArray = new GenerationInfoPtr[newSize];
    for (uint t=0; t<newSize; t++)
    {
      if (t < mSize)
      {
        newArray[t] = mArray[t];
      }
      else
      {
        newArray[t] = nullptr;
      }
    }

    delete[] mArray;
    mArray = newArray;
    mSize = newSize;
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
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    if (mArray != nullptr)
    {
      for (uint t=0; t<mSize; t++)
      {
        if (mArray[t] != nullptr  &&  mReleaseObjects)
          delete mArray[t];

        mArray[t] = nullptr;
      }

      delete[] mArray;
    }

    mArray = nullptr;
    mSize = 0;
    mLength = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int GenerationInfoList::getClosestIndex(uint comparisonMethod,GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    return getClosestIndexNoLock(comparisonMethod,generationInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int GenerationInfoList::getClosestIndexNoLock(uint comparisonMethod,GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr  ||  mLength == 0)
      return 0;

    if (comparisonMethod != mComparisonMethod)
    {
      for (uint t=0; t<mLength; t++)
      {
        if (mArray[t] != nullptr)
        {
          int res = mArray[t]->compare(comparisonMethod,&generationInfo);
          if (res == 0)
          {
            return t;
          }
        }
      }
      return 0;
    }

    int low = 0;
    int high = C_INT(mLength) - 1;
    int mid = 0;

    while (low <= high)
    {
      mid = (low + high) / 2;
      int res = mArray[mid]->compare(comparisonMethod,&generationInfo);

      if (res == 0)
      {
        while (mid > 0  &&  mArray[mid-1]->compare(comparisonMethod,&generationInfo) == 0)
          mid--;

        return mid;
      }

      if (res < 0)
        low = mid + 1;
      else
        high = mid - 1;
    }

    if (mid >= 0  &&  mid < C_INT(mLength))
    {
      if (mArray[mid]->compare(comparisonMethod,&generationInfo) < 0)
      {
        while (mid < C_INT(mLength)  &&  mArray[mid] != nullptr  &&   mArray[mid]->compare(comparisonMethod,&generationInfo) < 0)
          mid++;

        return mid-1;
      }
      else
      {
        while (mid > 0  &&  mArray[mid] != nullptr  &&   mArray[mid]->compare(comparisonMethod,&generationInfo) > 0)
          mid--;

        return mid;
      }
    }
    return 0;
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
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);

    GenerationInfo search;
    search.mGenerationId = generationId;
    int idx = getClosestIndexNoLock(GenerationInfo::ComparisonMethod::generationId,search);
    if (idx < 0 ||  idx >= C_INT(mLength))
      return false;

    GenerationInfo *info = mArray[idx];
    if (info->mGenerationId != generationId)
      return false;

    for (uint t=C_UINT(idx)+1; t<mLength; t++)
    {
      mArray[t-1] = mArray[t];
      mArray[t] = nullptr;
    }

    if (mReleaseObjects)
      delete info;

    mLength--;
    return true;
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
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      GenerationInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
      {
        if (info->mProducerId == producerId)
        {
          if (mReleaseObjects)
            delete info;
        }
        else
        {
          mArray[p] = info;
          p++;
        }
      }
    }
    mLength = p;
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
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    GenerationInfo search;
    search.mGenerationId = generationId;
    int idx = getClosestIndexNoLock(GenerationInfo::ComparisonMethod::generationId,search);
    if (idx < 0  ||  C_UINT(idx) >= getLength())
      return nullptr;

    GenerationInfo *info = getGenerationInfoByIndexNoCheck(idx);
    if (info != nullptr  &&  info->mGenerationId == generationId)
      return info;

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
    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    uint p = 0;
    for (uint t=0; t<mLength; t++)
    {
      GenerationInfo *info = mArray[t];
      mArray[t] = nullptr;
      if (info != nullptr)
      {
        if (info->mSourceId == sourceId)
        {
          if (mReleaseObjects)
            delete info;
        }
        else
        {
          mArray[p] = info;
          p++;
        }
      }
    }
    mLength = p;
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
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    if (index >= mLength)
      return nullptr;

    return mArray[index];
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
    return mArray[index];
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
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      GenerationInfo *info = mArray[t];
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
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      GenerationInfo *info = mArray[t];
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





int GenerationInfoList::getGenerationInfoIndexByAnalysisTime(std::string analysisTime)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      GenerationInfo *info = mArray[t];
      if (info != nullptr  &&  strcasecmp(info->mAnalysisTime.c_str(),analysisTime.c_str()) == 0)
        return t;

    }
    return -1;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int GenerationInfoList::getGenerationInfoIndexByGenerationId(uint generationId)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      GenerationInfo *info = mArray[t];
      if (info != nullptr  &&  info->mGenerationId == generationId)
        return t;
    }
    return -1;
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
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    int len =  getLength();
    int idx = 0;

    if (mComparisonMethod == GenerationInfo::ComparisonMethod::producerId)
    {
      GenerationInfo search;
      search.mProducerId = producerId;

      idx = getClosestIndexNoLock(GenerationInfo::ComparisonMethod::producerId,search);
      if (idx < 0  || idx >= len)
        return;

    }

    for (int t=idx; t<len; t++)
    {
      GenerationInfo *info = mArray[t];
      if (info != nullptr  &&  info->mProducerId == producerId)
      {
        generationInfoList.addGenerationInfo(info->duplicate());
      }
      else
      {
        if (mComparisonMethod == GenerationInfo::ComparisonMethod::producerId  &&  info->mProducerId > producerId)
          return;
      }
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
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      GenerationInfo *info = mArray[t];
      if (info != nullptr  &&  info->mProducerId == producerId  &&  info->mStatus == generationStatus)
        return info;
    }
    return nullptr;
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
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    T::GenerationInfo *generationInfo = nullptr;
    for (uint t=0; t<mLength; t++)
    {
      GenerationInfo *info = mArray[t];
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
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    T::GenerationInfo *generationInfo = nullptr;
    for (uint t=0; t<mLength; t++)
    {
      GenerationInfo *info = mArray[t];
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
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);
    for (uint t=0; t<mLength; t++)
    {
      GenerationInfo *info = mArray[t];
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
    return mLength;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint GenerationInfoList::getSize() const
{
  FUNCTION_TRACE
  try
  {
    return mSize;
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
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

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





void GenerationInfoList::sort(uint comparisonMethod)
{
  FUNCTION_TRACE
  try
  {
    if (mArray == nullptr)
      return;

    AutoWriteLock lock(mModificationLockPtr,__FILE__,__LINE__);
    mComparisonMethod = comparisonMethod;

    AutoThreadLock globalLock(&GenerationInfoList_sortLock);
    generationInfo_comparisonMethod = comparisonMethod;

    qsort(mArray,mLength,sizeof(GenerationInfoPtr),generationInfo_compare);
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
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    FILE *file = fopen(filename.c_str(),"we");
    if (file == nullptr)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot create the file!");
      exception.addParameter("Filename",filename);
      throw exception;
    }

    for (uint t=0; t<mLength; t++)
    {
      GenerationInfo *info = mArray[t];
      if (info != nullptr)
        fprintf(file,"%s\n",info->getCsv().c_str());
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
    AutoReadLock lock(mModificationLockPtr,__FILE__,__LINE__);

    stream << space(level) << "GenerationInfoList\n";
    for (uint t=0; t<mLength; t++)
    {
      GenerationInfo *info = mArray[t];
      if (info != nullptr)
        info->print(stream,level+1,optionFlags);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




}
}
