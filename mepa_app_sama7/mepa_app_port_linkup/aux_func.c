/* 
 *
 * Copyright (C) 2026 Microchip Technology Inc.
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
 * Note: all the codes here are taken from https://github.com/MicrochipTech/mepa-app-malibu10-rpi/
 * Used a different source file so the mepa app looks clean.
 */

#include <stdint.h>
#include "microchip/ethernet/phy/api.h"
#include "microchip/ethernet/board/api.h"
#include <vtss_phy_10g_api.h>
#include <vtss_phy_api.h>
#include "main.h"
#include <stdio.h>

#define MY_DEBUG
#define MY_DEBUG_SHOW_ADVANCED
#include "my_debug.h"

/*
 * Below codes are taken from https://github.com/MicrochipTech/mepa-app-malibu10-rpi
 */
// Reference: mesa/meba/src/sparx5/meba.c
typedef struct {
    vtss_gpio_10g_no_t gpio_tx_dis;      /* Tx Disable GPIO number */
    vtss_gpio_10g_no_t gpio_aggr_int;    /* Aggregated Interrupt-0 GPIO number */
    vtss_gpio_10g_no_t gpio_i2c_clk;     /* GPIO Pin selection as I2C_CLK for I2C
                                            communication with SFP  */
    vtss_gpio_10g_no_t gpio_i2c_dat;     /* GPIO Pin selection as I2C_DATA for I2C
                                            communication with SFP */
    vtss_gpio_10g_no_t gpio_virtual;     /* Per port Virtual GPIO number,for internal GPIO usage */
    vtss_gpio_10g_no_t gpio_sfp_mod_det; /* GPIO Pin selection as SFP module detect */
    vtss_gpio_10g_no_t gpio_tx_fault;    /* GPIO Pin for TX_FAULT */
    vtss_gpio_10g_no_t gpio_rx_los;      /* GPIO Pin for RX_LOS */
    uint32_t           aggr_intrpt;      /* Channel interrupt bitmask */
} appl_malibu_gpio_port_map_t;

// Reference: mesa/meba/src/sparx5/meba.c
static const appl_malibu_gpio_port_map_t malibu_gpio_map[] = {
    // PHY CH0
    {
        .gpio_tx_dis        = 4,
        .gpio_aggr_int      = 34,
        .gpio_i2c_clk       = 2,
        .gpio_i2c_dat       = 3,
        .gpio_virtual       = 0,
        .gpio_sfp_mod_det   = 1,
        .gpio_tx_fault      = 5,
        .gpio_rx_los        = 6,
        .aggr_intrpt        = ((1 << VTSS_10G_GPIO_AGGR_INTRPT_CH0_INTR0_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_IP1588_0_INTR0_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_IP1588_1_INTR0_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_GPIO_INTR_EN)),
    },
    // PHY CH1
    {
        .gpio_tx_dis        = 12, 
        .gpio_aggr_int      = 34,
        .gpio_i2c_clk       = 10,
        .gpio_i2c_dat       = 11,
        .gpio_virtual       = 0,
        .gpio_sfp_mod_det   = 9,
        .gpio_tx_fault      = 13,
        .gpio_rx_los        = 14,
        .aggr_intrpt        = ((1 << VTSS_10G_GPIO_AGGR_INTRPT_CH1_INTR0_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_IP1588_0_INTR1_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_IP1588_1_INTR1_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_GPIO_INTR_EN)),
     },    
     // PHY CH2
    {
        .gpio_tx_dis        = 20,
        .gpio_aggr_int      = 34,
        .gpio_i2c_clk       = 18,
        .gpio_i2c_dat       = 19,
        .gpio_virtual       = 0,
        .gpio_sfp_mod_det   = 17,
        .gpio_tx_fault      = 21,
        .gpio_rx_los        = 22,
        .aggr_intrpt        = ((1 << VTSS_10G_GPIO_AGGR_INTRPT_CH2_INTR0_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_IP1588_0_INTR2_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_IP1588_1_INTR2_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_GPIO_INTR_EN)),
     },
     // PHY CH3
    {
        .gpio_tx_dis        = 28,
        .gpio_aggr_int      = 34,
        .gpio_i2c_clk       = 26,
        .gpio_i2c_dat       = 27,
        .gpio_virtual       = 0,
        .gpio_sfp_mod_det   = 25,
        .gpio_tx_fault      = 29,
        .gpio_rx_los        = 30,
        .aggr_intrpt        = ((1 << VTSS_10G_GPIO_AGGR_INTRPT_CH3_INTR0_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_IP1588_0_INTR3_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_IP1588_1_INTR3_EN) |
                               (1 << VTSS_10G_GPIO_AGGR_INTRPT_GPIO_INTR_EN)),
     },
};

