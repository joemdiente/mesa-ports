# MEPA Sample Application - Sanity Check

- This simply builds example code taken from repository https://github.com/MicrochipTech/mepa_app_malibu10_rpi. \
- This is used as a starting point when developing own application. Used for sanity checking, example code includes simple IO test, simple mepa init, and basic port information.

### CMAKE FLAGS:

- `-Dmepa_app_new_api`

    - **ON** = Build sanity check app using sw-mepa/.
        - will configure,generate and build ../sw-mepa first.
        - see CMakeLists.txt for flags used for the "sw-mepa".
    - **OFF** = Build sanity_check app using ../mesa/.
        - uses phy_demo_appl for malibu.

<br>

- `-DCMAKE_TOOLCHAIN_FILE`
    - Make sure to add `toolchainfile.cmake` otherwise it will build natively.
        - Example toolchain directory: /opt/mscc/sama7-toolchain/share/buildroot/toolchainfile.cmake

### Expected Output
&nbsp;&nbsp;&nbsp;&nbsp;  Test setup: VSC8258EV connected to SAMA7D65 Curiosity Kit Mikrobus 1 through Linux SPIDEV. \
&nbsp;&nbsp;&nbsp;&nbsp; No SFP connected to VSC8258EV.
````
$ ./mepa_app_sanity_check
Raspberry Pi 5 Malibu Code.
main (ERROR): Error at 128
//Default Setup Assumptions:
//Board has Power Applied prior to Demo start
//Pwr Supply/Voltages Stable, Ref Clk Stable, Ref & PLL Stable

main: Connecting to the Malibu board via SPI...
main: Initializing RPI SPI...
In Line 149: Dev ID = 0x8258

mepa_create: rc: 0

appl_malibu_spi_io_test

Test reading global device ID register...
0x1Ex0000 = 0x8258
1x1Ex0000 = 0x8258
2x1Ex0000 = 0x8258

Test writing to 0x1xF112...
0x1xF112 = 0x7DF820
0x1xF112 = 0x3DF828

Test writing to 1x1xF112...
1x1xF112 = 0x7DF820
1x1xF112 = 0x4DF828

Test writing to 2x1xF112...
2x1xF112 = 0x7DF820
2x1xF112 = 0x5DF828
2x1xF112 = 0x7DF820

Test writing to 3x1xF112...
3x1xF112 = 0x7DF820
3x1xF112 = 0x6DF828
3x1xF112 = 0x7DF820

Test reading from 0x1xF120...
0x1xF120 = 0x4104E26
1x1xF120 = 0x4104CA6
2x1xF120 = 0x4104EA6
3x1xF120 = 0x4104CA6

Test writing to 0x1xF121...
0x1xF121 = 0x88888924
0x1xF121 = 0x48888924
0x1xF121 = 0x88888924


Resetting port 0
appl_mepa_reset_phy: rc: 0



Resetting port 1
appl_mepa_reset_phy: rc: 0



Resetting port 2
appl_mepa_reset_phy: rc: 0



Resetting port 3
appl_mepa_reset_phy: rc: 0



Configuring port 0
malibu_b_host_pll5g_register_cfg (ERROR): Failed to initialize Host LCPLL as the gain was too low
appl_mepa_phy_init: rc: 0



Configuring port 1
malibu_b_line_pma_sd10g_rx_register_cfg (ERROR): The expected value for SD10G65_rx_rcpll_stat0 pllf_lock_stat was 0x1 but is 0x0
malibu_b_line_pma_sd10g_rx_register_cfg (ERROR): The expected value for SD10G65_rx_rcpll_stat1 pllf_fsm_stat was 0xd but is 0x6
malibu_b_line_pma_apc10g_register_cfg (ERROR): The expected value for apc_is_cal_cfg1 offscal_done was 0x1 but is 0x0
malibu_b_host_pma_sd10g_tx_register_cfg (ERROR): The expected value for sd10g65_tx_rcpll_stat0 pllf_lock_stat was 0x1 but is 0x0
malibu_b_host_pma_sd10g_tx_register_cfg (ERROR): The expected value for sd10g65_tx_rcpll_stat1 pllf_fsm_stat was 0xd but is 0x0
appl_mepa_phy_init: rc: 0



Configuring port 2
appl_mepa_phy_init: rc: 0



Configuring port 3
malibu_b_host_pma_sd10g_rx_register_cfg (ERROR): The expected value for SD10G65_rx_rcpll_stat0 pllf_lock_stat was 0x1 but is 0x0
malibu_b_host_pma_sd10g_rx_register_cfg (ERROR): The expected value for SD10G65_rx_rcpll_stat1 pllf_fsm_stat was 0xd but is 0x6
malibu_b_host_pma_apc10g_register_cfg (ERROR): The expected value for apc_is_cal_cfg1 offscal_done was 0x1 but is 0x0
appl_mepa_phy_init: rc: 0



mepa_phy_info_get: rc: 0, PHY Cap: 0x24, Part: 0x8258
Port: 0, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 1, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 2, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 3, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down

Port: 0, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 1, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 2, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 3, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down

Port: 0, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 1, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 2, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 3, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down

Port: 0, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 1, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 2, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 3, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down

Port: 0, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 1, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 2, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 3, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down

Port: 0, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 1, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 2, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 3, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down

Port: 0, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 1, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 2, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 3, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down

Port: 0, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 1, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 2, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 3, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down

Port: 0, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 1, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 2, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 3, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down

Port: 0, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 1, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 2, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down
Port: 3, rc: 0, Speed: Undefined, fdx: No, Cu: No, Fi: No, Link: Down

````