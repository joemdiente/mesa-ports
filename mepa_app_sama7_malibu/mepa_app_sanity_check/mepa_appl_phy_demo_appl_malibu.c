//***************************************************************************
//* Malibu Linux SPIDEV Connector
//*
//* This file contains board specific functions needed for running the PHY  *
//* API on a VSC8258EV. linux_spidev is based on rpi_spi
//* 
//* Author: Joemel John Diente <joemdiente@gmail.com>
//***************************************************************************

#include <stdarg.h> // For va_list
#include "vtss_api.h"   // For BOOL and friends
#include "./appl/vtss_appl.h"  // For board types
#include <unistd.h>     // For TCP read/write
#include <sys/stat.h>// For /sys/class/mdio file status
#include <sys/types.h>// For /sys/class/mdio file status
#include <errno.h>// For System errors
#include <unistd.h>// For /sys/class/mdio
#include <stdio.h> // for FILE types
#include <stdlib.h> // for EXIT_SUCCESS
#include "vtss/api/options.h"

//RPI_SPI
#include "rpi_spi_vtss.h"

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

// SPI 32bit read_write implementation
vtss_rc linux_spidev_32bit_read_write(vtss_inst_t    inst,
		                 vtss_port_no_t port_no,
				 BOOL           read, // (0=rd, 1=wr)
                                 u8             dev,
                                 u16            reg_num,
                                 u32            *value)
{
    vtss_rc ret = 0;
    /* Use rpi_spi.c code */
    if(read) {
        ret = rpi_spi_32bit_read(NULL, port_no, dev, reg_num, value);
    }
    else {
        ret = rpi_spi_32bit_write(NULL, port_no, dev, reg_num, value);
    }
}
// Board Init
int malibu_linux_spidev_board_init(int argc, const char **argv, vtss_appl_board_t *board)
{
    char* spidev;
    // Arguments
    if (argc > 2) {
        if (strcmp(argv[1],"-h") == 0) {
        printf(" Help File: \r\n \
            Options:\
                -d /dev/spidevx.y \r\n \
            ");
        exit(EXIT_FAILURE);
        }
        if (strcmp(argv[1],"-d") == 0) {
            printf(" Using %s", argv[2]);
            spidev = (char*)argv[2];
        }
    }
    if (argc < 2) {
        printf("Error: Require Arguments. See -h\r\n");
        exit(EXIT_FAILURE);
    }

    // Initialize MESA appl_board.
    printf("In %s\n", __FUNCTION__); fflush(stdout);
    board->descr = "Malibu controlled by Linux using SPIDEV";
    board->target = VTSS_TARGET_10G_PHY;  // 10G PHY
    board->port_count = VTSS_PORTS; //Setup the number of port used

    board->port_interface = port_interface; // Define the port interface
    board->init.init_conf->spi_32bit_read_write = *linux_spidev_32bit_read_write; // Set pointer to the SPI read function for this board.

    //Initialize linux_spidev
    printf("Initialize rpi_spi_vtss\r\n");
    if(spi_initialize(spidev)) {
        printf("Error spi_initialize()\r\n \
            Exiting...\r\n");
        exit(1);
    }

    u32 *val = (u32*)malloc(sizeof(u32));

    // Read
    linux_spidev_32bit_read_write(NULL, 0x0, 1, 0x1e, 0x0000, val);
    printf("DEV_ID: 0x%08x\n",*val); fflush(stdout);
    printf("Out malibu_char_board_init\n"); fflush(stdout);
    return 0;
}

// Function for initializing the hardware board.
int malibu_board_init(int argc, const char **argv, vtss_appl_board_t *board)
{
    malibu_linux_spidev_board_init(argc, argv, board);
    return 0;
}
