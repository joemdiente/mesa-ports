/*
 * main entry app header file
 * Should be included by all MEPA Sample Applications.
 * Author: JoemelJohn.Diente <JoemelJohn.Diente@microchip.com>
 * 
 */
#ifndef __MAIN_
#define __MAIN_

// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************
#include "my_debug.h"
#include <stdint.h>
#include "microchip/ethernet/phy/api.h"
#include "microchip/ethernet/board/api.h"
// *****************************************************************************
// *****************************************************************************
// Section: Macros and Constant defines
// *****************************************************************************
// *****************************************************************************
#define MALIBU_EVB_PORT_COUNT 4
#define MALIBU_BASE_PORT 0
// *****************************************************************************
// *****************************************************************************
// Section: MEPA Structs
// *****************************************************************************
// *****************************************************************************

typedef struct {
    mepa_callout_t callout[MALIBU_EVB_PORT_COUNT];
    mepa_callout_ctx_t callout_ctx[MALIBU_EVB_PORT_COUNT];
    mepa_board_conf_t board_conf;
    mepa_device_t *phy[MALIBU_EVB_PORT_COUNT];
    mepa_conf_t malibu_conf;
} appl_inst_t;
// *****************************************************************************
// *****************************************************************************
// Section: Function prototypes
// *****************************************************************************
// *****************************************************************************
// This is where sample applications should be attached.
extern __attribute__((weak)) int mepa_app_sample_appl(appl_inst_t*); // Use "weak" so it is NULL in memory.

#endif /*__MAIN_*/