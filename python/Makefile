all: FmiSmartMet
		 
IDL = ../src/contentServer/corba/idl/ContentServer_serviceInterface.idl \
			../src/contentServer/corba/idl/ContentServer_typedefs.idl \
		  ../src/dataServer/corba/idl/DataServer_serviceInterface.idl \
			../src/dataServer/corba/idl/DataServer_typedefs.idl \
	 	  ../src/queryServer/corba/idl/QueryServer_serviceInterface.idl \
			../src/queryServer/corba/idl/QueryServer_typedefs.idl

clean:
	rm -rf FmiSmartMet 

FmiSmartMet: $(IDL)
	omniidl -bpython -Wbpackage=FmiSmartMet -I../src/contentServer/corba/idl $(IDL)
	
