#pragma once

#include "grid-files/grid/GridPointValue.h"
#include "grid-files/grid/GridPointValueList.h"
#include "grid-files/grid/GridValueList.h"

#include "queryServer/corba/stubs/QueryServer_serviceInterface_SK.h"
#include "queryServer/definition/Query.h"
#include "queryServer/definition/Query.h"



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

    static void   convert(std::vector<std::string>& source,QueryServer::Corba::CorbaStringList& target);
    static void   convert(QueryServer::Corba::CorbaStringList& source,std::vector<std::string>& target);

    static void   convert(T::GridPointValue& source,QueryServer::Corba::CorbaGridPointValue& target);
    static void   convert(QueryServer::Corba::CorbaGridPointValue& source,T::GridPointValue& target);

    static void   convert(T::GridPointValueList& source,QueryServer::Corba::CorbaGridPointValueList& target);
    static void   convert(QueryServer::Corba::CorbaGridPointValueList& source,T::GridPointValueList& target);

    static void   convert(T::GridValue& source,QueryServer::Corba::CorbaGridValue& target);
    static void   convert(QueryServer::Corba::CorbaGridValue& source,T::GridValue& target);

    static void   convert(QueryServer::Corba::CorbaGridValueList& source,T::GridValueList& target);
    static void   convert(T::GridValueList& source,QueryServer::Corba::CorbaGridValueList& target);

    static void   convert(QueryServer::Corba::CorbaParameterValues& source,QueryServer::ParameterValues& target);
    static void   convert(QueryServer::ParameterValues& source,QueryServer::Corba::CorbaParameterValues& target);

    static void   convert(QueryServer::Corba::CorbaParameterValuesList& source,QueryServer::ParameterValues_vec& target);
    static void   convert(QueryServer::ParameterValues_vec& source,QueryServer::Corba::CorbaParameterValuesList& target);

    static void   convert(QueryServer::Corba::CorbaQueryParameter& source,QueryServer::QueryParameter& target);
    static void   convert(QueryServer::QueryParameter& source,QueryServer::Corba::CorbaQueryParameter& target);

    static void   convert(QueryServer::Corba::CorbaQueryParameterList& source,QueryServer::QueryParameter_vec& target);
    static void   convert(QueryServer::QueryParameter_vec& source,QueryServer::Corba::CorbaQueryParameterList& target);

    static void   convert(T::Coordinate& source,QueryServer::Corba::CorbaCoordinate& target);
    static void   convert(QueryServer::Corba::CorbaCoordinate& source,T::Coordinate& target);

    static void   convert(QueryServer::Corba::CorbaCoordinateList& source,QueryServer::Coordinate_vec& target);
    static void   convert(QueryServer::Coordinate_vec& source,QueryServer::Corba::CorbaCoordinateList& target);

    static void   convert(QueryServer::Corba::CorbaQueryCoordinates& source,QueryServer::QueryCoordinates& target);
    static void   convert(QueryServer::QueryCoordinates& source,QueryServer::Corba::CorbaQueryCoordinates& target);

    static void   convert(QueryServer::Corba::CorbaQuery& source,QueryServer::Query& target);
    static void   convert(QueryServer::Query& source,QueryServer::Corba::CorbaQuery& target);

    static void   convert(SmartMet::QueryServer::Corba::CorbaFunctionParam& source, SmartMet::QueryServer::FunctionParam& target);
    static void   convert(SmartMet::QueryServer::FunctionParam& source,SmartMet::QueryServer::Corba::CorbaFunctionParam& target);

    static void   convert(SmartMet::QueryServer::Corba::CorbaFunctionParamList& source, SmartMet::QueryServer::FunctionParam_vec& target);
    static void   convert(SmartMet::QueryServer::FunctionParam_vec& source,SmartMet::QueryServer::Corba::CorbaFunctionParamList& target);

};



}
}
}

