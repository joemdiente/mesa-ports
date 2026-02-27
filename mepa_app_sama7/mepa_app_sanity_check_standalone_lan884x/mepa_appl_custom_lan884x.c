/* 
 * Custom program for LAN8841 EDS2 Daughterboard using MEPA.See docs for setup.
 * Code is loosely-based on https://github.com/MicrochipTech/mepa-app-malibu10-rpi
 * 
 * Copyright (C) 2026 Microchip Technology Inc.
 *
 * Author: Joemel John A. Diente <JoemelJohn.Diente@microchip.com>
 * For support, please reach out to microchip.com/support.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

// *****************************************************************************
// *****************************************************************************
// Section: Header Includes
// *****************************************************************************
// *****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "microchip/ethernet/phy/api.h"
#include "linux_mdio.h"
#include "main.h"
appl_inst_t inst;
// *****************************************************************************
// *****************************************************************************
// Section: Function prototypes
// *****************************************************************************
// *****************************************************************************
// Required by MEPA
void *mem_alloc(struct mepa_callout_ctx *ctx, size_t size)
{
    return malloc(size);
}

void mem_free(struct mepa_callout_ctx *ctx, void *ptr)
{
    free(ptr);
}
// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
int main(int argc, char* argv[]) {

    int ret = 0;
    int test_mode = 0; // For debugging only.
    mepa_port_no_t port_no = 0;
    char* mdio_bus = "";
    uint8_t phy_id = 0;

    // Check Arguments
    if (argc > 1) {
        if (strcmp(argv[1],"-h") == 0) {
            printf("Help File: \r\n");
            printf("   Options in order: \r\n");
            printf("       -h                       Show this help \r\n");
            printf("       -m <mdio-bus> <phy_id>   Open mdio bus \r\n");
            printf("            Run -m without arguments to see available mdio buses.\r\n");
            printf("            Make sure to use exact mdio bus name!.\r\n");
            printf("       -t                  Turn on test_mode \r\n");
            exit(EXIT_FAILURE);
        }
        else if (strcmp(argv[1],"-m") == 0) {
            if (argc < 3) {
                printf("No mdio bus given.\r\n");
                // Do not return failure here. Instead, let mdio_initialize handle it and print the available mdio buses.
            }
            else {
                mdio_bus = argv[2];
                printf("Using mdio bus: %s\r\n", mdio_bus);
                if (argc < 4) {
                    printf("No phy_id. Using default phy_id 0.\r\n");
                    phy_id = 0;
                } else {
                    // Just convert here to int. phy_id is checked in linux_mdio.c
                    uint8_t x = atoi(argv[3]);
                    phy_id = x;
                }
            }
        }
    }

    if (argc < 1) {
        printf("Error: Require Arguments. See -h\r\n");
        exit(EXIT_FAILURE);
    }
    
    if(mdio_initialize(mdio_bus, &phy_id) != 0) {
        printf("Failed to initialize mdio\r\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Successfully initialized mdio_bus!\r\n");
    }

    if (test_mode == 1) {
        mdio_test_code(); 
    }

    /* MEPA Initialization
     * Take ./sw-mepa/mepa_demo/mepa_apps/phy_port_config.c as an example.
     */ 
    memset(&inst.board_conf, 0, sizeof(inst.board_conf));

    // Create MEPA Devices
    for (port_no = 0; port_no < LAN884X_PORT_COUNT; port_no++) {

        memset(&inst.callout_ctx[port_no], 0, sizeof(inst.callout_ctx[port_no]));

        //Register Callouts (All of these are required)
        memset(&inst.callout[port_no], 0, sizeof(inst.callout[port_no]));
        inst.callout[port_no].miim_read = mdio_read;
        inst.callout[port_no].miim_write = mdio_write;
        inst.callout[port_no].mem_alloc = mem_alloc;
        inst.callout[port_no].mem_free = mem_free;
        inst.board_conf.numeric_handle = port_no;

        // Create MEPA device for this port. The MEPA device will be used by the application to call MEPA APIs for this port.
        inst.phy[port_no] = mepa_create(&inst.callout[port_no], &inst.callout_ctx[port_no], &inst.board_conf);
        
        if (inst.phy[port_no]) {
            printf("Phy has been probed on port %d\r\n", port_no);
        }
        else {
            printf("Failed to probe PHY, port_no: %d\r\n", port_no);
            exit(EXIT_FAILURE);
        }

        // mepa_driver_link_base_port not implemented on Malibu(??)
    }

    // 1. Simple - Reset PHY
    printf("\r\n MEPA Reset all PHYs\r\n");
    mepa_reset_param_t rst_conf = {};
    rst_conf.reset_point = MEPA_RESET_POINT_PRE;
    
    for (port_no = 0; port_no < LAN884X_PORT_COUNT; port_no++) {
        // Reset PHY ports
        ret = mepa_reset(inst.phy[port_no], &rst_conf);
        if (ret) {
            printf("Failed to reset PHY, port_no: %d\r\n", port_no);
            return ret;
        }
        else {
            printf("PHY Reset Success, port_no: %d\r\n", port_no);
        }
    }

    // 2. Register Read/Write from MEPA
    printf("\r\n MEPA Register Read Only\r\n");
    port_no = 0;
    uint32_t value = 0;
    inst.callout[port_no].miim_read(NULL, 0x2, &value);
    printf("PHY Register 2: 0x%04X\n", value);

    // 3. PHY Information Details
    printf("\r\n MEPA Get PHY Information Details\r\n");
    mepa_phy_info_t phy_info = {};

    for (port_no = 0; port_no < LAN884X_PORT_COUNT; port_no++) {
        if (mepa_phy_info_get(inst.phy[port_no], &phy_info) == 0) {
            printf("Port No: %d ; Part Number: 0x%X; Revision: 0x%X\r\n", 
                port_no, phy_info.part_number, phy_info.revision);
        } else {
            printf("Failed to get PHY information details\r\n");
        }
    }

    // 3. Simple - Configure PHY Operating Mode - Set to 10G Only
    printf("\r\n MEPA Port Configuration\r\n");
    mepa_conf_t conf;
    // Conf Get
    for (port_no = 0; port_no < LAN884X_PORT_COUNT; port_no++) {
        if (mepa_conf_get(inst.phy[port_no], &conf) < 0) {
            printf("Failed to get current port configuration for port %d\r\n", port_no);
        }
    }

    for (port_no = 0; port_no < LAN884X_PORT_COUNT; port_no++) {

        // PHY Configuration
        conf.speed = MESA_SPEED_1G;
        conf.fdx = true;
        conf.flow_control = true;
        conf.aneg.speed_1g_fdx = true;

        if (mepa_conf_set(inst.phy[port_no], &conf) == 0) {
            printf("Port %d configuration success.\r\n", port_no);
        } else {
             printf("Port %d configuration failed.\r\n", port_no);
        }
    }

    // 4. Simple - Poll PHY Link Status
    printf("\r\n MEPA Poll PHY link status\r\n");
    mepa_status_t status = {};
    // See microchip/ethernet/common.h > mesa_port_speed_t
    char *portspeed2txt[] = {
        "Undefined",
        "10 Mbps",
        "100 Mbps",
        "1000 Mbps",
        "2500 Mbps",
        "5 Gbps",
        "10 Gbps",
        "12 Gbps",
        "25 Gbps",
        "Auto",
    };

    while(1) {
        printf("\033[2J\033[H");
        printf(" MEPA Poll Link Information per Port \r\n");
        for (port_no = 0; port_no < LAN884X_PORT_COUNT; port_no++) {
            if (mepa_poll(inst.phy[port_no], &status) == 0) {
                printf("Port: %d, Speed: %s, fdx: %s, Cu: %s, Fi: %s, Link: %s\n", port_no, portspeed2txt[status.speed], \
                status.fdx? "Yes":"No", status.copper? "Yes":"No", status.fiber? "Yes":"No", status.link? "Up" : "Down");
            } else {
                printf("Port %d poll failed.\r\n", port_no);
            }
        }
        fflush(stdout);
        sleep(1);
    }

    return ret;
    exit(EXIT_SUCCESS);
}

// *****************************************************************************
// End of file