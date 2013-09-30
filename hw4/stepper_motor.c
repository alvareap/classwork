
#include <htc.h>  //Note the use of the CONFIG statement measn we NEED NOT set the PC16F887 configuration bits manually!
__CONFIG(FOSC_INTRC_NOCLKOUT & WDTE_OFF & PWRTE_ON & MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF & IESO_OFF & FCMEN_OFF & LVP_OFF & 
         DEBUG_ON & BOR4V_BOR21V & WRT_OFF);
void stepcw(void);
void stepccw(void);
void cw_to_ccw(void);
void ccw_to_cw(void);

unsigned int ADC_convert(unsigned char );

signed char step_index = 0;
const unsigned char step_tbl[4]={0b0011,0b0110,0b1100,0b1001};

#define clockwiseTurn 1
#define counterClockwiseTurn 0

char lastTurn=0;

void main(void){
    unsigned long int j;
  	unsigned char i;
	TRISD = 0b11110000;	//Make RD3:0 outputs (LEDs connected to RD3:0)
	TRISB = 0b11111111; ANS12=0;
	nRBPU = 0; //Enable pull ups on PORT B
	ADON=1;//turn on Analog-Digital Converter Module
	ADFM=1; //Right Justify result in ADRES= ADRESH:ADRESL registers
	VCFG1=0; VCFG0=0; //Use VSS and VDD to set conversion rate to ~0V-5V
	ADCS1=0; ADCS0=1; //ADC clock freq to Fosc/8=> 500kHz

	TRISA0= 1; TRISA1= 1;
	ANS0= 1; ANS1=1;	
	unsigned int voltages[20];
	unsigned int lowestVoltage= 0xff;
	char lowestVoltageIndex;


	while (RB4==1); //hold here until start press.

	for(int i=0; i<20; i++){
		unsigned int rightLED = ADC_convert(0);//get rightLED values
		if(rightLED < lowestVoltage){  //if rightLED is the least voltage
			lowestVoltage=rightLED;		//rightLED voltage is least voltage
			lowestVoltageIndex=i;		//remember index of said voltage
		}
		voltages[i]= rightLED;		//store voltage in array
		stepcw();					//move on to next position
		
	}
	cw_to_ccw();

	for(int i=0; i< (19-lowestVoltageIndex); i++){
		stepccw();
	}

	while(1) {
		unsigned int rightLED = ADC_convert(0); //gets value from IR LED1
		for(long i=0; i< 1000;i++); //delay
		unsigned int leftLED = ADC_convert(1); //gets value from IR LED2
	 	
		if(leftLED > rightLED){ //Left LED higher voltage, less light
			if(lastTurn== counterClockwiseTurn){
				ccw_to_cw(); //enable to change direction
			}
			stepcw();			//rotate clockwise
			lastTurn= clockwiseTurn;
		} else {
			if(lastTurn== clockwiseTurn){
				cw_to_ccw(); //enable to change direction
			}
			stepccw();			//rotate counterClockwise
			lastTurn= counterClockwiseTurn;
		}
	}
}

void stepcw()
{	unsigned int count;					 		
	PORTD = step_tbl[step_index];			//Send out the next step code from table
	step_index++;							//Step upward through the stepping seq table
	if (step_index == 4) step_index = 0;	//Wrap when index gets up to 4
	for(count=0;count<15000;count++);		//This delay loop sets the step time
}
void stepccw()
{	unsigned int count;
	PORTD = step_tbl[step_index];			//Send out the next step code from table.
	step_index--;							//Step downward through the stepping seq table						
	if (step_index == -1) step_index = 3;	//Wrap when index gets down to -1
	for(count=0;count<15000;count++);		//This delay loop sets the step time
}
void cw_to_ccw()							//Index pointer needs to be backed up
{	step_index--;							//When changing direction cw to ccw.
	if (step_index==-1) step_index = 3;
}
void ccw_to_cw()							//Index pointer needs to be moved forward
{	step_index++;							//when changing direction ccw to cw
	if (step_index==4) step_index = 0;
}

unsigned int ADC_convert(unsigned char channel_number){
	CHS3=0;CHS2=0; CHS1=0; CHS0=0;
	if(channel_number & 0b1000){
		CHS3=1;
	}
	if(channel_number & 0b0100){
		CHS2=1;
	}
	if(channel_number & 0b0010){
		CHS1=1;
	}
	if(channel_number & 0b0001){
		CHS0=1;
	}
	GO = 1;				//Start conversion ("GO" is the GO/DONE* bit)
	while(GO==1); 		//Wait here while converting
	return ADRESH*256+ADRESL; //scales high ADRESH by 256, then adds lower register
}