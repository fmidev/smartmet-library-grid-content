#include "VirtualMessage.h"
#include <grid-files/common/ShowFunction.h>
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF



namespace SmartMet
{
namespace GRID
{




VirtualMessage::VirtualMessage(GRID::VirtualGridFile *gridFile,std::vector<SourceMessage>& sourceMessages)
{
  FUNCTION_TRACE
  try
  {
    mVirtualGridFile = gridFile;
    mSourceMessages = sourceMessages;
    mLuaFileCollection = nullptr;
    mFunctionCallMethod = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
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
      /*
      if (strcasecmp(msg->getFmiParameterName().c_str(),mOverrideParameter.c_str()) == 0  &&  mOverrideParameter.length() > 0)
      {
        printf("SET VIRTUAL MESSAGE %u:%u %s\n",getFileId(),getMessageIndex(),msg->getFmiParameterName().c_str());
        msg->setVirtualMessage(getFileId(),getMessageIndex());
      }
      */

      mMessageList.push_back(msg);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint VirtualMessage::getFileId() const
{
  FUNCTION_TRACE
  try
  {
    if (mVirtualGridFile != nullptr)
      return mVirtualGridFile->getFileId();

    return 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the forecast time of the current grid.

        \return   The forecast time.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the forecast type of the current grid.

        \return   The forecast type.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the forecast number of the current grid.

        \return   The forecast number.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the grid geometry identifer.

        \return   The grid geometry identifier.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns all grid coordinates as a coordinate vector (in original projection).
    Notice that if the grid layout is "irregular" (i.e. its row lengths vary) then
    grid width is the same as the length of the longest grid row. I.e. the coordinates
    are returned as the grid would be a regular grid.

        \return   The grid coordinates.
*/

T::Coordinate_vec VirtualMessage::getGridCoordinates() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridOriginalCoordinates();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns all grid coordinates as a latlon coordinate vector. If the grid
    original coordiantes were not latlon coordinates then the original coordinates are converted
    to the latlon coordinates.

        \return   The grid latlon coordinates.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the grid dimensions (i.e. the width and the height).
    Notice that the grid might be irregular. For example, the number of rows might
    be specified while the number of columns is missing. This usually means that each
    row might have different number of columns.

        \return   The grid dimensions.
*/

T::Dimensions VirtualMessage::getGridDimensions() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridDimensions();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the grid hash value.

        \return   The grid hash value.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the grid latlon coordinates in the given grid point (= integer coordinates).

        \param grid_i  The grid i-coordinate.
        \param grid_j  The grid j-coordinate.
        \param lat     The latitude value is returned in this parameter.
        \param lon     The longitude value is returned in this parameter.
        \return   The method return true if the latlon values were succesfully returned.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the grid latlon coordinates in the given grid position (= double coordinates).

        \param grid_i  The grid i-coordinate.
        \param grid_j  The grid j-coordinate.
        \param lat     The latitude value is returned in this parameter.
        \param lon     The longitude value is returned in this parameter.
        \return        The method return true if the latlon values were succesfully returned.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the grid latlon coordinates according the grid original (projection) coordinates.

        \param x       The x-coordinate in the original projection.
        \param y       The y-coordinate in the original projection.
        \param lat     The latitude value is returned in this parameter.
        \param lon     The longitude value is returned in this parameter.
        \return        The method return true if the latlon values were succesfully returned.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the grid original (projection) coordinates in the given grid point (= integer coordinates).

        \param grid_i  The grid i-coordinate.
        \param grid_j  The grid j-coordinate.
        \param x       The x-coordinate in the original projection is returned in this parameter.
        \param y       The y-coordinate in the original projection is returned in this parameter.
        \return        The method return true if the original coordinates were succesfully returned.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the grid original (projection) coordinates in the given grid position (= double coordinates).

        \param grid_i  The grid i-coordinate.
        \param grid_j  The grid j-coordinate.
        \param x       The x-coordinate in the original projection is returned in this parameter.
        \param y       The y-coordinate in the original projection is returned in this parameter.
        \return        The method return true if the original coordinates were succesfully returned.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the grid original (projection) coordinates by the given latlon position.

        \param lat  The latitude value.
        \param lon  The longitude value.
        \param x    The x-coordinate in the original projection is returned in this parameter.
        \param y    The y-coordinate in the original projection is returned in this parameter.
        \return     The method return true if the original coordinates were succesfully returned.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the type of the grid layout.

     \return   The layout of the grid (expressed as an enum value).
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the number of columns used in the given original grid row.

      \param row    The grid row index (= j-position).
      \return       The number of columns in the given grid row.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the maximum number of the columns used in the original grid.
    If the grid is irregular, this method returns the length of the longest row.

       \return   The maximum number of the columns in the grid.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the number of rows used in the original grid.

     \return   The number of the grid rows.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the number of original values defined in the original grid.

      \return   The number of the values in the original grid.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the index of given grid position in the original grid data.
    In the case of irregular grid the grid rows might contain different number of
    columns. If the given grid position is outside of the grid data then the method
    returns -1. This is the case when the grid column (grid_i) is bigger than
    the length of the given grid row (grid_j), or if the given grid row (grid_j)
    is bigger than the actual number of grid rows.

      \param grid_i    The grid i-position.
      \param grid_j    The grid j-position.
      \return          The index of the original data position. If the position
                       is outside of the grid data then the method return -1.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief This method calculates the estimated grid position by using the latlon coordinates.
    The estimated grid position is returned in the 'grid_i' and 'grid_j' parameters.

        \param lat     The latitude.
        \param lon     The longitude.
        \param grid_i  The returned grid i-position.
        \param grid_j  The returned grid j-position.
        \return        Returns 'false' if the given coordinates are outside of the grid.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief This method calculates the estimated grid position by using the original coordinates.
    The estimated grid position is returned in the 'grid_i' and 'grid_j' parameters.

      \param x       The original x-coordinate.
      \param y       The original y-coordinate.
      \param grid_i  The returned grid i-position.
      \param grid_j  The returned grid j-position.
      \return        Returns 'false' if the given coordinates are outside of the grid.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief This method can be used for finding out the grid projection type (Mercator, LatLon, PolarStereographic, etc.).

        \return   The type of the grid projection (expressed as an enum value).
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns a list of grid projection attributes.

        \param prefix         The prefix that is added in the front of each attribute name.
        \param attributeList  The projection attributes are returned in this parameter.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the grid definition string (=> Projection name).

        \return   The projection used in the current grid (LatLon, Mercator, etc.)
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}






/*! \brief The method returns 'true' if the grid horizontal values are in the reverse order.

        \return   The method returns 'true' if the grid horizontal values are in the reverse
                  order. Otherwise it returns 'false'
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns 'true' if the grid vertical values are in the reverse order.

        \return   The method returns 'true' if the grid vertical values are in the reverse
                  order. Otherwise it returns 'false'
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void VirtualMessage::getGridValueByPoint(T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();

    T::ParamValue value1 = 0;
    T::ParamValue value2 = 0;
    T::ParamValue value3 = 0;
    T::ParamValue value4 = 0;

    std::vector<double> inParameters1;
    std::vector<double> inParameters2;
    std::vector<double> inParameters3;
    std::vector<double> inParameters4;
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

      case 7:
        if (mMessageList.size() != 3)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected three)!");

        mMessageList[1]->getGridValueByPoint(coordinateType,x,y,interpolationMethod,value2);
        inParameters2.push_back(value2);
        mMessageList[2]->getGridValueByPoint(coordinateType,x,y,interpolationMethod,value3);
        inParameters3.push_back(value3);
        executeFunctionCall7(inParameters1,inParameters2,inParameters3,outParameters);
        break;

      case 8:
        if (mMessageList.size() != 4)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected four)!");

        mMessageList[1]->getGridValueByPoint(coordinateType,x,y,interpolationMethod,value2);
        inParameters2.push_back(value2);
        mMessageList[2]->getGridValueByPoint(coordinateType,x,y,interpolationMethod,value3);
        inParameters3.push_back(value3);
        mMessageList[3]->getGridValueByPoint(coordinateType,x,y,interpolationMethod,value4);
        inParameters4.push_back(value4);
        executeFunctionCall8(inParameters1,inParameters2,inParameters3,inParameters4,outParameters);
        break;
    }

    if (outParameters.size() == inParameters1.size())
      value = outParameters[0];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

      case 7:
      {
        if (mMessageList.size() != 3)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected three)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        T::GridValueList valueList3;
        mMessageList[0]->getGridValueListByCircle(coordinateType,origoX,origoY,radius,valueList1);
        mMessageList[1]->getGridValueListByCircle(coordinateType,origoX,origoY,radius,valueList2);
        mMessageList[2]->getGridValueListByCircle(coordinateType,origoX,origoY,radius,valueList3);
        executeFunctionCall7(valueList1,valueList2,valueList3,valueList);
      }
      break;

      case 8:
      {
        if (mMessageList.size() != 4)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected four)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        T::GridValueList valueList3;
        T::GridValueList valueList4;
        mMessageList[0]->getGridValueListByCircle(coordinateType,origoX,origoY,radius,valueList1);
        mMessageList[1]->getGridValueListByCircle(coordinateType,origoX,origoY,radius,valueList2);
        mMessageList[2]->getGridValueListByCircle(coordinateType,origoX,origoY,radius,valueList3);
        mMessageList[3]->getGridValueListByCircle(coordinateType,origoX,origoY,radius,valueList4);
        executeFunctionCall8(valueList1,valueList2,valueList3,valueList4,valueList);
      }
      break;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void VirtualMessage::getGridValueListByPointList(T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short interpolationMethod,T::GridValueList& valueList)
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

      case 7:
      {
        if (mMessageList.size() != 3)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected three)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        T::GridValueList valueList3;
        mMessageList[0]->getGridValueListByPointList(coordinateType,pointList,interpolationMethod,valueList1);
        mMessageList[1]->getGridValueListByPointList(coordinateType,pointList,interpolationMethod,valueList2);
        mMessageList[2]->getGridValueListByPointList(coordinateType,pointList,interpolationMethod,valueList3);
        executeFunctionCall7(valueList1,valueList2,valueList3,valueList);
      }
      break;

      case 8:
      {
        if (mMessageList.size() != 4)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected four)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        T::GridValueList valueList3;
        T::GridValueList valueList4;
        mMessageList[0]->getGridValueListByPointList(coordinateType,pointList,interpolationMethod,valueList1);
        mMessageList[1]->getGridValueListByPointList(coordinateType,pointList,interpolationMethod,valueList2);
        mMessageList[2]->getGridValueListByPointList(coordinateType,pointList,interpolationMethod,valueList3);
        mMessageList[3]->getGridValueListByPointList(coordinateType,pointList,interpolationMethod,valueList4);
        executeFunctionCall8(valueList1,valueList2,valueList3,valueList4,valueList);
      }
      break;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

      case 7:
      {
        if (mMessageList.size() != 3)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected three)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        T::GridValueList valueList3;
        mMessageList[0]->getGridValueListByPolygon(coordinateType,polygonPoints,valueList1);
        mMessageList[1]->getGridValueListByPolygon(coordinateType,polygonPoints,valueList2);
        mMessageList[2]->getGridValueListByPolygon(coordinateType,polygonPoints,valueList3);
        executeFunctionCall7(valueList1,valueList2,valueList3,valueList);
      }
      break;

      case 8:
      {
        if (mMessageList.size() != 4)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected four)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        T::GridValueList valueList3;
        T::GridValueList valueList4;
        mMessageList[0]->getGridValueListByPolygon(coordinateType,polygonPoints,valueList1);
        mMessageList[1]->getGridValueListByPolygon(coordinateType,polygonPoints,valueList2);
        mMessageList[2]->getGridValueListByPolygon(coordinateType,polygonPoints,valueList3);
        executeFunctionCall8(valueList1,valueList2,valueList3,valueList4,valueList);
      }
      break;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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

      case 7:
      {
        if (mMessageList.size() != 3)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected three)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        T::GridValueList valueList3;
        mMessageList[0]->getGridValueListByPolygonPath(coordinateType,polygonPath,valueList1);
        mMessageList[1]->getGridValueListByPolygonPath(coordinateType,polygonPath,valueList2);
        mMessageList[2]->getGridValueListByPolygonPath(coordinateType,polygonPath,valueList3);
        executeFunctionCall7(valueList1,valueList2,valueList3,valueList);
      }
      break;

      case 8:
      {
        if (mMessageList.size() != 4)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected four)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        T::GridValueList valueList3;
        T::GridValueList valueList4;
        mMessageList[0]->getGridValueListByPolygonPath(coordinateType,polygonPath,valueList1);
        mMessageList[1]->getGridValueListByPolygonPath(coordinateType,polygonPath,valueList2);
        mMessageList[2]->getGridValueListByPolygonPath(coordinateType,polygonPath,valueList3);
        mMessageList[3]->getGridValueListByPolygonPath(coordinateType,polygonPath,valueList4);
        executeFunctionCall8(valueList1,valueList2,valueList3,valueList4,valueList);
      }
      break;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void VirtualMessage::getGridValueListByRectangle(T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,bool gridRectangle,T::GridValueList& valueList)
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

        mMessageList[0]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,gridRectangle,valueList);
        executeFunctionCall2(valueList);
        break;

      case 3:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        mMessageList[0]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,gridRectangle,valueList1);
        mMessageList[1]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,gridRectangle,valueList2);
        executeFunctionCall3(valueList1,valueList2,valueList);
      }
      break;

      case 4:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        mMessageList[0]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,gridRectangle,valueList1);
        mMessageList[1]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,gridRectangle,valueList2);
        executeFunctionCall4(coordinateType,valueList1,valueList2,valueList);
      }
      break;

      case 7:
      {
        if (mMessageList.size() != 3)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected three)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        T::GridValueList valueList3;
        mMessageList[0]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,gridRectangle,valueList1);
        mMessageList[1]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,gridRectangle,valueList2);
        mMessageList[2]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,gridRectangle,valueList3);
        executeFunctionCall7(valueList1,valueList2,valueList3,valueList);
      }
      break;

      case 8:
      {
        if (mMessageList.size() != 4)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected four)!");

        T::GridValueList valueList1;
        T::GridValueList valueList2;
        T::GridValueList valueList3;
        T::GridValueList valueList4;
        mMessageList[0]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,gridRectangle,valueList1);
        mMessageList[1]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,gridRectangle,valueList2);
        mMessageList[2]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,gridRectangle,valueList3);
        mMessageList[3]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,gridRectangle,valueList4);
        executeFunctionCall8(valueList1,valueList2,valueList3,valueList4,valueList);
      }
      break;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void VirtualMessage::getGridValueVector(T::ParamValue_vec& values) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    T::Dimensions d = getGridDimensions();
    if (d.getDimensions() != 2)
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

      case 7:
      {
        if (mMessageList.size() != 3)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected three)!");

        T::ParamValue_vec values1;
        T::ParamValue_vec values2;
        T::ParamValue_vec values3;
        mMessageList[0]->getGridValueVector(values1);
        mMessageList[1]->getGridValueVector(values2);
        mMessageList[2]->getGridValueVector(values3);
        executeFunctionCall7(values1,values2,values3,values);
      }
      break;

      case 8:
      {
        if (mMessageList.size() != 4)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected four)!");

        T::ParamValue_vec values1;
        T::ParamValue_vec values2;
        T::ParamValue_vec values3;
        T::ParamValue_vec values4;
        mMessageList[0]->getGridValueVector(values1);
        mMessageList[1]->getGridValueVector(values2);
        mMessageList[2]->getGridValueVector(values3);
        mMessageList[3]->getGridValueVector(values4);
        executeFunctionCall8(values1,values2,values3,values4,values);
      }
      break;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void VirtualMessage::getGridOriginalValueVector(T::ParamValue_vec& values) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();

    T::Dimensions d = getGridDimensions();
    if (d.getDimensions() != 2)
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

      case 7:
      {
        if (mMessageList.size() != 3)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected three)!");

        T::ParamValue_vec values1;
        T::ParamValue_vec values2;
        T::ParamValue_vec values3;
        mMessageList[0]->getGridOriginalValueVector(values1);
        mMessageList[1]->getGridOriginalValueVector(values2);
        mMessageList[2]->getGridOriginalValueVector(values3);
        executeFunctionCall7(values1,values2,values3,values);
      }
      break;

      case 8:
      {
        if (mMessageList.size() != 4)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected four)!");

        T::ParamValue_vec values1;
        T::ParamValue_vec values2;
        T::ParamValue_vec values3;
        T::ParamValue_vec values4;
        mMessageList[0]->getGridOriginalValueVector(values1);
        mMessageList[1]->getGridOriginalValueVector(values2);
        mMessageList[2]->getGridOriginalValueVector(values3);
        mMessageList[3]->getGridOriginalValueVector(values4);
        executeFunctionCall8(values1,values2,values3,values4,values);
      }
      break;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void VirtualMessage::getGridMinAndMaxValues(T::ParamValue& minValue,T::ParamValue& maxValue) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    Message::getGridMinAndMaxValues(minValue,maxValue);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





T::ParamValue VirtualMessage::getGridValueByGridPoint(uint grid_i,uint grid_j) const
{
  FUNCTION_TRACE
  try
  {
    //initMessagePtrs();
    //return mMessageList[0]->getGridValueByGridPoint(grid_i,grid_j);

    initMessagePtrs();
    T::ParamValue_vec values;

    T::Dimensions d = getGridDimensions();
    if (d.getDimensions() != 2)
      return ParamValueMissing;

    switch (mFunctionCallMethod)
    {
      case 2:
        if (mMessageList.size() != 1)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected one)!");

        values.push_back(mMessageList[0]->getGridValueByGridPoint(grid_i,grid_j));
        executeFunctionCall2(values);
        break;

      case 3:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::ParamValue_vec values1;
        T::ParamValue_vec values2;
        values1.push_back(mMessageList[0]->getGridValueByGridPoint(grid_i,grid_j));
        values2.push_back(mMessageList[1]->getGridValueByGridPoint(grid_i,grid_j));
        executeFunctionCall3(values1,values2,values);
      }
      break;

      case 4:
      {
        if (mMessageList.size() != 2)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected two)!");

        T::ParamValue_vec values1;
        T::ParamValue_vec values2;
        values1.push_back(mMessageList[0]->getGridValueByGridPoint(grid_i,grid_j));
        values2.push_back(mMessageList[1]->getGridValueByGridPoint(grid_i,grid_j));
        std::vector<float> angles;
        getGridPointAngles(angles);
        executeFunctionCall4(values1,values2,angles,values);
      }
      break;

      case 7:
      {
        if (mMessageList.size() != 3)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected three)!");

        T::ParamValue_vec values1;
        T::ParamValue_vec values2;
        T::ParamValue_vec values3;
        values1.push_back(mMessageList[0]->getGridValueByGridPoint(grid_i,grid_j));
        values2.push_back(mMessageList[1]->getGridValueByGridPoint(grid_i,grid_j));
        values3.push_back(mMessageList[2]->getGridValueByGridPoint(grid_i,grid_j));
        executeFunctionCall7(values1,values2,values3,values);
      }
      break;

      case 8:
      {
        if (mMessageList.size() != 4)
          throw SmartMet::Spine::Exception(BCP,"Invalid number of parameters (expected four)!");

        T::ParamValue_vec values1;
        T::ParamValue_vec values2;
        T::ParamValue_vec values3;
        T::ParamValue_vec values4;
        values1.push_back(mMessageList[0]->getGridValueByGridPoint(grid_i,grid_j));
        values2.push_back(mMessageList[1]->getGridValueByGridPoint(grid_i,grid_j));
        values3.push_back(mMessageList[2]->getGridValueByGridPoint(grid_i,grid_j));
        values4.push_back(mMessageList[3]->getGridValueByGridPoint(grid_i,grid_j));
        executeFunctionCall8(values1,values2,values3,values4,values);
      }
      break;

    }
    if (values.size() == 1)
      return values[0];

    return ParamValueMissing;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





T::ParamValue VirtualMessage::getGridValueByGridPoint_byInterpolation(double grid_i,double grid_j,short interpolationMethod) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return Message::getGridValueByGridPoint_byInterpolation(grid_i,grid_j,interpolationMethod);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





T::ParamValue VirtualMessage::getGridValueByGridPoint_nearest(double grid_i,double grid_j) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return Message::getGridValueByGridPoint_nearest(grid_i,grid_j);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





T::ParamValue VirtualMessage::getGridValueByGridPoint_linearInterpolation(double grid_i,double grid_j) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return Message::getGridValueByGridPoint_linearInterpolation(grid_i,grid_j);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





T::ParamValue VirtualMessage::getGridValueByLatLonCoordinate(double lat,double lon,short interpolationMethod) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return Message::getGridValueByLatLonCoordinate(lat,lon,interpolationMethod);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





T::ParamValue VirtualMessage::getGridValueByOriginalGridPoint(uint grid_i,uint grid_j) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return Message::getGridValueByOriginalGridPoint(grid_i,grid_j);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void VirtualMessage::getGridValueVectorByRectangle(uint grid_i_start,uint grid_j_start,uint grid_i_end,uint grid_j_end,T::GridPointValue_vec& gridPointValues) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    Message::getGridValueVectorByRectangle(grid_i_start,grid_j_start,grid_i_end,grid_j_end,gridPointValues);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void VirtualMessage::getParameterValuesByRectangle(uint grid_i_start,uint grid_j_start,uint grid_i_end,uint grid_j_end,T::GridPointValueList& gridPointValues) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    Message::getParameterValuesByRectangle(grid_i_start,grid_j_start,grid_i_end,grid_j_end,gridPointValues);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the reference time of the current grid. The forecast
    times are calculated from this.

        \return   The reference time of the current grid.
*/

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





/*! \brief The method returns the pointer to the spatial reference of the current grid.

        \return   The pointer to the spatial reference.
*/

T::SpatialRef* VirtualMessage::getSpatialReference() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getSpatialReference();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    if (function != nullptr)
    {
      return function->executeFunctionCall1(parameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall1(functionName,parameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    uint columns = getGridOriginalColumnCount();
    uint rows = getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != nullptr)
    {
      return function->executeFunctionCall2(columns,rows,inOutParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall2(functionName,columns,rows,inOutParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    uint columns = getGridOriginalColumnCount();
    uint rows = getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != nullptr)
    {
      return function->executeFunctionCall2(columns,rows,inParameters,outParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall2(functionName,columns,rows,inParameters,outParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    uint columns = getGridOriginalColumnCount();
    uint rows = getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != nullptr)
    {
      return function->executeFunctionCall2(columns,rows,inParameters,outParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall2(functionName,columns,rows,inParameters,outParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    uint columns = getGridOriginalColumnCount();
    uint rows = getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    std::cout << "FUNCTION : " << functionName << "\n";
    auto function = mFunctionCollection->getFunction(functionName);
    if (function != nullptr)
    {
      std::cout << "C++ call\n";
      return function->executeFunctionCall2(columns,rows,inOutParameters);
    }

    // Calling the LUA implementation of the function

    std::cout << "LUA call\n";
    return mLuaFileCollection->executeFunctionCall2(functionName,columns,rows,inOutParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      T::GridValue value;
      valueList.getGridValueByIndex(t,value);
      inParameters.push_back(value.mValue);
    }

    executeFunctionCall2(inParameters,outParameters);

    if (outParameters.size() == inParameters.size())
    {
      for (uint t=0; t<len; t++)
      {
        T::GridValue value;
        valueList.getGridValueByIndex(t,value);
        value.mValue = outParameters[t];
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    uint columns = getGridOriginalColumnCount();
    uint rows = getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != nullptr)
    {
      return function->executeFunctionCall3(columns,rows,inParameters1,inParameters2,outParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall3(functionName,columns,rows,inParameters1,inParameters2,outParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    uint columns = getGridOriginalColumnCount();
    uint rows = getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != nullptr)
    {
      return function->executeFunctionCall3(columns,rows,inParameters1,inParameters2,outParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall3(functionName,columns,rows,inParameters1,inParameters2,outParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      T::GridValue value1;
      inValueList1.getGridValueByIndex(t,value1);
      inParameters1.push_back(value1.mValue);

      T::GridValue value2;
      inValueList2.getGridValueByIndex(t,value2);
      inParameters2.push_back(value2.mValue);
    }

    executeFunctionCall3(inParameters1,inParameters2,outParameters);

    if (outParameters.size() == inParameters1.size())
    {
      for (uint t=0; t<len1; t++)
      {
        T::GridValue value;
        inValueList1.getGridValueByIndex(t,value);
        T::GridValue newValue(value);
        newValue.mValue = outParameters[t];
        outValueList.addGridValue(newValue);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    uint columns = getGridOriginalColumnCount();
    uint rows = getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != nullptr)
    {
      return function->executeFunctionCall4(columns,rows,inParameters1,inParameters2,angles,outParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall4(functionName,columns,rows,inParameters1,inParameters2,angles,outParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    uint columns = getGridOriginalColumnCount();
    uint rows = getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != nullptr)
    {
      return function->executeFunctionCall4(columns,rows,inParameters1,inParameters2,angles,outParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall4(functionName,columns,rows,inParameters1,inParameters2,angles,outParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      T::GridValue value1;
      inValueList1.getGridValueByIndex(t,value1);
      inParameters1.push_back(value1.mValue);
      float angle = mMessageList[0]->getGridPointAngle(coordinateType,value1.mX,value1.mY);
      angles.push_back(angle);

      T::GridValue value2;
      inValueList2.getGridValueByIndex(t,value2);
      inParameters2.push_back(value2.mValue);
    }

    executeFunctionCall4(inParameters1,inParameters2,angles,outParameters);

    if (outParameters.size() == inParameters1.size())
    {
      for (uint t=0; t<len1; t++)
      {
        T::GridValue value;
        inValueList1.getGridValueByIndex(t,value);
        T::GridValue newValue(value);
        newValue.mValue = outParameters[t];
        outValueList.addGridValue(newValue);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void VirtualMessage::executeFunctionCall7(std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& inParameters3,std::vector<float>& outParameters) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 3)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 3)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::string functionName = mFunctionName;
    uint columns = getGridOriginalColumnCount();
    uint rows = getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != nullptr)
    {
      return function->executeFunctionCall7(columns,rows,inParameters1,inParameters2,inParameters3,outParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall7(functionName,columns,rows,inParameters1,inParameters2,inParameters3,outParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void VirtualMessage::executeFunctionCall7(std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& inParameters3,std::vector<double>& outParameters) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 3)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 3)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::string functionName = mFunctionName;
    uint columns = getGridOriginalColumnCount();
    uint rows = getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != nullptr)
    {
      return function->executeFunctionCall7(columns,rows,inParameters1,inParameters2,inParameters3,outParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall7(functionName,columns,rows,inParameters1,inParameters2,inParameters3,outParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void VirtualMessage::executeFunctionCall7(T::GridValueList& inValueList1,T::GridValueList& inValueList2,T::GridValueList& inValueList3,T::GridValueList& outValueList) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 3)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 3)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::vector<double> inParameters1;
    std::vector<double> inParameters2;
    std::vector<double> inParameters3;
    std::vector<double> outParameters;

    uint len1 = inValueList1.getLength();
    uint len2 = inValueList2.getLength();
    uint len3 = inValueList3.getLength();

    if (len1 != len2 || len1 != len3)
    {
      SmartMet::Spine::Exception exception(BCP,"Input parameters must have the same number of values!");
      exception.addParameter("NumOfValues(param1)",std::to_string(len1));
      exception.addParameter("NumOfValues(param2)",std::to_string(len2));
      exception.addParameter("NumOfValues(param3)",std::to_string(len3));
      throw exception;
    }

    for (uint t=0; t<len1; t++)
    {
      T::GridValue value1;
      inValueList1.getGridValueByIndex(t,value1);
      inParameters1.push_back(value1.mValue);

      T::GridValue value2;
      inValueList2.getGridValueByIndex(t,value2);
      inParameters2.push_back(value2.mValue);

      T::GridValue value3;
      inValueList3.getGridValueByIndex(t,value3);
      inParameters3.push_back(value3.mValue);
    }

    executeFunctionCall7(inParameters1,inParameters2,inParameters3,outParameters);

    if (outParameters.size() == inParameters1.size())
    {
      for (uint t=0; t<len1; t++)
      {
        T::GridValue value;
        inValueList1.getGridValueByIndex(t,value);
        T::GridValue newValue(value);
        newValue.mValue = outParameters[t];
        outValueList.addGridValue(newValue);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void VirtualMessage::executeFunctionCall8(std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& inParameters3,std::vector<float>& inParameters4,std::vector<float>& outParameters) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 4)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 4)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::string functionName = mFunctionName;
    uint columns = getGridOriginalColumnCount();
    uint rows = getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != nullptr)
    {
      return function->executeFunctionCall8(columns,rows,inParameters1,inParameters2,inParameters3,inParameters4,outParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall8(functionName,columns,rows,inParameters1,inParameters2,inParameters3,inParameters4,outParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void VirtualMessage::executeFunctionCall8(std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& inParameters3,std::vector<double>& inParameters4,std::vector<double>& outParameters) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 4)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 4)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::string functionName = mFunctionName;
    uint columns = getGridOriginalColumnCount();
    uint rows = getGridOriginalRowCount();

    // Trying to find C++ implementation of the function

    auto function = mFunctionCollection->getFunction(functionName);
    if (function != nullptr)
    {
      return function->executeFunctionCall8(columns,rows,inParameters1,inParameters2,inParameters3,inParameters4,outParameters);
    }

    // Calling the LUA implementation of the function

    return mLuaFileCollection->executeFunctionCall8(functionName,columns,rows,inParameters1,inParameters2,inParameters3,inParameters4,outParameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void VirtualMessage::executeFunctionCall8(T::GridValueList& inValueList1,T::GridValueList& inValueList2,T::GridValueList& inValueList3,T::GridValueList& inValueList4,T::GridValueList& outValueList) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() != 4)
    {
      SmartMet::Spine::Exception exception(BCP,"Invalid number of source grids (expected 4)!");
      exception.addParameter("NumberOfGrids",std::to_string(mMessageList.size()));
      throw exception;
    }

    std::vector<double> inParameters1;
    std::vector<double> inParameters2;
    std::vector<double> inParameters3;
    std::vector<double> inParameters4;
    std::vector<double> outParameters;

    uint len1 = inValueList1.getLength();
    uint len2 = inValueList2.getLength();
    uint len3 = inValueList3.getLength();
    uint len4 = inValueList4.getLength();

    if (len1 != len2 || len1 != len3 || len1 != len4)
    {
      SmartMet::Spine::Exception exception(BCP,"Input parameters must have the same number of values!");
      exception.addParameter("NumOfValues(param1)",std::to_string(len1));
      exception.addParameter("NumOfValues(param2)",std::to_string(len2));
      exception.addParameter("NumOfValues(param3)",std::to_string(len3));
      exception.addParameter("NumOfValues(param4)",std::to_string(len4));
      throw exception;
    }

    for (uint t=0; t<len1; t++)
    {
      T::GridValue value1;
      inValueList1.getGridValueByIndex(t,value1);
      inParameters1.push_back(value1.mValue);

      T::GridValue value2;
      inValueList2.getGridValueByIndex(t,value2);
      inParameters2.push_back(value2.mValue);

      T::GridValue value3;
      inValueList3.getGridValueByIndex(t,value3);
      inParameters3.push_back(value3.mValue);

      T::GridValue value4;
      inValueList4.getGridValueByIndex(t,value4);
      inParameters4.push_back(value4.mValue);
    }

    executeFunctionCall8(inParameters1,inParameters2,inParameters3,inParameters4,outParameters);

    if (outParameters.size() == inParameters1.size())
    {
      for (uint t=0; t<len1; t++)
      {
        T::GridValue value;
        inValueList1.getGridValueByIndex(t,value);
        T::GridValue newValue(value);
        newValue.mValue = outParameters[t];
        outValueList.addGridValue(newValue);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}



}  // namespace GRID
}  // namespace SmartMet

