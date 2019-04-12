#pragma once

#include "ServiceResults.h"
#include "Query.h"
#include "Alias.h"
#include "ParameterValues.h"
#include "../../contentServer/definition/ContentInfoList.h"

#include <grid-files/grid/Typedefs.h>
#include <grid-files/grid/GridPointValueList.h>
#include <grid-files/common/Log.h>
#include <gis/DEM.h>


namespace SmartMet
{
namespace QueryServer
{

class ServiceInterface
{
  public:
                    ServiceInterface();
     virtual        ~ServiceInterface();

     virtual void   setDebugLog(Log *debugLog);
     virtual void   setProcessingLog(Log *processingLog);
     virtual void   shutdown();
     virtual void   setDem(boost::shared_ptr<Fmi::DEM> dem);

     virtual int    executeQuery(T::SessionId sessionId,Query& query);
     virtual int    getProducerList(T::SessionId sessionId,string_vec& producerList);
     virtual int    getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::GridPointValueList& valueList);

     virtual int    getParameterValueByPointAndTime(T::SessionId sessionId,std::string parameter,double x,double y,std::string& timeString,T::ParamValue& value);
     virtual int    getParameterValuesByPointAndTimeList(T::SessionId sessionId,std::string parameter,double x,double y,std::vector<std::string>& times,T::ParamValue_vec& values);

  protected:

     virtual int    _executeQuery(T::SessionId sessionId,Query& query);
     virtual int    _getProducerList(T::SessionId sessionId,string_vec& producerList);
     virtual int    _getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::GridPointValueList& valueList);

     virtual int    _getParameterValueByPointAndTime(T::SessionId sessionId,std::string parameter,double x,double y,std::string& timeString,T::ParamValue& value);
     virtual int    _getParameterValuesByPointAndTimeList(T::SessionId sessionId,std::string parameter,double x,double y,std::vector<std::string>& times,T::ParamValue_vec& values);

     Log            *mDebugLog;
     Log            *mProcessingLog;
};


}
}

