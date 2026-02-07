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
// *****************************************************************************
// *****************************************************************************
// Section: Macros and Constant defines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Function prototypes
// *****************************************************************************
// *****************************************************************************
extern __attribute__((weak)) int mepa_app_sample_appl(); // Use "weak" so it is NULL in memory.

// *****************************************************************************
// *****************************************************************************
// Section: MEPA Structs
// *****************************************************************************
// *****************************************************************************

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

    // Check Arguments
    if (argc > 1) {
        if (strcmp(argv[1],"-h") == 0) {
            printf(" Help File: \r\n");
            printf("    Options: \r\n");
            printf("        -h                  Show this help \r\n");
            printf("        -d /dev/spidevx.y   Open spidev X.Y \r\n");
            printf("        -t                  Turn on test_mode \r\n");
            exit(EXIT_FAILURE);
        }
        else if (strcmp(argv[1],"-d") == 0) {
            if (strcmp(argv[2], "") == 0) {
                printf(" No spi dev node given.\r\n");
                printf(" Using default. \r\n");
            }
            spidev = (char*)argv[2];
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

    printf("Using spidev: %s\r\n", spidev);
    spi_conf_t dev_spi;
    dev_spi.spidev = spidev;
    dev_spi.bits = 8;
    dev_spi.mode = 0;
    dev_spi.speed = 1000000; //1 MHz
    
    // Initialize SPI
    ret = spi_initialize(dev_spi);
    if (test_mode == 1) {
        spi_malibu_test_code();
    }

    // MEPA Init

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