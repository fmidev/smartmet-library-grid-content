#include "CacheImplementation.h"

#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/common/AutoThreadLock.h>
#include <grid-files/common/ShowFunction.h>
#include <boost/functional/hash.hpp>


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
    Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    mSSI = 0;

    mContentStorage = nullptr;
    mContentStorageStartTime = 0;
    mContentSortingFlags = 0;
    mStartTime = 0;
    mSessionId = 0;
    mShutdownRequested = false;
    mUpdateInProgress = false;
    mRequestForwardEnabled = false;
    mLastProcessedEventId = 0;
    mDataServerCount = 0xFFFFFFFF;
    mProducerCount = 0xFFFFFFFF;
    mGenerationCount = 0xFFFFFFFF;
    mFileCount = 0xFFFFFFFF;
    mContentCount = 0xFFFFFFFF;
    mSaveEnabled = false;
    mSaveDir = "/tmp";
    mReloadActivated = false;
    mContentDeleteCount = 0;
    mFileDeleteCount = 0;

    for (uint s=0; s<2; s++)
    {
      mSearchStructure[s].mFileInfoList.setComparisonMethod(T::FileInfo::ComparisonMethod::none);

      mSearchStructure[s].mFileInfoListByName.setReleaseObjects(false);
      mSearchStructure[s].mFileInfoListByName.setComparisonMethod(T::FileInfo::ComparisonMethod::none);

      mSearchStructure[s].mContentInfoList[0].setComparisonMethod(T::ContentInfo::ComparisonMethod::file_message);
      mContentInfoListEnabled[0] = true;

      for (int t=1; t<CONTENT_LIST_COUNT; t++)
      {
        mSearchStructure[s].mContentInfoList[t].setReleaseObjects(false);
        mContentInfoListEnabled[t] = false;
      }
    }

    mFileInfoList.setModificationLockPtr(&mFileModificationLock);
    mFileInfoList.setComparisonMethod(T::FileInfo::ComparisonMethod::none);

    mContentInfoList.setComparisonMethod(T::ContentInfo::ComparisonMethod::file_message);
    mContentInfoList.setModificationLockPtr(&mContentModificationLock);
    mContentInfoList.setLockingEnabled(true);

    mDataSwapTime = 0;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





