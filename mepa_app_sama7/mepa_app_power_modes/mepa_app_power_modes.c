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
 * This is an example for configuring power modes in MEPA.
 * Setup:
 *  In Nominal mode, see current (in ampere) lowers.
 *  In ActiPHY mode, https://ww1.microchip.com/downloads/en/Appnotes/VPPD-01962.pdf
 *  In Dynamic mode, PerfectReach (works by determining the cable length and lowering the power for ports with short cables)
 *  In Enabled mode, both ActiPHY and PerfectReach are enabled together at link-up. This is the recommended mode for power saving.
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

int set_power_mode(appl_inst_t* inst, mepa_power_mode_t pmode) 
{
    mepa_port_no_t port_no = 0;
    for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
        if (mepa_power_set(inst->phy[port_no], pmode) == 0) {
            printf("Port %d power mode set success.\r\n", port_no);
        } else {
            printf("Port %d power mode set failed.\r\n", port_no);
        }
    }
    return 0;
}

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

    // 4. Select Power Mode.
    int choice = 0;
    while(1) {

        printf("\033[2J\033[H");
        printf(" MEPA Power Mode \r\n");
        printf("================================\r\n");
        printf("       SYSTEM MAIN MENU         \r\n");
        printf("================================\r\n");
        printf("1. Nominal = Default power settings to restore the original power level.\r\n");
        printf("2. ACTIPHY = Actiphy Power Mgmt mode that supports power sensitive applications.\r\n");
        printf("3. Dynamic = Enables PerfectReach capability at link-up to adjust power output based on cable length.\r\n");
        printf("4. Enabled = Enables Actiphy and PerfectReach together at link-up.\r\n");
        printf("5. Exit (or use Ctrl+C)\r\n");
        printf("--------------------------------\r\n");
        printf("Enter selection: ");
        if (scanf("%d", &choice) != 1) {
            // Handle non-numeric input to prevent infinite loops
            while (getchar() != '\n'); 
            continue;
        }

        // Menu Logic
        switch (choice) {
            case 1:
                printf("\nNominal Mode:\n");
                set_power_mode(inst, MESA_PHY_POWER_NOMINAL);
                break;
            case 2:
                printf("\nActiPHY Mode:\n");
                set_power_mode(inst, MESA_PHY_POWER_ACTIPHY);
                break;
            case 3:
                printf("\nDynamic Mode: PerfectReach\n");
                set_power_mode(inst, MESA_PHY_POWER_DYNAMIC);
                break;
            case 4:
                printf("\nEnabled Mode: ActiPHY and PerfectReach\n");
                set_power_mode(inst, MESA_PHY_POWER_ENABLED);
                break;
            case 5:
                printf("\nExiting App...\n");
                exit(EXIT_SUCCESS);
            default:
                printf("\n[ERROR]: Invalid selection. Try again.\n");
        }

        // Pause so the user can see the result before the screen clears again
        printf("\nPress Enter to return to menu...");
        getchar(); // Catch the newline from scanf
        getchar(); // Wait for actual user press
    }

    
    return ret;
}

// *****************************************************************************
// End of file