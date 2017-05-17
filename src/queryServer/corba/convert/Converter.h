#pragma once

#include "grid-files/grid/GridPointValue.h"
#include "grid-files/grid/GridPointValueList.h"

#include "queryServer/corba/stubs/QueryServer_serviceInterface_SK.h"



namespace SmartMet
{
namespace QueryServer
{
namespace Corba
{



class Converter
{
  public:
                  Converter();
    virtual       ~Converter();

    static void   convert(T::GridPointValue& source,QueryServer::Corba::CorbaGridPointValue& target);
    static void   convert(QueryServer::Corba::CorbaGridPointValue& source,T::GridPointValue& target);

    static void   convert(T::GridPointValueList& source,QueryServer::Corba::CorbaGridPointValueList& target);
    static void   convert(QueryServer::Corba::CorbaGridPointValueList& source,T::GridPointValueList& target);
};



}
}
}

