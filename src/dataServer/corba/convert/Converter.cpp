#include "Converter.h"
#include "../stubs/DataServer_serviceInterface_SK.h"



namespace SmartMet
{
namespace DataServer
{
namespace Corba
{



Converter::Converter()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Constructor failed!",nullptr);
  }
}





Converter::~Converter()
{
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void Converter::convert(const T::GridData& source,C::GridData& target)
{
  try
  {
    target.serverId = source.mServerId;
    target.producerId = source.mProducerId;
    target.generationId = source.mGenerationId;
    target.fileId = source.mFileId;
    target.fileType = (CORBA::Octet)source.mFileType;
    target.fileName = CORBA::string_dup(source.mFileName.c_str());
    target.messageIndex = source.mMessageIndex;
    target.forecastTime = CORBA::string_dup(source.mForecastTime.c_str());
    target.forecastType = source.mForecastType;
    target.forecastNumber = source.mForecastNumber;
    target.gribParameterId = CORBA::string_dup(source.mGribParameterId.c_str());
    target.grib1LevelId = source.mGrib1ParameterLevelId;
    target.grib2LevelId = source.mGrib2ParameterLevelId;
    target.gribParameterName = CORBA::string_dup(source.mGribParameterName.c_str());
    target.parameterId = CORBA::string_dup(source.mFmiParameterId.c_str());
    target.levelId = source.mFmiParameterLevelId;
    target.parameterName = CORBA::string_dup(source.mFmiParameterName.c_str());
    target.newbaseParameterId = CORBA::string_dup(source.mNewbaseParameterId.c_str());
    target.newbaseParameterName = CORBA::string_dup(source.mNewbaseParameterName.c_str());
    target.level = source.mParameterLevel;
    target.geometryId = source.mGeometryId;
    target.reverseXDirection = source.mReverseXDirection;
    target.reverseYDirection = source.mReverseYDirection;
    target.projection = source.mProjection;
    target.columns = source.mColumns;
    target.rows = source.mRows;

    convert(source.mProjectionAttributes,target.projectionAttributes);

    uint len = source.mValues.size();
    target.valueList.length(len);
    for (uint t=0; t<len; t++)
    {
      target.valueList[t] = source.mValues[t];
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::GridData& source,T::GridData& target)
{
  try
  {
    target.mServerId = source.serverId;
    target.mProducerId = source.producerId;
    target.mGenerationId = source.generationId;
    target.mFileId = source.fileId;
    target.mFileType = source.fileType;
    target.mFileName = source.fileName;
    target.mMessageIndex = source.messageIndex;
    target.mForecastTime = source.forecastTime;
    target.mForecastType = source.forecastType;
    target.mForecastNumber = source.forecastNumber;
    target.mGribParameterId = source.gribParameterId;
    target.mGrib1ParameterLevelId = source.grib1LevelId;
    target.mGrib2ParameterLevelId = source.grib2LevelId;
    target.mGribParameterName = source.gribParameterName;
    target.mFmiParameterId = source.parameterId;
    target.mFmiParameterLevelId = source.levelId;
    target.mFmiParameterName = source.parameterName;
    target.mNewbaseParameterId = source.newbaseParameterId;
    target.mNewbaseParameterName = source.newbaseParameterName;
    target.mParameterLevel = source.level;
    target.mGeometryId = source.geometryId;
    target.mReverseXDirection = source.reverseXDirection;
    target.mReverseYDirection = source.reverseYDirection;
    target.mProjection = source.projection;
    target.mColumns = source.columns;
    target.mRows = source.rows;

    convert(source.projectionAttributes,target.mProjectionAttributes);

    std::size_t len = source.valueList.length();
    target.mValues.reserve(len);
    for (uint t=0; t<len; t++)
    {
      target.mValues.emplace_back(source.valueList[t]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const T::Attribute& source,C::AttributeRec& target)
{
  try
  {
    target.name = CORBA::string_dup(source.mName.c_str());
    target.value = CORBA::string_dup(source.mValue.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::AttributeRec& source,T::Attribute& target)
{
  try
  {
    target.mName = source.name;
    target.mValue = source.value;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const T::AttributeList& source,C::AttributeRecList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::Attribute *attr = source.getAttributeByIndex(t);
      C::AttributeRec corbaObject;
      convert(*attr,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::AttributeRecList& source,T::AttributeList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      C::AttributeRec corbaObject = source[t];
      T::Attribute *attr = new T::Attribute();
      convert(corbaObject,*attr);
      target.addAttribute(attr);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



















void Converter::convert(const T::PropertySetting& source,C::PropertySetting& target)
{
  try
  {
    target.propertyId = source.propertyId;
    target.propertyValue = CORBA::string_dup(source.propertyValue.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::PropertySetting& source,T::PropertySetting& target)
{
  try
  {
    target.propertyId = source.propertyId;
    target.propertyValue = source.propertyValue;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const T::PropertySettingVec& source,C::PropertySettingList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      C::PropertySetting corbaObject;
      convert(source[t],corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::PropertySettingList& source,T::PropertySettingVec& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      C::PropertySetting corbaObject = source[t];
      T::PropertySetting prop;
      convert(corbaObject,prop);
      target.push_back(prop);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}
















void Converter::convert(const T::GridCoordinates& source,C::GridCoordinates& target)
{
  try
  {
    target.projection = source.mProjection;
    convert(source.mProjectionAttributes,target.projectionAttributes);
    target.rows = source.mRows;
    target.columns = source.mColumns;
    target.reverseXDirection = source.mReverseXDirection;
    target.reverseYDirection = source.mReverseYDirection;
    target.coordinateType = (CORBA::Octet)source.mCoordinateType;

    uint len = source.mCoordinateList.size();
    target.coordinateList.length(len);
    for (uint t=0; t<len; t++)
    {
      T::Coordinate coordinate = source.mCoordinateList[t];
      C::Coordinate corbaObject;
      corbaObject.x = coordinate.x();
      corbaObject.y = coordinate.y();
      target.coordinateList[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::GridCoordinates& source,T::GridCoordinates& target)
{
  try
  {
    target.mProjection = source.projection;
    convert(source.projectionAttributes,target.mProjectionAttributes);
    target.mRows = source.rows;
    target.mColumns = source.columns;
    target.mCoordinateType = source.coordinateType;
    target.mReverseXDirection = source.reverseXDirection;
    target.mReverseYDirection = source.reverseYDirection;

    target.mCoordinateList.clear();
    uint len = source.coordinateList.length();
    for (uint t=0; t<len; t++)
    {
      C::Coordinate corbaObject = source.coordinateList[t];
      T::Coordinate coordinate(corbaObject.x,corbaObject.y);
      target.mCoordinateList.emplace_back(coordinate);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}







void Converter::convert(const T::ValueRecord& source,C::ValueRecord& target)
{
  try
  {
    target.fileId = source.mFileId;
    target.messageIndex = source.mMessageIndex;
    target.coordinateType = (CORBA::Octet)source.mCoordinateType;
    target.areaInterpolationMethod = (CORBA::Short)source.mAreaInterpolationMethod;
    target.x = source.mX;
    target.y = source.mY;
    target.value = source.mValue;
    target.result = source.mResult;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::ValueRecord& source,T::ValueRecord& target)
{
  try
  {
    target.mFileId = source.fileId;
    target.mMessageIndex = source.messageIndex;
    target.mCoordinateType = source.coordinateType;
    target.mAreaInterpolationMethod = (short)source.areaInterpolationMethod;
    target.mX = source.x;
    target.mY = source.y;
    target.mValue = source.value;
    target.mResult = source.result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Converter::convert(const T::ValueRecordList& source,C::ValueRecordList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::ValueRecord *obj = source.getValueRecordByIndex(t);
      C::ValueRecord corbaObject;
      convert(*obj,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::ValueRecordList& source,T::ValueRecordList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      C::ValueRecord corbaObject = source[t];
      T::ValueRecord *obj = new T::ValueRecord();
      convert(corbaObject,*obj);
      target.addValueRecord(obj);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const T::ParamValue_vec& source,C::ParameterValueList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      target[t] = source[t];
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Converter::convert(const C::ParameterValueList& source,T::ParamValue_vec& target)
{
  try
  {
    uint len = source.length();
    target.clear();
    for (uint t=0; t<len; t++)
    {
      target.emplace_back(source[t]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}









void Converter::convert(const T::GridValue& source,C::GridValue& target)
{
  try
  {
    target.x = source.mX;
    target.y = source.mY;
    target.value = source.mValue;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::GridValue& source,T::GridValue& target)
{
  try
  {
    target.mX = source.x;
    target.mY = source.y;
    target.mValue = source.value;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Converter::convert(const T::GridValueList& source,C::GridValueList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::GridValue obj;
      source.getGridValueByIndex(t,obj);
      C::GridValue corbaObject;
      convert(obj,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::GridValueList& source,T::GridValueList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      C::GridValue corbaObject = source[t];
      T::GridValue obj;
      convert(corbaObject,obj);
      target.addGridValue(obj);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






void Converter::convert(const T::Coordinate& source,C::Coordinate& target)
{
  try
  {
    target.x = source.x();
    target.y = source.y();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::Coordinate& source,T::Coordinate& target)
{
  try
  {
    target.set(source.x,source.y);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const std::vector<T::Coordinate>& source,C::CoordinateList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      auto obj = source[t];
      C::Coordinate corbaObject;
      convert(obj,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::CoordinateList& source,std::vector<T::Coordinate>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      C::Coordinate corbaObject = source[t];
      target.emplace_back(corbaObject.x,corbaObject.y);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const std::vector<std::vector<T::Coordinate>>& source,SmartMet::C::PolygonPath& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      auto obj = source[t];
      C::CoordinateList corbaObject;
      convert(obj,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const SmartMet::C::PolygonPath& source,std::vector<std::vector<T::Coordinate>>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      C::CoordinateList corbaObject = source[t];
      std::vector<T::Coordinate> obj;
      convert(corbaObject,obj);
      target.emplace_back(obj);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const double_vec& source,C::Float64List& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      target[t] = source[t];
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::Float64List& source,double_vec& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      target.emplace_back(source[t]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const SmartMet::T::ByteData& source, SmartMet::C::ByteData& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      target[t] = source[t];
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const SmartMet::C::ByteData& source,SmartMet::T::ByteData& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      target.emplace_back(source[t]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const SmartMet::T::ByteData_vec& source, SmartMet::C::ByteDataSequence& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      auto obj = source[t];
      C::ByteData corbaObject;
      convert(obj,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const SmartMet::C::ByteDataSequence& source,SmartMet::T::ByteData_vec& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      C::ByteData corbaObject = source[t];
      T::ByteData obj;
      convert(corbaObject,obj);
      target.emplace_back(obj);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Converter::convert(std::vector<uint>& source,C::UInt32List& target)
{
  try
  {
    uint len = source.size();
    target.length(len);

    uint t = 0;
    for (auto it=source.begin(); it!=source.end(); ++it)
    {
      target[t] = *it;
      t++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::UInt32List& source,std::vector<uint>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      target.emplace_back(source[t]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*
void Converter::convert(double_vec& source,C::Float64List& target)
{
  try
  {
    uint len = source.size();
    target.length(len);

    uint t = 0;
    for (auto it=source.begin(); it!=source.end(); ++it)
    {
      target[t] = *it;
      t++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::Float64List& source,double_vec& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      target.emplace_back(source[t]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}
*/





}
}
}

