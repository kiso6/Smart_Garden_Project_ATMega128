#define F_CPU 8000000UL
#include <avr/io.h>

#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

#define  Trigger_pin	PA0	/* Trigger pin */


int TimerOverflow = 0;
volatile unsigned int SC_state = 0;



void SC_buzzer();




//Hard-Coded 7-bit Sequence


//with top button, state goes to 0
void SC_out()
{
	if((PINF&0x80)==0x80) 
	{
		SC_state = 0;
		
		SC_buzzer();
		SC_buzzer();
		SC_buzzer();
	}
	
}


void SC_intruder()
{
	//Buzzer go brr, no escape
	if(SC_state==4);
	else
	{
		SC_buzzer();
		SC_buzzer();
		SC_buzzer();
	}
	
	
	//LED go brr
}

//connected to Ultrasonic sensor, make noises with input 



//make random noises
void SC_buzzer()
{
	PORTB = 0b00000001;
	_delay_us(1908*4);
	PORTB = 0b00000000;
	_delay_us(1908*4);
}

ISR(TIMER1_OVF_vect)
{
	TimerOverflow++;	/* Increment Timer Overflow count */
}

//Sequence of button inputs made in interrupts
ISR(INT4_vect)
{
	//SC_buzzer();
	if(SC_state == 0) 
	{
		SC_buzzer();
		SC_state = 1;
	}
	else if(SC_state==4);
	else SC_state = 0;
	
}

ISR(INT5_vect)
{
	//SC_buzzer();
	if(SC_state == 1)
	{
		SC_buzzer();
		 SC_state = 2;
	}
	else if(SC_state==4);
	else SC_state = 0;
	
}

ISR(INT6_vect)
{
	//SC_buzzer();	
	if(SC_state == 2) 
	{
		SC_buzzer();
		SC_state = 3;
	}
	else if(SC_state==4);
	else SC_state = 0;
	
}

ISR(INT7_vect)
{
	
	//SC_buzzer();
	if(SC_state == 3) 
	{
		SC_state = 4;
		
		SC_buzzer();
		SC_buzzer();
		SC_buzzer();
	}
	else if(SC_state==4);
	else SC_state = 0;
}


int main(void)
{
	
	
	//initialization for sonic sensor
	long count;
	double distance = 9999;
	DDRA = 0x01;			/* Make trigger pin as output */
	PORTD = 0xFF;				/* Turn on Pull-up */
	
	//PORT E & F to get button inputs
	//E for interrupt inputs, F for ordinary reset input
	DDRE = 0x00;
	DDRF = 0x00;
	
	//Initialize stuff to enable INT4~7
	PORTE = 0b11111111; // pull up resistance
	EICRA = 0b10000000; // interrupt trigger set
	EIMSK = 0b11111111; // allow interrupts
	
	SREG |= 0x80; //enable interrupt
	
	//PORT B0 to make buzzer brr, (an output)
	DDRB = 0b00000001;
	
	
	
	
	sei();						/* Enable global interrupt */
	TIMSK = (1 << TOIE1);	/* Enable Timer1 overflow interrupts */
	TCCR1A = 0;				/* Set all bit to zero Normal operation */
	
	
	SC_state = 0;
	while (1)
	{
		//Security Check
		if(SC_state!=4)
		{
			//try to get code
			//SC_getCode();
			//////////////////////////////////////////////////////////////////////////
			
			PORTA |= (1 << Trigger_pin);/* Give 10us trigger pulse on trig. pin to HC-SR04 */
			_delay_us(10);
			PORTA &= (~(1 << Trigger_pin));
			
			TCNT1 = 0;			/* Clear Timer counter */
			TCCR1B = 0x41;		/* Setting for capture rising edge, No pre-scaler*/
			TIFR = 1<<ICF1;		/* Clear ICP flag (Input Capture flag) */
			TIFR = 1<<TOV1;		/* Clear Timer Overflow flag */

			/*Calculate width of Echo by Input Capture (ICP) on PortD PD6 */
			
			while ((TIFR & (1 << ICF1)) == 0);	/* Wait for rising edge */
			TCNT1 = 0;			/* Clear Timer counter */
			TCCR1B = 0x01;		/* Setting for capture falling edge, No pre-scaler */
			TIFR = 1<<ICF1;		/* Clear ICP flag (Input Capture flag) */
			TIFR = 1<<TOV1;		/* Clear Timer Overflow flag */
			TimerOverflow = 0;	/* Clear Timer overflow count */

			while ((TIFR & (1 << ICF1)) == 0); /* Wait for falling edge */
			count = ICR1 + (65535 * TimerOverflow);	/* Take value of capture register */
			/* 8MHz Timer freq, sound speed =343 m/s, calculation mentioned in doc. */
			distance = (double)count / 466.47;
			
			//about 7-80
			if(distance<10) SC_intruder();
			_delay_ms(200);
			
			//////////////////////////////////////////////////////////////////////////
			//waits for button input

		}
		else
		{
			//wait for out signal
			SC_out();
			
			//Killian's selectors might be placed here
			
			
			
			
			
				
		}
		
	}
}