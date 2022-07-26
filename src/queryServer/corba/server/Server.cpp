#include "Server.h"


namespace SmartMet
{
namespace QueryServer
{
namespace Corba
{


static void* Server_maintenanceThread(void *arg)
{
  try
  {
    Server *server = static_cast<Server*>(arg);
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





Server::Server(const char *address,const char *port)
{
  try
  {
    mAddress = address;
    mPort = port;
    mServiceInterface = nullptr;
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





Server::~Server()
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





void Server::init(QueryServer::ServiceInterface *serviceInterface)
{
  try
  {
    mServiceInterface = serviceInterface;
    mCorbaInterface.init(mServiceInterface);

    try
    {
      PortableServer::ObjectId_var service_oid = PortableServer::string_to_ObjectId("QueryService");
      mPoa->activate_object_with_id(service_oid, &mCorbaInterface);

      CORBA::Object_var service_obj = ((QueryServer::Corba::ServerInterface*)&mCorbaInterface)->_this();
      CORBA::String_var service_ior(mOrb->object_to_string(service_obj));

      mServiceIor = (char*)service_ior;

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





void Server::shutdown()
{
  try
  {
    mOrb->shutdown(0);
    pthread_join(mThread, nullptr);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Server::start()
{
  try
  {
    pthread_create(&mThread,nullptr,Server_maintenanceThread,this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Server::run()
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





std::string Server::getServiceIor()
{
  try
  {
    return mServiceIor;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



}
}
}

