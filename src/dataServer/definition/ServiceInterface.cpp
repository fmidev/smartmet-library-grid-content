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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridCoordinates& coordinates)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridCoordinates(sessionId,fileId,messageIndex,coordinates);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGridValue(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValue(sessionId,fileId,messageIndex,coordinateType,x,y,interpolationMethod,value);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,%f,%f,%u,%f);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,(uint)coordinateType,x,y,(uint)interpolationMethod,value,result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGridValuesByArea(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,T::InterpolationMethod interpolationMethod,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getGridValuesByArea(sessionId,fileId,messageIndex,coordinateType,columns,rows,x,y,xStep,yStep,interpolationMethod,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLogPointer,"%s(%llu,%u,%u,%u,%u,%u,%f,%f,%f,%f,%u,ParamValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,(uint)coordinateType,columns,rows,x,y,xStep,yStep,(uint)interpolationMethod,(uint)values.size(),result,(float)requestTime / 1000000);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::_getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridCoordinates& coordinates)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getGridValueList(T::SessionId sessionId,T::ValueRecordList& valueRecordList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getGridValue(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::ParamValue& value)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}





int ServiceInterface::_getGridValuesByArea(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,T::InterpolationMethod interpolationMethod,T::ParamValue_vec& values)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}


}
}

