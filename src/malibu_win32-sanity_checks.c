// Copyright (c) 2004-2020 Microchip Technology Inc. and its subsidiaries.
// SPDX-License-Identifier: MIT

/*
 *****************************************************************************************
* Revision 0.6  2018/10/23 sunilp
* Fixed issue related to vtss_phy_10g_init() and commented out the call to vtss_phy_10g_mode_get
* Revision 0.5  2018/10/03 sunilp
* Changed MALIBU_EVAL_BOARD to MALIBU_CHAR_BOARD
* Revision 0.4  2018/08/31 sunilp
* Added support for different data rates
* Revision 0.3  2018/07/19 sunilp
* Modified GPIO functionality to match datasheet documentation and added aggregate interrupt
* Added SCKOUT/CKOUT support, 10GBASE-KR support
* Revision 0.2  2018/06/29 sunilp
* Added GPIO functionality
* Revision 0.1  2018/06/27 sunilp
* 0.1 is the first version for the Malibu family of products (VSC8254/57/58(-xx))
 *****************************************************************************************

*/
#include <vtss_api.h>   // For board initialization
#include <vtss_appl.h>  // For vtsss_board_t
#include <vtss_port_api.h>
#include <malibu_win32-sanity_checks.h> // For board init
#include "../base/ail/vtss_state.h" // For AIL dumping function
#include <stdarg.h> // For va_list

/* ================================================================= *
 *  Board init.
 * ================================================================= */
#define MAX_BOARD_INSTANCES    (2)
/* Board specifics */
static vtss_appl_board_t board_table[MAX_BOARD_INSTANCES];

// Function defining the board
//
// In : Pointer to the board definition
void vtss_board_phy_init(vtss_appl_board_t *board)
{
	memset (board, 0, sizeof(vtss_appl_board_t));
	board->descr = "PHY"; // Description
	board->target = VTSS_TARGET_10G_PHY; // Target
	board->board_init = malibu_mcp2210_board_init; // Pointer to function initializing the board
}

/* ================================================================= *
 * API REQUIRED FUNCTIONS
 * FOR SOME REASONS, REMOVING THESE FAILED LINKING.
 * ================================================================= */
/* ================================================================= *
 *  API lock/unlock functions - If the Application support threads
 *  the API must be protected via Mutexes
 * ================================================================= */
BOOL api_locked = FALSE;

// Function call by the API, when the API shall do mutex lock.
void vtss_callout_lock(const vtss_api_lock_t *const lock)
{
    // For testing we don't get a deadlock. The API must be unlocked before "locking"
    if (api_locked) {
        T_E("API lock problem");
    }
    api_locked = TRUE;
}

// Function call by the API, when the API shall do mutex unlock.
void vtss_callout_unlock(const vtss_api_lock_t *const lock)
{
    // For testing we don't get a deadlock. vtss_callout_lock must have been called before vtss_callout_unlock is called.
    if (!api_locked) {
        T_E("API unlock problem");
    }
    api_locked = FALSE;
}

/* ================================================================= *
 *  Debug trace
 * ================================================================= */
vtss_trace_conf_t vtss_appl_trace_conf = {
    //.level[0] = VTSS_TRACE_LEVEL_NONE
    //.level[0] = VTSS_TRACE_LEVEL_ERROR
    //.level[0] = VTSS_TRACE_LEVEL_INFO
    .level[0] = VTSS_TRACE_LEVEL_DEBUG
    //.level[0] = VTSS_TRACE_LEVEL_NOISE
};

// Trace callout function - This function is called for printing out debug information from the API
// Different levels of trace are support. The level are :
// 1) VTSS_TRACE_LEVEL_NONE   - No information from the API will be printed
// 2) VTSS_TRACE_LEVEL_ERROR  - Printout of T_E/VTSS_E trace. Error messages for malfunctioning API
// 3) VTSS_TRACE_LEVEL_WARNING- Printout of T_W/VTSS_W trace. Warning messages for unexpected API behavior.
// 4) VTSS_TRACE_LEVEL_INFO   - Printout of T_I/VTSS_I trace. Debug messages.
// 5) VTSS_TRACE_LEVEL_DEBUG  - Printout of T_D/VTSS_D trace. Even more debug messages.
// 6) VTSS_TRACE_LEVEL_NOISE  - Printout of T_N/VTSS_N trace. Even more debug messages.
void vtss_callout_trace_printf(const vtss_trace_layer_t layer,
                               const vtss_trace_group_t group,
                               const vtss_trace_level_t level,
                               const char *file,
                               const int line,
                               const char *function,
                               const char *format,
                               ...)
{
    va_list va;

}
/* ================================================================= *
 * END OF API REQUIRED FUNCTIONS
 * ================================================================= */

