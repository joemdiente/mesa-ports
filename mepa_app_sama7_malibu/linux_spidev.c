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

 //***************************************************************************
//* VSC8258EV to any linux system via spidev connector
//*
//* This file contains board specific functions needed for running the PHY  *
//* API on a VSC8258EV from any linux device that supports spidev.
//* This code tries to be a reuseable code.
//***************************************************************************
#define MY_DEBUG
#define MY_DEBUG_SHOW_ADVANCED
#include "my_debug.h"
// *****************************************************************************
// *****************************************************************************
// Section: Header Includes
// *****************************************************************************
// *****************************************************************************
// Standard Library
#include <stdio.h> // for FILE types
#include <stdlib.h> // for EXIT_SUCCESS
#include <string.h>
#include <stdint.h>

// Linux Includes
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
// My Includes
#include "linux_spidev.h"
// *****************************************************************************
// *****************************************************************************
// Section: Implementation
// *****************************************************************************
// *****************************************************************************
#define MALIBU_BIT_SEQ_BYTE_COUNT 7 // Malibu bit sequence is 7 bytes.
typedef uint8_t malibu_spi_slave_inst_bit_seq_t[MALIBU_BIT_SEQ_BYTE_COUNT];
#define MALIBU_RW_BIT_OFFSET 7 
#define MALIBU_PORTNO_BIT_OFFSET 5
#define MALIBU_DEVNO_BIT_SEQ_OFFSET 0
#define MALIBU_BYTE_COUNT_PER_TRANSACTION 14
malibu_spi_slave_inst_bit_seq_t dummy_bit_seq;
spi_conf_t spi_conf;
int fd = 0;
////////////////////////////////////////////////////////////////////////////////
// Initialize SPI
////////////////////////////////////////////////////////////////////////////////
int spi_initialize(spi_conf_t conf) {

  PRINT_FUN();
  int ret = 0;
  spi_conf = conf;
  fd = open(conf.spidev, O_RDWR);
  if (ret < 0 || fd < 0) {
    PRINT_RES("Error opening spidev", ret);
    PRINT_RES(conf.spidev,ret);
    return ret;
  }

  // Set SPI Mode
  ret = ioctl(fd, SPI_IOC_WR_MODE, &conf.mode);
  if (ret < 0) {
    PRINT_RES("Error setting mode",ret);
    return ret;
  }
  
  // Set Bits per word
  ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &conf.bits);
  if (ret < 0) {
    PRINT_RES("Error setting bits",ret);
    return ret;
  }

  // Set Max speed
  ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &conf.speed);
  if (ret < 0) {
    PRINT_RES("Error setting speed",ret);
    return ret;
  }

  // Initialize dummy transmit for receiving data.
  // Used by spi transaction, put here it to do it once.
  memset(&dummy_bit_seq, 0, sizeof(dummy_bit_seq));

  // Note: Use DEV_ID address to prevent clearing "clear-on-read" registers (Page 158)
  dummy_bit_seq[0] = (0 << MALIBU_RW_BIT_OFFSET);
  dummy_bit_seq[0] |= (0x0000 << MALIBU_PORTNO_BIT_OFFSET);
  dummy_bit_seq[0] |= (0x1E << MALIBU_DEVNO_BIT_SEQ_OFFSET);
  dummy_bit_seq[1] = 0x00; // DEV_ID
  dummy_bit_seq[2] = 0x00; // DEV_ID

  return ret;
}
int spi_transfer(int fd, uint8_t *tx, uint8_t *rx, uint32_t len) {

  PRINT_FUN();
  struct spi_ioc_transfer tr = {
    .tx_buf = (unsigned long)tx,
    .rx_buf = (unsigned long)rx,
    .len = len,
    .delay_usecs = 0,
    .speed_hz = spi_conf.speed,
    .bits_per_word = spi_conf.bits,
  };

  if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 1) {
      perror("Can't send spi message");
      return -1;
  }

  return 0;

}
////////////////////////////////////////////////////////////////////////////////
// spi32_bit_read code
////////////////////////////////////////////////////////////////////////////////
int spi_32bit_malibu_read_spidev(  void* inst,
		                              uint8_t port_no,
                                  uint8_t dev,
                                  uint16_t reg_num,
                                  uint32_t *value)
{
  /* 
   * The 1st bit sequence is for sending command to VSC8258.
   * The 2nd bit sequence is for receiving data from VSC8258
   */
  PRINT_FUN();
  int ret = 0;
  malibu_spi_slave_inst_bit_seq_t bit_seq_tx;
  malibu_spi_slave_inst_bit_seq_t bit_seq_rx;
  memset(&bit_seq_tx, 0, sizeof(bit_seq_tx));
  memset(&bit_seq_rx, 0, sizeof(bit_seq_rx));

  // Command Bit Sequence to Send.
  bit_seq_tx[0] = (0 << MALIBU_RW_BIT_OFFSET);
  bit_seq_tx[0] |= (port_no << MALIBU_PORTNO_BIT_OFFSET);
  bit_seq_tx[0] |= (dev << MALIBU_DEVNO_BIT_SEQ_OFFSET);
  bit_seq_tx[1] = (reg_num & 0xFF00) >> 8;
  bit_seq_tx[2] = (reg_num & 0x00FF);
  //bit_seq_tx[3] Don't Care "data" field

  // Transmit Bit Sequence
  ret = spi_transfer(fd, (uint8_t*)bit_seq_tx, NULL, MALIBU_BIT_SEQ_BYTE_COUNT);
  if (ret < 0) {
      PRINT_RES("Failed to Trasmit 1st bit sequence",ret);
  }
  // Receive Bit Sequece
  ret = spi_transfer(fd, (uint8_t*)dummy_bit_seq, (uint8_t*)bit_seq_rx, MALIBU_BIT_SEQ_BYTE_COUNT);
  if (ret < 0) {
      PRINT_RES("Failed to Trasmit 2nd bit sequence",ret);
  }

  // Copy 3-7 bytes of bit_seq_rx, contains data
  *value = bit_seq_rx[3] << 24;
  *value |= bit_seq_rx[4] << 16;
  *value |= bit_seq_rx[5] << 8;
  *value |= bit_seq_rx[6];
  PRINT_RES("Received value", *value);
  
  return 0;
}

