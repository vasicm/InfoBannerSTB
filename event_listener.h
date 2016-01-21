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

void initEventListener();
void deintiEventListener();
int32_t getKeys(int32_t count, uint8_t* buf, int32_t* eventsRead);

#endif