CacheImplementation::~CacheImplementation()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void CacheImplementation::init(T::SessionId sessionId,ServiceInterface *contentStorage,uint contentSortingFlags)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"* Init start\n");
    mStartTime = time(nullptr);
    mSessionId = sessionId;
    mContentStorage = contentStorage;
    mContentSortingFlags = contentSortingFlags;

    if (mContentStorage != nullptr)
    {
      AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

      T::EventInfo eventInfo;
      mUpdateInProgress = true;
      mContentStorage->getLastEventInfo(sessionId,0,eventInfo);

      mLastProcessedEventId = eventInfo.mEventId;
      mContentStorageStartTime = eventInfo.mServerTime;

      mDataServerCount = 0xFFFFFFFF;
      mProducerCount = 0xFFFFFFFF;
      mGenerationCount = 0xFFFFFFFF;
      mFileCount = 0xFFFFFFFF;
      mContentCount = 0xFFFFFFFF;

      readDataServerList();
      readProducerList();
      readGenerationList();
      readFileList();

      mFileInfoList.sort(T::FileInfo::ComparisonMethod::fileId);

      mContentInfoListEnabled[0] = true;
      mContentInfoListEnabled[1] = true;
      for (int t=1; t<CONTENT_LIST_COUNT; t++)
      {
        if ((mContentSortingFlags & (1 << t)) != 0)
          mContentInfoListEnabled[t] = true;
        else
          mContentInfoListEnabled[t] = false;
      }

      readContentList();

      mUpdateInProgress = false;
    }
    swapData();

    PRINT_DATA(mDebugLog,"* Init end\n");
  }
  catch (...)
  {
    mUpdateInProgress = false;
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    while (mFileDeleteCount > 0 || mContentDeleteCount > 0)
    {
      sleep(1);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





bool CacheImplementation::isSessionValid(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    return true;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::reloadData()
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage != nullptr)
    {
      AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

      mUpdateInProgress = true;
      mLastProcessedEventId = 0;

      mDataServerCount = 0xFFFFFFFF;
      mProducerCount = 0xFFFFFFFF;
      mGenerationCount = 0xFFFFFFFF;
      mFileCount = 0xFFFFFFFF;
      mContentCount = 0xFFFFFFFF;

      mFileInfoList.clear();
      mProducerInfoList.clear();
      mGenerationInfoList.clear();
      mDataServerInfoList.clear();
      mEventInfoList.clear();
      mContentInfoList.clear();

      T::EventInfo eventInfo;
      mContentStorage->getLastEventInfo(mSessionId,0,eventInfo);

      mLastProcessedEventId = eventInfo.mEventId;
      mContentStorageStartTime = eventInfo.mServerTime;

      readDataServerList();
      readProducerList();
      readGenerationList();
      readFileList();

      mFileInfoList.sort(T::FileInfo::ComparisonMethod::fileId);

      readContentList();

      mContentInfoList.sort(T::ContentInfo::ComparisonMethod::file_message);

      mUpdateInProgress = false;

      mStartTime = time(nullptr);

      T::EventInfo event;
      event.mType = EventType::CONTENT_SERVER_RELOAD;
      mContentStorage->addEventInfo(mSessionId,event);

      mReloadActivated = false;
    }
  }
  catch (...)
  {
    mUpdateInProgress = false;
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_addDataServerInfo(T::SessionId sessionId,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addDataServerInfo(sessionId,serverInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_deleteDataServerInfoById(T::SessionId sessionId,uint serverId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteDataServerInfoById(sessionId,serverId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getDataServerInfoById(T::SessionId sessionId,uint serverId,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getDataServerInfoById(sessionId,serverId,serverInfo);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ServerInfo *info = mSearchStructure[mSSI].mDataServerInfoList.getServerInfoById(serverId);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    serverInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getDataServerInfoByName(T::SessionId sessionId,std::string serverName,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getDataServerInfoByName(sessionId,serverName,serverInfo);;

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ServerInfo *info = mSearchStructure[mSSI].mDataServerInfoList.getServerInfoByName(serverName);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    serverInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getDataServerInfoByIor(T::SessionId sessionId,std::string serverIor,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getDataServerInfoByIor(sessionId,serverIor,serverInfo);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ServerInfo *info = mSearchStructure[mSSI].mDataServerInfoList.getServerInfoByIor(serverIor);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    serverInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getDataServerInfoList(T::SessionId sessionId,T::ServerInfoList& serverInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getDataServerInfoList(sessionId,serverInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    serverInfoList.clear();

    serverInfoList = mSearchStructure[mSSI].mDataServerInfoList;
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getDataServerInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getDataServerInfoCount(sessionId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mSearchStructure[mSSI].mDataServerInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_deleteProducerInfoByName(T::SessionId sessionId,std::string producerName)
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    T::ProducerInfo *info = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoById(producerId);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    producerInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getProducerInfoByName(T::SessionId sessionId,std::string producerName,T::ProducerInfo& producerInfo)
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

    T::ProducerInfo *info = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoByName(producerName);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    producerInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    producerInfoList.clear();

    producerInfoList = mSearchStructure[mSSI].mProducerInfoList;
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    char st[100];
    strcpy(st,parameterKey.c_str());

    if (parameterKeyType == T::ParamKeyTypeValue::FMI_NAME || parameterKeyType == T::ParamKeyTypeValue::GRIB_ID || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID  || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME)
    {
      if (strncasecmp(st,"GRIB-",5) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::GRIB_ID;
        parameterKey = st+5;
      }
      else
      if (strncasecmp(st,"NB-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_ID;
        parameterKey = st+3;
      }
      else
      if (strncasecmp(st,"NBN-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_NAME;
        parameterKey = st+4;
      }
    }

    T::ContentInfoList contentInfoList;

    std::string startTime = "19000101T00000";
    std::string endTime = "23000101T00000";

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        if (mContentInfoListEnabled[1])
          mSearchStructure[mSSI].mContentInfoList[1].getContentInfoListByFmiParameterId(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFmiParameterId(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        if (mContentInfoListEnabled[2])
          mSearchStructure[mSSI].mContentInfoList[2].getContentInfoListByFmiParameterName(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFmiParameterName(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        if (mContentInfoListEnabled[3])
          mSearchStructure[mSSI].mContentInfoList[3].getContentInfoListByGribParameterId(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByGribParameterId(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        if (mContentInfoListEnabled[4])
          mSearchStructure[mSSI].mContentInfoList[4].getContentInfoListByNewbaseParameterId(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByNewbaseParameterId(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        if (mContentInfoListEnabled[5])
          mSearchStructure[mSSI].mContentInfoList[5].getContentInfoListByNewbaseParameterName(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByNewbaseParameterName(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        break;

      case T::ParamKeyTypeValue::CDM_ID:
        if (mContentInfoListEnabled[6])
          mSearchStructure[mSSI].mContentInfoList[6].getContentInfoListByCdmParameterId(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByCdmParameterId(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        break;

      case T::ParamKeyTypeValue::CDM_NAME:
        if (mContentInfoListEnabled[7])
          mSearchStructure[mSSI].mContentInfoList[7].getContentInfoListByCdmParameterName(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByCdmParameterName(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        break;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    std::set<uint> producerIdList;
    uint len = contentInfoList.getLength();
    uint prevProducerId = 0;
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      if (prevProducerId != info->mProducerId  &&  producerIdList.find(info->mProducerId) == producerIdList.end())
      {
        prevProducerId = info->mProducerId;
        producerIdList.insert(info->mProducerId);

        T::ProducerInfo *producerInfo = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoById(info->mProducerId);
        if (producerInfo != nullptr)
          producerInfoList.addProducerInfo(producerInfo->duplicate());
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    mSearchStructure[mSSI].mProducerInfoList.getProducerInfoListBySourceId(sourceId,producerInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    count = mSearchStructure[mSSI].mProducerInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    list.clear();

    uint pLen = mSearchStructure[mSSI].mProducerInfoList.getLength();

    for (uint p=0; p<pLen; p++)
    {
      T::ProducerInfo *producerInfo = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoByIndex(p);

      std::set<uint> geometryIdList;

      uint len = mSearchStructure[mSSI].mContentInfoList[0].getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *contentInfo = mSearchStructure[mSSI].mContentInfoList[0].getContentInfoByIndex(t);
        if (producerInfo->mProducerId == contentInfo->mProducerId  &&  geometryIdList.find(contentInfo->mGeometryId) == geometryIdList.end())
        {
          char tmp[100];
          sprintf(tmp,"%s;%u",producerInfo->mName.c_str(),contentInfo->mGeometryId);
          list.insert(std::string(tmp));
          geometryIdList.insert(contentInfo->mGeometryId);
        }
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    list.clear();

    T::ProducerInfoList producerList = mSearchStructure[mSSI].mProducerInfoList;
    std::map<uint,T::ProducerInfo*> producers;
    uint plen = producerList.getLength();
    for (uint t=0; t<plen; t++)
    {
      auto prod = producerList.getProducerInfoByIndex(t);
      producers.insert(std::pair<uint,T::ProducerInfo*>(prod->mProducerId,prod));
    }


    std::set<std::size_t> tmpList;

    uint len = mSearchStructure[mSSI].mContentInfoList[0].getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = mSearchStructure[mSSI].mContentInfoList[0].getContentInfoByIndex(t);
      std::string sourceParamKey;
      std::string targetParamKey;
      T::ParamLevelIdType paramLevelIdType = T::ParamLevelIdTypeValue::FMI;
      T::ParamLevelId paramLevelId = contentInfo->mFmiParameterLevelId;

      switch (sourceParameterKeyType)
      {
        case T::ParamKeyTypeValue::FMI_ID:
          sourceParamKey = contentInfo->mFmiParameterId;
          break;

        case T::ParamKeyTypeValue::FMI_NAME:
          sourceParamKey = contentInfo->getFmiParameterName();
          break;

        case T::ParamKeyTypeValue::GRIB_ID:
          sourceParamKey = contentInfo->mGribParameterId;
          break;

        case T::ParamKeyTypeValue::NEWBASE_ID:
          sourceParamKey = contentInfo->mNewbaseParameterId;
          break;

        case T::ParamKeyTypeValue::NEWBASE_NAME:
          sourceParamKey = contentInfo->mNewbaseParameterName;
          break;

        case T::ParamKeyTypeValue::CDM_ID:
          sourceParamKey = contentInfo->mCdmParameterId;
          break;

        case T::ParamKeyTypeValue::CDM_NAME:
          sourceParamKey = contentInfo->mCdmParameterName;
          break;

        default:
          break;
      }


      switch (targetParameterKeyType)
      {
        case T::ParamKeyTypeValue::FMI_ID:
          targetParamKey = contentInfo->mFmiParameterId;
          break;

        case T::ParamKeyTypeValue::FMI_NAME:
          targetParamKey = contentInfo->getFmiParameterName();
          break;

        case T::ParamKeyTypeValue::GRIB_ID:
          targetParamKey = contentInfo->mGribParameterId;
          break;

        case T::ParamKeyTypeValue::NEWBASE_ID:
          targetParamKey = contentInfo->mNewbaseParameterId;
          break;

        case T::ParamKeyTypeValue::NEWBASE_NAME:
          targetParamKey = contentInfo->mNewbaseParameterName;
          break;

        case T::ParamKeyTypeValue::CDM_ID:
          targetParamKey = contentInfo->mCdmParameterId;
          break;

        case T::ParamKeyTypeValue::CDM_NAME:
          targetParamKey = contentInfo->mCdmParameterName;
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
        boost::hash_combine(seed,paramLevelIdType);
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
            p += sprintf(p,"%s;%s;%d;%s;%d;%d;%d;%05d;%d;%d",
                  it->second->mName.c_str(),
                  sourceParamKey.c_str(),
                  targetParameterKeyType,
                  targetParamKey.c_str(),
                  contentInfo->mGeometryId,
                  paramLevelIdType,
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





#if 0
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

    list.clear();

    T::ContentInfoList cList;
    cList.setReleaseObjects(false);
    cList = mSearchStructure[mSSI].mContentInfoList[0];
    cList.sort(T::ContentInfo::ComparisonMethod::producer_file_message);

    std::set<std::size_t> tmpList;

    uint producerId = 0;
    T::ProducerInfo *producerInfo = nullptr;

    uint len = cList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = cList.getContentInfoByIndex(t);
      std::string sourceParamKey;
      std::string targetParamKey;
      T::ParamLevelIdType paramLevelIdType = T::ParamLevelIdTypeValue::FMI;
      T::ParamLevelId paramLevelId = contentInfo->mFmiParameterLevelId;

      if (contentInfo->mProducerId != producerId)
      {
        producerInfo = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoById(contentInfo->mProducerId);
        if (producerInfo != nullptr)
          producerId = producerInfo->mProducerId;
      }

      if (producerInfo != nullptr  &&  producerInfo->mProducerId == contentInfo->mProducerId)
      {
        switch (sourceParameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            sourceParamKey = contentInfo->mFmiParameterId;
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            sourceParamKey = contentInfo->getFmiParameterName();
            break;

          case T::ParamKeyTypeValue::GRIB_ID:
            sourceParamKey = contentInfo->mGribParameterId;
            break;

          case T::ParamKeyTypeValue::NEWBASE_ID:
            sourceParamKey = contentInfo->mNewbaseParameterId;
            break;

          case T::ParamKeyTypeValue::NEWBASE_NAME:
            sourceParamKey = contentInfo->mNewbaseParameterName;
            break;

          case T::ParamKeyTypeValue::CDM_ID:
            sourceParamKey = contentInfo->mCdmParameterId;
            break;

          case T::ParamKeyTypeValue::CDM_NAME:
            sourceParamKey = contentInfo->mCdmParameterName;
            break;

          default:
            break;
        }


        switch (targetParameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            targetParamKey = contentInfo->mFmiParameterId;
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            targetParamKey = contentInfo->getFmiParameterName();
            break;

          case T::ParamKeyTypeValue::GRIB_ID:
            targetParamKey = contentInfo->mGribParameterId;
            break;

          case T::ParamKeyTypeValue::NEWBASE_ID:
            targetParamKey = contentInfo->mNewbaseParameterId;
            break;

          case T::ParamKeyTypeValue::NEWBASE_NAME:
            targetParamKey = contentInfo->mNewbaseParameterName;
            break;

          case T::ParamKeyTypeValue::CDM_ID:
            targetParamKey = contentInfo->mCdmParameterId;
            break;

          case T::ParamKeyTypeValue::CDM_NAME:
            targetParamKey = contentInfo->mCdmParameterName;
            break;

          default:
            break;
        }

        if (sourceParamKey.length() > 0  &&  targetParamKey.length() > 0)
        {
          std::size_t seed = 0;
          boost::hash_combine(seed,producerInfo->mName);
          boost::hash_combine(seed,sourceParamKey);
          boost::hash_combine(seed,targetParameterKeyType);
          boost::hash_combine(seed,targetParamKey);
          boost::hash_combine(seed,contentInfo->mGeometryId);
          boost::hash_combine(seed,paramLevelIdType);
          boost::hash_combine(seed,paramLevelId);
          boost::hash_combine(seed,contentInfo->mParameterLevel);
          boost::hash_combine(seed,contentInfo->mForecastType);
          boost::hash_combine(seed,contentInfo->mForecastNumber);


          if (tmpList.find(seed) == tmpList.end())
          {
            tmpList.insert(seed);

            char tmp[200];
            char *p = tmp;
            p += sprintf(p,"%s;%s;%d;%s;%d;%d;%d;%05d;%d;%d",
                  producerInfo->mName.c_str(),
                  sourceParamKey.c_str(),
                  targetParameterKeyType,
                  targetParamKey.c_str(),
                  contentInfo->mGeometryId,
                  paramLevelIdType,
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}
#endif





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

    list.clear();

    T::ProducerInfo *producerInfo = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    std::set<std::size_t> tmpList;

    uint len = mSearchStructure[mSSI].mContentInfoList[0].getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = mSearchStructure[mSSI].mContentInfoList[0].getContentInfoByIndex(t);
      if (contentInfo->mProducerId == producerId)
      {
        std::string sourceParamKey;
        std::string targetParamKey;
        T::ParamLevelIdType paramLevelIdType = T::ParamLevelIdTypeValue::FMI;
        T::ParamLevelId paramLevelId = contentInfo->mFmiParameterLevelId;

        switch (sourceParameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            sourceParamKey = contentInfo->mFmiParameterId;
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            sourceParamKey = contentInfo->getFmiParameterName();
            break;

          case T::ParamKeyTypeValue::GRIB_ID:
            sourceParamKey = contentInfo->mGribParameterId;
            break;

          case T::ParamKeyTypeValue::NEWBASE_ID:
            sourceParamKey = contentInfo->mNewbaseParameterId;
            break;

          case T::ParamKeyTypeValue::NEWBASE_NAME:
            sourceParamKey = contentInfo->mNewbaseParameterName;
            break;

          case T::ParamKeyTypeValue::CDM_ID:
            sourceParamKey = contentInfo->mCdmParameterId;
            break;

          case T::ParamKeyTypeValue::CDM_NAME:
            sourceParamKey = contentInfo->mCdmParameterName;
            break;

          default:
            break;
        }


        switch (targetParameterKeyType)
        {
          case T::ParamKeyTypeValue::FMI_ID:
            targetParamKey = contentInfo->mFmiParameterId;
            break;

          case T::ParamKeyTypeValue::FMI_NAME:
            targetParamKey = contentInfo->getFmiParameterName();
            break;

          case T::ParamKeyTypeValue::GRIB_ID:
            targetParamKey = contentInfo->mGribParameterId;
            break;

          case T::ParamKeyTypeValue::NEWBASE_ID:
            targetParamKey = contentInfo->mNewbaseParameterId;
            break;

          case T::ParamKeyTypeValue::NEWBASE_NAME:
            targetParamKey = contentInfo->mNewbaseParameterName;
            break;

          case T::ParamKeyTypeValue::CDM_ID:
            targetParamKey = contentInfo->mCdmParameterId;
            break;

          case T::ParamKeyTypeValue::CDM_NAME:
            targetParamKey = contentInfo->mCdmParameterName;
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
          boost::hash_combine(seed,paramLevelIdType);
          boost::hash_combine(seed,paramLevelId);
          boost::hash_combine(seed,contentInfo->mParameterLevel);
          boost::hash_combine(seed,contentInfo->mForecastType);
          boost::hash_combine(seed,contentInfo->mForecastNumber);


          if (tmpList.find(seed) == tmpList.end())
          {
            tmpList.insert(seed);

            char tmp[200];
            char *p = tmp;
            p += sprintf(p,"%s;%s;%d;%s;%d;%d;%d;%05d;%d;%d",
                  producerInfo->mName.c_str(),
                  sourceParamKey.c_str(),
                  targetParameterKeyType,
                  targetParamKey.c_str(),
                  contentInfo->mGeometryId,
                  paramLevelIdType,
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_deleteGenerationInfoByName(T::SessionId sessionId,std::string generationName)
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_deleteGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName)
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    list.clear();

    uint len = mSearchStructure[mSSI].mContentInfoList[0].getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = mSearchStructure[mSSI].mContentInfoList[0].getContentInfoByIndex(t);
      char st[200];
      sprintf(st,"%u;%u;%u;%d;%d;%s;%s;",info->mSourceId,info->mGenerationId,info->mGeometryId,info->mForecastType,info->mForecastNumber,info->mForecastTime.c_str(),info->mModificationTime.c_str());
      std::string str = st;


      if (list.find(str) == list.end())
        list.insert(str);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    generationInfoList.clear();

    std::set<uint> idList;

    mSearchStructure[mSSI].mContentInfoList[0].getGenerationIdListByGeometryId(geometryId,idList);

    for (auto it=idList.begin(); it!=idList.end(); ++it)
    {
      T::GenerationInfo *generationInfo = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoById(*it);
      if (generationInfo != nullptr)
        generationInfoList.addGenerationInfo(generationInfo->duplicate());
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    T::GenerationInfo *info = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoById(generationId);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGenerationInfoByName(T::SessionId sessionId,std::string generationName,T::GenerationInfo& generationInfo)
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

    T::GenerationInfo *info = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoByName(generationName);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    generationInfoList = mSearchStructure[mSSI].mGenerationInfoList;
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    T::ProducerInfo *producerInfo = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    generationInfoList.clear();

    mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoListByProducerId(producerId,generationInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName,T::GenerationInfoList& generationInfoList)
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

    T::ProducerInfo *producerInfo = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    generationInfoList.clear();

    mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoListByProducerId(producerInfo->mProducerId,generationInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoListBySourceId(sourceId,generationInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    T::ProducerInfo *producerInfo = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;


    T::GenerationInfo *info = mSearchStructure[mSSI].mGenerationInfoList.getLastGenerationInfoByProducerIdAndStatus(producerId,generationStatus);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,std::string producerName,uchar generationStatus,T::GenerationInfo& generationInfo)
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

    T::ProducerInfo *producerInfo = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo *info = mSearchStructure[mSSI].mGenerationInfoList.getLastGenerationInfoByProducerIdAndStatus(producerInfo->mProducerId,generationStatus);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    count = mSearchStructure[mSSI].mGenerationInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_setGenerationInfoStatusByName(T::SessionId sessionId,std::string generationName,uchar status)
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addFileInfoWithContentList(sessionId,fileInfo,contentInfoList);
    //processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addFileInfoListWithContent(sessionId,requestFlags,fileAndContentList);
    //processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_deleteFileInfoByName(T::SessionId sessionId,std::string filename)
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_deleteFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListByGroupFlags(sessionId,groupFlags);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_deleteFileInfoListByProducerName(T::SessionId sessionId,std::string producerName)
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,std::string forecastTime)
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_deleteFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName)
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    T::FileInfo *info = mSearchStructure[mSSI].mFileInfoList.getFileInfoById(fileId);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    fileInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getFileInfoByName(T::SessionId sessionId,std::string filename,T::FileInfo& fileInfo)
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

    T::FileInfo *info = mSearchStructure[mSSI].mFileInfoListByName.getFileInfoByName(filename);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    fileInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getFileInfoList(T::SessionId sessionId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
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

    mSearchStructure[mSSI].mFileInfoList.getFileInfoList(startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    for (auto it = fileIdList.begin(); it != fileIdList.end(); ++it)
    {
      T::FileInfo *info = mSearchStructure[mSSI].mFileInfoList.getFileInfoById(*it);
      if (info != nullptr)
        fileInfoList.addFileInfo(info->duplicate());
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




int CacheImplementation::_getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
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

    fileInfoList.clear();

    T::ProducerInfo *producerInfo = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    mSearchStructure[mSSI].mFileInfoList.getFileInfoListByProducerId(producerId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getFileInfoListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
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

    T::ProducerInfo *producerInfo = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    fileInfoList.clear();

    mSearchStructure[mSSI].mFileInfoList.getFileInfoListByProducerId(producerInfo->mProducerId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
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

    T::GenerationInfo *generationInfo = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    fileInfoList.clear();

    mSearchStructure[mSSI].mFileInfoList.getFileInfoListByGenerationId(generationId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
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

    T::GenerationInfo *generationInfo = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    fileInfoList.clear();

    mSearchStructure[mSSI].mFileInfoList.getFileInfoListByGenerationId(generationInfo->mGenerationId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getFileInfoListByGroupFlags(sessionId,groupFlags,startFileId,maxRecords,fileInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    fileInfoList.clear();

    mSearchStructure[mSSI].mFileInfoList.getFileInfoListByGroupFlags(groupFlags,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
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

    mSearchStructure[mSSI].mFileInfoList.getFileInfoListBySourceId(sourceId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    count = mSearchStructure[mSSI].mFileInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    count = mSearchStructure[mSSI].mFileInfoList.getFileInfoCountByProducerId(producerId);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    count = mSearchStructure[mSSI].mFileInfoList.getFileInfoCountByGenerationId(generationId);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    count = mSearchStructure[mSSI].mFileInfoList.getFileInfoCountBySourceId(sourceId);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getLastEventInfo(sessionId,requestingServerId,eventInfo);

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (requestingServerId != 0)
    {
      T::ServerInfo *info = mSearchStructure[mSSI].mDataServerInfoList.getServerInfoById(requestingServerId);
      if (info != nullptr)
        info->mLastCall = time(nullptr);
    }

    T::EventInfo *lastEvent = mEventInfoList.getLastEvent();
    if (lastEvent == nullptr)
      return Result::DATA_NOT_FOUND;

    eventInfo = *lastEvent;
    eventInfo.mServerTime = mStartTime;

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,uint maxRecords,T::EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getEventInfoList(sessionId,requestingServerId,startEventId,maxRecords,eventInfoList);

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    eventInfoList.clear();

    if (requestingServerId != 0)
    {
      T::ServerInfo *info = mSearchStructure[mSSI].mDataServerInfoList.getServerInfoById(requestingServerId);
      if (info != nullptr)
        info->mLastCall = time(nullptr);
    }

    mEventInfoList.getEventInfoList(startEventId,maxRecords,eventInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_deleteContentListByFileName(T::SessionId sessionId,std::string filename)
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_deleteContentListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentListByGroupFlags(sessionId,groupFlags);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_deleteContentListByProducerName(T::SessionId sessionId,std::string producerName)
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_deleteContentListByGenerationName(T::SessionId sessionId,std::string generationName)
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_registerContentList(T::SessionId sessionId,uint serverId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->registerContentList(sessionId,serverId,contentInfoList);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_registerContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->registerContentListByFileId(sessionId,serverId,fileId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_unregisterContentList(T::SessionId sessionId,uint serverId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->unregisterContentList(sessionId,serverId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_unregisterContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->unregisterContentListByFileId(sessionId,serverId,fileId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    T::ContentInfo *cInfo = mSearchStructure[mSSI].mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(fileId,messageIndex);
    if (cInfo != nullptr)
    {
      contentInfo = *cInfo;
      return Result::OK;
    }

    return Result::DATA_NOT_FOUND;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
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

    mSearchStructure[mSSI].mContentInfoList[0].getContentInfoList(startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFileId(fileId,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    for (auto it = fileIdList.begin(); it != fileIdList.end(); ++it)
    {
      T::ContentInfoList contentList;
      mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFileId(*it,contentList);
      contentInfoList.addContentInfoList(contentList);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getContentListByFileName(T::SessionId sessionId,std::string filename,T::ContentInfoList& contentInfoList)
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

    T::FileInfo *fileInfo = mSearchStructure[mSSI].mFileInfoListByName.getFileInfoByName(filename);
    if (fileInfo == nullptr)
      return Result::UNKNOWN_FILE_NAME;

    contentInfoList.clear();

    mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFileId(fileInfo->mFileId,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getContentListByServerId(T::SessionId sessionId,uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByServerId(sessionId,serverId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ServerInfo *info = mSearchStructure[mSSI].mDataServerInfoList.getServerInfoById(serverId);
    if (info == nullptr)
      return Result::UNKNOWN_SERVER_ID;

    contentInfoList.clear();

    mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByServerId(serverId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getContentListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByGroupFlags(sessionId,groupFlags,startFileId,startMessageIndex,maxRecords,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByGroupFlags(groupFlags,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
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

    T::ProducerInfo *producerInfo = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    contentInfoList.clear();

    mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByProducerId(producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getContentListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
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

    T::ProducerInfo *producerInfo = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    contentInfoList.clear();

    mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByProducerId(producerInfo->mProducerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




int CacheImplementation::_getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList)
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

    T::GenerationInfo *generationInfo = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    contentInfoList.clear();

    if (requestFlags < CONTENT_LIST_COUNT  &&  mContentInfoListEnabled[requestFlags])
      mSearchStructure[mSSI].mContentInfoList[requestFlags].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    else
      mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getContentListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
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

    T::GenerationInfo *generationInfo = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    contentInfoList.clear();

    mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTime,endTime,contentInfoList);

    T::GenerationInfo *generationInfo = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startTime,endTime,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,std::string generationName,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
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

    T::GenerationInfo *generationInfo = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    contentInfoList.clear();

    mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,startTime,endTime,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
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

    mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListBySourceId(sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {

    //printf("getContentListByParameter(%llu,%u,%s,%u,%u,%u,%u,%s,%s,%u)\n",sessionId,(uint)parameterKeyType,parameterKey.c_str(),(uint)parameterLevelIdType,(uint)parameterLevelId,minLevel,maxLevel,startTime.c_str(),endTime.c_str(),requestFlags);


    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameter(sessionId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    char st[100];
    strcpy(st,parameterKey.c_str());

    if (parameterKeyType == T::ParamKeyTypeValue::FMI_NAME || parameterKeyType == T::ParamKeyTypeValue::GRIB_ID || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID  || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME)
    {
      if (strncasecmp(st,"GRIB-",5) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::GRIB_ID;
        parameterKey = st+5;
      }
      else
      if (strncasecmp(st,"NB-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_ID;
        parameterKey = st+3;
      }
      else
      if (strncasecmp(st,"NBN-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_NAME;
        parameterKey = st+4;
      }
    }

    contentInfoList.clear();

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        if (mContentInfoListEnabled[1])
          mSearchStructure[mSSI].mContentInfoList[1].getContentInfoListByFmiParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFmiParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::FMI_NAME:
        if (mContentInfoListEnabled[2])
          mSearchStructure[mSSI].mContentInfoList[2].getContentInfoListByFmiParameterName(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFmiParameterName(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;


      case T::ParamKeyTypeValue::GRIB_ID:
        if (mContentInfoListEnabled[3])
          mSearchStructure[mSSI].mContentInfoList[3].getContentInfoListByGribParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByGribParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        if (mContentInfoListEnabled[4])
          mSearchStructure[mSSI].mContentInfoList[4].getContentInfoListByNewbaseParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByNewbaseParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        if (mContentInfoListEnabled[5])
          mSearchStructure[mSSI].mContentInfoList[5].getContentInfoListByNewbaseParameterName(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByNewbaseParameterName(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_ID:
        if (mContentInfoListEnabled[6])
          mSearchStructure[mSSI].mContentInfoList[6].getContentInfoListByCdmParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByCdmParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_NAME:
        if (mContentInfoListEnabled[7])
          mSearchStructure[mSSI].mContentInfoList[7].getContentInfoListByCdmParameterName(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByCdmParameterName(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::GenerationInfo *generationInfo = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    contentInfoList.clear();

    char st[100];
    strcpy(st,parameterKey.c_str());

    if (parameterKeyType == T::ParamKeyTypeValue::FMI_NAME || parameterKeyType == T::ParamKeyTypeValue::GRIB_ID || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID  || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME)
    {
      if (strncasecmp(st,"GRIB-",5) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::GRIB_ID;
        parameterKey = st+5;
      }
      else
      if (strncasecmp(st,"NB-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_ID;
        parameterKey = st+3;
      }
      else
      if (strncasecmp(st,"NBN-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_NAME;
        parameterKey = st+4;
      }
    }


    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        if (mContentInfoListEnabled[1])
          mSearchStructure[mSSI].mContentInfoList[1].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::FMI_NAME:
        if (mContentInfoListEnabled[2])
          mSearchStructure[mSSI].mContentInfoList[2].getContentInfoListByFmiParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFmiParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

        return Result::OK;

      case T::ParamKeyTypeValue::GRIB_ID:
        if (mContentInfoListEnabled[3])
          mSearchStructure[mSSI].mContentInfoList[3].getContentInfoListByGribParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByGribParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        if (mContentInfoListEnabled[4])
          mSearchStructure[mSSI].mContentInfoList[4].getContentInfoListByNewbaseParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByNewbaseParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        if (mContentInfoListEnabled[5])
          mSearchStructure[mSSI].mContentInfoList[5].getContentInfoListByNewbaseParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByNewbaseParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_ID:
        if (mContentInfoListEnabled[6])
          mSearchStructure[mSSI].mContentInfoList[6].getContentInfoListByCdmParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByCdmParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_NAME:
        if (mContentInfoListEnabled[7])
          mSearchStructure[mSSI].mContentInfoList[7].getContentInfoListByCdmParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByCdmParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getContentListByParameterAndGenerationName(T::SessionId sessionId,std::string generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::GenerationInfo *generationInfo = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    char st[100];
    strcpy(st,parameterKey.c_str());

    if (parameterKeyType == T::ParamKeyTypeValue::FMI_NAME || parameterKeyType == T::ParamKeyTypeValue::GRIB_ID || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID  || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME)
    {
      if (strncasecmp(st,"GRIB-",5) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::GRIB_ID;
        parameterKey = st+5;
      }
      else
      if (strncasecmp(st,"NB-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_ID;
        parameterKey = st+3;
      }
      else
      if (strncasecmp(st,"NBN-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_NAME;
        parameterKey = st+4;
      }
    }

    contentInfoList.clear();

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        if (mContentInfoListEnabled[1])
          mSearchStructure[mSSI].mContentInfoList[1].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::FMI_NAME:
        if (mContentInfoListEnabled[2])
          mSearchStructure[mSSI].mContentInfoList[2].getContentInfoListByFmiParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFmiParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::GRIB_ID:
        if (mContentInfoListEnabled[3])
          mSearchStructure[mSSI].mContentInfoList[3].getContentInfoListByGribParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByGribParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        if (mContentInfoListEnabled[4])
          mSearchStructure[mSSI].mContentInfoList[4].getContentInfoListByNewbaseParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByNewbaseParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        if (mContentInfoListEnabled[5])
          mSearchStructure[mSSI].mContentInfoList[5].getContentInfoListByNewbaseParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByNewbaseParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_ID:
        if (mContentInfoListEnabled[6])
          mSearchStructure[mSSI].mContentInfoList[6].getContentInfoListByCdmParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByCdmParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_NAME:
        if (mContentInfoListEnabled[7])
          mSearchStructure[mSSI].mContentInfoList[7].getContentInfoListByCdmParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByCdmParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




int CacheImplementation::_getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ProducerInfo *producerInfo = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    char st[100];
    strcpy(st,parameterKey.c_str());

    if (parameterKeyType == T::ParamKeyTypeValue::FMI_NAME || parameterKeyType == T::ParamKeyTypeValue::GRIB_ID || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID  || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME)
    {
      if (strncasecmp(st,"GRIB-",5) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::GRIB_ID;
        parameterKey = st+5;
      }
      else
      if (strncasecmp(st,"NB-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_ID;
        parameterKey = st+3;
      }
      else
      if (strncasecmp(st,"NBN-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_NAME;
        parameterKey = st+4;
      }
    }

    contentInfoList.clear();

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        if (mContentInfoListEnabled[1])
          mSearchStructure[mSSI].mContentInfoList[1].getContentInfoListByFmiParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFmiParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::FMI_NAME:
        if (mContentInfoListEnabled[2])
          mSearchStructure[mSSI].mContentInfoList[2].getContentInfoListByFmiParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFmiParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::GRIB_ID:
        if (mContentInfoListEnabled[3])
          mSearchStructure[mSSI].mContentInfoList[3].getContentInfoListByGribParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByGribParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        if (mContentInfoListEnabled[4])
          mSearchStructure[mSSI].mContentInfoList[4].getContentInfoListByNewbaseParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByNewbaseParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        if (mContentInfoListEnabled[5])
          mSearchStructure[mSSI].mContentInfoList[5].getContentInfoListByNewbaseParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByNewbaseParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_ID:
        if (mContentInfoListEnabled[6])
          mSearchStructure[mSSI].mContentInfoList[6].getContentInfoListByCdmParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByCdmParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_NAME:
        if (mContentInfoListEnabled[7])
          mSearchStructure[mSSI].mContentInfoList[7].getContentInfoListByCdmParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByCdmParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




int CacheImplementation::_getContentListByParameterAndProducerName(T::SessionId sessionId,std::string producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ProducerInfo *producerInfo = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    char st[100];
    strcpy(st,parameterKey.c_str());

    if (parameterKeyType == T::ParamKeyTypeValue::FMI_NAME || parameterKeyType == T::ParamKeyTypeValue::GRIB_ID || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID  || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME)
    {
      if (strncasecmp(st,"GRIB-",5) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::GRIB_ID;
        parameterKey = st+5;
      }
      else
      if (strncasecmp(st,"NB-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_ID;
        parameterKey = st+3;
      }
      else
      if (strncasecmp(st,"NBN-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_NAME;
        parameterKey = st+4;
      }
    }

    contentInfoList.clear();

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        if (mContentInfoListEnabled[1])
          mSearchStructure[mSSI].mContentInfoList[1].getContentInfoListByFmiParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFmiParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::FMI_NAME:
        if (mContentInfoListEnabled[2])
          mSearchStructure[mSSI].mContentInfoList[2].getContentInfoListByFmiParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFmiParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::GRIB_ID:
        if (mContentInfoListEnabled[3])
          mSearchStructure[mSSI].mContentInfoList[3].getContentInfoListByGribParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByGribParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        if (mContentInfoListEnabled[4])
          mSearchStructure[mSSI].mContentInfoList[4].getContentInfoListByNewbaseParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByNewbaseParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        if (mContentInfoListEnabled[5])
          mSearchStructure[mSSI].mContentInfoList[5].getContentInfoListByNewbaseParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByNewbaseParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_ID:
        if (mContentInfoListEnabled[6])
          mSearchStructure[mSSI].mContentInfoList[6].getContentInfoListByCdmParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByCdmParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_NAME:
        if (mContentInfoListEnabled[7])
          mSearchStructure[mSSI].mContentInfoList[7].getContentInfoListByCdmParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByCdmParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string forecastTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameterGenerationIdAndForecastTime(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::GenerationInfo *generationInfo = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    char st[100];
    strcpy(st,parameterKey.c_str());

    if (parameterKeyType == T::ParamKeyTypeValue::FMI_NAME || parameterKeyType == T::ParamKeyTypeValue::GRIB_ID || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_ID  || parameterKeyType == T::ParamKeyTypeValue::NEWBASE_NAME)
    {
      if (strncasecmp(st,"GRIB-",5) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::GRIB_ID;
        parameterKey = st+5;
      }
      else
      if (strncasecmp(st,"NB-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_ID;
        parameterKey = st+3;
      }
      else
      if (strncasecmp(st,"NBN-",3) == 0)
      {
        parameterKeyType = T::ParamKeyTypeValue::NEWBASE_NAME;
        parameterKey = st+4;
      }
    }

    contentInfoList.clear();

/*
    boost::posix_time::ptime s = toTimeStamp(forecastTime) - boost::posix_time::minutes(600);;
    boost::posix_time::ptime e = s + boost::posix_time::minutes(600);

    std::string startTime = toString(s);
    std::string endTime = toString(e);
*/

    std::string startTime = "13000101T000000";
    std::string endTime = "23000101T000000";

    T::ParamLevel minLevel = level;
    T::ParamLevel maxLevel = level;
    uint requestFlags = 0;
    T::ContentInfoList contentList;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        if (mContentInfoListEnabled[1])
          mSearchStructure[mSSI].mContentInfoList[1].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        if (mContentInfoListEnabled[2])
        {
          T::ContentInfo *cInfo = mSearchStructure[mSSI].mContentInfoList[2].getContentInfoByFmiParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime);
          if (cInfo != nullptr)
          {
            contentInfoList.addContentInfo(cInfo->duplicate());
            return Result::OK;
          }
          //mSearchStructure[mSSI].mContentInfoList[2].getContentInfoListByFmiParameterNameAndGenerationId(generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
          mSearchStructure[mSSI].mContentInfoList[2].getContentInfoListByFmiParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,contentList);
          //contentList.print(std::cout,0,0);
        }
        else
        {
          //printf("Not sorted\n");
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByFmiParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        }
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        if (mContentInfoListEnabled[3])
          mSearchStructure[mSSI].mContentInfoList[3].getContentInfoListByGribParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByGribParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        if (mContentInfoListEnabled[4])
          mSearchStructure[mSSI].mContentInfoList[4].getContentInfoListByNewbaseParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByNewbaseParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        if (mContentInfoListEnabled[5])
          mSearchStructure[mSSI].mContentInfoList[5].getContentInfoListByNewbaseParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByNewbaseParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        break;

      case T::ParamKeyTypeValue::CDM_ID:
        if (mContentInfoListEnabled[6])
          mSearchStructure[mSSI].mContentInfoList[6].getContentInfoListByCdmParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByCdmParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        break;

      case T::ParamKeyTypeValue::CDM_NAME:
        if (mContentInfoListEnabled[7])
          mSearchStructure[mSSI].mContentInfoList[7].getContentInfoListByCdmParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        else
          mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByCdmParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        break;

      case T::ParamKeyTypeValue::BUILD_IN:
        break;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    //contentList.print(std::cout,0,0);

    contentInfoList = contentList;
#if 0
    contentList.getContentListByForecastTime(forecastTime,contentInfoList);

    // If we cannot find any forecast time, lets add at least one
    // time in order to show that there are other times available.

    if (contentInfoList.getLength() == 0  &&  contentList.getLength() > 0)
    {
      T::ContentInfo *info = contentList.getContentInfoByIndex(0);
      if (info != nullptr)
      {
        if (contentInfoList.getReleaseObjects())
          contentInfoList.addContentInfo(info->duplicate());
        else
          contentInfoList.addContentInfo(info);
      }
    }
#endif
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getContentListByRequestCounterKey(T::SessionId sessionId,ulonglong key,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress &&  !mRequestForwardEnabled)
      return Result::OK;

    if (mUpdateInProgress)
      return mContentStorage->getContentListByRequestCounterKey(sessionId,key,contentInfoList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    contentInfoList.clear();

    mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByRequestCounterKey(key,contentInfoList);;

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    uint cLen = mSearchStructure[mSSI].mContentInfoList[0].getLength();
    for (uint c=0; c<cLen; c++)
    {
      T::ContentInfo *cInfo = mSearchStructure[mSSI].mContentInfoList[0].getContentInfoByIndex(c);
      T::ContentInfo *cError = nullptr;
      if (cInfo != nullptr)
      {
        T::FileInfo *fileInfo = mSearchStructure[mSSI].mFileInfoList.getFileInfoById(cInfo->mFileId);
        if (fileInfo == nullptr)
        {
          PRINT_DATA(mDebugLog,"**** INTEGRITY ERROR : File missing (%u)! *****\n",cInfo->mFileId);
          cError = cInfo;
        }

        if (cError == nullptr)
        {
          T::GenerationInfo *generationInfo = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoById(cInfo->mGenerationId);
          if (generationInfo == nullptr)
          {
            PRINT_DATA(mDebugLog,"**** INTEGRITY ERROR : Generation missing (%u)! *****\n",cInfo->mGenerationId);
            cError = cInfo;
          }
        }

        if (cError == nullptr)
        {
          T::ProducerInfo *producerInfo = mSearchStructure[mSSI].mProducerInfoList.getProducerInfoById(cInfo->mProducerId);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    T::GenerationInfo *generationInfo = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    geometryIdList.clear();

    mSearchStructure[mSSI].mContentInfoList[0].getContentGeometryIdListByGenerationId(generationInfo->mProducerId,generationId,geometryIdList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    T::GenerationInfo *generationInfo = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    contentParamList.clear();

    T::ContentInfoList contentInfoList;
    mSearchStructure[mSSI].mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,0,0,1000000,contentInfoList);
    contentInfoList.sort(T::ContentInfo::ComparisonMethod::fmiName_fmiLevelId_level_starttime_file_message);
    uint len = contentInfoList.getLength();
    T::ContentInfo *prev = nullptr;
    T::ContentInfo *currentInfo = nullptr;
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);

      if (prev == nullptr ||
          info->getFmiParameterName() != prev->getFmiParameterName() ||
          info->mFmiParameterLevelId != prev->mFmiParameterLevelId ||
          info->mParameterLevel != prev->mParameterLevel ||
          info->mForecastType != prev->mForecastType ||
          info->mForecastNumber != prev->mForecastNumber)
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    T::GenerationInfo *generationInfo = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    paramKeyList.clear();

    mSearchStructure[mSSI].mContentInfoList[0].getContentParamKeyListByGenerationId(generationInfo->mProducerId,generationId,parameterKeyType,paramKeyList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    T::GenerationInfo *generationInfo = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    contentTimeList.clear();

    auto it = mContentTimeCache.find(generationId);
    if (it == mContentTimeCache.end())
    {
      mSearchStructure[mSSI].mContentInfoList[0].getForecastTimeListByGenerationId(generationInfo->mProducerId,generationId,contentTimeList);
      mContentTimeCache.insert(std::pair<uint,std::set<std::string>>(generationId,contentTimeList));
    }
    else
    {
      contentTimeList = it->second;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    T::GenerationInfo *generationInfo = mSearchStructure[mSSI].mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    contentTimeList.clear();

    mSearchStructure[mSSI].mContentInfoList[0].getForecastTimeListByGenerationAndGeometry(generationInfo->mProducerId,generationId,geometryId,contentTimeList);

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    contentTimeList.clear();

    mSearchStructure[mSSI].mContentInfoList[0].getForecastTimeListByProducerId(producerId,contentTimeList);

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    count = mSearchStructure[mSSI].mContentInfoList[0].getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    std::size_t generationHash = mSearchStructure[mSSI].mGenerationInfoList.getHashByProducerId(producerId);
    std::size_t fileHash = mSearchStructure[mSSI].mFileInfoList.getHashByProducerId(producerId);
    std::size_t contentHash = mSearchStructure[mSSI].mContentInfoList[0].getHashByProducerId(producerId);

    std::size_t h = 0;
    boost::hash_combine(h,generationHash);
    boost::hash_combine(h,fileHash);
    boost::hash_combine(h,contentHash);

    hash = h;

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    mSearchStructure[mSSI].mContentInfoList[0].getLevelInfoList(levelInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_deleteVirtualContent(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteVirtualContent(sessionId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_updateVirtualContent(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == nullptr)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->updateVirtualContent(sessionId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    mProducerInfoList.clear();

    int result = mContentStorage->getProducerInfoList(mSessionId,mProducerInfoList);
    if (result != 0)
    {
      Spine::Exception exception(BCP,"Cannot read the producer list from the content storage!");
      exception.addParameter("ServiceResult",getResultString(result));
      throw exception;
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    mGenerationInfoList.clear();

    int result = mContentStorage->getGenerationInfoList(mSessionId,mGenerationInfoList);
    if (result != 0)
    {
      Spine::Exception exception(BCP,"Cannot read the generation list from the content storage!");
      exception.addParameter("ServiceResult",getResultString(result));
      throw exception;
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    mFileInfoList.clear();
    mFileInfoList.setComparisonMethod(T::FileInfo::ComparisonMethod::none);


    uint startFileId = 0;
    uint len = 50000;
    while (len > 0)
    {
      T::FileInfoList fileInfoList;

      int result = mContentStorage->getFileInfoList(mSessionId,startFileId,50000,fileInfoList);
      if (result != 0)
      {
        Spine::Exception exception(BCP,"Cannot read the file list from the content storage!");
        exception.addParameter("ServiceResult",getResultString(result));
        throw exception;
      }

      len = fileInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *fileInfo = fileInfoList.getFileInfoByIndex(t);
        if (fileInfo->mFileId >= startFileId)
          startFileId = fileInfo->mFileId + 1;

        mFileInfoList.addFileInfo(fileInfo->duplicate());
      }
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

    mContentInfoList.clear();

    uint startFileId = 0;
    uint startMessageIndex = 0;
    uint len = 50000;
    while (len > 0)
    {
      T::ContentInfoList contentInfoList;

      int result = mContentStorage->getContentList(mSessionId,startFileId,startMessageIndex,50000,contentInfoList);
      if (result != 0)
      {
        Spine::Exception exception(BCP,"Cannot read the content list from the content storage!");
        exception.addParameter("ServiceResult",getResultString(result));
        throw exception;
      }

      len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
        startFileId = contentInfo->mFileId;
        startMessageIndex = contentInfo->mMessageIndex + 1;

        T::ContentInfo *newContentInfo = contentInfo->duplicate();
        if (mContentInfoList.addContentInfo(newContentInfo) != newContentInfo)
          delete newContentInfo;
      }
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}



void CacheImplementation::readDataServerList()
{
  FUNCTION_TRACE
  try
  {
   PRINT_DATA(mDebugLog,"* Reading the data server list\n");
   if (mContentStorage == nullptr)
      return;

    mDataServerInfoList.clear();

    int result = mContentStorage->getDataServerInfoList(mSessionId,mDataServerInfoList);
    if (result != 0)
    {
      Spine::Exception exception(BCP,"Cannot read the data server list from the content storage!");
      exception.addParameter("ServiceResult",getResultString(result));
      throw exception;
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_clear(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: clear\n",eventInfo.mEventId);
    PRINT_DATA(mDebugLog,"*** Clear event : Deleting all cached information!\n");

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentTimeCache.clear();

    mFileInfoList.clear();
    mProducerInfoList.clear();
    mGenerationInfoList.clear();
    mDataServerInfoList.clear();
    mEventInfoList.clear();
    mContentInfoList.clear();
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_contentServerReload(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: reload\n",eventInfo.mEventId);
  }
  catch (...)
  {
    mUpdateInProgress = false;
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_producerAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: producerAdded(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ProducerInfo producerInfo;
    if (mContentStorage->getProducerInfoById(mSessionId,eventInfo.mId1,producerInfo) == Result::OK)
    {
      //producerInfo.print(std::cout,0,0);
      mProducerInfoList.addProducerInfo(producerInfo.duplicate());
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_producerDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: producerDeleted(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList.deleteContentInfoByProducerId(eventInfo.mId1);
    mFileInfoList.deleteFileInfoByProducerId(eventInfo.mId1);
    mProducerInfoList.deleteProducerInfoById(eventInfo.mId1);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_producerListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: producerListDeletedBySourceId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList.deleteContentInfoBySourceId(eventInfo.mId1);
    mFileInfoList.deleteFileInfoBySourceId(eventInfo.mId1);
    mGenerationInfoList.deleteGenerationInfoListBySourceId(eventInfo.mId1);
    mProducerInfoList.deleteProducerInfoListBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_generationAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: generationAdded(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo generationInfo;
    if (mContentStorage->getGenerationInfoById(mSessionId,eventInfo.mId1,generationInfo) == Result::OK)
    {
      mGenerationInfoList.addGenerationInfo(generationInfo.duplicate());
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_generationDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: generationDeleted(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    auto it = mContentTimeCache.find(eventInfo.mId1);
    if (it != mContentTimeCache.end())
      mContentTimeCache.erase(eventInfo.mId1);

    mContentInfoList.deleteContentInfoByGenerationId(eventInfo.mId1);
    mFileInfoList.deleteFileInfoByGenerationId(eventInfo.mId1);
    mGenerationInfoList.deleteGenerationInfoById(eventInfo.mId1);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_generationStatusChanged(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: generationStatusChanged(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoById(eventInfo.mId1);
    if (info != nullptr)
      info->mStatus = eventInfo.mId2;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_generationListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: generationListDeletedByProducerId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList.deleteContentInfoByProducerId(eventInfo.mId1);
    mFileInfoList.deleteFileInfoByProducerId(eventInfo.mId1);
    mGenerationInfoList.deleteGenerationInfoListByProducerId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_generationListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: generationListDeletedBySourceId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList.deleteContentInfoBySourceId(eventInfo.mId1);
    mFileInfoList.deleteFileInfoBySourceId(eventInfo.mId1);
    mGenerationInfoList.deleteGenerationInfoListBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_fileAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: fileAdded(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    uint len = eventInfo.mNote.length();

    if (len > 0)
    {
      char *buf = new char[len+10];

      strcpy(buf,eventInfo.mNote.c_str());
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
            //printf("%s\n",s);
            if (mContentInfoList.getContentInfoByFileIdAndMessageIndex(contentInfo->mFileId,contentInfo->mMessageIndex) == nullptr)
            {
              auto it = mContentTimeCache.find(contentInfo->mGenerationId);
              if (it != mContentTimeCache.end())
              {
                if (it->second.find(contentInfo->mForecastTime) == it->second.end())
                  it->second.insert(contentInfo->mForecastTime);
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
          if ((fileInfo.mFlags & T::FileInfo::Flags::PredefinedContent) != 0)
          {
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
                  if (oInfo != nullptr  &&  (oInfo->mFlags & T::ContentInfo::Flags::DeletedContent) != 0)
                  {
                    // We should remove the old content before the addition
                  }
                  else
                  {
                    T::ContentInfo *cInfo = info->duplicate();

                    auto it = mContentTimeCache.find(cInfo->mGenerationId);
                    if (it != mContentTimeCache.end())
                    {
                      if (it->second.find(cInfo->mForecastTime) == it->second.end())
                        it->second.insert(cInfo->mForecastTime);
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
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_fileDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: fileDeleted(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    int cnt = mContentInfoList.markDeletedByFileId(eventInfo.mId1);
    if (cnt > 0)
      mContentDeleteCount += cnt;

    // printf("--- content delete found  %u\n",cnt);

    T::FileInfo *fileInfo = mFileInfoList.getFileInfoById(eventInfo.mId1);
    if (fileInfo != nullptr)
    {
      mFileInfoList.markFileInfoDeletedById(eventInfo.mId1);
      mFileDeleteCount++;
    }
    else
    {
      // printf("--- file not found\n");
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_fileUpdated(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: fileUpdated(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::FileInfo fileInfo;
    if (mContentStorage->getFileInfoById(mSessionId,eventInfo.mId1,fileInfo) == Result::OK)
    {
      T::FileInfo *info = mFileInfoList.getFileInfoById(eventInfo.mId1);
      if (info != nullptr)
      {
        mContentDeleteCount += mContentInfoList.markDeletedByFileId(info->mFileId);

/*
        mContentInfoList.deleteContentInfoByFileId(eventInfo.mId1);
*/
        *info = fileInfo;
      }
      else
      {
        T::FileInfo *fInfo = fileInfo.duplicate();
        mFileInfoList.addFileInfo(fInfo);
      }

      if ((fileInfo.mFlags & T::FileInfo::Flags::PredefinedContent) != 0)
      {
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
              if (oInfo != nullptr  &&  (oInfo->mFlags & T::ContentInfo::Flags::DeletedContent) != 0)
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
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_fileListDeletedByGroupFlags(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: fileListDeletedByGroupFlags(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList.deleteContentInfoByGroupFlags(eventInfo.mId1);
    mFileInfoList.deleteFileInfoByGroupFlags(eventInfo.mId1);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_fileListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: fileListDeletedByProducerId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList.deleteContentInfoByProducerId(eventInfo.mId1);
    mFileInfoList.deleteFileInfoByProducerId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_fileListDeletedByGenerationId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: fileListDeletedByGenerationId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList.deleteContentInfoByGenerationId(eventInfo.mId1);
    mFileInfoList.deleteFileInfoByGenerationId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}






void CacheImplementation::event_fileListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: fileListDeletedBySourceId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList.deleteContentInfoBySourceId(eventInfo.mId1);
    mFileInfoList.deleteFileInfoBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_contentListDeletedByFileId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: contentListDeletedByFileId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    mContentInfoList.deleteContentInfoByFileId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_contentListDeletedByGroupFlags(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: contentListDeletedByGroupFlags(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    mContentInfoList.deleteContentInfoByGroupFlags(eventInfo.mId1);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_contentListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: contentListDeletedByProducerId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    mContentInfoList.deleteContentInfoByProducerId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_contentListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: contentListDeletedBySourceId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    mContentInfoList.deleteContentInfoBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_contentListDeletedByGenerationId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: contentListDeletedByGenerationId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
    mContentInfoList.deleteContentInfoByGenerationId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_dataServerAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: dataServerAdded(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ServerInfo serverInfo;
    if (mContentStorage->getDataServerInfoById(mSessionId,eventInfo.mId1,serverInfo) == Result::OK)
    {
      T::ServerInfo *info = serverInfo.duplicate();
      mDataServerInfoList.addServerInfo(info);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_dataServerDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: dataServerDeleted(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList.unregisterContentInfoByServerId(eventInfo.mId1);
    mContentCount = 0xFFFFFFFF;

    mDataServerInfoList.deleteServerInfoById(eventInfo.mId1);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_contentAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: contentAdded(%u,%u)\n",eventInfo.mEventId,eventInfo.mId1,eventInfo.mId2);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ContentInfo *oInfo = mContentInfoList.getContentInfoByFileIdAndMessageIndex(eventInfo.mId1,eventInfo.mId2);
    if (oInfo != nullptr  &&  (oInfo->mFlags & T::ContentInfo::Flags::DeletedContent) == 0)
    {
      // printf("ALREADY IN CACHE %u:%u\n",eventInfo.mId1,eventInfo.mId2);
      return;  // The content info is already in the cache.
    }

    // eventInfo.print(std::cout,0,0);

    T::ContentInfo contentInfo;
    if (eventInfo.mNote > " ")
      contentInfo.setCsv(eventInfo.mNote);
    else
    {
      if (mContentStorage->getContentInfo(mSessionId,eventInfo.mId1,eventInfo.mId2,contentInfo) != Result::OK)
        return;
    }

    //contentInfo.print(std::cout,0,0);

    if (mContentInfoList.getContentInfoByFileIdAndMessageIndex(contentInfo.mFileId,contentInfo.mMessageIndex) == nullptr)
    {
      auto it = mContentTimeCache.find(contentInfo.mGenerationId);
      if (it != mContentTimeCache.end())
      {
        if (it->second.find(contentInfo.mForecastTime) == it->second.end())
          it->second.insert(contentInfo.mForecastTime);
      }

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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_contentDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: contentDeleted(%u,%u)\n",eventInfo.mEventId,eventInfo.mId1,eventInfo.mId2);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ContentInfo *contentInfo = mContentInfoList.getContentInfoByFileIdAndMessageIndex(eventInfo.mId1,eventInfo.mId2);
    if (contentInfo != nullptr)
    {
       mContentInfoList.deleteContentInfoByFileIdAndMessageIndex(eventInfo.mId1,eventInfo.mId2);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_contentRegistered(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: contentRegistered(%u,%u,%u)\n",eventInfo.mEventId,eventInfo.mId1,eventInfo.mId2,eventInfo.mId3);

    if (eventInfo.mId3 < 1 ||  eventInfo.mId3 > 64)
    {
      PRINT_DATA(mDebugLog,"%s:%d:%s: Invalid server id (%u)!\n",__FILE__,__LINE__,__FUNCTION__,eventInfo.mId3);
      return;
    }

    unsigned long long sf = 1 << (eventInfo.mId3-1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ContentInfo *info = mContentInfoList.getContentInfoByFileIdAndMessageIndex(eventInfo.mId1,eventInfo.mId2);
    if (info != nullptr)
    {
      info->mServerFlags = info->mServerFlags | sf;
      mContentCount = 0xFFFFFFFF;
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_deleteVirtualContent(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"Delete virtual content event received\n");
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList.deleteVirtualContent();
    mFileInfoList.deleteVirtualFiles();
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::event_updateVirtualContent(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"Update virtual content event received\n");
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList.deleteVirtualContent();
    mFileInfoList.deleteVirtualFiles();
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void CacheImplementation::processEvent(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    switch (eventInfo.mType)
    {
      case EventType::UNKNOWN:
        break;

      case EventType::CLEAR:
        event_clear(eventInfo);
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

      case EventType::PRODUCER_LIST_DELETED_BY_SOURCE_ID:
        event_producerListDeletedBySourceId(eventInfo);
        break;

      case EventType::GENERATION_ADDED:
        event_generationAdded(eventInfo);
        break;

      case EventType::GENERATION_DELETED:
        event_generationDeleted(eventInfo);
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

      case EventType::FILE_ADDED:
        event_fileAdded(eventInfo);
        break;

      case EventType::FILE_DELETED:
        event_fileDeleted(eventInfo);
        break;

      case EventType::FILE_UPDATED:
        event_fileUpdated(eventInfo);
        break;

      case EventType::FILE_LIST_DELETED_BY_GROUP_FLAGS:
        event_fileListDeletedByGroupFlags(eventInfo);
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

      case EventType::CONTENT_LIST_DELETED_BY_GROUP_FLAGS:
        event_contentListDeletedByGroupFlags(eventInfo);
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

      case EventType::DATA_SERVER_ADDED:
        event_dataServerAdded(eventInfo);
        break;

      case EventType::DATA_SERVER_DELETED:
        event_dataServerDeleted(eventInfo);
        break;

      case EventType::CONTENT_ADDED:
        event_contentAdded(eventInfo);
        break;

      case EventType::CONTENT_DELETED:
        event_contentDeleted(eventInfo);
        break;

      case EventType::CONTENT_REGISTERED:
        event_contentRegistered(eventInfo);
        break;

      case EventType::DELETE_VIRTUAL_CONTENT:
        event_deleteVirtualContent(eventInfo);
        break;

      case EventType::UPDATE_VIRTUAL_CONTENT:
        event_updateVirtualContent(eventInfo);
        break;
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::processEvents(bool eventThread)
{
  FUNCTION_TRACE
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

    if (mFileDeleteCount == 0  &&  mContentDeleteCount == 0)
      swapData();

    AutoThreadLock eventLock(&mEventProcessingLock);

    uint len = 1000;
    while (len > 0)
    {
      len = 0;
      T::EventInfo eventInfo;
      int result = mContentStorage->getLastEventInfo(mSessionId,0,eventInfo);
      if (result != Result::OK)
        return;

      if (eventThread  &&  mContentStorageStartTime == 0)
        mContentStorageStartTime = eventInfo.mServerTime;

      if (eventThread  &&  mContentStorageStartTime < eventInfo.mServerTime)
      {
        mReloadActivated = true;
        reloadData();
        mContentStorageStartTime = eventInfo.mServerTime;
        mReloadActivated = false;
        return;
      }

      if (eventInfo.mEventId > mLastProcessedEventId)
      {

        T::EventInfoList eventInfoList;
        result = mContentStorage->getEventInfoList(mSessionId,0,mLastProcessedEventId+1,10000,eventInfoList);
        if (result != 0)
        {
          //printf("ERROR: getEventInfoList : %d\n",result);
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
            Spine::Exception exception(BCP,exception_operation_failed,nullptr);
            exception.printError();
          }


          T::EventInfo *event = it->duplicate();
          event->mServerTime = mStartTime;
          mEventInfoList.addEventInfo(event);
          mLastProcessedEventId = it->mEventId;

          it = it->nextItem;

          if (mShutdownRequested)
            return;
        }
      }
    }


    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    if (mFileDeleteCount > 0)
    {
      PRINT_DATA(mDebugLog,"* Deleting files that were marked to be deleted : %u\n",mFileDeleteCount);

      mFileInfoList.deleteMarkedFiles();
      mFileDeleteCount = 0;
    }

    if (mContentDeleteCount > 0)
    {
      PRINT_DATA(mDebugLog,"* Deleting content that was marked to be deleted : %u\n",mContentDeleteCount);
      mContentInfoList.deleteMarkedContent();
      mContentDeleteCount = 0;
    }

    for (auto it = mContentTimeCache.begin();  it != mContentTimeCache.end(); ++it)
    {
      if (mGenerationInfoList.getGenerationInfoById(it->first) == nullptr)
      {
        mContentTimeCache.erase(it->first);
      }
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
        Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void CacheImplementation::saveData()
{
  //FUNCTION_TRACE
  try
  {
    if (mSaveEnabled)
    {
      AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

      if (mDataServerCount != mDataServerInfoList.getLength())
        mDataServerInfoList.writeToFile(mSaveDir + "/dataServers.csv");

      if (mProducerCount != mProducerInfoList.getLength())
        mProducerInfoList.writeToFile(mSaveDir + "/producer.csv");

      if (mGenerationCount != mGenerationInfoList.getLength())
      {
        std::string filename = mSaveDir + "/generations.csv";
        mGenerationInfoList.writeToFile(filename);
      }

      if (mFileCount != mFileInfoList.getLength())
        mFileInfoList.writeToFile(mSaveDir + "/files.csv");

      if (mContentCount != mContentInfoList.getLength())
        mContentInfoList.writeToFile(mSaveDir + "/content.csv");

      mDataServerCount = mDataServerInfoList.getLength();
      mProducerCount = mProducerInfoList.getLength();
      mGenerationCount = mGenerationInfoList.getLength();
      mFileCount = mFileInfoList.getLength();
      mContentCount = mContentInfoList.getLength();
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void CacheImplementation::swapData()
{
  //FUNCTION_TRACE
  try
  {
    int diff = time(nullptr) - mDataSwapTime;

    if (diff < 60)
    {
      SearchStructure *ss = &mSearchStructure[1];
      if (mSSI == 1)
        ss = &mSearchStructure[0];

      if (diff > 30  &&  ss->mProducerInfoList.getLength() > 0)
      {
        // Releasing memory

        ss->mDataServerInfoList.clear();
        ss->mProducerInfoList.clear();
        ss->mGenerationInfoList.clear();
        ss->mFileInfoListByName.clear();
        ss->mFileInfoList.clear();

        for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
          ss->mContentInfoList[t].clear();
      }

      return;
    }


    printf("###### SWAP DATA ######\n");

    SearchStructure *ss = &mSearchStructure[1];
    SearchStructure *st = &mSearchStructure[0];
    if (mSSI == 1)
    {
      ss = &mSearchStructure[0];
      st = &mSearchStructure[1];
    }

    if (st->mDataServerInfoList.getHash() == mDataServerInfoList.getHash() &&
        st->mProducerInfoList.getHash() == mProducerInfoList.getHash() &&
        st->mGenerationInfoList.getHash() == mGenerationInfoList.getHash() &&
        st->mFileInfoList.getHash() == mFileInfoList.getHash() &&
        st->mContentInfoList[0].getHash() == mContentInfoList.getHash())
    {
      // Nothing has changed. No swapping needed.

      printf("---- swap not needed\n");
      mDataSwapTime = time(nullptr);
      return;
    }

    ss->mDataServerInfoList.clear();
    ss->mProducerInfoList.clear();
    ss->mGenerationInfoList.clear();
    ss->mFileInfoListByName.clear();
    ss->mFileInfoList.clear();

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      ss->mContentInfoList[t].clear();

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    ss->mDataServerInfoList = mDataServerInfoList;
    ss->mProducerInfoList = mProducerInfoList;
    ss->mGenerationInfoList = mGenerationInfoList;
    ss->mFileInfoList = mFileInfoList;
    ss->mFileInfoListByName = ss->mFileInfoList;
    ss->mContentInfoList[0] = mContentInfoList;

    ss->mFileInfoList.sort(T::FileInfo::ComparisonMethod::fileId);
    ss->mFileInfoListByName.sort(T::FileInfo::ComparisonMethod::fileName);


    for (int t=1; t<CONTENT_LIST_COUNT; t++)
    {
      if (mContentInfoListEnabled[t])
        ss->mContentInfoList[t] = ss->mContentInfoList[0];
    }

    ss->mContentInfoList[0].sort(T::ContentInfo::ComparisonMethod::file_message);

    if (mContentInfoListEnabled[1])
      ss->mContentInfoList[1].sort(T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time);

    if (mContentInfoListEnabled[2])
      ss->mContentInfoList[2].sort(T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time);

    if (mContentInfoListEnabled[3])
      ss->mContentInfoList[3].sort(T::ContentInfo::ComparisonMethod::gribId_producer_generation_level_time);

    if (mContentInfoListEnabled[4])
      ss->mContentInfoList[4].sort(T::ContentInfo::ComparisonMethod::newbaseId_producer_generation_level_time);

    if (mContentInfoListEnabled[5])
      ss->mContentInfoList[5].sort(T::ContentInfo::ComparisonMethod::newbaseName_producer_generation_level_time);

    if (mContentInfoListEnabled[6])
      ss->mContentInfoList[6].sort(T::ContentInfo::ComparisonMethod::cdmId_producer_generation_level_time);

    if (mContentInfoListEnabled[7])
      ss->mContentInfoList[7].sort(T::ContentInfo::ComparisonMethod::cdmName_producer_generation_level_time);

    if (mSSI == 1)
      mSSI = 0;
    else
      mSSI = 1;

    mDataSwapTime = time(nullptr);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}


}
}
