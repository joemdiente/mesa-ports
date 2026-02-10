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
 * Template Test
 * Make sure to add documentation in this simple app.
 * 
 */
// *****************************************************************************
// *****************************************************************************
// Section: Header Includes
// *****************************************************************************
// *****************************************************************************
#include "main.h"
#include <stdio.h>
#define MY_DEBUG
#define MY_DEBUG_SHOW_ADVANCED
#include "my_debug.h"

int mepa_app_sample_appl(appl_inst_t *inst)
{
    PRINT_FUN();
    int ret = 0;

    // 1. Simple - Reset PHY
    mepa_port_no_t port_no = 0;
    mepa_reset_param_t rst_conf = {};
    rst_conf.reset_point = MEPA_RESET_POINT_PRE;
    
    for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
        // Reset PHY ports
        ret = mepa_reset(inst->phy[port_no], &rst_conf);
        if (ret) {
            printf("Failed to reset PHY, port_no: %d\r\n", port_no);
            return ret;
        }
        else {
            printf("PHY Reset Success, port_no: %d\r\n", port_no);
        }
    }

    // 2. Simple - Configure PHY Operating Mode - Set to 1G Only

    // 3. Simple - Poll PHY Link Status

    // 4. Simple - Connect to VSC7514EV
    
    return ret;
}

// *****************************************************************************
// End of file