/* ********************************************************** */
/* ********************************************************** */
/* ******    Test SPI I/O                 ******************* */
/* ********************************************************** */
#define SPI_WR  (0)
#define SPI_RD  (1)
BOOL  vtss_appl_malibu_spi_io_test(const vtss_inst_t   inst, const vtss_port_no_t   port_no)
{
    u32 val32 = 0;
    u32 val32B = 0;
    u16 addr;
    u8 dev;

    dev = 0x1E; addr = 0x0;
    inst->init_conf.spi_32bit_read_write(inst, 0, SPI_RD, dev, addr, &val32);
    printf("0x%Xx%04X = 0x%X\n", dev, addr, val32); 

    dev = 0x1E; addr = 0x0;
    inst->init_conf.spi_32bit_read_write(inst, 1, SPI_RD, dev, addr, &val32);
    printf("1x%Xx%04X = 0x%X\n", dev, addr, val32);

    dev = 0x1E; addr = 0x0;
    inst->init_conf.spi_32bit_read_write(inst, 2, SPI_RD, dev, addr, &val32);
    printf("2x%Xx%04X = 0x%X\n\n", dev, addr, val32); 

    // dev = 0x1E; addr = 0x9002;
    // inst->init_conf.spi_32bit_read_write(NULL, 0, SPI_RD, dev, addr, &val32);
    // printf("0x%Xx%04X = 0x%X\n\n", dev, addr, val32); 
    // *val32 = 0x4;
    // inst->init_conf.spi_32bit_read_write(NULL, 0, SPI_WR, dev, addr, &val32);
    // inst->init_conf.spi_32bit_read_write(NULL, 0, SPI_RD, dev, addr, &val32);
    // printf("0x%Xx%04X = 0x%X\n\n", dev, addr, val32); 
    //
    // dev = 0x1E; addr = 0x9002;
    // inst->init_conf.spi_32bit_read_write(NULL, 2, SPI_RD, dev, addr, &val32);
    // printf("2x%Xx%04X = 0x%X\n\n", dev, addr, val32); 
    // *val32 = 0x4;
    // inst->init_conf.spi_32bit_read_write(NULL, 2, SPI_WR, dev, addr, &val32);
    // inst->init_conf.spi_32bit_read_write(NULL, 2, SPI_RD, dev, addr, &val32);
    // printf("2x%Xx%04X = 0x%X\n\n", dev, addr, val32); 
    //
    // dev = 0x1E; addr = 0x9202;
    // inst->init_conf.spi_32bit_read_write(NULL, 0, SPI_RD, dev, addr, &val32);
    // printf("0x%Xx%04X = 0x%X\n\n", dev, addr, val32); 
    // *val32 = 0x4;
    // inst->init_conf.spi_32bit_read_write(NULL, 0, SPI_WR, dev, addr, &val32);
    // inst->init_conf.spi_32bit_read_write(NULL, 0, SPI_RD, dev, addr, &val32);
    // printf("0x%Xx%04X = 0x%X\n\n", dev, addr, val32); 
    //
    // dev = 0x1E; addr = 0x9202;
    // inst->init_conf.spi_32bit_read_write(NULL, 2, SPI_RD, dev, addr, &val32);
    // printf("2x%Xx%04X = 0x%X\n\n", dev, addr, val32); 
    // *val32 = 0x4;
    // inst->init_conf.spi_32bit_read_write(NULL, 2, SPI_WR, dev, addr, &val32);
    // inst->init_conf.spi_32bit_read_write(NULL, 2, SPI_RD, dev, addr, &val32);
    // printf("2x%Xx%04X = 0x%X\n\n", dev, addr, val32); 

    dev = 0x1; addr = 0xF112;
    inst->init_conf.spi_32bit_read_write(inst, 0, SPI_RD, dev, addr, &val32);
    printf("0x%Xx%04X = 0x%X\n", dev, addr, val32); 
    val32 = 0x003DF828;
    inst->init_conf.spi_32bit_read_write(inst, 0, SPI_WR, dev, addr, &val32);
    inst->init_conf.spi_32bit_read_write(inst, 0, SPI_RD, dev, addr, &val32);
    printf("0x%Xx%04X = 0x%X\n\n", dev, addr, val32); 
    dev = 0x1; addr = 0xF112;
    inst->init_conf.spi_32bit_read_write(inst, 1, SPI_RD, dev, addr, &val32);
    printf("1x%Xx%04X = 0x%X\n", dev, addr, val32);
    val32 = 0x004DF828;
    inst->init_conf.spi_32bit_read_write(inst, 1, SPI_WR, dev, addr, &val32);
    inst->init_conf.spi_32bit_read_write(inst, 1, SPI_RD, dev, addr, &val32);
    printf("1x%Xx%04X = 0x%X\n\n", dev, addr, val32); 

    dev = 0x1; addr = 0xF112;
    inst->init_conf.spi_32bit_read_write(inst, 2, SPI_RD, dev, addr, &val32);
    printf("2x%Xx%04X = 0x%X\n", dev, addr, val32);
    val32 = 0x005DF828;
    inst->init_conf.spi_32bit_read_write(inst, 2, SPI_WR, dev, addr, &val32);
    inst->init_conf.spi_32bit_read_write(inst, 2, SPI_RD, dev, addr, &val32);
    printf("2x%Xx%04X = 0x%X\n", dev, addr, val32); 
    val32 = 0x007DF820;
    inst->init_conf.spi_32bit_read_write(inst, 2, SPI_WR, dev, addr, &val32);
    inst->init_conf.spi_32bit_read_write(inst, 2, SPI_RD, dev, addr, &val32);
    printf("2x%Xx%04X = 0x%X\n\n", dev, addr, val32); 

    dev = 0x1; addr = 0xF112;
    inst->init_conf.spi_32bit_read_write(inst, 3, SPI_RD, dev, addr, &val32);
    printf("3x%Xx%04X = 0x%X\n", dev, addr, val32); 
    val32 = 0x006DF828;
    inst->init_conf.spi_32bit_read_write(inst, 3, SPI_WR, dev, addr, &val32);
    inst->init_conf.spi_32bit_read_write(inst, 3, SPI_RD, dev, addr, &val32);
    printf("3x%Xx%04X = 0x%X\n", dev, addr, val32); 
    val32 = 0x007DF820;
    inst->init_conf.spi_32bit_read_write(inst, 3, SPI_WR, dev, addr, &val32);
    inst->init_conf.spi_32bit_read_write(inst, 3, SPI_RD, dev, addr, &val32);
    printf("3x%Xx%04X = 0x%X\n\n", dev, addr, val32); 

    dev = 0x1; addr = 0xF120;
    inst->init_conf.spi_32bit_read_write(inst, 0, SPI_RD, dev, addr, &val32);
    printf("0x%Xx%04X = 0x%X\n", dev, addr, val32); 
    inst->init_conf.spi_32bit_read_write(inst, 1, SPI_RD, dev, addr, &val32);
    printf("1x%Xx%04X = 0x%X\n", dev, addr, val32); 
    inst->init_conf.spi_32bit_read_write(inst, 2, SPI_RD, dev, addr, &val32);
    printf("2x%Xx%04X = 0x%X\n", dev, addr, val32); 
    inst->init_conf.spi_32bit_read_write(inst, 3, SPI_RD, dev, addr, &val32);
    printf("3x%Xx%04X = 0x%X\n\n", dev, addr, val32); 

    dev = 0x1; addr = 0xF121;
    inst->init_conf.spi_32bit_read_write(inst, 0, SPI_RD, dev, addr, &val32);
    printf("0x%Xx%04X = 0x%X\n", dev, addr, val32); 
    val32B = 0x48888924;
    inst->init_conf.spi_32bit_read_write(inst, 0, SPI_WR, dev, addr, &val32B);
    inst->init_conf.spi_32bit_read_write(inst, 0, SPI_RD, dev, addr, &val32B);
    printf("0x%Xx%04X = 0x%X\n", dev, addr, val32B); 

    // *val32 = 0x88888924;
    inst->init_conf.spi_32bit_read_write(inst, 0, SPI_WR, dev, addr, &val32);
    inst->init_conf.spi_32bit_read_write(inst, 0, SPI_RD, dev, addr, &val32);
    printf("0x%Xx%04X = 0x%X\n\n\n", dev, addr, val32); 

}

