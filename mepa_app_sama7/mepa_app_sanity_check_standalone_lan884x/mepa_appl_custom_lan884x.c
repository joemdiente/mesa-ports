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
// void *mem_alloc(struct mepa_callout_ctx *ctx, size_t size)
// {
//     return malloc(size);
// }

// void mem_free(struct mepa_callout_ctx *ctx, void *ptr)
// {
//     free(ptr);
// }
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
    int test_mode = 1; // For debugging only.
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

        // memset(&inst.callout_ctx[port_no], 0, sizeof(inst.callout_ctx[port_no]));

        // //Register Callouts (All of these are required)
        // memset(&inst.callout[port_no], 0, sizeof(inst.callout[port_no]));
        // inst.callout[port_no].spi_read = spi_32bit_malibu_read_spidev;
        // inst.callout[port_no].spi_write = spi_32bit_malibu_write_spidev;
        // inst.callout[port_no].mem_alloc = mem_alloc;
        // inst.callout[port_no].mem_free = mem_free;
        // inst.board_conf.numeric_handle = port_no;

        // Create MEPA device for this port. The MEPA device will be used by the application to call MEPA APIs for this port.
        // inst.phy[port_no] = mepa_create(&inst.callout[port_no], &inst.callout_ctx[port_no], &inst.board_conf);
        
        if (inst.phy[port_no]) {
            printf("Phy has been probed on port %d\r\n", port_no);
        }
        else {
            printf("Failed to probe PHY, port_no: %d\r\n", port_no);
            exit(EXIT_FAILURE);
        }

        // mepa_driver_link_base_port not implemented on Malibu(??)
    }

    // Attach here sample application
    printf("Jump to Sample Application.\r\n");
    if (mepa_app_sample_appl == NULL) {
        printf("Error: No Sample Application\r\n");
        exit(EXIT_FAILURE);
    }
    else {
        // Pass all required MEPA instance to sample application.
        mepa_app_sample_appl(&inst);
    }
    exit(EXIT_SUCCESS);
}

// *****************************************************************************
// End of file