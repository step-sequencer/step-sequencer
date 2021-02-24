#ifndef STEPSEQUENCER_GLOBALS_H /* Include guard */
#define STEPSEQUENCER_GLOBALS_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

// BPM Settings (max. 600)
#define MIN_TEMPO_BPM 80
#define MAX_TEMPO_BPM 600

// Note Range Settings (min: 0, max: 127)
#define MIN_NOTE_NUMBER 50
#define MAX_NOTE_NUMBER 110
#define NOTE_NUMBER_RANGE (MAX_NOTE_NUMBER - MIN_NOTE_NUMBER)

// Button settings
#define BUTTON_REGISTER DDRB
#define BUTTON_PIN PINB
#define BUTTON_PORT PORTB
#define BUTTON_START_STOP PB0
#define BUTTON_1 PB1
#define BUTTON_2 PB2
#define BUTTON_3 PB3
#define BUTTON_4 PB4

// LED settings
#define LED_REGISTER DDRD
#define LED_PORT PORTD
#define LED_START_STOP PD2
#define LED_1 PD3
#define LED_2 PD4
#define LED_3 PD5
#define LED_4 PD6

// Potentiometer settings
#define POTI_FREQUENCY PC0
#define POTI_VELOCITY PC1
#define POTI_NOTE_LENGTH PC2
#define POTI_TEMPO PC3

// Suart MIDI Library settings / DIN jack configuration
#define SUART_PIN PB5
#define SUART_PORT PORTB
#define SUART_DDR DDRB

#endif /* STEPSEQUENCER_GLOBALS_H */