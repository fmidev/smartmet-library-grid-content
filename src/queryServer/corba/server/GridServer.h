#pragma once

#include "ServerInterface.h"
#include "../../definition/ServiceInterface.h"
#include "../../../contentServer/definition/ServiceInterface.h"
#include "../../../contentServer/corba/server/ServerInterface.h"
#include "../../../dataServer/definition/ServiceInterface.h"
#include "../../../dataServer/corba/server/ServerInterface.h"

#include <pthread.h>


namespace SmartMet
{
namespace QueryServer
{
namespace Corba
{


class GridServer
{
  public:
                  GridServer(const char *address,const char *port);
    virtual       ~GridServer();

    virtual void  init(ContentServer::ServiceInterface *contentServiceInterface,DataServer::ServiceInterface *dataServiceInterface,QueryServer::ServiceInterface *queryServiceInterface);
    virtual void  start();
    virtual void  run();
    virtual void  shutdown();

    std::string   getContentServiceIor();
    std::string   getDataServiceIor();
    std::string   getQueryServiceIor();

  protected:

    CORBA::ORB_var                        mOrb;
    PortableServer::POA_var               mRootPoa;
    PortableServer::POA_var               mPoa;
    PortableServer::POAManager_var        mPman;
    ContentServer::Corba::ServerInterface mCorbaContentInterface;
    DataServer::Corba::ServerInterface    mCorbaDataInterface;
    QueryServer::Corba::ServerInterface   mCorbaQueryInterface;
    ContentServer::ServiceInterface*      mContentServiceInterface;
    DataServer::ServiceInterface*         mDataServiceInterface;
    QueryServer::ServiceInterface*        mQueryServiceInterface;
    std::string                           mContentServiceIor;
    std::string                           mDataServiceIor;
    std::string                           mQueryServiceIor;
    std::string                           mAddress;
    std::string                           mPort;
    pthread_t                             mThread;

};


}
}
}

