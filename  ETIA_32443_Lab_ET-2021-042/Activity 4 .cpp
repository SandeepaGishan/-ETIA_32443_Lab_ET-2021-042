  #define F_CPU 16000000UL
#include <avr/io.h>

int main(void)
{
    // PB0 as output
    DDRB |= (1 << PB0);

    // PD2 as input
    DDRD &= ~(1 << PD2);

    // Enable pull-up on PD2
    PORTD |= (1 << PD2);

    // -------- CASE 1 --------
    // PUD = 0  (pull-up enabled)
    MCUCR &= ~(1 << PUD);

    // -------- CASE 2 --------
    // Uncomment this line and comment above line
    // MCUCR |= (1 << PUD);

    while (1)
    {
        // Button pressed -> PD2 becomes LOW
        if (!(PIND & (1 << PD2)))
        {
            PORTB |= (1 << PB0);   // LED ON
        }
        else
        {
            PORTB &= ~(1 << PB0);  // LED OFF
        }
    }
}