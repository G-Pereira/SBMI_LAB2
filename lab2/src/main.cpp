#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "printf_tools.h"

#define RED PB0
#define GREEN PB1
#define BUTTON PB2

#define COUNT (256-250)

#ifndef F_CPU
#define F_CPU 16000000ul
#endif
#define BAUD 57600
#define UBBR_VAL ((F_CPU/(BAUD*16))-1)

uint8_t state = 0;
uint16_t volatile timer = 0;

uint8_t EEMEM signature = 0b11110100;
uint16_t EEMEM best = UINT16_MAX;

ISR(TIMER2_OVF_vect) {
    TCNT2 = COUNT;
    if (timer)
        timer--;
}

int main() {
    TCCR2A = 0;
    TCCR2B = 0;
    TCCR2B |= (1 << CS22);
    TIMSK2 |= (1 << TOIE2);
    TCNT2 = COUNT;

    DDRB |= (1 << GREEN);
    DDRB |= (1 << RED);

    DDRB &= ~(1 << BUTTON);
    PORTB |= (1 << BUTTON);

    UBRR0 = UBBR_VAL;
    UCSR0B = (1 << RXEN0 | 1 << TXEN0);
    UCSR0C = (1 << USBS0 | 3 << UCSZ00);

    sei();

    if (eeprom_read_byte(&signature) != 0b11110100) {
        eeprom_write_byte(&signature, 0b11110100);
        eeprom_write_word(&best, UINT16_MAX);
    }

    uint8_t attemptN = 0;
    uint16_t currentAttempt;
    uint64_t sum = 0;
    uint16_t average;

    init_printf_tools();

    while (1) {
        if (0 == state && (!(PINB & (1 << BUTTON)))) {
            srand(TCNT2);
            timer = (uint16_t) rand() % 5000 + 5000;
            state = 1;
        } else if (1 == state && 0 == timer) {
            state = 2;
            timer = UINT16_MAX;
        } else if (2 == state && (!(PINB & (1 << BUTTON)) || (0 == timer))) {
            // value to store UINT16_MAX - timer
            currentAttempt = UINT16_MAX - timer;
            printf("Attempt number %u= %ums", attemptN + 1, currentAttempt);
            sum += currentAttempt;
            attemptN++;
            state = 3;
        } else if (3 == state && (PINB & (1 << BUTTON))) {
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
        } else if (3 == state) {
            PORTB &= ~(1 << GREEN);
            PORTB &= ~(1 << RED);
        }

        if (5 == attemptN) {
            average = sum / 5;
            printf("Score: %u\n", average);
            if (average < eeprom_read_word(&best)) {
                printf("New High Score! %ums\n", average);
                eeprom_update_word(&best, average);
            }
            attemptN = 0;
            sum = 0;
        }
    }

}