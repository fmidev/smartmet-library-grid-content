

all: DataServer_typedefs_SK.h DataServer_typedefs_SK.cpp DataServer_serviceInterface_SK.h DataServer_serviceInterface_SK.cpp

clean:
	rm -f DataServer_typedefs_SK.h
	rm -f DataServer_typedefs_SK.cpp
	rm -f DataServer_serviceInterface_SK.h 
	rm -f DataServer_serviceInterface_SK.cpp
	rm -f DataServer_serviceInterface_i.cc

DataServer_serviceInterface_SK.h: ../idl/DataServer_serviceInterface.idl
	omniidl  -bcxx -Wbexample -Wbh=_SK.h -Wbs=_SK.cpp ../idl/DataServer_serviceInterface.idl
	
DataServer_serviceInterface_SK.cpp: DataServer_serviceInterface_SK.h

DataServer_typedefs_SK.h: ../idl/DataServer_typedefs.idl
	omniidl  -bcxx -Wbh=_SK.h -Wbs=_SK.cpp ../idl/DataServer_typedefs.idl
	
DataServer_typedefs_SK.cpp: DataServer_typedefs_SK.h
