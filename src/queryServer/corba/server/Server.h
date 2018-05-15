#pragma once

#include "queryServer/definition/ServiceInterface.h"
#include "queryServer/corba/server/ServerInterface.h"
#include <pthread.h>


namespace SmartMet
{
namespace QueryServer
{
namespace Corba
{


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

    CORBA::ORB_var                  mOrb;
    PortableServer::POA_var         mRootPoa;
    PortableServer::POA_var         mPoa;
    PortableServer::POAManager_var  mPman;
    ServerInterface                 mCorbaInterface;
    QueryServer::ServiceInterface*  mServiceInterface;
    std::string                     mServiceIor;
    std::string                     mAddress;
    std::string                     mPort;
    pthread_t                       mThread;

};


}
}
}

