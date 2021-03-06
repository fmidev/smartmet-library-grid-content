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

    ::CORBA::Long executeQuery(::CORBA::LongLong sessionId, SmartMet::QueryServer::Corba::CorbaQuery& query);
    ::CORBA::Long getProducerList(::CORBA::LongLong sessionId, SmartMet::QueryServer::Corba::CorbaStringList_out producerList);
    ::CORBA::Long getValuesByGridPoint(::CORBA::LongLong sessionId, const SmartMet::ContentServer::Corba::CorbaContentInfoList& contentInfoList, ::CORBA::Octet coordinateType, ::CORBA::Double x, ::CORBA::Double y, ::CORBA::Short interpolationMethod, SmartMet::QueryServer::Corba::CorbaGridPointValueList_out valueList);

  protected:

    QueryServer::ServiceInterface *mService;
};

}
}
}

