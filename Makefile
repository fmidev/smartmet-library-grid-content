SUBNAME = grid-content
LIB = smartmet-$(SUBNAME)
SPEC = smartmet-library-$(SUBNAME)
INCDIR = smartmet/$(SUBNAME)

# Enabling / disabling CORBA usage in this library.

CORBA = enabled

REQUIRES = gdal

include $(shell echo $${PREFIX-/usr})/share/smartmet/devel/makefile.inc

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

CFLAGS += $(CORBA_FLAGS)
INCLUDES += $(CORBA_INCLUDE)

CFLAGS += \
	-isystem /usr/include/lua \
	$(pkg-config --cflags icu-i18n)

LIBS += $(REQUIRED_LIBS) $(CORBA_LIBS) \
	-L$(libdir) \
	-lsmartmet-grid-files \
	-lsmartmet-spine \
	-lsmartmet-macgyver \
	-lsmartmet-gis \
	-lboost_date_time \
	-lhiredis \
	-lcurl \
	-llua \
	-lpthread

#-L/usr/pgsql-9.5/lib -lpq

# What to install

LIBFILE = libsmartmet-$(SUBNAME).so

# How to install

INSTALL_PROG = install -p -m 775
INSTALL_DATA = install -p -m 664

ifeq ($(CORBA), disabled)

# Compilation directories

vpath %.cpp src/ \
            src/contentServer \
			src/contentServer/cache \
			src/contentServer/definition \
			src/contentServer/http \
			src/contentServer/http/client \
			src/contentServer/http/common \
			src/contentServer/http/server \
			src/contentServer/memory \
			src/contentServer/redis \
			src/contentServer/sync \
      src/dataServer \
			src/dataServer/definition \
			src/dataServer/cache \
			src/dataServer/implementation \
			src/queryServer/definition \
			src/queryServer/implementation \
			src/functions \
			src/lua

vpath %.h 	src/ \
	src/contentServer \
			src/contentServer/cache \
			src/contentServer/definition \
			src/contentServer/http \
			src/contentServer/http/client \
			src/contentServer/http/common \
			src/contentServer/http/server \
			src/contentServer/memory \
			src/contentServer/redis \
			src/contentServer/sync \
      src/dataServer \
			src/dataServer/definition \
			src/dataServer/cache \
			src/dataServer/implementation \
			src/queryServer/definition \
			src/queryServer/implementation \
			src/functions \
			src/lua

vpath %.o obj


# The files to be compiled

