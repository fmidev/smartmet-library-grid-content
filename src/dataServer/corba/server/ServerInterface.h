#pragma once

#include "../../definition/ServiceInterface.h"
#include "../stubs/DataServer_serviceInterface_SK.h"


// This file contains the actual implementation of the CORBA server interface. Every time
// you modify the IDL file, a new skeleton will be generated into the "stubs" directory.
// Copy the new service interface (or its changes) from the "*_i.cc" file into this file and
// write the actual implementation. The point is that we do not use "*_i.cc" file in our build,
// because it is overwritten every time when the IDL file is changes.


namespace SmartMet
{
namespace DataServer
{
namespace Corba
{

class ServerInterface : public POA_SmartMet::DataServer::Corba::ServiceInterface
{
  public:
                  ServerInterface();
    virtual       ~ServerInterface();

    void          init(DataServer::ServiceInterface *service);

    ::CORBA::Long getGridAttributeList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, SmartMet::DataServer::Corba::CorbaAttributeList_out attributeList);
    ::CORBA::Long getGridCoordinates(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, SmartMet::DataServer::Corba::CorbaGridCoordinates_out coordinates);
    ::CORBA::Long getGridData(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, SmartMet::DataServer::Corba::CorbaGridData_out data);
    ::CORBA::Long getGridFileCount(::CORBA::LongLong sessionId, ::CORBA::ULong& count);

    ::CORBA::Long getGridValueByPoint(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Short areaInterpolationMethod, SmartMet::DataServer::Corba::CorbaParamValue& value);
    ::CORBA::Long getGridValueByLevelAndPoint(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::Long level1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long level2, ::CORBA::Long newLevel, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Short areaInterpolationMethod, ::CORBA::Short levelInterpolationMethod, SmartMet::DataServer::Corba::CorbaParamValue& value);
    ::CORBA::Long getGridValueByTimeAndPoint(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, const char* newTime, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Short areaInterpolationMethod, ::CORBA::Short timeInterpolationMethod, SmartMet::DataServer::Corba::CorbaParamValue& value);
    ::CORBA::Long getGridValueByTimeLevelAndPoint(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::Long level1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long level2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::Long level3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, ::CORBA::Long level4, const char* newTime, ::CORBA::Long newLevel, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Short areaInterpolationMethod, ::CORBA::Short timeInterpolationMethod, ::CORBA::Short levelInterpolationMethod, SmartMet::DataServer::Corba::CorbaParamValue& value);

    ::CORBA::Long getGridValueListByCircle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, ::CORBA::Double origoX, ::CORBA::Double origoY, ::CORBA::Double radius, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByPointList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& pointList, ::CORBA::Short areaInterpolationMethod, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByPolygon(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& polygonPoints, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByPolygonPath(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaPolygonPath& polygonPath, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByRectangle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, ::CORBA::Double x1, ::CORBA::Double y1, ::CORBA::Double x2, ::CORBA::Double y2, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByLevelAndPointList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::Long newLevel, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& pointList, ::CORBA::Short areaInterpolationMethod, ::CORBA::Short levelInterpolationMethod, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByTimeAndCircle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, const char* newTime, ::CORBA::Octet coordinateType, ::CORBA::Double origoX, ::CORBA::Double origoY, ::CORBA::Double radius, ::CORBA::Short timeInterpolationMethod, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByTimeAndPointList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, const char* newTime, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& pointList, ::CORBA::Short areaInterpolationMethod, ::CORBA::Short timeInterpolationMethod, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByTimeAndPolygon(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, const char* newTime, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& polygonPoints, ::CORBA::Short timeInterpolationMethod, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByTimeAndPolygonPath(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, const char* newTime, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaPolygonPath& polygonPath, ::CORBA::Short timeInterpolationMethod, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByTimeLevelAndPointList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, ::CORBA::ULong fileId3, ::CORBA::ULong messageIndex3, ::CORBA::ULong fileId4, ::CORBA::ULong messageIndex4, const char* newTime, ::CORBA::Long newLevel, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& pointList, ::CORBA::Short areaInterpolationMethod, ::CORBA::Short timeInterpolationMethod, ::CORBA::Short levelInterpolationMethod, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);

    ::CORBA::Long getGridValueVector(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, SmartMet::DataServer::Corba::CorbaParamValueList_out values);
    ::CORBA::Long getGridValueVectorByCoordinateList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& coordinates, ::CORBA::Short areaInterpolationMethod, SmartMet::DataServer::Corba::CorbaParamValueList_out values);
    ::CORBA::Long getGridValueVectorByGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, SmartMet::DataServer::Corba::CorbaParamValueList_out values);
    ::CORBA::Long getGridValueVectorByRectangle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, ::CORBA::ULong columns, ::CORBA::ULong rows, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Double xStep, ::CORBA::Double yStep, ::CORBA::Short areaInterpolationMethod, SmartMet::DataServer::Corba::CorbaParamValueList_out values);
    ::CORBA::Long getGridValueVectorByPoint(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::ULong vectorType, SmartMet::DataServer::Corba::CorbaDoubleList_out valueVector);
    ::CORBA::Long getGridValueVectorByTime(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, const char* newTime, ::CORBA::Short timeInterpolationMethod, SmartMet::DataServer::Corba::CorbaParamValueList_out values);
    ::CORBA::Long getGridValueVectorByTimeAndCoordinateList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, const char* newTime, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& coordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, SmartMet::DataServer::Corba::CorbaParamValueList_out values);
    ::CORBA::Long getGridValueVectorByTimeAndGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, const char* newTime, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, SmartMet::DataServer::Corba::CorbaParamValueList_out values);

