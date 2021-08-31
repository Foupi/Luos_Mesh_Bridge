/*      INCLUDES                                                    */

// C STANDARD
#include <stdbool.h>        // bool

// LUOS
#include "luos.h"           // Luos_*

// CUSTOM
#include "app_build_rtb.h"  // AppBuildRTB_*
#include "rtb_builder.h"    // RTBBuilder_*

int main(void)
{
    Luos_Init();
    RTBBuilder_Init();
    AppBuildRTB_Init();

    while(true)
    {
        Luos_Loop();
        RTBBuilder_Loop();
        AppBuildRTB_Loop();
    }
}
