#include "MemoryImplementation.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/common/ShowFunction.h>
#include <grid-files/common/GeneralFunctions.h>
#include <boost/functional/hash.hpp>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF



namespace SmartMet
{
namespace ContentServer
{


MemoryImplementation::MemoryImplementation()
{
  FUNCTION_TRACE
  try
  {
    mImplementationType = Implementation::Memory;
    mStartTime = time(nullptr);

    mContentLoadEnabled = false;
    mContentSaveEnabled = false;
    mContentSaveInterval = 60;
    mContentSortingFlags = 0;

    mMaxProducerId = 0;
    mMaxGenerationId = 0;
    mMaxFileId = 0;
    mMaxEventId = 0;

    mDataServerCount = 1000000000;
    mProducerCount = 1000000000;
    mGenerationCount = 1000000000;
    mFileCount = 1000000000;
    mContentCount = 1000000000;

    mShutdownRequested = false;
    mUpdateInProgress = false;
    mReloadActivated = false;
    mLastSaveTime = time(nullptr);

    mFileInfoList.setComparisonMethod(T::FileInfo::ComparisonMethod::none);

    mFileInfoListByName.setReleaseObjects(false);
    mFileInfoListByName.setComparisonMethod(T::FileInfo::ComparisonMethod::none);

    mContentInfoList[0].setComparisonMethod(T::ContentInfo::ComparisonMethod::file_message);
    mContentInfoListEnabled[0] = true;

    for (int t=1; t<CONTENT_LIST_COUNT; t++)
    {
      mContentInfoList[t].setReleaseObjects(false);
      mContentInfoListEnabled[t] = true;
    }


  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





MemoryImplementation::~MemoryImplementation()
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





void MemoryImplementation::init(bool contentLoadEnabled,bool contentSaveEnabled,std::string contentDir,uint contentSaveInterval,uint contentSortingFlags)
{
  FUNCTION_TRACE
  try
  {
    mContentLoadEnabled = contentLoadEnabled;
    mContentSaveEnabled = contentSaveEnabled;
    mContentDir = contentDir;
    mContentSaveInterval = contentSaveInterval;
    mContentSortingFlags = contentSortingFlags;

    readDataServerList();
    readProducerList();
    readGenerationList();
    readFileList();

    mFileInfoListByName = mFileInfoList;

    mFileInfoList.sort(T::FileInfo::ComparisonMethod::fileId);
    mFileInfoListByName.sort(T::FileInfo::ComparisonMethod::fileName);

    mContentInfoListEnabled[0] = true;

    readContentList();

    for (int t=1; t<CONTENT_LIST_COUNT; t++)
    {
      if ((mContentSortingFlags & (1 << t)) != 0)
        mContentInfoListEnabled[t] = true;
      else
        mContentInfoListEnabled[t] = false;

      if (mContentInfoListEnabled[t])
        mContentInfoList[t] = mContentInfoList[0];
    }

    mContentInfoList[0].sort(T::ContentInfo::ComparisonMethod::file_message);

    if (mContentInfoListEnabled[1])
      mContentInfoList[1].sort(T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time);

    if (mContentInfoListEnabled[2])
      mContentInfoList[2].sort(T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time);

    if (mContentInfoListEnabled[3])
      mContentInfoList[3].sort(T::ContentInfo::ComparisonMethod::gribId_producer_generation_level_time);

    if (mContentInfoListEnabled[4])
      mContentInfoList[4].sort(T::ContentInfo::ComparisonMethod::newbaseId_producer_generation_level_time);

    if (mContentInfoListEnabled[5])
      mContentInfoList[5].sort(T::ContentInfo::ComparisonMethod::newbaseName_producer_generation_level_time);

    if (mContentInfoListEnabled[6])
      mContentInfoList[6].sort(T::ContentInfo::ComparisonMethod::cdmId_producer_generation_level_time);

    if (mContentInfoListEnabled[7])
      mContentInfoList[7].sort(T::ContentInfo::ComparisonMethod::cdmName_producer_generation_level_time);

    mUpdateInProgress = false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}






void MemoryImplementation::shutdown()
{
  FUNCTION_TRACE
  try
  {
    //exit(0);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_clear(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    mFileInfoListByName.clear();
    mFileInfoList.clear();
    mProducerInfoList.clear();
    mGenerationInfoList.clear();
    mDataServerInfoList.clear();
    mEventInfoList.clear();

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].clear();

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_reload(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    mFileInfoListByName.clear();
    mFileInfoList.clear();
    mProducerInfoList.clear();
    mGenerationInfoList.clear();
    mDataServerInfoList.clear();
    mEventInfoList.clear();

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].clear();

    readDataServerList();
    readProducerList();
    readGenerationList();
    readFileList();

    mFileInfoListByName = mFileInfoList;

    mFileInfoList.sort(T::FileInfo::ComparisonMethod::fileId);
    mFileInfoListByName.sort(T::FileInfo::ComparisonMethod::fileName);

    //mContentInfoListEnabled[0] = true;

    readContentList();

    for (int t=1; t<CONTENT_LIST_COUNT; t++)
    {
      //if ((mContentSortingFlags & (1 << t)) != 0)
      //  mContentInfoListEnabled[t] = true;
      //else
      //  mContentInfoListEnabled[t] = false;

      if (mContentInfoListEnabled[t])
        mContentInfoList[t] = mContentInfoList[0];
    }

    mContentInfoList[0].sort(T::ContentInfo::ComparisonMethod::file_message);

    if (mContentInfoListEnabled[1])
      mContentInfoList[1].sort(T::ContentInfo::ComparisonMethod::fmiId_producer_generation_level_time);

    if (mContentInfoListEnabled[2])
      mContentInfoList[2].sort(T::ContentInfo::ComparisonMethod::fmiName_producer_generation_level_time);

    if (mContentInfoListEnabled[3])
      mContentInfoList[3].sort(T::ContentInfo::ComparisonMethod::gribId_producer_generation_level_time);

    if (mContentInfoListEnabled[4])
      mContentInfoList[4].sort(T::ContentInfo::ComparisonMethod::newbaseId_producer_generation_level_time);

    if (mContentInfoListEnabled[5])
      mContentInfoList[5].sort(T::ContentInfo::ComparisonMethod::newbaseName_producer_generation_level_time);

    if (mContentInfoListEnabled[6])
      mContentInfoList[6].sort(T::ContentInfo::ComparisonMethod::cdmId_producer_generation_level_time);

    if (mContentInfoListEnabled[7])
      mContentInfoList[7].sort(T::ContentInfo::ComparisonMethod::cdmName_producer_generation_level_time);

    mUpdateInProgress = false;

   return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_addDataServerInfo(T::SessionId sessionId,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    if (serverInfo.mServerId < 1  ||  serverInfo.mServerId > 64)
      return Result::INVALID_SERVER_ID;

    T::ServerInfo *info = mDataServerInfoList.getServerInfoById(serverInfo.mServerId);
    if (info != nullptr)
      return Result::SERVER_ID_ALREADY_REGISTERED;

    info = mDataServerInfoList.getServerInfoByName(serverInfo.mName);
    if (info != nullptr)
      return Result::SERVER_NAME_ALREADY_REGISTERED;

    if (serverInfo.mServerIor.length() > 0)
    {
      info = mDataServerInfoList.getServerInfoByIor(serverInfo.mServerIor);
      if (info != nullptr)
        return Result::SERVER_IOR_ALREADY_REGISTERED;
    }

    mDataServerInfoList.addServerInfo(serverInfo.duplicate());

    addEvent(EventType::DATA_SERVER_ADDED,serverInfo.mServerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteDataServerInfoById(T::SessionId sessionId,uint serverId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ServerInfo* serverInfo = mDataServerInfoList.getServerInfoById(serverId);
    if (serverInfo == nullptr)
      return Result::UNKNOWN_SERVER_ID;

    mDataServerInfoList.deleteServerInfoById(serverId);

    addEvent(EventType::DATA_SERVER_DELETED,serverId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getDataServerInfoById(T::SessionId sessionId,uint serverId,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ServerInfo *info = mDataServerInfoList.getServerInfoById(serverId);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    serverInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getDataServerInfoByName(T::SessionId sessionId,std::string serverName,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ServerInfo *info = mDataServerInfoList.getServerInfoByName(serverName);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    serverInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getDataServerInfoByIor(T::SessionId sessionId,std::string serverIor,T::ServerInfo& serverInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ServerInfo *info = mDataServerInfoList.getServerInfoByIor(serverIor);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    serverInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getDataServerInfoList(T::SessionId sessionId,T::ServerInfoList& serverInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    serverInfoList = mDataServerInfoList;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getDataServerInfoCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mDataServerInfoList.getLength();
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteProducerInfoById(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ProducerInfo *info = mProducerInfoList.getProducerInfoById(producerId);
    if (info == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByProducerId(producerId);

    mFileInfoListByName.deleteFileInfoByProducerId(producerId);
    mFileInfoList.deleteFileInfoByProducerId(producerId);

    mGenerationInfoList.deleteGenerationInfoListByProducerId(producerId);

    mProducerInfoList.deleteProducerInfoById(producerId);

    addEvent(EventType::PRODUCER_DELETED,producerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteProducerInfoByName(T::SessionId sessionId,std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ProducerInfo *info = mProducerInfoList.getProducerInfoByName(producerName);
    if (info == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByProducerId(info->mProducerId);

    mFileInfoListByName.deleteFileInfoByProducerId(info->mProducerId);
    mFileInfoList.deleteFileInfoByProducerId(info->mProducerId);

    mGenerationInfoList.deleteGenerationInfoListByProducerId(info->mProducerId);

    mProducerInfoList.deleteProducerInfoById(info->mProducerId);

    addEvent(EventType::PRODUCER_DELETED,info->mProducerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoBySourceId(sourceId);

    mFileInfoListByName.deleteFileInfoBySourceId(sourceId);
    mFileInfoList.deleteFileInfoBySourceId(sourceId);

    mGenerationInfoList.deleteGenerationInfoListBySourceId(sourceId);

    mProducerInfoList.deleteProducerInfoListBySourceId(sourceId);

    addEvent(EventType::PRODUCER_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ProducerInfo *info = mProducerInfoList.getProducerInfoById(producerId);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    producerInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getProducerInfoByName(T::SessionId sessionId,std::string producerName,T::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ProducerInfo *info = mProducerInfoList.getProducerInfoByName(producerName);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    producerInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    producerInfoList = mProducerInfoList;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ContentInfoList contentInfoList;

    std::string startTime = "19000101T00000";
    std::string endTime = "23000101T00000";

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        if (mContentInfoListEnabled[1])
          mContentInfoList[1].getContentInfoListByFmiParameterId(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByFmiParameterId(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        if (mContentInfoListEnabled[2])
          mContentInfoList[2].getContentInfoListByFmiParameterName(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByFmiParameterName(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        if (mContentInfoListEnabled[3])
          mContentInfoList[3].getContentInfoListByGribParameterId(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByGribParameterId(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        if (mContentInfoListEnabled[4])
          mContentInfoList[4].getContentInfoListByNewbaseParameterId(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByNewbaseParameterId(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        if (mContentInfoListEnabled[5])
          mContentInfoList[5].getContentInfoListByNewbaseParameterName(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByNewbaseParameterName(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        break;

      case T::ParamKeyTypeValue::CDM_ID:
        if (mContentInfoListEnabled[6])
          mContentInfoList[6].getContentInfoListByCdmParameterId(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByCdmParameterId(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        break;

      case T::ParamKeyTypeValue::CDM_NAME:
        if (mContentInfoListEnabled[7])
          mContentInfoList[7].getContentInfoListByCdmParameterName(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByCdmParameterName(parameterKey,T::ParamLevelIdTypeValue::IGNORE,0,0,0,-2,-2,-2,startTime,endTime,0,contentInfoList);
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

        T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(info->mProducerId);
        if (producerInfo != nullptr)
          producerInfoList.addProducerInfo(producerInfo->duplicate());
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    mProducerInfoList.getProducerInfoListBySourceId(sourceId,producerInfoList);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ContentInfoList cList;
    cList.setReleaseObjects(false);
    cList = mContentInfoList[0];
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
        producerInfo = mProducerInfoList.getProducerInfoById(contentInfo->mProducerId);
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
            sourceParamKey = contentInfo->mFmiParameterName;
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
            targetParamKey = contentInfo->mFmiParameterName;
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





int MemoryImplementation::_addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteGenerationInfoById(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoById(generationId);
    if (info == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGenerationId(info->mGenerationId);

    mFileInfoListByName.deleteFileInfoByGenerationId(info->mGenerationId);
    mFileInfoList.deleteFileInfoByGenerationId(info->mGenerationId);

    mGenerationInfoList.deleteGenerationInfoById(info->mGenerationId);

    addEvent(EventType::GENERATION_DELETED,info->mGenerationId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteGenerationInfoByName(T::SessionId sessionId,std::string generationName)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (info == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGenerationId(info->mGenerationId);

    mFileInfoListByName.deleteFileInfoByGenerationId(info->mGenerationId);
    mFileInfoList.deleteFileInfoByGenerationId(info->mGenerationId);

    mGenerationInfoList.deleteGenerationInfoById(info->mGenerationId);

    addEvent(EventType::GENERATION_DELETED,info->mGenerationId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<uint>& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGenerationIdList(generationIdList);

    mFileInfoListByName.deleteFileInfoByGenerationIdList(generationIdList);
    mFileInfoList.deleteFileInfoByGenerationIdList(generationIdList);

    for (auto it = generationIdList.begin(); it != generationIdList.end(); ++it)
    {
      mGenerationInfoList.deleteGenerationInfoById(*it);
      addEvent(EventType::GENERATION_DELETED,*it,0,0,0);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;


    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByProducerId(producerInfo->mProducerId);

    mFileInfoListByName.deleteFileInfoByProducerId(producerInfo->mProducerId);
    mFileInfoList.deleteFileInfoByProducerId(producerInfo->mProducerId);

    mGenerationInfoList.deleteGenerationInfoListByProducerId(producerInfo->mProducerId);

    addEvent(EventType::GENERATION_LIST_DELETED_BY_PRODUCER_ID,producerInfo->mProducerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;


    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByProducerId(producerInfo->mProducerId);

    mFileInfoListByName.deleteFileInfoByProducerId(producerInfo->mProducerId);
    mFileInfoList.deleteFileInfoByProducerId(producerInfo->mProducerId);

    mGenerationInfoList.deleteGenerationInfoListByProducerId(producerInfo->mProducerId);

    mGenerationInfoList.deleteGenerationInfoListByProducerId(producerInfo->mProducerId);

    addEvent(EventType::GENERATION_LIST_DELETED_BY_PRODUCER_ID,producerInfo->mProducerId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoBySourceId(sourceId);

    mFileInfoListByName.deleteFileInfoBySourceId(sourceId);
    mFileInfoList.deleteFileInfoBySourceId(sourceId);

    mGenerationInfoList.deleteGenerationInfoListBySourceId(sourceId);

    addEvent(EventType::GENERATION_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoById(generationId);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getGenerationInfoByName(T::SessionId sessionId,std::string generationName,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *info = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    generationInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    generationInfoList = mGenerationInfoList;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    mGenerationInfoList.getGenerationInfoListByProducerId(producerId,generationInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    mGenerationInfoList.getGenerationInfoListByProducerId(producerInfo->mProducerId,generationInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    mGenerationInfoList.getGenerationInfoListBySourceId(sourceId,generationInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




int MemoryImplementation::_getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,std::string producerName,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,uchar status)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    generationInfo->mStatus = status;

    addEvent(EventType::GENERATION_STATUS_CHANGED,generationId,status,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_setGenerationInfoStatusByName(T::SessionId sessionId,std::string generationName,uchar status)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    generationInfo->mStatus = status;

    addEvent(EventType::GENERATION_STATUS_CHANGED,generationInfo->mGenerationId,status,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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

      //printf("-- File updated %s\n",fileInfo.mName.c_str());
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

      //printf("-- File added %s\n",fileInfo.mName.c_str());
      addEvent(EventType::FILE_ADDED,fileInfo.mFileId,fileInfo.mFileType,0,0);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
      //printf("** File exists %s\n",fileInfo.mName.c_str());
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
      //printf("** File added %s\n",fileInfo.mName.c_str());

      mMaxFileId++;

      fileInfo.mFileId = mMaxFileId;

      info = fileInfo.duplicate();
      mFileInfoList.addFileInfo(info);
      mFileInfoListByName.addFileInfo(info);
    }


    // ### Adding the content information into the database.

    uint len = contentInfoList.getLength();
    //printf("-- contentList %u\n",len);
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      if (info != nullptr)
      {
        T::ContentInfo *cInfo = info->duplicate();
        // ### Making sure that content data matches the file data.

        cInfo->mFileId = fileInfo.mFileId;
        cInfo->mFileType = fileInfo.mFileType;
        cInfo->mProducerId = fileInfo.mProducerId;
        cInfo->mGenerationId = fileInfo.mGenerationId;
        cInfo->mGroupFlags = fileInfo.mGroupFlags;
        cInfo->mFlags = cInfo->mFlags | T::FileInfo::Flags::PredefinedContent;

        for (int t=0; t<CONTENT_LIST_COUNT; t++)
        {
          if (mContentInfoListEnabled[t])
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
      //printf("-- file add event\n");
      addEvent(EventType::FILE_ADDED,fileInfo.mFileId,fileInfo.mFileType,0,0);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    T::FileInfoList tmpFileList;
    tmpFileList.setReleaseObjects(false);

    T::ContentInfoList tmpContentList;
    tmpContentList.setReleaseObjects(false);

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
        //ff->print(std::cout,0,0);
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
          T::ContentInfo *cInfo = info->duplicate();
          // ### Making sure that content data matches the file data.

          cInfo->mFileId = ff->mFileInfo.mFileId;
          cInfo->mFileType = ff->mFileInfo.mFileType;
          cInfo->mProducerId = ff->mFileInfo.mProducerId;
          cInfo->mGenerationId = ff->mFileInfo.mGenerationId;
          cInfo->mGroupFlags = ff->mFileInfo.mGroupFlags;
          cInfo->mFlags = cInfo->mFlags | T::FileInfo::Flags::PredefinedContent;

          //mContentInfoList[0].addContentInfo(cInfo);
          tmpContentList.addContentInfo(cInfo);
        }
      }

      // ### Adding an event to the event list.

      if (ff->mFileInfo.mFileId != mMaxFileId)
      {
        //printf("-- file update event\n");
        addEvent(EventType::FILE_UPDATED,ff->mFileInfo.mFileId,ff->mFileInfo.mFileType,0,0);
      }
      else
      {
        //printf("-- file add event\n");
        addEvent(EventType::FILE_ADDED,ff->mFileInfo.mFileId,ff->mFileInfo.mFileType,0,0);
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
      if (mContentInfoListEnabled[t])
        mContentInfoList[t].addContentInfoList(tmpContentList);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




int MemoryImplementation::_deleteFileInfoById(T::SessionId sessionId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoByName(T::SessionId sessionId,std::string filename)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);


    for (auto fTime = forecastTimeList.begin(); fTime != forecastTimeList.end(); ++fTime)
    {
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
              info->mForecastTime == fTime->mForecastTime)
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGroupFlags(groupFlags);

    mFileInfoListByName.deleteFileInfoByGroupFlags(groupFlags);
    mFileInfoList.deleteFileInfoByGroupFlags(groupFlags);

    addEvent(EventType::FILE_LIST_DELETED_BY_GROUP_FLAGS,groupFlags,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,std::string forecastTime)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




int MemoryImplementation::_deleteFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoBySourceId(sourceId);

    mFileInfoListByName.deleteFileInfoBySourceId(sourceId);
    mFileInfoList.deleteFileInfoBySourceId(sourceId);

    addEvent(EventType::FILE_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::FileInfo *info = mFileInfoList.getFileInfoById(fileId);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    fileInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getFileInfoByName(T::SessionId sessionId,std::string filename,T::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::FileInfo *info = mFileInfoListByName.getFileInfoByName(filename);
    if (info == nullptr)
      return Result::DATA_NOT_FOUND;

    fileInfo = *info;
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getFileInfoList(T::SessionId sessionId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    mFileInfoList.getFileInfoList(startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    mFileInfoList.getFileInfoListByProducerId(producerId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getFileInfoListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    mFileInfoList.getFileInfoListByProducerId(producerInfo->mProducerId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    mFileInfoList.getFileInfoListByGenerationId(generationId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    mFileInfoList.getFileInfoListByGenerationId(generationInfo->mGenerationId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




int MemoryImplementation::_getFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    mFileInfoList.getFileInfoListByGroupFlags(groupFlags,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    mFileInfoList.getFileInfoListBySourceId(sourceId,startFileId,maxRecords,fileInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    eventInfo.mEventId = addEvent(eventInfo.mType,eventInfo.mId1,eventInfo.mId2,eventInfo.mId3,eventInfo.mFlags);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    if (requestingServerId != 0)
    {
      T::ServerInfo *info = mDataServerInfoList.getServerInfoById(requestingServerId);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,uint maxRecords,T::EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    if (requestingServerId != 0)
    {
      T::ServerInfo *info = mDataServerInfoList.getServerInfoById(requestingServerId);
      if (info != nullptr)
        info->mLastCall = time(nullptr);
    }

    mEventInfoList.getEventInfoList(startEventId,maxRecords,eventInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
      if (mContentInfoListEnabled[t])
        mContentInfoList[t].addContentInfo(cInfo);
    }

    addEvent(EventType::CONTENT_ADDED,contentInfo.mFileId,contentInfo.mMessageIndex,0,contentInfo.mServerFlags);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      T::ContentInfo contentInfo;
      if (mContentInfoList[0].getContentInfoByFileIdAndMessageIndex(info->mFileId,info->mMessageIndex) != nullptr)
      {
        // printf("-- content already added %u:%u\n",info->mFileId,info->mMessageIndex);
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
        for (int t=0; t<CONTENT_LIST_COUNT; t++)
        {
          if (mContentInfoListEnabled[t])
            mContentInfoList[t].addContentInfo(cInfo);
        }

        addEvent(EventType::CONTENT_ADDED,info->mFileId,info->mMessageIndex,0,info->mServerFlags);
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteContentListByFileId(T::SessionId sessionId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteContentListByFileName(T::SessionId sessionId,std::string filename)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteContentListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoByGroupFlags(groupFlags);

    addEvent(EventType::CONTENT_LIST_DELETED_BY_GROUP_FLAGS,groupFlags,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteContentListByProducerId(T::SessionId sessionId,uint producerId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteContentListByProducerName(T::SessionId sessionId,std::string producerName)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteContentListByGenerationId(T::SessionId sessionId,uint generationId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteContentListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteContentListBySourceId(T::SessionId sessionId,uint sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteContentInfoBySourceId(sourceId);

    addEvent(EventType::CONTENT_LIST_DELETED_BY_SOURCE_ID,sourceId,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_registerContentList(T::SessionId sessionId,uint serverId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
#if 0
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    unsigned long long sf = 0;
    if (serverId > 0)
      sf = (1 << (serverId-1));

    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);

      T::ContentInfo contentInfo;
      if (getContent(info->mFileId,info->mMessageIndex,contentInfo) == Result::OK)
      {
        printf("Register content %u\n",info->mFileId);

        if (serverId > 0  &&  (info->mServerFlags & sf) == 0)
        {
          info->mServerFlags = contentInfo.mServerFlags | sf;
          setContent(*info);
          addEvent(EventType::CONTENT_REGISTERED,info->mFileId,info->mMessageIndex,serverId,info->mServerFlags);
        }
      }
      else
      {
        T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(info->mProducerId);
        if (producerInfo == nullptr)
          return Result::UNKNOWN_PRODUCER_ID;

        T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(info->mGenerationId);
        if (generationInfo == nullptr)
          return Result::UNKNOWN_GENERATION_ID;

        T::FileInfo fileInfo;
        if (getFileInfoById(info->mFileId,fileInfo) != Result::OK)
          return Result::UNKNOWN_FILE_ID;

        if (producerInfo.mProducerId != generationInfo.mProducerId)
          return Result::PRODUCER_AND_GENERATION_DO_NOT_MATCH;

        if (producerInfo.mProducerId != fileInfo.mProducerId)
          return Result::PRODUCER_AND_FILE_DO_NOT_MATCH;

        if (generationInfo.mGenerationId != fileInfo.mGenerationId)
          return Result::GENERATION_AND_FILE_DO_NOT_MATCH;

        printf("Add content %u\n",info->mFileId);
        unsigned long long id = ((unsigned long long)info->mFileId << 32) + info->mMessageIndex;
        info->mServerFlags = sf;

        redisReply *reply = (redisReply*)redisCommand(mContext,"ZADD %scontent %llu %s",mTablePrefix.c_str(),id,info->getCsv().c_str());
        if (reply == nullptr)
        {
          closeConnection();
          return Result::PERMANENT_STORAGE_ERROR;
        }

        freeReplyObject(reply);

        addEvent(EventType::CONTENT_ADDED,info->mFileId,info->mMessageIndex,serverId,info->mServerFlags);
      }
    }
#endif
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_registerContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
#if 0
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    unsigned long long sf = (1 << (serverId-1));

    T::ContentInfoList contentInfoList;
    getContentByFileId(fileId,contentInfoList);
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      if ((info->mServerFlags & sf) == 0)
      {
        info->mServerFlags = info->mServerFlags | sf;
        setContent(*info);
        addEvent(EventType::CONTENT_REGISTERED,info->mFileId,info->mMessageIndex,serverId,info->mServerFlags);
      }
    }
#endif
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_unregisterContentList(T::SessionId sessionId,uint serverId)
{
  FUNCTION_TRACE
  try
  {
#if 0
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    return unregisterContent(serverId);
#endif
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_unregisterContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  FUNCTION_TRACE
  try
  {
#if 0
    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (!isConnectionValid())
      return Result::NO_CONNECTION_TO_PERMANENT_STORAGE;

    unsigned long long sf = (1 << (serverId-1));
    unsigned long long nsf = ~sf;

    T::ContentInfoList contentInfoList;
    getContentByFileId(fileId,contentInfoList);
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      if ((info->mServerFlags & sf) != 0)
      {
        info->mServerFlags = info->mServerFlags & nsf;
        setContent(*info);
      }
    }
#endif
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList[0].getContentInfoList(startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList[0].getContentInfoListByFileId(fileId,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByFileName(T::SessionId sessionId,std::string filename,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::FileInfo *fileInfo = mFileInfoListByName.getFileInfoByName(filename);
    if (fileInfo == nullptr)
      return Result::UNKNOWN_FILE_NAME;

    mContentInfoList[0].getContentInfoListByFileId(fileInfo->mFileId,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList[0].getContentInfoListByGroupFlags(groupFlags,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    mContentInfoList[0].getContentInfoListByProducerId(producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    mContentInfoList[0].getContentInfoListByProducerId(producerInfo->mProducerId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByServerId(T::SessionId sessionId,uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ServerInfo *info = mDataServerInfoList.getServerInfoById(serverId);
    if (info == nullptr)
      return Result::UNKNOWN_SERVER_ID;

    mContentInfoList[0].getContentInfoListByServerId(serverId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationId,startTime,endTime,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,std::string generationName,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    mContentInfoList[0].getContentInfoListByGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,startTime,endTime,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList[0].getContentInfoListBySourceId(sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        if (mContentInfoListEnabled[1])
          mContentInfoList[1].getContentInfoListByFmiParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByFmiParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::FMI_NAME:
        if (mContentInfoListEnabled[2])
          mContentInfoList[2].getContentInfoListByFmiParameterName(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByFmiParameterName(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;


      case T::ParamKeyTypeValue::GRIB_ID:
        if (mContentInfoListEnabled[3])
          mContentInfoList[3].getContentInfoListByGribParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByGribParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        if (mContentInfoListEnabled[4])
          mContentInfoList[4].getContentInfoListByNewbaseParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByNewbaseParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        if (mContentInfoListEnabled[5])
          mContentInfoList[5].getContentInfoListByNewbaseParameterName(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByNewbaseParameterName(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_ID:
        if (mContentInfoListEnabled[6])
          mContentInfoList[6].getContentInfoListByCdmParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByCdmParameterId(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_NAME:
        if (mContentInfoListEnabled[7])
          mContentInfoList[7].getContentInfoListByCdmParameterName(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByCdmParameterName(parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        if (mContentInfoListEnabled[1])
          mContentInfoList[1].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::FMI_NAME:
        if (mContentInfoListEnabled[2])
          mContentInfoList[2].getContentInfoListByFmiParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByFmiParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

        return Result::OK;

      case T::ParamKeyTypeValue::GRIB_ID:
        if (mContentInfoListEnabled[3])
          mContentInfoList[3].getContentInfoListByGribParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByGribParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        if (mContentInfoListEnabled[4])
          mContentInfoList[4].getContentInfoListByNewbaseParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByNewbaseParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        if (mContentInfoListEnabled[5])
          mContentInfoList[5].getContentInfoListByNewbaseParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByNewbaseParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_ID:
        if (mContentInfoListEnabled[6])
          mContentInfoList[6].getContentInfoListByCdmParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByCdmParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_NAME:
        if (mContentInfoListEnabled[7])
          mContentInfoList[7].getContentInfoListByCdmParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByCdmParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByParameterAndGenerationName(T::SessionId sessionId,std::string generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoByName(generationName);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_NAME;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        if (mContentInfoListEnabled[1])
          mContentInfoList[1].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::FMI_NAME:
        if (mContentInfoListEnabled[2])
          mContentInfoList[2].getContentInfoListByFmiParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByFmiParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::GRIB_ID:
        if (mContentInfoListEnabled[3])
          mContentInfoList[3].getContentInfoListByGribParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByGribParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        if (mContentInfoListEnabled[4])
          mContentInfoList[4].getContentInfoListByNewbaseParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByNewbaseParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        if (mContentInfoListEnabled[5])
          mContentInfoList[5].getContentInfoListByNewbaseParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByNewbaseParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_ID:
        if (mContentInfoListEnabled[6])
          mContentInfoList[6].getContentInfoListByCdmParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByCdmParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_NAME:
        if (mContentInfoListEnabled[7])
          mContentInfoList[7].getContentInfoListByCdmParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByCdmParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoById(producerId);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_ID;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        if (mContentInfoListEnabled[1])
          mContentInfoList[1].getContentInfoListByFmiParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByFmiParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::FMI_NAME:
        if (mContentInfoListEnabled[2])
          mContentInfoList[2].getContentInfoListByFmiParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByFmiParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::GRIB_ID:
        if (mContentInfoListEnabled[3])
          mContentInfoList[3].getContentInfoListByGribParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByGribParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        if (mContentInfoListEnabled[4])
          mContentInfoList[4].getContentInfoListByNewbaseParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByNewbaseParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        if (mContentInfoListEnabled[5])
          mContentInfoList[5].getContentInfoListByNewbaseParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByNewbaseParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_ID:
        if (mContentInfoListEnabled[6])
          mContentInfoList[6].getContentInfoListByCdmParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByCdmParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_NAME:
        if (mContentInfoListEnabled[7])
          mContentInfoList[7].getContentInfoListByCdmParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByCdmParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string forecastTime,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

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
          mContentInfoList[1].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        else
          mContentInfoList[0].getContentInfoListByFmiParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        break;

      case T::ParamKeyTypeValue::FMI_NAME:
        if (mContentInfoListEnabled[2])
        {
          T::ContentInfo *cInfo = mContentInfoList[2].getContentInfoByFmiParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime);
          if (cInfo != nullptr)
          {
            contentInfoList.addContentInfo(cInfo->duplicate());
            return Result::OK;
          }
          //mContentInfoList[2].getContentInfoListByFmiParameterNameAndGenerationId(generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
          mContentInfoList[2].getContentInfoListByFmiParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,contentList);
        }
        else
        {
          //printf("Not sorted\n");
          mContentInfoList[0].getContentInfoListByFmiParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        }
        break;

      case T::ParamKeyTypeValue::GRIB_ID:
        if (mContentInfoListEnabled[3])
          mContentInfoList[3].getContentInfoListByGribParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        else
          mContentInfoList[0].getContentInfoListByGribParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        break;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        if (mContentInfoListEnabled[4])
          mContentInfoList[4].getContentInfoListByNewbaseParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        else
          mContentInfoList[0].getContentInfoListByNewbaseParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        break;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        if (mContentInfoListEnabled[5])
          mContentInfoList[5].getContentInfoListByNewbaseParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        else
          mContentInfoList[0].getContentInfoListByNewbaseParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        break;

      case T::ParamKeyTypeValue::CDM_ID:
        if (mContentInfoListEnabled[6])
          mContentInfoList[6].getContentInfoListByCdmParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        else
          mContentInfoList[0].getContentInfoListByCdmParameterIdAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        break;

      case T::ParamKeyTypeValue::CDM_NAME:
        if (mContentInfoListEnabled[7])
          mContentInfoList[7].getContentInfoListByCdmParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
        else
          mContentInfoList[0].getContentInfoListByCdmParameterNameAndGenerationId(generationInfo->mProducerId,generationInfo->mGenerationId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentList);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByParameterAndProducerName(T::SessionId sessionId,std::string producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::ProducerInfo *producerInfo = mProducerInfoList.getProducerInfoByName(producerName);
    if (producerInfo == nullptr)
      return Result::UNKNOWN_PRODUCER_NAME;

    switch (parameterKeyType)
    {
      case T::ParamKeyTypeValue::FMI_ID:
        if (mContentInfoListEnabled[1])
          mContentInfoList[1].getContentInfoListByFmiParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByFmiParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::FMI_NAME:
        if (mContentInfoListEnabled[2])
          mContentInfoList[2].getContentInfoListByFmiParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByFmiParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::GRIB_ID:
        if (mContentInfoListEnabled[3])
          mContentInfoList[3].getContentInfoListByGribParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByGribParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_ID:
        if (mContentInfoListEnabled[4])
          mContentInfoList[4].getContentInfoListByNewbaseParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByNewbaseParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::NEWBASE_NAME:
        if (mContentInfoListEnabled[5])
          mContentInfoList[5].getContentInfoListByNewbaseParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByNewbaseParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_ID:
        if (mContentInfoListEnabled[6])
          mContentInfoList[6].getContentInfoListByCdmParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByCdmParameterIdAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        return Result::OK;

      case T::ParamKeyTypeValue::CDM_NAME:
        if (mContentInfoListEnabled[7])
          mContentInfoList[7].getContentInfoListByCdmParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
        else
          mContentInfoList[0].getContentInfoListByCdmParameterNameAndProducerId(producerInfo->mProducerId,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListByRequestCounterKey(T::SessionId sessionId,ulonglong key,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;


    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList[0].getContentInfoListByRequestCounterKey(key,contentInfoList);;

    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentListOfInvalidIntegrity(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    mContentInfoList[0].getContentGeometryIdListByGenerationId(generationInfo->mProducerId,generationId,geometryIdList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentParamKeyListByGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    mContentInfoList[0].getContentParamKeyListByGenerationId(generationInfo->mProducerId,generationId,parameterKeyType,paramKeyList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    mContentInfoList[0].getForecastTimeListByGenerationId(generationInfo->mProducerId,generationId,contentTimeList);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    T::GenerationInfo *generationInfo = mGenerationInfoList.getGenerationInfoById(generationId);
    if (generationInfo == nullptr)
      return Result::UNKNOWN_GENERATION_ID;

    mContentInfoList[0].getForecastTimeListByGenerationAndGeometry(generationInfo->mProducerId,generationId,geometryId,contentTimeList);
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getContentTimeListByProducerId(T::SessionId sessionId,uint producerId,std::set<std::string>& contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    mContentInfoList[0].getForecastTimeListByProducerId(producerId,contentTimeList);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoReadLock lock(&mModificationLock,__FILE__,__LINE__);

    uint len = mContentInfoList[0].getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = mContentInfoList[0].getContentInfoByIndex(t);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    mContentInfoList[0].getLevelInfoList(levelInfoList);
    return Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_deleteVirtualContent(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteVirtualContent();

    mFileInfoListByName.deleteVirtualFiles();
    mFileInfoList.deleteVirtualFiles();

    addEvent(EventType::DELETE_VIRTUAL_CONTENT,0,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int MemoryImplementation::_updateVirtualContent(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    AutoWriteLock lock(&mModificationLock,__FILE__,__LINE__);

    for (int t=CONTENT_LIST_COUNT-1; t>=0; t--)
      mContentInfoList[t].deleteVirtualContent();

    mFileInfoListByName.deleteVirtualFiles();
    mFileInfoList.deleteVirtualFiles();

    addEvent(EventType::UPDATE_VIRTUAL_CONTENT,0,0,0,0);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




T::EventId MemoryImplementation::addEvent(uint eventType,uint id1,uint id2,uint id3,unsigned long long flags)
{
  FUNCTION_TRACE
  try
  {
    saveData();

    mMaxEventId++;
    T::EventId eventId = mMaxEventId;

    T::EventInfo *eventInfo = new T::EventInfo(mStartTime,eventId,eventType,id1,id2,id3,flags);
    mEventInfoList.addEventInfo(eventInfo);

    return eventId;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void MemoryImplementation::readDataServerList()
{
  FUNCTION_TRACE
  try
  {
    mDataServerInfoList.clear();

    if (!mContentLoadEnabled)
      return;

    char filename[200];

    sprintf(filename,"%s/dataServers.csv",mContentDir.c_str());
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

        T::ServerInfo *rec = new T::ServerInfo();
        rec->setCsv(st);
        mDataServerInfoList.addServerInfo(rec);
      }
    }
    fclose(file);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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

        if (mDataServerCount != mDataServerInfoList.getLength())
          mDataServerInfoList.writeToFile(mContentDir + "/dataServers.csv");

        if (mProducerCount != mProducerInfoList.getLength())
          mProducerInfoList.writeToFile(mContentDir + "/producers.csv");

        if (mGenerationCount != mGenerationInfoList.getLength())
          mGenerationInfoList.writeToFile(mContentDir + "/generations.csv");

        if (mFileCount != mFileInfoList.getLength())
          mFileInfoList.writeToFile(mContentDir + "/files.csv");

        if (mContentCount != mContentInfoList[0].getLength())
          mContentInfoList[0].writeToFile(mContentDir + "/content.csv");

        mDataServerCount = mDataServerInfoList.getLength();
        mProducerCount = mProducerInfoList.getLength();
        mGenerationCount = mGenerationInfoList.getLength();
        mFileCount= mFileInfoList.getLength();
        mContentCount = mContentInfoList[0].getLength();
      }
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}







}
}
