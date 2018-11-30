#pragma once

#include "../definition/ServiceInterface.h"
#include "PointValueCache.h"


namespace SmartMet
{
namespace DataServer
{



class CacheImplementation : public ServiceInterface
{
  public:
                        CacheImplementation();
     virtual            ~CacheImplementation();

     virtual void       init(ServiceInterface *dataServer);

  protected:

     virtual int        _getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::AttributeList& attributeList);
     virtual int        _getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,T::GridCoordinates& coordinates);
     virtual int        _getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::GridData& data);
     virtual int        _getGridFileCount(T::SessionId sessionId,uint& count);
     virtual int        _getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::ParamValue& value);
     virtual int        _getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList);
     virtual int        _getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short interpolationMethod,T::GridValueList& valueList);
     virtual int        _getGridValueListByTimeAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,T::GridValueList& valueList);
     virtual int        _getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList);
     virtual int        _getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,T::GridValueList& valueList);
     virtual int        _getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,T::GridValueList& valueList);
     virtual int        _getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::ParamValue_vec& values);
     virtual int        _getGridValueVectorByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,short timeInterpolationMethod,T::ParamValue_vec& values);
     virtual int        _getGridValueVectorByCoordinateList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,short interpolationMethod,T::ParamValue_vec& values);
     virtual int        _getGridValueVectorByGeometryId(T::SessionId sessionId,uint fileId,uint messageIndex,T::GeometryId geometryId,short interpolationMethod,T::ParamValue_vec& values);
     virtual int        _getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,short interpolationMethod,T::ParamValue_vec& values);

     virtual int        _getGridIsobands(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::WkbData_vec& contours);
     virtual int        _getGridIsobandsByBox(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,std::string urn,double x1,double y1,double x2,double y2,T::AttributeList& attributeList,T::WkbData_vec& contours);
     virtual int        _getGridIsobandsByGeometryId(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::GeometryId geometryId,T::AttributeList& attributeList,T::WkbData_vec& contours);
     virtual int        _getGridIsobandsByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::WkbData_vec& contours);
     virtual int        _getGridIsobandsByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::WkbData_vec& contours);
     virtual int        _getGridIsobandsByTimeAndBox(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,std::string urn,double x1,double y1,double x2,double y2,T::AttributeList& attributeList,T::WkbData_vec& contours);
     virtual int        _getGridIsobandsByTimeAndGeometryId(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::GeometryId geometryId,T::AttributeList& attributeList,T::WkbData_vec& contours);
     virtual int        _getGridIsobandsByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::WkbData_vec& contours);

     virtual int        _getGridIsolines(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::WkbData_vec& contours);
     virtual int        _getGridIsolinesByBox(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,std::string urn,double x1,double y1,double x2,double y2,T::AttributeList& attributeList,T::WkbData_vec& contours);
     virtual int        _getGridIsolinesByGeometryId(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,T::GeometryId geometryId,T::AttributeList& attributeList,T::WkbData_vec& contours);
     virtual int        _getGridIsolinesByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::WkbData_vec& contours);
     virtual int        _getGridIsolinesByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::WkbData_vec& contours);
     virtual int        _getGridIsolinesByTimeAndBox(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourValues,std::string urn,double x1,double y1,double x2,double y2,T::AttributeList& attributeList,T::WkbData_vec& contours);
     virtual int        _getGridIsolinesByTimeAndGeometryId(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourValues,T::GeometryId geometryId,T::AttributeList& attributeList,T::WkbData_vec& contours);
     virtual int        _getGridIsolinesByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::WkbData_vec& contours);

     virtual int        _getGridValueVectorByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,uint vectorType,double_vec& valueVector);

     virtual int        _getMultipleGridValues(T::SessionId sessionId,T::ValueRecordList& valueRecordList);

     PointValueCache    mPointValueCache;
     T::GridData_ptr*   mDataCache;
     time_t*            mDataCacheAccessTime;
     uint               mDataCacheSize;
     ServiceInterface*  mDataServer;
};


}
}

