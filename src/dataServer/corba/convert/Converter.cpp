#include "Converter.h"
#include "dataServer/corba/stubs/DataServer_serviceInterface_SK.h"



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
    throw SmartMet::Spine::Exception(BCP,"Constructor failed!",NULL);
  }
}





Converter::~Converter()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Destructor failed!",NULL);
  }
}





void Converter::convert(T::GridData& source,DataServer::Corba::CorbaGridData& target)
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
    target.startTime = CORBA::string_dup(source.mStartTime.c_str());
    target.endTime = CORBA::string_dup(source.mEndTime.c_str());
    target.gribParameterId = CORBA::string_dup(source.mGribParameterId.c_str());
    target.grib1ParameterLevelId = source.mGrib1ParameterLevelId;
    target.grib2ParameterLevelId = source.mGrib2ParameterLevelId;
    target.gribParameterName = CORBA::string_dup(source.mGribParameterName.c_str());
    target.gribParameterDescription = CORBA::string_dup(source.mGribParameterDescription.c_str());
    target.gribParameterUnits = CORBA::string_dup(source.mGribParameterUnits.c_str());
    target.fmiParameterId = CORBA::string_dup(source.mFmiParameterId.c_str());
    target.fmiParameterLevelId = source.mFmiParameterLevelId;
    target.fmiParameterName = CORBA::string_dup(source.mFmiParameterName.c_str());
    target.fmiParameterDescription = CORBA::string_dup(source.mFmiParameterDescription.c_str());
    target.fmiParameterUnits = CORBA::string_dup(source.mFmiParameterUnits.c_str());
    target.cdmParameterId = CORBA::string_dup(source.mCdmParameterId.c_str());
    target.cdmParameterName = CORBA::string_dup(source.mCdmParameterName.c_str());
    target.newbaseParameterId = CORBA::string_dup(source.mNewbaseParameterId.c_str());
    target.newbaseParameterName = CORBA::string_dup(source.mNewbaseParameterName.c_str());
    target.parameterLevel = source.mParameterLevel;
    target.projection = source.mProjection;
    target.columns = source.mColumns;
    target.rows = source.mRows;
    target.typeOfEnsembleForecast = source.mTypeOfEnsembleForecast;
    target.perturbationNumber = source.mPerturbationNumber;

    convert(source.mProjectionAttributes,target.projectionAttributes);

    uint len = (uint)source.mValues.size();
    target.valueList.length(len);
    for (uint t=0; t<len; t++)
    {
      target.valueList[t] = source.mValues[t];
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void Converter::convert(DataServer::Corba::CorbaGridData& source,T::GridData& target)
{
  try
  {
    target.mServerId = source.serverId;
    target.mGroupFlags = source.groupFlags;
    target.mProducerId = source.producerId;
    target.mGenerationId = source.generationId;
    target.mFileId = source.fileId;
    target.mFileType = (T::FileType)source.fileType;
    target.mFileName = source.fileName;
    target.mMessageIndex = source.messageIndex;
    target.mStartTime = source.startTime;
    target.mEndTime = source.endTime;
    target.mGribParameterId = source.gribParameterId;
    target.mGrib1ParameterLevelId = (T::ParamLevelId)source.grib1ParameterLevelId;
    target.mGrib2ParameterLevelId = (T::ParamLevelId)source.grib2ParameterLevelId;
    target.mGribParameterName = source.gribParameterName;
    target.mGribParameterDescription = source.gribParameterDescription;
    target.mGribParameterUnits = source.gribParameterUnits;
    target.mFmiParameterId = source.fmiParameterId;
    target.mFmiParameterLevelId = (T::ParamLevelId)source.fmiParameterLevelId;
    target.mFmiParameterName = source.fmiParameterName;
    target.mFmiParameterDescription = source.fmiParameterDescription;
    target.mFmiParameterUnits = source.fmiParameterUnits;
    target.mCdmParameterId = source.cdmParameterId;
    target.mCdmParameterName = source.cdmParameterName;
    target.mNewbaseParameterId = source.newbaseParameterId;
    target.mNewbaseParameterName = source.newbaseParameterName;
    target.mParameterLevel = source.parameterLevel;
    target.mProjection = source.projection;
    target.mColumns = source.columns;
    target.mRows = source.rows;
    target.mTypeOfEnsembleForecast = source.typeOfEnsembleForecast;
    target.mPerturbationNumber = source.perturbationNumber;

    convert(source.projectionAttributes,target.mProjectionAttributes);

    std::size_t len = source.valueList.length();
    target.mValues.reserve(len);
    for (uint t=0; t<len; t++)
    {
      target.mValues.push_back((T::ParamValue)source.valueList[t]);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void Converter::convert(T::Attribute& source,DataServer::Corba::CorbaAttribute& target)
{
  try
  {
    target.name = CORBA::string_dup(source.mName.c_str());
    target.value = CORBA::string_dup(source.mValue.c_str());
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void Converter::convert(DataServer::Corba::CorbaAttribute& source,T::Attribute& target)
{
  try
  {
    target.mName = source.name;
    target.mValue = source.value;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void Converter::convert(T::AttributeList& source,DataServer::Corba::CorbaAttributeList& target)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void Converter::convert(DataServer::Corba::CorbaAttributeList& source,T::AttributeList& target)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void Converter::convert(T::GridCoordinates& source,DataServer::Corba::CorbaGridCoordinates& target)
{
  try
  {
    target.projection = source.mProjection;
    target.rows = source.mRows;
    target.columns = source.mColumns;

    convert(source.mProjectionAttributes,target.projectionAttributes);

    uint len = (uint)source.mCoordinateList.size();
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void Converter::convert(DataServer::Corba::CorbaGridCoordinates& source,T::GridCoordinates& target)
{
  try
  {
    target.mProjection = source.projection;
    target.mRows = source.rows;
    target.mColumns = source.columns;
    convert(source.projectionAttributes,target.mProjectionAttributes);

    target.mCoordinateList.clear();
    uint len = source.coordinateList.length();
    for (uint t=0; t<len; t++)
    {
      DataServer::Corba::CorbaCoordinate corbaObject = source.coordinateList[t];
      T::Coordinate coordinate(corbaObject.x,corbaObject.y);
      target.mCoordinateList.push_back(coordinate);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}







void Converter::convert(T::ValueRecord& source,DataServer::Corba::CorbaValueRecord& target)
{
  try
  {
    target.fileId = source.mFileId;
    target.messageIndex = source.mMessageIndex;
    target.coordinateType = (CORBA::Octet)source.mCoordinateType;
    target.interpolationMethod = (CORBA::Octet)source.mInterpolationMethod;
    target.x = source.mX;
    target.y = source.mY;
    target.value = source.mValue;
    target.result = source.mResult;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void Converter::convert(DataServer::Corba::CorbaValueRecord& source,T::ValueRecord& target)
{
  try
  {
    target.mFileId = source.fileId;
    target.mMessageIndex = source.messageIndex;
    target.mCoordinateType = (T::CoordinateType)source.coordinateType;
    target.mInterpolationMethod = (T::InterpolationMethod)source.interpolationMethod;
    target.mX = source.x;
    target.mY = source.y;
    target.mValue = source.value;
    target.mResult = source.result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void Converter::convert(T::ValueRecordList& source,DataServer::Corba::CorbaValueRecordList& target)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void Converter::convert(DataServer::Corba::CorbaValueRecordList& source,T::ValueRecordList& target)
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void Converter::convert(T::ParamValue_vec& source,DataServer::Corba::CorbaParamValueList& target)
{
  try
  {
    uint len = (uint)source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      target[t] = source[t];
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void Converter::convert(DataServer::Corba::CorbaParamValueList& source,T::ParamValue_vec& target)
{
  try
  {
    uint len = (uint)source.length();
    target.clear();
    for (uint t=0; t<len; t++)
    {
      target.push_back((T::ParamValue)source[t]);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




}
}
}

