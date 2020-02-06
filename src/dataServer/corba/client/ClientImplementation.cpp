#include "ClientImplementation.h"
#include "../convert/Converter.h"

#include <grid-files/common/Exception.h>


namespace SmartMet
{
namespace DataServer
{
namespace Corba
{

#define CATCH_EXCEPTION \
  catch (CORBA::TRANSIENT&)\
  {\
    Spine::Exception exception(BCP, "Caught system exception TRANSIENT -- unable to connect the server!");\
    throw exception;\
  }\
  catch (CORBA::SystemException& ex)\
  {\
    char msg[500];\
    sprintf(msg, "Caught a CORBA::%s\n", ex._name());\
    Spine::Exception exception(BCP, msg);\
    throw exception;\
  }\
  catch (CORBA::Exception& ex)\
  {\
    char msg[500];\
    sprintf(msg, "Exception CORBA::%s\n", ex._name());\
    Spine::Exception exception(BCP, msg);\
    throw exception;\
  }\
  catch (omniORB::fatalException& fe)\
  {\
    char msg[500];\
    sprintf(msg, "Caught omniORB::fatalException:%s\n", fe.errmsg());\
    Spine::Exception exception(BCP, msg);\
    throw exception;\
  }\
  catch (...)\
  {\
    throw Spine::Exception(BCP, exception_operation_failed, nullptr);\
  }




ClientImplementation::ClientImplementation()
{
  try
  {
    mInitialized = false;
    mService = nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}






ClientImplementation::~ClientImplementation()
{
  try
  {
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void ClientImplementation::init(std::string serviceIor)
{
  try
  {
    mServiceIor = serviceIor;

    int argc = 2;
    char *argv[] = { const_cast<char*>("-ORBgiopMaxMsgSize"),const_cast<char*>("500000000") };
    CORBA::ORB_var orb = CORBA::ORB_init(argc,argv);

    CORBA::Object_var obj;
    obj = orb->string_to_object(serviceIor.c_str());

    mService = DataServer::Corba::ServiceInterface::_narrow(obj);

    if (CORBA::is_nil(mService))
    {
      SmartMet::Spine::Exception exception(BCP,"Can't narrow reference to type DataServer::Corba::ServiceInterace (or it was nil)!");
      throw exception;
    }

    omniORB::setClientCallTimeout(3600000);

    mInitialized = true;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,T::GridCoordinates& coordinates)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaGridCoordinates_var corbaGridCoordinates;

    int result = mService->getGridCoordinates(sessionId,fileId,messageIndex,coordinateType,corbaGridCoordinates);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridCoordinates,coordinates);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaGridData_var corbaGridData;

    int result = mService->getGridData(sessionId,fileId,messageIndex,corbaGridData);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridData,data);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaAttributeList_var corbaAttributeList;

    int result = mService->getGridAttributeList(sessionId,fileId,messageIndex,corbaAttributeList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridFileCount(T::SessionId sessionId,uint& count)
{
  try
  {
    if (!mInitialized)
      throw Spine::Exception(BCP,"The client is not initialized!");

    ::CORBA::ULong corbaCount = 0;

    int result = mService->getGridFileCount(sessionId,corbaCount);

    if (result == 0)
      count = corbaCount;

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridMessageBytes(T::SessionId sessionId,uint fileId,uint messageIndex,std::vector<uchar>& messageBytes,std::vector<uint>& messageSections)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaByteData_var corbaMessageBytes;
    DataServer::Corba::CorbaULongList_var corbaMessageSections;


    int result = mService->getGridMessageBytes(sessionId,fileId,messageIndex,corbaMessageBytes,corbaMessageSections);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaMessageBytes,messageBytes);
      DataServer::Corba::Converter::convert(corbaMessageSections,messageSections);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,T::ParamValue& value)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    SmartMet::DataServer::Corba::CorbaParamValue corbaValue;

    int result = mService->getGridValueByPoint(sessionId,fileId,messageIndex,(CORBA::Octet)coordinateType,x,y,(CORBA::Short)areaInterpolationMethod,corbaValue);

    if (result == 0)
      value = corbaValue;

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueByLevelAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,int level1,uint fileId2,uint messageIndex2,int level2,int newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short levelInterpolationMethod,T::ParamValue& value)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    SmartMet::DataServer::Corba::CorbaParamValue corbaValue;

    int result = mService->getGridValueByLevelAndPoint(sessionId,fileId1,messageIndex1,level1,fileId2,messageIndex2,level2,newLevel,(CORBA::Octet)coordinateType,x,y,areaInterpolationMethod,levelInterpolationMethod,corbaValue);

    if (result == 0)
      value = corbaValue;

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueByTimeAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,T::ParamValue& value)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    SmartMet::DataServer::Corba::CorbaParamValue corbaValue;

    int result = mService->getGridValueByTimeAndPoint(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),(CORBA::Octet)coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,corbaValue);

