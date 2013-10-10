

#define ADDRESS 49
#define T_LOW 4b
#define T_HIGH 50
#define GPIO_PIN 30


void main( void ) {
	
	//Sets control register to interrupt mode	
	system("i2cset -y 1 0x49 1 0x82");
	//Sets lower temperature threshold (hex)
	system("i2cset -y 1 0x49 2 0x30"); // 69 degrees F
	//Sets higher temperature threshold (hex)
	system("i2cset -y 1 0x49 3 0x4B"); //  72 degrees F
	//Sets GPIO interrupt polling to pin 30
	system("./gpio-int-test 30"); 

}



