SUBNAME = grid-content
LIB = smartmet-$(SUBNAME)
SPEC = smartmet-library-$(SUBNAME)
INCDIR = smartmet/$(SUBNAME)

# Enabling / disabling CORBA usage in this library.

CORBA = enabled

REQUIRES = libpq gdal icu-i18n

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

CFLAGS += `pkg-config --cflags luajit`

LIBS += \
	$(PREFIX_LDFLAGS) \
	$(REQUIRED_LIBS) $(CORBA_LIBS) \
	-L$(libdir) \
	-lsmartmet-grid-files \
	-lsmartmet-spine \
	-lsmartmet-macgyver \
	-lsmartmet-gis \
	-lhiredis \
	-lcurl \
	`pkg-config --libs luajit` \
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
			src/contentServer/merge \
			src/contentServer/memory \
			src/contentServer/postgresql \
			src/contentServer/redis \
      src/dataServer \
			src/dataServer/definition \
			src/dataServer/implementation \
			src/queryServer/definition \
			src/queryServer/implementation \
      src/sessionManagement \
			src/sessionManagement/definition \
			src/sessionManagement/implementation \
      src/userManagement \
			src/userManagement/definition \
			src/userManagement/implementation \
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
			src/contentServer/merge \
			src/contentServer/postgresql \
			src/contentServer/redis \
      src/dataServer \
			src/dataServer/definition \
			src/dataServer/implementation \
			src/queryServer/definition \
			src/queryServer/implementation \
      src/sessionManagement \
			src/sessionManagement/definition \
			src/sessionManagement/implementation \
      src/userManagement \
			src/userManagement/definition \
			src/userManagement/implementation \
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
			src/contentServer/merge \
			src/contentServer/postgresql \
			src/contentServer/redis \
      src/dataServer \
			src/dataServer/definition \
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
      src/sessionManagement \
			src/sessionManagement/definition \
			src/sessionManagement/implementation \
      src/userManagement \
			src/userManagement/definition \
			src/userManagement/implementation \
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
			src/contentServer/merge \
			src/contentServer/postgresql \
			src/contentServer/redis \
      src/dataServer \
			src/dataServer/definition \
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
      src/sessionManagement \
			src/sessionManagement/definition \
			src/sessionManagement/implementation \
      src/userManagement \
			src/userManagement/definition \
			src/userManagement/implementation \
			src/functions \
			src/lua

vpath %.o obj


# The files to be compiled