mepa_rc aux_malibu_gpio_conf(appl_inst_t* inst, mepa_port_no_t port_no)
{
    PRINT_FUN();
    vtss_rc rc = VTSS_RC_OK;
    vtss_gpio_10g_gpio_mode_t gpio_conf = {};
    const appl_malibu_gpio_port_map_t *gmap = &malibu_gpio_map[port_no];

    // Initialize Malibu PHY GPIOs for VSC8258EV
    // See the VSC8258EV schematics in the Product page: https://www.microchip.com/en-us/product/vsc8258
    //      Scroll down to find "VSC8256/VSC8257/VSC8258 Evaluation Board Design Files"
    // NOTE: main reference for this function is meba/src/sparx5/meba.c > malibu_gpio_conf()
    // -------------
    // Also note that as of SW-MEPA 2025.12, no implementation of mepa_gpio_mode_set() exists for Malibu PHYs
    // yet, so we call vtss APIs directly.

    /* ********************************************************** */
    // GPIO Input functionality
    /* ********************************************************** */
    // SFP_MOD_DET
    rc = vtss_phy_10g_gpio_mode_get(NULL, port_no, gmap->gpio_sfp_mod_det, &gpio_conf);
    if(rc == VTSS_RC_OK)
    {
        gpio_conf.mode = VTSS_10G_PHY_GPIO_IN;
        gpio_conf.input = VTSS_10G_GPIO_INPUT_NONE;
        rc = vtss_phy_10g_gpio_mode_set(NULL, port_no, gmap->gpio_sfp_mod_det, &gpio_conf);
    }

    if(rc != VTSS_RC_OK)
    {
        PRINT_RES("vtss_phy_10g_gpio_mode_set, port %d, gpio %d, mode: INPUT (SFP_MOD_DET)\n", port_no, gmap->gpio_sfp_mod_det);
        return rc;
    }
    else
    {
        PRINT_RES("Malibu GPIO Input: SFP_MOD_DET configuration for port %d, gpio %d \n", port_no, gmap->gpio_sfp_mod_det);
    }

    // RX_LOS
    rc = vtss_phy_10g_gpio_mode_get(NULL, port_no, gmap->gpio_rx_los, &gpio_conf);
    if(rc == VTSS_RC_OK)
    {
        gpio_conf.mode = VTSS_10G_PHY_GPIO_IN;
        gpio_conf.input = VTSS_10G_GPIO_INPUT_LINE_LOPC;
        rc = vtss_phy_10g_gpio_mode_set(NULL, port_no, gmap->gpio_rx_los, &gpio_conf);
    }

    if(rc != VTSS_RC_OK)
    {
        PRINT_RES("vtss_phy_10g_gpio_mode_set, port %d, gpio %d, mode: INPUT (RX_LOS)\n", port_no, gmap->gpio_rx_los);
        return rc;
    }
    else
    {
        PRINT_RES("Malibu GPIO Input: RX_LOS configuration for port %d, gpio %d \n", port_no, gmap->gpio_rx_los);
    }

    // TX_FAULT
    rc = vtss_phy_10g_gpio_mode_get(NULL, port_no, gmap->gpio_tx_fault, &gpio_conf);
    if(rc == VTSS_RC_OK)
    {
        gpio_conf.mode = VTSS_10G_PHY_GPIO_IN;
        gpio_conf.input = VTSS_10G_GPIO_INPUT_NONE;
        rc = vtss_phy_10g_gpio_mode_set(NULL, port_no, gmap->gpio_tx_fault, &gpio_conf);
    }

    if(rc != VTSS_RC_OK)
    {
        PRINT_RES("vtss_phy_10g_gpio_mode_set, port %d, gpio %d, mode: INPUT (TX_FAULT)\n", port_no, gmap->gpio_tx_fault);
        return rc;
    }
    else
    {
        PRINT_RES("Malibu GPIO Input: TX_FAULT configuration for port %d, gpio %d \n", port_no, gmap->gpio_tx_fault);
    }

    /* ********************************************************** */
    // GPIO Output functionality
    /* ********************************************************** */
    // TXDIS
    rc = vtss_phy_10g_gpio_mode_get(NULL, port_no, gmap->gpio_tx_dis, &gpio_conf);
    if(rc == VTSS_RC_OK)
    {
        gpio_conf.mode = VTSS_10G_PHY_GPIO_DRIVE_LOW;
        gpio_conf.in_sig = VTSS_10G_GPIO_INTR_SGNL_NONE;
        rc = vtss_phy_10g_gpio_mode_set(NULL, port_no, gmap->gpio_tx_dis, &gpio_conf);
    }

    if(rc != VTSS_RC_OK)
    {
        PRINT_RES("vtss_phy_10g_gpio_mode_set, port %d, gpio %d, mode: DRIVE_LOW (TX_DISABLE)\n", port_no, gmap->gpio_tx_dis);
        return rc;
    }
    else
    {
        PRINT_RES("Malibu GPIO Output: Driving LOW configuration for port %d, gpio %d (TX_DISABLE)\n", port_no, gmap->gpio_tx_dis);
    }

    /* ********************************************************** */
    // GPIO I2C Pins
    /* ********************************************************** */
    // SDA
    rc = vtss_phy_10g_gpio_mode_get(NULL, port_no, gmap->gpio_i2c_dat, &gpio_conf);
    if(rc == VTSS_RC_OK)
    {
        PRINT_RES("Malibu port %d I2C DAT pin %d\n", port_no, gmap->gpio_i2c_dat);
        gpio_conf.mode = VTSS_10G_PHY_GPIO_OUT;
        gpio_conf.in_sig = VTSS_10G_GPIO_INTR_SGNL_I2C_MSTR_DATA_OUT;
        gpio_conf.p_gpio = 0;   // Route the internal signal "SDA Output" to GPIO0_OUT
        rc = vtss_phy_10g_gpio_mode_set(NULL, port_no, gmap->gpio_i2c_dat, &gpio_conf);
    }

    if(rc != VTSS_RC_OK)
    {
        PRINT_RES("vtss_phy_10g_gpio_mode_set, port %d, gpio %d, mode: OUTPUT (I2C SDA)\n", port_no, gmap->gpio_i2c_dat);
        return rc;
    }
    else
    {
        PRINT_RES("Malibu GPIO Output: I2C Master DATA configuration for port %d, gpio %d\n", port_no, gmap->gpio_i2c_dat);
    }

    // SCL
    rc = vtss_phy_10g_gpio_mode_get(NULL, port_no, gmap->gpio_i2c_clk, &gpio_conf);
    if(rc == VTSS_RC_OK)
    {
        PRINT_RES("Malibu port %d I2C CLK pin %d\n", port_no, gmap->gpio_i2c_clk);
        gpio_conf.mode = VTSS_10G_PHY_GPIO_OUT;
        gpio_conf.in_sig = VTSS_10G_GPIO_INTR_SGNL_I2C_MSTR_CLK_OUT;
        gpio_conf.p_gpio = 1;   // Route the internal signal "SCL Output" to GPIO1_OUT
        rc = vtss_phy_10g_gpio_mode_set(NULL, port_no, gmap->gpio_i2c_clk, &gpio_conf);
    }

    if(rc != VTSS_RC_OK)
    {
        PRINT_RES("vtss_phy_10g_gpio_mode_set, port %d, gpio %d, mode: OUTPUT (I2C SCL)\n", port_no, gmap->gpio_i2c_clk);
        return rc;
    }
    else
    {
        PRINT_RES("Malibu GPIO Output: I2C Master CLK configuration for port %d, gpio %d\n", port_no, gmap->gpio_i2c_clk);
    }

    /* ********************************************************** */
    // Test I2C Access to SFP+ ports!
    /* ********************************************************** */

    // Now call the read or write function with the relevant address and data
    // MEPA has I2C functions for Malibu PHYs, so we will use them instead of vtss_phy_10g_i2c_read/write().
    // Also, keep in mind the following comment from vtss.c:
    // ------------------------
    /* In 10G Malibu PHY's the arguments i2c_dev_addr and word_access of API i2c_read/write have no functionality,
    * so the value for these arguments can be given as zero
    */
    // ------------------------
    // The 'cnt' argument is also not used, so we can just pass '0'
    u16 address = 0x0;
    u8 data;

    // Skip I2C access for ports 0 and 1 (which don't connect to SFP+ ports on VSC8258EV)
    if(port_no < 2)
    {
        return rc;
    }

    for (address = 0; address < 16; address++)
    {
        if(mepa_i2c_read(inst->phy[port_no], 0, address, 0, 0, 0, &data) != MEPA_RC_OK)
        {
	        PRINT_RES("mepa_i2c_read, port %d, gpio %d, address = 0x%X\n", port_no, gmap->gpio_i2c_clk, address);
	        PRINT_RES("Malibu Error reading I2C register on SFP+ module for port %d, gpio %d \n", port_no, gmap->gpio_i2c_clk);
        } 
        else
        {
	        PRINT_RES("Malibu reading I2C register @ addr = %d: value = 0x%X \n", address, data);
        }
    }

    address = 0x3E;
    if (mepa_i2c_read(inst->phy[port_no], 0, address, 0, 0, 0, &data) != MEPA_RC_OK)
    {
        PRINT_RES("mepa_i2c_read, port %d, gpio %d, address = 0x%X\n", port_no, gmap->gpio_i2c_clk, address);
        PRINT_RES("Malibu Error reading I2C register on SFP+ module for port %d, gpio %d \n", port_no, gmap->gpio_i2c_clk);
    } 
    else
    {
        PRINT_RES("Malibu reading I2C register @ addr = 0x%X: value = 0x%X \n", address, data);
    }

    data = 0xAB;
    if (mepa_i2c_write(inst->phy[port_no], 0, address, 0, 0, 0, &data) != MEPA_RC_OK)
    {
        PRINT_RES("mepa_i2c_write, port %d, gpio %d, address = 0x%X, data = 0x%X\n", port_no, gmap->gpio_i2c_clk, address, data);
        PRINT_RES("Malibu Error writing I2C register on SFP+ module for port %d, gpio %d \n", port_no, gmap->gpio_i2c_clk);
    }

    if (mepa_i2c_read(inst->phy[port_no], 0, address, 0, 0, 0, &data) != MEPA_RC_OK)
    {
        PRINT_RES("mepa_i2c_read, port %d, gpio %d, address = 0x%X\n", port_no, gmap->gpio_i2c_clk, address);
        PRINT_RES("Malibu Error reading I2C register on SFP+ module for port %d, gpio %d \n", port_no, gmap->gpio_i2c_clk);
    }
    else
    {
        PRINT_RES("Malibu reading I2C register @ addr = 0x%X: value = 0x%X \n", address, data);
    }

    /* ********************************************************** */

    return rc;
}

