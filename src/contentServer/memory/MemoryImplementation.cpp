#include "MemoryImplementation.h"
#include <macgyver/Exception.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/common/ShowFunction.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/identification/GridDef.h>
#include <boost/functional/hash.hpp>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF



namespace SmartMet
{
namespace ContentServer
{



static void* MemoryImplementation_syncProcessingThread(void *arg)
{
  try
  {
    MemoryImplementation *service = (MemoryImplementation*)arg;
    service->syncProcessingThread();
    return nullptr;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Operation failed!",nullptr);
    exception.printError();
    exit(-1);
  }
}





MemoryImplementation::MemoryImplementation()
{
  FUNCTION_TRACE
  try
  {
    mImplementationType = Implementation::Memory;
    mStartTime = time(nullptr);

    mContentLoadEnabled = false;
    mContentSaveEnabled = false;
    mContentSyncEnabled = false;
    mContentSaveInterval = 60;
    mEventsEnabled = true;

    mMaxProducerId = 0;
    mMaxGenerationId = 0;
    mMaxFileId = 0;
    mMaxEventId = 0;

    mProducerCount = 1000000000;
    mGenerationCount = 1000000000;
    mGeometryCount = 1000000000;
    mFileCount = 1000000000;
    mContentCount = 1000000000;

    mShutdownRequested = false;
    mUpdateInProgress = false;
    mReloadActivated = false;
    mLastSaveTime = time(nullptr);

    mProducerInfoList.setLockingEnabled(true);
    mGenerationInfoList.setLockingEnabled(true);
    mGeometryInfoList.setLockingEnabled(true);
    mFileInfoList.setComparisonMethod(T::FileInfo::ComparisonMethod::none);
    mFileInfoList.setLockingEnabled(true);

    mFileInfoListByName.setReleaseObjects(false);
    mFileInfoListByName.setComparisonMethod(T::FileInfo::ComparisonMethod::none);
    mFileInfoListByName.setLockingEnabled(true);

    mContentInfoList[0].setComparisonMethod(T::ContentInfo::ComparisonMethod::file_message);
    mContentInfoList[0].setLockingEnabled(true);
    mContentChangeTime = 0;

    for (int t=1; t<CONTENT_LIST_COUNT; t++)
    {
      mContentInfoList[t].setReleaseObjects(false);
      mContentInfoList[t].setLockingEnabled(true);
    }

    mProducerStorage_modificationTime = 0;
    mGenerationStorage_modificationTime = 0;
    mGeometryStorage_modificationTime = 0;
    mFileStorage_modificationTime = 0;
    mContentStorage_modificationTime = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





MemoryImplementation::~MemoryImplementation()
{
  FUNCTION_TRACE
  try
  {
    mShutdownRequested = true;
    pthread_join(mThread, nullptr);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void MemoryImplementation::init(bool contentLoadEnabled,bool contentSaveEnabled,bool contentSyncEnabled,bool eventsEnabled,const std::string& contentDir,uint contentSaveInterval)
{
  FUNCTION_TRACE
  try
  {
    mSourceInfo = "memory";

    if (contentSaveEnabled && contentSyncEnabled)
      throw Fmi::Exception(BCP,"Content save and sync cannot be set at the same time!");

    mEventsEnabled = eventsEnabled;

    mContentLoadEnabled = contentLoadEnabled;
    mContentSaveEnabled = contentSaveEnabled;
    mContentSyncEnabled = contentSyncEnabled;

    if (mContentSyncEnabled)
      mContentLoadEnabled = true;

    mContentDir = contentDir;
    mContentSaveInterval = contentSaveInterval;

    readProducerList();
    readGenerationList();
    readGeometryList();
    readFileList();

    mFileInfoListByName = mFileInfoList;

    mFileInfoList.sort(T::FileInfo::ComparisonMethod::fileId);
    mFileInfoListByName.sort(T::FileInfo::ComparisonMethod::fileName);

    readContentList();

    for (int t=1; t<CONTENT_LIST_COUNT; t++)
    {
      mContentInfoList[t] = mContentInfoList[0];
    }

    mContentInfoList[0].sort(T::ContentInfo::ComparisonMethod::file_message);
    mContentInfoList[1].sort(T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time);

    mUpdateInProgress = false;

    if (mContentSyncEnabled)
      startSyncProcessing();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






void MemoryImplementation::shutdown()
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





bool MemoryImplementation::isSessionValid(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MemoryImplementation::startSyncProcessing()
{
  FUNCTION_TRACE
  try
  {
    pthread_create(&mThread,nullptr,MemoryImplementation_syncProcessingThread,this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MemoryImplementation::syncProcessingThread()
{
  FUNCTION_TRACE
  try
  {
    if (!mContentSyncEnabled)
      return;

    char filename[200];
    time_t delay = 10;
    while (!mShutdownRequested)
    {
      try
      {

        time_t tt = time(0) - delay;

        sprintf(filename,"%s/producers.csv",mContentDir.c_str());
        time_t t1 = getFileModificationTime(filename);

        sprintf(filename,"%s/generations.csv",mContentDir.c_str());
        time_t t2 = getFileModificationTime(filename);

        sprintf(filename,"%s/geometries.csv",mContentDir.c_str());
        time_t t3 = getFileModificationTime(filename);

        sprintf(filename,"%s/files.csv",mContentDir.c_str());
        time_t t4 = getFileModificationTime(filename);

        sprintf(filename,"%s/content.csv",mContentDir.c_str());
        time_t t5 = getFileModificationTime(filename);

        time_t max = t1;
        if (t2 > max)
          max = t2;

        if (t3 > max)
          max = t3;

        if (t4 > max)
          max = t4;

        if (t5 > max)
          max = t5;

        if (max < tt && (mProducerStorage_modificationTime != t1 ||
                         mGenerationStorage_modificationTime != t2 ||
                         mGeometryStorage_modificationTime != t3 ||
                         mFileStorage_modificationTime != t4 ||
                         mContentStorage_modificationTime != t5))
        {
          while (!syncProducerList()) sleep(10);
          while (!syncGenerationList()) sleep(10);
          while (!syncGeometryList()) sleep(10);
          while (!syncFileList()) sleep(10);
          while (!syncContentList()) sleep(10);
        }
        sleep(10);
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





void MemoryImplementation::setEventListMaxLength(uint maxLength)
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





int MemoryImplementation::_clear(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    mFileInfoListByName.clear();
    mFileInfoList.clear();
    mProducerInfoList.clear();
    mGenerationInfoList.clear();
    mGeometryInfoList.clear();
    mEventInfoList.clear();

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].clear();

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_reload(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    mFileInfoListByName.clear();
    mFileInfoList.clear();
    mProducerInfoList.clear();
    mGenerationInfoList.clear();
    mGeometryInfoList.clear();
    mEventInfoList.clear();

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].clear();

    readProducerList();
    readGenerationList();
    readGeometryList();
    readFileList();

    mFileInfoListByName = mFileInfoList;

    mFileInfoList.sort(T::FileInfo::ComparisonMethod::fileId);
    mFileInfoListByName.sort(T::FileInfo::ComparisonMethod::fileName);

    readContentList();

    for (int t=1; t<CONTENT_LIST_COUNT; t++)
    {
      mContentInfoList[t] = mContentInfoList[0];
    }

    mContentInfoList[0].sort(T::ContentInfo::ComparisonMethod::file_message);
    mContentInfoList[1].sort(T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time);

    mUpdateInProgress = false;

   return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentChangeTime(T::SessionId sessionId,time_t& changeTime)
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





int MemoryImplementation::_addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *info = mProducerInfoList.getProducerInfoByName(producerInfo.mName);
    if (info != nullptr)
      return Result::PRODUCER_NAME_ALREADY_REGISTERED;

    if (producerInfo.mProducerId == 0)
    {
      mMaxProducerId++;
      producerInfo.mProducerId = mMaxProducerId;
      mProducerInfoList.addProducerInfo(producerInfo.duplicate());
    }
    else
    {
      T::ProducerInfo *info = mProducerInfoList.getProducerInfoById(producerInfo.mProducerId);
      if (info != nullptr)
        return Result::PRODUCER_ID_ALREADY_REGISTERED;

      mProducerInfoList.addProducerInfo(producerInfo.duplicate());
    }

    addEvent(EventType::PRODUCER_ADDED,producerInfo.mProducerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_setProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *info = mProducerInfoList.getProducerInfoById(producerInfo.mProducerId);
    if (info == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    T::ProducerInfo *info2 = mProducerInfoList.getProducerInfoByName(producerInfo.mName);
    if (info != nullptr && info2->mProducerId != producerInfo.mProducerId)
      return Result::PRODUCER_NAME_ALREADY_REGISTERED;

    *info = producerInfo;

    addEvent(EventType::PRODUCER_UPDATED,producerInfo.mProducerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteProducerInfoById(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *info = mProducerInfoList.getProducerInfoById(producerId);
    if (info == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByProducerId(producerId);

    mFileInfoListByName.deleteFileInfoByProducerId(producerId);
    mFileInfoList.deleteFileInfoByProducerId(producerId);

    mGeometryInfoList.deleteGeometryInfoListByProducerId(producerId);
    mGenerationInfoList.deleteGenerationInfoListByProducerId(producerId);

    mProducerInfoList.deleteProducerInfoById(producerId);

    addEvent(EventType::PRODUCER_DELETED,producerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteProducerInfoByName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *info = mProducerInfoList.getProducerInfoByName(producerName);
    if (info == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByProducerId(info->mProducerId);

    mFileInfoListByName.deleteFileInfoByProducerId(info->mProducerId);
    mFileInfoList.deleteFileInfoByProducerId(info->mProducerId);

    mGeometryInfoList.deleteGeometryInfoListByProducerId(info->mProducerId);
    mGenerationInfoList.deleteGenerationInfoListByProducerId(info->mProducerId);

    mProducerInfoList.deleteProducerInfoById(info->mProducerId);

    addEvent(EventType::PRODUCER_DELETED,info->mProducerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoBySourceId(sourceId);

    mFileInfoListByName.deleteFileInfoBySourceId(sourceId);
    mFileInfoList.deleteFileInfoBySourceId(sourceId);

    mGeometryInfoList.deleteGeometryInfoListBySourceId(sourceId);
    mGenerationInfoList.deleteGenerationInfoListBySourceId(sourceId);

    mProducerInfoList.deleteProducerInfoListBySourceId(sourceId);

    addEvent(EventType::PRODUCER_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::ProducerInfo *info = mProducerInfoList.getProducerInfoById(producerId);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    updateProducerHashNoLock(info->mProducerId);

    producerInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getProducerInfoByName(T::SessionId sessionId,const std::string& producerName,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::ProducerInfo *info = mProducerInfoList.getProducerInfoByName(producerName);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    updateProducerHashNoLock(info->mProducerId);

    producerInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    producerInfoList.clear();

    uint pLen = mProducerInfoList.getLength();

    for (uint p=0; p<pLen; p++)
    {
      T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByIndex(p);
      if (producerInfo)
        updateProducerHashNoLock(producerInfo->mProducerId);
    }

    producerInfoList = mProducerInfoList;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    producerInfoList.clear();

    T::ContentInfoList contentInfoList;

    std::string startTime = "19000101T00000";
    std::string endTime = "23000101T00000";

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

    mContentInfoList[1].getContentInfoListByFmiParameterId(fmiId,0,-1000000000,1000000000,-2,-2,-2,startTime,endTime,0,contentInfoList);

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

        updateProducerHashNoLock(info->mProducerId);

        T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(info->mProducerId);
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





int MemoryImplementation::_getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    producerInfoList.clear();

    uint pLen = mProducerInfoList.getLength();

    for (uint p=0; p<pLen; p++)
    {
      T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByIndex(p);
      if (producerInfo)
        updateProducerHashNoLock(producerInfo->mProducerId);
    }

    mProducerInfoList.getProducerInfoListBySourceId(sourceId,producerInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getProducerInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mProducerInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    list.clear();

    uint pLen = mProducerInfoList.getLength();

    for (uint p=0; p<pLen; p++)
    {
      T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByIndex(p);

      std::set<uint> geometryIdList;

      uint len = mContentInfoList[0].getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *contentInfo = mContentInfoList[0].getContentInfoByIndex(t);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    list.clear();

    AutoReadLock lock(&mModificationLock);

    T::ProducerInfoList producerList = mProducerInfoList;
    std::map<uint,T::ProducerInfo*> producers;
    uint plen = producerList.getLength();
    for (uint t=0; t<plen; t++)
    {
      auto prod = producerList.getProducerInfoByIndex(t);
      producers.insert(std::pair<uint,T::ProducerInfo*>(prod->mProducerId,prod));
    }


    std::set<std::size_t> tmpList;

    uint len = mContentInfoList[0].getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = mContentInfoList[0].getContentInfoByIndex(t);
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

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getProducerParameterListByProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    list.clear();

    AutoReadLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    std::set<std::size_t> tmpList;

    uint len = mContentInfoList[0].getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = mContentInfoList[0].getContentInfoByIndex(t);
      if (contentInfo->mProducerId == producerId)
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
            p += sprintf(p,"%s;%s;%d;%s;%d;;%d;%05d;%d;%d",
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





int MemoryImplementation::_addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(generationInfo.mProducerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoByName(generationInfo.mName);
    if (info != nullptr)
      return Result::GENERATION_NAME_ALREADY_REGISTERED;

    mMaxGenerationId++;
    generationInfo.mGenerationId = mMaxGenerationId;

    mGenerationInfoList.addGenerationInfo(generationInfo.duplicate());

    addEvent(EventType::GENERATION_ADDED,generationInfo.mGenerationId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_setGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoById(generationInfo.mGenerationId);
    if (info == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    T::GenerationInfo *gInfo = mGenerationInfoList.getGenerationInfoByName(generationInfo.mName);
    if (gInfo != nullptr && gInfo != info)
      return Result::GENERATION_NAME_ALREADY_REGISTERED;

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(generationInfo.mProducerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    *info = generationInfo;

    addEvent(EventType::GENERATION_UPDATED,generationInfo.mGenerationId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteGenerationInfoById(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoById(generationId);
    if (info == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGenerationId(info->mGenerationId);

    mFileInfoListByName.deleteFileInfoByGenerationId(info->mGenerationId);
    mFileInfoList.deleteFileInfoByGenerationId(info->mGenerationId);

    mGeometryInfoList.deleteGeometryInfoListByGenerationId(info->mGenerationId);

    mGenerationInfoList.deleteGenerationInfoById(info->mGenerationId);

    addEvent(EventType::GENERATION_DELETED,info->mGenerationId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteGenerationInfoByName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (info == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGenerationId(info->mGenerationId);

    mFileInfoListByName.deleteFileInfoByGenerationId(info->mGenerationId);
    mFileInfoList.deleteFileInfoByGenerationId(info->mGenerationId);

    mGeometryInfoList.deleteGeometryInfoListByGenerationId(info->mGenerationId);

    mGenerationInfoList.deleteGenerationInfoById(info->mGenerationId);

    addEvent(EventType::GENERATION_DELETED,info->mGenerationId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<uint>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGenerationIdList(generationIdList);

    mFileInfoListByName.deleteFileInfoByGenerationIdList(generationIdList);
    mFileInfoList.deleteFileInfoByGenerationIdList(generationIdList);

    mGeometryInfoList.deleteGeometryInfoListByGenerationIdList(generationIdList);

    for (auto it = generationIdList.begin(); it != generationIdList.end(); ++it)
    {
      mGenerationInfoList.deleteGenerationInfoById(*it);
      addEvent(EventType::GENERATION_DELETED,*it,0,0,0);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;


    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByProducerId(producerInfo->mProducerId);

    mFileInfoListByName.deleteFileInfoByProducerId(producerInfo->mProducerId);
    mFileInfoList.deleteFileInfoByProducerId(producerInfo->mProducerId);

    mGeometryInfoList.deleteGeometryInfoListByProducerId(producerInfo->mProducerId);
    mGenerationInfoList.deleteGenerationInfoListByProducerId(producerInfo->mProducerId);

    addEvent(EventType::GENERATION_LIST_DELETED_BY_PRODUCER_ID,producerInfo->mProducerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;


    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByProducerId(producerInfo->mProducerId);

    mFileInfoListByName.deleteFileInfoByProducerId(producerInfo->mProducerId);
    mFileInfoList.deleteFileInfoByProducerId(producerInfo->mProducerId);

    mGeometryInfoList.deleteGeometryInfoListByProducerId(producerInfo->mProducerId);

    mGenerationInfoList.deleteGenerationInfoListByProducerId(producerInfo->mProducerId);

    addEvent(EventType::GENERATION_LIST_DELETED_BY_PRODUCER_ID,producerInfo->mProducerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoBySourceId(sourceId);

    mFileInfoListByName.deleteFileInfoBySourceId(sourceId);
    mFileInfoList.deleteFileInfoBySourceId(sourceId);

    mGeometryInfoList.deleteGeometryInfoListBySourceId(sourceId);
    mGenerationInfoList.deleteGenerationInfoListBySourceId(sourceId);

    addEvent(EventType::GENERATION_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoById(generationId);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    updateProducerHashNoLock(info->mProducerId);

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getGenerationInfoByName(T::SessionId sessionId,const std::string& generationName,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    updateProducerHashNoLock(info->mProducerId);

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    generationInfoList.clear();

    generationInfoList = mGenerationInfoList;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    generationInfoList.clear();

    std::set<uint> idList;

    mContentInfoList[0].getGenerationIdListByGeometryId(geometryId,idList);

    for (auto it=idList.begin(); it!=idList.end(); ++it)
    {
      T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(*it);
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





int MemoryImplementation::_getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    generationInfoList.clear();

    mGenerationInfoList.getGenerationInfoListByProducerId(producerId,generationInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    generationInfoList.clear();

    mGenerationInfoList.getGenerationInfoListByProducerId(producerInfo->mProducerId,generationInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    generationInfoList.clear();

    mGenerationInfoList.getGenerationInfoListBySourceId(sourceId,generationInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;


    T::GenerationInfo *info = mGenerationInfoList.getLastGenerationInfoByProducerIdAndStatus(producerId,generationStatus);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MemoryImplementation::_getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,const std::string& producerName,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo *info = mGenerationInfoList.getLastGenerationInfoByProducerIdAndStatus(producerInfo->mProducerId,generationStatus);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MemoryImplementation::_getGenerationInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mGenerationInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,uchar status)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    generationInfo->mStatus = status;

    addEvent(EventType::GENERATION_STATUS_CHANGED,generationId,status,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_setGenerationInfoStatusByName(T::SessionId sessionId,const std::string& generationName,uchar status)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    generationInfo->mStatus = status;

    addEvent(EventType::GENERATION_STATUS_CHANGED,generationInfo->mGenerationId,status,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_addGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(geometryInfo.mProducerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(geometryInfo.mGenerationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    T::GeometryInfo *info = mGeometryInfoList.getGeometryInfoById(geometryInfo.mGenerationId,geometryInfo.mGeometryId,geometryInfo.mLevelId);
    if (info != nullptr)
      return Result::GEOMETRY_ALREADY_REGISTERED;

    mGeometryInfoList.addGeometryInfo(geometryInfo.duplicate());

    addEvent(EventType::GEOMETRY_ADDED,geometryInfo.mGenerationId,geometryInfo.mGeometryId,geometryInfo.mLevelId,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::GeometryInfo *info = mGeometryInfoList.getGeometryInfoById(generationId,geometryId,levelId);
    if (info == nullptr)
      return Result::UNKNOWN_GEOMETRY;

    mGeometryInfoList.deleteGeometryInfoById(generationId,geometryId,levelId);

    addEvent(EventType::GEOMETRY_DELETED,generationId,geometryId,levelId,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    mGeometryInfoList.deleteGeometryInfoListByGenerationId(generationId);

    addEvent(EventType::GEOMETRY_LIST_DELETED_BY_GENERATION_ID,generationId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    mGeometryInfoList.deleteGeometryInfoListByProducerId(producerId);

    addEvent(EventType::GEOMETRY_LIST_DELETED_BY_PRODUCER_ID,producerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MemoryImplementation::_deleteGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    mGeometryInfoList.deleteGeometryInfoListBySourceId(sourceId);

    addEvent(EventType::GEOMETRY_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MemoryImplementation::_getGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GeometryInfo *info = mGeometryInfoList.getGeometryInfoById(generationId,geometryId,levelId);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    geometryInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MemoryImplementation::_getGeometryInfoList(T::SessionId sessionId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    geometryInfoList.clear();

    geometryInfoList = mGeometryInfoList;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MemoryImplementation::_getGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    geometryInfoList.clear();

    mGeometryInfoList.getGeometryInfoListByGenerationId(generationId,geometryInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    geometryInfoList.clear();

    mGeometryInfoList.getGeometryInfoListByProducerId(producerId,geometryInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MemoryImplementation::_getGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GeometryInfoList& geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    geometryInfoList.clear();

    mGeometryInfoList.getGeometryInfoListBySourceId(sourceId,geometryInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getGeometryInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mGeometryInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_setGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::GeometryInfo *info = mGeometryInfoList.getGeometryInfoById(geometryInfo.mGenerationId,geometryInfo.mGeometryId,geometryInfo.mLevelId);
    if (info == nullptr)
      return Result::UNKNOWN_GEOMETRY;

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(geometryInfo.mProducerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(geometryInfo.mGenerationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    *info = geometryInfo;

    addEvent(EventType::GEOMETRY_UPDATED,geometryInfo.mGenerationId,geometryInfo.mGeometryId,geometryInfo.mLevelId,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_setGeometryInfoStatusById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,uchar status)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::GeometryInfo *info = mGeometryInfoList.getGeometryInfoById(generationId,geometryId,levelId);
    if (info == nullptr)
      return Result::UNKNOWN_GEOMETRY;

    if (info->mStatus != status)
    {
      info->mStatus = status;
      addEvent(EventType::GEOMETRY_STATUS_CHANGED,generationId,geometryId,levelId,status);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MemoryImplementation::_addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(fileInfo.mProducerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo* generationInfo = mGenerationInfoList.getGenerationInfoById(fileInfo.mGenerationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    if (producerInfo->mProducerId != generationInfo->mProducerId)
      return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;

    // ### Checking if the filename already exists in the database.

    T::FileInfo *info = mFileInfoListByName.getFileInfoByName(fileInfo.mName);
    if (info != nullptr)
    {
      // ### File with the same name already exists. Let's return
      // ### the current file-id.

      fileInfo.mFileId = info->mFileId;

      // ### Adding an event to the event list.

      addEvent(EventType::FILE_UPDATED,fileInfo.mFileId,fileInfo.mFileType,0,0);
    }
    else
    {
      // ### Generating a new file-id.

      mMaxFileId++;

      fileInfo.mFileId = mMaxFileId;

      info = fileInfo.duplicate();
      mFileInfoList.addFileInfo(info);
      mFileInfoListByName.addFileInfo(info);

      // ### Adding an event to the event list.

      addEvent(EventType::FILE_ADDED,fileInfo.mFileId,fileInfo.mFileType,0,0);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_setFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::FileInfo *fInfo = mFileInfoList.getFileInfoById(fileInfo.mFileId);
    if (fInfo == nullptr)
      return Result::UNKNOWN_FILE_ID;

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(fileInfo.mProducerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo* generationInfo = mGenerationInfoList.getGenerationInfoById(fileInfo.mGenerationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    if (producerInfo->mProducerId != generationInfo->mProducerId)
      return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;

    // ### Checking if the filename already exists in the database.

    T::FileInfo *info = mFileInfoListByName.getFileInfoByName(fileInfo.mName);
    if (info != nullptr  &&  info->mFileId != fileInfo.mFileId)
      return Result::FILE_NAME_ALREADY_REGISTERED;

    if (fInfo->mName != fileInfo.mName)
    {
      mFileInfoListByName.deleteFileInfoByName(fInfo->mName);
      *fInfo = fileInfo;
      mFileInfoListByName.addFileInfo(fInfo);
    }
    else
    {
      *fInfo = fileInfo;
    }

    addEvent(EventType::FILE_UPDATED,fileInfo.mFileId,fileInfo.mFileType,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(fileInfo.mProducerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo* generationInfo = mGenerationInfoList.getGenerationInfoById(fileInfo.mGenerationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    if (producerInfo->mProducerId != generationInfo->mProducerId)
      return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;


    // ### Checking if the filename already exists in the database.

    T::FileInfo *info = mFileInfoListByName.getFileInfoByName(fileInfo.mName);
    if (info != nullptr)
    {
      // ### File with the same name already exists. Let's return
      // ### the current file-id.

      fileInfo.mFileId = info->mFileId;

      // ### Deleting old content information.

      for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
        mContentInfoList[t].deleteContentInfoByFileId(fileInfo.mFileId);
    }
    else
    {
      // ### Generating a new file-id.

      mMaxFileId++;

      fileInfo.mFileId = mMaxFileId;

      info = fileInfo.duplicate();
      mFileInfoList.addFileInfo(info);
      mFileInfoListByName.addFileInfo(info);
    }


    // ### Adding the content information into the database.

    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      if (info != nullptr)
      {
        T::ContentInfo *cInfo = info->duplicate();
        // ### Making sure that content data matches the file data.

        cInfo->mFileId = fileInfo.mFileId;
        if (cInfo->mFileType == 0)
          cInfo->mFileType = fileInfo.mFileType;
        cInfo->mProducerId = fileInfo.mProducerId;
        cInfo->mGenerationId = fileInfo.mGenerationId;
        cInfo->mFlags = cInfo->mFlags;

        for (int t=0; t<CONTENT_LIST_COUNT; t++)
        {
          mContentInfoList[t].addContentInfo(cInfo);
        }
      }
    }

    // ### Adding an event to the event list.

    if (fileInfo.mFileId != mMaxFileId)
    {
      //printf("-- file update event\n");
      addEvent(EventType::FILE_UPDATED,fileInfo.mFileId,fileInfo.mFileType,0,0);
    }
    else
    {
      addEvent(EventType::FILE_ADDED,fileInfo.mFileId,fileInfo.mFileType,0,0);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::FileInfoList tmpFileList;
    tmpFileList.setReleaseObjects(false);

    T::ContentInfoList tmpContentList;
    tmpContentList.setReleaseObjects(false);

    T::EventInfoList tmpEventList;

    for (auto ff = fileAndContentList.begin(); ff != fileAndContentList.end(); ++ff)
    {
      T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(ff->mFileInfo.mProducerId);
      if (producerInfo == nullptr)
        return Result::UNKNOWN_PRODUCER_ID;

      T::GenerationInfo* generationInfo = mGenerationInfoList.getGenerationInfoById(ff->mFileInfo.mGenerationId);
      if (generationInfo == nullptr)
        return Result::UNKNOWN_GENERATION_ID;

      if (producerInfo->mProducerId != generationInfo->mProducerId)
      {
        return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;
      }


      // ### Checking if the filename already exists in the database.

      T::FileInfo *info = mFileInfoListByName.getFileInfoByName(ff->mFileInfo.mName);
      if (info != nullptr)
      {
        //printf("** File exists %s\n",fileInfo.mName.c_str());
        // ### File with the same name already exists. Let's return
        // ### the current file-id.

        ff->mFileInfo.mFileId = info->mFileId;

        // ### Deleting old content information.

        if (requestFlags & 0x00000001)
          mContentInfoList[0].markDeletedByFileId(ff->mFileInfo.mFileId);
      }
      else
      {
        // ### Generating a new file-id.
        //printf("** File added %s\n",fileInfo.mName.c_str());

        mMaxFileId++;

        ff->mFileInfo.mFileId = mMaxFileId;

        info = ff->mFileInfo.duplicate();

        tmpFileList.addFileInfo(info);
        //mFileInfoList.addFileInfo(info);
        //mFileInfoListByName.addFileInfo(info);
      }


      // ### Adding the content information into the database.

      uint len = ff->mContentInfoList.getLength();
      //printf("-- contentList %u\n",len);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = ff->mContentInfoList.getContentInfoByIndex(t);
        if (info != nullptr)
        {
          //T::ContentInfo *cInfo = info->duplicate();
          // ### Making sure that content data matches the file data.

          info->mFileId = ff->mFileInfo.mFileId;
          if (info->mFileType == 0)
            info->mFileType = ff->mFileInfo.mFileType;

          info->mProducerId = ff->mFileInfo.mProducerId;
          info->mGenerationId = ff->mFileInfo.mGenerationId;
          info->mFlags = info->mFlags;


          //mContentInfoList[0].addContentInfo(cInfo);
          tmpContentList.addContentInfo(info);

          if (info->mFileId != mMaxFileId  &&  (requestFlags & 0x00000001) == 0)
            tmpEventList.addEventInfo(new T::EventInfo(0,0,EventType::CONTENT_ADDED,info->mFileId,info->mMessageIndex,0,0,""));
        }
      }

      // ### Adding an event to the event list.

      if (ff->mFileInfo.mFileId != mMaxFileId)
      {
        //printf("-- file update event\n");
        if (requestFlags & 0x00000001)
          tmpEventList.addEventInfo(new T::EventInfo(0,0,EventType::FILE_UPDATED,ff->mFileInfo.mFileId,ff->mFileInfo.mFileType,0,0,""));
      }
      else
      {
        //printf("-- file add event\n");
        tmpEventList.addEventInfo(new T::EventInfo(0,0,EventType::FILE_ADDED,ff->mFileInfo.mFileId,ff->mFileInfo.mFileType,len,0,""));
      }
    }

    if (requestFlags & 0x00000001)
    {
      for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
        mContentInfoList[t].deleteMarkedContent();
    }

    mFileInfoList.addFileInfoList(tmpFileList);
    mFileInfoListByName.addFileInfoList(tmpFileList);

    for (int t=0; t<CONTENT_LIST_COUNT; t++)
    {
      mContentInfoList[t].addContentInfoList(tmpContentList);
    }

    auto it = tmpEventList.getFirstEvent();
    while (it != nullptr)
    {
      addEvent(it->mType,it->mId1,it->mId2,it->mId3,it->mFlags);
      it = it->nextItem;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MemoryImplementation::_deleteFileInfoById(T::SessionId sessionId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::FileInfo *fileInfo = mFileInfoList.getFileInfoById(fileId);
    if (fileInfo == nullptr)
      return Result::UNKNOWN_FILE_ID;

    auto fType = fileInfo->mFileType;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByFileId(fileInfo->mFileId);

    mFileInfoListByName.deleteFileInfoByName(fileInfo->mName);
    mFileInfoList.deleteFileInfoById(fileId);
    addEvent(EventType::FILE_DELETED,fileId,fType,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoByName(T::SessionId sessionId,const std::string& filename)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::FileInfo *fileInfo = mFileInfoList.getFileInfoByName(filename);
    if (fileInfo == nullptr)
      return Result::UNKNOWN_FILE_NAME;

    T::FileInfo fInfo(*fileInfo);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByFileId(fInfo.mFileId);

    mFileInfoListByName.deleteFileInfoByName(fInfo.mName);
    mFileInfoList.deleteFileInfoById(fInfo.mFileId);

    addEvent(EventType::FILE_DELETED,fInfo.mFileId,fInfo.mFileType,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;


    AutoWriteLock lock(&mModificationLock);


    for (auto fTime = forecastTimeList.begin(); fTime != forecastTimeList.end(); ++fTime)
    {
      auto forecastTimeUTC = utcTimeToTimeT(fTime->mForecastTime);

      T::ContentInfoList contentInfoList;
      T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(fTime->mGenerationId);
      if (generationInfo != nullptr)
      {
        mContentInfoList[0].getContentInfoListByGenerationAndGeometryId(generationInfo->mProducerId,fTime->mGenerationId,fTime->mGeometryId,0,0,1000000,contentInfoList);
        uint len = contentInfoList.getLength();
        for (uint t=0; t<len; t++)
        {
          T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
          if (info->mForecastType == fTime->mForecastType  &&
              info->mForecastNumber == fTime->mForecastNumber &&
              info->mForecastTimeUTC == forecastTimeUTC)
          {
            T::ContentInfo *cInfo = mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(info->mFileId,info->mMessageIndex);
            if (cInfo != nullptr)
              cInfo->mFlags = cInfo->mFlags | T::ContentInfo::Flags::DeletedContent;

            T::FileInfo *fileInfo = mFileInfoList.getFileInfoById(info->mFileId);
            if (fileInfo != nullptr)
            {
              fileInfo->mFlags = fileInfo->mFlags | T::FileInfo::Flags::DeletedFile;
              addEvent(EventType::FILE_DELETED,fileInfo->mFileId,fileInfo->mFileType,0,0);
            }
          }
        }
      }
    }

    mFileInfoListByName.deleteMarkedFiles();
    mFileInfoList.deleteMarkedFiles();

    for (int a=CONTENT_LIST_COUNT-1; a>=0; a--)
      mContentInfoList[a].deleteMarkedContent();

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByProducerId(producerInfo->mProducerId);

    mFileInfoListByName.deleteFileInfoByProducerId(producerInfo->mProducerId);
    mFileInfoList.deleteFileInfoByProducerId(producerInfo->mProducerId);

    addEvent(EventType::FILE_LIST_DELETED_BY_PRODUCER_ID,producerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByProducerId(producerInfo->mProducerId);

    mFileInfoListByName.deleteFileInfoByProducerId(producerInfo->mProducerId);
    mFileInfoList.deleteFileInfoByProducerId(producerInfo->mProducerId);

    addEvent(EventType::FILE_LIST_DELETED_BY_PRODUCER_ID,producerInfo->mProducerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGenerationId(generationInfo->mGenerationId);

    mFileInfoListByName.deleteFileInfoByGenerationId(generationInfo->mGenerationId);
    mFileInfoList.deleteFileInfoByGenerationId(generationInfo->mGenerationId);

    addEvent(EventType::FILE_LIST_DELETED_BY_GENERATION_ID,generationInfo->mGenerationId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,time_t forecastTime)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;


    T::ContentInfoList contentInfoList;
    mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,forecastTime,forecastTime,contentInfoList);

    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      if (info->mGeometryId == geometryId  &&  info->mForecastType == forecastType  &&  info->mForecastNumber == forecastNumber)
      {
        T::ContentInfo *cInfo = mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(info->mFileId,info->mMessageIndex);
        if (cInfo != nullptr)
          cInfo->mFlags = cInfo->mFlags | T::ContentInfo::Flags::DeletedContent;

        T::FileInfo *fileInfo = mFileInfoList.getFileInfoById(info->mFileId);
        if (fileInfo != nullptr)
        {
          fileInfo->mFlags = fileInfo->mFlags | T::FileInfo::Flags::DeletedFile;
          addEvent(EventType::FILE_DELETED,fileInfo->mFileId,fileInfo->mFileType,0,0);
        }
      }
    }

    mFileInfoListByName.deleteMarkedFiles();
    mFileInfoList.deleteMarkedFiles();

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteMarkedContent();

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MemoryImplementation::_deleteFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGenerationId(generationInfo->mGenerationId);

    mFileInfoListByName.deleteFileInfoByGenerationId(generationInfo->mGenerationId);
    mFileInfoList.deleteFileInfoByGenerationId(generationInfo->mGenerationId);

    addEvent(EventType::FILE_LIST_DELETED_BY_GENERATION_ID,generationInfo->mGenerationId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoBySourceId(sourceId);

    mFileInfoListByName.deleteFileInfoBySourceId(sourceId);
    mFileInfoList.deleteFileInfoBySourceId(sourceId);

    addEvent(EventType::FILE_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByFileIdList(fileIdList);

    mFileInfoListByName.deleteFileInfoByFileIdList(fileIdList);
    mFileInfoList.deleteFileInfoByFileIdList(fileIdList);

    for (auto it=fileIdList.begin(); it!=fileIdList.end(); ++it)
    {
      addEvent(EventType::FILE_DELETED,*it,0,0,0);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::FileInfo *info = mFileInfoList.getFileInfoById(fileId);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    fileInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getFileInfoByName(T::SessionId sessionId,const std::string& filename,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::FileInfo *info = mFileInfoListByName.getFileInfoByName(filename);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    fileInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getFileInfoList(T::SessionId sessionId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    fileInfoList.clear();

    mFileInfoList.getFileInfoList(startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    fileInfoList.clear();

    for (auto it = fileIdList.begin(); it != fileIdList.end(); ++it)
    {
      T::FileInfo *info = mFileInfoList.getFileInfoById(*it);
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





int MemoryImplementation::_getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    fileInfoList.clear();

    mFileInfoList.getFileInfoListByProducerId(producerId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    fileInfoList.clear();

    mFileInfoList.getFileInfoListByProducerId(producerInfo->mProducerId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    fileInfoList.clear();

    mFileInfoList.getFileInfoListByGenerationId(generationId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    fileInfoList.clear();

    mFileInfoList.getFileInfoListByGenerationId(generationInfo->mGenerationId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MemoryImplementation::_getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    fileInfoList.clear();

    mFileInfoList.getFileInfoListBySourceId(sourceId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getFileInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mFileInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getFileInfoCountByProducerId(T::SessionId sessionId,uint producerId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mFileInfoList.getFileInfoCountByProducerId(producerId);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getFileInfoCountByGenerationId(T::SessionId sessionId,uint generationId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mFileInfoList.getFileInfoCountByGenerationId(generationId);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getFileInfoCountBySourceId(T::SessionId sessionId,uint sourceId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mFileInfoList.getFileInfoCountBySourceId(sourceId);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    eventInfo.mEventId = addEvent(eventInfo.mType,eventInfo.mId1,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    eventInfo.mServerTime = mStartTime;

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

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





int MemoryImplementation::_getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!mEventsEnabled)
      return Result::DATA_NOT_FOUND;

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
            mContentInfoList[0].getContentInfoListByFileId(fileInfo->mFileId,contentInfoList);

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
        T::ContentInfo *contentInfo = mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(eventInfo->mId1,eventInfo->mId2);

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





int MemoryImplementation::_getEventInfoCount(T::SessionId sessionId,uint& count)
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





int MemoryImplementation::_addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(contentInfo.mProducerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(contentInfo.mGenerationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    T::FileInfo *fileInfo = mFileInfoList.getFileInfoById(contentInfo.mFileId);
    if (fileInfo == nullptr)
      return Result::UNKNOWN_FILE_ID;

    if (producerInfo->mProducerId != generationInfo->mProducerId)
      return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;

    if (producerInfo->mProducerId != fileInfo->mProducerId)
      return Result::PRODUCER_AND_FILE_DO_NOT_MATCH;

    if (generationInfo->mGenerationId != fileInfo->mGenerationId)
      return Result::GENERATION_AND_FILE_DO_NOT_MATCH;


    T::ContentInfo *cInfo = mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(contentInfo.mFileId,contentInfo.mMessageIndex);
    if (cInfo != nullptr)
      return Result::CONTENT_ALREADY_EXISTS;

    cInfo = contentInfo.duplicate();
    for (int t=0; t<CONTENT_LIST_COUNT; t++)
    {
      mContentInfoList[t].addContentInfo(cInfo);
    }

    addEvent(EventType::CONTENT_ADDED,contentInfo.mFileId,contentInfo.mMessageIndex,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_setContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ContentInfo *cInfo = mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(contentInfo.mFileId,contentInfo.mMessageIndex);
    if (cInfo == nullptr)
      return Result::UNKNOWN_CONTENT;

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(contentInfo.mProducerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(contentInfo.mGenerationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    T::FileInfo *fileInfo = mFileInfoList.getFileInfoById(contentInfo.mFileId);
    if (fileInfo == nullptr)
      return Result::UNKNOWN_FILE_ID;

    if (producerInfo->mProducerId != generationInfo->mProducerId)
      return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;

    if (producerInfo->mProducerId != fileInfo->mProducerId)
      return Result::PRODUCER_AND_FILE_DO_NOT_MATCH;

    if (generationInfo->mGenerationId != fileInfo->mGenerationId)
      return Result::GENERATION_AND_FILE_DO_NOT_MATCH;

    for (int t=CONTENT_LIST_COUNT-1; t>0; t--)
      mContentInfoList[t].deleteContentInfoByFileIdAndMessageIndex(contentInfo.mFileId,contentInfo.mMessageIndex);

    for (int t=1; t<CONTENT_LIST_COUNT; t++)
    {
      mContentInfoList[t].addContentInfo(cInfo);
    }

    addEvent(EventType::CONTENT_UPDATED,contentInfo.mFileId,contentInfo.mMessageIndex,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ContentInfoList list;
    list.setReleaseObjects(false);
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      T::ContentInfo contentInfo;
      if (mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(info->mFileId,info->mMessageIndex) != nullptr)
      {
        // Content already added
      }
      else
      {
        T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(info->mProducerId);
        if (producerInfo == nullptr)
          return Result::UNKNOWN_PRODUCER_ID;

        T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(info->mGenerationId);
        if (generationInfo == nullptr)
          return Result::UNKNOWN_GENERATION_ID;

        T::FileInfo *fileInfo = mFileInfoList.getFileInfoById(info->mFileId);
        if (fileInfo == nullptr)
          return Result::UNKNOWN_FILE_ID;

        if (producerInfo->mProducerId != generationInfo->mProducerId)
          return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;

        if (producerInfo->mProducerId != fileInfo->mProducerId)
          return Result::PRODUCER_AND_FILE_DO_NOT_MATCH;

        if (generationInfo->mGenerationId != fileInfo->mGenerationId)
          return Result::GENERATION_AND_FILE_DO_NOT_MATCH;

        //printf("-- add content %u\n",info->mFileId);

        T::ContentInfo *cInfo = info->duplicate();
        mContentInfoList[0].addContentInfo(cInfo);
        addEvent(EventType::CONTENT_ADDED,info->mFileId,info->mMessageIndex,0,0);
        list.addContentInfo(cInfo);
      }
    }

    for (int t=1; t<CONTENT_LIST_COUNT; t++)
    {
      mContentInfoList[t].addContentInfoList(list);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ContentInfo *cInfo = mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(fileId,messageIndex);
    if (cInfo == nullptr)
      return Result::UNKNOWN_CONTENT;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByFileIdAndMessageIndex(fileId,messageIndex);


    addEvent(EventType::CONTENT_DELETED,fileId,messageIndex,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteContentListByFileId(T::SessionId sessionId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::FileInfo *fileInfo = mFileInfoList.getFileInfoById(fileId);
    if (fileInfo == nullptr)
      return Result::UNKNOWN_FILE_ID;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByFileId(fileId);

    addEvent(EventType::CONTENT_LIST_DELETED_BY_FILE_ID,fileId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteContentListByFileName(T::SessionId sessionId,const std::string& filename)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::FileInfo *fileInfo = mFileInfoList.getFileInfoByName(filename);
    if (fileInfo == nullptr)
      return Result::UNKNOWN_FILE_NAME;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByFileId(fileInfo->mFileId);

    addEvent(EventType::CONTENT_LIST_DELETED_BY_FILE_ID,fileInfo->mFileId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteContentListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByProducerId(producerId);

    addEvent(EventType::CONTENT_LIST_DELETED_BY_PRODUCER_ID,producerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteContentListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByProducerId(producerInfo->mProducerId);

    addEvent(EventType::CONTENT_LIST_DELETED_BY_PRODUCER_ID,producerInfo->mProducerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteContentListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGenerationId(generationInfo->mGenerationId);

    addEvent(EventType::CONTENT_LIST_DELETED_BY_GENERATION_ID,generationInfo->mGenerationId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteContentListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGenerationId(generationInfo->mGenerationId);

    addEvent(EventType::CONTENT_LIST_DELETED_BY_GENERATION_ID,generationInfo->mGenerationId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_deleteContentListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoBySourceId(sourceId);

    addEvent(EventType::CONTENT_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::ContentInfo *cInfo = mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(fileId,messageIndex);
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





int MemoryImplementation::_getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    contentInfoList.clear();

    mContentInfoList[0].getContentInfoList(startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    contentInfoList.clear();

    mContentInfoList[0].getContentInfoListByFileId(fileId,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    contentInfoList.clear();

    for (auto it = fileIdList.begin(); it != fileIdList.end(); ++it)
    {
      T::ContentInfoList contentList;
      mContentInfoList[0].getContentInfoListByFileId(*it,contentList);
      contentInfoList.addContentInfoList(contentList);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListByFileName(T::SessionId sessionId,const std::string& filename,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    contentInfoList.clear();

    T::FileInfo *fileInfo = mFileInfoListByName.getFileInfoByName(filename);
    if (fileInfo == nullptr)
      return Result::UNKNOWN_FILE_NAME;

    mContentInfoList[0].getContentInfoListByFileId(fileInfo->mFileId,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    contentInfoList.clear();

    mContentInfoList[0].getContentInfoListByProducerId(producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    contentInfoList.clear();

    mContentInfoList[0].getContentInfoListByProducerId(producerInfo->mProducerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,int maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    contentInfoList.clear();

    mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    contentInfoList.clear();

    mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    contentInfoList.clear();

    mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startTime,endTime,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,time_t startTime,time_t endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    contentInfoList.clear();

    mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,startTime,endTime,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    contentInfoList.clear();

    mContentInfoList[0].getContentInfoListBySourceId(sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

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

    mContentInfoList[1].getContentInfoListByFmiParameterId(fmiId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
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

    mContentInfoList[1].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,fmiId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
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

    mContentInfoList[1].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,fmiId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

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


    mContentInfoList[1].getContentInfoListByFmiParameterIdAndProducerId(producerInfo->mProducerId,fmiId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
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

    T::ContentInfo *cInfo = mContentInfoList[1].getContentInfoByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,fmiId,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime);
    if (cInfo != nullptr)
    {
      contentInfoList.addContentInfo(cInfo->duplicate());
      return Result::OK;
    }
    mContentInfoList[1].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,fmiId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
    contentInfoList = contentList;
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTime,time_t endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
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

    mContentInfoList[1].getContentInfoListByFmiParameterIdAndProducerId(producerInfo->mProducerId,fmiId,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    contentInfoList.clear();

    uint cLen = mContentInfoList[0].getLength();
    for (uint c=0; c<cLen; c++)
    {
      T::ContentInfo *cInfo = mContentInfoList[0].getContentInfoByIndex(c);
      T::ContentInfo *cError = nullptr;
      if (cInfo != nullptr)
      {
        T::FileInfo *fileInfo = mFileInfoList.getFileInfoById(cInfo->mFileId);
        if (fileInfo == nullptr)
        {
          printf("**** INTEGRITY ERROR : File missing (%u)! *****\n",cInfo->mFileId);
          cError = cInfo;
        }

        if (cError == nullptr)
        {
          T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(cInfo->mGenerationId);
          if (generationInfo == nullptr)
          {
            printf("**** INTEGRITY ERROR : Generation missing (%u)! *****\n",cInfo->mGenerationId);
            cError = cInfo;
          }
        }

        if (cError == nullptr)
        {
          T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(cInfo->mProducerId);
          if (producerInfo == nullptr)
          {
            printf("**** INTEGRITY ERROR : Producer missing (%u)! *****\n",cInfo->mProducerId);
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





int MemoryImplementation::_getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    geometryIdList.clear();

    mContentInfoList[0].getContentGeometryIdListByGenerationId(generationInfo->mProducerId,generationId,geometryIdList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    contentParamList.clear();

    T::ContentInfoList contentInfoList;
    mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,0,0,1000000,contentInfoList);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentParamKeyListByGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    paramKeyList.clear();

    mContentInfoList[0].getContentParamKeyListByGenerationId(generationInfo->mProducerId,generationId,parameterKeyType,paramKeyList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentParamKeyListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    paramKeyList.clear();

    mContentInfoList[0].getContentParamKeyListByGenerationAndGeometryId(generationInfo->mProducerId,generationId,geometryId,parameterKeyType,paramKeyList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentParamKeyListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    paramKeyList.clear();

    mContentInfoList[0].getContentParamKeyListByGenerationGeometryAndLevelId(generationInfo->mProducerId,generationId,geometryId,levelId,parameterKeyType,paramKeyList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    contentTimeList.clear();

    mContentInfoList[0].getForecastTimeListByGenerationId(generationInfo->mProducerId,generationId,contentTimeList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentTimeRangeByProducerAndGenerationId(T::SessionId sessionId,uint producerId,uint generationId,time_t& startTime,time_t& endTime)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    if (generationInfo->mContentStartTime == 0)
    {
      mContentInfoList[1].getForecastTimeRangeByGenerationId(producerId,generationId,startTime,endTime,generationInfo->mContentHash);
      generationInfo->mContentStartTime = startTime;
      generationInfo->mContentEndTime = endTime;
    }
    else
    {
      startTime = generationInfo->mContentStartTime;
      endTime = generationInfo->mContentEndTime;
    }

    //mContentInfoList[0].getForecastTimeRangeByGenerationId(producerId,generationId,startTime,endTime);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentTimeRangeByGenerationId(T::SessionId sessionId,uint generationId,time_t& startTime,time_t& endTime)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    mContentInfoList[0].getForecastTimeRangeByGenerationId(generationInfo->mProducerId,generationId,startTime,endTime);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    contentTimeList.clear();

    mContentInfoList[0].getForecastTimeListByGenerationAndGeometry(generationInfo->mProducerId,generationId,geometryId,contentTimeList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentLevelListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    contentLevelList.clear();

    mContentInfoList[0].getContentLevelListByGenerationGeometryAndLevelId(generationInfo->mProducerId,generationId,geometryId,levelId,contentLevelList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int MemoryImplementation::_getContentTimeListByGenerationGeometryAndLevelId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    contentTimeList.clear();

    mContentInfoList[0].getForecastTimeListByGenerationGeometryAndLevelId(generationInfo->mProducerId,generationId,geometryId,levelId,contentTimeList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentTimeListByProducerId(T::SessionId sessionId,uint producerId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    contentTimeList.clear();

    mContentInfoList[0].getForecastTimeListByProducerId(producerId,contentTimeList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock);

    list.clear();

    uint len = mContentInfoList[0].getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = mContentInfoList[0].getContentInfoByIndex(t);
      char st[200];
      sprintf(st,"%u;%u;%u;%d;%d;%s;%ld;%ld;",info->mSourceId,info->mGenerationId,info->mGeometryId,info->mForecastType,info->mForecastNumber,info->getForecastTime(),info->mModificationTime,info->mDeletionTime);
      std::string str = st;


      if (list.find(str) == list.end())
        list.insert(str);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getContentCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mContentInfoList[0].getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getHashByProducerId(T::SessionId sessionId,uint producerId,ulonglong& hash)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    hash = updateProducerHashNoLock(producerId);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ulonglong MemoryImplementation::updateProducerHashNoLock(uint producerId)
{
  FUNCTION_TRACE
  try
  {
    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return 0;

    std::size_t generationHash = mGenerationInfoList.getHashByProducerId(producerId);
    std::size_t geometryHash = mGeometryInfoList.getHashByProducerId(producerId);
    std::size_t fileHash = mFileInfoList.getHashByProducerId(producerId);
    std::size_t contentHash = mContentInfoList[0].getHashByProducerId(producerId);

    std::size_t h = 0;
    boost::hash_combine(h,generationHash);
    boost::hash_combine(h,geometryHash);
    boost::hash_combine(h,fileHash);
    boost::hash_combine(h,contentHash);

    producerInfo->mHash = h;
    return h;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int MemoryImplementation::_getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    levelInfoList.clear();

    mContentInfoList[0].getLevelInfoList(levelInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





T::EventId MemoryImplementation::addEvent(uint eventType,uint id1,uint id2,uint id3,unsigned long long flags)
{
  FUNCTION_TRACE
  try
  {
    saveData();

    if (!mEventsEnabled)
      return 0;

    mMaxEventId++;
    T::EventId eventId = mMaxEventId;

    T::EventInfo *eventInfo = new T::EventInfo(mStartTime,eventId,eventType,id1,id2,id3,flags,"");
    mEventInfoList.addEventInfo(eventInfo);

    return eventId;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void MemoryImplementation::readProducerList()
{
  FUNCTION_TRACE
  try
  {
    mProducerInfoList.clear();

    if (!mContentLoadEnabled)
      return;

    char filename[200];

    sprintf(filename,"%s/producers.csv",mContentDir.c_str());
    FILE *file = fopen(filename,"re");
    if (file == nullptr)
      return;

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st,1000,file) != nullptr  &&  st[0] != '#')
      {
        char *p = strstr(st,"\n");
        if (p != nullptr)
          *p = '\0';

        T::ProducerInfo *rec = new T::ProducerInfo();
        rec->setCsv(st);

        if (rec->mProducerId > mMaxProducerId)
          mMaxProducerId = rec->mProducerId;

        mProducerInfoList.addProducerInfo(rec);

      }
    }
    fclose(file);

    mProducerStorage_modificationTime = getFileModificationTime(filename);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MemoryImplementation::readGenerationList()
{
  FUNCTION_TRACE
  try
  {
    mGenerationInfoList.clear();

    if (!mContentLoadEnabled)
      return;

    char filename[200];

    sprintf(filename,"%s/generations.csv",mContentDir.c_str());
    FILE *file = fopen(filename,"re");
    if (file == nullptr)
      return;

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st,1000,file) != nullptr  &&  st[0] != '#')
      {
        char *p = strstr(st,"\n");
        if (p != nullptr)
          *p = '\0';

        T::GenerationInfo *rec = new T::GenerationInfo();
        rec->setCsv(st);

        if (rec->mGenerationId > mMaxGenerationId)
          mMaxGenerationId = rec->mGenerationId;

        mGenerationInfoList.addGenerationInfo(rec);
      }
    }
    fclose(file);
    mGenerationStorage_modificationTime = getFileModificationTime(filename);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MemoryImplementation::readGeometryList()
{
  FUNCTION_TRACE
  try
  {
    mGenerationInfoList.clear();

    if (!mContentLoadEnabled)
      return;

    char filename[200];

    sprintf(filename,"%s/geometries.csv",mContentDir.c_str());
    FILE *file = fopen(filename,"re");
    if (file == nullptr)
      return;

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st,1000,file) != nullptr  &&  st[0] != '#')
      {
        char *p = strstr(st,"\n");
        if (p != nullptr)
          *p = '\0';

        T::GeometryInfo *rec = new T::GeometryInfo();
        rec->setCsv(st);

        mGeometryInfoList.addGeometryInfo(rec);
      }
    }
    fclose(file);
    mGeometryStorage_modificationTime = getFileModificationTime(filename);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MemoryImplementation::readFileList()
{
  FUNCTION_TRACE
  try
  {
    mFileInfoList.clear();

    if (!mContentLoadEnabled)
      return;

    char filename[200];

    sprintf(filename,"%s/files.csv",mContentDir.c_str());
    FILE *file = fopen(filename,"re");
    if (file == nullptr)
      return;

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st,1000,file) != nullptr  &&  st[0] != '#')
      {
        char *p = strstr(st,"\n");
        if (p != nullptr)
          *p = '\0';

        T::FileInfo *rec = new T::FileInfo();
        rec->setCsv(st);

        if (rec->mFileId > mMaxFileId)
          mMaxFileId = rec->mFileId;

        mFileInfoList.addFileInfo(rec);
      }
    }
    fclose(file);
    mFileStorage_modificationTime = getFileModificationTime(filename);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MemoryImplementation::readContentList()
{
  FUNCTION_TRACE
  try
  {
    mContentInfoList[0].clear();

    if (!mContentLoadEnabled)
      return;

    char filename[200];

    sprintf(filename,"%s/content.csv",mContentDir.c_str());
    FILE *file = fopen(filename,"re");
    if (file == nullptr)
      return;

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st,1000,file) != nullptr  &&  st[0] != '#')
      {
        char *p = strstr(st,"\n");
        if (p != nullptr)
          *p = '\0';

        T::ContentInfo *rec = new T::ContentInfo();
        rec->setCsv(st);
        mContentInfoList[0].addContentInfo(rec);
      }
    }
    fclose(file);
    mContentStorage_modificationTime = getFileModificationTime(filename);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void MemoryImplementation::saveData()
{
  FUNCTION_TRACE
  try
  {
    if (mContentSaveEnabled)
    {
      if ((time(nullptr)-mLastSaveTime) >= mContentSaveInterval)
      {
        mLastSaveTime = time(nullptr);

        if (mProducerCount != mProducerInfoList.getLength())
          mProducerInfoList.writeToFile(mContentDir + "/producers.csv");

        if (mGenerationCount != mGenerationInfoList.getLength())
        {
          std::string filename = mContentDir + "/generations.csv";
          mGenerationInfoList.writeToFile(filename);
        }

        if (mGeometryCount != mGeometryInfoList.getLength())
        {
          std::string filename = mContentDir + "/geometries.csv";
          mGeometryInfoList.writeToFile(filename);
        }

        if (mFileCount != mFileInfoList.getLength())
          mFileInfoList.writeToFile(mContentDir + "/files.csv");

        if (mContentCount != mContentInfoList[0].getLength())
          mContentInfoList[0].writeToFile(mContentDir + "/content.csv");

        mProducerCount = mProducerInfoList.getLength();
        mGenerationCount = mGenerationInfoList.getLength();
        mGeometryCount = mGeometryInfoList.getLength();
        mFileCount = mFileInfoList.getLength();
        mContentCount = mContentInfoList[0].getLength();
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool MemoryImplementation::syncProducerList()
{
  FUNCTION_TRACE
  try
  {
    if (!mContentSyncEnabled)
      return true;

    AutoWriteLock lock(&mModificationLock);

    uint len = mProducerInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ProducerInfo *info = mProducerInfoList.getProducerInfoByIndex(t);
      info->mFlags = info->mFlags & 0x7FFFFFFF;
    }

    char filename[200];
    sprintf(filename,"%s/producers.csv",mContentDir.c_str());
    FILE *file = fopen(filename,"re");
    if (file == nullptr)
      return true;

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st,1000,file) != nullptr  &&  st[0] != '#')
      {
        char *p = strstr(st,"\n");
        if (p != nullptr)
          *p = '\0';

        T::ProducerInfo *rec = new T::ProducerInfo();
        rec->setCsv(st);

        if (rec->mProducerId > mMaxProducerId)
          mMaxProducerId = rec->mProducerId;

        T::ProducerInfo *info = mProducerInfoList.getProducerInfoById(rec->mProducerId);
        if (info != nullptr)
        {
          info->mFlags = info->mFlags | 0x80000000;
          delete rec;
        }
        else
        {
          rec->mFlags = rec->mFlags | 0x80000000;
          mProducerInfoList.addProducerInfo(rec);
          addEvent(EventType::PRODUCER_ADDED,rec->mProducerId,0,0,0);
        }
      }
    }
    fclose(file);

    std::vector<uint> idList;
    mProducerInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ProducerInfo *info = mProducerInfoList.getProducerInfoByIndex(t);
      if ((info->mFlags &  0x80000000) == 0)
      {
        // The producer is not in the csv-file anymore. We should delete it.
        idList.emplace_back(info->mProducerId);
      }
      info->mFlags = info->mFlags & 0x7FFFFFFF;
    }

    int count = 0;
    for (auto it = idList.begin(); it != idList.end(); ++it)
    {
      mProducerInfoList.deleteProducerInfoById(*it);
      mGenerationInfoList.deleteGenerationInfoListByProducerId(*it);
      mFileInfoListByName.deleteFileInfoByProducerId(*it);
      mFileInfoList.deleteFileInfoByProducerId(*it);
      count += mContentInfoList[0].markDeletedByProducerId(*it);
      addEvent(EventType::PRODUCER_DELETED,*it,0,0,0);
    }

    if (count > 0)
    {
      for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      {
        mContentInfoList[t].deleteMarkedContent();
      }
    }

    mProducerStorage_modificationTime = getFileModificationTime(filename);
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool MemoryImplementation::syncGenerationList()
{
  FUNCTION_TRACE
  try
  {
    if (!mContentSyncEnabled)
      return true;

    AutoWriteLock lock(&mModificationLock);

    uint len = mGenerationInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoByIndex(t);
      info->mFlags = info->mFlags & 0x7FFFFFFF;
    }

    char filename[200];
    sprintf(filename,"%s/generations.csv",mContentDir.c_str());
    FILE *file = fopen(filename,"re");
    if (file == nullptr)
      return true;

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st,1000,file) != nullptr  &&  st[0] != '#')
      {
        char *p = strstr(st,"\n");
        if (p != nullptr)
          *p = '\0';

        T::GenerationInfo *rec = new T::GenerationInfo();
        rec->setCsv(st);

        if (rec->mGenerationId > mMaxGenerationId)
          mMaxGenerationId = rec->mGenerationId;

        T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoById(rec->mGenerationId);
        if (info != nullptr)
        {
          info->mFlags = info->mFlags | 0x80000000;
          delete rec;
        }
        else
        {
          T::ProducerInfo *pInfo = mProducerInfoList.getProducerInfoById(rec->mProducerId);
          if (pInfo != nullptr)
          {
            rec->mFlags = rec->mFlags | 0x80000000;
            mGenerationInfoList.addGenerationInfo(rec);
            addEvent(EventType::GENERATION_ADDED,rec->mGenerationId,0,0,0);
          }
          else
          {
            // Invalid producer
            delete rec;
          }
        }
      }
    }
    fclose(file);

    std::vector<uint> idList;
    mGenerationInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoByIndex(t);
      if ((info->mFlags &  0x80000000) == 0)
      {
        // The generation is not in the csv-file anymore. We should delete it.
        idList.emplace_back(info->mGenerationId);
      }
      info->mFlags = info->mFlags & 0x7FFFFFFF;
    }

    int count = 0;
    for (auto it = idList.begin(); it != idList.end(); ++it)
    {
      mGenerationInfoList.deleteGenerationInfoById(*it);
      mFileInfoListByName.deleteFileInfoByGenerationId(*it);
      mFileInfoList.deleteFileInfoByGenerationId(*it);
      count += mContentInfoList[0].markDeletedByGenerationId(*it);
      addEvent(EventType::GENERATION_DELETED,*it,0,0,0);
    }

    if (count > 0)
    {
      for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      {
        mContentInfoList[t].deleteMarkedContent();
      }
    }

    mGenerationStorage_modificationTime = getFileModificationTime(filename);
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool MemoryImplementation::syncGeometryList()
{
  FUNCTION_TRACE
  try
  {
    if (!mContentSyncEnabled)
      return true;

    AutoWriteLock lock(&mModificationLock);

    uint len = mGeometryInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::GeometryInfo *info = mGeometryInfoList.getGeometryInfoByIndex(t);
      info->mFlags = info->mFlags & 0x7FFFFFFF;
    }

    char filename[200];
    sprintf(filename,"%s/geometries.csv",mContentDir.c_str());
    FILE *file = fopen(filename,"re");
    if (file == nullptr)
      return true;

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st,1000,file) != nullptr  &&  st[0] != '#')
      {
        char *p = strstr(st,"\n");
        if (p != nullptr)
          *p = '\0';

        T::GeometryInfo *rec = new T::GeometryInfo();
        rec->setCsv(st);

        T::GeometryInfo *info = mGeometryInfoList.getGeometryInfoById(rec->mGenerationId,rec->mGeometryId,rec->mLevelId);
        if (info != nullptr)
        {
          info->mFlags = info->mFlags | 0x80000000;
          delete rec;
        }
        else
        {
          T::GenerationInfo *gInfo = mGenerationInfoList.getGenerationInfoById(rec->mGenerationId);
          if (gInfo != nullptr)
          {
            rec->mFlags = rec->mFlags | 0x80000000;
            mGeometryInfoList.addGeometryInfo(rec);
            addEvent(EventType::GEOMETRY_ADDED,rec->mGenerationId,rec->mGeometryId,0,0);
          }
          else
          {
            // Invalid producer
            delete rec;
          }
        }
      }
    }
    fclose(file);

    std::vector<T::GeometryInfo> idList;
    mGeometryInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::GeometryInfo *info = mGeometryInfoList.getGeometryInfoByIndex(t);
      if ((info->mFlags &  0x80000000) == 0)
      {
        // The generation is not in the csv-file anymore. We should delete it.
        idList.emplace_back(*info);
      }
      info->mFlags = info->mFlags & 0x7FFFFFFF;
    }

    for (auto it = idList.begin(); it != idList.end(); ++it)
    {
      mGeometryInfoList.deleteGeometryInfoById(it->mGenerationId,it->mGeometryId,it->mLevelId);
      addEvent(EventType::GEOMETRY_DELETED,it->mGenerationId,it->mGeometryId,it->mLevelId,0);
    }

    mGeometryStorage_modificationTime = getFileModificationTime(filename);
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool MemoryImplementation::syncFileList()
{
  FUNCTION_TRACE
  try
  {
    if (!mContentSyncEnabled)
      return true;

    AutoWriteLock lock(&mModificationLock);

    T::EventId startEventId = mMaxEventId;
    uint maxLen = 1000000000;
    if (mEventsEnabled)
      maxLen = 3*mEventInfoList.getMaxLength() / 4;

    uint len = mFileInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::FileInfo *info = mFileInfoList.getFileInfoByIndex(t);
      info->mFlags = info->mFlags & 0x7FFFFFFF;
    }

    char filename[200];
    sprintf(filename,"%s/files.csv",mContentDir.c_str());
    FILE *file = fopen(filename,"re");
    if (file == nullptr)
      return true;

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st,1000,file) != nullptr  &&  st[0] != '#')
      {
        char *p = strstr(st,"\n");
        if (p != nullptr)
          *p = '\0';

        T::FileInfo *rec = new T::FileInfo();
        rec->setCsv(st);

        if (rec->mFileId > mMaxFileId)
          mMaxFileId = rec->mFileId;

        T::FileInfo *info = mFileInfoList.getFileInfoById(rec->mFileId);
        if (info != nullptr)
        {
          info->mFlags = info->mFlags | 0x80000000;
          delete rec;
        }
        else
        {
          T::GenerationInfo *gInfo = mGenerationInfoList.getGenerationInfoById(rec->mGenerationId);
          if (gInfo != nullptr)
          {
            rec->mFlags = rec->mFlags | 0x80000000;
            mFileInfoList.addFileInfo(rec);
            mFileInfoListByName.addFileInfo(rec);
            addEvent(EventType::FILE_ADDED,rec->mFileId,rec->mFileType,0,0);

            if ((mMaxEventId - startEventId) > maxLen)
              return false;
          }
          else
          {
            // Invalid generation

            delete rec;
          }
        }
      }
    }
    fclose(file);

    std::vector<uint> idList;
    std::vector<std::string> nameList;
    mFileInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::FileInfo *info = mFileInfoList.getFileInfoByIndex(t);
      if ((info->mFlags &  0x80000000) == 0)
      {
        // The file is not in the csv-file anymore. We should delete it.
        idList.emplace_back(info->mFileId);
        nameList.emplace_back(info->mName);
      }
      info->mFlags = info->mFlags & 0x7FFFFFFF;
    }

    for (auto name = nameList.begin(); name != nameList.end(); ++name)
      mFileInfoListByName.deleteFileInfoByName(*name);

    int count = 0;
    for (auto it = idList.begin(); it != idList.end(); ++it)
    {
      mFileInfoList.deleteFileInfoById(*it);
      count += mContentInfoList[0].markDeletedByFileId(*it);
      addEvent(EventType::FILE_DELETED,*it,0,0,0);
      if ((mMaxEventId - startEventId) > maxLen)
      {
        if (count > 0)
        {
          for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
          {
            mContentInfoList[t].deleteMarkedContent();
          }
        }
        return false;
      }
    }

    if (count > 0)
    {
      for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      {
        mContentInfoList[t].deleteMarkedContent();
      }
    }

    mFileStorage_modificationTime = getFileModificationTime(filename);
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool MemoryImplementation::syncContentList()
{
  FUNCTION_TRACE
  try
  {
    if (!mContentSyncEnabled)
      return true;

    AutoWriteLock lock(&mModificationLock);

    T::EventId startEventId = mMaxEventId;
    uint maxLen = 1000000000;
    if (mEventsEnabled)
      maxLen = 3*mEventInfoList.getMaxLength() / 4;

    uint len = mContentInfoList[0].getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = mContentInfoList[0].getContentInfoByIndex(t);
      info->mFlags = info->mFlags & 0x7FFFFFFF;
    }

    T::ContentInfoList contentToAdd;
    contentToAdd.setReleaseObjects(false);

    char filename[200];
    sprintf(filename,"%s/content.csv",mContentDir.c_str());
    FILE *file = fopen(filename,"re");
    if (file == nullptr)
      return true;

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st,1000,file) != nullptr  &&  st[0] != '#')
      {
        char *p = strstr(st,"\n");
        if (p != nullptr)
          *p = '\0';

        T::ContentInfo *rec = new T::ContentInfo();
        rec->setCsv(st);

        T::ContentInfo *info = mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(rec->mFileId,rec->mMessageIndex);
        if (info != nullptr)
        {
          info->mFlags = info->mFlags | 0x80000000;
          delete rec;
        }
        else
        {
          T::FileInfo *fInfo = mFileInfoList.getFileInfoById(rec->mFileId);
          if (fInfo != nullptr)
          {
            rec->mFlags = rec->mFlags | 0x80000000;
            contentToAdd.addContentInfo(rec);
            mContentInfoList[0].addContentInfo(rec);
            addEvent(EventType::CONTENT_ADDED,rec->mFileId,rec->mMessageIndex,0,0);
            if ((mMaxEventId - startEventId) > maxLen)
              return false;
          }
          else
          {
            // Invalid file
            delete rec;
          }
        }
      }
    }
    fclose(file);

    for (int t=1; t<CONTENT_LIST_COUNT; t++)
    {
      mContentInfoList[t].addContentInfoList(contentToAdd);
    }

    std::vector<uint> idList;
    mContentInfoList[0].getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = mContentInfoList[0].getContentInfoByIndex(t);
      if ((info->mFlags &  0x80000000) == 0)
      {
        // The generation is not in the csv-file anymore. We should delete it.
        info->mFlags = info->mFlags | T::ContentInfo::Flags::DeletedContent;
        addEvent(EventType::CONTENT_DELETED,info->mFileId,info->mMessageIndex,0,0);
        if ((mMaxEventId - startEventId) > maxLen)
        {
          info->mFlags = info->mFlags & 0x7FFFFFFF;
          for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
          {
            mContentInfoList[t].deleteMarkedContent();
          }
          return false;
        }
      }
      info->mFlags = info->mFlags & 0x7FFFFFFF;
    }

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
    {
      mContentInfoList[t].deleteMarkedContent();
    }
    mContentStorage_modificationTime = getFileModificationTime(filename);
    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




}
}
