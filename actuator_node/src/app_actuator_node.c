#include "app_actuator_node.h"

/*      INCLUDES                                                    */

// C STANDARD
#include <stdbool.h>            // bool
#include <stddef.h>             // size_t
#include <string.h>             // str*

// NRF
#include "boards.h"             // BSP_BUTTON_0, BUTTON_PULL
#include "sdk_errors.h"         // ret_code_t

// NRF APPS
#include "app_button.h"         // app_button_*
#include "app_error.h"          // APP_ERROR_CHECK
#include "app_timer.h"          // APP_TIMER_TICKS

// LUOS
#include "luos.h"               // Luos_*

// CUSTOM
#include "led_toggler.h"        // LED_TOGGLER_ALIAS
#include "mesh_bridge.h"        // MeshBridge_*
#include "mesh_bridge_utils.h"  // find_mesh_bridge_container_id
#include "rtb_builder.h"        // RTBBuilder_*

#ifdef DEBUG
#include "nrf_log.h"            // NRF_LOG_INFO
#endif /* DEBUG */

/*      STATIC VARIABLES & CONSTANTS                                */

// Button to press.
#define                 DTX_BTN_IDX     BSP_BUTTON_0
#define                 GET_BTN_IDX     BSP_BUTTON_1
#define                 PRINT_BTN_IDX   BSP_BUTTON_2
#define                 LED1_BTN_IDX    BSP_BUTTON_3

// Button detection delay.
static const uint32_t   BTN_DTX_DELAY               = APP_TIMER_TICKS(50);

#ifndef REV
#define                 REV                         {0,0,1}
#endif

// Static container instance for first detection and message sending.
static container_t*     s_app_instance;

// Describes if the first detection was run.
static bool             s_detection_performed       = false;

// Alias for the first remote LED container.
static char*            REMOTE_LED_ALIAS            = "led_toggler1";

// Static IDs of the containers to interact with.
static uint16_t         s_rtb_builder_container_id  = 0;
static uint16_t         s_mesh_bridge_container_id  = 0;
static uint16_t         s_remote_led_container_id   = 0;

/*      STATIC FUNCTIONS                                            */

// Initializes the button functionality.
static void init_button(void);

// Updates the static IDs of the Mesh Bridge and RTB Builder containers.
static void update_static_ids(void);

// Updates the RTB Builder static ID.
static void find_rtb_builder(const routing_table_t* rtb,
                             uint16_t last_entry_index);

// Updates the remote LED Toggler static ID.
static void find_remote_led(const routing_table_t* rtb,
                            uint16_t last_entry_index);

/*      CALLBACKS                                                   */

// If the index is the defined one, sends a detection message.
static void dtx_btn_evt_handler(uint8_t btn_idx, uint8_t event);

// If the index is the defined one, sends a Ext-RTB command.
static void get_btn_evt_handler(uint8_t btn_idx, uint8_t event);

// If the index is the defined one, sends a tables print command.
static void print_btn_evt_handler(uint8_t btn_idx, uint8_t event);

/* Sends an IO message corresponding to the event to the remote LED
** Toggler container.
*/
static void led_btn_evt_handler(uint8_t btn_idx, uint8_t event);

// On detection complete or Ext-RTB procedure complete, prints RTB.
static void AppActuatorNode_MsgHandler(container_t* container,
                                       msg_t* msg);

void AppActuatorNode_Init(void)
{
    revision_t  revision = { .unmap = REV };

    init_button();

    s_app_instance = Luos_CreateContainer(AppActuatorNode_MsgHandler,
                                          APP_ACTUATOR_NODE_TYPE,
                                          APP_ACTUATOR_NODE_ALIAS,
                                          revision);
}