    if (result == 0)
      value = corbaValue;

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueByTimeLevelAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,int level1,uint fileId2,uint messageIndex2,int level2,uint fileId3,uint messageIndex3,int level3,uint fileId4,uint messageIndex4,int level4,std::string newTime,int newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::ParamValue& value)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    SmartMet::DataServer::Corba::CorbaParamValue corbaValue;

    int result = mService->getGridValueByTimeLevelAndPoint(sessionId,fileId1,messageIndex1,level1,fileId2,messageIndex2,level2,fileId3,messageIndex3,level3,fileId4,messageIndex4,level4,newTime.c_str(),newLevel,(CORBA::Octet)coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,corbaValue);

    if (result == 0)
      value = corbaValue;

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;


    int result = mService->getGridValueListByCircle(sessionId,fileId,messageIndex,coordinateType,origoX,origoY,radius,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByLevelAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short levelInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;


    int result = mService->getGridValueListByLevelAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,origoX,origoY,radius,levelInterpolationMethod,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByTimeAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;


    int result = mService->getGridValueListByTimeAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),coordinateType,origoX,origoY,radius,timeInterpolationMethod,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByTimeLevelAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,short levelInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;


    int result = mService->getGridValueListByTimeLevelAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,coordinateType,origoX,origoY,radius,timeInterpolationMethod,levelInterpolationMethod,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaCoordinateList_var corbaPointList = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    DataServer::Corba::Converter::convert(pointList,corbaPointList);


    int result = mService->getGridValueListByPointList(sessionId,fileId,messageIndex,coordinateType,corbaPointList,(::CORBA::Short)areaInterpolationMethod,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByLevelAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short levelInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaCoordinateList_var corbaPointList = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    DataServer::Corba::Converter::convert(pointList,corbaPointList);


    int result = mService->getGridValueListByLevelAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,corbaPointList,areaInterpolationMethod,levelInterpolationMethod,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByTimeAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaCoordinateList_var corbaPointList = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    DataServer::Corba::Converter::convert(pointList,corbaPointList);


    int result = mService->getGridValueListByTimeAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),coordinateType,corbaPointList,areaInterpolationMethod,timeInterpolationMethod,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByTimeLevelAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaCoordinateList_var corbaPointList = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    DataServer::Corba::Converter::convert(pointList,corbaPointList);


    int result = mService->getGridValueListByTimeLevelAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,coordinateType,corbaPointList,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaCoordinateList_var corbaPolygonPoints = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    DataServer::Corba::Converter::convert(polygonPoints,corbaPolygonPoints);


    int result = mService->getGridValueListByPolygon(sessionId,fileId,messageIndex,coordinateType,corbaPolygonPoints,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByLevelAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short levelInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaCoordinateList_var corbaPolygonPoints = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    DataServer::Corba::Converter::convert(polygonPoints,corbaPolygonPoints);


    int result = mService->getGridValueListByLevelAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,corbaPolygonPoints,levelInterpolationMethod,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByTimeAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaCoordinateList_var corbaPolygonPoints = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    DataServer::Corba::Converter::convert(polygonPoints,corbaPolygonPoints);


    int result = mService->getGridValueListByTimeAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),coordinateType,corbaPolygonPoints,timeInterpolationMethod,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByTimeLevelAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,short levelInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaCoordinateList_var corbaPolygonPoints = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    DataServer::Corba::Converter::convert(polygonPoints,corbaPolygonPoints);


    int result = mService->getGridValueListByTimeLevelAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,coordinateType,corbaPolygonPoints,timeInterpolationMethod,levelInterpolationMethod,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaPolygonPath_var corbaPolygonPath = new DataServer::Corba::CorbaPolygonPath();
    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    DataServer::Corba::Converter::convert(polygonPath,corbaPolygonPath);

    int result = mService->getGridValueListByPolygonPath(sessionId,fileId,messageIndex,coordinateType,corbaPolygonPath,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByLevelAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short levelInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaPolygonPath_var corbaPolygonPath = new DataServer::Corba::CorbaPolygonPath();
    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    DataServer::Corba::Converter::convert(polygonPath,corbaPolygonPath);

    int result = mService->getGridValueListByLevelAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,corbaPolygonPath,levelInterpolationMethod,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByTimeAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaPolygonPath_var corbaPolygonPath = new DataServer::Corba::CorbaPolygonPath();
    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    DataServer::Corba::Converter::convert(polygonPath,corbaPolygonPath);

    int result = mService->getGridValueListByTimeAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),coordinateType,corbaPolygonPath,timeInterpolationMethod,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByTimeLevelAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,short levelInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaPolygonPath_var corbaPolygonPath = new DataServer::Corba::CorbaPolygonPath();
    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    DataServer::Corba::Converter::convert(polygonPath,corbaPolygonPath);

    int result = mService->getGridValueListByTimeLevelAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,coordinateType,corbaPolygonPath,timeInterpolationMethod,levelInterpolationMethod,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    int result = mService->getGridValueListByRectangle(sessionId,fileId,messageIndex,coordinateType,x1,y1,x2,y2,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& values)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaValues;

    int result = mService->getGridValueVector(sessionId,fileId,messageIndex,corbaValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaValues,values);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVectorByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,short levelInterpolationMethod,T::ParamValue_vec& values)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaValues;

