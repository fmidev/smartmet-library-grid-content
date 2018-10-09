Grid Content Library
====================
## Introduction

This library is used by the SmartMet Server grid support. The library defines three different service APIs (Application Programming Interfaces) and one or more implementations for each of these APIs. In practice, service API definitions are C++ class definitions, that contain service method definitions, but not their implementations. The actual implementations are inherited from these service classes. In this way, we can have multiple different implementations for the same service interface.

## Content Server API

The Content Server API is used in order to keep track on available grid files and general information related to them. So, technically it knows the grid filenames and their content (i.e. does the file contain wind, pressure,temperature, rain, etc. information). It also knows the producers and the generations where these files belong to. 

The "**ContentServer::ServiceInterface**" class defines the methods that should be available in the actual service implementation. Notice that we have several implementations for the current interface:
     
* The "**ContentServer::RedisImplementation**" class implements the methods of the current service interface by using the Redis database. In other words, it stores all the content related information into the Redis database (= permanent data storage).
        
* The "**ContentServer::CacheImplementation**" class implements the methods of the current service interface by using internal memory structures in order to storage the information. This implementation requires that there is "master data source" available that it can monitor. Usually this means, that the cache implementation uses the Redis implementation as its master data source. By monitoring the events in the master data source it can keeps its content up to data all the time. The cache implementation is optimized for fast information queries. That's why it is usually many times faster than the Redis implementation.
        
* The "**ContentServer::Corba::ClientImplementation**" class can be used in order to access other service implementations over the network. For example, if we would like to access a cache implementation running in a different computer, we need a "communication channel" that allows us remotely call the cache implementation in another computer. In this case the communication between the application and the cache implementation follows the path:  
        
     Appl ---> ClientImpl ---> CorbaClientInterface ---> CorbaServerInterface ---> CacheImpl
      
* As long as the ClientImplementation and CacheImplementation are inherited from the same ServiceInterface class, the actual application does not necessary do not know weather it is using the Cache implementation directly or over the network. This kind of service usage over the network is quite common for applications that just wants to use services without installing them into the same computer.
        
* It is good to realize that when services are used over a network, there must be always a server program in place. In this case we have defined the Corba communication interface by using Interface Description Language (IDL). It contains the similar service methods as the original ContentServer::ServiceInterface class, which means that service request and response information can be easily converter into this communication channel. The CorbaClientInterface and the CorbaServerInterface are generated automatically from this IDL description.
        
* It is also good to notice that in spite of that all service implementations are written in C++, all the services can be accessed from several other languages (like Java) via the CorbaServiceInterface. For example, accessing services from Java requires that the current IDL description is used in order to generate CorbaClientInterface classes for java. This can be done with the "idlj" program that is a part of the standard JDK.
        
* The "**ContentServer::HTTP::ClientImplementation**" class can be used in order to access other service implementations over the network. It is actually quite similar implementation than Corba::ClientImplementation. The only difference is that it uses HTTP protocol for the communication instead of IIOP/GIOP. This implementation is usually slower than Corba implementation, but it is useful if there is  a risk that a firewall blocks the Corba communication.
      
        
## Data Server API        
       
The Data Server API is used in order to access the actual grid files and the data inside them. The point is that you first use tge Content Server API in order to find grid files that you want to access (for example, you might want to find grid files that contain temperature forecasts in the given time range). After you have found the correct grid files, you can start to fetch information from them by using the Data Server API. For example, you might want to fetch values from certain coordinate points or geographical areas. 
        
The "**DataServer::ServiceInterface**" class defines the methods that should be available in the actual service implementation. Notice that we have several implementations for the current interface.
         
* The "**DataServer::ServiceImplementation**" class is the main implementation of the current service API. It listens the events of the Content Server API and this way knows which grid files should be available and which not. When data is requested from a grid file, the current file is memory mapped (=> virtual memory) so that it can be accessed as fast as possible. Notice that all the grid files defined in the ContentServer should be accessible from the current data server implementation. If not, the DataServer cannot return the requested information.
        