mepa_rc aux_malibu_lb_conf(appl_inst_t* inst, mepa_port_no_t port_no) 
{
    // vtss loopback from old phy_demo_appl vtss_appl_10g_phy_malibu.c
    // search for "strcmp(command, "lpback")"

    vtss_phy_10g_loopback_t    lpback;
    vtss_lb_type_t             lpback_type = VTSS_LB_NONE;
    char                       lpback_descr[6];
    char value_str[255] = {0};
    memset (&lpback_descr[0], 0, sizeof(lpback_descr));

    vtss_phy_10g_loopback_set(NULL, port_no, &lpback);

    switch (lpback.lb_type) {
    case VTSS_LB_NONE: lpback_descr[0] = 'N'; lpback_descr[1] = 'O'; lpback_descr[2] = 'N'; lpback_descr[3] = 'E'; break;
    case VTSS_LB_H2: lpback_descr[0] = 'H'; lpback_descr[1] = '2'; break;
    case VTSS_LB_H3: lpback_descr[0] = 'H'; lpback_descr[1] = '3'; break;
    case VTSS_LB_H4: lpback_descr[0] = 'H'; lpback_descr[1] = '4'; break;
    case VTSS_LB_H5: lpback_descr[0] = 'H'; lpback_descr[1] = '5'; break;
    case VTSS_LB_H6: lpback_descr[0] = 'H'; lpback_descr[1] = '6'; break;
    case VTSS_LB_L0: lpback_descr[0] = 'L'; lpback_descr[1] = '0'; break;
    case VTSS_LB_L1: lpback_descr[0] = 'L'; lpback_descr[1] = '1'; break;
    case VTSS_LB_L2: lpback_descr[0] = 'L'; lpback_descr[1] = '2'; break;
    case VTSS_LB_L3: lpback_descr[0] = 'L'; lpback_descr[1] = '3'; break;
    case VTSS_LB_L2C: lpback_descr[0] = 'L'; lpback_descr[1] = '2'; lpback_descr[2] = 'C'; break;
    default:
        printf ("Current Loopback Description INVALID,  Port_no: %d \n",  port_no);
        memset (&lpback_descr[0], 0, sizeof(lpback_descr));
    }
    printf ("Current Loopback is: %s  Type: %s \n\n",  (lpback.enable ? "Enabled" : "Disabled"), lpback_descr);

    printf ("Loopback Options for Port: %d \n", port_no);
    printf ("  H2: Host Loopback 2, 40-bit XAUI-PHY interface Mirror XAUI data\n");
    printf ("  H3: Host Loopback 3, 64-bit PCS after the gearbox FF00 repeating IEEE PCS system loopback \n");
    printf ("  H4: Host Loopback 4, 64-bit WIS FF00 repeating IEEE WIS system loopback \n");
    printf ("  H5: Host Loopback 5, 1-bit SFP+ after SerDes Mirror XAUI data IEEE PMA system loopback \n");
    printf ("  H6: Host Loopback 6, 32-bit XAUI-PHY interface Mirror XAUI data  \n\n");
    printf ("  L0: Line Loopback 0, 4-bit XAUI before SerDes Mirror SFP+ data \n");
    printf ("  L1: Line Loopback 1, 4-bit XAUI after SerDes Mirror SFP+ data IEEE PHY-XS network loopback \n");
    printf ("  L2: Line Loopback 2, 64-bit XGMII after FIFO Mirror SFP+ data  \n");
    printf ("  L3: Line Loopback 3, 64-bit PMA interface Mirror SFP+ data \n");
    printf ("  L2C: Line Line loopback 4 after cross connect    \n\n");
    printf ("Enter Loopback Type: H2/H3/H4/H5/H6/L0/L1/L2/L3/L2C \n");
    memset (&value_str[0], 0, sizeof(value_str));
    scanf("%s", &value_str[0]);

    if (value_str [0] == 'h' || value_str [0] == 'H' ) {
        switch (value_str [1]) {
        case '2':
            lpback_type = VTSS_LB_H2;
            break;
        case '3':
            lpback_type = VTSS_LB_H3;
            break;
        case '4':
            lpback_type = VTSS_LB_H4;
            break;
        case '5':
            lpback_type = VTSS_LB_H5;
            break;
        case '6':
            lpback_type = VTSS_LB_H6;
            break;
        default:
            break;
        }
    } else if (value_str [0] == 'l' || value_str [0] == 'L' ) {
        switch (value_str [1]) {
        case '0':
            lpback_type = VTSS_LB_L0;
            break;
        case '1':
            lpback_type = VTSS_LB_L1;
            break;
        case '2':
            if (value_str [2] == 'c' || value_str [2] == 'C') {
                lpback_type = VTSS_LB_L2C;
            } else {
                lpback_type = VTSS_LB_L2;
            }
            break;
        case '3':
            lpback_type = VTSS_LB_L3;
            break;
        default:
            break;
        }

    } else {
        lpback_type = VTSS_LB_NONE;
    }

    lpback.lb_type = lpback_type;
    switch (lpback.lb_type) {
    case VTSS_LB_NONE: lpback_descr[0] = 'N'; lpback_descr[1] = 'O'; lpback_descr[2] = 'N'; lpback_descr[3] = 'E'; break;
    case VTSS_LB_H2: lpback_descr[0] = 'H'; lpback_descr[1] = '2'; break;
    case VTSS_LB_H3: lpback_descr[0] = 'H'; lpback_descr[1] = '3'; break;
    case VTSS_LB_H4: lpback_descr[0] = 'H'; lpback_descr[1] = '4'; break;
    case VTSS_LB_H5: lpback_descr[0] = 'H'; lpback_descr[1] = '5'; break;
    case VTSS_LB_H6: lpback_descr[0] = 'H'; lpback_descr[1] = '6'; break;
    case VTSS_LB_L0: lpback_descr[0] = 'L'; lpback_descr[1] = '0'; break;
    case VTSS_LB_L1: lpback_descr[0] = 'L'; lpback_descr[1] = '1'; break;
    case VTSS_LB_L2: lpback_descr[0] = 'L'; lpback_descr[1] = '2'; break;
    case VTSS_LB_L3: lpback_descr[0] = 'L'; lpback_descr[1] = '3'; break;
    case VTSS_LB_L2C: lpback_descr[0] = 'L'; lpback_descr[1] = '2'; lpback_descr[2] = 'C'; break;
    default:
        printf ("Current Loopback Description INVALID,  Port_no: %d \n",  port_no);
        memset (&lpback_descr[0], 0, sizeof(lpback_descr));
    }
    printf ("Selected Loopback Type: %s \n",  lpback_descr);
    printf ("E=Enable or D=Disable Loopback? \n");
    memset (&value_str[0], 0, sizeof(value_str));
    scanf("%s", &value_str[0]);

    if (value_str [0] == 'e' || value_str [0] == 'E') {
        lpback.enable = 1;
    } else {
        lpback.enable = 0;
    }

    printf ("Port %d, Setting Loopback Type: %s  to  %s  \n",  port_no, lpback_descr, (lpback.enable ? "Enabled" : "Disabled"));
    vtss_phy_10g_loopback_set(NULL, port_no, &lpback);
            
    return MESA_RC_OK;
}