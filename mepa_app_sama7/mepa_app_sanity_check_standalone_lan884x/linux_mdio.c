/* 
 * MDIO Accessor Function for MEPA
 *
 * Copyright (C) 2026 Microchip Technology Inc.
 *
 * Author: Joemel John A. Diente <joemeljohn.diente@microchip.com>
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

#include <stdarg.h> // For va_list
#include <unistd.h>     // For TCP read/write
#include <sys/stat.h>// For /sys/class/mdio file status
#include <sys/types.h>// For /sys/class/mdio file status
#include <errno.h>// For System errors
#include <unistd.h>// For /sys/class/mdio
#include <stdio.h> // for FILE types
#include <stdlib.h> // for EXIT_SUCCESS

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include "linux_mdio.h"
#include "mdio.h"
#include <linux/mdio.h> // Use for #define PHY register

/* Initialize Variables */

////////////////////////////////////////////////////////////////////////////////
// MDIO Functions for SAMA7 or Any Linux Machine.
// This is a copy of wkz/mdio-tools/blob/master/src/mdio/main.c
////////////////////////////////////////////////////////////////////////////////
int list_available_mdio_buses() {
	DIR *d;
	struct dirent *dir;
	d = opendir("/sys/class/mdio_bus");
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_type == DT_LNK) {
				printf("--> %s\n", dir->d_name);
			}
		}
		closedir(d);
		return 0;
	} else {
		fprintf(stderr, "ERROR: Unable to open /sys/class/mdio_bus.\r\n");
		return -1;
	}
}

int match_available_mdio_bus(char* mdio_bus) {
	DIR *d;
	struct dirent *dir;
	d = opendir("/sys/class/mdio_bus");
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_type == DT_LNK) {
				if(strcmp(dir->d_name, mdio_bus) == 0) {
					printf("Selected mdio bus: %s\n", dir->d_name);
					return 0;
				}
			}
		}
		closedir(d);
		printf("ERROR: mdio bus \"%s\" not found in /sys/class/mdio_bus.\r\n", mdio_bus);
		return -1;
	} 
	printf("ERROR: Unable to open /sys/class/mdio_bus.\r\n");
	return -1;
}
int mdio_initialize(char* mdio_bus) {

	if (mdio_init()) {
		if (mdio_modprobe()) {
			fprintf(stderr, "ERROR: mdio-netlink module not "
				"detected, and could not be loaded.\n");
			return -1;
		}

		if (mdio_init()) {
			fprintf(stderr, "ERROR: Unable to initialize.\n");
			return -1;
		}
	}

	printf("mdio-netlink module loaded or already running.\r\n");

	if (strcmp(mdio_bus, "") == 0) {
		printf("Available mdio buses: \r\n");
		list_available_mdio_buses();
		printf("Please pass one of the above as an \'exact\' argument using -m <mdio_bus>. \r\n");
		printf("Using wildcard is not supported. \r\n");
		return -1;
	} else {
		if (match_available_mdio_bus(mdio_bus) != 0) {
			exit(EXIT_FAILURE);
		}
	} 

	return 0;
}

int mdio_test_code(char* mdio_bus, uint8_t phy_id) {

	int i = 0;

	// Get PHY ID
}
////////////////////////////////////////////////////////////////////////////////
// Wrapper Functions for MEPA
////////////////////////////////////////////////////////////////////////////////


// *******************************
// END OF FILE