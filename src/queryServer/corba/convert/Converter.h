#pragma once

#include "../stubs/QueryServer_serviceInterface_SK.h"
#include "../../definition/Query.h"
#include "../../definition/Query.h"

#include <grid-files/grid/GridPointValue.h>
#include <grid-files/grid/GridPointValueList.h>
#include <grid-files/grid/GridValueList.h>



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

    static void   convert(string_set& source,QueryServer::Corba::CorbaStringList& target);
    static void   convert(QueryServer::Corba::CorbaStringList& source,string_set& target);

    static void   convert(string_vec& source,QueryServer::Corba::CorbaStringList& target);
    static void   convert(QueryServer::Corba::CorbaStringList& source,string_vec& target);

    static void   convert(const T::Attribute& source,QueryServer::Corba::CorbaAttribute& target);
    static void   convert(const QueryServer::Corba::CorbaAttribute& source,T::Attribute& target);

    static void   convert(const T::AttributeList& source,QueryServer::Corba::CorbaAttributeList& target);
    static void   convert(const QueryServer::Corba::CorbaAttributeList& source,T::AttributeList& target);

    static void   convert(const T::ParamValue_vec& source,QueryServer::Corba::CorbaParamValueList& target);
    static void   convert(const QueryServer::Corba::CorbaParamValueList& source,T::ParamValue_vec& target);

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

    static void   convert(QueryServer::Corba::CorbaCoordinateList& source,T::Coordinate_vec& target);
    static void   convert(T::Coordinate_vec& source,QueryServer::Corba::CorbaCoordinateList& target);

    static void   convert(QueryServer::Corba::CorbaAreaCoordinates& source,T::AreaCoordinates& target);
    static void   convert(T::AreaCoordinates& source,QueryServer::Corba::CorbaAreaCoordinates& target);

    static void   convert(QueryServer::Corba::CorbaQuery& source,QueryServer::Query& target);
    static void   convert(QueryServer::Query& source,QueryServer::Corba::CorbaQuery& target);

    static void   convert(SmartMet::QueryServer::Corba::CorbaFunctionParam& source, SmartMet::QueryServer::FunctionParam& target);
    static void   convert(SmartMet::QueryServer::FunctionParam& source,SmartMet::QueryServer::Corba::CorbaFunctionParam& target);

    static void   convert(SmartMet::QueryServer::Corba::CorbaFunctionParamList& source, SmartMet::QueryServer::FunctionParam_vec& target);
    static void   convert(SmartMet::QueryServer::FunctionParam_vec& source,SmartMet::QueryServer::Corba::CorbaFunctionParamList& target);

    static void   convert(const SmartMet::T::WkbData& source, SmartMet::QueryServer::Corba::CorbaWkbData& target);
    static void   convert(const SmartMet::QueryServer::Corba::CorbaWkbData& source,SmartMet::T::WkbData& target);

    static void   convert(const SmartMet::T::WkbData_vec& source, SmartMet::QueryServer::Corba::CorbaWkbDataSequence& target);
    static void   convert(const SmartMet::QueryServer::Corba::CorbaWkbDataSequence& source,SmartMet::T::WkbData_vec& target);
};



}
}
}

