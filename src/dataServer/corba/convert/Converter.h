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

    static void   convert(const T::GridData& source,C::GridData& target);
    static void   convert(const C::GridData& source,T::GridData& target);

    static void   convert(const T::Attribute& source,C::AttributeRec& target);
    static void   convert(const C::AttributeRec& source,T::Attribute& target);

    static void   convert(const T::AttributeList& source,C::AttributeRecList& target);
    static void   convert(const C::AttributeRecList& source,T::AttributeList& target);

    static void   convert(const T::PropertySetting& source,C::PropertySetting& target);
    static void   convert(const C::PropertySetting& source,T::PropertySetting& target);

    static void   convert(const T::PropertySettingVec& source,C::PropertySettingList& target);
    static void   convert(const C::PropertySettingList& source,T::PropertySettingVec& target);

    static void   convert(const T::GridCoordinates& source,C::GridCoordinates& target);
    static void   convert(const C::GridCoordinates& source,T::GridCoordinates& target);

    static void   convert(const T::ValueRecord& source,C::ValueRecord& target);
    static void   convert(const C::ValueRecord& source,T::ValueRecord& target);

    static void   convert(const T::ValueRecordList& source,C::ValueRecordList& target);
    static void   convert(const C::ValueRecordList& source,T::ValueRecordList& target);

    static void   convert(const T::ParamValue_vec& source,C::ParameterValueList& target);
    static void   convert(const C::ParameterValueList& source,T::ParamValue_vec& target);

    static void   convert(const T::GridValue& source,C::GridValue& target);
    static void   convert(const C::GridValue& source,T::GridValue& target);

    static void   convert(const T::GridValueList& source,C::GridValueList& target);
    static void   convert(const C::GridValueList& source,T::GridValueList& target);

    static void   convert(const T::Coordinate& source,C::Coordinate& target);
    static void   convert(const C::Coordinate& source,T::Coordinate& target);

    static void   convert(const std::vector<T::Coordinate>& source,C::CoordinateList& target);
    static void   convert(const C::CoordinateList& source,std::vector<T::Coordinate>& target);

    static void   convert(const std::vector<std::vector<T::Coordinate>>& source,SmartMet::C::PolygonPath& target);
    static void   convert(const SmartMet::C::PolygonPath& source,std::vector<std::vector<T::Coordinate>>& target);

    static void   convert(const double_vec& source,C::Float64List& target);
    static void   convert(const C::Float64List& source,double_vec& target);

    static void   convert(const SmartMet::T::ByteData& source, SmartMet::C::ByteData& target);
    static void   convert(const SmartMet::C::ByteData& source,SmartMet::T::ByteData& target);

    static void   convert(const SmartMet::T::ByteData_vec& source, SmartMet::C::ByteDataSequence& target);
    static void   convert(const SmartMet::C::ByteDataSequence& source,SmartMet::T::ByteData_vec& target);

    static void   convert(std::vector<uint>& source,C::UInt32List& target);
    static void   convert(const C::UInt32List& source,std::vector<uint>& target);

    //static void   convert(double_vec& source,C::Float64List& target);
    //static void   convert(const C::Float64List& source,double_vec& target);
};



}
}
}

