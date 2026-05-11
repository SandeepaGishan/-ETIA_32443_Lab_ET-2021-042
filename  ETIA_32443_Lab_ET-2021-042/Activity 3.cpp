  #define F_CPU 16000000UL
 #include <avr/io.h>
 
 int main (void){
  
    DDRB  |= (1 << DDB0);
    DDRB  &= ~(1 << DDD2);

    while (1)
    {
      if (PIND & ( 1 << PIND2)){
        PORTB |= (1 << PORTB0);
       } else {     
    PORTB &= ~(1 << PORTB0); } 
    }}