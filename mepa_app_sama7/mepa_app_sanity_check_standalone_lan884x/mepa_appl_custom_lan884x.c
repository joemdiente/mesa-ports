/* 
 * Custom program for LAN8841 EDS2 Daughterboard using MEPA.See docs for setup.
 * Code is loosely-based on https://github.com/MicrochipTech/mepa-app-malibu10-rpi
 * 
 * Copyright (C) 2026 Microchip Technology Inc.
 *
 * Author: Joemel John A. Diente <JoemelJohn.Diente@microchip.com>
 * For support, please reach out to microchip.com/support.
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

// *****************************************************************************
// *****************************************************************************
// Section: Header Includes
// *****************************************************************************
// *****************************************************************************

// Standard C includes
#include <stdio.h>
#include <unistd.h>     // For usleep()
#include <stdint.h>

// MEPA includes
#include "microchip/ethernet/phy/api.h"

// Other includes
#include "sama7_mdio.h"        // For the MDIO accessor functions

// *****************************************************************************
// *****************************************************************************
// Section: Macros and Constant defines
// *****************************************************************************
// *****************************************************************************

// Macros for debug levels:
// APPL_TRACE_LVL_NOISE
// APPL_TRACE_LVL_DEBUG
// APPL_TRACE_LVL_INFO
// APPL_TRACE_LVL_WARNING

// *****************************************************************************
// *****************************************************************************
// Section: Function prototypes
// *****************************************************************************
// *****************************************************************************

void appl_set_trace(void);

void appl_mepa_tracer(const mepa_trace_data_t *data, va_list args);

// *****************************************************************************
// *****************************************************************************
// Section: MEPA Structs
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

 int main(void)
 {
    // Defining local variables
    mepa_rc rc = 0;
    unsigned int i = 0;

    printf("SAMA7 + lan884x MEPA Sanity Check Code.\r\n");
    printf("LAN8841 EDS2 PHY Daughterboard is connected to EDS2 Connector\r\n");
    printf("EDS2 connector is RGMII and uses GMAC1\r\n");

    appl_set_trace();

    // Test the trace functions. You should see an output in the terminal.
    T_E("Error at %d\r\n", __LINE__);
    T_D("Debug at %d\r\n", __LINE__);
    T_W("Warning at %d\r\n", __LINE__);
    T_I("Info at %d\r\n", __LINE__);

    printf ("//Default Setup Assumptions: \n" );
    printf ("//Board has Power Applied prior to Demo start \n" );
    printf ("//Pwr Supply/Voltages Stable, Ref Clk Stable, Ref & PLL Stable \n\n" );

    /* ****************************************************** */
    /*                       BOARD SETUP                      */
    /* ****************************************************** */
    // Initialize the "board" (which refers to the RPi and VSC8258EV setup)
    // Test the board... or at least read the Device ID
    printf("%s: Connecting to the lan884x board via mdio... \r\n", __func__);
    printf("%s: Initializing linux mdio-netlink... \r\n", __func__);
    appl_mdio_init();

    // Test MDIO; Get PHY Registers, fail if 0xFFFF or 0x0000
    uint32_t val = 0;
    // rpi_spi_32bit_read_rbt_test(0x0, 0x1e, 0x0, &val);
    // printf("In Line %d: Dev ID = 0x%x\r\n\r\n", __LINE__, val);

    return 0;
}

 // *****************************************************************************
// *****************************************************************************
// Section: Function Defines
// *****************************************************************************
// *****************************************************************************

void appl_set_trace(void)
{
    // Register the tracer function
    MEPA_TRACE_FUNCTION = appl_mepa_tracer;
}

void appl_mdio_init(void)
{
    mdio_initialize();
}

void appl_mepa_tracer(const mepa_trace_data_t *data, va_list args)
{
    // Taken from the example code snippet in mepa-doc.html#mepa/docs/mepa_instantiation
#if !defined(APPL_TRACE_LVL_NONE)
#if !defined(APPL_TRACE_LVL_ERROR)
#if !defined(APPL_TRACE_LVL_WARNING)
#if !defined(APPL_TRACE_LVL_INFO)
#if !defined(APPL_TRACE_LVL_DEBUG)
#if !defined(APPL_TRACE_LVL_NOISE)
    #define APPL_TRACE_LVL_NONE
#endif
#endif
#endif
#endif
#endif
#endif

#ifdef APPL_TRACE_LVL_NONE
    return;
#endif

#ifdef APPL_TRACE_LVL_ERROR
    if(data->level < MEPA_TRACE_LVL_ERROR)
    {
        return;
    }
#endif


#ifdef APPL_TRACE_LVL_WARNING
    if(data->level < MEPA_TRACE_LVL_WARNING)
    {
        return;
    }
#endif

#ifdef APPL_TRACE_LVL_INFO
    if(data->level < MEPA_TRACE_LVL_INFO)
    {
        return;
    }
#endif


#ifdef APPL_TRACE_LVL_DEBUG
    if(data->level < MEPA_TRACE_LVL_DEBUG)
    {
        return;
    }
#endif

#ifdef APPL_TRACE_LVL_NOISE
    if(data->level < MEPA_TRACE_LVL_NOISE)
    {
        return;
    }
#endif

    // Do filtering, and optionally add more details.
    printf("%s ", data->location);
    //vprintf(data->format, args);

    /* Note: for real applications, it may be useful to 
     * do filtering here such that only certain print levels
     * show up on the output. For example:
     *       if(data->level ==  MEPA_TRACE_LVL_DEBUG)
     * #ifdef (TRACE_LEVEL_DEBUG)
     *           vprintf(...);
     * #endif
     *
     *  Then, compile the application with the correct flags
     *  gcc .. -DTRACE_LEVEL_DEBUG -DTRACE_LEVEL_INFO ...
     *
     * It's the user's responsibility to use the trace levels
     * properly in the application.
    */

    char *trace_str[] = {
        "null",
        "RACKET",
        "NOISE",
        "null",
        "DEBUG",
        "null",
        "INFO",
        "null",
        "WARNING",
        "ERROR",
        "NONE"
    };
    
    char tempstr[512];
    memset(tempstr, 0, sizeof(tempstr));

    printf("(%s): ", trace_str[data->level]);
    vsprintf(tempstr, data->format, args);
    printf("%s", tempstr);

    // Check for newline:
    if(tempstr[strlen(tempstr)-1] != '\n')
    {
        printf("\r\n");
    }

    return;
}

// *****************************************************************************
// End of file