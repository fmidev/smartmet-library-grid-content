#include "MergeImplementation.h"

#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/common/AutoThreadLock.h>
#include <grid-files/identification/GridDef.h>
#include <grid-files/common/ShowFunction.h>
#include <macgyver/Hash.h>
#include <boost/make_shared.hpp>
#include <boost/thread/thread.hpp>
#include <atomic>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF

namespace SmartMet
{
namespace ContentServer
{


static void* MergeImplementation_eventProcessingThread(void *arg)
{
  try
  {
    MergeImplementation *service = (MergeImplementation*)arg;
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



inline uint getNewId(uint contentStorageIndex,uint oldId)
{
  uint id = (contentStorageIndex << 24) + (oldId & 0x00FFFFFF);
  return id;
}


inline T::ProducerId getNewProducerId(uint contentStorageIndex,T::ProducerId oldId)
{
  T::ProducerId id = (contentStorageIndex << 24) + (oldId & 0x00FFFFFF);
  return id;
}


inline T::GenerationId getNewGenerationId(uint contentStorageIndex,T::GenerationId oldId)
{
  T::GenerationId id = ((UInt64)contentStorageIndex << 32) + (oldId & 0xFFFFFFFF);
  return id;
}


inline T::FileId getNewFileId(uint contentStorageIndex,T::FileId oldId)
{
  T::FileId id = ((UInt64)contentStorageIndex << 32) + (oldId & 0xFFFFFFFF);
  return id;
}



inline T::SourceId getNewSourceId(uint contentStorageIndex,T::SourceId oldId)
{
  T::SourceId id = (contentStorageIndex << 24) + (oldId & 0x00FFFFFF);
  return id;
}



MergeImplementation::MergeImplementation()
{
  FUNCTION_TRACE
  try
  {
    mImplementationType = Implementation::Cache;
    mStartTime = 0;
    mSessionId = 0;
    mEventCounter = 0;
    mShutdownRequested = false;
    mUpdateInProgress = false;
    mReloadActivated = false;
    mProducerDeleteCounter = 0;
    mGenerationDeleteCounter = 0;
    mGeometryDeleteCounter = 0;
    mFileDeleteCounter = 0;
    mContentDeleteCounter = 0;
    mActiveSearchStructure = 0;

    //mFileInfoList.setComparisonMethod(T::FileInfo::ComparisonMethod::none);
    //mContentInfoList.setComparisonMethod(T::ContentInfo::ComparisonMethod::file_message);
    mGenerationInfoList.setComparisonMethod(T::GenerationInfo::ComparisonMethod::generationId);

    mProducerInfoList.setLockingEnabled(true);
    mGenerationInfoList.setLockingEnabled(true);
    //mFileInfoList.setLockingEnabled(true);
    //mContentInfoList.setLockingEnabled(true);

    mContentUpdateTime = 0;
    mContentUpdateInterval = 180;
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





MergeImplementation::~MergeImplementation()
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




void MergeImplementation::getCacheStats(Fmi::Cache::CacheStatistics& statistics) const
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





void MergeImplementation::init(T::SessionId sessionId,T::SessionId dataServerSessionId,ContentServer_sptr_vec& contentStorages)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"* Init start\n");
    mStartTime = time(nullptr);
    mSessionId = sessionId;
    mDataServerSessionId = dataServerSessionId;

    for (auto it = contentStorages.begin(); it != contentStorages.end(); ++it)
    {
      ContentSource source;
      source.mContentStorage = *it;
      mContentSources.push_back(source);
    }

    uint len = mContentSources.size();
    mUpdateInProgress = true;
    AutoWriteLock lock(&mModificationLock);

    for (uint t=0; t<len; t++)
    {
      if (mContentSources[t].mContentStorage != nullptr)
      {

        T::EventInfo eventInfo;
        eventInfo.mEventId = 0;
        if (mContentSources[t].mContentStorage->getLastEventInfo(sessionId,0,eventInfo) == 0)
        {
          mContentSources[t].mLastProcessedEventId = eventInfo.mEventId;
          mContentSources[t].mContentStorageStartTime = eventInfo.mServerTime;
        }

        readProducerList(t,mContentSources[t].mContentStorage);
        readGenerationList(t,mContentSources[t].mContentStorage);
        readGeometryList(t,mContentSources[t].mContentStorage);
        readFileList(t,mContentSources[t].mContentStorage);

        //mFileInfoList.sort(T::FileInfo::ComparisonMethod::fileId);

        readContentList(t,mContentSources[t].mContentStorage);
      }

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







bool MergeImplementation::isReady()
{
  FUNCTION_TRACE
  try
  {
    if (mContentSwapCounter > 0)
      return true;
    else
      return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::setContentUpdateInterval(uint intervalInSec)
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





void MergeImplementation::getStateAttributes(std::shared_ptr<T::AttributeNode> parent)
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

    //auto events = parent->addAttribute("Events");
    //events->addAttribute("Last event id",mLastProcessedEventId);

    auto swap = parent->addAttribute("Content Swap");
    long diff = time(nullptr) - mContentUpdateTime;
    swap->addAttribute("Last Swap (seconds ago)",diff);
    swap->addAttribute("Swap Interval (seconds)",mContentUpdateInterval);

    ServiceInterface::getStateAttributes(parent);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::setContentSwap(uint fileCacheMaxFirstWaitTime,uint fileCacheMaxWaitTime)
{
  FUNCTION_TRACE
  try
  {
    mCachedFiles_maxWaitTime = fileCacheMaxWaitTime;
    mCachedFiles_maxFirstWaitTime = fileCacheMaxFirstWaitTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::setEventListMaxLength(uint maxLength)
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




/*
std::string& MergeImplementation::getSourceInfo()
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
*/




void MergeImplementation::startEventProcessing()
{
  FUNCTION_TRACE
  try
  {
    pthread_create(&mThread,nullptr,MergeImplementation_eventProcessingThread,this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::shutdown()
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





bool MergeImplementation::isSessionValid(T::SessionId sessionId)
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





int MergeImplementation::_clear(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_reload(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    /*
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    mContentStorage->reload(sessionId);
  */

    reloadData();
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::reloadData()
{
  FUNCTION_TRACE
  try
  {
    mReloadActivated = true;

    uint len = mContentSources.size();
    mUpdateInProgress = true;
    AutoWriteLock lock(&mModificationLock);

    mFileInfoList.clear();
    mProducerInfoList.clear();
    mGenerationInfoList.clear();
    mGeometryInfoList.clear();
    mContentInfoList.clear();
    mEventInfoList.clear();

    for (uint t=0; t<len; t++)
    {
      if (mContentSources[t].mContentStorage != nullptr)
      {

        T::EventInfo eventInfo;
        eventInfo.mEventId = 0;
        if (mContentSources[t].mContentStorage->getLastEventInfo(mSessionId,0,eventInfo) == 0)
        {
          mContentSources[t].mLastProcessedEventId = eventInfo.mEventId;
          mContentSources[t].mContentStorageStartTime = eventInfo.mServerTime;
        }

        readProducerList(t,mContentSources[t].mContentStorage);
        readGenerationList(t,mContentSources[t].mContentStorage);
        readGeometryList(t,mContentSources[t].mContentStorage);
        readFileList(t,mContentSources[t].mContentStorage);

        //mFileInfoList.sort(T::FileInfo::ComparisonMethod::fileId);

        readContentList(t,mContentSources[t].mContentStorage);
      }
    }
    mContentUpdateTime = 0;
    mUpdateInProgress = false;
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





int MergeImplementation::_getContentChangeTime(T::SessionId sessionId,time_t& changeTime)
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





int MergeImplementation::_addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_setProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteProducerInfoById(T::SessionId sessionId,T::ProducerId producerId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteProducerInfoByName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getProducerInfoById(T::SessionId sessionId,T::ProducerId producerId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      if (!mProducerInfoList.getProducerInfoById(producerId,producerInfo))
        return Result::DATA_NOT_FOUND;

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!ssp->mProducerInfoList.getProducerInfoById(producerId,producerInfo))
      return Result::DATA_NOT_FOUND;

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteProducerInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getProducerInfoByName(T::SessionId sessionId,const std::string& producerName,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      if (!mProducerInfoList.getProducerInfoByName(producerName,producerInfo))
        return Result::DATA_NOT_FOUND;

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!ssp->mProducerInfoList.getProducerInfoByName(producerName,producerInfo))
      return Result::DATA_NOT_FOUND;

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      producerInfoList = mProducerInfoList;
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    producerInfoList.clear();
    producerInfoList = ssp->mProducerInfoList;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    producerInfoList.clear();

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    T::ContentInfoList contentInfoList;

    time_t startTime = 0;
    time_t endTime = 0xFFFFFFFF;

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

    std::set<T::ProducerId> producerIdList;
    uint len = contentInfoList.getLength();
    T::ProducerId prevProducerId = 0;
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




int MergeImplementation::_getProducerInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    producerInfoList.clear();

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mProducerInfoList.getProducerInfoListBySourceId(sourceId,producerInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ssp->mProducerInfoList.getProducerInfoListBySourceId(sourceId,producerInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getProducerInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      count = mProducerInfoList.getLength();
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    count = ssp->mProducerInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    list.clear();

    uint pLen = ssp->mProducerInfoList.getLength();
    for (uint p=0; p<pLen; p++)
    {
      T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoByIndex(p);
      if (producerInfo != nullptr)
      {
        std::set<T::GeometryId> geometryIdList;

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





int MergeImplementation::_getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    list.clear();

    T::ProducerInfoList producerList = ssp->mProducerInfoList;
    std::map<T::ProducerId,T::ProducerInfo*> producers;
    uint plen = producerList.getLength();
    for (uint t=0; t<plen; t++)
    {
      T::ProducerInfo *prod = producerList.getProducerInfoByIndex(t);
      if (prod != nullptr)
        producers.insert(std::pair<T::ProducerId,T::ProducerInfo*>(prod->mProducerId,prod));
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
          Fmi::hash_merge(seed,contentInfo->mProducerId);
          Fmi::hash_merge(seed,sourceParamKey);
          Fmi::hash_merge(seed,targetParameterKeyType);
          Fmi::hash_merge(seed,targetParamKey);
          Fmi::hash_merge(seed,contentInfo->mGeometryId);
          Fmi::hash_merge(seed,paramLevelId);
          Fmi::hash_merge(seed,contentInfo->mParameterLevel);
          Fmi::hash_merge(seed,contentInfo->mForecastType);
          Fmi::hash_merge(seed,contentInfo->mForecastNumber);


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





int MergeImplementation::_getProducerParameterListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    list.clear();

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
          Fmi::hash_merge(seed,contentInfo->mProducerId);
          Fmi::hash_merge(seed,sourceParamKey);
          Fmi::hash_merge(seed,targetParameterKeyType);
          Fmi::hash_merge(seed,targetParamKey);
          Fmi::hash_merge(seed,contentInfo->mGeometryId);
          Fmi::hash_merge(seed,paramLevelId);
          Fmi::hash_merge(seed,contentInfo->mParameterLevel);
          Fmi::hash_merge(seed,contentInfo->mForecastType);
          Fmi::hash_merge(seed,contentInfo->mForecastNumber);


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





int MergeImplementation::_addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_setGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteGenerationInfoById(T::SessionId sessionId,T::GenerationId generationId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteGenerationInfoByName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<T::GenerationId>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteGenerationInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteGenerationInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    list.clear();

    uint len = ssp->mContentInfoList[0].getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = ssp->mContentInfoList[0].getContentInfoByIndex(t);
      if (info != nullptr)
      {
        char st[200];
        sprintf(st,"%u;%lu;%u;%d;%d;%s;%ld;%ld;",info->mSourceId,info->mGenerationId,info->mGeometryId,info->mForecastType,info->mForecastNumber,info->getForecastTime(),info->mModificationTime,info->mDeletionTime);
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





int MergeImplementation::_getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    generationInfoList.clear();

    std::set<T::GenerationId> idList;

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





int MergeImplementation::_getGenerationInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      if (!mGenerationInfoList.getGenerationInfoById(generationId,generationInfo))
        return Result::DATA_NOT_FOUND;

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!ssp->mGenerationInfoList.getGenerationInfoById(generationId,generationInfo))
      return Result::DATA_NOT_FOUND;

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getGenerationInfoByName(T::SessionId sessionId,const std::string& generationName,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      if (!mGenerationInfoList.getGenerationInfoByName(generationName,generationInfo))
        return Result::DATA_NOT_FOUND;

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!ssp->mGenerationInfoList.getGenerationInfoByName(generationName,generationInfo))
      return Result::DATA_NOT_FOUND;

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    generationInfoList.clear();

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      generationInfoList = mGenerationInfoList;
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    generationInfoList = ssp->mGenerationInfoList;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getGenerationInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    generationInfoList.clear();

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mGenerationInfoList.getGenerationInfoListByProducerId(producerId,generationInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ssp->mGenerationInfoList.getGenerationInfoListByProducerId(producerId,generationInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    generationInfoList.clear();

    if (sessionId == mDataServerSessionId)
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

    T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    ssp->mGenerationInfoList.getGenerationInfoListByProducerId(producerInfo->mProducerId,generationInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getGenerationInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    generationInfoList.clear();

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mGenerationInfoList.getGenerationInfoListBySourceId(sourceId,generationInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ssp->mGenerationInfoList.getGenerationInfoListBySourceId(sourceId,generationInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,T::ProducerId producerId,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
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





int MergeImplementation::_getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,const std::string& producerName,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
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





int MergeImplementation::_getGenerationInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      count = mGenerationInfoList.getLength();
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    count = ssp->mGenerationInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_setGenerationInfoStatusById(T::SessionId sessionId,T::GenerationId generationId,uchar status)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_setGenerationInfoStatusByName(T::SessionId sessionId,const std::string& generationName,uchar status)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_addGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MergeImplementation::_deleteGeometryInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteGeometryInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteGeometryInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteGeometryInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getGeometryInfoById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      if (!mGeometryInfoList.getGeometryInfoById(generationId,geometryId,levelId,geometryInfo))
        return Result::DATA_NOT_FOUND;

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!ssp->mGeometryInfoList.getGeometryInfoById(generationId,geometryId,levelId,geometryInfo))
      return Result::DATA_NOT_FOUND;

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MergeImplementation::_getGeometryInfoList(T::SessionId sessionId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    geometryInfoList.clear();

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      geometryInfoList = mGeometryInfoList;
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    geometryInfoList = ssp->mGeometryInfoList;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getGeometryInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    geometryInfoList.clear();

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mGeometryInfoList.getGeometryInfoListByGenerationId(generationId,geometryInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ssp->mGeometryInfoList.getGeometryInfoListByGenerationId(generationId,geometryInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MergeImplementation::_getGeometryInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    geometryInfoList.clear();

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mGeometryInfoList.getGeometryInfoListByProducerId(producerId,geometryInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ssp->mGeometryInfoList.getGeometryInfoListByProducerId(producerId,geometryInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getGeometryInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    geometryInfoList.clear();

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mGeometryInfoList.getGeometryInfoListBySourceId(sourceId,geometryInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ssp->mGeometryInfoList.getGeometryInfoListBySourceId(sourceId,geometryInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getGeometryInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      count = mGeometryInfoList.getLength();
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    count = ssp->mGeometryInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_setGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_setGeometryInfoStatusById(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,uchar status)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_setFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteFileInfoById(T::SessionId sessionId,T::FileId fileId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteFileInfoByName(T::SessionId sessionId,const std::string& filename)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteFileInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteFileInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,time_t forecastTime)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteFileInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<T::FileId>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getFileInfoById(T::SessionId sessionId,T::FileId fileId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      if (!mFileInfoList.getFileInfoById(fileId,fileInfo))
        return Result::DATA_NOT_FOUND;

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!ssp->mFileInfoList.getFileInfoById(fileId,fileInfo))
      return Result::DATA_NOT_FOUND;

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getFileInfoByName(T::SessionId sessionId,const std::string& filename,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      if (!mFileInfoList.getFileInfoByName(filename,fileInfo))
        return Result::DATA_NOT_FOUND;

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    if (!ssp->mFileInfoListByName.getFileInfoByName(filename,fileInfo))
      return Result::DATA_NOT_FOUND;

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getFileInfoList(T::SessionId sessionId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    fileInfoList.clear();

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mFileInfoList.getFileInfoList(startFileId,maxRecords,fileInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ssp->mFileInfoList.getFileInfoList(startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<T::FileId>& fileIdList,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    fileInfoList.clear();

    if (sessionId == mDataServerSessionId)
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




int MergeImplementation::_getFileInfoListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
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

    ssp->mFileInfoList.getFileInfoListByProducerId(producerId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
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

    T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    ssp->mFileInfoList.getFileInfoListByProducerId(producerInfo->mProducerId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getFileInfoListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    fileInfoList.clear();

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mFileInfoList.getFileInfoListByGenerationId(generationId,startFileId,maxRecords,fileInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ssp->mFileInfoList.getFileInfoListByGenerationId(generationId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    fileInfoList.clear();

    if (sessionId == mDataServerSessionId)
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

    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    ssp->mFileInfoList.getFileInfoListByGenerationId(generationInfo->mGenerationId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getFileInfoListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::FileId startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    fileInfoList.clear();

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mFileInfoList.getFileInfoListBySourceId(sourceId,startFileId,maxRecords,fileInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ssp->mFileInfoList.getFileInfoListBySourceId(sourceId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getFileInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      count = mFileInfoList.getLength();
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    count = ssp->mFileInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getFileInfoCountByProducerId(T::SessionId sessionId,T::ProducerId producerId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      count = mFileInfoList.getFileInfoCountByProducerId(producerId);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    count = ssp->mFileInfoList.getFileInfoCountByProducerId(producerId);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MergeImplementation::_getFileInfoCountByGenerationId(T::SessionId sessionId,T::GenerationId generationId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      count = mFileInfoList.getFileInfoCountByGenerationId(generationId);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    count = ssp->mFileInfoList.getFileInfoCountByGenerationId(generationId);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getFileInfoCountBySourceId(T::SessionId sessionId,T::SourceId sourceId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      count = mFileInfoList.getFileInfoCountBySourceId(sourceId);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    count = ssp->mFileInfoList.getFileInfoCountBySourceId(sourceId);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





T::EventId MergeImplementation::addEvent(uint eventType,UInt64 id1,UInt64 id2,UInt64 id3,UInt64 flags)
{
  FUNCTION_TRACE
  try
  {
    mEventCounter++;

    T::EventInfo *eventInfo = new T::EventInfo(mStartTime,mEventCounter,eventType,id1,id2,id3,flags,"");
    eventInfo->mServerTime = mStartTime;
    mEventInfoList.addEventInfo(eventInfo);

    return mEventCounter;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MergeImplementation::_addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  //FUNCTION_TRACE
  try
  {
    eventInfo.mServerTime = mStartTime; //mContentStorageStartTime;

    if (mReloadActivated)
      return Result::DATA_NOT_FOUND;

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




int MergeImplementation::_getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    eventInfoList.clear();

    T::EventInfoList eventList;
    mEventInfoList.getEventInfoList(startEventId,maxRecords,eventList);


    T::EventInfo *event = eventList.getFirstEvent();
    while (event != nullptr)
    {
      T::EventInfo *eventInfo = new T::EventInfo(*event);
      if (eventInfo->mType == EventType::FILE_ADDED)
      {

        T::FileInfo *fileInfo = mFileInfoList.getFileInfoById(eventInfo->mId1);
        if (fileInfo != nullptr)
        {
          std::ostringstream output;
          output << fileInfo->getCsv() << "\n";

          if (eventInfo->mId3 > 0)
          {
            T::ContentInfoList contentInfoList;
            mContentInfoList.getContentInfoListByFileId(fileInfo->mFileId,contentInfoList);

            uint len = contentInfoList.getLength();
            for (uint t=0; t<len; t++)
            {
              T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
              output << contentInfo->getCsv() << "\n";
            }
          }
          eventInfo->mEventData = output.str();
        }
      }
      else
      if (eventInfo->mType == EventType::CONTENT_ADDED)
      {
        T::ContentInfo *contentInfo = mContentInfoList.getContentInfoByFileIdAndMessageIndex(eventInfo->mId1,eventInfo->mId2);

        if (contentInfo != nullptr)
        {
          eventInfo->mEventData = contentInfo->getCsv();
        }
        else
        {
         // Content not found
        }
      }

      eventInfoList.addEventInfo(eventInfo);
      event = event->nextItem;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*
int MergeImplementation::_getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList)
{
  //FUNCTION_TRACE
  try
  {
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
*/




int MergeImplementation::_getEventInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
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





int MergeImplementation::_addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_setContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteContentInfo(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteContentListByFileId(T::SessionId sessionId,T::FileId fileId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteContentListByFileName(T::SessionId sessionId,const std::string& filename)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteContentListByProducerId(T::SessionId sessionId,T::ProducerId producerId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteContentListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteContentListByGenerationId(T::SessionId sessionId,T::GenerationId generationId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteContentListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_deleteContentListBySourceId(T::SessionId sessionId,T::SourceId sourceId)
{
  FUNCTION_TRACE
  try
  {
    return Result::NO_PERMANENT_STORAGE_DEFINED;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentInfo(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
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

    T::ContentInfo *cInfo = ssp->mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(fileId,messageIndex);
    if (cInfo != nullptr)
    {
      contentInfo = *cInfo;
      return Result::OK;
    }
    return Result::DATA_NOT_FOUND;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentList(T::SessionId sessionId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mContentInfoList.getContentInfoList(startFileId,startMessageIndex,maxRecords,contentInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;


    ssp->mContentInfoList[0].getContentInfoList(startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentListByFileId(T::SessionId sessionId,T::FileId fileId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mContentInfoList.getContentInfoListByFileId(fileId,contentInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ssp->mContentInfoList[0].getContentInfoListByFileId(fileId,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentListByFileIdList(T::SessionId sessionId,std::vector<T::FileId>& fileIdList,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (sessionId == mDataServerSessionId)
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





int MergeImplementation::_getContentListByFileName(T::SessionId sessionId,const std::string& filename,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (sessionId == mDataServerSessionId)
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

    T::FileInfo *fileInfo = ssp->mFileInfoListByName.getFileInfoByName(filename);
    if (fileInfo == nullptr)
      return Result::UNKNOWN_FILE_NAME;

    ssp->mContentInfoList[0].getContentInfoListByFileId(fileInfo->mFileId,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentListByProducerId(T::SessionId sessionId,T::ProducerId producerId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (sessionId == mDataServerSessionId)
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

    ssp->mContentInfoList[0].getContentInfoListByProducerId(producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentListByProducerName(T::SessionId sessionId,const std::string& producerName,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (sessionId == mDataServerSessionId)
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

    T::ProducerInfo *producerInfo = ssp->mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    ssp->mContentInfoList[0].getContentInfoListByProducerId(producerInfo->mProducerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MergeImplementation::_getContentListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (sessionId == mDataServerSessionId)
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

    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    if (requestFlags < CONTENT_LIST_COUNT)
      ssp->mContentInfoList[requestFlags].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    else
      ssp->mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentListByGenerationName(T::SessionId sessionId,const std::string& generationName,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (sessionId == mDataServerSessionId)
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

    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    ssp->mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,T::GenerationId generationId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (sessionId == mDataServerSessionId)
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

    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    ssp->mContentInfoList[1].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startTime,endTime,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (sessionId == mDataServerSessionId)
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

    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    ssp->mContentInfoList[1].getContentInfoListByGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,startTime,endTime,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentListBySourceId(T::SessionId sessionId,T::SourceId sourceId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mContentInfoList.getContentInfoListBySourceId(sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ssp->mContentInfoList[0].getContentInfoListBySourceId(sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

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

    ssp->mContentInfoList[1].getContentInfoListByFmiParameterId(fmiId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentListByParameterAndGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

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





int MergeImplementation::_getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

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




int MergeImplementation::_getContentListByParameterAndProducerId(T::SessionId sessionId,T::ProducerId producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

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




int MergeImplementation::_getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

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





int MergeImplementation::_getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

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





int MergeImplementation::_getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
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
          PRINT_DATA(mDebugLog,"**** INTEGRITY ERROR : File missing (%lu)! *****\n",cInfo->mFileId);
          cError = cInfo;
        }

        if (cError == nullptr)
        {
          T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(cInfo->mGenerationId);
          if (generationInfo == nullptr)
          {
            PRINT_DATA(mDebugLog,"**** INTEGRITY ERROR : Generation missing (%lu)! *****\n",cInfo->mGenerationId);
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





int MergeImplementation::_getContentGeometryIdListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    geometryIdList.clear();

    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    ssp->mContentInfoList[1].getContentGeometryIdListByGenerationId(generationInfo->mProducerId,generationId,geometryIdList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentParamListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ContentInfoList& contentParamList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

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





int MergeImplementation::_getContentParamKeyListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    paramKeyList.clear();

    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    ssp->mContentInfoList[1].getContentParamKeyListByGenerationId(generationInfo->mProducerId,generationId,parameterKeyType,paramKeyList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentParamKeyListByGenerationAndGeometryId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    paramKeyList.clear();

    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    ssp->mContentInfoList[1].getContentParamKeyListByGenerationAndGeometryId(generationInfo->mProducerId,generationId,geometryId,parameterKeyType,paramKeyList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentParamKeyListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    paramKeyList.clear();

    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    ssp->mContentInfoList[1].getContentParamKeyListByGenerationGeometryAndLevelId(generationInfo->mProducerId,generationId,geometryId,levelId,parameterKeyType,paramKeyList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentLevelListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    contentLevelList.clear();

    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    ssp->mContentInfoList[1].getContentLevelListByGenerationGeometryAndLevelId(generationInfo->mProducerId,generationId,geometryId,levelId,contentLevelList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentLevelListByParameterGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,std::string parameterKey,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    contentLevelList.clear();

    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    ssp->mContentInfoList[1].getContentLevelListByParameterGenerationGeometryAndLevelId(generationInfo->mProducerId,generationId,geometryId,parameterKey,levelId,contentLevelList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MergeImplementation::_getContentTimeListByGenerationId(T::SessionId sessionId,T::GenerationId generationId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    contentTimeList.clear();

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

        mContentTimeCache.insert(std::pair<T::GenerationId,std::set<std::string>>(generationId,contentTimeList));
        mContentTimeCache_modificationLock.writeUnlock();
      }
    }
    else
    {
      contentTimeList = it->second;
    }
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentTimeRangeByProducerAndGenerationId(T::SessionId sessionId,T::ProducerId producerId,T::GenerationId generationId,time_t& startTime,time_t& endTime)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

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
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentTimeRangeByGenerationId(T::SessionId sessionId,T::GenerationId generationId,time_t& startTime,time_t& endTime)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

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
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    contentTimeList.clear();

    AutoReadLock readLock(&mSearchModificationLock);
    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    ssp->mContentInfoList[1].getForecastTimeListByGenerationAndGeometry(generationInfo->mProducerId,generationId,geometryId,contentTimeList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentTimeListByGenerationGeometryAndLevelId(T::SessionId sessionId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    contentTimeList.clear();

    AutoReadLock readLock(&mSearchModificationLock);
    T::GenerationInfo *generationInfo = ssp->mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    ssp->mContentInfoList[1].getForecastTimeListByGenerationGeometryAndLevelId(generationInfo->mProducerId,generationId,geometryId,levelId,contentTimeList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentTimeListByProducerId(T::SessionId sessionId,T::ProducerId producerId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    contentTimeList.clear();

    ssp->mContentInfoList[1].getForecastTimeListByProducerId(producerId,contentTimeList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getContentCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      count = mContentInfoList.getLength();
      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    count = ssp->mContentInfoList[0].getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getHashByProducerId(T::SessionId sessionId,T::ProducerId producerId,UInt64 & hash)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      std::size_t generationHash = mGenerationInfoList.getHashByProducerId(producerId);
      std::size_t geometryHash = mGeometryInfoList.getHashByProducerId(producerId);
      std::size_t fileHash = mFileInfoList.getHashByProducerId(producerId);
      std::size_t contentHash = mContentInfoList.getHashByProducerId(producerId);

      std::size_t h = 0;
      Fmi::hash_merge(h,generationHash);
      Fmi::hash_merge(h,geometryHash);
      Fmi::hash_merge(h,fileHash);
      Fmi::hash_merge(h,contentHash);

      hash = h;

      return Result::OK;
    }

    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    std::size_t generationHash = ssp->mGenerationInfoList.getHashByProducerId(producerId);
    std::size_t geometryHash = ssp->mGeometryInfoList.getHashByProducerId(producerId);
    std::size_t fileHash = ssp->mFileInfoList.getHashByProducerId(producerId);
    std::size_t contentHash = ssp->mContentInfoList[0].getHashByProducerId(producerId);

    std::size_t h = 0;
    Fmi::hash_merge(h,generationHash);
    Fmi::hash_merge(h,geometryHash);
    Fmi::hash_merge(h,fileHash);
    Fmi::hash_merge(h,contentHash);

    hash = h;

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    levelInfoList.clear();
/*
    if (sessionId == mDataServerSessionId)
    {
      AutoReadLock lock(&mModificationLock);
      mContentInfoList.getLevelInfoList(levelInfoList);
      return Result::OK;
    }
*/
    auto ssp = mSearchStructurePtr[mActiveSearchStructure];
    if (!ssp)
      return Result::DATA_NOT_FOUND;

    ssp->mContentInfoList[0].getLevelInfoList(levelInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MergeImplementation::_updateCachedFiles(T::SessionId sessionId,std::set<T::FileId>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    if (sessionId == mDataServerSessionId)
    {
      AutoWriteLock lock(&mModificationLock);

      mCachedFiles.clear();
      for (auto fileId : fileIdList)
        mCachedFiles.insert(static_cast<uint>(fileId));
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::readProducerList(uint contentStorageIndex,ContentServer_sptr contentStorage)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"* Reading the producer list\n");

    if (contentStorage == nullptr)
      return;

    if (mShutdownRequested)
      return;

    T::ProducerInfoList producerInfoList;
    int result = contentStorage->getProducerInfoList(mSessionId,producerInfoList);
    if (result != 0)
    {
      Fmi::Exception exception(BCP,"Cannot read the producer list from the content storage!");
      exception.addParameter("ServiceResult",getResultString(result));
      throw exception;
    }

    uint len = producerInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      if (mShutdownRequested)
        return;

      T::ProducerInfo *producerInfo = producerInfoList.getProducerInfoByIndex(t);
      if (producerInfo != nullptr)
      {
        producerInfo->mProducerId = getNewProducerId(contentStorageIndex,producerInfo->mProducerId);
        producerInfo->mSourceId = getNewSourceId(contentStorageIndex,producerInfo->mSourceId);
        producerInfo->mStorageId = contentStorageIndex;

        mProducerInfoList.addProducerInfo(producerInfo->duplicate());
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void MergeImplementation::readGenerationList(uint contentStorageIndex,ContentServer_sptr contentStorage)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"* Reading the generation list\n");

    if (contentStorage == nullptr)
      return;

    if (mShutdownRequested)
      return;

    T::GenerationInfoList generationInfoList;

    int result = contentStorage->getGenerationInfoList(mSessionId,generationInfoList);
    if (result != 0)
    {
      Fmi::Exception exception(BCP,"Cannot read the generation list from the content storage!");
      exception.addParameter("ServiceResult",getResultString(result));
      throw exception;
    }

    uint len = generationInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      if (mShutdownRequested)
        return;

      T::GenerationInfo *generationInfo = generationInfoList.getGenerationInfoByIndex(t);
      if (generationInfo != nullptr)
      {
        generationInfo->mProducerId = getNewProducerId(contentStorageIndex,generationInfo->mProducerId);
        generationInfo->mGenerationId = getNewGenerationId(contentStorageIndex,generationInfo->mGenerationId);
        generationInfo->mSourceId = getNewSourceId(contentStorageIndex,generationInfo->mSourceId);
        generationInfo->mStorageId = contentStorageIndex;

        mGenerationInfoList.addGenerationInfo(generationInfo->duplicate());
      }
    }

    mGenerationInfoList.setComparisonMethod(T::GenerationInfo::ComparisonMethod::generationId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void MergeImplementation::readGeometryList(uint contentStorageIndex,ContentServer_sptr contentStorage)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"* Reading the generation list\n");

    if (contentStorage == nullptr)
      return;

    if (mShutdownRequested)
      return;

    T::GeometryInfoList geometryInfoList;

    int result = contentStorage->getGeometryInfoList(mSessionId,geometryInfoList);
    if (result != 0)
    {
      Fmi::Exception exception(BCP,"Cannot read the geometry list from the content storage!");
      exception.addParameter("ServiceResult",getResultString(result));
      throw exception;
    }

    uint len = geometryInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      if (mShutdownRequested)
        return;

      T::GeometryInfo *geometryInfo = geometryInfoList.getGeometryInfoByIndex(t);
      if (geometryInfo != nullptr)
      {
        geometryInfo->mProducerId = getNewProducerId(contentStorageIndex,geometryInfo->mProducerId);
        geometryInfo->mGenerationId = getNewGenerationId(contentStorageIndex,geometryInfo->mGenerationId);
        //geometryInfo->mGeometryId = getNewIntId(contentStorageIndex,geometryInfo->mGeometryId);
        geometryInfo->mSourceId = getNewSourceId(contentStorageIndex,geometryInfo->mSourceId);
        geometryInfo->mStorageId = contentStorageIndex;

        mGeometryInfoList.addGeometryInfo(geometryInfo->duplicate());
      }
    }

    mGeometryInfoList.setComparisonMethod(T::GeometryInfo::ComparisonMethod::generationId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void MergeImplementation::readFileList(uint contentStorageIndex,ContentServer_sptr contentStorage)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"* Reading the file list\n");

    if (contentStorage == nullptr)
      return;

    if (mShutdownRequested)
      return;

    //mFileInfoList.clear();
    //mFileInfoList.setComparisonMethod(T::FileInfo::ComparisonMethod::none);


    T::FileId startFileId = 0;
    uint len = 50000;
    while (len > 0)
    {
      if (mShutdownRequested)
        return;

      T::FileInfoList fileInfoList;

      int result = contentStorage->getFileInfoList(mSessionId,startFileId,50000,fileInfoList);
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

          fileInfo->mProducerId = getNewProducerId(contentStorageIndex,fileInfo->mProducerId);
          fileInfo->mGenerationId = getNewGenerationId(contentStorageIndex,fileInfo->mGenerationId);
          fileInfo->mFileId = getNewFileId(contentStorageIndex,fileInfo->mFileId);
          fileInfo->mSourceId = getNewSourceId(contentStorageIndex,fileInfo->mSourceId);
          fileInfo->mStorageId = contentStorageIndex;

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




void MergeImplementation::readContentList(uint contentStorageIndex,ContentServer_sptr contentStorage)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"* Reading the content list\n");
    if (contentStorage == nullptr)
      return;

    if (mShutdownRequested)
      return;

    //mContentInfoList.clear();

    T::FileId startFileId = 0;
    T::MessageIndex startMessageIndex = 0;
    uint len = 50000;
    while (len > 0)
    {
      if (mShutdownRequested)
        return;

      T::ContentInfoList contentInfoList;

      int result = contentStorage->getContentList(mSessionId,startFileId,startMessageIndex,50000,contentInfoList);

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

          contentInfo->mProducerId = getNewProducerId(contentStorageIndex,contentInfo->mProducerId);
          contentInfo->mGenerationId = getNewGenerationId(contentStorageIndex,contentInfo->mGenerationId);
          contentInfo->mFileId = getNewFileId(contentStorageIndex,contentInfo->mFileId);
          contentInfo->mSourceId = getNewSourceId(contentStorageIndex,contentInfo->mSourceId);
          contentInfo->mStorageId = contentStorageIndex;

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



void MergeImplementation::event_clear(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"*** Clear event : Deleting all cached information (%u)!\n",contentStorageIndex);

    AutoWriteLock lock(&mModificationLock);

    {
      AutoWriteLock cacheLock(&mContentTimeCache_modificationLock);
      mContentTimeCache.clear();
    }

    mEventInfoList.clear();
    mFileInfoList.deleteFileInfoByStorageId(contentStorageIndex);
    mProducerInfoList.deleteProducerInfoListByStorageId(contentStorageIndex);
    mGenerationInfoList.deleteGenerationInfoListByStorageId(contentStorageIndex);
    mGeometryInfoList.deleteGeometryInfoListByStorageId(contentStorageIndex);
    mContentInfoList.deleteContentInfoByStorageId(contentStorageIndex);

    // Switching the search structure
    mContentUpdateTime = time(0) - mContentUpdateInterval;

    addEvent(EventType::STORAGE_CLEAR,contentStorageIndex,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_contentServerReload(uint contentStorageIndex,T::EventInfo& eventInfo)
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





void MergeImplementation::event_updateLoopStart(uint contentStorageIndex,T::EventInfo& eventInfo)
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





void MergeImplementation::event_updateLoopEnd(uint contentStorageIndex,T::EventInfo& eventInfo)
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





void MergeImplementation::event_producerAdded(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    T::ProducerId newProducerId = getNewProducerId(contentStorageIndex,eventInfo.mId1);

    T::ProducerInfo *info = mProducerInfoList.getProducerInfoById(newProducerId);
    if (info != nullptr)
      return; // Already in the cache

    T::ProducerInfo producerInfo;
    if (mContentSources[contentStorageIndex].mContentStorage->getProducerInfoById(mSessionId,eventInfo.mId1,producerInfo) == Result::OK)
    {
      producerInfo.mProducerId = newProducerId;
      producerInfo.mSourceId = getNewSourceId(contentStorageIndex,producerInfo.mSourceId);
      producerInfo.mStorageId = contentStorageIndex;
      mProducerInfoList.addProducerInfo(producerInfo.duplicate());

      addEvent(eventInfo.mType,newProducerId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_producerDeleted(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    T::ProducerId newProducerId = getNewProducerId(contentStorageIndex,eventInfo.mId1);

    mContentDeleteCounter += mContentInfoList.markDeletedByProducerId(newProducerId);
    mFileDeleteCounter += mFileInfoList.markDeletedByProducerId(newProducerId);
    mGeometryDeleteCounter += mGeometryInfoList.markDeletedByProducerId(newProducerId);
    mGenerationDeleteCounter += mGenerationInfoList.markDeletedByProducerId(newProducerId);
    mProducerDeleteCounter += mProducerInfoList.markDeletedById(newProducerId);

    addEvent(eventInfo.mType,newProducerId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_producerUpdated(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    T::ProducerInfo producerInfo;
    T::ProducerId newProducerId = getNewProducerId(contentStorageIndex,eventInfo.mId1);

    if (mContentSources[contentStorageIndex].mContentStorage->getProducerInfoById(mSessionId,eventInfo.mId1,producerInfo) != Result::OK)
      return;

    {
      AutoWriteLock lock(&mModificationLock);
      T::ProducerInfo *info = mProducerInfoList.getProducerInfoById(newProducerId);
      if (info)
      {
        producerInfo.mProducerId = newProducerId;
        producerInfo.mSourceId = getNewSourceId(contentStorageIndex,producerInfo.mSourceId);
        producerInfo.mStorageId = contentStorageIndex;
        *info = producerInfo;

        addEvent(eventInfo.mType,newProducerId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
        return;
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_producerListDeletedBySourceId(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    T::SourceId newSourceId = getNewSourceId(contentStorageIndex,eventInfo.mId1);

    mContentDeleteCounter += mContentInfoList.markDeletedBySourceId(newSourceId);
    mFileDeleteCounter += mFileInfoList.markDeletedBySourceId(newSourceId);
    mGeometryDeleteCounter += mGeometryInfoList.markDeletedBySourceId(newSourceId);
    mGenerationDeleteCounter += mGenerationInfoList.markDeletedBySourceId(newSourceId);
    mProducerDeleteCounter += mProducerInfoList.markDeletedBySourceId(newSourceId);

    addEvent(eventInfo.mType,newSourceId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_generationAdded(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    T::GenerationId newGenerationId = getNewGenerationId(contentStorageIndex,eventInfo.mId1);

    T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoById(newGenerationId);
    if (info != nullptr)
      return; // Already in the cache

    T::GenerationInfo generationInfo;
    if (mContentSources[contentStorageIndex].mContentStorage->getGenerationInfoById(mSessionId,eventInfo.mId1,generationInfo) == Result::OK)
    {
      generationInfo.mGenerationId = newGenerationId;
      generationInfo.mProducerId = getNewSourceId(contentStorageIndex,generationInfo.mProducerId);
      generationInfo.mSourceId = getNewSourceId(contentStorageIndex,generationInfo.mSourceId);
      generationInfo.mStorageId = contentStorageIndex;
      mGenerationInfoList.addGenerationInfo(generationInfo.duplicate());

      addEvent(eventInfo.mType,newGenerationId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_generationDeleted(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    T::GenerationId newGenerationId = getNewGenerationId(contentStorageIndex,eventInfo.mId1);

    {
      AutoWriteLock cacheLock(&mContentTimeCache_modificationLock);
      if (mContentTimeCache.find(newGenerationId) != mContentTimeCache.end())
        mContentTimeCache.erase(newGenerationId);
    }

    mContentDeleteCounter += mContentInfoList.markDeletedByGenerationId(newGenerationId);
    mFileDeleteCounter += mFileInfoList.markDeletedByGenerationId(newGenerationId);
    mGeometryDeleteCounter += mGeometryInfoList.markDeletedByGenerationId(newGenerationId);
    mGenerationDeleteCounter += mGenerationInfoList.deleteGenerationInfoById(newGenerationId);

    addEvent(eventInfo.mType,newGenerationId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_generationUpdated(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationId newGenerationId = getNewGenerationId(contentStorageIndex,eventInfo.mId1);

    T::GenerationInfo generationInfo;
    if (mContentSources[contentStorageIndex].mContentStorage->getGenerationInfoById(mSessionId,eventInfo.mId1,generationInfo) != Result::OK)
      return;

    {
      AutoWriteLock lock(&mModificationLock);
      T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoById(newGenerationId);
      if (info)
      {
        generationInfo.mGenerationId = newGenerationId;
        generationInfo.mProducerId = getNewProducerId(contentStorageIndex,generationInfo.mProducerId);
        generationInfo.mSourceId = getNewSourceId(contentStorageIndex,generationInfo.mSourceId);
        generationInfo.mStorageId = contentStorageIndex;
        *info = generationInfo;

        addEvent(eventInfo.mType,newGenerationId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_generationStatusChanged(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    {
      AutoWriteLock lock(&mModificationLock);
      T::GenerationId newGenerationId = getNewGenerationId(contentStorageIndex,eventInfo.mId1);

      T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoById(newGenerationId);
      if (info != nullptr)
        info->mStatus = eventInfo.mId2;

      addEvent(eventInfo.mType,newGenerationId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);

    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_generationListDeletedByProducerId(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    T::ProducerId newProducerId = getNewProducerId(contentStorageIndex,eventInfo.mId1);

    mContentDeleteCounter += mContentInfoList.deleteContentInfoByProducerId(newProducerId);
    mFileDeleteCounter += mFileInfoList.deleteFileInfoByProducerId(newProducerId);
    mGeometryDeleteCounter += mGeometryInfoList.deleteGeometryInfoListByProducerId(newProducerId);
    mGenerationDeleteCounter += mGenerationInfoList.deleteGenerationInfoListByProducerId(newProducerId);

    addEvent(eventInfo.mType,newProducerId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_generationListDeletedBySourceId(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    T::SourceId newSourceId = getNewSourceId(contentStorageIndex,eventInfo.mId1);

    mContentDeleteCounter += mContentInfoList.markDeletedBySourceId(newSourceId);
    mFileDeleteCounter += mFileInfoList.markDeletedBySourceId(newSourceId);
    mGeometryDeleteCounter += mGeometryInfoList.markDeletedBySourceId(newSourceId);
    mGenerationDeleteCounter += mGenerationInfoList.markDeletedBySourceId(newSourceId);

    addEvent(eventInfo.mType,newSourceId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_geometryAdded(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    T::GeometryInfo *info = mGeometryInfoList.getGeometryInfoById(eventInfo.mId1,eventInfo.mId2,eventInfo.mId3);
    if (info != nullptr)
      return; // Already in the cache

    T::GeometryInfo geometryInfo;
    if (mContentSources[contentStorageIndex].mContentStorage->getGeometryInfoById(mSessionId,eventInfo.mId1,eventInfo.mId2,eventInfo.mId3,geometryInfo) == Result::OK)
    {
      geometryInfo.mGenerationId = getNewGenerationId(contentStorageIndex,geometryInfo.mGenerationId);
      geometryInfo.mProducerId = getNewProducerId(contentStorageIndex,geometryInfo.mProducerId);
      geometryInfo.mSourceId = getNewSourceId(contentStorageIndex,geometryInfo.mSourceId);
      geometryInfo.mStorageId = contentStorageIndex;
      mGeometryInfoList.addGeometryInfo(geometryInfo.duplicate());

      addEvent(eventInfo.mType,eventInfo.mId1,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void MergeImplementation::event_geometryDeleted(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    mContentDeleteCounter += mContentInfoList.markDeletedByGenerationAndGeometryId(eventInfo.mId1,eventInfo.mId2);
    mGeometryDeleteCounter += mGeometryInfoList.deleteGeometryInfoById(eventInfo.mId1,eventInfo.mId2,eventInfo.mId3);

    addEvent(eventInfo.mType,eventInfo.mId1,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void MergeImplementation::event_geometryStatusChanged(uint contentStorageIndex,T::EventInfo& eventInfo)
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
        addEvent(eventInfo.mType,eventInfo.mId1,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
        return;
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void MergeImplementation::event_geometryUpdated(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    T::GeometryInfo geometryInfo;
    if (mContentSources[contentStorageIndex].mContentStorage->getGeometryInfoById(mSessionId,eventInfo.mId1,eventInfo.mId2,eventInfo.mId3,geometryInfo) != Result::OK)
      return;

    {
      AutoWriteLock lock(&mModificationLock);
      T::GeometryInfo *info = mGeometryInfoList.getGeometryInfoById(eventInfo.mId1,eventInfo.mId2,eventInfo.mId3);
      if (info)
      {
        geometryInfo.mGenerationId = getNewGenerationId(contentStorageIndex,geometryInfo.mGenerationId);
        geometryInfo.mProducerId = getNewProducerId(contentStorageIndex,geometryInfo.mProducerId);
        geometryInfo.mSourceId = getNewSourceId(contentStorageIndex,geometryInfo.mSourceId);
        geometryInfo.mStorageId = contentStorageIndex;
        *info = geometryInfo;
        addEvent(eventInfo.mType,eventInfo.mId1,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
        return;
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void MergeImplementation::event_geometryListDeletedByProducerId(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    T::ProducerId newProducerId = getNewProducerId(contentStorageIndex,eventInfo.mId1);
    mGeometryDeleteCounter += mGeometryInfoList.deleteGeometryInfoListByProducerId(newProducerId);

    addEvent(eventInfo.mType,newProducerId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_geometryListDeletedByGenerationId(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    T::GenerationId newGenerationId = getNewGenerationId(contentStorageIndex,eventInfo.mId1);
    mGeometryDeleteCounter += mGeometryInfoList.deleteGeometryInfoListByGenerationId(newGenerationId);

    addEvent(eventInfo.mType,newGenerationId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void MergeImplementation::event_geometryListDeletedBySourceId(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);

    T::SourceId newSourceId = getNewSourceId(contentStorageIndex,eventInfo.mId1);
    mGeometryDeleteCounter += mGeometryInfoList.deleteGeometryInfoListBySourceId(newSourceId);

    addEvent(eventInfo.mType,newSourceId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_fileAdded(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  //FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);

    T::FileId newFileId = getNewFileId(contentStorageIndex,eventInfo.mId1);
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

            if (mFileInfoList.getFileInfoById(newFileId) == nullptr)
            {
              fileInfo->mProducerId = getNewProducerId(contentStorageIndex,fileInfo->mProducerId);
              fileInfo->mGenerationId = getNewGenerationId(contentStorageIndex,fileInfo->mGenerationId);
              fileInfo->mFileId = getNewFileId(contentStorageIndex,fileInfo->mFileId);
              fileInfo->mSourceId = getNewSourceId(contentStorageIndex,fileInfo->mSourceId);
              fileInfo->mStorageId = contentStorageIndex;
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

            T::FileId newFileId = getNewFileId(contentStorageIndex,contentInfo->mFileId);
            T::GenerationId newGenerationId = getNewGenerationId(contentStorageIndex,contentInfo->mGenerationId);
            T::SourceId newSourceId = getNewSourceId(contentStorageIndex,contentInfo->mSourceId);

            if (mContentInfoList.getContentInfoByFileIdAndMessageIndex(newFileId,contentInfo->mMessageIndex) == nullptr)
            {
              AutoWriteLock cacheLock(&mContentTimeCache_modificationLock);
              auto it = mContentTimeCache.find(newGenerationId);
              if (it != mContentTimeCache.end())
              {
                //if (it->second.find(contentInfo->mForecastTime) == it->second.end())
                  it->second.insert(contentInfo->getForecastTime());
              }

              contentInfo->mProducerId = getNewProducerId(contentStorageIndex,contentInfo->mProducerId);
              contentInfo->mGenerationId = newGenerationId;
              contentInfo->mFileId = newFileId;
              contentInfo->mSourceId = newSourceId;
              contentInfo->mStorageId = contentStorageIndex;

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

      if (mContentSources[contentStorageIndex].mContentStorage->getFileInfoById(mSessionId,eventInfo.mId1,fileInfo) == Result::OK)
      {
        T::FileInfo *fInfo = fileInfo.duplicate();
        fInfo->mProducerId = getNewProducerId(contentStorageIndex,fileInfo.mProducerId);
        fInfo->mGenerationId = getNewGenerationId(contentStorageIndex,fileInfo.mGenerationId);
        fInfo->mFileId = newFileId;
        fInfo->mSourceId = getNewSourceId(contentStorageIndex,fileInfo.mSourceId);
        fInfo->mStorageId = contentStorageIndex;
        mFileInfoList.addFileInfo(fInfo);

        if (eventInfo.mId3 > 0)
        {
          T::ContentInfoList contentInfoList;
          if (mContentSources[contentStorageIndex].mContentStorage->getContentListByFileId(mSessionId,fileInfo.mFileId,contentInfoList) == Result::OK)
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
                  cInfo->mFileId = newFileId;
                  cInfo->mProducerId = getNewProducerId(contentStorageIndex,info->mProducerId);
                  cInfo->mGenerationId = getNewGenerationId(contentStorageIndex,info->mGenerationId);
                  cInfo->mSourceId = fInfo->mSourceId;
                  cInfo->mStorageId = contentStorageIndex;

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

    addEvent(eventInfo.mType,newFileId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_fileDeleted(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    T::FileId newFileId = getNewFileId(contentStorageIndex,eventInfo.mId1);

    mContentDeleteCounter += mContentInfoList.markDeletedByFileId(newFileId);
    mFileDeleteCounter += mFileInfoList.markDeletedById(newFileId);

    addEvent(eventInfo.mType,newFileId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);

  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_fileUpdated(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock lock(&mModificationLock);
    T::FileId newFileId = getNewFileId(contentStorageIndex,eventInfo.mId1);

    T::FileInfo fileInfo;
    if (mContentSources[contentStorageIndex].mContentStorage->getFileInfoById(mSessionId,eventInfo.mId1,fileInfo) == Result::OK)
    {
      T::FileInfo *info = mFileInfoList.getFileInfoById(newFileId);
      if (info != nullptr)
      {
        mContentDeleteCounter += mContentInfoList.markDeletedByFileId(newFileId);
        *info = fileInfo;
        info->mProducerId = getNewProducerId(contentStorageIndex,fileInfo.mProducerId);
        info->mGenerationId = getNewGenerationId(contentStorageIndex,fileInfo.mGenerationId);
        info->mSourceId = getNewSourceId(contentStorageIndex,fileInfo.mSourceId);
        info->mStorageId = contentStorageIndex;
        info->mFileId = newFileId;
      }
      else
      {
        T::FileInfo *fInfo = fileInfo.duplicate();
        fInfo->mProducerId = getNewProducerId(contentStorageIndex,fileInfo.mProducerId);
        fInfo->mGenerationId = getNewGenerationId(contentStorageIndex,fileInfo.mGenerationId);
        fInfo->mSourceId = getNewSourceId(contentStorageIndex,fileInfo.mSourceId);
        fInfo->mStorageId = contentStorageIndex;
        fInfo->mFileId = newFileId;
        mFileInfoList.addFileInfo(fInfo);
      }

      T::ContentInfoList contentInfoList;
      if (mContentSources[contentStorageIndex].mContentStorage->getContentListByFileId(mSessionId,fileInfo.mFileId,contentInfoList) == Result::OK)
      {
        uint len = contentInfoList.getLength();
        for (uint c=0; c<len; c++)
        {
          T::ContentInfo *info = contentInfoList.getContentInfoByIndex(c);
          T::ContentInfo *oInfo = mContentInfoList.getContentInfoByFileIdAndMessageIndex(newFileId,info->mMessageIndex);
          if (oInfo == nullptr  ||  (oInfo != nullptr  &&  (oInfo->mFlags & T::ContentInfo::Flags::DeletedContent) != 0))
          {
            if (info != nullptr && (oInfo != nullptr  &&  (oInfo->mFlags & T::ContentInfo::Flags::DeletedContent) != 0))
            {
              // We should remove the old content before the addition
            }
            else
            {
              T::ContentInfo *cInfo = info->duplicate();
              cInfo->mProducerId = getNewProducerId(contentStorageIndex,fileInfo.mProducerId);
              cInfo->mGenerationId = getNewProducerId(contentStorageIndex,fileInfo.mGenerationId);
              cInfo->mSourceId = getNewSourceId(contentStorageIndex,fileInfo.mSourceId);
              cInfo->mStorageId = contentStorageIndex;
              cInfo->mFileId = newFileId;

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
      addEvent(eventInfo.mType,newFileId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_fileListDeletedByProducerId(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    T::ProducerId newProducerId = getNewProducerId(contentStorageIndex,eventInfo.mId1);

    mContentDeleteCounter += mContentInfoList.markDeletedByProducerId(newProducerId);
    mFileDeleteCounter += mFileInfoList.markDeletedByProducerId(newProducerId);

    addEvent(eventInfo.mType,newProducerId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_fileListDeletedByGenerationId(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    T::GenerationId newGenerationId = getNewGenerationId(contentStorageIndex,eventInfo.mId1);

    mContentDeleteCounter += mContentInfoList.markDeletedByGenerationId(newGenerationId);
    mFileDeleteCounter += mFileInfoList.markDeletedByGenerationId(newGenerationId);

    addEvent(eventInfo.mType,newGenerationId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






void MergeImplementation::event_fileListDeletedBySourceId(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    T::SourceId newSourceId = getNewSourceId(contentStorageIndex,eventInfo.mId1);

    mContentDeleteCounter += mContentInfoList.markDeletedBySourceId(newSourceId);
    mFileDeleteCounter += mFileInfoList.markDeletedBySourceId(newSourceId);

    addEvent(eventInfo.mType,newSourceId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_contentListDeletedByFileId(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    T::FileId newFileId = getNewFileId(contentStorageIndex,eventInfo.mId1);

    mContentDeleteCounter += mContentInfoList.markDeletedByFileId(newFileId);

    addEvent(eventInfo.mType,newFileId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_contentListDeletedByProducerId(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    T::ProducerId newProducerId = getNewProducerId(contentStorageIndex,eventInfo.mId1);

    mContentDeleteCounter += mContentInfoList.markDeletedByProducerId(newProducerId);

    addEvent(eventInfo.mType,newProducerId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_contentListDeletedBySourceId(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    T::SourceId newSourceId = getNewSourceId(contentStorageIndex,eventInfo.mId1);

    mContentDeleteCounter += mContentInfoList.markDeletedBySourceId(newSourceId);

    addEvent(eventInfo.mType,newSourceId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_contentListDeletedByGenerationId(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    T::GenerationId newGenerationId = getNewGenerationId(contentStorageIndex,eventInfo.mId1);

    mContentDeleteCounter += mContentInfoList.markDeletedByGenerationId(newGenerationId);

    addEvent(eventInfo.mType,newGenerationId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_contentAdded(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    T::FileId newFileId = getNewFileId(contentStorageIndex,eventInfo.mId1);

    {
      AutoReadLock lock(&mModificationLock);
      T::ContentInfo *oInfo = mContentInfoList.getContentInfoByFileIdAndMessageIndex(newFileId,eventInfo.mId2);
      if (oInfo != nullptr  &&  (oInfo->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
      {
        return;  // The content info is already in the cache.
      }
    }

    T::ContentInfo contentInfo;
    if (eventInfo.mEventData > " ")
      contentInfo.setCsv(eventInfo.mEventData);
    else
    {
      if (mContentSources[contentStorageIndex].mContentStorage->getContentInfo(mSessionId,eventInfo.mId1,eventInfo.mId2,contentInfo) != Result::OK)
        return;
    }

    if (mContentInfoList.getContentInfoByFileIdAndMessageIndex(newFileId,contentInfo.mMessageIndex) == nullptr)
    {
      T::GenerationId newGenerationId = getNewGenerationId(contentStorageIndex,contentInfo.mGenerationId);

      AutoWriteLock cacheLock(&mContentTimeCache_modificationLock);
      auto it = mContentTimeCache.find(newGenerationId);
      if (it != mContentTimeCache.end())
      {
        //if (it->second.find(contentInfo.mForecastTime) == it->second.end())
          it->second.insert(contentInfo.getForecastTime());
      }

      AutoWriteLock lock(&mModificationLock);
      T::ContentInfo *cInfo = contentInfo.duplicate();
      cInfo->mProducerId = getNewProducerId(contentStorageIndex,contentInfo.mProducerId);
      cInfo->mGenerationId = getNewGenerationId(contentStorageIndex,contentInfo.mGenerationId);
      cInfo->mSourceId = getNewSourceId(contentStorageIndex,contentInfo.mSourceId);
      cInfo->mStorageId = contentStorageIndex;
      cInfo->mFileId = newFileId;

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

    addEvent(eventInfo.mType,newFileId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_contentUpdated(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    T::FileId newFileId = getNewFileId(contentStorageIndex,eventInfo.mId1);
   {
      AutoWriteLock lock(&mModificationLock);
      mContentInfoList.deleteContentInfoByFileIdAndMessageIndex(newFileId,eventInfo.mId2);
    }

    event_contentAdded(contentStorageIndex,eventInfo);

    addEvent(eventInfo.mType,newFileId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::event_contentDeleted(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock lock(&mModificationLock);
    T::FileId newFileId = getNewFileId(contentStorageIndex,eventInfo.mId1);

    mContentDeleteCounter += mContentInfoList.markDeletedByFileIdAndMessageIndex(newFileId,eventInfo.mId2);

    addEvent(eventInfo.mType,newFileId,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::processEvent(uint contentStorageIndex,T::EventInfo& eventInfo)
{
  //FUNCTION_TRACE
  try
  {
    if (contentStorageIndex >= mContentSources.size() || mContentSources[contentStorageIndex].mContentStorage == nullptr)
      return;

    if (mShutdownRequested)
      return;

    switch (eventInfo.mType)
    {
      case EventType::UNKNOWN:
        break;

      case EventType::CLEAR:
        event_clear(contentStorageIndex,eventInfo);
        break;

      case EventType::UPDATE_LOOP_START:
        event_updateLoopStart(contentStorageIndex,eventInfo);
        break;

      case EventType::UPDATE_LOOP_END:
        event_updateLoopEnd(contentStorageIndex,eventInfo);
        break;

      case EventType::CONTENT_SERVER_RELOAD:
        event_contentServerReload(contentStorageIndex,eventInfo);
        break;

      case EventType::PRODUCER_ADDED:
        event_producerAdded(contentStorageIndex,eventInfo);
        break;

      case EventType::PRODUCER_DELETED:
        event_producerDeleted(contentStorageIndex,eventInfo);
        break;

      case EventType::PRODUCER_UPDATED:
        event_producerUpdated(contentStorageIndex,eventInfo);
        break;

      case EventType::PRODUCER_LIST_DELETED_BY_SOURCE_ID:
        event_producerListDeletedBySourceId(contentStorageIndex,eventInfo);
        break;

      case EventType::GENERATION_ADDED:
        event_generationAdded(contentStorageIndex,eventInfo);
        break;

      case EventType::GENERATION_DELETED:
        event_generationDeleted(contentStorageIndex,eventInfo);
        break;

      case EventType::GENERATION_UPDATED:
        event_generationUpdated(contentStorageIndex,eventInfo);
        break;

      case EventType::GENERATION_STATUS_CHANGED:
        event_generationStatusChanged(contentStorageIndex,eventInfo);
        break;

      case EventType::GENERATION_LIST_DELETED_BY_PRODUCER_ID:
        event_generationListDeletedByProducerId(contentStorageIndex,eventInfo);
        break;

      case EventType::GENERATION_LIST_DELETED_BY_SOURCE_ID:
        event_generationListDeletedBySourceId(contentStorageIndex,eventInfo);
        break;

      case EventType::GEOMETRY_ADDED:
        event_geometryAdded(contentStorageIndex,eventInfo);
        break;

      case EventType::GEOMETRY_DELETED:
        event_geometryDeleted(contentStorageIndex,eventInfo);
        break;

      case EventType::GEOMETRY_UPDATED:
        event_geometryUpdated(contentStorageIndex,eventInfo);
        break;

      case EventType::GEOMETRY_STATUS_CHANGED:
        event_geometryStatusChanged(contentStorageIndex,eventInfo);
        break;

      case EventType::GEOMETRY_LIST_DELETED_BY_PRODUCER_ID:
        event_geometryListDeletedByProducerId(contentStorageIndex,eventInfo);
        break;

      case EventType::GEOMETRY_LIST_DELETED_BY_GENERATION_ID:
        event_geometryListDeletedByGenerationId(contentStorageIndex,eventInfo);
        break;

      case EventType::GEOMETRY_LIST_DELETED_BY_SOURCE_ID:
        event_geometryListDeletedBySourceId(contentStorageIndex,eventInfo);
        break;

      case EventType::FILE_ADDED:
        event_fileAdded(contentStorageIndex,eventInfo);
        break;

      case EventType::FILE_DELETED:
        event_fileDeleted(contentStorageIndex,eventInfo);
        break;

      case EventType::FILE_UPDATED:
        event_fileUpdated(contentStorageIndex,eventInfo);
        break;

      case EventType::FILE_LIST_DELETED_BY_PRODUCER_ID:
        event_fileListDeletedByProducerId(contentStorageIndex,eventInfo);
        break;

      case EventType::FILE_LIST_DELETED_BY_GENERATION_ID:
        event_fileListDeletedByGenerationId(contentStorageIndex,eventInfo);
        break;

      case EventType::FILE_LIST_DELETED_BY_SOURCE_ID:
        event_fileListDeletedBySourceId(contentStorageIndex,eventInfo);
        break;

      case EventType::CONTENT_LIST_DELETED_BY_FILE_ID:
        event_contentListDeletedByFileId(contentStorageIndex,eventInfo);
        break;

      case EventType::CONTENT_LIST_DELETED_BY_PRODUCER_ID:
        event_contentListDeletedByProducerId(contentStorageIndex,eventInfo);
        break;

      case EventType::CONTENT_LIST_DELETED_BY_GENERATION_ID:
        event_contentListDeletedByGenerationId(contentStorageIndex,eventInfo);
        break;

      case EventType::CONTENT_LIST_DELETED_BY_SOURCE_ID:
        event_contentListDeletedBySourceId(contentStorageIndex,eventInfo);
        break;

      case EventType::CONTENT_ADDED:
        event_contentAdded(contentStorageIndex,eventInfo);
        break;

      case EventType::CONTENT_DELETED:
        event_contentDeleted(contentStorageIndex,eventInfo);
        break;

      case EventType::CONTENT_UPDATED:
        event_contentUpdated(contentStorageIndex,eventInfo);
        break;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MergeImplementation::processEvents(bool eventThread)
{
  // FUNCTION_TRACE
  try
  {
    if (mContentSources.size() == 0)
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

    uint slen = mContentSources.size();

    uint tlen = 1000;
    while (tlen > 0)
    {
      if (mShutdownRequested)
        return;

      tlen = 0;
      for (uint s=0; s<slen; s++)
      {
        if (mShutdownRequested)
          return;

        T::EventInfo eventInfo;
        eventInfo.mEventId = 0;
        int result = mContentSources[s].mContentStorage->getLastEventInfo(mSessionId,0,eventInfo);
        if (result == Result::DATA_NOT_FOUND || result == Result::OK)
        {
          if (eventThread  &&  mContentSources[s].mContentStorageStartTime > 0 &&  mContentSources[s].mContentStorageStartTime < eventInfo.mServerTime)
          {
            PRINT_DATA(mDebugLog, "#### Content server restart detected, reload required #######\n");
            mEventInfoList.clear();
            reloadData();
            updateContent();
            mContentSources[s].mContentStorageStartTime = eventInfo.mServerTime;
            return;
          }
        }

        PRINT_DATA(mDebugLog, "#### processEvents %u %lu %lu\n",s,eventInfo.mEventId,mContentSources[s].mLastProcessedEventId);

        if (result == Result::OK)
        {
          if (eventThread  &&  mContentSources[s].mContentStorageStartTime == 0)
            mContentSources[s].mContentStorageStartTime = eventInfo.mServerTime;

          if (eventInfo.mEventId > mContentSources[s].mLastProcessedEventId)
          {
            T::EventInfoList eventInfoList;
            result = mContentSources[s].mContentStorage->getEventInfoList(mSessionId,0,mContentSources[s].mLastProcessedEventId+1,10000,eventInfoList);
            if (result != 0)
            {
              return;
            }

            uint len = eventInfoList.getLength();
            tlen += len;

            T::EventInfo *it = eventInfoList.getFirstEvent();
            while (it != nullptr)
            {
              try
              {
                processEvent(s,*it);
              }
              catch (...)
              {
                Fmi::Exception exception(BCP,"Operation failed!",nullptr);
                exception.printError();
              }

              mContentSources[s].mLastProcessedEventId = it->mEventId;
              it = it->nextItem;
              if (mShutdownRequested)
                return;
            }
          }
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





void MergeImplementation::eventProcessingThread()
{
  FUNCTION_TRACE
  try
  {
    while (!mShutdownRequested)
    {
      try
      {
        processEvents(true);
        time_usleep(1,0);
      }
      catch (...)
      {
        Fmi::Exception exception(BCP,"Operation failed!",nullptr);
        exception.printError();
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void MergeImplementation::updateContent()
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
    // We create totally new search structure and replace the the old one.

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
                    PRINT_DATA(mDebugLog, "- FileId      : %lu\n",fInfo->mFileId);
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
    mFileInfoList.getFileInfoList(nptr->mFileInfoList);
    nptr->mFileInfoListByName.setReleaseObjects(false);
    nptr->mFileInfoListByName = nptr->mFileInfoList;
    mContentInfoList.getContentInfoList(nptr->mContentInfoList[0]);

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
        Fmi::hash_merge(h,generationHash);
        Fmi::hash_merge(h,geometryHash);
        Fmi::hash_merge(h,fileHash);
        Fmi::hash_merge(h,contentHash);

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

    mContentUpdateTime = time(nullptr);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}


}
}
