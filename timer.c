#include "timer.h"

void initTimer(Timer* timer, void (*function)(), int32_t seconds)
{
    timer->signalEvent.sigev_notify = SIGEV_THREAD;
    timer->signalEvent.sigev_notify_function = function;
    timer->signalEvent.sigev_value.sival_ptr = NULL;
    timer->signalEvent.sigev_notify_attributes = NULL;
    timer_create (CLOCK_REALTIME, &timer->signalEvent, &timer->timerId);
    timer->timerFlags = 0;
    memset (&timer->timerSpec, 0, sizeof(timer->timerSpec));
    
    timer->timerSpec.it_value.tv_sec = seconds;
    timer->timerSpec.it_value.tv_nsec = 0;
}

void startTimer(Timer* timer)
{
    timer_settime (timer->timerId, timer->timerFlags, &timer->timerSpec, &timer->timerSpecOld);
}

void  removeTimer(Timer* timer)
{
    memset (&timer->timerSpec, 0, sizeof (timer->timerSpec));
    timer_settime (timer->timerId, 0, &timer->timerSpec, &timer->timerSpecOld);
    timer_delete (timer->timerId);
}