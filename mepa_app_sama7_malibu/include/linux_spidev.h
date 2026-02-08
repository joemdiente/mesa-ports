/*
 * linux_spidev header file
 *
 * Author: JoemelJohn.Diente <JoemelJohn.Diente@microchip.com>
 * 
 */
#ifndef __LINUX_SPIDEV_
#define __LINUX_SPIDEV_

// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************
#include "my_debug.h"
#include <stdint.h>
#include <linux/spi/spidev.h>
typedef struct {
    uint8_t mode;
    uint8_t bits;
    uint32_t speed;
    char* spidev;
} spi_conf_t;

// *****************************************************************************
// *****************************************************************************
// Section: Function prototypes
// *****************************************************************************
// *****************************************************************************

// Initialize SPIDEV
int spi_initialize(spi_conf_t);
int spi_malibu_test_code(void);
int spi_32bit_malibu_read_spidev(  void* inst,
		                              uint8_t port_no,
                                  uint8_t dev,
                                  uint16_t reg_num,
                                  uint32_t *value);
int spi_32bit_malibu_write_spidev(  void* inst,
		                              uint8_t port_no,
                                  uint8_t dev,
                                  uint16_t reg_num,
                                  uint32_t *value);
#endif /*__LINUX_SPIDEV_*/