/*      INCLUDES                                                    */

// C STANDARD
#include <stdbool.h>                    // bool

// LUOS
#include "luos.h"                       // Luos_*

// CUSTOM
#include "app_container_generator.h"    // AppContainerGenerator_*
#include "rtb_builder.h"                // RTBBuilder_*

int main(void)
{
    Luos_Init();
    RTBBuilder_Init();
    AppContainerGenerator_Init();

    while(true)
    {
        Luos_Loop();
        RTBBuilder_Loop();
        AppContainerGenerator_Loop();
    }
}
