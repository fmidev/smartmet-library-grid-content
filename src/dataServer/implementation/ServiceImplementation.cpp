#include "ServiceImplementation.h"
#include "VirtualContentFactory_type1.h"
#include "VirtualMessage.h"
#include "../../functions/Function_add.h"
#include "../../functions/Function_avg.h"
#include "../../functions/Function_inPrcnt.h"
#include "../../functions/Function_outPrcnt.h"
#include "../../functions/Function_min.h"
#include "../../functions/Function_max.h"
#include "../../functions/Function_multiply.h"
#include "../../functions/Function_sequence.h"
#include "../../functions/Function_hypotenuse.h"
#include "../../functions/Function_windDir.h"
#include "../../functions/Function_vectorU.h"
#include "../../functions/Function_vectorV.h"

#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/InterpolationFunctions.h>
#include <grid-files/common/ShowFunction.h>
#include <grid-files/common/GraphFunctions.h>
#include <grid-files/common/CoordinateConversions.h>
#include <grid-files/common/MemoryWriter.h>
#include <grid-files/common/RequestCounter.h>
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

#if 0

bool sigbusActive = false;
ThreadLock sigbusThreadLock;



void sigbus_action(int signo, siginfo_t *info, void *extra)
{
  sigaction(SIGBUS, nullptr,nullptr);

  ucontext_t *p=(ucontext_t *)extra;
  //int x;
  //printf("Signal %d received from parent\n", signo);
  //printf("siginfo address=%x\n",info->si_addr);

  //x = p->uc_mcontext.gregs[REG_RIP];

  //printf("address = %x\n",x);

  p->uc_mcontext.gregs[REG_RIP] += 6;
}



bool isMemoryMappingValid(char *address,long long size)
{
  if (address == nullptr || size <= 0)
    return false;

  AutoThreadLock lock(&sigbusThreadLock);

  sigbusActive = false;

  struct sigaction act;
  act.sa_handler = NULL;
  act.sa_sigaction = sigbus_action;
  sigemptyset (&act.sa_mask);
  act.sa_flags = 0;
  act.sa_restorer = nullptr;

  sigaction(SIGBUS, &act,nullptr);

  char *ch1 = address;
  if (sigbusActive)
    return false;

  sigaction(SIGBUS, &act,nullptr);
  char *ch2 = address+size-1;
  if (sigbusActive)
    return false;

  sigaction(SIGBUS, nullptr,nullptr);
  return true;
}
#endif




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
    SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
    exception.printError();
    exit(-1);
  }
}





