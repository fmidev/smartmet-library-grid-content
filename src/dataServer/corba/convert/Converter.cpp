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





void Converter::convert(const T::GridData& source,DataServer::Corba::CorbaGridData& target)
{
  try
  {
    target.serverId = source.mServerId;
    target.groupFlags = source.mGroupFlags;
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
    target.grib1ParameterLevelId = source.mGrib1ParameterLevelId;
    target.grib2ParameterLevelId = source.mGrib2ParameterLevelId;
    target.gribParameterName = CORBA::string_dup(source.mGribParameterName.c_str());
    target.gribParameterUnits = CORBA::string_dup(source.mGribParameterUnits.c_str());
    target.fmiParameterId = CORBA::string_dup(source.mFmiParameterId.c_str());
    target.fmiParameterLevelId = source.mFmiParameterLevelId;
    target.fmiParameterName = CORBA::string_dup(source.mFmiParameterName.c_str());
    target.fmiParameterUnits = CORBA::string_dup(source.mFmiParameterUnits.c_str());
    target.cdmParameterId = CORBA::string_dup(source.mCdmParameterId.c_str());
    target.cdmParameterName = CORBA::string_dup(source.mCdmParameterName.c_str());
    target.newbaseParameterId = CORBA::string_dup(source.mNewbaseParameterId.c_str());
    target.newbaseParameterName = CORBA::string_dup(source.mNewbaseParameterName.c_str());
    target.parameterLevel = source.mParameterLevel;
    target.geometryId = source.mGeometryId;
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





void Converter::convert(const DataServer::Corba::CorbaGridData& source,T::GridData& target)
{
  try
  {
    target.mServerId = source.serverId;
    target.mGroupFlags = source.groupFlags;
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
    target.mGrib1ParameterLevelId = source.grib1ParameterLevelId;
    target.mGrib2ParameterLevelId = source.grib2ParameterLevelId;
    target.mGribParameterName = source.gribParameterName;
    target.mGribParameterUnits = source.gribParameterUnits;
    target.mFmiParameterId = source.fmiParameterId;
    target.mFmiParameterLevelId = source.fmiParameterLevelId;
    target.mFmiParameterName = source.fmiParameterName;
    target.mFmiParameterUnits = source.fmiParameterUnits;
    target.mCdmParameterId = source.cdmParameterId;
    target.mCdmParameterName = source.cdmParameterName;
    target.mNewbaseParameterId = source.newbaseParameterId;
    target.mNewbaseParameterName = source.newbaseParameterName;
    target.mParameterLevel = source.parameterLevel;
    target.mGeometryId = source.geometryId;
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





void Converter::convert(const T::Attribute& source,DataServer::Corba::CorbaAttribute& target)
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





void Converter::convert(const DataServer::Corba::CorbaAttribute& source,T::Attribute& target)
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





void Converter::convert(const T::AttributeList& source,DataServer::Corba::CorbaAttributeList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::Attribute *attr = source.getAttributeByIndex(t);
      DataServer::Corba::CorbaAttribute corbaObject;
      convert(*attr,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const DataServer::Corba::CorbaAttributeList& source,T::AttributeList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      DataServer::Corba::CorbaAttribute corbaObject = source[t];
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




void Converter::convert(const T::GridCoordinates& source,DataServer::Corba::CorbaGridCoordinates& target)
{
  try
  {
    target.projection = source.mProjection;
    convert(source.mProjectionAttributes,target.projectionAttributes);
    target.rows = source.mRows;
    target.columns = source.mColumns;
    target.coordinateType = (CORBA::Octet)source.mCoordinateType;

    uint len = source.mCoordinateList.size();
    target.coordinateList.length(len);
    for (uint t=0; t<len; t++)
    {
      T::Coordinate coordinate = source.mCoordinateList[t];
      DataServer::Corba::CorbaCoordinate corbaObject;
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





void Converter::convert(const DataServer::Corba::CorbaGridCoordinates& source,T::GridCoordinates& target)
{
  try
  {
    target.mProjection = source.projection;
    convert(source.projectionAttributes,target.mProjectionAttributes);
    target.mRows = source.rows;
    target.mColumns = source.columns;
    target.mCoordinateType = source.coordinateType;

    target.mCoordinateList.clear();
    uint len = source.coordinateList.length();
    for (uint t=0; t<len; t++)
    {
      DataServer::Corba::CorbaCoordinate corbaObject = source.coordinateList[t];
      T::Coordinate coordinate(corbaObject.x,corbaObject.y);
      target.mCoordinateList.emplace_back(coordinate);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}







void Converter::convert(const T::ValueRecord& source,DataServer::Corba::CorbaValueRecord& target)
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





void Converter::convert(const DataServer::Corba::CorbaValueRecord& source,T::ValueRecord& target)
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




void Converter::convert(const T::ValueRecordList& source,DataServer::Corba::CorbaValueRecordList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::ValueRecord *obj = source.getValueRecordByIndex(t);
      DataServer::Corba::CorbaValueRecord corbaObject;
      convert(*obj,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const DataServer::Corba::CorbaValueRecordList& source,T::ValueRecordList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      DataServer::Corba::CorbaValueRecord corbaObject = source[t];
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





void Converter::convert(const T::ParamValue_vec& source,DataServer::Corba::CorbaParamValueList& target)
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




void Converter::convert(const DataServer::Corba::CorbaParamValueList& source,T::ParamValue_vec& target)
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









void Converter::convert(const T::GridValue& source,DataServer::Corba::CorbaGridValue& target)
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





void Converter::convert(const DataServer::Corba::CorbaGridValue& source,T::GridValue& target)
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




void Converter::convert(const T::GridValueList& source,DataServer::Corba::CorbaGridValueList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::GridValue obj;
      source.getGridValueByIndex(t,obj);
      DataServer::Corba::CorbaGridValue corbaObject;
      convert(obj,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const DataServer::Corba::CorbaGridValueList& source,T::GridValueList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      DataServer::Corba::CorbaGridValue corbaObject = source[t];
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






void Converter::convert(const T::Coordinate& source,DataServer::Corba::CorbaCoordinate& target)
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





void Converter::convert(const DataServer::Corba::CorbaCoordinate& source,T::Coordinate& target)
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





void Converter::convert(const std::vector<T::Coordinate>& source,DataServer::Corba::CorbaCoordinateList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      auto obj = source[t];
      DataServer::Corba::CorbaCoordinate corbaObject;
      convert(obj,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const DataServer::Corba::CorbaCoordinateList& source,std::vector<T::Coordinate>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      DataServer::Corba::CorbaCoordinate corbaObject = source[t];
      target.emplace_back(T::Coordinate(corbaObject.x,corbaObject.y));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const std::vector<std::vector<T::Coordinate>>& source,SmartMet::DataServer::Corba::CorbaPolygonPath& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      auto obj = source[t];
      DataServer::Corba::CorbaCoordinateList corbaObject;
      convert(obj,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const SmartMet::DataServer::Corba::CorbaPolygonPath& source,std::vector<std::vector<T::Coordinate>>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      DataServer::Corba::CorbaCoordinateList corbaObject = source[t];
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





void Converter::convert(const double_vec& source,DataServer::Corba::CorbaDoubleList& target)
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





void Converter::convert(const DataServer::Corba::CorbaDoubleList& source,double_vec& target)
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





void Converter::convert(const SmartMet::T::ByteData& source, SmartMet::DataServer::Corba::CorbaByteData& target)
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





void Converter::convert(const SmartMet::DataServer::Corba::CorbaByteData& source,SmartMet::T::ByteData& target)
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





void Converter::convert(const SmartMet::T::ByteData_vec& source, SmartMet::DataServer::Corba::CorbaByteDataSequence& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      auto obj = source[t];
      DataServer::Corba::CorbaByteData corbaObject;
      convert(obj,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const SmartMet::DataServer::Corba::CorbaByteDataSequence& source,SmartMet::T::ByteData_vec& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      DataServer::Corba::CorbaByteData corbaObject = source[t];
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




void Converter::convert(std::vector<uint>& source,DataServer::Corba::CorbaULongList& target)
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





void Converter::convert(const DataServer::Corba::CorbaULongList& source,std::vector<uint>& target)
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







}
}
}

