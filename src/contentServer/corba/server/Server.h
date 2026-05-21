#pragma once

#include "ServerInterface.h"
#include "../../definition/ServiceInterface.h"

#include <pthread.h>


namespace SmartMet
{
namespace ContentServer
{
namespace Corba
{

// ====================================================================================
/*! \brief Hosts a ContentServer ServiceInterface as a CORBA endpoint.
 *
 *  Creates and manages an omniORB ORB and POA, registers a ServerInterface servant,
 *  and runs the CORBA request loop in a dedicated background thread.  Clients obtain
 *  the service IOR via getServiceIor() and use it to construct a ClientImplementation. */
// ====================================================================================
class Server
{
  public:
                  Server(const char *address,const char *port);
    virtual       ~Server();

    virtual void  init(ContentServer::ServiceInterface *serviceInterface);
    virtual void  start();
    virtual void  run();
    virtual void  shutdown();

    std::string   getServiceIor();
    uint          getServerId();

  protected:

    CORBA::ORB_var                  mOrb;             //!< The ORB instance managing this server.
    PortableServer::POA_var         mRootPoa;         //!< Root Portable Object Adapter.
    PortableServer::POA_var         mPoa;             //!< POA that hosts the ContentServer servant.
    PortableServer::POAManager_var  mPman;            //!< POA manager used to activate request processing.
    ServerInterface                 mCorbaInterface;  //!< The CORBA servant that delegates to mServiceInterface.
    ContentServer::ServiceInterface *mServiceInterface;  //!< Non-owning pointer to the local service implementation.
    std::string                     mServiceIor;      //!< Stringified IOR of the registered servant.
    std::string                     mAddress;         //!< Hostname or IP address for the CORBA endpoint.
    std::string                     mPort;            //!< Port number for the CORBA endpoint.
    pthread_t                       mThread;          //!< Handle of the thread running the ORB event loop.

};


}
}
}
