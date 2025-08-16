#!/bin/sh
# SPDX-License-Identifier: MIT
# /* 
#  * Malibu MCP2210-HIDAPI Build Script
#  *
#  * Author: Joemel John Diente <joemdiente@gmail.com>
#  * 
#  */
##    -DRPI_MIIM \
##    -DVTSS_ARCH_VENICE_C=TRUE \     // Compile in support for Venice C and later
##    -DVTSS_CHIP_CU_PHY \            // Compile in support for 1G PHY Families in API
##    -DVTSS_CHIP_10G_PHY \           // Compile in support for 10G PHY Families in API
##    -DVTSS_OPT_PORT_COUNT=2  \      // Compile for MAX Number of Ports in this PHY_INST
##    -DVTSS_OPSYS_LINUX=1 \          // Compile in Linux OS Support <Default OS>
##    -DVTSS_FEATURE_10G  \           // Include 10G Capability, used in MESA builds
##    -DVTSS_FEATURE_10GBASE_KR \     // Compile in Support for 10G Base-KR
##    -DVTSS_USE_STDINT_H \           // Compile to Use Linux Std INT types as defined in stdint.h
##    -D_INCLUDE_DEBUG_TERM_PRINT_ \  // Include Debug Output to Console (EVB/Char Board Setup, in Sample Application Code)
##    -DVENICE_CHAR_BOARD \           // Compile in support for EVB/CHAR BOARD setup
##    -DVTSS_FEATURE_MACSEC \         // Compile in support for MACSEC Block
##    -DVTSS_OPT_PHY_TIMESTAMP \      // Compile in support for IEEE-1588 Timestamp Block
##    `find base/phy/phy_1g -name "*.c"` \    // Compile all files in the phy_1g Directory for 1G PHY Support
##
## Malibu Evaluation board with MCP2210 using HIDAPI Win32
MESA_DIR=./mesa
MESA_INC_DIR=${MESA_DIR}/include/
MESA_PHY_INC=${MESA_DIR}/base/phy/phy_10g/

# PHY_DEMO_APPL
APPL_MALIBU_MCP2210_WIN32_SRC="./malibu_mcp2210_hidapi_win32.c ${MESA_DIR}/phy_demo_appl/appl/vtss_appl_10g_phy_malibu.c"

# BASE AIL
MESA_BASE_AIL_PHY_SRC="${MESA_DIR}/base/ail/vtss_wis_api.c \
${MESA_DIR}/base/ail/vtss_api.c \
${MESA_DIR}/base/ail/vtss_port_api.c \
${MESA_DIR}/base/ail/vtss_common.c \
${MESA_DIR}/base/ail/vtss_sd10g65_procs.c \
${MESA_DIR}/base/ail/vtss_sd10g65_apc_procs.c \
${MESA_DIR}/base/ail/vtss_pll5g_procs.c \
${MESA_DIR}/base/phy/common/vtss_phy_common.c"

SRC_VERS=${MESA_DIR}/phy_demo_appl/appl/vtss_version.c

# Note: Backtick below means run command.
PHY_10G_SRC="${MESA_DIR}/base/phy/phy_10g/*.c \
${MESA_DIR}/base/phy/ts/*.c \
${MESA_DIR}/base/phy/macsec/*.c"

APPL_MALIBU_MCP2210_WIN32_DEFINE="-DVTSS_OPT_PORT_COUNT=4 \
-DVTSS_OPSYS_WIN32=1 \
-DVTSS_FEATURE_10G \
-DVTSS_FEATURE_10GBASE_KR \
-DVTSS_FEATURE_SERDES_MACRO_SETTINGS \
-DVTSS_USE_STDINT_H \
-D_INCLUDE_DEBUG_TERM_PRINT_ \
-DMALIBU_MCP2210 \
-DMALIBU_CHAR_BOARD \
-D_NO_BOOL_TYPEDEF"
# -DVTSS_FEATURE_MACSEC
# -DVTSS_OPT_PHY_TIMESTAMP'

# MCP2210-HIDAPI
# Make sure to point this to MCP2210-HIDAPI Library root dir
MCP2210_HIDAPI_DIR="./mcp2210-hidapi"

# Includes
MCP2210_HIDAPI_INC="${MCP2210_HIDAPI_DIR}/include/mcp2210-hidapi/"
HIDAPI_INC="${MCP2210_HIDAPI_DIR}/include/hidapi/"

MCP2210_HIDAPI_SRC="${MCP2210_HIDAPI_DIR}/src/*.c"
MCP2210_HIDAPI_ARGS="./*.o -L${MCP2210_HIDAPI_DIR}/x64/ -lhidapi" 

echo "[1] Building MCP2210-HIDAPI Objects"
gcc -c ${MCP2210_HIDAPI_DIR}/src/mcp2210-hidapi.c -I${MCP2210_HIDAPI_INC} -I${HIDAPI_INC} &
gcc -c ${MCP2210_HIDAPI_DIR}/src/mcp2210-hidapi-gpio.c -I${MCP2210_HIDAPI_INC} -I${HIDAPI_INC} &
gcc -c ${MCP2210_HIDAPI_DIR}/src/mcp2210-hidapi-spi.c -I${MCP2210_HIDAPI_INC} -I${HIDAPI_INC} &
gcc -c ${MCP2210_HIDAPI_DIR}/src/mcp2210-hidapi-misc.c -I${MCP2210_HIDAPI_INC} -I${HIDAPI_INC}

echo ""
echo "[2] Copy hidapi.dll alongside malibu_mcp2210.exe"
cp ${MCP2210_HIDAPI_DIR}/x64/hidapi.dll ./

echo ""
echo "[3] Building Malibu MCP2210-HIDAPI-WIN32 PHY_DEMO_APPL" 
gcc --static \
  -I ${MESA_INC_DIR} \
  -I ${MESA_PHY_INC} \
  -I ${MCP2210_HIDAPI_INC} \
  -I ${HIDAPI_INC} \
  -I ${MESA_PHY_DEMO_APPL_INC} \
  -std=gnu89 \
  ${APPL_MALIBU_MCP2210_WIN32_SRC} \
  ${MESA_BASE_AIL_PHY_SRC} \
  ${PHY_10G_SRC} \
  ${APPL_MALIBU_MCP2210_WIN32_DEFINE} \
  ${MCP2210_HIDAPI_ARGS} \
  -o malibu_mcp2210

if [ $? -ne 0 ]; then
  echo "malibu-mcp2210.exe compilation failed."
  exit 1 # Exit the script with an error code
else
  echo "malibu-mcp2210.exe compilation successful."
fi


