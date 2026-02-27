// Header file for sama7_mdio.c

#ifndef _SAMA7_MDIO_H_
#define _SAMA7_MDIO_H_

// Headers
#include "microchip/ethernet/phy/api.h"
#include <stdbool.h>

#define BOOL bool

#define T_D(format, ...) MEPA_trace(MEPA_TRACE_GRP_GEN, MEPA_TRACE_LVL_DEBUG, __FUNCTION__, __LINE__, __FILE__, format, ##__VA_ARGS__);
#define T_I(format, ...) MEPA_trace(MEPA_TRACE_GRP_GEN, MEPA_TRACE_LVL_INFO, __FUNCTION__, __LINE__, __FILE__, format, ##__VA_ARGS__);
#define T_W(format, ...) MEPA_trace(MEPA_TRACE_GRP_GEN, MEPA_TRACE_LVL_WARNING, __FUNCTION__, __LINE__, __FILE__, format, ##__VA_ARGS__);
#define T_E(format, ...) MEPA_trace(MEPA_TRACE_GRP_GEN, MEPA_TRACE_LVL_ERROR, __FUNCTION__, __LINE__, __FILE__, format, ##__VA_ARGS__);
#define T_N(format, ...) MEPA_trace(MEPA_TRACE_GRP_GEN, MEPA_TRACE_LVL_NOISE, __FUNCTION__, __LINE__, __FILE__, format, ##__VA_ARGS__);

// Function prototypes
int mdio_initialize(char* mdio_bus, uint8_t* phy_id);
int mdio_test_code(void);
uint8_t mdio_read (struct mepa_callout_ctx *ctx, uint8_t addr, uint16_t *value);
uint8_t mdio_write (struct mepa_callout_ctx *ctx, uint8_t addr, uint16_t value);

#endif /*_SAMA7_MDIO_H_*/