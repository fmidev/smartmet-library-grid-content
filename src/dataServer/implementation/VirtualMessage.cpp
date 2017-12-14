#include "VirtualMessage.h"
#include <grid-files/common/ShowFunction.h>
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF



namespace SmartMet
{
namespace GRID
{




VirtualMessage::VirtualMessage(std::vector<SourceMessage>& sourceMessages)
{
  FUNCTION_TRACE
  try
  {
    //mGridFile = gridFile;
    mSourceMessages = sourceMessages;
    mLuaFileCollection = NULL;
    mFunctionCallMethod = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





VirtualMessage::~VirtualMessage()
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





void VirtualMessage::initMessagePtrs() const
{
  FUNCTION_TRACE
  try
  {
    // We cannot do the initialization in the constructor, because the source GRID files
    // do not necessary have their internal structure available (because they are not
    // memory mapped) when a virtual message is created.

    if (mMessageList.size() > 0)
      return; // Already initialized

    for (auto it = mSourceMessages.begin(); it != mSourceMessages.end(); ++it)
    {
      auto msg = it->first->getMessageByIndex(it->second);
      mMessageList.push_back(msg);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}



void VirtualMessage::setFunction(Functions::FunctionCollection *functionCollection,Lua::LuaFileCollection *luaFileCollection,std::string functionName,uint functionCallMethod)
{
  FUNCTION_TRACE
  try
  {
    mFunctionCollection = functionCollection;
    mLuaFileCollection = luaFileCollection;
    mFunctionName = functionName;
    mFunctionCallMethod = functionCallMethod;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::getAttributeList(std::string prefix,T::AttributeList& attributeList) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    mMessageList[0]->getAttributeList(prefix,attributeList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::TimeString VirtualMessage::getForecastTime() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getForecastTime();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





short VirtualMessage::getForecastType() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getForecastType();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




short VirtualMessage::getForecastNumber() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getForecastNumber();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::GeometryId VirtualMessage::getGridGeometryId() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridGeometryId();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::Coordinate_vec VirtualMessage::getGridCoordinates() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridCoordinates();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::Coordinate_vec VirtualMessage::getGridLatLonCoordinates() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridLatLonCoordinates();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::Dimensions_opt VirtualMessage::getGridDimensions() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridDimensions();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::Hash VirtualMessage::getGridHash() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridHash();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool VirtualMessage::getGridLatLonCoordinatesByGridPoint(uint grid_i,uint grid_j,double& lat,double& lon) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridLatLonCoordinatesByGridPoint(grid_i,grid_j,lat,lon);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool VirtualMessage::getGridLatLonCoordinatesByGridPosition(double grid_i,double grid_j,double& lat,double& lon) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridLatLonCoordinatesByGridPosition(grid_i,grid_j,lat,lon);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool VirtualMessage::getGridLatLonCoordinatesByOriginalCoordinates(double x,double y,double& lat,double& lon) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridLatLonCoordinatesByOriginalCoordinates(x,y,lat,lon);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool VirtualMessage::getGridOriginalCoordinatesByGridPoint(uint grid_i,uint grid_j,double& x,double& y) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridOriginalCoordinatesByGridPoint(grid_i,grid_j,x,y);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool VirtualMessage::getGridOriginalCoordinatesByGridPosition(double grid_i,double grid_j,double& x,double& y) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridOriginalCoordinatesByGridPosition(grid_i,grid_j,x,y);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool VirtualMessage::getGridOriginalCoordinatesByLatLonCoordinates(double lat,double lon,double& x,double& y) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridOriginalCoordinatesByLatLonCoordinates(lat,lon,x,y);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::GridLayout VirtualMessage::getGridLayout() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridLayout();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





std::size_t VirtualMessage::getGridOriginalColumnCount(std::size_t row) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridOriginalColumnCount(row);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





std::size_t VirtualMessage::getGridOriginalColumnCount() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridOriginalColumnCount();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





std::size_t VirtualMessage::getGridOriginalRowCount() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridOriginalRowCount();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





std::size_t VirtualMessage::getGridOriginalValueCount() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridOriginalValueCount();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int VirtualMessage::getGridOriginalValueIndex(uint grid_i,uint grid_j) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridOriginalValueIndex(grid_i,grid_j);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool VirtualMessage::getGridPointByLatLonCoordinates(double lat,double lon,double& grid_i,double& grid_j) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridPointByLatLonCoordinates(lat,lon,grid_i,grid_j);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool VirtualMessage::getGridPointByOriginalCoordinates(double x,double y,double& grid_i,double& grid_j) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridPointByOriginalCoordinates(x,y,grid_i,grid_j);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::GridProjection VirtualMessage::getGridProjection() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridProjection();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::getGridProjectionAttributes(std::string prefix,T::AttributeList& attributeList) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridProjectionAttributes(prefix,attributeList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





std::string VirtualMessage::getGridProjectionString() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridProjectionString();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool VirtualMessage::reverseXDirection() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->reverseXDirection();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool VirtualMessage::reverseYDirection() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->reverseYDirection();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::getGridValueByPoint(T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();

    T::ParamValue value1 = 0;
    T::ParamValue value2 = 0;

    std::vector<double> inParameters1;
    std::vector<double> inParameters2;
    std::vector<double> outParameters;
    std::vector<float> angles;

    mMessageList[0]->getGridValueByPoint(coordinateType,x,y,interpolationMethod,value1);
    inParameters1.push_back(value1);

    switch (mFunctionCallMethod)
    {
      case 2:
        if (mMessageList.size() != 1)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected one)!");

        executeFunctionCall2(inParameters1,outParameters);
        break;

      case 3:
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        mMessageList[1]->getGridValueByPoint(coordinateType,x,y,interpolationMethod,value2);
        inParameters2.push_back(value2);
        executeFunctionCall3(inParameters1,inParameters2,outParameters);
        break;

      case 4:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        mMessageList[1]->getGridValueByPoint(coordinateType,x,y,interpolationMethod,value2);
        inParameters2.push_back(value2);

        float angle = mMessageList[0]->getGridPointAngle(coordinateType,x,y);
        angles.push_back(angle);

        executeFunctionCall4(inParameters1,inParameters2,angles,outParameters);
      }
      break;
    }

    if (outParameters.size() == inParameters1.size())
      value = outParameters[0];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::getGridValueListByCircle(T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();

    switch (mFunctionCallMethod)
    {
      case 2:
        if (mMessageList.size() != 1)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected one)!");

        mMessageList[0]->getGridValueListByCircle(coordinateType,origoX,origoY,radius,valueList);
        executeFunctionCall2(valueList);
        break;

      case 3:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        mMessageList[0]->getGridValueListByCircle(coordinateType,origoX,origoY,radius,valueList1);
        mMessageList[1]->getGridValueListByCircle(coordinateType,origoX,origoY,radius,valueList2);
        executeFunctionCall3(valueList1,valueList2,valueList);
      }
      break;

      case 4:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        mMessageList[0]->getGridValueListByCircle(coordinateType,origoX,origoY,radius,valueList1);
        mMessageList[1]->getGridValueListByCircle(coordinateType,origoX,origoY,radius,valueList2);
        executeFunctionCall4(coordinateType,valueList1,valueList2,valueList);
      }
      break;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::getGridValueListByPointList(T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,T::InterpolationMethod interpolationMethod,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();

    switch (mFunctionCallMethod)
    {
      case 2:
        if (mMessageList.size() != 1)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected one)!");

        mMessageList[0]->getGridValueListByPointList(coordinateType,pointList,interpolationMethod,valueList);
        executeFunctionCall2(valueList);
        break;

      case 3:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        mMessageList[0]->getGridValueListByPointList(coordinateType,pointList,interpolationMethod,valueList1);
        mMessageList[1]->getGridValueListByPointList(coordinateType,pointList,interpolationMethod,valueList2);
        executeFunctionCall3(valueList1,valueList2,valueList);
      }
      break;

      case 4:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        mMessageList[0]->getGridValueListByPointList(coordinateType,pointList,interpolationMethod,valueList1);
        mMessageList[1]->getGridValueListByPointList(coordinateType,pointList,interpolationMethod,valueList2);
        executeFunctionCall4(coordinateType,valueList1,valueList2,valueList);
      }
      break;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::getGridValueListByPolygon(T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();

    switch (mFunctionCallMethod)
    {
      case 2:
        if (mMessageList.size() != 1)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected one)!");

        mMessageList[0]->getGridValueListByPolygon(coordinateType,polygonPoints,valueList);
        executeFunctionCall2(valueList);
        break;

      case 3:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        mMessageList[0]->getGridValueListByPolygon(coordinateType,polygonPoints,valueList1);
        mMessageList[1]->getGridValueListByPolygon(coordinateType,polygonPoints,valueList2);
        executeFunctionCall3(valueList1,valueList2,valueList);
      }
      break;

      case 4:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        mMessageList[0]->getGridValueListByPolygon(coordinateType,polygonPoints,valueList1);
        mMessageList[1]->getGridValueListByPolygon(coordinateType,polygonPoints,valueList2);
        executeFunctionCall4(coordinateType,valueList1,valueList2,valueList);
      }
      break;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::getGridValueListByPolygonPath(T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();

    switch (mFunctionCallMethod)
    {
      case 2:
        if (mMessageList.size() != 1)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected one)!");

        mMessageList[0]->getGridValueListByPolygonPath(coordinateType,polygonPath,valueList);
        executeFunctionCall2(valueList);
        break;

      case 3:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        mMessageList[0]->getGridValueListByPolygonPath(coordinateType,polygonPath,valueList1);
        mMessageList[1]->getGridValueListByPolygonPath(coordinateType,polygonPath,valueList2);
        executeFunctionCall3(valueList1,valueList2,valueList);
      }
      break;

      case 4:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        mMessageList[0]->getGridValueListByPolygonPath(coordinateType,polygonPath,valueList1);
        mMessageList[1]->getGridValueListByPolygonPath(coordinateType,polygonPath,valueList2);
        executeFunctionCall4(coordinateType,valueList1,valueList2,valueList);
      }
      break;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::getGridValueListByRectangle(T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();

    switch (mFunctionCallMethod)
    {
      case 1:
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected one)!");

        mMessageList[0]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,valueList);
        executeFunctionCall2(valueList);
        break;

      case 3:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        mMessageList[0]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,valueList1);
        mMessageList[1]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,valueList2);
        executeFunctionCall3(valueList1,valueList2,valueList);
      }
      break;

