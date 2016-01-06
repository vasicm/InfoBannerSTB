#ifndef HEADER_TABLE_PARSER
#define HEADER_TABLE_PARSER

#include <stdint.h>
typedef struct PatHeader{
	uint8_t table_id;
	uint8_t section_syntax_indicator;
	uint8_t reserved;
	uint16_t section_length;
	uint16_t transport_stream_id;
	uint8_t version_number;
	uint8_t current_next_indicator;
	uint8_t section_number;
	uint8_t last_section_number;
}PatHeader;

typedef struct PatServiceInfo{
	uint16_t program_number;
	uint16_t program_map_pid;
	uint8_t reserved;
}PatServiceInfo;

typedef struct PmtHeader{
        uint8_t table_id; // dentifikator tabele, za PMT je 0x02
        uint8_t section_syntax_indicator; // postavljeno na 1.
        uint16_t section_length; // dužina PMT tabele pocevši od ovog polja
        uint16_t probram_number; // jedinstveni broj programa na koji se odnosi ova PMT tabela.
        uint8_t version_number; /*  verzija PMT-a. Ako se podaci u PMT-u menjaju ovo polje se
                                    inkrementira sve dok ne dostigne vrednost 31, kada se resetuje na 0. Kada je
                                    polje current_next_indicator postavljeno na 1 tada se verzija odnosi na trenutnu
                                    tabelu, a kada je polje current_next_indicator postavljeno na 0, tada se verzija
                                    odnosi na sledeću PMT tabelu.*/
        uint8_t current_next_indicator; //kada je postavljeno na 1 tada je ova tabela primenjiva, a kada je 0 tada je tek sledeca PMT tabela primenjiva.
        uint8_t section_number; //broj trenutne sekcije koja je sastavni deo PMT-a.
        uint8_t last_section_number; // broj poslednje sekcije koja cini PMT.
        uint16_t PCR_PID;
        uint16_t program_info_length; //sadrži broj bajtova koji cine deskriptore koji slede nakon
                                      //ovog polja.
}PmtHeader;

typedef struct StreamInfo{
        uint8_t stream_type;
        uint16_t elementary_PID; 
        uint16_t ES_info_length;
}StreamInfo;

uint8_t parsePatHeader(uint8_t* buffer, PatHeader* header);
uint8_t parsePatServiceInfo(uint8_t* buffer, PatServiceInfo* service);

uint8_t parsePmtHeader(uint8_t* buffer, PmtHeader* header);
uint8_t parseStreamInfo(uint8_t* buffer, StreamInfo* streamInfo);
#endif