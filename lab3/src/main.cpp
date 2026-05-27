#include <Arduino.h>

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* ── LED Bitmasks (PORTB) ─────────────────────────────────────── */
#define PED_GREEN   (1 << PB0)
#define PED_RED     (1 << PB1)
#define ROAD_RED    (1 << PB2)
#define ROAD_YELLOW (1 << PB3)
#define ROAD_GREEN  (1 << PB4)
#define ALL_LEDS    (PED_GREEN | PED_RED | ROAD_RED | ROAD_YELLOW | ROAD_GREEN)

/* ── Button Bitmasks (PORTD) ──────────────────────────────────── */
#define BTN_EMERGENCY   (1 << PD2)
#define BTN_PEDESTRIAN  (1 << PD3)
#define BTN_MAINTENANCE (1 << PD1)

/* ── Volatile flag variables set ONLY inside ISRs ─────────────── */
volatile uint8_t flag_emergency    = 0;
volatile uint8_t flag_pedestrian   = 0;
volatile uint8_t flag_maintenance  = 0;

/* ── Helper: turn off all LEDs ────────────────────────────────── */
static inline void leds_off(void)
{
    PORTB &= ~ALL_LEDS;
}

/* ── Helper: delay in 100 ms ticks, checking flags every tick ─── */
static void delay_seconds_interruptible(uint8_t seconds)
{
    uint16_t ticks = (uint16_t)seconds * 10;   /* 10 × 100 ms = 1 s */
    for (uint16_t i = 0; i < ticks; i++) {
        _delay_ms(100);
        /* Bail early if a higher-priority flag arrives */
        if (flag_emergency || flag_pedestrian || flag_maintenance) return;
    }
}

/* ── ISR: Emergency (INT0, falling edge on PD2) ───────────────── */
ISR(INT0_vect)
{
    flag_emergency = 1;
}

/* ── ISR: Pedestrian (INT1, falling edge on PD3) ─────────────── */
ISR(INT1_vect)
{
    if (!flag_emergency)          /* respect higher priority */
        flag_pedestrian = 1;
}

/* ── ISR: Maintenance (PCINT2 vector for PORTD) ──────────────── */
ISR(PCINT2_vect)
{
    /* Trigger only on falling edge of PD1 (button press = LOW) */
    if (!(PIND & BTN_MAINTENANCE)) {
        if (!flag_emergency && !flag_pedestrian)
            flag_maintenance ^= 1;   /* toggle: press=enter, press again=exit */
    }
}

/* ── Handle Emergency Mode ────────────────────────────────────── */
static void handle_emergency(void)
{
    flag_emergency   = 0;
    flag_pedestrian  = 0;    /* clear lower-priority flags */

    leds_off();
    PORTB |= PED_RED;        /* Pedestrian RED ON  */
    PORTB |= ROAD_GREEN;     /* Road GREEN ON      */

    /* Hold for 10 s; emergency cannot be pre-empted */
    for (uint8_t i = 0; i < 100; i++) _delay_ms(100);

    leds_off();
}

/* ── Handle Pedestrian Crossing ───────────────────────────────── */
static void handle_pedestrian(void)
{
    flag_pedestrian = 0;

    /* Phase 1: Yellow ON alongside whatever road+ped LEDs are
     * currently ON — do NOT call leds_off() here              */
    PORTB |= ROAD_YELLOW;
    delay_seconds_interruptible(5);

    if (flag_emergency) { leds_off(); return; }

    /* Phase 2: Road GREEN off, Road RED on, Ped GREEN on for 10 s */
    PORTB &= ~ROAD_GREEN;   /* turn off green explicitly */
    PORTB &= ~PED_RED;      /* turn off pedestrian red   */
    PORTB |=  ROAD_RED;     /* road red ON               */
    PORTB |=  PED_GREEN;    /* pedestrian green ON       */
    /* Yellow stays on during transition per spec */
    delay_seconds_interruptible(10);

    leds_off();
}

/* ── Handle Maintenance / Warning Mode ───────────────────────── */
static void handle_maintenance(void)
{
    leds_off();

    /* Blink yellow indefinitely until:
     *   - button pressed again (flag_maintenance toggled to 0), OR
     *   - emergency overrides                                        */
    while (flag_maintenance) {
        if (flag_emergency) { leds_off(); return; }
        PORTB |= ROAD_YELLOW;
        _delay_ms(500);
        PORTB &= ~ROAD_YELLOW;
        _delay_ms(500);
    }

    leds_off();
}

/* ── Peripheral Initialisation ────────────────────────────────── */
static void init(void)
{
    /* PORTB: all LED pins as outputs, initially OFF */
    DDRB  |=  ALL_LEDS;
    PORTB &= ~ALL_LEDS;

    /* PORTD: PD2, PD3, PD1 as inputs with internal pull-ups */
    DDRD  &= ~(BTN_EMERGENCY | BTN_PEDESTRIAN | BTN_MAINTENANCE);
    PORTD |=  (BTN_EMERGENCY | BTN_PEDESTRIAN | BTN_MAINTENANCE);

    /* INT0 (PD2): falling-edge trigger */
    EICRA |= (1 << ISC01);
    EICRA &= ~(1 << ISC00);
    EIMSK |= (1 << INT0);

    /* INT1 (PD3): falling-edge trigger */
    EICRA |= (1 << ISC11);
    EICRA &= ~(1 << ISC10);
    EIMSK |= (1 << INT1);

    /* PCINT for PORTD: enable PCINT2 group, mask PD1 (PCINT17) */
    PCICR  |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT17);

    sei();   /* global interrupt enable */
}

/* ── Main ─────────────────────────────────────────────────────── */
int main(void)
{
    init();

    while (1) {
        /* ── Check flags in priority order ── */
        if (flag_emergency) {
            handle_emergency();
            continue;
        }
        if (flag_pedestrian) {
            handle_pedestrian();
            continue;
        }
        if (flag_maintenance) {
            handle_maintenance();
            continue;
        }

        /* ── Normal Operation: Road GREEN + Pedestrian RED (5 s) ── */
        leds_off();
        PORTB |= ROAD_GREEN;
        PORTB |= PED_RED;
        delay_seconds_interruptible(5);

        if (flag_emergency || flag_pedestrian || flag_maintenance) continue;

        /* ── Normal Operation: Road RED + Pedestrian GREEN (5 s) ── */
        leds_off();
        PORTB |= ROAD_RED;
        PORTB |= PED_GREEN;
        delay_seconds_interruptible(5);
    }

    return 0;
}