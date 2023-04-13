#include "ServerInterface.h"
#include "../convert/Converter.h"
#include <macgyver/Exception.h>
#include <grid-files/common/ShowFunction.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace DataServer
{
namespace Corba
{


ServerInterface::ServerInterface()
{
  FUNCTION_TRACE
  try
  {
    mService = nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ServerInterface::~ServerInterface()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void ServerInterface::init(DataServer::ServiceInterface *service)
{
  FUNCTION_TRACE
  try
  {
    mService = service;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





::CORBA::Long ServerInterface::getGridCoordinates(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, SmartMet::DataServer::Corba::CorbaGridCoordinates_out coordinates)
{
  FUNCTION_TRACE
  try
  {
    T::GridCoordinates sCoordinates;
    DataServer::Corba::CorbaGridCoordinates *corbaGridCoordinates = new DataServer::Corba::CorbaGridCoordinates();
    coordinates = corbaGridCoordinates;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGridCoordinates(sessionId,fileId,messageIndex,coordinateType,sCoordinates);

    if (result == 0)
      DataServer::Corba::Converter::convert(sCoordinates,*corbaGridCoordinates);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridData(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, SmartMet::DataServer::Corba::CorbaGridData_out data)
{
  FUNCTION_TRACE
  try
  {
    T::GridData sGridData;
    DataServer::Corba::CorbaGridData *corbaGridData = new DataServer::Corba::CorbaGridData();
    data = corbaGridData;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGridData(sessionId,fileId,messageIndex,sGridData);

    if (result == 0)
      DataServer::Corba::Converter::convert(sGridData,*corbaGridData);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridFileCount(::CORBA::LongLong sessionId, ::CORBA::ULong& count)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    uint sCount = 0;
    int result = mService->getGridFileCount(sessionId,sCount);

    if (result == 0)
      count = sCount;

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridMessageBytes(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, SmartMet::DataServer::Corba::CorbaByteData_out messageBytes, SmartMet::DataServer::Corba::CorbaULongList_out messageSections)
{
  FUNCTION_TRACE
  try
  {
    std::vector<uchar> sMessageBytes;
    std::vector<uint> sMessageSections;

    DataServer::Corba::CorbaByteData *corbaMessageBytes = new DataServer::Corba::CorbaByteData();
    messageBytes = corbaMessageBytes;

    DataServer::Corba::CorbaULongList *corbaMessageSections = new DataServer::Corba::CorbaULongList();
    messageSections = corbaMessageSections;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGridMessageBytes(sessionId,fileId,messageIndex,sMessageBytes,sMessageSections);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sMessageBytes,*corbaMessageBytes);
      DataServer::Corba::Converter::convert(sMessageSections,*corbaMessageSections);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridAttributeList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, SmartMet::DataServer::Corba::CorbaAttributeList_out attributeList)
{
  FUNCTION_TRACE
  try
  {
    T::AttributeList sAttributeList;
    DataServer::Corba::CorbaAttributeList *corbaAttributeList = new DataServer::Corba::CorbaAttributeList();
    attributeList = corbaAttributeList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGridAttributeList(sessionId,fileId,messageIndex,sAttributeList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sAttributeList,*corbaAttributeList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueByPoint(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Short areaInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    T::ParamValue sValue = 0;
    int result = mService->getGridValueByPoint(sessionId,fileId,messageIndex,coordinateType,x,y,(short)areaInterpolationMethod,modificationOperation,modParams,sValue);

    if (result == 0)
      value = sValue;

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueByLevelAndPoint(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::Long level1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long level2, ::CORBA::Long newLevel, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Short areaInterpolationMethod, ::CORBA::Short levelInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    T::ParamValue sValue = 0;
    int result = mService->getGridValueByLevelAndPoint(sessionId,fileId1,messageIndex1,level1,fileId2,messageIndex2,level2,newLevel,coordinateType,x,y,areaInterpolationMethod,levelInterpolationMethod,modificationOperation,modParams,sValue);

    if (result == 0)
      value = sValue;

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueByTimeAndPoint(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Short areaInterpolationMethod, ::CORBA::Short timeInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    T::ParamValue sValue = 0;
    int result = mService->getGridValueByTimeAndPoint(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,(time_t)newTime,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modParams,sValue);

    if (result == 0)
      value = sValue;

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueByTimeLevelAndPoint(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::Long level1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long level2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::Long level3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::Long level4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Short areaInterpolationMethod, ::CORBA::Short timeInterpolationMethod, ::CORBA::Short levelInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    T::ParamValue sValue = 0;
    int result = mService->getGridValueByTimeLevelAndPoint(sessionId,fileId1,messageIndex1,level1,fileId2,messageIndex2,level2,fileId3,messageIndex3,level3,fileId4,messageIndex4,level4,(time_t)newTime,newLevel,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modParams,sValue);

    if (result == 0)
      value = sValue;

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getGridValueListByCircle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, ::CORBA::Double origoX, ::CORBA::Double origoY, ::CORBA::Double radius, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByCircle(sessionId,fileId,messageIndex,coordinateType,origoX,origoY,radius,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByLevelAndCircle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, ::CORBA::Octet coordinateType, ::CORBA::Double origoX, ::CORBA::Double origoY, ::CORBA::Double radius, ::CORBA::Short levelInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByLevelAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,origoX,origoY,radius,levelInterpolationMethod,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByTimeAndCircle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, ::CORBA::Octet coordinateType, ::CORBA::Double origoX, ::CORBA::Double origoY, ::CORBA::Double radius, ::CORBA::Short timeInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByTimeAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,(time_t)newTime,coordinateType,origoX,origoY,radius,timeInterpolationMethod,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByTimeLevelAndCircle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, ::CORBA::Octet coordinateType, ::CORBA::Double origoX, ::CORBA::Double origoY, ::CORBA::Double radius, ::CORBA::Short timeInterpolationMethod, ::CORBA::Short levelInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByTimeLevelAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,(time_t)newTime,newLevel,coordinateType,origoX,origoY,radius,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByPointList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& pointList, ::CORBA::Short areaInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<T::Coordinate> sPointList;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    DataServer::Corba::Converter::convert(pointList,sPointList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByPointList(sessionId,fileId,messageIndex,coordinateType,sPointList,(short)areaInterpolationMethod,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByLevelAndPointList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& pointList, ::CORBA::Short areaInterpolationMethod, ::CORBA::Short levelInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<T::Coordinate> sPointList;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    DataServer::Corba::Converter::convert(pointList,sPointList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByLevelAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,sPointList,(short)areaInterpolationMethod,(short)levelInterpolationMethod,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByTimeAndPointList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& pointList, ::CORBA::Short areaInterpolationMethod, ::CORBA::Short timeInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<T::Coordinate> sPointList;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    DataServer::Corba::Converter::convert(pointList,sPointList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByTimeAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,(time_t)newTime,coordinateType,sPointList,(short)areaInterpolationMethod,(short)timeInterpolationMethod,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByTimeLevelAndPointList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& pointList, ::CORBA::Short areaInterpolationMethod, ::CORBA::Short timeInterpolationMethod, ::CORBA::Short levelInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<T::Coordinate> sPointList;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    DataServer::Corba::Converter::convert(pointList,sPointList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByTimeLevelAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,(time_t)newTime,newLevel,coordinateType,sPointList,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByPolygon(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& polygonPoints, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<T::Coordinate> sPolygonPoints;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    DataServer::Corba::Converter::convert(polygonPoints,sPolygonPoints);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByPolygon(sessionId,fileId,messageIndex,coordinateType,sPolygonPoints,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByLevelAndPolygon(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& polygonPoints, ::CORBA::Short levelInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<T::Coordinate> sPolygonPoints;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    DataServer::Corba::Converter::convert(polygonPoints,sPolygonPoints);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByLevelAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,sPolygonPoints,levelInterpolationMethod,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByTimeAndPolygon(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& polygonPoints, ::CORBA::Short timeInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<T::Coordinate> sPolygonPoints;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    DataServer::Corba::Converter::convert(polygonPoints,sPolygonPoints);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByTimeAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,(time_t)newTime,coordinateType,sPolygonPoints,timeInterpolationMethod,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByTimeLevelAndPolygon(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& polygonPoints, ::CORBA::Short timeInterpolationMethod, ::CORBA::Short levelInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<T::Coordinate> sPolygonPoints;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    DataServer::Corba::Converter::convert(polygonPoints,sPolygonPoints);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByTimeLevelAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,(time_t)newTime,newLevel,coordinateType,sPolygonPoints,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByPolygonPath(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaPolygonPath& polygonPath, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<std::vector<T::Coordinate>> sPolygonPath;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    DataServer::Corba::Converter::convert(polygonPath,sPolygonPath);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByPolygonPath(sessionId,fileId,messageIndex,coordinateType,sPolygonPath,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByLevelAndPolygonPath(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaPolygonPath& polygonPath, ::CORBA::Short levelInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<std::vector<T::Coordinate>> sPolygonPath;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    DataServer::Corba::Converter::convert(polygonPath,sPolygonPath);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByLevelAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,sPolygonPath,levelInterpolationMethod,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByTimeAndPolygonPath(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaPolygonPath& polygonPath, ::CORBA::Short timeInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<std::vector<T::Coordinate>> sPolygonPath;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    DataServer::Corba::Converter::convert(polygonPath,sPolygonPath);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByTimeAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,(time_t)newTime,coordinateType,sPolygonPath,timeInterpolationMethod,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByTimeLevelAndPolygonPath(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaPolygonPath& polygonPath, ::CORBA::Short timeInterpolationMethod, ::CORBA::Short levelInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<std::vector<T::Coordinate>> sPolygonPath;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    DataServer::Corba::Converter::convert(polygonPath,sPolygonPath);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByTimeLevelAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,(time_t)newTime,newLevel,coordinateType,sPolygonPath,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByRectangle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, ::CORBA::Double x1, ::CORBA::Double y1, ::CORBA::Double x2, ::CORBA::Double y2, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueListByRectangle(sessionId,fileId,messageIndex,coordinateType,x1,y1,x2,y2,modificationOperation,modParams,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVector(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    DataServer::Corba::CorbaParamValueList *corbaValues = new DataServer::Corba::CorbaParamValueList();
    values = corbaValues;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueVector(sessionId,fileId,messageIndex,modificationOperation,modParams,sValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValues,*corbaValues);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVectorByLevel(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, ::CORBA::Short levelInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    DataServer::Corba::CorbaParamValueList *corbaValues = new DataServer::Corba::CorbaParamValueList();
    values = corbaValues;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueVectorByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,levelInterpolationMethod,modificationOperation,modParams,sValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValues,*corbaValues);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVectorByTime(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, ::CORBA::Short timeInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    DataServer::Corba::CorbaParamValueList *corbaValues = new DataServer::Corba::CorbaParamValueList();
    values = corbaValues;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueVectorByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,(time_t)newTime,timeInterpolationMethod,modificationOperation,modParams,sValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValues,*corbaValues);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVectorByLevelAndGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    DataServer::Corba::CorbaParamValueList *corbaValues = new DataServer::Corba::CorbaParamValueList();
    values = corbaValues;
    T::AttributeList sAttributeList;

    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueVectorByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,sAttributeList,modificationOperation,modParams,sValues);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sValues,*corbaValues);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getGridValueVectorByTimeAndGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    DataServer::Corba::CorbaParamValueList *corbaValues = new DataServer::Corba::CorbaParamValueList();
    values = corbaValues;
    T::AttributeList sAttributeList;

    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueVectorByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,(time_t)newTime,sAttributeList,modificationOperation,modParams,sValues);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sValues,*corbaValues);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVectorByCoordinateList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& coordinates, ::CORBA::Short areaInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    std::vector<T::Coordinate> sCoordinateList;
    DataServer::Corba::CorbaParamValueList *corbaValues = new DataServer::Corba::CorbaParamValueList();
    values = corbaValues;

    DataServer::Corba::Converter::convert(coordinates,sCoordinateList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueVectorByCoordinateList(sessionId,fileId,messageIndex,coordinateType,sCoordinateList,areaInterpolationMethod,modificationOperation,modParams,sValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValues,*corbaValues);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVectorByLevelAndCoordinateList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& coordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    std::vector<T::Coordinate> sCoordinateList;
    DataServer::Corba::CorbaParamValueList *corbaValues = new DataServer::Corba::CorbaParamValueList();
    values = corbaValues;
    T::AttributeList sAttributeList;

    DataServer::Corba::Converter::convert(attributeList,sAttributeList);
    DataServer::Corba::Converter::convert(coordinates,sCoordinateList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueVectorByLevelAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,sCoordinateList,sAttributeList,modificationOperation,modParams,sValues);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sValues,*corbaValues);
     DataServer::Corba::Converter::convert(sAttributeList,attributeList);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVectorByTimeAndCoordinateList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& coordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    std::vector<T::Coordinate> sCoordinateList;
    DataServer::Corba::CorbaParamValueList *corbaValues = new DataServer::Corba::CorbaParamValueList();
    values = corbaValues;
    T::AttributeList sAttributeList;

    DataServer::Corba::Converter::convert(attributeList,sAttributeList);
    DataServer::Corba::Converter::convert(coordinates,sCoordinateList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueVectorByTimeAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,(time_t)newTime,coordinateType,sCoordinateList,sAttributeList,modificationOperation,modParams,sValues);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sValues,*corbaValues);
     DataServer::Corba::Converter::convert(sAttributeList,attributeList);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVectorByGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    DataServer::Corba::CorbaParamValueList *corbaValues = new DataServer::Corba::CorbaParamValueList();
    values = corbaValues;
    T::AttributeList sAttributeList;

    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueVectorByGeometry(sessionId,fileId,messageIndex,sAttributeList,modificationOperation,modParams,sValues);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sValues,*corbaValues);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVectorByRectangle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, ::CORBA::ULong columns, ::CORBA::ULong rows, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Double xStep, ::CORBA::Double yStep, ::CORBA::Short areaInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    DataServer::Corba::CorbaParamValueList *corbaValues = new DataServer::Corba::CorbaParamValueList();
    values = corbaValues;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueVectorByRectangle(sessionId,fileId,messageIndex,coordinateType,columns,rows,x,y,xStep,yStep,(short)areaInterpolationMethod,modificationOperation,modParams,sValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValues,*corbaValues);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVectorByPoint(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::ULong vectorType, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaDoubleList_out valueVector)
{
  FUNCTION_TRACE
  try
  {
    double_vec sValueVector;
    DataServer::Corba::CorbaDoubleList *corbaValueVector = new DataServer::Corba::CorbaDoubleList();
    valueVector = corbaValueVector;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueVectorByPoint(sessionId,fileId,messageIndex,coordinateType,x,y,vectorType,modificationOperation,modParams,sValueVector);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueVector,*corbaValueVector);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVectorByTimeAndLevel(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, ::CORBA::Short areaInterpolationMethod, ::CORBA::Short timeInterpolationMethod, ::CORBA::Short levelInterpolationMethod, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    DataServer::Corba::CorbaParamValueList *corbaValues = new DataServer::Corba::CorbaParamValueList();
    values = corbaValues;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueVectorByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,(time_t)newTime,newLevel,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modParams,sValues);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sValues,*corbaValues);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVectorByTimeLevelAndGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    DataServer::Corba::CorbaParamValueList *corbaValues = new DataServer::Corba::CorbaParamValueList();
    values = corbaValues;
    T::AttributeList sAttributeList;

    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueVectorByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,(time_t)newTime,newLevel,sAttributeList,modificationOperation,modParams,sValues);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sValues,*corbaValues);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVectorByTimeLevelAndCoordinateList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& coordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaParamValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    std::vector<T::Coordinate> sCoordinateList;
    DataServer::Corba::CorbaParamValueList *corbaValues = new DataServer::Corba::CorbaParamValueList();
    values = corbaValues;
    T::AttributeList sAttributeList;

    DataServer::Corba::Converter::convert(attributeList,sAttributeList);
    DataServer::Corba::Converter::convert(coordinates,sCoordinateList);


    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridValueVectorByTimeLevelAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,(time_t)newTime,newLevel,coordinateType,sCoordinateList,sAttributeList,modificationOperation,modParams,sValues);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sValues,*corbaValues);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsobands(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;


    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(contourLowValues,sContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,sContourHighValues);
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsobands(sessionId,fileId,messageIndex,sContourLowValues,sContourHighValues,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsobandsByGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;

    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(contourLowValues,sContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,sContourHighValues);
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsobandsByGeometry(sessionId,fileId,messageIndex,sContourLowValues,sContourHighValues,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsobandsByGrid(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, ::CORBA::ULong gridWidth, ::CORBA::ULong gridHeight, const SmartMet::DataServer::Corba::CorbaCoordinateList& gridLatLonCoordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    std::vector<T::Coordinate> sCoordinateList;
    T::AttributeList sAttributeList;

    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(gridLatLonCoordinates,sCoordinateList);
    DataServer::Corba::Converter::convert(contourLowValues,sContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,sContourHighValues);
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsobandsByGrid(sessionId,fileId,messageIndex,sContourLowValues,sContourHighValues,gridWidth,gridHeight,sCoordinateList,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsobandsByLevel(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;


    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(contourLowValues,sContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,sContourHighValues);
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsobandsByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,sContourLowValues,sContourHighValues,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsobandsByTime(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;


    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(contourLowValues,sContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,sContourHighValues);
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsobandsByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,(time_t)newTime,sContourLowValues,sContourHighValues,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsobandsByLevelAndGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;

    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(contourLowValues,sContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,sContourHighValues);
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsobandsByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,sContourLowValues,sContourHighValues,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsobandsByTimeAndGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;

    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(contourLowValues,sContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,sContourHighValues);
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsobandsByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,(time_t)newTime,sContourLowValues,sContourHighValues,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsobandsByLevelAndGrid(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, ::CORBA::ULong gridWidth, ::CORBA::ULong gridHeight, const SmartMet::DataServer::Corba::CorbaCoordinateList& gridLatLonCoordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    std::vector<T::Coordinate> sCoordinateList;
    T::AttributeList sAttributeList;

    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(gridLatLonCoordinates,sCoordinateList);
    DataServer::Corba::Converter::convert(contourLowValues,sContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,sContourHighValues);
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsobandsByLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,sContourLowValues,sContourHighValues,gridWidth,gridHeight,sCoordinateList,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getGridIsobandsByTimeAndGrid(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, ::CORBA::ULong gridWidth, ::CORBA::ULong gridHeight, const SmartMet::DataServer::Corba::CorbaCoordinateList& gridLatLonCoordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    std::vector<T::Coordinate> sCoordinateList;
    T::AttributeList sAttributeList;

    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(gridLatLonCoordinates,sCoordinateList);
    DataServer::Corba::Converter::convert(contourLowValues,sContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,sContourHighValues);
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsobandsByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,(time_t)newTime,sContourLowValues,sContourHighValues,gridWidth,gridHeight,sCoordinateList,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsobandsByTimeAndLevel(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;


    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(contourLowValues,sContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,sContourHighValues);
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsobandsByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,(time_t)newTime,newLevel,sContourLowValues,sContourHighValues,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsobandsByTimeLevelAndGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;


    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(contourLowValues,sContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,sContourHighValues);
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsobandsByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,(time_t)newTime,newLevel,sContourLowValues,sContourHighValues,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsobandsByTimeLevelAndGrid(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, ::CORBA::ULong gridWidth, ::CORBA::ULong gridHeight, const SmartMet::DataServer::Corba::CorbaCoordinateList& gridLatLonCoordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;
    std::vector<T::Coordinate> sCoordinateList;


    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(gridLatLonCoordinates,sCoordinateList);
    DataServer::Corba::Converter::convert(contourLowValues,sContourLowValues);
    DataServer::Corba::Converter::convert(contourHighValues,sContourHighValues);
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsobandsByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,(time_t)newTime,newLevel,sContourLowValues,sContourHighValues,gridWidth,gridHeight,sCoordinateList,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsolinesByTimeAndLevel(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;


    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(contourValues,sContourValues);
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsolinesByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,(time_t)newTime,newLevel,sContourValues,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsolinesByTimeLevelAndGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;


    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(contourValues,sContourValues);
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsolinesByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,(time_t)newTime,newLevel,sContourValues,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsolines(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(contourValues,sContourValues);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsolines(sessionId,fileId,messageIndex,sContourValues,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsolinesByGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(contourValues,sContourValues);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsolinesByGeometry(sessionId,fileId,messageIndex,sContourValues,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsolinesByGrid(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, ::CORBA::ULong gridWidth, ::CORBA::ULong gridHeight, const SmartMet::DataServer::Corba::CorbaCoordinateList& gridLatLonCoordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    std::vector<T::Coordinate> sCoordinateList;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(gridLatLonCoordinates,sCoordinateList);
    DataServer::Corba::Converter::convert(contourValues,sContourValues);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsolinesByGrid(sessionId,fileId,messageIndex,sContourValues,gridWidth,gridHeight,sCoordinateList,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsolinesByLevel(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(contourValues,sContourValues);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsolinesByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,sContourValues,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsolinesByTime(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(contourValues,sContourValues);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsolinesByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,(time_t)newTime,sContourValues,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsolinesByLevelAndGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(contourValues,sContourValues);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsolinesByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,sContourValues,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsolinesByTimeAndGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(contourValues,sContourValues);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsolinesByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,(time_t)newTime,sContourValues,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsolinesByLevelAndGrid(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, ::CORBA::ULong gridWidth, ::CORBA::ULong gridHeight, const SmartMet::DataServer::Corba::CorbaCoordinateList& gridLatLonCoordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    std::vector<T::Coordinate> sCoordinateList;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(gridLatLonCoordinates,sCoordinateList);
    DataServer::Corba::Converter::convert(contourValues,sContourValues);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsolinesByLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,sContourValues,gridWidth,gridHeight,sCoordinateList,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsolinesByTimeAndGrid(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, ::CORBA::ULong gridWidth, ::CORBA::ULong gridHeight, const SmartMet::DataServer::Corba::CorbaCoordinateList& gridLatLonCoordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    std::vector<T::Coordinate> sCoordinateList;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(gridLatLonCoordinates,sCoordinateList);
    DataServer::Corba::Converter::convert(contourValues,sContourValues);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsolinesByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,(time_t)newTime,sContourValues,gridWidth,gridHeight,sCoordinateList,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridIsolinesByTimeLevelAndGrid(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, ::CORBA::ULong gridWidth, ::CORBA::ULong gridHeight, const SmartMet::DataServer::Corba::CorbaCoordinateList& gridLatLonCoordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;
    std::vector<T::Coordinate> sCoordinateList;

    DataServer::Corba::CorbaByteDataSequence *corbaContours = new DataServer::Corba::CorbaByteDataSequence();
    contours = corbaContours;

    DataServer::Corba::Converter::convert(gridLatLonCoordinates,sCoordinateList);
    DataServer::Corba::Converter::convert(contourValues,sContourValues);
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridIsolinesByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,(time_t)newTime,newLevel,sContourValues,gridWidth,gridHeight,sCoordinateList,sAttributeList,modificationOperation,modParams,sContours);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sContours,*corbaContours);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getGridStreamlines(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    DataServer::Corba::CorbaByteDataSequence *corbaStreamLines = new DataServer::Corba::CorbaByteDataSequence();
    streamlines = corbaStreamLines;

    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridStreamlines(sessionId,fileId,messageIndex,sAttributeList,modificationOperation,modParams,sStreamLines);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sStreamLines,*corbaStreamLines);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getGridStreamlinesByGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    DataServer::Corba::CorbaByteDataSequence *corbaStreamLines = new DataServer::Corba::CorbaByteDataSequence();
    streamlines = corbaStreamLines;

    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridStreamlinesByGeometry(sessionId,fileId,messageIndex,sAttributeList,modificationOperation,modParams,sStreamLines);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sStreamLines,*corbaStreamLines);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getGridStreamlinesByGrid(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong gridWidth, ::CORBA::ULong gridHeight, const SmartMet::DataServer::Corba::CorbaCoordinateList& gridLatLonCoordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    DataServer::Corba::CorbaByteDataSequence *corbaStreamLines = new DataServer::Corba::CorbaByteDataSequence();
    streamlines = corbaStreamLines;

    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    std::vector<T::Coordinate> sCoordinateList;
    DataServer::Corba::Converter::convert(gridLatLonCoordinates,sCoordinateList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridStreamlinesByGrid(sessionId,fileId,messageIndex,gridWidth,gridHeight,sCoordinateList,sAttributeList,modificationOperation,modParams,sStreamLines);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sStreamLines,*corbaStreamLines);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getGridStreamlinesByLevel(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    DataServer::Corba::CorbaByteDataSequence *corbaStreamLines = new DataServer::Corba::CorbaByteDataSequence();
    streamlines = corbaStreamLines;

    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridStreamlinesByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,sAttributeList,modificationOperation,modParams,sStreamLines);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sStreamLines,*corbaStreamLines);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getGridStreamlinesByLevelAndGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    DataServer::Corba::CorbaByteDataSequence *corbaStreamLines = new DataServer::Corba::CorbaByteDataSequence();
    streamlines = corbaStreamLines;

    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridStreamlinesByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,sAttributeList,modificationOperation,modParams,sStreamLines);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sStreamLines,*corbaStreamLines);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getGridStreamlinesByLevelAndGrid(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, ::CORBA::ULong gridWidth, ::CORBA::ULong gridHeight, const SmartMet::DataServer::Corba::CorbaCoordinateList& gridLatLonCoordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    DataServer::Corba::CorbaByteDataSequence *corbaStreamLines = new DataServer::Corba::CorbaByteDataSequence();
    streamlines = corbaStreamLines;

    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    std::vector<T::Coordinate> sCoordinateList;
    DataServer::Corba::Converter::convert(gridLatLonCoordinates,sCoordinateList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridStreamlinesByLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,gridWidth,gridHeight,sCoordinateList,sAttributeList,modificationOperation,modParams,sStreamLines);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sStreamLines,*corbaStreamLines);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getGridStreamlinesByTime(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    DataServer::Corba::CorbaByteDataSequence *corbaStreamLines = new DataServer::Corba::CorbaByteDataSequence();
    streamlines = corbaStreamLines;

    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridStreamlinesByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,sAttributeList,modificationOperation,modParams,sStreamLines);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sStreamLines,*corbaStreamLines);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getGridStreamlinesByTimeAndGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    DataServer::Corba::CorbaByteDataSequence *corbaStreamLines = new DataServer::Corba::CorbaByteDataSequence();
    streamlines = corbaStreamLines;

    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridStreamlinesByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,sAttributeList,modificationOperation,modParams,sStreamLines);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sStreamLines,*corbaStreamLines);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getGridStreamlinesByTimeAndGrid(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULongLong newTime, ::CORBA::ULong gridWidth, ::CORBA::ULong gridHeight, const SmartMet::DataServer::Corba::CorbaCoordinateList& gridLatLonCoordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    DataServer::Corba::CorbaByteDataSequence *corbaStreamLines = new DataServer::Corba::CorbaByteDataSequence();
    streamlines = corbaStreamLines;

    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    std::vector<T::Coordinate> sCoordinateList;
    DataServer::Corba::Converter::convert(gridLatLonCoordinates,sCoordinateList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridStreamlinesByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,gridWidth,gridHeight,sCoordinateList,sAttributeList,modificationOperation,modParams,sStreamLines);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sStreamLines,*corbaStreamLines);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getGridStreamlinesByTimeAndLevel(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    DataServer::Corba::CorbaByteDataSequence *corbaStreamLines = new DataServer::Corba::CorbaByteDataSequence();
    streamlines = corbaStreamLines;

    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridStreamlinesByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,sAttributeList,modificationOperation,modParams,sStreamLines);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sStreamLines,*corbaStreamLines);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getGridStreamlinesByTimeLevelAndGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    DataServer::Corba::CorbaByteDataSequence *corbaStreamLines = new DataServer::Corba::CorbaByteDataSequence();
    streamlines = corbaStreamLines;

    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridStreamlinesByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,sAttributeList,modificationOperation,modParams,sStreamLines);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sStreamLines,*corbaStreamLines);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getGridStreamlinesByTimeLevelAndGrid(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::ULongLong newTime, ::CORBA::Long newLevel, ::CORBA::ULong gridWidth, ::CORBA::ULong gridHeight, const SmartMet::DataServer::Corba::CorbaCoordinateList& gridLatLonCoordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    DataServer::Corba::CorbaByteDataSequence *corbaStreamLines = new DataServer::Corba::CorbaByteDataSequence();
    streamlines = corbaStreamLines;

    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    std::vector<T::Coordinate> sCoordinateList;
    DataServer::Corba::Converter::convert(gridLatLonCoordinates,sCoordinateList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getGridStreamlinesByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,gridWidth,gridHeight,sCoordinateList,sAttributeList,modificationOperation,modParams,sStreamLines);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sAttributeList,attributeList);
      DataServer::Corba::Converter::convert(sStreamLines,*corbaStreamLines);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getMultipleGridValues(::CORBA::LongLong sessionId, ::CORBA::ULong modificationOperation, const SmartMet::DataServer::Corba::CorbaDoubleList& modificationParameters, SmartMet::DataServer::Corba::CorbaValueRecordList& valueRecordList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::ValueRecordList sValueRecordList;
    DataServer::Corba::Converter::convert(valueRecordList,sValueRecordList);

    double_vec modParams;
    DataServer::Corba::Converter::convert(modificationParameters,modParams);

    int result = mService->getMultipleGridValues(sessionId,modificationOperation,modParams,sValueRecordList);
    if (result == 0)
      DataServer::Corba::Converter::convert(sValueRecordList,valueRecordList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





}
}
}

