/*      INCLUDES                                                    */

// C STANDARD
#include <stdbool.h>            // bool

// LUOS
#include "luos.h"               // Luos_*

// CUSTOM
#include "app_luos_list.h"      // MESH_BRIDGE_*
#include "app_actuator_node.h"  // AppActuatorNode_*
#include "led_toggler.h"        // LedToggler_*
#include "mesh_bridge.h"        // MeshBridge_*
#include "rtb_builder.h"        // RTBBuilder_*

int main(void)
{
    Luos_Init();
    LedToggler_Init();
    RTBBuilder_Init();
    MeshBridge_Init();
    AppActuatorNode_Init();

    while (true)
    {
        Luos_Loop();
        LedToggler_Loop();
        RTBBuilder_Loop();
        MeshBridge_Loop();
        AppActuatorNode_Loop();
    }
}
