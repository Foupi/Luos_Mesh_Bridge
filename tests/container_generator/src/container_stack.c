#include "container_stack.h"

/*      INCLUDES                                                    */

// C STANDARD
#include <stdint.h>     // uint16_t

// NRF
#include "nrf_log.h"    // NRF_LOG_INFO

// LUOS
#include "luos.h"       // Luos_*, container_t, msg_t

/*      STATIC VARIABLES & CONSTANTS                                */

// The stack of containers.
static container_t*     s_container_stack[CONTAINER_STACK_SIZE] = { 0 };

// Index in the stack.
static uint16_t         s_stack_index                           = 0;

// Dummy container type and alias.
static const uint8_t    DUMMY_TYPE                              = VOID_MOD;
static const char       DUMMY_ALIAS[]                           = "dummy";

#ifndef REV
#define REV {0,0,1}
#endif

/*      CALLBACKS                                                   */

// Logs a small message.
static void Dummy_MsgHandler(container_t* container, msg_t* msg);

void container_stack_push(void)
{
    if (s_stack_index >= CONTAINER_STACK_SIZE)
    {
        NRF_LOG_INFO("Container stack full!");

        return;
    }

    revision_t revision = {.unmap = REV};
    container_t* dummy = Luos_CreateContainer(Dummy_MsgHandler,
        DUMMY_TYPE, DUMMY_ALIAS, revision);

    s_container_stack[s_stack_index] = dummy;
    s_stack_index++;
}

void container_stack_pop(void)
{
    if (s_stack_index == 0)
    {
        NRF_LOG_INFO("Container stack empty!");

        return;
    }

    s_stack_index--;

    container_t* popped = s_container_stack[s_stack_index];
    Luos_DestroyContainer(popped);
}

static void Dummy_MsgHandler(container_t* container, msg_t* msg)
{
    NRF_LOG_INFO("Container %s received a message of type %u!",
                 (char*)(container->alias), msg->header.cmd);
}
