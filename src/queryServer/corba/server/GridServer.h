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

// ====================================================================================
/*! \brief Composite CORBA server that exposes ContentServer, DataServer, and QueryServer over a single ORB.
 *
 *  Activates three CORBA servants on the same POA so that all three service layers
 *  are reachable from a single address/port pair.  IORs for each service are
 *  available after start() returns. */
// ====================================================================================

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

    CORBA::ORB_var                        mOrb;                      //!< CORBA ORB instance.
    PortableServer::POA_var               mRootPoa;                  //!< Root Portable Object Adapter.
    PortableServer::POA_var               mPoa;                      //!< Child POA used to activate the servants.
    PortableServer::POAManager_var        mPman;                     //!< POA manager controlling request processing.
    ContentServer::Corba::ServerInterface mCorbaContentInterface;    //!< CORBA servant for the ContentServer layer.
    DataServer::Corba::ServerInterface    mCorbaDataInterface;       //!< CORBA servant for the DataServer layer.
    QueryServer::Corba::ServerInterface   mCorbaQueryInterface;      //!< CORBA servant for the QueryServer layer.
    ContentServer::ServiceInterface*      mContentServiceInterface;  //!< Local ContentServer implementation.
    DataServer::ServiceInterface*         mDataServiceInterface;     //!< Local DataServer implementation.
    QueryServer::ServiceInterface*        mQueryServiceInterface;    //!< Local QueryServer implementation.
    std::string                           mContentServiceIor;        //!< Published IOR for the ContentServer servant.
    std::string                           mDataServiceIor;           //!< Published IOR for the DataServer servant.
    std::string                           mQueryServiceIor;          //!< Published IOR for the QueryServer servant.
    std::string                           mAddress;                  //!< Network address the ORB listens on.
    std::string                           mPort;                     //!< Port number the ORB listens on.
    pthread_t                             mThread;                   //!< Background thread running the ORB event loop.

};


}
}
}

