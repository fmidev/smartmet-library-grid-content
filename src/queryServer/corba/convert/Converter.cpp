#include "Converter.h"
#include <macgyver/Exception.h>


namespace SmartMet
{
namespace QueryServer
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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





void Converter::convert(std::set<int>& source,QueryServer::Corba::CorbaLongList& target)
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





void Converter::convert(const QueryServer::Corba::CorbaLongList& source,std::set<int>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      target.insert(source[t]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}











void Converter::convert(std::set<time_t>& source,QueryServer::Corba::CorbaULongLongList& target)
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





void Converter::convert(const QueryServer::Corba::CorbaULongLongList& source,std::set<time_t>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      target.insert(source[t]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(std::vector<uint>& source,QueryServer::Corba::CorbaULongList& target)
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





void Converter::convert(const QueryServer::Corba::CorbaULongList& source,std::vector<uint>& target)
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





void Converter::convert(string_set& source,QueryServer::Corba::CorbaStringList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    uint t = 0;
    for (auto it=source.begin(); it!=source.end(); ++it)
    {
      target[t] = CORBA::string_dup(it->c_str());
      t++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Converter::convert(QueryServer::Corba::CorbaStringList& source,string_set& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      target.insert(std::string(source[t]));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(string_vec& source,QueryServer::Corba::CorbaStringList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    uint t = 0;
    for (auto it=source.begin(); it!=source.end(); ++it)
    {
      target[t] = CORBA::string_dup(it->c_str());
      t++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(QueryServer::Corba::CorbaStringList& source,string_vec& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      target.emplace_back(std::string(source[t]));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const T::Attribute& source,QueryServer::Corba::CorbaAttribute& target)
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





void Converter::convert(const QueryServer::Corba::CorbaAttribute& source,T::Attribute& target)
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





void Converter::convert(const T::AttributeList& source,QueryServer::Corba::CorbaAttributeList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::Attribute *attr = source.getAttributeByIndex(t);
      QueryServer::Corba::CorbaAttribute corbaObject;
      convert(*attr,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const QueryServer::Corba::CorbaAttributeList& source,T::AttributeList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      QueryServer::Corba::CorbaAttribute corbaObject = source[t];
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




void Converter::convert(const T::ParamValue_vec& source,QueryServer::Corba::CorbaParamValueList& target)
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




void Converter::convert(const QueryServer::Corba::CorbaParamValueList& source,T::ParamValue_vec& target)
{
  try
  {
    uint len = source.length();
    target.clear();
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





void Converter::convert(T::GridPointValue& source,QueryServer::Corba::CorbaGridPointValue& target)
{
  try
  {
    target.fileId = source.mFileId;
    target.messageIndex = source.mMessageIndex;
    target.time  = source.mTime;
    target.value = source.mValue;
    target.x = source.mX;
    target.y = source.mY;
    target.level = source.mLevel;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(QueryServer::Corba::CorbaGridPointValue& source,T::GridPointValue& target)
{
  try
  {
    target.mFileId = source.fileId;
    target.mMessageIndex = source.messageIndex;
    target.mTime  = source.time;
    target.mValue = source.value;
    target.mX = source.x;
    target.mY = source.y;
    target.mLevel = source.level;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::GridPointValueList& source,QueryServer::Corba::CorbaGridPointValueList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::GridPointValue *obj = source.getGridPointValueByIndex(t);
      QueryServer::Corba::CorbaGridPointValue corbaObject;
      convert(*obj,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(QueryServer::Corba::CorbaGridPointValueList& source,T::GridPointValueList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      QueryServer::Corba::CorbaGridPointValue corbaObject = source[t];
      T::GridPointValue *obj = new T::GridPointValue();
      convert(corbaObject,*obj);
      target.addGridPointValue(obj);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::GridValue& source,QueryServer::Corba::CorbaGridValue& target)
{
  try
  {
    target.x = source.mX;
    target.y = source.mY;
    target.value = source.mValue;
    target.valueString  = CORBA::string_dup(source.mValueString.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(QueryServer::Corba::CorbaGridValue& source,T::GridValue& target)
{
  try
  {
    target.mX = source.x;
    target.mY = source.y;
    target.mValue = source.value;
    target.mValueString = source.valueString;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(QueryServer::Corba::CorbaGridValueList& source,T::GridValueList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      QueryServer::Corba::CorbaGridValue corbaObject = source[t];
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





void Converter::convert(T::GridValueList& source,QueryServer::Corba::CorbaGridValueList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::GridValue obj;
      source.getGridValueByIndex(t,obj);
      QueryServer::Corba::CorbaGridValue corbaObject;
      convert(obj,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(QueryServer::Corba::CorbaParameterValues& source,QueryServer::ParameterValues& target)
{
  try
  {
    //target.mForecastTime = source.forecastTime;
    target.mForecastTimeUTC = (time_t)source.forecastTimeUTC;
    target.mProducerId = source.producerId;
    target.mGenerationId = source.generationId;
    target.mGenerationFlags = source.generationFlags;
    target.mGeometryId = source.geometryId;

    for (uint t=0; t<4; t++)
    {
      target.mFileId[t] = source.fileId[t];
      target.mMessageIndex[t] = source.messageIndex[t];
    }

    target.mModificationTime = source.modificationTime;
    target.mAnalysisTime = source.analysisTime;
    target.mParameterKeyType = source.parameterKeyType;
    target.mParameterKey = source.parameterKey;
    target.mParameterLevelId = source.parameterLevelId;
    target.mParameterLevel = source.parameterLevel;
    target.mForecastType = source.forecastType;
    target.mForecastNumber = source.forecastNumber;
    target.mFlags = source.flags;

    convert(source.valueList,target.mValueList);
    convert(source.valueVector,target.mValueVector);
    convert(source.valueData,target.mValueData);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(QueryServer::ParameterValues& source,QueryServer::Corba::CorbaParameterValues& target)
{
  try
  {
    //target.forecastTime = CORBA::string_dup(source.mForecastTime.c_str());
    target.forecastTimeUTC = source.mForecastTimeUTC;
    target.producerId = source.mProducerId;
    target.generationId = source.mGenerationId;
    target.generationFlags = source.mGenerationFlags;
    target.geometryId = source.mGeometryId;

    for (uint t=0; t<4; t++)
    {
      target.fileId[t] = source.mFileId[t];
      target.messageIndex[t] = source.mMessageIndex[t];
    }

    target.modificationTime = source.mModificationTime;
    target.analysisTime = CORBA::string_dup(source.mAnalysisTime.c_str());
    target.parameterKeyType = (::CORBA::Octet)source.mParameterKeyType;
    target.parameterKey = CORBA::string_dup(source.mParameterKey.c_str());
    target.parameterLevelId = (::CORBA::Octet)source.mParameterLevelId;
    target.parameterLevel = source.mParameterLevel;
    target.forecastType = source.mForecastType;
    target.forecastNumber = source.mForecastNumber;
    target.flags = source.mFlags;

    convert(source.mValueList,target.valueList);
    convert(source.mValueVector,target.valueVector);
    convert(source.mValueData,target.valueData);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(SmartMet::QueryServer::Corba::CorbaFunctionParam& source, SmartMet::QueryServer::FunctionParam& target)
{
  try
  {
    target.first = source.id;
    target.second = source.param;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(SmartMet::QueryServer::FunctionParam& source,SmartMet::QueryServer::Corba::CorbaFunctionParam& target)
{
  try
  {
    target.id = source.first;
    target.param = CORBA::string_dup(source.second.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Converter::convert(SmartMet::QueryServer::Corba::CorbaFunctionParamList& source, SmartMet::QueryServer::FunctionParam_vec& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      QueryServer::Corba::CorbaFunctionParam corbaObject = source[t];
      QueryServer::FunctionParam obj;
      convert(corbaObject,obj);
      target.emplace_back(obj);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






void Converter::convert(SmartMet::QueryServer::FunctionParam_vec& source,SmartMet::QueryServer::Corba::CorbaFunctionParamList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      QueryServer::Corba::CorbaFunctionParam corbaObject;
      convert(source[t],corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(QueryServer::Corba::CorbaParameterValuesList& source,QueryServer::ParameterValues_sptr_vec& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      QueryServer::Corba::CorbaParameterValues corbaObject = source[t];
      QueryServer::ParameterValues *obj = new QueryServer::ParameterValues();
      convert(corbaObject,*obj);
      target.emplace_back(std::shared_ptr<QueryServer::ParameterValues>(obj));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(QueryServer::ParameterValues_sptr_vec& source,QueryServer::Corba::CorbaParameterValuesList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      QueryServer::Corba::CorbaParameterValues corbaObject;
      convert(*source[t],corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}







void Converter::convert(QueryServer::Corba::CorbaQueryParameter& source,QueryServer::QueryParameter& target)
{
  try
  {
    target.mId = source.id;
    target.mAlternativeParamId = source.alternativeParamId;
    target.mType = source.type;
    target.mLocationType = source.locationType;
    target.mParam = source.param;
    target.mOrigParam = source.origParam;
    target.mSymbolicName = source.symbolicName;
    target.mParameterKeyType = source.parameterKeyType;
    target.mParameterKey = source.parameterKey;
    target.mProducerName = source.producerName;
    target.mGeometryId = source.geometryId;
    target.mParameterLevelId = source.parameterLevelId;
    target.mParameterLevel = source.parameterLevel;
    target.mForecastType = source.forecastType;
    target.mForecastNumber = source.forecastNumber;
    target.mAreaInterpolationMethod = (short)source.areaInterpolationMethod;
    target.mTimeInterpolationMethod = (short)source.timeInterpolationMethod;
    target.mLevelInterpolationMethod = (short)source.levelInterpolationMethod;
    target.mProducerId = source.producerId;
    target.mGenerationFlags = source.generationFlags;
    target.mPrecision = source.precision;
    target.mFunction = source.function;
    target.mTimestepsBefore = source.timestepsBefore;
    target.mTimestepsAfter = source.timestepsAfter;
    target.mTimestepSizeInMinutes = source.timestepSizeInMinutes;
    target.mFlags = source.flags;
    convert(source.contourLowValues,target.mContourLowValues);
    convert(source.contourHighValues,target.mContourHighValues);
    convert(source.contourColors,target.mContourColors);

    convert(source.functionParams,target.mFunctionParams);
    convert(source.valueList,target.mValueList);
    convert(source.coordinates,target.mCoordinates);
    convert(source.attributeList,target.mAttributeList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(QueryServer::QueryParameter& source,QueryServer::Corba::CorbaQueryParameter& target)
{
  try
  {
    target.id = source.mId;
    target.alternativeParamId = source.mAlternativeParamId;
    target.type = source.mType;
    target.locationType = source.mLocationType;
    target.param = CORBA::string_dup(source.mParam.c_str());
    target.origParam = CORBA::string_dup(source.mOrigParam.c_str());
    target.symbolicName = CORBA::string_dup(source.mSymbolicName.c_str());
    target.parameterKeyType = (::CORBA::Octet)source.mParameterKeyType;
    target.parameterKey = CORBA::string_dup(source.mParameterKey.c_str());
    target.producerName = CORBA::string_dup(source.mProducerName.c_str());
    target.geometryId = source.mGeometryId;
    target.parameterLevelId = (::CORBA::Octet)source.mParameterLevelId;
    target.parameterLevel = source.mParameterLevel;
    target.forecastType = source.mForecastType;
    target.forecastNumber = source.mForecastNumber;
    target.areaInterpolationMethod = (::CORBA::Short)source.mAreaInterpolationMethod;
    target.timeInterpolationMethod = (::CORBA::Short)source.mTimeInterpolationMethod;
    target.levelInterpolationMethod = (::CORBA::Short)source.mLevelInterpolationMethod;
    target.producerId = source.mProducerId;
    target.generationFlags = source.mGenerationFlags;
    target.precision = source.mPrecision;
    target.function = CORBA::string_dup(source.mFunction.c_str());
    target.timestepsBefore = source.mTimestepsBefore;
    target.timestepsAfter = source.mTimestepsAfter;
    target.timestepSizeInMinutes = source.mTimestepSizeInMinutes;
    target.flags = source.mFlags;
    convert(source.mContourLowValues,target.contourLowValues);
    convert(source.mContourHighValues,target.contourHighValues);
    convert(source.mContourColors,target.contourColors);

    convert(source.mFunctionParams,target.functionParams);
    convert(source.mValueList,target.valueList);
    convert(source.mCoordinates,target.coordinates);
    convert(source.mAttributeList,target.attributeList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(QueryServer::Corba::CorbaQueryParameterList& source,QueryServer::QueryParameter_vec& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      QueryServer::Corba::CorbaQueryParameter corbaObject = source[t];
      QueryServer::QueryParameter obj;
      convert(corbaObject,obj);
      target.emplace_back(obj);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(QueryServer::QueryParameter_vec& source,QueryServer::Corba::CorbaQueryParameterList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      QueryServer::Corba::CorbaQueryParameter corbaObject;
      convert(source[t],corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::Coordinate& source,QueryServer::Corba::CorbaCoordinate& target)
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





void Converter::convert(QueryServer::Corba::CorbaCoordinate& source,T::Coordinate& target)
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





void Converter::convert(QueryServer::Corba::CorbaCoordinateList& source,T::Coordinate_vec& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      QueryServer::Corba::CorbaCoordinate corbaObject = source[t];
      T::Coordinate obj;
      convert(corbaObject,obj);
      target.emplace_back(obj);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::Coordinate_vec& source,QueryServer::Corba::CorbaCoordinateList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      QueryServer::Corba::CorbaCoordinate corbaObject;
      convert(source[t],corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(QueryServer::Corba::CorbaAreaCoordinates& source,T::AreaCoordinates& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      QueryServer::Corba::CorbaCoordinateList corbaObject = source[t];
      T::Coordinate_vec obj;
      convert(corbaObject,obj);
      target.emplace_back(obj);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::AreaCoordinates& source,QueryServer::Corba::CorbaAreaCoordinates& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      QueryServer::Corba::CorbaCoordinateList corbaObject;
      convert(source[t],corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(QueryServer::Corba::CorbaQuery& source,QueryServer::Query& target)
{
  try
  {
    //target.mType = source.type;
    target.mSearchType = source.searchType;
    convert(source.producerNameList,target.mProducerNameList);
    target.mTimezone = source.timezone;
    target.mStartTime = source.startTime;
    target.mEndTime = source.endTime;
    target.mTimesteps = source.timesteps;
    target.mTimestepSizeInMinutes = source.timestepSizeInMinutes;
    target.mAnalysisTime = source.analysisTime;
    convert(source.geometryIdList,target.mGeometryIdList);
    convert(source.forecastTimeList,target.mForecastTimeList);
    target.mCoordinateType = source.coordinateType;
    convert(source.attributeList,target.mAttributeList);
    convert(source.areaCoordinates,target.mAreaCoordinates);
    target.mRadius = source.radius;
    convert(source.queryParameterList,target.mQueryParameterList);
    target.mLanguage = source.language;
    target.mGenerationFlags = source.generationFlags;
    target.mFlags = source.flags;
    target.mMaxParameterValues = source.maxParameterValues;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(QueryServer::Query& source,QueryServer::Corba::CorbaQuery& target)
{
  try
  {
    //target.type = (::CORBA::Octet)source.mType;
    target.searchType = (::CORBA::Octet)source.mSearchType;
    convert(source.mProducerNameList,target.producerNameList);
    target.timezone = CORBA::string_dup(source.mTimezone.c_str());
    target.startTime = source.mStartTime;
    target.endTime = source.mEndTime;
    target.timesteps = source.mTimesteps;
    target.timestepSizeInMinutes = source.mTimestepSizeInMinutes;
    target.analysisTime = CORBA::string_dup(source.mAnalysisTime.c_str());
    convert(source.mGeometryIdList,target.geometryIdList);
    convert(source.mForecastTimeList,target.forecastTimeList);
    target.coordinateType = (::CORBA::Octet)source.mCoordinateType;
    convert(source.mAttributeList,target.attributeList);
    convert(source.mAreaCoordinates,target.areaCoordinates);
    target.radius = source.mRadius;
    convert(source.mQueryParameterList,target.queryParameterList);
    target.language = CORBA::string_dup(source.mLanguage.c_str());
    target.generationFlags = source.mGenerationFlags;
    target.flags = source.mFlags;
    target.maxParameterValues = source.mMaxParameterValues;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






void Converter::convert(const SmartMet::T::ByteData& source, SmartMet::QueryServer::Corba::CorbaByteData& target)
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





void Converter::convert(const SmartMet::QueryServer::Corba::CorbaByteData& source,SmartMet::T::ByteData& target)
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





void Converter::convert(const SmartMet::T::ByteData_vec& source, SmartMet::QueryServer::Corba::CorbaByteDataSequence& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      auto obj = source[t];
      QueryServer::Corba::CorbaByteData corbaObject;
      convert(obj,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const SmartMet::QueryServer::Corba::CorbaByteDataSequence& source,SmartMet::T::ByteData_vec& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      QueryServer::Corba::CorbaByteData corbaObject = source[t];
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





}
}
}

