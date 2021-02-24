// Includes
#include <avr/io.h>
#include <util/delay.h>
#include "suart_midi.h"
#include "midi.h"
#include "../globals.h"

void send_midi_note(MidiMessageType type, MidiNote *note);

MidiChannel channel;

// Method initializes the software UART library for sending MIDI messages and setting the MIDI channel
void init_midi(MidiChannel ch)
{
    channel = ch;
    suart_midi_init();
}

// Method sends a MIDI message with note on for the given note parameter
void send_midi_note_on(MidiNote *note)
{
    send_midi_note(NOTE_ON, note);
}

// Method sends a MIDI message with note off for the given note parameter
void send_midi_note_off(MidiNote *note)
{
    send_midi_note(NOTE_OFF, note);
}

// Method sets the MIDI channel
void set_midi_channel(MidiChannel ch)
{
    channel = ch;
}

// Internal method to send a MIDI message via the suart library
void send_midi_note(MidiMessageType type, MidiNote *note)
{
    if (type == NOTE_ON || type == NOTE_OFF)
    {
        suart_put_byte(type + channel);
        suart_put_byte((note->number % 128));
        suart_put_byte((note->velocity % 128));
    }
}