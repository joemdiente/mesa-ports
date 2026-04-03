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
 * with this program; If not, see <https://www.gnu.org/licenses/>.
 *
 */

/*
 * mesa_app_port_linkup
 * demonstrates how to link up a 1G SFP.
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

// Use code from aux-func.c to keep this file clean.
extern __attribute__((weak)) mepa_rc aux_malibu_gpio_conf(appl_inst_t* inst, mepa_port_no_t port_no); // Use "weak" so it is NULL in memory.
extern __attribute__((weak)) mepa_rc aux_malibu_lb_conf(appl_inst_t* inst, mepa_port_no_t port_no); // Use "weak" so it is NULL in memory.
/*
 * This is the main function for the MEPA application sample.
 */
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
    uint32_t dev_id = 0;
    inst->callout[port_no].spi_read(NULL, port_no, 0x1E, 0x00, &dev_id);
    printf("DEV_ID: 0x%08x\n", dev_id);

    // 3. Simple - Configure PHY Operating Mode - Set to 1G for CuSFP.
    printf("\r\n MEPA Port Configuration\r\n");
    mepa_conf_t conf;
    // Conf Get
    for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
        if (mepa_conf_get(inst->phy[port_no], &conf) < 0) {
            printf("Failed to get current port configuration for port %d\r\n", port_no);
        }
        
        // PHY Configuration
        conf.speed = MESA_SPEED_1G;
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
        conf.conf_10g.h_media = MEPA_MEDIA_TYPE_SR2_SC;
        conf.conf_10g.l_media = MEPA_MEDIA_TYPE_SR2_SC;
        conf.conf_10g.channel_high_to_low = false;

        // 1G Control
        conf.mac_if_aneg_ena = false; // Disable MAC Aneg.
        conf.adv_dis = true; // Disable PHY Aneg Advertisement to force speed.
        conf.admin.enable = true;

        if (mepa_conf_set(inst->phy[port_no], &conf) == 0) {
            printf("Port %d configuration success.\r\n", port_no);
        } else {
             printf("Port %d configuration failed.\r\n", port_no);
        }
    }

    // 4. Control 1G CuSFP GPIOs    
    /*
     * #3 Configure PHY Operating Mode should run first before 
     * configuring GPIO (or I2C) else, gpio or i2c will NOT work.
     * eg, i2c read will always return 0x00. 
     */ 
    printf("\r\n MEPA GPIO and I2C Configuration\r\n");
    for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
        printf("Port %d gpio configuration %s\r\n", port_no, aux_malibu_gpio_conf(inst, port_no)? "failed" : "success");
    }

    // 5. Wait for 1G CU SFP Insertion.
    uint8_t data = 0;
    port_no = 2;
    printf(" Waiting to insert SFP/SFP+ on P1.\r\n");
    while (1) {
        mepa_i2c_read(inst->phy[port_no], 0, 0x0, 0, 0, 1, &data); // Read address 0x0 for 0x3 value.
        if (data == 0x3) {
            printf("SFP/SFP+ detected on P1.\r\n");

            // Then detect if 1000Base-T SFP.
            mepa_i2c_read(inst->phy[port_no], 0, 0x6, 0, 0, 1, &data); //Read address 0x6 bit 3 for 1000Base-T
            printf("Ethernet Compliance Code: 0x%02x\r\n", data);
            if ((data & 0b1000) == 0b1000) {
                printf("1000Base-T SFP/SFP+ detected on P1.\r\n");
                break;
            }
        }

        sleep(1);
    }
    
    // 6. Monitor Link Status and Print it.
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
        // printf("\033[2J\033[H");
        printf(" MEPA Poll Link Information per Port \r\n");
        for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
            if (port_no != 2) {
                continue;
            }
            if (mepa_poll(inst->phy[port_no], &status) == 0) {
                printf("Port: %d, Speed: %s, fdx: %s, Cu: %s, Fi: %s, Link: %s\n", port_no, portspeed2txt[status.speed], \
                status.fdx? "Yes":"No", status.copper? "Yes":"No", status.fiber? "Yes":"No", status.link? "Up" : "Down");
            } else {
                printf("Port %d poll failed.\r\n", port_no);
            }
        }
        // fflush(stdout); 
        sleep(1);
        break;
    }

    // Debug: Enable loopback on port 2 (P1)
    printf("\r\n MEPA Loopback Configuration\r\n");
    port_no = 2;
    if (aux_malibu_lb_conf(inst->phy[port_no], port_no) == 0) {
        printf("Port %d loopback enabled.\r\n", port_no);
    } else {
        printf("Port %d loopback enable failed.\r\n", port_no);
    }
    return ret;
}

// *****************************************************************************
// End of file