int spi_32bit_malibu_write_spidev(  void* inst,
		                                uint8_t port_no,
                                    uint8_t  dev,
                                    uint16_t reg_num,
                                    uint32_t *value)
{
  /* 
  * The first bit sequence is for sending command to VSC8258.
  * The second bit sequence is for receiving data from VSC8258.
  */
  PRINT_FUN();
  int ret = 0;
  malibu_spi_slave_inst_bit_seq_t bit_seq_tx;
  malibu_spi_slave_inst_bit_seq_t bit_seq_rx;

  memset(&bit_seq_tx, 0, sizeof(bit_seq_tx));
  memset(&bit_seq_rx, 0, sizeof(bit_seq_rx));

  // First Bit Sequence to Send.
  bit_seq_tx[0] = (1 << MALIBU_RW_BIT_OFFSET);
  bit_seq_tx[0] |= (port_no << MALIBU_PORTNO_BIT_OFFSET);
  bit_seq_tx[0] |= (dev << MALIBU_DEVNO_BIT_SEQ_OFFSET);
  bit_seq_tx[1] = (reg_num >> 8);
  bit_seq_tx[2] = (reg_num);
  bit_seq_tx[3] = (*value & 0xFF000000) >> 24;
  bit_seq_tx[4] = (*value & 0xFF0000) >> 16;
  bit_seq_tx[5] = (*value & 0xFF00) >> 8;
  bit_seq_tx[6] = (*value & 0xFF);

  // Transmit Bit Sequence
  ret = spi_transfer(fd, (uint8_t*)bit_seq_tx, NULL, MALIBU_BIT_SEQ_BYTE_COUNT);
  if (ret < 0) {
      PRINT_RES("Failed to Trasmit 1st bit sequence",ret);
  }
  /* Leaving this code below commented
   * in case if we want to implement verify writing.
   */
  // // Receive Bit Sequece
  // ret = spi_transfer(fd, (uint8_t*)dummy_bit_seq, (uint8_t*)bit_seq_rx, MALIBU_BIT_SEQ_BYTE_COUNT);
  // if (ret < 0) {
  //     PRINT_RES("Failed to Trasmit 2nd bit sequence",ret);
  // }

  // // Copy 3-7 bytes of bit_seq_rx, contains data
  // *value = bit_seq_rx[3] << 24;
  // *value |= bit_seq_rx[4] << 16;
  // *value |= bit_seq_rx[5] << 8;
  // *value |= bit_seq_rx[6];
  return 0;
}

int spi_malibu_test_code(void) {

  PRINT_FUN();

  uint32_t *val = (uint32_t*)malloc(sizeof(uint32_t));
  *val = 0x55AA55AA;

  // Write to Temperature Monitor (15:0 bits are writable; others 0x00)
  spi_32bit_malibu_write_spidev(NULL, 0, 0x1e, 0x01C0, val);
  printf("Write value: 0x%08X\r\n\r\n", *val);

  // Read to Temperature Monitor
  spi_32bit_malibu_read_spidev(NULL, 0, 0x1e, 0x01C0, val);
  printf("Read value = 0x%08X\n",*val); 
  printf("Expected value: 0x000055AA\r\n"); 
  printf("Write value != read value due to upper 16-bit unwritable/read-only (0x0000)\r\n");

  // Read 
  spi_32bit_malibu_read_spidev(NULL, 0x0, 0x1e, 0x0000, val);
  printf("DEV_ID: 0x%08X\n", *val);
  printf("Expected DEV_ID value: 0x00008258\n\r\n");

  printf("Malibu SPI Test Done \r\n");
  printf("Make sure to reset PHY\r\n");

  exit(EXIT_SUCCESS);

  return 0;
}

