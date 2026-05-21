#pragma once

#include "ServerInterface.h"
#include "../../definition/ServiceInterface.h"

#include <pthread.h>


namespace SmartMet
{
namespace QueryServer
{
namespace Corba
{

// ====================================================================================
/*! \brief Standalone CORBA server that publishes a single QueryServer service endpoint.
 *
 *  Binds an ORB to the given address and port, activates the ServerInterface servant,
 *  and runs the request loop in a dedicated background thread. */
// ====================================================================================

class Server
{
  public:
                  Server(const char *address,const char *port);
    virtual       ~Server();

    virtual void  init(QueryServer::ServiceInterface *serviceInterface);
    virtual void  start();
    virtual void  run();
    virtual void  shutdown();

    std::string   getServiceIor();

  protected:

    CORBA::ORB_var                  mOrb;              //!< CORBA ORB instance.
    PortableServer::POA_var         mRootPoa;          //!< Root Portable Object Adapter.
    PortableServer::POA_var         mPoa;              //!< Child POA used to activate the servant.
    PortableServer::POAManager_var  mPman;             //!< POA manager that controls request processing.
    ServerInterface                 mCorbaInterface;   //!< CORBA servant wrapping the local service.
    QueryServer::ServiceInterface*  mServiceInterface; //!< Local QueryServer service implementation.
    std::string                     mServiceIor;       //!< IOR string published after servant activation.
    std::string                     mAddress;          //!< Network address the ORB listens on.
    std::string                     mPort;             //!< Port number the ORB listens on.
    pthread_t                       mThread;           //!< Background thread running the ORB event loop.

};


}
}
}

