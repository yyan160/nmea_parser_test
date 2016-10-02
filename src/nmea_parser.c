/*
 ============================================================================
 Name        : nmea_parser.c
 Author      : Yuechuan Yang
 Version     :
 Copyright   : Your copyright notice
 Description : NMEA Parser test
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nmea_parser.h"

static char *command_list[COMMAND_LIST_LENGTH] = {"GPRMC", "GPGGA"};

int main(void) {

	//user_input();
	run_example();

	return EXIT_SUCCESS;
}

/**
 * allow user to enter a string of data
 */
void
user_input ()
{
	char input[CHUNK];
	printf("Please enter the data... \n");
	scanf("%s", input);
	printf("input is: %s\n", input);
	packet_t* packet_list = malloc(PACKET_LIST_SIZE_MAX * sizeof(packet_list));
	int list_length = process_input(input, packet_list);
	int index;
	for ( index=0; index < list_length; index++)
	{
		char *packet_buffer = malloc( (packet_list[index].length+1) * sizeof(char));
		memcpy( packet_buffer, &input[packet_list[index].start_index], packet_list[index].length );
		packet_buffer[packet_list[index].length] = END;
		process_packet(packet_buffer, packet_list[index]);
		free(packet_buffer);
	}
	free(packet_list);
}

/**
 * parse the example input string
 */
void
run_example ()
{
	char *nmea_input = "$GPRMC,233738.00,A,3620.87249,S,17445.53457,E,114.272,65.40,090115,,,A*41\r\n\
$GPGGA,233738.00,3620.87249,S,17445.53457,E,1,07,1.48,455.2,M,29.4,M,,*48\r\n\
$GPGGA,233738.00,3620.87249,S,17445.53457,E,1,07,1.48,455.2,M,29.4,M,,*48\r\n\
$GPRMC,233738.00,A,3620.87249,S,17445.53457,E,114.272,65.40,090115,,,A*41\r\n\
$GPRMC,233738.00,A,3620.87249,S,17445.53457,E,114.272,65.40,090115,,,A*41\r\n\
$GPGGA,233738.00,3620.87249,S,17445.53457,E,1,07,1.48,455.2,M,29.4,M,,*48\r\n\
$PUBX,00,233738.00,3620.87249,S,17445.53457,E,484.613,G3,7.7,6.7,211.747,65.40,-0.187,,1.48,1.82,1.40,7,0,0*6C\r\n";

	packet_t* packet_list = malloc(PACKET_LIST_SIZE_MAX * sizeof(packet_list));
	int list_length = process_input(nmea_input, packet_list);
	printf("list length is: %d\n", list_length);
	int index;
	for (index=0; index < list_length; index++)
	{
		char *packet_buffer = malloc( (packet_list[index].length+1) * sizeof(char));
		memcpy( packet_buffer, &nmea_input[packet_list[index].start_index], packet_list[index].length );
		packet_buffer[packet_list[index].length] = END;
		process_packet(packet_buffer, packet_list[index]);
		free(packet_buffer);
	}
	free(packet_list);
}

/**
 * process the input string
 * put the start index and length of each packet into the packet list
 * output the length of the packet list
 */
static int
process_input (const char *in_str, packet_t* packet_list)
{
	int index, list_length;
	index = 0;
	list_length = 0;

	while(in_str[index] != END)
	{
		if (in_str[index] == DOLLAR)
		{
			packet_list[list_length].start_index = index+1;
		}
		else if (in_str[index] == STAR)
		{
			packet_list[list_length].length =
					index + 1 + CHECKSUM_LENGTH - packet_list[list_length].start_index;
			list_length++;
		}
		index++;
	}
	return list_length;
}

/**
 * process the packet
 *
 * match the command with the command list and print out basic info,
 * if the checksum is correct then parse the GPRMC packet
 */
static void
process_packet (char * buffer, packet_t packet)
{
    uint8_t index = 0;
    uint8_t count = 0;
    char command_name[MAX_NAME_SIZE] = {0};

    while ((buffer[index] != COMMA) && (buffer[index] != CR))
    {
        command_name[index] = buffer[index];
        index++;
    }
    command_name[index] = END;

    /* find the command name in the command list and if yes, parse the packet */
    for (count = 0; count < COMMAND_LIST_LENGTH; count++)
    {
        if (strncmp(command_list[count], command_name, index) == 0)
        {
            //print current packet
        	printf("current packet is: %s\n", buffer);
        	printf("%s START INDEX: %d LENGTH: %d ", command_name, packet.start_index, packet.length);
        	if (check_packet(buffer) == COMMAND_OK)
        	{
        		printf("Checksum Correct\n");
        		if (strncmp("GPRMC", command_name, index) == 0)
        			parse_packet(buffer, packet.length);
        	}
        	else
        		printf("Checksum Incorrect\n");
        }
    }
}

/**
 * parse and check checksum and return COMMAND_OK if OK
 */