      case 4:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        mMessageList[0]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,valueList1);
        mMessageList[1]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,valueList2);
        executeFunctionCall4(coordinateType,valueList1,valueList2,valueList);
      }
      break;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::getGridValueVector(T::ParamValue_vec& values) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    T::Dimensions_opt d = getGridDimensions();
    if (!d)
      return;

    switch (mFunctionCallMethod)
    {
      case 2:
        if (mMessageList.size() != 1)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected one)!");

        mMessageList[0]->getGridValueVector(values);
        executeFunctionCall2(values);
        break;

      case 3:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::ParamValue_vec values1;
        T::ParamValue_vec values2;
        mMessageList[0]->getGridValueVector(values1);
        mMessageList[1]->getGridValueVector(values2);
        executeFunctionCall3(values1,values2,values);
      }
      break;

      case 4:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::ParamValue_vec values1;
        T::ParamValue_vec values2;

        mMessageList[0]->getGridValueVector(values1);
        mMessageList[1]->getGridValueVector(values2);

        std::vector<float> angles;
        getGridPointAngles(angles);
        executeFunctionCall4(values1,values2,angles,values);
      }
      break;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::getGridOriginalValueVector(T::ParamValue_vec& values) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();

    T::Dimensions_opt d = getGridDimensions();
    if (!d)
      return;

    switch (mFunctionCallMethod)
    {
      case 2:
        if (mMessageList.size() != 1)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected one)!");

        mMessageList[0]->getGridOriginalValueVector(values);
        executeFunctionCall2(values);
        break;

      case 3:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::ParamValue_vec values1;
        T::ParamValue_vec values2;
        mMessageList[0]->getGridOriginalValueVector(values1);
        mMessageList[1]->getGridOriginalValueVector(values2);
        executeFunctionCall3(values1,values2,values);
      }
      break;

      case 4:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::ParamValue_vec values1;
        T::ParamValue_vec values2;
        mMessageList[0]->getGridOriginalValueVector(values1);
        mMessageList[1]->getGridOriginalValueVector(values2);
        std::vector<float> angles;
        getGridPointAngles(angles);
        executeFunctionCall4(values1,values2,angles,values);
      }
      break;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::ParamLevel VirtualMessage::getGridParameterLevel() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridParameterLevel();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::ParamLevelId VirtualMessage::getGridParameterLevelId() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridParameterLevelId();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





