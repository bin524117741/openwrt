#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "led_mgmt.h"

#define LED_WAIT_TIME 100

typedef struct {
    char *name;
    struct list_head list;
    int interval_ms;
    int expire_ms;
    int total_ms;
    int current_ms;
    bool running;
    bool reverse;
    led_state_t state;
    void (*set)(bool on);
} led_mgmt_s;

static int inited = false;

static pthread_t led_tid;
static struct list_head led_list;

static void led_on_process(led_mgmt_s *led)
{
    if (led == NULL)
        return;

    led->set(true);
    led->running = false;
}

static void led_off_process(led_mgmt_s *led)
{
    if (led == NULL)
        return;

    led->set(false);
    led->running = false;
}

static void led_blink_process(led_mgmt_s *led)
{
    if (led == NULL)
        return;

    led->current_ms += LED_WAIT_TIME;
    led->total_ms += LED_WAIT_TIME;

    /* expired */
    if ((led->expire_ms != 0) && (led->total_ms >= led->expire_ms)) {
        led->set(false);
        led->running = false;
        return;
    }

    if (led->current_ms > led->interval_ms) {
        if (led->reverse) {
            led->set(false);
        } else {
            led->set(true);
        }

        led->current_ms = 0;
        led->reverse ^= 1;
    }
}

static void led_process(void)
{
    led_mgmt_s *led = NULL;
    struct list_head *pos = NULL;

    list_for_each(pos, &led_list) {
        led = list_entry(pos, led_mgmt_s, list);
        if (!led->running)
            continue;

        switch (led->state) {
        case LED_ON:
            led_on_process(led);
            break;
        case LED_OFF:
            led_off_process(led);
            break;
        case LED_BLINK:
            led_blink_process(led);
            break;
        default:
            return;
        }
    }
}

static void *led_run(void *args)
{
    while (1) {
        usleep(LED_WAIT_TIME*1000); /* LED_WAIT_TIME msec */
        led_process();
    }
}

static bool len_existed(const char *name)
{
    bool existed = false;
    led_mgmt_s *led = NULL;
    struct list_head *pos = NULL;

    list_for_each(pos, &led_list) {
        led = list_entry(pos, led_mgmt_s, list);
        if (!strcmp(led->name, name)) {
            existed = true;
            break;
        }
    }

    return existed;
}

int led_init(void)
{
    int ret = 0;
    pthread_attr_t attr;

    if (inited)
        return -1;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    ret = pthread_create(&led_tid, &attr, led_run, NULL);
    pthread_attr_destroy(&attr);

    if (ret != 0)
        return -1;

    INIT_LIST_HEAD(&led_list);

    inited = true;

    return 0;
}

void led_destroy(void)
{
    struct list_head *pos = NULL, *p = NULL;
    led_mgmt_s *led = NULL;

    pthread_cancel(led_tid);

    list_for_each_safe(pos, p, &led_list) {
        led = list_entry(pos, led_mgmt_s, list);
        list_del(&(led->list));
        if (led != NULL)
            free(led);
    }

    inited = false;
}

int led_create(const char *name, void (*func)(bool on))
{
    led_mgmt_s *led = NULL;

    if (!inited)
        return -1;

    if ((name == NULL) || (func == NULL)) {
        return -1;
    }

    if (len_existed(name))
        return 0;

    led = (led_mgmt_s *)calloc(1, sizeof(led_mgmt_s));
    if (led == NULL)
        return -1;

    led->name = strdup(name);
    led->set = func;
    led->running = 0;

    list_add_tail(&(led->list), &led_list);

    return 0;
}

void led_set(const char *name, led_state_t state, int interval, int expire)
{
    led_mgmt_s *led = NULL;
    struct list_head *pos = NULL;

    if (!inited)
        return;        

    if (name == NULL)
        return;

    list_for_each(pos, &led_list) {
        led = list_entry(pos, led_mgmt_s, list);
        if (!strcmp(led->name, name)) {
            led->state = state;
            led->interval_ms = interval;
            led->expire_ms = expire;
            led->total_ms = 0;
            led->current_ms = 0;
            led->running = true;
            led->reverse = 0;

            break;
        }
    }
}