SRCS     = $(patsubst src/%,%,$(wildcard src/*.cpp src/*/*.cpp src/*/definition/*.cpp src/*/cache/*.cpp src/*/redis/*.cpp src/*/memory/*.cpp src/*/http/*/*.cpp src/*/implementation/*.cpp src/*/sync/*.cpp ))

else

# Compilation directories

vpath %.cpp src/ \
      src/contentServer \
			src/contentServer/cache \
			src/contentServer/corba \
			src/contentServer/corba/client \
			src/contentServer/corba/convert \
			src/contentServer/corba/stubs \
			src/contentServer/corba/server \
			src/contentServer/definition \
			src/contentServer/http \
			src/contentServer/http/client \
			src/contentServer/http/common \
			src/contentServer/http/server \
			src/contentServer/memory \
			src/contentServer/redis \
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
			src/queryServer/corba/server \
			src/functions \
			src/lua

vpath %.h 	src/ \
      src/contentServer \
			src/contentServer/cache \
			src/contentServer/corba \
			src/contentServer/corba/client \
			src/contentServer/corba/convert \
			src/contentServer/corba/stubs \
			src/contentServer/corba/server \
			src/contentServer/definition \
			src/contentServer/http \
			src/contentServer/http/client \
			src/contentServer/http/common \
			src/contentServer/http/server \
			src/contentServer/memory \
			src/contentServer/redis \
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
			src/queryServer/corba/server \
			src/functions \
			src/lua

vpath %.o obj


# The files to be compiled

SRCS     = $(patsubst src/%,%,$(wildcard src/*.cpp src/*/*.cpp src/*/definition/*.cpp src/*/cache/*.cpp src/*/sync/*.cpp src/*/redis/*.cpp src/*/memory/*.cpp src/*/corba/*/*.cpp src/*/http/*/*.cpp src/*/implementation/*.cpp))

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
	$(MAKE) objdir
	$(MAKE) $(LIBFILE)

debug:
ifneq ($(CORBA), disabled)
	$(MAKE) create_stubs;
endif
	$(MAKE) objdir
	$(MAKE) $(LIBFILE)

release:
ifneq ($(CORBA), disabled)
	$(MAKE) create_stubs;
endif
	$(MAKE) objdir
	$(MAKE) $(LIBFILE)

profile:
ifneq ($(CORBA), disabled)
	$(MAKE) create_stubs;
endif
	$(MAKE) objdir
	$(MAKE) $(LIBFILE)

$(LIBFILE): $(OBJFILES)
	$(CC) $(LDFLAGS) -shared -rdynamic -o $(LIBFILE) $(OBJFILES) $(LIBS)

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
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/cache
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/definition
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/http
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/http/client
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/http/common/
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/http/server
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/memory
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/redis
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/sync
	@mkdir -p $(includedir)/$(INCDIR)/dataServer/definition
	@mkdir -p $(includedir)/$(INCDIR)/dataServer/cache
	@mkdir -p $(includedir)/$(INCDIR)/dataServer/implementation
	@mkdir -p $(includedir)/$(INCDIR)/queryServer
	@mkdir -p $(includedir)/$(INCDIR)/queryServer/definition
	@mkdir -p $(includedir)/$(INCDIR)/queryServer/implementation
	@mkdir -p $(includedir)/$(INCDIR)/functions
	@mkdir -p $(includedir)/$(INCDIR)/lua
	@cp src/contentServer/cache/*.h $(includedir)/$(INCDIR)/contentServer/cache
	@cp src/contentServer/definition/*.h $(includedir)/$(INCDIR)/contentServer/definition
	@cp src/contentServer/http/client/*.h $(includedir)/$(INCDIR)/contentServer/http/client
	@cp src/contentServer/http/common/*.h $(includedir)/$(INCDIR)/contentServer/http/common
	@cp src/contentServer/http/server/*.h $(includedir)/$(INCDIR)/contentServer/http/server
	@cp src/contentServer/memory/*.h $(includedir)/$(INCDIR)/contentServer/memory
	@cp src/contentServer/redis/*.h $(includedir)/$(INCDIR)/contentServer/redis
	@cp src/contentServer/sync/*.h $(includedir)/$(INCDIR)/contentServer/sync
	@cp src/dataServer/definition/*.h $(includedir)/$(INCDIR)/dataServer/definition
	@cp src/dataServer/cache/*.h $(includedir)/$(INCDIR)/dataServer/cache
	@cp src/dataServer/implementation/*.h $(includedir)/$(INCDIR)/dataServer/implementation
	@cp src/queryServer/definition/*.h $(includedir)/$(INCDIR)/queryServer/definition
	@cp src/queryServer/implementation/*.h $(includedir)/$(INCDIR)/queryServer/implementation
	@cp src/functions/*.h $(includedir)/$(INCDIR)/functions
	@cp src/lua/*.h $(includedir)/$(INCDIR)/lua
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
	@mkdir -p obj/contentServer/cache
	@mkdir -p obj/contentServer/definition
	@mkdir -p obj/contentServer/http
	@mkdir -p obj/contentServer/http/client
	@mkdir -p obj/contentServer/http/common
	@mkdir -p obj/contentServer/http/server
	@mkdir -p obj/contentServer/memory
	@mkdir -p obj/contentServer/redis
	@mkdir -p obj/contentServer/sync
	@mkdir -p obj/dataServer/definition
	@mkdir -p obj/dataServer/cache
	@mkdir -p obj/dataServer/implementation
	@mkdir -p obj/queryServer/definition
	@mkdir -p obj/queryServer/implementation
	@mkdir -p obj/functions
	@mkdir -p obj/lua
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


rpm: clean $(SPEC).spec
	rm -f $(SPEC).tar.gz # Clean a possible leftover from previous attempt
	tar -czvf $(SPEC).tar.gz --transform "s,^,$(SPEC)/," *
	rpmbuild -tb $(SPEC).tar.gz
	rm -f $(SPEC).tar.gz

.SUFFIXES: $(SUFFIXES) .cpp


obj/%.o: %.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c -o $@ $<

ifneq ($(wildcard obj/*.d),)
-include $(wildcard obj/*.d)
endif
