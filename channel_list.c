#include "channel_list.h"

#define MAX_SERVICE 20
#define ASSERT_TDP_RESULT(x,y)  if(NO_ERROR == x) \
                                    printf("%s success\n", y); \
                                else{ \
                                    printf("%s fail\n", y); \
                                    return; \
                                }

#define dbg printf("\nfunction:%s(%d)\n",__FUNCTION__,__LINE__)


static int32_t pmtCallback(uint8_t *buffer); //todo static
static int32_t patCallback(uint8_t *buffer); //todo static

static uint32_t playerHandle;
static uint32_t sourceHandle;
static uint32_t filterHandle;

static uint32_t videoStream;
static uint32_t audioStream;

static PatHeader* header;
static PatServiceInfo* service;

static PmtHeader* pmtHeader;
static StreamInfo* streamInfo;

static pthread_cond_t statusCondition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t statusMutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t statusCondition2 = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t statusMutex2 = PTHREAD_MUTEX_INITIALIZER;

static struct timespec lockStatusWaitTime;
static struct timeval now;

static channel channels[15];
static uint8_t channelNumber;
static uint8_t currChannel;

static uint8_t globalFlag;

static uint32_t vPID = 1001; 
static tStreamType vType = VIDEO_TYPE_MPEG2;
static uint32_t aPID = 1003; 
static tStreamType aType = AUDIO_TYPE_MPEG_AUDIO;

static uint32_t volume;
static uint8_t muteFlag = 0;

void setVPid(uint32_t pid) {
    vPID = pid;
}

void setVType(tStreamType type) {
    vType = type;
}

void setAPid(uint32_t pid) {
    aPID = pid;
}

void setAType(tStreamType type) {
    aType = type;
}

void volumeUp() {
    //Player_Volume_Get(playerHandle, &volume);
    volume = (volume > (MAX_VOLUME - MAX_VOLUME / 11UL)) ? MAX_VOLUME : (volume + (MAX_VOLUME / 11UL));
    Player_Volume_Set(playerHandle, volume);
    muteFlag = 0;
}

void volumeDown() {
    //Player_Volume_Get(playerHandle, &volume);
    volume = (volume < MAX_VOLUME / 11UL) ? 0UL : (volume - (MAX_VOLUME / 11UL));
    Player_Volume_Set(playerHandle, volume);
    muteFlag = 0;
}

void muteUnmute() {
    if(muteFlag) {
        Player_Volume_Set(playerHandle, volume);
        muteFlag = 0;
    } else {
        Player_Volume_Set(playerHandle, 0UL);
        muteFlag = 1;
    }
}

uint32_t getVolume() {
    if(muteFlag) {
        return 0;
    } else {
        return volume;
    }   
}

void stopStream() {
    // printf("\n videoStream = %d\n",videoStream);
    if(videoStream != 0) {
        Player_Stream_Remove(playerHandle, sourceHandle, videoStream);
    }

    // printf("\n videoStream = %d\n",videoStream);
    if(audioStream != 0) {
        Player_Stream_Remove(playerHandle, sourceHandle, audioStream);
    }
}

void playStream(){

    if(channels[currChannel].vPID != 0) {
        Player_Stream_Create(playerHandle, sourceHandle, channels[currChannel].vPID, channels[currChannel].vType, &videoStream);
    } else {
        videoStream = 0;
    }

    if(channels[currChannel].aPID != 0) {
        Player_Stream_Create(playerHandle, sourceHandle, channels[currChannel].aPID, channels[currChannel].aType, &audioStream);
    } else {
        audioStream = 0;
    }
    // printf("\n currChannel = %d\n",currChannel);
}

void printChannels() {
	uint8_t i;
	for(i = 0;i<=channelNumber;i++) {
		printf("\n channelNumber = %d",channels[i].channelNumber);
		printf("\n vPID = %d",channels[i].vPID);
		printf("\n vType = %d",channels[i].vType);
		printf("\n aPID = %d",channels[i].aPID);
		printf("\n aType = %d\n",channels[i].aType);
        printf("\n teletext = %d\n",channels[i].teletext);
	}
}

uint8_t zapChannel(uint16_t chNumb) {
    uint8_t ret = 0;
    uint8_t i;
    for(i = 0; i<channelNumber; i++) {
        if(channels[i].channelNumber == chNumb) {
            currChannel = i;
            ret = 1;
            stopStream();
            playStream();
        }
    }

    return ret;
}

void zapNext() {
	uint8_t i;
	for(i = 0;i<channelNumber;i++) {
		currChannel= (currChannel + 1)%(channelNumber);
		if(channels[currChannel].vPID != 0 || channels[currChannel].aPID != 0) {
            stopStream();
			playStream();
			return;
		}
	}
}

void zapPrevious() {
	uint8_t i;
	for(i = channelNumber - 1; i>=0; i--) {
		if(currChannel == 0) {
			currChannel=channelNumber;
		}else{
			currChannel--;
		}	
		if(channels[currChannel].vPID != 0 || channels[currChannel].aPID != 0) {
            stopStream();
			playStream();
			return;
		}
	}
}

channel getCurrentChannel() {
    return channels[currChannel];
}

