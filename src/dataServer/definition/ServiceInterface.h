#pragma once

#include "ValueRecordList.h"
#include "GridData.h"
#include "GridCoordinates.h"
#include "ServiceResults.h"

#include "grid-files/grid/Typedefs.h"
#include "grid-files/common/AttributeList.h"
#include "grid-files/common/Log.h"
#include "contentServer/definition/ServiceInterface.h"


namespace SmartMet
{
namespace DataServer
{



class ServiceInterface
{
  public:
                    ServiceInterface();
     virtual        ~ServiceInterface();

     virtual void   setProcessingLog(Log *processingLogPointer);
     virtual void   shutdown();

     virtual int    getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList);
     virtual int    getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridCoordinates& coordinates);
     virtual int    getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data);
     virtual int    getGridValue(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::ParamValue& value);
     virtual int    getGridValueList(T::SessionId sessionId,T::ValueRecordList& valueRecordList);
     virtual int    getGridValuesByArea(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,T::InterpolationMethod interpolationMethod,T::ParamValue_vec& values);

  protected:

     virtual int    _getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList);
     virtual int    _getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridCoordinates& coordinates);
     virtual int    _getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data);
     virtual int    _getGridValue(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::ParamValue& value);
     virtual int    _getGridValueList(T::SessionId sessionId,T::ValueRecordList& valueRecordList);
     virtual int    _getGridValuesByArea(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,T::InterpolationMethod interpolationMethod,T::ParamValue_vec& values);

     Log            *mProcessingLogPointer;
};


}
}