std::string VirtualMessage::getGridParameterLevelIdString() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridParameterLevelIdString();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::getGridMinAndMaxValues(T::ParamValue& minValue,T::ParamValue& maxValue) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    mMessageList[0]->getGridMinAndMaxValues(minValue,maxValue);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::ParamValue VirtualMessage::getGridValueByGridPoint(uint grid_i,uint grid_j) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridValueByGridPoint(grid_i,grid_j);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::ParamValue VirtualMessage::getGridValueByGridPoint(double grid_i,double grid_j,T::InterpolationMethod interpolationMethod) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridValueByGridPoint(grid_i,grid_j,interpolationMethod);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::ParamValue VirtualMessage::getGridValueByGridPoint_nearest(double grid_i,double grid_j) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridValueByGridPoint_nearest(grid_i,grid_j);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::ParamValue VirtualMessage::getGridValueByGridPoint_linearInterpolation(double grid_i,double grid_j) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridValueByGridPoint_linearInterpolation(grid_i,grid_j);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::ParamValue VirtualMessage::getGridValueByLatLonCoordinate(double lat,double lon,T::InterpolationMethod interpolationMethod) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridValueByLatLonCoordinate(lat,lon,interpolationMethod);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::ParamValue VirtualMessage::getGridValueByOriginalGridPoint(uint grid_i,uint grid_j) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridValueByOriginalGridPoint(grid_i,grid_j);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::getGridValueVectorByRectangle(uint grid_i_start,uint grid_j_start,uint grid_i_end,uint grid_j_end,T::GridPointValue_vec& gridPointValues) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    mMessageList[0]->getGridValueVectorByRectangle(grid_i_start,grid_j_start,grid_i_end,grid_j_end,gridPointValues);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::getParameterValuesByRectangle(uint grid_i_start,uint grid_j_start,uint grid_i_end,uint grid_j_end,T::GridPointValueList& gridPointValues) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    mMessageList[0]->getParameterValuesByRectangle(grid_i_start,grid_j_start,grid_i_end,grid_j_end,gridPointValues);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::TimeString VirtualMessage::getReferenceTime() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getReferenceTime();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::SpatialReference* VirtualMessage::getSpatialReference() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getSpatialReference();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





