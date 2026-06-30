#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
//----------------------------
// ADC Initialize
//----------------------------
void ADC_init(void)
{
 // AVCC reference, ADC0 start
 ADMUX = (1 << REFS0);
 // Enable ADC + prescaler 128
 ADCSRA = (1 << ADEN) |
 (1 << ADPS2) |
 (1 << ADPS1) |
 (1 << ADPS0);
 // Disable digital input buffers on ADC0-ADC2 (important for noise
reduction)
 DIDR0 = (1 << ADC0D) | (1 << ADC1D) | (1 << ADC2D);
}
//----------------------------
// Read ADC Channel (stable version)
//----------------------------
uint16_t ADC_read(uint8_t channel)
{
 channel &= 0x07;
 // Select channel but keep reference bits
 ADMUX = (ADMUX & 0xF8) | channel;
 // Small delay for channel settling
 _delay_us(10);
 // Start conversion
 ADCSRA |= (1 << ADSC);
 // Wait until conversion completes
 while (ADCSRA & (1 << ADSC));
 // Return ADC value
 return ADCW;
}
//----------------------------
// Main
//----------------------------
int main(void)
{
 uint16_t ldr, pot, gas;
 // Set PB0 and PB1 as output
 DDRB |= (1 << PB0) | (1 << PB1);
 PORTB &= ~((1 << PB0) | (1 << PB1));
 ADC_init();
 while (1)
 {
 // Read sensors
 ldr = ADC_read(0);
 pot = ADC_read(1);
 gas = ADC_read(2);
 //----------------------------
 // Adaptive Lighting Control
 //----------------------------
 if (ldr < pot)
 PORTB |= (1 << PB0);
 else
 PORTB &= ~(1 << PB0);
 //----------------------------
 // Gas Alert System
 //----------------------------
 if (gas > 600)
 PORTB |= (1 << PB1);
 else
 PORTB &= ~(1 << PB1);
 _delay_ms(100);
 }
}
