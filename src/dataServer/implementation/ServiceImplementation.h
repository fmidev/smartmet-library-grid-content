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


// ====================================================================================
/*! \brief Index record used during start-up file-cache pre-population.
 *
 *  Associates a file position and message size with the producer/generation/
 *  file/message identifiers so that the cache can be seeded without a full
 *  ContentServer scan. */
// ====================================================================================

struct StartUpCacheIndexRec
{
  T::ProducerId producerId;     //!< Producer identifier for this cached message.
  T::GenerationId generationId; //!< Generation (model run) identifier.
  T::FileId fileId;             //!< File identifier in the ContentServer.
  T::MessageIndex messageIndex; //!< Message index within the file.
  T::FilePosition filePosition; //!< Byte offset of the message within the file.
  uint messageSize;             //!< Size of the message in bytes.
};


typedef std::map<UInt64,StartUpCacheIndexRec> StartUpIndexMap;  //!< Map from composite key to start-up cache index record.


// ====================================================================================
/*! \brief Concrete DataServer implementation that reads grid files from disk.
 *
 *  Implements all ServiceInterface data-retrieval methods by locating grid files
 *  through the ContentServer, memory-mapping them via GridFileManager, and
 *  delegating interpolation to the grid-files library.  A background thread
 *  processes ContentServer change events to keep the open-file set in sync;
 *  an optional second thread pre-populates a local file cache. */
