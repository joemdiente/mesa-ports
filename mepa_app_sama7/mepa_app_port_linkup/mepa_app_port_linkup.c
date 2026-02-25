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

// Create a "wrapper" for 10G GPIOs.
#include <vtss_phy_10g_api.h>
#include <vtss_phy_api.h>
typedef vtss_gpio_10g_gpio_mode_t mepa_gpio_malibu_10g_gpio_mode_t;
typedef vtss_gpio_10g_no_t mepa_gpio_malibu_10g_no_t;

mepa_rc mepa_malibu_10g_gpio_mode_set(appl_inst_t* inst, mepa_port_no_t port_no, mepa_gpio_malibu_10g_no_t gpio_no, mepa_gpio_malibu_10g_gpio_mode_t* mode) {

    return (mepa_rc)vtss_phy_10g_gpio_mode_set((vtss_inst_t)inst, (vtss_port_no_t)port_no, (vtss_gpio_10g_no_t)gpio_no, (vtss_gpio_10g_gpio_mode_t*) &mode);
}

/*
 * Note: This function is only compatible with Malibu PHYs.
 */
int configure_malibu_gpios(appl_inst_t *inst, uint32_t dev_id)
{
    // Verify that the connected PHY is indeed a Malibu before proceeding with GPIO configuration
    if (dev_id != 0x00008258) {
        printf("Device ID is not Malibu. Aborting GPIO configuration.\n");
        exit(EXIT_FAILURE);
    }

    mepa_port_no_t port_no = 0;
    uint16_t gpio_no = 0;
    mepa_bool_t value = 0;
    mepa_gpio_malibu_10g_gpio_mode_t gpio_mode;
    /* ********************************************************** */
    // Below note was taken from vtss_appl_10g_phy_malibu.c
    // GPIO Output functionality
    // GPIO_0 -> GPIO_7  This repeats for each of the 4 ports.
    // In this Example:
    // 0 = GPIO output 0 from channel 0   - CH0_RS0
    // 1 = GPIO output 1 from channel 0   - N/A
    // 2 = GPIO output 2 from channel 0   - CH0_SCL - I2C Master for SFP+
    // 3 = GPIO output 3 from channel 0   - CH0_SDA - I2C Master for SFP+
    // 4 = GPIO output 4 from channel 0   - CH0_TX_DIS
    // 5 = GPIO output 5 from channel 0   - N/A
    // 6 = GPIO output 6 from channel 0   - N/A
    // 7 = GPIO output 7 from channel 0   - CH0_LINK_UP
    //
    // 8 = GPIO output 0 from channel 1   - CH1_RS0
    // 9 = GPIO output 1 from channel 1   - N/A
    // 10 = GPIO output 2 from channel 1  - CH1_SCL - I2C Master for SFP+
    // 11 = GPIO output 3 from channel 1  - CH1_SDA - I2C Master for SFP+
    // 12 = GPIO output 4 from channel 1  - CH1_TX_DIS
    // 13 = GPIO output 5 from channel 1  - N/A
    // 14 = GPIO output 6 from channel 1  - N/A
    // 15 = GPIO output 7 from channel 1  - CH1_LINK_UP
    // .....
    /* ********************************************************** */
    
    // GPIO used: #0 for Ch0 (CH0_RS0), #8 for Ch1 and so on
    printf(" Drive RS0 pins High\r\n");
    for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
        gpio_no = 0 + (port_no * 8);
        if (mepa_gpio_out_set(inst->phy[port_no], gpio_no, 1) == 0) {
            printf("PHY GPIO %d configured successfully for port %d\r\n", gpio_no, port_no);
        } else {
            printf("Failed to configure PHY GPIO %d for port %d\r\n", gpio_no, port_no);
        }
    }

    // GPIO used: #4 for Ch0 (CH0_TX_DIS), #12 for Ch1 and so on
    printf(" Drive TX_DIS pins Low\r\n");
    for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
        gpio_no = 4 + (port_no * 8);
        if (mepa_gpio_out_set(inst->phy[port_no], gpio_no, 0) == 0) {
            printf("PHY GPIO %d configured successfully for port %d\r\n", gpio_no, port_no);
        } else {
            printf("Failed to configure PHY GPIO %d for port %d\r\n", gpio_no, port_no);
        }
    }

    // GPIO used: #6 (CH0_RXLOS), #14 (CH1_RXLOS) and so on
    printf(" Check RX_LOS pins \r\n");
    for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
        gpio_no = 6 + (port_no * 8);
        if (mepa_gpio_in_get(inst->phy[port_no], gpio_no, &value) == 0) {
            printf("PHY GPIO %d configured successfully for port %d: value = %d\r\n", gpio_no, port_no, value);
        } else {
            printf("Failed to configure PHY GPIO %d for port %d\r\n", gpio_no, port_no);
        }
    }

    // GPIO used: #1 (Ch0_MOD_ABS), #9 (Ch1_MOD_ABS) and so on
    printf(" Check MOD_ABS pins \r\n");
    for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
        gpio_no = 1 + (port_no * 8);
        if (mepa_gpio_in_get(inst->phy[port_no], gpio_no, &value) == 0) {
            printf("PHY GPIO %d configured successfully for port %d: value = %d\r\n", gpio_no, port_no, value);
        } else {
            printf("Failed to configure PHY GPIO %d for port %d\r\n", gpio_no, port_no);
        }
    }

    // GPIO used: #5 (Ch0_TX_FAULT), #13 (Ch1_TX_FAULT) and so on
    printf(" Check TX_FAULT pins \r\n");
    for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
        gpio_no = 5 + (port_no * 8);
        if (mepa_gpio_in_get(inst->phy[port_no], gpio_no, &value) == 0) {
            printf("PHY GPIO %d configured successfully for port %d: value = %d\r\n", gpio_no, port_no, value);
        } else {
            printf("Failed to configure PHY GPIO %d for port %d\r\n", gpio_no, port_no);
        }
    }

    // I2C Configure SCL and SDA pins
    // GPIO used: #2/#3 (Ch0_SCL and CH0_SDA resp), #10/#11 (Ch1_SCL and CH1_SDA resp) and so on

    printf(" Drive I2C SDA pins \r\n");
    gpio_mode.mode = VTSS_10G_PHY_GPIO_OUT;
    gpio_mode.p_gpio = 0;
    gpio_mode.in_sig = VTSS_10G_GPIO_INTR_SGNL_I2C_MSTR_DATA_OUT;

    for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
        gpio_no = 2 + (port_no * 8);
        if (mepa_gpio_out_set(inst->phy[port_no], gpio_no, 1) == 0) {
            printf("I2C Data GPIO %d configured successfully for port %d\r\n", gpio_no, port_no);
        } else {
            printf("Failed to configure PHY GPIO %d for port %d\r\n", gpio_no, port_no);
        }
    }

    printf(" Configure I2C SCL pins \r\n");
    gpio_mode.mode = VTSS_10G_PHY_GPIO_OUT;
    gpio_mode.p_gpio = 0;
    gpio_mode.in_sig = VTSS_10G_GPIO_INTR_SGNL_I2C_MSTR_CLK_OUT;

    for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
        gpio_no = 3 + (port_no * 8);
         if (mepa_gpio_out_set(inst->phy[port_no], gpio_no, 1) == 0) {
            printf("I2C Clock GPIO %d configured successfully for port %d\r\n", gpio_no, port_no);
        } else {
            printf("Failed to configure PHY GPIO %d for port %d\r\n", gpio_no, port_no);
        }
    }

    return 0;
}

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

    // 3. Control 1G CuSFP GPIOs
    printf("\r\n MEPA Configure GPIOs\r\n");
    uint32_t value = 0;
    configure_malibu_gpios(inst, dev_id); // See code above. Pass Device ID.
    
    // mepa_i2c_clk_select_t i2c_clk = MEPA_I2C_CLK_SEL_2;
    // mepa_i2c_clock_select(inst->phy, &i2c_clk); // Set I2C clock to 800 KHz for faster transactions with SFP+
    
    while (1) {
        printf(" Insert 1G CuSFP on P2 (Channel 3)\r\n");
        for (port_no = 0; port_no < MALIBU_EVB_PORT_COUNT; port_no++) {
            mepa_i2c_read(inst->phy[port_no], NULL, 0x2, NULL, NULL, 1, &value);
            printf("Port:%d SFP+ Address 0xA0: 0x%08x\n", port_no, value);
        }
        if ((value) != 0) {
            printf("1G CuSFP detected on P2 (Channel 3)\r\n");
            break;
        } else {
            printf("No SFP+ detected on P2 (Channel 3). Please insert the SFP+ and try again.\r\n");
        }
        // Wait for 5 seconds before proceeding to the next step
        sleep(5);
    }

    // 4. Simple - Configure PHY Operating Mode - Set to 1G for CuSFP.
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

    // 5. Check Link Status

    
    return ret;
}

// *****************************************************************************
// End of file