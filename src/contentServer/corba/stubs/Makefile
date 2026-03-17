

all: Typedefs_SK.h Typedefs_SK.cpp ContentServer_typedefs_SK.h ContentServer_typedefs_SK.cpp ContentServer_serviceInterface_SK.h ContentServer_serviceInterface_SK.cpp

clean:
	rm -f Typedefs_SK.h
	rm -f Typedefs_SK.cpp
	rm -f ContentServer_typedefs_SK.h
	rm -f ContentServer_typedefs_SK.cpp
	rm -f ContentServer_serviceInterface_SK.h 
	rm -f ContentServer_serviceInterface_SK.cpp
	rm -f ContentServer_serviceInterface_i.cc

ContentServer_serviceInterface_SK.h: ../idl/ContentServer_serviceInterface.idl
	omniidl  -bcxx -Wbexample -Wbh=_SK.h -Wbs=_SK.cpp ../idl/ContentServer_serviceInterface.idl
	
ContentServer_serviceInterface_SK.cpp: ContentServer_serviceInterface_SK.h

Typedefs_SK.h: ../idl/Typedefs.idl
	omniidl  -bcxx -Wbh=_SK.h -Wbs=_SK.cpp ../idl/Typedefs.idl
	
Typedefs_SK.cpp: Typedefs_SK.h

ContentServer_typedefs_SK.h: ../idl/ContentServer_typedefs.idl
	omniidl  -bcxx -Wbh=_SK.h -Wbs=_SK.cpp ../idl/ContentServer_typedefs.idl
	
ContentServer_typedefs_SK.cpp: ContentServer_typedefs_SK.h
