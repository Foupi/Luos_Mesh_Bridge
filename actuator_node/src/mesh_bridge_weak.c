// C STANDARD
#include <stdint.h> // uint32_t

// NRF
#include "boards.h" // bsp_board_led_off, bsp_board_led_on

/*  STATIC VARIABLES & CONSTANTS                                     */

// LED toggled to match provisioning/configuration state.
static const uint32_t   INIT_LED    = 1;

// LED toggled to match Ext-RTB procedure state.
static const uint32_t   EXT_RTB_LED = 2;

void indicate_provisioning_begin(void)
{
    bsp_board_led_on(INIT_LED);
}

void indicate_configuration_end(void)
{
    bsp_board_led_off(INIT_LED);
}

void indicate_ext_rtb_engaged(void)
{
    bsp_board_led_on(EXT_RTB_LED);
}

void indicate_ext_rtb_complete(void)
{
    bsp_board_led_off(EXT_RTB_LED);
}
