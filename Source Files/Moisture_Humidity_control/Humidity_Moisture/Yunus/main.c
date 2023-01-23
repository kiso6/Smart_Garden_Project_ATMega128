/*
 * final_project_watering_system.c
 *
 * Created: 09.05.2022 15:36:52
 * Author : Yunus
 */ 


#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU / 16 / BAUD - 1

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


unsigned int msec, sec,min,hour;
int flag1 = 1;


void USART_Init(unsigned int ubrr){
	UBRR0H = (unsigned char) (ubrr >> 8);
	UBRR0L = (unsigned char) ubrr;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (3 << UCSZ0);
}

void USART_TX(unsigned char data){
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

unsigned char USART_RX(){
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

void USART_Transmit(char data)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}


void USART_TX_String(char *text){
	while(*text != 0)
	USART_TX(*text++);
}
void Timer_Init()
{
	TCCR0 = (4 << CS0);
	TIMSK = (1 << TOIE0);
	TCNT0 = 6;
}

void USART_Transmit_String(char *str)
{
	while (*str != '\0')
	USART_Transmit(*str++);
}

ISR(TIMER0_OVF_vect)
{
	if(flag1 == 1)
	{
		msec++;
		if(msec == 1000)
		{
			msec = 0;
			sec++;
		}
		if(sec == 60){
		sec = 0;
		min++;
	    }
		if(min == 60)
		min = 0;
		hour++; 
		if(hour == 24)
		hour = 0;
		
		TCNT0 = 6;
	}
}
void waterme(int _humidity){
	
	if(_humidity <30){
		USART_Transmit_String("Time \r\n");
		USART_Transmit((sec+1)/10+48);
		USART_Transmit((sec+1)%10+48);
		USART_Transmit('\r');
		USART_Transmit_String("Please Water now its urgent \r\n");
	}else if(_humidity < 40){
		USART_Transmit_String("Time \r\n");
		USART_Transmit((sec+1)/10+48);
		USART_Transmit((sec+1)%10+48);
		USART_Transmit('\r');
		USART_Transmit_String("You should really consider watering \r\n");
	}else if(_humidity < 50){
	USART_Transmit_String("Time \r\n");
	USART_Transmit((sec+1)/10+48);
	USART_Transmit((sec+1)%10+48);
	USART_Transmit('\r');
	USART_Transmit_String("You can consider watering \r\n");
	}else if(_humidity < 60){
	USART_Transmit_String("Time \r\n");
	USART_Transmit((sec+1)/10+48);
	USART_Transmit((sec+1)%10+48);
	USART_Transmit('\r');
	USART_Transmit_String("You can water, but no need \r\n");
	}else if(_humidity < 70){
	USART_Transmit_String("Time \r\n");
	USART_Transmit((sec+1)/10+48);
	USART_Transmit((sec+1)%10+48);
	USART_Transmit('\r');
	USART_Transmit_String("Do not Water ! \r\n");
    }
}
	
	

int main(void)
{
	DDRB = 0xFF;
	DDRD = 0x00;
	
	PORTB = 0x01;  //LED 1  on
	USART_Init(MYUBRR);
	Timer_Init();
	SREG = 0x80;
    /* Replace with your application code */
	USART_Transmit_String("Welcome to the Watersystem \r\n");
	int a = 0;
	int humidity = 50;
    while (1) 
    {
		a = sec+1; // actually hour not sec 
		a = a%10;
	
		if(a == 0){
		 waterme(humidity);
		}
		_delay_ms(800);
    }
}

