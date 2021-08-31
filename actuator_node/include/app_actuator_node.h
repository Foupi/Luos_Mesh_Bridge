#ifndef APP_ACTUATOR_NODE_H
#define APP_ACTUATOR_NODE_H

/*      INCLUDES                                                    */

// LUOS
#include "luos_list.h"      // VOID_MOD

/*      DEFINES                                                     */

// Type and default alias for the App Actuator Node container.
#define APP_ACTUATOR_NODE_TYPE  VOID_MOD
#define APP_ACTUATOR_NODE_ALIAS "app_actuator"

// Initialize the button functionality.
void AppActuatorNode_Init(void);

// Triggers a detection on first loop; does nothing afterwards.
void AppActuatorNode_Loop(void);

#endif /* ! APP_ACTUATOR_NODE_H */
