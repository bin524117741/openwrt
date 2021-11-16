#ifndef KEY_MGMT_H
#define KEY_MGMT_H

#include <linux/input.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief register key button event handler.
 * 
 * @param func. callback function.
 */
void key_event_func(void (*func)(struct input_event *ev));

/**
 * @brief initiate key button management.
 * 
 * @retval   0: sucess
 * @retval  !0: failure
 */
int key_init(void);

#ifdef __cplusplus
}
#endif
#endif