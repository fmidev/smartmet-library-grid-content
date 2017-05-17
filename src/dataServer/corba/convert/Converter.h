#pragma once

#include "dataServer/definition/GridData.h"
#include "dataServer/definition/GridCoordinates.h"
#include "dataServer/definition/ValueRecordList.h"
#include "grid-files/common/AttributeList.h"


#include "dataServer/corba/stubs/DataServer_serviceInterface_SK.h"



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
};



}
}
}

