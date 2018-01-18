CXX := $(CC)
SRCDIR := src
BUILDDIR := build
INCDIR := include
TARGETDIR := bin
TARGET := $(TARGETDIR)/server

SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -follow -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

LIB := -lpthread -L../common/lib -lcommon
INC := -I /usr/include -I /usr/local/include -I../common/include -I $(INCDIR)

## Language standard
CXXFLAGS := -std=gnu89

## Release build - full optimization, native build and strip debug symbols
ifeq ($(BUILD),release)
	CXXFLAGS += -O3 -march=native -s -DNDEBUG -pedantic
	## Optimization options
	CXXFLAGS += -fstack-protector-all -fstack-protector-strong \
				-fstrict-aliasing
	#
else
	# Debug build - no optimizations
	CXXFLAGS += -O0 -DDEBUG
	## -no-pie for recent gcc versions:
	## https://bugs.launchpad.net/ubuntu/+source/gcc-6/+bug/1678510
	# CXXFLAGS += -no-pie -pg
	#
	## Warning options
	CXXFLAGS += -Wall -Wextra -Wformat=2 -Wswitch-default \
				-Wcast-align -Wpointer-arith -Wbad-function-cast \
				-Wstrict-prototypes -Winline -Wundef -Wnested-externs \
				-Wcast-qual -Wshadow -Wwrite-strings -Wconversion \
				-Winit-self -Wstrict-aliasing -Wmissing-declarations \
				-Wmissing-include-dirs -Wno-unused-parameter -Wuninitialized \
				-Wold-style-definition -Wmissing-prototypes -Wunreachable-code \
				-Wno-unused-but-set-variable -Wmaybe-uninitialized
	#
	LDFLAGS += -z now
endif

.PHONY: clean release all

all: $(TARGET)

release_header:
	@echo "================================================="
	@echo "Release build initiated..."
	@echo "--------"

release: clean release_header
	$(MAKE) "BUILD=release"

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo "================================================="
	@echo "Building objects..."
	@echo "--------"
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(INCDIR)
	$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<

$(TARGET): $(OBJECTS)
	@echo "================================================="
	@echo "Linking objects..."
	@echo "--------"
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJECTS) $(LIB)

clean:
	@echo "================================================="
	@echo "Cleaning up build files and binaries...";
	@rm -rf $(BUILDDIR) $(TARGETDIR)
