/*      INCLUDES                                                    */

// C STANDARD
#include <stdbool.h>        // bool

// LUOS
#include "gate.h"           // Gate_*
#include "luos.h"           // Luos_*

// CUSTOM
#include "app_luos_list.h"  // LUOS_MESH_BRIDGE
#include "led_toggler.h"    // LedToggler_*
#include "mesh_bridge.h"    // MeshBridge_*

int main(void)
{
    Luos_Init();
    Gate_Init();
    MeshBridge_Init();
    LedToggler_Init();

    while (true)
    {
        Luos_Loop();
        Gate_Loop();
        MeshBridge_Loop();
        LedToggler_Loop();
    }
}
