#Release
#Debug
BUILD_TYPE = Release

ifneq ($(shell test -d .git), 0)
GIT_SHORT_HASH:= $(shell git rev-parse --short HEAD)
endif

VERSION_MAJOR = 0
VERSION_MINOR = 4
VERSION_PATCH = 0

VERSION = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)-$(GIT_SHORT_HASH)

COMMON_DEFINITIONS =                                      \
	-DCMAKE_BUILD_TYPE=$(BUILD_TYPE)                      \
	-DVERSION_MAJOR=$(VERSION_MAJOR)                      \
	-DVERSION_MINOR=$(VERSION_MINOR)                      \
	-DVERSION_PATCH=$(VERSION_PATCH)                      \
	-DVERSION=$(VERSION)                                  \

define generate_common
	rm -R -f build_$(1)
	mkdir build_$(1)
	cd build_$(1) && cmake -G"Unix Makefiles"           \
	$(COMMON_DEFINITIONS)                               \
	$(2)                                                \
	-DCMAKE_TOOLCHAIN_FILE=../toolchain/$(1).cmake ..
endef

generic:
	$(call generate_common,$@)

cortex-m0:
	$(call generate_common,$@)
	
cortex-m0+:
	$(call generate_common,$@)
	
cortex-m3:
	$(call generate_common,$@)
	
cortex-m4:
	$(call generate_common,$@)
	
cortex-m4f:
	$(call generate_common,$@)

mingw:
	$(call generate_common,$@,-DWIN32=1)

all: 
	generic

clean:
	rm -R -f build_*
