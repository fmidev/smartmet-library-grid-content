SUBNAME = grid-content
LIB = smartmet-$(SUBNAME)
SPEC = smartmet-library-$(SUBNAME)
INCDIR = smartmet/$(SUBNAME)

# Enabling / disabling CORBA usage in this library.

CORBA = enabled


# Installation directories

processor := $(shell uname -p)

ifeq ($(origin PREFIX), undefined)
  PREFIX = /usr
else
  PREFIX = $(PREFIX)
endif

ifeq ($(processor), x86_64)
  libdir = $(PREFIX)/lib64
else
  libdir = $(PREFIX)/lib
endif

bindir = $(PREFIX)/bin
includedir = $(PREFIX)/include
datadir = $(PREFIX)/share
objdir = obj

# Compiler options

DEFINES = -DUNIX -D_REENTRANT

ifeq ($(CORBA), disabled)
  CORBA_FLAGS = -DCORBA_DISABLED
else
  CORBA_INCLUDE = -Isrc/contentServer/corba/stubs \
                  -Isrc/dataServer/corba/stubs \
                  -Isrc/queryServer/corba/stubs
  CORBA_LIBS = -lomniORB4 -lomnithread  
endif


ifeq ($(CXX), clang++)

 FLAGS = \
	-std=c++11 -fPIC -MD \
	-Weverything \
	-Wno-c++98-compat \
	-Wno-float-equal \
	-Wno-padded \
	-Wno-missing-prototypes \
	$(CORBA_FLAGS)

 INCLUDES = \
	-isystem $(includedir) \
	-isystem $(includedir)/smartmet \
	-isystem $(includedir)/smartmet/grid-files

else

 FLAGS = -std=c++11 -fdiagnostics-color=always -fPIC -MD -Wall -W -Wno-unused-parameter $(CORBA_FLAGS)

 FLAGS_DEBUG = \
	-Wcast-align \
	-Winline \
	-Wno-multichar \
	-Wno-pmf-conversions \
	-Woverloaded-virtual  \
	-Wpointer-arith \
	-Wcast-qual \
	-Wredundant-decls \
	-Wwrite-strings \
	-Wno-sign-promo \
	-Wno-unknown-pragmas \
	-Wno-inline

 FLAGS_RELEASE = -Wuninitialized

 INCLUDES = \
	-I$(includedir) \
	-I$(includedir)/smartmet \
	-I$(includedir)/smartmet/grid-files \
	-I /usr/include/lua5.3 \
	$(pkg-config --cflags icu-i18n) \
	$(CORBA_INCLUDE)

endif

# Compile options in detault, debug and profile modes

CFLAGS         = $(DEFINES) $(FLAGS) $(FLAGS_RELEASE) -DNDEBUG -O2 -g
CFLAGS_DEBUG   = $(DEFINES) $(FLAGS) $(FLAGS_DEBUG)   -Werror  -Og -g
CFLAGS_PROFILE = $(DEFINES) $(FLAGS) $(FLAGS_PROFILE) -DNDEBUG -O2 -g -pg

LIBS = -L$(libdir) \
	-lsmartmet-grid-files \
	-lhiredis \
	-lboost_date_time \
	-lboost_filesystem \
	-lboost_thread \
	-lboost_regex \
	-lboost_system \
	$(pkg-config --libs icu-i18n) \
	-llua5.3 \
	-lfmt \
	-lctpp2 \
	-lpthread -lrt \
	$(CORBA_LIBS)

# What to install

LIBFILE = libsmartmet-$(SUBNAME).so

# How to install

INSTALL_PROG = install -p -m 775
INSTALL_DATA = install -p -m 664

# Compile option overrides

ifneq (,$(findstring debug,$(MAKECMDGOALS)))
  CFLAGS = $(CFLAGS_DEBUG)
endif

ifneq (,$(findstring profile,$(MAKECMDGOALS)))
  CFLAGS = $(CFLAGS_PROFILE)
endif


ifeq ($(CORBA), disabled)

# Compilation directories

