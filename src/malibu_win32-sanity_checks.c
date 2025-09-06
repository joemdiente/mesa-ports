//***************************************************************************
//* Malibu for Windows 32 Sanity Check Example code
//* 
//* Author: Joemel John Diente <joemdiente@gmail.com>
//***************************************************************************

#include <vtss_api.h>   // For board initialization
#include <vtss_appl.h>  // For vtsss_board_t
#include <vtss_port_api.h>
#include <malibu_win32-sanity_checks.h> // For board init
#include "../base/ail/vtss_state.h" // For AIL dumping function
#include <vtss_phy_10g_api.h> 
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

void appl_sanity_check_phy_init(vtss_inst_t inst) {
    vtss_port_no_t port_no = 0;
    vtss_phy_10g_init_parm_t  init_parm;
    vtss_phy_10g_mode_t     oper_mode;  

    // Configure Only as 10GLAN
    printf ("Operating MODE for ALL Ports: 0=MODE_10GLAN\n");

    // This loop should always start from the physical Ch0 of the device
    for (port_no = 0; port_no < 4; port_no++) {
        printf ("\nConfiguring port %d\n", port_no);

        memset(&oper_mode, 0, sizeof(vtss_phy_10g_mode_t));

        // The following call pre-populates the PHY_INST with initization params
        init_parm.channel_conf = VTSS_CHANNEL_AUTO;
        if(vtss_phy_10g_init(inst, port_no, &init_parm) != VTSS_RC_OK) {
            T_E("vtss_phy_10g_init failed, port %d\n", port_no);
            printf("vtss_phy_10g_init failed, port %d\n", port_no);
        }

        // The mode_get is used to retrieve the pre-populated values from the PHY_INST
        if (vtss_phy_10g_mode_get(inst, port_no, &oper_mode) != VTSS_RC_OK) {
            T_E("vtss_phy_10g_mode_get failed, port %d\n", port_no);
            printf("vtss_phy_10g_mode_get failed, port %d\n", port_no);
        }

        // For 10GLAN
        {
            oper_mode.oper_mode = VTSS_PHY_LAN_MODE;
            oper_mode.interface = VTSS_PHY_SFI_XFI;
            oper_mode.channel_id = VTSS_CHANNEL_AUTO;

            // h_media/l_media:
            // --> VTSS_MEDIA_TYPE_SR2_SC (limiting SR/LR/ER/ZR limiting modules)
            // --> VTSS_MEDIA_TYPE_DAC_SC (Direct Attach Cu Cable)
            // --> VTSS_MEDIA_TYPE_KR_SC  (10GBASE-KR backplane)
            // --> VTSS_MEDIA_TYPE_ZR_SC  (linear ZR modules)

            // Assuming SR/LR limiting module on the line side and long traces (> 10 in)
            // or backplane on the host side
            oper_mode.h_media = VTSS_MEDIA_TYPE_KR_SC;
            oper_mode.l_media = VTSS_MEDIA_TYPE_SR2_SC;
            // oper_mode.l_media = VTSS_MEDIA_TYPE_KR_SC;
        }

        // Invert Polarity of Line/Host Tx/Rx
        oper_mode.polarity.line_rx = FALSE;
        oper_mode.polarity.line_tx = FALSE;
        oper_mode.polarity.host_rx = FALSE;
        oper_mode.polarity.host_tx = FALSE;
        // H/LREFCLK is_high_amp :
        // --> TRUE (1100mV to 2400mV diff swing)
        // --> FALSE (200mV to 1200mV diff swing)
        oper_mode.h_clk_src.is_high_amp = TRUE;
        oper_mode.l_clk_src.is_high_amp = TRUE;

        /* Setup Port */
        printf("vtss_phy_10g_mode_set: port %d, oper_mode = %d\n", port_no, oper_mode.oper_mode);

        if (vtss_phy_10g_mode_set(inst, port_no, &oper_mode) != VTSS_RC_OK) {
            T_E("vtss_phy_10g_mode_set failed, port %d\n", port_no);
            printf("vtss_phy_10g_mode_set failed, port %d\n", port_no);
        }
    }  // End of For loop looping through channels for basic configuration

    // Wait for the chip to stablize -- about 1 sec
    uint msec = 1000;
    struct timespec ts;						  \
    ts.tv_sec = msec / 1000;					  \
    ts.tv_nsec = (msec % 1000) * 1000000;				  \
    while(nanosleep(&ts, &ts) == -1 && errno == EINTR) {		  \
    }
}
void appl_sanity_check_read_i2c_sfp(vtss_inst_t inst, vtss_port_no_t port_no) {
    /* P1 SFP - ch2
     * GPIO16 = RSEL0
     * GPIO17 = MOD_DET
     * GPIO18 = SCK
     * GPIO19 = SDA
     * GPIO20 = TXDISABLE
     * GPIO21 = TXFAULT
     * GPIO22 = LOS
     * pull-up = RSEL1
     */

    if (port_no < 2 || port_no > 3) {
        printf(" Port Number is not an SFP port\r\n");
        return;
    }

    // Copied from mesa/phy_demo_appl/appl/vtss_appl_10g_phy_malibu.c
    vtss_gpio_10g_gpio_mode_t  gpio_mode;
    u16 gpio_no;
    BOOL value;
    {
        /* ********************************************************** */
        // GPIO Output functionality
        // GPIO_0 -> GPIO_7  This repeats for each of the 4 ports.
        // In this Example:
        // 0 = GPIO output 0 from channel 0   - CH0_RS0
        // 1 = GPIO output 1 from channel 0   - N/A
        // 2 = GPIO output 2 from channel 0   - CH0_SCL - I2C Master for SFP+
        // 3 = GPIO output 3 from channel 0   - CH0_SDA - I2C Master for SFP+
        // 4 = GPIO output 4 from channel 0   - CH0_TX_DIS
        // 5 = GPIO output 5 from channel 0   - N/A
        // 6 = GPIO output 6 from channel 0   - N/A
        // 7 = GPIO output 7 from channel 0   - CH0_LINK_UP
        //
        // 8 = GPIO output 0 from channel 1   - CH1_RS0
        // 9 = GPIO output 1 from channel 1   - N/A
        // 10 = GPIO output 2 from channel 1  - CH1_SCL - I2C Master for SFP+
        // 11 = GPIO output 3 from channel 1  - CH1_SDA - I2C Master for SFP+
        // 12 = GPIO output 4 from channel 1  - CH1_TX_DIS
        // 13 = GPIO output 5 from channel 1  - N/A
        // 14 = GPIO output 6 from channel 1  - N/A
        // 15 = GPIO output 7 from channel 1  - CH1_LINK_UP
        // .....
        /* ********************************************************** */
        u32 val32 = 0;  // <- this is not common
        /* ********************************************************** */
        // GPIO used: #0 for Ch0 (CH0_RS0), #8 for Ch1 and so on
        memset(&gpio_mode, 0, sizeof(vtss_gpio_10g_gpio_mode_t));
        gpio_mode.mode = VTSS_10G_PHY_GPIO_DRIVE_HIGH;
        gpio_mode.in_sig = VTSS_10G_GPIO_INTR_SGNL_NONE;
        gpio_no = 0 + (port_no*8);

        printf("\nMalibu GPIO Output: Driving HIGH configuration for port %d, gpio %d \n", port_no, gpio_no);
        if (vtss_phy_10g_gpio_mode_set(inst, port_no, gpio_no, &gpio_mode) != VTSS_RC_OK) {
        T_E("vtss_phy_10g_gpio_mode_set, port %d, gpio %d, mode: DRIVE_HIGH/LOW (RS0)\n", port_no, gpio_no);
        printf("Malibu Error setting GPIO Output configuration for port %d, gpio %d \n", port_no, gpio_no);
        }

        /* ********************************************************** */
        // GPIO used: #4 for Ch0 (CH0_TX_DIS), #12 for Ch1 and so on
        memset(&gpio_mode, 0, sizeof(vtss_gpio_10g_gpio_mode_t));
        gpio_mode.mode = VTSS_10G_PHY_GPIO_DRIVE_LOW;
        gpio_mode.in_sig = VTSS_10G_GPIO_INTR_SGNL_NONE;
        gpio_no = 4 + (port_no*8);

        printf("\nMalibu GPIO Output: Driving LOW configuration for port %d, gpio %d \n", port_no, gpio_no);
        if (vtss_phy_10g_gpio_mode_set(inst, port_no, gpio_no, &gpio_mode) != VTSS_RC_OK) {
        T_E("vtss_phy_10g_gpio_mode_set, port %d, gpio %d, mode: DRIVE_HIGH/LOW (TX_DISABLE)\n", port_no, gpio_no);
        printf("Malibu Error setting GPIO Output configuration for port %d, gpio %d \n", port_no, gpio_no);
        }
    }
    {
        // GPIO as I2C Master Interface for SFP+ I2C register readout
        // GPIO used: #2/#3 (Ch0_SCL and CH0_SDA resp), #10/#11 (Ch1_SCL and CH1_SDA resp) and so on
        //
        // Notes:
        // 1) This GPIO allocation is per channel (A Data/Clk pair per channel),
        // 2) Based on this allocation, the I2C read/write functions send the commands
        //    out on those GPIOs assigned to that particular channel.
        // 3) Slave_ID is fixed at 0x50 (as per the SFP+ Standard, SFF-8472).
        // 4) The two-wire serial master does not support read-increment instructions.

        // First, assign GPIOs as I2C Master for a particular channel
        printf("\nMalibu Reading SFP+ I2C registers port %d\n", port_no);
        /* ********************************************************** */
        memset(&gpio_mode, 0, sizeof(vtss_gpio_10g_gpio_mode_t));
        gpio_mode.mode = VTSS_10G_PHY_GPIO_OUT;
        gpio_mode.p_gpio = 0; // GPIO_INTR_CTRL (Dev:1 ch_reg):GPIO_INTR:GPIO0_OUT.SEL0
        gpio_mode.in_sig = VTSS_10G_GPIO_INTR_SGNL_I2C_MSTR_DATA_OUT;
        gpio_no = 3 + (port_no*8); // This is corrected from phy_demo_appl (old: gpio_no = 2 + (port_no*8); )

        printf("\nMalibu GPIO Output: I2C Master DATA configuration for port %d, gpio %d \n", port_no, gpio_no);
        if (vtss_phy_10g_gpio_mode_set(inst, port_no, gpio_no, &gpio_mode) != VTSS_RC_OK) {
        T_E("vtss_phy_10g_gpio_mode_set, port %d, gpio %d, mode: I2C_MSTR_DATA_OUT\n", port_no, gpio_no);
        printf(" Malibu Error setting GPIO Output configuration for port %d, gpio %d \n", port_no, gpio_no);
        }

        memset(&gpio_mode, 0, sizeof(vtss_gpio_10g_gpio_mode_t));
        gpio_mode.mode = VTSS_10G_PHY_GPIO_OUT;
        gpio_mode.p_gpio = 1; // GPIO_INTR_CTRL (Dev:1 ch_reg):GPIO_INTR:GPIO0_OUT.SEL0 
        //Joem Notes: what is difference between p_gpio and in_sig??? seems they are the same.
        //The internal comments hints, p_gpio is per channel gpio. need to check other examples.
        gpio_mode.in_sig = VTSS_10G_GPIO_INTR_SGNL_I2C_MSTR_CLK_OUT;
        gpio_no = 2 + (port_no*8); // This is corrected from phy_demo_appl (old: gpio_no = 3 + (port_no*8); )

        printf("\n Malibu GPIO Output: I2C Master CLK configuration for port %d, gpio %d \n", port_no, gpio_no);
        if (vtss_phy_10g_gpio_mode_set(inst, port_no, gpio_no, &gpio_mode) != VTSS_RC_OK) {
        T_E("vtss_phy_10g_gpio_mode_set, port %d, gpio %d, mode: I2C_MSTR_CLK_OUT\n", port_no, gpio_no);
        printf(" Malibu Error setting GPIO Output configuration for port %d, gpio %d \n", port_no, gpio_no);
        }
    }
}

