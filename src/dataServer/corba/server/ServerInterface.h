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
    ::CORBA::Long getGridValue(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Octet interpolationMethod, SmartMet::DataServer::Corba::CorbaParamValue& value);
    ::CORBA::Long getGridValueList(::CORBA::LongLong sessionId, SmartMet::DataServer::Corba::CorbaValueRecordList& valueRecordList);
    ::CORBA::Long getGridValuesByArea(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, ::CORBA::Octet coordinateType, ::CORBA::ULong columns, ::CORBA::ULong rows, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Double xStep, ::CORBA::Double yStep, ::CORBA::Octet interpolationMethod, SmartMet::DataServer::Corba::CorbaParamValueList_out values);

  protected:

    DataServer::ServiceInterface *mService;
};

}
}
}

