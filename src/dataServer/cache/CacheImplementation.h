#pragma once

#include "dataServer/definition/ServiceInterface.h"
#include "PointValueCache.h"


namespace SmartMet
{
namespace DataServer
{



class CacheImplementation : public ServiceInterface
{
  public:
                    CacheImplementation();
     virtual        ~CacheImplementation();

     virtual void   init(ServiceInterface *dataServer);

  protected:

     virtual int    _getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList);
     virtual int    _getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,T::GridCoordinates& coordinates);
     virtual int    _getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data);
     virtual int    _getGridValue(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::ParamValue& value);
     virtual int    _getGridValueList(T::SessionId sessionId,T::ValueRecordList& valueRecordList);
     virtual int    _getGridValuesByArea(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,T::InterpolationMethod interpolationMethod,T::ParamValue_vec& values);

     PointValueCache           mPointValueCache;
     T::GridData_ptr*          mDataCache;
     time_t*                   mDataCacheAccessTime;
     uint                      mDataCacheSize;
     ServiceInterface*         mDataServer;
};


}
}