    int result = mService->getGridValueVectorByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,levelInterpolationMethod,corbaValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaValues,values);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVectorByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,short timeInterpolationMethod,T::ParamValue_vec& values)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaValues;

    int result = mService->getGridValueVectorByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),timeInterpolationMethod,corbaValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaValues,values);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVectorByLevelAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaValues;
    DataServer::Corba::CorbaCoordinateList_var corbaCoordinates = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(coordinates,corbaCoordinates);

    int result = mService->getGridValueVectorByLevelAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,corbaCoordinates,corbaAttributeList,corbaValues);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaValues,values);
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVectorByTimeAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaValues;
    DataServer::Corba::CorbaCoordinateList_var corbaCoordinates = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(coordinates,corbaCoordinates);

    int result = mService->getGridValueVectorByTimeAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),coordinateType,corbaCoordinates,corbaAttributeList,corbaValues);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaValues,values);
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVectorByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaValues;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);

    int result = mService->getGridValueVectorByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,corbaAttributeList,corbaValues);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaValues,values);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVectorByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaValues;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);

    int result = mService->getGridValueVectorByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),corbaAttributeList,corbaValues);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaValues,values);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVectorByCoordinateList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,short areaInterpolationMethod,T::ParamValue_vec& values)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaValues;
    DataServer::Corba::CorbaCoordinateList_var corbaCoordinates = new DataServer::Corba::CorbaCoordinateList();

    DataServer::Corba::Converter::convert(coordinates,corbaCoordinates);

    int result = mService->getGridValueVectorByCoordinateList(sessionId,fileId,messageIndex,(::CORBA::Octet)coordinateType,corbaCoordinates,(::CORBA::Short)areaInterpolationMethod,corbaValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaValues,values);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVectorByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaValues;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);

    int result = mService->getGridValueVectorByGeometry(sessionId,fileId,messageIndex,corbaAttributeList,corbaValues);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaValues,values);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,short areaInterpolationMethod,T::ParamValue_vec& values)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaValues;

    int result = mService->getGridValueVectorByRectangle(sessionId,fileId,messageIndex,(CORBA::Octet)coordinateType,columns,rows,x,y,xStep,yStep,(CORBA::Short)areaInterpolationMethod,corbaValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaValues,values);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVectorByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,uint vectorType,double_vec& valueVector)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaDoubleList_var corbaValueVector;


    int result = mService->getGridValueVectorByPoint(sessionId,fileId,messageIndex,coordinateType,x,y,vectorType,corbaValueVector);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaValueVector,valueVector);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVectorByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::ParamValue_vec& values)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaValues;


    int result = mService->getGridValueVectorByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,corbaValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaValues,values);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVectorByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaValues;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);


    int result = mService->getGridValueVectorByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,corbaAttributeList,corbaValues);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaValues,values);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVectorByTimeLevelAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaValues;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();
    DataServer::Corba::CorbaCoordinateList_var corbaCoordinates = new DataServer::Corba::CorbaCoordinateList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(coordinates,corbaCoordinates);

    int result = mService->getGridValueVectorByTimeLevelAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,coordinateType,corbaCoordinates,corbaAttributeList,corbaValues);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaValues,values);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsobands(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourLowValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaParamValueList_var corbaContourHighValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourLowValues,corbaContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,corbaContourHighValues);

    int result = mService->getGridIsobands(sessionId,fileId,messageIndex,corbaContourLowValues,corbaContourHighValues,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsobandsByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourLowValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaParamValueList_var corbaContourHighValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourLowValues,corbaContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,corbaContourHighValues);

    int result = mService->getGridIsobandsByGeometry(sessionId,fileId,messageIndex,corbaContourLowValues,corbaContourHighValues,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsobandsByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourLowValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaParamValueList_var corbaContourHighValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaCoordinateList_var corbaCoordinates = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(gridLatLonCoordinates,corbaCoordinates);
    DataServer::Corba::Converter::convert(contourLowValues,corbaContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,corbaContourHighValues);

    int result = mService->getGridIsobandsByGrid(sessionId,fileId,messageIndex,corbaContourLowValues,corbaContourHighValues,gridWidth,gridHeight,corbaCoordinates,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsobandsByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourLowValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaParamValueList_var corbaContourHighValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourLowValues,corbaContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,corbaContourHighValues);

    int result = mService->getGridIsobandsByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,corbaContourLowValues,corbaContourHighValues,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsobandsByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourLowValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaParamValueList_var corbaContourHighValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourLowValues,corbaContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,corbaContourHighValues);

    int result = mService->getGridIsobandsByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),corbaContourLowValues,corbaContourHighValues,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsobandsByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourLowValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaParamValueList_var corbaContourHighValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourLowValues,corbaContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,corbaContourHighValues);

    int result = mService->getGridIsobandsByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,corbaContourLowValues,corbaContourHighValues,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsobandsByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourLowValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaParamValueList_var corbaContourHighValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourLowValues,corbaContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,corbaContourHighValues);

    int result = mService->getGridIsobandsByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),corbaContourLowValues,corbaContourHighValues,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsobandsByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourLowValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaParamValueList_var corbaContourHighValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaCoordinateList_var corbaCoordinates = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(gridLatLonCoordinates,corbaCoordinates);
    DataServer::Corba::Converter::convert(contourLowValues,corbaContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,corbaContourHighValues);

    int result = mService->getGridIsobandsByLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,corbaContourLowValues,corbaContourHighValues,gridWidth,gridHeight,corbaCoordinates,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsobandsByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourLowValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaParamValueList_var corbaContourHighValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaCoordinateList_var corbaCoordinates = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(gridLatLonCoordinates,corbaCoordinates);
    DataServer::Corba::Converter::convert(contourLowValues,corbaContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,corbaContourHighValues);

    int result = mService->getGridIsobandsByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),corbaContourLowValues,corbaContourHighValues,gridWidth,gridHeight,corbaCoordinates,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsobandsByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourLowValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaParamValueList_var corbaContourHighValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourLowValues,corbaContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,corbaContourHighValues);

    int result = mService->getGridIsobandsByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,corbaContourLowValues,corbaContourHighValues,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsobandsByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourLowValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaParamValueList_var corbaContourHighValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourLowValues,corbaContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,corbaContourHighValues);

    int result = mService->getGridIsobandsByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,corbaContourLowValues,corbaContourHighValues,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsobandsByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourLowValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaParamValueList_var corbaContourHighValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();
    DataServer::Corba::CorbaCoordinateList_var corbaCoordinates = new DataServer::Corba::CorbaCoordinateList();

    DataServer::Corba::Converter::convert(gridLatLonCoordinates,corbaCoordinates);
    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourLowValues,corbaContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,corbaContourHighValues);

    int result = mService->getGridIsobandsByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,corbaContourLowValues,corbaContourHighValues,gridWidth,gridHeight,corbaCoordinates,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}




