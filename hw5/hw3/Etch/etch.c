#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <ncurses.h>
#include<signal.h>
#include<unistd.h>

//Matrix includes
#include <errno.h>
#include <string.h>

#include "i2c-dev.h"
#include "i2cbusses.h"
#include "gpio-utils.h"

#define BOARD_HEIGHT	8
#define BOARD_WIDTH	8
#define REFRESH_RATE	250000
#define BLINK_RATE	200000

#define BICOLOR		// undef if using a single color display


////////////////////////////////////////////////////////////
// External Functions
////////////////////////////////////////////////////////////
extern int gpio_export(unsigned int gpio);
extern int gpio_unexport(unsigned int gpio);
extern int gpio_set_dir(unsigned int gpio, const char* dir);
extern int gpio_set_value(unsigned int gpio, unsigned int value);
extern int gpio_get_value(unsigned int gpio, unsigned int *value);
extern int gpio_set_edge(unsigned int gpio, const char *edge);
extern int gpio_fd_open(unsigned int gpio, unsigned int dir);
extern int gpio_fd_close(int fd);


// The upper btye is RED, the lower is GREEN.
// The single color display responds only to the lower byte
static __u16 smile_bmp[]=
	{0x3c, 0x42, 0xa9, 0x85, 0x85, 0xa9, 0x42, 0x3c};
static __u16 frown_bmp[]=
	{0x3c00, 0x4200, 0xa900, 0x8500, 0x8500, 0xa900, 0x4200, 0x3c00};
static __u16 neutral_bmp[]=
	{0x3c3c, 0x4242, 0xa9a9, 0x8989, 0x8989, 0xa9a9, 0x4242, 0x3c3c};
