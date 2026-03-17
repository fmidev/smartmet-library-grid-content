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




SmartMet::C::Result ServerInterface::getGridCoordinates(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::CoordinateType coordinateType, SmartMet::C::GridCoordinates_out coordinates)
{
  FUNCTION_TRACE
  try
  {
    T::GridCoordinates sCoordinates;
    C::GridCoordinates *corbaGridCoordinates = new C::GridCoordinates();
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





SmartMet::C::Result ServerInterface::getGridLatlonCoordinatesByGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::GridCoordinates_out coordinates)
{
  FUNCTION_TRACE
  try
  {
    T::GridCoordinates sCoordinates;
    T::AttributeList sAttributeList;
    C::GridCoordinates *corbaGridCoordinates = new C::GridCoordinates();
    coordinates = corbaGridCoordinates;

    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGridLatlonCoordinatesByGeometry(sessionId,sAttributeList,sCoordinates);

    if (result == 0)
    {
      DataServer::Corba::Converter::convert(sCoordinates,*corbaGridCoordinates);
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





SmartMet::C::Result ServerInterface::getGridData(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::GridData_out data)
{
  FUNCTION_TRACE
  try
  {
    T::GridData sGridData;
    C::GridData *corbaGridData = new C::GridData();
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





SmartMet::C::Result ServerInterface::getGridFileCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count)
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





SmartMet::C::Result ServerInterface::getGridMessageBytes(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::ByteData_out messageBytes, SmartMet::C::UInt32List_out messageSections)
{
  FUNCTION_TRACE
  try
  {
    std::vector<uchar> sMessageBytes;
    std::vector<uint> sMessageSections;

    C::ByteData *corbaMessageBytes = new C::ByteData();
    messageBytes = corbaMessageBytes;

    C::UInt32List *corbaMessageSections = new C::UInt32List();
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





SmartMet::C::Result ServerInterface::getGridAttributeList(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::AttributeRecList_out attributeList)
{
  FUNCTION_TRACE
  try
  {
    T::AttributeList sAttributeList;
    C::AttributeRecList *corbaAttributeList = new C::AttributeRecList();
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





SmartMet::C::Result ServerInterface::getGridProperties(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::PropertySettingList_out propertyList)
{
  FUNCTION_TRACE
  try
  {
    T::PropertySettingVec sPropertySettingList;
    C::PropertySettingList *corbaPropertySettingList = new C::PropertySettingList();
    propertyList = corbaPropertySettingList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGridProperties(sessionId,fileId,messageIndex,sPropertySettingList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sPropertySettingList,*corbaPropertySettingList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}



SmartMet::C::Result ServerInterface::getPropertyValuesByCoordinates(SmartMet::C::SessionId sessionId, const char* propertyName, const SmartMet::C::CoordinateList& latlonCoordinates, SmartMet::C::ParameterValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    std::vector<T::Coordinate> sCoordinateList;
    C::ParameterValueList *corbaValues = new C::ParameterValueList();
    values = corbaValues;

    DataServer::Corba::Converter::convert(latlonCoordinates,sCoordinateList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getPropertyValuesByCoordinates(sessionId,propertyName,sCoordinateList,sValues);

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





SmartMet::C::Result ServerInterface::getGridValueByPoint(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::CoordinateType coordinateType, SmartMet::C::Float64 x, SmartMet::C::Float64 y, SmartMet::C::Int16 areaInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValue& value)
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





SmartMet::C::Result ServerInterface::getGridValueByLevelAndPoint(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::Int32 level1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Int32 level2, SmartMet::C::Float64 newLevel, SmartMet::C::CoordinateType coordinateType, SmartMet::C::Float64 x, SmartMet::C::Float64 y, SmartMet::C::Int16 areaInterpolationMethod, SmartMet::C::Int16 levelInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValue& value)
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





SmartMet::C::Result ServerInterface::getGridValueByTimeAndPoint(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, SmartMet::C::CoordinateType coordinateType, SmartMet::C::Float64 x, SmartMet::C::Float64 y, SmartMet::C::Int16 areaInterpolationMethod, SmartMet::C::Int16 timeInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValue& value)
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





SmartMet::C::Result ServerInterface::getGridValueByTimeLevelAndPoint(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::Int32 level1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Int32 level2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::Int32 level3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Int32 level4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, SmartMet::C::CoordinateType coordinateType, SmartMet::C::Float64 x, SmartMet::C::Float64 y, SmartMet::C::Int16 areaInterpolationMethod, SmartMet::C::Int16 timeInterpolationMethod, SmartMet::C::Int16 levelInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValue& value)
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




SmartMet::C::Result ServerInterface::getGridValueListByCircle(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::CoordinateType coordinateType, SmartMet::C::Float64 origoX, SmartMet::C::Float64 origoY, SmartMet::C::Float64 radius, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueListByLevelAndCircle(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Float64 newLevel, SmartMet::C::CoordinateType coordinateType, SmartMet::C::Float64 origoX, SmartMet::C::Float64 origoY, SmartMet::C::Float64 radius, SmartMet::C::Int16 levelInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueListByTimeAndCircle(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, SmartMet::C::CoordinateType coordinateType, SmartMet::C::Float64 origoX, SmartMet::C::Float64 origoY, SmartMet::C::Float64 radius, SmartMet::C::Int16 timeInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueListByTimeLevelAndCircle(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, SmartMet::C::CoordinateType coordinateType, SmartMet::C::Float64 origoX, SmartMet::C::Float64 origoY, SmartMet::C::Float64 radius, SmartMet::C::Int16 timeInterpolationMethod, SmartMet::C::Int16 levelInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueListByPointList(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::CoordinateType coordinateType, const SmartMet::C::CoordinateList& pointList, SmartMet::C::Int16 areaInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<T::Coordinate> sPointList;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueListByLevelAndPointList(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Float64 newLevel, SmartMet::C::CoordinateType coordinateType, const SmartMet::C::CoordinateList& pointList, SmartMet::C::Int16 areaInterpolationMethod, SmartMet::C::Int16 levelInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<T::Coordinate> sPointList;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueListByTimeAndPointList(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, SmartMet::C::CoordinateType coordinateType, const SmartMet::C::CoordinateList& pointList, SmartMet::C::Int16 areaInterpolationMethod, SmartMet::C::Int16 timeInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<T::Coordinate> sPointList;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueListByTimeLevelAndPointList(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, SmartMet::C::CoordinateType coordinateType, const SmartMet::C::CoordinateList& pointList, SmartMet::C::Int16 areaInterpolationMethod, SmartMet::C::Int16 timeInterpolationMethod, SmartMet::C::Int16 levelInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<T::Coordinate> sPointList;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueListByPolygon(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::CoordinateType coordinateType, const SmartMet::C::CoordinateList& polygonPoints, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<T::Coordinate> sPolygonPoints;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueListByLevelAndPolygon(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Float64 newLevel, SmartMet::C::CoordinateType coordinateType, const SmartMet::C::CoordinateList& polygonPoints, SmartMet::C::Int16 levelInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<T::Coordinate> sPolygonPoints;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueListByTimeAndPolygon(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, SmartMet::C::CoordinateType coordinateType, const SmartMet::C::CoordinateList& polygonPoints, SmartMet::C::Int16 timeInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<T::Coordinate> sPolygonPoints;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueListByTimeLevelAndPolygon(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, SmartMet::C::CoordinateType coordinateType, const SmartMet::C::CoordinateList& polygonPoints, SmartMet::C::Int16 timeInterpolationMethod, SmartMet::C::Int16 levelInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<T::Coordinate> sPolygonPoints;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueListByPolygonPath(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::CoordinateType coordinateType, const SmartMet::C::PolygonPath& polygonPath, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<std::vector<T::Coordinate>> sPolygonPath;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueListByLevelAndPolygonPath(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Float64 newLevel, SmartMet::C::CoordinateType coordinateType, const SmartMet::C::PolygonPath& polygonPath, SmartMet::C::Int16 levelInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<std::vector<T::Coordinate>> sPolygonPath;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueListByTimeAndPolygonPath(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, SmartMet::C::CoordinateType coordinateType, const SmartMet::C::PolygonPath& polygonPath, SmartMet::C::Int16 timeInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<std::vector<T::Coordinate>> sPolygonPath;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueListByTimeLevelAndPolygonPath(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, SmartMet::C::CoordinateType coordinateType, const SmartMet::C::PolygonPath& polygonPath, SmartMet::C::Int16 timeInterpolationMethod, SmartMet::C::Int16 levelInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    std::vector<std::vector<T::Coordinate>> sPolygonPath;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueListByRectangle(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::CoordinateType coordinateType, SmartMet::C::Float64 x1, SmartMet::C::Float64 y1, SmartMet::C::Float64 x2, SmartMet::C::Float64 y2, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::GridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    C::GridValueList *corbaValueList = new C::GridValueList();
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





SmartMet::C::Result ServerInterface::getGridValueVector(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    C::ParameterValueList *corbaValues = new C::ParameterValueList();
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





SmartMet::C::Result ServerInterface::getGridValueVectorByLevel(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Float64 newLevel, SmartMet::C::Int16 levelInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    C::ParameterValueList *corbaValues = new C::ParameterValueList();
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





SmartMet::C::Result ServerInterface::getGridValueVectorByTime(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, SmartMet::C::Int16 timeInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    C::ParameterValueList *corbaValues = new C::ParameterValueList();
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





SmartMet::C::Result ServerInterface::getGridValueVectorByLevelAndGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Float64 newLevel, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    C::ParameterValueList *corbaValues = new C::ParameterValueList();
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




SmartMet::C::Result ServerInterface::getGridValueVectorByTimeAndGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    C::ParameterValueList *corbaValues = new C::ParameterValueList();
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





SmartMet::C::Result ServerInterface::getGridValueVectorByCoordinateList(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::CoordinateType coordinateType, const SmartMet::C::CoordinateList& coordinates, SmartMet::C::Int16 areaInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    std::vector<T::Coordinate> sCoordinateList;
    C::ParameterValueList *corbaValues = new C::ParameterValueList();
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





SmartMet::C::Result ServerInterface::getGridValueVectorByLevelAndCoordinateList(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Float64 newLevel, SmartMet::C::CoordinateType coordinateType, const SmartMet::C::CoordinateList& coordinates, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    std::vector<T::Coordinate> sCoordinateList;
    C::ParameterValueList *corbaValues = new C::ParameterValueList();
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





SmartMet::C::Result ServerInterface::getGridValueVectorByTimeAndCoordinateList(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, SmartMet::C::CoordinateType coordinateType, const SmartMet::C::CoordinateList& coordinates, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    std::vector<T::Coordinate> sCoordinateList;
    C::ParameterValueList *corbaValues = new C::ParameterValueList();
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





SmartMet::C::Result ServerInterface::getGridValueVectorByGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    C::ParameterValueList *corbaValues = new C::ParameterValueList();
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





SmartMet::C::Result ServerInterface::getGridValueVectorByRectangle(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::CoordinateType coordinateType, SmartMet::C::UInt32 columns, SmartMet::C::UInt32 rows, SmartMet::C::Float64 x, SmartMet::C::Float64 y, SmartMet::C::Float64 xStep, SmartMet::C::Float64 yStep, SmartMet::C::Int16 areaInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    C::ParameterValueList *corbaValues = new C::ParameterValueList();
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





SmartMet::C::Result ServerInterface::getGridValueVectorByPoint(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::CoordinateType coordinateType, SmartMet::C::Float64 x, SmartMet::C::Float64 y, SmartMet::C::UInt32 vectorType, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::Float64List_out valueVector)
{
  FUNCTION_TRACE
  try
  {
    double_vec sValueVector;
    C::Float64List *corbaValueVector = new C::Float64List();
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





SmartMet::C::Result ServerInterface::getGridValueVectorByTimeAndLevel(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, SmartMet::C::Int16 areaInterpolationMethod, SmartMet::C::Int16 timeInterpolationMethod, SmartMet::C::Int16 levelInterpolationMethod, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    C::ParameterValueList *corbaValues = new C::ParameterValueList();
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





SmartMet::C::Result ServerInterface::getGridValueVectorByTimeLevelAndGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    C::ParameterValueList *corbaValues = new C::ParameterValueList();
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





SmartMet::C::Result ServerInterface::getGridValueVectorByTimeLevelAndCoordinateList(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, SmartMet::C::CoordinateType coordinateType, const SmartMet::C::CoordinateList& coordinates, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ParameterValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    std::vector<T::Coordinate> sCoordinateList;
    C::ParameterValueList *corbaValues = new C::ParameterValueList();
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




SmartMet::C::Result ServerInterface::getGridIsobands(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, const SmartMet::C::ParameterValueList& contourLowValues, const SmartMet::C::ParameterValueList& contourHighValues, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;


    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsobandsByGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, const SmartMet::C::ParameterValueList& contourLowValues, const SmartMet::C::ParameterValueList& contourHighValues, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;

    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsobandsByGrid(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, const SmartMet::C::ParameterValueList& contourLowValues, const SmartMet::C::ParameterValueList& contourHighValues, SmartMet::C::UInt32 gridWidth, SmartMet::C::UInt32 gridHeight, const SmartMet::C::CoordinateList& gridLatLonCoordinates, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    std::vector<T::Coordinate> sCoordinateList;
    T::AttributeList sAttributeList;

    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsobandsByLevel(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Float64 newLevel, const SmartMet::C::ParameterValueList& contourLowValues, const SmartMet::C::ParameterValueList& contourHighValues, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;


    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsobandsByTime(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, const SmartMet::C::ParameterValueList& contourLowValues, const SmartMet::C::ParameterValueList& contourHighValues, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;


    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsobandsByLevelAndGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Float64 newLevel, const SmartMet::C::ParameterValueList& contourLowValues, const SmartMet::C::ParameterValueList& contourHighValues, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;

    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsobandsByTimeAndGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, const SmartMet::C::ParameterValueList& contourLowValues, const SmartMet::C::ParameterValueList& contourHighValues, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;

    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsobandsByLevelAndGrid(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Float64 newLevel, const SmartMet::C::ParameterValueList& contourLowValues, const SmartMet::C::ParameterValueList& contourHighValues, SmartMet::C::UInt32 gridWidth, SmartMet::C::UInt32 gridHeight, const SmartMet::C::CoordinateList& gridLatLonCoordinates, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    std::vector<T::Coordinate> sCoordinateList;
    T::AttributeList sAttributeList;

    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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




SmartMet::C::Result ServerInterface::getGridIsobandsByTimeAndGrid(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, const SmartMet::C::ParameterValueList& contourLowValues, const SmartMet::C::ParameterValueList& contourHighValues, SmartMet::C::UInt32 gridWidth, SmartMet::C::UInt32 gridHeight, const SmartMet::C::CoordinateList& gridLatLonCoordinates, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    std::vector<T::Coordinate> sCoordinateList;
    T::AttributeList sAttributeList;

    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsobandsByTimeAndLevel(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, const SmartMet::C::ParameterValueList& contourLowValues, const SmartMet::C::ParameterValueList& contourHighValues, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;


    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsobandsByTimeLevelAndGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, const SmartMet::C::ParameterValueList& contourLowValues, const SmartMet::C::ParameterValueList& contourHighValues, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;


    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsobandsByTimeLevelAndGrid(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, const SmartMet::C::ParameterValueList& contourLowValues, const SmartMet::C::ParameterValueList& contourHighValues, SmartMet::C::UInt32 gridWidth, SmartMet::C::UInt32 gridHeight, const SmartMet::C::CoordinateList& gridLatLonCoordinates, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourLowValues;
    T::ParamValue_vec sContourHighValues;
    T::AttributeList sAttributeList;
    std::vector<T::Coordinate> sCoordinateList;


    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsolinesByTimeAndLevel(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, const SmartMet::C::ParameterValueList& contourValues, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;


    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsolinesByTimeLevelAndGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, const SmartMet::C::ParameterValueList& contourValues, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;


    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsolines(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, const SmartMet::C::ParameterValueList& contourValues, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsolinesByGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, const SmartMet::C::ParameterValueList& contourValues, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsolinesByGrid(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, const SmartMet::C::ParameterValueList& contourValues, SmartMet::C::UInt32 gridWidth, SmartMet::C::UInt32 gridHeight, const SmartMet::C::CoordinateList& gridLatLonCoordinates, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    std::vector<T::Coordinate> sCoordinateList;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsolinesByLevel(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Float64 newLevel, const SmartMet::C::ParameterValueList& contourValues, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsolinesByTime(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, const SmartMet::C::ParameterValueList& contourValues, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsolinesByLevelAndGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Float64 newLevel, const SmartMet::C::ParameterValueList& contourValues, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsolinesByTimeAndGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, const SmartMet::C::ParameterValueList& contourValues, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsolinesByLevelAndGrid(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Float64 newLevel, const SmartMet::C::ParameterValueList& contourValues, SmartMet::C::UInt32 gridWidth, SmartMet::C::UInt32 gridHeight, const SmartMet::C::CoordinateList& gridLatLonCoordinates, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    std::vector<T::Coordinate> sCoordinateList;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsolinesByTimeAndGrid(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, const SmartMet::C::ParameterValueList& contourValues, SmartMet::C::UInt32 gridWidth, SmartMet::C::UInt32 gridHeight, const SmartMet::C::CoordinateList& gridLatLonCoordinates, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    std::vector<T::Coordinate> sCoordinateList;
    T::AttributeList sAttributeList;
    DataServer::Corba::Converter::convert(attributeList,sAttributeList);

    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridIsolinesByTimeLevelAndGrid(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, const SmartMet::C::ParameterValueList& contourValues, SmartMet::C::UInt32 gridWidth, SmartMet::C::UInt32 gridHeight, const SmartMet::C::CoordinateList& gridLatLonCoordinates, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out contours)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sContours;
    T::ParamValue_vec sContourValues;
    T::AttributeList sAttributeList;
    std::vector<T::Coordinate> sCoordinateList;

    C::ByteDataSequence *corbaContours = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getGridStreamlines(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    C::ByteDataSequence *corbaStreamLines = new C::ByteDataSequence();
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




SmartMet::C::Result ServerInterface::getGridStreamlinesByGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    C::ByteDataSequence *corbaStreamLines = new C::ByteDataSequence();
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




SmartMet::C::Result ServerInterface::getGridStreamlinesByGrid(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::UInt32 gridWidth, SmartMet::C::UInt32 gridHeight, const SmartMet::C::CoordinateList& gridLatLonCoordinates, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    C::ByteDataSequence *corbaStreamLines = new C::ByteDataSequence();
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




SmartMet::C::Result ServerInterface::getGridStreamlinesByLevel(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Float64 newLevel, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    C::ByteDataSequence *corbaStreamLines = new C::ByteDataSequence();
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




SmartMet::C::Result ServerInterface::getGridStreamlinesByLevelAndGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Float64 newLevel, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    C::ByteDataSequence *corbaStreamLines = new C::ByteDataSequence();
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




SmartMet::C::Result ServerInterface::getGridStreamlinesByLevelAndGrid(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Float64 newLevel, SmartMet::C::UInt32 gridWidth, SmartMet::C::UInt32 gridHeight, const SmartMet::C::CoordinateList& gridLatLonCoordinates, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    C::ByteDataSequence *corbaStreamLines = new C::ByteDataSequence();
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




SmartMet::C::Result ServerInterface::getGridStreamlinesByTime(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    C::ByteDataSequence *corbaStreamLines = new C::ByteDataSequence();
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




SmartMet::C::Result ServerInterface::getGridStreamlinesByTimeAndGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    C::ByteDataSequence *corbaStreamLines = new C::ByteDataSequence();
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




SmartMet::C::Result ServerInterface::getGridStreamlinesByTimeAndGrid(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::Timestamp newTime, SmartMet::C::UInt32 gridWidth, SmartMet::C::UInt32 gridHeight, const SmartMet::C::CoordinateList& gridLatLonCoordinates, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    C::ByteDataSequence *corbaStreamLines = new C::ByteDataSequence();
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




SmartMet::C::Result ServerInterface::getGridStreamlinesByTimeAndLevel(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    C::ByteDataSequence *corbaStreamLines = new C::ByteDataSequence();
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




SmartMet::C::Result ServerInterface::getGridStreamlinesByTimeLevelAndGeometry(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    C::ByteDataSequence *corbaStreamLines = new C::ByteDataSequence();
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




SmartMet::C::Result ServerInterface::getGridStreamlinesByTimeLevelAndGrid(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId1, SmartMet::C::MessageIndex messageIndex1, SmartMet::C::FileId fileId2, SmartMet::C::MessageIndex messageIndex2, SmartMet::C::FileId fileId3, SmartMet::C::MessageIndex messageIndex3, SmartMet::C::FileId fileId4, SmartMet::C::MessageIndex messageIndex4, SmartMet::C::Timestamp newTime, SmartMet::C::Float64 newLevel, SmartMet::C::UInt32 gridWidth, SmartMet::C::UInt32 gridHeight, const SmartMet::C::CoordinateList& gridLatLonCoordinates, SmartMet::C::AttributeRecList& attributeList, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ByteDataSequence_out streamlines)
{
  FUNCTION_TRACE
  try
  {
    T::ByteData_vec sStreamLines;

    C::ByteDataSequence *corbaStreamLines = new C::ByteDataSequence();
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





SmartMet::C::Result ServerInterface::getMultipleGridValues(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32 modificationOperation, const SmartMet::C::Float64List& modificationParameters, SmartMet::C::ValueRecordList& valueRecordList)
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

