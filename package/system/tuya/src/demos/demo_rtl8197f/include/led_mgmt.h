#ifndef LED_MGMT_H
#define LED_MGMT_H

#include <stdbool.h>
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LED_OFF = 0,
    LED_ON,
    LED_BLINK,
} led_state_t;

/**
 * @brief initiate led management.
 * 
 * @retval   0: sucess
 * @retval  !0: failure
 */
int led_init(void);

/**
 * @brief destroy led mangement.
 */
void led_destroy(void);

/**
 * @brief create a led event.
 * 
 * @param name. unique identifier.
 * @param func. callback function to set led on or off.
 * 
 * @retval   0: sucess
 * @retval  !0: failure
 */
int led_create(const char *name, void (*func)(bool on));

/**
 * @brief configure led.
 * 
 * @param name. unique identifier which is match with `led_create`.
 * @param state. configure led to on/off/blink state.
 * @param interval. blink interval time when state is LED_BLINK. (unit: msec.)
 * @param expire.   blink expire time when state is LED_BLINK. (unit: msec.)
 */
void led_set(const char *name, led_state_t state, int interval, int expire);

#ifdef __cplusplus
}
#endif
#endif