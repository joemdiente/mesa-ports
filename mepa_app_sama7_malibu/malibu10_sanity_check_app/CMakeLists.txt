cmake_minimum_required(VERSION 3.10)
project(malibu10_sanity_check_app C)

set(CMAKE_BUILD_TYPE "Release")

# 1. Define Paths (Mirroring your MEPA_CMAKE_BUILD_DIR)
set(MEPA_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../sw-mepa/build")

# 2. Executable and Sources
add_executable(malibu10_sanity_check_app 
    mepa_appl_custom_malibu.c 
    rpi_spi.c
)

# 3. Include Directories
target_include_directories(malibu10_sanity_check_app PRIVATE 
    ${MEPA_BUILD_DIR}/include_common/
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 4. Compile Definitions (Macros)
target_compile_definitions(malibu10_sanity_check_app PRIVATE APPL_TRACE_LVL_ERROR)

# 5. Compile Options (Flags)
target_compile_options(malibu10_sanity_check_app PRIVATE 
    -g 
    -Wall 
    -Wextra
    -Wno-error=implicit-function-declaration 
    -Wno-error=int-conversion 
    -Wno-error=incompatible-pointer-types
)

# 6. Link Static Libraries
# Using absolute paths as per your original Makefile
target_link_libraries(malibu10_sanity_check_app PRIVATE 
    ${MEPA_BUILD_DIR}/mepa/libmepa.a
    ${MEPA_BUILD_DIR}/mepa/libmepa_common.a
    ${MEPA_BUILD_DIR}/mepa/vtss/libmepa_drv_vtss_custom.a
)