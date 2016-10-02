This C project contains a nmea_parser.c file and nmea_parser.h file.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USAGE:

To run the project first import the project into eclipse then click run.
You can also import the code to other IDE since it only contains two files.
The program will take the example input string and parse it by default.
You can comment the "run_example();" and uncomment the "user_input();" to run the user input mode.
But since the user input cannot take multiple line string, it does not simulate the example scenario.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FUNCTION DESCRIPTION

The code should be pretty much self-explanatory, so just some basic info here.

nmea_parser.c file function list:

1 void user_input ():
	This function takes the user input and perform the packet selecting and parsing.
2 void run_example ():
	this function takes the example input string and parse its content.
	The function first parse the input string into a list of packets,
	then parse the packet one by one.
3 static int process_input (const char *in_str, packet_t* packet_list):
	This function parse a given input string into a list of packets,
	then returns the list length.
4 static void process_packet (char * buffer, packet_t packet):
	This function takes a packet string as input, process it and print out its info.
	It matches the command found in the packet with the command list and print out basic info,
	if the checksum is correct then parse the GPRMC packet.
	The 'length' printed out here is counted from the first character in the packet to the end of checksum.
	It excludes the '$' character.
	I pass the packet struc as the input since it's more readable this way.
5 uint8_t check_packet (const char *packet):
	This function check the checksum of the given input string and return its correctness.
	Since this function is quite useful, I didn't define it as local function.
6 static void parse_packet (char *packet, int length):
	This function parse the given GPRMC packet string and make use of the following three functions:
	"split_string_by_comma", "parse_gprmc" and "print_gprmc".
7 uint8_t split_string_by_comma (char *string, char **values, uint8_t max_values):
	This function split a string to array of strings by delimiter comma.
	Since this function is quite useful, I didn't define it as local function.
8 static void parse_gprmc (gps_rec_min_data_t *packet_data, char *value, int val_index):
	This function parse the given GPRMC packet data field by field index
9 void print_gprmc (gps_rec_min_data_t *packet_data):
	This function prints out a GPRMC data structure.
	Since this function could be quite useful, I didn't define it as local function.

nmea_parser.h file structure description:

1 packet_t:
	This is the packet identifier structure for the input string.
	Note the 'length' here is counted from the first character in the packet to the end of checksum.
	It excludes the '$' character.
2 gps_rec_min_data_t:
	This is the GPRMC data structure.
	'latitude' and 'longitude' have to be double since float is not enough.
	'utc_time' and 'date' have to be character array since their formats are not convertable.
3 gps_fix_data_t:
	This is the GPGGA data structure.
	This structure has not been tested since it's not required by the test.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COMMENT ON THE CODE ROBUSTNESS

I have tried to define every meaningful numeric constant to a macro to make the code more readable.
The functions "check_packet" and "split_string_by_comma" must be pretty robust since their usage are well defined.
The functions "parse_gprmc" and "print_gprmc" have limitations on their scenario since I can't expect all receivers to not 
output magnetic variation field.

The "user_input" function cannot take multiple line string as input while in real life the data often comes from a serial comm,
and it will definitely contain <CR><LF> characters. Therefore it is not useful since it doesn't present the scenario.
The "run_example" function parse a given sample string which simulate a serial comm data stream. This function can be treated as
a unit test to the other functions.

The "process_input" function can only deal with formatted packet string, if the data is corrupted somehow then this function may
fail.
The "process_packet" function should be quite robust since it goes through several checks before it pass the data to the
parsing function. But again, it cannot handle a random string.

To be honest, the real structure should not be obtaining the start index and length first, we need to check the checksum first.
So the real run sequence should be: obtaining packet -> check checksum -> check command list -> parse packet.

