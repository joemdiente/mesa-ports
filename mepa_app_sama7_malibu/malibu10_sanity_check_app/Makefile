#
# This simply builds example code taken from repository https://github.com/MicrochipTech/mepa_app_malibu10_rpi.
# This is used as a starting point when developing own application.
# Used for sanity check, example code includes simple IO test.
#

# Define the C compiler and flags
CC=/opt/mscc/sama7-toolchain/bin/arm-buildroot-linux-gnueabihf-gcc
CFLAGS = -Wall -g

# Define source files, object files, and the final executable name

MEPA_CMAKE_BUILD_DIR=../sw-mepa/build
# From mepa-app-malibu10-rpi
INCLUDES=-I$(MEPA_CMAKE_BUILD_DIR)/include_common/ -I./
SOURCES=./mepa_appl_custom_malibu.c ./rpi_spi.c
DEBUGBUILD=-g
ERRORCHECK=-W -Werror -Wall
WARNINGCHECK=-Wno-error=implicit-function-declaration -Wno-error=int-conversion -Wno-error=incompatible-pointer-types
STATICLIBS=$(MEPA_CMAKE_BUILD_DIR)/mepa/libmepa.a \
			$(MEPA_CMAKE_BUILD_DIR)/mepa/libmepa_common.a \
			$(MEPA_CMAKE_BUILD_DIR)/mepa/vtss/libmepa_drv_vtss_custom.a
MACROS=-DAPPL_TRACE_LVL_ERROR

# The 'all' target is the default. It builds the executable.
all: 
	$(CC) $(INCLUDES) $(SOURCES) $(DEBUGBUILD) $(WARNINGCHECK) $(STATICLIBS) $(MACROS) -o malibu10_sanity_check_app

# A phony target to clean up generated files
clean:
	rm -f $(EXECUTABLE) $(OBJECTS)

.PHONY: all clean