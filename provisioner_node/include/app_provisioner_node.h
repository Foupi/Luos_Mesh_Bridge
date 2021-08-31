#ifndef APP_PROVISIONER_NODE_H
#define APP_PROVISIONER_NODE_H

/*      INCLUDES                                                    */

// LUOS
#include "luos_list.h"  // VOID_MOD

/*      DEFINES                                                     */

// Type and default alias for the App Provisioner Node container.
#define APP_PROVISIONER_NODE_TYPE   VOID_MOD
#define APP_PROVISIONER_NODE_ALIAS  "app_provisioner"

// Initialize the button functionality.
void AppProvisionerNode_Init(void);

// Triggers a detection on first loop; does nothing afterwards.
void AppProvisionerNode_Loop(void);

#endif /* ! APP_PROVISIONER_NODE_H */
