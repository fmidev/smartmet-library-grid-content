#include "ServiceInterface.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"
#include "grid-files/common/ShowFunction.h"


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
    mProcessingLogPointer = NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ServiceInterface::setProcessingLog(Log *processingLogPointer)
{
  FUNCTION_TRACE
  try
  {
    mProcessingLogPointer = processingLogPointer;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int ServiceInterface::getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,T::GridCoordinates& coordinates)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridCoordinates(sessionId,fileId,messageIndex,coordinateType,coordinates);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,(uint)coordinateType,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int ServiceInterface::getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridData(sessionId,fileId,messageIndex,data);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int ServiceInterface::getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridAttributeList(sessionId,fileId,messageIndex,attributeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,Attribute[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,attributeList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int ServiceInterface::getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueByPoint(sessionId,fileId,messageIndex,coordinateType,x,y,interpolationMethod,value);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,%f,%f,%u,%f);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,(uint)coordinateType,x,y,(uint)interpolationMethod,value,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int ServiceInterface::getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueListByCircle(sessionId,fileId,messageIndex,coordinateType,origoX,origoY,radius,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,%f,%f,%f,GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,(uint)coordinateType,origoX,origoY,radius,valueList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int ServiceInterface::getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueListByPolygon(sessionId,fileId,messageIndex,coordinateType,polygonPoints,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,Point[%u],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,(uint)coordinateType,(uint)polygonPoints.size(),valueList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int ServiceInterface::getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueListByRectangle(sessionId,fileId,messageIndex,coordinateType,x1,y1,x2,y2,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,%f,%f,%f,%f,GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,(uint)coordinateType,x1,y1,x2,y2,valueList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int ServiceInterface::getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueVector(sessionId,fileId,messageIndex,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,ParamValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,(uint)values.size(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int ServiceInterface::getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,T::InterpolationMethod interpolationMethod,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByRectangle(sessionId,fileId,messageIndex,coordinateType,columns,rows,x,y,xStep,yStep,interpolationMethod,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,%u,%u,%f,%f,%f,%f,%u,ParamValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,(uint)coordinateType,columns,rows,x,y,xStep,yStep,(uint)interpolationMethod,(uint)values.size(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int ServiceInterface::getGridValueList(T::SessionId sessionId,T::ValueRecordList& valueRecordList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValueList(sessionId,valueRecordList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,ValueRecord[%u]);result %d;time %f;",__FUNCTION__,sessionId,valueRecordList.getLength(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





// Notice : If you known the geometryId of the current grid, then you can fetch coordinates
// much faster by using local definitions:
//
//    GRIB2::GridDefinition_ptr def = Identification::gribDef.getGridDefinition2ByGeometryId(geometryId);
//    T::Coordinate_vec oridinalCoordinates = def->getGridCoordinates();
//    T::Coordinate_vec latLonCoordinates = def->getGridLatLonCoordinates();

int ServiceInterface::_getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,T::GridCoordinates& coordinates)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::ParamValue& value)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}




int ServiceInterface::_getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,T::GridValueList& valueList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& values)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,T::InterpolationMethod interpolationMethod,T::ParamValue_vec& values)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getGridValueList(T::SessionId sessionId,T::ValueRecordList& valueRecordList)
{
  throw SmartMet::Spine::Exception(BCP,exception_implementation_required);
}





}
}

