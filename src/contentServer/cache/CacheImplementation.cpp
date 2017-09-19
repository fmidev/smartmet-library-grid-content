#include "CacheImplementation.h"

#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"
#include "grid-files/common/AutoWriteLock.h"
#include "grid-files/common/AutoReadLock.h"
#include "grid-files/common/AutoThreadLock.h"
#include "grid-files/common/ShowFunction.h"


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
    return NULL;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Operation failed!",NULL);
    exception.printError();
    exit(-1);
  }
}





CacheImplementation::CacheImplementation()
{
  FUNCTION_TRACE
  try
  {
    mContentStorage = NULL;
    mContentStorageStartTime = 0;
    mStartTime = 0;
    mSessionId = 0;
    mShutdownRequested = false;
    mUpdateInProgress = false;
    mLastProcessedEventId = 0;
    mDataServerCount = 0xFFFFFFFF;
    mProducerCount = 0xFFFFFFFF;
    mGenerationCount = 0xFFFFFFFF;
    mFileCount = 0xFFFFFFFF;
    mContentCount = 0xFFFFFFFF;
    mSaveEnabled = false;
    mSaveDir = "/tmp";
    mReloadActivated = false;

    mFileInfoList.setComparisonMethod(T::FileInfo::ComparisonMethod::none);
    mContentInfoList[0].setComparisonMethod(T::ContentInfo::ComparisonMethod::file_message);
    for (int t=1; t<CONTENT_LIST_COUNT; t++)
      mContentInfoList[t].setReleaseObjects(false);

    mDelayedContentAddList.setReleaseObjects(false);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::init(T::SessionId sessionId,ServiceInterface *contentStorage)
{
  FUNCTION_TRACE
  try
  {
    mStartTime = time(0);
    mSessionId = sessionId;
    mContentStorage = contentStorage;

    if (mContentStorage != NULL)
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

      readContentList();

      for (int t=1; t<CONTENT_LIST_COUNT; t++)
        mContentInfoList[t] = mContentInfoList[0];

      mContentInfoList[0].sort(T::ContentInfo::ComparisonMethod::file_message);
      mContentInfoList[1].sort(T::ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message);
      mContentInfoList[2].sort(T::ContentInfo::ComparisonMethod::fmiName_level_starttime_file_message);
      mContentInfoList[3].sort(T::ContentInfo::ComparisonMethod::gribId_level_starttime_file_message);
      mContentInfoList[4].sort(T::ContentInfo::ComparisonMethod::newbaseId_level_starttime_file_message);
      mContentInfoList[5].sort(T::ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message);
      mContentInfoList[6].sort(T::ContentInfo::ComparisonMethod::cdmId_level_starttime_file_message);
      mContentInfoList[7].sort(T::ContentInfo::ComparisonMethod::cdmName_level_starttime_file_message);

      mUpdateInProgress = false;
    }
  }
  catch (...)
  {
    mUpdateInProgress = false;
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::startEventProcessing()
{
  FUNCTION_TRACE
  try
  {
    pthread_create(&mThread,NULL,CacheImplementation_eventProcessingThread,this);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_clear(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->clear(sessionId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_reload(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    mContentStorage->reload(sessionId);

    reloadData();
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::reloadData()
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage != NULL)
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

      for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
        mContentInfoList[t].clear();

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

      for (int t=1; t<CONTENT_LIST_COUNT; t++)
        mContentInfoList[t] = mContentInfoList[0];

      mContentInfoList[0].sort(T::ContentInfo::ComparisonMethod::file_message);
      mContentInfoList[1].sort(T::ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message);
      mContentInfoList[2].sort(T::ContentInfo::ComparisonMethod::fmiName_level_starttime_file_message);
      mContentInfoList[3].sort(T::ContentInfo::ComparisonMethod::gribId_level_starttime_file_message);
      mContentInfoList[4].sort(T::ContentInfo::ComparisonMethod::newbaseId_level_starttime_file_message);
      mContentInfoList[5].sort(T::ContentInfo::ComparisonMethod::newbaseName_level_starttime_file_message);
      mContentInfoList[6].sort(T::ContentInfo::ComparisonMethod::cdmId_level_starttime_file_message);
      mContentInfoList[7].sort(T::ContentInfo::ComparisonMethod::cdmName_level_starttime_file_message);

      mUpdateInProgress = false;

      mStartTime = time(0);

      T::EventInfo event;
      event.mType = EventType::CONTENT_SERVER_RELOAD;
      mContentStorage->addEventInfo(mSessionId,event);

      mReloadActivated = false;
    }
  }
  catch (...)
  {
    mUpdateInProgress = false;
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_addDataServerInfo(T::SessionId sessionId,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addDataServerInfo(sessionId,serverInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteDataServerInfoById(T::SessionId sessionId,uint serverId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteDataServerInfoById(sessionId,serverId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getDataServerInfoById(T::SessionId sessionId,uint serverId,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getDataServerInfoById(sessionId,serverId,serverInfo);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ServerInfo *info = mDataServerInfoList.getServerInfoById(serverId);
    if (info == NULL)
      return Result::DATA_NOT_FOUND;

    serverInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getDataServerInfoByName(T::SessionId sessionId,std::string serverName,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getDataServerInfoByName(sessionId,serverName,serverInfo);;

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ServerInfo *info = mDataServerInfoList.getServerInfoByName(serverName);
    if (info == NULL)
      return Result::DATA_NOT_FOUND;

    serverInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getDataServerInfoByIor(T::SessionId sessionId,std::string serverIor,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getDataServerInfoByIor(sessionId,serverIor,serverInfo);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ServerInfo *info = mDataServerInfoList.getServerInfoByIor(serverIor);
    if (info == NULL)
      return Result::DATA_NOT_FOUND;

    serverInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getDataServerInfoList(T::SessionId sessionId,T::ServerInfoList& serverInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getDataServerInfoList(sessionId,serverInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    serverInfoList = mDataServerInfoList;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getDataServerInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getDataServerInfoCount(sessionId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mDataServerInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addProducerInfo(sessionId,producerInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteProducerInfoById(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteProducerInfoById(sessionId,producerId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteProducerInfoByName(T::SessionId sessionId,std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

     int result = mContentStorage->deleteProducerInfoByName(sessionId,producerName);
     processEvents(false);
     return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getProducerInfoById(sessionId,producerId,producerInfo);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ProducerInfo *info = mProducerInfoList.getProducerInfoById(producerId);
    if (info == NULL)
      return Result::DATA_NOT_FOUND;

    producerInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteProducerInfoListBySourceId(sessionId,sourceId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getProducerInfoByName(T::SessionId sessionId,std::string producerName,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getProducerInfoByName(sessionId,producerName,producerInfo);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ProducerInfo *info = mProducerInfoList.getProducerInfoByName(producerName);
    if (info == NULL)
      return Result::DATA_NOT_FOUND;

    producerInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getProducerInfoList(sessionId,producerInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    producerInfoList = mProducerInfoList;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getProducerInfoListBySourceId(sessionId,sourceId,producerInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    mProducerInfoList.getProducerInfoListBySourceId(sourceId,producerInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getProducerInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getProducerInfoCount(sessionId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mProducerInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addGenerationInfo(sessionId,generationInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteGenerationInfoById(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteGenerationInfoById(sessionId,generationId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteGenerationInfoByName(T::SessionId sessionId,std::string generationName)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteGenerationInfoByName(sessionId,generationName);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteGenerationInfoListByProducerId(sessionId,producerId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteGenerationInfoListByProducerName(sessionId,producerName);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteGenerationInfoListBySourceId(sessionId,sourceId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getGenerationIdGeometryIdAndForecastTimeList(sessionId,list);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;


    uint len = mContentInfoList[0].getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = mContentInfoList[0].getContentInfoByIndex(t);
      char st[200];
      sprintf(st,"%u;%u;%d;%d;%s;%s;",info->mGenerationId,info->mGeometryId,info->mForecastType,info->mForecastNumber,info->mForecastTime.c_str(),info->mModificationTime.c_str());
      std::string str = st;


      if (list.find(str) == list.end())
        list.insert(str);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getGenerationInfoListByGeometryId(T::SessionId sessionId,uint geometryId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getGenerationInfoListByGeometryId(sessionId,geometryId,generationInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;


    std::set<uint> idList;

    mContentInfoList[0].getGenerationIdListByGeometryId(geometryId,idList);

    std::set<uint>::iterator it;
    for (it=idList.begin(); it!=idList.end(); ++it)
    {
      T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(*it);
      if (generationInfo != NULL)
        generationInfoList.addGenerationInfo(generationInfo->duplicate());
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getGenerationInfoById(sessionId,generationId,generationInfo);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoById(generationId);
    if (info == NULL)
      return Result::DATA_NOT_FOUND;

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getGenerationInfoByName(T::SessionId sessionId,std::string generationName,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getGenerationInfoByName(sessionId,generationName,generationInfo);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (info == NULL)
      return Result::DATA_NOT_FOUND;

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getGenerationInfoList(sessionId,generationInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    generationInfoList = mGenerationInfoList;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getGenerationInfoListByProducerId(sessionId,producerId,generationInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    mGenerationInfoList.getGenerationInfoListByProducerId(producerId,generationInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
   if (mUpdateInProgress)
      return mContentStorage->getGenerationInfoListByProducerName(sessionId,producerName,generationInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == NULL)
      return Result::UNKNOWN_PRODUCER_NAME;

    mGenerationInfoList.getGenerationInfoListByProducerId(producerInfo->mProducerId,generationInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getGenerationInfoListBySourceId(sessionId,sourceId,generationInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    mGenerationInfoList.getGenerationInfoListBySourceId(sourceId,generationInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,T::GenerationStatus generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getLastGenerationInfoByProducerIdAndStatus(sessionId,producerId,generationStatus,generationInfo);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == NULL)
      return Result::UNKNOWN_PRODUCER_ID;


    T::GenerationInfo *info = mGenerationInfoList.getLastGenerationInfoByProducerIdAndStatus(producerId,generationStatus);
    if (info == NULL)
      return Result::DATA_NOT_FOUND;

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,std::string producerName,T::GenerationStatus generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getLastGenerationInfoByProducerNameAndStatus(sessionId,producerName,generationStatus,generationInfo);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == NULL)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo *info = mGenerationInfoList.getLastGenerationInfoByProducerIdAndStatus(producerInfo->mProducerId,generationStatus);
    if (info == NULL)
      return Result::DATA_NOT_FOUND;

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getGenerationInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getGenerationInfoCount(sessionId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mGenerationInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,T::GenerationStatus status)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->setGenerationInfoStatusById(sessionId,generationId,status);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_setGenerationInfoStatusByName(T::SessionId sessionId,std::string generationName,T::GenerationStatus status)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->setGenerationInfoStatusByName(sessionId,generationName,status);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addFileInfo(sessionId,fileInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addFileInfoWithContentList(sessionId,fileInfo,contentInfoList);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteFileInfoById(T::SessionId sessionId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoById(sessionId,fileId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteFileInfoByName(T::SessionId sessionId,std::string filename)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoByName(sessionId,filename);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListByGroupFlags(sessionId,groupFlags);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListByProducerId(sessionId,producerId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteFileInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListByProducerName(sessionId,producerName);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListByGenerationId(sessionId,generationId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,uint geometryId,short forecastType,short forecastNumber,std::string forecastTime)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListByGenerationIdAndForecastTime(sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListByGenerationName(sessionId,generationName);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListBySourceId(sessionId,sourceId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteFileInfoListByFileIdList(sessionId,fileIdList);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getFileInfoById(sessionId,fileId,fileInfo);

    //AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::FileInfo *info = mFileInfoList.getFileInfoById(fileId);
    if (info == NULL)
      return Result::DATA_NOT_FOUND;

    fileInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getFileInfoByName(T::SessionId sessionId,std::string filename,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getFileInfoByName(sessionId,filename,fileInfo);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::FileInfo *info = mFileInfoList.getFileInfoByName(filename);
    if (info == NULL)
      return Result::DATA_NOT_FOUND;

    fileInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getFileInfoList(T::SessionId sessionId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getFileInfoList(sessionId,startFileId,maxRecords,fileInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    mFileInfoList.getFileInfoList(startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getFileInfoListByProducerId(sessionId,producerId,startFileId,maxRecords,fileInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == NULL)
      return Result::UNKNOWN_PRODUCER_ID;

    mFileInfoList.getFileInfoListByProducerId(producerId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getFileInfoListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getFileInfoListByProducerName(sessionId,producerName,startFileId,maxRecords,fileInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == NULL)
      return Result::UNKNOWN_PRODUCER_NAME;

    mFileInfoList.getFileInfoListByProducerId(producerInfo->mProducerId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getFileInfoListByGenerationId(sessionId,generationId,startFileId,maxRecords,fileInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == NULL)
      return Result::UNKNOWN_GENERATION_ID;

    mFileInfoList.getFileInfoListByGenerationId(generationId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getFileInfoListByGenerationName(sessionId,generationName,startFileId,maxRecords,fileInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == NULL)
      return Result::UNKNOWN_GENERATION_NAME;

    mFileInfoList.getFileInfoListByGenerationId(generationInfo->mGenerationId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getFileInfoListByGroupFlags(sessionId,groupFlags,startFileId,maxRecords,fileInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    mFileInfoList.getFileInfoListByGroupFlags(groupFlags,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getFileInfoListBySourceId(sessionId,sourceId,startFileId,maxRecords,fileInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    mFileInfoList.getFileInfoListBySourceId(sourceId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getFileInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getFileInfoCount(sessionId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mFileInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getFileInfoCountByProducerId(T::SessionId sessionId,uint producerId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getFileInfoCountByProducerId(sessionId,producerId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mFileInfoList.getFileInfoCountByProducerId(producerId);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




int CacheImplementation::_getFileInfoCountByGenerationId(T::SessionId sessionId,uint generationId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getFileInfoCountByGenerationId(sessionId,generationId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mFileInfoList.getFileInfoCountByGenerationId(generationId);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getFileInfoCountBySourceId(T::SessionId sessionId,uint sourceId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getFileInfoCountBySourceId(sessionId,sourceId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mFileInfoList.getFileInfoCountBySourceId(sourceId);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addEventInfo(sessionId,eventInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getLastEventInfo(sessionId,requestingServerId,eventInfo);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (requestingServerId != 0)
    {
      T::ServerInfo *info = mDataServerInfoList.getServerInfoById(requestingServerId);
      if (info != NULL)
        info->mLastCall = time(0);
    }

    T::EventInfo *lastEvent = mEventInfoList.getLastEvent();
    if (lastEvent == NULL)
      return Result::DATA_NOT_FOUND;

    eventInfo = *lastEvent;
    eventInfo.mServerTime = mStartTime;

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,uint maxRecords,T::EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getEventInfoList(sessionId,requestingServerId,startEventId,maxRecords,eventInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (requestingServerId != 0)
    {
      T::ServerInfo *info = mDataServerInfoList.getServerInfoById(requestingServerId);
      if (info != NULL)
        info->mLastCall = time(0);
    }

    mEventInfoList.getEventInfoList(startEventId,maxRecords,eventInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getEventInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getEventInfoCount(sessionId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mEventInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addContentInfo(sessionId,contentInfo);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->addContentList(sessionId,contentInfoList);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentInfo(sessionId,fileId,messageIndex);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteContentListByFileId(T::SessionId sessionId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentListByFileId(sessionId,fileId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteContentListByFileName(T::SessionId sessionId,std::string filename)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentListByFileName(sessionId,filename);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteContentListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentListByGroupFlags(sessionId,groupFlags);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteContentListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentListByProducerId(sessionId,producerId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteContentListByProducerName(T::SessionId sessionId,std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentListByProducerName(sessionId,producerName);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteContentListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentListByGenerationId(sessionId,generationId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteContentListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentListByGenerationName(sessionId,generationName);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_deleteContentListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->deleteContentListBySourceId(sessionId,sourceId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_registerContentList(T::SessionId sessionId,uint serverId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->registerContentList(sessionId,serverId,contentInfoList);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_registerContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->registerContentListByFileId(sessionId,serverId,fileId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_unregisterContentList(T::SessionId sessionId,uint serverId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->unregisterContentList(sessionId,serverId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_unregisterContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return Result::NO_PERMANENT_STORAGE_DEFINED;

    int result = mContentStorage->unregisterContentListByFileId(sessionId,serverId,fileId);
    processEvents(false);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentInfo(sessionId,fileId,messageIndex,contentInfo);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ContentInfo *cInfo = mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(fileId,messageIndex);
    if (cInfo != NULL)
    {
      contentInfo = *cInfo;
      return Result::OK;
    }

    return Result::DATA_NOT_FOUND;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentList(sessionId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    mContentInfoList[0].getContentInfoList(startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentListByFileId(sessionId,fileId,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    mContentInfoList[0].getContentInfoListByFileId(fileId,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentListByFileName(T::SessionId sessionId,std::string filename,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentListByFileName(sessionId,filename,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::FileInfo *fileInfo = mFileInfoList.getFileInfoByName(filename);
    if (fileInfo == NULL)
      return Result::UNKNOWN_FILE_NAME;

    mContentInfoList[0].getContentInfoListByFileId(fileInfo->mFileId,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentListByServerId(T::SessionId sessionId,uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentListByServerId(sessionId,serverId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ServerInfo *info = mDataServerInfoList.getServerInfoById(serverId);
    if (info == NULL)
      return Result::UNKNOWN_SERVER_ID;

    mContentInfoList[0].getContentInfoListByServerId(serverId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentListByGroupFlags(sessionId,groupFlags,startFileId,startMessageIndex,maxRecords,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    mContentInfoList[0].getContentInfoListByGroupFlags(groupFlags,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentListByProducerId(sessionId,producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == NULL)
      return Result::UNKNOWN_PRODUCER_ID;

    mContentInfoList[0].getContentInfoListByProducerId(producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentListByProducerName(sessionId,producerName,startFileId,startMessageIndex,maxRecords,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == NULL)
      return Result::UNKNOWN_PRODUCER_NAME;

    mContentInfoList[0].getContentInfoListByProducerId(producerInfo->mProducerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




int CacheImplementation::_getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentListByGenerationId(sessionId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    mContentInfoList[0].getContentInfoListByGenerationId(generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentListByGenerationName(sessionId,generationName,startFileId,maxRecords,startMessageIndex,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == NULL)
      return Result::UNKNOWN_GENERATION_NAME;

    mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mGenerationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTime,endTime,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    mContentInfoList[0].getContentInfoListByGenerationId(generationId,startTime,endTime,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,std::string generationName,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentListByGenerationNameAndTimeRange(sessionId,generationName,startTime,endTime,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == NULL)
      return Result::UNKNOWN_GENERATION_NAME;

    mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mGenerationId,startTime,endTime,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentListBySourceId(sessionId,sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    mContentInfoList[0].getContentInfoListBySourceId(sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {

    //printf("getContentListByParameter(%llu,%u,%s,%u,%u,%u,%u,%s,%s,%u)\n",sessionId,(uint)parameterKeyType,parameterKey.c_str(),(uint)parameterLevelIdType,(uint)parameterLevelId,minLevel,maxLevel,startTime.c_str(),endTime.c_str(),requestFlags);


    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameter(sessionId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    switch (parameterKeyType)
    {
      case T::ParamKeyType::FMI_ID:
        mContentInfoList[1].getContentInfoListByFmiParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::FMI_NAME:
      {
        mContentInfoList[2].getContentInfoListByFmiParameterName(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;
      }

      case T::ParamKeyType::GRIB_ID:
        mContentInfoList[3].getContentInfoListByGribParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::NEWBASE_ID:
        mContentInfoList[4].getContentInfoListByNewbaseParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::NEWBASE_NAME:
        mContentInfoList[5].getContentInfoListByNewbaseParameterName(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::CDM_ID:
        mContentInfoList[6].getContentInfoListByCdmParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::CDM_NAME:
        mContentInfoList[7].getContentInfoListByCdmParameterName(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == NULL)
      return Result::UNKNOWN_GENERATION_ID;

    switch (parameterKeyType)
    {
      case T::ParamKeyType::FMI_ID:
        mContentInfoList[1].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::FMI_NAME:
        mContentInfoList[2].getContentInfoListByFmiParameterNameAndGenerationId(generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::GRIB_ID:
        mContentInfoList[3].getContentInfoListByGribParameterIdAndGenerationId(generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::NEWBASE_ID:
        mContentInfoList[4].getContentInfoListByNewbaseParameterIdAndGenerationId(generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::NEWBASE_NAME:
        mContentInfoList[5].getContentInfoListByNewbaseParameterNameAndGenerationId(generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::CDM_ID:
        mContentInfoList[6].getContentInfoListByCdmParameterIdAndGenerationId(generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::CDM_NAME:
        mContentInfoList[7].getContentInfoListByCdmParameterNameAndGenerationId(generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentListByParameterAndGenerationName(T::SessionId sessionId,std::string generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == NULL)
      return Result::UNKNOWN_GENERATION_NAME;

    switch (parameterKeyType)
    {
      case T::ParamKeyType::FMI_ID:
        mContentInfoList[1].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::FMI_NAME:
        mContentInfoList[2].getContentInfoListByFmiParameterNameAndGenerationId(generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::GRIB_ID:
        mContentInfoList[3].getContentInfoListByGribParameterIdAndGenerationId(generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::NEWBASE_ID:
        mContentInfoList[4].getContentInfoListByNewbaseParameterIdAndGenerationId(generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::NEWBASE_NAME:
        mContentInfoList[5].getContentInfoListByNewbaseParameterNameAndGenerationId(generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::CDM_ID:
        mContentInfoList[6].getContentInfoListByCdmParameterIdAndGenerationId(generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::CDM_NAME:
        mContentInfoList[7].getContentInfoListByCdmParameterNameAndGenerationId(generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




int CacheImplementation::_getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == NULL)
      return Result::UNKNOWN_PRODUCER_ID;

    switch (parameterKeyType)
    {
      case T::ParamKeyType::FMI_ID:
        mContentInfoList[1].getContentInfoListByFmiParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::FMI_NAME:
        mContentInfoList[2].getContentInfoListByFmiParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::GRIB_ID:
        mContentInfoList[3].getContentInfoListByGribParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::NEWBASE_ID:
        mContentInfoList[4].getContentInfoListByNewbaseParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::NEWBASE_NAME:
        mContentInfoList[5].getContentInfoListByNewbaseParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::CDM_ID:
        mContentInfoList[6].getContentInfoListByCdmParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::CDM_NAME:
        mContentInfoList[7].getContentInfoListByCdmParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




int CacheImplementation::_getContentListByParameterAndProducerName(T::SessionId sessionId,std::string producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);

    AutoReadLock lock(&mModificationLock);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == NULL)
      return Result::UNKNOWN_PRODUCER_NAME;

    switch (parameterKeyType)
    {
      case T::ParamKeyType::FMI_ID:
        mContentInfoList[1].getContentInfoListByFmiParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::FMI_NAME:
        mContentInfoList[2].getContentInfoListByFmiParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::GRIB_ID:
        mContentInfoList[3].getContentInfoListByGribParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::NEWBASE_ID:
        mContentInfoList[4].getContentInfoListByNewbaseParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::NEWBASE_NAME:
        mContentInfoList[5].getContentInfoListByNewbaseParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::CDM_ID:
        mContentInfoList[6].getContentInfoListByCdmParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyType::CDM_NAME:
        mContentInfoList[7].getContentInfoListByCdmParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      default:
        return Result::UNKNOWN_PARAMETER_KEY_TYPE;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<uint>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentGeometryIdListByGenerationId(sessionId,generationId,geometryIdList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    mContentInfoList[0].getContentGeometryIdListByGenerationId(generationId,geometryIdList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentParamListByGenerationId(sessionId,generationId,contentParamList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::ContentInfoList contentInfoList;
    mContentInfoList[0].getContentInfoListByGenerationId(generationId,0,0,1000000,contentInfoList);
    contentInfoList.sort(T::ContentInfo::ComparisonMethod::fmiName_fmiLevelId_level_starttime_file_message);
    uint len = contentInfoList.getLength();
    T::ContentInfo *prev = NULL;
    T::ContentInfo *currentInfo = NULL;
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);

      if (prev == NULL ||
          info->mFmiParameterName != prev->mFmiParameterName ||
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,uint geometryId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentTimeListByGenerationAndGeometryId(sessionId,generationId,geometryId,contentTimeList);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::ContentInfoList contentInfoList;
    mContentInfoList[0].getContentInfoListByGenerationAndGeometryId(generationId,geometryId,0,0,1000000,contentInfoList);

    contentInfoList.sort(T::ContentInfo::ComparisonMethod::generationId_starttime_file_message);
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);

      if (contentTimeList.find(info->mForecastTime) == contentTimeList.end())
      {
        contentTimeList.insert(info->mForecastTime);
      }
    }
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int CacheImplementation::_getContentCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mUpdateInProgress)
      return mContentStorage->getContentCount(sessionId,count);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mContentInfoList[0].getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::readProducerList()
{
  FUNCTION_TRACE
  try
  {
   if (mContentStorage == NULL)
      return;

    mProducerInfoList.clear();

    int result = mContentStorage->getProducerInfoList(mSessionId,mProducerInfoList);
    if (result != 0)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot read the producer list from the content storage!");
      exception.addParameter("ServiceResult",getResultString(result));
      throw exception;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void CacheImplementation::readGenerationList()
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return;

    mGenerationInfoList.clear();

    int result = mContentStorage->getGenerationInfoList(mSessionId,mGenerationInfoList);
    if (result != 0)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot read the generation list from the content storage!");
      exception.addParameter("ServiceResult",getResultString(result));
      throw exception;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void CacheImplementation::readFileList()
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return;

    mFileInfoList.clear();

    uint startFileId = 0;
    uint len = 10000;
    while (len == 10000)
    {
      T::FileInfoList fileInfoList;

      int result = mContentStorage->getFileInfoList(mSessionId,startFileId,10000,fileInfoList);
      if (result != 0)
      {
        SmartMet::Spine::Exception exception(BCP,"Cannot read the file list from the content storage!");
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void CacheImplementation::readContentList()
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return;

    mContentInfoList[0].clear();

    uint startFileId = 0;
    uint startMessageIndex = 0;
    uint len = 10000;
    while (len > 0)
    {
      T::ContentInfoList contentInfoList;

      int result = mContentStorage->getContentList(mSessionId,startFileId,startMessageIndex,10000,contentInfoList);
      if (result != 0)
      {
        SmartMet::Spine::Exception exception(BCP,"Cannot read the content list from the content storage!");
        exception.addParameter("ServiceResult",getResultString(result));
        throw exception;
      }

      len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
        startFileId = contentInfo->mFileId;
        startMessageIndex = contentInfo->mMessageIndex + 1;

        mContentInfoList[0].addContentInfo(contentInfo->duplicate());
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}



void CacheImplementation::readDataServerList()
{
  FUNCTION_TRACE
  try
  {
   if (mContentStorage == NULL)
      return;

    mDataServerInfoList.clear();

    int result = mContentStorage->getDataServerInfoList(mSessionId,mDataServerInfoList);
    if (result != 0)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot read the data server list from the content storage!");
      exception.addParameter("ServiceResult",getResultString(result));
      throw exception;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_clear(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: clear\n",eventInfo.mEventId);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    mFileInfoList.clear();
    mProducerInfoList.clear();
    mGenerationInfoList.clear();
    mDataServerInfoList.clear();
    mEventInfoList.clear();

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_contentServerReload(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: reload\n",eventInfo.mEventId);
#if 0
    if (mContentStorage != NULL)
    {
      AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

      mUpdateInProgress = true;
      mLastProcessedEventId = 0;

      mFileInfoList.clear();
      mProducerInfoList.clear();
      mGenerationInfoList.clear();
      mDataServerInfoList.clear();
      mContentServerInfoList.clear();
      mEventInfoList.clear();

      for (int t=CONTENT_LIST_COUNT; t>=0; t--)
        mContentInfoList[t].clear();

      mMaxFileId = 0;
      mMaxGenerationId = 0;
      mMaxEventId = 0;

      T::EventInfo eventInfo;
      mUpdateInProgress = true;
      mContentStorage->getLastEventInfo(mSessionId,0,0,eventInfo);

      mLastProcessedEventId = eventInfo.mEventId;
      mContentStorageStartTime = eventInfo.mServerTime;

      readDataServerList();
      readProducerList();
      readGenerationList();
      readFileList();

      mFileInfoList.sort(T::FileInfo::ComparisonMethod::fileId);

      readContentList();

      for (int t=1; t<CONTENT_LIST_COUNT; t++)
        mContentInfoList[t] = mContentInfoList[0];

      mContentInfoList[0].sort(T::ContentInfo::ComparisonMethod::file_message);
      mContentInfoList[1].sort(T::ContentInfo::ComparisonMethod::fmiId_level_starttime_file_message);
      mContentInfoList[2].sort(T::ContentInfo::ComparisonMethod::gribId_level_starttime_file_message);
      mUpdateInProgress = false;
    }
#endif
  }
  catch (...)
  {
    mUpdateInProgress = false;
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
      mProducerInfoList.addProducerInfo(producerInfo.duplicate());
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_producerDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: producerDeleted(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByProducerId(eventInfo.mId1);

    mFileInfoList.deleteFileInfoByProducerId(eventInfo.mId1);
    mProducerInfoList.deleteProducerInfoById(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_producerListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: producerListDeletedBySourceId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoBySourceId(eventInfo.mId1);

    mFileInfoList.deleteFileInfoBySourceId(eventInfo.mId1);
    mGenerationInfoList.deleteGenerationInfoListBySourceId(eventInfo.mId1);
    mProducerInfoList.deleteProducerInfoListBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_generationDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: generationDeleted(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGenerationId(eventInfo.mId1);

    mFileInfoList.deleteFileInfoByGenerationId(eventInfo.mId1);

    mGenerationInfoList.deleteGenerationInfoById(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_generationStatusChanged(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: generationStatusChanged(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoById(eventInfo.mId1);
    if (info != NULL)
      info->mStatus = (T::GenerationStatus)eventInfo.mId2;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_generationListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: generationListDeletedByProducerId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByProducerId(eventInfo.mId1);

    mFileInfoList.deleteFileInfoByProducerId(eventInfo.mId1);
    mGenerationInfoList.deleteGenerationInfoListByProducerId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_generationListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: generationListDeletedBySourceId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoBySourceId(eventInfo.mId1);

    mFileInfoList.deleteFileInfoBySourceId(eventInfo.mId1);
    mGenerationInfoList.deleteGenerationInfoListBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_fileAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: fileAdded(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    uint len = eventInfo.mNote.length();

    if (len > 0)
    {
      char buf[len+10];
      strcpy(buf,eventInfo.mNote.c_str());
      char *s = buf;

      while (s != NULL)
      {
        char *p = strstr(s,"\n");
        if (p != NULL)
        {
          *p = '\0';
          if (s == buf)
          {
            T::FileInfo *fileInfo = new T::FileInfo();
            fileInfo->setCsv(s);
            if (mFileInfoList.getFileInfoById(fileInfo->mFileId) == NULL)
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
            if (mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(contentInfo->mFileId,contentInfo->mMessageIndex) == NULL)
            {
              mContentInfoList[0].addContentInfo(contentInfo);
              mDelayedContentAddList.addContentInfo(contentInfo);

//              for (int t=0; t<1 /*CONTENT_LIST_COUNT*/; t++)
//                mContentInfoList[t].addContentInfo(contentInfo);
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
          s = NULL;
        }
      }
    }
    else
    {
      T::FileInfo fileInfo;
      if (mContentStorage->getFileInfoById(mSessionId,eventInfo.mId1,fileInfo) == Result::OK)
      {
        mFileInfoList.addFileInfo(fileInfo.duplicate());
        if (fileInfo.mFlags & (uint)T::FileInfoFlags::CONTENT_PREDEFINED)
        {
          T::ContentInfoList contentInfoList;
          if (mContentStorage->getContentListByFileId(mSessionId,fileInfo.mFileId,contentInfoList) == Result::OK)
          {
            uint len = contentInfoList.getLength();
            for (uint c=0; c<len; c++)
            {
              T::ContentInfo *info = contentInfoList.getContentInfoByIndex(c);
              T::ContentInfo *cInfo = info->duplicate();

              mContentInfoList[0].addContentInfo(cInfo);
              mDelayedContentAddList.addContentInfo(cInfo);

              //for (int t=0; t<CONTENT_LIST_COUNT; t++)
              //  mContentInfoList[t].addContentInfo(cInfo);
            }
          }
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_fileDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: fileDeleted(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::FileInfo *fileInfo = mFileInfoList.getFileInfoById(eventInfo.mId1);
    if (fileInfo != NULL)
    {
      if (mDelayedContentDeleteList.find(fileInfo->mFileId) == mDelayedContentDeleteList.end())
        mDelayedContentDeleteList.insert(fileInfo->mFileId);

      //for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      //  mContentInfoList[t].deleteContentInfoByFileId(eventInfo.mId1);

      mFileInfoList.deleteFileInfoById(eventInfo.mId1);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
      if (info != NULL)
      {
        // ToDo: Deleting content one by one is quite slow. It would
        // be smarter to mark content records as deleted instead, and
        // remove them time to time.

        //unsigned long long startTime = getTime();

        for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
          mContentInfoList[t].deleteContentInfoByFileId(eventInfo.mId1);


        //unsigned long long endTime = getTime();
        //printf("DTIME : %f sec\n",(float)(endTime-startTime)/1000000);

        *info = fileInfo;
      }
      else
      {
        mFileInfoList.addFileInfo(fileInfo.duplicate());
      }

      if (fileInfo.mFlags & (uint)T::FileInfoFlags::CONTENT_PREDEFINED)
      {
        T::ContentInfoList contentInfoList;
        if (mContentStorage->getContentListByFileId(mSessionId,fileInfo.mFileId,contentInfoList) == Result::OK)
        {
          //unsigned long long startTime = getTime();

          uint len = contentInfoList.getLength();
          for (uint c=0; c<len; c++)
          {
            T::ContentInfo *info = contentInfoList.getContentInfoByIndex(c);
            T::ContentInfo *cInfo = info->duplicate();

            for (int t=0; t<CONTENT_LIST_COUNT; t++)
              mContentInfoList[t].addContentInfo(cInfo);
          }

          //unsigned long long endTime = getTime();
          //printf("ATIME : %f sec\n",(float)(endTime-startTime)/1000000);
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_fileListDeletedByGroupFlags(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: fileListDeletedByGroupFlags(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGroupFlags(eventInfo.mId1);

    mFileInfoList.deleteFileInfoByGroupFlags(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_fileListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: fileListDeletedByProducerId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByProducerId(eventInfo.mId1);

    mFileInfoList.deleteFileInfoByProducerId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_fileListDeletedByGenerationId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: fileListDeletedByGenerationId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGenerationId(eventInfo.mId1);

    mFileInfoList.deleteFileInfoByGenerationId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}






void CacheImplementation::event_fileListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: fileListDeletedBySourceId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoBySourceId(eventInfo.mId1);

    mFileInfoList.deleteFileInfoBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_contentListDeletedByFileId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: contentListDeletedByFileId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByFileId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_contentListDeletedByGroupFlags(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: contentListDeletedByGroupFlags(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGroupFlags(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_contentListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: contentListDeletedByProducerId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByProducerId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_contentListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: contentListDeletedBySourceId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_contentListDeletedByGenerationId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: contentListDeletedByGenerationId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGenerationId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_dataServerDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: dataServerDeleted(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList[0].unregisterContentInfoByServerId(eventInfo.mId1);
    mContentCount = 0xFFFFFFFF;

    mDataServerInfoList.deleteServerInfoById(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_contentAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: contentAdded(%u,%u)\n",eventInfo.mEventId,eventInfo.mId1,eventInfo.mId2);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ContentInfo contentInfo;
    if (mContentStorage->getContentInfo(mSessionId,eventInfo.mId1,eventInfo.mId2,contentInfo) == Result::OK)
    {
      //contentInfo.print(std::cout,0,0);
      T::ContentInfo *cInfo = contentInfo.duplicate();

      for (int t=0; t<CONTENT_LIST_COUNT; t++)
        mContentInfoList[t].addContentInfo(cInfo);
    }
    else
    {
      printf("#### CONTENT NOT FOUND: %u:%u\n",eventInfo.mId1,eventInfo.mId2);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::event_contentDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    // printf("EVENT[%llu]: contentDeleted(%u,%u)\n",eventInfo.mEventId,eventInfo.mId1,eventInfo.mId2);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ContentInfo *contentInfo = mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(eventInfo.mId1,eventInfo.mId2);
    if (contentInfo != NULL)
    {
      for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
        mContentInfoList[t].deleteContentInfoByFileIdAndMessageIndex(eventInfo.mId1,eventInfo.mId2);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
      printf("%s:%d:%s: Invalid server id (%u)!\n",__FILE__,__LINE__,__FUNCTION__,eventInfo.mId3);
      printf("Invalid server id %u\n",eventInfo.mId3);
      return;
    }

    unsigned long long sf = 1 << (eventInfo.mId3-1);

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ContentInfo *info = mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(eventInfo.mId1,eventInfo.mId2);
    if (info != NULL)
    {
      info->mServerFlags = info->mServerFlags | sf;
      mContentCount = 0xFFFFFFFF;
    }
    else
    {
      printf("%s:%d:%s: Cannot find the content (%u:%u)!\n",__FILE__,__LINE__,__FUNCTION__,eventInfo.mId1,eventInfo.mId2);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void CacheImplementation::processEvents(bool eventThread)
{
  FUNCTION_TRACE
  try
  {
    if (mContentStorage == NULL)
      return;

    if (!eventThread  &&  mReloadActivated)
    {
      // If this method is called from a service method (not from the event thread)
      // then we should exit, because otherwise the call is blocked until the reload
      // is finished.

      return;
    }

    AutoThreadLock eventLock(&mEventProcessingLock);

    T::EventInfoList delayedEventInfoList;

    uint len = 1000;
    while (len > 0)
    {
      len = 0;
      T::EventInfo eventInfo;
      int result = mContentStorage->getLastEventInfo(mSessionId,0,eventInfo);
      if (result != Result::OK)
        return;

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
        result = mContentStorage->getEventInfoList(mSessionId,0,mLastProcessedEventId+1,1000,eventInfoList);
        if (result != 0)
        {
          //printf("ERROR: getEventInfoList : %d\n",result);
          return;
        }

        len = eventInfoList.getLength();
        //printf("EVENT LIST %u\n",len);

        T::EventInfo *it = eventInfoList.getFirstEvent();
        while (it != NULL)
        {
          // printf("Process event %llu (%u)\n",it->mEventId,(uint)it->mType);
          //it->print(std::cout,2,0);
          processEvent(*it);
          {
            AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);
            //AutoThreadLock lock(&mEventLock);
            T::EventInfo *event = it->duplicate();
            event->mServerTime = mStartTime;

            delayedEventInfoList.addEventInfo(event);
            //mEventInfoList.addEventInfo(event);
            mLastProcessedEventId = it->mEventId;
          }

          it = it->nextItem;

          if (mShutdownRequested)
            return;
        }
      }
    }


    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    if (mDelayedContentAddList.getLength() > 0)
    {
      printf("**** CONTENT ADD TO WAIT %u\n",mDelayedContentAddList.getLength());

      for (int t=1; t<CONTENT_LIST_COUNT; t++)
        mContentInfoList[t].addContentInfoList(mDelayedContentAddList);

      mDelayedContentAddList.clear();

      //for (int t=0; t<CONTENT_LIST_COUNT; t++)
      //  printf("contentList[%u] = %u\n",t,mContentInfoList[t].getLength());
    }


    if (mDelayedContentDeleteList.size() > 0)
    {
      printf("**** CONTENT DELETE TO WAIT %u\n",(uint)mDelayedContentDeleteList.size());

      for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
        mContentInfoList[t].deleteContentInfoByFileIdList(mDelayedContentDeleteList);

      mDelayedContentDeleteList.clear();
    }


    T::EventInfo *it = delayedEventInfoList.getFirstEvent();
    while (it != NULL)
    {
      T::EventInfo *event = it->duplicate();
      mEventInfoList.addEventInfo(event);
      it = it->nextItem;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
        SmartMet::Spine::Exception exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void CacheImplementation::saveData()
{
  FUNCTION_TRACE
  try
  {
    if (mSaveEnabled)
    {
      AutoReadLock lock(&mModificationLock);

      if (mDataServerCount != mDataServerInfoList.getLength())
        mDataServerInfoList.writeToFile(mSaveDir + "/dataServers.csv");

      if (mProducerCount != mProducerInfoList.getLength())
        mProducerInfoList.writeToFile(mSaveDir + "/producer.csv");

      if (mGenerationCount != mGenerationInfoList.getLength())
        mGenerationInfoList.writeToFile(mSaveDir + "/generations.csv");

      if (mFileCount != mFileInfoList.getLength())
        mFileInfoList.writeToFile(mSaveDir + "/files.csv");

      if (mContentCount != mContentInfoList[0].getLength())
        mContentInfoList[0].writeToFile(mSaveDir + "/content.csv");

      mDataServerCount = mDataServerInfoList.getLength();
      mProducerCount = mProducerInfoList.getLength();
      mGenerationCount = mGenerationInfoList.getLength();
      mFileCount= mFileInfoList.getLength();
      mContentCount = mContentInfoList[0].getLength();
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




}
}