void appl_sanity_check_read_i2c_address(vtss_inst_t inst, vtss_port_no_t port_no, u16 start_addr, u16 length) {
    u16 addr = 0x00;
    u16 data;
    u16 i = 0;
    char sfp_vendor[16];

    if (start_addr >= length) {
        printf(" length should be less than start_address\r\n");
        return;
    }

    for (addr = start_addr; addr < length; addr++) {
        if (vtss_phy_10g_i2c_read(inst, port_no, addr, &data) != VTSS_RC_OK) {
            T_E("vtss_phy_10g_i2c_read, port %d, address = 0x%X\n", port_no, addr);
            printf(" Malibu Error reading I2C register on SFP+ module for port %d\n", port_no);
        } else {
            printf(" Malibu reading I2C register @ addr = %d: value = 0x%X \n", addr, data);
        }
    }
    printf("\r\n");    
}
void appl_sanity_check_read_i2c_vendor(vtss_inst_t inst, vtss_port_no_t port_no) {
    u16 address = 0x0;
    u16 data;
    u16 i = 0;
    char sfp_vendor[16];

    // SFF Specifications SFP Vendor Name Address Range.
    for (address = 20; address < 36; address++) {
        if (vtss_phy_10g_i2c_read(inst, port_no, address, &data) != VTSS_RC_OK) {
            T_E("vtss_phy_10g_i2c_read, port %d, address = 0x%X\n", port_no, address);
            printf(" Malibu Error reading I2C register on SFP+ module for port %d, \n", port_no);
        } else {
            sfp_vendor[i++] = (char)data;
        }
    }
    printf(" SFP Vendor: %s \r\n", sfp_vendor);
    printf("\r\n");
}

