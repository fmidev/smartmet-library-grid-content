#pragma once

#include "GridFileManager.h"
#include "../definition/ServiceInterface.h"
#include "../../lua/LuaFileCollection.h"

#include <grid-files/common/AttributeList.h>
#include <gis/DEM.h>
#include <gis/LandCover.h>
#include <pthread.h>


namespace SmartMet
{
namespace DataServer
{


struct StartUpCacheIndexRec
{
  uint producerId;
  uint generationId;
  uint fileId;
  uint messageIndex;
  unsigned long long filePosition;
  uint messageSize;
};


typedef std::map<unsigned long long,StartUpCacheIndexRec> StartUpIndexMap;


class ServiceImplementation : public ServiceInterface
{
  public:
                    ServiceImplementation();
     virtual        ~ServiceImplementation();

     virtual void   init(
                       T::SessionId serverSessionId,
                       uint serverId,
                       const std::string& serverName,
                       const std::string& serverIor,
                       const std::string& dataDir,
                       ContentServer::ServiceInterface *contentServer);

     virtual void   startEventProcessing();
     virtual void   startCacheProcessing();
     virtual void   shutdown();
     virtual void   setFileCache(bool enabled,const char *directory);
     virtual void   cacheFiles(std::map<uint,std::string>& filenames);
     virtual void   setCleanup(time_t age,time_t checkInterval);
     virtual void   setDem(std::shared_ptr<Fmi::DEM> dem);
     virtual void   setLandCover(std::shared_ptr<Fmi::LandCover> landCover);
     virtual void   getStateAttributes(std::shared_ptr<T::AttributeNode> parent);

     virtual void   eventProcessingThread();
     virtual void   cacheProcessingThread();



  protected:

