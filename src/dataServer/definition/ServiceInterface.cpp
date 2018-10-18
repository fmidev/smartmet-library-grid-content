#include "ServiceInterface.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/ShowFunction.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF



namespace SmartMet
{
namespace DataServer
{


ServiceInterface::ServiceInterface()
{
  FUNCTION_TRACE
  try
  {
    mDebugLog = nullptr;
    mProcessingLog = nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ServiceInterface::~ServiceInterface()
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





void ServiceInterface::setProcessingLog(Log *processingLog)
{
  FUNCTION_TRACE
  try
  {
    mProcessingLog = processingLog;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceInterface::setDebugLog(Log *debugLog)
{
  FUNCTION_TRACE
  try
  {
    mDebugLog = debugLog;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServiceInterface::shutdown()
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





int ServiceInterface::getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,T::GridCoordinates& coordinates)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridCoordinates(sessionId,fileId,messageIndex,flags,coordinateType,coordinates);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,flags,coordinateType,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::GridData& data)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridData(sessionId,fileId,messageIndex,flags,data);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%lu);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,flags,data.mValues.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridAttributeList(sessionId,fileId,messageIndex,flags,attributeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,Attribute[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,flags,attributeList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getGridFileCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridFileCount(sessionId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueByPoint(sessionId,fileId,messageIndex,flags,coordinateType,x,y,interpolationMethod,value);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%f,%f,%u,%f);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,flags,coordinateType,x,y,interpolationMethod,value,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueListByCircle(sessionId,fileId,messageIndex,flags,coordinateType,origoX,origoY,radius,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%f,%f,%f,GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,flags,coordinateType,origoX,origoY,radius,valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short interpolationMethod,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueListByPointList(sessionId,fileId,messageIndex,flags,coordinateType,pointList,interpolationMethod,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,Point[%lu],%u,GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,flags,coordinateType,pointList.size(),interpolationMethod,valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueListByPolygon(sessionId,fileId,messageIndex,flags,coordinateType,polygonPoints,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,Point[%lu],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,flags,coordinateType,polygonPoints.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueListByPolygonPath(sessionId,fileId,messageIndex,flags,coordinateType,polygonPath,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,Polygon[%lu],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,flags,coordinateType,polygonPath.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueListByRectangle(sessionId,fileId,messageIndex,flags,coordinateType,x1,y1,x2,y2,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%f,%f,%f,%f,GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,flags,coordinateType,x1,y1,x2,y2,valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueVector(sessionId,fileId,messageIndex,flags,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,ParamValue[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,flags,values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getGridValueVectorByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,uint flags,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,flags,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,%u,ParamValue[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),flags,values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getGridValueVectorByCoordinateList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,short interpolationMethod,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByCoordinateList(sessionId,fileId,messageIndex,coordinateType,coordinates,interpolationMethod,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,Coordinates[%lu],%u,ParamValue[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,coordinateType,coordinates.size(),interpolationMethod,values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getGridValueVectorByGeometryId(T::SessionId sessionId,uint fileId,uint messageIndex,T::GeometryId geometryId,short interpolationMethod,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByGeometryId(sessionId,fileId,messageIndex,geometryId,interpolationMethod,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,ParamValue[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,geometryId,interpolationMethod,values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,short interpolationMethod,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByRectangle(sessionId,fileId,messageIndex,flags,coordinateType,columns,rows,x,y,xStep,yStep,interpolationMethod,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%f,%f,%f,%f,%u,ParamValue[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,flags,coordinateType,columns,rows,x,y,xStep,yStep,interpolationMethod,values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getGridValueVectorByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,uint vectorType,double_vec& valueVector)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByPoint(sessionId,fileId,messageIndex,flags,coordinateType,x,y,vectorType,valueVector);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%f,%f,%u,value[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,flags,coordinateType,x,y,vectorType,valueVector.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getMultipleGridValues(T::SessionId sessionId,T::ValueRecordList& valueRecordList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getMultipleGridValues(sessionId,valueRecordList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,ValueRecord[%u]);result %d;time %f;",__FUNCTION__,sessionId,valueRecordList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





// Notice : If you known the geometryId of the current grid, then you can fetch coordinates
// much faster by using local definitions:
//
//    GRIB2::GridDef_ptr def = Identification::gridDef.getGrib2DefinitionByGeometryId(geometryId);
//    T::Coordinate_vec oridinalCoordinates = def->getGridCoordinates();
//    T::Coordinate_vec latLonCoordinates = def->getGridLatLonCoordinates();

int ServiceInterface::_getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,T::GridCoordinates& coordinates)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::GridData& data)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::AttributeList& attributeList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridFileCount(T::SessionId sessionId,uint& count)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::ParamValue& value)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short interpolationMethod,T::GridValueList& valueList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,T::GridValueList& valueList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,T::GridValueList& valueList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::ParamValue_vec& values)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueVectorByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,uint flags,T::ParamValue_vec& values)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueVectorByCoordinateList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,short interpolationMethod,T::ParamValue_vec& values)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueVectorByGeometryId(T::SessionId sessionId,uint fileId,uint messageIndex,T::GeometryId geometryId,short interpolationMethod,T::ParamValue_vec& values)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,short interpolationMethod,T::ParamValue_vec& values)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getMultipleGridValues(T::SessionId sessionId,T::ValueRecordList& valueRecordList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueVectorByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,uint vectorType,double_vec& valueVector)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}



}
}

