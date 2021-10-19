#include "VirtualMessage.h"
#include <grid-files/common/ShowFunction.h>
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <macgyver/StringConversion.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF



namespace SmartMet
{
namespace GRID
{




VirtualMessage::VirtualMessage(GRID::VirtualGridFile *gridFile,DataServer::ParameterDef& paramDef,std::vector<SourceMessage>& sourceMessages)
{
  FUNCTION_TRACE
  try
  {
    mVirtualGridFile = gridFile;
    mParamDef = paramDef;
    mSourceMessages = sourceMessages;
    mLuaFileCollection = nullptr;
    mFunctionCallMethod = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
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
      if (strcasecmp(msg->getFmiParameterName(),mOverrideParameter.c_str()) == 0  &&  mOverrideParameter.length() > 0)
      {
        printf("SET VIRTUAL MESSAGE %u:%u %s\n",getFileId(),getMessageIndex(),msg->getFmiParameterName());
        msg->setVirtualMessage(getFileId(),getMessageIndex());
      }
      */

      mMessageList.emplace_back(msg);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void VirtualMessage::setFunction(Functions::FunctionCollection *functionCollection,Lua::LuaFileCollection *luaFileCollection,const std::string& functionName,uint functionCallMethod,std::vector<double>& functionParameters)
{
  FUNCTION_TRACE
  try
  {
    mFunctionCollection = functionCollection;
    mLuaFileCollection = luaFileCollection;
    mFunctionName = functionName;
    mFunctionCallMethod = functionCallMethod;
    mFunctionParameters = functionParameters;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void VirtualMessage::getAttributeList(const std::string& prefix,T::AttributeList& attributeList) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    mMessageList[0]->getAttributeList(prefix,attributeList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool VirtualMessage::hasAttributeValue(const char *attributeName, const char *attributeValue) const
{
  return false;
}




bool VirtualMessage::getAttributeValue(const char *attributeName, std::string& attributeValue) const
{
  return false;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





time_t VirtualMessage::getForecastTimeT() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getForecastTimeT();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    return toInt16(mParamDef.mForecastType);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    return toInt16(mParamDef.mForecastNumber);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    return toInt32(mParamDef.mGeometryId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief The method returns all grid coordinates as a coordinate vector (in original projection).
    Notice that if the grid layout is "irregular" (i.e. its row lengths vary) then
    grid width is the same as the length of the longest grid row. I.e. the coordinates
    are returned as the grid would be a regular grid.

        \return   The grid coordinates.
*/

T::Coordinate_svec VirtualMessage::getGridCoordinates() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridOriginalCoordinates();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief The method returns all grid coordinates as a latlon coordinate vector. If the grid
    original coordiantes were not latlon coordinates then the original coordinates are converted
    to the latlon coordinates.

        \return   The grid latlon coordinates.
*/

T::Coordinate_svec VirtualMessage::getGridLatLonCoordinates() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridLatLonCoordinates();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief The method returns the type of the grid layout.

     \return   The layout of the grid (expressed as an enum value).
*/
/*
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}
*/




/*! \brief The method returns the number of columns used in the given original grid row.

      \param row    The grid row index (= j-position).
      \return       The number of columns in the given grid row.
*/
/*
std::size_t VirtualMessage::getGridColumnCount(std::size_t row) const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridColumnCount(row);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}
*/




/*! \brief The method returns the maximum number of the columns used in the original grid.
    If the grid is irregular, this method returns the length of the longest row.

       \return   The maximum number of the columns in the grid.
*/
/*
std::size_t VirtualMessage::getGridColumnCount() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridColumnCount();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}
*/




/*! \brief The method returns the number of rows used in the original grid.

     \return   The number of the grid rows.
*/
/*
std::size_t VirtualMessage::getGridRowCount() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return mMessageList[0]->getGridRowCount();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}
*/




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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief The method returns the grid definition string (=> Projection name).

        \return   The projection used in the current grid (LatLon, Mercator, etc.)
*/
/*
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}
*/





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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void VirtualMessage::getGridValueByPoint(T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();

    std::vector<double> outParameters;

    switch (mFunctionCallMethod)
    {
      case 9:
      {
        if (mMessageList.size() < 1)
          throw Fmi::Exception(BCP,"Invalid number of parameters!");

        std::vector<std::vector<double>> inParameters;

        uint sz = mMessageList.size();
        for (uint t=0; t<sz; t++)
        {
          T::ParamValue val = 0;
          std::vector<double> inValues;
          mMessageList[t]->getGridValueByPoint(coordinateType,x,y,interpolationMethod,val);
          inValues.emplace_back(val);
          inParameters.emplace_back(inValues);
        }
        executeFunctionCall9(1,1,inParameters,outParameters);
        break;
      }
    }

    if (outParameters.size() >= 1)
      value = outParameters[0];
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
      case 9:
      {
        if (mMessageList.size() < 1)
          throw Fmi::Exception(BCP,"Invalid number of parameters!");

        std::vector<T::GridValueList> inValueList;

        uint sz = mMessageList.size();
        for (uint t=0; t<sz; t++)
        {
          T::GridValueList valList;
          mMessageList[t]->getGridValueListByCircle(coordinateType,origoX,origoY,radius,valList);

          inValueList.emplace_back(valList);
        }
        executeFunctionCall9(inValueList,valueList);
      }
      break;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
      case 9:
      {
        if (mMessageList.size() < 1)
          throw Fmi::Exception(BCP,"Invalid number of parameters!");

        std::vector<T::GridValueList> inValueList;

        uint sz = mMessageList.size();
        for (uint t=0; t<sz; t++)
        {
          T::GridValueList valList;
          mMessageList[t]->getGridValueListByPointList(coordinateType,pointList,interpolationMethod,valList);

          inValueList.emplace_back(valList);
        }
        executeFunctionCall9(inValueList,valueList);
      }
      break;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
      case 9:
      {
        if (mMessageList.size() < 1)
          throw Fmi::Exception(BCP,"Invalid number of parameters!");

        std::vector<T::GridValueList> inValueList;

        uint sz = mMessageList.size();
        for (uint t=0; t<sz; t++)
        {
          T::GridValueList valList;
          mMessageList[t]->getGridValueListByPolygon(coordinateType,polygonPoints,valList);

          inValueList.emplace_back(valList);
        }
        executeFunctionCall9(inValueList,valueList);
      }
      break;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
      case 9:
      {
        if (mMessageList.size() < 1)
          throw Fmi::Exception(BCP,"Invalid number of parameters!");

        std::vector<T::GridValueList> inValueList;

        uint sz = mMessageList.size();
        for (uint t=0; t<sz; t++)
        {
          T::GridValueList valList;
          mMessageList[t]->getGridValueListByPolygonPath(coordinateType,polygonPath,valList);

          inValueList.emplace_back(valList);
        }
        executeFunctionCall9(inValueList,valueList);
      }
      break;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
      case 9:
      {
        if (mMessageList.size() < 1)
          throw Fmi::Exception(BCP,"Invalid number of parameters!");

        std::vector<T::GridValueList> inValueList;

        uint sz = mMessageList.size();
        for (uint t=0; t<sz; t++)
        {
          T::GridValueList valList;
          mMessageList[t]->getGridValueListByRectangle(coordinateType,x1,y1,x2,y2,gridRectangle,valList);

          inValueList.emplace_back(valList);
        }
        executeFunctionCall9(inValueList,valueList);
      }
      break;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    uint columns = getGridColumnCount();
    uint rows = getGridRowCount();

    switch (mFunctionCallMethod)
    {
      case 9:
      {
        if (mMessageList.size() < 1)
          throw Fmi::Exception(BCP,"Invalid number of parameters!");

        std::vector<std::vector<float>> valueList;
        uint sz = mMessageList.size();
        for (uint t=0; t<sz; t++)
        {
          T::ParamValue_vec val;
          mMessageList[t]->getGridValueVector(val);
          valueList.emplace_back(val);
        }
        executeFunctionCall9(columns,rows,valueList,values);
      }
      break;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    uint columns = getGridColumnCount();
    uint rows = getGridRowCount();

    switch (mFunctionCallMethod)
    {
      case 9:
      {
        if (mMessageList.size() < 1)
          throw Fmi::Exception(BCP,"Invalid number of parameters!");

        std::vector<std::vector<float>> valueList;
        uint sz = mMessageList.size();
        for (uint t=0; t<sz; t++)
        {
          T::ParamValue_vec val;
          mMessageList[t]->getGridOriginalValueVector(val);
          valueList.emplace_back(val);
        }
        executeFunctionCall9(columns,rows,valueList,values);
      }
      break;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





T::ParamLevel VirtualMessage::getGridParameterLevel() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return toInt32(mParamDef.mLevel);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





T::ParamLevelId VirtualMessage::getGridParameterLevelId() const
{
  FUNCTION_TRACE
  try
  {
    initMessagePtrs();
    return toUInt8(mParamDef.mLevelId);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}
*/




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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
      case 9:
      {
        if (mMessageList.size() < 1)
          throw Fmi::Exception(BCP,"Invalid number of parameters!");

        std::vector<std::vector<T::ParamValue>> inParameters;

        uint sz = mMessageList.size();
        for (uint t=0; t<sz; t++)
        {
          T::ParamValue_vec inValues;
          inValues.emplace_back(mMessageList[t]->getGridValueByGridPoint(grid_i,grid_j));
          inParameters.emplace_back(inValues);
        }
        executeFunctionCall9(1,1,inParameters,values);
      }
      break;

    }
    if (values.size() == 1)
      return values[0];

    return ParamValueMissing;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}
*/




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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void VirtualMessage::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,std::vector<float>& outParameters) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() < 1)
    {
      Fmi::Exception exception(BCP,"Invalid number of source grids!");
      exception.addParameter("NumberOfGrids",Fmi::to_string(mMessageList.size()));
      throw exception;
    }

    std::string functionName = mFunctionName;

    // Trying to find C++ implementation of the function
    auto function = mFunctionCollection->getFunction(functionName);
    if (function != nullptr)
    {
      function->executeFunctionCall9(columns,rows,inParameters,mFunctionParameters,outParameters);
      return;
    }

    // Calling the LUA implementation of the function

    mLuaFileCollection->executeFunctionCall9(functionName,columns,rows,inParameters,mFunctionParameters,outParameters);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void VirtualMessage::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,std::vector<double>& outParameters) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() < 1)
    {
      Fmi::Exception exception(BCP,"Invalid number of source grids!");
      exception.addParameter("NumberOfGrids",Fmi::to_string(mMessageList.size()));
      throw exception;
    }

    std::string functionName = mFunctionName;

    // Trying to find C++ implementation of the function
    auto function = mFunctionCollection->getFunction(functionName);
    if (function != nullptr)
    {
      function->executeFunctionCall9(columns,rows,inParameters,mFunctionParameters,outParameters);
      return;
    }

    // Calling the LUA implementation of the function

    mLuaFileCollection->executeFunctionCall9(functionName,columns,rows,inParameters,mFunctionParameters,outParameters);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void VirtualMessage::executeFunctionCall9(std::vector<T::GridValueList>& inValueList,T::GridValueList& outValueList) const
{
  FUNCTION_TRACE
  try
  {
    if (mMessageList.size() < 1)
    {
      Fmi::Exception exception(BCP,"Invalid number of source grids!");
      exception.addParameter("NumberOfGrids",Fmi::to_string(mMessageList.size()));
      throw exception;
    }

    std::vector<std::vector<double>> inParameters;
    std::vector<double> outParameters;

    uint sz = inValueList.size();
    for (uint t=0; t<sz; t++)
    {
      uint len = inValueList[t].getLength();
      std::vector<double> params;

      for (uint i=0; i<len; i++)
      {
        T::GridValue value;
        inValueList[t].getGridValueByIndex(i,value);
        params.emplace_back(value.mValue);
      }
      inParameters.emplace_back(params);
    }

    executeFunctionCall9(inValueList[0].getLength(),1,inParameters,outParameters);

    uint len = outParameters.size();
    for (uint t=0; t<len; t++)
    {
      T::GridValue value;
      inValueList[0].getGridValueByIndex(t,value);
      T::GridValue newValue(value);
      newValue.mValue = outParameters[t];
      outValueList.addGridValue(newValue);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



}  // namespace GRID
}  // namespace SmartMet