static void* ServiceImplementation_requestCounterThread(void *arg)
{
  try
  {
    ServiceImplementation *service = (ServiceImplementation*)arg;
    service->requestCounterThread();
    return nullptr;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    mRequestCountingActive = false;
    mContentRegistrationEnabled = false;
    mVirtualContentEnabled = false;
    mContentServerStartTime = 0;
    mLastVirtualFileRegistration = 0;
    mContentPreloadEnabled = true;
    mLastCacheCheck = time(0);
    mPointCacheEnabled = false;
    mPointCacheHitsRequired = 20;
    mPointCacheTimePeriod = 1200;

    mCounterFile_modificationTime = 0;
    mPreloadFileGenerationEnabled = false;
    mPreloadFile_modificationTime = 0;
    mPreloadMemoryLock = false;
    mMemoryMapCheckEnabled = false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void ServiceImplementation::init(T::SessionId serverSessionId,uint serverId,std::string serverName,std::string serverIor,std::string dataDir,ContentServer::ServiceInterface *contentServer,string_vec& luaFileNames)
{
  FUNCTION_TRACE
  try
  {
    if (contentServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"The 'contentServer' parameter points to nullptr!");

    //if (serverId == 0 ||  serverId > 64)
    //  throw SmartMet::Spine::Exception(BCP,"The 'serverId' parameter value must be in the range [1..64]!");

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

    checkServerRegistration();
    // fullUpdate();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::setPreload(bool preloadEnabled,bool preloadMemoryLock,std::string preloadFile,std::string counterFile,bool preloadFileGenerationEnabled,std::string generatedPreloadFile,std::string generatedCounterFile)
{
  FUNCTION_TRACE
  try
  {
    mContentPreloadEnabled = preloadEnabled;
    mPreloadMemoryLock = preloadMemoryLock;
    mPreloadFileGenerationEnabled = preloadFileGenerationEnabled;
    mPreloadFile = preloadFile;
    mCounterFile = counterFile;
    mGeneratedPreloadFile = generatedPreloadFile;
    mGeneratedCounterFile = generatedCounterFile;

    requestCounter.setCountingEnabled(mPreloadFileGenerationEnabled);

    if (mContentPreloadEnabled)
    {
      mCounterFile_modificationTime = getFileModificationTime(mCounterFile.c_str());
      requestCounter.loadGeometryHitCounters(mCounterFile.c_str());
      loadPreloadList();
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::setPointCacheEnabled(bool enabled,uint hitsRequired,uint timePeriod)
{
  FUNCTION_TRACE
  try
  {
    mPointCacheEnabled = enabled;
    mPointCacheHitsRequired = hitsRequired;
    mPointCacheTimePeriod = timePeriod;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void ServiceImplementation::setMemoryMapCheckEnabled(bool enabled)
{
  FUNCTION_TRACE
  try
  {
    mMemoryMapCheckEnabled = enabled;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::startRequestCounting()
{
  FUNCTION_TRACE
  try
  {
    pthread_create(&mRequestCounterThread,nullptr,ServiceImplementation_requestCounterThread,this);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





GRID::GridFile_sptr ServiceImplementation::getGridFile(uint fileId)
{
  FUNCTION_TRACE
  try
  {
    GRID::GridFile_sptr gridFile = mGridFileManager.getFileById(fileId);
    if (gridFile)
    {
      if (mMemoryMapCheckEnabled  &&  !gridFile->isVirtual() && gridFile->isMemoryMapped())
      {
        std::string fname = gridFile->getFileName();
        if (getFileSize(fname.c_str()) <= 100)
        {
          mGridFileManager.deleteFileById(fileId);
          return nullptr;
        }
      }

      time_t deletionTime = gridFile->getDeletionTime();
      if (deletionTime == 0)
        return gridFile;

      if ((time(nullptr) + 120) > deletionTime)
      {
        // The grid file will be deleted soon. We should not access it anymore.
        return nullptr;
      }
      else
      {
        return gridFile;
      }
    }

    // If the grid file is not found from the grid storage but it is registered
    // to the contentServer then we should try to add it to the grid storage.

    T::FileInfo fileInfo;
    T::ContentInfoList currentContentList;

    if (mContentServer->getFileInfoById(mServerSessionId,fileId,fileInfo) == 0 &&
        mContentServer->getContentListByFileId(mServerSessionId,fileId,currentContentList) == 0)
    {
      if (!fileInfo.mDeletionTime.empty())
      {
        time_t delTime = utcTimeToTimeT(fileInfo.mDeletionTime);
        if ((time(nullptr) + 120) > delTime)
        {
          // The grid file will be deleted soon. We should not access it anymore.
          return nullptr;
        }
      }

      if (getFileSize(fileInfo.mName.c_str()) > 0)
      {
        mContentServer->getContentListByFileId(mServerSessionId,fileId,currentContentList);

        T::ContentInfoList contentInfoList;

        addFile(fileInfo,currentContentList,contentInfoList);
        gridFile = mGridFileManager.getFileById(fileId);
      }
    }
    return gridFile;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getMultipleGridValues(T::SessionId sessionId,T::ValueRecordList& valueRecordList)
{
  FUNCTION_TRACE
  try
  {
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
                  rec->mValue = message->getGridValueByLatLonCoordinate(rec->mY,rec->mX,(short)rec->mAreaInterpolationMethod);
                  rec->mResult = 0;
                  break;

                case T::CoordinateTypeValue::GRID_COORDINATES:
                  rec->mValue = message->getGridValueByGridPoint_byInterpolation(rec->mX,rec->mY,(short)rec->mAreaInterpolationMethod);
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
              SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,T::GridCoordinates& coordinates)
{
  FUNCTION_TRACE
  try
  {
    GRID::GridFile_sptr gridFile = getGridFile(fileId);
    if (!gridFile)
      return Result::FILE_NOT_FOUND;

    GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
    if (message == nullptr)
      return Result::MESSAGE_NOT_FOUND;

    coordinates.mProjection = message->getGridProjection();
    message->getGridProjectionAttributes("",coordinates.mProjectionAttributes);
    coordinates.mColumns = message->getGridOriginalColumnCount();
    coordinates.mRows = message->getGridOriginalRowCount();
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
            coordinates.mCoordinateList.push_back(T::Coordinate(x,y));
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data)
{
  FUNCTION_TRACE
  try
  {
    GRID::GridFile_sptr gridFile = getGridFile(fileId);
    if (!gridFile)
      return Result::DATA_NOT_FOUND;

    GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
    if (message == nullptr)
      return Result::DATA_NOT_FOUND;

    data.mServerId = mServerId;
    data.mGroupFlags = gridFile->getGroupFlags();
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
    data.mGribParameterId = message->getGribParameterId();
    data.mGrib1ParameterLevelId = message->getGrib1ParameterLevelId();
    data.mGrib2ParameterLevelId = message->getGrib2ParameterLevelId();
    data.mGribParameterName = message->getGribParameterName();
    data.mGribParameterUnits = message->getGribParameterUnits();
    data.mFmiParameterId = message->getFmiParameterId();
    data.mFmiParameterLevelId = message->getFmiParameterLevelId();
    data.mFmiParameterName = message->getFmiParameterName();
    data.mFmiParameterUnits = message->getFmiParameterUnits();
    data.mCdmParameterId = message->getCdmParameterId();
    data.mCdmParameterName = message->getCdmParameterName();
    data.mNewbaseParameterId = message->getNewbaseParameterId();
    data.mNewbaseParameterName = message->getNewbaseParameterName();
    data.mParameterLevel = message->getGridParameterLevel();
    data.mGeometryId = message->getGridGeometryId();
    data.mProjection = message->getGridProjection();
    data.mColumns = message->getGridOriginalColumnCount();
    data.mRows = message->getGridOriginalRowCount();
    data.mForecastType = message->getForecastType();
    data.mForecastNumber = message->getForecastNumber();
    message->getGridProjectionAttributes("",data.mProjectionAttributes);

    message->getGridValueVector(data.mValues);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridFileCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    count = mGridFileManager.getFileCount();
    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridMessageBytes(T::SessionId sessionId,uint fileId,uint messageIndex,std::vector<uchar>& messageBytes,std::vector<uint>& messageSections)
{
  FUNCTION_TRACE
  try
  {
    GRID::GridFile_sptr gridFile = getGridFile(fileId);
    if (!gridFile)
      return Result::DATA_NOT_FOUND;

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
        messageSections.push_back(*it - messagePosition);
      }
    }

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridMessagePreloadCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (mContentPreloadEnabled)
      count = mPreloadList.size();
    else
      count = 0xFFFFFFFF;

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    GRID::GridFile_sptr gridFile = getGridFile(fileId);
    if (!gridFile)
      return Result::DATA_NOT_FOUND;

    GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
    if (message == nullptr)
      return Result::DATA_NOT_FOUND;

    message->getAttributeList(std::string(""),attributeList);

    return Result::OK;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueByPoint(coordinateType,x,y,areaInterpolationMethod,value);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueByLevelAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,int level1,uint fileId2,uint messageIndex2,int level2,int newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short levelInterpolationMethod,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueByLevelAndPoint(*message1,*message2,level1,level2,newLevel,coordinateType,x,y,areaInterpolationMethod,levelInterpolationMethod,value);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueByTimeAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueByTimeAndPoint(*message1,*message2,newTime,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,value);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueByTimeLevelAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,int level1,uint fileId2,uint messageIndex2,int level2,uint fileId3,uint messageIndex3,int level3,uint fileId4,uint messageIndex4,int level4,std::string newTime,int newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueByTimeLevelAndPoint(*message1,level1,*message2,level2,*message3,level3,*message4,level4,newTime,newLevel,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,value);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




int ServiceImplementation::_getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueVectorWithCaching(values);
      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,short levelInterpolationMethod,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueVectorByLevel(*message1,*message2,newLevel,levelInterpolationMethod,values);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,short timeInterpolationMethod,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueVectorByTime(*message1,*message2,newTime,timeInterpolationMethod,values);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueVectorByLevelAndGeometry(*message1,*message2,newLevel,attributeList,values);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueVectorByTimeAndGeometry(*message1,*message2,newTime,attributeList,values);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByCoordinateList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,short areaInterpolationMethod,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueVectorByCoordinateList(coordinateType,coordinates,areaInterpolationMethod,values);
      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByLevelAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueVectorByLevelAndCoordinateList(*message1,*message2,newLevel,coordinateType,coordinates,attributeList,values);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByTimeAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueVectorByTimeAndCoordinateList(*message1,*message2,newTime,coordinateType,coordinates,attributeList,values);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueVectorByTimeAndLevel(*message1,*message2,*message3,*message4,newTime,newLevel,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,values);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueVectorByTimeLevelAndGeometry(*message1,*message2,*message3,*message4,newTime,newLevel,attributeList,values);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByTimeLevelAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueVectorByTimeLevelAndCoordinateList(*message1,*message2,*message3,*message4,newTime,newLevel,coordinateType,coordinates,attributeList,values);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueVectorByGeometry(attributeList,values);
      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueListByCircle(coordinateType,origoX,origoY,radius,valueList);
      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueListByTimeAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueListByTimeAndCircle(*message1,*message2,newTime,coordinateType,origoX,origoY,radius,timeInterpolationMethod,valueList);

      return Result::OK;
    }
    catch (...)
    {
      SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueListByLevelAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short levelInterpolationMethod,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueListByLevelAndCircle(*message1,*message2,newLevel,coordinateType,origoX,origoY,radius,levelInterpolationMethod,valueList);

      return Result::OK;
    }
    catch (...)
    {
      SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueListByTimeLevelAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,short levelInterpolationMethod,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueListByTimeLevelAndCircle(*message1,*message2,*message3,*message4,newTime,newLevel,coordinateType,origoX,origoY,radius,timeInterpolationMethod,levelInterpolationMethod,valueList);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,short areaInterpolationMethod,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

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
              T::ParamValue value = message->getGridValueByLatLonCoordinate(y,xx,areaInterpolationMethod);
              values.push_back(value);
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
              T::ParamValue value = message->getGridValueByGridPoint_byInterpolation(xx,y,areaInterpolationMethod);
              values.push_back(value);
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
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueListByPointList(coordinateType,pointList,areaInterpolationMethod,valueList);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueListByLevelAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short levelInterpolationMethod,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueListByLevelAndPointList(*message1,*message2,newLevel,coordinateType,pointList,areaInterpolationMethod,levelInterpolationMethod,valueList);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueListByTimeAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueListByTimeAndPointList(*message1,*message2,newTime,coordinateType,pointList,areaInterpolationMethod,timeInterpolationMethod,valueList);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueListByTimeLevelAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueListByTimeLevelAndPointList(*message1,*message2,*message3,*message4,newTime,newLevel,coordinateType,pointList,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,valueList);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueListByPolygon(coordinateType,polygonPoints,valueList);
      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueListByLevelAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short levelInterpolationMethod,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueListByLevelAndPolygon(*message1,*message2,newLevel,coordinateType,polygonPoints,levelInterpolationMethod,valueList);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueListByTimeAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueListByTimeAndPolygon(*message1,*message2,newTime,coordinateType,polygonPoints,timeInterpolationMethod,valueList);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueListByTimeLevelAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,short levelInterpolationMethod,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueListByTimeLevelAndPolygon(*message1,*message2,*message3,*message4,newTime,newLevel,coordinateType,polygonPoints,timeInterpolationMethod,levelInterpolationMethod,valueList);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueListByPolygonPath(coordinateType,polygonPath,valueList);
      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueListByTimeAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueListByTimeAndPolygonPath(*message1,*message2,newTime,coordinateType,polygonPath,timeInterpolationMethod,valueList);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueListByLevelAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short levelInterpolationMethod,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueListByLevelAndPolygonPath(*message1,*message2,newLevel,coordinateType,polygonPath,levelInterpolationMethod,valueList);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}






int ServiceImplementation::_getGridValueListByTimeLevelAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,short levelInterpolationMethod,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridValueListByTimeLevelAndPolygonPath(*message1,*message2,*message3,*message4,newTime,newLevel,coordinateType,polygonPath,timeInterpolationMethod,levelInterpolationMethod,valueList);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}


int ServiceImplementation::_getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;


      bool gridRectangle = false;
      //if ((flags & GRID_RECTANGLE_FLAG) != 0)
      //  gridRectangle = true;

      message->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,gridRectangle,valueList);
      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridValueVectorByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,uint vectorType,double_vec& valueVector)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridValueVectorByPoint(coordinateType,x,y,vectorType,valueVector);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsobands(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridIsobands(contourLowValues,contourHighValues,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridIsobandsByGeometry(contourLowValues,contourHighValues,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridIsobandsByGrid(contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,gridLatLonCoordinates,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsobandsByLevel(*message1,*message2,newLevel,contourLowValues,contourHighValues,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsobandsByTime(*message1,*message2,newTime,contourLowValues,contourHighValues,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}



#if 0

int ServiceImplementation::getGridIsobandsByTimeAndGridImpl(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,std::vector<T::Coordinate>& gridCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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

      short timeInterpolationMethod = T::TimeInterpolationMethod::Linear;
      const char *ti = attributeList.getAttributeValue("grid.timeInterpolationMethod");
      if (ti != nullptr)
        timeInterpolationMethod = toInt16(ti);

      short areaInterpolationMethod = T::AreaInterpolationMethod::Linear;
      const char *s = attributeList.getAttributeValue("grid.areaInterpolationMethod");
      if (s != nullptr)
        areaInterpolationMethod = toInt16(s);

      T::CoordinateType coordinateType = T::CoordinateTypeValue::GRID_COORDINATES;
      const char *c = attributeList.getAttributeValue("contour.coordinateType");
      if (c != nullptr)
        coordinateType = toUInt8(c);

      size_t smooth_size = 0;
      const char *ss = attributeList.getAttributeValue("contour.smooth.size");
      if (ss != nullptr)
        smooth_size = toSize_t(ss);

      size_t smooth_degree = 0;
      const char *sd = attributeList.getAttributeValue("contour.smooth.degree");
      if (sd != nullptr)
        smooth_degree = toSize_t(sd);

      T::ParamValue_vec gridValues;
      T::ParamValue_vec values1;
      T::ParamValue_vec values2;

      message1->getGridValueVectorByCoordinateList(T::CoordinateTypeValue::LATLON_COORDINATES,gridLatLonCoordinates,areaInterpolationMethod,values1);
      message2->getGridValueVectorByCoordinateList(T::CoordinateTypeValue::LATLON_COORDINATES,gridLatLonCoordinates,areaInterpolationMethod,values2);

      time_t tt = utcTimeToTimeT(newTime);
      timeInterpolation(values1,values2,message1->getForecastTimeT(),message2->getForecastTimeT(),tt,timeInterpolationMethod,gridValues);

      T::Coordinate_vec *coordinatePtr = &gridCoordinates;

      switch (coordinateType)
      {
        case T::CoordinateTypeValue::UNKNOWN:
        case T::CoordinateTypeValue::LATLON_COORDINATES:
          coordinatePtr = &gridLatLonCoordinates;
          break;

        case T::CoordinateTypeValue::GRID_COORDINATES:
          coordinatePtr = nullptr;
          break;

        case T::CoordinateTypeValue::ORIGINAL_COORDINATES:
          break;
      }

      attributeList.setAttribute("grid.timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
      attributeList.setAttribute("grid.areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));
      attributeList.setAttribute("grid.width",Fmi::to_string(gridWidth));
      attributeList.setAttribute("grid.height",Fmi::to_string(gridHeight));
      attributeList.setAttribute("grid.original.reverseYDirection",Fmi::to_string((int)message1->reverseYDirection()));
      attributeList.setAttribute("grid.original.reverseXDirection",Fmi::to_string((int)message1->reverseXDirection()));
      attributeList.setAttribute("contour.coordinateType",Fmi::to_string(coordinateType));

      getIsobands(gridValues,coordinatePtr,gridWidth,gridHeight,contourLowValues,contourHighValues,areaInterpolationMethod,smooth_size,smooth_degree,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}

#endif




int ServiceImplementation::_getGridIsobandsByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsobandsByLevelAndGeometry(*message1,*message2,newLevel,contourLowValues,contourHighValues,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}






int ServiceImplementation::_getGridIsobandsByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsobandsByTimeAndGeometry(*message1,*message2,newTime,contourLowValues,contourHighValues,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsobandsByLevelAndGrid(*message1,*message2,newLevel,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsobandsByTimeAndGrid(*message1,*message2,newTime,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsobandsByTimeAndLevel(*message1,*message2,*message3,*message4,newTime,newLevel,contourLowValues,contourHighValues,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsobandsByTimeLevelAndGeometry(*message1,*message2,*message3,*message4,newTime,newLevel,contourLowValues,contourHighValues,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsobandsByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsobandsByTimeLevelAndGrid(*message1,*message2,*message3,*message4,newTime,newLevel,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsolinesByTimeAndLevel(*message1,*message2,*message3,*message4,newTime,newLevel,contourValues,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsolinesByTimeLevelAndGeometry(*message1,*message2,*message3,*message4,newTime,newLevel,contourValues,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsolines(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridIsolines(contourValues,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridIsolinesByGeometry(contourValues,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile = getGridFile(fileId);
      if (gridFile == nullptr)
        return Result::FILE_NOT_FOUND;

      GRID::Message *message = gridFile->getMessageByIndex(messageIndex);
      if (message == nullptr)
        return Result::MESSAGE_NOT_FOUND;

      message->getGridIsolinesByGrid(contourValues,gridWidth,gridHeight,gridLatLonCoordinates,gridLatLonCoordinates,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
       exception.addParameter("FileId",Fmi::to_string(fileId));
       exception.addParameter("MessageIndex",Fmi::to_string(messageIndex));
       std::string st = exception.getStackTrace();
       PRINT_DATA(mDebugLog,"%s",st.c_str());
       return Result::UNEXPECTED_EXCEPTION;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsolinesByLevel(*message1,*message2,newLevel,contourValues,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsolinesByTime(*message1,*message2,newTime,contourValues,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsolinesByLevelAndGeometry(*message1,*message2,newLevel,contourValues,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsolinesByTimeAndGeometry(*message1,*message2,newTime,contourValues,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsolinesByLevelAndGrid(*message1,*message2,newLevel,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsolinesByTimeAndGrid(*message1,*message2,newTime,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceImplementation::_getGridIsolinesByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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
      messageProcessing.getGridIsolinesByTimeLevelAndGrid(*message1,*message2,*message3,*message4,newTime,newLevel,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





#if 0



int ServiceImplementation::getGridIsolinesByTimeAndGridImpl(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,std::vector<T::Coordinate>& gridCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    try
    {
      GRID::GridFile_sptr gridFile1 = getGridFile(fileId1);
      if (gridFile1 == nullptr)
        return Result::FILE_NOT_FOUND;

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

      short timeInterpolationMethod = T::TimeInterpolationMethod::Linear;
      const char *ti = attributeList.getAttributeValue("grid.timeInterpolationMethod");
      if (ti != nullptr)
        timeInterpolationMethod = toInt16(ti);

      short areaInterpolationMethod = T::AreaInterpolationMethod::Linear;
      const char *s = attributeList.getAttributeValue("grid.areaInterpolationMethod");
      if (s != nullptr)
        areaInterpolationMethod = toInt16(s);

      T::CoordinateType coordinateType = T::CoordinateTypeValue::GRID_COORDINATES;
      const char *c = attributeList.getAttributeValue("contour.coordinateType");
      if (c != nullptr)
        coordinateType = toUInt8(c);

      size_t smooth_size = 0;
      const char *ss = attributeList.getAttributeValue("contour.smooth.size");
      if (ss != nullptr)
        smooth_size = toSize_t(ss);

      size_t smooth_degree = 0;
      const char *sd = attributeList.getAttributeValue("contour.smooth.degree");
      if (sd != nullptr)
        smooth_degree = toSize_t(sd);

      T::ParamValue_vec gridValues;
      T::ParamValue_vec values1;
      T::ParamValue_vec values2;

      message1->getGridValueVectorByCoordinateList(T::CoordinateTypeValue::LATLON_COORDINATES,gridLatLonCoordinates,areaInterpolationMethod,values1);
      message2->getGridValueVectorByCoordinateList(T::CoordinateTypeValue::LATLON_COORDINATES,gridLatLonCoordinates,areaInterpolationMethod,values2);

      time_t tt = utcTimeToTimeT(newTime);
      timeInterpolation(values1,values2,message1->getForecastTimeT(),message2->getForecastTimeT(),tt,timeInterpolationMethod,gridValues);

      T::Coordinate_vec *coordinatePtr = &gridCoordinates;

      switch (coordinateType)
      {
        case T::CoordinateTypeValue::UNKNOWN:
        case T::CoordinateTypeValue::LATLON_COORDINATES:
          coordinatePtr = &gridLatLonCoordinates;
          break;

        case T::CoordinateTypeValue::GRID_COORDINATES:
          coordinatePtr = nullptr;
          break;

        case T::CoordinateTypeValue::ORIGINAL_COORDINATES:
          break;
      }

      attributeList.setAttribute("grid.timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
      attributeList.setAttribute("grid.areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));
      attributeList.setAttribute("grid.width",Fmi::to_string(gridWidth));
      attributeList.setAttribute("grid.height",Fmi::to_string(gridHeight));
      attributeList.setAttribute("grid.original.reverseYDirection",Fmi::to_string((int)message1->reverseYDirection()));
      attributeList.setAttribute("grid.original.reverseXDirection",Fmi::to_string((int)message1->reverseXDirection()));
      attributeList.setAttribute("contour.coordinateType",Fmi::to_string(coordinateType));

      getIsolines(gridValues,coordinatePtr,gridWidth,gridHeight,contourValues,areaInterpolationMethod,smooth_size,smooth_degree,contours);

      return Result::OK;
    }
    catch (...)
    {
       SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}
#endif




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
        Spine::Exception exception(BCP,"Cannot read the content list from the content storage!");
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::registerVirtualFiles(VirtualGridFilePtr_map& gridFileMap)
{
  FUNCTION_TRACE
  try
  {
    if (!mVirtualContentEnabled)
      return;

    mLastVirtualFileRegistration = time(nullptr);

    std::vector<T::FileAndContent> fileAndContentList;

    uint c = 0;
    for (auto it = gridFileMap.begin(); it != gridFileMap.end(); ++it)
    {
      if (mShutdownRequested)
        return;

      auto virtualFilePtr = it->second;
      if (virtualFilePtr->getFileId() == 0)
      {
        T::FileAndContent fc;
        fc.mFileInfo.mName = virtualFilePtr->getFileName();
        fc.mFileInfo.mProducerId = virtualFilePtr->getProducerId();
        fc.mFileInfo.mGenerationId = virtualFilePtr->getGenerationId();
        fc.mFileInfo.mSourceId = virtualFilePtr->getSourceId();
        fc.mFileInfo.mFileType = T::FileTypeValue::Virtual;
        fc.mFileInfo.mFlags = fc.mFileInfo.mFlags | T::FileInfo::Flags::VirtualContent;

        std::size_t len = virtualFilePtr->getNumberOfMessages();
        for (std::size_t t = 0; t<len; t++)
        {
          GRID::VirtualMessage *msg = (GRID::VirtualMessage*)virtualFilePtr->getMessageByIndex(t);
          T::ContentInfo *contentInfo = msg->getContentInfo();
          fc.mContentInfoList.addContentInfo(contentInfo->duplicate());
        }

        fileAndContentList.push_back(fc);

        if (fileAndContentList.size() > 50000)
        {
          mContentServer->addFileInfoListWithContent(mServerSessionId,0,fileAndContentList);

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

                mGridFileManager.addFile(virtualFilePtr);
              }
            }
          }
          fileAndContentList.clear();
        }
      }
      else
      {
        //GRID::Message *vmsg = virtualFilePtr->getMessageByIndex(0);
        std::size_t fLen = virtualFilePtr->getNumberOfPhysicalGridFiles();
        for (uint f=0; f<fLen; f++)
        {
          GRID::GridFile_sptr fFile = virtualFilePtr->getPhysicalGridFileByIndex(f);
          fFile->addUser(virtualFilePtr->getFileId());
        }
        mGridFileManager.addFile(virtualFilePtr);
      }

      c++;
      if ((c % 10000) == 0)
      {
        PRINT_DATA(mDebugLog,"* Registering virtual files : %u\n",c);
        fflush(stdout);
      }
    }

    if (fileAndContentList.size() > 0)
    {
      mContentServer->addFileInfoListWithContent(mServerSessionId,0,fileAndContentList);

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

            mGridFileManager.addFile(virtualFilePtr);
          }
        }
      }

      fileAndContentList.clear();
    }

    PRINT_DATA(mDebugLog,"* Registering virtual files : %u\n",c);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::updateVirtualFiles(T::ContentInfoList fullContentList)
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
      std::string cPos = CODE_LOCATION;
      PRINT_DATA(mDebugLog,"%s: Cannot get the producer list from the content server!\n",cPos.c_str());
      PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
      return;
    }

    T::GenerationInfoList generationInfoList;
    result = mContentServer->getGenerationInfoList(mServerSessionId,generationInfoList);
    if (result != 0)
    {
      std::string cPos = CODE_LOCATION;
      PRINT_DATA(mDebugLog,"%s: Cannot get the generation list from the content server!\n",cPos.c_str());
      PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
      return;
    }

    VirtualGridFilePtr_map gridFileMap;

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
        std::string cPos = CODE_LOCATION;
        PRINT_DATA(mDebugLog,"%s: Cannot get the file list from the content server!\n",cPos.c_str());
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
              std::string cPos = CODE_LOCATION;
              PRINT_DATA(mDebugLog,"%s: Cannot find the generation (%u)!\n",cPos.c_str(),fileInfo->mGenerationId);
            }
          }
          else
          {
            std::string cPos = CODE_LOCATION;
            PRINT_DATA(mDebugLog,"%s: Cannot find the producer (%u)!\n",cPos.c_str(),fileInfo->mProducerId);
          }

          counter++;
          if ((counter % 10000) == 0)
            PRINT_DATA(mDebugLog,"* Creating virtual files : %lu\n",gridFileMap.size());
        }
        catch (...)
        {
          SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
          std::string st = exception.getStackTrace();
          PRINT_DATA(mDebugLog,"%s",st.c_str());
        }
      }
    }
    PRINT_DATA(mDebugLog,"* Creating virtual files : %lu\n",gridFileMap.size());

    registerVirtualFiles(gridFileMap);

    std::set<uint> idList;
    mGridFileManager.getVirtualFiles(idList);

    PRINT_DATA(mDebugLog,"* Removing old virtual file registrations (%lu/%u)\n",idList.size(),fullContentList.getLength());
    if (mVirtualContentEnabled)
    {
      std::set<uint> fileIdList;
      uint len = fullContentList.getLength();
      uint testCount = 0;
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *cInfo = fullContentList.getContentInfoByIndex(t);
        if (cInfo != nullptr  &&  (cInfo->mFlags &  T::ContentInfo::Flags::VirtualContent) != 0)
        {
          testCount++;
          if (idList.find(cInfo->mFileId) == idList.end())
          {
            fileIdList.insert(cInfo->mFileId);
          }
        }
      }
      if (fileIdList.size() > 0)
        mContentServer->deleteFileInfoListByFileIdList(mServerSessionId,fileIdList);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::addFile(T::FileInfo& fileInfo,T::ContentInfoList& currentContentList,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if ((fileInfo.mFlags & T::FileInfo::Flags::VirtualContent) != 0)
      return;

    time_t checkTime = time(nullptr);
    T::ContentInfoList contentList;

    GRID::GridFile_sptr storageFile = mGridFileManager.getFileByIdNoMapping(fileInfo.mFileId);

    GRID::GridFile *gridFile = nullptr;
    if (storageFile)
    {
      //printf("** file already in the content storage %u\n",fileInfo.mFileId);
      gridFile = storageFile.get();
    }
    else
    {
      gridFile = new GRID::PhysicalGridFile();
      gridFile->setFileName(mDataDir + "/" + fileInfo.mName);
      gridFile->setPointCacheEnabled(mPointCacheEnabled);

      gridFile->setFileId(fileInfo.mFileId);
      gridFile->setGroupFlags(fileInfo.mGroupFlags);
      gridFile->setProducerId(fileInfo.mProducerId);
      gridFile->setGenerationId(fileInfo.mGenerationId);
      gridFile->setSourceId(fileInfo.mSourceId);
      gridFile->setDeletionTime(fileInfo.mDeletionTime);
      // gridFile->mapToMemory();
    }

    T::ProducerInfo producerInfo;
    T::GenerationInfo generationInfo;

    if (mVirtualContentEnabled)
    {
      mContentServer->getProducerInfoById(mServerSessionId,fileInfo.mProducerId,producerInfo);
      mContentServer->getGenerationInfoById(mServerSessionId,fileInfo.mGenerationId,generationInfo);
    }


    if (gridFile->getModificationTime() != 0)
    {
      if ((fileInfo.mFlags & T::FileInfo::Flags::PredefinedContent) == 0)
      {
        // The content of the file is not predefined. However, some other data server might
        // have already registered the content.

        if (currentContentList.getLength() == 0)
        {
          // The content of the file is not registered. So, we need to read it.

          gridFile->read(mDataDir + "/" + fileInfo.mName);
        }
      }

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

    gridFile->setCheckTime(checkTime);

    if ((fileInfo.mFlags & T::FileInfo::Flags::PredefinedContent) != 0  ||  contentList.getLength() > 0)
    {
      uint cLen = currentContentList.getLength();
      GRID::GridFile_sptr gFile;
      for (uint t=0; t<cLen; t++)
      {
        T::ContentInfo *info = currentContentList.getContentInfoByIndex(t);

        GRID::MessageInfo mInfo;

        mInfo.mFilePosition = info->mFilePosition;
        mInfo.mMessageSize = info->mMessageSize;
        mInfo.mProducerId = info->mProducerId;
        mInfo.mGenerationId = info->mGenerationId;
        mInfo.mFmiParameterId = info->mFmiParameterId;
        mInfo.mFmiParameterName = info->mFmiParameterName;
        mInfo.mFmiParameterLevelId = info->mFmiParameterLevelId;
        mInfo.mParameterLevel = info->mParameterLevel;
        mInfo.mForecastType = info->mForecastType;
        mInfo.mForecastNumber = info->mForecastNumber;
        mInfo.mGeometryId = info->mGeometryId;

        gridFile->newMessage(info->mMessageIndex,mInfo);

        //if (mContentPreloadEnabled && (info->mFlags & T::ContentInfo::Flags::PreloadRequired) != 0)

        if (mContentPreloadEnabled)
        {
          char tmp[200];
          sprintf(tmp,"%u;%s;%u;1;%u;%05u;%d;%d;1",info->mProducerId,info->mFmiParameterName.c_str(),info->mGeometryId,info->mFmiParameterLevelId,info->mParameterLevel,info->mForecastType,info->mForecastNumber);
          //printf("FIND %s\n",tmp);
          if (mPreloadDefList.find(toLowerString(std::string(tmp))) != mPreloadDefList.end())
            mPreloadList.push_back(std::pair<uint,uint>(info->mFileId,info->mMessageIndex));
        }
/*
        {
          printf("PRELOAD %s\n",info->mFmiParameterName.c_str());

          if (!gFile)
            gFile = getGridFile(fileInfo.mFileId);

          if (gFile)
          {
            printf("** PRELOAD %s\n",info->mFmiParameterName.c_str());
            GRID::Message *message = gFile->getMessageByIndex(info->mMessageIndex);
            if (message != nullptr)
              message->lockData();
              //message->getGridValueByGridPoint(0,0);
          }
        }
        */

      }

      // The content of the file is predefined or registered by another server. So,
      // we can just register our server for the current content (if we have not already
      // registered. This is possible if the content server has been down).

      if (mServerId > 0  &&  mContentRegistrationEnabled)
      {
        unsigned long long sf = (1 << (mServerId-1));

        T::ContentInfo *info = contentList.getContentInfoByIndex(0);
        if (info == nullptr  ||  (info->mServerFlags & sf) == 0)
          mContentServer->registerContentListByFileId(mServerSessionId,mServerId,fileInfo.mFileId);
      }


      if (contentInfoList.getLength() == 0)
      {
        //T::ContentInfoList tmpContentList;
        //mContentServer->getContentListByFileId(mServerSessionId,fileInfo.mFileId,tmpContentList);
        //fullContentList->getContentListByFileId(fileInfo.mFileId,tmpContentList);
        if (mVirtualContentEnabled)
          mVirtualContentManager.addFile(producerInfo,generationInfo,fileInfo,currentContentList,mGridFileMap);
      }
      else
      {
        if (mVirtualContentEnabled)
          mVirtualContentManager.addFile(producerInfo,generationInfo,fileInfo,contentList,mGridFileMap);
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
        contentInfo->mMessageIndex = m;
        contentInfo->mForecastTime = message->getForecastTime();
        contentInfo->mFmiParameterId = message->getFmiParameterId();
        contentInfo->mFmiParameterName = message->getFmiParameterName();
        contentInfo->mGribParameterId = message->getGribParameterId();
        contentInfo->mFmiParameterLevelId = message->getFmiParameterLevelId();
        contentInfo->mGrib1ParameterLevelId = message->getGrib1ParameterLevelId();
        contentInfo->mGrib2ParameterLevelId = message->getGrib2ParameterLevelId();
        contentInfo->mParameterLevel = message->getGridParameterLevel();
        contentInfo->mFmiParameterUnits = message->getFmiParameterUnits();
        contentInfo->mGribParameterUnits = message->getGribParameterUnits();
        contentInfo->mCdmParameterId = message->getCdmParameterId();
        contentInfo->mCdmParameterName = message->getCdmParameterName();
        contentInfo->mNewbaseParameterId = message->getNewbaseParameterId();
        contentInfo->mNewbaseParameterName = message->getNewbaseParameterName();
        contentInfo->mForecastType = message->getForecastType();
        contentInfo->mForecastNumber = message->getForecastNumber();
        contentInfo->mFlags = 0;
        contentInfo->mSourceId = fileInfo.mSourceId;
        contentInfo->mGeometryId = message->getGridGeometryId();

        //contentInfo->print(std::cout,0,0);
        contentInfoList.addContentInfo(contentInfo);
      }
      if (mVirtualContentEnabled)
        mVirtualContentManager.addFile(producerInfo,generationInfo,fileInfo,contentList,mGridFileMap);
    }
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
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
        std::string cPos = CODE_LOCATION;
        PRINT_DATA(mDebugLog,"%s: Cannot get the file list from the content server!",cPos.c_str());
        PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
        return;
      }

      T::ContentInfoList contentInfoList;
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

          T::ContentInfoList currentContentList;
          if (mContentServer->getImplementationType() != ContentServer::Implementation::Cache || fullContentList.getLength() > 0)
            fullContentList.getContentInfoListByFileId(fileInfo->mFileId,currentContentList);
          else
            mContentServer->getContentListByFileId(mServerSessionId,fileInfo->mFileId,currentContentList);

          addFile(*fileInfo,currentContentList,contentInfoList);
          if (contentInfoList.getLength() > 1000)
          {
            mContentServer->registerContentList(mServerSessionId,mServerId,contentInfoList);
            contentInfoList.clear();
          }

          counter++;
          if ((counter % 10000) == 0)
            PRINT_DATA(mDebugLog,"* Adding grid files : %u\n",counter);
        }
        catch (...)
        {
          SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
          std::string st = exception.getStackTrace();
          PRINT_DATA(mDebugLog,"%s",st.c_str());
        }
      }

      if (contentInfoList.getLength() > 0)
      {
        mContentServer->registerContentList(mServerSessionId,mServerId,contentInfoList);
        contentInfoList.clear();
      }
    }

    PRINT_DATA(mDebugLog,"* Adding grid files : %u\n",counter);

    mVirtualContentEnabled  = vContentEnabled;
    mGridFileManager.deleteFilesByCheckTime(checkTime);
    mGridFileManager.deleteVirtualFiles();

    updateVirtualFiles(fullContentList);

    mFullUpdateRequired = false;

    PRINT_DATA(mDebugLog,"******************* FULL UPDATE END **********************\n");
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::event_clear(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: clear\n",eventInfo.mEventId);

    PRINT_DATA(mDebugLog,"*** Clear event : All content files deleted ****\n");
    mGridFileManager.clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::event_producerDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: producerDeleted(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridFileManager.deleteFilesByProducerId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::event_producerListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: producerListDeletedBySourceId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridFileManager.deleteFilesBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::event_generationDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: generationDeleted(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridFileManager.deleteFilesByGenerationId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::event_generationListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: generationListDeletedByProducerId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridFileManager.deleteFilesByProducerId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::event_generationListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: generationListDeletedBySourceId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridFileManager.deleteFilesBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::event_fileAdded(T::EventInfo& eventInfo,T::EventInfo *nextEventInfo)
{
  FUNCTION_TRACE
  try
  {
    uint len = eventInfo.mNote.length();
    T::FileInfo fileInfo;
    T::ContentInfoList currentContentList;

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
            fileInfo.setCsv(s);
          }
          else
          {
            T::ContentInfo *contentInfo = new T::ContentInfo();
            contentInfo->setCsv(s);
            currentContentList.addContentInfo(contentInfo);
          }
          s = p + 1;
        }
        else
        {
          s = nullptr;
        }
      }
      delete buf;
    }
    else
    {
      int result = mContentServer->getFileInfoById(mServerSessionId,eventInfo.mId1,fileInfo);
      if (result != 0)
      {
        std::string cPos = CODE_LOCATION;
        PRINT_DATA(mDebugLog,"%s: Cannot get the file info (fileId=%u) from the content server\n",cPos.c_str(),eventInfo.mId1);
        PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
        return;
      }

      result = mContentServer->getContentListByFileId(mServerSessionId,fileInfo.mFileId,currentContentList);
      if (result != 0)
      {
        std::string cPos = CODE_LOCATION;
        PRINT_DATA(mDebugLog,"%s: Cannot get the content list (fileId=%d) from the content server!\n",cPos.c_str(),fileInfo.mFileId);
        PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
        return;
      }
    }

    T::ContentInfoList contentInfoList;
    addFile(fileInfo,currentContentList,contentInfoList);
    if (contentInfoList.getLength() > 0)
    {
      mContentServer->addContentList(mServerSessionId,contentInfoList);
    }

    uint cnt = mGridFileManager.getFileCount();
    if ((cnt % 1000) == 0)
      PRINT_DATA(mDebugLog,"** file added %u\n",cnt);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}



#if 0
void ServiceImplementation::event_fileAdded(T::EventInfo& eventInfo,T::EventInfo *nextEventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: fileAdded(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    if (eventInfo.mId2 == T::FileTypeValue::Virtual)
      return; // The added file was virtual. No need to react.

    mFileAdditionList.push_back(eventInfo.mId1);

    if (nextEventInfo == nullptr  ||  nextEventInfo->mType != ContentServer::EventType::FILE_ADDED  ||  mFileAdditionList.size() > 1000)
    {
      T::FileInfoList fileInfoList;
      int result = mContentServer->getFileInfoListByFileIdList(mServerSessionId,mFileAdditionList,fileInfoList);
      if (result != 0)
      {
        std::string cPos = CODE_LOCATION;
        PRINT_DATA(mDebugLog,"%s: Cannot get the file info list from the content server\n",cPos.c_str());
        PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
        return;
      }

      T::ContentInfoList contentInfoList;
      result = mContentServer->getContentListByFileIdList(mServerSessionId,mFileAdditionList,contentInfoList);
      if (result != 0)
      {
        std::string cPos = CODE_LOCATION;
        PRINT_DATA(mDebugLog,"%s: Cannot get the content list from the content server!\n",cPos.c_str());
        PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
        return;
      }

      uint len = fileInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *fileInfo = fileInfoList.getFileInfoByIndex(t);
        if (fileInfo != nullptr)
        {
          T::ContentInfoList contentList;
          contentInfoList.getContentInfoListByFileId(fileInfo->mFileId,contentList);

          T::ContentInfoList cList;
          addFile(*fileInfo,contentList,cList);
          if (cList.getLength() > 0)
          {
            mContentServer->addContentList(mServerSessionId,cList);
          }

          if ((fileInfo->mFileId % 1000) == 0)
            PRINT_DATA(mDebugLog,"** fileAdded %lu\n",mGridFileManager.getFileCount());
        }
      }

      mFileAdditionList.clear();
    }

/*
    T::FileInfo fileInfo;
    int result = mContentServer->getFileInfoById(mServerSessionId,eventInfo.mId1,fileInfo);
    if (result != 0)
    {
      std::string cPos = CODE_LOCATION;
      PRINT_DATA(mDebugLog,"%s: Cannot get the file info (fileId=%u) from the content server\n",cPos.c_str(),eventInfo.mId1);
      PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
      return;
    }

    T::ContentInfoList currentContentList;
    result = mContentServer->getContentListByFileId(mServerSessionId,fileInfo.mFileId,currentContentList);
    if (result != 0)
    {
      std::string cPos = CODE_LOCATION;
      PRINT_DATA(mDebugLog,"%s: Cannot get the content list (fileId=%d) from the content server!\n",cPos.c_str(),fileInfo.mFileId);
      PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
      return;
    }

    T::ContentInfoList contentInfoList;
    addFile(fileInfo,currentContentList,contentInfoList);
    if (contentInfoList.getLength() > 0)
    {
      mContentServer->addContentList(mServerSessionId,contentInfoList);
    }

    if ((fileInfo.mFileId % 1000) == 0)
      PRINT_DATA(mDebugLog,"** fileAdded %lu\n",mGridFileManager.getFileCount());
*/
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}
#endif




void ServiceImplementation::event_fileDeleted(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: filedDeleted(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    if (eventInfo.mId2 == T::FileTypeValue::Virtual)
      return; // The deleted file was virtual. No need to react.

    mGridFileManager.deleteFileById(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      std::string cPos = CODE_LOCATION;
      PRINT_DATA(mDebugLog,"%s: Cannot get the file info (fileId=%u) from the content server\n",cPos.c_str(),eventInfo.mId1);
      PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
      return;
    }

    T::ContentInfoList currentContentList;

    // If the content list is not predefined the we should remove it and replace it with a new list.
    if ((fileInfo.mFlags & T::FileInfo::Flags::PredefinedContent) == 0)
    {
      mContentServer->deleteContentListByFileId(mServerSessionId,eventInfo.mId1);
    }
    else
    {
      result = mContentServer->getContentListByFileId(mServerSessionId,fileInfo.mFileId,currentContentList);
      if (result != 0)
      {
        std::string cPos = CODE_LOCATION;
        PRINT_DATA(mDebugLog,"%s: Cannot get the content list (fileId=%d) from the content server!\n",cPos.c_str(),fileInfo.mFileId);
        PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
        return;
      }
    }

    T::ContentInfoList contentInfoList;
    addFile(fileInfo,currentContentList,contentInfoList);
    if (contentInfoList.getLength() > 0)
    {
      mContentServer->addContentList(mServerSessionId,contentInfoList);
    }
#endif
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::event_fileListDeletedByGroupFlags(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: fileListDeletedByGroupFlags(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridFileManager.deleteFilesByGroupFlags(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::event_fileListDeletedByProducerId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: fileListDeletedByProducerId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridFileManager.deleteFilesByProducerId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::event_fileListDeletedByGenerationId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: fileListDeletedByGenerationId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridFileManager.deleteFilesByGenerationId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}






void ServiceImplementation::event_fileListDeletedBySourceId(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: fileListDeletedBySourceId(%u)\n",eventInfo.mEventId,eventInfo.mId1);

    mGridFileManager.deleteFilesBySourceId(eventInfo.mId1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

      //printf("**** SHUTDOWN REQUIRED ****\n");
      mShutdownRequested = true;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void ServiceImplementation::event_contentAdded(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("EVENT[%llu]: contentAdded(%u,%u)\n",eventInfo.mEventId,eventInfo.mId1,eventInfo.mId2);

    T::ContentInfo contentInfo;
    if (eventInfo.mNote > " ")
      contentInfo.setCsv(eventInfo.mNote);
    else
    {
      if (mContentServer->getContentInfo(mServerSessionId,eventInfo.mId1,eventInfo.mId2,contentInfo) != Result::OK)
        return;
    }

    GRID::GridFile_sptr storageFile = mGridFileManager.getFileByIdNoMapping(eventInfo.mId1);

    GRID::GridFile *gridFile = nullptr;
    if (storageFile)
    {
      gridFile = storageFile.get();
      try
      {
        GRID::MessageInfo mInfo;

        mInfo.mFilePosition = contentInfo.mFilePosition;
        mInfo.mMessageSize = contentInfo.mMessageSize;
        mInfo.mProducerId = contentInfo.mProducerId;
        mInfo.mGenerationId = contentInfo.mGenerationId;
        mInfo.mFmiParameterId = contentInfo.mFmiParameterId;
        mInfo.mFmiParameterName = contentInfo.mFmiParameterName;
        mInfo.mFmiParameterLevelId = contentInfo.mFmiParameterLevelId;
        mInfo.mParameterLevel = contentInfo.mParameterLevel;
        mInfo.mForecastType = contentInfo.mForecastType;
        mInfo.mForecastNumber = contentInfo.mForecastNumber;
        mInfo.mGeometryId = contentInfo.mGeometryId;

        gridFile->newMessage(contentInfo.mMessageIndex,mInfo);

        //if (mContentPreloadEnabled && (contentInfo.mFlags & T::ContentInfo::Flags::PreloadRequired) != 0)

        if (mContentPreloadEnabled)
        {
          char tmp[200];
          sprintf(tmp,"%u;%s;%u;1;%u;%05u;%d;%d;1",contentInfo.mProducerId,contentInfo.mFmiParameterName.c_str(),contentInfo.mGenerationId,contentInfo.mFmiParameterLevelId,contentInfo.mParameterLevel,contentInfo.mForecastType,contentInfo.mForecastNumber);
          if (mPreloadDefList.find(toLowerString(std::string(tmp))) != mPreloadDefList.end())
            mPreloadList.push_back(std::pair<uint,uint>(contentInfo.mFileId,contentInfo.mMessageIndex));
        }

        if (mVirtualContentEnabled)
        {
          T::ProducerInfo producerInfo;
          T::GenerationInfo generationInfo;
          T::FileInfo fileInfo;
          T::ContentInfoList contentList;
          contentList.addContentInfo(contentInfo.duplicate());

          mContentServer->getProducerInfoById(mServerSessionId,contentInfo.mProducerId,producerInfo);
          mContentServer->getGenerationInfoById(mServerSessionId,contentInfo.mGenerationId,generationInfo);
          mContentServer->getFileInfoById(mServerSessionId,contentInfo.mFileId,fileInfo);

          mVirtualContentManager.addFile(producerInfo,generationInfo,fileInfo,contentList,mGridFileMap);
        }
      }
      catch (...)
      {
        SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
        exception.printError();
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::event_deleteVirtualContent(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"* Delete virtual content\n");
    mGridFileManager.deleteVirtualFiles();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::event_updateVirtualContent(T::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
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

    updateVirtualFiles(fullContentList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::processEvent(T::EventInfo& eventInfo,T::EventInfo *nextEventInfo)
{
  FUNCTION_TRACE
  try
  {
    //printf("*** EVENT %llu %u\n",eventInfo.mEventId,eventInfo.mType);
    if (mGridFileMap.size() > 10000  ||  (mGridFileMap.size() > 0  &&  (mLastVirtualFileRegistration + 60) < time(nullptr)))
    {
      registerVirtualFiles(mGridFileMap);
      mGridFileMap.clear();
    }

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
        event_fileAdded(eventInfo,nextEventInfo);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void ServiceImplementation::loadPreloadList()
{
  FUNCTION_TRACE
  try
  {
    if (!mContentPreloadEnabled)
      return;

    FILE *file = fopen(mPreloadFile.c_str(), "re");
    if (file == nullptr)
    {
      PRINT_DATA(mDebugLog, "  -- Preload file not available (%s).\n",mPreloadFile.c_str());
      return;
    }

    mPreloadFile_modificationTime = getFileModificationTime(mPreloadFile.c_str());

    T::ProducerInfoList producerInfoList;
    mContentServer->getProducerInfoList(mServerSessionId,producerInfoList);

    mPreloadDefList.clear();

    char st[1000];
    while (!feof(file))
    {
      if (fgets(st, 1000, file) != nullptr)
      {
        if (st[0] != '#')
        {
          char *p = st;
          while (*p != '\0')
          {
            if (*p <= ' ')
              *p = '\0';
            else
              p++;
          }

          std::vector <std::string> a;
          splitString(st, ';', a);
          if (a.size() == 9)
          {
            if (a[8] == "1")
            {
              T::ProducerInfo *producer = producerInfoList.getProducerInfoByName(a[0]);
              if (producer != nullptr)
              {
                char tmp[200];
                sprintf(tmp,"%u;%s;%s;%s;%s;%s;%s;%s;%s",producer->mProducerId,a[1].c_str(),a[2].c_str(),a[3].c_str(),a[4].c_str(),a[5].c_str(),a[6].c_str(),a[7].c_str(),a[8].c_str());
                //printf("%s\n",tmp);
                mPreloadDefList.insert(toLowerString(std::string(tmp)));
              }
            }
          }
        }
      }
    }
    fclose(file);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, exception_operation_failed, nullptr);
  }
}




void ServiceImplementation::checkServerRegistration()
{
  //FUNCTION_TRACE
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

        PRINT_DATA(mDebugLog,"***** The same server is registered with different IOR. ********\n");
        PRINT_DATA(mDebugLog,"***** Shutting down the server! ****\n");
        mShutdownRequested = true;
        return;
      }
    }
    else
    {
      if (result != ContentServer::DATA_NOT_FOUND)
      {
        //printf("ERROR: getDataServerInfoById : %d\n",result);
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
          sprintf(msg,"ERROR addDataServerInfo (%d) : %s\n",result2,ContentServer::getResultString(result2).c_str());
          throw SmartMet::Spine::Exception(BCP,msg);
        }

        //printf("SERVER REGISTERED\n");
        //serverInfo.print(std::cout,0,0);

        mFullUpdateRequired = true;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void ServiceImplementation::processEvents()
{
  //FUNCTION_TRACE
  try
  {
    if (mShutdownRequested)
      return;

    checkServerRegistration();
    mLuaFileCollection.checkUpdates(false);

    if (mFullUpdateRequired)
    {
      fullUpdate();
      return;
    }

    time_t sTime = time(0);
    while (mContentPreloadEnabled  &&  !mPreloadList.empty()  &&  (time(0)-sTime) < 10)
    {
      auto it = mPreloadList.front();
      //printf("###### PRELOAD %u:%u\n",it.first,it.second);

      auto gFile = mGridFileManager.getFileByIdNoMapping(it.first);
      if (gFile)
      {
        if (!gFile->isMemoryMapped())
          gFile->mapToMemory();

        GRID::Message *message = gFile->getMessageByIndex(it.second);
        if (message != nullptr)
        {
          PRINT_DATA(mDebugLog,"** PRELOAD (%lu) : %s:%u:%s:%u:%u:%d:%d\n",mPreloadList.size(),message->getForecastTime().c_str(),message->getProducerId(),message->getFmiParameterName().c_str(),message->getFmiParameterLevelId(),message->getGridParameterLevel(),message->getForecastType(),message->getForecastNumber());

          if (mPreloadMemoryLock)
            message->lockData();

          T::Dimensions d = message->getGridDimensions();
          int geometryId = message->getGridGeometryId();

          HitCounter hitCounter = requestCounter.getGeometryHitCounters(geometryId);

          // printf("---- hit counter %lu\n",hitCounter.size());

          for (auto hh = hitCounter.begin(); hh != hitCounter.end(); ++hh)
          {
            uint x = hh->first  % d.nx();
            uint y = hh->first / d.nx();
            message->getGridValueByGridPoint(x,y);
          }
        }
      }

      mPreloadList.pop_front();
    }


    T::EventInfo eventInfo;
    int result = mContentServer->getLastEventInfo(mServerSessionId,mServerId,eventInfo);
    if (result == Result::OK)
    {
      if (eventInfo.mServerTime > mContentServerStartTime)
      {
        if (mContentServerStartTime > 0)
        {
          PRINT_DATA(mDebugLog,"**** The content server was restarted! *****\n");
          fullUpdate();
        }

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

    uint len = 10000;
    while (len > 0)
    {
      T::EventInfoList eventInfoList;
      result = mContentServer->getEventInfoList(mServerSessionId,mServerId,mLastProcessedEventId+1,10000,eventInfoList);
      if (result != 0)
      {
        std::string cPos = CODE_LOCATION;
        PRINT_DATA(mDebugLog,"%s : Cannot get the event info list from the content server!\n",cPos.c_str());
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

    if ((mLastCacheCheck + 120) < time(0))
    {
      mGridFileManager.clearCachedValues(mPointCacheHitsRequired,mPointCacheTimePeriod);
      mLastCacheCheck = time(0);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
        SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
        std::string st = exception.getStackTrace();
        PRINT_DATA(mDebugLog,"%s",st.c_str());
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::processRequestCounters()
{
  FUNCTION_TRACE
  try
  {
    if (requestCounter.getTotalRequests() > 1000000)
    {
      requestCounter.saveGeometryHitCounters(mGeneratedCounterFile.c_str());
      requestCounter.setCountingEnabled(false);
      requestCounter.resetTotalRequests();
      requestCounter.updateTopCounters();
      //requestCounter.saveTopCounters(mPreloadFile.c_str());
      requestCounter.multiplyCounters(0.9);
      requestCounter.setCountingEnabled(true);
    }

    T::ProducerInfoList producerInfoList;
    mContentServer->getProducerInfoList(mServerSessionId,producerInfoList);

    TopList toprc = requestCounter.getTopRequestCounters();
    std::set<std::string> preloadList;
    std::set<std::string> preloadList2;

    for (auto it = toprc.begin(); it != toprc.end(); ++it)
    {
      if (mShutdownRequested)
        return;

      if (it->first > 10)  // *** This limit should be defined in the configuration file
      {
        T::ContentInfoList contentInfoList;

        int result = mContentServer->getContentListByRequestCounterKey(mServerSessionId,it->second,contentInfoList);
        if (result != 0)
        {
          std::string cPos = CODE_LOCATION;
          PRINT_DATA(mDebugLog,"%s: Cannot get the content list (key=%llu) from the content server!\n",cPos.c_str(),it->second);
          PRINT_DATA(mDebugLog,"-- %d : %s\n",result,ContentServer::getResultString(result).c_str());
          return;
        }

        uint len = contentInfoList.getLength();

        for (uint t=0; t<len; t++)
        {
          if (mShutdownRequested)
            return;

          T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
          if (info != nullptr)
          {
            char tmp[100];
            sprintf(tmp,"%u;%s;%u;%u;%u;%d;%d;1",info->mProducerId,info->mFmiParameterName.c_str(),info->mGeometryId,info->mFmiParameterLevelId,info->mParameterLevel,info->mForecastType,info->mForecastNumber);

            if (preloadList.find(tmp) == preloadList.end())
            {
              preloadList.insert(tmp);
              auto producer = producerInfoList.getProducerInfoById(info->mProducerId);
              if (producer != nullptr)
              {
                sprintf(tmp,"%s;%s;%u;1;%u;%05u;%d;%d;1",producer->mName.c_str(),info->mFmiParameterName.c_str(),info->mGeometryId,info->mFmiParameterLevelId,info->mParameterLevel,info->mForecastType,info->mForecastNumber);
                preloadList2.insert(tmp);
              }
            }
          }
        }
      }
    }

    if (preloadList2.size() > 0)
    {
      FILE *file = fopen(mGeneratedPreloadFile.c_str(),"w");
      if (file != nullptr)
      {
        fprintf(file,"# This file is used for indicating which content should be preloaded.\n");
        fprintf(file,"#\n");
        fprintf(file,"# FIELDS\n");
        fprintf(file,"#  1) Producer name\n");
        fprintf(file,"#  2) Parameter name (Radon name)\n");
        fprintf(file,"#  3) GeometryId\n");
        fprintf(file,"#  4) Parameter level id type:\n");
        fprintf(file,"#         1 = FMI\n");
        fprintf(file,"#         2 = GRIB1\n");
        fprintf(file,"#         3 = GRIB2\n");
        fprintf(file,"#  5) Level id\n");
        fprintf(file,"#         FMI level identifiers:\n");
        fprintf(file,"#            1 Gound or water surface\n");
        fprintf(file,"#            2 Pressure level\n");
        fprintf(file,"#            3 Hybrid level\n");
        fprintf(file,"#            4 Altitude\n");
        fprintf(file,"#            5 Top of atmosphere\n");
        fprintf(file,"#            6 Height above ground in meters\n");
        fprintf(file,"#            7 Mean sea level\n");
        fprintf(file,"#            8 Entire atmosphere\n");
        fprintf(file,"#            9 Depth below land surface\n");
        fprintf(file,"#            10 Depth below some surface\n");
        fprintf(file,"#            11 Level at specified pressure difference from ground to level\n");
        fprintf(file,"#            12 Max equivalent potential temperature level\n");
        fprintf(file,"#            13 Layer between two metric heights above ground\n");
        fprintf(file,"#            14 Layer between two depths below land surface\n");
        fprintf(file,"#            15 Isothermal level, temperature in 1/100 K\n");
        fprintf(file,"#  6) Level\n");
        fprintf(file,"#  7) ForecastType\n");
        fprintf(file,"#  8) ForecastNumber\n");
        fprintf(file,"#  9) Preload requested (0 = No, 1 = Yes)\n");
        fprintf(file,"#\n");

        for (auto it = preloadList2.begin();it != preloadList2.end(); ++it)
        {
          fprintf(file,"%s\n",it->c_str());
        }
        fclose(file);
      }
    }

  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::requestCounterThread()
{
  FUNCTION_TRACE
  try
  {
    if (!mPreloadFileGenerationEnabled)
      return;

    sleep(5);

    while (!mShutdownRequested)
    {
      try
      {
        while (mFullUpdateRequired  &&  !mShutdownRequested)
          sleep(1);

        mRequestCountingActive = true;
        processRequestCounters();
      }
      catch (...)
      {
        SmartMet::Spine::Exception exception(BCP,exception_operation_failed,nullptr);
        std::string st = exception.getStackTrace();
        PRINT_DATA(mDebugLog,"%s",st.c_str());
      }

      for (int t=0; t<60; t++)
      {
        if (!mShutdownRequested)
          sleep(1);
      }

      if (mPreloadFile_modificationTime != getFileModificationTime(mPreloadFile.c_str()))
        loadPreloadList();

      time_t tt = getFileModificationTime(mCounterFile.c_str());

      if (tt != mCounterFile_modificationTime)
      {
        mCounterFile_modificationTime = tt;
        requestCounter.loadGeometryHitCounters(mCounterFile.c_str());
      }
    }

    mRequestCountingActive = false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceImplementation::shutdown()
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"*** SHUTDOWN ***\n");
    mShutdownRequested = true;
    while (mEventProcessingActive || mRequestCountingActive)
      sleep(1);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}

}
}

