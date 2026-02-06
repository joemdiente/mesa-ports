cmake_minimum_required(VERSION 3.10)
project(mepa_app_sama7_malibu C)

set(CMAKE_BUILD_TYPE "Release")

# option(mepa_app_port_linkup "Build mepa_app_sanity_check using mepa else build phy-demo-appl" ON)


# Define Paths
set(MEPA_MAIN_ENTRY_SRC "${CMAKE_CURRENT_SOURCE_DIR}/")
set(MEPA_MAIN_ENTRY_INC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Executable and Sources
add_executable(mepa_app_sample_applications 
    ${MEPA_MAIN_ENTRY_SRC}/main.c
    ${MEPA_MAIN_ENTRY_SRC}/linux_spidev.c   
)

# Include Directories
target_include_directories(mepa_app_sample_applications PRIVATE 
    ${MEPA_MAIN_ENTRY_INC}/
)

# Compile Definitions (Macros)
target_compile_definitions(mepa_app_sample_applications PRIVATE APPL_TRACE_LVL_ERROR)

# Compile Options (Flags)
target_compile_options(mepa_app_sample_applications PRIVATE 
    -g 
    -Wall 
    -Wextra
    -Wno-error=implicit-function-declaration 
    -Wno-error=int-conversion 
    -Wno-error=incompatible-pointer-types
)

# Link Static Libraries
# target_link_libraries(mepa_app_sample_applications PRIVATE 
#     ${MEPA_BUILD_DIR}/mepa/libmepa.a
#     ${MEPA_BUILD_DIR}/mepa/libmepa_common.a
#     ${MEPA_BUILD_DIR}/mepa/vtss/libmepa_drv_vtss_custom.a
# )
