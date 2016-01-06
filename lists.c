#include "lists.h"

#define MAX_SERVICE 20
#define ASSERT_TDP_RESULT(x,y)  if(NO_ERROR == x) \
                                    printf("%s success\n", y); \
                                else{ \
                                    printf("%s fail\n", y); \
                                    return; \
                                }

static int32_t demuxCallback2(uint8_t *buffer); //todo static
static int32_t demuxCallback(uint8_t *buffer); //todo static

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

void printChannels() {
	uint8_t i;
	for(i = 0;i<=channelNumber;i++) {
		printf("\n channelNumber = %d",channels[i].channelNumber);
		printf("\n vPID = %d",channels[i].vPID);
		printf("\n vType = %d",channels[i].vType);
		printf("\n aPID = %d",channels[i].aPID);
		printf("\n aType = %d\n",channels[i].aType);
	}
}

void zapNext() {
	uint8_t i;
	for(i = 0;i<=channelNumber;i++) {
		currChannel= (currChannel + 1)%(channelNumber+1);
		if(channels[currChannel].vPID != 0) {
			printf("\n currChannel = %d\n",currChannel);
			Player_Stream_Remove(playerHandle, sourceHandle, videoStream);
			// Player_Stream_Create(playerHandle, sourceHandle, 1001, vType, &videoStream);
			Player_Stream_Create(playerHandle, sourceHandle, channels[currChannel].vPID, channels[currChannel].vType, &videoStream);
			return;
		}
	}
}

void zapPrevious() {
	uint8_t i;
	for(i = channelNumber;i>=0;i--) {
		if(currChannel == 0) {
			currChannel=channelNumber;
		}else{
			currChannel--;
		}	
		if(channels[currChannel].vPID != 0) {
			printf("\n currChannel = %d\n",currChannel);
			Player_Stream_Remove(playerHandle, sourceHandle, videoStream);
			// Player_Stream_Create(playerHandle, sourceHandle, 1001, vType, &videoStream);
			Player_Stream_Create(playerHandle, sourceHandle, channels[currChannel].vPID, channels[currChannel].vType, &videoStream);
			return;
		}
	}
}

channel getCurrentChannel() {
    return channels[currChannel];
}

void initList() {
	uint32_t vPID = 101; 
    tStreamType vType = VIDEO_TYPE_MPEG2;
    uint32_t aPID = 103; 
    tStreamType aType = AUDIO_TYPE_MPEG_AUDIO;
	uint8_t i;

	gettimeofday(&now,NULL);
    lockStatusWaitTime.tv_sec = now.tv_sec+5000;

	header = (PatHeader*)malloc(sizeof(PatHeader));
    service = (PatServiceInfo*)malloc(MAX_SERVICE * sizeof(PatServiceInfo));
    pmtHeader = (PmtHeader*)malloc(sizeof(PmtHeader));
    streamInfo = (StreamInfo*)malloc(MAX_SERVICE * sizeof(StreamInfo));

	t_Error err = NO_ERROR;

	err = Player_Init(&playerHandle);
    ASSERT_TDP_RESULT(err,"\nPlayer_Init\n");

    err = Player_Source_Open(playerHandle, &sourceHandle);
    ASSERT_TDP_RESULT(err,"\nPlayer_Source_Open\n");

	err = Demux_Set_Filter(playerHandle, 0, 0, &filterHandle);
    ASSERT_TDP_RESULT(err,"\nDemux_Set_Filter\n");

    err = Demux_Register_Section_Filter_Callback(demuxCallback);
    ASSERT_TDP_RESULT(err,"\nDemux_Register_Section_Filter_Callback\n");
    
    /*Initialize player, set PAT pid to demultiplexer and register section filter callback*/
 	pthread_mutex_lock(&statusMutex);
    if(ETIMEDOUT == pthread_cond_timedwait(&statusCondition, &statusMutex, &lockStatusWaitTime))
    {
        printf("\n%s:ERROR PARSE PAT TABLE!\n",__FUNCTION__);
        return;
    }

	err = Demux_Unregister_Section_Filter_Callback(demuxCallback);
	ASSERT_TDP_RESULT(err,"\nDemux_Unregister_Section_Filter_Callback\n");

	err = Demux_Free_Filter(playerHandle, filterHandle);
	ASSERT_TDP_RESULT(err,"Demux_Free_Filter\n\n");

    sleep(3);
    for(i=1; i<7; i++) {
//     	pthread_mutex_lock(&statusMutex2);
//         gettimeofday(&now,NULL);
//         lockStatusWaitTime.tv_sec = now.tv_sec+3;
//     	if(ETIMEDOUT == pthread_cond_timedwait(&statusCondition2, &statusMutex2, &lockStatusWaitTime))
//         {
//             printf("\n%s:ERROR PARSE PAT TABLE!\n",__FUNCTION__);
//             return;
//         }
// printf("\nfunction:%s(%d)\n",__FUNCTION__,__LINE__);
        channelNumber++;

        printf("(service+i)->program_map_pid = %d\n", (service+i)->program_map_pid);
        printf("playerHandle = %d, filterHandle = %d \n", playerHandle, filterHandle);
    	err = Demux_Set_Filter(playerHandle, (service+i)->program_map_pid, 0x02, &filterHandle);
    	ASSERT_TDP_RESULT(err,"\nDemux_Set_Filter\n");

         err = Demux_Register_Section_Filter_Callback(demuxCallback2);
        ASSERT_TDP_RESULT(err,"\nDemux_Register_Section_Filter_Callback\n");

    	printf("%d %d mutex\n",i,channelNumber);
        //globalFlag = 1;
    	 sleep(20);
        err = Demux_Unregister_Section_Filter_Callback(demuxCallback2);
        ASSERT_TDP_RESULT(err,"\nDemux_Unregister_Section_Filter_Callback\n");

        err = Demux_Free_Filter(playerHandle, filterHandle);
        ASSERT_TDP_RESULT(err,"Demux_Free_Filter\n");
    }
    printChannels();
    sleep(1);


printf("\nfunction:%s(%d)\n",__FUNCTION__,__LINE__);
    /**TO DO:**/
    /*Play audio and video*/
    //Player_Stream_Create(playerHandle, sourceHandle, vPID, vType, &videoStream);
    Player_Stream_Create(playerHandle, sourceHandle, 1001, vType, &videoStream);
    Player_Stream_Create(playerHandle, sourceHandle, aPID, aType, &audioStream);
    printf("playerHandle = %d, filterHandle = %d, sourceHandle = %d\n",playerHandle, filterHandle, sourceHandle);
}

