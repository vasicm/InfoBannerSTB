#ifndef HEADER_LISTS
#define HEADER_LISTS

#include "tables_parser.h"

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

typedef struct channel {
	uint16_t channelNumber;
	uint16_t vPID; 
    tStreamType vType;
    uint16_t aPID; 
    tStreamType aType;
}channel;

void initList();
void deinitList();
void addPat(PatHeader* patHeader);
void addService(PatServiceInfo* patServiceInfo);
void addPmt(PmtHeader* pmtHeader);
void addStreamInfo(StreamInfo* streamInfo, uint16_t programNumber);
//getAllChannels
channel getCurrentChannel();
void zapNext();
void zapPrevious();
//zapLogNumb()
//zapChannel()
#endif