cmake_minimum_required(VERSION 3.11)
project(mepa_app_sama7_malibu C)

set(CMAKE_BUILD_TYPE "Release")

# option(mepa_app_port_linkup "Build mepa_app_sanity_check using mepa else build phy-demo-appl" ON)

# Define Paths
set(MEPA_MAIN_ENTRY_SRC "${CMAKE_CURRENT_SOURCE_DIR}/")
set(MEPA_MAIN_ENTRY_INC "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(MEPA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../sw-mepa/")
set(MEPA_BUILD_DIR "${MEPA_DIR}/build")

# Executable and Sources
add_executable(mepa_app_sample_applications 
    ${MEPA_MAIN_ENTRY_SRC}/main.c
    ${MEPA_MAIN_ENTRY_SRC}/linux_spidev.c   
)

# Include Directories
target_include_directories(mepa_app_sample_applications PRIVATE 
    ${MEPA_MAIN_ENTRY_INC}/
    ${MEPA_BUILD_DIR}/include_common
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

set(MEPA_USE_DOT_A OFF)
if (MEPA_USE_DOT_A) 
    # Link Static Libraries
    target_link_libraries(mepa_app_sample_applications PRIVATE 
        ${MEPA_BUILD_DIR}/mepa/libmepa.a
        ${MEPA_BUILD_DIR}/mepa/libmepa_common.a
        ${MEPA_BUILD_DIR}/mepa/vtss/libmepa_drv_vtss_custom.a
    )
else()
    # Add mepa_api
    include(FetchContent)
    set(MEPA_vtss ON CACHE BOOL "" FORCE)
    set(MEPA_vtss_opt_10g ON CACHE BOOL "" FORCE)
    set(BUILD_MEPA_DEMO OFF CACHE BOOL "" FORCE)
    set(MEPA_vtss_opt_1g ON CACHE BOOL "" FORCE)
    set(MEPA_vtss_opt_ts ON CACHE BOOL "" FORCE)
    set(MEPA_vtss_opt_macsec ON CACHE BOOL "" FORCE)
    set(CMAKE_BUILD_TYPE Debug)

    FetchContent_Declare(
    mepa
    SOURCE_DIR "${MEPA_DIR}"
    )

    #  Bring the targets into the project
    FetchContent_MakeAvailable(mepa)

    # Link against the library produced by proj_B
    target_link_libraries(mepa_app_sample_applications PRIVATE mepa)
endif()


