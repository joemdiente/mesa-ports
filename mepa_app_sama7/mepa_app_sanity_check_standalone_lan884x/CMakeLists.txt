cmake_minimum_required(VERSION 3.10)
project(mepa_app_sanity_check_lan884x C)

set(CMAKE_BUILD_TYPE "Release")

# 1. Define Paths
set(MEPA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../sw-mepa")
set(MEPA_BUILD_DIR "${MEPA_DIR}/build")
set(SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(MDIO_TOOLS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../mdio-tools")

# 2. Add mepa_api
# Make sure to change MEPA_DIR to where your sw-mepa is.
include(FetchContent)
set(MEPA_vtss ON CACHE BOOL "" FORCE)
set(MEPA_vtss_opt_10g ON CACHE BOOL "" FORCE)
set(BUILD_MEPA_DEMO OFF CACHE BOOL "" FORCE)
set(MEPA_vtss_opt_1g ON CACHE BOOL "" FORCE)
set(MEPA_vtss_opt_ts ON CACHE BOOL "" FORCE)
set(MEPA_vtss_opt_macsec ON CACHE BOOL "" FORCE)
set(MEPA_lan884x ON CACHE BOOL "" FORCE)
set(CMAKE_BUILD_TYPE Debug)

FetchContent_Declare(
mepa
SOURCE_DIR "${MEPA_DIR}"
)

# 3. Find libmnl in buildroot SDK
set(PKG_CONFIG_EXECUTABLE "${CMAKE_PROGRAM_PATH}/pkg-config")
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBMNL REQUIRED libmnl)

# 4. Executable and Sources
add_executable(mepa_app_sanity_check_lan884x 
    ${SOURCES_DIR}/mepa_appl_custom_lan884x.c 
    ${SOURCES_DIR}/linux_mdio.c
    ${MDIO_TOOLS_DIR}/src/mdio/mdio.c
)

# 4. Include Directories
target_include_directories(mepa_app_sanity_check_lan884x PRIVATE 
    ${MEPA_DIR}/mepa/include/
    ${MEPA_DIR}/me/include/
    ${MEPA_DIR}/board-configs/include/ # All these two for callout
    ${MEPA_DIR}/sw-mesa/mesa/include/  # All these two for callout
    ${INC_DIR}/
    ${MDIO_TOOLS_DIR}/include/
    ${MDIO_TOOLS_DIR}/src/mdio/
    ${LIBMNL_INCLUDE_DIRS}/
)

# 5. Compile Definitions (Macros)
target_compile_definitions(mepa_app_sanity_check_lan884x PRIVATE APPL_TRACE_LVL_ERROR)

# 6. Compile Options (Flags)
target_compile_options(mepa_app_sanity_check_lan884x PRIVATE 
    -g 
    -Wall 
    -Wextra
    -Wno-error=implicit-function-declaration 
    -Wno-error=int-conversion 
    -Wno-error=incompatible-pointer-types
)

# 7. Bring the targets into the project
FetchContent_MakeAvailable(mepa)

# 8. Link Static Libraries
# Using absolute paths as per your original Makefile
target_link_libraries(mepa_app_sanity_check_lan884x PRIVATE 
    ${LIBMNL_LIBRARIES}
    mepa
)