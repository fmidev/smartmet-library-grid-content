#include "ServiceImplementation.h"
#include "grid-files/common/GeneralFunctions.h"
#include "grid-files/common/ShowFunction.h"

#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace DataServer
{


static void* ServiceImplementation_eventProcessingThread(void *arg)
{
  try
  {
    ServiceImplementation *service = (ServiceImplementation*)arg;
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







ServiceImplementation::ServiceImplementation()
{
  FUNCTION_TRACE
  try
  {
    mServerSessionId = 0;
    mServerId = 0;
    mContentServer = NULL;

    mLastProcessedEventId = 0;
    mShutdownRequested = false;
    mFullUpdateRequired = false;
    mEventProcessingActive = false;
    mContentRegistrationEnabled = true;
    mContentServerStartTime = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ServiceImplementation::~ServiceImplementation()
{
  FUNCTION_TRACE
  try
  {
    if (!mShutdownRequested)
      shutdown();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::init(T::SessionId serverSessionId,uint serverId,std::string serverName,std::string serverIor,std::string dataDir,ContentServer::ServiceInterface *contentServer)
{
  FUNCTION_TRACE
  try
  {
    if (contentServer == NULL)
      throw SmartMet::Spine::Exception(BCP,"The 'contentServer' parameter points to NULL!");

    //if (serverId == 0 ||  serverId > 64)
    //  throw SmartMet::Spine::Exception(BCP,"The 'serverId' parameter value must be in the range [1..64]!");

    mServerId = serverId;
    mServerSessionId = serverSessionId;
    mServerName = serverName;
    mServerIor = serverIor;
    mDataDir = dataDir;
    mContentServer = contentServer;
    mFullUpdateRequired = true;

    checkServerRegistration();
    // fullUpdate();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::startEventProcessing()
{
  FUNCTION_TRACE
  try
  {
    pthread_create(&mThread,NULL,ServiceImplementation_eventProcessingThread,this);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceImplementation::_getGridValueList(T::SessionId sessionId,T::ValueRecordList& valueRecordList)
{
  FUNCTION_TRACE
  try
  {
    uint sz = valueRecordList.getLength();
    for (uint t=0; t<sz; t++)
    {
      T::ValueRecord *rec = valueRecordList.getValueRecordByIndex(t);
      if (rec != NULL)
      {
        //rec->print(std::cout,0,0);
        GRID::GridFile_sptr gridFile = mGridStorage.getFileById(rec->mFileId);
        if (gridFile)
        {
          GRID::Message *message = gridFile->getMessageByIndex(rec->mMessageIndex);
          if (message)
          {
            try
            {
              switch (rec->mCoordinateType)
              {
                case T::CoordinateType::UNKNOWN:
                case T::CoordinateType::LATLON_COORDINATES:
                  rec->mValue = message->getParameterValueByLatLon(rec->mY,rec->mX,(T::InterpolationMethod)rec->mInterpolationMethod);
                  rec->mResult = 0;
                  break;

                case T::CoordinateType::GRID_COORDINATES:
                  rec->mValue = message->getParameterValueByGridPosition(rec->mX,rec->mY,(T::InterpolationMethod)rec->mInterpolationMethod);
                  rec->mResult = 0;
                  break;

                case T::CoordinateType::ORIGINAL_COORDINATES:
                  // TODO: Implementation required
                  rec->mResult = Result::NOT_IMPLEMENTED;
                  break;
              }
            }
            catch (...)
            {
              SmartMet::Spine::Exception exception(BCP,"Operation failed!",NULL);
              exception.addParameter("FileId",std::to_string(rec->mFileId));
              exception.addParameter("MessageIndex",std::to_string(rec->mMessageIndex));
              rec->mResult = Result::UNEXPECTED_EXCEPTION;
              exception.printError();
            }
          }
          else
          {
            rec->mResult = Result::MESSAGE_NOT_FOUND;
          }
        }
        else
        {
          rec->mResult = Result::FILE_NOT_FOUND;
        }
      }
    }
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceImplementation::_getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridCoordinates& coordinates)
{
  FUNCTION_TRACE
  try
  {
    GRID::GridFile_sptr gridFile = mGridStorage.getFileById(fileId);
    if (!gridFile)
      return Result::FILE_NOT_FOUND;

    GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
    if (message == NULL)
      return Result::MESSAGE_NOT_FOUND;

    coordinates.mProjection = (uint)message->getGridProjection();
    coordinates.mColumns = (uint)message->getGridOriginalColumnCount();
    coordinates.mRows = (uint)message->getGridOriginalRowCount();
    message->getGridProjectionAttributes("",coordinates.mProjectionAttributes);
    coordinates.mCoordinateList = message->getGridCoordinates();

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceImplementation::_getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data)
{
  FUNCTION_TRACE
  try
  {
    GRID::GridFile_sptr gridFile = mGridStorage.getFileById(fileId);
    if (!gridFile)
      return Result::DATA_NOT_FOUND;

    GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
    if (message == NULL)
      return Result::DATA_NOT_FOUND;

    data.mServerId = mServerId;
    data.mGroupFlags = gridFile->getGroupFlags();
    data.mProducerId = gridFile->getProducerId();
    data.mGenerationId = gridFile->getGenerationId();
    data.mFileId = gridFile->getFileId();
    data.mFileType = gridFile->getFileType();
    data.mFileName = gridFile->getFileName().substr(mDataDir.length()+1);
    data.mMessageIndex = messageIndex;
    data.mStartTime = message->getForecastStartTime();
    data.mEndTime = message->getForecastEndTime();
    data.mGribParameterId = message->getGribParameterId();
    data.mGrib1ParameterLevelId = message->getGrib1ParameterLevelId();
    data.mGrib2ParameterLevelId = message->getGrib2ParameterLevelId();
    data.mGribParameterName = message->getGribParameterName();
    data.mGribParameterDescription = message->getGribParameterDescription();
    data.mGribParameterUnits = message->getGribParameterUnits();
    data.mFmiParameterId = message->getFmiParameterId();
    data.mFmiParameterLevelId = message->getFmiParameterLevelId();
    data.mFmiParameterName = message->getFmiParameterName();
    data.mFmiParameterDescription = message->getFmiParameterDescription();
    data.mFmiParameterUnits = message->getFmiParameterUnits();
    data.mCdmParameterId = message->getCdmParameterId();
    data.mCdmParameterName = message->getCdmParameterName();
    data.mNewbaseParameterId = message->getNewbaseParameterId();
    data.mNewbaseParameterName = message->getNewbaseParameterName();
    data.mParameterLevel = message->getParameterLevel();
    data.mProjection = (uint)message->getGridProjection();
    data.mColumns = (uint)message->getGridOriginalColumnCount();
    data.mRows = (uint)message->getGridOriginalRowCount();
    data.mTypeOfEnsembleForecast = (unsigned char)*message->getTypeOfEnsembleForecast();
    data.mPerturbationNumber = (unsigned char)*message->getPerturbationNumber();
    message->getGridProjectionAttributes("",data.mProjectionAttributes);

    message->getParameterValues(data.mValues);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceImplementation::_getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    GRID::GridFile_sptr gridFile = mGridStorage.getFileById(fileId);
    if (!gridFile)
      return Result::DATA_NOT_FOUND;

    GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
    if (message == NULL)
      return Result::DATA_NOT_FOUND;

    message->getAttributeList(std::string(""),attributeList);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceImplementation::_getGridValue(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = mGridStorage.getFileById(fileId);
      if (gridFile == NULL)
      {
        //printf("FILE NOT FOUND %u\n",fileId);
        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == NULL)
      {
        //printf("MESSAGE NOT FOUND %u:%u\n",fileId,messageIndex);
        return Result::MESSAGE_NOT_FOUND;
      }

      switch (coordinateType)
      {
        case T::CoordinateType::UNKNOWN:
        case T::CoordinateType::LATLON_COORDINATES:
           value = message->getParameterValueByLatLon(y,x,interpolationMethod);
           return Result::OK;

         case T::CoordinateType::GRID_COORDINATES:
           value = message->getParameterValueByGridPosition(x,y,interpolationMethod);
           return Result::OK;

         case T::CoordinateType::ORIGINAL_COORDINATES:
            // TODO: Implementation required
            return Result::NOT_IMPLEMENTED;
      }
      return Result::UNKNOW_COORDINATE_TYPE;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,"Operation failed!",NULL);
       exception.addParameter("FileId",std::to_string(fileId));
       exception.addParameter("MessageIndex",std::to_string(messageIndex));
       exception.printError();
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceImplementation::_getGridValuesByArea(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,T::InterpolationMethod interpolationMethod,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = mGridStorage.getFileById(fileId);
      if (gridFile == NULL)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == NULL)
        return Result::MESSAGE_NOT_FOUND;

      switch (coordinateType)
      {
        case T::CoordinateType::UNKNOWN:
        case T::CoordinateType::LATLON_COORDINATES:
          for (uint r=0; r < rows; r++)
          {
            double xx = x;
            for (uint c=0; c < columns; c++)
            {
              T::ParamValue value = message->getParameterValueByLatLon(y,xx,interpolationMethod);
              values.push_back(value);
              xx = xx + xStep;
            }
            y = y + yStep;
          }
          return Result::OK;

         case T::CoordinateType::GRID_COORDINATES:
           for (uint r=0; r < rows; r++)
           {
             double xx = x;
             for (uint c=0; c < columns; c++)
             {
               T::ParamValue value = message->getParameterValueByGridPosition(xx,y,interpolationMethod);
               values.push_back(value);
               xx = xx + xStep;
             }
             y = y + yStep;
           }
           return Result::OK;

         case T::CoordinateType::ORIGINAL_COORDINATES:
            // TODO: Implementation required
            return Result::NOT_IMPLEMENTED;
      }
      return Result::UNKNOW_COORDINATE_TYPE;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,"Operation failed!",NULL);
       exception.addParameter("FileId",std::to_string(fileId));
       exception.addParameter("MessageIndex",std::to_string(messageIndex));
       exception.printError();
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::addFile(T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    time_t checkTime = time(0);
    T::ContentInfoList contentList;

    if ((fileInfo.mFileId % 1000) == 0)
      printf("** addFile %u (%d)\n",fileInfo.mFileId,(int)mGridStorage.getFileCount());

    GRID::GridFile_sptr storageFile = mGridStorage.getFileByIdNoMapping(fileInfo.mFileId);

    GRID::GridFile *gridFile = NULL;
    if (storageFile)
    {
      //printf("** file already in the content storage %u\n",fileInfo.mFileId);
      gridFile = storageFile.get();
    }
    else
    {
      //if ((fileInfo.mFileId % 1000) == 0)
      //  fileInfo.print(std::cout,0,0);

      gridFile = new GRID::GridFile();
      gridFile->setFileName(mDataDir + "/" + fileInfo.mName);
      gridFile->setFileId(fileInfo.mFileId);
      gridFile->setGroupFlags(fileInfo.mGroupFlags);
      gridFile->setProducerId(fileInfo.mProducerId);
      gridFile->setGenerationId(fileInfo.mGenerationId);
      gridFile->setSourceId(fileInfo.mSourceId);
    }

    if (gridFile->getModificationTime() != 0)
    {
      if ((fileInfo.mFlags & (uint)T::FileInfoFlags::CONTENT_PREDEFINED) == 0)
      {
        // The content of the file is not predefined. However, some other data server might
        // have already registered the content.

        mContentServer->getContentListByFileId(mServerSessionId,fileInfo.mFileId,contentList);

        if (contentList.getLength() == 0)
        {
          // The content of the file is not registered. So, we need to read it.
          printf("  -- read content %u\n",fileInfo.mFileId);
          gridFile->read(mDataDir + "/" + fileInfo.mName);
        }
      }

      if (!storageFile)
        mGridStorage.addFile(gridFile);
    }
    else
    {
      // The grid file does not exist.

      //printf("*** modification time is zero\n");
      if (!storageFile)
        delete gridFile;

      return;
    }

    gridFile->setCheckTime(checkTime);

    if ((fileInfo.mFlags & (uint)T::FileInfoFlags::CONTENT_PREDEFINED) != 0  ||  contentList.getLength() > 0)
    {
      // The content of the file is predefined or registered by another server. So,
      // we can just register our server for the current content (if we have not already
      // registered. This is possible if the content server has been down).

      //printf("*** REGISTER %u\n",fileInfo.mFileId);
      if (mServerId > 0  &&  mContentRegistrationEnabled)
      {
        unsigned long long sf = (1 << (mServerId-1));

        T::ContentInfo *info = contentList.getContentInfoByIndex(0);
        if (info == NULL  ||  (info->mServerFlags & sf) == 0)
          mContentServer->registerContentListByFileId(mServerSessionId,mServerId,fileInfo.mFileId);
      }
    }
    else
    {
      // The content of the file is not predefined or registered earlier. So, we have
      // to register the content.

      gridFile->read(mDataDir + "/" + fileInfo.mName);

      std::size_t messageCount = gridFile->getNumberOfMessages();
      for (std::size_t m=0; m<messageCount; m++)
      {
        GRID::Message *message = gridFile->getMessageByIndex(m);
        T::ContentInfo *contentInfo = new T::ContentInfo();
        if (mServerId != 0)
          contentInfo->mServerFlags = 1 << (mServerId-1);
        else
          contentInfo->mServerFlags = 0;

        contentInfo->mGroupFlags = fileInfo.mGroupFlags;
        contentInfo->mProducerId = fileInfo.mProducerId;
        contentInfo->mGenerationId = fileInfo.mGenerationId;
        contentInfo->mFileType = gridFile->getFileType();
        contentInfo->mFileId = fileInfo.mFileId;
        contentInfo->mMessageIndex = (uint)m;
        contentInfo->mStartTime = message->getForecastStartTime();
        contentInfo->mEndTime = message->getForecastEndTime();
        contentInfo->mFmiParameterId = message->getFmiParameterId();
        contentInfo->mFmiParameterName = message->getFmiParameterName();
        contentInfo->mGribParameterId = message->getGribParameterId();
        contentInfo->mFmiParameterLevelId = message->getFmiParameterLevelId();
        contentInfo->mGrib1ParameterLevelId = message->getGrib1ParameterLevelId();
        contentInfo->mGrib2ParameterLevelId = message->getGrib2ParameterLevelId();
        contentInfo->mParameterLevel = message->getParameterLevel();
        contentInfo->mFmiParameterUnits = message->getFmiParameterUnits();
        contentInfo->mGribParameterUnits = message->getGribParameterUnits();
        contentInfo->mCdmParameterId = message->getCdmParameterId();
        contentInfo->mCdmParameterName = message->getCdmParameterName();
        contentInfo->mNewbaseParameterId = message->getNewbaseParameterId();
        contentInfo->mNewbaseParameterName = message->getNewbaseParameterName();
        contentInfo->mTypeOfEnsembleForecast = (unsigned char)*message->getTypeOfEnsembleForecast();
        contentInfo->mPerturbationNumber = (unsigned char)*message->getPerturbationNumber();
        contentInfo->mFlags = 0;
        contentInfo->mSourceId = fileInfo.mSourceId;

        //contentInfo->print(std::cout,0,0);
        contentInfoList.addContentInfo(contentInfo);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::fullUpdate()
{
  FUNCTION_TRACE
  try
  {
    //printf("********** FULL UPDATE %u ***************\n",mServerId);
    // Remove all existing content registered for this server.

    // mContentServer->unregisterContentList(mServerSessionId,mServerId,0);

    T::EventInfo eventInfo;

    int result  = mContentServer->getLastEventInfo(mServerSessionId,mServerId,eventInfo);
    if (result == Result::OK)
    {
      mLastProcessedEventId = eventInfo.mEventId;
      mContentServerStartTime = eventInfo.mServerTime;
    }
    else
    {

    }

    time_t checkTime = time(0);
    uint startFileId = 0;
    uint len = 1000;
    while (len == 1000)
    {
      if (mShutdownRequested)
        return;

      T::FileInfoList fileInfoList;

      int result = mContentServer->getFileInfoList(mServerSessionId,startFileId,1000,fileInfoList);
      if (result != 0)
      {
        fprintf(stderr,"ERROR: Cannot get the file list from the content server!");
        return;
      }


      T::ContentInfoList contentInfoList;
      len = fileInfoList.getLength();
      printf("READ %u\n",len);
      for (uint t=0; t<len; t++)
      {
        if (mShutdownRequested)
          return;

        try
        {
          T::FileInfo *fileInfo = fileInfoList.getFileInfoByIndex(t);
          if (fileInfo->mFileId >= startFileId)
            startFileId = fileInfo->mFileId + 1;

          addFile(*fileInfo,contentInfoList);
          if (contentInfoList.getLength() > 1000)
          {
            mContentServer->registerContentList(mServerSessionId,mServerId,contentInfoList);
            contentInfoList.clear();
          }
        }
        catch (...)
        {
          SmartMet::Spine::Exception exception(BCP,"Operation failed!",NULL);
          //exception.printError();
        }
      }

      if (contentInfoList.getLength() > 0)
      {
        mContentServer->registerContentList(mServerSessionId,mServerId,contentInfoList);
        contentInfoList.clear();
      }
    }


    std::size_t fcount = mGridStorage.getFileCount();
    if (fcount > 0)
    {
      for (long long t=(long long)fcount-1; t>=0; t--)
      {
        if (mShutdownRequested)
          return;

        auto gridFile = mGridStorage.getFileByIndex((std::size_t)t);
        if (gridFile->getCheckTime() < checkTime)
        {
          //printf("REMOVE %u\n",gridFile->getFileId());
          mGridStorage.deleteFileByIndex((std::size_t)t);
        }
      }
    }

    mFullUpdateRequired = false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_clear(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: clear\n",eventInfo.mEventId);
    mGridStorage.clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_contentServerReload(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: contentServerReload\n",eventInfo.mEventId);
    mFullUpdateRequired = true;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_producerAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: producerAdded(%u)\n",eventInfo.mEventId,eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_producerDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: producerDeleted(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridStorage.deleteFilesByProducerId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_producerListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: producerListDeletedBySourceId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridStorage.deleteFilesBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_generationAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: generationAdded(%u)\n",eventInfo.mEventId,eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_generationDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: generationDeleted(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridStorage.deleteFilesByGenerationId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_generationStatusChanged(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: generationStatusChanged(%u)\n",eventInfo.mEventId,eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_generationListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: generationListDeletedByProducerId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridStorage.deleteFilesByProducerId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_generationListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: generationListDeletedBySourceId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridStorage.deleteFilesBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_fileAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: fileAdded(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    T::FileInfo fileInfo;
    int result = mContentServer->getFileInfoById(mServerSessionId,eventInfo.mId1,fileInfo);
    if (result != 0)
    {
      printf("ERROR: getFileInfoById : %d\n",result);
      return;
    }

    T::ContentInfoList contentInfoList;
    addFile(fileInfo,contentInfoList);
    if (contentInfoList.getLength() > 0)
    {
      mContentServer->addContentList(mServerSessionId,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_fileDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: filedDeleted(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridStorage.deleteFileById(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_fileUpdated(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: filedUpdated(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    GRID::GridFile_sptr storageFile = mGridStorage.getFileByIdNoMapping(eventInfo.mId1);
    if (storageFile)
    {
      GRID::GridFile *gridFile = storageFile.get();
      std::string filename = gridFile->getFileName();
      if (getFileModificationTime(filename.c_str()) == gridFile->getModificationTime())
      {
        // The file has not been modified.
        //printf("-- no update required\n");
        return;
      }

      //printf("-- update required\n");
      // Removing the current file.
      mGridStorage.deleteFileById(eventInfo.mId1);
    }

    // Adding a new file information.
    T::FileInfo fileInfo;
    int result = mContentServer->getFileInfoById(mServerSessionId,eventInfo.mId1,fileInfo);
    if (result != 0)
    {
      printf("ERROR: getFileInfoById : %d\n",result);
      return;
    }

    // If the content list is not predefined the we should remove it and replace it with a new list.
    if ((fileInfo.mFlags & (uint)T::FileInfoFlags::CONTENT_PREDEFINED) == 0)
      mContentServer->deleteContentListByFileId(mServerSessionId,eventInfo.mId1);

    T::ContentInfoList contentInfoList;
    addFile(fileInfo,contentInfoList);
    if (contentInfoList.getLength() > 0)
    {
      mContentServer->addContentList(mServerSessionId,contentInfoList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_fileListDeletedByGroupFlags(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: fileListDeletedByGroupFlags(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridStorage.deleteFilesByGroupFlags(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_fileListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: fileListDeletedByProducerId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridStorage.deleteFilesByProducerId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_fileListDeletedByGenerationId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: fileListDeletedByGenerationId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridStorage.deleteFilesByGenerationId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}






void ServiceImplementation::event_fileListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: fileListDeletedBySourceId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridStorage.deleteFilesBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_contentListDeletedByFileId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: contentListDeletedByFileId(%u)\n",eventInfo.mEventId,eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_contentListDeletedByGroupFlags(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: contentListDeletedByGroupFlags(%u)\n",eventInfo.mEventId,eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_contentListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: contentListDeletedByProducerId(%u)\n",eventInfo.mEventId,eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_contentListDeletedByGenerationId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: contentListDeletedByGenerationId(%u)\n",eventInfo.mEventId,eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ServiceImplementation::event_contentListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: contentListDeletedBySourceId(%u)\n",eventInfo.mEventId,eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_dataServerAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: dataServerAdded(%u)\n",eventInfo.mEventId,eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_dataServerDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
//    printf("EVENT[%llu]: dataServerDeleted(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    if (eventInfo.mId1 == mServerId)
    {
      // It seems that somebody has deleted the current server registration from
      // the content server.

      printf("**** SHUTDOWN REQUIRED ****\n");
      mShutdownRequested = true;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ServiceImplementation::event_contentAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: contentAdded(%u,%u)\n",eventInfo.mEventId,eventInfo.mId1,eventInfo.mId2);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_contentDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: contentDeleted(%u,%u)\n",eventInfo.mEventId,eventInfo.mId1,eventInfo.mId2);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::event_contentRegistered(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: contentRegistered(%u,%u,%u)\n",eventInfo.mEventId,eventInfo.mId1,eventInfo.mId2,eventInfo.mId3);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::processEvent(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    switch (eventInfo.mType)
    {
      case ContentServer::EventType::UNKNOWN:
        break;

      case ContentServer::EventType::CLEAR:
        event_clear(eventInfo);
        break;

      case ContentServer::EventType::CONTENT_SERVER_RELOAD:
        event_contentServerReload(eventInfo);
        break;

      case ContentServer::EventType::PRODUCER_ADDED:
        event_producerAdded(eventInfo);
        break;

      case ContentServer::EventType::PRODUCER_DELETED:
        event_producerDeleted(eventInfo);
        break;

      case ContentServer::EventType::PRODUCER_LIST_DELETED_BY_SOURCE_ID:
        event_producerListDeletedBySourceId(eventInfo);
        break;

      case ContentServer::EventType::GENERATION_ADDED:
        event_generationAdded(eventInfo);
        break;

      case ContentServer::EventType::GENERATION_DELETED:
        event_generationDeleted(eventInfo);
        break;

      case ContentServer::EventType::GENERATION_STATUS_CHANGED:
        event_generationStatusChanged(eventInfo);
        break;

      case ContentServer::EventType::GENERATION_LIST_DELETED_BY_PRODUCER_ID:
        event_generationListDeletedByProducerId(eventInfo);
        break;

      case ContentServer::EventType::GENERATION_LIST_DELETED_BY_SOURCE_ID:
        event_generationListDeletedBySourceId(eventInfo);
        break;

      case ContentServer::EventType::FILE_ADDED:
        event_fileAdded(eventInfo);
        break;

      case ContentServer::EventType::FILE_DELETED:
        event_fileDeleted(eventInfo);
        break;

      case ContentServer::EventType::FILE_UPDATED:
        event_fileUpdated(eventInfo);
        break;

      case ContentServer::EventType::FILE_LIST_DELETED_BY_GROUP_FLAGS:
        event_fileListDeletedByGroupFlags(eventInfo);
        break;

      case ContentServer::EventType::FILE_LIST_DELETED_BY_PRODUCER_ID:
        event_fileListDeletedByProducerId(eventInfo);
        break;

      case ContentServer::EventType::FILE_LIST_DELETED_BY_GENERATION_ID:
        event_fileListDeletedByGenerationId(eventInfo);
        break;

      case ContentServer::EventType::FILE_LIST_DELETED_BY_SOURCE_ID:
        event_fileListDeletedBySourceId(eventInfo);
        break;

      case ContentServer::EventType::CONTENT_LIST_DELETED_BY_FILE_ID:
        event_contentListDeletedByFileId(eventInfo);
        break;

      case ContentServer::EventType::CONTENT_LIST_DELETED_BY_GROUP_FLAGS:
        event_contentListDeletedByGroupFlags(eventInfo);
        break;

      case ContentServer::EventType::CONTENT_LIST_DELETED_BY_PRODUCER_ID:
        event_contentListDeletedByProducerId(eventInfo);
        break;

      case ContentServer::EventType::CONTENT_LIST_DELETED_BY_GENERATION_ID:
        event_contentListDeletedByGenerationId(eventInfo);
        break;

      case ContentServer::EventType::CONTENT_LIST_DELETED_BY_SOURCE_ID:
        event_contentListDeletedBySourceId(eventInfo);
        break;

      case ContentServer::EventType::DATA_SERVER_ADDED:
        event_dataServerAdded(eventInfo);
        break;

      case ContentServer::EventType::DATA_SERVER_DELETED:
        event_dataServerDeleted(eventInfo);
        break;

      case ContentServer::EventType::CONTENT_ADDED:
        event_contentAdded(eventInfo);
        break;

      case ContentServer::EventType::CONTENT_DELETED:
        event_contentDeleted(eventInfo);
        break;

      case ContentServer::EventType::CONTENT_REGISTERED:
        event_contentRegistered(eventInfo);
        break;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::checkServerRegistration()
{
  FUNCTION_TRACE
  try
  {
    if (mServerId == 0)
      return;

    T::ServerInfo info;
    int result = mContentServer->getDataServerInfoById(mServerSessionId,mServerId,info);
    if (result == 0)
    {
      if (info.mServerIor != mServerIor)
      {
        // It seems that the same server id is registered with different IOR. We should
        // shutdown immediately.

        printf("***** Shutdown the server!\n");
        mShutdownRequested = true;
        return;
      }
    }
    else
    {
      if (result != ContentServer::DATA_NOT_FOUND)
      {
        printf("ERROR: getDataServerInfoById : %d\n",result);
        return;
      }

      if (result == ContentServer::DATA_NOT_FOUND)
      {
        // The server registration cannot be found. Notice that the server and the content
        // registrations will disappear when the content server is reseted. In this case
        // we should re-register them.

        T::ServerInfo serverInfo;
        serverInfo.mServerId = mServerId;
        serverInfo.mName = mServerName;
        serverInfo.mServerIor = mServerIor;

        int result2 = mContentServer->addDataServerInfo(mServerSessionId,serverInfo);
        if (result2 != ContentServer::OK)
        {
          char msg[200];
          sprintf(msg,"ERROR addDataServerInfo (%d) : %s\n",result,ContentServer::getResultString(result2).c_str());
          throw SmartMet::Spine::Exception(BCP,msg);
        }

        //printf("SERVER REGISTERED\n");
        serverInfo.print(std::cout,0,0);

        mFullUpdateRequired = true;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ServiceImplementation::processEvents()
{
  FUNCTION_TRACE
  try
  {
    checkServerRegistration();

    if (mFullUpdateRequired)
    {
      printf("** FULL UPDATE REQUIRED\n");
      fullUpdate();
      return;
    }

    T::EventInfo eventInfo;
    int result = mContentServer->getLastEventInfo(mServerSessionId,mServerId,eventInfo);
    if (result == Result::OK)
    {
      if (eventInfo.mServerTime > mContentServerStartTime)
      {
        printf("*** CONTENT SERVER START TIME CHANGED\n");
        fullUpdate();
        mContentServerStartTime = eventInfo.mServerTime;
        return;
      }
      if (eventInfo.mEventId == mLastProcessedEventId)
      {
        // There are no events to process.
        return;
      }
    }
    else
    {
      // printf("ERROR: getLastEventInfo : %d\n",result);
      return;
    }

    uint len = 1000;
    while (len > 0)
    {
      T::EventInfoList eventInfoList;
      result = mContentServer->getEventInfoList(mServerSessionId,mServerId,mLastProcessedEventId+1,1000,eventInfoList);
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
        mLastProcessedEventId = it->mEventId;
        processEvent(*it);
        it = it->nextItem;

        if (mShutdownRequested)
          return;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceImplementation::eventProcessingThread()
{
  FUNCTION_TRACE
  try
  {
    while (!mShutdownRequested)
    {
      try
      {
        mEventProcessingActive = true;
        processEvents();
      }
      catch (...)
      {
        SmartMet::Spine::Exception exception(BCP,"Operation failed!",NULL);
        exception.printError();
      }

      if (!mShutdownRequested)
        sleep(1);
    }

    mEventProcessingActive = false;
    if (mServerId != 0)
      mContentServer->deleteDataServerInfoById(mServerSessionId,mServerId);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ServiceImplementation::shutdown()
{
  FUNCTION_TRACE
  try
  {
    mShutdownRequested = true;
    while (mEventProcessingActive)
      sleep(1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}

}
}

