#include <avr/io.h>
#include <avr/interrupt.h>
#include "../globals.h"
#include "suart_midi.h"

// Global variable which is used to store the current message to be sent including wrapper bits
volatile uint16_t outframe;

// Initialize TIMER0 settings to send MIDI messages
void suart_midi_init()
{
    cli();
    // Activate Clear Timer on Compare Mode for TIMER0
    TCCR0A |= (1 << WGM01);

    // To match the Midi baud rate we we use a compare value of 63 and set the prescaler to 8.
    OCR0A = 63;
    TCCR0B |= (1 << CS01);

    // Set SUART pin as output pin and set output pin high
    SUART_DDR |= (1 << SUART_PIN);
    SUART_PORT |= (1 << SUART_PIN);

    // Initialize outframe which is later used to store the message to be sent
    outframe = 0;
}

// Sends a one byte MIDI message
void suart_put_byte(const uint8_t dataByte)
{
    // Loop while timer is busy sending another message. If timer is not busy, outframe is 0.
    do
    {
    } while (outframe);
    // Set outframe to the message to be sent.
    // outframe structure: 11nnnnnnnn0 (n: actual bits to be sent, surrounded by helper bits) more information in documentation.
    outframe = (3 << 9) | (dataByte << 1);

    // Reset current counting value of timer to 0
    TCNT0 = 0;
    // Activate TIMER0 to start sending the message
    TIMSK0 |= (1 << OCIE0A);
    sei();
}

// TIMER0 Interrupt Service Routine sends the single bits of a MIDI message which are part of the variable outframe
// It always sends the least significant bit and then shifts outframe to the right. Therefore on the next interrupt it will send the next bit.
ISR(TIMER0_COMPA_vect)
{
    uint16_t data = outframe;

    // If least significant bit is 1, set port high
    if (data & 1)
    {
        SUART_PORT |= (1 << SUART_PIN);
    }
    // else least significant bit is 0 and we set port to low
    else
    {
        SUART_PORT &= ~(1 << SUART_PIN);
    }
    // if data to be sent  is 1, all bits have been sent and we can deactivate the timer.
    if (1 == data)
    {
        TIMSK0 &= ~(1 << OCIE0A);
    }
    // shift outframe to  the right as we have sent the least significant bit and can send the next one.
    //In case all data bytes have been sent, outframe still needs to be shifted to free the possible blocking do-while loop in suart_put_byte()
    outframe = data >> 1;
}
