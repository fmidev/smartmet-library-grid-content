#pragma once

#include "queryServer/definition/ServiceInterface.h"
#include "queryServer/corba/stubs/QueryServer_serviceInterface_SK.h"


namespace SmartMet
{
namespace QueryServer
{
namespace Corba
{


class ClientImplementation : public QueryServer::ServiceInterface
{
  public:
                    ClientImplementation();
     virtual        ~ClientImplementation();

     virtual void   init(std::string serviceIor);

  protected:

     virtual int    _getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::GridPointValueList& valueList);

  private:

     bool           mInitialized;
     std::string    mServiceIor;

     QueryServer::Corba::ServiceInterface_var  mService;
};


}
}
}
