PROG = $(patsubst %.cpp,%,$(wildcard *Test.cpp))

REQUIRES = gdal icu-i18n fmt

include $(shell echo $${PREFIX-/usr})/share/smartmet/devel/makefile.inc

DEFINES = -DUNIX -D_REENTRANT
FLAGS = -std=$(CXX_STD) -Wall -W -fdiagnostics-color=$(GCC_DIAG_COLOR) -Wno-unused-parameter

ifeq ($(TSAN), yes)
  FLAGS += -fsanitize=thread
endif
ifeq ($(ASAN), yes)
  FLAGS += -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract \
           -fsanitize=undefined -fsanitize-address-use-after-scope
endif
ifeq ($(COVERAGE), yes)
  FLAGS += --coverage
endif

CFLAGS = -DUNIX -O0 -g $(FLAGS) `pkg-config --cflags luajit`

INCLUDES += -I../src

# Link against the freshly built library in the parent directory
LIBS += ../libsmartmet-grid-content.so \
	$(REQUIRED_LIBS) \
	-lsmartmet-grid-files \
	-lsmartmet-newbase \
	-lsmartmet-gis \
	-lsmartmet-spine \
	-lsmartmet-macgyver \
	-lpthread \
	$(PREFIX_LDFLAGS)

# Tests needing installed fixtures (smartmet-test-data, smartmet-engine-grid-test) fail when
# they are missing. Set GRID_TEST_ALLOW_SKIP=1 to skip them instead.

all: $(PROG)

clean:
	rm -f $(PROG) *~ *.gcda *.gcno

test: $(PROG)
	@echo Running tests:
	@ok=true; \
	for prog in $(PROG); do \
	  echo "### $$prog"; \
	  ./$$prog || ok=false; \
	done; \
	$$ok

$(PROG) : % : %.cpp Makefile $(wildcard *.h) $(shell find ../src -name "*.h") ../libsmartmet-grid-content.so
	$(CXX) $(CFLAGS) $(DEFINES) -o $@ $@.cpp $(INCLUDES) $(LIBS)
