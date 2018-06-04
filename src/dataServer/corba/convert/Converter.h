#pragma once

#include "../../definition/GridData.h"
#include "../../definition/GridCoordinates.h"
#include "../../definition/ValueRecordList.h"
#include "../../corba/stubs/DataServer_serviceInterface_SK.h"

#include <grid-files/common/AttributeList.h>
#include <grid-files/grid/GridValueList.h>



namespace SmartMet
{
namespace DataServer
{
namespace Corba
{



class Converter
{
  public:
                  Converter();
    virtual       ~Converter();

    static void   convert(T::GridData& source,DataServer::Corba::CorbaGridData& target);
    static void   convert(DataServer::Corba::CorbaGridData& source,T::GridData& target);

    static void   convert(T::Attribute& source,DataServer::Corba::CorbaAttribute& target);
    static void   convert(DataServer::Corba::CorbaAttribute& source,T::Attribute& target);

    static void   convert(T::AttributeList& source,DataServer::Corba::CorbaAttributeList& target);
    static void   convert(DataServer::Corba::CorbaAttributeList& source,T::AttributeList& target);

    static void   convert(T::GridCoordinates& source,DataServer::Corba::CorbaGridCoordinates& target);
    static void   convert(DataServer::Corba::CorbaGridCoordinates& source,T::GridCoordinates& target);

    static void   convert(T::ValueRecord& source,DataServer::Corba::CorbaValueRecord& target);
    static void   convert(DataServer::Corba::CorbaValueRecord& source,T::ValueRecord& target);

    static void   convert(T::ValueRecordList& source,DataServer::Corba::CorbaValueRecordList& target);
    static void   convert(DataServer::Corba::CorbaValueRecordList& source,T::ValueRecordList& target);

    static void   convert(T::ParamValue_vec& source,DataServer::Corba::CorbaParamValueList& target);
    static void   convert(DataServer::Corba::CorbaParamValueList& source,T::ParamValue_vec& target);

    static void   convert(T::GridValue& source,DataServer::Corba::CorbaGridValue& target);
    static void   convert(DataServer::Corba::CorbaGridValue& source,T::GridValue& target);

    static void   convert(T::GridValueList& source,DataServer::Corba::CorbaGridValueList& target);
    static void   convert(DataServer::Corba::CorbaGridValueList& source,T::GridValueList& target);

    static void   convert(T::Coordinate& source,DataServer::Corba::CorbaCoordinate& target);
    static void   convert(DataServer::Corba::CorbaCoordinate& source,T::Coordinate& target);

    static void   convert(std::vector<T::Coordinate>& source,DataServer::Corba::CorbaCoordinateList& target);
    static void   convert(const DataServer::Corba::CorbaCoordinateList& source,std::vector<T::Coordinate>& target);

    static void   convert(std::vector<std::vector<T::Coordinate>>& source,SmartMet::DataServer::Corba::CorbaPolygonPath& target);
    static void   convert(const SmartMet::DataServer::Corba::CorbaPolygonPath& source,std::vector<std::vector<T::Coordinate>>& target);

    static void   convert(double_vec& source,DataServer::Corba::CorbaDoubleList& target);
    static void   convert(DataServer::Corba::CorbaDoubleList& source,double_vec& target);

};



}
}
}

