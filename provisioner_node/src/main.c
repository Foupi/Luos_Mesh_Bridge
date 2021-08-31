/*      INCLUDES                                                    */

// C STANDARD
#include <stdbool.h>                // bool

// LUOS
#include "luos.h"                   // Luos_*, msg_t

// CUSTOM
#include "app_provisioner_node.h"   // AppProvisionerNode_*
#include "mesh_provisioner.h"       // MeshProvisioner_*

int main(void)
{
    Luos_Init();
    MeshProvisioner_Init();
    AppProvisionerNode_Init();

    while(true)
    {
        Luos_Loop();
        MeshProvisioner_Loop();
        AppProvisionerNode_Loop();
    }
}
