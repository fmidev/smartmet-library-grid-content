#pragma once

#include "../../definition/ServiceInterface.h"
#include "ServerInterface.h"
#include <pthread.h>


namespace SmartMet
{
namespace DataServer
{
namespace Corba
{


// ====================================================================================
/*! \brief CORBA server host that exposes a DataServer::ServiceInterface over the network.
 *
 *  Initialises an omniORB ORB, registers a ServerInterface servant, and runs
 *  the ORB event loop in a background thread.  Callers retrieve the published
 *  IOR string with getServiceIor() and pass it to remote ClientImplementation
 *  instances. */
// ====================================================================================

class Server
{
  public:
                  Server(const char *address,const char *port);
    virtual       ~Server();
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    virtual void  init(DataServer::ServiceInterface *serviceInterface);
    virtual void  start();
    virtual void  run();
    virtual void  shutdown();

    std::string   getServiceIor();

  protected:

    CORBA::ORB_var                  mOrb;             //!< The omniORB ORB instance.
    PortableServer::POA_var         mRootPoa;          //!< Root Portable Object Adapter.
    PortableServer::POA_var         mPoa;              //!< POA used to activate the servant.
    PortableServer::POAManager_var  mPman;             //!< POA manager that controls request dispatching.
    ServerInterface                 mCorbaInterface;   //!< The CORBA servant that wraps mServiceInterface.
    DataServer::ServiceInterface*   mServiceInterface; //!< Non-owning pointer to the local DataServer implementation.
    std::string                     mServiceIor;       //!< Stringified IOR published by this server.
    pthread_t                       mThread = 0;           //!< Background thread running the ORB event loop.

};


}
}
}

