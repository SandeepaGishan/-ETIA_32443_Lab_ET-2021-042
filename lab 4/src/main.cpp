#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    uint8_t initial_capacity = 10;
    uint8_t vehicles = 0;
    uint8_t available;

    uint8_t previous_state = 0;


    // LED pins PB0 PB1 PB2 as output
    DDRB |= (1<<PB0)|(1<<PB1)|(1<<PB2);


    // IR sensor PD2 input
    DDRD &= ~(1<<PD2);


    // Enable pull-up for IR input
    PORTD |= (1<<PD2);


    // Button PD3 input
    DDRD &= ~(1<<PD3);

    // Enable internal pull-up
    PORTD |= (1<<PD3);



    while(1)
    {

        uint8_t sensor_state;
        sensor_state = (PIND & (1<<PD2));


        // Vehicle detection using state change
        if(sensor_state && !previous_state)
        {
            if(vehicles < initial_capacity)
            {
                vehicles++;
            }

            _delay_ms(300);
        }


        previous_state = sensor_state;


        // Reset button pressed
        if(!(PIND & (1<<PD3)))
        {
            vehicles = 0;
            _delay_ms(300);
        }



        available = initial_capacity - vehicles;



        // Turn OFF all LEDs

        PORTB &= ~(1<<PB0);
        PORTB &= ~(1<<PB1);
        PORTB &= ~(1<<PB2);



        // LED indication

        if(available > initial_capacity/2)
        {
            // Green ON
            PORTB |= (1<<PB0);
        }


        else if(available > 0)
        {
            // Yellow ON
            PORTB |= (1<<PB1);
        }


        else
        {
            // Red ON
            PORTB |= (1<<PB2);
        }


    }

}