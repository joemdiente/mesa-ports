/* 
 *
 * Copyright (C) 2026 Microchip Technology Inc.
 *
 * Author: Joemel John A. Diente <JoemelJohn.Diente@microchip.com>
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

 /*
  * This code is main entry point for all MEPA Sample Applications.
  * 1 Week Training Output for "Introduction to MEPA".
  * 
  */

#define MY_DEBUG
#define MY_DEBUG_SHOW_ADVANCED
#include "my_debug.h"
// *****************************************************************************
// *****************************************************************************
// Section: Header Includes
// *****************************************************************************
// *****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linux_spidev.h"
#include "microchip/ethernet/phy/api.h"
#include "microchip/ethernet/board/api.h"
// *****************************************************************************
// *****************************************************************************
// Section: Macros and Constant defines
// *****************************************************************************
// *****************************************************************************
#define MALIBU_EVB_PORT_COUNT 4
#define MALIBU_BASE_PORT 0
// *****************************************************************************
// *****************************************************************************
// Section: Function prototypes
// *****************************************************************************
// *****************************************************************************
extern __attribute__((weak)) int mepa_app_sample_appl(); // Use "weak" so it is NULL in memory.

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
// Section: MEPA Structs
// *****************************************************************************
// *****************************************************************************
mepa_callout_t callout;
mepa_callout_ctx_t *callout_ctx;
mepa_board_conf_t board_conf = {};
mepa_device_t *phy[MALIBU_EVB_PORT_COUNT];
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

    PRINT_FUN();
    int ret = 0;
    char* spidev = "/dev/spidev0.1";
    int test_mode = 0;
    int port_no = 0;

    // Check Arguments
    if (argc > 1) {
        if (strcmp(argv[1],"-h") == 0) {
            printf("Help File: \r\n");
            printf("   Options: \r\n");
            printf("       -h                  Show this help \r\n");
            printf("       -d /dev/spidevx.y   Open spidev X.Y \r\n");
            printf("       -t                  Turn on test_mode \r\n");
            exit(EXIT_FAILURE);
        }
        else if (strcmp(argv[1],"-d") == 0) {
            if (strcmp(argv[2], "") == 0) {
                printf("No spi dev node given.\r\n");
                printf("Using default /dev/spidev0.1. \r\n");
                spidev = (char*)argv[2];
            }
            else {
                printf("Using spidev: %s\r\n", spidev);
                spidev = (char*)argv[2];
            }

        }
        else if (strcmp(argv[1], "-t") == 0) {
            printf(" test_mode = 0\r\n");
            test_mode = 1;
        }
    }
    if (argc < 1) {
        printf("Error: Require Arguments. See -h\r\n");
        exit(EXIT_FAILURE);
    }

    // Initialize SPI
    spi_conf_t dev_spi;
    dev_spi.spidev = spidev;
    dev_spi.bits = 8;
    dev_spi.mode = 0;
    dev_spi.speed = 1000000; //1 MHz
    
    ret = spi_initialize(dev_spi);
    if (test_mode == 1) {
        spi_malibu_test_code();
    }

    /* MEPA Initialization
     * Take ./sw-mepa/mepa_demo/mepa_apps/phy_port_config.c as an example.
     */ 
    // Assign callout
    callout.spi_read = spi_32bit_malibu_read_spidev;
    callout.spi_write = spi_32bit_malibu_write_spidev;
    callout.mem_alloc = mem_alloc;
    callout.mem_free = mem_free;
    // callout.lock_enter = ;
    // callout.lock_exit = ;

    // Create MEPA Devices
    for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
        phy[port_no] = mepa_create(&callout, &callout_ctx[MALIBU_EVB_PORT_COUNT], &board_conf);
        if (phy[port_no]) {
            printf("Phy has been probed on port %d\r\n", port_no);
            if ((ret = mepa_link_base_port(phy[port_no],
                                        MALIBU_BASE_PORT,
                                        port_no)) != MESA_RC_OK) {
                printf(" Error in Linking base port to Port  : %d\n", port_no);
            }
        }
        else {
            printf("Failed to probe PHY, port_no: %d\r\n", port_no);
            exit(EXIT_FAILURE);
        }
    }

    // Attach here sample application
    printf("Jump to Sample Application.\r\n");
    if (mepa_app_sample_appl == NULL) {
        printf("Error: No Sample Application\r\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

// *****************************************************************************
// End of file