void deinitList() {
	printf("\nfunction:%s(%d)\n",__FUNCTION__,__LINE__);
	printf("playerHandle = %d, filterHandle = %d, sourceHandle = %d\n",playerHandle, filterHandle, sourceHandle);
	//Demux_Free_Filter(playerHandle, filterHandle);
    Player_Source_Close(playerHandle, sourceHandle);
    free(header);
    free(service);
    free(pmtHeader);
    free(streamInfo);
}

//16 200 1000 1500 2000 2010 2020
int32_t demuxCallback2(uint8_t *buffer) {
        int i = 1;
        int N = 0;
       
        // if (globalFlag == 0) {
        // 	return 1;
        // }
        // pthread_mutex_lock(&statusMutex2);
        parsePmtHeader(buffer, pmtHeader);
        
        printf("\ntable_id = %d",pmtHeader->table_id);
        printf("\nsection_length = %d",pmtHeader->section_length);
        printf("\nprobram_number = %d",pmtHeader->probram_number);
        printf("\ncurrent_next_indicator = %d",pmtHeader->current_next_indicator);
        // if(pmtHeader->current_next_indicator == 0) {
        // 	return 1;
        // }
        printf("\nsection_number = %d",pmtHeader->section_number);
        printf("\nlast_section_number = %d",pmtHeader->last_section_number);
        printf("\nprogram_info_length = %d",pmtHeader->program_info_length);

        uint8_t* streamInfoBuffer = buffer + 12 + pmtHeader->program_info_length;

		channels[channelNumber].channelNumber = pmtHeader->probram_number;

        while(streamInfoBuffer < (buffer + pmtHeader->section_length - 15) ) {
printf("\nfunction:%s(%d)\n",__FUNCTION__,__LINE__);
                parseStreamInfo(streamInfoBuffer, streamInfo);
printf("\nfunction:%s(%d)\n",__FUNCTION__,__LINE__);

                printf("\n%d.\n",i++);
                if(streamInfo->stream_type == 2 || streamInfo->stream_type == 1) {
                	printf("\n   VIDEO streamInfo->stream_type = %d",streamInfo->stream_type);
                	printf("\n   VIDEO streamInfo->elementary_PID = %d",streamInfo->elementary_PID);
                	channels[channelNumber].vPID = streamInfo->elementary_PID;
                	channels[channelNumber].vType = VIDEO_TYPE_MPEG2;
                	// break;
                }else if(streamInfo->stream_type == 3) {
                	channels[channelNumber].aPID = streamInfo->elementary_PID;
                	channels[channelNumber].aType = AUDIO_TYPE_MPEG_AUDIO;
                }
                printf("\n   streamInfo->stream_type = %d",streamInfo->stream_type);
                printf("\n   streamInfo->elementary_PID = %d",streamInfo->elementary_PID);
                printf("\n   streamInfo->ES_info_length; = %d",streamInfo->ES_info_length);
                streamInfoBuffer += 5 + streamInfo->ES_info_length;

        }
		// globalFlag = 0;
		// pthread_cond_signal(&statusCondition2);
  //    	pthread_mutex_unlock(&statusMutex2);
         return NO_ERROR;
        
}

int32_t demuxCallback(uint8_t *buffer) {

 	int i = 0;
	int N = 0;
    pthread_mutex_lock(&statusMutex);
        
	parsePatHeader(buffer, header);
        
        printf("\ntable_id = %d",header->table_id);
        printf("\nsection length = %d",header->section_length);
        printf("\ntransport_stream_id = %d",header->transport_stream_id);
        printf("\nversion_number = %d",header->version_number);
        printf("\ncurrent_next_indicator = %d",header->current_next_indicator);
        printf("\nsection_number = %d",header->section_number);
        printf("\nlast_section_number = %d",header->last_section_number);

	N = (header->section_length - 9) / 4;

	for(i=0;i<N;i++){
		parsePatServiceInfo((buffer+8+(i*4)),service + i);

                printf("\nservice %d.",i);
                printf("\nservice->program_number = %d",(service + i)->program_number);
                printf("\nservice->program_map_pid = %d",(service + i)->program_map_pid);

	}

	pthread_cond_signal(&statusCondition);
    pthread_mutex_unlock(&statusMutex);
    return NO_ERROR;
}