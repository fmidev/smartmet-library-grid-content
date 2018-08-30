#include "ServerInterface.h"
#include "../convert/Converter.h"
#include <grid-files/common/Exception.h>
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





::CORBA::Long ServerInterface::getGridCoordinates(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, SmartMet::DataServer::Corba::CorbaGridCoordinates_out coordinates)
{
  FUNCTION_TRACE
  try
  {
    T::GridCoordinates sCoordinates;
    DataServer::Corba::CorbaGridCoordinates *corbaGridCoordinates = new DataServer::Corba::CorbaGridCoordinates();
    coordinates = corbaGridCoordinates;

    if (mService == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    int result = mService->getGridCoordinates(sessionId,fileId,messageIndex,flags,coordinateType,sCoordinates);

    if (result == 0)
      DataServer::Corba::Converter::convert(sCoordinates,*corbaGridCoordinates);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridData(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, SmartMet::DataServer::Corba::CorbaGridData_out data)
{
  FUNCTION_TRACE
  try
  {
    T::GridData sGridData;
    DataServer::Corba::CorbaGridData *corbaGridData = new DataServer::Corba::CorbaGridData();
    data = corbaGridData;

    if (mService == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    int result = mService->getGridData(sessionId,fileId,messageIndex,flags,sGridData);

    if (result == 0)
      DataServer::Corba::Converter::convert(sGridData,*corbaGridData);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
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
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    uint sCount = 0;
    int result = mService->getGridFileCount(sessionId,sCount);

    if (result == 0)
      count = sCount;

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getMultipleGridValues(::CORBA::LongLong sessionId, SmartMet::DataServer::Corba::CorbaValueRecordList& valueRecordList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    T::ValueRecordList sValueRecordList;
    DataServer::Corba::Converter::convert(valueRecordList,sValueRecordList);

    int result = mService->getMultipleGridValues(sessionId,sValueRecordList);
    if (result == 0)
      DataServer::Corba::Converter::convert(sValueRecordList,valueRecordList);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridAttributeList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, SmartMet::DataServer::Corba::CorbaAttributeList_out attributeList)
{
  FUNCTION_TRACE
  try
  {
    T::AttributeList sAttributeList;
    DataServer::Corba::CorbaAttributeList *corbaAttributeList = new DataServer::Corba::CorbaAttributeList();
    attributeList = corbaAttributeList;

    if (mService == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    int result = mService->getGridAttributeList(sessionId,fileId,messageIndex,flags,sAttributeList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sAttributeList,*corbaAttributeList);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueByPoint(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Short interpolationMethod, SmartMet::DataServer::Corba::CorbaParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    T::ParamValue sValue = 0;
    int result = mService->getGridValueByPoint(sessionId,fileId,messageIndex,flags,coordinateType,x,y,(short)interpolationMethod,sValue);

    if (result == 0)
      value = sValue;

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByCircle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, ::CORBA::Double origoX, ::CORBA::Double origoY, ::CORBA::Double radius, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    if (mService == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    int result = mService->getGridValueListByCircle(sessionId,fileId,messageIndex,flags,coordinateType,origoX,origoY,radius,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByPointList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& pointList, ::CORBA::Short interpolationMethod, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
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
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    int result = mService->getGridValueListByPointList(sessionId,fileId,messageIndex,flags,coordinateType,sPointList,(short)interpolationMethod,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}






::CORBA::Long ServerInterface::getGridValueListByPolygon(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& polygonPoints, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
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
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    int result = mService->getGridValueListByPolygon(sessionId,fileId,messageIndex,flags,coordinateType,sPolygonPoints,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByPolygonPath(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaPolygonPath& polygonPath, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
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
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    int result = mService->getGridValueListByPolygonPath(sessionId,fileId,messageIndex,flags,coordinateType,sPolygonPath,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueListByRectangle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, ::CORBA::Double x1, ::CORBA::Double y1, ::CORBA::Double x2, ::CORBA::Double y2, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList)
{
  FUNCTION_TRACE
  try
  {
    T::GridValueList sValueList;
    DataServer::Corba::CorbaGridValueList *corbaValueList = new DataServer::Corba::CorbaGridValueList();
    valueList = corbaValueList;

    if (mService == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    int result = mService->getGridValueListByRectangle(sessionId,fileId,messageIndex,flags,coordinateType,x1,y1,x2,y2,sValueList);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueList,*corbaValueList);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVector(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, SmartMet::DataServer::Corba::CorbaParamValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    DataServer::Corba::CorbaParamValueList *corbaValues = new DataServer::Corba::CorbaParamValueList();
    values = corbaValues;

    if (mService == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    int result = mService->getGridValueVector(sessionId,fileId,messageIndex,flags,sValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValues,*corbaValues);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVectorByCoordinateList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& coordinates, ::CORBA::Short interpolationMethod, SmartMet::DataServer::Corba::CorbaParamValueList_out values)
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
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    int result = mService->getGridValueVectorByCoordinateList(sessionId,fileId,messageIndex,coordinateType,sCoordinateList,interpolationMethod,sValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValues,*corbaValues);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVectorByRectangle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, ::CORBA::ULong columns, ::CORBA::ULong rows, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Double xStep, ::CORBA::Double yStep, ::CORBA::Short interpolationMethod, SmartMet::DataServer::Corba::CorbaParamValueList_out values)
{
  FUNCTION_TRACE
  try
  {
    T::ParamValue_vec sValues;
    DataServer::Corba::CorbaParamValueList *corbaValues = new DataServer::Corba::CorbaParamValueList();
    values = corbaValues;

    if (mService == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    int result = mService->getGridValueVectorByRectangle(sessionId,fileId,messageIndex,flags,coordinateType,columns,rows,x,y,xStep,yStep,(short)interpolationMethod,sValues);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValues,*corbaValues);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGridValueVectorByPoint(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::ULong vectorType, SmartMet::DataServer::Corba::CorbaDoubleList_out valueVector)
{
  FUNCTION_TRACE
  try
  {
    double_vec sValueVector;
    DataServer::Corba::CorbaDoubleList *corbaValueVector = new DataServer::Corba::CorbaDoubleList();
    valueVector = corbaValueVector;

    if (mService == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    int result = mService->getGridValueVectorByPoint(sessionId,fileId,messageIndex,flags,coordinateType,x,y,vectorType,sValueVector);

    if (result == 0)
      DataServer::Corba::Converter::convert(sValueVector,*corbaValueVector);

    return result;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}


}
}
}

