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

#define DEBUG_MEPA_APP
#define DEBUG_MEPA_APP_SHOW_ADVANCED
// *****************************************************************************
// *****************************************************************************
// Section: Header Includes
// *****************************************************************************
// *****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linux_spidev.h"
#include "main.h"
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

    int ret = 0;
    char* spidev = "/dev/spidev0.1";

    // Check Arguments
    if (argc > 2) {
        if (strcmp(argv[1],"-h") == 0) {
        printf(" Help File: \r\n \
            Options:\
                -d /dev/spidevx.y \r\n \
            ");
        exit(EXIT_FAILURE);
        }
        if (strcmp(argv[1],"-d") == 0) {
            spidev = (char*)argv[2];
        }
    }
    if (argc < 2) {
        printf("Error: Require Arguments. See -h\r\n");
        exit(EXIT_FAILURE);
    }

    printf("Using spidev: %s\r\n", spidev);

    // Initialize SPI
    ret = spi_initialize(spidev);

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