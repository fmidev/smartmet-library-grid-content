#pragma once

#include "dataServer/definition/ServiceInterface.h"
#include "DataServer_serviceInterface_SK.h"


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

    ::CORBA::Long getGridAttributeList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, SmartMet::DataServer::Corba::CorbaAttributeList_out attributeList);
    ::CORBA::Long getGridCoordinates(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, SmartMet::DataServer::Corba::CorbaGridCoordinates_out coordinates);
    ::CORBA::Long getGridData(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, SmartMet::DataServer::Corba::CorbaGridData_out data);
    ::CORBA::Long getGridFileCount(::CORBA::LongLong sessionId, ::CORBA::ULong& count);
    ::CORBA::Long getGridValueByPoint(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Octet interpolationMethod, SmartMet::DataServer::Corba::CorbaParamValue& value);
    ::CORBA::Long getGridValueListByCircle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, ::CORBA::Double origoX, ::CORBA::Double origoY, ::CORBA::Double radius, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByPointList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& pointList, ::CORBA::Octet interpolationMethod, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByPolygon(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& polygonPoints, ::CORBA::Double radius, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByPolygonPath(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaPolygonPath& polygonPath, ::CORBA::Double radius, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByRectangle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, ::CORBA::Double x1, ::CORBA::Double y1, ::CORBA::Double x2, ::CORBA::Double y2, ::CORBA::Double radius, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueVector(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, SmartMet::DataServer::Corba::CorbaParamValueList_out values);
    ::CORBA::Long getGridValueVectorByRectangle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::ULong flags, ::CORBA::Octet coordinateType, ::CORBA::ULong columns, ::CORBA::ULong rows, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Double xStep, ::CORBA::Double yStep, ::CORBA::Octet interpolationMethod, SmartMet::DataServer::Corba::CorbaParamValueList_out values);

    ::CORBA::Long getMultipleGridValues(::CORBA::LongLong sessionId, SmartMet::DataServer::Corba::CorbaValueRecordList& valueRecordList);

  protected:

    DataServer::ServiceInterface *mService;
};

}
}
}