SRCS     = $(patsubst src/%,%,$(wildcard src/*.cpp src/*/*.cpp src/*/definition/*.cpp src/*/cache/*.cpp src/*/redis/*.cpp src/*/memory/*.cpp src/*/merge/*.cpp src/*/postgresql/*.cpp src/*/corba/*/*.cpp src/*/http/*/*.cpp src/*/implementation/*.cpp))

endif


OBJS     = $(SRCS:%.cpp=%.o)
OBJFILES = $(OBJS:%.o=obj/%.o)
DEPFILES = $(OBJFILES:%.o=%.d)

INCLUDES := -Isrc $(INCLUDES)


.PHONY: test rpm doc

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
	$(CXX) $(LDFLAGS) -shared -rdynamic -o $(LIBFILE) $(OBJFILES) $(LIBS)
	@echo Checking $(LIBFILE) for unresolved references
	@if ldd -r $(LIBFILE) 2>&1 | c++filt | grep ^undefined\ symbol |\
			grep -Pv ':\ __(?:(?:a|t|ub)san_|sanitizer_)'; \
	then \
		rm -v $(LIBFILE); \
		exit 1; \
	fi

doc:
	doxygen Doxyfile

clean: delete_stubs
	rm -f src/*~ src/*/*~ src/*/*/*~
	rm -rf obj
	rm -rf $(LIBFILE)
	rm -rf doc/html

clean-install:
	rm -rf $(includedir)/$(INCDIR)/*
	rm -f $(libdir)/$(LIBFILE)

format:
	clang-format -i -style=file $(SUBNAME)/*.h $(SUBNAME)/*.cpp test/*.cpp

install:
	@mkdir -p $(includedir)/$(INCDIR)
	@mkdir -p $(includedir)/$(INCDIR)/contentServer
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/cache
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/definition
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/http
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/http/client
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/http/common/
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/http/server
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/idl
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/memory
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/merge
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/postgresql
	@mkdir -p $(includedir)/$(INCDIR)/contentServer/redis
	@mkdir -p $(includedir)/$(INCDIR)/dataServer/definition
	@mkdir -p $(includedir)/$(INCDIR)/dataServer/idl
	@mkdir -p $(includedir)/$(INCDIR)/dataServer/implementation
	@mkdir -p $(includedir)/$(INCDIR)/queryServer
	@mkdir -p $(includedir)/$(INCDIR)/queryServer/definition
	@mkdir -p $(includedir)/$(INCDIR)/queryServer/idl
	@mkdir -p $(includedir)/$(INCDIR)/queryServer/implementation
	@mkdir -p $(includedir)/$(INCDIR)/sessionManagement
	@mkdir -p $(includedir)/$(INCDIR)/sessionManagement/definition
	@mkdir -p $(includedir)/$(INCDIR)/sessionManagement/implementation
	@mkdir -p $(includedir)/$(INCDIR)/userManagement
	@mkdir -p $(includedir)/$(INCDIR)/userManagement/definition
	@mkdir -p $(includedir)/$(INCDIR)/userManagement/implementation
	@mkdir -p $(includedir)/$(INCDIR)/functions
	@mkdir -p $(includedir)/$(INCDIR)/lua
	$(INSTALL_DATA) src/contentServer/cache/*.h $(includedir)/$(INCDIR)/contentServer/cache
	$(INSTALL_DATA) src/contentServer/definition/*.h $(includedir)/$(INCDIR)/contentServer/definition
	$(INSTALL_DATA) src/contentServer/http/client/*.h $(includedir)/$(INCDIR)/contentServer/http/client
	$(INSTALL_DATA) src/contentServer/http/common/*.h $(includedir)/$(INCDIR)/contentServer/http/common
	$(INSTALL_DATA) src/contentServer/http/server/*.h $(includedir)/$(INCDIR)/contentServer/http/server
	$(INSTALL_DATA) src/contentServer/memory/*.h $(includedir)/$(INCDIR)/contentServer/memory
	$(INSTALL_DATA) src/contentServer/merge/*.h $(includedir)/$(INCDIR)/contentServer/merge
	$(INSTALL_DATA) src/contentServer/postgresql/*.h $(includedir)/$(INCDIR)/contentServer/postgresql
	$(INSTALL_DATA) src/contentServer/redis/*.h $(includedir)/$(INCDIR)/contentServer/redis
	$(INSTALL_DATA) src/dataServer/definition/*.h $(includedir)/$(INCDIR)/dataServer/definition
	$(INSTALL_DATA) src/dataServer/implementation/*.h $(includedir)/$(INCDIR)/dataServer/implementation
	$(INSTALL_DATA) src/queryServer/definition/*.h $(includedir)/$(INCDIR)/queryServer/definition
	$(INSTALL_DATA) src/queryServer/implementation/*.h $(includedir)/$(INCDIR)/queryServer/implementation
	$(INSTALL_DATA) src/sessionManagement/definition/*.h $(includedir)/$(INCDIR)/sessionManagement/definition
	$(INSTALL_DATA) src/sessionManagement/implementation/*.h $(includedir)/$(INCDIR)/sessionManagement/implementation
	$(INSTALL_DATA) src/userManagement/definition/*.h $(includedir)/$(INCDIR)/userManagement/definition
	$(INSTALL_DATA) src/userManagement/implementation/*.h $(includedir)/$(INCDIR)/userManagement/implementation
	$(INSTALL_DATA) src/functions/*.h $(includedir)/$(INCDIR)/functions
	$(INSTALL_DATA) src/lua/*.h $(includedir)/$(INCDIR)/lua
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
	@mkdir -p $(includedir)/$(INCDIR)/python
	$(INSTALL_DATA) src/contentServer/corba/client/*.h $(includedir)/$(INCDIR)/contentServer/corba/client
	$(INSTALL_DATA) src/contentServer/corba/convert/*.h $(includedir)/$(INCDIR)/contentServer/corba/convert
	$(INSTALL_DATA) src/contentServer/corba/idl/*.idl $(includedir)/$(INCDIR)/contentServer/idl
	$(INSTALL_DATA) src/contentServer/corba/stubs/*.h $(includedir)/$(INCDIR)/contentServer/corba/stubs
	$(INSTALL_DATA) src/contentServer/corba/server/*.h $(includedir)/$(INCDIR)/contentServer/corba/server
	$(INSTALL_DATA) src/dataServer/corba/client/*.h $(includedir)/$(INCDIR)/dataServer/corba/client
	$(INSTALL_DATA) src/dataServer/corba/convert/*.h $(includedir)/$(INCDIR)/dataServer/corba/convert
	$(INSTALL_DATA) src/dataServer/corba/idl/*.idl $(includedir)/$(INCDIR)/dataServer/idl
	$(INSTALL_DATA) src/dataServer/corba/stubs/*.h $(includedir)/$(INCDIR)/dataServer/corba/stubs
	$(INSTALL_DATA) src/dataServer/corba/server/*.h $(includedir)/$(INCDIR)/dataServer/corba/server
	$(INSTALL_DATA) src/queryServer/corba/client/*.h $(includedir)/$(INCDIR)/queryServer/corba/client
	$(INSTALL_DATA) src/queryServer/corba/convert/*.h $(includedir)/$(INCDIR)/queryServer/corba/convert
	$(INSTALL_DATA) src/queryServer/corba/idl/*.idl $(includedir)/$(INCDIR)/queryServer/idl
	$(INSTALL_DATA) src/queryServer/corba/stubs/*.h $(includedir)/$(INCDIR)/queryServer/corba/stubs
	$(INSTALL_DATA) src/queryServer/corba/server/*.h $(includedir)/$(INCDIR)/queryServer/corba/server
	@cp -r python/FmiSmartMet $(includedir)/$(INCDIR)/python
	@chmod -R u=rwX,g=rwX,o=rX $(includedir)/$(INCDIR)/python/FmiSmartMet
endif
	@mkdir -p $(libdir)
	$(INSTALL_PROG) $(LIBFILE) $(libdir)/$(LIBFILE)

test:
	+if test -f test/Makefile; then $(MAKE) -C test $@; fi

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
	@mkdir -p obj/contentServer/merge
	@mkdir -p obj/contentServer/postgresql
	@mkdir -p obj/contentServer/redis
	@mkdir -p obj/dataServer/definition
	@mkdir -p obj/dataServer/implementation
	@mkdir -p obj/queryServer/definition
	@mkdir -p obj/queryServer/implementation
	@mkdir -p obj/sessionManagement/definition
	@mkdir -p obj/sessionManagement/implementation
	@mkdir -p obj/userManagement/definition
	@mkdir -p obj/userManagement/implementation
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
ifneq ($(CORBA), disabled)
	cd src/contentServer/corba/stubs; $(MAKE) all;
	cd src/dataServer/corba/stubs; $(MAKE) all;
	cd src/queryServer/corba/stubs; $(MAKE) all;
	cd python; $(MAKE) all;
endif

delete_stubs:
ifneq ($(CORBA), disabled)
	cd src/contentServer/corba/stubs; $(MAKE) clean;
	cd src/dataServer/corba/stubs; $(MAKE) clean;
	cd src/queryServer/corba/stubs; $(MAKE) clean;
	cd python; $(MAKE) clean;
endif


rpm: clean $(SPEC).spec
	rm -f $(SPEC).tar.gz # Clean a possible leftover from previous attempt
	tar -czvf $(SPEC).tar.gz --transform "s,^,$(SPEC)/," *
	rpmbuild -tb $(SPEC).tar.gz
	rm -f $(SPEC).tar.gz

.SUFFIXES: $(SUFFIXES) .cpp


obj/%.o : %.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c -MD -MF $(patsubst obj/%.o, obj/%.d, $@) -MT $@ -o $@ $<

-include $(DEPFILES)
