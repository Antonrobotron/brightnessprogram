/*
 * program: brightness
 * descrip: Program that changes brightness of laptop
 *  author: AntonRobotron
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAXBRIGHT "/sys/class/backlight/intel_backlight/max_brightness"
#define BRIGHTNESS "/sys/class/backlight/intel_backlight/brightness"

void helpPrompt(const char *);
int readTillNewLine(char [], int);
void itoa(char [], const int);

int main(int argc, char *argv[]) {
	//check for all the parameters needed
	if (argc > 3) {
		helpPrompt(argv[0]);
		exit(0);
	}
	else if(argc == 2 && strcmp(argv[1], "get")) {
		helpPrompt(argv[0]);
		exit(0);
	}
	else if(argc < 2) {
		helpPrompt(argv[0]);
		exit(0);
	}

	//init variables
	int fd;
	char buffer[100];
	int brightnessNew;

	//Read and Set the max Brightness
	fd = open(MAXBRIGHT, O_RDONLY);
	readTillNewLine(buffer, fd);
	const int brightnessMax = atoi(buffer);
	close(fd);

	//Read and Set the current brightness
	fd = open(BRIGHTNESS, O_RDONLY);
	readTillNewLine(buffer, fd);
	const int brightnessCurrent = atoi(buffer);
	close(fd);

	//if using get, print the current and max brightness, then exit
	if(!strcmp(argv[1], "get")) {
		printf("%d/%d\n", brightnessCurrent, brightnessMax);
		exit(0);
	}

	//set new brightness
	if(!strcmp(argv[2], "25%"))
		brightnessNew = brightnessMax/4;
	else if(!strcmp(argv[2], "50%"))
		brightnessNew = brightnessMax/2;
	else if(!strcmp(argv[2], "100%"))
		brightnessNew = brightnessMax;
	else if(strcmp(argv[1], "get"))
		brightnessNew = atoi(argv[2]);

	//if statements to check for add, sub, or set
	if (!strcmp(argv[1], "add"))
		brightnessNew += brightnessCurrent;
	else if(!strcmp(argv[1], "sub")) 
		brightnessNew = brightnessCurrent - brightnessNew;
//	else if(!strcmp(argv[1], "set"))
//	since V1.1 checking for set is no longer needed
//	because brightnessNew is set before checking for 2nd value

	//check to make sure its above 0 and below max
	if(brightnessNew < 1) {
		printf("New Brightness is below min! setting to min instead.\n");
		brightnessNew = 1;
	}
	else if(brightnessNew > brightnessMax) {
		printf("New Brightness is above max! setting to max instead.\n");
		brightnessNew = brightnessMax;
	}

	//open and write
	if((fd = open(BRIGHTNESS, O_WRONLY)) == -1)
		printf("ERROR! Brightness File Not Accesible, double check you have access to write, and that the path is correct.\n");
	else {
		itoa(buffer, brightnessNew);
		write(fd, buffer, strlen(buffer));
		close(fd);
	}

	return 0;
}

//Usage Message if someone does something wrong
void helpPrompt(const char *str) {
	printf("Usage: %s (add/sub/set) [amount]\n", str);
	printf("Alternate use is to see the current brightness with:\n");
	printf("%s get\n", str);
	printf("%s is a command for arch linux that changes computer brightness\n", str);
	printf("%s will never allow for 0 brightness.\n", str);
	printf("Amount can be replaced with 25%%, 50%%, or 100%%\n");
}

//Function that reads all data from a file until theres a new line
//returns how many chars were read
int readTillNewLine(char buf[], int fd) {
	char byte = 'E';
	int i;

	for(i = 0; read(fd, &byte, 1) == 1; i++)
		buf[i] = byte;
	buf[i+1] = '\0';

	return i;
}

//Function that converts and int to a string 
//using mod and integer division
void itoa(char buf[], const int val) {
	int i = 0, x = val;

	for(i = 0; x > 0; i++)
		x /= 10;

	x = val;
	buf[i--] = '\0';
	for(; i >=0; i--) {
		//convert the ones place of x to char
		//48 converts an int to it's char value
		buf[i] = (char)((x%10) + 48);
		x /= 10;
	}
}
