#pragma once

#include "../../definition/ServiceInterface.h"
#include "../stubs/QueryServer_serviceInterface_SK.h"


// This file contains the actual implementation of the CORBA server interface. Every time
// you modify the IDL file, a new skeleton will be generated into the "stubs" directory.
// Copy the new service interface (or its changes) from the "*_i.cc" file into this file and
// write the actual implementation. The point is that we do not use "*_i.cc" file in our build,
// because it is overwritten every time when the IDL file is changes.


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

    SmartMet::C::Result executeQuery(SmartMet::C::SessionId sessionId, SmartMet::C::Query& query);
    SmartMet::C::Result getProducerList(SmartMet::C::SessionId sessionId, SmartMet::C::StringList_out producerList);
    SmartMet::C::Result getValuesByGridPoint(SmartMet::C::SessionId sessionId, const SmartMet::C::ContentInfoList& contentInfoList, SmartMet::C::UInt8 coordinateType, SmartMet::C::Float64 x, SmartMet::C::Float64 y, SmartMet::C::Int16 interpolationMethod, SmartMet::C::GridPointValueList_out valueList);

  protected:

    QueryServer::ServiceInterface *mService;
};

}
}
}

