#pragma once

#include "ServiceResults.h"
#include "Query.h"
#include "Alias.h"
#include "ParameterValues.h"
#include "../../contentServer/definition/ContentInfoList.h"

#include <grid-files/grid/Typedefs.h>
#include <grid-files/grid/GridPointValueList.h>
#include <grid-files/common/Log.h>
#include <grid-files/common/AttributeList.h>
#include <gis/DEM.h>
#include <gis/LandCover.h>


namespace SmartMet
{
namespace QueryServer
{

class ServiceInterface
{
  public:
                    ServiceInterface();
     virtual        ~ServiceInterface();

     virtual Log*   getDebugLog();
     virtual Log*   getProcessingLog();
     virtual void   setDebugLog(Log *debugLog);
     virtual void   setProcessingLog(Log *processingLog);

     virtual void   setEnabled(bool enabled);
     virtual bool   isEnabled();

     virtual void   shutdown();
     virtual void   setDem(boost::shared_ptr<Fmi::DEM> dem);
     virtual void   setLandCover(boost::shared_ptr<Fmi::LandCover> landCover);

     virtual int    executeQuery(T::SessionId sessionId,Query& query);
     virtual int    getProducerList(T::SessionId sessionId,string_vec& producerList);
     virtual int    getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::GridPointValueList& valueList);

     virtual int    getParameterValueByPointAndTime(T::SessionId sessionId,const std::string& producer,const std::string& parameter,T::CoordinateType coordinateType,double x,double y,const std::string& timeString,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::ParamValue& value);
     virtual int    getParameterValuesByPointListAndTime(T::SessionId sessionId,const std::string& producer,const std::string& parameter,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,const std::string& timeString,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,std::vector<T::ParamValue>& valueList);
     virtual int    getParameterValuesByPointAndTimeList(T::SessionId sessionId,const std::string& producer,const std::string& parameter,T::CoordinateType coordinateType,double x,double y,std::vector<std::string>& times,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::ParamValue_vec& values);

     virtual int    getParameterValueVectorByGeometryAndTime(T::SessionId sessionId,const std::string& producer,const std::string& parameter,const std::string& timeString,T::AttributeList& attributeList,T::ParamValue_vec& values);

  protected:

     virtual int    _executeQuery(T::SessionId sessionId,Query& query);
     virtual int    _getProducerList(T::SessionId sessionId,string_vec& producerList);
     virtual int    _getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::GridPointValueList& valueList);

     virtual int    _getParameterValueByPointAndTime(T::SessionId sessionId,const std::string& producer,const std::string& parameter,T::CoordinateType coordinateType,double x,double y,const std::string& timeString,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::ParamValue& value);
     virtual int    _getParameterValuesByPointListAndTime(T::SessionId sessionId,const std::string& producer,const std::string& parameter,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,const std::string& timeString,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,std::vector<T::ParamValue>& valueList);
     virtual int    _getParameterValuesByPointAndTimeList(T::SessionId sessionId,const std::string& producer,const std::string& parameter,T::CoordinateType coordinateType,double x,double y,std::vector<std::string>& times,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::ParamValue_vec& values);

     virtual int    _getParameterValueVectorByGeometryAndTime(T::SessionId sessionId,const std::string& producer,const std::string& parameter,const std::string& timeString,T::AttributeList& attributeList,T::ParamValue_vec& values);

     bool           mEnabled;
     Log            *mDebugLog;
     Log            *mProcessingLog;
};


}
}