vpath %.cpp src/ \
            src/contentServer \
			src/contentServer/definition \
			src/contentServer/cache \
			src/contentServer/redis \
			src/contentServer/http \
			src/contentServer/http/client \
			src/contentServer/http/common \
			src/contentServer/http/server \
			src/contentServer/sync \
            src/dataServer \
			src/dataServer/definition \
			src/dataServer/cache \
			src/dataServer/implementation \ 
			src/queryServer/definition \
			src/queryServer/implementation 
			
vpath %.h 	src/ \
            src/contentServer \
			src/contentServer/definition \
			src/contentServer/cache \
			src/contentServer/redis \
			src/contentServer/http \
			src/contentServer/http/client \
			src/contentServer/http/common \
			src/contentServer/http/server \
			src/contentServer/sync \
            src/dataServer \
			src/dataServer/definition \
			src/dataServer/cache \
			src/dataServer/implementation \ 
			src/queryServer/definition \
			src/queryServer/implementation 

vpath %.o obj


# The files to be compiled

SRCS     = $(patsubst src/%,%,$(wildcard src/*.cpp src/*/*.cpp src/*/definition/*.cpp src/*/cache/*.cpp src/*/redis/*.cpp src/*/http/*/*.cpp src/*/implementation/*.cpp src/*/sync/*.cpp ))

else

# Compilation directories

vpath %.cpp src/ \
            src/contentServer \
			src/contentServer/definition \
			src/contentServer/cache \
			src/contentServer/redis \
			src/contentServer/corba \
			src/contentServer/corba/client \
			src/contentServer/corba/convert \
			src/contentServer/corba/stubs \
			src/contentServer/corba/server \
			src/contentServer/http \
			src/contentServer/http/client \
			src/contentServer/http/common \
			src/contentServer/http/server \
			src/contentServer/sync \
            src/dataServer \
			src/dataServer/definition \
			src/dataServer/cache \
			src/dataServer/implementation \
			src/dataServer/corba/client \
			src/dataServer/corba/convert \
			src/dataServer/corba/stubs \
			src/dataServer/corba/server \
			src/queryServer/definition \
			src/queryServer/implementation \
			src/queryServer/corba/client \
			src/queryServer/corba/convert \
			src/queryServer/corba/stubs \
			src/queryServer/corba/server
			
vpath %.h 	src/ \
            src/contentServer \
			src/contentServer/definition \
			src/contentServer/cache \
			src/contentServer/redis \
			src/contentServer/corba \
			src/contentServer/corba/client \
			src/contentServer/corba/convert \
			src/contentServer/corba/stubs \
			src/contentServer/corba/server \
			src/contentServer/http \
			src/contentServer/http/client \
			src/contentServer/http/common \
			src/contentServer/http/server \
			src/contentServer/sync \
            src/dataServer \
			src/dataServer/definition \
			src/dataServer/cache \
			src/dataServer/implementation \
			src/dataServer/corba/client \
			src/dataServer/corba/convert \
			src/dataServer/corba/stubs \
			src/dataServer/corba/server \
			src/queryServer/definition \
			src/queryServer/implementation \
			src/queryServer/corba/client \
			src/queryServer/corba/convert \
			src/queryServer/corba/stubs \
			src/queryServer/corba/server

vpath %.o obj


# The files to be compiled

SRCS     = $(patsubst src/%,%,$(wildcard src/*.cpp src/*/*.cpp src/*/definition/*.cpp src/*/cache/*.cpp src/*/sync/*.cpp src/*/redis/*.cpp src/*/corba/*/*.cpp src/*/http/*/*.cpp src/*/implementation/*.cpp))

endif


OBJS     = $(SRCS:%.cpp=%.o)
OBJFILES = $(OBJS:%.o=obj/%.o)

INCLUDES := -Isrc $(INCLUDES)


.PHONY: test rpm

# The rules

all:
ifneq ($(CORBA), disabled)
	$(MAKE) create_stubs;
endif	
	$(MAKE) obj_and_lib; 

debug:
ifneq ($(CORBA), disabled)
	$(MAKE) create_stubs;
endif	
	$(MAKE) obj_and_lib; 

release:
ifneq ($(CORBA), disabled)
	$(MAKE) create_stubs;
endif	
	$(MAKE) obj_and_lib; 

profile:
ifneq ($(CORBA), disabled)
	$(MAKE) create_stubs;
endif	
	$(MAKE) obj_and_lib; 

obj_and_lib: objdir $(LIBFILE)


$(LIBFILE): $(OBJFILES)
	$(CXX) $(CFLAGS) -shared -rdynamic -o $(LIBFILE) $(OBJFILES) $(LIBS)

clean: delete_stubs
	rm -f src/*~ src/*/*~ src/*/*/*~
	rm -rf obj
	rm -rf $(LIBFILE)

format:
	clang-format -i -style=file $(SUBNAME)/*.h $(SUBNAME)/*.cpp test/*.cpp

install:
	@mkdir -p $(includedir)/$(INCDIR)
	@rm -rf $(includedir)/$(INCDIR)/*
	@mkdir -p $(includedir)/$(INCDIR)/contentServer
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/definition
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/cache
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/redis
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/http
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/http/client
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/http/common/
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/http/server
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/sync
	@mkdir -p $(includedir)/$(INCDIR)/dataServer/definition
	@mkdir -p $(includedir)/$(INCDIR)/dataServer/cache
	@mkdir -p $(includedir)/$(INCDIR)/dataServer/implementation
	@mkdir -p $(includedir)/$(INCDIR)/queryServer
	@mkdir -p $(includedir)/$(INCDIR)/queryServer/definition
	@mkdir -p $(includedir)/$(INCDIR)/queryServer/implementation
	@cp src/contentServer/definition/*.h $(includedir)/$(INCDIR)/contentServer/definition
	@cp src/contentServer/cache/*.h $(includedir)/$(INCDIR)/contentServer/cache
	@cp src/contentServer/redis/*.h $(includedir)/$(INCDIR)/contentServer/redis
	@cp src/contentServer/http/client/*.h $(includedir)/$(INCDIR)/contentServer/http/client
	@cp src/contentServer/http/common/*.h $(includedir)/$(INCDIR)/contentServer/http/common
	@cp src/contentServer/http/server/*.h $(includedir)/$(INCDIR)/contentServer/http/server	
	@cp src/contentServer/sync/*.h $(includedir)/$(INCDIR)/contentServer/sync
	@cp src/dataServer/definition/*.h $(includedir)/$(INCDIR)/dataServer/definition
	@cp src/dataServer/cache/*.h $(includedir)/$(INCDIR)/dataServer/cache
	@cp src/dataServer/implementation/*.h $(includedir)/$(INCDIR)/dataServer/implementation
	@cp src/queryServer/definition/*.h $(includedir)/$(INCDIR)/queryServer/definition
	@cp src/queryServer/implementation/*.h $(includedir)/$(INCDIR)/queryServer/implementation
ifneq ($(CORBA), disabled)
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/corba
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/corba/client
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/corba/convert
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/corba/stubs
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/corba/server
	@mkdir -p $(includedir)/$(INCDIR)/dataServer/corba
	@mkdir -p $(includedir)/$(INCDIR)/dataServer/corba/client
	@mkdir -p $(includedir)/$(INCDIR)/dataServer/corba/convert
	@mkdir -p $(includedir)/$(INCDIR)/dataServer/corba/stubs
	@mkdir -p $(includedir)/$(INCDIR)/dataServer/corba/server
	@mkdir -p $(includedir)/$(INCDIR)/queryServer/corba
	@mkdir -p $(includedir)/$(INCDIR)/queryServer/corba/client
	@mkdir -p $(includedir)/$(INCDIR)/queryServer/corba/convert
	@mkdir -p $(includedir)/$(INCDIR)/queryServer/corba/stubs
	@mkdir -p $(includedir)/$(INCDIR)/queryServer/corba/server
	@cp src/contentServer/corba/client/*.h $(includedir)/$(INCDIR)/contentServer/corba/client
	@cp src/contentServer/corba/convert/*.h $(includedir)/$(INCDIR)/contentServer/corba/convert
	@cp src/contentServer/corba/stubs/*.h $(includedir)/$(INCDIR)/contentServer/corba/stubs
	@cp src/contentServer/corba/server/*.h $(includedir)/$(INCDIR)/contentServer/corba/server
	@cp src/dataServer/corba/client/*.h $(includedir)/$(INCDIR)/dataServer/corba/client
	@cp src/dataServer/corba/convert/*.h $(includedir)/$(INCDIR)/dataServer/corba/convert
	@cp src/dataServer/corba/stubs/*.h $(includedir)/$(INCDIR)/dataServer/corba/stubs
	@cp src/dataServer/corba/server/*.h $(includedir)/$(INCDIR)/dataServer/corba/server
	@cp src/queryServer/corba/client/*.h $(includedir)/$(INCDIR)/queryServer/corba/client
	@cp src/queryServer/corba/convert/*.h $(includedir)/$(INCDIR)/queryServer/corba/convert
	@cp src/queryServer/corba/stubs/*.h $(includedir)/$(INCDIR)/queryServer/corba/stubs
	@cp src/queryServer/corba/server/*.h $(includedir)/$(INCDIR)/queryServer/corba/server
endif	
	@mkdir -p $(libdir)
	$(INSTALL_PROG) $(LIBFILE) $(libdir)/$(LIBFILE)

test:
	+cd test && make test

objdir:
	@mkdir -p obj
	@mkdir -p obj/contentServer
	@mkdir -p obj/contentServer/definition
	@mkdir -p obj/contentServer/cache
	@mkdir -p obj/contentServer/redis
	@mkdir -p obj/contentServer/http
	@mkdir -p obj/contentServer/http/client
	@mkdir -p obj/contentServer/http/common
	@mkdir -p obj/contentServer/http/server
	@mkdir -p obj/contentServer/sync
	@mkdir -p obj/dataServer/definition
	@mkdir -p obj/dataServer/cache
	@mkdir -p obj/dataServer/implementation
	@mkdir -p obj/queryServer/definition
	@mkdir -p obj/queryServer/implementation
ifneq ($(CORBA), disabled)
	@mkdir -p obj/contentServer/corba
	@mkdir -p obj/contentServer/corba/client
	@mkdir -p obj/contentServer/corba/convert
	@mkdir -p obj/contentServer/corba/stubs
	@mkdir -p obj/contentServer/corba/server
	@mkdir -p obj/dataServer/corba
	@mkdir -p obj/dataServer/corba/client
	@mkdir -p obj/dataServer/corba/convert
	@mkdir -p obj/dataServer/corba/stubs
	@mkdir -p obj/dataServer/corba/server
	@mkdir -p obj/queryServer/corba
	@mkdir -p obj/queryServer/corba/client
	@mkdir -p obj/queryServer/corba/convert
	@mkdir -p obj/queryServer/corba/stubs
	@mkdir -p obj/queryServer/corba/server
endif			  

create_stubs:
	cd src/contentServer/corba/stubs; $(MAKE) all;
	cd src/dataServer/corba/stubs; $(MAKE) all;
	cd src/queryServer/corba/stubs; $(MAKE) all;
	
delete_stubs:
	cd src/contentServer/corba/stubs; $(MAKE) clean;
	cd src/dataServer/corba/stubs; $(MAKE) clean;
	cd src/queryServer/corba/stubs; $(MAKE) clean;


rpm: clean
	if [ -e $(SPEC).spec ]; \
	then \
	  tar -czvf $(SPEC).tar.gz --transform "s,^,$(SPEC)/," * ; \
	  rpmbuild -ta $(SPEC).tar.gz ; \
	  rm -f $(SPEC).tar.gz ; \
	else \
	  echo $(SPEC).spec file missing; \
	fi;

.SUFFIXES: $(SUFFIXES) .cpp


obj/%.o: %.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c -o $@ $<

ifneq ($(wildcard obj/*.d),)
-include $(wildcard obj/*.d)
endif
