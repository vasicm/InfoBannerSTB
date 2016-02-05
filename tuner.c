#include "tuner.h"

#define DESIRED_FREQUENCY 818000000	/* Tune frequency in Hz */
#define BANDWIDTH 8    				/* Bandwidth in Mhz */
#define VIDEO_PID 101				/* Channel video pid */
#define AUDIO_PID 103				/* Channel audio pid */

// #define dbg printf("\nfunction:%s(%d)\n",__FUNCTION__,__LINE__)

#define ASSERT(x,y)  if(x)\
                        {\
                            printf("\n%s : %s\n", __FUNCTION__, y);\
                            return;\
                        }

static uint32_t tuneFrequency = 818000000;
static uint32_t bandwidth = 8;
static t_Module modul = DVB_T;

static pthread_cond_t statusCondition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t statusMutex = PTHREAD_MUTEX_INITIALIZER;

static struct timespec lockStatusWaitTime;
static struct timeval now;

static int32_t tunerStatusCallback(t_LockStatus status);

void setFrequency(uint32_t frequency) {
	tuneFrequency = frequency;
}

void setbBandwidth(uint32_t aBandwidth) {
	bandwidth = aBandwidth;
}

void setModul(t_Module aModul) {
	modul = aModul;
}

uint32_t getFrequency() {
	return tuneFrequency;
}

uint32_t getbBandwidth() {
	return bandwidth;
}

t_Module getModul() {
	return modul;
}

void initSTB() {

	gettimeofday(&now,NULL);
    lockStatusWaitTime.tv_sec = now.tv_sec+10;

	/*Initialize tuner device*/
    // if(Tuner_Init())
    // {
    //     printf("\n%s : ERROR Tuner_Init() fail\n", __FUNCTION__);
    //     return;
    // }
    ASSERT(Tuner_Init(),"ERROR Tuner_Init() fail")
    
    /* Register tuner status callback */
 //    if(Tuner_Register_Status_Callback(tunerStatusCallback))
 //    {
	// 	printf("\n%s : ERROR Tuner_Register_Status_Callback() fail\n", __FUNCTION__);
	// }
    ASSERT(Tuner_Register_Status_Callback(tunerStatusCallback), "ERROR Tuner_Register_Status_Callback() fail")
    
    /*Lock to frequency*/
    if(!Tuner_Lock_To_Frequency(tuneFrequency, bandwidth, modul))
    {
        printf("\n%s: INFO Tuner_Lock_To_Frequency(): %d Hz - success!\n",__FUNCTION__,DESIRED_FREQUENCY);
    }
    else
    {
        printf("\n%s: ERROR Tuner_Lock_To_Frequency(): %d Hz - fail!\n",__FUNCTION__,DESIRED_FREQUENCY);
        Tuner_Deinit();
        return;
    }
    
    /* Wait for tuner to lock*/
    if(ETIMEDOUT == pthread_cond_timedwait(&statusCondition, &statusMutex, &lockStatusWaitTime))
    {
        printf("\n%s:ERROR Lock timeout exceeded!\n",__FUNCTION__);
        Tuner_Deinit();
        return;
    }
    pthread_mutex_unlock(&statusMutex);
}

void deinitSTB() {
	Tuner_Deinit();
}

int32_t tunerStatusCallback(t_LockStatus status)
{
    if(status == STATUS_LOCKED)
    {
        pthread_mutex_lock(&statusMutex);
        pthread_cond_signal(&statusCondition);
        pthread_mutex_unlock(&statusMutex);
        printf("\n%s -----TUNER LOCKED-----\n",__FUNCTION__);
    }
    else
    {
        printf("\n%s -----TUNER NOT LOCKED-----\n",__FUNCTION__);
    }
    return 0;
}