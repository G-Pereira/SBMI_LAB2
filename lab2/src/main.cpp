#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

#define GREEN PB1
#define RED PB0

int state = 0;
float timerCount = 0;


ISR(INT0_vect) {

}


int main() {
    TCCR1B |= (1 << CS10);

    DDRB |= (1 << GREEN);
    DDRB |= (1 << RED);

    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);

    EICRA |= (1 << ISC01);
    EIMSK |= (1 << INT0);

    sei();

    while (1) {
        if (16000 <= TCNT1) {
            timerCount += 0.01;
            TCNT1 = 0; // Reset timer value
        }

        if (state == 0 && (!(PIND & (1 << PD2)))) {
            timerCount=0;
            state = 1;
        } else if (state == 1 && timerCount >= 15) {
            state=2;
            timerCount=0;
            else if(state ==1 && (!(PIND & (1 << PD2)))){
                state = 0;
                timerCount = 0;
            }
        } else if (state == 2 && (!(PIND & (1 << PD2)))) {
            state = 0;
            timerCount = 0;
        }
        if(state == 0){
            PORTB &= ~(1<<GREEN);
            PORTB &= ~(1<<RED);
        } else if (state == 1) {
            PORTB |= (1 << GREEN);
            PORTB &= ~(1 << RED);
        } else if (state == 2) {
            PORTB |= (1 << GREEN);
            PORTB |= (1 << RED);
        }
    }
}