std::string VirtualMessage::getWKT() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getWKT();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





bool VirtualMessage::isGridGlobal() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->isGridGlobal();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





double VirtualMessage::executeFunctionCall1(std::vector<double>& parameters) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 1)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 1)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::string functionName = mFunctionName;

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != NULL)
    {
      return function->executeFunctionCall1(parameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall1(functionName,parameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::executeFunctionCall2(std::vector<double>& inOutParameters) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 1)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 1)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }


    std::string functionName = mFunctionName;
    uint columns = (uint)getGridOriginalColumnCount();
    uint rows = (uint)getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != NULL)
    {
      return function->executeFunctionCall2(columns,rows,inOutParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall2(functionName,columns,rows,inOutParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::executeFunctionCall2(std::vector<double>& inParameters,std::vector<double>& outParameters) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 1)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 1)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::string functionName = mFunctionName;
    uint columns = (uint)getGridOriginalColumnCount();
    uint rows = (uint)getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != NULL)
    {
      return function->executeFunctionCall2(columns,rows,inParameters,outParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall2(functionName,columns,rows,inParameters,outParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::executeFunctionCall2(std::vector<float>& inParameters,std::vector<float>& outParameters) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 1)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 1)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::string functionName = mFunctionName;
    uint columns = (uint)getGridOriginalColumnCount();
    uint rows = (uint)getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != NULL)
    {
      return function->executeFunctionCall2(columns,rows,inParameters,outParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall2(functionName,columns,rows,inParameters,outParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::executeFunctionCall2(std::vector<float>& inOutParameters) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 1)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 1)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::string functionName = mFunctionName;
    uint columns = (uint)getGridOriginalColumnCount();
    uint rows = (uint)getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != NULL)
    {
      return function->executeFunctionCall2(columns,rows,inOutParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall2(functionName,columns,rows,inOutParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::executeFunctionCall2(T::GridValueList& valueList) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 1)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 1)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::vector<double> inParameters;
    std::vector<double> outParameters;

    uint len = valueList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::GridValue *value = valueList.getGridValueByIndex(t);
      inParameters.push_back(value->mValue);
    }

    executeFunctionCall2(inParameters,outParameters);

    if (outParameters.size() == inParameters.size())
    {
      for (uint t=0; t<len; t++)
      {
        T::GridValue *value = valueList.getGridValueByIndex(t);
        value->mValue = outParameters[t];
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::executeFunctionCall3(std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& outParameters) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 2)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 2)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::string functionName = mFunctionName;
    uint columns = (uint)getGridOriginalColumnCount();
    uint rows = (uint)getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != NULL)
    {
      return function->executeFunctionCall3(columns,rows,inParameters1,inParameters2,outParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall3(functionName,columns,rows,inParameters1,inParameters2,outParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::executeFunctionCall3(std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& outParameters) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 2)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 2)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::string functionName = mFunctionName;
    uint columns = (uint)getGridOriginalColumnCount();
    uint rows = (uint)getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != NULL)
    {
      return function->executeFunctionCall3(columns,rows,inParameters1,inParameters2,outParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall3(functionName,columns,rows,inParameters1,inParameters2,outParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::executeFunctionCall3(T::GridValueList& inValueList1,T::GridValueList& inValueList2,T::GridValueList& outValueList) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 2)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 2)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::vector<double> inParameters1;
    std::vector<double> inParameters2;
    std::vector<double> outParameters;

    uint len1 = inValueList1.getLength();
    uint len2 = inValueList2.getLength();

    if (len1 != len2)
    {
      SmartMet::Spine::Exception exception(BCP,"Input parameters must have the same number of values!");
      exception.addParameter("NumOfValues(param1)",std::to_string(len1));
      exception.addParameter("NumOfValues(param2)",std::to_string(len2));
      throw exception;
    }

    for (uint t=0; t<len1; t++)
    {
      T::GridValue *value1 = inValueList1.getGridValueByIndex(t);
      inParameters1.push_back(value1->mValue);

      T::GridValue *value2 = inValueList2.getGridValueByIndex(t);
      inParameters2.push_back(value2->mValue);
    }

    executeFunctionCall3(inParameters1,inParameters2,outParameters);

    if (outParameters.size() == inParameters1.size())
    {
      for (uint t=0; t<len1; t++)
      {
        T::GridValue *value = inValueList1.getGridValueByIndex(t);
        T::GridValue *newValue = new T::GridValue(*value);
        newValue->mValue = outParameters[t];
        outValueList.addGridValue(newValue);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::executeFunctionCall4(std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 2)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 2)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::string functionName = mFunctionName;
    uint columns = (uint)getGridOriginalColumnCount();
    uint rows = (uint)getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != NULL)
    {
      return function->executeFunctionCall4(columns,rows,inParameters1,inParameters2,angles,outParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall4(functionName,columns,rows,inParameters1,inParameters2,angles,outParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::executeFunctionCall4(std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 2)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 2)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::string functionName = mFunctionName;
    uint columns = (uint)getGridOriginalColumnCount();
    uint rows = (uint)getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != NULL)
    {
      return function->executeFunctionCall4(columns,rows,inParameters1,inParameters2,angles,outParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall4(functionName,columns,rows,inParameters1,inParameters2,angles,outParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::executeFunctionCall4(T::CoordinateType coordinateType,T::GridValueList& inValueList1,T::GridValueList& inValueList2,T::GridValueList& outValueList) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 2)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 2)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::vector<double> inParameters1;
    std::vector<double> inParameters2;
    std::vector<float> angles;
    std::vector<double> outParameters;

    uint len1 = inValueList1.getLength();
    uint len2 = inValueList2.getLength();

    if (len1 != len2)
    {
      SmartMet::Spine::Exception exception(BCP,"Input parameters must have the same number of values!");
      exception.addParameter("NumOfValues(param1)",std::to_string(len1));
      exception.addParameter("NumOfValues(param2)",std::to_string(len2));
      throw exception;
    }

    for (uint t=0; t<len1; t++)
    {
      T::GridValue *value1 = inValueList1.getGridValueByIndex(t);
      inParameters1.push_back(value1->mValue);
      float angle = mMessageList[0]->getGridPointAngle(coordinateType,value1->mX,value1->mY);
      angles.push_back(angle);

      T::GridValue *value2 = inValueList2.getGridValueByIndex(t);
      inParameters2.push_back(value2->mValue);
    }

    executeFunctionCall4(inParameters1,inParameters2,angles,outParameters);

    if (outParameters.size() == inParameters1.size())
    {
      for (uint t=0; t<len1; t++)
      {
        T::GridValue *value = inValueList1.getGridValueByIndex(t);
        T::GridValue *newValue = new T::GridValue(*value);
        newValue->mValue = outParameters[t];
        outValueList.addGridValue(newValue);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void VirtualMessage::setContentInfo(T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    mContentInfo = contentInfo;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





T::ContentInfo* VirtualMessage::getContentInfo()
{
  FUNCTION_TRACE
  try
  {
    return &mContentInfo;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualMessage::print(std::ostream& stream,uint level,uint optionFlags) const
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



}  // namespace GRID
}  // namespace SmartMet

