
all: QueryServer_typedefs_SK.h QueryServer_typedefs_SK.cpp QueryServer_serviceInterface_SK.h QueryServer_serviceInterface_SK.cpp

clean:
	rm -f QueryServer_typedefs_SK.h
	rm -f QueryServer_typedefs_SK.cpp
	rm -f QueryServer_serviceInterface_SK.h 
	rm -f QueryServer_serviceInterface_SK.cpp
	rm -f QueryServer_serviceInterface_i.cc

QueryServer_serviceInterface_SK.h: ../idl/QueryServer_serviceInterface.idl
	omniidl  -I../../../contentServer/corba/idl -bcxx -Wbexample -Wbh=_SK.h -Wbs=_SK.cpp ../idl/QueryServer_serviceInterface.idl
	
QueryServer_serviceInterface_SK.cpp: QueryServer_serviceInterface_SK.h

QueryServer_typedefs_SK.h: ../idl/QueryServer_typedefs.idl
	omniidl  -bcxx -Wbh=_SK.h -Wbs=_SK.cpp ../idl/QueryServer_typedefs.idl 
	
QueryServer_typedefs_SK.cpp: QueryServer_typedefs_SK.h
