#include "ServiceInterface.h"
#include "QueryConfigurator.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/ShowFunction.h>


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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    Spine::Exception exception(BCP,exception_operation_failed,nullptr);
    exception.printError();
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::getParameterValueByPointAndTime(T::SessionId sessionId,std::string parameter,double x,double y,std::string& timeString,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getParameterValueByPointAndTime(sessionId,parameter,x,y,timeString,value);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%f,%f,%s,%f);result %d;time %f;",__FUNCTION__,sessionId,parameter.c_str(),x,y,timeString.c_str(),value,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




int ServiceInterface::getParameterValuesByPointAndTimeList(T::SessionId sessionId,std::string parameter,double x,double y,std::vector<std::string>& times,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    unsigned long long timeStart = getTime();
    int result = _getParameterValuesByPointAndTimeList(sessionId,parameter,x,y,times,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,%f,%f,Time[%lu],Value[%lu]);result %d;time %f;",__FUNCTION__,sessionId,parameter.c_str(),x,y,times.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::_executeQuery(T::SessionId sessionId,Query& query)
{
  throw Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getProducerList(T::SessionId sessionId,string_vec& producerList)
{
  throw Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::GridPointValueList& valueList)
{
  throw Spine::Exception(BCP,exception_implementation_required);
}





int ServiceInterface::_getParameterValueByPointAndTime(T::SessionId sessionId,std::string parameter,double x,double y,std::string& timeString,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    T::AttributeList attributeList;

    attributeList.addAttribute("starttime",timeString);
    attributeList.addAttribute("timesteps","1");
    attributeList.addAttribute("param",parameter);


    QueryConfigurator configurator;

    Query query;
    configurator.configure(query,attributeList);

    T::Coordinate_vec cc;
    cc.push_back(T::Coordinate(x,y));
    query.mAreaCoordinates.push_back(cc);

    int result = _executeQuery(sessionId,query);
    //query.print(std::cout,0,0);

    if (result != 0)
      return result;


    if (query.mQueryParameterList.size() == 1  &&  query.mQueryParameterList[0].mValueList.size() == 1  &&  query.mQueryParameterList[0].mValueList[0].mValueList.getLength() == 1)
    {
      auto valuePtr = query.mQueryParameterList[0].mValueList[0].mValueList.getGridValuePtrByIndex(0);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int ServiceInterface::_getParameterValuesByPointAndTimeList(T::SessionId sessionId,std::string parameter,double x,double y,std::vector<std::string>& times,T::ParamValue_vec& values)
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
    attributeList.addAttribute("param",parameter);


    QueryConfigurator configurator;

    Query query;
    configurator.configure(query,attributeList);

    T::Coordinate_vec cc;
    cc.push_back(T::Coordinate(x,y));
    query.mAreaCoordinates.push_back(cc);

    int result = _executeQuery(sessionId,query);
    //query.print(std::cout,0,0);

    if (result != 0)
      return result;

    uint len =  query.mForecastTimeList.size();
    if (query.mQueryParameterList.size() == 1  &&  query.mQueryParameterList[0].mValueList.size() == len)
    {
      for (uint t=0; t<len; t++)
      {
        auto valuePtr = query.mQueryParameterList[0].mValueList[t].mValueList.getGridValuePtrByIndex(0);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}



}
}

