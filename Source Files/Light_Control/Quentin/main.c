#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void lights(int SC_STATE)
{
	float DAY_NIGHT_LIMIT = 1; //to define
	if(SC_STATE == 4 && (255-ADCH)*255 < DAY_NIGHT_LIMIT)
	PORTB |= (1<<PB0);
	else
	PORTB &= ~(1<<PB0);
}

void ADC_init() {
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescalar to 128 - 125KHz sample rate @ 16MHz
	ADMUX |= (1 << REFS0); // Set ADC reference to AVCC
	ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading
	// No MUX values needed to be changed to use ADC0
	ADCSRA |= (1 << ADFR);  // Set ADC to Free-Running Mode
	ADCSRA |= (1 << ADEN);  // Enable ADC
	ADCSRA |= (1 << ADSC);  // Start A2D Conversions
}

int main(void)
{
	DDRF = 0x00;
	DDRB = 0x01;
	ADC_init();
	
	int SC_STATE = 4; // value from the security module when the admin is present
	while (1) {
		if(1<<ADCSRA)
		lights(SC_STATE);
		_delay_ms(10);
	}
}


