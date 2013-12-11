/*
    Minimal version for teaching.  Mark A. Yoder 26-July-2011
    i2cget.c - A user-space program to read an I2C register.
    Copyright (C) 2005-2010  Jean Delvare <khali@linux-fr.org>

    Based on i2cset.c:
    Copyright (C) 2001-2003  Frodo Looijaard <frodol@dds.nl>, and
                             Mark D. Studebaker <mdsxyz123@yahoo.com>
    Copyright (C) 2004-2005  Jean Delvare <khali@linux-fr.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.data

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA 02110-1301 USA.
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "i2c-dev.h"
#include <fcntl.h>

void endiannessSwap( int * num );
int i2cReadWord( char* pfilename, int * i2cInfo );


int main(int argc, char *argv[])
{
	char *end;
	int res;

	int i2cInfo[] = {0,0,0,0}; // i2cBus, i2cAddr, i2cReg, i2cData
	int i2cbus,  size, file;
	int address; 
	int daddress;
	char filename[20];

	/* handle (optional) flags first */
	if(argc < 3) {
		fprintf(stderr, "Usage:  %s <i2c-bus> <i2c-address> <register>\n", argv[0]);
		exit(1);
	}

	//i2cbus   = atoi(argv[1]);
	//address = strtol(argv[2], NULL , 16); // accepts hex format
	//daddress = strtol(argv[3], NULL , 16);
	size = I2C_SMBUS_BYTE;

	*(i2cInfo)  = atoi(argv[1]);	// i2cbus
	*(i2cInfo+1) = strtol(argv[2], NULL , 16); // i2c address
	*(i2cInfo+2) = strtol(argv[3], NULL , 16);  // register address

	i2cReadWord( filename, i2cInfo);

	printf("0x%02x\n", *(i2cInfo+3));

	exit(0);
}

int i2cReadWord( char* pfilename, int* i2cInfo){
	int file;
	sprintf( pfilename, "/dev/i2c-%d", *(i2cInfo) );
	file = open(pfilename, O_RDWR);
	if (file<0) {
		if (errno == ENOENT) {
			fprintf(stderr, "Error: Could not open file "
				"/dev/i2c-%d: %s\n", *(i2cInfo+1), strerror(ENOENT));
		} else {
			fprintf(stderr, "Error: Could not open file "
				"`%s': %s\n", pfilename, strerror(errno));
			if (errno == EACCES)
				fprintf(stderr, "Run as root?\n");
		}
		exit(1);
	}

	if (ioctl(file, I2C_SLAVE, *(i2cInfo+1) ) < 0) { // address
		fprintf(stderr,
			"Error: Could not set address to 0x%02x: %s\n",
			*(i2cInfo+1), strerror(errno));
		return -errno;
	}

	*(i2cInfo+3) = i2c_smbus_read_word_data(file, *(i2cInfo+2) );
	close(file);

	if ( *(i2cInfo+3) < 0) {
		fprintf(stderr, "Error: Read failed, res=%d\n", *(i2cInfo+3) );
		exit(2);
	}
	endiannessSwap( (i2cInfo+3) );

}



// Swaps endianness of 16bit number on a 32 bit system
void endiannessSwap( int * num ){

	*num = ( (int) (*num)>>8) | ( (int) ( (*num) <<8 )  & 0x0000FFFF);

}
