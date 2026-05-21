#pragma once

#include "../../definition/ServiceInterface.h"
#include "../stubs/QueryServer_serviceInterface_SK.h"


namespace SmartMet
{
namespace QueryServer
{
namespace Corba
{

// ====================================================================================
/*! \brief CORBA client proxy that forwards QueryServer calls to a remote server.
 *
 *  Implements QueryServer::ServiceInterface by serialising each request over CORBA
 *  to the remote service identified by the IOR supplied to init(). */
// ====================================================================================

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

     bool           mInitialized;  //!< True after init() has successfully resolved the IOR.
     std::string    mServiceIor;   //!< CORBA IOR string used to locate the remote QueryServer service.

     QueryServer::Corba::ServiceInterface_var  mService;  //!< CORBA object reference to the remote service.
};


}
}
}
