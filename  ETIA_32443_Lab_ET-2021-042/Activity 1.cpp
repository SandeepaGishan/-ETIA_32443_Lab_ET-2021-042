#define F CPU 16000000UL 
#include <avr/io.h>
#include <util/delay.h>
int main (void)  {
  DDRB |=(1 << DDB0) ;
  DDRB &= ~(0xFE) ;
	
  while (1) {
    PORTB |= (1 << PORTB0);
    _delay_ms (1000);
     PORTB &= ~(1 << PORTB0);
    _delay_ms (1000);
    
  }}