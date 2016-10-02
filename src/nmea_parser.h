/*
 * nmea_parser.h
 *
 *  Created on: 1/10/2016
 *      Author: Yuechuan Yang
 */

#ifndef NMEA_PARSER_H_
#define NMEA_PARSER_H_

#define MAX_NAME_SIZE       		(6)
#define MAX_PACKET_PARTS    		(20)

#define COMMAND_OK              	(0)
#define ERR_COMMAND_INVALID     	(1)
#define ERR_COMMAND_CHECKSUM    	(2)

#define CHUNK 						(1024)
#define COMMAND_LIST_LENGTH 		(2)
#define PACKET_LIST_SIZE_MAX 		(20)

#define NMEA_PREFIX_LENGTH			(5)
#define CHECKSUM_LENGTH 			(2)

#define ARRAY_LENGTH(a) (sizeof a / sizeof (a[0]))

/* GPRMC packet part indexes */
#define GPRMC_TIME					(0)
#define GPRMC_STATUS				(1)
#define GPRMC_LATITUDE				(2)
#define GPRMC_NS_INDICATOR			(3)
#define GPRMC_LONGITUDE				(4)
#define GPRMC_EW_INDICATOR			(5)
#define GPRMC_SPEED_OVER_GROUD		(6)
#define GPRMC_COURSE_OVER_GROUND	(7)
#define GPRMC_DATE					(8)
#define GPRMC_MAGNETIC_VARIATION	(9)
#define GPRMC_MV_EW_INDICATOR		(10)
#define GPRMC_MODE					(11)

#define END 						'\0'
#define DOLLAR						'$'
#define STAR						'*'
#define COMMA						','
#define CR              			(13)

typedef unsigned char uint8_t;
typedef signed int int16_t;
typedef unsigned int uint16_t;

typedef struct
{
	int start_index;
	int length;
} packet_t;

typedef struct
{
    char* utc_time;
    char status;
    double latitude;
    char ns_indicator;
    double longitude;
    char ew_indicator;
    float spd;
    float cog;
    char* date;
    float mv;
    char mve;
    char mode;
    uint8_t checksum;
} gps_rec_min_data_t;

typedef struct
{
    char* utc_time;
    float latitude;
    char ns_indicator;
    float longitude;
    char ew_indicator;
    uint8_t fix_status;
    uint8_t nosv;
    float hdop;
    float msl;
    char umsl;
    float altref;
    char usep;
    uint8_t diff_age;
    uint16_t diff_station;
    uint8_t checksum;
} gps_fix_data_t;

void
user_input ();
void
run_example ();
static int
process_input (const char *in_str, packet_t* packet_list);
static void
process_packet (char * buffer, packet_t packet);
uint8_t
check_packet (const char *packet);
static void
parse_packet (char *packet, int length);
uint8_t
split_string_by_comma (char *string, char **values, uint8_t max_values);
static void
parse_gprmc (gps_rec_min_data_t *packet_data, char *value, int val_index);
void
print_gprmc (gps_rec_min_data_t *packet_data);

#endif /* NMEA_PARSER_H_ */