static __u16 blankLEDS[]=
	{0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static __u16 matrixLED[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static __u16 matrixLEDtemp[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

int etchBoard[BOARD_HEIGHT][BOARD_WIDTH] = { {0} };

typedef struct Board_t {
  int X;
  int Y;
} Board_t;

/////////////////////////
// Static Variables
static int file;
static int buttonPress;
/////////////////////////////////////////////////////////////////
// Local Function Prototypes
////////////////////////////////////////////////////////////////

char getcha(void);

int matrix_init( void );
void matrix_refresh( void );
void printMatrixToConsole( void );

static void help(void) __attribute__ ((noreturn));

static void help(void) {
	fprintf(stderr, "Usage: matrixLEDi2c (hardwired to bus 3, address 0x70)\n");
	exit(1);
}



void signal_handler(int signal){

	if (signal == SIGINT){
  		gpio_unexport(44);
  		gpio_unexport(45);
  		gpio_unexport(23);
		gpio_unexport(26);
	
	}

}



void main( void ){
  int inKey;
  static Board_t map;

  map.X = BOARD_WIDTH/2;
  map.Y = BOARD_HEIGHT/2;
  

  matrix_init();

  
  //GPIO inits
  gpio_export(44);
  gpio_export(45);
  gpio_export(23);
  gpio_export(26);


  gpio_set_dir(44, "in");
  gpio_set_dir(45, "in");
  gpio_set_dir(23, "in");
  gpio_set_dir(26, "in");
  unsigned int gpio_U, gpio_D, gpio_L,  gpio_R;

  while(1){
	    
    	buttonPress=0;
	// int gpio_get_value(unsigned int gpio, unsigned int *value)
   	
	gpio_get_value( 44, &gpio_L);
	gpio_get_value( 45, &gpio_U);
	gpio_get_value( 23, &gpio_D);
	gpio_get_value( 26, &gpio_R);	
	
	
	if (gpio_L){
		matrixLED[map.Y] |= ( 0x01 << (7-map.X) ); //make sure light remains on when moving away.
		if (map.X) // X positon is nonZero, move left (boundary detection)
		  map.X -= 1;
		//printf("L, ( %d, %d )\n", map.X, map.Y);
		buttonPress=1;

	} else if (gpio_R)
	{	
		matrixLED[map.Y] |= ( 0x01 << (7-map.X) ); //make sure light remains on when moving away.
		if (map.X < (BOARD_WIDTH-1) ) // X positon is less than width, move right (boundary detection)
		  map.X += 1;
		//printf("R, ( %d, %d )\n", map.X, map.Y);
		buttonPress=1;

	} else if (gpio_U)
	{	
		matrixLED[map.Y] |= ( 0x01 << (7-map.X) ); //make sure light remains on when moving away.
		if (map.Y) // Y positon is nonZero, move up (boundary detection)
		  map.Y -= 1;
		//printf("U, ( %d, %d )\n", map.X, map.Y);
		buttonPress=1;

	} else if (gpio_D)
	{	
		matrixLED[map.Y] |= ( 0x01 << (7-map.X) ); //make sure light remains on when moving away.
		if (map.Y < (BOARD_HEIGHT-1) ) // Y positon is less than height, move down (boundary detection)
		  map.Y += 1;
		//printf("D, ( %d, %d )\n", map.X, map.Y);
		buttonPress=1;
	}




  	//printf("%d\n", buttonPress);

	if (!buttonPress){
		matrixLEDtemp[map.Y] ^= matrixLED[map.Y];
		matrixLED[map.Y] ^= ( 0x01 << (7-map.X) );
		
	} else {
	  matrixLED[map.Y] = matrixLED[map.Y] | ( 0x01 << (7-map.X) );
	}
	
	

	matrix_refresh();
  }



}



char getcha() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}

/*
    my2cset.c - First try at controlling Adafruit 8x8matrix.
    Mark A. Yoder, 14-Aug-2012.
    Mark A. Yoder, 26-Oct-2012.  Cleaned up.
    Page numbers are from the HT16K33 manual
    http://www.adafruit.com/datasheets/ht16K33v110.pdf
*/
/*
    i2cset.c - A user-space program to write an I2C register.
    Copyright (C) 2001-2003  Frodo Looijaard <frodol@dds.nl>, and
                             Mark D. Studebaker <mdsxyz123@yahoo.com>
    Copyright (C) 2004-2010  Jean Delvare <khali@linux-fr.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA 02110-1301 USA.
*/




static int check_funcs(int file) {
	unsigned long funcs;

	/* check adapter functionality */
	if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
		fprintf(stderr, "Error: Could not get the adapter "
			"functionality matrix: %s\n", strerror(errno));
		return -1;
	}

	if (!(funcs & I2C_FUNC_SMBUS_WRITE_BYTE)) {
		fprintf(stderr, MISSING_FUNC_FMT, "SMBus send byte");
		return -1;
	}
	return 0;
}

// Writes block of data to the display
static int write_block(int file, __u16 *data) {
	int res;
#ifdef BICOLOR
	res = i2c_smbus_write_i2c_block_data(file, 0x00, 16, 
		(__u8 *)data);
	return res;
#else
/*
 * For some reason the single color display is rotated one column, 
 * so pre-unrotate the data.
 */
	int i;
	__u16 block[I2C_SMBUS_BLOCK_MAX];
//	printf("rotating\n");
	for(i=0; i<8; i++) {
		block[i] = (data[i]&0xfe) >> 1 | 
			   (data[i]&0x01) << 7;
	}
	res = i2c_smbus_write_i2c_block_data(file, 0x00, 16, 
		(__u8 *)block);
	return res;
#endif
}


int matrix_init( void )
{
	int res, i2cbus, address, file;
	char filename[20];
	int force = 0;

	i2cbus = lookup_i2c_bus("1");
	//printf("i2cbus = %d\n", i2cbus);
	if (i2cbus < 0)
		help();

	address = parse_i2c_address("0x70");
	//printf("address = 0x%2x\n", address);
	if (address < 0)
		help();

	file = open_i2c_dev(i2cbus, filename, sizeof(filename), 0);
//	printf("file = %d\n", file);
	if (file < 0
	 || check_funcs(file)
	 || set_slave_addr(file, address, force))
		exit(1);

	// Check the return value on these if there is trouble
	i2c_smbus_write_byte(file, 0x21); // Start oscillator (p10)
	i2c_smbus_write_byte(file, 0x81); // Disp on, blink off (p11)
	i2c_smbus_write_byte(file, 0xe7); // Full brightness (page 15)

//	Clear display
	write_block(file, blankLEDS);
	return 0;
}

void matrix_refresh( void ){

	int res, i2cbus, address, file;
	char filename[20];
	int force = 0;

	i2cbus = lookup_i2c_bus("1");
	//printf("i2cbus = %d\n", i2cbus);
	if (i2cbus < 0)
		help();

	address = parse_i2c_address("0x70");
	//printf("address = 0x%2x\n", address);
	if (address < 0)
		help();

	file = open_i2c_dev(i2cbus, filename, sizeof(filename), 0);
//	printf("file = %d\n", file);
	if (file < 0
	 || check_funcs(file)
	 || set_slave_addr(file, address, force))
		exit(1);

	// Check the return value on these if there is trouble
	i2c_smbus_write_byte(file, 0x21); // Start oscillator (p10)
	i2c_smbus_write_byte(file, 0x81); // Disp on, blink off (p11)
	i2c_smbus_write_byte(file, 0xe7); // Full brightness (page 15)


//	Write Matrix
	write_block(file, matrixLED);

	if(buttonPress){
	  usleep(REFRESH_RATE);
	} else {
	  usleep(BLINK_RATE);
	}
}

void printMatrixToConsole( void ){
	int col;

		  for (col = 0; col < (BOARD_HEIGHT -1); col++ )
		  {
		   printf("%x\n",  ( matrixLED[col] ) );
		  }
		  
	printf("\n");
	
	
	


}


