

#define ADDRESS 49
#define T_LOW 4b
#define T_HIGH 50
#define GPIO_PIN 30


void main( void ) {
	
	//Sets control register to interrupt mode	
	system("i2cset -y 1 0x49 1 82");
	//Sets lower temperature threshold (hex)
	system("i2cset -y 2 0x49 2 0x4b");
	//Sets higher temperature threshold (hex)
	system("i2cset -y 3 0x49 3 0x50");
	//Sets GPIO interrupt polling to pin 30
	system("./gpioIntHandler 30");

}



