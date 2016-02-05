#ifndef TIMER_HEADER
#define TIMER_HEADER

#include <time.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>

typedef struct _Timer {
    timer_t timerId;
    struct itimerspec timerSpec;
    struct itimerspec timerSpecOld;
    struct sigevent signalEvent;
    int32_t timerFlags;
}Timer;

void initTimer(Timer* timer, void (*function)(), int32_t seconds);
void startTimer(Timer* timer);
void removeTimer(Timer* timer);

#endif