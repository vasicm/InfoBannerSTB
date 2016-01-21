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
#include "view.h"
#include "config_reader.h"

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
#define dbg printf("\nfunction:%s(%d)\n",__FUNCTION__,__LINE__)


void applicationInit(int argc, char *argv[]);
void applicationDeinit();
void eventHandle();

int main(int argc, char *argv[])
{    
    if(argc != 2 ) {
        return 0;
    }

    applicationInit(argc, argv);
   
    eventHandle();
    
    applicationDeinit();

    return 0;
}

void applicationInit(int argc, char *argv[]) {
    config conf;

    initEventListener();
    initConfig(argc, argv);
    conf = getConfig();

    setFrequency(conf.frequency);
    setbBandwidth(conf.bandwidth);
    setModul(conf.module);
    initSTB();

    setVPid(conf.vPID);
    setVType(conf.vType);
    setAPid(conf.aPID);
    setAType(conf.aType);
    initList();

    initView(argc, argv);

}

void applicationDeinit() {
    deintiView();
    deinitList();
    deinitSTB();
    deintiEventListener();
}

void eventHandle()
{
    struct input_event* eventBuf;
    uint32_t eventCnt;
    uint32_t i;
    uint32_t count=3;
    int p = 0;
    int pp = 0;
    channel currChannel;
    int currNumber = 0;
    struct timeval now;
    int flag = 1;
    int muteFlag = 0;

    eventBuf = malloc(NUM_EVENTS * sizeof(struct input_event));
    if(!eventBuf)
    {
        printf("Error allocating memory !");
        return ;
    }

    while(flag)
    {
        /* read input eventS */
        if(getKeys(NUM_EVENTS, (uint8_t*)eventBuf, &eventCnt))
        {
            printf("Error while reading input events !");
            return ;
        }

        gettimeofday(&now,NULL);

        for(i = 0; i < eventCnt && flag; i++)
        {
            if(eventBuf[i].value == 1) {
                
                //TODO replase with switch
                if(eventBuf[i].code == KEY_VOLUME_UP) {
                    count++;
                    muteFlag = 0;
                    showVolumeGraph(count);
                }else if(eventBuf[i].code == KEY_VOLUME_DOWN) {
                    count--;
                    muteFlag = 0;
                    showVolumeGraph(count);
                }else if(eventBuf[i].code == KEY_INFO_){
                    if(isInfoBannerShowM()) {
                        hiddeInfoBanner();
                    } else {
                        currChannel = getCurrentChannel();
                        showInfoBanner(currChannel.channelNumber, currChannel.aPID, currChannel.vPID, currChannel.teletext);
                    }
                    currNumber = 0;
                }else if(eventBuf[i].code >= 2 && eventBuf[i].code <= 11) {
                    pp = p;
                    p = now.tv_sec; 

                    if( (p - pp)>2 ) {
                        currNumber = (eventBuf[i].code - 1) % 10; 
                    } else {
                        currNumber *= 10;
                        currNumber += (eventBuf[i].code - 1) % 10;
                    }

                    showInfoBanner(currNumber, 0, 0, 0);
                    if(zapChannel((uint16_t)currNumber)) {
                        currChannel = getCurrentChannel();
                        showInfoBanner(currChannel.channelNumber, currChannel.aPID, currChannel.vPID, currChannel.teletext);
                    }
                }else if(eventBuf[i].code == KEY_EXIT_) {
                    flag = 0;
                }else if(eventBuf[i].code == KEY_CHANNEL_UP){  
                    zapNext();
                    currChannel = getCurrentChannel();
                    showInfoBanner(currChannel.channelNumber, currChannel.aPID, currChannel.vPID, currChannel.teletext);
                }else if(eventBuf[i].code == KEY_CHANNEL_DOWN){
                    zapPrevious();
                    currChannel = getCurrentChannel();
                    showInfoBanner(currChannel.channelNumber, currChannel.aPID, currChannel.vPID, currChannel.teletext);
                }else if(eventBuf[i].code == KEY_MUTE_){
                    if(muteFlag) {
                        showVolumeGraph(count);
                        muteFlag = 0;
                    } else {
                        muteFlag = 1;
                        showVolumeGraph(0);
                    }
                }
                
            }
        }
        
    }
    
    free(eventBuf);
    return;
}