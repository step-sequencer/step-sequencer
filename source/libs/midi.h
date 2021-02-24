#ifndef _MIDI_H_
#define _MIDI_H_

// Struct has all parameters that are necessary to send a MIDI message
typedef struct
{
    // Midi note number (0 - 127)
    uint8_t number;
    // Midi note velocity (0 - 127)
    uint8_t velocity;
    // After *length* time ticks, note_off has to be sent
    uint8_t length;
} MidiNote;

// Enum has standard byte values for MIDI message types
typedef enum
{
    NOTE_ON = 0b10010000,
    NOTE_OFF = 0b10000000,
} MidiMessageType;

// ENUM has default values for MIDI channels
typedef enum
{
    CH1 = 0, // Other enum items will be assigned to the corresponding number automatically
    CH2,
    CH3,
    CH4,
    CH5,
    CH6,
    CH7,
    CH8,
    CH9,
    CH10,
    CH11,
    CH12,
    CH13,
    CH14,
    CH15,
    CH16,
} MidiChannel;

void init_midi(MidiChannel ch);
void send_midi_note_on(MidiNote *note);
void send_midi_note_off(MidiNote *note);
void set_midi_channel(MidiChannel ch);

#endif /* _MIDI_H_ */