void scannChannel(uint16_t servicePid)
{
    t_Error err = NO_ERROR;

    err = Demux_Set_Filter(playerHandle, servicePid, 0x02, &filterHandle);
    ASSERT_TDP_RESULT(err,"\nDemux_Set_Filter\n");

    err = Demux_Register_Section_Filter_Callback(pmtCallback);
    ASSERT_TDP_RESULT(err,"\nDemux_Register_Section_Filter_Callback\n");

    printf("%d channels\n",channelNumber);

    gettimeofday(&now,NULL);
    lockStatusWaitTime.tv_sec = now.tv_sec+20;

    pthread_mutex_lock(&statusMutex2);
    if(ETIMEDOUT == pthread_cond_timedwait(&statusCondition2, &statusMutex2, &lockStatusWaitTime))
    {
        printf("\n%s:ERROR PARSE PAT TABLE!\n",__FUNCTION__);
        return;
    }
    pthread_mutex_unlock(&statusMutex2);

    err = Demux_Unregister_Section_Filter_Callback(pmtCallback);
    ASSERT_TDP_RESULT(err,"\nDemux_Unregister_Section_Filter_Callback\n");

    err = Demux_Free_Filter(playerHandle, filterHandle);
    ASSERT_TDP_RESULT(err,"Demux_Free_Filter\n");
}

void parsePat() {
    uint8_t i;
    uint8_t N;
    t_Error err = NO_ERROR;

    gettimeofday(&now,NULL);
    lockStatusWaitTime.tv_sec = now.tv_sec+5;

    err = Demux_Set_Filter(playerHandle, 0, 0, &filterHandle);
    ASSERT_TDP_RESULT(err,"\nDemux_Set_Filter\n");

    err = Demux_Register_Section_Filter_Callback(patCallback);
    ASSERT_TDP_RESULT(err,"\nDemux_Register_Section_Filter_Callback\n");
    
    /*Initialize player, set PAT pid to demultiplexer and register section filter callback*/
    pthread_mutex_lock(&statusMutex);
    if(ETIMEDOUT == pthread_cond_timedwait(&statusCondition, &statusMutex, &lockStatusWaitTime))
    {
        printf("\n%s:ERROR PARSE PAT TABLE!\n",__FUNCTION__);
        return;
    }
    pthread_mutex_unlock(&statusMutex);

    err = Demux_Unregister_Section_Filter_Callback(patCallback);
    ASSERT_TDP_RESULT(err,"\nDemux_Unregister_Section_Filter_Callback\n");

    err = Demux_Free_Filter(playerHandle, filterHandle);
    ASSERT_TDP_RESULT(err,"Demux_Free_Filter\n\n");

    N = (header->section_length - 9) / 4;
    for(i = 1; i < N; i++) {
        scannChannel((service+i)->program_map_pid);
        channelNumber++;
    }
}

void initList() {

	header = (PatHeader*)malloc(sizeof(PatHeader));
    service = (PatServiceInfo*)malloc(MAX_SERVICE * sizeof(PatServiceInfo));
    pmtHeader = (PmtHeader*)malloc(sizeof(PmtHeader));
    streamInfo = (StreamInfo*)malloc(MAX_SERVICE * sizeof(StreamInfo));

	t_Error err = NO_ERROR;

	err = Player_Init(&playerHandle);
    ASSERT_TDP_RESULT(err,"\nPlayer_Init\n");

    err = Player_Source_Open(playerHandle, &sourceHandle);
    ASSERT_TDP_RESULT(err,"\nPlayer_Source_Open\n");

    parsePat();

    printChannels();

    /*Play audio and video*/
    Player_Stream_Create(playerHandle, sourceHandle, vPID, vType, &videoStream);
    Player_Stream_Create(playerHandle, sourceHandle, aPID, aType, &audioStream);

    Player_Volume_Get(playerHandle, &volume);
}

void deinitList() {
    stopStream();
	//Demux_Free_Filter(playerHandle, filterHandle);
    Player_Source_Close(playerHandle, sourceHandle);
    free(header);
    free(service);
    free(pmtHeader);
    free(streamInfo);
}

int32_t pmtCallback(uint8_t *buffer) {
        int i = 1;
        int N = 0;
    
        parsePmtHeader(buffer, pmtHeader);
        if(pmtHeader->current_next_indicator == 0) {
            return NO_ERROR;
        }
        pthread_mutex_lock(&statusMutex2);

        printPmtHeader(pmtHeader);
        uint8_t* streamInfoBuffer = buffer + 12 + pmtHeader->program_info_length;

		channels[channelNumber].channelNumber = pmtHeader->probram_number;
        channels[channelNumber].teletext = 0;
        
        while(streamInfoBuffer < (buffer + pmtHeader->section_length - 15) ) {
                parseStreamInfo(streamInfoBuffer, streamInfo);

                printf("\n%d.\n",i++);
                if(streamInfo->stream_type == 2 || streamInfo->stream_type == 1) {
                	channels[channelNumber].vPID = streamInfo->elementary_PID;
                	channels[channelNumber].vType = VIDEO_TYPE_MPEG2;
                }else if(streamInfo->stream_type == 3) {
                	channels[channelNumber].aPID = streamInfo->elementary_PID;
                	channels[channelNumber].aType = AUDIO_TYPE_MPEG_AUDIO;
                }

                printStreamInfo(streamInfo);
                streamInfoBuffer += 5 + streamInfo->ES_info_length;

                if(streamInfo->descriptor_tag == 0x56) {
                    channels[channelNumber].teletext = 1;
                }

        }

        pthread_cond_signal(&statusCondition2);
        pthread_mutex_unlock(&statusMutex2);
        return NO_ERROR;
        
}

int32_t patCallback(uint8_t *buffer) {

 	int i = 0;
	int N = 0;
    pthread_mutex_lock(&statusMutex);
        
	parsePatHeader(buffer, header);
    printPatHeader(header);    

	N = (header->section_length - 9) / 4;
	for(i=0;i<N;i++){
		parsePatServiceInfo((buffer+8+(i*4)),service + i);
        printPatServiceInfo(service + i);
	}

	pthread_cond_signal(&statusCondition);
    pthread_mutex_unlock(&statusMutex);
    return NO_ERROR;
}