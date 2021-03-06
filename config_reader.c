
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
    fflush(stdout);
}

void addProperty(char name[], char value[]) {
    // printf("\n *%s* *%s* \n", name, value);
	if(strcmp(name,"frequency")==0) {
		d_config.frequency = atoi(value);
	}else if(strcmp(name,"bandwidth")==0) {
		d_config.bandwidth = atoi(value);
	}else if(strcmp(name,"aPID")==0) {
		d_config.aPID = atoi(value);
	}else if(strcmp(name,"vPID")==0) {
		d_config.vPID = atoi(value);
	}else if(strcmp(name,"module")==0) {
		d_config.module = toTModule(value);
	}else if(strcmp(name,"aType")==0) {
        // printf("toAudioTStreamType %d\n", toAudioTStreamType(value));
		d_config.aType = toAudioTStreamType(value);
	}else if(strcmp(name,"vType")==0) {
        // printf("toVideoTStreamType %d\n", toVideoTStreamType(value));
		d_config.vType = toVideoTStreamType(value);
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
            value[i++] = 0;
			break;
		}
		value[i++] = ch;
	}
	
	value[i - 1] = 0;

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
    // print(d_config);
	fclose(fp);
	return 0;
}

tStreamType toAudioTStreamType(char type[])
{
    if (strcmp(type, "ac3") == 0) {
        return AUDIO_TYPE_DOLBY_AC3;
    }
    if (strcmp(type, "plus") == 0) {
        return AUDIO_TYPE_DOLBY_PLUS;
    }
    if (strcmp(type, "true_hd") == 0) {
        return AUDIO_TYPE_DOLBY_TRUE_HD;
    }
    if (strcmp(type, "lpcm_sd") == 0) {
        return AUDIO_TYPE_LPCM_SD;
    }
    if (strcmp(type, "lpcm_bd") == 0) {
        return AUDIO_TYPE_LPCM_BD;
    }
    if (strcmp(type, "lpcm_hd") == 0) {
        return AUDIO_TYPE_LPCM_HD;
    }
    if (strcmp(type, "mlp") == 0) {
        return AUDIO_TYPE_MLP;
    }
    if (strcmp(type, "dts") == 0) {
        return AUDIO_TYPE_DTS;
    }
    if (strcmp(type, "dts_hd") == 0) {
        return AUDIO_TYPE_DTS_HD;
    }
    if (strcmp(type, "mpeg_audio") == 0) {
        return AUDIO_TYPE_MPEG_AUDIO;
    }
    if (strcmp(type, "mp3") == 0) {
        return AUDIO_TYPE_MP3;
    }
    if (strcmp(type, "he_aac") == 0) {
        return AUDIO_TYPE_HE_AAC;
    }
    if (strcmp(type, "wma") == 0) {
        return AUDIO_TYPE_WMA;
    }
    if (strcmp(type, "wma_pro") == 0) {
        return AUDIO_TYPE_WMA_PRO;
    }
    if (strcmp(type, "wma_lossless") == 0) {
        return AUDIO_TYPE_WMA_LOSSLESS;
    }
    if (strcmp(type, "raw_pcm") == 0) {
        return AUDIO_TYPE_RAW_PCM;
    }
    if (strcmp(type, "sdds") == 0) {
        return AUDIO_TYPE_SDDS;
    }
    if (strcmp(type, "dd_dcv") == 0) {
        return AUDIO_TYPE_DD_DCV;
    }
    if (strcmp(type, "dra") == 0) {
        return AUDIO_TYPE_DRA;
    }
    if (strcmp(type, "dra_ext") == 0) {
        return AUDIO_TYPE_DRA_EXT;
    }
    if (strcmp(type, "dra_lbr") == 0) {
        return AUDIO_TYPE_DTS_LBR;
    }
    if (strcmp(type, "dts_hres") == 0) {
        return AUDIO_TYPE_DTS_HRES;
    }
    if (strcmp(type, "lpcm_sesf") == 0) {
        return AUDIO_TYPE_LPCM_SESF;
    }
    if (strcmp(type, "dv_sd") == 0) {
        return AUDIO_TYPE_DV_SD;
    }
    if (strcmp(type, "vorbis") == 0) {
        return AUDIO_TYPE_VORBIS;
    }
    if (strcmp(type, "flac") == 0) {
        return AUDIO_TYPE_FLAC;
    }
    if (strcmp(type, "raw_aac") == 0) {
        return AUDIO_TYPE_RAW_AAC;
    }
    if (strcmp(type, "ra8") == 0) {
        return AUDIO_TYPE_RA8;
    }
    if (strcmp(type, "raac") == 0) {
        return AUDIO_TYPE_RAAC;
    }
    if (strcmp(type, "adpcm") == 0) {
        return AUDIO_TYPE_ADPCM;
    }
    if (strcmp(type, "spdif_input") == 0) {
        return AUDIO_TYPE_SPDIF_INPUT;
    }
    if (strcmp(type, "G711A") == 0) {
        return AUDIO_TYPE_G711A;
    }
    if (strcmp(type, "G711U") == 0) {
        return AUDIO_TYPE_G711U;
    }
    if (strcmp(type, "raw_signed_pcm") == 0) {
        return AUDIO_RAW_SIGNED_PCM;
    }
    if (strcmp(type, "raw_unsigned_pcm") == 0) {
        return AUDIO_RAW_UNSIGNED_PCM;
    }
    if (strcmp(type, "amr_wb") == 0) {
        return AUDIO_AMR_WB;
    }
    if (strcmp(type, "amr_nb") == 0) {
        return AUDIO_AMR_NB;
    }
    return AUDIO_TYPE_UNSUPPORTED;
}