void AppActuatorNode_Loop(void)
{
    if (!s_detection_performed)
    {
        RoutingTB_DetectContainers(s_app_instance);

        update_static_ids();

        #ifdef DEBUG
        NRF_LOG_INFO("Detection complete: RTB Builder ID = %u, Mesh Bridge ID = %u!",
                     s_rtb_builder_container_id,
                     s_mesh_bridge_container_id);
        #endif /* DEBUG */

        msg_t   local_fill_req;
        memset(&local_fill_req, 0, sizeof(msg_t));
        local_fill_req.header.target_mode   = ID;
        local_fill_req.header.target        = s_mesh_bridge_container_id;
        local_fill_req.header.cmd           = MESH_BRIDGE_FILL_LOCAL_CONTAINER_TABLE;

        Luos_SendMsg(s_app_instance, &local_fill_req);

        s_detection_performed = true;
    }
}

static void init_button(void)
{
    static app_button_cfg_t buttons[] =
    {
        #ifndef DEMO
        {
            DTX_BTN_IDX,
            APP_BUTTON_ACTIVE_LOW,
            BUTTON_PULL,
            dtx_btn_evt_handler,
        },
        {
            GET_BTN_IDX,
            APP_BUTTON_ACTIVE_LOW,
            BUTTON_PULL,
            get_btn_evt_handler,
        },
        {
            PRINT_BTN_IDX,
            APP_BUTTON_ACTIVE_LOW,
            BUTTON_PULL,
            print_btn_evt_handler,
        },
        #endif /* ! DEMO */
        {
            LED1_BTN_IDX,
            APP_BUTTON_ACTIVE_LOW,
            BUTTON_PULL,
            led_btn_evt_handler,
        },
    };

    uint16_t btn_arr_size   = sizeof(buttons) / sizeof(app_button_cfg_t);

    ret_code_t err_code = app_button_init(buttons, btn_arr_size,
                                          BTN_DTX_DELAY);
    APP_ERROR_CHECK(err_code);

    app_button_enable();
}

static void update_static_ids(void)
{
    routing_table_t*    rtb                 = RoutingTB_Get();
    uint16_t            last_entry_index    = RoutingTB_GetLastEntry();

    s_mesh_bridge_container_id = find_mesh_bridge_container_id(rtb, last_entry_index);

    find_rtb_builder(rtb, last_entry_index);
    find_remote_led(rtb, last_entry_index);
}

static void find_rtb_builder(const routing_table_t* rtb,
                             uint16_t last_entry_index)
{
    for (uint16_t entry_idx = 0; entry_idx < last_entry_index;
         entry_idx++)
    {
        routing_table_t entry       = rtb[entry_idx];

        if (entry.mode == CONTAINER && entry.type == RTB_MOD)
        {
            s_rtb_builder_container_id = entry.id;
            break;
        }
    }
}

static void find_remote_led(const routing_table_t* rtb,
                            uint16_t last_entry_index)
{
    // Find ID in routing table.
    uint16_t    remote_led_id   = RoutingTB_IDFromAlias(
                                    REMOTE_LED_ALIAS
                                  );

    if (remote_led_id == 0xFFFF)
    {
        s_remote_led_container_id   = 0;
    }
    else
    {
        s_remote_led_container_id   = remote_led_id;
    }
}

static void dtx_btn_evt_handler(uint8_t btn_idx, uint8_t event)
{
    if ((btn_idx != DTX_BTN_IDX) || (event != APP_BUTTON_PUSH))
    {
        return;
    }

    msg_t   update;
    memset(&update, 0, sizeof(msg_t));
    update.header.target_mode   = ID;
    update.header.target        = s_mesh_bridge_container_id;
    update.header.cmd           = MESH_BRIDGE_UPDATE_INTERNAL_TABLES;
    update.header.size          = sizeof(uint16_t);
    memcpy(update.data, &s_rtb_builder_container_id, sizeof(uint16_t));

    Luos_SendMsg(s_app_instance, &update);
}

static void get_btn_evt_handler(uint8_t btn_idx, uint8_t event)
{
    if ((btn_idx != GET_BTN_IDX) || (event != APP_BUTTON_PUSH))
    {
        return;
    }

    msg_t ext_rtb_req;
    memset(&ext_rtb_req, 0, sizeof(msg_t));
    ext_rtb_req.header.target_mode  = ID;
    ext_rtb_req.header.target       = s_mesh_bridge_container_id;
    ext_rtb_req.header.cmd          = MESH_BRIDGE_EXT_RTB_CMD;

    Luos_SendMsg(s_app_instance, &ext_rtb_req);
}

