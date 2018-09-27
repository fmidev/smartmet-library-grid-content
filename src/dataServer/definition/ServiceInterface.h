#pragma once

#include "ValueRecordList.h"
#include "GridData.h"
#include "GridCoordinates.h"
#include "ServiceResults.h"
#include "../../contentServer/definition/ServiceInterface.h"

#include <grid-files/grid/Typedefs.h>
#include <grid-files/common/AttributeList.h>
#include <grid-files/common/Log.h>
#include <grid-files/grid/GridValueList.h>


namespace SmartMet
{
namespace DataServer
{



class ServiceInterface
{
  public:
                    ServiceInterface();
     virtual        ~ServiceInterface();

     virtual void   setDebugLog(Log *debugLog);
     virtual void   setProcessingLog(Log *processingLog);
     virtual void   shutdown();

     // ### Services for single grid

     virtual int    getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::AttributeList& attributeList);
     virtual int    getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,T::GridCoordinates& coordinates);
     virtual int    getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::GridData& data);
     virtual int    getGridFileCount(T::SessionId sessionId,uint& count);
     virtual int    getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::ParamValue& value);
     virtual int    getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList);
     virtual int    getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short interpolationMethod,T::GridValueList& valueList);
     virtual int    getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList);
     virtual int    getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,T::GridValueList& valueList);
     virtual int    getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,T::GridValueList& valueList);
     virtual int    getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::ParamValue_vec& values);
     virtual int    getGridValueVectorByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,uint flags,T::ParamValue_vec& values);
     virtual int    getGridValueVectorByCoordinateList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,short interpolationMethod,T::ParamValue_vec& values);
     virtual int    getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,short interpolationMethod,T::ParamValue_vec& values);

     virtual int    getGridValueVectorByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,uint vectorType,double_vec& valueVector);


     // ### Services over multiple grids

     virtual int    getMultipleGridValues(T::SessionId sessionId,T::ValueRecordList& valueRecordList);

  protected:

     virtual int    _getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::AttributeList& attributeList);
     virtual int    _getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,T::GridCoordinates& coordinates);
     virtual int    _getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::GridData& data);
     virtual int    _getGridFileCount(T::SessionId sessionId,uint& count);
     virtual int    _getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::ParamValue& value);
     virtual int    _getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList);
     virtual int    _getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short interpolationMethod,T::GridValueList& valueList);
     virtual int    _getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList);
     virtual int    _getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,T::GridValueList& valueList);
     virtual int    _getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,T::GridValueList& valueList);
     virtual int    _getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,uint flags,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByCoordinateList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,short interpolationMethod,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,short interpolationMethod,T::ParamValue_vec& values);

     virtual int    _getGridValueVectorByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,uint vectorType,double_vec& valueVector);

     virtual int    _getMultipleGridValues(T::SessionId sessionId,T::ValueRecordList& valueRecordList);

     Log            *mDebugLog;
     Log            *mProcessingLog;
};



// Flags for getGridValueListByRectangle -method:

#define GRID_RECTANGLE_FLAG  1     // The request fetches values from the rectangular area in the grid. The missing corners
                                   // are counted after the given corners are mapped to the grid. If the flag is not set
                                   // then the missing corners are directly taken from the opposite corners. For example,
                                   // corners: (lat1,lon1),(lat2,lon2) => area: (lat1,lon1),(lat1,lon2),(lat2,lon2),(lat2,lon1)



}
}

