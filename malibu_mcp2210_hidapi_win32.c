//***************************************************************************
//* Malibu MCP2210-HIDAPI Win32 Connector
//*
//* This file contains board specific functions needed for running the PHY  *
//* API on a VSC8258EV with MCP2210. HID API is used to do hidraw read and write.*
//* HID API is supposedly able to run with Linux and WIN32.*
//* Currently, this demo is for WIN32 for now.
//* 
//* Author: Joemel John Diente <joemdiente@gmail.com>
//***************************************************************************

#include <stdarg.h> // For va_list
#include "vtss_api.h"   // For BOOL and friends
#include "./mesa/phy_demo_appl/appl/vtss_appl.h"  // For board types
#include <unistd.h>     // For TCP read/write
#include <sys/stat.h>// For /sys/class/mdio file status
#include <sys/types.h>// For /sys/class/mdio file status
#include <errno.h>// For System errors
#include <unistd.h>// For /sys/class/mdio
#include <stdio.h> // for FILE types
#include <stdlib.h> // for EXIT_SUCCESS
#include "vtss/api/options.h"

//MCP2210-HIDAPI
#include "hidapi.h"
#include <stdio.h> // printf
#include <wchar.h> // wchar_t
#include <Windows.h>
#include "mcp2210-hidapi.h"
#include "mcp2210-hidapi-gpio.h"
#include "mcp2210-hidapi-spi.h"
static hid_device *hidapi_handle;

// Define which trace group to use for VTSS printout in this file
#define VTSS_TRACE_GROUP VTSS_TRACE_GROUP_PHY

/* ================================================================= *
 *  Misc. functions
 * ================================================================= */

// Function defining the port interface.
static vtss_port_interface_t port_interface(vtss_port_no_t port_no)
{
return VTSS_PORT_INTERFACE_RGMII;
}

////////////////////////////////////////////////////////////////////////////////
// SPI_Read_Write Code for MCP2210 HIDAPI
////////////////////////////////////////////////////////////////////////////////
#define VSC85XX_BIT_SEQ_BYTE_COUNT 7 // Malibu bit sequence is 7 bytes.
typedef uint8_t vsc85xx_spi_slave_inst_bit_seq_t[VSC85XX_BIT_SEQ_BYTE_COUNT];

#define MALIBU_RW_BIT_OFFSET 7 
#define MALIBU_PORTNO_BIT_OFFSET 5
#define MALIBU_DEVNO_BIT_SEQ_OFFSET 0
#define MALIBU_BYTE_COUNT_PER_TRANSACTION 14

vtss_rc spi_32bit_read_write_mcp2210_hidapi(vtss_inst_t    inst,
		                 vtss_port_no_t port_no,
				 BOOL           read, // (0=rd, 1=wr)
                                 u8             dev,
                                 u16            reg_num,
                                 u32            *value)
{

  vsc85xx_spi_slave_inst_bit_seq_t bit_seq_1;
  vsc85xx_spi_slave_inst_bit_seq_t bit_seq_2;
  uint8_t bit_seq_all[7 * 2];
  uint8_t spi_stat = 0x0; 

  memset(&bit_seq_1, 0, sizeof(bit_seq_1));
  memset(&bit_seq_2, 0, sizeof(bit_seq_2));
  memset(&bit_seq_all, 0, sizeof(bit_seq_all));

  uint32_t i;
  // Read
  if (read) {
    // First Bit Sequence to Send.
    bit_seq_1[0] = (0 << MALIBU_RW_BIT_OFFSET);
    bit_seq_1[0] |= (port_no << MALIBU_PORTNO_BIT_OFFSET);
    bit_seq_1[0] |= (dev << MALIBU_DEVNO_BIT_SEQ_OFFSET);
    bit_seq_1[1] = (reg_num & 0xFF00) >> 8;
    bit_seq_1[2] = (reg_num & 0x00FF);
    //bit_seq_1[3] Don't Care "data" field

    // Second Bit Sequence to Send
    // Note: Use DEV_ID address to prevent clearing "clear-on-read" registers (Page 158)
    bit_seq_2[0] = (0 << MALIBU_RW_BIT_OFFSET);
    bit_seq_2[0] |= (0x0000 << MALIBU_PORTNO_BIT_OFFSET);
    bit_seq_2[0] |= (0x1E << MALIBU_DEVNO_BIT_SEQ_OFFSET);
    bit_seq_2[1] = 0x00; // DEV_ID
    bit_seq_2[2] = 0x00; // DEV_ID
    //bit_seq_1[3] Don't Care "data" field
  }
  // Write
  else {
    // First Bit Sequence to Send.
    bit_seq_1[0] = (1 << MALIBU_RW_BIT_OFFSET);
    bit_seq_1[0] |= (port_no << MALIBU_PORTNO_BIT_OFFSET);
    bit_seq_1[0] |= (dev << MALIBU_DEVNO_BIT_SEQ_OFFSET);
    bit_seq_1[1] = (reg_num >> 8);
    bit_seq_1[2] = (reg_num);
    bit_seq_1[3] = (*value & 0xFF000000) >> 24;
    bit_seq_1[4] = (*value & 0xFF0000) >> 16;
    bit_seq_1[5] = (*value & 0xFF00) >> 8;
    bit_seq_1[6] = (*value & 0xFF);

    // Second Bit Sequence to Send
    // Note: Use DEV_ID address to prevent clearing "clear-on-read" registers (Page 158)
    bit_seq_2[0] = (1 << MALIBU_RW_BIT_OFFSET);
    bit_seq_2[0] |= (0x0000 << MALIBU_PORTNO_BIT_OFFSET);
    bit_seq_2[0] |= (0x1E << MALIBU_DEVNO_BIT_SEQ_OFFSET);
    bit_seq_2[1] = 0x00; // DEV_ID
    bit_seq_2[2] = 0x00; // DEV_ID
    //Don't Care "data" field
  }

  // Combine bit_seq_1 and bit_seq_2 to issue only one mcp2210_spi_transfer_data()
  memcpy(&bit_seq_all, &bit_seq_1, sizeof(bit_seq_1));
  memcpy(&bit_seq_all[7], &bit_seq_2, sizeof(bit_seq_2));

  // Transmit Bit Sequences
  while (spi_stat != 0x10) {
      //Sent value is also being updated after received (byte_ptr_tx)
      spi_stat = mcp2210_spi_transfer_data(hidapi_handle, bit_seq_all, MALIBU_BYTE_COUNT_PER_TRANSACTION, bit_seq_all);
  };
  // Copy Byte offset 10 (Data in the 2nd Transaction) to value 
  // regardless of operation
  *value = bit_seq_all[10] << 24;
  *value |= bit_seq_all[11] << 16;
  *value |= bit_seq_all[12] << 8;
  *value |= bit_seq_all[13];
  return VTSS_RC_OK;
}

