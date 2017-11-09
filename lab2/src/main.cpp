#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#define GREEN PB1
#define RED PB0
#define BUTTON PD2

#define COUNT (256-250)

uint8_t volatile state = 0;
uint8_t timer = 0;

ISR(TIMER2_OVF_vect) {
    TCNT2 = COUNT;
    if (timer)
        timer--;
}

int main() {
    TCCR2B |= (1 << CS22);

    DDRB |= (1 << GREEN);
    DDRB |= (1 << RED);

    DDRD &= ~(1 << BUTTON);
    PORTD |= (1 << BUTTON);

    EICRA |= (1 << ISC01);
    EIMSK |= (1 << INT0);

    sei();

    while (1) {

        if (state == 0 && (!(PIND & (1 << PD2)))) {
            srand(TCNT2);
            timer = (uint8_t) rand() % 5000 + 5000;
            state = 1;
        } else if (state == 1 && timer == 0) {
            state = 2;
        } else if (state == 2 && (!(PIND & (1 << PD2)))) {
            state = 0;
        }
        if (state == 0) {
            PORTB &= ~(1 << GREEN);
            PORTB &= ~(1 << RED);
        } else if (state == 1) {
            PORTB |= (1 << GREEN);
            PORTB &= ~(1 << RED);
        } else if (state == 2) {
            PORTB |= (1 << GREEN);
            PORTB |= (1 << RED);
        }
    }

}