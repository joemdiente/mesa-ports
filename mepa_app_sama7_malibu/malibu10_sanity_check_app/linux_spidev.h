// Header file for linux_spidev.c
// Based on rpi_spi.c

#ifndef _LINUX_SPIDEV_H_
#define _LINUX_SPIDEV_H_

// Headers
#include "vtss_api.h"   // For BOOL and friends
#include "./appl/vtss_appl.h"  // For board types
#include <stdbool.h>

#define BOOL bool

// Function prototypes
int spi_set_cs_high(void);
int spi_set_cs_low(void);
void spi_write(uint8_t *data, unsigned int len);
void spi_read(uint8_t *data, unsigned int len);
int spi_initialize(const char*);

vtss_rc rpi_spi_32bit_write(
            vtss_inst_t    inst,
            vtss_port_no_t port_no,
            uint8_t             mmd,
            uint16_t            addr,
            uint32_t            *value);

vtss_rc rpi_spi_32bit_read(
                vtss_inst_t    inst,
                vtss_port_no_t port_no,
                uint8_t             mmd,
                uint16_t            addr,
                uint32_t            *value);

vtss_rc rpi_spi_32bit_read_rbt_test(
                vtss_port_no_t port_no,
                uint8_t             mmd,
                uint16_t            addr,
                uint32_t            *value);

BOOL vtss_appl_malibu_spi_io_test(const vtss_inst_t inst, const vtss_port_no_t port_no);


#endif /*_LINUX_SPIDEV_H_*/