// Initialize MCP2210 HIDAPI
int mcp2210_hidapi_init(void) {

  int res;
	unsigned char buf[65];
  #define MAX_STR 255
	wchar_t wstr[MAX_STR];

  res = hid_init();

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	hidapi_handle = hid_open(0x4d8, 0xDE, NULL);
	if (!hidapi_handle) {
		printf("Unable to open device VID:0x4d8 PID:0xDE\n");
		hid_exit();
 		exit(EXIT_FAILURE);
	}
	printf("Opened MCP2210\r\n");

  //Initialize MCP2210 SPI Transfer (Comments with () shows Malibu Specs)
  mcp2210_spi_transfer_settings_t spi_cfg;
  mcp2210_spi_get_transfer_settings(hidapi_handle, &spi_cfg);
  spi_cfg.bitrate = 3000000; //3 MHz (Malibu can do up to 15.4MHz in normal; 30 MHz in fast)
  spi_cfg.active_cs_val = clear_bit(spi_cfg.active_cs_val, GP0); // Active Low
  spi_cfg.idle_cs_val = set_bit(spi_cfg.active_cs_val, GP0); // Idle High
  spi_cfg.cs_to_data_dly = 1; // 100 us (15 ns = TSU,SSN)
  spi_cfg.last_data_byte_to_cs = 1; // 100us (SCK clock + 15 ns = THD,SSN)
  spi_cfg.dly_bw_subseq_data_byte = 0; // No delay
  spi_cfg.byte_to_tx_per_transact = MALIBU_BYTE_COUNT_PER_TRANSACTION;
  spi_cfg.mode = SPI_MODE_0;
  mcp2210_spi_set_transfer_settings(hidapi_handle, spi_cfg);
  mcp2210_gpio_chip_settings_t gp_cfg;
  mcp2210_gpio_get_current_chip_settings(hidapi_handle, &gp_cfg);
  gp_cfg.gp_pin_designation[0] = GP_FUNC_CHIP_SELECTS; // CS0
  gp_cfg.spi_bus_release_disable = 0; // Released in each transfer
  mcp2210_gpio_set_current_chip_settings(hidapi_handle, gp_cfg);

  return VTSS_RC_OK;
}

// Board Init
int malibu_mcp2210_board_init(int argc, const char **argv, vtss_appl_board_t *board)
{
  // Arguments
  if (argc > 1) {
    if (strcmp(argv[1],"-h") == 0) {
      printf(" Help File: \r\n \
        Using VID: 0x04D8, PID: 0x00DE \r\n \
        Will use serial number = \"NULL\". \r\n \
        So only 1 MCP2210 should be connected.\r\n \
        ");
        
      exit(EXIT_FAILURE);
    } 
  }
  
  // Initialize MESA appl_board.
  printf("In %s\n", __FUNCTION__); fflush(stdout);
  board->descr = "Malibu_MCP2210";
  board->target = VTSS_TARGET_10G_PHY;  // 10G PHY
  board->port_count = VTSS_PORTS; //Setup the number of port used

  board->port_interface = port_interface; // Define the port interface
  board->init.init_conf->spi_32bit_read_write = *spi_32bit_read_write_mcp2210_hidapi; // Set pointer to the SPI read function for this board.

  //Initialize mcp2210-hidapi
  mcp2210_hidapi_init();

  u32 *val = (u32*)malloc(sizeof(u32));
#ifdef TEST_CODE
  *val = 0x55AA55AA;
  // Write to Temperature Monitor (15:0bits are writable; others 0x00)
  spi_32bit_read_write_mcp2210_hidapi(NULL, 0x0, 0, 0x1e, 0x01C0, val);
  // Read
  spi_32bit_read_write_mcp2210_hidapi(NULL, 0x0, 1, 0x1e, 0x01C0, val);
  printf("Value = 0x%08x\n",*val); fflush(stdout);
#endif

  // Read
  spi_32bit_read_write_mcp2210_hidapi(NULL, 0x0, 1, 0x1e, 0x0000, val);
  printf("DEV_ID: 0x%08x\n",*val); fflush(stdout);
  printf("Out malibu_char_board_init\n"); fflush(stdout);
  return 0;
}

// Function for initializing the hardware board.
int malibu_board_init(int argc, const char **argv, vtss_appl_board_t *board)
{
  malibu_mcp2210_board_init(argc, argv, board); // Use init function for Rabbit MIIM.
  return 0;
}