// ====================================================================================

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
     virtual void   cacheFiles(std::map<T::FileId,std::string>& filenames);
     virtual void   setCleanup(time_t age,time_t checkInterval);
     virtual void   setDem(std::shared_ptr<Fmi::DEM> dem);
     virtual void   setLandCover(std::shared_ptr<Fmi::LandCover> landCover);
     virtual void   getStateAttributes(std::shared_ptr<T::AttributeNode> parent);

     virtual void   eventProcessingThread();
     virtual void   cacheProcessingThread();



  protected:

     virtual int    _getGridAttributeList(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::AttributeList& attributeList);
     virtual int    _getGridProperties(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::PropertySettingVec& propertyList);
     virtual int    _getGridCoordinates(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::CoordinateType coordinateType,T::GridCoordinates& coordinates);
     virtual int    _getGridLatlonCoordinatesByGeometry(T::SessionId sessionId,T::AttributeList& attributeList,T::GridCoordinates& coordinates);
     virtual int    _getGridData(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::GridData& data);
     virtual int    _getGridFileCount(T::SessionId sessionId,uint& count);
     virtual int    _getGridMessageBytes(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,std::vector<uchar>& messageBytes,std::vector<uint>& messageSections);

     virtual int    _getPropertyValuesByCoordinates(T::SessionId sessionId,const char *propertyName,T::Coordinate_vec& latlonCoordinates,T::ParamValue_vec& values);

     virtual int    _getGridValueByPoint(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value);
     virtual int    _getGridValueByLevelAndPoint(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,int level1,T::FileId fileId2,T::MessageIndex messageIndex2,int level2,double newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value);
     virtual int    _getGridValueByTimeAndPoint(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value);
     virtual int    _getGridValueByTimeLevelAndPoint(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,int level1,T::FileId fileId2,T::MessageIndex messageIndex2,int level2,T::FileId fileId3,T::MessageIndex messageIndex3,int level3,T::FileId fileId4,T::MessageIndex messageIndex4,int level4,time_t newTime,double newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value);

     virtual int    _getGridValueListByCircle(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::CoordinateType coordinateType,double origoX,double origoY,double radius,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByPointList(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByPolygon(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByPolygonPath(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByRectangle(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByLevelAndCircle(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,double newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByLevelAndPointList(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByLevelAndPolygon(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByLevelAndPolygonPath(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,double newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByTimeAndCircle(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByTimeAndPointList(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByTimeAndPolygon(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByTimeAndPolygonPath(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByTimeLevelAndCircle(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,T::FileId fileId3,T::MessageIndex messageIndex3,T::FileId fileId4,T::MessageIndex messageIndex4,time_t newTime,double newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByTimeLevelAndPointList(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,T::FileId fileId3,T::MessageIndex messageIndex3,T::FileId fileId4,T::MessageIndex messageIndex4,time_t newTime,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByTimeLevelAndPolygon(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,T::FileId fileId3,T::MessageIndex messageIndex3,T::FileId fileId4,T::MessageIndex messageIndex4,time_t newTime,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);
     virtual int    _getGridValueListByTimeLevelAndPolygonPath(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,T::FileId fileId3,T::MessageIndex messageIndex3,T::FileId fileId4,T::MessageIndex messageIndex4,time_t newTime,double newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList);

     virtual int    _getGridValueVector(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByCoordinateList(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByGeometry(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByRectangle(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByLevel(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,double newLevel,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByLevelAndCoordinateList(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByLevelAndGeometry(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByTime(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByTimeAndCoordinateList(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByTimeAndGeometry(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByTimeAndLevel(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,T::FileId fileId3,T::MessageIndex messageIndex3,T::FileId fileId4,T::MessageIndex messageIndex4,time_t newTime,double newLevel,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByTimeLevelAndGeometry(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,T::FileId fileId3,T::MessageIndex messageIndex3,T::FileId fileId4,T::MessageIndex messageIndex4,time_t newTime,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByTimeLevelAndCoordinateList(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,T::FileId fileId3,T::MessageIndex messageIndex3,T::FileId fileId4,T::MessageIndex messageIndex4,time_t newTime,double newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values);

     virtual int    _getGridIsobands(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByGeometry(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByGrid(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByLevel(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByLevelAndGeometry(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByLevelAndGrid(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByTime(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByTimeAndGeometry(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByTimeAndGrid(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByTimeAndLevel(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,T::FileId fileId3,T::MessageIndex messageIndex3,T::FileId fileId4,T::MessageIndex messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByTimeLevelAndGeometry(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,T::FileId fileId3,T::MessageIndex messageIndex3,T::FileId fileId4,T::MessageIndex messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsobandsByTimeLevelAndGrid(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,T::FileId fileId3,T::MessageIndex messageIndex3,T::FileId fileId4,T::MessageIndex messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);

     virtual int    _getGridIsolines(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByGeometry(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByGrid(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByLevel(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,double newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByLevelAndGeometry(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,double newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByLevelAndGrid(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,double newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByTime(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByTimeAndGeometry(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByTimeAndGrid(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByTimeAndLevel(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,T::FileId fileId3,T::MessageIndex messageIndex3,T::FileId fileId4,T::MessageIndex messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByTimeLevelAndGeometry(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,T::FileId fileId3,T::MessageIndex messageIndex3,T::FileId fileId4,T::MessageIndex messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);
     virtual int    _getGridIsolinesByTimeLevelAndGrid(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,T::FileId fileId3,T::MessageIndex messageIndex3,T::FileId fileId4,T::MessageIndex messageIndex4,time_t newTime,double newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours);

     virtual int    _getGridStreamlines(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByGeometry(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByGrid(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByLevel(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByLevelAndGeometry(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByLevelAndGrid(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,double newLevel,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByTime(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByTimeAndGeometry(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByTimeAndGrid(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,time_t newTime,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByTimeAndLevel(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,T::FileId fileId3,T::MessageIndex messageIndex3,T::FileId fileId4,T::MessageIndex messageIndex4,time_t newTime,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByTimeLevelAndGeometry(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,T::FileId fileId3,T::MessageIndex messageIndex3,T::FileId fileId4,T::MessageIndex messageIndex4,time_t newTime,double newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);
     virtual int    _getGridStreamlinesByTimeLevelAndGrid(T::SessionId sessionId,T::FileId fileId1,T::MessageIndex messageIndex1,T::FileId fileId2,T::MessageIndex messageIndex2,T::FileId fileId3,T::MessageIndex messageIndex3,T::FileId fileId4,T::MessageIndex messageIndex4,time_t newTime,double newLevel,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines);

     virtual int    _getGridValueVectorByPoint(T::SessionId sessionId,T::FileId fileId,T::MessageIndex messageIndex,T::CoordinateType coordinateType,double x,double y,uint vectorType,uint modificationOperation,double_vec& modificationParameters,double_vec& valueVector);

     virtual int    _getMultipleGridValues(T::SessionId sessionId,uint modificationOperation,double_vec& modificationParameters,T::ValueRecordList& valueRecordList);

  protected:

     virtual bool   isSessionValid(T::SessionId sessionId);

     virtual void   event_clear(T::EventInfo& eventInfo);
     virtual void   event_storageClear(T::EventInfo& eventInfo);
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
     virtual void   removeOldCacheFiles(std::map<T::FileId,std::string>& cachedFilenames);

     ServiceInterface*    getDataServerByFileId(T::FileId fileId);
     GRID::GridFile_sptr  getGridFile(T::FileId fileId);

     T::EventId           mLastProcessedEventId;        //!< Event identifier of the last ContentServer event processed.
     bool                 mShutdownRequested;            //!< True once shutdown() has been called.
     bool                 mFullUpdateRequired;           //!< True if a full ContentServer resync is pending.
     bool                 mEventProcessingActive;        //!< True while the event-processing thread is running.
     bool                 mCacheProcessingActive;        //!< True while the cache-processing thread is running.
     T::SessionId         mServerSessionId;              //!< Session identifier used for ContentServer calls.
     uint                 mServerId;                     //!< Numeric identifier of this DataServer instance.
     std::string          mServerName;                   //!< Human-readable name of this server instance.
     std::string          mServerIor;                    //!< CORBA IOR string published by this server (may be empty).
     std::string          mDataDir;                      //!< Filesystem path used for locally stored grid files.
     pthread_t            mEventProcessingThread;        //!< Thread handle for the event-processing background thread.
     pthread_t            mCacheProcessingThread;        //!< Thread handle for the cache-processing background thread.
     time_t               mContentServerStartTime;       //!< Startup timestamp of the ContentServer, used for event replay detection.
     GridFileManager      mGridFileManager;              //!< Registry of open memory-mapped grid files.
     std::vector<uint>    mFileAdditionList;             //!< Pending file-addition queue between event and main threads.
     ThreadLock           mThreadLock;                   //!< Lock protecting mFileAdditionList.
     time_t               mContentChangeTime;            //!< Timestamp of the most recent ContentServer change event.
     std::string          mFileCache_directory;          //!< Directory path for the optional local file cache.
     bool                 mFileCache_enabled;            //!< True if the local file cache is enabled.
     uint                 mFileCache_fileCount;          //!< Number of files currently held in the local cache.
     time_t               mStartTime;                    //!< Wall-clock time when init() was called.


     ContentServer::ServiceInterface*   mContentServer;            //!< Non-owning pointer to the ContentServer used for metadata queries.
     time_t                             mFileCleanup_age;          //!< Minimum age (seconds) before an unused open file is closed.
     time_t                             mFileCleanup_checkInterval; //!< How often (seconds) to check for files to close.
     time_t                             mFileCleanup_time;         //!< Timestamp of the next scheduled cleanup pass.
     time_t                             mDeletedFileCleanup_time;  //!< Timestamp of the next deleted-file cleanup pass.
     std::shared_ptr<Fmi::DEM>          mDem;                      //!< Optional digital elevation model used for DEM-based queries.
     std::shared_ptr<Fmi::LandCover>    mLandCover;                //!< Optional land-cover dataset used for land-cover-based queries.
};


}
}

