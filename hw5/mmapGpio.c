#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <signal.h>    // Defines signal-handling functions (i.e. trap Ctrl-C)
#include "beaglebone_gpio.h"


#define GPIO0_START_ADDR 0x44E07000
#define GPIO1_START_ADDR 0x4804C000
#define GPIO2_START_ADDR 0x481AC000

#define GPIO0_END_ADDR	0x44E0_7FFF
#define GPIO1_END_ADDR	0x4804CFFF
#define GPIO2_END_ADDR	0x481ACFFF

#define GPIO0_SIZE (GPIO2_END_ADDR - GPIO2_START_ADDR)
#define GPIO1_SIZE (GPIO1_END_ADDR - GPIO1_START_ADDR)
#define GPIO2_SIZE (GPIO2_END_ADDR - GPIO2_START_ADDR)


#define GPIO_DATAOUT 0x13C
#define GPIO_DATA_IN 0x138
#define GPIO_CLEARDATAOUT 0x190

#define USR3 (1<<24)
#define USR2 (1<<23)
#define USR1 (1<<22)
#define USR0 (1<<20)

#define ADDR_ADD( addr, offset) (addr + offset)
#define ADDR_DIF( addr1, addr2) ( addr1 - addr2)

volatile void *gpio0_addr;
volatile unsigned int *gpio0_setdataout_addr;
volatile unsigned int *gpio0_cleardataout_addr;

volatile void *gpio1_addr;
volatile unsigned int *gpio1_setdataout_addr;
volatile unsigned int *gpio1_cleardataout_addr;
volatile unsigned int *gpio1_readdatain_addr;

volatile void *gpio2_addr;
volatile unsigned int *gpio2_setdataout_addr;
volatile unsigned int *gpio2_cleardataout_addr;
volatile unsigned int *gpio2_readdatain_addr;

void main( void )
{

	
  int fd = open("/dev/mem", O_RDWR);
  gpio1_addr = mmap(0, GPIO1_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_START_ADDR);

  gpio1_setdataout_addr   = gpio1_addr + GPIO_SETDATAOUT;
  gpio1_cleardataout_addr = gpio1_addr + GPIO_CLEARDATAOUT;
  gpio1_readdatain_addr = gpio1_addr + GPIO_DATA_IN;
	
  //GPIO2
  gpio2_addr = mmap(0, GPIO2_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO2_START_ADDR);

  gpio2_setdataout_addr   = gpio2_addr + GPIO_SETDATAOUT;
  gpio2_cleardataout_addr = gpio2_addr + GPIO_CLEARDATAOUT;
  gpio2_readdatain_addr = gpio2_addr + GPIO_DATA_IN;
  



  while(1)
  {
    if ( (*gpio2_readdatain_addr & 0x00000008) == 0x00000000 )
    {
      *gpio1_setdataout_addr = USR3;
    }
	//0x3E030300

    if ( (*gpio1_readdatain_addr & 0x00020000) == 0x00020000 )
    {
      *gpio1_setdataout_addr = USR1;
    }

    system("sleep 1");

    *gpio1_cleardataout_addr = ( USR3 | USR1 );

    
   }


}



