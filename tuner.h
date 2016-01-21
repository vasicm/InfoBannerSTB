#ifndef HEADER_TUNER
#define HEADER_TUNER
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

void setFrequency(uint32_t frequency);
void setbBandwidth(uint32_t bandwidth);
void setModul(t_Module modul);
uint32_t getFrequency();
uint32_t getbBandwidth();
t_Module getModul();

void initSTB();
void deinitSTB();

void scanPrograms();

#endif