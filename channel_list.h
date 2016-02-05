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

#define MAX_VOLUME 4294967295UL

typedef struct channel {
	uint16_t channelNumber;
	uint16_t vPID; 
    tStreamType vType;
    uint16_t aPID; 
    tStreamType aType;
    uint8_t teletext;
}channel;

void initList();
void deinitList();
void addPat(PatHeader* patHeader);
void addService(PatServiceInfo* patServiceInfo);
void addPmt(PmtHeader* pmtHeader);
void addStreamInfo(StreamInfo* streamInfo, uint16_t programNumber);

channel getCurrentChannel();
void zapNext();
void zapPrevious();

void volumeUp();
void volumeDown();
void muteUnmute();
uint32_t getVolume();

uint8_t zapChannel(uint16_t channelNumber);
void playStream();

void setVPid(uint32_t pid);
void setVType(tStreamType type);
void setAPid(uint32_t pid);
void setAType(tStreamType type);
void scannChannel(uint16_t servicePid);
void parsePat();
void stopStream();

#endif