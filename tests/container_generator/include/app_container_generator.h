#ifndef APP_CONTAINER_GENERATOR_H
#define APP_CONTAINER_GENERATOR_H

/*      INCLUDES                                                    */

// LUOS
#include "luos_list.h"  // VOID_MOD

/*      DEFINES                                                     */

// Container Generator container type and default alias.
#define APP_CONTAINER_GENERATOR_TYPE    VOID_MOD
#define APP_CONTAINER_GENERATOR_ALIAS   "app_container_generator"


// Initialize the button functionality.
void AppContainerGenerator_Init(void);

// On first loop runs a detection; then does nothing.
void AppContainerGenerator_Loop(void);

#endif /* ! APP_CONTAINER_GENERATOR_H */