uint8_t
check_packet (const char *packet)
{
    uint8_t calcdCkSum = 0;
    char calcdCkSumAsc[CHECKSUM_LENGTH + 1];

    /* calculate checksum not including '*' */
    while (*packet != STAR)
    {
        if (*packet == 0)
        {
            return ERR_COMMAND_INVALID;
        }
        calcdCkSum ^= *packet;
        packet++;
    }

    snprintf(calcdCkSumAsc, CHECKSUM_LENGTH + 1, "%02X", calcdCkSum);
    if (strncmp(calcdCkSumAsc, ++packet, CHECKSUM_LENGTH + 1) == 0)
    {
        return COMMAND_OK;
    }
    else
    {
        return ERR_COMMAND_CHECKSUM;
    }
}

/**
 * parse GPRMC packet
 */
static void
parse_packet (char *packet, int length)
{
	uint8_t n_vals, val_index;
	char *value;
	char *values[MAX_PACKET_PARTS];
	gps_rec_min_data_t *packet_data = malloc(sizeof(*packet_data));
	/* Get checksum from packet */
	char checksum[CHECKSUM_LENGTH + 1];
	memcpy( checksum, &packet[length-CHECKSUM_LENGTH], CHECKSUM_LENGTH );
	checksum[CHECKSUM_LENGTH] = END;
	packet_data->checksum = (uint8_t)strtol(checksum, NULL, 16);
	/* Skip command word, 6 characters (including ',') */
	packet += NMEA_PREFIX_LENGTH + 1;
	/* Remove checksum */
	packet[length - (NMEA_PREFIX_LENGTH + 1) - (CHECKSUM_LENGTH + 1)] = END;
	/* Split the sentence into values */
	n_vals = split_string_by_comma(packet, values, ARRAY_LENGTH(values));
	/* Loop through the values and parse them... */
	for (val_index = 0; val_index < n_vals; val_index++) {
		value = values[val_index];
		parse_gprmc(packet_data, value, val_index);
	}
	/* Print the obtained GPRMC structure */
	print_gprmc(packet_data);
	free(packet_data);
}

/**
 * Splits a string by comma.
 *
 * string is the string to split, will be manipulated. Needs to be
 *        null-terminated.
 * values is a char pointer array that will be filled with pointers to the
 *        splitted values in the string.
 * max_values is the maximum number of values to be parsed.
 *
 * Returns the number of values found in string.
 */
uint8_t
split_string_by_comma (char *string, char **values, uint8_t max_values)
{
	uint8_t index = 0;

	values[index++] = string;
	while (index < max_values && NULL != (string = strchr(string, COMMA))) {
		*string = END;
		values[index++] = ++string;
	}

	return index;
}

/**
 * parse GPRMC packet field by field index
 *
 * packet_data is the GPRMC packet to put the field data into.
 * value is the field data string to parse.
 * val_index is the corresponding field index.
 */
static void
parse_gprmc (gps_rec_min_data_t *packet_data, char *value, int val_index)
{
	switch (val_index)
	{
	case GPRMC_TIME:
		packet_data->utc_time = strdup(value);
		break;
	case GPRMC_STATUS:
		packet_data->status = value[0];
		break;
	case GPRMC_LATITUDE:
		sscanf(value, "%lf", &packet_data->latitude);
		break;
	case GPRMC_NS_INDICATOR:
		packet_data->ns_indicator = value[0];
		break;
	case GPRMC_LONGITUDE:
		sscanf(value, "%lf", &packet_data->longitude);
		break;
	case GPRMC_EW_INDICATOR:
		packet_data->ew_indicator = value[0];
		break;
	case GPRMC_SPEED_OVER_GROUD:
		packet_data->spd = strtof(value, NULL);
		break;
	case GPRMC_COURSE_OVER_GROUND:
		packet_data->cog = strtof(value, NULL);
		break;
	case GPRMC_DATE:
		packet_data->date = strdup(value);
		break;
	case GPRMC_MODE:
		packet_data->mode = value[0];
		break;
	default:
		break;
	}
}

/**
 * print GPRMC structure field by field
 */
void
print_gprmc (gps_rec_min_data_t *packet_data)
{
	printf("UTC TIME : %s\n", packet_data->utc_time);
	printf("STATUS : %c\n", packet_data->status);
	printf("LATITUDE : %.5f\n", packet_data->latitude);
	printf("N/S INDICATOR : %c\n", packet_data->ns_indicator);
	printf("LONGITUDE : %.5f\n", packet_data->longitude);
	printf("E/W INDICATOR : %c\n", packet_data->ew_indicator);
	printf("SPEED OVER GROUND : %.3f\n", packet_data->spd);
	printf("COURSE OVER GROUND : %.3f\n", packet_data->cog);
	printf("DATE : %s\n", packet_data->date);
	printf("MAGNETIC VARIATION : 0\n");
	printf("MAGNETIC VARIATION E/W : 0\n");
	printf("MODE INDICATOR : %c\n", packet_data->mode);
	printf("CHECKSUM: %x\n", packet_data->checksum);
}
