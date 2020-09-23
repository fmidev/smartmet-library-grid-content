#include "GridServer.h"


namespace SmartMet
{
namespace QueryServer
{
namespace Corba
{


static void* GridServer_maintenanceThread(void *arg)
{
  try
  {
    GridServer *server = static_cast<GridServer*>(arg);
    server->run();
    return nullptr;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Operation failed!",nullptr);
    exception.printError();
    exit(-1);
  }
}





GridServer::GridServer(const char *address,const char *port)
{
  try
  {
    mAddress = address;
    mPort = port;
    mContentServiceInterface = nullptr;
    mDataServiceInterface = nullptr;
    mQueryServiceInterface = nullptr;
    int orb_argc = 4;
    char addr[100];
    sprintf(addr,"giop:tcp:%s:%s",address,port);
    char *orb_argv[] = {const_cast<char*>("-ORBendPoint"),const_cast<char*>(addr),const_cast<char*>("-ORBgiopMaxMsgSize"),const_cast<char*>("500000000")};

    try
    {
      mOrb = CORBA::ORB_init(orb_argc,orb_argv);

      CORBA::Object_var obj = mOrb->resolve_initial_references("RootPOA");
      mRootPoa = PortableServer::POA::_narrow(obj);
      mPman = mRootPoa->the_POAManager();
      mPman->activate();


      // Create a new POA with the persistent lifespan policy.
      CORBA::PolicyList pl;
      pl.length(2);
      pl[0] = mRootPoa->create_lifespan_policy(PortableServer::PERSISTENT);
      pl[1] = mRootPoa->create_id_assignment_policy(PortableServer::USER_ID);

      mPoa = mRootPoa->create_POA("my poa", mPman, pl);
    }

    catch(CORBA::SystemException& ex)
    {
      char msg[1000];
      sprintf(msg,"Caught CORBA::System Exception %s",ex._name());
      Fmi::Exception exception(BCP,msg);
      throw exception;
    }
    catch(CORBA::Exception& ex)
    {
      char msg[1000];
      sprintf(msg,"Caught CORBA::Exception %s",ex._name());
      Fmi::Exception exception(BCP,msg);
      throw exception;
    }
    catch(omniORB::fatalException& fe)
    {
      char msg[1000];
      sprintf(msg,"Caught omniORB:fatalException %s (%s:%d)",fe.errmsg(),fe.file(),fe.line());
      Fmi::Exception exception(BCP,msg);
      throw exception;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





GridServer::~GridServer()
{
  try
  {
    mOrb->destroy();
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void GridServer::init(ContentServer::ServiceInterface *contentServiceInterface,DataServer::ServiceInterface *dataServiceInterface,QueryServer::ServiceInterface *queryServiceInterface)
{
  try
  {

    mContentServiceInterface = contentServiceInterface;
    mCorbaContentInterface.init(mContentServiceInterface);

    mDataServiceInterface = dataServiceInterface;
    mCorbaDataInterface.init(mDataServiceInterface);

    mQueryServiceInterface = queryServiceInterface;
    mCorbaQueryInterface.init(mQueryServiceInterface);

    try
    {
      PortableServer::ObjectId_var content_service_oid = PortableServer::string_to_ObjectId("ContentService");
      mPoa->activate_object_with_id(content_service_oid, &mCorbaContentInterface);

      CORBA::Object_var content_service_obj = ((ContentServer::Corba::ServerInterface*)&mCorbaContentInterface)->_this();
      CORBA::String_var content_service_ior(mOrb->object_to_string(content_service_obj));
      mCorbaContentInterface._remove_ref();

      mContentServiceIor = (char*)content_service_ior;



      PortableServer::ObjectId_var data_service_oid = PortableServer::string_to_ObjectId("DataService");
      mPoa->activate_object_with_id(data_service_oid, &mCorbaDataInterface);

      CORBA::Object_var data_service_obj = ((DataServer::Corba::ServerInterface*)&mCorbaDataInterface)->_this();
      CORBA::String_var data_service_ior(mOrb->object_to_string(data_service_obj));
      mCorbaDataInterface._remove_ref();

      mDataServiceIor = (char*)data_service_ior;



      PortableServer::ObjectId_var query_service_oid = PortableServer::string_to_ObjectId("QueryService");
      mPoa->activate_object_with_id(query_service_oid, &mCorbaQueryInterface);

      CORBA::Object_var query_service_obj = ((QueryServer::Corba::ServerInterface*)&mCorbaQueryInterface)->_this();
      CORBA::String_var query_service_ior(mOrb->object_to_string(query_service_obj));
      mCorbaQueryInterface._remove_ref();

      mQueryServiceIor = (char*)query_service_ior;


      // std::cout << (char*)service_ior << "\n";
    }
    catch(CORBA::SystemException& ex)
    {
      char msg[1000];
      sprintf(msg,"Caught CORBA::System Exception %s",ex._name());
      Fmi::Exception exception(BCP,msg);
      throw exception;
    }
    catch(CORBA::Exception& ex)
    {
      char msg[1000];
      sprintf(msg,"Caught CORBA::Exception %s",ex._name());
      Fmi::Exception exception(BCP,msg);
      throw exception;
    }
    catch(omniORB::fatalException& fe)
    {
      char msg[1000];
      sprintf(msg,"Caught omniORB:fatalException %s (%s:%d)",fe.errmsg(),fe.file(),fe.line());
      Fmi::Exception exception(BCP,msg);
      throw exception;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void GridServer::shutdown()
{
  try
  {
    mOrb->shutdown(0);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void GridServer::start()
{
  try
  {
    pthread_create(&mThread,nullptr,GridServer_maintenanceThread,this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void GridServer::run()
{
  try
  {
    try
    {
      mOrb->run();
    }
    catch(CORBA::SystemException& ex)
    {
      char msg[1000];
      sprintf(msg,"Caught CORBA::System Exception %s",ex._name());
      Fmi::Exception exception(BCP,msg);
      throw exception;
    }
    catch(CORBA::Exception& ex)
    {
      char msg[1000];
      sprintf(msg,"Caught CORBA::Exception %s",ex._name());
      Fmi::Exception exception(BCP,msg);
      throw exception;
    }
    catch(omniORB::fatalException& fe)
    {
      char msg[1000];
      sprintf(msg,"Caught omniORB:fatalException %s (%s:%d)",fe.errmsg(),fe.file(),fe.line());
      Fmi::Exception exception(BCP,msg);
      throw exception;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::string GridServer::getContentServiceIor()
{
  try
  {
    return mContentServiceIor;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::string GridServer::getDataServiceIor()
{
  try
  {
    return mDataServiceIor;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::string GridServer::getQueryServiceIor()
{
  try
  {
    return mQueryServiceIor;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



}
}
}

