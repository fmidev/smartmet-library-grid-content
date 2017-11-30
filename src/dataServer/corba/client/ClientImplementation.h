#pragma once

#include "dataServer/definition/ServiceInterface.h"
#include "dataServer/corba/stubs/DataServer_serviceInterface_SK.h"


namespace SmartMet
{
namespace DataServer
{
namespace Corba
{


class ClientImplementation : public DataServer::ServiceInterface
{
  public:
                    ClientImplementation();
     virtual        ~ClientImplementation();

     virtual void   init(std::string serviceIor);

  protected:

     virtual int    _getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList);
     virtual int    _getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,T::GridCoordinates& coordinates);
     virtual int    _getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data);
     virtual int    _getGridFileCount(T::SessionId sessionId,uint& count);
     virtual int    _getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::ParamValue& value);
     virtual int    _getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList);
     virtual int    _getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,T::InterpolationMethod interpolationMethod,T::GridValueList& valueList);
     virtual int    _getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList);
     virtual int    _getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,T::GridValueList& valueList);
     virtual int    _getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,T::GridValueList& valueList);
     virtual int    _getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& values);
     virtual int    _getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,T::InterpolationMethod interpolationMethod,T::ParamValue_vec& values);

     virtual int    _getMultipleGridValues(T::SessionId sessionId,T::ValueRecordList& valueRecordList);

  private:

     bool           mInitialized;
     std::string    mServiceIor;

     DataServer::Corba::ServiceInterface_var  mService;
};


}
}
}