/* ================================================================= *
 *  START_OF_MAIN
 * ================================================================= */
int main(int argc, const char **argv) {
  vtss_rc                 rc;
  int                     phy_mode = 0;
  vtss_appl_board_t       *board;
  vtss_inst_create_t      create;
  vtss_port_no_t          port_no=0;
  vtss_port_no_t          phy_base_port=0;
  vtss_init_conf_t        init_conf;
  vtss_port_interface_t   mac_if;
  char value_str[255] = {0};
  char command[255] = {0};
  char port_no_str[255] = {0};
  BOOL enable_1588 = FALSE;
  vtss_port_no_t ing_port_no = 0;
  vtss_port_no_t egr_port_no = 0;
  u32 val32 = 0;
  u16 value = 0;

  // Setup trace level for PHY group -- in increasing order of logging messages
  // VTSS_TRACE_LEVEL_NONE
  // VTSS_TRACE_LEVEL_ERROR
  // VTSS_TRACE_LEVEL_INFO
  // VTSS_TRACE_LEVEL_DEBUG
  // VTSS_TRACE_LEVEL_NOISE
  vtss_appl_trace_conf.level[VTSS_TRACE_LAYER_AIL] = VTSS_TRACE_LEVEL_ERROR;
  vtss_appl_trace_conf.level[VTSS_TRACE_LAYER_CIL] = VTSS_TRACE_LEVEL_ERROR;
  printf ("//Default AIL TRACE Level : VTSS_TRACE_LEVEL_ERROR\n" );
  printf ("//Default CIL TRACE Level : VTSS_TRACE_LEVEL_ERROR\n" );
  //vtss_appl_trace_conf.level[VTSS_TRACE_LAYER_AIL] = VTSS_TRACE_LEVEL_DEBUG;
  //printf ("//Default AIL TRACE Level : VTSS_TRACE_LEVEL_DEBUG\n" );

  // Setup trace level for PHY group - vtss_appl_trace_conf is a global
  vtss_trace_conf_set(VTSS_TRACE_GROUP_PHY, &vtss_appl_trace_conf);

  // In this case we only have one board. Assign point to the board definition
  board = &board_table[0];

  memset (&board_table[0], 0, (sizeof(vtss_appl_board_t) * MAX_BOARD_INSTANCES));

  printf ("//Default Setup Assumptions: \n" );
  printf ("//Board has Power Applied prior to Demo start \n" );
  // printf ("//The PHY Initialization Timing delays have been satisfied \n" );
  printf ("//Pwr Supply/Voltages Stable, Ref Clk Stable, Ref & PLL Stable \n\n" );

  // Initialize
  vtss_board_phy_init(board);

  // "Create" the board
  vtss_inst_get(board->target, &create);
  vtss_inst_create(&create, &board->inst);
  vtss_init_conf_get(board->inst, &init_conf);

  board->init.init_conf = &init_conf;

  if (board->board_init(argc, argv, board)) {
    T_E("Could not initialize board");
    return 1;
  } else {
    printf ("//Comment: Board being initialized\n");
  }

  if (vtss_init_conf_set(board->inst, &init_conf) == VTSS_RC_ERROR) {
        T_E("Could not initialize");
        return 1;
  }

  if (board->inst->init_conf.spi_32bit_read_write) {
      vtss_appl_malibu_spi_io_test(board->inst, port_no);
  }

  printf ("/* VSC8258EV + MCP2210 hidapi Sanity Checks Example Code */\r\n");
  mac_if = VTSS_PORT_INTERFACE_SFI; // vtss_api/include/vtss/api/types.h used in 10G?

  /* ****************************************************** */
  /* USER CONFIGURATION                                     */
  /* ****************************************************** */
  printf (" End of PHY Initialization \r\n");

  printf (" *************************************\n");
  printf (" Available Example Codes for Sanity Checks\r\n");
  printf (" *************************************\n");
  printf (" 1. Loopback Test for 1G CuSFP.\n");
  printf (" \n");
  printf (" \n");
  printf (" *************************************\n");
  while (1);

  return 0; // All done

} // end of main
