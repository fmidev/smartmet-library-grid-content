#pragma once

#include "queryServer/definition/ServiceInterface.h"
#include "QueryServer_serviceInterface_SK.h"


namespace SmartMet
{
namespace QueryServer
{
namespace Corba
{


class ServerInterface : public POA_SmartMet::QueryServer::Corba::ServiceInterface
{
  public:
                  ServerInterface();
    virtual       ~ServerInterface();

    void          init(QueryServer::ServiceInterface *service);

    ::CORBA::Long getValuesByGridPoint(::CORBA::LongLong sessionId, const SmartMet::ContentServer::Corba::CorbaContentInfoList& contentInfoList, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Octet interpolationMethod, SmartMet::QueryServer::Corba::CorbaGridPointValueList_out valueList);

  protected:

    QueryServer::ServiceInterface *mService;
};

}
}
}

