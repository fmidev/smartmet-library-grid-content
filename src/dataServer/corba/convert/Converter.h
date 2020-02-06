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

    static void   convert(const T::GridData& source,DataServer::Corba::CorbaGridData& target);
    static void   convert(const DataServer::Corba::CorbaGridData& source,T::GridData& target);

    static void   convert(const T::Attribute& source,DataServer::Corba::CorbaAttribute& target);
    static void   convert(const DataServer::Corba::CorbaAttribute& source,T::Attribute& target);

    static void   convert(const T::AttributeList& source,DataServer::Corba::CorbaAttributeList& target);
    static void   convert(const DataServer::Corba::CorbaAttributeList& source,T::AttributeList& target);

    static void   convert(const T::GridCoordinates& source,DataServer::Corba::CorbaGridCoordinates& target);
    static void   convert(const DataServer::Corba::CorbaGridCoordinates& source,T::GridCoordinates& target);

    static void   convert(const T::ValueRecord& source,DataServer::Corba::CorbaValueRecord& target);
    static void   convert(const DataServer::Corba::CorbaValueRecord& source,T::ValueRecord& target);

    static void   convert(const T::ValueRecordList& source,DataServer::Corba::CorbaValueRecordList& target);
    static void   convert(const DataServer::Corba::CorbaValueRecordList& source,T::ValueRecordList& target);

    static void   convert(const T::ParamValue_vec& source,DataServer::Corba::CorbaParamValueList& target);
    static void   convert(const DataServer::Corba::CorbaParamValueList& source,T::ParamValue_vec& target);

    static void   convert(const T::GridValue& source,DataServer::Corba::CorbaGridValue& target);
    static void   convert(const DataServer::Corba::CorbaGridValue& source,T::GridValue& target);

    static void   convert(const T::GridValueList& source,DataServer::Corba::CorbaGridValueList& target);
    static void   convert(const DataServer::Corba::CorbaGridValueList& source,T::GridValueList& target);

    static void   convert(const T::Coordinate& source,DataServer::Corba::CorbaCoordinate& target);
    static void   convert(const DataServer::Corba::CorbaCoordinate& source,T::Coordinate& target);

    static void   convert(const std::vector<T::Coordinate>& source,DataServer::Corba::CorbaCoordinateList& target);
    static void   convert(const DataServer::Corba::CorbaCoordinateList& source,std::vector<T::Coordinate>& target);

    static void   convert(const std::vector<std::vector<T::Coordinate>>& source,SmartMet::DataServer::Corba::CorbaPolygonPath& target);
    static void   convert(const SmartMet::DataServer::Corba::CorbaPolygonPath& source,std::vector<std::vector<T::Coordinate>>& target);

    static void   convert(const double_vec& source,DataServer::Corba::CorbaDoubleList& target);
    static void   convert(const DataServer::Corba::CorbaDoubleList& source,double_vec& target);

    static void   convert(const SmartMet::T::ByteData& source, SmartMet::DataServer::Corba::CorbaByteData& target);
    static void   convert(const SmartMet::DataServer::Corba::CorbaByteData& source,SmartMet::T::ByteData& target);

    static void   convert(const SmartMet::T::ByteData_vec& source, SmartMet::DataServer::Corba::CorbaByteDataSequence& target);
    static void   convert(const SmartMet::DataServer::Corba::CorbaByteDataSequence& source,SmartMet::T::ByteData_vec& target);

    static void   convert(std::vector<uint>& source,DataServer::Corba::CorbaULongList& target);
    static void   convert(const DataServer::Corba::CorbaULongList& source,std::vector<uint>& target);

};



}
}
}