static void print_btn_evt_handler(uint8_t btn_idx, uint8_t event)
{
    if ((btn_idx != PRINT_BTN_IDX) || (event != APP_BUTTON_PUSH))
    {
        return;
    }

    msg_t   print_req;
    memset(&print_req, 0, sizeof(print_req));
    print_req.header.target_mode    = ID;
    print_req.header.target         = s_mesh_bridge_container_id;
    print_req.header.cmd            = MESH_BRIDGE_PRINT_INTERNAL_TABLES;

    Luos_SendMsg(s_app_instance, &print_req);
}

static void led_btn_evt_handler(uint8_t btn_idx, uint8_t event)
{
    if (btn_idx != LED1_BTN_IDX)
    {
        return;
    }

    uint8_t     new_state;
    if (event == APP_BUTTON_PUSH)
    {
        new_state = 0x01;
    }
    else
    {
        new_state = 0x00;
    }

    if (s_remote_led_container_id == 0)
    {
        #ifdef DEBUG
        NRF_LOG_INFO("Remote LED Toggler not detected yet!");
        #endif /* DEBUG */

        return;
    }

    msg_t   led_state_msg;
    memset(&led_state_msg, 0, sizeof(msg_t));
    led_state_msg.header.target_mode    = ID;
    led_state_msg.header.target         = s_remote_led_container_id;
    led_state_msg.header.cmd            = IO_STATE;
    led_state_msg.header.size           = sizeof(uint8_t);
    led_state_msg.data[0]               = new_state;

    Luos_SendMsg(s_app_instance, &led_state_msg);
}

static void AppActuatorNode_MsgHandler(container_t* container,
                                       msg_t* msg)
{
    switch (msg->header.cmd)
    {
    case RTB_COMPLETE:
    {
        update_static_ids();

        msg_t   print_req;
        memset(&print_req, 0, sizeof(msg_t));
        print_req.header.target_mode    = ID;
        print_req.header.target         = s_rtb_builder_container_id;
        print_req.header.cmd            = RTB_PRINT;

        Luos_SendMsg(s_app_instance, &print_req);
    }
        break;

    case MESH_BRIDGE_EXT_RTB_COMPLETE:
    {
        #ifdef DEBUG
        NRF_LOG_INFO("Ext-RTB procedure complete!");
        #endif /* DEBUG */

        update_static_ids();

        msg_t   print_req;
        memset(&print_req, 0, sizeof(msg_t));
        print_req.header.target_mode    = ID;
        print_req.header.target         = s_rtb_builder_container_id;
        print_req.header.cmd            = RTB_PRINT;

        Luos_SendMsg(s_app_instance, &print_req);
    }
        break;

    case MESH_BRIDGE_INTERNAL_TABLES_CLEARED:
        #ifdef DEBUG
        NRF_LOG_INFO("Mesh Bridge internal tables cleared!");
        #endif /* DEBUG */

        break;

    case MESH_BRIDGE_LOCAL_CONTAINER_TABLE_FILLED:
    {
        uint16_t nb_local_containers;
        memcpy(&nb_local_containers, msg->data, sizeof(uint16_t));

        #ifdef DEBUG
        NRF_LOG_INFO("Mesh Bridge local container table filled with %u entries!",
                     nb_local_containers);
        #endif /* DEBUG */
    }
        break;

    case MESH_BRIDGE_INTERNAL_TABLES_UPDATED:
    {
        #ifdef DEBUG
        NRF_LOG_INFO("Mesh Bridge internal tables updated!");
        #endif /* DEBUG */

        // When internal IDs have been updated, detection can be run.

        msg_t   rtb_build_req;
        memset(&rtb_build_req, 0, sizeof(msg_t));
        rtb_build_req.header.target_mode    = ID;
        rtb_build_req.header.target         = s_rtb_builder_container_id;
        rtb_build_req.header.cmd            = RTB_BUILD;

        Luos_SendMsg(s_app_instance, &rtb_build_req);
    }
        break;

    default:
        break;
    }
}
