#pragma once

#include "dataServer/definition/ServiceInterface.h"
#include "DataServer_serviceInterface_SK.h"


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
    ::CORBA::Long getGridValueByPoint(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Octet interpolationMethod, SmartMet::DataServer::Corba::CorbaParamValue& value);
    ::CORBA::Long getGridValueListByCircle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, ::CORBA::Double origoX, ::CORBA::Double origoY, ::CORBA::Double radius, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByPointList(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& pointList, ::CORBA::Octet interpolationMethod, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByPolygon(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaCoordinateList& polygonPoints, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByPolygonPath(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, const SmartMet::DataServer::Corba::CorbaPolygonPath& polygonPath, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueListByRectangle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, ::CORBA::Double x1, ::CORBA::Double y1, ::CORBA::Double x2, ::CORBA::Double y2, SmartMet::DataServer::Corba::CorbaGridValueList_out valueList);
    ::CORBA::Long getGridValueVector(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, SmartMet::DataServer::Corba::CorbaParamValueList_out values);
    ::CORBA::Long getGridValueVectorByRectangle(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, ::CORBA::ULong columns, ::CORBA::ULong rows, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Double xStep, ::CORBA::Double yStep, ::CORBA::Octet interpolationMethod, SmartMet::DataServer::Corba::CorbaParamValueList_out values);

    ::CORBA::Long getMultipleGridValues(::CORBA::LongLong sessionId, SmartMet::DataServer::Corba::CorbaValueRecordList& valueRecordList);

  protected:

    DataServer::ServiceInterface *mService;
};

}
}
}