     virtual int    _getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList);
     virtual int    _getGridProperties(T::SessionId sessionId,uint fileId,uint messageIndex,T::PropertySettingVec& propertyList);
     virtual int    _getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,T::GridCoordinates& coordinates);
     virtual int    _getGridLatlonCoordinatesByGeometry(T::SessionId sessionId,T::AttributeList& attributeList,T::GridCoordinates& coordinates);
     virtual int    _getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data);
     virtual int    _getGridFileCount(T::SessionId sessionId,uint& count);
     virtual int    _getGridMessageBytes(T::SessionId sessionId,uint fileId,uint messageIndex,std::vector<uchar>& messageBytes,std::vector<uint>& messageSections);

     virtual int    _getPropertyValuesByCoordinates(T::SessionId sessionId,const char *propertyName,T::Coordinate_vec& latlonCoordinates,T::ParamValue_vec& values);

     virtual int    _getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value);
     virtual int    _getGridValueByLevelAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,int level1,uint fileId2,uint messageIndex2,int level2,double newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value);
     virtual int    _getGridValueByTimeAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value);
     virtual int    _getGridValueByTimeLevelAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,int level1,uint fileId2,uint messageIndex2,int level2,uint fileId3,uint messageIndex3,int level3,uint fileId4,uint messageIndex4,int level4,time_t newTime,double newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value);

     virtual int    _getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double origoX,double origoY,double radius,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByLevelAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByLevelAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByLevelAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByLevelAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByTimeAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByTimeAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByTimeAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByTimeAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByTimeLevelAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByTimeLevelAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByTimeLevelAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByTimeLevelAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);

     virtual int    _getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByCoordinateList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByLevelAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByTimeAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByTimeLevelAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);

     virtual int    _getGridIsobands(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);

     virtual int    _getGridIsolines(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);

     virtual int    _getGridStreamlines(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,double newLevel,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,double newLevel,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);

     virtual int    _getGridValueVectorByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,uint vectorType,uint modificationOperation,double_vec& modificationParameters,double_vec& valueVector);

     virtual int    _getMultipleGridValues(T::SessionId sessionId,uint modificationOperation,double_vec& modificationParameters,T::ValueRecordList& valueRecordList);

  protected:

     virtual bool   isSessionValid(T::SessionId sessionId);

     virtual void   event_clear(T::EventInfo& eventInfo);
     virtual void   event_contentServerReload(T::EventInfo& eventInfo);
     virtual void   event_producerAdded(T::EventInfo& eventInfo);
     virtual void   event_producerDeleted(T::EventInfo& eventInfo);
     virtual void   event_producerUpdated(T::EventInfo& eventInfo);
     virtual void   event_producerListDeletedBySourceId(T::EventInfo& eventInfo);
     virtual void   event_generationAdded(T::EventInfo& eventInfo);
     virtual void   event_generationDeleted(T::EventInfo& eventInfo);
     virtual void   event_generationUpdated(T::EventInfo& eventInfo);
     virtual void   event_generationStatusChanged(T::EventInfo& eventInfo);
     virtual void   event_generationListDeletedByProducerId(T::EventInfo& eventInfo);
     virtual void   event_generationListDeletedBySourceId(T::EventInfo& eventInfo);
     virtual void   event_fileAdded(T::EventInfo& eventInfo,T::EventInfo *nextEventInfo);
     virtual void   event_fileDeleted(T::EventInfo& eventInfo);
     virtual void   event_fileUpdated(T::EventInfo& eventInfo);
     virtual void   event_fileListDeletedByProducerId(T::EventInfo& eventInfo);
     virtual void   event_fileListDeletedByGenerationId(T::EventInfo& eventInfo);
     virtual void   event_fileListDeletedBySourceId(T::EventInfo& eventInfo);
     virtual void   event_contentListDeletedByFileId(T::EventInfo& eventInfo);
     virtual void   event_contentListDeletedByProducerId(T::EventInfo& eventInfo);
     virtual void   event_contentListDeletedByGenerationId(T::EventInfo& eventInfo);
     virtual void   event_contentListDeletedBySourceId(T::EventInfo& eventInfo);
     virtual void   event_contentAdded(T::EventInfo& eventInfo);
     virtual void   event_contentDeleted(T::EventInfo& eventInfo);
     virtual void   event_contentUpdated(T::EventInfo& eventInfo);

     virtual void   addFile(T::FileInfo& fileInfo,T::ContentInfoList& currentContentList);
     virtual void   fullUpdate();
     virtual void   processEvent(T::EventInfo& eventInfo,T::EventInfo *nextEventInfo);
     virtual void   processEvents();
     virtual void   readContentList(T::ContentInfoList& contentList);
     virtual void   removeOldCacheFiles(std::map<uint,std::string>& cachedFilenames);

     ServiceInterface*    getDataServerByFileId(uint fileId);
     GRID::GridFile_sptr  getGridFile(uint fileId);

     T::EventId           mLastProcessedEventId;
     bool                 mShutdownRequested;
     bool                 mFullUpdateRequired;
     bool                 mEventProcessingActive;
     bool                 mCacheProcessingActive;
     T::SessionId         mServerSessionId;
     uint                 mServerId;
     std::string          mServerName;
     std::string          mServerIor;
     std::string          mDataDir;
     pthread_t            mEventProcessingThread;
     pthread_t            mCacheProcessingThread;
     time_t               mContentServerStartTime;
     GridFileManager      mGridFileManager;
     std::vector<uint>    mFileAdditionList;
     ThreadLock           mThreadLock;
     time_t               mContentChangeTime;
     std::string          mFileCache_directory;
     bool                 mFileCache_enabled;
     uint                 mFileCache_fileCount;
     time_t               mStartTime;


     ContentServer::ServiceInterface*   mContentServer;
     time_t                             mFileCleanup_age;
     time_t                             mFileCleanup_checkInterval;
     time_t                             mFileCleanup_time;
     time_t                             mDeletedFileCleanup_time;
     std::shared_ptr<Fmi::DEM>          mDem;
     std::shared_ptr<Fmi::LandCover>    mLandCover;
};


}
}

