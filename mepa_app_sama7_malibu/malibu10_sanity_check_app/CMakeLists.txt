cmake_minimum_required(VERSION 3.10)
project(malibu10_sanity_check_app C)

set(CMAKE_BUILD_TYPE "Release")

option(mepa_app_new_api "Build malibu10_sanity_check_app using new API else build phy-demo-appl" ON)

if (mepa_app_new_api)
    # 1. Define Paths
    set(MEPA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../sw-mepa")
    set(MEPA_BUILD_DIR "${MEPA_DIR}/build")

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
    target_link_libraries(malibu10_sanity_check_app PRIVATE 
        ${MEPA_BUILD_DIR}/mepa/libmepa.a
        ${MEPA_BUILD_DIR}/mepa/libmepa_common.a
        ${MEPA_BUILD_DIR}/mepa/vtss/libmepa_drv_vtss.a
        ${MEPA_BUILD_DIR}/mepa/vtss/libmepa_drv_vtss_custom.a
    )
else()
    # 1. Define Paths (Mirroring your MEPA_CMAKE_BUILD_DIR)
    set(MESA_PHY_DEMO_APPL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../mesa/phy_demo_appl")

    # 2. Collect Source Files
    # Explicitly listed files
    set(SOURCES
        ${MESA_PHY_DEMO_APPL_DIR}/appl/vtss_appl_board_malibu.c
        ${MESA_PHY_DEMO_APPL_DIR}/appl/vtss_appl_10g_phy_malibu.c
        ${MESA_PHY_DEMO_APPL_DIR}/appl/vtss_appl_macsec_demo.c
        ${MESA_PHY_DEMO_APPL_DIR}/appl/vtss_appl_ts_demo.c
        ${MESA_PHY_DEMO_APPL_DIR}/../base/ail/vtss_wis_api.c
        ${MESA_PHY_DEMO_APPL_DIR}/../base/ail/vtss_api.c
        ${MESA_PHY_DEMO_APPL_DIR}/../base/ail/vtss_port_api.c
        ${MESA_PHY_DEMO_APPL_DIR}/../base/ail/vtss_common.c
        ${MESA_PHY_DEMO_APPL_DIR}/../base/ail/vtss_sd10g65_procs.c
        ${MESA_PHY_DEMO_APPL_DIR}/../base/ail/vtss_sd10g65_apc_procs.c
        ${MESA_PHY_DEMO_APPL_DIR}/../base/ail/vtss_pll5g_procs.c
        ${MESA_PHY_DEMO_APPL_DIR}/../base/phy/common/vtss_phy_common.c
        ${MESA_PHY_DEMO_APPL_DIR}/appl/vtss_version.c
    )

    # Replacing the 'find' shell commands with CMake globbing
    file(GLOB_RECURSE PHY_10G_SRCS "${MESA_PHY_DEMO_APPL_DIR}/../base/phy/phy_10g/*.c")
    file(GLOB_RECURSE TS_SRCS "${MESA_PHY_DEMO_APPL_DIR}/../base/phy/ts/*.c")
    file(GLOB_RECURSE MACSEC_SRCS "${MESA_PHY_DEMO_APPL_DIR}/../base/phy/macsec/*.c")

    # Combine all source lists
    list(APPEND SOURCES ${PHY_10G_SRCS} ${TS_SRCS} ${MACSEC_SRCS})

    # 3. Set Compiler Flags and Definitions
    add_definitions(
        -DVTSS_OPT_PORT_COUNT=4
        -DVTSS_OPSYS_LINUX=1
        -DVTSS_FEATURE_10G
        -DVTSS_FEATURE_10GBASE_KR
        -DVTSS_FEATURE_SERDES_MACRO_SETTINGS
        -DVTSS_USE_STDINT_H
        -D_INCLUDE_DEBUG_TERM_PRINT_
        -DMALIBU_CHAR_BOARD
        -DVTSS_FEATURE_MACSEC
        -DVTSS_OPT_PHY_TIMESTAMP
    )
    # 4. Define Include Directories
    include_directories(
        ${MESA_PHY_DEMO_APPL_DIR}/../include
        ${MESA_PHY_DEMO_APPL_DIR}/../base/phy/phy_10g
    )

    # 5. Compile Definitions (Macros)
    add_executable(malibu10_sanity_check_app ${SOURCES})

endif()