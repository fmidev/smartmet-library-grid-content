#include "ServiceImplementation.h"
#include "VirtualContentFactory_type1.h"
#include "VirtualMessage.h"
#include "../../functions/Function_add.h"
#include "../../functions/Function_avg.h"
#include "../../functions/Function_div.h"
#include "../../functions/Function_inPrcnt.h"
#include "../../functions/Function_outPrcnt.h"
#include "../../functions/Function_min.h"
#include "../../functions/Function_max.h"
#include "../../functions/Function_mul.h"
#include "../../functions/Function_multiply.h"
#include "../../functions/Function_sdev.h"
#include "../../functions/Function_sequence.h"
#include "../../functions/Function_sum.h"
#include "../../functions/Function_hypotenuse.h"
#include "../../functions/Function_windDir.h"
#include "../../functions/Function_vectorU.h"
#include "../../functions/Function_vectorV.h"
#include "../../functions/Function_feelsLike.h"

#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/InterpolationFunctions.h>
#include <grid-files/common/ShowFunction.h>
#include <grid-files/common/GraphFunctions.h>
#include <grid-files/common/CoordinateConversions.h>
#include <grid-files/common/MemoryWriter.h>
#include <grid-files/grid/PhysicalGridFile.h>
#include <grid-files/grid/MessageProcessing.h>
#include <grid-files/identification/GridDef.h>
#include <macgyver/StringConversion.h>

#include <signal.h>

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
    return nullptr;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Operation failed!",nullptr);
    exception.printError();
    exit(-1);
  }
}




