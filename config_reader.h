#ifndef CONFIG_READER_LISTS
#define CONFIG_READER_LISTS

//#include <cstdlib>
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

//TODO type change
typedef struct _config {  
	uint32_t frequency;
	uint32_t bandwidth;
	int module;
	int aPID;
	int vPID;
	int aType;
	int vType;
}config;

int readLine(FILE* fp, char name[], char value[]);
void addProperty(char name[], char value[]);
void print(config conf);
int initConfig(int argc, char** argv);
config getConfig();

#endif