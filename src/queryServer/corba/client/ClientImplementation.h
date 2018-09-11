#pragma once

#include "../../definition/ServiceInterface.h"
#include "../stubs/QueryServer_serviceInterface_SK.h"


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

     virtual void   init(const std::string& serviceIor);

  protected:

     virtual int    _executeQuery(T::SessionId sessionId,Query& query);
     virtual int    _getProducerList(T::SessionId sessionId,string_vec& producerList);
     virtual int    _getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::GridPointValueList& valueList);

  private:

     bool           mInitialized;
     std::string    mServiceIor;

     QueryServer::Corba::ServiceInterface_var  mService;
};


}
}
}
