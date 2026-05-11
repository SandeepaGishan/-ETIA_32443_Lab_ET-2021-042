   #define F_CPU 16000000UL
 #include <avr/io.h>
 #include <util/delay.h>

 int main (void){
  
    DDRC = 0x0F;
    

    while (1)
    {
    PORTC = 0x0F;  
    _delay_ms(1000);

    PORTC = 0x0F;  
    _delay_ms(1000);
    
    }}