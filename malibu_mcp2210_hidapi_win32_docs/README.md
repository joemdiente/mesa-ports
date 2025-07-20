# Malibu phy_demo_appl for Win32 using MCP2210-HIDAPI

This code uses a library called MCP2210-HIDAPI to be able to run 
phy_demo_appl for Malibu PHY on Windows.

## Hardware Setup
* MCP2210 Breakout Module - [ADM00419](https://www.microchip.com/en-us/development-tool/adm00419)

* VSC8258EV Evaluation Board - [VSC8258EV](https://www.microchip.com/en-us/development-tool/vsc8258ev)
    * Make sure to add jumper/short in `J33 CLK to GND`.

| MCP2210 Breakout Module  | VSC8258 Evaluation Board |
| ------------------------ |:------------------------:|
| Pin 1   GPIO0            | J48 SSN                  |
| Pin 6   MOSI             | J48 MOSI                 |
| Pin 7   SCK              | J48 SCLK                 |
| Pin 8   MISO             | J48 MISO                 |
| Pin 13  GND              | J48 GND                  |

## Software Setup
### Pre-requisites 
* __MCP2210-HIDAPI__ Library 
    * [Branch: `main`](https://github.com/joemdiente/mcp2210-hidapi)
    * Install [MSYS2](https://www.msys2.org/):
        * Run __MSYS2 UCRT64__ and __install__ the following:
            ```
            pacman -S mingw-w64-ucrt-x86_64-toolchain 
            ```
        * Select "__All__" and "__Yes__".
* __Fork of Microchip Ethernet Switch API__ 
    * [Branch: `v2021.09-mesa-ports`](https://github.com/joemdiente/mesa/tree/v2021.09-mesa-ports)

### Build Instructions
1. Initialize submodule
    ```
    # git submodule init
    # git submodule update
    ```
2. Open bash and run build script.
    ```
    # bash
    # ./build_malibu_mcp2210_hidapi_win32.sh
    ```

### Run Instructions 
Make sure that __hidapi.dll__ is alongside __malibu_mcp2210.exe__.