tStreamType toVideoTStreamType(char vType[])
{
    printf("type = *%s* \n", vType);

    if (strcmp(vType, "mpeg2") == 0) {
        return VIDEO_TYPE_MPEG2;
    }
    if (strcmp(vType, "h264") == 0) {
        return VIDEO_TYPE_H264;
    }
    if (strcmp(vType, "vci") == 0) {
        return VIDEO_TYPE_VC1;
    }
    if (strcmp(vType, "mpeg4") == 0) {
        return VIDEO_TYPE_MPEG4;
    }
    if (strcmp(vType, "mpeg1") == 0) {
        return VIDEO_TYPE_MPEG1;
    }
    if (strcmp(vType, "jpeg") == 0) {
        return VIDEO_TYPE_JPEG;
    }
    if (strcmp(vType, "div3") == 0) {
        return VIDEO_TYPE_DIV3;
    }
    if (strcmp(vType, "div4") == 0) {
        return VIDEO_TYPE_DIV4;
    }
    if (strcmp(vType, "dx50") == 0) {
        return VIDEO_TYPE_DX50;
    }
    if (strcmp(vType, "mvc") == 0) {
        return VIDEO_TYPE_MVC;
    }
    if (strcmp(vType, "wmv3") == 0) {
        return VIDEO_TYPE_WMV3;
    }
    if (strcmp(vType, "divx") == 0) {
        return VIDEO_TYPE_DIVX;
    }
    if (strcmp(vType, "div5") == 0) {
        return VIDEO_TYPE_DIV5;
    }
    if (strcmp(vType, "rv30") == 0) {
        return VIDEO_TYPE_RV30;
    }
    if (strcmp(vType, "rv40") == 0) {
        return VIDEO_TYPE_RV40;
    }
    if (strcmp(vType, "vp60") == 0) {
        return VIDEO_TYPE_VP6;
    }
    if (strcmp(vType, "sorenson") == 0) {
        return VIDEO_TYPE_SORENSON;
    }
    if (strcmp(vType, "h263") == 0) {
        return VIDEO_TYPE_H263;
    }
    if (strcmp(vType, "jpeg_signle") == 0) {
        return VIDEO_TYPE_JPEG_SINGLE;
    }
    if (strcmp(vType, "vp8") == 0) {
        return VIDEO_TYPE_VP8;
    }
    if (strcmp(vType, "vp6f") == 0) {
        return VIDEO_TYPE_VP6F;
    }
    return VIDEO_TYPE_VP6F + 1;
}

t_Module toTModule(char module[])
{
    if (strcmp(module, "DVT-T")) {
        return DVB_T;
    }
    if (strcmp(module, "DVT-T2")) {
        return DVB_T2;
    }
}
