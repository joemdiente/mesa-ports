cmake_minimum_required(VERSION 3.11)
project(mepa_app_sama7_malibu C)

set(CMAKE_BUILD_TYPE "Debug")

# Define Paths
set(MEPA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../sw-mepa/")
set(MEPA_BUILD_DIR "${MEPA_DIR}/build")

set(sample_applications
    "mepa_app_main_entry"
    "mepa_app_simple"
    "mepa_app_port_linkup"
    "mepa_app_cable_diag"
    "mepa_app_power_modes"
    "mepa_app_warm_start"
)

set(MEPA_MAIN_ENTRY_SRC "${CMAKE_CURRENT_SOURCE_DIR}/")
set(MEPA_MAIN_ENTRY_INC "${CMAKE_CURRENT_SOURCE_DIR}/include")

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

# CMake Options
# By default, builds main entry point without sample application.
foreach(VAR ${sample_applications})
    option(${VAR} "Build ${VAR}" ON)

    if(NOT "${VAR}" STREQUAL "mepa_app_main_entry")
        message(STATUS "Building ${VAR}")

        add_executable(${VAR}
            ${CMAKE_CURRENT_SOURCE_DIR}/main.c
            ${CMAKE_CURRENT_SOURCE_DIR}/linux_spidev.c 
            ${CMAKE_CURRENT_SOURCE_DIR}/${VAR}/${VAR}.c
        )

        # Include Directories
        target_include_directories(${VAR} PRIVATE
            ${MEPA_MAIN_ENTRY_INC}/
            ${MEPA_BUILD_DIR}/include_common
        )

        # Link against the library produced by mepa
        target_link_libraries(${VAR} PRIVATE mepa)

            # Compile Options (Flags)
        target_compile_options(${VAR} PRIVATE
            -g 
            -Wall 
            -Wextra
            -Wno-error=implicit-function-declaration 
            -Wno-error=int-conversion 
            -Wno-error=incompatible-pointer-types
        )
    endif()
endforeach()

## main entry point application
#
# Build this for quick testing and validation of mepa.
# This code is for testing spidev and mepa instantiation.
# 
if(mepa_app_main_entry)
    message(STATUS "Building mepa_app_main_entry")

        ## main entry point aplplication
        add_executable(mepa_app_main_entry
            ${MEPA_MAIN_ENTRY_SRC}/main.c
            ${MEPA_MAIN_ENTRY_SRC}/linux_spidev.c   
        )

        # Include Directories
        target_include_directories(mepa_app_main_entry PRIVATE
            ${MEPA_MAIN_ENTRY_INC}/
            ${MEPA_BUILD_DIR}/include_common
        )

        # Link against the library produced by mepa
        target_link_libraries(mepa_app_main_entry PRIVATE mepa)

        # Compile Options (Flags)
        target_compile_options(mepa_app_main_entry PRIVATE
            -g 
            -Wall 
            -Wextra
            -Wno-error=implicit-function-declaration 
            -Wno-error=int-conversion 
            -Wno-error=incompatible-pointer-types
        )

endif()