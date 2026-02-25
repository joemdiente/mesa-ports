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
#include "sama7_mdio.h"
#include "mdio.h"
#include <linux/mdio.h> // Use for #define PHY register

/* Initialize Variables */


////////////////////////////////////////////////////////////////////////////////
// MDIO Functions for SAMA7 or Any Linux Machine.
// This is a copy of wkz/mdio-tools/blob/master/src/mdio/main.c
////////////////////////////////////////////////////////////////////////////////

void mdio_initialize(void) {

    // Not yet building
	// if (mdio_init()) {
	// 	if (mdio_modprobe()) {
	// 		fprintf(stderr, "ERROR: mdio-netlink module not "
	// 			"detected, and could not be loaded.\n");
	// 		return 1;
	// 	}

	// 	if (mdio_init()) {
	// 		fprintf(stderr, "ERROR: Unable to initialize.\n");
	// 		return 1;
	// 	}
	// }
    

}

////////////////////////////////////////////////////////////////////////////////
// Wrapper Functions for MEPA
////////////////////////////////////////////////////////////////////////////////


// *******************************
// END OF FILE