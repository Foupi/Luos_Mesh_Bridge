#include "app_build_rtb.h"

/*      INCLUDES                                                    */

// C STANDARD
#include <stdint.h>         // uint*_t
#include <string.h>         // memset

// NRF
#include "boards.h"         // BSP_BUTTON_0, BUTTON_PULL
#include "nrf_log.h"        // NRF_LOG_INFO
#include "sdk_errors.h"     // ret_code_t

// NRF APPS
#include "app_button.h"     // app_button_*
#include "app_error.h"      // APP_ERROR_CHECK
#include "app_timer.h"      // APP_TIMER_TICKS

// LUOS
#include "luos.h"           // Luos_*
#include "routing_table.h"  // RoutingTB_*

// CUSTOM
#include "rtb_builder.h"    // RTB_*

/*      STATIC VARIABLES & CONSTANTS                                */

// Button to press.
#define                 BUTTON_IDX                  BSP_BUTTON_0

// Button detection delay.
static const uint32_t   BTN_DTX_DELAY               = APP_TIMER_TICKS(50);

#ifndef REV
#define REV {0,0,1}
#endif

// Static container instance for first detection and message sending.
static container_t*     s_app_instance;

// Describes if the first detection was run.
static bool             s_detection_performed       = false;

// Index of the RTB Builder container.
static uint16_t         s_rtb_builder_container_id  = 0;

/*      STATIC FUNCTIONS                                            */

// Initializes the button functionality.
static void init_button(void);

// Find RTB Builder container index in the routing table.
static void find_rtb_builder(void);

/*      CALLBACKS                                                   */

// If the index is the defined one, toggles the detection boolean.
static void button_evt_handler(uint8_t btn_idx, uint8_t event);

// When receiving RTB Complete message, update static ID and ask dump.
static void AppBuildRTB_MsgHandler(container_t* container, msg_t* msg);

void AppBuildRTB_Init(void)
{
    revision_t  revision = { .unmap = REV };

    init_button();

    s_app_instance = Luos_CreateContainer(AppBuildRTB_MsgHandler,
                                          APP_BUILD_RTB_TYPE,
                                          APP_BUILD_RTB_ALIAS,
                                          revision);
}

void AppBuildRTB_Loop(void)
{
    if (!s_detection_performed)
    {
        RoutingTB_DetectContainers(s_app_instance);

        find_rtb_builder();

        NRF_LOG_INFO("Detection complete: RTB Builder index = %u!",
                     s_rtb_builder_container_id);

        msg_t   print_req;
        memset(&print_req, 0, sizeof(msg_t));
        print_req.header.target_mode    = ID;
        print_req.header.target         = s_rtb_builder_container_id;
        print_req.header.cmd            = RTB_PRINT;

        Luos_SendMsg(s_app_instance, &print_req);

        s_detection_performed = true;
    }
}

static void init_button(void)
{
    static app_button_cfg_t buttons[] =
    {
        {
            BUTTON_IDX,
            APP_BUTTON_ACTIVE_LOW,
            BUTTON_PULL,
            button_evt_handler,
        },
    };

    ret_code_t err_code = app_button_init(buttons, 1, BTN_DTX_DELAY);
    APP_ERROR_CHECK(err_code);

    app_button_enable();
}

static void find_rtb_builder(void)
{
    routing_table_t*    rtb                 = RoutingTB_Get();
    uint16_t            last_entry_index    = RoutingTB_GetLastEntry();

    for (uint16_t entry_idx = 0; entry_idx < last_entry_index;
         entry_idx++)
    {
        routing_table_t entry       = rtb[entry_idx];

        if (entry.mode != CONTAINER)
        {
            // Only container entries have aliases.
            continue;
        }

        if (entry.type == RTB_MOD)
        {
            s_rtb_builder_container_id  = entry.id;
            return;
        }
    }
}

static void button_evt_handler(uint8_t btn_idx, uint8_t event)
{
    if (btn_idx != BUTTON_IDX)
    {
        return;
    }

    if (event == APP_BUTTON_PUSH)
    {
        msg_t   rtb_build_req;
        memset(&rtb_build_req, 0, sizeof(msg_t));
        rtb_build_req.header.target_mode    = ID;
        rtb_build_req.header.target         = s_rtb_builder_container_id;
        rtb_build_req.header.cmd            = RTB_BUILD;

        Luos_SendMsg(s_app_instance, &rtb_build_req);
    }
}

static void AppBuildRTB_MsgHandler(container_t* container, msg_t* msg)
{
    if (msg->header.cmd == RTB_COMPLETE)
    {
        find_rtb_builder();

        msg_t   print_req;
        memset(&print_req, 0, sizeof(msg_t));
        print_req.header.target_mode    = ID;
        print_req.header.target         = s_rtb_builder_container_id;
        print_req.header.cmd            = RTB_PRINT;

        Luos_SendMsg(s_app_instance, &print_req);
    }
}
