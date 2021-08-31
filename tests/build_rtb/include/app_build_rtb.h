#ifndef APP_BUILD_RTB_H
#define APP_BUILD_RTB_H

/*      INCLUDES                                                    */

// LUOS
#include "luos_list.h"  // VOID_MOD

/*      DEFINES                                                     */

// Type of the App Build RTB container.
#define APP_BUILD_RTB_TYPE  VOID_MOD

// Default alias for the App Build RTB container.
#define APP_BUILD_RTB_ALIAS "app_build_rtb"

// Initialize the button functionality.
void AppBuildRTB_Init(void);

// On first loop runs a detection; then does nothing.
void AppBuildRTB_Loop(void);

#endif /* ! APP_BUILD_RTB_H */
