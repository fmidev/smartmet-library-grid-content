#pragma once

#include "dataServer/definition/ServiceInterface.h"
#include "dataServer/corba/server/ServerInterface.h"
#include <pthread.h>


namespace SmartMet
{
namespace DataServer
{
namespace Corba
{


class Server
{
  public:
                  Server(const char *address,const char *port);
    virtual       ~Server();

    virtual void  init(DataServer::ServiceInterface *serviceInterface);
    virtual void  start();
    virtual void  run();
    virtual void  shutdown();

    std::string   getServiceIor();

  protected:

    CORBA::ORB_var                  mOrb;
    PortableServer::POA_var         mRootPoa;
    PortableServer::POA_var         mPoa;
    PortableServer::POAManager_var  mPman;
    ServerInterface            mCorbaInterface;
    DataServer::ServiceInterface*   mServiceInterface;
    std::string                     mServiceIor;
    pthread_t                       mThread;

};


}
}
}

