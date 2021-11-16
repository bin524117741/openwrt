#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "key_mgmt.h"

#define MAX_DEVICES 32

#define BITS_PER_LONG (sizeof(unsigned long) * 8)
#define BITS_TO_LONGS(x) (((x) + BITS_PER_LONG - 1) / BITS_PER_LONG)

static unsigned int device_count = 0;
static int epoll_fd = 0;
static int device_fds[MAX_DEVICES] = {0};

void (*key_event_cb)(struct input_event *ev);

static int test_bit(size_t bit, unsigned long* array)
{
    return (array[bit/BITS_PER_LONG] & (1UL << (bit % BITS_PER_LONG))) != 0;
}

static void *event_loop(void *args)
{
    int i = 0, event_count = 0;
    ssize_t rbytes = 0;
    struct input_event ev;
    struct epoll_event poll_events[MAX_DEVICES];

    while (1) {
        event_count = epoll_wait(epoll_fd, poll_events, MAX_DEVICES, -1);
        if (event_count <= 0)
            continue;

        for (i = 0; i < event_count; i++) {
            if (poll_events[i].events & EPOLLIN) {
                memset(&ev, 0, sizeof(struct input_event));
                rbytes = read(poll_events[i].data.fd, &ev, sizeof(struct input_event));
                if ((rbytes == sizeof(struct input_event)) && (ev.type == EV_KEY)) {
                    if (key_event_cb != NULL)
                        key_event_cb(&ev);
                }
            }
        }
    }
}

void key_event_func(void (*func)(struct input_event *ev))
{
    key_event_cb = func;
}

int key_init(void)
{
    int fd = 0;
    int ret = 0;
    DIR *dir = NULL;
    struct dirent *de = NULL;
    pthread_t tid = 0;
    pthread_attr_t attr;
    unsigned long ev_bits[BITS_TO_LONGS(EV_MAX)];

    if (epoll_fd > 0)
        return 0;

    epoll_fd = epoll_create(MAX_DEVICES);
    if (epoll_fd < 0)
        return -1;

    dir = opendir("/dev/input");
    if (dir != NULL) {
        while ((de = readdir(dir)) != NULL) {
            struct epoll_event ev;

            if (strncmp(de->d_name, "event", 5))
                continue;

            fd = openat(dirfd(dir), de->d_name, O_RDONLY);
            if (fd < 0)
                continue;

            memset(ev_bits, 0, sizeof(ev_bits));
            /* get the bit field of available event types, such as EV_KEY/EV_REL/EV_ABS... */
            if (ioctl(fd, EVIOCGBIT(0, sizeof(ev_bits)), &ev_bits) < 0) {
                close(fd);
                continue;
            }

            /* only care EV_KEY event types in here */
            if (!test_bit(EV_KEY, ev_bits)) {
                close(fd);
                continue;
            }

            ev.events = EPOLLIN;
            ev.data.fd = fd;

            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
                close(fd);
                continue;
            }

            if (device_count >= MAX_DEVICES) {
                close(fd);
                break;
            }

            device_fds[device_count] = fd;
            device_count += 1;
        }

        closedir(dir);
    }

    if (device_count == 0) {
        goto err;
    }

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    ret = pthread_create(&tid, &attr, event_loop, NULL);
    pthread_attr_destroy(&attr);
    if (ret != 0) {
        goto err;
    }

    return 0;

err:
    close(epoll_fd);
    epoll_fd = -1;
    return -1;
}