* The "**DataServer::Corba::ClientImplementation**" class can be used in order to access the Data Server API implementations over the network. This usually makes sense if the grid files cannot be locally accessed (= mounted) or if we just do not want to memory map grid files into our local computer. This kind of service usage over the network is quite common for applications that just want to use services without installing them into the same computer.        
        
* It is also good to notice that as long as the current implementation uses the CorbaServerInterface, it allows the usage of the DataServer also from other programming languages.        
        
* The DataServer::CacheImplementation can be used for caching information that is fetched from the grid files. Usually this implementation is used if the "**DataServer::ServiceImplementation**" is located in another computer. The cache implementation first check if the information can be found from its local cache, before it actually tries to fetch it over the network. 
        
     Appl->CacheImpl->ClientImp->CorbaClientInterface->CorbaServerInterface->ServiceImpl
        
## Query Server API
        
The Query Server API is used in order to query information from the grid files. This service needs the Content Server API in order to find correct grid files. It also needs the Data Server API in order to fetch requested data from these grid files.
     
* A Query contains 1) a list of parameters which value should be fetched 2a) over the given time interval or 2b) in the given time steps 3a) in the given coordinate points or 3b) or in the given geographical area. 

* A query can also contain a list of producers in order to define the search order of the requested data. If no producers are defined then the default search order is used. 

* A query can can also contain functions that allow mathematical operations over the queried parameter values.
         
The "**QueryServer::ServiceInterface**" class defines the methods that should be available in the actual service implementation. Notice that we have several implementations for the current interface.
         
* The "**QueryServer::ServiceImplementation**" class is the main implementation of the current service. The functions used in the query parameters are defined in the LUA programming language. This means in practice that new functions can be dynamically added into the service without a need for the restart. 

* The "**QueryServer::Corba::ClientImplementation**" class can be used in order to access other service implementations over the network. This kind of service usage over the network is quite common for applications that just want to use services without installing them into the same computer.
        
* It is also good to notice that as long as the current implementation uses the CorbaServerInterface, it allows the usage of the DataServer also from other programming languages.        

## About the implementation

All service APIs are defined in the similar way. Usually the interface contains method-pairs which has almost the same name. The first method in the pair is public and the second method is protected. When a service user calls the public method, the public method calls the protected method. For example:

  serviceInterface.getProducerInfo() ==> serviceInterface._getProducerInfo() 

The idea is that each implementation overrides the protected method, which means that the public method stays same in all implementations. This allows us to "observe" service calls in different implementations. 

For example, we might measure the time used for each service call and write this information into the log file. So, it does not matter weather we have the actual implementation, a cache implementation or a client implementation - they all have the same "observation" and "logging" capabilities.

## Architecture

The Grid Content Library enables flexibe usage of diffent kind of service API implementations. These implementations can be used as parts of the other components or they can be as separated distributed components, which is show in the figure below.

![Image](https://github.com/fmidev/smartmet-tools-grid/blob/master/doc/grid-support-img/grid-support-img7.png "gui")


## Licence
The Grid Content Library is a part of the SmartMet Server (https://github.com/fmidev/smartmet-server), which is published with MIT-license.

## How to contribute
Found a bug? Want to implement a new feature? Your contribution is very welcome!

Small changes and bug fixes can be submitted via pull request. In larger contributions, premilinary plan is recommended (in GitHub wiki). 

CLA is required in order to contribute. Please contact us for more information!

## Documentation
SmartMet Server Grid support (https://github.com/fmidev/smartmet-tools-grid/raw/master/doc/grid-support.pdf)


## Communication and Resources
You may contact us from following channels:
* Email: beta@fmi.fi
* Facebook: https://www.facebook.com/fmibeta/
* GitHub: [issues](../../issues)

Other resources which may be useful:  
* Presentation about the server: http://www.slideshare.net/tervo/smartmet-server-providing-metocean-data  
* Our public web pages (in Finnish): http://ilmatieteenlaitos.fi/avoin-lahdekoodi   




