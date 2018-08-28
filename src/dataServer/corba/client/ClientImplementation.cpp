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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ClientImplementation::init(std::string serviceIor)
{
  try
  {
    mServiceIor = serviceIor;

    int argc = 2;
    char *argv[] = { const_cast<char*>("-ORBgiopMaxMsgSize"),const_cast<char*>("250000000") };
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





int ClientImplementation::_getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,T::GridCoordinates& coordinates)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaGridCoordinates_var corbaGridCoordinates;

    int result = mService->getGridCoordinates(sessionId,fileId,messageIndex,flags,coordinateType,corbaGridCoordinates);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridCoordinates,coordinates);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::GridData& data)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaGridData_var corbaGridData;

    int result = mService->getGridData(sessionId,fileId,messageIndex,flags,corbaGridData);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridData,data);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::AttributeList& attributeList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaAttributeList_var corbaAttributeList;

    int result = mService->getGridAttributeList(sessionId,fileId,messageIndex,flags,corbaAttributeList);

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
      count = (uint)corbaCount;

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::ParamValue& value)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    SmartMet::DataServer::Corba::CorbaParamValue corbaValue;

    int result = mService->getGridValueByPoint(sessionId,fileId,messageIndex,flags,(CORBA::Octet)coordinateType,x,y,(CORBA::Short)interpolationMethod,corbaValue);

    if (result == 0)
      value = (T::ParamValue)corbaValue;

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;


    int result = mService->getGridValueListByCircle(sessionId,fileId,messageIndex,flags,coordinateType,origoX,origoY,radius,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short interpolationMethod,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaCoordinateList_var corbaPointList = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    DataServer::Corba::Converter::convert(pointList,corbaPointList);


    int result = mService->getGridValueListByPointList(sessionId,fileId,messageIndex,flags,coordinateType,corbaPointList,(::CORBA::Short)interpolationMethod,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaCoordinateList_var corbaPolygonPoints = new DataServer::Corba::CorbaCoordinateList();
    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    DataServer::Corba::Converter::convert(polygonPoints,corbaPolygonPoints);


    int result = mService->getGridValueListByPolygon(sessionId,fileId,messageIndex,flags,coordinateType,corbaPolygonPoints,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaPolygonPath_var corbaPolygonPath = new DataServer::Corba::CorbaPolygonPath();
    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    DataServer::Corba::Converter::convert(polygonPath,corbaPolygonPath);


    int result = mService->getGridValueListByPolygonPath(sessionId,fileId,messageIndex,flags,coordinateType,corbaPolygonPath,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,T::GridValueList& valueList)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaGridValueList_var corbaGridValueList;

    int result = mService->getGridValueListByRectangle(sessionId,fileId,messageIndex,flags,coordinateType,x1,y1,x2,y2,corbaGridValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaGridValueList,valueList);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::ParamValue_vec& values)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaValues;

    int result = mService->getGridValueVector(sessionId,fileId,messageIndex,flags,corbaValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaValues,values);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVectorByCoordinateList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,short interpolationMethod,T::ParamValue_vec& values)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaValues;
    DataServer::Corba::CorbaCoordinateList_var corbaCoordinates = new DataServer::Corba::CorbaCoordinateList();

    DataServer::Corba::Converter::convert(coordinates,corbaCoordinates);

    int result = mService->getGridValueVectorByCoordinateList(sessionId,fileId,messageIndex,(::CORBA::Octet)coordinateType,corbaCoordinates,(::CORBA::Short)interpolationMethod,corbaValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaValues,values);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,short interpolationMethod,T::ParamValue_vec& values)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaParamValueList_var corbaValues;

    int result = mService->getGridValueVectorByRectangle(sessionId,fileId,messageIndex,flags,(CORBA::Octet)coordinateType,columns,rows,x,y,xStep,yStep,(CORBA::Short)interpolationMethod,corbaValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaValues,values);

    return result;
  }
  CATCH_EXCEPTION
}





int ClientImplementation::_getGridValueVectorByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,uint vectorType,double_vec& valueVector)
{
  try
  {
    if (!mInitialized)
      throw SmartMet::Spine::Exception(BCP,"The client is not initialized!");

    DataServer::Corba::CorbaDoubleList_var corbaValueVector;


    int result = mService->getGridValueVectorByPoint(sessionId,fileId,messageIndex,flags,coordinateType,x,y,vectorType,corbaValueVector);

    if (result == 0)
      DataServer::Corba::Converter::convert(corbaValueVector,valueVector);

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
