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

    CORBA::ORB_var                  mOrb;
    PortableServer::POA_var         mRootPoa;
    PortableServer::POA_var         mPoa;
    PortableServer::POAManager_var  mPman;
    ServerInterface                 mCorbaInterface;
    ContentServer::ServiceInterface *mServiceInterface;
    std::string                     mServiceIor;
    std::string                     mAddress;
    std::string                     mPort;
    pthread_t                       mThread;

};


}
}
}
