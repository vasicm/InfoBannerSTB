#ifndef EVENT_LISTENER_HEADER
#define EVENT_LISTENER_HEADER

#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>

typedef enum _KEY
{
    KEY_NUM_1 = 2,
    KEY_NUM_2 = 3,
    KEY_NUM_3 = 4,
    KEY_NUM_4 = 5,
    KEY_NUM_5 = 6,
    KEY_NUM_6 = 7,
    KEY_NUM_7 = 8,
    KEY_NUM_8 = 9,
    KEY_NUM_9 = 10,
    KEY_NUM_0 = 11,
    KEY_VOLUME_UP = 63,
    KEY_VOLUME_DOWN = 64,
    KEY_CHANNEL_UP = 62,
    KEY_CHANNEL_DOWN = 61,
    KEY_INFO_BTN = 358,
    KEY_EXIT_BTN = 102,
    KEY_MUTE_BTN = 60
} KEY;

void initEventListener();
void deintiEventListener();
int32_t getKeys(int32_t count, uint8_t* buf, int32_t* eventsRead);

#endif
