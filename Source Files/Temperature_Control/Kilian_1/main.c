#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "LCD_16x2_H_file.h"
#include "LCD_16x2_C_file.c"
#define degree_sysmbol 0xdf

typedef struct MODE{
	float min_val;
	float max_val;
}MODE;

MODE IPT;

void ADC_Init();
int ADC_Read(char channel);
void BoardInit(void);
int WhichRange(float temperature,MODE ipt);
void heating(void);
void cooling(void);
void system_init(void);
float sensing(void);
void temperature_displaying(float temperature);
void temperature_control(float temperature, MODE ipt);


int main()
{
	system_init();
	while(1)
	{
		temperature_displaying(sensing());
		temperature_control(sensing(),IPT);
	}
	return 0;
}

void ADC_Init(){
	DDRA = 0x00;							        /* Make ADC port as input */
	ADCSRA = 0x87;									/* Enable ADC, with freq/128  */
	ADMUX = 0x40;									/* Vref: Avcc, ADC channel: 0 */
}

int ADC_Read(char channel)
{
	ADMUX = 0x40 | (channel & 0x07);				/* set input channel to read */
	ADCSRA |= (1<<ADSC);							/* Start ADC conversion */
	while (!(ADCSRA & (1<<ADIF)));					/* Wait until end of conversion by polling ADC interrupt flag */
	ADCSRA |= (1<<ADIF);							/* Clear interrupt flag */
	_delay_ms(1);									/* Wait a little bit */
	return ADCW;									/* Return ADC word */
}

//This function is used to initialize the board ATmega16
void BoardInit(void){
	ADC_Init(); //Initializes the ADC system
	LCD_Init();//Initializes the LCD displaying system
	DDRD=0xFF; //DDRD is initialized to receive output ports
}

/*WhichRange function is basically used to compare
the value of the sensor and the value of the running mode we selected
it returns an integer that we use to activates output in the next functions*/
int WhichRange(float temperature,MODE ipt){
	int to_return=0; //init. of return variable
	if (temperature<=ipt.min_val){
		to_return=1; // return one if sensed temp is under the min. limit
		}else if((temperature>ipt.min_val)&&(temperature<=ipt.max_val)){
		to_return=2;// return 2 if sensed temp is in range
		}else if (temperature>ipt.max_val){
		to_return=3;//return 3 if sensed temp is above max. limit
	}
	return to_return;
}

void heating(void){
	PIND = 0x01;//lights a diode used to simulate a radiator
}
void cooling(void){
	PORTD = 0x02;//enable a motor used to simulate a FAN system
}

/*Initializes the system.
Calls BoardInit() function to initialize the ports, and the ADC.
Initializes the running mode of temperature management system.
Displays a welcome message*/
void system_init(){
	BoardInit();
	IPT.max_val=20.0;
	IPT.min_val=10.0;
	LCD_String_xy(1,0,"WELCOME TC_SYS");
	_delay_ms(2000);
	LCD_Clear();
	
}

/*Basically a packaging of ADC value reading + conversion
it returns the float value of temperature which is used to do comparisons into
the principal function*/
float sensing(void){
	float celsius;
	celsius = (ADC_Read(0)*4.88);
	celsius = (celsius/10.00);
	return celsius;
}


/*Packaging of the display function that converts a float into a string
and displays the result to the LCD screen. */
void temperature_displaying(float temperature){
	char Temperature[10];
	LCD_String_xy(1,0,"Temperature");
	sprintf(Temperature,"%d%cC  ", (int)temperature, degree_sysmbol);
	LCD_String_xy(2,0,Temperature);
	memset(Temperature,0,10);
	
}

/*This is the temperature control function, which check the output of
WhichRange() function. Depending on the return value, it enalble or not an output*/
void temperature_control(float temperature, MODE ipt){
	int WR_result=WhichRange(temperature,ipt);
	if (WR_result==1){
		heating(); //Temp is to cold
		}else if (WR_result==2){
		PORTD = 0x00;//Temp. is in range
		}else if (WR_result==3){
		cooling();//Temp. is to hot
	}
}



