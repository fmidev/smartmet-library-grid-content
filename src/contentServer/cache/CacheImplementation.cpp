#include "CacheImplementation.h"

#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/common/AutoThreadLock.h>
#include <grid-files/identification/GridDef.h>
#include <grid-files/common/ShowFunction.h>
#include <boost/functional/hash.hpp>
#include <boost/make_shared.hpp>
#include <atomic>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF

namespace SmartMet
{
namespace ContentServer
{


static void* CacheImplementation_eventProcessingThread(void *arg)
{
  try
  {
    CacheImplementation *service = (CacheImplementation*)arg;
    service->eventProcessingThread();
    return nullptr;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Operation failed!",nullptr);
    exception.printError();
    exit(-1);
  }
}





CacheImplementation::CacheImplementation()
{
  FUNCTION_TRACE
  try
  {
    mImplementationType = Implementation::Cache;
    mContentStorage = nullptr;
    mContentStorageStartTime = 0;
    mStartTime = 0;
    mSessionId = 0;
    mShutdownRequested = false;
    mUpdateInProgress = false;
    mRequestForwardEnabled = false;
    mLastProcessedEventId = 0;
    mProducerCount = 0xFFFFFFFF;
    mGenerationCount = 0xFFFFFFFF;
    mGeometryCount = 0xFFFFFFFF;
    mFileCount = 0xFFFFFFFF;
    mContentCount = 0xFFFFFFFF;
    mSaveEnabled = false;
    mSaveDir = "/tmp";
    mReloadActivated = false;
    mProducerDeleteCounter = 0;
    mGenerationDeleteCounter = 0;
    mGeometryDeleteCounter = 0;
    mFileDeleteCounter = 0;
    mContentDeleteCounter = 0;
    mActiveSearchStructure = 0;

    mFileInfoList.setComparisonMethod(T::FileInfo::ComparisonMethod::none);
    mContentInfoList.setComparisonMethod(T::ContentInfo::ComparisonMethod::file_message);
    mGenerationInfoList.setComparisonMethod(T::GenerationInfo::ComparisonMethod::generationId);

    mProducerInfoList.setLockingEnabled(true);
    mGenerationInfoList.setLockingEnabled(true);
    mFileInfoList.setLockingEnabled(true);
    mContentInfoList.setLockingEnabled(true);

    mContentUpdateTime = 0;
    mContentUpdateInterval = 180;
    mContentSwapEnabled = true;
    mSearchStructurePtr[0] = nullptr;
    mSearchStructurePtr[1] = nullptr;
    mContentChangeTime = 0;
    mContentSwapCounter = 0;

    mCachedFiles_waitTime = 20;          // Wait time before the next local cache check
    mCachedFiles_totalWaitTime = 0;      // Total time that the search structure swapping is waited
    mCachedFiles_maxWaitTime = 0;        // The search structure swapping is forced after this wait (in seconds)
    mCachedFiles_maxFirstWaitTime = 0;   // The search structure swapping (the first time) is forced after this wait (in seconds)

  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





CacheImplementation::~CacheImplementation()
{
  FUNCTION_TRACE
  try
  {
    mShutdownRequested = true;
    pthread_join(mThread, nullptr);

    if (mSearchStructurePtr[0])
      delete mSearchStructurePtr[0];

    if (mSearchStructurePtr[1])
      delete mSearchStructurePtr[1];
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}




void CacheImplementation::getCacheStats(Fmi::Cache::CacheStatistics& statistics) const
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::init(T::SessionId sessionId,T::SessionId dataServerSessionId,ServiceInterface *contentStorage)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"* Init start\n");
    mStartTime = time(nullptr);
    mSessionId = sessionId;
    mDataServerSessionId = dataServerSessionId;
    mContentStorage = contentStorage;

    if (mContentStorage != nullptr)
    {
      AutoWriteLock lock(&mModificationLock);

      T::EventInfo eventInfo;
      mUpdateInProgress = true;
      mContentStorage->getLastEventInfo(sessionId,0,eventInfo);

      mLastProcessedEventId = eventInfo.mEventId;
      mContentStorageStartTime = eventInfo.mServerTime;

      mProducerCount = 0xFFFFFFFF;
      mGenerationCount = 0xFFFFFFFF;
      mFileCount = 0xFFFFFFFF;
      mContentCount = 0xFFFFFFFF;

      readProducerList();
      readGenerationList();
      readGeometryList();
      readFileList();

      mFileInfoList.sort(T::FileInfo::ComparisonMethod::fileId);

      readContentList();

      mUpdateInProgress = false;
    }
    updateContent();

    PRINT_DATA(mDebugLog,"* Init end\n");
  }
  catch (...)
  {
    mUpdateInProgress = false;
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}







bool CacheImplementation::isReady()
{
  FUNCTION_TRACE
  try
  {
    if (mContentSwapEnabled)
    {
      if (mContentSwapCounter > 0)
        return true;
      else
        return false;
    }

    if (mStartTime > 0)
      return true;

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::setRequestForwardEnabled(bool enabled)
{
  FUNCTION_TRACE
  try
  {
    mRequestForwardEnabled = enabled;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::setContentUpdateInterval(uint intervalInSec)
{
  FUNCTION_TRACE
  try
  {
    mContentUpdateInterval = intervalInSec;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::getStateAttributes(std::shared_ptr<T::AttributeNode> parent)
{
  FUNCTION_TRACE
  try
  {
    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return;

    AutoReadLock readLock(&mSearchModificationLock);
    auto records = parent->addAttribute("Data Records");
    records->addAttribute("Producer",ssp->mProducerInfoList.getLength());
    records->addAttribute("Generation",ssp->mGenerationInfoList.getLength());
    records->addAttribute("Geometry",ssp->mGeometryInfoList.getLength());
    records->addAttribute("File",ssp->mFileInfoList.getLength());
    records->addAttribute("Content",ssp->mContentInfoList[0].getLength());

    auto events = parent->addAttribute("Events");
    events->addAttribute("Last event id",mLastProcessedEventId);

    auto swap = parent->addAttribute("Content Swap");
    if (!mContentSwapEnabled)
    {
      swap->addAttribute("Enabled","False");
    }
    else
    {
      swap->addAttribute("Enabled","True");
      long diff = time(nullptr) - mContentUpdateTime;
      swap->addAttribute("Last Swap (seconds ago)",diff);
      swap->addAttribute("Swap Interval (seconds)",mContentUpdateInterval);
    }

    ServiceInterface::getStateAttributes(parent);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::setContentSwap(bool enabled,uint fileCacheMaxFirstWaitTime,uint fileCacheMaxWaitTime)
{
  FUNCTION_TRACE
  try
  {
    mContentSwapEnabled = enabled;
    mCachedFiles_maxWaitTime = fileCacheMaxWaitTime;
    mCachedFiles_maxFirstWaitTime = fileCacheMaxFirstWaitTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::setEventListMaxLength(uint maxLength)
{
  FUNCTION_TRACE
  try
  {
    mEventInfoList.setMaxLength(maxLength);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::string& CacheImplementation::getSourceInfo()
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage)
      return mContentStorage->getSourceInfo();

    mSourceInfo = "No source";
    return mSourceInfo;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void CacheImplementation::synchronize()
{
  FUNCTION_TRACE
  try
  {
    T::EventInfo eventInfo;
    mContentStorage->getLastEventInfo(mSessionId,0,eventInfo);
    while (mLastProcessedEventId < eventInfo.mEventId)
      sleep(1);
/*
    while (mFileDeleteCount > 0 || mContentDeleteCount > 0)
    {
      sleep(1);
    }
*/
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void CacheImplementation::startEventProcessing()
{
  FUNCTION_TRACE
  try
  {
    pthread_create(&mThread,nullptr,CacheImplementation_eventProcessingThread,this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::shutdown()
{
  FUNCTION_TRACE
  try
  {
    mShutdownRequested = true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool CacheImplementation::isSessionValid(T::SessionId sessionId)
{
  //FUNCTION_TRACE
  try
  {
    if (mShutdownRequested)
      return false;

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_clear(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->clear(sessionId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_reload(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    mContentStorage->reload(sessionId);

    reloadData();
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::reloadData()
{
  FUNCTION_TRACE
  try
  {
    mReloadActivated = true;
    if (mContentStorage != nullptr)
    {
      AutoWriteLock lock(&mModificationLock);

      T::EventInfo eventInfo;
      mUpdateInProgress = true;
      mContentStorage->getLastEventInfo(0,0,eventInfo);

      mLastProcessedEventId = eventInfo.mEventId;
      //mContentStorageStartTime = eventInfo.mServerTime;

      mProducerCount = 0xFFFFFFFF;
      mGenerationCount = 0xFFFFFFFF;
      mGeometryCount = 0xFFFFFFFF;
      mFileCount = 0xFFFFFFFF;
      mContentCount = 0xFFFFFFFF;

      readProducerList();
      readGenerationList();
      readGeometryList();
      readFileList();

      mFileInfoList.sort(T::FileInfo::ComparisonMethod::fileId);

      readContentList();

      mUpdateInProgress = false;
      mContentUpdateTime = 0;
    }
    mReloadActivated = false;
    updateContent();
  }
  catch (...)
  {
    mReloadActivated = false;
    mUpdateInProgress = false;
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentChangeTime(T::SessionId sessionId,time_t& changeTime)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    changeTime = mContentChangeTime;

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addProducerInfo(sessionId,producerInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_setProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->setProducerInfo(sessionId,producerInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteProducerInfoById(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteProducerInfoById(sessionId,producerId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteProducerInfoByName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

     int result = mContentStorage->deleteProducerInfoByName(sessionId,producerName);
     processEvents(false);
     return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getProducerInfoById(sessionId,producerId,producerInfo);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      if (!mProducerInfoList.getProducerInfoById(producerId,producerInfo))
        return Result::DATA_NOT_FOUND;

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      if (!ssp->mProducerInfoList.getProducerInfoById(producerId,producerInfo))
        return Result::DATA_NOT_FOUND;
    }
    else
    {
      if (!ssp->mProducerInfoList.getProducerInfoById(producerId,producerInfo))
        return Result::DATA_NOT_FOUND;
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteProducerInfoListBySourceId(sessionId,sourceId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getProducerInfoByName(T::SessionId sessionId,const std::string& producerName,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getProducerInfoByName(sessionId,producerName,producerInfo);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      if (!mProducerInfoList.getProducerInfoByName(producerName,producerInfo))
        return Result::DATA_NOT_FOUND;

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      if (!ssp->mProducerInfoList.getProducerInfoByName(producerName,producerInfo))
        return Result::DATA_NOT_FOUND;
    }
    else
    {
      if (!ssp->mProducerInfoList.getProducerInfoByName(producerName,producerInfo))
        return Result::DATA_NOT_FOUND;
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getProducerInfoList(sessionId,producerInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      producerInfoList = mProducerInfoList;
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    producerInfoList.clear();

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      producerInfoList = ssp->mProducerInfoList;
    }
    else
    {
      producerInfoList = ssp->mProducerInfoList;
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getProducerInfoListByParameter(sessionId,parameterKeyType,parameterKey,producerInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    producerInfoList.clear();

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    T::ContentInfoList contentInfoList;

    time_t startTime = 0;
    time_t endTime = 0xFFFFFFFF;

    ModificationLock *mlock = &mSearchModificationLock;
    if (mContentSwapEnabled)
      mlock = nullptr;

    AutoReadLock readLock(mlock);

    T::FmiParamId fmiId = 0;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        fmiId = toUInt32(parameterKey);
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByFmiName(parameterKey);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseName(parameterKey);
        break;

      case T::ParamKeyTypeValue::NETCDF_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNetCdfName(parameterKey);
        break;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"FMI-",4) == 0)
      fmiId = toUInt32(parameterKey.c_str()+4);
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"NB-",3) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey.c_str()+3));
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"GRIB-",5) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey.c_str()+5));
    else
    if (fmiId == 0)
      return Result::UNKNOWN_PARAMETER_NAME;

    ssp->mContentInfoList[1].getContentInfoListByFmiParameterId(fmiId,-1,-1000000000,1000000000,-2,-2,-2,startTime,endTime,0,contentInfoList);

    std::set<uint> producerIdList;
    uint len = contentInfoList.getLength();
    uint prevProducerId = 0;
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      if (info != nullptr &&  prevProducerId != info->mProducerId  &&  producerIdList.find(info->mProducerId) == producerIdList.end())
      {
        prevProducerId = info->mProducerId;
        producerIdList.insert(info->mProducerId);

        T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoById(info->mProducerId);
        if (producerInfo != nullptr)
          producerInfoList.addProducerInfo(producerInfo->duplicate());
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int CacheImplementation::_getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getProducerInfoListBySourceId(sessionId,sourceId,producerInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    producerInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mProducerInfoList.getProducerInfoListBySourceId(sourceId,producerInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      ssp->mProducerInfoList.getProducerInfoListBySourceId(sourceId,producerInfoList);
    }
    else
    {
      ssp->mProducerInfoList.getProducerInfoListBySourceId(sourceId,producerInfoList);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getProducerInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getProducerInfoCount(sessionId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      count = mProducerInfoList.getLength();
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      count = ssp->mProducerInfoList.getLength();
    }
    else
    {
      count = ssp->mProducerInfoList.getLength();
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getProducerNameAndGeometryList(sessionId,list);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    list.clear();

    ModificationLock *mlock = &mSearchModificationLock;
    if (mContentSwapEnabled)
      mlock = nullptr;

    AutoReadLock readLock(mlock);

    uint pLen = ssp->mProducerInfoList.getLength();
    for (uint p=0; p<pLen; p++)
    {
      T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoByIndex(p);
      if (producerInfo != nullptr)
      {
        std::set<uint> geometryIdList;

        uint len = ssp->mContentInfoList[0].getLength();
        for (uint t=0; t<len; t++)
        {
          T::ContentInfo *contentInfo = ssp->mContentInfoList[0].getContentInfoByIndex(t);
          if (contentInfo != nullptr && producerInfo->mProducerId == contentInfo->mProducerId  &&  geometryIdList.find(contentInfo->mGeometryId) == geometryIdList.end())
          {
            char tmp[100];
            sprintf(tmp,"%s;%u",producerInfo->mName.c_str(),contentInfo->mGeometryId);
            list.insert(std::string(tmp));
            geometryIdList.insert(contentInfo->mGeometryId);
          }
        }
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getProducerParameterList(sessionId,sourceParameterKeyType,targetParameterKeyType,list);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    list.clear();

    ModificationLock *mlock = &mSearchModificationLock;
    if (mContentSwapEnabled)
      mlock = nullptr;

    AutoReadLock readLock(mlock);

    T::ProducerInfoList producerList = ssp->mProducerInfoList;
    std::map<uint,T::ProducerInfo*> producers;
    uint plen = producerList.getLength();
    for (uint t=0; t<plen; t++)
    {
      T::ProducerInfo *prod = producerList.getProducerInfoByIndex(t);
      if (prod != nullptr)
        producers.insert(std::pair<uint,T::ProducerInfo*>(prod->mProducerId,prod));
    }

    std::set<std::size_t> tmpList;

    uint len = ssp->mContentInfoList[0].getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = ssp->mContentInfoList[0].getContentInfoByIndex(t);
      if (contentInfo != nullptr)
      {
        std::string sourceParamKey;
        std::string targetParamKey;
        T::ParamLevelId paramLevelId = contentInfo->mFmiParameterLevelId;

        auto fmiId = contentInfo->mFmiParameterId;

        switch (sourceParameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            sourceParamKey = std::to_string(contentInfo->mFmiParameterId);
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            sourceParamKey = contentInfo->getFmiParameterName();
            if (sourceParamKey.empty())
              sourceParamKey = "FMI-" + toString(contentInfo->mFmiParameterId);
            break;

          case T::ParamKeyTypeValue::GRIB_ID:
          {
            Identification::FmiParameterId_grib def;
            if (Identification::gridDef.getGribParameterMappingByFmiId(fmiId,def))
              sourceParamKey = std::to_string(def.mGribParameterId);
          }
          break;

          case T::ParamKeyTypeValue::NEWBASE_ID:
          {
            Identification::FmiParameterId_newbase def;
            if (Identification::gridDef.getNewbaseParameterMappingByFmiId(fmiId,def))
              sourceParamKey = std::to_string(def.mNewbaseParameterId);
          }
          break;

          case T::ParamKeyTypeValue::NEWBASE_NAME:
          {
            Identification::NewbaseParameterDef def;
            if (Identification::gridDef.getNewbaseParameterDefByFmiId(fmiId,def))
              sourceParamKey = def.mParameterName;
          }
          break;

          case T::ParamKeyTypeValue::NETCDF_NAME:
          {
            Identification::FmiParameterId_netCdf def;
            if (Identification::gridDef.getNetCdfParameterMappingByFmiId(fmiId,def))
              sourceParamKey = def.mNetCdfParameterName;
          }
          break;

          default:
            break;
        }


        switch (targetParameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            targetParamKey = std::to_string(contentInfo->mFmiParameterId);
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            targetParamKey = contentInfo->getFmiParameterName();
            if (targetParamKey.empty())
              targetParamKey = "FMI-" + toString(contentInfo->mFmiParameterId);
            break;

          case T::ParamKeyTypeValue::GRIB_ID:
          {
            Identification::FmiParameterId_grib def;
            if (Identification::gridDef.getGribParameterMappingByFmiId(fmiId,def))
              targetParamKey = std::to_string(def.mGribParameterId);
          }
          break;

          case T::ParamKeyTypeValue::NEWBASE_ID:
          {
            Identification::FmiParameterId_newbase def;
            if (Identification::gridDef.getNewbaseParameterMappingByFmiId(fmiId,def))
              targetParamKey = std::to_string(def.mNewbaseParameterId);
          }
          break;

          case T::ParamKeyTypeValue::NEWBASE_NAME:
          {
            Identification::NewbaseParameterDef def;
            if (Identification::gridDef.getNewbaseParameterDefByFmiId(fmiId,def))
              targetParamKey = def.mParameterName;
          }
          break;

          case T::ParamKeyTypeValue::NETCDF_NAME:
          {
            Identification::FmiParameterId_netCdf def;
            if (Identification::gridDef.getNetCdfParameterMappingByFmiId(fmiId,def))
              targetParamKey = def.mNetCdfParameterName;
          }
          break;

          default:
            break;
        }

        if (!sourceParamKey.empty()  &&  !targetParamKey.empty())
        {
          std::size_t seed = 0;
          boost::hash_combine(seed,contentInfo->mProducerId);
          boost::hash_combine(seed,sourceParamKey);
          boost::hash_combine(seed,targetParameterKeyType);
          boost::hash_combine(seed,targetParamKey);
          boost::hash_combine(seed,contentInfo->mGeometryId);
          boost::hash_combine(seed,paramLevelId);
          boost::hash_combine(seed,contentInfo->mParameterLevel);
          boost::hash_combine(seed,contentInfo->mForecastType);
          boost::hash_combine(seed,contentInfo->mForecastNumber);


          if (tmpList.find(seed) == tmpList.end())
          {
            tmpList.insert(seed);

            auto it = producers.find(contentInfo->mProducerId);
            if (it != producers.end())
            {
              char tmp[200];
              char *p = tmp;
              p += sprintf(p,"%s;%s;%d;%s;%d;;%d;%05d;%d;%d",
                    it->second->mName.c_str(),
                    sourceParamKey.c_str(),
                    targetParameterKeyType,
                    targetParamKey.c_str(),
                    contentInfo->mGeometryId,
                    //paramLevelIdType,
                    paramLevelId,
                    contentInfo->mParameterLevel,
                    contentInfo->mForecastType,
                    contentInfo->mForecastNumber);

              if ((contentInfo->mFlags & T::ContentInfo::Flags::PreloadRequired) != 0)
                p += sprintf(p,";1");
              else
                p += sprintf(p,";0");

              list.insert(std::string(tmp));
            }
          }
        }
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getProducerParameterListByProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getProducerParameterList(sessionId,sourceParameterKeyType,targetParameterKeyType,list);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    list.clear();

    ModificationLock *mlock = &mSearchModificationLock;
    if (mContentSwapEnabled)
      mlock = nullptr;

    AutoReadLock readLock(mlock);

    T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    std::set<std::size_t> tmpList;

    uint len = ssp->mContentInfoList[0].getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = ssp->mContentInfoList[0].getContentInfoByIndex(t);
      if (contentInfo != nullptr && contentInfo->mProducerId == producerId)
      {
        std::string sourceParamKey;
        std::string targetParamKey;
        T::ParamLevelId paramLevelId = contentInfo->mFmiParameterLevelId;
        auto fmiId = contentInfo->mFmiParameterId;

        switch (sourceParameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            sourceParamKey = std::to_string(contentInfo->mFmiParameterId);
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            sourceParamKey = contentInfo->getFmiParameterName();
            if (sourceParamKey.empty())
              sourceParamKey = "FMI-" + toString(contentInfo->mFmiParameterId);
            break;

          case T::ParamKeyTypeValue::GRIB_ID:
          {
            Identification::FmiParameterId_grib def;
            if (Identification::gridDef.getGribParameterMappingByFmiId(fmiId,def))
              sourceParamKey = std::to_string(def.mGribParameterId);
          }
          break;

          case T::ParamKeyTypeValue::NEWBASE_ID:
          {
            Identification::FmiParameterId_newbase def;
            if (Identification::gridDef.getNewbaseParameterMappingByFmiId(fmiId,def))
              sourceParamKey = std::to_string(def.mNewbaseParameterId);
          }
          break;

          case T::ParamKeyTypeValue::NEWBASE_NAME:
          {
            Identification::NewbaseParameterDef def;
            if (Identification::gridDef.getNewbaseParameterDefByFmiId(fmiId,def))
              sourceParamKey = def.mParameterName;
          }
          break;

          case T::ParamKeyTypeValue::NETCDF_NAME:
          {
            Identification::FmiParameterId_netCdf def;
            if (Identification::gridDef.getNetCdfParameterMappingByFmiId(fmiId,def))
              sourceParamKey = def.mNetCdfParameterName;
          }
          break;

          default:
            break;
        }


        switch (targetParameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            targetParamKey = std::to_string(contentInfo->mFmiParameterId);
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            targetParamKey = contentInfo->getFmiParameterName();
            if (targetParamKey.empty())
              targetParamKey = "FMI-" + toString(contentInfo->mFmiParameterId);
            break;

          case T::ParamKeyTypeValue::GRIB_ID:
          {
            Identification::FmiParameterId_grib def;
            if (Identification::gridDef.getGribParameterMappingByFmiId(fmiId,def))
              targetParamKey = std::to_string(def.mGribParameterId);
          }
          break;

          case T::ParamKeyTypeValue::NEWBASE_ID:
          {
            Identification::FmiParameterId_newbase def;
            if (Identification::gridDef.getNewbaseParameterMappingByFmiId(fmiId,def))
              targetParamKey = std::to_string(def.mNewbaseParameterId);
          }
          break;

          case T::ParamKeyTypeValue::NEWBASE_NAME:
          {
            Identification::NewbaseParameterDef def;
            if (Identification::gridDef.getNewbaseParameterDefByFmiId(fmiId,def))
              targetParamKey = def.mParameterName;
          }
          break;

          case T::ParamKeyTypeValue::NETCDF_NAME:
          {
            Identification::FmiParameterId_netCdf def;
            if (Identification::gridDef.getNetCdfParameterMappingByFmiId(fmiId,def))
              targetParamKey = def.mNetCdfParameterName;
          }
          break;

          default:
            break;
        }

        if (!sourceParamKey.empty()  &&  !targetParamKey.empty())
        {
          std::size_t seed = 0;
          boost::hash_combine(seed,contentInfo->mProducerId);
          boost::hash_combine(seed,sourceParamKey);
          boost::hash_combine(seed,targetParameterKeyType);
          boost::hash_combine(seed,targetParamKey);
          boost::hash_combine(seed,contentInfo->mGeometryId);
          boost::hash_combine(seed,paramLevelId);
          boost::hash_combine(seed,contentInfo->mParameterLevel);
          boost::hash_combine(seed,contentInfo->mForecastType);
          boost::hash_combine(seed,contentInfo->mForecastNumber);


          if (tmpList.find(seed) == tmpList.end())
          {
            tmpList.insert(seed);

            char tmp[200];
            char *p = tmp;
            p += sprintf(p,"%s;%s;%d;%s;%d;1;%d;%06d;%d;%d",
                  producerInfo->mName.c_str(),
                  sourceParamKey.c_str(),
                  targetParameterKeyType,
                  targetParamKey.c_str(),
                  contentInfo->mGeometryId,
                  //paramLevelIdType,
                  paramLevelId,
                  contentInfo->mParameterLevel,
                  contentInfo->mForecastType,
                  contentInfo->mForecastNumber);

            if ((contentInfo->mFlags & T::ContentInfo::Flags::PreloadRequired) != 0)
              p += sprintf(p,";1");
            else
              p += sprintf(p,";0");

            list.insert(std::string(tmp));
          }
        }
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addGenerationInfo(sessionId,generationInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_setGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->setGenerationInfo(sessionId,generationInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteGenerationInfoById(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteGenerationInfoById(sessionId,generationId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteGenerationInfoByName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteGenerationInfoByName(sessionId,generationName);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<uint>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteGenerationInfoListByIdList(sessionId,generationIdList);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteGenerationInfoListByProducerId(sessionId,producerId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteGenerationInfoListByProducerName(sessionId,producerName);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteGenerationInfoListBySourceId(sessionId,sourceId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getGenerationIdGeometryIdAndForecastTimeList(sessionId,list);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    list.clear();

    ModificationLock *mlock = &mSearchModificationLock;
    if (mContentSwapEnabled)
      mlock = nullptr;

    AutoReadLock readLock(mlock);

    uint len = ssp->mContentInfoList[0].getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = ssp->mContentInfoList[0].getContentInfoByIndex(t);
      if (info != nullptr)
      {
        char st[200];
        sprintf(st,"%u;%u;%u;%d;%d;%s;%ld;%ld;",info->mSourceId,info->mGenerationId,info->mGeometryId,info->mForecastType,info->mForecastNumber,info->getForecastTime(),info->mModificationTime,info->mDeletionTime);
        std::string str = st;
        if (list.find(str) == list.end())
          list.insert(str);
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getGenerationInfoListByGeometryId(sessionId,geometryId,generationInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    generationInfoList.clear();

    std::set<uint> idList;

    ModificationLock *mlock = &mSearchModificationLock;
    if (mContentSwapEnabled)
      mlock = nullptr;

    AutoReadLock readLock(mlock);

    ssp->mContentInfoList[0].getGenerationIdListByGeometryId(geometryId,idList);

    for (auto it=idList.begin(); it!=idList.end(); ++it)
    {
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(*it);
      if (generationInfo != nullptr)
        generationInfoList.addGenerationInfo(generationInfo->duplicate());
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getGenerationInfoById(sessionId,generationId,generationInfo);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      if (!mGenerationInfoList.getGenerationInfoById(generationId,generationInfo))
        return Result::DATA_NOT_FOUND;

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      if (!ssp->mGenerationInfoList.getGenerationInfoById(generationId,generationInfo))
        return Result::DATA_NOT_FOUND;
    }
    else
    {
      if (!ssp->mGenerationInfoList.getGenerationInfoById(generationId,generationInfo))
        return Result::DATA_NOT_FOUND;
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getGenerationInfoByName(T::SessionId sessionId,const std::string& generationName,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getGenerationInfoByName(sessionId,generationName,generationInfo);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      if (!mGenerationInfoList.getGenerationInfoByName(generationName,generationInfo))
        return Result::DATA_NOT_FOUND;

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      if (!ssp->mGenerationInfoList.getGenerationInfoByName(generationName,generationInfo))
        return Result::DATA_NOT_FOUND;
    }
    else
    {
      if (!ssp->mGenerationInfoList.getGenerationInfoByName(generationName,generationInfo))
        return Result::DATA_NOT_FOUND;
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getGenerationInfoList(sessionId,generationInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    generationInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      generationInfoList = mGenerationInfoList;
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;


    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      generationInfoList = ssp->mGenerationInfoList;
    }
    else
    {
      generationInfoList = ssp->mGenerationInfoList;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getGenerationInfoListByProducerId(sessionId,producerId,generationInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    generationInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mGenerationInfoList.getGenerationInfoListByProducerId(producerId,generationInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      ssp->mGenerationInfoList.getGenerationInfoListByProducerId(producerId,generationInfoList);
    }
    else
    {
      ssp->mGenerationInfoList.getGenerationInfoListByProducerId(producerId,generationInfoList);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

   if (mUpdateInProgress)
      return mContentStorage->getGenerationInfoListByProducerName(sessionId,producerName,generationInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    generationInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
      if (producerInfo == nullptr)
        return Result::UNKNOWN_PRODUCER_NAME;

      mGenerationInfoList.getGenerationInfoListByProducerId(producerInfo->mProducerId,generationInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);

      T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoByName(producerName);
      if (producerInfo == nullptr)
        return Result::UNKNOWN_PRODUCER_NAME;

      ssp->mGenerationInfoList.getGenerationInfoListByProducerId(producerInfo->mProducerId,generationInfoList);
    }
    else
    {
      T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoByName(producerName);
      if (producerInfo == nullptr)
        return Result::UNKNOWN_PRODUCER_NAME;

      ssp->mGenerationInfoList.getGenerationInfoListByProducerId(producerInfo->mProducerId,generationInfoList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getGenerationInfoListBySourceId(sessionId,sourceId,generationInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    generationInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mGenerationInfoList.getGenerationInfoListBySourceId(sourceId,generationInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      ssp->mGenerationInfoList.getGenerationInfoListBySourceId(sourceId,generationInfoList);
    }
    else
    {
      ssp->mGenerationInfoList.getGenerationInfoListBySourceId(sourceId,generationInfoList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getLastGenerationInfoByProducerIdAndStatus(sessionId,producerId,generationStatus,generationInfo);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    AutoReadLock readLock(&mSearchModificationLock);
/*
    T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;
*/
    if (!ssp->mGenerationInfoList.getLastGenerationInfoByProducerIdAndStatus(producerId,generationStatus,generationInfo))
      return Result::DATA_NOT_FOUND;

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,const std::string& producerName,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getLastGenerationInfoByProducerNameAndStatus(sessionId,producerName,generationStatus,generationInfo);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    AutoReadLock readLock(&mSearchModificationLock);

    T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    if (!ssp->mGenerationInfoList.getLastGenerationInfoByProducerIdAndStatus(producerInfo->mProducerId,generationStatus,generationInfo))
      return Result::DATA_NOT_FOUND;

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getGenerationInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getGenerationInfoCount(sessionId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      count = mGenerationInfoList.getLength();
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      count = ssp->mGenerationInfoList.getLength();
    }
    else
    {
      count = ssp->mGenerationInfoList.getLength();
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,uchar status)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->setGenerationInfoStatusById(sessionId,generationId,status);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_setGenerationInfoStatusByName(T::SessionId sessionId,const std::string& generationName,uchar status)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->setGenerationInfoStatusByName(sessionId,generationName,status);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_addGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addGeometryInfo(sessionId,geometryInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int CacheImplementation::_deleteGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteGeometryInfoById(sessionId,generationId,geometryId,levelId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteGeometryInfoListByGenerationId(sessionId,generationId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteGeometryInfoListByProducerId(sessionId,producerId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteGeometryInfoListBySourceId(sessionId,sourceId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getGeometryInfoById(sessionId,generationId,geometryId,levelId,geometryInfo);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      if (!mGeometryInfoList.getGeometryInfoById(generationId,geometryId,levelId,geometryInfo))
        return Result::DATA_NOT_FOUND;

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      if (!ssp->mGeometryInfoList.getGeometryInfoById(generationId,geometryId,levelId,geometryInfo))
        return Result::DATA_NOT_FOUND;
    }
    else
    {
      if (!ssp->mGeometryInfoList.getGeometryInfoById(generationId,geometryId,levelId,geometryInfo))
        return Result::DATA_NOT_FOUND;
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int CacheImplementation::_getGeometryInfoList(T::SessionId sessionId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getGeometryInfoList(sessionId,geometryInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    geometryInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      geometryInfoList = mGeometryInfoList;
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      geometryInfoList = ssp->mGeometryInfoList;
    }
    else
    {
      geometryInfoList = ssp->mGeometryInfoList;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getGeometryInfoListByGenerationId(sessionId,generationId,geometryInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    geometryInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mGeometryInfoList.getGeometryInfoListByGenerationId(generationId,geometryInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      ssp->mGeometryInfoList.getGeometryInfoListByGenerationId(generationId,geometryInfoList);
    }
    else
    {
      ssp->mGeometryInfoList.getGeometryInfoListByGenerationId(generationId,geometryInfoList);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int CacheImplementation::_getGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getGeometryInfoListByProducerId(sessionId,producerId,geometryInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    geometryInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mGeometryInfoList.getGeometryInfoListByProducerId(producerId,geometryInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      ssp->mGeometryInfoList.getGeometryInfoListByProducerId(producerId,geometryInfoList);
    }
    else
    {
      ssp->mGeometryInfoList.getGeometryInfoListByProducerId(producerId,geometryInfoList);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getGeometryInfoListBySourceId(sessionId,sourceId,geometryInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    geometryInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mGeometryInfoList.getGeometryInfoListBySourceId(sourceId,geometryInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      ssp->mGeometryInfoList.getGeometryInfoListBySourceId(sourceId,geometryInfoList);
    }
    else
    {
      ssp->mGeometryInfoList.getGeometryInfoListBySourceId(sourceId,geometryInfoList);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getGeometryInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getGeometryInfoCount(sessionId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      count = mGeometryInfoList.getLength();
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      count = ssp->mGeometryInfoList.getLength();
    }
    else
    {
      count = ssp->mGeometryInfoList.getLength();
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_setGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->setGeometryInfo(sessionId,geometryInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_setGeometryInfoStatusById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,uchar status)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->setGeometryInfoStatusById(sessionId,generationId,geometryId,levelId,status);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addFileInfo(sessionId,fileInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_setFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->setFileInfo(sessionId,fileInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    return mContentStorage->addFileInfoWithContentList(sessionId,fileInfo,contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    return mContentStorage->addFileInfoListWithContent(sessionId,requestFlags,fileAndContentList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteFileInfoById(T::SessionId sessionId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoById(sessionId,fileId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteFileInfoByName(T::SessionId sessionId,const std::string& filename)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoByName(sessionId,filename);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListByProducerId(sessionId,producerId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListByProducerName(sessionId,producerName);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListByGenerationId(sessionId,generationId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,time_t forecastTime)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListByGenerationIdAndForecastTime(sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListByForecastTimeList(sessionId,forecastTimeList);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListByGenerationName(sessionId,generationName);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListBySourceId(sessionId,sourceId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListByFileIdList(sessionId,fileIdList);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getFileInfoById(sessionId,fileId,fileInfo);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      if (!mFileInfoList.getFileInfoById(fileId,fileInfo))
        return Result::DATA_NOT_FOUND;

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      if (!ssp->mFileInfoList.getFileInfoById(fileId,fileInfo))
        return Result::DATA_NOT_FOUND;
    }
    else
    {
      if (!ssp->mFileInfoList.getFileInfoById(fileId,fileInfo))
        return Result::DATA_NOT_FOUND;
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getFileInfoByName(T::SessionId sessionId,const std::string& filename,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getFileInfoByName(sessionId,filename,fileInfo);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      if (!mFileInfoList.getFileInfoByName(filename,fileInfo))
        return Result::DATA_NOT_FOUND;

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      if (!ssp->mFileInfoListByName.getFileInfoByName(filename,fileInfo))
        return Result::DATA_NOT_FOUND;
    }
    else
    {
      if (!ssp->mFileInfoListByName.getFileInfoByName(filename,fileInfo))
        return Result::DATA_NOT_FOUND;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getFileInfoList(T::SessionId sessionId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getFileInfoList(sessionId,startFileId,maxRecords,fileInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    fileInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mFileInfoList.getFileInfoList(startFileId,maxRecords,fileInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      ssp->mFileInfoList.getFileInfoList(startFileId,maxRecords,fileInfoList);
    }
    else
    {
      ssp->mFileInfoList.getFileInfoList(startFileId,maxRecords,fileInfoList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    fileInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      for (auto it = fileIdList.begin(); it != fileIdList.end(); ++it)
      {
        T::FileInfo *info = mFileInfoList.getFileInfoById(*it);
        if (info != nullptr)
          fileInfoList.addFileInfo(info->duplicate());
      }
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    AutoReadLock readLock(&mSearchModificationLock);

    for (auto it = fileIdList.begin(); it != fileIdList.end(); ++it)
    {
      T::FileInfo *info = ssp->mFileInfoList.getFileInfoById(*it);
      if (info != nullptr)
        fileInfoList.addFileInfo(info->duplicate());
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int CacheImplementation::_getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getFileInfoListByProducerId(sessionId,producerId,startFileId,maxRecords,fileInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mFileInfoList.getFileInfoListByProducerId(producerId,startFileId,maxRecords,fileInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    AutoReadLock readLock(&mSearchModificationLock);
/*
    T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;
*/
    fileInfoList.clear();

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      ssp->mFileInfoList.getFileInfoListByProducerId(producerId,startFileId,maxRecords,fileInfoList);
    }
    else
    {
      ssp->mFileInfoList.getFileInfoListByProducerId(producerId,startFileId,maxRecords,fileInfoList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getFileInfoListByProducerName(sessionId,producerName,startFileId,maxRecords,fileInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
      if (producerInfo == nullptr)
        return Result::UNKNOWN_PRODUCER_NAME;

      mFileInfoList.getFileInfoListByProducerId(producerInfo->mProducerId,startFileId,maxRecords,fileInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    fileInfoList.clear();

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoByName(producerName);
      if (producerInfo == nullptr)
        return Result::UNKNOWN_PRODUCER_NAME;

      ssp->mFileInfoList.getFileInfoListByProducerId(producerInfo->mProducerId,startFileId,maxRecords,fileInfoList);
    }
    else
    {
      T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoByName(producerName);
      if (producerInfo == nullptr)
        return Result::UNKNOWN_PRODUCER_NAME;

      ssp->mFileInfoList.getFileInfoListByProducerId(producerInfo->mProducerId,startFileId,maxRecords,fileInfoList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getFileInfoListByGenerationId(sessionId,generationId,startFileId,maxRecords,fileInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    fileInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mFileInfoList.getFileInfoListByGenerationId(generationId,startFileId,maxRecords,fileInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      ssp->mFileInfoList.getFileInfoListByGenerationId(generationId,startFileId,maxRecords,fileInfoList);
    }
    else
    {
      ssp->mFileInfoList.getFileInfoListByGenerationId(generationId,startFileId,maxRecords,fileInfoList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getFileInfoListByGenerationName(sessionId,generationName,startFileId,maxRecords,fileInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    fileInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_NAME;

      mFileInfoList.getFileInfoListByGenerationId(generationInfo->mGenerationId,startFileId,maxRecords,fileInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoByName(generationName);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_NAME;

      ssp->mFileInfoList.getFileInfoListByGenerationId(generationInfo->mGenerationId,startFileId,maxRecords,fileInfoList);
    }
    else
    {
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoByName(generationName);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_NAME;

      ssp->mFileInfoList.getFileInfoListByGenerationId(generationInfo->mGenerationId,startFileId,maxRecords,fileInfoList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getFileInfoListBySourceId(sessionId,sourceId,startFileId,maxRecords,fileInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    fileInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mFileInfoList.getFileInfoListBySourceId(sourceId,startFileId,maxRecords,fileInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      ssp->mFileInfoList.getFileInfoListBySourceId(sourceId,startFileId,maxRecords,fileInfoList);
    }
    else
    {
      ssp->mFileInfoList.getFileInfoListBySourceId(sourceId,startFileId,maxRecords,fileInfoList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getFileInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getFileInfoCount(sessionId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      count = mFileInfoList.getLength();
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      count = ssp->mFileInfoList.getLength();
    }
    else
    {
      count = ssp->mFileInfoList.getLength();
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getFileInfoCountByProducerId(T::SessionId sessionId,uint producerId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getFileInfoCountByProducerId(sessionId,producerId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      count = mFileInfoList.getFileInfoCountByProducerId(producerId);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      count = ssp->mFileInfoList.getFileInfoCountByProducerId(producerId);
    }
    else
    {
      count = ssp->mFileInfoList.getFileInfoCountByProducerId(producerId);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int CacheImplementation::_getFileInfoCountByGenerationId(T::SessionId sessionId,uint generationId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getFileInfoCountByGenerationId(sessionId,generationId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      count = mFileInfoList.getFileInfoCountByGenerationId(generationId);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      count = ssp->mFileInfoList.getFileInfoCountByGenerationId(generationId);
    }
    else
    {
      count = ssp->mFileInfoList.getFileInfoCountByGenerationId(generationId);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getFileInfoCountBySourceId(T::SessionId sessionId,uint sourceId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getFileInfoCountBySourceId(sessionId,sourceId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      count = mFileInfoList.getFileInfoCountBySourceId(sourceId);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      count = ssp->mFileInfoList.getFileInfoCountBySourceId(sourceId);
    }
    else
    {
      count = ssp->mFileInfoList.getFileInfoCountBySourceId(sourceId);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addEventInfo(sessionId,eventInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  //FUNCTION_TRACE
  try
  {
    eventInfo.mServerTime = mContentStorageStartTime;

    if (mReloadActivated)
      return Result::DATA_NOT_FOUND;

    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getLastEventInfo(sessionId,requestingServerId,eventInfo);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::EventInfo *lastEvent = mEventInfoList.getLastEvent();
    if (lastEvent == nullptr)
      return Result::DATA_NOT_FOUND;

    eventInfo = *lastEvent;

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList)
{
  //FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getEventInfoList(sessionId,requestingServerId,startEventId,maxRecords,eventInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    eventInfoList.clear();

    mEventInfoList.getEventInfoList(startEventId,maxRecords,eventInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getEventInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getEventInfoCount(sessionId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mEventInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addContentInfo(sessionId,contentInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_setContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->setContentInfo(sessionId,contentInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addContentList(sessionId,contentInfoList);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentInfo(sessionId,fileId,messageIndex);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteContentListByFileId(T::SessionId sessionId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentListByFileId(sessionId,fileId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteContentListByFileName(T::SessionId sessionId,const std::string& filename)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentListByFileName(sessionId,filename);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteContentListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentListByProducerId(sessionId,producerId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteContentListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentListByProducerName(sessionId,producerName);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteContentListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentListByGenerationId(sessionId,generationId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteContentListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentListByGenerationName(sessionId,generationName);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_deleteContentListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentListBySourceId(sessionId,sourceId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentInfo(sessionId,fileId,messageIndex,contentInfo);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      T::ContentInfo *cInfo = mContentInfoList.getContentInfoByFileIdAndMessageIndex(fileId,messageIndex);
      if (cInfo != nullptr)
      {
        contentInfo = *cInfo;
        return Result::OK;
      }
      return Result::DATA_NOT_FOUND;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::ContentInfo *cInfo = ssp->mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(fileId,messageIndex);
      if (cInfo != nullptr)
      {
        contentInfo = *cInfo;
        return Result::OK;
      }
    }
    else
    {
      T::ContentInfo *cInfo = ssp->mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(fileId,messageIndex);
      if (cInfo != nullptr)
      {
        contentInfo = *cInfo;
        return Result::OK;
      }
    }

    return Result::DATA_NOT_FOUND;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentList(sessionId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mContentInfoList.getContentInfoList(startFileId,startMessageIndex,maxRecords,contentInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;


    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      ssp->mContentInfoList[0].getContentInfoList(startFileId,startMessageIndex,maxRecords,contentInfoList);
    }
    else
    {
      ssp->mContentInfoList[0].getContentInfoList(startFileId,startMessageIndex,maxRecords,contentInfoList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByFileId(sessionId,fileId,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mContentInfoList.getContentInfoListByFileId(fileId,contentInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      ssp->mContentInfoList[0].getContentInfoListByFileId(fileId,contentInfoList);
    }
    else
    {
      ssp->mContentInfoList[0].getContentInfoListByFileId(fileId,contentInfoList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByFileIdList(sessionId,fileIdList,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      for (auto it = fileIdList.begin(); it != fileIdList.end(); ++it)
      {
        T::ContentInfoList contentList;
        mContentInfoList.getContentInfoListByFileId(*it,contentList);
        contentInfoList.addContentInfoList(contentList);
      }
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ModificationLock *mlock = &mSearchModificationLock;
    if (mContentSwapEnabled)
      mlock = nullptr;

    AutoReadLock readLock(mlock);

    for (auto it = fileIdList.begin(); it != fileIdList.end(); ++it)
    {
      T::ContentInfoList contentList;
      ssp->mContentInfoList[0].getContentInfoListByFileId(*it,contentList);
      contentInfoList.addContentInfoList(contentList);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentListByFileName(T::SessionId sessionId,const std::string& filename,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByFileName(sessionId,filename,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      T::FileInfo *fileInfo = mFileInfoList.getFileInfoByName(filename);
      if (fileInfo == nullptr)
        return Result::UNKNOWN_FILE_NAME;

      mContentInfoList.getContentInfoListByFileId(fileInfo->mFileId,contentInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);

      T::FileInfo *fileInfo = ssp->mFileInfoListByName.getFileInfoByName(filename);
      if (fileInfo == nullptr)
        return Result::UNKNOWN_FILE_NAME;

      ssp->mContentInfoList[0].getContentInfoListByFileId(fileInfo->mFileId,contentInfoList);
    }
    else
    {
      T::FileInfo *fileInfo = ssp->mFileInfoListByName.getFileInfoByName(filename);
      if (fileInfo == nullptr)
        return Result::UNKNOWN_FILE_NAME;

      ssp->mContentInfoList[0].getContentInfoListByFileId(fileInfo->mFileId,contentInfoList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByProducerId(sessionId,producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mContentInfoList.getContentInfoListByProducerId(producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;
/*
    T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;
*/

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      ssp->mContentInfoList[0].getContentInfoListByProducerId(producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    }
    else
    {
      ssp->mContentInfoList[0].getContentInfoListByProducerId(producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByProducerName(sessionId,producerName,startFileId,startMessageIndex,maxRecords,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
      if (producerInfo == nullptr)
        return Result::UNKNOWN_PRODUCER_NAME;

      mContentInfoList.getContentInfoListByProducerId(producerInfo->mProducerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoByName(producerName);
      if (producerInfo == nullptr)
        return Result::UNKNOWN_PRODUCER_NAME;

      ssp->mContentInfoList[0].getContentInfoListByProducerId(producerInfo->mProducerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    }
    else
    {
      T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoByName(producerName);
      if (producerInfo == nullptr)
        return Result::UNKNOWN_PRODUCER_NAME;

      ssp->mContentInfoList[0].getContentInfoListByProducerId(producerInfo->mProducerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int CacheImplementation::_getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,int maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByGenerationId(sessionId,generationId,startFileId,startMessageIndex,maxRecords,requestFlags,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      mContentInfoList.getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      if (requestFlags < CONTENT_LIST_COUNT)
        ssp->mContentInfoList[requestFlags].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
      else
        ssp->mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    }
    else
    {
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      if (requestFlags < CONTENT_LIST_COUNT)
        ssp->mContentInfoList[requestFlags].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
      else
        ssp->mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByGenerationName(sessionId,generationName,startFileId,maxRecords,startMessageIndex,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_NAME;

      mContentInfoList.getContentInfoListByGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoByName(generationName);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_NAME;

      ssp->mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    }
    else
    {
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoByName(generationName);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_NAME;

      ssp->mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTime,endTime,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      mContentInfoList.getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startTime,endTime,contentInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      ssp->mContentInfoList[1].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startTime,endTime,contentInfoList);
    }
    else
    {
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      ssp->mContentInfoList[1].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startTime,endTime,contentInfoList);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByGenerationNameAndTimeRange(sessionId,generationName,startTime,endTime,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_NAME;

      mContentInfoList.getContentInfoListByGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,startTime,endTime,contentInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoByName(generationName);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_NAME;

      ssp->mContentInfoList[1].getContentInfoListByGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,startTime,endTime,contentInfoList);
    }
    else
    {
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoByName(generationName);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_NAME;

      ssp->mContentInfoList[1].getContentInfoListByGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,startTime,endTime,contentInfoList);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListBySourceId(sessionId,sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mContentInfoList.getContentInfoListBySourceId(sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      ssp->mContentInfoList[0].getContentInfoListBySourceId(sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    }
    else
    {
      ssp->mContentInfoList[0].getContentInfoListBySourceId(sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {

    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameter(sessionId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    contentInfoList.clear();

    ModificationLock *mlock = &mSearchModificationLock;
    if (mContentSwapEnabled)
      mlock = nullptr;

    AutoReadLock readLock(mlock);

    T::FmiParamId fmiId = 0;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        fmiId = toUInt32(parameterKey);
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByFmiName(parameterKey);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseName(parameterKey);
        break;

      case T::ParamKeyTypeValue::NETCDF_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNetCdfName(parameterKey);
        break;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"FMI-",4) == 0)
      fmiId = toUInt32(parameterKey.c_str()+4);
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"NB-",3) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey.c_str()+3));
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"GRIB-",5) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey.c_str()+5));
    else
    if (fmiId == 0)
      return Result::UNKNOWN_PARAMETER_NAME;

    ssp->mContentInfoList[1].getContentInfoListByFmiParameterId(fmiId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ModificationLock *mlock = &mSearchModificationLock;
    if (mContentSwapEnabled)
      mlock = nullptr;

    AutoReadLock readLock(mlock);

    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;


    contentInfoList.clear();

    T::FmiParamId fmiId = 0;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        fmiId = toUInt32(parameterKey);
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByFmiName(parameterKey);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseName(parameterKey);
        break;

      case T::ParamKeyTypeValue::NETCDF_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNetCdfName(parameterKey);
        break;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"FMI-",4) == 0)
      fmiId = toUInt32(parameterKey.c_str()+4);
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"NB-",3) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey.c_str()+3));
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"GRIB-",5) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey.c_str()+5));
    else
    if (fmiId == 0)
      return Result::UNKNOWN_PARAMETER_NAME;

    ssp->mContentInfoList[1].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,fmiId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ModificationLock *mlock = &mSearchModificationLock;
    if (mContentSwapEnabled)
      mlock = nullptr;

    AutoReadLock readLock(mlock);

    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    contentInfoList.clear();

    T::FmiParamId fmiId = 0;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        fmiId = toUInt32(parameterKey);
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByFmiName(parameterKey);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseName(parameterKey);
        break;

      case T::ParamKeyTypeValue::NETCDF_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNetCdfName(parameterKey);
        break;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"FMI-",4) == 0)
      fmiId = toUInt32(parameterKey.c_str()+4);
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"NB-",3) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey.c_str()+3));
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"GRIB-",5) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey.c_str()+5));
    else
    if (fmiId == 0)
      return Result::UNKNOWN_PARAMETER_NAME;

    ssp->mContentInfoList[1].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,fmiId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int CacheImplementation::_getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ModificationLock *mlock = &mSearchModificationLock;
    if (mContentSwapEnabled)
      mlock = nullptr;

    AutoReadLock readLock(mlock);

    T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfoList generationList;
    ssp->mGenerationInfoList.getGenerationInfoListByProducerId(producerId,generationList);

    contentInfoList.clear();

    T::FmiParamId fmiId = 0;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        fmiId = toUInt32(parameterKey);
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByFmiName(parameterKey);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseName(parameterKey);
        break;

      case T::ParamKeyTypeValue::NETCDF_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNetCdfName(parameterKey);
        break;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"FMI-",4) == 0)
      fmiId = toUInt32(parameterKey.c_str()+4);
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"NB-",3) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey.c_str()+3));
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"GRIB-",5) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey.c_str()+5));
    else
    if (fmiId == 0)
      return Result::UNKNOWN_PARAMETER_NAME;

    ssp->mContentInfoList[1].getContentInfoListByFmiParameterIdAndProducerId(producerInfo->mProducerId,fmiId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int CacheImplementation::_getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ModificationLock *mlock = &mSearchModificationLock;
    if (mContentSwapEnabled)
      mlock = nullptr;

    AutoReadLock readLock(mlock);

    T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    contentInfoList.clear();

    T::FmiParamId fmiId = 0;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        fmiId = toUInt32(parameterKey);
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByFmiName(parameterKey);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseName(parameterKey);
        break;

      case T::ParamKeyTypeValue::NETCDF_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNetCdfName(parameterKey);
        break;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"FMI-",4) == 0)
      fmiId = toUInt32(parameterKey.c_str()+4);
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"NB-",3) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey.c_str()+3));
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"GRIB-",5) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey.c_str()+5));
    else
    if (fmiId == 0)
      return Result::UNKNOWN_PARAMETER_NAME;

    ssp->mContentInfoList[1].getContentInfoListByFmiParameterIdAndProducerId(producerInfo->mProducerId,fmiId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameterGenerationIdAndForecastTime(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ModificationLock *mlock = &mSearchModificationLock;
    if (mContentSwapEnabled)
      mlock = nullptr;

    AutoReadLock readLock(mlock);

    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    contentInfoList.clear();

    time_t startTime = 0;
    time_t endTime = 0xFFFFFFFF;

    T::ParamLevel minLevel = level;
    T::ParamLevel maxLevel = level;
    uint requestFlags = 0;
    T::ContentInfoList contentList;

    T::FmiParamId fmiId = 0;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        fmiId = toUInt32(parameterKey);
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByFmiName(parameterKey);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey));
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNewbaseName(parameterKey);
        break;

      case T::ParamKeyTypeValue::NETCDF_NAME:
        fmiId = Identification::gridDef.getFmiParameterIdByNetCdfName(parameterKey);
        break;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"FMI-",4) == 0)
      fmiId = toUInt32(parameterKey.c_str()+4);
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"NB-",3) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByNewbaseId(toUInt32(parameterKey.c_str()+3));
    else
    if (fmiId == 0  &&  strncasecmp(parameterKey.c_str(),"GRIB-",5) == 0)
      fmiId = Identification::gridDef.getFmiParameterIdByGribId(toUInt32(parameterKey.c_str()+5));
    else
    if (fmiId == 0)
      return Result::UNKNOWN_PARAMETER_NAME;

    T::ContentInfo *cInfo = ssp->mContentInfoList[1].getContentInfoByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,fmiId,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime);
    if (cInfo != nullptr)
    {
      contentInfoList.addContentInfo(cInfo->duplicate());
      return Result::OK;
    }
    ssp->mContentInfoList[1].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,fmiId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
    contentInfoList = contentList;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ModificationLock *mlock = &mSearchModificationLock;
    if (mContentSwapEnabled)
      mlock = nullptr;

    AutoReadLock readLock(mlock);

    uint cLen = ssp->mContentInfoList[0].getLength();
    for (uint c=0; c<cLen; c++)
    {
      T::ContentInfo *cInfo = ssp->mContentInfoList[0].getContentInfoByIndex(c);
      T::ContentInfo *cError = nullptr;
      if (cInfo != nullptr)
      {
        T::FileInfo *fileInfo = ssp->mFileInfoList.getFileInfoById(cInfo->mFileId);
        if (fileInfo == nullptr)
        {
          PRINT_DATA(mDebugLog,"**** INTEGRITY ERROR : File missing (%u)! *****\n",cInfo->mFileId);
          cError = cInfo;
        }

        if (cError == nullptr)
        {
          T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(cInfo->mGenerationId);
          if (generationInfo == nullptr)
          {
            PRINT_DATA(mDebugLog,"**** INTEGRITY ERROR : Generation missing (%u)! *****\n",cInfo->mGenerationId);
            cError = cInfo;
          }
        }

        if (cError == nullptr)
        {
          T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoById(cInfo->mProducerId);
          if (producerInfo == nullptr)
          {
            PRINT_DATA(mDebugLog,"**** INTEGRITY ERROR : Producer missing (%u)! *****\n",cInfo->mProducerId);
            cError = cInfo;
          }
        }

        if (cError != nullptr)
          contentInfoList.addContentInfo(cError->duplicate());
      }
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentGeometryIdListByGenerationId(sessionId,generationId,geometryIdList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    geometryIdList.clear();

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      ssp->mContentInfoList[1].getContentGeometryIdListByGenerationId(generationInfo->mProducerId,generationId,geometryIdList);
    }
    else
    {
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      ssp->mContentInfoList[1].getContentGeometryIdListByGenerationId(generationInfo->mProducerId,generationId,geometryIdList);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentParamListByGenerationId(sessionId,generationId,contentParamList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ModificationLock *mlock = &mSearchModificationLock;
    if (mContentSwapEnabled)
      mlock = nullptr;

    AutoReadLock readLock(mlock);

    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    contentParamList.clear();

    T::ContentInfoList contentInfoList;
    ssp->mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,0,0,1000000,contentInfoList);
    contentInfoList.sort(T::ContentInfo::ComparisonMethod::fmiName_fmiLevelId_level_starttime_file_message);
    uint len = contentInfoList.getLength();
    T::ContentInfo *prev = nullptr;
    T::ContentInfo *currentInfo = nullptr;
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);

      if (info != nullptr  &&  (prev == nullptr ||
          info->getFmiParameterName() != prev->getFmiParameterName() ||
          info->mFmiParameterLevelId != prev->mFmiParameterLevelId ||
          info->mParameterLevel != prev->mParameterLevel ||
          info->mForecastType != prev->mForecastType ||
          info->mForecastNumber != prev->mForecastNumber))
      {
        currentInfo = info->duplicate();
        currentInfo->mMessageIndex = 1;
        contentParamList.addContentInfo(currentInfo);
      }
      else
      {
        currentInfo->mMessageIndex++;
      }
      prev = info;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentParamKeyListByGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentParamKeyListByGenerationId(sessionId,generationId,parameterKeyType,paramKeyList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    paramKeyList.clear();

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      ssp->mContentInfoList[1].getContentParamKeyListByGenerationId(generationInfo->mProducerId,generationId,parameterKeyType,paramKeyList);
    }
    else
    {
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      ssp->mContentInfoList[1].getContentParamKeyListByGenerationId(generationInfo->mProducerId,generationId,parameterKeyType,paramKeyList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentParamKeyListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentParamKeyListByGenerationAndGeometryId(sessionId,generationId,geometryId,parameterKeyType,paramKeyList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    paramKeyList.clear();

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      ssp->mContentInfoList[1].getContentParamKeyListByGenerationAndGeometryId(generationInfo->mProducerId,generationId,geometryId,parameterKeyType,paramKeyList);
    }
    else
    {
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      ssp->mContentInfoList[1].getContentParamKeyListByGenerationAndGeometryId(generationInfo->mProducerId,generationId,geometryId,parameterKeyType,paramKeyList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentParamKeyListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentParamKeyListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,parameterKeyType,paramKeyList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    paramKeyList.clear();

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      ssp->mContentInfoList[1].getContentParamKeyListByGenerationGeometryAndLevelId(generationInfo->mProducerId,generationId,geometryId,levelId,parameterKeyType,paramKeyList);
    }
    else
    {
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      ssp->mContentInfoList[1].getContentParamKeyListByGenerationGeometryAndLevelId(generationInfo->mProducerId,generationId,geometryId,levelId,parameterKeyType,paramKeyList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentLevelListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentLevelListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,contentLevelList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    contentLevelList.clear();

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      ssp->mContentInfoList[1].getContentLevelListByGenerationGeometryAndLevelId(generationInfo->mProducerId,generationId,geometryId,levelId,contentLevelList);
    }
    else
    {
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      ssp->mContentInfoList[1].getContentLevelListByGenerationGeometryAndLevelId(generationInfo->mProducerId,generationId,geometryId,levelId,contentLevelList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentTimeListByGenerationId(sessionId,generationId,contentTimeList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    contentTimeList.clear();

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      AutoReadLock cacheLock(&mContentTimeCache_modificationLock);
      auto it = mContentTimeCache.find(generationId);
      if (it == mContentTimeCache.end())
      {
        ssp->mContentInfoList[1].getForecastTimeListByGenerationId(generationInfo->mProducerId,generationId,contentTimeList);

        if (generationInfo->mStatus == T::GenerationInfo::Status::Ready)
        {
          mContentTimeCache_modificationLock.writeLockWhenInsideReadLock();
          if (mContentTimeCache.size() > 100000)
            mContentTimeCache.clear();

          mContentTimeCache.insert(std::pair<uint,std::set<std::string>>(generationId,contentTimeList));
          mContentTimeCache_modificationLock.writeUnlock();
        }
      }
      else
      {
        contentTimeList = it->second;
      }
    }
    else
    {
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      AutoReadLock cacheLock(&mContentTimeCache_modificationLock);
      auto it = mContentTimeCache.find(generationId);
      if (it == mContentTimeCache.end())
      {
        ssp->mContentInfoList[1].getForecastTimeListByGenerationId(generationInfo->mProducerId,generationId,contentTimeList);

        if (generationInfo->mStatus == T::GenerationInfo::Status::Ready)
        {
          mContentTimeCache_modificationLock.writeLockWhenInsideReadLock();
          if (mContentTimeCache.size() > 100000)
            mContentTimeCache.clear();

          mContentTimeCache.insert(std::pair<uint,std::set<std::string>>(generationId,contentTimeList));
          mContentTimeCache_modificationLock.writeUnlock();
        }
      }
      else
      {
        contentTimeList = it->second;
      }
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentTimeRangeByProducerAndGenerationId(T::SessionId sessionId,uint producerId,uint generationId,time_t& startTime,time_t& endTime)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentTimeRangeByProducerAndGenerationId(sessionId,producerId,generationId,startTime,endTime);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      if (generationInfo->mContentStartTime == 0)
      {
        ssp->mContentInfoList[1].getForecastTimeRangeByGenerationId(producerId,generationId,startTime,endTime,generationInfo->mContentHash);
        generationInfo->mContentStartTime = startTime;
        generationInfo->mContentEndTime = endTime;
      }
      else
      {
        startTime = generationInfo->mContentStartTime;
        endTime = generationInfo->mContentEndTime;
      }
    }
    else
    {
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      if (generationInfo->mContentStartTime == 0)
      {
        ssp->mContentInfoList[1].getForecastTimeRangeByGenerationId(producerId,generationId,startTime,endTime,generationInfo->mContentHash);
        generationInfo->mContentStartTime = startTime;
        generationInfo->mContentEndTime = endTime;
      }
      else
      {
        startTime = generationInfo->mContentStartTime;
        endTime = generationInfo->mContentEndTime;
      }
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentTimeRangeByGenerationId(T::SessionId sessionId,uint generationId,time_t& startTime,time_t& endTime)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentTimeRangeByGenerationId(sessionId,generationId,startTime,endTime);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      if (generationInfo->mContentStartTime == 0)
      {
        ssp->mContentInfoList[1].getForecastTimeRangeByGenerationId(generationInfo->mProducerId,generationId,startTime,endTime,generationInfo->mContentHash);
        generationInfo->mContentStartTime = startTime;
        generationInfo->mContentEndTime = endTime;
      }
      else
      {
        startTime = generationInfo->mContentStartTime;
        endTime = generationInfo->mContentEndTime;
      }
    }
    else
    {
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      if (generationInfo->mContentStartTime == 0)
      {
        ssp->mContentInfoList[1].getForecastTimeRangeByGenerationId(generationInfo->mProducerId,generationId,startTime,endTime,generationInfo->mContentHash);
        generationInfo->mContentStartTime = startTime;
        generationInfo->mContentEndTime = endTime;
      }
      else
      {
        startTime = generationInfo->mContentStartTime;
        endTime = generationInfo->mContentEndTime;
      }
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentTimeListByGenerationAndGeometryId(sessionId,generationId,geometryId,contentTimeList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    contentTimeList.clear();

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      ssp->mContentInfoList[1].getForecastTimeListByGenerationAndGeometry(generationInfo->mProducerId,generationId,geometryId,contentTimeList);
    }
    else
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      ssp->mContentInfoList[1].getForecastTimeListByGenerationAndGeometry(generationInfo->mProducerId,generationId,geometryId,contentTimeList);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentTimeListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentTimeListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,contentTimeList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    contentTimeList.clear();

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      ssp->mContentInfoList[1].getForecastTimeListByGenerationGeometryAndLevelId(generationInfo->mProducerId,generationId,geometryId,levelId,contentTimeList);
    }
    else
    {
      AutoReadLock readLock(&mSearchModificationLock);
      T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      ssp->mContentInfoList[1].getForecastTimeListByGenerationGeometryAndLevelId(generationInfo->mProducerId,generationId,geometryId,levelId,contentTimeList);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentTimeListByProducerId(T::SessionId sessionId,uint producerId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentTimeListByProducerId(sessionId,producerId,contentTimeList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    contentTimeList.clear();

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      ssp->mContentInfoList[1].getForecastTimeListByProducerId(producerId,contentTimeList);
    }
    else
    {
      ssp->mContentInfoList[1].getForecastTimeListByProducerId(producerId,contentTimeList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getContentCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentCount(sessionId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      count = mContentInfoList.getLength();
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      count = ssp->mContentInfoList[0].getLength();
    }
    else
    {
      count = ssp->mContentInfoList[0].getLength();
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getHashByProducerId(T::SessionId sessionId,uint producerId,ulonglong& hash)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getHashByProducerId(sessionId,producerId,hash);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      std::size_t generationHash = mGenerationInfoList.getHashByProducerId(producerId);
      std::size_t geometryHash = mGeometryInfoList.getHashByProducerId(producerId);
      std::size_t fileHash = mFileInfoList.getHashByProducerId(producerId);
      std::size_t contentHash = mContentInfoList.getHashByProducerId(producerId);

      std::size_t h = 0;
      boost::hash_combine(h,generationHash);
      boost::hash_combine(h,geometryHash);
      boost::hash_combine(h,fileHash);
      boost::hash_combine(h,contentHash);

      hash = h;

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ModificationLock *mlock = &mSearchModificationLock;
    if (mContentSwapEnabled)
      mlock = nullptr;

    AutoReadLock readLock(mlock);

    std::size_t generationHash = ssp->mGenerationInfoList.getHashByProducerId(producerId);
    std::size_t geometryHash = ssp->mGeometryInfoList.getHashByProducerId(producerId);
    std::size_t fileHash = ssp->mFileInfoList.getHashByProducerId(producerId);
    std::size_t contentHash = ssp->mContentInfoList[0].getHashByProducerId(producerId);

    std::size_t h = 0;
    boost::hash_combine(h,generationHash);
    boost::hash_combine(h,geometryHash);
    boost::hash_combine(h,fileHash);
    boost::hash_combine(h,contentHash);

    hash = h;

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getLevelInfoList(sessionId,levelInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    levelInfoList.clear();

    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mContentInfoList.getLevelInfoList(levelInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!mContentSwapEnabled)
    {
      AutoReadLock readLock(&mSearchModificationLock);
      ssp->mContentInfoList[0].getLevelInfoList(levelInfoList);
    }
    else
    {
      ssp->mContentInfoList[0].getLevelInfoList(levelInfoList);
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int CacheImplementation::_updateCachedFiles(T::SessionId sessionId,std::set<uint>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mContentSwapEnabled &&  sessionId == mDataServerSessionId)
    {
      AutoWriteLock lock(&mModificationLock);

      // Question: Is is faster to just replace the whole set:
      //
      //   mCachedFiles = fileIdList;

      for (auto it = mCachedFiles.begin(); it != mCachedFiles.end(); ++it)
      {
        if (fileIdList.find(*it) == fileIdList.end())
          mCachedFiles.erase(*it);
      }

      for (auto it = fileIdList.begin(); it != fileIdList.end(); ++it)
      {
        mCachedFiles.insert(*it);
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::readProducerList()
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"* Reading the producer list\n");

    if (mContentStorage == nullptr)
      return;

    if (mShutdownRequested)
      return;

    mProducerInfoList.clear();

    int result = mContentStorage->getProducerInfoList(mSessionId,mProducerInfoList);
    if (result != 0)
    {
      Fmi::Exception exception(BCP,"Cannot read the producer list from the content storage!");
      exception.addParameter("ServiceResult",getResultString(result));
      throw exception;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void CacheImplementation::readGenerationList()
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"* Reading the generation list\n");

    if (mContentStorage == nullptr)
      return;

    if (mShutdownRequested)
      return;

    mGenerationInfoList.clear();

    int result = mContentStorage->getGenerationInfoList(mSessionId,mGenerationInfoList);
    if (result != 0)
    {
      Fmi::Exception exception(BCP,"Cannot read the generation list from the content storage!");
      exception.addParameter("ServiceResult",getResultString(result));
      throw exception;
    }

    mGenerationInfoList.setComparisonMethod(T::GenerationInfo::ComparisonMethod::generationId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void CacheImplementation::readGeometryList()
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"* Reading the generation list\n");

    if (mContentStorage == nullptr)
      return;

    if (mShutdownRequested)
      return;

    mGeometryInfoList.clear();

    int result = mContentStorage->getGeometryInfoList(mSessionId,mGeometryInfoList);
    if (result != 0)
    {
      Fmi::Exception exception(BCP,"Cannot read the geometry list from the content storage!");
      exception.addParameter("ServiceResult",getResultString(result));
      throw exception;
    }

    mGeometryInfoList.setComparisonMethod(T::GeometryInfo::ComparisonMethod::generationId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void CacheImplementation::readFileList()
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"* Reading the file list\n");

    if (mContentStorage == nullptr)
      return;

    if (mShutdownRequested)
      return;

    mFileInfoList.clear();
    mFileInfoList.setComparisonMethod(T::FileInfo::ComparisonMethod::none);


    uint startFileId = 0;
    uint len = 50000;
    while (len > 0)
    {
      if (mShutdownRequested)
        return;

      T::FileInfoList fileInfoList;

      int result = mContentStorage->getFileInfoList(mSessionId,startFileId,50000,fileInfoList);
      if (result != 0)
      {
        Fmi::Exception exception(BCP,"Cannot read the file list from the content storage!");
        exception.addParameter("ServiceResult",getResultString(result));
        throw exception;
      }

      len = fileInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        if (mShutdownRequested)
          return;

        T::FileInfo *fileInfo = fileInfoList.getFileInfoByIndex(t);
        if (fileInfo != nullptr)
        {
          if (fileInfo->mFileId >= startFileId)
            startFileId = fileInfo->mFileId + 1;

          mFileInfoList.addFileInfo(fileInfo->duplicate());
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void CacheImplementation::readContentList()
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"* Reading the content list\n");
    if (mContentStorage == nullptr)
      return;

    if (mShutdownRequested)
      return;

    mContentInfoList.clear();

    uint startFileId = 0;
    uint startMessageIndex = 0;
    uint len = 50000;
    while (len > 0)
    {
      if (mShutdownRequested)
        return;

      T::ContentInfoList contentInfoList;

      int result = mContentStorage->getContentList(mSessionId,startFileId,startMessageIndex,50000,contentInfoList);

      if (result != 0)
      {
        Fmi::Exception exception(BCP,"Cannot read the content list from the content storage!");
        exception.addParameter("ServiceResult",getResultString(result));
        throw exception;
      }

      len = contentInfoList.getLength();

      for (uint t=0; t<len; t++)
      {
        if (mShutdownRequested)
          return;

        T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
        if (contentInfo != nullptr)
        {
          startFileId = contentInfo->mFileId;
          startMessageIndex = contentInfo->mMessageIndex + 1;

          T::ContentInfo *newContentInfo = contentInfo->duplicate();
          if (mContentInfoList.addContentInfo(newContentInfo) != newContentInfo)
            delete newContentInfo;
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



void CacheImplementation::event_clear(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"*** Clear event : Deleting all cached information!\n");

    AutoWriteLock lock(&mModificationLock);

    {
      AutoWriteLock cacheLock(&mContentTimeCache_modificationLock);
      mContentTimeCache.clear();
    }

    mEventInfoList.clear();
    mFileInfoList.clear();
    mProducerInfoList.clear();
    mGenerationInfoList.clear();
    mContentInfoList.clear();

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoWriteLock writeLock(&mSearchModificationLock);

        for (int t=CONTENT_LIST_COUNT-1; t>= 0; t--)
          ssp->mContentInfoList[t].clear();

        ssp->mFileInfoListByName.clear();
        ssp->mFileInfoList.clear();
        ssp->mGenerationInfoList.clear();
        ssp->mProducerInfoList.clear();
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_contentServerReload(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    mUpdateInProgress = false;
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_updateLoopStart(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    mUpdateInProgress = false;
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_updateLoopEnd(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // Content will be swapped if it has changed
    mContentUpdateTime = time(0) - mContentUpdateInterval;
  }
  catch (...)
  {
    mUpdateInProgress = false;
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_producerAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *info = mProducerInfoList.getProducerInfoById(eventInfo.mId1);
    if (info != nullptr)
      return; // Already in the cache

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        T::ProducerInfo *info = ssp->mProducerInfoList.getProducerInfoById(eventInfo.mId1);
        if (info != nullptr)
          return; // Already in the cache
      }
    }

    T::ProducerInfo producerInfo;
    if (mContentStorage->getProducerInfoById(mSessionId,eventInfo.mId1,producerInfo) == Result::OK)
    {
      mProducerInfoList.addProducerInfo(producerInfo.duplicate());
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_producerDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    mContentDeleteCounter += mContentInfoList.markDeletedByProducerId(eventInfo.mId1);
    mFileDeleteCounter += mFileInfoList.markDeletedByProducerId(eventInfo.mId1);
    mGeometryDeleteCounter += mGeometryInfoList.markDeletedByProducerId(eventInfo.mId1);
    mGenerationDeleteCounter += mGenerationInfoList.markDeletedByProducerId(eventInfo.mId1);
    mProducerDeleteCounter += mProducerInfoList.markDeletedById(eventInfo.mId1);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedByProducerId(eventInfo.mId1);
        mFileDeleteCounter += ssp->mFileInfoList.markDeletedByProducerId(eventInfo.mId1);
        mGeometryDeleteCounter += ssp->mGeometryInfoList.markDeletedByProducerId(eventInfo.mId1);
        mGenerationDeleteCounter += ssp->mGenerationInfoList.markDeletedByProducerId(eventInfo.mId1);
        mProducerDeleteCounter += ssp->mProducerInfoList.markDeletedById(eventInfo.mId1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_producerUpdated(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    T::ProducerInfo producerInfo;
    if (mContentStorage->getProducerInfoById(mSessionId,eventInfo.mId1,producerInfo) != Result::OK)
      return;

    {
      AutoWriteLock lock(&mModificationLock);
      T::ProducerInfo *info = mProducerInfoList.getProducerInfoById(eventInfo.mId1);
      if (info)
      {
        *info = producerInfo;
        return;
      }
    }

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        T::ProducerInfo *info = ssp->mProducerInfoList.getProducerInfoById(eventInfo.mId1);
        if (info)
        {
          *info = producerInfo;
          return;
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_producerListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    mContentDeleteCounter += mContentInfoList.markDeletedBySourceId(eventInfo.mId1);
    mFileDeleteCounter += mFileInfoList.markDeletedBySourceId(eventInfo.mId1);
    mGeometryDeleteCounter += mGeometryInfoList.markDeletedBySourceId(eventInfo.mId1);
    mGenerationDeleteCounter += mGenerationInfoList.markDeletedBySourceId(eventInfo.mId1);
    mProducerDeleteCounter += mProducerInfoList.markDeletedBySourceId(eventInfo.mId1);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedBySourceId(eventInfo.mId1);
        mFileDeleteCounter += ssp->mFileInfoList.markDeletedBySourceId(eventInfo.mId1);
        mGeometryDeleteCounter += ssp->mGeometryInfoList.markDeletedBySourceId(eventInfo.mId1);
        mGenerationDeleteCounter += ssp->mGenerationInfoList.markDeletedBySourceId(eventInfo.mId1);
        mProducerDeleteCounter += ssp->mProducerInfoList.markDeletedBySourceId(eventInfo.mId1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_generationAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoById(eventInfo.mId1);
    if (info != nullptr)
      return; // Already in the cache

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        T::GenerationInfo *info = ssp->mGenerationInfoList.getGenerationInfoById(eventInfo.mId1);
        if (info != nullptr)
          return; // Already in the cache
      }
    }

    T::GenerationInfo generationInfo;
    if (mContentStorage->getGenerationInfoById(mSessionId,eventInfo.mId1,generationInfo) == Result::OK)
    {
      mGenerationInfoList.addGenerationInfo(generationInfo.duplicate());
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_generationDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    {
      AutoWriteLock cacheLock(&mContentTimeCache_modificationLock);
      if (mContentTimeCache.find(eventInfo.mId1) != mContentTimeCache.end())
        mContentTimeCache.erase(eventInfo.mId1);
    }

    mContentDeleteCounter += mContentInfoList.markDeletedByGenerationId(eventInfo.mId1);
    mFileDeleteCounter += mFileInfoList.markDeletedByGenerationId(eventInfo.mId1);
    mGeometryDeleteCounter += mGeometryInfoList.markDeletedByGenerationId(eventInfo.mId1);
    mGenerationDeleteCounter += mGenerationInfoList.deleteGenerationInfoById(eventInfo.mId1);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedByGenerationId(eventInfo.mId1);
        mFileDeleteCounter += ssp->mFileInfoList.markDeletedByGenerationId(eventInfo.mId1);
        mGeometryDeleteCounter += ssp->mGeometryInfoList.markDeletedByGenerationId(eventInfo.mId1);
        mGenerationDeleteCounter += ssp->mGenerationInfoList.markDeletedById(eventInfo.mId1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_generationUpdated(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfo generationInfo;
    if (mContentStorage->getGenerationInfoById(mSessionId,eventInfo.mId1,generationInfo) != Result::OK)
      return;

    {
      AutoWriteLock lock(&mModificationLock);
      T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoById(eventInfo.mId1);
      if (info)
        *info = generationInfo;
    }

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        T::GenerationInfo *info = ssp->mGenerationInfoList.getGenerationInfoById(eventInfo.mId1);
        if (info)
          *info = generationInfo;
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_generationStatusChanged(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    {
      AutoWriteLock lock(&mModificationLock);
      T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoById(eventInfo.mId1);
      if (info != nullptr)
        info->mStatus = eventInfo.mId2;
    }

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        T::GenerationInfo *ginfo = ssp->mGenerationInfoList.getGenerationInfoById(eventInfo.mId1);
        if (ginfo != nullptr)
        {
          ginfo->mStatus = eventInfo.mId2;
          ssp->mContentInfoList[1].getForecastTimeRangeByGenerationId(ginfo->mProducerId,ginfo->mGenerationId,ginfo->mContentStartTime,ginfo->mContentEndTime,ginfo->mContentHash);
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_generationListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    mContentDeleteCounter += mContentInfoList.deleteContentInfoByProducerId(eventInfo.mId1);
    mFileDeleteCounter += mFileInfoList.deleteFileInfoByProducerId(eventInfo.mId1);
    mGeometryDeleteCounter += mGeometryInfoList.deleteGeometryInfoListByProducerId(eventInfo.mId1);
    mGenerationDeleteCounter += mGenerationInfoList.deleteGenerationInfoListByProducerId(eventInfo.mId1);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedByProducerId(eventInfo.mId1);
        mFileDeleteCounter += ssp->mFileInfoList.markDeletedByProducerId(eventInfo.mId1);
        mGeometryDeleteCounter += ssp->mGeometryInfoList.markDeletedByProducerId(eventInfo.mId1);
        mGenerationDeleteCounter += ssp->mGenerationInfoList.markDeletedByProducerId(eventInfo.mId1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_generationListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    mContentDeleteCounter += mContentInfoList.markDeletedBySourceId(eventInfo.mId1);
    mFileDeleteCounter += mFileInfoList.markDeletedBySourceId(eventInfo.mId1);
    mGeometryDeleteCounter += mGeometryInfoList.markDeletedBySourceId(eventInfo.mId1);
    mGenerationDeleteCounter += mGenerationInfoList.markDeletedBySourceId(eventInfo.mId1);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedBySourceId(eventInfo.mId1);
        mFileDeleteCounter += ssp->mFileInfoList.markDeletedBySourceId(eventInfo.mId1);
        mGeometryDeleteCounter += ssp->mGeometryInfoList.markDeletedBySourceId(eventInfo.mId1);
        mGenerationDeleteCounter += ssp->mGenerationInfoList.markDeletedBySourceId(eventInfo.mId1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_geometryAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    T::GeometryInfo *info = mGeometryInfoList.getGeometryInfoById(eventInfo.mId1,eventInfo.mId2,eventInfo.mId3);
    if (info != nullptr)
      return; // Already in the cache

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        T::GeometryInfo *info = ssp->mGeometryInfoList.getGeometryInfoById(eventInfo.mId1,eventInfo.mId2,eventInfo.mId3);
        if (info != nullptr)
          return; // Already in the cache
      }
    }

    T::GeometryInfo geometryInfo;
    if (mContentStorage->getGeometryInfoById(mSessionId,eventInfo.mId1,eventInfo.mId2,eventInfo.mId3,geometryInfo) == Result::OK)
    {
      mGeometryInfoList.addGeometryInfo(geometryInfo.duplicate());
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void CacheImplementation::event_geometryDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    mContentDeleteCounter += mContentInfoList.markDeletedByGenerationAndGeometryId(eventInfo.mId1,eventInfo.mId2);
    // mFileDeleteCounter += mFileInfoList.markDeletedByGenerationAndGeometryId(eventInfo.mId1,eventInfo.mId2);
    mGeometryDeleteCounter += mGeometryInfoList.deleteGeometryInfoById(eventInfo.mId1,eventInfo.mId2,eventInfo.mId3);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedByGenerationAndGeometryId(eventInfo.mId1,eventInfo.mId2);
        //mFileDeleteCounter += ssp->mFileInfoList.markDeletedByGenerationAndGeometryId(eventInfo.mId1,eventInfo.mId2);
        mGeometryDeleteCounter += ssp->mGeometryInfoList.markDeletedById(eventInfo.mId1,eventInfo.mId2,eventInfo.mId3);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void CacheImplementation::event_geometryStatusChanged(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    {
      AutoWriteLock lock(&mModificationLock);
      T::GeometryInfo *info = mGeometryInfoList.getGeometryInfoById(eventInfo.mId1,eventInfo.mId2,eventInfo.mId3);
      if (info)
      {
        info->mStatus = eventInfo.mFlags;
        return;
      }
    }

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        T::GeometryInfo *info = ssp->mGeometryInfoList.getGeometryInfoById(eventInfo.mId1,eventInfo.mId2,eventInfo.mId3);
        if (info != nullptr)
          info->mStatus = eventInfo.mFlags;
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void CacheImplementation::event_geometryUpdated(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    T::GeometryInfo geometryInfo;
    if (mContentStorage->getGeometryInfoById(mSessionId,eventInfo.mId1,eventInfo.mId2,eventInfo.mId3,geometryInfo) != Result::OK)
      return;

    {
      AutoWriteLock lock(&mModificationLock);
      T::GeometryInfo *info = mGeometryInfoList.getGeometryInfoById(eventInfo.mId1,eventInfo.mId2,eventInfo.mId3);
      if (info)
      {
        *info = geometryInfo;
        return;
      }
    }

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        T::GeometryInfo *info = ssp->mGeometryInfoList.getGeometryInfoById(eventInfo.mId1,eventInfo.mId2,eventInfo.mId3);
        if (info)
          *info = geometryInfo;
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void CacheImplementation::event_geometryListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    //mContentDeleteCounter += mContentInfoList.deleteContentInfoByProducerId(eventInfo.mId1);
    //mFileDeleteCounter += mFileInfoList.deleteFileInfoByProducerId(eventInfo.mId1);
    mGeometryDeleteCounter += mGeometryInfoList.deleteGeometryInfoListByProducerId(eventInfo.mId1);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        //mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedByProducerId(eventInfo.mId1);
        //mFileDeleteCounter += ssp->mFileInfoList.markDeletedByProducerId(eventInfo.mId1);
        mGeometryDeleteCounter += ssp->mGeometryInfoList.markDeletedByProducerId(eventInfo.mId1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_geometryListDeletedByGenerationId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    //mContentDeleteCounter += mContentInfoList.deleteContentInfoByProducerId(eventInfo.mId1);
    //mFileDeleteCounter += mFileInfoList.deleteFileInfoByProducerId(eventInfo.mId1);
    mGeometryDeleteCounter += mGeometryInfoList.deleteGeometryInfoListByGenerationId(eventInfo.mId1);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        //mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedByProducerId(eventInfo.mId1);
        //mFileDeleteCounter += ssp->mFileInfoList.markDeletedByProducerId(eventInfo.mId1);
        mGeometryDeleteCounter += ssp->mGeometryInfoList.markDeletedByGenerationId(eventInfo.mId1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void CacheImplementation::event_geometryListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    //mContentDeleteCounter += mContentInfoList.deleteContentInfoByProducerId(eventInfo.mId1);
    //mFileDeleteCounter += mFileInfoList.deleteFileInfoByProducerId(eventInfo.mId1);
    mGeometryDeleteCounter += mGeometryInfoList.deleteGeometryInfoListBySourceId(eventInfo.mId1);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        //mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedByProducerId(eventInfo.mId1);
        //mFileDeleteCounter += ssp->mFileInfoList.markDeletedByProducerId(eventInfo.mId1);
        mGeometryDeleteCounter += ssp->mGeometryInfoList.markDeletedBySourceId(eventInfo.mId1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_fileAdded(T::EventInfo& eventInfo)
{
  //FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    uint len = eventInfo.mEventData.length();

    if (len > 0)
    {
      char *buf = new char[len+10];

      strcpy(buf,eventInfo.mEventData.c_str());
      char *s = buf;

      while (s != nullptr)
      {
        char *p = strstr(s,"\n");
        if (p != nullptr)
        {
          *p = '\0';
          if (s == buf)
          {
            T::FileInfo *fileInfo = new T::FileInfo();
            fileInfo->setCsv(s);

            if (mFileInfoList.getFileInfoById(fileInfo->mFileId) == nullptr)
            {
              mFileInfoList.addFileInfo(fileInfo);
            }
            else
            {
              delete fileInfo;
            }
          }
          else
          {
            T::ContentInfo *contentInfo = new T::ContentInfo();
            contentInfo->setCsv(s);
            if (mContentInfoList.getContentInfoByFileIdAndMessageIndex(contentInfo->mFileId,contentInfo->mMessageIndex) == nullptr)
            {
              AutoWriteLock cacheLock(&mContentTimeCache_modificationLock);
              auto it = mContentTimeCache.find(contentInfo->mGenerationId);
              if (it != mContentTimeCache.end())
              {
                //if (it->second.find(contentInfo->mForecastTime) == it->second.end())
                  it->second.insert(contentInfo->getForecastTime());
              }

              if (mContentInfoList.addContentInfo(contentInfo) == contentInfo)
              {
              }
              else
              {
                // Additon failed. The content probably exists
                delete contentInfo;
              }
            }
            else
            {
              delete contentInfo;
            }
          }
          s = p + 1;
        }
        else
        {
          s = nullptr;
        }
      }
      delete [] buf;
    }
    else
    {
      T::FileInfo fileInfo;
      if (mContentStorage->getFileInfoById(mSessionId,eventInfo.mId1,fileInfo) == Result::OK)
      {
        T::FileInfo *fInfo = fileInfo.duplicate();
        mFileInfoList.addFileInfo(fInfo);

        if (eventInfo.mId3 > 0)
        {
          T::ContentInfoList contentInfoList;
          if (mContentStorage->getContentListByFileId(mSessionId,fileInfo.mFileId,contentInfoList) == Result::OK)
          {
            uint len = contentInfoList.getLength();
            for (uint c=0; c<len; c++)
            {
              T::ContentInfo *info = contentInfoList.getContentInfoByIndex(c);
              T::ContentInfo *oInfo = mContentInfoList.getContentInfoByFileIdAndMessageIndex(info->mFileId,info->mMessageIndex);
              if (info != nullptr &&  (oInfo == nullptr  ||  (oInfo != nullptr  &&  (oInfo->mFlags & T::ContentInfo::Flags::DeletedContent) != 0)))
              {
                if (oInfo != nullptr  &&  (oInfo->mFlags & T::ContentInfo::Flags::DeletedContent) != 0)
                {
                  // We should remove the old content before the addition
                }
                else
                {
                  T::ContentInfo *cInfo = info->duplicate();
                  AutoWriteLock cacheLock(&mContentTimeCache_modificationLock);
                  auto it = mContentTimeCache.find(cInfo->mGenerationId);
                  if (it != mContentTimeCache.end())
                  {
                    //if (it->second.find(cInfo->mForecastTime) == it->second.end())
                      it->second.insert(cInfo->getForecastTime());
                  }

                  if (mContentInfoList.addContentInfo(cInfo) == cInfo)
                  {
                  }
                  else
                  {
                    // Addition failed
                    delete cInfo;
                  }
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_fileDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    mContentDeleteCounter += mContentInfoList.markDeletedByFileId(eventInfo.mId1);
    mFileDeleteCounter += mFileInfoList.markDeletedById(eventInfo.mId1);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        mFileDeleteCounter += ssp->mFileInfoList.markDeletedById(eventInfo.mId1);
        mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedByFileId(eventInfo.mId1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_fileUpdated(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    T::FileInfo fileInfo;
    if (mContentStorage->getFileInfoById(mSessionId,eventInfo.mId1,fileInfo) == Result::OK)
    {
      T::FileInfo *info = mFileInfoList.getFileInfoById(eventInfo.mId1);
      if (info != nullptr)
      {
        mContentDeleteCounter += mContentInfoList.markDeletedByFileId(info->mFileId);
        *info = fileInfo;
      }
      else
      {
        T::FileInfo *fInfo = fileInfo.duplicate();
        mFileInfoList.addFileInfo(fInfo);
      }

      T::ContentInfoList contentInfoList;
      if (mContentStorage->getContentListByFileId(mSessionId,fileInfo.mFileId,contentInfoList) == Result::OK)
      {
        uint len = contentInfoList.getLength();
        for (uint c=0; c<len; c++)
        {
          T::ContentInfo *info = contentInfoList.getContentInfoByIndex(c);
          T::ContentInfo *oInfo = mContentInfoList.getContentInfoByFileIdAndMessageIndex(info->mFileId,info->mMessageIndex);
          if (oInfo == nullptr  ||  (oInfo != nullptr  &&  (oInfo->mFlags & T::ContentInfo::Flags::DeletedContent) != 0))
          {
            if (info != nullptr && (oInfo != nullptr  &&  (oInfo->mFlags & T::ContentInfo::Flags::DeletedContent) != 0))
            {
              // We should remove the old content before the addition
            }
            else
            {
              T::ContentInfo *cInfo = info->duplicate();

              if (mContentInfoList.addContentInfo(cInfo) == cInfo)
              {
              }
              else
              {
                // Addtion failed
                delete cInfo;
              }
            }
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_fileListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    mContentDeleteCounter += mContentInfoList.markDeletedByProducerId(eventInfo.mId1);
    mFileDeleteCounter += mFileInfoList.markDeletedByProducerId(eventInfo.mId1);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedByProducerId(eventInfo.mId1);
        mFileDeleteCounter += ssp->mFileInfoList.markDeletedByProducerId(eventInfo.mId1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_fileListDeletedByGenerationId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    mContentDeleteCounter += mContentInfoList.markDeletedByGenerationId(eventInfo.mId1);
    mFileDeleteCounter += mFileInfoList.markDeletedByGenerationId(eventInfo.mId1);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedByGenerationId(eventInfo.mId1);
        mFileDeleteCounter += ssp->mFileInfoList.markDeletedByGenerationId(eventInfo.mId1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






void CacheImplementation::event_fileListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    mContentDeleteCounter += mContentInfoList.markDeletedBySourceId(eventInfo.mId1);
    mFileDeleteCounter += mFileInfoList.markDeletedBySourceId(eventInfo.mId1);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedBySourceId(eventInfo.mId1);
        mFileDeleteCounter += ssp->mFileInfoList.markDeletedBySourceId(eventInfo.mId1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_contentListDeletedByFileId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    mContentDeleteCounter += mContentInfoList.markDeletedByFileId(eventInfo.mId1);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedByFileId(eventInfo.mId1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_contentListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    mContentDeleteCounter += mContentInfoList.markDeletedByProducerId(eventInfo.mId1);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedByProducerId(eventInfo.mId1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_contentListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    mContentDeleteCounter += mContentInfoList.markDeletedBySourceId(eventInfo.mId1);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedBySourceId(eventInfo.mId1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_contentListDeletedByGenerationId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    mContentDeleteCounter += mContentInfoList.markDeletedByGenerationId(eventInfo.mId1);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedByGenerationId(eventInfo.mId1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_contentAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    {
      AutoReadLock lock(&mModificationLock);
      T::ContentInfo *oInfo = mContentInfoList.getContentInfoByFileIdAndMessageIndex(eventInfo.mId1,eventInfo.mId2);
      if (oInfo != nullptr  &&  (oInfo->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        return;  // The content info is already in the cache.
      }

      if (!mContentSwapEnabled)
      {
        auto ssp = mSearchStructurePtr[mActiveSearchStructure];
        if (ssp)
        {
          AutoReadLock readLock(&mSearchModificationLock);
          T::ContentInfo *cInfo = ssp->mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(eventInfo.mId1,eventInfo.mId2);
          if (cInfo != nullptr)
            return;  // The content info is already in the cache.
        }
      }
    }

    T::ContentInfo contentInfo;
    if (eventInfo.mEventData > " ")
      contentInfo.setCsv(eventInfo.mEventData);
    else
    {
      if (mContentStorage->getContentInfo(mSessionId,eventInfo.mId1,eventInfo.mId2,contentInfo) != Result::OK)
        return;
    }

    if (mContentInfoList.getContentInfoByFileIdAndMessageIndex(contentInfo.mFileId,contentInfo.mMessageIndex) == nullptr)
    {
      AutoWriteLock cacheLock(&mContentTimeCache_modificationLock);
      auto it = mContentTimeCache.find(contentInfo.mGenerationId);
      if (it != mContentTimeCache.end())
      {
        //if (it->second.find(contentInfo.mForecastTime) == it->second.end())
          it->second.insert(contentInfo.getForecastTime());
      }

      AutoWriteLock lock(&mModificationLock);
      T::ContentInfo *cInfo = contentInfo.duplicate();
      if (mContentInfoList.addContentInfo(cInfo) == cInfo)
      {
        // Addition ok
      }
      else
      {
        // Additon failed. The content probably exists
        delete cInfo;
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_contentUpdated(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    {
      AutoWriteLock lock(&mModificationLock);

      mContentInfoList.deleteContentInfoByFileIdAndMessageIndex(eventInfo.mId1,eventInfo.mId2);

      if (!mContentSwapEnabled)
      {
        auto ssp = mSearchStructurePtr[mActiveSearchStructure];
        if (ssp)
        {
          AutoReadLock readLock(&mSearchModificationLock);
          mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedByFileIdAndMessageIndex(eventInfo.mId1,eventInfo.mId2);
        }
      }
    }

    event_contentAdded(eventInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::event_contentDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    mContentDeleteCounter += mContentInfoList.markDeletedByFileIdAndMessageIndex(eventInfo.mId1,eventInfo.mId2);

    if (!mContentSwapEnabled)
    {
      auto ssp = mSearchStructurePtr[mActiveSearchStructure];
      if (ssp)
      {
        AutoReadLock readLock(&mSearchModificationLock);
        mContentDeleteCounter += ssp->mContentInfoList[0].markDeletedByFileIdAndMessageIndex(eventInfo.mId1,eventInfo.mId2);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::processEvent(T::EventInfo& eventInfo)
{
  //FUNCTION_TRACE
  try
  {
    if (mShutdownRequested)
      return;

    switch (eventInfo.mType)
    {
      case EventType::UNKNOWN:
        break;

      case EventType::CLEAR:
        event_clear(eventInfo);
        break;

      case EventType::UPDATE_LOOP_START:
        event_updateLoopStart(eventInfo);
        break;

      case EventType::UPDATE_LOOP_END:
        event_updateLoopEnd(eventInfo);
        break;

      case EventType::CONTENT_SERVER_RELOAD:
        event_contentServerReload(eventInfo);
        break;

      case EventType::PRODUCER_ADDED:
        event_producerAdded(eventInfo);
        break;

      case EventType::PRODUCER_DELETED:
        event_producerDeleted(eventInfo);
        break;

      case EventType::PRODUCER_UPDATED:
        event_producerUpdated(eventInfo);
        break;

      case EventType::PRODUCER_LIST_DELETED_BY_SOURCE_ID:
        event_producerListDeletedBySourceId(eventInfo);
        break;

      case EventType::GENERATION_ADDED:
        event_generationAdded(eventInfo);
        break;

      case EventType::GENERATION_DELETED:
        event_generationDeleted(eventInfo);
        break;

      case EventType::GENERATION_UPDATED:
        event_generationUpdated(eventInfo);
        break;

      case EventType::GENERATION_STATUS_CHANGED:
        event_generationStatusChanged(eventInfo);
        break;

      case EventType::GENERATION_LIST_DELETED_BY_PRODUCER_ID:
        event_generationListDeletedByProducerId(eventInfo);
        break;

      case EventType::GENERATION_LIST_DELETED_BY_SOURCE_ID:
        event_generationListDeletedBySourceId(eventInfo);
        break;

      case EventType::GEOMETRY_ADDED:
        event_geometryAdded(eventInfo);
        break;

      case EventType::GEOMETRY_DELETED:
        event_geometryDeleted(eventInfo);
        break;

      case EventType::GEOMETRY_UPDATED:
        event_geometryUpdated(eventInfo);
        break;

      case EventType::GEOMETRY_STATUS_CHANGED:
        event_geometryStatusChanged(eventInfo);
        break;

      case EventType::GEOMETRY_LIST_DELETED_BY_PRODUCER_ID:
        event_geometryListDeletedByProducerId(eventInfo);
        break;

      case EventType::GEOMETRY_LIST_DELETED_BY_GENERATION_ID:
        event_geometryListDeletedByGenerationId(eventInfo);
        break;

      case EventType::GEOMETRY_LIST_DELETED_BY_SOURCE_ID:
        event_geometryListDeletedBySourceId(eventInfo);
        break;

      case EventType::FILE_ADDED:
        event_fileAdded(eventInfo);
        break;

      case EventType::FILE_DELETED:
        event_fileDeleted(eventInfo);
        break;

      case EventType::FILE_UPDATED:
        event_fileUpdated(eventInfo);
        break;

      case EventType::FILE_LIST_DELETED_BY_PRODUCER_ID:
        event_fileListDeletedByProducerId(eventInfo);
        break;

      case EventType::FILE_LIST_DELETED_BY_GENERATION_ID:
        event_fileListDeletedByGenerationId(eventInfo);
        break;

      case EventType::FILE_LIST_DELETED_BY_SOURCE_ID:
        event_fileListDeletedBySourceId(eventInfo);
        break;

      case EventType::CONTENT_LIST_DELETED_BY_FILE_ID:
        event_contentListDeletedByFileId(eventInfo);
        break;

      case EventType::CONTENT_LIST_DELETED_BY_PRODUCER_ID:
        event_contentListDeletedByProducerId(eventInfo);
        break;

      case EventType::CONTENT_LIST_DELETED_BY_GENERATION_ID:
        event_contentListDeletedByGenerationId(eventInfo);
        break;

      case EventType::CONTENT_LIST_DELETED_BY_SOURCE_ID:
        event_contentListDeletedBySourceId(eventInfo);
        break;

      case EventType::CONTENT_ADDED:
        event_contentAdded(eventInfo);
        break;

      case EventType::CONTENT_DELETED:
        event_contentDeleted(eventInfo);
        break;

      case EventType::CONTENT_UPDATED:
        event_contentUpdated(eventInfo);
        break;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::processEvents(bool eventThread)
{
  // FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return;

    if (!eventThread  &&  mReloadActivated)
    {
      // If this method is called from a service method (not from the event thread)
      // then we should exit, because otherwise the call is blocked until the reload
      // is finished.

      return;
    }

    updateContent();

    AutoThreadLock eventLock(&mEventProcessingLock);

    uint len = 1000;
    while (len > 0)
    {
      if (mShutdownRequested)
        return;

      len = 0;
      T::EventInfo eventInfo;
      int result = mContentStorage->getLastEventInfo(mSessionId,0,eventInfo);
      if (result == Result::DATA_NOT_FOUND || result == Result::OK)
      {
        if (eventThread  &&  mContentStorageStartTime > 0 &&  mContentStorageStartTime < eventInfo.mServerTime)
        {
          PRINT_DATA(mDebugLog, "#### Content server restart detected, reload required #######\n");
          mEventInfoList.clear();
          reloadData();
          updateContent();
          mContentStorageStartTime = eventInfo.mServerTime;
          return;
        }
      }

      if (result != Result::OK)
        return;

      if (eventThread  &&  mContentStorageStartTime == 0)
        mContentStorageStartTime = eventInfo.mServerTime;

      if (eventInfo.mEventId > mLastProcessedEventId)
      {

        T::EventInfoList eventInfoList;
        result = mContentStorage->getEventInfoList(mSessionId,0,mLastProcessedEventId+1,10000,eventInfoList);
        if (result != 0)
        {
          return;
        }

        len = eventInfoList.getLength();

        T::EventInfo *it = eventInfoList.getFirstEvent();
        while (it != nullptr)
        {
          try
          {
            processEvent(*it);
          }
          catch (...)
          {
            Fmi::Exception exception(BCP,"Operation failed!",nullptr);
            exception.printError();
          }


          T::EventInfo *event = it->duplicate();
          //event->mServerTime = mContentStorageStartTime;
          mEventInfoList.addEventInfo(event);
          mLastProcessedEventId = it->mEventId;

          it = it->nextItem;

          if (mShutdownRequested)
            return;
        }
      }
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (ssp)
    {
      AutoReadLock lock(&mContentTimeCache_modificationLock);

      for (auto it = mContentTimeCache.begin();  it != mContentTimeCache.end(); ++it)
      {
        if (ssp->mGenerationInfoList.getGenerationInfoById(it->first) == nullptr)
        {
          if (mShutdownRequested)
            return;

          mContentTimeCache_modificationLock.writeLockWhenInsideReadLock();
          mContentTimeCache.erase(it->first);
          mContentTimeCache_modificationLock.writeUnlock();
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::eventProcessingThread()
{
  FUNCTION_TRACE
  try
  {
    while (!mShutdownRequested)
    {
      try
      {
        processEvents(true);
      }
      catch (...)
      {
        Fmi::Exception exception(BCP,"Operation failed!",nullptr);
        exception.printError();
      }

      if (!mShutdownRequested)
      {
        saveData();
        sleep(1);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void CacheImplementation::saveData()
{
  //FUNCTION_TRACE
  try
  {
    if (mSaveEnabled)
    {
      AutoReadLock lock(&mModificationLock);

      if (mProducerCount != mProducerInfoList.getLength())
        mProducerInfoList.writeToFile(mSaveDir + "/producer.csv");

      if (mGenerationCount != mGenerationInfoList.getLength())
      {
        std::string filename = mSaveDir + "/generations.csv";
        mGenerationInfoList.writeToFile(filename);
      }

      if (mGeometryCount != mGeometryInfoList.getLength())
      {
        std::string filename = mSaveDir + "/geometries.csv";
        mGeometryInfoList.writeToFile(filename);
      }

      if (mFileCount != mFileInfoList.getLength())
        mFileInfoList.writeToFile(mSaveDir + "/files.csv");

      if (mContentCount != mContentInfoList.getLength())
        mContentInfoList.writeToFile(mSaveDir + "/content.csv");

      mProducerCount = mProducerInfoList.getLength();
      mGenerationCount = mGenerationInfoList.getLength();
      mFileCount = mFileInfoList.getLength();
      mContentCount = mContentInfoList.getLength();
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void CacheImplementation::updateContent()
{
  //FUNCTION_TRACE
  try
  {
    if (mShutdownRequested)
      return;

    if (mReloadActivated)
      return;

    long diff = time(nullptr) - mContentUpdateTime;

    if (diff < mContentUpdateInterval)
      return;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (mContentSwapEnabled)
    {
      // If the swapping is enabled then we create totally new search structure
      // and replace the the old one.

      if (ssp)
      {
        if (mDebugLog &&  mDebugLog->isEnabled())
        {
          PRINT_DATA(mDebugLog,"\nproducers  %ld  %ld\n",ssp->mProducerInfoList.getHash(),mProducerInfoList.getHash());
          PRINT_DATA(mDebugLog,"generations  %ld  %ld\n",ssp->mGenerationInfoList.getHash(),mGenerationInfoList.getHash());
          PRINT_DATA(mDebugLog,"geometries  %ld  %ld\n",ssp->mGeometryInfoList.getHash(),mGeometryInfoList.getHash());
          PRINT_DATA(mDebugLog,"files  %ld  %ld\n",ssp->mFileInfoList.getHash(),mFileInfoList.getHash());
          PRINT_DATA(mDebugLog,"content  %ld  %ld\n",ssp->mContentInfoList[0].getHash(),mContentInfoList.getHash());
        }

        if (mCachedFiles_totalWaitTime == 0 &&
            ssp->mProducerInfoList.getHash() == mProducerInfoList.getHash() &&
            ssp->mGenerationInfoList.getHash() == mGenerationInfoList.getHash() &&
            ssp->mGeometryInfoList.getHash() == mGeometryInfoList.getHash() &&
            ssp->mFileInfoList.getHash() == mFileInfoList.getHash() &&
            ssp->mContentInfoList[0].getHash() == mContentInfoList.getHash())
        {
          // Nothing has changed. No swapping needed.

          PRINT_DATA(mDebugLog, "#### No search structure swapping required #######\n");

          mContentUpdateTime = time(nullptr);
          return;
        }
      }

      AutoReadLock lock(&mModificationLock);

      if ((mContentSwapCounter == 0 && mCachedFiles_maxFirstWaitTime > 0 && mCachedFiles_totalWaitTime < mCachedFiles_maxFirstWaitTime) ||
          (mContentSwapCounter > 0 && mCachedFiles_maxWaitTime > 0 && mCachedFiles_totalWaitTime < mCachedFiles_maxWaitTime))
      {
        // We can swap the search structure only if the locally cached files are ready.

        uint fileCount = mFileInfoList.getLength();
        for (uint t=0; t<fileCount; t++)
        {
          T::FileInfo *fInfo = mFileInfoList.getFileInfoByIndex(t);
          if (fInfo)
          {
            if ((fInfo->mFlags & T::FileInfo::Flags::LocalCacheRecommended)  &&  (fInfo->mFlags & T::FileInfo::Flags::DeletedFile) == 0  &&
                fInfo->mServerType == T::FileInfo::ServerType::Filesys)
            {
              // We are caching locally only marked files.

              if (mCachedFiles.find(fInfo->mFileId) == mCachedFiles.end())
              {
                // Cannot find the file from the cached file list.

                if (fInfo->mDeletionTime == 0 || (time(nullptr) + 180) < fInfo->mDeletionTime)
                {
                  // The file deletion time is not very close, so it should be in the cache

                  T::GenerationInfo *gInfo = mGenerationInfoList.getGenerationInfoById(fInfo->mGenerationId);
                  if (gInfo && gInfo->mStatus == T::GenerationInfo::Status::Ready)
                  {
                    // The file generation status is ready, so it should be in the cache
                    auto fsize = getFileSize(fInfo->mName.c_str());
                    if (fsize > 0)
                    {
                      // The original file exists,so it should be in the cache. If not, we should Wait the swapping.
                      PRINT_DATA(mDebugLog, "#### Delaying the search structure swap about %u seconds #######\n",mCachedFiles_waitTime);
                      PRINT_DATA(mDebugLog, "- Reason      : Required file is not ready in the local cache.\n");
                      PRINT_DATA(mDebugLog, "- FileId      : %u\n",fInfo->mFileId);
                      PRINT_DATA(mDebugLog, "- FileName    : %s\n",fInfo->mName.c_str());
                      PRINT_DATA(mDebugLog, "- Wait time   : %u\n",mCachedFiles_totalWaitTime);
                      if (mContentSwapCounter == 0)
                      {
                        PRINT_DATA(mDebugLog, "- MaxWaitTime : %u\n",mCachedFiles_maxFirstWaitTime);
                      }
                      else
                      {
                        PRINT_DATA(mDebugLog, "- MaxWaitTime : %u\n",mCachedFiles_maxWaitTime);
                      }

                      mContentUpdateTime = time(nullptr) - mContentUpdateInterval + mCachedFiles_waitTime;
                      mCachedFiles_totalWaitTime += mCachedFiles_waitTime;
                      return;
                    }
                  }
                }
              }
            }
          }
        }
      }

      if ((mContentSwapCounter == 0 && mCachedFiles_maxFirstWaitTime > 0 &&  mCachedFiles_totalWaitTime > mCachedFiles_maxFirstWaitTime) ||
          (mContentSwapCounter > 0 && mCachedFiles_maxWaitTime > 0 && mCachedFiles_totalWaitTime > mCachedFiles_maxWaitTime))
      {
        Fmi::Exception exception(BCP,"Ignoring local file cache requirement, because max wait time was exceeded!");
        exception.addParameter("contentSwapCounter",std::to_string(mContentSwapCounter));
        exception.addParameter("totalWaitTime (in seconds)",std::to_string(mCachedFiles_totalWaitTime));
        exception.addParameter("maxFirstWaitTime (in seconds)",std::to_string(mCachedFiles_maxFirstWaitTime));
        exception.addParameter("maxWaitTime (in seconds)",std::to_string(mCachedFiles_maxWaitTime));
        exception.printError();
      }

      mCachedFiles_totalWaitTime = 0;

      auto nptr = new SearchStructure();
      if (mActiveSearchStructure == 0)
      {
        if (mSearchStructurePtr[1])
          delete mSearchStructurePtr[1];

        mSearchStructurePtr[1] = nptr;
      }
      else
      {
        if (mSearchStructurePtr[0])
          delete mSearchStructurePtr[0];

        mSearchStructurePtr[0] = nptr;
      }

      nptr->mProducerInfoList = mProducerInfoList;
      nptr->mGenerationInfoList = mGenerationInfoList;
      nptr->mGeometryInfoList = mGeometryInfoList;
      nptr->mFileInfoList = mFileInfoList;
      nptr->mFileInfoListByName.setReleaseObjects(false);
      nptr->mFileInfoListByName = nptr->mFileInfoList;
      nptr->mContentInfoList[0] = mContentInfoList;

      nptr->mFileInfoList.sort(T::FileInfo::ComparisonMethod::fileId);
      nptr->mFileInfoListByName.sort(T::FileInfo::ComparisonMethod::fileName);

      for (int t=1; t<CONTENT_LIST_COUNT; t++)
      {
        nptr->mContentInfoList[t].setReleaseObjects(false);
        nptr->mContentInfoList[t] = nptr->mContentInfoList[0];
      }

      nptr->mContentInfoList[0].sort(T::ContentInfo::ComparisonMethod::file_message);
      nptr->mContentInfoList[1].sort(T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time);

      uint pLen = nptr->mProducerInfoList.getLength();

      for (uint p=0; p<pLen; p++)
      {
        if (mShutdownRequested)
          return;

        T::ProducerInfo *producerInfo = nptr->mProducerInfoList.getProducerInfoByIndex(p);
        if (producerInfo != nullptr)
        {
          std::size_t generationHash = nptr->mGenerationInfoList.getHashByProducerId(producerInfo->mProducerId);
          std::size_t geometryHash = nptr->mGeometryInfoList.getHashByProducerId(producerInfo->mProducerId);
          std::size_t fileHash = nptr->mFileInfoList.getHashByProducerId(producerInfo->mProducerId);
          std::size_t contentHash = nptr->mContentInfoList[0].getHashByProducerId(producerInfo->mProducerId);

          std::size_t h = 0;
          boost::hash_combine(h,generationHash);
          boost::hash_combine(h,geometryHash);
          boost::hash_combine(h,fileHash);
          boost::hash_combine(h,contentHash);

          producerInfo->mHash = h;
        }
      }


      // ### Updating generation time ranges

      uint glen = nptr->mGenerationInfoList.getLength();
      for (uint g=0; g<glen; g++)
      {
        if (mShutdownRequested)
          return;

        auto ginfo = nptr->mGenerationInfoList.getGenerationInfoByIndex(g);
        nptr->mContentInfoList[1].getForecastTimeRangeByGenerationId(ginfo->mProducerId,ginfo->mGenerationId,ginfo->mContentStartTime,ginfo->mContentEndTime,ginfo->mContentHash);
      }

      PRINT_DATA(mDebugLog, "#### Search structure swapped #######\n");

      if (mActiveSearchStructure == 0)
        mActiveSearchStructure = 1;
      else
        mActiveSearchStructure = 0;

      mContentChangeTime = time(nullptr);
      mContentSwapCounter++;
    }
    else
    {
      // If the swapping is not enabled then we just add the new information into the search structure.

      PRINT_DATA(mDebugLog, "#### Search structure update start #######\n");
      if (!ssp)
      {
        PRINT_DATA(mDebugLog, "  -- Create search structure\n");
        ssp = new SearchStructure();


        ssp->mFileInfoListByName.setReleaseObjects(false);

        ssp->mContentInfoList[0].setReleaseObjects(true);
        for (int t=1; t<CONTENT_LIST_COUNT; t++)
          ssp->mContentInfoList[t].setReleaseObjects(false);

        ssp->mGeometryInfoList.setComparisonMethod(T::GeometryInfo::ComparisonMethod::generationId);
        ssp->mGenerationInfoList.setComparisonMethod(T::GenerationInfo::ComparisonMethod::generationId);
        ssp->mFileInfoList.setComparisonMethod(T::FileInfo::ComparisonMethod::fileId);
        ssp->mFileInfoListByName.setComparisonMethod(T::FileInfo::ComparisonMethod::fileName);
        ssp->mContentInfoList[0].setComparisonMethod(T::ContentInfo::ComparisonMethod::file_message);
        ssp->mContentInfoList[1].setComparisonMethod(T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time);

        mSearchStructurePtr[0] = ssp;
      }

      AutoWriteLock lock(&mModificationLock);
      AutoWriteLock writeLock(&mSearchModificationLock);

      // Enabled locking

      ssp->mProducerInfoList.setLockingEnabled(true);
      ssp->mGenerationInfoList.setLockingEnabled(true);
      ssp->mGeometryInfoList.setLockingEnabled(true);
      ssp->mFileInfoList.setLockingEnabled(true);
      ssp->mFileInfoListByName.setLockingEnabled(true);

      for (int t=0; t<CONTENT_LIST_COUNT; t++)
        ssp->mContentInfoList[t].setLockingEnabled(true);

      // Removing information that is marked to be deleted

      if (mContentDeleteCounter > 0  &&  !mShutdownRequested)
      {
        PRINT_DATA(mDebugLog, "  -- Delete content records (%u)\n",mContentDeleteCounter);
        mContentInfoList.deleteMarkedContent();
        for (int t=CONTENT_LIST_COUNT-1; t>= 0; t--)
          ssp->mContentInfoList[t].deleteMarkedContent();

        mContentDeleteCounter = 0;
        PRINT_DATA(mDebugLog, "     => Content %u\n",ssp->mContentInfoList[0].getLength());
      }

      if (mFileDeleteCounter > 0  &&  !mShutdownRequested)
      {
        PRINT_DATA(mDebugLog, "  -- Delete file records (%u)\n",mFileDeleteCounter);
        mFileInfoList.deleteMarkedFiles();
        ssp->mFileInfoListByName.deleteMarkedFiles();
        ssp->mFileInfoList.deleteMarkedFiles();
        mFileDeleteCounter = 0;
        PRINT_DATA(mDebugLog, "     => Files %u\n",ssp->mFileInfoList.getLength());
      }

      if (mGenerationDeleteCounter > 0  &&  !mShutdownRequested)
      {
        PRINT_DATA(mDebugLog, "  -- Delete generation records (%u)\n",mGenerationDeleteCounter);
        mGenerationInfoList.deleteMarkedGenerations();
        ssp->mGenerationInfoList.deleteMarkedGenerations();
        mGenerationDeleteCounter = 0;
        PRINT_DATA(mDebugLog, "     => Generations %u\n",ssp->mGenerationInfoList.getLength());
      }

      if (mGeometryDeleteCounter > 0  &&  !mShutdownRequested)
      {
        PRINT_DATA(mDebugLog, "  -- Delete geometry records (%u)\n",mGeometryDeleteCounter);
        mGeometryInfoList.deleteMarkedGeometries();
        ssp->mGeometryInfoList.deleteMarkedGeometries();
        mGeometryDeleteCounter = 0;
        PRINT_DATA(mDebugLog, "     => Geometries %u\n",ssp->mGeometryInfoList.getLength());
      }

      if (mProducerDeleteCounter > 0  &&  !mShutdownRequested)
      {
        PRINT_DATA(mDebugLog, "  -- Delete producer records (%u)\n",mProducerDeleteCounter);
        mProducerInfoList.deleteMarkedProducers();
        ssp->mProducerInfoList.deleteMarkedProducers();
        mProducerDeleteCounter = 0;
        PRINT_DATA(mDebugLog, "     => Producers %u\n",ssp->mProducerInfoList.getLength());
      }


      if (mProducerInfoList.getLength() > 0  &&  !mShutdownRequested)
      {
        // Adding producer information into the search structure

        PRINT_DATA(mDebugLog, "  -- Add producers %u\n",mProducerInfoList.getLength());
        uint len = mProducerInfoList.getLength();
        for (uint t=0; t<len; t++)
        {
          T::ProducerInfo *info = mProducerInfoList.getProducerInfoByIndex(t);
          if (info != NULL)
            ssp->mProducerInfoList.addProducerInfo(info->duplicate());
        }
        mProducerInfoList.clear();
        PRINT_DATA(mDebugLog, "     => Producers %u\n",ssp->mProducerInfoList.getLength());
      }


      if (mGeometryInfoList.getLength() > 0  &&  !mShutdownRequested)
      {
        // Adding geometry information into the search structure

        PRINT_DATA(mDebugLog, "  -- Add geometries %u\n",mGeometryInfoList.getLength());
        uint len = mGeometryInfoList.getLength();
        for (uint t=0; t<len; t++)
        {
          T::GeometryInfo *info = mGeometryInfoList.getGeometryInfoByIndex(t);
          if (info != NULL)
            ssp->mGeometryInfoList.addGeometryInfo(info->duplicate());
        }
        mGeometryInfoList.clear();
        PRINT_DATA(mDebugLog, "     => Geometries %u\n",ssp->mGeometryInfoList.getLength());
      }

      if (mGenerationInfoList.getLength() > 0  &&  !mShutdownRequested)
      {
        // Adding generation information into the search structure

        PRINT_DATA(mDebugLog, "  -- Add generations %u\n",mGenerationInfoList.getLength());
        uint len = mGenerationInfoList.getLength();
        for (uint t=0; t<len; t++)
        {
          T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoByIndex(t);
          if (info != NULL)
            ssp->mGenerationInfoList.addGenerationInfo(info->duplicate());
        }
        mGenerationInfoList.clear();
        PRINT_DATA(mDebugLog, "     => Generations %u\n",ssp->mGenerationInfoList.getLength());
      }

      if (mFileInfoList.getLength() > 0  &&  !mShutdownRequested)
      {
        // Adding file information (= moving FileInfo -objects from mFileInfoList into the search structure)

        PRINT_DATA(mDebugLog, "  -- Add files %u\n",mFileInfoList.getLength());
        mFileInfoList.setReleaseObjects(false);
        ssp->mFileInfoList.setReleaseObjects(false);
        ssp->mFileInfoList.addFileInfoList(mFileInfoList);
        ssp->mFileInfoListByName.addFileInfoList(mFileInfoList);
        ssp->mFileInfoList.setReleaseObjects(true);
        mFileInfoList.clear();
        mFileInfoList.setReleaseObjects(true);
        mFileInfoList.setComparisonMethod(T::FileInfo::ComparisonMethod::fileId);
        PRINT_DATA(mDebugLog, "     => Files %u\n",ssp->mFileInfoList.getLength());
      }


      if (mContentInfoList.getLength() > 0  &&  !mShutdownRequested)
      {
        PRINT_DATA(mDebugLog, "  -- Add content %u\n",mContentInfoList.getLength());
        // Adding content information (= moving ContentInfo -objects from mContentInfoList into the search structure)

        mContentInfoList.setReleaseObjects(false);
        ssp->mContentInfoList[0].setReleaseObjects(false);
        for (int t=0; t<CONTENT_LIST_COUNT; t++)
        {
          ssp->mContentInfoList[t].addContentInfoList(mContentInfoList);
        }
        ssp->mContentInfoList[0].setReleaseObjects(true);
        mContentInfoList.clear();
        mContentInfoList.setReleaseObjects(true);
        mContentInfoList.setComparisonMethod(T::ContentInfo::ComparisonMethod::file_message);
        PRINT_DATA(mDebugLog, "     => Content %u\n",ssp->mContentInfoList[0].getLength());
      }

      uint pLen = ssp->mProducerInfoList.getLength();

      for (uint p=0; p<pLen; p++)
      {
        if (mShutdownRequested)
          return;

        T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoByIndex(p);
        if (producerInfo != nullptr)
        {
          std::size_t generationHash = ssp->mGenerationInfoList.getHashByProducerId(producerInfo->mProducerId);
          std::size_t geometryHash = ssp->mGeometryInfoList.getHashByProducerId(producerInfo->mProducerId);
          std::size_t fileHash = ssp->mFileInfoList.getHashByProducerId(producerInfo->mProducerId);
          std::size_t contentHash = ssp->mContentInfoList[0].getHashByProducerId(producerInfo->mProducerId);

          std::size_t h = 0;
          boost::hash_combine(h,generationHash);
          boost::hash_combine(h,geometryHash);
          boost::hash_combine(h,fileHash);
          boost::hash_combine(h,contentHash);

          producerInfo->mHash = h;
        }
      }


      // ### Updating generation time ranges


      PRINT_DATA(mDebugLog, "  -- Updating generation time ranges\n");
      uint glen = ssp->mGenerationInfoList.getLength();
      for (uint g=0; g<glen; g++)
      {
        auto ginfo = ssp->mGenerationInfoList.getGenerationInfoByIndex(g);
        ssp->mContentInfoList[1].getForecastTimeRangeByGenerationId(ginfo->mProducerId,ginfo->mGenerationId,ginfo->mContentStartTime,ginfo->mContentEndTime,ginfo->mContentHash);
      }

      // Disabled locking

      ssp->mProducerInfoList.setLockingEnabled(false);
      ssp->mGenerationInfoList.setLockingEnabled(false);
      ssp->mGeometryInfoList.setLockingEnabled(false);
      ssp->mFileInfoList.setLockingEnabled(false);
      ssp->mFileInfoListByName.setLockingEnabled(false);

      for (int t=0; t<CONTENT_LIST_COUNT; t++)
        ssp->mContentInfoList[t].setLockingEnabled(false);

      mContentChangeTime = time(nullptr);

      PRINT_DATA(mDebugLog, "#### Search structure update end #######\n");
    }

    mContentUpdateTime = time(nullptr);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}


}
}
