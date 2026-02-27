/* 
 * MDIO Accessor Function for MEPA
 *
 * Copyright (C) 2026 Microchip Technology Inc.
 *
 * Author: Joemel John A. Diente <joemeljohn.diente@microchip.com>
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

#include <stdarg.h> // For va_list
#include <unistd.h> 
#include <sys/stat.h> // For /sys/class/mdio file status
#include <sys/types.h> // For /sys/class/mdio file status
#include <errno.h> // For System errors
#include <unistd.h> // For /sys/class/mdio
#include <stdio.h> // for FILE types
#include <stdlib.h> // for EXIT_SUCCESS
#include <time.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include "linux_mdio.h"
#include "mdio.h"

////////////////////////////////////////////////////////////////////////////////
// Initialize Variables 
////////////////////////////////////////////////////////////////////////////////
/* Track PHY_ID */
static uint8_t _phy_id = 0;
static char* _mdio_bus = ""; 
static mdio_init_done = 0;

/* Flag to track if callback has been executed */
static volatile int callback_done = 0;
static volatile int callback_err = 0;
/* Store PHY Register Value */
static volatile uint16_t phy_reg_val = 0;

typedef struct {
	int cb_done;
	int cb_err;
	uint16_t value;
} mdio_ops_t;
////////////////////////////////////////////////////////////////////////////////
// MDIO Functions for SAMA7 or Any Linux Machine.
// This is a copy of wkz/mdio-tools/blob/master/src/mdio/main.c
////////////////////////////////////////////////////////////////////////////////
int list_available_mdio_buses() {
	DIR *d;
	struct dirent *dir;
	d = opendir("/sys/class/mdio_bus");
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_type == DT_LNK) {
				printf("--> %s\n", dir->d_name);
			}
		}
		closedir(d);
		return 0;
	} else {
		fprintf(stderr, "ERROR: Unable to open /sys/class/mdio_bus.\r\n");
		return -1;
	}
}

