cmake_minimum_required(VERSION 3.10)
project(mepa_app_sanity_check_malibu C)

set(CMAKE_BUILD_TYPE "Debug")

# 1. Define Paths
set(MEPA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../sw-mepa")
set(MEPA_BUILD_DIR "${MEPA_DIR}/build")
set(SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# 2. Executable and Sources
add_executable(mepa_app_sanity_check_malibu 
    ${SOURCES_DIR}/mepa_appl_custom_malibu.c 
    ${SOURCES_DIR}/rpi_spi.c
)

# 3. Include Directories
target_include_directories(mepa_app_sanity_check_malibu PRIVATE 
    ${MEPA_BUILD_DIR}/include_common/
    ${INC_DIR}/
)

# 4. Compile Definitions (Macros)
target_compile_definitions(mepa_app_sanity_check_malibu PRIVATE APPL_TRACE_LVL_ERROR)

# 5. Compile Options (Flags)
target_compile_options(mepa_app_sanity_check_malibu PRIVATE 
    -g 
    -Wall 
    -Wextra
    -Wno-error=implicit-function-declaration 
    -Wno-error=int-conversion 
    -Wno-error=incompatible-pointer-types
)

# 6. Build sw-mepa libs first
execute_process(COMMAND mkdir build
                WORKING_DIRECTORY ${MEPA_DIR}
)
execute_process(COMMAND cmake .. -DMEPA_vtss=ON -DMEPA_vtss_opt_10g=ON -DBUILD_MEPA_DEMO=OFF 
                -DMEPA_vtss_opt_1g=ON -DMEPA_vtss_opt_ts=ON -DMEPA_vtss_opt_macsec=ON
                -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                WORKING_DIRECTORY ${MEPA_BUILD_DIR}       
)
execute_process(COMMAND make -j${nproc}
                WORKING_DIRECTORY ${MEPA_BUILD_DIR}
)

# 7. Link Static Libraries
# Using absolute paths as per your original Makefile
target_link_libraries(mepa_app_sanity_check_malibu PRIVATE 
    ${MEPA_BUILD_DIR}/mepa/libmepa.a
    ${MEPA_BUILD_DIR}/mepa/libmepa_common.a
    ${MEPA_BUILD_DIR}/mepa/vtss/libmepa_drv_vtss_custom.a
)
