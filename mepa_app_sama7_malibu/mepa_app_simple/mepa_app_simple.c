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
#include <stdbool.h>

int mepa_app_sample_appl(appl_inst_t *inst)
{
    PRINT_FUN();
    int ret = 0;
    mepa_port_no_t port_no = 0;

    // 1. Simple - Reset PHY
    printf("\r\n MEPA Reset all PHYs\r\n");
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

    // 2. Register Read/Write from MEPA
    printf("\r\n MEPA Register Read Only\r\n");
    port_no = 0;
    uint32_t value = 0;
    inst->callout[port_no].spi_read(NULL, port_no, 0x1E, 0x00, &value);
    printf("DEV_ID: 0x%08x\n", value);

    // 3. PHY Information Details
    printf("\r\n MEPA Get PHY Information Details\r\n");
    mepa_phy_info_t phy_info = {};

    for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
        if (mepa_phy_info_get(inst->phy[port_no], &phy_info) == 0) {
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
    for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
        if (mepa_conf_get(inst->phy[port_no], &conf) < 0) {
            printf("Failed to get current port configuration for port %d\r\n", port_no);
        }
    }

    for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {

        // PHY Configuration
        conf.speed = MESA_SPEED_10G;
        conf.conf_10g.oper_mode = MEPA_PHY_LAN_MODE;
        conf.conf_10g.interface_mode = MEPA_PHY_SFI_XFI;
        conf.conf_10g.channel_id = MEPA_CHANNELID_NONE;
        conf.conf_10g.xfi_pol_invert = 1;
        conf.conf_10g.polarity.host_rx = false;
        conf.conf_10g.polarity.line_rx = false;
        conf.conf_10g.polarity.host_tx = false;
        conf.conf_10g.polarity.line_tx = (port_no < 2)? false : true;
        conf.conf_10g.is_host_wan = false;
        conf.conf_10g.lref_for_host = false;
        conf.conf_10g.h_clk_src_is_high_amp = true;
        conf.conf_10g.l_clk_src_is_high_amp = true;
        conf.conf_10g.h_media = MEPA_MEDIA_TYPE_DAC;
        conf.conf_10g.l_media = MEPA_MEDIA_TYPE_DAC;
        conf.conf_10g.channel_high_to_low = false;

        if (mepa_conf_set(inst->phy[port_no], &conf) == 0) {
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
        for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
            if (mepa_poll(inst->phy[port_no], &status) == 0) {
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
}

// *****************************************************************************
// End of file