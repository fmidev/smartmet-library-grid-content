#pragma once

#include "queryServer/definition/ServiceInterface.h"
#include "contentServer/definition/ServiceInterface.h"
#include "dataServer/definition/ServiceInterface.h"


namespace SmartMet
{
namespace QueryServer
{


class ServiceImplementation : public ServiceInterface
{
  public:
                    ServiceImplementation();
     virtual        ~ServiceImplementation();

     virtual void   init (ContentServer::ServiceInterface *contentServer,DataServer::ServiceInterface *dataServer);
     virtual void   shutdown();

  protected:

     virtual int    _getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::GridPointValueList& valueList);

  private:

     DataServer::ServiceInterface*      getServerPtr(unsigned long long serverFlags);
     void                               checkDataServerConnections();

     ContentServer::ServiceInterface*   mContentServer;
     DataServer::ServiceInterface*      mDataServer;
     DataServer::ServiceInterface*      mDataServerClient[64];
     uint                               mServerCounter;
     time_t                             mServerCheckTime;
};


}
}

