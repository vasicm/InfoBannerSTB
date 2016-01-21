
/* 
 * File:   config_reader.c
 * Author: Vasic
 *
 * Created on 21 December 2015, 18:03
 */
#include "config_reader.h"
// using namespace std;
#define dbg printf("\nfunction:%s(%d)\n",__FUNCTION__,__LINE__)


static config d_config;

void print(config conf) {
	printf("frequency = %d \n",conf.frequency);
	printf("bandwidth = %d \n",conf.bandwidth);
	printf("module = %d \n",conf.module);
	printf("aPID = %d \n",conf.aPID);
	printf("vPID = %d \n",conf.vPID);
	printf("aType = %d \n",conf.aType);
	printf("vType = %d \n\n",conf.vType);
}

void addProperty(char name[], char value[]) {
	if(strcmp(name,"frequency")==0) {
		d_config.frequency = atoi(value);
	}else if(strcmp(name,"bandwidth")==0) {
		d_config.bandwidth = atoi(value);
	}else if(strcmp(name,"aPID")==0) {
		d_config.aPID = atoi(value);
	}else if(strcmp(name,"vPID")==0) {
		d_config.vPID = atoi(value);
	}else if(strcmp(name,"module")==0) {
		//TODO
		d_config.module = DVB_T;
	}else if(strcmp(name,"aType")==0) {
		//TODO
		d_config.aType = AUDIO_TYPE_MPEG_AUDIO;
	}else if(strcmp(name,"vType")==0) {
		//TODO
		d_config.vType = VIDEO_TYPE_MPEG2;
	}
}
int readLine(FILE* fp, char name[], char value[]){
	char ch;
	int i = 0;
	while( ( ch = fgetc(fp) ) != 61 ) {
		if(ch == 10 || feof(fp)) {
			name[i++] = 0;
			return 0;
		}
		name[i++] = ch;
	}
	name[i++] = 0;
	i = 0;
	while( ( ch = fgetc(fp) ) != 10) {
		if(feof(fp)) {
			break;
		}
		value[i++] = ch;
	}
	
	value[i++] = 0;

	return 1;
}

config getConfig()
{
	return d_config;
}

int initConfig(int argc, char** argv)
{
	char ch;
	char file_name[50] = "";
	char name[15] = {0};
	char value[15] = {0};
	FILE *fp;
	strcpy(file_name, argv[1]);
	fp = fopen(file_name,"r"); // read mode

	if( fp == NULL )
	{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	int i = 0;
	while(!feof(fp)) {
		if (readLine(fp, name, value)) {
			addProperty(name, value);
		}
	}

	fclose(fp);
	return 0;
}

