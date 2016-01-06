// telnet 192.168.22.74
// mount -o port=2049,nolock,proto=tcp -t nfs 192.168.20.197:/home/student/pputvios1/ploca /mnt/PPUTVIOS_Student1
#include "tdp_api.h"

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

#include "event_listener.h"
#include "tables_parser.h"
#include "tuner.h"
#include "lists.h"

#define NUM_EVENTS  5

#define NON_STOP    1

/* error codes */
#define NO_ERROR 		0
#define ERROR			1

#define KEY_CHANNEL_UP             62
#define KEY_CHANNEL_DOWN           61
#define KEY_MUTE_                   60
#define KEY_VOLUME_UP              63
#define KEY_VOLUME_DOWN            64
#define KEY_EXIT_                  102
#define KEY_INFO_                  358

#define ASSERT_TDP_RESULT(x,y)  if(NO_ERROR == x) \
                                    printf("%s success\n", y); \
                                else{ \
                                    printf("%s fail\n", y); \
                                    return -1; \
                                }


static pthread_cond_t statusCondition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t statusMutex = PTHREAD_MUTEX_INITIALIZER;


static int32_t inputFileDesc;
    
int32_t getKeys(int32_t count, uint8_t* buf, int32_t* eventsRead);

void *init()
{
    const char* dev = "/dev/input/event0";
    char deviceName[20];
    struct input_event* eventBuf;
    uint32_t eventCnt;
    uint32_t i;
    uint32_t count=0;
    int delay;
    int p = 0;
    int pp = 0;
    
    inputFileDesc = open(dev, O_RDWR);
    if(inputFileDesc == -1)
    {
        printf("Error while opening device (%s) !", strerror(errno));
            return ;
    }
    
    ioctl(inputFileDesc, EVIOCGNAME(sizeof(deviceName)), deviceName);
        printf("RC device opened succesfully [%s]\n", deviceName);
    
    eventBuf = malloc(NUM_EVENTS * sizeof(struct input_event));
    if(!eventBuf)
    {
        printf("Error allocating memory !");
        return ;
    }

    while(NON_STOP)
    {
        /* read input eventS */
        if(getKeys(NUM_EVENTS, (uint8_t*)eventBuf, &eventCnt))
        {
                        printf("Error while reading input events !");
                        return ;
                }

        for(i = 0; i < eventCnt; i++)
        {
            if(eventBuf[i].value == 1) {
                pp = p;
                p = (int)eventBuf[i].time.tv_sec;
                
                if((p - pp) > 0){
                    printf("\n");
                    //printf("time = %d\n", p - pp);
                }
                
                if(eventBuf[i].code == KEY_VOLUME_UP) {
                    count++;
                }else if(eventBuf[i].code == KEY_VOLUME_DOWN) {
                    count--;
                }else if(eventBuf[i].code == KEY_INFO_){
                    printf("count = %d \n",count);
                }else if(eventBuf[i].code >= 2 && eventBuf[i].code <= 10) {
                    printf("%d",eventBuf[i].code - 1);
                    fflush(stdout);
                }else if(eventBuf[i].code ==11) {
                    printf("0");
                    fflush(stdout);
                }else if(eventBuf[i].code == KEY_EXIT_) {
                    return ;
                }else if(eventBuf[i].code == KEY_CHANNEL_UP){  
                    zapNext();
                }else if(eventBuf[i].code == KEY_CHANNEL_DOWN){
                    zapPrevious();
                }else if(eventBuf[i].code == KEY_MUTE_){
                }
                
            }
        }
        
    }
    
    free(eventBuf);
    return NULL;
}


int main(int argc, char *argv[])
{    
    pthread_t init_thread;
    
    if(pthread_create(&init_thread, NULL, init, NULL)) {
        fprintf(stderr, "Error creating thread\n");
        return ERROR;
    }
    
    initSTB();
    initList();

    /* Wait for a while */
    fflush(stdin);
    
    if(pthread_join(init_thread, NULL)) {
        fprintf(stderr, "Error joining thread\n");
        return ERROR;
    }
    
    /*Deinitialization*/
    deinitList();
    deinitSTB();

    return 0;
}

int32_t getKeys(int32_t count, uint8_t* buf, int32_t* eventsRead)
{
    int32_t ret = 0;
    
    /* read input events and put them in buffer */
    ret = read(inputFileDesc, buf, (size_t)(count * (int)sizeof(struct input_event)));
    if(ret <= 0)
    {
        printf("Error code %d", ret);
        return ERROR;
    }
    /* calculate number of read events */
    *eventsRead = ret / (int)sizeof(struct input_event);
    
    return NO_ERROR;
}