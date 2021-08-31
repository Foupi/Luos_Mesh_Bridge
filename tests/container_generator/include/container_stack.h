#ifndef CONTAINER_STACK_H
#define CONTAINER_STACK_H

/*      DEFINES                                                     */

#define CONTAINER_STACK_SIZE    8

// Creates a new container and pushes it in the stack.
void container_stack_push(void);

// Destroys the last container in the stack.
void container_stack_pop(void);

#endif /* CONTAINER_STACK_H */
