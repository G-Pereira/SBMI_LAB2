#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#define RED PB0
#define GREEN PB1
#define BUTTON PB2

#define COUNT (256-250)

uint8_t state = 0;
uint16_t volatile timer = 0;

ISR(TIMER2_OVF_vect) {
    TCNT2 = COUNT;
    if (timer)
        timer--;
}

int main() {
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = COUNT;

    DDRB |= (1 << GREEN);
    DDRB |= (1 << RED);

    DDRB &= ~(1 << BUTTON);
    PORTB |= (1 << BUTTON);

    TIMSK2 |= (1 << TOIE2);

    sei();
    TCCR2B |= (1 << CS22);

    while (1) {
        if (0 == state && (!(PINB & (1 << BUTTON)))) {
            srand(TCNT2);
            timer = (uint16_t) rand() % 5000 + 5000;
            state = 1;
        } else if (1 == state && 0 == timer) {
            state = 2;
            timer = UINT16_MAX;
        } else if (2 == state && ((!(PINB & (1 << BUTTON)))) || (0 == timer)) {
            state = 0;
        }
        if (0 == state) {
            PORTB &= ~(1 << GREEN);
            PORTB &= ~(1 << RED);
        } else if (1 == state) {
            PORTB |= (1 << GREEN);
            PORTB &= ~(1 << RED);
        } else if (2 == state) {
            PORTB |= (1 << GREEN);
            PORTB |= (1 << RED);
        }
    }

}