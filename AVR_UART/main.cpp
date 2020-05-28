/*
 * AVR_UART.cpp
 *
 * Created: 5/28/2020 7:22:30 PM
 * Author : MCLEANS
 */ 

#define F_CPU 1000000UL 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 256
char receive_buffer[BUFFER_SIZE];
uint8_t buffer_position = 0;

char querry[] = "AT";
char response[] = "OK\n";

bool end_line = false;


void set_baudrate(uint32_t baud){
	switch(F_CPU){
		case 1000000UL:
			switch(baud){
				case 9600:
					UBRR0L = 6;
					break;
				case 4800:
					UBRR0L = 12;
					break;
				default:
					break;
				}
		case 8000000UL:
			switch(baud){
				case 115200:
					UBRR0L = 3;
					break;
				case 9600:
					UBRR0L = 51;
					break;
				case 4800:
					UBRR0L = 207;
					break;
				default:
					break;
			}
		
		case 16000000UL:
			switch(baud){
				case 115200:
					 UBRR0L = 8;
					 break;
				case 9600:
					UBRR0L = 103;
					break;
				case 4800:
					UBRR0L = 207;
					break;
				default:
					break;
					
			}
		default:
			break;
			
	}
}

void init_UART(uint32_t baudrate){
	set_baudrate(baudrate);
	//Enable RECEIVER and TRANSMITER
	UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
	//Enable receiver complete Interrupt;
	UCSR0B |= (1<<RXCIE0);
	//set character size to 8 bits
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);
	//set Asynchronous mode
	UCSR0C &= ~(1<<UMSEL00);
	UCSR0C &= ~(1<<UMSEL01);
	//enable global Interrupt
	sei();
	
	}
	
void send_char(char data){
	while(!(UCSR0A & (1<<UDRE0))){}
	UDR0 = data;
}

void send_string(char *byte){
	for(;*byte;byte++) send_char(*byte);
}

char receive_char(void){
	char in_data;
	in_data = UDR0;
	return in_data;
	
}

void flush_buffer(){
	for(int i = 0; i < BUFFER_SIZE; i++){
		receive_buffer[i] = 0x00;
	}
	buffer_position = 0;
}

ISR (USART0_RX_vect){
	receive_buffer[buffer_position] = UDR0;

	if(receive_buffer[buffer_position] == '\n') {
	end_line = true;
	
}

	
	buffer_position++;
	
	if(buffer_position > BUFFER_SIZE) buffer_position = 0;
}


void listen(){
	if(end_line){

		if(strncmp(receive_buffer,querry,2) == 0){
		send_string(response);
		flush_buffer();
		}
		else{
			flush_buffer();
		}
		end_line = false;
	}

	

}

int main(void)
{
	init_UART(9600);
	
    
    while (1) 
    {
		listen();
    }
}

