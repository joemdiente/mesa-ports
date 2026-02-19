
# MEPA Sample Applications

Available sample applications:
- mepa_app_sanity_check = Use for checking test setup. 
    - Do not follow "How to use" in this README.md.
    - Please refer to `mepa_app_sanity_check/README.md`
- mepa_app_gpio_demo =
- mepa_app_port_linkup =
- mepa_app_warm_startup =
- mepa_app_power_modes = Try different power modes in a menu.
- mepa_app_cable_diag =

## How to use:

1. Make sure you are in `mepa_app_sama7_malibu` root directory.
2. `$ mkdir ./build/ && cd ./build/`
3. `$ cmake ../${your_desired_sample_app}/ -DCMAKE_TOOLCHAIN_FILE=${your_toolchainfile.cmake}`

    ex. `$ cmake ../mepa_app_sanity_check/ -DCMAKE_TOOLCHAIN_FILE=$(find /opt/mscc/sama7-toolchain/ -name "toolchainfile.cmake")`
    
    - Builds mepa_app_sanity_check

4. `$ make`