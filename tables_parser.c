#include "tables_parser.h"

uint8_t parsePatHeader(uint8_t* buffer, PatHeader* header)
{
	header->table_id = (uint8_t) *buffer;
	header->section_syntax_indicator = (uint8_t) (*(buffer + 1) >> 7) & 0x01;
	header->section_length = (uint16_t) (((*(buffer+1) << 8) + *(buffer + 2)) & 0x0FFF);
	header->transport_stream_id = (uint16_t) (((*(buffer+3) << 8) + *(buffer + 4)) & 0xFFFF);
	header->version_number = (uint8_t) ((*(buffer + 5) >> 1) & 0x1F);
	header->current_next_indicator = (uint8_t) (*(buffer + 5) & 0x01);
	header->section_number = (uint8_t) (*(buffer + 6) & 0xFF);
	header->last_section_number = (uint8_t) (*(buffer + 7) & 0xFF);
}

uint8_t parsePatServiceInfo(uint8_t* buffer, PatServiceInfo* service)
{
	service->program_number = (uint16_t) (((*(buffer) << 8) + *(buffer + 1)) & 0xFFFF);
	service->program_map_pid =  (uint16_t) (((*(buffer + 2) << 8) + *(buffer + 3)) & 0x1FFF);
}

// (1)8  (2)16  (3)24  (4)32  (5)40  (6)48 (7)56  (8)64   (9)72  (10)80 (11)88  (12)96
uint8_t parsePmtHeader(uint8_t* buffer, PmtHeader* header)
{
        header->table_id = (uint8_t) *buffer; 
        header->section_syntax_indicator = (uint8_t) (*(buffer + 1) >> 7) & 0x01;
        header->section_length = (uint16_t) (((*(buffer + 1) << 8) + *(buffer + 2)) & 0x0FFF);
        header->probram_number = (uint16_t) (((*(buffer + 3) << 8) + *(buffer + 4)) & 0xFFFF); 
        header->version_number = (uint8_t) (*(buffer + 5) >> 1) & 0x1F;
        header->current_next_indicator = (uint8_t) (*(buffer + 5)) & 0x01;
        header->section_number = (uint8_t) (*(buffer + 6)) & 0xFF;
        header->last_section_number = (uint8_t) (*(buffer + 7)) & 0xFF;;
        header->PCR_PID = (uint16_t) (((*(buffer + 8) << 8) + *(buffer + 9)) & 0x1FFF);
        header->program_info_length = (uint16_t) (((*(buffer + 10) << 8) + *(buffer + 11)) & 0x0FFF);
}

// (0)96  (1)104  (2)112  (3)120  (4)128  (5)136
uint8_t parseStreamInfo(uint8_t* buffer, StreamInfo* streamInfo)
{
        streamInfo->stream_type = (uint8_t) *buffer;
        streamInfo->elementary_PID = (uint16_t) (((*(buffer + 1) << 8) + *(buffer + 2)) & 0x1FFF); 
        streamInfo->ES_info_length = (uint16_t) (((*(buffer + 3) << 8) + *(buffer + 4)) & 0x0FFF); 

        if(streamInfo->ES_info_length > 1) {
                streamInfo->descriptor_tag = (uint8_t)(*(buffer + 5));
        } else {
                streamInfo->descriptor_tag = 0;
        }
}

void printPatHeader(PatHeader* header) {
        printf("\ntable_id = %d",header->table_id);
        printf("\nsection length = %d",header->section_length);
        printf("\ntransport_stream_id = %d",header->transport_stream_id);
        printf("\nversion_number = %d",header->version_number);
        printf("\ncurrent_next_indicator = %d",header->current_next_indicator);
        printf("\nsection_number = %d",header->section_number);
        printf("\nlast_section_number = %d",header->last_section_number);
}

void printPatServiceInfo(PatServiceInfo* service) {
        printf("\nservice->program_number = %d",service->program_number);
        printf("\nservice->program_map_pid = %d",service->program_map_pid);
}

void printPmtHeader(PmtHeader* pmtHeader) {
        printf("\ntable_id = %d",pmtHeader->table_id);
        printf("\nsection_length = %d",pmtHeader->section_length);
        printf("\nprobram_number = %d",pmtHeader->probram_number);
        printf("\ncurrent_next_indicator = %d",pmtHeader->current_next_indicator);
        printf("\nsection_number = %d",pmtHeader->section_number);
        printf("\nlast_section_number = %d",pmtHeader->last_section_number);
        printf("\nprogram_info_length = %d",pmtHeader->program_info_length);
}

void printStreamInfo(StreamInfo* streamInfo) {
        printf("\n   streamInfo->stream_type = %d",streamInfo->stream_type);
        printf("\n   streamInfo->elementary_PID = %d",streamInfo->elementary_PID);
        printf("\n   streamInfo->ES_info_length = %d",streamInfo->ES_info_length);
        printf("\n   streamInfo->descriptor_tag = %d",streamInfo->descriptor_tag);
}