    ::CORBA::Long getGridIsobands(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, SmartMet::DataServer::Corba::CorbaWkbDataSequence_out contours);
    ::CORBA::Long getGridIsobandsByGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, SmartMet::DataServer::Corba::CorbaWkbDataSequence_out contours);
    ::CORBA::Long getGridIsobandsByGrid(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, ::CORBA::ULong gridWidth, ::CORBA::ULong gridHeight, const SmartMet::DataServer::Corba::CorbaCoordinateList& gridLatLonCoordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, SmartMet::DataServer::Corba::CorbaWkbDataSequence_out contours);
    ::CORBA::Long getGridIsobandsByTime(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, const char* newTime, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, SmartMet::DataServer::Corba::CorbaWkbDataSequence_out contours);
    ::CORBA::Long getGridIsobandsByTimeAndGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, const char* newTime, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, SmartMet::DataServer::Corba::CorbaWkbDataSequence_out contours);
    ::CORBA::Long getGridIsobandsByTimeAndGrid(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, const char* newTime, const SmartMet::DataServer::Corba::CorbaParamValueList& contourLowValues, const SmartMet::DataServer::Corba::CorbaParamValueList& contourHighValues, ::CORBA::ULong gridWidth, ::CORBA::ULong gridHeight, const SmartMet::DataServer::Corba::CorbaCoordinateList& gridLatLonCoordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, SmartMet::DataServer::Corba::CorbaWkbDataSequence_out contours);

    ::CORBA::Long getGridIsolines(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, SmartMet::DataServer::Corba::CorbaWkbDataSequence_out contours);
    ::CORBA::Long getGridIsolinesByGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, SmartMet::DataServer::Corba::CorbaWkbDataSequence_out contours);
    ::CORBA::Long getGridIsolinesByGrid(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, ::CORBA::ULong gridWidth, ::CORBA::ULong gridHeight, const SmartMet::DataServer::Corba::CorbaCoordinateList& gridLatLonCoordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, SmartMet::DataServer::Corba::CorbaWkbDataSequence_out contours);
    ::CORBA::Long getGridIsolinesByTime(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, const char* newTime, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, SmartMet::DataServer::Corba::CorbaWkbDataSequence_out contours);
    ::CORBA::Long getGridIsolinesByTimeAndGeometry(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, const char* newTime, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, SmartMet::DataServer::Corba::CorbaWkbDataSequence_out contours);
    ::CORBA::Long getGridIsolinesByTimeAndGrid(::CORBA::LongLong sessionId, ::CORBA::ULong fileId1, ::CORBA::ULong messageIndex1, ::CORBA::ULong fileId2, ::CORBA::ULong messageIndex2, const char* newTime, const SmartMet::DataServer::Corba::CorbaParamValueList& contourValues, ::CORBA::ULong gridWidth, ::CORBA::ULong gridHeight, const SmartMet::DataServer::Corba::CorbaCoordinateList& gridLatLonCoordinates, SmartMet::DataServer::Corba::CorbaAttributeList& attributeList, SmartMet::DataServer::Corba::CorbaWkbDataSequence_out contours);

    ::CORBA::Long getMultipleGridValues(::CORBA::LongLong sessionId, SmartMet::DataServer::Corba::CorbaValueRecordList& valueRecordList);

  protected:

    DataServer::ServiceInterface *mService;
};

}
}
}

