#include "ServiceInterface.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"



namespace SmartMet
{
namespace DataServer
{



ServiceInterface::ServiceInterface()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




ServiceInterface::~ServiceInterface()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}






void ServiceInterface::shutdown()
{
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
  try
  {
    int result = _getGridCoordinates(sessionId,fileId,messageIndex,coordinates);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data)
{
  try
  {
    int result = _getGridData(sessionId,fileId,messageIndex,data);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGridValueList(T::SessionId sessionId,T::ValueRecordList& valueRecordList)
{
  try
  {
    unsigned long long startTime = getTime();
    int result = _getGridValueList(sessionId,valueRecordList);
    unsigned long long endTime = getTime();
    printf("%s: %f sec\n\n",__FUNCTION__,(float)(endTime-startTime)/1000000);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList)
{
  try
  {
    int result = _getGridAttributeList(sessionId,fileId,messageIndex,attributeList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGridValue(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::ParamValue& value)
{
  try
  {
    int result = _getGridValue(sessionId,fileId,messageIndex,coordinateType,x,y,interpolationMethod,value);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getGridValuesByArea(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,T::InterpolationMethod interpolationMethod,T::ParamValue_vec& values)
{
  try
  {
    int result = _getGridValuesByArea(sessionId,fileId,messageIndex,coordinateType,columns,rows,x,y,xStep,yStep,interpolationMethod,values);
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