static void* ServiceImplementation_cacheProcessingThread(void *arg)
{
  try
  {
    ServiceImplementation *service = (ServiceImplementation*)arg;
    service->cacheProcessingThread();
    return nullptr;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Operation failed!",nullptr);
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
    mContentServer = nullptr;

    mLastProcessedEventId = 0;
    mShutdownRequested = false;
    mFullUpdateRequired = false;
    mEventProcessingActive = false;
    mCacheProcessingActive = false;
    mVirtualContentEnabled = false;
    mContentServerStartTime = 0;
    mLastVirtualFileRegistration = 0;
    mFileCleanup_age = 3600;
    mFileCleanup_checkInterval = 60;
    mFileCleanup_time = 0;
    mDeletedFileCleanup_time = 0;
    mLastVirtualFileCheck = time(nullptr);
    mContentChangeTime = 0;
    mStartUpCache_enabled = false;
    mStartUpCache_saveDiskData = false;
    mStartUpCache_saveNetworkData = false;
    mStartUpCache_saveInterval = 30*60;
    mStartUpCache_saveTime = time(0);
    mStartUpCache_maxSize = 30000000000LL;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void ServiceImplementation::init(T::SessionId serverSessionId,uint serverId,const std::string& serverName,const std::string& serverIor,const std::string& dataDir,ContentServer::ServiceInterface *contentServer,string_vec& luaFileNames)
{
  FUNCTION_TRACE
  try
  {
    if (contentServer == nullptr)
      throw Fmi::Exception(BCP,"The 'contentServer' parameter points to nullptr!");

    mServerId = serverId;
    mServerSessionId = serverSessionId;
    mServerName = serverName;
    mServerIor = serverIor;
    mDataDir = dataDir;
    mContentServer = contentServer;
    mFullUpdateRequired = true;

    mVirtualContentManager.init();
    mGridFileManager.init(contentServer);
    mLuaFileCollection.init(luaFileNames);

    mFunctionCollection.addFunction("K2C",new Functions::Function_add(-273.15));

    Functions::Function_sequence *k2f = new Functions::Function_sequence();
    k2f->addFunction(new Functions::Function_add(-273.15));
    k2f->addFunction(new Functions::Function_multiply(1.8));
    k2f->addFunction(new Functions::Function_add(32.0));
    mFunctionCollection.addFunction("K2F",k2f);

    mFunctionCollection.addFunction("SUM",new Functions::Function_sum());
    mFunctionCollection.addFunction("DIV",new Functions::Function_div());
    mFunctionCollection.addFunction("MUL",new Functions::Function_mul());
    mFunctionCollection.addFunction("SDEV",new Functions::Function_sdev());

    mFunctionCollection.addFunction("AVG",new Functions::Function_avg());
    mFunctionCollection.addFunction("MIN",new Functions::Function_min());
    mFunctionCollection.addFunction("MAX",new Functions::Function_max());
    mFunctionCollection.addFunction("IN_PRCNT",new Functions::Function_inPrcnt());
    mFunctionCollection.addFunction("OUT_PRCNT",new Functions::Function_outPrcnt());

    // Radians to degrees
    mFunctionCollection.addFunction("RAD2DEG",new Functions::Function_multiply((360.0/2*3.1415926535)));

    // Degrees to radians
    mFunctionCollection.addFunction("DEG2RAD",new Functions::Function_multiply((2*3.1415926535/360.0)));

    mFunctionCollection.addFunction("WIND_SPEED",new Functions::Function_hypotenuse());
    mFunctionCollection.addFunction("WIND_DIR",new Functions::Function_windDir());
    mFunctionCollection.addFunction("WIND_V",new Functions::Function_vectorV());
    mFunctionCollection.addFunction("WIND_U",new Functions::Function_vectorU());

    mFunctionCollection.addFunction("FEELS_LIKE",new Functions::Function_feelsLike());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::setStartUpCache(bool enabled,bool saveDiskData,bool saveNetworkData,const char *filename,uint saveIntervalInMinutes,long long maxSizeInMegaBytes)
{
  FUNCTION_TRACE
  try
  {
    mStartUpCache_enabled = enabled;
    mStartUpCache_saveDiskData = saveDiskData;
    mStartUpCache_saveNetworkData = saveNetworkData;
    mStartUpCache_filename = filename;
    mStartUpCache_saveInterval = 60*saveIntervalInMinutes;
    mStartUpCache_saveTime = time(0) - mStartUpCache_saveInterval + 600;
    mStartUpCache_maxSize = maxSizeInMegaBytes * 1000000;

    if (mStartUpCache_enabled)
    {
      char fname1[200];
      char fname2[200];
      char nfname1[200];
      char nfname2[200];

      sprintf(fname1,"%s.dat.new",filename);
      sprintf(fname2,"%s.index.new",filename);

      sprintf(nfname1,"%s.dat",filename);
      sprintf(nfname2,"%s.index",filename);

      auto fs1 = getFileSize(fname1);
      auto fs2 = getFileSize(fname2);


      if (fs1 > 0  &&  fs2 > 0)
      {
        time_t modTime1 = getFileModificationTime(fname1);
        if ((time(0) - modTime1) > 7200)
        {
          // New cache is too old.
          remove(fname1);
          remove(fname2);
        }
        else
        {
          rename(fname1,nfname1);
          rename(fname2,nfname2);
        }
      }


      time_t modTime2 = getFileModificationTime(nfname1);
      if ((time(0) - modTime2) > 7200)
        return;    // Cache is too old.

      FILE *file = fopen(nfname2,"r");
      if (!file)
        return;

      while (!feof(file))
      {
        StartUpCacheIndexRec rec;
        fread(&rec,sizeof(rec),1,file);

        unsigned long long key = ((unsigned long long)rec.fileId << 32) + rec.messageIndex;

        mStartUpCache_indexMap.insert(std::pair<unsigned long long,StartUpCacheIndexRec>(key,rec));
      }
      fclose(file);

      mStartUpCache_memoryMapInfo.protocol = 0;
      mStartUpCache_memoryMapInfo.serverType = 0;
      //mStartUpCache_memoryMapInfo.server;
      mStartUpCache_memoryMapInfo.filename = nfname1;
      mStartUpCache_memoryMapInfo.fileSize = getFileSize(nfname1);
      mStartUpCache_memoryMapInfo.allocatedSize = 0;
      mStartUpCache_memoryMapInfo.mappingTime = 0;
      mStartUpCache_memoryMapInfo.memoryPtr = nullptr;
      mStartUpCache_memoryMapInfo.mappingError = false;
      //mStartUpCache_memoryMapInfo.mappedFile;
      memoryMapper.map(mStartUpCache_memoryMapInfo);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ServiceImplementation::shutdown()
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"*** SHUTDOWN ***\n");
    mShutdownRequested = true;
    pthread_join(mEventProcessingThread, nullptr);
    if (mStartUpCache_enabled)
      pthread_join(mCacheProcessingThread, nullptr);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::setLandCover(boost::shared_ptr<Fmi::LandCover> landCover)
{
  try
  {
    mLandCover = landCover;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP, "Operation failed!", nullptr);
    throw exception;
  }
}





void ServiceImplementation::setDem(boost::shared_ptr<Fmi::DEM> dem)
{
  try
  {
    mDem = dem;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP, "Operation failed!", nullptr);
    throw exception;
  }
}





bool ServiceImplementation::isSessionValid(T::SessionId sessionId)
{
  FUNCTION_TRACE
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





void ServiceImplementation::setCleanup(time_t age,time_t checkInterval)
{
  FUNCTION_TRACE
  try
  {
    mFileCleanup_age = age;
    mFileCleanup_checkInterval = checkInterval;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::setVirtualContentEnabled(bool enabled)
{
  FUNCTION_TRACE
  try
  {
    mVirtualContentEnabled = enabled;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::addVirtualContentFactory(VirtualContentFactory *factory)
{
  FUNCTION_TRACE
  try
  {
    factory->setContentServer(mContentServer);
    factory->setGridFileManager(&mGridFileManager);
    factory->setLuaFileCollection(&mLuaFileCollection);
    factory->setFunctionCollection(&mFunctionCollection);

    mVirtualContentManager.addVirtualContentFactory(factory);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ServiceImplementation::addSubServer(std::string& name,ServiceInterface *subServer)
{
  FUNCTION_TRACE
  try
  {
    mDataServers.insert(std::pair<std::string,ServiceInterface*>(name,subServer));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ServiceImplementation::startEventProcessing()
{
  FUNCTION_TRACE
  try
  {
    pthread_create(&mEventProcessingThread,nullptr,ServiceImplementation_eventProcessingThread,this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::startCacheProcessing()
{
  FUNCTION_TRACE
  try
  {
    if (mStartUpCache_enabled)
      pthread_create(&mCacheProcessingThread,nullptr,ServiceImplementation_cacheProcessingThread,this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ServiceInterface* ServiceImplementation::getDataServerByFileId(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfo fileInfo;

    if (mContentServer->getFileInfoById(mServerSessionId,fileId,fileInfo) == 0)
    {
      if (fileInfo.mDeletionTime != 0)
      {
        if ((time(nullptr) + 180) > fileInfo.mDeletionTime)
        {
          // The grid file will be deleted soon. We should not access it anymore.
          return nullptr;
        }
      }

      if (fileInfo.mFileType == T::FileTypeValue::Virtual)
      {
        auto it = mDataServers.find(fileInfo.mServer);
        if (it != mDataServers.end())
          return it->second;
      }
    }
    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





GRID::GridFile_sptr ServiceImplementation::getGridFile(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    GRID::GridFile_sptr gridFile = mGridFileManager.getFileByIdNoMapping(fileId);
    if (gridFile)
    {
      if (gridFile->hasMessagePositionError())
      {
        mGridFileManager.deleteFileById(fileId);
      }
      else
      {
        time_t deletionTime = gridFile->getDeletionTime();
        if (deletionTime == 0)
          return gridFile;

        if ((time(nullptr) + 180) > deletionTime)
        {
          // The grid file will be deleted soon. We should not access it anymore.
          return nullptr;
        }
        else
        {
          if (!gridFile->isMemoryMapped())
            gridFile->mapToMemory();

          gridFile->setAccessTime(time(nullptr));
          return gridFile;
        }
      }
    }

    // If the grid file is not found from the grid storage but it is registered
    // to the contentServer then we should try to add it to the grid storage.

    T::FileInfo fileInfo;

    if (mContentServer->getFileInfoById(mServerSessionId,fileId,fileInfo) == 0)
    {
      if (fileInfo.mDeletionTime != 0)
      {
        if ((time(nullptr) + 180) > fileInfo.mDeletionTime)
        {
          // The grid file will be deleted soon. We should not access it anymore.
          return nullptr;
        }
      }

      if (fileInfo.mFileType != T::FileTypeValue::Virtual  &&  (fileInfo.mProtocol > 1 || getFileSize(fileInfo.mName.c_str()) > 0))
      {
        T::ContentInfoList contentList;
        if (mContentServer->getContentListByFileId(mServerSessionId,fileId,contentList) == 0)
        {
          addFile(fileInfo,contentList);
          gridFile = mGridFileManager.getFileByIdNoMapping(fileId);
          if (!gridFile->isMemoryMapped())
            gridFile->mapToMemory();

          gridFile->setAccessTime(time(nullptr));
        }
      }
    }
    return gridFile;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getMultipleGridValues(T::SessionId sessionId,uint modificationOperation,double_vec& modificationParameters,T::ValueRecordList& valueRecordList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    uint sz = valueRecordList.getLength();
    for (uint t=0; t<sz; t++)
    {
      T::ValueRecord *rec = valueRecordList.getValueRecordByIndex(t);
      if (rec != nullptr)
      {
        GRID::GridFile_sptr gridFile = getGridFile(rec->mFileId);
        if (gridFile)
        {
          GRID::Message *message = gridFile->getMessageByIndex(rec->mMessageIndex);
          if (message)
          {
            try
            {
              switch (rec->mCoordinateType)
              {
                case T::CoordinateTypeValue::UNKNOWN:
                case T::CoordinateTypeValue::LATLON_COORDINATES:
                  rec->mValue = message->getGridValueByLatLonCoordinate(rec->mY,rec->mX,(short)rec->mAreaInterpolationMethod,modificationOperation,modificationParameters);
                  rec->mResult = 0;
                  break;

                case T::CoordinateTypeValue::GRID_COORDINATES:
                  rec->mValue = message->getGridValueByGridPoint_byInterpolation(rec->mX,rec->mY,(short)rec->mAreaInterpolationMethod,modificationOperation,modificationParameters);
                  rec->mResult = 0;
                  break;

                case T::CoordinateTypeValue::ORIGINAL_COORDINATES:
                  // TODO: Implementation required
                  rec->mResult = Result::NOT_IMPLEMENTED;
                  break;
              }
            }
            catch (...)
            {
              Fmi::Exception exception(BCP,"Operation failed!",nullptr);
              exception.addParameter("FileId",Fmi::to_string(rec->mFileId));
              exception.addParameter("MessageIndex",Fmi::to_string(rec->mMessageIndex));
              rec->mResult = Result::UNEXPECTED_EXCEPTION;
              std::string st = exception.getStackTrace();
              PRINT_DATA(mDebugLog,"%s",st.c_str());
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,T::GridCoordinates& coordinates)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    GRID::GridFile_sptr gridFile = getGridFile(fileId);
    if (!gridFile)
    {
      ServiceInterface *dataServer = getDataServerByFileId(fileId);
      if (dataServer)
        return dataServer->getGridCoordinates(sessionId,fileId,messageIndex,coordinateType,coordinates);

      return Result::FILE_NOT_FOUND;
    }

    GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
    if (message == nullptr)
      return Result::MESSAGE_NOT_FOUND;

    coordinates.mProjection = message->getGridProjection();
    message->getGridProjectionAttributes("",coordinates.mProjectionAttributes);
    coordinates.mColumns = message->getGridColumnCount();
    coordinates.mRows = message->getGridRowCount();
    coordinates.mReverseXDirection = message->reverseXDirection();
    coordinates.mReverseYDirection = message->reverseYDirection();
    coordinates.mCoordinateType = coordinateType;

    switch (coordinateType)
    {
      case T::CoordinateTypeValue::UNKNOWN:
      case T::CoordinateTypeValue::LATLON_COORDINATES:
        coordinates.mCoordinateList = *message->getGridLatLonCoordinates();
        break;

      case T::CoordinateTypeValue::GRID_COORDINATES:
        coordinates.mCoordinateList.clear();
        for (uint y=0; y<coordinates.mRows; y++)
        {
          for (uint x=0; x<coordinates.mColumns; x++)
          {
            coordinates.mCoordinateList.emplace_back(x,y);
          }
        }
        break;

      case T::CoordinateTypeValue::ORIGINAL_COORDINATES:
        coordinates.mCoordinateList = *message->getGridOriginalCoordinates();
        break;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    GRID::GridFile_sptr gridFile = getGridFile(fileId);
    if (!gridFile)
    {
      ServiceInterface *dataServer = getDataServerByFileId(fileId);
      if (dataServer)
        return dataServer->getGridData(sessionId,fileId,messageIndex,data);

      return Result::FILE_NOT_FOUND;
    }

    GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
    if (message == nullptr)
      return Result::DATA_NOT_FOUND;

    data.mServerId = mServerId;
    data.mProducerId = gridFile->getProducerId();
    data.mGenerationId = gridFile->getGenerationId();
    data.mFileId = gridFile->getFileId();
    data.mFileType = gridFile->getFileType();
    std::string fname = gridFile->getFileName();
    if (fname.substr(0,1) == "/")
      data.mFileName = gridFile->getFileName().substr(mDataDir.length()+1);
    else
      data.mFileName = fname;
    data.mMessageIndex = messageIndex;
    data.mForecastTime = message->getForecastTime();
    data.mGribParameterId = std::to_string(message->getGribParameterId());
    //data.mGrib1ParameterLevelId = message->getGrib1ParameterLevelId();
    //data.mGrib2ParameterLevelId = message->getGrib2ParameterLevelId();
    data.mGribParameterName = message->getGribParameterName();
    data.mFmiParameterId = std::to_string(message->getFmiParameterId());
    data.mFmiParameterLevelId = message->getFmiParameterLevelId();
    data.mFmiParameterName = message->getFmiParameterName();
    data.mNewbaseParameterId = message->getNewbaseParameterId();
    data.mNewbaseParameterName = message->getNewbaseParameterName();
    data.mParameterLevel = message->getGridParameterLevel();
    data.mGeometryId = message->getGridGeometryId();
    data.mReverseXDirection = message->reverseXDirection();
    data.mReverseYDirection = message->reverseYDirection();
    data.mProjection = message->getGridProjection();
    data.mColumns = message->getGridColumnCount();
    data.mRows = message->getGridRowCount();
    data.mForecastType = message->getForecastType();
    data.mForecastNumber = message->getForecastNumber();
    message->getGridProjectionAttributes("",data.mProjectionAttributes);

    message->getGridValueVector(data.mValues);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridFileCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    count = mGridFileManager.getFileCount();
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridMessageBytes(T::SessionId sessionId,uint fileId,uint messageIndex,std::vector<uchar>& messageBytes,std::vector<uint>& messageSections)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    GRID::GridFile_sptr gridFile = getGridFile(fileId);
    if (!gridFile)
    {
      ServiceInterface *dataServer = getDataServerByFileId(fileId);
      if (dataServer)
        return dataServer->getGridMessageBytes(sessionId,fileId,messageIndex,messageBytes,messageSections);

      return Result::FILE_NOT_FOUND;
    }

    GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
    if (message == nullptr)
      return Result::DATA_NOT_FOUND;

    uchar *fileStartAddr = (uchar*)gridFile->getMemoryPtr();
    if (fileStartAddr == nullptr)
      return Result::DATA_NOT_FOUND;

    long long fileSize = gridFile->getSize();
    uchar *fileEndAddr = fileStartAddr + fileSize;

    T::FilePosition messagePosition = message->getFilePosition();
    uint messageSize = message->getMessageSize();

    uchar *messageStartAddr = fileStartAddr + messagePosition;
    uchar *messageEndAddr = messageStartAddr + messageSize;

    if (messageStartAddr >= fileStartAddr  &&  messageEndAddr <= fileEndAddr)
    {

      messageBytes.clear();
      messageBytes.reserve(messageSize);
      std::copy(messageStartAddr,messageEndAddr,std::back_inserter(messageBytes));

      std::set<T::FilePosition> positions;
      message->getSectionPositions(positions);

      for (auto it = positions.begin(); it != positions.end(); ++it)
      {
        messageSections.emplace_back(*it - messagePosition);
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getPropertyValuesByCoordinates(T::SessionId sessionId,const char *propertyName,T::Coordinate_vec& latlonCoordinates,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    if (strcasecmp(propertyName,"dem") == 0 && mDem)
    {
      values.reserve(latlonCoordinates.size());
      for (auto it = latlonCoordinates.begin(); it != latlonCoordinates.end(); ++it)
      {
        auto val = mDem->elevation(it->x(),it->y());
        values.push_back(val);
      }
      return Result::OK;
    }

    if (strcasecmp(propertyName,"covertype") == 0 && mLandCover)
    {
      values.reserve(latlonCoordinates.size());
      for (auto it = latlonCoordinates.begin(); it != latlonCoordinates.end(); ++it)
      {
        auto val = mLandCover->coverType(it->x(),it->y());
        values.push_back(val);
      }
      return Result::OK;
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    GRID::GridFile_sptr gridFile = getGridFile(fileId);
    if (!gridFile)
    {
      ServiceInterface *dataServer = getDataServerByFileId(fileId);
      if (dataServer)
        return dataServer->getGridAttributeList(sessionId,fileId,messageIndex,attributeList);

      return Result::FILE_NOT_FOUND;
    }

    GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
    if (message == nullptr)
      return Result::DATA_NOT_FOUND;

    message->getAttributeList(std::string(""),attributeList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridProperties(T::SessionId sessionId,uint fileId,uint messageIndex,T::PropertySettingVec& propertyList)
{
  FUNCTION_TRACE
  try
  {
    if (!isSessionValid(sessionId))
      return Result::INVALID_SESSION;

    GRID::GridFile_sptr gridFile = getGridFile(fileId);
    if (!gridFile)
    {
      ServiceInterface *dataServer = getDataServerByFileId(fileId);
      if (dataServer)
        return dataServer->getGridProperties(sessionId,fileId,messageIndex,propertyList);

      return Result::FILE_NOT_FOUND;
    }

    GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
    if (message == nullptr)
      return Result::DATA_NOT_FOUND;

    message->getProperties(propertyList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (!gridFile)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridValueByPoint(sessionId,fileId,messageIndex,coordinateType,x,y,areaInterpolationMethod,modificationOperation,modificationParameters,value);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueByPoint(coordinateType,x,y,areaInterpolationMethod,modificationOperation,modificationParameters,value);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueByLevelAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,int level1,uint fileId2,uint messageIndex2,int level2,double newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (!gridFile1)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueByLevelAndPoint(sessionId,fileId1,messageIndex1,level1,fileId2,messageIndex2,level2,newLevel,coordinateType,x,y,areaInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,value);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueByLevelAndPoint(*message1,*message2,level1,level2,newLevel,coordinateType,x,y,areaInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,value);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueByTimeAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (!gridFile1)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueByTimeAndPoint(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters,value);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueByTimeAndPoint(*message1,*message2,newTime,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters,value);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueByTimeLevelAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,int level1,uint fileId2,uint messageIndex2,int level2,uint fileId3,uint messageIndex3,int level3,uint fileId4,uint messageIndex4,int level4,time_t newTime,double newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueByTimeLevelAndPoint(sessionId,fileId1,messageIndex1,level1,fileId2,messageIndex2,level2,fileId3,messageIndex3,level3,fileId4,messageIndex4,level4,newTime,newLevel,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,value);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;


      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueByTimeLevelAndPoint(*message1,level1,*message2,level2,*message3,level3,*message4,level4,newTime,newLevel,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,value);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       exception.addParameter("FileId3",Fmi::to_string(fileId3));
       exception.addParameter("MessageIndex3",Fmi::to_string(messageIndex3));
       exception.addParameter("FileId4",Fmi::to_string(fileId4));
       exception.addParameter("MessageIndex4",Fmi::to_string(messageIndex4));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridValueVector(sessionId,fileId,messageIndex,modificationOperation,modificationParameters,values);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueVectorWithCaching(modificationOperation,modificationParameters,values);
      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueVectorByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,levelInterpolationMethod,modificationOperation,modificationParameters,values);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueVectorByLevel(*message1,*message2,newLevel,levelInterpolationMethod,modificationOperation,modificationParameters,values);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueVectorByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,timeInterpolationMethod,modificationOperation,modificationParameters,values);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueVectorByTime(*message1,*message2,newTime,timeInterpolationMethod,modificationOperation,modificationParameters,values);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueVectorByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,attributeList,modificationOperation,modificationParameters,values);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueVectorByLevelAndGeometry(*message1,*message2,newLevel,attributeList,modificationOperation,modificationParameters,values);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueVectorByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,attributeList,modificationOperation,modificationParameters,values);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueVectorByTimeAndGeometry(*message1,*message2,newTime,attributeList,modificationOperation,modificationParameters,values);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByCoordinateList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridValueVectorByCoordinateList(sessionId,fileId,messageIndex,coordinateType,coordinates,areaInterpolationMethod,modificationOperation,modificationParameters,values);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueVectorByCoordinateList(coordinateType,coordinates,areaInterpolationMethod,modificationOperation,modificationParameters,values);
      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByLevelAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueVectorByLevelAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,coordinates,attributeList,modificationOperation,modificationParameters,values);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueVectorByLevelAndCoordinateList(*message1,*message2,newLevel,coordinateType,coordinates,attributeList,modificationOperation,modificationParameters,values);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByTimeAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueVectorByTimeAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,coordinates,attributeList,modificationOperation,modificationParameters,values);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueVectorByTimeAndCoordinateList(*message1,*message2,newTime,coordinateType,coordinates,attributeList,modificationOperation,modificationParameters,values);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueVectorByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,values);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;


      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueVectorByTimeAndLevel(*message1,*message2,*message3,*message4,newTime,newLevel,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,values);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       exception.addParameter("FileId3",Fmi::to_string(fileId3));
       exception.addParameter("MessageIndex3",Fmi::to_string(messageIndex3));
       exception.addParameter("FileId4",Fmi::to_string(fileId4));
       exception.addParameter("MessageIndex4",Fmi::to_string(messageIndex4));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueVectorByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,attributeList,modificationOperation,modificationParameters,values);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueVectorByTimeLevelAndGeometry(*message1,*message2,*message3,*message4,newTime,newLevel,attributeList,modificationOperation,modificationParameters,values);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       exception.addParameter("FileId3",Fmi::to_string(fileId3));
       exception.addParameter("MessageIndex3",Fmi::to_string(messageIndex3));
       exception.addParameter("FileId4",Fmi::to_string(fileId4));
       exception.addParameter("MessageIndex4",Fmi::to_string(messageIndex4));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByTimeLevelAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueVectorByTimeLevelAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,coordinates,attributeList,modificationOperation,modificationParameters,values);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;


      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueVectorByTimeLevelAndCoordinateList(*message1,*message2,*message3,*message4,newTime,newLevel,coordinateType,coordinates,attributeList,modificationOperation,modificationParameters,values);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       exception.addParameter("FileId3",Fmi::to_string(fileId3));
       exception.addParameter("MessageIndex3",Fmi::to_string(messageIndex3));
       exception.addParameter("FileId4",Fmi::to_string(fileId4));
       exception.addParameter("MessageIndex4",Fmi::to_string(messageIndex4));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridValueVectorByGeometry(sessionId,fileId,messageIndex,attributeList,modificationOperation,modificationParameters,values);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueVectorByGeometry(attributeList,modificationOperation,modificationParameters,values);
      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridLatlonCoordinatesByGeometry(T::SessionId sessionId,T::AttributeList& attributeList,T::GridCoordinates& coordinates)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      uint width = 0;
      uint height = 0;
      T::Coordinate_svec latLonCoordinates;
      Identification::gridDef.getGridLatLonCoordinatesByGeometry(attributeList,latLonCoordinates,width,height);

      if (!latLonCoordinates || latLonCoordinates->size() == 0)
        return Result::DATA_NOT_FOUND;

      coordinates.mCoordinateList = *latLonCoordinates;
      coordinates.mColumns = width;
      coordinates.mRows = height;
      coordinates.mCoordinateType = T::CoordinateTypeValue::LATLON_COORDINATES;
      if (coordinates.mCoordinateList[0].y() > coordinates.mCoordinateList[coordinates.mCoordinateList.size()-1].y())
        coordinates.mReverseYDirection = true;

      attributeList.setAttribute("grid.width",Fmi::to_string(width));
      attributeList.setAttribute("grid.height",Fmi::to_string(height));
      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double origoX,double origoY,double radius,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridValueListByCircle(sessionId,fileId,messageIndex,coordinateType,origoX,origoY,radius,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueListByCircle(coordinateType,origoX,origoY,radius,modificationOperation,modificationParameters,valueList);
      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueListByTimeAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueListByTimeAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,origoX,origoY,radius,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;


      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueListByTimeAndCircle(*message1,*message2,newTime,coordinateType,origoX,origoY,radius,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);

      return Result::OK;
    }
    catch (...)
    {
      Fmi::Exception exception(BCP,"Operation failed!",nullptr);
      exception.addParameter("FileId1",Fmi::to_string(fileId1));
      exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
      exception.addParameter("FileId2",Fmi::to_string(fileId2));
      exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
      std::string st = exception.getStackTrace();
      PRINT_DATA(mDebugLog,"%s",st.c_str());
      return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueListByLevelAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueListByLevelAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,origoX,origoY,radius,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueListByLevelAndCircle(*message1,*message2,newLevel,coordinateType,origoX,origoY,radius,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);

      return Result::OK;
    }
    catch (...)
    {
      Fmi::Exception exception(BCP,"Operation failed!",nullptr);
      exception.addParameter("FileId1",Fmi::to_string(fileId1));
      exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
      exception.addParameter("FileId2",Fmi::to_string(fileId2));
      exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
      std::string st = exception.getStackTrace();
      PRINT_DATA(mDebugLog,"%s",st.c_str());
      return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueListByTimeLevelAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueListByTimeLevelAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,origoX,origoY,radius,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;


      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueListByTimeLevelAndCircle(*message1,*message2,*message3,*message4,newTime,newLevel,coordinateType,origoX,origoY,radius,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       exception.addParameter("FileId3",Fmi::to_string(fileId3));
       exception.addParameter("MessageIndex3",Fmi::to_string(messageIndex3));
       exception.addParameter("FileId4",Fmi::to_string(fileId4));
       exception.addParameter("MessageIndex4",Fmi::to_string(messageIndex4));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridValueVectorByRectangle(sessionId,fileId,messageIndex,coordinateType,columns,rows,x,y,xStep,yStep,areaInterpolationMethod,modificationOperation,modificationParameters,values);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      switch (coordinateType)
      {
        case T::CoordinateTypeValue::UNKNOWN:
        case T::CoordinateTypeValue::LATLON_COORDINATES:
          for (uint r=0; r < rows; r++)
          {
            double xx = x;
            for (uint c=0; c < columns; c++)
            {
              T::ParamValue value = message->getGridValueByLatLonCoordinate(y,xx,areaInterpolationMethod,modificationOperation,modificationParameters);
              values.emplace_back(value);
              xx = xx + xStep;
            }
            y = y + yStep;
          }
          return Result::OK;

        case T::CoordinateTypeValue::GRID_COORDINATES:
          for (uint r=0; r < rows; r++)
          {
            double xx = x;
            for (uint c=0; c < columns; c++)
            {
              T::ParamValue value = message->getGridValueByGridPoint_byInterpolation(xx,y,areaInterpolationMethod,modificationOperation,modificationParameters);
              values.emplace_back(value);
              xx = xx + xStep;
            }
            y = y + yStep;
          }
          return Result::OK;

        case T::CoordinateTypeValue::ORIGINAL_COORDINATES:
          // TODO: Implementation required
          return Result::NOT_IMPLEMENTED;
      }
      return Result::UNKNOW_COORDINATE_TYPE;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridValueListByPointList(sessionId,fileId,messageIndex,coordinateType,pointList,areaInterpolationMethod,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueListByPointList(coordinateType,pointList,areaInterpolationMethod,modificationOperation,modificationParameters,valueList);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       exception.printError();
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueListByLevelAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueListByLevelAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,pointList,areaInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueListByLevelAndPointList(*message1,*message2,newLevel,coordinateType,pointList,areaInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueListByTimeAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueListByTimeAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,pointList,areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueListByTimeAndPointList(*message1,*message2,newTime,coordinateType,pointList,areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueListByTimeLevelAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueListByTimeLevelAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,pointList,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueListByTimeLevelAndPointList(*message1,*message2,*message3,*message4,newTime,newLevel,coordinateType,pointList,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       exception.addParameter("FileId3",Fmi::to_string(fileId3));
       exception.addParameter("MessageIndex3",Fmi::to_string(messageIndex3));
       exception.addParameter("FileId4",Fmi::to_string(fileId4));
       exception.addParameter("MessageIndex4",Fmi::to_string(messageIndex4));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridValueListByPolygon(sessionId,fileId,messageIndex,coordinateType,polygonPoints,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueListByPolygon(coordinateType,polygonPoints,modificationOperation,modificationParameters,valueList);
      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueListByLevelAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueListByLevelAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,polygonPoints,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueListByLevelAndPolygon(*message1,*message2,newLevel,coordinateType,polygonPoints,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueListByTimeAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueListByTimeAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,polygonPoints,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueListByTimeAndPolygon(*message1,*message2,newTime,coordinateType,polygonPoints,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueListByTimeLevelAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueListByTimeLevelAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,polygonPoints,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueListByTimeLevelAndPolygon(*message1,*message2,*message3,*message4,newTime,newLevel,coordinateType,polygonPoints,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridValueListByPolygonPath(sessionId,fileId,messageIndex,coordinateType,polygonPath,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueListByPolygonPath(coordinateType,polygonPath,modificationOperation,modificationParameters,valueList);
      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueListByTimeAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueListByTimeAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,polygonPath,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueListByTimeAndPolygonPath(*message1,*message2,newTime,coordinateType,polygonPath,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueListByLevelAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueListByLevelAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,polygonPath,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueListByLevelAndPolygonPath(*message1,*message2,newLevel,coordinateType,polygonPath,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






int ServiceImplementation::_getGridValueListByTimeLevelAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridValueListByTimeLevelAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,polygonPath,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridValueListByTimeLevelAndPolygonPath(*message1,*message2,*message3,*message4,newTime,newLevel,coordinateType,polygonPath,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridValueListByRectangle(sessionId,fileId,messageIndex,coordinateType,x1,y1,x2,y2,modificationOperation,modificationParameters,valueList);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      bool gridRectangle = false;
      message->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,gridRectangle,modificationOperation,modificationParameters,valueList);
      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,uint vectorType,uint modificationOperation,double_vec& modificationParameters,double_vec& valueVector)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridValueVectorByPoint(sessionId,fileId,messageIndex,coordinateType,x,y,vectorType,modificationOperation,modificationParameters,valueVector);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueVectorByPoint(coordinateType,x,y,vectorType,modificationOperation,modificationParameters,valueVector);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsobands(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridIsobands(sessionId,fileId,messageIndex,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridIsobands(contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridIsobandsByGeometry(sessionId,fileId,messageIndex,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridIsobandsByGeometry(contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridIsobandsByGrid(sessionId,fileId,messageIndex,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridIsobandsByGrid(contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsobandsByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsobandsByLevel(*message1,*message2,newLevel,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsobandsByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsobandsByTime(*message1,*message2,newTime,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsobandsByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsobandsByLevelAndGeometry(*message1,*message2,newLevel,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }

  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






int ServiceImplementation::_getGridIsobandsByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsobandsByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsobandsByTimeAndGeometry(*message1,*message2,newTime,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }

  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsobandsByLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      if (gridLatLonCoordinates.size() == 0 ||  gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_NUMBER_OF_COORDINATES;

      if (gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_DIMENSIONS;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsobandsByLevelAndGrid(*message1,*message2,newLevel,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsobandsByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      if (gridLatLonCoordinates.size() == 0 ||  gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_NUMBER_OF_COORDINATES;

      if (gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_DIMENSIONS;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsobandsByTimeAndGrid(*message1,*message2,newTime,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsobandsByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;


      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsobandsByTimeAndLevel(*message1,*message2,*message3,*message4,newTime,newLevel,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       exception.addParameter("FileId3",Fmi::to_string(fileId3));
       exception.addParameter("MessageIndex3",Fmi::to_string(messageIndex3));
       exception.addParameter("FileId4",Fmi::to_string(fileId4));
       exception.addParameter("MessageIndex4",Fmi::to_string(messageIndex4));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsobandsByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;


      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsobandsByTimeLevelAndGeometry(*message1,*message2,*message3,*message4,newTime,newLevel,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       exception.addParameter("FileId3",Fmi::to_string(fileId3));
       exception.addParameter("MessageIndex3",Fmi::to_string(messageIndex3));
       exception.addParameter("FileId4",Fmi::to_string(fileId4));
       exception.addParameter("MessageIndex4",Fmi::to_string(messageIndex4));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsobandsByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;


      if (gridLatLonCoordinates.size() == 0 ||  gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_NUMBER_OF_COORDINATES;

      if (gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_DIMENSIONS;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsobandsByTimeLevelAndGrid(*message1,*message2,*message3,*message4,newTime,newLevel,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       exception.addParameter("FileId3",Fmi::to_string(fileId3));
       exception.addParameter("MessageIndex3",Fmi::to_string(messageIndex3));
       exception.addParameter("FileId4",Fmi::to_string(fileId4));
       exception.addParameter("MessageIndex4",Fmi::to_string(messageIndex4));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsolinesByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourValues,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;


      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsolinesByTimeAndLevel(*message1,*message2,*message3,*message4,newTime,newLevel,contourValues,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       exception.addParameter("FileId3",Fmi::to_string(fileId3));
       exception.addParameter("MessageIndex3",Fmi::to_string(messageIndex3));
       exception.addParameter("FileId4",Fmi::to_string(fileId4));
       exception.addParameter("MessageIndex4",Fmi::to_string(messageIndex4));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsolinesByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourValues,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsolinesByTimeLevelAndGeometry(*message1,*message2,*message3,*message4,newTime,newLevel,contourValues,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       exception.addParameter("FileId3",Fmi::to_string(fileId3));
       exception.addParameter("MessageIndex3",Fmi::to_string(messageIndex3));
       exception.addParameter("FileId4",Fmi::to_string(fileId4));
       exception.addParameter("MessageIndex4",Fmi::to_string(messageIndex4));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsolines(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridIsolines(sessionId,fileId,messageIndex,contourValues,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridIsolines(contourValues,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridIsolinesByGeometry(sessionId,fileId,messageIndex,contourValues,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridIsolinesByGeometry(contourValues,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridIsolinesByGrid(sessionId,fileId,messageIndex,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridIsolinesByGrid(contourValues,gridWidth,gridHeight,gridLatLonCoordinates,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsolinesByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourValues,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsolinesByLevel(*message1,*message2,newLevel,contourValues,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsolinesByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourValues,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsolinesByTime(*message1,*message2,newTime,contourValues,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsolinesByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourValues,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsolinesByLevelAndGeometry(*message1,*message2,newLevel,contourValues,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }

  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsolinesByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourValues,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsolinesByTimeAndGeometry(*message1,*message2,newTime,contourValues,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }

  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsolinesByLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      if (gridLatLonCoordinates.size() == 0 ||  gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_NUMBER_OF_COORDINATES;

      if (gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_DIMENSIONS;


      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsolinesByLevelAndGrid(*message1,*message2,newLevel,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsolinesByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      if (gridLatLonCoordinates.size() == 0 ||  gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_NUMBER_OF_COORDINATES;

      if (gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_DIMENSIONS;


      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsolinesByTimeAndGrid(*message1,*message2,newTime,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridIsolinesByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4, messageIndex4,newTime,newLevel,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;


      if (gridLatLonCoordinates.size() == 0 ||  gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_NUMBER_OF_COORDINATES;

      if (gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_DIMENSIONS;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridIsolinesByTimeLevelAndGrid(*message1,*message2,*message3,*message4,newTime,newLevel,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       exception.addParameter("FileId3",Fmi::to_string(fileId3));
       exception.addParameter("MessageIndex3",Fmi::to_string(messageIndex3));
       exception.addParameter("FileId4",Fmi::to_string(fileId4));
       exception.addParameter("MessageIndex4",Fmi::to_string(messageIndex4));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridStreamlines(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridStreamlines(sessionId,fileId,messageIndex,attributeList,modificationOperation,modificationParameters,streamlines);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridStreamlines(attributeList,modificationOperation,modificationParameters,streamlines);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridStreamlinesByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridStreamlinesByGeometry(sessionId,fileId,messageIndex,attributeList,modificationOperation,modificationParameters,streamlines);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridStreamlinesByGeometry(attributeList,modificationOperation,modificationParameters,streamlines);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_getGridStreamlinesByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId);
        if (dataServer)
          return dataServer->getGridStreamlinesByGrid(sessionId,fileId,messageIndex,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,streamlines);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridStreamlinesByGrid(gridWidth,gridHeight,gridLatLonCoordinates,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,streamlines);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridStreamlinesByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridStreamlinesByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,attributeList,modificationOperation,modificationParameters,streamlines);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridStreamlinesByLevel(*message1,*message2,newLevel,attributeList,modificationOperation,modificationParameters,streamlines);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridStreamlinesByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridStreamlinesByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,attributeList,modificationOperation,modificationParameters,streamlines);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridStreamlinesByLevelAndGeometry(*message1,*message2,newLevel,attributeList,modificationOperation,modificationParameters,streamlines);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridStreamlinesByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridStreamlinesByLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,streamlines);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      if (gridLatLonCoordinates.size() == 0 ||  gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_NUMBER_OF_COORDINATES;

      if (gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_DIMENSIONS;


      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridStreamlinesByLevelAndGrid(*message1,*message2,newLevel,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,streamlines);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridStreamlinesByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridStreamlinesByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,attributeList,modificationOperation,modificationParameters,streamlines);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridStreamlinesByTime(*message1,*message2,newTime,attributeList,modificationOperation,modificationParameters,streamlines);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridStreamlinesByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridStreamlinesByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,attributeList,modificationOperation,modificationParameters,streamlines);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridStreamlinesByTimeAndGeometry(*message1,*message2,newTime,attributeList,modificationOperation,modificationParameters,streamlines);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }

  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}









int ServiceImplementation::_getGridStreamlinesByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridStreamlinesByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,streamlines);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      if (gridLatLonCoordinates.size() == 0 ||  gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_NUMBER_OF_COORDINATES;

      if (gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_DIMENSIONS;


      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridStreamlinesByTimeAndGrid(*message1,*message2,newTime,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,streamlines);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex1));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}







int ServiceImplementation::_getGridStreamlinesByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridStreamlinesByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,attributeList,modificationOperation,modificationParameters,streamlines);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;


      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridStreamlinesByTimeAndLevel(*message1,*message2,*message3,*message4,newTime,newLevel,attributeList,modificationOperation,modificationParameters,streamlines);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       exception.addParameter("FileId3",Fmi::to_string(fileId3));
       exception.addParameter("MessageIndex3",Fmi::to_string(messageIndex3));
       exception.addParameter("FileId4",Fmi::to_string(fileId4));
       exception.addParameter("MessageIndex4",Fmi::to_string(messageIndex4));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridStreamlinesByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridStreamlinesByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,attributeList,modificationOperation,modificationParameters,streamlines);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridStreamlinesByTimeLevelAndGeometry(*message1,*message2,*message3,*message4,newTime,newLevel,attributeList,modificationOperation,modificationParameters,streamlines);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       exception.addParameter("FileId3",Fmi::to_string(fileId3));
       exception.addParameter("MessageIndex3",Fmi::to_string(messageIndex3));
       exception.addParameter("FileId4",Fmi::to_string(fileId4));
       exception.addParameter("MessageIndex4",Fmi::to_string(messageIndex4));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getGridStreamlinesByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      if (!isSessionValid(sessionId))
        return Result::INVALID_SESSION;

      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
      {
        ServiceInterface *dataServer = getDataServerByFileId(fileId1);
        if (dataServer)
          return dataServer->getGridStreamlinesByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,streamlines);

        return Result::FILE_NOT_FOUND;
      }

      GRID::Message *message1 = gridFile1->getMessageByIndex(messageIndex1);
      if (message1 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile2 = getGridFile(fileId2);
      if (gridFile2 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message2 = gridFile2->getMessageByIndex(messageIndex2);
      if (message2 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile3 = getGridFile(fileId3);
      if (gridFile3 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message3 = gridFile3->getMessageByIndex(messageIndex3);
      if (message3 == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      GRID::GridFile_sptr gridFile4 = getGridFile(fileId4);
      if (gridFile4 == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message4 = gridFile4->getMessageByIndex(messageIndex4);
      if (message4 == nullptr)
        return Result::MESSAGE_NOT_FOUND;


      if (gridLatLonCoordinates.size() == 0 ||  gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_NUMBER_OF_COORDINATES;

      if (gridWidth == 0 || gridHeight == 0)
        return Result::INVALID_DIMENSIONS;

      GRID::MessageProcessing messageProcessing;
      messageProcessing.getGridStreamlinesByTimeLevelAndGrid(*message1,*message2,*message3,*message4,newTime,newLevel,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,streamlines);

      return Result::OK;
    }
    catch (...)
    {
       Fmi::Exception exception(BCP,"Operation failed!",nullptr);
       exception.addParameter("FileId1",Fmi::to_string(fileId1));
       exception.addParameter("MessageIndex1",Fmi::to_string(messageIndex1));
       exception.addParameter("FileId2",Fmi::to_string(fileId2));
       exception.addParameter("MessageIndex2",Fmi::to_string(messageIndex2));
       exception.addParameter("FileId3",Fmi::to_string(fileId3));
       exception.addParameter("MessageIndex3",Fmi::to_string(messageIndex3));
       exception.addParameter("FileId4",Fmi::to_string(fileId4));
       exception.addParameter("MessageIndex4",Fmi::to_string(messageIndex4));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ServiceImplementation::readContentList(T::ContentInfoList& contentList,bool includePhysicalContent,bool includeVirtualContent)
{
  FUNCTION_TRACE
  try
  {
    uint startFileId = 0;
    uint startMessageIndex = 0;
    uint len = 50000;
    uint counter = 0;
    while (len > 0)
    {
      T::ContentInfoList contentInfoList;

      int result = mContentServer->getContentList(0,startFileId,startMessageIndex,50000,contentInfoList);
      if (result != 0)
      {
        Fmi::Exception exception(BCP,"Cannot read the content list from the content storage!");
        exception.addParameter("ServiceResult",ContentServer::getResultString(result));
        throw exception;
      }

      len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
        startFileId = contentInfo->mFileId;
        startMessageIndex = contentInfo->mMessageIndex + 1;

        if (includePhysicalContent && contentInfo->mFileType != T::FileTypeValue::Virtual)
          contentList.addContentInfo(contentInfo->duplicate());
        else
        if (includeVirtualContent && contentInfo->mFileType == T::FileTypeValue::Virtual)
          contentList.addContentInfo(contentInfo->duplicate());

        counter++;
        if ((counter % 10000) == 0)
          PRINT_DATA(mDebugLog,"* Caching content information : %u\n",counter);
      }
    }
    PRINT_DATA(mDebugLog,"* Caching content information : %u\n",counter);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::registerVirtualFiles(VirtualGridFilePtr_map& gridFileMap,std::set<uint>& idList)
{
  FUNCTION_TRACE
  try
  {
    if (!mVirtualContentEnabled)
      return;

    mLastVirtualFileRegistration = time(nullptr);

    if (gridFileMap.size() == 0)
      return;

    std::vector<T::FileAndContent> fileAndContentList;

    uint c = 0;
    for (auto it = gridFileMap.begin(); it != gridFileMap.end(); ++it)
    {
      if (mShutdownRequested)
        return;

      auto virtualFilePtr = it->second;
      //printf(" -- register %u %s\n",virtualFilePtr->getFileId(),virtualFilePtr->getFileName().c_str());
      if (virtualFilePtr->getFileId() == 0)
      {
        T::FileAndContent fc;
        fc.mFileInfo.mName = virtualFilePtr->getFileName();
        fc.mFileInfo.mProducerId = virtualFilePtr->getProducerId();
        fc.mFileInfo.mGenerationId = virtualFilePtr->getGenerationId();
        fc.mFileInfo.mSourceId = mServerId;
        fc.mFileInfo.mServer = mServerName;
        fc.mFileInfo.mFileType = T::FileTypeValue::Virtual;
        fc.mFileInfo.mFlags = fc.mFileInfo.mFlags | T::FileInfo::Flags::VirtualContent;

        std::size_t len = virtualFilePtr->getNumberOfMessages();
        for (std::size_t t = 0; t<len; t++)
        {
          GRID::VirtualMessage *msg = (GRID::VirtualMessage*)virtualFilePtr->getMessageByIndex(t);
          T::ContentInfo *contentInfo = msg->getContentInfo();
          contentInfo->mSourceId = mServerId;
          fc.mContentInfoList.addContentInfo(contentInfo->duplicate());
        }

        fileAndContentList.emplace_back(fc);

        if (fileAndContentList.size() > 50000)
        {
          mContentServer->addFileInfoListWithContent(mServerSessionId,0,fileAndContentList);
          PRINT_DATA(mDebugLog,"* Registering virtual files : %u\n",c);

          for (auto ff = fileAndContentList.begin(); ff != fileAndContentList.end(); ++ff)
          {
            if (mShutdownRequested)
              return;

            if (ff->mFileInfo.mFileId > 0)
            {
              auto tt = gridFileMap.find(ff->mFileInfo.mName);
              if (tt != gridFileMap.end())
              {
                auto virtualFilePtr = tt->second;
                virtualFilePtr->setFileId(ff->mFileInfo.mFileId);

                std::size_t fLen = virtualFilePtr->getNumberOfPhysicalGridFiles();
                for (uint f=0; f<fLen; f++)
                {
                  GRID::GridFile_sptr fFile = virtualFilePtr->getPhysicalGridFileByIndex(f);
                  fFile->addUser(ff->mFileInfo.mFileId);
                }

                //printf(" -- new %u %s\n",virtualFilePtr->getFileId(),virtualFilePtr->getFileName().c_str());
                idList.insert(virtualFilePtr->getFileId());

                mGridFileManager.addFile(virtualFilePtr);
              }
            }
          }
          fileAndContentList.clear();
        }
      }
      else
      {
        std::size_t fLen = virtualFilePtr->getNumberOfPhysicalGridFiles();
        for (uint f=0; f<fLen; f++)
        {
          GRID::GridFile_sptr fFile = virtualFilePtr->getPhysicalGridFileByIndex(f);
          fFile->addUser(virtualFilePtr->getFileId());
        }
        mGridFileManager.addFile(virtualFilePtr);
        idList.insert(virtualFilePtr->getFileId());
        //printf(" -- add %u %s\n",virtualFilePtr->getFileId(),virtualFilePtr->getFileName().c_str());
      }
      c++;
    }

    if (fileAndContentList.size() > 0)
    {
      mContentServer->addFileInfoListWithContent(mServerSessionId,0,fileAndContentList);
      PRINT_DATA(mDebugLog,"* Registering virtual files : %u\n",c);

      for (auto ff = fileAndContentList.begin(); ff != fileAndContentList.end(); ++ff)
      {
        if (mShutdownRequested)
          return;

        if (ff->mFileInfo.mFileId > 0)
        {
          auto tt = gridFileMap.find(ff->mFileInfo.mName);
          if (tt != gridFileMap.end())
          {
            auto virtualFilePtr = tt->second;
            virtualFilePtr->setFileId(ff->mFileInfo.mFileId);

            std::size_t fLen = virtualFilePtr->getNumberOfPhysicalGridFiles();
            for (uint f=0; f<fLen; f++)
            {
              GRID::GridFile_sptr fFile = virtualFilePtr->getPhysicalGridFileByIndex(f);
              fFile->addUser(ff->mFileInfo.mFileId);
            }

            //printf(" -- new %u %s\n",virtualFilePtr->getFileId(),virtualFilePtr->getFileName().c_str());

            idList.insert(virtualFilePtr->getFileId());
            mGridFileManager.addFile(virtualFilePtr);
          }
        }
      }
      fileAndContentList.clear();
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::updateVirtualFiles(T::ContentInfoList& fullContentList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"Update virtual files (%d)\n",(int)mVirtualContentEnabled);
    if (!mVirtualContentEnabled)
      return;

    T::ProducerInfoList producerInfoList;
    int result = mContentServer->getProducerInfoList(mServerSessionId,producerInfoList);
    if (result != 0)
    {
      PRINT_DATA(mDebugLog,"%s:%d: Cannot get the producer list from the content server!\n",__FILE__,__LINE__);
      PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
      return;
    }

    T::GenerationInfoList generationInfoList;
    result = mContentServer->getGenerationInfoList(mServerSessionId,generationInfoList);
    if (result != 0)
    {
      PRINT_DATA(mDebugLog,"%s:%d Cannot get the generation list from the content server!\n",__FILE__,__LINE__);
      PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
      return;
    }

    VirtualGridFilePtr_map gridFileMap;

    std::set<uint> virtualFileIdList;

    uint counter = 0;
    uint startFileId = 0;
    uint len = 50000;
    while (len > 0)
    {
      if (mShutdownRequested)
        return;

      T::FileInfoList fileInfoList;

      result = mContentServer->getFileInfoList(mServerSessionId,startFileId,10000,fileInfoList);
      if (result != 0)
      {
        PRINT_DATA(mDebugLog,"%s:%d: Cannot get the file list from the content server!\n",__FILE__,__LINE__);
        PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
        return;
      }

      len = fileInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        if (mShutdownRequested)
          return;

        try
        {
          T::FileInfo *fileInfo = fileInfoList.getFileInfoByIndex(t);
          if (fileInfo->mFileId >= startFileId)
            startFileId = fileInfo->mFileId + 1;

          if (fileInfo->mFileType != T::FileTypeValue::Virtual)
          {
            T::ContentInfoList contentInfoList;
            if (mContentServer->getImplementationType() != ContentServer::Implementation::Cache || fullContentList.getLength() > 0)
              fullContentList.getContentInfoListByFileId(fileInfo->mFileId,contentInfoList);
            else
              mContentServer->getContentListByFileId(mServerSessionId,fileInfo->mFileId,contentInfoList);

            T::ProducerInfo *producerInfo = producerInfoList.getProducerInfoById(fileInfo->mProducerId);
            if (producerInfo != nullptr)
            {
              T::GenerationInfo *generationInfo = generationInfoList.getGenerationInfoById(fileInfo->mGenerationId);
              if (generationInfo != nullptr)
              {
                mVirtualContentManager.addFile(*producerInfo,*generationInfo,*fileInfo,contentInfoList,gridFileMap);
              }
              else
              {
                PRINT_DATA(mDebugLog,"%s:%d: Cannot find the generation (%u)!\n",__FILE__,__LINE__,fileInfo->mGenerationId);
              }
            }
            else
            {
              PRINT_DATA(mDebugLog,"%s:%d: Cannot find the producer (%u)!\n",__FILE__,__LINE__,fileInfo->mProducerId);
            }

            counter++;
            if ((counter % 10000) == 0)
              PRINT_DATA(mDebugLog,"* Processing files : %lu\n",gridFileMap.size());
          }
          else
          {
            if (fileInfo->mSourceId == mServerId)
              virtualFileIdList.insert(fileInfo->mFileId);
          }
        }
        catch (...)
        {
          Fmi::Exception exception(BCP,"Operation failed!",nullptr);
          std::string st = exception.getStackTrace();
          PRINT_DATA(mDebugLog,"%s",st.c_str());
        }
      }
    }
    PRINT_DATA(mDebugLog,"* Creating virtual files : %lu\n",gridFileMap.size());

    std::set<uint> idList;
    registerVirtualFiles(gridFileMap,idList);

    //mGridFileManager.getVirtualFiles(idList);

    PRINT_DATA(mDebugLog,"* Checking old virtual file registrations (%lu/%lu)\n",idList.size(),virtualFileIdList.size());

    std::set<uint> fileDeleteList;
    for (auto it = virtualFileIdList.begin(); it != virtualFileIdList.end(); ++it)
    {
      if (idList.find(*it) == idList.end())
      {
        fileDeleteList.insert(*it);
      }
    }

    if (fileDeleteList.size() > 0)
    {
      PRINT_DATA(mDebugLog,"* Removing virtual files (%lu)\n",fileDeleteList.size());
      mContentServer->deleteFileInfoListByFileIdList(mServerSessionId,fileDeleteList);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::addFile(T::FileInfo& fileInfo,T::ContentInfoList& contentList)
{
  FUNCTION_TRACE
  try
  {
    if (mShutdownRequested)
      return;

    AutoThreadLock lock(&mThreadLock);

    if (fileInfo.mFileType == T::FileTypeValue::Virtual || (fileInfo.mFlags & T::FileInfo::Flags::VirtualContent) != 0)
      return;

    time_t checkTime = time(nullptr);
    std::string filename;
    if (!mDataDir.empty()  &&  mDataDir != "/")
      filename = mDataDir + "/" + fileInfo.mName;
    else
      filename = fileInfo.mName;

    GRID::GridFile_sptr storageFile = mGridFileManager.getFileByIdNoMapping(fileInfo.mFileId);

    GRID::GridFile *gridFile = nullptr;
    if (storageFile)
    {
      // File already in the content storage
      gridFile = storageFile.get();

      if (gridFile->getFileName() != filename)
      {
        // The filename does not match to the found grid file. We should remove the old file.

        // std::cout << "DELETING FILE INFO : " << gridFile->getFileName() << " : " << filename << "\n";
        mGridFileManager.deleteFileById(fileInfo.mFileId);
        gridFile = nullptr;
      }
    }

    if (gridFile == nullptr)
    {
      gridFile = new GRID::PhysicalGridFile();
      gridFile->setFileName(filename);
      gridFile->setFileId(fileInfo.mFileId);
      gridFile->setProducerId(fileInfo.mProducerId);
      gridFile->setGenerationId(fileInfo.mGenerationId);
      gridFile->setSourceId(fileInfo.mSourceId);
      gridFile->setDeletionTime(fileInfo.mDeletionTime);
      gridFile->setProtocol(fileInfo.mProtocol);
      gridFile->setServerType(fileInfo.mServerType);
      gridFile->setServer(fileInfo.mServer);
      gridFile->setSize(fileInfo.mSize);
    }

    T::ProducerInfo producerInfo;
    T::GenerationInfo generationInfo;

    if (mVirtualContentEnabled)
    {
      mContentServer->getProducerInfoById(mServerSessionId,fileInfo.mProducerId,producerInfo);
      mContentServer->getGenerationInfoById(mServerSessionId,fileInfo.mGenerationId,generationInfo);
    }


    uint cLen = contentList.getLength();

    if (gridFile->getNumberOfMessages() < cLen)
    {
      for (uint t=0; t<cLen; t++)
      {
        if (mShutdownRequested)
          return;

        T::ContentInfo *info = contentList.getContentInfoByIndex(t);

        GRID::MessageInfo mInfo;
        mInfo.mFileMemoryPtr = nullptr;
        mInfo.mFilePosition = info->mFilePosition;

        if (mStartUpCache_enabled)
        {
          unsigned long long key = ((unsigned long long)fileInfo.mFileId << 32) + info->mMessageIndex;
          auto rec = mStartUpCache_indexMap.find(key);
          if (rec != mStartUpCache_indexMap.end() &&  rec->second.producerId == fileInfo.mProducerId  &&  rec->second.generationId == fileInfo.mGenerationId)
          {
            mInfo.mFileMemoryPtr = mStartUpCache_memoryMapInfo.memoryPtr;
            mInfo.mFilePosition = rec->second.filePosition;
          }
        }

        mInfo.mMessageType = info->mFileType;
        mInfo.mMessageSize = info->mMessageSize;
        mInfo.mProducerId = info->mProducerId;
        mInfo.mGenerationId = info->mGenerationId;
        mInfo.mFmiParameterId = info->mFmiParameterId;
        mInfo.mFmiParameterName = stringFactory.create(info->getFmiParameterName());
        mInfo.mFmiParameterLevelId = info->mFmiParameterLevelId;
        mInfo.mParameterLevel = info->mParameterLevel;
        mInfo.mForecastTime = info->mForecastTimeUTC;
        mInfo.mForecastType = info->mForecastType;
        mInfo.mForecastNumber = info->mForecastNumber;
        mInfo.mGeometryId = info->mGeometryId;

        gridFile->newMessage(info->mMessageIndex,mInfo);
      }
    }

    if (gridFile->getModificationTime() != 0)
    {
      if (!storageFile)
      {
        mGridFileManager.addFile(gridFile);
      }
    }
    else
    {
      // The grid file does not exist.

      if (!storageFile)
        delete gridFile;

      return;
    }

    gridFile->setAccessTime(checkTime);
    gridFile->setCheckTime(checkTime);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Operation failed!",nullptr);
    exception.addParameter("FileId",Fmi::to_string(fileInfo.mFileId));
    exception.addParameter("Filename",mDataDir + "/" + fileInfo.mName);
    throw exception;
  }
}





void ServiceImplementation::fullUpdate()
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"****************** FULL UPDATE START *********************\n");

    T::ContentInfoList fullContentList;
    readContentList(fullContentList,true,true);
    fullContentList.sort(T::ContentInfo::ComparisonMethod::file_message);

    bool vContentEnabled = mVirtualContentEnabled;
    mVirtualContentEnabled = false;

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

    // Removing all file mappings
    mGridFileManager.clear();

    uint counter = 0;
    time_t checkTime = time(nullptr);
    uint startFileId = 0;
    uint len = 50000;
    while (len > 0)
    {
      if (mShutdownRequested)
        return;

      T::FileInfoList fileInfoList;
      int result = mContentServer->getFileInfoList(mServerSessionId,startFileId,50000,fileInfoList);
      if (result != 0)
      {
        PRINT_DATA(mDebugLog,"%s:%d: Cannot get the file list from the content server!",__FILE__,__LINE__);
        PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
        return;
      }

      len = fileInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        if (mShutdownRequested)
          return;

        try
        {
          T::FileInfo *fileInfo = fileInfoList.getFileInfoByIndex(t);
          if (fileInfo->mFileId >= startFileId)
            startFileId = fileInfo->mFileId + 1;

          T::ContentInfoList contentList;
          if (mContentServer->getImplementationType() != ContentServer::Implementation::Cache || fullContentList.getLength() > 0)
            fullContentList.getContentInfoListByFileId(fileInfo->mFileId,contentList);
          else
            mContentServer->getContentListByFileId(mServerSessionId,fileInfo->mFileId,contentList);

          addFile(*fileInfo,contentList);

          counter++;
          if ((counter % 10000) == 0)
            PRINT_DATA(mDebugLog,"* Adding grid files : %u\n",counter);
        }
        catch (...)
        {
          Fmi::Exception exception(BCP,"Operation failed!",nullptr);
          std::string st = exception.getStackTrace();
          PRINT_DATA(mDebugLog,"%s",st.c_str());
        }
      }
    }

    PRINT_DATA(mDebugLog,"* Adding grid files : %u\n",counter);

    mVirtualContentEnabled  = vContentEnabled;
    mGridFileManager.deleteFilesByCheckTime(checkTime);
    mGridFileManager.deleteVirtualFiles();

    if (mVirtualContentEnabled)
    {
      time_t changeTime = 0;
      mContentServer->getContentChangeTime(mServerSessionId,changeTime);
      updateVirtualFiles(fullContentList);
      mContentChangeTime = changeTime;
    }


    mFullUpdateRequired = false;

    PRINT_DATA(mDebugLog,"******************* FULL UPDATE END **********************\n");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::event_clear(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"*** Clear event : All content files deleted ****\n");
    mGridFileManager.clear();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::event_contentServerReload(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    mFullUpdateRequired = true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::event_producerAdded(T::EventInfo& eventInfo)
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





void ServiceImplementation::event_producerDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    mGridFileManager.deleteFilesByProducerId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::event_producerUpdated(T::EventInfo& eventInfo)
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





void ServiceImplementation::event_producerListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    mGridFileManager.deleteFilesBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::event_generationAdded(T::EventInfo& eventInfo)
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





void ServiceImplementation::event_generationDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    mGridFileManager.deleteFilesByGenerationId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::event_generationUpdated(T::EventInfo& eventInfo)
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





void ServiceImplementation::event_generationStatusChanged(T::EventInfo& eventInfo)
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





void ServiceImplementation::event_generationListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    mGridFileManager.deleteFilesByProducerId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::event_generationListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    mGridFileManager.deleteFilesBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::event_fileAdded(T::EventInfo& eventInfo,T::EventInfo *nextEventInfo)
{
  //FUNCTION_TRACE
  try
  {
    //printf("event_fileAdded %u %u\n",eventInfo.mId1,eventInfo.mId2);
    //eventInfo.print(std::cout,0,0);

    if (eventInfo.mId2 == T::FileTypeValue::Virtual)  // This is a virtual file that we have added.
      return;

    uint len = eventInfo.mEventData.length();
    T::FileInfo fileInfo;
    T::ContentInfoList contentList;

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
            fileInfo.setCsv(s);
          }
          else
          {
            T::ContentInfo *contentInfo = new T::ContentInfo();
            contentInfo->setCsv(s);
            contentList.addContentInfo(contentInfo);
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
      int result = mContentServer->getFileInfoById(mServerSessionId,eventInfo.mId1,fileInfo);
      if (result != 0)
      {
        PRINT_DATA(mDebugLog,"%s:%d: Cannot get the file info (fileId=%u) from the content server\n",__FILE__,__LINE__,eventInfo.mId1);
        PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
        return;
      }

      result = mContentServer->getContentListByFileId(mServerSessionId,fileInfo.mFileId,contentList);
      if (result != 0)
      {
        PRINT_DATA(mDebugLog,"%s:%d: Cannot get the content list (fileId=%d) from the content server!\n",__FILE__,__LINE__,fileInfo.mFileId);
        PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
        return;
      }
    }

    //fileInfo.print(std::cout,0,0);
    addFile(fileInfo,contentList);

    uint cnt = mGridFileManager.getFileCount();
    if ((cnt % 1000) == 0)
      PRINT_DATA(mDebugLog,"** file added %u\n",cnt);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






void ServiceImplementation::event_fileDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    if (eventInfo.mId2 == T::FileTypeValue::Virtual)
      return; // The deleted file was virtual. No need to react.

    mGridFileManager.deleteFileById(eventInfo.mId1);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::event_fileUpdated(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
#if 0
    //printf("EVENT[%llu]: filedUpdated(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    if (eventInfo.mId2 == T::FileTypeValue::Virtual)
      return; // The updated file was virtual. No need to react.

    GRID::GridFile_sptr storageFile = mGridFileManager.getFileByIdNoMapping(eventInfo.mId1);
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
      mGridFileManager.deleteFileById(eventInfo.mId1);
    }

    // Adding a new file information.
    T::FileInfo fileInfo;
    int result = mContentServer->getFileInfoById(mServerSessionId,eventInfo.mId1,fileInfo);
    if (result != 0)
    {
      PRINT_DATA(mDebugLog,"%s:%d: Cannot get the file info (fileId=%u) from the content server\n",__FILE__,__LINE__,eventInfo.mId1);
      PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
      return;
    }

    T::ContentInfoList currentContentList;

    // If the content list is not predefined the we should remove it and replace it with a new list.
    result = mContentServer->getContentListByFileId(mServerSessionId,fileInfo.mFileId,currentContentList);
    if (result != 0)
    {
      PRINT_DATA(mDebugLog,"%s:%d: Cannot get the content list (fileId=%d) from the content server!\n",__FILE__,__LINE__,fileInfo.mFileId);
      PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
      return;
    }

    addFile(fileInfo,currentContentList);
#endif
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::event_fileListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    mGridFileManager.deleteFilesByProducerId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::event_fileListDeletedByGenerationId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    mGridFileManager.deleteFilesByGenerationId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






void ServiceImplementation::event_fileListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    mGridFileManager.deleteFilesBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::event_contentListDeletedByFileId(T::EventInfo& eventInfo)
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





void ServiceImplementation::event_contentListDeletedByProducerId(T::EventInfo& eventInfo)
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





void ServiceImplementation::event_contentListDeletedByGenerationId(T::EventInfo& eventInfo)
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




void ServiceImplementation::event_contentListDeletedBySourceId(T::EventInfo& eventInfo)
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





void ServiceImplementation::event_contentAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("event_contentAdded %u %u\n",eventInfo.mId1,eventInfo.mId2);

    if (eventInfo.mFlags & T::ContentInfo::Flags::VirtualContent)
      return; // This is virtual content that we had added

    //eventInfo.print(std::cout,0,0);
    T::ContentInfo contentInfo;
    if (eventInfo.mEventData > " ")
      contentInfo.setCsv(eventInfo.mEventData);
    else
    {
      if (mContentServer->getContentInfo(mServerSessionId,eventInfo.mId1,eventInfo.mId2,contentInfo) != Result::OK)
      {
        //printf("--- no found\n");
        return;
      }
    }

    GRID::GridFile_sptr storageFile = mGridFileManager.getFileByIdNoMapping(eventInfo.mId1);

    GRID::GridFile *gridFile = nullptr;
    if (storageFile)
    {
      gridFile = storageFile.get();
      try
      {
        GRID::MessageInfo mInfo;

        mInfo.mFileMemoryPtr = nullptr;
        mInfo.mFilePosition = contentInfo.mFilePosition;

        if (mStartUpCache_enabled)
        {
          unsigned long long key = ((unsigned long long)contentInfo.mFileId << 32) + contentInfo.mMessageIndex;
          auto rec = mStartUpCache_indexMap.find(key);
          if (rec != mStartUpCache_indexMap.end() &&  rec->second.producerId == contentInfo.mProducerId  &&  rec->second.generationId == contentInfo.mGenerationId)
          {
            mInfo.mFileMemoryPtr = mStartUpCache_memoryMapInfo.memoryPtr;
            mInfo.mFilePosition = rec->second.filePosition;
          }
        }

        mInfo.mMessageType = contentInfo.mFileType;
        mInfo.mMessageSize = contentInfo.mMessageSize;
        mInfo.mProducerId = contentInfo.mProducerId;
        mInfo.mGenerationId = contentInfo.mGenerationId;
        mInfo.mFmiParameterId = contentInfo.mFmiParameterId;
        mInfo.mFmiParameterName = stringFactory.create(contentInfo.getFmiParameterName());
        mInfo.mFmiParameterLevelId = contentInfo.mFmiParameterLevelId;
        mInfo.mParameterLevel = contentInfo.mParameterLevel;
        mInfo.mForecastTime = contentInfo.mForecastTimeUTC;
        mInfo.mForecastType = contentInfo.mForecastType;
        mInfo.mForecastNumber = contentInfo.mForecastNumber;
        mInfo.mGeometryId = contentInfo.mGeometryId;

        gridFile->newMessage(contentInfo.mMessageIndex,mInfo);
      }
      catch (...)
      {
        Fmi::Exception exception(BCP,"Operation failed!",nullptr);
        exception.printError();
      }
    }
    else
    {
      //printf("--- storage file not found\n");
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::event_contentUpdated(T::EventInfo& eventInfo)
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





void ServiceImplementation::event_contentDeleted(T::EventInfo& eventInfo)
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





void ServiceImplementation::event_deleteVirtualContent(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"* Delete virtual content\n");
    mGridFileManager.deleteVirtualFiles();
    mContentChangeTime = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::event_updateVirtualContent(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mVirtualContentEnabled)
      return;

    PRINT_DATA(mDebugLog,"Update virtual content\n");
    mGridFileManager.deleteVirtualFiles();
    sleep(5);

    // If we are not using the content server cache implementation then it is faster
    // to cache all content before processing.

    T::ContentInfoList fullContentList;
    if (mContentServer->getImplementationType() != ContentServer::Implementation::Cache)
    {
      readContentList(fullContentList,true,false);
      fullContentList.sort(T::ContentInfo::ComparisonMethod::file_message);
    }

    time_t changeTime = 0;
    mContentServer->getContentChangeTime(mServerSessionId,changeTime);
    updateVirtualFiles(fullContentList);
    mContentChangeTime = changeTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::processEvent(T::EventInfo& eventInfo,T::EventInfo *nextEventInfo)
{
  //FUNCTION_TRACE
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

      case ContentServer::EventType::PRODUCER_UPDATED:
        event_producerUpdated(eventInfo);
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

      case ContentServer::EventType::GENERATION_UPDATED:
        event_generationUpdated(eventInfo);
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
        event_fileAdded(eventInfo,nextEventInfo);
        break;

      case ContentServer::EventType::FILE_DELETED:
        event_fileDeleted(eventInfo);
        break;

      case ContentServer::EventType::FILE_UPDATED:
        event_fileUpdated(eventInfo);
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

      case ContentServer::EventType::CONTENT_LIST_DELETED_BY_PRODUCER_ID:
        event_contentListDeletedByProducerId(eventInfo);
        break;

      case ContentServer::EventType::CONTENT_LIST_DELETED_BY_GENERATION_ID:
        event_contentListDeletedByGenerationId(eventInfo);
        break;

      case ContentServer::EventType::CONTENT_LIST_DELETED_BY_SOURCE_ID:
        event_contentListDeletedBySourceId(eventInfo);
        break;

      case ContentServer::EventType::CONTENT_ADDED:
        event_contentAdded(eventInfo);
        break;

      case ContentServer::EventType::CONTENT_DELETED:
        event_contentDeleted(eventInfo);
        break;

      case ContentServer::EventType::CONTENT_UPDATED:
        event_contentUpdated(eventInfo);
        break;

      case ContentServer::EventType::DELETE_VIRTUAL_CONTENT:
        event_deleteVirtualContent(eventInfo);
        break;

      case ContentServer::EventType::UPDATE_VIRTUAL_CONTENT:
        event_updateVirtualContent(eventInfo);
        break;
    }

  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ServiceImplementation::processEvents()
{
  //FUNCTION_TRACE
  try
  {
    if (mShutdownRequested)
      return;

    mLuaFileCollection.checkUpdates(false);

    time_t currentTime = time(nullptr);
    if (mFileCleanup_checkInterval > 0  && (currentTime - mFileCleanup_time) > mFileCleanup_checkInterval)
    {
      mFileCleanup_time = currentTime;
      mGridFileManager.deleteFilesByAccessTime(currentTime - mFileCleanup_age);
    }

    if ((currentTime - mDeletedFileCleanup_time) > 120)
    {
      mGridFileManager.deleteFilesByDeletionTime(currentTime-120);
      mDeletedFileCleanup_time = currentTime;
    }

    if (mFullUpdateRequired)
    {
      fullUpdate();
      return;
    }

    if (mVirtualContentEnabled  &&  (mLastVirtualFileCheck + 30) < time(nullptr))
    {
      time_t changeTime = 0;
      mContentServer->getContentChangeTime(mServerSessionId,changeTime);

      if (mContentChangeTime != changeTime || mVirtualContentManager.checkUpdates())
      {
        T::EventInfo eventInfo;
        event_updateVirtualContent(eventInfo);
        mContentChangeTime = changeTime;
      }
      mLastVirtualFileCheck = time(nullptr);
    }

    T::EventInfo eventInfo;
    int result = mContentServer->getLastEventInfo(mServerSessionId,mServerId,eventInfo);
    if (result == Result::DATA_NOT_FOUND)
    {
      if (mContentServerStartTime > 0 &&  eventInfo.mServerTime > mContentServerStartTime)
      {
        if (mContentServerStartTime > 0)
        {
          PRINT_DATA(mDebugLog, "#### Content server restart detected, full update required #######\n");
          fullUpdate();
        }
      }
      mContentServerStartTime = eventInfo.mServerTime;
      return;
    }


    if (result == Result::OK)
    {
      if (eventInfo.mEventId == mLastProcessedEventId)
      {
        // There are no events to process.
        return;
      }
    }
    else
    {
      // ERROR: getLastEventInfo
      return;
    }

    uint len = 10000;
    while (len > 0)
    {
      T::EventInfoList eventInfoList;
      result = mContentServer->getEventInfoList(mServerSessionId,mServerId,mLastProcessedEventId+1,10000,eventInfoList);
      if (result != 0)
      {
        PRINT_DATA(mDebugLog,"%s:%d: Cannot get the event info list from the content server!\n",__FILE__,__LINE__);
        PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
        return;
      }

      len = eventInfoList.getLength();

      T::EventInfo *it = eventInfoList.getFirstEvent();
      while (it != nullptr)
      {
        processEvent(*it,it->nextItem);
        mLastProcessedEventId = it->mEventId;
        it = it->nextItem;

        if (mShutdownRequested)
          return;
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
        Fmi::Exception exception(BCP,"Operation failed!",nullptr);
        std::string st = exception.getStackTrace();
        PRINT_DATA(mDebugLog,"%s",st.c_str());
      }

      if (!mShutdownRequested)
        sleep(1);
    }

    mEventProcessingActive = false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::cacheProcessingThread()
{
  FUNCTION_TRACE
  try
  {
    while (!mShutdownRequested)
    {
      time_t nextSaveTime = mStartUpCache_saveTime + mStartUpCache_saveInterval;

      while (time(0) < nextSaveTime  &&  !mShutdownRequested)
        sleep(1);

      if (!mShutdownRequested)
      {
        try
        {
          mStartUpCache_saveTime = time(0);
          mCacheProcessingActive = true;
          GRID::RequestCounters requestCounters;
          std::map<long long,std::vector<unsigned long long>> values;
          mGridFileManager.getRequestCounters(requestCounters,mStartUpCache_saveDiskData,mStartUpCache_saveNetworkData);
          mGridFileManager.resetRequestCounters();

          for (auto it=requestCounters.begin(); it!=requestCounters.end();++it)
          {
            auto rec = values.find(it->second);
            if (rec != values.end())
              rec->second.push_back(it->first);
            else
            {
              std::vector<unsigned long long> vec;
              vec.push_back(it->first);
              values.insert(std::pair<long long,std::vector<unsigned long long>>(it->second,vec));
            }
          }

          char fname1[200];
          char fname2[200];
          char ifname1[200];
          char ifname2[200];
          sprintf(fname1,"%s.dat.tmp",mStartUpCache_filename.c_str());
          sprintf(fname2,"%s.dat.new",mStartUpCache_filename.c_str());

          sprintf(ifname1,"%s.index.tmp",mStartUpCache_filename.c_str());
          sprintf(ifname2,"%s.index.new",mStartUpCache_filename.c_str());

          time_t startTime = time(0);
          FILE *file1 = fopen(fname1,"w");
          FILE *file2 = fopen(ifname1,"w");

          long long total = 0;
          //std::cout << "REQUESTS  " << requestCounters.size() << "\n";
          for (auto it = values.rbegin(); it != values.rend()  &&  !mShutdownRequested  &&  total < mStartUpCache_maxSize; ++it)
          {
            for (auto v = it->second.begin(); v != it->second.end(); ++v)
            {
              uint fileId = ((*v) >> 32) & 0xFFFFFFFF;
              uint messageIndex = (*v) & 0xFFFFFFFF;

              GRID::GridFile_sptr gridFile = getGridFile(fileId);
              if (gridFile  &&  !mShutdownRequested)
              {
                GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
                if (message  &&  (total + message->getMessageSize()) < mStartUpCache_maxSize)
                {
                  char *p = message->getMemoryPtr();

                  StartUpCacheIndexRec rec;
                  rec.producerId = message->getProducerId();
                  rec.generationId = message->getGenerationId();
                  rec.fileId = fileId;
                  rec.messageIndex = messageIndex;
                  rec.filePosition = ftell(file1);
                  rec.messageSize = message->getMessageSize();

                  fwrite(&rec,sizeof(rec),1,file2);
                  fwrite(p,rec.messageSize,1,file1);

                  total = total + rec.messageSize;
                }
              }
            }

            //std::cout << " * " << it->first << " : " << it->second.size() << " = " << total << "\n";
          }
          fclose(file1);
          fclose(file2);

          if (!mShutdownRequested)
          {
            rename(fname1,fname2);
            rename(ifname1,ifname2);
          }

          time_t endTime = time(0);

          //printf("WRITE TIME %u\n",endTime-startTime);
        }
        catch (...)
        {
          Fmi::Exception exception(BCP,"Operation failed!",nullptr);
          std::string st = exception.getStackTrace();
          PRINT_DATA(mDebugLog,"%s",st.c_str());
        }
      }
    }

    mCacheProcessingActive = false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}


}
}

