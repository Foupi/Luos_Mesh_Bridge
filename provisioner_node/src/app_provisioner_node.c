#include "app_provisioner_node.h"

/*      INCLUDES                                                    */

// C STANDARD
#include <stddef.h>             // size_t
#include <stdint.h>             // uint*_t
#include <string.h>             // memset, str*

// NRF
#include "boards.h"             // BSP_BUTTON_0, BUTTON_PULL
#include "sdk_errors.h"         // ret_code_t

// NRF APPS
#include "app_button.h"         // app_button_*
#include "app_error.h"          // APP_ERROR_CHECK
#include "app_timer.h"          // APP_TIMER_TICKS

// LUOS
#include "luos.h"               // Luos_*
#include "routing_table.h"      // routing_table_t, RoutingTB_*

// CUSTOM
#include "mesh_provisioner.h"   // g_prov_scan_req

#ifdef DEBUG
#include "nrf_log.h"            // NRF_LOG_INFO
#endif /* DEBUG */

/*      STATIC VARIABLES & CONSTANTS                                */

// Buttons to press.
#define                 PROV_BUTTON_IDX     BSP_BUTTON_0

// Button detection delay.
static const uint32_t   BTN_DTX_DELAY       = APP_TIMER_TICKS(50);

#ifndef REV
#define REV {0,0,1}
#endif

// Static container instance for first detection and message sending.
static container_t*     s_app_instance;

// Describes if the first detection was run.
static bool             s_detection_performed       = false;

// Index of the Mesh Provisioner container.
static uint16_t         s_provisioner_container_id  = 0;

// Describes if the device is currently scanning or not.
static bool             s_prov_scan_state           = false;

/*      STATIC FUNCTIONS                                            */

// Find Mesh Provisioner container index in the routing table.
static void find_mesh_provisioner(void);

// Initializes the button functionality.
static void init_button(void);

/*      CALLBACKS                                                   */

// Logs message.
static void AppProvisionerNode_MsgHandler(container_t* container,
                                          msg_t* msg);

/* If a detection has been performed, the action is a button press and
** the index is the given one, sends a message to the provisioner
** container.
*/
static void prov_button_cb(uint8_t btn_idx, uint8_t event);

void AppProvisionerNode_Init(void)
{
    revision_t  revision = { .unmap = REV };

    init_button();

    s_app_instance = Luos_CreateContainer(AppProvisionerNode_MsgHandler,
                                          APP_PROVISIONER_NODE_TYPE,
                                          APP_PROVISIONER_NODE_ALIAS,
                                          revision);
}

void AppProvisionerNode_Loop(void)
{
    if (!s_detection_performed)
    {
        RoutingTB_DetectContainers(s_app_instance);

        find_mesh_provisioner();

        #ifdef DEBUG
        NRF_LOG_INFO("Detection complete: Mesh Provisioner index = %u!",
                     s_provisioner_container_id);
        #endif /* DEBUG */

        s_detection_performed = true;
    }
}

static void init_button(void)
{
    static app_button_cfg_t buttons[] =
    {
        {
            PROV_BUTTON_IDX,
            APP_BUTTON_ACTIVE_LOW,
            BUTTON_PULL,
            prov_button_cb,
        },
    };

    ret_code_t err_code = app_button_init(buttons, 1, BTN_DTX_DELAY);
    APP_ERROR_CHECK(err_code);

    app_button_enable();
}

static void find_mesh_provisioner(void)
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

        size_t  alias_len   = strlen(entry.alias);
        size_t  exp_len     = strlen(MESH_PROVISIONER_ALIAS);
        if (alias_len == exp_len)
        {
            int alias_neq   = strncmp(entry.alias,
                                      MESH_PROVISIONER_ALIAS, exp_len);
            if (!alias_neq)
            {
                s_provisioner_container_id  = entry.id;
                continue;
            }
        }
    }
}

static void prov_button_cb(uint8_t btn_idx, uint8_t event)
{
    if ((!s_detection_performed) || (event != APP_BUTTON_PUSH)
        || (btn_idx != PROV_BUTTON_IDX))
    {
        return;
    }

    s_prov_scan_state = !s_prov_scan_state;

    msg_t prov_scan_req;
    memset(&prov_scan_req, 0, sizeof(msg_t));
    prov_scan_req.header.target_mode    = ID;
    prov_scan_req.header.target         = s_provisioner_container_id;
    prov_scan_req.header.cmd            = IO_STATE;
    prov_scan_req.header.size           = sizeof(uint8_t);
    prov_scan_req.data[0]               = s_prov_scan_state;

    Luos_SendMsg(s_app_instance, &prov_scan_req);
}

static void AppProvisionerNode_MsgHandler(container_t* container,
                                          msg_t* msg)
{
    if (msg->header.cmd == IO_STATE)
    {
        if ((msg->header.size >= sizeof(uint8_t))
            && (msg->data[0] == 0x00))
        {
            #ifdef DEBUG
            NRF_LOG_INFO("Mesh Provisioner container unable to scan!");
            #endif /* DEBUG */

            s_prov_scan_state = false;
        }
    }
}
