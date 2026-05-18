#include <avr/io.h>
#include <util/delay.h>

void setup() { 
    DDRB |= (1 << DDB5);  
    DDRB |= (1 << DDB0);  
    DDRB |= (1 << DDB1);      
    DDRD &= ~(1 << DDD2);    
}

 // Set PD2 as INPUT 
    DDRD &= ~(1 << DDD2);

    void loop() {

    // -------- Status LED Blink --------
    PORTB |= (1 << PORTB5);   // LED ON
    _delay_ms(500);
    PORTB &= ~(1 << PORTB5);  // LED OFF
    _delay_ms(500);
     // -------- Read PIR Sensor --------
    if (PIND & (1 << PIND2)) {
       
        PORTB |= (1 << PORTB0);  // Alert LED ON
        PORTB |= (1 << PORTB1);  // Buzzer ON

        _delay_ms(5000);  // 5 seconds delay
         // Turn OFF
        PORTB &= ~(1 << PORTB0);
        PORTB &= ~(1 << PORTB1);
    }
}
int main(void) {
    setup();

    
    _delay_ms(30000);  

    while (1) {
        loop();
    }
}
