#pragma once

#include "../stubs/QueryServer_serviceInterface_SK.h"
#include "../../definition/Query.h"
#include "../../definition/Query.h"

#include <grid-files/grid/GridPointValue.h>
#include <grid-files/grid/GridPointValueList.h>
#include <grid-files/grid/GridValueList.h>
#include <grid-files/grid/Typedefs.h>



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

    static void   convert(T::GeometryId_set& source,C::GeometryIdList& target);
    static void   convert(const C::GeometryIdList& source,T::GeometryId_set& target);

    static void   convert(std::set<time_t>& source,C::TimestampList& target);
    static void   convert(const C::TimestampList& source,std::set<time_t>& target);

    static void   convert(std::vector<uint>& source,C::UInt32List& target);
    static void   convert(const C::UInt32List& source,std::vector<uint>& target);

    static void   convert(std::vector<int>& source,C::LevelList& target);
    static void   convert(const C::LevelList& source,std::vector<int>& target);

    static void   convert(string_set& source,C::StringList& target);
    static void   convert(C::StringList& source,string_set& target);

    static void   convert(string_vec& source,C::StringList& target);
    static void   convert(C::StringList& source,string_vec& target);

    static void   convert(const T::Attribute& source,C::AttributeRec& target);
    static void   convert(const C::AttributeRec& source,T::Attribute& target);

    static void   convert(const T::AttributeList& source,C::AttributeRecList& target);
    static void   convert(const C::AttributeRecList& source,T::AttributeList& target);

    static void   convert(const T::ParamValue_vec& source,C::ParameterValueList& target);
    static void   convert(const C::ParameterValueList& source,T::ParamValue_vec& target);

    static void   convert(T::GridPointValue& source,C::GridPointValue& target);
    static void   convert(C::GridPointValue& source,T::GridPointValue& target);

    static void   convert(T::GridPointValueList& source,C::GridPointValueList& target);
    static void   convert(C::GridPointValueList& source,T::GridPointValueList& target);

    static void   convert(T::GridValue& source,C::GridValue& target);
    static void   convert(C::GridValue& source,T::GridValue& target);

    static void   convert(C::GridValueList& source,T::GridValueList& target);
    static void   convert(T::GridValueList& source,C::GridValueList& target);

    static void   convert(C::ParameterValues& source,QueryServer::ParameterValues& target);
    static void   convert(QueryServer::ParameterValues& source,C::ParameterValues& target);

    static void   convert(C::ParameterValuesList& source,QueryServer::ParameterValues_sptr_vec& target);
    static void   convert(QueryServer::ParameterValues_sptr_vec& source,C::ParameterValuesList& target);

    static void   convert(C::QueryParameter& source,QueryServer::QueryParameter& target);
    static void   convert(QueryServer::QueryParameter& source,C::QueryParameter& target);

    static void   convert(C::QueryParameterList& source,QueryServer::QueryParameter_vec& target);
    static void   convert(QueryServer::QueryParameter_vec& source,C::QueryParameterList& target);

    static void   convert(T::Coordinate& source,C::Coordinate& target);
    static void   convert(C::Coordinate& source,T::Coordinate& target);

    static void   convert(C::CoordinateList& source,T::Coordinate_vec& target);
    static void   convert(T::Coordinate_vec& source,C::CoordinateList& target);

    static void   convert(C::AreaCoordinates& source,T::AreaCoordinates& target);
    static void   convert(T::AreaCoordinates& source,C::AreaCoordinates& target);

    static void   convert(C::Query& source,QueryServer::Query& target);
    static void   convert(QueryServer::Query& source,C::Query& target);

    static void   convert(C::FunctionParam& source, SmartMet::QueryServer::FunctionParam& target);
    static void   convert(SmartMet::QueryServer::FunctionParam& source,C::FunctionParam& target);

    static void   convert(C::FunctionParamList& source, SmartMet::QueryServer::FunctionParam_vec& target);
    static void   convert(SmartMet::QueryServer::FunctionParam_vec& source,C::FunctionParamList& target);

    static void   convert(const SmartMet::T::ByteData& source, C::ByteData& target);
    static void   convert(const C::ByteData& source,SmartMet::T::ByteData& target);

    static void   convert(const SmartMet::T::ByteData_vec& source, C::ByteDataSequence& target);
    static void   convert(const C::ByteDataSequence& source,SmartMet::T::ByteData_vec& target);
};



}
}
}

