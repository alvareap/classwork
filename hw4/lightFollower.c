#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h> // For usleep()
#include <signal.h> // Defines signal-handling functions (i.e. trap Ctrl-C)
#include "gpio.h"

#define PIN_MUX_PATH "/sys/kernel/debug/omap_mux/"
#define MAX_BUF 64

/*Created by Chris Andrews and Alex Alvarez, heavily based off of LightTracker.c

./lightFollower <toggle_button> <AINPin1> <AINPin2>

Both AIN pins are given a default value in case none is entered. The default
pins are AIN0 and AIN1
*/

//Globals
int controlGPIOs[5];
char ain[2];

//*****************BEGIN Functions taken from LightTracker******************
//Sets the pin muxes on the gpios
int mode_gpio_out(char *pinMux){
	int fd, len;
	char buf[MAX_BUF];
 
	len = snprintf(buf, sizeof(buf), PIN_MUX_PATH "%s", pinMux);
 
	fd = open(buf, O_WRONLY);
	if (fd < 0){
		perror("mode/gpio");
		return fd;
	}
 
	write(fd, "7", 2); 
	close(fd);
	return 0;
}

//Reads analog input
int analogIn(char ainn)
{
	FILE *fp;
	char ainPath[MAX_BUF];
	char ainVal[MAX_BUF];
	
    snprintf(ainPath, sizeof ainPath, "/sys/devices/ocp.2/helper.14/AIN%c", ainn);

	if((fp = fopen(ainPath, "r")) == NULL){
	printf("Can't open this pin, %c\n", ainn);
	return 1;
	}

	fgets(ainVal, MAX_BUF, fp);

	fclose(fp);
	return atoi(ainVal);		
}

//Even though it's annoying to be locked into these 4 gpio pins, the fact
//that they'll always work without physical adjustment is a nice feature
void initIO(){
	//Mode0 names
	char gpio1[] = "gpmc_wait0";
	char gpio2[] = "gpmc_wpn";
	char gpio3[] = "gpmc_a0";
	char gpio4[] = "spi0_cs0";

	int i;

	//Set pin mux in gpio output mode for controlGPIOss
#ifdef SET_PIN_MUX
mode_gpio_out(gpio1);
	mode_gpio_out(gpio2);
	mode_gpio_out(gpio3);
	mode_gpio_out(gpio4);
#endif

	//Export gpios and set up output direction for controlGPIOss
	for (i = 0; i < 5; i++){
		gpio_export(controlGPIOs[i]);
		gpio_set_direction(controlGPIOs[i], 1);
	}
	//Toggle switch gpio
	printf("%d\n", controlGPIOs[4]);
	gpio_export(controlGPIOs[4]);
	gpio_set_direction(controlGPIOs[4], 0);
	
}

//Rotate clockwise
void rotateClock (int current){
	//Use a 4 state position to decide how to rotate
	switch(current){
	case 0:	gpio_set_value(controlGPIOs[0], 0);
			gpio_set_value(controlGPIOs[1], 1);
			gpio_set_value(controlGPIOs[2], 1);
			gpio_set_value(controlGPIOs[3], 0);
			break;

	case 1:	gpio_set_value(controlGPIOs[0], 1);
			gpio_set_value(controlGPIOs[1], 1);	
			gpio_set_value(controlGPIOs[2], 0);
			gpio_set_value(controlGPIOs[3], 0);
			break;

	case 2:	gpio_set_value(controlGPIOs[0], 1);
			gpio_set_value(controlGPIOs[1], 0);
			gpio_set_value(controlGPIOs[2], 0);
			gpio_set_value(controlGPIOs[3], 1);
			break;

	case 3:	gpio_set_value(controlGPIOs[0], 0);
			gpio_set_value(controlGPIOs[1], 0);
			gpio_set_value(controlGPIOs[2], 1);
			gpio_set_value(controlGPIOs[3], 1);
			break;
	}
	usleep(60000);

}
//*****************END Functions taken from LightTracker******************


//MAIN function.
int main(int argc, char *argv[]){
		controlGPIOs[0] = 23;
		controlGPIOs[1] = 47;
		controlGPIOs[2] = 26;
		controlGPIOs[3] = 46;
	//setup Analog IO's
	system("SLOTS=/sys/devices/bone_capemgr.*/slots");
	system("PINS=/sys/kernel/debug/pinctrl/44e10800.pinmux/pins");
	system("echo cape-bone-iio > $SLOTS");

	int i, k, samples[20], min, toggle, minIndex, contine, counter, minFound;
	minFound = 0;
	min = 99999;
	counter = 0;
	contine = 1;
	if(argc < 3){
		printf("Fewers than 2 arguments detected: using default values.\n");
		ain[0] = '0';
		ain[1] = '1';
		controlGPIOs[4] = 65;
	}else{
		ain[0] = argv[2][0];
		ain[1] = argv[3][0];
		controlGPIOs[4] = atoi(argv[1]);
	}

	//Initialize IO
	initIO();

while(contine){	
	gpio_get_value(controlGPIOs[4], &toggle);
	//if toggle pressed, rotate once then counter-rotate to light source
	if(toggle == 0){
	//Clockwise rotate for a cycle and record the value in different directions
		for(i=0; i<5; i++){
			for(k=0; k<4; k++){
				rotateClock(k);
				samples[counter] = analogIn(ain[0])+analogIn(ain[1]);
				if(min>samples[counter]){
				minIndex=counter;min=samples[counter];}
				counter++;
			}
		}
		
		printf("%d\n", minIndex);
		counter = 0;
		
		for(k=0; k<((20-minIndex)/4); k++){
			for(i=0; i<4; i++){
				rotateClock(3-i);
				counter++;
			}
		}
		printf("%d\n", counter);
		for(i=0; i<((20-minIndex)%4); i++){
			rotateClock(3-i);
			counter++;
		}
		printf("here%d\n", counter);
		minFound = 1;
	}
	//Track light source
	if(minFound == 1){
		//Moving Left and Right
		if((analogIn(ain[0])-analogIn(ain[1])>150)){
			rotateClock(0);
			rotateClock(1);
			rotateClock(2);
			rotateClock(3);
			printf("Should move Clockwise");
		}else if((analogIn(ain[0])-analogIn(ain[1])<-150)){
			rotateClock(3);
			rotateClock(2);
			rotateClock(1);
			rotateClock(0);
			printf("Should move Counterclockwise");
		}
		//printf("Ain[0] =%d\nAin[1]=%d",  analogIn(ain[0]), analogIn(ain[1]) );
		fflush(stdout);
		usleep(2000000);
	}
}
return 0;
}


