#include "ServiceInterface.h"
#include "QueryConfigurator.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/ShowFunction.h>
#include <macgyver/StringConversion.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace QueryServer
{


ServiceInterface::ServiceInterface()
{
  FUNCTION_TRACE
  try
  {
    mDebugLog = nullptr;
    mProcessingLog = nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ServiceInterface::~ServiceInterface()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Operation failed!",nullptr);
    exception.printError();
  }
}





Log* ServiceInterface::getDebugLog()
{
  FUNCTION_TRACE
  try
  {
    return mDebugLog;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




Log* ServiceInterface::getProcessingLog()
{
  FUNCTION_TRACE
  try
  {
    return mProcessingLog;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceInterface::setDebugLog(Log *debugLog)
{
  FUNCTION_TRACE
  try
  {
    mDebugLog = debugLog;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceInterface::setProcessingLog(Log *processingLog)
{
  FUNCTION_TRACE
  try
  {
    mProcessingLog = processingLog;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void  ServiceInterface::setDem(boost::shared_ptr<Fmi::DEM> dem)
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceInterface::setLandCover(boost::shared_ptr<Fmi::LandCover> landCover)
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceInterface::shutdown()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::executeQuery(T::SessionId sessionId,Query& query)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _executeQuery(sessionId,query);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,Query);result %d;time %f;",__FUNCTION__,sessionId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getProducerList(T::SessionId sessionId,string_vec& producerList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getProducerList(sessionId,producerList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,producer[%lu]);result %d;time %f;",__FUNCTION__,sessionId,producerList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::GridPointValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getValuesByGridPoint(sessionId,contentInfoList,coordinateType,x,y,interpolationMethod,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,ContentInfo[%u],%u,%f,%f,%u,GridPointValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,contentInfoList.getLength(),coordinateType,x,y,interpolationMethod,valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getParameterValueByPointAndTime(T::SessionId sessionId,std::string producer,std::string parameter,T::CoordinateType coordinateType,double x,double y,std::string& timeString,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getParameterValueByPointAndTime(sessionId,producer,parameter,coordinateType,x,y,timeString,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,value);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%s,%u,%f,%f,%s,%d,%d,%d,%f);result %d;time %f;",__FUNCTION__,sessionId,producer.c_str(),parameter.c_str(),coordinateType,x,y,timeString.c_str(),areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,value,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getParameterValuesByPointListAndTime(T::SessionId sessionId,std::string producer,std::string parameter,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,std::string& timeString,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,std::vector<T::ParamValue>& valueList)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getParameterValuesByPointListAndTime(sessionId,producer,parameter,coordinateType,coordinates,timeString,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%s,%u,Coordinate[%lu],%s,%d,%d,%d,Value[%lu]);result %d;time %f;",__FUNCTION__,sessionId,producer.c_str(),parameter.c_str(),coordinateType,coordinates.size(),timeString.c_str(),areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,valueList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getParameterValuesByPointAndTimeList(T::SessionId sessionId,std::string producer,std::string parameter,T::CoordinateType coordinateType,double x,double y,std::vector<std::string>& times,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getParameterValuesByPointAndTimeList(sessionId,producer,parameter,coordinateType,x,y,times,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%s,%u,%f,%f,Time[%lu],%d,%d,%d,Value[%lu]);result %d;time %f;",__FUNCTION__,sessionId,producer.c_str(),parameter.c_str(),coordinateType,x,y,times.size(),areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getParameterValueVectorByGeometryAndTime(T::SessionId sessionId,std::string producer,std::string parameter,std::string& timeString,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getParameterValueVectorByGeometryAndTime(sessionId,producer,parameter,timeString,attributeList,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%s,%s,Attribute[%u],Value[%lu]);result %d;time %f;",__FUNCTION__,sessionId,producer.c_str(),parameter.c_str(),timeString.c_str(),attributeList.getLength(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_executeQuery(T::SessionId sessionId,Query& query)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getProducerList(T::SessionId sessionId,string_vec& producerList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::GridPointValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getParameterValueByPointAndTime(T::SessionId sessionId,std::string producer,std::string parameter,T::CoordinateType coordinateType,double x,double y,std::string& timeString,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    T::AttributeList attributeList;

    attributeList.addAttribute("starttime",timeString);
    attributeList.addAttribute("timesteps","1");
    attributeList.addAttribute("producer",producer);
    attributeList.addAttribute("param",parameter);
    attributeList.addAttribute("areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));
    attributeList.addAttribute("timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
    attributeList.addAttribute("levelInterpolationMethod",Fmi::to_string(levelInterpolationMethod));

    QueryConfigurator configurator;

    Query query;
    configurator.configure(query,attributeList);

    query.mCoordinateType = coordinateType;
    T::Coordinate_vec cc;
    cc.push_back(T::Coordinate(x,y));
    query.mAreaCoordinates.push_back(cc);

    int result = _executeQuery(sessionId,query);
    if (result != 0)
      return result;

    if (query.mQueryParameterList.size() == 1  &&  query.mQueryParameterList[0].mValueList.size() == 1  &&  (*query.mQueryParameterList[0].mValueList.begin())->mValueList.getLength() == 1)
    {
      auto valuePtr = (*query.mQueryParameterList[0].mValueList.begin())->mValueList.getGridValuePtrByIndex(0);
      if (valuePtr != nullptr)
      {
        value = valuePtr->mValue;
        return 0;
      }
    }

    value = ParamValueMissing;
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getParameterValuesByPointListAndTime(T::SessionId sessionId,std::string producer,std::string parameter,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,std::string& timeString,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,std::vector<T::ParamValue>& valueList)
{
  FUNCTION_TRACE
  try
  {
    T::AttributeList attributeList;

    attributeList.addAttribute("starttime",timeString);
    attributeList.addAttribute("timesteps","1");
    attributeList.addAttribute("producer",producer);
    attributeList.addAttribute("param",parameter);
    attributeList.addAttribute("areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));
    attributeList.addAttribute("timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
    attributeList.addAttribute("levelInterpolationMethod",Fmi::to_string(levelInterpolationMethod));

    QueryConfigurator configurator;

    Query query;
    configurator.configure(query,attributeList);

    query.mCoordinateType = coordinateType;
    query.mAreaCoordinates.push_back(coordinates);

    int result = _executeQuery(sessionId,query);
    if (result != 0)
      return result;

    if (query.mQueryParameterList.size() == 1  &&  query.mQueryParameterList[0].mValueList.size() == 1  &&  (*query.mQueryParameterList[0].mValueList.begin())->mValueList.getLength() == coordinates.size())
    {
      uint len = (*query.mQueryParameterList[0].mValueList.begin())->mValueList.getLength();
      for (uint t=0; t<len; t++)
      {
        auto valuePtr = (*query.mQueryParameterList[0].mValueList.begin())->mValueList.getGridValuePtrByIndex(t);
        if (valuePtr != nullptr)
          valueList.push_back(valuePtr->mValue);
        else
          valueList.push_back(ParamValueMissing);
      }
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getParameterValuesByPointAndTimeList(T::SessionId sessionId,std::string producer,std::string parameter,T::CoordinateType coordinateType,double x,double y,std::vector<std::string>& times,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    T::AttributeList attributeList;

    char tmp[10000];
    char *p = tmp;
    for (auto it = times.begin(); it != times.end(); ++it)
    {
      if (p != tmp)
        p += sprintf(p,",");

      p += sprintf(p,"%s",it->c_str());
    }

    attributeList.addAttribute("timelist",tmp);
    attributeList.addAttribute("producer",producer);
    attributeList.addAttribute("param",parameter);
    attributeList.addAttribute("areaInterpolationMethod",Fmi::to_string(areaInterpolationMethod));
    attributeList.addAttribute("timeInterpolationMethod",Fmi::to_string(timeInterpolationMethod));
    attributeList.addAttribute("levelInterpolationMethod",Fmi::to_string(levelInterpolationMethod));

    QueryConfigurator configurator;

    Query query;
    configurator.configure(query,attributeList);
    query.mCoordinateType = coordinateType;

    T::Coordinate_vec cc;
    cc.push_back(T::Coordinate(x,y));
    query.mAreaCoordinates.push_back(cc);

    int result = _executeQuery(sessionId,query);
    if (result != 0)
      return result;

    uint len =  query.mForecastTimeList.size();
    if (query.mQueryParameterList.size() == 1  &&  query.mQueryParameterList[0].mValueList.size() == len)
    {
      for (auto it = query.mQueryParameterList[0].mValueList.begin(); it != query.mQueryParameterList[0].mValueList.end(); ++it)
      {
        auto valuePtr = (*it)->mValueList.getGridValuePtrByIndex(0);
        if (valuePtr != nullptr)
          values.push_back(valuePtr->mValue);
        else
          values.push_back(ParamValueMissing);
      }
    }
    else
    {
      for (uint t=0; t<len; t++)
      {
        values.push_back(ParamValueMissing);
      }
    }

    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getParameterValueVectorByGeometryAndTime(T::SessionId sessionId,std::string producer,std::string parameter,std::string& timeString,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    T::AttributeList qAttributeList;

    attributeList.addAttribute("starttime",timeString);
    attributeList.addAttribute("timesteps","1");
    attributeList.addAttribute("producer",producer);
    attributeList.addAttribute("param",parameter);

    QueryConfigurator configurator;

    Query query;
    configurator.configure(query,qAttributeList);

    uint len = attributeList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::Attribute *attr = attributeList.getAttributeByIndex(t);
      if (attr != nullptr)
        query.mAttributeList.addAttribute(attr->mName,attr->mValue);
    }


    int result = _executeQuery(sessionId,query);
    if (result != 0)
      return result;


    if (query.mQueryParameterList.size() == 1  &&  query.mQueryParameterList[0].mValueList.size() == 1  &&  (*query.mQueryParameterList[0].mValueList.begin())->mValueList.getLength() == 1)
    {
      values = (*query.mQueryParameterList[0].mValueList.begin())->mValueVector;
    }

    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}

}
}