int ClientImplementation::_getGridIsolinesByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourValues,corbaContourValues);

    int result = mService->getGridIsolinesByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,corbaContourValues,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsolinesByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourValues,corbaContourValues);

    int result = mService->getGridIsolinesByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,corbaContourValues,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsolines(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourValues,corbaContourValues);

    int result = mService->getGridIsolines(sessionId,fileId,messageIndex,corbaContourValues,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsolinesByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourValues,corbaContourValues);

    int result = mService->getGridIsolinesByGeometry(sessionId,fileId,messageIndex,corbaContourValues,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsolinesByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaCoordinateList_var corbaCoordinates = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(gridLatLonCoordinates,corbaCoordinates);
    DataServer::Corba::Converter::convert(contourValues,corbaContourValues);

    int result = mService->getGridIsolinesByGrid(sessionId,fileId,messageIndex,corbaContourValues,gridWidth,gridHeight,corbaCoordinates,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsolinesByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourValues,corbaContourValues);

    int result = mService->getGridIsolinesByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,corbaContourValues,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsolinesByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourValues,corbaContourValues);

    int result = mService->getGridIsolinesByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),corbaContourValues,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsolinesByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourValues,corbaContourValues);

    int result = mService->getGridIsolinesByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,corbaContourValues,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsolinesByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourValues,corbaContourValues);

    int result = mService->getGridIsolinesByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),corbaContourValues,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsolinesByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaCoordinateList_var corbaCoordinates = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(gridLatLonCoordinates,corbaCoordinates);
    DataServer::Corba::Converter::convert(contourValues,corbaContourValues);

    int result = mService->getGridIsolinesByLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,corbaContourValues,gridWidth,gridHeight,corbaCoordinates,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsolinesByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaCoordinateList_var corbaCoordinates = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();

    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(gridLatLonCoordinates,corbaCoordinates);
    DataServer::Corba::Converter::convert(contourValues,corbaContourValues);

    int result = mService->getGridIsolinesByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),corbaContourValues,gridWidth,gridHeight,corbaCoordinates,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridIsolinesByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaContourValues = new DataServer::Corba::CorbaParamValueList();
    DataServer::Corba::CorbaByteDataSequence_var corbaContours;
    DataServer::Corba::CorbaAttributeList_var corbaAttributeList = new DataServer::Corba::CorbaAttributeList();
    DataServer::Corba::CorbaCoordinateList_var corbaCoordinates = new DataServer::Corba::CorbaCoordinateList();

    DataServer::Corba::Converter::convert(gridLatLonCoordinates,corbaCoordinates);
    DataServer::Corba::Converter::convert(attributeList,corbaAttributeList);
    DataServer::Corba::Converter::convert(contourValues,corbaContourValues);

    int result = mService->getGridIsolinesByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,corbaContourValues,gridWidth,gridHeight,corbaCoordinates,corbaAttributeList,corbaContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(corbaAttributeList,attributeList);
      DataServer::Corba::Converter::convert(corbaContours,contours);
    }

    return result;
  }
  CATCH_EXCEPTION
}




int ClientImplementation::_getMultipleGridValues(T::SessionId sessionId,T::ValueRecordList& valueRecordList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaValueRecordList_var corbaValueRecordList = new DataServer::Corba::CorbaValueRecordList();
    DataServer::Corba::Converter::convert(valueRecordList,corbaValueRecordList);

    int result = mService->getMultipleGridValues(sessionId,corbaValueRecordList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaValueRecordList,valueRecordList);

    return result;
  }
  CATCH_EXCEPTION
}





}
}
}
