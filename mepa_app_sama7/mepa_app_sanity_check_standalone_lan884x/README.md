# MEPA Sample Application - Sanity Check

- This simply builds a sanity check code to check if hardware works.
    - Uses MDIO-NETLINK (https://github.com/wkz/mdio-tools) for MDIO BUS Access. 
    - MEPA code was  referenced from repository https://github.com/MicrochipTech/mepa_app_malibu10_rpi.
- This is used as a starting point when developing own application. Used for sanity checking, example code includes simple IO test, simple mepa init, and basic port information.
- Note: This is not a production code! If there are any changes, you are on your own.

### CMAKE FLAGS:

- `None as of now`

<br>

- `-DCMAKE_TOOLCHAIN_FILE`
    - Make sure to add `toolchainfile.cmake` otherwise it will build natively.
        - Example toolchain directory: /opt/mscc/sama7-toolchain/share/buildroot/toolchainfile.cmake

### Test Setup
- LAN8841 PHY Daughterboard connected to SAMA7D65 Curiosity Kit EDS2 Connector.
- Make sure device tree overlay is added in BSP as follows.
````
&gmac1 {
    #address-cells = <1>;
    #size-cells = <0>;
    phy-mode = "rgmii-id";
    status = "okay";

    // Fix MAC at 1000Mbps
    fixed-link {
        speed = <1000>;
        full-duplex;
    };

    // Add mdio to appear in mdio-tools
    mdio {
        #address-cells = <1>;
        #size-cells = <0>;

        // Make sure disabled status of PHY connected to EDS2 to avoid loading drivers. 
        // In this case, PHYAD = 1 because of @1 & reg = <1>;
        ethernet-phy@1 {
            reg = <1>;
            status = "disabled";
        };
    };
};

````

### Expected Output
- See docs/app_log.txt