void appl_sanity_check_cusfp_lb(vtss_inst_t inst, vtss_port_no_t port_no) {

    return;
}
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
  /* ********************************************************** */
  /* ***************    Basic Bring-up      ******************* */
  /* ********************************************************** */  
  printf("VTSS_PORT_NO_START = %d\n", VTSS_PORT_NO_START);
  printf("VTSS_PORTS = %d\n", VTSS_PORTS);

  printf("\nBoard Info --> \n");
  printf("Board name: %s\n", board->descr);
  printf("port_no = %d\n", port_no);
  printf("Board Target: 0x%X\n", board->target);

  printf("Board Inst: %p\n\n", board->inst);

  appl_sanity_check_phy_init(board->inst);

  printf (" End of PHY Initialization \r\n");

  /* ****************************************************** */
  /* USER CONFIGURATION                                     */
  /* ****************************************************** */
  while (1) {
    printf (" *************************************\n");
    printf (" Available Example Codes for Sanity Checks\r\n");
    printf (" *************************************\n");
    printf (" 1. Read SFP via I2C.\n");
    printf (" 2. Loopback Test for 1G CuSFP.\n");
    printf (" 3. \n");
    printf (" *************************************\n");
    printf (" Type the number of example to run, then press enter. Type \"exit\" to quit \n");

    rc = scanf("%s", &command[0]);

    /* ****************************************************** */
    if (strcmp(command, "1")  == 0) {
        system("clear");
        printf (" *************************************\n");
        printf (" Running example code #%s\r\n", command);
        printf (" *************************************\n");

        printf (" ********Configure SFP GPIO and I2C********\n");
        vtss_port_no_t sfp_port = 2;
        appl_sanity_check_sfp_config(board->inst, sfp_port);
        sfp_port = 3;
        appl_sanity_check_read_i2c_sfp(board->inst, sfp_port);

        printf (" ********Test Read I2C SFP Port 2 from Address 0-16********\n");
        appl_sanity_check_read_i2c_address(board->inst, 2, 0, 16);
        printf (" ********Test Read I2C SFP Port 3 from Address 0-16********\n");
        appl_sanity_check_read_i2c_address(board->inst, 3, 0, 16);

        printf (" ********Read Vendor for Port 2********\n");
        appl_sanity_check_read_i2c_vendor(board->inst, 2);
        printf (" ********Read Vendor for Port 3********\n");
        appl_sanity_check_read_i2c_vendor(board->inst, 3);

        printf (" *************************************\n");
        printf (" Done.\r\n");
        printf (" Press anything to go back\r\n");
        getch();
    }
    /* ****************************************************** */
    else if (strcmp(command, "2")  == 0) {
        system("clear");
        printf (" *************************************\n");
        printf (" Running example code #%s\r\n", command);
        printf (" *************************************\n");

        printf (" ********Cu SFP Loopback Test********\n");
        appl_sanity_check_cusfp_lb(board->inst, 2);
        
        printf (" *************************************\n");
        printf (" Done.\r\n");
        printf (" Press anything to go back\r\n");
        getch();
    }
    /* ****************************************************** */
    else if (strcmp(command, "3")  == 0) {
        system("clear");
        printf (" *************************************\n");
        printf (" command: %s not yet implemented\r\n", command);
        printf (" *************************************\n");
        
        // Add here code
        
        printf (" *************************************\n");
        printf (" Done.\r\n");
        printf (" Press anything to go back\r\n");
        getch();
    }
    /* ****************************************************** */
    else if (strcmp(command, "exit")  == 0) {
        break;
    }
  }

  return 0; // All done

} // end of main