int match_available_mdio_bus(char* mdio_bus) {
	DIR *d;
	struct dirent *dir;
	d = opendir("/sys/class/mdio_bus");
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_type == DT_LNK) {
				if(strcmp(dir->d_name, mdio_bus) == 0) {
					printf("Selected mdio bus: %s;", dir->d_name);
					return 0;
				}
			}
		}
		closedir(d);
		printf("ERROR: mdio bus \"%s\" not found in /sys/class/mdio_bus.\r\n", mdio_bus);
		return -1;
	} 
	printf("ERROR: Unable to open /sys/class/mdio_bus.\r\n");
	return -1;
}
int mdio_initialize(char* mdio_bus, uint8_t* phy_id) {

	if (*phy_id < 0 || *phy_id > 0x1F) {
		printf("Error: Invalid phy_id 0x%02X\r\n", *phy_id);
		exit(EXIT_FAILURE);
	} else{
		// Store PHY ID
		_phy_id = *phy_id;
	}

	if (mdio_init()) {
		if (mdio_modprobe()) {
			fprintf(stderr, "ERROR: mdio-netlink module not "
				"detected, and could not be loaded.\n");
			return -1;
		}

		if (mdio_init()) {
			fprintf(stderr, "ERROR: Unable to initialize.\n");
			return -1;
		}
	}

	printf("mdio-netlink module loaded or already running.\r\n");

	if (strcmp(mdio_bus, "") == 0) {
		printf("Available mdio buses: \r\n");
		list_available_mdio_buses();
		printf("Please pass one of the above as an \'exact\' argument using -m <mdio_bus>. \r\n");
		printf("Using wildcard is not supported. \r\n");
		return -1;
	} else {
		if (match_available_mdio_bus(mdio_bus) != 0) {
			exit(EXIT_FAILURE);
		}
		_mdio_bus = mdio_bus;
		printf(" Using PHY Address: %d\r\n",_phy_id);
	} 
	mdio_init_done = 1;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
// Wrapper Functions for MEPA
////////////////////////////////////////////////////////////////////////////////
int mdio_read_cb(uint32_t *data, int len, int err, void *arg)
{
	mdio_ops_t* cb_ops = (struct mdio_ops_t*)arg;
	cb_ops->cb_done = 1;
	printf("mdio_read_cb called\r\n");
	return 0;
}
uint8_t mdio_read (struct mepa_callout_ctx *ctx, uint8_t addr, uint16_t *value) {
	mdio_ops_t mdio_op = {};
	int err = 0;
	printf("mdio_read called\r\n");
	/* Create instructions */
	struct mdio_nl_insn insns[] = {
		INSN(READ,  IMM(_phy_id), IMM(addr),  REG(0)),  /* PHY register 2 */
		INSN(EMIT,  REG(0),      0,       0),        /* Emit register 2 value */
	};

	/* Create program from instructions */
	struct mdio_prog prog = MDIO_PROG_FIXED(insns);

	/* Execute the program and call callback with results */
	err = mdio_xfer(_mdio_bus, &prog, mdio_read_cb, &mdio_op); // read_phy_regs_cb was writen by AI.

	while(mdio_op.cb_done != 1);

	if (err) {
		fprintf(stderr, "ERROR: Failed to execute mdio_xfer (%d)\n", err);
		return 1;
	}
	return 0;
}
int mdio_write_cb(uint32_t *data, int len, int err, void *arg)
{
	return 0;
}
uint8_t mdio_write (struct mepa_callout_ctx *ctx, uint8_t addr, uint16_t value) {
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
// Test Functions
////////////////////////////////////////////////////////////////////////////////
/* Callback to process the PHY register data */
int read_phy_reg_cb_test1(uint32_t *data, int len, int err, void *arg)
{
	if (len != 2) {
		fprintf(stderr, "	[cb]ERROR: Expected 2 register values, got %d\n", len);
		return 1;
	}
	printf("	[cb]read_phy_regs_cb length: %d\r\n", len);
	uint16_t reg2 = data[0];  /* PHY ID High */
	uint16_t reg3 = data[1];  /* PHY ID Low */
	uint32_t phy_id = ((uint32_t)reg2 << 16) | reg3;

	printf("	[cb]PHY Register 2 (ID High): 0x%04x\n", reg2);
	printf("	[cb]PHY Register 3 (ID Low):  0x%04x\n", reg3);
	printf("	[cb]Combined PHY ID:          0x%08x\n", phy_id);

	return err;
}
/* Structure to store register data test2 */
struct phy_regs {
	uint32_t values[32];
	int num_values;
};
/* Callback that stores all register values in the struct */
int store_phy_regs_cb_test2(uint32_t *data, int len, int err, void *arg)
{
	struct phy_regs *regs = (struct phy_regs *)arg;

	if (len > 32) {
		fprintf(stderr, "	[cb]ERROR: Too many values (%d)\n", len);
		callback_err = err;
		return 1;
	}

	/* Copy all register values to our struct */
	memcpy(regs->values, data, len * sizeof(uint32_t));
	regs->num_values = len;
	callback_err = err;
	callback_done = 1;

	return err;
}
/* Callback for read operation */
static int read_reg_cb_test3(uint32_t *data, int len, int err, void *arg)
{
	if (len != 1) {
		fprintf(stderr, "	[cb]ERROR: Expected 1 value, got %d\n", len);
		callback_err = err;
		callback_done = 1;
		return 1;
	}
	printf("	[cb]PHY register 14 value: 0x%04X\r\n", data[0]);
	phy_reg_val = data[0];
	callback_done = 1;
	return err;
}

/* Callback for write operation */
static int write_reg_cb_test3(uint32_t *data, int len, int err, void *arg)
{
	if (len != 0) {
		fprintf(stderr, "	[cb]ERROR: Expected 0 values for write, got %d\n", len);
		callback_err = err;
		callback_done = 1;
		return 1;
	}

	printf("	[cb]PHY register 14 written with value.\n");

	callback_err = err;
	callback_done = 1;

	return err;
}
// mdio_test_code
int mdio_test_code(void) {

	int err = 0;
	if (mdio_init_done != 1) {
		printf("Please initialize mdio first!!\r\n");
		return -1;
	}

	printf("Entered mdio_test_code()...");
	printf("Using mdio dev, phy_id: %s, 0x%02X\r\n", _mdio_bus, _phy_id);
	printf("===> mdio_test_code: Test #1\r\n");

	// Test 1 - Read PHY register 2 and 3
	// Uses MDIO_PROG_FIXED without callback checking.
	/* Create instruction array to read registers 2 and 3 */
	struct mdio_nl_insn insns_test1[] = {
		INSN(READ,  IMM(_phy_id), IMM(2),  REG(0)),  /* PHY register 2 */
		INSN(EMIT,  REG(0),      0,       0),        /* Emit register 2 value */
		INSN(READ,  IMM(_phy_id), IMM(3),  REG(0)),  /* PHY register 2 */
		INSN(EMIT,  REG(0),      0,       0),        /* Emit register 3 value */
	};

	/* Create program from instructions */
	struct mdio_prog prog_test1 = MDIO_PROG_FIXED(insns_test1);

	/* Execute the program and call callback with results */
	err = mdio_xfer(_mdio_bus, &prog_test1, read_phy_reg_cb_test1, NULL); // read_phy_regs_cb was writen by AI.

	if (err) {
		fprintf(stderr, "	ERROR: Failed to read PHY registers (%d)\n", err);
		return 1;
	}
	
	printf("===> mdio_test_code: Test #2\r\n");
	// Test 2 - Pretty dump PHY registers
	// Uses MDIO_PROG_EMPTY and with callback timeout checking
	/* Create a struct to store results */
	struct phy_regs regs = {0};
	int reg_no;
	err = 0;

	printf("	Building instruction program to read all 32 registers...\n");

	/* Create program */
	struct mdio_prog prog_test2 = MDIO_PROG_EMPTY;

	/* Add READ + EMIT instructions for all 32 registers */
	for (reg_no = 0; reg_no < 32; reg_no++) {
		mdio_prog_push(&prog_test2, INSN(READ, IMM(_phy_id), IMM(reg_no), REG(0)));
		mdio_prog_push(&prog_test2, INSN(EMIT, REG(0), 0, 0));
	}

	printf("	Program created with %d instructions\n", prog_test2.len);
	printf("	Executing MDIO program on bus: %s\n", _mdio_bus);
	printf("	Reading PHY ID %u (all registers 0-31)...\n\n", _phy_id);

	/* Execute with callback that stores in struct */
	err = mdio_xfer(_mdio_bus, &prog_test2, store_phy_regs_cb_test2, &regs);
	
	while(callback_done != 1);
	
	free(prog_test2.insns);

	if (err) {
		fprintf(stderr, "E	RROR: mdio_xfer failed with code: %d\n", err);
		return 1;
	}

	/* Print all register values */
	printf("	=== PHY Register Dump (PHY ID = %u) ===\n\n", _phy_id);
	printf("	Received %d register values\n", regs.num_values);
	printf("	Callback error code: %d\n\n", callback_err);

	printf("	%-10s %-10s %-40s\n", "Register", "Value", "Description");
	printf("	%-10s %-10s %-40s\n", "--------", "-----", "-----------");

	const char *descriptions[] = {
		"BMCR (Basic Mode Control)",
		"BMSR (Basic Mode Status)",
		"PHYIDR1 (PHY Identifier High)",
		"PHYIDR2 (PHY Identifier Low)",
		"ANAR (Auto-Negotiation Advertisement)",
		"ANLPAR (Auto-Neg Link Partner Ability)",
		"ANER (Auto-Negotiation Expansion)",
		"ANNPTR (Auto-Neg Next Page TX)",
		"ANNPRR (Auto-Neg Next Page RX)",
		"GBCR (Gigabit Basic Control)",
		"GBSR (Gigabit Basic Status)",
		"MSCR (MMD Access Control 1)",
		"MSCSR (MMD Access Control 2)",
		"EXSR (Extended Status)",
		"Reserved 14",
		"Reserved 15",
		"Reserved 16",
		"Reserved 17",
		"Reserved 18",
		"Reserved 19",
		"Reserved 20",
		"Reserved 21",
		"Reserved 22",
		"Reserved 23",
		"Reserved 24",
		"Reserved 25",
		"Reserved 26",
		"Reserved 27",
		"Reserved 28",
		"Reserved 29",
		"Reserved 30",
		"Reserved 31"
	};

	if (regs.num_values != 32) {
		fprintf(stderr, "	WARNING: Expected 32 values, got %d\n", regs.num_values);
	}

	for (int i = 0; i < regs.num_values; i++) {
		printf("	%-10d 0x%-8x %s\n", i, regs.values[i], descriptions[i]);
	}

	printf("===> mdio_test_code: Test #3\r\n");

	// Test 3 - Read - Write - Verify - Reset Val Test 
	// This is the exact logic being used by MEPA MDIO Accessor Functions.
	phy_reg_val = 0; // Clear value;
	err = 0;
	callback_done = 0;
	callback_err = 0;

	// Read
	printf("	Read PHY Register 14\r\n");
	printf("	Will use as scratchpad.\r\n");

	// Create Fixed Instruction
	struct mdio_nl_insn read_insns_test3[] = {
		INSN(READ,  IMM(_phy_id), IMM(14),  REG(0)), /* PHY register 14 (MMD Access Address/Data Register) - Used as a scratchpad */
		INSN(EMIT,  REG(0),      0,       0),        /* Emit register 14 value */
	};

	/* Create program from instructions */
	struct mdio_prog prog_test3 = MDIO_PROG_FIXED(read_insns_test3);

	err = mdio_xfer(_mdio_bus, &prog_test3, read_reg_cb_test3, NULL); // Note: 4th parameter can be used to get data from cb to this app.
	while(callback_done != 1);

	if (err) {
		fprintf(stderr, "	ERROR: mdio_xfer failed (%d)\n", err);
		return -1;
	}
	if (callback_err) {
		fprintf(stderr, "	ERROR: Read operation failed\n");
		return -1;
	}

	// Write
	printf("	Write PHY Register 14; Use value 0xAA55\r\n");
	uint16_t value = 0xAA55;
	err = 0;
	callback_done = 0;
	callback_err = 0;

	memset(&prog_test3, 0, sizeof(prog_test3));
	mdio_prog_push(&prog_test3, INSN(WRITE, IMM(_phy_id), IMM(14), IMM(value)));
	err = mdio_xfer(_mdio_bus, &prog_test3, write_reg_cb_test3, NULL);

	while(callback_done != 1);

	if (err) {
		fprintf(stderr, "	ERROR: mdio_xfer failed (%d)\n", err);
		return -1;
	}
	if (callback_err) {
		fprintf(stderr, "	ERROR: Write operation failed\n");
		return -1;
	}

	// Verify - Just repeats read.
	printf("	Verify PHY Register 14\r\n");
	printf("	Value should be 0xAA55.\r\n");
	err = 0;
	callback_done = 0;
	callback_err = 0;
	phy_reg_val = 0;

	/* Create program from instructions */
	memset(&prog_test3, 0, sizeof(prog_test3));
	prog_test3 = MDIO_PROG_FIXED(read_insns_test3);

	// Note: 4th parameter can be used to get data from cb to this app. But data is currently global.
	err = mdio_xfer(_mdio_bus, &prog_test3, read_reg_cb_test3, NULL); 

	while(callback_done != 1);

	if (err) {
		fprintf(stderr, "ERROR: mdio_xfer failed (%d)\n", err);
		return -1;
	}
	if (callback_err) {
		fprintf(stderr, "ERROR: Verify operation failed\n");
		return -1;
	}

	if (phy_reg_val != 0xAA55) {
		printf("	Verified failed!\r\n");
	} else {
		printf("	Verified Success. read_val == 0x%04X\r\n", phy_reg_val);
	}

	// Clear PHY Registers 14
	printf("	Clear PHY Register 14; Use value 0x0000\r\n");
	err = 0;
	callback_done = 0;
	callback_err = 0;

	memset(&prog_test3, 0, sizeof(prog_test3));
	mdio_prog_push(&prog_test3, INSN(WRITE, IMM(_phy_id), IMM(14), IMM(0x0000)));
	err = mdio_xfer(_mdio_bus, &prog_test3, write_reg_cb_test3, NULL);

	while(callback_done != 1);

	if (err) {
		fprintf(stderr, "	ERROR: mdio_xfer failed (%d)\n", err);
		return -1;
	}
	if (callback_err) {
		fprintf(stderr, "	ERROR: Clear operation failed\n");
		return -1;
	}

	printf("	Test Code Success!\n");
	free(&prog_test3);
	return 0;
	// Get PHY 
}

// *******************************
// END OF FILE