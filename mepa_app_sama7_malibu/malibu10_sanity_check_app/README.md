#
# This simply builds example code taken from repository https://github.com/MicrochipTech/mepa_app_malibu10_rpi.
# This is used as a starting point when developing own application.
# Used for sanity check, example code includes simple IO test.
#

CMAKE FLAGS:
    -Dmepa_app_new_api
        ON = Builds mepa
            will configure,generate and build ../sw-mepa first.
            see CMakeLists.txt for flags used for the "sw-mepa".

        OFF = Builds phy_demo_appl.

    CMAKE_TOOLCHAIN_FILE
        Make sure to add toolchainfile.cmake otherwise will build natively.

Known toolchainfile used.
/opt/mscc/sama7-toolchain/share/buildroot/toolchainfile.cmake