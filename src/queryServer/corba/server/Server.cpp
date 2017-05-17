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
    Server *server = (Server*)arg;
    server->run();
    return NULL;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Operation failed!",NULL);
    exception.printError();
    exit(-1);
  }
}





Server::Server(const char *address,const char *port)
{
  try
  {
    mServiceInterface = NULL;
    int orb_argc = 4;
    char addr[100];
    sprintf(addr,"giop:tcp:%s:%s",address,port);
    char *orb_argv[] = {const_cast<char*>("-ORBendPoint"),const_cast<char*>(addr),const_cast<char*>("-ORBgiopMaxMsgSize"),const_cast<char*>("100000000")};

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
      SmartMet::Spine::Exception exception(BCP,msg);
      throw exception;
    }
    catch(CORBA::Exception& ex)
    {
      char msg[1000];
      sprintf(msg,"Caught CORBA::Exception %s",ex._name());
      SmartMet::Spine::Exception exception(BCP,msg);
      throw exception;
    }
    catch(omniORB::fatalException& fe)
    {
      char msg[1000];
      sprintf(msg,"Caught omniORB:fatalException %s (%s:%d)",fe.errmsg(),fe.file(),fe.line());
      SmartMet::Spine::Exception exception(BCP,msg);
      throw exception;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
      SmartMet::Spine::Exception exception(BCP,msg);
      throw exception;
    }
    catch(CORBA::Exception& ex)
    {
      char msg[1000];
      sprintf(msg,"Caught CORBA::Exception %s",ex._name());
      SmartMet::Spine::Exception exception(BCP,msg);
      throw exception;
    }
    catch(omniORB::fatalException& fe)
    {
      char msg[1000];
      sprintf(msg,"Caught omniORB:fatalException %s (%s:%d)",fe.errmsg(),fe.file(),fe.line());
      SmartMet::Spine::Exception exception(BCP,msg);
      throw exception;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void Server::start()
{
  try
  {
    pthread_create(&mThread,NULL,Server_maintenanceThread,this);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
      SmartMet::Spine::Exception exception(BCP,msg);
      throw exception;
    }
    catch(CORBA::Exception& ex)
    {
      char msg[1000];
      sprintf(msg,"Caught CORBA::Exception %s",ex._name());
      SmartMet::Spine::Exception exception(BCP,msg);
      throw exception;
    }
    catch(omniORB::fatalException& fe)
    {
      char msg[1000];
      sprintf(msg,"Caught omniORB:fatalException %s (%s:%d)",fe.errmsg(),fe.file(),fe.line());
      SmartMet::Spine::Exception exception(BCP,msg);
      throw exception;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}



}
}
}

